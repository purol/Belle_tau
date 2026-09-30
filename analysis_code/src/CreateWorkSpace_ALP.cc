#include <stdio.h>
#include <string>
#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>
#include <format>

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
    ABCDParameters parameters = { BDT_cut_1, BDT_cut_2, deltaE_peak_g, deltaE_left_sigma_g, deltaE_right_sigma_g, M_peak_g, M_left_sigma_g, M_right_sigma_g, sizeM };
    return mapping_function_ABCD(variables_, parameters, false);
}

double mapping_function_plus_M(std::vector<double> variables_) {
    ABCDParameters parameters = { BDT_cut_1, BDT_cut_2, deltaE_peak_g, deltaE_left_sigma_g, deltaE_right_sigma_g, M_peak_g, M_left_sigma_g, M_right_sigma_g, sizeM };
    return mapping_function_ABCD(variables_, parameters, false, 0);
}

double mapping_function_minus_M(std::vector<double> variables_) {
    ABCDParameters parameters = { BDT_cut_1, BDT_cut_2, deltaE_peak_g, deltaE_left_sigma_g, deltaE_right_sigma_g, M_peak_g, M_left_sigma_g, M_right_sigma_g, sizeM };
    return mapping_function_ABCD(variables_, parameters, false, 1);
}

double mapping_function_plus_DeltaE(std::vector<double> variables_) {
    ABCDParameters parameters = { BDT_cut_1, BDT_cut_2, deltaE_peak_g, deltaE_left_sigma_g, deltaE_right_sigma_g, M_peak_g, M_left_sigma_g, M_right_sigma_g, sizeM };
    return mapping_function_ABCD(variables_, parameters, false, 2);
}

double mapping_function_minus_DeltaE(std::vector<double> variables_) {
    ABCDParameters parameters = { BDT_cut_1, BDT_cut_2, deltaE_peak_g, deltaE_left_sigma_g, deltaE_right_sigma_g, M_peak_g, M_left_sigma_g, M_right_sigma_g, sizeM };
    return mapping_function_ABCD(variables_, parameters, false, 3);
}

