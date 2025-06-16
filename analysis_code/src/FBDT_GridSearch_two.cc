#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include "TFile.h"
#include "RooRandom.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "MyModule.h"
#include "functions.h"

int main(int argc, char* argv[]) {
    /*
    * argv[1]: variable num (ex. N in here)
    * argv[2]: variable name 
    * argv[1 + N]: variable name
    * argv[2 + N]: input path
    * argv[3 + N]: output path
    * argv[4 + N]: NTrees
    * argv[5 + N]: Depth
    * argv[6 + N]: Shrinkage
    * argv[7 + N]: Subsample
    * argv[8 + N]: Binning
    */

    RooRandom::randomGenerator()->SetSeed(42);
    srand(42);

    int variable_num = std::atoi(argv[1]);
    std::vector<std::string> intput_variables;
    for (int i = 0; i < variable_num; i++) {
        std::string variable_(argv[2 + i]);
        intput_variables.push_back(variable_);
    }
    std::map<std::string, double> hyperparameters;
    hyperparameters.insert(std::pair<std::string, double>("NTrees", std::stod(argv[4 + variable_num])));
    hyperparameters.insert(std::pair<std::string, double>("Depth", std::stod(argv[5 + variable_num])));
    hyperparameters.insert(std::pair<std::string, double>("Shrinkage", std::stod(argv[6 + variable_num])));
    hyperparameters.insert(std::pair<std::string, double>("Subsample", std::stod(argv[7 + variable_num])));
    hyperparameters.insert(std::pair<std::string, double>("Binning", std::stod(argv[8 + variable_num])));


    std::vector<std::string> signal_list = { "SIGNAL" };
    std::vector<std::string> background_list = { "CHARM", "CHG", "DDBAR", "EE", "EEEE", 
        "EEKK", "EEMUMU", "EEPIPI", "EEPP", "EETAUTAU", "GG", 
        "K0K0BARISR", "KKISR", "MIX", "MUMU", "MUMUMUMU", 
        "MUMUTAUTAU", "PIPIISR", "SSBAR", "TAUPAIR", "TAUTAUTAUTAU", "UUBAR" };

    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution((std::string(argv[2 + variable_num]) + "/M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    ObtainWeight = MyScaleFunction_halfsplit;

    Loader loader("tau_lfv");

    for (int i = 0; i < signal_list.size(); i++) loader.Load((argv[2 + variable_num] + std::string("/") + signal_list.at(i) + std::string("/final_output_train/")).c_str(), "root", signal_list.at(i).c_str());
    for (int i = 0; i < background_list.size(); i++) loader.Load((argv[2 + variable_num] + std::string("/") + background_list.at(i) + std::string("/final_output_train/")).c_str(), "root", background_list.at(i).c_str());

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
    loader.Cut(("(deltaE < " + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + ")").c_str());
    loader.PrintInformation("========== deltaE < -5 delta ==========");
    loader.Cut(("(" + std::to_string(M_peak - 3 * M_left_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak + 3 * M_right_sigma) + ")").c_str());
    loader.PrintInformation("========== -3 delta < M < 3 delta ==========");

    loader.FastBDTTrain(intput_variables, "", "", hyperparameters, argv[3 + variable_num]);

    loader.end();

    return 0;
}
