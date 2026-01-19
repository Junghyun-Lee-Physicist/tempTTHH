#include <TFile.h>
#include <TH1D.h>
#include <TGraphAsymmErrors.h>
#include <TEfficiency.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TLine.h>
#include <iostream>
#include <vector>

void PlotTriggerEfficiency()
{
    TH1::SetDefaultSumw2();

    // =========================================================
    // 1. Configuration
    // =========================================================
    TString dataFileName = "corrected_Data.root";   
    TString mcFileName   = "corrected_ttJets.root"; 

    std::vector<TString> variables = { "HT", "Jet6PT" };
    std::vector<TString> xTitles   = { "HT [GeV]", "6th Jet p_{T} [GeV]" };

    std::vector<TString> histNames_Total = { "h_HT_Total", "h_Jet6PT_Total" };
    std::vector<TString> histNames_Pass  = { "h_HT_Pass",  "h_Jet6PT_Pass" };

    // =========================================================
    // 2. Load Files
    // =========================================================
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
    TFile* outputFile = new TFile("TriggerEfficiency_Output.root", "RECREATE");

    // =========================================================
    // 3. Plotting Loop
    // =========================================================
    for (size_t i = 0; i < variables.size(); ++i)
    {
        // Get Histograms
        TH1D* h_Data_Total = (TH1D*)dataFile->Get(histNames_Total[i]);
        TH1D* h_Data_Pass  = (TH1D*)dataFile->Get(histNames_Pass[i]);
        TH1D* h_MC_Total   = (TH1D*)mcFile->Get(histNames_Total[i]);
        TH1D* h_MC_Pass    = (TH1D*)mcFile->Get(histNames_Pass[i]);

        if (!h_Data_Total || !h_Data_Pass || !h_MC_Total || !h_MC_Pass) {
            std::cerr << "[Error] Missing histograms for " << variables[i] << std::endl;
            continue;
        }

        // ---------------------------------------------------------
        // A. Data Efficiency (Use TEfficiency as strict check is fine for Data)
        // ---------------------------------------------------------
        TEfficiency* effData = new TEfficiency(*h_Data_Pass, *h_Data_Total);
        effData->SetStatisticOption(TEfficiency::kBUniform);
        
        TGraphAsymmErrors* grEffData = effData->CreateGraph();
        grEffData->SetMarkerStyle(20);
        grEffData->SetMarkerColor(kBlack);
        grEffData->SetLineColor(kBlack);

        // ---------------------------------------------------------
        // B. MC Efficiency (Use TH1::Divide to handle SF > 1 cases)
        // [수정됨] TEfficiency 대신 직접 나눗셈으로 계산
        // ---------------------------------------------------------
        TH1D* h_EffMC_Hist = (TH1D*)h_MC_Pass->Clone(Form("h_EffMC_%s", variables[i].Data()));
        h_EffMC_Hist->Divide(h_MC_Total); // Pass / Total

        TGraphAsymmErrors* grEffMC = new TGraphAsymmErrors(h_EffMC_Hist);
        grEffMC->SetMarkerStyle(21);
        grEffMC->SetMarkerColor(kRed);
        grEffMC->SetLineColor(kRed);

        // ---------------------------------------------------------
        // C. Ratio (Data / MC)
        // ---------------------------------------------------------
        TGraphAsymmErrors* grRatio = new TGraphAsymmErrors();
        int nPoints = grEffData->GetN();
        int nRatioPoints = 0;

        for (int j = 0; j < nPoints; ++j) {
            double xData, yData, xMC, yMC;
            
            // Data Point
            grEffData->GetPoint(j, xData, yData);
            
            // MC Point (from Histogram-based Graph)
            // TGraphAsymmErrors from TH1 stores x-centers correctly
            grEffMC->GetPoint(j, xMC, yMC); 

            if (yMC == 0 || yData == 0) continue;

            double ratio = yData / yMC;

            // Error Propagation
            double errYDataLow  = grEffData->GetErrorYlow(j);
            double errYDataHigh = grEffData->GetErrorYhigh(j);
            double errYMCLow    = grEffMC->GetErrorYlow(j);  // Statistical error from MC
            double errYMCHigh   = grEffMC->GetErrorYhigh(j);

            double errRatio_Low = ratio * sqrt(pow(errYDataLow / yData, 2) + pow(errYMCHigh / yMC, 2));
            double errRatio_High = ratio * sqrt(pow(errYDataHigh / yData, 2) + pow(errYMCLow / yMC, 2));

            double errXLow  = grEffData->GetErrorXlow(j);
            double errXHigh = grEffData->GetErrorXhigh(j);

            grRatio->SetPoint(nRatioPoints, xData, ratio);
            grRatio->SetPointError(nRatioPoints, errXLow, errXHigh, errRatio_Low, errRatio_High);
            nRatioPoints++;
        }

        grRatio->SetMarkerStyle(20);
        grRatio->SetMarkerColor(kBlue);
        grRatio->SetLineColor(kBlue);


        // ---------------------------------------------------------
        // D. Drawing
        // ---------------------------------------------------------
        TCanvas* c1 = new TCanvas(Form("c_eff_%s", variables[i].Data()), "", 800, 800);
        
        // Upper Pad
        TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
        pad1->SetBottomMargin(0.02);
        pad1->SetLeftMargin(0.15);
        pad1->Draw();
        pad1->cd();

        grEffData->SetTitle("");
        grEffData->GetYaxis()->SetTitle("Efficiency");
        grEffData->GetYaxis()->SetTitleSize(0.05);
        grEffData->GetYaxis()->SetTitleOffset(1.1);
        grEffData->GetYaxis()->SetRangeUser(0.0, 1.3); // Range를 조금 넉넉하게 (SF>1 고려)
        grEffData->GetXaxis()->SetLabelSize(0); 

        grEffData->Draw("AP");
        grEffMC->Draw("P SAME");

        TLegend* leg = new TLegend(0.65, 0.2, 0.9, 0.4);
        leg->SetBorderSize(0);
        leg->AddEntry(grEffData, "Data (with SF)", "p");
        leg->AddEntry(grEffMC, "MC (Corrected)", "p");
        leg->Draw();

        TPaveText* pt = new TPaveText(0.15, 0.92, 0.9, 0.98, "NDC");
        pt->SetFillColor(0);
        pt->SetBorderSize(0);
        pt->AddText(Form("Trigger Verification: %s", variables[i].Data()));
        pt->Draw();

        // Lower Pad
        c1->cd();
        TPad *pad2 = new TPad("pad2", "pad2", 0, 0.0, 1, 0.3);
        pad2->SetTopMargin(0.02);
        pad2->SetBottomMargin(0.3);
        pad2->SetLeftMargin(0.15);
        pad2->Draw();
        pad2->cd();

        grRatio->SetTitle("");
        grRatio->GetYaxis()->SetTitle("Data / MC");
        grRatio->GetYaxis()->SetNdivisions(505);
        grRatio->GetYaxis()->SetTitleSize(0.1);
        grRatio->GetYaxis()->SetTitleOffset(0.4);
        grRatio->GetYaxis()->SetLabelSize(0.08);
        grRatio->GetYaxis()->SetRangeUser(0.5, 1.5);

        grRatio->GetXaxis()->SetTitle(xTitles[i]);
        grRatio->GetXaxis()->SetTitleSize(0.12);
        grRatio->GetXaxis()->SetTitleOffset(1.0);
        grRatio->GetXaxis()->SetLabelSize(0.1);

        grRatio->Draw("AP");

        TLine* line = new TLine(grRatio->GetXaxis()->GetXmin(), 1.0, grRatio->GetXaxis()->GetXmax(), 1.0);
        line->SetLineStyle(2);
        line->SetLineColor(kGray+1);
        line->Draw("SAME");

        c1->SaveAs(Form("TriggerEfficiencyAndRatio_%s.pdf", variables[i].Data()));

        outputFile->cd();
        grEffData->Write(Form("Efficiency_Data_%s", variables[i].Data()));
        grEffMC->Write(Form("Efficiency_MC_%s", variables[i].Data()));
        grRatio->Write(Form("RatioPlot_%s", variables[i].Data()));

        delete c1;
        delete effData;
        // delete effMC; // TEfficiency 객체 삭제 대신 히스토그램 정리
        delete h_EffMC_Hist;
    }

    dataFile->Close();
    mcFile->Close();
    outputFile->Close();
}
