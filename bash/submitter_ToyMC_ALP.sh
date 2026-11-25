#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.+-eE]\+\)_life\([0-9.+-eE]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\).*/\1 \2 \3 \4/p' | \
  sort -u
}

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

  local Params
  Params=$(get_params "./${VerName}/${Analysis_VerName}/ALP/final_output")
  if [[ -z "$Params" ]]; then
    echo "No ALP samples found."
    return 1
  fi

  while read mass life A B; do
    local ScanStep=0.1 # initial upper limit scan step
    if awk -v life="$life" 'BEGIN { exit !(life >= 700) }'; then
      ScanStep=20
    elif awk -v life="$life" 'BEGIN { exit !(life >= 200 && life < 700) }'; then
      ScanStep=5
    elif awk -v life="$life" 'BEGIN { exit !(life >= 50 && life < 200) }'; then
      ScanStep=0.2
    fi

    if [[ ! -f "./${VerName}/${Analysis_VerName}/workspace_${mass}_${life}_${A}_${B}.root" ]]; then
      echo "Missing ToyMC workspace."
      return 1
    fi
    mkdir -p "./${VerName}/${Analysis_VerName}/ToyMC_${mass}_${life}_${A}_${B}/out"
    mkdir -p "./${VerName}/${Analysis_VerName}/ToyMC_${mass}_${life}_${A}_${B}/log"

    for mu in $(seq 0 0.25 5.0)
    do
      for (( index=0; index<NToys; index+=ToysPerJob )); do
        local Count=$ToysPerJob
        if (( index + Count > NToys )); then
          Count=$((NToys - index))
        fi
        : > "./${VerName}/${Analysis_VerName}/ToyMC_${mass}_${life}_${A}_${B}/log/${mu}_${index}.log" || return 1
        bsub -q s \
        -J TAUTOY \
        -o "./${VerName}/${Analysis_VerName}/ToyMC_${mass}_${life}_${A}_${B}/log/${mu}_${index}.log" \
        ${Code} \
        "./${VerName}/${Analysis_VerName}" \
        "workspace_${mass}_${life}_${A}_${B}.root" \
        "./${VerName}/${Analysis_VerName}/ToyMC_${mass}_${life}_${A}_${B}/out" \
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

  done <<< "$Params"
}


code="${Belle_tau_DIR}/analysis_code/bin/Run_ToyMC"
submit_ToyMC ${code} ${Analysis_Name}
