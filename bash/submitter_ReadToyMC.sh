#!/bin/bash

submit_code() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice

  local NToys=${ToyMC_NToys:-10000} # number of toys per injected mu
  local ToysPerJob=${ToyMC_ToysPerJob:-10} # number of toys in one batch job
  local CLsToys=${ToyMC_CLsToys:-10000} # number of CLs toys per hypothesis
  local Seed=${ToyMC_Seed:-12345} # base random seed

  if ! [[ "$NToys" =~ ^[0-9]+$ && "$ToysPerJob" =~ ^[0-9]+$ && "$CLsToys" =~ ^[0-9]+$ && "$Seed" =~ ^[0-9]+$ ]] || (( NToys < 1 || NToys > 10000 || ToysPerJob < 1 || CLsToys < 1 || Seed > 4294967295 )); then
    echo "Invalid ToyMC configuration."
    return 1
  fi

  mkdir -p "./${VerName}/${Analysis_VerName}/ToyMC"

  for mu in $(seq 0 0.25 5.0)
  do
    for (( index=0; index<NToys; index+=ToysPerJob )); do
      local LogFile="./${VerName}/${Analysis_VerName}/ToyMC/log/${mu}_${index}.log"
      if [[ ! -f "$LogFile" ]] || ! grep -Fxq "Successfully completed ToyMC." "$LogFile" || ! grep -Fxq "Successfully completed." "$LogFile"; then
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
  ${ToyMC_NToys:-10000}
  if [[ $? -ne 0 ]]; then
    echo "Failed to submit ReadToyMC job."
    return 1
  fi

}


code="${Belle_tau_DIR}/analysis_code/bin/ReadToyMC"
submit_code ${code} ${Analysis_Name}
