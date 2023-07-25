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

std::vector<double> totalAsymmetryWeighted(std::string fileName, std::string treeName);
std::vector<double> totalAsymmetry(std::string fileName, std::string treeName);


int main(){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        gROOT->ProcessLine(".L ../ChargedPi/lhcbStyle.C");
        gStyle->SetPalette(55);
        ROOT::EnableImplicitMT();

        std::vector<std::string> fileNames = {
                "KK_Dst_reduced_weighted_from_D0.root",
                "KK_Dst_reduced_weighted_from_Dst.root",
        };
        std::vector<std::string> fileNamesNew = {
                "KK_Dst_reduced_weighted_from_D0.root",
                "KK_Dst_reduced_weighted_from_Dst.root",
                "pipi_Dst_reduced.root"
        };
        std::string treeName = "DecayTree";

        std::vector<std::vector<double>> asymmetriesWeighted;
        std::vector<std::vector<double>> asymmetries;
        for (size_t i = 0; i < fileNames.size(); i++){
                asymmetriesWeighted.push_back(totalAsymmetryWeighted(fileNames[i], treeName));
        }
        std::cout << std::endl;
        for (size_t i = 0; i < fileNamesNew.size(); i++){
                asymmetries.push_back(totalAsymmetry(fileNamesNew[i], treeName));
        }

        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << std::endl << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}

std::vector<double> totalAsymmetryWeighted(std::string fileName, std::string treeName){
        std::vector<double> result;
        ROOT::RDataFrame dataFrame(treeName, fileName);

        double countPos = *dataFrame.Filter(
                "sPi_ID == 211"
        ).Sum<double>(
                "Weight"
        );
        double countNeg = *dataFrame.Filter(
                "sPi_ID == -211"
        ).Sum<double>(
                "Weight"
        );
        double countPosError = *dataFrame.Filter(
                "sPi_ID == 211"
        ).Define(
                "Weight2", "Weight*Weight"
        ).Sum<double>(
                "Weight2"
        );
        double countNegError = *dataFrame.Filter(
                "sPi_ID == -211"
        ).Define(
                "Weight2", "Weight*Weight"
        ).Sum<double>(
                "Weight2"
        );
        countPosError = std::pow(countPosError, 0.5);
        countNegError = std::pow(countNegError, 0.5);

        std::cout << fileName << std::endl;
        std::cout << "Total Asymmetry (with weights): ";

        double asymmetry, asymmetryError, derivPos, derivNeg;

        asymmetry = (countPos - countNeg)/(countPos + countNeg);

        derivPos = 1.0/(countPos + countNeg) - (countPos + countNeg)/std::pow(countPos + countNeg, 2.0);
        derivNeg = - 1.0/(countPos + countNeg) - (countPos + countNeg)/std::pow(countPos + countNeg, 2.0);

        asymmetryError = std::pow(derivPos * countPosError, 2.0) + std::pow(derivNeg * countNegError, 2.0);
        asymmetryError = std::pow(asymmetryError, 0.5);

        std::cout << asymmetry << " +/- " << asymmetryError << std::endl;
        result.push_back(asymmetry);
        result.push_back(asymmetryError);

        return result;
}


std::vector<double> totalAsymmetry(std::string fileName, std::string treeName){
        std::vector<double> result;
        ROOT::RDataFrame dataFrame(treeName, fileName);

        double countPos = static_cast<double>(
                dataFrame.Filter(
                        "sPi_ID == 211"
                ).Count().GetValue()
        );
        double countNeg = static_cast<double>(
                dataFrame.Filter(
                        "sPi_ID == -211"
                ).Count().GetValue()
        );
        auto countPosError = std::pow(countPos, 0.5);
        auto countNegError = std::pow(countNeg, 0.5);

        std::cout << fileName << std::endl;
        std::cout << "Total Asymmetry (without weights): ";

        double asymmetry, asymmetryError, derivPos, derivNeg;

        asymmetry = (countPos - countNeg)/(countPos + countNeg);

        derivPos = 1.0/(countPos + countNeg) - (countPos + countNeg)/std::pow(countPos + countNeg, 2.0);
        derivNeg = - 1.0/(countPos + countNeg) - (countPos + countNeg)/std::pow(countPos + countNeg, 2.0);

        asymmetryError = std::pow(derivPos * countPosError, 2.0) + std::pow(derivNeg * countNegError, 2.0);
        asymmetryError = std::pow(asymmetryError, 0.5);

        std::cout << asymmetry << " +/- " << asymmetryError << std::endl;
        result.push_back(asymmetry);
        result.push_back(asymmetryError);

        return result;
}