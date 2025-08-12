#!/bin/bash

check_log_files(){
  local VerName=$1 # ex. Alice
  local SampleName=$2 # ex. MUMUTAUTAU

  LogDirName="./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test_after_application/" 
  if compgen -G "${LogDirName}/*.log" > /dev/null; then
    for log_file in "${LogDirName}"/*.log; do
      if ! grep -q "Successfully completed" "$log_file"; then
        echo "Unsuccessful log found in: $log_file"
        exit 1  # Return non-zero status to indicate failure
      fi
    done
  fi

}

Types=("CHG" "MIX" "UUBAR" "DDBAR" "SSBAR" "CHARM" "MUMU" "EE" "EEEE" "EEMUMU" "EEPIPI" "EEKK" "EEPP" "PIPIISR" "KKISR" "GG" "EETAUTAU" "K0K0BARISR" "MUMUMUMU" "MUMUTAUTAU" "TAUTAUTAUTAU" "TAUPAIR" "SIGNAL")
for Type in "${Types[@]}"; do
    check_log_files ${Analysis_Name} ${Type}
    sleep 0.5s
done

echo "All logs successfully completed."
exit 0  # Return zero status to indicate success

