#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <cmath>
#include <limits>

#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TROOT.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TText.h>
#include <TLegend.h>
#include <TMath.h>

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

void Draw_ToyGraph(TGraphErrors& graph, const char* path, const char* filename, double mu_max, bool diagonal = false, double reference = 0.0) {
	TCanvas c("c", "c", 696, 472);
	graph.SetMarkerStyle(20);
	graph.SetLineWidth(2);
	TText message;
	if (graph.GetN() > 0) graph.Draw("AP");
	else {
		c.DrawFrame(0.0, 0.0, mu_max, 1.05, graph.GetTitle());
		message.DrawTextNDC(0.25, 0.5, "No successful calculations");
	}
	TLine line(0.0, reference, mu_max, diagonal ? mu_max : reference);
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
	* argv[4]: injected mu step
	*/

	if (argc != 5) {
		printf("[ERROR] expected ToyMC root path, output path, number of toys per injected mu and injected mu step\n");
		return 1;
	}
	int expected = std::stoi(argv[3]);
	if (expected < 1 || expected > 10000) {
		printf("[ERROR] invalid expected toy count\n");
		return 1;
	}
	double injection_step = std::stod(argv[4]);
	double mu_max = 10.0 * injection_step;
	if (!std::isfinite(injection_step) || injection_step <= 0.0 || !std::isfinite(mu_max)) {
		printf("[ERROR] invalid injected mu step\n");
		return 1;
	}
	gROOT->SetBatch(true);
	std::vector<ToySummary> summaries(11);
	TChain toys("toys");
	TChain scans("scan");
	std::string files = std::string(argv[1]) + "/ToyMC_*.root";
	toys.Add(files.c_str());
	scans.Add(files.c_str());

	// read toy results
	double injected, fitted, upper, low, high;
	int index, fit_status, ul_status, at_boundary;
	toys.SetBranchAddress("mu_injected", &injected);
	toys.SetBranchAddress("toy_index", &index);
	toys.SetBranchAddress("mu_fitted", &fitted);
	toys.SetBranchAddress("fit_status", &fit_status);
	toys.SetBranchAddress("mu_up", &upper);
	toys.SetBranchAddress("mu_up_low", &low);
	toys.SetBranchAddress("mu_up_high", &high);
	toys.SetBranchAddress("ul_status", &ul_status);
	toys.SetBranchAddress("at_boundary", &at_boundary);
	for (Long64_t i = 0; i < toys.GetEntries(); i++) {
		if (toys.GetEntry(i) <= 0) {
			printf("[ERROR] cannot read toy entry %lld\n", i);
			return 1;
		}
		if (!std::isfinite(injected) || injected < 0.0 || injected > mu_max || index < 0 || index >= expected) {
			printf("[ERROR] invalid injected mu or toy index\n");
			return 1;
		}
		int mu_index = (int)std::lround(injected / injection_step);
		if (mu_index < 0 || mu_index > 10 || std::abs(injected - mu_index * injection_step) > 1e-8) {
			printf("[ERROR] injected mu does not match the requested step\n");
			return 1;
		}
		ToySummary& summary = summaries.at(mu_index);
		if (!summary.indices.insert(index).second) {
			printf("[ERROR] duplicate toy %d at injected mu %lf\n", index, injected);
			return 1;
		}
		summary.total++;
		if (fit_status == 0 && std::isfinite(fitted)) {
			summary.fits++;
			summary.sum += fitted;
			summary.sum2 += fitted * fitted;
			if (at_boundary == 1) summary.boundaries++;
		}
		if (fit_status == 0 && ul_status == 0 && std::isfinite(upper)) {
			summary.limits++;
			if (upper >= injected) summary.covered++;
			if (low < injected && injected <= high) summary.ambiguous++;
			summary.upper_limits.push_back(upper);
		}
	}
	for (int i = 0; i < 11; i++) {
		if (summaries.at(i).total != expected) {
			printf("[ERROR] missing toys at injected mu %lf\n", i * injection_step);
			return 1;
		}
	}

	/* ======================== SUMMARY ======================== */

	TFile output((std::string(argv[2]) + "/ToyMC.root").c_str(), "RECREATE");
	if (output.IsZombie()) {
		printf("[ERROR] cannot create summary ROOT file\n");
		return 1;
	}
	if (!toys.CloneTree(-1, "fast") || !scans.CloneTree(-1, "fast")) {
		printf("[ERROR] cannot merge toy trees\n");
		return 1;
	}
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
	if (!text) {
		printf("[ERROR] cannot create summary text file\n");
		return 1;
	}
	text << std::setprecision(10);
	text << "# Mean and UL quantiles use successful calculations. coverage is NaN if any UL failed.\n";
	text << "# coverage_low/high bound full-ensemble coverage by treating failed ULs as all excluded/all covered.\n";
	text << "# mu total fits limits mean mean_error bias ul_median ul_2.5% ul_16% ul_84% ul_97.5% coverage coverage_error coverage_valid coverage_valid_error coverage_low coverage_high fit_failure ul_failure boundaries ambiguous\n";
	for (int i = 0; i < 11; i++) {
		ToySummary& summary = summaries.at(i);
		mu = i * injection_step;
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
	if (!text) {
		printf("[ERROR] failed to write summary text\n");
		return 1;
	}
	Draw_ToyGraph(mean_graph, argv[2], "fitted_mu_mean", mu_max, true);
	Draw_ToyGraph(bias_graph, argv[2], "fitted_mu_bias", mu_max);
	coverage_graph.SetMinimum(0.0);
	coverage_graph.SetMaximum(1.05);
	Draw_ToyGraph(coverage_graph, argv[2], "coverage_valid", mu_max, false, 0.9);
	Draw_ToyGraph(fit_failure_graph, argv[2], "fit_failure", mu_max);
	Draw_ToyGraph(ul_failure_graph, argv[2], "ul_failure", mu_max);
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
		c.DrawFrame(0.0, 0.0, mu_max, 1.0, band2.GetTitle());
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
	TLine line(0.0, 0.9, mu_max, 0.9);
	line.SetLineStyle(2);
	line.Draw();
	c.SaveAs((std::string(argv[2]) + "/coverage_bounds.png").c_str());
	band1.Write(); band2.Write(); coverage_bounds.Write();
	tree.Write();
	output.Write();
	output.Flush();
	if (output.TestBit(TFile::kWriteError)) {
		printf("[ERROR] failed to write summary ROOT file\n");
		return 1;
	}
	printf("Successfully completed ReadToyMC.\n");
	return 0;
}
