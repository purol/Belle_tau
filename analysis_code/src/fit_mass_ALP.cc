#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <regex>
#include <set>
#include <filesystem>
#include <format>

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "MyModule.h"

#include <TCanvas.h>
#include <RooWorkspace.h>
#include <RooDataSet.h>
#include <RooRealVar.h>
#include <RooArgSet.h>
#include <RooBifurGauss.h>
#include <RooExtendPdf.h>
#include <RooFitResult.h>
#include <RooPlot.h>
#include <RooHist.h>
#include <TPad.h>
#include <TAxis.h>
#include <TLatex.h>
#include <TProfile.h>
#include <TF1.h>

// https://gitlab.desy.de/belle2/publications/73/tau_eellell/-/blob/main/processing/SignalRegion_fit.py

std::string toStringWithPrecision(double value, int precision) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

struct Params {
    double mass;
    double life;
    int A;
    int B;

    bool operator<(const Params& other) const {
        return std::tie(mass, life, A, B) <
            std::tie(other.mass, other.life, other.A, other.B);
    }
};

std::set<Params> GetParameters(const char* dirname) {
    std::regex pattern(R"(alpha_mass([0-9.+-eE]+)_life([0-9.+-eE]+)_A([0-9+-]+)_B([0-9+-]+))");

    std::set<Params> results;

    for (const auto& entry : std::filesystem::directory_iterator(dirname)) {
        if (!entry.is_regular_file()) continue;

        std::string filename = entry.path().filename().string();
        std::smatch match;

        if (std::regex_search(filename, match, pattern)) {
            Params p;
            p.mass = std::stod(match[1]);
            p.life = std::stod(match[2]);
            p.A = std::stoi(match[3]);
            p.B = std::stoi(match[4]);

            results.insert(p);
        }
    }

    return results;

}

double GetMIN(double a, double b) {
    if (a < b) return a;
    else return b;
}

double GetMAX(double a, double b) {
    if (a < b) return b;
    else return a;
}

