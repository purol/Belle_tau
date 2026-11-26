#!/bin/sh

mkdir -p ./output
mkdir -p ./log
mkdir -p ./err

source /cvmfs/belle.cern.ch/tools/b2setup release-08-00-09
export PYTHONNOUSERSITE=1

# ========================================== tau- -> e- e+ e- ========================================================= #
# MC16ri run1 plus
for i in {1..270}
do
    bsub -q l -J MCGEN -o "./log/MC16ri_run1_SIGNAL_plus_${i}.log" -e "./err/MC16ri_run1_SIGNAL_plus_${i}.err" ./MC16ri_generation/MC16ri_run1_gen.py --decfile ./epepem.dat --outputfile "./output/MC16ri_run1_SIGNAL_plus_${i}.root"
    sleep 0.5
done

# MC16ri run1 minus
for i in {1..270}
do
    bsub -q l -J MCGEN -o "./log/MC16ri_run1_SIGNAL_minus_${i}.log" -e "./err/MC16ri_run1_SIGNAL_minus_${i}.err" ./MC16ri_generation/MC16ri_run1_gen.py --decfile ./ememep.dat --outputfile "./output/MC16ri_run1_SIGNAL_minus_${i}.root"
    sleep 0.5
done

source /cvmfs/belle.cern.ch/tools/b2setup release-08-03-00
export PYTHONNOUSERSITE=1

# MC16ri run2 PXD OFF plus
for i in {1..160}
do
    bsub -q l -J MCGEN -o "./log/MC16ri_run2_PXD_OFF_SIGNAL_plus_${i}.log" -e "./err/MC16ri_run2_PXD_OFF_SIGNAL_plus_${i}.err" ./MC16ri_generation/MC16ri_run2_PXD_OFF_gen.py --decfile ./epepem.dat --outputfile "./output/MC16ri_run2_PXD_OFF_SIGNAL_plus_${i}.root"
    sleep 0.5
done

# MC16ri run2 PXD OFF minus
for i in {1..160}
do
    bsub -q l -J MCGEN -o "./log/MC16ri_run2_PXD_OFF_SIGNAL_minus_${i}.log" -e "./err/MC16ri_run2_PXD_OFF_SIGNAL_minus_${i}.err" ./MC16ri_generation/MC16ri_run2_PXD_OFF_gen.py --decfile ./ememep.dat --outputfile "./output/MC16ri_run2_PXD_OFF_SIGNAL_minus_${i}.root"
    sleep 0.5
done

# MC16ri run2 PXD ON plus
for i in {1..160}
do
    bsub -q l -J MCGEN -o "./log/MC16ri_run2_PXD_ON_SIGNAL_plus_${i}.log" -e "./err/MC16ri_run2_PXD_ON_SIGNAL_plus_${i}.err" ./MC16ri_generation/MC16ri_run2_PXD_ON_gen.py --decfile ./epepem.dat --outputfile "./output/MC16ri_run2_PXD_ON_SIGNAL_plus_${i}.root"
    sleep 0.5
done

# MC16ri run2 PXD ON minus
for i in {1..160}
do
    bsub -q l -J MCGEN -o "./log/MC16ri_run2_PXD_ON_SIGNAL_minus_${i}.log" -e "./err/MC16ri_run2_PXD_ON_SIGNAL_minus_${i}.err" ./MC16ri_generation/MC16ri_run2_PXD_ON_gen.py --decfile ./ememep.dat --outputfile "./output/MC16ri_run2_PXD_ON_SIGNAL_minus_${i}.root"
    sleep 0.5
done

# ========================================== tau- -> mu- mu+ e- ========================================================= #
# MC16ri run1 plus
for i in {1..270}
do
    bsub -q l -J MCGEN -o "./log/MC16ri_run1_SIGNAL_plus_${i}.log" -e "./err/MC16ri_run1_SIGNAL_plus_${i}.err" ./MC16ri_generation/MC16ri_run1_gen.py --decfile ./mupepmum.dat --outputfile "./output/MC16ri_run1_SIGNAL_plus_${i}.root"
    sleep 0.5
done

# MC16ri run1 minus
for i in {1..270}
do
    bsub -q l -J MCGEN -o "./log/MC16ri_run1_SIGNAL_minus_${i}.log" -e "./err/MC16ri_run1_SIGNAL_minus_${i}.err" ./MC16ri_generation/MC16ri_run1_gen.py --decfile ./mumemmup.dat --outputfile "./output/MC16ri_run1_SIGNAL_minus_${i}.root"
    sleep 0.5
done

source /cvmfs/belle.cern.ch/tools/b2setup release-08-03-00
export PYTHONNOUSERSITE=1

# MC16ri run2 PXD OFF plus
for i in {1..160}
do
    bsub -q l -J MCGEN -o "./log/MC16ri_run2_PXD_OFF_SIGNAL_plus_${i}.log" -e "./err/MC16ri_run2_PXD_OFF_SIGNAL_plus_${i}.err" ./MC16ri_generation/MC16ri_run2_PXD_OFF_gen.py --decfile ./mupepmum.dat --outputfile "./output/MC16ri_run2_PXD_OFF_SIGNAL_plus_${i}.root"
    sleep 0.5
done

# MC16ri run2 PXD OFF minus
for i in {1..160}
do
    bsub -q l -J MCGEN -o "./log/MC16ri_run2_PXD_OFF_SIGNAL_minus_${i}.log" -e "./err/MC16ri_run2_PXD_OFF_SIGNAL_minus_${i}.err" ./MC16ri_generation/MC16ri_run2_PXD_OFF_gen.py --decfile ./mumemmup.dat --outputfile "./output/MC16ri_run2_PXD_OFF_SIGNAL_minus_${i}.root"
    sleep 0.5
done

# MC16ri run2 PXD ON plus
for i in {1..160}
do
    bsub -q l -J MCGEN -o "./log/MC16ri_run2_PXD_ON_SIGNAL_plus_${i}.log" -e "./err/MC16ri_run2_PXD_ON_SIGNAL_plus_${i}.err" ./MC16ri_generation/MC16ri_run2_PXD_ON_gen.py --decfile ./mupepmum.dat --outputfile "./output/MC16ri_run2_PXD_ON_SIGNAL_plus_${i}.root"
    sleep 0.5
done

# MC16ri run2 PXD ON minus
for i in {1..160}
do
    bsub -q l -J MCGEN -o "./log/MC16ri_run2_PXD_ON_SIGNAL_minus_${i}.log" -e "./err/MC16ri_run2_PXD_ON_SIGNAL_minus_${i}.err" ./MC16ri_generation/MC16ri_run2_PXD_ON_gen.py --decfile ./mumemmup.dat --outputfile "./output/MC16ri_run2_PXD_ON_SIGNAL_minus_${i}.root"
    sleep 0.5
done