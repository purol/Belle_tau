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
    * argv[1]: input path
    * argv[2]: input file name
    * argv[3]: output path
    */

    ObtainWeight = MyScaleFunction_halfsplit;

    Loader loader("tau_lfv");

    loader.Load(argv[1], argv[2], "label");

    Module::Module* temp_module = new Module::ReadAutogluonCSVandWrite(std::string(argv[3]) + "/" + argv[2] + ".root.csv", loader.Getvariable_names_address(), loader.VariableTypes_address());
    loader.InsertCustomizedModule(temp_module);

    loader.PrintSeparateRootFile(argv[3], "", "");

    loader.end();

    return 0;
}
