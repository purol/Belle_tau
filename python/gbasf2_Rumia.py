#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# usage: basf2 MakeNtuple_multi.py "./20210402/evt-0.mdst"
# last: 2024-11-21-00

import os
import sys

import basf2
import modularAnalysis as ma
import variables as va
import variables.collections as vc
import variables.utils as vu
import vertex

import stdPi0s
import stdV0s
import stdPhotons

from glob import glob

import argparse

# get data type
parser = argparse.ArgumentParser(description='Sample type')

# data or MC
parser.add_argument('--IsItMC', required=True, type=bool, help='Specify it is MC or data')
parser.add_argument('--type', required=True, type=str, help='type of sample. list) ')

args = parser.parse_args()

basf2.conditions.prepend_globaltag(ma.getAnalysisGlobaltag())

# set random seed
basf2.set_random_seed(42)

# names
hashmapName = "hashmap_Upsilon.root"
output_file = "Ntuple.root"
    
# Read uDST
my_path = basf2.create_path()
    
inputfile="/group/belle2/dataprod/MC/SkimTraining/mixed_BGx1.mdst_000001_prod00009434_task10020000001.root"
ma.inputMdst(environmentType='default',filename=inputfile,path=my_path)

# merge particle lists
ma.copyLists(outputListName="tau+:LFV_lll", inputListNames=["tau+:LFV_lll0", "tau+:LFV_lll1", "tau+:LFV_lll2", "tau+:LFV_lll3", "tau+:LFV_lll4", "tau+:LFV_lll5", path=my_path)

# file particles
stdV0s.stdKshorts(path=my_path)
ma.cutAndCopyList("K_S0:myKaonshort", "K_S0:merged", cut="[0.4876 < M < 0.5076] and significanceOfDistance > 50", path=my_path)
    
stdPi0s.stdPi0s(listtype="eff30_May2020",path=my_path)
ma.cutAndCopyList("pi0:myneutralPion", "pi0:eff30_May2020", cut="p > 0.4", path=my_path)

# treefit
vertex.treeFit(list_name="tau+:LFV_lll", conf_level=-1, updateAllDaughters=False, path=my_path)

# mask definition
track_selection = "[dr < 1] and [abs(dz) < 3]"
cluster_selection_v200 = '[[clusterReg==1 and E>0.10] or \
                            [clusterReg==2 and E>0.06] or \
                            [clusterReg==3 and E>0.15]] and [minC2TDist > 50] and [inCDCAcceptance]'

# --- build ROE ---
cleanMask = ("cleanMask", track_selection, cluster_selection_v200)
ma.buildRestOfEvent("tau+:LFV_lll", path=my_path)
ma.appendROEMasks("tau+:LFV_lll", [cleanMask], path=my_path)

# --- build Continuum suppression ---
for ch in range(6):
    ma.buildContinuumSuppression(list_name="tau+:LFV_lll" + str(ch),roe_mask = "cleanMask", path=my_path)

# --- build Event Kinematics ---
track_default = 'pt > 0.1 and thetaInCDCAcceptance and abs(dz) < 3 and dr < 0.5'
gamma_default = 'E > 0.05 and thetaInCDCAcceptance and abs(clusterTiming) < 200'
ma.buildEventKinematics(custom_cuts=(track_default, gamma_default), path=my_path)
    
# --- build Event shape ---
ma.fillParticleList('pi+:evtshape_default', track_default, path=my_path)
ma.fillParticleList('gamma:evtshape_default', gamma_default, path=my_path)
ma.buildEventShape(inputListNames=['pi+:evtshape_default', 'gamma:evtshape_default'], path=my_path)

# define vars
sig_vars = ["daughter(0,muonID)", "daughter(1,muonID)", "daughter(2,muonID)"] + \
           ["daughter(0,electronID)", "daughter(1,electronID)", "daughter(2,electronID)"] + \
           ["Mbc", "deltaE"] + \
           ["E", "px", "py", "pz"] + \
           ["useCMSFrame(E)", "useCMSFrame(px)", "useCMSFrame(py)", "useCMSFrame(pz)"] + \
           ["daughter(0, E)", "daughter(0, px)", "daughter(0, py)", "daughter(0, pz)"] + \
           ["daughter(1, E)", "daughter(1, px)", "daughter(1, py)", "daughter(1, pz)"] + \
           ["daughter(2, E)", "daughter(2, px)", "daughter(2, py)", "daughter(2, pz)"] + \
           ["daughter(0, useCMSFrame(E))", "daughter(0, useCMSFrame(px))", "daughter(0, useCMSFrame(py))", "daughter(0, useCMSFrame(pz))"] + \
           ["daughter(1, useCMSFrame(E))", "daughter(1, useCMSFrame(px))", "daughter(1, useCMSFrame(py))", "daughter(1, useCMSFrame(pz))"] + \
           ["daughter(2, useCMSFrame(E))", "daughter(2, useCMSFrame(px))", "daughter(2, useCMSFrame(py))", "daughter(2, useCMSFrame(pz))"] + \
           ["R2", "thrustBm", "thrustOm", "cosTBTO", "cosTBz"] + \
           ["KSFWVariables(et)", "KSFWVariables(mm2)"] + \
           ["KSFWVariables(hso00)", "KSFWVariables(hso01)", "KSFWVariables(hso02)", "KSFWVariables(hso03)", "KSFWVariables(hso04)"] + \
           ["KSFWVariables(hso10)", "KSFWVariables(hso12)", "KSFWVariables(hso14)"] + \
           ["KSFWVariables(hso20)", "KSFWVariables(hso22)", "KSFWVariables(hso24)"] + \
           ["KSFWVariables(hoo0)", "KSFWVariables(hoo1)", "KSFWVariables(hoo2)", "KSFWVariables(hoo3)", "KSFWVariables(hoo4)"] + \
           ["CleoConeCS(1)", "CleoConeCS(2)", "CleoConeCS(3)", "CleoConeCS(4)", "CleoConeCS(5)", "CleoConeCS(6)", "CleoConeCS(7)", "CleoConeCS(8)", "CleoConeCS(9)"] + \
           ["extraInfo(decayModeID)"]
tag_vars = vc.recoil_kinematics + vc.roe_kinematics + vc.track + vc.vertex 
event_vars = ["beamE"] + vc.event_shape + vc.event_kinematics + ["eventExtraInfo(EventCode)"]

# Ntuple output
ma.variablesToNtuple(decayString="tau+:LFV_lll" ,variables=sig_vars,filename=output_file,treename="tau_lfv",path=my_path)
ma.variablesToNtuple(decayString="tau+:LFV_lll",variables=tag_vars,filename=output_file,treename="tau_lfv",path=my_path)
ma.variablesToNtuple(decayString="tau+:LFV_lll",variables=event_vars,filename=output_file,treename="tau_lfv",path=my_path)

# hashmap
my_path.add_module('ParticleMCDecayString', listName='tau+:LFV_lll', fileName=hashmapName)
             
# progress
basf2.process(my_path)

# Print call statistics
print(basf2.statistics)
