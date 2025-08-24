#!/usr/bin/env python3
import uproot
import pandas as pd
import os
import argparse
from typing import List, Optional, Union
from autogluon.tabular import TabularPredictor
from concurrent.futures import ThreadPoolExecutor
import tqdm
import sys

# =========== scale factor for MC15ri ===========
Scale_CHG_MC15ri = (0.36537/1.0)
Scale_MIX_MC15ri = (0.36537/1.0)
Scale_UUBAR_MC15ri = (0.36537/8.0)
Scale_DDBAR_MC15ri = (0.36537/8.0)
Scale_SSBAR_MC15ri = (0.36537/8.0)
Scale_CHARM_MC15ri = (0.36537/8.0)
Scale_MUMU_MC15ri = (0.36537/1.0)
Scale_EE_MC15ri = (0.36537/0.1)
Scale_EEEE_MC15ri = (0.36537/0.2)
Scale_EEMUMU_MC15ri = (0.36537/0.2)
Scale_EEPIPI_MC15ri = (0.36537/1.0)
Scale_EEKK_MC15ri = (0.36537/2.0)
Scale_EEPP_MC15ri = (0.36537/2.0)
Scale_PIPIISR_MC15ri = (0.36537/2.0)
Scale_KKISR_MC15ri = (0.36537/2.0)
Scale_GG_MC15ri = (0.36537/0.5)
Scale_EETAUTAU_MC15ri = (0.36537/2.0)
Scale_K0K0BARISR_MC15ri = (0.36537/2.0)
Scale_MUMUMUMU_MC15ri = (0.36537/2.0)
Scale_MUMUTAUTAU_MC15ri = (0.36537/2.0)
Scale_TAUTAUTAUTAU_MC15ri = (0.36537/10.0)
Scale_TAUPAIR_MC15ri = (0.36537/1.455052)

tau_crosssection = 0.919 # nb
Nevt_taupair = ((0.36537/0.000000001) * tau_crosssection)
Nevt_SIGNAL_MC15ri = 5000000
BR_SIGNAL = 0.00000001 # just set 10^(-8) 
Nevt_SIGNAL = (Nevt_taupair * BR_SIGNAL * 2.0)
Scale_SIGNAL_MC15ri = (Nevt_SIGNAL/Nevt_SIGNAL_MC15ri)

def ObtainWeight(dir):
    if dir == "SIGNAL":
        return Scale_SIGNAL_MC15ri
    elif dir == "CHARM":
        return Scale_CHARM_MC15ri
    elif dir == "CHG":
        return Scale_CHG_MC15ri
    elif dir == "DDBAR":
        return Scale_DDBAR_MC15ri
    elif dir == "EE":
        return Scale_EE_MC15ri
    elif dir == "EEEE":
        return Scale_EEEE_MC15ri
    elif dir == "EEKK":
        return Scale_EEKK_MC15ri
    elif dir == "EEMUMU":
        return Scale_EEMUMU_MC15ri
    elif dir == "EEPIPI":
        return Scale_EEPIPI_MC15ri
    elif dir == "EEPP":
        return Scale_EEPP_MC15ri
    elif dir == "EETAUTAU":
        return Scale_EETAUTAU_MC15ri
    elif dir == "GG":
        return Scale_GG_MC15ri
    elif dir == "K0K0BARISR":
        return Scale_K0K0BARISR_MC15ri
    elif dir == "KKISR":
        return Scale_KKISR_MC15ri
    elif dir == "MIX":
        return Scale_MIX_MC15ri
    elif dir == "MUMU":
        return Scale_MUMU_MC15ri
    elif dir == "MUMUMUMU":
        return Scale_MUMUMUMU_MC15ri
    elif dir == "MUMUTAUTAU":
        return Scale_MUMUTAUTAU_MC15ri
    elif dir == "PIPIISR":
        return Scale_PIPIISR_MC15ri
    elif dir == "SSBAR":
        return Scale_SSBAR_MC15ri
    elif dir == "TAUPAIR":
        return Scale_TAUPAIR_MC15ri
    elif dir == "TAUTAUTAUTAU":
        return Scale_TAUTAUTAUTAU_MC15ri
    elif dir == "UUBAR":
        return Scale_UUBAR_MC15ri
    else:
        sys.exit(1)

