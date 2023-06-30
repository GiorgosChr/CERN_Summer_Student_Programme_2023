#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TBranch.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TH1.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TLatex.h>
#include <TMathText.h>
#include <TRandom3.h>

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <chrono>

void detectionAsymmetry(const std::string fileName, const std::string fileNameNew, const std::string treeName, const std::string branchNameX, const std::string branchNameZ, const std::string decay, std::string filter){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        ROOT::RDataFrame dataFrame(treeName, fileName);
        TRandom3* random = new TRandom3();
        random->SetSeed();

        auto dataFrameNew = dataFrame.Define(
                "sPi_C", [random](){
                        return (random->Uniform() < 0.5) ? -1 : 1;
                }
        );
        // .Define(
        //         "MustSkip", [random](int sPi_C, double sPi_PX, double sPi_PZ){
        //                 return ((abs(sPi_PX/sPi_PZ) < (0.4/3.0)) && (sPi_C == -1));
        //         }, {"sPi_C", "sPi_PX", "sPi_PZ"}
        // );

        // auto dataFrameFiltered = dataFrameNew.Filter(
        //         "MustSkip == false"
        // );

        auto dataFrameFiltered = dataFrameNew.Filter(
                filter
        );
        
        auto hist = dataFrameFiltered.Histo2D({"hist", "", 100, 0, 6, 100, -0.4, 0.4}, "sPi_PZ", "sPi_PX");
        
        TCanvas* canvas = new TCanvas("canvas", "");
        
        gStyle->SetPalette(55);
        hist->SetStats(0);
        hist->GetYaxis()->SetTitle("p_{x}(#pi) [GeV/c]");
        hist->GetXaxis()->SetTitle("p_{z}(#pi) [GeV/c]");
        hist->DrawNormalized("COLZ");
        canvas->SaveAs(decay.c_str());

        dataFrameFiltered.Snapshot(treeName, fileNameNew);

        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "detectionAsymmetry() Elapsed time: " << duration.count() << " sec" << std::endl;
}


int main(){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        gROOT->ProcessLine(".L ../ChargedPi/lhcbStyle.C");

        std::string fileNameK, fileNameP, fileNameNewK, fileNameNewP, treeName, branchNameX, branchNameZ;
        std::vector<std::string> fileNames;
        std::vector<std::string> filenamesNew;
        std::vector<std::string> decays;
        // std::vector<std::string> filters {"sPi_C == -1 && abs(sPi_PX) < 0.15 * sPi_PZ", "sPi_C == -1 && abs(sPi_PX) < 0.15 * sPi_PZ"};
        std::vector<std::string> filters {"!(abs(sPi_PX) < 0.15 * sPi_PZ && sPi_C == -1)", "!(abs(sPi_PX) < 0.15 * sPi_PZ && sPi_C == -1)"};

        treeName = "DecayTree";
        fileNameK = "../FirstPlots/Dstp_D0__KmKp_pip_tree.root";
        fileNameP = "../ChargedPi/Dstp_D0__pimpip_pip_tree.root";
        fileNameNewK = "Dstp_D0__KmKp_pip_sPi_tree.root";
        fileNameNewP = "Dstp_D0__pimpip_pip_sPi_tree.root";
        branchNameX = "sPi_PX";
        branchNameZ = "sPi_PZ";

        fileNames.push_back(fileNameK);
        fileNames.push_back(fileNameP);
        filenamesNew.push_back(fileNameNewK);
        filenamesNew.push_back(fileNameNewP);
        decays.push_back("Plots/sPi_PXPZ_KmKp_Detection.pdf");
        decays.push_back("Plots/sPi_PXPZ_pimpip_Detection.pdf");

        for (size_t i = 0; i < fileNames.size(); i++){
                detectionAsymmetry(fileNames[i], filenamesNew[i], treeName, branchNameX, branchNameZ, decays[i], filters[i]);
        }



        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << std::endl << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}
