#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>

#include <RooStats/HypoTestInverter.h>
#include <RooStats/HypoTestInverterResult.h>
#include <RooStats/FrequentistCalculator.h>
#include <RooStats/ToyMCSampler.h>
#include <RooStats/ProfileLikelihoodTestStat.h>

#include <RooRandom.h>
#include <TFile.h>
#include <RooWorkspace.h>
#include <RooDataSet.h>
#include <RooRealVar.h>
#include <RooAbsBinning.h>
#include <RooAbsCategory.h>
#include <TStopwatch.h>
#include <TMath.h>

#include <Math/MinimizerOptions.h>

std::random_device rd;
std::default_random_engine generator(rd());

# define NToys 10000

class TemporaryParameterState {
private:
	struct ParameterState {
		RooRealVar* parameter;
		double value;
		bool constant;
		bool has_error;
		double error;
		bool has_asym_error;
		double error_low;
		double error_high;
	};
	std::vector<ParameterState> states;

public:
	TemporaryParameterState() = default;
	TemporaryParameterState(const TemporaryParameterState&) = delete;
	TemporaryParameterState& operator=(const TemporaryParameterState&) = delete;

	void FixForThisFit(RooRealVar* parameter, double value) {
		states.push_back({ parameter, parameter->getVal(), parameter->isConstant(), parameter->hasError(), parameter->getError(), parameter->hasAsymError(), parameter->getAsymErrorLo(), parameter->getAsymErrorHi() });
		parameter->setVal(value);
		parameter->setConstant(true);
	}

	~TemporaryParameterState() {
		// Restore the entry state even when a fit fails or throws an exception.
		for (auto state = states.rbegin(); state != states.rend(); state++) {
			state->parameter->setVal(state->value);
			state->parameter->setConstant(state->constant);
			if (state->has_error) state->parameter->setError(state->error);
			else state->parameter->removeError();
			if (state->has_asym_error) state->parameter->setAsymError(state->error_low, state->error_high);
			else state->parameter->removeAsymError();
		}
	}
};

class ABCDProfileLikelihoodTestStat : public RooStats::ProfileLikelihoodTestStat {
private:
	RooWorkspace* w;

public:
	ABCDProfileLikelihoodTestStat(RooAbsPdf& pdf_, RooWorkspace& w_) : RooStats::ProfileLikelihoodTestStat(pdf_), w(&w_) {
		RooRealVar* observable = w->var("obs_x_Belle_II");
		if (observable == nullptr || observable->getBinning().numBins() != 8) {
			throw std::runtime_error("[ABCD fit] expected the eight-bin Belle_II model");
		}
		for (int i = 1; i <= 2; i++) {
			for (const std::string& name : { "ABCD_T_AB", "ABCD_T_CD", "ABCD_p" }) {
				if (w->var((name + "_region" + std::to_string(i)).c_str()) == nullptr) {
					throw std::runtime_error("[ABCD fit] rebuild the workspace with T_AB, T_CD and p before running CLs");
				}
			}
		}
		// The fixed parameters can differ between datasets. Build a fresh NLL for each evaluation.
		RooStats::ProfileLikelihoodTestStat::SetAlwaysReuseNLL(false);
		SetReuseNLL(false);
	}

