#!/bin/sh

if [ "$#" -ne 2 ]; then
    echo "usage: gen_cut_KEK_study.sh {input dirname} {output path}"
    exit 1
fi

input_dir=${1}
output_dir=${2}

mkdir -p "${output_dir}/output"
mkdir -p "${output_dir}/log"
mkdir -p "${output_dir}/err"

if compgen -G "${input_dir}/*.root" > /dev/null; then
  for file in "${input_dir}"/*.root; do
    filename=$(basename "$file" .root) # without path, without extension
    bsub -q s -J GenCutStudy -o "${output_dir}/log/${filename}.log" -e "${output_dir}/err/${filename}.err" "./python/gbasf2_Youmu.py" --sample "MC15ri" --type "uubar" --energy "4S" --prompt --vertex --gencut --KEKCC --inputfile "${file}"  --destination "${output_dir}/output" 
  done
fi
