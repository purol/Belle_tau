#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <limits>
#include <cstdint>
#include <stdexcept>

#include <TFile.h>
#include <TTree.h>
#include <TStopwatch.h>
#include <TH1.h>
#include <RooRandom.h>
#include <RooDataSet.h>
#include <RooCategory.h>
#include <RooFitResult.h>
#include <RooMinimizer.h>
#include <RooMsgService.h>
#include <RooStats/ModelConfig.h>
#include <RooStats/FrequentistCalculator.h>
#include <RooStats/ToyMCSampler.h>
#include <RooStats/HypoTestInverter.h>
#include <RooStats/HypoTestInverterResult.h>
#include <Math/MinimizerOptions.h>

#include "ABCD_fit.h"

# define ConfidenceLevel 0.90
# define MuAbsTolerance 0.0025
# define MuRelTolerance 0.005

struct ToyFitResult {
	int status = -1;
	int covQual = -1;
	double mu = std::numeric_limits<double>::quiet_NaN();
	double error = std::numeric_limits<double>::quiet_NaN();
	double edm = std::numeric_limits<double>::quiet_NaN();
	std::unique_ptr<RooArgSet> parameters;
};

ToyFitResult Fit_Toy(RooWorkspace* w, RooStats::ModelConfig* model, RooAbsData& data, bool get_error) {
	TemporaryParameterState saved_state;
	Set_ABCD_boundaries(w, data, saved_state);
	RooArgSet globalObs;
	if (model->GetGlobalObservables()) globalObs.add(*model->GetGlobalObservables());
	std::unique_ptr<RooArgSet> allParams(model->GetPdf()->getParameters(data));
	RooStats::RemoveConstantParameters(allParams.get());
	std::unique_ptr<RooAbsReal> nll(model->GetPdf()->createNLL(data, RooFit::Constrain(*allParams), RooFit::GlobalObservables(globalObs), RooFit::Offset(true)));
	ToyFitResult result;
	if (allParams->empty()) {
		result.status = std::isfinite(nll->getVal()) ? 0 : -1;
		result.edm = 0.0;
	}
	else {
		RooMinimizer minim(*nll);
		minim.setPrintLevel(-1);
		minim.setStrategy(1);
		minim.setEps(0.001);
		result.status = minim.minimize("Minuit2", "Minimize");
		if (result.status != 0) {
			minim.setStrategy(2);
			result.status = minim.minimize("Minuit2", "Minimize");
		}
		// A covariance failure at a physical boundary is recorded separately from the minimum.
		if (get_error && result.status == 0) minim.hesse();
		std::unique_ptr<RooFitResult> fitres(minim.save());
		result.covQual = fitres->covQual();
		result.edm = fitres->edm();
		if (!std::isfinite(fitres->minNll()) || !std::isfinite(result.edm)) result.status = -1;
	}
	result.mu = w->var("mu")->getVal();
	if (get_error && w->var("mu")->hasError()) result.error = w->var("mu")->getError();
	// Capture fitted boundary values before TemporaryParameterState restores the starting values.
	result.parameters.reset(w->allVars().snapshot());
	return result;
}

class ToyMCProfileLikelihoodTestStat : public ABCDProfileLikelihoodTestStat {
public:
	int failures = 0;

	ToyMCProfileLikelihoodTestStat(RooAbsPdf& pdf, RooWorkspace& w) : ABCDProfileLikelihoodTestStat(pdf, w) {}

	double Evaluate(RooAbsData& data, RooArgSet& paramsOfInterest) override {
		double value = ABCDProfileLikelihoodTestStat::Evaluate(data, paramsOfInterest);
		if (!std::isfinite(value) || value < 0.0) failures++;
		return value;
	}
};

struct CLsPoint {
	double mu = 0.0;
	double cls = 1.0;
	double error = 0.0;
	int failures = 0;
	int status = 0;
};

