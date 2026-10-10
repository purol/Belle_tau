#!/bin/bash

submit_mvasplit() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local SampleName=$3 # ex. MUMUTAUTAU

  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test"

  if compgen -G "./${VerName}/${Analysis_VerName}/${SampleName}/final_output/*.root" > /dev/null; then
    for file in "./${VerName}/${Analysis_VerName}/${SampleName}/final_output"/*.root; do
      filename=$(basename "$file" .root) # without path, without extension
      bsub -q s -J MVASPLIT -o "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train/${filename}_0.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train/${filename}_0.err" ${Code} "./${VerName}/${Analysis_VerName}/${SampleName}/final_output" "${filename}.root" 4 0 "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train/"
      sleep 0.5s
      bsub -q s -J MVASPLIT -o "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train/${filename}_1.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train/${filename}_1.err" ${Code} "./${VerName}/${Analysis_VerName}/${SampleName}/final_output" "${filename}.root" 4 1 "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train/"
      sleep 0.5s
      bsub -q s -J MVASPLIT -o "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train/${filename}_2.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train/${filename}_2.err" ${Code} "./${VerName}/${Analysis_VerName}/${SampleName}/final_output" "${filename}.root" 4 2 "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train/"
      sleep 0.5s
      bsub -q s -J MVASPLIT -o "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test/${filename}_3.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test/${filename}_3.err" ${Code} "./${VerName}/${Analysis_VerName}/${SampleName}/final_output" "${filename}.root" 4 3 "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test/"
      sleep 0.5s
    done
  fi

}


IFS=':' read -r -a Types <<< "$Types_STR_WITH_SIGNAL_ALP"

code="${Belle_tau_DIR}/analysis_code/bin/FBDT_train_test_split"
for Type in "${Types[@]}"; do
    submit_mvasplit ${code} ${Analysis_Name} ${Type}
    sleep 0.5s
done

