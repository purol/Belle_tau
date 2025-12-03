#!/bin/bash

submit_code() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local NToys=$3
  local indicator=$4
  local LogName=$5

  mkdir -p "./${VerName}/${Analysis_VerName}/cal_out"
  mkdir -p "./${VerName}/${Analysis_VerName}/cal_log"

  bsub -q s -J SYSTCAL -o "./${VerName}/${Analysis_VerName}/cal_log/${LogName}_${indicator}.log" ${Code} "./${VerName}/${Analysis_VerName}" "final_output_test_after_application_after_cut" "./${VerName}/${Analysis_VerName}/cal_out" ${NToys} ${indicator}
}


code="${Belle_tau_DIR}/analysis_code/bin/muonID_calculator"
NToys=10
for i in {0..99}; do
    submit_code ${code} ${Analysis_Name} ${NToys} ${i} "PID_calculator"
done


code="${Belle_tau_DIR}/analysis_code/bin/luminosity_calculator"
NToys=10
for i in {0..99}; do
    submit_code ${code} ${Analysis_Name} ${NToys} ${i} "L_calculator"
done

