#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.+-eE]\+\)_life\([0-9.+-eE]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\).*/\1 \2 \3 \4/p' | \
  sort -u
}

submit_CLs() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice

  get_params "./${VerName}/${Analysis_VerName}/ALP/final_output" | while read mass life A B; do

    mkdir -p "./${VerName}/${Analysis_VerName}/CLs_${mass}_${life}_${A}_${B}/out"
    mkdir -p "./${VerName}/${Analysis_VerName}/CLs_${mass}_${life}_${A}_${B}/log"

    for mu in $(seq 0 0.1 5.0)
    do
      for index in {0..10}; do
        bsub -q s -J TAUCLS -o "./${VerName}/${Analysis_VerName}/CLs_${mass}_${life}_${A}_${B}/log/${mu}_${index}.log" ${Code} "./${VerName}/${Analysis_VerName}" "workspace_${mass}_${life}_${A}_${B}.root" "./${VerName}/${Analysis_VerName}/CLs_${mass}_${life}_${A}_${B}/out" ${mu} ${index}
      done
    done

  done

}


code="${Belle_tau_DIR}/analysis_code/bin/Run_CLs"
submit_CLs ${code} ${Analysis_Name}