int main(int argc, char* argv[]) {
    /*
    * argv[1]: dirname
    * argv[2]: output path
    */

    std::set<Params> parameters = GetParameters(argv[1]);

    // open file to save the results
    FILE* fp = fopen((std::string(argv[2]) + "/M_deltaE_result.csv").c_str(), "w");

    for (const auto& p : parameters) {
        ObtainWeight = MyScaleFunction;

        Loader loader("tau_lfv");

        loader.Load(argv[1], ("alpha_mass" + std::format("{:g}", p.mass) + "_life" + std::format("{:g}", p.life) + "_A" + std::to_string(p.A) + "_B" + std::to_string(p.B) + "_").c_str(), "SIGNAL");

        double M_cut_value = 0;
        if ((0 < p.life) && (p.life < 15)) M_cut_value = 0.05;
        else if ((15 <= p.life) && (p.life < 85)) M_cut_value = 0.15;
        else if (85 <= p.life) M_cut_value = 0.3;

        loader.Cut("0.5 < isSignal");
        loader.PrintInformation("========== isSignal ==========");
        loader.Cut(("(" + std::to_string(p.mass - M_cut_value) + "< extraInfo__boALP_M__bc) && (extraInfo__boALP_M__bc <" + std::to_string(p.mass + M_cut_value) + ")").c_str());
        loader.PrintInformation(("========== nominal_mass - " + std::to_string(M_cut_value) + " < M_alp < nominal_mass + " + std::to_string(M_cut_value) + " ==========").c_str());

        RooRealVar M_ALP("M_ALP", "M_ALP", p.mass - M_cut_value, p.mass + M_cut_value);
        RooRealVar weight("weight", "weight", 0.0, 1.0);
        RooDataSet dataset("dataset", "dataset", RooArgSet(M_ALP, weight), RooFit::WeightVar("weight"));

        loader.FillDataSet(&dataset, { &M_ALP }, { "extraInfo__boALP_M__bc" });

        loader.end();

        // set range
        M_ALP.setRange("full", p.mass - M_cut_value, p.mass + M_cut_value);
        M_ALP.setRange("peak", p.mass - dataset.sigma(M_ALP) * 0.5, p.mass + dataset.sigma(M_ALP) * 0.5);


        // M_ALP fit
        RooDataSet* dataset_M = (RooDataSet*)dataset.reduce(RooArgSet(M_ALP));
        RooRealVar mean_M("mean_M", "mean_M", p.mass, p.mass - 0.03, p.mass + 0.03);
        RooRealVar sigma_left_M("sigma_left_M", "sigma_left_M", dataset.sigma(M_ALP), dataset.sigma(M_ALP) * 0.7, dataset.sigma(M_ALP) * 1.3);
        RooRealVar sigma_right_M("sigma_right_M", "sigma_right_M", dataset.sigma(M_ALP), dataset.sigma(M_ALP) * 0.7, dataset.sigma(M_ALP) * 1.3);

        RooBifurGauss bifurcated_M("bifurcated_M", "bifurcated_M", M_ALP, mean_M, sigma_left_M, sigma_right_M);

        RooFitResult* result_M = bifurcated_M.fitTo(*dataset_M, RooFit::Save(), RooFit::Strategy(2), RooFit::SumW2Error(true), RooFit::Range("peak"));
        RooArgSet fitargs_M = result_M->floatParsFinal();
        TIterator* iter_M(fitargs_M.createIterator());
        double mean_M_fit;
        double mean_M_fit_error;
        double sigma_left_M_fit;
        double sigma_left_M_fit_error;
        double sigma_right_M_fit;
        double sigma_right_M_fit_error;
        for (TObject* a = iter_M->Next(); a != 0; a = iter_M->Next()) {
            RooRealVar* rrv = dynamic_cast<RooRealVar*>(a);
            std::string name = rrv->GetName();
            if (name == std::string("mean_M")) {
                mean_M_fit = rrv->getVal();
                mean_M_fit_error = rrv->getError();
            }
            else if (name == std::string("sigma_left_M")) {
                sigma_left_M_fit = rrv->getVal();
                sigma_left_M_fit_error = rrv->getError();
            }
            else if (name == std::string("sigma_right_M")) {
                sigma_right_M_fit = rrv->getVal();
                sigma_right_M_fit_error = rrv->getError();
            }
        }

        // plot M fit
        RooPlot* M_ALP_frame = M_ALP.frame(RooFit::Bins(200), RooFit::Title(" "));
        dataset_M->plotOn(M_ALP_frame, RooFit::DataError(RooAbsData::SumW2), RooFit::Name("signal MC"), RooFit::Range("full"));
        bifurcated_M.plotOn(M_ALP_frame, RooFit::LineColor(kRed), RooFit::LineStyle(kDashed), RooFit::Range("full"), RooFit::NormRange("peak"));
        bifurcated_M.plotOn(M_ALP_frame, RooFit::LineColor(kBlue), RooFit::LineStyle(kSolid), RooFit::Range("peak"), RooFit::NormRange("peak"), RooFit::Name("BifurGauss"));

        RooHist* pull_M = M_ALP_frame->pullHist("signal MC", "BifurGauss");
        RooPlot* M_ALP_pull_frame = M_ALP.frame(RooFit::Title(""));
        M_ALP_pull_frame->addPlotable(pull_M, "P");

        TCanvas* c_M = new TCanvas("canvas_M_fit", "canvas_M_fit", 800, 800);

        c_M->cd();
        TPad* pad1_M = new TPad("pad1_M", "pad1_M", 0.0, 0.3, 1.0, 1.0);
        pad1_M->SetBottomMargin(0.05); pad1_M->SetLeftMargin(0.15); pad1_M->SetGridx(); pad1_M->Draw(); pad1_M->cd();
        M_ALP_frame->GetXaxis()->SetLabelSize(0); M_ALP_frame->GetXaxis()->SetTitleSize(0);
        M_ALP_frame->Draw();
        TLegend* legend_M = new TLegend(0.2, 0.75, 0.45, 0.85);
        legend_M->AddEntry("signal MC", "signal MC", "lpe");
        legend_M->AddEntry("BifurGauss", "BifurGauss", "l");
        legend_M->SetFillStyle(0); legend_M->SetLineWidth(0);
        legend_M->Draw();
        TLatex latex_M;
        latex_M.SetNDC();
        latex_M.SetTextSize(0.04);
        latex_M.DrawLatex(0.2, 0.7, ("#mu = " + toStringWithPrecision(mean_M_fit, 4) + " #pm " + toStringWithPrecision(mean_M_fit_error, 4) + " [GeV]").c_str());
        latex_M.DrawLatex(0.2, 0.6, ("#delta^{left}_{Gauss} = " + toStringWithPrecision(sigma_left_M_fit * 1000.0, 3) + " #pm " + toStringWithPrecision(sigma_left_M_fit_error * 1000.0, 3) + " [MeV]").c_str());
        latex_M.DrawLatex(0.2, 0.5, ("#delta^{right}_{Gauss} = " + toStringWithPrecision(sigma_right_M_fit * 1000.0, 3) + " #pm " + toStringWithPrecision(sigma_right_M_fit_error * 1000.0, 3) + " [MeV]").c_str());

        c_M->cd();
        TPad* pad2_M = new TPad("pad2_M", "pad2_M", 0.0, 0.0, 1, 0.3);
        pad2_M->SetTopMargin(0.05); pad2_M->SetBottomMargin(0.3); pad2_M->SetLeftMargin(0.15); pad2_M->SetGridx(); pad2_M->Draw(); pad2_M->cd();
        M_ALP_pull_frame->GetXaxis()->SetLabelSize(0.1); M_ALP_pull_frame->GetXaxis()->SetTitleSize(0.1); M_ALP_pull_frame->GetYaxis()->SetTitleOffset(0.4);
        M_ALP_pull_frame->GetYaxis()->SetLabelSize(0.1); M_ALP_pull_frame->GetYaxis()->SetTitleSize(0.1); M_ALP_pull_frame->GetYaxis()->SetTitle("pull"); M_ALP_pull_frame->SetTitle("");
        M_ALP_pull_frame->Draw();

        c_M->SetBottomMargin(0.0);
        c_M->SaveAs((std::string(argv[2]) + "/" + "alpha_mass" + std::to_string(p.mass) + "_life" + std::to_string(p.life) + "_A" + std::to_string(p.A) + "_B" + std::to_string(p.B) + "_M_fit.png").c_str());
        delete c_M;

        // save result
        fprintf(fp, "%lf,%lf,%d,%d,%lf,%lf,%lf,%d\n", p.mass, p.life, p.A, p.B, mean_M.getVal(), sigma_left_M.getVal(), sigma_right_M.getVal(), result_M->status());
    }

    fclose(fp);

    return 0;
}