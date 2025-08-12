#ifndef FITTER_H
#define FITTER_H

#include <vector>
#include <string>
#include <random>

#include <RooWorkspace.h>
#include <RooDataSet.h>
#include <RooAbsReal.h>
#include <RooArgSet.h>
#include <RooMinimizer.h>
#include <TMath.h>
#include <RooSimultaneous.h>
#include <THStack.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TStyle.h>
#include <TColor.h>
#include <TLegend.h>
#include <TLine.h>
#include <RooCategory.h>
#include <TPaveText.h>

#include <RooStats/ModelConfig.h>
#include <RooStats/RooStatsUtils.h>

std::random_device rd;
std::default_random_engine generator(rd());

std::vector<std::string> scaleFactors_pdf_names = {
    "signal_Belle_II_Belle_II_scaleFactors",
    "bkg_Belle_II_Belle_II_scaleFactors"
};

std::vector<std::string> shapes_pdf_names = {
    "signal_Belle_II_Belle_II_shapes",
    "bkg_Belle_II_Belle_II_shapes"
};

RooFitResult* MyMinimizeNLL(RooWorkspace* w, RooDataSet* data, RooAbsReal** nll, double tolerance = -1.0, bool Minos = true) {
    // what we have done
    w->loadSnapshot("ParamValues");
    RooStats::ModelConfig* mc = (RooStats::ModelConfig*)w->obj("ModelConfig"); // Get model manually
    RooSimultaneous* model = (RooSimultaneous*)mc->GetPdf();

    // get nll
    RooArgSet* allParams = model->getParameters(*data);
    RooStats::RemoveConstantParameters(allParams);
    RooArgSet fGlobalObs = *mc->GetGlobalObservables();
    RooArgSet fConditionalObs;
    Bool_t fLOffset = RooStats::IsNLLOffset();
    //std::string fLOffset = "bin";
    (*nll) = model->createNLL(*data, RooFit::CloneData(kFALSE), RooFit::Constrain(*allParams), RooFit::GlobalObservables(fGlobalObs), RooFit::ConditionalObservables(fConditionalObs), RooFit::Offset(fLOffset));

    // minimizer option
    TString fMinimizer = ::ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str();
    TString minimizer = fMinimizer;

    TString algorithm = ::ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo();

    Int_t fStrategy = ::ROOT::Math::MinimizerOptions::DefaultStrategy();

    Double_t fTolerance;
    if (tolerance < 0) fTolerance = TMath::Max(1., ::ROOT::Math::MinimizerOptions::DefaultTolerance());
    else fTolerance = tolerance;

    Int_t fPrintLevel = ::ROOT::Math::MinimizerOptions::DefaultPrintLevel();
    //LM: RooMinimizer.setPrintLevel has +1 offset - so subtract  here -1 + an extra -1
    int level = (fPrintLevel == 0) ? -1 : fPrintLevel - 2;


    // follow what ProfileLikelihoodTestStat.cxx does
    const auto& config = RooStats::GetGlobalRooStatsConfig();
    RooMinimizer minim(*(*nll));
    minim.setStrategy(fStrategy);
    minim.setEvalErrorWall(config.useEvalErrorWall);
    minim.setEps(fTolerance);
    minim.setPrintLevel(level);
    // this causes a memory leak
    minim.optimizeConst(2);
    minim.migrad();
    if (Minos) minim.minos(RooArgSet(*w->var("mu")));

    // fit!
    int status;
    status = minim.minimize(minimizer, algorithm);

    return minim.save();
}

