void FirstMacroHistogram(){
        TH1F *hist = new TH1F("hist", "", 100, -3, 3);
        TCanvas *canvas = new TCanvas("canvas");
        hist->FillRandom("gaus", 100000);
        hist->Draw();
}