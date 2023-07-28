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

ROOT::RDF::RResultPtr<TH3D> normDistribution(std::string fileName, std::string treeName, int binNumber, std::vector<std::vector<double>> filters);
void calculateWeights(std::vector<std::vector<ROOT::RDataFrame>> dataFrame, std::vector<std::vector<ROOT::RDF::RResultPtr<TH3D>>> normDistributionsKK, std::vector<std::vector<ROOT::RDF::RResultPtr<TH3D>>> normDistributionsPP, std::vector<std::vector<double>> filters, int binNumber, std::vector<std::string> sampleName, std::string treeName);

int main(){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        gROOT->ProcessLine(".L ../ChargedPi/lhcbStyle.C");
        gStyle->SetPalette(55);
        ROOT::EnableImplicitMT();

        std::string path = "/eos/lhcb/user/f/fbetti/SummerStudent_proj_2023/pGun/";
        std::vector<std::vector<std::string>> fileNamesKKDown = {
                {
                        "pGun_reducedD0_KK_2015_Down.root",
                        "pGun_reducedDst_KK_2015_Down.root"
                },
                {
                        "pGun_reducedD0_KK_2016_Down.root",
                        "pGun_reducedDst_KK_2016_Down.root"
                },
                {
                        "pGun_reducedD0_KK_2016_Down.root",
                        "pGun_reducedDst_KK_2016_Down.root"
                },
                {
                        "pGun_reducedD0_KK_2016_Down.root",
                        "pGun_reducedDst_KK_2016_Down.root"
                },
        };
        std::vector<std::vector<std::string>> fileNamesKKUp = {
                {
                        "pGun_reducedD0_KK_2015_Up.root",
                        "pGun_reducedDst_KK_2015_Up.root"
                },
                {
                        "pGun_reducedD0_KK_2016_Up.root",
                        "pGun_reducedDst_KK_2016_Up.root"
                },
                {
                        "pGun_reducedD0_KK_2016_Up.root",
                        "pGun_reducedDst_KK_2016_Up.root"
                },
                {
                        "pGun_reducedD0_KK_2016_Up.root",
                        "pGun_reducedDst_KK_2016_Up.root"
                },
        };
        std::vector<std::vector<std::string>> fileNamesPPDown = {
                {
                        "pGun_reducedD0_PiPi_2015_Down.root",
                        "pGun_reducedDst_PiPi_2015_Down.root"
                },
                {
                        "pGun_reducedD0_PiPi_2016_Down.root",
                        "pGun_reducedDst_PiPi_2016_Down.root"
                },
                {
                        "pGun_reducedD0_PiPi_2016_Down.root",
                        "pGun_reducedDst_PiPi_2016_Down.root"
                },
                {
                        "pGun_reducedD0_PiPi_2016_Down.root",
                        "pGun_reducedDst_PiPi_2016_Down.root"
                },
        };
        std::vector<std::vector<std::string>> fileNamesPPUp = {
                {
                        "pGun_reducedD0_PiPi_2015_Up.root",
                        "pGun_reducedDst_PiPi_2015_Up.root"
                },
                {
                        "pGun_reducedD0_PiPi_2016_Up.root",
                        "pGun_reducedDst_PiPi_2016_Up.root"
                },
                {
                        "pGun_reducedD0_PiPi_2016_Up.root",
                        "pGun_reducedDst_PiPi_2016_Up.root"
                },
                {
                        "pGun_reducedD0_PiPi_2016_Up.root",
                        "pGun_reducedDst_PiPi_2016_Up.root"
                },
        };

        std::vector<std::vector<double>> filters = {
                {0.0, 15000},
                {0.0, 6.1},
                {-3.2, 3.2}
        };
        int binNumber = 25;

        std::vector<std::vector<ROOT::RDF::RResultPtr<TH3D>>> normDistributionsKKDown;
        std::vector<std::vector<ROOT::RDF::RResultPtr<TH3D>>> normDistributionsKKUp;
        std::vector<std::vector<ROOT::RDF::RResultPtr<TH3D>>> normDistributionsPPDown;
        std::vector<std::vector<ROOT::RDF::RResultPtr<TH3D>>> normDistributionsPPUp;
        std::vector<ROOT::RDF::RResultPtr<TH3D>> tempDistributionsKKDown;
        std::vector<ROOT::RDF::RResultPtr<TH3D>> tempDistributionsKKUp;
        std::vector<ROOT::RDF::RResultPtr<TH3D>> tempDistributionsPPDown;
        std::vector<ROOT::RDF::RResultPtr<TH3D>> tempDistributionsPPUp;
        std::string filePath;
        std::string treeName = "DecayTree";

        std::vector<std::vector<ROOT::RDataFrame>> dataFrameKKDown;
        std::vector<std::vector<ROOT::RDataFrame>> dataFrameKKUp;
        std::vector<std::vector<ROOT::RDataFrame>> dataFramePPDown;
        std::vector<std::vector<ROOT::RDataFrame>> dataFramePPUp;
        std::vector<ROOT::RDataFrame> tempdataFrame;

        for (size_t i = 0; i < fileNamesKKDown.size(); i++){
                for (size_t j = 0; j < fileNamesKKDown[i].size(); j++){
                        filePath = path + fileNamesKKDown[i][j];
                        tempDistributionsKKDown.push_back(
                                normDistribution(filePath, treeName, binNumber, filters)
                        );
                        tempdataFrame.push_back(
                                ROOT::RDataFrame(treeName, filePath.c_str())
                        );
                }
                normDistributionsKKDown.push_back(tempDistributionsKKDown);
                tempDistributionsKKDown.clear();

                dataFrameKKDown.push_back(tempdataFrame);
                tempdataFrame.clear();
        }
        tempdataFrame.clear();
        for (size_t i = 0; i < fileNamesKKDown.size(); i++){
                for (size_t j = 0; j < fileNamesKKDown[i].size(); j++){
                        filePath = path + fileNamesKKUp[i][j];
                        tempDistributionsKKUp.push_back(
                                normDistribution(filePath, treeName, binNumber, filters)
                        );
                        tempdataFrame.push_back(
                                ROOT::RDataFrame(treeName, filePath.c_str())
                        );
                }
                normDistributionsKKUp.push_back(tempDistributionsKKUp);
                tempDistributionsKKUp.clear();

                dataFrameKKUp.push_back(tempdataFrame);
                tempdataFrame.clear();
        }
        tempdataFrame.clear();
        for (size_t i = 0; i < fileNamesKKDown.size(); i++){
                for (size_t j = 0; j < fileNamesKKDown[i].size(); j++){
                        filePath = path + fileNamesPPDown[i][j];
                        tempDistributionsPPDown.push_back(
                                normDistribution(filePath, treeName, binNumber, filters)
                        );
                        tempdataFrame.push_back(
                                ROOT::RDataFrame(treeName, filePath.c_str())
                        );
                }
                normDistributionsPPDown.push_back(tempDistributionsPPDown);
                tempDistributionsPPDown.clear();

                dataFramePPDown.push_back(tempdataFrame);
                tempdataFrame.clear();
        }
        tempdataFrame.clear();
        for (size_t i = 0; i < fileNamesKKDown.size(); i++){
                for (size_t j = 0; j < fileNamesKKDown[i].size(); j++){
                        filePath = path + fileNamesPPUp[i][j];
                        tempDistributionsPPUp.push_back(
                                normDistribution(filePath, treeName, binNumber, filters)
                        );
                        tempdataFrame.push_back(
                                ROOT::RDataFrame(treeName, filePath.c_str())
                        );
                }
                normDistributionsPPUp.push_back(tempDistributionsPPUp);
                tempDistributionsPPUp.clear();

                dataFramePPUp.push_back(tempdataFrame);
                tempdataFrame.clear();
        }

        std::vector<std::string> sampleName = {
                "pGun_reducedDst_KK_2015_Down.root",
                "pGun_reducedDst_KK_2016_Down.root",
                "pGun_reducedDst_KK_2017_Down.root",
                "pGun_reducedDst_KK_2018_Down.root"
        };
        calculateWeights(dataFrameKKDown, normDistributionsKKDown, normDistributionsPPDown, filters, binNumber, sampleName, treeName);
        sampleName.clear();
        sampleName = {
                "pGun_reducedDst_KK_2015_Up.root",
                "pGun_reducedDst_KK_2016_Up.root",
                "pGun_reducedDst_KK_2017_Up.root",
                "pGun_reducedDst_KK_2018_Up.root"
        };
        calculateWeights(dataFrameKKUp, normDistributionsKKUp, normDistributionsPPUp, filters, binNumber, sampleName, treeName);


        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << std::endl << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}

