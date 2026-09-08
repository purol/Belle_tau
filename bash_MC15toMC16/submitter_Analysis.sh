#!/bin/bash

export Analysis_Name="Larva" # name of analysis
export Analysis_VerName="v000" # version of analysis

export Belle_tau_DIR="./" # analysis code path
export Ntuple_DIR="/home/belle2/junewoo/storage_ghi/tau_Ntuple_compare" # Ntuple path

submit_analysis() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local SampleName=$3 # ex. MUMUTAUTAU
  local MC_version=${4}

  mkdir -p "./${VerName}/${Analysis_VerName}_${MC_version}/${SampleName}"
  #mkdir -p "./${VerName}/${Analysis_VerName}_${MC_version}/${SampleName}/before_M_deltaE_selection"
  mkdir -p "./${VerName}/${Analysis_VerName}_${MC_version}/${SampleName}/before_PrimarymuonID_selection"
  mkdir -p "./${VerName}/${Analysis_VerName}_${MC_version}/${SampleName}/before_SecondarymuonID_selection"
  mkdir -p "./${VerName}/${Analysis_VerName}_${MC_version}/${SampleName}/before_ThirdmuonID_selection"
  mkdir -p "./${VerName}/${Analysis_VerName}_${MC_version}/${SampleName}/before_SecondarymuonP_selection"
  mkdir -p "./${VerName}/${Analysis_VerName}_${MC_version}/${SampleName}/before_theta_miss_cut"
  mkdir -p "./${VerName}/${Analysis_VerName}_${MC_version}/${SampleName}/before_thrust_cut"
  mkdir -p "./${VerName}/${Analysis_VerName}_${MC_version}/${SampleName}/before_Eecl_cut"
  mkdir -p "./${VerName}/${Analysis_VerName}_${MC_version}/${SampleName}/before_diffthrust_cut"
  mkdir -p "./${VerName}/${Analysis_VerName}_${MC_version}/${SampleName}/before_avgthrust_cut"
  mkdir -p "./${VerName}/${Analysis_VerName}_${MC_version}/${SampleName}/before_missingEnergy_cut"
  mkdir -p "./${VerName}/${Analysis_VerName}_${MC_version}/${SampleName}/before_strict_M_deltaE_selection"
  mkdir -p "./${VerName}/${Analysis_VerName}_${MC_version}/${SampleName}/log"
  mkdir -p "./${VerName}/${Analysis_VerName}_${MC_version}/${SampleName}/err"

  if compgen -G "${Ntuple_DIR}/${VerName}/${SampleName}/${MC_version}/*.root" > /dev/null; then
    for file in "${Ntuple_DIR}/${VerName}/${SampleName}/${MC_version}"/*.root; do
      filename=$(basename "$file" .root) # without path, without extension
      bsub -q s \
      -J Analyze \
      -o "./${VerName}/${Analysis_VerName}_${MC_version}/${SampleName}/log/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.log" \
      -e "./${VerName}/${Analysis_VerName}_${MC_version}/${SampleName}/err/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.err" \
      ${Code} \
      "${Ntuple_DIR}/${VerName}/${SampleName}/${MC_version}" \
      "${filename}.root" \
      "./${VerName}/${Analysis_VerName}_${MC_version}/${SampleName}"
    done
  fi

}

# MC15ri signal
code="${Belle_tau_DIR}/analysis_code/bin/Analysis_main"
submit_analysis ${code} ${Analysis_Name} "SIGNAL" "MC15ri"

# MC16ri signal
code="${Belle_tau_DIR}/analysis_code/bin/Analysis_main"
submit_analysis ${code} ${Analysis_Name} "SIGNAL" "MC16ri"