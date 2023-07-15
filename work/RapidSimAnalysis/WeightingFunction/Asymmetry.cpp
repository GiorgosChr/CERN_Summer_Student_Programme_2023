#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TRandom3.h>


#include <iostream>
#include <sstream>
#include <chrono>
#include <cmath>

void introduceAsymmetries(std::vector<std::string> fileNames, std::vector<std::string> fileNamesNew);

int main(){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        gROOT->ProcessLine(".L ../ChargedPi/lhcbStyle.C");
        gStyle->SetPalette(55);
        // ROOT::EnableImplicitMT();

        std::vector<std::string> fileNamesHighStatistics = {
                "Dstp_D0__KmKp_pip_tree_HighStatistics.root",
                "Dstp_D0__pimpip_pip_tree_HighStatistics.root"
        };
        std::vector<std::string> fileNamesLowStatistics = {
                "Dstp_D0__KmKp_pip_tree_LowStatistics.root",
                "Dstp_D0__pimpip_pip_tree_LowStatistics.root"
        };
        std::vector<std::string> fileNamesHighStatisticsAsymmetry = {
                "Dstp_D0__KmKp_pip_tree_HighStatistics_Asymmetry.root",
                "Dstp_D0__pimpip_pip_tree_HighStatistics_Asymmetry.root"
        };
        std::vector<std::string> fileNamesLowStatisticsAsymmetry = {
                "Dstp_D0__KmKp_pip_tree_LowStatistics_Asymmetry.root",
                "Dstp_D0__pimpip_pip_tree_LowStatistics_Asymmetry.root"
        };
        
        introduceAsymmetries(fileNamesHighStatistics, fileNamesHighStatisticsAsymmetry);
        introduceAsymmetries(fileNamesLowStatistics, fileNamesLowStatisticsAsymmetry);


        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << std::endl << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}


void introduceAsymmetries(std::vector<std::string> fileNames, std::vector<std::string> fileNamesNew){
        std::string treeName = "DecayTree";
        double slope = 0.1;
        std::vector<double> asymmetriesCP = {
                0.1,
                0.2
        };

        ROOT::RDataFrame dataFrame1(treeName, fileNames[0]);
        ROOT::RDataFrame dataFrame2(treeName, fileNames[1]);

        std::vector<ROOT::RDataFrame> dataFrames = {
                dataFrame1,
                dataFrame2
        };
        std::vector<ROOT::RDataFrame> dataFramesFinal;
        std::vector<ROOT::RDataFrame> dataFramesAsymmetry;

        TRandom3* random = new TRandom3();
        // random->SetSeed();
        
        for (size_t i = 0; i < dataFrames.size(); i++){
                dataFrames[i].Define(
                        "sPi_C", [random](){
                                if (random->Uniform() < 0.5){
                                        return 1;
                                }
                                else{
                                        return -1;
                                }
                        }
                ).Filter(
                        [random, asymmetriesCP, i](const int& sPi_C){
                                double probability = 1.0 - (1.0 - asymmetriesCP[i])/(1.0 + asymmetriesCP[i]);
                                if ((sPi_C == -1) && (random->Uniform() < probability)){
                                        return false;
                                }
                                else{
                                        return true;
                                }
                        }, {"sPi_C"}
                ).Filter([slope](int& sPi_C, double& sPi_PX, double& sPi_PZ){
                        if (sPi_C == -1 && (std::abs(sPi_PX) > slope*sPi_PZ)){
                                return false;
                        }
                        return true;
                        }, {"sPi_C", "sPi_PX", "sPi_PZ"}
                ).Snapshot(
                        treeName, fileNamesNew[i]
                );
        }
}