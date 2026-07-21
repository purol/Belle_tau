#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"
#include "MyObtainWeight.h"
#include "MyModule.h"

int main(int argc, char* argv[]) {
    /*
    * argv[1]: dirname
    * argv[2]: including string
    * argv[3]: output path
    */

    Loader loader_signal("gen_info");
    loader_signal.LoadWithCut(argv[1], argv[2], "label", "nParticlesInList__botau__pl__clLFV_comb__bc > 0.5");
    loader_signal.PrintSeparateRootFile((std::string(argv[3]) + "/signal").c_str(), "signal_", "");
    loader_signal.end();

    Loader loader_bkg("gen_info");
    loader_bkg.LoadWithCut(argv[1], argv[2], "label", "nParticlesInList__botau__pl__clLFV_comb__bc < 0.5");
    loader_bkg.PrintSeparateRootFile((std::string(argv[3]) + "/background").c_str(), "background_", "");
    loader_bkg.end();

    return 0;
}
