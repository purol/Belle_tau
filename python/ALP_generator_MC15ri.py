#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import basf2 as b2
import generators as ge
import simulation as si
import reconstruction as re
import mdst as mdst
import glob
import pdg

if len(sys.argv) != 6:
    print("Usage: python generate_mc.py <dat_file> <mass> <lifetime> <A_coef> <B_coef>")
    sys.exit(1)

dat_file = sys.argv[1]
mass = float(sys.argv[2])
lifetime = float(sys.argv[3])
A_coef = float(sys.argv[4])
B_coef = float(sys.argv[5])
width = 0.004  # GeV

# Add exotic particle
pdg.add_particle('alpha', 94144, mass, width, 0, 0)

# Background files
bg_files = glob.glob('/group/belle2/dataprod/BGOverlay/early_phase3/release-06-00-05/overlay/BGx1/set0/*.root')

# Set database conditions
b2.conditions.prepend_globaltag("mc_production_MC15ri_a")

# Create path
main = b2.create_path()
main.add_module("EventInfoSetter", expList=1003, runList=0, evtNumList=10000)

# Generate events from dat file using KKMC
ge.add_kkmc_generator(path=main, finalstate='tau-tau+', signalconfigfile=dat_file)

# Detector simulation
si.add_simulation(main, bkgfiles=bg_files)

# Reconstruction
re.add_reconstruction(main)

# MDST output: include all parameters in filename
basename = os.path.basename(dat_file).replace('.dat','')
output_name = f"mdst_{basename}.root"
mdst.add_mdst_output(main, filename=output_name)

# Process events
b2.process(main)
print(b2.statistics)
