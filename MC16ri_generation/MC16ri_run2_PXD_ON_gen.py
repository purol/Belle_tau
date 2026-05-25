#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Descriptor: taupair

#############################################################
# Steering file for official MC production of early phase 3
# 'taupair' samples with beam backgrounds (BGx1).
#
# September 2020 - Belle II Collaboration
#############################################################

import basf2 as b2
import generators as ge
import simulation as si
import reconstruction as re
import mdst as mdst
import glob as glob

import argparse

# generation settings
parser = argparse.ArgumentParser(description='generation settings')
parser.add_argument('--decfile', required=True, type=str, help='dec file with path')
parser.add_argument('--outputfile', required=True, type=str, help='output file with path')

args = parser.parse_args()
decfile = args.decfile
outputfile = args.outputfile

# background (collision) files
bg = glob.glob('./*.root')
# background if running locally
bg_local = glob.glob('/group/belle2/dataprod/BGOverlay/run2/prerelease-08-00-00a/new_overlay/BGx1/set?/*root')
# switching to CVMFS as primary metadata provider.
b2.conditions.metadata_providers = ['/cvmfs/belle.cern.ch/conditions/database.sqlite']

# set database conditions (in addition to default)
# b2.conditions.prepend_globaltag("release-08-00-09")

# create path
main = b2.Path()

# default to early phase 3 (exp=1003), run 0
main.add_module("EventInfoSetter", expList=1004, runList=0, evtNumList=10000)

# use KKMC to generate taupair events
ge.add_kkmc_generator(path=main, finalstate='tau-tau+', signalconfigfile=decfile)

# detector simulation
si.add_simulation(path=main, bkgfiles=bg_local)

# reconstruction
re.add_reconstruction(path=main)

# Finally add mdst output (file name overwritten on the grid)
mdst.add_mdst_output(path=main, filename=outputfile)

# process events and print call statistics
b2.process(path=main)
print(b2.statistics)

