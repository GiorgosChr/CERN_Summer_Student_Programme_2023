#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>


#include <iostream>
#include <sstream>
#include <chrono>
#include <cmath>

void assignWeights(std::string fileName, std::string fileNameNew, std::string treeName, int binNumber, std::vector<double> xRange, std::vector<double> yRange, std::vector<double> zRange, std::vector<std::vector<std::vector<double>>> weights){
	ROOT::RDataFrame dataFrame(treeName, fileName);
	
	double dPT = (xRange[1] - xRange[0])/static_cast<double>(binNumber);
	double dEta = (yRange[1] - yRange[0])/static_cast<double>(binNumber);
	double dPhi = (zRange[1] - zRange[0])/static_cast<double>(binNumber);


	auto dataFrameWeighted = dataFrame.Filter(
		[xRange, yRange, zRange](const double& D0_PT, const double& D0_eta, const double& D0_phi){
			if (D0_PT >=xRange[0] && D0_PT <=xRange[1] &&  D0_eta >=yRange[0] && D0_eta <=yRange[1] &&  D0_phi >=zRange[0] && D0_phi <=zRange[1]){
				return true;
			}
			return false;
		}, {"D0_PT", "D0_eta", "D0_phi"}
	).Define(
		"Weight", [weights, dPT, dEta, dPhi, xRange, yRange, zRange, binNumber](const double& D0_PT, const double& D0_eta, const double& D0_phi){
			int iIndex, jIndex, kIndex;
                        iIndex = 0;
                        jIndex = 0;
                        kIndex = 0;

                        double PT = xRange[0];
                        double eta = yRange[0];
                        double phi = zRange[0];

			for (size_t i = 0; i < binNumber; i++){
				if ((D0_PT > PT) && (D0_PT < (PT + dPT))){
					iIndex = i;
				}
				PT += dPT;
			}

                        for (size_t j = 0; j < binNumber; j++){
                                if ((D0_eta > eta) && (D0_eta < (eta + dEta))){
                                        jIndex = j;
                                }
                                eta += dEta;
                        }

			for (size_t k = 0; k < binNumber; k++){
				if ((D0_phi > phi) && (D0_phi < (phi + dPhi))){
					kIndex = k;
				}
				phi += dPhi;
			}


			return weights[iIndex][jIndex][kIndex];
		}, {"D0_PT", "D0_eta", "D0_phi"}		
	);

	dataFrameWeighted.Snapshot(treeName, fileNameNew);
}

ROOT::RDF::RResultPtr<TH3D> normDistribution(std::string fileName, std::string treeName, int binNumber, std::vector<double> xRange, std::vector<double> yRange, std::vector<double> zRange){
        ROOT::RDataFrame dataFrame(treeName, fileName);

        auto hist = dataFrame.Histo3D(
                {(fileName).c_str(), "", binNumber, xRange[0], xRange[1], binNumber, yRange[0], yRange[1], binNumber, zRange[0], zRange[1]}, "D0_PT", "D0_eta","D0_phi"
        );
        hist->Scale(1.0/hist->Integral());
        return hist;
}

std::vector<std::vector<std::vector<double>>> getWeights(std::vector<ROOT::RDF::RResultPtr<TH3D>> distributions){
        // // distributions[1] -> KK
        // // distributions[0] -> ππ

        // // w = Γ_{ππ}/Γ_{KK} -> Γ_{ππ} = w*Γ_{KK}
        
        int xSize, ySize, zSize;
        xSize = distributions[0]->GetNbinsX();
        ySize = distributions[0]->GetNbinsY();
        zSize = distributions[0]->GetNbinsX();
        
        double weight;
        std::vector<std::vector<std::vector<double>>> weights(xSize, std::vector<std::vector<double>>(ySize, std::vector<double>(zSize)));
        for (size_t i = 0; i < distributions[0]->GetNbinsX(); i++){
                for (size_t j = 0; j < distributions[0]->GetNbinsY(); j++){
                        for (size_t k = 0; k < distributions[0]->GetNbinsZ(); k++){
				if (distributions[1]->GetBinContent(i + 1, j + 1, k + 1) == 0.0){
					weight = 0.0;
				}
				else{
					weight = distributions[0]->GetBinContent(i + 1, j + 1, k + 1)/distributions[1]->GetBinContent(i + 1, j + 1, k + 1);
				}
                                weights[i][j][k] = weight;
                        }
                }
        }

        return weights;
}

