#!/bin/bash

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

  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}"
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/out"
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/log_AUC_train"
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/err_AUC_train"

  bsub -q l -J AUCTRN -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log_AUC_train/${nTree}_${depth}_${shrinkage}_${subsample}_${binning}.log" -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err_AUC_train/${nTree}_${depth}_${shrinkage}_${subsample}_${binning}.err" ${Code} "${#input_variables_ref[@]}" "${input_variables_ref[@]}" "./${VerName}/${Analysis_VerName}" "./${VerName}/${Analysis_VerName}/${OutputPath}/out" "${nTree}" "${depth}" "${shrinkage}" "${subsample}" "${binning}"

}


code="${Belle_tau_DIR}/analysis_code/bin/FBDT_AUC_train_one"
output="GridSearch_one"
for nTree in 100 250 500 750 1000
do
  for depth in 1 2 3 4
  do
    for shrinkage in 0.01 0.05 0.1
    do
      for subsample in 0.01 0.3 0.4 0.5 0.6 0.7
      do
        for binning in 5 6 7 8 9
        do
          submit_GridSearch ${code} ${Analysis_Name} ${nTree} ${depth} ${shrinkage} ${subsample} ${binning} ${output} "input_variables_one"
        done
      done
    done
  done
done

code="${Belle_tau_DIR}/analysis_code/bin/FBDT_AUC_train_two"
output="GridSearch_two"
for nTree in 100 250 500 750 1000
do
  for depth in 1 2 3 4
  do
    for shrinkage in 0.01 0.05 0.1
    do
      for subsample in 0.01 0.3 0.4 0.5 0.6 0.7
      do
        for binning in 5 6 7 8 9
        do
          submit_GridSearch ${code} ${Analysis_Name} ${nTree} ${depth} ${shrinkage} ${subsample} ${binning} ${output} "input_variables_two"
        done
      done
    done
  done
done


