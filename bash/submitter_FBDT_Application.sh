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

submit_Application() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local SampleName=$3 # ex. MUMUTAUTAU

  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test_after_application"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train_after_application"

  if compgen -G "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test/*.root" > /dev/null; then
    for file in "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test"/*.root; do
      filename=$(basename "$file" .root) # without path, without extension
      bsub -q l -J FBDTAPP -o "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test_after_application/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test_after_application/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.err" ${Code} "${#input_variables_one[@]}" "${input_variables_one[@]}" "${#input_variables_two[@]}" "${input_variables_two[@]}" "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test" ${filename} "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test_after_application" "./${VerName}/${Analysis_VerName}/GridSearch_one" "./${VerName}/${Analysis_VerName}/GridSearch_two"
    done
  fi

  if compgen -G "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train/*.root" > /dev/null; then
    for file in "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train"/*.root; do
      filename=$(basename "$file" .root) # without path, without extension
      bsub -q l -J FBDTAPP -o "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train_after_application/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train_after_application/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.err" ${Code} "${#input_variables_one[@]}" "${input_variables_one[@]}" "${#input_variables_two[@]}" "${input_variables_two[@]}" "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train" ${filename} "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train_after_application" "./${VerName}/${Analysis_VerName}/GridSearch_one" "./${VerName}/${Analysis_VerName}/GridSearch_two"
    done
  fi

}

Types=("CHG" "MIX" "UUBAR" "DDBAR" "SSBAR" "CHARM" "MUMU" "EE" "EEEE" "EEMUMU" "EEPIPI" "EEKK" "EEPP" "PIPIISR" "PIPIPI0ISR" "KKISR" "GG" "EETAUTAU" "K0K0BARISR" "MUMUMUMU" "MUMUTAUTAU" "TAUTAUTAUTAU" "TAUPAIR" "BBs" "BsBs" "SIGNAL")

code="${Belle_tau_DIR}/analysis_code/bin/FBDT_Application"
for Type in "${Types[@]}"; do
    submit_Application ${code} ${Analysis_Name} ${Type}
    sleep 0.5s
done

