#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "MyModule.h"

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
    */

    ObtainWeight = MyScaleFunction_halfsplit;

    Loader loader("tau_lfv");

    loader.Load(argv[1], "root");

    RooWorkspace* w = new RooWorkspace("ws", "ws");
    RooRealVar M_inv("M_inv", "M_inv", 1.5, 1.9);
    RooRealVar deltaE("deltaE", "deltaE", -0.9, 0.4);
    RooRealVar weight("weight", "weight", 0.0, 1.0);
    RooDataSet dataset("dataset", "dataset", RooArgSet(M_inv, deltaE, weight), WeightVar("weight"));

    Module::Module* temp_module = new Module::FillDataSetWorkspace(w, &dataset, { &M_inv, &deltaE }, &weight, { "M_inv_tau", "deltaE" }, loader.Getvariable_names_address(), loader.VariableTypes_address());
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

    RooFitResult* result_M = e_bifurcated_M.fitTo(*dataset_M, Save(), SumW2Error(true));

    // deltaE fit
    RooDataSet* dataset_deltaE = (RooDataSet*)dataset.reduce(RooArgSet(deltaE));
    RooRealVar mean_deltaE("mean_deltaE", "mean_deltaE", 0.0, -0.1, 0.1);
    RooRealVar sigma_left_deltaE("sigma_left_deltaE", "sigma_left_deltaE", 0.014, 0.008, 0.020);
    RooRealVar sigma_right_deltaE("sigma_right_deltaE", "sigma_right_deltaE", 0.014, 0.008, 0.020);

    RooBifurGauss bifurcated_deltaE("bifurcated_deltaE", "bifurcated_deltaE", M_inv, mean_deltaE, sigma_left_deltaE, sigma_right_deltaE);
    RooRealVar nevt_deltaE("nevt_deltaE", "number of events", 4.0, 0.0, 10.0);
    RooExtendPdf e_bifurcated_deltaE("e_bifurcated_deltaE", "extended bifurcated_deltaE", bifurcated_deltaE, nevt_deltaE);

    RooFitResult* result_deltaE = e_bifurcated_deltaE.fitTo(*dataset_deltaE, Save(), SumW2Error(true));

    return 0;
}