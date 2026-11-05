#!/bin/bash

submit_FBDT_syst() {
  local SignalType=$1
  local OutputName="FBDTSyst"
  shift
  if [ "$#" -eq 4 ]; then
    OutputName="FBDTSyst_$1_$2_$3_$4"
  fi

  bsub -q l \
  -J FBDTSyst \
  -o "${output_path}/${OutputName}.log" \
  -e "${output_path}/${OutputName}.err" \
  "${code}" \
  "${nominal_analysis_DIR}" \
  "${SignalType}" \
  "${control_path}" \
  "${Types_STR_WITH_SIGNAL}" \
  "${data_Type}" \
  "${output_path}" \
  "$@"
}

control_path="./${Analysis_Name}/${Analysis_VerName}"
output_path="${control_path}/FBDTSyst"
code="${Belle_tau_DIR}/analysis_code/bin/Calculate_FBDT_syst_CTRL"

mkdir -p "${output_path}"

if [ "$1" = "SIGNAL" ]; then
  submit_FBDT_syst "SIGNAL"
elif [ "$1" = "ALP" ]; then
  # get ALP parameters from nominal signal files
  find "${nominal_analysis_DIR}/${ALP_Type}/final_output_after_application" -maxdepth 1 -name '*.root' -printf '%f\n' | \
    sed -nE 's/.*alpha_mass([0-9.eE+-]+)_life([0-9.eE+-]+)_A([-+0-9]+)_B([-+0-9]+)_.*/\1 \2 \3 \4/p' | sort -u | \
  while read -r mass life A B; do
    submit_FBDT_syst "${ALP_Type}" "${mass}" "${life}" "${A}" "${B}"
  done
fi
