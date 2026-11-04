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
    * argv[1]: dirname
    * argv[2]: filename
    * argv[3]: split_num
    * argv[4]: selected_index_train (seperated by colon)
    * argv[5]: output path train
    * argv[6]: selected_index_test (seperated by colon)
    * argv[7]: output path test
    */

    std::vector<std::string> train_index_str = split(argv[4], ':');
    std::vector<std::string> test_index_str = split(argv[6], ':');

    std::vector<int> train_index;
    try {
        for (int i = 0; i < train_index_str.size(); i++) {
            int value = std::stoi(train_index_str.at(i));
            train_index.push_back(value);
        }
    }
    catch (const std::exception& error) {
        printf("invalid index: %s\n", error.what());
        return 1;
    }

    std::vector<int> test_index;
    try {
        for (int i = 0; i < test_index_str.size(); i++) {
            int value = std::stoi(test_index_str.at(i));
            test_index.push_back(value);
        }
    }
    catch (const std::exception& error) {
        printf("invalid index: %s\n", error.what());
        return 1;
    }

    Loader loader_train("tau_lfv");
    loader_train.Load(argv[1], argv[2], "label");
    loader_train.RandomEventSelection(atoi(argv[3]), train_index);
    loader_train.PrintInformation("========== random event selection train ==========");
    loader_train.PrintSeparateRootFile(argv[5], "", ("split_" + std::string(argv[3])).c_str());
    loader_train.end();

    Loader loader_test("tau_lfv");
    loader_test.Load(argv[1], argv[2], "label");
    loader_test.RandomEventSelection(atoi(argv[3]), test_index);
    loader_test.PrintInformation("========== random event selection test ==========");
    loader_test.PrintSeparateRootFile(argv[7], "", ("split_" + std::string(argv[3])).c_str());
    loader_test.end();

    return 0;
}
