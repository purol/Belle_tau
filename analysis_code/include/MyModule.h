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

    class FillDataSetWorkspace : public Module {
        /*
        * This module is used to fill RooDataSet for RooWorkspace
        * This module does not create/write workspace
        */
    private:
        RooWorkspace* w;

        RooDataSet* dataset;
        std::vector<RooRealVar*> realvars;
        RooRealVar* weight;

        std::vector<std::string> equations;
        std::vector<std::string> replaced_exprs;

        std::vector<std::string> variable_names;
        std::vector<std::string> VariableTypes;

    public:
        FillDataSetWorkspace(RooWorkspace* w_, RooDataSet* dataset_, std::vector<RooRealVar*> realvars_, RooRealVar* weight_, std::vector<std::string> equations_, std::vector<std::string>* variable_names_, std::vector<std::string>* VariableTypes_) : Module(), w(w_), dataset(dataset_), realvars(realvars_), weight(weight_), equations(equations_), variable_names(*variable_names_), VariableTypes(*VariableTypes_) {}
        ~FillDataSetWorkspace() {}
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
                *weight = ObtainWeight(iter);

                RooArgSet temp_;
                for (int i = 0; i < replaced_exprs.size(); i++) temp_.add(*(realvars.at(i)));

                dataset->add(temp_, weight->getVal());

                ++iter;
            }
            return 1;
        }
        void End() override {
            w->import(*dataset, RooFit::Rename("data"));
        }
    };

}

#endif 