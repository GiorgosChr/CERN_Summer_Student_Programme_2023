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

void plotDistribution(ROOT::RDF::RResultPtr<TH3D> distribution, std::string plotName);

void calculateWeights(std::vector<ROOT::RDF::RResultPtr<TH3D>> normDistributions, std::vector<std::string> fileNamesLowStatisticsAsymmetry, std::vector<std::string> fileNamesLowStatisticsAsymmetryWeights, std::string treeName, std::vector<std::vector<double>> filters, int binNumber);

void assignWeights(std::string fileNameLowStatisticsAsymmetry, std::string fileNameLowStatisticsAsymmetryWeights, std::string treeName, std::vector<std::vector<std::vector<double>>> weights, std::vector<std::vector<double>> filters, int binNumber);

void compareKinematics(std::vector<std::string> fileNamesLowStatisticsAsymmetryWeights, std::string treeName, std::vector<double> filters, int binNumber, std::string observable, std::string label, std::vector<double> legendPosition);

void comparisonPlots(std::vector<std::string> fileNamesLowStatisticsAsymmetryWeights, std::string treeName, std::vector<double> filters, int binNumber, std::string observable, std::string label, std::vector<double> legendPosition);

void plotWeights(std::string fileName, std::string treeName);

int main(){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        gROOT->ProcessLine(".L ../ChargedPi/lhcbStyle.C");
        gStyle->SetPalette(55);
        ROOT::EnableImplicitMT();

        std::vector<std::string> fileNamesHighStatisticsAsymmetry = {
                "Dstp_D0__KmKp_pip_tree_HighStatistics_Asymmetry.root",
                "Dstp_D0__pimpip_pip_tree_HighStatistics_Asymmetry.root"
        };
        std::vector<std::string> fileNamesLowStatisticsAsymmetry = {
                "Dstp_D0__KmKp_pip_tree_HighStatistics_Analysis_Asymmetry.root",
                "Dstp_D0__pimpip_pip_tree_HighStatistics_Analysis_Asymmetry.root"
        };
        std::vector<std::string> fileNamesLowStatisticsAsymmetryWeights = {
                "Dstp_D0__KmKp_pip_tree_HighStatistics_Analysis_Asymmetry_Weights.root",
                "Dstp_D0__pimpip_pip_tree_HighStatistics_Analysis_Asymmetry_Weights.root"
        };
        std::string treeName = "DecayTree";
        std::vector<std::string> distributionNames = {
                "Plots/KmKpNormalizedDistribution.pdf",
                "Plots/pimpipNormalizedDistribution.pdf"
        };
        std::vector<ROOT::RDF::RResultPtr<TH3D>> normDistributions;
        std::vector<std::vector<double>> filters = {
                {0.0, 15},
                {0.0, 6.1},
                {-3.2, 3.2}
        };
        int binNumber = 25;

        for (size_t i = 0; i < fileNamesHighStatisticsAsymmetry.size(); i++){
                normDistributions.push_back(
                        normDistribution(fileNamesHighStatisticsAsymmetry[i], treeName, binNumber, filters)
                );
                plotDistribution(normDistributions[i], distributionNames[i]);

        }

        calculateWeights(normDistributions, fileNamesLowStatisticsAsymmetry, fileNamesLowStatisticsAsymmetryWeights, treeName, filters, binNumber);

        
        std::vector<std::string> observables = {
                "D0_PT",
                "D0_eta",
                "D0_phi",
                "Dst_PT",
                "Dst_eta",
                "Dst_phi",
                "sPi_PT",
                "sPi_eta",
                "sPi_phi"
        };
        std::vector<std::string> labels = {
                "p_{T}(D^{0}) GeV/c",
                "#eta(D^{0})",
                "#phi(D^{0}) Rad",
                "p_{T}(D^{*}) GeV/c",
                "#eta(D^{*})",
                "#phi(D^{*}) Rad",
                "p_{T}(#pi) GeV/c",
                "#eta(#pi)",
                "#phi(#pi) Rad"
        };
        std::vector<std::vector<double>> legendPosition = {
                {0.5, 0.7, 0.7, 0.9},
                {0.2, 0.7, 0.4, 0.9},
                {0.4, 0.4, 0.6, 0.6},
                {0.5, 0.7, 0.7, 0.9},
                {0.2, 0.7, 0.4, 0.9},
                {0.4, 0.4, 0.6, 0.6},
                {0.5, 0.7, 0.7, 0.9},
                {0.2, 0.7, 0.4, 0.9},
                {0.4, 0.4, 0.6, 0.6}
        };
        std::vector<std::vector<double>> ranges = {
                {0.0, 15},
                {0.0, 6.1},
                {-3.2, 3.2},
                {0.0, 15},
                {0.0, 6.1},
                {-3.2, 3.2},
                {0.0, 1.5},
                {0.0, 6.1},
                {-3.2, 3.2}
        };
        for (size_t i = 0; i < observables.size(); i++){
                compareKinematics(fileNamesLowStatisticsAsymmetryWeights, treeName, ranges[i], binNumber, observables[i], labels[i], legendPosition[i]);
                comparisonPlots(fileNamesLowStatisticsAsymmetryWeights, treeName, ranges[i], binNumber, observables[i], labels[i], legendPosition[i]);
        }

        plotWeights(fileNamesLowStatisticsAsymmetryWeights[0], treeName);

        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << std::endl << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}


