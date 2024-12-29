#ifndef MYOBTAINWEIGHT_H
#define MYOBTAINWEIGHT_H

#include <vector>
#include <string>

#include "data.h"
#include "constants.h"

double MyScaleFunction(std::vector<Data>::iterator data_) {
    if ((*data_).filename.find("CHG_") != std::string::npos) return Scale_CHG_MC15ri;
    else if ((*data_).filename.find("MIX_") != std::string::npos) return Scale_MIX_MC15ri;
    else if ((*data_).filename.find("UUBAR_") != std::string::npos) return Scale_UUBAR_MC15ri;
    else if ((*data_).filename.find("DDBAR_") != std::string::npos) return Scale_DDBAR_MC15ri;
    else if ((*data_).filename.find("SSBAR_") != std::string::npos) return Scale_SSBAR_MC15ri;
    else if ((*data_).filename.find("CHARM_") != std::string::npos) return Scale_CHARM_MC15ri;
    else if ((*data_).filename.find("MUMU_") != std::string::npos) return Scale_MUMU_MC15ri;
    else if ((*data_).filename.find("EE_") != std::string::npos) return Scale_EE_MC15ri;
    else if ((*data_).filename.find("EEEE_") != std::string::npos) return Scale_EEEE_MC15ri;
    else if ((*data_).filename.find("EEMUMU_") != std::string::npos) return Scale_EEMUMU_MC15ri;
    else if ((*data_).filename.find("EEPIPI_") != std::string::npos) return Scale_EEPIPI_MC15ri;
    else if ((*data_).filename.find("EEKK_") != std::string::npos) return Scale_EEKK_MC15ri;
    else if ((*data_).filename.find("EEPP_") != std::string::npos) return Scale_EEPP_MC15ri;
    else if ((*data_).filename.find("PIPIISR_") != std::string::npos) return Scale_PIPIISR_MC15ri;
    else if ((*data_).filename.find("KKISR_") != std::string::npos) return Scale_KKISR_MC15ri;
    else if ((*data_).filename.find("GG_") != std::string::npos) return Scale_GG_MC15ri;
    else if ((*data_).filename.find("EETAUTAU_") != std::string::npos) return Scale_EETAUTAU_MC15ri;
    else if ((*data_).filename.find("K0K0BARISR_") != std::string::npos) return Scale_K0K0BARISR_MC15ri;
    else if ((*data_).filename.find("MUMUMUMU_") != std::string::npos) return Scale_MUMUMUMU_MC15ri;
    else if ((*data_).filename.find("MUMUTAUTAU_") != std::string::npos) return Scale_MUMUTAUTAU_MC15ri;
    else if ((*data_).filename.find("TAUTAUTAUTAU_") != std::string::npos) return Scale_TAUTAUTAUTAU_MC15ri;
    else if ((*data_).filename.find("TAUPAIR_") != std::string::npos) return Scale_TAUPAIR_MC15ri;
    else if ((*data_).filename.find("SIGNAL_") != std::string::npos) return Scale_SIGNAL_MC15ri;
    else {
        printf("unexpected sample type\n");
        exit(1);
        return 0.0;
    }
    printf("unexpected sample type\n");
    exit(1);
    return 0.0;
}

#endif 