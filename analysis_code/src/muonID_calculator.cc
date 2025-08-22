#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>

#include "TH1D.h"
#include "TH2D.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "functions.h"
#include "MyModule.h"
#include "correctors.h"
#include "data.h"

Corrector_PID muonID_corrector_05("/home/belle2/junewoo/storage_b2/tau_workspace/tables/muonID_csv/MC15ri/mu_efficiency_table.csv");

double MyScaleFunction_correction_halfsplit(std::vector<Data>::iterator data_, std::vector<std::string> variable_names_) {

    std::vector<std::string>::iterator it;

    // first muonID correction
    double first_muon_p;
    double first_muon_theta;
    double first_muon_correction;
    it = std::find(variable_names_.begin(), variable_names_.end(), "first_muon_p");
    if (it != variable_names_.end()) {
        int index = std::distance(variable_names_.begin(), it);
        first_muon_p = std::get<double>((*data_).variable.at(index));
    }
    it = std::find(variable_names_.begin(), variable_names_.end(), "first_muon_theta");
    if (it != variable_names_.end()) {
        int index = std::distance(variable_names_.begin(), it);
        first_muon_theta = std::get<double>((*data_).variable.at(index));
    }
    first_muon_correction = muonID_corrector_05.GetCorrectionFactor(first_muon_p, first_muon_theta, "+");

    double total_correction = first_muon_correction;

    if ((*data_).filename.find("CHG_") != std::string::npos) return 2.0 * Scale_CHG_MC15ri * total_correction;
    else if ((*data_).filename.find("MIX_") != std::string::npos) return 2.0 * Scale_MIX_MC15ri * total_correction;
    else if ((*data_).filename.find("UUBAR_") != std::string::npos) return 2.0 * Scale_UUBAR_MC15ri * total_correction;
    else if ((*data_).filename.find("DDBAR_") != std::string::npos) return 2.0 * Scale_DDBAR_MC15ri * total_correction;
    else if ((*data_).filename.find("SSBAR_") != std::string::npos) return 2.0 * Scale_SSBAR_MC15ri * total_correction;
    else if ((*data_).filename.find("CHARM_") != std::string::npos) return 2.0 * Scale_CHARM_MC15ri * total_correction;
    else if ((*data_).filename.find("MUMU_") != std::string::npos) return 2.0 * Scale_MUMU_MC15ri * total_correction;
    else if ((*data_).filename.find("EE_") != std::string::npos) return 2.0 * Scale_EE_MC15ri * total_correction;
    else if ((*data_).filename.find("EEEE_") != std::string::npos) return 2.0 * Scale_EEEE_MC15ri * total_correction;
    else if ((*data_).filename.find("EEMUMU_") != std::string::npos) return 2.0 * Scale_EEMUMU_MC15ri * total_correction;
    else if ((*data_).filename.find("EEPIPI_") != std::string::npos) return 2.0 * Scale_EEPIPI_MC15ri * total_correction;
    else if ((*data_).filename.find("EEKK_") != std::string::npos) return 2.0 * Scale_EEKK_MC15ri * total_correction;
    else if ((*data_).filename.find("EEPP_") != std::string::npos) return 2.0 * Scale_EEPP_MC15ri * total_correction;
    else if ((*data_).filename.find("PIPIISR_") != std::string::npos) return 2.0 * Scale_PIPIISR_MC15ri * total_correction;
    else if ((*data_).filename.find("KKISR_") != std::string::npos) return 2.0 * Scale_KKISR_MC15ri * total_correction;
    else if ((*data_).filename.find("GG_") != std::string::npos) return 2.0 * Scale_GG_MC15ri * total_correction;
    else if ((*data_).filename.find("EETAUTAU_") != std::string::npos) return 2.0 * Scale_EETAUTAU_MC15ri * total_correction;
    else if ((*data_).filename.find("K0K0BARISR_") != std::string::npos) return 2.0 * Scale_K0K0BARISR_MC15ri * total_correction;
    else if ((*data_).filename.find("MUMUMUMU_") != std::string::npos) return 2.0 * Scale_MUMUMUMU_MC15ri * total_correction;
    else if ((*data_).filename.find("MUMUTAUTAU_") != std::string::npos) return 2.0 * Scale_MUMUTAUTAU_MC15ri * total_correction;
    else if ((*data_).filename.find("TAUTAUTAUTAU_") != std::string::npos) return 2.0 * Scale_TAUTAUTAUTAU_MC15ri * total_correction;
    else if ((*data_).filename.find("TAUPAIR_") != std::string::npos) return 2.0 * Scale_TAUPAIR_MC15ri * total_correction;
    else if ((*data_).filename.find("SIGNAL_") != std::string::npos) return 2.0 * Scale_SIGNAL_MC15ri * total_correction;
    else {
        printf("unexpected sample type\n");
        exit(1);
        return 0.0;
    }
    printf("unexpected sample type\n");
    exit(1);
    return 0.0;
}

