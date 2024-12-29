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

        std::string condition;
        std::string inequality;
        double value;

        std::vector<std::string>* variable_names;
        std::vector<std::string>* VariableTypes;

        static char to_upper(char c) {
            return std::toupper(static_cast<unsigned char>(c));
        }

    public:
        ConditionalCut(std::vector<std::string> equation_list_, const char* condition_, const char* inequality_, double value_, std::vector<std::string>* variable_names_, std::vector<std::string>* VariableTypes_) : Module(), equation_list(equation_list_), condition(condition_), inequality(inequality_), value(value_), variable_names(variable_names_), VariableTypes(VariableTypes_) {}
        ~ConditionalCut() {}

        void Start() {
            for (int i = 0; i < equation_list.size(); i++) {
                std::string replaced_expr = replaceVariables(equation_list.at(i), variable_names);
                replaced_expr_list.push_back(replaced_expr);
            }

            // convert `condition` into upper case
            std::transform(condition.begin(), condition.end(), condition.begin(), to_upper);

            if ((condition != "HIGHEST") && (condition != "LOWEST")) {
                printf("condition for ConditionalCut should be `highest` or `lowest`\n");
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
                if (condition != "HIGHEST") result = std::numeric_limits<double>::lowest();
                else result = std::numeric_limits<double>::max();

                for (int i = 0; i < replaced_expr_list.size(); i++) {
                    double temp_ = evaluateExpression(replaced_expr_list.at(i), iter->variable, VariableTypes);
                    if ((condition != "HIGHEST") && (temp_ > result)) result = temp_;
                    else if ((condition != "LOWEST") && (temp_ < result)) result = temp_;
                }

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

        std::string condition;
        std::string inequality;
        double value;

        std::vector<std::string>* variable_names;
        std::vector<std::string>* VariableTypes;

        static char to_upper(char c) {
            return std::toupper(static_cast<unsigned char>(c));
        }

    public:
        ConditionalPairCut(std::map<std::string, std::string> condition_equation__criteria_equation_list_, const char* condition_, const char* inequality_, double value_, std::vector<std::string>* variable_names_, std::vector<std::string>* VariableTypes_) : Module(), condition_equation__criteria_equation_list(condition_equation__criteria_equation_list_), condition(condition_), inequality(inequality_), value(value_), variable_names(variable_names_), VariableTypes(VariableTypes_) {}
        ~ConditionalPairCut() {}

        void Start() {
            for (std::map<std::string, std::string>::iterator iter_eq = condition_equation__criteria_equation_list.begin(); iter_eq != condition_equation__criteria_equation_list.end(); ++iter_eq) {
                std::string condition_replaced_expr = replaceVariables(iter_eq->first, variable_names);
                std::string criteria_replaced_expr = replaceVariables(iter_eq->second, variable_names);

                condition_replaced_expr__criteria_replaced_expr_list.insert(std::make_pair(condition_replaced_expr, criteria_replaced_expr));
            }

            // convert `condition` into upper case
            std::transform(condition.begin(), condition.end(), condition.begin(), to_upper);

            if ((condition != "HIGHEST") && (condition != "LOWEST")) {
                printf("condition for ConditionalPairCut should be `highest` or `lowest`\n");
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
                double criteria_result = std::numeric_limits<double>::max();
                if (condition != "HIGHEST") condition_result = std::numeric_limits<double>::lowest();
                else condition_result = std::numeric_limits<double>::max();

                for (std::map<std::string, std::string>::iterator iter_eq = condition_replaced_expr__criteria_replaced_expr_list.begin(); iter_eq != condition_replaced_expr__criteria_replaced_expr_list.end(); ++iter_eq) {
                    double temp_ = evaluateExpression(iter_eq->first, iter->variable, VariableTypes);
                    if ((condition != "HIGHEST") && (temp_ > condition_result)) {
                        condition_result = temp_;
                        criteria_result = evaluateExpression(iter_eq->second, iter->variable, VariableTypes);
                    }
                    else if ((condition != "LOWEST") && (temp_ < condition_result)) {
                        condition_result = temp_;
                        criteria_result = evaluateExpression(iter_eq->second, iter->variable, VariableTypes);
                    }
                }

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