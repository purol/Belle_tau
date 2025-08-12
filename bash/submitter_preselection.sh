#!/bin/bash


submit_preselection() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local SampleName=$3 # ex. MUMUTAUTAU

  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/preselection_output"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/log"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/err"

  for file in "${Ntuple_DIR}/${VerName}/${SampleName}/MC15ri"/*; do
    filename=$(basename "$file" .root) # without path, without extension
    if [ "$SampleName" == "SIGNAL" ]; then
      bsub -q l -J Presel -o "./${VerName}/${Analysis_VerName}/${SampleName}/log/${filename}.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/err/${filename}.err" ${Code} "${Ntuple_DIR}/${VerName}/${SampleName}/MC15ri" "${filename}.root" "./${VerName}/${Analysis_VerName}/${SampleName}"
    else
      bsub -q s -J Presel -o "./${VerName}/${Analysis_VerName}/${SampleName}/log/${filename}.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/err/${filename}.err" ${Code} "${Ntuple_DIR}/${VerName}/${SampleName}/MC15ri" "${filename}.root" "./${VerName}/${Analysis_VerName}/${SampleName}"
    fi
    sleep 0.5s
  done

}

Types=("CHG" "MIX" "UUBAR" "DDBAR" "SSBAR" "CHARM" "MUMU" "EE" "EEEE" "EEMUMU" "EEPIPI" "EEKK" "EEPP" "PIPIISR" "KKISR" "GG" "EETAUTAU" "K0K0BARISR" "MUMUMUMU" "MUMUTAUTAU" "TAUTAUTAUTAU" "TAUPAIR" "SIGNAL")

code="${Belle_tau_DIR}/analysis_code/bin/Preselection"
for Type in "${Types[@]}"; do
    submit_preselection ${code} ${Analysis_Name} ${Type}
    sleep 0.5s
done



