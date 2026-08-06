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

    ReadFOM(argv[5], &BDT_cut_1);
    ReadFOM(argv[6], &BDT_cut_2);

    EventWeights::Register("MC_weight", MC_weight);
    EventWeights::Register("double_weight", double_weight);

    Loader loader("tau_lfv");

    loader.Load(argv[1], argv[2], "label");
    loader.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"} });
    loader.AddWeight("double_weight");

    loader.PrintInformation("========== initial ==========");

    std::string cut_BDT_1 = "(" + std::to_string(BDT_cut_1) + " < BDT_output_1)";
    std::string cut_M_1 = "((" + std::to_string(M_peak - 20 * M_left_sigma) + " < M) && (M < " + std::to_string(M_peak + 20 * M_right_sigma) + "))";
    std::string cut_deltaE_1 = "((" + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak + 6 * deltaE_right_sigma) + "))";
    std::string cut_M_deltaE_1 = "(" + cut_M_1 + "&&" + cut_deltaE_1 + ")";
    std::string cut_total_1 = "(" + cut_M_deltaE_1 + "&&" + cut_BDT_1 + ")";

    std::string cut_BDT_2 = "(" + std::to_string(BDT_cut_2) + " < BDT_output_2)";
    std::string cut_M_2 = "((" + std::to_string(M_peak - 20 * M_left_sigma) + " < M) && (M < " + std::to_string(M_peak + 20 * M_right_sigma) + "))";
    std::string cut_deltaE_2 = "((" + std::to_string(deltaE_peak - 15 * deltaE_left_sigma) + "<= deltaE) && (deltaE < " + std::to_string(deltaE_peak - 5 * deltaE_left_sigma) + "))";
    std::string cut_M_deltaE_2 = "(" + cut_M_2 + "&&" + cut_deltaE_2 + ")";
    std::string cut_total_2 = "(" + cut_M_deltaE_2 + "&&" + cut_BDT_2 + ")";

    std::string cut_region = cut_M_deltaE_1 + "||" + cut_M_deltaE_2;
    std::string cut_total = cut_total_1 + "||" + cut_total_2;

    loader.Cut(cut_region.c_str());
    loader.PrintInformation("========== (-20 delta < M < 20 delta) && (-15 delta < deltaE < 6 delta) ==========");

    loader.RandomBCS();
    loader.IsBCSValid();
    loader.PrintInformation("========== Random BCS ==========");

    loader.Cut(cut_total.c_str());
    loader.PrintInformation(("========== BDT1 > " + std::to_string(BDT_cut_1) + ", BDT2 > " + std::to_string(BDT_cut_2) + " ==========").c_str());

    loader.Cut(("deltaE < " + std::to_string(deltaE_peak + 5 * deltaE_right_sigma)).c_str());
    loader.PrintInformation("========== deltaE < 5 delta) ==========");

    // loader.PrintSeparateRootFile(argv[3], "", "");

    loader.end();

    return 0;
}
