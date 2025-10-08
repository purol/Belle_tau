#!/bin/bash

# predefined input variables
input_variables=(
    "missingEnergyOfEventCMS"
    "roeMbc__bocleanMask__bc"
    "missingMomentumOfEventCMS"
    "cosTBTO__bocleanMask__bc"
    "harmonicMomentThrust1"
    "second_muon_p"
    "KSFWVariables__bohso22__cm__spcleanMask__bc"
    "CleoConeCS__bo1__cm__spcleanMask__bc"
    "roeE__bocleanMask__bc"
    "third_muon_p"
    "cosAngleBetweenMomentumAndVertexVector"
    "first_muon_p"
    "KSFWVariables__bohso24__cm__spcleanMask__bc"
    "roeEextra__bocleanMask__bc"
    "dcosTheta"
    "angleToClosestInList__bopi__pl__clevtshape_kinematics__bc"
    "totalEnergyOfParticlesInList__bogamma__clevtshape_kinematics__bc"
    "dr"
    "missingMomentumOfEventCMS_theta"
    "cosTBz__bocleanMask__bc"
    "useCMSFrame__botheta__bc"
    "dphi"
    "useCMSFrame__bophi__bc"
    "thrustAxisCosTheta"
    "harmonicMomentThrust3"
    "flightTime_dividedby_flightTimeErr"
    "cosToThrustOfEvent"
    "KSFWVariables__bohso14__cm__spcleanMask__bc"
    "chiProb"
    "charge_times_ROEcharge"
)

other_variables_log=(
    "first_muon_muonID"
    "second_muon_muonID"
    "third_muon_muonID"
    "first_muon_electronID"
    "second_muon_electronID"
    "third_muon_electronID"
    "first_muon_pionID"
    "second_muon_pionID"
    "third_muon_pionID"
)

other_variables_linear=(
    "missingMomentumOfEvent_theta"
    "thrustAxisCosTheta"
    "thrust"
    "avg_cosToThrustOfEvent_CM"
    "stddev_cosToThrustOfEvent_CM"
    "diff_cosToThrustOfEvent_CM"
)

all_variables=(
    "${input_variables[@]}"
    "${other_variables_log[@]}"
    "${other_variables_linear[@]}"
)

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

 
#code="${Belle_tau_DIR}/analysis_code/bin/Plotter"
#VarName="deltaE"
#submit_Plotter ${code} ${Analysis_Name} ${VarName} -1.1 0.6 "before_M_deltaE_selection" "before_M_deltaE_selection_deltaE" "plot"

#code="${Belle_tau_DIR}/analysis_code/bin/Plotter"
#VarName="M_inv_tau"
#submit_Plotter ${code} ${Analysis_Name} ${VarName} 1.3 2.1 "before_M_deltaE_selection" "before_M_deltaE_selection_M_inv_tau" "plot"

code="${Belle_tau_DIR}/analysis_code/bin/Plotter"
VarName="deltaE"
submit_Plotter ${code} ${Analysis_Name} ${VarName} -0.9 0.4 "before_strict_M_deltaE_selection" "before_strict_M_deltaE_selection_deltaE" "plot"

code="${Belle_tau_DIR}/analysis_code/bin/Plotter"
VarName="M_inv_tau"
submit_Plotter ${code} ${Analysis_Name} ${VarName} 1.5 1.9 "before_strict_M_deltaE_selection" "before_strict_M_deltaE_selection_M_inv_tau" "plot"

Directories=(
    "before_PrimarymuonID_selection"
    "before_SecondarymuonID_selection"
    "before_ThirdmuonID_selection"
    "before_SecondarymuonP_selection"
    "before_theta_miss_cut"
    "before_thrust_cut"
    "before_Eecl_cut"
    "before_diffthrust_cut"
    "before_strict_M_deltaE_selection"
    "final_output"
)

for ((i = 0; i < ${#Directories[@]}; i++)); do
  Directory="${Directories[i]}"

  for ((j = 0; j < ${#input_variables[@]}; j++)); do
    code="${Belle_tau_DIR}/analysis_code/bin/Plotter"
    VarName="${input_variables[j]}"
    submit_Plotter ${code} ${Analysis_Name} ${VarName} "${Directory}" "${Directory}_${VarName}" "plot"
  done

  for ((j = 0; j < ${#other_variables_log[@]}; j++)); do
    code="${Belle_tau_DIR}/analysis_code/bin/Plotter_log"
    VarName="${other_variables_log[j]}"
    submit_Plotter ${code} ${Analysis_Name} ${VarName} "${Directory}" "${Directory}_${VarName}" "plot"
  done

  for ((j = 0; j < ${#other_variables_linear[@]}; j++)); do
    code="${Belle_tau_DIR}/analysis_code/bin/Plotter"
    VarName="${other_variables_linear[j]}"
    submit_Plotter ${code} ${Analysis_Name} ${VarName} "${Directory}" "${Directory}_${VarName}" "plot"
  done

done

