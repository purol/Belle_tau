#!/bin/bash

submit_boundary() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice

  bsub -q s \
  -J ABCDBND \
  -o "./${VerName}/${Analysis_VerName}/GetABCDBoundary.log" \
  ${Code} \
  "./${VerName}/${Analysis_VerName}" \
  "final_output_after_application" \
  "${Background_Types_STR}"
}


code="${Belle_tau_DIR}/analysis_code/bin/GetABCDBoundary"
submit_boundary ${code} ${Analysis_Name}
