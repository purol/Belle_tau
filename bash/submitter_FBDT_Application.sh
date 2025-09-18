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

submit_Application() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local SampleName=$3 # ex. MUMUTAUTAU

  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test_after_application"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train_after_application"

  if compgen -G "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test/*.root" > /dev/null; then
    for file in "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test"/*.root; do
      filename=$(basename "$file" .root) # without path, without extension
      bsub -q l -J FBDTAPP -o "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test_after_application/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test_after_application/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.err" ${Code} "${#input_variables[@]}" "${input_variables[@]}" "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test" ${filename} "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test_after_application" "./${VerName}/${Analysis_VerName}/GridSearch_one" "./${VerName}/${Analysis_VerName}/GridSearch_two"
    done
  fi

  if compgen -G "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train/*.root" > /dev/null; then
    for file in "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train"/*.root; do
      filename=$(basename "$file" .root) # without path, without extension
      bsub -q l -J FBDTAPP -o "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train_after_application/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train_after_application/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.err" ${Code} "${#input_variables[@]}" "${input_variables[@]}" "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train" ${filename} "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train_after_application" "./${VerName}/${Analysis_VerName}/GridSearch_one" "./${VerName}/${Analysis_VerName}/GridSearch_two"
    done
  fi

}

Types=("CHG" "MIX" "UUBAR" "DDBAR" "SSBAR" "CHARM" "MUMU" "EE" "EEEE" "EEMUMU" "EEPIPI" "EEKK" "EEPP" "PIPIISR" "PIPIPI0ISR" "KKISR" "GG" "EETAUTAU" "K0K0BARISR" "MUMUMUMU" "MUMUTAUTAU" "TAUTAUTAUTAU" "TAUPAIR" "BBs" "BsBs" "SIGNAL")

code="${Belle_tau_DIR}/analysis_code/bin/FBDT_Application"
for Type in "${Types[@]}"; do
    submit_Application ${code} ${Analysis_Name} ${Type}
    sleep 0.5s
done

