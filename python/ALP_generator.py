#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Descriptor: tau- -> mu- mu- mu+ 

#############################################################
# Steering file for official MC production of signal samples
#
# October 2020 - Belle II Collaboration
#############################################################

import basf2 as b2
import generators as ge
import simulation as si
import reconstruction as re
import mdst as mdst
import glob as glob
import pdg as pdg

mass = 1.0  # in GeV
width = 0.004  # in GeV
pdg.add_particle('alpha', 94144, mass, width, 0, 0)

# decay file
decfile = b2.find_file('./3410931052.dat')

# background (collision) files
bg = glob.glob('/group/belle2/dataprod/BGOverlay/early_phase3/release-06-00-05/overlay/BGx1/set0/*.root')

# set database conditions (in addition to default)
b2.conditions.prepend_globaltag("mc_production_MC15ri_a")

# create path
main = b2.create_path()

# specify number of events to be generated
main.add_module("EventInfoSetter", expList=1003, runList=0, evtNumList=10000)

# generate events from decfile
# use KKMC to generate taupair events 
ge.add_kkmc_generator(path=main, finalstate='tau-tau+', signalconfigfile=decfile)

# detector simulation
si.add_simulation(main, bkgfiles=bg)


# reconstruction
re.add_reconstruction(main)

# Finally add mdst output
mdst.add_mdst_output(main, filename="mdst.root")

# process events and print call statistics
b2.process(main)
print(b2.statistics)
