#include <stdio.h>
#include <string>
#include <vector>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "MyModule.h"

int main(int argc, char* argv[]) {
    /*
    * argv[1]: variable num (N in this case)
    * argv[2]: condition variable name ...
    * argv[1 + N]: condition variable name
    * argv[2 + N]: criteria variable name ...
    * argv[1 + 2N]: criteria variable name
    * argv[2 + 2N]: condition order
    * argv[3 + 2N]: input path 1
    * argv[4 + 2N]: input path 2
    * argv[5 + 2N]: output path
    * argv[6 + 2N]: output name
    * (argv[7 + 2N]): min value
    * (argv[8 + 2N]): max value
    */

    int variable_num = std::atoi(argv[1]);
    std::vector<std::string> condition_variables;
    std::vector<std::string> criteria_variables;

    for (int i = 0; i < variable_num; i++) condition_variables.push_back(std::string(argv[2 + i]));
    for (int i = 0; i < variable_num; i++) criteria_variables.push_back(std::string(argv[2 + variable_num + i]));

    int condition_order = std::atoi(argv[2 + 2 * variable_num]);

    std::map<std::string, std::string> variable_pair;
    for (int i = 0; i < variable_num; i++) variable_pair.insert(std::pair<std::string, std::string>(condition_variables.at(i), criteria_variables.at(i)));

    std::vector<std::string> signal_list = { "SIGNAL" };
    std::vector<std::string> background_list = { "CHARM", "CHG", "DDBAR", "EE", "EEEE", 
        "EEKK", "EEMUMU", "EEPIPI", "EEPP", "EETAUTAU", "GG", 
        "K0K0BARISR", "KKISR", "MIX", "MUMU", "MUMUMUMU", 
        "MUMUTAUTAU", "PIPIISR", "SSBAR", "TAUPAIR", "TAUTAUTAUTAU", "UUBAR" };

    ObtainWeight = MyScaleFunction;

    Loader loader("tau_lfv");

    for (int i = 0; i < signal_list.size(); i++) loader.Load((argv[3 + 2 * variable_num] + std::string("/") + signal_list.at(i) + std::string("/") + std::string(argv[4 + 2 * variable_num])).c_str(), "root", signal_list.at(i).c_str());
    for (int i = 0; i < background_list.size(); i++) loader.Load((argv[3 + 2 * variable_num] + std::string("/") + background_list.at(i) + std::string("/") + std::string(argv[4 + 2 * variable_num])).c_str(), "root", background_list.at(i).c_str());

    // Create a new vector to hold the combined elements
    std::vector<std::string> all_label;
    all_label.reserve(signal_list.size() + background_list.size());
    all_label.insert(all_label.end(), signal_list.begin(), signal_list.end());
    all_label.insert(all_label.end(), background_list.begin(), background_list.end());

    loader.SetMC(all_label);
    loader.SetData({});
    loader.SetSignal(signal_list);
    loader.SetBackground(background_list);

    if (argc == (7 + 2 * variable_num)) {
        Module::Module* temp_module = new Module::ConditionalPairDrawStack(variable_pair, condition_order, (";" + std::string(argv[6 + 2 * variable_num]) + ";arbitrary unit").c_str(), (argv[5 + 2 * variable_num] + std::string("/") + argv[6 + 2 * variable_num] + ".png").c_str(), true, loader.Getvariable_names_address(), loader.VariableTypes_address());
        loader.InsertCustomizedModule(temp_module);
    }
    else if (argc == (9 + 2 * variable_num)) {
        Module::Module* temp_module = new Module::ConditionalPairDrawStack(variable_pair, condition_order, (";" + std::string(argv[6 + 2 * variable_num]) + ";arbitrary unit").c_str(), 50, std::stod(argv[7 + 2 * variable_num]), std::stod(argv[8 + 2 * variable_num]), (argv[5 + 2 * variable_num] + std::string("/") + argv[6 + 2 * variable_num] + ".png").c_str(), true, loader.Getvariable_names_address(), loader.VariableTypes_address());
        loader.InsertCustomizedModule(temp_module);
    }

    loader.end();

    return 0;
}
