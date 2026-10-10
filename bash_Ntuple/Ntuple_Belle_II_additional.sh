#!/bin/bash

version="Ibaraki"
script="./python/gbasf2_${version}.py"
Skim_path="/home/belle2/junewoo/storage_ghi/tau_SKIM_2"
Ntuple_path="/home/belle2/junewoo/storage_ghi/tau_Ntuple"

# 4S on-resonance
on_list=("CCBAR" "DDBAR" "SSBAR" "TAUPAIR" "UUBAR")

on_flag=("ccbar" "ddbar" "ssbar" "taupair" "uubar")

for i in "${!on_list[@]}"; do
    Type="${on_list[$i]}"
    Flag="${on_flag[$i]}"

    Type_path="${Ntuple_path}/${version}/${Type}"
    output_path="${Type_path}/MC16ri"
    log_path="${Type_path}/MC16ri/log"

    mkdir -p "${Type_path}"
    mkdir -p "${output_path}"
    mkdir -p "${log_path}"

    for file in $(find "${Skim_path}/MC16ri_on_additional_qqbar/${Type}/output" -maxdepth 1 -name "*.root")
    do
        echo $file
        basename=$(basename -s .root $file)
        bsub -q s \
        -o "${log_path}/${basename}.log" \
        ${script} \
        --sample "MC16ri" \
        --type ${Flag} \
        --energy "4S" \
        --prompt \
        --vertex \
        --KEKCC \
        --inputfile ${file} \
        --destination "${output_path}"
        sleep 0.3
    done
done 
