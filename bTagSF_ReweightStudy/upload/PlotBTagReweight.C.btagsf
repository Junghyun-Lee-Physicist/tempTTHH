// ============================================================================
// PlotBTagReweight.C
//
// Per-sample validation plotter for b-tag shape SF reweighting.
//
// Draws overlay of three weight tiers:
//   - No b-tag SF       (black)
//   - b-tag SF applied  (blue)
//   - b-tag SF + reweight (red)
//
// Lower panel ratio: withSF/noSF and reweighted/noSF
//   (MC-internal comparison, NOT Data/MC)
//
// Outputs:
//   plots/<sample>_<var>.pdf        — ROOT-drawn plot
//   plotdata/<sample>.json          — histogram data for Python/matplotlib
//
// Usage:
//   root -l -b -q 'PlotBTagReweight.C("TTToHadronic")'
//   root -l -b -q 'PlotBTagReweight.C("all")'       // all samples
//
// Author: Junghyun Lee
// ============================================================================

#include <TFile.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TString.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TLatex.h>
#include <TPaveText.h>
#include <TLine.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// ============================================================================
// Sample lists
// ============================================================================

const std::vector<std::string> allMCSamples = {
    "TTToHadronic", "TTTo2L2Nu", "TTToSemiLeptonic",
    "ttHH", "tt4b", "ttHtobb", "tttt", "tttW",
    "ttWH", "ttWW", "ttWZ", "ttZHto4b", "ttZtobb", "ttZZto4b", "ttbb",
    "QCD_HT200to300", "QCD_HT300to500", "QCD_HT500to700",
    "QCD_HT700to1000", "QCD_HT1000to1500", "QCD_HT1500to2000", "QCD_HT2000toInf"
};

const std::vector<std::string> allDataSamples = {
    "BTagCSV_B", "BTagCSV_C", "BTagCSV_D", "BTagCSV_E", "BTagCSV_F",
    "JetHT_B",   "JetHT_C",   "JetHT_D",   "JetHT_E",   "JetHT_F"
};

const TString filePrefix = "bTagReweight_";

// ============================================================================
// Drawing helpers
// ============================================================================

void DrawCMSLabel() {
    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.060);
    latex.SetTextAlign(11);
    latex.DrawLatex(0.10, 0.925,
        "CMS#scale[0.5]{ }#scale[0.85]{#font[52]{Private work}}");
}

void DrawLumiLabel() {
    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.050);
    latex.SetTextAlign(31);
    latex.DrawLatex(0.92, 0.925, "#bf{41.5 fb^{-1} (13 TeV, 2017)}");
}

void DrawSampleLabel(const TString& sampleName) {
    TPaveText* pt = new TPaveText(0.15, 0.72, 0.48, 0.85, "NDC");
    pt->AddText("#it{ttHH} hadronic channel");
    pt->AddText(Form("#it{%s}", sampleName.Data()));
    pt->SetTextSize(0.045);
    pt->SetFillColor(0);
    pt->SetBorderSize(0);
    pt->Draw();
}

// ============================================================================
// JSON data dumper (manual, no nlohmann needed in ROOT macro)
// ============================================================================

void WriteHistJSON(std::ofstream& out, const TString& name,
                   TH1D* h, bool last = false) {
    out << "      \"" << name.Data() << "\": {\n";
    out << "        \"bins\": [";
    for (int i = 1; i <= h->GetNbinsX(); ++i) {
        out << h->GetBinContent(i);
        if (i < h->GetNbinsX()) out << ", ";
    }
    out << "],\n";
    out << "        \"errors\": [";
    for (int i = 1; i <= h->GetNbinsX(); ++i) {
        out << h->GetBinError(i);
        if (i < h->GetNbinsX()) out << ", ";
    }
    out << "],\n";
    out << "        \"edges\": [";
    for (int i = 1; i <= h->GetNbinsX() + 1; ++i) {
        out << h->GetBinLowEdge(i);
        if (i <= h->GetNbinsX()) out << ", ";
    }
    out << "]\n";
    out << "      }" << (last ? "" : ",") << "\n";
}

// ============================================================================
// Draw one variable for one sample
// ============================================================================

