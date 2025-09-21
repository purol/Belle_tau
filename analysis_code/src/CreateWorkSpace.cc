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

    // several index
    double SampleType;
    double EventType;
    double EnergyType;

    it = std::find(variable_names_.begin(), variable_names_.end(), "MySampleType");
    if (it != variable_names_.end()) {
        int index = std::distance(variable_names_.begin(), it);
        SampleType = std::get<double>((*data_).variable.at(index));
    }

    it = std::find(variable_names_.begin(), variable_names_.end(), "MyEventType");
    if (it != variable_names_.end()) {
        int index = std::distance(variable_names_.begin(), it);
        EventType = std::get<double>((*data_).variable.at(index));
    }

    it = std::find(variable_names_.begin(), variable_names_.end(), "MyEnergyType");
    if (it != variable_names_.end()) {
        int index = std::distance(variable_names_.begin(), it);
        EnergyType = std::get<double>((*data_).variable.at(index));
    }

    if ((-1.5 < SampleType) && (SampleType < -0.5)) { // data
        printf("why do you try to split data?\n");
        exit(1);
        return 0.0;
    }
    else if ((0.5 < SampleType) && (SampleType < 1.5)) { // MC15ri
        if ((0.5 < EnergyType) && (EnergyType < 1.5)) { // 4S
            if ((-0.5 < EventType) && (EventType < 0.5)) return 2.0 * Scale_SIGNAL_BelleII_4S_MC15ri * total_correction; // signal
            else if ((0.5 < EventType) && (EventType < 1.5)) return 2.0 * Scale_BelleII_4S_CHG_MC15ri * total_correction; // CHG
            else if ((1.5 < EventType) && (EventType < 2.5)) return 2.0 * Scale_BelleII_4S_MIX_MC15ri * total_correction; // MIX
            else if ((2.5 < EventType) && (EventType < 3.5)) return 2.0 * Scale_BelleII_4S_UUBAR_MC15ri * total_correction; // UUBAR
            else if ((3.5 < EventType) && (EventType < 4.5)) return 2.0 * Scale_BelleII_4S_DDBAR_MC15ri * total_correction; // DDBAR
            else if ((4.5 < EventType) && (EventType < 5.5)) return 2.0 * Scale_BelleII_4S_SSBAR_MC15ri * total_correction; // SSBAR
            else if ((5.5 < EventType) && (EventType < 6.5)) return 2.0 * Scale_BelleII_4S_CHARM_MC15ri * total_correction; // CHARM
            else if ((6.5 < EventType) && (EventType < 7.5)) return 2.0 * Scale_BelleII_4S_MUMU_MC15ri * total_correction; // MUMU
            else if ((7.5 < EventType) && (EventType < 8.5)) return 2.0 * Scale_BelleII_4S_EE_MC15ri * total_correction; // EE
            else if ((8.5 < EventType) && (EventType < 9.5)) return 2.0 * Scale_BelleII_4S_EEEE_MC15ri * total_correction; // EEEE
            else if ((9.5 < EventType) && (EventType < 10.5)) return 2.0 * Scale_BelleII_4S_EEMUMU_MC15ri * total_correction; // EEMUMU
            else if ((10.5 < EventType) && (EventType < 11.5)) return 2.0 * Scale_BelleII_4S_EEPIPI_MC15ri * total_correction; // EEPIPI
            else if ((11.5 < EventType) && (EventType < 12.5)) return 2.0 * Scale_BelleII_4S_EEKK_MC15ri * total_correction; // EEKK
            else if ((12.5 < EventType) && (EventType < 13.5)) return 2.0 * Scale_BelleII_4S_EEPP_MC15ri * total_correction; // EEPP
            else if ((13.5 < EventType) && (EventType < 14.5)) return 2.0 * Scale_BelleII_4S_PIPIISR_MC15ri * total_correction; // PIPIISR
            else if ((14.5 < EventType) && (EventType < 15.5)) return 2.0 * Scale_BelleII_4S_KKISR_MC15ri * total_correction; // KKISR
            else if ((15.5 < EventType) && (EventType < 16.5)) return 2.0 * Scale_BelleII_4S_GG_MC15ri * total_correction; // GG
            else if ((16.5 < EventType) && (EventType < 17.5)) return 2.0 * Scale_BelleII_4S_EETAUTAU_MC15ri * total_correction; // EETAUTAU
            else if ((17.5 < EventType) && (EventType < 18.5)) return 2.0 * Scale_BelleII_4S_K0K0BARISR_MC15ri * total_correction; // K0K0BARISR
            else if ((18.5 < EventType) && (EventType < 19.5)) return 2.0 * Scale_BelleII_4S_MUMUMUMU_MC15ri * total_correction; // MUMUMUMU
            else if ((19.5 < EventType) && (EventType < 20.5)) return 2.0 * Scale_BelleII_4S_MUMUTAUTAU_MC15ri * total_correction; // MUMUTAUTAU
            else if ((20.5 < EventType) && (EventType < 21.5)) return 2.0 * Scale_BelleII_4S_TAUTAUTAUTAU_MC15ri * total_correction; // TAUTAUTAUTAU
            else if ((21.5 < EventType) && (EventType < 22.5)) return 2.0 * Scale_BelleII_4S_TAUPAIR_MC15ri * total_correction; // TAUPAIR
            else if ((22.5 < EventType) && (EventType < 23.5)) return 2.0 * Scale_BelleII_4S_PIPIPI0ISR_MC15ri * total_correction; // PIPIPI0ISR
        }
        else if ((1.5 < EnergyType) && (EnergyType < 2.5)) { // off-resonance
            if ((-0.5 < EventType) && (EventType < 0.5)) return 2.0 * Scale_SIGNAL_BelleII_off_MC15ri * total_correction; // signal
            else if ((2.5 < EventType) && (EventType < 3.5)) return 2.0 * Scale_BelleII_off_UUBAR_MC15ri * total_correction; // UUBAR
            else if ((3.5 < EventType) && (EventType < 4.5)) return 2.0 * Scale_BelleII_off_DDBAR_MC15ri * total_correction; // DDBAR
            else if ((4.5 < EventType) && (EventType < 5.5)) return 2.0 * Scale_BelleII_off_SSBAR_MC15ri * total_correction; // SSBAR
            else if ((5.5 < EventType) && (EventType < 6.5)) return 2.0 * Scale_BelleII_off_CHARM_MC15ri * total_correction; // CHARM
            else if ((6.5 < EventType) && (EventType < 7.5)) return 2.0 * Scale_BelleII_off_MUMU_MC15ri * total_correction; // MUMU
            else if ((7.5 < EventType) && (EventType < 8.5)) return 2.0 * Scale_BelleII_off_EE_MC15ri * total_correction; // EE
            else if ((8.5 < EventType) && (EventType < 9.5)) return 2.0 * Scale_BelleII_off_EEEE_MC15ri * total_correction; // EEEE
            else if ((9.5 < EventType) && (EventType < 10.5)) return 2.0 * Scale_BelleII_off_EEMUMU_MC15ri * total_correction; // EEMUMU
            else if ((10.5 < EventType) && (EventType < 11.5)) return 2.0 * Scale_BelleII_off_EEPIPI_MC15ri * total_correction; // EEPIPI
            else if ((11.5 < EventType) && (EventType < 12.5)) return 2.0 * Scale_BelleII_off_EEKK_MC15ri * total_correction; // EEKK
            else if ((12.5 < EventType) && (EventType < 13.5)) return 2.0 * Scale_BelleII_off_EEPP_MC15ri * total_correction; // EEPP
            else if ((15.5 < EventType) && (EventType < 16.5)) return 2.0 * Scale_BelleII_off_GG_MC15ri * total_correction; // GG
            else if ((16.5 < EventType) && (EventType < 17.5)) return 2.0 * Scale_BelleII_off_EETAUTAU_MC15ri * total_correction; // EETAUTAU
            else if ((18.5 < EventType) && (EventType < 19.5)) return 2.0 * Scale_BelleII_off_MUMUMUMU_MC15ri * total_correction; // MUMUMUMU
            else if ((21.5 < EventType) && (EventType < 22.5)) return 2.0 * Scale_BelleII_off_TAUPAIR_MC15ri * total_correction; // TAUPAIR
        }
        else if ((2.5 < EnergyType) && (EnergyType < 3.5)) {} // 10657
        else if ((3.5 < EnergyType) && (EnergyType < 4.5)) {} // 10706
        else if ((4.5 < EnergyType) && (EnergyType < 5.5)) {} // 10751
        else if ((5.5 < EnergyType) && (EnergyType < 6.5)) { // 10810
            if ((-0.5 < EventType) && (EventType < 0.5)) return 2.0 * Scale_SIGNAL_BelleII_10810_MC15ri * total_correction; // signal
            else if ((0.5 < EventType) && (EventType < 1.5)) return 2.0 * Scale_BelleII_10810_CHG_MC15ri * total_correction; // CHG
            else if ((1.5 < EventType) && (EventType < 2.5)) return 2.0 * Scale_BelleII_10810_MIX_MC15ri * total_correction; // MIX
            else if ((2.5 < EventType) && (EventType < 3.5)) return 2.0 * Scale_BelleII_10810_UUBAR_MC15ri * total_correction; // UUBAR
            else if ((3.5 < EventType) && (EventType < 4.5)) return 2.0 * Scale_BelleII_10810_DDBAR_MC15ri * total_correction; // DDBAR
            else if ((4.5 < EventType) && (EventType < 5.5)) return 2.0 * Scale_BelleII_10810_SSBAR_MC15ri * total_correction; // SSBAR
            else if ((5.5 < EventType) && (EventType < 6.5)) return 2.0 * Scale_BelleII_10810_CHARM_MC15ri * total_correction; // CHARM
            else if ((6.5 < EventType) && (EventType < 7.5)) return 2.0 * Scale_BelleII_10810_MUMU_MC15ri * total_correction; // MUMU
            else if ((21.5 < EventType) && (EventType < 22.5)) return 2.0 * Scale_BelleII_10810_TAUPAIR_MC15ri * total_correction; // TAUPAIR
            else if ((23.5 < EventType) && (EventType < 24.5)) return 2.0 * Scale_BelleII_10810_BBs_MC15ri * total_correction; // BBs
            else if ((24.5 < EventType) && (EventType < 25.5)) return 2.0 * Scale_BelleII_10810_BsBs_MC15ri * total_correction; // BsBs
        }
    }
    else if ((1.5 < SampleType) && (SampleType < 2.5)) {} // MC15rd
    else if ((2.5 < SampleType) && (SampleType < 3.5)) {} // MC16ri
    else if ((3.5 < SampleType) && (SampleType < 4.5)) {} // MC16rd
    else if ((4.5 < SampleType) && (SampleType < 5.5)) { // Belle data
        printf("why do you try to split data?\n");
        exit(1);
        return 0.0;
    }
    else if ((5.5 < SampleType) && (SampleType < 6.5)) {} // Belle MC

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
    else if (((M_peak_g - 3.0 * M_left_sigma_g) < M) && (M <= (M_peak_g + 3.0 * M_right_sigma_g)) && ((deltaE_peak_g - 15 * deltaE_left_sigma_g) < deltaE) && (deltaE <= (deltaE_peak_g - 5 * deltaE_left_sigma_g))) return 2.0;
    else return NAN;

}

