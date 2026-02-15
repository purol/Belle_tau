#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <regex>
#include <filesystem>
#include <format>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "MyModule.h"

struct Params {
    double mass;
    double life;
    int A;
    int B;

    bool operator<(const Params& other) const {
        return std::tie(mass, life, A, B) <
            std::tie(other.mass, other.life, other.A, other.B);
    }
};

std::set<Params> GetParameters(const char* dirname) {
    std::regex pattern(R"(alpha_mass([0-9.+-eE]+)_life([0-9.+-eE]+)_A([0-9+-]+)_B([0-9+-]+))");

    std::set<Params> results;

    for (const auto& entry : std::filesystem::directory_iterator(dirname)) {
        if (!entry.is_regular_file()) continue;

        std::string filename = entry.path().filename().string();
        std::smatch match;

        if (std::regex_search(filename, match, pattern)) {
            Params p;
            p.mass = std::stod(match[1]);
            p.life = std::stod(match[2]);
            p.A = std::stoi(match[3]);
            p.B = std::stoi(match[4]);

            results.insert(p);
        }
    }

    return results;

}

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

    ObtainWeight = MyScaleFunction_halfsplit;

    Loader loader("tau_lfv");

    loader.Load(argv[3 + variable_num_one + variable_num_two], argv[4 + variable_num_one + variable_num_two], "label");

    std::set<Params> parameters = GetParameters(argv[3 + variable_num_one + variable_num_two]);

    for (const Params& p : parameters) {
        std::string classifier_one_path = ReadSelect(argv[6 + variable_num_one + variable_num_two], ("alpha_mass" + std::format("{:g}", p.mass) + "_life" + std::format("{:g}", p.life) + "_A" + std::to_string(p.A) + "_B" + std::to_string(p.B) + "_selected.txt").c_str());
        std::string classifier_two_path = ReadSelect(argv[7 + variable_num_one + variable_num_two], ("alpha_mass" + std::format("{:g}", p.mass) + "_life" + std::format("{:g}", p.life) + "_A" + std::to_string(p.A) + "_B" + std::to_string(p.B) + "_selected.txt").c_str());

        std::string classifier_one_path = std::string(argv[6 + variable_num_one + variable_num_two]) + "/out_" + std::format("{:g}", p.mass) + "_" + std::format("{:g}", p.life) + "_" + std::to_string(p.A) + "_" + std::to_string(p.B) + "/" + ReadSelect(argv[6 + variable_num_one + variable_num_two], "selected.txt");
        std::string classifier_two_path = std::string(argv[7 + variable_num_one + variable_num_two]) + "/out_" + std::format("{:g}", p.mass) + "_" + std::format("{:g}", p.life) + "_" + std::to_string(p.A) + "_" + std::to_string(p.B) + "/" + ReadSelect(argv[7 + variable_num_one + variable_num_two], "selected.txt");

        loader.FastBDTApplication(intput_variables_one, classifier_one_path.c_str(), ("BDT_output_1" + std::format("{:g}", p.mass) + "_" + std::format("{:g}", p.life) + "_" + std::to_string(p.A) + "_" + std::to_string(p.B)).c_str());
        loader.FastBDTApplication(intput_variables_two, classifier_two_path.c_str(), ("BDT_output_2" + std::format("{:g}", p.mass) + "_" + std::format("{:g}", p.life) + "_" + std::to_string(p.A) + "_" + std::to_string(p.B)).c_str());
    }

    loader.PrintSeparateRootFile(argv[5 + variable_num_one + variable_num_two], "", "");

    loader.end();

    return 0;
}
