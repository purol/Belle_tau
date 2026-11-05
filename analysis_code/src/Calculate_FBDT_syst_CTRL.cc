#include <stdio.h>
#include <cmath>
#include <format>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <limits>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "TH1D.h"
#include "RooRealVar.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "functions.h"

ABCDParameters parameters;

double mapping_nominal(std::vector<double> variables_) {
    return mapping_function_ABCD(variables_, parameters, false);
}

double mapping_before_BDT(std::vector<double> variables_) {
    // Count all central M events in A1 or A2 before the BDT selection.
    variables_.at(2) = 2.0;
    variables_.at(3) = 2.0;
    return mapping_function_ABCD(variables_, parameters, false);
}

void AddMCWeight(Loader& loader_) {
    loader_.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
}

struct Yield {
    double sum = 0.0;
    double sum2 = 0.0;
};

struct ScoreDistribution {
    std::map<double, Yield, std::greater<double>> bins;
    double total = 0.0;
};

ScoreDistribution GetControlDistribution(const std::string& path_, const std::vector<std::string>& samples_, const std::string& score_name_,
    const std::string& weight_name_, const std::string& var1_, const std::string& var2_, bool is_MC_) {
    RooRealVar score("score", "score", 0.5);
    RooRealVar weight("weight", "weight", 1.0);
    RooDataSet dataset("control", "control", RooArgSet(score, weight), RooFit::WeightVar("weight"));

    Loader loader("tau_lfv");
    for (const std::string& sample : samples_) loader.Load((path_ + "/" + sample + "/final_output_after_application").c_str(), "root", sample.c_str());
    if (is_MC_) AddMCWeight(loader);
    loader.AddWeight(weight_name_.c_str(), { {var1_, var1_}, {var2_, var2_} });
    loader.FillDataSet(&dataset, { &score }, { score_name_ });
    loader.end();

    ScoreDistribution result;
    for (int i = 0; i < dataset.numEntries(); i++) {
        const double value = dataset.get(i)->getRealValue("score");
        const double w = dataset.weight();
        if (!std::isfinite(w) || w < 0.0 || !std::isfinite(value) || value < 0.0 || value > 1.0) {
            throw std::runtime_error("Invalid weight or BDT score in " + score_name_);
        }
        if (w == 0.0) continue;
        Yield& bin = result.bins[value];
        bin.sum += w;
        bin.sum2 += w * w;
        result.total += w;
    }
    return result;
}

