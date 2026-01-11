#!/bin/bash

version="Saki"
script="./python/gbasf2_${version}.py"
Skim_path="/home/belle2/junewoo/storage_ghi/tau_SKIM"
Ntuple_path="/home/belle2/junewoo/storage_ghi/tau_Ntuple"

# 4S on-resonance
on_list=("CHARM" "DDBAR" "SSBAR" "UUBAR")

on_flag=("ccbar" "ddbar" "ssbar" "uubar")

for i in "${!on_list[@]}"; do
    Type="${on_list[$i]}"
    Flag="${on_flag[$i]}"

    Type_path="${Ntuple_path}/${version}/${Type}"
    output_path="${Type_path}/MC15ri"
    log_path="${Type_path}/MC15ri/log"

    mkdir -p "${Type_path}"
    mkdir -p "${output_path}"
    mkdir -p "${log_path}"

    for file in $(find "/home/belle2/junewoo/storage_ghi/tau_SKIM/MC15ri_on_additional_qqbar/${Type}/output" -maxdepth 1 -name "*.root")
    do
        echo $file
        basename=$(basename -s .root $file)
        bsub -q s -o "${log_path}/${basename}.log" ${script} --sample "MC15ri" --type ${Flag} --energy "4S" --prompt --vertex --KEKCC --inputfile ${file} --destination "${output_path}"
        sleep 0.3
    done
done 