CLsPoint Get_ToyCLs(RooWorkspace* w, RooStats::ModelConfig* sbModel, RooAbsData& data, double mu, int ntoys) {
	CLsPoint point;
	point.mu = mu;
	point.cls = point.error = std::numeric_limits<double>::quiet_NaN();
	RooRealVar* poi = w->var("mu");
	w->loadSnapshot("ToyMC_start");
	poi->setVal(0.0);
	poi->setConstant(true);
	ToyFitResult bfit = Fit_Toy(w, sbModel, data, false);
	w->loadSnapshot("ToyMC_start");
	poi->setVal(mu);
	poi->setConstant(true);
	ToyFitResult sbfit = Fit_Toy(w, sbModel, data, false);
	if (bfit.status != 0 || sbfit.status != 0) {
		point.status = 1;
		return point;
	}
	// Use the same empty-bin boundaries for the conditional generation fits and the test statistic.
	std::unique_ptr<RooArgSet> bNuis(sbModel->GetNuisanceParameters()->snapshot());
	std::unique_ptr<RooArgSet> sbNuis(sbModel->GetNuisanceParameters()->snapshot());
	bNuis->assignValueOnly(*bfit.parameters);
	sbNuis->assignValueOnly(*sbfit.parameters);
	w->loadSnapshot("ToyMC_start");
	std::unique_ptr<RooStats::ModelConfig> bModel((RooStats::ModelConfig*)sbModel->Clone("BonlyModel"));
	poi->setVal(0.0);
	bModel->SetSnapshot(*poi);
	poi->setVal(mu);
	sbModel->SetSnapshot(*poi);
	ToyMCProfileLikelihoodTestStat plr(*sbModel->GetPdf(), *w);
	plr.SetOneSided(true);
	plr.SetMinimizer("Minuit2");
	plr.SetStrategy(1);
	plr.SetLOffset(true);
	plr.SetTolerance(0.001);
	plr.SetPrintLevel(-1);
	if (sbModel->GetGlobalObservables()) plr.SetGlobalObservables(*sbModel->GetGlobalObservables());
	RooStats::FrequentistCalculator FreqCalc(data, *bModel, *sbModel);
	FreqCalc.SetConditionalMLEsAlt(bNuis.get());
	FreqCalc.SetConditionalMLEsNull(sbNuis.get());
	RooStats::ToyMCSampler* toymcs = (RooStats::ToyMCSampler*)FreqCalc.GetTestStatSampler();
	toymcs->SetTestStatistic(&plr);
	toymcs->SetGenerateBinned(true);
	FreqCalc.SetToys(ntoys, ntoys);
	RooStats::HypoTestInverter inverter(FreqCalc);
	inverter.SetConfidenceLevel(ConfidenceLevel);
	inverter.UseCLs(true);
	inverter.SetVerbose(false);
	inverter.SetFixedScan(1, mu, mu);
	std::unique_ptr<RooStats::HypoTestInverterResult> result(inverter.GetInterval());
	point.failures = plr.failures;
	if (!result || result->ArraySize() != 1) point.status = 2;
	else {
		point.cls = result->CLs(0);
		point.error = result->CLsError(0);
		if (!std::isfinite(point.cls) || point.cls < 0.0 || !std::isfinite(point.error) || point.error < 0.0) point.status = 2;
		// Do not silently accept a sampling distribution containing failed fits.
		if (point.failures != 0) point.status = 3;
	}
	return point;
}

struct ToyLimitResult {
	// status: 0 success, 1 fit/generation failure, 2 invalid CLs,
	// 3 failed inner fit, 4 no crossing, 5 scan did not converge.
	int status = 5;
	int failures = 0;
	double mu_up = std::numeric_limits<double>::quiet_NaN();
	double low = std::numeric_limits<double>::quiet_NaN();
	double high = std::numeric_limits<double>::quiet_NaN();
	double error = std::numeric_limits<double>::quiet_NaN();
};

ToyLimitResult Get_ToyLimit(RooWorkspace* w, RooStats::ModelConfig* model, RooAbsData& data, double mu_fitted, double scan_step, int cls_toys, TTree& scan, CLsPoint& point) {
	ToyLimitResult limit;
	CLsPoint low, high;
	low.mu = mu_fitted; // The one-sided statistic is zero at the fitted mu.
	double maximum = w->var("mu")->getMax();
	double next_mu = std::min(maximum, mu_fitted + scan_step);

	// Find two points around CLs = 0.1.
	for (int i = 0; i < 64; i++) {
		point = Get_ToyCLs(w, model, data, next_mu, cls_toys);
		scan.Fill();
		limit.failures += point.failures;
		if (point.status != 0) {
			limit.status = point.status == 3 ? 3 : 2;
			return limit;
		}
		if (point.cls <= 1.0 - ConfidenceLevel) {
			high = point;
			limit.status = 0;
			break;
		}
		low = point;
		if (next_mu >= maximum) {
			limit.status = 4;
			return limit;
		}
		next_mu = std::min(maximum, mu_fitted + 2.0 * (next_mu - mu_fitted));
	}
	if (limit.status != 0) return limit;

	// Narrow the interval and interpolate the crossing.
	for (int i = 0; i < 64; i++) {
		if (high.mu - low.mu <= MuAbsTolerance + MuRelTolerance * high.mu) break;
		point = Get_ToyCLs(w, model, data, 0.5 * (low.mu + high.mu), cls_toys);
		scan.Fill();
		limit.failures += point.failures;
		if (point.status != 0) {
			limit.status = point.status == 3 ? 3 : 2;
			return limit;
		}
		if (point.cls > 1.0 - ConfidenceLevel) low = point;
		else high = point;
	}
	if (high.mu - low.mu > MuAbsTolerance + MuRelTolerance * high.mu) {
		limit.status = 5;
		return limit;
	}
	limit.low = low.mu;
	limit.high = high.mu;
	double fraction = (low.cls - (1.0 - ConfidenceLevel)) / (low.cls - high.cls);
	limit.mu_up = low.mu + fraction * (high.mu - low.mu);
	limit.error = (high.mu - low.mu) * std::hypot((1.0 - fraction) * low.error, fraction * high.error) / (low.cls - high.cls);
	return limit;
}

