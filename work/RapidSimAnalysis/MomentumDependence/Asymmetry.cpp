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
#include <cmath>

double detection(const double& slope, const double& sPi_PZ, const double& sPi_PX){
        double line = slope * sPi_PZ;

        if (std::abs(sPi_PX) > line){
                return 1.0;
        }

        return 0.0;
}

std::vector<double> integratedAsymmetry(ROOT::RDF::RResultPtr<TH2D> hist, double slope, int binNumber, std::vector<double> ranges){
        std::vector<double> asymmetry = {0.0, 0.0}; // integrated asymmetry and asymmetry error
        double dPX, dPZ;
        double PX, PZ;
        dPX = (ranges[3] - ranges[2])/(static_cast<double>(binNumber));
        dPZ = (ranges[1] - ranges[0])/(static_cast<double>(binNumber));

        PX = -0.4;
        PZ = 0.0;

        for (size_t i = 0; i < binNumber; i++){ // runs over sPi_PX
                PX += dPX;
                for (size_t j = 0; j < binNumber; j++){ // runs over sPi_PZ
                        PZ += dPZ;
                        if (detection(slope, PZ, PX) == 1.0){
                                asymmetry[0] += hist->GetBinContent(j, i);
                        }
                        asymmetry[1] += hist->GetBinContent(j, i);

                }
                PZ = 0.0;
        }

        std::cout << "Integrated detection asymmetry: " << asymmetry[0]/asymmetry[1] << std::endl;

        return asymmetry;
}

void detectionAsymmetry(const std::string fileName, const std::string fileNameNew, const std::string fileNameCP, const std::string treeName, const std::string branchNameX, const std::string branchNameZ, const std::string decay, std::string pion, double asymmetry){

        ROOT::RDataFrame dataFrame(treeName, fileName);
        TRandom3* random = new TRandom3();
        random->SetSeed();

        int binNumber;
        double xMin, xMax, zMin, zMax;
        binNumber = 100;
        xMin = -0.4;
        xMax = -xMin;
        zMin = 0.0;
        zMax = 6.0;


        // // Generate soft pion charges with uniform distribution
        auto dataFramesPi = dataFrame.Define(
                "sPi_C", [random](){return (random->Uniform() < 0.5) ? -1 : 1;}
        );
        // // Plot the soft pion charge distribution
        TCanvas* canvasCharges = new TCanvas("canvasCharges", "");
        auto histCharges = dataFramesPi.Histo1D({"histCharges", "", 10, -2, 2}, "sPi_C");
        histCharges->SetStats(0);
        histCharges->GetXaxis()->SetTitle("soft #pi charge");
        histCharges->Draw("hist");
        canvasCharges->SaveAs((pion + "Initial.pdf").c_str());

        // // Filter out events according to a CP asymmetry
        auto dataFrameCP = dataFramesPi.Filter(
                [random, asymmetry](const int& sPi_C){
                        double probability;
                        probability = 1.0 - (1.0 - asymmetry)/(1.0 + asymmetry);

                        if (sPi_C == -1 && random->Uniform() < probability){
                                return false;
                        }
                        return true;
                }, {"sPi_C"}
        );
        // // Plot the soft pion charge distribution
        auto histCP = dataFrameCP.Histo1D({"histCharges", "", 10, -2, 2}, "sPi_C");
        histCP->SetStats(0);
        histCP->GetXaxis()->SetTitle("soft #pi charge");
        histCP->Draw("hist");
        canvasCharges->SaveAs((pion + "CP.pdf").c_str());



        // // Filter out events according to a detection asymmetry
        // // with momentum dependence
        double slope = 0.1;
        auto dataFrameDetection = dataFrameCP.Filter([slope](int& sPi_C, double& sPi_PX, double& sPi_PZ){
                if (sPi_C == -1 && (std::abs(sPi_PX) > slope*sPi_PZ)){
                        return false;
                }
                return true;
                }, {"sPi_C", "sPi_PX", "sPi_PZ"}
        );
}


int main(){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        gROOT->ProcessLine(".L ../ChargedPi/lhcbStyle.C");

        std::string fileNameK, fileNameP, fileNameNewK, fileNameNewP, treeName, branchNameX, branchNameZ;
        std::vector<std::string> fileNames;
        std::vector<std::string> filenamesNew;
        std::vector<std::string> fileNamesCP;
        std::vector<std::string> decays;
        std::vector<std::string> pions;
        std::vector<double> asymmetries;
        // std::vector<std::string> filters {"sPi_C == -1 && abs(sPi_PX) < 0.15 * sPi_PZ", "sPi_C == -1 && abs(sPi_PX) < 0.15 * sPi_PZ"};
        // std::vector<std::string> filters {"!(abs(sPi_PX) < 0.15 * sPi_PZ && sPi_C == -1)", "!(abs(sPi_PX) < 0.15 * sPi_PZ && sPi_C == -1)"};

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
        pions.push_back("Plots/sPi_C_KmKp");
        pions.push_back("Plots/sPi_C_pimpip");
        fileNamesCP.push_back("Dstp_D0__KmKp_pip_sPiCP.root");
        fileNamesCP.push_back("Dstp_D0__pimpip_pip_sPiCP.root");
        asymmetries.push_back(0.2);
        asymmetries.push_back(0.3);

        for (size_t i = 0; i < fileNames.size(); i++){
                detectionAsymmetry(fileNames[i], filenamesNew[i], fileNamesCP[i], treeName, branchNameX, branchNameZ, decays[i], pions[i], asymmetries[i]);
        }



        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << std::endl << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}