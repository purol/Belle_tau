#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "MyModule.h"

#include "TCanvas.h"
#include "RooWorkspace.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooArgSet.h"
#include "RooBifurGauss.h"'
#include "RooExtendPdf.h"
#include "RooFitResult.h"

int main(int argc, char* argv[]) {
    /*
    * argv[1]: dirname
    * argv[2]: output path
    */

    ObtainWeight = MyScaleFunction_halfsplit;

    Loader loader("tau_lfv");

    loader.Load(argv[1], "root");

    loader.Cut("(-0.3 < deltaE) && (deltaE < 0.15)");
    loader.PrintInformation("========== -0.3 < deltaE < 0.15 ==========");
    loader.Cut("(1.73 < M_inv_tau) && (M_inv_tau < 1.81)");
    loader.PrintInformation("========== 1.73 < M < 1.81 ==========");

    RooWorkspace* w = new RooWorkspace("ws", "ws");
    RooRealVar M_inv("M_inv", "M_inv", 1.73, 1.81);
    RooRealVar deltaE("deltaE", "deltaE", -0.3, 0.15);
    RooRealVar weight("weight", "weight", 0.0, 1.0);
    RooDataSet dataset("dataset", "dataset", RooArgSet(M_inv, deltaE, weight), WeightVar("weight"));

    Module::Module* temp_module = new Module::FillDataSet(&dataset, { &M_inv, &deltaE }, { "M_inv_tau", "deltaE" }, loader.Getvariable_names_address(), loader.VariableTypes_address());
    loader.InsertCustomizedModule(temp_module);

    loader.end();

    // M fit
    RooDataSet* dataset_M = (RooDataSet*)dataset.reduce(RooArgSet(M_inv));
    RooRealVar mean_M("mean_M", "mean_M", 1.77, 1.75, 1.80);
    RooRealVar sigma_left_M("sigma_left_M", "sigma_left_M", 0.0048, 0.0038, 0.0058);
    RooRealVar sigma_right_M("sigma_right_M", "sigma_right_M", 0.0048, 0.0038, 0.0058);

    RooBifurGauss bifurcated_M("bifurcated_M", "bifurcated_M", M_inv, mean_M, sigma_left_M, sigma_right_M);
    RooRealVar nevt_M("nevt_M", "number of events", 4.0, 0.0, 10.0);
    RooExtendPdf e_bifurcated_M("e_bifurcated_M", "extended bifurcated_M", bifurcated_M, nevt_M);

    RooFitResult* result_M = e_bifurcated_M.fitTo(*dataset_M, RooFit::Save(), RooFit::SumW2Error(true), RooFit::Range(1.770, 1.786));

    // plot M fit
    RooPlot* M_inv_frame = M_inv.frame(Bins(100), Title(" "));
    dataset_M->plotOn(M_inv_frame, RooFit::DataError(RooAbsData::SumW2));
    e_bifurcated_M.plotOn(M_inv_frame, RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed));

    TCanvas* c_M = new TCanvas("canvas_M_fit", "canvas_M_fit", 800, 800);
    M_inv_frame->Draw();
    c->SaveAs((std::string(argv[2]) + "/M_fit.png").c_str());
    delete c_M;

    // deltaE fit
    RooDataSet* dataset_deltaE = (RooDataSet*)dataset.reduce(RooArgSet(deltaE));
    RooRealVar mean_deltaE("mean_deltaE", "mean_deltaE", 0.0, -0.1, 0.1);
    RooRealVar sigma_left_deltaE("sigma_left_deltaE", "sigma_left_deltaE", 0.014, 0.008, 0.020);
    RooRealVar sigma_right_deltaE("sigma_right_deltaE", "sigma_right_deltaE", 0.014, 0.008, 0.020);

    RooBifurGauss bifurcated_deltaE("bifurcated_deltaE", "bifurcated_deltaE", M_inv, mean_deltaE, sigma_left_deltaE, sigma_right_deltaE);
    RooRealVar nevt_deltaE("nevt_deltaE", "number of events", 4.0, 0.0, 10.0);
    RooExtendPdf e_bifurcated_deltaE("e_bifurcated_deltaE", "extended bifurcated_deltaE", bifurcated_deltaE, nevt_deltaE);

    RooFitResult* result_deltaE = e_bifurcated_deltaE.fitTo(*dataset_deltaE, RooFit::Save(), RooFit::SumW2Error(true), RooFit::Range(-0.025, 0.025));

    // plot deltaE fit
    RooPlot* deltaE_frame = deltaE.frame(Bins(100), Title(" "));
    dataset_deltaE->plotOn(deltaE_frame, RooFit::DataError(RooAbsData::SumW2));
    e_bifurcated_deltaE.plotOn(deltaE_frame, RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed));

    TCanvas* c_deltaE = new TCanvas("canvas_deltaE_fit", "canvas_deltaE_fit", 800, 800);
    deltaE_frame->Draw();
    c->SaveAs((std::string(argv[2]) + "/deltaE_fit.png").c_str());
    delete c_deltaE;

    // save result
    FILE* fp = fopen((std::string(argv[2]) + "/1D_M_deltaE_result.txt").c_str(), "w");
    fprintf("%lf %lf %lf\n", mean_M.getVal(), sigma_left_M.getVal(), sigma_right_M.getVal());
    fprintf("%lf %lf %lf\n", mean_deltaE.getVal(), sigma_left_deltaE.getVal(), sigma_right_deltaE.getVal());
    fclose(fp);

    return 0;
}