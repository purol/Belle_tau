#include <stdio.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <cmath>

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TROOT.h>
#include <TLatex.h>

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "functions.h"

std::string toStringWithPrecision(double value, int precision) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

void DrawValidTH2D(TH2D* histogram_, TH2D* valid_ = nullptr, const std::vector<TH2D*>& range_histograms_ = {}) {
    TH2D* plot = static_cast<TH2D*>(histogram_->Clone((std::string(histogram_->GetName()) + "_plot").c_str()));
    plot->SetDirectory(nullptr);

    // get range from valid bins
    double min_value = 0.0;
    double max_value = 0.0;
    bool found_valid = false;
    std::vector<int> invalid_bins;
    for (int ix = 1; ix <= histogram_->GetNbinsX(); ix++) {
        for (int iy = 1; iy <= histogram_->GetNbinsY(); iy++) {
            const int bin = histogram_->GetBin(ix, iy);
            const double value = histogram_->GetBinContent(bin);
            const bool valid = std::isfinite(value) && value >= 0.0 && (valid_ == nullptr || valid_->GetBinContent(bin) > 0.5);
            if (!valid) {
                invalid_bins.push_back(bin);
                continue;
            }

            if (!found_valid) {
                min_value = value;
                max_value = value;
                found_valid = true;
            }
            else {
                if (value < min_value) min_value = value;
                if (value > max_value) max_value = value;
            }
        }
    }

    // get common range for comparison
    for (int i = 0; i < range_histograms_.size(); i++) {
        TH2D* histogram = range_histograms_.at(i);
        if (histogram == histogram_) continue;
        for (int ix = 1; ix <= histogram->GetNbinsX(); ix++) {
            for (int iy = 1; iy <= histogram->GetNbinsY(); iy++) {
                const double value = histogram->GetBinContent(ix, iy);
                if (!std::isfinite(value) || value < 0.0) continue;

                if (!found_valid) {
                    min_value = value;
                    max_value = value;
                    found_valid = true;
                }
                else {
                    if (value < min_value) min_value = value;
                    if (value > max_value) max_value = value;
                }
            }
        }
    }

    // include valid zero bins and allow a constant value
    if (found_valid) {
        double margin = 0.05 * (max_value - min_value);
        if (margin == 0.0) margin = 0.01 * (std::fabs(max_value) + 1.0);
        min_value -= margin;
        max_value += margin;
    }
    else {
        min_value = 0.0;
        max_value = 1.0;
    }

    // COLZ leaves bins below the minimum blank
    for (int i = 0; i < invalid_bins.size(); i++) {
        plot->SetBinContent(invalid_bins.at(i), min_value - (max_value - min_value));
        plot->SetBinError(invalid_bins.at(i), 0.0);
    }
    plot->SetMinimum(min_value);
    plot->SetMaximum(max_value);
    plot->DrawCopy("COLZ");
    delete plot;
}

