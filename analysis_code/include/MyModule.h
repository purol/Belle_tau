#ifndef MYMODULE_H
#define MYMODULE_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

#include <module.h>

namespace Module {

    class ReadAutogluonCSVandWrite : public Module {
        /*
        * Read autogluon CSV output and write them into data
        */
    private:

        std::string csv_file;

        std::vector<std::string> variable_names;
        std::vector<std::string> VariableTypes;

    public:
        ReadAutogluonCSVandWrite(std::string csv_file_, std::vector<std::string>* variable_names_, std::vector<std::string>* VariableTypes_) : Module(), csv_file(csv_file_), variable_names(*variable_names_), VariableTypes(*VariableTypes_) {
            // check there is the same branch name or not
            if (std::find(variable_names_->begin(), variable_names_->end(), "BDT_output_1") != variable_names_->end()) {
                printf("[DefineNewVariable] there is already BDT_output_1 variable\n");
                exit(1);
            }

            if (std::find(variable_names_->begin(), variable_names_->end(), "BDT_output_2") != variable_names_->end()) {
                printf("[DefineNewVariable] there is already BDT_output_2 variable\n");
                exit(1);
            }

            // copy variable list first, because we use it inside the module
            variable_names = (*variable_names_);
            VariableTypes = (*VariableTypes_);

            // add variable
            variable_names_->push_back("BDT_output_1");
            VariableTypes_->push_back("Double_t");

            variable_names_->push_back("BDT_output_2");
            VariableTypes_->push_back("Double_t");
        }
        ~ReadAutogluonCSVandWrite() {}
        void Start() {

        }
        int Process(std::vector<Data>* data) override {

            std::ifstream file(csv_file.c_str());

            std::string line;

            // Skip header
            std::getline(file, line);

            // Variables to hold data per line
            int experiment, run, event, production, candidate, ncandidates;
            double BDT_output_1, BDT_output_2;

            for (std::vector<Data>::iterator iter = data->begin(); iter != data->end(); ) {
                if (!std::getline(file, line)) {
                    printf("[ReadAutogluonCSVandWrite] csv file is shorter than data\n");
                    exit(1);
                }

                std::stringstream ss(line);
                std::string value;

                std::getline(ss, value, ','); experiment = std::stoi(value);
                std::getline(ss, value, ','); run = std::stoi(value);
                std::getline(ss, value, ','); event = std::stoi(value);
                std::getline(ss, value, ','); production = std::stoi(value);
                std::getline(ss, value, ','); candidate = std::stoi(value);
                std::getline(ss, value, ','); ncandidates = std::stoi(value);
                std::getline(ss, value, ','); BDT_output_1 = std::stod(value);
                std::getline(ss, value, ','); BDT_output_2 = std::stod(value);

                iter->variable.push_back(BDT_output_1);
                iter->variable.push_back(BDT_output_2);

                ++iter;
            }
            file.close();
            return 1;
        }
        void End() override {}
    };

}

#endif 