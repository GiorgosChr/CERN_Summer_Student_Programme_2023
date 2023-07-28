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

void asymmetryWithWeights(std::vector<std::string> fileNames);
void asymmetryWithoutWeights(std::vector<std::string> fileNames);

int main(){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        gROOT->ProcessLine(".L ../ChargedPi/lhcbStyle.C");
        gStyle->SetPalette(55);
        ROOT::EnableImplicitMT();

        std::vector<std::vector<std::string>> fileNamesKK = {
                {
                        "pGun_reducedDst_KK_2015_Down.root",
                        "pGun_reducedDst_KK_2015_Up.root"
                },
                {
                        "pGun_reducedDst_KK_2016_Down.root",
                        "pGun_reducedDst_KK_2016_Up.root"
                },
                {
                        "pGun_reducedDst_KK_2017_Down.root",
                        "pGun_reducedDst_KK_2017_Up.root"
                },
                {
                        "pGun_reducedDst_KK_2018_Down.root",
                        "pGun_reducedDst_KK_2018_Up.root"
                }
        };
        std::vector<std::vector<std::string>> fileNamesPP = {
                {
                        "/eos/lhcb/user/f/fbetti/SummerStudent_proj_2023/pGun/pGun_reducedDst_PiPi_2015_Down.root",
                        "/eos/lhcb/user/f/fbetti/SummerStudent_proj_2023/pGun/pGun_reducedDst_PiPi_2015_Up.root",
                },
                {
                        "/eos/lhcb/user/f/fbetti/SummerStudent_proj_2023/pGun/pGun_reducedDst_PiPi_2016_Down.root",
                        "/eos/lhcb/user/f/fbetti/SummerStudent_proj_2023/pGun/pGun_reducedDst_PiPi_2016_Up.root",
                },
                {
                        "/eos/lhcb/user/f/fbetti/SummerStudent_proj_2023/pGun/pGun_reducedDst_PiPi_2017_Down.root",
                        "/eos/lhcb/user/f/fbetti/SummerStudent_proj_2023/pGun/pGun_reducedDst_PiPi_2017_Up.root",
                },
                {
                        "/eos/lhcb/user/f/fbetti/SummerStudent_proj_2023/pGun/pGun_reducedDst_PiPi_2018_Down.root",
                        "/eos/lhcb/user/f/fbetti/SummerStudent_proj_2023/pGun/pGun_reducedDst_PiPi_2018_Up.root",
                }
        };

        for (size_t i = 0; i < fileNamesKK.size(); i++){
                asymmetryWithWeights(fileNamesKK[i]);
                asymmetryWithoutWeights(fileNamesPP[i]);
        }

        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << std::endl << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}

