#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>
#include <vector>
#include <string>
#include <format>

#include <TLegend.h>
#include <TGraph.h>
#include <TCanvas.h>

typedef struct FBDTinfo {
    unsigned int nTrees;
    unsigned int depth;
    double shrinkage;
    double subsample;
    unsigned int binning;
    double train_AUC;
    double test_AUC;
} FBDTInfo;

bool data_sorter(FBDTInfo const& lhs, FBDTInfo const& rhs) {
    return lhs.test_AUC > rhs.test_AUC;
}

int main(int argc, char* argv[]) {
    /*
    * argv[1]: auc file path
    * argv[2]: output path
    * argv[3]: mass
    * argv[4]: lifetime
    * argv[5]: A constant
    * argv[6]: B constant
    */

    double mass = std::stod(argv[3]);
    double life = std::stod(argv[4]);
    int A = std::stoi(argv[5]);
    int B = std::stoi(argv[6]);

    std::string input_path = std::string(argv[1]);
    std::regex filename_pattern(R"((\d+\.\d+)_(\d+\.\d+)_(\d+\.\d+)_(\d+\.\d+)_(\d+\.\d+)\.auc)");
    std::vector<FBDTInfo> Datas;

    // read auc files
    for (std::filesystem::directory_entry entry : std::filesystem::directory_iterator(input_path)) {
        if (entry.path().extension() == ".auc") {
            std::string filename = entry.path().filename().string();
            std::smatch match;

            if (std::regex_match(filename, match, filename_pattern)) {
                unsigned int nTrees_ = std::stoi(match.str(1));
                unsigned int depth_ = std::stoi(match.str(2));
                double shrinkage_ = std::stod(match.str(3));
                double subsample_ = std::stod(match.str(4));
                unsigned int binning_ = std::stoi(match.str(5));

                std::ifstream file(entry.path());
                double train_AUC_;
                double test_AUC_;
                file >> train_AUC_ >> test_AUC_;

                Datas.push_back({ nTrees_, depth_, shrinkage_, subsample_, binning_, train_AUC_, test_AUC_ });

            }
        }
    }

    // sort
    std::sort(Datas.begin(), Datas.end(), &data_sorter);

    int Nlist = Datas.size();

    double* Rank = (double*) malloc(sizeof(double) * Nlist);
    for (int i = 0; i < Nlist; i++) Rank[i] = i + 1;
    double* train_AUCs = (double*)malloc(sizeof(double) * Nlist);
    for (int i = 0; i < Nlist; i++) train_AUCs[i] = Datas.at(i).train_AUC;
    double* test_AUCs = (double*)malloc(sizeof(double) * Nlist);
    for (int i = 0; i < Nlist; i++) test_AUCs[i] = Datas.at(i).test_AUC;

    // print result
    FILE* fp;
    fp = fopen((std::string(argv[2]) + "/alpha_mass" + std::format("{:g}", mass) + "_life" + std::format("{:g}", life) + "_A" + std::to_string(A) + "_B" + std::to_string(B) + "_total_auc.txt").c_str(), "w");
    for (int i = 0; i < Nlist; i++) fprintf(fp, "%u_%u_%lf_%lf_%u %lf %lf\n", Datas.at(i).nTrees, Datas.at(i).depth, Datas.at(i).shrinkage, Datas.at(i).subsample, Datas.at(i).binning, Datas.at(i).train_AUC, Datas.at(i).test_AUC);
    fclose(fp);

    // draw plot
    TGraph* gr_train = new TGraph(Nlist, Rank, train_AUCs);
    TGraph* gr_test = new TGraph(Nlist, Rank, test_AUCs);

    gr_train->SetMarkerStyle(8); gr_train->SetMarkerSize(0.8);
    gr_test->SetMarkerStyle(8); gr_test->SetMarkerSize(0.8);

    gr_train->SetMarkerColor(kRed + 1);
    gr_test->SetMarkerColor(kBlue + 1);

    gr_train->SetTitle(";test AUC rank;AUC");

    TCanvas* c = new TCanvas("c1", "AUC", 200, 10, 600, 600);
    c->SetLeftMargin(0.15);

    gr_train->Draw("AP");
    gr_test->Draw("P");

    TLegend* legend = new TLegend(0.15, 0.8, 0.35, 0.9); legend->SetFillStyle(0); legend->SetLineWidth(0);
    legend->AddEntry(gr_train, "train", "P"); legend->AddEntry(gr_test, "test", "P");
    legend->Draw();
    c->SaveAs((std::string(argv[2]) + "/AUC.png").c_str());

    // get best result
    bool find_good_FBDT = false;
    for (int i = 0; i < Nlist; i++) {
        if ((Datas.at(i).train_AUC / Datas.at(i).test_AUC) < 1.015) {
            FILE* fp;
            fp = fopen((std::string(argv[2]) + "/alpha_mass" + std::format("{:g}", mass) + "_life" + std::format("{:g}", life) + "_A" + std::to_string(A) + "_B" + std::to_string(B) + "_selected.txt").c_str(), "w");
            fprintf(fp, "%u_%u_%lf_%lf_%u %lf %lf\n", Datas.at(i).nTrees, Datas.at(i).depth, Datas.at(i).shrinkage, Datas.at(i).subsample, Datas.at(i).binning, Datas.at(i).train_AUC, Datas.at(i).test_AUC);
            fclose(fp);

            find_good_FBDT = true;
            break;
        }
    }

    if (find_good_FBDT == false) {
        FILE* fp;
        fp = fopen((std::string(argv[2]) + "/alpha_mass" + std::format("{:g}", mass) + "_life" + std::format("{:g}", life) + "_A" + std::to_string(A) + "_B" + std::to_string(B) + "_selected.txt").c_str(), "w");
        fprintf(fp, "cannot find good FBDT weight\n");
        fclose(fp);
    }

    return 0;
}