#!/bin/sh

version="Nitori"
script="./python/gbasf2_${version}.py"
Skim_path="/home/belle2/junewoo/storage_ghi/tau_SKIM"
Ntuple_path="/home/belle2/junewoo/storage_ghi/tau_Ntuple"

4S_list=("CHARM" "CHG" "DDBAR" "EE" "EEEE"
         "EEKK" "EEMUMU" "EEPIPI" "EEPP" "EETAUTAU"
         "GG" "K0K0ISR" "KKISR" "MIX" "MUMU"
         "MUMUMUMU" "MUMUTAUTAU" "PIPIISR" "PIPIPI0ISR" "SIGNAL"
         "SSBAR" "TAUPAIR" "TAUTAUTAUTAU" "UUBAR")

4S_flag=("ccbar" "charged" "ddbar" "ee" "eeee"
        "eeKK" "eemumu" "eepipi" "eepp" "eetautau"
        "gg" "K0K0barISR" "KKISR" "mixed" "mumu"
        "mumumumu" "mumutautau" "pipiISR" "pipipi0ISR" "signal"
        "ssbar" "taupair" "tautautautau" "uubar")

for i in "${!4S_list[@]}"; do
    Type="${4S_list[$i]}"
    Flag="${4S_flag[$i]}"

    Type_path="${Ntuple_path}/${version}/${Type}"
    output_path="${Type_path}/MC15ri_4S"
    log_path="${Type_path}/log"

    mkdir -p "${Type_path}"
    mkdir -p "${output_path}"
    mkdir -p "${log_path}"

    for file in $(find "/home/belle2/junewoo/storage_ghi/tau_SKIM/MC15ri_on/${Type}/output" -maxdepth 1 -name "*.root")
    do
        echo $file
        basename=$(basename -s .root $file)
        bsub -q s -o "${log_path}/${basename}.log" ${script} --sample "MC15ri" --type ${Flag} --energy "4S" --vertex --KEKCC --input_file ${file} --destination "${output_path}"
        sleep 0.3
    done
done 


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
    output_path="${Type_path}/MC15ri_off"
    log_path="${Type_path}/log"

    mkdir -p "${Type_path}"
    mkdir -p "${output_path}"
    mkdir -p "${log_path}"

    for file in $(find "/home/belle2/junewoo/storage_ghi/tau_SKIM/MC15ri_off/${Type}/output" -maxdepth 1 -name "*.root")
    do
        echo $file
        basename=$(basename -s .root $file)
        bsub -q s -o "${log_path}/${basename}.log" ${script} --sample "MC15ri" --type ${Flag} --energy "off" --vertex --KEKCC --input_file ${file} --destination "${output_path}"
        sleep 0.3
    done
done 

10810_list=("BBs" "BsBs" "CHARM" "CHG" "DDBAR"
            "MIX" "MUMU" "SIGNAL" "SSBAR" "TAUPAIR"
            "UUBAR")

10810_flag=("BBs" "BsBs" "ccbar" "charged" "ddbar" 
            "mixed" "mumu" "signal" "ssbar" "taupair"
            "uubar")

for i in "${!10810_list[@]}"; do
    Type="${10810_list[$i]}"
    Flag="${10810_flag[$i]}"

    Type_path="${Ntuple_path}/${version}/${Type}"
    output_path="${Type_path}/MC15ri_10810"
    log_path="${Type_path}/log"

    mkdir -p "${Type_path}"
    mkdir -p "${output_path}"
    mkdir -p "${log_path}"

    for file in $(find "/home/belle2/junewoo/storage_ghi/tau_SKIM/MC15ri_5S/${Type}/output" -maxdepth 1 -name "*.root")
    do
        echo $file
        basename=$(basename -s .root $file)
        bsub -q s -o "${log_path}/${basename}.log" ${script} --sample "MC15ri" --type ${Flag} --energy "10810" --vertex --KEKCC --input_file ${file} --destination "${output_path}"
        sleep 0.3
    done
done 