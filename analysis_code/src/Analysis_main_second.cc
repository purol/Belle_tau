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
    */

    double deltaE_peak;
    double deltaE_left_sigma;
    double deltaE_right_sigma;
    double M_peak;
    double M_left_sigma;
    double M_right_sigma;
    double theta;

    ReadResolution((std::string(argv[4]) + "/M_deltaE_result.txt").c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

    ObtainWeight = MyScaleFunction;

    Loader loader("tau_lfv");

    loader.Load(argv[1], argv[2], "label");

    loader.PrintInformation("========== initial ==========");

    loader.Cut(("(deltaE < " + std::to_string(deltaE_peak + 20 * deltaE_right_sigma) + ")").c_str());
    loader.PrintInformation("========== deltaE < 20 delta ==========");
    loader.Cut(("(" + std::to_string(M_peak - 20 * M_left_sigma) + "< M_inv_tau) && (M_inv_tau < " + std::to_string(M_peak + 20 * M_right_sigma) + ")").c_str());
    loader.PrintInformation("========== -20 delta < M < 20 delta ==========");
    //loader.DrawTH2D("(E*E-px*px-py*py-pz*pz)^0.5", "deltaE", ";M [GeV];deltaE [GeV];", 50, 1.3, 1.9, 50, -0.9, 0.4, "M_deltaE_before_cut.png");

    loader.PrintSeparateRootFile((std::string(argv[3]) + "/final_output").c_str(), "", "");

    loader.end();

    return 0;
}
