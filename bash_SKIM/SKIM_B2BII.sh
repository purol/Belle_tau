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
                    find -L "${ENERGY_PATH}" -mindepth 2 -type f -name "*.mdst"
                fi
            done
        done
    done
done