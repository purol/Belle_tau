#ifndef MYMODULE_H
#define MYMODULE_H

#include <string>
#include <vector>
#include <map>

#include <module.h>

namespace Module {

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

    class ConditionalPairDrawStack : public Module {
    private:
        THStack* stack;
        TH1D** stack_hist;
        TH1D* stack_error;
        TH1D* hist;
        TH1D* RatioorPull;
        std::string stack_title;
        int nbins;
        double x_low;
        double x_high;
        bool normalized;
        bool LogScale;

        std::vector<std::string>* variable_names;
        std::vector<std::string>* VariableTypes;
        std::map<std::string, std::string> condition_equation__criteria_equation_list;
        std::map<std::string, std::string> condition_replaced_expr__criteria_replaced_expr_list;
        int condition_order; // start from 0. 0 means highest

        std::string png_name;

        std::vector<double> x_variable;
        std::vector<double> weight;
        std::vector<std::string> label;

        std::vector<std::string> Signal_label_list;
        std::vector<std::string> Background_label_list;
        std::vector<std::string> data_label_list;
        std::vector<std::string> MC_label_list;

        std::vector<std::string> stack_label_list;
        std::vector<std::string> hist_label_list;

        /*
        * draw option:
        * 0: `SetMC` and `SetData`. stack MC and black dot data
        * 1: `SetSignal` and `SetBackground`. red line signal and stack background
        * 2: `SetMC` only. stack only
        */
        int hist_draw_option;

    public:
        ConditionalPairDrawStack(std::map<std::string, std::string> condition_equation__criteria_equation_list_, int condition_order_, const char* stack_title_, int nbins_, double x_low_, double x_high_, const char* png_name_, std::vector<std::string> Signal_label_list_, std::vector<std::string> Background_label_list_, std::vector<std::string> data_label_list_, std::vector<std::string> MC_label_list_, std::vector<std::string>* variable_names_, std::vector<std::string>* VariableTypes_) : Module(), condition_equation__criteria_equation_list(condition_equation__criteria_equation_list_), condition_order(condition_order_), stack_title(stack_title_), nbins(nbins_), x_low(x_low_), x_high(x_high_), png_name(png_name_), normalized(false), LogScale(false), Signal_label_list(Signal_label_list_), Background_label_list(Background_label_list_), data_label_list(data_label_list_), MC_label_list(MC_label_list_), variable_names(variable_names_), VariableTypes(VariableTypes_) {}
        ConditionalPairDrawStack(std::map<std::string, std::string> condition_equation__criteria_equation_list_, int condition_order_, const char* stack_title_, int nbins_, double x_low_, double x_high_, const char* png_name_, bool normalized_, bool LogScale_, std::vector<std::string> Signal_label_list_, std::vector<std::string> Background_label_list_, std::vector<std::string> data_label_list_, std::vector<std::string> MC_label_list_, std::vector<std::string>* variable_names_, std::vector<std::string>* VariableTypes_) : Module(), condition_equation__criteria_equation_list(condition_equation__criteria_equation_list_), condition_order(condition_order_), stack_title(stack_title_), nbins(nbins_), x_low(x_low_), x_high(x_high_), png_name(png_name_), normalized(normalized_), LogScale(LogScale_), Signal_label_list(Signal_label_list_), Background_label_list(Background_label_list_), data_label_list(data_label_list_), MC_label_list(MC_label_list_), variable_names(variable_names_), VariableTypes(VariableTypes_) {}
        ConditionalPairDrawStack(std::map<std::string, std::string> condition_equation__criteria_equation_list_, int condition_order_, const char* stack_title_, const char* png_name_, std::vector<std::string> Signal_label_list_, std::vector<std::string> Background_label_list_, std::vector<std::string> data_label_list_, std::vector<std::string> MC_label_list_, std::vector<std::string>* variable_names_, std::vector<std::string>* VariableTypes_) : Module(), condition_equation__criteria_equation_list(condition_equation__criteria_equation_list_), condition_order(condition_order_), stack_title(stack_title_), nbins(50), x_low(std::numeric_limits<double>::max()), x_high(std::numeric_limits<double>::max()), png_name(png_name_), normalized(false), LogScale(false), Signal_label_list(Signal_label_list_), Background_label_list(Background_label_list_), data_label_list(data_label_list_), MC_label_list(MC_label_list_), variable_names(variable_names_), VariableTypes(VariableTypes_) {}
        ConditionalPairDrawStack(std::map<std::string, std::string> condition_equation__criteria_equation_list_, int condition_order_, const char* stack_title_, const char* png_name_, bool normalized_, bool LogScale_, std::vector<std::string> Signal_label_list_, std::vector<std::string> Background_label_list_, std::vector<std::string> data_label_list_, std::vector<std::string> MC_label_list_, std::vector<std::string>* variable_names_, std::vector<std::string>* VariableTypes_) : Module(), condition_equation__criteria_equation_list(condition_equation__criteria_equation_list_), condition_order(condition_order_), stack_title(stack_title_), nbins(50), x_low(std::numeric_limits<double>::max()), x_high(std::numeric_limits<double>::max()), png_name(png_name_), normalized(normalized_), LogScale(LogScale_), Signal_label_list(Signal_label_list_), Background_label_list(Background_label_list_), data_label_list(data_label_list_), MC_label_list(MC_label_list_), variable_names(variable_names_), VariableTypes(VariableTypes_) {}

