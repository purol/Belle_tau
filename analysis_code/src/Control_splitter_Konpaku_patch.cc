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
    * argv[3]: signal output path
    * argv[4]: background output path
    */

    Loader toSignal("tau_lfv");
    toSignal.LoadWithCut(argv[1], argv[2], "label", "(nParticlesInList__botau__pl__clpipipi__bc > 0.5) || (nParticlesInList__botau__pl__cldirect__bc > 0.5)");
    toSignal.PrintSeparateRootFile(argv[3], "SIGNAL_", "");
    toSignal.end();

    Loader toTau("tau_lfv");
    toTau.LoadWithCut(argv[1], argv[2], "label", "(nParticlesInList__botau__pl__clpipipi__bc < 0.5) && (nParticlesInList__botau__pl__cldirect__bc < 0.5)");
    toTau.PrintSeparateRootFile(argv[4], "TAUPAIR_", "");
    toTau.end();

    return 0;
}
