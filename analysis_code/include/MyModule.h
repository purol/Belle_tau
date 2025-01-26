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
#include "TProfile.h"
#include "TH1.h"
#include "TH2.h"

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

    class FillTProfile : public Module {
        /*
        * This module is used to fill RooDataSet
        */
    private:

        RooDataSet* dataset;
        std::vector<RooRealVar*> realvars;

        TProfile* tprofile;

        std::string equation_x;
        std::string replaced_expr_x;

        std::string equation_y;
        std::string replaced_expr_y;

        std::vector<std::string> variable_names;
        std::vector<std::string> VariableTypes;

    public:
        FillTProfile(TProfile* tprofile_, std::string equation_x_, std::string equation_y_, std::vector<std::string>* variable_names_, std::vector<std::string>* VariableTypes_) : Module(), tprofile(tprofile_), equation_x(equation_x_), equation_y(equation_y_), variable_names(*variable_names_), VariableTypes(*VariableTypes_) {}
        ~FillTProfile() {}
        void Start() {
            replaced_expr_x = replaceVariables(equation_x, &variable_names);
            replaced_expr_y = replaceVariables(equation_y, &variable_names);
        }
        int Process(std::vector<Data>* data) override {
            for (std::vector<Data>::iterator iter = data->begin(); iter != data->end(); ) {
                double result_x = evaluateExpression(replaced_expr_x, iter->variable, &VariableTypes);
                double result_y = evaluateExpression(replaced_expr_y, iter->variable, &VariableTypes);

                tprofile->Fill(result_x, result_y, ObtainWeight(iter));

                ++iter;
            }
            return 1;
        }
        void End() override {}
    };

    class FillTH1D : public Module {
        /*
        * This module is used to fill TH1D
        */
    private:

        TH1D* th1d;

        std::string equation;
        std::string replaced_expr;

        std::vector<std::string> variable_names;
        std::vector<std::string> VariableTypes;

    public:
        FillTH1D(TH1D* th1d_, std::string equation_, std::vector<std::string>* variable_names_, std::vector<std::string>* VariableTypes_) : Module(), th1d(th1d_), equation(equation_), variable_names(*variable_names_), VariableTypes(*VariableTypes_) {}
        ~FillTH1D() {}
        void Start() {
            replaced_expr = replaceVariables(equation, &variable_names);
        }
        int Process(std::vector<Data>* data) override {
            for (std::vector<Data>::iterator iter = data->begin(); iter != data->end(); ) {
                double result = evaluateExpression(replaced_expr, iter->variable, &VariableTypes);

                th1d->Fill(result, ObtainWeight(iter));

                ++iter;
            }
            return 1;
        }
        void End() override {}
    };

    class FillCustomizedTH1D : public Module {
        /*
        * This module is used to fill TH1D with customized function
        */
    private:

        TH1D* th1d;
        double (*custom_function)(double);

        std::string equation;
        std::string replaced_expr;

        std::vector<std::string> variable_names;
        std::vector<std::string> VariableTypes;

    public:
        FillCustomizedTH1D(TH1D* th1d_, std::string equation_, double (*custom_function_)(double), std::vector<std::string>* variable_names_, std::vector<std::string>* VariableTypes_) : Module(), th1d(th1d_), equation(equation_), custom_function(custom_function_), variable_names(*variable_names_), VariableTypes(*VariableTypes_) {}
        ~FillCustomizedTH1D() {}
        void Start() {
            replaced_expr = replaceVariables(equation, &variable_names);
        }
        int Process(std::vector<Data>* data) override {
            for (std::vector<Data>::iterator iter = data->begin(); iter != data->end(); ) {
                double result = evaluateExpression(replaced_expr, iter->variable, &VariableTypes);

                th1d->Fill(custom_function(result), ObtainWeight(iter));

                ++iter;
            }
            return 1;
        }
        void End() override {}
    };

    class FillTH2D : public Module {
        /*
        * This module is used to fill TH2D
        */
    private:

        TH2D* th2d;

        std::string x_expression;
        std::string x_replaced_expr;
        std::string y_expression;
        std::string y_replaced_expr;

        std::vector<std::string> variable_names;
        std::vector<std::string> VariableTypes;

    public:
        FillTH2D(TH2D* th2d_, const char* x_expression_, const char* y_expression_, std::vector<std::string>* variable_names_, std::vector<std::string>* VariableTypes_) : Module(), th2d(th2d_), x_expression(x_expression_), y_expression(y_expression_), variable_names(*variable_names_), VariableTypes(*VariableTypes_) {}
        ~FillTH2D() {}
        void Start() {
            x_replaced_expr = replaceVariables(x_expression, &variable_names);
            y_replaced_expr = replaceVariables(y_expression, &variable_names);
        }
        int Process(std::vector<Data>* data) override {
            for (std::vector<Data>::iterator iter = data->begin(); iter != data->end(); ) {
                double x_result = evaluateExpression(x_replaced_expr, iter->variable, &VariableTypes);
                double y_result = evaluateExpression(y_replaced_expr, iter->variable, &VariableTypes);

                th1d->Fill(x_result, y_result, ObtainWeight(iter));

                ++iter;
            }
            return 1;
        }
        void End() override {}
    };

    class FillCustomizedTH2D : public Module {
        /*
        * This module is used to fill TH2D with customized function
        */
    private:

        TH2D* th2d;
        double (*x_custom_function)(double, double);
        double (*y_custom_function)(double, double);

        std::string x_expression;
        std::string x_replaced_expr;
        std::string y_expression;
        std::string y_replaced_expr;

        std::vector<std::string> variable_names;
        std::vector<std::string> VariableTypes;

    public:
        FillCustomizedTH2D(TH2D* th2d_, const char* x_expression_, const char* y_expression_, double (*x_custom_function_)(double, double), double (*y_custom_function_)(double, double), std::vector<std::string>* variable_names_, std::vector<std::string>* VariableTypes_) : Module(), th2d(th2d_), x_expression(x_expression_), y_expression(y_expression_), x_custom_function(x_custom_function_), y_custom_function(y_custom_function_), variable_names(*variable_names_), VariableTypes(*VariableTypes_) {}
        ~FillCustomizedTH2D() {}
        void Start() {
            x_replaced_expr = replaceVariables(x_expression, &variable_names);
            y_replaced_expr = replaceVariables(y_expression, &variable_names);
        }
        int Process(std::vector<Data>* data) override {
            for (std::vector<Data>::iterator iter = data->begin(); iter != data->end(); ) {
                double x_result = evaluateExpression(x_replaced_expr, iter->variable, &VariableTypes);
                double y_result = evaluateExpression(y_replaced_expr, iter->variable, &VariableTypes);

                th1d->Fill(x_custom_function(x_result, y_result), y_custom_function(x_result, y_result), ObtainWeight(iter));

                ++iter;
            }
            return 1;
        }
        void End() override {}
    };

}

#endif 