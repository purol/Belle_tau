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
from variables import variables as vm
import vertex

import stdPi0s
import stdV0s
import stdPhotons

import pdg

from glob import glob

import argparse
import re

def convert_name(name: str) -> str:
    # Step 1: replace "+:" or "-:" with "_"
    name = re.sub(r'[+-]:', '_', name)

    # Step 2: replace "(" and "," with "_"
    name = name.replace("(", "_").replace(",", "_")

    # Step 3: remove ")"
    name = name.replace(")", "")

    # Step 4: remove spaces around underscores (optional cleanup)
    name = re.sub(r'\s*_\s*', '_', name)

    return name

BELLEONE = False

def AssignIndex(sample_name, event_name, energy_name):
    sample_index = -100
    type_index = -100
    energy_index = -100

    if(sample_name=="data"):
        sample_index = -1
    elif(sample_name=="MC15ri"):
        sample_index = 1
    elif(sample_name=="MC15rd"):
        sample_index = 2
    elif(sample_name=="MC16ri"):
        sample_index = 3
    elif(sample_name=="MC16rd"):
        sample_index = 4
    elif(sample_name=="Belle_data"):
        sample_index = 5
    elif(sample_name=="Belle_MC"):
        sample_index = 6
    else:
        print("enter the proper sample type")
        exit(1)

    if(event_name=="data"):
        type_index = -1
    elif(event_name=="signal"):
        type_index = 0
    elif(event_name=="charged"):
        type_index = 1
    elif(event_name=="mixed"):
        type_index = 2
    elif(event_name=="uubar"):
        type_index = 3
    elif(event_name=="ddbar"):
        type_index = 4
    elif(event_name=="ssbar"):
        type_index = 5
    elif(event_name=="ccbar"):
        type_index = 6
    elif(event_name=="mumu"):
        type_index = 7
    elif(event_name=="ee"):
        type_index = 8
    elif(event_name=="eeee"):
        type_index = 9
    elif(event_name=="eemumu"):
        type_index = 10
    elif(event_name=="eepipi"):
        type_index = 11
    elif(event_name=="eeKK"):
        type_index = 12
    elif(event_name=="eepp"):
        type_index = 13
    elif(event_name=="pipiISR"):
        type_index = 14
    elif(event_name=="KKISR"):
        type_index = 15
    elif(event_name=="gg"):
        type_index = 16
    elif(event_name=="eetautau"):
        type_index = 17
    elif(event_name=="K0K0barISR"):
        type_index = 18
    elif(event_name=="mumumumu"):
        type_index = 19
    elif(event_name=="mumutautau"):
        type_index = 20
    elif(event_name=="tautautautau"):
        type_index = 21
    elif(event_name=="taupair"):
        type_index = 22
    elif(event_name=="pipipi0ISR"):
        type_index = 23
    elif(event_name=="BBs"):
        type_index = 24
    elif(event_name=="BsBs"):
        type_index = 25
    elif(event_name=="uds"):
        type_index = 25
    elif(event_name=="bsbs"):
        type_index = 25
    elif(event_name=="nonbsbs"):
        type_index = 25
    elif(event_name=="eecc"):
        type_index = 25
    elif(event_name=="eess"):
        type_index = 25
    elif(event_name=="eeuu"):
        type_index = 25
    else:
        print("enter the proper event type")
        exit(1)

    if(energy_name=="4S"):
        energy_index = 1
    elif(energy_name=="off"):
        energy_index = 2
    elif(energy_name=="10657"):
        energy_index = 3
    elif(energy_name=="10706"):
        energy_index = 4
    elif(energy_name=="10751"):
        energy_index = 5
    elif(energy_name=="10810"):
        energy_index = 6

    return sample_index, type_index, energy_index

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

def BasicAnalysisForTau(tau_list, sample_index, type_index, energy_index, path):
    # treefit
    vertex.treeFit(list_name=tau_list, conf_level=-1, updateAllDaughters=False, path=path)

    # mask for ROE
    track_selection = "[inCDCAcceptance] and [pt > 0.075] and [dr < 1] and [abs(dz) < 3]"
    cluster_selection = '[inCDCAcceptance] and [E > 0.2]'

    # --- build ROE ---
    cleanMask = ("cleanMask", track_selection, cluster_selection)
    ma.buildRestOfEvent(tau_list, fillWithMostLikely=True, path=path)
    ma.appendROEMasks(tau_list, [cleanMask], path=path)

    # --- build Continuum suppression ---
    ma.buildContinuumSuppression(list_name=tau_list, roe_mask = "cleanMask", path=path)

    # true match
    ma.matchMCTruth(tau_list, path=path)

    # add event type / sample type
    vm.addAlias("MySampleType", "constant(" + str(sample_index) + ")")
    vm.addAlias("MyEventType", "constant(" + str(type_index) + ")")
    vm.addAlias("MyEnergyType", "constant(" + str(energy_index) + ")")