void GetPlotTemplate(RooWorkspace* w, RooDataSet* data = nullptr, const char* plot_name = "hist_data_plot.png") {

    // get observable
    RooRealVar* x_val = w->var("obs_x_Belle_II");
    RooAbsBinning const& binning = x_val->getBinning();
    const double oldVal = x_val->getVal();

    // define stack and histogram
    THStack* Stack = new THStack("Stack", ";bin index;Events");
    TH1D* SIGNAL_hist = new TH1D("SIGNAL", ";bin index;Events", binning.numBins(), 0.5, binning.numBins() + 0.5);
    TH1D* bkg_hist = new TH1D("bkg", ";bin index;Events", binning.numBins(), 0.5, binning.numBins() + 0.5);
    TH1D* all_hist = new TH1D("all", ";bin index;Events", binning.numBins(), 0.5, binning.numBins() + 0.5);
    TH1D* data_hist = nullptr;
    if (data != nullptr) {
        data_hist = new TH1D("data", ";bin index;Events", binning.numBins(), 0.5, binning.numBins() + 0.5);
        data_hist->SetBinErrorOption(TH1::EBinErrorOpt::kPoisson);
    }
    TH1D* Ratio_hist = new TH1D("Ratio", ";bin index;data/MC", binning.numBins(), 0.5, binning.numBins() + 0.5);

    // fill histogram
    for (int i = 0; i < scaleFactors_pdf_names.size(); i++) {

        TH1D* temp_hist;
        if (std::strstr(scaleFactors_pdf_names.at(i).c_str(), "signal") != nullptr) temp_hist = SIGNAL_hist;
        else if (std::strstr(scaleFactors_pdf_names.at(i).c_str(), "bkg") != nullptr) temp_hist = bkg_hist;
        else {
            printf("[ERROR] unexpected sample type!\n");
            exit(1);
        }

        for (std::size_t iBin = 0; iBin < binning.numBins(); ++iBin) {
            double binCenter = binning.binCenter(iBin);
            double binWidth = binning.binWidth(iBin);

            *x_val = binCenter; // set x value

            RooAbsReal* temp_func_scaleFactors = w->function(scaleFactors_pdf_names.at(i).c_str());
            RooAbsReal* temp_func_shapes = w->function(shapes_pdf_names.at(i).c_str());
            if ((temp_func_scaleFactors == nullptr) || (temp_func_shapes == nullptr)) {
                printf("[WARNING] cannot find %s or %s. Just skip.\n", scaleFactors_pdf_names.at(i).c_str(), shapes_pdf_names.at(i).c_str());
                break;
            }

            double Nevt = (temp_func_scaleFactors->getValV() * temp_func_shapes->getValV());

            temp_hist->SetBinContent(iBin + 1, temp_hist->GetBinContent(iBin + 1) + Nevt);
            all_hist->Fill(iBin + 1, Nevt);
            all_hist->SetBinError(iBin + 1, 0);
        }

        *x_val = oldVal;

    }

    // Fill data
    if (data != nullptr) {
        for (int i = 0; i < binning.numBins(); i++) {
            const RooArgSet* argSet = data->get(i);
            if (!argSet) data_hist->SetBinContent(i + 1, 0.0);
            else data_hist->SetBinContent(i + 1, data->weight());
        }
    }

    // fill stack
    Stack->Add(bkg_hist);
    Stack->Add(SIGNAL_hist);

    // fill ratio
    Ratio_hist->SetLineColor(kBlack); Ratio_hist->SetMarkerStyle(21); Ratio_hist->Sumw2(); Ratio_hist->SetStats(0);
    Ratio_hist->Divide(data_hist, all_hist);

    // draw plot
    TCanvas* c_temp = new TCanvas("c", "", 800, 800); c_temp->cd();

    TPad* pad1 = new TPad("pad1", "pad1", 0.0, 0.3, 1.0, 1.0);
    pad1->SetBottomMargin(0.02); pad1->SetLeftMargin(0.15);
    pad1->Draw(); pad1->cd();

    gStyle->SetPalette(kPastel);

    Float_t ymax_1 = Stack->GetMaximum();
    Float_t ymax_2 = data_hist->GetMaximum();
    double real_max = 0;
    if (ymax_1 > ymax_2) real_max = ymax_1;
    else real_max = ymax_2;

    Stack->SetMaximum(real_max * 1.3);

    Stack->Draw("pfc Hist"); Stack->GetXaxis()->SetLabelSize(0.0); Stack->GetXaxis()->SetTitleSize(0.0);
    if (data != nullptr) {
        data_hist->SetLineWidth(2);
        data_hist->SetLineColor(kBlack);
        data_hist->SetMarkerStyle(8);
        data_hist->Draw("SAME eP EX0");
    }
    TLegend* legend = pad1->BuildLegend(0.95, 0.9, 0.75, 0.7);
    legend->SetFillStyle(0); legend->SetLineWidth(0);

    // write Belle text
    TPaveText* pt_belle = new TPaveText(0.16, 0.83, 0.4, 0.88, "NDC NB");
    pt_belle->SetTextSize(0.035); pt_belle->SetFillStyle(0); pt_belle->SetLineWidth(0); pt_belle->SetTextAlign(11); pt_belle->AddText("Belle II"); pt_belle->Draw();
    TPaveText* pt_lumi = new TPaveText(0.16, 0.76, 0.4, 0.81, "NDC NB");
    pt_lumi->SetTextSize(0.035); pt_lumi->SetFillStyle(0); pt_lumi->SetLineWidth(0); pt_lumi->SetTextAlign(11); pt_lumi->AddText("#int L dt = 365.4 fb^{-1}"); pt_lumi->Draw();

    c_temp->cd();
    TPad* pad2 = new TPad("pad2", "pad2", 0.0, 0.0, 1, 0.3); pad2->SetBottomMargin(0.2); pad2->SetLeftMargin(0.15); pad2->SetTopMargin(0.05); pad2->Draw(); pad2->cd();
    Ratio_hist->SetMinimum(0.5); Ratio_hist->SetMaximum(1.5); Ratio_hist->SetLineWidth(2);
    Ratio_hist->GetYaxis()->SetTitleSize(0.08); Ratio_hist->GetYaxis()->SetTitleOffset(0.5); Ratio_hist->GetYaxis()->SetLabelSize(0.08);
    Ratio_hist->GetXaxis()->SetLabelSize(0.08); Ratio_hist->GetXaxis()->SetTitleSize(0.08);
    Ratio_hist->Draw("E X0 P");
    TLine* line = new TLine(Ratio_hist->GetXaxis()->GetXmin(), 1.0, Ratio_hist->GetXaxis()->GetXmax(), 1.0);
    line->SetLineColor(kRed);
    line->SetLineStyle(1); line->SetLineWidth(3);
    line->Draw();

    c_temp->SetBottomMargin(0.0);
    c_temp->SaveAs(plot_name);

    // delete
    delete c_temp;

    delete Stack;
    delete SIGNAL_hist;
    delete bkg_hist;
    delete all_hist;
    if (data != nullptr) {
        delete data_hist;
    }
    delete Ratio_hist;

}

