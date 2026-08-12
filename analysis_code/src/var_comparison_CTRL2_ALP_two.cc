#include <stdio.h>
#include <string>
#include <vector>
#include <format>
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
    * argv[1]: variable name
    * argv[2]: bin number
    * argv[3]: min value
    * argv[4]: max value
    * argv[5]: input1 path
    * argv[6]: input2 path
    * argv[7]: output path
    * argv[8]: output name
    * argv[9]: sample1 list (separated by colon)
    * argv[10]: sample2 list (separated by colon)
    * argv[11]: sample1 lable
    * argv[12]: sample2 lable
    * argv[13]: {none|ratio}
    * argv[14]: M_deltaE path for tau -> a mu decay
    * argv[15]: mass
    * argv[16]: lifetime
    * argv[17]: A constant
    * argv[18]: B constant
    */

    bool ThereIsRatio = false;
    if(std::string(argv[13]) == "") ThereIsRatio = false;
    else if(std::string(argv[13]) == "none") ThereIsRatio = false;
    if(std::string(argv[13]) == "ratio") ThereIsRatio = true;

    double mass = std::stod(argv[15]);
    double life = std::stod(argv[16]);
    int A = std::stoi(argv[17]);
    int B = std::stoi(argv[18]);

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

    ReadResolution((std::string(argv[14]) + "/alpha_mass" + std::format("{:g}", mass) + "_life" + std::format("{:g}", life) + "_A" + std::to_string(A) + "_B" + std::to_string(B) + "_M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    std::string variable_name(argv[1]);

    std::vector<std::string> sample1_list = split(argv[9], ':');
    std::vector<std::string> sample2_list = split(argv[10], ':');

    // define TH1D
    TH1D* sample1_test_th = new TH1D("sample1_test_th", ("sample1 test;" + variable_name + ";arbitrary unit").c_str(), atoi(argv[2]), atof(argv[3]), atof(argv[4]));
    TH1D* sample2_test_th = new TH1D("sample2_test_th", ("sample2 test;" + variable_name + ";arbitrary unit").c_str(), atoi(argv[2]), atof(argv[3]), atof(argv[4]));

    // define another TH1D, which is used for KS test. In principle, KS test cannot be used for binned data. To be close to the exact result, we use fine bin width here
    TH1D* sample1_test_th_KS = new TH1D("sample1_test_th_KS", ("sample1 test;" + variable_name + ";arbitrary unit").c_str(), 100 * atoi(argv[2]), atof(argv[3]), atof(argv[4]));
    TH1D* sample2_test_th_KS = new TH1D("sample2_test_th_KS", ("sample2 test;" + variable_name + ";arbitrary unit").c_str(), 100 * atoi(argv[2]), atof(argv[3]), atof(argv[4]));

    // sample1 test (Here, we assume it is tau -> pi pi pi nu with background populated region)
    Loader loader_sample1_test("tau_lfv");
    for (int i = 0; i < sample1_list.size(); i++) loader_sample1_test.Load(argv[5], "root", sample1_list.at(i).c_str());
    loader_sample1_test.Cut(("(" + std::to_string(deltaE_peak - 16 * deltaE_left_sigma) + "< deltaE) && (deltaE < " + std::to_string(deltaE_peak + 6 * deltaE_right_sigma) + ")").c_str());
    loader_sample1_test.Cut(("(" + std::to_string(M_peak - 20 * M_left_sigma) + "< M) && (M < " + std::to_string(M_peak + 20 * M_right_sigma) + ")").c_str());
    // loader_sample1_test.Cut(("(" + std::to_string(mass - M_left_cut_value) + "< myM_ALP) && (myM_ALP <" + std::to_string(mass + M_right_cut_value) + ")").c_str());
    loader_sample1_test.RandomBCS();
    loader_sample1_test.IsBCSValid();
    loader_sample1_test.Cut(("(" + std::to_string(deltaE_peak - 15 * deltaE_left_sigma) + "< deltaE) && (deltaE < " + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + ")").c_str());
    loader_sample1_test.Cut(("(" + std::to_string(M_peak - 5 * M_left_sigma) + "< M) && (M < " + std::to_string(M_peak + 5 * M_right_sigma) + ")").c_str());
    loader_sample1_test.FillTH1D(sample1_test_th, variable_name);
    loader_sample1_test.FillTH1D(sample1_test_th_KS, variable_name);
    loader_sample1_test.end();

    // sample2 test (Here, we assume it is tau -> a mu)
    Loader loader_sample2_test("tau_lfv");
    for (int i = 0; i < sample2_list.size(); i++) loader_sample2_test.Load(argv[6], "root", sample2_list.at(i).c_str());
    loader_sample2_test.Cut(("(" + std::to_string(deltaE_peak - 16 * deltaE_left_sigma) + "< deltaE) && (deltaE < " + std::to_string(deltaE_peak + 6 * deltaE_right_sigma) + ")").c_str());
    loader_sample2_test.Cut(("(" + std::to_string(M_peak - 20 * M_left_sigma) + "< M) && (M < " + std::to_string(M_peak + 20 * M_right_sigma) + ")").c_str());
    loader_sample2_test.Cut(("(" + std::to_string(mass - M_left_cut_value) + "< extraInfo__boALP_M__bc) && (extraInfo__boALP_M__bc <" + std::to_string(mass + M_right_cut_value) + ")").c_str());
    loader_sample2_test.RandomBCS();
    loader_sample2_test.IsBCSValid();
    loader_sample2_test.Cut(("(" + std::to_string(deltaE_peak - 15 * deltaE_left_sigma) + "< deltaE) && (deltaE < " + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + ")").c_str());
    loader_sample2_test.Cut(("(" + std::to_string(M_peak - 5 * M_left_sigma) + "< M) && (M < " + std::to_string(M_peak + 5 * M_right_sigma) + ")").c_str());
    loader_sample2_test.FillTH1D(sample2_test_th, variable_name);
    loader_sample2_test.FillTH1D(sample2_test_th_KS, variable_name);
    loader_sample2_test.end();


    // draw and KS test
    double factor = 1.0;

    sample1_test_th->Scale(factor / sample1_test_th->Integral(), "width");
    sample2_test_th->Scale(factor / sample2_test_th->Integral(), "width");

    // set color (sample1: kRed, sample2: kBlue)
    sample1_test_th->SetFillStyle(3004);
    sample1_test_th->SetLineColor(kBlue);
    sample1_test_th->SetFillColor(kBlue);

    sample2_test_th->SetFillStyle(3005);
    sample2_test_th->SetLineColor(kRed);
    sample2_test_th->SetFillColor(kRed);

    double p_value = sample1_test_th_KS->KolmogorovTest(sample2_test_th_KS);

    gStyle->SetOptStat(0);

    // draw plot
    if(ThereIsRatio){
        TCanvas* c_temp = new TCanvas("c", "", 800, 800); c_temp->cd();

        TPad* pad1 = new TPad("pad1", "pad1", 0.0, 0.3, 1.0, 1.0);
        pad1->SetBottomMargin(0.05); pad1->SetLeftMargin(0.15); pad1->SetGridx(); pad1->Draw(); pad1->cd();

        double sample1_th_max = sample1_test_th->GetMaximum();
        double sample2_th_max = sample2_test_th->GetMaximum();

        if (sample1_th_max > sample2_th_max) sample2_test_th->SetMaximum(1.40 * sample1_th_max);
        else sample2_test_th->SetMaximum(1.40 * sample2_th_max);

        sample2_test_th->SetTitle(""); sample1_test_th->SetTitle("");

        sample2_test_th->Draw("Hist"); sample1_test_th->Draw("HistSAME");

        TLegend* legend = new TLegend(0.9, 0.9, 0.6, 0.6);
        legend->AddEntry(sample1_test_th, argv[11], "f");
        legend->AddEntry(sample2_test_th, argv[12], "f");
        legend->SetFillStyle(0); legend->SetLineWidth(0);
        legend->Draw();

        TLatex latex_pvalue;
        latex_pvalue.SetNDC();
        latex_pvalue.SetTextSize(0.04);
        latex_pvalue.DrawLatex(0.15, 0.85, ("p-value = " + toStringWithPrecision(p_value, 3)).c_str());

        c_temp->cd();
        TPad* pad2 = new TPad("pad2", "pad2", 0.0, 0.0, 1, 0.3);
        pad2->SetTopMargin(0.05); pad2->SetBottomMargin(0.3); pad2->SetLeftMargin(0.15); pad2->SetGridx(); pad2->Draw(); pad2->cd();

        TH1D* ratio_th = new TH1D("ratio", (";" + variable_name + ";ratio").c_str(), atoi(argv[2]), atof(argv[3]), atof(argv[4]));
        ratio_th->Divide(sample1_test_th, sample2_test_th);
        ratio_th->Draw();

        c_temp->SetBottomMargin(0.0);
        c_temp->SaveAs((std::string(argv[7]) + "/" + std::string(argv[8])).c_str());
    }
    else{
        TCanvas* c_temp = new TCanvas("c", "", 800, 800); c_temp->cd();

        double sample1_th_max = sample1_test_th->GetMaximum();
        double sample2_th_max = sample2_test_th->GetMaximum();

        if (sample1_th_max > sample2_th_max) sample2_test_th->SetMaximum(1.40 * sample1_th_max);
        else sample2_test_th->SetMaximum(1.40 * sample2_th_max);

        sample2_test_th->SetTitle(""); sample1_test_th->SetTitle("");

        sample2_test_th->Draw("Hist"); sample1_test_th->Draw("HistSAME");

        TLegend* legend = new TLegend(0.9, 0.9, 0.6, 0.6);
        legend->AddEntry(sample1_test_th, argv[11], "f");
        legend->AddEntry(sample2_test_th, argv[12], "f");
        legend->SetFillStyle(0); legend->SetLineWidth(0);
        legend->Draw();

        TLatex latex_pvalue;
        latex_pvalue.SetNDC();
        latex_pvalue.SetTextSize(0.04);
        latex_pvalue.DrawLatex(0.15, 0.85, ("p-value = " + toStringWithPrecision(p_value, 3)).c_str());

        c_temp->SaveAs((std::string(argv[7]) + "/" + std::string(argv[8])).c_str());
    }

    return 0;
}
