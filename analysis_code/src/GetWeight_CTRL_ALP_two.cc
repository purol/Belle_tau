#include <stdio.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <iomanip>

#include <TH1.h>
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
    * argv[5]: compare variable
    * argv[6]: sample1 list (separated by colon)
    * argv[7]: sample2 list (separated by colon)
    * argv[8]: sample1 lable
    * argv[9]: sample2 lable
    * argv[10]: output path
    * argv[11]: M_deltaE path for tau -> a mu decay
    * argv[12]: mass
    * argv[13]: lifetime
    * argv[14]: A constant
    * argv[15]: B constant
    */

    double mass = std::stod(argv[12]);
    double life = std::stod(argv[13]);
    int A = std::stoi(argv[14]);
    int B = std::stoi(argv[15]);

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

    std::vector<std::string> sample_list = split(argv[4], ':');

    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution((std::string(argv[11]) + "/alpha_mass" + std::format("{:g}", mass) + "_life" + std::format("{:g}", life) + "_A" + std::to_string(A) + "_B" + std::to_string(B) + "_M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    std::vector<std::string> sample1_list = split(argv[6], ':');
    std::vector<std::string> sample2_list = split(argv[7], ':');

    // define histograms
    const int nbins = 50;
    TH1D* hist_CTRL = new TH1D("hist_CTRL", ";mass of ALP [GeV/c^{2}];arbitrary unit", nbins, 0.2, 2.0);
    TH1D* hist_CTRL_ALP = new TH1D("hist_CTRL_ALP", ";mass of ALP [GeV/c^{2}];arbitrary unit", nbins, 0.2, 2.0);
    TH1D* hist_ratio = new TH1D("hist_ratio", ";mass of ALP [GeV/c^{2}];ratio", nbins, 0.2, 2.0);

    // CTRL ALP
    Loader loader_sample1("tau_lfv");
    for (int i = 0; i < sample1_list.size(); i++) loader_sample1.Load((std::string(argv[1]) + "/" + sample1_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", sample1_list.at(i).c_str());
    loader_sample1.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_sample1.GetRandom({ "extraInfo__boinvMOneTwo__bc", "extraInfo__boinvMOneThree__bc" , "extraInfo__boinvMTwoThree__bc" }, "random_M");
    loader_sample1.FillTH1D(hist_CTRL, "random_M");
    loader_sample1.end();

    // ALP
    Loader loader_sample2("tau_lfv");
    for (int i = 0; i < sample2_list.size(); i++) loader_sample2.Load((std::string(argv[3]) + "/" + sample2_list.at(i) + "/" + std::string(argv[4])).c_str(), "root", sample2_list.at(i).c_str());
    loader_sample2.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_sample2.Cut(("(" + std::to_string(deltaE_peak - 16 * deltaE_left_sigma) + "< deltaE) && (deltaE < " + std::to_string(deltaE_peak + 6 * deltaE_right_sigma) + ")").c_str());
    loader_sample2.Cut(("(" + std::to_string(M_peak - 20 * M_left_sigma) + "< M) && (M < " + std::to_string(M_peak + 20 * M_right_sigma) + ")").c_str());
    loader_sample2.Cut(("(" + std::to_string(mass - M_left_cut_value) + "< extraInfo__boALP_M__bc) && (extraInfo__boALP_M__bc <" + std::to_string(mass + M_right_cut_value) + ")").c_str());
    loader_sample2.RandomBCS();
    loader_sample2.IsBCSValid();
    loader_sample2.Cut(("(" + std::to_string(deltaE_peak - 15 * deltaE_left_sigma) + "< deltaE) && (deltaE < " + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + ")").c_str());
    loader_sample2.Cut(("(" + std::to_string(M_peak - 5 * M_left_sigma) + "< M) && (M < " + std::to_string(M_peak + 5 * M_right_sigma) + ")").c_str());
    loader_sample2.FillTH1D(hist_CTRL_ALP, argv[5]);
    loader_sample2.end();

    // calculate weights
    double N_CTRL_total = 0;
    for (int i = 1; i <= nbins; i++) {
        double Nevt = hist_CTRL->GetBinContent(i);
        N_CTRL_total = N_CTRL_total + Nevt;
    }

    double N_ALP_total = 0;
    for (int i = 1; i <= nbins; i++) {
        double Nevt = hist_CTRL_ALP->GetBinContent(i);
        N_ALP_total = N_ALP_total + Nevt;
    }

    double N_ALP_used_for_weight = 0.0;
    for (int i = 1; i <= nbins; i++) {
        double N_CTRL = hist_CTRL->GetBinContent(i);
        double N_ALP = hist_CTRL_ALP->GetBinContent(i);
        if (N_CTRL > 0.000001) {
            hist_ratio->SetBinContent(i, N_ALP / N_CTRL);
            N_ALP_used_for_weight = N_ALP_used_for_weight + N_ALP;
        }
        else {
            hist_ratio->SetBinContent(i, 0.0);
        }
    }

    // this is needed to preserve N_CTRL_total
    hist_ratio->Scale(N_CTRL_total / N_ALP_used_for_weight);

    FILE* fp = fopen((std::string(argv[10]) + "/weight_two_M_CTRL_" + std::string(argv[12]) + "_" + std::string(argv[13]) + "_" + std::string(argv[14]) + "_" + std::string(argv[15]) + ".csv").c_str(), "w");
    fprintf(fp, "weight,M_min,M_max");
    for (int i = 1; i <= nbins; i++) {
        fprintf(fp, "\n");
        const double low = hist_ratio->GetXaxis()->GetBinLowEdge(i);
        const double high = hist_ratio->GetXaxis()->GetBinUpEdge(i);
        const double weight = hist_ratio->GetBinContent(i);
        fprintf(fp, "%lf,%lf,%lf", weight, low, high);
    }
    fclose(fp);

    // make plots
    hist_CTRL->Scale(1.0 / hist_CTRL->Integral());
    hist_CTRL_ALP->Scale(1.0 / hist_CTRL_ALP->Integral());

    hist_CTRL->SetFillStyle(3004);
    hist_CTRL->SetLineColor(kBlue);
    hist_CTRL->SetFillColor(kBlue);

    hist_CTRL_ALP->SetFillStyle(3005);
    hist_CTRL_ALP->SetLineColor(kRed);
    hist_CTRL_ALP->SetFillColor(kRed);

    TCanvas* c_temp = new TCanvas("c", "", 800, 800); c_temp->cd();

    double maxY = 0.0;

    if (hist_CTRL->GetMaximum() > hist_CTRL_ALP->GetMaximum()) maxY = hist_CTRL->GetMaximum();
    else maxY = hist_CTRL_ALP->GetMaximum();

    hist_CTRL->SetMaximum(1.40 * maxY);

    hist_CTRL->SetTitle(""); hist_CTRL_ALP->SetTitle("");
    hist_CTRL->Draw("Hist"); hist_CTRL_ALP->Draw("HistSAME");

    TLegend* legend = new TLegend(0.9, 0.9, 0.6, 0.6);
    legend->AddEntry(hist_CTRL, argv[8], "f");
    legend->AddEntry(hist_CTRL_ALP, argv[9], "f");
    legend->SetFillStyle(0); legend->SetLineWidth(0);
    legend->Draw();

    c_temp->SaveAs((std::string(argv[10]) + "/mass_comparison_two_" + std::string(argv[12]) + "_" + std::string(argv[13]) + "_" + std::string(argv[14]) + "_" + std::string(argv[15]) + ".png").c_str());

    return 0;
}
