#include <stdio.h>
#include <string>
#include <vector>
#include <deque>
#include <cmath>
#include <cstdlib>

#include "TH1D.h"
#include "RooRealVar.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "functions.h"
#include "MyModule.h"
#include "data.h"

double BDT_cut_1;
double BDT_cut_2;

double deltaE_peak_g;
double deltaE_left_sigma_g;
double deltaE_right_sigma_g;
double M_peak_g;
double M_left_sigma_g;
double M_right_sigma_g;
double theta_g;

void GetABCDBoundary(const char* input_path_1_, const char* input_path_2_, const char* filename_, std::vector<std::string> background_list_) {
    std::string cut_region = GetBCSCut(deltaE_peak_g, deltaE_left_sigma_g, deltaE_right_sigma_g, M_peak_g, M_left_sigma_g, M_right_sigma_g);

    RooRealVar sideband_M("M", "M", M_peak_g - 20 * M_left_sigma_g, M_peak_g + 20 * M_right_sigma_g);
    RooRealVar sideband_deltaE("deltaE", "deltaE", deltaE_peak_g - sizeDeltaE_left_BCS * deltaE_left_sigma_g, deltaE_peak_g + sizeDeltaE_right_BCS * deltaE_right_sigma_g);
    RooRealVar sideband_BDT_1("BDT_1", "BDT_1", 0.0, 1.0);
    RooRealVar sideband_BDT_2("BDT_2", "BDT_2", 0.0, 1.0);
    RooRealVar sideband_weight("weight", "weight", 1.0);
    RooDataSet background_MC("background_MC", "background_MC", RooArgSet(sideband_M, sideband_deltaE, sideband_BDT_1, sideband_BDT_2, sideband_weight), RooFit::WeightVar("weight"));

    // Boundaries always use nominal background MC, including after box open. Keep these weights.
    Loader loader_bkg("tau_lfv");
    for (int i = 0; i < background_list_.size(); i++) loader_bkg.Load((input_path_1_ + std::string("/") + background_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), "root", background_list_.at(i).c_str());
    loader_bkg.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_bkg.AddWeight("muonID_05_prompt", { {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
    loader_bkg.AddWeight("muonID_05_prompt", { {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
    loader_bkg.AddWeight("muonID_01_prompt", { {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
    loader_bkg.Cut(cut_region.c_str());
    loader_bkg.RandomBCS();
    loader_bkg.IsBCSValid();
    // The boundary is determined in the nominal mass range after choosing the candidate.
    loader_bkg.Cut(("(" + std::to_string(M_peak_g - 20 * M_left_sigma_g) + "< M) && (M < " + std::to_string(M_peak_g + 20 * M_right_sigma_g) + ")").c_str());
    loader_bkg.FillDataSet(&background_MC, { &sideband_M, &sideband_deltaE, &sideband_BDT_1, &sideband_BDT_2 }, { "M", "deltaE", "BDT_output_1", "BDT_output_2" });
    loader_bkg.end();
    ABCDParameters parameters = { BDT_cut_1, BDT_cut_2, deltaE_peak_g, deltaE_left_sigma_g, deltaE_right_sigma_g, M_peak_g, M_left_sigma_g, M_right_sigma_g, sizeM };
    std::vector<double> sideband_BDT_cuts = GetABCDBoundary(background_MC, parameters);
    parameters.sideband_BDT_cut_1 = sideband_BDT_cuts.at(0);
    parameters.sideband_BDT_cut_2 = sideband_BDT_cuts.at(1);
    std::vector<double> validation_BDT_cuts = GetValidationBoundary(background_MC, parameters);

    WriteABCDBoundary(filename_, { BDT_cut_1, BDT_cut_2 }, sideband_BDT_cuts, validation_BDT_cuts);
}

int main(int argc, char* argv[]) {
    /*
    * argv[1]: input path 1
    * argv[2]: input path 2 for background MC
    * argv[3]: background list (separated by colon)
    */

    if (argc != 4) {
        printf("Usage: %s input_path_1 input_path_2 background_list\n", argv[0]);
        return 1;
    }

    ReadFOM((std::string(argv[1]) + "/GridSearch_one/FOM.log").c_str(), &BDT_cut_1);
    ReadFOM((std::string(argv[1]) + "/GridSearch_two/FOM.log").c_str(), &BDT_cut_2);

    std::vector<std::string> background_list = split(argv[3], ':');

    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution((std::string(argv[1]) + "/M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    deltaE_peak_g = deltaE_peak;
    deltaE_left_sigma_g = deltaE_left_sigma;
    deltaE_right_sigma_g = deltaE_right_sigma;
    M_peak_g = M_peak;
    M_left_sigma_g = M_left_sigma;
    M_right_sigma_g = M_right_sigma;
    theta_g = theta;

    EventWeights::Register("MC_weight", MC_weight);
    EventWeights::Register("muonID_01_prompt", muonID_01_prompt);
    EventWeights::Register("muonID_05_prompt", muonID_05_prompt);

    // Save one set of boundaries for every workspace and systematic calculator.
    GetABCDBoundary(argv[1], argv[2], (std::string(argv[1]) + "/ABCD_boundary.txt").c_str(), background_list);

    return 0;
}
