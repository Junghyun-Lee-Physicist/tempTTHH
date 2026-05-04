// ============================================================================
// WeightAblation.h
// ----------------------------------------------------------------------------
// Validation-mode helper: maintains a set of parallel "weight chains" and
// fills the same kinematic histogram once per chain so that the marginal
// contribution of each SF (b-tag shape, b-tag norm reweight, trigger SF,
// top-pT reweight) can be read off as ratios between chain outputs.
//
// Designed to be a pure ADD: zero impact on the production weight pipeline
// when used in main mode (the ablation switch is OFF by default).
//
// Used in: kValidationStudy mode (NEW; see ttHHanalyzer_unified.h additions).
//
// Author: Junghyun Lee
// ============================================================================
#ifndef WEIGHT_ABLATION_H
#define WEIGHT_ABLATION_H

#include <TH1D.h>
#include <TFile.h>
#include <TDirectory.h>
#include <string>
#include <vector>
#include <map>
#include <array>

class WeightAblation {
public:
    // ── Chain identifiers ──────────────────────────────────────────────────
    // Order matches the docs/PHYSICS.md weight-ablation table.
    enum Chain {
        kRaw          = 0,    // genW · PU · L1 · xsec/Σgenw  (no SF)
        kBtagShape    = 1,    // + b-tag shape SF
        kBtagFull     = 2,    // + b-tag shape SF + b-tag norm reweight
        kTrig         = 3,    // + trigger SF only
        kTrigBtag     = 4,    // + trigSF · btagShape · btagNormRW (current default)
        kTopPt        = 5,    // + top pT reweight only
        kFullAllSF    = 6,    // + everything (trigSF · btagShape · btagNormRW · topPt)
        N_CHAINS      = 7
    };

    // human-readable suffix appended to histogram names
    static const std::array<const char*, N_CHAINS>& ChainNames() {
        static const std::array<const char*, N_CHAINS> names = {
            "raw", "btagShape", "btagFull", "trig",
            "trigBtag", "topPt", "fullAllSF"
        };
        return names;
    }

    // ── Constructor ────────────────────────────────────────────────────────
    // outDir : ROOT directory in which to create per-chain TH1Ds.
    //          A subdirectory "ablation/" will be created if not present.
    // varName: e.g. "ht", "higgs_can01", "jet1_BTag" — must match the
    //          histogram-naming convention used elsewhere in the analyzer.
    // step   : cutflow step index (6, 7, 8, ...) — the histogram is created
    //          per (step, varName, chain) triplet.
    // ───────────────────────────────────────────────────────────────────────
    WeightAblation(TDirectory* outDir,
                   const std::string& varName,
                   int step,
                   int nbins, double xlo, double xhi)
    {
        outDir->cd();
        TDirectory* sub = outDir->GetDirectory("ablation");
        if (!sub) sub = outDir->mkdir("ablation");
        sub->cd();
        for (int c = 0; c < N_CHAINS; ++c) {
            const std::string hname = "h_" + varName + "_step"
                + std::to_string(step) + "_" + ChainNames()[c];
            const std::string htitle = varName + " (step " + std::to_string(step)
                + ", chain=" + ChainNames()[c] + ")";
            _hists[c] = new TH1D(hname.c_str(), htitle.c_str(), nbins, xlo, xhi);
            _hists[c]->Sumw2();
            _hists[c]->SetDirectory(sub);
        }
    }

    // ── Weight bundle (filled by analyzer at fill time) ────────────────────
    struct Weights {
        // Components — pass them in as the analyzer computes them.
        double w_base       = 1.0;  // genW · PU · L1 · xsec/Σgenw · stitchingW
        double sf_btagShape = 1.0;  // Π SF_jet — b-tag shape
        double sf_btagNorm  = 1.0;  // ratio[group, nJets, HT]
        double sf_trig      = 1.0;  // trigger SF
        double sf_topPt     = 1.0;  // top pT reweight (1.0 if not used)
    };

    // ── Fill all chains with one call ──────────────────────────────────────
    // For Data, pass everything as 1.0 except w_base which should be 1.0 too,
    // and use a separate Data histogram outside this helper (Data is not part
    // of the ablation; it's the comparison target).
    void Fill(double x, const Weights& w) {
        const double w_raw       = w.w_base;
        const double w_btagShape = w_raw * w.sf_btagShape;
        const double w_btagFull  = w_btagShape * w.sf_btagNorm;
        const double w_trig      = w_raw * w.sf_trig;
        const double w_trigBtag  = w_btagFull * w.sf_trig;
        const double w_topPt     = w_raw * w.sf_topPt;
        const double w_full      = w_trigBtag * w.sf_topPt;

        _hists[kRaw]       ->Fill(x, w_raw);
        _hists[kBtagShape] ->Fill(x, w_btagShape);
        _hists[kBtagFull]  ->Fill(x, w_btagFull);
        _hists[kTrig]      ->Fill(x, w_trig);
        _hists[kTrigBtag]  ->Fill(x, w_trigBtag);
        _hists[kTopPt]     ->Fill(x, w_topPt);
        _hists[kFullAllSF] ->Fill(x, w_full);
    }

    TH1D* Get(Chain c) { return _hists[c]; }

private:
    std::array<TH1D*, N_CHAINS> _hists{};
};

#endif // WEIGHT_ABLATION_H
