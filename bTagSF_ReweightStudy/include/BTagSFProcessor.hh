#ifndef BTagSFProcessor_hh
#define BTagSFProcessor_hh

// ============================================================================
// BTagSFProcessor.hh
//
// B-tag reshape Scale Factor reweighting processor.
//
// Uses correctionlib (deepJet_shape) to compute per-event b-tag weights,
// following BTV recommendations for shape correction SFs.
//
// Two-pass architecture:
//   Pass 1: Compute normalization ratios per (nJet, [HT]) bin per systematic
//   Pass 2: Apply b-tag SF × normalization × trigger SF, fill histograms
//
// The reweight axis dimensionality (1D nJets or 2D nJets×HT) is controlled
// by Config::useHTForReweight at compile time.
//
// Reference:
//   - BTV Internal Wiki: "Recommendations for Shape Correction SFs"
//
// Author: Junghyun Lee
// ============================================================================

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TString.h>

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "NtupleReader.hh"
#include "correction.h"

class BTagSFProcessor {
public:
    BTagSFProcessor();
    virtual ~BTagSFProcessor();

    void setNtupleName(TString name);
    void Init();
    void Loop();

private:
    // ── File naming ──
    TString getInputName()  const;
    TString getOutputName() const;

    // ── B-tag SF evaluation ──

    /// Evaluate single-jet b-tag shape SF via correctionlib.
    double evaluateJetBTagSF(const std::string& systematic,
                             int    hadronFlavor,
                             double abseta,
                             double pt,
                             double discriminant) const;

    /// Per-event b-tag weight = ∏ SF(jet_i) over all jets
    double computeBTagEventWeight(const std::string& systematic) const;

    /// Flavor-aware systematic selection (BTV recommendation):
    ///   c-jet → only cferr1/2;  b/light → everything except cferr
    static std::string resolveJetSystematic(int hadronFlavor,
                                            const std::string& eventSystematic);

    /// Auto-detect b-tag JSON path (CVMFS first, then local fallback)
    static std::string resolveBTagJSONPath();

    // ── Trigger SF evaluation ──

    /// Evaluate trigger SF.  FATAL on failure (no fallback).
    double evaluateTriggerSF(int nBJets, double jet6Eta,
                             double HT, double jet6PT) const;

    // ── Trigger logic ──

    /// Hadronic trigger OR with PD-exclusivity for Data
    bool passHadronicTrigger(const TString& dataSet, const TString& era) const;

    // ── Members ──
    TString ntupleName = "notDefined";
    bool    isData     = true;

    TFile*        inputFile = nullptr;
    TTree*        fChain    = nullptr;
    NtupleReader* reader    = nullptr;

    // correctionlib: b-tag shape SF
    std::unique_ptr<correction::CorrectionSet>    btagCorrectionSet;
    std::shared_ptr<const correction::Correction> btagShapeSFProvider;

    // Input-slot mapping for btagShapeSFProvider->evaluate()
    // Discovered at runtime by inspecting JSON schema
    struct BTagInputSlots {
        int systematic   = -1;
        int workingPoint = -1;
        int flavor       = -1;
        int abseta       = -1;
        int pt           = -1;
        int discriminant = -1;
        int totalInputs  = 0;
    };
    BTagInputSlots btagSlots;

    // correctionlib: trigger SF
    std::unique_ptr<correction::CorrectionSet>    trigCorrectionSet;
    std::shared_ptr<const correction::Correction> trigSFProvider;
};

#endif
