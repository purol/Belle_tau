#!/bin/bash

submit_analysis() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local SampleNames=$3 # ex. MUMUTAUTAU

  mkdir -p "./${VerName}/${Analysis_VerName}/correlation"

  bsub -q s \
  -J Corr \
  -o "./${VerName}/${Analysis_VerName}/correlation/correlation.log" \
  -e "./${VerName}/${Analysis_VerName}/correlation/correlation.err" \
  ${Code} \
  "./${VerName}/${Analysis_VerName}" \
  "final_output_after_application" \
  "root" \
  "${SampleNames}" \
  "./${VerName}/${Analysis_VerName}/" \
  "./${VerName}/${Analysis_VerName}/correlation/"

}

code="${Belle_tau_DIR}/analysis_code/bin/GetCorrelation_one"
submit_analysis ${code} ${Analysis_Name} ${Background_Types_STR}

code="${Belle_tau_DIR}/analysis_code/bin/GetCorrelation_two"
submit_analysis ${code} ${Analysis_Name} ${Background_Types_STR}