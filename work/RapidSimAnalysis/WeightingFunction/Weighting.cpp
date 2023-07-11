#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>
#include <TCanvas.h>
#include <TStyle.h>


#include <iostream>
#include <sstream>
#include <chrono>
#include <cmath>

ROOT::RDF::RResultPtr<TH3D> normDistribution(std::string fileName, std::string treeName, std::string filter){
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
        auto hist = dataFrame.Filter(
                filter
        ).Histo3D(
                {(fileName + filter).c_str(), "", binNumber, xRange[0], xRange[1], binNumber, yRange[0], yRange[1], binNumber, zRange[0], zRange[1]}, "D0_PT", "D0_eta","D0_phi"
        );
        hist->Scale(1.0/hist->Integral());
        return hist;
};

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
        std::vector<std::string> filters = {
                "sPi_C == 1",
                "sPi_C == -1"
        };
        std::vector<ROOT::RDF::RResultPtr<TH3D>> distributions;
        std::vector<std::string> fileNamesNew = {
                "Dstp_D0__KmKp_pip_sPi_weighted_tree.root",
                "Dstp_D0__pimpip_pip_sPi_weighted_tree.root"
        };
        std::vector<std::string> plotNames = {
                "Plots/KmKpPositive.pdf",
                "Plots/KmKpNegative.pdf",
                "Plots/pimpipPositive.pdf",
                "Plots/pimpipNegative.pdf"
        };
        std::string treeName = "DecayTree";

        TCanvas* canvas = new TCanvas("canvas", "");
        int k = 0;
        for (size_t i = 0; i < fileNames.size(); i++){
                for (size_t j = 0; j < filters.size(); j++){
                        distributions.push_back(normDistribution(fileNames[i], treeName, filters[j]));
                        distributions[k]->SetStats(0);
                        distributions[k]->GetXaxis()->SetTitle("P_{T} GeV/c");
                        distributions[k]->GetYaxis()->SetTitle("#eta");
                        distributions[k]->GetZaxis()->SetTitle("#phi");
                        distributions[k]->Draw("LEGO2");
                        canvas->SaveAs(plotNames[k].c_str());
                        k += 1;
                }
        }


        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << std::endl << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}