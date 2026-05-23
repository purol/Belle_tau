#!/bin/sh

export Belle_tau_DIR="/home/belle2/junewoo/storage_b2/tau_workspace/Belle_tau" # analysis code path
Code="${Belle_tau_DIR}/python/tau_mumumu_TauToMuMuMu.py"
mdst_path="/home/belle2/junewoo/storage_ghi/tau_mdst/SIGNAL/"

# 4S
enery_name="MC16rd_on"
mkdir -p "./log_${enery_name}"
mkdir -p "./err_${enery_name}"
mkdir -p "./output_${enery_name}"
if compgen -G "${mdst_path}/${enery_name}/*.root" > /dev/null; then
  for file in "${mdst_path}/${enery_name}"/*.root; do
    filename=$(basename "$file" .root) # without path, without extension
    bsub -q s -J SKIM -o "./log_${enery_name}/${filename}_SKIM.log" -e "./err_${enery_name}/${filename}_SKIM.err" ${Code} --input_file "${mdst_path}/${enery_name}/${filename}.root" --output_file "./output_${enery_name}/SKIM_${filename}.root"
  done
fi

# off
enery_name="MC16rd_off"
mkdir -p "./log_${enery_name}"
mkdir -p "./err_${enery_name}"
mkdir -p "./output_${enery_name}"
if compgen -G "${mdst_path}/${enery_name}/*.root" > /dev/null; then
  for file in "${mdst_path}/${enery_name}"/*.root; do
    filename=$(basename "$file" .root) # without path, without extension
    bsub -q s -J SKIM -o "./log_${enery_name}/${filename}_SKIM.log" -e "./err_${enery_name}/${filename}_SKIM.err" ${Code} --input_file "${mdst_path}/${enery_name}/${filename}.root" --output_file "./output_${enery_name}/SKIM_${filename}.root"
  done
fi

# 5S
enery_name="MC16rd_5S"
mkdir -p "./log_${enery_name}"
mkdir -p "./err_${enery_name}"
mkdir -p "./output_${enery_name}"
if compgen -G "${mdst_path}/${enery_name}/*.root" > /dev/null; then
  for file in "${mdst_path}/${enery_name}"/*.root; do
    filename=$(basename "$file" .root) # without path, without extension
    bsub -q s -J SKIM -o "./log_${enery_name}/${filename}_SKIM.log" -e "./err_${enery_name}/${filename}_SKIM.err" ${Code} --input_file "${mdst_path}/${enery_name}/${filename}.root" --output_file "./output_${enery_name}/SKIM_${filename}.root"
  done
fi