unsigned int Get_ToySeed(const std::string& workspace, unsigned int seed, int mu_index, int toy_index) {
	std::uint32_t hash = 2166136261u;
	for (unsigned char c : workspace) hash = (hash ^ c) * 16777619u;
	// Nonzero seeds, independent of batch size and unique within one 11 x 10000 study.
	return (unsigned int)(((std::uint64_t)hash + seed + mu_index * 10000ULL + toy_index) % 4294967295ULL + 1ULL);
}

void Check_MC_histogram(const TH1* histogram, const RooAbsBinning& binning) {
	if (!histogram || histogram->GetDimension() != 1 || histogram->GetNbinsX() != 8) throw std::runtime_error("expected an eight-bin MC histogram");
	for (int i = 0; i < 8; i++) {
		double value = histogram->GetBinContent(i + 1);
		if (!std::isfinite(value) || value < 0.0) throw std::runtime_error(std::string("negative or nonfinite MC expectation in ") + histogram->GetName());
		if (std::abs(histogram->GetXaxis()->GetBinLowEdge(i + 1) - binning.binLow(i)) > 1e-9 || std::abs(histogram->GetXaxis()->GetBinUpEdge(i + 1) - binning.binHigh(i)) > 1e-9) throw std::runtime_error("MC histogram binning does not match the workspace");
	}
}

std::unique_ptr<RooDataSet> Generate_MC_toy(RooWorkspace* w, RooStats::ModelConfig* model, const TH1* background, const TH1* signal, double mu, double* counts) {
	RooRealVar* observable = w->var("obs_x_Belle_II");
	RooCategory* channel = w->cat("channelCat");
	channel->setLabel("Belle_II");
	RooRealVar weight("toy_weight", "toy_weight", 1.0);
	RooArgSet row(*model->GetObservables());
	row.add(weight);
	std::unique_ptr<RooDataSet> data(new RooDataSet("toyData", "toyData", row, RooFit::WeightVar(weight)));
	for (int i = 0; i < 8; i++) {
		// Preserve the MC nonclosure. Do not derive A from B, C and D.
		double expected = background->GetBinContent(i + 1) + mu * signal->GetBinContent(i + 1);
		if (!std::isfinite(expected) || expected < 0.0) throw std::runtime_error("invalid injected MC expectation");
		counts[i] = expected == 0.0 ? 0.0 : RooRandom::randomGenerator()->PoissonD(expected);
		observable->setVal(observable->getBinning().binCenter(i));
		data->add(row, counts[i]);
	}
	return data;
}