int main(int argc, char* argv[]) {
    /*
    * argv[1]: input1 path1 (MC)
    * argv[2]: input1 path2 (MC)
    * argv[3]: input2 path1 (data)
    * argv[4]: input2 path2 (data)
    * argv[5]: output path
    * argv[6]: output name
    * argv[7]: MC list (separated by colon)
    * argv[8]: data list (separated by colon)
    * argv[9]: MC lable
    * argv[10]: data lable
    * argv[11]: used variables (separated by colon)
    * argv[12]: binnings (separated by colon. min1:max1:numbin1:min2:max2:numbin2)
    */

    EventWeights::Register("MC_weight", MC_weight);
    EventWeights::Register("KS0_tracking_run1", KS0_tracking_MC16rd_run1);
    EventWeights::Register("KS0_tracking_run2", KS0_tracking_MC16rd_run2);
    EventWeights::Register("pionID_09_ALP", pionID_09_ALP);

    std::vector<std::string> MC_list = split(argv[7], ':');
    std::vector<std::string> data_list = split(argv[8], ':');

    std::vector<std::string> used_variables = split(argv[11], ':');
    std::vector<std::string> binnings = split(argv[12], ':');
    if (used_variables.size() != 2) {
        printf("There should be two variables.\n");
        return 1;
    }
    if (used_variables.size() * 3 != binnings.size()) {
        printf("The number of variables and binnings should match.\n");
        return 1;
    }

    std::vector<double> binning_values;
    try {
        for (int i = 0; i < binnings.size(); i++) {
            double value = std::stod(binnings.at(i));
            binning_values.push_back(value);
        }
    }
    catch (const std::exception& error) {
        printf("invalid binning: %s\n", error.what());
        return 1;
    }

    const double min1 = binning_values.at(0);
    const double max1 = binning_values.at(1);
    const int nbins1 = static_cast<int>(binning_values.at(2));

    const double min2 = binning_values.at(3);
    const double max2 = binning_values.at(4);
    const int nbins2 = static_cast<int>(binning_values.at(5));

    // define bin edges
    std::vector<double> bins1(nbins1 + 1);
    std::vector<double> bins2(nbins2 + 1);
    for (int i = 0; i <= nbins1; i++) bins1.at(i) = min1 + (max1 - min1) * i / nbins1;
    for (int i = 0; i <= nbins2; i++) bins2.at(i) = min2 + (max2 - min2) * i / nbins2;

    const std::string distance_variable = "extraInfo__boALP_distance__bc";
    std::vector<double> distance_bins = { 0.0, 5.0, 15.0, 30.0, 1000.0 };
    const int nbins3 = static_cast<int>(distance_bins.size()) - 1;

    std::string histogram_title = ";" + used_variables.at(0) + ";" + used_variables.at(1) + ";" + distance_variable;

    // ECL|CDC
    TH3D* hist_00_MC = new TH3D("hist_00_MC", histogram_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data(), nbins3, distance_bins.data());
    TH3D* hist_01_MC = new TH3D("hist_01_MC", histogram_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data(), nbins3, distance_bins.data());
    TH3D* hist_10_MC = new TH3D("hist_10_MC", histogram_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data(), nbins3, distance_bins.data());
    TH3D* hist_11_MC = new TH3D("hist_11_MC", histogram_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data(), nbins3, distance_bins.data());

    TH3D* hist_01_data = new TH3D("hist_01_data", histogram_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data(), nbins3, distance_bins.data());
    TH3D* hist_10_data = new TH3D("hist_10_data", histogram_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data(), nbins3, distance_bins.data());
    TH3D* hist_11_data = new TH3D("hist_11_data", histogram_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data(), nbins3, distance_bins.data());

    // efficiency matrix
    TH3D* eff_trigger_MC = new TH3D("eff_trigger_MC", histogram_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data(), nbins3, distance_bins.data());
    TH3D* eff_trigger_1ineff_MC = new TH3D("eff_trigger_1ineff_MC", histogram_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data(), nbins3, distance_bins.data());
    TH3D* eff_trigger_1ineff_data = new TH3D("eff_trigger_1ineff_data", histogram_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data(), nbins3, distance_bins.data());

    // data/MC ratio
    TH3D* weight_trigger = new TH3D("weight_trigger", histogram_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data(), nbins3, distance_bins.data());
    TH3D* weight_syst_err = new TH3D("weight_syst_err", histogram_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data(), nbins3, distance_bins.data());
    TH3D* valid_weight = new TH3D("valid_weight", histogram_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data(), nbins3, distance_bins.data());

    // MC closure
    TH3D* closure_MC = new TH3D("closure_MC", histogram_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data(), nbins3, distance_bins.data());
    TH3D* valid_method = new TH3D("valid_method", histogram_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data(), nbins3, distance_bins.data());

    // category yields and sum of squared weights
    std::vector<TH3D*> category_histograms = { hist_00_MC, hist_01_MC, hist_10_MC, hist_11_MC, hist_01_data, hist_10_data, hist_11_data };
    for (int i = 0; i < category_histograms.size(); i++) category_histograms.at(i)->Sumw2();

    // define conditions
    std::string ECL_pass = "(0.5 < ifNANgiveX__boL1PSNM__bohie__bc__cm__mi1__bc) || (0.5 < ifNANgiveX__boL1PSNM__bolml6__bc__cm__mi1__bc) || (0.5 < ifNANgiveX__boL1PSNM__bolml7__bc__cm__mi1__bc) || (0.5 < ifNANgiveX__boL1PSNM__bolml8__bc__cm__mi1__bc) || (0.5 < ifNANgiveX__boL1PSNM__bolml9__bc__cm__mi1__bc) || (0.5 < ifNANgiveX__boL1PSNM__bolml10__bc__cm__mi1__bc) || (0.5 < ifNANgiveX__boL1PSNM__bolml12__bc__cm__mi1__bc)";
    std::string ECL_fail = "(0.5 > ifNANgiveX__boL1PSNM__bohie__bc__cm__mi1__bc) && (0.5 > ifNANgiveX__boL1PSNM__bolml6__bc__cm__mi1__bc) && (0.5 > ifNANgiveX__boL1PSNM__bolml7__bc__cm__mi1__bc) && (0.5 > ifNANgiveX__boL1PSNM__bolml8__bc__cm__mi1__bc) && (0.5 > ifNANgiveX__boL1PSNM__bolml9__bc__cm__mi1__bc) && (0.5 > ifNANgiveX__boL1PSNM__bolml10__bc__cm__mi1__bc) && (0.5 > ifNANgiveX__boL1PSNM__bolml12__bc__cm__mi1__bc)";
    std::string CDC_pass = "(0.5 < ifNANgiveX__boL1PSNM__boffy__bc__cm__mi1__bc) || (0.5 < ifNANgiveX__boL1PSNM__bofyo__bc__cm__mi1__bc) || (0.5 < ifNANgiveX__boL1PSNM__bostt__bc__cm__mi1__bc)";
    std::string CDC_fail = "(0.5 > ifNANgiveX__boL1PSNM__boffy__bc__cm__mi1__bc) && (0.5 > ifNANgiveX__boL1PSNM__bofyo__bc__cm__mi1__bc) && (0.5 > ifNANgiveX__boL1PSNM__bostt__bc__cm__mi1__bc)";

    // MC
    Loader loader_00_MC("tau_lfv");
    for (int i = 0; i < MC_list.size(); i++) loader_00_MC.Load((std::string(argv[1]) + "/" + MC_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", MC_list.at(i).c_str());
    loader_00_MC.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_00_MC.AddWeight("pionID_09_ALP", { {"sample", "MySampleType"}, {"index", "first_muon_index"},  {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
    loader_00_MC.AddWeight("pionID_09_ALP", { {"sample", "MySampleType"}, {"index", "second_muon_index"},  {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
    loader_00_MC.AddWeight("pionID_09_ALP", { {"sample", "MySampleType"}, {"index", "third_muon_index"},  {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
    loader_00_MC.AddWeight("KS0_tracking_run1", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
    loader_00_MC.AddWeight("KS0_tracking_run2", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
    loader_00_MC.Cut(ECL_fail.c_str());
    loader_00_MC.Cut(CDC_fail.c_str());
    loader_00_MC.FillTH3D(hist_00_MC, used_variables.at(0).c_str(), used_variables.at(1).c_str(), distance_variable.c_str());
    loader_00_MC.end();

    Loader loader_01_MC("tau_lfv");
    for (int i = 0; i < MC_list.size(); i++) loader_01_MC.Load((std::string(argv[1]) + "/" + MC_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", MC_list.at(i).c_str());
    loader_01_MC.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_01_MC.AddWeight("pionID_09_ALP", { {"sample", "MySampleType"}, {"index", "first_muon_index"},  {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
    loader_01_MC.AddWeight("pionID_09_ALP", { {"sample", "MySampleType"}, {"index", "second_muon_index"},  {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
    loader_01_MC.AddWeight("pionID_09_ALP", { {"sample", "MySampleType"}, {"index", "third_muon_index"},  {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
    loader_01_MC.AddWeight("KS0_tracking_run1", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
    loader_01_MC.AddWeight("KS0_tracking_run2", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
    loader_01_MC.Cut(ECL_fail.c_str());
    loader_01_MC.Cut(CDC_pass.c_str());
    loader_01_MC.FillTH3D(hist_01_MC, used_variables.at(0).c_str(), used_variables.at(1).c_str(), distance_variable.c_str());
    loader_01_MC.end();

    Loader loader_10_MC("tau_lfv");
    for (int i = 0; i < MC_list.size(); i++) loader_10_MC.Load((std::string(argv[1]) + "/" + MC_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", MC_list.at(i).c_str());
    loader_10_MC.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_10_MC.AddWeight("pionID_09_ALP", { {"sample", "MySampleType"}, {"index", "first_muon_index"},  {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
    loader_10_MC.AddWeight("pionID_09_ALP", { {"sample", "MySampleType"}, {"index", "second_muon_index"},  {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
    loader_10_MC.AddWeight("pionID_09_ALP", { {"sample", "MySampleType"}, {"index", "third_muon_index"},  {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
    loader_10_MC.AddWeight("KS0_tracking_run1", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
    loader_10_MC.AddWeight("KS0_tracking_run2", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
    loader_10_MC.Cut(ECL_pass.c_str());
    loader_10_MC.Cut(CDC_fail.c_str());
    loader_10_MC.FillTH3D(hist_10_MC, used_variables.at(0).c_str(), used_variables.at(1).c_str(), distance_variable.c_str());
    loader_10_MC.end();

    Loader loader_11_MC("tau_lfv");
    for (int i = 0; i < MC_list.size(); i++) loader_11_MC.Load((std::string(argv[1]) + "/" + MC_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", MC_list.at(i).c_str());
    loader_11_MC.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_11_MC.AddWeight("pionID_09_ALP", { {"sample", "MySampleType"}, {"index", "first_muon_index"},  {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
    loader_11_MC.AddWeight("pionID_09_ALP", { {"sample", "MySampleType"}, {"index", "second_muon_index"},  {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
    loader_11_MC.AddWeight("pionID_09_ALP", { {"sample", "MySampleType"}, {"index", "third_muon_index"},  {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
    loader_11_MC.AddWeight("KS0_tracking_run1", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
    loader_11_MC.AddWeight("KS0_tracking_run2", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
    loader_11_MC.Cut(ECL_pass.c_str());
    loader_11_MC.Cut(CDC_pass.c_str());
    loader_11_MC.FillTH3D(hist_11_MC, used_variables.at(0).c_str(), used_variables.at(1).c_str(), distance_variable.c_str());
    loader_11_MC.end();

    // data
    Loader loader_01_data("tau_lfv");
    for (int i = 0; i < data_list.size(); i++) loader_01_data.Load((std::string(argv[3]) + "/" + data_list.at(i) + "/" + std::string(argv[4])).c_str(), "root", data_list.at(i).c_str());
    loader_01_data.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_01_data.Cut(ECL_fail.c_str());
    loader_01_data.Cut(CDC_pass.c_str());
    loader_01_data.FillTH3D(hist_01_data, used_variables.at(0).c_str(), used_variables.at(1).c_str(), distance_variable.c_str());
    loader_01_data.end();

    Loader loader_10_data("tau_lfv");
    for (int i = 0; i < data_list.size(); i++) loader_10_data.Load((std::string(argv[3]) + "/" + data_list.at(i) + "/" + std::string(argv[4])).c_str(), "root", data_list.at(i).c_str());
    loader_10_data.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_10_data.Cut(ECL_pass.c_str());
    loader_10_data.Cut(CDC_fail.c_str());
    loader_10_data.FillTH3D(hist_10_data, used_variables.at(0).c_str(), used_variables.at(1).c_str(), distance_variable.c_str());
    loader_10_data.end();

    Loader loader_11_data("tau_lfv");
    for (int i = 0; i < data_list.size(); i++) loader_11_data.Load((std::string(argv[3]) + "/" + data_list.at(i) + "/" + std::string(argv[4])).c_str(), "root", data_list.at(i).c_str());
    loader_11_data.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_11_data.Cut(ECL_pass.c_str());
    loader_11_data.Cut(CDC_pass.c_str());
    loader_11_data.FillTH3D(hist_11_data, used_variables.at(0).c_str(), used_variables.at(1).c_str(), distance_variable.c_str());
    loader_11_data.end();

    // get trigger eff for MC
    for (int ix = 1; ix <= nbins1; ++ix) {
        for (int iy = 1; iy <= nbins2; ++iy) {
            for (int iz = 1; iz <= nbins3; ++iz) {
                const int bin = eff_trigger_MC->GetBin(ix, iy, iz);

                const double pass = hist_01_MC->GetBinContent(bin) + hist_10_MC->GetBinContent(bin) + hist_11_MC->GetBinContent(bin);

                const double fail = hist_00_MC->GetBinContent(bin);
                const double total = pass + fail;

                if (!(pass >= 0.0 && fail >= 0.0 && total > 0.0) || !std::isfinite(total)) {
                    // just set eff = -1

                    eff_trigger_MC->SetBinContent(bin, -1.0);
                    eff_trigger_MC->SetBinError(bin, 0.0);
                    continue;
                }

                const double var_pass = std::pow(hist_01_MC->GetBinError(bin), 2) + std::pow(hist_10_MC->GetBinError(bin), 2) + std::pow(hist_11_MC->GetBinError(bin), 2);

                const double var_fail = std::pow(hist_00_MC->GetBinError(bin), 2);

                const double eff = pass / total;
                const double var_eff = ((1.0 - eff) * (1.0 - eff) * var_pass + eff * eff * var_fail) / (total * total);
                if (!std::isfinite(var_eff)) {
                    eff_trigger_MC->SetBinContent(bin, -1.0);
                    eff_trigger_MC->SetBinError(bin, 0.0);
                    continue;
                }

                eff_trigger_MC->SetBinContent(bin, eff);
                eff_trigger_MC->SetBinError(bin, std::sqrt(var_eff));
            }
        }
    }

    // get trigger correction and MC closure
    std::vector<TH3D*> histograms_01 = { hist_01_MC, hist_01_data };
    std::vector<TH3D*> histograms_10 = { hist_10_MC, hist_10_data };
    std::vector<TH3D*> histograms_11 = { hist_11_MC, hist_11_data };
    std::vector<TH3D*> efficiencies = { eff_trigger_1ineff_MC, eff_trigger_1ineff_data };

    for (int ix = 1; ix <= nbins1; ++ix) {
        for (int iy = 1; iy <= nbins2; ++iy) {
            for (int iz = 1; iz <= nbins3; ++iz) {
                const int bin = eff_trigger_MC->GetBin(ix, iy, iz);
                std::vector<double> derivatives_MC(3, 0.0);

                // get 1-inefficiency estimates
                for (int i = 0; i < efficiencies.size(); i++) {
                    efficiencies.at(i)->SetBinContent(bin, -1.0);
                    efficiencies.at(i)->SetBinError(bin, 0.0);

                    const double n_01 = histograms_01.at(i)->GetBinContent(bin);
                    const double n_10 = histograms_10.at(i)->GetBinContent(bin);
                    const double n_11 = histograms_11.at(i)->GetBinContent(bin);
                    const double total_E = n_10 + n_11;
                    const double total_C = n_01 + n_11;

                    // just set eff = -1 for invalid bins
                    if (!(n_01 >= 0.0 && n_10 >= 0.0 && n_11 >= 0.0 && total_E > 0.0 && total_C > 0.0)) continue;
                    if (!std::isfinite(total_E) || !std::isfinite(total_C)) continue;

                    const double var_01 = std::pow(histograms_01.at(i)->GetBinError(bin), 2);
                    const double var_10 = std::pow(histograms_10.at(i)->GetBinError(bin), 2);
                    const double var_11 = std::pow(histograms_11.at(i)->GetBinError(bin), 2);
                    const double eff_E = n_11 / total_C;
                    const double eff_C = n_11 / total_E;
                    const double eff = 1.0 - (1.0 - eff_E) * (1.0 - eff_C);

                    // use category yields to include the common N11 uncertainty
                    const double derivative_01 = -(1.0 - eff_C) * eff_E / total_C;
                    const double derivative_10 = -(1.0 - eff_E) * eff_C / total_E;
                    const double derivative_11 = (1.0 - eff_E) * (1.0 - eff_C) * (1.0 / total_C + 1.0 / total_E);
                    const double var_eff = derivative_01 * derivative_01 * var_01 + derivative_10 * derivative_10 * var_10 + derivative_11 * derivative_11 * var_11;
                    if (!std::isfinite(eff) || !std::isfinite(var_eff)) continue;

                    efficiencies.at(i)->SetBinContent(bin, eff);
                    efficiencies.at(i)->SetBinError(bin, std::sqrt(var_eff));
                    if (i == 0) derivatives_MC = { derivative_01, derivative_10, derivative_11 };
                }

                const double eff_MC = eff_trigger_1ineff_MC->GetBinContent(bin);
                const double eff_data = eff_trigger_1ineff_data->GetBinContent(bin);
                const double eff_true_MC = eff_trigger_MC->GetBinContent(bin);

                // get MC closure
                closure_MC->SetBinContent(bin, -1.0);
                closure_MC->SetBinError(bin, 0.0);
                if (eff_MC > 0.0 && eff_true_MC >= 0.0) {
                    const double total = hist_00_MC->GetBinContent(bin) + hist_01_MC->GetBinContent(bin) + hist_10_MC->GetBinContent(bin) + hist_11_MC->GetBinContent(bin);
                    const double closure = eff_true_MC / eff_MC;
                    const double var_00 = std::pow(hist_00_MC->GetBinError(bin), 2);
                    const double var_01 = std::pow(hist_01_MC->GetBinError(bin), 2);
                    const double var_10 = std::pow(hist_10_MC->GetBinError(bin), 2);
                    const double var_11 = std::pow(hist_11_MC->GetBinError(bin), 2);

                    // true and estimated efficiencies use the same MC events
                    const double derivative_00 = -eff_true_MC / (total * eff_MC);
                    const double derivative_01 = ((1.0 - eff_true_MC) / total - closure * derivatives_MC.at(0)) / eff_MC;
                    const double derivative_10 = ((1.0 - eff_true_MC) / total - closure * derivatives_MC.at(1)) / eff_MC;
                    const double derivative_11 = ((1.0 - eff_true_MC) / total - closure * derivatives_MC.at(2)) / eff_MC;
                    const double var_closure = derivative_00 * derivative_00 * var_00 + derivative_01 * derivative_01 * var_01 + derivative_10 * derivative_10 * var_10 + derivative_11 * derivative_11 * var_11;
                    if (std::isfinite(closure) && std::isfinite(var_closure)) {
                        closure_MC->SetBinContent(bin, closure);
                        closure_MC->SetBinError(bin, std::sqrt(var_closure));
                    }
                }

                // just set weight = 1 for invalid bins
                weight_trigger->SetBinContent(bin, 1.0);
                weight_trigger->SetBinError(bin, 0.0);
                weight_syst_err->SetBinContent(bin, 0.0);
                weight_syst_err->SetBinError(bin, 0.0);
                valid_weight->SetBinContent(bin, 0.0);
                valid_method->SetBinContent(bin, 0.0);
                if (!(eff_MC > 0.0 && eff_data >= 0.0)) continue;

                // get data/MC ratio
                const double weight = eff_data / eff_MC;
                const double var_weight = (std::pow(eff_trigger_1ineff_data->GetBinError(bin), 2) + weight * weight * std::pow(eff_trigger_1ineff_MC->GetBinError(bin), 2)) / (eff_MC * eff_MC);
                if (!std::isfinite(weight) || !std::isfinite(var_weight)) continue;

                weight_trigger->SetBinContent(bin, weight);
                weight_trigger->SetBinError(bin, std::sqrt(var_weight));
                valid_weight->SetBinContent(bin, 1.0);

                // get absolute systematic error from MC closure
                const double closure = closure_MC->GetBinContent(bin);
                if (closure < 0.0) continue;
                const double syst_err = std::fabs(weight * (closure - 1.0));
                if (!std::isfinite(syst_err)) continue;

                weight_syst_err->SetBinContent(bin, syst_err);
                valid_method->SetBinContent(bin, 1.0);
            }
        }
    }

    // MC16rd: MySampleType = 4
    const double sample_min = 3.5;
    const double sample_max = 4.5;

    // print result
    FILE* fp = fopen((std::string(argv[5]) + "/" + std::string(argv[6]) + ".csv").c_str(), "w");
    if (fp == nullptr) {
        printf("Cannot open output file.\n");
        return 1;
    }
    fprintf(fp, "sample_min,sample_max,weight,weight_stat_err,weight_syst_err,valid_weight,valid_method,%s_min,%s_max,%s_min,%s_max,%s_min,%s_max", used_variables.at(0).c_str(), used_variables.at(0).c_str(), used_variables.at(1).c_str(), used_variables.at(1).c_str(), distance_variable.c_str(), distance_variable.c_str());
    for (int ix = 1; ix <= nbins1; ++ix) {
        for (int iy = 1; iy <= nbins2; ++iy) {
            for (int iz = 1; iz <= nbins3; ++iz) {
                const int bin = eff_trigger_MC->GetBin(ix, iy, iz);
                fprintf(fp, "\n%.17g,%.17g,%.17g,%.17g,%.17g,%.0f,%.0f", sample_min, sample_max, weight_trigger->GetBinContent(bin), weight_trigger->GetBinError(bin), weight_syst_err->GetBinContent(bin), valid_weight->GetBinContent(bin), valid_method->GetBinContent(bin));
                fprintf(fp, ",%.17g,%.17g", eff_trigger_MC->GetXaxis()->GetBinLowEdge(ix), eff_trigger_MC->GetXaxis()->GetBinUpEdge(ix));
                fprintf(fp, ",%.17g,%.17g", eff_trigger_MC->GetYaxis()->GetBinLowEdge(iy), eff_trigger_MC->GetYaxis()->GetBinUpEdge(iy));
                fprintf(fp, ",%.17g,%.17g", eff_trigger_MC->GetZaxis()->GetBinLowEdge(iz), eff_trigger_MC->GetZaxis()->GetBinUpEdge(iz));
            }
        }
    }
    fclose(fp);

    // save histograms
    TFile* file = new TFile((std::string(argv[5]) + "/" + std::string(argv[6]) + ".root").c_str(), "RECREATE");
    if (file->IsZombie()) {
        printf("Cannot open histogram output file.\n");
        delete file;
        return 1;
    }
    for (int i = 0; i < category_histograms.size(); i++) category_histograms.at(i)->Write();
    eff_trigger_MC->Write();
    eff_trigger_1ineff_MC->Write();
    eff_trigger_1ineff_data->Write();
    weight_trigger->Write();
    weight_syst_err->Write();
    closure_MC->Write();
    valid_weight->Write();
    valid_method->Write();
    file->Close();
    delete file;

    // plot result
    std::string plot_name = std::string(argv[6]);
    const std::size_t extension_pos = plot_name.find_last_of('.');
    if (extension_pos != std::string::npos) plot_name = plot_name.substr(0, extension_pos);

    for (int iz = 1; iz <= nbins3; iz++) {
        const std::string distance_low = toStringWithPrecision(distance_bins.at(iz - 1), 0);
        const std::string distance_high = toStringWithPrecision(distance_bins.at(iz), 0);
        const std::string distance_name = "_distance_" + distance_low + "_" + distance_high;
        const std::string slice_title = distance_low + " <= KS0 distance < " + distance_high + ";" + used_variables.at(0) + ";" + used_variables.at(1);

        // get distance slice
        TH2D* eff_trigger_MC_slice = new TH2D(("eff_trigger_MC" + distance_name).c_str(), slice_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data());
        TH2D* eff_trigger_1ineff_MC_slice = new TH2D(("eff_trigger_1ineff_MC" + distance_name).c_str(), slice_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data());
        TH2D* eff_trigger_1ineff_data_slice = new TH2D(("eff_trigger_1ineff_data" + distance_name).c_str(), slice_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data());
        TH2D* weight_trigger_slice = new TH2D(("weight_trigger" + distance_name).c_str(), slice_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data());
        TH2D* closure_MC_slice = new TH2D(("closure_MC" + distance_name).c_str(), slice_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data());
        TH2D* weight_syst_err_slice = new TH2D(("weight_syst_err" + distance_name).c_str(), slice_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data());
        TH2D* valid_weight_slice = new TH2D(("valid_weight" + distance_name).c_str(), slice_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data());
        TH2D* valid_method_slice = new TH2D(("valid_method" + distance_name).c_str(), slice_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data());
        for (int ix = 1; ix <= nbins1; ix++) {
            for (int iy = 1; iy <= nbins2; iy++) {
                const int bin = eff_trigger_MC->GetBin(ix, iy, iz);
                const int slice_bin = eff_trigger_MC_slice->GetBin(ix, iy);
                eff_trigger_MC_slice->SetBinContent(slice_bin, eff_trigger_MC->GetBinContent(bin));
                eff_trigger_MC_slice->SetBinError(slice_bin, eff_trigger_MC->GetBinError(bin));
                eff_trigger_1ineff_MC_slice->SetBinContent(slice_bin, eff_trigger_1ineff_MC->GetBinContent(bin));
                eff_trigger_1ineff_MC_slice->SetBinError(slice_bin, eff_trigger_1ineff_MC->GetBinError(bin));
                eff_trigger_1ineff_data_slice->SetBinContent(slice_bin, eff_trigger_1ineff_data->GetBinContent(bin));
                eff_trigger_1ineff_data_slice->SetBinError(slice_bin, eff_trigger_1ineff_data->GetBinError(bin));
                weight_trigger_slice->SetBinContent(slice_bin, weight_trigger->GetBinContent(bin));
                weight_trigger_slice->SetBinError(slice_bin, weight_trigger->GetBinError(bin));
                closure_MC_slice->SetBinContent(slice_bin, closure_MC->GetBinContent(bin));
                closure_MC_slice->SetBinError(slice_bin, closure_MC->GetBinError(bin));
                weight_syst_err_slice->SetBinContent(slice_bin, weight_syst_err->GetBinContent(bin));
                weight_syst_err_slice->SetBinError(slice_bin, weight_syst_err->GetBinError(bin));
                valid_weight_slice->SetBinContent(slice_bin, valid_weight->GetBinContent(bin));
                valid_method_slice->SetBinContent(slice_bin, valid_method->GetBinContent(bin));
            }
        }

        // common color range for trigger efficiencies
        std::vector<TH2D*> eff_plot_histograms = { eff_trigger_MC_slice, eff_trigger_1ineff_MC_slice, eff_trigger_1ineff_data_slice };

        // trigger eff for MC
        TCanvas* c_eff_trigger_MC = new TCanvas("c_eff_trigger_MC", "", 900, 800);
        c_eff_trigger_MC->cd();
        c_eff_trigger_MC->SetRightMargin(0.18);
        c_eff_trigger_MC->SetFillColor(0);
        c_eff_trigger_MC->SetFrameFillColor(0);
        eff_trigger_MC_slice->SetStats(false);
        eff_trigger_MC_slice->GetZaxis()->SetTitle("Trigger efficiency");
        DrawValidTH2D(eff_trigger_MC_slice, nullptr, eff_plot_histograms);
        c_eff_trigger_MC->SaveAs((std::string(argv[5]) + "/" + plot_name + "_eff_trigger_MC" + distance_name + ".png").c_str());
        delete c_eff_trigger_MC;

        // 1-inefficiency estimate for MC
        TCanvas* c_eff_trigger_1ineff_MC = new TCanvas("c_eff_trigger_1ineff_MC", "", 900, 800);
        c_eff_trigger_1ineff_MC->cd();
        c_eff_trigger_1ineff_MC->SetRightMargin(0.18);
        c_eff_trigger_1ineff_MC->SetFillColor(0);
        c_eff_trigger_1ineff_MC->SetFrameFillColor(0);
        eff_trigger_1ineff_MC_slice->SetStats(false);
        eff_trigger_1ineff_MC_slice->GetZaxis()->SetTitle("Trigger efficiency (1-inefficiency, MC)");
        DrawValidTH2D(eff_trigger_1ineff_MC_slice, nullptr, eff_plot_histograms);
        c_eff_trigger_1ineff_MC->SaveAs((std::string(argv[5]) + "/" + plot_name + "_eff_trigger_1ineff_MC" + distance_name + ".png").c_str());
        delete c_eff_trigger_1ineff_MC;

        // 1-inefficiency estimate for data
        TCanvas* c_eff_trigger_1ineff_data = new TCanvas("c_eff_trigger_1ineff_data", "", 900, 800);
        c_eff_trigger_1ineff_data->cd();
        c_eff_trigger_1ineff_data->SetRightMargin(0.18);
        c_eff_trigger_1ineff_data->SetFillColor(0);
        c_eff_trigger_1ineff_data->SetFrameFillColor(0);
        eff_trigger_1ineff_data_slice->SetStats(false);
        eff_trigger_1ineff_data_slice->GetZaxis()->SetTitle("Trigger efficiency (1-inefficiency, data)");
        DrawValidTH2D(eff_trigger_1ineff_data_slice, nullptr, eff_plot_histograms);
        c_eff_trigger_1ineff_data->SaveAs((std::string(argv[5]) + "/" + plot_name + "_eff_trigger_1ineff_data" + distance_name + ".png").c_str());
        delete c_eff_trigger_1ineff_data;

        // trigger data/MC ratio
        TCanvas* c_weight_trigger = new TCanvas("c_weight_trigger", "", 900, 800);
        c_weight_trigger->cd();
        c_weight_trigger->SetRightMargin(0.18);
        c_weight_trigger->SetFillColor(0);
        c_weight_trigger->SetFrameFillColor(0);
        weight_trigger_slice->SetStats(false);
        weight_trigger_slice->GetZaxis()->SetTitle("Trigger data/MC ratio");
        DrawValidTH2D(weight_trigger_slice, valid_weight_slice);
        c_weight_trigger->SaveAs((std::string(argv[5]) + "/" + plot_name + "_weight_trigger" + distance_name + ".png").c_str());
        delete c_weight_trigger;

        // MC closure
        TCanvas* c_closure_MC = new TCanvas("c_closure_MC", "", 900, 800);
        c_closure_MC->cd();
        c_closure_MC->SetRightMargin(0.18);
        c_closure_MC->SetFillColor(0);
        c_closure_MC->SetFrameFillColor(0);
        closure_MC_slice->SetStats(false);
        closure_MC_slice->GetZaxis()->SetTitle("True / 1-inefficiency efficiency");
        DrawValidTH2D(closure_MC_slice);
        c_closure_MC->SaveAs((std::string(argv[5]) + "/" + plot_name + "_closure_MC" + distance_name + ".png").c_str());
        delete c_closure_MC;

        // absolute systematic error
        TCanvas* c_weight_syst_err = new TCanvas("c_weight_syst_err", "", 900, 800);
        c_weight_syst_err->cd();
        c_weight_syst_err->SetRightMargin(0.18);
        c_weight_syst_err->SetFillColor(0);
        c_weight_syst_err->SetFrameFillColor(0);
        weight_syst_err_slice->SetStats(false);
        weight_syst_err_slice->GetZaxis()->SetTitle("Absolute systematic error on weight");
        DrawValidTH2D(weight_syst_err_slice, valid_method_slice);
        c_weight_syst_err->SaveAs((std::string(argv[5]) + "/" + plot_name + "_weight_syst_err" + distance_name + ".png").c_str());
        delete c_weight_syst_err;

        delete eff_trigger_MC_slice;
        delete eff_trigger_1ineff_MC_slice;
        delete eff_trigger_1ineff_data_slice;
        delete weight_trigger_slice;
        delete closure_MC_slice;
        delete weight_syst_err_slice;
        delete valid_weight_slice;
        delete valid_method_slice;
    }

    return 0;
}
