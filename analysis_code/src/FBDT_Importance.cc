#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <ostream>

#include "constants.h"
#include "Classifier.h"

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
    * argv[2 + N]: select.txt path 1
    * argv[3 + N]: select.txt path 2
    */

    int variable_num = std::atoi(argv[1]);
    std::vector<std::string> intput_variables;
    for (int i = 0; i < variable_num; i++) {
        std::string variable_(argv[2 + i]);
        intput_variables.push_back(variable_);
    }

    std::string classifier_one_path = ReadSelect(argv[2 + variable_num]);
    std::string classifier_two_path = ReadSelect(argv[3 + variable_num]);

    std::fstream in_stream_one(classifier_one_path.c_str(), std::ios_base::in);
    FastBDT::Classifier classifier_one = FastBDT::Classifier(in_stream_one);

    std::fstream in_stream_two(classifier_two_path.c_str(), std::ios_base::in);
    FastBDT::Classifier classifier_two = FastBDT::Classifier(in_stream_two);

    // define variables
    std::map<unsigned int, double> rank;

    // print importance for one
    rank = classifier_one.GetVariableRanking();
    printf("Variable importance for first classifier:\n");
    for (auto iter = rank.begin(); iter != rank.end(); iter++)
    {
        std::cout << "[" << iter->first << ", " << iter->second << "]" << " ";
    }
    printf("\n\n");
    printf("Variable importance for plot:\n");
    printf("[");
    for (auto iter = rank.begin(); iter != rank.end(); iter++)
    {
        int index = std::distance(rank.begin(), iter);
        std::cout << "(\'" << intput_variables.at(index) << "\'," << iter->second << ")";
        if (index == variable_num - 1) {}
        else {
            std::cout << "," << std::endl;
        }
    }
    printf("]");
    printf("\n\n");

    // print importance for two
    rank = classifier_two.GetVariableRanking();
    printf("Variable importance for second classifier:\n");
    for (auto iter = rank.begin(); iter != rank.end(); iter++)
    {
        std::cout << "[" << iter->first << ", " << iter->second << "]" << " ";
    }
    printf("\n\n");
    printf("Variable importance for plot:\n");
    printf("[");
    for (auto iter = rank.begin(); iter != rank.end(); iter++)
    {
        int index = std::distance(rank.begin(), iter);
        std::cout << "(\'" << intput_variables.at(index) << "\'," << iter->second << ")";
        if (index == variable_num - 1) {}
        else {
            std::cout << "," << std::endl;
        }
    }
    printf("]");
    printf("\n\n");


    return 0;
}
