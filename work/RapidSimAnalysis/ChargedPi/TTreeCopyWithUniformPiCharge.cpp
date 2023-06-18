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

#include <iostream>

int main(){
        gROOT->ProcessLine(".L lhcbStyle.C");

        TString fileName = "../FirstPlots/Dstp_D0__KmKp_pip_tree.root";
        TString fileNameOutput = "Dstp_D0_KmKp_pip_ChargedPi_tree.root";
        TString treeName = "DecayTree";
        TString branchNameNew = "sPi_C";

        TFile *file = TFile::Open(fileName);
        TTree *tree = dynamic_cast<TTree*>(file->Get(treeName));

        TFile *outFile = new TFile(fileNameOutput, "RECREATE"); // Open new file in write mode
        TTree *outTree = tree->CloneTree(); // Copy the tree to outTree

        Int_t charge;
        TBranch *newBranch = outTree->Branch(branchNameNew, &charge, branchNameNew + "/I");
        TRandom3 *random = new TRandom3();
        random->SetSeed(); // Set computer time as seed
        Double_t randomNumber;

        TCanvas *canvas = new TCanvas("canvas", "");
        TH1F *hist = new TH1F("hist", "", 10, -2, 2);

        for (size_t i = 0; i < tree->GetEntries(); i++){
                randomNumber = random->Uniform();
                if (randomNumber > 0.5){
                        charge = +1;
                }
                else{
                        charge = -1;
                }
                newBranch->Fill();
                hist->Fill(charge);
        }

        outTree->GetListOfBranches()->Add(newBranch);

        hist->GetYaxis()->SetTitle("Counts");
        hist->GetXaxis()->SetTitle("soft #pi charge");
        hist->SetStats(0);

        hist->Draw();
        canvas->SaveAs("Plots/sPi_Charge.pdf");

        outTree->Write();
        outFile->Close();
        file->Close();

        return 0;
}