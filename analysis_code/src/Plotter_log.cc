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
    */

    std::string variable_name(argv[1]);

    std::vector<std::string> signal_list = split(argv[6], ':');
    std::vector<std::string> background_list = split(argv[7], ':');
    std::vector<std::string> signal_legend_list = split(argv[8], ':');
    std::vector<std::string> background_legend_list = split(argv[9], ':');

    std::vector<std::string> data_list = split(argv[10], ':');
    std::vector<std::string> data_legend_list = split(argv[11], ':');

    EventWeights::Register("MC_weight", MC_weight);

    Loader loader("tau_lfv");

    for (int i = 0; i < signal_list.size(); i++) loader.Load((argv[2] + std::string("/") + signal_list.at(i) + std::string("/") + std::string(argv[3])).c_str(), "root", signal_legend_list.at(i).c_str());
    for (int i = 0; i < background_list.size(); i++) loader.Load((argv[2] + std::string("/") + background_list.at(i) + std::string("/") + std::string(argv[3])).c_str(), "root", background_legend_list.at(i).c_str());
    for (int i = 0; i < data_list.size(); i++) loader.Load((argv[2] + std::string("/") + data_list.at(i) + std::string("/") + std::string(argv[3])).c_str(), "root", data_legend_list.at(i).c_str());
    loader.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });

    // Create a new vector to hold the combined elements
    std::vector<std::string> all_label;
    all_label.reserve(signal_legend_list.size() + background_legend_list.size());
    all_label.insert(all_label.end(), signal_legend_list.begin(), signal_legend_list.end());
    all_label.insert(all_label.end(), background_legend_list.begin(), background_legend_list.end());

    loader.SetMC(all_label);
    loader.SetData({ data_legend_list });
    loader.SetSignal(signal_legend_list);
    loader.SetBackground(background_legend_list);

    if (argc == 12) loader.DrawStack(variable_name.c_str(), (";" + std::string(argv[5]) + ";arbitrary unit").c_str(), (argv[4] + std::string("/") + argv[5] + ".png").c_str(), true, true);
    else if (argc == 14) loader.DrawStack(variable_name.c_str(), (";" + std::string(argv[5]) + ";arbitrary unit").c_str(), 50, std::stod(argv[12]), std::stod(argv[13]), (argv[4] + std::string("/") + argv[5] + ".png").c_str(), true, true);

    loader.end();

    return 0;
}
