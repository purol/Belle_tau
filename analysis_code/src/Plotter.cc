#include <stdio.h>
#include <string>
#include <vector>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"

# define tau_crosssection 0.919 // nb
# define Nevt_taupair ((0.36537/0.000000001) * tau_crosssection)

// I only use `mdst_000001_prod00026883_task10020000001.root`
# define Nevt_SIGNAL_MC15ri 552000

# define BR_SIGNAL 0.00000001 // just set 10^(-8) 
# define Nevt_SIGNAL (Nevt_taupair * BR_SIGNAL * 2.0)
# define Scale_SIGNAL_MC15ri (Nevt_SIGNAL/Nevt_SIGNAL_MC15ri)

double MyScaleFunction(std::vector<Data>::iterator data_) {
    if ((*data_).filename.find("CHG_") != std::string::npos) return Scale_CHG_MC15ri;
    else if ((*data_).filename.find("MIX_") != std::string::npos) return Scale_MIX_MC15ri;
    else if ((*data_).filename.find("UUBAR_") != std::string::npos) return Scale_UUBAR_MC15ri;
    else if ((*data_).filename.find("DDBAR_") != std::string::npos) return Scale_DDBAR_MC15ri;
    else if ((*data_).filename.find("SSBAR_") != std::string::npos) return Scale_SSBAR_MC15ri;
    else if ((*data_).filename.find("CHARM_") != std::string::npos) return Scale_CHARM_MC15ri;
    else if ((*data_).filename.find("MUMU_") != std::string::npos) return Scale_MUMU_MC15ri;
    else if ((*data_).filename.find("EE_") != std::string::npos) return Scale_EE_MC15ri;
    else if ((*data_).filename.find("EEEE_") != std::string::npos) return Scale_EEEE_MC15ri;
    else if ((*data_).filename.find("EEMUMU_") != std::string::npos) return Scale_EEMUMU_MC15ri;
    else if ((*data_).filename.find("EEPIPI_") != std::string::npos) return Scale_EEPIPI_MC15ri;
    else if ((*data_).filename.find("EEKK_") != std::string::npos) return Scale_EEKK_MC15ri;
    else if ((*data_).filename.find("EEPP_") != std::string::npos) return Scale_EEPP_MC15ri;
    else if ((*data_).filename.find("PIPIISR_") != std::string::npos) return Scale_PIPIISR_MC15ri;
    else if ((*data_).filename.find("KKISR_") != std::string::npos) return Scale_KKISR_MC15ri;
    else if ((*data_).filename.find("GG_") != std::string::npos) return Scale_GG_MC15ri;
    else if ((*data_).filename.find("EETAUTAU_") != std::string::npos) return Scale_EETAUTAU_MC15ri;
    else if ((*data_).filename.find("K0K0BARISR_") != std::string::npos) return Scale_K0K0BARISR_MC15ri;
    else if ((*data_).filename.find("MUMUMUMU_") != std::string::npos) return Scale_MUMUMUMU_MC15ri;
    else if ((*data_).filename.find("MUMUTAUTAU_") != std::string::npos) return Scale_MUMUTAUTAU_MC15ri;
    else if ((*data_).filename.find("TAUTAUTAUTAU_") != std::string::npos) return Scale_TAUTAUTAUTAU_MC15ri;
    else if ((*data_).filename.find("TAUPAIR_") != std::string::npos) return Scale_TAUPAIR_MC15ri;
    else if ((*data_).filename.find("SIGNAL_") != std::string::npos) return Scale_SIGNAL_MC15ri;
    else {
        printf("unexpected sample type\n");
        exit(1);
        return 0.0;
    }
    printf("unexpected sample type\n");
    exit(1);
    return 0.0;
}

int main(int argc, char* argv[]) {
    /*
    * argv[1]: variable name
    * argv[2]: signal num
    * argv[3], ...: signal path, signal label
    * argv[]: background num
    * argv[], ...: background path, background label
    */

    std::string variable_name(argv[1]);
    int signal_num = atoi(argv[2]);
    std::vector<std::string> signal_paths;
    std::vector<std::string> signal_labels;
    for (int i = 0; i < signal_num; i++) {
        signal_paths.push_back(argv[3 + 2 * i]);
        signal_labels.push_back(argv[4 + 2 * i]);
    }
    int background_num = atoi(argv[3 + 2 * signal_num]);
    std::vector<std::string> background_paths;
    std::vector<std::string> background_labels;
    for (int i = 0; i < background_num; i++) {
        background_paths.push_back(argv[4 + 2 * signal_num + 2 * i]);
        background_labels.push_back(argv[5 + 2 * signal_num + 2 * i]);
    }

    if (argc == (4 + 2 * signal_num + 2 * background_num)) {}
    else {
        printf("improper argument\n");
        exit(1);
    }

    ObtainWeight = MyScaleFunction;

    Loader loader("tau_lfv");

    for (int i = 0; i < signal_num; i++) loader.Load(signal_paths.at(i).c_str(), "root", signal_labels.at(i).c_str());
    for (int i = 0; i < signal_num; i++) loader.Load(background_paths.at(i).c_str(), "root", background_labels.at(i).c_str());

    // Create a new vector to hold the combined elements
    std::vector<std::string> all_label;
    all_label.reserve(signal_labels.size() + background_labels.size());
    all_label.insert(all_label.end(), signal_labels.begin(), signal_labels.end());
    all_label.insert(all_label.end(), background_labels.begin(), background_labels.end());

    loader.SetMC(all_label);
    loader.SetData({});
    loader.SetSignal(signal_labels);
    loader.SetBackground(background_labels);

    loader.DrawStack(argv[1], (";" + variable_name + ";events").c_str(), (variable_name + ".png").c_str());

    loader.end();

    return 0;
}
