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


    Loader loader("tau_lfv");

    loader.Load(argv[1], argv[2], "label");

    loader.PrintEvent({ "__experiment__", "__run__", "__event__", "__production__", "__ncandidates__" });

    loader.end();

    return 0;
}
