#include <TFile.h>
#include <TTree.h>
#include <TString.h>

#include <iostream>

int main() {
        // Read the .root file and the DecayTree from the file

        TString fileName = "Dstp_D0__KmKp_pip_tree.root";
        TFile *file = TFile::Open(fileName);
        TTree *tree = dynamic_cast<TTree*>(file->Get("DecayTree"));

        TString branchName = "D0_M";
        auto branch = tree->Branch(branchName);


        file->Close();
        return 0;
}
