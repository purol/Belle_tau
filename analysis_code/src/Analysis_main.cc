#include <stdio.h>
#include <string>
#include <vector>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"

double reserve_function(std::vector<Data>::iterator data_) {
    
}

int main(int argc, char* argv[]) {
    /*
    * argv[1]: dirname
    * argv[2]: outputname
    * argv[3]: output path
    */

    Loader loader("tau_lfv");

    loader.Load(argv[1], ".root", "label");
    loader.Cut("(0.5 < extraInfo__bodecayModeID__bc) && (extraInfo__bodecayModeID__bc < 1.5)"); // select tau -> mu mu mu only

    loader.PrintInformation("========== initial ==========");

    loader.Cut("(-0.296904 < deltaE) && (deltaE < 0.20147)");
    loader.PrintInformation("========== -0.296904 < deltaE < 0.20147 ==========");

    loader.Cut("(1.681298 < (E*E-px*px-py*py-pz*pz)^0.5) && ((E*E-px*px-py*py-pz*pz)^0.5 < 1.8656)");
    loader.PrintInformation("========== 1.681298 < M < 1.8656 ==========");

    //loader.DrawTH2D("(E*E-px*px-py*py-pz*pz)^0.5", "deltaE", ";M [GeV];deltaE [GeV];", 50, 1.681298, 1.8656, 50, -0.296904, 0.20147, "M_deltaE_before_cut.png");

    loader.Cut("(0.3 < daughter__bo0__cm__sppx__bc) && (0.6 < daughter__bo0__cmmuonID__bc)");
    loader.Cut("(0.3 < daughter__bo1__cm__sppx__bc) && (0.6 < daughter__bo1__cmmuonID__bc)");
    loader.Cut("(0.3 < daughter__bo2__cm__sppx__bc) && (0.6 < daughter__bo2__cmmuonID__bc)");
    loader.PrintInformation("========== 0.3 < p_muon && 0.6 < muonID ==========");

    //loader.DrawTH1D("(E*E-px*px-py*py-pz*pz)^0.5", "M_after_cut", "M_after_cut.png");
    //loader.DrawTH1D("deltaE", "deltaE_after_cut", "deltaE_after_cut.png");
    //loader.DrawTH2D("(E*E-px*px-py*py-pz*pz)^0.5", "deltaE", ";M [GeV];deltaE [GeV];", 50, 1.681298, 1.8656, 50, -0.296904, 0.20147, "M_deltaE_after_cut.png");

    //loader.DrawTH1D("missingMomentumOfEventCMS_theta", "theta_miss", "theta_miss.png");
    loader.Cut("(0.3 < missingMomentumOfEventCMS_theta) && (missingMomentumOfEventCMS_theta < 2.7)");
    loader.PrintInformation("========== 0.3 < theta_miss < 2.7 ==========");

    loader.Cut("(0.89 < thrust) && (thrust < 0.97)");
    loader.PrintInformation("========== 0.89 < thrust < 0.97 ==========");

    loader.PrintSeparateRootFile("./", "", "_after_cut");

    loader.end();

    return 0;
}
