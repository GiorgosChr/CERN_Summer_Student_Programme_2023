#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>
#include <TCanvas.h>
#include <TStyle.h>


#include <iostream>
#include <sstream>
#include <chrono>
#include <cmath>

ROOT::RDF::RResultPtr<TH3D> normDistribution(std::string fileName, std::string treeName){
        ROOT::RDataFrame dataFrame(treeName, fileName);

        int binNumber = 15;
        std::vector<double> xRange = {
                -0.0,
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
        auto hist = dataFrame.Histo3D(
                {(fileName).c_str(), "", binNumber, xRange[0], xRange[1], binNumber, yRange[0], yRange[1], binNumber, zRange[0], zRange[1]}, "D0_PT", "D0_eta","D0_phi"
        );
        hist->Scale(1.0/hist->Integral());
        return hist;
}

std::vector<std::vector<std::vector<double>>> getWeights(std::vector<ROOT::RDF::RResultPtr<TH3D>> distributions){
        // // distributions[0] -> KK
        // // distributions[1] -> ππ

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
                                weight = distributions[1]->GetBinContent(i, j, k)/distributions[0]->GetBinContent(i, j, k);
                                weights[i][j][k] = weight;
                                
                        }
                }
        }

        return weights;
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
                "Dstp_D0__KmKp_pip_sPi_weighted_tree.root",
                "Dstp_D0__pimpip_pip_sPi_weighted_tree.root"
        };
        std::vector<std::string> plotNames = {
                "Plots/KmKpNormalized.pdf",
                "Plots/pimpipNormalized.pdf"
        };
        std::string treeName = "DecayTree";

        TCanvas* canvas = new TCanvas("canvas", "");
        for (size_t i = 0; i < fileNames.size(); i++){
                distributions.push_back(normDistribution(fileNames[i], treeName));
                distributions[i]->SetStats(0);
                distributions[i]->GetXaxis()->SetTitle("P_{T} GeV/c");
                distributions[i]->GetYaxis()->SetTitle("#eta");
                distributions[i]->GetZaxis()->SetTitle("#phi");
                distributions[i]->Draw("LEGO2");
                canvas->SaveAs(plotNames[i].c_str());
        }

        auto weights = getWeights(distributions);


        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << std::endl << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}