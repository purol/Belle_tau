#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Descriptor: ddbar

#############################################################
# Steering file for official MC production of early phase 3
# 'ddbar' samples with beam backgrounds (BGx1).
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
# Generator level cut for DDBAR background
# This reduces 73% of entire sample
# but keeps 98% of signal-like ddbar
#============================================================
def GenCutDDBAR(path):
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
    cut_mumumu = "[M < formula(1.43 * deltaE + 4.57)] and [-1.1 < deltaE]"
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
        "formula(0.599509*(((thrust-0.879466)/0.055152)) + 0.744846*(((thrustAxisCosTheta-0.494913)/0.447313)) + 0.215717*(((genTotalPhotonsEnergyOfEvent-2.429146)/1.657484)) + 0.187271*(((Net_gencut-6.831006)/2.099294)) + -0.765951*(((sphericity-0.123883)/0.092319)) + -0.220018*(((aplanarity-0.028274)/0.024773)) + 1.514159*(((Ntau_gencut-5.064231)/4.322380)) + -1.179923*(((Ntrack_gencut-6.323897)/1.761332)) + -0.192636*(((NTBz_gencut-0.589775)/0.288338)) + 0.242786*(((foxWolframR1-0.024977)/0.086210)) + -0.839758*(((foxWolframR2-0.543564)/0.156709)) + -0.053193*(((foxWolframR3-0.074240)/0.088148)) + 0.018253*(((thrust-0.879466)/0.055152)^2) + 0.053534*(((thrust-0.879466)/0.055152)*((thrustAxisCosTheta-0.494913)/0.447313)) + 0.007897*(((thrust-0.879466)/0.055152)*((genTotalPhotonsEnergyOfEvent-2.429146)/1.657484)) + -0.526754*(((thrust-0.879466)/0.055152)*((Net_gencut-6.831006)/2.099294)) + 0.248347*(((thrust-0.879466)/0.055152)*((sphericity-0.123883)/0.092319)) + 0.083203*(((thrust-0.879466)/0.055152)*((aplanarity-0.028274)/0.024773)) + -0.212776*(((thrust-0.879466)/0.055152)*((Ntau_gencut-5.064231)/4.322380)) + 0.332060*(((thrust-0.879466)/0.055152)*((Ntrack_gencut-6.323897)/1.761332)) + -0.296844*(((thrust-0.879466)/0.055152)*((NTBz_gencut-0.589775)/0.288338)) + 0.083072*(((thrust-0.879466)/0.055152)*((foxWolframR1-0.024977)/0.086210)) + -0.163577*(((thrust-0.879466)/0.055152)*((foxWolframR2-0.543564)/0.156709)) + -0.240457*(((thrust-0.879466)/0.055152)*((foxWolframR3-0.074240)/0.088148)) + 0.357778*(((thrustAxisCosTheta-0.494913)/0.447313)^2) + -0.003053*(((thrustAxisCosTheta-0.494913)/0.447313)*((genTotalPhotonsEnergyOfEvent-2.429146)/1.657484)) + 0.102367*(((thrustAxisCosTheta-0.494913)/0.447313)*((Net_gencut-6.831006)/2.099294)) + -0.050742*(((thrustAxisCosTheta-0.494913)/0.447313)*((sphericity-0.123883)/0.092319)) + 0.019688*(((thrustAxisCosTheta-0.494913)/0.447313)*((aplanarity-0.028274)/0.024773)) + 0.004579*(((thrustAxisCosTheta-0.494913)/0.447313)*((Ntau_gencut-5.064231)/4.322380)) + 0.029218*(((thrustAxisCosTheta-0.494913)/0.447313)*((Ntrack_gencut-6.323897)/1.761332)) + 0.086724*(((thrustAxisCosTheta-0.494913)/0.447313)*((NTBz_gencut-0.589775)/0.288338)) + 0.014297*(((thrustAxisCosTheta-0.494913)/0.447313)*((foxWolframR1-0.024977)/0.086210)) + -0.138144*(((thrustAxisCosTheta-0.494913)/0.447313)*((foxWolframR2-0.543564)/0.156709)) + -0.035199*(((thrustAxisCosTheta-0.494913)/0.447313)*((foxWolframR3-0.074240)/0.088148)) + -0.145972*(((genTotalPhotonsEnergyOfEvent-2.429146)/1.657484)^2) + 0.003584*(((genTotalPhotonsEnergyOfEvent-2.429146)/1.657484)*((Net_gencut-6.831006)/2.099294)) + 0.026098*(((genTotalPhotonsEnergyOfEvent-2.429146)/1.657484)*((sphericity-0.123883)/0.092319)) + -0.038588*(((genTotalPhotonsEnergyOfEvent-2.429146)/1.657484)*((aplanarity-0.028274)/0.024773)) + 0.246480*(((genTotalPhotonsEnergyOfEvent-2.429146)/1.657484)*((Ntau_gencut-5.064231)/4.322380)) + -0.315322*(((genTotalPhotonsEnergyOfEvent-2.429146)/1.657484)*((Ntrack_gencut-6.323897)/1.761332)) + -0.070375*(((genTotalPhotonsEnergyOfEvent-2.429146)/1.657484)*((NTBz_gencut-0.589775)/0.288338)) + -0.279750*(((genTotalPhotonsEnergyOfEvent-2.429146)/1.657484)*((foxWolframR1-0.024977)/0.086210)) + -0.061142*(((genTotalPhotonsEnergyOfEvent-2.429146)/1.657484)*((foxWolframR2-0.543564)/0.156709)) + 0.035723*(((genTotalPhotonsEnergyOfEvent-2.429146)/1.657484)*((foxWolframR3-0.074240)/0.088148)) + -0.501704*(((Net_gencut-6.831006)/2.099294)^2) + -0.249851*(((Net_gencut-6.831006)/2.099294)*((sphericity-0.123883)/0.092319)) + 0.137740*(((Net_gencut-6.831006)/2.099294)*((aplanarity-0.028274)/0.024773)) + 0.077316*(((Net_gencut-6.831006)/2.099294)*((Ntau_gencut-5.064231)/4.322380)) + -0.056132*(((Net_gencut-6.831006)/2.099294)*((Ntrack_gencut-6.323897)/1.761332)) + -0.362897*(((Net_gencut-6.831006)/2.099294)*((NTBz_gencut-0.589775)/0.288338)) + 0.139151*(((Net_gencut-6.831006)/2.099294)*((foxWolframR1-0.024977)/0.086210)) + 0.299756*(((Net_gencut-6.831006)/2.099294)*((foxWolframR2-0.543564)/0.156709)) + -0.102881*(((Net_gencut-6.831006)/2.099294)*((foxWolframR3-0.074240)/0.088148)) + 0.088366*(((sphericity-0.123883)/0.092319)^2) + -0.036270*(((sphericity-0.123883)/0.092319)*((aplanarity-0.028274)/0.024773)) + 0.359201*(((sphericity-0.123883)/0.092319)*((Ntau_gencut-5.064231)/4.322380)) + 0.035807*(((sphericity-0.123883)/0.092319)*((Ntrack_gencut-6.323897)/1.761332)) + -0.140866*(((sphericity-0.123883)/0.092319)*((NTBz_gencut-0.589775)/0.288338)) + -0.020867*(((sphericity-0.123883)/0.092319)*((foxWolframR1-0.024977)/0.086210)) + -0.370994*(((sphericity-0.123883)/0.092319)*((foxWolframR2-0.543564)/0.156709)) + -0.012553*(((sphericity-0.123883)/0.092319)*((foxWolframR3-0.074240)/0.088148)) + 0.002606*(((aplanarity-0.028274)/0.024773)^2) + -0.025830*(((aplanarity-0.028274)/0.024773)*((Ntau_gencut-5.064231)/4.322380)) + -0.050285*(((aplanarity-0.028274)/0.024773)*((Ntrack_gencut-6.323897)/1.761332)) + 0.078456*(((aplanarity-0.028274)/0.024773)*((NTBz_gencut-0.589775)/0.288338)) + 0.001950*(((aplanarity-0.028274)/0.024773)*((foxWolframR1-0.024977)/0.086210)) + -0.212830*(((aplanarity-0.028274)/0.024773)*((foxWolframR2-0.543564)/0.156709)) + -0.038258*(((aplanarity-0.028274)/0.024773)*((foxWolframR3-0.074240)/0.088148)) + -0.096082*(((Ntau_gencut-5.064231)/4.322380)^2) + -0.029814*(((Ntau_gencut-5.064231)/4.322380)*((Ntrack_gencut-6.323897)/1.761332)) + -0.034678*(((Ntau_gencut-5.064231)/4.322380)*((NTBz_gencut-0.589775)/0.288338)) + 0.356372*(((Ntau_gencut-5.064231)/4.322380)*((foxWolframR1-0.024977)/0.086210)) + 0.481433*(((Ntau_gencut-5.064231)/4.322380)*((foxWolframR2-0.543564)/0.156709)) + 0.003347*(((Ntau_gencut-5.064231)/4.322380)*((foxWolframR3-0.074240)/0.088148)) + -0.448842*(((Ntrack_gencut-6.323897)/1.761332)^2) + 0.120655*(((Ntrack_gencut-6.323897)/1.761332)*((NTBz_gencut-0.589775)/0.288338)) + -0.117393*(((Ntrack_gencut-6.323897)/1.761332)*((foxWolframR1-0.024977)/0.086210)) + -0.482059*(((Ntrack_gencut-6.323897)/1.761332)*((foxWolframR2-0.543564)/0.156709)) + -0.070807*(((Ntrack_gencut-6.323897)/1.761332)*((foxWolframR3-0.074240)/0.088148)) + -0.164695*(((NTBz_gencut-0.589775)/0.288338)^2) + 0.059167*(((NTBz_gencut-0.589775)/0.288338)*((foxWolframR1-0.024977)/0.086210)) + 0.241508*(((NTBz_gencut-0.589775)/0.288338)*((foxWolframR2-0.543564)/0.156709)) + -0.003301*(((NTBz_gencut-0.589775)/0.288338)*((foxWolframR3-0.074240)/0.088148)) + -0.040713*(((foxWolframR1-0.024977)/0.086210)^2) + -0.101247*(((foxWolframR1-0.024977)/0.086210)*((foxWolframR2-0.543564)/0.156709)) + -0.003266*(((foxWolframR1-0.024977)/0.086210)*((foxWolframR3-0.074240)/0.088148)) + -0.074677*(((foxWolframR2-0.543564)/0.156709)^2) + 0.143110*(((foxWolframR2-0.543564)/0.156709)*((foxWolframR3-0.074240)/0.088148)) + -0.021794*(((foxWolframR3-0.074240)/0.088148)^2) + (0.024937))"
    )
    va.variables.addAlias(
        "LL_score_B_quad",
        "formula(1.915175*(((thrust-0.850365)/0.095081)) + -0.211527*(((thrustAxisCosTheta-0.596997)/0.453623)) + -1.087277*(((genTotalPhotonsEnergyOfEvent-4.604719)/2.441146)) + 0.404608*(((Net_gencut-5.330529)/2.764797)) + -1.150345*(((sphericity-0.221712)/0.178395)) + -1.256322*(((aplanarity-0.056776)/0.055064)) + -1.723358*(((Ntrack_gencut-5.973211)/3.172449)) + 0.704291*(((NTBz_gencut-0.656048)/0.309678)) + -1.093754*(((foxWolframR1-0.036796)/0.106628)) + -3.076509*(((foxWolframR2-0.485893)/0.269871)) + -0.161275*(((foxWolframR3-0.083291)/0.104152)) + 2.494187*(((thrust-0.850365)/0.095081)^2) + -0.086609*(((thrust-0.850365)/0.095081)*((thrustAxisCosTheta-0.596997)/0.453623)) + 0.306958*(((thrust-0.850365)/0.095081)*((genTotalPhotonsEnergyOfEvent-4.604719)/2.441146)) + -2.058114*(((thrust-0.850365)/0.095081)*((Net_gencut-5.330529)/2.764797)) + 0.288981*(((thrust-0.850365)/0.095081)*((sphericity-0.221712)/0.178395)) + 1.305463*(((thrust-0.850365)/0.095081)*((aplanarity-0.056776)/0.055064)) + 1.087804*(((thrust-0.850365)/0.095081)*((Ntrack_gencut-5.973211)/3.172449)) + -1.194828*(((thrust-0.850365)/0.095081)*((NTBz_gencut-0.656048)/0.309678)) + -0.003653*(((thrust-0.850365)/0.095081)*((foxWolframR1-0.036796)/0.106628)) + -4.054256*(((thrust-0.850365)/0.095081)*((foxWolframR2-0.485893)/0.269871)) + 0.441040*(((thrust-0.850365)/0.095081)*((foxWolframR3-0.083291)/0.104152)) + -0.028676*(((thrustAxisCosTheta-0.596997)/0.453623)^2) + 0.048092*(((thrustAxisCosTheta-0.596997)/0.453623)*((genTotalPhotonsEnergyOfEvent-4.604719)/2.441146)) + 0.188552*(((thrustAxisCosTheta-0.596997)/0.453623)*((Net_gencut-5.330529)/2.764797)) + 0.089222*(((thrustAxisCosTheta-0.596997)/0.453623)*((sphericity-0.221712)/0.178395)) + -0.367675*(((thrustAxisCosTheta-0.596997)/0.453623)*((aplanarity-0.056776)/0.055064)) + -0.010714*(((thrustAxisCosTheta-0.596997)/0.453623)*((Ntrack_gencut-5.973211)/3.172449)) + 0.004657*(((thrustAxisCosTheta-0.596997)/0.453623)*((NTBz_gencut-0.656048)/0.309678)) + 0.109999*(((thrustAxisCosTheta-0.596997)/0.453623)*((foxWolframR1-0.036796)/0.106628)) + -0.321548*(((thrustAxisCosTheta-0.596997)/0.453623)*((foxWolframR2-0.485893)/0.269871)) + -0.178469*(((thrustAxisCosTheta-0.596997)/0.453623)*((foxWolframR3-0.083291)/0.104152)) + -0.658240*(((genTotalPhotonsEnergyOfEvent-4.604719)/2.441146)^2) + -0.109552*(((genTotalPhotonsEnergyOfEvent-4.604719)/2.441146)*((Net_gencut-5.330529)/2.764797)) + 0.584441*(((genTotalPhotonsEnergyOfEvent-4.604719)/2.441146)*((sphericity-0.221712)/0.178395)) + -0.182092*(((genTotalPhotonsEnergyOfEvent-4.604719)/2.441146)*((aplanarity-0.056776)/0.055064)) + -0.470740*(((genTotalPhotonsEnergyOfEvent-4.604719)/2.441146)*((Ntrack_gencut-5.973211)/3.172449)) + -0.094628*(((genTotalPhotonsEnergyOfEvent-4.604719)/2.441146)*((NTBz_gencut-0.656048)/0.309678)) + -0.511779*(((genTotalPhotonsEnergyOfEvent-4.604719)/2.441146)*((foxWolframR1-0.036796)/0.106628)) + -0.044637*(((genTotalPhotonsEnergyOfEvent-4.604719)/2.441146)*((foxWolframR2-0.485893)/0.269871)) + -0.181317*(((genTotalPhotonsEnergyOfEvent-4.604719)/2.441146)*((foxWolframR3-0.083291)/0.104152)) + -1.763339*(((Net_gencut-5.330529)/2.764797)^2) + -0.481902*(((Net_gencut-5.330529)/2.764797)*((sphericity-0.221712)/0.178395)) + 0.559818*(((Net_gencut-5.330529)/2.764797)*((aplanarity-0.056776)/0.055064)) + -0.281020*(((Net_gencut-5.330529)/2.764797)*((Ntrack_gencut-5.973211)/3.172449)) + -1.423085*(((Net_gencut-5.330529)/2.764797)*((NTBz_gencut-0.656048)/0.309678)) + 0.163295*(((Net_gencut-5.330529)/2.764797)*((foxWolframR1-0.036796)/0.106628)) + 2.730464*(((Net_gencut-5.330529)/2.764797)*((foxWolframR2-0.485893)/0.269871)) + -0.126989*(((Net_gencut-5.330529)/2.764797)*((foxWolframR3-0.083291)/0.104152)) + 0.002394*(((sphericity-0.221712)/0.178395)^2) + 0.686109*(((sphericity-0.221712)/0.178395)*((aplanarity-0.056776)/0.055064)) + 0.128658*(((sphericity-0.221712)/0.178395)*((Ntrack_gencut-5.973211)/3.172449)) + -0.107611*(((sphericity-0.221712)/0.178395)*((NTBz_gencut-0.656048)/0.309678)) + -0.010813*(((sphericity-0.221712)/0.178395)*((foxWolframR1-0.036796)/0.106628)) + -0.739365*(((sphericity-0.221712)/0.178395)*((foxWolframR2-0.485893)/0.269871)) + 0.435184*(((sphericity-0.221712)/0.178395)*((foxWolframR3-0.083291)/0.104152)) + -0.378993*(((aplanarity-0.056776)/0.055064)^2) + -0.201445*(((aplanarity-0.056776)/0.055064)*((Ntrack_gencut-5.973211)/3.172449)) + 0.786616*(((aplanarity-0.056776)/0.055064)*((NTBz_gencut-0.656048)/0.309678)) + 0.209855*(((aplanarity-0.056776)/0.055064)*((foxWolframR1-0.036796)/0.106628)) + -1.800504*(((aplanarity-0.056776)/0.055064)*((foxWolframR2-0.485893)/0.269871)) + -0.724904*(((aplanarity-0.056776)/0.055064)*((foxWolframR3-0.083291)/0.104152)) + -0.465496*(((Ntrack_gencut-5.973211)/3.172449)^2) + 0.345235*(((Ntrack_gencut-5.973211)/3.172449)*((NTBz_gencut-0.656048)/0.309678)) + 0.082933*(((Ntrack_gencut-5.973211)/3.172449)*((foxWolframR1-0.036796)/0.106628)) + -1.722222*(((Ntrack_gencut-5.973211)/3.172449)*((foxWolframR2-0.485893)/0.269871)) + -0.129181*(((Ntrack_gencut-5.973211)/3.172449)*((foxWolframR3-0.083291)/0.104152)) + -0.317095*(((NTBz_gencut-0.656048)/0.309678)^2) + -0.004514*(((NTBz_gencut-0.656048)/0.309678)*((foxWolframR1-0.036796)/0.106628)) + 2.191376*(((NTBz_gencut-0.656048)/0.309678)*((foxWolframR2-0.485893)/0.269871)) + 0.148099*(((NTBz_gencut-0.656048)/0.309678)*((foxWolframR3-0.083291)/0.104152)) + 0.045689*(((foxWolframR1-0.036796)/0.106628)^2) + -0.056125*(((foxWolframR1-0.036796)/0.106628)*((foxWolframR2-0.485893)/0.269871)) + 0.115625*(((foxWolframR1-0.036796)/0.106628)*((foxWolframR3-0.083291)/0.104152)) + 1.452936*(((foxWolframR2-0.485893)/0.269871)^2) + -0.621393*(((foxWolframR2-0.485893)/0.269871)*((foxWolframR3-0.083291)/0.104152)) + -0.139031*(((foxWolframR3-0.083291)/0.104152)^2) + (-0.762818))"
    )

    # event cut
    ma.applyEventCuts("[[nParticlesInList(tau+:fake) > 0.5] and [LL_score_A_quad > -4.22129]] or [[nParticlesInList(tau+:fake) < 0.5] and [LL_score_B_quad > 0.76754]]", path=path)


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

# generate ddbar events
ge.add_continuum_generator(path=main, finalstate='ddbar', eventType='ddbar')

# generator level cut
GenCutDDBAR(path=main)

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
