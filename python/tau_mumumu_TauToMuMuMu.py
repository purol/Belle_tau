#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# usage: basf2 tau_mumumu_SKIM.py
# last: 2025-08-30-00

import basf2
import modularAnalysis as ma
from b2biiConversion import convertBelleMdstToBelleIIMdst

import stdCharged
import mdst

import argparse

parser = argparse.ArgumentParser()
parser.add_argument(
    '--b2bii',
    action='store_true',
    help='Enable b2bii'
)
parser.add_argument(
    '--input_file',
    type=str,
    default="",
    help='Path to input ROOT file'
)
parser.add_argument(
    '--output_file',
    type=str,
    default="SKIM.root",
    help='Path to output ROOT file'
)
args = parser.parse_args()

# set random seed
basf2.set_random_seed(42)
    
# Read mdst
my_path = basf2.create_path()

inputfile = args.input_file
if args.b2bii:
    convertBelleMdstToBelleIIMdst(inputfile, path=my_path)
else:
    ma.inputMdst(environmentType='default', filename=inputfile, path=my_path)

output_file = args.output_file

# fill FSP
trackCuts = "[-3.0 < dz < 3.0] and [dr < 1.0]"
stdCharged.stdMu("all", path=my_path)
stdCharged.stdPi("all", path=my_path)
ma.cutAndCopyList("pi+:tau_3mu_goodtrack", "pi+:all", trackCuts, path=my_path)
ma.cutAndCopyList("mu+:tau_3mu_goodtrack", "mu+:all", trackCuts, path=my_path)

# reconstruct tau
ma.reconstructDecay(decayString="tau+:tau_3mu_mumumu -> mu+:tau_3mu_goodtrack mu+:all mu-:all", cut="[nParticlesInList(pi+:tau_3mu_goodtrack) < 7] and [1.3 < M < 2.2] and [-1.0 < deltaE < 0.5]", path=my_path)

# event cut
ma.applyEventCuts("nParticlesInList(tau+:tau_3mu_mumumu) > 0", path=my_path)
             
# print udst
if args.b2bii:
    b2bii_list = ["gamma:mdst", "anti-n0:mdst", "pi0:mdst", "K_S0:mdst", "K_L0:mdst", "Lambda0:mdst", "anti-Lambda0:mdst", "gamma:v0mdst"]
    ma.removeParticlesNotInLists(lists_to_keep=b2bii_list, path=my_path)
    ma.outputUdst(filename=output_file, particleLists=b2bii_list, path=my_path)
else:
    mdst.add_mdst_output(my_path, mc=True, filename=output_file, additionalBranches=[], dataDescription=None)

# progress
basf2.process(my_path)

# Print call statistics
print(basf2.statistics)
