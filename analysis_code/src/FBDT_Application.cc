#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "MyModule.h"

std::string ReadSelect(const char* select_path, const char* select_file_name) {
    double nTrees;
    double depth;
    double shrinkage;
    double subsample;
    double binning;
    double train_AUC;
    double test_AUC;

    FILE* fp = fopen((std::string(select_path) + "/" + std::string(select_file_name)).c_str(), "r");
    fscanf(fp, "%lf_%lf_%lf_%lf_%lf %lf %lf\n", &nTrees, &depth, &shrinkage, &subsample, &binning, &train_AUC, &test_AUC);
    fclose(fp);

    std::string classifier = std::to_string(nTrees) + "_" + std::to_string(depth) + "_" + std::to_string(shrinkage) + "_" + std::to_string(subsample) + "_" + std::to_string(binning) + ".weightfile";
    return classifier;
}

int main(int argc, char* argv[]) {
    /*
    * argv[1]: variable num for region one (ex. N in here)
    * argv[2]: variable name for region one
    * argv[1 + N]: variable name for region one
    * argv[2 + N]: variable num for region two (ex. M in here)
    * argv[3 + N]: variable name for region two
    * argv[2 + N + M]: variable name for region two
    * argv[3 + N + M]: input path
    * argv[4 + N + M]: input file name
    * argv[5 + N + M]: output path
    * argv[6 + N + M]: select.txt path 1
    * argv[7 + N + M]: select.txt path 2
    */

    int variable_num_one = std::atoi(argv[1]);
    std::vector<std::string> intput_variables_one;
    for (int i = 0; i < variable_num_one; i++) {
        std::string variable_(argv[2 + i]);
        intput_variables_one.push_back(variable_);
    }

    int variable_num_two = std::atoi(argv[2 + variable_num_one]);
    std::vector<std::string> intput_variables_two;
    for (int i = 0; i < variable_num_two; i++) {
        std::string variable_(argv[3 + variable_num_one + i]);
        intput_variables_two.push_back(variable_);
    }

    std::string classifier_one_path = std::string(argv[6 + variable_num_one + variable_num_two]) + "/out/" + ReadSelect(argv[6 + variable_num_one + variable_num_two], "selected.txt");
    std::string classifier_two_path = std::string(argv[7 + variable_num_one + variable_num_two]) + "/out/" + ReadSelect(argv[7 + variable_num_one + variable_num_two], "selected.txt");

    ObtainWeight = MyScaleFunction_halfsplit;

    Loader loader("tau_lfv");

    loader.Load(argv[3 + variable_num_one + variable_num_two], argv[4 + variable_num_one + variable_num_two], "label");

    loader.FastBDTApplication(intput_variables_one, classifier_one_path.c_str(), "BDT_output_1");
    loader.FastBDTApplication(intput_variables_two, classifier_two_path.c_str(), "BDT_output_2");

    loader.PrintSeparateRootFile(argv[5 + variable_num_one + variable_num_two], "", "");

    loader.end();

    return 0;
}
