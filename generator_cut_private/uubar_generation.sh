#!/bin/bash

export PYTHONNOUSERSITE=1

output_path="/home/belle2/junewoo/storage_ghi/tau_gen_cut/MC16ri/MC16ri_private_gencut/UUBAR"


for i in {0..2378}; do
    queue="l"

    bsub -q "$queue" \
        -o "${output_path}/run1_${i}.log" \
        -e "${output_path}/run1_${i}.err" \
        ./generator_cut_private/uubar_eph3_MC16ri_run1_gencut.py \
        --output "${output_path}/UUBAR_run1_${i}.root"

    sleep 0.3s
done


for i in {0..1427}; do
    queue="l"

    bsub -q "$queue" \
        -o "${output_path}/run2_PXDOFF_${i}.log" \
        -e "${output_path}/run2_PXDOFF_${i}.err" \
        ./generator_cut_private/uubar_eph3_MC16ri_run2_PXD_OFF_gencut.py \
        --output "${output_path}/UUBAR_run2_PXDOFF_${i}.root"

    sleep 0.3s
done


for i in {0..1427}; do
    queue="l"

    bsub -q "$queue" \
        -o "${output_path}/run2_PXDON_${i}.log" \
        -e "${output_path}/run2_PXDON_${i}.err" \
        ./generator_cut_private/uubar_eph3_MC16ri_run2_PXD_ON_gencut.py \
        --output "${output_path}/UUBAR_run2_PXDON_${i}.root"

    sleep 0.3s
done