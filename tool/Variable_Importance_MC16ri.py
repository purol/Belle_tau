#!/usr/bin/env python3
import uproot
import pandas as pd
import os
import argparse
from fnmatch import fnmatchcase
from typing import List, Optional, Union
from concurrent.futures import ThreadPoolExecutor
import tqdm
import sys
import numpy as np
from scipy.stats import spearmanr
from scipy.spatial.distance import pdist, squareform
import matplotlib.pyplot as plt
import seaborn as sns

# Edit this tuple to change the variables used as the dCor target.
DCOR_TARGET_COLUMNS = ("M", "deltaE")

# MC16ri 4S scale factors from analysis_code/include/constants.h.
LUMINOSITY_MC16RI_4S = 0.49841  # ab-1

# MyEventType -> generated equivalent luminosity (ab-1).
BACKGROUND_LUMINOSITY_MC16RI_4S = {
    1: 1.1,  # CHG
    2: 1.1,  # MIX
    3: 1.1 + 21.8,  # UUBAR: nominal + generator cut
    4: 1.1 + 21.8,  # DDBAR: nominal + generator cut
    5: 1.1 + 21.8,  # SSBAR: nominal + generator cut
    6: 1.1 + 21.8,  # CHARM / CCBAR: nominal + generator cut
    7: 1.1,  # MUMU
    8: 0.175,  # EE
    9: 1.1,  # EEEE
    10: 1.1,  # EEMUMU
    16: 1.35,  # GG
    22: 1.1 + 14.5,  # TAUPAIR: nominal + generator cut
    34: 1.1,  # hhISR / HHISR
    35: 1.1,  # llXX / LLXX
}

BR_SIGNAL = 1e-8
TAU_CROSSSECTION_4S = 0.919  # nb
SIGNAL_MC16RI_EVENTS = 11000000


def signal_scale_mc16ri():
    tau_pairs = LUMINOSITY_MC16RI_4S / 1e-9 * TAU_CROSSSECTION_4S
    return tau_pairs * BR_SIGNAL * 2.0 / SIGNAL_MC16RI_EVENTS


