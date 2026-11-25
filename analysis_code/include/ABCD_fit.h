#ifndef ABCD_FIT_H
#define ABCD_FIT_H

#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>

#include <RooWorkspace.h>
#include <RooAbsData.h>
#include <RooRealVar.h>
#include <RooAbsBinning.h>
#include <RooAbsCategory.h>
#include <RooStats/ProfileLikelihoodTestStat.h>

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

inline void Set_ABCD_boundaries(RooWorkspace* w, RooAbsData& data, TemporaryParameterState& saved_state) {
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
}

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
		TemporaryParameterState saved_state;
		Set_ABCD_boundaries(w, data, saved_state);

		// Apply the same boundaries to the unconditional and conditional fits; keep systematic nuisances free.
		return RooStats::ProfileLikelihoodTestStat::Evaluate(data, paramsOfInterest);
	}
};


#endif
