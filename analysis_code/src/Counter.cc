#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "MyModule.h"
#include "functions.h"

int main(int argc, char* argv[]) {
    /*
    * argv[1]: dirname
    * argv[2]: including string
    * argv[3]: resolution file path
    */

    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution((std::string(argv[3]) + "/M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    ObtainWeight = MyScaleFunction_halfsplit;

    Loader loader_one("tau_lfv");
    loader_one.Load(argv[1], argv[2], "label");
    loader_one.PrintInformation("========== initial ==========");
    loader_one.Cut(("(" + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + "< deltaE) && (deltaE < " + std::to_string(deltaE_peak + 5 * deltaE_right_sigma) + ")").c_str());
    loader_one.PrintInformation("========== -5 delta < deltaE < 5 delta ==========");
    loader_one.Cut(("(" + std::to_string(M_peak - 5 * M_left_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak + 5 * M_right_sigma) + ")").c_str());
    loader_one.PrintInformation("========== -5 delta < M < 5 delta ==========");
    loader_one.end();

    Loader loader_two("tau_lfv");
    loader_two.Load(argv[1], argv[2], "label");
    loader_two.PrintInformation("========== initial ==========");
    loader.Cut(("(deltaE < " + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + ")").c_str());
    loader.PrintInformation("========== deltaE < -5 delta ==========");
    double sloop_1 = 10 * deltaE_left_sigma / M_right_sigma;
    double const_term_1 = -M_peak * (10 * deltaE_left_sigma / M_right_sigma) + deltaE_peak - 35 * deltaE_left_sigma;
    double sloop_2 = -10 * deltaE_left_sigma / M_left_sigma;
    double const_term_2 = M_peak * (10 * deltaE_left_sigma / M_left_sigma) + deltaE_peak - 35 * deltaE_left_sigma;
    loader_two.Cut(("deltaE > ((" + std::to_string(sloop_1) + ")*M_inv_tau+(" + std::to_string(const_term_1) + "))").c_str());
    loader_two.Cut(("deltaE > ((" + std::to_string(sloop_2) + ")*M_inv_tau+(" + std::to_string(const_term_2) + "))").c_str());
    loader_two.PrintInformation("========== triangle region ==========");
    loader_two.end();

    return 0;
}
