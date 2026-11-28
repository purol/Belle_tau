#!/bin/bash

submit_code() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice

  local NToys=${ToyMC_NToys:-10000} # number of toys per injected mu
  local ToysPerJob=${ToyMC_ToysPerJob:-10} # number of toys in one batch job
  local InjectionStep=0.5 # injected mu step; 11 points from zero
  local MuMax
  MuMax=$(awk -v step="$InjectionStep" 'BEGIN { print 10 * step }')

  if (( NToys < 1 || NToys > 10000 || ToysPerJob < 1 )); then
    echo "Invalid ToyMC batch size."
    return 1
  fi

  mkdir -p "./${VerName}/${Analysis_VerName}/ToyMC"

  for mu in $(seq 0 ${InjectionStep} ${MuMax})
  do
    for (( index=0; index<NToys; index+=ToysPerJob )); do
      local LogFile="./${VerName}/${Analysis_VerName}/ToyMC/log/${mu}_${index}.log"
      if [[ ! -f "$LogFile" ]] || ! grep -Fxq "Successfully completed." "$LogFile"; then
        echo "Unsuccessful log found in: $LogFile"
        return 1
      fi
    done
  done

  : > "./${VerName}/${Analysis_VerName}/ToyMC/ToyMC.log" || return 1
  bsub -q s \
  -J READTOY \
  -o "./${VerName}/${Analysis_VerName}/ToyMC/ToyMC.log" \
  ${Code} \
  "./${VerName}/${Analysis_VerName}/ToyMC/out" \
  "./${VerName}/${Analysis_VerName}/ToyMC" \
  ${ToyMC_NToys:-10000} \
  ${InjectionStep}
  if [[ $? -ne 0 ]]; then
    echo "Failed to submit ReadToyMC job."
    return 1
  fi

}


code="${Belle_tau_DIR}/analysis_code/bin/ReadToyMC"
submit_code ${code} ${Analysis_Name}
