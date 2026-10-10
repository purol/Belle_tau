#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.+-eE]\+\)_life\([0-9.+-eE]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\).*/\1 \2 \3 \4/p' | \
  sort -u
}

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

  get_params "${nominal_analysis_DIR}/${Sample2List}/${InputDir2}/" | while read mass life A B; do

    printf -v command '%q ' \
      ${Code} \
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
      "${nominal_analysis_DIR}" \
      "${mass}" \
      "${life}" \
      "${A}" \
      "${B}"

    bsub -q l \
    -J GetWeight \
    -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log/GetWeight_${Flag}_${mass}_${life}_${A}_${B}.log" \
    -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err/GetWeight_${Flag}_${mass}_${life}_${A}_${B}.err" \
    "${command}"
  done

}
 
code="${Belle_tau_DIR}/analysis_code/bin/GetWeight_CTRL_ALP_one"
VarNames="${Two_weight_vars_STR}"
VarBins="${Two_vars_binnings_STR}"
submit_Plotter ${code} ${Analysis_Name} ${VarNames} ${VarBins} "final_output_after_application" "final_output_after_application" "Weight" "${Signal_Type}" "ALP" "${Signal_Legends}" "${ALP_Legends}" "one"

code="${Belle_tau_DIR}/analysis_code/bin/GetWeight_CTRL_ALP_two"
VarNames="${Two_weight_vars_STR}"
VarBins="${Two_vars_binnings_STR}"
submit_Plotter ${code} ${Analysis_Name} ${VarNames} ${VarBins} "final_output_after_application" "final_output_after_application" "Weight" "${Signal_Type}" "ALP" "${Signal_Legends}" "${ALP_Legends}" "two"