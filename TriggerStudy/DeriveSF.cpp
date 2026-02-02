// ============================================================================
// DeriveSF.cpp
//
// Purpose:
//   1. Compute trigger efficiencies from Pass/Total histograms
//   2. Derive scale factors (SF = effData / effMC)
//   3. Fill empty bins using fit extrapolation / neighbor interpolation
//   4. Generate correctionlib-compatible JSON (trigger_sf.json.gz)
//   5. Generate validation plots (2D maps for counts, efficiency, SF, errors)
//
// Input:
//   - output_SingleMuon.root (Data)
//   - output_TTbarInc.root (MC)
//
// Output:
//   - TriggerSF.root (histograms for validation)
//   - trigger_sf.json.gz (correctionlib JSON for EventLooper Step 2)
//   - PDF plots: Total/Pass counts, Efficiency, SF, SF error
//
// Dependencies:
//   - ROOT
//   - nlohmann/json (header-only)
//   - zlib (for gzip compression)
//
// Run:
//   root -l -b -q "DeriveSF.cpp+"
//
// ============================================================================

#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TLatex.h>
#include <TEfficiency.h>
#include <TF2.h>
#include <TFitResultPtr.h>
#include <TFitResult.h>
#include <TMatrixDSym.h>
#include <TGraph2DErrors.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <array>
#include <memory>
#include <map>

// nlohmann/json (single header)
#include "include/nlohmann/json.hpp"

// zlib for gzip
#include <zlib.h>

// Configuration
#include "include/Config.hh"

using json = nlohmann::json;

// ============================================================================
// [Section 1] Utility Functions
// ============================================================================

namespace {

inline bool isFinite(double x) { return std::isfinite(x); }

inline double calcNeff(double sumw, double err) {
    if (!isFinite(sumw) || !isFinite(err) || err <= 0.0) return 0.0;
    return (sumw * sumw) / (err * err);
}

double median(std::vector<double> v) {
    if (v.empty()) return 0.0;
    std::sort(v.begin(), v.end());
    size_t n = v.size();
    return (n % 2) ? v[n/2] : 0.5 * (v[n/2 - 1] + v[n/2]);
}

// ============================================================================
// Efficiency Result Structure
// ============================================================================
struct EffResult {
    double eff = 0.0;
    double err = 0.0;
    bool ok = false;
    double neff_pass = 0.0;
};

// ============================================================================
// Compute Efficiency
//   - Data: Clopper-Pearson interval
//   - MC: Error propagation (requires Sumw2)
// ============================================================================
EffResult computeEfficiency(bool isData,
                            double passW, double passErr,
                            double totalW, double totalErr)
{
    EffResult r;
    if (!isFinite(passW) || !isFinite(totalW) || totalW <= 0.0 || passW < 0.0) return r;
    if (passW > totalW && isData) return r;

    r.eff = passW / totalW;
    if (!isFinite(r.eff) || r.eff < 0.0) return r;

    r.neff_pass = calcNeff(passW, passErr);

    if (isData) {
        int n = std::max(0, static_cast<int>(std::llround(totalW)));
        int k = std::max(0, static_cast<int>(std::llround(passW)));
        if (n <= 0 || k < 0 || k > n) return r;

        const double level = 0.682689492137; // 1 sigma level
        double lo = TEfficiency::ClopperPearson(n, k, level, false);
        double hi = TEfficiency::ClopperPearson(n, k, level, true);
        r.err = 0.5 * (hi - lo);
        r.ok = isFinite(r.err);
        return r;
    }

    // MC error propagation
////    if (!isFinite(passErr) || !isFinite(totalErr) || passW <= 0.0) return r;
////
////    double relPass = (passErr > 0.0) ? (passErr / passW) : 0.0;
////    double relTot = (totalErr > 0.0) ? (totalErr / totalW) : 0.0;
////    r.err = r.eff * std::sqrt(relPass * relPass + relTot * relTot);
////    r.ok = isFinite(r.err);
////    return r;
    // MC: N_eff 기반 이항 오차
    // N_eff = (Σw)² / Σ(w²), 여기서 totalErr = √(Σw²)
    if (!isFinite(totalErr) || totalErr <= 0.0) return r;
    
    double neff = (totalW * totalW) / (totalErr * totalErr);
    if (neff <= 0.0) return r;
    
    r.err = std::sqrt(r.eff * (1.0 - r.eff) / neff);
    r.ok = isFinite(r.err);
    return r;


}

// ============================================================================
// 2D Quadratic Fit Model (in log space)
// ============================================================================
struct FitModel2D {
    double ht0 = 0, htScale = 1, pt0 = 0, ptScale = 1;
    TF2* func = nullptr;
    TMatrixDSym cov;
    bool ok = false;
    double chi2ndf = -1;
    int npoints = 0;

