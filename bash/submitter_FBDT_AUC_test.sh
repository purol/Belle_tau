#!/bin/bash

# predefined input variables
input_variables=(
    "foxWolframR1"
    "cosTBTO__bocleanMask__bc"
    "harmonicMomentThrust1"
    "cosAngleBetweenMomentumAndVertexVector"
    "first_muon_p"
    "second_muon_p"
    "third_muon_p"
    "roeEextra__bocleanMask__bc"
    "dcosTheta"
    "angleToClosestInList__bopi__pl__clevtshape_kinematics__bc"
    "missingMomentumOfEventCMS_theta"
    "cosTBz__bocleanMask__bc"
    "useCMSFrame__bophi__bc"
    "flightTime_dividedby_flightTimeErr"
    "cosToThrustOfEvent"
    "KSFWVariables__boet__cm__spcleanMask__bc"
    "KSFWVariables__bohso00__cm__spcleanMask__bc"
    "KSFWVariables__bohso10__cm__spcleanMask__bc"
    "KSFWVariables__bohso14__cm__spcleanMask__bc"
    "CleoConeCS__bo1__cm__spcleanMask__bc"
    "CleoConeCS__bo2__cm__spcleanMask__bc"
    "cleoConeThrust1"
    "cleoConeThrust2"
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

  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}"
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/out"
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/log_AUC_test"
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/err_AUC_test"

  bsub -q l -J AUCTST -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log_AUC_test/${nTree}_${depth}_${shrinkage}_${subsample}_${binning}.log" -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err_AUC_test/${nTree}_${depth}_${shrinkage}_${subsample}_${binning}.err" ${Code} "${#input_variables[@]}" "${input_variables[@]}" "./${VerName}/${Analysis_VerName}" "./${VerName}/${Analysis_VerName}/${OutputPath}/out" "${nTree}" "${depth}" "${shrinkage}" "${subsample}" "${binning}"

}


code="${Belle_tau_DIR}/analysis_code/bin/FBDT_AUC_test_one"
output="GridSearch_one"
for nTree in 100 500 1000 1500 2000
do
  for depth in 1 2 3 4
  do
    for shrinkage in 0.01 0.05 0.1
    do
      for subsample in 0.01 0.3 0.4 0.5 0.6 0.7
      do
        for binning in 5 6 7 8 9
        do
          submit_GridSearch ${code} ${Analysis_Name} ${nTree} ${depth} ${shrinkage} ${subsample} ${binning} ${output}
        done
      done
    done
  done
done

code="${Belle_tau_DIR}/analysis_code/bin/FBDT_AUC_test_two"
output="GridSearch_two"
for nTree in 100 500 1000 1500 2000
do
  for depth in 1 2 3 4
  do
    for shrinkage in 0.01 0.05 0.1
    do
      for subsample in 0.01 0.3 0.4 0.5 0.6 0.7
      do
        for binning in 5 6 7 8 9
        do
          submit_GridSearch ${code} ${Analysis_Name} ${nTree} ${depth} ${shrinkage} ${subsample} ${binning} ${output}
        done
      done
    done
  done
done



