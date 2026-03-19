#!/usr/bin/env python3
import uproot
import pandas as pd
import os
import argparse
from typing import List, Optional, Union
from concurrent.futures import ThreadPoolExecutor
import tqdm
import sys
import numpy as np
from scipy.stats import spearmanr, chatterjeexi
import matplotlib.pyplot as plt
import seaborn as sns

from itertools import combinations
from concurrent.futures import ProcessPoolExecutor

# =========== scale factor for MC15ri ===========
# Luminosity (ab-1)
lumi_BelleII_4S = 0.36357
lumi_BelleII_off = 0.04228
lumi_BelleII_10810 = 0.00469

# --- Scale factors for BelleII 4S ---
Scale_BelleII_4S_CHG_MC15ri = (lumi_BelleII_4S/6.0)
Scale_BelleII_4S_MIX_MC15ri = (lumi_BelleII_4S/6.0)
Scale_BelleII_4S_UUBAR_MC15ri = (lumi_BelleII_4S/8.0)
Scale_BelleII_4S_DDBAR_MC15ri = (lumi_BelleII_4S/8.0)
Scale_BelleII_4S_SSBAR_MC15ri = (lumi_BelleII_4S/8.0)
Scale_BelleII_4S_CHARM_MC15ri = (lumi_BelleII_4S/8.0)
Scale_BelleII_4S_MUMU_MC15ri = (lumi_BelleII_4S/1.0)
Scale_BelleII_4S_EE_MC15ri = (lumi_BelleII_4S/0.1)
Scale_BelleII_4S_EEEE_MC15ri = (lumi_BelleII_4S/0.2)
Scale_BelleII_4S_EEMUMU_MC15ri = (lumi_BelleII_4S/0.2)
Scale_BelleII_4S_EEPIPI_MC15ri = (lumi_BelleII_4S/1.0)
Scale_BelleII_4S_EEKK_MC15ri = (lumi_BelleII_4S/2.0)
Scale_BelleII_4S_EEPP_MC15ri = (lumi_BelleII_4S/2.0)
Scale_BelleII_4S_PIPIISR_MC15ri = (lumi_BelleII_4S/2.0)
Scale_BelleII_4S_PIPIPI0ISR_MC15ri = (lumi_BelleII_4S/2.0)
Scale_BelleII_4S_KKISR_MC15ri = (lumi_BelleII_4S/2.0)
Scale_BelleII_4S_GG_MC15ri = (lumi_BelleII_4S/0.5)
Scale_BelleII_4S_EETAUTAU_MC15ri = (lumi_BelleII_4S/2.0)
Scale_BelleII_4S_K0K0BARISR_MC15ri = (lumi_BelleII_4S/2.0)
Scale_BelleII_4S_MUMUMUMU_MC15ri = (lumi_BelleII_4S/2.0)
Scale_BelleII_4S_MUMUTAUTAU_MC15ri = (lumi_BelleII_4S/2.0)
Scale_BelleII_4S_TAUTAUTAUTAU_MC15ri = (lumi_BelleII_4S/10.0)
Scale_BelleII_4S_TAUPAIR_MC15ri = (lumi_BelleII_4S/1.0)

# --- Scale factors for BelleII off-resonance ---
Scale_BelleII_off_UUBAR_MC15ri = (lumi_BelleII_off/0.05)
Scale_BelleII_off_DDBAR_MC15ri = (lumi_BelleII_off/0.05)
Scale_BelleII_off_SSBAR_MC15ri = (lumi_BelleII_off/0.05)
Scale_BelleII_off_CHARM_MC15ri = (lumi_BelleII_off/0.05)
Scale_BelleII_off_EE_MC15ri = (lumi_BelleII_off/0.005)
Scale_BelleII_off_EEEE_MC15ri = (lumi_BelleII_off/0.05)
Scale_BelleII_off_EEMUMU_MC15ri = (lumi_BelleII_off/0.05)
Scale_BelleII_off_EETAUTAU_MC15ri = (lumi_BelleII_off/0.5)
Scale_BelleII_off_EEPIPI_MC15ri = (lumi_BelleII_off/0.05)
Scale_BelleII_off_EEKK_MC15ri = (lumi_BelleII_off/0.05)
Scale_BelleII_off_EEPP_MC15ri = (lumi_BelleII_off/0.5)
Scale_BelleII_off_GG_MC15ri = (lumi_BelleII_off/0.005)
Scale_BelleII_off_MUMU_MC15ri = (lumi_BelleII_off/0.05)
Scale_BelleII_off_MUMUMUMU_MC15ri = (lumi_BelleII_off/0.5)
Scale_BelleII_off_TAUPAIR_MC15ri = (lumi_BelleII_off/0.05)

