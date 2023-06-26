#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TBranch.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TH1.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TLatex.h>
#include <TMathText.h>
#include <TRandom3.h>

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <chrono>

std::string convertTStringToString(const TString& tstring) {
    const char* cstr = tstring.Data();
    std::string str(cstr);
    return str;
}

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
        canvas->SaveAs(("Plots/pimpip/sPi_ProductionAsymmetryCharge" + asymmetryString + ".pdf").c_str());

        // Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "addAsymmetryBranch() Elapsed time: " << duration.count() << " sec" << std::endl;
}

void skipEvents(std::string fileName, std::string fileNameNew, std::string treeName, std::string branchName, Double_t asymmetry, std::string asymmetryType){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        // ROOT::EnableImplicitMT(); // Enable parallelization
        ROOT::RDataFrame dataFrame(treeName, fileName);
        auto branch = dataFrame.Take<Int_t>(branchName);

        ROOT::RVec<ULong64_t> entriesToRemove;
        TRandom3 *random = new TRandom3();
        random->SetSeed();

        Double_t probability, randomNum;
        probability = 1.0 - (1.0 - asymmetry)/(1.0 + asymmetry);

        Int_t index = 0;

        for (auto& entry : branch){
                randomNum = random->Uniform();
                if (entry == -1 && randomNum < probability){
                        entriesToRemove.push_back(static_cast<ULong64_t>(index));
                }
                index ++;
        }

        

        // auto dataFrameFiltered = dataFrame.Filter([&entriesToRemove](Int_t entryNumber){
        //         return std::find(entriesToRemove.begin(), entriesToRemove.end(), entryNumber) == entriesToRemove.end();;
        // }, {branchName});
        auto dataFrameNew = dataFrame.Define("RemoveEntry", [&entriesToRemove](ULong64_t entryIndex) {
                return std::find(entriesToRemove.begin(), entriesToRemove.end(), entryIndex) != entriesToRemove.end();
        }, {"rdfentry_"});

        // ROOT::RVec<Double_t> binedges;
        // Double_t edge = -2.0;
        // for (size_t i = 0; i < 11; i++){
        //         binedges.push_back(edge);
        //         edge += 0.4;
        // }

        TCanvas *canvas = new TCanvas("canvasNew", "");
        // TH1D *hist = new TH1D("hist", "", 10, -2, 2);
        auto dataFrameFiltered = dataFrameNew.Filter("RemoveEntry == false");
        auto hist = dataFrameFiltered.Histo1D({"hist", "", 10, -2, 2}, branchName);

        // auto countNeg = dataFrameFiltered.Filter([](Int_t value) {return  value = -1;}, branchName).Count().GetValue();
        // auto countPos = *dataFrameFiltered.Filter([](Int_t value) {return  value = 1;}, branchName).Count()

        auto countPos = dataFrameFiltered.Filter("sPi_C==1").Count().GetValue();
        auto countNeg = dataFrameFiltered.Filter("sPi_C==-1").Count().GetValue();

        Double_t errorPos, errorNeg;

        errorPos = TMath::Power(countPos, 0.5);
        errorNeg = TMath::Power(countNeg, 0.5);

        std::cout << "Number of positive pions: " << countPos << std::endl;
        std::cout << "Number of negative pions: " << countNeg << std::endl;
        // std::cout << "Error of positive pions: " << errorPos << std::endl;
        // std::cout << "Error of negative pions: " << errorNeg << std::endl;
        
        Double_t finalAsymmetry = static_cast<Double_t>(countPos - countNeg)/static_cast<Double_t>(countPos + countNeg);
        Double_t finalAsymmetryError, derivPos, derivNeg;
        derivPos = 1.0/(static_cast<Double_t>(countPos + countNeg)) - (static_cast<Double_t>(countPos - countNeg))/TMath::Power((static_cast<Double_t>(countPos + countNeg)), 2);
        derivPos =  - 1.0/(static_cast<Double_t>(countPos + countNeg)) - (static_cast<Double_t>(countPos - countNeg))/TMath::Power((static_cast<Double_t>(countPos + countNeg)), 2);
        finalAsymmetryError = TMath::Power(TMath::Power(derivPos * errorPos, 2.0) + TMath::Power(derivNeg * errorNeg, 2.0), 0.5);

        std::cout << "Final asymmetry: " << finalAsymmetry << " +/- "<< finalAsymmetryError << std::endl;


        hist->GetYaxis()->SetTitle("Counts");
        hist->GetXaxis()->SetTitle("soft #pi charge");
        hist->SetStats(0);
        hist->Draw();
        // hist->DrawClone();

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << asymmetry;
        std::string asymmetryString = oss.str();
        canvas->SaveAs(("Plots/pimpip/sPi_" + asymmetryType + "AsymmetryCharge" + asymmetryString + ".pdf").c_str());

        std::vector<std::string> branchesToSave = dataFrameFiltered.GetColumnNames();
        branchesToSave.erase(std::remove(branchesToSave.begin(), branchesToSave.end(), "RemoveEntry"), branchesToSave.end());


        dataFrameFiltered.Snapshot(treeName, fileNameNew, branchesToSave);

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "skipEvents() Elapsed time: " << duration.count() << " sec" << std::endl;
}


int main(){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        gROOT->ProcessLine(".L lhcbStyle.C");

        TString fileNameOld, fileNameNew, fileNameInterm, treeName, branchName;
        fileNameOld = "Dstp_D0__pimpip_pip_tree.root";
        fileNameNew = "Dstp_D0__pimpip_pip_CPAsymmetry_tree.root";
        fileNameInterm = "Dstp_D0__pimpip_pip_ProductionAsymmetry_tree.root";
        treeName = "DecayTree";
        branchName = "sPi_C";

        TTree* treeOld = getTree(fileNameOld, treeName);

        TFile* fileInterm = new TFile(fileNameInterm, "RECREATE");
        TTree* treeInterm = treeOld->CloneTree();

        Double_t asymmetry = 0.1;
        std::string asymmetryType;
        addAsymmetryBranch(treeInterm, branchName, asymmetry);

        // std::cout << "Saving intermediate file..." << std::endl;
        fileInterm->cd();
        treeInterm->Write();
        // std::cout << "Closing intermediate files..." << std::endl;
        fileInterm->Close();
        asymmetry = 0.3;
        asymmetryType = "CP";
        skipEvents(convertTStringToString(fileNameInterm), convertTStringToString(fileNameNew), convertTStringToString(treeName), convertTStringToString(branchName), asymmetry, asymmetryType);

        fileNameInterm = fileNameNew;
        fileNameNew = "Dstp_D0__pimpip_pip_DetectionAsymmetry_tree.root";
        asymmetry = 0.1;
        asymmetryType = "Detection";
        skipEvents(convertTStringToString(fileNameInterm), convertTStringToString(fileNameNew), convertTStringToString(treeName), convertTStringToString(branchName), asymmetry, asymmetryType);

        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}
