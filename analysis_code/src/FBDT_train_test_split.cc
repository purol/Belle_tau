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

    ObtainWeight = MyScaleFunction;

    Loader loader("tau_lfv");

    loader.Load(argv[1], argv[2], "label");

    loader.RandomEventSelection(atoi(argv[3]), atoi(argv[4]));

    loader.PrintInformation("========== random event selection ==========");

    loader.PrintSeparateRootFile(argv[5], "", ("split_" + std::string(argv[3]) + "_" + std::string(argv[4])).c_str());

    loader.end();

    return 0;
}
