#ifndef FBDTEFFICIENCY_H
#define FBDTEFFICIENCY_H

#include <stdio.h>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iterator>
#include <limits>
#include <map>
#include <string>

#include "RooDataSet.h"
#include "RooArgSet.h"

struct FBDTYield {
    double sum = 0.0;
    double sum2 = 0.0;
};

struct FBDTDistribution {
    std::map<double, FBDTYield, std::greater<double>> bins;
    double total = 0.0;
};

struct FBDTResult {
    double nominal_low_cut, nominal_high_cut;
    double control_low_cut = NAN, control_high_cut = NAN;
    double target_eff[2] = { NAN, NAN };
    double eff_MC[2], eff_data[2];
    double ratio[2] = { NAN, NAN };
    double stat_err[2] = { NAN, NAN };
};

inline bool ReadFBDTWeightInfo(const std::string& filename_, std::string& var1_, std::string& var2_) {
    std::ifstream input(filename_);
    double original_D;
    std::string minimum_D;
    if (input >> original_D >> minimum_D) {
        // GetWeight writes the minimum D and the first variable without a separator
        char* first_variable;
        errno = 0;
        std::strtod(minimum_D.c_str(), &first_variable);
        if (first_variable != minimum_D.c_str() && errno != ERANGE) {
            var1_ = first_variable;
            if (var1_.empty()) input >> var1_;
            if (input >> var2_) return true;
        }
    }
    printf("Cannot read %s\n", filename_.c_str());
    return false;
}

inline bool GetFBDTDistribution(RooDataSet& dataset_, const std::string& BDT_name_, FBDTDistribution& distribution_) {
    for (int i = 0; i < dataset_.numEntries(); i++) {
        const double BDT = dataset_.get(i)->getRealValue("score");
        const double weight = dataset_.weight();
        if (!std::isfinite(weight) || weight < 0.0 || !std::isfinite(BDT) || BDT < 0.0 || BDT > 1.0) {
            printf("Invalid weight or BDT score in %s\n", BDT_name_.c_str());
            return false;
        }
        if (weight == 0.0) continue;
        FBDTYield& bin = distribution_.bins[BDT];
        bin.sum += weight;
        bin.sum2 += weight * weight;
        distribution_.total += weight;
    }
    return true;
}

inline double FindControlCut(const FBDTDistribution& MC_, double upper_, double target_eff_) {
    // keep events with the same BDT output on the same side of the cut
    double cut = upper_;
    const double target = MC_.total * target_eff_;
    double best_difference = std::fabs(target);
    double selected = 0.0;
    for (auto it = MC_.bins.begin(); it != MC_.bins.end(); ++it) {
        if (it->first >= upper_) continue;
        selected += it->second.sum;
        const double difference = std::fabs(selected - target);
        if (difference < best_difference) {
            best_difference = difference;
            auto next = std::next(it);
            if (next == MC_.bins.end()) cut = -std::numeric_limits<double>::epsilon();
            else {
                cut = next->first + (it->first - next->first) / 2.0;
                if (cut >= it->first) cut = next->first;
            }
        }
    }
    return cut;
}

inline void GetFBDTEfficiency(const FBDTDistribution& sample_, double lower_, double upper_, double* eff_, double* variance_) {
    *eff_ = NAN;
    *variance_ = NAN;
    if (sample_.total <= 0.0 || !std::isfinite(lower_) || !std::isfinite(upper_)) return;

    FBDTYield pass, fail;
    for (const auto& bin : sample_.bins) {
        FBDTYield& yield = (lower_ < bin.first && bin.first < upper_) ? pass : fail;
        yield.sum += bin.second.sum;
        yield.sum2 += bin.second.sum2;
    }
    const double eff = pass.sum / sample_.total;
    *eff_ = eff;
    *variance_ = (std::pow(1.0 - eff, 2) * pass.sum2 + eff * eff * fail.sum2) / std::pow(sample_.total, 2);
}

inline FBDTResult CalculateFBDTEfficiency(const FBDTDistribution& MC_, const FBDTDistribution& data_, double nominal_low_cut_, double nominal_high_cut_, double before_BDT_, double high_yield_, double low_yield_) {
    FBDTResult result;
    result.nominal_low_cut = nominal_low_cut_;
    result.nominal_high_cut = nominal_high_cut_;
    const double above_max = std::nextafter(1.0, 2.0);

    // match high and low efficiencies to nominal signal MC
    if (before_BDT_ > 0.0) {
        result.target_eff[0] = high_yield_ / before_BDT_;
        result.target_eff[1] = low_yield_ / before_BDT_;
        if (MC_.total > 0.0) {
            result.control_high_cut = FindControlCut(MC_, above_max, result.target_eff[0]);
            result.control_low_cut = FindControlCut(MC_, result.control_high_cut, result.target_eff[1]);
        }
    }

    // get data/MC ratio
    for (int i = 0; i < 2; i++) {
        const double lower = (i == 0) ? result.control_high_cut : result.control_low_cut;
        const double upper = (i == 0) ? above_max : result.control_high_cut;
        double variance_MC, variance_data;
        GetFBDTEfficiency(MC_, lower, upper, &result.eff_MC[i], &variance_MC);
        GetFBDTEfficiency(data_, lower, upper, &result.eff_data[i], &variance_data);
        if (result.eff_MC[i] > 0.0 && std::isfinite(result.eff_data[i])) {
            const double ratio = result.eff_data[i] / result.eff_MC[i];
            result.ratio[i] = ratio;
            result.stat_err[i] = std::sqrt(variance_data + ratio * ratio * variance_MC) / result.eff_MC[i];
        }
    }
    return result;
}

inline bool WriteFBDTResult(const std::string& filename_, const FBDTResult (&results_)[2]) {
    FILE* fp = fopen(filename_.c_str(), "w");
    if (fp == nullptr) {
        printf("Cannot open %s\n", filename_.c_str());
        return false;
    }
    fprintf(fp, "region,bdt_range,nominal_low_cut,nominal_high_cut,control_low_cut,control_high_cut,target_eff,control_eff_MC,control_eff_data,data_over_MC,stat_err,abs_syst,valid\n");
    for (int i = 0; i < 2; i++) {
        const FBDTResult& result = results_[i];
        for (int j = 0; j < 2; j++) {
            const bool valid = std::isfinite(result.ratio[j]) && std::isfinite(result.stat_err[j]);
            fprintf(fp, "%d,%s,%.17g,%.17g,%.17g,%.17g", i + 1, (j == 0) ? "high" : "low", result.nominal_low_cut, result.nominal_high_cut, result.control_low_cut, result.control_high_cut);
            fprintf(fp, ",%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%d\n", result.target_eff[j], result.eff_MC[j], result.eff_data[j], result.ratio[j], result.stat_err[j], std::fabs(result.ratio[j] - 1.0), valid);
        }
    }
    const int write_error = ferror(fp);
    if (fclose(fp) != 0 || write_error) {
        printf("Cannot write %s\n", filename_.c_str());
        return false;
    }
    return true;
}

#endif