void plotWeightedKinematics(std::string fileNameNew, std::string treeName, std::vector<std::string> particles, std::vector<std::string> observables, std::vector<std::string> labels, std::vector<std::vector<double>> ranges){

        ROOT::RDataFrame dataFrame(treeName, fileNameNew);
        TCanvas* canvasNew = new TCanvas("canvasNew", "");
        TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);

        for (const auto& particle : particles){
                for (size_t i = 0; i < observables.size(); i++){
                        auto histUnweighted = dataFrame.Histo1D(
                                {(particle + observables[i] + "Unweighted").c_str(), "", 50, ranges[i][0], ranges[i][1]
                                }, (particle + "_" + observables[i]).c_str()
                        );

                        auto histWeighted = dataFrame.Histo1D(
                                {(particle + observables[i] + "Weighted").c_str(), "", 50, ranges[i][0], ranges[i][1]
                                }, (particle + "_" + observables[i]).c_str(), "Weight"
                        );

                        histUnweighted->SetStats(0);
                        histWeighted->SetStats(0);

                        histUnweighted->SetLineColor(kBlack);
                        histWeighted->SetLineColor(kRed);

                        histUnweighted->GetXaxis()->SetTitle(("D^{0}#rightarrow#pi^{-}#pi^{+}, " + labels[i]).c_str());

                        legend->AddEntry(histUnweighted.GetPtr(), "Unweighted", "l");
                        legend->AddEntry(histWeighted.GetPtr(), "Weighted", "l");
                        legend->SetFillColor(0);
                        legend->SetBorderSize(0);

                        histUnweighted->DrawNormalized("HIST");
                        histWeighted->DrawNormalized("HIST SAME");
                        legend->Draw();
                        canvasNew->SaveAs(("Plots/" + particle + observables[i] + ".pdf").c_str());

                        legend->Clear();
                }
        }

}


int main(){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        gROOT->ProcessLine(".L ../ChargedPi/lhcbStyle.C");
        gStyle->SetPalette(55);
        ROOT::EnableImplicitMT();
        
        std::vector<std::string> fileNames = {
                "../MomentumDependence/Dstp_D0__pimpip_pip_sPi_tree.root",
                "../MomentumDependence/Dstp_D0__KmKp_pip_sPi_tree.root"
        };
        // std::vector<std::string> filters = {
        //         "sPi_C == 1",
        //         "sPi_C == -1"
        // };
        std::vector<ROOT::RDF::RResultPtr<TH3D>> distributions;
        std::vector<std::string> fileNamesNew = {
                "Dstp_D0__pimpip_pip_sPi_weighted_tree.root",
                "Dstp_D0__KmKp_pip_sPi_weighted_tree.root"
        };
        std::vector<std::string> plotNames = {
                "Plots/pimpipNormalized.pdf",
                "Plots/KmKpNormalized.pdf"
        };
        std::string treeName = "DecayTree";

        int binNumber = 15;
        std::vector<double> xRange = {
                0.0,
                20.0
        };
        std::vector<double> yRange = {
                0.0,
                7.0
        };
        std::vector<double> zRange = {
                -3.5,
                3.5
        };
        TCanvas* canvas = new TCanvas("canvas", "");
        for (size_t i = 0; i < fileNames.size(); i++){
                distributions.push_back(normDistribution(fileNames[i], treeName, binNumber, xRange, yRange, zRange));
                distributions[i]->SetStats(0);
                distributions[i]->GetXaxis()->SetTitle("P_{T} GeV/c");
                distributions[i]->GetYaxis()->SetTitle("#eta");
                distributions[i]->GetZaxis()->SetTitle("#phi Rad");
                distributions[i]->Draw("LEGO2");
                canvas->SaveAs(plotNames[i].c_str());
        }

        // auto weights = getWeights(distributions);

	// for (size_t i = 0; i < fileNames.size(); i++){
	// 	assignWeights(fileNames[i], fileNamesNew[i], treeName, binNumber, xRange, yRange, zRange, weights);
	// }

        // std::vector<std::string> particles = {"D0"};
        // std::vector<std::string> observables = {"PT", "eta", "phi"};
        // std::vector<std::string> labels = {"p_{T} GeV/c", "#eta", "#phi Rad"};
        // std::vector<std::vector<double>> ranges {xRange, yRange, zRange};

        // plotWeightedKinematics(fileNamesNew[0], treeName, particles, observables, labels, ranges);
        // compareResults(fileNamesNew, treeName, ranges);

        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << std::endl << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}
