#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Descriptor: ccbar

#############################################################
# Steering file for official MC production of early phase 3
# 'ccbar' samples with beam backgrounds (BGx1).
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
# Generator level cut for CCBAR background
# This reduces 50% of entire sample
# but keeps 98% of signal-like ccbar
#============================================================
def GenCutCCBAR(path):
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
        "formula(0.441936*(((thrust-0.880297)/0.045029)) + 0.571197*(((thrustAxisCosTheta-0.446266)/0.456254)) + 0.105200*(((genTotalPhotonsEnergyOfEvent-2.166204)/1.375192)) + 0.699278*(((Net_gencut-6.771720)/1.961342)) + -0.472257*(((sphericity-0.134242)/0.085222)) + -0.084436*(((aplanarity-0.031312)/0.025192)) + 1.338152*(((Ntau_gencut-4.875053)/4.013626)) + -1.126513*(((Ntrack_gencut-6.743252)/1.663522)) + 0.401855*(((NTBz_gencut-0.564149)/0.285549)) + 0.075172*(((foxWolframR1-0.025054)/0.062854)) + -0.368610*(((foxWolframR2-0.530706)/0.128185)) + 0.084974*(((foxWolframR3-0.069600)/0.070933)) + 0.008059*(((thrust-0.880297)/0.045029)^2) + 0.018787*(((thrust-0.880297)/0.045029)*((thrustAxisCosTheta-0.446266)/0.456254)) + -0.047046*(((thrust-0.880297)/0.045029)*((genTotalPhotonsEnergyOfEvent-2.166204)/1.375192)) + -0.059199*(((thrust-0.880297)/0.045029)*((Net_gencut-6.771720)/1.961342)) + -0.073696*(((thrust-0.880297)/0.045029)*((sphericity-0.134242)/0.085222)) + 0.147688*(((thrust-0.880297)/0.045029)*((aplanarity-0.031312)/0.025192)) + -0.160036*(((thrust-0.880297)/0.045029)*((Ntau_gencut-4.875053)/4.013626)) + 0.273855*(((thrust-0.880297)/0.045029)*((Ntrack_gencut-6.743252)/1.663522)) + 0.120664*(((thrust-0.880297)/0.045029)*((NTBz_gencut-0.564149)/0.285549)) + -0.058809*(((thrust-0.880297)/0.045029)*((foxWolframR1-0.025054)/0.062854)) + -0.068830*(((thrust-0.880297)/0.045029)*((foxWolframR2-0.530706)/0.128185)) + -0.075062*(((thrust-0.880297)/0.045029)*((foxWolframR3-0.069600)/0.070933)) + 0.334795*(((thrustAxisCosTheta-0.446266)/0.456254)^2) + -0.016913*(((thrustAxisCosTheta-0.446266)/0.456254)*((genTotalPhotonsEnergyOfEvent-2.166204)/1.375192)) + 0.038520*(((thrustAxisCosTheta-0.446266)/0.456254)*((Net_gencut-6.771720)/1.961342)) + -0.022951*(((thrustAxisCosTheta-0.446266)/0.456254)*((sphericity-0.134242)/0.085222)) + 0.003764*(((thrustAxisCosTheta-0.446266)/0.456254)*((aplanarity-0.031312)/0.025192)) + 0.005086*(((thrustAxisCosTheta-0.446266)/0.456254)*((Ntau_gencut-4.875053)/4.013626)) + 0.008635*(((thrustAxisCosTheta-0.446266)/0.456254)*((Ntrack_gencut-6.743252)/1.663522)) + 0.054886*(((thrustAxisCosTheta-0.446266)/0.456254)*((NTBz_gencut-0.564149)/0.285549)) + 0.017113*(((thrustAxisCosTheta-0.446266)/0.456254)*((foxWolframR1-0.025054)/0.062854)) + -0.073272*(((thrustAxisCosTheta-0.446266)/0.456254)*((foxWolframR2-0.530706)/0.128185)) + -0.029827*(((thrustAxisCosTheta-0.446266)/0.456254)*((foxWolframR3-0.069600)/0.070933)) + 0.011787*(((genTotalPhotonsEnergyOfEvent-2.166204)/1.375192)^2) + -0.024782*(((genTotalPhotonsEnergyOfEvent-2.166204)/1.375192)*((Net_gencut-6.771720)/1.961342)) + 0.018136*(((genTotalPhotonsEnergyOfEvent-2.166204)/1.375192)*((sphericity-0.134242)/0.085222)) + -0.002409*(((genTotalPhotonsEnergyOfEvent-2.166204)/1.375192)*((aplanarity-0.031312)/0.025192)) + 0.256963*(((genTotalPhotonsEnergyOfEvent-2.166204)/1.375192)*((Ntau_gencut-4.875053)/4.013626)) + -0.033680*(((genTotalPhotonsEnergyOfEvent-2.166204)/1.375192)*((Ntrack_gencut-6.743252)/1.663522)) + -0.037633*(((genTotalPhotonsEnergyOfEvent-2.166204)/1.375192)*((NTBz_gencut-0.564149)/0.285549)) + -0.108638*(((genTotalPhotonsEnergyOfEvent-2.166204)/1.375192)*((foxWolframR1-0.025054)/0.062854)) + 0.040096*(((genTotalPhotonsEnergyOfEvent-2.166204)/1.375192)*((foxWolframR2-0.530706)/0.128185)) + 0.048524*(((genTotalPhotonsEnergyOfEvent-2.166204)/1.375192)*((foxWolframR3-0.069600)/0.070933)) + -0.174897*(((Net_gencut-6.771720)/1.961342)^2) + -0.173409*(((Net_gencut-6.771720)/1.961342)*((sphericity-0.134242)/0.085222)) + 0.025282*(((Net_gencut-6.771720)/1.961342)*((aplanarity-0.031312)/0.025192)) + -0.012811*(((Net_gencut-6.771720)/1.961342)*((Ntau_gencut-4.875053)/4.013626)) + -0.255927*(((Net_gencut-6.771720)/1.961342)*((Ntrack_gencut-6.743252)/1.663522)) + -0.111724*(((Net_gencut-6.771720)/1.961342)*((NTBz_gencut-0.564149)/0.285549)) + 0.128330*(((Net_gencut-6.771720)/1.961342)*((foxWolframR1-0.025054)/0.062854)) + 0.017653*(((Net_gencut-6.771720)/1.961342)*((foxWolframR2-0.530706)/0.128185)) + -0.144182*(((Net_gencut-6.771720)/1.961342)*((foxWolframR3-0.069600)/0.070933)) + 0.062829*(((sphericity-0.134242)/0.085222)^2) + 0.006189*(((sphericity-0.134242)/0.085222)*((aplanarity-0.031312)/0.025192)) + 0.049221*(((sphericity-0.134242)/0.085222)*((Ntau_gencut-4.875053)/4.013626)) + 0.007920*(((sphericity-0.134242)/0.085222)*((Ntrack_gencut-6.743252)/1.663522)) + -0.093464*(((sphericity-0.134242)/0.085222)*((NTBz_gencut-0.564149)/0.285549)) + 0.009423*(((sphericity-0.134242)/0.085222)*((foxWolframR1-0.025054)/0.062854)) + 0.157657*(((sphericity-0.134242)/0.085222)*((foxWolframR2-0.530706)/0.128185)) + 0.023318*(((sphericity-0.134242)/0.085222)*((foxWolframR3-0.069600)/0.070933)) + 0.021861*(((aplanarity-0.031312)/0.025192)^2) + 0.017029*(((aplanarity-0.031312)/0.025192)*((Ntau_gencut-4.875053)/4.013626)) + -0.039265*(((aplanarity-0.031312)/0.025192)*((Ntrack_gencut-6.743252)/1.663522)) + 0.013530*(((aplanarity-0.031312)/0.025192)*((NTBz_gencut-0.564149)/0.285549)) + -0.095261*(((aplanarity-0.031312)/0.025192)*((foxWolframR1-0.025054)/0.062854)) + -0.122497*(((aplanarity-0.031312)/0.025192)*((foxWolframR2-0.530706)/0.128185)) + 0.087041*(((aplanarity-0.031312)/0.025192)*((foxWolframR3-0.069600)/0.070933)) + -0.137413*(((Ntau_gencut-4.875053)/4.013626)^2) + 0.018021*(((Ntau_gencut-4.875053)/4.013626)*((Ntrack_gencut-6.743252)/1.663522)) + -0.053122*(((Ntau_gencut-4.875053)/4.013626)*((NTBz_gencut-0.564149)/0.285549)) + 0.144461*(((Ntau_gencut-4.875053)/4.013626)*((foxWolframR1-0.025054)/0.062854)) + 0.310685*(((Ntau_gencut-4.875053)/4.013626)*((foxWolframR2-0.530706)/0.128185)) + 0.162671*(((Ntau_gencut-4.875053)/4.013626)*((foxWolframR3-0.069600)/0.070933)) + -0.254024*(((Ntrack_gencut-6.743252)/1.663522)^2) + 0.004671*(((Ntrack_gencut-6.743252)/1.663522)*((NTBz_gencut-0.564149)/0.285549)) + 0.026523*(((Ntrack_gencut-6.743252)/1.663522)*((foxWolframR1-0.025054)/0.062854)) + -0.343965*(((Ntrack_gencut-6.743252)/1.663522)*((foxWolframR2-0.530706)/0.128185)) + -0.120597*(((Ntrack_gencut-6.743252)/1.663522)*((foxWolframR3-0.069600)/0.070933)) + 0.166845*(((NTBz_gencut-0.564149)/0.285549)^2) + 0.085796*(((NTBz_gencut-0.564149)/0.285549)*((foxWolframR1-0.025054)/0.062854)) + -0.011539*(((NTBz_gencut-0.564149)/0.285549)*((foxWolframR2-0.530706)/0.128185)) + -0.076539*(((NTBz_gencut-0.564149)/0.285549)*((foxWolframR3-0.069600)/0.070933)) + -0.017909*(((foxWolframR1-0.025054)/0.062854)^2) + -0.016465*(((foxWolframR1-0.025054)/0.062854)*((foxWolframR2-0.530706)/0.128185)) + 0.019972*(((foxWolframR1-0.025054)/0.062854)*((foxWolframR3-0.069600)/0.070933)) + 0.022543*(((foxWolframR2-0.530706)/0.128185)^2) + 0.111825*(((foxWolframR2-0.530706)/0.128185)*((foxWolframR3-0.069600)/0.070933)) + -0.026674*(((foxWolframR3-0.069600)/0.070933)^2) + (0.093147))"
    )
    va.variables.addAlias(
        "LL_score_B_quad",
        "formula(0.969155*(((thrust-0.807100)/0.069727)) + 0.192692*(((thrustAxisCosTheta-0.489222)/0.463760)) + -0.388185*(((genTotalPhotonsEnergyOfEvent-3.387214)/1.760295)) + 0.074488*(((Net_gencut-5.902292)/2.036192)) + -1.121953*(((sphericity-0.302268)/0.156104)) + -0.557741*(((aplanarity-0.082612)/0.054507)) + -1.584761*(((Ntrack_gencut-8.245883)/2.501888)) + 0.379834*(((NTBz_gencut-0.575718)/0.296082)) + -0.423345*(((foxWolframR1-0.025712)/0.052172)) + -1.747938*(((foxWolframR2-0.342525)/0.147949)) + 0.037325*(((foxWolframR3-0.079628)/0.063809)) + 0.151182*(((thrust-0.807100)/0.069727)^2) + 0.103207*(((thrust-0.807100)/0.069727)*((thrustAxisCosTheta-0.489222)/0.463760)) + -0.157222*(((thrust-0.807100)/0.069727)*((genTotalPhotonsEnergyOfEvent-3.387214)/1.760295)) + -0.102492*(((thrust-0.807100)/0.069727)*((Net_gencut-5.902292)/2.036192)) + 0.128509*(((thrust-0.807100)/0.069727)*((sphericity-0.302268)/0.156104)) + 0.091552*(((thrust-0.807100)/0.069727)*((aplanarity-0.082612)/0.054507)) + -0.214060*(((thrust-0.807100)/0.069727)*((Ntrack_gencut-8.245883)/2.501888)) + -0.137324*(((thrust-0.807100)/0.069727)*((NTBz_gencut-0.575718)/0.296082)) + -0.077906*(((thrust-0.807100)/0.069727)*((foxWolframR1-0.025712)/0.052172)) + 0.032369*(((thrust-0.807100)/0.069727)*((foxWolframR2-0.342525)/0.147949)) + -0.020023*(((thrust-0.807100)/0.069727)*((foxWolframR3-0.079628)/0.063809)) + 0.105213*(((thrustAxisCosTheta-0.489222)/0.463760)^2) + 0.028503*(((thrustAxisCosTheta-0.489222)/0.463760)*((genTotalPhotonsEnergyOfEvent-3.387214)/1.760295)) + 0.038819*(((thrustAxisCosTheta-0.489222)/0.463760)*((Net_gencut-5.902292)/2.036192)) + 0.081409*(((thrustAxisCosTheta-0.489222)/0.463760)*((sphericity-0.302268)/0.156104)) + -0.041741*(((thrustAxisCosTheta-0.489222)/0.463760)*((aplanarity-0.082612)/0.054507)) + 0.014078*(((thrustAxisCosTheta-0.489222)/0.463760)*((Ntrack_gencut-8.245883)/2.501888)) + 0.030402*(((thrustAxisCosTheta-0.489222)/0.463760)*((NTBz_gencut-0.575718)/0.296082)) + -0.001714*(((thrustAxisCosTheta-0.489222)/0.463760)*((foxWolframR1-0.025712)/0.052172)) + -0.044223*(((thrustAxisCosTheta-0.489222)/0.463760)*((foxWolframR2-0.342525)/0.147949)) + -0.020792*(((thrustAxisCosTheta-0.489222)/0.463760)*((foxWolframR3-0.079628)/0.063809)) + -0.154416*(((genTotalPhotonsEnergyOfEvent-3.387214)/1.760295)^2) + -0.111023*(((genTotalPhotonsEnergyOfEvent-3.387214)/1.760295)*((Net_gencut-5.902292)/2.036192)) + 0.050330*(((genTotalPhotonsEnergyOfEvent-3.387214)/1.760295)*((sphericity-0.302268)/0.156104)) + 0.030425*(((genTotalPhotonsEnergyOfEvent-3.387214)/1.760295)*((aplanarity-0.082612)/0.054507)) + -0.028585*(((genTotalPhotonsEnergyOfEvent-3.387214)/1.760295)*((Ntrack_gencut-8.245883)/2.501888)) + -0.099628*(((genTotalPhotonsEnergyOfEvent-3.387214)/1.760295)*((NTBz_gencut-0.575718)/0.296082)) + -0.112394*(((genTotalPhotonsEnergyOfEvent-3.387214)/1.760295)*((foxWolframR1-0.025712)/0.052172)) + 0.116662*(((genTotalPhotonsEnergyOfEvent-3.387214)/1.760295)*((foxWolframR2-0.342525)/0.147949)) + 0.000940*(((genTotalPhotonsEnergyOfEvent-3.387214)/1.760295)*((foxWolframR3-0.079628)/0.063809)) + 0.013634*(((Net_gencut-5.902292)/2.036192)^2) + -0.518489*(((Net_gencut-5.902292)/2.036192)*((sphericity-0.302268)/0.156104)) + 0.052144*(((Net_gencut-5.902292)/2.036192)*((aplanarity-0.082612)/0.054507)) + -0.477620*(((Net_gencut-5.902292)/2.036192)*((Ntrack_gencut-8.245883)/2.501888)) + -0.031657*(((Net_gencut-5.902292)/2.036192)*((NTBz_gencut-0.575718)/0.296082)) + -0.050970*(((Net_gencut-5.902292)/2.036192)*((foxWolframR1-0.025712)/0.052172)) + -0.211823*(((Net_gencut-5.902292)/2.036192)*((foxWolframR2-0.342525)/0.147949)) + -0.009665*(((Net_gencut-5.902292)/2.036192)*((foxWolframR3-0.079628)/0.063809)) + 0.191423*(((sphericity-0.302268)/0.156104)^2) + -0.134254*(((sphericity-0.302268)/0.156104)*((aplanarity-0.082612)/0.054507)) + -0.018468*(((sphericity-0.302268)/0.156104)*((Ntrack_gencut-8.245883)/2.501888)) + -0.283592*(((sphericity-0.302268)/0.156104)*((NTBz_gencut-0.575718)/0.296082)) + -0.048434*(((sphericity-0.302268)/0.156104)*((foxWolframR1-0.025712)/0.052172)) + -0.076175*(((sphericity-0.302268)/0.156104)*((foxWolframR2-0.342525)/0.147949)) + 0.114215*(((sphericity-0.302268)/0.156104)*((foxWolframR3-0.079628)/0.063809)) + 0.126828*(((aplanarity-0.082612)/0.054507)^2) + 0.020040*(((aplanarity-0.082612)/0.054507)*((Ntrack_gencut-8.245883)/2.501888)) + 0.244093*(((aplanarity-0.082612)/0.054507)*((NTBz_gencut-0.575718)/0.296082)) + 0.018381*(((aplanarity-0.082612)/0.054507)*((foxWolframR1-0.025712)/0.052172)) + -0.219301*(((aplanarity-0.082612)/0.054507)*((foxWolframR2-0.342525)/0.147949)) + -0.052295*(((aplanarity-0.082612)/0.054507)*((foxWolframR3-0.079628)/0.063809)) + -0.385868*(((Ntrack_gencut-8.245883)/2.501888)^2) + 0.016377*(((Ntrack_gencut-8.245883)/2.501888)*((NTBz_gencut-0.575718)/0.296082)) + 0.129446*(((Ntrack_gencut-8.245883)/2.501888)*((foxWolframR1-0.025712)/0.052172)) + 0.171594*(((Ntrack_gencut-8.245883)/2.501888)*((foxWolframR2-0.342525)/0.147949)) + -0.070338*(((Ntrack_gencut-8.245883)/2.501888)*((foxWolframR3-0.079628)/0.063809)) + 0.080818*(((NTBz_gencut-0.575718)/0.296082)^2) + 0.025379*(((NTBz_gencut-0.575718)/0.296082)*((foxWolframR1-0.025712)/0.052172)) + 0.204785*(((NTBz_gencut-0.575718)/0.296082)*((foxWolframR2-0.342525)/0.147949)) + -0.036418*(((NTBz_gencut-0.575718)/0.296082)*((foxWolframR3-0.079628)/0.063809)) + 0.043793*(((foxWolframR1-0.025712)/0.052172)^2) + 0.033384*(((foxWolframR1-0.025712)/0.052172)*((foxWolframR2-0.342525)/0.147949)) + 0.021180*(((foxWolframR1-0.025712)/0.052172)*((foxWolframR3-0.079628)/0.063809)) + -0.039405*(((foxWolframR2-0.342525)/0.147949)^2) + 0.021978*(((foxWolframR2-0.342525)/0.147949)*((foxWolframR3-0.079628)/0.063809)) + -0.030570*(((foxWolframR3-0.079628)/0.063809)^2) + (-1.216743))"
    )

    # event cut
    ma.applyEventCuts("[[nParticlesInList(tau+:fake) > 0.5] and [LL_score_A_quad > -4.69576]] or [[nParticlesInList(tau+:fake) < 0.5] and [LL_score_B_quad > -0.51801]]", path=path)

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

# generate ccbar events
ge.add_continuum_generator(path=main, finalstate='ccbar', eventType='ccbar')

# generator level cut
GenCutCCBAR(path=main)

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
