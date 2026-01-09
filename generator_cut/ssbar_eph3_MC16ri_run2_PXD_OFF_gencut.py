#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Descriptor: ssbar

#############################################################
# Steering file for official MC production of early phase 3
# 'ssbar' samples with beam backgrounds (BGx1).
#
# September 2020 - Belle II Collaboration
#############################################################

import basf2 as b2
import generators as ge
import simulation as si
import reconstruction as re
import mdst as mdst
import glob as glob

#============================================================
# Generator level cut for SSBAR background
# This reduces 54% of entire sample
# but keeps 98% of signal-like ssbar
#============================================================
def GenCutSSBAR(path):
    import modularAnalysis as ma
    import variables as va

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

    # reconstruct tau
    cut_mumumu = "[M < formula(2.25 * deltaE + 5.975)] and [-1.1 < deltaE]"
    ma.reconstructDecay(decayString="tau+:fake1 -> pi+:PrimaryMC pi-:PrimaryMC pi+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake2 -> pi+:PrimaryMC pi-:PrimaryMC mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake3 -> pi+:PrimaryMC mu-:PrimaryMC pi+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake4 -> pi+:PrimaryMC mu-:PrimaryMC mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake5 -> mu+:PrimaryMC pi-:PrimaryMC mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake6 -> mu+:PrimaryMC mu-:PrimaryMC mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake7 -> pi+:PrimaryMC pi-:PrimaryMC K+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake8 -> pi+:PrimaryMC K-:PrimaryMC pi+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake9 -> pi+:PrimaryMC K-:PrimaryMC K+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake10 -> K+:PrimaryMC pi-:PrimaryMC K+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake11 -> K+:PrimaryMC K-:PrimaryMC K+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake12 -> pi+:PrimaryMC pi-:PrimaryMC e+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake13 -> pi+:PrimaryMC e-:PrimaryMC pi+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake14 -> pi+:PrimaryMC e-:PrimaryMC e+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake15 -> e+:PrimaryMC pi-:PrimaryMC e+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake16 -> e+:PrimaryMC e-:PrimaryMC e+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake17 -> pi+:PrimaryMC pi-:PrimaryMC p+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake18 -> pi+:PrimaryMC anti-p-:PrimaryMC pi+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake19 -> pi+:PrimaryMC anti-p-:PrimaryMC p+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake20 -> p+:PrimaryMC pi-:PrimaryMC p+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake21 -> p+:PrimaryMC anti-p-:PrimaryMC p+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake22 -> mu+:PrimaryMC mu-:PrimaryMC p+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake23 -> mu+:PrimaryMC anti-p-:PrimaryMC mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake24 -> mu+:PrimaryMC anti-p-:PrimaryMC p+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake25 -> p+:PrimaryMC mu-:PrimaryMC p+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake26 -> mu+:PrimaryMC anti-p-:PrimaryMC pi+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake27 -> mu+:PrimaryMC pi-:PrimaryMC p+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake28 -> p+:PrimaryMC mu-:PrimaryMC pi+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake29 -> pi+:PrimaryMC K-:PrimaryMC p+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake30 -> pi+:PrimaryMC anti-p-:PrimaryMC K+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake31 -> K+:PrimaryMC pi-:PrimaryMC p+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake32 -> K+:PrimaryMC K-:PrimaryMC mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake33 -> K+:PrimaryMC mu-:PrimaryMC K+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake34 -> K+:PrimaryMC mu-:PrimaryMC mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake35 -> mu+:PrimaryMC K-:PrimaryMC mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake36 -> K+:PrimaryMC mu-:PrimaryMC pi+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake37 -> K+:PrimaryMC pi-:PrimaryMC mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake38 -> mu+:PrimaryMC K-:PrimaryMC pi+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake39 -> p+:PrimaryMC anti-p-:PrimaryMC K+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake40 -> p+:PrimaryMC K-:PrimaryMC p+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake41 -> K+:PrimaryMC anti-p-:PrimaryMC K+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake42 -> K+:PrimaryMC K-:PrimaryMC p+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake43 -> K+:PrimaryMC e-:PrimaryMC K+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake44 -> K+:PrimaryMC K-:PrimaryMC e+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake45 -> K+:PrimaryMC e-:PrimaryMC e+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake46 -> e+:PrimaryMC K-:PrimaryMC e+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake47 -> anti-Xi+:PrimaryMC pi-:PrimaryMC pi+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake48 -> pi+:PrimaryMC Xi-:PrimaryMC pi+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.copyLists(outputListName="tau+:fake", inputListNames=[f"tau+:fake{i}" for i in range(1, 49)], path=path)

    # event kinematics
    ma.buildEventKinematicsFromMC(inputListNames=None, selectionCut='', path=path)
    ma.buildEventShape(inputListNames=["pi+:PrimaryMC", "K+:PrimaryMC", "e+:PrimaryMC", "mu+:PrimaryMC", "p+:PrimaryMC", "gamma:PrimaryMC"], default_cleanup=False, path=path)
    ma.buildRestOfEventFromMC("Z0:PrimaryMC", inputParticlelists=["pi+:PrimaryMC", "K+:PrimaryMC", "e+:PrimaryMC", "mu+:PrimaryMC", "p+:PrimaryMC", "gamma:PrimaryMC"], path=path)
    ma.appendROEMasks(list_name="Z0:PrimaryMC", mask_tuples=[("cleanMask_gencut","","")], path=path)
    ma.buildContinuumSuppression(list_name="Z0:PrimaryMC", roe_mask="cleanMask_gencut", path=path)

    # define variables for 2nd order logistic regression
    va.variables.addAlias("Ntrack_gencut","formula(nParticlesInList(pi+:PrimaryMC) + nParticlesInList(K+:PrimaryMC) + nParticlesInList(e+:PrimaryMC) + nParticlesInList(mu+:PrimaryMC) + nParticlesInList(p+:PrimaryMC))")
    va.variables.addAlias("Ntau_gencut","nParticlesInList(tau+:fake)")
    va.variables.addAlias("Net_gencut","averageValueInList(Z0:PrimaryMC,KSFWVariables(et))")
    va.variables.addAlias("NTBz_gencut","averageValueInList(Z0:PrimaryMC,cosTBz)")

    # calculate LR output
    va.variables.addAlias(
        "LL_score_A_quad",
        "formula(0.688475*(((thrust-0.874093)/0.061364)) + 0.682274*(((thrustAxisCosTheta-0.454278)/0.470417)) + 0.479700*(((genTotalPhotonsEnergyOfEvent-2.030605)/1.515041)) + 0.545048*(((Net_gencut-6.874306)/2.024900)) + -1.398790*(((sphericity-0.137271)/0.107704)) + -0.430421*(((aplanarity-0.031305)/0.028408)) + 1.465907*(((Ntau_gencut-6.941108)/5.435570)) + -1.118236*(((Ntrack_gencut-6.627173)/1.904388)) + 0.059231*(((NTBz_gencut-0.571692)/0.290518)) + 0.451229*(((foxWolframR1-0.033908)/0.091783)) + -1.190476*(((foxWolframR2-0.531854)/0.170459)) + 0.228079*(((foxWolframR3-0.086205)/0.096889)) + 0.754324*(((thrust-0.874093)/0.061364)^2) + 0.150046*(((thrust-0.874093)/0.061364)*((thrustAxisCosTheta-0.454278)/0.470417)) + 0.164823*(((thrust-0.874093)/0.061364)*((genTotalPhotonsEnergyOfEvent-2.030605)/1.515041)) + -0.021071*(((thrust-0.874093)/0.061364)*((Net_gencut-6.874306)/2.024900)) + 0.652112*(((thrust-0.874093)/0.061364)*((sphericity-0.137271)/0.107704)) + 0.046141*(((thrust-0.874093)/0.061364)*((aplanarity-0.031305)/0.028408)) + 0.128199*(((thrust-0.874093)/0.061364)*((Ntau_gencut-6.941108)/5.435570)) + 0.224614*(((thrust-0.874093)/0.061364)*((Ntrack_gencut-6.627173)/1.904388)) + 0.073516*(((thrust-0.874093)/0.061364)*((NTBz_gencut-0.571692)/0.290518)) + 0.026753*(((thrust-0.874093)/0.061364)*((foxWolframR1-0.033908)/0.091783)) + -1.321176*(((thrust-0.874093)/0.061364)*((foxWolframR2-0.531854)/0.170459)) + 0.018260*(((thrust-0.874093)/0.061364)*((foxWolframR3-0.086205)/0.096889)) + 0.388810*(((thrustAxisCosTheta-0.454278)/0.470417)^2) + -0.024294*(((thrustAxisCosTheta-0.454278)/0.470417)*((genTotalPhotonsEnergyOfEvent-2.030605)/1.515041)) + 0.060448*(((thrustAxisCosTheta-0.454278)/0.470417)*((Net_gencut-6.874306)/2.024900)) + -0.026727*(((thrustAxisCosTheta-0.454278)/0.470417)*((sphericity-0.137271)/0.107704)) + 0.003502*(((thrustAxisCosTheta-0.454278)/0.470417)*((aplanarity-0.031305)/0.028408)) + -0.012832*(((thrustAxisCosTheta-0.454278)/0.470417)*((Ntau_gencut-6.941108)/5.435570)) + 0.054296*(((thrustAxisCosTheta-0.454278)/0.470417)*((Ntrack_gencut-6.627173)/1.904388)) + 0.051889*(((thrustAxisCosTheta-0.454278)/0.470417)*((NTBz_gencut-0.571692)/0.290518)) + 0.027005*(((thrustAxisCosTheta-0.454278)/0.470417)*((foxWolframR1-0.033908)/0.091783)) + -0.184410*(((thrustAxisCosTheta-0.454278)/0.470417)*((foxWolframR2-0.531854)/0.170459)) + -0.039771*(((thrustAxisCosTheta-0.454278)/0.470417)*((foxWolframR3-0.086205)/0.096889)) + 0.006914*(((genTotalPhotonsEnergyOfEvent-2.030605)/1.515041)^2) + -0.056136*(((genTotalPhotonsEnergyOfEvent-2.030605)/1.515041)*((Net_gencut-6.874306)/2.024900)) + -0.064021*(((genTotalPhotonsEnergyOfEvent-2.030605)/1.515041)*((sphericity-0.137271)/0.107704)) + -0.022758*(((genTotalPhotonsEnergyOfEvent-2.030605)/1.515041)*((aplanarity-0.031305)/0.028408)) + 0.170785*(((genTotalPhotonsEnergyOfEvent-2.030605)/1.515041)*((Ntau_gencut-6.941108)/5.435570)) + -0.028673*(((genTotalPhotonsEnergyOfEvent-2.030605)/1.515041)*((Ntrack_gencut-6.627173)/1.904388)) + -0.086116*(((genTotalPhotonsEnergyOfEvent-2.030605)/1.515041)*((NTBz_gencut-0.571692)/0.290518)) + -0.110490*(((genTotalPhotonsEnergyOfEvent-2.030605)/1.515041)*((foxWolframR1-0.033908)/0.091783)) + -0.183942*(((genTotalPhotonsEnergyOfEvent-2.030605)/1.515041)*((foxWolframR2-0.531854)/0.170459)) + 0.064403*(((genTotalPhotonsEnergyOfEvent-2.030605)/1.515041)*((foxWolframR3-0.086205)/0.096889)) + -0.527832*(((Net_gencut-6.874306)/2.024900)^2) + -0.330119*(((Net_gencut-6.874306)/2.024900)*((sphericity-0.137271)/0.107704)) + 0.087889*(((Net_gencut-6.874306)/2.024900)*((aplanarity-0.031305)/0.028408)) + 0.131534*(((Net_gencut-6.874306)/2.024900)*((Ntau_gencut-6.941108)/5.435570)) + -0.182270*(((Net_gencut-6.874306)/2.024900)*((Ntrack_gencut-6.627173)/1.904388)) + -0.343359*(((Net_gencut-6.874306)/2.024900)*((NTBz_gencut-0.571692)/0.290518)) + 0.186536*(((Net_gencut-6.874306)/2.024900)*((foxWolframR1-0.033908)/0.091783)) + -0.309720*(((Net_gencut-6.874306)/2.024900)*((foxWolframR2-0.531854)/0.170459)) + -0.128621*(((Net_gencut-6.874306)/2.024900)*((foxWolframR3-0.086205)/0.096889)) + 0.045443*(((sphericity-0.137271)/0.107704)^2) + -0.122076*(((sphericity-0.137271)/0.107704)*((aplanarity-0.031305)/0.028408)) + -0.133279*(((sphericity-0.137271)/0.107704)*((Ntau_gencut-6.941108)/5.435570)) + 0.168947*(((sphericity-0.137271)/0.107704)*((Ntrack_gencut-6.627173)/1.904388)) + -0.172605*(((sphericity-0.137271)/0.107704)*((NTBz_gencut-0.571692)/0.290518)) + -0.023253*(((sphericity-0.137271)/0.107704)*((foxWolframR1-0.033908)/0.091783)) + -1.245427*(((sphericity-0.137271)/0.107704)*((foxWolframR2-0.531854)/0.170459)) + -0.058912*(((sphericity-0.137271)/0.107704)*((foxWolframR3-0.086205)/0.096889)) + 0.052503*(((aplanarity-0.031305)/0.028408)^2) + -0.016845*(((aplanarity-0.031305)/0.028408)*((Ntau_gencut-6.941108)/5.435570)) + 0.015595*(((aplanarity-0.031305)/0.028408)*((Ntrack_gencut-6.627173)/1.904388)) + 0.073181*(((aplanarity-0.031305)/0.028408)*((NTBz_gencut-0.571692)/0.290518)) + 0.057057*(((aplanarity-0.031305)/0.028408)*((foxWolframR1-0.033908)/0.091783)) + -0.218552*(((aplanarity-0.031305)/0.028408)*((foxWolframR2-0.531854)/0.170459)) + -0.035951*(((aplanarity-0.031305)/0.028408)*((foxWolframR3-0.086205)/0.096889)) + -0.055076*(((Ntau_gencut-6.941108)/5.435570)^2) + -0.252578*(((Ntau_gencut-6.941108)/5.435570)*((Ntrack_gencut-6.627173)/1.904388)) + 0.000990*(((Ntau_gencut-6.941108)/5.435570)*((NTBz_gencut-0.571692)/0.290518)) + 0.195710*(((Ntau_gencut-6.941108)/5.435570)*((foxWolframR1-0.033908)/0.091783)) + 0.032740*(((Ntau_gencut-6.941108)/5.435570)*((foxWolframR2-0.531854)/0.170459)) + 0.123992*(((Ntau_gencut-6.941108)/5.435570)*((foxWolframR3-0.086205)/0.096889)) + -0.160279*(((Ntrack_gencut-6.627173)/1.904388)^2) + 0.066911*(((Ntrack_gencut-6.627173)/1.904388)*((NTBz_gencut-0.571692)/0.290518)) + 0.043058*(((Ntrack_gencut-6.627173)/1.904388)*((foxWolframR1-0.033908)/0.091783)) + -0.234710*(((Ntrack_gencut-6.627173)/1.904388)*((foxWolframR2-0.531854)/0.170459)) + -0.024040*(((Ntrack_gencut-6.627173)/1.904388)*((foxWolframR3-0.086205)/0.096889)) + 0.002077*(((NTBz_gencut-0.571692)/0.290518)^2) + 0.090692*(((NTBz_gencut-0.571692)/0.290518)*((foxWolframR1-0.033908)/0.091783)) + -0.134756*(((NTBz_gencut-0.571692)/0.290518)*((foxWolframR2-0.531854)/0.170459)) + -0.014620*(((NTBz_gencut-0.571692)/0.290518)*((foxWolframR3-0.086205)/0.096889)) + -0.020165*(((foxWolframR1-0.033908)/0.091783)^2) + -0.009006*(((foxWolframR1-0.033908)/0.091783)*((foxWolframR2-0.531854)/0.170459)) + 0.005472*(((foxWolframR1-0.033908)/0.091783)*((foxWolframR3-0.086205)/0.096889)) + 0.208114*(((foxWolframR2-0.531854)/0.170459)^2) + -0.118869*(((foxWolframR2-0.531854)/0.170459)*((foxWolframR3-0.086205)/0.096889)) + -0.045294*(((foxWolframR3-0.086205)/0.096889)^2) + (-0.777123))"
    )
    va.variables.addAlias(
        "LL_score_B_quad",
        "formula(2.015937*(((thrust-0.830651)/0.092701)) + 0.319853*(((thrustAxisCosTheta-0.566195)/0.471018)) + -0.350213*(((genTotalPhotonsEnergyOfEvent-3.833066)/2.122493)) + 0.313205*(((Net_gencut-5.582483)/2.498193)) + -2.005009*(((sphericity-0.258945)/0.184140)) + -1.050747*(((aplanarity-0.066221)/0.058837)) + -0.907080*(((Ntrack_gencut-6.387370)/3.123586)) + 0.050686*(((NTBz_gencut-0.635681)/0.307935)) + -0.940647*(((foxWolframR1-0.071007)/0.149839)) + -4.080751*(((foxWolframR2-0.429132)/0.245784)) + 0.064929*(((foxWolframR3-0.120957)/0.139974)) + 0.678090*(((thrust-0.830651)/0.092701)^2) + 0.196820*(((thrust-0.830651)/0.092701)*((thrustAxisCosTheta-0.566195)/0.471018)) + 0.242622*(((thrust-0.830651)/0.092701)*((genTotalPhotonsEnergyOfEvent-3.833066)/2.122493)) + -0.884533*(((thrust-0.830651)/0.092701)*((Net_gencut-5.582483)/2.498193)) + 0.677447*(((thrust-0.830651)/0.092701)*((sphericity-0.258945)/0.184140)) + 0.451080*(((thrust-0.830651)/0.092701)*((aplanarity-0.066221)/0.058837)) + 0.769300*(((thrust-0.830651)/0.092701)*((Ntrack_gencut-6.387370)/3.123586)) + -0.314610*(((thrust-0.830651)/0.092701)*((NTBz_gencut-0.635681)/0.307935)) + 0.995017*(((thrust-0.830651)/0.092701)*((foxWolframR1-0.071007)/0.149839)) + -0.448856*(((thrust-0.830651)/0.092701)*((foxWolframR2-0.429132)/0.245784)) + -0.241481*(((thrust-0.830651)/0.092701)*((foxWolframR3-0.120957)/0.139974)) + 0.137930*(((thrustAxisCosTheta-0.566195)/0.471018)^2) + -0.082904*(((thrustAxisCosTheta-0.566195)/0.471018)*((genTotalPhotonsEnergyOfEvent-3.833066)/2.122493)) + -0.064893*(((thrustAxisCosTheta-0.566195)/0.471018)*((Net_gencut-5.582483)/2.498193)) + 0.028660*(((thrustAxisCosTheta-0.566195)/0.471018)*((sphericity-0.258945)/0.184140)) + 0.062832*(((thrustAxisCosTheta-0.566195)/0.471018)*((aplanarity-0.066221)/0.058837)) + -0.107616*(((thrustAxisCosTheta-0.566195)/0.471018)*((Ntrack_gencut-6.387370)/3.123586)) + -0.072507*(((thrustAxisCosTheta-0.566195)/0.471018)*((NTBz_gencut-0.635681)/0.307935)) + -0.023726*(((thrustAxisCosTheta-0.566195)/0.471018)*((foxWolframR1-0.071007)/0.149839)) + -0.125790*(((thrustAxisCosTheta-0.566195)/0.471018)*((foxWolframR2-0.429132)/0.245784)) + -0.056323*(((thrustAxisCosTheta-0.566195)/0.471018)*((foxWolframR3-0.120957)/0.139974)) + -0.345910*(((genTotalPhotonsEnergyOfEvent-3.833066)/2.122493)^2) + -0.001375*(((genTotalPhotonsEnergyOfEvent-3.833066)/2.122493)*((Net_gencut-5.582483)/2.498193)) + 0.247693*(((genTotalPhotonsEnergyOfEvent-3.833066)/2.122493)*((sphericity-0.258945)/0.184140)) + 0.006491*(((genTotalPhotonsEnergyOfEvent-3.833066)/2.122493)*((aplanarity-0.066221)/0.058837)) + -0.262076*(((genTotalPhotonsEnergyOfEvent-3.833066)/2.122493)*((Ntrack_gencut-6.387370)/3.123586)) + -0.037149*(((genTotalPhotonsEnergyOfEvent-3.833066)/2.122493)*((NTBz_gencut-0.635681)/0.307935)) + -0.431109*(((genTotalPhotonsEnergyOfEvent-3.833066)/2.122493)*((foxWolframR1-0.071007)/0.149839)) + 0.014388*(((genTotalPhotonsEnergyOfEvent-3.833066)/2.122493)*((foxWolframR2-0.429132)/0.245784)) + -0.000396*(((genTotalPhotonsEnergyOfEvent-3.833066)/2.122493)*((foxWolframR3-0.120957)/0.139974)) + -0.840431*(((Net_gencut-5.582483)/2.498193)^2) + -1.115204*(((Net_gencut-5.582483)/2.498193)*((sphericity-0.258945)/0.184140)) + 0.255806*(((Net_gencut-5.582483)/2.498193)*((aplanarity-0.066221)/0.058837)) + -0.609160*(((Net_gencut-5.582483)/2.498193)*((Ntrack_gencut-6.387370)/3.123586)) + -0.499549*(((Net_gencut-5.582483)/2.498193)*((NTBz_gencut-0.635681)/0.307935)) + 0.231173*(((Net_gencut-5.582483)/2.498193)*((foxWolframR1-0.071007)/0.149839)) + 0.120763*(((Net_gencut-5.582483)/2.498193)*((foxWolframR2-0.429132)/0.245784)) + -0.332032*(((Net_gencut-5.582483)/2.498193)*((foxWolframR3-0.120957)/0.139974)) + 0.288569*(((sphericity-0.258945)/0.184140)^2) + -0.556852*(((sphericity-0.258945)/0.184140)*((aplanarity-0.066221)/0.058837)) + -0.469543*(((sphericity-0.258945)/0.184140)*((Ntrack_gencut-6.387370)/3.123586)) + -0.321532*(((sphericity-0.258945)/0.184140)*((NTBz_gencut-0.635681)/0.307935)) + -0.040658*(((sphericity-0.258945)/0.184140)*((foxWolframR1-0.071007)/0.149839)) + -1.545200*(((sphericity-0.258945)/0.184140)*((foxWolframR2-0.429132)/0.245784)) + 0.010990*(((sphericity-0.258945)/0.184140)*((foxWolframR3-0.120957)/0.139974)) + 0.167953*(((aplanarity-0.066221)/0.058837)^2) + 0.315926*(((aplanarity-0.066221)/0.058837)*((Ntrack_gencut-6.387370)/3.123586)) + 0.119018*(((aplanarity-0.066221)/0.058837)*((NTBz_gencut-0.635681)/0.307935)) + -0.080312*(((aplanarity-0.066221)/0.058837)*((foxWolframR1-0.071007)/0.149839)) + -1.085098*(((aplanarity-0.066221)/0.058837)*((foxWolframR2-0.429132)/0.245784)) + 0.004962*(((aplanarity-0.066221)/0.058837)*((foxWolframR3-0.120957)/0.139974)) + -0.524705*(((Ntrack_gencut-6.387370)/3.123586)^2) + -0.050522*(((Ntrack_gencut-6.387370)/3.123586)*((NTBz_gencut-0.635681)/0.307935)) + 0.077083*(((Ntrack_gencut-6.387370)/3.123586)*((foxWolframR1-0.071007)/0.149839)) + -1.123251*(((Ntrack_gencut-6.387370)/3.123586)*((foxWolframR2-0.429132)/0.245784)) + -0.056772*(((Ntrack_gencut-6.387370)/3.123586)*((foxWolframR3-0.120957)/0.139974)) + -0.141221*(((NTBz_gencut-0.635681)/0.307935)^2) + 0.183646*(((NTBz_gencut-0.635681)/0.307935)*((foxWolframR1-0.071007)/0.149839)) + 0.148746*(((NTBz_gencut-0.635681)/0.307935)*((foxWolframR2-0.429132)/0.245784)) + -0.186372*(((NTBz_gencut-0.635681)/0.307935)*((foxWolframR3-0.120957)/0.139974)) + 0.055138*(((foxWolframR1-0.071007)/0.149839)^2) + -1.286464*(((foxWolframR1-0.071007)/0.149839)*((foxWolframR2-0.429132)/0.245784)) + 0.134835*(((foxWolframR1-0.071007)/0.149839)*((foxWolframR3-0.120957)/0.139974)) + -0.273100*(((foxWolframR2-0.429132)/0.245784)^2) + 0.510475*(((foxWolframR2-0.429132)/0.245784)*((foxWolframR3-0.120957)/0.139974)) + -0.071465*(((foxWolframR3-0.120957)/0.139974)^2) + (-1.077501))"
    )

    # event cut
    ma.applyEventCuts("[[nParticlesInList(tau+:fake) > 0.5] and [LL_score_A_quad > -3.18463]] or [[nParticlesInList(tau+:fake) < 0.5] and [LL_score_B_quad > 0.38876]]", path=path)

# background (collision) files
bg = glob.glob('./*.root')
# background if running locally
bg_local = glob.glob('/group/belle2/dataprod/BGOverlay/run2/prerelease-08-00-00a/new_overlay/BGx1/set?/*root')
# switching to CVMFS as primary metadata provider.
b2.conditions.metadata_providers = ['/cvmfs/belle.cern.ch/conditions/database.sqlite']

# set database conditions (in addition to default)
# b2.conditions.prepend_globaltag("release-08-00-09")

b2.conditions.prepend_globaltag("tracking_PXD-off")
# create path
main = b2.Path()

# default to early phase 3 (exp=1003), run 0
main.add_module("EventInfoSetter", expList=1004, runList=0, evtNumList=10000)

# generate ssbar events
ge.add_continuum_generator(path=main, finalstate='ssbar', eventType='ssbar')

# generator level cut
GenCutSSBAR(path=main)

# detector simulation
si.add_simulation(path=main, bkgfiles=bg)

# reconstruction
re.add_reconstruction(path=main)

# Finally add mdst output (file name overwritten on the grid)
mdst.add_mdst_output(path=main, filename="mdst.root")

# produce systematic skims
from skim import BaseSkim, CombinedSkim
import skim.WGs.systematics as systematics

#skim_sclm = systematics.SystematicsCombinedLowMulti()
#skim_sclm(path=main)

skim_sch = systematics.SystematicsCombinedHadronic()
skim_sch(path=main)

# process events and print call statistics
b2.process(path=main)
print(b2.statistics)
