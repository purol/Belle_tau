#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <limits>
#include <stdexcept>

#include "TSystemDirectory.h"
#include "TList.h"
#include "TSystemFile.h"
#include "TString.h"
#include "TCollection.h"
#include "TH1.h"
#include "RooDataSet.h"
#include "RooArgSet.h"


void ReadResolution(const char* filename_, double* deltaE_peak_, double* deltaE_left_sigma_, double* deltaE_right_sigma_, double* M_peak_, double* M_left_sigma_, double* M_right_sigma_, double* theta_) {
    FILE* fp = fopen(filename_, "r");

    double mean_M;
    double sigma_left_M;
    double sigma_right_M;
    double result_M;

    double mean_deltaE;
    double sigma_left_deltaE;
    double sigma_right_deltaE;
    double result_deltaE;

    double theta;

    fscanf(fp, "%lf %lf %lf %d\n", &mean_M, &sigma_left_M, &sigma_right_M, &result_M);
    fscanf(fp, "%lf %lf %lf %d\n", &mean_deltaE, &sigma_left_deltaE, &sigma_right_deltaE, &result_deltaE);
    fscanf(fp, "%lf\n", &theta);

    fclose(fp);

    *deltaE_peak_ = mean_deltaE;
    *deltaE_left_sigma_ = sigma_left_deltaE;
    *deltaE_right_sigma_ = sigma_right_deltaE;

    *M_peak_ = mean_M;
    *M_left_sigma_ = sigma_left_M;
    *M_right_sigma_ = sigma_right_M;

    *theta_ = theta;
}

void ReadFOM(const char* filename, double* cut_value_) {
    std::ifstream logFile(filename);
    if (!logFile.is_open()) {
        std::cerr << "Error: Could not open FOM.log file!" << std::endl;
        return;
    }

    std::string line;
    double cutValue = 0.0;

    while (std::getline(logFile, line)) {
        // Check if the line contains "Cut value:"
        if (line.find("Cut value:") != std::string::npos) {
            std::istringstream iss(line);
            std::string temp;
            iss >> temp >> temp; // Skip "Cut" and "value:"
            iss >> cutValue;     // Read the actual cut value
            break;               // Stop searching after finding the cut value
        }
    }

    logFile.close();

    if (cutValue != 0.0) {
        std::cout << "[ReadFOM] Cut value extracted: " << cutValue << std::endl;
    }
    else {
        std::cerr << "[ReadFOM] Error: Cut value not found in the log file!" << std::endl;
    }

    *cut_value_ = cutValue;

}

