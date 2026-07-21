#!/bin/sh

if [ "$#" -ne 2 ]; then
    echo "usage: submitter_split_gencut.sh {input dirname} {output path}"
    exit 1
fi

input_dir=${1}
output_dir=${2}

mkdir -p "${output_dir}/signal"
mkdir -p "${output_dir}/background"

mkdir -p "${output_dir}/log"
mkdir -p "${output_dir}/err"

if compgen -G "${input_dir}/*.root" > /dev/null; then
  for file in "${input_dir}"/*.root; do
    filename=$(basename "$file" .root) # without path, without extension
    bsub -q s -J SplitGen -o "${output_dir}/log/${filename}.log" -e "${output_dir}/err/${filename}.err" "Split_gencut" "${input_dir}" "${filename}" "${output_dir}"
  done
fi