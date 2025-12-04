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
    * (argv[6]): min value
    * (argv[7]): max value
    */

    std::string variable_name(argv[1]);

    std::vector<std::string> signal_list = { "SIGNAL" };
    std::vector<std::string> background_list = { "BBs", "BsBs", "CHARM", "CHG", "DDBAR",
        "EE", "EEEE", "EEKK", "EEMUMU", "EEPIPI",
        "EEPP", "EETAUTAU", "GG", "K0K0BARISR", "KKISR",
        "MIX", "MUMU", "MUMUMUMU", "MUMUTAUTAU", "PIPIPI0ISR",
        "PIPIISR", "SSBAR", "TAUPAIR", "TAUTAUTAUTAU", "UUBAR" };

    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution((std::string(argv[2]) + "/M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    ObtainWeight = MyScaleFunction_halfsplit;

    Loader loader("tau_lfv");

    for (int i = 0; i < signal_list.size(); i++) loader.Load((argv[2] + std::string("/") + signal_list.at(i) + std::string("/") + std::string(argv[3])).c_str(), "root", signal_list.at(i).c_str());
    for (int i = 0; i < background_list.size(); i++) loader.Load((argv[2] + std::string("/") + background_list.at(i) + std::string("/") + std::string(argv[3])).c_str(), "root", background_list.at(i).c_str());

    // Create a new vector to hold the combined elements
    std::vector<std::string> all_label;
    all_label.reserve(signal_list.size() + background_list.size());
    all_label.insert(all_label.end(), signal_list.begin(), signal_list.end());
    all_label.insert(all_label.end(), background_list.begin(), background_list.end());

    loader.SetMC(all_label);
    loader.SetData({});
    loader.SetSignal(signal_list);
    loader.SetBackground(background_list);

    // cut on deltaE
    loader.Cut(("(" + std::to_string(deltaE_peak - 15 * deltaE_left_sigma) + "< deltaE) && (deltaE < " + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + ")").c_str());
    loader.PrintInformation("========== -15 delta < deltaE < -5 delta ==========");
    loader.Cut(("(" + std::to_string(M_peak - 5 * M_left_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak + 5 * M_right_sigma) + ")").c_str());
    loader.PrintInformation("========== -5 delta < M < 5 delta ==========");

    if(argc == 6) loader.DrawStack(variable_name.c_str(), (";" + std::string(argv[5]) + ";arbitrary unit").c_str(), (argv[4] + std::string("/") + argv[5] + ".png").c_str(), true, false);
    else if (argc == 8) loader.DrawStack(variable_name.c_str(), (";" + std::string(argv[5]) + ";arbitrary unit").c_str(), 50, std::stod(argv[6]), std::stod(argv[7]), (argv[4] + std::string("/") + argv[5] + ".png").c_str(), true, false);

    loader.end();

    return 0;
}
