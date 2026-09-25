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

# MC16rd luminosities (ab-1) and generated equivalent luminosities (ab-1).
# Values follow analysis_code/include/constants.h.
LUMINOSITY_MC16RD = {1: 0.49841, 2: 0.061, 7: 0.01976}  # 4S, off, 5S

# MyEventType -> generated luminosity. Event types follow MyObtainWeight.h.
BACKGROUND_LUMINOSITY_MC16RD = {
    1: {
        1: 1.94633937621,  # CHG
        2: 1.94563145615,  # MIX
        3: 1.94633637827,  # UUBAR
        4: 1.94633884394,  # DDBAR
        5: 1.94633884394,  # SSBAR
        6: 1.94633711858,  # CHARM
        7: 1.94633884394,  # MUMU
        8: 0.0486578614459,  # EE
        9: 0.486566030869,  # EEEE
        10: 0.486578551736,  # EEMUMU
        16: 0.973157373475,  # GG
        22: 1.94633937621,  # TAUPAIR
        33: 1.94598541618,  # BB
        34: 0.453130660382,  # hhISR
        35: 1.94633637827,  # llXX
        36: 1.94633779618,  # UDSC
    },
    2: {
        3: 0.239977757314,  # UUBAR
        4: 0.239977757314,  # DDBAR
        5: 0.239977757314,  # SSBAR
        6: 0.239977757314,  # CHARM
        7: 0.239977757314,  # MUMU
        8: 0.00599944393284,  # EE
        9: 0.0586445594131,  # EEEE
        10: 0.0586450400904,  # EEMUMU
        16: 0.119988878657,  # GG
        22: 0.239977757314,  # TAUPAIR
        34: 0.0599961249791,  # hhISR
        35: 0.238369538444,  # llXX
        36: 0.239977757314,  # UDSC
    },
    7: {
        3: 0.0785390801196,  # UUBAR
        4: 0.0785390801196,  # DDBAR
        5: 0.0785390801196,  # SSBAR
        6: 0.0785390801196,  # CHARM
        7: 0.0785390801196,  # MUMU
        8: 0.00196347700299,  # EE
        9: 0.0196347700299,  # EEEE
        10: 0.0196347700299,  # EEMUMU
        16: 0.0392695400598,  # GG
        22: 0.0785390801196,  # TAUPAIR
        33: 0.0785390801196,  # BB
        34: 0.0196347700299,  # hhISR
        35: 0.0785390801196,  # llXX
        36: 0.0785390801196,  # UDSC
    },
}

BR_SIGNAL = 1e-8
TAU_CROSSSECTION_4S = 0.919  # nb
SIGNAL_MC16RD_EVENTS = {1: 4632618, 2: 564223, 7: 186901}
SIGNAL_ENERGY_GEV = {
    1: 10.58,
    2: 10.52,
    7: (10.657 * 3.544 + 10.706 * 1.628 + 10.751 * 9.880 + 10.810 * 4.713) / 19.764,
}


def signal_scale_mc16rd(energy_type):
    cross_section = TAU_CROSSSECTION_4S * (10.58 / SIGNAL_ENERGY_GEV[energy_type]) ** 2
    tau_pairs = LUMINOSITY_MC16RD[energy_type] / 1e-9 * cross_section
    return tau_pairs * BR_SIGNAL * 2.0 / SIGNAL_MC16RD_EVENTS[energy_type]


