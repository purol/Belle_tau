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
    * argv[5]: input path
    * argv[6]: output path
    * argv[7]: output name
    */

    std::string variable_name(argv[1]);

    std::vector<std::string> signal_list = { "SIGNAL" };
    std::vector<std::string> background_list = { "CHARM", "CHG", "DDBAR", "EE", "EEEE", 
        "EEKK", "EEMUMU", "EEPIPI", "EEPP", "EETAUTAU", "GG", 
        "K0K0BARISR", "KKISR", "MIX", "MUMU", "MUMUMUMU", 
        "MUMUTAUTAU", "PIPIISR", "SSBAR", "TAUPAIR", "TAUTAUTAUTAU", "UUBAR" };

    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution((std::string(argv[5]) + "/M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    ObtainWeight = MyScaleFunction_halfsplit;

    // define TH1D
    TH1D* signal_train_th = new TH1D("signal_train_th", ("signal train;" + variable_name + ";arbitrary unit").c_str(), atoi(argv[2]), atof(argv[3]), atof(argv[4]));
    TH1D* signal_test_th = new TH1D("signal_test_th", ("signal test;" + variable_name + ";arbitrary unit").c_str(), atoi(argv[2]), atof(argv[3]), atof(argv[4]));
    TH1D* background_train_th = new TH1D("background_train_th", ("bkg train;" + variable_name + ";arbitrary unit").c_str(), atoi(argv[2]), atof(argv[3]), atof(argv[4]));
    TH1D* background_test_th = new TH1D("background_test_th", ("bkg test;" + variable_name + ";arbitrary unit").c_str(), atoi(argv[2]), atof(argv[3]), atof(argv[4]));

    // define another TH1D, which is used for KS test. In principle, KS test cannot be used for binned data. To be close to the exact result, we use fine bin width here
    TH1D* signal_train_th_KS = new TH1D("signal_train_th_KS", ("signal train;" + variable_name + ";arbitrary unit").c_str(), 100 * atoi(argv[2]), atof(argv[3]), atof(argv[4]));
    TH1D* signal_test_th_KS = new TH1D("signal_test_th_KS", ("signal test;" + variable_name + ";arbitrary unit").c_str(), 100 * atoi(argv[2]), atof(argv[3]), atof(argv[4]));
    TH1D* background_train_th_KS = new TH1D("background_train_th_KS", ("bkg train;" + variable_name + ";arbitrary unit").c_str(), 100 * atoi(argv[2]), atof(argv[3]), atof(argv[4]));
    TH1D* background_test_th_KS = new TH1D("background_test_th_KS", ("bkg test;" + variable_name + ";arbitrary unit").c_str(), 100 * atoi(argv[2]), atof(argv[3]), atof(argv[4]));

    // signal train
    Loader loader_signal_train("tau_lfv");
    for (int i = 0; i < signal_list.size(); i++) loader_signal_train.Load((argv[5] + std::string("/") + signal_list.at(i) + std::string("/final_output_train_after_application/")).c_str(), "root", signal_list.at(i).c_str());
    loader_signal_train.Cut(("(deltaE < " + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + ")").c_str());
    loader_signal_train.Cut(("(" + std::to_string(M_peak - 3 * M_left_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak + 3 * M_right_sigma) + ")").c_str());
    loader_signal_train.FillTH1D(signal_train_th, variable_name);
    loader_signal_train.FillTH1D(signal_train_th_KS, variable_name);
    loader_signal_train.end();

    // signal test
    Loader loader_signal_test("tau_lfv");
    for (int i = 0; i < signal_list.size(); i++) loader_signal_test.Load((argv[5] + std::string("/") + signal_list.at(i) + std::string("/final_output_test_after_application/")).c_str(), "root", signal_list.at(i).c_str());
    loader_signal_test.Cut(("(deltaE < " + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + ")").c_str());
    loader_signal_test.Cut(("(" + std::to_string(M_peak - 3 * M_left_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak + 3 * M_right_sigma) + ")").c_str());
    loader_signal_test.FillTH1D(signal_test_th, variable_name);
    loader_signal_test.FillTH1D(signal_test_th_KS, variable_name);
    loader_signal_test.end();

    // background train
    Loader loader_background_train("tau_lfv");
    for (int i = 0; i < background_list.size(); i++) loader_background_train.Load((argv[5] + std::string("/") + background_list.at(i) + std::string("/final_output_train_after_application/")).c_str(), "root", background_list.at(i).c_str());
    loader_background_train.Cut(("(deltaE < " + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + ")").c_str());
    loader_background_train.Cut(("(" + std::to_string(M_peak - 3 * M_left_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak + 3 * M_right_sigma) + ")").c_str());
    loader_background_train.FillTH1D(background_train_th, variable_name);
    loader_background_train.FillTH1D(background_train_th_KS, variable_name);
    loader_background_train.end();

    // background test
    Loader loader_background_test("tau_lfv");
    for (int i = 0; i < background_list.size(); i++) loader_background_test.Load((argv[5] + std::string("/") + background_list.at(i) + std::string("/final_output_test_after_application/")).c_str(), "root", background_list.at(i).c_str());
    loader_background_test.Cut(("(deltaE < " + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + ")").c_str());
    loader_background_test.Cut(("(" + std::to_string(M_peak - 3 * M_left_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak + 3 * M_right_sigma) + ")").c_str());
    loader_background_test.FillTH1D(background_test_th, variable_name);
    loader_background_test.FillTH1D(background_test_th_KS, variable_name);
    loader_background_test.end();


    // draw and KS test
    double factor = 1.0;

    signal_train_th->Scale(factor / signal_train_th->Integral(), "width");
    signal_test_th->Scale(factor / signal_test_th->Integral(), "width");
    background_train_th->Scale(factor / background_train_th->Integral(), "width");
    background_test_th->Scale(factor / background_test_th->Integral(), "width");

    // set color (BKG: kRed, SIGNAL: kBlue)
    // solid line: test, histogram: traing
    signal_train_th->SetFillStyle(3004);
    signal_train_th->SetLineColor(kBlue);
    signal_train_th->SetFillColor(kBlue);

    signal_test_th->SetMarkerStyle(kFullCircle);
    signal_test_th->SetLineColor(kBlue);
    signal_test_th->SetMarkerColor(kBlue);
    signal_test_th->SetLineWidth(1);

    background_train_th->SetFillStyle(3005);
    background_train_th->SetLineColor(kRed);
    background_train_th->SetFillColor(kRed);

    background_test_th->SetMarkerStyle(kFullCircle);
    background_test_th->SetLineColor(kRed);
    background_test_th->SetMarkerColor(kRed);
    background_test_th->SetLineWidth(1);

    double p_value_signal = signal_test_th_KS->KolmogorovTest(signal_train_th_KS);
    double p_value_background = background_test_th_KS->KolmogorovTest(background_train_th_KS);

    gStyle->SetOptStat(0);

    TCanvas* c_temp = new TCanvas("c", "", 600, 600); c_temp->cd();

    double background_train_th_max = background_train_th->GetMaximum();
    double signal_train_th_max = signal_train_th->GetMaximum();

    if (background_train_th_max > signal_train_th_max) background_train_th->SetMaximum(1.40 * background_train_th_max);
    else background_train_th->SetMaximum(1.40 * signal_train_th_max);

    background_train_th->SetTitle(""); signal_train_th->SetTitle("");
    background_test_th->SetTitle(""); signal_test_th->SetTitle("");

    background_train_th->Draw("Hist"); signal_train_th->Draw("HistSAME");
    background_test_th->Draw("AP SAME"); signal_test_th->Draw("AP SAME");

    TLegend* legend = new TLegend(0.9, 0.9, 0.6, 0.6);
    legend->AddEntry(signal_train_th, "signal train", "f");
    legend->AddEntry(signal_test_th, "signal test", "lpe");
    legend->AddEntry(background_train_th, "background train", "f");
    legend->AddEntry(background_test_th, "background test", "lpe");
    legend->SetFillStyle(0); legend->SetLineWidth(0);
    legend->Draw();

    TLatex latex_pvalue;
    latex_pvalue.SetNDC();
    latex_pvalue.SetTextSize(0.04);
    latex_pvalue.DrawLatex(0.15, 0.85, ("p-value = " + toStringWithPrecision(p_value_signal, 3) + " (signal)").c_str());
    latex_pvalue.DrawLatex(0.15, 0.8, ("p-value = " + toStringWithPrecision(p_value_background, 3) + " (bkg)").c_str());

    c_temp->SaveAs((std::string(argv[6]) + "/" + std::string(argv[7])).c_str());

    return 0;
}