class RepeatedDistanceCorrelation:
    """Estimate full weighted dCor from repeated, bounded-size event samples.

    Events are sampled with replacement in proportion to their weights.
    Each batch uses unbiased U-statistic estimates of the three squared
    distance-covariance components. Pool components before taking the dCor
    ratio: increasing repeats then converges to full-data weighted dCor.
    Memory depends on max_events squared, but not on the full event count or
    the number of repeats.
    """

    def __init__(self, df, max_events, repeats, seed, workers=1):
        missing = [column for column in DCOR_TARGET_COLUMNS if column not in df.columns]
        if missing:
            raise ValueError(f"Missing dCor target columns: {missing}")
        target_columns = list(DCOR_TARGET_COLUMNS)
        valid = np.isfinite(df[target_columns + ["weight"]].to_numpy(dtype=float)).all(axis=1)
        valid &= df["weight"].to_numpy(dtype=float) > 0
        self.sample = df.loc[valid]
        if len(self.sample) < 5:
            raise ValueError("Distance correlation needs at least five positive-weight events.")

        weights = self.sample["weight"].to_numpy(dtype=float, copy=True)
        weights /= weights.sum()
        self.weights = weights
        self.target = self._standardize(self.sample[target_columns].to_numpy(dtype=float))
        if self.target is None:
            raise ValueError(f"A dCor target column has no variation: {target_columns}")

        self.max_events = max_events
        self.repeats = repeats
        self.seed = seed
        self.workers = min(workers, repeats)
        self.cache = {}
        print(f"Configured distance correlation: {max_events} events per draw, {repeats} draws, {self.workers} worker(s), {len(self.sample)} valid events")

    def _standardize(self, values):
        if not np.isfinite(values).all():
            return None
        means = np.average(values, axis=0, weights=self.weights)
        scales = np.sqrt(np.average((values - means) ** 2, axis=0, weights=self.weights))
        if np.any(scales <= 0):
            return None
        return (values - means) / scales

    @staticmethod
    def _u_moment(a, b):
        """Unbiased estimate of the full empirical squared distance covariance."""
        n = len(a)
        a_rows = a.sum(axis=1)
        b_rows = b.sum(axis=1)
        ab = np.einsum("ij,ij->", a, b)
        row_product = a_rows @ b_rows
        pair = ab / (n * (n - 1))
        triple = (row_product - ab) / (n * (n - 1) * (n - 2))
        quadruple = (a_rows.sum() * b_rows.sum() - 4 * row_product + 2 * ab) / (n * (n - 1) * (n - 2) * (n - 3))
        return float(pair + quadruple - 2 * triple)

    @staticmethod
    def _ratio(covariance, x_variance, y_variance):
        if x_variance <= 0 or y_variance <= 0:
            return np.nan
        return float(np.sqrt(np.clip(covariance / np.sqrt(x_variance * y_variance), 0, 1)))

    def _batch_moments(self, values, indices):
        # Both input arrays are read-only across workers; distance matrices are local.
        a = squareform(pdist(values[indices], metric="euclidean"))
        b = squareform(pdist(self.target[indices], metric="euclidean"))
        covariance = self._u_moment(a, b)
        x_variance = self._u_moment(a, a)
        y_variance = self._u_moment(b, b)
        return covariance, x_variance, y_variance

    def _iter_batch_moments(self, values):
        rng = np.random.default_rng(self.seed)

        def draw_indices():
            return rng.choice(len(self.sample), size=self.max_events, p=self.weights)

        if self.workers == 1:
            for _ in range(self.repeats):
                yield self._batch_moments(values, draw_indices())
            return

        # Submit at most one batch per worker, then collect in draw order.
        # This preserves the sequential RNG stream and bounds peak memory.
        with ThreadPoolExecutor(max_workers=self.workers) as executor:
            pending = [executor.submit(self._batch_moments, values, draw_indices())
                       for _ in range(self.workers)]
            for repeat in range(self.repeats):
                slot = repeat % self.workers
                moments = pending[slot].result()
                if repeat + self.workers < self.repeats:
                    pending[slot] = executor.submit(self._batch_moments, values, draw_indices())
                yield moments

    def stats(self, columns):
        key = tuple(columns)
        if key in self.cache:
            return self.cache[key]

        values = self._standardize(self.sample[list(key)].to_numpy(dtype=float))
        if values is None:
            result = {"estimate": np.nan, "batch_mean": np.nan, "batch_std": np.nan}
        else:
            moment_sum = np.zeros(3, dtype=float)
            batch_mean = 0.0
            batch_m2 = 0.0
            batch_valid = True
            for repeat, (covariance, x_variance, y_variance) in enumerate(self._iter_batch_moments(values)):
                moment_sum += (covariance, x_variance, y_variance)
                score = self._ratio(covariance, x_variance, y_variance)
                if np.isfinite(score):
                    delta = score - batch_mean
                    batch_mean += delta / (repeat + 1)
                    batch_m2 += delta * (score - batch_mean)
                else:
                    batch_valid = False

            # A mean of batch dCor values is not consistent for full-data dCor.
            # The ratio of pooled covariance/variance terms is consistent.
            pooled = moment_sum / self.repeats
            result = {
                "estimate": self._ratio(*pooled),
                "batch_mean": batch_mean if batch_valid else np.nan,
                "batch_std": float(np.sqrt(batch_m2 / (self.repeats - 1))) if batch_valid and self.repeats > 1 else (0.0 if batch_valid else np.nan),
            }
        self.cache[key] = result
        return result


def make_distance_evaluators(df, max_events, repeats, seed, workers):
    return {
        1: RepeatedDistanceCorrelation(df[df["label"] == 1], max_events, repeats, seed, workers),
        0: RepeatedDistanceCorrelation(df[df["label"] == 0], max_events, repeats, seed + 1, workers),
    }