def DefineVariables(tau_list, photon_names, IsItPrompt, path):
    # define vars
    sig_vars = ["Mbc", "deltaE"] + \
               ["M", "E", "px", "py", "pz", "pt", "theta", "phi"] + \
               ["useCMSFrame(E)", "useCMSFrame(px)", "useCMSFrame(py)", "useCMSFrame(pz)", "useCMSFrame(pt)", "useCMSFrame(theta)", "useCMSFrame(phi)"] + \
               ["extraInfo(decayModeID)"] + \
               GetContinuumSuppressionVariables("cleanMask") + \
               ["cosAngleBetweenMomentumAndVertexVector", "charge", \
                "flightTime", "flightTimeErr", "angleToClosestInList(pi+:evtshape_kinematics)"] + \
               vc.vertex
    if(IsItPrompt):
        roe_path_prompt = basf2.Path()
        deadEndPath_prompt = basf2.Path()
        ma.signalSideParticleFilter(tau_list, '', roe_path_prompt, deadEndPath_prompt)
        ma.fillSignalSideParticleList(tau_list + "_ROE", "^" + tau_list, roe_path_prompt)

        variable_list = ["dr", "dz"] + \
                        ["muonID", "muonID_noSVD", "electronID", "pionID", "pionIDNN"] + \
                        ["charge"] + \
                        ["minET2ETIsoScoreAsWeightedAvg(mu+:taulfv, 0, ECL, KLM)"] + \
                        ["E", "p", "px", "py", "pz", "pt"] + \
                        ["theta", "phi"] + \
                        ["useCMSFrame(E)", "useCMSFrame(px)", "useCMSFrame(py)", "useCMSFrame(pz)", "useCMSFrame(pt)"] + \
                        ["useCMSFrame(theta)", "useCMSFrame(phi)"] + \
                        ["cosToThrustOfEvent"]
        variable_list_converted = [convert_name(var) for var in variable_list]

        variable_list_one_muon = ["daughter(0," + var + ")" for var in variable_list]
        variable_list_two_muon = ["daughter(1," + var + ")" for var in variable_list]
        variable_list_three_muon = ["daughter(2," + var + ")" for var in variable_list]

        variable_list_converted_one_muon = ["OneMuon_" + var for var in variable_list_converted]
        variable_list_converted_two_muon = ["TwoMuon_" + var for var in variable_list_converted]
        variable_list_converted_three_muon = ["ThreeMuon_" + var for var in variable_list_converted]
        
        ma.variableToSignalSideExtraInfo(tau_list + "_ROE", dict(zip(variable_list_one_muon, variable_list_converted_one_muon)), path=roe_path_prompt)
        ma.variableToSignalSideExtraInfo(tau_list + "_ROE", dict(zip(variable_list_two_muon, variable_list_converted_two_muon)), path=roe_path_prompt)
        ma.variableToSignalSideExtraInfo(tau_list + "_ROE", dict(zip(variable_list_three_muon, variable_list_converted_three_muon)), path=roe_path_prompt)

        variable_list = ["E", "px", "py", "pz", "pt"] + \
                        ["theta", "phi"] + \
                        ["M"] + \
                        ["x", "y", "z", "dx", "dy", "dz"] + \
                        ["flightTime", "flightTimeErr", "distance", "significanceOfDistance"] + \
                        ["chiProb"]
        variable_list_converted = [convert_name(var) for var in variable_list]

        variable_list_ALP = ["constant(-1)" for var in variable_list]

        variable_list_converted_ALP = ["ALP_" + var for var in variable_list_converted]

        for original_variable, converted_name in zip(variable_list_ALP, variable_list_converted_ALP):
            ma.variableToSignalSideExtraInfo(tau_list + "_ROE", {original_variable: converted_name}, path=roe_path_prompt)

        path.for_each('RestOfEvent', 'RestOfEvents', roe_path_prompt)

        daughter_vars = ["extraInfo(" + var + ")" for var in variable_list_converted_one_muon + variable_list_converted_two_muon + variable_list_converted_three_muon + variable_list_converted_ALP]
    else:
        roe_path_ALP = basf2.Path()
        deadEndPath_ALP = basf2.Path()
        ma.signalSideParticleFilter(tau_list, '', roe_path_ALP, deadEndPath_ALP)
        ma.fillSignalSideParticleList(tau_list + "_ROE", "^" + tau_list, roe_path_ALP)

        variable_list = ["dr", "dz"] + \
                        ["muonID", "muonID_noSVD", "electronID", "pionID", "pionIDNN"] + \
                        ["charge"] + \
                        ["minET2ETIsoScoreAsWeightedAvg(mu+:taulfv, 0, ECL, KLM)"] + \
                        ["E", "p", "px", "py", "pz", "pt"] + \
                        ["theta", "phi"] + \
                        ["useCMSFrame(E)", "useCMSFrame(px)", "useCMSFrame(py)", "useCMSFrame(pz)", "useCMSFrame(pt)"] + \
                        ["useCMSFrame(theta)", "useCMSFrame(phi)"] + \
                        ["cosToThrustOfEvent"]
        variable_list_converted = [convert_name(var) for var in variable_list]

        variable_list_one_muon = ["daughter(0, daughter(0, " + var + "))" for var in variable_list]
        variable_list_two_muon = ["daughter(0, daughter(1, " + var + "))" for var in variable_list]
        variable_list_three_muon = ["daughter(1," + var + ")" for var in variable_list]

        variable_list_converted_one_muon = ["OneMuon_" + var for var in variable_list_converted]
        variable_list_converted_two_muon = ["TwoMuon_" + var for var in variable_list_converted]
        variable_list_converted_three_muon = ["ThreeMuon_" + var for var in variable_list_converted]

        ma.variableToSignalSideExtraInfo(tau_list + "_ROE", dict(zip(variable_list_one_muon, variable_list_converted_one_muon)), path=roe_path_ALP)
        ma.variableToSignalSideExtraInfo(tau_list + "_ROE", dict(zip(variable_list_two_muon, variable_list_converted_two_muon)), path=roe_path_ALP)
        ma.variableToSignalSideExtraInfo(tau_list + "_ROE", dict(zip(variable_list_three_muon, variable_list_converted_three_muon)), path=roe_path_ALP)

        variable_list = ["E", "px", "py", "pz", "pt"] + \
                        ["theta", "phi"] + \
                        ["M"] + \
                        ["x", "y", "z", "dx", "dy", "dz"] + \
                        ["flightTime", "flightTimeErr", "distance", "significanceOfDistance"] + \
                        ["chiProb"]
        variable_list_converted = [convert_name(var) for var in variable_list]

        variable_list_ALP = ["daughter(0, " + var + ")" for var in variable_list]

        variable_list_converted_ALP = ["ALP_" + var for var in variable_list_converted]

        for original_variable, converted_name in zip(variable_list_ALP, variable_list_converted_ALP):
            ma.variableToSignalSideExtraInfo(tau_list + "_ROE", {original_variable: converted_name}, path=roe_path_ALP)

        path.for_each('RestOfEvent', 'RestOfEvents', roe_path_ALP)

        daughter_vars = ["extraInfo(" + var + ")" for var in variable_list_converted_one_muon + variable_list_converted_two_muon + variable_list_converted_three_muon + variable_list_converted_ALP]
    if BELLEONE:
        daughter_vars = [
            v.replace("muonID", "muIDBelle")
             .replace("electronID", "eIDBelle")
             .replace("pionID", "atcPIDBelle(2,3)")
            for v in daughter_vars if ("pionIDNN" not in v) and ("minET2ETIsoScoreAsWeightedAvg" not in v)
        ]
    else:
        pass
    tag_vars = vc.recoil_kinematics + GetROEVariables("cleanMask")
    event_vars = ["beamE"] + vc.event_shape + vc.event_kinematics + ["cosToThrustOfEvent"] + ["eventExtraInfo(EventCode)"] + ["nParticlesInList(pi+:evtshape_kinematics)", "nParticlesInList(gamma:evtshape_kinematics)"] + \
                 ["totalEnergyOfParticlesInList(gamma:evtshape_kinematics)"] + \
                 ["MySampleType", "MyEventType", "MyEnergyType"]
    triggers = ["L1FTDL(fff)", "L1FTDL(ffo)", "L1FTDL(fyo)", "L1FTDL(ffy)", "L1FTDL(stt)", "L1FTDL(hie)", "L1FTDL(lml0)", "L1FTDL(lml1)", "L1FTDL(lml2)", "L1FTDL(lml3)", \
                "L1FTDL(lml4)", "L1FTDL(lml5)", "L1FTDL(lml6)", "L1FTDL(lml7)", "L1FTDL(lml8)", "L1FTDL(lml9)", "L1FTDL(lml10)", "L1FTDL(lml11)", "L1FTDL(lml12)", "L1FTDL(lml13)", \
                "L1PSNM(fff)", "L1PSNM(ffo)", "L1PSNM(fyo)", "L1PSNM(ffy)", "L1PSNM(stt)", "L1PSNM(hie)", "L1PSNM(lml0)", "L1PSNM(lml1)", "L1PSNM(lml2)", "L1PSNM(lml3)", \
                "L1PSNM(lml4)", "L1PSNM(lml5)", "L1PSNM(lml6)", "L1PSNM(lml7)", "L1PSNM(lml8)", "L1PSNM(lml9)", "L1PSNM(lml10)", "L1PSNM(lml11)", "L1PSNM(lml12)", "L1PSNM(lml13)"]
    MC_vars = ["isSignal", "isSignalAcceptMissingNeutrino"] + ['extraInfo(DecayHash)', 'extraInfo(DecayHashExtended)'] + ['tauPlusMCMode', 'tauMinusMCMode']

    # add variables about other photon candidates
    for photon_name in photon_names:
        event_vars.append("nParticlesInList(" + photon_name + ")")
        event_vars.append("totalEnergyOfParticlesInList(" + photon_name + ")")

    # return the list of variables
    return sig_vars + daughter_vars + tag_vars + event_vars + triggers + MC_vars

