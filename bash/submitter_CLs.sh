#!/bin/bash

submit_CLs() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice

  mkdir -p "./${VerName}/${Analysis_VerName}/CLs/out"
  mkdir -p "./${VerName}/${Analysis_VerName}/CLs/log"

  for mu in $(seq 0 0.1 5.0)
  do
    for index in {0..10}; do
      bsub -q s -J TAUCLS -o "./${VerName}/${Analysis_VerName}/CLs/log/${mu}_${index}.log" ${Code} "./${VerName}/${Analysis_VerName}" "./${VerName}/${Analysis_VerName}/CLs/out" ${mu} ${index}
    done
  done

}


code="${Belle_tau_DIR}/analysis_code/bin/Run_CLs"
submit_CLs ${code} ${Analysis_Name}

