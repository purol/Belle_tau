#include <stdio.h>
#include <string>
#include <vector>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"

int main(int argc, char* argv[]) {
    /*
    * argv[1]: dirname
    * argv[2]: including string
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
