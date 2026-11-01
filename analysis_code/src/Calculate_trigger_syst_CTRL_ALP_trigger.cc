#include <stdio.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <iomanip>

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
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
    TH3D* eff_trigger_data = new TH3D("eff_trigger_data", histogram_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data(), nbins3, distance_bins.data());
    TH3D* correlation_MC = new TH3D("correlation_MC", histogram_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data(), nbins3, distance_bins.data());

    // define conditions
    std::string ECL_pass = "(0.5 < L1PSNM__bohie__bc) || (0.5 < L1PSNM__bolml6__bc) || (0.5 < L1PSNM__bolml7__bc) || (0.5 < L1PSNM__bolml8__bc) || (0.5 < L1PSNM__bolml9__bc) || (0.5 < L1PSNM__bolml10__bc) || (0.5 < L1PSNM__bolml12__bc)";
    std::string ECL_fail = "(0.5 > L1PSNM__bohie__bc) && (0.5 > L1PSNM__bolml6__bc) && (0.5 > L1PSNM__bolml7__bc) && (0.5 > L1PSNM__bolml8__bc) && (0.5 > L1PSNM__bolml9__bc) && (0.5 > L1PSNM__bolml10__bc) && (0.5 > L1PSNM__bolml12__bc)";
    std::string CDC_pass = "(0.5 < L1PSNM__boffy__bc) || (0.5 < L1PSNM__bofyo__bc) || (0.5 < L1PSNM__bostt__bc)";
    std::string CDC_fail = "(0.5 > L1PSNM__boffy__bc) && (0.5 > L1PSNM__bofyo__bc) && (0.5 > L1PSNM__bostt__bc)";

    // MC
    Loader loader_00_MC("tau_lfv");
    for (int i = 0; i < MC_list.size(); i++) loader_00_MC.Load((std::string(argv[1]) + "/" + MC_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", MC_list.at(i).c_str());
    loader_00_MC.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_00_MC.Cut(ECL_fail.c_str());
    loader_00_MC.Cut(CDC_fail.c_str());
    loader_00_MC.FillTH3D(hist_00_MC, used_variables.at(0).c_str(), used_variables.at(1).c_str(), distance_variable.c_str());
    loader_00_MC.end();

    Loader loader_01_MC("tau_lfv");
    for (int i = 0; i < MC_list.size(); i++) loader_01_MC.Load((std::string(argv[1]) + "/" + MC_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", MC_list.at(i).c_str());
    loader_01_MC.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_01_MC.Cut(ECL_fail.c_str());
    loader_01_MC.Cut(CDC_pass.c_str());
    loader_01_MC.FillTH3D(hist_01_MC, used_variables.at(0).c_str(), used_variables.at(1).c_str(), distance_variable.c_str());
    loader_01_MC.end();

    Loader loader_10_MC("tau_lfv");
    for (int i = 0; i < MC_list.size(); i++) loader_10_MC.Load((std::string(argv[1]) + "/" + MC_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", MC_list.at(i).c_str());
    loader_10_MC.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_10_MC.Cut(ECL_pass.c_str());
    loader_10_MC.Cut(CDC_fail.c_str());
    loader_10_MC.FillTH3D(hist_10_MC, used_variables.at(0).c_str(), used_variables.at(1).c_str(), distance_variable.c_str());
    loader_10_MC.end();

    Loader loader_11_MC("tau_lfv");
    for (int i = 0; i < MC_list.size(); i++) loader_11_MC.Load((std::string(argv[1]) + "/" + MC_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", MC_list.at(i).c_str());
    loader_11_MC.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
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

    // correlation for MC
    correlation_MC->Reset();
    correlation_MC->Add(hist_11_MC);
    correlation_MC->Multiply(hist_00_MC);
    correlation_MC->Divide(hist_10_MC);
    correlation_MC->Divide(hist_01_MC);

    // get trigger eff for MC
    for (int ix = 1; ix <= nbins1; ++ix) {
        for (int iy = 1; iy <= nbins2; ++iy) {
            for (int iz = 1; iz <= nbins3; ++iz) {
                const int bin = eff_trigger_MC->GetBin(ix, iy, iz);

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
    }

    // estimate trigger eff for data
    for (int ix = 1; ix <= nbins1; ++ix) {
        for (int iy = 1; iy <= nbins2; ++iy) {
            for (int iz = 1; iz <= nbins3; ++iz) {
                const int bin = eff_trigger_data->GetBin(ix, iy, iz);

                const double a = hist_01_data->GetBinContent(bin);
                const double b = hist_10_data->GetBinContent(bin);
                const double c = hist_11_data->GetBinContent(bin);
                const double rho = correlation_MC->GetBinContent(bin);

                // just set eff = -1
                if (!(a > 0.0 && b > 0.0 && c > 0.0 && rho > 0.0) || !std::isfinite(rho)) {

                    eff_trigger_data->SetBinContent(bin, -1.0);
                    eff_trigger_data->SetBinError(bin, 0.0);
                    continue;
                }

                const double pass = a + b + c;
                const double fail_est = rho * a * b / c;
                const double total = pass + fail_est;
                const double total2 = total * total;

                const double eff = pass / total;

                const double d_da = (fail_est - pass * rho * b / c) / total2;

                const double d_db = (fail_est - pass * rho * a / c) / total2;

                const double d_dc = (fail_est + pass * fail_est / c) / total2;

                const double d_drho = -pass * a * b / c / total2;

                const double var_eff = std::pow(d_da * hist_01_data->GetBinError(bin), 2) + std::pow(d_db * hist_10_data->GetBinError(bin), 2) + std::pow(d_dc * hist_11_data->GetBinError(bin), 2) + std::pow(d_drho * correlation_MC->GetBinError(bin), 2);

                eff_trigger_data->SetBinContent(bin, eff);
                eff_trigger_data->SetBinError(bin, std::sqrt(var_eff));
            }
        }
    }

    // print result
    FILE* fp = fopen((std::string(argv[5]) + "/" + std::string(argv[6]) + ".txt").c_str(), "w");
    fprintf(fp, "weight,%s_min,%s_max,%s_min,%s_max,%s_min,%s_max", used_variables.at(0).c_str(), used_variables.at(0).c_str(), used_variables.at(1).c_str(), used_variables.at(1).c_str(), distance_variable.c_str(), distance_variable.c_str());
    for (int i = 1; i <= nbins1; i++) {
        for (int j = 1; j <= nbins2; j++) {
            for (int k = 1; k <= nbins3; k++) {
                fprintf(fp, "\n");
                const double low1 = eff_trigger_MC->GetXaxis()->GetBinLowEdge(i);
                const double high1 = eff_trigger_MC->GetXaxis()->GetBinUpEdge(i);
                const double low2 = eff_trigger_MC->GetYaxis()->GetBinLowEdge(j);
                const double high2 = eff_trigger_MC->GetYaxis()->GetBinUpEdge(j);
                const double low3 = eff_trigger_MC->GetZaxis()->GetBinLowEdge(k);
                const double high3 = eff_trigger_MC->GetZaxis()->GetBinUpEdge(k);
                double weight = 1.0;
                if((eff_trigger_data->GetBinContent(i, j, k) >= 0) && (eff_trigger_MC->GetBinContent(i, j, k) >= 0)) weight = eff_trigger_data->GetBinContent(i, j, k) / eff_trigger_MC->GetBinContent(i, j, k);
                fprintf(fp, "%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g", weight, low1, high1, low2, high2, low3, high3);
            }
        }
    }
    fclose(fp);

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
        TH2D* eff_trigger_data_slice = new TH2D(("eff_trigger_data" + distance_name).c_str(), slice_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data());
        TH2D* correlation_MC_slice = new TH2D(("correlation_MC" + distance_name).c_str(), slice_title.c_str(), nbins1, bins1.data(), nbins2, bins2.data());
        for (int ix = 1; ix <= nbins1; ix++) {
            for (int iy = 1; iy <= nbins2; iy++) {
                const int bin = eff_trigger_MC->GetBin(ix, iy, iz);
                const int slice_bin = eff_trigger_MC_slice->GetBin(ix, iy);
                eff_trigger_MC_slice->SetBinContent(slice_bin, eff_trigger_MC->GetBinContent(bin));
                eff_trigger_MC_slice->SetBinError(slice_bin, eff_trigger_MC->GetBinError(bin));
                eff_trigger_data_slice->SetBinContent(slice_bin, eff_trigger_data->GetBinContent(bin));
                eff_trigger_data_slice->SetBinError(slice_bin, eff_trigger_data->GetBinError(bin));
                correlation_MC_slice->SetBinContent(slice_bin, correlation_MC->GetBinContent(bin));
                correlation_MC_slice->SetBinError(slice_bin, correlation_MC->GetBinError(bin));
            }
        }

        // trigger eff for MC
        TCanvas* c_eff_trigger_MC = new TCanvas("c_eff_trigger_MC", "", 900, 800);
        c_eff_trigger_MC->cd();
        c_eff_trigger_MC->SetRightMargin(0.18);
        eff_trigger_MC_slice->SetStats(false);
        eff_trigger_MC_slice->GetZaxis()->SetTitle("Trigger efficiency");
        eff_trigger_MC_slice->Draw("COLZ");
        c_eff_trigger_MC->SaveAs((std::string(argv[5]) + "/" + plot_name + "_eff_trigger_MC" + distance_name + ".png").c_str());
        delete c_eff_trigger_MC;

        // trigger eff for data
        TCanvas* c_eff_trigger_data = new TCanvas("c_eff_trigger_data", "", 900, 800);
        c_eff_trigger_data->cd();
        c_eff_trigger_data->SetRightMargin(0.18);
        eff_trigger_data_slice->SetStats(false);
        eff_trigger_data_slice->GetZaxis()->SetTitle("Trigger efficiency");
        eff_trigger_data_slice->Draw("COLZ");
        c_eff_trigger_data->SaveAs((std::string(argv[5]) + "/" + plot_name + "_eff_trigger_data" + distance_name + ".png").c_str());
        delete c_eff_trigger_data;

        // correlation for MC
        TCanvas* c_correlation_MC = new TCanvas("c_correlation_MC", "", 900, 800);
        c_correlation_MC->cd();
        c_correlation_MC->SetRightMargin(0.18);
        correlation_MC_slice->SetStats(false);
        correlation_MC_slice->GetZaxis()->SetTitle("MC odds ratio");
        correlation_MC_slice->Draw("COLZ");
        c_correlation_MC->SaveAs((std::string(argv[5]) + "/" + plot_name + "_correlation_MC" + distance_name + ".png").c_str());
        delete c_correlation_MC;

        delete eff_trigger_MC_slice;
        delete eff_trigger_data_slice;
        delete correlation_MC_slice;
    }

    return 0;
}
