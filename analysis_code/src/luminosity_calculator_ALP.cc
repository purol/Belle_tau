#include <stdio.h>
#include <string>
#include <vector>
#include <deque>
#include <cmath>
#include <cstdlib>
#include <random>
#include <format>

#include "TH1D.h"
#include "TH2D.h"

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

double mapping_function(std::vector<double> variables_) {
    double M = variables_.at(0);
    double deltaE = variables_.at(1);

    if (((M_peak_g - 5.0 * M_left_sigma_g) < M) && (M <= (M_peak_g + 5.0 * M_right_sigma_g)) && ((deltaE_peak_g - 5 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g + 5 * deltaE_right_sigma_g))) return 1.0;
    else if (((M_peak_g - 5.0 * M_left_sigma_g) < M) && (M <= (M_peak_g + 5.0 * M_right_sigma_g)) && ((deltaE_peak_g - 15 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g - 5 * deltaE_left_sigma_g))) return 2.0;
    else return NAN;

}

void FillHistogram(const char* input_path_1_, const char* input_path_2_, TH1D* data_th1d_, TH1D* signal_MC_th1d_, TH1D* bkg_MC_th1d_, TH1D* data_th1d_stat_err_, TH1D* signal_MC_th1d_stat_err_, TH1D* bkg_MC_th1d_stat_err_, std::vector<std::string> data_list_, std::vector<std::string> signal_list_, std::vector<std::string> background_list_) {
    std::string cut_BDT_1 = "(" + std::to_string(BDT_cut_1) + " < " + BDT_output_1_name + ")";
    std::string cut_M_1 = "((" + std::to_string(M_peak_g - 20 * M_left_sigma_g) + " < M) && (M < " + std::to_string(M_peak_g + 20 * M_right_sigma_g) + "))";
    std::string cut_deltaE_1 = "((" + std::to_string(deltaE_peak_g - 5 * deltaE_left_sigma_g) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak_g + 6 * deltaE_right_sigma_g) + "))";
    std::string cut_M_deltaE_1 = "(" + cut_M_1 + "&&" + cut_deltaE_1 + ")";
    std::string cut_total_1 = "(" + cut_M_deltaE_1 + "&&" + cut_BDT_1 + ")";

    std::string cut_BDT_2 = "(" + std::to_string(BDT_cut_2) + " < " + BDT_output_2_name + ")";
    std::string cut_M_2 = "((" + std::to_string(M_peak_g - 20 * M_left_sigma_g) + " < M) && (M < " + std::to_string(M_peak_g + 20 * M_right_sigma_g) + "))";
    std::string cut_deltaE_2 = "((" + std::to_string(deltaE_peak_g - 15 * deltaE_left_sigma_g) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak_g - 5 * deltaE_left_sigma_g) + "))";
    std::string cut_M_deltaE_2 = "(" + cut_M_2 + "&&" + cut_deltaE_2 + ")";
    std::string cut_total_2 = "(" + cut_M_deltaE_2 + "&&" + cut_BDT_2 + ")";

    std::string cut_region = cut_M_deltaE_1 + "||" + cut_M_deltaE_2;
    std::string cut_total = cut_total_1 + "||" + cut_total_2;

    std::string cut_m_alpha = "(" + std::to_string(mass - M_left_cut_value) + "< extraInfo__boALP_M__bc) && (extraInfo__boALP_M__bc <" + std::to_string(mass + M_right_cut_value) + ")";

    // data
    Loader loader_data("tau_lfv");
    for (int i = 0; i < data_list_.size(); i++) loader_data.Load((input_path_1_ + std::string("/") + data_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), "root", data_list_.at(i).c_str());
    loader_data.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_05", { {"charge", "first_muon_charge"}, {"momentum", "first_muon_p"}, {"theta", "first_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_05", { {"charge", "second_muon_charge"}, {"momentum", "second_muon_p"}, {"theta", "second_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("double_weight"); /* After box open, it should be removed! */
    loader_data.AddWeight("KS0_tracking", { {"theta", "extraInfo__boALP_theta__bc"}, {"momentum", "p_ALP"}, {"distance", "extraInfo__boALP_distance__bc"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("luminosity_scale", { {"MyEnergyType", "MyEnergyType"} }); /* After box open, it should be removed! */
    loader_data.Cut(cut_region.c_str());
    loader_data.Cut(cut_m_alpha.c_str());
    loader_data.RandomBCS();
    loader_data.IsBCSValid();
    loader_data.Cut(cut_total.c_str());
    loader_data.FillCustomizedTH1D(data_th1d_, { "M", "deltaE" }, { mapping_function });
    loader_data.end();

    // signal MC
    Loader loader_signal("tau_lfv");
    for (int i = 0; i < signal_list_.size(); i++) loader_signal.Load((input_path_1_ + std::string("/") + signal_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), ("alpha_mass" + std::format("{:g}", mass) + "_life" + std::format("{:g}", life) + "_A" + std::to_string(A) + "_B" + std::to_string(B) + "_").c_str(), signal_list_.at(i).c_str());
    loader_signal.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"} });
    loader_signal.AddWeight("muonID_05", { {"charge", "first_muon_charge"}, {"momentum", "first_muon_p"}, {"theta", "first_muon_theta"} });
    loader_signal.AddWeight("muonID_05", { {"charge", "second_muon_charge"}, {"momentum", "second_muon_p"}, {"theta", "second_muon_theta"} });
    loader_signal.AddWeight("double_weight");
    loader_signal.AddWeight("KS0_tracking", { {"theta", "extraInfo__boALP_theta__bc"}, {"momentum", "p_ALP"}, {"distance", "extraInfo__boALP_distance__bc"} });
    loader_signal.AddWeight("luminosity_scale", { {"MyEnergyType", "MyEnergyType"} });
    loader_signal.Cut(cut_region.c_str());
    loader_signal.Cut(cut_m_alpha.c_str());
    loader_signal.RandomBCS();
    loader_signal.IsBCSValid();
    loader_signal.Cut(cut_total.c_str());
    loader_signal.FillCustomizedTH1D(signal_MC_th1d_, { "M", "deltaE" }, { mapping_function });
    loader_signal.end();

    // background MC
    Loader loader_bkg("tau_lfv");
    for (int i = 0; i < background_list_.size(); i++) loader_bkg.Load((input_path_1_ + std::string("/") + background_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), "root", background_list_.at(i).c_str());
    loader_bkg.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"} });
    loader_bkg.AddWeight("muonID_05", { {"charge", "first_muon_charge"}, {"momentum", "first_muon_p"}, {"theta", "first_muon_theta"} });
    loader_bkg.AddWeight("muonID_05", { {"charge", "second_muon_charge"}, {"momentum", "second_muon_p"}, {"theta", "second_muon_theta"} });
    loader_bkg.AddWeight("double_weight");
    loader_bkg.AddWeight("KS0_tracking", { {"theta", "extraInfo__boALP_theta__bc"}, {"momentum", "p_ALP"}, {"distance", "extraInfo__boALP_distance__bc"} });
    loader_bkg.AddWeight("luminosity_scale", { {"MyEnergyType", "MyEnergyType"} });
    loader_bkg.Cut(cut_region.c_str());
    loader_bkg.Cut(cut_m_alpha.c_str());
    loader_bkg.RandomBCS();
    loader_bkg.IsBCSValid();
    loader_bkg.Cut(cut_total.c_str());
    loader_bkg.FillCustomizedTH1D(bkg_MC_th1d_, { "M", "deltaE" }, { mapping_function });
    loader_bkg.end();


    // get statistical uncertainty
    data_th1d_stat_err_->SetBinContent(1, data_th1d_->GetBinError(1));
    data_th1d_stat_err_->SetBinContent(2, data_th1d_->GetBinError(2));
    signal_MC_th1d_stat_err_->SetBinContent(1, signal_MC_th1d_->GetBinError(1));
    signal_MC_th1d_stat_err_->SetBinContent(2, signal_MC_th1d_->GetBinError(2));
    bkg_MC_th1d_stat_err_->SetBinContent(1, bkg_MC_th1d_->GetBinError(1));
    bkg_MC_th1d_stat_err_->SetBinContent(2, bkg_MC_th1d_->GetBinError(2));


    // We do not open the box, So data_th1d is MC. We use the proper uncertainty
    data_th1d_->SetBinError(1, std::sqrt(data_th1d_->GetBinContent(1)));
    data_th1d_->SetBinError(2, std::sqrt(data_th1d_->GetBinContent(2)));
}

int main(int argc, char* argv[]) {
    /*
    * argv[1]: input path 1
    * argv[2]: input path 2
    * argv[3]: output path
    * argv[4]: NToys
    * argv[5]: indicator
    * argv[6]: signal list (separated by colon)
    * argv[7]: background list (separated by colon)
    * argv[8]: mass
    * argv[9]: lifetime
    * argv[10]: A constant
    * argv[11]: B constant
    */

    // TH1 list
    /*
    *
    *   deltaE
    *      ^
    *   +5 +-----+-------+-----+
    *      |     |       |     |
    *      |     |   1   |     |
    *   -5 +-----+-------+-----+
    *      |     |       |     |
    *      |     |       |     |
    *      |     |   2   |     |
    *  -15 +-----+-------+-----+---> M
    *     -20   -5      +5    +20
    */
    TH1D* data_th1d = new TH1D("data_th1d", ";bin index;", 2, 0.5, 2.5);
    TH1D* signal_MC_th1d = new TH1D("signal_MC_th1d", ";bin index;", 2, 0.5, 2.5);
    TH1D* bkg_MC_th1d = new TH1D("bkg_MC_th1d", ";bin index;", 2, 0.5, 2.5);

    TH1D* data_th1d_stat_err = new TH1D("data_th1d_stat_err", ";bin index;", 2, 0.5, 2.5);
    TH1D* signal_MC_th1d_stat_err = new TH1D("signal_MC_th1d_stat_err", ";bin index;", 2, 0.5, 2.5);
    TH1D* bkg_MC_th1d_stat_err = new TH1D("bkg_MC_th1d_stat_err", ";bin index;", 2, 0.5, 2.5);

    mass = std::stod(argv[8]);
    life = std::stod(argv[9]);
    A = std::stoi(argv[10]);
    B = std::stoi(argv[11]);

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

    std::vector<std::string> signal_list = split(argv[6], ':');
    std::vector<std::string> background_list = split(argv[7], ':');

    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution((std::string(argv[1]) + "/alpha_mass" + std::format("{:g}", mass) + "_life" + std::format("{:g}", life) + "_A" + std::to_string(A) + "_B" + std::to_string(B) + "_M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    deltaE_peak_g = deltaE_peak;
    deltaE_left_sigma_g = deltaE_left_sigma;
    deltaE_right_sigma_g = deltaE_right_sigma;
    M_peak_g = M_peak;
    M_left_sigma_g = M_left_sigma;
    M_right_sigma_g = M_right_sigma;
    theta_g = theta;

    EventWeights::Register("MC_weight", MC_weight);
    EventWeights::Register("muonID_05", muonID_05);
    EventWeights::Register("double_weight", double_weight);
    EventWeights::Register("KS0_tracking", KS0_tracking);
    EventWeights::Register("luminosity_scale", luminosity_scale);

    // get nominal value
    std::vector<double> MC_th1d_nominal;
   
    // reset histograms
    data_th1d->Reset();
    signal_MC_th1d->Reset();
    bkg_MC_th1d->Reset();

    // we do not open the box, so I just use background MC
    FillHistogram(argv[1], argv[2], data_th1d, signal_MC_th1d, bkg_MC_th1d, data_th1d_stat_err, signal_MC_th1d_stat_err, bkg_MC_th1d_stat_err, background_list, signal_list, background_list);

    MC_th1d_nominal.push_back(signal_MC_th1d->GetBinContent(1));
    MC_th1d_nominal.push_back(signal_MC_th1d->GetBinContent(2));
    MC_th1d_nominal.push_back(bkg_MC_th1d->GetBinContent(1));
    MC_th1d_nominal.push_back(bkg_MC_th1d->GetBinContent(2));

    // print output
    FILE* fp;
    fp = fopen((std::string(argv[3]) + "/luminosity_toys_" + std::string(argv[5]) + ".csv").c_str(), "w");

    int NToys = atoi(argv[4]);
    for (int i = 0; i < NToys; i++) {
        // reset histograms
        data_th1d->Reset();
        signal_MC_th1d->Reset();
        bkg_MC_th1d->Reset();

        // fluctuate luminosity
        EventWeights::Fluctuate("luminosity_scale");

        // we do not open the box, so I just use background MC
        FillHistogram(argv[1], argv[2], data_th1d, signal_MC_th1d, bkg_MC_th1d, data_th1d_stat_err, signal_MC_th1d_stat_err, bkg_MC_th1d_stat_err, background_list, signal_list, background_list);

        if (MC_th1d_nominal.at(0) != 0) fprintf(fp, "%lf,", signal_MC_th1d->GetBinContent(1) / MC_th1d_nominal.at(0));
        else fprintf(fp, "1.0,");

        if (MC_th1d_nominal.at(1) != 0) fprintf(fp, "%lf,", signal_MC_th1d->GetBinContent(2) / MC_th1d_nominal.at(1));
        else fprintf(fp, "1.0,");

        if (MC_th1d_nominal.at(2) != 0) fprintf(fp, "%lf,", bkg_MC_th1d->GetBinContent(1) / MC_th1d_nominal.at(2));
        else fprintf(fp, "1.0,");

        if (MC_th1d_nominal.at(3) != 0) fprintf(fp, "%lf\n", bkg_MC_th1d->GetBinContent(2) / MC_th1d_nominal.at(3));
        else fprintf(fp, "1.0\n");

    }

    fclose(fp);

    return 0;
}
