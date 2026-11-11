#include <stdio.h>
#include <string>
#include <vector>
#include <format>

#include "TH1D.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "functions.h"

ABCDParameters parameters;

double mapping_nominal(std::vector<double> variables_) {
    return mapping_function_ABCD(variables_, parameters, false);
}

double mapping_before_BDT(std::vector<double> variables_) {
    // count events before the BDT cut in A and B
    variables_.at(2) = 2.0;
    variables_.at(3) = 2.0;
    return mapping_function_ABCD(variables_, parameters, false);
}

int main(int argc, char* argv[]) {
    /*
    * argv[1]: input1 path1 (nominal signal MC)
    * argv[2]: input1 path2 (nominal signal MC)
    * argv[3]: input2 path1 (control sample)
    * argv[4]: input2 path2 (control sample)
    * argv[5]: output path
    * argv[6]: nominal signal sample
    * argv[7]: control MC list (separated by colon)
    * argv[8]: control data list (separated by colon)
    * argv[9]: weight path
    * argv[10]: mass
    * argv[11]: lifetime
    * argv[12]: A constant
    * argv[13]: B constant
    */

    if (argc != 14) {
        printf("13 arguments are required.\n");
        return 1;
    }

    std::string nominal_path = argv[1];
    std::string control_path = argv[3];
    std::vector<std::string> MC_list = split(argv[7], ':');
    std::vector<std::string> data_list = split(argv[8], ':');

    double mass = std::stod(argv[10]);
    double life = std::stod(argv[11]);
    int A = std::stoi(argv[12]);
    int B = std::stoi(argv[13]);

    double M_left_cut_value = 0;
    double M_right_cut_value = 0;
    if ((0 < life) && (life < 0.7)) {
        M_left_cut_value = 0.025;
        M_right_cut_value = 0.025;
    }
    else if ((0.7 <= life) && (life < 7)) {
        M_left_cut_value = 0.03;
        M_right_cut_value = 0.03;
    }
    else if ((7 <= life) && (life < 70)) {
        M_left_cut_value = 0.035;
        M_right_cut_value = 0.035;

    }
    else if (70 <= life) {
        M_left_cut_value = 0.075;
        M_right_cut_value = 0.075;

    }
    std::string cut_m_alpha = "(" + std::to_string(mass - M_left_cut_value) + " < extraInfo__boALP_M__bc) && (extraInfo__boALP_M__bc < " + std::to_string(mass + M_right_cut_value) + ")";

    // get nominal BDT cuts and signal region
    ReadFOM((nominal_path + "/GridSearch_one/FOM_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".log").c_str(), &parameters.BDT_cut_1);
    ReadFOM((nominal_path + "/GridSearch_two/FOM_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".log").c_str(), &parameters.BDT_cut_2);
    ReadABCDBoundary((nominal_path + "/ABCD_boundary_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".txt").c_str(), { parameters.BDT_cut_1, parameters.BDT_cut_2 }, &parameters.sideband_BDT_cut_1, &parameters.sideband_BDT_cut_2);
    double theta;

    ReadResolution((nominal_path + "/alpha_mass" + std::format("{:g}", mass) + "_life" + std::format("{:g}", life) + "_A" + std::to_string(A) + "_B" + std::to_string(B) + "_M_deltaE_result.txt").c_str(), &parameters.deltaE_peak, &parameters.deltaE_left_sigma, &parameters.deltaE_right_sigma, &parameters.M_peak, &parameters.M_left_sigma, &parameters.M_right_sigma, &theta);
    parameters.M_size = sizeM;
    std::string cut_region = GetBCSCut(parameters.deltaE_peak, parameters.deltaE_left_sigma, parameters.deltaE_right_sigma, parameters.M_peak, parameters.M_left_sigma, parameters.M_right_sigma);

    std::string strMass = std::format("{:g}", mass);
    std::string strLife = std::format("{:g}", life);
    std::string strA;
    std::string strB;
    if (A >= 0) strA = std::to_string(A);
    else strA = "m" + std::to_string(std::abs(A));
    if (B >= 0) strB = std::to_string(B);
    else strB = "m" + std::to_string(std::abs(B));

    std::vector<std::string> BDT_names;
    BDT_names.push_back("BDT_output_1_" + strMass + "_" + strLife + "_" + strA + "_" + strB);
    BDT_names.push_back("BDT_output_2_" + strMass + "_" + strLife + "_" + strA + "_" + strB);

    EventWeights::Register("MC_weight", MC_weight);
    EventWeights::Register("muonID_01_ALP", muonID_01_ALP);
    EventWeights::Register("muonID_05_ALP", muonID_05_ALP);
    EventWeights::Register("KS0_tracking_run1", KS0_tracking_MC16rd_run1);
    EventWeights::Register("KS0_tracking_run2", KS0_tracking_MC16rd_run2);
    EventWeights::Register("pionID_09_ALP", pionID_09_ALP);

    // signal MC
    TH1D before_BDT("before_BDT", ";bin index;", 8, 0.5, 8.5);
    TH1D after_BDT("after_BDT", ";bin index;", 8, 0.5, 8.5);
    Loader loader_signal("tau_lfv");
    loader_signal.Load((nominal_path + "/" + argv[6] + "/" + argv[2]).c_str(), ("alpha_mass" + std::format("{:g}", mass) + "_life" + std::format("{:g}", life) + "_A" + std::to_string(A) + "_B" + std::to_string(B) + "_").c_str(), argv[6]);
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
    loader_signal.Cut(("(" + std::to_string(parameters.M_peak - 20 * parameters.M_left_sigma) + "< M) && (M < " + std::to_string(parameters.M_peak + 20 * parameters.M_right_sigma) + ")").c_str());
    loader_signal.FillCustomizedTH1D(&before_BDT, { "M", "deltaE", BDT_names.at(0), BDT_names.at(1) }, mapping_before_BDT);
    loader_signal.FillCustomizedTH1D(&after_BDT, { "M", "deltaE", BDT_names.at(0), BDT_names.at(1) }, mapping_nominal);
    loader_signal.end();

    // include BDT = 1 in the last bin
    double BDT_bins[101];
    for (int i = 0; i < 100; i++) BDT_bins[i] = i / 100.0;
    BDT_bins[100] = std::nextafter(1.0, 2.0);

    // get control efficiencies
    std::vector<std::string> regions = { "one", "two" };
    std::vector<double> nominal_high = { parameters.BDT_cut_1, parameters.BDT_cut_2 };
    std::vector<double> nominal_low = { parameters.sideband_BDT_cut_1, parameters.sideband_BDT_cut_2 };
    FBDTResult results[2];
    for (int i = 0; i < 2; i++) {
        std::string weight_name = "weight_" + regions.at(i) + "_2D_CTRL_" + std::string(argv[10]) + "_" + std::string(argv[11]) + "_" + std::string(argv[12]) + "_" + std::string(argv[13]);
        std::string weight_path = std::string(argv[9]) + "/" + weight_name;
        FILE* fp = fopen((weight_path + "_info.txt").c_str(), "r");
        if (fp == nullptr) {
            printf("Cannot open %s_info.txt\n", weight_path.c_str());
            return 1;
        }
        double original_D, minimum_D;
        char first[1024], second[1024];
        if (fscanf(fp, "%lf %lf %1023s %1023s", &original_D, &minimum_D, first, second) != 4) {
            printf("Cannot read %s_info.txt\n", weight_path.c_str());
            fclose(fp);
            return 1;
        }
        fclose(fp);
        std::string var1(first), var2(second);

        EventWeight CTRL_weight(weight_path + ".csv", { {var1, var1 + "_min", var1 + "_max"}, {var2, var2 + "_min", var2 + "_max"} }, "weight", true);
        EventWeights::Register(weight_name, CTRL_weight);

        TH1D FBDT_MC("FBDT_MC", ";BDT output;arbitrary unit", 100, BDT_bins);
        TH1D FBDT_data("FBDT_data", ";BDT output;arbitrary unit", 100, BDT_bins);

        // control MC
        Loader loader_MC("tau_lfv");
        for (int j = 0; j < MC_list.size(); j++) loader_MC.Load((control_path + "/" + MC_list.at(j) + "/" + argv[4]).c_str(), "root", MC_list.at(j).c_str());
        loader_MC.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
        loader_MC.AddWeight("pionID_09_ALP", { {"index", "first_muon_index"},  {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
        loader_MC.AddWeight("pionID_09_ALP", { {"index", "second_muon_index"},  {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
        loader_MC.AddWeight("pionID_09_ALP", { {"index", "third_muon_index"},  {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
        loader_MC.AddWeight("KS0_tracking_run1", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
        loader_MC.AddWeight("KS0_tracking_run2", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
        loader_MC.AddWeight(weight_name.c_str(), { {var1, var1}, {var2, var2} });
        loader_MC.FillTH1D(&FBDT_MC, BDT_names.at(i));
        loader_MC.end();

        // control data
        Loader loader_data("tau_lfv");
        for (int j = 0; j < data_list.size(); j++) loader_data.Load((control_path + "/" + data_list.at(j) + "/" + argv[4]).c_str(), "root", data_list.at(j).c_str());
        loader_data.AddWeight(weight_name.c_str(), { {var1, var1}, {var2, var2} });
        loader_data.FillTH1D(&FBDT_data, BDT_names.at(i));
        loader_data.end();

        // high and low BDT efficiencies use the same pre-BDT denominator
        const int bin_A = 4 * i + 1;
        results[i] = CalculateFBDTEfficiency(&FBDT_MC, &FBDT_data, before_BDT.Integral(bin_A, bin_A + 1), after_BDT.Integral(bin_A, bin_A + 1), after_BDT.Integral(bin_A + 2, bin_A + 3));
    }

    // print result
    std::string output_filename = std::string(argv[5]) + "/FBDTSyst_" + std::format("{:g}", mass) + "_" + std::format("{:g}", life) + "_" + std::to_string(A) + "_" + std::to_string(B) + ".csv";
    FILE* fp = fopen(output_filename.c_str(), "w");
    if (fp == nullptr) {
        printf("Cannot open %s\n", output_filename.c_str());
        return 1;
    }
    fprintf(fp, "region,bdt_range,nominal_low_cut,nominal_high_cut,control_low_cut,control_high_cut,target_eff,control_eff_MC,control_eff_data,data_over_MC,stat_err,abs_syst,valid\n");
    for (int i = 0; i < 2; i++) {
        const FBDTResult& result = results[i];
        for (int j = 0; j < 2; j++) {
            const bool valid = std::isfinite(result.ratio[j]) && std::isfinite(result.stat_err[j]);
            fprintf(fp, "%d,%s,%.17g,%.17g,%.17g,%.17g", i + 1, (j == 0) ? "high" : "low", nominal_low.at(i), nominal_high.at(i), result.control_cut[1], result.control_cut[0]);
            fprintf(fp, ",%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%d\n", result.target_eff[j], result.eff_MC[j], result.eff_data[j], result.ratio[j], result.stat_err[j], std::fabs(result.ratio[j] - 1.0), valid);
        }
    }
    fclose(fp);

    return 0;
}
