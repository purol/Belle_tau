#include <stdio.h>
#include <string>
#include <vector>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "functions.h"

int main(int argc, char* argv[]) {
    /*
    * argv[1]: variable name
    * argv[2]: input path 1
    * argv[3]: input path 2
    * argv[4]: output path
    * argv[5]: output name
    * argv[6]: MC list (separated by colon)
    * argv[7]: MC legend list (separated by colon)
    * argv[8]: data list (separated by colon, empty if there is no data)
    * argv[9]: data legend list (separated by colon, empty if there is no data)
    * argv[10]: weight path
    * argv[11]: weight name (without extension)
    * (argv[12]): min value
    * (argv[13]): max value
    * (argv[14]): control sample (CTRL or CTRL_ALP, requires min and max values)
    * (argv[15]): trigger weight path (CSV, required for control samples)
    * (argv[16]): trigger variables (separated by colon, required for control samples)
    */

    if (argc != 12 && argc != 14 && argc != 17) {
        printf("11, 13 or 16 arguments are required.\n");
        return 1;
    }

    std::string control_sample;
    if (argc > 14) {
        control_sample = argv[14];
        if (control_sample != "CTRL" && control_sample != "CTRL_ALP") {
            printf("Control sample should be CTRL or CTRL_ALP.\n");
            return 1;
        }
    }

    std::string variable_name(argv[1]);

    std::vector<std::string> MC_list = split(argv[6], ':');
    std::vector<std::string> MC_legend_list = split(argv[7], ':');

    std::vector<std::string> data_list = split(argv[8], ':');
    std::vector<std::string> data_legend_list = split(argv[9], ':');

    FILE* fp = fopen((std::string(argv[10]) + "/" + std::string(argv[11]) + "_info.txt").c_str(), "r");
    if (fp == NULL) {
        printf("Cannot open weight information file.\n");
        return 1;
    }
    std::vector<std::string> compare_variables;
    double temp_double;
    char temp_char_one[100];
    char temp_char_two[100];
    if (fscanf(fp, "%lf", &temp_double) != 1 || fscanf(fp, "%lf", &temp_double) != 1 || fscanf(fp, "%99s", temp_char_one) != 1 || fscanf(fp, "%99s", temp_char_two) != 1) {
        printf("Cannot read weight information file.\n");
        fclose(fp);
        return 1;
    }
    compare_variables.push_back(temp_char_one);
    compare_variables.push_back(temp_char_two);
    fclose(fp);

    // get trigger correction
    std::vector<std::string> trigger_variables;
    if (!control_sample.empty()) {
        trigger_variables = split(argv[16], ':');
        if (trigger_variables.size() != 2 || trigger_variables.at(0).empty() || trigger_variables.at(1).empty() || trigger_variables.at(0) == trigger_variables.at(1)) {
            printf("Two different trigger variables are required.\n");
            return 1;
        }

        if (control_sample == "CTRL") {
            EventWeight trigger_weight(
                argv[15],
                { {"sample", "sample_min", "sample_max"}, {trigger_variables.at(0), trigger_variables.at(0) + "_min", trigger_variables.at(0) + "_max"}, {trigger_variables.at(1), trigger_variables.at(1) + "_min", trigger_variables.at(1) + "_max"} },
                "weight",
                true
            );
            EventWeights::Register("trigger_weight", trigger_weight);
        }
        else if (control_sample == "CTRL_ALP") {
            trigger_variables.push_back("extraInfo__boALP_distance__bc");
            EventWeight trigger_weight(
                argv[15],
                { {"sample", "sample_min", "sample_max"}, {trigger_variables.at(0), trigger_variables.at(0) + "_min", trigger_variables.at(0) + "_max"}, {trigger_variables.at(1), trigger_variables.at(1) + "_min", trigger_variables.at(1) + "_max"}, {trigger_variables.at(2), trigger_variables.at(2) + "_min", trigger_variables.at(2) + "_max"} },
                "weight",
                true
            );
            EventWeights::Register("trigger_weight", trigger_weight);
        }
    }

    EventWeight CTRL_weight(
        std::string(argv[10]) + "/" + std::string(argv[11]) + ".csv",
        { {compare_variables.at(0), compare_variables.at(0) + "_min", compare_variables.at(0) + "_max"}, {compare_variables.at(1), compare_variables.at(1) + "_min", compare_variables.at(1) + "_max"} },
        "weight",
        true
    );
    EventWeights::Register("CTRL_weight", CTRL_weight);
    EventWeights::Register("MC_weight", MC_weight);
    EventWeights::Register("pionID_09_prompt", pionID_09_prompt);
    EventWeights::Register("pionID_09_ALP", pionID_09_ALP);
    EventWeights::Register("KS0_tracking_run1", KS0_tracking_MC16rd_run1);
    EventWeights::Register("KS0_tracking_run2", KS0_tracking_MC16rd_run2);

    Loader loader("tau_lfv");

    for (int i = 0; i < MC_list.size(); i++) loader.Load((argv[2] + std::string("/") + MC_list.at(i) + std::string("/") + std::string(argv[3])).c_str(), "root", MC_legend_list.at(i).c_str());
    for (int i = 0; i < data_list.size(); i++) loader.Load((argv[2] + std::string("/") + data_list.at(i) + std::string("/") + std::string(argv[3])).c_str(), "root", data_legend_list.at(i).c_str());
    loader.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader.AddWeight("CTRL_weight", { {compare_variables.at(0), compare_variables.at(0)}, {compare_variables.at(1), compare_variables.at(1)} });

    if (control_sample == "CTRL") {
        loader.AddWeight("trigger_weight", { {"sample", "MySampleType"}, {trigger_variables.at(0), trigger_variables.at(0)}, {trigger_variables.at(1), trigger_variables.at(1)} });
        loader.AddWeight("pionID_09_prompt", { {"sample", "MySampleType"}, {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
        loader.AddWeight("pionID_09_prompt", { {"sample", "MySampleType"}, {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
        loader.AddWeight("pionID_09_prompt", { {"sample", "MySampleType"}, {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
    }
    else if (control_sample == "CTRL_ALP") {
        loader.AddWeight("trigger_weight", { {"sample", "MySampleType"}, {trigger_variables.at(0), trigger_variables.at(0)}, {trigger_variables.at(1), trigger_variables.at(1)}, {trigger_variables.at(2), trigger_variables.at(2)} });
        loader.AddWeight("pionID_09_ALP", { {"sample", "MySampleType"}, {"index", "first_muon_index"},  {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
        loader.AddWeight("pionID_09_ALP", { {"sample", "MySampleType"}, {"index", "second_muon_index"},  {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
        loader.AddWeight("pionID_09_ALP", { {"sample", "MySampleType"}, {"index", "third_muon_index"},  {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
        loader.AddWeight("KS0_tracking_run1", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
        loader.AddWeight("KS0_tracking_run2", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
    }

    loader.SetMC(MC_legend_list);
    loader.SetData(data_legend_list);

    if (argc == 12) loader.DrawStack(variable_name.c_str(), (";" + std::string(argv[5]) + ";arbitrary unit").c_str(), (argv[4] + std::string("/") + argv[5] + ".png").c_str(), false, false, "579 fb^{-1}");
    else loader.DrawStack(variable_name.c_str(), (";" + std::string(argv[5]) + ";arbitrary unit").c_str(), 50, std::stod(argv[12]), std::stod(argv[13]), (argv[4] + std::string("/") + argv[5] + ".png").c_str(), false, false, "579 fb^{-1}");

    loader.end();

    return 0;
}