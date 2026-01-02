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
    * argv[3]: output path
    * argv[4]: resolution file path
    * argv[5]: FOM_1 filename
    * argv[6]: FOM_2 filename
    */

    double BDT_cut_1 = -1;
    double BDT_cut_2 = -1;

    ReadFOM(argv[5], &BDT_cut_1);
    ReadFOM(argv[6], &BDT_cut_2);

    ObtainWeight = MyScaleFunction_halfsplit;

    Loader loader("tau_lfv");

    loader.Load(argv[1], argv[2], "label");

    loader.PrintInformation("========== initial ==========");

    std::string cut_BDT_1 = "(" + get_region_upper((std::string(argv[4]) + "/M_deltaE_result.txt").c_str(), "deltaE", "M_inv_tau") + " && ( BDT_output_1 > " + std::to_string(BDT_cut_1) + "))";
    std::string cut_BDT_2 = "(" + get_region_lower((std::string(argv[4]) + "/M_deltaE_result.txt").c_str(), "deltaE", "M_inv_tau") + " && ( BDT_output_2 > " + std::to_string(BDT_cut_2) + "))";
    std::string cut_total = cut_BDT_1 + "||" + cut_BDT_2;

    loader.Cut(cut_total.c_str());
    loader.PrintInformation(("========== BDT1 > " + std::to_string(BDT_cut_1) + ", BDT2 > " + std::to_string(BDT_cut_2) + " ==========").c_str());

    loader.RandomBCS();
    loader.IsBCSValid();
    loader.PrintInformation("========== Random BCS ==========");

    //loader.PrintSeparateRootFile(argv[3], "", "");

    loader.end();

    return 0;
}
