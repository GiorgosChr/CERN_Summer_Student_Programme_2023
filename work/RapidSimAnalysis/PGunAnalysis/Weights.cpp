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

std::vector<double> calculateATotal(std::string fileName, std::string treeName);
std::vector<double> calculateATotalWeighted(std::string fileName, std::string treeName);
ROOT::RDF::RResultPtr<TH3D> normDistribution(std::string fileName, std::string treeName, int binNumber, std::vector<std::vector<double>> filters);
void plotDistribution(ROOT::RDF::RResultPtr<TH3D> distribution, std::string plotName);
void calculateWeights(std::vector<ROOT::RDF::RResultPtr<TH3D>> normDistributions, std::string fileNameDst, std::string fileNameDstWeights, std::string treeName, std::vector<std::vector<double>> filters, int binNumber, std::string weightName);
void assignWeights(std::string fileNameDst, std::string fileNameDstWeights, std::string treeName, std::vector<std::vector<std::vector<double>>> weights, std::vector<std::vector<double>> filters, int binNumber, std::string weightName);

int main(){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        gROOT->ProcessLine(".L ../ChargedPi/lhcbStyle.C");
        gStyle->SetPalette(55);
        ROOT::EnableImplicitMT();

        std::vector<std::string> fileNamesD0 = {
                "KK_D0_reduced.root",
                "pipi_D0_reduced.root"
        };
        std::vector<std::string> fileNamesDst = {
                "KK_Dst_reduced.root",
                "pipi_Dst_reduced.root"
        };
        std::string fileNameDstWeights = "KK_Dst_reduced_weighted_from_D0.root";
        std::string weightName = "Plots/WeightsD0.pdf";
        std::vector<std::string> distributionNames = {
                "Plots/KmKpNormalizedDistribution.pdf",
                "Plots/pimpipNormalizedDistribution.pdf"
        };
        std::string treeName = "DecayTree";

        std::vector<ROOT::RDF::RResultPtr<TH3D>> normDistributions;
        std::vector<std::vector<double>> filters = {
                {0.0, 15000},
                {0.0, 6.1},
                {-3.2, 3.2}
        };
        int binNumber = 25;

        for (const auto& fileName : fileNamesD0){
                normDistributions.push_back(
                        normDistribution(fileName, treeName, binNumber, filters)
                );
        }
        for (size_t i = 0; i < fileNamesD0.size(); i++){
                plotDistribution(normDistributions[i], distributionNames[i]);
        }

        calculateWeights(normDistributions, fileNamesDst[0], fileNameDstWeights, treeName, filters, binNumber, weightName);

        fileNameDstWeights = "KK_Dst_reduced_weighted_from_Dst.root";
        weightName = "Plots/WeightsDst.pdf";
        std::vector<ROOT::RDF::RResultPtr<TH3D>> normDistributionsNew;
        std::vector<std::string> distributionNamesNew = {
                "Plots/KmKpNormalizedDistributionFromDst.pdf",
                "Plots/pimpipNormalizedDistributionFromDst.pdf"
        };

        for (const auto& fileName : fileNamesDst){
                normDistributionsNew.push_back(
                        normDistribution(fileName, treeName, binNumber, filters)
                );
        }
        for (size_t i = 0; i < fileNamesDst.size(); i++){
                plotDistribution(normDistributionsNew[i], distributionNamesNew[i]);
        }

        calculateWeights(normDistributionsNew, fileNamesDst[0], fileNameDstWeights, treeName, filters, binNumber, weightName);

        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << std::endl << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}

std::vector<double> calculateATotal(std::string fileName, std::string treeName){
        ROOT::RDataFrame dataFrame(treeName, fileName);

        int ID = 211;
        std::vector<double> asymmetry;

        auto countPos = static_cast<double>(
                dataFrame.Filter(
                        [ID](const int& sPi_ID){
                                if (sPi_ID == ID){
                                        return true;
                                }
                                return false;
                        }, {"sPi_ID"}
                ).Count().GetValue()
        );
        auto countNeg = static_cast<double>(
                dataFrame.Filter(
                        [ID](const int& sPi_ID){
                                if (sPi_ID == -ID){
                                        return true;
                                }
                                return false;
                        }, {"sPi_ID"}
                ).Count().GetValue()
        );

        asymmetry.push_back((countPos - countNeg)/(countPos + countNeg));

        auto countPosError = std::pow(countPos, 0.5);
        auto countNegError = std::pow(countNeg, 0.5);

        auto derivPos = 1.0/(countPos + countNeg) - (countPos + countNeg)/std::pow(countPos + countNeg, 2.0);
        auto derivNeg = - 1.0/(countPos + countNeg) - (countPos + countNeg)/std::pow(countPos + countNeg, 2.0);

        auto asymmetryError = std::pow(derivPos * countPosError, 2.0) + std::pow(derivNeg * countNegError, 2.0);
        asymmetryError = std::pow(asymmetryError, 0.5);

        asymmetry.push_back(asymmetryError);

        return asymmetry;
}

