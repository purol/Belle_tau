#include <stdio.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <cmath>

#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
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

    std::string histogram_title = ";" + used_variables.at(0) + ";" + used_variables.at(1);

    // ECL|CDC
    TH2D* hist_00_MC = new TH2D("hist_00_MC", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* hist_01_MC = new TH2D("hist_01_MC", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* hist_10_MC = new TH2D("hist_10_MC", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* hist_11_MC = new TH2D("hist_11_MC", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);

    TH2D* hist_01_data = new TH2D("hist_01_data", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* hist_10_data = new TH2D("hist_10_data", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* hist_11_data = new TH2D("hist_11_data", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);

    // efficiency matrix
    TH2D* eff_trigger_MC = new TH2D("eff_trigger_MC", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);

    // data/MC ratio
    TH2D* r_E = new TH2D("r_E", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* r_C = new TH2D("r_C", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* valid_E = new TH2D("valid_E", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* valid_C = new TH2D("valid_C", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);

    // alternative reference
    TH2D* hist_E_alt_pass_MC = new TH2D("hist_E_alt_pass_MC", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* hist_E_alt_fail_MC = new TH2D("hist_E_alt_fail_MC", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* hist_E_alt_pass_data = new TH2D("hist_E_alt_pass_data", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* hist_E_alt_fail_data = new TH2D("hist_E_alt_fail_data", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* hist_C_alt_pass_MC = new TH2D("hist_C_alt_pass_MC", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* hist_C_alt_fail_MC = new TH2D("hist_C_alt_fail_MC", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* hist_C_alt_pass_data = new TH2D("hist_C_alt_pass_data", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* hist_C_alt_fail_data = new TH2D("hist_C_alt_fail_data", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* r_E_alt = new TH2D("r_E_alt", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* r_C_alt = new TH2D("r_C_alt", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* r_E_ref_shift = new TH2D("r_E_ref_shift", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* r_C_ref_shift = new TH2D("r_C_ref_shift", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* valid_E_ref = new TH2D("valid_E_ref", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* valid_C_ref = new TH2D("valid_C_ref", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);

    // category yields and sum of squared weights
    std::vector<TH2D*> category_histograms = { hist_00_MC, hist_01_MC, hist_10_MC, hist_11_MC, hist_01_data, hist_10_data, hist_11_data };
    for (int i = 0; i < category_histograms.size(); i++) category_histograms.at(i)->Sumw2();
    std::vector<TH2D*> reference_histograms = { hist_E_alt_pass_MC, hist_E_alt_fail_MC, hist_E_alt_pass_data, hist_E_alt_fail_data, hist_C_alt_pass_MC, hist_C_alt_fail_MC, hist_C_alt_pass_data, hist_C_alt_fail_data };
    for (int i = 0; i < reference_histograms.size(); i++) reference_histograms.at(i)->Sumw2();

    // define conditions
    std::string ECL_pass = "(0.5 < ifNANgiveX__boL1PSNM__bohie__bc__cm__mi1__bc) || (0.5 < ifNANgiveX__boL1PSNM__bolml6__bc__cm__mi1__bc) || (0.5 < ifNANgiveX__boL1PSNM__bolml7__bc__cm__mi1__bc) || (0.5 < ifNANgiveX__boL1PSNM__bolml8__bc__cm__mi1__bc) || (0.5 < ifNANgiveX__boL1PSNM__bolml9__bc__cm__mi1__bc) || (0.5 < ifNANgiveX__boL1PSNM__bolml10__bc__cm__mi1__bc) || (0.5 < ifNANgiveX__boL1PSNM__bolml12__bc__cm__mi1__bc)";
    std::string ECL_fail = "(0.5 > ifNANgiveX__boL1PSNM__bohie__bc__cm__mi1__bc) && (0.5 > ifNANgiveX__boL1PSNM__bolml6__bc__cm__mi1__bc) && (0.5 > ifNANgiveX__boL1PSNM__bolml7__bc__cm__mi1__bc) && (0.5 > ifNANgiveX__boL1PSNM__bolml8__bc__cm__mi1__bc) && (0.5 > ifNANgiveX__boL1PSNM__bolml9__bc__cm__mi1__bc) && (0.5 > ifNANgiveX__boL1PSNM__bolml10__bc__cm__mi1__bc) && (0.5 > ifNANgiveX__boL1PSNM__bolml12__bc__cm__mi1__bc)";
    std::string CDC_pass = "(0.5 < ifNANgiveX__boL1PSNM__boffy__bc__cm__mi1__bc) || (0.5 < ifNANgiveX__boL1PSNM__bofyo__bc__cm__mi1__bc) || (0.5 < ifNANgiveX__boL1PSNM__bostt__bc__cm__mi1__bc)";
    std::string CDC_fail = "(0.5 > ifNANgiveX__boL1PSNM__boffy__bc__cm__mi1__bc) && (0.5 > ifNANgiveX__boL1PSNM__bofyo__bc__cm__mi1__bc) && (0.5 > ifNANgiveX__boL1PSNM__bostt__bc__cm__mi1__bc)";

    // trigger bits for alternative references
    std::string ECL_reference_pass = "(0.5 < ifNANgiveX__boL1PSNM__bohie__bc__cm__mi1__bc)";
    std::string CDC_reference_pass = "(0.5 < ifNANgiveX__boL1PSNM__bofff__bc__cm__mi1__bc) || (0.5 < ifNANgiveX__boL1PSNM__boffo__bc__cm__mi1__bc)";

    // MC
    Loader loader_00_MC("tau_lfv");
    for (int i = 0; i < MC_list.size(); i++) loader_00_MC.Load((std::string(argv[1]) + "/" + MC_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", MC_list.at(i).c_str());
    loader_00_MC.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_00_MC.Cut(ECL_fail.c_str());
    loader_00_MC.Cut(CDC_fail.c_str());
    loader_00_MC.FillTH2D(hist_00_MC, used_variables.at(0).c_str(), used_variables.at(1).c_str());
    loader_00_MC.end();

    Loader loader_01_MC("tau_lfv");
    for (int i = 0; i < MC_list.size(); i++) loader_01_MC.Load((std::string(argv[1]) + "/" + MC_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", MC_list.at(i).c_str());
    loader_01_MC.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_01_MC.Cut(ECL_fail.c_str());
    loader_01_MC.Cut(CDC_pass.c_str());
    loader_01_MC.FillTH2D(hist_01_MC, used_variables.at(0).c_str(), used_variables.at(1).c_str());
    loader_01_MC.end();

    Loader loader_10_MC("tau_lfv");
    for (int i = 0; i < MC_list.size(); i++) loader_10_MC.Load((std::string(argv[1]) + "/" + MC_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", MC_list.at(i).c_str());
    loader_10_MC.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_10_MC.Cut(ECL_pass.c_str());
    loader_10_MC.Cut(CDC_fail.c_str());
    loader_10_MC.FillTH2D(hist_10_MC, used_variables.at(0).c_str(), used_variables.at(1).c_str());
    loader_10_MC.end();

    Loader loader_11_MC("tau_lfv");
    for (int i = 0; i < MC_list.size(); i++) loader_11_MC.Load((std::string(argv[1]) + "/" + MC_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", MC_list.at(i).c_str());
    loader_11_MC.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_11_MC.Cut(ECL_pass.c_str());
    loader_11_MC.Cut(CDC_pass.c_str());
    loader_11_MC.FillTH2D(hist_11_MC, used_variables.at(0).c_str(), used_variables.at(1).c_str());
    loader_11_MC.end();

    // data
    Loader loader_01_data("tau_lfv");
    for (int i = 0; i < data_list.size(); i++) loader_01_data.Load((std::string(argv[3]) + "/" + data_list.at(i) + "/" + std::string(argv[4])).c_str(), "root", data_list.at(i).c_str());
    loader_01_data.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_01_data.Cut(ECL_fail.c_str());
    loader_01_data.Cut(CDC_pass.c_str());
    loader_01_data.FillTH2D(hist_01_data, used_variables.at(0).c_str(), used_variables.at(1).c_str());
    loader_01_data.end();

    Loader loader_10_data("tau_lfv");
    for (int i = 0; i < data_list.size(); i++) loader_10_data.Load((std::string(argv[3]) + "/" + data_list.at(i) + "/" + std::string(argv[4])).c_str(), "root", data_list.at(i).c_str());
    loader_10_data.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_10_data.Cut(ECL_pass.c_str());
    loader_10_data.Cut(CDC_fail.c_str());
    loader_10_data.FillTH2D(hist_10_data, used_variables.at(0).c_str(), used_variables.at(1).c_str());
    loader_10_data.end();

    Loader loader_11_data("tau_lfv");
    for (int i = 0; i < data_list.size(); i++) loader_11_data.Load((std::string(argv[3]) + "/" + data_list.at(i) + "/" + std::string(argv[4])).c_str(), "root", data_list.at(i).c_str());
    loader_11_data.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_11_data.Cut(ECL_pass.c_str());
    loader_11_data.Cut(CDC_pass.c_str());
    loader_11_data.FillTH2D(hist_11_data, used_variables.at(0).c_str(), used_variables.at(1).c_str());
    loader_11_data.end();

    // alternative reference for ECL
    Loader loader_E_alt_pass_MC("tau_lfv");
    for (int i = 0; i < MC_list.size(); i++) loader_E_alt_pass_MC.Load((std::string(argv[1]) + "/" + MC_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", MC_list.at(i).c_str());
    loader_E_alt_pass_MC.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_E_alt_pass_MC.Cut(ECL_pass.c_str());
    loader_E_alt_pass_MC.Cut(CDC_reference_pass.c_str());
    loader_E_alt_pass_MC.FillTH2D(hist_E_alt_pass_MC, used_variables.at(0).c_str(), used_variables.at(1).c_str());
    loader_E_alt_pass_MC.end();

    Loader loader_E_alt_fail_MC("tau_lfv");
    for (int i = 0; i < MC_list.size(); i++) loader_E_alt_fail_MC.Load((std::string(argv[1]) + "/" + MC_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", MC_list.at(i).c_str());
    loader_E_alt_fail_MC.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_E_alt_fail_MC.Cut(ECL_fail.c_str());
    loader_E_alt_fail_MC.Cut(CDC_reference_pass.c_str());
    loader_E_alt_fail_MC.FillTH2D(hist_E_alt_fail_MC, used_variables.at(0).c_str(), used_variables.at(1).c_str());
    loader_E_alt_fail_MC.end();

    Loader loader_E_alt_pass_data("tau_lfv");
    for (int i = 0; i < data_list.size(); i++) loader_E_alt_pass_data.Load((std::string(argv[3]) + "/" + data_list.at(i) + "/" + std::string(argv[4])).c_str(), "root", data_list.at(i).c_str());
    loader_E_alt_pass_data.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_E_alt_pass_data.Cut(ECL_pass.c_str());
    loader_E_alt_pass_data.Cut(CDC_reference_pass.c_str());
    loader_E_alt_pass_data.FillTH2D(hist_E_alt_pass_data, used_variables.at(0).c_str(), used_variables.at(1).c_str());
    loader_E_alt_pass_data.end();

    Loader loader_E_alt_fail_data("tau_lfv");
    for (int i = 0; i < data_list.size(); i++) loader_E_alt_fail_data.Load((std::string(argv[3]) + "/" + data_list.at(i) + "/" + std::string(argv[4])).c_str(), "root", data_list.at(i).c_str());
    loader_E_alt_fail_data.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_E_alt_fail_data.Cut(ECL_fail.c_str());
    loader_E_alt_fail_data.Cut(CDC_reference_pass.c_str());
    loader_E_alt_fail_data.FillTH2D(hist_E_alt_fail_data, used_variables.at(0).c_str(), used_variables.at(1).c_str());
    loader_E_alt_fail_data.end();

    // alternative reference for CDC
    Loader loader_C_alt_pass_MC("tau_lfv");
    for (int i = 0; i < MC_list.size(); i++) loader_C_alt_pass_MC.Load((std::string(argv[1]) + "/" + MC_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", MC_list.at(i).c_str());
    loader_C_alt_pass_MC.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_C_alt_pass_MC.Cut(CDC_pass.c_str());
    loader_C_alt_pass_MC.Cut(ECL_reference_pass.c_str());
    loader_C_alt_pass_MC.FillTH2D(hist_C_alt_pass_MC, used_variables.at(0).c_str(), used_variables.at(1).c_str());
    loader_C_alt_pass_MC.end();

    Loader loader_C_alt_fail_MC("tau_lfv");
    for (int i = 0; i < MC_list.size(); i++) loader_C_alt_fail_MC.Load((std::string(argv[1]) + "/" + MC_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", MC_list.at(i).c_str());
    loader_C_alt_fail_MC.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_C_alt_fail_MC.Cut(CDC_fail.c_str());
    loader_C_alt_fail_MC.Cut(ECL_reference_pass.c_str());
    loader_C_alt_fail_MC.FillTH2D(hist_C_alt_fail_MC, used_variables.at(0).c_str(), used_variables.at(1).c_str());
    loader_C_alt_fail_MC.end();

    Loader loader_C_alt_pass_data("tau_lfv");
    for (int i = 0; i < data_list.size(); i++) loader_C_alt_pass_data.Load((std::string(argv[3]) + "/" + data_list.at(i) + "/" + std::string(argv[4])).c_str(), "root", data_list.at(i).c_str());
    loader_C_alt_pass_data.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_C_alt_pass_data.Cut(CDC_pass.c_str());
    loader_C_alt_pass_data.Cut(ECL_reference_pass.c_str());
    loader_C_alt_pass_data.FillTH2D(hist_C_alt_pass_data, used_variables.at(0).c_str(), used_variables.at(1).c_str());
    loader_C_alt_pass_data.end();

    Loader loader_C_alt_fail_data("tau_lfv");
    for (int i = 0; i < data_list.size(); i++) loader_C_alt_fail_data.Load((std::string(argv[3]) + "/" + data_list.at(i) + "/" + std::string(argv[4])).c_str(), "root", data_list.at(i).c_str());
    loader_C_alt_fail_data.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_C_alt_fail_data.Cut(CDC_fail.c_str());
    loader_C_alt_fail_data.Cut(ECL_reference_pass.c_str());
    loader_C_alt_fail_data.FillTH2D(hist_C_alt_fail_data, used_variables.at(0).c_str(), used_variables.at(1).c_str());
    loader_C_alt_fail_data.end();

    // get trigger eff for MC
    for (int ix = 1; ix <= nbins1; ++ix) {
        for (int iy = 1; iy <= nbins2; ++iy) {
            const int bin = eff_trigger_MC->GetBin(ix, iy);

            const double pass = hist_01_MC->GetBinContent(bin) + hist_10_MC->GetBinContent(bin) + hist_11_MC->GetBinContent(bin);

            const double fail = hist_00_MC->GetBinContent(bin);
            const double total = pass + fail;

            if (total <= 0.0) {
                // just set eff = -1

                eff_trigger_MC->SetBinContent(bin, -1.0);
                eff_trigger_MC->SetBinError(bin, 0.0);
                continue;
            }

            const double var_pass = std::pow(hist_01_MC->GetBinError(bin), 2) + std::pow(hist_10_MC->GetBinError(bin), 2) + std::pow(hist_11_MC->GetBinError(bin), 2);

            const double var_fail = std::pow(hist_00_MC->GetBinError(bin), 2);

            const double eff = pass / total;
            const double var_eff = ((1.0 - eff) * (1.0 - eff) * var_pass + eff * eff * var_fail) / (total * total);

            eff_trigger_MC->SetBinContent(bin, eff);
            eff_trigger_MC->SetBinError(bin, std::sqrt(var_eff));
        }
    }

    // get relative trigger eff
    std::vector<TH2D*> pass_histograms = { hist_11_MC, hist_11_data, hist_11_MC, hist_11_data, hist_E_alt_pass_MC, hist_E_alt_pass_data, hist_C_alt_pass_MC, hist_C_alt_pass_data };
    std::vector<TH2D*> fail_histograms = { hist_01_MC, hist_01_data, hist_10_MC, hist_10_data, hist_E_alt_fail_MC, hist_E_alt_fail_data, hist_C_alt_fail_MC, hist_C_alt_fail_data };
    std::vector<TH2D*> ratios = { r_E, r_C, r_E_alt, r_C_alt };
    std::vector<TH2D*> reference_shifts = { r_E_ref_shift, r_C_ref_shift };
    std::vector<TH2D*> valid_reference_bins = { valid_E_ref, valid_C_ref };

    for (int ix = 1; ix <= nbins1; ++ix) {
        for (int iy = 1; iy <= nbins2; ++iy) {
            const int bin = eff_trigger_MC->GetBin(ix, iy);
            std::vector<double> relative_efficiencies(pass_histograms.size(), -1.0);
            std::vector<double> relative_efficiency_errors(pass_histograms.size(), 0.0);
            std::vector<bool> valid_bins(ratios.size(), false);

            for (int i = 0; i < relative_efficiencies.size(); i++) {
                const double pass = pass_histograms.at(i)->GetBinContent(bin);
                const double fail = fail_histograms.at(i)->GetBinContent(bin);
                const double total = pass + fail;

                // just set eff = -1
                if (!(pass >= 0.0 && fail >= 0.0 && total > 0.0) || !std::isfinite(total)) continue;

                const double var_pass = std::pow(pass_histograms.at(i)->GetBinError(bin), 2);
                const double var_fail = std::pow(fail_histograms.at(i)->GetBinError(bin), 2);
                const double eff = pass / total;
                const double var_eff = ((1.0 - eff) * (1.0 - eff) * var_pass + eff * eff * var_fail) / (total * total);

                relative_efficiencies.at(i) = eff;
                relative_efficiency_errors.at(i) = std::sqrt(var_eff);
            }

            // get data/MC ratio
            for (int i = 0; i < ratios.size(); i++) {
                const double eff_MC = relative_efficiencies.at(2 * i);
                const double eff_data = relative_efficiencies.at(2 * i + 1);

                // just set ratio = 1 for invalid bins
                ratios.at(i)->SetBinContent(bin, 1.0);
                ratios.at(i)->SetBinError(bin, 0.0);
                if (!(eff_MC > 0.0 && eff_data >= 0.0)) continue;

                const double ratio = eff_data / eff_MC;
                const double var_ratio = (std::pow(relative_efficiency_errors.at(2 * i + 1), 2) + ratio * ratio * std::pow(relative_efficiency_errors.at(2 * i), 2)) / (eff_MC * eff_MC);
                if (!std::isfinite(ratio) || !std::isfinite(var_ratio)) continue;

                ratios.at(i)->SetBinContent(bin, ratio);
                ratios.at(i)->SetBinError(bin, std::sqrt(var_ratio));
                valid_bins.at(i) = true;
            }
            valid_E->SetBinContent(bin, valid_bins.at(0));
            valid_C->SetBinContent(bin, valid_bins.at(1));

            // reference shift
            for (int i = 0; i < reference_shifts.size(); i++) {
                reference_shifts.at(i)->SetBinContent(bin, 0.0);
                reference_shifts.at(i)->SetBinError(bin, 0.0);
                valid_reference_bins.at(i)->SetBinContent(bin, 0.0);
                if (!(valid_bins.at(i) && valid_bins.at(i + 2))) continue;

                const double ratio_nominal = ratios.at(i)->GetBinContent(bin);
                const double ratio_alternative = ratios.at(i + 2)->GetBinContent(bin);
                const double shift = ratio_alternative - ratio_nominal;
                if (!std::isfinite(shift)) continue;

                reference_shifts.at(i)->SetBinContent(bin, shift);
                valid_reference_bins.at(i)->SetBinContent(bin, 1.0);
            }
        }
    }

    // print result
    FILE* fp = fopen((std::string(argv[5]) + "/" + std::string(argv[6]) + ".csv").c_str(), "w");
    if (fp == nullptr) {
        printf("Cannot open output file.\n");
        return 1;
    }
    fprintf(fp, "r_E,r_C,valid_E,valid_C,%s_min,%s_max,%s_min,%s_max", used_variables.at(0).c_str(), used_variables.at(0).c_str(), used_variables.at(1).c_str(), used_variables.at(1).c_str());
    fprintf(fp, ",r_E_ref_shift,r_C_ref_shift,valid_E_ref,valid_C_ref");
    fprintf(fp, ",r_E_stat_err,r_C_stat_err,r_E_alt,r_E_alt_stat_err,r_C_alt,r_C_alt_stat_err");
    for (int ix = 1; ix <= nbins1; ++ix) {
        for (int iy = 1; iy <= nbins2; ++iy) {
            const int bin = eff_trigger_MC->GetBin(ix, iy);
            fprintf(fp, "\n%.17g,%.17g,%.0f,%.0f", r_E->GetBinContent(bin), r_C->GetBinContent(bin), valid_E->GetBinContent(bin), valid_C->GetBinContent(bin));
            fprintf(fp, ",%.17g,%.17g", eff_trigger_MC->GetXaxis()->GetBinLowEdge(ix), eff_trigger_MC->GetXaxis()->GetBinUpEdge(ix));
            fprintf(fp, ",%.17g,%.17g", eff_trigger_MC->GetYaxis()->GetBinLowEdge(iy), eff_trigger_MC->GetYaxis()->GetBinUpEdge(iy));
            fprintf(fp, ",%.17g,%.17g,%.0f,%.0f", r_E_ref_shift->GetBinContent(bin), r_C_ref_shift->GetBinContent(bin), valid_E_ref->GetBinContent(bin), valid_C_ref->GetBinContent(bin));
            fprintf(fp, ",%.17g,%.17g", r_E->GetBinError(bin), r_C->GetBinError(bin));
            fprintf(fp, ",%.17g,%.17g", r_E_alt->GetBinContent(bin), r_E_alt->GetBinError(bin));
            fprintf(fp, ",%.17g,%.17g", r_C_alt->GetBinContent(bin), r_C_alt->GetBinError(bin));
        }
    }
    fclose(fp);

    // plot result
    std::string plot_name = std::string(argv[6]);
    const std::size_t extension_pos = plot_name.find_last_of('.');
    if (extension_pos != std::string::npos) plot_name = plot_name.substr(0, extension_pos);

    // trigger eff for MC
    TCanvas* c_eff_trigger_MC = new TCanvas("c_eff_trigger_MC", "", 900, 800);
    c_eff_trigger_MC->cd();
    c_eff_trigger_MC->SetRightMargin(0.18);
    eff_trigger_MC->SetStats(false);
    eff_trigger_MC->GetZaxis()->SetTitle("Trigger efficiency");
    eff_trigger_MC->Draw("COLZ");
    c_eff_trigger_MC->SaveAs((std::string(argv[5]) + "/" + plot_name + "_eff_trigger_MC.png").c_str());
    delete c_eff_trigger_MC;

    // ECL data/MC ratio
    TCanvas* c_r_E = new TCanvas("c_r_E", "", 900, 800);
    c_r_E->cd();
    c_r_E->SetRightMargin(0.18);
    r_E->SetStats(false);
    r_E->GetZaxis()->SetTitle("ECL data/MC ratio");
    r_E->Draw("COLZ");
    c_r_E->SaveAs((std::string(argv[5]) + "/" + plot_name + "_r_E.png").c_str());
    delete c_r_E;

    // ECL reference shift
    TCanvas* c_r_E_ref_shift = new TCanvas("c_r_E_ref_shift", "", 900, 800);
    c_r_E_ref_shift->cd();
    c_r_E_ref_shift->SetRightMargin(0.18);
    r_E_ref_shift->SetStats(false);
    r_E_ref_shift->GetZaxis()->SetTitle("ECL reference shift");
    r_E_ref_shift->Draw("COLZ");
    c_r_E_ref_shift->SaveAs((std::string(argv[5]) + "/" + plot_name + "_r_E_ref_shift.png").c_str());
    delete c_r_E_ref_shift;

    // ECL data/MC ratio with alternative reference
    TCanvas* c_r_E_alt = new TCanvas("c_r_E_alt", "", 900, 800);
    c_r_E_alt->cd();
    c_r_E_alt->SetRightMargin(0.18);
    r_E_alt->SetStats(false);
    r_E_alt->GetZaxis()->SetTitle("ECL data/MC ratio (alternative reference)");
    r_E_alt->Draw("COLZ");
    c_r_E_alt->SaveAs((std::string(argv[5]) + "/" + plot_name + "_r_E_alt.png").c_str());
    delete c_r_E_alt;

    // CDC data/MC ratio
    TCanvas* c_r_C = new TCanvas("c_r_C", "", 900, 800);
    c_r_C->cd();
    c_r_C->SetRightMargin(0.18);
    r_C->SetStats(false);
    r_C->GetZaxis()->SetTitle("CDC data/MC ratio");
    r_C->Draw("COLZ");
    c_r_C->SaveAs((std::string(argv[5]) + "/" + plot_name + "_r_C.png").c_str());
    delete c_r_C;

    // CDC reference shift
    TCanvas* c_r_C_ref_shift = new TCanvas("c_r_C_ref_shift", "", 900, 800);
    c_r_C_ref_shift->cd();
    c_r_C_ref_shift->SetRightMargin(0.18);
    r_C_ref_shift->SetStats(false);
    r_C_ref_shift->GetZaxis()->SetTitle("CDC reference shift");
    r_C_ref_shift->Draw("COLZ");
    c_r_C_ref_shift->SaveAs((std::string(argv[5]) + "/" + plot_name + "_r_C_ref_shift.png").c_str());
    delete c_r_C_ref_shift;

    // CDC data/MC ratio with alternative reference
    TCanvas* c_r_C_alt = new TCanvas("c_r_C_alt", "", 900, 800);
    c_r_C_alt->cd();
    c_r_C_alt->SetRightMargin(0.18);
    r_C_alt->SetStats(false);
    r_C_alt->GetZaxis()->SetTitle("CDC data/MC ratio (alternative reference)");
    r_C_alt->Draw("COLZ");
    c_r_C_alt->SaveAs((std::string(argv[5]) + "/" + plot_name + "_r_C_alt.png").c_str());
    delete c_r_C_alt;

    return 0;
}
