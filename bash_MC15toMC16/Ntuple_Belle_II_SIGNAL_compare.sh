#!/bin/bash

# MC15ri signal
version="Larva"
script="./python/gbasf2_${version}.py"
MCVersion="MC15ri"
Skim_path="/home/belle2/junewoo/storage_ghi/tau_SKIM_2"
Ntuple_path="/home/belle2/junewoo/storage_ghi/tau_Ntuple_compare"
Type="SIGNAL"

Type_path="${Ntuple_path}/${version}/${Type}"
output_path="${Type_path}/${MCVersion}"
log_path="${Type_path}/${MCVersion}/log"

for file in $(find "${Skim_path}/${MCVersion}_on/${Type}/output" -maxdepth 1 -name "*.root")
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

# MC16ri signal
version="Larva"
script="./python/gbasf2_${version}.py"
MCVersion="MC16ri"
Skim_path="/home/belle2/junewoo/storage_ghi/tau_SKIM_2"
Ntuple_path="/home/belle2/junewoo/storage_ghi/tau_Ntuple_compare"
Type="SIGNAL"

Type_path="${Ntuple_path}/${version}/${Type}"
output_path="${Type_path}/${MCVersion}"
log_path="${Type_path}/${MCVersion}/log"

for file in $(find "${Skim_path}/${MCVersion}_on/${Type}/output" -maxdepth 1 -name "*.root")
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