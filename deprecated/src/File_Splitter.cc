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
    * argv[2]: filename
    * argv[3]: split_num
    * argv[4]: selected_index
    * argv[5]: output path
    */

    Loader loader("tau_lfv");

    // loader.Load(argv[1], argv[2], "label");
    loader.LoadWithCut(argv[1], argv[2], "label", "(0.5 < extraInfo__bodecayModeID__bc) && (extraInfo__bodecayModeID__bc < 1.5)");

    loader.RandomEventSelection(atoi(argv[3]), atoi(argv[4]));

    loader.PrintSeparateRootFile(argv[5], "", ("split_" + std::string(argv[3]) + "_" + std::string(argv[4])).c_str());

    loader.end();

    return 0;
}
