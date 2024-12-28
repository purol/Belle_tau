#include <stdio.h>
#include <string>
#include <vector>

#include "TFile.h"

#include "Loader.h"
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
    loader.Cut("(0.5 < extraInfo__bodecayModeID__bc) && (extraInfo__bodecayModeID__bc < 1.5)");

    loader.PrintInformation("========== initial ==========");

    loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_M_deltaE_selection").c_str(), "", "");

    loader.Cut("(-0.9 < deltaE) && (deltaE < 0.4)");
    loader.PrintInformation("========== -0.9 < deltaE < 0.4 ==========");

    loader.Cut("(1.3 < (E*E-px*px-py*py-pz*pz)^0.5) && ((E*E-px*px-py*py-pz*pz)^0.5 < 1.9)");
    loader.PrintInformation("========== 1.3 < M < 1.9 ==========");

    //loader.DrawTH2D("(E*E-px*px-py*py-pz*pz)^0.5", "deltaE", ";M [GeV];deltaE [GeV];", 50, 1.3, 1.9, 50, -0.9, 0.4, "M_deltaE_before_cut.png");

    loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_momentum_selection").c_str(), "", "");

    loader.Cut("0.3 < (daughter__bo0__cm__sppx__bc*daughter__bo0__cm__sppx__bc+daughter__bo0__cm__sppy__bc*daughter__bo0__cm__sppy__bc+daughter__bo0__cm__sppz__bc*daughter__bo0__cm__sppz__bc)^0.5");
    loader.Cut("0.3 < (daughter__bo1__cm__sppx__bc*daughter__bo1__cm__sppx__bc+daughter__bo1__cm__sppy__bc*daughter__bo1__cm__sppy__bc+daughter__bo1__cm__sppz__bc*daughter__bo1__cm__sppz__bc)^0.5");
    loader.Cut("0.3 < (daughter__bo2__cm__sppx__bc*daughter__bo2__cm__sppx__bc+daughter__bo2__cm__sppy__bc*daughter__bo2__cm__sppy__bc+daughter__bo2__cm__sppz__bc*daughter__bo2__cm__sppz__bc)^0.5");
    loader.PrintInformation("========== 0.3 < p_muon ==========");
    
    loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_muonID_selection").c_str(), "", "");

    loader.Cut("0.5 < daughter__bo0__cmmuonID__bc");
    loader.Cut("0.5 < daughter__bo1__cmmuonID__bc");
    loader.Cut("0.5 < daughter__bo2__cmmuonID__bc");
    loader.PrintInformation("========== 0.5 < muonID ==========");

    //loader.DrawTH1D("(E*E-px*px-py*py-pz*pz)^0.5", "M_after_cut", "M_after_cut.png");
    //loader.DrawTH1D("deltaE", "deltaE_after_cut", "deltaE_after_cut.png");
    //loader.DrawTH2D("(E*E-px*px-py*py-pz*pz)^0.5", "deltaE", ";M [GeV];deltaE [GeV];", 50, 1.3, 1.9, 50, -0.9, 0.4, "M_deltaE_after_cut.png");

    loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_theta_miss_cut").c_str(), "", "");

    //loader.DrawTH1D("missingMomentumOfEventCMS_theta", "theta_miss", "theta_miss.png");
    loader.Cut("(0.3 < missingMomentumOfEventCMS_theta) && (missingMomentumOfEventCMS_theta < 2.7)");
    loader.PrintInformation("========== 0.3 < theta_miss < 2.7 ==========");

    loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_thrust_cut").c_str(), "", "");

    //loader.DrawTH1D("thrust", "thrust", "thrust.png");
    loader.Cut("(0.89 < thrust) && (thrust < 0.97)");
    loader.PrintInformation("========== 0.89 < thrust < 0.97 ==========");

    loader.PrintSeparateRootFile((std::string(argv[3]) + "/final_output").c_str(), "", "");

    loader.end();

    return 0;
}
