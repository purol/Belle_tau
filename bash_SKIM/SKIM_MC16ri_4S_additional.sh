#!/bin/sh

export Belle_tau_DIR="/home/belle2/junewoo/storage_b2/tau_workspace/Belle_tau" # analysis code path
Code="${Belle_tau_DIR}/python/tau_mumumu_TauToMuMuMu.py"


# UUBAR
sample="UUBAR"
mdst_path="/home/belle2/junewoo/storage_ghi/tau_gen_cut/MC16ri/MC16ri_private_gencut/${sample}/"
enery_name="MC16ri_on"
mkdir -p "./log_${sample}_${enery_name}"
mkdir -p "./err_${sample}_${enery_name}"
mkdir -p "./output_${sample}_${enery_name}"
if compgen -G "${mdst_path}/${enery_name}/*.root" > /dev/null; then
  for file in "${mdst_path}/${enery_name}"/*.root; do
    filename=$(basename "$file" .root) # without path, without extension
    bsub -q s \
    -J SKIM \
    -o "./log_${sample}_${enery_name}/${filename}_SKIM.log" \
    -e "./err_${sample}_${enery_name}/${filename}_SKIM.err" \
    ${Code} \
    --input_file "${mdst_path}/${enery_name}/${filename}.root" \
    --output_file "./output_${sample}_${enery_name}/SKIM_${filename}.root"
  done
fi

# DDBAR
sample="DDBAR"
mdst_path="/home/belle2/junewoo/storage_ghi/tau_gen_cut/MC16ri/MC16ri_private_gencut/${sample}/"
enery_name="MC16ri_on"
mkdir -p "./log_${sample}_${enery_name}"
mkdir -p "./err_${sample}_${enery_name}"
mkdir -p "./output_${sample}_${enery_name}"
if compgen -G "${mdst_path}/${enery_name}/*.root" > /dev/null; then
  for file in "${mdst_path}/${enery_name}"/*.root; do
    filename=$(basename "$file" .root) # without path, without extension
    bsub -q s \
    -J SKIM \
    -o "./log_${sample}_${enery_name}/${filename}_SKIM.log" \
    -e "./err_${sample}_${enery_name}/${filename}_SKIM.err" \
    ${Code} \
    --input_file "${mdst_path}/${enery_name}/${filename}.root" \
    --output_file "./output_${sample}_${enery_name}/SKIM_${filename}.root"
  done
fi

# SSBAR
sample="SSBAR"
mdst_path="/home/belle2/junewoo/storage_ghi/tau_gen_cut/MC16ri/MC16ri_private_gencut/${sample}/"
enery_name="MC16ri_on"
mkdir -p "./log_${sample}_${enery_name}"
mkdir -p "./err_${sample}_${enery_name}"
mkdir -p "./output_${sample}_${enery_name}"
if compgen -G "${mdst_path}/${enery_name}/*.root" > /dev/null; then
  for file in "${mdst_path}/${enery_name}"/*.root; do
    filename=$(basename "$file" .root) # without path, without extension
    bsub -q s \
    -J SKIM \
    -o "./log_${sample}_${enery_name}/${filename}_SKIM.log" \
    -e "./err_${sample}_${enery_name}/${filename}_SKIM.err" \
    ${Code} \
    --input_file "${mdst_path}/${enery_name}/${filename}.root" \
    --output_file "./output_${sample}_${enery_name}/SKIM_${filename}.root"
  done
fi

# CCBAR
sample="CCBAR"
mdst_path="/home/belle2/junewoo/storage_ghi/tau_gen_cut/MC16ri/MC16ri_private_gencut/${sample}/"
enery_name="MC16ri_on"
mkdir -p "./log_${sample}_${enery_name}"
mkdir -p "./err_${sample}_${enery_name}"
mkdir -p "./output_${sample}_${enery_name}"
if compgen -G "${mdst_path}/${enery_name}/*.root" > /dev/null; then
  for file in "${mdst_path}/${enery_name}"/*.root; do
    filename=$(basename "$file" .root) # without path, without extension
    bsub -q s \
    -J SKIM \
    -o "./log_${sample}_${enery_name}/${filename}_SKIM.log" \
    -e "./err_${sample}_${enery_name}/${filename}_SKIM.err" \
    ${Code} \
    --input_file "${mdst_path}/${enery_name}/${filename}.root" \
    --output_file "./output_${sample}_${enery_name}/SKIM_${filename}.root"
  done
fi

# TAUPAIR
sample="TAUPAIR"
mdst_path="/home/belle2/junewoo/storage_ghi/tau_gen_cut/MC16ri/MC16ri_private_gencut/${sample}/"
enery_name="MC16ri_on"
mkdir -p "./log_${sample}_${enery_name}"
mkdir -p "./err_${sample}_${enery_name}"
mkdir -p "./output_${sample}_${enery_name}"
if compgen -G "${mdst_path}/${enery_name}/*.root" > /dev/null; then
  for file in "${mdst_path}/${enery_name}"/*.root; do
    filename=$(basename "$file" .root) # without path, without extension
    bsub -q s \
    -J SKIM \
    -o "./log_${sample}_${enery_name}/${filename}_SKIM.log" \
    -e "./err_${sample}_${enery_name}/${filename}_SKIM.err" \
    ${Code} \
    --input_file "${mdst_path}/${enery_name}/${filename}.root" \
    --output_file "./output_${sample}_${enery_name}/SKIM_${filename}.root"
  done
fi