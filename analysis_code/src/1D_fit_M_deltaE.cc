#include <stdio.h>
#include <string>
#include <vector>
#include <map>

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

// https://gitlab.desy.de/belle2/publications/73/tau_eellell/-/blob/main/processing/SignalRegion_fit.py

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
    deltaE.setRange("peak", -0.03, 0.02);

    Module::Module* temp_module = new Module::FillDataSet(&dataset, { &M_inv, &deltaE }, { "M_inv_tau", "deltaE" }, loader.Getvariable_names_address(), loader.VariableTypes_address());
    loader.InsertCustomizedModule(temp_module);

    loader.end();

    // M fit
    RooDataSet* dataset_M = (RooDataSet*)dataset.reduce(RooArgSet(M_inv));
    RooRealVar mean_M("mean_M", "mean_M", 1.777, 1.767, 1.787);
    RooRealVar sigma_left_M("sigma_left_M", "sigma_left_M", 0.0048, 0.0038, 0.0058);
    RooRealVar sigma_right_M("sigma_right_M", "sigma_right_M", 0.0048, 0.0038, 0.0058);

    RooBifurGauss bifurcated_M("bifurcated_M", "bifurcated_M", M_inv, mean_M, sigma_left_M, sigma_right_M);

    RooFitResult* result_M = bifurcated_M.fitTo(*dataset_M, RooFit::Save(), RooFit::Strategy(2), RooFit::SumW2Error(true), RooFit::Range("peak"));

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
    TPad* pad1 = new TPad("pad1", "pad1", 0.0, 0.3, 1.0, 1.0);
    pad1->SetBottomMargin(0.05); pad1->SetLeftMargin(0.15); pad1->SetGridx(); pad1->Draw(); pad1->cd();
    M_inv_frame->GetXaxis()->SetLabelSize(0); M_inv_frame->GetXaxis()->SetTitleSize(0);
    M_inv_frame->Draw();
    TLegend* legend = pad1->BuildLegend(0.1, 0.7, 0.3, 0.9);
    legend->SetFillStyle(0); legend->SetLineWidth(0);

    c_M->cd();
    TPad* pad2 = new TPad("pad2", "pad2", 0.0, 0.0, 1, 0.3);
    pad2->SetTopMargin(0.05); pad2->SetBottomMargin(0.3); pad2->SetLeftMargin(0.15); pad2->SetGridx(); pad2->Draw(); pad2->cd();
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

    // plot deltaE fit
    RooPlot* deltaE_frame = deltaE.frame(RooFit::Bins(200), RooFit::Title(" "));
    dataset_deltaE->plotOn(deltaE_frame, RooFit::DataError(RooAbsData::SumW2));
    bifurcated_deltaE.plotOn(deltaE_frame, RooFit::LineColor(kRed), RooFit::LineStyle(kDashed), RooFit::Range("full"), RooFit::NormRange("peak"));
    bifurcated_deltaE.plotOn(deltaE_frame, RooFit::LineColor(kBlue), RooFit::LineStyle(kSolid), RooFit::Range("peak"), RooFit::NormRange("peak"));

    TCanvas* c_deltaE = new TCanvas("canvas_deltaE_fit", "canvas_deltaE_fit", 800, 800);
    deltaE_frame->Draw();
    c_deltaE->SaveAs((std::string(argv[2]) + "/deltaE_fit.png").c_str());
    delete c_deltaE;

    // save result
    FILE* fp = fopen((std::string(argv[2]) + "/1D_M_deltaE_result.txt").c_str(), "w");
    fprintf(fp, "%lf %lf %lf %d\n", mean_M.getVal(), sigma_left_M.getVal(), sigma_right_M.getVal(), result_M->status());
    fprintf(fp, "%lf %lf %lf %d\n", mean_deltaE.getVal(), sigma_left_deltaE.getVal(), sigma_right_deltaE.getVal(), result_deltaE->status());
    fclose(fp);

    return 0;
}