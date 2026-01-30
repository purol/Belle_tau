#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <filesystem>

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

    std::filesystem::path dir_path = argv[4];
    std::vector<std::string> resolution_filenames;

    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(dir_path)) {
        if (!entry.is_regular_file()) continue;

        std::string name = entry.path().filename().string();

        // match "*M_deltaE_result.txt"
        if (name.ends_with("M_deltaE_result.txt")) resolution_filenames.push_back(name);
    }

    std::priority_queue<double> deltaE_boundary_max;
    std::priority_queue<double, std::vector<double>, std::greater<double>> deltaE_boundary_min;
    std::priority_queue<double> M_boundary_max;
    std::priority_queue<double, std::vector<double>, std::greater<double>> M_boundary_min;

    for(int i = 0; i < resolution_filenames.size(); i++){
        std::string resolution_filename = resolution_filenames.at(i);

        double deltaE_peak;
        double deltaE_left_sigma;
        double deltaE_right_sigma;
        double M_peak;
        double M_left_sigma;
        double M_right_sigma;
        double theta;

        ReadResolution((std::string(argv[4]) + "/" + resolution_filename).c_str(), &deltaE_peak, &deltaE_left_sigma, &deltaE_right_sigma, &M_peak, &M_left_sigma, &M_right_sigma, &theta);

        deltaE_boundary_max.push(deltaE_peak + 20 * deltaE_right_sigma);
        deltaE_boundary_min.push(deltaE_peak - 20 * deltaE_left_sigma);
        M_boundary_max.push(M_peak + 20 * M_right_sigma);
        M_boundary_min.push(M_peak - 20 * M_left_sigma);
    }

    ObtainWeight = MyScaleFunction;

    Loader loader("tau_lfv");

    loader.Load(argv[1], argv[2], "label");

    loader.PrintInformation("========== initial ==========");

    loader.Cut(("(" + std::to_string(deltaE_boundary_min.top()) + "< deltaE) && (deltaE < " + std::to_string(deltaE_boundary_max.top()) + ")").c_str());
    loader.PrintInformation(("========== " + std::to_string(deltaE_boundary_min.top()) + " < deltaE < " + std::to_string(deltaE_boundary_max.top()) + " ==========").c_str());
    loader.Cut(("(" + std::to_string(M_boundary_min.top()) + "< M) && (M < " + std::to_string(M_boundary_max.top()) + ")").c_str());
    loader.PrintInformation(("========== " + std::to_string(M_boundary_min.top()) + " < M < " + std::to_string(M_boundary_max.top()) + " ==========").c_str());
    //loader.DrawTH2D("(E*E-px*px-py*py-pz*pz)^0.5", "deltaE", ";M [GeV];deltaE [GeV];", 50, 1.3, 1.9, 50, -0.9, 0.4, "M_deltaE_before_cut.png");

    loader.PrintSeparateRootFile((std::string(argv[3]) + "/final_output").c_str(), "", "");

    loader.end();

    return 0;
}
