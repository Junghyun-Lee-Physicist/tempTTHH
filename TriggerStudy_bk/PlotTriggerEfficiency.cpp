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

void PlotTriggerEfficiency()
{
    TH1::SetDefaultSumw2();

    // Data and MC files
    TString dataFileName = "corrected_Data.root"; // 스케일 팩터가 적용된 데이터 파일
    TString mcFileName = "corrected_ttJets.root"; // 스케일 팩터가 적용된 MC 파일

    std::string expantion = "pdf";

    // Variable names and axis titles
    std::vector<TString> variables = { "HT", "Jet6PT" };
    std::vector<TString> xTitles = { "HT [GeV]", "6th Jet p_{T} [GeV]" };

    // Histogram names
    std::vector<TString> histNames_Total = {
        "h_HT_Total",
        "h_Jet6PT_Total",
    };
    std::vector<TString> histNames_Pass = {
        "h_HT_Pass",
        "h_Jet6PT_Pass",
    };

    // Open data file
    TFile* dataFile = TFile::Open(dataFileName);
    if (!dataFile || dataFile->IsZombie())
    {
        std::cerr << "Cannot open data file: " << dataFileName << std::endl;
        return;
    }

    // Open MC file
    TFile* mcFile = TFile::Open(mcFileName);
    if (!mcFile || mcFile->IsZombie())
    {
        std::cerr << "Cannot open MC file: " << mcFileName << std::endl;
        return;
    }

    // Retrieve histograms
    TH1D* data_Total[2];
    TH1D* data_Pass[2];
    TH1D* mc_Total[2];
    TH1D* mc_Pass[2];

    for (int i = 0; i < 2; ++i)
    {
        data_Total[i] = (TH1D*)dataFile->Get(histNames_Total[i]);
        data_Pass[i] = (TH1D*)dataFile->Get(histNames_Pass[i]);
        mc_Total[i] = (TH1D*)mcFile->Get(histNames_Total[i]);
        mc_Pass[i] = (TH1D*)mcFile->Get(histNames_Pass[i]);

        if (!data_Total[i] || !data_Pass[i])
        {
            std::cerr << "Cannot find data histograms: " << histNames_Total[i] << " or " << histNames_Pass[i] << std::endl;
            return;
        }

        if (!mc_Total[i] || !mc_Pass[i])
        {
            std::cerr << "Cannot find MC histograms: " << histNames_Total[i] << " or " << histNames_Pass[i] << std::endl;
            return;
        }
    }

    // Create output ROOT file
    TFile* outputFile = new TFile("TriggerEfficiency_Output.root", "RECREATE");

    // Calculate efficiencies and draw graphs
    for (int i = 0; i < 2; ++i)
    {
        // Data efficiency calculation
        TEfficiency* effData = new TEfficiency(*data_Pass[i], *data_Total[i]);
        effData->SetStatisticOption(TEfficiency::kBUniform); // Wilson Score Interval
        TGraphAsymmErrors* grEffData = effData->CreateGraph();
        grEffData->SetMarkerStyle(20);
        grEffData->SetMarkerColor(kBlack);
        grEffData->SetLineColor(kBlack);

        // MC efficiency calculation
        TEfficiency* effMC = new TEfficiency(*mc_Pass[i], *mc_Total[i]);
        effMC->SetStatisticOption(TEfficiency::kBUniform); // Wilson Score Interval
        TGraphAsymmErrors* grEffMC = effMC->CreateGraph();
        grEffMC->SetMarkerStyle(21);
        grEffMC->SetMarkerColor(kRed);
        grEffMC->SetLineColor(kRed);

        // Calculate ratio plot
        TGraphAsymmErrors* grRatio = new TGraphAsymmErrors();
        int nPoints = grEffData->GetN();
        int nRatioPoints = 0;
        for (int j = 0; j < nPoints; ++j)
        {
            double xData, yData;
            grEffData->GetPoint(j, xData, yData);
            double xMC, yMC;
            grEffMC->GetPoint(j, xMC, yMC);

            if (yMC == 0 || yData == 0) continue;

            double ratio = yData / yMC;

            // Error calculation
            double errYDataLow = grEffData->GetErrorYlow(j);
            double errYDataHigh = grEffData->GetErrorYhigh(j);
            double errYMCLow = grEffMC->GetErrorYlow(j);
            double errYMCHigh = grEffMC->GetErrorYhigh(j);

            double errRatio_Low = ratio * sqrt(
                pow(errYDataLow / yData, 2) + pow(errYMCHigh / yMC, 2)
            );
            double errRatio_High = ratio * sqrt(
                pow(errYDataHigh / yData, 2) + pow(errYMCLow / yMC, 2)
            );

            // x-axis errors
            double errXLow = grEffData->GetErrorXlow(j);
            double errXHigh = grEffData->GetErrorXhigh(j);

            grRatio->SetPoint(nRatioPoints, xData, ratio);
            grRatio->SetPointError(nRatioPoints, errXLow, errXHigh, errRatio_Low, errRatio_High);
            nRatioPoints++;
        }

        grRatio->SetMarkerStyle(22);
        grRatio->SetMarkerColor(kBlue);
        grRatio->SetLineColor(kBlue);

        // Create canvas with upper and lower pads
        TCanvas* c1 = new TCanvas(Form("c_eff_%s", variables[i].Data()), Form("Trigger Efficiency and Ratio vs %s", variables[i].Data()), 800, 800);

        // Define pads
        float ysplit = 0.3; // Fraction of the canvas for the lower pad

        // Upper pad (Efficiency)
        TPad *pad1 = new TPad("pad1","pad1",0, ysplit,1,1);
        pad1->SetBottomMargin(0.02);
        pad1->SetTopMargin(0.1);
        pad1->SetLeftMargin(0.15);
        pad1->SetRightMargin(0.05);
        pad1->Draw();

        // Lower pad (Ratio plot)
        TPad *pad2 = new TPad("pad2","pad2",0,0,1, ysplit);
        pad2->SetTopMargin(0.02);
        pad2->SetBottomMargin(0.3);
        pad2->SetLeftMargin(0.15);
        pad2->SetRightMargin(0.05);
        pad2->Draw();

        // y-axis ranges
        //double yMin = 0.40;
        double yMin = 0.00;
        double yMax = 1.20;
        double sfYMin = 0.65;
        double sfYMax = 1.20;

        // Uniform axis label and title sizes
        double labelSize = 0.05; // Uniform label size
        double titleSize = 0.06; // Uniform title size

        // Plot efficiencies on upper pad
        pad1->cd();
        grEffData->SetTitle("");
        grEffData->GetXaxis()->SetLabelSize(0);
        grEffData->GetXaxis()->SetTitle("");
        grEffData->GetYaxis()->SetTitle("Efficiency");
        grEffData->GetYaxis()->SetTitleSize(titleSize);
        grEffData->GetYaxis()->SetTitleOffset(0.8);
        grEffData->GetYaxis()->SetLabelSize(labelSize);
        grEffData->GetYaxis()->SetRangeUser(yMin, yMax);
        grEffData->Draw("AP");
        grEffMC->Draw("P SAME");

        TLegend* leg = new TLegend(0.6, 0.2, 0.9, 0.4);
        leg->SetBorderSize(0); // Remove border
        leg->SetFillStyle(0);  // Transparent background
        leg->SetTextSize(0.04);
        leg->AddEntry(grEffData, "Data", "p");
        leg->AddEntry(grEffMC, "MC", "p");
        leg->Draw();

        // Add title in upper right corner
        TPaveText* pt = new TPaveText(0.6, 0.78, 0.9, 0.88, "NDC");
        pt->SetFillColor(0);
        pt->SetBorderSize(0);
        pt->SetTextAlign(32); // Align right and center vertically
        pt->SetTextSize(0.05);
        pt->AddText(Form("Trigger Efficiency vs %s", variables[i].Data()));
        pt->Draw();

        // Plot ratio plot on lower pad
        pad2->cd();
        grRatio->SetTitle("");
        grRatio->GetXaxis()->SetTitle(xTitles[i]);
        grRatio->GetXaxis()->SetTitleSize(titleSize);
        grRatio->GetXaxis()->SetTitleOffset(1.0);
        grRatio->GetXaxis()->SetLabelSize(labelSize);

        grRatio->GetYaxis()->SetTitle("Data / MC");
        grRatio->GetYaxis()->SetTitleSize(titleSize);
        grRatio->GetYaxis()->SetTitleOffset(0.8);
        grRatio->GetYaxis()->SetLabelSize(labelSize);
        grRatio->GetYaxis()->SetNdivisions(505);
        grRatio->GetYaxis()->SetRangeUser(sfYMin, sfYMax);
        grRatio->Draw("AP");

        // Draw horizontal line at y=1
        TLine* line = new TLine(grRatio->GetXaxis()->GetXmin(), 1.0, grRatio->GetXaxis()->GetXmax(), 1.0);
        line->SetLineStyle(2);
        line->SetLineColor(kGray+2);
        line->Draw("SAME");

        // Save the canvas
        c1->SaveAs(Form("TriggerEfficiencyAndRatio_%s.%s", variables[i].Data(), expantion.c_str()));

        // Save graphs to the ROOT file
        outputFile->cd();
        grEffData->Write(Form("Efficiency_Data_%s", variables[i].Data()));
        grEffMC->Write(Form("Efficiency_MC_%s", variables[i].Data()));
        grRatio->Write(Form("RatioPlot_%s", variables[i].Data()));

        // Clean up
        delete c1;
        delete grEffData;
        delete grEffMC;
        delete grRatio;
        delete effData;
        delete effMC;
        delete line;
    }

    // Close files
    dataFile->Close();
    mcFile->Close();
    outputFile->Close();
}

