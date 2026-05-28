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
    * argv[1]: variable name 1
    * argv[2]: variable name 2
    * argv[3]: input path 1
    * argv[4]: input path 2
    * argv[5]: output path
    * argv[6]: output name
    * argv[7]: MC list (separated by colon)
    * (argv[8]): min value 1
    * (argv[9]): max value 1
    * (argv[10]): min value 2
    * (argv[11]): max value 2
    */

    std::string variable_name_1(argv[1]);
    std::string variable_name_2(argv[2]);

    std::vector<std::string> MC_list = split(argv[7], ':');

    ObtainWeight = MyScaleFunction;

    Loader loader("tau_lfv");

    for (int i = 0; i < MC_list.size(); i++) loader.Load((argv[3] + std::string("/") + MC_list.at(i) + std::string("/") + std::string(argv[4])).c_str(), "root", MC_list.at(i).c_str());

    // Create a new vector to hold the combined elements
    std::vector<std::string> all_label;
    all_label.reserve(MC_list.size());
    all_label.insert(all_label.end(), MC_list.begin(), MC_list.end());

    loader.SetMC(all_label);
    loader.SetData({});

    if (argc == 8) loader.DrawTH2D(variable_name_1.c_str(), variable_name_2.c_str(), (";" + variable_name_1 + ";" + variable_name_2 + ";Number of event").c_str(), (argv[5] + std::string("/") + argv[6] + ".png").c_str(), "BOX");
    else if (argc == 12) loader.DrawTH2D(variable_name_1.c_str(), variable_name_2.c_str(), (";" + variable_name_1 + ";" + variable_name_2 + ";Number of event").c_str(), 50, std::stod(argv[8]), std::stod(argv[9]), 50, std::stod(argv[10]), std::stod(argv[11]), (argv[5] + std::string("/") + argv[6] + ".png").c_str(), "BOX");

    loader.end();

    return 0;
}
