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
#include <TNamed.h>
#include <TParameter.h>
#include <TStopwatch.h>
#include <TMD5.h>
#include <RooRandom.h>
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

unsigned int Get_ToySeed(const std::string& workspace, unsigned int seed, int mu_index, int toy_index) {
	std::uint32_t hash = 2166136261u;
	for (unsigned char c : workspace) hash = (hash ^ c) * 16777619u;
	// Nonzero seeds, independent of batch size and unique within one 21 x 10000 study.
	return (unsigned int)(((std::uint64_t)hash + seed + mu_index * 10000ULL + toy_index) % 4294967295ULL + 1ULL);
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
	*/

	try {
		if (argc != 10) throw std::runtime_error("expected input path, workspace, output path, injected mu, first index, number of toys, CLs toys, scan step and seed");
		double mu_injected = std::stod(argv[4]);
		if (!std::isfinite(mu_injected) || mu_injected < 0.0 || mu_injected > 5.0) throw std::runtime_error("injected mu must be between zero and five");
		int mu_index = (int)std::lround(mu_injected * 4.0);
		int first_index = std::stoi(argv[5]);
		int n_toys = std::stoi(argv[6]);
		int cls_toys = std::stoi(argv[7]);
		double scan_step = std::stod(argv[8]);
		unsigned long long seed_value = std::stoull(argv[9]);
		if (!std::isfinite(mu_injected) || mu_index < 0 || mu_index > 20 || std::abs(mu_injected - mu_index * 0.25) > 1e-8 || first_index < 0 || n_toys < 1 || first_index > 10000 - n_toys || cls_toys < 1 || !std::isfinite(scan_step) || scan_step <= 0.0 || seed_value > 4294967295ULL) throw std::runtime_error("invalid toy configuration");
		ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2", "Minimize"); // default: Minuit Migrad
		ROOT::Math::MinimizerOptions::SetDefaultStrategy(1); // default 1
		ROOT::Math::MinimizerOptions::SetDefaultTolerance(0.001);
		RooStats::UseNLLOffset(true); // default off
		RooMsgService::instance().setGlobalKillBelow(RooFit::WARNING);

		std::unique_ptr<TFile> input(TFile::Open((std::string(argv[1]) + "/" + argv[2]).c_str()));
		if (!input || input->IsZombie()) throw std::runtime_error("cannot open workspace file");
		RooWorkspace* w = dynamic_cast<RooWorkspace*>(input->Get("combined"));
		if (!w) throw std::runtime_error("missing combined workspace");
		RooStats::ModelConfig* model = dynamic_cast<RooStats::ModelConfig*>(w->obj("ModelConfig"));
		if (!model || !model->GetPdf() || !model->GetObservables() || !model->GetNuisanceParameters() || !model->GetParametersOfInterest() || model->GetParametersOfInterest()->getSize() != 1 || !model->GetParametersOfInterest()->find("mu") || !w->data("obsData") || !w->var("mu")) throw std::runtime_error("incomplete ModelConfig");
		RooRealVar* poi = w->var("mu");
		if (poi->getMin() != 0.0 || !std::isfinite(poi->getMax()) || poi->getMax() < 5.0) throw std::runtime_error("expected a nonnegative mu with finite maximum at least five");
		ToyMCProfileLikelihoodTestStat generation_stat(*model->GetPdf(), *w);

		/* ======================== GENERATION TRUTH ======================== */

		w->saveSnapshot("ToyMC_nominal", w->allVars());
		// Determine background truth once per job at mu = 0, holding systematic nuisances nominal.
		// Every injection and batch uses the same deterministic fit to the original obsData.
		for (RooAbsArg* arg : *model->GetNuisanceParameters()) {
			RooRealVar* parameter = dynamic_cast<RooRealVar*>(arg);
			if (parameter && std::string(parameter->GetName()).find("ABCD_T_") != 0 && std::string(parameter->GetName()).find("ABCD_p_region") != 0) parameter->setConstant(true);
		}
		poi->setVal(0.0);
		poi->setConstant(true);
		ToyFitResult truth_fit = Fit_Toy(w, model, *w->data("obsData"), false);
		if (truth_fit.status != 0) throw std::runtime_error("background truth fit failed");
		w->loadSnapshot("ToyMC_nominal");
		for (int i = 1; i <= 2; i++) {
			for (const std::string& name : { "ABCD_T_AB", "ABCD_T_CD", "ABCD_p" }) {
				RooRealVar* parameter = w->var((name + "_region" + std::to_string(i)).c_str());
				parameter->setVal(truth_fit.parameters->getRealValue(parameter->GetName()));
			}
		}
		poi->setVal(1.0);
		poi->setConstant(false);
		w->saveSnapshot("ToyMC_truth", w->allVars());
		RooStats::ToyMCSampler sampler(generation_stat, 1);
		sampler.SetPdf(*model->GetPdf());
		sampler.SetObservables(*model->GetObservables());
		sampler.SetGenerateBinned(true);
		if (model->GetGlobalObservables()) sampler.SetGlobalObservables(*model->GetGlobalObservables());

		/* ======================== OUTPUT ======================== */

		std::string filename = std::string(argv[3]) + "/ToyMC_" + argv[4] + "_" + argv[5] + ".root";
		TFile output(filename.c_str(), "RECREATE");
		if (output.IsZombie()) throw std::runtime_error("cannot create toy output file");
		TNamed("workspace", argv[2]).Write();
		std::unique_ptr<TMD5> checksum(TMD5::FileChecksum((std::string(argv[1]) + "/" + argv[2]).c_str()));
		if (!checksum) throw std::runtime_error("cannot checksum workspace");
		TNamed("workspace_md5", checksum->AsString()).Write();
		TNamed("truth_method", "mu=0 ABCD fit to obsData; nominal systematic truth; fluctuated global observables").Write();
		TParameter<int>("cls_toys", cls_toys).Write();
		TParameter<Long64_t>("base_seed", (Long64_t)seed_value).Write();
		TParameter<double>("scan_step", scan_step).Write();
		TParameter<double>("confidence_level", ConfidenceLevel).Write();
		TParameter<double>("mu_abs_tolerance", MuAbsTolerance).Write();
		TParameter<double>("mu_rel_tolerance", MuRelTolerance).Write();
		w->getSnapshot("ToyMC_truth")->Write("generation_truth");
		int toy_index, fit_status, covQual, ul_status, covered, at_boundary, inner_failures;
		unsigned int seed;
		double mu_fitted, mu_error, edm, mu_up, mu_up_low, mu_up_high, mu_up_error, seconds;
		double counts[8];
		std::vector<double> global_values;
		std::string global_names;
		if (model->GetGlobalObservables()) for (RooAbsArg* arg : *model->GetGlobalObservables()) {
			if (!dynamic_cast<RooRealVar*>(arg)) throw std::runtime_error("expected real global observables");
			global_names += std::string(arg->GetName()) + ":";
		}
		TNamed("global_names", global_names.c_str()).Write();
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
		tree.Branch("global_values", &global_values);
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
			global_values.clear();
			seed = Get_ToySeed(argv[2], (unsigned int)seed_value, mu_index, toy_index);
			RooRandom::randomGenerator()->SetSeed(seed);
			try {
				w->loadSnapshot("ToyMC_truth");
				poi->setVal(mu_injected);
				RooArgSet truth(*model->GetParametersOfInterest());
				truth.add(*model->GetNuisanceParameters());
				std::unique_ptr<RooAbsData> data(sampler.GenerateToyData(truth));
				if (!data) throw std::runtime_error("toy generation failed");
				if (model->GetGlobalObservables()) for (RooAbsArg* arg : *model->GetGlobalObservables()) global_values.push_back(((RooRealVar*)arg)->getVal());
				for (int j = 0; j < data->numEntries(); j++) {
					const RooArgSet* row = data->get(j);
					int bin = w->var("obs_x_Belle_II")->getBinning().binNumber(row->getRealValue("obs_x_Belle_II"));
					if (bin < 0 || bin >= 8) throw std::runtime_error("invalid toy bin");
					counts[bin] += data->weight();
				}
				// Keep the toy auxiliary observations. Use an injection-independent starting mu.
				poi->setVal(1.0);
				w->saveSnapshot("ToyMC_start", w->allVars());
				ToyFitResult fit = Fit_Toy(w, model, *data, true);
				fit_status = fit.status;
				covQual = fit.covQual;
				mu_fitted = fit.mu;
				mu_error = fit.error;
				edm = fit.edm;
				at_boundary = (mu_fitted <= poi->getMin() + 1e-6 || mu_fitted >= poi->getMax() - 1e-6);
				if (fit_status == 0) {
					// ul_status: 0 success, 1 fit/generation failure, 2 invalid CLs,
					// 3 failed inner fit, 4 no crossing before the POI maximum, 5 scan did not converge.
					CLsPoint low;
					low.mu = mu_fitted; // The one-sided statistic is zero at the unconditional minimum.
					CLsPoint high;
					double next_mu = std::min(poi->getMax(), mu_fitted + scan_step);
					ul_status = 5;
					for (int j = 0; j < 64; j++) {
						point = Get_ToyCLs(w, model, *data, next_mu, cls_toys);
						scan.Fill();
						inner_failures += point.failures;
						if (point.status != 0) { ul_status = point.status == 3 ? 3 : 2; break; }
						if (point.cls <= 1.0 - ConfidenceLevel) { high = point; ul_status = 0; break; }
						low = point;
						if (next_mu >= poi->getMax()) { ul_status = 4; break; }
						next_mu = std::min(poi->getMax(), mu_fitted + 2.0 * (next_mu - mu_fitted));
					}
					if (ul_status == 0) {
						for (int j = 0; j < 64; j++) {
							if (high.mu - low.mu <= MuAbsTolerance + MuRelTolerance * high.mu) break;
							point = Get_ToyCLs(w, model, *data, 0.5 * (low.mu + high.mu), cls_toys);
							scan.Fill();
							inner_failures += point.failures;
							if (point.status != 0) { ul_status = point.status == 3 ? 3 : 2; break; }
							if (point.cls > 1.0 - ConfidenceLevel) low = point;
							else high = point;
						}
						if (ul_status == 0 && high.mu - low.mu > MuAbsTolerance + MuRelTolerance * high.mu) ul_status = 5;
						if (ul_status == 0) {
							mu_up_low = low.mu;
							mu_up_high = high.mu;
							double fraction = (low.cls - (1.0 - ConfidenceLevel)) / (low.cls - high.cls);
							mu_up = low.mu + fraction * (high.mu - low.mu);
							// Local MC error estimate; the stored scan and bracket retain the numerical diagnostics.
							mu_up_error = (high.mu - low.mu) * std::hypot((1.0 - fraction) * low.error, fraction * high.error) / (low.cls - high.cls);
							covered = mu_up >= mu_injected;
						}
					}
				}
			}
			catch (const std::exception& error) {
				ul_status = 1;
				covered = -1;
				printf("[ERROR] toy %d: %s\n", toy_index, error.what());
			}
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
		TParameter<int>("completed", n_toys).Write();
		output.Flush();
		if (output.TestBit(TFile::kWriteError)) throw std::runtime_error("failed to write toy output");
		printf("Successfully completed ToyMC.\n");
	}
	catch (const std::exception& error) {
		printf("[ERROR] %s\n", error.what());
		return 1;
	}
	return 0;
}