def select_variables(summary_df, train_df, region_name, distance_evaluators, distance_threshold,
                     report_path=None, skipped_variables=None):
    """
    Selects variables based on separation, dCor with DCOR_TARGET_COLUMNS,
    and correlation with already selected variables.
    """
    print(f"\n--- Selecting variables for Region {region_name} ---")

    # Sort variables by separation in descending order
    sorted_summary = summary_df.sort_values(by="separation", ascending=False)

    # Separate signal and background dataframes
    signal_df = train_df[train_df["label"] == 1]
    bkg_df = train_df[train_df["label"] == 0]

    selected_variables = []
    selected_dcor = []
    report_lines = [
        f"Variable selection report - Region {region_name}",
        f"dCor target: {', '.join(DCOR_TARGET_COLUMNS)}",
        f"Rule: joint dCor must be < {distance_threshold:g} in both signal and background.",
        "Spearman rule: reject when |rho| > 0.5 in both classes for an already selected variable.",
        "Candidates are tested in descending separation order. dCor uses the selected set plus the candidate.",
        "",
    ]

    for rank, (_, row) in enumerate(sorted_summary.iterrows(), start=1):
        candidate_var = row["varname"]

        # Check the candidate AND all previously selected variables jointly.
        candidate_set = selected_variables + [candidate_var]
        signal_dcor = distance_evaluators[1].stats(candidate_set)
        bkg_dcor = distance_evaluators[0].stats(candidate_set)
        signal_estimate = signal_dcor["estimate"]
        bkg_estimate = bkg_dcor["estimate"]
        reasons = []
        # Decide with the pooled full-data estimate, not an extreme batch value.
        if signal_estimate < distance_threshold and bkg_estimate < distance_threshold:
            # 2. Check correlation with already selected variables
            for selected_var in selected_variables:
                # Spearman correlation
                bkg_spearman_corr = spearmanr(bkg_df[candidate_var], bkg_df[selected_var]).correlation
                signal_spearman_corr = spearmanr(signal_df[candidate_var], signal_df[selected_var]).correlation
                if (abs(bkg_spearman_corr) > 0.5 and abs(signal_spearman_corr) > 0.5):
                    reasons.append(
                        f"Spearman with selected variable {selected_var}: "
                        f"signal rho={signal_spearman_corr:.6g}, "
                        f"background rho={bkg_spearman_corr:.6g}; both |rho| > 0.5"
                    )
                    break  # The first blocking variable explains the rejection.

            if not reasons:
                selected_variables.append(candidate_var)
                selected_dcor.append({
                    "varname": candidate_var,
                    "joint_signal_dcor_estimate": signal_dcor["estimate"],
                    "joint_signal_dcor_batch_mean": signal_dcor["batch_mean"],
                    "joint_signal_dcor_batch_std": signal_dcor["batch_std"],
                    "joint_bkg_dcor_estimate": bkg_dcor["estimate"],
                    "joint_bkg_dcor_batch_mean": bkg_dcor["batch_mean"],
                    "joint_bkg_dcor_batch_std": bkg_dcor["batch_std"],
                })
        else:
            for class_name, estimate in (("signal", signal_estimate), ("background", bkg_estimate)):
                if not np.isfinite(estimate):
                    reasons.append(f"{class_name} joint dCor is non-finite ({estimate})")
                elif estimate >= distance_threshold:
                    reasons.append(
                        f"{class_name} joint dCor {estimate:.6g} >= {distance_threshold:g}"
                    )

        report_lines.extend([
            f"{rank}. {candidate_var} - {'REJECTED' if reasons else 'SELECTED'}",
            f"   separation: {row['separation']:.6g}",
            f"   tested set: {', '.join(candidate_set)}",
            f"   joint dCor: signal={signal_estimate:.6g}, background={bkg_estimate:.6g}",
            *[f"   reason: {reason}" for reason in reasons],
            "",
        ])

    if skipped_variables:
        report_lines.append("Not tested because separation could not be calculated:")
        report_lines.extend(f"- {name}: {reason}" for name, reason in skipped_variables)
        report_lines.append("")
    report_lines.append(
        f"Summary: {len(selected_variables)} selected, "
        f"{len(sorted_summary) - len(selected_variables)} rejected, "
        f"{len(skipped_variables or [])} not tested."
    )
    if report_path is not None:
        with open(report_path, "w", encoding="utf-8") as report_file:
            report_file.write("\n".join(report_lines) + "\n")
        print(f"Saved selection report to {report_path}")

    print(f"Selected {len(selected_variables)} variables for Region {region_name}:")
    print(selected_variables)
    return selected_variables, pd.DataFrame(selected_dcor, columns=[
        "varname", "joint_signal_dcor_estimate", "joint_signal_dcor_batch_mean",
        "joint_signal_dcor_batch_std", "joint_bkg_dcor_estimate",
        "joint_bkg_dcor_batch_mean", "joint_bkg_dcor_batch_std",
    ])

