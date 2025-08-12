#!/bin/bash

# predefined input variables
input_variables=(
    "roeM__bocleanMask__bc"
    "roeDeltae__bocleanMask__bc"
    "first_muon_p"
    "second_muon_p"
    "third_muon_p"
    "thrustBm__bocleanMask__bc"
    "thrustOm__bocleanMask__bc"
    "nParticlesInList__bogamma__clevtshape_kinematics__bc"
    "cosAngleBetweenMomentumAndVertexVector"
    "nParticlesInList__bopi__pl__clevtshape_kinematics__bc"
    "charge_times_ROEcharge"
    "flightTime_dividedby_flightTimeErr"
    "missingEnergyOfEventCMS"
    "missingMass2OfEvent"
    "missingMomentumOfEventCMS_theta"
    "thrust"
    "cosTBTO__bocleanMask__bc"
    "chiProb"
    "angleToClosestInList__bopi__pl__clevtshape_kinematics__bc"
    "KSFWVariables__boet__cm__spcleanMask__bc"
    "KSFWVariables__bomm2__cm__spcleanMask__bc"
    "KSFWVariables__bohso00__cm__spcleanMask__bc"
    "KSFWVariables__bohso01__cm__spcleanMask__bc"
    "KSFWVariables__bohso02__cm__spcleanMask__bc"
    "KSFWVariables__bohso03__cm__spcleanMask__bc"
    "KSFWVariables__bohso04__cm__spcleanMask__bc"
    "KSFWVariables__bohso10__cm__spcleanMask__bc"
    "KSFWVariables__bohso12__cm__spcleanMask__bc"
    "KSFWVariables__bohso14__cm__spcleanMask__bc"
    "KSFWVariables__bohso20__cm__spcleanMask__bc"
    "KSFWVariables__bohso22__cm__spcleanMask__bc"
    "KSFWVariables__bohso24__cm__spcleanMask__bc"
#    "KSFWVariables__bohoo0__cm__spcleanMask__bc"
#    "KSFWVariables__bohoo1__cm__spcleanMask__bc"
#    "KSFWVariables__bohoo2__cm__spcleanMask__bc"
#    "KSFWVariables__bohoo3__cm__spcleanMask__bc"
#    "KSFWVariables__bohoo4__cm__spcleanMask__bc"
    "harmonicMomentThrust0"
    "harmonicMomentThrust1"
    "harmonicMomentThrust2"
    "harmonicMomentThrust3"
    "harmonicMomentThrust4"
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
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/log"
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/err"

  bsub -q l -J FBDTTRN -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log/${nTree}_${depth}_${shrinkage}_${subsample}_${binning}.log" -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err/${nTree}_${depth}_${shrinkage}_${subsample}_${binning}.err" ${Code} "${#input_variables[@]}" "${input_variables[@]}" "./${VerName}/${Analysis_VerName}" "./${VerName}/${Analysis_VerName}/${OutputPath}/out" "${nTree}" "${depth}" "${shrinkage}" "${subsample}" "${binning}"

}


code="${Belle_tau_DIR}/analysis_code/bin/FBDT_GridSearch_one"
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


code="${Belle_tau_DIR}/analysis_code/bin/FBDT_GridSearch_two"
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