    std::array<double, 6> basis(double ht, double pt) const {
        double xn = (htScale != 0.0) ? (ht - ht0) / htScale : 0.0;
        double yn = (ptScale != 0.0) ? (pt - pt0) / ptScale : 0.0;
        return {1.0, xn, yn, xn * yn, xn * xn, yn * yn};
    }

    double evalLog(double ht, double pt) const {
        if (!ok || !func) return 0.0;
        double xn = (htScale != 0.0) ? (ht - ht0) / htScale : 0.0;
        double yn = (ptScale != 0.0) ? (pt - pt0) / ptScale : 0.0;
        return func->Eval(xn, yn);
    }

    double evalLogUnc(double ht, double pt) const {
        if (!ok || cov.GetNrows() < 6) return 1.0;
        auto b = basis(ht, pt);
        double var = 0.0;
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                var += b[i] * cov(i, j) * b[j];
            }
        }
        return (var > 0.0) ? std::sqrt(var) : 1.0;
    }

    ~FitModel2D() { if (func) delete func; }
};

FitModel2D fitQuadratic2DLog(const std::string& name,
                              const std::vector<double>& htEdges,
                              const std::vector<double>& ptEdges,
                              TH2D* hSF)
{
    FitModel2D m;
    if (!hSF) return m;

    std::vector<double> vHT, vPT, vZ, vEZ;
    int nx = hSF->GetNbinsX();
    int ny = hSF->GetNbinsY();

    for (int i = 1; i <= nx; ++i) {
        for (int j = 1; j <= ny; ++j) {
            double sf = hSF->GetBinContent(i, j);
            double esf = hSF->GetBinError(i, j);
            if (sf > 0.0 && esf > 0.0 && isFinite(sf) && isFinite(esf)) {
                vHT.push_back(hSF->GetXaxis()->GetBinCenter(i));
                vPT.push_back(hSF->GetYaxis()->GetBinCenter(j));
                vZ.push_back(std::log(sf));
                vEZ.push_back(esf / sf);
            }
        }
    }

    m.npoints = static_cast<int>(vHT.size());
    if (m.npoints < 6) return m;

    m.ht0 = 0.5 * (htEdges.front() + htEdges.back());
    m.htScale = 0.5 * (htEdges.back() - htEdges.front());
    m.pt0 = 0.5 * (ptEdges.front() + ptEdges.back());
    m.ptScale = 0.5 * (ptEdges.back() - ptEdges.front());

    TGraph2DErrors g(m.npoints);
    for (int k = 0; k < m.npoints; ++k) {
        double xn = (vHT[k] - m.ht0) / m.htScale;
        double yn = (vPT[k] - m.pt0) / m.ptScale;
        g.SetPoint(k, xn, yn, vZ[k]);
        g.SetPointError(k, 0, 0, vEZ[k]);
    }

    m.func = new TF2(Form("fit_%s", name.c_str()),
                     "[0]+[1]*x+[2]*y+[3]*x*y+[4]*x*x+[5]*y*y", -2, 2, -2, 2);
    m.func->SetParameters(0, 0, 0, 0, 0, 0);

    TFitResultPtr rp = g.Fit(m.func, "SQ0");
    if (rp.Get() && rp->IsValid()) {
        m.ok = true;
        m.cov.ResizeTo(6, 6);
        m.cov = rp->GetCovarianceMatrix();
        m.chi2ndf = (rp->Ndf() > 0) ? rp->Chi2() / rp->Ndf() : -1;
    }

    return m;
}

