#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.+-eE]\+\)_life\([0-9.+-eE]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\).*/\1 \2 \3 \4/p' | \
  sort -u
}

submit_code() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local toys_name=$3
  local output_name=$4

  get_params "./${VerName}/${Analysis_VerName}/ALP/final_output" | while read mass life A B; do

    cat "./${VerName}/${Analysis_VerName}/cal_out_${mass}_${life}_${A}_${B}/${toys_name}"*csv > "./${VerName}/${Analysis_VerName}/${toys_name}_${mass}_${life}_${A}_${B}.csv"

    bsub -q s -J PCA -o "./${VerName}/${Analysis_VerName}/${toys_name}_cal_${mass}_${life}_${A}_${B}.log" ${Code} --input_file "./${VerName}/${Analysis_VerName}/${toys_name}_${mass}_${life}_${A}_${B}.csv" --output_file "./${VerName}/${Analysis_VerName}/${output_name}_${mass}_${life}_${A}_${B}"

  done
}


code="${Belle_tau_DIR}/analysis_code/src/PCA_toys.py"
toys="muonID_toys"
output_name="muonID_PCA"
submit_code ${code} ${Analysis_Name} ${toys} ${output_name}


code="${Belle_tau_DIR}/analysis_code/src/PCA_toys.py"
toys="luminosity_toys"
output_name="luminosity_PCA"
submit_code ${code} ${Analysis_Name} ${toys} ${output_name}

