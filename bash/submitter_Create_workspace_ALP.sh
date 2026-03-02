#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.+-eE]\+\)_life\([0-9.+-eE]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\).*/\1 \2 \3 \4/p' | \
  sort -u
}

submit_workspace() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice

  get_params "./${VerName}/${Analysis_VerName}/ALP/final_output" | while read mass life A B; do
    bsub -q s -J CRTWS -o "./${VerName}/${Analysis_VerName}/CreateWorkSpace_${mass}_${life}_${A}_${B}.log" ${Code} "./${VerName}/${Analysis_VerName}" "final_output_test_after_application" "final_output_test_after_application" "./${VerName}/${Analysis_VerName}/GridSearch_one/FOM_${mass}_${life}_${A}_${B}.log" "./${VerName}/${Analysis_VerName}/GridSearch_two/FOM_${mass}_${life}_${A}_${B}.log" "./${VerName}/${Analysis_VerName}/" "${mass}" "${life}" "${A}" "${B}"
  done
}


code="${Belle_tau_DIR}/analysis_code/bin/CreateWorkSpace_ALP"
submit_workspace ${code} ${Analysis_Name}