// ============================================================================
// Neighbor Interpolation
// ============================================================================
bool fillByNeighbors(int i, int j, TH2D* h, double& val, double& unc)
{
    val = 0.0;
    unc = 0.0;
    if (!h) return false;

    double sumW = 0.0, sumV = 0.0, sumE2 = 0.0;
    int nx = h->GetNbinsX();
    int ny = h->GetNbinsY();

    for (int di = -1; di <= 1; ++di) {
        for (int dj = -1; dj <= 1; ++dj) {
            if (di == 0 && dj == 0) continue;
            int ni = i + di, nj = j + dj;
            if (ni < 1 || ni > nx || nj < 1 || nj > ny) continue;

            double v = h->GetBinContent(ni, nj);
            double e = h->GetBinError(ni, nj);
            if (v > 0.0 && e > 0.0 && isFinite(v) && isFinite(e)) {
                double w = 1.0 / (e * e);
                sumW += w;
                sumV += w * v;
                sumE2 += 1.0;
            }
        }
    }

    if (sumW > 0.0 && sumE2 > 0.0) {
        val = sumV / sumW;
        unc = std::sqrt(sumE2) / sumW;
        return true;
    }
    return false;
}

// ============================================================================
// Nearest Neighbor Extrapolation (Flat)
// ============================================================================
bool fillByNearestExtrapolation(int i, int j, TH2D* hFilled, double& val, double& unc)
{
    // Try to take value from the bin below (Same HT, lower pT)
    if (j > 1) {
        double v = hFilled->GetBinContent(i, j - 1);
        double e = hFilled->GetBinError(i, j - 1);
        if (v > 0.0 && isFinite(v)) {
            val = v;
            unc = e;
            return true;
        }
    }
    // Try to take value from the bin to the left (Lower HT, same pT)
    if (i > 1) {
        double v = hFilled->GetBinContent(i - 1, j);
        double e = hFilled->GetBinError(i - 1, j);
        if (v > 0.0 && isFinite(v)) {
            val = v;
            unc = e;
            return true;
        }
    }
    return false;
}

}  // anonymous namespace

// ============================================================================
// [Section 2] Pretty Plot Function
// ============================================================================

enum class PlotType {
    Counts,     // Integer event counts (%.0f)
    Efficiency, // Efficiency values (%.4f, range 0-1)
    SF,         // Scale factor (%.4f, range 0.5-1.5)
    Error       // Error values (%.4f, range 0-0.3)
};

