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
    * argv[2]: FOM filename
    * argv[3]: resolution file path
    */

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

    ReadResolution((std::string(argv[3]) + "/M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    double BDT_cut_1 = -1;

    ReadFOM(argv[2], &BDT_cut_1);

    // assume -5 delta < deltaE < 5 delta is already applied
    std::string region_A = "(" + std::to_string(M_peak - 5 * M_left_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak + 5 * M_right_sigma) + ") && ( BDT_output_1 > " + std::to_string(BDT_cut_1) + ")";
    std::string region_B = "(((" + std::to_string(M_peak - 10 * M_left_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak - 5 * M_left_sigma) + ")) || ((" + std::to_string(M_peak + 5 * M_right_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak + 10 * M_right_sigma) + ")))" + "&& ( BDT_output_1 > " + std::to_string(BDT_cut_1) + ")";
    std::string region_C = "(" + std::to_string(M_peak - 5 * M_left_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak + 5 * M_right_sigma) + ") && ( 0.5 < BDT_output_1 ) && ( BDT_output_1 < " + std::to_string(BDT_cut_1) + ")";
    std::string region_D = "(((" + std::to_string(M_peak - 10 * M_left_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak - 5 * M_left_sigma) + ")) || ((" + std::to_string(M_peak + 5 * M_right_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak + 10 * M_right_sigma) + ")))" + " && ( 0.5 < BDT_output_1 ) && ( BDT_output_1 < " + std::to_string(BDT_cut_1) + ")";

    std::string region_Aprime = "(((" + std::to_string(M_peak - 7.5 * M_left_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak - 5 * M_left_sigma) + ")) || ((" + std::to_string(M_peak + 5 * M_right_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak + 7.5 * M_right_sigma) + ")))" + "&& ( BDT_output_1 > " + std::to_string(BDT_cut_1) + ")";
    std::string region_Bprime = "(((" + std::to_string(M_peak - 10 * M_left_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak - 7.5 * M_left_sigma) + ")) || ((" + std::to_string(M_peak + 7.5 * M_right_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak + 10 * M_right_sigma) + ")))" + "&& ( BDT_output_1 > " + std::to_string(BDT_cut_1) + ")";
    std::string region_Cprime = "(((" + std::to_string(M_peak - 7.5 * M_left_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak - 5 * M_left_sigma) + ")) || ((" + std::to_string(M_peak + 5 * M_right_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak + 7.5 * M_right_sigma) + ")))" + " && ( 0.5 < BDT_output_1 ) && ( BDT_output_1 < " + std::to_string(BDT_cut_1) + ")";
    std::string region_Dprime = "(((" + std::to_string(M_peak - 10 * M_left_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak - 7.5 * M_left_sigma) + ")) || ((" + std::to_string(M_peak + 7.5 * M_right_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak + 10 * M_right_sigma) + ")))" + " && ( 0.5 < BDT_output_1 ) && ( BDT_output_1 < " + std::to_string(BDT_cut_1) + ")";

    ObtainWeight = MyScaleFunction_halfsplit;

    Loader loader("tau_lfv");

    for (int i = 0; i < background_list.size(); i++) loader.Load((argv[1] + std::string("/") + background_list.at(i) + std::string("/final_output_test_after_application/")).c_str(), "root", background_list.at(i).c_str());

    loader.Cut(("(" + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + "< deltaE) && (deltaE < " + std::to_string(deltaE_peak + 5 * deltaE_right_sigma) + ")").c_str());
    loader.PrintInformation("========== -5 delta < deltaE < 5 delta ==========");
    loader.ABCDmethod(region_A.c_str(), region_B.c_str(), region_C.c_str(), region_D.c_str(), region_Aprime.c_str(), region_Bprime.c_str(), region_Cprime.c_str(), region_Dprime.c_str(), false);

    loader.end();

    return 0;
}