def calculate_weights(df: pd.DataFrame) -> pd.Series:
    """Match the MC16ri 4S MC_weight entries in MyObtainWeight.h."""
    sample_type = df["MySampleType"].to_numpy()
    event_type = df["MyEventType"].to_numpy()
    energy_type = df["MyEnergyType"].to_numpy()

    weights = np.zeros(len(df), dtype=np.float64)
    is_data = ((-1.5 < sample_type) & (sample_type < -0.5)) | (
        (4.5 < sample_type) & (sample_type < 5.5)
    )
    weights[is_data] = 1.0

    is_mc16ri = (2.5 < sample_type) & (sample_type < 3.5)
    is_4s = (0.5 < energy_type) & (energy_type < 1.5)
    matched = is_data.copy()

    signal_mask = is_mc16ri & is_4s & (-0.5 < event_type) & (event_type < 0.5)
    weights[signal_mask] = signal_scale_mc16ri()
    matched |= signal_mask

    for event, generated_luminosity in BACKGROUND_LUMINOSITY_MC16RI_4S.items():
        event_mask = is_mc16ri & is_4s & (event - 0.5 < event_type) & (event_type < event + 0.5)
        weights[event_mask] = LUMINOSITY_MC16RI_4S / generated_luminosity
        matched |= event_mask

    if np.any(is_mc16ri & ~matched):
        unknown = df.loc[is_mc16ri & ~matched, ["MyEnergyType", "MyEventType"]]
        raise ValueError(f"No MC16ri weight for energy/event types: {unknown.drop_duplicates().to_dict('records')}")

    return pd.Series(weights, index=df.index)


def summarize_variable_metrics(df, bins=1000, skip_cols=["label", "weight"], skipped_variables=None):
    # Subset signal and background
    signal_df = df[df["label"] == 1]
    bkg_df    = df[df["label"] == 0]
    signal_weights = signal_df["weight"].values
    bkg_weights    = bkg_df["weight"].values

    features = [col for col in df.columns if col not in skip_cols and col not in DCOR_TARGET_COLUMNS]
    results = []

    def compute_separation(signal, background, signal_weights, background_weights, bins):
        signal_valid = np.isfinite(signal) & np.isfinite(signal_weights) & (signal_weights > 0)
        background_valid = np.isfinite(background) & np.isfinite(background_weights) & (background_weights > 0)
        if not signal_valid.any() or not background_valid.any():
            return np.nan
        h_s, bin_edges = np.histogram(signal[signal_valid], bins=bins,
                                      weights=signal_weights[signal_valid])
        h_b, _ = np.histogram(background[background_valid], bins=bin_edges,
                              weights=background_weights[background_valid])
        if h_s.sum() <= 0 or h_b.sum() <= 0:
            return np.nan
        bin_widths = np.diff(bin_edges)
        h_s = h_s / (h_s.sum() * bin_widths)
        h_b = h_b / (h_b.sum() * bin_widths)
        epsilon = 1e-10
        separation = 0.5 * np.sum(((h_s - h_b) ** 2) / (h_s + h_b + epsilon) * bin_widths)
        return float(separation) if np.isfinite(separation) else np.nan

    for feature in features:
        try:
            signal_values = signal_df[feature].values
            bkg_values    = bkg_df[feature].values
            sep = compute_separation(signal_values, bkg_values, signal_weights, bkg_weights, bins)

            if np.isfinite(sep):
                results.append({"varname": feature, "separation": sep})
            else:
                print(f"Skipping {feature} due to invalid separation.")
                if skipped_variables is not None:
                    skipped_variables.append((feature, "invalid separation (no usable weighted histogram)"))
        except Exception as e:
            print(f"Skipping {feature} due to error: {e}")
            if skipped_variables is not None:
                skipped_variables.append((feature, f"separation error: {e}"))

    return pd.DataFrame(results, columns=["varname", "separation"]).sort_values(by="separation", ascending=False)


def add_selected_dcor_metrics(selected_summary, distance_evaluators):
    """Calculate individual dCor only for variables retained by selection."""
    target_label = "_".join(DCOR_TARGET_COLUMNS)
    columns = [
        f"signal_dcor_{target_label}", f"signal_dcor_batch_mean_{target_label}",
        f"signal_dcor_batch_std_{target_label}", f"bkg_dcor_{target_label}",
        f"bkg_dcor_batch_mean_{target_label}", f"bkg_dcor_batch_std_{target_label}",
    ]
    metrics = []
    for feature in selected_summary["varname"]:
        signal_dcor = distance_evaluators[1].stats([feature])
        bkg_dcor = distance_evaluators[0].stats([feature])
        metrics.append({
            f"signal_dcor_{target_label}": signal_dcor["estimate"],
            f"signal_dcor_batch_mean_{target_label}": signal_dcor["batch_mean"],
            f"signal_dcor_batch_std_{target_label}": signal_dcor["batch_std"],
            f"bkg_dcor_{target_label}": bkg_dcor["estimate"],
            f"bkg_dcor_batch_mean_{target_label}": bkg_dcor["batch_mean"],
            f"bkg_dcor_batch_std_{target_label}": bkg_dcor["batch_std"],
        })
    return pd.concat([selected_summary.reset_index(drop=True),
                      pd.DataFrame(metrics, columns=columns)], axis=1)

