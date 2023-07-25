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

int main(){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        gROOT->ProcessLine(".L ../ChargedPi/lhcbStyle.C");
        gStyle->SetPalette(55);
        ROOT::EnableImplicitMT();

        ROOT::RDataFrame dataFrameBefore("DecayTree", "Dstp_D0__KmKp_pip_tree_HighStatistics_Analysis_Asymmetry_Weights.root");
        ROOT::RDataFrame dataFrameAfter("DecayTree", "../WeightingFunction/Dstp_D0__KmKp_pip_tree_HighStatistics_Analysis_Asymmetry_Weights.root");

        TCanvas* canvas = new TCanvas("canvas", "");
        TLegend* legend = new TLegend(0.2, 0.7, 0.4, 0.9);

        auto histBefore = dataFrameBefore.Histo1D(
                {"hist", "", 50, 0, 1.5}, "Weight"
        );
        auto histAfter = dataFrameAfter.Histo1D(
                {"hist", "", 50, 0, 1.5}, "Weight"
        );

        histBefore->SetStats(0);
        histAfter->SetStats(0);

        histBefore->SetMarkerSize(0);
        histAfter->SetMarkerSize(0);

        histAfter->SetLineColor(kRed);

        histAfter->GetXaxis()->SetTitle("Weights");

        histBefore->GetXaxis()->SetTitle("Weights");
        histAfter->GetXaxis()->SetTitle("Weights");
        histBefore->DrawNormalized("HIST E");
        histAfter->DrawNormalized("HIST E SAME");

        legend->AddEntry(histBefore.GetPtr(), "Before", "l");
        legend->AddEntry(histAfter.GetPtr(), "After", "l");

        legend->SetFillColor(0);
        legend->SetBorderSize(0);
        legend->SetTextSize(0.05);
        legend->Draw();

        canvas->SaveAs("Plots/WeighsBeforeAfter.pdf");
        


        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << std::endl << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}