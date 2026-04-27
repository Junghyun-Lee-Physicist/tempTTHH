#ifndef CONFIG_HH
#define CONFIG_HH

// ============================================================================
// Config.hh
//
// Centralized configuration for B-Tag Shape SF Reweight.
// All settings are defined in pure C++ - no external config files needed.
//
// Structure:
//   Section 1: Path & I/O Settings
//     1-A: Trigger SF JSON (for evaluation only)
//     1-B: Sample Registry
//     1-C: B-Tag Reweight Configuration
//   Section 2: Analysis Flags
//   Section 3: Histogram Definitions
//   Section 4: Reweight Bin Utilities
//   Section 5: Debug Dump
//
// Author: Junghyun Lee
// ============================================================================

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <sstream>

#include "Config_TtCatGroup.hh"


// ============================================================================
// Config: Central configuration manager
// ============================================================================
class Config {
public:
    // ========================================================================
    // [Section 1] Path & I/O Settings
    // ========================================================================

    // Base directory containing input ntuple files
    static inline const std::string inputBaseDir =
        "/Users/jhlee/ttHH/ntuple/skimmed/gen_tier3/";

    // TTree path inside ROOT file
    static inline const std::string treePath = "Tree/Tree";

    // ========================================================================
    // [Section 1-A] Trigger SF JSON
    //
    // Path to the pre-derived trigger SF (correctionlib JSON).
    // This file was produced by the Trigger SF derivation pipeline
    // with edge-value extrapolation enabled, so every bin is guaranteed
    // to have a valid SF value.  If evaluate() fails, it is a FATAL error.
    // ========================================================================

    static inline const std::string sfOutputJSON =
        "../DerivedCorr/TriggerSF/trigger_sf.json.gz";

    // ========================================================================
    // [Section 1-B] Sample Registry
    //
    // Central definition of all known samples.
    //   - isData: true for Data, false for MC
    //   - weight: (Xsec * Lumi) / Sum(genEventSumw) for MC, 1.0 for Data
    //
    // Usage:
    //   auto info = Config::GetSampleInfo("TTTo2L2Nu");
    //   if (!info) { /* unknown sample → error */ }
    //   bool isData = info->isData;
    //   double w    = info->weight;
    // ========================================================================

    struct SampleInfo {
        bool   isData;
        double weight;
    };

private:
    static const std::map<std::string, SampleInfo>& SampleRegistry() {
        static const std::map<std::string, SampleInfo> registry = {
            // Data: SingleMuon (for trigger SF derivation; NOT used here)
            {"SingleMuon_B",       {true,  1.0}},
            {"SingleMuon_C",       {true,  1.0}},
            {"SingleMuon_D",       {true,  1.0}},
            {"SingleMuon_E",       {true,  1.0}},
            {"SingleMuon_F",       {true,  1.0}},
            // Data: BTagCSV
            {"BTagCSV_B",          {true,  1.0}},
            {"BTagCSV_C",          {true,  1.0}},
            {"BTagCSV_D",          {true,  1.0}},
            {"BTagCSV_E",          {true,  1.0}},
            {"BTagCSV_F",          {true,  1.0}},
            // Data: JetHT
            {"JetHT_B",            {true,  1.0}},
            {"JetHT_C",            {true,  1.0}},
            {"JetHT_D",            {true,  1.0}},
            {"JetHT_E",            {true,  1.0}},
            {"JetHT_F",            {true,  1.0}},
            // MC: ttbar inclusive
            {"TTToHadronic",       {false, 0.000214351205}},
            {"TTTo2L2Nu",          {false, 0.0004761561474}},
            {"TTToSemiLeptonic",   {false, 0.0001455793461}},
            // MC: ttHH signal & rare
            {"ttHH",               {false, 0.00000109763773}},
            {"tt4b",               {false, 0.001292157441}},
            {"ttHtobb",            {false, 0.003125301546}},
            {"tttt",               {false, 0.00399317683}},
            {"tttW",               {false, 0.00008453854444}},
            {"ttWH",               {false, 0.000131699}},
            {"ttWW",               {false, 0.0004155131232}},
            {"ttWZ",               {false, 0.0002901229714}},
            {"ttZHto4b",           {false, 0.00000111247369}},
            {"ttZtobb",            {false, 0.006587820794}},
            {"ttZZto4b",           {false, 0.0000003824366722}},
            {"ttbb",               {false, 0.0005295497645}},
            // MC: QCD
            {"QCD_HT200to300",     {false, 1071.943332}},
            {"QCD_HT300to500",     {false, 243.1813795}},
            {"QCD_HT500to700",     {false, 20.77575731}},
            {"QCD_HT700to1000",    {false, 5.58692197}},
            {"QCD_HT1000to1500",   {false, 3.285809224}},
            {"QCD_HT1500to2000",   {false, 0.3658958167}},
            {"QCD_HT2000toInf",    {false, 0.1606282808}},
        };
        return registry;
    }

public:
    // Returns pointer to SampleInfo if found, nullptr if unknown sample
    static const SampleInfo* GetSampleInfo(const std::string& name) {
        const auto& reg = SampleRegistry();
        auto it = reg.find(name);
        return (it != reg.end()) ? &it->second : nullptr;
    }

