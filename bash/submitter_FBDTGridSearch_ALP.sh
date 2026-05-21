#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.+-eE]\+\)_life\([0-9.+-eE]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\).*/\1 \2 \3 \4/p' | \
  sort -u
}

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

  get_params "./${VerName}/${Analysis_VerName}/ALP/final_output" | while read mass life A B; do

    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}"
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/out_${mass}_${life}_${A}_${B}"
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/log_${mass}_${life}_${A}_${B}"
    mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}/err_${mass}_${life}_${A}_${B}"

    bsub -q s -J FBDTTRN -o "./${VerName}/${Analysis_VerName}/${OutputPath}/log_${mass}_${life}_${A}_${B}/${nTree}_${depth}_${shrinkage}_${subsample}_${binning}.log" -e "./${VerName}/${Analysis_VerName}/${OutputPath}/err_${mass}_${life}_${A}_${B}/${nTree}_${depth}_${shrinkage}_${subsample}_${binning}.err" ${Code} "${#input_variables_ref[@]}" "${input_variables_ref[@]}" "./${VerName}/${Analysis_VerName}" "./${VerName}/${Analysis_VerName}/${OutputPath}/out_${mass}_${life}_${A}_${B}" "${nTree}" "${depth}" "${shrinkage}" "${subsample}" "${binning}" "${mass}" "${life}" "${A}" "${B}"
  done

}


code="${Belle_tau_DIR}/analysis_code/bin/FBDT_GridSearch_one_ALP"
output="GridSearch_one"
for nTree in 250 500 750
do
  for depth in 1 2
  do
    for shrinkage in 0.01 0.1 0.2
    do
      for subsample in 0.01 0.2 0.5
      do
        for binning in 6 8
        do
          submit_GridSearch ${code} ${Analysis_Name} ${nTree} ${depth} ${shrinkage} ${subsample} ${binning} ${output} "input_variables_one"
        done
      done
    done
  done
done

code="${Belle_tau_DIR}/analysis_code/bin/FBDT_GridSearch_two_ALP"
output="GridSearch_two"
for nTree in 100 250 500 750
do
  for depth in 1 2
  do
    for shrinkage in 0.01 0.1 0.2
    do
      for subsample in 0.01 0.2 0.5
      do
        for binning in 6 8
        do
          submit_GridSearch ${code} ${Analysis_Name} ${nTree} ${depth} ${shrinkage} ${subsample} ${binning} ${output} "input_variables_two"
        done
      done
    done
  done
done




