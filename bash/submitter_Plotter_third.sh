#!/bin/bash

submit_Plotter() {

  if [ "$#" -eq 6 ]; then
    local Code=$1 # ex. ./bin/Plotter
    local VerName=$2 # ex. Alice
    local VarName=$3 # ex. deltaE
    local InputDir=$4 # ex. before_M_deltaE_selection
    local OutputName=$5 # ex. deltaE
    local OutputPath=$6 # ex. plot
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}"
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/log"
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/err"

    bsub -q l -J Plotter -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log/${InputDir}_${OutputName}.log" -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err/${InputDir}_${OutputName}.err" ${Code} "${VarName}" "./${VerName}/${Analysis_VerName}/" "/${InputDir}/" "./${VerName}/${Analysis_VerName}/${OutputPath}" "${OutputName}"
  elif [ "$#" -eq 8 ]; then
    local Code=$1 # ex. ./bin/Plotter
    local VerName=$2 # ex. Alice
    local VarName=$3 # ex. deltaE
    local VarMin=$4
    local VarMax=$5
    local InputDir=$6 # ex. before_M_deltaE_selection
    local OutputName=$7 # ex. deltaE
    local OutputPath=$8 # ex. plot
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}"
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/log"
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/err"

    bsub -q l -J Plotter -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log/${InputDir}_${OutputName}.log" -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err/${InputDir}_${OutputName}.err" ${Code} "${VarName}" "./${VerName}/${Analysis_VerName}/" "/${InputDir}/" "./${VerName}/${Analysis_VerName}/${OutputPath}" "${OutputName}" "$VarMin" "$VarMax"
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

  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/log"
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/err"

  bsub -q l -J Plotter -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log/${InputDir}_${OutputName}.log" -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err/${InputDir}_${OutputName}.err" ${Code} "${VarName_1}" "${VarName_2}" "./${VerName}/${Analysis_VerName}/" "/${InputDir}/" "./${VerName}/${Analysis_VerName}/${OutputPath}" "${OutputName}"

}
 

code="${Belle_tau_DIR}/analysis_code/bin/Plotter_2D_signal_half"
VarName_1="M"
VarName_2="deltaE"
submit_Plotter_2D ${code} ${Analysis_Name} ${VarName_1} ${VarName_2} "final_output_test_after_application_after_cut" "final_output_test_after_application_after_cut_M_deltaE_signal" "plot"

code="${Belle_tau_DIR}/analysis_code/bin/Plotter_2D_bkg_half"
VarName_1="M"
VarName_2="deltaE"
submit_Plotter_2D ${code} ${Analysis_Name} ${VarName_1} ${VarName_2} "final_output_test_after_application_after_cut" "final_output_test_after_application_after_cut_M_deltaE_bkg" "plot"


