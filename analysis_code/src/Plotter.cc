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
    * argv[6]: signal list (separated by colon)
    * argv[7]: background list (separated by colon)
    * argv[8]: signal legend list (separated by colon)
    * argv[9]: background legend list (separated by colon)
    * argv[10]: data list (separated by colon, empty if there is no data)
    * argv[11]: data legend list (separated by colon, empty if there is no data)
    * (argv[12]): min value
    * (argv[13]): max value
    * (argv[14]): control sample (CTRL or CTRL_ALP)
    * (argv[15]): trigger weight path (CSV)
    * (argv[16]): trigger variables (separated by colon)
    * Without min and max values, the control sample and trigger arguments start at argv[12].
    */

    if (argc != 12 && argc != 14 && argc != 15 && argc != 17) {
        printf("11, 13, 14 or 16 arguments are required.\n");
        return 1;
    }

    std::string control_sample;
    if (argc == 15 || argc == 17) {
        control_sample = argv[argc - 3];
        if (control_sample != "CTRL" && control_sample != "CTRL_ALP") {
            printf("Control sample should be CTRL or CTRL_ALP.\n");
            return 1;
        }
    }

    std::string variable_name(argv[1]);

    std::vector<std::string> signal_list = split(argv[6], ':');
    std::vector<std::string> background_list = split(argv[7], ':');
    std::vector<std::string> signal_legend_list = split(argv[8], ':');
    std::vector<std::string> background_legend_list = split(argv[9], ':');

    std::vector<std::string> data_list = split(argv[10], ':');
    std::vector<std::string> data_legend_list = split(argv[11], ':');

    // get trigger correction
    std::vector<std::string> trigger_variables;
    if (argc == 15 || argc == 17) {
        trigger_variables = split(argv[argc - 1], ':');
        if (trigger_variables.size() != 2 || trigger_variables.at(0).empty() || trigger_variables.at(1).empty() || trigger_variables.at(0) == trigger_variables.at(1)) {
            printf("Two different trigger variables are required.\n");
            return 1;
        }

        if (control_sample == "CTRL") {
            EventWeight trigger_weight(
                argv[argc - 2],
                { {"sample", "sample_min", "sample_max"}, {trigger_variables.at(0), trigger_variables.at(0) + "_min", trigger_variables.at(0) + "_max"}, {trigger_variables.at(1), trigger_variables.at(1) + "_min", trigger_variables.at(1) + "_max"} },
                "weight",
                true
            );
            EventWeights::Register("trigger_weight", trigger_weight);
        }
        else if (control_sample == "CTRL_ALP") {
            trigger_variables.push_back("extraInfo__boALP_distance__bc");
            EventWeight trigger_weight(
                argv[argc - 2],
                { {"sample", "sample_min", "sample_max"}, {trigger_variables.at(0), trigger_variables.at(0) + "_min", trigger_variables.at(0) + "_max"}, {trigger_variables.at(1), trigger_variables.at(1) + "_min", trigger_variables.at(1) + "_max"}, {trigger_variables.at(2), trigger_variables.at(2) + "_min", trigger_variables.at(2) + "_max"} },
                "weight",
                true
            );
            EventWeights::Register("trigger_weight", trigger_weight);
        }
    }

    EventWeights::Register("MC_weight", MC_weight);

    Loader loader("tau_lfv");

    for (int i = 0; i < signal_list.size(); i++) loader.Load((argv[2] + std::string("/") + signal_list.at(i) + std::string("/") + std::string(argv[3])).c_str(), "root", signal_legend_list.at(i).c_str());
    for (int i = 0; i < background_list.size(); i++) loader.Load((argv[2] + std::string("/") + background_list.at(i) + std::string("/") + std::string(argv[3])).c_str(), "root", background_legend_list.at(i).c_str());
    for (int i = 0; i < data_list.size(); i++) loader.Load((argv[2] + std::string("/") + data_list.at(i) + std::string("/") + std::string(argv[3])).c_str(), "root", data_legend_list.at(i).c_str());
    loader.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });

    if (argc == 15 || argc == 17) {
        if (control_sample == "CTRL") {
            loader.AddWeight("trigger_weight", { {"sample", "MySampleType"}, {trigger_variables.at(0), trigger_variables.at(0)}, {trigger_variables.at(1), trigger_variables.at(1)} });
        }
        else if (control_sample == "CTRL_ALP") {
            loader.AddWeight("trigger_weight", { {"sample", "MySampleType"}, {trigger_variables.at(0), trigger_variables.at(0)}, {trigger_variables.at(1), trigger_variables.at(1)}, {trigger_variables.at(2), trigger_variables.at(2)} });
        }
    }

    // Create a new vector to hold the combined elements
    std::vector<std::string> all_label;
    all_label.reserve(signal_legend_list.size() + background_legend_list.size());
    all_label.insert(all_label.end(), signal_legend_list.begin(), signal_legend_list.end());
    all_label.insert(all_label.end(), background_legend_list.begin(), background_legend_list.end());

    loader.SetMC(all_label);
    loader.SetData(data_legend_list);
    loader.SetSignal(signal_legend_list);
    loader.SetBackground(background_legend_list);

    if (argc == 12 || argc == 15) loader.DrawStack(variable_name.c_str(), (";" + std::string(argv[5]) + ";arbitrary unit").c_str(), (argv[4] + std::string("/") + argv[5] + ".png").c_str(), true, false);
    else if (argc == 14 || argc == 17) loader.DrawStack(variable_name.c_str(), (";" + std::string(argv[5]) + ";arbitrary unit").c_str(), 50, std::stod(argv[12]), std::stod(argv[13]), (argv[4] + std::string("/") + argv[5] + ".png").c_str(), true, false);

    loader.end();

    return 0;
}
