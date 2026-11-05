#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.eE+-]\+\)_life\([0-9.eE+-]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\)_.*\.root$/\1 \2 \3 \4/p' | \
  sort -u
}

submit_code() {
  local Code=$1 # ex. ./bin/Calculate_FBDT_syst_CTRL_ALP
  local VerName=$2 # ex. Alice
  local InputDir1=$3 # ex. final_output_after_application
  local InputDir2=$4 # ex. final_output_after_application
  local OutputPath=$5 # ex. FBDTSyst
  local SignalType=$6 # ex. ALP
  local MCList=$7
  local dataList=$8
  local WeightPath=$9 # ex. Weight

  local Type
  local -a Types dataTypes
  IFS=':' read -r -a Types <<< "${dataList}"
  for Type in "${Types[@]}"; do
    if compgen -G "./${VerName}/${Analysis_VerName}/${Type}/${InputDir2}/*.root" > /dev/null; then
      dataTypes+=("${Type}")
    fi
  done
  if [ "${#dataTypes[@]}" -eq 0 ]; then
    echo "No control data for FBDT efficiency."
    return
  fi
  dataList=$(IFS=:; echo "${dataTypes[*]}")

  mkdir -p "./${VerName}/${Analysis_VerName}/${OutputPath}"

  get_params "${nominal_analysis_DIR}/${SignalType}/${InputDir1}/" | while read mass life A B; do
    bsub -q l \
    -J FBDTSyst \
    -o "./${VerName}/${Analysis_VerName}/${OutputPath}/FBDTSyst_${mass}_${life}_${A}_${B}.log" \
    -e "./${VerName}/${Analysis_VerName}/${OutputPath}/FBDTSyst_${mass}_${life}_${A}_${B}.err" \
    "${Code}" \
    "${nominal_analysis_DIR}" \
    "${InputDir1}" \
    "./${VerName}/${Analysis_VerName}" \
    "${InputDir2}" \
    "./${VerName}/${Analysis_VerName}/${OutputPath}" \
    "${SignalType}" \
    "${MCList}" \
    "${dataList}" \
    "./${VerName}/${Analysis_VerName}/${WeightPath}" \
    "${mass}" \
    "${life}" \
    "${A}" \
    "${B}"
  done
}


code="${Belle_tau_DIR}/analysis_code/bin/Calculate_FBDT_syst_CTRL_ALP"
submit_code "${code}" "${Analysis_Name}" "final_output_after_application" "final_output_after_application" "FBDTSyst" "${ALP_Type}" "${Types_STR_WITH_SIGNAL}" "${data_Type}" "Weight"
