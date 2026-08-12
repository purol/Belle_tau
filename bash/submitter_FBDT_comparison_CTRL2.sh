#!/bin/bash

submit_Plotter() {
  local command

  local Code=$1 # ex. ./bin/Plotter
  local VerName=$2 # ex. Alice
  local VarName=$3 # ex. deltaE
  local VarMin=$4
  local VarMax=$5
  local InputDir1=$6 # ex. before_M_deltaE_selection
  local InputDir2=$7 # ex. before_M_deltaE_selection
  local OutputName=$8 # ex. deltaE
  local OutputPath=$9 # ex. plot
  local Type1=${10}
  local Type2=${11}
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}"
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/log"
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/err"

  printf -v command '%q ' \
    "${Code}" \
    "${VarName}" \
    "50" \
    "${VarMin}" \
    "${VarMax}" \
    "./${VerName}/${Analysis_VerName}/${Type1}/${InputDir1}/" \
    "${FBDT_weight_DIR}/${Type2}/${InputDir2}/" \
    "./${VerName}/${Analysis_VerName}/${OutputPath}/" \
    "${OutputName}.png" \
    "${Type1}" \
    "${Type2}" \
    "#piID sideband" \
    "#tau#rightarrow#mu#mu#mu" \
    "ratio" \
    "${FBDT_weight_DIR}"

  bsub -q l \
  -J Compare \
  -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log/compare_${VarName}_${OutputName}.log" \
  -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err/compare_${VarName}_${OutputName}.err" \
  "${command}"
}
 
code="${Belle_tau_DIR}/analysis_code/bin/var_comparison_CTRL_one"
VarName="BDT_output_1"
submit_Plotter ${code} ${Analysis_Name} ${VarName} 0.0 1.0 "final_output_after_application" "final_output_test_after_application" "FBDT1_comp" "plot" "${Background_Types_STR}" "${Background_Types_STR}"

code="${Belle_tau_DIR}/analysis_code/bin/var_comparison_CTRL_two"
VarName="BDT_output_2"
submit_Plotter ${code} ${Analysis_Name} ${VarName} 0.0 1.0 "final_output_after_application" "final_output_test_after_application" "FBDT2_comp" "plot" "${Background_Types_STR}" "${Background_Types_STR}"