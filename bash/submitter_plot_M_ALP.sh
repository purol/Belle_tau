#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.+-eE]\+\)_life\([0-9.+-eE]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\).*/\1 \2 \3 \4/p' | \
  sort -u
}

submit_fitter() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local SampleName=$3 # ex. MUMUTAUTAU

  mkdir -p "./${VerName}/${Analysis_VerName}/plot/"

  get_params "./${VerName}/${Analysis_VerName}/${SampleName}/before_strict_M_deltaE_selection" | while read mass life A B; do
    bsub -q s -J M_ALP_PLT -o "/dev/null" ${Code} "./${VerName}/${Analysis_VerName}/${SampleName}/before_strict_M_deltaE_selection" "./${VerName}/${Analysis_VerName}/" "${mass}" "${life}" "${A}" "${B}"
  done

  sleep 0.5s
}


code="${Belle_tau_DIR}/analysis_code/bin/Plotter_mass_ALP"
submit_fitter ${code} ${Analysis_Name} "ALP"