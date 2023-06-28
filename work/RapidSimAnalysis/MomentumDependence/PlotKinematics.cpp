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
#include <THStack.h>


#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <chrono>




int main(){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        gROOT->ProcessLine(".L ../ChargedPi/lhcbStyle.C");

        std::string treeName, fileNameK, fileNameP;
        treeName = "DecayTree";
        fileNameK = "../FirstPlots/Dstp_D0__KmKp_pip_tree.root";
        fileNameP = "../ChargedPi/Dstp_D0__pimpip_pip_tree.root";

        std::string branchNameX, branchNameZ;
        branchNameX = "sPi_PX";
        branchNameZ = "sPi_PZ";

        ROOT::RDataFrame dataFrameK(treeName, fileNameK);
        ROOT::RDataFrame dataFrameP(treeName, fileNameP);

        auto histK = dataFrameK.Histo2D({"histK", "Histogram 1", 150, 0.5, 6, 150, -0.4, 0.4},branchNameZ, branchNameX);
        auto histP = dataFrameP.Histo2D({"histP", "Histogram 2", 150, 0.5, 6, 150, -0.4, 0.4},branchNameZ, branchNameX);
        histK->SetStats(0);
        histP->SetStats(0);

        TCanvas* canvas = new TCanvas("canvas", "", 1500, 600);
        canvas->Divide(2, 1);
        gStyle->SetPalette(55);
        
        histK->GetXaxis()->SetTitle("p_{z} [GeV/c]");
        histK->GetYaxis()->SetTitle("p_{x} [GeV/c]");
        // histK->GetYaxis()->SetTitleOffset(0.5);
        // histK->GetYaxis()->SetLabelOffset(0.02);
        canvas->cd(1);
        // histK->SetTitle("Title1");
        histK->DrawNormalized("COLZ");

        histP->GetXaxis()->SetTitle("p_{z} [GeV/c]");
        histP->GetYaxis()->SetTitle("p_{x} [GeV/c]");
        canvas->cd(2);
        histP->DrawNormalized("COLZ");
        canvas->SaveAs("Plots/ComparisonNormalizedPXPZ.pdf");

        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}
