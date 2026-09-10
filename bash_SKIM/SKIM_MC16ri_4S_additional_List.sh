#!/bin/bash

export Belle_tau_DIR="/home/belle2/junewoo/storage_b2/tau_workspace/Belle_tau" # analysis code path
Code="${Belle_tau_DIR}/python/tau_mumumu_TauToMuMuMu_List.py"

energy_name="MC16ri_on"
batch_size=100
samples=(UUBAR DDBAR SSBAR CCBAR TAUPAIR)
run_groups=(run1 run2_PXDOFF run2_PXDON)

shopt -s nullglob

for sample in "${samples[@]}"; do
  input_dir="/home/belle2/junewoo/storage_ghi/tau_gen_cut/MC16ri/MC16ri_private_gencut/${sample}/${energy_name}"
  log_dir="./log_${sample}_${energy_name}"
  err_dir="./err_${sample}_${energy_name}"
  output_dir="./output_${sample}_${energy_name}"

  mkdir -p "${log_dir}" "${err_dir}" "${output_dir}"

  for run_group in "${run_groups[@]}"; do
    # The glob contains both sample and run group, so neither can be mixed.
    files=("${input_dir}/${sample}_${run_group}_"*.root)
    n_files=${#files[@]}

    if (( n_files == 0 )); then
      echo "No files found for ${sample} ${run_group}; skipping."
      continue
    fi

    batch_number=1
    for (( start=0; start<n_files; start+=batch_size )); do
      batch_files=("${files[@]:start:batch_size}")
      batch_tag=$(printf 'batch%04d' "${batch_number}")
      job_tag="${sample}_${run_group}_${batch_tag}"

      echo "Submitting ${job_tag} with ${#batch_files[@]} file(s)."
      bsub -q s \
        -J "SKIM_${job_tag}" \
        -o "${log_dir}/${job_tag}_SKIM.log" \
        -e "${err_dir}/${job_tag}_SKIM.err" \
        "${Code}" \
        --input_file "${batch_files[@]}" \
        --output_file "${output_dir}/SKIM_${job_tag}.root"

      ((batch_number++))

      sleep 0.5s
    done
  done
done