void asymmetryWithWeights(std::vector<std::string> fileNames){
        std::vector<ROOT::RDataFrame> dataFrames;
        double countPos, countNeg, countPosError, countNegError;
        double asymmetry, asymmetryError, derivPos, derivNeg;
        for (size_t i = 0; i < fileNames.size(); i++){
                dataFrames.push_back(
                        ROOT::RDataFrame("DecayTree", fileNames[i])
                );
                countPos = *dataFrames[i].Filter(
                "sPi_ID == 211"
                ).Sum<double>(
                        "WeightD0"
                );
                countNeg = *dataFrames[i].Filter(
                        "sPi_ID == -211"
                ).Sum<double>(
                        "WeightD0"
                );
                countPosError = *dataFrames[i].Filter(
                        "sPi_ID == 211"
                ).Define(
                        "Weight2", "WeightD0*WeightD0"
                ).Sum<double>(
                        "Weight2"
                );
                countNegError = *dataFrames[i].Filter(
                        "sPi_ID == -211"
                ).Define(
                        "Weight2", "WeightD0*WeightD0"
                ).Sum<double>(
                        "Weight2"
                );
                countPosError = std::pow(countPosError, 0.5);
                countNegError = std::pow(countNegError, 0.5);

                std::cout << fileNames[i] << std::endl;

                asymmetry = (countPos - countNeg)/(countPos + countNeg);

                derivPos = 1.0/(countPos + countNeg) - (countPos + countNeg)/std::pow(countPos + countNeg, 2.0);
                derivNeg = - 1.0/(countPos + countNeg) - (countPos + countNeg)/std::pow(countPos + countNeg, 2.0);

                asymmetryError = std::pow(derivPos * countPosError, 2.0) + std::pow(derivNeg * countNegError, 2.0);
                asymmetryError = std::pow(asymmetryError, 0.5);

                std::cout << "D0 Weights " << asymmetry << " +/- " << asymmetryError << std::endl;

                countPos = *dataFrames[i].Filter(
                "sPi_ID == 211"
                ).Sum<double>(
                        "WeightDst"
                );
                countNeg = *dataFrames[i].Filter(
                        "sPi_ID == -211"
                ).Sum<double>(
                        "WeightDst"
                );
                countPosError = *dataFrames[i].Filter(
                        "sPi_ID == 211"
                ).Define(
                        "Weight2", "WeightDst*WeightDst"
                ).Sum<double>(
                        "Weight2"
                );
                countNegError = *dataFrames[i].Filter(
                        "sPi_ID == -211"
                ).Define(
                        "Weight2", "WeightDst*WeightDst"
                ).Sum<double>(
                        "Weight2"
                );
                countPosError = std::pow(countPosError, 0.5);
                countNegError = std::pow(countNegError, 0.5);


                asymmetry = (countPos - countNeg)/(countPos + countNeg);

                derivPos = 1.0/(countPos + countNeg) - (countPos + countNeg)/std::pow(countPos + countNeg, 2.0);
                derivNeg = - 1.0/(countPos + countNeg) - (countPos + countNeg)/std::pow(countPos + countNeg, 2.0);

                asymmetryError = std::pow(derivPos * countPosError, 2.0) + std::pow(derivNeg * countNegError, 2.0);
                asymmetryError = std::pow(asymmetryError, 0.5);

                std::cout << "Dst Weights " << asymmetry << " +/- " << asymmetryError << std::endl;
                std::cout << std::endl;
        }
}

void asymmetryWithoutWeights(std::vector<std::string> fileNames){
        std::vector<ROOT::RDataFrame> dataFrames;
        double countPos, countNeg, countPosError, countNegError;
        double asymmetry, asymmetryError, derivPos, derivNeg;
        for (size_t i = 0; i < fileNames.size(); i++){
                dataFrames.push_back(
                        ROOT::RDataFrame("DecayTree", fileNames[i])
                );
                countPos = static_cast<double>(
                dataFrames[i].Filter(
                        "sPi_ID == 211"
                ).Count().GetValue()
                );
                countNeg = static_cast<double>(
                dataFrames[i].Filter(
                        "sPi_ID == -211"
                ).Count().GetValue()
                );
                countPosError = std::pow(countPos, 0.5);
                countNegError = std::pow(countNeg, 0.5);

                countPosError = std::pow(countPosError, 0.5);
                countNegError = std::pow(countNegError, 0.5);

                std::cout << fileNames[i] << std::endl;

                asymmetry = (countPos - countNeg)/(countPos + countNeg);

                derivPos = 1.0/(countPos + countNeg) - (countPos + countNeg)/std::pow(countPos + countNeg, 2.0);
                derivNeg = - 1.0/(countPos + countNeg) - (countPos + countNeg)/std::pow(countPos + countNeg, 2.0);

                asymmetryError = std::pow(derivPos * countPosError, 2.0) + std::pow(derivNeg * countNegError, 2.0);
                asymmetryError = std::pow(asymmetryError, 0.5);

                std::cout << asymmetry << " +/- " << asymmetryError << std::endl;
                std::cout << std::endl;
        }
}