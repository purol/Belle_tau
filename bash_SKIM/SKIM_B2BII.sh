#!/bin/bash

BASE_DIR="/group/belle/bdata_b/mcprod/dat"

# Explicit experiment numbers
EXP_LIST=(
    "e000007" "e000009" "e000011" "e000013" "e000015"
    "e000017" "e000019" "e000021" "e000023" "e000025"
    "e000027" "e000031" "e000033" "e000035" "e000037"
    "e000039" "e000041" "e000043" "e000045" "e000047"
    "e000049" "e000051" "e000053" "e000055" "e000061"
    "e000063" "e000065" "e000067" "e000069" "e000071"
)

# Experiments that use 0807 level
LEVEL_0807_EXPS=(
    "e000007" "e000009" "e000011" "e000013" "e000015"
    "e000017" "e000019" "e000021" "e000023" "e000025"
    "e000027"
)

# Valid MC types
MC_TYPES=("charged" "charm" "mixed" "uds" "bsbs" "nonbsbs")

# Valid Belle levels
BELLE_LEVEL_0807="0807"
BELLE_LEVEL_0127="0127"

# Valid energy directories
ENERGIES=("on_resonance" "continuum" "energy_scan" "5S_scan" "5S_onresonance")

# --- Define your job script and bsub options here ---
# The executable script that will process one mdst file
MY_EXECUTABLE="/path/to/your/job_script.sh"

# Directory to store job log files
OUTPUT_DIR="/home/belle2/junewoo/storage_ghi/tau_SKIM/Belle"


for EXP in "${EXP_LIST[@]}"; do
    EXP_DIR="$BASE_DIR/$EXP"

    # Determine correct Belle level
    BELLE_LEVEL="$BELLE_LEVEL_0127"
    for EXP0807 in "${LEVEL_0807_EXPS[@]}"; do
        if [[ "$EXP" == "$EXP0807" ]]; then
            BELLE_LEVEL="$BELLE_LEVEL_0807"
            break
        fi
    done

    for MC_TYPE in "${MC_TYPES[@]}"; do
        MC_PATH="$EXP_DIR/evtgen/$MC_TYPE"
        if [ ! -d "$MC_PATH" ]; then
            continue
        fi

        # Loop over stream numbers (e.g., 01, 02, 11, etc.)
        for STREAM_DIR in "$MC_PATH"/*/; do
            [ -d "$STREAM_DIR" ] || continue

            ALL_PATH="$STREAM_DIR/all/$BELLE_LEVEL"
            [ -d "$ALL_PATH" ] || continue

            for ENERGY in "${ENERGIES[@]}"; do
                ENERGY_PATH="$ALL_PATH/$ENERGY"
                if [ -d "$ENERGY_PATH" ]; then
                    # --- MODIFICATION START ---
                    # Find all .mdst files and pipe the list to a while loop
                    find -L "${ENERGY_PATH}" -mindepth 2 -type f -name "*.mdst" | while IFS= read -r mdst_file; do
                    
                        file_name=$(basename "${mdst_file%.*}")

                        JOB_DIR="${OUTPUT_DIR}/${ENERGY}/${MC_TYPE}"
                        LOG_DIR="${JOB_DIR}/log"
                        OUTPUT_DIR="${JOB_DIR}/output"

                        mkdir -p ${JOB_DIR}
                        mkdir -p ${LOG_DIR}
                        mkdir -p ${OUTPUT_DIR}

                        bsub -q l -o "${LOG_DIR}/${file_name}.log" ./python/tau_mumumu_SKIM.py --input_file ${mdst_file} --output_file "${OUTPUT_DIR}/SKIM_${file_name}.root" --b2bii

                        sleep 0.3s
                    done
                fi
            done
        done
    done
done

echo "All submission loops complete."