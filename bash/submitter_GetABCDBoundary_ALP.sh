#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.+-eE]\+\)_life\([0-9.+-eE]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\).*/\1 \2 \3 \4/p' | \
  sort -u
}

submit_boundary() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice

  get_params "./${VerName}/${Analysis_VerName}/ALP/final_output" | while read mass life A B; do
    bsub -q s \
    -J ABCDBND \
    -o "./${VerName}/${Analysis_VerName}/GetABCDBoundary_${mass}_${life}_${A}_${B}.log" \
    ${Code} \
    "./${VerName}/${Analysis_VerName}" \
    "final_output_after_application" \
    "${Background_Types_STR}" \
    "${mass}" \
    "${life}" \
    "${A}" \
    "${B}"
  done
}


code="${Belle_tau_DIR}/analysis_code/bin/GetABCDBoundary_ALP"
submit_boundary ${code} ${Analysis_Name}
