#!/bin/bash

version="Ichirin"
script="./python/gbasf2_${version}.py"
Skim_path="/home/belle2/junewoo/storage_ghi/tau_SKIM_2"
Ntuple_path="/home/belle2/junewoo/storage_ghi/tau_Ntuple"

# 4S on-resonance
on_list=("CHARM" "CHG" "DDBAR" "EE" "EEEE"
         "EEKK" "EEMUMU" "EEPIPI" "EEPP" "EETAUTAU"
         "GG" "K0K0ISR" "KKISR" "MIX" "MUMU"
         "MUMUMUMU" "MUMUTAUTAU" "PIPIISR" "PIPIPI0ISR" "SIGNAL"
         "SSBAR" "TAUPAIR" "TAUTAUTAUTAU" "UUBAR" "ALP")

on_flag=("ccbar" "charged" "ddbar" "ee" "eeee"
        "eeKK" "eemumu" "eepipi" "eepp" "eetautau"
        "gg" "K0K0barISR" "KKISR" "mixed" "mumu"
        "mumumumu" "mumutautau" "pipiISR" "pipipi0ISR" "signal"
        "ssbar" "taupair" "tautautautau" "uubar" "ALP")

for i in "${!on_list[@]}"; do
    Type="${on_list[$i]}"
    Flag="${on_flag[$i]}"

    Type_path="${Ntuple_path}/${version}/${Type}"
    output_path="${Type_path}/MC15ri"
    log_path="${Type_path}/MC15ri/log"

    mkdir -p "${Type_path}"
    mkdir -p "${output_path}"
    mkdir -p "${log_path}"

    for file in $(find "${Skim_path}/MC15ri_on/${Type}/output" -maxdepth 1 -name "*.root")
    do
        echo $file
        basename=$(basename -s .root $file)
        bsub -q s -o "${log_path}/${basename}.log" ${script} --sample "MC15ri" --type ${Flag} --energy "4S" --prompt --vertex --KEKCC --inputfile ${file} --destination "${output_path}"
        sleep 0.3
    done
done 

# off-resonance
off_list=("CHARM" "DDBAR" "EE" "EEEE" "EEKK"
          "EEMUMU" "EEPIPI" "EEPP" "EETAUTAU" "GG" 
          "MUMU" "MUMUMUMU" "SIGNAL" "SSBAR" "TAUPAIR"
          "UUBAR")

off_flag=("ccbar" "ddbar" "ee" "eeee" "eeKK"
          "eemumu" "eepipi" "eepp" "eetautau" "gg"
          "mumu" "mumumumu" "signal" "ssbar" "taupair"
          "uubar")

for i in "${!off_list[@]}"; do
    Type="${off_list[$i]}"
    Flag="${off_flag[$i]}"

    Type_path="${Ntuple_path}/${version}/${Type}"
    output_path="${Type_path}/MC15ri"
    log_path="${Type_path}/MC15ri/log"

    mkdir -p "${Type_path}"
    mkdir -p "${output_path}"
    mkdir -p "${log_path}"

    for file in $(find "${Skim_path}/MC15ri_off/${Type}/output" -maxdepth 1 -name "*.root")
    do
        echo $file
        basename=$(basename -s .root $file)
        bsub -q s -o "${log_path}/${basename}.log" ${script} --sample "MC15ri" --type ${Flag} --energy "off" --prompt --vertex --KEKCC --inputfile ${file} --destination "${output_path}"
        sleep 0.3
    done
done 

# E = 10.810 GeV
Scan_10810_list=("BBs" "BsBs" "CHARM" "CHG" "DDBAR"
            "MIX" "MUMU" "SIGNAL" "SSBAR" "TAUPAIR"
            "UUBAR")

Scan_10810_flag=("BBs" "BsBs" "ccbar" "charged" "ddbar" 
            "mixed" "mumu" "signal" "ssbar" "taupair"
            "uubar")

for i in "${!Scan_10810_list[@]}"; do
    Type="${Scan_10810_list[$i]}"
    Flag="${Scan_10810_flag[$i]}"

    Type_path="${Ntuple_path}/${version}/${Type}"
    output_path="${Type_path}/MC15ri"
    log_path="${Type_path}/MC15ri/log"

    mkdir -p "${Type_path}"
    mkdir -p "${output_path}"
    mkdir -p "${log_path}"

    for file in $(find "${Skim_path}/MC15ri_5S/${Type}/output" -maxdepth 1 -name "*.root")
    do
        echo $file
        basename=$(basename -s .root $file)
        bsub -q s -o "${log_path}/${basename}.log" ${script} --sample "MC15ri" --type ${Flag} --energy "10810" --prompt --vertex --KEKCC --inputfile ${file} --destination "${output_path}"
        sleep 0.3
    done
done 