# --- Scale factors for BelleII 10810 ---
Scale_BelleII_10810_BBs_MC15ri = (lumi_BelleII_10810/0.046)
Scale_BelleII_10810_BsBs_MC15ri = (lumi_BelleII_10810/0.046)
Scale_BelleII_10810_CHG_MC15ri = (lumi_BelleII_10810/0.046)
Scale_BelleII_10810_MIX_MC15ri = (lumi_BelleII_10810/0.046)
Scale_BelleII_10810_UUBAR_MC15ri = (lumi_BelleII_10810/0.046)
Scale_BelleII_10810_DDBAR_MC15ri = (lumi_BelleII_10810/0.046)
Scale_BelleII_10810_SSBAR_MC15ri = (lumi_BelleII_10810/0.046)
Scale_BelleII_10810_CHARM_MC15ri = (lumi_BelleII_10810/0.046)
Scale_BelleII_10810_MUMU_MC15ri = (lumi_BelleII_10810/0.046)
Scale_BelleII_10810_TAUPAIR_MC15ri = (lumi_BelleII_10810/0.046)

# --- Signal Scale Factors ---
tau_crosssection_4S = 0.919 # nb
tau_crosssection_off = 0.929 # nb
tau_crosssection_10810 = 0.880 # nb

Nevt_taupair_BelleII_4S = ((lumi_BelleII_4S / 1e-9) * tau_crosssection_4S)
Nevt_taupair_BelleII_off = ((lumi_BelleII_off / 1e-9) * tau_crosssection_off)
Nevt_taupair_BelleII_10810 = ((lumi_BelleII_10810 / 1e-9) * tau_crosssection_10810)

Nevt_SIGNAL_BelleII_4S_MC15ri = 10000000
Nevt_SIGNAL_BelleII_off_MC15ri = 400000
Nevt_SIGNAL_BelleII_10810_MC15ri = 400000

BR_SIGNAL = 1e-8 # 10^(-8) 

Nevt_SIGNAL_BelleII_4S = (Nevt_taupair_BelleII_4S * BR_SIGNAL * 2.0)
Nevt_SIGNAL_BelleII_off = (Nevt_taupair_BelleII_off * BR_SIGNAL * 2.0)
Nevt_SIGNAL_BelleII_10810 = (Nevt_taupair_BelleII_10810 * BR_SIGNAL * 2.0)

Scale_SIGNAL_BelleII_4S_MC15ri = (Nevt_SIGNAL_BelleII_4S / Nevt_SIGNAL_BelleII_4S_MC15ri)
Scale_SIGNAL_BelleII_off_MC15ri = (Nevt_SIGNAL_BelleII_off / Nevt_SIGNAL_BelleII_off_MC15ri)
Scale_SIGNAL_BelleII_10810_MC15ri = (Nevt_SIGNAL_BelleII_10810 / Nevt_SIGNAL_BelleII_10810_MC15ri)

def calculate_symmetric_xi_for_pair(args):
    """
    Worker function for parallel execution.
    Calculates symmetric xi for a single pair of variable names.
    """
    df, var1, var2 = args
    try:
        # Using .values is crucial for performance with multiprocessing
        x = df[var1].values
        y = df[var2].values

        # Remove NaNs consistently before passing to the function
        mask = np.isfinite(x) & np.isfinite(y)
        if mask.sum() < 5: # Not enough data points to be meaningful
             return var1, var2, np.nan

        xi_xy = chatterjeexi(x[mask], y[mask]).statistic
        xi_yx = chatterjeexi(y[mask], x[mask]).statistic
        xi_val = max(xi_xy, xi_yx)
        return var1, var2, xi_val
    except Exception as e:
        # This will run in a separate process, so printing might be messy.
        # It's better to just return NaN on failure.
        return var1, var2, np.nan
        