void FillHistogram(const char* input_path_1_, const char* input_path_2_, TH1D* data_th1d_, TH1D* signal_MC_th1d_, TH1D* bkg_MC_th1d_, TH1D* data_th1d_stat_err_, TH1D* signal_MC_th1d_stat_err_, TH1D* bkg_MC_th1d_stat_err_, std::vector<std::string> data_list_, std::vector<std::string> signal_list_, std::vector<std::string> background_list_) {
    std::string cut_M_1 = "((" + std::to_string(M_peak_g - 20 * M_left_sigma_g) + " < M) && (M < " + std::to_string(M_peak_g + 20 * M_right_sigma_g) + "))";
    std::string cut_deltaE_1 = "((" + std::to_string(deltaE_peak_g - 5 * deltaE_left_sigma_g) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak_g + 6 * deltaE_right_sigma_g) + "))";
    std::string cut_M_deltaE_1 = "(" + cut_M_1 + "&&" + cut_deltaE_1 + ")";

    std::string cut_M_2 = "((" + std::to_string(M_peak_g - 20 * M_left_sigma_g) + " < M) && (M < " + std::to_string(M_peak_g + 20 * M_right_sigma_g) + "))";
    std::string cut_deltaE_2 = "((" + std::to_string(deltaE_peak_g - 16 * deltaE_left_sigma_g) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak_g - 5 * deltaE_left_sigma_g) + "))";
    std::string cut_M_deltaE_2 = "(" + cut_M_2 + "&&" + cut_deltaE_2 + ")";

    std::string cut_region = cut_M_deltaE_1 + "||" + cut_M_deltaE_2;

    std::string cut_m_alpha = "(" + std::to_string(mass - M_left_cut_value) + "< extraInfo__boALP_M__bc) && (extraInfo__boALP_M__bc <" + std::to_string(mass + M_right_cut_value) + ")";
    
    // data
    Loader loader_data("tau_lfv");
    for (int i = 0; i < data_list_.size(); i++) loader_data.Load((input_path_1_ + std::string("/") + data_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), "root", data_list_.at(i).c_str());
    loader_data.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_05_ALP", { {"index", "first_muon_index"},  {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_05_ALP", { {"index", "second_muon_index"},  {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_01_ALP", { {"index", "third_muon_index"},  {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("KS0_tracking_run1", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("KS0_tracking_run2", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} }); /* After box open, it should be removed! */
    loader_data.Cut(cut_region.c_str());
    loader_data.Cut(cut_m_alpha.c_str());
    loader_data.RandomBCS();
    loader_data.IsBCSValid();
    loader_data.FillCustomizedTH1D(data_th1d_, { "M", "deltaE", BDT_output_1_name.c_str(), BDT_output_2_name.c_str() }, { mapping_function });
    loader_data.end();

    // signal MC
    Loader loader_signal("tau_lfv");
    for (int i = 0; i < signal_list_.size(); i++) loader_signal.Load((input_path_1_ + std::string("/") + signal_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), ("alpha_mass" + std::format("{:g}", mass) + "_life" + std::format("{:g}", life) + "_A" + std::to_string(A) + "_B" + std::to_string(B) + "_").c_str(), signal_list_.at(i).c_str());
    loader_signal.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_signal.AddWeight("muonID_05_ALP", { {"index", "first_muon_index"},  {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
    loader_signal.AddWeight("muonID_05_ALP", { {"index", "second_muon_index"},  {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
    loader_signal.AddWeight("muonID_01_ALP", { {"index", "third_muon_index"},  {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
    loader_signal.AddWeight("KS0_tracking_run1", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
    loader_signal.AddWeight("KS0_tracking_run2", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
    loader_signal.Cut(cut_region.c_str());
    loader_signal.Cut(cut_m_alpha.c_str());
    loader_signal.RandomBCS();
    loader_signal.IsBCSValid();
    loader_signal.FillCustomizedTH1D(signal_MC_th1d_, { "M", "deltaE", BDT_output_1_name.c_str(), BDT_output_2_name.c_str() }, { mapping_function });
    loader_signal.end();

    // background MC
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
    loader_bkg.FillCustomizedTH1D(bkg_MC_th1d_, { "M", "deltaE", BDT_output_1_name.c_str(), BDT_output_2_name.c_str() }, { mapping_function });
    loader_bkg.end();


    // We do not open the box, So data_th1d is MC. We use the proper uncertainty
    // These are projected data errors. Remove the overrides after box open.
    for (int i = 1; i <= 8; i++) {
        data_th1d_->SetBinError(i, std::sqrt(data_th1d_->GetBinContent(i))); /* After box open, it should be removed! */
    }

    // get statistical uncertainty (relative error)
    // Empty MC bins have zero relative uncertainty; all eight bins are filled from the selected events.
    for (int i = 1; i <= 8; i++) {
        if (data_th1d_->GetBinContent(i) > 0.0) data_th1d_stat_err_->SetBinContent(i, data_th1d_->GetBinError(i) / data_th1d_->GetBinContent(i));
        else data_th1d_stat_err_->SetBinContent(i, 0.0);
        if (signal_MC_th1d_->GetBinContent(i) > 0.0) signal_MC_th1d_stat_err_->SetBinContent(i, signal_MC_th1d_->GetBinError(i) / signal_MC_th1d_->GetBinContent(i));
        else signal_MC_th1d_stat_err_->SetBinContent(i, 0.0);
        if (bkg_MC_th1d_->GetBinContent(i) > 0.0) bkg_MC_th1d_stat_err_->SetBinContent(i, bkg_MC_th1d_->GetBinError(i) / bkg_MC_th1d_->GetBinContent(i));
        else bkg_MC_th1d_stat_err_->SetBinContent(i, 0.0);
    }
}

void FillHistogram_fluc_SR(const char* input_path_1_, const char* input_path_2_, TH1D* data_th1d_, TH1D* signal_MC_th1d_, TH1D* bkg_MC_th1d_, std::vector<std::string> data_list_, std::vector<std::string> signal_list_, std::vector<std::string> background_list_, int fluc_mode) {
    /*
    * fluc mode:
    * 0: positive M fluctuation
    * 1: negative M fluctuation
    * 2: positive DeltaE fluctuation
    * 3: negative DeltaE fluctuation
    */

    std::string cut_M_1 = "((" + std::to_string(M_peak_g - 20 * M_left_sigma_g) + " < M) && (M < " + std::to_string(M_peak_g + 20 * M_right_sigma_g) + "))";
    std::string cut_deltaE_1 = "((" + std::to_string(deltaE_peak_g - 5 * deltaE_left_sigma_g) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak_g + 6 * deltaE_right_sigma_g) + "))";
    std::string cut_M_deltaE_1 = "(" + cut_M_1 + "&&" + cut_deltaE_1 + ")";

    std::string cut_M_2 = "((" + std::to_string(M_peak_g - 20 * M_left_sigma_g) + " < M) && (M < " + std::to_string(M_peak_g + 20 * M_right_sigma_g) + "))";
    std::string cut_deltaE_2 = "((" + std::to_string(deltaE_peak_g - 16 * deltaE_left_sigma_g) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak_g - 5 * deltaE_left_sigma_g) + "))";
    std::string cut_M_deltaE_2 = "(" + cut_M_2 + "&&" + cut_deltaE_2 + ")";

    std::string cut_region = cut_M_deltaE_1 + "||" + cut_M_deltaE_2;

    std::string cut_m_alpha = "(" + std::to_string(mass - M_left_cut_value) + "< extraInfo__boALP_M__bc) && (extraInfo__boALP_M__bc <" + std::to_string(mass + M_right_cut_value) + ")";

    // data
    Loader loader_data("tau_lfv");
    for (int i = 0; i < data_list_.size(); i++) loader_data.Load((input_path_1_ + std::string("/") + data_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), "root", data_list_.at(i).c_str());
    loader_data.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_05_ALP", { {"index", "first_muon_index"},  {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_05_ALP", { {"index", "second_muon_index"},  {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_01_ALP", { {"index", "third_muon_index"},  {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("KS0_tracking_run1", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("KS0_tracking_run2", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} }); /* After box open, it should be removed! */
    loader_data.Cut(cut_region.c_str());
    loader_data.Cut(cut_m_alpha.c_str());
    loader_data.RandomBCS();
    loader_data.IsBCSValid();
    if (fluc_mode == 0) loader_data.FillCustomizedTH1D(data_th1d_, { "M", "deltaE", BDT_output_1_name.c_str(), BDT_output_2_name.c_str() }, { mapping_function_plus_M });
    else if (fluc_mode == 1) loader_data.FillCustomizedTH1D(data_th1d_, { "M", "deltaE", BDT_output_1_name.c_str(), BDT_output_2_name.c_str() }, { mapping_function_minus_M });
    else if (fluc_mode == 2) loader_data.FillCustomizedTH1D(data_th1d_, { "M", "deltaE", BDT_output_1_name.c_str(), BDT_output_2_name.c_str() }, { mapping_function_plus_DeltaE });
    else if (fluc_mode == 3) loader_data.FillCustomizedTH1D(data_th1d_, { "M", "deltaE", BDT_output_1_name.c_str(), BDT_output_2_name.c_str() }, { mapping_function_minus_DeltaE });
    else {
        printf("[FillHistogram_fluc_SR] fluctuation index should be one of 0, 1, 2, or 3\n");
        exit(1);
    }
    loader_data.end();

    // signal MC
    Loader loader_signal("tau_lfv");
    for (int i = 0; i < signal_list_.size(); i++) loader_signal.Load((input_path_1_ + std::string("/") + signal_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), ("alpha_mass" + std::format("{:g}", mass) + "_life" + std::format("{:g}", life) + "_A" + std::to_string(A) + "_B" + std::to_string(B) + "_").c_str(), signal_list_.at(i).c_str());
    loader_signal.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_signal.AddWeight("muonID_05_ALP", { {"index", "first_muon_index"},  {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
    loader_signal.AddWeight("muonID_05_ALP", { {"index", "second_muon_index"},  {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
    loader_signal.AddWeight("muonID_01_ALP", { {"index", "third_muon_index"},  {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
    loader_signal.AddWeight("KS0_tracking_run1", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
    loader_signal.AddWeight("KS0_tracking_run2", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
    loader_signal.Cut(cut_region.c_str());
    loader_signal.Cut(cut_m_alpha.c_str());
    loader_signal.RandomBCS();
    loader_signal.IsBCSValid();
    if (fluc_mode == 0) loader_signal.FillCustomizedTH1D(signal_MC_th1d_, { "M", "deltaE", BDT_output_1_name.c_str(), BDT_output_2_name.c_str() }, { mapping_function_plus_M });
    else if (fluc_mode == 1) loader_signal.FillCustomizedTH1D(signal_MC_th1d_, { "M", "deltaE", BDT_output_1_name.c_str(), BDT_output_2_name.c_str() }, { mapping_function_minus_M });
    else if (fluc_mode == 2) loader_signal.FillCustomizedTH1D(signal_MC_th1d_, { "M", "deltaE", BDT_output_1_name.c_str(), BDT_output_2_name.c_str() }, { mapping_function_plus_DeltaE });
    else if (fluc_mode == 3) loader_signal.FillCustomizedTH1D(signal_MC_th1d_, { "M", "deltaE", BDT_output_1_name.c_str(), BDT_output_2_name.c_str() }, { mapping_function_minus_DeltaE });
    else {
        printf("[FillHistogram_fluc_SR] fluctuation index should be one of 0, 1, 2, or 3\n");
        exit(1);
    }
    loader_signal.end();

    // background MC
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
    if (fluc_mode == 0) loader_bkg.FillCustomizedTH1D(bkg_MC_th1d_, { "M", "deltaE", BDT_output_1_name.c_str(), BDT_output_2_name.c_str() }, { mapping_function_plus_M });
    else if (fluc_mode == 1) loader_bkg.FillCustomizedTH1D(bkg_MC_th1d_, { "M", "deltaE", BDT_output_1_name.c_str(), BDT_output_2_name.c_str() }, { mapping_function_minus_M });
    else if (fluc_mode == 2) loader_bkg.FillCustomizedTH1D(bkg_MC_th1d_, { "M", "deltaE", BDT_output_1_name.c_str(), BDT_output_2_name.c_str() }, { mapping_function_plus_DeltaE });
    else if (fluc_mode == 3) loader_bkg.FillCustomizedTH1D(bkg_MC_th1d_, { "M", "deltaE", BDT_output_1_name.c_str(), BDT_output_2_name.c_str() }, { mapping_function_minus_DeltaE });
    else {
        printf("[FillHistogram_fluc_SR] fluctuation index should be one of 0, 1, 2, or 3\n");
        exit(1);
    }
    loader_bkg.end();


    // We do not open the box, So data_th1d is MC. We use the proper uncertainty
    // Remove the projected data error overrides after box open.
    for (int i = 1; i <= 8; i++) {
        data_th1d_->SetBinError(i, std::sqrt(data_th1d_->GetBinContent(i))); /* After box open, it should be removed! */
    }
}

std::vector<double> ABCD_method(const char* input_path_1_, const char* input_path_2_, const char* FOM_1_path_, const char* FOM_2_path_, TH1D* data_th1d_, TH1D* validation_th1d_, TH1D* data_stat_err_, std::vector<std::string> data_list_) {
    data_th1d_->Reset();
    validation_th1d_->Reset();
    ReadFOM(FOM_1_path_, &BDT_cut_1);
    ReadFOM(FOM_2_path_, &BDT_cut_2);
    std::string cut_M_1 = "((" + std::to_string(M_peak_g - 20 * M_left_sigma_g) + " < M) && (M < " + std::to_string(M_peak_g + 20 * M_right_sigma_g) + "))";
    std::string cut_deltaE_1 = "((" + std::to_string(deltaE_peak_g - 5 * deltaE_left_sigma_g) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak_g + 6 * deltaE_right_sigma_g) + "))";
    std::string cut_M_deltaE_1 = "(" + cut_M_1 + "&&" + cut_deltaE_1 + ")";

    std::string cut_M_2 = "((" + std::to_string(M_peak_g - 20 * M_left_sigma_g) + " < M) && (M < " + std::to_string(M_peak_g + 20 * M_right_sigma_g) + "))";
    std::string cut_deltaE_2 = "((" + std::to_string(deltaE_peak_g - 16 * deltaE_left_sigma_g) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak_g - 5 * deltaE_left_sigma_g) + "))";
    std::string cut_M_deltaE_2 = "(" + cut_M_2 + "&&" + cut_deltaE_2 + ")";

    std::string cut_region = cut_M_deltaE_1 + "||" + cut_M_deltaE_2;

    std::string cut_m_alpha = "(" + std::to_string(mass - M_left_cut_value) + "< extraInfo__boALP_M__bc) && (extraInfo__boALP_M__bc <" + std::to_string(mass + M_right_cut_value) + ")";

    RooRealVar validation_M("M", "M", M_peak_g - 20 * M_left_sigma_g, M_peak_g + 20 * M_right_sigma_g);
    RooRealVar validation_deltaE("deltaE", "deltaE", deltaE_peak_g - 16 * deltaE_left_sigma_g, deltaE_peak_g + 6 * deltaE_right_sigma_g);
    RooRealVar validation_BDT_1("BDT_1", "BDT_1", 0.0, 1.0);
    RooRealVar validation_BDT_2("BDT_2", "BDT_2", 0.0, 1.0);
    RooRealVar validation_weight("weight", "weight", 1.0);
    RooDataSet validation_data("validation_data", "validation_data", RooArgSet(validation_M, validation_deltaE, validation_BDT_1, validation_BDT_2, validation_weight), RooFit::WeightVar("weight"));

    Loader loader_data("tau_lfv");
    for (int i = 0; i < data_list_.size(); i++) loader_data.Load((input_path_1_ + std::string("/") + data_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), "root", data_list_.at(i).c_str());
    loader_data.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_05_ALP", { {"index", "first_muon_index"},  {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_05_ALP", { {"index", "second_muon_index"},  {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("muonID_01_ALP", { {"index", "third_muon_index"},  {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("KS0_tracking_run1", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} }); /* After box open, it should be removed! */
    loader_data.AddWeight("KS0_tracking_run2", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} }); /* After box open, it should be removed! */
    loader_data.Cut(cut_region.c_str());
    loader_data.Cut(cut_m_alpha.c_str());
    loader_data.RandomBCS();
    loader_data.IsBCSValid();
    loader_data.FillCustomizedTH1D(data_th1d_, { "M", "deltaE", BDT_output_1_name.c_str(), BDT_output_2_name.c_str() }, { mapping_function });
    // Keep the same selected events and weights; choose the validation boundary after all batches are loaded.
    loader_data.FillDataSet(&validation_data, { &validation_M, &validation_deltaE, &validation_BDT_1, &validation_BDT_2 }, { "M", "deltaE", BDT_output_1_name.c_str(), BDT_output_2_name.c_str() });
    loader_data.end();
    ABCDParameters parameters = { BDT_cut_1, BDT_cut_2, deltaE_peak_g, deltaE_left_sigma_g, deltaE_right_sigma_g, M_peak_g, M_left_sigma_g, M_right_sigma_g, sizeM };
    std::vector<double> validation_BDT_cuts = Fill_ABCD_validation(validation_th1d_, validation_data, parameters);

    // The two observed histograms use A1, B1, C1, D1, A2, B2, C2, D2 order.
    std::vector<TH1D*> histograms = { data_th1d_, validation_th1d_ };
    for (int j = 0; j < (int)histograms.size(); j++) {
        TH1D* hist = histograms.at(j);
        for (int i = 1; i <= 8; i++) {
            double yield = hist->GetBinContent(i);
            if (!std::isfinite(yield) || yield < 0.0) {
                printf("[ABCD_method] invalid yield in %s, bin %d\n", hist->GetName(), i);
                exit(1);
            }
            // We do not open the box, So data_th1d is MC. We use the proper uncertainty
            // This is a projected data error for display, not an additional likelihood constraint.
            // After box open, remove the override and use unweighted data counts.
            hist->SetBinError(i, std::sqrt(yield)); /* After box open, it should be removed! */
            printf("%s bin %d = %lf\n", hist->GetName(), i, yield);
        }
    }
    for (int i = 1; i <= 8; i++) {
        if (data_th1d_->GetBinContent(i) > 0.0) data_stat_err_->SetBinContent(i, data_th1d_->GetBinError(i) / data_th1d_->GetBinContent(i));
        else data_stat_err_->SetBinContent(i, 0.0);
    }
    // B, C and D can contain signal. Their counts are fitted with mu * signal + background, not divided into a fixed estimate.
    return validation_BDT_cuts;
}

void Write_ABCD_histograms(const std::vector<ABCDValidation>& validation_) {
    // Each unit template selects one bin of the eight-bin channel; its yield is set by the norm factors.
    std::vector<std::string> regions = { "A", "B", "C", "D" };
    for (int i = 1; i <= 2; i++) {
        for (int j = 0; j < (int)regions.size(); j++) {
            std::string name = "bkg_ABCD_unit_" + regions.at(j) + "_region" + std::to_string(i);
            TH1D* hist = new TH1D(name.c_str(), ";bin index;", 8, 0.5, 8.5);
            hist->SetBinContent(4 * (i - 1) + j + 1, 1.0);
            hist->SetBinError(4 * (i - 1) + j + 1, 0.0);
            hist->Write();
        }
        // The independent validation discrepancy abs(kappa_hat - 1) sets the up/down templates for A only.
        double discrepancy = validation_.at(i - 1).discrepancy;
        if (!std::isfinite(discrepancy)) continue;

        double down = 1.0 - discrepancy;
        if (down < 0.0) down = 0.0;
        std::string suffix = "_region" + std::to_string(i);
        TH1D* hist_p = new TH1D(("bkg_ABCD_nonclosure_p" + suffix).c_str(), ";bin index;", 8, 0.5, 8.5);
        TH1D* hist_n = new TH1D(("bkg_ABCD_nonclosure_n" + suffix).c_str(), ";bin index;", 8, 0.5, 8.5);
        hist_p->SetBinContent(4 * (i - 1) + 1, 1.0 + discrepancy);
        hist_n->SetBinContent(4 * (i - 1) + 1, down);
        hist_p->SetBinError(4 * (i - 1) + 1, 0.0);
        hist_n->SetBinError(4 * (i - 1) + 1, 0.0);
        hist_p->Write();
        hist_n->Write();
    }
}

void Add_ABCD_samples(RooStats::HistFactory::Channel& channel_, const char* filename_, TH1D* data_, const std::vector<ABCDValidation>& validation_, bool use_nonclosure_) {
    // A = beta * r, B = beta, C = nu * r, D = nu, independently for each deltaE region.
    // The signal sample supplies mu * signal in every bin, including B, C and D.
    // Do not add ActivateStatError: all eight bins already have their Poisson counting terms.
    std::vector<std::string> regions = { "A", "B", "C", "D" };
    for (int i = 1; i <= 2; i++) {
        std::string suffix = "_region" + std::to_string(i);
        double N_B = data_->GetBinContent(4 * (i - 1) + 2);
        double N_C = data_->GetBinContent(4 * (i - 1) + 3);
        double N_D = data_->GetBinContent(4 * (i - 1) + 4);
        // Positive starting values are only minimizer seeds. No events are added to the observations.
        double beta = 1.0;
        double nu = 1.0;
        double r = 1.0;
        if (N_B > 0.0) beta = N_B;
        if (N_D > 0.0) nu = N_D;
        if (N_C > 0.0) r = N_C / nu;
        double beta_max = std::max(100.0, 10.0 * beta);
        double nu_max = std::max(100.0, 10.0 * nu);
        double r_max = std::max(100.0, 10.0 * r);
        for (int j = 0; j < (int)regions.size(); j++) {
            std::string name = "bkg_" + regions.at(j) + suffix;
            RooStats::HistFactory::Sample bkg(name.c_str(), ("bkg_ABCD_unit_" + regions.at(j) + suffix).c_str(), filename_);
            if (j < 2) bkg.AddNormFactor("ABCD_beta" + suffix, beta, 0.0, beta_max);
            else bkg.AddNormFactor("ABCD_nu" + suffix, nu, 0.0, nu_max);
            if (j == 0 || j == 2) bkg.AddNormFactor("ABCD_r" + suffix, r, 0.0, r_max);
            // Keep the nominal ABCD prediction. Validation enters only through this separate nuisance.
            // Applying the validation discrepancy to the application BDT range is an extra assumption.
            if (j == 0 && use_nonclosure_ && validation_.at(i - 1).discrepancy > 0.0) {
                bkg.AddHistoSys(("ABCD_nonclosure" + suffix).c_str(), ("bkg_ABCD_nonclosure_n" + suffix).c_str(), filename_, "", ("bkg_ABCD_nonclosure_p" + suffix).c_str(), filename_, "");
            }
            bkg.SetNormalizeByTheory(false);
            channel_.AddSample(bkg);
        }
    }
}

void Set_ABCD_parameter_ranges(RooWorkspace* w_) {
    // HistFactory needs finite construction ranges. They must not act as artificial statistical constraints.
    // Remove the upper bounds in the saved workspace, retaining the physical lower bound of zero.
    std::vector<std::string> parameters = { "beta", "nu", "r" };
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
    * argv[9]: mass
    * argv[10]: lifetime
    * argv[11]: A constant
    * argv[12]: B constant
    */

    bool use_ABCD_nonclosure = false;
    /* ABCD_nonclosure is commented out for now. Uncomment the next line to include the independent validation systematic. */
    // use_ABCD_nonclosure = true;
    // Validation is signal-free and is fitted separately. Signal PCA files always contain eight bins.

    // TH1 list
    /*
    * bin:         1   2   3   4   5   6   7   8
    * region:      A1  B1  C1  D1  A2  B2  C2  D2
    * validation:  A'1 B'1 C'1 D'1 A'2 B'2 C'2 D'2
    * Signal and its systematic variations use every bin, including the control regions.
    */
    TH1D* data_th1d = new TH1D("data_th1d", ";bin index;", 8, 0.5, 8.5);
    TH1D* data_validation_th1d = new TH1D("data_validation_th1d", ";bin index;", 8, 0.5, 8.5);
    TH1D* signal_MC_th1d = new TH1D("signal_MC_th1d", ";bin index;", 8, 0.5, 8.5);
    TH1D* bkg_MC_th1d = new TH1D("bkg_MC_th1d", ";bin index;", 8, 0.5, 8.5);

    // relative error
    TH1D* data_th1d_stat_err = new TH1D("data_th1d_stat_err", ";bin index;", 8, 0.5, 8.5);
    TH1D* signal_MC_th1d_stat_err = new TH1D("signal_MC_th1d_stat_err", ";bin index;", 8, 0.5, 8.5);
    TH1D* bkg_MC_th1d_stat_err = new TH1D("bkg_MC_th1d_stat_err", ";bin index;", 8, 0.5, 8.5);

    TH1D* data_pos_M_th1d = new TH1D("data_pos_M_th1d", ";bin index;", 8, 0.5, 8.5);
    TH1D* signal_pos_M_MC_th1d = new TH1D("signal_pos_M_MC_th1d", ";bin index;", 8, 0.5, 8.5);
    TH1D* bkg_pos_M_MC_th1d = new TH1D("bkg_pos_M_MC_th1d", ";bin index;", 8, 0.5, 8.5);

    TH1D* data_neg_M_th1d = new TH1D("data_neg_M_th1d", ";bin index;", 8, 0.5, 8.5);
    TH1D* signal_neg_M_MC_th1d = new TH1D("signal_neg_M_MC_th1d", ";bin index;", 8, 0.5, 8.5);
    TH1D* bkg_neg_M_MC_th1d = new TH1D("bkg_neg_M_MC_th1d", ";bin index;", 8, 0.5, 8.5);

    TH1D* data_pos_DeltaE_th1d = new TH1D("data_pos_DeltaE_th1d", ";bin index;", 8, 0.5, 8.5);
    TH1D* signal_pos_DeltaE_MC_th1d = new TH1D("signal_pos_DeltaE_MC_th1d", ";bin index;", 8, 0.5, 8.5);
    TH1D* bkg_pos_DeltaE_MC_th1d = new TH1D("bkg_pos_DeltaE_MC_th1d", ";bin index;", 8, 0.5, 8.5);

    TH1D* data_neg_DeltaE_th1d = new TH1D("data_neg_DeltaE_th1d", ";bin index;", 8, 0.5, 8.5);
    TH1D* signal_neg_DeltaE_MC_th1d = new TH1D("signal_neg_DeltaE_MC_th1d", ";bin index;", 8, 0.5, 8.5);
    TH1D* bkg_neg_DeltaE_MC_th1d = new TH1D("bkg_neg_DeltaE_MC_th1d", ";bin index;", 8, 0.5, 8.5);

    mass = std::stod(argv[9]);
    life = std::stod(argv[10]);
    A = std::stoi(argv[11]);
    B = std::stoi(argv[12]);

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

    ReadFOM(argv[4], &BDT_cut_1);
    ReadFOM(argv[5], &BDT_cut_2);

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

    std::vector<TH1D*> signal_MC_th1d_muonID;
    std::vector<TH1D*> bkg_MC_th1d_muonID;

    std::vector<TH1D*> signal_MC_th1d_luminosity;
    std::vector<TH1D*> bkg_MC_th1d_luminosity;

    std::vector<TH1D*> signal_MC_th1d_KS0;
    std::vector<TH1D*> bkg_MC_th1d_KS0;

    // uncorrelated relative uncertainty
    TH1D* signal_MC_th1d_uncorr = new TH1D("signal_MC_th1d_uncorr", ";bin index;", 8, 0.5, 8.5);
    TH1D* bkg_MC_th1d_uncorr = new TH1D("bkg_MC_th1d_uncorr", ";bin index;", 8, 0.5, 8.5);

    std::vector<std::string> signal_list = split(argv[7], ':');
    std::vector<std::string> background_list = split(argv[8], ':');

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
    EventWeights::Register("muonID_01_ALP", muonID_01_ALP);
    EventWeights::Register("muonID_05_ALP", muonID_05_ALP);
    EventWeights::Register("KS0_tracking_run1", KS0_tracking_MC16rd_run1);
    EventWeights::Register("KS0_tracking_run2", KS0_tracking_MC16rd_run2);

    // we do not open the box, so I just use background MC
    FillHistogram(argv[1], argv[2], data_th1d, signal_MC_th1d, bkg_MC_th1d, data_th1d_stat_err, signal_MC_th1d_stat_err, bkg_MC_th1d_stat_err, background_list, signal_list, background_list);

    // muonID histogram
    ReadPCA((std::string(argv[1]) + "/muonID_PCA_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B)).c_str(), signal_MC_th1d, "muonID", &signal_MC_th1d_muonID);
    ReadPCA_remain((std::string(argv[1]) + "/muonID_PCA_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + "_remain").c_str(), signal_MC_th1d, signal_MC_th1d_uncorr);

    // luminosity histogram
    ReadPCA((std::string(argv[1]) + "/luminosity_PCA_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B)).c_str(), signal_MC_th1d, "luminosity", &signal_MC_th1d_luminosity);
    ReadPCA_remain((std::string(argv[1]) + "/luminosity_PCA_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + "_remain").c_str(), signal_MC_th1d, signal_MC_th1d_uncorr);

    // KS0 tracking histogram
    ReadPCA((std::string(argv[1]) + "/KS0_PCA_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B)).c_str(), signal_MC_th1d, "KS0", &signal_MC_th1d_KS0);
    ReadPCA_remain((std::string(argv[1]) + "/KS0_PCA_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + "_remain").c_str(), signal_MC_th1d, signal_MC_th1d_uncorr);

    // SR fluctuation
    FillHistogram_fluc_SR(argv[1], argv[2], data_pos_M_th1d, signal_pos_M_MC_th1d, bkg_pos_M_MC_th1d, background_list, signal_list, background_list, 0);
    FillHistogram_fluc_SR(argv[1], argv[2], data_neg_M_th1d, signal_neg_M_MC_th1d, bkg_neg_M_MC_th1d, background_list, signal_list, background_list, 1);
    FillHistogram_fluc_SR(argv[1], argv[2], data_pos_DeltaE_th1d, signal_pos_DeltaE_MC_th1d, bkg_pos_DeltaE_MC_th1d, background_list, signal_list, background_list, 2);
    FillHistogram_fluc_SR(argv[1], argv[2], data_neg_DeltaE_th1d, signal_neg_DeltaE_MC_th1d, bkg_neg_DeltaE_MC_th1d, background_list, signal_list, background_list, 3);

    // ABCD method
    std::vector<double> validation_BDT_cuts = ABCD_method(argv[1], argv[3], argv[4], argv[5], data_th1d, data_validation_th1d, data_th1d_stat_err, background_list);

    // Fit only the eight validation observations, with no signal or application data.
    std::vector<ABCDValidation> validation = Validate_ABCD(data_validation_th1d, (std::string(argv[6]) + "/ABCD_validation_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".txt").c_str(), { BDT_cut_1, BDT_cut_2 }, validation_BDT_cuts);
    if (use_ABCD_nonclosure) {
        for (int i = 0; i < (int)validation.size(); i++) {
            if (!std::isfinite(validation.at(i).discrepancy)) {
                printf("[ABCD_method] validation region %d has no finite non-closure systematic; see the validation report\n", i + 1);
                exit(1);
            }
        }
    }

    // print information
    printf("data:\n");
    for (int i = 1; i <= 8; i++) printf("bin %d: %lf+-%lf\n", i, data_th1d->GetBinContent(i), data_th1d->GetBinError(i));

    printf("\n");

    printf("signal:\n");
    for (int i = 1; i <= 8; i++) printf("bin %d: %lf+-%lf\n", i, signal_MC_th1d->GetBinContent(i), signal_MC_th1d->GetBinError(i));

    printf("\n");

    printf("bkg:\n");
    for (int i = 1; i <= 8; i++) printf("bin %d: %lf+-%lf\n", i, bkg_MC_th1d->GetBinContent(i), bkg_MC_th1d->GetBinError(i));

    printf("\n");

    // Save in root file
    TFile* file = new TFile((std::string(argv[6]) + "/histogram_output_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".root").c_str(), "RECREATE");

    data_th1d->Write();
    data_validation_th1d->Write();
    signal_MC_th1d->Write();
    bkg_MC_th1d->Write();
    Write_ABCD_histograms(validation);

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

    for (int i = 0; i < signal_MC_th1d_KS0.size(); i++) signal_MC_th1d_KS0.at(i)->Write();
    for (int i = 0; i < bkg_MC_th1d_KS0.size(); i++) bkg_MC_th1d_KS0.at(i)->Write();

    signal_MC_th1d_uncorr->Write();
    bkg_MC_th1d_uncorr->Write();

    file->Close();


    // make workspace
    RooStats::HistFactory::Measurement meas(("my_measurement_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B)).c_str(), ("my_measurement_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B)).c_str());
    meas.SetOutputFilePrefix((argv[1] + std::string("/") + "my_measurement_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B)).c_str());

    // setting measurement
    meas.SetPOI("mu");
    meas.SetLumi(1.0);
    meas.AddConstantParam("Lumi");

    // define channels
    RooStats::HistFactory::Channel channel_Belle_II("Belle_II");
    channel_Belle_II.SetStatErrorConfig(1e-5, "Poisson");

    // fill channels
    channel_Belle_II.SetData("data_th1d", (std::string(argv[6]) + "/histogram_output_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".root").c_str());

    RooStats::HistFactory::Sample signal_Belle_II("signal_Belle_II", "signal_MC_th1d", (std::string(argv[6]) + "/histogram_output_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".root").c_str());
    signal_Belle_II.ActivateStatError("signal_MC_th1d_stat_err", (std::string(argv[6]) + "/histogram_output_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".root").c_str(), "");
    signal_Belle_II.AddNormFactor("mu", 1.0, 0.0, 1200.0);
    signal_Belle_II.AddOverallSys("tracking_efficiency", 1.0 - (track_rel_uncertainty / 100.0) * 3, 1.0 + (track_rel_uncertainty / 100.0) * 3);
    signal_Belle_II.AddHistoSys("M_resolution", "signal_neg_M_MC_th1d", (std::string(argv[6]) + "/histogram_output_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".root").c_str(), "", "signal_pos_M_MC_th1d", (std::string(argv[6]) + "/histogram_output_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".root").c_str(), "");
    signal_Belle_II.AddHistoSys("DeltaE_resolution", "signal_neg_DeltaE_MC_th1d", (std::string(argv[6]) + "/histogram_output_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".root").c_str(), "", "signal_pos_DeltaE_MC_th1d", (std::string(argv[6]) + "/histogram_output_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".root").c_str(), "");
    signal_Belle_II.AddOverallSys("cross_section", 1.0 - tau_crosssection_4S_reluncertainty, 1.0 + tau_crosssection_4S_reluncertainty);
    for (int i = 0; i < signal_MC_th1d_muonID.size() / 2; i++) signal_Belle_II.AddHistoSys(("muonID_" + std::to_string(i)).c_str(), ("signal_hist_muonID_n_" + std::to_string(i)).c_str(), (std::string(argv[6]) + "/histogram_output_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".root").c_str(), "", ("signal_hist_muonID_p_" + std::to_string(i)).c_str(), (std::string(argv[6]) + "/histogram_output_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".root").c_str(), "");
    for (int i = 0; i < signal_MC_th1d_luminosity.size() / 2; i++) signal_Belle_II.AddHistoSys(("luminosity_" + std::to_string(i)).c_str(), ("signal_hist_luminosity_n_" + std::to_string(i)).c_str(), (std::string(argv[6]) + "/histogram_output_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".root").c_str(), "", ("signal_hist_luminosity_p_" + std::to_string(i)).c_str(), (std::string(argv[6]) + "/histogram_output_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".root").c_str(), "");
    for (int i = 0; i < signal_MC_th1d_KS0.size() / 2; i++) signal_Belle_II.AddHistoSys(("KS0_" + std::to_string(i)).c_str(), ("signal_hist_KS0_n_" + std::to_string(i)).c_str(), (std::string(argv[6]) + "/histogram_output_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".root").c_str(), "", ("signal_hist_KS0_p_" + std::to_string(i)).c_str(), (std::string(argv[6]) + "/histogram_output_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".root").c_str(), "");
    signal_Belle_II.AddShapeSys("uncorrelated_error", RooStats::HistFactory::Constraint::Gaussian, "signal_MC_th1d_uncorr", (std::string(argv[6]) + "/histogram_output_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".root").c_str(), "");
    signal_Belle_II.SetNormalizeByTheory(false);

    channel_Belle_II.AddSample(signal_Belle_II);
    Add_ABCD_samples(channel_Belle_II, (std::string(argv[6]) + "/histogram_output_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".root").c_str(), data_th1d, validation, use_ABCD_nonclosure);

    // add channel to measurement
    meas.AddChannel(channel_Belle_II);
    meas.CollectHistograms();

    RooWorkspace* w;
    w = RooStats::HistFactory::MakeModelAndMeasurementFast(meas);
    Set_ABCD_parameter_ranges(w);

    w->Print();
    w->writeToFile((std::string(argv[6]) + "/workspace_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) +  ".root").c_str());

    meas.PrintXML(("my_measurement_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B)).c_str());

    return 0;
}
