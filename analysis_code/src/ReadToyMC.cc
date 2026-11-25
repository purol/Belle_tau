#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <memory>
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TLeaf.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TNamed.h>
#include <TParameter.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TText.h>
#include <TLegend.h>
#include <TMath.h>
#include <RooArgSet.h>
#include <RooRealVar.h>

struct ToySummary {
	int total = 0;
	int fits = 0;
	int limits = 0;
	int covered = 0;
	int boundaries = 0;
	int ambiguous = 0;
	double sum = 0.0;
	double sum2 = 0.0;
	std::vector<double> upper_limits;
	std::set<int> indices;
};

template <class T> T Read_Parameter(TFile& file, const char* name) {
	TParameter<T>* parameter = dynamic_cast<TParameter<T>*>(file.Get(name));
	if (!parameter) throw std::runtime_error(std::string("missing parameter: ") + name);
	return parameter->GetVal();
}

std::string Read_Name(TFile& file, const char* name) {
	TNamed* value = dynamic_cast<TNamed*>(file.Get(name));
	if (!value) throw std::runtime_error(std::string("missing metadata: ") + name);
	return value->GetTitle();
}

void Draw_ToyGraph(TGraphErrors& graph, const char* path, const char* filename, bool diagonal = false, double reference = 0.0) {
	TCanvas c("c", "c", 696, 472);
	graph.SetMarkerStyle(20);
	graph.SetLineWidth(2);
	TText message;
	if (graph.GetN() > 0) graph.Draw("AP");
	else {
		c.DrawFrame(0.0, 0.0, 5.0, 1.05, graph.GetTitle());
		message.DrawTextNDC(0.25, 0.5, "No successful calculations");
	}
	TLine line(0.0, reference, 5.0, diagonal ? 5.0 : reference);
	line.SetLineStyle(2);
	line.Draw();
	c.SaveAs((std::string(path) + "/" + filename + ".png").c_str());
	graph.Write();
}

