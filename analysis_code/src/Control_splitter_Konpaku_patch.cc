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

    Loader topipipinu("tau_lfv");
    topipipinu.LoadWithCut(argv[1], argv[2], "label", "(nParticlesInList__botau__pl__clpipipi__bc > 0.5) || (nParticlesInList__botau__pl__cldirect__bc > 0.5)");
    topipipinu.LoadWithCut(argv[1], argv[2], "label", "nParticlesInList__botau__pl__clpipipi__bc > 0.5");
    topipipinu.PrintSeparateRootFile(argv[3], "pipipinu_", "");
    topipipinu.end();

    Loader toKS0pinu("tau_lfv");
    toKS0pinu.LoadWithCut(argv[1], argv[2], "label", "(nParticlesInList__botau__pl__clpipipi__bc > 0.5) || (nParticlesInList__botau__pl__cldirect__bc > 0.5)");
    toKS0pinu.LoadWithCut(argv[1], argv[2], "label", "nParticlesInList__botau__pl__cldirect__bc > 0.5");
    toKS0pinu.PrintSeparateRootFile(argv[3], "KS0pinu_", "");
    toKS0pinu.end();

    Loader topurepipipinu("tau_lfv");
    topurepipipinu.LoadWithCut(argv[1], argv[2], "label", "(nParticlesInList__botau__pl__clpipipi__bc > 0.5) || (nParticlesInList__botau__pl__cldirect__bc > 0.5)");
    topurepipipinu.LoadWithCut(argv[1], argv[2], "label", "nParticlesInList__botau__pl__cldirect__bc < 0.5");
    topurepipipinu.PrintSeparateRootFile(argv[3], "pure_pipipinu_", "");
    topurepipipinu.end();

    Loader topureKS0pinu("tau_lfv");
    topureKS0pinu.LoadWithCut(argv[1], argv[2], "label", "(nParticlesInList__botau__pl__clpipipi__bc > 0.5) || (nParticlesInList__botau__pl__cldirect__bc > 0.5)");
    topureKS0pinu.LoadWithCut(argv[1], argv[2], "label", "nParticlesInList__botau__pl__clpipipi__bc < 0.5");
    topureKS0pinu.PrintSeparateRootFile(argv[3], "pure_KS0pinu_", "");
    topureKS0pinu.end();

    Loader toTau("tau_lfv");
    toTau.LoadWithCut(argv[1], argv[2], "label", "(nParticlesInList__botau__pl__clpipipi__bc < 0.5) && (nParticlesInList__botau__pl__cldirect__bc < 0.5)");
    toTau.PrintSeparateRootFile(argv[4], "TAUPAIR_", "");
    toTau.end();

    return 0;
}