ROOT::RDF::RResultPtr<TH3D> normDistribution(std::string fileName, std::string treeName, int binNumber, std::vector<std::vector<double>> filters){
        ROOT::RDataFrame dataFrame(treeName, fileName.c_str());

        auto hist = dataFrame.Histo3D(
                {(fileName).c_str(), "", binNumber, filters[0][0], filters[0][1], binNumber, filters[1][0], filters[1][1], binNumber, filters[2][0], filters[2][1]}, "D0_PT", "D0_ETA","D0_PHI"
        );
        hist->Scale(1.0/hist->Integral());
        return hist;
}

void calculateWeights(std::vector<std::vector<ROOT::RDataFrame>> dataFrame, std::vector<std::vector<ROOT::RDF::RResultPtr<TH3D>>> normDistributionsKK, std::vector<std::vector<ROOT::RDF::RResultPtr<TH3D>>> normDistributionsPP, std::vector<std::vector<double>> filters, int binNumber, std::vector<std::string> sampleName, std::string treeName){

        double weight;
        std::vector<std::vector<std::vector<std::vector<double>>>> weights(2, std::vector<std::vector<std::vector<double>>>(binNumber, std::vector<std::vector<double>>(binNumber, std::vector<double>(binNumber))));

        double dPT = (filters[0][1] - filters[0][0])/static_cast<double>(binNumber);
	double dEta = (filters[1][1] - filters[1][0])/static_cast<double>(binNumber);
	double dPhi = (filters[2][1] - filters[2][0])/static_cast<double>(binNumber);
        
        for (size_t i = 0; i < dataFrame.size(); i++){
                for (size_t j = 0; j < dataFrame[i].size(); j++){
                        // weights.clear();
                        for (size_t k = 0; k < binNumber; k++){
                                for (size_t l = 0; l < binNumber; l++){
                                        for (size_t m = 0; m < binNumber; m++){
				                if (normDistributionsKK[i][j]->GetBinContent(k + 1, l + 1, m + 1) == 0.0){
                                                        weight = 0.0;
                                                }
                                                else {
                                                        weight = normDistributionsPP[i][j]->GetBinContent(k + 1, l + 1, m + 1) / normDistributionsKK[i][j]->GetBinContent(k + 1, l + 1, m + 1);
                                                }
                                                weights[j][k][l][m] = weight;
                                        }
                                }
                        }
                }
                dataFrame[i][1].Filter(
		        [filters](const double& D0_PT, const double& D0_ETA, const double& D0_PHI){
		        	if (D0_PT >= filters[0][0] && D0_PT <= filters[0][1] &&  D0_ETA >= filters[1][0] && D0_ETA <= filters[1][1] &&  D0_PHI >= filters[2][0] && D0_PHI <= filters[2][1]){
		        		return true;
		        	}
		        	return false;
		        }, {"D0_PT", "D0_ETA", "D0_PHI"}
                ).Define(
		        "WeightDst", [weights, dPT, dEta, dPhi, filters, binNumber](const double& D0_PT, const double& D0_ETA, const double& D0_PHI){
		        	int iIndex, jIndex, kIndex;
                                iIndex = 0;
                                jIndex = 0;
                                kIndex = 0;
                                double PT = filters[0][0];
                                double eta = filters[1][0];
                                double phi = filters[2][0];
		        	for (size_t iNew = 0; iNew < binNumber; iNew++){
		        		if ((D0_PT > PT) && (D0_PT < (PT + dPT))){
		        			iIndex = iNew;
		        		}
		        		PT += dPT;
		        	}
                                for (size_t jNew = 0; jNew < binNumber; jNew++){
                                        if ((D0_ETA > eta) && (D0_ETA < (eta + dEta))){
                                                jIndex = jNew;
                                        }
                                        eta += dEta;
                                }
		        	for (size_t kNew = 0; kNew < binNumber; kNew++){
		        		if ((D0_PHI > phi) && (D0_PHI < (phi + dPhi))){
		        			kIndex = kNew;
		        		}
		        		phi += dPhi;
		        	}
		        	return weights[1][iIndex][jIndex][kIndex];
		        }, {"D0_PT", "D0_ETA", "D0_PHI"}		
	        ).Define(
		        "WeightD0", [weights, dPT, dEta, dPhi, filters, binNumber](const double& D0_PT, const double& D0_ETA, const double& D0_PHI){
		        	int iIndex, jIndex, kIndex;
                                iIndex = 0;
                                jIndex = 0;
                                kIndex = 0;
                                double PT = filters[0][0];
                                double eta = filters[1][0];
                                double phi = filters[2][0];
		        	for (size_t iNew = 0; iNew < binNumber; iNew++){
		        		if ((D0_PT > PT) && (D0_PT < (PT + dPT))){
		        			iIndex = iNew;
		        		}
		        		PT += dPT;
		        	}
                                for (size_t jNew = 0; jNew < binNumber; jNew++){
                                        if ((D0_ETA > eta) && (D0_ETA < (eta + dEta))){
                                                jIndex = jNew;
                                        }
                                        eta += dEta;
                                }
		        	for (size_t kNew = 0; kNew < binNumber; kNew++){
		        		if ((D0_PHI > phi) && (D0_PHI < (phi + dPhi))){
		        			kIndex = kNew;
		        		}
		        		phi += dPhi;
		        	}
		        	return weights[0][iIndex][jIndex][kIndex];
		        }, {"D0_PT", "D0_ETA", "D0_PHI"}		
	        ).Snapshot(treeName, sampleName[i].c_str());
        }
}