int main(int argc, char* argv[]) {
	/*
	* argv[1]: input path
	* argv[2]: workspace root file name
	* argv[3]: output path
	* argv[4]: injected mu
	* argv[5]: first toy index
	* argv[6]: number of toys in this job
	* argv[7]: number of CLs toys per hypothesis
	* argv[8]: initial upper limit scan step
	* argv[9]: random seed
	* argv[10]: injected mu step
	*/

	if (argc != 11) {
		printf("[ERROR] expected input path, workspace, output path, injected mu, first index, number of toys, CLs toys, scan step, seed and injected mu step\n");
		return 1;
	}
	double injection_step = std::stod(argv[10]);
	double mu_max = 10.0 * injection_step;
	double mu_injected = std::stod(argv[4]);
	if (!std::isfinite(injection_step) || injection_step <= 0.0 || !std::isfinite(mu_max) || !std::isfinite(mu_injected) || mu_injected < 0.0 || mu_injected > mu_max) {
		printf("[ERROR] invalid injected mu or step\n");
		return 1;
	}
	int mu_index = (int)std::lround(mu_injected / injection_step);
	int first_index = std::stoi(argv[5]);
	int n_toys = std::stoi(argv[6]);
	int cls_toys = std::stoi(argv[7]);
	double scan_step = std::stod(argv[8]);
	unsigned long long seed_value = std::stoull(argv[9]);
	if (!std::isfinite(mu_injected) || mu_index < 0 || mu_index > 10 || std::abs(mu_injected - mu_index * injection_step) > 1e-8 || first_index < 0 || n_toys < 1 || first_index > 10000 - n_toys || cls_toys < 1 || !std::isfinite(scan_step) || scan_step <= 0.0 || seed_value > 4294967295ULL) {
		printf("[ERROR] invalid toy configuration\n");
		return 1;
	}
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2", "Minimize"); // default: Minuit Migrad
	ROOT::Math::MinimizerOptions::SetDefaultStrategy(1); // default 1
	ROOT::Math::MinimizerOptions::SetDefaultTolerance(0.001);
	RooStats::UseNLLOffset(true); // default off
	RooMsgService::instance().setGlobalKillBelow(RooFit::WARNING);

	std::unique_ptr<TFile> input(TFile::Open((std::string(argv[1]) + "/" + argv[2]).c_str()));
	if (!input || input->IsZombie()) {
		printf("[ERROR] cannot open workspace file\n");
		return 1;
	}
	RooWorkspace* w = dynamic_cast<RooWorkspace*>(input->Get("combined"));
	if (!w) {
		printf("[ERROR] missing combined workspace\n");
		return 1;
	}
	RooStats::ModelConfig* model = dynamic_cast<RooStats::ModelConfig*>(w->obj("ModelConfig"));
	if (!model || !w->var("mu") || !w->var("obs_x_Belle_II") || !w->cat("channelCat")) {
		printf("[ERROR] incomplete ModelConfig\n");
		return 1;
	}
	RooRealVar* poi = w->var("mu");
	if (poi->getMin() != 0.0 || !std::isfinite(poi->getMax()) || poi->getMax() < mu_max) {
		printf("[ERROR] expected a nonnegative mu with finite maximum covering the injection range\n");
		return 1;
	}
	// The saved workspace uses two ABCD regions.
	for (int i = 1; i <= 2; i++) {
		for (const std::string& name : { "ABCD_T_AB", "ABCD_T_CD", "ABCD_p" }) {
			if (!w->var((name + "_region" + std::to_string(i)).c_str())) {
				printf("[ERROR] rebuild the ABCD workspace before running ToyMC\n");
				return 1;
			}
		}
	}

	/* ======================== READ MC ======================== */

	// The workspace and nominal MC histograms are written together by CreateWorkSpace.
	std::string histogram_name = argv[2];
	if (histogram_name != "workspace.root" && (histogram_name.find("workspace_") != 0 || histogram_name.size() < 15 || histogram_name.substr(histogram_name.size() - 5) != ".root")) {
		printf("[ERROR] expected workspace.root or workspace_<parameters>.root\n");
		return 1;
	}
	histogram_name.replace(0, 9, "histogram_output");
	std::string histogram_path = std::string(argv[1]) + "/" + histogram_name;
	std::unique_ptr<TFile> histograms(TFile::Open(histogram_path.c_str()));
	if (!histograms || histograms->IsZombie()) {
		printf("[ERROR] cannot open MC histogram file: %s\n", histogram_path.c_str());
		return 1;
	}
	TH1* background = dynamic_cast<TH1*>(histograms->Get("bkg_MC_th1d"));
	TH1* signal = dynamic_cast<TH1*>(histograms->Get("signal_MC_th1d"));
	Check_MC_histogram(background, w->var("obs_x_Belle_II")->getBinning());
	Check_MC_histogram(signal, w->var("obs_x_Belle_II")->getBinning());
	poi->setVal(1.0);
	poi->setConstant(false);
	// ABCD values are only fit starting values. No fit to obsData defines generation truth.
	w->saveSnapshot("ToyMC_nominal", w->allVars());


	/* ======================== OUTPUT ======================== */

	std::string filename = std::string(argv[3]) + "/ToyMC_" + argv[4] + "_" + argv[5] + ".root";
	TFile output(filename.c_str(), "RECREATE");
	if (output.IsZombie()) {
		printf("[ERROR] cannot create toy output file\n");
		return 1;
	}
	int toy_index, fit_status, covQual, ul_status, covered, at_boundary, inner_failures;
	unsigned int seed;
	double mu_fitted, mu_error, edm, mu_up, mu_up_low, mu_up_high, mu_up_error, seconds;
	double counts[8];
	TTree tree("toys", "Toy MC fit and upper limit results");
	tree.Branch("mu_injected", &mu_injected);
	tree.Branch("toy_index", &toy_index);
	tree.Branch("seed", &seed);
	tree.Branch("mu_fitted", &mu_fitted);
	tree.Branch("mu_error", &mu_error);
	tree.Branch("fit_status", &fit_status);
	tree.Branch("covQual", &covQual);
	tree.Branch("edm", &edm);
	tree.Branch("at_boundary", &at_boundary);
	tree.Branch("mu_up", &mu_up);
	tree.Branch("mu_up_low", &mu_up_low);
	tree.Branch("mu_up_high", &mu_up_high);
	tree.Branch("mu_up_error", &mu_up_error);
	tree.Branch("ul_status", &ul_status);
	tree.Branch("covered", &covered);
	tree.Branch("inner_failures", &inner_failures);
	tree.Branch("seconds", &seconds);
	tree.Branch("counts", counts, "counts[8]/D");
	CLsPoint point;
	TTree scan("scan", "CLs scan points for each toy");
	scan.Branch("toy_index", &toy_index);
	scan.Branch("mu_injected", &mu_injected);
	scan.Branch("mu_test", &point.mu);
	scan.Branch("cls", &point.cls);
	scan.Branch("cls_error", &point.error);
	scan.Branch("failures", &point.failures);
	scan.Branch("status", &point.status);

	/* ======================== TOY STUDY ======================== */

	for (toy_index = first_index; toy_index < first_index + n_toys; toy_index++) {
		TStopwatch sw;
		sw.Start();
		mu_fitted = mu_error = edm = mu_up = mu_up_low = mu_up_high = mu_up_error = std::numeric_limits<double>::quiet_NaN();
		fit_status = covQual = covered = at_boundary = -1;
		ul_status = 1;
		inner_failures = 0;
		std::fill(counts, counts + 8, 0.0);
		seed = Get_ToySeed(argv[2], (unsigned int)seed_value, mu_index, toy_index);
		RooRandom::randomGenerator()->SetSeed(seed);
		try {
			w->loadSnapshot("ToyMC_nominal");
			poi->setVal(mu_injected);
			// Only auxiliary observations use the workspace PDF; event counts use the MC expectations.
			if (model->GetGlobalObservables() && !model->GetGlobalObservables()->empty()) {
				std::unique_ptr<RooDataSet> globals(model->GetPdf()->generateSimGlobal(*model->GetGlobalObservables(), 1));
				RooArgSet globalObs(*model->GetGlobalObservables());
				globalObs.assignValueOnly(*globals->get(0));
			}
			// generate bin counts
			std::unique_ptr<RooDataSet> data = Generate_MC_toy(w, model, background, signal, mu_injected, counts);
			// Keep the toy auxiliary observations. Use an injection-independent starting mu.
			poi->setVal(1.0);
			w->saveSnapshot("ToyMC_start", w->allVars());
			// fit
			ToyFitResult fit = Fit_Toy(w, model, *data, true);
			fit_status = fit.status;
			covQual = fit.covQual;
			mu_fitted = fit.mu;
			mu_error = fit.error;
			edm = fit.edm;
			at_boundary = (mu_fitted <= poi->getMin() + 1e-6 || mu_fitted >= poi->getMax() - 1e-6);
			// upper limit
			if (fit_status == 0) {
				ToyLimitResult limit = Get_ToyLimit(w, model, *data, mu_fitted, scan_step, cls_toys, scan, point);
				mu_up = limit.mu_up;
				mu_up_low = limit.low;
				mu_up_high = limit.high;
				mu_up_error = limit.error;
				ul_status = limit.status;
				inner_failures = limit.failures;
				if (ul_status == 0) covered = mu_up >= mu_injected;
			}
		}
		catch (const std::exception& error) {
			ul_status = 1;
			covered = -1;
			printf("[ERROR] toy %d: %s\n", toy_index, error.what());
		}
		// save result
		sw.Stop();
		seconds = sw.RealTime();
		output.cd();
		tree.Fill();
		tree.AutoSave("SaveSelf");
		scan.AutoSave("SaveSelf");
		printf("injected mu: %lf, toy: %d, fitted mu: %lf, upper limit: %lf, fit status: %d, UL status: %d, consumed time: %lf (s)\n", mu_injected, toy_index, mu_fitted, mu_up, fit_status, ul_status, seconds);
		fflush(stdout);
	}
	output.cd();
	tree.Write("", TObject::kOverwrite);
	scan.Write("", TObject::kOverwrite);
	output.Flush();
	if (output.TestBit(TFile::kWriteError)) {
		printf("[ERROR] failed to write toy output\n");
		return 1;
	}
	printf("Successfully completed ToyMC.\n");
	return 0;
}