class DistanceCorrelationEvaluator:
    """Weighted distance correlation on a fixed, reproducible event subsample.

    The sample statistic is biased upward at finite sample size, especially as
    the number of selected variables grows. Calibrate the threshold on MC.
    """

    def __init__(self, df, max_events=1000, seed=42):
        valid = np.isfinite(df[["M", "deltaE", "weight"]].to_numpy(dtype=float)).all(axis=1)
        valid &= df["weight"].to_numpy(dtype=float) > 0
        sample = df.loc[valid]
        if len(sample) > max_events:
            sample = sample.sample(n=max_events, random_state=seed)
        if len(sample) < 5:
            raise ValueError("Distance correlation needs at least five positive-weight events.")

        self.sample = sample
        self.weights = sample["weight"].to_numpy(dtype=float, copy=True)
        self.weights /= self.weights.sum()
        self.target = self._centered_distances(sample[["M", "deltaE"]].to_numpy(dtype=float))
        if self.target is None:
            raise ValueError("M or deltaE has no variation in the distance-correlation sample.")
        self.target_variance = self._distance_variance(self.target)
        if self.target_variance <= 0:
            raise ValueError("M and deltaE have no variation in the distance-correlation sample.")

    def _centered_distances(self, values):
        values = np.asarray(values, dtype=float)
        if values.ndim == 1:
            values = values[:, None]
        if not np.isfinite(values).all():
            return None
        means = np.average(values, axis=0, weights=self.weights)
        scales = np.sqrt(np.average((values - means) ** 2, axis=0, weights=self.weights))
        if np.any(scales <= 0):
            return None
        # Standardize coordinates so Euclidean distance is not set by units.
        distances = squareform(pdist((values - means) / scales, metric="euclidean"))
        row_means = distances @ self.weights
        grand_mean = self.weights @ row_means
        return distances - row_means[:, None] - row_means[None, :] + grand_mean

    def _distance_variance(self, centered):
        return np.einsum("i,ij,ij,j->", self.weights, centered, centered, self.weights)

    def score(self, columns):
        centered = self._centered_distances(self.sample[list(columns)].to_numpy(dtype=float))
        if centered is None:
            return np.nan
        x_variance = self._distance_variance(centered)
        if x_variance <= 0:
            return np.nan
        covariance = np.einsum("i,ij,ij,j->", self.weights, centered, self.target, self.weights)
        return float(np.sqrt(np.clip(covariance / np.sqrt(x_variance * self.target_variance), 0, 1)))


def make_distance_evaluators(df, max_events):
    return {
        1: DistanceCorrelationEvaluator(df[df["label"] == 1], max_events, seed=42),
        0: DistanceCorrelationEvaluator(df[df["label"] == 0], max_events, seed=43),
    }


def select_variables(summary_df, data_df, region_name, distance_evaluators, distance_threshold):
    """
    Selects variables based on separation, correlation with M and deltaE,
    and correlation with already selected variables.
    """
    print(f"\n--- Selecting variables for Region {region_name} ---")

    # Sort variables by separation in descending order
    sorted_summary = summary_df.sort_values(by="separation", ascending=False)

    # Separate signal and background dataframes
    signal_df = data_df[data_df["label"] == 1]
    bkg_df = data_df[data_df["label"] == 0]

    selected_variables = []

    for index, row in sorted_summary.iterrows():
        candidate_var = row["varname"]

        # Check the candidate AND all previously selected variables jointly.
        candidate_set = selected_variables + [candidate_var]
        signal_dcor = distance_evaluators[1].score(candidate_set)
        bkg_dcor = distance_evaluators[0].score(candidate_set)
        if signal_dcor < distance_threshold and bkg_dcor < distance_threshold:
            is_correlated_with_selected = False
            # 2. Check correlation with already selected variables
            for selected_var in selected_variables:
                # Spearman correlation
                bkg_spearman_corr = spearmanr(bkg_df[candidate_var], bkg_df[selected_var]).correlation
                signal_spearman_corr = spearmanr(signal_df[candidate_var], signal_df[selected_var]).correlation
                if (abs(bkg_spearman_corr) > 0.5 and abs(signal_spearman_corr) > 0.5):
                    is_correlated_with_selected = True
                    break  # No need to check other selected variables

            if not is_correlated_with_selected:
                selected_variables.append(candidate_var)

    print(f"Selected {len(selected_variables)} variables for Region {region_name}:")
    print(selected_variables)
    return selected_variables

