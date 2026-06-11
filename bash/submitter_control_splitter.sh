#!/bin/bash

export Analysis_Name="Konpaku_CTRL"
export Analysis_VerName="v000"

export Belle_tau_DIR="/home/belle2/junewoo/storage_b2/tau_workspace/Belle_tau"
export Ntuple_DIR="/home/belle2/junewoo/storage_ghi/tau_Ntuple_CTRL"

submit_splitter() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local SampleName=$3 # ex. MUMUTAUTAU

  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/output"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/log"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/err"

  for file in "${Ntuple_DIR}/${VerName}/${SampleName}/MC15ri"/*.root; do
    filename=$(basename "$file" .root) # without path, without extension
    bsub -q s -J Split -o "./${VerName}/${Analysis_VerName}/${SampleName}/log/${filename}.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/err/${filename}.err" ${Code} "${Ntuple_DIR}/${VerName}/${SampleName}/MC15ri" "${filename}.root" "./${VerName}/${Analysis_VerName}/${SampleName}/output/" "./${VerName}/${Analysis_VerName}/${SampleName}/output/"
    sleep 0.3s
  done

}

Types=("TAUPAIR_original_notOF")

code="${Belle_tau_DIR}/analysis_code/bin/Control_splitter"
for Type in "${Types[@]}"; do
    submit_splitter ${code} ${Analysis_Name} ${Type}
    sleep 0.5s
done


