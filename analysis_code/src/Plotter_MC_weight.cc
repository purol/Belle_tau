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
    * argv[8]: weight path
    * argv[9]: weight name (without extension)
    * (argv[10]): min value
    * (argv[11]): max value
    */

    if (argc != 10 && argc != 12) {
        printf("9 or 11 arguments are required.\n");
        return 1;
    }

    std::string variable_name(argv[1]);

    std::vector<std::string> MC_list = split(argv[6], ':');
    std::vector<std::string> MC_legend_list = split(argv[7], ':');

    FILE* fp = fopen((std::string(argv[8]) + "/" + std::string(argv[9]) + "_info.txt").c_str(), "r");
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

    EventWeight CTRL_weight(
        std::string(argv[8]) + "/" + std::string(argv[9]) + ".csv",
        { {compare_variables.at(0), compare_variables.at(0) + "_min", compare_variables.at(0) + "_max"}, {compare_variables.at(1), compare_variables.at(1) + "_min", compare_variables.at(1) + "_max"} },
        "weight",
        true
    );
    EventWeights::Register("CTRL_weight", CTRL_weight);
    EventWeights::Register("MC_weight", MC_weight);

    Loader loader("tau_lfv");

    for (int i = 0; i < MC_list.size(); i++) loader.Load((argv[2] + std::string("/") + MC_list.at(i) + std::string("/") + std::string(argv[3])).c_str(), "root", MC_legend_list.at(i).c_str());
    loader.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader.AddWeight("CTRL_weight", { {compare_variables.at(0), compare_variables.at(0)}, {compare_variables.at(1), compare_variables.at(1)} });

    loader.SetMC(MC_legend_list);
    loader.SetData({});

    if(argc == 10) loader.DrawStack(variable_name.c_str(), (";" + std::string(argv[5]) + ";arbitrary unit").c_str(), (argv[4] + std::string("/") + argv[5] + ".png").c_str(), false, false);
    else if (argc == 12) loader.DrawStack(variable_name.c_str(), (";" + std::string(argv[5]) + ";arbitrary unit").c_str(), 50, std::stod(argv[10]), std::stod(argv[11]), (argv[4] + std::string("/") + argv[5] + ".png").c_str(), false, false);

    loader.end();

    return 0;
}