void MakePrettyPlot(TH2D* hOrig,
                    const std::vector<double>& xEdges,
                    const std::vector<double>& yEdges,
                    const TString& outName,
                    const TString& title,
                    PlotType ptype)
{
    if (!hOrig) return;

    int nBinsX = static_cast<int>(xEdges.size()) - 1;
    int nBinsY = static_cast<int>(yEdges.size()) - 1;

    TH2D* hPlot = new TH2D(Form("%s_plot", outName.Data()), title,
                           nBinsX, 0, nBinsX, nBinsY, 0, nBinsY);

    for (int i = 1; i <= nBinsX; ++i) {
        for (int j = 1; j <= nBinsY; ++j) {
            double val = hOrig->GetBinContent(i, j);
            if (!isFinite(val)) val = 0.0;
            hPlot->SetBinContent(i, j, val);
        }
        hPlot->GetXaxis()->SetBinLabel(i, Form("%.0f-%.0f", xEdges[i-1], xEdges[i]));
    }
    for (int j = 1; j <= nBinsY; ++j) {
        hPlot->GetYaxis()->SetBinLabel(j, Form("%.0f-%.0f", yEdges[j-1], yEdges[j]));
    }

    hPlot->GetXaxis()->SetTitle("HT [GeV]");
    hPlot->GetYaxis()->SetTitle("6th jet p_{T} [GeV]");

    // Set z-axis range and format based on plot type
    TString zFormat;
    double zMin = 0, zMax = 0;
    TString zTitle;

    switch (ptype) {
        case PlotType::Counts:
            zFormat = ".0f";
            zTitle = "Events";
            break;
        case PlotType::Efficiency:
            zFormat = ".4f";
            zMin = 0.0;
            zMax = 1.0;
            zTitle = "Efficiency";
            break;
        case PlotType::SF:
            zFormat = ".4f";
            zMin = 0.5;
            zMax = 1.5;
            zTitle = "Scale Factor";
            break;
        case PlotType::Error:
            zFormat = ".4f";
            zMin = 0.0;
            zMax = 0.3;
            zTitle = "Error";
            break;
    }

    hPlot->GetZaxis()->SetTitle(zTitle);

    TCanvas* c = new TCanvas(Form("c_%s", outName.Data()), "", 1200, 900);
    c->SetRightMargin(0.15);
    c->SetLeftMargin(0.12);
    c->SetBottomMargin(0.12);

    gStyle->SetOptStat(0);
    gStyle->SetPaintTextFormat(zFormat.Data());

    if (ptype != PlotType::Counts) {
        hPlot->GetZaxis()->SetRangeUser(zMin, zMax);
    }

    hPlot->Draw("COLZ");

    // Draw text values
    TLatex latex;
    latex.SetTextSize(0.022);
    latex.SetTextAlign(22);

    for (int i = 1; i <= nBinsX; ++i) {
        for (int j = 1; j <= nBinsY; ++j) {
            double val = hPlot->GetBinContent(i, j);
            TString txt;
            if (ptype == PlotType::Counts) {
                txt = Form("%.0f", val);
            } else {
                txt = Form("%.4f", val);
            }
            latex.DrawLatex(hPlot->GetXaxis()->GetBinCenter(i),
                            hPlot->GetYaxis()->GetBinCenter(j), txt);
        }
    }

    c->SaveAs(outName + ".pdf");
    delete c;
    delete hPlot;
}

// ============================================================================
// [Section 3] JSON Generation (correctionlib schema v2)
// ============================================================================

