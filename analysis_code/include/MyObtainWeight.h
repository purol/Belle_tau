#ifndef MYOBTAINWEIGHT_H
#define MYOBTAINWEIGHT_H

#include <vector>
#include <deque>
#include <string>

#include "data.h"
#include "constants.h"
#include "eventweight.h"

EventWeight double_weight = EventWeight(2.0);

EventWeight train_weight = EventWeight(4.0/3.0);
EventWeight test_weight = EventWeight(4.0);

EventWeight MC_weight(
    {
        "MySampleType",
        "MyEventType",
        "MyEnergyType",
        "MyALPLife"
    },
    {
        { { { -1.5, -0.5 }, { -10000.0, 10000.0 }, { -10000.0, 10000.0 }, { -10000.0, 10000.0 } }, 1.0 },  // Belle II data
        { { { 0.5, 1.5 }, { -0.5, 0.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_SIGNAL_BelleII_4S_MC15ri },  // MC15ri, 4S, signal
        { { { 0.5, 1.5 }, { 0.5, 1.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_CHG_MC15ri },  // MC15ri, 4S, CHG
        { { { 0.5, 1.5 }, { 1.5, 2.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_MIX_MC15ri },  // MC15ri, 4S, MIX
        { { { 0.5, 1.5 }, { 2.5, 3.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_UUBAR_MC15ri },  // MC15ri, 4S, UUBAR
        { { { 0.5, 1.5 }, { 3.5, 4.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_DDBAR_MC15ri },  // MC15ri, 4S, DDBAR
        { { { 0.5, 1.5 }, { 4.5, 5.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_SSBAR_MC15ri },  // MC15ri, 4S, SSBAR
        { { { 0.5, 1.5 }, { 5.5, 6.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_CHARM_MC15ri },  // MC15ri, 4S, CHARM
        { { { 0.5, 1.5 }, { 6.5, 7.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_MUMU_MC15ri },  // MC15ri, 4S, MUMU
        { { { 0.5, 1.5 }, { 7.5, 8.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_EE_MC15ri },  // MC15ri, 4S, EE
        { { { 0.5, 1.5 }, { 8.5, 9.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_EEEE_MC15ri },  // MC15ri, 4S, EEEE
        { { { 0.5, 1.5 }, { 9.5, 10.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_EEMUMU_MC15ri },  // MC15ri, 4S, EEMUMU
        { { { 0.5, 1.5 }, { 10.5, 11.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_EEPIPI_MC15ri },  // MC15ri, 4S, EEPIPI
        { { { 0.5, 1.5 }, { 11.5, 12.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_EEKK_MC15ri },  // MC15ri, 4S, EEKK
        { { { 0.5, 1.5 }, { 12.5, 13.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_EEPP_MC15ri },  // MC15ri, 4S, EEPP
        { { { 0.5, 1.5 }, { 13.5, 14.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_PIPIISR_MC15ri },  // MC15ri, 4S, PIPIISR
        { { { 0.5, 1.5 }, { 14.5, 15.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_KKISR_MC15ri },  // MC15ri, 4S, KKISR
        { { { 0.5, 1.5 }, { 15.5, 16.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_GG_MC15ri },  // MC15ri, 4S, GG
        { { { 0.5, 1.5 }, { 16.5, 17.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_EETAUTAU_MC15ri },  // MC15ri, 4S, EETAUTAU
        { { { 0.5, 1.5 }, { 17.5, 18.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_K0K0BARISR_MC15ri },  // MC15ri, 4S, K0K0BARISR
        { { { 0.5, 1.5 }, { 18.5, 19.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_MUMUMUMU_MC15ri },  // MC15ri, 4S, MUMUMUMU
        { { { 0.5, 1.5 }, { 19.5, 20.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_MUMUTAUTAU_MC15ri },  // MC15ri, 4S, MUMUTAUTAU
        { { { 0.5, 1.5 }, { 20.5, 21.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_TAUTAUTAUTAU_MC15ri },  // MC15ri, 4S, TAUTAUTAUTAU
        { { { 0.5, 1.5 }, { 21.5, 22.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_TAUPAIR_MC15ri },  // MC15ri, 4S, TAUPAIR
        { { { 0.5, 1.5 }, { 22.5, 23.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_PIPIPI0ISR_MC15ri },  // MC15ri, 4S, PIPIPI0ISR
        { { { 0.5, 1.5 }, { 31.5, 32.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_ALP_BelleII_4S_MC15ri },  // MC15ri, 4S, ALP
        { { { 0.5, 1.5 }, { -0.5, 0.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_SIGNAL_BelleII_off_MC15ri },  // MC15ri, off-resonance, signal
        { { { 0.5, 1.5 }, { 2.5, 3.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_UUBAR_MC15ri },  // MC15ri, off-resonance, UUBAR
        { { { 0.5, 1.5 }, { 3.5, 4.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_DDBAR_MC15ri },  // MC15ri, off-resonance, DDBAR
        { { { 0.5, 1.5 }, { 4.5, 5.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_SSBAR_MC15ri },  // MC15ri, off-resonance, SSBAR
        { { { 0.5, 1.5 }, { 5.5, 6.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_CHARM_MC15ri },  // MC15ri, off-resonance, CHARM
        { { { 0.5, 1.5 }, { 6.5, 7.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_MUMU_MC15ri },  // MC15ri, off-resonance, MUMU
        { { { 0.5, 1.5 }, { 7.5, 8.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_EE_MC15ri },  // MC15ri, off-resonance, EE
        { { { 0.5, 1.5 }, { 8.5, 9.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_EEEE_MC15ri },  // MC15ri, off-resonance, EEEE
        { { { 0.5, 1.5 }, { 9.5, 10.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_EEMUMU_MC15ri },  // MC15ri, off-resonance, EEMUMU
        { { { 0.5, 1.5 }, { 10.5, 11.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_EEPIPI_MC15ri },  // MC15ri, off-resonance, EEPIPI
        { { { 0.5, 1.5 }, { 11.5, 12.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_EEKK_MC15ri },  // MC15ri, off-resonance, EEKK
        { { { 0.5, 1.5 }, { 12.5, 13.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_EEPP_MC15ri },  // MC15ri, off-resonance, EEPP
        { { { 0.5, 1.5 }, { 15.5, 16.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_GG_MC15ri },  // MC15ri, off-resonance, GG
        { { { 0.5, 1.5 }, { 16.5, 17.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_EETAUTAU_MC15ri },  // MC15ri, off-resonance, EETAUTAU
        { { { 0.5, 1.5 }, { 18.5, 19.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_MUMUMUMU_MC15ri },  // MC15ri, off-resonance, MUMUMUMU
        { { { 0.5, 1.5 }, { 21.5, 22.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_TAUPAIR_MC15ri },  // MC15ri, off-resonance, TAUPAIR
        { { { 0.5, 1.5 }, { -0.5, 0.5 }, { 5.5, 6.5 }, { -10000.0, 10000.0 } }, Scale_SIGNAL_BelleII_10810_MC15ri },  // MC15ri, 10810, signal
        { { { 0.5, 1.5 }, { 0.5, 1.5 }, { 5.5, 6.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_10810_CHG_MC15ri },  // MC15ri, 10810, CHG
        { { { 0.5, 1.5 }, { 1.5, 2.5 }, { 5.5, 6.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_10810_MIX_MC15ri },  // MC15ri, 10810, MIX
        { { { 0.5, 1.5 }, { 2.5, 3.5 }, { 5.5, 6.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_10810_UUBAR_MC15ri },  // MC15ri, 10810, UUBAR
        { { { 0.5, 1.5 }, { 3.5, 4.5 }, { 5.5, 6.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_10810_DDBAR_MC15ri },  // MC15ri, 10810, DDBAR
        { { { 0.5, 1.5 }, { 4.5, 5.5 }, { 5.5, 6.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_10810_SSBAR_MC15ri },  // MC15ri, 10810, SSBAR
        { { { 0.5, 1.5 }, { 5.5, 6.5 }, { 5.5, 6.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_10810_CHARM_MC15ri },  // MC15ri, 10810, CHARM
        { { { 0.5, 1.5 }, { 6.5, 7.5 }, { 5.5, 6.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_10810_MUMU_MC15ri },  // MC15ri, 10810, MUMU
        { { { 0.5, 1.5 }, { 21.5, 22.5 }, { 5.5, 6.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_10810_TAUPAIR_MC15ri },  // MC15ri, 10810, TAUPAIR
        { { { 0.5, 1.5 }, { 23.5, 24.5 }, { 5.5, 6.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_10810_BBs_MC15ri },  // MC15ri, 10810, BBs
        { { { 0.5, 1.5 }, { 24.5, 25.5 }, { 5.5, 6.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_10810_BsBs_MC15ri },  // MC15ri, 10810, BsBs
        { { { 2.5, 3.5 }, { -0.5, 0.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_SIGNAL_BelleII_4S_MC16ri },  // MC16ri, 4S, signal
        { { { 2.5, 3.5 }, { 0.5, 1.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_CHG_MC16ri },  // MC16ri, 4S, CHG
        { { { 2.5, 3.5 }, { 1.5, 2.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_MIX_MC16ri },  // MC16ri, 4S, MIX
        { { { 2.5, 3.5 }, { 2.5, 3.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_UUBAR_MC16ri },  // MC16ri, 4S, UUBAR
        { { { 2.5, 3.5 }, { 3.5, 4.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_DDBAR_MC16ri },  // MC16ri, 4S, DDBAR
        { { { 2.5, 3.5 }, { 4.5, 5.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_SSBAR_MC16ri },  // MC16ri, 4S, SSBAR
        { { { 2.5, 3.5 }, { 5.5, 6.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_CHARM_MC16ri },  // MC16ri, 4S, CHARM
        { { { 2.5, 3.5 }, { 6.5, 7.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_MUMU_MC16ri },  // MC16ri, 4S, MUMU
        { { { 2.5, 3.5 }, { 7.5, 8.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_EE_MC16ri },  // MC16ri, 4S, EE
        { { { 2.5, 3.5 }, { 8.5, 9.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_EEEE_MC16ri },  // MC16ri, 4S, EEEE
        { { { 2.5, 3.5 }, { 9.5, 10.5 }, { 0.5, 1.5 } , { -10000.0, 10000.0 }}, Scale_BelleII_4S_EEMUMU_MC16ri },  // MC16ri, 4S, EEMUMU
        { { { 2.5, 3.5 }, { 33.5, 34.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_hhISR_MC16ri },  // MC16ri, 4S, hhISR
        { { { 2.5, 3.5 }, { 15.5, 16.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_GG_MC16ri },  // MC16ri, 4S, GG
        { { { 2.5, 3.5 }, { 34.5, 35.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_llXX_MC16ri },  // MC16ri, 4S, llXX
        { { { 2.5, 3.5 }, { 21.5, 22.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_TAUPAIR_MC16ri },  // MC16ri, 4S, TAUPAIR
        { { { 2.5, 3.5 }, { 31.5, 32.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_ALP_BelleII_4S_MC16ri },  // MC16ri, 4S, ALP
        { { { 3.5, 4.5 }, { -0.5, 0.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_SIGNAL_BelleII_4S_MC16rd },  // MC16rd, 4S, signal
        { { { 3.5, 4.5 }, { 0.5, 1.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_CHG_MC16rd },  // MC16rd, 4S, CHG
        { { { 3.5, 4.5 }, { 1.5, 2.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_MIX_MC16rd },  // MC16rd, 4S, MIX
        { { { 3.5, 4.5 }, { 2.5, 3.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_UUBAR_MC16rd },  // MC16rd, 4S, UUBAR
        { { { 3.5, 4.5 }, { 3.5, 4.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_DDBAR_MC16rd },  // MC16rd, 4S, DDBAR
        { { { 3.5, 4.5 }, { 4.5, 5.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_SSBAR_MC16rd },  // MC16rd, 4S, SSBAR
        { { { 3.5, 4.5 }, { 5.5, 6.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_CHARM_MC16rd },  // MC16rd, 4S, CHARM
        { { { 3.5, 4.5 }, { 6.5, 7.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_MUMU_MC16rd },  // MC16rd, 4S, MUMU
        { { { 3.5, 4.5 }, { 7.5, 8.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_EE_MC16rd },  // MC16rd, 4S, EE
        { { { 3.5, 4.5 }, { 8.5, 9.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_EEEE_MC16rd },  // MC16rd, 4S, EEEE
        { { { 3.5, 4.5 }, { 9.5, 10.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_EEMUMU_MC16rd },  // MC16rd, 4S, EEMUMU
        { { { 3.5, 4.5 }, { 33.5, 34.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_hhISR_MC16rd },  // MC16rd, 4S, hhISR
        { { { 3.5, 4.5 }, { 15.5, 16.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_GG_MC16rd },  // MC16rd, 4S, GG
        { { { 3.5, 4.5 }, { 34.5, 35.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_llXX_MC16rd },  // MC16rd, 4S, llXX
        { { { 3.5, 4.5 }, { 21.5, 22.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_TAUPAIR_MC16rd },  // MC16rd, 4S, TAUPAIR
        { { { 3.5, 4.5 }, { 32.5, 33.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_BB_MC16rd },  // MC16rd, 4S, BB
        { { { 3.5, 4.5 }, { 35.5, 36.5 }, { 0.5, 1.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_4S_UDSC_MC16rd },  // MC16rd, 4S, UDSC
        { { { 3.5, 4.5 }, { 31.5, 32.5 }, { 0.5, 1.5 }, { 0.1 - 0.01, 0.1 + 0.01 } }, Scale_ALP_BelleII_4S_MC16rd_ctau_01 },  // MC16rd, 4S, ALP_ctau_0.1
        { { { 3.5, 4.5 }, { 31.5, 32.5 }, { 0.5, 1.5 }, { 1.0 - 0.01, 1.0 + 0.01 } }, Scale_ALP_BelleII_4S_MC16rd_ctau_1 },  // MC16rd, 4S, ALP_ctau_1
        { { { 3.5, 4.5 }, { 31.5, 32.5 }, { 0.5, 1.5 }, { 10.0 - 0.01, 10.0 + 0.01 } }, Scale_ALP_BelleII_4S_MC16rd_ctau_10 },  // MC16rd, 4S, ALP_ctau_10
        { { { 3.5, 4.5 }, { 31.5, 32.5 }, { 0.5, 1.5 }, { 100.0 - 0.01, 100.0 + 0.01 } }, Scale_ALP_BelleII_4S_MC16rd_ctau_100 },  // MC16rd, 4S, ALP_ctau_100
        { { { 3.5, 4.5 }, { 31.5, 32.5 }, { 0.5, 1.5 }, { 250.0 - 0.01, 250.0 + 0.01 } }, Scale_ALP_BelleII_4S_MC16rd_ctau_250 },  // MC16rd, 4S, ALP_ctau_250
        { { { 3.5, 4.5 }, { 31.5, 32.5 }, { 0.5, 1.5 }, { 500.0 - 0.01, 500.0 + 0.01 } }, Scale_ALP_BelleII_4S_MC16rd_ctau_500 },  // MC16rd, 4S, ALP_ctau_500
        { { { 3.5, 4.5 }, { 31.5, 32.5 }, { 0.5, 1.5 }, { 1000.0 - 0.01, 1000.0 + 0.01 } }, Scale_ALP_BelleII_4S_MC16rd_ctau_1000 },  // MC16rd, 4S, ALP_ctau_1000
        { { { 3.5, 4.5 }, { -0.5, 0.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_SIGNAL_BelleII_off_MC16rd },  // MC16rd, off-resonance, signal
        { { { 3.5, 4.5 }, { 2.5, 3.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_UUBAR_MC16rd },  // MC16rd, off-resonance, UUBAR
        { { { 3.5, 4.5 }, { 3.5, 4.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_DDBAR_MC16rd },  // MC16rd, off-resonance, DDBAR
        { { { 3.5, 4.5 }, { 4.5, 5.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_SSBAR_MC16rd },  // MC16rd, off-resonance, SSBAR
        { { { 3.5, 4.5 }, { 5.5, 6.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_CHARM_MC16rd },  // MC16rd, off-resonance, CHARM
        { { { 3.5, 4.5 }, { 6.5, 7.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_MUMU_MC16rd },  // MC16rd, off-resonance, MUMU
        { { { 3.5, 4.5 }, { 7.5, 8.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_EE_MC16rd },  // MC16rd, off-resonance, EE
        { { { 3.5, 4.5 }, { 8.5, 9.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_EEEE_MC16rd },  // MC16rd, off-resonance, EEEE
        { { { 3.5, 4.5 }, { 9.5, 10.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_EEMUMU_MC16rd },  // MC16rd, off-resonance, EEMUMU
        { { { 3.5, 4.5 }, { 33.5, 34.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_hhISR_MC16rd },  // MC16rd, off-resonance, hhISR
        { { { 3.5, 4.5 }, { 15.5, 16.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_GG_MC16rd },  // MC16rd, off-resonance, GG
        { { { 3.5, 4.5 }, { 34.5, 35.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_llXX_MC16rd },  // MC16rd, off-resonance, llXX
        { { { 3.5, 4.5 }, { 21.5, 22.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_TAUPAIR_MC16rd },  // MC16rd, off-resonance, TAUPAIR
        { { { 3.5, 4.5 }, { 35.5, 36.5 }, { 1.5, 2.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_off_UDSC_MC16rd },  // MC16rd, off-resonance, UDSC
        { { { 3.5, 4.5 }, { -0.5, 0.5 }, { 6.5, 7.5 }, { -10000.0, 10000.0 } }, Scale_SIGNAL_BelleII_5S_MC16rd },  // MC16rd, 5S, signal
        { { { 3.5, 4.5 }, { 2.5, 3.5 }, { 6.5, 7.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_5S_UUBAR_MC16rd },  // MC16rd, 5S, UUBAR
        { { { 3.5, 4.5 }, { 3.5, 4.5 }, { 6.5, 7.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_5S_DDBAR_MC16rd },  // MC16rd, 5S, DDBAR
        { { { 3.5, 4.5 }, { 4.5, 5.5 }, { 6.5, 7.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_5S_SSBAR_MC16rd },  // MC16rd, 5S, SSBAR
        { { { 3.5, 4.5 }, { 5.5, 6.5 }, { 6.5, 7.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_5S_CHARM_MC16rd },  // MC16rd, 5S, CHARM
        { { { 3.5, 4.5 }, { 6.5, 7.5 }, { 6.5, 7.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_5S_MUMU_MC16rd },  // MC16rd, 5S, MUMU
        { { { 3.5, 4.5 }, { 7.5, 8.5 }, { 6.5, 7.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_5S_EE_MC16rd },  // MC16rd, 5S, EE
        { { { 3.5, 4.5 }, { 8.5, 9.5 }, { 6.5, 7.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_5S_EEEE_MC16rd },  // MC16rd, 5S, EEEE
        { { { 3.5, 4.5 }, { 9.5, 10.5 }, { 6.5, 7.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_5S_EEMUMU_MC16rd },  // MC16rd, 5S, EEMUMU
        { { { 3.5, 4.5 }, { 33.5, 34.5 }, { 6.5, 7.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_5S_hhISR_MC16rd },  // MC16rd, 5S, hhISR
        { { { 3.5, 4.5 }, { 15.5, 16.5 }, { 6.5, 7.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_5S_GG_MC16rd },  // MC16rd, 5S, GG
        { { { 3.5, 4.5 }, { 34.5, 35.5 }, { 6.5, 7.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_5S_llXX_MC16rd },  // MC16rd, 5S, llXX
        { { { 3.5, 4.5 }, { 21.5, 22.5 }, { 6.5, 7.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_5S_TAUPAIR_MC16rd },  // MC16rd, 5S, TAUPAIR
        { { { 3.5, 4.5 }, { 35.5, 36.5 }, { 6.5, 7.5 }, { -10000.0, 10000.0 } }, Scale_BelleII_5S_UDSC_MC16rd },  // MC16rd, 5S, UDSC
        { { { 4.5, 5.5 }, { -10000.0, 10000.0 }, { -10000.0, 10000.0 }, { -10000.0, 10000.0 } }, 1.0 }  // Belle data
    },
    false
);

EventWeight muonID_05(
    "/home/belle2/junewoo/storage_b2/tau_workspace/tables/muonID_csv/MC15ri/my_mu_efficiency_table_05.csv",
    {
        {"charge",   "charge_min", "charge_max"},
        {"momentum", "p_min",      "p_max"},
        {"theta",    "theta_min",  "theta_max"}
    },
    "data_MC_ratio",
    {
        {"stat", "data_MC_uncertainty_stat_up", "data_MC_uncertainty_stat_dn", false},
        {"syst", "data_MC_uncertainty_sys_up",  "data_MC_uncertainty_sys_dn",  false}
    },
    true
);

EventWeight luminosity_scale(
    { "MyEnergyType" },
    {
        {
            { {0.5, 1.5} }, 1.0,
            { { "luminosity", (lumi_BelleII_4S_uncertainty / lumi_BelleII_4S), (lumi_BelleII_4S_uncertainty / lumi_BelleII_4S) } }
        },
        {
            { {1.5, 2.5} }, 1.0,
            { { "luminosity", (lumi_BelleII_off_uncertainty / lumi_BelleII_off), (lumi_BelleII_off_uncertainty / lumi_BelleII_off) } }
        },
        {
            { {5.5, 6.5} }, 1.0,
            { { "luminosity", (lumi_BelleII_10810_uncertainty / lumi_BelleII_10810), (lumi_BelleII_10810_uncertainty / lumi_BelleII_10810) } }
        },
        {
            { {6.5, 7.5} }, 1.0,
            { { "luminosity", (lumi_BelleII_5S_uncertainty / lumi_BelleII_5S), (lumi_BelleII_5S_uncertainty / lumi_BelleII_5S) } }
        }
    },
    {
        {"luminosity", false}
    },
    false
);

EventWeight KS0_tracking(
    "/home/belle2/junewoo/storage_b2/tau_workspace/tables/KS0_tracking_csv/MC15rd/KS0_tracking_correction_MC15rd_converted.csv",
    {
        {"theta",    "thetamin", "thetamax"},
        {"momentum", "pmin",     "pmax"},
        {"distance", "dmin",     "dmax"}
    },
    "w",
    {
        {"stat", "w_e_stat", "w_e_stat", false},
        {"syst", "w_e_syst", "w_e_syst", false}
    },
    true
);

/* legacy code
EventWeight MC_weight = EventWeight(
    {
        "MySampleType",
        "MyEventType",
        "MyEnergyType"
    },
    {
        { -1.5, -10000, -10000 },  // Belle II data
        { 0.5, -0.5, 0.5 },  // MC15ri, 4S, signal
        { 0.5, 0.5, 0.5 },  // MC15ri, 4S, CHG
        { 0.5, 1.5, 0.5 },  // MC15ri, 4S, MIX
        { 0.5, 2.5, 0.5 },  // MC15ri, 4S, UUBAR
        { 0.5, 3.5, 0.5 },  // MC15ri, 4S, DDBAR
        { 0.5, 4.5, 0.5 },  // MC15ri, 4S, SSBAR
        { 0.5, 5.5, 0.5 },  // MC15ri, 4S, CHARM
        { 0.5, 6.5, 0.5 },  // MC15ri, 4S, MUMU
        { 0.5, 7.5, 0.5 },  // MC15ri, 4S, EE
        { 0.5, 8.5, 0.5 },  // MC15ri, 4S, EEEE
        { 0.5, 9.5, 0.5 },  // MC15ri, 4S, EEMUMU
        { 0.5, 10.5, 0.5 },  // MC15ri, 4S, EEPIPI
        { 0.5, 11.5, 0.5 },  // MC15ri, 4S, EEKK
        { 0.5, 12.5, 0.5 },  // MC15ri, 4S, EEPP
        { 0.5, 13.5, 0.5 },  // MC15ri, 4S, PIPIISR
        { 0.5, 14.5, 0.5 },  // MC15ri, 4S, KKISR
        { 0.5, 15.5, 0.5 },  // MC15ri, 4S, GG
        { 0.5, 16.5, 0.5 },  // MC15ri, 4S, EETAUTAU
        { 0.5, 17.5, 0.5 },  // MC15ri, 4S, K0K0BARISR
        { 0.5, 18.5, 0.5 },  // MC15ri, 4S, MUMUMUMU
        { 0.5, 19.5, 0.5 },  // MC15ri, 4S, MUMUTAUTAU
        { 0.5, 20.5, 0.5 },  // MC15ri, 4S, TAUTAUTAUTAU
        { 0.5, 21.5, 0.5 },  // MC15ri, 4S, TAUPAIR
        { 0.5, 22.5, 0.5 },  // MC15ri, 4S, PIPIPI0ISR
        { 0.5, 31.5, 0.5 },  // MC15ri, 4S, ALP
        { 0.5, -0.5, 1.5 },  // MC15ri, off-resonance, signal
        { 0.5, 2.5, 1.5 },  // MC15ri, off-resonance, UUBAR
        { 0.5, 3.5, 1.5 },  // MC15ri, off-resonance, DDBAR
        { 0.5, 4.5, 1.5 },  // MC15ri, off-resonance, SSBAR
        { 0.5, 5.5, 1.5 },  // MC15ri, off-resonance, CHARM
        { 0.5, 6.5, 1.5 },  // MC15ri, off-resonance, MUMU
        { 0.5, 7.5, 1.5 },  // MC15ri, off-resonance, EE
        { 0.5, 8.5, 1.5 },  // MC15ri, off-resonance, EEEE
        { 0.5, 9.5, 1.5 },  // MC15ri, off-resonance, EEMUMU
        { 0.5, 10.5, 1.5 },  // MC15ri, off-resonance, EEPIPI
        { 0.5, 11.5, 1.5 },  // MC15ri, off-resonance, EEKK
        { 0.5, 12.5, 1.5 },  // MC15ri, off-resonance, EEPP
        { 0.5, 15.5, 1.5 },  // MC15ri, off-resonance, GG
        { 0.5, 16.5, 1.5 },  // MC15ri, off-resonance, EETAUTAU
        { 0.5, 18.5, 1.5 },  // MC15ri, off-resonance, MUMUMUMU
        { 0.5, 21.5, 1.5 },  // MC15ri, off-resonance, TAUPAIR
        { 0.5, -0.5, 5.5 },  // MC15ri, 10810, signal
        { 0.5, 0.5, 5.5 },  // MC15ri, 10810, CHG
        { 0.5, 1.5, 5.5 },  // MC15ri, 10810, MIX
        { 0.5, 2.5, 5.5 },  // MC15ri, 10810, UUBAR
        { 0.5, 3.5, 5.5 },  // MC15ri, 10810, DDBAR
        { 0.5, 4.5, 5.5 },  // MC15ri, 10810, SSBAR
        { 0.5, 5.5, 5.5 },  // MC15ri, 10810, CHARM
        { 0.5, 6.5, 5.5 },  // MC15ri, 10810, MUMU
        { 0.5, 21.5, 5.5 },  // MC15ri, 10810, TAUPAIR
        { 0.5, 23.5, 5.5 },  // MC15ri, 10810, BBs
        { 0.5, 24.5, 5.5 },  // MC15ri, 10810, BsBs
        { 4.5, -10000, -10000 },  // Belle data
    },
    {
        { -0.5, 10000, 10000 },  // Belle II data
        { 1.5, 0.5, 1.5 },  // MC15ri, 4S, signal
        { 1.5, 1.5, 1.5 },  // MC15ri, 4S, CHG
        { 1.5, 2.5, 1.5 },  // MC15ri, 4S, MIX
        { 1.5, 3.5, 1.5 },  // MC15ri, 4S, UUBAR
        { 1.5, 4.5, 1.5 },  // MC15ri, 4S, DDBAR
        { 1.5, 5.5, 1.5 },  // MC15ri, 4S, SSBAR
        { 1.5, 6.5, 1.5 },  // MC15ri, 4S, CHARM
        { 1.5, 7.5, 1.5 },  // MC15ri, 4S, MUMU
        { 1.5, 8.5, 1.5 },  // MC15ri, 4S, EE
        { 1.5, 9.5, 1.5 },  // MC15ri, 4S, EEEE
        { 1.5, 10.5, 1.5 },  // MC15ri, 4S, EEMUMU
        { 1.5, 11.5, 1.5 },  // MC15ri, 4S, EEPIPI
        { 1.5, 12.5, 1.5 },  // MC15ri, 4S, EEKK
        { 1.5, 13.5, 1.5 },  // MC15ri, 4S, EEPP
        { 1.5, 14.5, 1.5 },  // MC15ri, 4S, PIPIISR
        { 1.5, 15.5, 1.5 },  // MC15ri, 4S, KKISR
        { 1.5, 16.5, 1.5 },  // MC15ri, 4S, GG
        { 1.5, 17.5, 1.5 },  // MC15ri, 4S, EETAUTAU
        { 1.5, 18.5, 1.5 },  // MC15ri, 4S, K0K0BARISR
        { 1.5, 19.5, 1.5 },  // MC15ri, 4S, MUMUMUMU
        { 1.5, 20.5, 1.5 },  // MC15ri, 4S, MUMUTAUTAU
        { 1.5, 21.5, 1.5 },  // MC15ri, 4S, TAUTAUTAUTAU
        { 1.5, 22.5, 1.5 },  // MC15ri, 4S, TAUPAIR
        { 1.5, 23.5, 1.5 },  // MC15ri, 4S, PIPIPI0ISR
        { 1.5, 32.5, 1.5 },  // MC15ri, 4S, ALP
        { 1.5, 0.5, 2.5 },  // MC15ri, off-resonance, signal
        { 1.5, 3.5, 2.5 },  // MC15ri, off-resonance, UUBAR
        { 1.5, 4.5, 2.5 },  // MC15ri, off-resonance, DDBAR
        { 1.5, 5.5, 2.5 },  // MC15ri, off-resonance, SSBAR
        { 1.5, 6.5, 2.5 },  // MC15ri, off-resonance, CHARM
        { 1.5, 7.5, 2.5 },  // MC15ri, off-resonance, MUMU
        { 1.5, 8.5, 2.5 },  // MC15ri, off-resonance, EE
        { 1.5, 9.5, 2.5 },  // MC15ri, off-resonance, EEEE
        { 1.5, 10.5, 2.5 },  // MC15ri, off-resonance, EEMUMU
        { 1.5, 11.5, 2.5 },  // MC15ri, off-resonance, EEPIPI
        { 1.5, 12.5, 2.5 },  // MC15ri, off-resonance, EEKK
        { 1.5, 13.5, 2.5 },  // MC15ri, off-resonance, EEPP
        { 1.5, 16.5, 2.5 },  // MC15ri, off-resonance, GG
        { 1.5, 17.5, 2.5 },  // MC15ri, off-resonance, EETAUTAU
        { 1.5, 19.5, 2.5 },  // MC15ri, off-resonance, MUMUMUMU
        { 1.5, 22.5, 2.5 },  // MC15ri, off-resonance, TAUPAIR
        { 1.5, 0.5, 6.5 },  // MC15ri, 10810, signal
        { 1.5, 1.5, 6.5 },  // MC15ri, 10810, CHG
        { 1.5, 2.5, 6.5 },  // MC15ri, 10810, MIX
        { 1.5, 3.5, 6.5 },  // MC15ri, 10810, UUBAR
        { 1.5, 4.5, 6.5 },  // MC15ri, 10810, DDBAR
        { 1.5, 5.5, 6.5 },  // MC15ri, 10810, SSBAR
        { 1.5, 6.5, 6.5 },  // MC15ri, 10810, CHARM
        { 1.5, 7.5, 6.5 },  // MC15ri, 10810, MUMU
        { 1.5, 22.5, 6.5 },  // MC15ri, 10810, TAUPAIR
        { 1.5, 24.5, 6.5 },  // MC15ri, 10810, BBs
        { 1.5, 25.5, 6.5 },  // MC15ri, 10810, BsBs
        { 5.5, 10000, 10000 },  // Belle data
    },
    {
        1.0,  // Belle II data
        Scale_SIGNAL_BelleII_4S_MC15ri,  // MC15ri, 4S, signal
        Scale_BelleII_4S_CHG_MC15ri,  // MC15ri, 4S, CHG
        Scale_BelleII_4S_MIX_MC15ri,  // MC15ri, 4S, MIX
        Scale_BelleII_4S_UUBAR_MC15ri,  // MC15ri, 4S, UUBAR
        Scale_BelleII_4S_DDBAR_MC15ri,  // MC15ri, 4S, DDBAR
        Scale_BelleII_4S_SSBAR_MC15ri,  // MC15ri, 4S, SSBAR
        Scale_BelleII_4S_CHARM_MC15ri,  // MC15ri, 4S, CHARM
        Scale_BelleII_4S_MUMU_MC15ri,  // MC15ri, 4S, MUMU
        Scale_BelleII_4S_EE_MC15ri,  // MC15ri, 4S, EE
        Scale_BelleII_4S_EEEE_MC15ri,  // MC15ri, 4S, EEEE
        Scale_BelleII_4S_EEMUMU_MC15ri,  // MC15ri, 4S, EEMUMU
        Scale_BelleII_4S_EEPIPI_MC15ri,  // MC15ri, 4S, EEPIPI
        Scale_BelleII_4S_EEKK_MC15ri,  // MC15ri, 4S, EEKK
        Scale_BelleII_4S_EEPP_MC15ri,  // MC15ri, 4S, EEPP
        Scale_BelleII_4S_PIPIISR_MC15ri,  // MC15ri, 4S, PIPIISR
        Scale_BelleII_4S_KKISR_MC15ri,  // MC15ri, 4S, KKISR
        Scale_BelleII_4S_GG_MC15ri,  // MC15ri, 4S, GG
        Scale_BelleII_4S_EETAUTAU_MC15ri,  // MC15ri, 4S, EETAUTAU
        Scale_BelleII_4S_K0K0BARISR_MC15ri,  // MC15ri, 4S, K0K0BARISR
        Scale_BelleII_4S_MUMUMUMU_MC15ri,  // MC15ri, 4S, MUMUMUMU
        Scale_BelleII_4S_MUMUTAUTAU_MC15ri,  // MC15ri, 4S, MUMUTAUTAU
        Scale_BelleII_4S_TAUTAUTAUTAU_MC15ri,  // MC15ri, 4S, TAUTAUTAUTAU
        Scale_BelleII_4S_TAUPAIR_MC15ri,  // MC15ri, 4S, TAUPAIR
        Scale_BelleII_4S_PIPIPI0ISR_MC15ri,  // MC15ri, 4S, PIPIPI0ISR
        Scale_ALP_BelleII_4S_MC15ri,  // MC15ri, 4S, ALP
        Scale_SIGNAL_BelleII_off_MC15ri,  // MC15ri, off-resonance, signal
        Scale_BelleII_off_UUBAR_MC15ri,  // MC15ri, off-resonance, UUBAR
        Scale_BelleII_off_DDBAR_MC15ri,  // MC15ri, off-resonance, DDBAR
        Scale_BelleII_off_SSBAR_MC15ri,  // MC15ri, off-resonance, SSBAR
        Scale_BelleII_off_CHARM_MC15ri,  // MC15ri, off-resonance, CHARM
        Scale_BelleII_off_MUMU_MC15ri,  // MC15ri, off-resonance, MUMU
        Scale_BelleII_off_EE_MC15ri,  // MC15ri, off-resonance, EE
        Scale_BelleII_off_EEEE_MC15ri,  // MC15ri, off-resonance, EEEE
        Scale_BelleII_off_EEMUMU_MC15ri,  // MC15ri, off-resonance, EEMUMU
        Scale_BelleII_off_EEPIPI_MC15ri,  // MC15ri, off-resonance, EEPIPI
        Scale_BelleII_off_EEKK_MC15ri,  // MC15ri, off-resonance, EEKK
        Scale_BelleII_off_EEPP_MC15ri,  // MC15ri, off-resonance, EEPP
        Scale_BelleII_off_GG_MC15ri,  // MC15ri, off-resonance, GG
        Scale_BelleII_off_EETAUTAU_MC15ri,  // MC15ri, off-resonance, EETAUTAU
        Scale_BelleII_off_MUMUMUMU_MC15ri,  // MC15ri, off-resonance, MUMUMUMU
        Scale_BelleII_off_TAUPAIR_MC15ri,  // MC15ri, off-resonance, TAUPAIR
        Scale_SIGNAL_BelleII_10810_MC15ri,  // MC15ri, 10810, signal
        Scale_BelleII_10810_CHG_MC15ri,  // MC15ri, 10810, CHG
        Scale_BelleII_10810_MIX_MC15ri,  // MC15ri, 10810, MIX
        Scale_BelleII_10810_UUBAR_MC15ri,  // MC15ri, 10810, UUBAR
        Scale_BelleII_10810_DDBAR_MC15ri,  // MC15ri, 10810, DDBAR
        Scale_BelleII_10810_SSBAR_MC15ri,  // MC15ri, 10810, SSBAR
        Scale_BelleII_10810_CHARM_MC15ri,  // MC15ri, 10810, CHARM
        Scale_BelleII_10810_MUMU_MC15ri,  // MC15ri, 10810, MUMU
        Scale_BelleII_10810_TAUPAIR_MC15ri,  // MC15ri, 10810, TAUPAIR
        Scale_BelleII_10810_BBs_MC15ri,  // MC15ri, 10810, BBs
        Scale_BelleII_10810_BsBs_MC15ri,  // MC15ri, 10810, BsBs
        1.0,  // Belle data
    },
    false
);

EventWeight muonID_05 = EventWeight(
    "/home/belle2/junewoo/storage_b2/tau_workspace/tables/muonID_csv/MC15ri/my_mu_efficiency_table_05.csv",
    { {"charge", "charge_min", "charge_max"},  {"momentum", "p_min","p_max"}, {"theta", "theta_min","theta_max"} },
    "data_MC_ratio",
    { {"data_MC_uncertainty_stat_up", "data_MC_uncertainty_stat_dn", false}, {"data_MC_uncertainty_sys_up", "data_MC_uncertainty_sys_dn", false} },
    true
);

EventWeight luminosity_scale = EventWeight(
    { "MyEnergyType" },
    {
        { 0.5 },
        { 1.5 },
        { 5.5 }
    },
    {
        { 1.5 },
        { 2.5 },
        { 6.5 }
    },
    {
        1.0,
        1.0,
        1.0
    },
    {
        { (lumi_BelleII_4S_uncertainty / lumi_BelleII_4S), (lumi_BelleII_off_uncertainty / lumi_BelleII_off), (lumi_BelleII_10810_uncertainty / lumi_BelleII_10810) }
    },
    {
        { (lumi_BelleII_4S_uncertainty / lumi_BelleII_4S), (lumi_BelleII_off_uncertainty / lumi_BelleII_off), (lumi_BelleII_10810_uncertainty / lumi_BelleII_10810) }
    },
    {
        false
    },
    false
);

EventWeight KS0_tracking = EventWeight(
    "/home/belle2/junewoo/storage_b2/tau_workspace/tables/KS0_tracking_csv/MC15rd/KS0_tracking_correction_MC15rd_converted.csv",
    { {"theta", "thetamin", "thetamax"},  {"momentum", "pmin","pmax"}, {"distance", "dmin","dmax"} },
    "w",
    { {"w_e_stat", "w_e_stat", false}, {"w_e_syst", "w_e_syst", false} },
    true
);

double MyScaleFunction(std::deque<Data>::iterator data_, std::vector<std::string> variable_names_) {

    std::vector<std::string>::iterator it;

    // several index
    double SampleType;
    double EventType;
    double EnergyType;

    it = std::find(variable_names_.begin(), variable_names_.end(), "MySampleType");
    if (it != variable_names_.end()) {
        int index = std::distance(variable_names_.begin(), it);
        SampleType = std::get<double>((*data_).variable.at(index));
    }

    it = std::find(variable_names_.begin(), variable_names_.end(), "MyEventType");
    if (it != variable_names_.end()) {
        int index = std::distance(variable_names_.begin(), it);
        EventType = std::get<double>((*data_).variable.at(index));
    }

    it = std::find(variable_names_.begin(), variable_names_.end(), "MyEnergyType");
    if (it != variable_names_.end()) {
        int index = std::distance(variable_names_.begin(), it);
        EnergyType = std::get<double>((*data_).variable.at(index));
    }

    if ((-1.5 < SampleType) && (SampleType < -0.5)) { // data
        return 1.0;
    }
    else if ((0.5 < SampleType) && (SampleType < 1.5)) { // MC15ri
        if ((0.5 < EnergyType) && (EnergyType < 1.5)) { // 4S
            if ((-0.5 < EventType) && (EventType < 0.5)) return Scale_SIGNAL_BelleII_4S_MC15ri; // signal
            else if ((0.5 < EventType) && (EventType < 1.5)) return Scale_BelleII_4S_CHG_MC15ri; // CHG
            else if ((1.5 < EventType) && (EventType < 2.5)) return Scale_BelleII_4S_MIX_MC15ri; // MIX
            else if ((2.5 < EventType) && (EventType < 3.5)) return Scale_BelleII_4S_UUBAR_MC15ri; // UUBAR
            else if ((3.5 < EventType) && (EventType < 4.5)) return Scale_BelleII_4S_DDBAR_MC15ri; // DDBAR
            else if ((4.5 < EventType) && (EventType < 5.5)) return Scale_BelleII_4S_SSBAR_MC15ri; // SSBAR
            else if ((5.5 < EventType) && (EventType < 6.5)) return Scale_BelleII_4S_CHARM_MC15ri; // CHARM
            else if ((6.5 < EventType) && (EventType < 7.5)) return Scale_BelleII_4S_MUMU_MC15ri; // MUMU
            else if ((7.5 < EventType) && (EventType < 8.5)) return Scale_BelleII_4S_EE_MC15ri; // EE
            else if ((8.5 < EventType) && (EventType < 9.5)) return Scale_BelleII_4S_EEEE_MC15ri; // EEEE
            else if ((9.5 < EventType) && (EventType < 10.5)) return Scale_BelleII_4S_EEMUMU_MC15ri; // EEMUMU
            else if ((10.5 < EventType) && (EventType < 11.5)) return Scale_BelleII_4S_EEPIPI_MC15ri; // EEPIPI
            else if ((11.5 < EventType) && (EventType < 12.5)) return Scale_BelleII_4S_EEKK_MC15ri; // EEKK
            else if ((12.5 < EventType) && (EventType < 13.5)) return Scale_BelleII_4S_EEPP_MC15ri; // EEPP
            else if ((13.5 < EventType) && (EventType < 14.5)) return Scale_BelleII_4S_PIPIISR_MC15ri; // PIPIISR
            else if ((14.5 < EventType) && (EventType < 15.5)) return Scale_BelleII_4S_KKISR_MC15ri; // KKISR
            else if ((15.5 < EventType) && (EventType < 16.5)) return Scale_BelleII_4S_GG_MC15ri; // GG
            else if ((16.5 < EventType) && (EventType < 17.5)) return Scale_BelleII_4S_EETAUTAU_MC15ri; // EETAUTAU
            else if ((17.5 < EventType) && (EventType < 18.5)) return Scale_BelleII_4S_K0K0BARISR_MC15ri; // K0K0BARISR
            else if ((18.5 < EventType) && (EventType < 19.5)) return Scale_BelleII_4S_MUMUMUMU_MC15ri; // MUMUMUMU
            else if ((19.5 < EventType) && (EventType < 20.5)) return Scale_BelleII_4S_MUMUTAUTAU_MC15ri; // MUMUTAUTAU
            else if ((20.5 < EventType) && (EventType < 21.5)) return Scale_BelleII_4S_TAUTAUTAUTAU_MC15ri; // TAUTAUTAUTAU
            else if ((21.5 < EventType) && (EventType < 22.5)) return Scale_BelleII_4S_TAUPAIR_MC15ri; // TAUPAIR
            else if ((22.5 < EventType) && (EventType < 23.5)) return Scale_BelleII_4S_PIPIPI0ISR_MC15ri; // PIPIPI0ISR
            else if ((31.5 < EventType) && (EventType < 32.5)) return Scale_ALP_BelleII_4S_MC15ri; // ALP
        }
        else if ((1.5 < EnergyType) && (EnergyType < 2.5)) { // off-resonance
            if ((-0.5 < EventType) && (EventType < 0.5)) return Scale_SIGNAL_BelleII_off_MC15ri; // signal
            else if ((2.5 < EventType) && (EventType < 3.5)) return Scale_BelleII_off_UUBAR_MC15ri; // UUBAR
            else if ((3.5 < EventType) && (EventType < 4.5)) return Scale_BelleII_off_DDBAR_MC15ri; // DDBAR
            else if ((4.5 < EventType) && (EventType < 5.5)) return Scale_BelleII_off_SSBAR_MC15ri; // SSBAR
            else if ((5.5 < EventType) && (EventType < 6.5)) return Scale_BelleII_off_CHARM_MC15ri; // CHARM
            else if ((6.5 < EventType) && (EventType < 7.5)) return Scale_BelleII_off_MUMU_MC15ri; // MUMU
            else if ((7.5 < EventType) && (EventType < 8.5)) return Scale_BelleII_off_EE_MC15ri; // EE
            else if ((8.5 < EventType) && (EventType < 9.5)) return Scale_BelleII_off_EEEE_MC15ri; // EEEE
            else if ((9.5 < EventType) && (EventType < 10.5)) return Scale_BelleII_off_EEMUMU_MC15ri; // EEMUMU
            else if ((10.5 < EventType) && (EventType < 11.5)) return Scale_BelleII_off_EEPIPI_MC15ri; // EEPIPI
            else if ((11.5 < EventType) && (EventType < 12.5)) return Scale_BelleII_off_EEKK_MC15ri; // EEKK
            else if ((12.5 < EventType) && (EventType < 13.5)) return Scale_BelleII_off_EEPP_MC15ri; // EEPP
            else if ((15.5 < EventType) && (EventType < 16.5)) return Scale_BelleII_off_GG_MC15ri; // GG
            else if ((16.5 < EventType) && (EventType < 17.5)) return Scale_BelleII_off_EETAUTAU_MC15ri; // EETAUTAU
            else if ((18.5 < EventType) && (EventType < 19.5)) return Scale_BelleII_off_MUMUMUMU_MC15ri; // MUMUMUMU
            else if ((21.5 < EventType) && (EventType < 22.5)) return Scale_BelleII_off_TAUPAIR_MC15ri; // TAUPAIR
        }
        else if ((2.5 < EnergyType) && (EnergyType < 3.5)) {} // 10657
        else if ((3.5 < EnergyType) && (EnergyType < 4.5)) {} // 10706
        else if ((4.5 < EnergyType) && (EnergyType < 5.5)) {} // 10751
        else if ((5.5 < EnergyType) && (EnergyType < 6.5)) { // 10810
            if ((-0.5 < EventType) && (EventType < 0.5)) return Scale_SIGNAL_BelleII_10810_MC15ri; // signal
            else if ((0.5 < EventType) && (EventType < 1.5)) return Scale_BelleII_10810_CHG_MC15ri; // CHG
            else if ((1.5 < EventType) && (EventType < 2.5)) return Scale_BelleII_10810_MIX_MC15ri; // MIX
            else if ((2.5 < EventType) && (EventType < 3.5)) return Scale_BelleII_10810_UUBAR_MC15ri; // UUBAR
            else if ((3.5 < EventType) && (EventType < 4.5)) return Scale_BelleII_10810_DDBAR_MC15ri; // DDBAR
            else if ((4.5 < EventType) && (EventType < 5.5)) return Scale_BelleII_10810_SSBAR_MC15ri; // SSBAR
            else if ((5.5 < EventType) && (EventType < 6.5)) return Scale_BelleII_10810_CHARM_MC15ri; // CHARM
            else if ((6.5 < EventType) && (EventType < 7.5)) return Scale_BelleII_10810_MUMU_MC15ri; // MUMU
            else if ((21.5 < EventType) && (EventType < 22.5)) return Scale_BelleII_10810_TAUPAIR_MC15ri; // TAUPAIR
            else if ((23.5 < EventType) && (EventType < 24.5)) return Scale_BelleII_10810_BBs_MC15ri; // BBs
            else if ((24.5 < EventType) && (EventType < 25.5)) return Scale_BelleII_10810_BsBs_MC15ri; // BsBs
        }
    }
    else if ((1.5 < SampleType) && (SampleType < 2.5)) {} // MC15rd
    else if ((2.5 < SampleType) && (SampleType < 3.5)) {} // MC16ri
    else if ((3.5 < SampleType) && (SampleType < 4.5)) {} // MC16rd
    else if ((4.5 < SampleType) && (SampleType < 5.5)) { // Belle data
        return 1.0;
    }
    else if ((5.5 < SampleType) && (SampleType < 6.5)) {} // Belle MC

    printf("unexpected sample type\n");
    exit(1);
    return 0.0;
}
*/

#endif 