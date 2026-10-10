#!/bin/bash

submit_analysis() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local SampleNames=$3 # ex. MUMUTAUTAU
  local Flag=${4} 

  mkdir -p "./${VerName}/${Analysis_VerName}/correlation"

  bsub -q s \
  -J Corr \
  -o "./${VerName}/${Analysis_VerName}/correlation/correlation_${Flag}.log" \
  -e "./${VerName}/${Analysis_VerName}/correlation/correlation_${Flag}.err" \
  ${Code} \
  "./${VerName}/${Analysis_VerName}" \
  "final_output_after_application" \
  "root" \
  "${SampleNames}" \
  "./${VerName}/${Analysis_VerName}/" \
  "./${VerName}/${Analysis_VerName}/correlation/"

}

code="${Belle_tau_DIR}/analysis_code/bin/GetCorrelation_one"
submit_analysis ${code} ${Analysis_Name} ${Background_Types_STR} "one"

code="${Belle_tau_DIR}/analysis_code/bin/GetCorrelation_two"
submit_analysis ${code} ${Analysis_Name} ${Background_Types_STR} "two"