void DrawVariable(TH1D* hNoSF, TH1D* hSF, TH1D* hReW,
                  const TString& varName, const TString& xTitle,
                  double xLow, double xHigh,
                  const TString& sampleName, const TString& outDir) {
    if (!hNoSF || !hSF || !hReW) {
        std::cerr << "[WARN] Missing histogram for " << varName
                  << " in " << sampleName << "\n";
        return;
    }

    hNoSF->Sumw2(); hSF->Sumw2(); hReW->Sumw2();
    hNoSF->GetXaxis()->SetRangeUser(xLow, xHigh);
    hSF->GetXaxis()->SetRangeUser(xLow, xHigh);
    hReW->GetXaxis()->SetRangeUser(xLow, xHigh);

    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    TCanvas* c = new TCanvas("c", "", 800, 800);
    c->SetLeftMargin(0.125);
    c->Divide(1, 2);

    // ── Upper pad: overlay ──
    c->cd(1);
    gPad->SetPad(0.0, 0.35, 1.0, 1.0);
    gPad->SetLogy(1);
    gPad->SetBottomMargin(0.005);
    gPad->SetGridx();

    hNoSF->SetLineColor(kBlack);
    hNoSF->SetLineWidth(2);
    hNoSF->SetMarkerStyle(22);
    hNoSF->SetMarkerColor(kBlack);
    hNoSF->SetMarkerSize(0.9);

    hSF->SetLineColor(kBlue);
    hSF->SetLineWidth(2);
    hSF->SetMarkerStyle(20);
    hSF->SetMarkerColor(kBlue);
    hSF->SetMarkerSize(0.9);

    hReW->SetLineColor(kRed);
    hReW->SetLineWidth(2);
    hReW->SetMarkerStyle(23);
    hReW->SetMarkerColor(kRed);
    hReW->SetMarkerSize(0.9);

    double yMax = std::max({hNoSF->GetMaximum(), hSF->GetMaximum(),
                            hReW->GetMaximum()});
    hNoSF->SetMaximum(yMax * 20.0);
    hNoSF->SetMinimum(std::max(0.1, yMax * 1e-5));
    hNoSF->GetYaxis()->SetTitle("Events");
    hNoSF->GetYaxis()->SetTitleSize(0.060);
    hNoSF->GetYaxis()->SetTitleOffset(0.75);
    hNoSF->GetYaxis()->SetLabelSize(0.045);
    hNoSF->GetXaxis()->SetTitleSize(0);
    hNoSF->GetXaxis()->SetLabelSize(0);

    hNoSF->Draw("E");
    hSF->Draw("E SAME");
    hReW->Draw("E SAME");

    TLegend* leg = new TLegend(0.52, 0.60, 0.88, 0.85);
    leg->AddEntry(hNoSF, "#bf{no b-tag SF}",       "pe");
    leg->AddEntry(hSF,   "#bf{b-tag SF}",          "pe");
    leg->AddEntry(hReW,  "#bf{b-tag SF + reweight}","pe");
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextFont(42);
    leg->Draw();

    DrawCMSLabel();
    DrawLumiLabel();
    DrawSampleLabel(sampleName);

    // ── Lower pad: ratio to noSF ──
    c->cd(2);
    gPad->SetPad(0.0, 0.0, 1.0, 0.35);
    gPad->SetBottomMargin(0.30);
    gPad->SetGridx();
    gPad->SetGridy();

    TH1D* rSF  = (TH1D*)hSF->Clone("rSF");
    rSF->Divide(hNoSF);
    rSF->SetLineColor(kBlue);
    rSF->SetMarkerColor(kBlue);
    rSF->SetMarkerStyle(20);
    rSF->SetMarkerSize(0.8);

    TH1D* rReW = (TH1D*)hReW->Clone("rReW");
    rReW->Divide(hNoSF);
    rReW->SetLineColor(kRed);
    rReW->SetMarkerColor(kRed);
    rReW->SetMarkerStyle(23);
    rReW->SetMarkerSize(0.8);

    rSF->SetMinimum(0.7);
    rSF->SetMaximum(1.6);
    rSF->GetXaxis()->SetTitle(xTitle);
    rSF->GetXaxis()->SetTitleSize(0.12);
    rSF->GetXaxis()->SetTitleOffset(1.0);
    rSF->GetXaxis()->SetLabelSize(0.09);
    rSF->GetXaxis()->SetLabelOffset(0.02);
    rSF->GetYaxis()->SetTitle("Ratio to no SF");
    rSF->GetYaxis()->SetTitleSize(0.09);
    rSF->GetYaxis()->SetTitleOffset(0.50);
    rSF->GetYaxis()->SetLabelSize(0.08);
    rSF->GetYaxis()->SetNdivisions(505);

    rSF->Draw("E");
    rReW->Draw("E SAME");

    TLine* line = new TLine(xLow, 1.0, xHigh, 1.0);
    line->SetLineColor(kBlack);
    line->SetLineStyle(2);
    line->Draw();

    TString pdfName = outDir + "/" + sampleName + "_" + varName + ".pdf";
    c->SaveAs(pdfName);

    delete c;
}

// ============================================================================
// Process one sample: draw all variables + JSON dump
// ============================================================================

