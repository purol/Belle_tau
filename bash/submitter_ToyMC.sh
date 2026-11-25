#!/bin/bash

submit_ToyMC() {
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

  local ScanStep=0.1 # initial upper limit scan step

  if [[ ! -f "./${VerName}/${Analysis_VerName}/workspace.root" ]]; then
    echo "Missing ToyMC workspace."
    return 1
  fi
  mkdir -p "./${VerName}/${Analysis_VerName}/ToyMC/out"
  mkdir -p "./${VerName}/${Analysis_VerName}/ToyMC/log"

  for mu in $(seq 0 0.25 5.0)
  do
    for (( index=0; index<NToys; index+=ToysPerJob )); do
      local Count=$ToysPerJob
      if (( index + Count > NToys )); then
        Count=$((NToys - index))
      fi
      : > "./${VerName}/${Analysis_VerName}/ToyMC/log/${mu}_${index}.log" || return 1
      bsub -q s \
      -J TAUTOY \
      -o "./${VerName}/${Analysis_VerName}/ToyMC/log/${mu}_${index}.log" \
      ${Code} \
      "./${VerName}/${Analysis_VerName}" \
      "workspace.root" \
      "./${VerName}/${Analysis_VerName}/ToyMC/out" \
      ${mu} \
      ${index} \
      ${Count} \
      ${CLsToys} \
      ${ScanStep} \
      ${Seed}
      if [[ $? -ne 0 ]]; then
        echo "Failed to submit ToyMC job."
        return 1
      fi
    done
  done

}


code="${Belle_tau_DIR}/analysis_code/bin/Run_ToyMC"
submit_ToyMC ${code} ${Analysis_Name}
