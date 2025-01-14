#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "MyModule.h"

std::string ReadSelect(const char* select_path) {
    double nTrees;
    double depth;
    double shrinkage;
    double subsample;
    double binning;
    double train_AUC;
    double test_AUC;

    FILE* fp = fopen((std::string(select_path) + "/selected.txt").c_str(), "r");
    fscanf(fp, "%lf_%lf_%lf_%lf_%lf %lf %lf\n", &nTrees, &depth, &shrinkage, &subsample, &binning, &train_AUC, &test_AUC);
    fclose(fp);

    std::string classifier_path = std::string(select_path) + "/out/" + std::to_string(nTrees) + "_" + std::to_string(depth) + "_" + std::to_string(shrinkage) + "_" + std::to_string(subsample) + "_" + std::to_string(binning) + ".weightfile";
    return classifier_path;
}

int main(int argc, char* argv[]) {
    /*
    * argv[1]: variable num (ex. N in here)
    * argv[2]: variable name
    * argv[1 + N]: variable name
    * argv[2 + N]: input path
    * argv[3 + N]: input file name
    * argv[4 + N]: output path
    * argv[5 + N]: select.txt path
    */

    int variable_num = std::atoi(argv[1]);
    std::vector<std::string> intput_variables;
    for (int i = 0; i < variable_num; i++) {
        std::string variable_(argv[2 + i]);
        intput_variables.push_back(variable_);
    }

    std::string classifier_path = ReadSelect(argv[5 + variable_num]);

    ObtainWeight = MyScaleFunction_halfsplit;

    Loader loader("tau_lfv");

    loader.Load(argv[2 + variable_num], argv[3 + variable_num], "label");

    loader.FastBDTApplication(intput_variables, classifier_path.c_str(), "BDT_output");

    loader.PrintSeparateRootFile(argv[4 + variable_num], "", "");

    loader.end();

    return 0;
}
