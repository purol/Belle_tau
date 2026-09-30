#include <stdio.h>
#include <string>
#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>

#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "RooWorkspace.h"
#include "RooRealVar.h"

#include "RooStats/HistFactory/Measurement.h"
#include "RooStats/HistFactory/Channel.h"
#include "RooStats/HistFactory/Sample.h"
#include "RooStats/HistFactory/MakeModelAndMeasurementsFast.h"

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

double mapping_function(std::vector<double> variables_) {
    double M = variables_.at(0);
    double deltaE = variables_.at(1);

    if (((M_peak_g - sizeM * M_left_sigma_g) < M) && (M <= (M_peak_g + sizeM * M_right_sigma_g)) && ((deltaE_peak_g - 5 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g + 5 * deltaE_right_sigma_g))) return 1.0;
    else if (((M_peak_g - sizeM * M_left_sigma_g) < M) && (M <= (M_peak_g + sizeM * M_right_sigma_g)) && ((deltaE_peak_g - 15 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g - 5 * deltaE_left_sigma_g))) return 2.0;
    else return NAN;

}

double mapping_function_plus_M(std::vector<double> variables_) {
    double M = variables_.at(0);
    double deltaE = variables_.at(1);

    if (((M_peak_g - (sizeM - 1.0) * M_left_sigma_g) < M) && (M <= (M_peak_g + (sizeM + 1.0) * M_right_sigma_g)) && ((deltaE_peak_g - 5 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g + 5 * deltaE_right_sigma_g))) return 1.0;
    else if (((M_peak_g - (sizeM - 1.0) * M_left_sigma_g) < M) && (M <= (M_peak_g + (sizeM + 1.0) * M_right_sigma_g)) && ((deltaE_peak_g - 15 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g - 5 * deltaE_left_sigma_g))) return 2.0;
    else return NAN;

}

double mapping_function_minus_M(std::vector<double> variables_) {
    double M = variables_.at(0);
    double deltaE = variables_.at(1);

    if (((M_peak_g - (sizeM + 1.0) * M_left_sigma_g) < M) && (M <= (M_peak_g + (sizeM - 1.0) * M_right_sigma_g)) && ((deltaE_peak_g - 5 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g + 5 * deltaE_right_sigma_g))) return 1.0;
    else if (((M_peak_g - (sizeM + 1.0) * M_left_sigma_g) < M) && (M <= (M_peak_g + (sizeM - 1.0) * M_right_sigma_g)) && ((deltaE_peak_g - 15 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g - 5 * deltaE_left_sigma_g))) return 2.0;
    else return NAN;

}

double mapping_function_plus_DeltaE(std::vector<double> variables_) {
    double M = variables_.at(0);
    double deltaE = variables_.at(1);

    if (((M_peak_g - sizeM * M_left_sigma_g) < M) && (M <= (M_peak_g + sizeM * M_right_sigma_g)) && ((deltaE_peak_g - 4 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g + 6 * deltaE_right_sigma_g))) return 1.0;
    else if (((M_peak_g - sizeM * M_left_sigma_g) < M) && (M <= (M_peak_g + sizeM * M_right_sigma_g)) && ((deltaE_peak_g - 14 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g - 4 * deltaE_left_sigma_g))) return 2.0;
    else return NAN;

}

double mapping_function_minus_DeltaE(std::vector<double> variables_) {
    double M = variables_.at(0);
    double deltaE = variables_.at(1);

    if (((M_peak_g - sizeM * M_left_sigma_g) < M) && (M <= (M_peak_g + sizeM * M_right_sigma_g)) && ((deltaE_peak_g - 6 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g + 4 * deltaE_right_sigma_g))) return 1.0;
    else if (((M_peak_g - sizeM * M_left_sigma_g) < M) && (M <= (M_peak_g + sizeM * M_right_sigma_g)) && ((deltaE_peak_g - 16 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g - 6 * deltaE_left_sigma_g))) return 2.0;
    else return NAN;

}

