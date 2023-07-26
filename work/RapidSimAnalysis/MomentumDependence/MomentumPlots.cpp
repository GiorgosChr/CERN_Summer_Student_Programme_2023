#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TBranch.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TH2D.h>
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
        gStyle->SetPalette(55);
        ROOT::EnableImplicitMT();

        std::vector<std::string> fileNames = {
                "Dstp_D0__KmKp_pip_sPi_tree.root",
                "KK_Dst_reduced_weighted_from_Dst.root",
        };
        std::string treeName = "DecayTree";

        int binNumber = 100;

        ROOT::RDataFrame dataFrame(treeName, fileNames[0]);
        TCanvas* canvas = new TCanvas("canvas", "");

        auto histPos = dataFrame.Filter(
                "sPi_C == 1"
        ).Histo2D(
                {"histPos", "", binNumber, 0, 15, binNumber, -0.400, 0.400}, "sPi_PZ", "sPi_PX"
        );
        auto histNeg = dataFrame.Filter(
                "sPi_C == -1"
        ).Histo2D(
                {"histNeg", "", binNumber, 0, 15, binNumber, -0.400, 0.400}, "sPi_PZ", "sPi_PX"
        );

        histPos->SetStats(0);
        histNeg->SetStats(0);

        histPos->GetYaxis()->SetTitle("p_{x}(#pi^{+}) MeV/c");
        histPos->GetXaxis()->SetTitle("p_{z}(#pi^{+}) MeV/c");

        histNeg->GetYaxis()->SetTitle("p_{x}(#pi^{-}) MeV/c");
        histNeg->GetXaxis()->SetTitle("p_{z}(#pi^{-}) MeV/c");

        histPos->DrawNormalized("COLZ");
        // histNeg->DrawNormalized("COLZ");
        canvas->SaveAs("Plots/KK_Dst_PXPZ_Positive.pdf");


        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << std::endl << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}