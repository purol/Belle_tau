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

def select_variables(summary_df, train_df, region_name):
    """
    Selects variables based on separation, correlation with M and deltaE,
    and correlation with already selected variables.
    """
    print(f"\n--- Selecting variables for Region {region_name} ---")

    # Sort variables by separation in descending order
    sorted_summary = summary_df.sort_values(by="separation", ascending=False)

    # Separate signal and background dataframes
    signal_df = train_df[train_df["label"] == 1]
    bkg_df = train_df[train_df["label"] == 0]

    selected_variables = []

    for index, row in sorted_summary.iterrows():
        candidate_var = row["varname"]

        # 1. Check correlation with M and deltaE
        if (
            abs(row["signal_spea_M"]) < 0.1 and
            abs(row["signal_spea_deltaE"]) < 0.1 and
            abs(row["bkg_spea_M"]) < 0.1 and
            abs(row["bkg_spea_deltaE"]) < 0.1 and
            abs(row["signal_xi_M"]) < 0.1 and
            abs(row["signal_xi_deltaE"]) < 0.1 and
            abs(row["bkg_xi_M"]) < 0.1 and
            abs(row["bkg_xi_deltaE"]) < 0.1
        ):
            is_correlated_with_selected = False
            # 2. Check correlation with already selected variables
            for selected_var in selected_variables:
                # Spearman correlation
                bkg_spearman_corr = spearmanr(bkg_df[candidate_var], bkg_df[selected_var]).correlation
                signal_spearman_corr = spearmanr(signal_df[candidate_var], signal_df[selected_var]).correlation

                # Chatterjee's Xi correlation
                bkg_xi_corr = symmetric_xi(bkg_df[candidate_var].values, bkg_df[selected_var].values)
                signal_xi_corr = symmetric_xi(signal_df[candidate_var].values,signal_df[selected_var].values)

                if (
                    (abs(bkg_spearman_corr) > 0.8 and abs(signal_spearman_corr) > 0.8) or
                    (abs(bkg_xi_corr) > 0.8 and abs(signal_xi_corr) > 0.8)
                ):
                    is_correlated_with_selected = True
                    break  # No need to check other selected variables

            if not is_correlated_with_selected:
                selected_variables.append(candidate_var)

    print(f"Selected {len(selected_variables)} variables for Region {region_name}:")
    print(selected_variables)
    return selected_variables

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

def summarize_variable_metrics(df, bins=1000, skip_cols=["label", "weight"]):
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

            signal_spea_M = spearmanr(signal_df[feature], signal_df["M"]).correlation if "M" in signal_df.columns else np.nan
            signal_spea_de  = spearmanr(signal_df[feature], signal_df["deltaE"]).correlation if "deltaE" in signal_df.columns else np.nan
            bkg_spea_M = spearmanr(bkg_df[feature], bkg_df["M"]).correlation if "M" in bkg_df.columns else np.nan
            bkg_spea_de  = spearmanr(bkg_df[feature], bkg_df["deltaE"]).correlation if "deltaE" in bkg_df.columns else np.nan

            signal_xi_M = max(chatterjeexi(signal_df[feature].values, signal_df["M"].values).statistic, chatterjeexi(signal_df["M"].values, signal_df[feature].values).statistic) if "M" in signal_df.columns else np.nan
            signal_xi_de  = max(chatterjeexi(signal_df[feature].values, signal_df["deltaE"].values).statistic, chatterjeexi(signal_df["deltaE"].values, signal_df[feature].values).statistic) if "deltaE" in signal_df.columns else np.nan
            bkg_xi_M = max(chatterjeexi(bkg_df[feature].values, bkg_df["M"].values).statistic, chatterjeexi(bkg_df["M"].values, bkg_df[feature].values).statistic) if "M" in bkg_df.columns else np.nan
            bkg_xi_de  = max(chatterjeexi(bkg_df[feature].values, bkg_df["deltaE"].values).statistic, chatterjeexi(bkg_df["deltaE"].values, bkg_df[feature].values).statistic) if "deltaE" in bkg_df.columns else np.nan

            results.append({
                "varname": feature,
                "separation": sep,
                "signal_spea_M": signal_spea_M,
                "signal_spea_deltaE": signal_spea_de,
                "bkg_spea_M": bkg_spea_M,
                "bkg_spea_deltaE": bkg_spea_de,
                "signal_xi_M": signal_xi_M,
                "signal_xi_deltaE": signal_xi_de,
                "bkg_xi_M": bkg_xi_M,
                "bkg_xi_deltaE": bkg_xi_de
            })
        except Exception as e:
            print(f"Skipping {feature} due to error: {e}")

    return pd.DataFrame(results).sort_values(by="separation", ascending=False)

