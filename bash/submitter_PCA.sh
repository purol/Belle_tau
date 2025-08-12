#!/bin/bash

submit_code() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local toys_name=$3
  local output_name=$4

  cat "./${VerName}/${Analysis_VerName}/cal_out/${toys_name}"*csv > "./${VerName}/${Analysis_VerName}/${toys_name}.csv"

  bsub -q s -J PCA -o "./test.log" ${Code} --input_file "./${VerName}/${Analysis_VerName}/${toys_name}.csv" --output_file "./${VerName}/${Analysis_VerName}/${output_name}"
}


code="${Belle_tau_DIR}/analysis_code/src/PCA_toys.py"
toys="muonID_toys"
output_name="muonID_PCA"
submit_code ${code} ${Analysis_Name} ${toys} ${output_name}

