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
    * argv[1]: variable name
    * argv[2]: input path 1
    * argv[3]: input path 2
    * argv[4]: output path
    * argv[5]: output name
    * argv[6]: signal list (separated by colon)
    * argv[7]: background list (separated by colon)
    * argv[8]: signal legend list (separated by colon)
    * argv[9]: background legend list (separated by colon)
    * (argv[10]): min value
    * (argv[11]): max value
    * argv[10 ? 12]: mass
    * argv[11 ? 13]: lifetime
    * argv[12 ? 14]: A constant
    * argv[13 ? 15]: B constant
    */

    double mass = (argc == 14) ? std::stod(argv[10]) : std::stod(argv[12]);
    double life = (argc == 14) ? std::stod(argv[11]) : std::stod(argv[13]);
    int A = (argc == 14) ? std::stoi(argv[12]) : std::stoi(argv[14]);
    int B = (argc == 14) ? std::stoi(argv[13]) : std::stoi(argv[15]);

    std::string variable_name(argv[1]);

    std::vector<std::string> signal_list = split(argv[6], ':');
    std::vector<std::string> background_list = split(argv[7], ':');
    std::vector<std::string> signal_legend_list = split(argv[8], ':');
    std::vector<std::string> background_legend_list = split(argv[9], ':');

    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution((std::string(argv[2]) + "/alpha_mass" + std::format("{:g}", mass) + "_life" + std::format("{:g}", life) + "_A" + std::to_string(A) + "_B" + std::to_string(B) + "_M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    EventWeights::Register("MC_weight", MC_weight);
    EventWeights::Register("double_weight", double_weight);

    Loader loader("tau_lfv");

    for (int i = 0; i < signal_list.size(); i++) loader.Load((argv[2] + std::string("/") + signal_list.at(i) + std::string("/") + std::string(argv[3])).c_str(), ("alpha_mass" + std::format("{:g}", mass) + "_life" + std::format("{:g}", life) + "_A" + std::to_string(A) + "_B" + std::to_string(B) + "_").c_str(), signal_legend_list.at(i).c_str());
    for (int i = 0; i < background_list.size(); i++) loader.Load((argv[2] + std::string("/") + background_list.at(i) + std::string("/") + std::string(argv[3])).c_str(), "root", background_legend_list.at(i).c_str());
    loader.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader.AddWeight("double_weight");

    // Create a new vector to hold the combined elements
    std::vector<std::string> all_label;
    all_label.reserve(signal_legend_list.size() + background_legend_list.size());
    all_label.insert(all_label.end(), signal_legend_list.begin(), signal_legend_list.end());
    all_label.insert(all_label.end(), background_legend_list.begin(), background_legend_list.end());

    loader.SetMC(all_label);
    loader.SetData({});
    loader.SetSignal(signal_legend_list);
    loader.SetBackground(background_legend_list);

    loader.Cut(("(" + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + "< deltaE) && (deltaE < " + std::to_string(deltaE_peak + 5 * deltaE_right_sigma) + ")").c_str());
    loader.PrintInformation("========== -5 delta < deltaE < 5 delta ==========");
    loader.Cut(("(" + std::to_string(M_peak - 5 * M_left_sigma) + "< M) && (M < " + std::to_string(M_peak + 5 * M_right_sigma) + ")").c_str());
    loader.PrintInformation("========== -5 delta < M < 5 delta ==========");

    if(argc == 14) loader.DrawStack(variable_name.c_str(), (";" + std::string(argv[5]) + ";arbitrary unit").c_str(), (argv[4] + std::string("/") + argv[5] + ".png").c_str(), true, false);
    else if (argc == 16) loader.DrawStack(variable_name.c_str(), (";" + std::string(argv[5]) + ";arbitrary unit").c_str(), 50, std::stod(argv[10]), std::stod(argv[11]), (argv[4] + std::string("/") + argv[5] + ".png").c_str(), true, false);

    loader.end();

    return 0;
}