def symmetric_xi(x, y):
    """
    Direction-robust Chatterjee's xi correlation.
    Returns max(xi(x->y), xi(y->x)).
    """
    x = np.asarray(x)
    y = np.asarray(y)

    # remove NaNs consistently
    mask = np.isfinite(x) & np.isfinite(y)
    if mask.sum() < 5:
        return np.nan

    xi_xy = chatterjeexi(x[mask], y[mask]).statistic
    xi_yx = chatterjeexi(y[mask], x[mask]).statistic

    return max(xi_xy, xi_yx)

def calculate_weights(df: pd.DataFrame) -> pd.Series:
    """
    Calculates weights for events based on SampleType, EnergyType, and EventType.
    This function is a Python implementation of the C++ logic.
    """
    # Define column names for clarity
    sample_type_col = 'MySampleType'
    event_type_col = 'MyEventType'
    energy_type_col = 'MyEnergyType' # Assuming this is the name

    # Conditions for MC15ri
    is_mc15ri = (df[sample_type_col] > 0.5) & (df[sample_type_col] < 1.5)
    
    # Energy type conditions
    is_4s = (df[energy_type_col] > 0.5) & (df[energy_type_col] < 1.5)
    is_off = (df[energy_type_col] > 1.5) & (df[energy_type_col] < 2.5)
    is_10810 = (df[energy_type_col] > 5.5) & (df[energy_type_col] < 6.5)

    # A list of all conditions and their corresponding weight choices
    conditions = [
        # Belle data
        (df[sample_type_col] > 4.5) & (df[sample_type_col] < 5.5),
        # Data
        (df[sample_type_col] > -1.5) & (df[sample_type_col] < -0.5),
        
        # MC15ri @ 4S
        is_mc15ri & is_4s & (df[event_type_col] > -0.5) & (df[event_type_col] < 0.5),   # SIGNAL
        is_mc15ri & is_4s & (df[event_type_col] > 0.5) & (df[event_type_col] < 1.5),    # CHG
        is_mc15ri & is_4s & (df[event_type_col] > 1.5) & (df[event_type_col] < 2.5),    # MIX
        is_mc15ri & is_4s & (df[event_type_col] > 2.5) & (df[event_type_col] < 3.5),    # UUBAR
        is_mc15ri & is_4s & (df[event_type_col] > 3.5) & (df[event_type_col] < 4.5),    # DDBAR
        is_mc15ri & is_4s & (df[event_type_col] > 4.5) & (df[event_type_col] < 5.5),    # SSBAR
        is_mc15ri & is_4s & (df[event_type_col] > 5.5) & (df[event_type_col] < 6.5),    # CHARM
        is_mc15ri & is_4s & (df[event_type_col] > 6.5) & (df[event_type_col] < 7.5),    # MUMU
        is_mc15ri & is_4s & (df[event_type_col] > 7.5) & (df[event_type_col] < 8.5),    # EE
        is_mc15ri & is_4s & (df[event_type_col] > 8.5) & (df[event_type_col] < 9.5),    # EEEE
        is_mc15ri & is_4s & (df[event_type_col] > 9.5) & (df[event_type_col] < 10.5),   # EEMUMU
        is_mc15ri & is_4s & (df[event_type_col] > 10.5) & (df[event_type_col] < 11.5),  # EEPIPI
        is_mc15ri & is_4s & (df[event_type_col] > 11.5) & (df[event_type_col] < 12.5),  # EEKK
        is_mc15ri & is_4s & (df[event_type_col] > 12.5) & (df[event_type_col] < 13.5),  # EEPP
        is_mc15ri & is_4s & (df[event_type_col] > 13.5) & (df[event_type_col] < 14.5),  # PIPIISR
        is_mc15ri & is_4s & (df[event_type_col] > 14.5) & (df[event_type_col] < 15.5),  # KKISR
        is_mc15ri & is_4s & (df[event_type_col] > 15.5) & (df[event_type_col] < 16.5),  # GG
        is_mc15ri & is_4s & (df[event_type_col] > 16.5) & (df[event_type_col] < 17.5),  # EETAUTAU
        is_mc15ri & is_4s & (df[event_type_col] > 17.5) & (df[event_type_col] < 18.5),  # K0K0BARISR
        is_mc15ri & is_4s & (df[event_type_col] > 18.5) & (df[event_type_col] < 19.5),  # MUMUMUMU
        is_mc15ri & is_4s & (df[event_type_col] > 19.5) & (df[event_type_col] < 20.5),  # MUMUTAUTAU
        is_mc15ri & is_4s & (df[event_type_col] > 20.5) & (df[event_type_col] < 21.5),  # TAUTAUTAUTAU
        is_mc15ri & is_4s & (df[event_type_col] > 21.5) & (df[event_type_col] < 22.5),  # TAUPAIR
        is_mc15ri & is_4s & (df[event_type_col] > 22.5) & (df[event_type_col] < 23.5),  # PIPIPI0ISR

        # MC15ri @ off-resonance
        is_mc15ri & is_off & (df[event_type_col] > -0.5) & (df[event_type_col] < 0.5),   # SIGNAL
        is_mc15ri & is_off & (df[event_type_col] > 2.5) & (df[event_type_col] < 3.5),   # UUBAR
        is_mc15ri & is_off & (df[event_type_col] > 3.5) & (df[event_type_col] < 4.5),   # DDBAR
        is_mc15ri & is_off & (df[event_type_col] > 4.5) & (df[event_type_col] < 5.5),   # SSBAR
        is_mc15ri & is_off & (df[event_type_col] > 5.5) & (df[event_type_col] < 6.5),   # CHARM
        is_mc15ri & is_off & (df[event_type_col] > 6.5) & (df[event_type_col] < 7.5),   # MUMU
        is_mc15ri & is_off & (df[event_type_col] > 7.5) & (df[event_type_col] < 8.5),   # EE
        is_mc15ri & is_off & (df[event_type_col] > 8.5) & (df[event_type_col] < 9.5),   # EEEE
        is_mc15ri & is_off & (df[event_type_col] > 9.5) & (df[event_type_col] < 10.5),  # EEMUMU
        is_mc15ri & is_off & (df[event_type_col] > 10.5) & (df[event_type_col] < 11.5), # EEPIPI
        is_mc15ri & is_off & (df[event_type_col] > 11.5) & (df[event_type_col] < 12.5), # EEKK
        is_mc15ri & is_off & (df[event_type_col] > 12.5) & (df[event_type_col] < 13.5), # EEPP
        is_mc15ri & is_off & (df[event_type_col] > 15.5) & (df[event_type_col] < 16.5), # GG
        is_mc15ri & is_off & (df[event_type_col] > 16.5) & (df[event_type_col] < 17.5), # EETAUTAU
        is_mc15ri & is_off & (df[event_type_col] > 18.5) & (df[event_type_col] < 19.5), # MUMUMUMU
        is_mc15ri & is_off & (df[event_type_col] > 21.5) & (df[event_type_col] < 22.5), # TAUPAIR
        
        # MC15ri @ 10810
        is_mc15ri & is_10810 & (df[event_type_col] > -0.5) & (df[event_type_col] < 0.5),   # SIGNAL
        is_mc15ri & is_10810 & (df[event_type_col] > 0.5) & (df[event_type_col] < 1.5),   # CHG
        is_mc15ri & is_10810 & (df[event_type_col] > 1.5) & (df[event_type_col] < 2.5),   # MIX
        is_mc15ri & is_10810 & (df[event_type_col] > 2.5) & (df[event_type_col] < 3.5),   # UUBAR
        is_mc15ri & is_10810 & (df[event_type_col] > 3.5) & (df[event_type_col] < 4.5),   # DDBAR
        is_mc15ri & is_10810 & (df[event_type_col] > 4.5) & (df[event_type_col] < 5.5),   # SSBAR
        is_mc15ri & is_10810 & (df[event_type_col] > 5.5) & (df[event_type_col] < 6.5),   # CHARM
        is_mc15ri & is_10810 & (df[event_type_col] > 6.5) & (df[event_type_col] < 7.5),   # MUMU
        is_mc15ri & is_10810 & (df[event_type_col] > 21.5) & (df[event_type_col] < 22.5), # TAUPAIR
        is_mc15ri & is_10810 & (df[event_type_col] > 23.5) & (df[event_type_col] < 24.5), # BBs
        is_mc15ri & is_10810 & (df[event_type_col] > 24.5) & (df[event_type_col] < 25.5), # BsBs
    ]
    
    choices = [
        1.0, # Belle data weight
        1.0, # Data weight
        
        Scale_SIGNAL_BelleII_4S_MC15ri,
        Scale_BelleII_4S_CHG_MC15ri,
        Scale_BelleII_4S_MIX_MC15ri,
        Scale_BelleII_4S_UUBAR_MC15ri,
        Scale_BelleII_4S_DDBAR_MC15ri,
        Scale_BelleII_4S_SSBAR_MC15ri,
        Scale_BelleII_4S_CHARM_MC15ri,
        Scale_BelleII_4S_MUMU_MC15ri,
        Scale_BelleII_4S_EE_MC15ri,
        Scale_BelleII_4S_EEEE_MC15ri,
        Scale_BelleII_4S_EEMUMU_MC15ri,
        Scale_BelleII_4S_EEPIPI_MC15ri,
        Scale_BelleII_4S_EEKK_MC15ri,
        Scale_BelleII_4S_EEPP_MC15ri,
        Scale_BelleII_4S_PIPIISR_MC15ri,
        Scale_BelleII_4S_KKISR_MC15ri,
        Scale_BelleII_4S_GG_MC15ri,
        Scale_BelleII_4S_EETAUTAU_MC15ri,
        Scale_BelleII_4S_K0K0BARISR_MC15ri,
        Scale_BelleII_4S_MUMUMUMU_MC15ri,
        Scale_BelleII_4S_MUMUTAUTAU_MC15ri,
        Scale_BelleII_4S_TAUTAUTAUTAU_MC15ri,
        Scale_BelleII_4S_TAUPAIR_MC15ri,
        Scale_BelleII_4S_PIPIPI0ISR_MC15ri,
        
        Scale_SIGNAL_BelleII_off_MC15ri,
        Scale_BelleII_off_UUBAR_MC15ri,
        Scale_BelleII_off_DDBAR_MC15ri,
        Scale_BelleII_off_SSBAR_MC15ri,
        Scale_BelleII_off_CHARM_MC15ri,
        Scale_BelleII_off_MUMU_MC15ri,
        Scale_BelleII_off_EE_MC15ri,
        Scale_BelleII_off_EEEE_MC15ri,
        Scale_BelleII_off_EEMUMU_MC15ri,
        Scale_BelleII_off_EEPIPI_MC15ri,
        Scale_BelleII_off_EEKK_MC15ri,
        Scale_BelleII_off_EEPP_MC15ri,
        Scale_BelleII_off_GG_MC15ri,
        Scale_BelleII_off_EETAUTAU_MC15ri,
        Scale_BelleII_off_MUMUMUMU_MC15ri,
        Scale_BelleII_off_TAUPAIR_MC15ri,
        
        Scale_SIGNAL_BelleII_10810_MC15ri,
        Scale_BelleII_10810_CHG_MC15ri,
        Scale_BelleII_10810_MIX_MC15ri,
        Scale_BelleII_10810_UUBAR_MC15ri,
        Scale_BelleII_10810_DDBAR_MC15ri,
        Scale_BelleII_10810_SSBAR_MC15ri,
        Scale_BelleII_10810_CHARM_MC15ri,
        Scale_BelleII_10810_MUMU_MC15ri,
        Scale_BelleII_10810_TAUPAIR_MC15ri,
        Scale_BelleII_10810_BBs_MC15ri,
        Scale_BelleII_10810_BsBs_MC15ri,
    ]

    # np.select is a vectorized and efficient way to perform this assignment
    return pd.Series(np.select(conditions, choices, default=0.0), index=df.index)

