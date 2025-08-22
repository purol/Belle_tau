#!/bin/bash

submit_code() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local VarName=$3
  local Nbin=$4
  local MIN=$5
  local MAX=$6
  local OutputPath=$7

  bsub -q l -J KSTEST -o "/dev/null" ${Code} "${VarName}" "${Nbin}" "${MIN}" "${MAX}" "./${VerName}/${Analysis_VerName}" "./${VerName}/${Analysis_VerName}/${OutputPath}" "KS_${VarName}.png"

}


code="${Belle_tau_DIR}/analysis_code/bin/KS_test_half_one"
varname="BDT_output_1"
Nbin="50"
MIN="0.0"
MAX="1.0"
output="AutogluonModels"
submit_code ${code} ${Analysis_Name} ${varname} ${Nbin} ${MIN} ${MAX} ${output}

code="${Belle_tau_DIR}/analysis_code/bin/KS_test_half_two"
varname="BDT_output_2"
Nbin="50"
MIN="0.0"
MAX="1.0"
output="AutogluonModels"
submit_code ${code} ${Analysis_Name} ${varname} ${Nbin} ${MIN} ${MAX} ${output}

