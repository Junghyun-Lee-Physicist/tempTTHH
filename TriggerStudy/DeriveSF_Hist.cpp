// DeriveSF_Hist.cpp
// ============================================================================
// Purpose
//   - Build trigger efficiencies and trigger scale factors (SF = effData/effMC)
//     from existing Pass/Total TH2 histograms produced by EventLooper.
//   - Use ONLY statistical uncertainties at this stage.
//   - First: build SF only in bins with reliable statistics (Pass >= 50).
//   - Then: "extend/fill" empty (masked) bins so SF map has no holes.
//     Motivation: leaving holes (or forcing SF=1.0) can degrade the corrected
//     efficiency in validation, while nearby bins indicate SF varies smoothly.
//     Therefore we assume SF is a smooth function of (HT, pT6, optional categories)
//     and we fill empty bins using a statistically-weighted fit (and fallback
//     local interpolation).
//
// Notes for future (systematics)
//   - Later we will add method/systematic uncertainties (e.g. model dependence,
//     selection variations, reference trigger bias, etc.) as additional NP(s).
//   - For now, the filled-bin uncertainty is taken from the fit covariance,
//     i.e. propagated statistical uncertainty only.
//
// IMPORTANT (errors / Sumw2)
//   - For MC weighted histograms, EventLooper MUST enable Sumw2 (or TH1::SetDefaultSumw2())
//     before filling histograms. Otherwise GetBinError() will be zero/incorrect
//     and statistical uncertainties cannot be computed properly.
//
// Run
//   root -l -b -q "DeriveSF_Hist.cpp"
//
// ============================================================================

#include <TFile.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TLatex.h>
#include <TString.h>

#include <TGraph2DErrors.h>
#include <TF2.h>
#include <TFitResultPtr.h>
#include <TFitResult.h>
#include <TMatrixDSym.h>
#include <TEfficiency.h>

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <limits>
#include <memory>
#include <array>

#include "include/BinConfig.hh"

