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
        // // Returns either 1.0 or 0.0 according to the user defined line
        // // If 1 then in that region we have 100% detection asymmetry
        // // If 0 then we have no asymmetry
        double line = slope * sPi_PZ;

        if (std::abs(sPi_PX) > line){
                return 1.0;
        }

        return 0.0;
}

std::vector<double> integratedAsymmetry(ROOT::RDF::RResultPtr<TH2D> hist, double slope, int binNumber, double xMin,double xMax, double zMin, double zMax){
        // // Returns the integrated detection asymmetry and the corresponding error
        // // according to a user defined line with a slope

        std::vector<double> asymmetry = {0.0, 0.0}; // asymmetry, error
        double dPX, dPZ;
        double PX, PZ;
        dPX = (xMax - xMin)/(static_cast<double>(binNumber));
        dPZ = (zMax - zMin)/(static_cast<double>(binNumber));

        // define the two integrals
        double integralA, integralN;
        double integralError;
        integralA = 0.0;
        integralN = 0.0;
        integralError == 0.0;

        // calculate the momentum at the bin center
        PX = -0.4 + dPX/2.0;
        PZ = 0.0 + dPX/2.0;

        for (size_t i = 0; i < binNumber; i++){ // runs over sPi_PX
                for (size_t j = 0; j < binNumber; j++){ // runs over sPi_PZ
                        integralA += detection(slope, PZ, PX) * hist->GetBinContent(j, i);
                        integralN += hist->GetBinContent(j, i);
                        PZ += dPZ;
                }
                PX += dPX;
                PZ = 0.0 + dPX/2.0;
        }

        // calculate the momentum at the bin center
        PX = -0.4 + dPX/2.0;
        PZ = 0.0 + dPX/2.0;

        for (size_t i = 0; i < binNumber; i++){ // runs over sPi_PX
                for (size_t j = 0; j < binNumber; j++){ // runs over sPi_PZ
                        integralError += std::pow(detection(slope, PZ, PX)*integralN - integralA, 2.0) * hist->GetBinContent(j, i);
                        PZ += dPZ;
                }
                PX += dPX;
                PZ = 0.0 + dPX/2.0;
        }
        integralError = std::pow(integralError/std::pow(integralN, 4.0), 0.5);

        asymmetry[0] = integralA/integralN;
        asymmetry[1] = integralError;


        return asymmetry;
}

