#include <stdio.h>
#include <string>
#include <vector>

#include "TFile.h"

#include "Loader.h"
#include "constants.h"

# define tau_crosssection 0.919 // nb
# define Nevt_taupair ((0.36537/0.000000001) * tau_crosssection)

# define Nevt_SIGNAL_MC15ri 3597981

# define BR_SIGNAL 0.00000001 // just set 10^(-8) 
# define Nevt_SIGNAL (Nevt_taupair * BR_SIGNAL * 2.0)
# define Scale_SIGNAL_MC15ri (Nevt_SIGNAL/Nevt_SIGNAL_MC15ri)

namespace Module {

    class CutM : public Module {
    private:
        double MIN;
        double MAX;

        std::vector<std::string>* variable_names;
        std::vector<std::string>* VariableTypes;

        int index_E;
        int index_px;
        int index_py;
        int index_pz;

    public:
        CutM(double MIN_, double MAX_, std::vector<std::string>* variable_names_, std::vector<std::string>* VariableTypes_) : Module(), MIN(MIN_), MAX(MAX_), variable_names(variable_names_), VariableTypes(VariableTypes_) {}
        ~CutM() {}

        void Start() {
            std::vector<std::string>::iterator iter;

            iter = std::find(variable_names->begin(), variable_names->end(), "E");
            if (iter != variable_names->end()) {
                index_E = std::distance(variable_names->begin(), iter);
            }
            else {
                printf("[CutM] unknown variable: E\n");
                exit(1);
            }

            iter = std::find(variable_names->begin(), variable_names->end(), "px");
            if (iter != variable_names->end()) {
                index_px = std::distance(variable_names->begin(), iter);
            }
            else {
                printf("[CutM] unknown variable: px\n");
                exit(1);
            }

            iter = std::find(variable_names->begin(), variable_names->end(), "py");
            if (iter != variable_names->end()) {
                index_py = std::distance(variable_names->begin(), iter);
            }
            else {
                printf("[CutM] unknown variable: py\n");
                exit(1);
            }

            iter = std::find(variable_names->begin(), variable_names->end(), "pz");
            if (iter != variable_names->end()) {
                index_pz = std::distance(variable_names->begin(), iter);
            }
            else {
                printf("[CutM] unknown variable: pz\n");
                exit(1);
            }
        }

        int Process(std::vector<Data>* data) override {
            for (std::vector<Data>::iterator iter = data->begin(); iter != data->end(); ) {
                double E = (double)std::get<double>(iter->variable.at(index_E));
                double px = (double)std::get<double>(iter->variable.at(index_px));
                double py = (double)std::get<double>(iter->variable.at(index_py));
                double pz = (double)std::get<double>(iter->variable.at(index_pz));
                double result = std::sqrt(E * E - px * px - py * py - pz * pz);
                if ((result < MIN) || (result > MAX)) {
                    data->erase(iter);
                }
                else ++iter;
            }

            return 1;
        }

        void End() override {}
    };

}

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
    * argv[1]: dirname
    */

    ObtainWeight = MyScaleFunction;

    Loader loader("tau_lfv");

    loader.Load(argv[1], ".root", "label");
    loader.Cut("(0.5 < extraInfo__bodecayModeID__bc) && (extraInfo__bodecayModeID__bc < 1.5)"); // select tau -> mu mu mu only

    loader.PrintInformation("========== initial ==========");

    loader.Cut("(-0.296904 < deltaE) && (deltaE < 0.20147)");
    loader.PrintInformation("========== -0.296904 < deltaE < 0.20147 ==========");

    Module::Module* temp_module = new Module::CutM(-0.296904, 0.20147, loader.Getvariable_names_address(), loader.VariableTypes_address());
    loader.InsertCustomizedModule(temp_module);
    loader.PrintInformation("========== 1.681298 < M < 1.8656 ==========");

    //loader.DrawTH2D("(E*E-px*px-py*py-pz*pz)^0.5", "deltaE", ";M [GeV];deltaE [GeV];", 50, 1.681298, 1.8656, 50, -0.296904, 0.20147, "M_deltaE_before_cut.png");

    //loader.PrintSeparateRootFile((std::string(argv[2]) + "/before_muon_selection").c_str(), "", "");

    loader.Cut("(0.3 < daughter__bo0__cm__sppx__bc) && (0.6 < daughter__bo0__cmmuonID__bc)");
    loader.Cut("(0.3 < daughter__bo1__cm__sppx__bc) && (0.6 < daughter__bo1__cmmuonID__bc)");
    loader.Cut("(0.3 < daughter__bo2__cm__sppx__bc) && (0.6 < daughter__bo2__cmmuonID__bc)");
    loader.PrintInformation("========== 0.3 < p_muon && 0.6 < muonID ==========");

    //loader.DrawTH1D("(E*E-px*px-py*py-pz*pz)^0.5", "M_after_cut", "M_after_cut.png");
    //loader.DrawTH1D("deltaE", "deltaE_after_cut", "deltaE_after_cut.png");
    //loader.DrawTH2D("(E*E-px*px-py*py-pz*pz)^0.5", "deltaE", ";M [GeV];deltaE [GeV];", 50, 1.681298, 1.8656, 50, -0.296904, 0.20147, "M_deltaE_after_cut.png");

    //loader.PrintSeparateRootFile((std::string(argv[2]) + "/before_theta_miss_cut").c_str(), "", "");

    //loader.DrawTH1D("missingMomentumOfEventCMS_theta", "theta_miss", "theta_miss.png");
    loader.Cut("(0.3 < missingMomentumOfEventCMS_theta) && (missingMomentumOfEventCMS_theta < 2.7)");
    loader.PrintInformation("========== 0.3 < theta_miss < 2.7 ==========");

    //loader.PrintSeparateRootFile((std::string(argv[2]) + "/before_thrust_cut").c_str(), "", "");

    //loader.DrawTH1D("thrust", "thrust", "thrust.png");
    loader.Cut("(0.89 < thrust) && (thrust < 0.97)");
    loader.PrintInformation("========== 0.89 < thrust < 0.97 ==========");

    //loader.PrintSeparateRootFile((std::string(argv[2]) + "/final_output").c_str(), "", "");

    loader.end();

    return 0;
}
