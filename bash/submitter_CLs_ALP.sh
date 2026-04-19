#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.+-eE]\+\)_life\([0-9.+-eE]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\).*/\1 \2 \3 \4/p' | \
  sort -u
}

submit_CLs() {
  local Code=$1 # ex. ./bin/Analysis_main
  local VerName=$2 # ex. Alice

  get_params "./${VerName}/${Analysis_VerName}/ALP/final_output" | while read mass life A B; do

    mkdir -p "./${VerName}/${Analysis_VerName}/CLs_${mass}_${life}_${A}_${B}/out"
    mkdir -p "./${VerName}/${Analysis_VerName}/CLs_${mass}_${life}_${A}_${B}/log"

    # life 값이 100 이상인지 awk를 이용해 확인 (소수점, 지수 표기법 처리 가능)
    is_large_life=$(awk -v life="$life" 'BEGIN { if (life >= 100) print 1; else print 0 }')

    # 조건에 따라 mu 반복 범위(seq) 설정
    if [ "$is_large_life" -eq 1 ]; then
      mu_list=$(seq 0 1 50)     # life가 100 이상일 때 (0부터 50까지 1단위)
    else
      mu_list=$(seq 0 0.1 5.0)  # life가 100 미만일 때 (0부터 5.0까지 0.1단위)
    fi

    for mu in $mu_list
    do
      for index in {0..10}; do
        bsub -q s -J TAUCLS -o "./${VerName}/${Analysis_VerName}/CLs_${mass}_${life}_${A}_${B}/log/${mu}_${index}.log" ${Code} "./${VerName}/${Analysis_VerName}" "workspace_${mass}_${life}_${A}_${B}.root" "./${VerName}/${Analysis_VerName}/CLs_${mass}_${life}_${A}_${B}/out" ${mu} ${index}
      done
    done

  done

}


code="${Belle_tau_DIR}/analysis_code/bin/Run_CLs"
submit_CLs ${code} ${Analysis_Name}