ROOT::RDF::RResultPtr<TH3D> normDistribution(std::string fileName, std::string treeName, int binNumber, std::vector<std::vector<double>> filters){
        ROOT::RDataFrame dataFrame(treeName, fileName);

        auto hist = dataFrame.Histo3D(
                {(fileName).c_str(), "", binNumber, filters[0][0], filters[0][1], binNumber, filters[1][0], filters[1][1], binNumber, filters[2][0], filters[2][1]}, "D0_PT", "D0_eta","D0_phi"
        );
        hist->Scale(1.0/hist->Integral());
        return hist;
}

void plotDistribution(ROOT::RDF::RResultPtr<TH3D> distribution, std::string plotName){
        TCanvas* canvas = new TCanvas("canvas", "");
        distribution->SetStats(0);
        distribution->GetXaxis()->SetTitle("P_{T} GeV/c");
        distribution->GetYaxis()->SetTitle("#eta");
        distribution->GetZaxis()->SetTitle("#phi Rad");
        distribution->Draw("LEGO2");
        canvas->SaveAs(plotName.c_str());
}

void calculateWeights(std::vector<ROOT::RDF::RResultPtr<TH3D>> normDistributions, std::vector<std::string> fileNamesLowStatisticsAsymmetry, std::vector<std::string> fileNamesLowStatisticsAsymmetryWeights, std::string treeName, std::vector<std::vector<double>> filters, int binNumber){
        // // normDistributions[0] = KmKp
        // // normDistributions[1] = pimpip

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

        for (size_t i = 0; i < fileNamesLowStatisticsAsymmetryWeights.size(); i++){
                assignWeights(fileNamesLowStatisticsAsymmetry[i], fileNamesLowStatisticsAsymmetryWeights[i], treeName, weights, filters, binNumber);
        }
}

