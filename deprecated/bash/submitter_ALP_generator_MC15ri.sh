#!/bin/bash

DAT_DIR="dat_files"

for datfile in $DAT_DIR/*.dat; do
    # Extract parameters from filename: alpha_mass0.2_life0.1_A1_B1.dat
    mass=$(echo $datfile | grep -oP 'mass\K[0-9.]+')
    lifetime=$(echo $datfile | grep -oP 'life\K[0-9.]+')
    A=$(echo $datfile | grep -oP 'A\K-?[01]')
    B=$(echo $datfile | grep -oP 'B\K-?[01]')
    
    echo "Generating MC for $datfile"
    bsub -q l -o "./${datfile}.log" ./ALP_generator_MC15ri.py "$datfile" "$mass" "$lifetime" "$A" "$B"
done
