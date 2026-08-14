#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Descriptor: uubar

#############################################################
# Steering file for official MC production of early phase 3
# 'uubar' samples with beam backgrounds (BGx1).
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

#============================================================
# Generator level cut for UUBAR background
# This reduces 70% of entire sample
# but keeps 98% of signal-like uubar
#============================================================

def AnalysisGenCut(path):
    # copy from Doremy
    import modularAnalysis as ma
    import variables as va
    import variables.collections as vc

    MDeltaCuts = ["", "[1.0 < M < 4.0] and [-1.0 < deltaE < 1.0]", "[1.0 < M < 2.5] and [-1.0 < deltaE < 1.0]"]

    # Load particles from MC at first
    ma.fillParticleListFromMC('pi+:PrimaryMC', cut = 'mcPrimary', addDaughters=True, skipNonPrimaryDaughters=True, path=path)
    ma.fillParticleListFromMC('K+:PrimaryMC', cut = 'mcPrimary', addDaughters=True, skipNonPrimaryDaughters=True, path=path)
    ma.fillParticleListFromMC('e+:PrimaryMC', cut = 'mcPrimary', addDaughters=True, skipNonPrimaryDaughters=True, path=path)
    ma.fillParticleListFromMC('mu+:PrimaryMC', cut = 'mcPrimary', addDaughters=True, skipNonPrimaryDaughters=True, path=path)
    ma.fillParticleListFromMC('p+:PrimaryMC', cut = 'mcPrimary', addDaughters=True, skipNonPrimaryDaughters=True, path=path)
    ma.fillParticleListFromMC('nu_e:PrimaryMC', cut = 'mcPrimary', addDaughters=True, skipNonPrimaryDaughters=True, path=path)
    ma.fillParticleListFromMC('nu_mu:PrimaryMC', cut = 'mcPrimary', addDaughters=True, skipNonPrimaryDaughters=True, path=path)
    ma.fillParticleListFromMC('nu_tau:PrimaryMC', cut = 'mcPrimary', addDaughters=True, skipNonPrimaryDaughters=True, path=path)
    ma.fillParticleListFromMC('gamma:PrimaryMC', cut = 'mcPrimary', addDaughters=True, skipNonPrimaryDaughters=True, path=path)
    ma.fillParticleListFromMC("Xi-:PrimaryMC", cut = 'mcPrimary', addDaughters=True, skipNonPrimaryDaughters=True, path=path)
    ma.fillParticleListFromMC('Z0:PrimaryMC', cut = 'mcPrimary', addDaughters=True, skipNonPrimaryDaughters=True, path=path)
    ma.fillParticleListFromMC('D0:PrimaryMC', cut = 'mcPrimary', addDaughters=True, skipNonPrimaryDaughters=True, path=path)
    ma.fillParticleListFromMC('D+:PrimaryMC', cut = 'mcPrimary', addDaughters=True, skipNonPrimaryDaughters=True, path=path)
    ma.fillParticleListFromMC('pi+:PrimaryMC_good', cut = 'mcPrimary and [-5.0 < dz < 5.0] and [dr < 3.0]', addDaughters=True, skipNonPrimaryDaughters=True, path=path)
    ma.fillParticleListFromMC('K+:PrimaryMC_good', cut = 'mcPrimary and [-5.0 < dz < 5.0] and [dr < 3.0]', addDaughters=True, skipNonPrimaryDaughters=True, path=path)
    ma.fillParticleListFromMC('e+:PrimaryMC_good', cut = 'mcPrimary and [-5.0 < dz < 5.0] and [dr < 3.0]', addDaughters=True, skipNonPrimaryDaughters=True, path=path)
    ma.fillParticleListFromMC('mu+:PrimaryMC_good', cut = 'mcPrimary and [-5.0 < dz < 5.0] and [dr < 3.0]', addDaughters=True, skipNonPrimaryDaughters=True, path=path)
    ma.fillParticleListFromMC('p+:PrimaryMC_good', cut = 'mcPrimary and [-5.0 < dz < 5.0] and [dr < 3.0]', addDaughters=True, skipNonPrimaryDaughters=True, path=path)

    # convert mass hypothesis
    ma.copyList(outputListName="pi+:PrimaryMC_muMass", inputListName="pi+:PrimaryMC", path=path)
    ma.copyList(outputListName="K+:PrimaryMC_muMass",inputListName="K+:PrimaryMC",path=path)
    ma.copyList(outputListName="e+:PrimaryMC_muMass",inputListName="e+:PrimaryMC",path=path)
    ma.copyList(outputListName="p+:PrimaryMC_muMass",inputListName="p+:PrimaryMC",path=path)
    ma.copyList(outputListName="Xi-:PrimaryMC_muMass",inputListName="Xi-:PrimaryMC",path=path)
    path.add_module("ParticleMassUpdater", particleLists=["pi+:PrimaryMC_muMass","K+:PrimaryMC_muMass","e+:PrimaryMC_muMass","p+:PrimaryMC_muMass", "Xi-:PrimaryMC_muMass"], pdgCode=13)

    # reconstruct tau
    cut_mumumu = "[0.5 < M < 6.0] and [-4.0 < deltaE < 1.5]"
    ma.reconstructDecay(decayString="tau+:fake1 -> pi+:PrimaryMC_muMass pi-:PrimaryMC_muMass pi+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake2 -> pi+:PrimaryMC_muMass pi-:PrimaryMC_muMass mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake3 -> pi+:PrimaryMC_muMass mu-:PrimaryMC pi+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake4 -> pi+:PrimaryMC_muMass mu-:PrimaryMC mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake5 -> mu+:PrimaryMC pi-:PrimaryMC_muMass mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake6 -> mu+:PrimaryMC mu-:PrimaryMC mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake7 -> pi+:PrimaryMC_muMass pi-:PrimaryMC_muMass K+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake8 -> pi+:PrimaryMC_muMass K-:PrimaryMC_muMass pi+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake9 -> pi+:PrimaryMC_muMass K-:PrimaryMC_muMass K+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake10 -> K+:PrimaryMC_muMass pi-:PrimaryMC_muMass K+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake11 -> K+:PrimaryMC_muMass K-:PrimaryMC_muMass K+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake12 -> pi+:PrimaryMC_muMass pi-:PrimaryMC_muMass e+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake13 -> pi+:PrimaryMC_muMass e-:PrimaryMC_muMass pi+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake14 -> pi+:PrimaryMC_muMass e-:PrimaryMC_muMass e+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake15 -> e+:PrimaryMC_muMass pi-:PrimaryMC_muMass e+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake16 -> e+:PrimaryMC_muMass e-:PrimaryMC_muMass e+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake17 -> pi+:PrimaryMC_muMass pi-:PrimaryMC_muMass p+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake18 -> pi+:PrimaryMC_muMass anti-p-:PrimaryMC_muMass pi+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake19 -> pi+:PrimaryMC_muMass anti-p-:PrimaryMC_muMass p+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake20 -> p+:PrimaryMC_muMass pi-:PrimaryMC_muMass p+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake21 -> p+:PrimaryMC_muMass anti-p-:PrimaryMC_muMass p+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake22 -> mu+:PrimaryMC mu-:PrimaryMC p+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake23 -> mu+:PrimaryMC anti-p-:PrimaryMC_muMass mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake24 -> mu+:PrimaryMC anti-p-:PrimaryMC_muMass p+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake25 -> p+:PrimaryMC_muMass mu-:PrimaryMC p+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake26 -> mu+:PrimaryMC anti-p-:PrimaryMC_muMass pi+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake27 -> mu+:PrimaryMC pi-:PrimaryMC_muMass p+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake28 -> p+:PrimaryMC_muMass mu-:PrimaryMC pi+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake29 -> pi+:PrimaryMC_muMass K-:PrimaryMC_muMass p+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake30 -> pi+:PrimaryMC_muMass anti-p-:PrimaryMC_muMass K+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake31 -> K+:PrimaryMC_muMass pi-:PrimaryMC_muMass p+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake32 -> K+:PrimaryMC_muMass K-:PrimaryMC_muMass mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake33 -> K+:PrimaryMC_muMass mu-:PrimaryMC K+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake34 -> K+:PrimaryMC_muMass mu-:PrimaryMC mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake35 -> mu+:PrimaryMC K-:PrimaryMC_muMass mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake36 -> K+:PrimaryMC_muMass mu-:PrimaryMC pi+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake37 -> K+:PrimaryMC_muMass pi-:PrimaryMC_muMass mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake38 -> mu+:PrimaryMC K-:PrimaryMC_muMass pi+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake39 -> p+:PrimaryMC_muMass anti-p-:PrimaryMC_muMass K+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake40 -> p+:PrimaryMC_muMass K-:PrimaryMC_muMass p+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake41 -> K+:PrimaryMC_muMass anti-p-:PrimaryMC_muMass K+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake42 -> K+:PrimaryMC_muMass K-:PrimaryMC_muMass p+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake43 -> K+:PrimaryMC_muMass e-:PrimaryMC_muMass K+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake44 -> K+:PrimaryMC_muMass K-:PrimaryMC_muMass e+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake45 -> K+:PrimaryMC_muMass e-:PrimaryMC_muMass e+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake46 -> e+:PrimaryMC_muMass K-:PrimaryMC_muMass e+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake47 -> anti-Xi+:PrimaryMC_muMass pi-:PrimaryMC_muMass pi+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake48 -> pi+:PrimaryMC_muMass Xi-:PrimaryMC_muMass pi+:PrimaryMC_muMass", cut=cut_mumumu, path=path)
    ma.copyLists(outputListName="tau+:fake", inputListNames=[f"tau+:fake{i}" for i in range(1, 49)], path=path)

    # define several gen tau candidates
    for idx, MDeltaCut in enumerate(MDeltaCuts):
        ma.cutAndCopyList(outputListName = "tau+:fake_strict" + str(idx), inputListName = "tau+:fake", cut = MDeltaCut, path=path)
        ma.rankByLowest(particleList="tau+:fake_strict" + str(idx), variable="abs(deltaE)", outputVariable='bcs_deltaE', overwriteRank = True, path=path)
        ma.rankByLowest(particleList="tau+:fake_strict" + str(idx), variable="abs(dM)", outputVariable='bcs_dM', overwriteRank = True, path=path)
        ma.cutAndCopyList(outputListName = "tau+:BCS_strict_deltaE" + str(idx), inputListName = "tau+:fake_strict" + str(idx), cut = "extraInfo(bcs_deltaE) == 1", path=path)
        ma.cutAndCopyList(outputListName = "tau+:BCS_strict_dM" + str(idx), inputListName = "tau+:fake_strict" + str(idx), cut = "extraInfo(bcs_dM) == 1", path=path)

    # event kinematics
    ma.buildEventKinematicsFromMC(inputListNames=None, selectionCut='', path=path)
    ma.buildEventShape(inputListNames=["pi+:PrimaryMC", "K+:PrimaryMC", "e+:PrimaryMC", "mu+:PrimaryMC", "p+:PrimaryMC", "gamma:PrimaryMC"], default_cleanup=False, path=path)
    ma.buildRestOfEventFromMC("Z0:PrimaryMC", inputParticlelists=["pi+:PrimaryMC", "K+:PrimaryMC", "e+:PrimaryMC", "mu+:PrimaryMC", "p+:PrimaryMC", "gamma:PrimaryMC"], path=path)
    ma.appendROEMasks(list_name="Z0:PrimaryMC", mask_tuples=[("cleanMask_gencut","","")], path=path)
    ma.buildContinuumSuppression(list_name="Z0:PrimaryMC", roe_mask="cleanMask_gencut", path=path)

    # define roe path for Z0 and variables
    roe_path_Z = b2.Path()
    deadEndPath_Z = b2.Path()
    ma.signalSideParticleFilter("Z0:PrimaryMC", '', roe_path_Z, deadEndPath_Z)
    ma.fillSignalSideParticleList("Z0:PrimaryMC_ROE", "^Z0:PrimaryMC", roe_path_Z)

    for idx, MDeltaCut in enumerate(MDeltaCuts):
        tau_exist_path = b2.create_path()
        tau_nonexist_path = b2.create_path()
        ma.variableToSignalSideExtraInfo("Z0:PrimaryMC_ROE", {'formula(averageValueInList(tau+:fake_strict' + str(idx) + ', formula((M-averageValueInList(tau+:fake_strict' + str(idx) + ',M))**2))**0.5)': 'std_M' + str(idx)}, path=tau_exist_path)
        ma.variableToSignalSideExtraInfo("Z0:PrimaryMC_ROE", {'formula(averageValueInList(tau+:fake_strict' + str(idx) + ', formula((deltaE-averageValueInList(tau+:fake_strict' + str(idx) + ',deltaE))**2))**0.5)': 'std_deltaE' + str(idx)}, path=tau_exist_path)
        ma.variableToSignalSideExtraInfo('Z0:PrimaryMC_ROE', {'constant(-1)': 'std_M' + str(idx)}, path=tau_nonexist_path)
        ma.variableToSignalSideExtraInfo('Z0:PrimaryMC_ROE', {'constant(-1)': 'std_deltaE' + str(idx)}, path=tau_nonexist_path)
        tau_gencut_module = roe_path_Z.add_module("VariableToReturnValue", variable="nParticlesInList(tau+:fake_strict" + str(idx) + ")")
        tau_gencut_module.if_value(">=1", tau_exist_path, b2.AfterConditionPath.CONTINUE)
        tau_gencut_module.if_value("<1", tau_nonexist_path, b2.AfterConditionPath.CONTINUE)

    path.for_each('RestOfEvent', 'RestOfEvents', roe_path_Z)

    # define variables for 2nd order logistic regression
    va.variables.addAlias("Ntrack_gencut","formula(nParticlesInList(pi+:PrimaryMC) + nParticlesInList(K+:PrimaryMC) + nParticlesInList(e+:PrimaryMC) + nParticlesInList(mu+:PrimaryMC) + nParticlesInList(p+:PrimaryMC))")
    va.variables.addAlias("et_gencut","averageValueInList(Z0:PrimaryMC,KSFWVariables(et))")
    va.variables.addAlias("cosTBz_gencut","averageValueInList(Z0:PrimaryMC,cosTBz)")

    va.variables.addAlias("Ntau_fake_strict0","nParticlesInList(tau+:fake_strict0)")
    va.variables.addAlias("Ntau_fake_strict1","nParticlesInList(tau+:fake_strict1)")
    va.variables.addAlias("Ntau_fake_strict2","nParticlesInList(tau+:fake_strict2)")
    va.variables.addAlias("sumValueInList_BCS_strict_deltaE2_M","sumValueInList(tau+:BCS_strict_deltaE2, M)")
    va.variables.addAlias("sumValueInList_BCS_strict_deltaE2_deltaE","sumValueInList(tau+:BCS_strict_deltaE2, deltaE)")
    va.variables.addAlias("sumValueInList_BCS_strict_deltaE2_Mbc","sumValueInList(tau+:BCS_strict_deltaE2, Mbc)")
    va.variables.addAlias("sumValueInList_BCS_strict_dM2_M","sumValueInList(tau+:BCS_strict_dM2, M)")
    va.variables.addAlias("sumValueInList_BCS_strict_dM2_deltaE","sumValueInList(tau+:BCS_strict_dM2, deltaE)")
    va.variables.addAlias("sumValueInList_BCS_strict_dM2_Mbc","sumValueInList(tau+:BCS_strict_dM2, Mbc)")
    va.variables.addAlias("averageValueInList_fake_strict2_M","averageValueInList(tau+:fake_strict2, M)")
    va.variables.addAlias("averageValueInList_fake_strict2_deltaE","averageValueInList(tau+:fake_strict2, deltaE)")
    va.variables.addAlias("std_M2","extraInfo(std_M2)")
    va.variables.addAlias("std_deltaE2","extraInfo(std_deltaE2)")
    va.variables.addAlias("sumValueInList_BCS_strict_deltaE2_daughterHighest_p","sumValueInList(tau+:BCS_strict_deltaE2,daughterHighest(p))")
    va.variables.addAlias("sumValueInList_BCS_strict_deltaE2_daughterLowest_p","sumValueInList(tau+:BCS_strict_deltaE2,daughterLowest(p))")
    va.variables.addAlias("sumValueInList_BCS_strict_dM2_daughterHighest_p","sumValueInList(tau+:BCS_strict_dM2,daughterHighest(p))")
    va.variables.addAlias("sumValueInList_BCS_strict_dM2_daughterLowest_p","sumValueInList(tau+:BCS_strict_dM2,daughterLowest(p))")

    variable_list = []
    variable_list = variable_list + ["formula(nParticlesInList(pi+:PrimaryMC) + nParticlesInList(K+:PrimaryMC) + nParticlesInList(e+:PrimaryMC) + nParticlesInList(mu+:PrimaryMC) + nParticlesInList(p+:PrimaryMC))"]
    variable_list = variable_list + ["formula(nParticlesInList(pi+:PrimaryMC_good) + nParticlesInList(K+:PrimaryMC_good) + nParticlesInList(e+:PrimaryMC_good) + nParticlesInList(mu+:PrimaryMC_good) + nParticlesInList(p+:PrimaryMC_good))"]
    variable_list = variable_list + ["nParticlesInList(pi+:PrimaryMC)", "nParticlesInList(mu+:PrimaryMC)"]
    variable_list = variable_list + ["nParticlesInList(D0:PrimaryMC)", "nParticlesInList(D+:PrimaryMC)"]
    variable_list = variable_list + ["formula(nParticlesInList(nu_e:PrimaryMC) + nParticlesInList(nu_mu:PrimaryMC) + nParticlesInList(nu_tau:PrimaryMC))"]
    variable_list = variable_list + vc.mc_event_kinematics
    variable_list = variable_list + vc.event_shape
    variable_list = variable_list + ["R2", "cosTBTO", "cosTBz", "thrustBm", "thrustOm", "CleoConeCS(1)", "CleoConeCS(2)", "CleoConeCS(3)", "CleoConeCS(4)", "CleoConeCS(5)", "CleoConeCS(6)", "CleoConeCS(7)", "CleoConeCS(8)", "CleoConeCS(9)", "KSFWVariables(mm2)", "KSFWVariables(et)", "KSFWVariables(hso00)", "KSFWVariables(hso01)", "KSFWVariables(hso02)", "KSFWVariables(hso03)", "KSFWVariables(hso04)", "KSFWVariables(hso10)", "KSFWVariables(hso12)", "KSFWVariables(hso14)", "KSFWVariables(hso20)", "KSFWVariables(hso22)", "KSFWVariables(hso24)", "KSFWVariables(hoo0)", "KSFWVariables(hoo1)", "KSFWVariables(hoo2)", "KSFWVariables(hoo3)", "KSFWVariables(hoo4)"]

    for idx, MDeltaCut in enumerate(MDeltaCuts):
        listName = "tau+:fake_strict" + str(idx)
        variable_list = variable_list + ["nParticlesInList(" + listName + ")"]
        variable_list = variable_list + ["averageValueInList(" + listName + ", M)", "averageValueInList(" + listName + ", deltaE)"]
        variable_list = variable_list + ["extraInfo(std_M" + str(idx) +  ")", "extraInfo(std_deltaE" + str(idx) +  ")"]

        listName = "tau+:BCS_strict_deltaE" + str(idx)
        variable_list = variable_list + ["sumValueInList(" + listName + ", dM)", "sumValueInList(" + listName + ", M)", "sumValueInList(" + listName + ", deltaE)", "sumValueInList(" + listName + ", p)", "sumValueInList(" + listName + ", Mbc)"]
        variable_list = variable_list + ["sumValueInList(" + listName + ",R2)", "sumValueInList(" + listName + ",cosTBTO)", "sumValueInList(" + listName + ",cosTBz)", "sumValueInList(" + listName + ",thrustBm)", "sumValueInList(" + listName + ",thrustOm)"]
        variable_list = variable_list + ["sumValueInList(" + listName + ",daughterHighest(p))", "sumValueInList(" + listName + ",daughterLowest(p))", "sumValueInList(" + listName + ",daughterHighest(E))", "sumValueInList(" + listName + ",daughterLowest(E))"]

        listName = "tau+:BCS_strict_dM" + str(idx)
        variable_list = variable_list + ["sumValueInList(" + listName + ", dM)", "sumValueInList(" + listName + ", M)", "sumValueInList(" + listName + ", deltaE)", "sumValueInList(" + listName + ", p)", "sumValueInList(" + listName + ", Mbc)"]
        variable_list = variable_list + ["sumValueInList(" + listName + ",R2)", "sumValueInList(" + listName + ",cosTBTO)", "sumValueInList(" + listName + ",cosTBz)", "sumValueInList(" + listName + ",thrustBm)", "sumValueInList(" + listName + ",thrustOm)"]
        variable_list = variable_list + ["sumValueInList(" + listName + ",daughterHighest(p))", "sumValueInList(" + listName + ",daughterLowest(p))", "sumValueInList(" + listName + ",daughterHighest(E))", "sumValueInList(" + listName + ",daughterLowest(E))"]
    
    variable_list = variable_list + ["Ntrack_gencut", "et_gencut", "cosTBz_gencut"]

    # calculate LR output
    va.variables.addAlias(
        "LR_score_A_quad",
        "formula((0.1186688840389252) + (0.2055148780345917)*(((thrustAxisCosTheta-(0.5146591413059449))/(0.4293886338681585))) + (-0.0549737960100174)*(((genTotalPhotonsEnergyOfEvent-(2.275976616651866))/(1.701732626123625))) + (-0.2284597605466843)*(((pt_sum_gencut-(6.798332307135105))/(2.327924768090643))) + (-0.4824159145355225)*(((Ntrack_gencut-(6.380391663835695))/(1.711191225122634))) + (0.2278170585632324)*(((cosTBz_gencut-(0.5950749008205581))/(0.2947507726445153))) + (0.02244050800800323)*(((foxWolframR1-(0.01081985664397396))/(0.04871152680809205))) + (-0.4640422463417053)*(((Ntau_fake_strict0-(25.64691138746151))/(23.6254377377478))) + (0.1128687337040901)*(((Ntau_fake_strict2-(1.752246275823144))/(1.162838617211971))) + (-0.1439648568630219)*(((sumValueInList_BCS_strict_deltaE2_M-(1.849384925862775))/(0.3868020880377942))) + (0.6606563329696655)*(((sumValueInList_BCS_strict_deltaE2_deltaE-(-0.5622677670186995))/(0.2934337790981981))) + (-0.6297210454940796)*(((sumValueInList_BCS_strict_deltaE2_Mbc-(2.970287399442505))/(0.5417830982981453))) + (-0.3730591535568237)*(((sumValueInList_BCS_strict_dM2_M-(1.812249086670265))/(0.3316852184507579))) + (0.5768011808395386)*(((sumValueInList_BCS_strict_dM2_deltaE-(-0.6152694269953234))/(0.2907571499778808))) + (-0.5975413918495178)*(((sumValueInList_BCS_strict_dM2_Mbc-(3.027244666078906))/(0.5255793101404719))) + (0.2500165998935699)*(((sumValueInList_BCS_strict_dM2_daughterHighest_p-(3.143958461381385))/(0.9465581819457167))) + (0.08113214373588562)*((((thrustAxisCosTheta-(0.5146591413059449))/(0.4293886338681585))^2)) + (-0.00777830695733428)*(((thrustAxisCosTheta-(0.5146591413059449))/(0.4293886338681585))*((genTotalPhotonsEnergyOfEvent-(2.275976616651866))/(1.701732626123625))) + (0.0390806756913662)*(((thrustAxisCosTheta-(0.5146591413059449))/(0.4293886338681585))*((pt_sum_gencut-(6.798332307135105))/(2.327924768090643))) + (-0.06220989301800728)*(((thrustAxisCosTheta-(0.5146591413059449))/(0.4293886338681585))*((Ntrack_gencut-(6.380391663835695))/(1.711191225122634))) + (0.03568518534302711)*(((thrustAxisCosTheta-(0.5146591413059449))/(0.4293886338681585))*((cosTBz_gencut-(0.5950749008205581))/(0.2947507726445153))) + (0.0002008324372582138)*(((thrustAxisCosTheta-(0.5146591413059449))/(0.4293886338681585))*((foxWolframR1-(0.01081985664397396))/(0.04871152680809205))) + (0.06098149716854095)*(((thrustAxisCosTheta-(0.5146591413059449))/(0.4293886338681585))*((Ntau_fake_strict0-(25.64691138746151))/(23.6254377377478))) + (0.007879340089857578)*(((thrustAxisCosTheta-(0.5146591413059449))/(0.4293886338681585))*((Ntau_fake_strict2-(1.752246275823144))/(1.162838617211971))) + (0.06303373724222183)*(((thrustAxisCosTheta-(0.5146591413059449))/(0.4293886338681585))*((sumValueInList_BCS_strict_deltaE2_M-(1.849384925862775))/(0.3868020880377942))) + (-0.1079274341464043)*(((thrustAxisCosTheta-(0.5146591413059449))/(0.4293886338681585))*((sumValueInList_BCS_strict_deltaE2_deltaE-(-0.5622677670186995))/(0.2934337790981981))) + (-0.0625104159116745)*(((thrustAxisCosTheta-(0.5146591413059449))/(0.4293886338681585))*((sumValueInList_BCS_strict_deltaE2_Mbc-(2.970287399442505))/(0.5417830982981453))) + (0.03626422211527824)*(((thrustAxisCosTheta-(0.5146591413059449))/(0.4293886338681585))*((sumValueInList_BCS_strict_dM2_M-(1.812249086670265))/(0.3316852184507579))) + (-0.06294599175453186)*(((thrustAxisCosTheta-(0.5146591413059449))/(0.4293886338681585))*((sumValueInList_BCS_strict_dM2_deltaE-(-0.6152694269953234))/(0.2907571499778808))) + (-0.08610847592353821)*(((thrustAxisCosTheta-(0.5146591413059449))/(0.4293886338681585))*((sumValueInList_BCS_strict_dM2_Mbc-(3.027244666078906))/(0.5255793101404719))) + (0.0002310115960426629)*(((thrustAxisCosTheta-(0.5146591413059449))/(0.4293886338681585))*((sumValueInList_BCS_strict_dM2_daughterHighest_p-(3.143958461381385))/(0.9465581819457167))) + (-0.1195145547389984)*((((genTotalPhotonsEnergyOfEvent-(2.275976616651866))/(1.701732626123625))^2)) + (-0.07064411789178848)*(((genTotalPhotonsEnergyOfEvent-(2.275976616651866))/(1.701732626123625))*((pt_sum_gencut-(6.798332307135105))/(2.327924768090643))) + (0.03135225549340248)*(((genTotalPhotonsEnergyOfEvent-(2.275976616651866))/(1.701732626123625))*((Ntrack_gencut-(6.380391663835695))/(1.711191225122634))) + (-0.03491450101137161)*(((genTotalPhotonsEnergyOfEvent-(2.275976616651866))/(1.701732626123625))*((cosTBz_gencut-(0.5950749008205581))/(0.2947507726445153))) + (-0.02742075547575951)*(((genTotalPhotonsEnergyOfEvent-(2.275976616651866))/(1.701732626123625))*((foxWolframR1-(0.01081985664397396))/(0.04871152680809205))) + (-0.007818615064024925)*(((genTotalPhotonsEnergyOfEvent-(2.275976616651866))/(1.701732626123625))*((Ntau_fake_strict0-(25.64691138746151))/(23.6254377377478))) + (-0.0572962611913681)*(((genTotalPhotonsEnergyOfEvent-(2.275976616651866))/(1.701732626123625))*((Ntau_fake_strict2-(1.752246275823144))/(1.162838617211971))) + (0.01721076481044292)*(((genTotalPhotonsEnergyOfEvent-(2.275976616651866))/(1.701732626123625))*((sumValueInList_BCS_strict_deltaE2_M-(1.849384925862775))/(0.3868020880377942))) + (-0.02098436839878559)*(((genTotalPhotonsEnergyOfEvent-(2.275976616651866))/(1.701732626123625))*((sumValueInList_BCS_strict_deltaE2_deltaE-(-0.5622677670186995))/(0.2934337790981981))) + (0.005118965171277523)*(((genTotalPhotonsEnergyOfEvent-(2.275976616651866))/(1.701732626123625))*((sumValueInList_BCS_strict_deltaE2_Mbc-(2.970287399442505))/(0.5417830982981453))) + (0.01688759215176105)*(((genTotalPhotonsEnergyOfEvent-(2.275976616651866))/(1.701732626123625))*((sumValueInList_BCS_strict_dM2_M-(1.812249086670265))/(0.3316852184507579))) + (0.02546093799173832)*(((genTotalPhotonsEnergyOfEvent-(2.275976616651866))/(1.701732626123625))*((sumValueInList_BCS_strict_dM2_deltaE-(-0.6152694269953234))/(0.2907571499778808))) + (0.007848691195249557)*(((genTotalPhotonsEnergyOfEvent-(2.275976616651866))/(1.701732626123625))*((sumValueInList_BCS_strict_dM2_Mbc-(3.027244666078906))/(0.5255793101404719))) + (0.07027687877416611)*(((genTotalPhotonsEnergyOfEvent-(2.275976616651866))/(1.701732626123625))*((sumValueInList_BCS_strict_dM2_daughterHighest_p-(3.143958461381385))/(0.9465581819457167))) + (-0.4098920524120331)*((((pt_sum_gencut-(6.798332307135105))/(2.327924768090643))^2)) + (-0.0001705346221569926)*(((pt_sum_gencut-(6.798332307135105))/(2.327924768090643))*((Ntrack_gencut-(6.380391663835695))/(1.711191225122634))) + (-0.06275465339422226)*(((pt_sum_gencut-(6.798332307135105))/(2.327924768090643))*((cosTBz_gencut-(0.5950749008205581))/(0.2947507726445153))) + (0.02861950732767582)*(((pt_sum_gencut-(6.798332307135105))/(2.327924768090643))*((foxWolframR1-(0.01081985664397396))/(0.04871152680809205))) + (-0.2371815294027328)*(((pt_sum_gencut-(6.798332307135105))/(2.327924768090643))*((Ntau_fake_strict0-(25.64691138746151))/(23.6254377377478))) + (0.03816239908337593)*(((pt_sum_gencut-(6.798332307135105))/(2.327924768090643))*((Ntau_fake_strict2-(1.752246275823144))/(1.162838617211971))) + (0.02075269445776939)*(((pt_sum_gencut-(6.798332307135105))/(2.327924768090643))*((sumValueInList_BCS_strict_deltaE2_M-(1.849384925862775))/(0.3868020880377942))) + (0.2842419445514679)*(((pt_sum_gencut-(6.798332307135105))/(2.327924768090643))*((sumValueInList_BCS_strict_deltaE2_deltaE-(-0.5622677670186995))/(0.2934337790981981))) + (-0.09046971797943115)*(((pt_sum_gencut-(6.798332307135105))/(2.327924768090643))*((sumValueInList_BCS_strict_deltaE2_Mbc-(2.970287399442505))/(0.5417830982981453))) + (0.03874855861067772)*(((pt_sum_gencut-(6.798332307135105))/(2.327924768090643))*((sumValueInList_BCS_strict_dM2_M-(1.812249086670265))/(0.3316852184507579))) + (0.2514238059520721)*(((pt_sum_gencut-(6.798332307135105))/(2.327924768090643))*((sumValueInList_BCS_strict_dM2_deltaE-(-0.6152694269953234))/(0.2907571499778808))) + (0.08873993158340454)*(((pt_sum_gencut-(6.798332307135105))/(2.327924768090643))*((sumValueInList_BCS_strict_dM2_Mbc-(3.027244666078906))/(0.5255793101404719))) + (-0.04524453356862068)*(((pt_sum_gencut-(6.798332307135105))/(2.327924768090643))*((sumValueInList_BCS_strict_dM2_daughterHighest_p-(3.143958461381385))/(0.9465581819457167))) + (-0.2510567605495453)*((((Ntrack_gencut-(6.380391663835695))/(1.711191225122634))^2)) + (-0.06614241003990173)*(((Ntrack_gencut-(6.380391663835695))/(1.711191225122634))*((cosTBz_gencut-(0.5950749008205581))/(0.2947507726445153))) + (0.05272238329052925)*(((Ntrack_gencut-(6.380391663835695))/(1.711191225122634))*((foxWolframR1-(0.01081985664397396))/(0.04871152680809205))) + (-0.07549957931041718)*(((Ntrack_gencut-(6.380391663835695))/(1.711191225122634))*((Ntau_fake_strict0-(25.64691138746151))/(23.6254377377478))) + (-0.04625397920608521)*(((Ntrack_gencut-(6.380391663835695))/(1.711191225122634))*((Ntau_fake_strict2-(1.752246275823144))/(1.162838617211971))) + (-0.04665509611368179)*(((Ntrack_gencut-(6.380391663835695))/(1.711191225122634))*((sumValueInList_BCS_strict_deltaE2_M-(1.849384925862775))/(0.3868020880377942))) + (-0.01572211645543575)*(((Ntrack_gencut-(6.380391663835695))/(1.711191225122634))*((sumValueInList_BCS_strict_deltaE2_deltaE-(-0.5622677670186995))/(0.2934337790981981))) + (0.004707342945039272)*(((Ntrack_gencut-(6.380391663835695))/(1.711191225122634))*((sumValueInList_BCS_strict_deltaE2_Mbc-(2.970287399442505))/(0.5417830982981453))) + (-0.03400611504912376)*(((Ntrack_gencut-(6.380391663835695))/(1.711191225122634))*((sumValueInList_BCS_strict_dM2_M-(1.812249086670265))/(0.3316852184507579))) + (0.02161242999136448)*(((Ntrack_gencut-(6.380391663835695))/(1.711191225122634))*((sumValueInList_BCS_strict_dM2_deltaE-(-0.6152694269953234))/(0.2907571499778808))) + (-0.06196905300021172)*(((Ntrack_gencut-(6.380391663835695))/(1.711191225122634))*((sumValueInList_BCS_strict_dM2_Mbc-(3.027244666078906))/(0.5255793101404719))) + (-0.01949671655893326)*(((Ntrack_gencut-(6.380391663835695))/(1.711191225122634))*((sumValueInList_BCS_strict_dM2_daughterHighest_p-(3.143958461381385))/(0.9465581819457167))) + (0.08877944946289062)*((((cosTBz_gencut-(0.5950749008205581))/(0.2947507726445153))^2)) + (0.01843264140188694)*(((cosTBz_gencut-(0.5950749008205581))/(0.2947507726445153))*((foxWolframR1-(0.01081985664397396))/(0.04871152680809205))) + (0.104617528617382)*(((cosTBz_gencut-(0.5950749008205581))/(0.2947507726445153))*((Ntau_fake_strict0-(25.64691138746151))/(23.6254377377478))) + (0.01001971866935492)*(((cosTBz_gencut-(0.5950749008205581))/(0.2947507726445153))*((Ntau_fake_strict2-(1.752246275823144))/(1.162838617211971))) + (0.06748604029417038)*(((cosTBz_gencut-(0.5950749008205581))/(0.2947507726445153))*((sumValueInList_BCS_strict_deltaE2_M-(1.849384925862775))/(0.3868020880377942))) + (-0.1692784875631332)*(((cosTBz_gencut-(0.5950749008205581))/(0.2947507726445153))*((sumValueInList_BCS_strict_deltaE2_deltaE-(-0.5622677670186995))/(0.2934337790981981))) + (-0.07593703269958496)*(((cosTBz_gencut-(0.5950749008205581))/(0.2947507726445153))*((sumValueInList_BCS_strict_deltaE2_Mbc-(2.970287399442505))/(0.5417830982981453))) + (0.009501133114099503)*(((cosTBz_gencut-(0.5950749008205581))/(0.2947507726445153))*((sumValueInList_BCS_strict_dM2_M-(1.812249086670265))/(0.3316852184507579))) + (-0.08947980403900146)*(((cosTBz_gencut-(0.5950749008205581))/(0.2947507726445153))*((sumValueInList_BCS_strict_dM2_deltaE-(-0.6152694269953234))/(0.2907571499778808))) + (-0.1272817701101303)*(((cosTBz_gencut-(0.5950749008205581))/(0.2947507726445153))*((sumValueInList_BCS_strict_dM2_Mbc-(3.027244666078906))/(0.5255793101404719))) + (0.04484111443161964)*(((cosTBz_gencut-(0.5950749008205581))/(0.2947507726445153))*((sumValueInList_BCS_strict_dM2_daughterHighest_p-(3.143958461381385))/(0.9465581819457167))) + (0.003590549575164914)*((((foxWolframR1-(0.01081985664397396))/(0.04871152680809205))^2)) + (0.07299890369176865)*(((foxWolframR1-(0.01081985664397396))/(0.04871152680809205))*((Ntau_fake_strict0-(25.64691138746151))/(23.6254377377478))) + (-0.01908277906477451)*(((foxWolframR1-(0.01081985664397396))/(0.04871152680809205))*((Ntau_fake_strict2-(1.752246275823144))/(1.162838617211971))) + (-0.0004556726489681751)*(((foxWolframR1-(0.01081985664397396))/(0.04871152680809205))*((sumValueInList_BCS_strict_deltaE2_M-(1.849384925862775))/(0.3868020880377942))) + (0.01577415317296982)*(((foxWolframR1-(0.01081985664397396))/(0.04871152680809205))*((sumValueInList_BCS_strict_deltaE2_deltaE-(-0.5622677670186995))/(0.2934337790981981))) + (0.01241827756166458)*(((foxWolframR1-(0.01081985664397396))/(0.04871152680809205))*((sumValueInList_BCS_strict_deltaE2_Mbc-(2.970287399442505))/(0.5417830982981453))) + (-0.01536391116678715)*(((foxWolframR1-(0.01081985664397396))/(0.04871152680809205))*((sumValueInList_BCS_strict_dM2_M-(1.812249086670265))/(0.3316852184507579))) + (0.01039275713264942)*(((foxWolframR1-(0.01081985664397396))/(0.04871152680809205))*((sumValueInList_BCS_strict_dM2_deltaE-(-0.6152694269953234))/(0.2907571499778808))) + (0.01405764278024435)*(((foxWolframR1-(0.01081985664397396))/(0.04871152680809205))*((sumValueInList_BCS_strict_dM2_Mbc-(3.027244666078906))/(0.5255793101404719))) + (0.01585699431598186)*(((foxWolframR1-(0.01081985664397396))/(0.04871152680809205))*((sumValueInList_BCS_strict_dM2_daughterHighest_p-(3.143958461381385))/(0.9465581819457167))) + (0.1173619702458382)*((((Ntau_fake_strict0-(25.64691138746151))/(23.6254377377478))^2)) + (0.01113045960664749)*(((Ntau_fake_strict0-(25.64691138746151))/(23.6254377377478))*((Ntau_fake_strict2-(1.752246275823144))/(1.162838617211971))) + (-0.007881266064941883)*(((Ntau_fake_strict0-(25.64691138746151))/(23.6254377377478))*((sumValueInList_BCS_strict_deltaE2_M-(1.849384925862775))/(0.3868020880377942))) + (-0.07680971175432205)*(((Ntau_fake_strict0-(25.64691138746151))/(23.6254377377478))*((sumValueInList_BCS_strict_deltaE2_deltaE-(-0.5622677670186995))/(0.2934337790981981))) + (0.04766774550080299)*(((Ntau_fake_strict0-(25.64691138746151))/(23.6254377377478))*((sumValueInList_BCS_strict_deltaE2_Mbc-(2.970287399442505))/(0.5417830982981453))) + (0.03596783429384232)*(((Ntau_fake_strict0-(25.64691138746151))/(23.6254377377478))*((sumValueInList_BCS_strict_dM2_M-(1.812249086670265))/(0.3316852184507579))) + (-0.04600825905799866)*(((Ntau_fake_strict0-(25.64691138746151))/(23.6254377377478))*((sumValueInList_BCS_strict_dM2_deltaE-(-0.6152694269953234))/(0.2907571499778808))) + (-0.01567505300045013)*(((Ntau_fake_strict0-(25.64691138746151))/(23.6254377377478))*((sumValueInList_BCS_strict_dM2_Mbc-(3.027244666078906))/(0.5255793101404719))) + (0.0313185453414917)*(((Ntau_fake_strict0-(25.64691138746151))/(23.6254377377478))*((sumValueInList_BCS_strict_dM2_daughterHighest_p-(3.143958461381385))/(0.9465581819457167))) + (-0.01273221150040627)*((((Ntau_fake_strict2-(1.752246275823144))/(1.162838617211971))^2)) + (0.07071412354707718)*(((Ntau_fake_strict2-(1.752246275823144))/(1.162838617211971))*((sumValueInList_BCS_strict_deltaE2_M-(1.849384925862775))/(0.3868020880377942))) + (0.1089488565921783)*(((Ntau_fake_strict2-(1.752246275823144))/(1.162838617211971))*((sumValueInList_BCS_strict_deltaE2_deltaE-(-0.5622677670186995))/(0.2934337790981981))) + (-0.04171757027506828)*(((Ntau_fake_strict2-(1.752246275823144))/(1.162838617211971))*((sumValueInList_BCS_strict_deltaE2_Mbc-(2.970287399442505))/(0.5417830982981453))) + (0.01563476584851742)*(((Ntau_fake_strict2-(1.752246275823144))/(1.162838617211971))*((sumValueInList_BCS_strict_dM2_M-(1.812249086670265))/(0.3316852184507579))) + (-0.0310070738196373)*(((Ntau_fake_strict2-(1.752246275823144))/(1.162838617211971))*((sumValueInList_BCS_strict_dM2_deltaE-(-0.6152694269953234))/(0.2907571499778808))) + (0.02439548820257187)*(((Ntau_fake_strict2-(1.752246275823144))/(1.162838617211971))*((sumValueInList_BCS_strict_dM2_Mbc-(3.027244666078906))/(0.5255793101404719))) + (0.0105758523568511)*(((Ntau_fake_strict2-(1.752246275823144))/(1.162838617211971))*((sumValueInList_BCS_strict_dM2_daughterHighest_p-(3.143958461381385))/(0.9465581819457167))) + (-0.4305009841918945)*((((sumValueInList_BCS_strict_deltaE2_M-(1.849384925862775))/(0.3868020880377942))^2)) + (-0.1146741658449173)*(((sumValueInList_BCS_strict_deltaE2_M-(1.849384925862775))/(0.3868020880377942))*((sumValueInList_BCS_strict_deltaE2_deltaE-(-0.5622677670186995))/(0.2934337790981981))) + (0.04993986710906029)*(((sumValueInList_BCS_strict_deltaE2_M-(1.849384925862775))/(0.3868020880377942))*((sumValueInList_BCS_strict_deltaE2_Mbc-(2.970287399442505))/(0.5417830982981453))) + (-0.3420024812221527)*(((sumValueInList_BCS_strict_deltaE2_M-(1.849384925862775))/(0.3868020880377942))*((sumValueInList_BCS_strict_dM2_M-(1.812249086670265))/(0.3316852184507579))) + (0.003919490147382021)*(((sumValueInList_BCS_strict_deltaE2_M-(1.849384925862775))/(0.3868020880377942))*((sumValueInList_BCS_strict_dM2_deltaE-(-0.6152694269953234))/(0.2907571499778808))) + (-0.0918261706829071)*(((sumValueInList_BCS_strict_deltaE2_M-(1.849384925862775))/(0.3868020880377942))*((sumValueInList_BCS_strict_dM2_Mbc-(3.027244666078906))/(0.5255793101404719))) + (0.02696399018168449)*(((sumValueInList_BCS_strict_deltaE2_M-(1.849384925862775))/(0.3868020880377942))*((sumValueInList_BCS_strict_dM2_daughterHighest_p-(3.143958461381385))/(0.9465581819457167))) + (-0.04553355649113655)*((((sumValueInList_BCS_strict_deltaE2_deltaE-(-0.5622677670186995))/(0.2934337790981981))^2)) + (0.1269608587026596)*(((sumValueInList_BCS_strict_deltaE2_deltaE-(-0.5622677670186995))/(0.2934337790981981))*((sumValueInList_BCS_strict_deltaE2_Mbc-(2.970287399442505))/(0.5417830982981453))) + (-0.1263502687215805)*(((sumValueInList_BCS_strict_deltaE2_deltaE-(-0.5622677670186995))/(0.2934337790981981))*((sumValueInList_BCS_strict_dM2_M-(1.812249086670265))/(0.3316852184507579))) + (0.03324685618281364)*(((sumValueInList_BCS_strict_deltaE2_deltaE-(-0.5622677670186995))/(0.2934337790981981))*((sumValueInList_BCS_strict_dM2_deltaE-(-0.6152694269953234))/(0.2907571499778808))) + (0.1255383491516113)*(((sumValueInList_BCS_strict_deltaE2_deltaE-(-0.5622677670186995))/(0.2934337790981981))*((sumValueInList_BCS_strict_dM2_Mbc-(3.027244666078906))/(0.5255793101404719))) + (-0.0570482537150383)*(((sumValueInList_BCS_strict_deltaE2_deltaE-(-0.5622677670186995))/(0.2934337790981981))*((sumValueInList_BCS_strict_dM2_daughterHighest_p-(3.143958461381385))/(0.9465581819457167))) + (-0.1308189779520035)*((((sumValueInList_BCS_strict_deltaE2_Mbc-(2.970287399442505))/(0.5417830982981453))^2)) + (0.0312504805624485)*(((sumValueInList_BCS_strict_deltaE2_Mbc-(2.970287399442505))/(0.5417830982981453))*((sumValueInList_BCS_strict_dM2_M-(1.812249086670265))/(0.3316852184507579))) + (0.1253832578659058)*(((sumValueInList_BCS_strict_deltaE2_Mbc-(2.970287399442505))/(0.5417830982981453))*((sumValueInList_BCS_strict_dM2_deltaE-(-0.6152694269953234))/(0.2907571499778808))) + (-0.1802378445863724)*(((sumValueInList_BCS_strict_deltaE2_Mbc-(2.970287399442505))/(0.5417830982981453))*((sumValueInList_BCS_strict_dM2_Mbc-(3.027244666078906))/(0.5255793101404719))) + (-0.01594977267086506)*(((sumValueInList_BCS_strict_deltaE2_Mbc-(2.970287399442505))/(0.5417830982981453))*((sumValueInList_BCS_strict_dM2_daughterHighest_p-(3.143958461381385))/(0.9465581819457167))) + (-1.016951203346252)*((((sumValueInList_BCS_strict_dM2_M-(1.812249086670265))/(0.3316852184507579))^2)) + (-0.2500491440296173)*(((sumValueInList_BCS_strict_dM2_M-(1.812249086670265))/(0.3316852184507579))*((sumValueInList_BCS_strict_dM2_deltaE-(-0.6152694269953234))/(0.2907571499778808))) + (-0.2511942684650421)*(((sumValueInList_BCS_strict_dM2_M-(1.812249086670265))/(0.3316852184507579))*((sumValueInList_BCS_strict_dM2_Mbc-(3.027244666078906))/(0.5255793101404719))) + (-0.04930664598941803)*(((sumValueInList_BCS_strict_dM2_M-(1.812249086670265))/(0.3316852184507579))*((sumValueInList_BCS_strict_dM2_daughterHighest_p-(3.143958461381385))/(0.9465581819457167))) + (0.1606395840644836)*((((sumValueInList_BCS_strict_dM2_deltaE-(-0.6152694269953234))/(0.2907571499778808))^2)) + (-0.08500049263238907)*(((sumValueInList_BCS_strict_dM2_deltaE-(-0.6152694269953234))/(0.2907571499778808))*((sumValueInList_BCS_strict_dM2_Mbc-(3.027244666078906))/(0.5255793101404719))) + (-0.06294024735689163)*(((sumValueInList_BCS_strict_dM2_deltaE-(-0.6152694269953234))/(0.2907571499778808))*((sumValueInList_BCS_strict_dM2_daughterHighest_p-(3.143958461381385))/(0.9465581819457167))) + (-0.145693764090538)*((((sumValueInList_BCS_strict_dM2_Mbc-(3.027244666078906))/(0.5255793101404719))^2)) + (-0.01568492688238621)*(((sumValueInList_BCS_strict_dM2_Mbc-(3.027244666078906))/(0.5255793101404719))*((sumValueInList_BCS_strict_dM2_daughterHighest_p-(3.143958461381385))/(0.9465581819457167))) + (0.01505881175398827)*((((sumValueInList_BCS_strict_dM2_daughterHighest_p-(3.143958461381385))/(0.9465581819457167))^2)))"
    )

    # event cut
    ma.applyEventCuts("[nParticlesInList(tau+:fake_strict2) > 0.5] and [LR_score_A_quad > 2.398015901795199]", path=path)

# setting
parser = argparse.ArgumentParser(description='setting')
parser.add_argument('--output', required=True, type=str, help='output file')
args = parser.parse_args()

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
main.add_module("EventInfoSetter", expList=1004, runList=0, evtNumList=4000000)

# generate uubar events
ge.add_continuum_generator(path=main, finalstate='uubar', eventType='uubar')

# generator level cut
AnalysisGenCut(path=main)

# detector simulation
si.add_simulation(path=main, bkgfiles=bg_local)

# reconstruction
re.add_reconstruction(path=main)

# Finally add mdst output (file name overwritten on the grid)
mdst.add_mdst_output(path=main, filename=args.output)

# process events and print call statistics
b2.process(path=main)
print(b2.statistics)
