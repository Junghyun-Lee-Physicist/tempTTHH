#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TGraphAsymmErrors.h>
#include <TEfficiency.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <iostream>
#include <vector>
#include <string>

// BinConfig 헤더 제거 (EventLooper와 동일하게 직접 정의)

void TriggerEfficiency()
{
    // =========================================================
    // 1. Bin Configuration (Must match EventLooper.cpp)
    // =========================================================
    
    // HT bins
    const int nBinsHT = 6;
    double HT_bins[nBinsHT + 1] = {500., 600., 700., 800., 1000., 1300., 2500.};

    // pT bins
    const int nBinspT = 6;
    double pT_bins[nBinspT + 1] = {40., 45., 50., 60., 70., 90., 150.};

    // b-jet bins (EventLooper에서 1개만 사용하므로 여기도 1개로 설정)
    const int nBjetBins = 1; 
    // double nBjets_bins[nBjetBins + 1] = {3, 12}; // Display용

    // Eta 분할 사용 안 함 (EventLooper 고정값)
    bool useEtaBinning = false; 

    // =========================================================
    // 2. File I/O
    // =========================================================

    TString dataFileName = "Data.root";
    TString mcFileName   = "ttJets.root";

    TFile* dataFile = TFile::Open(dataFileName);
    if (!dataFile || dataFile->IsZombie()) {
        std::cerr << "Cannot open data file: " << dataFileName << std::endl;
        return;
    }

    TFile* mcFile = TFile::Open(mcFileName);
    if (!mcFile || mcFile->IsZombie()) {
        std::cerr << "Cannot open MC file: " << mcFileName << std::endl;
        return;
    }

    // Output File
    TFile* outputFile = new TFile("ScaleFactors.root", "RECREATE");
    outputFile->mkdir("ScaleFactors");
    outputFile->cd("ScaleFactors");

    // =========================================================
    // 3. Calculation Loop
    // =========================================================

    // EventLooper와 동일하게 단순화된 루프 (1 Eta, 1 Bjet)
    for (int iEta = 0; iEta < 1; ++iEta)
    {
        for (int iBjet = 0; iBjet < 1; ++iBjet)
        {
            // Hist Name matching EventLooper::Loop() "Mode 1"
            TString histName_Total = Form("h_Total_Bjet%d", iBjet);
            TString histName_Pass  = Form("h_Pass_Bjet%d", iBjet);

            // Get Histograms
            TH2D* data_Total = (TH2D*)dataFile->Get(histName_Total);
            TH2D* data_Pass  = (TH2D*)dataFile->Get(histName_Pass);
            TH2D* mc_Total   = (TH2D*)mcFile->Get(histName_Total);
            TH2D* mc_Pass    = (TH2D*)mcFile->Get(histName_Pass);

            // Debug Info
            if (data_Total && data_Pass && mc_Total && mc_Pass) {
                std::cout << "\n[ Debug ] Processing Bjet Bin [" << iBjet << "]" << std::endl;
                std::cout << "  Data Total: " << data_Total->GetEntries() << ", Pass: " << data_Pass->GetEntries() << std::endl;
                std::cout << "  MC   Total: " << mc_Total->GetEntries()   << ", Pass: " << mc_Pass->GetEntries()   << std::endl;
            } else {
                std::cerr << "[Error] Missing histograms for Bjet " << iBjet << std::endl;
                continue;
            }

            // Calculate Efficiency
            // Name format: effData_Bjet0, SF_Bjet0
            TString effDataName = Form("effData_Bjet%d", iBjet);
            TString effMCName   = Form("effMC_Bjet%d", iBjet);
            TString sfName      = Form("SF_Bjet%d", iBjet);

            TH2D* effData = (TH2D*)data_Pass->Clone(effDataName);
            effData->Divide(data_Total);

            TH2D* effMC = (TH2D*)mc_Pass->Clone(effMCName);
            effMC->Divide(mc_Total);

            // Calculate Scale Factor (Data / MC)
            TH2D* sfHist = (TH2D*)effData->Clone(sfName);
            sfHist->Divide(effMC);

            // Write Raw SF to ROOT file (Used by Step 2)
            outputFile->cd("ScaleFactors");
            sfHist->Write();

            // =========================================================
            // 4. Visualization (Uniform Bin Width for Plotting)
            // =========================================================
            
            // Labels for X/Y axis
            std::vector<std::string> xBinLabels, yBinLabels;
            for (int i = 0; i < nBinsHT; ++i) xBinLabels.push_back(Form("%.0f-%.0f", HT_bins[i], HT_bins[i+1]));
            for (int i = 0; i < nBinspT; ++i) yBinLabels.push_back(Form("%.0f-%.0f", pT_bins[i], pT_bins[i+1]));

            TH2D* sfHistUniform = new TH2D(Form("SFUniform_Bjet%d", iBjet),
                                           sfHist->GetTitle(),
                                           nBinsHT, 0, nBinsHT,
                                           nBinspT, 0, nBinspT);

            // Copy Content
            for (int ix = 1; ix <= nBinsHT; ++ix) {
                for (int iy = 1; iy <= nBinspT; ++iy) {
                    sfHistUniform->SetBinContent(ix, iy, sfHist->GetBinContent(ix, iy));
                    sfHistUniform->SetBinError(ix, iy, sfHist->GetBinError(ix, iy));
                }
            }

            // Set Labels
            for (int ix = 1; ix <= nBinsHT; ++ix) sfHistUniform->GetXaxis()->SetBinLabel(ix, xBinLabels[ix-1].c_str());
            for (int iy = 1; iy <= nBinspT; ++iy) sfHistUniform->GetYaxis()->SetBinLabel(iy, yBinLabels[iy-1].c_str());

            // Draw
            TCanvas* c = new TCanvas("c", "Scale Factor", 1000, 800);
            sfHistUniform->SetStats(0);
            sfHistUniform->GetZaxis()->SetRangeUser(0.5, 1.2); // Range 조절
            sfHistUniform->GetXaxis()->SetTitle("HT [GeV]");
            sfHistUniform->GetYaxis()->SetTitle("6th Jet p_{T} [GeV]");
            sfHistUniform->GetXaxis()->LabelsOption("h");
            sfHistUniform->SetMarkerSize(1.5); // 텍스트 크기

            sfHistUniform->Draw("COLZ");
            sfHistUniform->Draw("TEXT SAME"); // 값 표시

            sfHistUniform->SetTitle(Form("Trigger SF (b-jets bin %d)", iBjet));

            // Save Plot
            c->SaveAs(Form("ScaleFactor_Bjet%d.pdf", iBjet));
            sfHistUniform->Write();

            delete c;
            delete effData;
            delete effMC;
            delete sfHist;
            delete sfHistUniform;
        }
    }

    dataFile->Close();
    mcFile->Close();
    outputFile->Close();
    std::cout << "ScaleFactors.root created successfully." << std::endl;
}
