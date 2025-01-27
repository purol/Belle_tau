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
    * argv[5]: FOM_1 file path
    * argv[6]: FOM_2 file path
    */

    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution((std::string(argv[4]) + "/M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    double BDT_cut_1 = -1;
    double BDT_cut_2 = -1;

    ReadFOM(argv[5], BDT_cut_1);
    ReadFOM(argv[6], BDT_cut_2);

    ObtainWeight = MyScaleFunction_halfsplit;

    Loader loader("tau_lfv");

    loader.Load(argv[1], argv[2], "label");

    loader.PrintInformation("========== initial ==========");

    std::string cut_BDT_1 = "((deltaE >= " + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + ") && ( BDT_output_1 > " + std::to_string(BDT_cut_1) + "))";
    std::string cut_BDT_2 = "((deltaE < " + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + ") && ( BDT_output_2 > " + std::to_string(BDT_cut_2) + "))";
    std::string cut_total = cut_BDT_1 + "||" + cut_BDT_2;

    loader.Cut(cut_total.c_str());
    loader.PrintInformation("========== BDT1 > " + std::to_string(BDT_cut_1) + ", BDT2 > " + std::to_string(BDT_cut_2) + " ==========");

    loader.PrintSeparateRootFile(argv[3], "", "");

    loader.end();

    return 0;
}
