#!/bin/bash

submit_workspace() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice

  bsub -q s -J CRTWS -o "./${VerName}/${Analysis_VerName}/CreateWorkSpace.log" ${Code} "./${VerName}/${Analysis_VerName}" "final_output_test_after_application" "final_output_test_after_application" "./${VerName}/${Analysis_VerName}/GridSearch_one/FOM.log" "./${VerName}/${Analysis_VerName}/GridSearch_two/FOM.log" "./${VerName}/${Analysis_VerName}/" ${Signal_Type} ${Background_Types_STR}
}


code="${Belle_tau_DIR}/analysis_code/bin/CreateWorkSpace"
submit_workspace ${code} ${Analysis_Name}