def plot_binned_1d_distributions(df, bdt_col, region_str, num_bins=4):
    bkg_df = df[df["label"] == 0].copy()
    
    # do not draw if there is not enough background
    if len(bkg_df) < num_bins:
        print(f"Not enough data in {region_str} to plot binned distributions.")
        return

    # divide BDT region
    # pd.qcut to put the same number of events
    try:
        bkg_df['BDT_bin'], bin_edges = pd.qcut(bkg_df[bdt_col], q=num_bins, retbins=True, duplicates='drop')
    except ValueError:
        print(f"Warning: Could not use qcut for {region_str}. Using equal-width bins instead.")
        bkg_df['BDT_bin'], bin_edges = pd.cut(bkg_df[bdt_col], bins=num_bins, retbins=True)

    fig, axes = plt.subplots(1, 2, figsize=(14, 6))
    
    # define color map
    colors = plt.cm.viridis(np.linspace(0, 0.9, len(bin_edges)-1))
    
    for i in range(len(bin_edges)-1):
        lower = bin_edges[i]
        upper = bin_edges[i+1]
        
        # masking the current region
        if i == len(bin_edges) - 2:
            mask = (bkg_df[bdt_col] >= lower) & (bkg_df[bdt_col] <= upper)
        else:
            mask = (bkg_df[bdt_col] >= lower) & (bkg_df[bdt_col] < upper)
            
        subset = bkg_df[mask]
        if len(subset) == 0:
            continue
            
        label_str = f"{lower:.3f} <= BDT < {upper:.3f} (N={len(subset)})"
        
        # 1. M distribution
        axes[0].hist(
            subset["M"], 
            bins=30, 
            weights=subset["weight"], 
            density=True,          
            histtype='step',      
            linewidth=2.5, 
            color=colors[i], 
            label=label_str
        )
        
        # 2. deltaE distribution
        axes[1].hist(
            subset["deltaE"], 
            bins=30, 
            weights=subset["weight"], 
            density=True,
            histtype='step', 
            linewidth=2.5, 
            color=colors[i], 
            label=label_str
        )

    # setting for plots
    axes[0].set_xlabel("M")
    axes[0].set_ylabel("Normalized Events (A.U.)")
    axes[0].set_title(f"[{region_str}] Weighted Shape of M across BDT Bins")
    axes[0].legend(loc='best', fontsize='small')

    axes[1].set_xlabel("deltaE")
    axes[1].set_ylabel("Normalized Events (A.U.)")
    axes[1].set_title(f"[{region_str}] Weighted Shape of deltaE across BDT Bins")
    axes[1].legend(loc='best', fontsize='small')

    plt.tight_layout()
    save_name = f"Binned_1D_Shape_{region_str}.png"
    plt.savefig(save_name, dpi=300)
    plt.close()
    print(f"Saved 1D shape comparison to: {save_name}\n")

