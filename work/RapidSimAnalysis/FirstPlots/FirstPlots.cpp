#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TBranch.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TLatex.h>
#include <TMathText.h>

#include <iostream>

int main() {
        gROOT->ProcessLine(".L lhcbStyle.C");

        TString fileName = "Dstp_D0__KmKp_pip_tree.root";
        TString treeName = "DecayTree";

        TFile *file = TFile::Open(fileName);
        TTree *tree = dynamic_cast<TTree*>(file->Get(treeName));
        TCanvas* canvas = new TCanvas("canvas", "Histogram Canvas");

        TString branchNameZero = "D0_M";
        TString branchNameSt = "Dst_M";
        TString branchNameDiff = "DstD0Diff";
        TString branchNamePt = "D0_PT";
        TString branchNameSPiPt = "sPi_PT";
        TString branchNameP1Pt = "P1_PT";
        TString branchNameP2Pt = "P2_PT";
        TString branchNamePx = "D0_PX";
        TString branchNamePz = "D0_PZ";
        
        Double_t entryZero, entrySt, entryPt, entrySPiPt, entryP1Pt, entryP2Pt, entryPx, entryPz;
        Double_t histValue;
        tree->SetBranchAddress(branchNameZero, &entryZero);
        tree->SetBranchAddress(branchNameSt, &entrySt);
        tree->SetBranchAddress(branchNamePt, &entryPt);
        tree->SetBranchAddress(branchNameSPiPt, &entrySPiPt);
        tree->SetBranchAddress(branchNameP1Pt, &entryP1Pt);
        tree->SetBranchAddress(branchNameP2Pt, &entryP2Pt);
        tree->SetBranchAddress(branchNamePx, &entryPx);
        tree->SetBranchAddress(branchNamePz, &entryPz);

        Double_t binNumber, binMin, binMax, binWidth;
        binWidth = 0.002; // 2MeV/c^2 bin width
        binMin = 1.83;
        binMax = 1.9;
        binNumber = int((binMax - binMin)/binWidth);
        TH1F* histZero = new TH1F("histZero", "", binNumber, binMin, binMax);

        binMin = 1.98;
        binMax = 2.06;
        binNumber = int((binMax - binMin)/binWidth);
        TH1F* histSt = new TH1F("histSt", "", binNumber, binMin, binMax);

        binWidth = 0.001;
        binMin = 0.14;
        binMax = 0.16;
        binNumber = int((binMax - binMin)/binWidth);
        TH1F* histDiff = new TH1F("histDiff", "", binNumber, binMin, binMax);

        binWidth = 0.02;
        binMin = 0;
        binMax = 10;
        binNumber = int((binMax - binMin)/binWidth);
        TH1F* histPt = new TH1F("histPt", "", binNumber, binMin, binMax);

        binWidth = 0.02;
        binMin = 0;
        binMax = 2;
        binNumber = int((binMax - binMin)/binWidth);
        TH1F* histSPiPt = new TH1F("histSPiPt", "", binNumber, binMin, binMax);

        binWidth = 0.02;
        binMin = 0;
        binMax = 7;
        binNumber = int((binMax - binMin)/binWidth);
        TH1F* histP1Pt = new TH1F("histP1Pt", "", binNumber, binMin, binMax);

        binWidth = 0.02;
        binMin = 0;
        binMax = 7;
        binNumber = int((binMax - binMin)/binWidth);
        TH1F* histP2Pt = new TH1F("histP2Pt", "", binNumber, binMin, binMax);

        binWidth = 0.1;
        binMin = 3;
        binMax = 30;
        binNumber = int((binMax - binMin)/binWidth);
        Double_t binMinY, binMaxY, binNumberY;
        binMinY = -5;
        binMaxY = -binMinY;
        binNumberY = int((binMaxY - binMinY)/binWidth);
        TH2F* histPxPz = new TH2F("histPxPz", "", binNumber, binMin, binMax, binNumberY, binMinY, binMaxY);

        for (size_t i = 0; i < tree->GetEntries(); i++){
                tree->GetEntry(i);
                histZero->Fill(entryZero);
                histSt->Fill(entrySt);
                histDiff->Fill(entrySt - entryZero);
                histPt->Fill(entryPt);
                histSPiPt->Fill(entrySPiPt);
                histP1Pt->Fill(entryP1Pt);
                histP2Pt->Fill(entryP2Pt);
                histPxPz->Fill(entryPz, entryPx);
        }

        histZero->GetYaxis()->SetTitle("Events/(2MeV/c^{2})");
        histZero->GetXaxis()->SetTitle("m(D^{0}) [GeV/c^{2}]");
        histZero->Draw();
        canvas->SaveAs("Plots/" + branchNameZero + ".pdf");

        histSt->GetYaxis()->SetTitle("Events/(2MeV/c^{2})");
        histSt->GetXaxis()->SetTitle("m(D^{*}) [GeV/c^{2}]");
        histSt->Draw();
        canvas->SaveAs("Plots/" + branchNameSt + ".pdf");

        histDiff->GetYaxis()->SetTitle("Events/(1MeV/c^{2})");
        histDiff->GetXaxis()->SetTitle("m(D^{*}) - m(D^{0}) [GeV/c^{2}]");
        histDiff->Draw();
        canvas->SaveAs("Plots/" + branchNameDiff + ".pdf");

        histPt->GetYaxis()->SetTitle("Events/(20MeV/c^{2})");
        histPt->GetXaxis()->SetTitle( "P_{T}(D^{0}) [GeV/c]");
        histPt->Draw();
        canvas->SaveAs("Plots/" + branchNamePt + ".pdf");

        histSPiPt->GetYaxis()->SetTitle("Events/(20MeV/c^{2})");
        histSPiPt->GetXaxis()->SetTitle( "P_{T}(#pi^{+}) [GeV/c]");
        histSPiPt->Draw();
        canvas->SaveAs("Plots/" + branchNameSPiPt + ".pdf");

        histP1Pt->GetYaxis()->SetTitle("Events/(20MeV/c^{2})");
        histP1Pt->GetXaxis()->SetTitle( "P_{T}(#pi_{1}) [GeV/c]");
        histP1Pt->Draw();
        canvas->SaveAs("Plots/" + branchNameP1Pt + ".pdf");

        histP2Pt->GetYaxis()->SetTitle("Events/(20MeV/c^{2})");
        histP2Pt->GetXaxis()->SetTitle( "P_{T}(#pi_{2}) [GeV/c]");
        histP2Pt->Draw();
        canvas->SaveAs("Plots/" + branchNameP2Pt + ".pdf");

        gStyle->SetPalette(55);
        histPxPz->GetYaxis()->SetTitle("P_{x}(D^{0}) [GeV/c]");
        histPxPz->GetXaxis()->SetTitle( "P_{z}(D^{0}) [GeV/c]");
        histPxPz->Draw("colz");
        canvas->SaveAs("Plots/" + branchNamePx + "PZ" + ".pdf");
        file->Close();
        return 0;
}
