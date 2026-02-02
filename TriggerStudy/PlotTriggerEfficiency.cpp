// ============================================================================
// PlotTriggerEfficiency.cpp
//
// Purpose:
//   Validate trigger SF by comparing Data vs MC efficiency
//   - Before SF: MC raw efficiency
//   - After SF: MC corrected efficiency (should match Data)
//
// Input:
//   - validated_SingleMuon.root (Data)
//   - validated_TTbarInc.root (MC)
//
// Output:
//   - PDF plots comparing efficiencies and ratios
//
// Run:
//   root -l -b -q "PlotTriggerEfficiency.cpp"
//
// ============================================================================

#include <TFile.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLine.h>
#include <TPad.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TROOT.h>

#include <iostream>
#include <vector>
#include <string>

#include "include/Config.hh"

// ============================================================================
// Helper: Compute efficiency histogram with proper errors
// ============================================================================
TH1D* ComputeEfficiency(TH1D* hPass, TH1D* hTotal, const char* name, bool isData = true)
{
    if (!hPass || !hTotal) return nullptr;
    
    TH1D* hEff = static_cast<TH1D*>(hPass->Clone(name));
    hEff->Reset();
    
////    int nBins = hPass->GetNbinsX();
////    for (int i = 1; i <= nBins; ++i) {
////        double pass = hPass->GetBinContent(i);
////        double total = hTotal->GetBinContent(i);
////        
////        if (total > 0) {
////            double eff = pass / total;
////            // Binomial error: sqrt(eff * (1-eff) / N)
////            double err = std::sqrt(eff * (1.0 - eff) / total);
////            hEff->SetBinContent(i, eff);
////            hEff->SetBinError(i, err);
////        } else {
////            hEff->SetBinContent(i, 0);
////            hEff->SetBinError(i, 0);
////        }
////    }
    int nBins = hPass->GetNbinsX();
    for (int i = 1; i <= nBins; ++i) {
        double pass  = hPass->GetBinContent(i);
        double total = hTotal->GetBinContent(i);
        
        if (total > 0) {
            double eff = pass / total;
            double err = 0.0;
            
            if (isData) {
                // Data: unweighted → 단순 이항 오차
                err = std::sqrt(eff * (1.0 - eff) / total);
            } else {
                // MC: weighted → N_eff 기반
                double totalErr = hTotal->GetBinError(i);
                if (totalErr > 0.0) {
                    double neff = (total * total) / (totalErr * totalErr);
                    err = std::sqrt(eff * (1.0 - eff) / neff);
                }
            }
            
            hEff->SetBinContent(i, eff);
            hEff->SetBinError(i, err);
        }
    }


    return hEff;
}

// ============================================================================
// Helper: Create ratio histogram
// ============================================================================
TH1D* ComputeRatio(TH1D* hNum, TH1D* hDen, const char* name)
{
    if (!hNum || !hDen) return nullptr;
    
    TH1D* hRatio = static_cast<TH1D*>(hNum->Clone(name));
    hRatio->Reset();
    
    int nBins = hNum->GetNbinsX();
    for (int i = 1; i <= nBins; ++i) {
        double num = hNum->GetBinContent(i);
        double den = hDen->GetBinContent(i);
        double numErr = hNum->GetBinError(i);
        double denErr = hDen->GetBinError(i);
        
        if (den > 0 && num > 0) {
            double ratio = num / den;
            // Error propagation
            double relErrNum = numErr / num;
            double relErrDen = denErr / den;
            double err = ratio * std::sqrt(relErrNum * relErrNum + relErrDen * relErrDen);
            hRatio->SetBinContent(i, ratio);
            hRatio->SetBinError(i, err);
        } else if (den > 0) {
            hRatio->SetBinContent(i, 0);
            hRatio->SetBinError(i, 0);
        }
    }
    
    return hRatio;
}