def create_and_plot_spearman_matrix(df, selected_vars, region_name):
    print(f"\n--- Generating Spearman Matrix for Region {region_name} ---")
    if not selected_vars:
        print("No variables passed final selection; skipping Spearman outputs.")
        return

    # Calculate the matrix only for variables that passed final selection.
    df_top = df[selected_vars]
    print("Calculating Spearman correlation matrix...")
    spearman_corr = df_top.corr(method='spearman')

    # Plot Spearman heatmap.
    plt.figure(figsize=(20, 18))
    sns.heatmap(spearman_corr, annot=False, cmap='viridis', fmt=".2f")
    plt.title(f'Spearman Correlation Matrix ({len(selected_vars)} Selected Variables) - Region {region_name}', fontsize=16)
    plt.xticks(rotation=90)
    plt.yticks(rotation=0)
    plt.tight_layout()
    spearman_filename = f'spearman_correlation_heatmap_region_{region_name}.png'
    plt.savefig(spearman_filename)
    plt.close()
    print(f"Saved Spearman heatmap to {spearman_filename}")

    # Save Spearman matrix.
    spearman_corr.to_csv(f'spearman_correlation_heatmap_region_{region_name}.csv')


def plot_selected_separation_power(selected_summary, region_name):
    """Plot the separation power of variables retained by final selection."""
    if selected_summary.empty:
        print(f"No selected variables for Region {region_name}; skipping separation plot.")
        return

    plot_df = selected_summary.sort_values("separation", ascending=True)
    fig_height = max(4.5, 1.5 + 0.38 * len(plot_df))
    fig, ax = plt.subplots(figsize=(12, fig_height))
    bars = ax.barh(plot_df["varname"], plot_df["separation"], color="steelblue")
    for bar, value in zip(bars, plot_df["separation"]):
        ax.text(bar.get_width(), bar.get_y() + bar.get_height() / 2,
                f" {value:.3g}", va="center")
    ax.set_xlabel("Separation power")
    ax.set_ylabel("Selected variable")
    ax.set_title(f"Separation Power of Selected Variables - Region {region_name}")
    ax.grid(axis="x", linestyle="--", alpha=0.3)
    ax.set_axisbelow(True)
    ax.margins(x=0.15)
    fig.tight_layout()

    filename = f"separation_power_region_{region_name}.png"
    fig.savefig(filename, dpi=150, bbox_inches="tight")
    plt.close(fig)
    print(f"Saved separation power bar plot to {filename}")

parser = argparse.ArgumentParser()
parser.add_argument(
    '--removed_variables', 
    nargs='+',  # Accepts one or more values
    default=[
        "__experiment__",
        "__run__",
        "__event__",
        "__production__",
        "__candidate__",
        "__ncandidates__",
        "__eventType__",
        "__weight__",
        "px",
        "py",
        "pz",
        "dx",
        "dy",
        "dz",
        "x",
        "y",
        "z",
        "x_uncertainty",
        "y_uncertainty",
        "z_uncertainty",
        "beamE",
        "extraInfo__bodecayModeID__bc",
        "prodVertexX",
        "prodVertexY",
        "prodVertexZ",
        "prodVertexXErr",
        "prodVertexYErr",
        "prodVertexZErr",
        "eventExtraInfo__boEventCode__bc",
        "first_muon_isolation",
        "second_muon_isolation",
        "third_muon_isolation",
        "MySampleType",
        "MyEventType",
        "MyEnergyType",
        "mcPDG"
    ],
    help='Column names or shell-style patterns to remove before calculating separation and correlation.'
)
parser.add_argument(
    '--input_path', 
    required=True,
    help='input path'
)
parser.add_argument(
    '--distance_threshold', type=float, default=0.1,
    help=f'Threshold on the pooled Monte Carlo estimate of full weighted dCor between selected variables and {DCOR_TARGET_COLUMNS}.'
)
parser.add_argument(
    '--distance_max_events', type=int, default=6000,
    help='Events sampled with replacement in EACH draw, per class and region (default: 6000). Memory scales with its square.'
)
parser.add_argument(
    '--distance_repeats', type=int, default=10,
    help='Independent draws per class and region (default: 10); each draw contains --distance_max_events events.'
)
parser.add_argument(
    '--distance_workers', type=int, default=10,
    help='Concurrent threads for distance-correlation batches (default: 10). Memory grows with the number of workers.'
)
parser.add_argument(
    '--distance_seed', type=int, default=42,
    help='Base random seed for distance-correlation subsampling (default: 42).'
)
args = parser.parse_args()
if not 0 <= args.distance_threshold <= 1:
    parser.error('--distance_threshold must be between 0 and 1')
