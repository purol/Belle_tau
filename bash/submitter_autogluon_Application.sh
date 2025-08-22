#!/bin/bash

submit_Application() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local SampleName=$3 # ex. MUMUTAUTAU

  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test_after_application"
  mkdir -p "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train_after_application"

  if compgen -G "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test/*.root" > /dev/null; then
    for file in "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test"/*.root; do
      filename=$(basename "$file" .root) # without path, without extension
      bsub -q l -J MVAAPP -o "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test_after_application/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test_after_application/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.err" ~/miniconda3/envs/autogluon/bin/python ${Code} --input_path "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test" --input_file_name "${filename}.root" --output_path "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_test_after_application" --model_one "./${VerName}/${Analysis_VerName}/AutogluonModels/model_one" --model_two "./${VerName}/${Analysis_VerName}/AutogluonModels/model_two"
    done
  fi

  if compgen -G "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train/*.root" > /dev/null; then
    for file in "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train"/*.root; do
      filename=$(basename "$file" .root) # without path, without extension
      bsub -q l -J MVAAPP -o "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train_after_application/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.log" -e "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train_after_application/${filename}_${SampleName}_${VerName}_${Analysis_VerName}.err" ~/miniconda3/envs/autogluon/bin/python ${Code} --input_path "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train" --input_file_name "${filename}.root" --output_path "./${VerName}/${Analysis_VerName}/${SampleName}/final_output_train_after_application" --model_one "./${VerName}/${Analysis_VerName}/AutogluonModels/model_one" --model_two "./${VerName}/${Analysis_VerName}/AutogluonModels/model_two"
    done
  fi

}

# activate autogluon env
source ~/miniconda3/etc/profile.d/conda.sh
conda activate autogluon

Types=("CHG" "MIX" "UUBAR" "DDBAR" "SSBAR" "CHARM" "MUMU" "EE" "EEEE" "EEMUMU" "EEPIPI" "EEKK" "EEPP" "PIPIISR" "KKISR" "GG" "EETAUTAU" "K0K0BARISR" "MUMUMUMU" "MUMUTAUTAU" "TAUTAUTAUTAU" "TAUPAIR" "SIGNAL")

code="${Belle_tau_DIR}/analysis_code/src/apply_autogluon.py"
for Type in "${Types[@]}"; do
    submit_Application ${code} ${Analysis_Name} ${Type}
    sleep 0.5s
done

conda deactivate
