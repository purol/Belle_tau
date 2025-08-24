#!/bin/bash

submit_analysis() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local SampleName=$3 # ex. MUMUTAUTAU
  local InputDir=$4 # ex. final_output_train_after_application

  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/${InputDir}_after_cut"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/log_third_${InputDir}"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/err_third_${InputDir}"

  if compgen -G "./${VerName}/${Analysis_VerName}/${SampleName}/${InputDir}/*.root" > /dev/null; then
    for file in "./${VerName}/${Analysis_VerName}/${SampleName}/${InputDir}"/*.root; do
      filename=$(basename "$file" .root) # without path, without extension
      bsub -q s -J Analyze -o "./${VerName}/${Analysis_VerName}/${SampleName}/log_third_${InputDir}/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/err_third_${InputDir}/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.err" ${Code} "./${VerName}/${Analysis_VerName}/${SampleName}/${InputDir}" "${filename}.root" "./${VerName}/${Analysis_VerName}/${SampleName}/${InputDir}_after_cut" "./${VerName}/${Analysis_VerName}/" "./${VerName}/${Analysis_VerName}/model_one/FOM.log" "./${VerName}/${Analysis_VerName}/model_two/FOM.log"
    done
  fi

}

submit_logger() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local SampleName=$3 # ex. MUMUTAUTAU
  local InputDir=$4 # ex. final_output_train_after_application

  if compgen -G "./${VerName}/${Analysis_VerName}/${SampleName}/${InputDir}/*.root" > /dev/null; then
    bsub -q l -J Logger -o "./${VerName}/${Analysis_VerName}/${SampleName}/${SampleName}_${VerName}_${Analysis_VerName}_${InputDir}_third.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/${SampleName}_${VerName}_${Analysis_VerName}_${InputDir}_third.err" ${Code} "./${VerName}/${Analysis_VerName}/${SampleName}/${InputDir}" "root" "./${VerName}/${Analysis_VerName}/${InputDir}_after_cut" "./${VerName}/${Analysis_VerName}/" "./${VerName}/${Analysis_VerName}/model_one/FOM.log" "./${VerName}/${Analysis_VerName}/model_two/FOM.log"
  fi

}

Types=("CHG" "MIX" "UUBAR" "DDBAR" "SSBAR" "CHARM" "MUMU" "EE" "EEEE" "EEMUMU" "EEPIPI" "EEKK" "EEPP" "PIPIISR" "KKISR" "GG" "EETAUTAU" "K0K0BARISR" "MUMUMUMU" "MUMUTAUTAU" "TAUTAUTAUTAU" "TAUPAIR" "SIGNAL")

code="${Belle_tau_DIR}/analysis_code/bin/Analysis_main_third"
for Type in "${Types[@]}"; do
    submit_analysis ${code} ${Analysis_Name} ${Type} "final_output_test_after_application"
    sleep 0.5s
done

code="${Belle_tau_DIR}/analysis_code/bin/Analysis_main_third_log"
for Type in "${Types[@]}"; do
    submit_logger ${code} ${Analysis_Name} ${Type} "final_output_test_after_application"
    sleep 0.5s
done
