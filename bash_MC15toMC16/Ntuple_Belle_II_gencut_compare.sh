#!/bin/bash

# MC16ri signal
version="Larva"
script="./python/gbasf2_${version}.py"
MCVersion="MC16ri"
mdst_path="/home/belle2/junewoo/storage_ghi/tau_gen_cut/MC16ri/MC16ri_private_gencut/DDBAR"
Ntuple_path="/home/belle2/junewoo/storage_ghi/tau_Ntuple_compare"
Type="DDBAR"

Type_path="${Ntuple_path}/${version}/${Type}"
output_path="${Type_path}/${MCVersion}"
log_path="${Type_path}/${MCVersion}/log"

mkdir -p "${output_path}"
mkdir -p "${log_path}"

for file in $(find "${mdst_path}" -maxdepth 1 -name "*.root")
do
    echo $file
    basename=$(basename -s .root $file)
    bsub -q s \
    -o "${log_path}/${basename}.log" \
    ${script} \
    --sample "${MCVersion}" \
    --type "signal" \
    --energy "4S" \
    --prompt --vertex --KEKCC \
    --inputfile ${file} \
    --destination "${output_path}"
    sleep 0.3
done