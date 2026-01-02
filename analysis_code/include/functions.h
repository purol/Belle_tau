#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

#include "TSystemDirectory.h"
#include "TList.h"
#include "TSystemFile.h"
#include "TString.h"
#include "TCollection.h"


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

// this functions have some overhead: we call `ReadResolution` every time. However, it is not dominant during the entire process
std::string get_region_large(const char* filename_, const char* deltaE_name_, const char* M_name_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // M direction criteria
    std::string condition_M = "((" + std::to_string(M_peak - 20.0 * M_left_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak + 20.0 * M_right_sigma) + "))";

    // deltaE direction criteria
    std::string condition_deltaE = "(" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak + 20.0 * deltaE_right_sigma) + ")";

    std::string total = "(" + condition_M + "&&" + condition_deltaE + ")";

    return total;
}

std::string get_region_upper(const char* filename_, const char* deltaE_name_, const char* M_name_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // deltaE direction criteria
    std::string condition_deltaE = "(" + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + " =< " + std::string(deltaE_name_) + ")";

    std::string total = condition_deltaE;

    return total;
}

std::string get_region_lower(const char* filename_, const char* deltaE_name_, const char* M_name_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // deltaE direction criteria
    std::string condition_deltaE = "(" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + ")";

    std::string total = condition_deltaE;

    return total;
}

std::string get_region_one(const char* filename_, const char* deltaE_name_, const char* M_name_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // M direction criteria
    std::string condition_M = "((" + std::to_string(M_peak - 5.0 * M_left_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak + 5.0 * M_right_sigma) + "))";

    // deltaE direction criteria
    std::string condition_deltaE = "((" + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + "< " + std::string(deltaE_name_) + ") && (" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak + 5.0 * deltaE_right_sigma) + "))";

    std::string total = "(" + condition_M + "&&" + condition_deltaE + ")";

    return total;
}

std::string get_region_one_A(const char* filename_, const char* deltaE_name_, const char* M_name_, const char* BDT_name_, double BDT_cut_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // M direction criteria
    std::string condition_M = "((" + std::to_string(M_peak - 5.0 * M_left_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak + 5.0 * M_right_sigma) + "))";

    // deltaE direction criteria
    std::string condition_deltaE = "((" + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + "< " + std::string(deltaE_name_) + ") && (" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak + 5.0 * deltaE_right_sigma) + "))";

    // BDT criteria
    std::string condition_BDT = "(" + std::to_string(BDT_cut_) + "<" + std::string(BDT_name_) + ")";

    std::string total = "(" + condition_M + "&&" + condition_deltaE + "&&" + condition_BDT + ")";

    return total;
}

std::string get_region_one_B(const char* filename_, const char* deltaE_name_, const char* M_name_, const char* BDT_name_, double BDT_cut_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // M direction criteria
    std::string condition_M = "(((" + std::to_string(M_peak - 10.0 * M_left_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak - 5.0 * M_left_sigma) + ")) || ((" + std::to_string(M_peak + 5.0 * M_right_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak + 10.0 * M_right_sigma) + ")))";

    // deltaE direction criteria
    std::string condition_deltaE = "((" + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + "< " + std::string(deltaE_name_) + ") && (" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak + 5.0 * deltaE_right_sigma) + "))";

    // BDT criteria
    std::string condition_BDT = "(" + std::to_string(BDT_cut_) + "<" + std::string(BDT_name_) + ")";

    std::string total = "(" + condition_M + "&&" + condition_deltaE + "&&" + condition_BDT + ")";

    return total;
}

std::string get_region_one_C(const char* filename_, const char* deltaE_name_, const char* M_name_, const char* BDT_name_, double BDT_cut_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // M direction criteria
    std::string condition_M = "((" + std::to_string(M_peak - 5.0 * M_left_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak + 5.0 * M_right_sigma) + "))";

    // deltaE direction criteria
    std::string condition_deltaE = "((" + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + "< " + std::string(deltaE_name_) + ") && (" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak + 5.0 * deltaE_right_sigma) + "))";

    // BDT criteria
    std::string condition_BDT = "((" + std::to_string(0.5) + "<" + std::string(BDT_name_) + ") && (" + std::string(BDT_name_) + "<" + std::to_string(BDT_cut_) + "))";

    std::string total = "(" + condition_M + "&&" + condition_deltaE + "&&" + condition_BDT + ")";

    return total;
}

