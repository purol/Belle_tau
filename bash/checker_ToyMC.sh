#!/bin/bash

check_log_files(){
  local VerName=$1 # ex. Alice

  local LogFile="./${VerName}/${Analysis_VerName}/ToyMC/ToyMC.log"
  if [[ ! -f "$LogFile" ]] || ! grep -Fxq "Successfully completed ReadToyMC." "$LogFile" || ! grep -Fxq "Successfully completed." "$LogFile"; then
    echo "Unsuccessful log found in: $LogFile"
    return 1
  fi

}

check_log_files ${Analysis_Name}
if [[ $? -ne 0 ]]; then
  exit 1
fi

echo "All logs successfully completed."
exit 0  # Return zero status to indicate success
