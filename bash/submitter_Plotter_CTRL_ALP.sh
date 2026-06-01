#!/bin/bash

merge_unique_arrays() {
  local -A seen
  local out=()
  for v in "$@"; do
    [[ -z ${seen[$v]} ]] && seen[$v]=1 && out+=("$v")
  done
  printf '%s\n' "${out[@]}"
}

# predefined input variables
IFS=':' read -r -a input_variables_one <<< "$input_variables_one_STR"
IFS=':' read -r -a input_variables_two <<< "$input_variables_two_STR"
input_variables=()
mapfile -t input_variables < <(
  merge_unique_arrays \
    "${input_variables_one[@]}" \
    "${input_variables_two[@]}"
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
    "extraInfo__boALP_flightTime__bc"
    "extraInfo__boALP_significanceOfDistance__bc"
    "extraInfo__boALP_M__bc"
)

all_variables=()
mapfile -t all_variables < <(
  merge_unique_arrays \
    "${input_variables[@]}" \
    "${other_variables_log[@]}" \
    "${other_variables_linear[@]}"
)

submit_Plotter() {

  if [ "$#" -eq 8 ]; then
    local Code=$1 # ex. ./bin/Plotter
    local VerName=$2 # ex. Alice
    local VarName=$3 # ex. deltaE
    local InputDir=$4 # ex. before_M_deltaE_selection
    local OutputName=$5 # ex. deltaE
    local OutputPath=$6 # ex. plot
    local SignalTypes=$7
    local BackgroundTypes=$8
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}"
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/log"
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/err"

    bsub -q l -J Plotter -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log/${InputDir}_${OutputName}.log" -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err/${InputDir}_${OutputName}.err" ${Code} "${VarName}" "./${VerName}/${Analysis_VerName}/" "/${InputDir}/" "./${VerName}/${Analysis_VerName}/${OutputPath}" "${OutputName}" "${SignalTypes}" "${BackgroundTypes}"
  elif [ "$#" -eq 10 ]; then
    local Code=$1 # ex. ./bin/Plotter
    local VerName=$2 # ex. Alice
    local VarName=$3 # ex. deltaE
    local VarMin=$4
    local VarMax=$5
    local InputDir=$6 # ex. before_M_deltaE_selection
    local OutputName=$7 # ex. deltaE
    local OutputPath=$8 # ex. plot
    local SignalTypes=$9
    local BackgroundTypes=${10}
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}"
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/log"
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/err"

    bsub -q l -J Plotter -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log/${InputDir}_${OutputName}.log" -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err/${InputDir}_${OutputName}.err" ${Code} "${VarName}" "./${VerName}/${Analysis_VerName}/" "/${InputDir}/" "./${VerName}/${Analysis_VerName}/${OutputPath}" "${OutputName}" "${SignalTypes}" "${BackgroundTypes}" "$VarMin" "$VarMax"
  fi

}

 
#code="${Belle_tau_DIR}/analysis_code/bin/Plotter"
#VarName="deltaE"
#submit_Plotter ${code} ${Analysis_Name} ${VarName} -1.1 0.6 "before_M_deltaE_selection" "before_M_deltaE_selection_deltaE" "plot" "${Signal_Type}" "${Background_Types_STR}"

#code="${Belle_tau_DIR}/analysis_code/bin/Plotter"
#VarName="M"
#submit_Plotter ${code} ${Analysis_Name} ${VarName} 1.3 2.1 "before_M_deltaE_selection" "before_M_deltaE_selection_M" "plot" "${Signal_Type}" "${Background_Types_STR}"

code="${Belle_tau_DIR}/analysis_code/bin/Plotter"
VarName="deltaE"
submit_Plotter ${code} ${Analysis_Name} ${VarName} -0.4 0.0 "before_strict_M_deltaE_selection" "before_strict_M_deltaE_selection_deltaE" "plot" "${Signal_Type}" "${Background_Types_STR}"

code="${Belle_tau_DIR}/analysis_code/bin/Plotter"
VarName="M"
submit_Plotter ${code} ${Analysis_Name} ${VarName} 0.5 1.7 "before_strict_M_deltaE_selection" "before_strict_M_deltaE_selection_M" "plot" "${Signal_Type}" "${Background_Types_STR}"

Directories=(
    "before_PrimarypionID_selection"
    "before_SecondarypionID_selection"
    "before_ThirdpionID_selection"
    "before_SecondarymuonP_selection"
    "before_theta_miss_cut"
    "before_thrust_cut"
    "before_Eecl_cut"
    "before_diffthrust_cut"
    "before_avgthrust_cut"
    "before_missingEnergy_cut"
    "before_flighttime_cut"
    "before_significance_distance_cut"
    "before_KS0_M_cut"
    "before_strict_M_deltaE_selection"
#    "final_output"
)

for ((i = 0; i < ${#Directories[@]}; i++)); do
  Directory="${Directories[i]}"

  for ((j = 0; j < ${#input_variables[@]}; j++)); do
    code="${Belle_tau_DIR}/analysis_code/bin/Plotter"
    VarName="${input_variables[j]}"
    submit_Plotter ${code} ${Analysis_Name} ${VarName} "${Directory}" "${Directory}_${VarName}" "plot" "${Signal_Type}" "${Background_Types_STR}"
  done

  for ((j = 0; j < ${#other_variables_log[@]}; j++)); do
    code="${Belle_tau_DIR}/analysis_code/bin/Plotter_log"
    VarName="${other_variables_log[j]}"
    submit_Plotter ${code} ${Analysis_Name} ${VarName} "${Directory}" "${Directory}_${VarName}" "plot" "${Signal_Type}" "${Background_Types_STR}"
  done

  for ((j = 0; j < ${#other_variables_linear[@]}; j++)); do
    code="${Belle_tau_DIR}/analysis_code/bin/Plotter"
    VarName="${other_variables_linear[j]}"
    submit_Plotter ${code} ${Analysis_Name} ${VarName} "${Directory}" "${Directory}_${VarName}" "plot" "${Signal_Type}" "${Background_Types_STR}"
  done

done