parser = argparse.ArgumentParser()
parser.add_argument(
    '--input_variables', 
    nargs='+',  # Accepts one or more values
    default=[
        "roeM__bocleanMask__bc",
        "roeDeltae__bocleanMask__bc",
        "first_muon_p",
        "second_muon_p",
        "third_muon_p",
        "thrustBm__bocleanMask__bc",
        "thrustOm__bocleanMask__bc",
        "nParticlesInList__bogamma__clevtshape_kinematics__bc",
        "cosAngleBetweenMomentumAndVertexVector",
        "nParticlesInList__bopi__pl__clevtshape_kinematics__bc",
        "charge_times_ROEcharge",
        "flightTime_dividedby_flightTimeErr",
        "missingEnergyOfEventCMS",
        "missingMass2OfEvent",
        "missingMomentumOfEventCMS_theta",
        "thrust",
        "cosTBTO__bocleanMask__bc",
        "chiProb",
        "angleToClosestInList__bopi__pl__clevtshape_kinematics__bc",
        "KSFWVariables__boet__cm__spcleanMask__bc",
        "KSFWVariables__bomm2__cm__spcleanMask__bc",
        "KSFWVariables__bohso00__cm__spcleanMask__bc",
        "KSFWVariables__bohso01__cm__spcleanMask__bc",
        "KSFWVariables__bohso02__cm__spcleanMask__bc",
        "KSFWVariables__bohso03__cm__spcleanMask__bc",
        "KSFWVariables__bohso04__cm__spcleanMask__bc",
        "KSFWVariables__bohso10__cm__spcleanMask__bc",
        "KSFWVariables__bohso12__cm__spcleanMask__bc",
        "KSFWVariables__bohso14__cm__spcleanMask__bc",
        "KSFWVariables__bohso20__cm__spcleanMask__bc",
        "KSFWVariables__bohso22__cm__spcleanMask__bc",
        "KSFWVariables__bohso24__cm__spcleanMask__bc",
        "harmonicMomentThrust0",
        "harmonicMomentThrust1",
        "harmonicMomentThrust2",
        "harmonicMomentThrust3",
        "harmonicMomentThrust4"
    ],
    help='List of input variables'
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
            "M_inv_tau": {"peak": ..., "left_sigma": ..., "right_sigma": ...},
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
        "M_inv_tau": {
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
            expressions=branches,
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

def read_with_weight(paths, label, tree_name, input_variables):
    df = read_all_root_files_self_function(dirs=paths, tree_name=tree_name, branches=input_variables)
    if not df.empty:
        df["weight"] = ObtainWeight(label)
    return df

signal_list = ["SIGNAL"]
background_list = ["CHARM", "CHG", "DDBAR", "EE", "EEEE", 
    "EEKK", "EEMUMU", "EEPIPI", "EEPP", "EETAUTAU", "GG", 
    "K0K0BARISR", "KKISR", "MIX", "MUMU", "MUMUMUMU", 
    "MUMUTAUTAU", "PIPIISR", "SSBAR", "TAUPAIR", "TAUTAUTAUTAU", "UUBAR"]

input_variables = args.input_variables
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
    
    df_train = read_with_weight(train_path, label, "tau_lfv", input_variables = input_variables + ["deltaE", "M_inv_tau"])
    df_test  = read_with_weight(test_path,  label, "tau_lfv", input_variables = input_variables + ["deltaE", "M_inv_tau"])

    df_SIGNAL_train_list.append(df_train)
    df_SIGNAL_test_list.append(df_test)

for label in background_list:
    train_path = [f"{input_path}/{label}/final_output_train/"]
    test_path = [f"{input_path}/{label}/final_output_test/"]
    
    df_train = read_with_weight(train_path, label, "tau_lfv", input_variables = input_variables + ["deltaE", "M_inv_tau"])
    df_test  = read_with_weight(test_path,  label, "tau_lfv", input_variables = input_variables + ["deltaE", "M_inv_tau"])

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
df_train_one = df_train_one[((resolution["M_inv_tau"]["peak"] - 5*resolution["M_inv_tau"]["left_sigma"]) < df_train_one["M_inv_tau"]) & (df_train_one["M_inv_tau"] < (resolution["M_inv_tau"]["peak"] + 5*resolution["M_inv_tau"]["right_sigma"]))]
df_train_one = df_train_one[input_variables + ["label"]]
df_test_one = df_test[((resolution["deltaE"]["peak"] - 5*resolution["deltaE"]["left_sigma"]) < df_test["deltaE"]) & (df_test["deltaE"] < (resolution["deltaE"]["peak"] + 5*resolution["deltaE"]["right_sigma"]))]
df_test_one = df_test_one[((resolution["M_inv_tau"]["peak"] - 5*resolution["M_inv_tau"]["left_sigma"]) < df_test_one["M_inv_tau"]) & (df_test_one["M_inv_tau"] < (resolution["M_inv_tau"]["peak"] + 5*resolution["M_inv_tau"]["right_sigma"]))]
df_test_one = df_test_one[input_variables + ["label"]]

# train
predictor = TabularPredictor(label="label", problem_type='binary', sample_weight = "weight", weight_evaluation = True, eval_metric='roc_auc', path=f"{input_path}/AutogluonModels/model_one").fit(
    df_train_one,
    time_limit=36000,
    presets="good_quality",
    save_bag_folds=True,
    refit_full=True,
    set_best_to_refit_full=False
)

# leaderboard
leaderboard_test = predictor.leaderboard(df_test_one, extra_metrics=[], silent=True)
leaderboard_train = predictor.leaderboard(df_train_one, extra_metrics=[], silent=True)
leaderboard_test_simplified = leaderboard_test[['model', 'score_test', 'score_val']]
leaderboard_train_simplified = leaderboard_train[['model', 'score_test']].rename(columns={'model': 'model', 'score_test': 'score_train'})
leaderboard = pd.merge(
    leaderboard_test_simplified,
    leaderboard_train_simplified,
    on='model',
    how='inner'  # or 'outer'/'left'/'right' depending on what you want
)

pd.set_option('display.max_rows', None)
pd.set_option('display.max_columns', None)
print(leaderboard)
pd.reset_option('display.max_rows')
pd.reset_option('display.max_columns')

feature = predictor.feature_importance(df_test_one)
print(feature)

# save model
predictor.save()

# ====================================================== region two ====================================================== #
# filter
df_train_two = df_train[(df_train["deltaE"] < (resolution["deltaE"]["peak"] - 5*resolution["deltaE"]["left_sigma"]))]
df_train_two = df_train_two[((resolution["M_inv_tau"]["peak"] - 3*resolution["M_inv_tau"]["left_sigma"]) < df_train_two["M_inv_tau"]) & (df_train_two["M_inv_tau"] < (resolution["M_inv_tau"]["peak"] + 3*resolution["M_inv_tau"]["right_sigma"]))]
df_train_two = df_train_two[input_variables + ["label"]]
df_test_two = df_test[(df_test["deltaE"] < (resolution["deltaE"]["peak"] - 5*resolution["deltaE"]["left_sigma"]))]
df_test_two = df_test_two[((resolution["M_inv_tau"]["peak"] - 3*resolution["M_inv_tau"]["left_sigma"]) < df_test_two["M_inv_tau"]) & (df_test_two["M_inv_tau"] < (resolution["M_inv_tau"]["peak"] + 3*resolution["M_inv_tau"]["right_sigma"]))]
df_test_two = df_test_two[input_variables + ["label"]]

# train
predictor = TabularPredictor(label="label", problem_type='binary', sample_weight = "weight", weight_evaluation = True, eval_metric='roc_auc', path=f"{input_path}/AutogluonModels/model_two").fit(
    df_train_two,
    time_limit=36000,
    presets="good_quality",
    save_bag_folds=True,
    refit_full=True,
    set_best_to_refit_full=False
)

# leaderboard
leaderboard_test = predictor.leaderboard(df_test_two, extra_metrics=[], silent=True)
leaderboard_train = predictor.leaderboard(df_train_two, extra_metrics=[], silent=True)
leaderboard_test_simplified = leaderboard_test[['model', 'score_test', 'score_val']]
leaderboard_train_simplified = leaderboard_train[['model', 'score_test']].rename(columns={'model': 'model', 'score_test': 'score_train'})
leaderboard = pd.merge(
    leaderboard_test_simplified,
    leaderboard_train_simplified,
    on='model',
    how='inner'  # or 'outer'/'left'/'right' depending on what you want
)

pd.set_option('display.max_rows', None)
pd.set_option('display.max_columns', None)
print(leaderboard)
pd.reset_option('display.max_rows')
pd.reset_option('display.max_columns')

feature = predictor.feature_importance(df_test_two)
print(feature)

# save model
predictor.save()  
