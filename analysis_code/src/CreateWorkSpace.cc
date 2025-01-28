#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>

#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "RooWorkspace.h"

#include "RooStats/HistFactory/Measurement.h"
#include "RooStats/HistFactory/Channel.h"
#include "RooStats/HistFactory/Sample.h"
#include "RooStats/HistFactory/MakeModelAndMeasurementsFast.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "functions.h"
#include "MyModule.h"

double deltaE_peak_g;
double deltaE_left_sigma_g;
double deltaE_right_sigma_g;
double M_peak_g;
double M_left_sigma_g;
double M_right_sigma_g;
double theta_g;

double x_mapping_function(double M_, double deltaE_) {
    if (((M_peak_g - 20.0 * M_left_sigma_g) < M_) && (M_ <= (M_peak_g - 5.0 * M_left_sigma_g))) return 1.0;
    else if (((M_peak_g - 5.0 * M_left_sigma_g) < M_) && (M_ <= (M_peak_g + 5.0 * M_right_sigma_g))) return 2.0;
    else if (((M_peak_g + 5.0 * M_right_sigma_g) < M_) && (M_ <= (M_peak_g + 20.0 * M_right_sigma_g))) return 3.0;
    else return 4.0;
}

double y_mapping_function(double M_, double deltaE_) {
    if (deltaE_ < (deltaE_peak_g - 5 * deltaE_left_sigma_g)) return 1.0;
    else if (deltaE_ < (deltaE_peak_g + 5 * deltaE_right_sigma_g)) return 2.0;
    else return 3.0;
}