def summarize_variable_metrics(df, bins=1000, skip_cols=["label", "weight"], isItFirstRegion = True):
    bkg_df = df[df["label"] == 0].copy()
    
    region_str = "Region_1" if isItFirstRegion else "Region_2"
    bdt_col = "BDT_output_1" if isItFirstRegion else "BDT_output_2"

    print(f"--- Processing {region_str} ---")

    fig, axes = plt.subplots(1, 2, figsize=(14, 6))
    
    # BDT vs M
    h1 = axes[0].hist2d(
        bkg_df[bdt_col], bkg_df["M"], 
        bins=50, 
        weights=bkg_df["weight"], 
        cmap='Blues', 
        cmin=1e-10
    )
    axes[0].set_xlabel(bdt_col)
    axes[0].set_ylabel("M")
    axes[0].set_title(f"Weighted 2D Hist: {bdt_col} vs M")
    fig.colorbar(h1[3], ax=axes[0], label="Weighted Events")

    # BDT vs deltaE
    h2 = axes[1].hist2d(
        bkg_df[bdt_col], bkg_df["deltaE"], 
        bins=50, 
        weights=bkg_df["weight"], 
        cmap='Oranges', 
        cmin=1e-10
    )
    axes[1].set_xlabel(bdt_col)
    axes[1].set_ylabel("deltaE")
    axes[1].set_title(f"Weighted 2D Hist: {bdt_col} vs deltaE")
    fig.colorbar(h2[3], ax=axes[1], label="Weighted Events")

    plt.tight_layout()

    save_name = f"Correlation_2D_{region_str}.png"
    plt.savefig(save_name, dpi=300)
    plt.close()
    print(f"Saved 2D distributions to: {save_name}")

    spea_M  = spearmanr(bkg_df[bdt_col], bkg_df["M"])[0]
    spea_de = spearmanr(bkg_df[bdt_col], bkg_df["deltaE"])[0]
    
    xi_M = max(
        chatterjeexi(bkg_df[bdt_col].values, bkg_df["M"].values).statistic, 
        chatterjeexi(bkg_df["M"].values, bkg_df[bdt_col].values).statistic
    )
    xi_de = max(
        chatterjeexi(bkg_df[bdt_col].values, bkg_df["deltaE"].values).statistic, 
        chatterjeexi(bkg_df["deltaE"].values, bkg_df[bdt_col].values).statistic
    )

    print(f"[{region_str}] Weighted Correlation:")
    print(f"  Spearman M: {spea_M:.4f}, Spearman deltaE: {spea_de:.4f}")
    print(f"  Xi M: {xi_M:.4f}, Xi deltaE: {xi_de:.4f}\n")

    return {
        "Spearman_M": spea_M,
        "Spearman_deltaE": spea_de,
        "Xi_M": xi_M,
        "Xi_deltaE": xi_de
    }

