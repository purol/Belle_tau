#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "MyModule.h"

#include <TCanvas.h>
#include <RooWorkspace.h>
#include <RooDataSet.h>
#include <RooRealVar.h>
#include <RooArgSet.h>
#include <RooBifurGauss.h>
#include <RooExtendPdf.h>
#include <RooFitResult.h>
#include <RooPlot.h>
#include <RooHist.h>
#include <TPad.h>
#include <TAxis.h>
#include <TLatex.h>
#include <TProfile.h>
#include <TF1.h>

// https://gitlab.desy.de/belle2/publications/73/tau_eellell/-/blob/main/processing/SignalRegion_fit.py

std::string toStringWithPrecision(double value, int precision) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

int main(int argc, char* argv[]) {
    /*
    * argv[1]: dirname
    * argv[2]: output path
    */

    ObtainWeight = MyScaleFunction_halfsplit;

    Loader loader("tau_lfv");

    loader.Load(argv[1], "root", "SIGNAL");

    loader.Cut("(-0.3 < deltaE) && (deltaE < 0.15)");
    loader.PrintInformation("========== -0.3 < deltaE < 0.15 ==========");
    loader.Cut("(1.71 < M_inv_tau) && (M_inv_tau < 1.82)");
    loader.PrintInformation("========== 1.71 < M < 1.82 ==========");

    RooRealVar M_inv("M_inv", "M_inv", 1.71, 1.82);
    RooRealVar deltaE("deltaE", "deltaE", -0.3, 0.15);
    RooRealVar weight("weight", "weight", 0.0, 1.0);
    RooDataSet dataset("dataset", "dataset", RooArgSet(M_inv, deltaE, weight), RooFit::WeightVar("weight"));

    // set range
    M_inv.setRange("full", 1.71, 1.82);
    M_inv.setRange("peak", 1.77, 1.785);
    deltaE.setRange("full", -0.3, 0.15);
    deltaE.setRange("peak", -0.02, 0.02);

    Module::Module* temp_module = new Module::FillDataSet(&dataset, { &M_inv, &deltaE }, { "M_inv_tau", "deltaE" }, loader.Getvariable_names_address(), loader.VariableTypes_address());
    loader.InsertCustomizedModule(temp_module);

    TProfile* deltaE_M_profile = new TProfile("hprof", "Profile of deltaE versus M", 100, -0.3, 0.15, 1.71, 1.82);
    Module::Module* temp_module_2 = new Module::FillTProfile(deltaE_M_profile, "deltaE", "M_inv_tau", loader.Getvariable_names_address(), loader.VariableTypes_address());
    loader.InsertCustomizedModule(temp_module_2);

    loader.end();

    // M fit
    RooDataSet* dataset_M = (RooDataSet*)dataset.reduce(RooArgSet(M_inv));
    RooRealVar mean_M("mean_M", "mean_M", 1.777, 1.767, 1.787);
    RooRealVar sigma_left_M("sigma_left_M", "sigma_left_M", 0.0048, 0.0038, 0.0058);
    RooRealVar sigma_right_M("sigma_right_M", "sigma_right_M", 0.0048, 0.0038, 0.0058);

    RooBifurGauss bifurcated_M("bifurcated_M", "bifurcated_M", M_inv, mean_M, sigma_left_M, sigma_right_M);

    RooFitResult* result_M = bifurcated_M.fitTo(*dataset_M, RooFit::Save(), RooFit::Strategy(2), RooFit::SumW2Error(true), RooFit::Range("peak"));
    RooArgSet fitargs_M = result_M->floatParsFinal();
    TIterator* iter_M(fitargs_M.createIterator());
    double mean_M_fit;
    double mean_M_fit_error;
    double sigma_left_M_fit;
    double sigma_left_M_fit_error;
    double sigma_right_M_fit;
    double sigma_right_M_fit_error;
    for (TObject* a = iter_M->Next(); a != 0; a = iter_M->Next()) {
        RooRealVar* rrv = dynamic_cast<RooRealVar*>(a);
        std::string name = rrv->GetName();
        if (name == std::string("mean_M")) {
            mean_M_fit = rrv->getVal();
            mean_M_fit_error = rrv->getError();
        }
        else if (name == std::string("sigma_left_M")) {
            sigma_left_M_fit = rrv->getVal();
            sigma_left_M_fit_error = rrv->getError();
        }
        else if (name == std::string("sigma_right_M")) {
            sigma_right_M_fit = rrv->getVal();
            sigma_right_M_fit_error = rrv->getError();
        }
    }

    // plot M fit
    RooPlot* M_inv_frame = M_inv.frame(RooFit::Bins(200), RooFit::Title(" "));
    dataset_M->plotOn(M_inv_frame, RooFit::DataError(RooAbsData::SumW2), RooFit::Name("signal MC"));
    bifurcated_M.plotOn(M_inv_frame, RooFit::LineColor(kRed), RooFit::LineStyle(kDashed), RooFit::Range("full"), RooFit::NormRange("peak"));
    bifurcated_M.plotOn(M_inv_frame, RooFit::LineColor(kBlue), RooFit::LineStyle(kSolid), RooFit::Range("peak"), RooFit::NormRange("peak"), RooFit::Name("BifurGauss"));

    RooHist* pull_M = M_inv_frame->pullHist("signal MC", "BifurGauss");
    RooPlot* M_inv_pull_frame = M_inv.frame(RooFit::Title(""));
    M_inv_pull_frame->addPlotable(pull_M, "P");

    TCanvas* c_M = new TCanvas("canvas_M_fit", "canvas_M_fit", 800, 800);

    c_M->cd();
    TPad* pad1_M = new TPad("pad1_M", "pad1_M", 0.0, 0.3, 1.0, 1.0);
    pad1_M->SetBottomMargin(0.05); pad1_M->SetLeftMargin(0.15); pad1_M->SetGridx(); pad1_M->Draw(); pad1_M->cd();
    M_inv_frame->GetXaxis()->SetLabelSize(0); M_inv_frame->GetXaxis()->SetTitleSize(0);
    M_inv_frame->Draw();
    TLegend* legend_M = new TLegend(0.2, 0.75, 0.45, 0.85);
    legend_M->AddEntry("signal MC", "signal MC", "lpe");
    legend_M->AddEntry("BifurGauss", "BifurGauss", "l");
    legend_M->SetFillStyle(0); legend_M->SetLineWidth(0);
    legend_M->Draw();
    TLatex latex_M;
    latex_M.SetNDC();
    latex_M.SetTextSize(0.04);
    latex_M.DrawLatex(0.2, 0.7, ("#mu = " + toStringWithPrecision(mean_M_fit, 4) + " #pm " + toStringWithPrecision(mean_M_fit_error, 4) + " [GeV]").c_str());
    latex_M.DrawLatex(0.2, 0.6, ("#delta^{left}_{Gauss} = " + toStringWithPrecision(sigma_left_M_fit * 1000.0, 3) + " #pm " + toStringWithPrecision(sigma_left_M_fit_error * 1000.0, 3) + " [MeV]").c_str());
    latex_M.DrawLatex(0.2, 0.5, ("#delta^{right}_{Gauss} = " + toStringWithPrecision(sigma_right_M_fit * 1000.0, 3) + " #pm " + toStringWithPrecision(sigma_right_M_fit_error * 1000.0, 3) + " [MeV]").c_str());

    c_M->cd();
    TPad* pad2_M = new TPad("pad2_M", "pad2_M", 0.0, 0.0, 1, 0.3);
    pad2_M->SetTopMargin(0.05); pad2_M->SetBottomMargin(0.3); pad2_M->SetLeftMargin(0.15); pad2_M->SetGridx(); pad2_M->Draw(); pad2_M->cd();
    M_inv_pull_frame->GetXaxis()->SetLabelSize(0.1); M_inv_pull_frame->GetXaxis()->SetTitleSize(0.1); M_inv_pull_frame->GetYaxis()->SetTitleOffset(0.4);
    M_inv_pull_frame->GetYaxis()->SetLabelSize(0.1); M_inv_pull_frame->GetYaxis()->SetTitleSize(0.1); M_inv_pull_frame->GetYaxis()->SetTitle("pull"); M_inv_pull_frame->SetTitle("");
    M_inv_pull_frame->Draw();

    c_M->SetBottomMargin(0.0);
    c_M->SaveAs((std::string(argv[2]) + "/M_fit.png").c_str());
    delete c_M;





    // deltaE fit
    RooDataSet* dataset_deltaE = (RooDataSet*)dataset.reduce(RooArgSet(deltaE));
    RooRealVar mean_deltaE("mean_deltaE", "mean_deltaE", 0.0, -0.1, 0.1);
    RooRealVar sigma_left_deltaE("sigma_left_deltaE", "sigma_left_deltaE", 0.014, 0.008, 0.020);
    RooRealVar sigma_right_deltaE("sigma_right_deltaE", "sigma_right_deltaE", 0.014, 0.008, 0.020);

    RooBifurGauss bifurcated_deltaE("bifurcated_deltaE", "bifurcated_deltaE", deltaE, mean_deltaE, sigma_left_deltaE, sigma_right_deltaE);

    RooFitResult* result_deltaE = bifurcated_deltaE.fitTo(*dataset_deltaE, RooFit::Save(), RooFit::Strategy(2), RooFit::SumW2Error(true), RooFit::Range("peak"));
    RooArgSet fitargs_deltaE = result_deltaE->floatParsFinal();
    TIterator* iter_deltaE(fitargs_deltaE.createIterator());
    double mean_deltaE_fit;
    double mean_deltaE_fit_error;
    double sigma_left_deltaE_fit;
    double sigma_left_deltaE_fit_error;
    double sigma_right_deltaE_fit;
    double sigma_right_deltaE_fit_error;
    for (TObject* a = iter_deltaE->Next(); a != 0; a = iter_deltaE->Next()) {
        RooRealVar* rrv = dynamic_cast<RooRealVar*>(a);
        std::string name = rrv->GetName();
        if (name == std::string("mean_deltaE")) {
            mean_deltaE_fit = rrv->getVal();
            mean_deltaE_fit_error = rrv->getError();
        }
        else if (name == std::string("sigma_left_deltaE")) {
            sigma_left_deltaE_fit = rrv->getVal();
            sigma_left_deltaE_fit_error = rrv->getError();
        }
        else if (name == std::string("sigma_right_deltaE")) {
            sigma_right_deltaE_fit = rrv->getVal();
            sigma_right_deltaE_fit_error = rrv->getError();
        }
    }

    // plot deltaE fit
    RooPlot* deltaE_frame = deltaE.frame(RooFit::Bins(200), RooFit::Title(" "));
    dataset_deltaE->plotOn(deltaE_frame, RooFit::DataError(RooAbsData::SumW2), RooFit::Name("signal MC"));
    bifurcated_deltaE.plotOn(deltaE_frame, RooFit::LineColor(kRed), RooFit::LineStyle(kDashed), RooFit::Range("full"), RooFit::NormRange("peak"));
    bifurcated_deltaE.plotOn(deltaE_frame, RooFit::LineColor(kBlue), RooFit::LineStyle(kSolid), RooFit::Range("peak"), RooFit::NormRange("peak"), RooFit::Name("BifurGauss"));

    RooHist* pull_deltaE = deltaE_frame->pullHist("signal MC", "BifurGauss");
    RooPlot* deltaE_pull_frame = deltaE.frame(RooFit::Title(""));
    deltaE_pull_frame->addPlotable(pull_deltaE, "P");

    TCanvas* c_deltaE = new TCanvas("canvas_deltaE_fit", "canvas_deltaE_fit", 800, 800);

    c_deltaE->cd();
    TPad* pad1_deltaE = new TPad("pad1_deltaE", "pad1_deltaE", 0.0, 0.3, 1.0, 1.0);
    pad1_deltaE->SetBottomMargin(0.05); pad1_deltaE->SetLeftMargin(0.15); pad1_deltaE->SetGridx(); pad1_deltaE->Draw(); pad1_deltaE->cd();
    deltaE_frame->GetXaxis()->SetLabelSize(0); deltaE_frame->GetXaxis()->SetTitleSize(0);
    deltaE_frame->Draw();
    TLegend* legend_deltaE = new TLegend(0.2, 0.75, 0.45, 0.85);
    legend_deltaE->AddEntry("signal MC", "signal MC", "lpe");
    legend_deltaE->AddEntry("BifurGauss", "BifurGauss", "l");
    legend_deltaE->SetFillStyle(0); legend_deltaE->SetLineWidth(0);
    legend_deltaE->Draw();
    TLatex latex_deltaE;
    latex_deltaE.SetNDC();
    latex_deltaE.SetTextSize(0.04);
    latex_deltaE.DrawLatex(0.2, 0.7, ("#mu = " + toStringWithPrecision(mean_deltaE_fit, 4) + " #pm " + toStringWithPrecision(mean_deltaE_fit_error, 4) + " [GeV]").c_str());
    latex_deltaE.DrawLatex(0.2, 0.6, ("#delta^{left}_{Gauss} = " + toStringWithPrecision(sigma_left_deltaE_fit * 1000.0, 3) + " #pm " + toStringWithPrecision(sigma_left_deltaE_fit_error * 1000.0, 3) + " [MeV]").c_str());
    latex_deltaE.DrawLatex(0.2, 0.5, ("#delta^{right}_{Gauss} = " + toStringWithPrecision(sigma_right_deltaE_fit * 1000.0, 3) + " #pm " + toStringWithPrecision(sigma_right_deltaE_fit_error * 1000.0, 3) + " [MeV]").c_str());

    c_deltaE->cd();
    TPad* pad2_deltaE = new TPad("pad2_deltaE", "pad2_deltaE", 0.0, 0.0, 1, 0.3);
    pad2_deltaE->SetTopMargin(0.05); pad2_deltaE->SetBottomMargin(0.3); pad2_deltaE->SetLeftMargin(0.15); pad2_deltaE->SetGridx(); pad2_deltaE->Draw(); pad2_deltaE->cd();
    deltaE_pull_frame->GetXaxis()->SetLabelSize(0.1); deltaE_pull_frame->GetXaxis()->SetTitleSize(0.1); deltaE_pull_frame->GetYaxis()->SetTitleOffset(0.4);
    deltaE_pull_frame->GetYaxis()->SetLabelSize(0.1); deltaE_pull_frame->GetYaxis()->SetTitleSize(0.1); deltaE_pull_frame->GetYaxis()->SetTitle("pull"); deltaE_pull_frame->SetTitle("");
    deltaE_pull_frame->Draw();

    c_deltaE->SaveAs((std::string(argv[2]) + "/deltaE_fit.png").c_str());
    delete c_deltaE;




    // profile fit
    TF1* first_order_poly = new TF1("first_order_poly", "[0]*x+[1]", -0.01, 0.01);
    deltaE_M_profile->Fit(first_order_poly);
    double a1 = first_order_poly->GetParameter(0);
    double a2 = first_order_poly->GetParameter(1);
    printf("%lf %lf\n", a1, a2);

    // plot profile
    TCanvas* c_deltaE_M = new TCanvas("canvas_deltaE_M_fit", "canvas_deltaE_M_fit", 800, 800);
    c_deltaE_M->cd();
    deltaE_M_profile->SetStats(false); deltaE_M_profile->Draw();
    deltaE_M_profile->Draw();
    c_deltaE_M->SaveAs((std::string(argv[2]) + "/deltaE_M_fit.png").c_str());
    delete c_deltaE_M;




    // save result
    FILE* fp = fopen((std::string(argv[2]) + "/1D_M_deltaE_result.txt").c_str(), "w");
    fprintf(fp, "%lf %lf %lf %d\n", mean_M.getVal(), sigma_left_M.getVal(), sigma_right_M.getVal(), result_M->status());
    fprintf(fp, "%lf %lf %lf %d\n", mean_deltaE.getVal(), sigma_left_deltaE.getVal(), sigma_right_deltaE.getVal(), result_deltaE->status());
    fclose(fp);

    return 0;
}