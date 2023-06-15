#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TBranch.h>
#include <TH1F.h>
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

        TString branchName = "D0_M";

        auto binWidth = 0.002;
        auto binMin = 1.83;
        auto binMax = 1.9;
        auto binNumber = int((binMax - binMin)/binWidth);

        TH1F* hist = new TH1F("hist", "", binNumber, binMin, binMax);
  
        Double_t entry;
        tree->SetBranchAddress(branchName, &entry);
        for (size_t i = 0; i < tree->GetEntries(); i++){
                tree->GetEntry(i);
                hist->Fill(entry);
        }

        hist->GetYaxis()->SetTitle("Events/2MeV/c^{2}");
        hist->GetXaxis()->SetTitle("m(D^{0})");
        hist->Draw();
        canvas->SaveAs("Plots/" + branchName + ".pdf");

        branchName = "Dst_M";
        binWidth = 0.002;
        binMin = 1.83;
        binMax = 1.9;
        binNumber = int((binMax - binMin)/binWidth);

        delete hist;
  
        TH1F* hist = new TH1F("hist", "", binNumber, binMin, binMax);

        tree->SetBranchAddress(branchName, &entry);
        for (size_t i = 0; i < tree->GetEntries(); i++){
                tree->GetEntry(i);
                hist->Fill(entry);
        }

        hist->GetYaxis()->SetTitle("Events/2MeV/c^{2}");
        hist->GetXaxis()->SetTitle("m(D^{*})");
        hist->Draw();
        canvas->SaveAs("Plots/" + branchName + ".pdf");
        file->Close();
        return 0;
}