def create_and_plot_correlation_matrices(df, summary_df, region_name, separation_thres=-1, n_top=300):
    print(f"\n--- Generating Correlation Matrices for Region {region_name} ---")

    # --- 1. Select variables by separation threshold and correlations ---
    filtered_summary = summary_df[
    (summary_df["separation"] >= separation_thres) &

    (summary_df["signal_spea_M"].abs() < 0.1) &
    (summary_df["signal_spea_deltaE"].abs() < 0.1) &
    (summary_df["bkg_spea_M"].abs() < 0.1) &
    (summary_df["bkg_spea_deltaE"].abs() < 0.1) &

    (summary_df["signal_xi_M"].abs() < 0.1) &
    (summary_df["signal_xi_deltaE"].abs() < 0.1) &
    (summary_df["bkg_xi_M"].abs() < 0.1) &
    (summary_df["bkg_xi_deltaE"].abs() < 0.1)
    ]

    if filtered_summary.empty:
        print(f"No variables passed separation >= {separation_thres}")
        return

    # --- 2. Select top variables ---
    filtered_summary = filtered_summary.sort_values(by="separation", ascending=False).head(n_top)
    top_vars = filtered_summary["varname"].tolist()
    df_top = df[top_vars]
    print(f"Selected top {len(top_vars)} variables with highest separation.")

    # --- 3. Calculate Spearman correlation matrix ---
    print("Calculating Spearman correlation matrix...")
    spearman_corr = df_top.corr(method='spearman')

    # --- 4. Plot Spearman heatmap ---
    plt.figure(figsize=(20, 18))
    sns.heatmap(spearman_corr, annot=False, cmap='viridis', fmt=".2f")
    plt.title(f'Spearman Correlation Matrix (Top {n_top} Variables) - Region {region_name}', fontsize=16)
    plt.xticks(rotation=90)
    plt.yticks(rotation=0)
    plt.tight_layout()
    spearman_filename = f'spearman_correlation_heatmap_region_{region_name}.png'
    plt.savefig(spearman_filename)
    plt.close()
    print(f"Saved Spearman heatmap to {spearman_filename}")

    # --- 5. csv Spearman heatmap ---
    spearman_corr.to_csv(f'spearman_correlation_heatmap_region_{region_name}.csv')

    # --- 6. Calculate Chatterjee's Xi correlation matrix ---
    print("Calculating Chatterjee's Xi correlation matrix (this may take a while)...")
    xi_corr_matrix = pd.DataFrame(index=top_vars, columns=top_vars, dtype=np.float64)

    # Use tqdm for progress tracking
    for i in tqdm.tqdm(range(len(top_vars)), desc="Calculating Xi"):
        for j in range(i, len(top_vars)):
            var1 = top_vars[i]
            var2 = top_vars[j]
            if i == j:
                xi_corr_matrix.loc[var1, var2] = 1.0
            else:
                # Ensure data is numpy array for chatterjeexi
                x = df_top[var1].values
                y = df_top[var2].values
                try:
                    xi_xy = chatterjeexi(x, y).statistic
                    xi_yx = chatterjeexi(y, x).statistic
                    xi_val = max(xi_xy, xi_yx)
                    xi_corr_matrix.loc[var1, var2] = xi_val
                    xi_corr_matrix.loc[var2, var1] = xi_val # set just symmetric
                except Exception as e:
                    print(f"Could not calculate Xi for ({var1}, {var2}): {e}")
                    xi_corr_matrix.loc[var1, var2] = np.nan
                    xi_corr_matrix.loc[var2, var1] = np.nan

    # --- 7. Plot Xi heatmap ---
    plt.figure(figsize=(20, 18))
    sns.heatmap(xi_corr_matrix, annot=False, cmap='plasma', fmt=".2f")
    plt.title(f"Chatterjee's Xi Correlation Matrix (Top {n_top} Variables) - Region {region_name}", fontsize=16)
    plt.xticks(rotation=90)
    plt.yticks(rotation=0)
    plt.tight_layout()
    xi_filename = f'xi_correlation_heatmap_region_{region_name}.png'
    plt.savefig(xi_filename)
    plt.close()
    print(f"Saved Chatterjee's Xi heatmap to {xi_filename}")

    # --- 8. csv Xi heatmap ---
    xi_corr_matrix.to_csv(f'xi_correlation_heatmap_region_{region_name}.csv')

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
    help='List of removed variables. This variables are read but not used to calculate separation power and correlation.'
)
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
df_train = df_train.drop(columns=removed_variables, errors='ignore')
df_test = df_test.drop(columns=removed_variables, errors='ignore')

