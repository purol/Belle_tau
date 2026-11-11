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
    * (argv[10]): min value
    * (argv[11]): max value
    * (argv[12]): control sample (CTRL or CTRL_ALP, requires min and max values)
    */

    if (argc != 10 && argc != 12 && argc != 13) {
        printf("9, 11 or 12 arguments are required.\n");
        return 1;
    }

    std::string control_sample;
    if (argc == 13) {
        control_sample = argv[12];
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

    EventWeights::Register("MC_weight", MC_weight);
    EventWeights::Register("pionID_09_prompt", pionID_09_prompt);
    EventWeights::Register("pionID_09_ALP", pionID_09_ALP);
    EventWeights::Register("KS0_tracking_run1", KS0_tracking_MC16rd_run1);
    EventWeights::Register("KS0_tracking_run2", KS0_tracking_MC16rd_run2);

    Loader loader("tau_lfv");

    for (int i = 0; i < MC_list.size(); i++) loader.Load((argv[2] + std::string("/") + MC_list.at(i) + std::string("/") + std::string(argv[3])).c_str(), "root", MC_legend_list.at(i).c_str());
    for (int i = 0; i < data_list.size(); i++) loader.Load((argv[2] + std::string("/") + data_list.at(i) + std::string("/") + std::string(argv[3])).c_str(), "root", data_legend_list.at(i).c_str());
    loader.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });

    if (control_sample == "CTRL") {
        loader.AddWeight("pionID_09_prompt", { {"sample", "MySampleType"}, {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
        loader.AddWeight("pionID_09_prompt", { {"sample", "MySampleType"}, {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
        loader.AddWeight("pionID_09_prompt", { {"sample", "MySampleType"}, {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
    }
    else if (control_sample == "CTRL_ALP") {
        loader.AddWeight("pionID_09_ALP", { {"sample", "MySampleType"}, {"index", "first_muon_index"},  {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
        loader.AddWeight("pionID_09_ALP", { {"sample", "MySampleType"}, {"index", "second_muon_index"},  {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
        loader.AddWeight("pionID_09_ALP", { {"sample", "MySampleType"}, {"index", "third_muon_index"},  {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
        loader.AddWeight("KS0_tracking_run1", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
        loader.AddWeight("KS0_tracking_run2", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
    }

    loader.SetMC(MC_legend_list);
    loader.SetData(data_legend_list);

    if(argc == 10) loader.DrawStack(variable_name.c_str(), (";" + std::string(argv[5]) + ";arbitrary unit").c_str(), (argv[4] + std::string("/") + argv[5] + ".png").c_str(), false, false);
    else if (argc == 12 || argc == 13) loader.DrawStack(variable_name.c_str(), (";" + std::string(argv[5]) + ";arbitrary unit").c_str(), 50, std::stod(argv[10]), std::stod(argv[11]), (argv[4] + std::string("/") + argv[5] + ".png").c_str(), false, false);

    loader.end();

    return 0;
}
