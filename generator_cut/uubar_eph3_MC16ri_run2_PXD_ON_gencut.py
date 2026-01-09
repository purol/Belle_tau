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

#============================================================
# Generator level cut for UUBAR background
# This reduces 70% of entire sample
# but keeps 98% of signal-like uubar
#============================================================
def GenCutUUBAR(path):
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
        "formula(0.477303*(((thrust-0.883913)/0.053154)) + 0.827174*(((thrustAxisCosTheta-0.506870)/0.430503)) + 0.339198*(((genTotalPhotonsEnergyOfEvent-2.432417)/1.649687)) + 0.281080*(((Net_gencut-6.870460)/2.059113)) + -0.646543*(((sphericity-0.114600)/0.088405)) + -0.213220*(((aplanarity-0.026092)/0.023662)) + 1.547518*(((Ntau_gencut-5.594226)/4.493023)) + -1.059991*(((Ntrack_gencut-6.224573)/1.666514)) + -0.185704*(((NTBz_gencut-0.588639)/0.286403)) + 0.288919*(((foxWolframR1-0.012318)/0.049781)) + -0.667917*(((foxWolframR2-0.557476)/0.153390)) + -0.112258*(((foxWolframR3-0.062683)/0.068709)) + 0.047060*(((thrust-0.883913)/0.053154)^2) + 0.095688*(((thrust-0.883913)/0.053154)*((thrustAxisCosTheta-0.506870)/0.430503)) + 0.041497*(((thrust-0.883913)/0.053154)*((genTotalPhotonsEnergyOfEvent-2.432417)/1.649687)) + -0.261349*(((thrust-0.883913)/0.053154)*((Net_gencut-6.870460)/2.059113)) + 0.447995*(((thrust-0.883913)/0.053154)*((sphericity-0.114600)/0.088405)) + 0.054435*(((thrust-0.883913)/0.053154)*((aplanarity-0.026092)/0.023662)) + -0.191613*(((thrust-0.883913)/0.053154)*((Ntau_gencut-5.594226)/4.493023)) + 0.234394*(((thrust-0.883913)/0.053154)*((Ntrack_gencut-6.224573)/1.666514)) + -0.111400*(((thrust-0.883913)/0.053154)*((NTBz_gencut-0.588639)/0.286403)) + 0.126255*(((thrust-0.883913)/0.053154)*((foxWolframR1-0.012318)/0.049781)) + -0.106127*(((thrust-0.883913)/0.053154)*((foxWolframR2-0.557476)/0.153390)) + -0.173679*(((thrust-0.883913)/0.053154)*((foxWolframR3-0.062683)/0.068709)) + 0.359369*(((thrustAxisCosTheta-0.506870)/0.430503)^2) + -0.015604*(((thrustAxisCosTheta-0.506870)/0.430503)*((genTotalPhotonsEnergyOfEvent-2.432417)/1.649687)) + 0.030341*(((thrustAxisCosTheta-0.506870)/0.430503)*((Net_gencut-6.870460)/2.059113)) + -0.030731*(((thrustAxisCosTheta-0.506870)/0.430503)*((sphericity-0.114600)/0.088405)) + 0.003727*(((thrustAxisCosTheta-0.506870)/0.430503)*((aplanarity-0.026092)/0.023662)) + -0.018448*(((thrustAxisCosTheta-0.506870)/0.430503)*((Ntau_gencut-5.594226)/4.493023)) + -0.009197*(((thrustAxisCosTheta-0.506870)/0.430503)*((Ntrack_gencut-6.224573)/1.666514)) + 0.024259*(((thrustAxisCosTheta-0.506870)/0.430503)*((NTBz_gencut-0.588639)/0.286403)) + 0.006181*(((thrustAxisCosTheta-0.506870)/0.430503)*((foxWolframR1-0.012318)/0.049781)) + -0.145145*(((thrustAxisCosTheta-0.506870)/0.430503)*((foxWolframR2-0.557476)/0.153390)) + -0.023323*(((thrustAxisCosTheta-0.506870)/0.430503)*((foxWolframR3-0.062683)/0.068709)) + -0.134164*(((genTotalPhotonsEnergyOfEvent-2.432417)/1.649687)^2) + 0.001102*(((genTotalPhotonsEnergyOfEvent-2.432417)/1.649687)*((Net_gencut-6.870460)/2.059113)) + 0.007136*(((genTotalPhotonsEnergyOfEvent-2.432417)/1.649687)*((sphericity-0.114600)/0.088405)) + -0.002187*(((genTotalPhotonsEnergyOfEvent-2.432417)/1.649687)*((aplanarity-0.026092)/0.023662)) + 0.278684*(((genTotalPhotonsEnergyOfEvent-2.432417)/1.649687)*((Ntau_gencut-5.594226)/4.493023)) + -0.294090*(((genTotalPhotonsEnergyOfEvent-2.432417)/1.649687)*((Ntrack_gencut-6.224573)/1.666514)) + -0.053717*(((genTotalPhotonsEnergyOfEvent-2.432417)/1.649687)*((NTBz_gencut-0.588639)/0.286403)) + -0.190650*(((genTotalPhotonsEnergyOfEvent-2.432417)/1.649687)*((foxWolframR1-0.012318)/0.049781)) + -0.117735*(((genTotalPhotonsEnergyOfEvent-2.432417)/1.649687)*((foxWolframR2-0.557476)/0.153390)) + 0.017002*(((genTotalPhotonsEnergyOfEvent-2.432417)/1.649687)*((foxWolframR3-0.062683)/0.068709)) + -0.457708*(((Net_gencut-6.870460)/2.059113)^2) + -0.212385*(((Net_gencut-6.870460)/2.059113)*((sphericity-0.114600)/0.088405)) + 0.082574*(((Net_gencut-6.870460)/2.059113)*((aplanarity-0.026092)/0.023662)) + 0.077715*(((Net_gencut-6.870460)/2.059113)*((Ntau_gencut-5.594226)/4.493023)) + -0.062723*(((Net_gencut-6.870460)/2.059113)*((Ntrack_gencut-6.224573)/1.666514)) + -0.300595*(((Net_gencut-6.870460)/2.059113)*((NTBz_gencut-0.588639)/0.286403)) + 0.147993*(((Net_gencut-6.870460)/2.059113)*((foxWolframR1-0.012318)/0.049781)) + 0.023472*(((Net_gencut-6.870460)/2.059113)*((foxWolframR2-0.557476)/0.153390)) + -0.096095*(((Net_gencut-6.870460)/2.059113)*((foxWolframR3-0.062683)/0.068709)) + 0.032291*(((sphericity-0.114600)/0.088405)^2) + -0.007686*(((sphericity-0.114600)/0.088405)*((aplanarity-0.026092)/0.023662)) + 0.447202*(((sphericity-0.114600)/0.088405)*((Ntau_gencut-5.594226)/4.493023)) + -0.012904*(((sphericity-0.114600)/0.088405)*((Ntrack_gencut-6.224573)/1.666514)) + -0.104856*(((sphericity-0.114600)/0.088405)*((NTBz_gencut-0.588639)/0.286403)) + -0.018292*(((sphericity-0.114600)/0.088405)*((foxWolframR1-0.012318)/0.049781)) + -0.609580*(((sphericity-0.114600)/0.088405)*((foxWolframR2-0.557476)/0.153390)) + 0.051185*(((sphericity-0.114600)/0.088405)*((foxWolframR3-0.062683)/0.068709)) + 0.004054*(((aplanarity-0.026092)/0.023662)^2) + -0.013008*(((aplanarity-0.026092)/0.023662)*((Ntau_gencut-5.594226)/4.493023)) + 0.015694*(((aplanarity-0.026092)/0.023662)*((Ntrack_gencut-6.224573)/1.666514)) + 0.065457*(((aplanarity-0.026092)/0.023662)*((NTBz_gencut-0.588639)/0.286403)) + 0.024240*(((aplanarity-0.026092)/0.023662)*((foxWolframR1-0.012318)/0.049781)) + -0.165032*(((aplanarity-0.026092)/0.023662)*((foxWolframR2-0.557476)/0.153390)) + -0.053253*(((aplanarity-0.026092)/0.023662)*((foxWolframR3-0.062683)/0.068709)) + -0.070133*(((Ntau_gencut-5.594226)/4.493023)^2) + -0.085188*(((Ntau_gencut-5.594226)/4.493023)*((Ntrack_gencut-6.224573)/1.666514)) + -0.071657*(((Ntau_gencut-5.594226)/4.493023)*((NTBz_gencut-0.588639)/0.286403)) + 0.336325*(((Ntau_gencut-5.594226)/4.493023)*((foxWolframR1-0.012318)/0.049781)) + 0.519441*(((Ntau_gencut-5.594226)/4.493023)*((foxWolframR2-0.557476)/0.153390)) + -0.009635*(((Ntau_gencut-5.594226)/4.493023)*((foxWolframR3-0.062683)/0.068709)) + -0.406922*(((Ntrack_gencut-6.224573)/1.666514)^2) + 0.199822*(((Ntrack_gencut-6.224573)/1.666514)*((NTBz_gencut-0.588639)/0.286403)) + -0.064332*(((Ntrack_gencut-6.224573)/1.666514)*((foxWolframR1-0.012318)/0.049781)) + -0.383795*(((Ntrack_gencut-6.224573)/1.666514)*((foxWolframR2-0.557476)/0.153390)) + -0.072715*(((Ntrack_gencut-6.224573)/1.666514)*((foxWolframR3-0.062683)/0.068709)) + -0.122637*(((NTBz_gencut-0.588639)/0.286403)^2) + 0.080417*(((NTBz_gencut-0.588639)/0.286403)*((foxWolframR1-0.012318)/0.049781)) + 0.070188*(((NTBz_gencut-0.588639)/0.286403)*((foxWolframR2-0.557476)/0.153390)) + -0.009396*(((NTBz_gencut-0.588639)/0.286403)*((foxWolframR3-0.062683)/0.068709)) + -0.029022*(((foxWolframR1-0.012318)/0.049781)^2) + -0.095908*(((foxWolframR1-0.012318)/0.049781)*((foxWolframR2-0.557476)/0.153390)) + 0.047146*(((foxWolframR1-0.012318)/0.049781)*((foxWolframR3-0.062683)/0.068709)) + -0.177767*(((foxWolframR2-0.557476)/0.153390)^2) + 0.056164*(((foxWolframR2-0.557476)/0.153390)*((foxWolframR3-0.062683)/0.068709)) + -0.055761*(((foxWolframR3-0.062683)/0.068709)^2) + (0.279651))"
    )
    va.variables.addAlias(
        "LL_score_B_quad",
        "formula(0.471314*(((thrust-0.853123)/0.095201)) + 0.345790*(((thrustAxisCosTheta-0.605868)/0.444480)) + -1.015049*(((genTotalPhotonsEnergyOfEvent-4.694620)/2.440363)) + 1.033388*(((Net_gencut-5.279024)/2.784668)) + -1.995900*(((sphericity-0.215989)/0.176837)) + -1.217849*(((aplanarity-0.055205)/0.054264)) + -1.683418*(((Ntrack_gencut-6.041230)/3.183411)) + 0.434086*(((NTBz_gencut-0.657215)/0.310822)) + -0.967448*(((foxWolframR1-0.015728)/0.049540)) + -2.129004*(((foxWolframR2-0.494220)/0.271963)) + 0.276642*(((foxWolframR3-0.068553)/0.074331)) + 2.479565*(((thrust-0.853123)/0.095201)^2) + 0.048470*(((thrust-0.853123)/0.095201)*((thrustAxisCosTheta-0.605868)/0.444480)) + -0.476948*(((thrust-0.853123)/0.095201)*((genTotalPhotonsEnergyOfEvent-4.694620)/2.440363)) + -2.511821*(((thrust-0.853123)/0.095201)*((Net_gencut-5.279024)/2.784668)) + 1.207090*(((thrust-0.853123)/0.095201)*((sphericity-0.215989)/0.176837)) + 0.211493*(((thrust-0.853123)/0.095201)*((aplanarity-0.055205)/0.054264)) + 0.426446*(((thrust-0.853123)/0.095201)*((Ntrack_gencut-6.041230)/3.183411)) + -0.977333*(((thrust-0.853123)/0.095201)*((NTBz_gencut-0.657215)/0.310822)) + 0.206774*(((thrust-0.853123)/0.095201)*((foxWolframR1-0.015728)/0.049540)) + -4.633188*(((thrust-0.853123)/0.095201)*((foxWolframR2-0.494220)/0.271963)) + -0.094700*(((thrust-0.853123)/0.095201)*((foxWolframR3-0.068553)/0.074331)) + 0.115294*(((thrustAxisCosTheta-0.605868)/0.444480)^2) + 0.061301*(((thrustAxisCosTheta-0.605868)/0.444480)*((genTotalPhotonsEnergyOfEvent-4.694620)/2.440363)) + -0.220911*(((thrustAxisCosTheta-0.605868)/0.444480)*((Net_gencut-5.279024)/2.784668)) + -0.124314*(((thrustAxisCosTheta-0.605868)/0.444480)*((sphericity-0.215989)/0.176837)) + 0.036517*(((thrustAxisCosTheta-0.605868)/0.444480)*((aplanarity-0.055205)/0.054264)) + 0.191772*(((thrustAxisCosTheta-0.605868)/0.444480)*((Ntrack_gencut-6.041230)/3.183411)) + -0.164306*(((thrustAxisCosTheta-0.605868)/0.444480)*((NTBz_gencut-0.657215)/0.310822)) + 0.006673*(((thrustAxisCosTheta-0.605868)/0.444480)*((foxWolframR1-0.015728)/0.049540)) + -0.005683*(((thrustAxisCosTheta-0.605868)/0.444480)*((foxWolframR2-0.494220)/0.271963)) + -0.045335*(((thrustAxisCosTheta-0.605868)/0.444480)*((foxWolframR3-0.068553)/0.074331)) + -0.706217*(((genTotalPhotonsEnergyOfEvent-4.694620)/2.440363)^2) + -0.055052*(((genTotalPhotonsEnergyOfEvent-4.694620)/2.440363)*((Net_gencut-5.279024)/2.784668)) + 0.129050*(((genTotalPhotonsEnergyOfEvent-4.694620)/2.440363)*((sphericity-0.215989)/0.176837)) + -0.112275*(((genTotalPhotonsEnergyOfEvent-4.694620)/2.440363)*((aplanarity-0.055205)/0.054264)) + -0.456502*(((genTotalPhotonsEnergyOfEvent-4.694620)/2.440363)*((Ntrack_gencut-6.041230)/3.183411)) + -0.075220*(((genTotalPhotonsEnergyOfEvent-4.694620)/2.440363)*((NTBz_gencut-0.657215)/0.310822)) + -0.448565*(((genTotalPhotonsEnergyOfEvent-4.694620)/2.440363)*((foxWolframR1-0.015728)/0.049540)) + 0.467453*(((genTotalPhotonsEnergyOfEvent-4.694620)/2.440363)*((foxWolframR2-0.494220)/0.271963)) + 0.027747*(((genTotalPhotonsEnergyOfEvent-4.694620)/2.440363)*((foxWolframR3-0.068553)/0.074331)) + -1.993116*(((Net_gencut-5.279024)/2.784668)^2) + -0.646496*(((Net_gencut-5.279024)/2.784668)*((sphericity-0.215989)/0.176837)) + 0.736562*(((Net_gencut-5.279024)/2.784668)*((aplanarity-0.055205)/0.054264)) + -0.331952*(((Net_gencut-5.279024)/2.784668)*((Ntrack_gencut-6.041230)/3.183411)) + -1.239144*(((Net_gencut-5.279024)/2.784668)*((NTBz_gencut-0.657215)/0.310822)) + 0.178227*(((Net_gencut-5.279024)/2.784668)*((foxWolframR1-0.015728)/0.049540)) + 2.384205*(((Net_gencut-5.279024)/2.784668)*((foxWolframR2-0.494220)/0.271963)) + -0.244730*(((Net_gencut-5.279024)/2.784668)*((foxWolframR3-0.068553)/0.074331)) + -0.090091*(((sphericity-0.215989)/0.176837)^2) + -0.180766*(((sphericity-0.215989)/0.176837)*((aplanarity-0.055205)/0.054264)) + -0.264243*(((sphericity-0.215989)/0.176837)*((Ntrack_gencut-6.041230)/3.183411)) + -0.196424*(((sphericity-0.215989)/0.176837)*((NTBz_gencut-0.657215)/0.310822)) + -0.140673*(((sphericity-0.215989)/0.176837)*((foxWolframR1-0.015728)/0.049540)) + -2.609016*(((sphericity-0.215989)/0.176837)*((foxWolframR2-0.494220)/0.271963)) + 0.144869*(((sphericity-0.215989)/0.176837)*((foxWolframR3-0.068553)/0.074331)) + 0.065929*(((aplanarity-0.055205)/0.054264)^2) + -0.147164*(((aplanarity-0.055205)/0.054264)*((Ntrack_gencut-6.041230)/3.183411)) + 0.358409*(((aplanarity-0.055205)/0.054264)*((NTBz_gencut-0.657215)/0.310822)) + 0.157688*(((aplanarity-0.055205)/0.054264)*((foxWolframR1-0.015728)/0.049540)) + -0.793957*(((aplanarity-0.055205)/0.054264)*((foxWolframR2-0.494220)/0.271963)) + 0.053505*(((aplanarity-0.055205)/0.054264)*((foxWolframR3-0.068553)/0.074331)) + -0.459730*(((Ntrack_gencut-6.041230)/3.183411)^2) + 0.200784*(((Ntrack_gencut-6.041230)/3.183411)*((NTBz_gencut-0.657215)/0.310822)) + 0.101365*(((Ntrack_gencut-6.041230)/3.183411)*((foxWolframR1-0.015728)/0.049540)) + -1.241904*(((Ntrack_gencut-6.041230)/3.183411)*((foxWolframR2-0.494220)/0.271963)) + -0.063816*(((Ntrack_gencut-6.041230)/3.183411)*((foxWolframR3-0.068553)/0.074331)) + -0.218220*(((NTBz_gencut-0.657215)/0.310822)^2) + 0.150167*(((NTBz_gencut-0.657215)/0.310822)*((foxWolframR1-0.015728)/0.049540)) + 1.124364*(((NTBz_gencut-0.657215)/0.310822)*((foxWolframR2-0.494220)/0.271963)) + -0.087189*(((NTBz_gencut-0.657215)/0.310822)*((foxWolframR3-0.068553)/0.074331)) + 0.024363*(((foxWolframR1-0.015728)/0.049540)^2) + -0.376432*(((foxWolframR1-0.015728)/0.049540)*((foxWolframR2-0.494220)/0.271963)) + 0.061083*(((foxWolframR1-0.015728)/0.049540)*((foxWolframR3-0.068553)/0.074331)) + 1.542677*(((foxWolframR2-0.494220)/0.271963)^2) + 0.269758*(((foxWolframR2-0.494220)/0.271963)*((foxWolframR3-0.068553)/0.074331)) + -0.051452*(((foxWolframR3-0.068553)/0.074331)^2) + (-0.778680))"
    )

    # event cut
    ma.applyEventCuts("[[nParticlesInList(tau+:fake) > 0.5] and [LL_score_A_quad > -3.50633]] or [[nParticlesInList(tau+:fake) < 0.5] and [LL_score_B_quad > 0.71855]]", path=path)

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

# generate uubar events
ge.add_continuum_generator(path=main, finalstate='uubar', eventType='uubar')

# generator level cut
GenCutUUBAR(path=main)

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