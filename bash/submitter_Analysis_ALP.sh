#!/bin/bash

submit_analysis() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local SampleName=$3 # ex. MUMUTAUTAU

  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/before_SecondarymuonP_selection"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/before_theta_miss_cut"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/before_thrust_cut"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/before_Eecl_cut"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/before_diffthrust_cut"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/before_avgthrust_cut"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/before_missingEnergy_cut"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/before_strict_M_deltaE_selection"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/log"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/err"

  if compgen -G "${Ntuple_DIR}/${VerName}/${SampleName}/MC15ri/*.root" > /dev/null; then
    for file in "${Ntuple_DIR}/${VerName}/${SampleName}/MC15ri"/*.root; do
      filename=$(basename "$file" .root) # without path, without extension
      bsub -q s -J Analyze -o "./${VerName}/${Analysis_VerName}/${SampleName}/log/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/err/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.err" ${Code} "${Ntuple_DIR}/${VerName}/${SampleName}/MC15ri" "${filename}.root" "./${VerName}/${Analysis_VerName}/${SampleName}"
    done
  fi

}

submit_logger() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local SampleName=$3 # ex. MUMUTAUTAU

  if compgen -G "${Ntuple_DIR}/${VerName}/${SampleName}/MC15ri/*.root" > /dev/null; then
    bsub -q l -J Logger -o "./${VerName}/${Analysis_VerName}/${SampleName}/${SampleName}_${VerName}_${Analysis_VerName}.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/${SampleName}_${VerName}_${Analysis_VerName}.err" ${Code} "${Ntuple_DIR}/${VerName}/${SampleName}/MC15ri"
  fi

}

Types=("CHG" "MIX" "UUBAR" "DDBAR" "SSBAR" "CHARM" "MUMU" "EE" "EEEE" "EEMUMU" "EEPIPI" "EEKK" "EEPP" "PIPIISR" "PIPIPI0ISR" "KKISR" "GG" "EETAUTAU" "K0K0BARISR" "MUMUMUMU" "MUMUTAUTAU" "TAUTAUTAUTAU" "TAUPAIR" "BBs" "BsBs" "ALP")

code="${Belle_tau_DIR}/analysis_code/bin/Analysis_main_ALP"
for Type in "${Types[@]}"; do
    submit_analysis ${code} ${Analysis_Name} ${Type}
    sleep 0.5s
done

code="${Belle_tau_DIR}/analysis_code/bin/Analysis_main_log_ALP"
for Type in "${Types[@]}"; do
    submit_logger ${code} ${Analysis_Name} ${Type}
    sleep 0.5s
done