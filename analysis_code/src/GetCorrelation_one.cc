#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <utility>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "MyModule.h"
#include "functions.h"

int main(int argc, char* argv[]) {
    /*
    * argv[1]: input path 1
    * argv[2]: input path 2
    * argv[3]: including string
    * argv[4]: sample list (separated by colon)
    * argv[5]: resolution file path
    * argv[6]: output path
    */

    std::vector<std::string> sample_list = split(argv[4], ':');

    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution((std::string(argv[5]) + "/M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    EventWeights::Register("MC_weight", MC_weight);

    std::string cut_M_1 = "((" + std::to_string(M_peak - 20 * M_left_sigma) + " < M) && (M < " + std::to_string(M_peak + 20 * M_right_sigma) + "))";
    std::string cut_deltaE_1 = "((" + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak + 6 * deltaE_right_sigma) + "))";
    std::string cut_M_deltaE_1 = "(" + cut_M_1 + "&&" + cut_deltaE_1 + ")";

    std::string cut_M_2 = "((" + std::to_string(M_peak - 20 * M_left_sigma) + " < M) && (M < " + std::to_string(M_peak + 20 * M_right_sigma) + "))";
    std::string cut_deltaE_2 = "((" + std::to_string(deltaE_peak - 16 * deltaE_left_sigma) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + "))";
    std::string cut_M_deltaE_2 = "(" + cut_M_2 + "&&" + cut_deltaE_2 + ")";

    std::string cut_region = cut_M_deltaE_1 + "||" + cut_M_deltaE_2;

    // define roorealvar
    RooRealVar BDT1("BDT1", "BDT1", 0.0, 1.0);
    RooRealVar M_inv("M", "M", M_peak - 20 * M_left_sigma, M_peak + 20 * M_right_sigma);
    RooRealVar weight("weight", "weight", 0.0, 10.0);

    // loader
    Loader loader("tau_lfv");

    for (int i = 0; i < sample_list.size(); i++) loader.Load((argv[1] + std::string("/") + sample_list.at(i) + std::string("/") + std::string(argv[2])).c_str(), argv[3], "label");
    loader.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });

    loader.PrintInformation("========== initial ==========");

    loader.Cut(cut_region.c_str());
    loader.PrintInformation("========== (-20 delta < M < 20 delta) && (-16 delta < deltaE < 6 delta) ==========");

    loader.RandomBCS();
    loader.IsBCSValid();
    loader.PrintInformation("========== Random BCS ==========");

    loader.Cut(("(" + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak + 5 * deltaE_right_sigma) + ")").c_str());
    loader.PrintInformation("========== (-5 delta < deltaE < 5 delta) ==========");

    loader.DrawTH2D("BDT1", "M", ";BDT1 output;M [GeV/c^{2}]", 50, 0.0, 1.0, 50, M_peak - 20 * M_left_sigma, M_peak + 20 * M_right_sigma, (argv[6] + std::string("/") + "BDT1_M_distribution.png").c_str(), "COLZ");
   
    RooDataSet dataset("dataset", "dataset", RooArgSet(BDT1, M_inv, weight), RooFit::WeightVar("weight"));
    loader.FillDataSet(&dataset, { &BDT1, &M_inv }, { "BDT1", "M" });

    loader.end();

    // make detail plots
    std::vector<std::pair<double, double>> BDT_bins = {
        {0.0, 0.2},
        {0.2, 0.4},
        {0.4, 0.6},
        {0.6, 0.8},
        {0.8, 1.0}
    };
    std::vector<TH1D*> M_BDT_binning;

    for (const auto& [lower, upper] : BDT_bins) {
        TH1D* temp_th1d = new TH1D(("temp_th1d_" + std::to_string(lower) + "_" + std::to_string(upper)).c_str(), ";M [GeV/c^{2}]; arbitrary unit", 50, M_peak - 20 * M_left_sigma, M_peak + 20 * M_right_sigma);
        M_BDT_binning.push_back(temp_th1d);

        Loader loader_temp("tau_lfv");

        for (int i = 0; i < sample_list.size(); i++) loader_temp.Load((argv[1] + std::string("/") + sample_list.at(i) + std::string("/") + std::string(argv[2])).c_str(), argv[3], "label");
        loader_temp.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });

        loader_temp.PrintInformation("========== initial ==========");

        loader_temp.Cut(cut_region.c_str());
        loader_temp.PrintInformation("========== (-20 delta < M < 20 delta) && (-16 delta < deltaE < 6 delta) ==========");

        loader_temp.RandomBCS();
        loader_temp.IsBCSValid();
        loader_temp.PrintInformation("========== Random BCS ==========");

        loader_temp.Cut(("(" + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak + 5 * deltaE_right_sigma) + ")").c_str());
        loader_temp.PrintInformation("========== (-5 delta < deltaE < 5 delta) ==========");

        loader_temp.Cut(("(" + std::to_string(lower) + "< BDT1) && (BDT1 < " + std::to_string(upper) + ")").c_str());
        loader_temp.PrintInformation(("========== " + std::to_string(lower) + " < BDT1 < " + std::to_string(upper) + " ==========").c_str());

        loader_temp.FillTH1D(temp_th1d, "M");

        loader_temp.end();
    }

    for (auto& temp_hist : M_BDT_binning) {
        temp_hist->Scale(1.0 / temp_hist->Integral());
    }

    std::vector<int> colors = {kBlue + 1, kRed + 1, kGreen + 2, kOrange + 7, kMagenta + 1, kCyan + 2, kViolet + 1, kGray + 2 };
    std::vector<int> fillStyles = { 3004, 3005, 3006, 3007, 3013, 3014, 3021, 3022 };

    TCanvas* canvas_M_BDT_diff = new TCanvas("canvas_M_BDT_diff", "canvas_M_BDT_diff", 800, 800);
    TLegend* leg_M_BDT_diff = new TLegend(0.65, 0.70, 0.88, 0.88);

    double maxY = 0.0;
    for (int i = 0; i < BDT_bins.size(); i++) {
        TH1D* temp_hist = M_BDT_binning.at(i);
        if (temp_hist->GetMaximum() > maxY) maxY = temp_hist->GetMaximum();
    }
    for (int i = 0; i < BDT_bins.size(); i++) {
        double lower = BDT_bins.at(i).first;
        double upper = BDT_bins.at(i).second;

        canvas_M_BDT_diff->cd();

        TH1D* temp_hist = M_BDT_binning.at(i);
        temp_hist->SetLineColor(colors.at(i));
        temp_hist->SetLineWidth(2);
        temp_hist->SetFillStyle(fillStyles.at(i));

        if (i == 0) {
            temp_hist->SetMaximum(1.15 * maxY);
            temp_hist->Draw("HIST");
        }
        else temp_hist->Draw("HIST SAME");

        leg_M_BDT_diff->AddEntry(temp_hist, ("[" + std::to_string(lower) + ", " + std::to_string(upper) + "] GeV/c^{2}").c_str(), "l");
    }
    leg_M_BDT_diff->SetBorderSize(0);
    leg_M_BDT_diff->SetFillStyle(0);
    leg_M_BDT_diff->Draw();

    canvas_M_BDT_diff->Update();
    canvas_M_BDT_diff->SaveAs((argv[6] + std::string("/") + "BDT1_M_diff_distribution.png").c_str());

    // print result
    printf("weighted pearson correlation between BDT1 and M on (-20 delta < M < 20 delta) && (-5 delta < deltaE < 5 delta): %lf\n", dataset.correlation(BDT1, M_inv));

    return 0;
}