def MakeNtupleandHashmap(tau_list, variable_list, Ntuple_name, hashmap_name, path):
    # names
    output_file = Ntuple_name + ".root"
    hashmapName = hashmap_name + ".root"

    # hashmap
    path.add_module('ParticleMCDecayString', listName=tau_list, fileName=hashmapName)

    # Ntuple output
    ma.variablesToNtuple(decayString=tau_list ,variables=variable_list, filename=output_file, treename="tau_lfv", path=path)

# get data type
parser = argparse.ArgumentParser(description='Sample type')

# data or MC
parser.add_argument('--sample', required=True, type=str, help='type of sample. list) data, MC15ri, MC15rd, MC16ri, MC16rd, Belle_data, Belle_MC')
parser.add_argument('--type', required=True, type=str, help='type of event. list) data, signal, charged, mixed, uubar, ddbar, ssbar, ccbar, mumu, ee, eeee, eemumu, eepipi, eeKK, eepp, pipiISR, KKISR, gg, eetautau, K0K0barISR, mumumumu, mumutautau, tautautautau, taupair, pipipi0ISR, BBs, BsBs, uds, bsbs, nonbsbs, eecc, eess, eeuu')
parser.add_argument('--energy', required=True, type=str, help='energy of sample. list) 4S, off, 10657, 10706, 10751, 10810')
parser.add_argument('--prompt', action='store_true', help='If you want to reconstruct the prompt tau-> mu mu mu, use this flag')
parser.add_argument('--vertex', action='store_true', help='If you want to reconstruct the tau->(alpha -> mu mu) mu, use this flag')
parser.add_argument('--control', action='store_true', help='If you want to reconstruct the tau-> pi pi pi, use this flag')

