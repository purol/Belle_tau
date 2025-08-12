#!/bin/bash

export Analysis_Name="test"
export Analysis_VerName="v000"

export Belle_tau_DIR="/home/belle2/junewoo/storage_b2/tau_workspace/Belle_tau"
export Ntuple_DIR="/home/belle2/junewoo/storage_ghi/tau_Ntuple"

submit_splitter() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local SampleName=$3 # ex. MUMUTAUTAU

  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/output"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/log"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/err"

  for file in "${Ntuple_DIR}/${VerName}/${SampleName}/MC15ri"/*; do
    filename=$(basename "$file" .root) # without path, without extension
    for i in {0..19}
    do
      if [ "$SampleName" == "SIGNAL" ]; then
        bsub -q l -J Split -o "./${VerName}/${Analysis_VerName}/${SampleName}/log/${filename}_20_${i}.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/err/${filename}_20_${i}.err" ${Code} "${Ntuple_DIR}/${VerName}/${SampleName}/MC15ri" "${filename}.root" "20" ${i} "./${VerName}/${Analysis_VerName}/${SampleName}/output/"
      else
        bsub -q s -J Split -o "./${VerName}/${Analysis_VerName}/${SampleName}/log/${filename}_20_${i}.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/err/${filename}_20_${i}.err" ${Code} "${Ntuple_DIR}/${VerName}/${SampleName}/MC15ri" "${filename}.root" "20" ${i} "./${VerName}/${Analysis_VerName}/${SampleName}/output/"
      fi
    done
    sleep 0.5s
  done

}

Types=("SIGNAL")

code="${Belle_tau_DIR}/analysis_code/bin/File_Splitter"
for Type in "${Types[@]}"; do
    submit_splitter ${code} ${Analysis_Name} ${Type}
    sleep 0.5s
done