parser = argparse.ArgumentParser()
parser.add_argument(
    '--input_path', 
    required=True,
    help='input path'
)
args = parser.parse_args()

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

signal_list = ["SIGNAL"]
background_list = ["BBs", "BsBs", "CHARM", "CHG", "DDBAR", "EE", "EEEE", 
    "EEKK", "EEMUMU", "EEPIPI", "EEPP", "EETAUTAU", "GG", 
    "K0K0BARISR", "KKISR", "MIX", "MUMU", "MUMUMUMU", 
    "MUMUTAUTAU", "PIPIPI0ISR", "PIPIISR", "SSBAR", "TAUPAIR", "TAUTAUTAUTAU", "UUBAR"]

input_path = args.input_path

SIGNAL_train_path = [f"{input_path}/{e}/final_output_train_after_application/" for e in signal_list]
SIGNAL_test_path = [f"{input_path}/{e}/final_output_test_after_application/" for e in signal_list]
BKG_train_path = [f"{input_path}/{e}/final_output_train_after_application/" for e in background_list]
BKG_test_path = [f"{input_path}/{e}/final_output_test_after_application/" for e in background_list]

resolution = ReadResolution(f"{input_path}/M_deltaE_result.txt")

# read ROOT files
df_SIGNAL_train_list = []
df_SIGNAL_test_list  = []
df_BKG_train_list = []
df_BKG_test_list = []

