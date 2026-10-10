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

    if awk -v life="$life" 'BEGIN { exit !(life >= 700) }'; then
      mu_list="$(seq 0 20 1000)"
    elif awk -v life="$life" 'BEGIN { exit !(life >= 300 && life < 700) }'; then
      mu_list="$(seq 0 5 200)"
    elif awk -v life="$life" 'BEGIN { exit !(life >= 50 && life < 300) }'; then
      mu_list="$(seq 0 0.2 10.0)"
    else
      mu_list="$(seq 0 0.1 5.0)"
    fi

    for mu in $mu_list
    do
      for index in {0..10}; do
        bsub -q s -J TAUCLS -o "./${VerName}/${Analysis_VerName}/CLs_${mass}_${life}_${A}_${B}/log/${mu}_${index}.log" ${Code} "./${VerName}/${Analysis_VerName}" "workspace_${mass}_${life}_${A}_${B}.root" "./${VerName}/${Analysis_VerName}/CLs_${mass}_${life}_${A}_${B}/out" ${mu} ${index}
      done
    done

  done

}


code="${Belle_tau_DIR}/analysis_code/bin/Run_CLs"
submit_CLs ${code} ${Analysis_Name}