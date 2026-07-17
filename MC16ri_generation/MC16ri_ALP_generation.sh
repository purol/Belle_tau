#!/bin/sh

DAT_DIR_PLUS="./dat_files_plus"
DAT_DIR_MINUS="./dat_files_minus"

mkdir -p ./output
mkdir -p ./log
mkdir -p ./err

for datfile in ${DAT_DIR_PLUS}/*.dat; do
    for iterator in {1..5}; do
        # Extract parameters from filename: alpha_mass0.2_life0.1_A1_B1.dat
        mass=$(echo $datfile | grep -oP 'mass\K[0-9.]+')
        lifetime=$(echo $datfile | grep -oP 'life\K[0-9.]+')
        A=$(echo $datfile | grep -oP 'A\K-?[01]')
        B=$(echo $datfile | grep -oP 'B\K-?[01]')

        nopath_datfile=$(basename "$datfile")

        echo "Generating MC for $datfile"
        bsub -q l -o "./log/MC16ri_run1_plus_${nopath_datfile}_${iterator}.log" ./MC16ri_generation/MC16ri_run1_gen.py --decfile ${datfile} --outputfile "./output/MC16ri_run1_plus_alpha_mass${mass}_life${lifetime}_A${A}_B${B}_${iterator}.root"
        sleep 0.5
    done
done

for datfile in ${DAT_DIR_PLUS}/*.dat; do
    for iterator in {1..3}; do
        # Extract parameters from filename: alpha_mass0.2_life0.1_A1_B1.dat
        mass=$(echo $datfile | grep -oP 'mass\K[0-9.]+')
        lifetime=$(echo $datfile | grep -oP 'life\K[0-9.]+')
        A=$(echo $datfile | grep -oP 'A\K-?[01]')
        B=$(echo $datfile | grep -oP 'B\K-?[01]')

        nopath_datfile=$(basename "$datfile")

        echo "Generating MC for $datfile"
        bsub -q l -o "./log/MC16ri_run2_PXD_OFF_plus_${nopath_datfile}_${iterator}.log" ./MC16ri_generation/MC16ri_run2_PXD_OFF_gen.py --decfile ${datfile} --outputfile "./output/MC16ri_run2_PXD_OFF_plus_alpha_mass${mass}_life${lifetime}_A${A}_B${B}_${iterator}.root"
        sleep 0.5
    done
done

for datfile in ${DAT_DIR_PLUS}/*.dat; do
    for iterator in {1..3}; do
        # Extract parameters from filename: alpha_mass0.2_life0.1_A1_B1.dat
        mass=$(echo $datfile | grep -oP 'mass\K[0-9.]+')
        lifetime=$(echo $datfile | grep -oP 'life\K[0-9.]+')
        A=$(echo $datfile | grep -oP 'A\K-?[01]')
        B=$(echo $datfile | grep -oP 'B\K-?[01]')

        nopath_datfile=$(basename "$datfile")

        echo "Generating MC for $datfile"
        bsub -q l -o "./log/MC16ri_run2_PXD_ON_plus_${nopath_datfile}_${iterator}.log" ./MC16ri_generation/MC16ri_run2_PXD_ON_gen.py --decfile ${datfile} --outputfile "./output/MC16ri_run2_PXD_ON_plus_alpha_mass${mass}_life${lifetime}_A${A}_B${B}_${iterator}.root"
        sleep 0.5
    done
done

for datfile in ${DAT_DIR_MINUS}/*.dat; do
    for iterator in {1..5}; do
        # Extract parameters from filename: alpha_mass0.2_life0.1_A1_B1.dat
        mass=$(echo $datfile | grep -oP 'mass\K[0-9.]+')
        lifetime=$(echo $datfile | grep -oP 'life\K[0-9.]+')
        A=$(echo $datfile | grep -oP 'A\K-?[01]')
        B=$(echo $datfile | grep -oP 'B\K-?[01]')

        nopath_datfile=$(basename "$datfile")

        echo "Generating MC for $datfile"
        bsub -q l -o "./log/MC16ri_run1_minus_${nopath_datfile}_${iterator}.log" ./MC16ri_generation/MC16ri_run1_gen.py --decfile ${datfile} --outputfile "./output/MC16ri_run1_minus_alpha_mass${mass}_life${lifetime}_A${A}_B${B}_${iterator}.root"
        sleep 0.5
    done
done

for datfile in ${DAT_DIR_MINUS}/*.dat; do
    for iterator in {1..3}; do
        # Extract parameters from filename: alpha_mass0.2_life0.1_A1_B1.dat
        mass=$(echo $datfile | grep -oP 'mass\K[0-9.]+')
        lifetime=$(echo $datfile | grep -oP 'life\K[0-9.]+')
        A=$(echo $datfile | grep -oP 'A\K-?[01]')
        B=$(echo $datfile | grep -oP 'B\K-?[01]')

        nopath_datfile=$(basename "$datfile")

        echo "Generating MC for $datfile"
        bsub -q l -o "./log/MC16ri_run2_PXD_OFF_minus_${nopath_datfile}_${iterator}.log" ./MC16ri_generation/MC16ri_run2_PXD_OFF_gen.py --decfile ${datfile} --outputfile "./output/MC16ri_run2_PXD_OFF_minus_alpha_mass${mass}_life${lifetime}_A${A}_B${B}_${iterator}.root"
        sleep 0.5
    done
done

for datfile in ${DAT_DIR_MINUS}/*.dat; do
    for iterator in {1..3}; do
        # Extract parameters from filename: alpha_mass0.2_life0.1_A1_B1.dat
        mass=$(echo $datfile | grep -oP 'mass\K[0-9.]+')
        lifetime=$(echo $datfile | grep -oP 'life\K[0-9.]+')
        A=$(echo $datfile | grep -oP 'A\K-?[01]')
        B=$(echo $datfile | grep -oP 'B\K-?[01]')

        nopath_datfile=$(basename "$datfile")

        echo "Generating MC for $datfile"
        bsub -q l -o "./log/MC16ri_run2_PXD_ON_minus_${nopath_datfile}_${iterator}.log" ./MC16ri_generation/MC16ri_run2_PXD_ON_gen.py --decfile ${datfile} --outputfile "./output/MC16ri_run2_PXD_ON_minus_alpha_mass${mass}_life${lifetime}_A${A}_B${B}_${iterator}.root"
        sleep 0.5
    done
done