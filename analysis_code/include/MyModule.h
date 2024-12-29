#ifndef MYMODULE_H
#define MYMODULE_H

#include <string>
#include <vector>
#include <map>

#include <module.h>

namespace Module {

    class ConditionalCut : public Module {
    private:
        std::vector<std::string> equation_list;
        std::vector<std::string> replaced_expr_list;

        int condition_order; // start from 0. 0 means highest
        std::string inequality;
        double value;

        std::vector<std::string>* variable_names;
        std::vector<std::string>* VariableTypes;

        static char to_upper(char c) {
            return std::toupper(static_cast<unsigned char>(c));
        }

    public:
        ConditionalCut(std::vector<std::string> equation_list_, int condition_order_, const char* inequality_, double value_, std::vector<std::string>* variable_names_, std::vector<std::string>* VariableTypes_) : Module(), equation_list(equation_list_), condition_order(condition_order_), inequality(inequality_), value(value_), variable_names(variable_names_), VariableTypes(VariableTypes_) {}
        ~ConditionalCut() {}

        void Start() {
            for (int i = 0; i < equation_list.size(); i++) {
                std::string replaced_expr = replaceVariables(equation_list.at(i), variable_names);
                replaced_expr_list.push_back(replaced_expr);
            }

            // check `condition_order` is valid
            if (condition_order >= equation_list.size()) {
                printf("[ConditionalCut] condition order (%d) should be smaller than size of equation list (%d)\n", condition_order, equation_list.size());
                exit(1);
            }
            if (condition_order < 0) {
                printf("[ConditionalCut] condition order (%d) should be larger or equal to 0.\n");
                exit(1);
            }

            // check `inequality`
            if (inequality == "<") {}
            else if (inequality == ">") {}
            else if (inequality == "<=") {}
            else if (inequality == ">=") {}
            else if (inequality == "==") {}
            else if (inequality == "!=") {}
            else {
                printf("inequality for ConditionalCut should be one of `>`, `<`, `<=`, `>=`, `==`, or `!=`\n");
                exit(1);
            }
        }

        int Process(std::vector<Data>* data) override {
            for (std::vector<Data>::iterator iter = data->begin(); iter != data->end(); ) {
                double result = -1;
                std::vector<double> results;

                for (int i = 0; i < replaced_expr_list.size(); i++) {
                    double temp_ = evaluateExpression(replaced_expr_list.at(i), iter->variable, VariableTypes);
                    results.push_back(temp_);
                }

                std::vector<double> temp_results = results;
                std::nth_element(temp_results.begin(), temp_results.begin() + condition_order, temp_results.end(), std::greater<double>());

                // The n-th largest value
                result = temp_results.at(condition_order);

                // Find the original index of the n-th largest value
                std::vector<double>::iterator iter_results = std::find(results.begin(), results.end(), result);
                std::size_t index = std::distance(results.begin(), iter_results);

                // then consider about (in)equality
                if (inequality == "<") {
                    if (result < value) {
                        ++iter;
                    }
                    else data->erase(iter);
                }
                else if (inequality == ">") {
                    if (result > value) {
                        ++iter;
                    }
                    else data->erase(iter);
                }
                else if (inequality == "<=") {
                    if (result <= value) {
                        ++iter;
                    }
                    else data->erase(iter);
                }
                else if (inequality == ">=") {
                    if (result >= value) {
                        ++iter;
                    }
                    else data->erase(iter);
                }
                else if (inequality == "==") {
                    if (result == value) {
                        ++iter;
                    }
                    else data->erase(iter);
                }
                else if (inequality == "!=") {
                    if (result != value) {
                        ++iter;
                    }
                    else data->erase(iter);
                }

            }

            return 1;
        }

        void End() override {}
    };

    class ConditionalPairCut : public Module {
    private:
        /*
        * contition equation - criteria equation
        * conditional equation is used to select criteria equation. It is determined by highest or lowest
        * criteria equation is used to select candidates.
        */
        std::map<std::string, std::string> condition_equation__criteria_equation_list;
        std::map<std::string, std::string> condition_replaced_expr__criteria_replaced_expr_list;

        int condition_order; // start from 0. 0 means highest
        std::string inequality;
        double value;

        std::vector<std::string>* variable_names;
        std::vector<std::string>* VariableTypes;