double MyScaleFunction_correction_fluctuation_halfsplit(std::vector<Data>::iterator data_, std::vector<std::string> variable_names_) {

    std::vector<std::string>::iterator it;
    
    // first muonID correction
    double first_muon_p;
    double first_muon_theta;
    double first_muon_correction;
    it = std::find(variable_names_.begin(), variable_names_.end(), "first_muon_p");
    if (it != variable_names_.end()) {
        int index = std::distance(variable_names_.begin(), it);
        first_muon_p = std::get<double>((*data_).variable.at(index));
    }
    it = std::find(variable_names_.begin(), variable_names_.end(), "first_muon_theta");
    if (it != variable_names_.end()) {
        int index = std::distance(variable_names_.begin(), it);
        first_muon_theta = std::get<double>((*data_).variable.at(index));
    }
    first_muon_correction = muonID_corrector_05.GetFluctuatedCorrectionFactor(first_muon_p, first_muon_theta, "+");

    double total_correction = first_muon_correction;

    if ((*data_).filename.find("CHG_") != std::string::npos) return 2.0 * Scale_CHG_MC15ri * total_correction;
    else if ((*data_).filename.find("MIX_") != std::string::npos) return 2.0 * Scale_MIX_MC15ri * total_correction;
    else if ((*data_).filename.find("UUBAR_") != std::string::npos) return 2.0 * Scale_UUBAR_MC15ri * total_correction;
    else if ((*data_).filename.find("DDBAR_") != std::string::npos) return 2.0 * Scale_DDBAR_MC15ri * total_correction;
    else if ((*data_).filename.find("SSBAR_") != std::string::npos) return 2.0 * Scale_SSBAR_MC15ri * total_correction;
    else if ((*data_).filename.find("CHARM_") != std::string::npos) return 2.0 * Scale_CHARM_MC15ri * total_correction;
    else if ((*data_).filename.find("MUMU_") != std::string::npos) return 2.0 * Scale_MUMU_MC15ri * total_correction;
    else if ((*data_).filename.find("EE_") != std::string::npos) return 2.0 * Scale_EE_MC15ri * total_correction;
    else if ((*data_).filename.find("EEEE_") != std::string::npos) return 2.0 * Scale_EEEE_MC15ri * total_correction;
    else if ((*data_).filename.find("EEMUMU_") != std::string::npos) return 2.0 * Scale_EEMUMU_MC15ri * total_correction;
    else if ((*data_).filename.find("EEPIPI_") != std::string::npos) return 2.0 * Scale_EEPIPI_MC15ri * total_correction;
    else if ((*data_).filename.find("EEKK_") != std::string::npos) return 2.0 * Scale_EEKK_MC15ri * total_correction;
    else if ((*data_).filename.find("EEPP_") != std::string::npos) return 2.0 * Scale_EEPP_MC15ri * total_correction;
    else if ((*data_).filename.find("PIPIISR_") != std::string::npos) return 2.0 * Scale_PIPIISR_MC15ri * total_correction;
    else if ((*data_).filename.find("KKISR_") != std::string::npos) return 2.0 * Scale_KKISR_MC15ri * total_correction;
    else if ((*data_).filename.find("GG_") != std::string::npos) return 2.0 * Scale_GG_MC15ri * total_correction;
    else if ((*data_).filename.find("EETAUTAU_") != std::string::npos) return 2.0 * Scale_EETAUTAU_MC15ri * total_correction;
    else if ((*data_).filename.find("K0K0BARISR_") != std::string::npos) return 2.0 * Scale_K0K0BARISR_MC15ri * total_correction;
    else if ((*data_).filename.find("MUMUMUMU_") != std::string::npos) return 2.0 * Scale_MUMUMUMU_MC15ri * total_correction;
    else if ((*data_).filename.find("MUMUTAUTAU_") != std::string::npos) return 2.0 * Scale_MUMUTAUTAU_MC15ri * total_correction;
    else if ((*data_).filename.find("TAUTAUTAUTAU_") != std::string::npos) return 2.0 * Scale_TAUTAUTAUTAU_MC15ri * total_correction;
    else if ((*data_).filename.find("TAUPAIR_") != std::string::npos) return 2.0 * Scale_TAUPAIR_MC15ri * total_correction;
    else if ((*data_).filename.find("SIGNAL_") != std::string::npos) return 2.0 * Scale_SIGNAL_MC15ri * total_correction;
    else {
        printf("unexpected sample type\n");
        exit(1);
        return 0.0;
    }
    printf("unexpected sample type\n");
    exit(1);
    return 0.0;
}

