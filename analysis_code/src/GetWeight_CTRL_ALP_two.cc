#include <stdio.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <format>
#include <cmath>
#include <limits>
#include <exception>

#include <TH1.h>
#include <TH2.h>
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
    * argv[1]: input1 path 1 (CTRL_ALP)
    * argv[2]: input1 path 2 (CTRL_ALP)
    * argv[3]: input2 path 1 (ALP)
    * argv[4]: input2 path 2 (ALP)
    * argv[5]: two compare variables (separated by colon)
    * argv[6]: two binnings (separated by colon. min1:max1:numbin1:min2:max2:numbin2)
    * argv[7]: sample1 list (separated by colon)
    * argv[8]: sample2 list (separated by colon)
    * argv[9]: sample1 lable
    * argv[10]: sample2 lable
    * argv[11]: output path
    * argv[12]: M_deltaE path for tau -> a mu decay
    * argv[13]: mass
    * argv[14]: lifetime
    * argv[15]: A constant
    * argv[16]: B constant
    */

    if (argc != 17) {
        printf("16 arguments are required.\n");
        return 1;
    }

    std::vector<std::string> compare_variables = split(argv[5], ':');
    std::vector<std::string> binnings = split(argv[6], ':');
    if ((compare_variables.size() != 2) || (binnings.size() != 6)) {
        printf("two compare variables and min1:max1:numbin1:min2:max2:numbin2 are required.\n");
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

    double mass = std::stod(argv[13]);
    double life = std::stod(argv[14]);
    int A = std::stoi(argv[15]);
    int B = std::stoi(argv[16]);

    double M_left_cut_value = 0;
    double M_right_cut_value = 0;
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

    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution((std::string(argv[12]) + "/alpha_mass" + std::format("{:g}", mass) + "_life" + std::format("{:g}", life) + "_A" + std::to_string(A) + "_B" + std::to_string(B) + "_M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    EventWeights::Register("MC_weight", MC_weight);

    std::vector<std::string> sample1_list = split(argv[7], ':');
    std::vector<std::string> sample2_list = split(argv[8], ':');

    // define histograms
    const double min1 = binning_values.at(0);
    const double max1 = binning_values.at(1);
    const int nbins1 = static_cast<int>(binning_values.at(2));
    const double min2 = binning_values.at(3);
    const double max2 = binning_values.at(4);
    const int nbins2 = static_cast<int>(binning_values.at(5));
    std::string histogram_title = ";" + compare_variables.at(0) + ";" + compare_variables.at(1);
    TH2D* hist_CTRL = new TH2D("hist_CTRL", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* hist_CTRL_ALP = new TH2D("hist_CTRL_ALP", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* hist_ratio = new TH2D("hist_ratio", (histogram_title + ";ratio").c_str(), nbins1, min1, max1, nbins2, min2, max2);

    // CTRL ALP
    Loader loader_sample1("tau_lfv");
    for (int i = 0; i < sample1_list.size(); i++) loader_sample1.Load((std::string(argv[1]) + "/" + sample1_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", sample1_list.at(i).c_str());
    loader_sample1.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_sample1.FillTH2D(hist_CTRL, compare_variables.at(0).c_str(), compare_variables.at(1).c_str());
    loader_sample1.end();

    // ALP
    Loader loader_sample2("tau_lfv");
    for (int i = 0; i < sample2_list.size(); i++) loader_sample2.Load((std::string(argv[3]) + "/" + sample2_list.at(i) + "/" + std::string(argv[4])).c_str(), "root", sample2_list.at(i).c_str());
    loader_sample2.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_sample2.Cut(GetBCSCut(deltaE_peak, deltaE_left_sigma, deltaE_right_sigma, M_peak, M_left_sigma, M_right_sigma).c_str());
    loader_sample2.Cut(("(" + std::to_string(mass - M_left_cut_value) + "< extraInfo__boALP_M__bc) && (extraInfo__boALP_M__bc <" + std::to_string(mass + M_right_cut_value) + ")").c_str());
    loader_sample2.RandomBCS();
    loader_sample2.IsBCSValid();
    loader_sample2.Cut(("(" + std::to_string(M_peak - 20 * M_left_sigma) + "< M) && (M < " + std::to_string(M_peak + 20 * M_right_sigma) + ")").c_str());
    loader_sample2.Cut(("(" + std::to_string(deltaE_peak - 15 * deltaE_left_sigma) + "< deltaE) && (deltaE < " + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + ")").c_str());
    loader_sample2.FillTH2D(hist_CTRL_ALP, compare_variables.at(0).c_str(), compare_variables.at(1).c_str());
    loader_sample2.end();

    // calculate weights
    double N_CTRL_total = 0;
    // underflow and overflow bins are not used for weights
    for (int i = 1; i <= nbins1; i++) {
        for (int j = 1; j <= nbins2; j++) {
            double Nevt = hist_CTRL->GetBinContent(i, j);
            N_CTRL_total = N_CTRL_total + Nevt;
        }
    }

    double N_ALP_total = 0;
    for (int i = 1; i <= nbins1; i++) {
        for (int j = 1; j <= nbins2; j++) {
            double Nevt = hist_CTRL_ALP->GetBinContent(i, j);
            N_ALP_total = N_ALP_total + Nevt;
        }
    }

    double N_ALP_used_for_weight = 0.0;
    for (int i = 1; i <= nbins1; i++) {
        for (int j = 1; j <= nbins2; j++) {
            double N_CTRL = hist_CTRL->GetBinContent(i, j);
            double N_ALP = hist_CTRL_ALP->GetBinContent(i, j);
            if (N_CTRL > 0.000001) {
                hist_ratio->SetBinContent(i, j, N_ALP / N_CTRL);
                N_ALP_used_for_weight = N_ALP_used_for_weight + N_ALP;
            }
            else {
                hist_ratio->SetBinContent(i, j, 0.0);
            }
        }
    }

    // this is needed to preserve N_CTRL_total
    hist_ratio->Scale(N_CTRL_total / N_ALP_used_for_weight);

    FILE* fp = fopen((std::string(argv[11]) + "/weight_two_2D_CTRL_" + std::string(argv[13]) + "_" + std::string(argv[14]) + "_" + std::string(argv[15]) + "_" + std::string(argv[16]) + ".csv").c_str(), "w");
    fprintf(fp, "weight,%s_min,%s_max,%s_min,%s_max", compare_variables.at(0).c_str(), compare_variables.at(0).c_str(), compare_variables.at(1).c_str(), compare_variables.at(1).c_str());
    for (int i = 1; i <= nbins1; i++) {
        for (int j = 1; j <= nbins2; j++) {
            fprintf(fp, "\n");
            const double low1 = hist_ratio->GetXaxis()->GetBinLowEdge(i);
            const double high1 = hist_ratio->GetXaxis()->GetBinUpEdge(i);
            const double low2 = hist_ratio->GetYaxis()->GetBinLowEdge(j);
            const double high2 = hist_ratio->GetYaxis()->GetBinUpEdge(j);
            const double weight = hist_ratio->GetBinContent(i, j);
            fprintf(fp, "%.17g,%.17g,%.17g,%.17g,%.17g", weight, low1, high1, low2, high2);
        }
    }
    fclose(fp);

    // make plots
    hist_CTRL->Scale(1.0 / hist_CTRL->Integral());
    hist_CTRL_ALP->Scale(1.0 / hist_CTRL_ALP->Integral());

    gStyle->SetOptStat(0);

    TCanvas* c_temp = new TCanvas("c", "", 1500, 500); c_temp->Divide(3, 1);

    double maxY = 0.0;

    if (hist_CTRL->GetMaximum() > hist_CTRL_ALP->GetMaximum()) maxY = hist_CTRL->GetMaximum();
    else maxY = hist_CTRL_ALP->GetMaximum();

    hist_CTRL->SetMinimum(0.0); hist_CTRL_ALP->SetMinimum(0.0);
    hist_CTRL->SetMaximum(maxY); hist_CTRL_ALP->SetMaximum(maxY);

    hist_CTRL->SetTitle(argv[9]); hist_CTRL_ALP->SetTitle(argv[10]);
    hist_ratio->SetTitle("weight");
    c_temp->cd(1); gPad->SetRightMargin(0.16); hist_CTRL->Draw("COLZ");
    c_temp->cd(2); gPad->SetRightMargin(0.16); hist_CTRL_ALP->Draw("COLZ");
    c_temp->cd(3); gPad->SetRightMargin(0.16); hist_ratio->Draw("COLZ");

    c_temp->SaveAs((std::string(argv[11]) + "/comparison_two_2D_CTRL_" + std::string(argv[13]) + "_" + std::string(argv[14]) + "_" + std::string(argv[15]) + "_" + std::string(argv[16]) + ".png").c_str());

    return 0;
}
