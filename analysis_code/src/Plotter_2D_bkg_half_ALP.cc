#include <stdio.h>
#include <string>
#include <vector>
#include <format>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "functions.h"

int main(int argc, char* argv[]) {
    /*
    * argv[1]: variable name 1
    * argv[2]: variable name 2
    * argv[3]: input path 1
    * argv[4]: input path 2
    * argv[5]: output path
    * argv[6]: output name
    * argv[7]: background list (separated by colon)
    * argv[8]: background legend list (separated by colon)
    * (argv[9]): min value 1
    * (argv[10]): max value 1
    * (argv[11]): min value 2
    * (argv[12]): max value 2
    * argv[9 ? 13]: mass
    * argv[10 ? 14]: lifetime
    * argv[11 ? 15]: A constant
    * argv[12 ? 16]: B constant
    */

    double mass = (argc == 13) ? std::stod(argv[9]) : std::stod(argv[13]);
    double life = (argc == 13) ? std::stod(argv[10]) : std::stod(argv[14]);
    int A = (argc == 13) ? std::stoi(argv[11]) : std::stoi(argv[15]);
    int B = (argc == 13) ? std::stoi(argv[12]) : std::stoi(argv[16]);

    std::string variable_name_1(argv[1]);
    std::string variable_name_2(argv[2]);

    std::vector<std::string> background_list = split(argv[7], ':');
    std::vector<std::string> background_legend_list = split(argv[8], ':');

    EventWeights::Register("MC_weight", MC_weight);
    EventWeights::Register("double_weight", double_weight);

    Loader loader("tau_lfv");

    for (int i = 0; i < background_list.size(); i++) loader.Load((argv[3] + std::string("/") + background_list.at(i) + std::string("/") + std::string(argv[4])).c_str(), "root", background_legend_list.at(i).c_str());
    loader.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader.AddWeight("double_weight");

    // Create a new vector to hold the combined elements
    std::vector<std::string> all_label;
    all_label.reserve(background_legend_list.size());
    all_label.insert(all_label.end(), background_legend_list.begin(), background_legend_list.end());

    loader.SetMC(all_label);
    loader.SetData({});
    loader.SetBackground(background_legend_list);

    if (argc == 13) loader.DrawTH2D(variable_name_1.c_str(), variable_name_2.c_str(), (";" + variable_name_1 + ";" + variable_name_2 + ";Number of event").c_str(), (argv[5] + std::string("/") + argv[6] + ".png").c_str(), "BOX");
    else if (argc == 17) loader.DrawTH2D(variable_name_1.c_str(), variable_name_2.c_str(), (";" + variable_name_1 + ";" + variable_name_2 + ";Number of event").c_str(), 50, std::stod(argv[9]), std::stod(argv[10]), 50, std::stod(argv[11]), std::stod(argv[12]), (argv[5] + std::string("/") + argv[6] + ".png").c_str(), "BOX");

    loader.end();

    return 0;
}
