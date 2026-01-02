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

    double BDT_cut_1 = -1;

    ReadFOM(argv[2], &BDT_cut_1);

    std::string region_A = get_region_one_A((std::string(argv[3]) + "/M_deltaE_result.txt").c_str(), "deltaE", "M_inv_tau", "BDT_output_1", BDT_cut_1);
    std::string region_B = get_region_one_B((std::string(argv[3]) + "/M_deltaE_result.txt").c_str(), "deltaE", "M_inv_tau", "BDT_output_1", BDT_cut_1);
    std::string region_C = get_region_one_C((std::string(argv[3]) + "/M_deltaE_result.txt").c_str(), "deltaE", "M_inv_tau", "BDT_output_1", BDT_cut_1);
    std::string region_D = get_region_one_D((std::string(argv[3]) + "/M_deltaE_result.txt").c_str(), "deltaE", "M_inv_tau", "BDT_output_1", BDT_cut_1);

    std::string region_Aprime = get_region_one_Aprime((std::string(argv[3]) + "/M_deltaE_result.txt").c_str(), "deltaE", "M_inv_tau", "BDT_output_1", BDT_cut_1);
    std::string region_Bprime = get_region_one_Bprime((std::string(argv[3]) + "/M_deltaE_result.txt").c_str(), "deltaE", "M_inv_tau", "BDT_output_1", BDT_cut_1);
    std::string region_Cprime = get_region_one_Cprime((std::string(argv[3]) + "/M_deltaE_result.txt").c_str(), "deltaE", "M_inv_tau", "BDT_output_1", BDT_cut_1);
    std::string region_Dprime = get_region_one_Dprime((std::string(argv[3]) + "/M_deltaE_result.txt").c_str(), "deltaE", "M_inv_tau", "BDT_output_1", BDT_cut_1);

    ObtainWeight = MyScaleFunction_halfsplit;

    Loader loader("tau_lfv");

    for (int i = 0; i < background_list.size(); i++) loader.Load((argv[1] + std::string("/") + background_list.at(i) + std::string("/final_output_test_after_application/")).c_str(), "root", background_list.at(i).c_str());

    loader.ABCDmethod(region_A.c_str(), region_B.c_str(), region_C.c_str(), region_D.c_str(), region_Aprime.c_str(), region_Bprime.c_str(), region_Cprime.c_str(), region_Dprime.c_str(), false);

    loader.end();

    return 0;
}
