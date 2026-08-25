#!/bin/bash

check_log_files(){
  local VerName=$1 # ex. Alice
  local SampleName=$2 # ex. MUMUTAUTAU

  LogDirName="./${VerName}/${Analysis_VerName}/${SampleName}/log_second" 
  if compgen -G "${LogDirName}/*.log" > /dev/null; then
    for log_file in "${LogDirName}"/*.log; do
      if ! grep -q "Successfully completed" "$log_file"; then
        echo "Unsuccessful log found in: $log_file"
        exit 1  # Return non-zero status to indicate failure
      fi
    done
  fi

}

IFS=':' read -r -a Types <<< "$Types_STR_WITH_SIGNAL_ALP"

for Type in "${Types[@]}"; do
    check_log_files ${Analysis_Name} ${Type}
    sleep 0.5s
done

echo "All logs successfully completed."
exit 0  # Return zero status to indicate success