void detectionAsymmetry(const std::string fileName, const std::string fileNameNew, const std::string fileNameCP, const std::string treeName, const std::string branchNameX, const std::string branchNameZ, const std::string decay, std::string pion, double asymmetry){

        ROOT::RDataFrame dataFrame(treeName, fileName);
        TRandom3* random = new TRandom3();
        random->SetSeed();

        int binNumber;
        double xMin, xMax, zMin, zMax;
        binNumber = 100;
        xMin = -0.4;
        xMax = -xMin;
        zMin = 0.0;
        zMax = 10.0;

        int countPos, countNeg;
        double errorPos, errorNeg;


        // // Generate soft pion charges with uniform distribution
        auto dataFramesPi = dataFrame.Define(
                "sPi_C", [random](){
                        return (random->Uniform() < 0.5) ? -1 : 1;
                }
        );


        // // Filter out events according to a CP asymmetry
        auto dataFrameCP = dataFramesPi.Filter(
                [random, asymmetry](const int& sPi_C){
                        double probability;
                        probability = 1.0 - (1.0 - asymmetry)/(1.0 + asymmetry);

                        if (sPi_C == -1 && random->Uniform() < probability){
                                return false;
                        }
                        return true;
                }, {"sPi_C"}
        );
        

        // // Filter out events according to a detection asymmetry
        // // with momentum dependence
        double slope = 0.1;
        auto dataFrameDetection = dataFrameCP.Filter([slope](int& sPi_C, double& sPi_PX, double& sPi_PZ){
                if (sPi_C == -1 && (std::abs(sPi_PX) > slope*sPi_PZ)){
                        return false;
                }
                return true;
                }, {"sPi_C", "sPi_PX", "sPi_PZ"}
        );


        // // Plot the initial pion charge distribution 
        TCanvas* canvasCharges = new TCanvas("canvasCharges", "");
        auto histCharges = dataFramesPi.Histo1D({"histCharges", "", 10, -2, 2}, "sPi_C");
        histCharges->SetStats(0);
        histCharges->GetXaxis()->SetTitle("soft #pi charge");
        histCharges->Draw("hist");
        canvasCharges->SaveAs((pion + "Initial.pdf").c_str());

        // // Plot the CP pion charge distribution
        auto histCP = dataFrameCP.Histo1D({"histCharges", "", 10, -2, 2}, "sPi_C");
        histCP->SetStats(0);
        histCP->GetXaxis()->SetTitle("soft #pi charge");
        histCP->Draw("hist");
        canvasCharges->SaveAs((pion + "CP.pdf").c_str());

        // // Plot the pion charge distribution after
        // // the detection asymmetry
        auto histDet = dataFrameDetection.Histo1D({"histDet", "", 10, -2, 2}, "sPi_C");
        histDet->SetStats(0);
        histDet->GetXaxis()->SetTitle("soft #pi charge");
        histDet->Draw("hist");
        canvasCharges->SaveAs((pion + "Detection.pdf").c_str());

        // // Plot before/after comparison of PXPZ
        TCanvas* canvasDual = new TCanvas("canvasDual", "", 1500, 600);
        canvasDual->Divide(2, 1);
        gStyle->SetPalette(55);
        auto histAfter = dataFrameDetection.Histo2D({"histAfter", "", binNumber, zMin, zMax, binNumber, xMin, xMax}, "sPi_PZ", "sPi_PX");
        auto histBefore = dataFramesPi.Histo2D({"histbefore", "", binNumber, zMin, zMax, binNumber, xMin, xMax}, "sPi_PZ", "sPi_PX");
        canvasDual->cd(1);
        histAfter->SetStats(0);
        histAfter->GetYaxis()->SetTitle("p_{x}(#pi) [GeV/c]");
        histAfter->GetXaxis()->SetTitle("After p_{z}(#pi) [GeV/c]");
        histAfter->DrawNormalized("colz");
        canvasDual->cd(2);
        histBefore->SetStats(0);
        histBefore->GetYaxis()->SetTitle("p_{x}(#pi) [GeV/c]");
        histBefore->GetXaxis()->SetTitle("Before p_{z}(#pi) [GeV/c]");
        histBefore->DrawNormalized("colz");
        canvasDual->SaveAs((pion + "BeforeAfter.pdf").c_str());

        // // Plot positive/negative comparison PXPZ
        auto histPositive = dataFrameDetection.Filter("sPi_C==1").Histo2D({"histAfter", "", binNumber, zMin, zMax, binNumber, xMin, xMax}, "sPi_PZ", "sPi_PX");
        auto histNegative = dataFrameDetection.Filter("sPi_C==-1").Histo2D({"histbefore", "", binNumber, zMin, zMax, binNumber, xMin, xMax}, "sPi_PZ", "sPi_PX");
        canvasDual->cd(1);
        histPositive->SetStats(0);
        histPositive->GetYaxis()->SetTitle("p_{x}(#pi^{+}) [GeV/c]");
        histPositive->GetXaxis()->SetTitle("p_{z}(#pi^{+}) [GeV/c]");
        histPositive->DrawNormalized("colz");
        canvasDual->cd(2);
        histNegative->SetStats(0);
        histNegative->GetYaxis()->SetTitle("p_{x}(#pi^{-}) [GeV/c]");
        histNegative->GetXaxis()->SetTitle("p_{z}(#pi^{-}) [GeV/c]");
        histNegative->DrawNormalized("colz");
        canvasDual->SaveAs((pion + "PositiveNegative.pdf").c_str());

        // CP Asymmetry calculation
        countPos = dataFrameCP.Filter("sPi_C==1").Count().GetValue();
        countNeg = dataFrameCP.Filter("sPi_C==-1").Count().GetValue();
        
        errorPos = std::pow(static_cast<double>(countPos), 0.5);
        errorNeg = std::pow(static_cast<double>(countNeg), 0.5);

        double calcAsymmetry, calcAsymmetryError, derivPos, derivNeg;

        calcAsymmetry = (static_cast<double>(countPos) - static_cast<double>(countNeg)) / (static_cast<double>(countPos) + static_cast<double>(countNeg));
        derivPos = 1.0/(static_cast<double>(countPos + countNeg)) - (static_cast<double>(countPos - countNeg))/std::pow((static_cast<double>(countPos + countNeg)), 2);
        derivPos =  - 1.0/(static_cast<double>(countPos + countNeg)) - (static_cast<double>(countPos - countNeg))/std::pow((static_cast<double>(countPos + countNeg)), 2);
        calcAsymmetryError = std::pow(std::pow(derivPos * errorPos, 2.0) + std::pow(derivNeg * errorNeg, 2.0), 0.5);
        std::cout << "Calculated CP Asymmetry: " << calcAsymmetry << " +/- " << calcAsymmetryError << std::endl;
        std::cout << "Expected CP Asymmetry: " << asymmetry << std::endl;
        std::cout << "|Calc - Exp|/σ: " << std::abs(calcAsymmetry - asymmetry)/calcAsymmetryError << " σ" << std::endl << std::endl;

        // Detection asymmetry calculation
        // std::vector<double> intDetAsymmetry = integratedAsymmetry(histAfter, slope, binNumber, xMin, xMax, zMin, zMax);
        std::vector<double> intDetAsymmetry = integratedAsymmetry(dataFrameDetection.Histo2D({"histAfter", "", 600, 0.0, 15.0, 600, -0.4, 0.4}, "sPi_PZ", "sPi_PX"), slope, 600, -0.4, 0.4, 0.0, 15.0);
        std::cout << "Calculated Detection Asymmetry: " << intDetAsymmetry[0] << " +/- " << intDetAsymmetry[1] << std::endl;

        double calcTotalAsymmetry, calcTotalAsymmetryError;
        countPos = dataFrameDetection.Filter("sPi_C==1").Count().GetValue();
        countNeg = dataFrameDetection.Filter("sPi_C==-1").Count().GetValue();
        
        errorPos = std::pow(static_cast<double>(countPos), 0.5);
        errorNeg = std::pow(static_cast<double>(countNeg), 0.5);


        calcTotalAsymmetry = (static_cast<double>(countPos) - static_cast<double>(countNeg)) / (static_cast<double>(countPos) + static_cast<double>(countNeg));
        derivPos = 1.0/(static_cast<double>(countPos + countNeg)) - (static_cast<double>(countPos - countNeg))/std::pow((static_cast<double>(countPos + countNeg)), 2);
        derivPos =  - 1.0/(static_cast<double>(countPos + countNeg)) - (static_cast<double>(countPos - countNeg))/std::pow((static_cast<double>(countPos + countNeg)), 2);
        calcTotalAsymmetryError = std::pow(std::pow(derivPos * errorPos, 2.0) + std::pow(derivNeg * errorNeg, 2.0), 0.5);

        double expDetAsymmetry, expDetAsymmetryError;
        expDetAsymmetry = (calcTotalAsymmetry - calcAsymmetry)/(1.0 - calcTotalAsymmetry*calcAsymmetry);
        derivPos = -1.0/(1.0 - calcTotalAsymmetry*calcAsymmetry) + calcTotalAsymmetry*(calcTotalAsymmetry - calcAsymmetry)/std::pow(1.0 - calcTotalAsymmetry*calcAsymmetry, 2.0);
        derivNeg = 1.0/(1.0 - calcTotalAsymmetry*calcAsymmetry) + calcAsymmetry*(calcTotalAsymmetry - calcAsymmetry)/std::pow(1.0 - calcTotalAsymmetry*calcAsymmetry, 2.0);
        expDetAsymmetryError = std::pow(std::pow(derivPos*calcAsymmetryError, 2.0) + std::pow(derivNeg*calcTotalAsymmetryError, 2.0), 0.5);

        // std::cout << "Expected Detection Asymmetry: " << expDetAsymmetry << " +/- " << expDetAsymmetryError << std::endl << std::endl;

        // Total asymmetry calculation

        std::cout << "Calculated Total Asymmetry: " << calcTotalAsymmetry << " +/- " << calcTotalAsymmetryError << std::endl;

        double expTotalAsymmetry, expTotalAsymmetryError;
        expTotalAsymmetry = (asymmetry + intDetAsymmetry[0])/(1.0 + asymmetry*intDetAsymmetry[0]);
        derivPos = 1.0 / (1.0 + intDetAsymmetry[0]*asymmetry) - asymmetry*(asymmetry + intDetAsymmetry[0])/std::pow(1.0 + intDetAsymmetry[0]*asymmetry, 2.0);
        derivPos = derivPos * intDetAsymmetry[1];
        expDetAsymmetryError = std::abs(derivPos);
        std::cout << "Expected Total Asymmetry: " << expTotalAsymmetry << " +/- " << expDetAsymmetryError<< std::endl << std::endl;
        
        
        dataFrameDetection.Snapshot(treeName, fileNameNew);

        std::cout << std::endl << std::endl;
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
        asymmetries.push_back(0.1);
        asymmetries.push_back(0.2);


        // // add pion charges
        // // add CP asymmetry
        // // add Detection asymmetry
        // // Calculate CP, integrated Detection and total asymmetries
        // // Compare with expected results
        for (size_t i = 0; i < fileNames.size(); i++){
                detectionAsymmetry(fileNames[i], filenamesNew[i], fileNamesCP[i], treeName, branchNameX, branchNameZ, decays[i], pions[i], asymmetries[i]);
        }



        //  Stop timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << std::endl << "Total Elapsed time: " << duration.count() << " sec" << std::endl;

        return 0;
}