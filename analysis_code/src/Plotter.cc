#include <stdio.h>
#include <string>
#include <vector>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"

int main(int argc, char* argv[]) {
    /*
    * argv[1]: variable name
    * argv[2]: signal num
    * argv[3], ...: signal path, signal label
    * argv[]: background num
    * argv[], ...: background path, background label
    */

    std::string variable_name(argv[1]);
    int signal_num = atoi(argv[2]);
    std::vector<std::string> signal_paths;
    std::vector<std::string> signal_labels;
    for (int i = 0; i < signal_num; i++) {
        signal_paths.push_back(argv[3 + 2 * i]);
        signal_labels.push_back(argv[4 + 2 * i]);
    }
    int background_num = atoi(argv[3 + 2 * signal_num]);
    std::vector<std::string> background_paths;
    std::vector<std::string> background_labels;
    for (int i = 0; i < background_num; i++) {
        background_paths.push_back(argv[4 + 2 * signal_num + 2 * i]);
        background_labels.push_back(argv[5 + 2 * signal_num + 2 * i]);
    }

    if (argc == (4 + 2 * signal_num + 2 * background_num)) {}
    else {
        printf("improper argument\n");
        exit(1);
    }

    ObtainWeight = MyScaleFunction;

    Loader loader("tau_lfv");

    for (int i = 0; i < signal_num; i++) loader.Load(signal_paths.at(i).c_str(), "root", signal_labels.at(i).c_str());
    for (int i = 0; i < signal_num; i++) loader.Load(background_paths.at(i).c_str(), "root", background_labels.at(i).c_str());

    // Create a new vector to hold the combined elements
    std::vector<std::string> all_label;
    all_label.reserve(signal_labels.size() + background_labels.size());
    all_label.insert(all_label.end(), signal_labels.begin(), signal_labels.end());
    all_label.insert(all_label.end(), background_labels.begin(), background_labels.end());

    loader.SetMC(all_label);
    loader.SetData({});
    loader.SetSignal(signal_labels);
    loader.SetBackground(background_labels);

    loader.DrawStack(argv[1], (";" + variable_name + ";events").c_str(), (variable_name + ".png").c_str());

    loader.end();

    return 0;
}
