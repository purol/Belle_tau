#!/bin/bash

get_params() {
  local dir="$1"

  find "$dir" -maxdepth 1 -name '*.root' -printf '%f\n' | \
  sed -nE 's/.*alpha_mass([0-9.eE+-]+)_life([0-9.eE+-]+)_A([-+0-9]+)_B([-+0-9]+)_.*/\1 \2 \3 \4/p' | \
  sort -u
}

submit_code() {
  local Code=$1 # ex. ./bin/Calculate_FBDT_syst_CTRL_ALP
  local VerName=$2 # ex. Alice
  local SignalType=$3 # ex. ALP
  local MCList=$4
  local dataList=$5
  local OutputPath=$6 # ex. FBDTSyst
  local params mass life A B

  local ControlPath="./${VerName}/${Analysis_VerName}"
  if ! compgen -G "${ControlPath}/${dataList}/final_output_after_application/*.root" > /dev/null; then
    echo "No control data for FBDT efficiency."
    return 0
  fi
  mkdir -p "${ControlPath}/${OutputPath}" || return 1

  params=$(get_params "${nominal_analysis_DIR}/${SignalType}/final_output_after_application")
  if [ -z "${params}" ]; then
    echo "Cannot find ALP signal parameters."
    return 1
  fi
  while read -r mass life A B; do
    bsub -q l \
    -J FBDTSyst \
    -o "${ControlPath}/${OutputPath}/FBDTSyst_${mass}_${life}_${A}_${B}.log" \
    -e "${ControlPath}/${OutputPath}/FBDTSyst_${mass}_${life}_${A}_${B}.err" \
    "${Code}" \
    "${nominal_analysis_DIR}" \
    "${SignalType}" \
    "${ControlPath}" \
    "${MCList}" \
    "${dataList}" \
    "${ControlPath}/${OutputPath}" \
    "${mass}" \
    "${life}" \
    "${A}" \
    "${B}" || return 1
  done <<< "${params}"
}


if [ "$#" -ne 0 ]; then
  echo "Usage: $0"
  exit 1
fi

code="${Belle_tau_DIR}/analysis_code/bin/Calculate_FBDT_syst_CTRL_ALP"
submit_code "${code}" "${Analysis_Name}" "${ALP_Type}" "${Types_STR_WITH_SIGNAL}" "${data_Type}" "FBDTSyst"
