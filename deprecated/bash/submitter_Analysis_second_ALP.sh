#!/bin/bash

submit_analysis() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local SampleName=$3 # ex. MUMUTAUTAU

  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/final_output"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/log_second"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/err_second"

  if compgen -G "./${VerName}/${Analysis_VerName}/${SampleName}/before_strict_M_deltaE_selection/*.root" > /dev/null; then
    for file in "./${VerName}/${Analysis_VerName}/${SampleName}/before_strict_M_deltaE_selection"/*.root; do
      filename=$(basename "$file" .root) # without path, without extension
      bsub -q s -J Analyze -o "./${VerName}/${Analysis_VerName}/${SampleName}/log_second/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/err_second/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.err" ${Code} "./${VerName}/${Analysis_VerName}/${SampleName}/before_strict_M_deltaE_selection" "${filename}.root" "./${VerName}/${Analysis_VerName}/${SampleName}" "./${VerName}/${Analysis_VerName}/"
    done
  fi

}


IFS=':' read -r -a Types <<< "$Types_STR_WITH_SIGNAL_ALP"

code="${Belle_tau_DIR}/analysis_code/bin/Analysis_main_second_ALP"
for Type in "${Types[@]}"; do
    submit_analysis ${code} ${Analysis_Name} ${Type}
    sleep 0.5s
done

