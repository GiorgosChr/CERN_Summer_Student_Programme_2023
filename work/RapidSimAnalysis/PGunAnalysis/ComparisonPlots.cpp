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

void comparisonPlots(std::vector<std::string> fileNames, std::string treeName, std::vector<double> ranges, int binNumber, std::string observable, std::string label, std::vector<double> legendPosition);

int main(){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        gROOT->ProcessLine(".L ../ChargedPi/lhcbStyle.C");
        gStyle->SetPalette(55);
        ROOT::EnableImplicitMT();

        std::vector<std::string> fileNames = {
                "KK_Dst_reduced_weighted_from_D0.root",
                "KK_Dst_reduced_weighted_from_Dst.root",
                "pipi_Dst_reduced.root"
        };
        std::string treeName = "DecayTree";

        int binNumber = 25;
        std::vector<std::string> observables = {
                "D0_PT",
                "D0_ETA",
                "D0_PHI",
                "sPi_PT",
                "sPi_ETA",
                "sPi_PHI"
        };
        std::vector<std::string> labels = {
                "p_{T}(D^{0}) MeV/c",
                "#eta(D^{0})",
                "#phi(D^{0}) Rad",
                "p_{T}(#pi) MeV/c",
                "#eta(#pi)",
                "#phi(#pi) Rad"
        };
        std::vector<std::vector<double>> legendPosition = {
                {0.5, 0.7, 0.7, 0.9},
                {0.2, 0.7, 0.4, 0.9},
                {0.4, 0.2, 0.6, 0.4},
                {0.5, 0.7, 0.7, 0.9},
                {0.2, 0.7, 0.4, 0.9},
                {0.4, 0.2, 0.6, 0.4}
        };
        std::vector<std::vector<double>> ranges = {
                {0.0, 15000},
                {0.0, 6.1},
                {-3.2, 3.2},
                {0.0, 1500},
                {0.0, 6.1},
                {-3.2, 3.2}
        };

        for (size_t i = 0; i < observables.size(); i ++){
                comparisonPlots(fileNames, treeName, ranges[i], binNumber, observables[i], labels[i], legendPosition[i]);
        }

        TCanvas* canvas = new TCanvas("canvas", "");
        TLegend* legend = new TLegend(0.6, 0.7, 0.7, 0.9);

        ROOT::RDataFrame dataFrameKD0(treeName, fileNames[0]);
        ROOT::RDataFrame dataFrameKDst(treeName, fileNames[1]);

        auto histD0 = dataFrameKD0.Histo1D(
                {"hist", "", 50, 0.5, 1.5},
                "Weight"
        );
        auto histDst = dataFrameKDst.Histo1D(
                {"hist", "", 50, 0.5, 1.5},
                "Weight"
        );

        histD0->SetStats(0);
        histDst->SetStats(0);

        histD0->SetLineColor(kRed);
        histD0->GetXaxis()->SetTitle("Weights");

        histD0->SetMarkerSize(0);
        histDst->SetMarkerSize(0);
        histD0->DrawNormalized("HIST E");
        histDst->DrawNormalized("HIST E SAME");

        legend->AddEntry(histD0.GetPtr(), "New Technique", "l");
        legend->AddEntry(histDst.GetPtr(), "D*/#pi_{s} Reconstruction", "l");
        legend->SetFillColor(0);
        legend->SetBorderSize(0);
        legend->SetTextSize(0.05);
        legend->Draw();

        canvas->SaveAs("Plots/Weights.pdf");

        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << std::endl << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}

void comparisonPlots(std::vector<std::string> fileNames, std::string treeName, std::vector<double> ranges, int binNumber, std::string observable, std::string label, std::vector<double> legendPosition){
        TCanvas* canvas = new TCanvas("canvas", "");
        TLegend* legend = new TLegend(legendPosition[0], legendPosition[1], legendPosition[2], legendPosition[3]);
        ROOT::RDataFrame dataFrameKD0(treeName, fileNames[0]);
        ROOT::RDataFrame dataFrameKDst(treeName, fileNames[1]);
        ROOT::RDataFrame dataFrameP(treeName, fileNames[2]);

        auto histKD0Weighted = dataFrameKD0.Histo1D(
                {"hist", "", binNumber, ranges[0], ranges[1]},
                observable.c_str(), "Weight"
        );
        auto histKD0 = dataFrameKD0.Histo1D(
                {"hist", "", binNumber, ranges[0], ranges[1]},
                observable.c_str()
        );

        auto histKDstWeighted = dataFrameKDst.Histo1D(
                {"hist", "", binNumber, ranges[0], ranges[1]},
                observable.c_str(), "Weight"
        );
        auto histKDst = dataFrameKDst.Histo1D(
                {"hist", "", binNumber, ranges[0], ranges[1]},
                observable.c_str()
        );

        auto histP = dataFrameP.Histo1D(
                {"hist", "", binNumber, ranges[0], ranges[1]},
                observable.c_str()
        );

        histKD0->SetStats(0);
        histKD0Weighted->SetStats(0);
        histKDst->SetStats(0);
        histKDstWeighted->SetStats(0);
        histP->SetStats(0);

        histKD0Weighted->Sumw2();
        histKDstWeighted->Sumw2();

        histP->SetLineColor(kBlack);
        histKD0->SetLineColor(kGreen);
        histKD0Weighted->SetLineColor(kRed);
        histKDst->SetLineColor(kOrange);
        histKDstWeighted->SetLineColor(kBlue);

        histP->SetMarkerSize(0);
        histKD0->SetMarkerSize(0);
        histKD0Weighted->SetMarkerSize(0);
        histKDst->SetMarkerSize(0);
        histKDstWeighted->SetMarkerSize(0);

        histP->GetXaxis()->SetTitle(label.c_str());
        histP->DrawNormalized("HIST E");
        histKD0->DrawNormalized("HIST E SAME");
        histKD0Weighted->DrawNormalized("HIST E SAME");
        // histKDst->DrawNormalized("HIST E SAME");
        histKDstWeighted->DrawNormalized("HIST E SAME");

        legend->AddEntry(histP.GetPtr(), "D^{0}#rightarrow #pi^{-}#pi^{+}", "l");
        legend->AddEntry(histKD0.GetPtr(), "D^{0}#rightarrow K^{-}K^{+}", "l");
        legend->AddEntry(histKD0Weighted.GetPtr(), "D^{0}#rightarrow K^{-}K^{+}, New Technique", "l");
        legend->AddEntry(histKDstWeighted.GetPtr(), "D^{0}#rightarrow K^{-}K^{+}, D*/#pi_{s} Reconstr.", "l");
        legend->SetFillColor(0);
        legend->SetBorderSize(0);
        legend->SetTextSize(0.05);
        legend->Draw();

        canvas->SaveAs(("Plots/" + observable + ".pdf").c_str());
}
