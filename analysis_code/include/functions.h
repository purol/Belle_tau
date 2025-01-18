#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>

void ReadResolution(const char* filename_, double* deltaE_peak_, double* deltaE_left_sigma_, double* deltaE_right_sigma_, double* M_peak_, double* M_left_sigma_, double* M_right_sigma_) {
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
}

#endif 