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
    * argv[3]: mass
    * argv[4]: lifetime
    * argv[5]: A constant
    * argv[6]: B constant
    */

    double mass = std::stod(argv[3]);
    double life = std::stod(argv[4]);
    int A = std::stoi(argv[5]);
    int B = std::stoi(argv[6]);

    Params p { mass, life, A, B };

    std::set<Params> parameters; // currently single-point execution, kept as set for future grid scan
    parameters.insert(p);

    for (const auto& p : parameters) {
        ObtainWeight = MyScaleFunction;

        Loader loader("tau_lfv");

        loader.Load(argv[1], ("alpha_mass" + std::format("{:g}", p.mass) + "_life" + std::format("{:g}", p.life) + "_A" + std::to_string(p.A) + "_B" + std::to_string(p.B) + "_").c_str(), "SIGNAL");

        double M_left_cut_value = 0;
        double M_right_cut_value = 0;
        if ((0 < p.life) && (p.life < 0.7)) {
            M_left_cut_value = 0.02;
            M_right_cut_value = 0.02;
        }
        else if ((0.7 <= p.life) && (p.life < 7)) {
            M_left_cut_value = 0.03;
            M_right_cut_value = 0.03;
        }
        else if ((7 <= p.life) && (p.life < 70)) {
            M_left_cut_value = 0.1;
            M_right_cut_value = 0.1;
        }
        else if (70 <= p.life) {
            M_left_cut_value = 0.25;
            M_right_cut_value = 0.2;
        }

        loader.Cut("0.5 < isSignal");
        loader.PrintInformation("========== isSignal ==========");
        loader.Cut(("(" + std::to_string(p.mass - M_left_cut_value * 1.3) + "< extraInfo__boALP_M__bc) && (extraInfo__boALP_M__bc <" + std::to_string(p.mass + M_right_cut_value * 1.3) + ")").c_str());
        loader.PrintInformation(("========== nominal_mass - " + std::to_string(M_left_cut_value * 1.3) + " < M_alp < nominal_mass + " + std::to_string(M_right_cut_value * 1.3) + " ==========").c_str());

        RooRealVar M_ALP("M_ALP", "M_ALP", p.mass - M_left_cut_value * 1.3, p.mass + M_right_cut_value * 1.3);
        RooRealVar weight("weight", "weight", 0.0, 1.0);
        RooDataSet dataset("dataset", "dataset", RooArgSet(M_ALP, weight), RooFit::WeightVar("weight"));

        loader.FillDataSet(&dataset, { &M_ALP }, { "extraInfo__boALP_M__bc" });

        loader.end();

        // set range
        M_ALP.setRange("full", p.mass - M_left_cut_value * 1.3, p.mass + M_right_cut_value * 1.3);


        // plot M fit
        RooDataSet* dataset_M = (RooDataSet*)dataset.reduce(RooArgSet(M_ALP));
        RooPlot* M_ALP_frame = M_ALP.frame(RooFit::Bins(200), RooFit::Title(" "));
        dataset_M->plotOn(M_ALP_frame, RooFit::DataError(RooAbsData::SumW2), RooFit::Name("signal MC"), RooFit::Range("full"));

        TCanvas* c_M = new TCanvas("canvas_M_fit", "canvas_M_fit", 800, 560);

        c_M->cd();
        TPad* pad1_M = new TPad("pad1_M", "pad1_M", 0.0, 0.0, 1.0, 1.0);
        pad1_M->SetBottomMargin(0.1); pad1_M->SetLeftMargin(0.15); pad1_M->SetGridx(); pad1_M->Draw(); pad1_M->cd();
        M_ALP_frame->GetXaxis()->SetLabelSize(0.02); M_ALP_frame->GetXaxis()->SetTitleSize(0.02);
        M_ALP_frame->Draw();
        TLegend* legend_M = new TLegend(0.2, 0.75, 0.40, 0.82);
        legend_M->AddEntry("signal MC", "signal MC", "lpe");
        legend_M->SetFillStyle(0); legend_M->SetLineWidth(0);
        legend_M->Draw();
        TLatex latex_M;
        latex_M.SetNDC();
        latex_M.SetTextSize(0.04);
        latex_M.DrawLatex(0.2, 0.7, ("m_{#alpha} = " + std::format("{:g}", p.mass) + " [GeV]").c_str());
        latex_M.DrawLatex(0.2, 0.6, ("c*#tau = " + std::format("{:g}", p.life) + " [mm]").c_str());
        latex_M.DrawLatex(0.2, 0.5, ("A = " + std::to_string(p.A)).c_str());
        latex_M.DrawLatex(0.2, 0.4, ("B = " + std::to_string(p.B)).c_str());

        c_M->SetBottomMargin(0.0);
        c_M->SaveAs((std::string(argv[2]) + "/" + "alpha_mass" + std::format("{:g}", p.mass) + "_life" + std::format("{:g}", p.life) + "_A" + std::to_string(p.A) + "_B" + std::to_string(p.B) + "_M_fit.png").c_str());
        delete c_M;

    }

    return 0;
}