#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.+-eE]\+\)_life\([0-9.+-eE]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\).*/\1 \2 \3 \4/p' | \
  sort -u
}

# predefined input variables
input_variables_one=(
    "missingEnergyOfEventCMS"
    "cleoConeThrust0"
    "diff_cosToThrustOfEvent_CM"
    "second_muon_p"
    "cosAngleBetweenMomentumAndVertexVector"
    "first_muon_p"
    "missingMomentumOfEventCMS_theta"
    "totalEnergyOfParticlesInList__bogamma__clevtshape_kinematics__bc"
    "useCMSFrame__bopx__bc"
    "dphi"
    "cosTBz__bocleanMask__bc"
    "third_muon_theta"
    "dcosTheta"
    "angleToClosestInList__bopi__pl__clevtshape_kinematics__bc"
    "CleoConeCS__bo2__cm__spcleanMask__bc"
    "harmonicMomentThrust3"
    "CleoConeCS__bo3__cm__spcleanMask__bc"
    "aplanarity"
    "KSFWVariables__bohso01__cm__spcleanMask__bc"
    "KSFWVariables__bohso03__cm__spcleanMask__bc"
    "cosToThrustOfEvent"
    "KSFWVariables__bohso00__cm__spcleanMask__bc"
    "KSFWVariables__bohoo3__cm__spcleanMask__bc"
    "cleoConeThrust5"
    "cleoConeThrust6"
    "cleoConeThrust8"
    "KSFWVariables__bohoo0__cm__spcleanMask__bc"
    "cleoConeThrust7"
    "charge_times_ROEcharge"
    "dr"
)
input_variables_two=(
    "missingEnergyOfEventCMS"
    "second_muon_p"
    "diff_cosToThrustOfEvent_CM"
    "missingMomentumOfEventCMS_Px"
    "cosAngleBetweenMomentumAndVertexVector"
    "first_muon_p"
    "roeEextra__bocleanMask__bc"
    "angleToClosestInList__bopi__pl__clevtshape_kinematics__bc"
    "dcosTheta"
    "missingMomentumOfEventCMS_theta"
    "dr"
    "KSFWVariables__bohoo3__cm__spcleanMask__bc"
    "third_muon_theta"
    "dphi"
    "KSFWVariables__bohoo0__cm__spcleanMask__bc"
    "cleoConeThrust1"
    "CleoConeCS__bo2__cm__spcleanMask__bc"
    "KSFWVariables__bohso14__cm__spcleanMask__bc"
    "harmonicMomentThrust3"
    "nParticlesInList__bopi__pl__clevtshape_kinematics__bc"
    "KSFWVariables__bohso01__cm__spcleanMask__bc"
    "cleoConeThrust2"
    "KSFWVariables__bohso04__cm__spcleanMask__bc"
    "aplanarity"
    "cleoConeThrust3"
    "KSFWVariables__bohso03__cm__spcleanMask__bc"
    "cleoConeThrust5"
    "cleoConeThrust6"
    "CleoConeCS__bo8__cm__spcleanMask__bc"
    "cleoConeThrust8"
    "cleoConeThrust4"
)

submit_GridSearch() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local nTree=$3
  local depth=$4
  local shrinkage=$5
  local subsample=$6
  local binning=$7
  local OutputPath=$8
  local array_name=$9

  # nameref to the array
  local -n input_variables_ref=${array_name}

  get_params "./${VerName}/${Analysis_VerName}/ALP/final_output" | while read mass life A B; do

    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}"
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/out_${mass}_${life}_${A}_${B}"
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/log_AUC_test_${mass}_${life}_${A}_${B}"
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/err_AUC_test_${mass}_${life}_${A}_${B}"

    bsub -q l -J AUCTST -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log_AUC_test_${mass}_${life}_${A}_${B}/${nTree}_${depth}_${shrinkage}_${subsample}_${binning}.log" -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err_AUC_test_${mass}_${life}_${A}_${B}/${nTree}_${depth}_${shrinkage}_${subsample}_${binning}.err" ${Code} "${#input_variables_ref[@]}" "${input_variables_ref[@]}" "./${VerName}/${Analysis_VerName}" "./${VerName}/${Analysis_VerName}/${OutputPath}/out_${mass}_${life}_${A}_${B}" "${nTree}" "${depth}" "${shrinkage}" "${subsample}" "${binning}" "${mass}" "${life}" "${A}" "${B}"
  done
}


code="${Belle_tau_DIR}/analysis_code/bin/FBDT_AUC_test_one_ALP"
output="GridSearch_one"
for nTree in 250 500 750
do
  for depth in 1 2
  do
    for shrinkage in 0.01 0.1 0.2
    do
      for subsample in 0.01 0.2 0.5
      do
        for binning in 6 8
        do
          submit_GridSearch ${code} ${Analysis_Name} ${nTree} ${depth} ${shrinkage} ${subsample} ${binning} ${output} "input_variables_one"
        done
      done
    done
  done
done

code="${Belle_tau_DIR}/analysis_code/bin/FBDT_AUC_test_two_ALP"
output="GridSearch_two"
for nTree in 100 250 500 750
do
  for depth in 1 2
  do
    for shrinkage in 0.01 0.1 0.2
    do
      for subsample in 0.01 0.2 0.5
      do
        for binning in 6 8
        do
          submit_GridSearch ${code} ${Analysis_Name} ${nTree} ${depth} ${shrinkage} ${subsample} ${binning} ${output} "input_variables_two"
        done
      done
    done
  done
done



