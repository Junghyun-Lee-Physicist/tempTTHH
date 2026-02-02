#include <TFile.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TLatex.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "include/BinConfig.hh"

// ============================================================================
// [Helper] Drawing Function (기존과 동일)
// ============================================================================
void MakePrettyPlot(TH2D* h_orig, const std::vector<double>& x_vec, const std::vector<double>& y_vec, 
                    TString outName, TString title, TString zTitle, TString zFormat) 
{
    if (!h_orig) { std::cerr << "[ERROR] h_orig is NULL for " << outName << std::endl; return; }

    int nBinsX = x_vec.size() - 1;
    int nBinsY = y_vec.size() - 1;

    TH2D* h_uniform = new TH2D(Form("%s_uniform_%s", h_orig->GetName(), outName.Data()), 
                               title, nBinsX, 0, nBinsX, nBinsY, 0, nBinsY);

    for (int i = 1; i <= nBinsX; ++i) {
        for (int j = 1; j <= nBinsY; ++j) {
            double content = h_orig->GetBinContent(i, j);
            if (std::isnan(content) || std::isinf(content)) content = 0.0;
            h_uniform->SetBinContent(i, j, content);
        }
    }

    for (int i = 1; i <= nBinsX; ++i) h_uniform->GetXaxis()->SetBinLabel(i, Form("%.0f-%.0f", x_vec[i-1], x_vec[i]));
    for (int j = 1; j <= nBinsY; ++j) h_uniform->GetYaxis()->SetBinLabel(j, Form("%.0f-%.0f", y_vec[j-1], y_vec[j]));

    TCanvas* c = new TCanvas(Form("c_%s", outName.Data()), title, 1200, 900);
    gStyle->SetOptStat(0);
    gStyle->SetPalette(kRainBow);

    c->SetRightMargin(0.15);
    c->SetLeftMargin(0.12);
    c->SetBottomMargin(0.12);

    h_uniform->GetXaxis()->SetTitle("HT [GeV]");
    h_uniform->GetYaxis()->SetTitle("6th Jet p_{T} [GeV]");
    h_uniform->GetXaxis()->LabelsOption("h");
    
    if (zTitle != "") h_uniform->GetZaxis()->SetTitle(zTitle);
    if (title.Contains("Scale Factor")) h_uniform->GetZaxis()->SetRangeUser(0.5, 1.5);

    h_uniform->Draw("COLZ");

    TLatex latex;
    latex.SetTextSize(0.035);
    latex.SetTextAlign(22);
    latex.SetTextColor(kBlack);

    for (int i = 1; i <= nBinsX; ++i) {
        for (int j = 1; j <= nBinsY; ++j) {
            double val = h_uniform->GetBinContent(i, j);
            double x = h_uniform->GetXaxis()->GetBinCenter(i);
            double y = h_uniform->GetYaxis()->GetBinCenter(j);
            
            TString strVal;
            if (zFormat.Contains("0f")) strVal = Form("%.0f", val);
            else strVal = Form("%.3f", val);

            latex.DrawLatex(x, y, strVal);
        }
    }

    c->SaveAs(outName + ".pdf");
    delete c;
    delete h_uniform;
}

// ============================================================================
// Main Function
// ============================================================================
void DeriveSF_Hist()
{
    BinConfig::Load("config.yaml");

    TFile* dataFile = TFile::Open("Data.root");
    TFile* mcFile   = TFile::Open("ttJets.root");
    
    if (!dataFile || !mcFile) {
        std::cerr << "[CRITICAL] Input root files missing!" << std::endl;
        return;
    }

    TFile* outFile  = new TFile("ScaleFactors.root", "RECREATE");
    outFile->mkdir("ScaleFactors");

    std::vector<std::string> nbLabels = BinConfig::NB_Labels();
    std::vector<std::string> etaLabels = BinConfig::Eta_Labels();
    const std::vector<double>& HT_vec = BinConfig::HT_Bins();
    const std::vector<double>& PT_vec = BinConfig::PT_Bins();

    for (const auto& nb : nbLabels) {
        for (const auto& eta : etaLabels) {
            std::string key = nb + "_" + eta;
            std::cout << "[Processing] " << key << std::endl;

            TH2D* h_data_total = (TH2D*)dataFile->Get(("Total_" + key).c_str());
            TH2D* h_data_pass  = (TH2D*)dataFile->Get(("Pass_" + key).c_str());
            TH2D* h_mc_total   = (TH2D*)mcFile->Get(("Total_" + key).c_str());
            TH2D* h_mc_pass    = (TH2D*)mcFile->Get(("Pass_" + key).c_str());

            if (!h_data_total || !h_mc_total || !h_data_pass || !h_mc_pass) {
                std::cout << "  > [Warning] Missing histograms for " << key << std::endl;
                continue;
            }

            outFile->cd("ScaleFactors");
            h_data_total->Write(("Data_Total_" + key).c_str());
            h_data_pass->Write(("Data_Pass_" + key).c_str());
            h_mc_total->Write(("MC_Total_" + key).c_str());
            h_mc_pass->Write(("MC_Pass_" + key).c_str());

            // 1. Efficiency Calculation
            TH2D* effData = (TH2D*)h_data_pass->Clone(("effData_"+key).c_str());
            effData->Divide(h_data_total);

            TH2D* effMC = (TH2D*)h_mc_pass->Clone(("effMC_"+key).c_str());
            effMC->Divide(h_mc_total);

            // 2. SF Calculation
            TH2D* sf = (TH2D*)effData->Clone(("SF_"+key).c_str());
            sf->Divide(effMC);

            // =========================================================
            // [NEW] Low Stat Cut (< 50 Events) -> Set SF to 0.0
            // =========================================================
            int nx = sf->GetNbinsX();
            int ny = sf->GetNbinsY();

            for (int i = 1; i <= nx; ++i) {
                for (int j = 1; j <= ny; ++j) {
                    double nDataPass = h_data_pass->GetBinContent(i, j);
                    double nMCPass   = h_mc_pass->GetBinContent(i, j);

                    // Data나 MC 둘 중 하나라도 Pass 개수가 50 미만이면 0.0 처리
                    if (nDataPass < 50 || nMCPass < 50) {
                        sf->SetBinContent(i, j, 0.0);
                        sf->SetBinError(i, j, 0.0);
                    }
                }
            }
            // =========================================================

            sf->Write();

            // 3. Plotting
            MakePrettyPlot(h_data_pass, HT_vec, PT_vec, 
                           "Yield_Data_Pass_" + key, "Data Pass Yield (" + key + ")", "", "%.0f");
            MakePrettyPlot(h_mc_pass, HT_vec, PT_vec, 
                           "Yield_MC_Pass_" + key, "MC Pass Yield (" + key + ")", "", "%.0f");
            MakePrettyPlot(effData, HT_vec, PT_vec, 
                           "Eff_Data_" + key, "Data Efficiency (" + key + ")", "Efficiency", "%.3f");
            MakePrettyPlot(effMC, HT_vec, PT_vec, 
                           "Eff_MC_" + key, "MC Efficiency (" + key + ")", "Efficiency", "%.3f");
            MakePrettyPlot(sf, HT_vec, PT_vec, 
                           "SF_" + key, "Scale Factor (" + key + ")", "Scale Factor", "%.3f");

            delete effData;
            delete effMC;
            delete sf;
        }
    }
    outFile->Close();
    std::cout << ">>> All plots created successfully (With Stat < 50 Cut)." << std::endl;
}
