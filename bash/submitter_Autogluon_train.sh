#!/bin/bash

submit_GridSearch() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice

  mkdir -p "./${VerName}/${Analysis_VerName}/AutogluonModels/"

  bsub -q l -J MVATRN -o "./${VerName}/${Analysis_VerName}/AutogluonModels/Autogluon_train.log" -e "./${VerName}/${Analysis_VerName}/AutogluonModels/Autogluon_train.err" ${Code} --input_path "./${VerName}/${Analysis_VerName}"

}

code="${Belle_tau_DIR}/analysis_code/src/train_autogluon.py"
submit_GridSearch ${code} ${Analysis_Name}




