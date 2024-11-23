#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# usage: basf2 MakeNtuple_multi.py "./20210402/evt-0.mdst"
# last: 2024-11-21-00

import os
import sys
from itertools import product

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

def GetROEVariables(maskname):
    var_names = ["roeE", "roeM", "roeP", "roeMbc", "roeDeltae"] + \
                ["nROE_Charged", "nROE_Photons", "nROE_NeutralHadrons"] + \
                ["roeEextra"] + \
                ["nROE_RemainingTracks", "nROE_ECLClusters"] + \
                ["roeCharge"]
    
    def format_variable(var_name):
        if '(' in var_name and ')' in var_name:
            prefix, suffix = var_name.split('(', 1)
            return f"{prefix}({suffix.strip(')')}, {maskname})"
        else:
            return f"{var_name}({maskname})"

    return [format_variable(var_name) for var_name in var_names]

def GetContinuumSuppressionVariables(maskname):
    var_names = ["R2", "thrustBm", "thrustOm", "cosTBTO", "cosTBz"] + \
                ["KSFWVariables(et)", "KSFWVariables(mm2)"] + \
                ["KSFWVariables(hso00)", "KSFWVariables(hso01)", "KSFWVariables(hso02)", "KSFWVariables(hso03)", "KSFWVariables(hso04)"] + \
                ["KSFWVariables(hso10)", "KSFWVariables(hso12)", "KSFWVariables(hso14)"] + \
                ["KSFWVariables(hso20)", "KSFWVariables(hso22)", "KSFWVariables(hso24)"] + \
                ["KSFWVariables(hoo0)", "KSFWVariables(hoo1)", "KSFWVariables(hoo2)", "KSFWVariables(hoo3)", "KSFWVariables(hoo4)"] + \
                ["CleoConeCS(1)", "CleoConeCS(2)", "CleoConeCS(3)", "CleoConeCS(4)", "CleoConeCS(5)", "CleoConeCS(6)", "CleoConeCS(7)", "CleoConeCS(8)", "CleoConeCS(9)"]

    def format_variable(var_name):
        if '(' in var_name and ')' in var_name:
            prefix, suffix = var_name.split('(', 1)
            return f"{prefix}({suffix.strip(')')}, {maskname})"
        else:
            return f"{var_name}({maskname})"

    return [format_variable(var_name) for var_name in var_names]

def FillSeveralPhotons(path):
    list_names = []
    
    base_cut = '[inCDCAcceptance]'

    energy_cuts = ['[[clusterReg==1 and E>0.10] or [clusterReg==2 and E>0.06] or [clusterReg==3 and E>0.15]]', \
                   '[E>0.2]']

    distance_cuts = ['[True]', \
                     '[minC2TDist > 50]']

    timing_cuts = ['[True]', \
                   '[abs(clusterTiming) < 200]', \
                   '[[abs(clusterTiming) < 200] and [abs(clusterTiming/clusterErrorTiming)<2.0]]']

    combinations = [
        f"{base_cut} and {energy} and {distance} and {timing}"
        for energy, distance, timing in product(energy_cuts, distance_cuts, timing_cuts)
    ]

    for index, photon_cut in enumerate(combinations):
        ma.fillParticleList("gamma:cut_v" + str(index), cut=photon_cut, path=path)
        list_names.append("gamma:cut_v" + str(index))

    return list_names


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

# treefit
vertex.treeFit(list_name="tau+:LFV_lll", conf_level=-1, updateAllDaughters=False, path=my_path)

# mask for ROE
track_selection = "[inCDCAcceptance] and [pt > 0.075] and [dr < 1] and [abs(dz) < 3]"
cluster_selection = '[inCDCAcceptance] and [E > 0.2]'

# --- build ROE ---
cleanMask = ("cleanMask", track_selection, cluster_selection)
ma.buildRestOfEvent("tau+:LFV_lll", fillWithMostLikely=True, path=my_path)
ma.appendROEMasks("tau+:LFV_lll", [cleanMask], path=my_path)

# --- build Continuum suppression ---
ma.buildContinuumSuppression(list_name="tau+:LFV_lll", roe_mask = "cleanMask", path=my_path)