std::string get_region_one_D(const char* filename_, const char* deltaE_name_, const char* M_name_, const char* BDT_name_, double BDT_cut_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // M direction criteria
    std::string condition_M = "(((" + std::to_string(M_peak - 10.0 * M_left_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak - 5.0 * M_left_sigma) + ")) || ((" + std::to_string(M_peak + 5.0 * M_right_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak + 10.0 * M_right_sigma) + ")))";

    // deltaE direction criteria
    std::string condition_deltaE = "((" + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + "< " + std::string(deltaE_name_) + ") && (" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak + 5.0 * deltaE_right_sigma) + "))";

    // BDT criteria
    std::string condition_BDT = "((" + std::to_string(0.5) + "<" + std::string(BDT_name_) + ") && (" + std::string(BDT_name_) + "<" + std::to_string(BDT_cut_) + "))";

    std::string total = "(" + condition_M + "&&" + condition_deltaE + "&&" + condition_BDT + ")";

    return total;
}

std::string get_region_one_Aprime(const char* filename_, const char* deltaE_name_, const char* M_name_, const char* BDT_name_, double BDT_cut_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // M direction criteria
    std::string condition_M = "(((" + std::to_string(M_peak - 7.5 * M_left_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak - 5.0 * M_left_sigma) + ")) || ((" + std::to_string(M_peak + 5.0 * M_right_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak + 7.5 * M_right_sigma) + ")))";

    // deltaE direction criteria
    std::string condition_deltaE = "((" + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + "< " + std::string(deltaE_name_) + ") && (" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak + 5.0 * deltaE_right_sigma) + "))";

    // BDT criteria
    std::string condition_BDT = "(" + std::to_string(BDT_cut_) + "<" + std::string(BDT_name_) + ")";

    std::string total = "(" + condition_M + "&&" + condition_deltaE + "&&" + condition_BDT + ")";

    return total;
}

std::string get_region_one_Bprime(const char* filename_, const char* deltaE_name_, const char* M_name_, const char* BDT_name_, double BDT_cut_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // M direction criteria
    std::string condition_M = "(((" + std::to_string(M_peak - 10.0 * M_left_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak - 7.5 * M_left_sigma) + ")) || ((" + std::to_string(M_peak + 7.5 * M_right_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak + 10.0 * M_right_sigma) + ")))";

    // deltaE direction criteria
    std::string condition_deltaE = "((" + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + "< " + std::string(deltaE_name_) + ") && (" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak + 5.0 * deltaE_right_sigma) + "))";

    // BDT criteria
    std::string condition_BDT = "(" + std::to_string(BDT_cut_) + "<" + std::string(BDT_name_) + ")";

    std::string total = "(" + condition_M + "&&" + condition_deltaE + "&&" + condition_BDT + ")";

    return total;
}

std::string get_region_one_Cprime(const char* filename_, const char* deltaE_name_, const char* M_name_, const char* BDT_name_, double BDT_cut_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // M direction criteria
    std::string condition_M = "(((" + std::to_string(M_peak - 7.5 * M_left_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak - 5.0 * M_left_sigma) + ")) || ((" + std::to_string(M_peak + 5.0 * M_right_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak + 7.5 * M_right_sigma) + ")))";

    // deltaE direction criteria
    std::string condition_deltaE = "((" + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + "< " + std::string(deltaE_name_) + ") && (" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak + 5.0 * deltaE_right_sigma) + "))";

    // BDT criteria
    std::string condition_BDT = "((" + std::to_string(0.5) + "<" + std::string(BDT_name_) + ") && (" + std::string(BDT_name_) + "<" + std::to_string(BDT_cut_) + "))";

    std::string total = "(" + condition_M + "&&" + condition_deltaE + "&&" + condition_BDT + ")";

    return total;
}

std::string get_region_one_Dprime(const char* filename_, const char* deltaE_name_, const char* M_name_, const char* BDT_name_, double BDT_cut_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // M direction criteria
    std::string condition_M = "(((" + std::to_string(M_peak - 10.0 * M_left_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak - 7.5 * M_left_sigma) + ")) || ((" + std::to_string(M_peak + 7.5 * M_right_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak + 10.0 * M_right_sigma) + ")))";

    // deltaE direction criteria
    std::string condition_deltaE = "((" + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + "< " + std::string(deltaE_name_) + ") && (" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak + 5.0 * deltaE_right_sigma) + "))";

    // BDT criteria
    std::string condition_BDT = "((" + std::to_string(0.5) + "<" + std::string(BDT_name_) + ") && (" + std::string(BDT_name_) + "<" + std::to_string(BDT_cut_) + "))";

    std::string total = "(" + condition_M + "&&" + condition_deltaE + "&&" + condition_BDT + ")";

    return total;
}