# input file/output path when it is running on KEKCC
parser.add_argument('--KEKCC', action='store_true', help='run on KEKCC, not GRID')
parser.add_argument('--inputfile', required=False, type=str, help='destination of output files for local run. `--KEKCC` should be on. ex) /home/belle2/junewoo/storage_b1/input_Jpsi/B02XJpsi_SKIM_00012_job227326565_00.udst.root')
parser.add_argument('--destination', required=False, type=str, help='destination of output files for local run. `--KEKCC` should be on. ex) /home/belle2/junewoo/storage_b1/output')

# momentum scale for data
parser.add_argument('--momentum_scale', default='center', type=str, help='Momentum scale for data. This option is ignored if it is not Belle II data. list) center, up, down')

args = parser.parse_args()

# set BELLEONE flag if it is Belle_data or Belle_MC
if "Belle_" in args.sample:
    BELLEONE = True
else:
    BELLEONE = False

# assign sample index and type index
sample_index, type_index, energy_index = AssignIndex(sample_name=args.sample, event_name=args.type, energy_name=args.energy)

if BELLEONE:
    # check https://questions.belle2.org/question/14927/global-tag-use-for-flavor-tagging-for-light-2305-korat/
    basf2.conditions.prepend_globaltag("analysis_tools_light-2012-minos")