for label in signal_list:
    train_path = [f"{input_path}/{label}/final_output_train_after_application/"]
    test_path = [f"{input_path}/{label}/final_output_test_after_application/"]
    
    df_train = read_with_weight(train_path, "tau_lfv", input_variables = ["M", "deltaE", "BDT_output_1", "BDT_output_2", "MySampleType", "MyEventType", "MyEnergyType"])
    df_test  = read_with_weight(test_path,  "tau_lfv", input_variables = ["M", "deltaE", "BDT_output_1", "BDT_output_2", "MySampleType", "MyEventType", "MyEnergyType"])

    df_SIGNAL_train_list.append(df_train)
    df_SIGNAL_test_list.append(df_test)

for label in background_list:
    train_path = [f"{input_path}/{label}/final_output_train_after_application/"]
    test_path = [f"{input_path}/{label}/final_output_test_after_application/"]
    
    df_train = read_with_weight(train_path, "tau_lfv", input_variables = ["M", "deltaE", "BDT_output_1", "BDT_output_2", "MySampleType", "MyEventType", "MyEnergyType"])
    df_test  = read_with_weight(test_path, "tau_lfv", input_variables = ["M", "deltaE", "BDT_output_1", "BDT_output_2", "MySampleType", "MyEventType", "MyEnergyType"])

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

