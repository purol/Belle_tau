#include <stdio.h>
#include <string>
#include <vector>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"

int main(int argc, char* argv[]) {
    /*
    * argv[1]: dirname
    * argv[2]: including string
    * argv[3]: output path
    */

    EventWeights::Register("MC_weight", MC_weight);

    Loader loader("tau_lfv");

    loader.Load(argv[1], argv[2], "label");
    loader.AddWeight("MC_weight", { {"MySampleType", "MySampleType"}, {"MyEventType", "MyEventType"}, {"MyEnergyType", "MyEnergyType"}, {"MyALPLife", "MyALPLife"} });
    loader.Cut("(0.5 < extraInfo__bodecayModeID__bc) && (extraInfo__bodecayModeID__bc < 1.5)"); // select tau -> mu mu mu only

    loader.PrintSeparateRootFile((std::string(argv[3]) + "/preselection_output").c_str(), "", "");

    loader.end();

    return 0;
}