	double Evaluate(RooAbsData& data, RooArgSet& paramsOfInterest) override {
		double counts[8] = { 0.0 };
		const RooAbsBinning& binning = w->var("obs_x_Belle_II")->getBinning();
		// Use the bin coordinates and weights. Empty bins need not have a row in the dataset.
		for (int j = 0; j < data.numEntries(); j++) {
			const RooArgSet* row = data.get(j);
			const RooRealVar* observable = dynamic_cast<const RooRealVar*>(row->find("obs_x_Belle_II"));
			const RooAbsCategory* channel = dynamic_cast<const RooAbsCategory*>(row->find("channelCat"));
			if (observable == nullptr || (channel != nullptr && std::string(channel->getCurrentLabel()) != "Belle_II")) {
				throw std::runtime_error("[ABCD fit] unexpected observable or channel in the dataset");
			}
			double value = observable->getVal();
			double weight = data.weight();
			if (!std::isfinite(value) || value < binning.lowBound() || value > binning.highBound() || !std::isfinite(weight) || weight < 0.0) {
				throw std::runtime_error("[ABCD fit] invalid bin coordinate or negative/nonfinite event count");
			}
			int bin = binning.binNumber(value);
			if (bin < 0 || bin >= 8) throw std::runtime_error("[ABCD fit] invalid bin index");
			counts[bin] += weight;
			if (!std::isfinite(counts[bin])) throw std::runtime_error("[ABCD fit] nonfinite event count");
		}

		TemporaryParameterState saved_state;
		for (int i = 1; i <= 2; i++) {
			std::string suffix = "_region" + std::to_string(i);
			RooRealVar* T_AB = w->var(("ABCD_T_AB" + suffix).c_str());
			RooRealVar* T_CD = w->var(("ABCD_T_CD" + suffix).c_str());
			RooRealVar* p = w->var(("ABCD_p" + suffix).c_str());
			// These boundaries assume free ABCD parameters. Preserve externally fixed regions.
			if (T_AB->isConstant() || T_CD->isConstant() || p->isConstant()) continue;
			double nA = counts[4 * (i - 1)];
			double nB = counts[4 * (i - 1) + 1];
			double nC = counts[4 * (i - 1) + 2];
			double nD = counts[4 * (i - 1) + 3];

			// Empty pairs have their maximum likelihood at zero background.
			if (nA == 0.0 && nB == 0.0) saved_state.FixForThisFit(T_AB, 0.0);
			if (nC == 0.0 && nD == 0.0) saved_state.FixForThisFit(T_CD, 0.0);
			// With both totals zero, the fraction does not affect the likelihood.
			if (nA == 0.0 && nB == 0.0 && nC == 0.0 && nD == 0.0) saved_state.FixForThisFit(p, 0.5);
			else if (nA == 0.0 && nC == 0.0) saved_state.FixForThisFit(p, 0.0);
			else if (nB == 0.0 && nD == 0.0) saved_state.FixForThisFit(p, 1.0);
		}

		// Apply the same boundaries to the unconditional and conditional fits; keep systematic nuisances free.
		return RooStats::ProfileLikelihoodTestStat::Evaluate(data, paramsOfInterest);
	}
};


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
		RooStats::SamplingDistribution* s = fResults->GetExpectedPValueDist(i);
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
	* argv[2]: workspace root file name
	* argv[3]: output path
	* argv[4]: mu value to test
	* argv[5]: index
	*/

	RooRandom::randomGenerator()->SetSeed(rd());

	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2", "Minimize"); // default: Minuit Migrad
	ROOT::Math::MinimizerOptions::SetDefaultStrategy(1); // default 1
	RooStats::UseNLLOffset(true); // default off
	double eps = 0.001;
	ROOT::Math::MinimizerOptions::SetDefaultTolerance(eps); // default 0.01. but it is better to use 0.001

	std::string fname = std::string(argv[1]) + "/" + std::string(argv[2]);

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
	RooStats::ProfileLikelihoodTestStat* plr = new ABCDProfileLikelihoodTestStat(*sbModel->GetPdf(), *w);
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
	inverter.SetFixedScan(1, std::stof(argv[4]), std::stof(argv[4])); // set number of points , xmin and xmax

	TStopwatch sw;
	sw.Start();

	RooStats::HypoTestInverterResult* result = inverter.GetInterval();

	sw.Stop();
	printf("consumed time: %lf (s)\n", sw.RealTime());

	TFile* file = new TFile((std::string(argv[3]) + "/Hypotestinverter_freq_" + std::string(argv[4]) + "_" + std::string(argv[5]) + ".root").c_str(), "RECREATE");
	result->Write();
	file->Close();


	//GetExpectedCL(result, argv[4]);
	//GetObservedCLs(result, argv[4], 0);
	//GetObservedCLs(result, argv[4], 1);
	//GetObservedCLs(result, argv[4], 2);

	return 0;
}
