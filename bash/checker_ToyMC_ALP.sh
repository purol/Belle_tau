#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.+-eE]\+\)_life\([0-9.+-eE]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\).*/\1 \2 \3 \4/p' | \
  sort -u
}

check_log_files(){
  local VerName=$1 # ex. Alice

  local Params
  Params=$(get_params "./${VerName}/${Analysis_VerName}/ALP/final_output")
  if [[ -z "$Params" ]]; then
    echo "No ALP samples found."
    return 1
  fi

  while read mass life A B; do
    local LogFile="./${VerName}/${Analysis_VerName}/ToyMC_${mass}_${life}_${A}_${B}/ToyMC.log"
    if [[ ! -f "$LogFile" ]] || ! grep -Fxq "Successfully completed." "$LogFile"; then
      echo "Unsuccessful log found in: $LogFile"
      return 1
    fi

  done <<< "$Params"
}

check_log_files ${Analysis_Name}
if [[ $? -ne 0 ]]; then
  exit 1
fi

echo "All logs successfully completed."
exit 0  # Return zero status to indicate success
