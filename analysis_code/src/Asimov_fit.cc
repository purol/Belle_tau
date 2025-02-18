#include <string>
#include <stdio.h>

#include "Fitter.h"

#include <RooAbsReal.h>
#include <RooFitResult.h>
#include <RooDataSet.h>
#include <RooArgSet.h>
#include <TIterator.h>
#include <TObject.h>
#include <RooRealVar.h>
#include <TFile.h>

int main(int argc, char* argv[]) {

	RooStats::UseNLLOffset(true); // default off

	// read workspace
	const char* fname = "./workspace.root";
	TFile* f = TFile::Open(fname);
	RooWorkspace* w = (RooWorkspace*)f->Get("combined");

	// get data
	RooDataSet* data = (RooDataSet*)w->data("asimovData");

	// fit
	double eps = 0.1;
	RooAbsReal* nll;
	RooFitResult* fitres = MyMinimizeNLL(w, data, &nll, eps);

	// print fit result
	RooArgSet fitargs = fitres->floatParsFinal();
	TIterator* iter(fitargs.createIterator());

	for (TObject* a = iter->Next(); a != 0; a = iter->Next()) {
		RooRealVar* rrv = dynamic_cast<RooRealVar*>(a);
		std::string name = rrv->GetName();
		double val = rrv->getVal();
		double err = rrv->getError();
		double HIerr = rrv->getAsymErrorHi();
		double LOerr = rrv->getAsymErrorLo();

		printf("fit result for %s = %lf +- %lf\n", name.c_str(), val, err);
		printf("MINOS error: %lf %lf\n", HIerr, LOerr);

	}

	// draw plot
	GetPlotTemplate(w, data, "./hist_data_plot.png")

	return 0;
}
