#ifndef BTAGSFPROCESSOR_HH
#define BTAGSFPROCESSOR_HH

// ============================================================================
// BTagSFProcessor.hh
//
// B-tag Shape Correction Scale Factor Processor
//
// Applies BTV shape correction SFs using correctionlib,
// following the BTV Internal Wiki recommendations:
//   ω_event = Π_i SF(D_i, pT_i, η_i)   (per-jet SF → event weight)
//   r = Σω_before / Σω_after             (normalization per nJets bin)
//
// Two-step processing (mirrors EventLooper):
//   Mode 0: Compute normalization ratios  → bTagNorm_<sample>.root
//   Mode 1: Apply b-tag SF + trigger SF   → bTagReweight_<sample>.root
//
// Dependencies:
//   - Config.hh       : Paths, binning, sample definitions
//   - NtupleReader.hh : Common branch reader (triggers, kinematics, weights)
//   - correction.h    : correctionlib (b-tag SF + trigger SF JSON provider)
//
// Note on NtupleReader:
//   NtupleReader does not expose bTagScore / hadronFlavour branches.
//   This class reads those additional branches directly from the same TTree,
//   keeping NtupleReader unmodified. Both readers share the same TTree pointer
//   so a single GetEntry() call loads all branches.
//
// Author: Junghyun Lee
// ============================================================================

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TString.h>

#include <vector>
#include <string>
#include <memory>
#include <iostream>

#include "NtupleReader.hh"
#include "correction.h"

// ============================================================================
// BTagConfig: B-tag–specific configuration
//
// Separate from Config.hh (which handles trigger SF binning/paths).
// Keeps b-tag settings self-contained and easily adjustable.
// ============================================================================
namespace BTagConfig {

    // ========================================================================
    // [B-tag SF JSON] correctionlib paths
    // ========================================================================
    // The processor tries each path in order and uses the first accessible one.
    // Path 1: CVMFS (lxplus / condor / grid)
    // Path 2: Local development machine
    inline const std::vector<std::string> btagJsonPaths = {
        "/cvmfs/cms-griddata.cern.ch/cat/metadata/BTV/"
            "Run2-2017-UL-NanoAODv9/latest/btagging.json.gz",
        "/Users/jhlee/correctionLib/corrections/"
            "jsonpog-integration/POG/BTV/2017_UL/btagging.json.gz"
    };

    // Correction key inside the b-tag JSON
    inline const std::string btagCorrectionKey = "deepJet_shape";

    // ========================================================================
    // [Trigger SF JSON] from our trigger SF derivation pipeline
    // ========================================================================
    inline const std::string triggerSFJsonPath = "trigger_sf.json.gz";
    inline const std::string triggerSFCorrectionKey = "triggerSF";

    // ========================================================================
    // [Normalization] nJets binning for yield-preservation ratio
    // ========================================================================
    // BTV Wiki: "the ratio has to be measured and applied per jet multiplicity"
    // Range covers our analysis phase space (nJets ≥ 7 from skimming)
    inline constexpr int nJetsBinMin   = 6;
    inline constexpr int nJetsBinMax   = 22;
    inline constexpr int nJetsNBins    = nJetsBinMax - nJetsBinMin;

    // ========================================================================
    // [Output] file prefixes
    // ========================================================================
    inline const std::string normOutputPrefix     = "bTagNorm_";
    inline const std::string reweightOutputPrefix = "bTagReweight_";

    // ========================================================================
    // [Validation Histograms]
    // ========================================================================
    inline constexpr int maxJetsForHist = 10;   // per-jet histograms: jet 0..9

    // ========================================================================
    // [Systematic] default variation name
    // ========================================================================
    // Central value only for now.
    // Future extension: vector of systematic source names 
    //   (e.g., "up_lf", "down_hf", "up_cferr1", ...)
    // Each systematic requires a separate normalization ratio.
    inline const std::string defaultSystematic = "central";
}


// ============================================================================
// BTagSFProcessor
// ============================================================================
class BTagSFProcessor {
public:
    // ========================================================================
    // Constructor & Destructor
    // ========================================================================
    //   mode = 0 : Compute normalization ratios (first pass)
    //   mode = 1 : Apply b-tag SF + trigger SF + normalization (second pass)
    explicit BTagSFProcessor(int mode);
    ~BTagSFProcessor();

    // ========================================================================
    // Core Interface (same pattern as EventLooper)
    // ========================================================================
    void setNtupleName(TString name);
    void Init();
    void Loop();

private:
    // ========================================================================
    // Processing Mode
    // ========================================================================
    int processingMode;       // 0 = normalization, 1 = reweight

    // ========================================================================
    // Sample Identity
    // ========================================================================
    TString ntupleName = "notDefined";
    bool    isData     = true;
    double  MC_weight  = 1.0;   // (cross section × luminosity) / Σ(genEventSumw)
    TString dataSet    = "default";
    TString era        = "default";

    // ========================================================================
    // I/O: Input file / TTree / NtupleReader
    // ========================================================================
    TFile*        inputFile = nullptr;
    TTree*        fChain    = nullptr;
    NtupleReader* reader    = nullptr;

    // ========================================================================
    // Additional Branches (not in NtupleReader)
    //
    // These are set up on the SAME TTree that NtupleReader uses.
    // When reader->GetEntry(i) loads an event, these branches are
    // also filled because they share the same TTree pointer.
    // ========================================================================
    std::vector<float>* bTagScore      = nullptr;
    std::vector<int>*   hadronFlavour  = nullptr;
    TBranch* b_bTagScore      = nullptr;
    TBranch* b_hadronFlavour  = nullptr;

    // ========================================================================
    // correctionlib Providers
    // ========================================================================
    // B-tag shape SF: deepJet_shape
    std::unique_ptr<correction::CorrectionSet>  btagCorrSet;
    correction::Correction::Ref                 btagSFProvider = nullptr;

    // Trigger SF: from our derivation pipeline
    std::unique_ptr<correction::CorrectionSet>  trigCorrSet;
    correction::Correction::Ref                 trigSFProvider = nullptr;

    // ========================================================================
    // Normalization (loaded in mode=1 from mode=0 output)
    // ========================================================================
    TH1D*  normRatioHist = nullptr;
    TFile* normFile      = nullptr;

    // ========================================================================
    // Helper Methods
    // ========================================================================
    TString getInputName()  const;
    TString getOutputName() const;

    /// Identify whether sample is Data or MC, set MC_weight, parse era
    void identifySample(const TString& sampleName);

    /// Compute the b-tag event weight: product of per-jet SFs
    /// Returns 1.0 for data or if evaluation fails
    double computeBTagEventWeight(const std::string& systematic) const;

    /// Get trigger SF from correctionlib JSON (returns 1.0 for data or failure)
    double getTriggerSF(int nbJets, double jet6Eta,
                        double ht, double jet6Pt) const;

    /// Look up normalization ratio for a given nJets value (mode=1 only)
    double getNormRatio(int nJets) const;
};

#endif // BTAGSFPROCESSOR_HH
