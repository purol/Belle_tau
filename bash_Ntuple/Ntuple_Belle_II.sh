#!/bin/bash

version="Ibaraki"
script="./python/gbasf2_${version}.py"
Skim_path="/home/belle2/junewoo/storage_ghi/tau_SKIM_2"
Ntuple_path="/home/belle2/junewoo/storage_ghi/tau_Ntuple"


# Build ALP-specific command-line options from the input filename.
# Expected pattern: ..._mass<MASS>_life<LIFETIME>_...
alp_args=()
set_alp_args() {
    local type="$1"
    local filename_base="$2"

    alp_args=()

    # Non-ALP samples must not receive --mass_ALP / --life_ALP.
    if [[ "${type}" != "ALP" ]]; then
        return 0
    fi

    if [[ "${filename_base}" =~ _mass([^_]+)_life([^_]+)_ ]]; then
        local mass_ALP="${BASH_REMATCH[1]}"
        local life_ALP="${BASH_REMATCH[2]}"
        alp_args=(--mass_ALP "${mass_ALP}" --life_ALP "${life_ALP}")
        echo "ALP parameters: mass=${mass_ALP}, life=${life_ALP}"
    else
        echo "ERROR: could not extract ALP mass/lifetime from filename: ${filename_base}" >&2
        return 1
    fi
}

# MC16ri 4S on-resonance
on_list=("ALP" "CCBAR" "CHG" "DDBAR" "EE" "EEEE" 
         "EEMUMU" "GG" "HHISR" "LLXX" "MIX" 
         "MUMU" "SIGNAL" "SSBAR" "TAUPAIR" "UUBAR")

on_flag=("ALP" "ccbar" "charged" "ddbar" "ee" "eeee"
         "eemumu" "gg" "hhISR" "llXX" "mixed"
         "mumu" "signal" "ssbar" "taupair" "uubar")

queue_type=("s" "s" "s" "s" "s" "s"
         "s" "s" "s" "s" "s"
         "s" "s" "s" "s" "s")

for i in "${!on_list[@]}"; do
    Type="${on_list[$i]}"
    Flag="${on_flag[$i]}"
    Queue="${queue_type[$i]}"

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
        set_alp_args "${Type}" "${basename}" || continue
        bsub -q ${Queue} \
        -o "${log_path}/${basename}.log" \
        ${script} \
        --sample "MC16ri" \
        --type ${Flag} \
        --energy "4S" \
        --prompt \
        --vertex \
        "${alp_args[@]}" \
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

queue_type=("s" "s" "s" "l"
         "l" "s" "s" "l"
         "s" "s" "s" "s")

for i in "${!on_list[@]}"; do
    Type="${on_list[$i]}"
    Flag="${on_flag[$i]}"
    Queue="${queue_type[$i]}"

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
        set_alp_args "${Type}" "${basename}" || continue
        bsub -q ${Queue} \
        -o "${log_path}/${basename}.log" \
        ${script} \
        --sample "MC16rd" \
        --type ${Flag} \
        --energy "4S" \
        --prompt \
        --vertex \
        "${alp_args[@]}" \
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

queue_type=("s" "l" "s" "s"
         "s" "s" "s"
         "s" "s" "s")

for i in "${!on_list[@]}"; do
    Type="${on_list[$i]}"
    Flag="${on_flag[$i]}"
    Queue="${queue_type[$i]}"

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
        set_alp_args "${Type}" "${basename}" || continue
        bsub -q ${Queue} \
        -o "${log_path}/${basename}.log" \
        ${script} \
        --sample "MC16rd" \
        --type ${Flag} \
        --energy "off" \
        --prompt \
        --vertex \
        "${alp_args[@]}" \
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

queue_type=("s" "s" "s" "s"
         "s" "s" "s" "s"
         "s" "s" "s")

for i in "${!on_list[@]}"; do
    Type="${on_list[$i]}"
    Flag="${on_flag[$i]}"
    Queue="${queue_type[$i]}"

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
        set_alp_args "${Type}" "${basename}" || continue
        bsub -q ${Queue} \
        -o "${log_path}/${basename}.log" \
        ${script} \
        --sample "MC16rd" \
        --type ${Flag} \
        --energy "5Sscan" \
        --prompt \
        --vertex \
        "${alp_args[@]}" \
        --KEKCC \
        --inputfile ${file} \
        --destination "${output_path}"
        sleep 0.5
    done
done 