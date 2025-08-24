#!/bin/bash

submit_PunziFOM() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local BDTName=$3
  local OutputPath=$4

  bsub -q l -J FBDTFOM -o "./${VerName}/${Analysis_VerName}/${OutputPath}/FOM.log" -e "./${VerName}/${Analysis_VerName}/${OutputPath}/FOM.err" ${Code} "./${VerName}/${Analysis_VerName}" ${BDTName} ${OutputPath}

}


code="${Belle_tau_DIR}/analysis_code/bin/FBDT_PunziFOM_one"
bdtname="BDT_output_1"
output="model_one"
submit_PunziFOM ${code} ${Analysis_Name} ${bdtname} ${output}

code="${Belle_tau_DIR}/analysis_code/bin/FBDT_PunziFOM_two"
bdtname="BDT_output_2"
output="model_two"
submit_PunziFOM ${code} ${Analysis_Name} ${bdtname} ${output}

