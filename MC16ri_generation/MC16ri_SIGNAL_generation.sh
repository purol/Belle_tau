#!/bin/sh

mkdir -p ./output
mkdir -p ./log
mkdir -p ./err

# MC16ri run1 plus
for i in {1..270}
do
    bsub -q l -J MCGEN -o "./log/MC16ri_run1_SIGNAL_plus_${i}.log" -e "./err/MC16ri_run1_SIGNAL_plus_${i}.err" ./python/MC16ri_run1_gen.py --decfile ./3410931053.dat --outputfile "./output/MC16ri_run1_SIGNAL_plus_${i}.root"
    sleep 0.5
done

# MC16ri run1 minus
for i in {1..270}
do
    bsub -q l -J MCGEN -o "./log/MC16ri_run1_SIGNAL_minus_${i}.log" -e "./err/MC16ri_run1_SIGNAL_minus_${i}.err" ./python/MC16ri_run1_gen.py --decfile ./3410931052.dat --outputfile "./output/MC16ri_run1_SIGNAL_minus_${i}.root"
    sleep 0.5
done

# MC16ri run2 PXD OFF plus
for i in {1..160}
do
    bsub -q l -J MCGEN -o "./log/MC16ri_run2_PXD_OFF_SIGNAL_plus_${i}.log" -e "./err/MC16ri_run2_PXD_OFF_SIGNAL_plus_${i}.err" ./python/MC16ri_run2_PXD_OFF_gen.py --decfile ./3410931053.dat --outputfile "./output/MC16ri_run2_PXD_OFF_SIGNAL_plus_${i}.root"
    sleep 0.5
done

# MC16ri run2 PXD OFF minus
for i in {1..160}
do
    bsub -q l -J MCGEN -o "./log/MC16ri_run2_PXD_OFF_SIGNAL_minus_${i}.log" -e "./err/MC16ri_run2_PXD_OFF_SIGNAL_minus_${i}.err" ./python/MC16ri_run2_PXD_OFF_gen.py --decfile ./3410931052.dat --outputfile "./output/MC16ri_run2_PXD_OFF_SIGNAL_minus_${i}.root"
    sleep 0.5
done

# MC16ri run2 PXD ON plus
for i in {1..160}
do
    bsub -q l -J MCGEN -o "./log/MC16ri_run2_PXD_ON_SIGNAL_plus_${i}.log" -e "./err/MC16ri_run2_PXD_ON_SIGNAL_plus_${i}.err" ./python/MC16ri_run2_PXD_ON_gen.py --decfile ./3410931053.dat --outputfile "./output/MC16ri_run2_PXD_ON_SIGNAL_plus_${i}.root"
    sleep 0.5
done

# MC16ri run2 PXD ON minus
for i in {1..160}
do
    bsub -q l -J MCGEN -o "./log/MC16ri_run2_PXD_ON_SIGNAL_minus_${i}.log" -e "./err/MC16ri_run2_PXD_ON_SIGNAL_minus_${i}.err" ./python/MC16ri_run2_PXD_ON_gen.py --decfile ./3410931052.dat --outputfile "./output/MC16ri_run2_PXD_ON_SIGNAL_minus_${i}.root"
    sleep 0.5
done
