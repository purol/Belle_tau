#include <stdio.h>
#include <string>
#include <vector>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"

# define tau_crosssection 0.919 // nb
# define Nevt_taupair ((0.36537/0.000000001) * tau_crosssection)

# define Nevt_SIGNAL_MC15ri 3597981

# define BR_SIGNAL 0.00000001 // just set 10^(-8) 
# define Nevt_SIGNAL (Nevt_taupair * BR_SIGNAL * 2.0)
# define Scale_SIGNAL_MC15ri (Nevt_SIGNAL/Nevt_SIGNAL_MC15ri)

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

int main(int argc, char* argv[]) {
    /*
    * argv[1]: dirname
    * argv[2]: filename
    * argv[3]: output path
    */

    ObtainWeight = MyScaleFunction;

    Loader loader("tau_lfv");

    loader.Load(argv[1], argv[2], "label");
    loader.Cut("0.1 < daughter__bo0__cmmuonID__bc");
    loader.Cut("0.1 < daughter__bo1__cmmuonID__bc");
    loader.Cut("0.1 < daughter__bo2__cmmuonID__bc");
    loader.Cut("(0.5 < extraInfo__bodecayModeID__bc) && (extraInfo__bodecayModeID__bc < 1.5)"); // select tau -> mu mu mu only

    loader.PrintSeparateRootFile((std::string(argv[3]) + "/preselection_output").c_str(), "", "");

    loader.end();

    return 0;
}
