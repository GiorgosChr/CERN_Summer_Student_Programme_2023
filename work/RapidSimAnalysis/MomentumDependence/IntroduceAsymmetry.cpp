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
#include <cmath>

double detection(const double& slope, const double& sPi_PZ, const double& sPi_PX){
        double line = slope * sPi_PZ;

        if (std::abs(sPi_PX) > line){
                return 1.0;
        }

        return 0.0;
}

std::vector<double> integratedAsymmetry(ROOT::RDF::RResultPtr<TH2D> hist, double slope, int binNumber, std::vector<double> ranges){
        std::vector<double> asymmetry = {0.0, 0.0}; // integrated asymmetry and asymmetry error
        double dPX, dPZ;
        double PX, PZ;
        dPX = (ranges[3] - ranges[2])/(static_cast<double>(binNumber));
        dPZ = (ranges[1] - ranges[0])/(static_cast<double>(binNumber));

        PX = -0.4;
        PZ = 0.0;

        for (size_t i = 0; i < binNumber; i++){ // runs over sPi_PX
                PX += dPX;
                for (size_t j = 0; j < binNumber; j++){ // runs over sPi_PZ
                        PZ += dPZ;
                        if (detection(slope, PZ, PX) == 1.0){
                                asymmetry[0] += hist->GetBinContent(j, i);
                        }
                        asymmetry[1] += hist->GetBinContent(j, i);

                }
                PZ = 0.0;
        }

        std::cout << "Integrated detection asymmetry: " << asymmetry[0]/asymmetry[1] << std::endl;

        return asymmetry;
}

void detectionAsymmetry(const std::string fileName, const std::string fileNameNew, const std::string fileNameCP, const std::string treeName, const std::string branchNameX, const std::string branchNameZ, const std::string decay, std::string pion, double asymmetry){
        // // Start timer
        // std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        ROOT::RDataFrame dataFrame(treeName, fileName);
        TRandom3* random = new TRandom3();
        random->SetSeed();

        auto dataFrameNew = dataFrame.Define(
                "sPi_C", [random](){return (random->Uniform() < 0.5) ? -1 : 1;}
        );

        double slope = 0.1;
        auto dataFrameFiltered = dataFrameNew.Filter([slope](int& sPi_C, double& sPi_PX, double& sPi_PZ){
                if (sPi_C == -1 && (std::abs(sPi_PX) > slope*sPi_PZ)){
                        return false;
                }
                return true;
                }, {"sPi_C", "sPi_PX", "sPi_PZ"}
        );

        int binNumber = 100;
        
        auto hist = dataFrameFiltered.Histo2D({"hist", "", binNumber, 0, 6, binNumber, -0.4, 0.4}, "sPi_PZ", "sPi_PX");
        auto histPi = dataFrameFiltered.Histo1D({"histPi", "", 10, -2, 2}, "sPi_C");
        auto histBefore = dataFrameNew.Histo2D({"histBefore", "", binNumber, 0, 6, binNumber, -0.4, 0.4}, "sPi_PZ", "sPi_PX");
        auto histPiBefore = dataFrameNew.Histo1D({"histPiBefore", "", 10, -2, 2}, "sPi_C");

        auto histPositive = dataFrameFiltered.Filter("sPi_C==1").Histo2D({"histPositive", "", binNumber, 0, 6, binNumber, -0.4, 0.4}, "sPi_PZ", "sPi_PX");
        auto histNegative = dataFrameFiltered.Filter("sPi_C==-1").Histo2D({"histNegative", "", binNumber, 0, 6, binNumber, -0.4, 0.4}, "sPi_PZ", "sPi_PX");

        TCanvas* canvas = new TCanvas("canvas", "", 1500, 600);
        canvas->Divide(2, 1);
        canvas->cd(1);

        gStyle->SetPalette(55);
        hist->SetStats(0);
        hist->GetYaxis()->SetTitle("p_{x}(#pi) [GeV/c]");
        hist->GetXaxis()->SetTitle("After p_{z}(#pi) [GeV/c]");
        hist->Draw("COLZ");

        canvas->cd(2);
        histBefore->SetStats(0);
        histBefore->GetYaxis()->SetTitle("p_{x}(#pi) [GeV/c]");
        histBefore->GetXaxis()->SetTitle("Before p_{z}(#pi) [GeV/c]");
        histBefore->Draw("COLZ");
        canvas->SaveAs(decay.c_str());


        TCanvas* canvasPi = new TCanvas("canvasPi", "");
        canvasPi->cd();
        histPiBefore->SetStats(0);
        histPiBefore->GetXaxis()->SetTitle("soft #pi charge");
        histPiBefore->Draw("hist");
        canvasPi->SaveAs((pion + "Before.pdf").c_str());
        histPi->SetStats(0);
        histPi->GetXaxis()->SetTitle("soft #pi charge");
        histPi->Draw("hist");
        canvasPi->SaveAs((pion + "Detection.pdf").c_str());

        histPositive->SetStats(0);
        histPositive->GetYaxis()->SetTitle("p_{x}(#pi) [GeV/c]");
        histPositive->GetXaxis()->SetTitle(" p_{z}(#pi^{+}) [GeV/c]");
        histPositive->Draw("COLZ");
        canvasPi->SaveAs((pion + "Positive.pdf").c_str());

        histNegative->SetStats(0);
        histNegative->GetYaxis()->SetTitle("p_{x}(#pi) [GeV/c]");
        histNegative->GetXaxis()->SetTitle(" p_{z}(#pi^{-}) [GeV/c]");
        histNegative->Draw("COLZ");
        canvasPi->SaveAs((pion + "Negative.pdf").c_str());

        double probability;
        probability = 1.0 - (1.0 - asymmetry)/(1.0 + asymmetry);

        auto dataFrameCP = dataFrameFiltered.Filter(
                [random, probability](int& sPi_C){
                        if ((sPi_C == -1) && (random->Uniform() < probability)){
                                return false;
                        }
                        return true;
                }, {"sPi_C"}
        );

        auto histPiCP = dataFrameCP.Histo1D({"histPiCP", "", 10, -2 ,2}, "sPi_C");
        histPiCP->SetStats(0);
        histPiCP->GetXaxis()->SetTitle("soft #pi charge");
        histPiCP->Draw("hist");
        canvasPi->SaveAs((pion + "CP.pdf").c_str());



        // Calculate the total asymmetry (detection and CP)
        auto countPos = dataFrameCP.Filter("sPi_C==1").Count().GetValue();
        auto countNeg = dataFrameCP.Filter("sPi_C==-1").Count().GetValue();

        auto errorPos = std::pow(countPos, 0.5);
        auto errorNeg = std::pow(countNeg, 0.5);

        // std::cout << "Number of positive pions: " << countPos << std::endl;
        // std::cout << "Number of negative pions: " << countNeg << std::endl;

        double finalAsymmetry = static_cast<double>(countPos - countNeg)/static_cast<double>(countPos + countNeg);
        double finalAsymmetryError, derivPos, derivNeg;
        derivPos = 1.0/(static_cast<double>(countPos + countNeg)) - (static_cast<double>(countPos - countNeg))/TMath::Power((static_cast<double>(countPos + countNeg)), 2);
        derivPos =  - 1.0/(static_cast<double>(countPos + countNeg)) - (static_cast<double>(countPos - countNeg))/TMath::Power((static_cast<double>(countPos + countNeg)), 2);
        finalAsymmetryError = TMath::Power(TMath::Power(derivPos * errorPos, 2.0) + TMath::Power(derivNeg * errorNeg, 2.0), 0.5);

        std::cout << "Final asymmetry: " << finalAsymmetry << " +/- "<< finalAsymmetryError << std::endl;

        std::vector<double> ranges = {0.0, 6.0, -0.4, 0.4};
        auto intDetectionAsymmetry = integratedAsymmetry(hist, slope, binNumber, ranges);

        double expectedAsymmetry = asymmetry*intDetectionAsymmetry[0]/intDetectionAsymmetry[1] / (1.0 + asymmetry*intDetectionAsymmetry[0]/intDetectionAsymmetry[1]);
        std::cout << "Expected final asymmetry: " << expectedAsymmetry << std::endl;


        // Save the filtered dataframe to a root file
        // dataFrameFiltered.Snapshot(treeName, fileNameNew);
        // std::cout << dataFrameFiltered.Count().GetValue() << std::endl;

        // //  Stop timer
        // std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        // std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        // std::cout << "detectionAsymmetry() Elapsed time: " << duration.count() << " sec" << std::endl;
}


