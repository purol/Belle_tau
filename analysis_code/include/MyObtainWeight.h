#ifndef MYOBTAINWEIGHT_H
#define MYOBTAINWEIGHT_H

#include <vector>
#include <deque>
#include <string>

#include "data.h"
#include "constants.h"
#include "eventweight.h"

EventWeight double_weight = EventWeight(2.0);

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

/* legacy code
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