std::string get_region_two(const char* filename_, const char* deltaE_name_, const char* M_name_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // M direction criteria
    std::string condition_M = "((" + std::to_string(M_peak - 5.0 * M_left_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak + 5.0 * M_right_sigma) + "))";

    // deltaE direction criteria
    std::string condition_deltaE = "((" + std::to_string(deltaE_peak - 15.0 * deltaE_left_sigma) + "< " + std::string(deltaE_name_) + ") && (" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + "))";

    std::string total = "(" + condition_M + "&&" + condition_deltaE + ")";

    return total;
}

std::string get_region_two_A(const char* filename_, const char* deltaE_name_, const char* M_name_, const char* BDT_name_, double BDT_cut_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // M direction criteria
    std::string condition_M = "((" + std::to_string(M_peak - 5.0 * M_left_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak + 5.0 * M_right_sigma) + "))";

    // deltaE direction criteria
    std::string condition_deltaE = "((" + std::to_string(deltaE_peak - 15.0 * deltaE_left_sigma) + "< " + std::string(deltaE_name_) + ") && (" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + "))";

    // BDT criteria
    std::string condition_BDT = "(" + std::to_string(BDT_cut_) + "<" + std::string(BDT_name_) + ")";

    std::string total = "(" + condition_M + "&&" + condition_deltaE + "&&" + condition_BDT + ")";

    return total;
}

std::string get_region_two_B(const char* filename_, const char* deltaE_name_, const char* M_name_, const char* BDT_name_, double BDT_cut_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // M direction criteria
    std::string condition_M = "(((" + std::to_string(M_peak - 10.0 * M_left_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak - 5.0 * M_left_sigma) + ")) || ((" + std::to_string(M_peak + 5.0 * M_right_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak + 10.0 * M_right_sigma) + ")))";

    // deltaE direction criteria
    std::string condition_deltaE = "((" + std::to_string(deltaE_peak - 15.0 * deltaE_left_sigma) + "< " + std::string(deltaE_name_) + ") && (" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + "))";

    // BDT criteria
    std::string condition_BDT = "(" + std::to_string(BDT_cut_) + "<" + std::string(BDT_name_) + ")";

    std::string total = "(" + condition_M + "&&" + condition_deltaE + "&&" + condition_BDT + ")";

    return total;
}

std::string get_region_two_C(const char* filename_, const char* deltaE_name_, const char* M_name_, const char* BDT_name_, double BDT_cut_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // M direction criteria
    std::string condition_M = "((" + std::to_string(M_peak - 5.0 * M_left_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak + 5.0 * M_right_sigma) + "))";

    // deltaE direction criteria
    std::string condition_deltaE = "((" + std::to_string(deltaE_peak - 15.0 * deltaE_left_sigma) + "< " + std::string(deltaE_name_) + ") && (" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + "))";

    // BDT criteria
    std::string condition_BDT = "((" + std::to_string(0.5) + "<" + std::string(BDT_name_) + ") && (" + std::string(BDT_name_) + "<" + std::to_string(BDT_cut_) + "))";

    std::string total = "(" + condition_M + "&&" + condition_deltaE + "&&" + condition_BDT + ")";

    return total;
}

std::string get_region_two_D(const char* filename_, const char* deltaE_name_, const char* M_name_, const char* BDT_name_, double BDT_cut_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // M direction criteria
    std::string condition_M = "(((" + std::to_string(M_peak - 10.0 * M_left_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak - 5.0 * M_left_sigma) + ")) || ((" + std::to_string(M_peak + 5.0 * M_right_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak + 10.0 * M_right_sigma) + ")))";

    // deltaE direction criteria
    std::string condition_deltaE = "((" + std::to_string(deltaE_peak - 15.0 * deltaE_left_sigma) + "< " + std::string(deltaE_name_) + ") && (" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + "))";

    // BDT criteria
    std::string condition_BDT = "((" + std::to_string(0.5) + "<" + std::string(BDT_name_) + ") && (" + std::string(BDT_name_) + "<" + std::to_string(BDT_cut_) + "))";

    std::string total = "(" + condition_M + "&&" + condition_deltaE + "&&" + condition_BDT + ")";

    return total;
}

