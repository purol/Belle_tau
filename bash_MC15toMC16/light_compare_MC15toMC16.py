import glob
import numpy as np
import uproot
import matplotlib.pyplot as plt
from matplotlib.patches import Patch

# ============================================================
# Hard-coded settings
# ============================================================

DIR1 = "/home/belle2/junewoo/storage_b2/temp/Belle_tau/Larva_MC15ri/v000/SIGNAL/before_strict_M_deltaE_selection/"
DIR2 = "/home/belle2/junewoo/storage_b2/temp/Belle_tau/Larva_MC16ri/v000/SIGNAL/before_strict_M_deltaE_selection/"

TREE_NAME = "tau_lfv"
VARIABLE = "deltaE"

LEGEND1 = r"MC15ri prompt $\tau \rightarrow \mu\mu\mu$"
LEGEND2 = r"MC16ri prompt $\tau \rightarrow \mu\mu\mu$"

NBINS = 50
XRANGE = (-0.4, 0.2)

OUTPUT = "distribution_comparison_deltaE.png"

LOG_SCALE = False


# ============================================================
# Read ROOT files
# ============================================================

def load_variable(directory, tree_name, variable):
    files = sorted(glob.glob(f"{directory}/*.root"))

    if len(files) == 0:
        raise RuntimeError(f"No ROOT files found in: {directory}")

    print(f"{directory}: {len(files)} ROOT files")

    values = []

    for filename in files:
        try:
            with uproot.open(filename) as f:
                tree = f[tree_name]
                arr = tree[variable].array(library="np")

                # Remove NaN / inf
                arr = arr[np.isfinite(arr)]

                values.append(arr)

        except Exception as e:
            print(f"Skipping {filename}: {e}")

    if len(values) == 0:
        raise RuntimeError(f"No valid data loaded from {directory}")

    return np.concatenate(values)


data1 = load_variable(DIR1, TREE_NAME, VARIABLE)
data2 = load_variable(DIR2, TREE_NAME, VARIABLE)

print(f"{LEGEND1}: {len(data1)} entries")
print(f"{LEGEND2}: {len(data2)} entries")


# ============================================================
# Histogram
# ============================================================

bins = np.linspace(XRANGE[0], XRANGE[1], NBINS + 1)

count1, edges = np.histogram(data1, bins=bins)
count2, _ = np.histogram(data2, bins=bins)

N1 = np.sum(count1)
N2 = np.sum(count2)

# Normalize
hist1 = count1 / N1
hist2 = count2 / N2

# Statistical uncertainty
err1 = np.sqrt(count1) / N1
err2 = np.sqrt(count2) / N2

centers = 0.5 * (edges[:-1] + edges[1:])


# ============================================================
# Ratio + statistical uncertainty
# ============================================================

ratio = np.full_like(hist1, np.nan, dtype=float)
ratio_err = np.full_like(hist1, np.nan, dtype=float)

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

color1 = "C0"
color2 = "C1"


# ------------------------------------------------------------
# Main distribution
# ------------------------------------------------------------

# Sample 1
ax.stairs(
    hist1,
    edges,
    edgecolor=color1,
    facecolor=color1,
    linewidth=1.8,
    fill=True,
    alpha=0.30
)

# Sample 2: transparent fill
ax.stairs(
    hist2,
    edges,
    edgecolor=color2,
    facecolor=color2,
    linewidth=1.8,
    fill=True,
    alpha=0.25
)

# Sample 2: colored hatch overlay
ax.stairs(
    hist2,
    edges,
    edgecolor=color2,
    facecolor="none",
    linewidth=1.8,
    fill=True,
    hatch="//"
)


# ------------------------------------------------------------
# Custom legend
# ------------------------------------------------------------

legend_handles = [
    Patch(
        facecolor=color1,
        edgecolor=color1,
        alpha=0.30,
        label=LEGEND1
    ),
    Patch(
        facecolor=color2,
        edgecolor=color2,
        alpha=0.30,
        hatch="//",
        label=LEGEND2
    )
]

ax.legend(handles=legend_handles)

ax.set_ylabel("Normalized entries")
ax.grid(alpha=0.2)


# ------------------------------------------------------------
# Log scale option
# ------------------------------------------------------------

if LOG_SCALE:
    ax.set_yscale("log")


# ------------------------------------------------------------
# Ratio
# ------------------------------------------------------------

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

ax_ratio.set_ylabel("MC15ri / MC16ri", fontsize=10)
ax_ratio.set_xlabel(VARIABLE)

ax_ratio.set_ylim(0.5, 1.5)
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