else:
    basf2.conditions.prepend_globaltag(ma.getAnalysisGlobaltag())
    basf2.conditions.prepend_globaltag("pid_nn_release08_v1")

# set random seed
basf2.set_random_seed(42)
    
# Read uDST
my_path = basf2.create_path()

inputfile = ""
basename = ""
name = ""
if(args.KEKCC):
    inputfile = args.inputfile
    if not inputfile.endswith(".root"): sys.exit(2)
    else:
        basename = os.path.basename(inputfile)
        name = os.path.splitext(basename)[0]
else:
    # just any file
    inputfile="/group/belle2/dataprod/MC/SkimTraining/mixed_BGx1.mdst_000001_prod00009434_task10020000001.root"

if BELLEONE:
    ma.inputMdst(environmentType='Belle',filename=inputfile,path=my_path)
else:
    ma.inputMdst(environmentType='default',filename=inputfile,path=my_path)

# track momentum scale
if BELLEONE:
    pass
else:
    if(args.type == "data"):
        basf2.conditions.prepend_globaltag('tracking_data_Moriond23_v1')
        if(args.momentum_scale == "center"):
            ma.scaleTrackMomenta(inputListNames=['e-:all', 'mu-:all', 'pi+:all', 'K+:all', 'p+:all'], payloadName='tracking_momentumScaleFactor_global', scalingFactorName='sf_global_central', path=my_path)
        elif(args.momentum_scale == "up"):
            ma.scaleTrackMomenta(inputListNames=['e-:all', 'mu-:all', 'pi+:all', 'K+:all', 'p+:all'], payloadName='tracking_momentumScaleFactor_global', scalingFactorName='sf_global_up', path=my_path)
        elif(args.momentum_scale == "down"):
            ma.scaleTrackMomenta(inputListNames=['e-:all', 'mu-:all', 'pi+:all', 'K+:all', 'p+:all'], payloadName='tracking_momentumScaleFactor_global', scalingFactorName='sf_global_down', path=my_path)
        else:
            print("unexpected momentum scale option")
            exit(1)

# set output file and path
outputpath = args.destination
output_file = "Ntuple"
hashmapName = "hashmap_tau"
if(args.KEKCC):
    output_file = outputpath + "/" + name + "_" + output_file
    hashmapName = outputpath + "/" + name + "_" + hashmapName
else:
    output_file = output_file
    hashmapName = hashmapName

# get MC decay mode
ma.labelTauPairMC(path=my_path)

# fill photon for Event shape and kinematics
ma.fillParticleList("pi+:evtshape_kinematics", cut='[dr < 1] and [abs(dz) < 3]', path=my_path)
ma.fillParticleList("gamma:case_base", cut='[inCDCAcceptance] and [clusterNHits > 1.5]', path=my_path)
ma.reconstructDecay(decayString="pi0:case_1 -> gamma:case_base gamma:case_base", cut="0.115 < M < 0.152", path=my_path)
ma.cutAndCopyList("gamma:case_1", "gamma:case_base", cut="[isDescendantOfList(pi0:case_1, 1)] and [E>0.1]", path=my_path)
ma.cutAndCopyList("gamma:case_2", "gamma:case_base", cut="[E>0.2]", path=my_path)
ma.copyLists(outputListName="gamma:evtshape_kinematics", inputListNames=["gamma:case_1", "gamma:case_2"], path=my_path)

# fill photon for brem correction
ma.cutAndCopyList("gamma:for_brem", "gamma:case_base", cut="[isDescendantOfList(pi0:case_1, 1) == 0] and [E>0.02]", path=my_path)

# fill particles for tau reconstruction
trackCuts = "[-3.0 < dz < 3.0] and [dr < 1.0]"
ma.fillParticleList(decayString="e+:taulfv", cut=trackCuts, path=my_path)
ma.correctBremsBelle('e+:taulfv_brem', 'e+:taulfv', 'gamma:for_brem', multiplePhotons=True, angleThreshold=0.15, path=my_path)
ma.fillParticleList(decayString="mu+:taulfv", cut=trackCuts, path=my_path)
ma.fillParticleList(decayString="pi+:taulfv", cut=trackCuts, path=my_path)

# fill photons with several cuts
photon_names = FillSeveralPhotons(my_path)

