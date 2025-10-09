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
ma.cutAndCopyList("pi+:taulfv", "pi+:all", trackCuts, path=my_path)

# reconstruct tau
ma.reconstructDecay(decayString="tau+:LFV_lll -> mu+:all mu+:all mu-:all", cut="[nParticlesInList(pi+:taulfv) < 7] and [1.4 < M < 2.0] and [-1.0 < deltaE < 0.5]", path=my_path)

# muonID > 0.1 for primary muon
if args.b2bii:
    Condition_one = '[[daughter(0, p) > daughter(1, p)] and [daughter(0, p) > daughter(2, p)] and [daughter(0, muIDBelle) > 0.1]]'
    Condition_two = '[[daughter(1, p) > daughter(0, p)] and [daughter(1, p) > daughter(2, p)] and [daughter(1, muIDBelle) > 0.1]]'
    Condition_three = '[[daughter(2, p) > daughter(0, p)] and [daughter(2, p) > daughter(1, p)] and [daughter(2, muIDBelle) > 0.1]]'
else:
    Condition_one = '[[daughter(0, p) > daughter(1, p)] and [daughter(0, p) > daughter(2, p)] and [daughter(0, muonID) > 0.1]]'
    Condition_two = '[[daughter(1, p) > daughter(0, p)] and [daughter(1, p) > daughter(2, p)] and [daughter(1, muonID) > 0.1]]'
    Condition_three = '[[daughter(2, p) > daughter(0, p)] and [daughter(2, p) > daughter(1, p)] and [daughter(2, muonID) > 0.1]]'
ma.applyCuts('tau+:LFV_lll', Condition_one + ' or ' + Condition_two + ' or ' + Condition_three, my_path)

# tau reconstruction for control sample
if args.b2bii:
    ma.cutAndCopyList("pi+:control", "pi+:all", 'atcPIDBelle(2,3) > 0.9', path=my_path)
else:
    ma.cutAndCopyList("pi+:control", "pi+:all", 'pionID > 0.9', path=my_path)
ma.reconstructDecay(decayString="tau+:control -> pi+:control pi+:control pi-:control", cut="[nParticlesInList(pi+:taulfv) < 7] and [0.5 < M < 2.0] and [-1.0 < deltaE < 0.5]", path=my_path)

# combine tau list
ma.copyLists(outputListName="tau+:comb", inputListNames=["tau+:LFV_lll", "tau+:control"], path=my_path)

# event cut
ma.applyEventCuts("nParticlesInList(tau+:comb) > 0", path=my_path)
             
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
