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
    {"extraInfo__boOneMuon_p__bc", "extraInfo__boOneMuon_muonID__bc"},
    {"extraInfo__boTwoMuon_p__bc", "extraInfo__boTwoMuon_muonID__bc"},
    {"extraInfo__boThreeMuon_p__bc", "extraInfo__boThreeMuon_muonID__bc"}
};

std::map<std::string, std::string> momentum_electronID = {
    {"extraInfo__boOneMuon_p__bc", "extraInfo__boOneMuon_electronID__bc"},
    {"extraInfo__boTwoMuon_p__bc", "extraInfo__boTwoMuon_electronID__bc"},
    {"extraInfo__boThreeMuon_p__bc", "extraInfo__boThreeMuon_electronID__bc"}
};

std::map<std::string, std::string> momentum_pionID = {
    {"extraInfo__boOneMuon_p__bc", "extraInfo__boOneMuon_pionID__bc"},
    {"extraInfo__boTwoMuon_p__bc", "extraInfo__boTwoMuon_pionID__bc"},
    {"extraInfo__boThreeMuon_p__bc", "extraInfo__boThreeMuon_pionID__bc"}
};

std::map<std::string, std::string> momentum_muonmomentum = {
    {"extraInfo__boOneMuon_p__bc", "extraInfo__boOneMuon_p__bc"},
    {"extraInfo__boTwoMuon_p__bc", "extraInfo__boTwoMuon_p__bc"},
    {"extraInfo__boThreeMuon_p__bc", "extraInfo__boThreeMuon_p__bc"}
};

std::map<std::string, std::string> momentum_isolation = {
    {"extraInfo__boOneMuon_p__bc", "extraInfo__boOneMuon_minET2ETIsoScoreAsWeightedAvg_mu_taulfv_0_ECL_KLM__bc"},
    {"extraInfo__boTwoMuon_p__bc", "extraInfo__boTwoMuon_minET2ETIsoScoreAsWeightedAvg_mu_taulfv_0_ECL_KLM__bc"},
    {"extraInfo__boThreeMuon_p__bc", "extraInfo__boThreeMuon_minET2ETIsoScoreAsWeightedAvg_mu_taulfv_0_ECL_KLM__bc"}
};

std::map<std::string, std::string> momentum_theta = {
    {"extraInfo__boOneMuon_p__bc", "extraInfo__boOneMuon_theta__bc"},
    {"extraInfo__boTwoMuon_p__bc", "extraInfo__boTwoMuon_theta__bc"},
    {"extraInfo__boThreeMuon_p__bc", "extraInfo__boThreeMuon_theta__bc"}
};

std::vector<std::string> cosToThrustOfEvent_CM = {
    "extraInfo__boOneMuon_cosToThrustOfEvent__bc",
    "extraInfo__boTwoMuon_cosToThrustOfEvent__bc",
    "extraInfo__boThreeMuon_cosToThrustOfEvent__bc"
};

