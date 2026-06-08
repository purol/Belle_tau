#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.+-eE]\+\)_life\([0-9.+-eE]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\).*/\1 \2 \3 \4/p' | \
  sort -u
}

submit_Plotter() {

  if [ "$#" -eq 7 ]; then
    local Code=$1 # ex. ./bin/Plotter
    local VerName=$2 # ex. Alice
    local VarName=$3 # ex. deltaE
    local InputDir=$4 # ex. before_M_deltaE_selection
    local OutputName=$5 # ex. deltaE
    local OutputPath=$6 # ex. plot
    local MCTypes=$7
    local MCLegends=${8}
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}"
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/log"
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/err"

    bsub -q l -J Plotter -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log/${InputDir}_${OutputName}.log" -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err/${InputDir}_${OutputName}.err" ${Code} "${VarName}" "./${VerName}/${Analysis_VerName}/" "/${InputDir}/" "./${VerName}/${Analysis_VerName}/${OutputPath}" "${OutputName}" "${MCTypes}" "${MCLegends}"
  elif [ "$#" -eq 9 ]; then
    local Code=$1 # ex. ./bin/Plotter
    local VerName=$2 # ex. Alice
    local VarName=$3 # ex. deltaE
    local VarMin=$4
    local VarMax=$5
    local InputDir=$6 # ex. before_M_deltaE_selection
    local OutputName=$7 # ex. deltaE
    local OutputPath=$8 # ex. plot
    local MCTypes=$9
    local MCLegends=${10}
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}"
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/log"
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/err"

    bsub -q l -J Plotter -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log/${InputDir}_${OutputName}.log" -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err/${InputDir}_${OutputName}.err" ${Code} "${VarName}" "./${VerName}/${Analysis_VerName}/" "/${InputDir}/" "./${VerName}/${Analysis_VerName}/${OutputPath}" "${OutputName}" "${MCTypes}" "${MCLegends}" "$VarMin" "$VarMax"
  fi

}

submit_Plotter_2D(){
  local Code=$1
  local VerName=$2 # ex. Alice
  local VarName_1=$3
  local VarName_2=$4
  local InputDir=$5 # ex. before_M_deltaE_selection
  local OutputName=$6 # ex. deltaE
  local OutputPath=$7 # ex. plot
  local Types=$8
  local Legends=$9

  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/log"
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/err"

  bsub -q l -J Plotter -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log/${InputDir}_${OutputName}.log" -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err/${InputDir}_${OutputName}.err" ${Code} "${VarName_1}" "${VarName_2}" "./${VerName}/${Analysis_VerName}/" "/${InputDir}/" "./${VerName}/${Analysis_VerName}/${OutputPath}" "${OutputName}" "${Types}" "${Legends}"

}

get_params "${FBDT_weight_DIR}/${ALP_Type}/final_output_test_after_application" | while read mass life A B; do
  if [ "${B}" = "-1" ]; then
    B_tag="m1"
  elif [ "${B}" = "0" ]; then
    B_tag="0"
  else
    B_tag="${B}"
  fi

  code="${Belle_tau_DIR}/analysis_code/bin/Plotter_MC"
  VarName="BDT_output_1_${mass}_${life}_${A}_${B_tag}"
  submit_Plotter ${code} ${Analysis_Name} ${VarName} 0.0 1.0 "final_output_after_application" "final_output_after_application_BDT_output_1_${mass}_${life}_${A}_${B_tag}" "plot" "${Types_STR_WITH_SIGNAL}" "${Legends_STR_WITH_SIGNAL}"

  code="${Belle_tau_DIR}/analysis_code/bin/Plotter_MC"
  VarName="BDT_output_2_${mass}_${life}_${A}_${B_tag}"
  submit_Plotter ${code} ${Analysis_Name} ${VarName} 0.0 1.0 "final_output_after_application" "final_output_after_application_BDT_output_2_${mass}_${life}_${A}_${B_tag}" "plot" "${Types_STR_WITH_SIGNAL}" "${Legends_STR_WITH_SIGNAL}"
done

code="${Belle_tau_DIR}/analysis_code/bin/Plotter_2D_MC"
VarName_1="M"
VarName_2="deltaE"
submit_Plotter_2D ${code} ${Analysis_Name} ${VarName_1} ${VarName_2} "final_output_after_application" "final_output_after_application_M_deltaE_signal" "plot" "${Signal_Type}" "${Signal_Legends}"

code="${Belle_tau_DIR}/analysis_code/bin/Plotter_2D_MC"
VarName_1="M"
VarName_2="deltaE"
submit_Plotter_2D ${code} ${Analysis_Name} ${VarName_1} ${VarName_2} "final_output_after_application" "final_output_after_application_M_deltaE_bkg" "plot" "${Background_Types_STR}" "${Background_Legends_STR}"