// ============================================================================
// Main plotting function for one variable
// ============================================================================
void PlotVariable(TFile* dataFile, TFile* mcFile,
                  const std::string& var,
                  const std::string& xTitle,
                  double yMin = 0.0, double yMax = 1.1)
{
    // ------------------------------------------------------------------------
    // Get histograms from files
    // ------------------------------------------------------------------------
    // Data (no SF concept for data, but uses same naming)
    TH1D* h_data_total = dynamic_cast<TH1D*>(dataFile->Get(("h_" + var + "_Total_noSF").c_str()));
    TH1D* h_data_pass  = dynamic_cast<TH1D*>(dataFile->Get(("h_" + var + "_Pass_noSF").c_str()));
    
    // MC without SF
    TH1D* h_mc_total_noSF = dynamic_cast<TH1D*>(mcFile->Get(("h_" + var + "_Total_noSF").c_str()));
    TH1D* h_mc_pass_noSF  = dynamic_cast<TH1D*>(mcFile->Get(("h_" + var + "_Pass_noSF").c_str()));
    
    // MC with SF
    TH1D* h_mc_total_SF = dynamic_cast<TH1D*>(mcFile->Get(("h_" + var + "_Total_SF").c_str()));
    TH1D* h_mc_pass_SF  = dynamic_cast<TH1D*>(mcFile->Get(("h_" + var + "_Pass_SF").c_str()));
    
    // Check if histograms exist
    if (!h_data_total || !h_data_pass) {
        std::cout << "[Skip] " << var << ": Data histograms not found.\n";
        return;
    }
    if (!h_mc_total_noSF || !h_mc_pass_noSF || !h_mc_total_SF || !h_mc_pass_SF) {
        std::cout << "[Skip] " << var << ": MC histograms not found.\n";
        return;
    }
    
    std::cout << "[Plot] " << var << "\n";
    
    // ------------------------------------------------------------------------
    // Compute efficiencies
    // ------------------------------------------------------------------------
    TH1D* eff_data    = ComputeEfficiency(h_data_pass, h_data_total, ("eff_data_" + var).c_str(), true);
    TH1D* eff_mc_noSF = ComputeEfficiency(h_mc_pass_noSF, h_mc_total_noSF, ("eff_mc_noSF_" + var).c_str(), false);
    TH1D* eff_mc_SF   = ComputeEfficiency(h_mc_pass_SF, h_mc_total_SF, ("eff_mc_SF_" + var).c_str(), false);
    
    if (!eff_data || !eff_mc_noSF || !eff_mc_SF) {
        std::cout << "[Error] Failed to compute efficiencies for " << var << "\n";
        return;
    }
    
    // ------------------------------------------------------------------------
    // Compute ratios (Data / MC)
    // ------------------------------------------------------------------------
    TH1D* ratio_noSF = ComputeRatio(eff_data, eff_mc_noSF, ("ratio_noSF_" + var).c_str());
    TH1D* ratio_SF   = ComputeRatio(eff_data, eff_mc_SF, ("ratio_SF_" + var).c_str());
    
    // ------------------------------------------------------------------------
    // Style settings
    // ------------------------------------------------------------------------
    // Data
    eff_data->SetMarkerStyle(20);
    eff_data->SetMarkerSize(1.0);
    eff_data->SetMarkerColor(kBlack);
    eff_data->SetLineColor(kBlack);
    eff_data->SetLineWidth(2);
    
    // MC without SF
    eff_mc_noSF->SetMarkerStyle(24);  // Open circle
    eff_mc_noSF->SetMarkerSize(1.0);
    eff_mc_noSF->SetMarkerColor(kBlue);
    eff_mc_noSF->SetLineColor(kBlue);
    eff_mc_noSF->SetLineWidth(2);
    eff_mc_noSF->SetLineStyle(2);  // Dashed
    
    // MC with SF
    eff_mc_SF->SetMarkerStyle(21);  // Filled square
    eff_mc_SF->SetMarkerSize(1.0);
    eff_mc_SF->SetMarkerColor(kRed);
    eff_mc_SF->SetLineColor(kRed);
    eff_mc_SF->SetLineWidth(2);
    
    // Ratio styles
    if (ratio_noSF) {
        ratio_noSF->SetMarkerStyle(24);
        ratio_noSF->SetMarkerColor(kBlue);
        ratio_noSF->SetLineColor(kBlue);
        ratio_noSF->SetLineStyle(2);
    }
    if (ratio_SF) {
        ratio_SF->SetMarkerStyle(21);
        ratio_SF->SetMarkerColor(kRed);
        ratio_SF->SetLineColor(kRed);
    }
    
    // ------------------------------------------------------------------------
    // Create canvas with ratio panel
    // ------------------------------------------------------------------------
    TCanvas* c = new TCanvas(("c_" + var).c_str(), var.c_str(), 800, 900);
    
    // Upper pad for efficiencies
    TPad* pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
    pad1->SetBottomMargin(0.02);
    pad1->SetLeftMargin(0.12);
    pad1->SetRightMargin(0.05);
    pad1->SetGridy();
    pad1->Draw();
    
    // Lower pad for ratio
    TPad* pad2 = new TPad("pad2", "pad2", 0, 0.0, 1, 0.3);
    pad2->SetTopMargin(0.02);
    pad2->SetBottomMargin(0.35);
    pad2->SetLeftMargin(0.12);
    pad2->SetRightMargin(0.05);
    pad2->SetGridy();
    pad2->Draw();
    
    // ------------------------------------------------------------------------
    // Draw efficiency panel
    // ------------------------------------------------------------------------
    pad1->cd();
    
    eff_data->SetTitle("");
    eff_data->GetYaxis()->SetTitle("Trigger Efficiency");
    eff_data->GetYaxis()->SetRangeUser(yMin, yMax);
    eff_data->GetYaxis()->SetTitleSize(0.05);
    eff_data->GetYaxis()->SetTitleOffset(1.0);
    eff_data->GetYaxis()->SetLabelSize(0.04);
    eff_data->GetXaxis()->SetLabelSize(0);  // Hide x-axis labels (shown in ratio)
    eff_data->SetStats(0);
    
    eff_data->Draw("PE");
    eff_mc_noSF->Draw("PE SAME");
    eff_mc_SF->Draw("PE SAME");
    
    // Legend
    TLegend* leg = new TLegend(0.55, 0.15, 0.92, 0.40);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    leg->AddEntry(eff_data, "Data", "lp");
    leg->AddEntry(eff_mc_noSF, "MC (no SF)", "lp");
    leg->AddEntry(eff_mc_SF, "MC (with SF)", "lp");
    leg->Draw();
    
    // Title
    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.045);
    latex.DrawLatex(0.15, 0.85, ("Trigger Efficiency vs " + var).c_str());
    
    // ------------------------------------------------------------------------
    // Draw ratio panel
    // ------------------------------------------------------------------------
    pad2->cd();
    
    if (ratio_SF) {
        ratio_SF->SetTitle("");
        ratio_SF->GetYaxis()->SetTitle("Data / MC");
        ratio_SF->GetYaxis()->SetRangeUser(0.85, 1.15);
        ratio_SF->GetYaxis()->SetNdivisions(505);
        ratio_SF->GetYaxis()->SetTitleSize(0.12);
        ratio_SF->GetYaxis()->SetTitleOffset(0.4);
        ratio_SF->GetYaxis()->SetLabelSize(0.10);
        ratio_SF->GetXaxis()->SetTitle(xTitle.c_str());
        ratio_SF->GetXaxis()->SetTitleSize(0.14);
        ratio_SF->GetXaxis()->SetTitleOffset(1.0);
        ratio_SF->GetXaxis()->SetLabelSize(0.10);
        ratio_SF->SetStats(0);
        
        ratio_SF->Draw("PE");
        if (ratio_noSF) ratio_noSF->Draw("PE SAME");
        
        // Draw line at 1
        double xmin = ratio_SF->GetXaxis()->GetXmin();
        double xmax = ratio_SF->GetXaxis()->GetXmax();
        TLine* line = new TLine(xmin, 1.0, xmax, 1.0);
        line->SetLineColor(kGray+2);
        line->SetLineStyle(2);
        line->SetLineWidth(2);
        line->Draw();
    }
    
    // ------------------------------------------------------------------------
    // Save
    // ------------------------------------------------------------------------
    c->SaveAs(("Validation_TrigEff_" + var + ".pdf").c_str());
    
    // Cleanup
    delete c;
    delete eff_data;
    delete eff_mc_noSF;
    delete eff_mc_SF;
    if (ratio_noSF) delete ratio_noSF;
    if (ratio_SF) delete ratio_SF;
}

