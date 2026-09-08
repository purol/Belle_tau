#!/bin/bash

# MC16ri ddbar nominal
version="Larva"
script="./python/gbasf2_${version}.py"
MCVersion="MC16ri"
Skim_path="/home/belle2/junewoo/storage_ghi/tau_SKIM_2/"
Ntuple_path="/home/belle2/junewoo/storage_ghi/tau_Ntuple_compare"
Type="DDBAR"

Type_path="${Ntuple_path}/${version}/${Type}"
output_path="${Type_path}/${MCVersion}"
log_path="${Type_path}/${MCVersion}/log"

mkdir -p "${output_path}"
mkdir -p "${log_path}"

for file in $(find "${Skim_path}/${MCVersion}_on/${Type}/output" -maxdepth 1 -name "*.root")
do
    echo $file
    basename=$(basename -s .root $file)
    bsub -q s \
    -o "${log_path}/${basename}.log" \
    ${script} \
    --sample "${MCVersion}" \
    --type "ddbar" \
    --energy "4S" \
    --prompt --vertex --KEKCC \
    --inputfile ${file} \
    --destination "${output_path}"
    sleep 0.3
done

# MC16ri ddbar gencut
version="Larva"
script="./python/gbasf2_${version}.py"
MCVersion="MC16ri"
Skim_path="/home/belle2/junewoo/storage_ghi/tau_SKIM_compare/"
Ntuple_path="/home/belle2/junewoo/storage_ghi/tau_Ntuple_compare"
Type="DDBAR"

Type_path="${Ntuple_path}/${version}/${Type}"
output_path="${Type_path}/${MCVersion}"
log_path="${Type_path}/${MCVersion}/log"

mkdir -p "${output_path}"
mkdir -p "${log_path}"

for file in $(find "${Skim_path}/${MCVersion}_on/${Type}/output" -maxdepth 1 -name "*.root")
do
    echo $file
    basename=$(basename -s .root $file)
    bsub -q s \
    -o "${log_path}/${basename}.log" \
    ${script} \
    --sample "${MCVersion}" \
    --type "ddbar" \
    --energy "4S" \
    --prompt --vertex --KEKCC \
    --inputfile ${file} \
    --destination "${output_path}"
    sleep 0.3
done