def calculate_weights(df: pd.DataFrame) -> pd.Series:
    """Match the MC16rd MC_weight entries in MyObtainWeight.h."""
    sample_type = df["MySampleType"].to_numpy()
    event_type = df["MyEventType"].to_numpy()
    energy_type = df["MyEnergyType"].to_numpy()

    weights = np.zeros(len(df), dtype=np.float64)
    is_data = ((-1.5 < sample_type) & (sample_type < -0.5)) | (
        (4.5 < sample_type) & (sample_type < 5.5)
    )
    weights[is_data] = 1.0

    is_mc16rd = (3.5 < sample_type) & (sample_type < 4.5)
    matched = is_data.copy()
    for energy, luminosity in LUMINOSITY_MC16RD.items():
        energy_mask = is_mc16rd & (energy - 0.5 < energy_type) & (energy_type < energy + 0.5)
        signal_mask = energy_mask & (-0.5 < event_type) & (event_type < 0.5)
        weights[signal_mask] = signal_scale_mc16rd(energy)
        matched |= signal_mask

        for event, generated_luminosity in BACKGROUND_LUMINOSITY_MC16RD[energy].items():
            event_mask = energy_mask & (event - 0.5 < event_type) & (event_type < event + 0.5)
            weights[event_mask] = luminosity / generated_luminosity
            matched |= event_mask

    if np.any(is_mc16rd & ~matched):
        unknown = df.loc[is_mc16rd & ~matched, ["MyEnergyType", "MyEventType"]]
        raise ValueError(f"No MC16rd weight for energy/event types: {unknown.drop_duplicates().to_dict('records')}")

    return pd.Series(weights, index=df.index)


def summarize_variable_metrics(df, distance_evaluators, bins=1000, skip_cols=["label", "weight"]):
    # Subset signal and background
    signal_df = df[df["label"] == 1]
    bkg_df    = df[df["label"] == 0]
    signal_weights = signal_df["weight"].values
    bkg_weights    = bkg_df["weight"].values

    features = [col for col in df.columns if col not in skip_cols]
    results = []

    def compute_separation(signal, background, signal_weights, background_weights, bins):
        h_s, bin_edges = np.histogram(signal, bins=bins, weights=signal_weights, density=True)
        h_b, _ = np.histogram(background, bins=bin_edges, weights=background_weights, density=True)
        epsilon = 1e-10
        bin_width = bin_edges[1] - bin_edges[0]
        separation = 0.5 * np.sum(((h_s - h_b) ** 2) / (h_s + h_b + epsilon)) * bin_width
        return separation

    for feature in features:
        try:
            signal_values = signal_df[feature].values
            bkg_values    = bkg_df[feature].values
            sep = compute_separation(signal_values, bkg_values, signal_weights, bkg_weights, bins)

            signal_dcor = distance_evaluators[1].score([feature])
            bkg_dcor = distance_evaluators[0].score([feature])

            results.append({
                "varname": feature,
                "separation": sep,
                "signal_dcor_M_deltaE": signal_dcor,
                "bkg_dcor_M_deltaE": bkg_dcor
            })
        except Exception as e:
            print(f"Skipping {feature} due to error: {e}")

    return pd.DataFrame(results).sort_values(by="separation", ascending=False)

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
        "MyEnergyType"
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
    help='Maximum joint distance correlation of selected variables with (M, deltaE). Recalibrate for each sample and event limit.'
)
parser.add_argument(
    '--distance_max_events', type=int, default=50000,
    help='Maximum events per class and region for the O(n^2) distance-correlation calculation.'
)
args = parser.parse_args()
if not 0 <= args.distance_threshold <= 1:
    parser.error('--distance_threshold must be between 0 and 1')
