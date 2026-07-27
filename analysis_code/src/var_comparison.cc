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
    */

    std::string variable_name(argv[1]);

    std::vector<std::string> sample1_list = split(argv[9], ':');
    std::vector<std::string> sample2_list = split(argv[10], ':');

    // define TH1D
    TH1D* sample1_test_th = new TH1D("sample1_test_th", ("sample1 test;" + variable_name + ";arbitrary unit").c_str(), atoi(argv[2]), atof(argv[3]), atof(argv[4]));
    TH1D* sample2_test_th = new TH1D("sample2_test_th", ("sample2 test;" + variable_name + ";arbitrary unit").c_str(), atoi(argv[2]), atof(argv[3]), atof(argv[4]));

    // define another TH1D, which is used for KS test. In principle, KS test cannot be used for binned data. To be close to the exact result, we use fine bin width here
    TH1D* sample1_test_th_KS = new TH1D("sample1_test_th_KS", ("sample1 test;" + variable_name + ";arbitrary unit").c_str(), 100 * atoi(argv[2]), atof(argv[3]), atof(argv[4]));
    TH1D* sample2_test_th_KS = new TH1D("sample2_test_th_KS", ("sample2 test;" + variable_name + ";arbitrary unit").c_str(), 100 * atoi(argv[2]), atof(argv[3]), atof(argv[4]));

    // sample1 test
    Loader loader_sample1_test("tau_lfv");
    for (int i = 0; i < sample1_list.size(); i++) loader_sample1_test.Load(argv[5], "root", sample1_list.at(i).c_str());
    loader_sample1_test.FillTH1D(sample1_test_th, variable_name);
    loader_sample1_test.FillTH1D(sample1_test_th_KS, variable_name);
    loader_sample1_test.end();

    // sample2 test
    Loader loader_sample2_test("tau_lfv");
    for (int i = 0; i < sample2_list.size(); i++) loader_sample2_test.Load(argv[6], "root", sample2_list.at(i).c_str());
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

    TCanvas* c_temp = new TCanvas("c", "", 600, 600); c_temp->cd();

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

    return 0;
}
