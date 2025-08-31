#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# usage: basf2 tau_mumumu_SKIM.py
# last: 2025-08-30-00

import basf2
import modularAnalysis as ma

import stdCharged
import mdst

# set random seed
basf2.set_random_seed(42)
    
# Read mdst
my_path = basf2.create_path()

inputfile = ""
ma.inputMdst(environmentType='default', filename=inputfile, path=my_path)

output_file = "SKIM"

# fill FSP
trackCuts = "[-3.0 < dz < 3.0] and [dr < 1.0]"
stdCharged.stdMu("all", path=my_path)
stdCharged.stdPi("all", path=my_path)
ma.cutAndCopyList("pi+:taulfv", "pi+:all", trackCuts, path=my_path)

# reconstruct tau
ma.reconstructDecay(decayString="tau+:LFV_lll -> mu+:all mu+:all mu-:all", cut="[nParticlesInList(pi+:taulfv) < 7] and [1.4 < M < 2.0] and [-1.0 < deltaE < 0.5]", path=my_path)

# muonID > 0.1 for primary muon
Condition_one = '[[daughter(0, p) > daughter(1, p)] and [daughter(0, p) > daughter(2, p)] and [daughter(0, muonID) > 0.1]]'
Condition_two = '[[daughter(1, p) > daughter(0, p)] and [daughter(1, p) > daughter(2, p)] and [daughter(1, muonID) > 0.1]]'
Condition_three = '[[daughter(2, p) > daughter(0, p)] and [daughter(2, p) > daughter(1, p)] and [daughter(2, muonID) > 0.1]]'
ma.applyCuts('tau+:LFV_lll', Condition_one + ' or ' + Condition_two + ' or ' + Condition_three, my_path)

# tau reconstruction for control sample
ma.cutAndCopyList("pi+:control", "pi+:all", 'pionID > 0.9', path=my_path)
ma.reconstructDecay(decayString="tau+:control -> pi+:control pi+:control pi-:control", cut="[nParticlesInList(pi+:taulfv) < 7] and [0.5 < M < 1.7] and [-1.0 < deltaE < 0.0]", path=my_path)

# combine tau list
ma.copyLists(outputListName="tau+:comb", inputListNames=["tau+:LFV_lll", "tau+:control"], path=my_path)

# event cut
ma.applyEventCuts("nParticlesInList(tau+:comb) > 0", path=my_path)
             
# print mdst (not udst!)
mdst.add_mdst_output(my_path, mc=True, filename=output_file, additionalBranches=[], dataDescription=None)

# progress
basf2.process(my_path)

# Print call statistics
print(basf2.statistics)
