#!/usr/bin/env python3
import uproot
import pandas as pd
import os
import argparse
from typing import List, Optional, Union
from autogluon.tabular import TabularPredictor

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
parser.add_argument(
    '--input_file_name', 
    required=True,
    help='input file name'
)
parser.add_argument(
    '--output_path', 
    required=True,
    help='output path'
)
parser.add_argument(
    '--model_one', 
    required=True,
    help='model one file'
)
parser.add_argument(
    '--model_two', 
    required=True,
    help='model two file'
)
args = parser.parse_args()

def read_root_file(
    file_path: str,
    tree_name: str = "tau_lfv",
    branches: Optional[list[str]] = None
) -> pd.DataFrame:
    """
    Reads a single ROOT file, extracts specified branches from a given tree,
    and returns a DataFrame.

    Parameters:
    - file_path: str, path to the ROOT file.
    - tree_name: str, the name of the TTree inside the ROOT file.
    - branches: Optional[list[str]], list of branch names to read. If None, reads all branches.

    Returns:
    - pd.DataFrame containing the data from the specified tree.
    """
    try:
        with uproot.open(file_path) as file:
            if tree_name in file:
                tree = file[tree_name]
                df = tree.arrays(filter_name=branches, library="pd")
                return df
            else:
                print(f"Tree '{tree_name}' not found in file: {file_path}")
                return pd.DataFrame()
    except Exception as e:
        print(f"Error reading file {file_path}: {e}")
        return pd.DataFrame()

signal_list = ["SIGNAL"]
background_list = ["CHARM", "CHG", "DDBAR", "EE", "EEEE", 
    "EEKK", "EEMUMU", "EEPIPI", "EEPP", "EETAUTAU", "GG", 
    "K0K0BARISR", "KKISR", "MIX", "MUMU", "MUMUMUMU", 
    "MUMUTAUTAU", "PIPIISR", "SSBAR", "TAUPAIR", "TAUTAUTAUTAU", "UUBAR"]

input_variables = args.input_variables
input_path = args.input_path
input_file_name = args.input_file_name
output_path = args.output_path
model_one=args.model_one
model_two=args.model_two

# load the model
predictor_one = TabularPredictor.load(model_one)
predictor_two = TabularPredictor.load(model_two)

# apply model
df = read_root_file(input_path + "/" + input_file_name, branches=None)

X_ml = df[input_variables]
df["BDT_output_1"] = predictor_one.predict_proba(data = X_ml)[1]
df["BDT_output_2"] = predictor_two.predict_proba(data = X_ml)[1]

# write into CSV file (for the compatibility with C++ ROOT)
df[["__experiment__","__run__","__event__","__production__","__candidate__","__ncandidates__","BDT_output_1", "BDT_output_2"]].to_csv(output_path + "/" + input_file_name + ".csv", index=False)

#with uproot.recreate(output_path + "/" + input_file_name) as f_out:
#    f_out["tau_lfv"] = {col: df[col].to_numpy() for col in df.columns}