if args.distance_max_events < 5:
    parser.error('--distance_max_events must be at least 5')

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
    EXCLUDE_SUBSTRINGS = ( "OneMuon", "TwoMuon", "ThreeMuon", "FTDL", "PSNM", "bogamma__clcut_v", "isSignal", "DecayHash", "MCMode" )
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
# MC16rd directory names from bash/one_touch_MC16rd.sh.
background_list = ["BB", "UDSC", "MUMU", "EE", "EEEE", "EEMUMU", "LLXX", "HHISR", "GG", "TAUPAIR"]

removed_variables = args.removed_variables
input_path = args.input_path

resolution = ReadResolution(f"{input_path}/M_deltaE_result.txt")

# read ROOT files
df_signal_list = []
df_bkg_list = []

for label in signal_list:
    sample_path = f"{input_path}/{label}/final_output/"
    df_signal_list.append(read_with_weight(sample_path, "tau_lfv", input_variables=None))

for label in background_list:
    sample_path = f"{input_path}/{label}/final_output/"
    df_bkg_list.append(read_with_weight(sample_path, "tau_lfv", input_variables=None))

df_signal = pd.concat(df_signal_list, ignore_index=True)
df_bkg = pd.concat(df_bkg_list, ignore_index=True)

del df_signal_list
del df_bkg_list

# Add labels (signal = 1, background = 0)
df_signal["label"] = 1
df_bkg["label"] = 0

# merge data
df_all = pd.concat([df_signal, df_bkg], ignore_index=True)

# remove unneeded features
df_all = drop_removed_variables(df_all, removed_variables)

# ====================================================== region one ====================================================== #
# filter
df_one = df_all[((resolution["deltaE"]["peak"] - 5*resolution["deltaE"]["left_sigma"]) < df_all["deltaE"]) & (df_all["deltaE"] < (resolution["deltaE"]["peak"] + 5*resolution["deltaE"]["right_sigma"]))]
df_one = df_one[((resolution["M"]["peak"] - 20*resolution["M"]["left_sigma"]) < df_one["M"]) & (df_one["M"] < (resolution["M"]["peak"] + 20*resolution["M"]["right_sigma"]))]

distance_evaluators = make_distance_evaluators(df_one, args.distance_max_events)
summary_result = summarize_variable_metrics(df_one, distance_evaluators)
selected_vars_one = select_variables(summary_result, df_one, "one", distance_evaluators, args.distance_threshold)
selected_summary_one = summary_result.set_index("varname").loc[selected_vars_one].reset_index()
print(selected_summary_one)
selected_summary_one.to_csv("Importance_one.csv", index=False)
plot_selected_separation_power(selected_summary_one, "one")
create_and_plot_spearman_matrix(df_one[df_one["label"] == 1], selected_vars_one, "one_signal")
create_and_plot_spearman_matrix(df_one[df_one["label"] == 0], selected_vars_one, "one_bkg")

# ====================================================== region two ====================================================== #
# filter
df_two = df_all[((resolution["deltaE"]["peak"] - 15*resolution["deltaE"]["left_sigma"]) < df_all["deltaE"]) & (df_all["deltaE"] < (resolution["deltaE"]["peak"] - 5*resolution["deltaE"]["left_sigma"]))]
df_two = df_two[((resolution["M"]["peak"] - 20*resolution["M"]["left_sigma"]) < df_two["M"]) & (df_two["M"] < (resolution["M"]["peak"] + 20*resolution["M"]["right_sigma"]))]

distance_evaluators = make_distance_evaluators(df_two, args.distance_max_events)
summary_result = summarize_variable_metrics(df_two, distance_evaluators)
selected_vars_two = select_variables(summary_result, df_two, "two", distance_evaluators, args.distance_threshold)
selected_summary_two = summary_result.set_index("varname").loc[selected_vars_two].reset_index()
print(selected_summary_two)
selected_summary_two.to_csv("Importance_two.csv", index=False)
plot_selected_separation_power(selected_summary_two, "two")
create_and_plot_spearman_matrix(df_two[df_two["label"] == 1], selected_vars_two, "two_signal")
create_and_plot_spearman_matrix(df_two[df_two["label"] == 0], selected_vars_two, "two_bkg")