// ============================================================================
// Main function
// ============================================================================
void PlotTriggerEfficiency()
{
    gStyle->SetOptStat(0);
    gROOT->SetBatch(kTRUE);
    
    // Print config
    TH1::SetDefaultSumw2(true);
    Config::Dump();
    
    // ------------------------------------------------------------------------
    // Open validated files (from Config)
    // ------------------------------------------------------------------------
    const std::string& dataFileName = Config::validatedData;
    const std::string& mcFileName   = Config::validatedMC;
    
    TFile* dataFile = TFile::Open(dataFileName.c_str(), "READ");
    TFile* mcFile   = TFile::Open(mcFileName.c_str(), "READ");
    
    if (!dataFile || dataFile->IsZombie()) {
        std::cerr << "[ERROR] Cannot open " << dataFileName << "\n";
        std::cerr << "  Did you run EventLooper Step 2 for Data?\n";
        return;
    }
    if (!mcFile || mcFile->IsZombie()) {
        std::cerr << "[ERROR] Cannot open " << mcFileName << "\n";
        std::cerr << "  Did you run EventLooper Step 2 for MC?\n";
        return;
    }
    
    std::cout << ">>> Input files:\n";
    std::cout << "    Data: " << dataFileName << "\n";
    std::cout << "    MC  : " << mcFileName << "\n\n";
    
    // ------------------------------------------------------------------------
    // Plot each variable
    // ------------------------------------------------------------------------
    PlotVariable(dataFile, mcFile, "HT", "HT [GeV]", 0.0, 1.1);
    PlotVariable(dataFile, mcFile, "pT", "6th jet p_{T} [GeV]", 0.0, 1.1);
    
    if (Config::useEta) {
        PlotVariable(dataFile, mcFile, "Eta", "6th jet #eta", 0.0, 1.1);
    }
    
    if (Config::useNBjets) {
        PlotVariable(dataFile, mcFile, "nbJets", "N_{b-jets} category", 0.0, 1.1);
    }
    
    // ------------------------------------------------------------------------
    // Cleanup
    // ------------------------------------------------------------------------
    dataFile->Close();
    mcFile->Close();
    
    std::cout << "\n>>> All validation plots created.\n";
}
