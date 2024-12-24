#include <stdio.h>
#include <string>
#include <vector>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"

double reserve_function(std::vector<Data>::iterator data_) {
    if (data_.find("CHG_") != std::string::npos) return Scale_CHG_MC15ri;
    else if (data_.find("MIX_") != std::string::npos) return Scale_MIX_MC15ri;
    else if (data_.find("UUBAR_") != std::string::npos) return Scale_UUBAR_MC15ri;
    else if (data_.find("DDBAR_") != std::string::npos) return Scale_DDBAR_MC15ri;
    else if (data_.find("SSBAR_") != std::string::npos) return Scale_SSBAR_MC15ri;
    else if (data_.find("CHARM_") != std::string::npos) return Scale_CHARM_MC15ri;
    else if (data_.find("MUMU_") != std::string::npos) return Scale_MUMU_MC15ri;
    else if (data_.find("EE_") != std::string::npos) return Scale_EE_MC15ri;
    else if (data_.find("EEEE_") != std::string::npos) return Scale_EEEE_MC15ri;
    else if (data_.find("EEMUMU_") != std::string::npos) return Scale_EEMUMU_MC15ri;
    else if (data_.find("EEPIPI_") != std::string::npos) return Scale_EEPIPI_MC15ri;
    else if (data_.find("EEKK_") != std::string::npos) return Scale_EEKK_MC15ri;
    else if (data_.find("EEPP_") != std::string::npos) return Scale_EEPP_MC15ri;
    else if (data_.find("PIPIISR_") != std::string::npos) return Scale_PIPIISR_MC15ri;
    else if (data_.find("KKISR_") != std::string::npos) return Scale_KKISR_MC15ri;
    else if (data_.find("GG_") != std::string::npos) return Scale_GG_MC15ri;
    else if (data_.find("EETAUTAU_") != std::string::npos) return Scale_EETAUTAU_MC15ri;
    else if (data_.find("K0K0BARISR_") != std::string::npos) return Scale_K0K0BARISR_MC15ri;
    else if (data_.find("MUMUMUMU_") != std::string::npos) return Scale_MUMUMUMU_MC15ri;
    else if (data_.find("MUMUTAUTAU_") != std::string::npos) return Scale_MUMUTAUTAU_MC15ri;
    else if (data_.find("TAUTAUTAUTAU_") != std::string::npos) return Scale_TAUTAUTAUTAU_MC15ri;
    else if (data_.find("TAUPAIR_") != std::string::npos) return Scale_TAUPAIR_MC15ri;
    else if (data_.find("SIGNAL_") != std::string::npos) return 1.0;
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
    * argv[2]: output path
    */

    ObtainWeight = reserve_function;

    Loader loader("tau_lfv");

    loader.Load(argv[1], ".root", "label");
    loader.Cut("(0.5 < extraInfo__bodecayModeID__bc) && (extraInfo__bodecayModeID__bc < 1.5)"); // select tau -> mu mu mu only

    loader.PrintInformation("========== initial ==========");

    loader.Cut("(-0.296904 < deltaE) && (deltaE < 0.20147)");
    loader.PrintInformation("========== -0.296904 < deltaE < 0.20147 ==========");

    loader.Cut("(1.681298 < (E*E-px*px-py*py-pz*pz)^0.5) && ((E*E-px*px-py*py-pz*pz)^0.5 < 1.8656)");
    loader.PrintInformation("========== 1.681298 < M < 1.8656 ==========");

    //loader.DrawTH2D("(E*E-px*px-py*py-pz*pz)^0.5", "deltaE", ";M [GeV];deltaE [GeV];", 50, 1.681298, 1.8656, 50, -0.296904, 0.20147, "M_deltaE_before_cut.png");

    loader.PrintSeparateRootFile((std::string(argv[2]) + "/before_muon_selection").c_str(), "", "");

    loader.Cut("(0.3 < daughter__bo0__cm__sppx__bc) && (0.6 < daughter__bo0__cmmuonID__bc)");
    loader.Cut("(0.3 < daughter__bo1__cm__sppx__bc) && (0.6 < daughter__bo1__cmmuonID__bc)");
    loader.Cut("(0.3 < daughter__bo2__cm__sppx__bc) && (0.6 < daughter__bo2__cmmuonID__bc)");
    loader.PrintInformation("========== 0.3 < p_muon && 0.6 < muonID ==========");

    //loader.DrawTH1D("(E*E-px*px-py*py-pz*pz)^0.5", "M_after_cut", "M_after_cut.png");
    //loader.DrawTH1D("deltaE", "deltaE_after_cut", "deltaE_after_cut.png");
    //loader.DrawTH2D("(E*E-px*px-py*py-pz*pz)^0.5", "deltaE", ";M [GeV];deltaE [GeV];", 50, 1.681298, 1.8656, 50, -0.296904, 0.20147, "M_deltaE_after_cut.png");

    loader.PrintSeparateRootFile((std::string(argv[2]) + "/before_theta_miss_cut").c_str(), "", "");

    //loader.DrawTH1D("missingMomentumOfEventCMS_theta", "theta_miss", "theta_miss.png");
    loader.Cut("(0.3 < missingMomentumOfEventCMS_theta) && (missingMomentumOfEventCMS_theta < 2.7)");
    loader.PrintInformation("========== 0.3 < theta_miss < 2.7 ==========");

    loader.PrintSeparateRootFile((std::string(argv[2]) + "/before_thrust_cut").c_str(), "", "");

    //loader.DrawTH1D("thrust", "thrust", "thrust.png");
    loader.Cut("(0.89 < thrust) && (thrust < 0.97)");
    loader.PrintInformation("========== 0.89 < thrust < 0.97 ==========");

    loader.PrintSeparateRootFile((std::string(argv[2]) + "/final_output").c_str(), "", "");

    loader.end();

    return 0;
}