std::string get_ellipse_region_one(const char* deltaE_name_, const char* M_name_, double sigma_, double deltaE_peak_, double deltaE_left_sigma_, double deltaE_right_sigma_, double M_peak_, double M_left_sigma_, double M_right_sigma_, double theta_) {

    // ellipse variable
    std::string x_var = "((" + std::string(M_name_) + "-" + std::to_string(M_peak_) + ")*(" + std::to_string(std::cos(theta_)) + ")+(" + std::string(deltaE_name_) + "-" + std::to_string(deltaE_peak_) + ")*(" + std::to_string(std::sin(theta_)) + "))";
    std::string y_var = "(-(" + std::string(M_name_) + "-" + std::to_string(M_peak_) + ")*(" + std::to_string(std::sin(theta_)) + ")+(" + std::string(deltaE_name_) + "-" + std::to_string(deltaE_peak_) + ")*(" + std::to_string(std::cos(theta_)) + "))";

    // case 1
    std::string condition_one = "(((" + std::string(M_name_) + "-" + std::to_string(M_peak_) + ")*(" + std::to_string(std::tan(theta_)) + ")<=(" + std::string(deltaE_name_) + "-" + std::to_string(deltaE_peak_) + ")) && ((" + std::string(M_name_) + "-" + std::to_string(M_peak_) + ")*(" + std::to_string(std::tan(theta_ - M_PI / 2.0)) + ")>(" + std::string(deltaE_name_) + "-" + std::to_string(deltaE_peak_) + ")))";
    std::string ellipse_one = "(((" + x_var + "^2)/((" + std::to_string(sigma_) + "*" + std::to_string(M_right_sigma_) + ")^2) + (" + y_var + "^2)/((" + std::to_string(sigma_) + "*" + std::to_string(deltaE_right_sigma_) + ")^2))<=1)";

    // case 2
    std::string condition_two = "(((" + std::string(M_name_) + "-" + std::to_string(M_peak_) + ")*(" + std::to_string(std::tan(theta_)) + ")<=(" + std::string(deltaE_name_) + "-" + std::to_string(deltaE_peak_) + ")) && ((" + std::string(M_name_) + "-" + std::to_string(M_peak_) + ")*(" + std::to_string(std::tan(theta_ - M_PI / 2.0)) + ")<=(" + std::string(deltaE_name_) + "-" + std::to_string(deltaE_peak_) + ")))";
    std::string ellipse_two = "(((" + x_var + "^2)/((" + std::to_string(sigma_) + "*" + std::to_string(M_left_sigma_) + ")^2) + (" + y_var + "^2)/((" + std::to_string(sigma_) + "*" + std::to_string(deltaE_right_sigma_) + ")^2))<=1)";

    // case 3
    std::string condition_three = "(((" + std::string(M_name_) + "-" + std::to_string(M_peak_) + ")*(" + std::to_string(std::tan(theta_)) + ")>(" + std::string(deltaE_name_) + "-" + std::to_string(deltaE_peak_) + ")) && ((" + std::string(M_name_) + "-" + std::to_string(M_peak_) + ")*(" + std::to_string(std::tan(theta_ - M_PI / 2.0)) + ")>(" + std::string(deltaE_name_) + "-" + std::to_string(deltaE_peak_) + ")))";
    std::string ellipse_three = "(((" + x_var + "^2)/((" + std::to_string(sigma_) + "*" + std::to_string(M_right_sigma_) + ")^2) + (" + y_var + "^2)/((" + std::to_string(sigma_) + "*" + std::to_string(deltaE_left_sigma_) + ")^2))<=1)";

    // case 4
    std::string condition_four = "(((" + std::string(M_name_) + "-" + std::to_string(M_peak_) + ")*(" + std::to_string(std::tan(theta_)) + ")>(" + std::string(deltaE_name_) + "-" + std::to_string(deltaE_peak_) + ")) && ((" + std::string(M_name_) + "-" + std::to_string(M_peak_) + ")*(" + std::to_string(std::tan(theta_ - M_PI / 2.0)) + ")<=(" + std::string(deltaE_name_) + "-" + std::to_string(deltaE_peak_) + ")))";
    std::string ellipse_four = "(((" + x_var + "^2)/((" + std::to_string(sigma_) + "*" + std::to_string(M_left_sigma_) + ")^2) + (" + y_var + "^2)/((" + std::to_string(sigma_) + "*" + std::to_string(deltaE_left_sigma_) + ")^2))<=1)";

    std::string total = "(" + condition_one + "&&" + ellipse_one + ")||(" + condition_two + "&&" + ellipse_two + ")||(" + condition_three + "&&" + ellipse_three + ")||(" + condition_four + "&&" + ellipse_four + ")";
    
    return total;
}

std::string get_ellipse_region_two(const char* deltaE_name_, const char* M_name_, double sigma_, double deltaE_peak_, double deltaE_left_sigma_, double deltaE_right_sigma_, double M_peak_, double M_left_sigma_, double M_right_sigma_, double theta_) {

    std::string region_one = get_ellipse_region_one(deltaE_name_, M_name_, sigma_, deltaE_peak_, deltaE_left_sigma_, deltaE_right_sigma_, M_peak_, M_left_sigma_, M_right_sigma_, theta_);

    // M direction criteria
    std::string condition_M = "((" + std::to_string(M_peak_ + std::sin(theta_) * sigma_ * deltaE_left_sigma_) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "<" + std::to_string(M_peak_ - std::sin(theta_) * sigma_ * deltaE_right_sigma_) + "))";

    // deltaE direction criteria
    std::string condition_deltaE = "(" + std::string(deltaE_name_) + "<((" + std::to_string(-1.0 / std::tan(theta_)) + ")*(" + std::string(M_name_) + "-" + std::to_string(M_peak_) + ")+" + std::to_string(deltaE_peak_) + "))";

    std::string total = "((" + region_one + ")<0.5) &&" + condition_M + "&&" + condition_deltaE;

    return total;
}