// ============================================================================
// [Helper] Drawing Function
// ============================================================================
void MakePrettyPlot(TH2D* h_orig,
                    const std::vector<double>& x_vec,
                    const std::vector<double>& y_vec,
                    TString outName,
                    TString title,
                    TString zTitle,
                    TString zFormat)
{
    if (!h_orig) { std::cerr << "[ERROR] h_orig is NULL for " << outName << std::endl; return; }

    int nBinsX = (int)x_vec.size() - 1;
    int nBinsY = (int)y_vec.size() - 1;

    TH2D* h_uniform = new TH2D(Form("%s_uniform_%s", h_orig->GetName(), outName.Data()),
                               title, nBinsX, 0, nBinsX, nBinsY, 0, nBinsY);

    for (int i = 1; i <= nBinsX; ++i) {
        for (int j = 1; j <= nBinsY; ++j) {
            double content = h_orig->GetBinContent(i, j);
            if (std::isnan(content) || std::isinf(content)) content = 0.0;
            h_uniform->SetBinContent(i, j, content);
        }
    }

    for (int i = 1; i <= nBinsX; ++i) {
        h_uniform->GetXaxis()->SetBinLabel(i, Form("%.0f-%.0f", x_vec[i-1], x_vec[i]));
    }
    for (int j = 1; j <= nBinsY; ++j) {
        h_uniform->GetYaxis()->SetBinLabel(j, Form("%.0f-%.0f", y_vec[j-1], y_vec[j]));
    }

    h_uniform->GetXaxis()->SetTitle("HT bin [GeV]");
    h_uniform->GetYaxis()->SetTitle("6th jet pT bin [GeV]");
    if (zTitle.Length() > 0) h_uniform->GetZaxis()->SetTitle(zTitle);

    auto* c = new TCanvas(Form("c_%s", outName.Data()), "c", 1000, 800);
    gStyle->SetOptStat(0);
    gStyle->SetPaintTextFormat(zFormat);

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

namespace {

inline bool isFinite(double x) { return std::isfinite(x); }

// Neff = (sumw)^2 / sumw2 ; if hist errors are sqrt(sumw2)
inline double neff(double sumw, double err) {
    if (!isFinite(sumw) || !isFinite(err) || err <= 0.0) return 0.0;
    return (sumw * sumw) / (err * err);
}

double median(std::vector<double> v) {
    if (v.empty()) return 0.0;
    std::sort(v.begin(), v.end());
    size_t n = v.size();
    if (n % 2) return v[n/2];
    return 0.5*(v[n/2-1] + v[n/2]);
}

struct EffResult {
    double eff = 0.0;
    double err = 0.0;
    bool ok = false;
    double neff_total = 0.0;
    double neff_pass  = 0.0;
};

// Data: Clopper-Pearson 68% CI (requires integer-like counts)
// MC:   ratio propagation using bin errors (Sumw2 needed for weighted hist)
EffResult computeEfficiency(bool isData,
                            double passW, double passErr,
                            double totalW, double totalErr)
{
    EffResult r;
    if (!isFinite(passW) || !isFinite(totalW) || totalW <= 0.0) return r;
    if (passW < 0.0) return r;

    if (passW > totalW && isData) return r;

    r.eff = passW / totalW;
    if (!isFinite(r.eff) || r.eff < 0.0) return r;

    r.neff_total = neff(totalW, totalErr);
    r.neff_pass  = neff(passW,  passErr);

    if (isData) {
        int n = std::max(0, (int)std::llround(totalW));
        int k = std::max(0, (int)std::llround(passW));
        if (n <= 0 || k < 0 || k > n) return r;

        const double level = 0.682689492137; // ~1 sigma
        double lo = TEfficiency::ClopperPearson(n, k, level, false);
        double hi = TEfficiency::ClopperPearson(n, k, level, true);
        r.err = 0.5*(hi - lo);
        r.ok = isFinite(r.err);
        return r;
    }

    if (!isFinite(passErr) || !isFinite(totalErr)) return r;
    if (passW <= 0.0 || totalW <= 0.0) return r;

    double relPass = (passErr > 0.0) ? (passErr/passW) : 0.0;
    double relTot  = (totalErr > 0.0) ? (totalErr/totalW) : 0.0;
    r.err = r.eff * std::sqrt(relPass*relPass + relTot*relTot);
    r.ok = isFinite(r.err);
    return r;
}

// Fit model for log(SF) in normalized coordinates
struct FitModel2DLog {
    double ht0=0, htS=1, pt0=0, ptS=1;
    TF2* f=nullptr;
    TMatrixDSym cov;
    bool ok=false;
    double chi2ndf=-1;
    int npoints=0;

    std::array<double,6> basis(double ht, double pt) const {
        double xn = (htS!=0.0) ? (ht-ht0)/htS : 0.0;
        double yn = (ptS!=0.0) ? (pt-pt0)/ptS : 0.0;
        return {1.0, xn, yn, xn*yn, xn*xn, yn*yn};
    }
    double evalLog(double ht, double pt) const { return f ? f->Eval(ht,pt) : 0.0; }

    double evalLogUnc(double ht, double pt) const {
        if (!ok) return 0.0;
        if (cov.GetNrows() != 6) return 0.0;
        auto b = basis(ht,pt);
        double v=0.0;
        for(int i=0;i<6;++i){
            for(int j=0;j<6;++j){
                v += b[i]*cov(i,j)*b[j];
            }
        }
        return (v>0 && isFinite(v)) ? std::sqrt(v) : 0.0;
    }
};

FitModel2DLog fitQuadratic2DLog(const std::string& name,
                                const std::vector<double>& HT_vec,
                                const std::vector<double>& PT_vec,
                                TH2D* sf_measured)
{
    FitModel2DLog m;
    if (!sf_measured) return m;

    double htMin = HT_vec.front(), htMax = HT_vec.back();
    double ptMin = PT_vec.front(), ptMax = PT_vec.back();

    m.ht0 = 0.5*(htMin+htMax); m.htS = 0.5*(htMax-htMin);
    m.pt0 = 0.5*(ptMin+ptMax); m.ptS = 0.5*(ptMax-ptMin);

    auto gr = std::make_unique<TGraph2DErrors>();
    gr->SetName(Form("grLog_%s", name.c_str()));

    int nx = sf_measured->GetNbinsX();
    int ny = sf_measured->GetNbinsY();

    int ip=0;
    for(int i=1;i<=nx;++i){
        for(int j=1;j<=ny;++j){
            double sf  = sf_measured->GetBinContent(i,j);
            double esf = sf_measured->GetBinError(i,j);
            if (!isFinite(sf) || !isFinite(esf)) continue;
            if (sf <= 0.0 || esf <= 0.0) continue;

            double z  = std::log(sf);
            double ez = esf / sf; // sigma(log sf) ≈ sigma(sf)/sf
            if (!isFinite(z) || !isFinite(ez) || ez<=0.0) continue;

            double x = sf_measured->GetXaxis()->GetBinCenter(i);
            double y = sf_measured->GetYaxis()->GetBinCenter(j);

            gr->SetPoint(ip, x, y, z);
            gr->SetPointError(ip, 0.0, 0.0, ez);
            ++ip;
        }
    }

    m.npoints = ip;
    if (ip < 12) return m;

    TString formula = Form(
        "[0]"
        "+[1]*((x-%.9g)/%.9g)"
        "+[2]*((y-%.9g)/%.9g)"
        "+[3]*((x-%.9g)/%.9g)*((y-%.9g)/%.9g)"
        "+[4]*pow((x-%.9g)/%.9g,2)"
        "+[5]*pow((y-%.9g)/%.9g,2)",
        m.ht0,m.htS, m.pt0,m.ptS,
        m.ht0,m.htS, m.pt0,m.ptS,
        m.ht0,m.htS, m.pt0,m.ptS
    );

    m.f = new TF2(Form("fitLog_%s", name.c_str()), formula, htMin, htMax, ptMin, ptMax);
    m.f->SetParameters(0,0,0,0,0,0); // log(SF) ~ 0 => SF ~ 1

    TFitResultPtr fr = gr->Fit(m.f, "QSR");
    if (!fr.Get() || fr->Status()!=0) return m;

    m.cov = fr->GetCovarianceMatrix();
    if (m.cov.GetNrows() != 6) return m;

    double ndf = fr->Ndf();
    m.chi2ndf = (ndf>0) ? (fr->Chi2()/ndf) : -1.0;
    m.ok = true;
    return m;
}

// Local interpolation fallback
bool fillByNeighbors(int ix, int iy,
                     TH2D* sf_measured,
                     double& out_sf,
                     double& out_esf)
{
    int nx = sf_measured->GetNbinsX();
    int ny = sf_measured->GetNbinsY();

    for (int r = 1; r <= std::max(nx, ny); ++r) {
        double wsum=0.0, vsum=0.0;
        for (int i = std::max(1, ix-r); i <= std::min(nx, ix+r); ++i) {
            for (int j = std::max(1, iy-r); j <= std::min(ny, iy+r); ++j) {
                double sf  = sf_measured->GetBinContent(i,j);
                double esf = sf_measured->GetBinError(i,j);
                if (sf<=0.0 || esf<=0.0) continue;

                int di = std::abs(i-ix);
                int dj = std::abs(j-iy);
                int d  = std::max(1, di+dj);
                double w = 1.0/(esf*esf) * 1.0/(double)d;
                wsum += w;
                vsum += w*sf;
            }
        }
        if (wsum > 0.0) {
            out_sf  = vsum/wsum;
            out_esf = std::sqrt(1.0/wsum);
            return isFinite(out_sf) && isFinite(out_esf) && out_sf>0.0 && out_esf>0.0;
        }
    }
    return false;
}

} // namespace

// ============================================================================
// Main
// ============================================================================
void DeriveSF_Hist()
{
    BinConfig::Load("config.yaml");

    TFile* dataFile = TFile::Open("Data.root");
    TFile* mcFile   = TFile::Open("ttJets.root");
    if (!dataFile || !mcFile) {
        std::cerr << "[CRITICAL] Input root files missing!\n";
        return;
    }

    TFile* outFile = new TFile("ScaleFactors.root","RECREATE");
    outFile->mkdir("ScaleFactors");

    auto nbLabels  = BinConfig::NB_Labels();
    auto etaLabels = BinConfig::Eta_Labels();
    const auto& HT_vec = BinConfig::HT_Bins();
    const auto& PT_vec = BinConfig::PT_Bins();

    const double kMinPassCount_Data = 50.0;
    const double kMinPassNeff_MC    = 50.0;

    for (const auto& nb : nbLabels) {
        for (const auto& eta : etaLabels) {
            std::string key = nb + "_" + eta;
            std::cout << "\n[Processing] " << key << "\n";

            TH2D* h_data_total = (TH2D*)dataFile->Get(("Total_" + key).c_str());
            TH2D* h_data_pass  = (TH2D*)dataFile->Get(("Pass_" + key).c_str());
            TH2D* h_mc_total   = (TH2D*)mcFile->Get(("Total_" + key).c_str());
            TH2D* h_mc_pass    = (TH2D*)mcFile->Get(("Pass_" + key).c_str());

            if (!h_data_total || !h_data_pass || !h_mc_total || !h_mc_pass) {
                std::cout << "  > [Warning] Missing histograms for " << key << "\n";
                continue;
            }

            outFile->cd("ScaleFactors");
            h_data_total->Write(("Data_Total_" + key).c_str());
            h_data_pass->Write(("Data_Pass_" + key).c_str());
            h_mc_total->Write(("MC_Total_" + key).c_str());
            h_mc_pass->Write(("MC_Pass_" + key).c_str());

            TH2D* effData = (TH2D*)h_data_pass->Clone(("effData_"+key).c_str());
            TH2D* effMC   = (TH2D*)h_mc_pass->Clone(("effMC_"+key).c_str());
            effData->Reset("ICES"); effMC->Reset("ICES");

            TH2D* sf_meas = (TH2D*)h_data_pass->Clone(("SF_measured_"+key).c_str());
            TH2D* sf_fill = (TH2D*)h_data_pass->Clone(("SF_filled_"+key).c_str());
            sf_meas->Reset("ICES"); sf_fill->Reset("ICES");

            TH2D* h_source = (TH2D*)h_data_pass->Clone(("SF_source_"+key).c_str());
            h_source->Reset("ICES"); // 0 measured / 1 fit / 2 neighbor / 3 fallback

            int nx = h_data_pass->GetNbinsX();
            int ny = h_data_pass->GetNbinsY();

            int nMeasured=0;
            std::vector<double> relErrs; relErrs.reserve(nx*ny);

            // 1) efficiencies + measured SF
            for(int i=1;i<=nx;++i){
                for(int j=1;j<=ny;++j){
                    double dPass = h_data_pass->GetBinContent(i,j);
                    double dTot  = h_data_total->GetBinContent(i,j);
                    double mPass = h_mc_pass->GetBinContent(i,j);
                    double mTot  = h_mc_total->GetBinContent(i,j);

                    double dPassErr = h_data_pass->GetBinError(i,j);
                    double dTotErr  = h_data_total->GetBinError(i,j);
                    double mPassErr = h_mc_pass->GetBinError(i,j);
                    double mTotErr  = h_mc_total->GetBinError(i,j);

                    EffResult ed = computeEfficiency(true,  dPass, dPassErr, dTot, dTotErr);
                    EffResult em = computeEfficiency(false, mPass, mPassErr, mTot, mTotErr);

                    if (ed.ok) { effData->SetBinContent(i,j,ed.eff); effData->SetBinError(i,j,ed.err); }
                    if (em.ok) { effMC->SetBinContent(i,j,em.eff);   effMC->SetBinError(i,j,em.err);   }

                    bool lowStatData = (dPass < kMinPassCount_Data);
                    bool lowStatMC   = (em.neff_pass < kMinPassNeff_MC);

                    if (!ed.ok || !em.ok || lowStatData || lowStatMC) {
                        sf_meas->SetBinContent(i,j,0.0);
                        sf_meas->SetBinError(i,j,0.0);
                        continue;
                    }

                    if (ed.eff<=0.0 || em.eff<=0.0) {
                        sf_meas->SetBinContent(i,j,0.0);
                        sf_meas->SetBinError(i,j,0.0);
                        continue;
                    }

                    double sf = ed.eff / em.eff;
                    double sfErr = sf * std::sqrt( std::pow(ed.err/ed.eff,2) + std::pow(em.err/em.eff,2) );

                    if (!isFinite(sf) || !isFinite(sfErr) || sf<=0.0 || sfErr<=0.0) {
                        sf_meas->SetBinContent(i,j,0.0);
                        sf_meas->SetBinError(i,j,0.0);
                        continue;
                    }

                    sf_meas->SetBinContent(i,j,sf);
                    sf_meas->SetBinError(i,j,sfErr);
                    h_source->SetBinContent(i,j,0.0);
                    ++nMeasured;
                    relErrs.push_back(sfErr/sf);
                }
            }

            // 2) fill holes (fit in log(SF) + neighbor fallback)
            double relErrFloor = std::max(0.05, median(relErrs));
            FitModel2DLog fit = fitQuadratic2DLog("SF_"+key, HT_vec, PT_vec, sf_meas);

            int nFitFill=0, nNeighborFill=0, nFallback=0;

            for(int i=1;i<=nx;++i){
                for(int j=1;j<=ny;++j){
                    double sf0  = sf_meas->GetBinContent(i,j);
                    double esf0 = sf_meas->GetBinError(i,j);

                    if (sf0>0.0 && esf0>0.0) {
                        sf_fill->SetBinContent(i,j,sf0);
                        sf_fill->SetBinError(i,j,esf0);
                        continue;
                    }

                    double ht = h_data_pass->GetXaxis()->GetBinCenter(i);
                    double pt = h_data_pass->GetYaxis()->GetBinCenter(j);

                    if (fit.ok) {
                        double g  = fit.evalLog(ht,pt);
                        double sg = fit.evalLogUnc(ht,pt);

                        if (isFinite(g)) {
                            double pred = std::exp(g);
                            double unc  = pred * std::max(sg, relErrFloor);

                            if (isFinite(pred) && isFinite(unc) && pred>0.0 && unc>0.0) {
                                sf_fill->SetBinContent(i,j,pred);
                                sf_fill->SetBinError(i,j,unc);
                                h_source->SetBinContent(i,j,1.0);
                                ++nFitFill;
                                continue;
                            }
                        }
                    }

                    double predN=0.0, uncN=0.0;
                    if (fillByNeighbors(i,j,sf_meas,predN,uncN)) {
                        uncN = std::max(uncN, relErrFloor*predN);
                        sf_fill->SetBinContent(i,j,predN);
                        sf_fill->SetBinError(i,j,uncN);
                        h_source->SetBinContent(i,j,2.0);
                        ++nNeighborFill;
                        continue;
                    }

                    sf_fill->SetBinContent(i,j,1.0);
                    sf_fill->SetBinError(i,j,0.50);
                    h_source->SetBinContent(i,j,3.0);
                    ++nFallback;
                }
            }

            // Write outputs
            outFile->cd("ScaleFactors");
            effData->Write();
            effMC->Write();
            sf_meas->Write();
            sf_fill->Write();

            // Backward-compatible name (older produceJSON.py usually expects "SF_<key>")
            TH2D* sf_apply = (TH2D*)sf_fill->Clone(("SF_"+key).c_str());
            sf_apply->Write();
            delete sf_apply;

            h_source->Write();
            if (fit.f) fit.f->Write(Form("FitFuncLog_%s", key.c_str()));

            std::cout << "  > measured bins   : " << nMeasured << "\n";
            std::cout << "  > fit status      : " << (fit.ok ? "OK" : "FAIL") << "\n";
            std::cout << "  > fit points      : " << fit.npoints << "\n";
            std::cout << "  > chi2/ndf        : " << fit.chi2ndf << "\n";
            std::cout << "  > filled by fit   : " << nFitFill << "\n";
            std::cout << "  > filled neighbor : " << nNeighborFill << "\n";
            std::cout << "  > fallback        : " << nFallback << "\n";
            std::cout << "  > relErr floor    : " << relErrFloor << "\n";

            MakePrettyPlot(effData, HT_vec, PT_vec,
                           "Eff_Data_"+key, "Data Efficiency ("+key+")", "Efficiency", "%.3f");
            MakePrettyPlot(effMC,   HT_vec, PT_vec,
                           "Eff_MC_"+key,   "MC Efficiency ("+key+")", "Efficiency", "%.3f");
            MakePrettyPlot(sf_meas, HT_vec, PT_vec,
                           "SF_measured_"+key, "Scale Factor measured ("+key+")", "Scale Factor", "%.3f");
            MakePrettyPlot(sf_fill, HT_vec, PT_vec,
                           "SF_filled_"+key,   "Scale Factor filled ("+key+")", "Scale Factor", "%.3f");

            delete effData;
            delete effMC;
            delete sf_meas;
            delete sf_fill;
            delete h_source;
        }
    }

    outFile->Close();
    std::cout << "\n>>> Done: ScaleFactors.root written (stat-only, filled).\n";
}