    /// Return a list of all MC sample names in the registry
    static std::vector<std::string> GetMCSampleNames() {
        std::vector<std::string> names;
        for (const auto& [name, info] : SampleRegistry()) {
            if (!info.isData) names.push_back(name);
        }
        return names;
    }

    /// Return a list of Data sample names for hadronic analysis
    /// (BTagCSV + JetHT only; SingleMuon is used for trigger SF derivation)
    static std::vector<std::string> GetHadronicDataSampleNames() {
        std::vector<std::string> names;
        for (const auto& [name, info] : SampleRegistry()) {
            if (info.isData &&
                name.find("SingleMuon") == std::string::npos) {
                names.push_back(name);
            }
        }
        return names;
    }

    // ========================================================================
    // [Section 1-C] B-Tag Reweight Configuration
    //
    // Settings for b-tag shape correction SF (correctionlib deepJet_shape).
    // Reference: BTV Internal Wiki - "Recommendations for Shape Correction SFs"
    // ========================================================================

    // ── B-tag JSON paths (tried in order; first existing file wins) ──
    static inline const std::vector<std::string> btagJSONPaths = {
        "/cvmfs/cms-griddata.cern.ch/cat/metadata/BTV/"
        "Run2-2017-UL-NanoAODv9/latest/btagging.json.gz",
        "/Users/jhlee/correctionLib/corrections/"
        "jsonpog-integration/POG/BTV/2017_UL/btagging.json.gz"
    };

    // Correction name inside the b-tag JSON
    static inline const std::string btagCorrectionName = "deepJet_shape";

    // Output JSON for normalization reweight factors (correctionlib schema v2)
    // Written by exe_MakeJSON after all samples have been processed.
    // Loaded by downstream analysis code via correctionlib.
    static inline const std::string btagReweightJSON = "btagNormReweight.json";

    // Output file prefix for b-tag reweight ROOT files
    static inline const std::string btagOutPrefix = "bTagReweight_";

    // Maximum nJets bin index for normalization ratio histogram
    // (nJets above this value are clamped to this bin)
    static inline const int btagMaxNJetsBin = 25;

    // Maximum number of leading jets to produce per-jet histograms for
    static inline const int btagMaxJetsForPerJetHist = 12;

    // ── HT reweight axis ──
    // When true, normalization ratios are computed in 2D (nJets × HT).
    // When false, only 1D (nJets) is used.
    //
    // Toggle this flag and recompile + rerun to switch between 1D and 2D.
    // Both exe_BTagSF and exe_MakeJSON MUST be compiled with the same flag.
    static inline const bool useHTForReweight = true;

    // HT bin edges for the reweight axis (only used when useHTForReweight = true).
    // Defines (N-1) bins: [edge[0], edge[1]), [edge[1], edge[2]), ...
    // With flow="clamp": HT < first edge → bin 0, HT >= last edge → last bin.
    // All events have HT >= 500 after skimming, so first edge = 500.
    static inline const std::vector<double> reweightHT_edges = {
        //500.0, 600.0, 700.0, 800.0, 1000.0, 1200.0, 1500.0, 2000.0, 3000.0
        500.0, 550.0, 600.0, 700.0, 800.0, 1000.0, 1100.0, 1200.0, 1300.0, 1400.0, 1500.0, 1600.0, 1700.0, 1800.0, 2100.0, 2500.0
    };

