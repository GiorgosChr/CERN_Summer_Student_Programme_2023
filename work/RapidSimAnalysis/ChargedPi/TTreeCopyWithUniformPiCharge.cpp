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

int main(){
        gROOT->ProcessLine(".L lhcbStyle.C");

        TString fileName = "../FirstPlots/Dstp_D0__KmKp_pip_tree.root";
        TString treeName = "DecayTree";
                
        TFile *file = TFile::Open(fileName);
        TTree *tree = dynamic_cast<TTree*>(file->Get(treeName));

        file->Close();

        return 0;
}