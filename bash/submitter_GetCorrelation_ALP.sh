#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.+-eE]\+\)_life\([0-9.+-eE]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\).*/\1 \2 \3 \4/p' | \
  sort -u
}

submit_analysis() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local SampleNames=$3 # ex. MUMUTAUTAU
  local Flag=${4}

  mkdir -p "./${VerName}/${Analysis_VerName}/correlation"

  get_params "./${VerName}/${Analysis_VerName}/ALP/final_output_after_application" | while read mass life A B; do

    bsub -q s \
    -J Corr \
    -o "./${VerName}/${Analysis_VerName}/correlation/correlation_${Flag}_${mass}_${life}_${A}_${B}.log" \
    -e "./${VerName}/${Analysis_VerName}/correlation/correlation_${Flag}_${mass}_${life}_${A}_${B}.err" \
    ${Code} \
    "./${VerName}/${Analysis_VerName}" \
    "final_output_after_application" \
    "root" \
    "${SampleNames}" \
    "./${VerName}/${Analysis_VerName}/" \
    "./${VerName}/${Analysis_VerName}/correlation/" \
    "${mass}" \
    "${life}" \
    "${A}" \
    "${B}"

  done

}

code="${Belle_tau_DIR}/analysis_code/bin/GetCorrelation_one_ALP"
submit_analysis ${code} ${Analysis_Name} ${Background_Types_STR} "one"

code="${Belle_tau_DIR}/analysis_code/bin/GetCorrelation_two_ALP"
submit_analysis ${code} ${Analysis_Name} ${Background_Types_STR} "two"