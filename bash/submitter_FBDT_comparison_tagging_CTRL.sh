#!/bin/bash

submit_Plotter() {

  local Code=$1 # ex. ./bin/Plotter
  local VerName=$2 # ex. Alice
  local VarName=$3 # ex. deltaE
  local VarMin=$4
  local VarMax=$5
  local InputDir=$6 # ex. before_M_deltaE_selection
  local OutputName=$8 # ex. deltaE
  local OutputPath=$9 # ex. plot
  local Type=${10}
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}"
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/log"
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/err"

  bsub -q l \
  -J Compare \
  -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log/compare_tag_${VarName}_${OutputName}.log" \
  -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err/compare_tag_${VarName}_${OutputName}.err" \
  ${Code} \
  "${VarName}" \
  50 \
  "${VarMin}" \
  "${VarMax}" \
  "./${VerName}/${Analysis_VerName}/${Type}/${InputDir}/" \
  "./${VerName}/${Analysis_VerName}/${OutputPath}/" \
  "${OutputName}.png" \
  "${Type}"
}
 
code="${Belle_tau_DIR}/analysis_code/bin/var_comparison_tag"
VarName="BDT_output_1"
submit_Plotter ${code} ${Analysis_Name} ${VarName} 0.0 1.0 "before_leptonic_tag_after_application" "FBDT1_comp_tag" "plot" "${Signal_Type}"

code="${Belle_tau_DIR}/analysis_code/bin/var_comparison_tag"
VarName="BDT_output_2"
submit_Plotter ${code} ${Analysis_Name} ${VarName} 0.0 1.0 "before_leptonic_tag_after_application" "FBDT2_comp_tag" "plot" "${Signal_Type}"