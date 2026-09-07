#!/bin/bash

submit_code() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice

  mkdir -p "./${VerName}/${Analysis_VerName}/CLs/out"

  bsub -q s -J MERGECLS -o "/dev/null" ${Code} "./${VerName}/${Analysis_VerName}/CLs/out" "./${VerName}/${Analysis_VerName}/CLs/out"

}


code="${Belle_tau_DIR}/analysis_code/bin/Merge_CLs"
submit_code ${code} ${Analysis_Name}

