#!/bin/bash

submit_Plotter() {
  local command

  local Code=${1} # ex. ./bin/Plotter
  local VerName=${2} # ex. Alice
  local VarNames=${3} # ex. deltaE
  local VarBins=${4} # ex. deltaE
  local InputDir1=${5} # ex. before_M_deltaE_selection
  local InputDir2=${6} # ex. before_M_deltaE_selection
  local OutputPath=${7} # ex. plot
  local Sample1List=${8}
  local Sample2List=${9}
  local Sample1Label=${10}
  local Sample2Label=${11}
  local Flag=${12}

  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}"
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/log"
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/err"

  printf -v command '%q ' \
    "${Code}" \
    "./${VerName}/${Analysis_VerName}" \
    "${InputDir1}" \
    "${nominal_analysis_DIR}" \
    "${InputDir2}" \
    "${VarNames}" \
    "${VarBins}" \
    "${Sample1List}" \
    "${Sample2List}" \
    "${Sample1Label}" \
    "${Sample2Label}" \
    "./${VerName}/${Analysis_VerName}/${OutputPath}" \
    "${nominal_analysis_DIR}"

  bsub -q l \
  -J GetWeight \
  -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log/GetWeight_${Flag}.log" \
  -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err/GetWeight_${Flag}.err" \
  "${command}"

}
 
code="${Belle_tau_DIR}/analysis_code/bin/GetWeight_CTRL_one"
VarNames="${Two_weight_vars_STR}"
VarBins="${Two_vars_binnings_STR}"
submit_Plotter "${code}" "${Analysis_Name}" "${VarNames}" "${VarBins}" "final_output_after_application" "final_output_after_application" "Weight" "${Signal_Type}" "SIGNAL" "${Signal_Legends}" "#tau#rightarrow#mu#mu#mu" "one"

code="${Belle_tau_DIR}/analysis_code/bin/GetWeight_CTRL_two"
VarNames="${Two_weight_vars_STR}"
VarBins="${Two_vars_binnings_STR}"
submit_Plotter "${code}" "${Analysis_Name}" "${VarNames}" "${VarBins}" "final_output_after_application" "final_output_after_application" "Weight" "${Signal_Type}" "SIGNAL" "${Signal_Legends}" "#tau#rightarrow#mu#mu#mu" "two"