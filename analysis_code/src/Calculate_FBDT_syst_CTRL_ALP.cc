#include <stdio.h>
#include <string>
#include <vector>
#include <format>

#include "TH1D.h"
#include "RooRealVar.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "functions.h"
#include "FBDTEfficiency.h"

ABCDParameters parameters;

double mapping_nominal(std::vector<double> variables_) {
    return mapping_function_ABCD(variables_, parameters, false);
}

double mapping_before_BDT(std::vector<double> variables_) {
    // count central M events before the BDT cut in A1 and A2
    variables_.at(2) = 2.0;
    variables_.at(3) = 2.0;
    return mapping_function_ABCD(variables_, parameters, false);
}

int main(int argc, char* argv[]) {
    /*
    * argv[1]: nominal analysis path
    * argv[2]: nominal signal sample
    * argv[3]: control analysis path
    * argv[4]: control MC list (separated by colon)
    * argv[5]: control data list (separated by colon)
    * argv[6]: output path
    * argv[7]: mass
    * argv[8]: lifetime
    * argv[9]: A constant
    * argv[10]: B constant
    */

    if (argc != 11) {
        printf("10 arguments are required.\n");
        return 1;
    }

    std::string nominal_path = argv[1];
    std::string control_path = argv[3];
    std::vector<std::string> MC_list = split(argv[4], ':');
    std::vector<std::string> data_list = split(argv[5], ':');

    double mass = std::stod(argv[7]);
    double life = std::stod(argv[8]);
    int A = std::stoi(argv[9]);
    int B = std::stoi(argv[10]);
    if (!(life > 0.0)) {
        printf("Invalid ALP lifetime.\n");
        return 1;
    }

    std::string suffix = std::format("_{:g}_{:g}_{}_{}", mass, life, A, B);
    std::string weight_suffix = std::string("_") + argv[7] + "_" + argv[8] + "_" + argv[9] + "_" + argv[10];
    std::string strA = (A < 0) ? "m" + std::to_string(-A) : std::to_string(A);
    std::string strB = (B < 0) ? "m" + std::to_string(-B) : std::to_string(B);
    std::string score_suffix = std::format("_{:g}_{:g}_{}_{}", mass, life, strA, strB);
    std::string signal_file = std::format("alpha_mass{:g}_life{:g}_A{}_B{}_", mass, life, A, B);

    double width = 0.075;
    if (life < 0.7) width = 0.025;
    else if (life < 7.0) width = 0.03;
    else if (life < 70.0) width = 0.035;
    std::string cut_m_alpha = "(" + std::to_string(mass - width) + " < extraInfo__boALP_M__bc) && (extraInfo__boALP_M__bc < " + std::to_string(mass + width) + ")";

    // get nominal BDT cuts and signal region
    ReadFOM((nominal_path + "/GridSearch_one/FOM" + suffix + ".log").c_str(), &parameters.BDT_cut_1);
    ReadFOM((nominal_path + "/GridSearch_two/FOM" + suffix + ".log").c_str(), &parameters.BDT_cut_2);
    ReadABCDBoundary((nominal_path + "/ABCD_boundary" + suffix + ".txt").c_str(), { parameters.BDT_cut_1, parameters.BDT_cut_2 }, &parameters.sideband_BDT_cut_1, &parameters.sideband_BDT_cut_2);
    double theta;
    ReadResolution((nominal_path + "/" + signal_file + "M_deltaE_result.txt").c_str(), &parameters.deltaE_peak, &parameters.deltaE_left_sigma, &parameters.deltaE_right_sigma,
        &parameters.M_peak, &parameters.M_left_sigma, &parameters.M_right_sigma, &theta);
    parameters.M_size = sizeM;
    std::string cut_region = GetBCSCut(parameters.deltaE_peak, parameters.deltaE_left_sigma, parameters.deltaE_right_sigma, parameters.M_peak, parameters.M_left_sigma, parameters.M_right_sigma);
    std::vector<std::string> BDT_names = { "BDT_output_1" + score_suffix, "BDT_output_2" + score_suffix };

    EventWeights::Register("MC_weight", MC_weight);
    EventWeights::Register("muonID_01_ALP", muonID_01_ALP);
    EventWeights::Register("muonID_05_ALP", muonID_05_ALP);
    EventWeights::Register("KS0_tracking_run1", KS0_tracking_MC16rd_run1);
    EventWeights::Register("KS0_tracking_run2", KS0_tracking_MC16rd_run2);

    // signal MC
    TH1D before_BDT("before_BDT", ";bin index;", 8, 0.5, 8.5);
    TH1D after_BDT("after_BDT", ";bin index;", 8, 0.5, 8.5);
    Loader loader_signal("tau_lfv");
    loader_signal.Load((nominal_path + "/" + argv[2] + "/final_output_after_application").c_str(), signal_file.c_str(), argv[2]);
    loader_signal.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_signal.AddWeight("muonID_05_ALP", { {"index", "first_muon_index"}, {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
    loader_signal.AddWeight("muonID_05_ALP", { {"index", "second_muon_index"}, {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
    loader_signal.AddWeight("muonID_01_ALP", { {"index", "third_muon_index"}, {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
    loader_signal.AddWeight("KS0_tracking_run1", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
    loader_signal.AddWeight("KS0_tracking_run2", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
    loader_signal.Cut(cut_region.c_str());
    loader_signal.Cut(cut_m_alpha.c_str());
    loader_signal.RandomBCS();
    loader_signal.IsBCSValid();
    loader_signal.FillCustomizedTH1D(&before_BDT, { "M", "deltaE", BDT_names.at(0), BDT_names.at(1) }, mapping_before_BDT);
    loader_signal.FillCustomizedTH1D(&after_BDT, { "M", "deltaE", BDT_names.at(0), BDT_names.at(1) }, mapping_nominal);
    loader_signal.end();

    // get control efficiencies
    std::vector<std::string> regions = { "one", "two" };
    std::vector<double> nominal_high = { parameters.BDT_cut_1, parameters.BDT_cut_2 };
    std::vector<double> nominal_low = { parameters.sideband_BDT_cut_1, parameters.sideband_BDT_cut_2 };
    FBDTResult results[2];
    for (int i = 0; i < 2; i++) {
        std::string weight_name = "weight_" + regions.at(i) + "_2D_CTRL" + weight_suffix;
        std::string weight_path = control_path + "/Weight/" + weight_name;
        std::string var1, var2;
        if (!ReadFBDTWeightInfo(weight_path + "_info.txt", var1, var2)) return 1;
        EventWeight CTRL_weight(weight_path + ".csv",
            { {var1, var1 + "_min", var1 + "_max"}, {var2, var2 + "_min", var2 + "_max"} }, "weight", true);
        EventWeights::Register(weight_name, CTRL_weight);

        RooRealVar BDT("score", "score", 0.5);
        RooRealVar weight("weight", "weight", 1.0);

        // control MC
        FBDTDistribution MC;
        {
            RooDataSet control_MC("control_MC", "control_MC", RooArgSet(BDT, weight), RooFit::WeightVar("weight"));
            Loader loader_MC("tau_lfv");
            for (int j = 0; j < MC_list.size(); j++) loader_MC.Load((control_path + "/" + MC_list.at(j) + "/final_output_after_application").c_str(), "root", MC_list.at(j).c_str());
            loader_MC.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
            loader_MC.AddWeight(weight_name.c_str(), { {var1, var1}, {var2, var2} });
            loader_MC.FillDataSet(&control_MC, { &BDT }, { BDT_names.at(i) });
            loader_MC.end();
            if (!GetFBDTDistribution(control_MC, BDT_names.at(i), MC)) return 1;
        }

        // control data
        FBDTDistribution data;
        {
            RooDataSet control_data("control_data", "control_data", RooArgSet(BDT, weight), RooFit::WeightVar("weight"));
            Loader loader_data("tau_lfv");
            for (int j = 0; j < data_list.size(); j++) loader_data.Load((control_path + "/" + data_list.at(j) + "/final_output_after_application").c_str(), "root", data_list.at(j).c_str());
            loader_data.AddWeight(weight_name.c_str(), { {var1, var1}, {var2, var2} });
            loader_data.FillDataSet(&control_data, { &BDT }, { BDT_names.at(i) });
            loader_data.end();
            if (!GetFBDTDistribution(control_data, BDT_names.at(i), data)) return 1;
        }

        // high and low BDT efficiencies use the same pre-BDT denominator
        const int bin_A = 4 * i + 1;
        results[i] = CalculateFBDTEfficiency(MC, data, nominal_low.at(i), nominal_high.at(i),
            before_BDT.GetBinContent(bin_A), after_BDT.GetBinContent(bin_A), after_BDT.GetBinContent(bin_A + 2));
    }

    // print result
    if (!WriteFBDTResult(std::string(argv[6]) + "/FBDTSyst" + suffix + ".csv", results)) return 1;
    return 0;
}
