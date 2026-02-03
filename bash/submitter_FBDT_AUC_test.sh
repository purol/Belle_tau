#!/bin/bash

# predefined input variables
input_variables_one=(
    "missingEnergyOfEventCMS"
    "cleoConeThrust0"
    "harmonicMomentThrust4"
    "diff_cosToThrustOfEvent_CM"
    "second_muon_p"
    "harmonicMomentThrust1"
    "CleoConeCS__bo1__cm__spcleanMask__bc"
    "roeEextra__bocleanMask__bc"
    "first_muon_p"
    "avg_cosToThrustOfEvent_CM"
    "missingMomentumOfEventCMS_theta"
    "sphericity"
    "third_muon_p"
    "useCMSFrame__bopz__bc"
    "cosAngleBetweenMomentumAndVertexVector"
    "foxWolframR3"
    "angleToClosestInList__bopi__pl__clevtshape_kinematics__bc"
    "cosTBTO__bocleanMask__bc"
    "totalEnergyOfParticlesInList__bogamma__clevtshape_kinematics__bc"
    "KSFWVariables__bohso24__cm__spcleanMask__bc"
    "roeE__bocleanMask__bc"
    "thrustAxisCosTheta"
    "cleoConeThrust1"
    "KSFWVariables__boet__cm__spcleanMask__bc"
    "CleoConeCS__bo2__cm__spcleanMask__bc"
    "KSFWVariables__bohso14__cm__spcleanMask__bc"
    "aplanarity"
    "CleoConeCS__bo3__cm__spcleanMask__bc"
    "KSFWVariables__bohso01__cm__spcleanMask__bc"
    "KSFWVariables__bohso04__cm__spcleanMask__bc"
    "charge_times_ROEcharge"
)
input_variables_two=(
    "visibleEnergyOfEventCMS"
    "second_muon_p"
    "roeE__bocleanMask__bc"
    "diff_cosToThrustOfEvent_CM"
    "harmonicMomentThrust1"
    "avg_cosToThrustOfEvent_CM"
    "third_muon_p"
    "first_muon_p"
    "sphericity"
    "foxWolframR3"
    "roeEextra__bocleanMask__bc"
    "KSFWVariables__bohoo3__cm__spcleanMask__bc"
    "KSFWVariables__bohoo1__cm__spcleanMask__bc"
    "totalEnergyOfParticlesInList__bogamma__clevtshape_kinematics__bc"
    "KSFWVariables__bohso24__cm__spcleanMask__bc"
    "angleToClosestInList__bopi__pl__clevtshape_kinematics__bc"
    "missingMomentumOfEventCMS_Pz"
    "KSFWVariables__bohso00__cm__spcleanMask__bc"
    "nParticlesInList__bopi__pl__clevtshape_kinematics__bc"
    "pzRecoil"
    "missingMomentumOfEvent_theta"
    "cosAngleBetweenMomentumAndVertexVector"
    "KSFWVariables__bohoo0__cm__spcleanMask__bc"
    "cleoConeThrust1"
    "KSFWVariables__bohso14__cm__spcleanMask__bc"
    "CleoConeCS__bo2__cm__spcleanMask__bc"
    "cleoConeThrust2"
    "KSFWVariables__bohso01__cm__spcleanMask__bc"
    "KSFWVariables__bohso04__cm__spcleanMask__bc"
    "KSFWVariables__bohoo4__cm__spcleanMask__bc"
    "charge_times_ROEcharge"
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
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/log_AUC_test"
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/err_AUC_test"

  bsub -q l -J AUCTST -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log_AUC_test/${nTree}_${depth}_${shrinkage}_${subsample}_${binning}.log" -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err_AUC_test/${nTree}_${depth}_${shrinkage}_${subsample}_${binning}.err" ${Code} "${#input_variables_ref[@]}" "${input_variables_ref[@]}" "./${VerName}/${Analysis_VerName}" "./${VerName}/${Analysis_VerName}/${OutputPath}/out" "${nTree}" "${depth}" "${shrinkage}" "${subsample}" "${binning}"

}


code="${Belle_tau_DIR}/analysis_code/bin/FBDT_AUC_test_one"
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

code="${Belle_tau_DIR}/analysis_code/bin/FBDT_AUC_test_two"
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



