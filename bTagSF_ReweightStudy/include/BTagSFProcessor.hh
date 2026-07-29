#ifndef BTagSFProcessor_hh
#define BTagSFProcessor_hh

// (모든 기존 #include는 그대로 유지)
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TH1D.h>
#include <TH2D.h>

#include <memory>
#include <string>
#include <vector>
#include <map>

#include "correction.h"
#include "NtupleReader.hh"

class BTagSFProcessor {
public:
    BTagSFProcessor();
    virtual ~BTagSFProcessor();

    void setNtupleName(TString name);
    void Init();
    void Loop();

private:
    TString getInputName()  const;
    TString getOutputName() const;

    double evaluateJetBTagSF(const std::string& systematic,
                             int    hadronFlavor,
                             double abseta,
                             double pt,
                             double discriminant) const;

    double computeBTagEventWeight(const std::string& systematic) const;

    static std::string resolveJetSystematic(int hadronFlavor,
                                            const std::string& eventSystematic);

    static std::string resolveBTagJSONPath();

    double evaluateTriggerSF(int nBJets, double jet6Eta,
                             double HT, double jet6PT) const;

    bool passHadronicTrigger(const TString& dataSet, const TString& era) const;

    /// [2026-07-29] 측정 영역의 lepton/MET 조건 (env TTHH_BTAGRW_REGION).
    ///   FH / muonCR / none — 구현부 주석 참조.
    bool passRegion() const;

    // [NEW] ttbar category dispatch helper.
    // Returns the process key for the *current* event being processed.
    //   - inclusive ttbar (TTbar_Hadronic / TTbar_SemiLep / TTbar_DiLep):
    //     "<sample>_LF" / "<sample>_cc" / "<sample>_B"  (via genTtbarId)
    //   - other samples: sample name unchanged
    // [Ref] ttH AN-19-094 §A.2
    std::string CurrentEventProcessKey(const std::string& sampleName) const;

    // ── Members ──
    TString ntupleName = "notDefined";
    bool    isData     = true;

    TFile*        inputFile = nullptr;
    TTree*        fChain    = nullptr;
    NtupleReader* reader    = nullptr;

    std::unique_ptr<correction::CorrectionSet>    btagCorrectionSet;
    std::shared_ptr<const correction::Correction> btagShapeSFProvider;

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

    std::unique_ptr<correction::CorrectionSet>    trigCorrectionSet;
    std::shared_ptr<const correction::Correction> trigSFProvider;
};

#endif
