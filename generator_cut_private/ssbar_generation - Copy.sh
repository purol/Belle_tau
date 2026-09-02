#!/bin/bash

output_path="/home/belle2/junewoo/storage_ghi/tau_gen_cut/MC16ri/MC16ri_private_gencut/CCBAR"

source /cvmfs/belle.cern.ch/tools/b2setup release-08-00-09
export PYTHONNOUSERSITE=1

for i in {0..7899}; do
    queue="l"

    bsub -q "$queue" \
        -o "${output_path}/run1_${i}.log" \
        -e "${output_path}/run1_${i}.err" \
        ./generator_cut_private/ccbar_eph3_MC16ri_run1_gencut.py \
        --output "${output_path}/CCBAR_run1_${i}.root"

    sleep 0.3s
done

source /cvmfs/belle.cern.ch/tools/b2setup release-08-03-00
export PYTHONNOUSERSITE=1

for i in {0..4779}; do
    queue="l"

    bsub -q "$queue" \
        -o "${output_path}/run2_PXDOFF_${i}.log" \
        -e "${output_path}/run2_PXDOFF_${i}.err" \
        ./generator_cut_private/ccbar_eph3_MC16ri_run2_PXD_OFF_gencut.py \
        --output "${output_path}/CCBAR_run2_PXDOFF_${i}.root"

    sleep 0.3s
done


for i in {0..4779}; do
    queue="l"

    bsub -q "$queue" \
        -o "${output_path}/run2_PXDON_${i}.log" \
        -e "${output_path}/run2_PXDON_${i}.err" \
        ./generator_cut_private/ccbar_eph3_MC16ri_run2_PXD_ON_gencut.py \
        --output "${output_path}/CCBAR_run2_PXDON_${i}.root"

    sleep 0.3s
done