    // ── B-tag shape SF systematic variations ──
    // Reference: BTV Wiki
    //   b/light jets: lf, hf, hfstats1/2, lfstats1/2, jes
    //   c jets:       cferr1, cferr2
    static inline const std::vector<std::string> btagSystematics = {
        "central",
        "up_lf",       "down_lf",
        "up_hf",       "down_hf",
        "up_hfstats1", "down_hfstats1",
        "up_hfstats2", "down_hfstats2",
        "up_lfstats1", "down_lfstats1",
        "up_lfstats2", "down_lfstats2",
        "up_cferr1",   "down_cferr1",
        "up_cferr2",   "down_cferr2",
        "up_jes",      "down_jes"
    };

    // ========================================================================
    // [Section 2] Analysis Flags
    // ========================================================================

    // Minimum number of jets required after skimming (invariant check)
    static inline const int minNJets = 6;

    // Enable verbose output during event loop
    static inline const bool verbose = false;

    // Progress report interval (number of events)
    static inline const Long64_t progressInterval = 10000000;

    // ========================================================================
    // [Section 3] Histogram Definitions
    //
    // All validation histogram binning is defined here.
    // Adjust nBins / Low / High as needed.
    // ========================================================================

    // ── nJets ──
    //   minNJets = 6  →  first bin starts at 5.5 so that nJets=6 lands in bin 1
    static inline const int    histNJets_nBins = 20;
    static inline const double histNJets_Low   = 5.5;
    static inline const double histNJets_High  = 25.5;

    // ── HT [GeV] ──
    static inline const int    histHT_nBins = 50;
    static inline const double histHT_Low   = 0.0;
    static inline const double histHT_High  = 2500.0;

    // ── nbJets (number of b-tagged jets) ──
    static inline const int    histNBJets_nBins = 10;
    static inline const double histNBJets_Low   = -0.5;
    static inline const double histNBJets_High  = 9.5;

    // ── Per-jet pT [GeV] ──
    static inline const int    histJetPt_nBins = 50;
    static inline const double histJetPt_Low   = 0.0;
    static inline const double histJetPt_High  = 600.0;

    // ── Per-jet b-tag discriminant (DeepJet) ──
    static inline const int    histBTag_nBins = 50;
    static inline const double histBTag_Low   = 0.0;
    static inline const double histBTag_High  = 1.0;

    // ========================================================================
    // [Section 4] Reweight Bin Utilities
    //
    // These functions compute the flat bin index for normalization ratio
    // storage and lookup.
    //
    // 1D mode (useHTForReweight = false):
    //   flatIndex = nJetsBin          (0 .. btagMaxNJetsBin)
    //   totalBins = btagMaxNJetsBin + 1
    //
    // 2D mode (useHTForReweight = true):
    //   flatIndex = nJetsBin * nHTBins + htBin
    //   totalBins = (btagMaxNJetsBin + 1) * nHTBins
    //
    // correctionlib convention: row-major, last axis (HT) varies fastest.
    // ========================================================================

    /// Number of HT bins (1 if HT axis is disabled)
    static int getNumHTBins() {
        return useHTForReweight
            ? static_cast<int>(reweightHT_edges.size()) - 1
            : 1;
    }

    /// Total number of flat reweight bins
    static int getTotalReweightBins() {
        return (btagMaxNJetsBin + 1) * getNumHTBins();
    }

    /// Find HT bin index using upper_bound (O(log N), clamped)
    /// Returns 0 if useHTForReweight is false.
    static int getHTBinIndex(double HT) {
        if (!useHTForReweight) return 0;
        const auto& e = reweightHT_edges;
        // upper_bound → first edge strictly greater than HT
        auto it = std::upper_bound(e.begin(), e.end(), HT);
        int bin = static_cast<int>(std::distance(e.begin(), it)) - 1;
        return std::clamp(bin, 0, static_cast<int>(e.size()) - 2);
    }

