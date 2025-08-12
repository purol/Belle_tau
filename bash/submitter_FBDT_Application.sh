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

Types=("CHG" "MIX" "UUBAR" "DDBAR" "SSBAR" "CHARM" "MUMU" "EE" "EEEE" "EEMUMU" "EEPIPI" "EEKK" "EEPP" "PIPIISR" "KKISR" "GG" "EETAUTAU" "K0K0BARISR" "MUMUMUMU" "MUMUTAUTAU" "TAUTAUTAUTAU" "TAUPAIR" "SIGNAL")

code="${Belle_tau_DIR}/analysis_code/bin/FBDT_Application"
for Type in "${Types[@]}"; do
    submit_Application ${code} ${Analysis_Name} ${Type}
    sleep 0.5s
done

