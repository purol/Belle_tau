#!/bin/bash

submit_code() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local InputDir1=${3} # ex. before_M_deltaE_selection
  local InputDir2=${4} # ex. before_M_deltaE_selection
  local Outputname=${5} # ex. TriggerSyst
  local MCList=${6}
  local dataList=${7}
  local MCLabel=${8}
  local dataLabel=${9}
  local TargetVar=${10}
  local TargetBinning=${11}

  mkdir -p "./${VerName}/${Analysis_VerName}/Trigger"

  bsub -q s \
  -J TriggerSyst \
  -o "./${VerName}/${Analysis_VerName}/Trigger/Trigger.log" \
  ${Code} \
  "./${VerName}/${Analysis_VerName}" \
  "${InputDir1}" \
  "./${VerName}/${Analysis_VerName}" \
  "${InputDir2}" \
  "./${VerName}/${Analysis_VerName}/Trigger/" \
  "${Outputname}" \
  "${MCList}" \
  "${dataList}" \
  "${MCLabel}" \
  "${dataLabel}" \
  "${TargetVar}" \
  "${TargetBinning}"

}


code="${Belle_tau_DIR}/analysis_code/bin/Calculate_trigger_syst_CTRL_ALP_trigger"
submit_code ${code} ${Analysis_Name} "final_output" "final_output" "TriggerSyst" "${Types_STR_WITH_SIGNAL}" "${data_Type}" "${Legends_STR_WITH_SIGNAL}" "${data_Legends}" "${Trigger_syst_vars_STR}" "${Trigger_syst_binnings_STR}"

