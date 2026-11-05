#!/bin/bash

submit_code() {
  local Code=$1 # ex. ./bin/Calculate_FBDT_syst_CTRL
  local VerName=$2 # ex. Alice
  local SignalType=$3 # ex. SIGNAL
  local MCList=$4
  local dataList=$5
  local OutputPath=$6 # ex. FBDTSyst

  local ControlPath="./${VerName}/${Analysis_VerName}"
  if ! compgen -G "${ControlPath}/${dataList}/final_output_after_application/*.root" > /dev/null; then
    echo "No control data for FBDT efficiency."
    return 0
  fi
  mkdir -p "${ControlPath}/${OutputPath}" || return 1

  bsub -q l \
  -J FBDTSyst \
  -o "${ControlPath}/${OutputPath}/FBDTSyst.log" \
  -e "${ControlPath}/${OutputPath}/FBDTSyst.err" \
  "${Code}" \
  "${nominal_analysis_DIR}" \
  "${SignalType}" \
  "${ControlPath}" \
  "${MCList}" \
  "${dataList}" \
  "${ControlPath}/${OutputPath}"
}


if [ "$#" -ne 0 ]; then
  echo "Usage: $0"
  exit 1
fi

code="${Belle_tau_DIR}/analysis_code/bin/Calculate_FBDT_syst_CTRL"
submit_code "${code}" "${Analysis_Name}" "SIGNAL" "${Types_STR_WITH_SIGNAL}" "${data_Type}" "FBDTSyst"
