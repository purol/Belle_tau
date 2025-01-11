#ifndef MYMODULE_H
#define MYMODULE_H

#include <string>
#include <vector>
#include <map>

#include <module.h>

#include "RooWorkspace.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooArgSet.h"

namespace Module {

    class FillDataSet : public Module {
        /*
        * This module is used to fill RooDataSet
        */
    private:

        RooDataSet* dataset;
        std::vector<RooRealVar*> realvars;

        std::vector<std::string> equations;
        std::vector<std::string> replaced_exprs;

        std::vector<std::string> variable_names;
        std::vector<std::string> VariableTypes;

    public:
        FillDataSet(RooDataSet* dataset_, std::vector<RooRealVar*> realvars_, std::vector<std::string> equations_, std::vector<std::string>* variable_names_, std::vector<std::string>* VariableTypes_) : Module(), dataset(dataset_), realvars(realvars_), equations(equations_), variable_names(*variable_names_), VariableTypes(*VariableTypes_) {}
        ~FillDataSet() {}
        void Start() {
            for (int i = 0; i < equations.size(); i++) {
                std::string equation = equations.at(i);
                std::string replaced_expr = replaceVariables(equation, &variable_names);
                replaced_exprs.push_back(replaced_expr);
            }

        }
        int Process(std::vector<Data>* data) override {
            for (std::vector<Data>::iterator iter = data->begin(); iter != data->end(); ) {
                for (int i = 0; i < replaced_exprs.size(); i++) {
                    std::string replaced_expr = replaced_exprs.at(i);
                    double result = evaluateExpression(replaced_expr, iter->variable, &VariableTypes);
                    *(realvars.at(i)) = result;
                }

                RooArgSet temp_;
                for (int i = 0; i < replaced_exprs.size(); i++) temp_.add(*(realvars.at(i)));

                dataset->add(temp_, ObtainWeight(iter));

                ++iter;
            }
            return 1;
        }
        void End() override {}
    };

}

#endif 