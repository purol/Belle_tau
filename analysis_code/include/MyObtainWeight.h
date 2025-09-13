#ifndef MYOBTAINWEIGHT_H
#define MYOBTAINWEIGHT_H

#include <vector>
#include <string>

#include "data.h"
#include "constants.h"

double MyScaleFunction(std::vector<Data>::iterator data_, std::vector<std::string> variable_names_) {

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
        }
        else if ((1.5 < EnergyType) && (EnergyType < 2.5)) { // off-resonance
            if ((2.5 < EventType) && (EventType < 3.5)) return Scale_BelleII_off_UUBAR_MC15ri; // UUBAR
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
        else if ((4.5 < EnergyType) && (EnergyType < 5.5)) { // 10810
            if ((0.5 < EventType) && (EventType < 1.5)) return Scale_BelleII_10810_CHG_MC15ri; // CHG
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
    else if ((4.5 < SampleType) && (SampleType < 5.5)) {} // Belle data
    else if ((5.5 < SampleType) && (SampleType < 6.5)) { // Belle MC
        return 1.0;
    }

    printf("unexpected sample type\n");
    exit(1);
    return 0.0;
}

double MyScaleFunction_halfsplit(std::vector<Data>::iterator data_, std::vector<std::string> variable_names_) {

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
        printf("why do you try to split data?\n");
        exit(1);
        return 0.0;
    }
    else if ((0.5 < SampleType) && (SampleType < 1.5)) { // MC15ri
        if ((0.5 < EnergyType) && (EnergyType < 1.5)) { // 4S
            if ((-0.5 < EventType) && (EventType < 0.5)) return 2.0 * Scale_SIGNAL_BelleII_4S_MC15ri; // signal
            else if ((0.5 < EventType) && (EventType < 1.5)) return 2.0 * Scale_BelleII_4S_CHG_MC15ri; // CHG
            else if ((1.5 < EventType) && (EventType < 2.5)) return 2.0 * Scale_BelleII_4S_MIX_MC15ri; // MIX
            else if ((2.5 < EventType) && (EventType < 3.5)) return 2.0 * Scale_BelleII_4S_UUBAR_MC15ri; // UUBAR
            else if ((3.5 < EventType) && (EventType < 4.5)) return 2.0 * Scale_BelleII_4S_DDBAR_MC15ri; // DDBAR
            else if ((4.5 < EventType) && (EventType < 5.5)) return 2.0 * Scale_BelleII_4S_SSBAR_MC15ri; // SSBAR
            else if ((5.5 < EventType) && (EventType < 6.5)) return 2.0 * Scale_BelleII_4S_CHARM_MC15ri; // CHARM
            else if ((6.5 < EventType) && (EventType < 7.5)) return 2.0 * Scale_BelleII_4S_MUMU_MC15ri; // MUMU
            else if ((7.5 < EventType) && (EventType < 8.5)) return 2.0 * Scale_BelleII_4S_EE_MC15ri; // EE
            else if ((8.5 < EventType) && (EventType < 9.5)) return 2.0 * Scale_BelleII_4S_EEEE_MC15ri; // EEEE
            else if ((9.5 < EventType) && (EventType < 10.5)) return 2.0 * Scale_BelleII_4S_EEMUMU_MC15ri; // EEMUMU
            else if ((10.5 < EventType) && (EventType < 11.5)) return 2.0 * Scale_BelleII_4S_EEPIPI_MC15ri; // EEPIPI
            else if ((11.5 < EventType) && (EventType < 12.5)) return 2.0 * Scale_BelleII_4S_EEKK_MC15ri; // EEKK
            else if ((12.5 < EventType) && (EventType < 13.5)) return 2.0 * Scale_BelleII_4S_EEPP_MC15ri; // EEPP
            else if ((13.5 < EventType) && (EventType < 14.5)) return 2.0 * Scale_BelleII_4S_PIPIISR_MC15ri; // PIPIISR
            else if ((14.5 < EventType) && (EventType < 15.5)) return 2.0 * Scale_BelleII_4S_KKISR_MC15ri; // KKISR
            else if ((15.5 < EventType) && (EventType < 16.5)) return 2.0 * Scale_BelleII_4S_GG_MC15ri; // GG
            else if ((16.5 < EventType) && (EventType < 17.5)) return 2.0 * Scale_BelleII_4S_EETAUTAU_MC15ri; // EETAUTAU
            else if ((17.5 < EventType) && (EventType < 18.5)) return 2.0 * Scale_BelleII_4S_K0K0BARISR_MC15ri; // K0K0BARISR
            else if ((18.5 < EventType) && (EventType < 19.5)) return 2.0 * Scale_BelleII_4S_MUMUMUMU_MC15ri; // MUMUMUMU
            else if ((19.5 < EventType) && (EventType < 20.5)) return 2.0 * Scale_BelleII_4S_MUMUTAUTAU_MC15ri; // MUMUTAUTAU
            else if ((20.5 < EventType) && (EventType < 21.5)) return 2.0 * Scale_BelleII_4S_TAUTAUTAUTAU_MC15ri; // TAUTAUTAUTAU
            else if ((21.5 < EventType) && (EventType < 22.5)) return 2.0 * Scale_BelleII_4S_TAUPAIR_MC15ri; // TAUPAIR
            else if ((22.5 < EventType) && (EventType < 23.5)) return 2.0 * Scale_BelleII_4S_PIPIPI0ISR_MC15ri; // PIPIPI0ISR
        }
        else if ((1.5 < EnergyType) && (EnergyType < 2.5)) { // off-resonance
            if ((2.5 < EventType) && (EventType < 3.5)) return 2.0 * Scale_BelleII_off_UUBAR_MC15ri; // UUBAR
            else if ((3.5 < EventType) && (EventType < 4.5)) return 2.0 * Scale_BelleII_off_DDBAR_MC15ri; // DDBAR
            else if ((4.5 < EventType) && (EventType < 5.5)) return 2.0 * Scale_BelleII_off_SSBAR_MC15ri; // SSBAR
            else if ((5.5 < EventType) && (EventType < 6.5)) return 2.0 * Scale_BelleII_off_CHARM_MC15ri; // CHARM
            else if ((6.5 < EventType) && (EventType < 7.5)) return 2.0 * Scale_BelleII_off_MUMU_MC15ri; // MUMU
            else if ((7.5 < EventType) && (EventType < 8.5)) return 2.0 * Scale_BelleII_off_EE_MC15ri; // EE
            else if ((8.5 < EventType) && (EventType < 9.5)) return 2.0 * Scale_BelleII_off_EEEE_MC15ri; // EEEE
            else if ((9.5 < EventType) && (EventType < 10.5)) return 2.0 * Scale_BelleII_off_EEMUMU_MC15ri; // EEMUMU
            else if ((10.5 < EventType) && (EventType < 11.5)) return 2.0 * Scale_BelleII_off_EEPIPI_MC15ri; // EEPIPI
            else if ((11.5 < EventType) && (EventType < 12.5)) return 2.0 * Scale_BelleII_off_EEKK_MC15ri; // EEKK
            else if ((12.5 < EventType) && (EventType < 13.5)) return 2.0 * Scale_BelleII_off_EEPP_MC15ri; // EEPP
            else if ((15.5 < EventType) && (EventType < 16.5)) return 2.0 * Scale_BelleII_off_GG_MC15ri; // GG
            else if ((16.5 < EventType) && (EventType < 17.5)) return 2.0 * Scale_BelleII_off_EETAUTAU_MC15ri; // EETAUTAU
            else if ((18.5 < EventType) && (EventType < 19.5)) return 2.0 * Scale_BelleII_off_MUMUMUMU_MC15ri; // MUMUMUMU
            else if ((21.5 < EventType) && (EventType < 22.5)) return 2.0 * Scale_BelleII_off_TAUPAIR_MC15ri; // TAUPAIR
        }
        else if ((2.5 < EnergyType) && (EnergyType < 3.5)) {} // 10657
        else if ((3.5 < EnergyType) && (EnergyType < 4.5)) {} // 10706
        else if ((4.5 < EnergyType) && (EnergyType < 5.5)) {} // 10751
        else if ((4.5 < EnergyType) && (EnergyType < 5.5)) { // 10810
            if ((0.5 < EventType) && (EventType < 1.5)) return 2.0 * Scale_BelleII_10810_CHG_MC15ri; // CHG
            else if ((1.5 < EventType) && (EventType < 2.5)) return 2.0 * Scale_BelleII_10810_MIX_MC15ri; // MIX
            else if ((2.5 < EventType) && (EventType < 3.5)) return 2.0 * Scale_BelleII_10810_UUBAR_MC15ri; // UUBAR
            else if ((3.5 < EventType) && (EventType < 4.5)) return 2.0 * Scale_BelleII_10810_DDBAR_MC15ri; // DDBAR
            else if ((4.5 < EventType) && (EventType < 5.5)) return 2.0 * Scale_BelleII_10810_SSBAR_MC15ri; // SSBAR
            else if ((5.5 < EventType) && (EventType < 6.5)) return 2.0 * Scale_BelleII_10810_CHARM_MC15ri; // CHARM
            else if ((6.5 < EventType) && (EventType < 7.5)) return 2.0 * Scale_BelleII_10810_MUMU_MC15ri; // MUMU
            else if ((21.5 < EventType) && (EventType < 22.5)) return 2.0 * Scale_BelleII_10810_TAUPAIR_MC15ri; // TAUPAIR
            else if ((23.5 < EventType) && (EventType < 24.5)) return 2.0 * Scale_BelleII_10810_BBs_MC15ri; // BBs
            else if ((24.5 < EventType) && (EventType < 25.5)) return 2.0 * Scale_BelleII_10810_BsBs_MC15ri; // BsBs
        }
    }
    else if ((1.5 < SampleType) && (SampleType < 2.5)) {} // MC15rd
    else if ((2.5 < SampleType) && (SampleType < 3.5)) {} // MC16ri
    else if ((3.5 < SampleType) && (SampleType < 4.5)) {} // MC16rd
    else if ((4.5 < SampleType) && (SampleType < 5.5)) {} // Belle data
    else if ((5.5 < SampleType) && (SampleType < 6.5)) { // Belle MC
        printf("why do you try to split data?\n");
        exit(1);
        return 0.0;
    }

    printf("unexpected sample type\n");
    exit(1);
    return 0.0;
}

#endif 