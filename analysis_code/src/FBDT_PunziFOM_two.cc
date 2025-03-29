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
    * argv[1]: input path
    * argv[2]: BDT variable name
    * argv[3]: gridsearch directory name
    */

    std::vector<std::string> signal_list = { "SIGNAL" };
    std::vector<std::string> background_list = { "CHARM", "CHG", "DDBAR", "EE", "EEEE", 
        "EEKK", "EEMUMU", "EEPIPI", "EEPP", "EETAUTAU", "GG", 
        "K0K0BARISR", "KKISR", "MIX", "MUMU", "MUMUMUMU", 
        "MUMUTAUTAU", "PIPIISR", "SSBAR", "TAUPAIR", "TAUTAUTAUTAU", "UUBAR" };

    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution((std::string(argv[1]) + "/M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    ObtainWeight = MyScaleFunction_halfsplit;

    Loader loader("tau_lfv");

    for (int i = 0; i < signal_list.size(); i++) loader.Load((argv[1] + std::string("/") + signal_list.at(i) + std::string("/final_output_test_after_application/")).c_str(), "root", signal_list.at(i).c_str());
    for (int i = 0; i < background_list.size(); i++) loader.Load((argv[1] + std::string("/") + background_list.at(i) + std::string("/final_output_test_after_application/")).c_str(), "root", background_list.at(i).c_str());

    // Create a new vector to hold the combined elements
    std::vector<std::string> all_label;
    all_label.reserve(signal_list.size() + background_list.size());
    all_label.insert(all_label.end(), signal_list.begin(), signal_list.end());
    all_label.insert(all_label.end(), background_list.begin(), background_list.end());

    loader.SetMC(all_label);
    loader.SetData({});
    loader.SetSignal(signal_list);
    loader.SetBackground(background_list);

    // cut on deltaE
    loader.Cut(("(deltaE < " + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + ")").c_str());
    loader.PrintInformation("========== deltaE < -5 delta ==========");

    // cut for triangle region
    double sloop_1 = 10 * deltaE_left_sigma / M_right_sigma;
    double const_term_1 = -M_peak * (10 * deltaE_left_sigma / M_right_sigma) + deltaE_peak - 35 * deltaE_left_sigma;
    double sloop_2 = -10 * deltaE_left_sigma / M_left_sigma;
    double const_term_2 = M_peak * (10 * deltaE_left_sigma / M_left_sigma) + deltaE_peak - 35 * deltaE_left_sigma;
    loader.Cut(("deltaE > ((" + std::to_string(sloop_1) + ")*M_inv_tau+(" + std::to_string(const_term_1) + "))").c_str());
    loader.Cut(("deltaE > ((" + std::to_string(sloop_2) + ")*M_inv_tau+(" + std::to_string(const_term_2) + "))").c_str());
    loader.PrintInformation("========== triangle region ==========");

    loader.DrawPunziFOM(argv[2], 0.0, 1.0, 500, Nevt_SIGNAL, 1.28, (std::string(argv[1])  + "/" + std::string(argv[3]) + "/PunziFOM.png").c_str());

    loader.end();

    return 0;
}