std::string get_square_region_one(const char* deltaE_name_, const char* M_name_, double sigma_, double deltaE_peak_, double deltaE_left_sigma_, double deltaE_right_sigma_, double M_peak_, double M_left_sigma_, double M_right_sigma_, double theta_) {

    // M direction criteria
    std::string condition_M = "((" + std::to_string(M_peak_ - sigma_ * M_left_sigma_) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak_ + sigma_ * M_right_sigma_) + "))";

    // deltaE direction criteria
    std::string condition_deltaE = "((" + std::to_string(deltaE_peak_ - sigma_ * deltaE_left_sigma_) + "< " + std::string(deltaE_name_) + ") && (" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak_ + sigma_ * deltaE_right_sigma_) + "))";

    std::string total = condition_M + "&&" + condition_deltaE;

    return total;

}

std::string get_square_region_two(const char* deltaE_name_, const char* M_name_, double sigma_, double deltaE_peak_, double deltaE_left_sigma_, double deltaE_right_sigma_, double M_peak_, double M_left_sigma_, double M_right_sigma_, double theta_) {

    // M direction criteria
    std::string condition_M = "((" + std::to_string(M_peak_ - sigma_ * M_left_sigma_) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak_ + sigma_ * M_right_sigma_) + "))";

    // deltaE direction criteria
    std::string condition_deltaE = "(" + std::string(deltaE_name_) + "< " + std::to_string(deltaE_peak_ - sigma_ * deltaE_left_sigma_) + ")";

    std::string total = condition_M + "&&" + condition_deltaE;

    return total;

}

void My_load_files(const char* dirname, std::vector<std::string>* names) {
    TSystemDirectory dir(dirname, dirname);
    TList* files = dir.GetListOfFiles();
    if (files) {
        TSystemFile* file;
        TString fname;
        TIter next(files);
        while ((file = (TSystemFile*)next())) {
            fname = file->GetName();
            if (!file->IsDirectory() && fname.EndsWith(".root")) {
                names->push_back(fname.Data());
            }
        }
    }
}

void My_load_files(const char* dirname, std::vector<std::string>* names, const char* included_string) {
    TSystemDirectory dir(dirname, dirname);
    TList* files = dir.GetListOfFiles();
    if (files) {
        TSystemFile* file;
        TString fname;
        TIter next(files);
        while ((file = (TSystemFile*)next())) {
            fname = file->GetName();
            if (!file->IsDirectory() && fname.EndsWith(".root") && fname.Contains(included_string)) {
                names->push_back(fname.Data());
            }
        }
    }
}

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        if (!item.empty()) {
            result.push_back(item);
        }
    }

    return result;
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

void ReadPCA(const char* filename, TH1D* signal_MC_th1d_nominal, const char* syst_name, std::vector<TH1D*>* signal_MC_th1d_syst) {
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

    if (Nbin != signal_MC_th1d_nominal->GetNbinsX()) {
        throw std::runtime_error("[ReadToys] Unexpected Nbin value");
    }

    for (int i = 0; i < NComponent; i++) {

        std::string hist_name_signal = std::string("signal_hist_") + syst_name;

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

    }

}

