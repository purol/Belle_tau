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

    Params p{ mass, life, A, B };

    std::set<Params> parameters; // currently single-point execution, kept as set for future grid scan
    parameters.insert(p);

    for (const auto& p : parameters) {
        ObtainWeight = MyScaleFunction;

        Loader loader("tau_lfv");

        loader.Load(argv[1], ("alpha_mass" + std::format("{:g}", p.mass) + "_life" + std::format("{:g}", p.life) + "_A" + std::to_string(p.A) + "_B" + std::to_string(p.B) + "_").c_str(), "SIGNAL");

        // set tag for file name
        std::string tag = "alpha_mass" + std::format("{:g}", mass) + "_life" + std::format("{:g}", life) + "_A" + std::to_string(A) + "_B" + std::to_string(B);

        double M_left_cut_value = 0;
        double M_right_cut_value = 0;
        double M_inv_full_left = 0;
        double M_inv_full_right = 0;
        double M_inv_peak_left = 0;
        double M_inv_peak_right = 0;
        double deltaE_full_left = 0;
        double deltaE_full_right = 0;
        double deltaE_peak_left = 0;
        double deltaE_peak_right = 0;
        if ((0 < p.life) && (p.life < 0.7)) {
            M_left_cut_value = 0.02;
            M_right_cut_value = 0.02;
            M_inv_full_left = 1.71;
            M_inv_full_right = 1.82;
            M_inv_peak_left = 1.77;
            M_inv_peak_right = 1.785;
            deltaE_full_left = -0.3;
            deltaE_full_right = 0.15;
            deltaE_peak_left = -0.02;
            deltaE_peak_right = 0.02;
        }
        else if ((0.7 <= p.life) && (p.life < 7)) {
            M_left_cut_value = 0.03;
            M_right_cut_value = 0.03;
            M_inv_full_left = 1.70;
            M_inv_full_right = 1.83;
            M_inv_peak_left = 1.765;
            M_inv_peak_right = 1.790;
            deltaE_full_left = -0.3;
            deltaE_full_right = 0.15;
            deltaE_peak_left = -0.02;
            deltaE_peak_right = 0.02;
        }
        else if ((7 <= p.life) && (p.life < 70)) {
            M_left_cut_value = 0.1;
            M_right_cut_value = 0.1;
            M_inv_full_left = 1.55;
            M_inv_full_right = 1.90;
            M_inv_peak_left = 1.75;
            M_inv_peak_right = 1.795;
            deltaE_full_left = -0.40;
            deltaE_full_right = 0.25;
            deltaE_peak_left = -0.025;
            deltaE_peak_right = 0.025;
        }
        else if (70 <= p.life) {
            M_left_cut_value = 0.25;
            M_right_cut_value = 0.2;
            M_inv_full_left = 1.4;
            M_inv_full_right = 2.1;
            M_inv_peak_left = 1.75;
            M_inv_peak_right = 1.80;
            deltaE_full_left = -0.50;
            deltaE_full_right = 0.30;
            deltaE_peak_left = -0.03;
            deltaE_peak_right = 0.03;
        }

        loader.Cut(("(" + std::to_string(p.mass - M_left_cut_value) + "< extraInfo__boALP_M__bc) && (extraInfo__boALP_M__bc <" + std::to_string(p.mass + M_right_cut_value) + ")").c_str());
        loader.PrintInformation(("========== nominal_mass - " + std::to_string(M_left_cut_value) + " < M_alp < nominal_mass + " + std::to_string(M_right_cut_value) + " ==========").c_str());
        loader.Cut(("(" + std::to_string(deltaE_full_left) + " < deltaE) && (deltaE < " + std::to_string(deltaE_full_right) + ")").c_str());
        loader.PrintInformation(("========== " + std::to_string(deltaE_full_left) + " < deltaE < " + std::to_string(deltaE_full_right) + " ==========").c_str());
        loader.Cut(("(" + std::to_string(M_inv_full_left) + "< M) && (M < " + std::to_string(M_inv_full_right) + ")").c_str());
        loader.PrintInformation(("========== " + std::to_string(M_inv_full_left) + " < M < " + std::to_string(M_inv_full_right) + " ==========").c_str());
        loader.Cut("0.5 < isSignal");
        loader.PrintInformation("========== isSignal ==========");

        RooRealVar M_inv("M_inv", "M_inv", M_inv_full_left, M_inv_full_right);
        RooRealVar deltaE("deltaE", "deltaE", deltaE_full_left, deltaE_full_right);
        RooRealVar weight("weight", "weight", 0.0, 1.0);
        RooDataSet dataset("dataset", "dataset", RooArgSet(M_inv, deltaE, weight), RooFit::WeightVar("weight"));

        // set range
        M_inv.setRange("full", M_inv_full_left, M_inv_full_right);
        M_inv.setRange("peak", M_inv_peak_left, M_inv_peak_right);
        deltaE.setRange("full", deltaE_full_left, deltaE_full_right);
        deltaE.setRange("peak", deltaE_peak_left, deltaE_peak_right);

        loader.FillDataSet(&dataset, { &M_inv, &deltaE }, { "M", "deltaE" });

        TProfile* deltaE_M_profile = new TProfile("hprof", ";#Delta E_{3#mu} [GeV];M_{3#mu} [GeV]", 100, deltaE_full_left, deltaE_full_right, M_inv_full_left, M_inv_full_right);
        loader.FillTProfile(deltaE_M_profile, "deltaE", "M");

        loader.end();

        // M fit
        RooDataSet* dataset_M = (RooDataSet*)dataset.reduce(RooArgSet(M_inv));
        RooRealVar mean_M("mean_M", "mean_M", 1.777, 1.767, 1.787);
        RooRealVar sigma_left_M("sigma_left_M", "sigma_left_M", 0.0048, 0.0038, 0.03);
        RooRealVar sigma_right_M("sigma_right_M", "sigma_right_M", 0.0048, 0.0038, 0.03);

        RooBifurGauss bifurcated_M("bifurcated_M", "bifurcated_M", M_inv, mean_M, sigma_left_M, sigma_right_M);

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
        RooPlot* M_inv_frame = M_inv.frame(RooFit::Bins(200), RooFit::Title(" "));
        dataset_M->plotOn(M_inv_frame, RooFit::DataError(RooAbsData::SumW2), RooFit::Name("signal MC"));
        bifurcated_M.plotOn(M_inv_frame, RooFit::LineColor(kRed), RooFit::LineStyle(kDashed), RooFit::Range("full"), RooFit::NormRange("peak"));
        bifurcated_M.plotOn(M_inv_frame, RooFit::LineColor(kBlue), RooFit::LineStyle(kSolid), RooFit::Range("peak"), RooFit::NormRange("peak"), RooFit::Name("BifurGauss"));

        RooHist* pull_M = M_inv_frame->pullHist("signal MC", "BifurGauss");
        RooPlot* M_inv_pull_frame = M_inv.frame(RooFit::Title(""));
        M_inv_pull_frame->addPlotable(pull_M, "P");

        TCanvas* c_M = new TCanvas("canvas_M_fit", "canvas_M_fit", 800, 800);

        c_M->cd();
        TPad* pad1_M = new TPad("pad1_M", "pad1_M", 0.0, 0.3, 1.0, 1.0);
        pad1_M->SetBottomMargin(0.05); pad1_M->SetLeftMargin(0.15); pad1_M->SetGridx(); pad1_M->Draw(); pad1_M->cd();
        M_inv_frame->GetXaxis()->SetLabelSize(0); M_inv_frame->GetXaxis()->SetTitleSize(0);
        M_inv_frame->Draw();
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
        M_inv_pull_frame->GetXaxis()->SetLabelSize(0.1); M_inv_pull_frame->GetXaxis()->SetTitleSize(0.1); M_inv_pull_frame->GetYaxis()->SetTitleOffset(0.4);
        M_inv_pull_frame->GetYaxis()->SetLabelSize(0.1); M_inv_pull_frame->GetYaxis()->SetTitleSize(0.1); M_inv_pull_frame->GetYaxis()->SetTitle("pull"); M_inv_pull_frame->SetTitle("");
        M_inv_pull_frame->Draw();

        c_M->SetBottomMargin(0.0);
        c_M->SaveAs((std::string(argv[2]) + "/" + tag + "_M_fit.png").c_str());
        delete c_M;





        // deltaE fit
        RooDataSet* dataset_deltaE = (RooDataSet*)dataset.reduce(RooArgSet(deltaE));
        RooRealVar mean_deltaE("mean_deltaE", "mean_deltaE", 0.0, -0.1, 0.1);
        RooRealVar sigma_left_deltaE("sigma_left_deltaE", "sigma_left_deltaE", 0.014, 0.008, 0.025);
        RooRealVar sigma_right_deltaE("sigma_right_deltaE", "sigma_right_deltaE", 0.014, 0.008, 0.025);

        RooBifurGauss bifurcated_deltaE("bifurcated_deltaE", "bifurcated_deltaE", deltaE, mean_deltaE, sigma_left_deltaE, sigma_right_deltaE);

        RooFitResult* result_deltaE = bifurcated_deltaE.fitTo(*dataset_deltaE, RooFit::Save(), RooFit::Strategy(2), RooFit::SumW2Error(true), RooFit::Range("peak"));
        RooArgSet fitargs_deltaE = result_deltaE->floatParsFinal();
        TIterator* iter_deltaE(fitargs_deltaE.createIterator());
        double mean_deltaE_fit;
        double mean_deltaE_fit_error;
        double sigma_left_deltaE_fit;
        double sigma_left_deltaE_fit_error;
        double sigma_right_deltaE_fit;
        double sigma_right_deltaE_fit_error;
        for (TObject* a = iter_deltaE->Next(); a != 0; a = iter_deltaE->Next()) {
            RooRealVar* rrv = dynamic_cast<RooRealVar*>(a);
            std::string name = rrv->GetName();
            if (name == std::string("mean_deltaE")) {
                mean_deltaE_fit = rrv->getVal();
                mean_deltaE_fit_error = rrv->getError();
            }
            else if (name == std::string("sigma_left_deltaE")) {
                sigma_left_deltaE_fit = rrv->getVal();
                sigma_left_deltaE_fit_error = rrv->getError();
            }
            else if (name == std::string("sigma_right_deltaE")) {
                sigma_right_deltaE_fit = rrv->getVal();
                sigma_right_deltaE_fit_error = rrv->getError();
            }
        }

        // plot deltaE fit
        RooPlot* deltaE_frame = deltaE.frame(RooFit::Bins(200), RooFit::Title(" "));
        dataset_deltaE->plotOn(deltaE_frame, RooFit::DataError(RooAbsData::SumW2), RooFit::Name("signal MC"));
        bifurcated_deltaE.plotOn(deltaE_frame, RooFit::LineColor(kRed), RooFit::LineStyle(kDashed), RooFit::Range("full"), RooFit::NormRange("peak"));
        bifurcated_deltaE.plotOn(deltaE_frame, RooFit::LineColor(kBlue), RooFit::LineStyle(kSolid), RooFit::Range("peak"), RooFit::NormRange("peak"), RooFit::Name("BifurGauss"));

        RooHist* pull_deltaE = deltaE_frame->pullHist("signal MC", "BifurGauss");
        RooPlot* deltaE_pull_frame = deltaE.frame(RooFit::Title(""));
        deltaE_pull_frame->addPlotable(pull_deltaE, "P");

        TCanvas* c_deltaE = new TCanvas("canvas_deltaE_fit", "canvas_deltaE_fit", 800, 800);

        c_deltaE->cd();
        TPad* pad1_deltaE = new TPad("pad1_deltaE", "pad1_deltaE", 0.0, 0.3, 1.0, 1.0);
        pad1_deltaE->SetBottomMargin(0.05); pad1_deltaE->SetLeftMargin(0.15); pad1_deltaE->SetGridx(); pad1_deltaE->Draw(); pad1_deltaE->cd();
        deltaE_frame->GetXaxis()->SetLabelSize(0); deltaE_frame->GetXaxis()->SetTitleSize(0);
        deltaE_frame->Draw();
        TLegend* legend_deltaE = new TLegend(0.2, 0.75, 0.45, 0.85);
        legend_deltaE->AddEntry("signal MC", "signal MC", "lpe");
        legend_deltaE->AddEntry("BifurGauss", "BifurGauss", "l");
        legend_deltaE->SetFillStyle(0); legend_deltaE->SetLineWidth(0);
        legend_deltaE->Draw();
        TLatex latex_deltaE;
        latex_deltaE.SetNDC();
        latex_deltaE.SetTextSize(0.04);
        latex_deltaE.DrawLatex(0.2, 0.7, ("#mu = " + toStringWithPrecision(mean_deltaE_fit, 4) + " #pm " + toStringWithPrecision(mean_deltaE_fit_error, 4) + " [GeV]").c_str());
        latex_deltaE.DrawLatex(0.2, 0.6, ("#delta^{left}_{Gauss} = " + toStringWithPrecision(sigma_left_deltaE_fit * 1000.0, 3) + " #pm " + toStringWithPrecision(sigma_left_deltaE_fit_error * 1000.0, 3) + " [MeV]").c_str());
        latex_deltaE.DrawLatex(0.2, 0.5, ("#delta^{right}_{Gauss} = " + toStringWithPrecision(sigma_right_deltaE_fit * 1000.0, 3) + " #pm " + toStringWithPrecision(sigma_right_deltaE_fit_error * 1000.0, 3) + " [MeV]").c_str());

        c_deltaE->cd();
        TPad* pad2_deltaE = new TPad("pad2_deltaE", "pad2_deltaE", 0.0, 0.0, 1, 0.3);
        pad2_deltaE->SetTopMargin(0.05); pad2_deltaE->SetBottomMargin(0.3); pad2_deltaE->SetLeftMargin(0.15); pad2_deltaE->SetGridx(); pad2_deltaE->Draw(); pad2_deltaE->cd();
        deltaE_pull_frame->GetXaxis()->SetLabelSize(0.1); deltaE_pull_frame->GetXaxis()->SetTitleSize(0.1); deltaE_pull_frame->GetYaxis()->SetTitleOffset(0.4);
        deltaE_pull_frame->GetYaxis()->SetLabelSize(0.1); deltaE_pull_frame->GetYaxis()->SetTitleSize(0.1); deltaE_pull_frame->GetYaxis()->SetTitle("pull"); deltaE_pull_frame->SetTitle("");
        deltaE_pull_frame->Draw();

        c_deltaE->SaveAs((std::string(argv[2]) + "/" + tag + "_deltaE_fit.png").c_str());
        delete c_deltaE;




        // profile fit
        TF1* first_order_poly = new TF1("first_order_poly", "[0]*x+[1]", -0.01, 0.01);
        first_order_poly->SetParameter(0, 0.242);
        first_order_poly->SetParameter(1, 1.777);
        deltaE_M_profile->Fit(first_order_poly, "R");
        double p0 = first_order_poly->GetParameter(0);
        double p1 = first_order_poly->GetParameter(1);

        // plot profile
        TCanvas* c_deltaE_M = new TCanvas("canvas_deltaE_M_fit", "canvas_deltaE_M_fit", 800, 800);
        c_deltaE_M->cd(); c_deltaE_M->SetLeftMargin(0.15);
        deltaE_M_profile->SetStats(false);
        deltaE_M_profile->Draw();

        TLatex latex_deltaE_M;
        latex_deltaE_M.SetNDC();
        latex_deltaE_M.SetTextSize(0.04);
        latex_deltaE_M.DrawLatex(0.2, 0.7, ("M_{3#mu} = " + toStringWithPrecision(p0, 3) + " #times #DeltaE_{3#mu} + " + toStringWithPrecision(p1, 3)).c_str());
        latex_deltaE_M.DrawLatex(0.2, 0.6, ("#theta = " + toStringWithPrecision(std::atan(-p0), 3) + "rad").c_str());

        c_deltaE_M->SaveAs((std::string(argv[2]) + "/" + tag + "_deltaE_M_fit.png").c_str());
        delete c_deltaE_M;




        // save result
        FILE* fp = fopen((std::string(argv[2]) + "/" + tag + "_M_deltaE_result.txt").c_str(), "w");
        fprintf(fp, "%lf %lf %lf %d\n", mean_M.getVal(), sigma_left_M.getVal(), sigma_right_M.getVal(), result_M->status());
        fprintf(fp, "%lf %lf %lf %d\n", mean_deltaE.getVal(), sigma_left_deltaE.getVal(), sigma_right_deltaE.getVal(), result_deltaE->status());
        fprintf(fp, "%lf\n", std::atan(-p0));
        fclose(fp);
    }

    return 0;
}