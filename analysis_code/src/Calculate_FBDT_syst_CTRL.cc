#include <stdio.h>
#include <string>
#include <vector>

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
    * argv[1]: input1 path1 (nominal signal MC)
    * argv[2]: input1 path2 (nominal signal MC)
    * argv[3]: input2 path1 (control sample)
    * argv[4]: input2 path2 (control sample)
    * argv[5]: output path
    * argv[6]: nominal signal sample
    * argv[7]: control MC list (separated by colon)
    * argv[8]: control data list (separated by colon)
    * argv[9]: weight path
    */

    if (argc != 10) {
        printf("9 arguments are required.\n");
        return 1;
    }

    std::string nominal_path = argv[1];
    std::string control_path = argv[3];
    std::vector<std::string> MC_list = split(argv[7], ':');
    std::vector<std::string> data_list = split(argv[8], ':');

    // get nominal BDT cuts and signal region
    ReadFOM((nominal_path + "/GridSearch_one/FOM.log").c_str(), &parameters.BDT_cut_1);
    ReadFOM((nominal_path + "/GridSearch_two/FOM.log").c_str(), &parameters.BDT_cut_2);
    ReadABCDBoundary((nominal_path + "/ABCD_boundary.txt").c_str(), { parameters.BDT_cut_1, parameters.BDT_cut_2 }, &parameters.sideband_BDT_cut_1, &parameters.sideband_BDT_cut_2);
    double theta;
    ReadResolution((nominal_path + "/M_deltaE_result.txt").c_str(), &parameters.deltaE_peak, &parameters.deltaE_left_sigma, &parameters.deltaE_right_sigma,
        &parameters.M_peak, &parameters.M_left_sigma, &parameters.M_right_sigma, &theta);
    parameters.M_size = sizeM;
    std::string cut_region = GetBCSCut(parameters.deltaE_peak, parameters.deltaE_left_sigma, parameters.deltaE_right_sigma, parameters.M_peak, parameters.M_left_sigma, parameters.M_right_sigma);
    std::vector<std::string> BDT_names = { "BDT_output_1", "BDT_output_2" };

    EventWeights::Register("MC_weight", MC_weight);
    EventWeights::Register("muonID_01_prompt", muonID_01_prompt);
    EventWeights::Register("muonID_05_prompt", muonID_05_prompt);

    // signal MC
    TH1D before_BDT("before_BDT", ";bin index;", 8, 0.5, 8.5);
    TH1D after_BDT("after_BDT", ";bin index;", 8, 0.5, 8.5);
    Loader loader_signal("tau_lfv");
    loader_signal.Load((nominal_path + "/" + argv[6] + "/" + argv[2]).c_str(), "root", argv[6]);
    loader_signal.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_signal.AddWeight("muonID_05_prompt", { {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
    loader_signal.AddWeight("muonID_05_prompt", { {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
    loader_signal.AddWeight("muonID_01_prompt", { {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
    loader_signal.Cut(cut_region.c_str());
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
        std::string weight_name = "weight_" + regions.at(i) + "_2D_CTRL";
        std::string weight_path = std::string(argv[9]) + "/" + weight_name;
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
            for (int j = 0; j < MC_list.size(); j++) loader_MC.Load((control_path + "/" + MC_list.at(j) + "/" + argv[4]).c_str(), "root", MC_list.at(j).c_str());
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
            for (int j = 0; j < data_list.size(); j++) loader_data.Load((control_path + "/" + data_list.at(j) + "/" + argv[4]).c_str(), "root", data_list.at(j).c_str());
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
    if (!WriteFBDTResult(std::string(argv[5]) + "/FBDTSyst.csv", results)) return 1;
    return 0;
}