namespace JSONBuilder {

json MakeMultiBinning(TH2D* hSF,
                      const std::vector<double>& htEdges,
                      const std::vector<double>& ptEdges)
{
    if (!hSF) {
        size_t nHT = htEdges.size() - 1;
        size_t nPT = ptEdges.size() - 1;
        std::vector<double> content(nHT * nPT, 1.0);
        return {
            {"nodetype", "multibinning"},
            {"inputs", {"ht", "pt"}},
            {"edges", {htEdges, ptEdges}},
            {"content", content},
            {"flow", "clamp"}
        };
    }

    int nx = hSF->GetNbinsX(); // HT bin
    int ny = hSF->GetNbinsY(); // pT bin

    std::vector<double> content;
    content.reserve(nx * ny);

    // correctionlib MultiBinning: last input axis varies fastest (row-major)
    // inputs = ["ht", "pt"] → pt varies fastest
    // Order: (HT0,PT0), (HT0,PT1), ..., (HT0,PTn), (HT1,PT0), ...
    for (int i = 1; i <= nx; ++i) {      // HT (outer loop = slow)
        for (int j = 1; j <= ny; ++j) {  // pT (inner loop = fast)
            double sf = hSF->GetBinContent(i, j);
            if (!isFinite(sf) || sf <= 0.0) sf = 1.0;
            content.push_back(sf);
        }
    }

    return {
        {"nodetype", "multibinning"},
        {"inputs", {"ht", "pt"}},
        {"edges", {htEdges, ptEdges}},
        {"content", content},
        {"flow", "clamp"}
    };
}

json MakeEtaNode(const std::map<std::string, TH2D*>& sfMap,
                 const std::string& nbLabel,
                 const std::vector<double>& htEdges,
                 const std::vector<double>& ptEdges)
{
    if (!Config::useEta) {
        std::string key = nbLabel + "_Eta_Inc";
        auto it = sfMap.find(key);
        TH2D* h = (it != sfMap.end()) ? it->second : nullptr;
        return MakeMultiBinning(h, htEdges, ptEdges);
    }

    const auto& etaEdges = Config::Eta_Bins();
    auto etaLabels = Config::Eta_Labels();

    // For flow="clamp", content size = edges size - 1
    // No need to add extra underflow/overflow bins
    std::vector<json> content;

    for (const auto& etaLabel : etaLabels) {
        std::string key = nbLabel + "_" + etaLabel;
        auto it = sfMap.find(key);
        TH2D* h = (it != sfMap.end()) ? it->second : nullptr;
        content.push_back(MakeMultiBinning(h, htEdges, ptEdges));
    }

    return {
        {"nodetype", "binning"},
        {"input", "eta"},
        {"edges", etaEdges},
        {"content", content},
        {"flow", "clamp"}
    };
}

json MakeNBNode(const std::map<std::string, TH2D*>& sfMap,
                const std::vector<double>& htEdges,
                const std::vector<double>& ptEdges)
{
    if (!Config::useNBjets) {
        return MakeEtaNode(sfMap, "nB_Inc", htEdges, ptEdges);
    }

    auto nbLabels = Config::NB_Labels();
    auto nbEdges = Config::MakeNBEdges();

    // For flow="clamp", content size = edges size - 1
    // No need to add extra underflow/overflow bins
    std::vector<json> content;

    for (const auto& nbLabel : nbLabels) {
        content.push_back(MakeEtaNode(sfMap, nbLabel, htEdges, ptEdges));
    }

    return {
        {"nodetype", "binning"},
        {"input", "nbJets"},
        {"edges", nbEdges},
        {"content", content},
        {"flow", "clamp"}
    };
}

json BuildCorrectionSet(const std::map<std::string, TH2D*>& sfMap,
                        const std::vector<double>& htEdges,
                        const std::vector<double>& ptEdges)
{
    json correction = {
        {"name", "triggerSF"},
        {"version", 1},
        {"description", "Hadronic trigger scale factors"},
        {"inputs", {
            {{"name", "nbJets"}, {"type", "int"}, {"description", "Number of b-jets"}},
            {{"name", "eta"}, {"type", "real"}, {"description", "6th jet eta"}},
            {{"name", "ht"}, {"type", "real"}, {"description", "HT [GeV]"}},
            {{"name", "pt"}, {"type", "real"}, {"description", "6th jet pT [GeV]"}}
        }},
        {"output", {{"name", "weight"}, {"type", "real"}, {"description", "Scale factor"}}},
        {"data", MakeNBNode(sfMap, htEdges, ptEdges)}
    };

    return {
        {"schema_version", 2},
        {"description", "Trigger SF for ttHH analysis"},
        {"corrections", {correction}}
    };
}

bool WriteGzipJSON(const json& j, const std::string& filename)
{
    std::string jsonStr = j.dump();

    gzFile gz = gzopen(filename.c_str(), "wb9");
    if (!gz) {
        std::cerr << "[ERROR] Cannot open " << filename << " for writing\n";
        return false;
    }

    int written = gzwrite(gz, jsonStr.c_str(), static_cast<unsigned>(jsonStr.size()));
    gzclose(gz);

    if (written != static_cast<int>(jsonStr.size())) {
        std::cerr << "[ERROR] Failed to write complete JSON to " << filename << "\n";
        return false;
    }

    return true;
}

}  // namespace JSONBuilder

// ============================================================================
// [Section 4] Main Function
// ============================================================================