void FillHistogram(const char* input_path_1_, const char* input_path_2_, TH1D* data_th1d_, TH1D* signal_MC_th1d_, TH1D* bkg_MC_th1d_, TH1D* data_th1d_stat_err_, TH1D* signal_MC_th1d_stat_err_, TH1D* bkg_MC_th1d_stat_err_, std::vector<std::string> data_list_, std::vector<std::string> signal_list_, std::vector<std::string> background_list_) {
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


    // get statistical uncertainty
    data_th1d_stat_err_->SetBinContent(1, data_th1d_->GetBinError(1));
    data_th1d_stat_err_->SetBinContent(2, data_th1d_->GetBinError(2));
    signal_MC_th1d_stat_err_->SetBinContent(1, signal_MC_th1d_->GetBinError(1));
    signal_MC_th1d_stat_err_->SetBinContent(2, signal_MC_th1d_->GetBinError(2));
    bkg_MC_th1d_stat_err_->SetBinContent(1, bkg_MC_th1d_->GetBinError(1));
    bkg_MC_th1d_stat_err_->SetBinContent(2, bkg_MC_th1d_->GetBinError(2));


    // We do not open the box, So data_th1d is MC. We use the proper uncertainty
    data_th1d_->SetBinError(1, std::sqrt(data_th1d_->GetBinContent(1)));
    data_th1d_->SetBinError(2, std::sqrt(data_th1d_->GetBinContent(2)));
}