# ====================================================== region one ====================================================== #
# filter
df_train_one = df_train[((resolution["deltaE"]["peak"] - 5*resolution["deltaE"]["left_sigma"]) < df_train["deltaE"]) & (df_train["deltaE"] < (resolution["deltaE"]["peak"] + 5*resolution["deltaE"]["right_sigma"]))]
df_train_one = df_train_one[((resolution["M"]["peak"] - 20*resolution["M"]["left_sigma"]) < df_train_one["M"]) & (df_train_one["M"] < (resolution["M"]["peak"] + 20*resolution["M"]["right_sigma"]))]
df_train_one = df_train_one[0.3 < df_train_one["BDT_output_1"]]
df_test_one = df_test[((resolution["deltaE"]["peak"] - 5*resolution["deltaE"]["left_sigma"]) < df_test["deltaE"]) & (df_test["deltaE"] < (resolution["deltaE"]["peak"] + 5*resolution["deltaE"]["right_sigma"]))]
df_test_one = df_test_one[((resolution["M"]["peak"] - 20*resolution["M"]["left_sigma"]) < df_test_one["M"]) & (df_test_one["M"] < (resolution["M"]["peak"] + 20*resolution["M"]["right_sigma"]))]
df_test_one = df_test_one[0.3 < df_test_one["BDT_output_1"]]

summary_one = summarize_variable_metrics(df_train_one, isItFirstRegion = True)
print("%f %f %f %f" % (summary_one['Spearman_M'], summary_one['Spearman_deltaE'], summary_one['Xi_M'], summary_one['Xi_deltaE']))

plot_binned_1d_distributions(df_train_one, "BDT_output_1", "Region_1", num_bins=4)

# ====================================================== region two ====================================================== #
# filter
df_train_two = df_train[((resolution["deltaE"]["peak"] - 15*resolution["deltaE"]["left_sigma"]) < df_train["deltaE"]) & (df_train["deltaE"] < (resolution["deltaE"]["peak"] - 5*resolution["deltaE"]["left_sigma"]))]
df_train_two = df_train_two[((resolution["M"]["peak"] - 20*resolution["M"]["left_sigma"]) < df_train_two["M"]) & (df_train_two["M"] < (resolution["M"]["peak"] + 20*resolution["M"]["right_sigma"]))]
df_train_two = df_train_two[0.3 < df_train_two["BDT_output_2"]]
df_test_two = df_test[((resolution["deltaE"]["peak"] - 15*resolution["deltaE"]["left_sigma"]) < df_test["deltaE"]) & (df_test["deltaE"] < (resolution["deltaE"]["peak"] - 5*resolution["deltaE"]["left_sigma"]))]
df_test_two = df_test_two[((resolution["M"]["peak"] - 20*resolution["M"]["left_sigma"]) < df_test_two["M"]) & (df_test_two["M"] < (resolution["M"]["peak"] + 20*resolution["M"]["right_sigma"]))]
df_test_two = df_test_two[0.3 < df_test_two["BDT_output_2"]]

summary_two = summarize_variable_metrics(df_train_two, isItFirstRegion = False)
print("%f %f %f %f" % (summary_two['Spearman_M'], summary_two['Spearman_deltaE'], summary_two['Xi_M'], summary_two['Xi_deltaE']))

plot_binned_1d_distributions(df_train_two, "BDT_output_2", "Region_2", num_bins=4)