void DeriveSF()
{
    TH1::SetDefaultSumw2(true);
    Config::Dump();

    // ------------------------------------------------------------------------
    // File names (from Config)
    // ------------------------------------------------------------------------
    const std::string& dataFileName = Config::sfInputData;
    const std::string& mcFileName   = Config::sfInputMC;
    const std::string& outFileName  = Config::sfOutputRoot;
    const std::string& jsonFileName = Config::sfOutputJSON;

    // ------------------------------------------------------------------------
    // Open input files
    // ------------------------------------------------------------------------
    TFile* dataFile = TFile::Open(dataFileName.c_str(), "READ");
    TFile* mcFile = TFile::Open(mcFileName.c_str(), "READ");

    if (!dataFile || dataFile->IsZombie()) {
        std::cerr << "[ERROR] Cannot open " << dataFileName << "\n";
        return;
    }
    if (!mcFile || mcFile->IsZombie()) {
        std::cerr << "[ERROR] Cannot open " << mcFileName << "\n";
        return;
    }

    std::cout << ">>> Input files opened.\n";
    std::cout << "    Data: " << dataFileName << "\n";
    std::cout << "    MC  : " << mcFileName << "\n";

    // ------------------------------------------------------------------------
    // Create output file
    // ------------------------------------------------------------------------
    TFile* outFile = new TFile(outFileName.c_str(), "RECREATE");
    outFile->mkdir("Histograms");

    const auto& HT_Edges = Config::HT_Bins();
    const auto& PT_Edges = Config::PT_Bins();
    auto nbLabels = Config::NB_Labels();
    auto etaLabels = Config::Eta_Labels();

    std::map<std::string, TH2D*> sfMapForJSON;

    // ------------------------------------------------------------------------
    // Process each (NB, Eta) category
    // ------------------------------------------------------------------------
    for (const auto& nb : nbLabels) {
        for (const auto& eta : etaLabels) {
            std::string key = nb + "_" + eta;
            std::cout << "\n[Processing] " << key << "\n";

            TH2D* hDataTotal = dynamic_cast<TH2D*>(dataFile->Get(("Total_" + key).c_str()));
            TH2D* hDataPass = dynamic_cast<TH2D*>(dataFile->Get(("Pass_" + key).c_str()));
            TH2D* hMCTotal = dynamic_cast<TH2D*>(mcFile->Get(("Total_" + key).c_str()));
            TH2D* hMCPass = dynamic_cast<TH2D*>(mcFile->Get(("Pass_" + key).c_str()));

            if (!hDataTotal || !hDataPass || !hMCTotal || !hMCPass) {
                std::cout << "  [WARNING] Missing histograms, skipping.\n";
                continue;
            }

            outFile->cd("Histograms");
            hDataTotal->Write(("Data_Total_" + key).c_str());
            hDataPass->Write(("Data_Pass_" + key).c_str());
            hMCTotal->Write(("MC_Total_" + key).c_str());
            hMCPass->Write(("MC_Pass_" + key).c_str());

            TH2D* hEffData = static_cast<TH2D*>(hDataPass->Clone(("effData_" + key).c_str()));
            TH2D* hEffMC = static_cast<TH2D*>(hMCPass->Clone(("effMC_" + key).c_str()));
            TH2D* hSFMeas = static_cast<TH2D*>(hDataPass->Clone(("SF_measured_" + key).c_str()));
            TH2D* hSFFill = static_cast<TH2D*>(hDataPass->Clone(("SF_" + key).c_str()));
            TH2D* hSFErr = static_cast<TH2D*>(hDataPass->Clone(("SF_error_" + key).c_str()));

            hEffData->Reset("ICES");
            hEffMC->Reset("ICES");
            hSFMeas->Reset("ICES");
            hSFFill->Reset("ICES");
            hSFErr->Reset("ICES");

            int nx = hDataPass->GetNbinsX();
            int ny = hDataPass->GetNbinsY();
            int nMeasured = 0;
            std::vector<double> relErrs;

            // Step 1: Compute efficiencies and measured SF
            for (int i = 1; i <= nx; ++i) {
                for (int j = 1; j <= ny; ++j) {
                    double dPass = hDataPass->GetBinContent(i, j);
                    double dTotal = hDataTotal->GetBinContent(i, j);
                    double mPass = hMCPass->GetBinContent(i, j);
                    double mTotal = hMCTotal->GetBinContent(i, j);

                    double dPassErr = hDataPass->GetBinError(i, j);
                    double dTotalErr = hDataTotal->GetBinError(i, j);
                    double mPassErr = hMCPass->GetBinError(i, j);
                    double mTotalErr = hMCTotal->GetBinError(i, j);

                    EffResult effD = computeEfficiency(true, dPass, dPassErr, dTotal, dTotalErr);
                    EffResult effM = computeEfficiency(false, mPass, mPassErr, mTotal, mTotalErr);

                    if (effD.ok) {
                        hEffData->SetBinContent(i, j, effD.eff);
                        hEffData->SetBinError(i, j, effD.err);
                    }
                    if (effM.ok) {
                        hEffMC->SetBinContent(i, j, effM.eff);
                        hEffMC->SetBinError(i, j, effM.err);
                    }

                    bool lowStatData = (dPass < Config::kMinPassData);
                    bool lowStatMC = (effM.neff_pass < Config::kMinNeffMC);

                    if (!effD.ok || !effM.ok || lowStatData || lowStatMC ||
                        effD.eff <= 0.0 || effM.eff <= 0.0) continue;

                    double sf = effD.eff / effM.eff;
                    double sfErr = sf * std::sqrt(std::pow(effD.err / effD.eff, 2) +
                                                   std::pow(effM.err / effM.eff, 2));

                    if (!isFinite(sf) || !isFinite(sfErr) || sf <= 0.0) continue;

                    hSFMeas->SetBinContent(i, j, sf);
                    hSFMeas->SetBinError(i, j, sfErr);
                    ++nMeasured;
                    relErrs.push_back(sfErr / sf);
                }
            }

            // Step 2: Fill empty bins
            double relErrFloor = std::max(0.05, median(relErrs));
            FitModel2D fit = fitQuadratic2DLog("SF_" + key, HT_Edges, PT_Edges, hSFMeas);

            int nFit = 0, nNeighbor = 0, nFallback = 0;

            for (int i = 1; i <= nx; ++i) {
                for (int j = 1; j <= ny; ++j) {
                    double sf0 = hSFMeas->GetBinContent(i, j);
                    double esf0 = hSFMeas->GetBinError(i, j);

                    if (sf0 > 0.0 && esf0 > 0.0) {
                        hSFFill->SetBinContent(i, j, sf0);
                        hSFFill->SetBinError(i, j, esf0);
                        hSFErr->SetBinContent(i, j, esf0);
                        continue;
                    }

                    if (Config::useFitInterpolation) {
                        double ht = hDataPass->GetXaxis()->GetBinCenter(i);
                        double pt = hDataPass->GetYaxis()->GetBinCenter(j);

                        if (fit.ok) {
                            double g = fit.evalLog(ht, pt);
                            double sg = fit.evalLogUnc(ht, pt);
                            if (isFinite(g)) {
                                double pred = std::exp(g);
                                double unc = pred * std::max(sg, relErrFloor);
                                if (isFinite(pred) && pred > 0.0) {
                                    hSFFill->SetBinContent(i, j, pred);
                                    hSFFill->SetBinError(i, j, unc);
                                    hSFErr->SetBinContent(i, j, unc);
                                    ++nFit;
                                    continue;
                                }
                            }
                        }

                        double predN = 0.0, uncN = 0.0;
                        if (fillByNeighbors(i, j, hSFMeas, predN, uncN)) {
                            uncN = std::max(uncN, relErrFloor * predN);
                            hSFFill->SetBinContent(i, j, predN);
                            hSFFill->SetBinError(i, j, uncN);
                            hSFErr->SetBinContent(i, j, uncN);
                            ++nNeighbor;
                            continue;
                        }

                        hSFFill->SetBinContent(i, j, 1.0);
                        hSFFill->SetBinError(i, j, 0.5);
                        hSFErr->SetBinContent(i, j, 0.5);
                        ++nFallback;
                    } else {
                        double predE = 0.0, uncE = 0.0;
                        //if (fillByNearestExtrapolation(i, j, hSFFill, predE, uncE)) {
                            if (fillByNearestExtrapolation(i, j, hSFMeas, predE, uncE)) {
                            uncE = std::max(uncE, relErrFloor * predE);  // 최소 오차 보장
                            hSFFill->SetBinContent(i, j, predE);
                            hSFFill->SetBinError(i, j, uncE);
                            hSFErr->SetBinContent(i, j, uncE);
                            ++nNeighbor;
                        } else {
                            hSFFill->SetBinContent(i, j, 1.0);
                            hSFFill->SetBinError(i, j, 0.5);
                            hSFErr->SetBinContent(i, j, 0.5);
                            ++nFallback;
                        }
                    }
                }
            }

            // Write histograms
            outFile->cd("Histograms");
            hEffData->Write();
            hEffMC->Write();
            hSFMeas->Write();
            hSFFill->Write();
            hSFErr->Write();

            TH2D* hForJSON = static_cast<TH2D*>(hSFFill->Clone(("ForJSON_" + key).c_str()));
            hForJSON->SetDirectory(nullptr);
            sfMapForJSON[key] = hForJSON;

            std::cout << "  Measured: " << nMeasured
                      << ", Fit: " << nFit
                      << ", Neighbor: " << nNeighbor
                      << ", Fallback: " << nFallback << "\n";

            // Generate plots
            MakePrettyPlot(hDataTotal, HT_Edges, PT_Edges,
                           "Data_Total_" + key, "Data Total (" + key + ")",
                           PlotType::Counts);
            MakePrettyPlot(hDataPass, HT_Edges, PT_Edges,
                           "Data_Pass_" + key, "Data Pass (" + key + ")",
                           PlotType::Counts);
            MakePrettyPlot(hMCTotal, HT_Edges, PT_Edges,
                           "MC_Total_" + key, "MC Total (" + key + ")",
                           PlotType::Counts);
            MakePrettyPlot(hMCPass, HT_Edges, PT_Edges,
                           "MC_Pass_" + key, "MC Pass (" + key + ")",
                           PlotType::Counts);

            MakePrettyPlot(hEffData, HT_Edges, PT_Edges,
                           "Eff_Data_" + key, "Data Efficiency (" + key + ")",
                           PlotType::Efficiency);
            MakePrettyPlot(hEffMC, HT_Edges, PT_Edges,
                           "Eff_MC_" + key, "MC Efficiency (" + key + ")",
                           PlotType::Efficiency);

            MakePrettyPlot(hSFMeas, HT_Edges, PT_Edges,
                           "SF_measured_" + key, "SF Measured (" + key + ")",
                           PlotType::SF);
            MakePrettyPlot(hSFFill, HT_Edges, PT_Edges,
                           "SF_filled_" + key, "SF Filled (" + key + ")",
                           PlotType::SF);

            MakePrettyPlot(hSFErr, HT_Edges, PT_Edges,
                           "SF_error_" + key, "SF Error (" + key + ")",
                           PlotType::Error);

            delete hEffData;
            delete hEffMC;
            delete hSFMeas;
            delete hSFFill;
            delete hSFErr;
        }
    }

    outFile->Close();
    dataFile->Close();
    mcFile->Close();

    // ------------------------------------------------------------------------
    // Generate JSON
    // ------------------------------------------------------------------------
    std::cout << "\n>>> Generating " << jsonFileName << "...\n";

    json cset = JSONBuilder::BuildCorrectionSet(sfMapForJSON, HT_Edges, PT_Edges);

    if (JSONBuilder::WriteGzipJSON(cset, jsonFileName)) {
        std::cout << ">>> Success: " << jsonFileName << " created.\n";
    } else {
        std::cerr << "[ERROR] Failed to write JSON.\n";
    }

    for (auto& kv : sfMapForJSON) {
        if (kv.second) delete kv.second;
    }

    delete outFile;
    delete dataFile;
    delete mcFile;

    std::cout << "\n>>> Output: " << outFileName << "\n";
    std::cout << ">>> Output: " << jsonFileName << "\n";
    std::cout << ">>> Done.\n";
}

void DeriveSF_main() { DeriveSF(); }
