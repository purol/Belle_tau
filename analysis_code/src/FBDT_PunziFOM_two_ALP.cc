#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <format>

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
    * argv[4]: mass
    * argv[5]: lifetime
    * argv[6]: A constant
    * argv[7]: B constant
    */

    double mass = std::stod(argv[4]);
    double life = std::stod(argv[5]);
    int A = std::stoi(argv[6]);
    int B = std::stoi(argv[7]);

    double M_left_cut_value = 0;
    double M_right_cut_value = 0;
    if ((0 < life) && (life < 0.7)) {
        M_left_cut_value = 0.025;
        M_right_cut_value = 0.025;
    }
    else if ((0.7 <= life) && (life < 7)) {
        M_left_cut_value = 0.03;
        M_right_cut_value = 0.03;
    }
    else if ((7 <= life) && (life < 70)) {
        M_left_cut_value = 0.035;
        M_right_cut_value = 0.035;

    }
    else if (70 <= life) {
        M_left_cut_value = 0.075;
        M_right_cut_value = 0.075;

    }

    std::vector<std::string> signal_list = { "ALP" };
    std::vector<std::string> background_list = { "BBs", "BsBs", "CHARM", "CHG", "DDBAR",
        "EE", "EEEE", "EEKK", "EEMUMU", "EEPIPI",
        "EEPP", "EETAUTAU", "GG", "K0K0BARISR", "KKISR",
        "MIX", "MUMU", "MUMUMUMU", "MUMUTAUTAU", "PIPIPI0ISR",
        "PIPIISR", "SSBAR", "TAUPAIR", "TAUTAUTAUTAU", "UUBAR" };

    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution((std::string(argv[1]) + "/alpha_mass" + std::format("{:g}", mass) + "_life" + std::format("{:g}", life) + "_A" + std::to_string(A) + "_B" + std::to_string(B) + "_M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

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

    loader.Cut("(0.5 < MyEnergyType) && (MyEnergyType < 1.5)");
    loader.PrintInformation("========== 4S Energy ==========");
    // cut on deltaE
    loader.Cut(("(" + std::to_string(deltaE_peak - 15 * deltaE_left_sigma) + "< deltaE) && (deltaE < " + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + ")").c_str());
    loader.PrintInformation("========== -15 delta < deltaE < -5 delta ==========");
    loader.Cut(("(" + std::to_string(M_peak - 5 * M_left_sigma) + "< M) && (M < " + std::to_string(M_peak + 5 * M_right_sigma) + ")").c_str());
    loader.PrintInformation("========== -5 delta < M < 5 delta ==========");
    loader.Cut(("(" + std::to_string(mass - M_left_cut_value) + "< extraInfo__boALP_M__bc) && (extraInfo__boALP_M__bc <" + std::to_string(mass + M_right_cut_value) + ")").c_str());
    loader.PrintInformation(("========== nominal_mass - " + std::to_string(M_left_cut_value) + " < M_alp < nominal_mass + " + std::to_string(M_right_cut_value) + " ==========").c_str());

    loader.DrawPunziFOM(argv[2], 0.0, 1.0, 100, Nevt_SIGNAL, 1.28, 1, (std::string(argv[1]) + "/" + std::string(argv[3]) + "/alpha_mass" + std::format("{:g}", mass) + "_life" + std::format("{:g}", life) + "_A" + std::to_string(A) + "_B" + std::to_string(B) + "_PunziFOM.png").c_str());

    loader.end();

    return 0;
}
