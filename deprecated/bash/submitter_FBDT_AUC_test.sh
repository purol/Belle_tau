#!/bin/bash

# predefined input variables
IFS=':' read -r -a input_variables_one <<< "$input_variables_one_STR"
IFS=':' read -r -a input_variables_two <<< "$input_variables_two_STR"

submit_GridSearch() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice
  local nTree=$3
  local depth=$4
  local shrinkage=$5
  local subsample=$6
  local binning=$7
  local OutputPath=$8
  local array_name=$9

  # nameref to the array
  local -n input_variables_ref=${array_name}

  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}"
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/out"
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/log_AUC_test"
  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/err_AUC_test"

  bsub -q l -J AUCTST -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log_AUC_test/${nTree}_${depth}_${shrinkage}_${subsample}_${binning}.log" -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err_AUC_test/${nTree}_${depth}_${shrinkage}_${subsample}_${binning}.err" ${Code} "${#input_variables_ref[@]}" "${input_variables_ref[@]}" "./${VerName}/${Analysis_VerName}" "./${VerName}/${Analysis_VerName}/${OutputPath}/out" "${nTree}" "${depth}" "${shrinkage}" "${subsample}" "${binning}" "${Signal_Type}" "${Background_Types_STR}"

}


code="${Belle_tau_DIR}/analysis_code/bin/FBDT_AUC_test_one"
output="GridSearch_one"
for nTree in 100 250 500 750 1000
do
  for depth in 1 2 3 4
  do
    for shrinkage in 0.01 0.05 0.1
    do
      for subsample in 0.01 0.3 0.4 0.5 0.6 0.7
      do
        for binning in 5 6 7 8 9
        do
          submit_GridSearch ${code} ${Analysis_Name} ${nTree} ${depth} ${shrinkage} ${subsample} ${binning} ${output} "input_variables_one"
        done
      done
    done
  done
done

code="${Belle_tau_DIR}/analysis_code/bin/FBDT_AUC_test_two"
output="GridSearch_two"
for nTree in 100 250 500 750 1000
do
  for depth in 1 2 3 4
  do
    for shrinkage in 0.01 0.05 0.1
    do
      for subsample in 0.01 0.3 0.4 0.5 0.6 0.7
      do
        for binning in 5 6 7 8 9
        do
          submit_GridSearch ${code} ${Analysis_Name} ${nTree} ${depth} ${shrinkage} ${subsample} ${binning} ${output} "input_variables_two"
        done
      done
    done
  done
done



