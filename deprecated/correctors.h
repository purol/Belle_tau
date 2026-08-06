#ifndef CORRECTORS_H
#define CORRECTORS_H

#include <cmath>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <random>

#include "TH1.h"
#include "TH2.h"

class Corrector_PID {
private:
    struct Entry {
        double p_min, p_max;
        double theta_min, theta_max;
        std::string charge;
        double data_MC_ratio;
        double data_MC_uncertainty_statsys_dn;
        double data_MC_uncertainty_statsys_up;

        // it is fluctuated data_MC_ratio. If FluctuateCorrectionFactor() is not called, it is nothing but `data_MC_ratio`
        double data_MC_ratio_fluctuated;
    };

    std::vector<Entry> table;

    // for the random fluctuation
    std::random_device rd;
    std::mt19937 gen;
    std::normal_distribution<double> Normal_distribution;

public:
    Corrector_PID(const std::string& filename) : gen(rd()), Normal_distribution(0.0, 1.0) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file");
        }

        std::string line;
        std::getline(file, line); // Read header
        std::istringstream header_ss(line);
        std::unordered_map<std::string, int> column_map;
        std::string column;
        int index = 0;
        while (std::getline(header_ss, column, ',')) {
            column_map[column] = index++;
        }

        // Check if required columns exist
        if (column_map.find("charge") == column_map.end() ||
            column_map.find("p_min") == column_map.end() ||
            column_map.find("p_max") == column_map.end() ||
            column_map.find("theta_min") == column_map.end() ||
            column_map.find("theta_max") == column_map.end() ||
            column_map.find("data_MC_ratio") == column_map.end() ||
            column_map.find("data_MC_uncertainty_statsys_dn") == column_map.end() ||
            column_map.find("data_MC_uncertainty_statsys_up") == column_map.end()) {
            throw std::runtime_error("Missing required columns in CSV file");
        }

        while (std::getline(file, line)) {
            std::istringstream ss(line);
            std::vector<std::string> tokens;
            std::string token;
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }

            if (tokens.size() <= std::max({ column_map["charge"], column_map["p_min"], column_map["p_max"],
                                            column_map["theta_min"], column_map["theta_max"], column_map["data_MC_ratio"],
                                            column_map["data_MC_uncertainty_statsys_dn"], column_map["data_MC_uncertainty_statsys_up"] })) {
                continue; // Skip malformed lines
            }

            Entry entry;
            entry.charge = tokens[column_map["charge"]];
            entry.p_min = std::stod(tokens[column_map["p_min"]]);
            entry.p_max = std::stod(tokens[column_map["p_max"]]);
            entry.theta_min = std::stod(tokens[column_map["theta_min"]]);
            entry.theta_max = std::stod(tokens[column_map["theta_max"]]);
            entry.data_MC_ratio = std::stod(tokens[column_map["data_MC_ratio"]]);
            entry.data_MC_uncertainty_statsys_dn = std::stod(tokens[column_map["data_MC_uncertainty_statsys_dn"]]);
            entry.data_MC_uncertainty_statsys_up = std::stod(tokens[column_map["data_MC_uncertainty_statsys_up"]]);
            entry.data_MC_ratio_fluctuated = entry.data_MC_ratio;

            table.push_back(entry);
        }
    }

    double GetCorrectionFactor(double p, double theta, const std::string& charge) const {
        for (size_t i = 0; i < table.size(); ++i) {
            if (table[i].charge == charge &&
                table[i].p_min <= p && p < table[i].p_max &&
                table[i].theta_min <= theta && theta < table[i].theta_max) {
                return table[i].data_MC_ratio;
            }
        }
        return 1.0; // Indicate not found. Just return 1.0
    }

    double GetUncertaintyUp(double p, double theta, const std::string& charge) const {
        for (size_t i = 0; i < table.size(); ++i) {
            if (table[i].charge == charge &&
                table[i].p_min <= p && p < table[i].p_max &&
                table[i].theta_min <= theta && theta < table[i].theta_max) {
                return table[i].data_MC_uncertainty_statsys_up;
            }
        }
        return 0.0; // Indicate not found. Just return 0.0
    }

    double GetUncertaintyDown(double p, double theta, const std::string& charge) const {
        for (size_t i = 0; i < table.size(); ++i) {
            if (table[i].charge == charge &&
                table[i].p_min <= p && p < table[i].p_max &&
                table[i].theta_min <= theta && theta < table[i].theta_max) {
                return table[i].data_MC_uncertainty_statsys_dn;
            }
        }
        return 0.0; // Indicate not found. Just return 0.0
    }

    void FluctuateCorrectionFactor() {
        for (size_t i = 0; i < table.size(); ++i) {
            double random_number = Normal_distribution(gen);
            if (random_number > 0) table.at(i).data_MC_ratio_fluctuated = table.at(i).data_MC_ratio + random_number * table.at(i).data_MC_uncertainty_statsys_up;
            else table.at(i).data_MC_ratio_fluctuated = table.at(i).data_MC_ratio + random_number * table.at(i).data_MC_uncertainty_statsys_dn;
        }
    }

    double GetFluctuatedCorrectionFactor(double p, double theta, const std::string& charge) const {
        for (size_t i = 0; i < table.size(); ++i) {
            if (table[i].charge == charge &&
                table[i].p_min <= p && p < table[i].p_max &&
                table[i].theta_min <= theta && theta < table[i].theta_max) {
                return table[i].data_MC_ratio_fluctuated;
            }
        }
        return 1.0; // Indicate not found. Just return 1.0
    }

};






#endif 