int main(int argc, char* argv[]) {
    /*
    * argv[1]: dirname
    * argv[2]: including string
    * argv[3]: output path
    */

    EventWeights::Register("MC_weight", MC_weight);

    Loader loader("tau_lfv");

    // It is prompt decay analysis
    loader.LoadWithCut(argv[1], argv[2], "label", "(29.5 < extraInfo__bodecayModeID__bc) && (extraInfo__bodecayModeID__bc < 30.5)");
    loader.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"} });

    loader.RemoveVariable({ "nParticlesInList__botau__pl__clpipipi__bc" });
    loader.RemoveVariable({ "nParticlesInList__botau__pl__cldirect__bc" });

    loader.ConditionalPairDefineNewVariable(momentum_muonmomentum, 0, "first_muon_p");
    loader.ConditionalPairDefineNewVariable(momentum_muonmomentum, 1, "second_muon_p");
    loader.ConditionalPairDefineNewVariable(momentum_muonmomentum, 2, "third_muon_p");
    loader.ConditionalPairDefineNewVariable(momentum_muonID, 0, "first_muon_muonID");
    loader.ConditionalPairDefineNewVariable(momentum_muonID, 1, "second_muon_muonID");
    loader.ConditionalPairDefineNewVariable(momentum_muonID, 2, "third_muon_muonID");
    loader.ConditionalPairDefineNewVariable(momentum_electronID, 0, "first_muon_electronID");
    loader.ConditionalPairDefineNewVariable(momentum_electronID, 1, "second_muon_electronID");
    loader.ConditionalPairDefineNewVariable(momentum_electronID, 2, "third_muon_electronID");
    loader.ConditionalPairDefineNewVariable(momentum_pionID, 0, "first_muon_pionID");
    loader.ConditionalPairDefineNewVariable(momentum_pionID, 1, "second_muon_pionID");
    loader.ConditionalPairDefineNewVariable(momentum_pionID, 2, "third_muon_pionID");
    loader.ConditionalPairDefineNewVariable(momentum_isolation, 0, "first_muon_isolation");
    loader.ConditionalPairDefineNewVariable(momentum_isolation, 1, "second_muon_isolation");
    loader.ConditionalPairDefineNewVariable(momentum_isolation, 2, "third_muon_isolation");
    loader.ConditionalPairDefineNewVariable(momentum_theta, 0, "first_muon_theta");
    loader.ConditionalPairDefineNewVariable(momentum_theta, 1, "second_muon_theta");
    loader.ConditionalPairDefineNewVariable(momentum_theta, 2, "third_muon_theta");
    loader.ConditionalPairDefineNewVariable(momentum_charge, 0, "first_muon_charge");
    loader.ConditionalPairDefineNewVariable(momentum_charge, 1, "second_muon_charge");
    loader.ConditionalPairDefineNewVariable(momentum_charge, 2, "third_muon_charge");
    loader.DefineNewVariable("charge*roeCharge__bocleanMask__bc", "charge_times_ROEcharge");
    loader.DefineNewVariable("(flightTime/flightTimeErr)", "flightTime_dividedby_flightTimeErr");
    loader.GetAverage(cosToThrustOfEvent_CM, "avg_cosToThrustOfEvent_CM");
    loader.GetStdDev(cosToThrustOfEvent_CM, "stddev_cosToThrustOfEvent_CM");
    loader.GetDiff(cosToThrustOfEvent_CM, 0, "diff_cosToThrustOfEvent_CM");

    loader.PrintInformation("========== initial ==========");

    //loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_M_deltaE_selection").c_str(), "", "");
    loader.Cut("(-0.4 < deltaE) && (deltaE < 0)");
    loader.PrintInformation("========== -0.4 < deltaE < 0 ==========");
    loader.Cut("(0.5 < M) && (M < 1.7)");
    loader.PrintInformation("========== 0.5 < M < 1.7 ==========");
    //loader.DrawTH2D("(E*E-px*px-py*py-pz*pz)^0.5", "deltaE", ";M [GeV];deltaE [GeV];", 50, 1.3, 1.9, 50, -0.9, 0.4, "M_deltaE_before_cut.png");

    loader.Cut("(0.5 < L1PSNM__boffy__bc) || (0.5 < L1PSNM__bofyo__bc) || (0.5 < L1PSNM__bostt__bc) || (0.5 < L1PSNM__bohie__bc) || (0.5 < L1PSNM__bolml6__bc) || (0.5 < L1PSNM__bolml7__bc) || (0.5 < L1PSNM__bolml8__bc) || (0.5 < L1PSNM__bolml9__bc) || (0.5 < L1PSNM__bolml10__bc) || (0.5 < L1PSNM__bolml12__bc)");
    loader.PrintInformation("========== trigger ==========");

    //loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_PrimarypionID_selection").c_str(), "", "");
    loader.Cut("0.1 < first_muon_pionID");
    loader.PrintInformation("========== 0.1 < pionID for leading muon ==========");

    //loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_SecondarypionID_selection").c_str(), "", "");
    loader.Cut("0.1 < second_muon_pionID");
    loader.PrintInformation("========== 0.1 < pionID for secondary muon ==========");

    //loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_ThirdpionID_selection").c_str(), "", "");
    loader.Cut("0.1 < third_muon_pionID");
    loader.PrintInformation("========== 0.1 < pionID for third muon ==========");

    //loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_SecondarymuonP_selection").c_str(), "", "");
    loader.Cut("0.3 < second_muon_p");
    loader.PrintInformation("========== 0.3 < muon momentum for secondary muon ==========");

    //loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_theta_miss_cut").c_str(), "", "");
    //loader.DrawTH1D("missingMomentumOfEventCMS_theta", "theta_miss", "theta_miss.png");
    loader.Cut("(0.3 < missingMomentumOfEventCMS_theta) && (missingMomentumOfEventCMS_theta < 2.7)");
    loader.PrintInformation("========== 0.3 < theta_miss < 2.7 ==========");

    //loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_thrust_cut").c_str(), "", "");
    //loader.DrawTH1D("thrust", "thrust", "thrust.png");
    loader.Cut("(0.86 < thrust) && (thrust < 0.97)");
    loader.PrintInformation("========== 0.86 < thrust < 0.97 ==========");

    //loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_Eecl_cut").c_str(), "", "");
    loader.Cut("roeEextra__bocleanMask__bc < 5.0");
    loader.PrintInformation("========== Eecl < 5.0 GeV ==========");

    //loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_diffthrust_cut").c_str(), "", "");
    loader.Cut("diff_cosToThrustOfEvent_CM < 1.6");
    loader.PrintInformation("========== Maxdiff(cosToThrustOfEvent from daughter) < 1.6 rad ==========");

    //loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_avgthrust_cut").c_str(), "", "");
    loader.Cut("(avg_cosToThrustOfEvent_CM < -0.5) || (avg_cosToThrustOfEvent_CM > 0.5)");
    loader.PrintInformation("========== |avg(cosToThrustOfEvent from daughter)| > 0.5 rad ==========");

    //loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_missingEnergy_cut").c_str(), "", "");
    loader.Cut("missingEnergyOfEventCMS > 0.0");
    loader.PrintInformation("========== missing Energy CMS > 0.0 GeV ==========");

    //loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_flighttime_cut").c_str(), "", "");
    loader.Cut("extraInfo__boALP_flightTime__bc > 0.0");
    loader.PrintInformation("========== flighttime of KS0 > 0 ==========");

    //loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_significance_distance_cut").c_str(), "", "");
    loader.Cut("extraInfo__boALP_significanceOfDistance__bc > 10.0");
    loader.PrintInformation("========== significance of distance of KS0 > 10 ==========");

    //loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_KS0_M_cut").c_str(), "", "");
    loader.Cut("(extraInfo__boALP_M__bc > 0.487611) && (extraInfo__boALP_M__bc < 0.507611)");
    loader.PrintInformation("========== deltaM of KS0 < 0.01 GeV ==========");

    //loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_strict_M_deltaE_selection").c_str(), "", "");
    loader.Cut("(M > 0.8) && (M < 1.0) && (deltaE > -0.3)");
    loader.PrintInformation("========== (0.8 < M < 1.0) and (deltaE > -0.3) ==========");

    loader.Cut("missingEnergyOfEventCMS > 2.0");
    loader.PrintInformation("========== missing Energy CMS > 2.0 GeV ==========");

    loader.Cut("roeDeltae__bocleanMask__bc < -1.5");
    loader.PrintInformation("========== roe Delta E < -1.5 GeV ==========");

    //loader.PrintSeparateRootFile((std::string(argv[3]) + "/before_leptonic_tag").c_str(), "", "");
    loader.Cut("(extraInfo__bonROE_RemainingTracks_cleanMask__bc < 1.5) && (((extraInfo__bon_vpho_muID9__bc > 0.5) && (extraInfo__bon_vpho_muID9__bc < 1.5)) || ((extraInfo__bon_vpho_eID9__bc > 0.5) && (extraInfo__bon_vpho_eID9__bc < 1.5)))");
    loader.PrintInformation("========== leptonic tag ==========");

    loader.RandomBCS();
    loader.IsBCSValid();
    loader.PrintInformation("========== Random BCS ==========");

    //loader.PrintSeparateRootFile((std::string(argv[3]) + "/final_output").c_str(), "", "");

    loader.end();

    return 0;
}