    /// Compute flat reweight bin index from (nJets, HT)
    static int getReweightFlatIndex(int nJets, double HT) {
        int nJetsBin = std::min(nJets, btagMaxNJetsBin);
        if (!useHTForReweight) return nJetsBin;
        int htBin = getHTBinIndex(HT);
        return nJetsBin * getNumHTBins() + htBin;
    }

    // ========================================================================
    // [Section 5] Debug Dump
    // ========================================================================
    static void Dump(std::ostream& os = std::cout) {

        os << "\n";
        os << "╔══════════════════════════════════════════════════════════════╗\n";
        os << "║          B-Tag Reweight Configuration                        ║\n";
        os << "╠══════════════════════════════════════════════════════════════╣\n";

        // Section 1: Paths
        os << "║ [Section 1] Path & I/O                                       ║\n";
        os << "╟──────────────────────────────────────────────────────────────╢\n";
        os << "  Input base dir : " << inputBaseDir << "\n";
        os << "  Tree path      : " << treePath << "\n";
        os << "  Trigger SF JSON: " << sfOutputJSON << "\n";

        // Section 1-C: B-Tag
        os << "╟──────────────────────────────────────────────────────────────╢\n";
        os << "║ [Section 1-C] B-Tag Reweight                                 ║\n";
        os << "╟──────────────────────────────────────────────────────────────╢\n";
        os << "  btagCorrectionName    : " << btagCorrectionName << "\n";
        os << "  btagReweightJSON      : " << btagReweightJSON << "\n";
        os << "  btagOutPrefix         : " << btagOutPrefix << "\n";
        os << "  btagMaxNJetsBin       : " << btagMaxNJetsBin << "\n";
        os << "  btagMaxJetsForPerJet  : " << btagMaxJetsForPerJetHist << "\n";
        os << "  btagSystematics       : " << btagSystematics.size() << " variations\n";
        os << "  btagJSONPaths         :\n";
        for (const auto& p : btagJSONPaths)
            os << "    " << p << "\n";

        // HT reweight
        os << "╟──────────────────────────────────────────────────────────────╢\n";
        os << "║ Reweight Axis                                                ║\n";
        os << "╟──────────────────────────────────────────────────────────────╢\n";
        os << "  useHTForReweight      : " << (useHTForReweight ? "TRUE (2D: nJets × HT)" : "FALSE (1D: nJets only)") << "\n";
        if (useHTForReweight) {
            os << "  reweightHT_edges      : [";
            for (size_t i = 0; i < reweightHT_edges.size(); ++i) {
                os << reweightHT_edges[i];
                if (i < reweightHT_edges.size() - 1) os << ", ";
            }
            os << "]\n";
            os << "  nHTBins               : " << getNumHTBins() << "\n";
        }
        os << "  totalReweightBins     : " << getTotalReweightBins() << "\n";

        // Section 2: Flags
        os << "╟──────────────────────────────────────────────────────────────╢\n";
        os << "║ [Section 2] Analysis Flags                                   ║\n";
        os << "╟──────────────────────────────────────────────────────────────╢\n";
        os << "  minNJets         : " << minNJets << "\n";
        os << "  verbose          : " << (verbose ? "true" : "false") << "\n";
        os << "  progressInterval : " << progressInterval << "\n";

        // Section 3: Histograms
        os << "╟──────────────────────────────────────────────────────────────╢\n";
        os << "║ [Section 3] Histogram Definitions                            ║\n";
        os << "╟──────────────────────────────────────────────────────────────╢\n";
        os << "  nJets  : " << histNJets_nBins
           << " bins [" << histNJets_Low << ", " << histNJets_High << "]\n";
        os << "  HT     : " << histHT_nBins
           << " bins [" << histHT_Low << ", " << histHT_High << "] GeV\n";
        os << "  nbJets : " << histNBJets_nBins
           << " bins [" << histNBJets_Low << ", " << histNBJets_High << "]\n";
        os << "  jetPt  : " << histJetPt_nBins
           << " bins [" << histJetPt_Low << ", " << histJetPt_High << "] GeV\n";
        os << "  bTag   : " << histBTag_nBins
           << " bins [" << histBTag_Low << ", " << histBTag_High << "]\n";

        os << "╚══════════════════════════════════════════════════════════════╝\n\n";


    }
};

#endif

