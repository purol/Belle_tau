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

    ObtainWeight = MyScaleFunction_halfsplit;

    Loader loader_one("tau_lfv");
    loader_one.Load(argv[1], argv[2], "label");
    loader_one.PrintInformation("========== initial ==========");
    loader_one.Cut(get_region_one((std::string(argv[3]) + "/M_deltaE_result.txt").c_str(), "deltaE", "M_inv_tau").c_str());
    loader_one.PrintInformation("========== (-5 delta < M < 5 delta) && (-5 delta < deltaE < 5 delta) ==========");
    loader_one.end();

    Loader loader_two("tau_lfv");
    loader_two.Load(argv[1], argv[2], "label");
    loader_two.PrintInformation("========== initial ==========");
    loader_two.Cut(get_region_two((std::string(argv[3]) + "/M_deltaE_result.txt").c_str(), "deltaE", "M_inv_tau").c_str());
    loader_two.PrintInformation("========== (-5 delta < M < 5 delta) && (-15 delta < deltaE < -5 delta) ==========");
    loader_two.end();

    return 0;
}