void FillHistogram(const char* input_path_1_, const char* input_path_2_, TH1D* data_th1d_, TH1D* signal_MC_th1d_, TH1D* bkg_MC_th1d_, TH1D* data_th1d_stat_err_, TH1D* signal_MC_th1d_stat_err_, TH1D* bkg_MC_th1d_stat_err_, std::vector<std::string> data_list_, std::vector<std::string> signal_list_, std::vector<std::string> background_list_) {
    std::string cut_BDT_1 = "(" + std::to_string(BDT_cut_1) + " < BDT_output_1)";
    std::string cut_M_1 = "((" + std::to_string(M_peak_g - 20 * M_left_sigma_g) + " < M) && (M < " + std::to_string(M_peak_g + 20 * M_right_sigma_g) + "))";
    std::string cut_deltaE_1 = "((" + std::to_string(deltaE_peak_g - 5 * deltaE_left_sigma_g) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak_g + 6 * deltaE_right_sigma_g) + "))";
    std::string cut_M_deltaE_1 = "(" + cut_M_1 + "&&" + cut_deltaE_1 + ")";
    std::string cut_total_1 = "(" + cut_M_deltaE_1 + "&&" + cut_BDT_1 + ")";

    std::string cut_BDT_2 = "(" + std::to_string(BDT_cut_2) + " < BDT_output_2)";
    std::string cut_M_2 = "((" + std::to_string(M_peak_g - 20 * M_left_sigma_g) + " < M) && (M < " + std::to_string(M_peak_g + 20 * M_right_sigma_g) + "))";
    std::string cut_deltaE_2 = "((" + std::to_string(deltaE_peak_g - 16 * deltaE_left_sigma_g) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak_g - 5 * deltaE_left_sigma_g) + "))";
    std::string cut_M_deltaE_2 = "(" + cut_M_2 + "&&" + cut_deltaE_2 + ")";
    std::string cut_total_2 = "(" + cut_M_deltaE_2 + "&&" + cut_BDT_2 + ")";

    std::string cut_region = cut_M_deltaE_1 + "||" + cut_M_deltaE_2;
    std::string cut_total = cut_total_1 + "||" + cut_total_2;
    
    // data
    Loader loader_data("tau_lfv");
    for (int i = 0; i < data_list_.size(); i++) loader_data.Load((input_path_1_ + std::string("/") + data_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), "root", data_list_.at(i).c_str());
    loader_data.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_05_prompt", { {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_05_prompt", { {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_01_prompt", { {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.Cut(cut_region.c_str());
    loader_data.RandomBCS();
    loader_data.IsBCSValid();
    loader_data.Cut(cut_total.c_str());
    loader_data.FillCustomizedTH1D(data_th1d_, { "M", "deltaE" }, { mapping_function });
    loader_data.end();

    // signal MC
    Loader loader_signal("tau_lfv");
    for (int i = 0; i < signal_list_.size(); i++) loader_signal.Load((input_path_1_ + std::string("/") + signal_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), "root", signal_list_.at(i).c_str());
    loader_signal.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_signal.AddWeight("muonID_05_prompt", { {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
    loader_signal.AddWeight("muonID_05_prompt", { {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
    loader_signal.AddWeight("muonID_01_prompt", { {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
    loader_signal.Cut(cut_region.c_str());
    loader_signal.RandomBCS();
    loader_signal.IsBCSValid();
    loader_signal.Cut(cut_total.c_str());
    loader_signal.FillCustomizedTH1D(signal_MC_th1d_, { "M", "deltaE" }, { mapping_function });
    loader_signal.end();

    // background MC
    Loader loader_bkg("tau_lfv");
    for (int i = 0; i < background_list_.size(); i++) loader_bkg.Load((input_path_1_ + std::string("/") + background_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), "root", background_list_.at(i).c_str());
    loader_bkg.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_bkg.AddWeight("muonID_05_prompt", { {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
    loader_bkg.AddWeight("muonID_05_prompt", { {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
    loader_bkg.AddWeight("muonID_01_prompt", { {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
    loader_bkg.Cut(cut_region.c_str());
    loader_bkg.RandomBCS();
    loader_bkg.IsBCSValid();
    loader_bkg.Cut(cut_total.c_str());
    loader_bkg.FillCustomizedTH1D(bkg_MC_th1d_, { "M", "deltaE" }, { mapping_function });
    loader_bkg.end();


    // get statistical uncertainty (relative error)
    data_th1d_stat_err_->SetBinContent(1, data_th1d_->GetBinError(1) / data_th1d_->GetBinContent(1));
    data_th1d_stat_err_->SetBinContent(2, data_th1d_->GetBinError(2) / data_th1d_->GetBinContent(2));
    signal_MC_th1d_stat_err_->SetBinContent(1, signal_MC_th1d_->GetBinError(1) / signal_MC_th1d_->GetBinContent(1));
    signal_MC_th1d_stat_err_->SetBinContent(2, signal_MC_th1d_->GetBinError(2) / signal_MC_th1d_->GetBinContent(2));
    bkg_MC_th1d_stat_err_->SetBinContent(1, bkg_MC_th1d_->GetBinError(1) / bkg_MC_th1d_->GetBinContent(1));
    bkg_MC_th1d_stat_err_->SetBinContent(2, bkg_MC_th1d_->GetBinError(2) / bkg_MC_th1d_->GetBinContent(2));


    // We do not open the box, So data_th1d is MC. We use the proper uncertainty
    data_th1d_->SetBinError(1, std::sqrt(data_th1d_->GetBinContent(1)));
    data_th1d_->SetBinError(2, std::sqrt(data_th1d_->GetBinContent(2)));
}

void FillHistogram_fluc_SR(const char* input_path_1_, const char* input_path_2_, TH1D* data_th1d_, TH1D* signal_MC_th1d_, TH1D* bkg_MC_th1d_, std::vector<std::string> data_list_, std::vector<std::string> signal_list_, std::vector<std::string> background_list_, int fluc_mode) {
    /*
    * fluc mode:
    * 0: positive M fluctuation
    * 1: negative M fluctuation
    * 2: positive DeltaE fluctuation
    * 3: negative DeltaE fluctuation
    */
    // data

    std::string cut_BDT_1 = "(" + std::to_string(BDT_cut_1) + " < BDT_output_1)";
    std::string cut_M_1 = "((" + std::to_string(M_peak_g - 20 * M_left_sigma_g) + " < M) && (M < " + std::to_string(M_peak_g + 20 * M_right_sigma_g) + "))";
    std::string cut_deltaE_1 = "((" + std::to_string(deltaE_peak_g - 5 * deltaE_left_sigma_g) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak_g + 6 * deltaE_right_sigma_g) + "))";
    std::string cut_M_deltaE_1 = "(" + cut_M_1 + "&&" + cut_deltaE_1 + ")";
    std::string cut_total_1 = "(" + cut_M_deltaE_1 + "&&" + cut_BDT_1 + ")";

    std::string cut_BDT_2 = "(" + std::to_string(BDT_cut_2) + " < BDT_output_2)";
    std::string cut_M_2 = "((" + std::to_string(M_peak_g - 20 * M_left_sigma_g) + " < M) && (M < " + std::to_string(M_peak_g + 20 * M_right_sigma_g) + "))";
    std::string cut_deltaE_2 = "((" + std::to_string(deltaE_peak_g - 16 * deltaE_left_sigma_g) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak_g - 5 * deltaE_left_sigma_g) + "))";
    std::string cut_M_deltaE_2 = "(" + cut_M_2 + "&&" + cut_deltaE_2 + ")";
    std::string cut_total_2 = "(" + cut_M_deltaE_2 + "&&" + cut_BDT_2 + ")";

    std::string cut_region = cut_M_deltaE_1 + "||" + cut_M_deltaE_2;
    std::string cut_total = cut_total_1 + "||" + cut_total_2;

    Loader loader_data("tau_lfv");
    for (int i = 0; i < data_list_.size(); i++) loader_data.Load((input_path_1_ + std::string("/") + data_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), "root", data_list_.at(i).c_str());
    loader_data.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_05_prompt", { {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_05_prompt", { {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_01_prompt", { {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.Cut(cut_region.c_str());
    loader_data.RandomBCS();
    loader_data.IsBCSValid();
    loader_data.Cut(cut_total.c_str());
    if (fluc_mode == 0) loader_data.FillCustomizedTH1D(data_th1d_, { "M", "deltaE" }, { mapping_function_plus_M });
    else if (fluc_mode == 1) loader_data.FillCustomizedTH1D(data_th1d_, { "M", "deltaE" }, { mapping_function_minus_M });
    else if (fluc_mode == 2) loader_data.FillCustomizedTH1D(data_th1d_, { "M", "deltaE" }, { mapping_function_plus_DeltaE });
    else if (fluc_mode == 3) loader_data.FillCustomizedTH1D(data_th1d_, { "M", "deltaE" }, { mapping_function_minus_DeltaE });
    else {
        printf("[FillHistogram_fluc_SR] fluctuation index should be one of 0, 1, 2, or 3\n");
        exit(1);
    }
    loader_data.end();

    // signal MC
    Loader loader_signal("tau_lfv");
    for (int i = 0; i < signal_list_.size(); i++) loader_signal.Load((input_path_1_ + std::string("/") + signal_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), "root", signal_list_.at(i).c_str());
    loader_signal.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_signal.AddWeight("muonID_05_prompt", { {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
    loader_signal.AddWeight("muonID_05_prompt", { {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
    loader_signal.AddWeight("muonID_01_prompt", { {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
    loader_signal.Cut(cut_region.c_str());
    loader_signal.RandomBCS();
    loader_signal.IsBCSValid();
    loader_signal.Cut(cut_total.c_str());
    if (fluc_mode == 0) loader_signal.FillCustomizedTH1D(signal_MC_th1d_, { "M", "deltaE" }, { mapping_function_plus_M });
    else if (fluc_mode == 1) loader_signal.FillCustomizedTH1D(signal_MC_th1d_, { "M", "deltaE" }, { mapping_function_minus_M });
    else if (fluc_mode == 2) loader_signal.FillCustomizedTH1D(signal_MC_th1d_, { "M", "deltaE" }, { mapping_function_plus_DeltaE });
    else if (fluc_mode == 3) loader_signal.FillCustomizedTH1D(signal_MC_th1d_, { "M", "deltaE" }, { mapping_function_minus_DeltaE });
    else {
        printf("[FillHistogram_fluc_SR] fluctuation index should be one of 0, 1, 2, or 3\n");
        exit(1);
    }
    loader_signal.end();

    // background MC
    Loader loader_bkg("tau_lfv");
    for (int i = 0; i < background_list_.size(); i++) loader_bkg.Load((input_path_1_ + std::string("/") + background_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), "root", background_list_.at(i).c_str());
    loader_bkg.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_bkg.AddWeight("muonID_05_prompt", { {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
    loader_bkg.AddWeight("muonID_05_prompt", { {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
    loader_bkg.AddWeight("muonID_01_prompt", { {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
    loader_bkg.Cut(cut_region.c_str());
    loader_bkg.RandomBCS();
    loader_bkg.IsBCSValid();
    loader_bkg.Cut(cut_total.c_str());
    if (fluc_mode == 0) loader_bkg.FillCustomizedTH1D(bkg_MC_th1d_, { "M", "deltaE" }, { mapping_function_plus_M });
    else if (fluc_mode == 1) loader_bkg.FillCustomizedTH1D(bkg_MC_th1d_, { "M", "deltaE" }, { mapping_function_minus_M });
    else if (fluc_mode == 2) loader_bkg.FillCustomizedTH1D(bkg_MC_th1d_, { "M", "deltaE" }, { mapping_function_plus_DeltaE });
    else if (fluc_mode == 3) loader_bkg.FillCustomizedTH1D(bkg_MC_th1d_, { "M", "deltaE" }, { mapping_function_minus_DeltaE });
    else {
        printf("[FillHistogram_fluc_SR] fluctuation index should be one of 0, 1, 2, or 3\n");
        exit(1);
    }
    loader_bkg.end();


    // We do not open the box, So data_th1d is MC. We use the proper uncertainty
    data_th1d_->SetBinError(1, std::sqrt(data_th1d_->GetBinContent(1)));
    data_th1d_->SetBinError(2, std::sqrt(data_th1d_->GetBinContent(2)));
}

double BDT_cut_1_g = -1;
double BDT_cut_2_g = -1;

double mapping_function_A(std::vector<double> variables_) {
    double M = variables_.at(0);
    double deltaE = variables_.at(1);
    double BDT_1 = variables_.at(2);
    double BDT_2 = variables_.at(3);

    if (((M_peak_g - sizeM * M_left_sigma_g) < M) && (M <= (M_peak_g + sizeM * M_right_sigma_g)) && ((deltaE_peak_g - 5 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g + 5 * deltaE_right_sigma_g)) && (BDT_cut_1_g < BDT_1)) return 1.0;
    else if (((M_peak_g - sizeM * M_left_sigma_g) < M) && (M <= (M_peak_g + sizeM * M_right_sigma_g)) && ((deltaE_peak_g - 15 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g - 5 * deltaE_left_sigma_g)) && (BDT_cut_2_g < BDT_2)) return 2.0;
    else return NAN;

}

double mapping_function_B(std::vector<double> variables_) {
    double M = variables_.at(0);
    double deltaE = variables_.at(1);
    double BDT_1 = variables_.at(2);
    double BDT_2 = variables_.at(3);

    if (((((M_peak_g - 20.0 * M_left_sigma_g) < M) && (M <= (M_peak_g - 5.0 * M_left_sigma_g))) || (((M_peak_g + 5.0 * M_right_sigma_g) < M) && (M <= (M_peak_g + 20.0 * M_right_sigma_g)))) && ((deltaE_peak_g - 5 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g + 5 * deltaE_right_sigma_g)) && (BDT_cut_1_g < BDT_1)) return 1.0;
    else if (((((M_peak_g - 20.0 * M_left_sigma_g) < M) && (M <= (M_peak_g - 5.0 * M_left_sigma_g))) || (((M_peak_g + 5.0 * M_right_sigma_g) < M) && (M <= (M_peak_g + 20.0 * M_right_sigma_g)))) && ((deltaE_peak_g - 15 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g - 5 * deltaE_left_sigma_g)) && (BDT_cut_2_g < BDT_2)) return 2.0;
    else return NAN;

}

double mapping_function_C(std::vector<double> variables_) {
    double M = variables_.at(0);
    double deltaE = variables_.at(1);
    double BDT_1 = variables_.at(2);
    double BDT_2 = variables_.at(3);

    if (((M_peak_g - sizeM * M_left_sigma_g) < M) && (M <= (M_peak_g + sizeM * M_right_sigma_g)) && ((deltaE_peak_g - 5 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g + 5 * deltaE_right_sigma_g)) && (BDT_cut_1_g / 2.0 < BDT_1) && (BDT_1 <= BDT_cut_1_g)) return 1.0;
    else if (((M_peak_g - sizeM * M_left_sigma_g) < M) && (M <= (M_peak_g + sizeM * M_right_sigma_g)) && ((deltaE_peak_g - 15 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g - 5 * deltaE_left_sigma_g)) && (BDT_cut_2_g / 2.0 < BDT_2) && (BDT_2 <= BDT_cut_2_g)) return 2.0;
    else return NAN;

}

double mapping_function_D(std::vector<double> variables_) {
    double M = variables_.at(0);
    double deltaE = variables_.at(1);
    double BDT_1 = variables_.at(2);
    double BDT_2 = variables_.at(3);

    if (((((M_peak_g - 20.0 * M_left_sigma_g) < M) && (M <= (M_peak_g - 5.0 * M_left_sigma_g))) || (((M_peak_g + 5.0 * M_right_sigma_g) < M) && (M <= (M_peak_g + 20.0 * M_right_sigma_g)))) && ((deltaE_peak_g - 5 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g + 5 * deltaE_right_sigma_g)) && (BDT_cut_1_g / 2.0 < BDT_1) && (BDT_1 <= BDT_cut_1_g)) return 1.0;
    else if (((((M_peak_g - 20.0 * M_left_sigma_g) < M) && (M <= (M_peak_g - 5.0 * M_left_sigma_g))) || (((M_peak_g + 5.0 * M_right_sigma_g) < M) && (M <= (M_peak_g + 20.0 * M_right_sigma_g)))) && ((deltaE_peak_g - 15 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g - 5 * deltaE_left_sigma_g)) && (BDT_cut_2_g / 2.0 < BDT_2) && (BDT_2 <= BDT_cut_2_g)) return 2.0;
    else return NAN;

}

double mapping_function_Aprime(std::vector<double> variables_) {
    double M = variables_.at(0);
    double deltaE = variables_.at(1);
    double BDT_1 = variables_.at(2);
    double BDT_2 = variables_.at(3);

    if (((M_peak_g - sizeM * M_left_sigma_g) < M) && (M < (M_peak_g + sizeM * M_right_sigma_g)) && ((deltaE_peak_g - 5 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g + 5 * deltaE_right_sigma_g)) && (0.3 * BDT_cut_1_g < BDT_1) && (BDT_1 < 0.5 * BDT_cut_1_g)) return 1.0;
    else if (((M_peak_g - sizeM * M_left_sigma_g) < M) && (M < (M_peak_g + sizeM * M_right_sigma_g)) && ((deltaE_peak_g - 15 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g - 5 * deltaE_left_sigma_g)) && (0.3 * BDT_cut_2_g < BDT_2) && (BDT_2 < 0.5 * BDT_cut_2_g)) return 2.0;
    else return NAN;

}

double mapping_function_Bprime(std::vector<double> variables_) {
    double M = variables_.at(0);
    double deltaE = variables_.at(1);
    double BDT_1 = variables_.at(2);
    double BDT_2 = variables_.at(3);

    if (((((M_peak_g - 20.0 * M_left_sigma_g) < M) && (M < (M_peak_g - 5.0 * M_left_sigma_g))) || (((M_peak_g + 5.0 * M_right_sigma_g) < M) && (M < (M_peak_g + 20.0 * M_right_sigma_g)))) && ((deltaE_peak_g - 5 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g + 5 * deltaE_right_sigma_g)) && (0.3 * BDT_cut_1_g < BDT_1) && (BDT_1 < 0.5 * BDT_cut_1_g)) return 1.0;
    else if (((((M_peak_g - 20.0 * M_left_sigma_g) < M) && (M < (M_peak_g - 5.0 * M_left_sigma_g))) || (((M_peak_g + 5.0 * M_right_sigma_g) < M) && (M < (M_peak_g + 20.0 * M_right_sigma_g)))) && ((deltaE_peak_g - 15 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g - 5 * deltaE_left_sigma_g)) && (0.3 * BDT_cut_2_g < BDT_2) && (BDT_2 < 0.5 * BDT_cut_2_g)) return 2.0;
    else return NAN;

}

double mapping_function_Cprime(std::vector<double> variables_) {
    double M = variables_.at(0);
    double deltaE = variables_.at(1);
    double BDT_1 = variables_.at(2);
    double BDT_2 = variables_.at(3);

    if (((M_peak_g - sizeM * M_left_sigma_g) < M) && (M < (M_peak_g + sizeM * M_right_sigma_g)) && ((deltaE_peak_g - 5 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g + 5 * deltaE_right_sigma_g)) && (0.1 * BDT_cut_1_g < BDT_1) && (BDT_1 < 0.3 * BDT_cut_1_g)) return 1.0;
    else if (((M_peak_g - sizeM * M_left_sigma_g) < M) && (M < (M_peak_g + sizeM * M_right_sigma_g)) && ((deltaE_peak_g - 15 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g - 5 * deltaE_left_sigma_g)) && (0.1 * BDT_cut_2_g < BDT_2) && (BDT_2 < 0.3 * BDT_cut_2_g)) return 2.0;
    else return NAN;

}

double mapping_function_Dprime(std::vector<double> variables_) {
    double M = variables_.at(0);
    double deltaE = variables_.at(1);
    double BDT_1 = variables_.at(2);
    double BDT_2 = variables_.at(3);

    if (((((M_peak_g - 20.0 * M_left_sigma_g) < M) && (M < (M_peak_g - 5.0 * M_left_sigma_g))) || (((M_peak_g + 5.0 * M_right_sigma_g) < M) && (M < (M_peak_g + 20.0 * M_right_sigma_g)))) && ((deltaE_peak_g - 5 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g + 5 * deltaE_right_sigma_g)) && (0.1 * BDT_cut_1_g < BDT_1) && (BDT_1 < 0.3 * BDT_cut_1_g)) return 1.0;
    else if (((((M_peak_g - 20.0 * M_left_sigma_g) < M) && (M < (M_peak_g - 5.0 * M_left_sigma_g))) || (((M_peak_g + 5.0 * M_right_sigma_g) < M) && (M < (M_peak_g + 20.0 * M_right_sigma_g)))) && ((deltaE_peak_g - 15 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g - 5 * deltaE_left_sigma_g)) && (0.1 * BDT_cut_2_g < BDT_2) && (BDT_2 < 0.3 * BDT_cut_2_g)) return 2.0;
    else return NAN;

}

void ABCD_method(const char* input_path_1_, const char* input_path_2_, const char* FOM_1_path_, const char* FOM_2_path_, std::vector<TH1D*>* data_ABCD_, std::vector<std::string> data_list_) {
    std::string cut_BDT_1 = "(" + std::to_string(BDT_cut_1) + " < BDT_output_1)";
    std::string cut_M_1 = "((" + std::to_string(M_peak_g - 20 * M_left_sigma_g) + " < M) && (M < " + std::to_string(M_peak_g + 20 * M_right_sigma_g) + "))";
    std::string cut_deltaE_1 = "((" + std::to_string(deltaE_peak_g - 5 * deltaE_left_sigma_g) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak_g + 6 * deltaE_right_sigma_g) + "))";
    std::string cut_M_deltaE_1 = "(" + cut_M_1 + "&&" + cut_deltaE_1 + ")";
    std::string cut_total_1 = "(" + cut_M_deltaE_1 + "&&" + cut_BDT_1 + ")";

    std::string cut_BDT_2 = "(" + std::to_string(BDT_cut_2) + " < BDT_output_2)";
    std::string cut_M_2 = "((" + std::to_string(M_peak_g - 20 * M_left_sigma_g) + " < M) && (M < " + std::to_string(M_peak_g + 20 * M_right_sigma_g) + "))";
    std::string cut_deltaE_2 = "((" + std::to_string(deltaE_peak_g - 16 * deltaE_left_sigma_g) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak_g - 5 * deltaE_left_sigma_g) + "))";
    std::string cut_M_deltaE_2 = "(" + cut_M_2 + "&&" + cut_deltaE_2 + ")";
    std::string cut_total_2 = "(" + cut_M_deltaE_2 + "&&" + cut_BDT_2 + ")";

    std::string cut_region = cut_M_deltaE_1 + "||" + cut_M_deltaE_2;
    std::string cut_total = cut_total_1 + "||" + cut_total_2;
    
    ReadFOM(FOM_1_path_, &BDT_cut_1_g);
    ReadFOM(FOM_2_path_, &BDT_cut_2_g);

    TH1D* data_th1d_B = new TH1D("data_th1d_B", ";bin index;", 2, 0.5, 2.5);
    TH1D* data_th1d_C = new TH1D("data_th1d_C", ";bin index;", 2, 0.5, 2.5);
    TH1D* data_th1d_D = new TH1D("data_th1d_D", ";bin index;", 2, 0.5, 2.5);
    TH1D* data_th1d_Aprime = new TH1D("data_th1d_Aprime", ";bin index;", 2, 0.5, 2.5);
    TH1D* data_th1d_Bprime = new TH1D("data_th1d_Bprime", ";bin index;", 2, 0.5, 2.5);
    TH1D* data_th1d_Cprime = new TH1D("data_th1d_Cprime", ";bin index;", 2, 0.5, 2.5);
    TH1D* data_th1d_Dprime = new TH1D("data_th1d_Dprime", ";bin index;", 2, 0.5, 2.5);


    Loader loader_data("tau_lfv");
    for (int i = 0; i < data_list_.size(); i++) loader_data.Load((input_path_1_ + std::string("/") + data_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), "root", data_list_.at(i).c_str());
    loader_data.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_05_prompt", { {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_05_prompt", { {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_01_prompt", { {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.Cut(cut_region.c_str());
    loader_data.RandomBCS();
    loader_data.IsBCSValid();
    loader_data.FillCustomizedTH1D(data_th1d_B, { "M", "deltaE", "BDT_output_1", "BDT_output_2" }, { mapping_function_B });
    loader_data.FillCustomizedTH1D(data_th1d_C, { "M", "deltaE", "BDT_output_1", "BDT_output_2" }, { mapping_function_C });
    loader_data.FillCustomizedTH1D(data_th1d_D, { "M", "deltaE", "BDT_output_1", "BDT_output_2" }, { mapping_function_D });
    loader_data.FillCustomizedTH1D(data_th1d_Aprime, { "M", "deltaE", "BDT_output_1", "BDT_output_2" }, { mapping_function_Aprime });
    loader_data.FillCustomizedTH1D(data_th1d_Bprime, { "M", "deltaE", "BDT_output_1", "BDT_output_2" }, { mapping_function_Bprime });
    loader_data.FillCustomizedTH1D(data_th1d_Cprime, { "M", "deltaE", "BDT_output_1", "BDT_output_2" }, { mapping_function_Cprime });
    loader_data.FillCustomizedTH1D(data_th1d_Dprime, { "M", "deltaE", "BDT_output_1", "BDT_output_2" }, { mapping_function_Dprime });
    loader_data.end();

    // Keep the control observations, including empty bins, for the Poisson likelihood.
    // The order is B, C, D, A', B', C', D'. Region A is not used for background estimation.
    *data_ABCD_ = { data_th1d_B, data_th1d_C, data_th1d_D, data_th1d_Aprime, data_th1d_Bprime, data_th1d_Cprime, data_th1d_Dprime };
    for (int i = 1; i <= 2; i++) {
        printf("============== Poisson ABCD control region %d ==============\n", i);
        for (int j = 0; j < (int)data_ABCD_->size(); j++) {
            TH1D* hist = data_ABCD_->at(j);
            double yield = hist->GetBinContent(i);
            if (!std::isfinite(yield) || yield < 0.0) {
                printf("[ABCD_method] invalid yield in %s, region %d\n", hist->GetName(), i);
                exit(1);
            }
            // We do not open the box, So data_th1d is MC. We use the proper uncertainty
            // MC yields are projected data counts. This error is only for display, not a likelihood constraint.
            // After box open, remove this override and use the unweighted data counts.
            hist->SetBinError(i, std::sqrt(yield)); /* After box open, it should be removed! */
            printf("%s = %lf\n", hist->GetName(), yield);
        }
        double N_B = data_th1d_B->GetBinContent(i);
        double N_C = data_th1d_C->GetBinContent(i);
        double N_D = data_th1d_D->GetBinContent(i);
        // This is only the control-only maximum likelihood estimate, not a fixed background template.
        if (N_D > 0.0) printf("control-only estimated N_A = %lf (uncertainty from Poisson likelihood)\n", N_B * (N_C / N_D));
        else printf("[ABCD_method] N_D = 0; the transfer factor requires a likelihood scan\n");
    }
}

void Write_ABCD_histograms(std::vector<TH1D*> data_ABCD_) {
    // Unit templates keep the expected yields free even when an observed control bin is empty.
    TH1D* bkg_ABCD_unit = new TH1D("bkg_ABCD_unit", ";bin index;", 1, 0.5, 1.5);
    bkg_ABCD_unit->SetBinContent(1, 1.0);
    bkg_ABCD_unit->SetBinError(1, 0.0);
    bkg_ABCD_unit->Write();
    for (int i = 1; i <= 2; i++) {
        std::string suffix = "_region" + std::to_string(i);
        TH1D* bkg_A = new TH1D(("bkg_ABCD_unit" + suffix).c_str(), ";bin index;", 2, 0.5, 2.5);
        bkg_A->SetBinContent(i, 1.0);
        bkg_A->SetBinError(i, 0.0);
        bkg_A->Write();
        for (int j = 0; j < (int)data_ABCD_.size(); j++) {
            TH1D* hist = data_ABCD_.at(j);
            TH1D* data = new TH1D((std::string(hist->GetName()) + suffix).c_str(), ";bin index;", 1, 0.5, 1.5);
            data->SetBinContent(1, hist->GetBinContent(i));
            data->SetBinError(1, hist->GetBinError(i));
            data->Write();
        }
    }
    for (int i = 0; i < (int)data_ABCD_.size(); i++) data_ABCD_.at(i)->Write();
}

void Add_ABCD_channels(RooStats::HistFactory::Measurement& meas_, RooStats::HistFactory::Channel& channel_A_, const char* filename_, std::vector<TH1D*> data_ABCD_, bool use_nonclosure_) {
    // A = beta * r, B = beta, C = nu * r, D = nu, independently for each deltaE region.
    // The control samples assume negligible signal contamination, as in the original ABCD method.
    // Do not add ActivateStatError: the control channel Poisson terms already give the counting uncertainty.
    for (int i = 1; i <= 2; i++) {
        std::string suffix = "_region" + std::to_string(i);
        double N_B = data_ABCD_.at(0)->GetBinContent(i);
        double N_C = data_ABCD_.at(1)->GetBinContent(i);
        double N_D = data_ABCD_.at(2)->GetBinContent(i);
        // Positive starting values are only minimizer seeds. No events are added to the observations.
        double beta = N_B > 0.0 ? N_B : 1.0;
        double nu = N_D > 0.0 ? N_D : 1.0;
        double r = N_C > 0.0 ? N_C / nu : 1.0;
        double beta_max = std::max(100.0, 10.0 * beta);
        double nu_max = std::max(100.0, 10.0 * nu);
        double r_max = std::max(100.0, 10.0 * r);

        RooStats::HistFactory::Sample bkg_A(("bkg_Belle_II" + suffix).c_str(), ("bkg_ABCD_unit" + suffix).c_str(), filename_);
        bkg_A.AddNormFactor("ABCD_beta" + suffix, beta, 0.0, beta_max);
        bkg_A.AddNormFactor("ABCD_r" + suffix, r, 0.0, r_max);
        if (use_nonclosure_) bkg_A.AddNormFactor("ABCD_kappa" + suffix, 1.0, 0.0, 100.0);
        bkg_A.SetNormalizeByTheory(false);
        channel_A_.AddSample(bkg_A);

        std::vector<std::string> regions = { "B", "C", "D" };
        for (int j = 0; j < (int)regions.size(); j++) {
            std::string name = "ABCD_" + regions.at(j) + suffix;
            RooStats::HistFactory::Channel channel(name.c_str());
            channel.SetData(("data_th1d_" + regions.at(j) + suffix).c_str(), filename_);
            RooStats::HistFactory::Sample bkg(("bkg_" + name).c_str(), "bkg_ABCD_unit", filename_);
            if (j == 0) bkg.AddNormFactor("ABCD_beta" + suffix, beta, 0.0, beta_max);
            else bkg.AddNormFactor("ABCD_nu" + suffix, nu, 0.0, nu_max);
            if (j == 1) bkg.AddNormFactor("ABCD_r" + suffix, r, 0.0, r_max);
            bkg.SetNormalizeByTheory(false);
            channel.AddSample(bkg);
            meas_.AddChannel(channel);
        }

        // Optional non-closure constraint from the four disjoint validation regions.
        // A' = kappa * beta' * r', B' = beta', C' = nu' * r', D' = nu'.
        // Only kappa is shared with the application; its transfer to the higher BDT region is an extra assumption.
        // Do not add a separate non-closure HistoSys, which would count the validation uncertainty twice.
        if (use_nonclosure_) {
            double N_Bprime = data_ABCD_.at(4)->GetBinContent(i);
            double N_Cprime = data_ABCD_.at(5)->GetBinContent(i);
            double N_Dprime = data_ABCD_.at(6)->GetBinContent(i);
            double beta_prime = N_Bprime > 0.0 ? N_Bprime : 1.0;
            double nu_prime = N_Dprime > 0.0 ? N_Dprime : 1.0;
            double r_prime = N_Cprime > 0.0 ? N_Cprime / nu_prime : 1.0;
            double beta_prime_max = std::max(100.0, 10.0 * beta_prime);
            double nu_prime_max = std::max(100.0, 10.0 * nu_prime);
            double r_prime_max = std::max(100.0, 10.0 * r_prime);
            std::vector<std::string> validation_regions = { "Aprime", "Bprime", "Cprime", "Dprime" };
            for (int j = 0; j < (int)validation_regions.size(); j++) {
                std::string name = "ABCD_" + validation_regions.at(j) + suffix;
                RooStats::HistFactory::Channel channel(name.c_str());
                channel.SetData(("data_th1d_" + validation_regions.at(j) + suffix).c_str(), filename_);
                RooStats::HistFactory::Sample bkg(("bkg_" + name).c_str(), "bkg_ABCD_unit", filename_);
                if (j < 2) bkg.AddNormFactor("ABCD_beta_validation" + suffix, beta_prime, 0.0, beta_prime_max);
                else bkg.AddNormFactor("ABCD_nu_validation" + suffix, nu_prime, 0.0, nu_prime_max);
                if (j == 0 || j == 2) bkg.AddNormFactor("ABCD_r_validation" + suffix, r_prime, 0.0, r_prime_max);
                if (j == 0) bkg.AddNormFactor("ABCD_kappa" + suffix, 1.0, 0.0, 100.0);
                bkg.SetNormalizeByTheory(false);
                channel.AddSample(bkg);
                meas_.AddChannel(channel);
            }
            if (N_Bprime == 0.0 || N_Dprime == 0.0) printf("[ABCD_method] empty validation sideband in region %d; kappa may have no finite upper bound\n", i);
        }
    }
}

void Set_ABCD_parameter_ranges(RooWorkspace* w_, bool use_nonclosure_) {
    // HistFactory needs finite construction ranges. They must not act as artificial statistical constraints.
    // Remove the upper bounds in the saved workspace, retaining the physical lower bound of zero.
    std::vector<std::string> parameters = { "beta", "nu", "r" };
    if (use_nonclosure_) {
        parameters.push_back("beta_validation");
        parameters.push_back("nu_validation");
        parameters.push_back("r_validation");
        parameters.push_back("kappa");
    }
    for (int i = 1; i <= 2; i++) {
        for (int j = 0; j < (int)parameters.size(); j++) {
            std::string name = "ABCD_" + parameters.at(j) + "_region" + std::to_string(i);
            RooRealVar* parameter = w_->var(name.c_str());
            if (parameter == nullptr) {
                printf("[ABCD_method] missing likelihood parameter %s\n", name.c_str());
                exit(1);
            }
            parameter->removeMax();
        }
    }
}

int main(int argc, char* argv[]) {
    /*
    * argv[1]: input path 1
    * argv[2]: input path 2
    * argv[3]: input path 1 for ABCD method
    * argv[4]: FOM_1 filename
    * argv[5]: FOM_2 filename
    * argv[6]: output path
    * argv[7]: signal list (separated by colon)
    * argv[8]: background list (separated by colon)
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

    // relative error
    TH1D* data_th1d_stat_err = new TH1D("data_th1d_stat_err", ";bin index;", 2, 0.5, 2.5);
    TH1D* signal_MC_th1d_stat_err = new TH1D("signal_MC_th1d_stat_err", ";bin index;", 2, 0.5, 2.5);
    TH1D* bkg_MC_th1d_stat_err = new TH1D("bkg_MC_th1d_stat_err", ";bin index;", 2, 0.5, 2.5);

    TH1D* data_pos_M_th1d = new TH1D("data_pos_M_th1d", ";bin index;", 2, 0.5, 2.5);
    TH1D* signal_pos_M_MC_th1d = new TH1D("signal_pos_M_MC_th1d", ";bin index;", 2, 0.5, 2.5);
    TH1D* bkg_pos_M_MC_th1d = new TH1D("bkg_pos_M_MC_th1d", ";bin index;", 2, 0.5, 2.5);

    TH1D* data_neg_M_th1d = new TH1D("data_neg_M_th1d", ";bin index;", 2, 0.5, 2.5);
    TH1D* signal_neg_M_MC_th1d = new TH1D("signal_neg_M_MC_th1d", ";bin index;", 2, 0.5, 2.5);
    TH1D* bkg_neg_M_MC_th1d = new TH1D("bkg_neg_M_MC_th1d", ";bin index;", 2, 0.5, 2.5);

    TH1D* data_pos_DeltaE_th1d = new TH1D("data_pos_DeltaE_th1d", ";bin index;", 2, 0.5, 2.5);
    TH1D* signal_pos_DeltaE_MC_th1d = new TH1D("signal_pos_DeltaE_MC_th1d", ";bin index;", 2, 0.5, 2.5);
    TH1D* bkg_pos_DeltaE_MC_th1d = new TH1D("bkg_pos_DeltaE_MC_th1d", ";bin index;", 2, 0.5, 2.5);

    TH1D* data_neg_DeltaE_th1d = new TH1D("data_neg_DeltaE_th1d", ";bin index;", 2, 0.5, 2.5);
    TH1D* signal_neg_DeltaE_MC_th1d = new TH1D("signal_neg_DeltaE_MC_th1d", ";bin index;", 2, 0.5, 2.5);
    TH1D* bkg_neg_DeltaE_MC_th1d = new TH1D("bkg_neg_DeltaE_MC_th1d", ";bin index;", 2, 0.5, 2.5);

    ReadFOM((std::string(argv[1]) + "/GridSearch_one/FOM.log").c_str(), &BDT_cut_1);
    ReadFOM((std::string(argv[1]) + "/GridSearch_two/FOM.log").c_str(), &BDT_cut_2);

    std::vector<TH1D*> signal_MC_th1d_muonID;
    std::vector<TH1D*> bkg_MC_th1d_muonID;

    std::vector<TH1D*> signal_MC_th1d_luminosity;
    std::vector<TH1D*> bkg_MC_th1d_luminosity;

    // uncorrelated relative uncertainty
    TH1D* signal_MC_th1d_uncorr = new TH1D("signal_MC_th1d_uncorr", ";bin index;", 2, 0.5, 2.5);
    TH1D* bkg_MC_th1d_uncorr = new TH1D("bkg_MC_th1d_uncorr", ";bin index;", 2, 0.5, 2.5);

    std::vector<std::string> signal_list = split(argv[7], ':');
    std::vector<std::string> background_list = split(argv[8], ':');

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

    // we do not open the box, so I just use background MC
    FillHistogram(argv[1], argv[2], data_th1d, signal_MC_th1d, bkg_MC_th1d, data_th1d_stat_err, signal_MC_th1d_stat_err, bkg_MC_th1d_stat_err, background_list, signal_list, background_list);

    // muonID histogram
    ReadPCA((std::string(argv[1]) + "/muonID_PCA").c_str(), signal_MC_th1d, "muonID", &signal_MC_th1d_muonID);
    ReadPCA_remain((std::string(argv[1]) + "/muonID_PCA_remain").c_str(), signal_MC_th1d, signal_MC_th1d_uncorr);

    // luminosity histogram
    ReadPCA((std::string(argv[1]) + "/luminosity_PCA").c_str(), signal_MC_th1d, "luminosity", &signal_MC_th1d_luminosity);
    ReadPCA_remain((std::string(argv[1]) + "/luminosity_PCA_remain").c_str(), signal_MC_th1d, signal_MC_th1d_uncorr);

    // SR fluctuation
    FillHistogram_fluc_SR(argv[1], argv[2], data_pos_M_th1d, signal_pos_M_MC_th1d, bkg_pos_M_MC_th1d, background_list, signal_list, background_list, 0);
    FillHistogram_fluc_SR(argv[1], argv[2], data_neg_M_th1d, signal_neg_M_MC_th1d, bkg_neg_M_MC_th1d, background_list, signal_list, background_list, 1);
    FillHistogram_fluc_SR(argv[1], argv[2], data_pos_DeltaE_th1d, signal_pos_DeltaE_MC_th1d, bkg_pos_DeltaE_MC_th1d, background_list, signal_list, background_list, 2);
    FillHistogram_fluc_SR(argv[1], argv[2], data_neg_DeltaE_th1d, signal_neg_DeltaE_MC_th1d, bkg_neg_DeltaE_MC_th1d, background_list, signal_list, background_list, 3);

    // ABCD method
    std::vector<TH1D*> data_ABCD;
    ABCD_method(argv[1], argv[3], argv[4], argv[5], &data_ABCD, background_list);
    bool use_ABCD_nonclosure = false;
    /* ABCD_nonclosure is commented out for now. Uncomment the next line to include the validation Poisson terms. */
    // use_ABCD_nonclosure = true;

    // print information
    printf("data:\n");
    printf("%lf+-%lf %lf+-%lf\n", data_th1d->GetBinContent(1), data_th1d->GetBinError(1), data_th1d->GetBinContent(2), data_th1d->GetBinError(2));

    printf("\n");

    printf("signal:\n");
    printf("%lf+-%lf %lf+-%lf\n", signal_MC_th1d->GetBinContent(1), signal_MC_th1d->GetBinError(1), signal_MC_th1d->GetBinContent(2), signal_MC_th1d->GetBinError(2));

    printf("\n");

    printf("bkg:\n");
    printf("%lf+-%lf %lf+-%lf\n", bkg_MC_th1d->GetBinContent(1), bkg_MC_th1d->GetBinError(1), bkg_MC_th1d->GetBinContent(2), bkg_MC_th1d->GetBinError(2));

    printf("\n");

    // Save in root file
    TFile* file = new TFile((std::string(argv[6]) + "/histogram_output.root").c_str(), "RECREATE");

    data_th1d->Write();
    signal_MC_th1d->Write();
    bkg_MC_th1d->Write();
    Write_ABCD_histograms(data_ABCD);

    data_th1d_stat_err->Write();
    signal_MC_th1d_stat_err->Write();
    bkg_MC_th1d_stat_err->Write();

    data_pos_M_th1d->Write();
    signal_pos_M_MC_th1d->Write();
    bkg_pos_M_MC_th1d->Write();

    data_neg_M_th1d->Write();
    signal_neg_M_MC_th1d->Write();
    bkg_neg_M_MC_th1d->Write();

    data_pos_DeltaE_th1d->Write();
    signal_pos_DeltaE_MC_th1d->Write();
    bkg_pos_DeltaE_MC_th1d->Write();

    data_neg_DeltaE_th1d->Write();
    signal_neg_DeltaE_MC_th1d->Write();
    bkg_neg_DeltaE_MC_th1d->Write();

    for (int i = 0; i < signal_MC_th1d_muonID.size(); i++) signal_MC_th1d_muonID.at(i)->Write();
    for (int i = 0; i < bkg_MC_th1d_muonID.size(); i++) bkg_MC_th1d_muonID.at(i)->Write();

    for (int i = 0; i < signal_MC_th1d_luminosity.size(); i++) signal_MC_th1d_luminosity.at(i)->Write();
    for (int i = 0; i < bkg_MC_th1d_luminosity.size(); i++) bkg_MC_th1d_luminosity.at(i)->Write();

    signal_MC_th1d_uncorr->Write();
    bkg_MC_th1d_uncorr->Write();

    file->Close();


    // make workspace
    RooStats::HistFactory::Measurement meas("my_measurement", "my measurement");
    meas.SetOutputFilePrefix((argv[1] + std::string("/") + "my_measurement").c_str());

    // setting measurement
    meas.SetPOI("mu");
    meas.SetLumi(1.0);
    meas.AddConstantParam("Lumi");

    // define channels
    RooStats::HistFactory::Channel channel_Belle_II("Belle_II");
    channel_Belle_II.SetStatErrorConfig(1e-5, "Poisson");

    // fill channels
    channel_Belle_II.SetData("data_th1d", (std::string(argv[6]) + "/histogram_output.root").c_str());

    RooStats::HistFactory::Sample signal_Belle_II("signal_Belle_II", "signal_MC_th1d", (std::string(argv[6]) + "/histogram_output.root").c_str());
    signal_Belle_II.ActivateStatError("signal_MC_th1d_stat_err", (std::string(argv[6]) + "/histogram_output.root").c_str(), "");
    signal_Belle_II.AddNormFactor("mu", 1.0, 0.0, 100.0);
    signal_Belle_II.AddOverallSys("tracking_efficiency", 1.0 - (track_rel_uncertainty / 100.0) * 3, 1.0 + (track_rel_uncertainty / 100.0) * 3);
    signal_Belle_II.AddHistoSys("M_resolution", "signal_neg_M_MC_th1d", (std::string(argv[6]) + "/histogram_output.root").c_str(), "", "signal_pos_M_MC_th1d", (std::string(argv[6]) + "/histogram_output.root").c_str(), "");
    signal_Belle_II.AddHistoSys("DeltaE_resolution", "signal_neg_DeltaE_MC_th1d", (std::string(argv[6]) + "/histogram_output.root").c_str(), "", "signal_pos_DeltaE_MC_th1d", (std::string(argv[6]) + "/histogram_output.root").c_str(), "");
    signal_Belle_II.AddOverallSys("cross_section", 1.0 - tau_crosssection_4S_reluncertainty, 1.0 + tau_crosssection_4S_reluncertainty);
    for (int i = 0; i < signal_MC_th1d_muonID.size() / 2; i++) signal_Belle_II.AddHistoSys(("muonID_" + std::to_string(i)).c_str(), ("signal_hist_muonID_n_" + std::to_string(i)).c_str(), (std::string(argv[6]) + "/histogram_output.root").c_str(), "", ("signal_hist_muonID_p_" + std::to_string(i)).c_str(), (std::string(argv[6]) + "/histogram_output.root").c_str(), "");
    for (int i = 0; i < signal_MC_th1d_luminosity.size() / 2; i++) signal_Belle_II.AddHistoSys(("luminosity_" + std::to_string(i)).c_str(), ("signal_hist_luminosity_n_" + std::to_string(i)).c_str(), (std::string(argv[6]) + "/histogram_output.root").c_str(), "", ("signal_hist_luminosity_p_" + std::to_string(i)).c_str(), (std::string(argv[6]) + "/histogram_output.root").c_str(), "");
    signal_Belle_II.AddShapeSys("uncorrelated_error", RooStats::HistFactory::Constraint::Gaussian, "signal_MC_th1d_uncorr", (std::string(argv[6]) + "/histogram_output.root").c_str(), "");
    signal_Belle_II.SetNormalizeByTheory(false);

    channel_Belle_II.AddSample(signal_Belle_II);
    Add_ABCD_channels(meas, channel_Belle_II, (std::string(argv[6]) + "/histogram_output.root").c_str(), data_ABCD, use_ABCD_nonclosure);

    // add channel to measurement
    meas.AddChannel(channel_Belle_II);
    meas.CollectHistograms();

    RooWorkspace* w;
    w = RooStats::HistFactory::MakeModelAndMeasurementFast(meas);
    Set_ABCD_parameter_ranges(w, use_ABCD_nonclosure);

    w->Print();
    w->writeToFile((std::string(argv[6]) + "/workspace.root").c_str());

    meas.PrintXML("my_measurement");

    return 0;
}
