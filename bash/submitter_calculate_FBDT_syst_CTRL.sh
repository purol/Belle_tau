#!/bin/bash

submit_code() {
  local Code=$1 # ex. ./bin/Calculate_FBDT_syst_CTRL
  local VerName=$2 # ex. Alice
  local InputDir1=$3 # ex. final_output_after_application
  local InputDir2=$4 # ex. final_output_after_application
  local OutputPath=$5 # ex. FBDTSyst
  local SignalType=$6 # ex. SIGNAL
  local MCList=$7
  local dataList=$8
  local WeightPath=$9 # ex. Weight

  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}"

  bsub -q l \
  -J FBDTSyst \
  -o "./${VerName}/${Analysis_VerName}/${OutputPath}/FBDTSyst.log" \
  -e "./${VerName}/${Analysis_VerName}/${OutputPath}/FBDTSyst.err" \
  "${Code}" \
  "${nominal_analysis_DIR}" \
  "${InputDir1}" \
  "./${VerName}/${Analysis_VerName}" \
  "${InputDir2}" \
  "./${VerName}/${Analysis_VerName}/${OutputPath}" \
  "${SignalType}" \
  "${MCList}" \
  "${dataList}" \
  "./${VerName}/${Analysis_VerName}/${WeightPath}"
}


code="${Belle_tau_DIR}/analysis_code/bin/Calculate_FBDT_syst_CTRL"
submit_code "${code}" "${Analysis_Name}" "final_output_after_application" "final_output_after_application" "FBDTSyst" "SIGNAL" "${Types_STR_WITH_SIGNAL}" "${data_Type}" "Weight"