int main(int argc, char* argv[]) {
    /*
    * argv[1]: input path 1
    * argv[2]: input path 2
    * argv[3]: output path
    */

    // TH2 list
    /*
    * 
    *  deltaE
    *   ^
    * 3 |
    *   |
    *   |--------------------
    *   |     |       |     |
    * 2 |     |       |     |
    *   |--------------------
    *   |     |       |     |
    * 1 |     |       |     |
    *   |     |       |     |
    *   +---------------------------> M
    *      1      2      3      4
    */
    TH2D* data_th2d = new TH2D("data_th2d", ";M index; deltaE index;", 4, 0.5, 4.5, 3, 0.5, 3.5);
    TH2D* signal_MC_th2d = new TH2D("signal_MC_th2d", ";M index; deltaE index;", 4, 0.5, 4.5, 3, 0.5, 3.5);
    TH2D* bkg_MC_th2d = new TH2D("bkg_MC_th2d", ";M index; deltaE index;", 4, 0.5, 4.5, 3, 0.5, 3.5);

    // TH1 list
    /*
    * 
    *  deltaE
    *   ^
    *   |--------------------
    *   |     |       |     |
    *   |     |   1   |     |
    *   |--------------------
    *   |     |       |     |
    *   |     |       |     |
    *   |     |   2   |     |
    *   +-----------------------> M
    *   
    */
    TH1D* data_th1d = new TH1D("data_th1d", ";bin index;", 2, 0.5, 2.5);
    TH1D* signal_MC_th1d = new TH1D("signal_MC_th1d", ";bin index;", 2, 0.5, 2.5);
    TH1D* bkg_MC_th1d = new TH1D("bkg_MC_th1d", ";bin index;", 2, 0.5, 2.5);

    TH1D* data_th1d_stat_err = new TH1D("data_th1d_stat_err", ";bin index;", 2, 0.5, 2.5);
    TH1D* signal_MC_th1d_stat_err = new TH1D("signal_MC_th1d_stat_err", ";bin index;", 2, 0.5, 2.5);
    TH1D* bkg_MC_th1d_stat_err = new TH1D("bkg_MC_th1d_stat_err", ";bin index;", 2, 0.5, 2.5);

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

    ReadResolution((std::string(argv[1]) + "/M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    deltaE_peak_g = deltaE_peak;
    deltaE_left_sigma_g = deltaE_left_sigma;
    deltaE_right_sigma_g = deltaE_right_sigma;
    M_peak_g = M_peak;
    M_left_sigma_g = M_left_sigma;
    M_right_sigma_g = M_right_sigma;
    theta_g = theta;

    ObtainWeight = MyScaleFunction_halfsplit;

    // data (we do not open the box, so I just use background MC)
    Loader loader_data("tau_lfv");
    for (int i = 0; i < background_list.size(); i++) loader_data.Load((argv[1] + std::string("/") + background_list.at(i) + std::string("/") + std::string(argv[2])).c_str(), "root", background_list.at(i).c_str());
    Module::Module* temp_module_data = new Module::FillCustomizedTH2D(data_th2d, "M_inv_tau", "deltaE", x_mapping_function, y_mapping_function, loader_data.Getvariable_names_address(), loader_data.VariableTypes_address());
    loader_data.InsertCustomizedModule(temp_module_data);
    loader_data.end();

    // signal MC
    Loader loader_signal("tau_lfv");
    for (int i = 0; i < signal_list.size(); i++) loader_signal.Load((argv[1] + std::string("/") + signal_list.at(i) + std::string("/") + std::string(argv[2])).c_str(), "root", signal_list.at(i).c_str());
    Module::Module* temp_module_signal = new Module::FillCustomizedTH2D(signal_MC_th2d, "M_inv_tau", "deltaE", x_mapping_function, y_mapping_function, loader_signal.Getvariable_names_address(), loader_signal.VariableTypes_address());
    loader_signal.InsertCustomizedModule(temp_module_signal);
    loader_signal.end();

    // background MC
    Loader loader_bkg("tau_lfv");
    for (int i = 0; i < background_list.size(); i++) loader_bkg.Load((argv[1] + std::string("/") + background_list.at(i) + std::string("/") + std::string(argv[2])).c_str(), "root", background_list.at(i).c_str());
    Module::Module* temp_module_bkg = new Module::FillCustomizedTH2D(bkg_MC_th2d, "M_inv_tau", "deltaE", x_mapping_function, y_mapping_function, loader_bkg.Getvariable_names_address(), loader_bkg.VariableTypes_address());
    loader_bkg.InsertCustomizedModule(temp_module_bkg);
    loader_bkg.end();



    // TH2D to TH1D
    data_th1d->SetBinContent(1, data_th2d->GetBinContent(2, 2));
    data_th1d->SetBinContent(2, data_th2d->GetBinContent(2, 1));

    signal_MC_th1d->SetBinContent(1, signal_MC_th2d->GetBinContent(2, 2));
    signal_MC_th1d->SetBinContent(2, signal_MC_th2d->GetBinContent(2, 1));
    signal_MC_th1d->SetBinError(1, signal_MC_th2d->GetBinError(2, 2));
    signal_MC_th1d->SetBinError(2, signal_MC_th2d->GetBinError(2, 1));

    // here, we interpolate expected backgrounds from sideband
    bkg_MC_th1d->SetBinContent(1, (bkg_MC_th2d->GetBinContent(1, 2) + bkg_MC_th2d->GetBinContent(3, 2)) / 3.0);
    bkg_MC_th1d->SetBinContent(2, (bkg_MC_th2d->GetBinContent(1, 1) + bkg_MC_th2d->GetBinContent(3, 1)) / 3.0);
    bkg_MC_th1d->SetBinError(1, std::sqrt(bkg_MC_th2d->GetBinError(1, 2) * bkg_MC_th2d->GetBinError(1, 2) + bkg_MC_th2d->GetBinError(3, 2) * bkg_MC_th2d->GetBinError(3, 2)) / 3.0);
    bkg_MC_th1d->SetBinError(2, std::sqrt(bkg_MC_th2d->GetBinError(1, 1) * bkg_MC_th2d->GetBinError(1, 1) + bkg_MC_th2d->GetBinError(3, 1) * bkg_MC_th2d->GetBinError(3, 1)) / 3.0);

    // calculate stat err
    signal_MC_th1d_stat_err->SetBinContent(1, signal_MC_th1d->GetBinError(1) / signal_MC_th1d->GetBinContent(1));
    signal_MC_th1d_stat_err->SetBinContent(2, signal_MC_th1d->GetBinError(2) / signal_MC_th1d->GetBinContent(2));
    bkg_MC_th1d_stat_err->SetBinContent(1, bkg_MC_th1d->GetBinError(1) / bkg_MC_th1d->GetBinContent(1));
    bkg_MC_th1d_stat_err->SetBinContent(2, bkg_MC_th1d->GetBinError(2) / bkg_MC_th1d->GetBinContent(2));


    // print information
    printf("data:\n");
    printf("%lf+-%lf %lf+-%lf %lf+-%lf\n", data_th2d->GetBinContent(1, 2), data_th2d->GetBinError(1, 2), data_th2d->GetBinContent(2, 2), data_th2d->GetBinError(2, 2), data_th2d->GetBinContent(3, 2), data_th2d->GetBinError(3, 2));
    printf("%lf+-%lf %lf+-%lf %lf+-%lf\n", data_th2d->GetBinContent(1, 1), data_th2d->GetBinError(1, 1), data_th2d->GetBinContent(2, 1), data_th2d->GetBinError(2, 1), data_th2d->GetBinContent(3, 1), data_th2d->GetBinError(3, 1));

    printf("\n");

    printf("signal:\n");
    printf("%lf+-%lf %lf+-%lf %lf+-%lf\n", signal_MC_th2d->GetBinContent(1, 2), signal_MC_th2d->GetBinError(1, 2), signal_MC_th2d->GetBinContent(2, 2), signal_MC_th2d->GetBinError(2, 2), signal_MC_th2d->GetBinContent(3, 2), signal_MC_th2d->GetBinError(3, 2));
    printf("%lf+-%lf %lf+-%lf %lf+-%lf\n", signal_MC_th2d->GetBinContent(1, 1), signal_MC_th2d->GetBinError(1, 1), signal_MC_th2d->GetBinContent(2, 1), signal_MC_th2d->GetBinError(2, 1), signal_MC_th2d->GetBinContent(3, 1), signal_MC_th2d->GetBinError(3, 1));

    printf("\n");

    printf("bkg:\n");
    printf("%lf+-%lf %lf+-%lf %lf+-%lf\n", bkg_MC_th2d->GetBinContent(1, 2), bkg_MC_th2d->GetBinError(1, 2), bkg_MC_th2d->GetBinContent(2, 2), bkg_MC_th2d->GetBinError(2, 2), bkg_MC_th2d->GetBinContent(3, 2), bkg_MC_th2d->GetBinError(3, 2));
    printf("%lf+-%lf %lf+-%lf %lf+-%lf\n", bkg_MC_th2d->GetBinContent(1, 1), bkg_MC_th2d->GetBinError(1, 1), bkg_MC_th2d->GetBinContent(2, 1), bkg_MC_th2d->GetBinError(2, 1), bkg_MC_th2d->GetBinContent(3, 1), bkg_MC_th2d->GetBinError(3, 1));

    printf("\n");

    // Save in root file
    TFile* file = new TFile((std::string(argv[3]) + "/histogram_output.root").c_str(), "RECREATE");

    data_th1d->Write();
    signal_MC_th1d->Write();
    bkg_MC_th1d->Write();

    signal_MC_th1d_stat_err->Write();
    bkg_MC_th1d_stat_err->Write();

    file->Close();


    // make workspace
    RooStats::HistFactory::Measurement meas("my_measurement", "my measurement");
    meas.SetOutputFilePrefix((argv[1] + std::string("/") + "my_measurement").c_str());
    meas.SetExportOnly(true);

    // setting measurement
    meas.SetPOI("mu");
    meas.SetLumi(1.0);
    meas.AddConstantParam("Lumi");

    // define channels
    RooStats::HistFactory::Channel channel_Belle_II("Belle_II");
    channel_Belle_II.SetStatErrorConfig(1e-5, "Gaussian");

    // fill channels
    channel_Belle_II.SetData("data_th1d", (std::string(argv[3]) + "/histogram_output.root").c_str());

    RooStats::HistFactory::Sample signal_Belle_II("signal_Belle_II", "signal_MC_th1d", (std::string(argv[3]) + "/histogram_output.root").c_str());
    signal_Belle_II.ActivateStatError("signal_MC_th1d_stat_err", (std::string(argv[3]) + "/histogram_output.root").c_str(), "");
    signal_Belle_II.SetNormalizeByTheory(false);
    signal_Belle_II.AddNormFactor("mu", 1.0, -100.0, 100.0);

    RooStats::HistFactory::Sample bkg_Belle_II("bkg_Belle_II", "bkg_MC_th1d", (std::string(argv[3]) + "/histogram_output.root").c_str());
    bkg_Belle_II.ActivateStatError("bkg_MC_th1d_stat_err", (std::string(argv[3]) + "/histogram_output.root").c_str(), "");
    bkg_Belle_II.SetNormalizeByTheory(false);

    channel_Belle_II.AddSample(signal_Belle_II);
    channel_Belle_II.AddSample(bkg_Belle_II);

    // add channel to measurement
    meas.AddChannel(channel_Belle_II);
    meas.CollectHistograms();

    RooWorkspace* w;
    w = RooStats::HistFactory::MakeModelAndMeasurementFast(meas);

    w->Print();
    w->writeToFile((std::string(argv[3]) + "/workspace.root").c_str());

    meas.PrintXML("my_measurement");

    return 0;
}
