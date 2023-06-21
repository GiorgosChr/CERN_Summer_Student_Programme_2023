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

#include <ROOT/RDataFrame.hxx>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <chrono>

TTree *getTree(TString fileName, TString treeName){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        TFile *file = TFile::Open(fileName);
        TTree *tree = dynamic_cast<TTree*>(file->Get(treeName));

        // Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "getTree() Elapsed time: " << duration.count() << " sec" << std::endl;

        return tree;
}

void addAsymmetryBranch(TTree *tree, TString branchName, Double_t asymmetry){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        Int_t entry;
        TBranch *branchCharge = tree->Branch(branchName, &entry, branchName + "/I");
        TCanvas *canvas = new TCanvas("canvas", "");
        TH1F *hist = new TH1F("hist", "", 10, -2, 2);

        // Generate random numbers to assign charge
        TRandom3 *random = new TRandom3();
        random->SetSeed();
        Double_t randomNum, probability;
        probability = (1.0 + asymmetry)/2.0;

        for (size_t i = 0; i < tree->GetEntries(); i++){
                randomNum = random->Uniform();
                if (randomNum < probability){
                        entry = 1;
                }
                else{
                        entry = -1;
                }
                branchCharge->Fill();
                hist->Fill(entry);
        }

        hist->GetYaxis()->SetTitle("Counts");
        hist->GetXaxis()->SetTitle("soft #pi charge");
        hist->SetStats(0);
        hist->Draw();

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << asymmetry;
        std::string asymmetryString = oss.str();
        canvas->SaveAs(("Plots/sPi_ProductionAsymmetryCharge" + asymmetryString + ".pdf").c_str());

        // Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "addAsymmetryBranch() Elapsed time: " << duration.count() << " sec" << std::endl;
}

// void skipBranches(TString fileName, TString treeName, TString branchName, Double_t asymmetry){
//         // Start timer
//         std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

//         TFile *file = new TFile(fileName, "READ");
//         ROOT::RDataFrame df(treeName, file);

//         // Stop timer
//         std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//         std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
//         std::cout << "addAsymmetryBranch() Elapsed time: " << duration.count() << " sec" << std::endl;
// }


int main(){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        gROOT->ProcessLine(".L lhcbStyle.C");

        TString fileNameOld, fileNameNew, fileNameInterm, treeName, branchName;
        fileNameOld = "../FirstPlots/Dstp_D0__KmKp_pip_tree.root";
        fileNameNew = "Dstp_D0__KmKp_pip_CPAsymmetry_tree.root";
        fileNameInterm = "Dstp_D0__KmKp_pip_ProductionAsymmetry_tree.root";
        treeName = "DecayTree";
        branchName = "sPi_C";

        TTree* treeOld = getTree(fileNameOld, treeName);

        TFile* fileInterm = new TFile(fileNameInterm, "RECREATE");
        TTree* treeInterm = treeOld->CloneTree();

        Double_t asymmetry = 0.25;
        addAsymmetryBranch(treeInterm, branchName, asymmetry);

        std::cout << "Saving/Closing files..." << std::endl;
        fileInterm->cd();
        treeInterm->Write();
        fileInterm->Close();


        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}