std::vector<double> calculateATotalWeighted(std::string fileName, std::string treeName){
        ROOT::RDataFrame dataFrame(treeName, fileName);

        int ID = 211;
        std::vector<double> asymmetry;

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

        asymmetry.push_back((countPos - countNeg)/(countPos + countNeg));

        auto derivPos = 1.0/(countPos + countNeg) - (countPos + countNeg)/std::pow(countPos + countNeg, 2.0);
        auto derivNeg = - 1.0/(countPos + countNeg) - (countPos + countNeg)/std::pow(countPos + countNeg, 2.0);

        auto asymmetryError = std::pow(derivPos * countPosError, 2.0) + std::pow(derivNeg * countNegError, 2.0);
        asymmetryError = std::pow(asymmetryError, 0.5);

        asymmetry.push_back(asymmetryError);

        return asymmetry;
}

ROOT::RDF::RResultPtr<TH3D> normDistribution(std::string fileName, std::string treeName, int binNumber, std::vector<std::vector<double>> filters){
        ROOT::RDataFrame dataFrame(treeName, fileName);

        auto hist = dataFrame.Histo3D(
                {(fileName).c_str(), "", binNumber, filters[0][0], filters[0][1], binNumber, filters[1][0], filters[1][1], binNumber, filters[2][0], filters[2][1]}, "D0_PT", "D0_ETA","D0_PHI"
        );
        hist->Scale(1.0/hist->Integral());
        return hist;
}

void plotDistribution(ROOT::RDF::RResultPtr<TH3D> distribution, std::string plotName){
        TCanvas* canvas = new TCanvas("canvas", "");
        distribution->SetStats(0);
        distribution->GetXaxis()->SetTitle("P_{T} MeV/c");
        distribution->GetYaxis()->SetTitle("#eta");
        distribution->GetZaxis()->SetTitle("#phi Rad");
        distribution->Draw("LEGO2");
        canvas->SaveAs(plotName.c_str());
}

void calculateWeights(std::vector<ROOT::RDF::RResultPtr<TH3D>> normDistributions, std::string fileNameDst, std::string fileNameDstWeights, std::string treeName, std::vector<std::vector<double>> filters, int binNumber, std::string weightName){
        double weight;
        std::vector<std::vector<std::vector<double>>> weights(binNumber, std::vector<std::vector<double>>(binNumber, std::vector<double>(binNumber)));
        for (size_t i = 0; i < binNumber; i++){
                for (size_t j = 0; j < binNumber; j++){
                        for (size_t k = 0; k < binNumber; k++){
				if (normDistributions[0]->GetBinContent(i + 1, j + 1, k + 1) == 0.0){
                                        weight = 0.0;
                                }
                                else{
                                        weight = normDistributions[1]->GetBinContent(i + 1, j + 1, k + 1) / normDistributions[0]->GetBinContent(i + 1, j + 1, k + 1);
                                }
                                weights[i][j][k] = weight;
                        }
                }
        }
        assignWeights(fileNameDst, fileNameDstWeights, treeName, weights, filters, binNumber, weightName);
}

void assignWeights(std::string fileNameDst, std::string fileNameDstWeights, std::string treeName, std::vector<std::vector<std::vector<double>>> weights, std::vector<std::vector<double>> filters, int binNumber, std::string weightName){
        double dPT = (filters[0][1] - filters[0][0])/static_cast<double>(binNumber);
	double dEta = (filters[1][1] - filters[1][0])/static_cast<double>(binNumber);
	double dPhi = (filters[2][1] - filters[2][0])/static_cast<double>(binNumber);

        ROOT::RDataFrame dataFrame(treeName, fileNameDst);

        auto dataFrameWeighted = dataFrame.Filter(
		[filters](const double& D0_PT, const double& D0_ETA, const double& D0_PHI){
			if (D0_PT >= filters[0][0] && D0_PT <= filters[0][1] &&  D0_ETA >= filters[1][0] && D0_ETA <= filters[1][1] &&  D0_PHI >= filters[2][0] && D0_PHI <= filters[2][1]){
				return true;
			}
			return false;
		}, {"D0_PT", "D0_ETA", "D0_PHI"}
	).Define(
		"Weight", [weights, dPT, dEta, dPhi, filters, binNumber](const double& D0_PT, const double& D0_ETA, const double& D0_PHI){
			int iIndex, jIndex, kIndex;
                        iIndex = 0;
                        jIndex = 0;
                        kIndex = 0;

                        double PT = filters[0][0];
                        double eta = filters[1][0];
                        double phi = filters[2][0];

			for (size_t i = 0; i < binNumber; i++){
				if ((D0_PT > PT) && (D0_PT < (PT + dPT))){
					iIndex = i;
				}
				PT += dPT;
			}

                        for (size_t j = 0; j < binNumber; j++){
                                if ((D0_ETA > eta) && (D0_ETA < (eta + dEta))){
                                        jIndex = j;
                                }
                                eta += dEta;
                        }

			for (size_t k = 0; k < binNumber; k++){
				if ((D0_PHI > phi) && (D0_PHI < (phi + dPhi))){
					kIndex = k;
				}
				phi += dPhi;
			}


			return weights[iIndex][jIndex][kIndex];
		}, {"D0_PT", "D0_ETA", "D0_PHI"}		
	);

	dataFrameWeighted.Snapshot(treeName, fileNameDstWeights);
        TCanvas* canvas = new TCanvas("canvas", "");

        auto hist = dataFrameWeighted.Histo1D(
                {"hist", "", 50, 0.5, 1.5},
                "Weight"
        );
        hist->SetStats(0);
        hist->GetXaxis()->SetTitle("Weight");
        hist->Draw("HIST");
        canvas->SaveAs(weightName.c_str());
}