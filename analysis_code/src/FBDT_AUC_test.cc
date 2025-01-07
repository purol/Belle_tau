#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "MyModule.h"

int main(int argc, char* argv[]) {
    /*
    * argv[1]: variable num (ex. N in here)
    * argv[2]: variable name 
    * argv[1 + N]: variable name
    * argv[2 + N]: input path
    * argv[3 + N]: output path used in Gridsearch
    * argv[4 + N]: NTrees
    * argv[5 + N]: Depth
    * argv[6 + N]: Shrinkage
    * argv[7 + N]: Subsample
    * argv[8 + N]: Binning
    */

    int variable_num = std::atoi(argv[1]);
    std::vector<std::map<std::string, std::string>> condition_variable_criteria_variable;
    std::vector<int> condition_orders;
    for (int i = 0; i < variable_num; i++) {
        std::map<std::string, std::string> variable_;
        if (std::string(argv[2 + i]) == "Primary_muon_p") {
            variable_.insert(std::make_pair(
                std::string("(daughter__bo0__cm__sppx__bc*daughter__bo0__cm__sppx__bc+daughter__bo0__cm__sppy__bc*daughter__bo0__cm__sppy__bc+daughter__bo0__cm__sppz__bc*daughter__bo0__cm__sppz__bc)^0.5"),
                std::string("(daughter__bo0__cm__sppx__bc*daughter__bo0__cm__sppx__bc+daughter__bo0__cm__sppy__bc*daughter__bo0__cm__sppy__bc+daughter__bo0__cm__sppz__bc*daughter__bo0__cm__sppz__bc)^0.5")
            ));
            variable_.insert(std::make_pair(
                std::string("(daughter__bo1__cm__sppx__bc*daughter__bo1__cm__sppx__bc+daughter__bo1__cm__sppy__bc*daughter__bo1__cm__sppy__bc+daughter__bo1__cm__sppz__bc*daughter__bo1__cm__sppz__bc)^0.5"),
                std::string("(daughter__bo1__cm__sppx__bc*daughter__bo1__cm__sppx__bc+daughter__bo1__cm__sppy__bc*daughter__bo1__cm__sppy__bc+daughter__bo1__cm__sppz__bc*daughter__bo1__cm__sppz__bc)^0.5")
            ));
            variable_.insert(std::make_pair(
                std::string("(daughter__bo2__cm__sppx__bc*daughter__bo2__cm__sppx__bc+daughter__bo2__cm__sppy__bc*daughter__bo2__cm__sppy__bc+daughter__bo2__cm__sppz__bc*daughter__bo2__cm__sppz__bc)^0.5"),
                std::string("(daughter__bo2__cm__sppx__bc*daughter__bo2__cm__sppx__bc+daughter__bo2__cm__sppy__bc*daughter__bo2__cm__sppy__bc+daughter__bo2__cm__sppz__bc*daughter__bo2__cm__sppz__bc)^0.5")
            ));
            condition_variable_criteria_variable.push_back(variable_);
            condition_orders.push_back(0);
        }
        else if (std::string(argv[2 + i]) == "secondary_muon_p") {
            variable_.insert(std::make_pair(
                std::string("(daughter__bo0__cm__sppx__bc*daughter__bo0__cm__sppx__bc+daughter__bo0__cm__sppy__bc*daughter__bo0__cm__sppy__bc+daughter__bo0__cm__sppz__bc*daughter__bo0__cm__sppz__bc)^0.5"),
                std::string("(daughter__bo0__cm__sppx__bc*daughter__bo0__cm__sppx__bc+daughter__bo0__cm__sppy__bc*daughter__bo0__cm__sppy__bc+daughter__bo0__cm__sppz__bc*daughter__bo0__cm__sppz__bc)^0.5")
            ));
            variable_.insert(std::make_pair(
                std::string("(daughter__bo1__cm__sppx__bc*daughter__bo1__cm__sppx__bc+daughter__bo1__cm__sppy__bc*daughter__bo1__cm__sppy__bc+daughter__bo1__cm__sppz__bc*daughter__bo1__cm__sppz__bc)^0.5"),
                std::string("(daughter__bo1__cm__sppx__bc*daughter__bo1__cm__sppx__bc+daughter__bo1__cm__sppy__bc*daughter__bo1__cm__sppy__bc+daughter__bo1__cm__sppz__bc*daughter__bo1__cm__sppz__bc)^0.5")
            ));
            variable_.insert(std::make_pair(
                std::string("(daughter__bo2__cm__sppx__bc*daughter__bo2__cm__sppx__bc+daughter__bo2__cm__sppy__bc*daughter__bo2__cm__sppy__bc+daughter__bo2__cm__sppz__bc*daughter__bo2__cm__sppz__bc)^0.5"),
                std::string("(daughter__bo2__cm__sppx__bc*daughter__bo2__cm__sppx__bc+daughter__bo2__cm__sppy__bc*daughter__bo2__cm__sppy__bc+daughter__bo2__cm__sppz__bc*daughter__bo2__cm__sppz__bc)^0.5")
            ));
            condition_variable_criteria_variable.push_back(variable_);
            condition_orders.push_back(1);
        }
        else if (std::string(argv[2 + i]) == "third_muon_p") {
            variable_.insert(std::make_pair(
                std::string("(daughter__bo0__cm__sppx__bc*daughter__bo0__cm__sppx__bc+daughter__bo0__cm__sppy__bc*daughter__bo0__cm__sppy__bc+daughter__bo0__cm__sppz__bc*daughter__bo0__cm__sppz__bc)^0.5"),
                std::string("(daughter__bo0__cm__sppx__bc*daughter__bo0__cm__sppx__bc+daughter__bo0__cm__sppy__bc*daughter__bo0__cm__sppy__bc+daughter__bo0__cm__sppz__bc*daughter__bo0__cm__sppz__bc)^0.5")
            ));
            variable_.insert(std::make_pair(
                std::string("(daughter__bo1__cm__sppx__bc*daughter__bo1__cm__sppx__bc+daughter__bo1__cm__sppy__bc*daughter__bo1__cm__sppy__bc+daughter__bo1__cm__sppz__bc*daughter__bo1__cm__sppz__bc)^0.5"),
                std::string("(daughter__bo1__cm__sppx__bc*daughter__bo1__cm__sppx__bc+daughter__bo1__cm__sppy__bc*daughter__bo1__cm__sppy__bc+daughter__bo1__cm__sppz__bc*daughter__bo1__cm__sppz__bc)^0.5")
            ));
            variable_.insert(std::make_pair(
                std::string("(daughter__bo2__cm__sppx__bc*daughter__bo2__cm__sppx__bc+daughter__bo2__cm__sppy__bc*daughter__bo2__cm__sppy__bc+daughter__bo2__cm__sppz__bc*daughter__bo2__cm__sppz__bc)^0.5"),
                std::string("(daughter__bo2__cm__sppx__bc*daughter__bo2__cm__sppx__bc+daughter__bo2__cm__sppy__bc*daughter__bo2__cm__sppy__bc+daughter__bo2__cm__sppz__bc*daughter__bo2__cm__sppz__bc)^0.5")
            ));
            condition_variable_criteria_variable.push_back(variable_);
            condition_orders.push_back(2);
        }
        else {
            variable_.insert(std::make_pair(std::string(argv[2 + i]), std::string(argv[2 + i])));
            condition_variable_criteria_variable.push_back(variable_);
            condition_orders.push_back(0);
        }
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

    ObtainWeight = MyScaleFunction_halfsplit;

    Loader loader("tau_lfv");

    for (int i = 0; i < signal_list.size(); i++) loader.Load((argv[2 + variable_num] + std::string("/") + signal_list.at(i) + std::string("/final_output_test/")).c_str(), "root", signal_list.at(i).c_str());
    for (int i = 0; i < background_list.size(); i++) loader.Load((argv[2 + variable_num] + std::string("/") + background_list.at(i) + std::string("/final_output_test/")).c_str(), "root", background_list.at(i).c_str());

    // Create a new vector to hold the combined elements
    std::vector<std::string> all_label;
    all_label.reserve(signal_list.size() + background_list.size());
    all_label.insert(all_label.end(), signal_list.begin(), signal_list.end());
    all_label.insert(all_label.end(), background_list.begin(), background_list.end());

    loader.SetMC(all_label);
    loader.SetData({});
    loader.SetSignal(signal_list);
    loader.SetBackground(background_list);

    std::string weightfile_path = (std::string(argv[3 + variable_num]) + "/" + std::to_string(hyperparameters["NTrees"]) + "_" + std::to_string(hyperparameters["Depth"]) + "_" + std::to_string(hyperparameters["Shrinkage"]) + "_" + std::to_string(hyperparameters["Subsample"]) + "_" + std::to_string(hyperparameters["Binning"]) + ".weightfile");
    Module::Module* temp_module = new Module::ConditionalPairFastBDTApplication(condition_variable_criteria_variable, condition_orders, "", "", weightfile_path.c_str(), "FBDT_output", loader.Getvariable_names_address(), loader.VariableTypes_address());
    loader.InsertCustomizedModule(temp_module);

    std::string AUC_path = (std::string(argv[3 + variable_num]) + "/" + std::to_string(hyperparameters["NTrees"]) + "_" + std::to_string(hyperparameters["Depth"]) + "_" + std::to_string(hyperparameters["Shrinkage"]) + "_" + std::to_string(hyperparameters["Subsample"]) + "_" + std::to_string(hyperparameters["Binning"]) + ".auc");
    loader.CalculateAUC("FBDT_output", 0.0, 1.0, AUC_path.c_str(), "w");

    loader.end();

    return 0;
}