void PlotOneSample(const TString& sampleName) {
    TString fname = filePrefix + sampleName + ".root";
    TFile* f = TFile::Open(fname, "READ");
    if (!f || f->IsZombie()) {
        std::cerr << "[WARN] Cannot open " << fname << "\n";
        return;
    }
    std::cout << ">>> Plotting: " << sampleName << "\n";

    TString outDir = "plots";
    TString jsonDir = "plotdata";
    if (gSystem->AccessPathName(outDir.Data())) gSystem->mkdir(outDir, kTRUE);
    if (gSystem->AccessPathName(jsonDir.Data())) gSystem->mkdir(jsonDir, kTRUE);

    // ── Variable definitions ──
    struct VarDef {
        TString histNoSF, histSF, histReW;
        TString plotName, xTitle;
        double xLow, xHigh;
    };

    std::vector<VarDef> vars = {
        {"h_nJets_noSF",  "h_nJets_withSF",  "h_nJets_reweighted",
         "nJets",  "nJets",       6, 18},
        {"h_HT_noSF",     "h_HT_withSF",     "h_HT_reweighted",
         "HT",     "HT [GeV]",   500, 2500},
        {"h_nbJets_noSF", "h_nbJets_withSF", "h_nbJets_reweighted",
         "nbJets", "n_{b-jets}",  0, 9},
        {"PerJet/h_jetPt_noSF_jet0", "PerJet/h_jetPt_withSF_jet0",
         "PerJet/h_jetPt_reweighted_jet0",
         "jet0_pT", "Leading jet p_{T} [GeV]", 30, 500},
        {"PerJet/h_bTag_noSF_jet0", "PerJet/h_bTag_withSF_jet0",
         "PerJet/h_bTag_reweighted_jet0",
         "jet0_bTag", "Leading jet DeepJet", 0.0, 1.0},
        {"PerJet/h_jetPt_noSF_jet1", "PerJet/h_jetPt_withSF_jet1",
         "PerJet/h_jetPt_reweighted_jet1",
         "jet1_pT", "Sub-leading jet p_{T} [GeV]", 30, 500},
        {"PerJet/h_bTag_noSF_jet1", "PerJet/h_bTag_withSF_jet1",
         "PerJet/h_bTag_reweighted_jet1",
         "jet1_bTag", "Sub-leading jet DeepJet", 0.0, 1.0},
        {"PerJet/h_jetPt_noSF_jet5", "PerJet/h_jetPt_withSF_jet5",
         "PerJet/h_jetPt_reweighted_jet5",
         "jet5_pT", "6th jet p_{T} [GeV]", 30, 250},
        {"PerJet/h_bTag_noSF_jet5", "PerJet/h_bTag_withSF_jet5",
         "PerJet/h_bTag_reweighted_jet5",
         "jet5_bTag", "6th jet DeepJet", 0.0, 1.0},
    };

    // ── Open JSON data file ──
    std::ofstream jsonOut(Form("%s/%s.json", jsonDir.Data(), sampleName.Data()));
    jsonOut << "{\n";
    jsonOut << "  \"sample\": \"" << sampleName.Data() << "\",\n";
    jsonOut << "  \"variables\": {\n";

    for (size_t iv = 0; iv < vars.size(); ++iv) {
        const auto& v = vars[iv];
        bool isLast = (iv == vars.size() - 1);

        TH1D* hNoSF = dynamic_cast<TH1D*>(f->Get(v.histNoSF));
        TH1D* hSF   = dynamic_cast<TH1D*>(f->Get(v.histSF));
        TH1D* hReW  = dynamic_cast<TH1D*>(f->Get(v.histReW));

        // Draw PDF
        DrawVariable(hNoSF, hSF, hReW,
                     v.plotName, v.xTitle, v.xLow, v.xHigh,
                     sampleName, outDir);

        // Dump to JSON
        if (hNoSF && hSF && hReW) {
            jsonOut << "    \"" << v.plotName.Data() << "\": {\n";
            WriteHistJSON(jsonOut, "noSF",       hNoSF, false);
            WriteHistJSON(jsonOut, "withSF",     hSF,   false);
            WriteHistJSON(jsonOut, "reweighted", hReW,  true);
            jsonOut << "    }" << (isLast ? "" : ",") << "\n";
        }
    }

    jsonOut << "  }\n";
    jsonOut << "}\n";
    jsonOut.close();

    f->Close();
    std::cout << "  → PDFs in " << outDir << "/,  JSON in " << jsonDir << "/\n";
}

// ============================================================================
// Main entry: single sample or "all"
// ============================================================================

void PlotBTagReweight(TString sampleName = "TTToHadronic") {
    if (sampleName == "all") {
        for (const auto& s : allMCSamples)  PlotOneSample(TString(s.c_str()));
        for (const auto& s : allDataSamples) PlotOneSample(TString(s.c_str()));
    } else {
        PlotOneSample(sampleName);
    }
    std::cout << "\n>>> Done.\n";
}
