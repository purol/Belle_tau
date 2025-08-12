#!/bin/bash

submit_workspace() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice

  bsub -q s -J CRTWS -o "./${VerName}/${Analysis_VerName}/CreateWorkSpace.log" ${Code} "./${VerName}/${Analysis_VerName}" "final_output_test_after_application_after_cut" "./${VerName}/${Analysis_VerName}/" 
}


code="${Belle_tau_DIR}/analysis_code/bin/CreateWorkSpace"
submit_workspace ${code} ${Analysis_Name}