void ReadPCA_remain(const char* filename, TH1D* signal_MC_th1d_nominal, TH1D* signal_MC_th1d_relative_syst) {
    FILE* fp = fopen(filename, "r");

    int Nbin = -1;

    std::vector<double> relative_uncertainties;

    fscanf(fp, "%d\n", &Nbin);
    for(int i = 0; i < Nbin; i++) {
        double element = -1;
        fscanf(fp, "%lf\n", &element);
        relative_uncertainties.push_back(element);
    }
    fclose(fp);

    if (Nbin != signal_MC_th1d_nominal->GetNbinsX()) {
        throw std::runtime_error("[ReadPCA_remain] Unexpected Nbin value");
    }

    for(int i = 0; i < Nbin; i++) {
        double previous_relative_error = signal_MC_th1d_relative_syst->GetBinContent(i + 1);
        signal_MC_th1d_relative_syst->SetBinContent(i + 1, std::sqrt(previous_relative_error * previous_relative_error + relative_uncertainties.at(i) * relative_uncertainties.at(i)));
    }

}

struct ABCDValidation {
    double kappa;
    double discrepancy;
};

inline double ABCD_validation_kappa(const std::vector<double>& observed_) {
    double mean_A = observed_.at(0);
    double mean_B = observed_.at(1);
    double mean_C = observed_.at(2);
    double mean_D = observed_.at(3);
    if (mean_B == 0.0 || mean_C == 0.0) {
        if (mean_A == 0.0 || mean_D == 0.0) return NAN;
        return std::numeric_limits<double>::infinity();
    }
    return (mean_A / mean_B) * (mean_D / mean_C);
}

inline ABCDValidation Calculate_ABCD_nonclosure(const std::vector<double>& observed_) {
    if (observed_.size() != 4) throw std::runtime_error("[Calculate_ABCD_nonclosure] four observations are required");
    for (double observed : observed_) {
        if (!std::isfinite(observed) || observed < 0.0) throw std::runtime_error("[Calculate_ABCD_nonclosure] invalid observation");
    }

    ABCDValidation result;
    // The signal-free Poisson model gives kappa_hat = N_A * N_D / (N_B * N_C).
    result.kappa = ABCD_validation_kappa(observed_);
    result.discrepancy = std::numeric_limits<double>::infinity();

    // Use abs(kappa_hat - 1), calculated directly from the validation yields, for the systematic.
    if (std::isfinite(result.kappa)) {
        result.discrepancy = std::fabs(result.kappa - 1.0);
    }
    return result;
}

inline std::vector<ABCDValidation> Validate_ABCD(TH1* validation_, const char* filename_, const std::vector<double>& sideband_BDT_cuts_, const std::vector<double>& validation_BDT_cuts_) {
    if (validation_->GetNbinsX() != 8) throw std::runtime_error("[Validate_ABCD] eight validation bins are required");

    std::vector<ABCDValidation> results;
    for (int region = 1; region <= 2; region++) {
        std::vector<double> observed;
        for (int j = 1; j <= 4; j++) observed.push_back(validation_->GetBinContent(4 * (region - 1) + j));
        results.push_back(Calculate_ABCD_nonclosure(observed));
    }

    FILE* fp = fopen(filename_, "w");
    if (fp == nullptr) throw std::runtime_error("[Validate_ABCD] cannot write the validation report");

    fprintf(fp, "Independent signal-free eight-bin Poisson validation; the two deltaE regions factorize.\n");
    fprintf(fp, "discrepancy = abs(kappa_hat - 1); no nominal correction.\n");
    fprintf(fp, "Down/up A templates: max(0, 1-discrepancy), 1+discrepancy; B/C/D unchanged.\n");
    fprintf(fp, "A non-finite or undefined kappa_hat cannot supply a finite systematic; no fallback or pseudocounts.\n");

    for (int region = 1; region <= 2; region++) {
        const ABCDValidation& result = results.at(region - 1);
        fprintf(fp, "region %d\n", region);
        fprintf(fp, "  validation BDT: 0 < O_BDT < %.17g; C/D <= %.17g < A/B\n", sideband_BDT_cuts_.at(region - 1), validation_BDT_cuts_.at(region - 1));
        fprintf(fp, "  A+B=%.17g C+D=%.17g\n", validation_->GetBinContent(4 * (region - 1) + 1) + validation_->GetBinContent(4 * (region - 1) + 2), validation_->GetBinContent(4 * (region - 1) + 3) + validation_->GetBinContent(4 * (region - 1) + 4));

        for (int j = 1; j <= 4; j++) fprintf(fp, "  bin %d: observed=%.17g\n", 4 * (region - 1) + j, validation_->GetBinContent(4 * (region - 1) + j));
        fprintf(fp, "  kappa_hat=%.17g discrepancy=%.17g\n", result.kappa, result.discrepancy);
        
        printf("[ABCD validation] region %d: kappa=%g, non-closure uncertainty=%g\n", region, result.kappa, result.discrepancy);
        if (!std::isfinite(result.discrepancy)) printf("[ABCD validation] region %d: a finite systematic cannot be determined from these observations\n", region);
        else if (result.discrepancy > 1.0) printf("[ABCD validation] region %d: the down variation is limited to zero yield\n", region);
    }

    fclose(fp);
    return results;
}