void ReadPCA(const char* filename, TH1D* signal_MC_th1d_nominal, TH1D* bkg_MC_th1d_nominal, const char* syst_name, std::vector<TH1D*>* signal_MC_th1d_syst, std::vector<TH1D*>* bkg_MC_th1d_syst) {
    FILE* fp = fopen(filename, "r");

    int Nbin = -1;
    int NComponent = -1;
    std::vector<double> eigen_values;
    std::vector<std::vector<double>> eigen_vectors;

    fscanf(fp, "%d,%d\n", &Nbin, &NComponent);
    for (int i = 0; i < NComponent; i++) {
        double eigen_value = -1;
        fscanf(fp, "%lf\n", &eigen_value);
        eigen_values.push_back(eigen_value);

        std::vector<double> eigen_vector;
        for (int j = 0; j < Nbin; j++) {
            double element = -1;
            fscanf(fp, "%lf\n", &element);
            eigen_vector.push_back(element);
        }
        eigen_vectors.push_back(eigen_vector);
    }
    fclose(fp);

    if (Nbin != (signal_MC_th1d_nominal->GetNbinsX() + bkg_MC_th1d_nominal->GetNbinsX())) {
        throw std::runtime_error("[ReadToys] Unexpected Nbin value");
    }

    for (int i = 0; i < NComponent; i++) {

        std::string hist_name_signal = std::string("signal_hist_") + syst_name;
        std::string hist_name_bkg = std::string("bkg_hist_") + syst_name;

        TH1D* temp_signal_p = new TH1D((hist_name_signal + "_p_" + std::to_string(i)).c_str(), ";;", signal_MC_th1d_nominal->GetNbinsX(), signal_MC_th1d_nominal->GetXaxis()->GetXmin(), signal_MC_th1d_nominal->GetXaxis()->GetXmax());
        for (int j = 0; j < signal_MC_th1d_nominal->GetNbinsX(); j++) {
            temp_signal_p->SetBinContent(j + 1, (1.0 + eigen_values.at(i) * eigen_vectors.at(i).at(j)) * signal_MC_th1d_nominal->GetBinContent(j + 1));
            temp_signal_p->SetBinError(j + 1, (1.0 + eigen_values.at(i) * eigen_vectors.at(i).at(j)) * signal_MC_th1d_nominal->GetBinError(j + 1));
        }
        signal_MC_th1d_syst->push_back(temp_signal_p);

        TH1D* temp_signal_n = new TH1D((hist_name_signal + "_n_" + std::to_string(i)).c_str(), ";;", signal_MC_th1d_nominal->GetNbinsX(), signal_MC_th1d_nominal->GetXaxis()->GetXmin(), signal_MC_th1d_nominal->GetXaxis()->GetXmax());
        for (int j = 0; j < signal_MC_th1d_nominal->GetNbinsX(); j++) {
            temp_signal_n->SetBinContent(j + 1, (1.0 - eigen_values.at(i) * eigen_vectors.at(i).at(j)) * signal_MC_th1d_nominal->GetBinContent(j + 1));
            temp_signal_n->SetBinError(j + 1, (1.0 - eigen_values.at(i) * eigen_vectors.at(i).at(j)) * signal_MC_th1d_nominal->GetBinError(j + 1));
        }
        signal_MC_th1d_syst->push_back(temp_signal_n);

        TH1D* temp_bkg_p = new TH1D((hist_name_bkg + "_p_" + std::to_string(i)).c_str(), ";;", bkg_MC_th1d_nominal->GetNbinsX(), bkg_MC_th1d_nominal->GetXaxis()->GetXmin(), bkg_MC_th1d_nominal->GetXaxis()->GetXmax());
        for (int j = 0; j < bkg_MC_th1d_nominal->GetNbinsX(); j++) {
            temp_bkg_p->SetBinContent(j + 1, (1.0 + eigen_values.at(i) * eigen_vectors.at(i).at(j + signal_MC_th1d_nominal->GetNbinsX())) * bkg_MC_th1d_nominal->GetBinContent(j + 1));
            temp_bkg_p->SetBinError(j + 1, (1.0 + eigen_values.at(i) * eigen_vectors.at(i).at(j + signal_MC_th1d_nominal->GetNbinsX())) * bkg_MC_th1d_nominal->GetBinError(j + 1));
        }
        bkg_MC_th1d_syst->push_back(temp_bkg_p);

        TH1D* temp_bkg_n = new TH1D((hist_name_bkg + "_n_" + std::to_string(i)).c_str(), ";;", bkg_MC_th1d_nominal->GetNbinsX(), bkg_MC_th1d_nominal->GetXaxis()->GetXmin(), bkg_MC_th1d_nominal->GetXaxis()->GetXmax());
        for (int j = 0; j < bkg_MC_th1d_nominal->GetNbinsX(); j++) {
            temp_bkg_n->SetBinContent(j + 1, (1.0 - eigen_values.at(i) * eigen_vectors.at(i).at(j + signal_MC_th1d_nominal->GetNbinsX())) * bkg_MC_th1d_nominal->GetBinContent(j + 1));
            temp_bkg_n->SetBinError(j + 1, (1.0 - eigen_values.at(i) * eigen_vectors.at(i).at(j + signal_MC_th1d_nominal->GetNbinsX())) * bkg_MC_th1d_nominal->GetBinError(j + 1));
        }
        bkg_MC_th1d_syst->push_back(temp_bkg_n);

    }

}