# file particles for Event shape and kinematics
ma.fillParticleList("pi+:evtshape_kinematics", cut='[dr < 1] and [abs(dz) < 3]', path=my_path)
ma.fillParticleList("gamma:case_base", cut='[inCDCAcceptance] and [clusterNHits > 1.5]', path=my_path)
ma.reconstructDecay(decayString="pi0:case_1 -> gamma:case_base gamma:case_base", cut="0.115 < M < 0.152", path=my_path)
ma.cutAndCopyList("gamma:case_1", "gamma:case_base", cut="[isDaughterOfList(pi0:case_1)] and [E>0.1]", path=my_path)
ma.cutAndCopyList("gamma:case_2", "gamma:case_base", cut="[E>0.2]", path=my_path)
ma.copyLists(outputListName="gamma:evtshape_kinematics", inputListNames=["gamma:case_1", "gamma:case_2", path=my_path)

# --- build Event Kinematics ---
ma.buildEventKinematics(inputListNames = ["pi+:evtshape_kinematics", "gamma:evtshape_kinematics"], path=my_path)
    
# --- build Event shape ---
ma.buildEventShape(inputListNames=['pi+:evtshape_kinematics', 'gamma:evtshape_kinematics'], path=my_path)

# fill photons with several cuts
photon_names = FillSeveralPhotons(my_path)

# true match
ma.matchMCTruth("tau+:LFV_lll", path=my_path)

# define vars
sig_vars = ["daughter(0,muonID)", "daughter(1,muonID)", "daughter(2,muonID)"] + \
           ["daughter(0,electronID)", "daughter(1,electronID)", "daughter(2,electronID)"] + \
           ["Mbc", "deltaE"] + \
           ["E", "px", "py", "pz", "pt", "theta", "phi"] + \
           ["useCMSFrame(E)", "useCMSFrame(px)", "useCMSFrame(py)", "useCMSFrame(pz)", "useCMSFrame(pt)", "useCMSFrame(theta)", "useCMSFrame(phi)"] + \
           ["daughter(0, E)", "daughter(0, px)", "daughter(0, py)", "daughter(0, pz)", "daughter(0, pt)", "daughter(0, theta)", "daughter(0, phi)"] + \
           ["daughter(1, E)", "daughter(1, px)", "daughter(1, py)", "daughter(1, pz)", "daughter(1, pt)", "daughter(1, theta)", "daughter(1, phi)"] + \
           ["daughter(2, E)", "daughter(2, px)", "daughter(2, py)", "daughter(2, pz)", "daughter(2, pt)", "daughter(2, theta)", "daughter(2, phi)"] + \
           ["daughter(0, useCMSFrame(E))", "daughter(0, useCMSFrame(px))", "daughter(0, useCMSFrame(py))", "daughter(0, useCMSFrame(pz))", \
            "daughter(0, useCMSFrame(pt))", "daughter(0, useCMSFrame(theta))", "daughter(0, useCMSFrame(phi))"] + \
           ["daughter(1, useCMSFrame(E))", "daughter(1, useCMSFrame(px))", "daughter(1, useCMSFrame(py))", "daughter(1, useCMSFrame(pz))", \
            "daughter(1, useCMSFrame(pt))", "daughter(1, useCMSFrame(theta))", "daughter(1, useCMSFrame(phi))"] + \
           ["daughter(2, useCMSFrame(E))", "daughter(2, useCMSFrame(px))", "daughter(2, useCMSFrame(py))", "daughter(2, useCMSFrame(pz))", \
            "daughter(2, useCMSFrame(pt))", "daughter(2, useCMSFrame(theta))", "daughter(2, useCMSFrame(phi))"] + \
           ["extraInfo(decayModeID)"] + \
           GetContinuumSuppressionVariables("cleanMask") + \
           ["cosAngleBetweenMomentumAndVertexVector", "charge", \
            "flightTime", "flightTimeErr", "chiProb", "angleToClosestInList(pi+:evtshape_kinematics)"]
tag_vars = vc.recoil_kinematics + GetROEVariables("cleanMask") + vc.track + vc.vertex 
event_vars = ["beamE"] + vc.event_shape + vc.event_kinematics + ["eventExtraInfo(EventCode)"] + ["nParticlesInList(pi+:evtshape_kinematics)", "nParticlesInList(gamma:evtshape_kinematics)"] + \
             ["totalEnergyOfParticlesInList(gamma:evtshape_kinematics)"]
MC_vars = ["isSignal", "isSignalAcceptMissingNeutrino"]

# add variables about other photon candidates
for photon_name in photon_names:
    event_vars.append("nParticlesInList(" + photon_name + ")")
    event_vars.append("totalEnergyOfParticlesInList(" + photon_name + ")")

# Ntuple output
ma.variablesToNtuple(decayString="tau+:LFV_lll" ,variables=sig_vars,filename=output_file,treename="tau_lfv",path=my_path)
ma.variablesToNtuple(decayString="tau+:LFV_lll",variables=tag_vars,filename=output_file,treename="tau_lfv",path=my_path)
ma.variablesToNtuple(decayString="tau+:LFV_lll",variables=event_vars,filename=output_file,treename="tau_lfv",path=my_path)
ma.variablesToNtuple(decayString="tau+:LFV_lll",variables=MC_vars,filename=output_file,treename="tau_lfv",path=my_path)

# hashmap
my_path.add_module('ParticleMCDecayString', listName='tau+:LFV_lll', fileName=hashmapName)
             
# progress
basf2.process(my_path)

# Print call statistics
print(basf2.statistics)
