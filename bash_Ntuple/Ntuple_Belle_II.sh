#!/bin/bash

version="Ibaraki"
script="./python/gbasf2_${version}.py"
Skim_path="/home/belle2/junewoo/storage_ghi/tau_SKIM_2"
Ntuple_path="/home/belle2/junewoo/storage_ghi/tau_Ntuple"

# MC16ri 4S on-resonance
on_list=("ALP" "CCBAR" "CHG" "DDBAR" "EE" "EEEE" 
         "EEMUMU" "GG" "HHISR" "LLXX" "MIX" 
         "MUMU" "SIGNAL" "SSBAR" "TAUPAIR" "UUBAR")

on_flag=("ALP" "ccbar" "charged" "ddbar" "ee" "eee"
         "eemumu" "gg" "hhISR" "llXX" "mixed"
         "mumu" "signal" "ssbar" "taupair" "uubar")

for i in "${!on_list[@]}"; do
    Type="${on_list[$i]}"
    Flag="${on_flag[$i]}"

    Type_path="${Ntuple_path}/${version}/${Type}"
    output_path="${Type_path}/MC16ri"
    log_path="${Type_path}/MC16ri/log"

    mkdir -p "${Type_path}"
    mkdir -p "${output_path}"
    mkdir -p "${log_path}"

    for file in $(find "${Skim_path}/MC16ri_on/${Type}/output" -maxdepth 1 -name "*.root")
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
        sleep 0.5
    done
done 

# MC16rd 4S on-resonance
on_list=("ALP" "BB" "EE" "EEEE" 
         "EEMUMU" "GG" "HHISR" "LLXX"
         "MUMU" "SIGNAL" "TAUPAIR" "UDSC")

on_flag=("ALP" "BB" "ee" "eeee"
         "eemumu" "gg" "hhISR" "llXX"
         "mumu" "signal" "taupair" "udsc")

for i in "${!on_list[@]}"; do
    Type="${on_list[$i]}"
    Flag="${on_flag[$i]}"

    Type_path="${Ntuple_path}/${version}/${Type}"
    output_path="${Type_path}/MC16rd"
    log_path="${Type_path}/MC16rd/log"

    mkdir -p "${Type_path}"
    mkdir -p "${output_path}"
    mkdir -p "${log_path}"

    for file in $(find "${Skim_path}/MC16rd_on/${Type}/output" -maxdepth 1 -name "*.root")
    do
        echo $file
        basename=$(basename -s .root $file)
        bsub -q s \
        -o "${log_path}/${basename}.log" \
        ${script} \
        --sample "MC16rd" \
        --type ${Flag} \
        --energy "4S" \
        --prompt \
        --vertex \
        --KEKCC \
        --inputfile ${file} \
        --destination "${output_path}"
        sleep 0.5
    done
done 

# MC16rd 4S off-resonance
on_list=("EE" "EEEE" "EEMUMU" "GG" 
         "HHISR" "LLXX" "MUMU" 
         "SIGNAL" "TAUPAIR" "UDSC")

on_flag=("ee" "eeee" "eemumu" "gg"
         "hhISR" "llXX" "mumu"
         "signal" "taupair" "udsc")

for i in "${!on_list[@]}"; do
    Type="${on_list[$i]}"
    Flag="${on_flag[$i]}"

    Type_path="${Ntuple_path}/${version}/${Type}"
    output_path="${Type_path}/MC16rd"
    log_path="${Type_path}/MC16rd/log"

    mkdir -p "${Type_path}"
    mkdir -p "${output_path}"
    mkdir -p "${log_path}"

    for file in $(find "${Skim_path}/MC16rd_off/${Type}/output" -maxdepth 1 -name "*.root")
    do
        echo $file
        basename=$(basename -s .root $file)
        bsub -q s \
        -o "${log_path}/${basename}.log" \
        ${script} \
        --sample "MC16rd" \
        --type ${Flag} \
        --energy "off" \
        --prompt \
        --vertex \
        --KEKCC \
        --inputfile ${file} \
        --destination "${output_path}"
        sleep 0.5
    done
done 

# MC16rd 4S 5S
on_list=("BB" "EE" "EEEE" "EEMUMU"
         "GG" "HHISR" "LLXX" "MUMU"
         "SIGNAL" "TAUPAIR" "UDSC")

on_flag=("BB" "ee" "eeee" "eemumu"
         "gg" "hhISR" "llXX" "mumu"
         "signal" "taupair" "udsc")

for i in "${!on_list[@]}"; do
    Type="${on_list[$i]}"
    Flag="${on_flag[$i]}"

    Type_path="${Ntuple_path}/${version}/${Type}"
    output_path="${Type_path}/MC16rd"
    log_path="${Type_path}/MC16rd/log"

    mkdir -p "${Type_path}"
    mkdir -p "${output_path}"
    mkdir -p "${log_path}"

    for file in $(find "${Skim_path}/MC16rd_5S/${Type}/output" -maxdepth 1 -name "*.root")
    do
        echo $file
        basename=$(basename -s .root $file)
        bsub -q s \
        -o "${log_path}/${basename}.log" \
        ${script} \
        --sample "MC16rd" \
        --type ${Flag} \
        --energy "5Sscan" \
        --prompt \
        --vertex \
        --KEKCC \
        --inputfile ${file} \
        --destination "${output_path}"
        sleep 0.5
    done
done 