double deltaE_peak_g;
double deltaE_left_sigma_g;
double deltaE_right_sigma_g;
double M_peak_g;
double M_left_sigma_g;
double M_right_sigma_g;
double theta_g;

double mapping_function(std::vector<double> variables_) {
    double M = variables_.at(0);
    double deltaE = variables_.at(1);

    if (((M_peak_g - 5.0 * M_left_sigma_g) < M) && (M <= (M_peak_g + 5.0 * M_right_sigma_g)) && ((deltaE_peak_g - 5 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g + 5 * deltaE_right_sigma_g))) return 1.0;
    else if (((M_peak_g - 3.0 * M_left_sigma_g) < M) && (M <= (M_peak_g + 3.0 * M_right_sigma_g)) && (deltaE <= (deltaE_peak_g - 5 * deltaE_left_sigma_g))) return 2.0;
    else return NAN;

}

void FillHistogram(const char* input_path_1_, const char* input_path_2_, TH1D* data_th1d_, TH1D* signal_MC_th1d_, TH1D* bkg_MC_th1d_, std::vector<std::string> data_list_, std::vector<std::string> signal_list_, std::vector<std::string> background_list_) {
    // data
    Loader loader_data("tau_lfv");
    for (int i = 0; i < data_list_.size(); i++) loader_data.Load((input_path_1_ + std::string("/") + data_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), "root", data_list_.at(i).c_str());
    loader_data.FillCustomizedTH1D(data_th1d_, { "M_inv_tau", "deltaE" }, { mapping_function });
    loader_data.end();

    // signal MC
    Loader loader_signal("tau_lfv");
    for (int i = 0; i < signal_list_.size(); i++) loader_signal.Load((input_path_1_ + std::string("/") + signal_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), "root", signal_list_.at(i).c_str());
    loader_signal.FillCustomizedTH1D(signal_MC_th1d_, { "M_inv_tau", "deltaE" }, { mapping_function });
    loader_signal.end();

    // background MC
    Loader loader_bkg("tau_lfv");
    for (int i = 0; i < background_list_.size(); i++) loader_bkg.Load((input_path_1_ + std::string("/") + background_list_.at(i) + std::string("/") + std::string(input_path_2_)).c_str(), "root", background_list_.at(i).c_str());
    loader_bkg.FillCustomizedTH1D(bkg_MC_th1d_, { "M_inv_tau", "deltaE" }, { mapping_function });
    loader_bkg.end();



    // We do not open the box, So data_th1d is MC. We use the proper uncertainty
    data_th1d_->SetBinError(1, std::sqrt(data_th1d_->GetBinContent(1)));
    data_th1d_->SetBinError(2, std::sqrt(data_th1d_->GetBinContent(2)));
}

