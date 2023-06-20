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
#include <TRandom3.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <chrono>

int main(){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        gROOT->ProcessLine(".L lhcbStyle.C");

        TString fileName = "Dstp_D0__KmKp_pip_WithCPAsymmetry.root";
        TString treeName = "DecayTree";
        TString branchName = "sPi_C";
        TString plotName = "Plots/sPi_CPAsymmetryCharge0.50.pdf";

        TFile *file = TFile::Open(fileName);
        TTree *tree = dynamic_cast<TTree*>(file->Get(treeName));
        TH1F *hist = new TH1F("hist", "", 10, -2, 2);
        TCanvas *canvas = new TCanvas("canvas", "");

        Int_t entry;
        tree->SetBranchAddress(branchName, &entry);

        for (size_t i = 0; i < tree->GetEntries(); i++){
                tree->GetEntry(i);
                hist->Fill(entry);
        }

        hist->GetYaxis()->SetTitle("Counts");
        hist->GetXaxis()->SetTitle("soft #pi charge");
        hist->SetStats(0);
        hist->Draw();

        canvas->SaveAs(plotName);

        file->Close();


        // Stop timer and calculate elapsed time
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "Elapsed time: " << duration.count() << " seconds" << std::endl;
        return 0;
}