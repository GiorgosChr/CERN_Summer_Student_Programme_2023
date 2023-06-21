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

TTree* getTree(TString fileName, TString treeName){
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

        auto treeOld = getTree(fileNameOld, treeName);

        // TFile *fileOld = TFile::Open(fileNameOld);
        // TTree *treeOld = dynamic_cast<TTree*>(fileOld->Get(treeName));

        // TFile *fileInterm = new TFile(fileNameInterm, "RECREATE");
        // TTree *treeInterm = treeOld->CloneTree();
        // // Create new Int_t TBranch to attach to the intermediate TTree
        // Int_t sPi_Charge;
        // TBranch *branchCharge = treeInterm->Branch(branchName, &sPi_Charge, branchName + "/I");

        // // Generate random numbers with TRandom3 to assign charge to the soft pion
        // TRandom3 *random = new TRandom3();
        // random->SetSeed();

        // Double_t asymmetry, probability, randomNum;
        // asymmetry = 0.25;
        // probability = (1.0 + asymmetry) / 2.0;

        // for (size_t i = 0; i < treeOld->GetEntries(); i++){
        //         randomNum = random->Uniform();

        //         if (randomNum < probability){
        //                 sPi_Charge = +1;
        //         }
        //         else{
        //                 sPi_Charge = -1;
        //         }
        //         branchCharge->Fill();
        // }


        // fileOld->Close();

        // Stop timer and calculate elapsed time
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}