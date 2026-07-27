#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.+-eE]\+\)_life\([0-9.+-eE]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\).*/\1 \2 \3 \4/p' | \
  sort -u
}

submit_Plotter() {

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

  get_params "${FBDT_weight_DIR}/SIGNAL/${InputDir2}/" | while read mass life A B; do
    BDTName_ALP=${VarName}_${mass}_${life}_${A}_${B}
    BDTName_ALP=${BDTName_ALP//-/m}

    bsub -q l \
    -J Compare \
    -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log/compare_${BDTName_ALP}_${OutputName}.log" \
    -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err/compare_${BDTName_ALP}_${OutputName}.err" \
    ${Code} \
    "${BDTName_ALP}" \
    50 \
    "${VarMin}" \
    "${VarMax}" \
    "./${VerName}/${Analysis_VerName}/${Signal_Type}/${InputDir1}/" \
    "${FBDT_weight_DIR}/SIGNAL/${InputDir2}/" \
    "./${VerName}/${Analysis_VerName}/${OutputPath}/" \
    "${OutputName}.png" \
    "${Type1}" \
    "${Type2}" \
    "${Signal_Legends}" \
    "#tau#rightarrow#alpha#mu" \
    "${mass}" \
    "${life}" \
    "${A}" \
    "${B}"
  done

}
 
code="${Belle_tau_DIR}/analysis_code/bin/var_comparison_ALP"
VarName="BDT_output_1"
submit_Plotter ${code} ${Analysis_Name} ${VarName} 0.0 1.0 "final_output_after_application" "final_output_test_after_application" "FBDT1_comp" "plot" "${Signal_Type}" "${Signal_Type}"

code="${Belle_tau_DIR}/analysis_code/bin/var_comparison_ALP"
VarName="BDT_output_2"
submit_Plotter ${code} ${Analysis_Name} ${VarName} 0.0 1.0 "final_output_after_application" "final_output_test_after_application" "FBDT2_comp" "plot" "${Signal_Type}" "${Signal_Type}"