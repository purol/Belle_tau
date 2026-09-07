#!/bin/bash

get_params() {
  local dir="$1"

  ls "$dir" | \
  sed -n 's/.*alpha_mass\([0-9.+-eE]\+\)_life\([0-9.+-eE]\+\)_A\([0-9+-]\+\)_B\([0-9+-]\+\).*/\1 \2 \3 \4/p' | \
  sort -u
}

code="${Belle_tau_DIR}/analysis_code/bin/ReadGridSearchFile_ALP"

get_params "./${Analysis_Name}/${Analysis_VerName}/ALP/final_output" | while read mass life A B; do
  bsub -q s -J GRIDFILE -o "/dev/null" ${code} "./${Analysis_Name}/${Analysis_VerName}/GridSearch_one/out_${mass}_${life}_${A}_${B}/" "./${Analysis_Name}/${Analysis_VerName}/GridSearch_one/" "${mass}" "${life}" "${A}" "${B}"
  bsub -q s -J GRIDFILE -o "/dev/null" ${code} "./${Analysis_Name}/${Analysis_VerName}/GridSearch_two/out_${mass}_${life}_${A}_${B}/" "./${Analysis_Name}/${Analysis_VerName}/GridSearch_two/" "${mass}" "${life}" "${A}" "${B}"
done