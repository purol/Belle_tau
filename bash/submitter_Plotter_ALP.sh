#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.+-eE]\+\)_life\([0-9.+-eE]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\).*/\1 \2 \3 \4/p' | \
  sort -u
}

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
)

all_variables=()
mapfile -t all_variables < <(
  merge_unique_arrays \
    "${input_variables[@]}" \
    "${other_variables_log[@]}" \
    "${other_variables_linear[@]}"
)

submit_Plotter() {

  if [ "$#" -eq 10 ]; then
    local Code=$1 # ex. ./bin/Plotter
    local VerName=$2 # ex. Alice
    local VarName=$3 # ex. deltaE
    local InputDir=$4 # ex. before_M_deltaE_selection
    local OutputName=$5 # ex. deltaE
    local OutputPath=$6 # ex. plot
    local SignalTypes=$7
    local BackgroundTypes=$8
    local SignalLegends=${9}
    local BackgroundLegends=${10}

    get_params "./${VerName}/${Analysis_VerName}/ALP/${InputDir}" | while read mass life A B; do
      mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}_${mass}_${life}_${A}_${B}"
      mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}_${mass}_${life}_${A}_${B}/log"
      mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}_${mass}_${life}_${A}_${B}/err"

      bsub -q l -J Plotter -o "./${VerName}/${Analysis_VerName}/${OutputPath}_${mass}_${life}_${A}_${B}/log/${InputDir}_${OutputName}.log" -e "./${VerName}/${Analysis_VerName}/${OutputPath}_${mass}_${life}_${A}_${B}/err/${InputDir}_${OutputName}.err" ${Code} "${VarName}" "./${VerName}/${Analysis_VerName}/" "/${InputDir}/" "./${VerName}/${Analysis_VerName}/${OutputPath}_${mass}_${life}_${A}_${B}" "${OutputName}" "${SignalTypes}" "${BackgroundTypes}" "${SignalLegends}" "${BackgroundLegends}" "${mass}" "${life}" "${A}" "${B}"
    done

  elif [ "$#" -eq 12 ]; then
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
    local SignalLegends=${11}
    local BackgroundLegends=${12}

    get_params "./${VerName}/${Analysis_VerName}/ALP/${InputDir}" | while read mass life A B; do
      mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}_${mass}_${life}_${A}_${B}"
      mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}_${mass}_${life}_${A}_${B}/log"
      mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}_${mass}_${life}_${A}_${B}/err"

      bsub -q l -J Plotter -o "./${VerName}/${Analysis_VerName}/${OutputPath}_${mass}_${life}_${A}_${B}/log/${InputDir}_${OutputName}.log" -e "./${VerName}/${Analysis_VerName}/${OutputPath}_${mass}_${life}_${A}_${B}/err/${InputDir}_${OutputName}.err" ${Code} "${VarName}" "./${VerName}/${Analysis_VerName}/" "/${InputDir}/" "./${VerName}/${Analysis_VerName}/${OutputPath}_${mass}_${life}_${A}_${B}" "${OutputName}" "${SignalTypes}" "${BackgroundTypes}" "${SignalLegends}" "${BackgroundLegends}" "$VarMin" "$VarMax" "${mass}" "${life}" "${A}" "${B}"
    done

  fi

}

 
#code="${Belle_tau_DIR}/analysis_code/bin/Plotter_ALP"
#VarName="deltaE"
#submit_Plotter ${code} ${Analysis_Name} ${VarName} -1.1 0.6 "before_M_deltaE_selection" "before_M_deltaE_selection_deltaE" "plot" "${ALP_Type}" "${Background_Types_STR}" "${ALP_Legends}" "${Background_Legends_STR}"

#code="${Belle_tau_DIR}/analysis_code/bin/Plotter_ALP"
#VarName="M"
#submit_Plotter ${code} ${Analysis_Name} ${VarName} 1.3 2.1 "before_M_deltaE_selection" "before_M_deltaE_selection_M" "plot" "${ALP_Type}" "${Background_Types_STR}" "${ALP_Legends}" "${Background_Legends_STR}"

code="${Belle_tau_DIR}/analysis_code/bin/Plotter_ALP"
VarName="deltaE"
submit_Plotter ${code} ${Analysis_Name} ${VarName} -0.5 0.4 "before_strict_M_deltaE_selection" "before_strict_M_deltaE_selection_deltaE" "plot" "${ALP_Type}" "${Background_Types_STR}" "${ALP_Legends}" "${Background_Legends_STR}"

code="${Belle_tau_DIR}/analysis_code/bin/Plotter_ALP"
VarName="M"
submit_Plotter ${code} ${Analysis_Name} ${VarName} 1.4 2.1 "before_strict_M_deltaE_selection" "before_strict_M_deltaE_selection_M" "plot" "${ALP_Type}" "${Background_Types_STR}" "${ALP_Legends}" "${Background_Legends_STR}"

Directories=(
    "before_PrimarymuonID_selection"
    "before_SecondarymuonID_selection"
    "before_ThirdmuonID_selection"
    "before_SecondarymuonP_selection"
    "before_theta_miss_cut"
    "before_thrust_cut"
    "before_Eecl_cut"
    "before_diffthrust_cut"
    "before_avgthrust_cut"
    "before_missingEnergy_cut"
    "before_strict_M_deltaE_selection"
    "final_output"
)

for ((i = 0; i < ${#Directories[@]}; i++)); do
  Directory="${Directories[i]}"

  for ((j = 0; j < ${#input_variables[@]}; j++)); do
    code="${Belle_tau_DIR}/analysis_code/bin/Plotter_ALP"
    VarName="${input_variables[j]}"
    submit_Plotter ${code} ${Analysis_Name} ${VarName} "${Directory}" "${Directory}_${VarName}" "plot" "${ALP_Type}" "${Background_Types_STR}" "${ALP_Legends}" "${Background_Legends_STR}"
  done

  for ((j = 0; j < ${#other_variables_log[@]}; j++)); do
    code="${Belle_tau_DIR}/analysis_code/bin/Plotter_log_ALP"
    VarName="${other_variables_log[j]}"
    submit_Plotter ${code} ${Analysis_Name} ${VarName} "${Directory}" "${Directory}_${VarName}" "plot" "${ALP_Type}" "${Background_Types_STR}" "${ALP_Legends}" "${Background_Legends_STR}"
  done

  for ((j = 0; j < ${#other_variables_linear[@]}; j++)); do
    code="${Belle_tau_DIR}/analysis_code/bin/Plotter_ALP"
    VarName="${other_variables_linear[j]}"
    submit_Plotter ${code} ${Analysis_Name} ${VarName} "${Directory}" "${Directory}_${VarName}" "plot" "${ALP_Type}" "${Background_Types_STR}" "${ALP_Legends}" "${Background_Legends_STR}"
  done

done