if args.distance_max_events < 5:
    parser.error('--distance_max_events must be at least 5')
if args.distance_repeats < 1:
    parser.error('--distance_repeats must be at least 1')
if args.distance_workers < 1:
    parser.error('--distance_workers must be at least 1')
if args.distance_seed < 0:
    parser.error('--distance_seed must be nonnegative')

def ReadResolution(file_path: str):
    """
    Reads M_deltaE_result.txt and returns M, deltaE, and theta values.
    
    Args:
        file_path (str): M_deltaE_result.txt file
        
    Returns:
        dict: {
            "M": {"peak": ..., "left_sigma": ..., "right_sigma": ...},
            "deltaE": {"peak": ..., "left_sigma": ..., "right_sigma": ...},
            "theta": ...
        }
    """

    with open(file_path, "r") as f:
        lines = f.read().splitlines()

        M_values = [float(x) for x in lines[0].split()]
        deltaE_values = [float(x) for x in lines[1].split()]
        theta = float(lines[2].split()[0])
        
    return {
        "M": {
            "peak": M_values[0],
            "left_sigma": M_values[1],
            "right_sigma": M_values[2],
            "result": M_values[3]
        },
        "deltaE": {
            "peak": deltaE_values[0],
            "left_sigma": deltaE_values[1],
            "right_sigma": deltaE_values[2],
            "result": deltaE_values[3]
        },
        "theta": theta
    }

def read_single_file(path, tree_name, branches):
    try:
        with uproot.open(path) as file:
            if tree_name in file:
                return file[tree_name].arrays(filter_name=branches, library="pd")
    except Exception as e:
        print(f"Error reading file {path}: {e}")
    return None

def read_all_root_files_self_function(
    dirs: Union[str, List[str]],
    tree_name: str = "tau_lfv",
    branches: Optional[List[str]] = None,
    step_size: str = "100 MB"
) -> pd.DataFrame:
    """
    Reads all ROOT files from one or more directories with an accurate tqdm progress bar,
    extracts specified branches, and returns a single concatenated DataFrame.

    Parameters:
    - dirs: str or List[str], one or more base directories to search for ROOT files.
    - tree_name: str, the name of the TTree inside each ROOT file.
    - branches: Optional[List[str]], list of branch names to read. If None, reads all branches.
    - step_size: str or int, the size of data chunks to read at a time (e.g., "100 MB" or 100000 entries).

    Returns:
    - pd.DataFrame containing the data from the specified tree across all ROOT files.
    """
    if isinstance(dirs, str):
        dirs = [dirs]

    # --- 1. File Discovery ---
    root_files = []
    print("Discovering .root files...")
    for base_dir in dirs:
        for root, _, files in os.walk(base_dir):
            for file in files:
                if file.endswith(".root"):
                    full_path = os.path.normpath(os.path.join(root, file))
                    root_files.append(full_path)

    if not root_files:
        print("No .root files found.")
        return pd.DataFrame()

    # Create a list of "file:tree" strings for uproot
    files_with_trees = [f"{path}:{tree_name}" for path in root_files]

    # remove unneeded variables
    EXCLUDE_SUBSTRINGS = ( "OneMuon", "TwoMuon", "ThreeMuon", "FTDL", "PSNM", "bogamma__clcut_v", "isSignal", "DecayHash", "MCMode", "ID", "mcPDG" )
    branches_postfilter = []
    if branches is None:
        with uproot.open(files_with_trees[0]) as tree:
            all_branches = tree.keys()
        for b in all_branches:
            skip = False
            for s in EXCLUDE_SUBSTRINGS:
                if s in b:
                    skip = True
                    break
            if not skip:
                branches_postfilter.append(b)
    else:
        for b in branches:
            skip = False
            for s in EXCLUDE_SUBSTRINGS:
                if s in b:
                    skip = True
                    break
            if not skip:
                branches_postfilter.append(b)
        
    
    # --- 2. Pre-scan to get total entries for tqdm ---
    total_entries = 0
    print("Pre-scanning files to determine total entries...")
    try:
        # --- CORRECTED PRE-SCAN LOGIC ---
        # Iterate through each file path, open it, and get the number of entries.
        for path in tqdm.tqdm(files_with_trees, desc="Pre-scanning files"):
            with uproot.open(path) as f:
                # The file object 'f' here is the TTree because we specified it in the path
                total_entries += f.num_entries
    except Exception as e:
        print(f"Error during pre-scan: {e}")
        print("Could not determine total entries. Progress bar will not show percentage.")
        total_entries = None

    # --- 3. Iterate over chunks with a manually updated tqdm progress bar ---
    dfs = []
    print("Reading data...")
    try:
        file_iterator = uproot.iterate(
            files_with_trees,
            expressions=branches_postfilter,
            library="pd",
            step_size=step_size
        )
        
        # Manually create and update the progress bar for accuracy
        with tqdm.tqdm(total=total_entries, desc="Processing entries", unit=" entries") as progress_bar:
            for df_chunk in file_iterator:
                dfs.append(df_chunk)
                progress_bar.update(len(df_chunk)) # Update by the actual number of entries in the chunk

    except Exception as e:
        print(f"An error occurred while reading the ROOT files: {e}")
        return pd.DataFrame()

    # --- 4. Concatenate all chunks at the end ---
    if not dfs:
        print("No data was read.")
        return pd.DataFrame()
        
    print("Concatenating data chunks...")
    return pd.concat(dfs, ignore_index=True)

