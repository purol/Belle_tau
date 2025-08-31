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
trackCuts = "-3.0 < dz < 3.0 and dr < 1.0"
stdCharged.stdMu("all", path=my_path)
stdCharged.stdPi("all", path=my_path)
ma.cutAndCopyList("pi+:taulfv", "pi+:all", trackCuts, path=my_path)

# reconstruct tau
ma.reconstructDecay(decayString="tau+:LFV_lll -> mu+:all mu+:all mu-:all", cut="[nParticlesInList(pi+:taulfv) < 7] and [1.4 < M < 2.0] and [-1.0 < deltaE < 0.5]", path=my_path)

# muonID > 0.1 for primary muon
ma.buildRestOfEvent("tau+:LFV_lll",path=my_path)
roe_path = basf2.Path()
deadEndPath = basf2.Path()
ma.signalSideParticleFilter('tau+:LFV_lll', '', roe_path, deadEndPath)
ma.fillSignalSideParticleList('mu+:sig', 'tau+:LFV_lll -> ^mu+:all ^mu+:all ^mu-:all', roe_path)
ma.rankByHighest(particleList="mu+:sig", variable="p", outputVariable="p_rank",path=roe_path)
ma.cutAndCopyList("mu+:sig_primary", "mu+:sig", cut="extraInfo(p_rank)==1", path=roe_path)
ma.variableToSignalSideExtraInfo('mu+:sig_primary', {'muonID': 'muonID_primary_daughter'}, path=roe_path)
my_path.for_each('RestOfEvent', 'RestOfEvents', roe_path)
ma.applyCuts('tau+:LFV_lll', 'extraInfo(muonID_primary_daughter) > 0.1', my_path)

# event cut
ma.applyEventCuts("nParticlesInList(tau+:LFV_lll) > 0", path=my_path)
             
# print mdst (not udst!)
mdst.add_mdst_output(my_path, mc=True, filename=output_file, additionalBranches=[], dataDescription=None)

# progress
basf2.process(my_path)

# Print call statistics
print(basf2.statistics)
