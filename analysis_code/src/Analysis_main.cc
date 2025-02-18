#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "MyModule.h"

std::map<std::string, std::string> momentum_muonID = {
    {"daughter__bo0__cm__spp__bc", "daughter__bo0__cmmuonID__bc"},
    {"daughter__bo1__cm__spp__bc", "daughter__bo1__cmmuonID__bc"},
    {"daughter__bo2__cm__spp__bc", "daughter__bo2__cmmuonID__bc"}
};

std::map<std::string, std::string> momentum_muonmomentum = {
    {"daughter__bo0__cm__spp__bc", "daughter__bo0__cm__spp__bc"},
    {"daughter__bo1__cm__spp__bc", "daughter__bo1__cm__spp__bc"},
    {"daughter__bo2__cm__spp__bc", "daughter__bo2__cm__spp__bc"}
};

std::map<std::string, std::string> momentum_isolation = {
    {"daughter__bo0__cm__spp__bc", "daughter__bo0__cmminET2ETIsoScoreAsWeightedAvg__bomu__pl__cltaulfv__cm__sp0__cm__spECL__cm__spKLM__bc__bc"},
    {"daughter__bo1__cm__spp__bc", "daughter__bo1__cmminET2ETIsoScoreAsWeightedAvg__bomu__pl__cltaulfv__cm__sp0__cm__spECL__cm__spKLM__bc__bc"},
    {"daughter__bo2__cm__spp__bc", "daughter__bo2__cmminET2ETIsoScoreAsWeightedAvg__bomu__pl__cltaulfv__cm__sp0__cm__spECL__cm__spKLM__bc__bc"}
};

int main(int argc, char* argv[]) {
    /*
    * argv[1]: dirname
    * argv[2]: including string
    * argv[3]: output path
    */

    ObtainWeight = MyScaleFunction;

    Loader loader("tau_lfv");

    loader.Load(argv[1], argv[2], "label");

    loader.ConditionalPairDefineNewVariable(momentum_muonmomentum, 0, "first_muon_p");
    loader.ConditionalPairDefineNewVariable(momentum_muonmomentum, 1, "second_muon_p");
    loader.ConditionalPairDefineNewVariable(momentum_muonmomentum, 2, "third_muon_p");
    loader.ConditionalPairDefineNewVariable(momentum_muonID, 0, "first_muon_muonID");
    loader.ConditionalPairDefineNewVariable(momentum_muonID, 1, "second_muon_muonID");
    loader.ConditionalPairDefineNewVariable(momentum_muonID, 2, "third_muon_muonID");
    loader.ConditionalPairDefineNewVariable(momentum_isolation, 0, "first_muon_isolation");
    loader.ConditionalPairDefineNewVariable(momentum_isolation, 1, "second_muon_isolation");
    loader.ConditionalPairDefineNewVariable(momentum_isolation, 2, "third_muon_isolation");
    loader.DefineNewVariable("(E*E-px*px-py*py-pz*pz)^0.5", "M_inv_tau");
    loader.DefineNewVariable("charge*roeCharge__bocleanMask__bc", "charge_times_ROEcharge");
    loader.DefineNewVariable("(flightTime/flightTimeErr)", "flightTime_dividedby_flightTimeErr");

    loader.PrintInformation("========== initial ==========");

    loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_M_deltaE_selection").c_str(), "", "");
    loader.Cut("(-0.9 < deltaE) && (deltaE < 0.4)");
    loader.PrintInformation("========== -0.9 < deltaE < 0.4 ==========");
    loader.Cut("(1.5 < M_inv_tau) && (M_inv_tau < 1.9)");
    loader.PrintInformation("========== 1.5 < M < 1.9 ==========");
    //loader.DrawTH2D("(E*E-px*px-py*py-pz*pz)^0.5", "deltaE", ";M [GeV];deltaE [GeV];", 50, 1.3, 1.9, 50, -0.9, 0.4, "M_deltaE_before_cut.png");

    loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_PrimarymuonID_selection").c_str(), "", "");
    loader.Cut("0.5 < first_muon_muonID");
    loader.PrintInformation("========== 0.5 < muonID for leading muon ==========");

    loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_SecondarymuonID_selection").c_str(), "", "");
    loader.Cut("0.5 < second_muon_muonID");
    loader.PrintInformation("========== 0.5 < muonID for secondary muon ==========");

    loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_SecondarymuonP_selection").c_str(), "", "");
    loader.Cut("0.3 < second_muon_p");
    loader.PrintInformation("========== 0.3 < muon momentum for secondary muon ==========");

    loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_theta_miss_cut").c_str(), "", "");
    //loader.DrawTH1D("missingMomentumOfEventCMS_theta", "theta_miss", "theta_miss.png");
    loader.Cut("(0.3 < missingMomentumOfEventCMS_theta) && (missingMomentumOfEventCMS_theta < 2.7)");
    loader.PrintInformation("========== 0.3 < theta_miss < 2.7 ==========");

    loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_thrust_cut").c_str(), "", "");
    //loader.DrawTH1D("thrust", "thrust", "thrust.png");
    loader.Cut("(0.89 < thrust) && (thrust < 0.97)");
    loader.PrintInformation("========== 0.89 < thrust < 0.97 ==========");

    loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_strict_M_deltaE_selection").c_str(), "", "");

    loader.end();

    return 0;
}