void assignWeights(std::string fileNameLowStatisticsAsymmetry, std::string fileNameLowStatisticsAsymmetryWeights, std::string treeName, std::vector<std::vector<std::vector<double>>> weights, std::vector<std::vector<double>> filters, int binNumber){
        double dPT = (filters[0][1] - filters[0][0])/static_cast<double>(binNumber);
	double dEta = (filters[1][1] - filters[1][0])/static_cast<double>(binNumber);
	double dPhi = (filters[2][1] - filters[2][0])/static_cast<double>(binNumber);

        ROOT::RDataFrame dataFrame(treeName, fileNameLowStatisticsAsymmetry);

        auto dataFrameWeighted = dataFrame.Filter(
		[filters](const double& D0_PT, const double& D0_eta, const double& D0_phi){
			if (D0_PT >= filters[0][0] && D0_PT <= filters[0][1] &&  D0_eta >= filters[1][0] && D0_eta <= filters[1][1] &&  D0_phi >= filters[2][0] && D0_phi <= filters[2][1]){
				return true;
			}
			return false;
		}, {"D0_PT", "D0_eta", "D0_phi"}
	).Define(
		"Weight", [weights, dPT, dEta, dPhi, filters, binNumber](const double& D0_PT, const double& D0_eta, const double& D0_phi){
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

	dataFrameWeighted.Snapshot(treeName, fileNameLowStatisticsAsymmetryWeights);
}

void compareKinematics(std::vector<std::string> fileNamesLowStatisticsAsymmetryWeights, std::string treeName, std::vector<double> filters, int binNumber, std::string observable, std::string label, std::vector<double> legendPosition){
        ROOT::RDataFrame dataFrameK(treeName, fileNamesLowStatisticsAsymmetryWeights[0]);
        ROOT::RDataFrame dataFrameP(treeName, fileNamesLowStatisticsAsymmetryWeights[1]);
        
        std::vector<ROOT::RDataFrame> dataFrames= {
                dataFrameK,
                dataFrameP
        };

        TCanvas* canvas = new TCanvas("canvas", "");
        TLegend *legend = new TLegend(legendPosition[0],legendPosition[1], legendPosition[2], legendPosition[3]);

        auto histK = dataFrames[0].Histo1D(
                {"histK", "", binNumber, filters[0], filters[1]}, observable.c_str(), "Weight"
        );
        auto histP = dataFrames[1].Histo1D(
                {"histP", "", binNumber, filters[0], filters[1]}, observable.c_str()
        );

        histK->SetStats(0);
        histP->SetStats(0);

        histK->SetLineColor(kBlack);
        histP->SetLineColor(kRed);

        histK->GetXaxis()->SetTitle(label.c_str());
        histK->Sumw2();

        histK->SetMarkerSize(0);
        histP->SetMarkerSize(0);
        histK->DrawNormalized("HIST E");
        histP->DrawNormalized("HIST E SAME");

        legend->AddEntry(histK.GetPtr(), "D^{0}#rightarrow K^{-}K^{+}", "l");
        legend->AddEntry(histP.GetPtr(), "D^{0}#rightarrow #pi^{-}#pi^{+}", "l");
        legend->SetFillColor(0);
        legend->SetBorderSize(0);
        legend->SetTextSize(0.05);
        legend->Draw();
        // canvas->BuildLegend();
        canvas->SaveAs(("Plots/" + observable + "_Comparison.pdf").c_str());
}

void comparisonPlots(std::vector<std::string> fileNamesLowStatisticsAsymmetryWeights, std::string treeName, std::vector<double> filters, int binNumber, std::string observable, std::string label, std::vector<double> legendPosition){
        ROOT::RDataFrame dataFrameK(treeName, fileNamesLowStatisticsAsymmetryWeights[0]);
        ROOT::RDataFrame dataFrameP(treeName, fileNamesLowStatisticsAsymmetryWeights[1]);
        
        std::vector<ROOT::RDataFrame> dataFrames= {
                dataFrameK,
                dataFrameP
        };

        TCanvas* canvas = new TCanvas("canvas", "");
        TLegend *legend = new TLegend(legendPosition[0],legendPosition[1], legendPosition[2], legendPosition[3]);

        auto histKWeight = dataFrames[0].Histo1D(
                {"histKWeight", "", binNumber, filters[0], filters[1]}, observable.c_str(), "Weight"
        );
        auto histK = dataFrames[0].Histo1D(
                {"histK", "", binNumber, filters[0], filters[1]}, observable.c_str()
        );
        auto histP = dataFrames[1].Histo1D(
                {"histP", "", binNumber, filters[0], filters[1]}, observable.c_str()
        );

        histKWeight->SetStats(0);
        histK->SetStats(0);
        histP->SetStats(0);

        histKWeight->SetLineColor(kRed);
        histK->SetLineColor(kGreen);
        histP->SetLineColor(kBlack);

        histKWeight->GetXaxis()->SetTitle(label.c_str());
        histKWeight->Sumw2();

        histKWeight->SetMarkerSize(0);
        histK->SetMarkerSize(0);
        histP->SetMarkerSize(0);
        histKWeight->DrawNormalized("HIST E");
        histK->DrawNormalized("HIST E SAME");
        histP->DrawNormalized("HIST E SAME");

        legend->AddEntry(histKWeight.GetPtr(), "D^{0}#rightarrow K^{-}K^{+} Weighted", "l");
        legend->AddEntry(histK.GetPtr(), "D^{0}#rightarrow K^{-}K^{+}", "l");
        legend->AddEntry(histP.GetPtr(), "D^{0}#rightarrow #pi^{-}#pi^{+}", "l");
        legend->SetFillColor(0);
        legend->SetBorderSize(0);
        legend->SetTextSize(0.05);
        legend->Draw();

        canvas->SaveAs(("Plots/" + observable + "_All.pdf").c_str());
}

void plotWeights(std::string fileName, std::string treeName){
        ROOT::RDataFrame dataFrame(treeName, fileName);
        TCanvas* canvas = new TCanvas("canvas", "");
        auto hist = dataFrame.Histo1D(
                {"hist", "", 50, 0, 1.5},
                "Weight"
        );
        hist->SetStats(0);
        hist->GetXaxis()->SetTitle("Weight");
        hist->Draw("HIST");
        canvas->SaveAs("Plots/Weights.pdf");
}