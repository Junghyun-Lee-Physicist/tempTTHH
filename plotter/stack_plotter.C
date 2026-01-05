/**
 * CMS Stack Plotter (Fixed: Empty Histogram Crash)
 * * [ Usage ]
 * Run with compilation mode (+) for speed and proper debugging:
 * $ root -l -b -q stack_plotter.C+
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <iomanip> 

// ROOT Headers
#include "TFile.h"
#include "TH1F.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TLine.h"
#include "TColor.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TError.h"  
#include "TString.h" 

// ============================================================================
// 1. DATA STRUCTURES
// ============================================================================

struct SampleInfo {
    std::string name;       
    std::string type;       
    std::string label;      
    std::string colorHex;   
    std::vector<std::string> files; 
};

struct HistInfo {
    std::string key_path;   
    std::string clean_name; 
};

// ============================================================================
// 2. HELPER FUNCTIONS
// ============================================================================

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

void Log(std::string msg) {
    std::cout << "[StackPlotter] " << msg << std::endl;
}

std::vector<SampleInfo> ParseSampleConfig(std::string filename) {
    Log("Parsing SAMPLE config: " + filename);
    std::vector<SampleInfo> samples;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "   [Error] Cannot open config file: " << filename << std::endl;
        return samples;
    }

    std::string line;
    SampleInfo currentSample;
    bool inSamplesBlock = false;
    bool parsingFiles = false;

    while (std::getline(file, line)) {
        size_t commentPos = line.find('#');
        if (line.find("color:") == std::string::npos && commentPos != std::string::npos) {
             line = line.substr(0, commentPos);
        }
        
        std::string raw = trim(line);
        if (raw.empty()) continue;

        size_t indent = line.find_first_not_of(" ");
        if (indent == std::string::npos) indent = 0;

        if (raw == "samples:") {
            inSamplesBlock = true;
            continue;
        }

        if (inSamplesBlock) {
            if (indent == 2 && raw.back() == ':') {
                if (!currentSample.name.empty()) samples.push_back(currentSample);
                currentSample = SampleInfo();
                currentSample.name = raw.substr(0, raw.size() - 1);
                parsingFiles = false;
            }
            else if (indent > 2) {
                if (raw.rfind("type:", 0) == 0) { currentSample.type = trim(raw.substr(5)); parsingFiles = false; }
                else if (raw.rfind("label:", 0) == 0) { currentSample.label = trim(raw.substr(6)); parsingFiles = false; }
                else if (raw.rfind("color:", 0) == 0) {
                    std::string c = trim(raw.substr(6));
                    c.erase(std::remove(c.begin(), c.end(), '\''), c.end());
                    c.erase(std::remove(c.begin(), c.end(), '\"'), c.end());
                    currentSample.colorHex = c;
                    parsingFiles = false;
                }
                else if (raw.rfind("files:", 0) == 0) { parsingFiles = true; }
                else if (raw.rfind("- ", 0) == 0) {
                    if (parsingFiles) currentSample.files.push_back(trim(raw.substr(2)));
                }
            }
        }
    }
    if (!currentSample.name.empty()) samples.push_back(currentSample);
    
    std::cout << "   -> Successfully parsed " << samples.size() << " samples." << std::endl;
    return samples;
}

std::vector<HistInfo> ParseStructureConfig(std::string filename) {
    Log("Parsing STRUCTURE config: " + filename);
    std::vector<HistInfo> hists;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "   [Error] Cannot open config file: " << filename << std::endl;
        return hists;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string raw = trim(line);
        if (raw.rfind("key_path:", 0) == 0) {
            std::string path = trim(raw.substr(9));
            size_t bracketPos = path.find("()/");
            if (bracketPos != std::string::npos) path.replace(bracketPos, 3, ""); 
            if (path.find("Tree") != std::string::npos || path.find("cutflow") != std::string::npos) continue;

            HistInfo hi;
            hi.key_path = path;
            std::string safe = path;
            std::replace(safe.begin(), safe.end(), '/', '_');
            hi.clean_name = safe;
            hists.push_back(hi);
        }
    }
    std::cout << "   -> Found " << hists.size() << " target histograms." << std::endl;
    return hists;
}

// ============================================================================
// 3. DRAWING LOGIC
// ============================================================================

void DrawCMSLabel(double lumi) {
    TLatex latex;
    latex.SetNDC();
    latex.SetTextFont(61); latex.SetTextSize(0.055);
    latex.DrawLatex(0.16, 0.93, "CMS");
    latex.SetTextFont(52); latex.SetTextSize(0.045);
    latex.DrawLatex(0.26, 0.93, "Preliminary");
    latex.SetTextFont(42); latex.SetTextSize(0.045); latex.SetTextAlign(31);
    latex.DrawLatex(0.96, 0.93, Form("%.1f fb^{-1} (13 TeV)", lumi));
}

TH1F* GetSummedHist(const SampleInfo& sample, const std::string& histPath) {
    TH1F* hTotal = nullptr;
    gErrorIgnoreLevel = kError; 

    for (const auto& fname : sample.files) {
        TFile* f = TFile::Open(fname.c_str(), "READ");
        if (!f || f->IsZombie()) { if (f) delete f; continue; }

        TH1F* h = (TH1F*)f->Get(histPath.c_str());
        if (!h && histPath.front() == '/') h = (TH1F*)f->Get(histPath.substr(1).c_str());

        if (h) {
            if (!hTotal) {
                hTotal = (TH1F*)h->Clone();
                hTotal->SetDirectory(0); 
            } else {
                hTotal->Add(h);
            }
        }
        delete f; 
    }
    
    if (hTotal) {
        if (sample.type == "DATA") {
            hTotal->SetMarkerStyle(20); hTotal->SetMarkerSize(1.0); hTotal->SetLineColor(kBlack);
        } else {
            int color = kGray; 
            if (!sample.colorHex.empty()) { color = TColor::GetColor(sample.colorHex.c_str()); }
            hTotal->SetFillColor(color); hTotal->SetLineColor(kBlack); hTotal->SetLineWidth(1);
        }
    }
    return hTotal;
}

// ============================================================================
// 4. MAIN ENTRY POINT
// ============================================================================

void stack_plotter() {
    std::cout << std::unitbuf;
    std::cout << "\n===========================================" << std::endl;
    std::cout << "   CMS Stack Plotter Initialized" << std::endl;
    std::cout << "===========================================\n" << std::endl;

    double LUMI = 41.48;  // Lumi for 2017 UL
    std::string outDir = "plots";
    
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    gROOT->SetBatch(kTRUE); 

    if (gSystem->AccessPathName(outDir.c_str())) {
        gSystem->mkdir(outDir.c_str(), kTRUE);
    }

    auto samples = ParseSampleConfig("samples_config.yml");
    if (samples.empty()) return;
    auto hists = ParseStructureConfig("structure_info.yml");
    if (hists.empty()) return;

    Log("Starting Plotting Loop...");
    
    int total = hists.size();
    int count = 0;

    for (const auto& hInfo : hists) {
        count++;
        std::cout << "\r[Processing " << count << "/" << total << "] " 
                  << std::left << std::setw(60) << hInfo.key_path << std::flush;

        THStack* hs = new THStack("hs", "");
        TH1F* hData = nullptr;
        TH1F* hMcSum = nullptr; 
        
        TLegend* leg = new TLegend(0.60, 0.60, 0.93, 0.89);
        leg->SetBorderSize(0); leg->SetFillStyle(0); leg->SetTextSize(0.035); leg->SetNColumns(2); 

        bool hasData = false;
        bool hasMC = false;

        for (const auto& sample : samples) {
            TH1F* h = GetSummedHist(sample, hInfo.key_path);
            if (!h) continue; 

            if (sample.type == "DATA") {
                if (!hData) { hData = (TH1F*)h->Clone("hData"); hData->SetDirectory(0); }
                else hData->Add(h);
                hasData = true; delete h; 
            } 
            else { 
                hs->Add(h); 
                leg->AddEntry(h, sample.label.c_str(), "f");
                if (!hMcSum) { hMcSum = (TH1F*)h->Clone("hMcSum"); hMcSum->SetDirectory(0); }
                else hMcSum->Add(h);
                hasMC = true;
            }
        }
        
        if (hasData) leg->AddEntry(hData, "Data", "lp");

        if (!hasMC) {
            // Cleanup and skip
            delete hs; delete leg; if(hData) delete hData; if(hMcSum) delete hMcSum;
            std::cout << " -> [Skip] No MC." << std::endl;
            continue;
        }

        // [Fix] Check if Histogram is empty (Max <= 0) to avoid Log scale crash
        double yMaxMC = (hMcSum) ? hMcSum->GetMaximum() : 0.0;
        double yMaxData = (hasData && hData) ? hData->GetMaximum() : 0.0;
        double globalMax = std::max(yMaxMC, yMaxData);

        if (globalMax <= 0.0) {
            delete hs; delete leg; if(hData) delete hData; if(hMcSum) delete hMcSum;
            std::cout << " -> [Skip] Empty (Max=0)." << std::endl;
            continue;
        }

        // --- Draw ---
        TCanvas* c = new TCanvas("c", "", 800, 800);
        
        TPad* pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
        pad1->SetBottomMargin(0.02); pad1->SetTopMargin(0.08); 
        pad1->SetLeftMargin(0.15); pad1->SetRightMargin(0.05);
        pad1->Draw();
        
        TPad* pad2 = new TPad("pad2", "pad2", 0, 0.0, 1, 0.3);
        pad2->SetTopMargin(0.02); pad2->SetBottomMargin(0.35); 
        pad2->SetLeftMargin(0.15); pad2->SetRightMargin(0.05);
        pad2->Draw();

        pad1->cd(); 
        pad1->SetLogy(kTRUE); // Safe now because we checked Max > 0

        // [Fix] Set Minimum BEFORE Draw to allow Log scale to work properly
        hs->SetMinimum(0.1); 
        hs->SetMaximum(globalMax * 500.0); // More margin for Log

        hs->Draw("HIST");
        
        // [Fix] Check pointer validity before accessing axes
        if (hs->GetYaxis()) {
            hs->GetYaxis()->SetTitle("Events"); 
            hs->GetYaxis()->SetTitleSize(0.06); hs->GetYaxis()->SetTitleOffset(1.1); hs->GetYaxis()->SetLabelSize(0.05);
        }
        if (hs->GetXaxis()) {
            hs->GetXaxis()->SetLabelSize(0);
        }
        
        if (hasData) hData->Draw("SAME EP");
        leg->Draw(); DrawCMSLabel(LUMI);

        pad2->cd(); pad2->SetGridy();
        if (hasData && hMcSum) {
            TH1F* hRatio = (TH1F*)hData->Clone("hRatio");
            hRatio->Divide(hMcSum);
            
            hRatio->SetTitle(""); hRatio->SetMarkerStyle(20); hRatio->SetMarkerSize(0.8);
            
            hRatio->GetYaxis()->SetTitle("Data / Pred."); hRatio->GetYaxis()->SetNdivisions(505);
            hRatio->GetYaxis()->SetTitleSize(0.12); hRatio->GetYaxis()->SetTitleOffset(0.5); hRatio->GetYaxis()->SetLabelSize(0.1); 
            hRatio->GetYaxis()->SetRangeUser(0.0, 2.0);
            
            hRatio->GetXaxis()->SetTitle(hInfo.key_path.c_str()); 
            hRatio->GetXaxis()->SetTitleSize(0.14); hRatio->GetXaxis()->SetTitleOffset(1.0); hRatio->GetXaxis()->SetLabelSize(0.12);
            
            hRatio->Draw("EP");
            TLine* line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineStyle(2); line->SetLineColor(kRed); line->Draw();
        }

        c->SaveAs(Form("%s/%s.png", outDir.c_str(), hInfo.clean_name.c_str()));
        
        delete c; delete hMcSum; if(hData) delete hData;
    }

    std::cout << "\n\n[Success] All plots saved to '" << outDir << "' directory." << std::endl;
}
