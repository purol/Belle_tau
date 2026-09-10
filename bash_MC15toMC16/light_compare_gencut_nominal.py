import glob
import numpy as np
import uproot
import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib.patches import Patch
from matplotlib.lines import Line2D
from scipy.stats import ks_2samp


# ============================================================
# Hard-coded settings
# ============================================================

DIR1 = "/home/belle2/junewoo/storage_b2/temp/Belle_tau/Larva_MC16ri_gencut/v000/DDBAR/before_strict_M_deltaE_selection/"

DIR2 = "/home/belle2/junewoo/storage_b2/temp/Belle_tau/Larva_MC16ri_nominal/v000/DDBAR/before_strict_M_deltaE_selection/"

TREE_NAME = "tau_lfv"


VARIABLE = "cleoConeThrust0"

NBINS = 20
XRANGE = (0.0, 10.0)


CUTS = {
    "deltaE": (-0.214264, 0.061181),
    "M":      (1.684249, 1.871269),
}


LEGEND1 = r"MC16ri $d\bar{d}$ with gencut"
LEGEND2 = r"MC16ri $d\bar{d}$ without gencut"

RATIO_LABEL = "Gencut / Nominal"


NORMALIZATION_MODE = "unit"

# Used only when NORMALIZATION_MODE == "scale"
SCALE_FACTOR1 = 0.03333333333
SCALE_FACTOR2 = 1.0


LOG_SCALE = False
RATIO_RANGE = (0.0, 2.0)

OUTPUT = "distribution_comparison_signal_region_cleoConeThrust0.png"


# ============================================================
# Read ROOT files
# ============================================================

def load_variable(directory, tree_name, variable, cuts):

    files = sorted(glob.glob(f"{directory}/*.root"))

    if len(files) == 0:
        raise RuntimeError(f"No ROOT files found in: {directory}")

    print()
    print("=" * 80)
    print(f"Directory : {directory}")
    print(f"ROOT files: {len(files)}")
    print("=" * 80)

    values = []

    total_before = 0
    total_after = 0

    # Branches needed for plotting and cuts
    branches = list(dict.fromkeys([variable] + list(cuts.keys())))

    for filename in files:
        try:
            with uproot.open(filename) as f:
                tree = f[TREE_NAME]

                arrays = {
                    branch: tree[branch].array(library="np")
                    for branch in branches
                }

                n_before = len(arrays[variable])

                mask = np.ones(n_before, dtype=bool)

                # Remove NaN / inf
                for branch in branches:
                    mask &= np.isfinite(arrays[branch])

                # Apply cuts
                for branch, (low, high) in cuts.items():
                    mask &= (arrays[branch] > low)
                    mask &= (arrays[branch] < high)

                n_after = np.sum(mask)

                total_before += n_before
                total_after += n_after

                values.append(arrays[variable][mask])

        except Exception as e:
            print(f"Skipping {filename}: {e}")

    if len(values) == 0:
        raise RuntimeError(f"No valid data loaded from {directory}")

    if total_before == 0:
        raise RuntimeError(f"No entries found in {directory}")

    print(
        f"Entries after cuts: "
        f"{total_before} -> {total_after} "
        f"({total_after / total_before:.3%})"
    )

    return np.concatenate(values)


# ============================================================
# Load samples
# ============================================================

data1 = load_variable(DIR1, TREE_NAME, VARIABLE, CUTS)
data2 = load_variable(DIR2, TREE_NAME, VARIABLE, CUTS)

print()
print("=" * 80)
print("Selected entries")
print("=" * 80)
print(f"{LEGEND1}: {len(data1)}")
print(f"{LEGEND2}: {len(data2)}")


# ============================================================
# Kolmogorov-Smirnov test
# ============================================================

ks_data1 = data1[
    (data1 >= XRANGE[0]) &
    (data1 <= XRANGE[1])
]

ks_data2 = data2[
    (data2 >= XRANGE[0]) &
    (data2 <= XRANGE[1])
]

ks_result = ks_2samp(
    ks_data1,
    ks_data2,
    alternative="two-sided",
    method="auto"
)

KS_STATISTIC = ks_result.statistic
KS_PVALUE = ks_result.pvalue

print()
print("=" * 80)
print("Kolmogorov-Smirnov test")
print("=" * 80)
print(f"KS statistic : {KS_STATISTIC:.6g}")
print(f"p-value      : {KS_PVALUE:.6g}")


# ============================================================
# Histogram
# ============================================================

bins = np.linspace(XRANGE[0], XRANGE[1], NBINS + 1)

count1, edges = np.histogram(data1, bins=bins)
count2, _     = np.histogram(data2, bins=bins)

# Number of entries inside XRANGE
N1 = np.sum(count1)
N2 = np.sum(count2)

print()
print("=" * 80)
print("Entries inside XRANGE")
print("=" * 80)
print(f"Sample 1: {N1}")
print(f"Sample 2: {N2}")

if N1 == 0 or N2 == 0:
    raise RuntimeError("One of the histograms has zero entries inside XRANGE.")


# ============================================================
# Normalization / scaling
# ============================================================

if NORMALIZATION_MODE == "unit":

    hist1 = count1.astype(float) / N1
    hist2 = count2.astype(float) / N2

    # Statistical uncertainty: sqrt(n) / N
    err1 = np.sqrt(count1) / N1
    err2 = np.sqrt(count2) / N2

    YLABEL = "Normalized entries"