        ~ConditionalPairDrawStack() {
            delete stack;
            for (int i = 0; i < stack_label_list.size(); i++) delete stack_hist[i];
            free(stack_hist);
            delete stack_error;
            delete hist;
        }

        void Start() override {
            stack = nullptr;
            stack_hist = nullptr;
            stack_error = nullptr;
            hist = nullptr;
            RatioorPull = nullptr;

            // actually, the first and third else-if can be written in one line. However, I write them into the two line explicitly
            if ((data_label_list.size() != 0) && (MC_label_list.size() != 0)) {}
            else if ((Signal_label_list.size() != 0) && (Background_label_list.size() != 0)) {}
            else if ((data_label_list.size() == 0) && (MC_label_list.size() != 0)) {}
            else {
                printf("`DrawStack` requires one of them:\n");
                printf("1. `SetMC` and `SetData`\n");
                printf("2. `SetSignal` and `SetBackground`\n");
                printf("3. `SetMC` only\n");
                exit(1);
            }

            if ((data_label_list.size() != 0) && (MC_label_list.size() != 0)) {
                hist_label_list = data_label_list;
                stack_label_list = MC_label_list;
                hist_draw_option = 0;
            }
            else if ((Signal_label_list.size() != 0) && (Background_label_list.size() != 0)) {
                hist_label_list = Signal_label_list;
                stack_label_list = Background_label_list;
                hist_draw_option = 1;
            }
            else if ((data_label_list.size() == 0) && (MC_label_list.size() != 0)) {
                hist_label_list = {};
                stack_label_list = MC_label_list;
                hist_draw_option = 2;
            }
            else {
                printf("never reach\n");
                exit(1);
            }

            // change variable name into placeholder
            for (std::map<std::string, std::string>::iterator iter_eq = condition_equation__criteria_equation_list.begin(); iter_eq != condition_equation__criteria_equation_list.end(); ++iter_eq) {
                std::string condition_replaced_expr = replaceVariables(iter_eq->first, variable_names);
                std::string criteria_replaced_expr = replaceVariables(iter_eq->second, variable_names);

                condition_replaced_expr__criteria_replaced_expr_list.insert(std::make_pair(condition_replaced_expr, criteria_replaced_expr));
            }

            if ((x_low != std::numeric_limits<double>::max()) && (x_high != std::numeric_limits<double>::max())) {
                std::string hist_name = generateRandomString(12);
                hist = new TH1D(hist_name.c_str(), stack_title.c_str(), nbins, x_low, x_high);

                // create histogram for stack
                stack_hist = (TH1D**)malloc(sizeof(TH1D*) * stack_label_list.size());
                for (int i = 0; i < stack_label_list.size(); i++) {
                    std::string hist_name = generateRandomString(12);
                    stack_hist[i] = new TH1D(hist_name.c_str(), stack_title.c_str(), nbins, x_low, x_high);
                }
                hist_name = generateRandomString(12);
                stack_error = new TH1D(hist_name.c_str(), stack_title.c_str(), nbins, x_low, x_high);

                // create pull or ratio histogram
                hist_name = generateRandomString(12);
                RatioorPull = new TH1D(hist_name.c_str(), stack_title.c_str(), nbins, x_low, x_high);
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

                if ((std::find(stack_label_list.begin(), stack_label_list.end(), iter->label) != stack_label_list.end()) || (std::find(hist_label_list.begin(), hist_label_list.end(), iter->label) != hist_label_list.end())) {

                    if (stack_hist == nullptr) {
                        x_variable.push_back(criteria_result);
                        weight.push_back(ObtainWeight(iter));
                        label.push_back(iter->label);
                    }
                    else {
                        if (std::find(stack_label_list.begin(), stack_label_list.end(), iter->label) != stack_label_list.end()) {
                            int label_index = std::find(stack_label_list.begin(), stack_label_list.end(), iter->label) - stack_label_list.begin();
                            stack_hist[label_index]->Fill(criteria_result, ObtainWeight(iter));
                            stack_error->Fill(criteria_result, ObtainWeight(iter));
                        }
                        else if (std::find(hist_label_list.begin(), hist_label_list.end(), iter->label) != hist_label_list.end()) {
                            hist->Fill(criteria_result, ObtainWeight(iter));
                        }
                    }

                    // if saved variable exceed 10MB, calculate max, min and create histogram. It is to save memory
                    if ((sizeof(double) * x_variable.size() > 10000000.0) && (stack_hist == nullptr)) {
                        std::vector<double>::iterator min_it = std::min_element(x_variable.begin(), x_variable.end());
                        std::vector<double>::iterator max_it = std::max_element(x_variable.begin(), x_variable.end());

                        x_low = *min_it;
                        x_high = *max_it;

                        // create histogram
                        std::string hist_name = generateRandomString(12);
                        hist = new TH1D(hist_name.c_str(), stack_title.c_str(), nbins, x_low, x_high);

                        // create histogram for stack
                        stack_hist = (TH1D**)malloc(sizeof(TH1D*) * stack_label_list.size());
                        for (int i = 0; i < stack_label_list.size(); i++) {
                            std::string hist_name = generateRandomString(12);
                            stack_hist[i] = new TH1D(hist_name.c_str(), stack_title.c_str(), nbins, x_low, x_high);
                        }
                        hist_name = generateRandomString(12);
                        stack_error = new TH1D(hist_name.c_str(), stack_title.c_str(), nbins, x_low, x_high);

                        // create pull or ratio histogram
                        hist_name = generateRandomString(12);
                        RatioorPull = new TH1D(hist_name.c_str(), stack_title.c_str(), nbins, x_low, x_high);

                        // fill histogram
                        for (int i = 0; i < weight.size(); i++) {
                            if (std::find(hist_label_list.begin(), hist_label_list.end(), label.at(i)) != hist_label_list.end()) {
                                hist->Fill(x_variable.at(i), weight.at(i));
                            }
                        }

                        // fill histogram for stack
                        for (int i = 0; i < weight.size(); i++) {
                            if (std::find(stack_label_list.begin(), stack_label_list.end(), label.at(i)) != stack_label_list.end()) {
                                int label_index = std::find(stack_label_list.begin(), stack_label_list.end(), label.at(i)) - stack_label_list.begin();
                                stack_hist[label_index]->Fill(x_variable.at(i), weight.at(i));
                                stack_error->Fill(x_variable.at(i), weight.at(i));
                            }
                        }

                        x_variable.clear();
                        std::vector<double>().swap(x_variable);
                        weight.clear();
                        std::vector<double>().swap(weight);
                        label.clear();
                        std::vector<std::string>().swap(label);
                    }

                }

                ++iter;
            }

            return 1;
        }

