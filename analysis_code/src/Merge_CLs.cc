#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <RooStats/HypoTestInverterResult.h>
#include <RooStats/SamplingDistribution.h>
#include <RooStats/HypoTestInverterPlot.h>
#include <RooStats/SamplingDistPlot.h>

#include <TFile.h>
#include <TMath.h>
#include <TCanvas.h>

void GetExpectedCL(RooStats::HypoTestInverterResult* fResults, const char* mu, const char* path) {
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
		fp = fopen((std::string(path) + "/CLs_freq_" + std::string(mu) + ".txt").c_str(), "a");
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

void GetObservedCLs(RooStats::HypoTestInverterResult* fResults, const char* mu, const char* path, int type = 0) {
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
		fp = fopen((std::string(path) + "/CLs_freq_" + std::string(mu) + ".txt").c_str(), "a");
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

void PrintTestStat(RooStats::HypoTestInverterResult* result, std::string mu_string, const char* path) {
	TCanvas* c = new TCanvas("c", "c", 696, 472);

	HypoTestInverterPlot* plot = new HypoTestInverterPlot("plot", "plot", result);
	SamplingDistPlot* pl = plot->MakeTestStatPlot(0);
	pl->SetLogYaxis(true);
	pl->Draw();

	c->SaveAs((std::string(path) + "/TestStat_" + mu_string + ".png").c_str());

	delete plot;
	delete c;

}

int main(int argc, char* argv[]) {
	/*
	* argv[1]: Hypotestinverter root path
	*/

	const double mu_max = 5.0;

	double mu = 0.0;

	mu = 0.0;
	while (mu < mu_max) {
		std::stringstream stream;
		stream << std::fixed << std::setprecision(1) << mu;
		std::string mu_string = stream.str();

		std::vector<std::string> names;
		load_files(argv[1], &names, ("Hypotestinverter_freq_" + mu_string + "_").c_str());

		RooStats::HypoTestInverterResult* result = nullptr;

		for (unsigned int i = 0; i < names.size(); i++) {
			TFile* file = TFile::Open(names.at(i).c_str());
			if (result == nullptr) result = (RooStats::HypoTestInverterResult*)file->Get("result_mu");
			else {
				RooStats::HypoTestInverterResult* result_temp = (RooStats::HypoTestInverterResult*)file->Get("result_mu");
				result->Add(*result_temp);
			}
			file->Close();
		}

		// print result
		if (result != nullptr) {
			GetExpectedCL(result, mu_string.c_str());
			GetObservedCLs(result, mu_string.c_str(), 0);
			GetObservedCLs(result, mu_string.c_str(), 1);
			GetObservedCLs(result, mu_string.c_str(), 2);
			PrintTestStat(result, mu_string);
		}

		mu = mu + 0.1;
	}

	return 0;
}