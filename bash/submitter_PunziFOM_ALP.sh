#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.+-eE]\+\)_life\([0-9.+-eE]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\).*/\1 \2 \3 \4/p' | \
  sort -u
}

submit_PunziFOM() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local BDTName=$3
  local OutputPath=$4

  get_params "./${VerName}/${Analysis_VerName}/ALP/final_output" | while read mass life A B; do
    BDTName_ALP=${BDTName}_${mass}_${life}_${A}_${B}
    BDTName_ALP=${BDTName_ALP//-/m}

    bsub -q l -J FBDTFOM -o "./${VerName}/${Analysis_VerName}/${OutputPath}/FOM_${mass}_${life}_${A}_${B}.log" -e "./${VerName}/${Analysis_VerName}/${OutputPath}/FOM_${mass}_${life}_${A}_${B}.err" ${Code} "./${VerName}/${Analysis_VerName}" ${BDTName_ALP} ${OutputPath} "${mass}" "${life}" "${A}" "${B}"
  done
}


code="${Belle_tau_DIR}/analysis_code/bin/FBDT_PunziFOM_one_ALP"
bdtname="BDT_output_1"
output="GridSearch_one"
submit_PunziFOM ${code} ${Analysis_Name} ${bdtname} ${output}

code="${Belle_tau_DIR}/analysis_code/bin/FBDT_PunziFOM_two_ALP"
bdtname="BDT_output_2"
output="GridSearch_two"
submit_PunziFOM ${code} ${Analysis_Name} ${bdtname} ${output}