int main(int argc, char* argv[]) {
	/*
	* argv[1]: ToyMC root path
	* argv[2]: output path
	* argv[3]: expected number of toys per injected mu
	*/

	try {
		if (argc != 4) throw std::runtime_error("expected ToyMC root path, output path and number of toys per injected mu");
		int expected = std::stoi(argv[3]);
		if (expected < 1 || expected > 10000) throw std::runtime_error("invalid expected toy count");
		gROOT->SetBatch(true);
		std::vector<std::string> names;
		void* directory = gSystem->OpenDirectory(argv[1]);
		if (!directory) throw std::runtime_error("cannot open ToyMC directory");
		const char* entry;
		while ((entry = gSystem->GetDirEntry(directory))) {
			std::string name = entry;
			if (name.find("ToyMC_") == 0 && name.size() > 5 && name.substr(name.size() - 5) == ".root") names.push_back(std::string(argv[1]) + "/" + name);
		}
		gSystem->FreeDirectory(directory);
		std::sort(names.begin(), names.end());
		if (names.empty()) throw std::runtime_error("no ToyMC output files");
		std::vector<ToySummary> summaries(21);
		std::set<unsigned int> seeds;
		std::string configuration;
		std::unique_ptr<RooArgSet> truth;
		TChain toys("toys");
		TChain scans("scan");

		/* ======================== CHECK INPUT FILES ======================== */

		for (const std::string& name : names) {
			TFile input(name.c_str());
			if (input.IsZombie() || input.TestBit(TFile::kRecovered)) throw std::runtime_error("incomplete ROOT file: " + name);
			int completed = Read_Parameter<int>(input, "completed");
			TTree* tree = dynamic_cast<TTree*>(input.Get("toys"));
			if (!tree || completed < 1 || tree->GetEntries() != completed || !input.Get("scan")) throw std::runtime_error("incomplete toy batch: " + name);
			std::string current = Read_Name(input, "workspace") + "\n" + Read_Name(input, "workspace_md5") + "\n" + Read_Name(input, "truth_method") + "\n" + Read_Name(input, "global_names");
			current += "\n" + std::to_string(Read_Parameter<int>(input, "cls_toys"));
			current += "\n" + std::to_string(Read_Parameter<Long64_t>(input, "base_seed"));
			for (const char* parameter : { "scan_step", "confidence_level", "mu_abs_tolerance", "mu_rel_tolerance" }) current += "\n" + std::to_string(Read_Parameter<double>(input, parameter));
			RooArgSet* current_truth = dynamic_cast<RooArgSet*>(input.Get("generation_truth"));
			if (!current_truth) throw std::runtime_error("missing generation truth: " + name);
			if (configuration.empty()) {
				configuration = current;
				truth.reset(current_truth->snapshot());
			}
			else {
				if (configuration != current || truth->getSize() != current_truth->getSize()) throw std::runtime_error("mixed toy configurations: " + name);
				for (RooAbsArg* arg : *truth) {
					RooRealVar* first = dynamic_cast<RooRealVar*>(arg);
					RooRealVar* second = dynamic_cast<RooRealVar*>(current_truth->find(arg->GetName()));
					if (!first || !second || std::abs(first->getVal() - second->getVal()) > 1e-8 * (1.0 + std::abs(first->getVal())) || first->isConstant() != second->isConstant()) throw std::runtime_error("inconsistent generation truth: " + name);
				}
			}
			for (const char* leaf : { "mu_injected", "toy_index", "seed", "fit_status", "mu_fitted", "ul_status", "mu_up", "mu_up_low", "mu_up_high", "covered", "at_boundary" }) {
				if (!tree->GetLeaf(leaf)) throw std::runtime_error(std::string("missing toy branch: ") + leaf);
			}
			for (Long64_t i = 0; i < tree->GetEntries(); i++) {
				if (tree->GetEntry(i) <= 0) throw std::runtime_error("cannot read toy row: " + name);
				double mu = tree->GetLeaf("mu_injected")->GetValue();
				if (!std::isfinite(mu) || mu < 0.0 || mu > 5.0) throw std::runtime_error("invalid injected mu: " + name);
				int mu_index = (int)std::lround(mu * 4.0);
				int index = (int)tree->GetLeaf("toy_index")->GetValue();
				if (!std::isfinite(mu) || mu_index < 0 || mu_index > 20 || std::abs(mu - mu_index * 0.25) > 1e-8 || index < 0 || index >= expected) throw std::runtime_error("unexpected injection or toy index: " + name);
				ToySummary& summary = summaries.at(mu_index);
				if (!summary.indices.insert(index).second) throw std::runtime_error("duplicate toy index: " + name);
				unsigned int seed = (unsigned int)tree->GetLeaf("seed")->GetValue();
				if (seed == 0 || !seeds.insert(seed).second) throw std::runtime_error("duplicate or zero toy seed: " + name);
				summary.total++;
				int fit_status = (int)tree->GetLeaf("fit_status")->GetValue();
				double fitted = tree->GetLeaf("mu_fitted")->GetValue();
				if (fit_status == 0) {
					if (!std::isfinite(fitted)) throw std::runtime_error("successful fit has nonfinite mu: " + name);
					summary.fits++;
					summary.sum += fitted;
					summary.sum2 += fitted * fitted;
					if (tree->GetLeaf("at_boundary")->GetValue() == 1) summary.boundaries++;
				}
				if (tree->GetLeaf("ul_status")->GetValue() == 0) {
					double upper = tree->GetLeaf("mu_up")->GetValue();
					double low = tree->GetLeaf("mu_up_low")->GetValue();
					double high = tree->GetLeaf("mu_up_high")->GetValue();
					if (fit_status != 0 || !std::isfinite(upper) || !std::isfinite(low) || !std::isfinite(high) || low < 0.0 || low > upper || upper > high) throw std::runtime_error("invalid successful upper limit: " + name);
					int covered = upper >= mu;
					if (covered != (int)tree->GetLeaf("covered")->GetValue()) throw std::runtime_error("inconsistent coverage flag: " + name);
					summary.limits++;
					summary.covered += covered;
					if (low < mu && mu <= high) summary.ambiguous++;
					summary.upper_limits.push_back(upper);
				}
			}
			toys.Add(name.c_str());
			scans.Add(name.c_str());
		}
		for (int i = 0; i <= 20; i++) if (summaries.at(i).total != expected) throw std::runtime_error("missing toys at injected mu " + std::to_string(i * 0.25));

		/* ======================== SUMMARY ======================== */

		TFile output((std::string(argv[2]) + "/ToyMC.root").c_str(), "RECREATE");
		if (output.IsZombie()) throw std::runtime_error("cannot create summary ROOT file");
		TNamed("configuration", configuration.c_str()).Write();
		truth->Write("generation_truth");
		if (!toys.CloneTree(-1, "fast") || !scans.CloneTree(-1, "fast")) throw std::runtime_error("cannot merge toy trees");
		double mu, mean, mean_error, bias, median, minus1, plus1, minus2, plus2;
		double coverage, coverage_error, coverage_low, coverage_high, coverage_valid, coverage_valid_error, fit_failure, ul_failure;
		int total, fits, limits, covered, boundaries, ambiguous;
		TTree tree("summary", "Toy MC summary for each injected mu");
		tree.Branch("mu_injected", &mu);
		tree.Branch("total", &total);
		tree.Branch("fits", &fits);
		tree.Branch("limits", &limits);
		tree.Branch("covered", &covered);
		tree.Branch("boundaries", &boundaries);
		tree.Branch("ambiguous", &ambiguous);
		tree.Branch("mean", &mean);
		tree.Branch("mean_error", &mean_error);
		tree.Branch("bias", &bias);
		tree.Branch("ul_median", &median);
		tree.Branch("ul_minus1", &minus1);
		tree.Branch("ul_plus1", &plus1);
		tree.Branch("ul_minus2", &minus2);
		tree.Branch("ul_plus2", &plus2);
		tree.Branch("coverage", &coverage);
		tree.Branch("coverage_error", &coverage_error);
		tree.Branch("coverage_valid", &coverage_valid);
		tree.Branch("coverage_valid_error", &coverage_valid_error);
		tree.Branch("coverage_low", &coverage_low);
		tree.Branch("coverage_high", &coverage_high);
		tree.Branch("fit_failure", &fit_failure);
		tree.Branch("ul_failure", &ul_failure);
		TGraphErrors mean_graph, bias_graph, coverage_graph, fit_failure_graph, ul_failure_graph;
		TGraphAsymmErrors band1, band2, coverage_bounds;
		mean_graph.SetNameTitle("fitted_mu_mean", "Successful fits;Injected #mu;Mean fitted #mu");
		bias_graph.SetNameTitle("fitted_mu_bias", "Successful fits;Injected #mu;Mean fitted #mu - injected #mu");
		coverage_graph.SetNameTitle("coverage_valid", "Coverage among successful UL calculations;Injected #mu;Coverage");
		coverage_bounds.SetNameTitle("coverage_bounds", "Coverage bounds including failed UL calculations;Injected #mu;Coverage");
		fit_failure_graph.SetNameTitle("fit_failure", "Fit failure fraction;Injected #mu;Fraction");
		ul_failure_graph.SetNameTitle("ul_failure", "Upper limit failure fraction;Injected #mu;Fraction");
		band1.SetNameTitle("ul_68", "Successful 90% CLs upper limits;Injected #mu;Upper limit on #mu");
		band2.SetNameTitle("ul_95", "Successful 90% CLs upper limits;Injected #mu;Upper limit on #mu");
		std::ofstream text(std::string(argv[2]) + "/ToyMC.txt");
		if (!text) throw std::runtime_error("cannot create summary text file");
		text << std::setprecision(10);
		text << "# Mean and UL quantiles use successful calculations. coverage is NaN if any UL failed.\n";
		text << "# coverage_low/high bound full-ensemble coverage by treating failed ULs as all excluded/all covered.\n";
		text << "# mu total fits limits mean mean_error bias ul_median ul_2.5% ul_16% ul_84% ul_97.5% coverage coverage_error coverage_valid coverage_valid_error coverage_low coverage_high fit_failure ul_failure boundaries ambiguous\n";
		for (int i = 0; i <= 20; i++) {
			ToySummary& summary = summaries.at(i);
			mu = i * 0.25;
			total = summary.total;
			fits = summary.fits;
			limits = summary.limits;
			covered = summary.covered;
			boundaries = summary.boundaries;
			ambiguous = summary.ambiguous;
			mean = mean_error = bias = median = minus1 = plus1 = minus2 = plus2 = coverage = coverage_error = coverage_valid = coverage_valid_error = std::numeric_limits<double>::quiet_NaN();
			if (fits > 0) {
				mean = summary.sum / fits;
				if (fits > 1) mean_error = std::sqrt(std::max(0.0, (summary.sum2 - summary.sum * summary.sum / fits) / (fits - 1)) / fits);
				bias = mean - mu;
				int point = mean_graph.GetN();
				mean_graph.SetPoint(point, mu, mean);
				mean_graph.SetPointError(point, 0.0, fits > 1 ? mean_error : 0.0);
				bias_graph.SetPoint(point, mu, bias);
				bias_graph.SetPointError(point, 0.0, fits > 1 ? mean_error : 0.0);
			}
			if (limits > 0) {
				double probabilities[5] = { 0.025, 0.16, 0.5, 0.84, 0.975 };
				double quantiles[5];
				TMath::Quantiles(limits, 5, summary.upper_limits.data(), quantiles, probabilities, false);
				minus2 = quantiles[0]; minus1 = quantiles[1]; median = quantiles[2]; plus1 = quantiles[3]; plus2 = quantiles[4];
				int point = band1.GetN();
				band1.SetPoint(point, mu, median);
				band2.SetPoint(point, mu, median);
				band1.SetPointError(point, 0.0, 0.0, median - minus1, plus1 - median);
				band2.SetPointError(point, 0.0, 0.0, median - minus2, plus2 - median);
				coverage_valid = (double)covered / limits;
				coverage_valid_error = std::sqrt(coverage_valid * (1.0 - coverage_valid) / limits);
				point = coverage_graph.GetN();
				coverage_graph.SetPoint(point, mu, coverage_valid);
				coverage_graph.SetPointError(point, 0.0, coverage_valid_error);
			}
			if (limits == total) { coverage = coverage_valid; coverage_error = coverage_valid_error; }
			coverage_low = (double)covered / total;
			coverage_high = (double)(covered + total - limits) / total;
			coverage_bounds.SetPoint(i, mu, 0.5 * (coverage_low + coverage_high));
			coverage_bounds.SetPointError(i, 0.0, 0.0, 0.5 * (coverage_high - coverage_low), 0.5 * (coverage_high - coverage_low));
			fit_failure = (double)(total - fits) / total;
			ul_failure = (double)(total - limits) / total;
			fit_failure_graph.SetPoint(i, mu, fit_failure);
			ul_failure_graph.SetPoint(i, mu, ul_failure);
			tree.Fill();
			text << mu << ' ' << total << ' ' << fits << ' ' << limits << ' ' << mean << ' ' << mean_error << ' ' << bias << ' ' << median << ' ' << minus2 << ' ' << minus1 << ' ' << plus1 << ' ' << plus2 << ' ' << coverage << ' ' << coverage_error << ' ' << coverage_valid << ' ' << coverage_valid_error << ' ' << coverage_low << ' ' << coverage_high << ' ' << fit_failure << ' ' << ul_failure << ' ' << boundaries << ' ' << ambiguous << '\n';
			printf("injected mu: %lf, fitted mean: %lf +- %lf, valid limits: %d/%d, coverage bounds: [%lf, %lf]\n", mu, mean, mean_error, limits, total, coverage_low, coverage_high);
		}
		text.close();
		if (!text) throw std::runtime_error("failed to write summary text");
		Draw_ToyGraph(mean_graph, argv[2], "fitted_mu_mean", true);
		Draw_ToyGraph(bias_graph, argv[2], "fitted_mu_bias");
		coverage_graph.SetMinimum(0.0);
		coverage_graph.SetMaximum(1.05);
		Draw_ToyGraph(coverage_graph, argv[2], "coverage_valid", false, 0.9);
		Draw_ToyGraph(fit_failure_graph, argv[2], "fit_failure");
		Draw_ToyGraph(ul_failure_graph, argv[2], "ul_failure");
		TCanvas c("bands", "bands", 696, 472);
		band2.SetFillColor(kYellow);
		band1.SetFillColor(kGreen + 1);
		TText message;
		if (band2.GetN() > 0) {
			band2.Draw("A3");
			band1.Draw("3 SAME");
			band1.Draw("LX SAME");
		}
		else {
			c.DrawFrame(0.0, 0.0, 5.0, 1.0, band2.GetTitle());
			message.DrawTextNDC(0.25, 0.5, "No successful upper limits");
		}
		TLegend legend(0.15, 0.7, 0.45, 0.88);
		legend.AddEntry(&band1, "Median and 68% interval", "lf");
		legend.AddEntry(&band2, "95% interval", "f");
		legend.Draw();
		c.SaveAs((std::string(argv[2]) + "/upper_limit.png").c_str());
		c.Clear();
		coverage_bounds.SetMinimum(0.0);
		coverage_bounds.SetMaximum(1.05);
		coverage_bounds.SetMarkerStyle(20);
		coverage_bounds.Draw("AP");
		TLine line(0.0, 0.9, 5.0, 0.9);
		line.SetLineStyle(2);
		line.Draw();
		c.SaveAs((std::string(argv[2]) + "/coverage_bounds.png").c_str());
		band1.Write(); band2.Write(); coverage_bounds.Write();
		tree.Write();
		output.Write();
		output.Flush();
		if (output.TestBit(TFile::kWriteError)) throw std::runtime_error("failed to write summary ROOT file");
		printf("Successfully completed ReadToyMC.\n");
	}
	catch (const std::exception& error) {
		printf("[ERROR] %s\n", error.what());
		return 1;
	}
	return 0;
}
