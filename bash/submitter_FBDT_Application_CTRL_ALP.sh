#!/bin/bash

# predefined input variables
IFS=':' read -r -a input_variables_one <<< "$input_variables_one_STR"
IFS=':' read -r -a input_variables_two <<< "$input_variables_two_STR"

submit_Application() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local SampleName=$3 # ex. MUMUTAUTAU

  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_after_application"

  if compgen -G "./${VerName}/${Analysis_VerName}/${SampleName}/before_strict_M_deltaE_selection/*.root" > /dev/null; then
    for file in "./${VerName}/${Analysis_VerName}/${SampleName}/before_strict_M_deltaE_selection"/*.root; do
      filename=$(basename "$file" .root) # without path, without extension
      bsub -q l -J FBDTAPP -o "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_after_application/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_after_application/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.err" ${Code} "${#input_variables_one[@]}" "${input_variables_one[@]}" "${#input_variables_two[@]}" "${input_variables_two[@]}" "./${VerName}/${Analysis_VerName}/${SampleName}/before_strict_M_deltaE_selection" ${filename} "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_after_application" "${FBDT_weight_DIR}/GridSearch_one" "${FBDT_weight_DIR}/GridSearch_two"
    done
  fi

}

IFS=':' read -r -a Types <<< "$Types_STR_WITH_SIGNAL_ALP"

code="${Belle_tau_DIR}/analysis_code/bin/FBDT_Application_ALP"
for Type in "${Types[@]}"; do
    submit_Application ${code} ${Analysis_Name} ${Type}
    sleep 0.5s
done

