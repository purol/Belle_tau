#include <stdio.h>
#include <string>
#include <vector>
#include <deque>
#include <cmath>
#include <cstdlib>
#include <random>

#include "TH1D.h"
#include "TH2D.h"

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

bool include_validation = false;

double mapping_function(std::vector<double> variables_) {
    double bin = mapping_function_ABCD(variables_, false);
    if (std::isfinite(bin) || !include_validation) return bin;
    return 8.0 + mapping_function_ABCD(variables_, true);
}

void FillHistogram(const char* input_path_1_, const char* input_path_2_, TH1D* data_th1d_, TH1D* signal_MC_th1d_, TH1D* bkg_MC_th1d_, TH1D* data_th1d_stat_err_, TH1D* signal_MC_th1d_stat_err_, TH1D* bkg_MC_th1d_stat_err_, std::vector<std::string> data_list_, std::vector<std::string> signal_list_, std::vector<std::string> background_list_) {
    std::string cut_M_1 = "((" + std::to_string(M_peak_g - 20 * M_left_sigma_g) + " < M) && (M < " + std::to_string(M_peak_g + 20 * M_right_sigma_g) + "))";
    std::string cut_deltaE_1 = "((" + std::to_string(deltaE_peak_g - 5 * deltaE_left_sigma_g) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak_g + 6 * deltaE_right_sigma_g) + "))";
    std::string cut_M_deltaE_1 = "(" + cut_M_1 + "&&" + cut_deltaE_1 + ")";

    std::string cut_M_2 = "((" + std::to_string(M_peak_g - 20 * M_left_sigma_g) + " < M) && (M < " + std::to_string(M_peak_g + 20 * M_right_sigma_g) + "))";
    std::string cut_deltaE_2 = "((" + std::to_string(deltaE_peak_g - 16 * deltaE_left_sigma_g) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak_g - 5 * deltaE_left_sigma_g) + "))";
    std::string cut_M_deltaE_2 = "(" + cut_M_2 + "&&" + cut_deltaE_2 + ")";

    std::string cut_region = cut_M_deltaE_1 + "||" + cut_M_deltaE_2;
    
    // data
    Loader loader_data("tau_lfv");
    for (int i = 0; i < data_list_.size(); i++) loader_data.Load((input_path_1_ + std::string("/") + data_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), "root", data_list_.at(i).c_str());
    loader_data.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_05_prompt", { {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_05_prompt", { {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_01_prompt", { {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("luminosity_scale", { {"MyEnergyType", "MyEnergyType"} }); /* After box open, it should be removed! */
    loader_data.Cut(cut_region.c_str());
    loader_data.RandomBCS();
    loader_data.IsBCSValid();
    loader_data.FillCustomizedTH1D(data_th1d_, { "M", "deltaE", "BDT_output_1", "BDT_output_2" }, { mapping_function });
    loader_data.end();

    // signal MC
    Loader loader_signal("tau_lfv");
    for (int i = 0; i < signal_list_.size(); i++) loader_signal.Load((input_path_1_ + std::string("/") + signal_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), "root", signal_list_.at(i).c_str());
    loader_signal.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_signal.AddWeight("muonID_05_prompt", { {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
    loader_signal.AddWeight("muonID_05_prompt", { {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
    loader_signal.AddWeight("muonID_01_prompt", { {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
    loader_signal.AddWeight("luminosity_scale", { {"MyEnergyType", "MyEnergyType"} });
    loader_signal.Cut(cut_region.c_str());
    loader_signal.RandomBCS();
    loader_signal.IsBCSValid();
    loader_signal.FillCustomizedTH1D(signal_MC_th1d_, { "M", "deltaE", "BDT_output_1", "BDT_output_2" }, { mapping_function });
    loader_signal.end();

    // background MC
    Loader loader_bkg("tau_lfv");
    for (int i = 0; i < background_list_.size(); i++) loader_bkg.Load((input_path_1_ + std::string("/") + background_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), "root", background_list_.at(i).c_str());
    loader_bkg.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_bkg.AddWeight("muonID_05_prompt", { {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
    loader_bkg.AddWeight("muonID_05_prompt", { {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
    loader_bkg.AddWeight("muonID_01_prompt", { {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
    loader_bkg.AddWeight("luminosity_scale", { {"MyEnergyType", "MyEnergyType"} });
    loader_bkg.Cut(cut_region.c_str());
    loader_bkg.RandomBCS();
    loader_bkg.IsBCSValid();
    loader_bkg.FillCustomizedTH1D(bkg_MC_th1d_, { "M", "deltaE", "BDT_output_1", "BDT_output_2" }, { mapping_function });
    loader_bkg.end();


    // get statistical uncertainty
    for (int i = 1; i <= signal_MC_th1d_->GetNbinsX(); i++) {
        data_th1d_stat_err_->SetBinContent(i, data_th1d_->GetBinError(i));
        signal_MC_th1d_stat_err_->SetBinContent(i, signal_MC_th1d_->GetBinError(i));
        bkg_MC_th1d_stat_err_->SetBinContent(i, bkg_MC_th1d_->GetBinError(i));
        // We do not open the box, So data_th1d is MC. We use the proper uncertainty
        data_th1d_->SetBinError(i, std::sqrt(data_th1d_->GetBinContent(i))); /* After box open, it should be removed! */
    }

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
    */

    // Optional last argument: validation. Use joint toys only when enabling the validation likelihood.
    include_validation = argc > 8 && std::string(argv[8]) == "validation";
    int NBin = include_validation ? 16 : 8;
    // A1, B1, C1, D1, A2, B2, C2, D2; optional validation bins follow in the same order.
    TH1D* data_th1d = new TH1D("data_th1d", ";bin index;", NBin, 0.5, NBin + 0.5);
    TH1D* signal_MC_th1d = new TH1D("signal_MC_th1d", ";bin index;", NBin, 0.5, NBin + 0.5);
    TH1D* bkg_MC_th1d = new TH1D("bkg_MC_th1d", ";bin index;", NBin, 0.5, NBin + 0.5);

    TH1D* data_th1d_stat_err = new TH1D("data_th1d_stat_err", ";bin index;", NBin, 0.5, NBin + 0.5);
    TH1D* signal_MC_th1d_stat_err = new TH1D("signal_MC_th1d_stat_err", ";bin index;", NBin, 0.5, NBin + 0.5);
    TH1D* bkg_MC_th1d_stat_err = new TH1D("bkg_MC_th1d_stat_err", ";bin index;", NBin, 0.5, NBin + 0.5);

    ReadFOM((std::string(argv[1]) + "/GridSearch_one/FOM.log").c_str(), &BDT_cut_1);
    ReadFOM((std::string(argv[1]) + "/GridSearch_two/FOM.log").c_str(), &BDT_cut_2);

    std::vector<std::string> signal_list = split(argv[6], ':');
    std::vector<std::string> background_list = split(argv[7], ':');

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
    EventWeights::Register("luminosity_scale", luminosity_scale);

    // get nominal value
    std::vector<double> MC_th1d_nominal;
   
    // reset histograms
    data_th1d->Reset();
    signal_MC_th1d->Reset();
    bkg_MC_th1d->Reset();

    // we do not open the box, so I just use background MC
    FillHistogram(argv[1], argv[2], data_th1d, signal_MC_th1d, bkg_MC_th1d, data_th1d_stat_err, signal_MC_th1d_stat_err, bkg_MC_th1d_stat_err, background_list, signal_list, background_list);

    for (int i = 1; i <= NBin; i++) MC_th1d_nominal.push_back(signal_MC_th1d->GetBinContent(i));
    for (int i = 1; i <= NBin; i++) MC_th1d_nominal.push_back(bkg_MC_th1d->GetBinContent(i));

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

        // Signal columns first, then background columns, so PCA_toys.py --half_only selects the signal.
        for (int j = 1; j <= NBin; j++) {
            double nominal = MC_th1d_nominal.at(j - 1);
            fprintf(fp, "%lf,", nominal != 0.0 ? signal_MC_th1d->GetBinContent(j) / nominal : 1.0);
        }
        for (int j = 1; j <= NBin; j++) {
            double nominal = MC_th1d_nominal.at(NBin + j - 1);
            fprintf(fp, "%lf%s", nominal != 0.0 ? bkg_MC_th1d->GetBinContent(j) / nominal : 1.0, j == NBin ? "\n" : ",");
        }

    }

    fclose(fp);

    return 0;
}
