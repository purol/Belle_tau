#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Descriptor: taupair

#############################################################
# Steering file for official MC production of early phase 3
# 'taupair' samples with beam backgrounds (BGx1).
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
# Generator level cut for taupair background
# This reduces 63% of entire sample
# but keeps 98% of signal-like taupair
#============================================================
def GenCutTAUPAIR(path):
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
    cut_mumumu = "[-1.1 < deltaE] and [M < formula(2.22 * deltaE + 5.21)] and [M < formula(1.37 * deltaE + 5.00)]"
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
    ma.reconstructDecay(decayString="tau+:fake49 -> e+:PrimaryMC e-:PrimaryMC mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake50 -> e+:PrimaryMC mu-:PrimaryMC e+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake51 -> e+:PrimaryMC mu-:PrimaryMC mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.reconstructDecay(decayString="tau+:fake52 -> mu+:PrimaryMC e-:PrimaryMC mu+:PrimaryMC", cut=cut_mumumu, path=path)
    ma.copyLists(outputListName="tau+:fake", inputListNames=[f"tau+:fake{i}" for i in range(1, 53)], path=path)

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
        "formula(-0.904754*(((thrust-0.956887)/0.029016)) + -0.064378*(((thrustAxisCosTheta-0.522645)/0.363745)) + 0.194451*(((genTotalPhotonsEnergyOfEvent-1.029538)/1.289912)) + -0.065279*(((Net_gencut-7.283356)/2.224453)) + -1.259128*(((sphericity-0.051990)/0.050292)) + -0.389614*(((aplanarity-0.008852)/0.009578)) + 1.506793*(((Ntau_gencut-3.066888)/1.907056)) + -0.257262*(((Ntrack_gencut-4.252824)/0.670521)) + 0.274522*(((NTBz_gencut-0.560508)/0.281601)) + 2.840491*(((foxWolframR1-0.133878)/0.208048)) + -0.041525*(((foxWolframR2-0.795967)/0.103103)) + -0.238068*(((foxWolframR3-0.172558)/0.189523)) + 0.473195*(((harmonicMomentThrust0-0.722706)/0.127990)) + 0.049518*(((harmonicMomentThrust1-0.000653)/0.198908)) + 1.304087*(((thrust-0.956887)/0.029016)^2) + 0.097115*(((thrust-0.956887)/0.029016)*((thrustAxisCosTheta-0.522645)/0.363745)) + 0.019629*(((thrust-0.956887)/0.029016)*((genTotalPhotonsEnergyOfEvent-1.029538)/1.289912)) + -0.190213*(((thrust-0.956887)/0.029016)*((Net_gencut-7.283356)/2.224453)) + 0.778363*(((thrust-0.956887)/0.029016)*((sphericity-0.051990)/0.050292)) + 0.216756*(((thrust-0.956887)/0.029016)*((aplanarity-0.008852)/0.009578)) + -0.601367*(((thrust-0.956887)/0.029016)*((Ntau_gencut-3.066888)/1.907056)) + 0.017094*(((thrust-0.956887)/0.029016)*((Ntrack_gencut-4.252824)/0.670521)) + -0.280798*(((thrust-0.956887)/0.029016)*((NTBz_gencut-0.560508)/0.281601)) + 1.996144*(((thrust-0.956887)/0.029016)*((foxWolframR1-0.133878)/0.208048)) + -3.001438*(((thrust-0.956887)/0.029016)*((foxWolframR2-0.795967)/0.103103)) + -2.085527*(((thrust-0.956887)/0.029016)*((foxWolframR3-0.172558)/0.189523)) + 0.417931*(((thrust-0.956887)/0.029016)*((harmonicMomentThrust0-0.722706)/0.127990)) + 0.114831*(((thrust-0.956887)/0.029016)*((harmonicMomentThrust1-0.000653)/0.198908)) + 0.034859*(((thrustAxisCosTheta-0.522645)/0.363745)^2) + 0.013843*(((thrustAxisCosTheta-0.522645)/0.363745)*((genTotalPhotonsEnergyOfEvent-1.029538)/1.289912)) + 0.045735*(((thrustAxisCosTheta-0.522645)/0.363745)*((Net_gencut-7.283356)/2.224453)) + -0.020694*(((thrustAxisCosTheta-0.522645)/0.363745)*((sphericity-0.051990)/0.050292)) + 0.000029*(((thrustAxisCosTheta-0.522645)/0.363745)*((aplanarity-0.008852)/0.009578)) + -0.002628*(((thrustAxisCosTheta-0.522645)/0.363745)*((Ntau_gencut-3.066888)/1.907056)) + 0.039057*(((thrustAxisCosTheta-0.522645)/0.363745)*((Ntrack_gencut-4.252824)/0.670521)) + 0.062853*(((thrustAxisCosTheta-0.522645)/0.363745)*((NTBz_gencut-0.560508)/0.281601)) + 0.017432*(((thrustAxisCosTheta-0.522645)/0.363745)*((foxWolframR1-0.133878)/0.208048)) + -0.138917*(((thrustAxisCosTheta-0.522645)/0.363745)*((foxWolframR2-0.795967)/0.103103)) + 0.069372*(((thrustAxisCosTheta-0.522645)/0.363745)*((foxWolframR3-0.172558)/0.189523)) + 0.044191*(((thrustAxisCosTheta-0.522645)/0.363745)*((harmonicMomentThrust0-0.722706)/0.127990)) + -0.042118*(((thrustAxisCosTheta-0.522645)/0.363745)*((harmonicMomentThrust1-0.000653)/0.198908)) + 0.119402*(((genTotalPhotonsEnergyOfEvent-1.029538)/1.289912)^2) + 0.098629*(((genTotalPhotonsEnergyOfEvent-1.029538)/1.289912)*((Net_gencut-7.283356)/2.224453)) + -0.267704*(((genTotalPhotonsEnergyOfEvent-1.029538)/1.289912)*((sphericity-0.051990)/0.050292)) + -0.002163*(((genTotalPhotonsEnergyOfEvent-1.029538)/1.289912)*((aplanarity-0.008852)/0.009578)) + 0.239726*(((genTotalPhotonsEnergyOfEvent-1.029538)/1.289912)*((Ntau_gencut-3.066888)/1.907056)) + -0.077215*(((genTotalPhotonsEnergyOfEvent-1.029538)/1.289912)*((Ntrack_gencut-4.252824)/0.670521)) + 0.015631*(((genTotalPhotonsEnergyOfEvent-1.029538)/1.289912)*((NTBz_gencut-0.560508)/0.281601)) + -0.888560*(((genTotalPhotonsEnergyOfEvent-1.029538)/1.289912)*((foxWolframR1-0.133878)/0.208048)) + -0.110703*(((genTotalPhotonsEnergyOfEvent-1.029538)/1.289912)*((foxWolframR2-0.795967)/0.103103)) + 0.565663*(((genTotalPhotonsEnergyOfEvent-1.029538)/1.289912)*((foxWolframR3-0.172558)/0.189523)) + -0.200522*(((genTotalPhotonsEnergyOfEvent-1.029538)/1.289912)*((harmonicMomentThrust0-0.722706)/0.127990)) + -0.023942*(((genTotalPhotonsEnergyOfEvent-1.029538)/1.289912)*((harmonicMomentThrust1-0.000653)/0.198908)) + -0.796386*(((Net_gencut-7.283356)/2.224453)^2) + 0.066482*(((Net_gencut-7.283356)/2.224453)*((sphericity-0.051990)/0.050292)) + 0.007851*(((Net_gencut-7.283356)/2.224453)*((aplanarity-0.008852)/0.009578)) + 0.327385*(((Net_gencut-7.283356)/2.224453)*((Ntau_gencut-3.066888)/1.907056)) + -0.068731*(((Net_gencut-7.283356)/2.224453)*((Ntrack_gencut-4.252824)/0.670521)) + -0.803367*(((Net_gencut-7.283356)/2.224453)*((NTBz_gencut-0.560508)/0.281601)) + 0.428613*(((Net_gencut-7.283356)/2.224453)*((foxWolframR1-0.133878)/0.208048)) + 0.313635*(((Net_gencut-7.283356)/2.224453)*((foxWolframR2-0.795967)/0.103103)) + -0.477506*(((Net_gencut-7.283356)/2.224453)*((foxWolframR3-0.172558)/0.189523)) + -0.381909*(((Net_gencut-7.283356)/2.224453)*((harmonicMomentThrust0-0.722706)/0.127990)) + -0.370855*(((Net_gencut-7.283356)/2.224453)*((harmonicMomentThrust1-0.000653)/0.198908)) + -0.065775*(((sphericity-0.051990)/0.050292)^2) + -0.000452*(((sphericity-0.051990)/0.050292)*((aplanarity-0.008852)/0.009578)) + 0.250157*(((sphericity-0.051990)/0.050292)*((Ntau_gencut-3.066888)/1.907056)) + -0.086018*(((sphericity-0.051990)/0.050292)*((Ntrack_gencut-4.252824)/0.670521)) + 0.187950*(((sphericity-0.051990)/0.050292)*((NTBz_gencut-0.560508)/0.281601)) + 0.329265*(((sphericity-0.051990)/0.050292)*((foxWolframR1-0.133878)/0.208048)) + -1.456521*(((sphericity-0.051990)/0.050292)*((foxWolframR2-0.795967)/0.103103)) + -0.774844*(((sphericity-0.051990)/0.050292)*((foxWolframR3-0.172558)/0.189523)) + -0.006660*(((sphericity-0.051990)/0.050292)*((harmonicMomentThrust0-0.722706)/0.127990)) + -0.081655*(((sphericity-0.051990)/0.050292)*((harmonicMomentThrust1-0.000653)/0.198908)) + 0.045619*(((aplanarity-0.008852)/0.009578)^2) + -0.008755*(((aplanarity-0.008852)/0.009578)*((Ntau_gencut-3.066888)/1.907056)) + -0.023330*(((aplanarity-0.008852)/0.009578)*((Ntrack_gencut-4.252824)/0.670521)) + 0.053943*(((aplanarity-0.008852)/0.009578)*((NTBz_gencut-0.560508)/0.281601)) + 0.108273*(((aplanarity-0.008852)/0.009578)*((foxWolframR1-0.133878)/0.208048)) + -0.278632*(((aplanarity-0.008852)/0.009578)*((foxWolframR2-0.795967)/0.103103)) + -0.189485*(((aplanarity-0.008852)/0.009578)*((foxWolframR3-0.172558)/0.189523)) + -0.039237*(((aplanarity-0.008852)/0.009578)*((harmonicMomentThrust0-0.722706)/0.127990)) + -0.013937*(((aplanarity-0.008852)/0.009578)*((harmonicMomentThrust1-0.000653)/0.198908)) + 0.028415*(((Ntau_gencut-3.066888)/1.907056)^2) + -0.211162*(((Ntau_gencut-3.066888)/1.907056)*((Ntrack_gencut-4.252824)/0.670521)) + 0.243721*(((Ntau_gencut-3.066888)/1.907056)*((NTBz_gencut-0.560508)/0.281601)) + 0.613005*(((Ntau_gencut-3.066888)/1.907056)*((foxWolframR1-0.133878)/0.208048)) + 0.448409*(((Ntau_gencut-3.066888)/1.907056)*((foxWolframR2-0.795967)/0.103103)) + -0.682494*(((Ntau_gencut-3.066888)/1.907056)*((foxWolframR3-0.172558)/0.189523)) + -0.406581*(((Ntau_gencut-3.066888)/1.907056)*((harmonicMomentThrust0-0.722706)/0.127990)) + -0.051331*(((Ntau_gencut-3.066888)/1.907056)*((harmonicMomentThrust1-0.000653)/0.198908)) + 0.008091*(((Ntrack_gencut-4.252824)/0.670521)^2) + -0.034948*(((Ntrack_gencut-4.252824)/0.670521)*((NTBz_gencut-0.560508)/0.281601)) + -0.094471*(((Ntrack_gencut-4.252824)/0.670521)*((foxWolframR1-0.133878)/0.208048)) + -0.010732*(((Ntrack_gencut-4.252824)/0.670521)*((foxWolframR2-0.795967)/0.103103)) + 0.179703*(((Ntrack_gencut-4.252824)/0.670521)*((foxWolframR3-0.172558)/0.189523)) + 0.153918*(((Ntrack_gencut-4.252824)/0.670521)*((harmonicMomentThrust0-0.722706)/0.127990)) + 0.010033*(((Ntrack_gencut-4.252824)/0.670521)*((harmonicMomentThrust1-0.000653)/0.198908)) + -0.172119*(((NTBz_gencut-0.560508)/0.281601)^2) + 0.460824*(((NTBz_gencut-0.560508)/0.281601)*((foxWolframR1-0.133878)/0.208048)) + 0.362550*(((NTBz_gencut-0.560508)/0.281601)*((foxWolframR2-0.795967)/0.103103)) + -0.566189*(((NTBz_gencut-0.560508)/0.281601)*((foxWolframR3-0.172558)/0.189523)) + -0.319401*(((NTBz_gencut-0.560508)/0.281601)*((harmonicMomentThrust0-0.722706)/0.127990)) + -0.190416*(((NTBz_gencut-0.560508)/0.281601)*((harmonicMomentThrust1-0.000653)/0.198908)) + 0.447427*(((foxWolframR1-0.133878)/0.208048)^2) + -2.107629*(((foxWolframR1-0.133878)/0.208048)*((foxWolframR2-0.795967)/0.103103)) + -0.961887*(((foxWolframR1-0.133878)/0.208048)*((foxWolframR3-0.172558)/0.189523)) + 0.744723*(((foxWolframR1-0.133878)/0.208048)*((harmonicMomentThrust0-0.722706)/0.127990)) + 0.040274*(((foxWolframR1-0.133878)/0.208048)*((harmonicMomentThrust1-0.000653)/0.198908)) + 1.305555*(((foxWolframR2-0.795967)/0.103103)^2) + 1.916540*(((foxWolframR2-0.795967)/0.103103)*((foxWolframR3-0.172558)/0.189523)) + -0.363337*(((foxWolframR2-0.795967)/0.103103)*((harmonicMomentThrust0-0.722706)/0.127990)) + -0.136459*(((foxWolframR2-0.795967)/0.103103)*((harmonicMomentThrust1-0.000653)/0.198908)) + 0.244360*(((foxWolframR3-0.172558)/0.189523)^2) + -0.223667*(((foxWolframR3-0.172558)/0.189523)*((harmonicMomentThrust0-0.722706)/0.127990)) + -0.037114*(((foxWolframR3-0.172558)/0.189523)*((harmonicMomentThrust1-0.000653)/0.198908)) + -0.140279*(((harmonicMomentThrust0-0.722706)/0.127990)^2) + 0.044508*(((harmonicMomentThrust0-0.722706)/0.127990)*((harmonicMomentThrust1-0.000653)/0.198908)) + -0.761791*(((harmonicMomentThrust1-0.000653)/0.198908)^2) + (1.587373))"
    )
    va.variables.addAlias(
        "LL_score_B_quad",
        "formula(-1.468575*(((thrust-0.931532)/0.057675)) + 0.147614*(((thrustAxisCosTheta-0.543235)/0.382543)) + -0.658791*(((genTotalPhotonsEnergyOfEvent-2.082091)/1.924634)) + -1.540870*(((Net_gencut-6.937697)/2.494185)) + -4.848738*(((sphericity-0.101686)/0.111456)) + -1.253075*(((aplanarity-0.012514)/0.019085)) + 0.082304*(((Ntrack_gencut-2.384814)/0.865034)) + -1.056821*(((NTBz_gencut-0.561522)/0.292300)) + -0.856372*(((foxWolframR1-0.207768)/0.234790)) + -0.502995*(((foxWolframR2-0.713083)/0.181992)) + 2.284183*(((foxWolframR3-0.300326)/0.235700)) + 2.493462*(((harmonicMomentThrust0-0.539758)/0.181052)) + 0.149075*(((harmonicMomentThrust1--0.000225)/0.191300)) + 2.430399*(((thrust-0.931532)/0.057675)^2) + 0.095289*(((thrust-0.931532)/0.057675)*((thrustAxisCosTheta-0.543235)/0.382543)) + -0.282548*(((thrust-0.931532)/0.057675)*((genTotalPhotonsEnergyOfEvent-2.082091)/1.924634)) + -2.193972*(((thrust-0.931532)/0.057675)*((Net_gencut-6.937697)/2.494185)) + 1.041593*(((thrust-0.931532)/0.057675)*((sphericity-0.101686)/0.111456)) + 0.075547*(((thrust-0.931532)/0.057675)*((aplanarity-0.012514)/0.019085)) + 0.280942*(((thrust-0.931532)/0.057675)*((Ntrack_gencut-2.384814)/0.865034)) + -0.966756*(((thrust-0.931532)/0.057675)*((NTBz_gencut-0.561522)/0.292300)) + 0.253008*(((thrust-0.931532)/0.057675)*((foxWolframR1-0.207768)/0.234790)) + -5.945993*(((thrust-0.931532)/0.057675)*((foxWolframR2-0.713083)/0.181992)) + -1.344497*(((thrust-0.931532)/0.057675)*((foxWolframR3-0.300326)/0.235700)) + -0.281680*(((thrust-0.931532)/0.057675)*((harmonicMomentThrust0-0.539758)/0.181052)) + 0.061185*(((thrust-0.931532)/0.057675)*((harmonicMomentThrust1--0.000225)/0.191300)) + 0.188934*(((thrustAxisCosTheta-0.543235)/0.382543)^2) + 0.035304*(((thrustAxisCosTheta-0.543235)/0.382543)*((genTotalPhotonsEnergyOfEvent-2.082091)/1.924634)) + 0.037674*(((thrustAxisCosTheta-0.543235)/0.382543)*((Net_gencut-6.937697)/2.494185)) + -0.133205*(((thrustAxisCosTheta-0.543235)/0.382543)*((sphericity-0.101686)/0.111456)) + -0.068160*(((thrustAxisCosTheta-0.543235)/0.382543)*((aplanarity-0.012514)/0.019085)) + 0.119588*(((thrustAxisCosTheta-0.543235)/0.382543)*((Ntrack_gencut-2.384814)/0.865034)) + -0.005890*(((thrustAxisCosTheta-0.543235)/0.382543)*((NTBz_gencut-0.561522)/0.292300)) + 0.023089*(((thrustAxisCosTheta-0.543235)/0.382543)*((foxWolframR1-0.207768)/0.234790)) + -0.321359*(((thrustAxisCosTheta-0.543235)/0.382543)*((foxWolframR2-0.713083)/0.181992)) + 0.029353*(((thrustAxisCosTheta-0.543235)/0.382543)*((foxWolframR3-0.300326)/0.235700)) + -0.046118*(((thrustAxisCosTheta-0.543235)/0.382543)*((harmonicMomentThrust0-0.539758)/0.181052)) + 0.007899*(((thrustAxisCosTheta-0.543235)/0.382543)*((harmonicMomentThrust1--0.000225)/0.191300)) + -0.738617*(((genTotalPhotonsEnergyOfEvent-2.082091)/1.924634)^2) + -0.067179*(((genTotalPhotonsEnergyOfEvent-2.082091)/1.924634)*((Net_gencut-6.937697)/2.494185)) + 0.048183*(((genTotalPhotonsEnergyOfEvent-2.082091)/1.924634)*((sphericity-0.101686)/0.111456)) + -0.213768*(((genTotalPhotonsEnergyOfEvent-2.082091)/1.924634)*((aplanarity-0.012514)/0.019085)) + -0.159578*(((genTotalPhotonsEnergyOfEvent-2.082091)/1.924634)*((Ntrack_gencut-2.384814)/0.865034)) + -0.043202*(((genTotalPhotonsEnergyOfEvent-2.082091)/1.924634)*((NTBz_gencut-0.561522)/0.292300)) + -0.373401*(((genTotalPhotonsEnergyOfEvent-2.082091)/1.924634)*((foxWolframR1-0.207768)/0.234790)) + 0.060320*(((genTotalPhotonsEnergyOfEvent-2.082091)/1.924634)*((foxWolframR2-0.713083)/0.181992)) + -0.055975*(((genTotalPhotonsEnergyOfEvent-2.082091)/1.924634)*((foxWolframR3-0.300326)/0.235700)) + 0.576320*(((genTotalPhotonsEnergyOfEvent-2.082091)/1.924634)*((harmonicMomentThrust0-0.539758)/0.181052)) + 0.152488*(((genTotalPhotonsEnergyOfEvent-2.082091)/1.924634)*((harmonicMomentThrust1--0.000225)/0.191300)) + -1.536561*(((Net_gencut-6.937697)/2.494185)^2) + -0.999339*(((Net_gencut-6.937697)/2.494185)*((sphericity-0.101686)/0.111456)) + -0.352839*(((Net_gencut-6.937697)/2.494185)*((aplanarity-0.012514)/0.019085)) + -0.168439*(((Net_gencut-6.937697)/2.494185)*((Ntrack_gencut-2.384814)/0.865034)) + -1.720535*(((Net_gencut-6.937697)/2.494185)*((NTBz_gencut-0.561522)/0.292300)) + -0.218618*(((Net_gencut-6.937697)/2.494185)*((foxWolframR1-0.207768)/0.234790)) + 1.434713*(((Net_gencut-6.937697)/2.494185)*((foxWolframR2-0.713083)/0.181992)) + 0.087436*(((Net_gencut-6.937697)/2.494185)*((foxWolframR3-0.300326)/0.235700)) + -0.222726*(((Net_gencut-6.937697)/2.494185)*((harmonicMomentThrust0-0.539758)/0.181052)) + -0.055344*(((Net_gencut-6.937697)/2.494185)*((harmonicMomentThrust1--0.000225)/0.191300)) + -0.046732*(((sphericity-0.101686)/0.111456)^2) + -0.563629*(((sphericity-0.101686)/0.111456)*((aplanarity-0.012514)/0.019085)) + 0.402001*(((sphericity-0.101686)/0.111456)*((Ntrack_gencut-2.384814)/0.865034)) + -0.362657*(((sphericity-0.101686)/0.111456)*((NTBz_gencut-0.561522)/0.292300)) + 0.108470*(((sphericity-0.101686)/0.111456)*((foxWolframR1-0.207768)/0.234790)) + -2.929054*(((sphericity-0.101686)/0.111456)*((foxWolframR2-0.713083)/0.181992)) + 0.079592*(((sphericity-0.101686)/0.111456)*((foxWolframR3-0.300326)/0.235700)) + -0.666738*(((sphericity-0.101686)/0.111456)*((harmonicMomentThrust0-0.539758)/0.181052)) + -0.019399*(((sphericity-0.101686)/0.111456)*((harmonicMomentThrust1--0.000225)/0.191300)) + 0.158393*(((aplanarity-0.012514)/0.019085)^2) + 0.082741*(((aplanarity-0.012514)/0.019085)*((Ntrack_gencut-2.384814)/0.865034)) + -0.050328*(((aplanarity-0.012514)/0.019085)*((NTBz_gencut-0.561522)/0.292300)) + -0.152817*(((aplanarity-0.012514)/0.019085)*((foxWolframR1-0.207768)/0.234790)) + -0.449028*(((aplanarity-0.012514)/0.019085)*((foxWolframR2-0.713083)/0.181992)) + -0.084616*(((aplanarity-0.012514)/0.019085)*((foxWolframR3-0.300326)/0.235700)) + -0.020044*(((aplanarity-0.012514)/0.019085)*((harmonicMomentThrust0-0.539758)/0.181052)) + 0.105123*(((aplanarity-0.012514)/0.019085)*((harmonicMomentThrust1--0.000225)/0.191300)) + -0.024386*(((Ntrack_gencut-2.384814)/0.865034)^2) + -0.110484*(((Ntrack_gencut-2.384814)/0.865034)*((NTBz_gencut-0.561522)/0.292300)) + 0.027388*(((Ntrack_gencut-2.384814)/0.865034)*((foxWolframR1-0.207768)/0.234790)) + -0.033332*(((Ntrack_gencut-2.384814)/0.865034)*((foxWolframR2-0.713083)/0.181992)) + -0.383008*(((Ntrack_gencut-2.384814)/0.865034)*((foxWolframR3-0.300326)/0.235700)) + -0.389075*(((Ntrack_gencut-2.384814)/0.865034)*((harmonicMomentThrust0-0.539758)/0.181052)) + 0.042104*(((Ntrack_gencut-2.384814)/0.865034)*((harmonicMomentThrust1--0.000225)/0.191300)) + -0.923660*(((NTBz_gencut-0.561522)/0.292300)^2) + 0.012233*(((NTBz_gencut-0.561522)/0.292300)*((foxWolframR1-0.207768)/0.234790)) + 0.712592*(((NTBz_gencut-0.561522)/0.292300)*((foxWolframR2-0.713083)/0.181992)) + -0.236920*(((NTBz_gencut-0.561522)/0.292300)*((foxWolframR3-0.300326)/0.235700)) + -0.303811*(((NTBz_gencut-0.561522)/0.292300)*((harmonicMomentThrust0-0.539758)/0.181052)) + -0.034625*(((NTBz_gencut-0.561522)/0.292300)*((harmonicMomentThrust1--0.000225)/0.191300)) + 0.460320*(((foxWolframR1-0.207768)/0.234790)^2) + -1.006434*(((foxWolframR1-0.207768)/0.234790)*((foxWolframR2-0.713083)/0.181992)) + -0.078417*(((foxWolframR1-0.207768)/0.234790)*((foxWolframR3-0.300326)/0.235700)) + 0.029219*(((foxWolframR1-0.207768)/0.234790)*((harmonicMomentThrust0-0.539758)/0.181052)) + -0.010924*(((foxWolframR1-0.207768)/0.234790)*((harmonicMomentThrust1--0.000225)/0.191300)) + 2.932022*(((foxWolframR2-0.713083)/0.181992)^2) + 2.004960*(((foxWolframR2-0.713083)/0.181992)*((foxWolframR3-0.300326)/0.235700)) + 0.185571*(((foxWolframR2-0.713083)/0.181992)*((harmonicMomentThrust0-0.539758)/0.181052)) + -0.027893*(((foxWolframR2-0.713083)/0.181992)*((harmonicMomentThrust1--0.000225)/0.191300)) + -0.640797*(((foxWolframR3-0.300326)/0.235700)^2) + 0.062415*(((foxWolframR3-0.300326)/0.235700)*((harmonicMomentThrust0-0.539758)/0.181052)) + 0.043469*(((foxWolframR3-0.300326)/0.235700)*((harmonicMomentThrust1--0.000225)/0.191300)) + -0.511793*(((harmonicMomentThrust0-0.539758)/0.181052)^2) + -0.063219*(((harmonicMomentThrust0-0.539758)/0.181052)*((harmonicMomentThrust1--0.000225)/0.191300)) + -0.549055*(((harmonicMomentThrust1--0.000225)/0.191300)^2) + (-1.543464))"
    )

    # event cut
    ma.applyEventCuts("[[nParticlesInList(tau+:fake) > 0.5] and [LL_score_A_quad > -4.04750]] or [[nParticlesInList(tau+:fake) < 0.5] and [LL_score_B_quad > -1.22118]]", path=path)

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
main.add_module("EventInfoSetter", expList=1004, runList=0, evtNumList=10000)

# use KKMC to generate taupair events
ge.add_kkmc_generator(path=main, finalstate='tau-tau+', eventType='taupair')

# generator level cut
GenCutTAUPAIR(path=main)

# detector simulation
si.add_simulation(path=main, bkgfiles=bg)

# reconstruction
re.add_reconstruction(path=main)

# Finally add mdst output (file name overwritten on the grid)
mdst.add_mdst_output(path=main, filename="mdst.root")

# produce systematic skims
from skim import BaseSkim, CombinedSkim
import skim.WGs.systematics as systematics

skim_sclm = systematics.SystematicsCombinedLowMulti()
skim_sclm(path=main)

skim_sch = systematics.SystematicsCombinedHadronic()
skim_sch(path=main)

# process events and print call statistics
b2.process(path=main)
print(b2.statistics)