void PrintNevtFile(RooWorkspace* w, const char* filename = nullptr) {

    // Nevts for each samples
    std::vector<double> Signal_Nevts; // Nevt for signal
    std::vector<double> BKG_Nevts; // Nevt for background

    // get observable
    RooRealVar* x_val = w->var("obs_x_Belle_II");

    // count Nevt
    {
        RooAbsBinning const& binning = x_val->getBinning();
        const double oldVal = x_val->getVal();

        for (std::size_t iBin = 0; iBin < binning.numBins(); ++iBin) {
            double binCenter = binning.binCenter(iBin);
            double binWidth = binning.binWidth(iBin);

            *x_val = binCenter; // set x value

            for (unsigned int j = 0; j < scaleFactors_pdf_names.size(); j++) {
                RooAbsReal* temp_func_scaleFactors = w->function(scaleFactors_pdf_names.at(j).c_str());
                RooAbsReal* temp_func_shapes = w->function(shapes_pdf_names.at(j).c_str());

                double Nevt = temp_func_scaleFactors->getValV() * temp_func_shapes->getValV();
                if (std::strstr(scaleFactors_pdf_names.at(j).c_str(), "signal") != nullptr) Signal_Nevts.push_back(Nevt);
                else if (std::strstr(scaleFactors_pdf_names.at(j).c_str(), "bkg") != nullptr) BKG_Nevts.push_back(Nevt);
                else {
                    printf("[ERROR] unexpected sample\n");
                    exit(1);
                }

                if ((temp_func_scaleFactors->getValV() * temp_func_shapes->getValV()) < 0) {
                    printf("[ERROR] negative count!\n");
                    // exit(1);
                }
            }

        }

        *x_val = oldVal;
    }

    if (filename != nullptr) {
        FILE* fp;
        fp = fopen(filename, "w");

        fprintf(fp, "Signal:\n");
        for (int i = 0; i < Signal_Nevts.size(); i++) fprintf(fp, "%lf ", Signal_Nevts.at(i));
        fprintf(fp, "\n");

        fprintf(fp, "BKG:\n");
        for (int i = 0; i < BKG_Nevts.size(); i++) fprintf(fp, "%lf ", BKG_Nevts.at(i));
        fprintf(fp, "\n");

        fclose(fp);
    }
    else {
        FILE* fp = stdout;

        fprintf(fp, "Signal:\n");
        for (int i = 0; i < Signal_Nevts.size(); i++) fprintf(fp, "%lf ", Signal_Nevts.at(i));
        fprintf(fp, "\n");

        fprintf(fp, "BKG:\n");
        for (int i = 0; i < BKG_Nevts.size(); i++) fprintf(fp, "%lf ", BKG_Nevts.at(i));
        fprintf(fp, "\n");
    }

}

