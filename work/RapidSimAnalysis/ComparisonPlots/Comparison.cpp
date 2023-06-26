#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TBranch.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TH1.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TLatex.h>
#include <TMathText.h>
#include <TRandom3.h>
#include <TLegend.h>

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <chrono>

void compareHistograms(std::string fileNameK, std::string fileNameP, std::string treeName, std::string branchName, std::string axisLabel, std::vector<double> range){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        ROOT::RDataFrame dataFrameK(treeName, fileNameK);
        ROOT::RDataFrame dataFrameP(treeName, fileNameP);

        TCanvas *canvas = new TCanvas("canvas", "");
        TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);

        auto histoK = dataFrameK.Histo1D({"histk", "", 100, range[0], range[1]}, branchName);
        auto histoP = dataFrameP.Histo1D({"histp", "", 100, range[0], range[1]}, branchName);

        auto label = static_cast<TString>(axisLabel);
        histoK->GetYaxis()->SetTitle("Events");        
        histoP->GetYaxis()->SetTitle("Events");
        histoK->GetXaxis()->SetTitle(label);        
        histoP->GetXaxis()->SetTitle(label);
        histoP->SetLineColor(kRed);
        histoK->SetStats(0);
        histoP->SetStats(0);

        // double normK = 1.0 / histoK->Integral();
        // double normP = 1.0 / histoP->Integral();
        // histoK->Scale(normK);
        // histoP->Scale(normP);

        legend->AddEntry(histoK.GetPtr(), "D^{0}#rightarrow K^{-}K^{+}", "l");
        legend->AddEntry(histoP.GetPtr(), "D^{0}#rightarrow #pi^{-}#pi^{+}", "l");
        legend->SetFillColor(0);
        legend->SetBorderSize(0);

        histoK->DrawNormalized("HIST");  // Draw histoK as a normalized histogram with lines
        histoP->DrawNormalized("HIST SAME");
        legend->Draw();
        canvas->SaveAs(("Plots/Comparison" + branchName + ".pdf").c_str());

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "compareHistograms() Elapsed time: " << duration.count() << " sec" << std::endl;
}

int main(){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        gROOT->ProcessLine(".L ../ChargedPi/lhcbStyle.C");

        std::vector<std::vector<double>> range = {{0, 150}, {0, 10}, {-4, 5}, {0, 10}, {0, 10}, {0, 1}, {-4, 5}, {0, 10}};
        std::string fileNameK, fileNameP, treeName;
        std::vector<std::string> branchNames = {"Dst_P", "Dst_PT", "Dst_phi", "Dst_eta", "sPi_P", "sPi_PT", "sPi_phi", "sPi_eta"};
        std::vector<std::string> axisLabel = {"p(D^{*}) GeV/c", "p_{T}(D^{*}) GeV/c", "#phi(D^{*}) Rad", "#eta(D^{*})", "p(#pi) GeV/c", "p_{T}(#pi) GeV/c", "#phi(#pi) Rad", "#eta(#pi)"};
        fileNameK = "../FirstPlots/Dstp_D0__KmKp_pip_tree.root";
        fileNameP = "../ChargedPi/Dstp_D0__pimpip_pip_tree.root";
        treeName = "DecayTree";
        
        for (size_t i = 0; i < branchNames.size(); i++){
                compareHistograms(fileNameK, fileNameP, treeName, branchNames[i], axisLabel[i], range[i]);
        }

        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}