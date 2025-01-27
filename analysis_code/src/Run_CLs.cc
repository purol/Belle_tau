#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <HypoTestInverter.h>
#include <HypoTestInverterResult.h>
#include <RooRandom.h>
#include <TFile.h>
#include <RooWorkspace.h>
#include <RooDataSet.h>
#include <RooRealVar.h>
#include <ToyMCSampler.h>
#include <TStopwatch.h>

std::random_device rd;
std::default_random_engine generator(rd());

# define NToys 10000

void GetExpectedCL(RooStats::HypoTestInverterResult* fResults, const char* mu) {
	// get CLs CLb CLs+b
	const int nEntries = fResults->ArraySize();

	double p[7];
	double q[7];
	p[0] = ROOT::Math::normal_cdf(-2);
	p[1] = ROOT::Math::normal_cdf(-1);
	p[2] = 0.5;
	p[3] = ROOT::Math::normal_cdf(1);
	p[4] = ROOT::Math::normal_cdf(2);

	int np = 0;
	for (int j = 0; j < nEntries; ++j) {
		int i = j; // i is the order index
		SamplingDistribution* s = fResults->GetExpectedPValueDist(i);
		if (!s)  continue;
		const std::vector<double>& values = s->GetSamplingDistribution();

		double* x = const_cast<double*>(&values[0]); // need to change TMath::Quantiles
		TMath::Quantiles(values.size(), 5, x, q, p, false);

		FILE* fp;
		fp = fopen(("CLs_freq_" + std::string(mu) + ".txt").c_str(), "a");
		fprintf(fp, "expected CLs median: %lf\n", q[2]);
		fprintf(fp, "expected CLs +1sigma: %lf\n", q[3] - q[2]);
		fprintf(fp, "expected CLs -1sigma: %lf\n", q[2] - q[1]);
		fprintf(fp, "expected CLs +2sigma: %lf\n", q[4] - q[2]);
		fprintf(fp, "expected CLs -2sigma: %lf\n", q[2] - q[0]);
		fclose(fp);
		if (s) delete s;
		np++;
	}
}

void GetObservedCLs(RooStats::HypoTestInverterResult* fResults, const char* mu, int type = 0) {
	// type 0: CLs
	// type 1: CLb
	// type 2: CLs+b
	if (!(type == 0) && !(type == 1) && !(type == 2)) {
		printf("[ERROR] unvalid type!\n");
		exit(1);
	}

	const int nEntries = fResults->ArraySize();

	std::vector<Double_t> xArray;
	std::vector<Double_t> yArray;
	std::vector<Double_t> yErrArray;

	for (int i = 0; i < nEntries; i++) {
		int index = i;

		double CLVal = 0.0;
		double CLErr = 0.0;
		if (type == 0) {
			CLVal = fResults->GetYValue(index);
			CLErr = fResults->GetYError(index);
		}
		else if (type == 1) {
			CLVal = fResults->CLb(index);
			CLErr = fResults->CLbError(index);
		}
		else if (type == 2) {
			CLVal = fResults->CLsplusb(index);
			CLErr = fResults->CLsplusbError(index);
		}

		if (CLVal < 0.0 || !std::isfinite(CLVal)) {
			printf("Got a confidence level of %f at x=%f (failed fit?). Skipping this point.", CLVal, fResults->GetXValue(index));
			continue;
		}

		FILE* fp;
		fp = fopen(("CLs_freq_" + std::string(mu) + ".txt").c_str(), "a");
		if (type == 0) {
			fprintf(fp, "observed CLs central value: %lf\n", CLVal);
			fprintf(fp, "observed CLs error: %lf\n", CLErr);
		}
		else if (type == 1) {
			fprintf(fp, "observed CLb central value: %lf\n", CLVal);
			fprintf(fp, "observed CLb error: %lf\n", CLErr);
		}
		else if (type == 2) {
			fprintf(fp, "observed CLs+b central value: %lf\n", CLVal);
			fprintf(fp, "observed CLs+b error: %lf\n", CLErr);
		}
		fclose(fp);

		yArray.push_back(CLVal);
		yErrArray.push_back(CLErr);
		xArray.push_back(fResults->GetXValue(index));
	}
}

int main(int argc, char* argv[]) {
	/*
	* argv[1]: input path
	* argv[2]: output path
	* argv[3]: mu value to test
	* argv[4]: index
	*/

	RooRandom::randomGenerator()->SetSeed(rd());

	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2", "Minimize"); // default: Minuit Migrad
	ROOT::Math::MinimizerOptions::SetDefaultStrategy(1); // default 1
	RooStats::UseNLLOffset(true); // default off
	double eps = 0.001;
	ROOT::Math::MinimizerOptions::SetDefaultTolerance(eps); // default 0.01. but it is better to use 0.001

	std::string fname = std::string(argv[1]) + "/workspace.root";

	TFile* f = TFile::Open(fname.c_str());

	RooWorkspace* w = (RooWorkspace*)f->Get("combined");

	// get data
	RooDataSet* data = (RooDataSet*)w->data("obsData");

	/* ======================== CLS ======================== */

	if (data->isWeighted()) {
		Info("Hypocal", "Data set is weighted, nentries = %d and sum of weights = %8.1f.\n", data->numEntries(), data->sumEntries());
	}

	RooStats::ModelConfig* sbModel = (RooStats::ModelConfig*)w->obj("ModelConfig");
	RooStats::ModelConfig* bModel = (RooStats::ModelConfig*)sbModel->Clone("BonlyModel");
	RooRealVar* poi = (RooRealVar*)bModel->GetParametersOfInterest()->first();
	poi->setVal(0);
	bModel->SetSnapshot(*poi);

	// start to do CLs method
	RooStats::FrequentistCalculator FreqCalc(*data, *bModel, *sbModel);
	RooStats::ProfileLikelihoodTestStat* plr = new RooStats::ProfileLikelihoodTestStat(*sbModel->GetPdf());
	plr->SetOneSided(true);
	plr->SetMinimizer("Minuit2");
	plr->SetStrategy(1);
	plr->SetLOffset(true);
	plr->SetTolerance(eps);

	RooStats::ToyMCSampler* toymcs = (RooStats::ToyMCSampler*)FreqCalc.GetTestStatSampler();
	toymcs->SetTestStatistic(plr);
	FreqCalc.SetToys(NToys, NToys);

	RooStats::HypoTestInverter inverter(FreqCalc);
	//inverter.SetConfidenceLevel(0.90);
	inverter.UseCLs(true);
	inverter.SetVerbose(false);
	inverter.SetFixedScan(1, std::stof(argv[3]), std::stof(argv[3])); // set number of points , xmin and xmax

	TStopwatch sw;
	sw.Start();

	RooStats::HypoTestInverterResult* result = inverter.GetInterval();

	sw.Stop();
	printf("consumed time: %lf (s)\n", sw.RealTime());

	TFile* file = new TFile((std::string(argv[2]) + "/Hypotestinverter_freq_" + std::string(argv[3]) + "_" + std::string(argv[4]) + ".root").c_str(), "RECREATE");
	result->Write();
	file->Close();


	//GetExpectedCL(result, argv[3]);
	//GetObservedCLs(result, argv[3], 0);
	//GetObservedCLs(result, argv[3], 1);
	//GetObservedCLs(result, argv[3], 2);

	return 0;
}
