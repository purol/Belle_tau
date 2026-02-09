#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.+-eE]\+\)_life\([0-9.+-eE]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\).*/\1 \2 \3 \4/p' | \
  sort -u
}


check_log_files(){
  local VerName=$1 # ex. Alice

  get_params "./${VerName}/${Analysis_VerName}/${SampleName}/final_output" | while read mass life A B; do

    LogDirName="./${VerName}/${Analysis_VerName}/GridSearch_one/log_${mass}_${life}_${A}_${B}" 
    if compgen -G "${LogDirName}/*.log" > /dev/null; then
      for log_file in "${LogDirName}"/*.log; do
        if ! grep -q "Successfully completed" "$log_file"; then
          echo "Unsuccessful log found in: $log_file"
          exit 1  # Return non-zero status to indicate failure
        fi
      done
    fi

    LogDirName="./${VerName}/${Analysis_VerName}/GridSearch_two/log_${mass}_${life}_${A}_${B}"
    if compgen -G "${LogDirName}/*.log" > /dev/null; then
      for log_file in "${LogDirName}"/*.log; do
        if ! grep -q "Successfully completed" "$log_file"; then
          echo "Unsuccessful log found in: $log_file"
          exit 1  # Return non-zero status to indicate failure
        fi
      done
    fi

  done

}

check_log_files ${Analysis_Name}

echo "All logs successfully completed."
exit 0  # Return zero status to indicate success