void PrintNevtFile(RooWorkspace* w, RooDataSet* data, const char* filename = nullptr) {

    // Nevts for each samples
    std::vector<double> data_Nevts; // Nevt for data

    // get observable
    RooRealVar* x_val = w->var("obs_x_Belle_II");
    RooAbsBinning const& binning = x_val->getBinning();

    // count Nevt
    for (int i = 0; i < binning.numBins(); i++) {
        const RooArgSet* argSet = data->get(i);
        if (!argSet) data_Nevts.push_back(0.0);
        else data_Nevts.push_back(data->weight());
    }

    if (filename != nullptr) {
        FILE* fp;
        fp = fopen(filename, "w");

        fprintf(fp, "data:\n");
        for (int i = 0; i < data_Nevts.size(); i++) fprintf(fp, "%lf ", data_Nevts.at(i));
        fprintf(fp, "\n");

        fclose(fp);
    }
    else {
        FILE* fp = stdout;

        fprintf(fp, "data:\n");
        for (int i = 0; i < data_Nevts.size(); i++) fprintf(fp, "%lf ", data_Nevts.at(i));
        fprintf(fp, "\n");
    }

}

RooDataSet* MyGenerate(RooWorkspace* w, std::vector<double> Nevts, bool extended) {

    RooStats::ModelConfig* mc = (RooStats::ModelConfig*)w->obj("ModelConfig"); // Get model manually
    RooSimultaneous* model = (RooSimultaneous*)mc->GetPdf();

    // get variables and weight
    RooRealVar* x_val = w->var("obs_x_Belle_II");
    RooAbsBinning const& binning = x_val->getBinning();
    const double oldVal = x_val->getVal();

    RooCategory* channelCat = (RooCategory*)(&model->indexCat());
    RooRealVar* weight_ = new RooRealVar("weight_", "", 0.0, 1000.0);

    // define data
    RooDataSet* genData = new RooDataSet("hmaster", "hmaster", RooArgSet(*x_val, *channelCat, *weight_), weight_->GetName());

    for (int i = 0; i < binning.numBins(); i++) {
        x_val->setVal(0.5 + i);
        channelCat->setLabel("Belle_II");

        // generate
        if (Nevts.at(i) > 0.00001) {
            if (extended) {
                std::poisson_distribution<int> distribution((int)floor(Nevts.at(i) + 0.5));
                int Nentry_with_fluctuation = distribution(generator);
                genData->add(RooArgSet(*x_val, *channelCat), Nentry_with_fluctuation);
            }
            else {
                genData->add(RooArgSet(*x_val, *channelCat), (int)floor(Nevts.at(i) + 0.5));
            }
        }
        else { // no event. Maybe because of partial unblind. Just set 0
            genData->add(RooArgSet(*x_val, *channelCat), 0);
        }
    }

    delete weight_;

    return genData;
}


#endif 