int main(int argc, char* argv[]) {
    /*
    * argv[1]: input path 1
    * argv[2]: input path 2
    * argv[3]: output path
    */

    // TH1 list
    /*
    *
    *   deltaE
    *      ^
    *   +5 +-----+-------+-----+
    *      |     |       |     |
    *      |     |   1   |     |
    *   -5 +-----+-+---+-+-----+
    *      |     | |   | |     |
    *      |     | |   | |     |
    *      |     | | 2 | |     |
    *  -15 +-----+-+---+-+-----+---> M
    *     -20  -5 -3  +3 +5   +20
    */
    TH1D* data_th1d = new TH1D("data_th1d", ";bin index;", 2, 0.5, 2.5);
    TH1D* signal_MC_th1d = new TH1D("signal_MC_th1d", ";bin index;", 2, 0.5, 2.5);
    TH1D* bkg_MC_th1d = new TH1D("bkg_MC_th1d", ";bin index;", 2, 0.5, 2.5);

    TH1D* data_th1d_stat_err = new TH1D("data_th1d_stat_err", ";bin index;", 2, 0.5, 2.5);
    TH1D* signal_MC_th1d_stat_err = new TH1D("signal_MC_th1d_stat_err", ";bin index;", 2, 0.5, 2.5);
    TH1D* bkg_MC_th1d_stat_err = new TH1D("bkg_MC_th1d_stat_err", ";bin index;", 2, 0.5, 2.5);

    std::vector<TH1D*> signal_MC_th1d_muonID;
    std::vector<TH1D*> bkg_MC_th1d_muonID;

    std::vector<std::string> signal_list = { "SIGNAL" };
    std::vector<std::string> background_list = { "BBs", "BsBs", "CHARM", "CHG", "DDBAR",
        "EE", "EEEE", "EEKK", "EEMUMU", "EEPIPI",
        "EEPP", "EETAUTAU", "GG", "K0K0BARISR", "KKISR",
        "MIX", "MUMU", "MUMUMUMU", "MUMUTAUTAU", "PIPIPI0ISR",
        "PIPIISR", "SSBAR", "TAUPAIR", "TAUTAUTAUTAU", "UUBAR" };

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

    ObtainWeight = MyScaleFunction_correction_halfsplit;

    // we do not open the box, so I just use background MC
    FillHistogram(argv[1], argv[2], data_th1d, signal_MC_th1d, bkg_MC_th1d, data_th1d_stat_err, signal_MC_th1d_stat_err, bkg_MC_th1d_stat_err, background_list, signal_list, background_list);

    // muonID histogram
    ReadPCA((std::string(argv[1]) + "/muonID_PCA").c_str(), signal_MC_th1d, bkg_MC_th1d, "muonID", &signal_MC_th1d_muonID, &bkg_MC_th1d_muonID);

    // print information
    printf("data:\n");
    printf("%lf+-%lf %lf+-%lf\n", data_th1d->GetBinContent(1), data_th1d->GetBinError(1), data_th1d->GetBinContent(2), data_th1d->GetBinError(2));

    printf("\n");

    printf("signal:\n");
    printf("%lf+-%lf %lf+-%lf\n", signal_MC_th1d->GetBinContent(1), signal_MC_th1d->GetBinError(1), signal_MC_th1d->GetBinContent(2), signal_MC_th1d->GetBinError(2));

    printf("\n");

    printf("bkg:\n");
    printf("%lf+-%lf %lf+-%lf\n", bkg_MC_th1d->GetBinContent(1), bkg_MC_th1d->GetBinError(1), bkg_MC_th1d->GetBinContent(2), bkg_MC_th1d->GetBinError(2));

    printf("\n");

    // Save in root file
    TFile* file = new TFile((std::string(argv[3]) + "/histogram_output.root").c_str(), "RECREATE");

    data_th1d->Write();
    signal_MC_th1d->Write();
    bkg_MC_th1d->Write();

    data_th1d_stat_err->Write();
    signal_MC_th1d_stat_err->Write();
    bkg_MC_th1d_stat_err->Write();

    for (int i = 0; i < signal_MC_th1d_muonID.size(); i++) signal_MC_th1d_muonID.at(i)->Write();
    for (int i = 0; i < bkg_MC_th1d_muonID.size(); i++) bkg_MC_th1d_muonID.at(i)->Write();

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
    signal_Belle_II.AddNormFactor("mu", 1.0, 0.0, 100.0);
    signal_Belle_II.AddOverallSys("tracking_efficiency", 1.0 - (track_rel_uncertainty / 100.0) * 3, 1.0 + (track_rel_uncertainty / 100.0) * 3);
    for (int i = 0; i < signal_MC_th1d_muonID.size() / 2; i++) signal_Belle_II.AddHistoSys(("muonID_" + std::to_string(i)).c_str(), ("signal_hist_muonID_n_" + std::to_string(i)).c_str(), (std::string(argv[3]) + "/histogram_output.root").c_str(), "", ("signal_hist_muonID_p_" + std::to_string(i)).c_str(), (std::string(argv[3]) + "/histogram_output.root").c_str(), "");

    RooStats::HistFactory::Sample bkg_Belle_II("bkg_Belle_II", "bkg_MC_th1d", (std::string(argv[3]) + "/histogram_output.root").c_str());
    bkg_Belle_II.ActivateStatError("bkg_MC_th1d_stat_err", (std::string(argv[3]) + "/histogram_output.root").c_str(), "");
    for (int i = 0; i < bkg_MC_th1d_muonID.size() / 2; i++) bkg_Belle_II.AddHistoSys(("muonID_" + std::to_string(i)).c_str(), ("bkg_hist_muonID_n_" + std::to_string(i)).c_str(), (std::string(argv[3]) + "/histogram_output.root").c_str(), "", ("bkg_hist_muonID_p_" + std::to_string(i)).c_str(), (std::string(argv[3]) + "/histogram_output.root").c_str(), "");
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
