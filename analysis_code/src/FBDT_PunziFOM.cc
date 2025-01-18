#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "MyModule.h"
#include "base.h"

int main(int argc, char* argv[]) {
    /*
    * argv[1]: input path
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

    ReadResolution((std::string(argv[1]) + "/M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma);

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

    loader.Cut(("(deltaE < " + std::to_string(deltaE_peak + 5 * deltaE_right_sigma) + ")").c_str());
    loader.PrintInformation("========== deltaE < 5 delta ==========");
    loader.Cut(("(" + std::to_string(M_peak - 5 * M_left_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak + 5 * M_right_sigma) + ")").c_str());
    loader.PrintInformation("========== -5 delta < M < 5 delta ==========");

    loader.DrawPunziFOM("BDT_output", 0.0, 1.0, Nevt_SIGNAL, 1.28, (std::string(argv[1])  + "/GridSearch/PunziFOM.png").c_str());

    loader.end();

    return 0;
}