        void End() override {
            // if range is not determined, determined from this side
            if ((x_low == std::numeric_limits<double>::max()) && (x_high == std::numeric_limits<double>::max())) {
                std::vector<double>::iterator min_it = std::min_element(x_variable.begin(), x_variable.end());
                std::vector<double>::iterator max_it = std::max_element(x_variable.begin(), x_variable.end());

                x_low = *min_it;
                x_high = *max_it;
            }

            // create stack
            std::string stack_name = generateRandomString(12);
            stack = new THStack(stack_name.c_str(), stack_title.c_str());

            if (stack_hist == nullptr) {
                // create histogram
                std::string hist_name = generateRandomString(12);
                hist = new TH1D(hist_name.c_str(), stack_title.c_str(), nbins, x_low, x_high);

                // create histogram for stack
                stack_hist = (TH1D**)malloc(sizeof(TH1D*) * stack_label_list.size());
                for (int i = 0; i < stack_label_list.size(); i++) {
                    std::string hist_name = generateRandomString(12);
                    stack_hist[i] = new TH1D(hist_name.c_str(), stack_title.c_str(), nbins, x_low, x_high);
                }
                hist_name = generateRandomString(12);
                stack_error = new TH1D(hist_name.c_str(), stack_title.c_str(), nbins, x_low, x_high);

                // create pull or ratio histogram
                hist_name = generateRandomString(12);
                RatioorPull = new TH1D(hist_name.c_str(), stack_title.c_str(), nbins, x_low, x_high);
            }

            // fill histogram
            for (int i = 0; i < weight.size(); i++) {
                if (std::find(hist_label_list.begin(), hist_label_list.end(), label.at(i)) != hist_label_list.end()) {
                    hist->Fill(x_variable.at(i), weight.at(i));
                }
            }

            // fill histogram for stack
            for (int i = 0; i < weight.size(); i++) {
                if (std::find(stack_label_list.begin(), stack_label_list.end(), label.at(i)) != stack_label_list.end()) {
                    int label_index = std::find(stack_label_list.begin(), stack_label_list.end(), label.at(i)) - stack_label_list.begin();
                    stack_hist[label_index]->Fill(x_variable.at(i), weight.at(i));
                    stack_error->Fill(x_variable.at(i), weight.at(i));
                }
            }

            // fill pull or ratio
            RatioorPull->SetLineColor(kBlack); RatioorPull->SetMarkerStyle(21); RatioorPull->Sumw2(); RatioorPull->SetStats(0);
            RatioorPull->Divide(hist, stack_error);

            // clear vector. Maybe not needed but to save memory...
            x_variable.clear();
            std::vector<double>().swap(x_variable);
            weight.clear();
            std::vector<double>().swap(weight);
            label.clear();
            std::vector<std::string>().swap(label);

            if (normalized) {
                if (hist_draw_option == 0) printf("[DrawStack] normalized option does not work when there is data\n");
                else if (hist_draw_option == 1) {
                    double sum_int = 0;
                    for (int i = 0; i < stack_label_list.size(); i++) sum_int = sum_int + stack_hist[i]->Integral();
                    for (int i = 0; i < stack_label_list.size(); i++) stack_hist[i]->Scale(1.0 / sum_int, "width");
                    stack_error->Scale(1.0 / stack_error->Integral(), "width");
                    hist->Scale(1.0 / hist->Integral(), "width");
                }
                else if (hist_draw_option == 2) {
                    double sum_int = 0;
                    for (int i = 0; i < stack_label_list.size(); i++) sum_int = sum_int + stack_hist[i]->Integral();
                    for (int i = 0; i < stack_label_list.size(); i++) stack_hist[i]->Scale(1.0 / sum_int, "width");
                    stack_error->Scale(1.0 / stack_error->Integral(), "width");
                }
            }

            // stack histogram
            for (int i = 0; i < stack_label_list.size(); i++) {
                stack->Add(stack_hist[i]);
            }

            // set palette
            gStyle->SetPalette(kPastel);

            // set maximum value of y-axis
            double ymax_1 = stack->GetMaximum();
            double ymax_2 = hist->GetMaximum();
            double real_max = 0;
            if (ymax_1 > ymax_2) real_max = ymax_1;
            else real_max = ymax_2;

            stack->SetMaximum(real_max * 1.4);

            if (hist_draw_option == 0) {
                // define Canvas and pad
                TCanvas* c_temp = new TCanvas("c", "", 800, 800); c_temp->cd();
                TPad* pad1 = new TPad("pad1", "pad1", 0.0, 0.3, 1.0, 1.0);
                pad1->SetBottomMargin(0.02); pad1->SetLeftMargin(0.15);
                pad1->SetGridx(); pad1->Draw(); pad1->cd();
                if (LogScale) pad1->SetLogy(1);
                else pad1->SetLogy(0);

                stack->Draw("pfc Hist"); stack->GetXaxis()->SetLabelSize(0.0); stack->GetXaxis()->SetTitleSize(0.0);

                stack_error->SetFillColor(12);
                stack_error->SetLineWidth(0);
                stack_error->SetFillStyle(3004);
                stack_error->Draw("e2 SAME");

                hist->SetLineWidth(2);
                hist->SetLineColor(kBlack);
                hist->SetMarkerStyle(8);
                hist->Draw("SAME eP EX0");

                // set legend
                TLegend* legend = new TLegend(0.94, 0.9, 0.44, 0.7);
                for (int i = 0; i < stack_label_list.size(); i++) {
                    legend->AddEntry(stack_hist[i], stack_label_list.at(i).c_str(), "f");
                }
                legend->AddEntry(stack_error, "MC stat error", "f");
                legend->AddEntry(hist, "data", "LP");
                legend->SetNColumns(3);

                legend->SetFillStyle(0); legend->SetLineWidth(0);
                legend->Draw();

                // write Belle text
                TPaveText* pt_belle = new TPaveText(0.13, 0.83, 0.37, 0.88, "NDC NB");
                pt_belle->SetTextSize(0.035); pt_belle->SetFillStyle(0); pt_belle->SetLineWidth(0); pt_belle->SetTextAlign(11); pt_belle->AddText("Belle II"); pt_belle->Draw();
                TPaveText* pt_lumi = new TPaveText(0.13, 0.79, 0.37, 0.81, "NDC NB");
                pt_lumi->SetTextSize(0.035); pt_lumi->SetFillStyle(0); pt_lumi->SetLineWidth(0); pt_lumi->SetTextAlign(11); pt_lumi->AddText("#int L dt = 365.4 fb^{-1}"); pt_lumi->Draw();

                // draw ratio/pull
                c_temp->cd();
                TPad* pad2 = new TPad("pad2", "pad2", 0.0, 0.0, 1, 0.3);
                pad2->SetTopMargin(0.05);
                pad2->SetBottomMargin(0.2);
                pad2->SetLeftMargin(0.15);
                pad2->SetGridx();
                pad2->Draw();
                pad2->cd();

                RatioorPull->SetMinimum(0.5); RatioorPull->SetMaximum(1.5); RatioorPull->SetLineWidth(2);
                RatioorPull->GetYaxis()->SetTitleSize(0.08); RatioorPull->GetYaxis()->SetTitleOffset(0.5); RatioorPull->GetYaxis()->SetLabelSize(0.08);
                RatioorPull->GetXaxis()->SetLabelSize(0.08); RatioorPull->GetXaxis()->SetTitleSize(0.08);
                RatioorPull->Draw("eP EX0");
                TLine* line = new TLine(RatioorPull->GetXaxis()->GetXmin(), 1.0, RatioorPull->GetXaxis()->GetXmax(), 1.0);
                line->SetLineColor(kRed);
                line->SetLineStyle(1); line->SetLineWidth(3);
                line->Draw();

                c_temp->SaveAs(png_name.c_str());
                delete c_temp;
            }
            else if (hist_draw_option == 1) {
                // define Canvas and pad
                TCanvas* c_temp = new TCanvas("c", "", 800, 800); c_temp->cd();
                if (LogScale) gPad->SetLogy(1);
                else gPad->SetLogy(0);

                stack->Draw("pfc Hist");

                stack_error->SetFillColor(12);
                stack_error->SetLineWidth(0);
                stack_error->SetFillStyle(3004);
                stack_error->Draw("e2 SAME");

                hist->SetLineWidth(3);
                hist->SetLineColor(2);
                hist->SetFillStyle(0);
                hist->Draw("Hist SAME");

                // set legend
                TLegend* legend = new TLegend(0.94, 0.9, 0.44, 0.7);
                for (int i = 0; i < stack_label_list.size(); i++) {
                    legend->AddEntry(stack_hist[i], stack_label_list.at(i).c_str(), "f");
                }
                legend->AddEntry(stack_error, "MC stat error", "f");
                legend->AddEntry(hist, "signal", "f");
                legend->SetNColumns(3);

                legend->SetFillStyle(0); legend->SetLineWidth(0);
                legend->Draw();

                // write Belle text
                TPaveText* pt_belle = new TPaveText(0.13, 0.83, 0.37, 0.88, "NDC NB");
                pt_belle->SetTextSize(0.035); pt_belle->SetFillStyle(0); pt_belle->SetLineWidth(0); pt_belle->SetTextAlign(11); pt_belle->AddText("Belle II"); pt_belle->Draw();
                TPaveText* pt_lumi = new TPaveText(0.13, 0.79, 0.37, 0.81, "NDC NB");
                pt_lumi->SetTextSize(0.035); pt_lumi->SetFillStyle(0); pt_lumi->SetLineWidth(0); pt_lumi->SetTextAlign(11); pt_lumi->AddText("#int L dt = 365.4 fb^{-1}"); pt_lumi->Draw();

                c_temp->SaveAs(png_name.c_str());
                delete c_temp;
            }
            else if (hist_draw_option == 2) {
                // define Canvas and pad
                TCanvas* c_temp = new TCanvas("c", "", 800, 800); c_temp->cd();
                if(LogScale) gPad->SetLogy(1);
                else gPad->SetLogy(0);

                stack->Draw("pfc Hist");

                stack_error->SetFillColor(12);
                stack_error->SetLineWidth(0);
                stack_error->SetFillStyle(3004);
                stack_error->Draw("e2 SAME");

                // set legend
                TLegend* legend = new TLegend(0.94, 0.9, 0.44, 0.7);
                for (int i = 0; i < stack_label_list.size(); i++) {
                    legend->AddEntry(stack_hist[i], stack_label_list.at(i).c_str(), "f");
                }
                legend->AddEntry(stack_error, "MC stat error", "f");
                legend->SetNColumns(3);

                legend->SetFillStyle(0); legend->SetLineWidth(0);
                legend->Draw();

                // write Belle text
                TPaveText* pt_belle = new TPaveText(0.13, 0.83, 0.37, 0.88, "NDC NB");
                pt_belle->SetTextSize(0.035); pt_belle->SetFillStyle(0); pt_belle->SetLineWidth(0); pt_belle->SetTextAlign(11); pt_belle->AddText("Belle II"); pt_belle->Draw();
                TPaveText* pt_lumi = new TPaveText(0.13, 0.79, 0.37, 0.81, "NDC NB");
                pt_lumi->SetTextSize(0.035); pt_lumi->SetFillStyle(0); pt_lumi->SetLineWidth(0); pt_lumi->SetTextAlign(11); pt_lumi->AddText("#int L dt = 365.4 fb^{-1}"); pt_lumi->Draw();

                c_temp->SaveAs(png_name.c_str());
                delete c_temp;
            }
            else {
                printf("never reach\n");
                exit(1);
            }

        }
    };

}

#endif 