def read_with_weight(paths, tree_name, input_variables):
    print("start to read %s" % paths)
    df = read_all_root_files_self_function(dirs=paths, tree_name=tree_name, branches=input_variables)
    if not df.empty:
        df["weight"] = calculate_weights(df)
    return df


def drop_removed_variables(df, patterns):
    """Remove columns matching exact names or shell-style patterns."""
    columns_to_drop = [
        column for column in df.columns
        if any(column == pattern or fnmatchcase(column, pattern) for pattern in patterns)
    ]
    return df.drop(columns=columns_to_drop)

signal_list = ["SIGNAL"]
# MC16ri directory names from bash/one_touch_MC16ri.sh.
background_list = ["CHG", "MIX", "UUBAR", "DDBAR", "SSBAR", "CCBAR",
                   "MUMU", "EE", "EEEE", "EEMUMU", "LLXX", "HHISR", "GG", "TAUPAIR"]

removed_variables = args.removed_variables
input_path = args.input_path

SIGNAL_train_path = [f"{input_path}/{e}/final_output_train/" for e in signal_list]
SIGNAL_test_path = [f"{input_path}/{e}/final_output_test/" for e in signal_list]
BKG_train_path = [f"{input_path}/{e}/final_output_train/" for e in background_list]
BKG_test_path = [f"{input_path}/{e}/final_output_test/" for e in background_list]

resolution = ReadResolution(f"{input_path}/M_deltaE_result.txt")

# read ROOT files
df_SIGNAL_train_list = []
df_SIGNAL_test_list  = []
df_BKG_train_list = []
df_BKG_test_list = []

for label in signal_list:
    train_path = [f"{input_path}/{label}/final_output_train/"]
    test_path = [f"{input_path}/{label}/final_output_test/"]
    
    df_train = read_with_weight(train_path, "tau_lfv", input_variables = None)
    df_test  = read_with_weight(test_path,  "tau_lfv", input_variables = None)

    df_SIGNAL_train_list.append(df_train)
    df_SIGNAL_test_list.append(df_test)

for label in background_list:
    train_path = [f"{input_path}/{label}/final_output_train/"]
    test_path = [f"{input_path}/{label}/final_output_test/"]
    
    df_train = read_with_weight(train_path, "tau_lfv", input_variables = None)
    df_test  = read_with_weight(test_path, "tau_lfv", input_variables = None)

    df_BKG_train_list.append(df_train)
    df_BKG_test_list.append(df_test)

df_SIGNAL_train = pd.concat(df_SIGNAL_train_list, ignore_index=True)
df_SIGNAL_test  = pd.concat(df_SIGNAL_test_list, ignore_index=True)
df_BKG_train = pd.concat(df_BKG_train_list, ignore_index=True)
df_BKG_test  = pd.concat(df_BKG_test_list, ignore_index=True)

del df_SIGNAL_train_list
del df_SIGNAL_test_list
del df_BKG_train_list
del df_BKG_test_list

# Add labels (signal = 1, background = 0)
df_SIGNAL_train["label"] = 1
df_SIGNAL_test["label"] = 1
df_BKG_train["label"] = 0
df_BKG_test["label"] = 0

# merge data
df_train = pd.concat([df_SIGNAL_train, df_BKG_train], ignore_index=True)
df_test = pd.concat([df_SIGNAL_test, df_BKG_test], ignore_index=True)

# remove unneeded features
df_train = drop_removed_variables(df_train, removed_variables)
df_test = drop_removed_variables(df_test, removed_variables)