int main(){
        // Start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        gROOT->ProcessLine(".L ../ChargedPi/lhcbStyle.C");

        std::string fileNameK, fileNameP, fileNameNewK, fileNameNewP, treeName, branchNameX, branchNameZ;
        std::vector<std::string> fileNames;
        std::vector<std::string> filenamesNew;
        std::vector<std::string> fileNamesCP;
        std::vector<std::string> decays;
        std::vector<std::string> pions;
        std::vector<double> asymmetries;
        // std::vector<std::string> filters {"sPi_C == -1 && abs(sPi_PX) < 0.15 * sPi_PZ", "sPi_C == -1 && abs(sPi_PX) < 0.15 * sPi_PZ"};
        // std::vector<std::string> filters {"!(abs(sPi_PX) < 0.15 * sPi_PZ && sPi_C == -1)", "!(abs(sPi_PX) < 0.15 * sPi_PZ && sPi_C == -1)"};

        treeName = "DecayTree";
        fileNameK = "../FirstPlots/Dstp_D0__KmKp_pip_tree.root";
        fileNameP = "../ChargedPi/Dstp_D0__pimpip_pip_tree.root";
        fileNameNewK = "Dstp_D0__KmKp_pip_sPi_tree.root";
        fileNameNewP = "Dstp_D0__pimpip_pip_sPi_tree.root";
        branchNameX = "sPi_PX";
        branchNameZ = "sPi_PZ";

        fileNames.push_back(fileNameK);
        fileNames.push_back(fileNameP);
        filenamesNew.push_back(fileNameNewK);
        filenamesNew.push_back(fileNameNewP);
        decays.push_back("Plots/sPi_PXPZ_KmKp_Detection.pdf");
        decays.push_back("Plots/sPi_PXPZ_pimpip_Detection.pdf");
        pions.push_back("Plots/sPi_C_KmKp");
        pions.push_back("Plots/sPi_C_pimpip");
        fileNamesCP.push_back("Dstp_D0__KmKp_pip_sPiCP.root");
        fileNamesCP.push_back("Dstp_D0__pimpip_pip_sPiCP.root");
        asymmetries.push_back(0.2);
        asymmetries.push_back(0.3);

        for (size_t i = 0; i < fileNames.size(); i++){
                detectionAsymmetry(fileNames[i], filenamesNew[i], fileNamesCP[i], treeName, branchNameX, branchNameZ, decays[i], pions[i], asymmetries[i]);
        }



        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << std::endl << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}