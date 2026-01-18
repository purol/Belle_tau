#!/bin/bash

submit_fitter() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local SampleName=$3 # ex. MUMUTAUTAU

  bsub -q s -J ALPFIT -o "/dev/null" ${Code} "./${VerName}/${Analysis_VerName}/${SampleName}/before_strict_M_deltaE_selection" "./${VerName}/${Analysis_VerName}/" 
}


code="${Belle_tau_DIR}/analysis_code/bin/fit_mass_ALP"
submit_fitter ${code} ${Analysis_Name} "ALP"