elif NORMALIZATION_MODE == "scale":

    if SCALE_FACTOR1 < 0 or SCALE_FACTOR2 < 0:
        raise ValueError("Scale factors must be non-negative.")

    hist1 = count1.astype(float) * SCALE_FACTOR1
    hist2 = count2.astype(float) * SCALE_FACTOR2

    # Statistical uncertainty: scale * sqrt(n)
    err1 = np.sqrt(count1) * SCALE_FACTOR1
    err2 = np.sqrt(count2) * SCALE_FACTOR2

    YLABEL = "Scaled entries"

else:
    raise ValueError("NORMALIZATION_MODE must be either 'unit' or 'scale'.")


print()
print("=" * 80)
print("Normalization")
print("=" * 80)
print(f"Mode: {NORMALIZATION_MODE}")

if NORMALIZATION_MODE == "unit":
    print(f"Sample 1 integral: {np.sum(hist1):.6f}")
    print(f"Sample 2 integral: {np.sum(hist2):.6f}")
else:
    print(f"Sample 1 scale factor: {SCALE_FACTOR1}")
    print(f"Sample 2 scale factor: {SCALE_FACTOR2}")


# Bin centers
centers = 0.5 * (edges[:-1] + edges[1:])


# ============================================================
# Ratio
#
# R = hist1 / hist2
# ============================================================

ratio = np.full(len(hist1), np.nan, dtype=float)
ratio_err = np.full(len(hist1), np.nan, dtype=float)

valid = (hist1 > 0) & (hist2 > 0)

ratio[valid] = hist1[valid] / hist2[valid]

ratio_err[valid] = ratio[valid] * np.sqrt(
    (err1[valid] / hist1[valid])**2
    +
    (err2[valid] / hist2[valid])**2
)


# ============================================================
# Plot
# ============================================================

fig, (ax, ax_ratio) = plt.subplots(
    2,
    1,
    figsize=(8, 8),
    sharex=True,
    gridspec_kw={
        "height_ratios": [4, 1],
        "hspace": 0.05
    }
)

default_colors = plt.rcParams["axes.prop_cycle"].by_key()["color"]
color1 = default_colors[0]
color2 = default_colors[1]


# ============================================================
# Upper panel
#
# DIR1: histogram + hatched uncertainty band
# DIR2: dot + line + error bars
# ============================================================

# ------------------------------------------------------------
# DIR1 central histogram
# ------------------------------------------------------------

ax.stairs(
    hist1,
    edges,
    edgecolor=color1,
    facecolor=color1,
    linewidth=1.8,
    fill=True,
    alpha=0.25,
    zorder=2
)

# ------------------------------------------------------------
# DIR1 uncertainty band (hatched)
# ------------------------------------------------------------

hist1_step = np.r_[hist1, hist1[-1]]
err1_step = np.r_[err1, err1[-1]]

upper1 = hist1_step + err1_step

if LOG_SCALE:
    lower1 = np.maximum(hist1_step - err1_step, 1e-12)
else:
    lower1 = np.maximum(hist1_step - err1_step, 0.0)

old_hatch_color = mpl.rcParams["hatch.color"]
mpl.rcParams["hatch.color"] = color1

ax.fill_between(
    edges,
    lower1,
    upper1,
    step="post",
    facecolor="none",
    edgecolor=color1,
    hatch="////",
    linewidth=0.0,
    zorder=3
)

mpl.rcParams["hatch.color"] = old_hatch_color


# ------------------------------------------------------------
# DIR2: dot + line + uncertainty
# ------------------------------------------------------------

ax.errorbar(
    centers,
    hist2,
    yerr=err2,
    fmt="o",
    color=color2,
    markersize=4,
    linewidth=1.5,
    elinewidth=1.2,
    capsize=2,
    zorder=4
)


# ============================================================
# Custom legend
# ============================================================

legend_handles = [
    Patch(
        facecolor=color1,
        edgecolor=color1,
        alpha=0.25,
        hatch="////",
        label=LEGEND1
    ),
    Line2D(
        [0], [0],
        color=color2,
        marker="o",
        linestyle="-",
        linewidth=1.5,
        markersize=5,
        label=LEGEND2
    )
]

ax.legend(handles=legend_handles, loc="best")

ax.set_ylabel(YLABEL)
ax.grid(alpha=0.2)


# ============================================================
# Log scale option
# ============================================================

if LOG_SCALE:
    ax.set_yscale("log")


# ============================================================
# Ratio panel
# ============================================================

ax_ratio.axhline(
    1.0,
    color="black",
    linewidth=1,
    linestyle="--"
)

ax_ratio.errorbar(
    centers[valid],
    ratio[valid],
    yerr=ratio_err[valid],
    fmt="o",
    markersize=3,
    capsize=2,
    linewidth=1,
    color=color1
)

ax_ratio.set_ylabel(RATIO_LABEL, fontsize=10)
ax_ratio.set_xlabel(VARIABLE)
ax_ratio.set_ylim(RATIO_RANGE[0], RATIO_RANGE[1])
ax_ratio.grid(alpha=0.2)


# ============================================================
# Save
# ============================================================

plt.savefig(
    OUTPUT,
    dpi=200,
    bbox_inches="tight"
)

plt.show()