std::string get_region_two_Aprime(const char* filename_, const char* deltaE_name_, const char* M_name_, const char* BDT_name_, double BDT_cut_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // M direction criteria
    std::string condition_M = "(((" + std::to_string(M_peak - 7.5 * M_left_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak - 5.0 * M_left_sigma) + ")) || ((" + std::to_string(M_peak + 5.0 * M_right_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak + 7.5 * M_right_sigma) + ")))";

    // deltaE direction criteria
    std::string condition_deltaE = "((" + std::to_string(deltaE_peak - 15.0 * deltaE_left_sigma) + "< " + std::string(deltaE_name_) + ") && (" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + "))";

    // BDT criteria
    std::string condition_BDT = "(" + std::to_string(BDT_cut_) + "<" + std::string(BDT_name_) + ")";

    std::string total = "(" + condition_M + "&&" + condition_deltaE + "&&" + condition_BDT + ")";

    return total;
}

std::string get_region_two_Bprime(const char* filename_, const char* deltaE_name_, const char* M_name_, const char* BDT_name_, double BDT_cut_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // M direction criteria
    std::string condition_M = "(((" + std::to_string(M_peak - 10.0 * M_left_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak - 7.5 * M_left_sigma) + ")) || ((" + std::to_string(M_peak + 7.5 * M_right_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak + 10.0 * M_right_sigma) + ")))";

    // deltaE direction criteria
    std::string condition_deltaE = "((" + std::to_string(deltaE_peak - 15.0 * deltaE_left_sigma) + "< " + std::string(deltaE_name_) + ") && (" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + "))";

    // BDT criteria
    std::string condition_BDT = "(" + std::to_string(BDT_cut_) + "<" + std::string(BDT_name_) + ")";

    std::string total = "(" + condition_M + "&&" + condition_deltaE + "&&" + condition_BDT + ")";

    return total;
}

std::string get_region_two_Cprime(const char* filename_, const char* deltaE_name_, const char* M_name_, const char* BDT_name_, double BDT_cut_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // M direction criteria
    std::string condition_M = "(((" + std::to_string(M_peak - 7.5 * M_left_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak - 5.0 * M_left_sigma) + ")) || ((" + std::to_string(M_peak + 5.0 * M_right_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak + 7.5 * M_right_sigma) + ")))";

    // deltaE direction criteria
    std::string condition_deltaE = "((" + std::to_string(deltaE_peak - 15.0 * deltaE_left_sigma) + "< " + std::string(deltaE_name_) + ") && (" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + "))";

    // BDT criteria
    std::string condition_BDT = "((" + std::to_string(0.5) + "<" + std::string(BDT_name_) + ") && (" + std::string(BDT_name_) + "<" + std::to_string(BDT_cut_) + "))";

    std::string total = "(" + condition_M + "&&" + condition_deltaE + "&&" + condition_BDT + ")";

    return total;
}

std::string get_region_two_Dprime(const char* filename_, const char* deltaE_name_, const char* M_name_, const char* BDT_name_, double BDT_cut_) {
    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution(filename_, &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    // M direction criteria
    std::string condition_M = "(((" + std::to_string(M_peak - 10.0 * M_left_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak - 7.5 * M_left_sigma) + ")) || ((" + std::to_string(M_peak + 7.5 * M_right_sigma) + "<" + std::string(M_name_) + ") && (" + std::string(M_name_) + "< " + std::to_string(M_peak + 10.0 * M_right_sigma) + ")))";

    // deltaE direction criteria
    std::string condition_deltaE = "((" + std::to_string(deltaE_peak - 15.0 * deltaE_left_sigma) + "< " + std::string(deltaE_name_) + ") && (" + std::string(deltaE_name_) + " < " + std::to_string(deltaE_peak - 5.0 * deltaE_left_sigma) + "))";

    // BDT criteria
    std::string condition_BDT = "((" + std::to_string(0.5) + "<" + std::string(BDT_name_) + ") && (" + std::string(BDT_name_) + "<" + std::to_string(BDT_cut_) + "))";

    std::string total = "(" + condition_M + "&&" + condition_deltaE + "&&" + condition_BDT + ")";

    return total;
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

#endif 