// Pass the selection values explicitly so this helper does not depend on executable globals.
struct ABCDParameters {
    double BDT_cut_1;
    double BDT_cut_2;
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double M_size;
    double validation_BDT_cut_1 = 0.0;
    double validation_BDT_cut_2 = 0.0;
    double sideband_BDT_cut_1 = 0.0;
    double sideband_BDT_cut_2 = 0.0;
};

inline double mapping_function_ABCD(std::vector<double> variables_, const ABCDParameters& parameters_, bool validation_, int fluc_mode = -1) {
    /*
     * fluc_mode
     * -1: no fluctuation
     *  0: M positive
     *  1: M negative
     *  2: deltaE positive
     *  3: deltaE negative
    */
    double M = variables_.at(0);
    double deltaE = variables_.at(1);
    double BDT_1 = variables_.at(2);
    double BDT_2 = variables_.at(3);
    double M_shift = 0.0;
    double deltaE_shift = 0.0;
    if (fluc_mode == 0) M_shift = 1.0;
    else if (fluc_mode == 1) M_shift = -1.0;
    else if (fluc_mode == 2) deltaE_shift = 1.0;
    else if (fluc_mode == 3) deltaE_shift = -1.0;

    int region = 0;
    if (((parameters_.deltaE_peak - (5.0 - deltaE_shift) * parameters_.deltaE_left_sigma) < deltaE) && (deltaE <= (parameters_.deltaE_peak + (5.0 + deltaE_shift) * parameters_.deltaE_right_sigma))) region = 1;
    else if (((parameters_.deltaE_peak - (15.0 - deltaE_shift) * parameters_.deltaE_left_sigma) < deltaE) && (deltaE <= (parameters_.deltaE_peak - (5.0 - deltaE_shift) * parameters_.deltaE_left_sigma))) region = 2;
    else return NAN;

    // Shift the common boundaries together for resolution variations; the outer mass boundary stays at 20 sigma.
    double central_lower = parameters_.M_peak - (parameters_.M_size - M_shift) * parameters_.M_left_sigma;
    double central_upper = parameters_.M_peak + (parameters_.M_size + M_shift) * parameters_.M_right_sigma;
    double sideband_left_lower = parameters_.M_peak - 20.0 * parameters_.M_left_sigma;
    double sideband_left_upper = parameters_.M_peak - (5.0 - M_shift) * parameters_.M_left_sigma;
    double sideband_right_lower = parameters_.M_peak + (5.0 + M_shift) * parameters_.M_right_sigma;
    double sideband_right_upper = parameters_.M_peak + 20.0 * parameters_.M_right_sigma;

    double BDT;
    double BDT_cut;
    double validation_BDT_cut;
    double sideband_BDT_cut;
    if (region == 1) {
        BDT = BDT_1;
        BDT_cut = parameters_.BDT_cut_1;
        validation_BDT_cut = parameters_.validation_BDT_cut_1;
        sideband_BDT_cut = parameters_.sideband_BDT_cut_1;
    }
    else {
        BDT = BDT_2;
        BDT_cut = parameters_.BDT_cut_2;
        validation_BDT_cut = parameters_.validation_BDT_cut_2;
        sideband_BDT_cut = parameters_.sideband_BDT_cut_2;
    }

    bool central;
    bool sideband;
    bool high_BDT;
    bool low_BDT;
    if (validation_) {
        central = (central_lower < M) && (M < central_upper);
        sideband = ((sideband_left_lower < M) && (M < sideband_left_upper)) ||
                   ((sideband_right_lower < M) && (M < sideband_right_upper));
        high_BDT = (0.0 < BDT) && (validation_BDT_cut < BDT) && (BDT < sideband_BDT_cut);
        low_BDT = (0.0 < BDT) && (BDT <= validation_BDT_cut) && (BDT < sideband_BDT_cut);
    }
    else {
        central = (central_lower < M) && (M <= central_upper);
        sideband = ((sideband_left_lower < M) && (M <= sideband_left_upper)) ||
                   ((sideband_right_lower < M) && (M <= sideband_right_upper));
        high_BDT = BDT_cut < BDT;
        low_BDT = (sideband_BDT_cut < BDT) && (BDT < BDT_cut);
    }

    // A1, B1, C1, D1, A2, B2, C2, D2, with the same ordering for validation.
    if (central && high_BDT) return 4.0 * (region - 1) + 1.0;
    else if (sideband && high_BDT) return 4.0 * (region - 1) + 2.0;
    else if (central && low_BDT) return 4.0 * (region - 1) + 3.0;
    else if (sideband && low_BDT) return 4.0 * (region - 1) + 4.0;
    else return NAN;
}