        static char to_upper(char c) {
            return std::toupper(static_cast<unsigned char>(c));
        }

    public:
        ConditionalPairCut(std::map<std::string, std::string> condition_equation__criteria_equation_list_, int condition_order_, const char* inequality_, double value_, std::vector<std::string>* variable_names_, std::vector<std::string>* VariableTypes_) : Module(), condition_equation__criteria_equation_list(condition_equation__criteria_equation_list_), condition_order(condition_order_), inequality(inequality_), value(value_), variable_names(variable_names_), VariableTypes(VariableTypes_) {}
        ~ConditionalPairCut() {}

        void Start() {
            for (std::map<std::string, std::string>::iterator iter_eq = condition_equation__criteria_equation_list.begin(); iter_eq != condition_equation__criteria_equation_list.end(); ++iter_eq) {
                std::string condition_replaced_expr = replaceVariables(iter_eq->first, variable_names);
                std::string criteria_replaced_expr = replaceVariables(iter_eq->second, variable_names);

                condition_replaced_expr__criteria_replaced_expr_list.insert(std::make_pair(condition_replaced_expr, criteria_replaced_expr));
            }

            // check `condition_order` is valid
            if (condition_order >= condition_equation__criteria_equation_list.size()) {
                printf("[ConditionalPairCut] condition order (%d) should be smaller than size of condition_equation__criteria_equation_list (%d)\n", condition_order, condition_equation__criteria_equation_list.size());
                exit(1);
            }
            if (condition_order < 0) {
                printf("[ConditionalPairCut] condition order (%d) should be larger or equal to 0.\n");
                exit(1);
            }

            // check `inequality`
            if (inequality == "<") {}
            else if (inequality == ">") {}
            else if (inequality == "<=") {}
            else if (inequality == ">=") {}
            else if (inequality == "==") {}
            else if (inequality == "!=") {}
            else {
                printf("inequality for ConditionalPairCut should be one of `>`, `<`, `<=`, `>=`, `==`, or `!=`\n");
                exit(1);
            }
        }

        int Process(std::vector<Data>* data) override {
            for (std::vector<Data>::iterator iter = data->begin(); iter != data->end(); ) {
                double condition_result = -1;
                std::vector<double> condition_results;
                double criteria_result = std::numeric_limits<double>::max();
                std::vector<std::string> criteria_replaced_exprs;

                for (std::map<std::string, std::string>::iterator iter_eq = condition_replaced_expr__criteria_replaced_expr_list.begin(); iter_eq != condition_replaced_expr__criteria_replaced_expr_list.end(); ++iter_eq) {
                    double temp_ = evaluateExpression(iter_eq->first, iter->variable, VariableTypes);
                    condition_results.push_back(temp_);
                    criteria_replaced_exprs.push_back(iter_eq->second);
                }

                std::vector<double> temp_condition_results = condition_results;
                std::nth_element(temp_condition_results.begin(), temp_condition_results.begin() + condition_order, temp_condition_results.end(), std::greater<double>());

                // The n-th largest value
                condition_result = temp_condition_results.at(condition_order);

                // Find the original index of the n-th largest value
                std::vector<double>::iterator iter_condition_results = std::find(condition_results.begin(), condition_results.end(), condition_result);
                std::size_t index = std::distance(condition_results.begin(), iter_condition_results);

                criteria_result = evaluateExpression(criteria_replaced_exprs.at(index), iter->variable, VariableTypes);

                // then consider about (in)equality
                if (inequality == "<") {
                    if (criteria_result < value) {
                        ++iter;
                    }
                    else data->erase(iter);
                }
                else if (inequality == ">") {
                    if (criteria_result > value) {
                        ++iter;
                    }
                    else data->erase(iter);
                }
                else if (inequality == "<=") {
                    if (criteria_result <= value) {
                        ++iter;
                    }
                    else data->erase(iter);
                }
                else if (inequality == ">=") {
                    if (criteria_result >= value) {
                        ++iter;
                    }
                    else data->erase(iter);
                }
                else if (inequality == "==") {
                    if (criteria_result == value) {
                        ++iter;
                    }
                    else data->erase(iter);
                }
                else if (inequality == "!=") {
                    if (criteria_result != value) {
                        ++iter;
                    }
                    else data->erase(iter);
                }

            }

            return 1;
        }

        void End() override {}
    };

}

#endif 