# ====================================================== region one ====================================================== #
# filter
df_train_one = df_train[((resolution["deltaE"]["peak"] - 5*resolution["deltaE"]["left_sigma"]) < df_train["deltaE"]) & (df_train["deltaE"] < (resolution["deltaE"]["peak"] + 5*resolution["deltaE"]["right_sigma"]))]
df_train_one = df_train_one[((resolution["M"]["peak"] - 5*resolution["M"]["left_sigma"]) < df_train_one["M"]) & (df_train_one["M"] < (resolution["M"]["peak"] + 5*resolution["M"]["right_sigma"]))]
df_test_one = df_test[((resolution["deltaE"]["peak"] - 5*resolution["deltaE"]["left_sigma"]) < df_test["deltaE"]) & (df_test["deltaE"] < (resolution["deltaE"]["peak"] + 5*resolution["deltaE"]["right_sigma"]))]
df_test_one = df_test_one[((resolution["M"]["peak"] - 5*resolution["M"]["left_sigma"]) < df_test_one["M"]) & (df_test_one["M"] < (resolution["M"]["peak"] + 5*resolution["M"]["right_sigma"]))]

summary_result = summarize_variable_metrics(df_train_one)
print(summary_result)
summary_result.to_csv("Importance_one.csv")
create_and_plot_correlation_matrices(df_train_one[df_train_one["label"] == 1], summary_result, "one_signal")
create_and_plot_correlation_matrices(df_train_one[df_train_one["label"] == 0], summary_result, "one_bkg")

# Select variables for region one
selected_vars_one = select_variables(summary_result, df_train_one, "one")

# ====================================================== region two ====================================================== #
# filter
df_train_two = df_train[((resolution["deltaE"]["peak"] - 15*resolution["deltaE"]["left_sigma"]) < df_train["deltaE"]) & (df_train["deltaE"] < (resolution["deltaE"]["peak"] - 5*resolution["deltaE"]["left_sigma"]))]
df_train_two = df_train_two[((resolution["M"]["peak"] - 5*resolution["M"]["left_sigma"]) < df_train_two["M"]) & (df_train_two["M"] < (resolution["M"]["peak"] + 5*resolution["M"]["right_sigma"]))]
df_test_two = df_test[((resolution["deltaE"]["peak"] - 15*resolution["deltaE"]["left_sigma"]) < df_test["deltaE"]) & (df_test["deltaE"] < (resolution["deltaE"]["peak"] - 5*resolution["deltaE"]["left_sigma"]))]
df_test_two = df_test_two[((resolution["M"]["peak"] - 5*resolution["M"]["left_sigma"]) < df_test_two["M"]) & (df_test_two["M"] < (resolution["M"]["peak"] + 5*resolution["M"]["right_sigma"]))]

summary_result = summarize_variable_metrics(df_train_two)
print(summary_result)
summary_result.to_csv("Importance_two.csv")
create_and_plot_correlation_matrices(df_train_two[df_train_two["label"] == 1], summary_result, "two_signal")
create_and_plot_correlation_matrices(df_test_two[df_test_two["label"] == 0], summary_result, "two_bkg")

# Select variables for region two
selected_vars_two = select_variables(summary_result, df_train_two, "two")