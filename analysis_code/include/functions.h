#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "TSystemDirectory.h"
#include "TList.h"
#include "TSystemFile.h"
#include "TString.h"
#include "TCollection.h"
#include "TH1.h"


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
        fscanf("%lf\n", &element);
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

#endif 