struct ABCDValidationEvent {
    double BDT;
    double weight;
};

inline double Find_BDT_boundary(std::vector<ABCDValidationEvent>& events_, double upper_, double upper_fraction_) {
    std::sort(events_.begin(), events_.end(), [](const ABCDValidationEvent& left, const ABCDValidationEvent& right) { return left.BDT < right.BDT; });
    double total = 0.0;

    for (const ABCDValidationEvent& event : events_) total += event.weight;
    if (!std::isfinite(total)) throw std::runtime_error("[Find_BDT_boundary] invalid total weight");
    if (total == 0.0) {
        printf("[BDT boundary] no positive yield; use the range fraction and leave the observations empty\n");
        return upper_ * (1.0 - upper_fraction_);
    }

    // Select the requested fraction above the boundary. Equal BDT values stay on the same side.
    double target = total * upper_fraction_;
    // Start with an empty upper range; keep the last BDT group below the boundary for validation.
    double boundary = events_.back().BDT + (upper_ - events_.back().BDT) / 2.0;
    if (boundary <= events_.back().BDT) boundary = upper_;
    double difference = target;
    double low = 0.0;
    for (std::size_t i = 0; i < events_.size(); i++) {
        low += events_.at(i).weight;
        if (i + 1 < events_.size() && events_.at(i).BDT == events_.at(i + 1).BDT) continue;
        double candidate_difference = std::fabs(total - low - target);
        if (candidate_difference < difference) {
            difference = candidate_difference;
            boundary = events_.at(i).BDT;
            if (i + 1 < events_.size()) {
                double midpoint = boundary + (events_.at(i + 1).BDT - boundary) / 2.0;
                if (midpoint < events_.at(i + 1).BDT) boundary = midpoint;
            }
        }
    }
    return boundary;
}

