#!/bin/bash

check_log_files(){
  local VerName=$1 # ex. Alice

  LogDirName="./${VerName}/${Analysis_VerName}/AutogluonModels/" 
  if compgen -G "${LogDirName}/*.log" > /dev/null; then
    for log_file in "${LogDirName}"/*.log; do
      if ! grep -q "Successfully completed" "$log_file"; then
        echo "Unsuccessful log found in: $log_file"
        exit 1  # Return non-zero status to indicate failure
      fi
    done
  fi

}

check_log_files ${Analysis_Name}

echo "All logs successfully completed."
exit 0  # Return zero status to indicate success

