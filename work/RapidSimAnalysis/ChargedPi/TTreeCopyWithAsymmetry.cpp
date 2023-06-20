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

        TString fileName = "../FirstPlots/Dstp_D0__KmKp_pip_tree.root";
        TString fileNameOutput = "Dstp_D0__KmKp_pip_WithProductionAsymmetry.root";
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
        Double_t randomNumber, asymmetry;
        asymmetry = 0.25;

        TCanvas *canvas = new TCanvas("canvas", "");
        TH1F *hist = new TH1F("hist", "", 10, -2, 2);

        // To emulate the production asymmetry of 25% we assume that 75% of pions are
        // positively charged and the remaining ones are negatively charged.
        // Subsequently, we emulate a CP asymmetry of 50% and remove positively charged 
        // pions according to the new distribution.

        for (size_t i = 0; i < tree->GetEntries(); i++){
                randomNumber = random->Uniform();
                if (randomNumber > asymmetry){
                        charge = +1;
                }
                else{
                        charge = -1;
                }
                newBranch->Fill();
                hist->Fill(charge);
        }


        hist->GetYaxis()->SetTitle("Counts");
        hist->GetXaxis()->SetTitle("soft #pi charge");
        hist->SetStats(0);

        hist->Draw();

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << asymmetry;
        std::string asymmetryString = oss.str();
        canvas->SaveAs(("Plots/sPi_ProductionAsymmetryCharge" + asymmetryString + ".pdf").c_str());


        outTree->Write();
        outFile->Close();
        file->Close();

        delete file, outTree, outFile, tree, newBranch;

        fileName = "Dstp_D0__KmKp_pip_WithProductionAsymmetry.root";
        fileNameOutput = "Dstp_D0__KmKp_pip_WithCPAsymmetry.root";
        treeName = "DecayTree";

        TFile *fileOld = TFile::Open(fileName);
        TFile *fileNew = new TFile(fileNameOutput, "RECREATE");
        TTree *treeOld = dynamic_cast<TTree*>(fileOld->Get(treeName));
        TTree *treeNew = treeOld->CloneTree(0);
        TBranch *branchCharge = treeOld->GetBranch("sPi_C");

        Bool_t sPi_C;
        branchCharge->SetAddress(&sPi_C);

        asymmetry = 0.5;
        for (size_t i = 0; i < treeOld->GetEntries(); i++){
                treeOld->GetEntry(i);

                if (sPi_C && random->Uniform() < asymmetry){
                        continue;
                }
                treeNew->Fill();
        }

        fileNew->cd();
        treeNew->Write();

        fileNew->Close();
        fileOld->Close();


        // Stop timer and calculate elapsed time
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "Elapsed time: " << duration.count() << " seconds" << std::endl;
        return 0;
}