# calculate isolation
if BELLEONE:
    pass
else:
    my_path.add_module("TrackIsoCalculator", decayString="mu+:taulfv", particleListReference="mu+:taulfv", detectorNames=["ECL", "KLM"], useHighestProbMassForExt=False)

# --- build Event Kinematics ---
ma.buildEventKinematics(inputListNames = ["pi+:evtshape_kinematics", "gamma:evtshape_kinematics"], path=my_path)
    
# --- build Event shape ---
ma.buildEventShape(inputListNames=['pi+:evtshape_kinematics', 'gamma:evtshape_kinematics'], path=my_path)

# define default cut criteria
tauLFVCuts3 = "nParticlesInList(pi+:taulfv) < 7 and 1.3 < M < 2.2 and -1.0 < deltaE < 0.5"
tauLFVCuts_control_base = "nParticlesInList(pi+:taulfv) < 7 and 0.5 < M < 2.0 and -1.0 < deltaE < 0.5"
tauLFVCuts_control_prompt = tauLFVCuts_control_base + " and [daughter(0, pionID) > 0.1] and [daughter(1, pionID) > 0.1] and [daughter(2, pionID) > 0.1]"
tauLFVCuts_control_vertex = tauLFVCuts_control_base + " and [daughter(1, pionID) > 0.1]"

# define tau particle list and variable list
tau_list = []
var_list = []

if(args.prompt):
    # here, we only reconstruct tau -> mu mu mu
    ma.reconstructDecay("tau+:LFV_lll -> mu+:taulfv mu+:taulfv mu-:taulfv", tauLFVCuts3, 1, path=my_path)

    # basic analysis for tau: treefit, buildROE, continuum suppression, truth match, assign event type / sample type
    BasicAnalysisForTau("tau+:LFV_lll", sample_index=sample_index, type_index=type_index, energy_index=energy_index, path=my_path)

    # define variables
    var_list = DefineVariables("tau+:LFV_lll", photon_names=photon_names, IsItPrompt=True, path=my_path)

    # add in tau list
    tau_list = tau_list + ["tau+:LFV_lll"]

# do the same thing for the vertex displacement
if(args.vertex):
    pdg.add_particle('alpha0', 94144, 1.0, 0.004, 0, 0)  # name, PDG, mass, width, charge, spin
    ma.reconstructDecay("alpha0:LFV_vertex -> mu+:all mu-:all", cut="", path=my_path)
    ma.reconstructDecay("tau+:LFV_vertex -> alpha0:LFV_vertex mu+:taulfv", tauLFVCuts3, 10, path=my_path)
    BasicAnalysisForTau("tau+:LFV_vertex", sample_index=sample_index, type_index=type_index, energy_index=energy_index, path=my_path)
    var_list = DefineVariables("tau+:LFV_vertex", photon_names=photon_names, IsItPrompt=False, path=my_path)
    tau_list = tau_list + ["tau+:LFV_vertex"]

# do the same thing for the control sample
if(args.control):
    ma.reconstructDecay("tau+:LFV_control -> pi+:taulfv pi-:taulfv pi+:taulfv", tauLFVCuts_control_prompt, 20, path=my_path)
    BasicAnalysisForTau("tau+:LFV_control", sample_index=sample_index, type_index=type_index, energy_index=energy_index, path=my_path)
    var_list = DefineVariables("tau+:LFV_control", photon_names=photon_names, IsItPrompt=True, path=my_path)
    tau_list = tau_list + ["tau+:LFV_control"]

    stdV0s.stdKshorts(path=my_path)
    ma.reconstructDecay("tau+:LFV_control_KS -> K_S0:merged pi+:taulfv", tauLFVCuts_control_vertex, 30, path=my_path)
    BasicAnalysisForTau("tau+:LFV_control_KS", sample_index=sample_index, type_index=type_index, energy_index=energy_index, path=my_path)
    var_list = DefineVariables("tau+:LFV_control_KS", photon_names=photon_names, IsItPrompt=False, path=my_path)
    tau_list = tau_list + ["tau+:LFV_control_KS"]

# Make Ntuple and hashmap
if tau_list:
    ma.copyLists(outputListName="tau+:LFV_comb", inputListNames=tau_list, path=my_path)
    MakeNtupleandHashmap("tau+:LFV_comb", variable_list=var_list, Ntuple_name=output_file, hashmap_name=hashmapName, path=my_path)
             
# progress
basf2.process(my_path)

# Print call statistics
print(basf2.statistics)
