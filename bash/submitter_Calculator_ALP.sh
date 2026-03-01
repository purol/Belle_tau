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
  local NToys=$3
  local indicator=$4
  local LogName=$5

  get_params "./${VerName}/${Analysis_VerName}/ALP/final_output" | while read mass life A B; do
    mkdir -p "./${VerName}/${Analysis_VerName}/cal_out_${mass}_${life}_${A}_${B}"
    mkdir -p "./${VerName}/${Analysis_VerName}/cal_log_${mass}_${life}_${A}_${B}"

    bsub -q s -J SYSTCAL -o "./${VerName}/${Analysis_VerName}/cal_log_${mass}_${life}_${A}_${B}/${LogName}_${indicator}.log" ${Code} "./${VerName}/${Analysis_VerName}" "final_output_test_after_application" "./${VerName}/${Analysis_VerName}/cal_out_${mass}_${life}_${A}_${B}" ${NToys} ${indicator} "${mass}" "${life}" "${A}" "${B}"

  done
}


code="${Belle_tau_DIR}/analysis_code/bin/muonID_calculator_ALP"
NToys=25
for i in {0..39}; do
    submit_code ${code} ${Analysis_Name} ${NToys} ${i} "PID_calculator"
done


code="${Belle_tau_DIR}/analysis_code/bin/luminosity_calculator_ALP"
NToys=25
for i in {0..39}; do
    submit_code ${code} ${Analysis_Name} ${NToys} ${i} "L_calculator"
done