double FindControlCut(const ScoreDistribution& MC_, double upper_, double target_eff_) {
    // Match the fraction of the full pre-BDT sample in (cut, upper).
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

struct Efficiency {
    double value = NAN;
    double variance = NAN;
};

Efficiency GetEfficiency(const ScoreDistribution& sample_, double lower_, double upper_) {
    if (sample_.total <= 0.0 || !std::isfinite(lower_) || !std::isfinite(upper_)) return {};
    Yield pass;
    Yield fail;
    for (const auto& bin : sample_.bins) {
        Yield& yield = (lower_ < bin.first && bin.first < upper_) ? pass : fail;
        yield.sum += bin.second.sum;
        yield.sum2 += bin.second.sum2;
    }
    const double eff = pass.sum / sample_.total;
    const double variance = (std::pow(1.0 - eff, 2) * pass.sum2 + eff * eff * fail.sum2) / std::pow(sample_.total, 2);
    return { eff, variance };
}

int main(int argc, char* argv[]) {
    /*
    * argv[1]: nominal analysis path
    * argv[2]: nominal signal sample
    * argv[3]: control analysis path
    * argv[4]: control MC list (separated by colon)
    * argv[5]: control data list (separated by colon)
    * argv[6]: output path
    * (argv[7]): mass
    * (argv[8]): lifetime
    * (argv[9]): A constant
    * (argv[10]): B constant
    */

    if (argc != 7 && argc != 11) {
        printf("6 or 10 arguments are required.\n");
        return 1;
    }

    const std::string nominal_path = argv[1];
    const std::string control_path = argv[3];
    const bool is_ALP = (argc == 11);
    std::string suffix;
    std::string weight_suffix;
    std::string score_suffix;
    std::string signal_file = "root";
    std::string resolution_file = "M_deltaE_result.txt";
    std::string cut_m_alpha;
    if (is_ALP) {
        const double mass = std::stod(argv[7]);
        const double life = std::stod(argv[8]);
        const int A = std::stoi(argv[9]);
        const int B = std::stoi(argv[10]);
        if (!(life > 0.0)) throw std::runtime_error("Invalid ALP lifetime");
        suffix = std::format("_{:g}_{:g}_{}_{}", mass, life, A, B);
        weight_suffix = std::string("_") + argv[7] + "_" + argv[8] + "_" + argv[9] + "_" + argv[10];
        const std::string strA = (A < 0) ? "m" + std::to_string(-A) : std::to_string(A);
        const std::string strB = (B < 0) ? "m" + std::to_string(-B) : std::to_string(B);
        score_suffix = std::format("_{:g}_{:g}_{}_{}", mass, life, strA, strB);
        signal_file = std::format("alpha_mass{:g}_life{:g}_A{}_B{}_", mass, life, A, B);
        resolution_file = signal_file + resolution_file;
        double width = 0.075;
        if (life < 0.7) width = 0.025;
        else if (life < 7.0) width = 0.03;
        else if (life < 70.0) width = 0.035;
        cut_m_alpha = "(" + std::to_string(mass - width) + " < extraInfo__boALP_M__bc) && (extraInfo__boALP_M__bc < " + std::to_string(mass + width) + ")";
    }

    // get nominal BDT cuts and signal region
    ReadFOM((nominal_path + "/GridSearch_one/FOM" + suffix + ".log").c_str(), &parameters.BDT_cut_1);
    ReadFOM((nominal_path + "/GridSearch_two/FOM" + suffix + ".log").c_str(), &parameters.BDT_cut_2);
    ReadABCDBoundary((nominal_path + "/ABCD_boundary" + suffix + ".txt").c_str(), { parameters.BDT_cut_1, parameters.BDT_cut_2 }, &parameters.sideband_BDT_cut_1, &parameters.sideband_BDT_cut_2);
    double theta;
    ReadResolution((nominal_path + "/" + resolution_file).c_str(), &parameters.deltaE_peak, &parameters.deltaE_left_sigma, &parameters.deltaE_right_sigma,
        &parameters.M_peak, &parameters.M_left_sigma, &parameters.M_right_sigma, &theta);
    parameters.M_size = sizeM;
    const std::string cut_region = GetBCSCut(parameters.deltaE_peak, parameters.deltaE_left_sigma, parameters.deltaE_right_sigma, parameters.M_peak, parameters.M_left_sigma, parameters.M_right_sigma);
    const std::vector<std::string> scores = { "BDT_output_1" + score_suffix, "BDT_output_2" + score_suffix };

    // nominal signal MC
    TH1D before_BDT("before_BDT", ";bin index;", 8, 0.5, 8.5);
    TH1D after_BDT("after_BDT", ";bin index;", 8, 0.5, 8.5);
    EventWeights::Register("MC_weight", MC_weight);
    Loader loader_signal("tau_lfv");
    loader_signal.Load((nominal_path + "/" + argv[2] + "/final_output_after_application").c_str(), signal_file.c_str(), argv[2]);
    AddMCWeight(loader_signal);
    if (is_ALP) {
        EventWeights::Register("muonID_01_ALP", muonID_01_ALP);
        EventWeights::Register("muonID_05_ALP", muonID_05_ALP);
        EventWeights::Register("KS0_tracking_run1", KS0_tracking_MC16rd_run1);
        EventWeights::Register("KS0_tracking_run2", KS0_tracking_MC16rd_run2);
        loader_signal.AddWeight("muonID_05_ALP", { {"index", "first_muon_index"}, {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
        loader_signal.AddWeight("muonID_05_ALP", { {"index", "second_muon_index"}, {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
        loader_signal.AddWeight("muonID_01_ALP", { {"index", "third_muon_index"}, {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
        loader_signal.AddWeight("KS0_tracking_run1", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
        loader_signal.AddWeight("KS0_tracking_run2", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
    }
    else {
        EventWeights::Register("muonID_01_prompt", muonID_01_prompt);
        EventWeights::Register("muonID_05_prompt", muonID_05_prompt);
        loader_signal.AddWeight("muonID_05_prompt", { {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
        loader_signal.AddWeight("muonID_05_prompt", { {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
        loader_signal.AddWeight("muonID_01_prompt", { {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
    }
    loader_signal.Cut(cut_region.c_str());
    if (is_ALP) loader_signal.Cut(cut_m_alpha.c_str());
    loader_signal.RandomBCS();
    loader_signal.IsBCSValid();
    loader_signal.FillCustomizedTH1D(&before_BDT, { "M", "deltaE", scores.at(0), scores.at(1) }, mapping_before_BDT);
    loader_signal.FillCustomizedTH1D(&after_BDT, { "M", "deltaE", scores.at(0), scores.at(1) }, mapping_nominal);
    loader_signal.end();

    // get control efficiencies
    const std::vector<std::string> regions = { "one", "two" };
    const std::vector<double> nominal_high = { parameters.BDT_cut_1, parameters.BDT_cut_2 };
    const std::vector<double> nominal_low = { parameters.sideband_BDT_cut_1, parameters.sideband_BDT_cut_2 };
    const double above_max = std::nextafter(1.0, 2.0);
    std::ostringstream rows;
    rows << std::setprecision(17);
    for (int i = 0; i < 2; i++) {
        const std::string weight_name = "weight_" + regions.at(i) + "_2D_CTRL" + weight_suffix;
        const std::string weight_path = control_path + "/Weight/" + weight_name;
        double original_D;
        std::string var1, var2;
        std::ifstream info(weight_path + "_info.txt");
        // GetWeight can write the minimum D and the first variable without a separator.
        std::string minimum_D;
        if (!(info >> original_D >> minimum_D)) throw std::runtime_error("Cannot read " + weight_path + "_info.txt");
        size_t pos;
        std::stod(minimum_D, &pos);
        var1 = minimum_D.substr(pos);
        if (var1.empty()) info >> var1;
        if (!(info >> var2)) throw std::runtime_error("Cannot read " + weight_path + "_info.txt");
        EventWeight CTRL_weight(weight_path + ".csv",
            { {var1, var1 + "_min", var1 + "_max"}, {var2, var2 + "_min", var2 + "_max"} }, "weight", true);
        EventWeights::Register(weight_name, CTRL_weight);
        ScoreDistribution MC = GetControlDistribution(control_path, split(argv[4], ':'), scores.at(i), weight_name, var1, var2, true);
        ScoreDistribution data = GetControlDistribution(control_path, split(argv[5], ':'), scores.at(i), weight_name, var1, var2, false);

        const int bin_A = 4 * i + 1;
        const double total = before_BDT.GetBinContent(bin_A);
        double target_high = NAN, target_low = NAN;
        double control_high = NAN, control_low = NAN;
        if (total > 0.0) {
            target_high = after_BDT.GetBinContent(bin_A) / total;
            target_low = after_BDT.GetBinContent(bin_A + 2) / total;
            if (MC.total > 0.0) {
                control_high = FindControlCut(MC, above_max, target_high);
                control_low = FindControlCut(MC, control_high, target_low);
            }
        }

        for (int j = 0; j < 2; j++) {
            const bool high = (j == 0);
            const double lower = high ? control_high : control_low;
            const double upper = high ? above_max : control_high;
            const Efficiency eff_MC = GetEfficiency(MC, lower, upper);
            const Efficiency eff_data = GetEfficiency(data, lower, upper);
            double ratio = NAN, error = NAN;
            if (eff_MC.value > 0.0 && std::isfinite(eff_data.value)) {
                ratio = eff_data.value / eff_MC.value;
                error = std::sqrt(eff_data.variance + ratio * ratio * eff_MC.variance) / eff_MC.value;
            }
            rows << i + 1 << ',' << (high ? "high" : "low") << ',' << nominal_low.at(i) << ',' << nominal_high.at(i)
                 << ',' << control_low << ',' << control_high << ',' << (high ? target_high : target_low)
                 << ',' << eff_MC.value << ',' << eff_data.value << ',' << ratio << ',' << error
                 << ',' << std::fabs(ratio - 1.0) << ',' << (std::isfinite(ratio) && std::isfinite(error)) << '\n';
        }
    }

    // print result
    const std::string filename = std::string(argv[6]) + "/FBDTSyst" + suffix + ".csv";
    std::ofstream output(filename);
    output << "region,bdt_range,nominal_low_cut,nominal_high_cut,control_low_cut,control_high_cut,target_eff,control_eff_MC,control_eff_data,data_over_MC,stat_err,abs_syst,valid\n";
    output << rows.str();
    output.close();
    if (!output) throw std::runtime_error("Cannot write " + filename);
    return 0;
}
