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

std::vector<double> totalAsymmetry(std::string fileNameLowStatisticsAsymmetryWeights, std::string treeName);

int main(){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        gROOT->ProcessLine(".L ../ChargedPi/lhcbStyle.C");
        gStyle->SetPalette(55);
        ROOT::EnableImplicitMT();

        std::vector<std::string> fileNamesLowStatisticsAsymmetryWeights = {
                "Dstp_D0__KmKp_pip_tree_HighStatistics_Analysis_Asymmetry_Weights.root",
                "Dstp_D0__pimpip_pip_tree_HighStatistics_Analysis_Asymmetry_Weights.root"
        };
        std::string treeName = "DecayTree";

        std::vector<std::vector<double>> asymmetries;
        for (size_t i = 0; i < fileNamesLowStatisticsAsymmetryWeights.size(); i++){
                asymmetries.push_back(totalAsymmetry(fileNamesLowStatisticsAsymmetryWeights[i], treeName));
        }

        std::cout << "ΔΑ Total: " << asymmetries[1][2] - asymmetries[0][0];
        std::cout << " +/- " << std::pow(std::pow(asymmetries[1][3], 2.0) + std::pow(asymmetries[0][1], 2.0), 0.5) << std::endl;
        std::cout << "Deviation: " << std::abs(0.1 - (asymmetries[1][2] - asymmetries[0][0]))/std::pow(std::pow(asymmetries[1][3], 2.0) + std::pow(asymmetries[0][1], 2.0), 0.5);
        std::cout << " σ" << std::endl;

        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << std::endl << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}

std::vector<double> totalAsymmetry(std::string fileNameLowStatisticsAsymmetryWeights, std::string treeName){
        std::vector<double> result;
        ROOT::RDataFrame dataFrame(treeName, fileNameLowStatisticsAsymmetryWeights);

        double countPos = *dataFrame.Filter(
                "sPi_C == 1"
        ).Sum<double>(
                "Weight"
        );
        double countNeg = *dataFrame.Filter(
                "sPi_C == -1"
        ).Sum<double>(
                "Weight"
        );
        double countPosError = *dataFrame.Filter(
                "sPi_C == 1"
        ).Define(
                "Weight2", "Weight*Weight"
        ).Sum<double>(
                "Weight2"
        );
        double countNegError = *dataFrame.Filter(
                "sPi_C == -1"
        ).Define(
                "Weight2", "Weight*Weight"
        ).Sum<double>(
                "Weight2"
        );
        countPosError = std::pow(countPosError, 0.5);
        countNegError = std::pow(countNegError, 0.5);

        std::cout << fileNameLowStatisticsAsymmetryWeights << std::endl;
        std::cout << "Total Asymmetry (with weights): ";

        double asymmetry, asymmetryError, derivPos, derivNeg;

        asymmetry = (countPos - countNeg)/(countPos + countNeg);

        derivPos = 1.0/(countPos + countNeg) - (countPos - countNeg)/std::pow(countPos + countNeg, 2.0);
        derivNeg = - 1.0/(countPos + countNeg) - (countPos - countNeg)/std::pow(countPos + countNeg, 2.0);

        asymmetryError = std::pow(derivPos * countPosError, 2.0) + std::pow(derivNeg * countNegError, 2.0);
        asymmetryError = std::pow(asymmetryError, 0.5);

        std::cout << asymmetry << " +/- " << asymmetryError << std::endl;
        result.push_back(asymmetry);
        result.push_back(asymmetryError);

        std::cout << "Total Asymmetry (without weights): ";

        countPos = dataFrame.Filter(
                "sPi_C == 1"
        ).Count().GetValue();
        countNeg = dataFrame.Filter(
                "sPi_C == -1"
        ).Count().GetValue();

        countPosError = std::pow(countPos, 0.5);
        countNegError = std::pow(countNeg, 0.5);

        asymmetry = (countPos - countNeg)/(countPos + countNeg);

        derivPos = 1.0/(countPos + countNeg) - (countPos - countNeg)/std::pow(countPos + countNeg, 2.0);
        derivNeg = - 1.0/(countPos + countNeg) - (countPos - countNeg)/std::pow(countPos + countNeg, 2.0);

        asymmetryError = std::pow(derivPos * countPosError, 2.0) + std::pow(derivNeg * countNegError, 2.0);
        asymmetryError = std::pow(asymmetryError, 0.5);

        std::cout << asymmetry << " +/- " << asymmetryError << std::endl;
        result.push_back(asymmetry);
        result.push_back(asymmetryError);

        return result;
}