# ====================================================== region one ====================================================== #
# filter
df_train_one = df_train[((resolution["deltaE"]["peak"] - 5*resolution["deltaE"]["left_sigma"]) < df_train["deltaE"]) & (df_train["deltaE"] < (resolution["deltaE"]["peak"] + 5*resolution["deltaE"]["right_sigma"]))]
df_train_one = df_train_one[((resolution["M"]["peak"] - 5*resolution["M"]["left_sigma"]) < df_train_one["M"]) & (df_train_one["M"] < (resolution["M"]["peak"] + 5*resolution["M"]["right_sigma"]))]
df_test_one = df_test[((resolution["deltaE"]["peak"] - 5*resolution["deltaE"]["left_sigma"]) < df_test["deltaE"]) & (df_test["deltaE"] < (resolution["deltaE"]["peak"] + 5*resolution["deltaE"]["right_sigma"]))]
df_test_one = df_test_one[((resolution["M"]["peak"] - 5*resolution["M"]["left_sigma"]) < df_test_one["M"]) & (df_test_one["M"] < (resolution["M"]["peak"] + 5*resolution["M"]["right_sigma"]))]

skipped_variables_one = []
summary_result = summarize_variable_metrics(df_train_one, skipped_variables=skipped_variables_one)
distance_evaluators = make_distance_evaluators(df_train_one, args.distance_max_events, args.distance_repeats, args.distance_seed, args.distance_workers)
selected_vars_one, selected_dcor_one = select_variables(
    summary_result, df_train_one, "one", distance_evaluators, args.distance_threshold,
    report_path="variable_selection_report_one.txt", skipped_variables=skipped_variables_one,
)
selected_summary_one = add_selected_dcor_metrics(summary_result.set_index("varname").loc[selected_vars_one].reset_index(), distance_evaluators).merge(selected_dcor_one, on="varname", sort=False, validate="one_to_one")
print(selected_summary_one)
selected_summary_one.to_csv("Importance_one.csv", index=False)
plot_selected_separation_power(selected_summary_one, "one")
create_and_plot_spearman_matrix(df_train_one[df_train_one["label"] == 1], selected_vars_one, "one_signal")
create_and_plot_spearman_matrix(df_train_one[df_train_one["label"] == 0], selected_vars_one, "one_bkg")

# ====================================================== region two ====================================================== #
# filter
df_train_two = df_train[((resolution["deltaE"]["peak"] - 15*resolution["deltaE"]["left_sigma"]) < df_train["deltaE"]) & (df_train["deltaE"] < (resolution["deltaE"]["peak"] - 5*resolution["deltaE"]["left_sigma"]))]
df_train_two = df_train_two[((resolution["M"]["peak"] - 5*resolution["M"]["left_sigma"]) < df_train_two["M"]) & (df_train_two["M"] < (resolution["M"]["peak"] + 5*resolution["M"]["right_sigma"]))]
df_test_two = df_test[((resolution["deltaE"]["peak"] - 15*resolution["deltaE"]["left_sigma"]) < df_test["deltaE"]) & (df_test["deltaE"] < (resolution["deltaE"]["peak"] - 5*resolution["deltaE"]["left_sigma"]))]
df_test_two = df_test_two[((resolution["M"]["peak"] - 5*resolution["M"]["left_sigma"]) < df_test_two["M"]) & (df_test_two["M"] < (resolution["M"]["peak"] + 5*resolution["M"]["right_sigma"]))]

skipped_variables_two = []
summary_result = summarize_variable_metrics(df_train_two, skipped_variables=skipped_variables_two)
distance_evaluators = make_distance_evaluators(df_train_two, args.distance_max_events, args.distance_repeats, args.distance_seed, args.distance_workers)
selected_vars_two, selected_dcor_two = select_variables(
    summary_result, df_train_two, "two", distance_evaluators, args.distance_threshold,
    report_path="variable_selection_report_two.txt", skipped_variables=skipped_variables_two,
)
selected_summary_two = add_selected_dcor_metrics(summary_result.set_index("varname").loc[selected_vars_two].reset_index(), distance_evaluators).merge(selected_dcor_two, on="varname", sort=False, validate="one_to_one")
print(selected_summary_two)
selected_summary_two.to_csv("Importance_two.csv", index=False)
plot_selected_separation_power(selected_summary_two, "two")
create_and_plot_spearman_matrix(df_train_two[df_train_two["label"] == 1], selected_vars_two, "two_signal")
create_and_plot_spearman_matrix(df_test_two[df_test_two["label"] == 0], selected_vars_two, "two_bkg")