inline std::vector<double> GetABCDBoundary(RooDataSet& data_, ABCDParameters parameters_) {
    // Select all events below O_cut with the nominal mass and deltaE cuts, including O_BDT = 0.
    parameters_.sideband_BDT_cut_1 = -std::numeric_limits<double>::infinity();
    parameters_.sideband_BDT_cut_2 = -std::numeric_limits<double>::infinity();
    std::vector<std::vector<ABCDValidationEvent>> events(2);

    for (int i = 0; i < data_.numEntries(); i++) {
        const RooArgSet* row = data_.get(i);
        std::vector<double> variables = { row->getRealValue("M"), row->getRealValue("deltaE"), row->getRealValue("BDT_1"), row->getRealValue("BDT_2") };
        double bin = mapping_function_ABCD(variables, parameters_, false);
        if (!std::isfinite(bin)) continue;
        int local_bin = ((int)bin - 1) % 4;
        if (local_bin < 2) continue;

        int region = ((int)bin - 1) / 4;
        double weight = data_.weight();
        if (!std::isfinite(weight) || weight < 0.0) throw std::runtime_error("[GetABCDBoundary] data weights must be finite and non-negative");
        if (weight == 0.0) continue;

        if (region == 0) events.at(region).push_back({ variables.at(2), weight });
        else if (region == 1) events.at(region).push_back({ variables.at(3), weight });
    }

    std::vector<double> BDT_cuts = { parameters_.BDT_cut_1, parameters_.BDT_cut_2 };
    std::vector<double> boundaries;
    for (int region = 0; region < 2; region++) {
        // Before box open, use weighted MC yields. Removing the MC weights gives event counts.
        double boundary = Find_BDT_boundary(events.at(region), BDT_cuts.at(region), 1.0 / 3.0);
        boundaries.push_back(boundary);

        double total = 0.0;
        double selected = 0.0;
        for (const ABCDValidationEvent& event : events.at(region)) {
            total += event.weight;
            if (boundary < event.BDT) selected += event.weight;
        }
        printf("[ABCD boundary] region %d: BDT boundary=%.17g, below O_cut=%g, C+D=%g, target=%g\n", region + 1, boundary, total, selected, total / 3.0);
    }
    return boundaries;
}

inline std::vector<double> GetValidationBoundary(RooDataSet& data_, ABCDParameters parameters_) {
    // Validation ends at the C/D lower boundary. A zero split temporarily selects its full range into A/B.
    parameters_.validation_BDT_cut_1 = 0.0;
    parameters_.validation_BDT_cut_2 = 0.0;
    std::vector<std::vector<ABCDValidationEvent>> events(2);

    for (int i = 0; i < data_.numEntries(); i++) {
        const RooArgSet* row = data_.get(i);
        std::vector<double> variables = { row->getRealValue("M"), row->getRealValue("deltaE"), row->getRealValue("BDT_1"), row->getRealValue("BDT_2") };

        double bin = mapping_function_ABCD(variables, parameters_, true);
        if (!std::isfinite(bin)) continue;

        int region = ((int)bin - 1) / 4;
        double weight = data_.weight();

        if (!std::isfinite(weight) || weight < 0.0) throw std::runtime_error("[GetValidationBoundary] validation weights must be finite and non-negative");
        if (weight == 0.0) continue;

        if(region == 0) events.at(region).push_back({ variables.at(2), weight });
        else if (region == 1) events.at(region).push_back({ variables.at(3), weight });
    }

    std::vector<double> BDT_cuts = { parameters_.sideband_BDT_cut_1, parameters_.sideband_BDT_cut_2 };
    std::vector<double> boundaries;

    for (int region = 0; region < 2; region++) {
        // Before box open, balance the weighted MC yields. Removing the MC weights gives event counts.
        double boundary = Find_BDT_boundary(events.at(region), BDT_cuts.at(region), 0.5);
        boundaries.push_back(boundary);

        double high = 0.0;
        double low = 0.0;
        for (const ABCDValidationEvent& event : events.at(region)) {
            if (event.BDT <= boundary) low += event.weight;
            else high += event.weight;
        }
        printf("[ABCD validation] region %d: BDT boundary=%.17g, A+B=%g, C+D=%g\n", region + 1, boundary, high, low);
    }

    return boundaries;
}

#endif
