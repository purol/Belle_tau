#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "functions.h"

int main(int argc, char* argv[]) {
    /*
    * argv[1]: variable name
    * argv[2]: input path 1
    * argv[3]: input path 2
    * argv[4]: output path
    * argv[5]: output name
    * argv[6]: MC list (separated by colon)
    * argv[7]: MC legend list (separated by colon)
    * argv[8]: data list (separated by colon, empty if there is no data)
    * argv[9]: data legend list (separated by colon, empty if there is no data)
    * argv[10]: weight path
    * argv[11]: weight name (without extension)
    * (argv[12]): min value
    * (argv[13]): max value
    * (argv[14]): control sample (CTRL or CTRL_ALP, requires min and max values)
    */

    if (argc != 12 && argc != 14 && argc != 15) {
        printf("11, 13 or 14 arguments are required.\n");
        return 1;
    }

    bool IsItALP = false;
    if (argc == 15) {
        std::string control_sample(argv[14]);
        if (control_sample != "CTRL" && control_sample != "CTRL_ALP") {
            printf("Control sample should be CTRL or CTRL_ALP.\n");
            return 1;
        }
        IsItALP = (control_sample == "CTRL_ALP");
    }

    std::string variable_name(argv[1]);

    std::vector<std::string> MC_list = split(argv[6], ':');
    std::vector<std::string> MC_legend_list = split(argv[7], ':');

    std::vector<std::string> data_list = split(argv[8], ':');
    std::vector<std::string> data_legend_list = split(argv[9], ':');

    FILE* fp = fopen((std::string(argv[10]) + "/" + std::string(argv[11]) + "_info.txt").c_str(), "r");
    if (fp == NULL) {
        printf("Cannot open weight information file.\n");
        return 1;
    }
    std::vector<std::string> compare_variables;
    double temp_double;
    char temp_char_one[100];
    char temp_char_two[100];
    if (fscanf(fp, "%lf", &temp_double) != 1 || fscanf(fp, "%lf", &temp_double) != 1 || fscanf(fp, "%99s", temp_char_one) != 1 || fscanf(fp, "%99s", temp_char_two) != 1) {
        printf("Cannot read weight information file.\n");
        fclose(fp);
        return 1;
    }
    compare_variables.push_back(temp_char_one);
    compare_variables.push_back(temp_char_two);
    fclose(fp);

    EventWeight CTRL_weight(
        std::string(argv[10]) + "/" + std::string(argv[11]) + ".csv",
        { {compare_variables.at(0), compare_variables.at(0) + "_min", compare_variables.at(0) + "_max"}, {compare_variables.at(1), compare_variables.at(1) + "_min", compare_variables.at(1) + "_max"} },
        "weight",
        true
    );
    EventWeights::Register("CTRL_weight", CTRL_weight);
    EventWeights::Register("MC_weight", MC_weight);

    if (argc == 15) {
        EventWeights::Register("pionID_09_prompt", pionID_09_prompt);
        EventWeights::Register("pionID_09_ALP", pionID_09_ALP);
        EventWeights::Register("KS0_tracking_run1", KS0_tracking_MC16rd_run1);
        EventWeights::Register("KS0_tracking_run2", KS0_tracking_MC16rd_run2);

        if (MC_list.empty() || MC_list.size() != MC_legend_list.size()) {
            printf("MC samples and legends should have the same nonzero size.\n");
            return 1;
        }
        if (data_list.size() != data_legend_list.size()) {
            printf("Data samples and legends should have the same size.\n");
            return 1;
        }

        double minimum = std::stod(argv[12]);
        double maximum = std::stod(argv[13]);
        if (!std::isfinite(minimum) || !std::isfinite(maximum) || minimum >= maximum) {
            printf("Invalid histogram range.\n");
            return 1;
        }

        // define histograms
        std::string histogram_title = ";" + std::string(argv[5]) + ";arbitrary unit";
        std::vector<std::string> stack_label_list = MC_legend_list;
        removeDuplicates(stack_label_list);
        std::vector<TH1D*> stack_hist;
        THStack stack("stack", histogram_title.c_str());
        TH1D stack_error("stack_error", histogram_title.c_str(), 50, minimum, maximum);
        TH1D hist("hist", histogram_title.c_str(), 50, minimum, maximum);
        TH1D RatioorPull("RatioorPull", histogram_title.c_str(), 50, minimum, maximum);
        stack_error.SetDirectory(nullptr); stack_error.Sumw2();
        hist.SetDirectory(nullptr); hist.Sumw2();
        RatioorPull.SetDirectory(nullptr); RatioorPull.Sumw2();

        // control MC
        for (int i = 0; i < stack_label_list.size(); i++) {
            TH1D* temp_hist = new TH1D(("stack_hist_" + std::to_string(i)).c_str(), histogram_title.c_str(), 50, minimum, maximum);
            temp_hist->SetDirectory(nullptr); temp_hist->Sumw2();
            stack_hist.push_back(temp_hist);
            stack.Add(temp_hist);

            Loader loader_MC("tau_lfv");
            for (int j = 0; j < MC_list.size(); j++) {
                if (MC_legend_list.at(j) == stack_label_list.at(i)) loader_MC.Load((argv[2] + std::string("/") + MC_list.at(j) + std::string("/") + std::string(argv[3])).c_str(), "root", MC_legend_list.at(j).c_str());
            }
            if (loader_MC.Getvariable_names_address()->empty()) {
                loader_MC.end();
                continue;
            }
            loader_MC.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
            if (IsItALP) {
                loader_MC.AddWeight("pionID_09_ALP", { {"index", "first_muon_index"},  {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
                loader_MC.AddWeight("pionID_09_ALP", { {"index", "second_muon_index"},  {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
                loader_MC.AddWeight("pionID_09_ALP", { {"index", "third_muon_index"},  {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
                loader_MC.AddWeight("KS0_tracking_run1", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
                loader_MC.AddWeight("KS0_tracking_run2", { {"sample", "MySampleType"}, {"experiment", "__experiment__"}, {"costheta", "extraInfo__boALP_cosTheta__bc"}, {"momentum", "extraInfo__boALP_p__bc"}, {"distance", "extraInfo__boALP_distance__bc"} });
            }
            else {
                loader_MC.AddWeight("pionID_09_prompt", { {"PID", "first_muon_mcPDG"}, {"momentum", "first_muon_p"}, {"Theta", "first_muon_theta"} });
                loader_MC.AddWeight("pionID_09_prompt", { {"PID", "second_muon_mcPDG"}, {"momentum", "second_muon_p"}, {"Theta", "second_muon_theta"} });
                loader_MC.AddWeight("pionID_09_prompt", { {"PID", "third_muon_mcPDG"}, {"momentum", "third_muon_p"}, {"Theta", "third_muon_theta"} });
            }
            loader_MC.AddWeight("CTRL_weight", { {compare_variables.at(0), compare_variables.at(0)}, {compare_variables.at(1), compare_variables.at(1)} });
            loader_MC.FillTH1D(temp_hist, variable_name);
            loader_MC.FillTH1D(&stack_error, variable_name);
            loader_MC.end();

        }

        // control data
        if (!data_list.empty()) {
            Loader loader_data("tau_lfv");
            for (int i = 0; i < data_list.size(); i++) loader_data.Load((argv[2] + std::string("/") + data_list.at(i) + std::string("/") + std::string(argv[3])).c_str(), "root", data_legend_list.at(i).c_str());
            if (!loader_data.Getvariable_names_address()->empty()) {
                loader_data.AddWeight("CTRL_weight", { {compare_variables.at(0), compare_variables.at(0)}, {compare_variables.at(1), compare_variables.at(1)} });
                loader_data.FillTH1D(&hist, variable_name);
            }
            loader_data.end();
        }

        // draw plot
        gStyle->SetPalette(kPastel);
        double real_max = stack.GetMaximum();
        if (hist.GetMaximum() > real_max) real_max = hist.GetMaximum();
        stack.SetMaximum(real_max * 1.4);

        TCanvas* c_temp = new TCanvas("c", "", 800, 800); c_temp->cd();
        if (!data_list.empty()) {
            TPad* pad1 = new TPad("pad1", "pad1", 0.0, 0.3, 1.0, 1.0);
            pad1->SetBottomMargin(0.02); pad1->SetLeftMargin(0.15);
            pad1->SetGridx(); pad1->Draw(); pad1->cd();
        }
        stack.Draw("pfc Hist");
        if (!data_list.empty()) {
            stack.GetXaxis()->SetLabelSize(0.0); stack.GetXaxis()->SetTitleSize(0.0);
        }
        stack_error.SetFillColor(12);
        stack_error.SetLineWidth(0);
        stack_error.SetFillStyle(3004);
        stack_error.Draw("e2 SAME");

        TLegend* legend = new TLegend(0.94, 0.9, 0.44, 0.7);
        for (int i = 0; i < stack_label_list.size(); i++) legend->AddEntry(stack_hist.at(i), stack_label_list.at(i).c_str(), "f");
        legend->AddEntry(&stack_error, "MC stat error", "f");
        if (!data_list.empty()) {
            hist.SetLineWidth(2);
            hist.SetLineColor(kBlack);
            hist.SetMarkerStyle(8);
            hist.Draw("SAME eP EX0");
            legend->AddEntry(&hist, "data", "LP");
        }
        legend->SetNColumns(3);
        legend->SetFillStyle(0); legend->SetLineWidth(0);
        legend->Draw();

        // write Belle text
        TPaveText* pt_belle = new TPaveText(0.13, 0.83, 0.37, 0.88, "NDC NB");
        pt_belle->SetTextSize(0.035); pt_belle->SetFillStyle(0); pt_belle->SetLineWidth(0); pt_belle->SetTextAlign(11); pt_belle->AddText("Belle II"); pt_belle->Draw();
        TPaveText* pt_lumi = new TPaveText(0.13, 0.79, 0.37, 0.81, "NDC NB");
        pt_lumi->SetTextSize(0.035); pt_lumi->SetFillStyle(0); pt_lumi->SetLineWidth(0); pt_lumi->SetTextAlign(11); pt_lumi->AddText("#int L dt = 365.4 fb^{-1}"); pt_lumi->Draw();

        // draw ratio/pull
        if (!data_list.empty()) {
            RatioorPull.SetLineColor(kBlack); RatioorPull.SetMarkerStyle(21); RatioorPull.SetStats(0);
            RatioorPull.Divide(&hist, &stack_error);
            c_temp->cd();
            TPad* pad2 = new TPad("pad2", "pad2", 0.0, 0.0, 1, 0.3);
            pad2->SetTopMargin(0.05);
            pad2->SetBottomMargin(0.2);
            pad2->SetLeftMargin(0.15);
            pad2->SetGridx();
            pad2->Draw(); pad2->cd();

            RatioorPull.SetMinimum(0.5); RatioorPull.SetMaximum(1.5); RatioorPull.SetLineWidth(2);
            RatioorPull.GetYaxis()->SetTitleSize(0.08); RatioorPull.GetYaxis()->SetTitleOffset(0.5); RatioorPull.GetYaxis()->SetLabelSize(0.08);
            RatioorPull.GetXaxis()->SetLabelSize(0.08); RatioorPull.GetXaxis()->SetTitleSize(0.08);
            RatioorPull.Draw("eP EX0");
            TLine* line = new TLine(minimum, 1.0, maximum, 1.0);
            line->SetLineColor(kRed);
            line->SetLineStyle(1); line->SetLineWidth(3);
            line->Draw();
        }

        c_temp->SaveAs((argv[4] + std::string("/") + argv[5] + ".png").c_str());
        delete c_temp;
        for (int i = 0; i < stack_hist.size(); i++) delete stack_hist.at(i);

        return 0;
    }

    Loader loader("tau_lfv");

    for (int i = 0; i < MC_list.size(); i++) loader.Load((argv[2] + std::string("/") + MC_list.at(i) + std::string("/") + std::string(argv[3])).c_str(), "root", MC_legend_list.at(i).c_str());
    for (int i = 0; i < data_list.size(); i++) loader.Load((argv[2] + std::string("/") + data_list.at(i) + std::string("/") + std::string(argv[3])).c_str(), "root", data_legend_list.at(i).c_str());
    loader.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader.AddWeight("CTRL_weight", { {compare_variables.at(0), compare_variables.at(0)}, {compare_variables.at(1), compare_variables.at(1)} });

    loader.SetMC(MC_legend_list);
    loader.SetData(data_legend_list);

    if(argc == 12) loader.DrawStack(variable_name.c_str(), (";" + std::string(argv[5]) + ";arbitrary unit").c_str(), (argv[4] + std::string("/") + argv[5] + ".png").c_str(), false, false);
    else if (argc == 14) loader.DrawStack(variable_name.c_str(), (";" + std::string(argv[5]) + ";arbitrary unit").c_str(), 50, std::stod(argv[12]), std::stod(argv[13]), (argv[4] + std::string("/") + argv[5] + ".png").c_str(), false, false);

    loader.end();

    return 0;
}
