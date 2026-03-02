#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.+-eE]\+\)_life\([0-9.+-eE]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\).*/\1 \2 \3 \4/p' | \
  sort -u
}

submit_code() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice

  get_params "./${VerName}/${Analysis_VerName}/ALP/final_output" | while read mass life A B; do

    mkdir -p "./${VerName}/${Analysis_VerName}/CLs_${mass}_${life}_${A}_${B}/out"

    bsub -q s -J MERGECLS -o "/dev/null" ${Code} "./${VerName}/${Analysis_VerName}/CLs_${mass}_${life}_${A}_${B}/out" "./${VerName}/${Analysis_VerName}/CLs_${mass}_${life}_${A}_${B}/out"

  done
}


code="${Belle_tau_DIR}/analysis_code/bin/Merge_CLs"
submit_code ${code} ${Analysis_Name}

