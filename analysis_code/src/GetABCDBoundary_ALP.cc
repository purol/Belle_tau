#include <stdio.h>
#include <string>
#include <vector>
#include <deque>
#include <cmath>
#include <cstdlib>
#include <format>

#include "TH1D.h"
#include "RooRealVar.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "functions.h"
#include "MyModule.h"
#include "data.h"

double mass;
double life;
int A;
int B;

double M_left_cut_value;
double M_right_cut_value;

double BDT_cut_1;
double BDT_cut_2;

std::string BDT_output_1_name;
std::string BDT_output_2_name;

double deltaE_peak_g;
double deltaE_left_sigma_g;
double deltaE_right_sigma_g;
double M_peak_g;
double M_left_sigma_g;
double M_right_sigma_g;
double theta_g;

void GetABCDBoundary(const char* input_path_1_, const char* input_path_2_, const char* filename_, std::vector<std::string> background_list_) {
    std::string cut_region = GetBCSCut(deltaE_peak_g, deltaE_left_sigma_g, deltaE_right_sigma_g, M_peak_g, M_left_sigma_g, M_right_sigma_g);

    std::string cut_m_alpha = "(" + std::to_string(mass - M_left_cut_value) + "< extraInfo__boALP_M__bc) && (extraInfo__boALP_M__bc <" + std::to_string(mass + M_right_cut_value) + ")";

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
    loader_bkg.AddWeight("muonID_05_ALP", { {"index", "first_muon_index"},  {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
    loader_bkg.AddWeight("muonID_05_ALP", { {"index", "second_muon_index"},  {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
    loader_bkg.AddWeight("muonID_01_ALP", { {"index", "third_muon_index"},  {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
    loader_bkg.AddWeight("KS0_tracking_run1", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
    loader_bkg.AddWeight("KS0_tracking_run2", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
    loader_bkg.Cut(cut_region.c_str());
    loader_bkg.Cut(cut_m_alpha.c_str());
    loader_bkg.RandomBCS();
    loader_bkg.IsBCSValid();
    // The boundary is determined in the nominal mass range after choosing the candidate.
    loader_bkg.Cut(("(" + std::to_string(M_peak_g - 20 * M_left_sigma_g) + "< M) && (M < " + std::to_string(M_peak_g + 20 * M_right_sigma_g) + ")").c_str());
    loader_bkg.FillDataSet(&background_MC, { &sideband_M, &sideband_deltaE, &sideband_BDT_1, &sideband_BDT_2 }, { "M", "deltaE", BDT_output_1_name.c_str(), BDT_output_2_name.c_str() });
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
    * argv[4]: mass
    * argv[5]: lifetime
    * argv[6]: A constant
    * argv[7]: B constant
    */

    if (argc != 8) {
        printf("Usage: %s input_path_1 input_path_2 background_list mass lifetime A B\n", argv[0]);
        return 1;
    }

    mass = std::stod(argv[4]);
    life = std::stod(argv[5]);
    A = std::stoi(argv[6]);
    B = std::stoi(argv[7]);

    M_left_cut_value = 0;
    M_right_cut_value = 0;
    if ((0 < life) && (life < 0.7)) {
        M_left_cut_value = 0.025;
        M_right_cut_value = 0.025;
    }
    else if ((0.7 <= life) && (life < 7)) {
        M_left_cut_value = 0.03;
        M_right_cut_value = 0.03;
    }
    else if ((7 <= life) && (life < 70)) {
        M_left_cut_value = 0.035;
        M_right_cut_value = 0.035;

    }
    else if (70 <= life) {
        M_left_cut_value = 0.075;
        M_right_cut_value = 0.075;

    }

    ReadFOM((std::string(argv[1]) + "/GridSearch_one/FOM_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".log").c_str(), &BDT_cut_1);
    ReadFOM((std::string(argv[1]) + "/GridSearch_two/FOM_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".log").c_str(), &BDT_cut_2);

    std::string strMass = std::format("{:g}", mass);
    std::string strLife = std::format("{:g}", life);
    std::string strA;
    std::string strB;
    if (A >= 0) strA = std::to_string(A);
    else strA = "m" + std::to_string(std::abs(A));
    if (B >= 0) strB = std::to_string(B);
    else strB = "m" + std::to_string(std::abs(B));

    BDT_output_1_name = "BDT_output_1_" + strMass + "_" + strLife + "_" + strA + "_" + strB;
    BDT_output_2_name = "BDT_output_2_" + strMass + "_" + strLife + "_" + strA + "_" + strB;

    std::vector<std::string> background_list = split(argv[3], ':');

    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    EventWeights::Register("MC_weight", MC_weight);
    EventWeights::Register("muonID_01_ALP", muonID_01_ALP);
    EventWeights::Register("muonID_05_ALP", muonID_05_ALP);
    EventWeights::Register("KS0_tracking_run1", KS0_tracking_MC16rd_run1);
    EventWeights::Register("KS0_tracking_run2", KS0_tracking_MC16rd_run2);

    ReadResolution((std::string(argv[1]) + "/alpha_mass" + std::format("{:g}", mass) + "_life" + std::format("{:g}", life) + "_A" + std::to_string(A) + "_B" + std::to_string(B) + "_M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    deltaE_peak_g = deltaE_peak;
    deltaE_left_sigma_g = deltaE_left_sigma;
    deltaE_right_sigma_g = deltaE_right_sigma;
    M_peak_g = M_peak;
    M_left_sigma_g = M_left_sigma;
    M_right_sigma_g = M_right_sigma;
    theta_g = theta;

    // Save one set of boundaries for every workspace and systematic calculator.
    GetABCDBoundary(argv[1], argv[2], (std::string(argv[1]) + "/ABCD_boundary_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".txt").c_str(), background_list);

    return 0;
}
