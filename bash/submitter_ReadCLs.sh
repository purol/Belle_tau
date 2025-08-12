#!/bin/bash

submit_code() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice

  bsub -q s -J READCLS -o "./${VerName}/${Analysis_VerName}/CLs/CLs.log" ${Code} "./${VerName}/${Analysis_VerName}/CLs/out" "./${VerName}/${Analysis_VerName}/CLs/"

}


code="${Belle_tau_DIR}/analysis_code/bin/ReadCLsTextFile"
submit_code ${code} ${Analysis_Name}

