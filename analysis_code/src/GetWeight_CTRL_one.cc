#include <stdio.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <exception>

#include <TH1.h>
#include <TH2.h>
#include <TLatex.h>

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "functions.h"

std::string toStringWithPrecision(double value, int precision) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

int main(int argc, char* argv[]) {
    /*
    * argv[1]: input1 path 1 (CTRL)
    * argv[2]: input1 path 2 (CTRL)
    * argv[3]: input2 path 1 (SIGNAL)
    * argv[4]: input2 path 2 (SIGNAL)
    * argv[5]: compare variables (separated by colon)
    * argv[6]: binnings (separated by colon. min1:max1:numbin1:min2:max2:numbin2)
    * argv[7]: sample1 list (separated by colon)
    * argv[8]: sample2 list (separated by colon)
    * argv[9]: sample1 lable
    * argv[10]: sample2 lable
    * argv[11]: output path
    * argv[12]: M_deltaE path for tau -> mu mu mu decay
    * argv[13]: target variable name (name:min:max:bin)
    */

    if (argc != 14) {
        printf("13 arguments are required.\n");
        return 1;
    }

    std::vector<std::string> compare_variables = split(argv[5], ':');
    std::vector<std::string> binnings = split(argv[6], ':');
    if (compare_variables.size() < 2) {
        printf("At least two variables are needed.\n");
        return 1;
    }
    if (compare_variables.size() * 3 != binnings.size()) {
        printf("The number of variables and binnings should match.\n");
        return 1;
    }

    std::vector<double> binning_values;
    try {
        for (int i = 0; i < binnings.size(); i++) {
            double value = std::stod(binnings.at(i));
            binning_values.push_back(value);
        }
    }
    catch (const std::exception& error) {
        printf("invalid binning: %s\n", error.what());
        return 1;
    }

    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution((std::string(argv[12]) + "/M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    EventWeights::Register("MC_weight", MC_weight);

    std::vector<std::string> sample1_list = split(argv[7], ':');
    std::vector<std::string> sample2_list = split(argv[8], ':');

    // get the best variable combination
    std::vector<std::string> target_info = split(argv[13], ':');
    double originalD = 10; // KS distance before weighting
    double minD = 10;
    int var1_index = -1;
    int var2_index = -1;
    {
        std::vector<std::string> var1s;
        std::vector<int> var1s_index;
        std::vector<std::string> var2s;
        std::vector<int> var2s_index;

        std::vector<TH2D*> hist_CTRLs;
        std::vector<TH2D*> hist_SIGNALs;
        std::vector<TH2D*> hist_ratios;

        for (size_t i = 0; i < compare_variables.size(); i++) {
            for (size_t j = i + 1; j < compare_variables.size(); j++) {
                std::string var1 = compare_variables.at(i);
                std::string var2 = compare_variables.at(j);

                const double min1 = binning_values.at(3 * i);
                const double max1 = binning_values.at(3 * i + 1);
                const int nbins1 = static_cast<int>(binning_values.at(3 * i + 2));

                const double min2 = binning_values.at(3 * j);
                const double max2 = binning_values.at(3 * j + 1);
                const int nbins2 = static_cast<int>(binning_values.at(3 * j + 2));

                std::string histogram_title = ";" + var1 + ";" + var2;
                TH2D* hist_CTRL = new TH2D("hist_CTRL", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
                TH2D* hist_SIGNAL = new TH2D("hist_SIGNAL", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
                TH2D* hist_ratio = new TH2D("hist_ratio", (histogram_title + ";ratio").c_str(), nbins1, min1, max1, nbins2, min2, max2);

                // fill
                var1s.push_back(var1);
                var1s_index.push_back(i);
                var2s.push_back(var2);
                var2s_index.push_back(j);

                hist_CTRLs.push_back(hist_CTRL);
                hist_SIGNALs.push_back(hist_SIGNAL);
                hist_ratios.push_back(hist_ratio);
            }
        }

        // CTRL
        Loader loader_sample1("tau_lfv");
        for (int i = 0; i < sample1_list.size(); i++) loader_sample1.Load((std::string(argv[1]) + "/" + sample1_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", sample1_list.at(i).c_str());
        loader_sample1.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
        for (int k = 0; k < hist_ratios.size(); k++) loader_sample1.FillTH2D(hist_CTRLs.at(k), var1s.at(k).c_str(), var2s.at(k).c_str());
        loader_sample1.end();

        // SIGNAL
        Loader loader_sample2("tau_lfv");
        for (int i = 0; i < sample2_list.size(); i++) loader_sample2.Load((std::string(argv[3]) + "/" + sample2_list.at(i) + "/" + std::string(argv[4])).c_str(), "root", sample2_list.at(i).c_str());
        loader_sample2.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
        loader_sample2.Cut(GetBCSCut(deltaE_peak, deltaE_left_sigma, deltaE_right_sigma, M_peak, M_left_sigma, M_right_sigma).c_str());
        loader_sample2.RandomBCS();
        loader_sample2.IsBCSValid();
        loader_sample2.Cut(("(" + std::to_string(M_peak - 20 * M_left_sigma) + "< M) && (M < " + std::to_string(M_peak + 20 * M_right_sigma) + ")").c_str());
        loader_sample2.Cut(("(" + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + "< deltaE) && (deltaE < " + std::to_string(deltaE_peak + 5 * deltaE_right_sigma) + ")").c_str());
        for (int k = 0; k < hist_ratios.size(); k++) loader_sample2.FillTH2D(hist_SIGNALs.at(k), var1s.at(k).c_str(), var2s.at(k).c_str());
        loader_sample2.end();

        // get ratio
        for (int k = 0; k < hist_ratios.size(); k++) {
            const int nbins1 = hist_ratios.at(k)->GetNbinsX();
            const int nbins2 = hist_ratios.at(k)->GetNbinsY();
            for (int i = 1; i <= nbins1; i++) {
                for (int j = 1; j <= nbins2; j++) {
                    double N_CTRL = hist_CTRLs.at(k)->GetBinContent(i, j);
                    double N_SIGNAL = hist_SIGNALs.at(k)->GetBinContent(i, j);
                    if (N_CTRL > 0.000001) {
                        hist_ratios.at(k)->SetBinContent(i, j, N_SIGNAL / N_CTRL);
                    }
                    else {
                        hist_ratios.at(k)->SetBinContent(i, j, 0.0);
                    }
                }
            }
        }

        // construct event weight
        for (int k = 0; k < hist_ratios.size(); k++) {
            std::vector<WeightBin> weightbins;
            for (int i = 1; i <= hist_ratios.at(k)->GetNbinsX(); i++) {
                for (int j = 1; j <= hist_ratios.at(k)->GetNbinsY(); j++) {
                    weightbins.push_back(
                        WeightBin(
                            {
                                {hist_ratios.at(k)->GetXaxis()->GetBinLowEdge(i), hist_ratios.at(k)->GetXaxis()->GetBinUpEdge(i)},
                                {hist_ratios.at(k)->GetYaxis()->GetBinLowEdge(j), hist_ratios.at(k)->GetYaxis()->GetBinUpEdge(j)}
                            },
                            hist_ratios.at(k)->GetBinContent(i, j),
                            {})
                    );
                }
            }
            EventWeight tempweight = EventWeight({ var1s.at(k).c_str(), var2s.at(k).c_str() }, weightbins, true);

            EventWeights::Register(("weight_" + var1s.at(k) + var2s.at(k)).c_str(), tempweight);
        }

        // free memory
        for (int k = 0; k < hist_ratios.size(); k++) {
            delete hist_CTRLs.at(k);
            delete hist_SIGNALs.at(k);
            delete hist_ratios.at(k);
        }

        // fill target
        TH1D* hist_target_CTRL = new TH1D("hist_target_CTRL", (";" + target_info.at(0) + ";arbitrary unit").c_str(), std::stoi(target_info.at(3)), std::stod(target_info.at(1)), std::stod(target_info.at(2)));
        std::vector<TH1D*> hist_target_CTRL_weights;
        for (int k = 0; k < hist_ratios.size(); k++) {
            TH1D* hist_target_CTRL_weight = new TH1D(("hist_target_CTRL_weight_" + var1s.at(k) + var2s.at(k)).c_str(), (";" + target_info.at(0) + ";arbitrary unit").c_str(), std::stoi(target_info.at(3)), std::stod(target_info.at(1)), std::stod(target_info.at(2)));
            hist_target_CTRL_weights.push_back(hist_target_CTRL_weight);
        }
        TH1D* hist_target_SIGNAL = new TH1D("hist_target_SIGNAL", (";" + target_info.at(0) + ";arbitrary unit").c_str(), std::stoi(target_info.at(3)), std::stod(target_info.at(1)), std::stod(target_info.at(2)));

        // CTRL
        Loader loader_target_sample1("tau_lfv");
        for (int i = 0; i < sample1_list.size(); i++) loader_target_sample1.Load((std::string(argv[1]) + "/" + sample1_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", sample1_list.at(i).c_str());
        loader_target_sample1.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
        loader_target_sample1.FillTH1D(hist_target_CTRL, target_info.at(0).c_str());
        for (int k = 0; k < hist_ratios.size(); k++) {
            loader_target_sample1.AddWeight(("weight_" + var1s.at(k) + var2s.at(k)).c_str(), { {var1s.at(k).c_str(), var1s.at(k).c_str()}, {var2s.at(k).c_str(), var2s.at(k).c_str()} });
            loader_target_sample1.FillTH1D(hist_target_CTRL_weights.at(k), target_info.at(0).c_str());
            loader_target_sample1.RemoveWeight(("weight_" + var1s.at(k) + var2s.at(k)).c_str());
        }
        loader_target_sample1.end();

        // SIGNAL
        Loader loader_target_sample2("tau_lfv");
        for (int i = 0; i < sample2_list.size(); i++) loader_target_sample2.Load((std::string(argv[3]) + "/" + sample2_list.at(i) + "/" + std::string(argv[4])).c_str(), "root", sample2_list.at(i).c_str());
        loader_target_sample2.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
        loader_target_sample2.Cut(GetBCSCut(deltaE_peak, deltaE_left_sigma, deltaE_right_sigma, M_peak, M_left_sigma, M_right_sigma).c_str());
        loader_target_sample2.RandomBCS();
        loader_target_sample2.IsBCSValid();
        loader_target_sample2.Cut(("(" + std::to_string(M_peak - 20 * M_left_sigma) + "< M) && (M < " + std::to_string(M_peak + 20 * M_right_sigma) + ")").c_str());
        loader_target_sample2.Cut(("(" + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + "< deltaE) && (deltaE < " + std::to_string(deltaE_peak + 5 * deltaE_right_sigma) + ")").c_str());
        loader_target_sample2.FillTH1D(hist_target_SIGNAL, target_info.at(0).c_str());
        loader_target_sample2.end();

        // find shortest KS distance
        minD = originalD = hist_target_SIGNAL->KolmogorovTest(hist_target_CTRL, "M");
        for (int k = 0; k < hist_ratios.size(); k++) {
            double distance = hist_target_SIGNAL->KolmogorovTest(hist_target_CTRL_weights.at(k), "M");
            if (distance < minD) {
                minD = distance;
                var1_index = var1s_index.at(k);
                var2_index = var2s_index.at(k);
            }
        }

        // free memory
        delete hist_target_CTRL;
        for (int k = 0; k < hist_ratios.size(); k++) delete hist_target_CTRL_weights.at(k);
        delete hist_target_SIGNAL;
    }

    // if there is no improvement, just write unity
    if ((var1_index < 0) || (var2_index < 0)) {
        FILE* fp = fopen((std::string(argv[11]) + "/weight_one_2D_CTRL.csv").c_str(), "w");
        fprintf(fp, "weight,%s_min,%s_max,%s_min,%s_max", compare_variables.at(0).c_str(), compare_variables.at(0).c_str(), compare_variables.at(1).c_str(), compare_variables.at(1).c_str());
        fprintf(fp, "\n");
        fprintf(fp, "%.17g,%.17g,%.17g,%.17g,%.17g", 1.0, binning_values.at(0), binning_values.at(1), binning_values.at(3), binning_values.at(4));
        fclose(fp);

        fp = fopen((std::string(argv[11]) + "/weight_one_2D_CTRL_info.txt").c_str(), "w");
        fprintf(fp, "%lf\n%lf", originalD, minD);
        fprintf(fp, "%s\n%s", compare_variables.at(0).c_str(), compare_variables.at(1).c_str());
        fclose(fp);

        return 0;
    }

    // define histograms
    const double min1 = binning_values.at(3 * var1_index);
    const double max1 = binning_values.at(3 * var1_index + 1);
    const int nbins1 = static_cast<int>(binning_values.at(3 * var1_index + 2));
    const double min2 = binning_values.at(3 * var2_index);
    const double max2 = binning_values.at(3 * var2_index + 1);
    const int nbins2 = static_cast<int>(binning_values.at(3 * var2_index + 2));
    std::string histogram_title = ";" + compare_variables.at(var1_index) + ";" + compare_variables.at(var2_index);
    TH2D* hist_CTRL = new TH2D("hist_CTRL", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* hist_SIGNAL = new TH2D("hist_SIGNAL", (histogram_title + ";arbitrary unit").c_str(), nbins1, min1, max1, nbins2, min2, max2);
    TH2D* hist_ratio = new TH2D("hist_ratio", (histogram_title + ";ratio").c_str(), nbins1, min1, max1, nbins2, min2, max2);

    // CTRL
    Loader loader_sample1("tau_lfv");
    for (int i = 0; i < sample1_list.size(); i++) loader_sample1.Load((std::string(argv[1]) + "/" + sample1_list.at(i) + "/" + std::string(argv[2])).c_str(), "root", sample1_list.at(i).c_str());
    loader_sample1.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_sample1.FillTH2D(hist_CTRL, compare_variables.at(var1_index).c_str(), compare_variables.at(var2_index).c_str());
    loader_sample1.end();

    // SIGNAL
    Loader loader_sample2("tau_lfv");
    for (int i = 0; i < sample2_list.size(); i++) loader_sample2.Load((std::string(argv[3]) + "/" + sample2_list.at(i) + "/" + std::string(argv[4])).c_str(), "root", sample2_list.at(i).c_str());
    loader_sample2.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader_sample2.Cut(GetBCSCut(deltaE_peak, deltaE_left_sigma, deltaE_right_sigma, M_peak, M_left_sigma, M_right_sigma).c_str());
    loader_sample2.RandomBCS();
    loader_sample2.IsBCSValid();
    loader_sample2.Cut(("(" + std::to_string(M_peak - 20 * M_left_sigma) + "< M) && (M < " + std::to_string(M_peak + 20 * M_right_sigma) + ")").c_str());
    loader_sample2.Cut(("(" + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + "< deltaE) && (deltaE < " + std::to_string(deltaE_peak + 5 * deltaE_right_sigma) + ")").c_str());
    loader_sample2.FillTH2D(hist_SIGNAL, compare_variables.at(var1_index).c_str(), compare_variables.at(var2_index).c_str());
    loader_sample2.end();

    // calculate weights
    double N_CTRL_total = 0;
    // underflow and overflow bins are not used for weights
    for (int i = 1; i <= nbins1; i++) {
        for (int j = 1; j <= nbins2; j++) {
            double Nevt = hist_CTRL->GetBinContent(i, j);
            N_CTRL_total = N_CTRL_total + Nevt;
        }
    }

    double N_SIGNAL_total = 0;
    for (int i = 1; i <= nbins1; i++) {
        for (int j = 1; j <= nbins2; j++) {
            double Nevt = hist_SIGNAL->GetBinContent(i, j);
            N_SIGNAL_total = N_SIGNAL_total + Nevt;
        }
    }

    double N_SIGNAL_used_for_weight = 0.0;
    for (int i = 1; i <= nbins1; i++) {
        for (int j = 1; j <= nbins2; j++) {
            double N_CTRL = hist_CTRL->GetBinContent(i, j);
            double N_SIGNAL = hist_SIGNAL->GetBinContent(i, j);
            if (N_CTRL > 0.000001) {
                hist_ratio->SetBinContent(i, j, N_SIGNAL / N_CTRL);
                N_SIGNAL_used_for_weight = N_SIGNAL_used_for_weight + N_SIGNAL;
            }
            else {
                hist_ratio->SetBinContent(i, j, 0.0);
            }
        }
    }

    // this is needed to preserve N_CTRL_total
    hist_ratio->Scale(N_CTRL_total / N_SIGNAL_used_for_weight);

    FILE* fp = fopen((std::string(argv[11]) + "/weight_one_2D_CTRL.csv").c_str(), "w");
    fprintf(fp, "weight,%s_min,%s_max,%s_min,%s_max", compare_variables.at(var1_index).c_str(), compare_variables.at(var1_index).c_str(), compare_variables.at(var2_index).c_str(), compare_variables.at(var2_index).c_str());
    for (int i = 1; i <= nbins1; i++) {
        for (int j = 1; j <= nbins2; j++) {
            fprintf(fp, "\n");
            const double low1 = hist_ratio->GetXaxis()->GetBinLowEdge(i);
            const double high1 = hist_ratio->GetXaxis()->GetBinUpEdge(i);
            const double low2 = hist_ratio->GetYaxis()->GetBinLowEdge(j);
            const double high2 = hist_ratio->GetYaxis()->GetBinUpEdge(j);
            const double weight = hist_ratio->GetBinContent(i, j);
            fprintf(fp, "%.17g,%.17g,%.17g,%.17g,%.17g", weight, low1, high1, low2, high2);
        }
    }
    fclose(fp);

    fp = fopen((std::string(argv[11]) + "/weight_one_2D_CTRL_info.txt").c_str(), "w");
    fprintf(fp, "%lf\n%lf", originalD, minD);
    fprintf(fp, "%s\n%s", compare_variables.at(var1_index).c_str(), compare_variables.at(var2_index).c_str());
    fclose(fp);

    // make plots
    hist_CTRL->Scale(1.0 / hist_CTRL->Integral());
    hist_SIGNAL->Scale(1.0 / hist_SIGNAL->Integral());

    gStyle->SetOptStat(0);

    TCanvas* c_temp = new TCanvas("c", "", 1500, 500); c_temp->Divide(3, 1);

    double maxY = 0.0;

    if (hist_CTRL->GetMaximum() > hist_SIGNAL->GetMaximum()) maxY = hist_CTRL->GetMaximum();
    else maxY = hist_SIGNAL->GetMaximum();

    hist_CTRL->SetMinimum(0.0); hist_SIGNAL->SetMinimum(0.0);
    hist_CTRL->SetMaximum(maxY); hist_SIGNAL->SetMaximum(maxY);

    hist_CTRL->SetTitle(argv[9]); hist_SIGNAL->SetTitle(argv[10]);
    hist_ratio->SetTitle("weight");
    c_temp->cd(1); gPad->SetRightMargin(0.16); hist_CTRL->Draw("COLZ");
    c_temp->cd(2); gPad->SetRightMargin(0.16); hist_SIGNAL->Draw("COLZ");
    c_temp->cd(3); gPad->SetRightMargin(0.16); hist_ratio->Draw("COLZ");

    c_temp->SaveAs((std::string(argv[11]) + "/comparison_one_2D_CTRL.png").c_str());

    return 0;
}