int main(int argc, char* argv[]) {
    /*
    * argv[1]: input path 1
    * argv[2]: input path 2
    * argv[3]: output path
    * argv[4]: NToys
    * argv[5]: indicator
    */

    // TH1 list
    /*
    *
    *  deltaE
    *   ^
    *   +-----+-------+-----+
    *   |     |       |     |
    *   |     |   1   |     |
    *   +-----+-+---+-+-----+
    *   |     | |   | |     |
    *   |     | |   | |     |
    *   |     | | 2 | |     |
    *   +-----+-+---+-+-----+---> M
    *  -20  -5 -3  +3 +5   +20
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

    // get nominal value
    std::vector<double> MC_th1d_nominal;
    ObtainWeight = MyScaleFunction_correction_halfsplit;
   
    // reset histograms
    data_th1d->Reset();
    signal_MC_th1d->Reset();
    bkg_MC_th1d->Reset();

    // we do not open the box, so I just use background MC
    FillHistogram(argv[1], argv[2], data_th1d, signal_MC_th1d, bkg_MC_th1d, background_list, signal_list, background_list);

    MC_th1d_nominal.push_back(signal_MC_th1d->GetBinContent(1));
    MC_th1d_nominal.push_back(signal_MC_th1d->GetBinContent(2));
    MC_th1d_nominal.push_back(bkg_MC_th1d->GetBinContent(1));
    MC_th1d_nominal.push_back(bkg_MC_th1d->GetBinContent(2));

    // get fluctuated value
    ObtainWeight = MyScaleFunction_correction_fluctuation_halfsplit;

    // print output
    FILE* fp;
    fp = fopen((std::string(argv[3]) + "/muonID_toys_" + std::string(argv[5]) + ".csv").c_str(), "w");

    int NToys = atoi(argv[4]);
    for (int i = 0; i < NToys; i++) {
        // reset histograms
        data_th1d->Reset();
        signal_MC_th1d->Reset();
        bkg_MC_th1d->Reset();

        // fluctuate correction factors
        muonID_corrector_05.FluctuateCorrectionFactor();

        // we do not open the box, so I just use background MC
        FillHistogram(argv[1], argv[2], data_th1d, signal_MC_th1d, bkg_MC_th1d, background_list, signal_list, background_list);

        if (MC_th1d_nominal.at(0) != 0) fprintf(fp, "%lf,", signal_MC_th1d->GetBinContent(1) / MC_th1d_nominal.at(0));
        else fprintf(fp, "1.0,");

        if (MC_th1d_nominal.at(1) != 0) fprintf(fp, "%lf,", signal_MC_th1d->GetBinContent(2) / MC_th1d_nominal.at(1));
        else fprintf(fp, "1.0,");

        if (MC_th1d_nominal.at(2) != 0) fprintf(fp, "%lf,", bkg_MC_th1d->GetBinContent(1) / MC_th1d_nominal.at(2));
        else fprintf(fp, "1.0,");

        if (MC_th1d_nominal.at(3) != 0) fprintf(fp, "%lf\n", bkg_MC_th1d->GetBinContent(2) / MC_th1d_nominal.at(3));
        else fprintf(fp, "1.0\n");

    }

    fclose(fp);

    return 0;
}
