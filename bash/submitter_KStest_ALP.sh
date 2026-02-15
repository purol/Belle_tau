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
  local VarName=$3
  local Nbin=$4
  local MIN=$5
  local MAX=$6
  local OutputPath=$7

  get_params "./${VerName}/${Analysis_VerName}/ALP/final_output" | while read mass life A B; do
    bsub -q l -J KSTEST -o "/dev/null" ${Code} "${VarName}" "${Nbin}" "${MIN}" "${MAX}" "./${VerName}/${Analysis_VerName}" "./${VerName}/${Analysis_VerName}/${OutputPath}" "KS_${VarName}_${mass}_${life}_${A}_${B}.png" "${mass}" "${life}" "${A}" "${B}"
  done

}


code="${Belle_tau_DIR}/analysis_code/bin/KS_test_half_one_ALP"
varname="BDT_output_1"
Nbin="50"
MIN="0.0"
MAX="1.0"
output="GridSearch_one"
submit_code ${code} ${Analysis_Name} ${varname} ${Nbin} ${MIN} ${MAX} ${output}

code="${Belle_tau_DIR}/analysis_code/bin/KS_test_half_two_ALP"
varname="BDT_output_2"
Nbin="50"
MIN="0.0"
MAX="1.0"
output="GridSearch_two"
submit_code ${code} ${Analysis_Name} ${varname} ${Nbin} ${MIN} ${MAX} ${output}

