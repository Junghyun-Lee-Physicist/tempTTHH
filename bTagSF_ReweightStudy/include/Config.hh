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

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <sstream>

#include "Config_TtCatGroup.hh"

// [2026-07-29] sample weight / Data era 는 analyzer 와 같은 단일 소스에서 온다.
//   TriggerStudy 와 **같은 헤더**를 본다 (Makefile 의 SHARED_INC = ../include).
#include "SampleRegistry.hh"

// [2026-07-29] offline selection 상수도 analyzer 와 같은 헤더를 본다.
//   reweight 는 main 에 곱해질 보정이므로 측정 영역이 main 의 baseline 과
//   같아야 한다. 사본을 두면 그 동기화가 사람의 기억에 의존하게 된다.
#include "SelectionCuts.h"


// ============================================================================
// Config: Central configuration manager
// ============================================================================
class Config {
public:
    // ========================================================================
    // [Section 1] Path & I/O Settings
    // ========================================================================

    // ── Base directory containing input (btagtrig skim) ntuple files ────────
    //   [2026-07-29] 하드코딩 절대경로 → `TTHH_SKIM_DIR` override.
    //   TriggerStudy/include/Config.hh 의 같은 함수와 **같은 env 이름**을 쓴다.
    //   두 도구가 서로 다른 skim 을 읽으면 trigger SF 와 b-tag reweight 가
    //   다른 위상공간에서 유도되는데, 그건 어디서도 검출되지 않는다.
    static const std::string& InputBaseDir() {
        static const std::string dir = [] {
            const char* env = std::getenv("TTHH_SKIM_DIR");
            std::string d = (env && *env)
                ? std::string(env)
                : std::string("/Users/jhlee/ttHH/ntuple/skimmed/gen_tier3/");
            if (!d.empty() && d.back() != '/') d += '/';
            return d;
        }();
        return dir;
    }

    static bool SkimDirFromEnv() {
        const char* env = std::getenv("TTHH_SKIM_DIR");
        return env && *env;
    }

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

    //   [2026-07-29] `TTHH_TRIGSF_JSON` 로 override 가능.
    //   trigger SF 는 이 도구의 **입력**이므로, 방금 재유도한 파일을 가리키게
    //   하는 일이 잦다 (기본값은 예전에 복사해 둔 파일일 수 있다).
    static const std::string& TriggerSFJSON() {
        static const std::string p = [] {
            const char* env = std::getenv("TTHH_TRIGSF_JSON");
            return (env && *env) ? std::string(env)
                                 : std::string("TriggerSF/trigger_sf.json.gz");
        }();
        return p;
    }

    // ========================================================================
    // [Section 1-B] Sample Registry  →  ../include/SampleRegistry.hh 로 이관
    //
    // [2026-07-29] 여기 있던 하드코딩 표를 제거했다. TriggerStudy/include/Config.hh
    //   에 있던 것과 **글자 그대로 같은 복사본**이었고, 둘 다 STEP18 이전
    //   dataset 의 Σgenw 로 계산된 값이었다.
    //
    //   b-tag norm reweight 는 process group 별 Σω_noSF / Σω_withSF 다.
    //   한 group 에 여러 샘플이 합쳐지므로 샘플별 weight 의 **상대 비중**이
    //   그대로 비율에 들어간다. 즉 틀린 Σgenw 는 lumi 처럼 상쇄되지 않는다.
    //
    //   이제 `SampleRegistry::get()` 이 analyzer 제출기와 같은 두 파일에서 같은
    //   공식으로 런타임 합성한다. 모르는 샘플·prescan 없는 샘플은 FATAL.
    // ========================================================================

    using SampleInfo = ::SampleRegistry::Info;

public:
    /// 샘플 정보. 모르는 샘플이면 SampleRegistry 안에서 FATAL 이므로 nullptr 없음.
    static const SampleInfo* GetSampleInfo(const std::string& name) {
        return &::SampleRegistry::get(name);
    }

    /// xsec_db 에 등록된 MC 샘플 전체 (campaign 이름).
    static std::vector<std::string> GetMCSampleNames() {
        return ::SampleRegistry::mcSampleNames();
    }

    /// Hadronic 분석용 Data 샘플 (BTagCSV + JetHT).
    /// SingleMuon 은 trigger SF 유도 전용이라 제외한다.
    static std::vector<std::string> GetHadronicDataSampleNames() {
        return ::SampleRegistry::dataSampleNames({"SingleMuon"});
    }

    // ========================================================================
    // [Section 1-C] B-Tag Reweight Configuration
    //
    // Settings for b-tag shape correction SF (correctionlib deepJet_shape).
    // Reference: BTV Internal Wiki - "Recommendations for Shape Correction SFs"
    // ========================================================================

    // ── B-tag JSON paths (tried in order; first existing file wins) ──
    //   [2026-07-29] `TTHH_BTAG_JSON` 가 설정돼 있으면 **그것만** 쓴다.
    //   fallback 체인은 편하지만, 의도한 파일이 없을 때 조용히 다른 파일로
    //   넘어가므로 연도/campaign 이 어긋난 SF 를 쓰게 될 수 있다. env 를 준
    //   경우엔 그 파일이 없으면 그냥 실패하는 편이 낫다.
    static const std::vector<std::string>& BTagJSONPaths() {
        static const std::vector<std::string> paths = [] {
            const char* env = std::getenv("TTHH_BTAG_JSON");
            if (env && *env) return std::vector<std::string>{ std::string(env) };
            return std::vector<std::string>{
                "/cvmfs/cms-griddata.cern.ch/cat/metadata/BTV/"
                "Run2-2017-UL-NanoAODv9/latest/btagging.json.gz",
                "/cvmfs/cms.cern.ch/rsync/cms-nanoAOD/jsonpog-integration/"
                "POG/BTV/2017_UL/btagging.json.gz",
                "/Users/jhlee/correctionLib/corrections/"
                "jsonpog-integration/POG/BTV/2017_UL/btagging.json.gz"
            };
        }();
        return paths;
    }

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
    //   [2026-07-29] 값을 여기 적지 않고 analyzer 의 SelectionCuts.h 를 그대로 쓴다.
    static inline const int minNJets = Cuts::nJets;

    // ── [2026-07-29] 측정 영역의 lepton 조건 ──────────────────────────────
    //
    //   btagtrig skim 은 **두 종류**의 이벤트를 함께 담는다:
    //     - FH 이벤트 (lead muon 없음)            → main 분석의 위상공간
    //     - muon control 이벤트 (lead muon 있음)  → trigger SF 측정용
    //   analyzer 가 btagtrig 에서 lepton veto 를 강제하지 않기 때문이다
    //   (ttHHanalyzer_unified.cc kCutSequence 6번: kSelBitMainLike).
    //
    //   b-tag norm reweight 는 **main 에 곱해질 보정**이므로 main 과 같은
    //   위상공간(FH, lepton veto)에서 유도해야 한다. 이전 코드에는 lepton
    //   조건이 아예 없어서 두 종류를 섞어 유도하고 있었고, 그건 어떤 경고도
    //   내지 않는다.
    //
    //   requireLeptonVeto = true  : nVetoLeptons == 0  (FH — main 과 동일) ← 기본
    //   requireLeptonVeto = false : 컷 없음 (구 동작 재현용; 권장하지 않음)
    //
    //   ⚠ nMuons==0 로 대신하면 안 된다. nMuons 는 lead-muon gate 를 통과한
    //     이벤트에서만 채워지므로, soft(pT 15~29) lepton 이 있는 이벤트가
    //     nMuons==0 으로 살아남는다. main 은 그런 이벤트를 veto 한다.
    static inline const bool requireLeptonVeto = true;

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
        os << "  Input base dir : " << InputBaseDir()
           << (SkimDirFromEnv() ? "   [TTHH_SKIM_DIR]" : "   [built-in default]") << "\n";
        os << "  Tree path      : " << treePath << "\n";
        os << "  Trigger SF JSON: " << TriggerSFJSON() << "\n";

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
        for (const auto& p : BTagJSONPaths())
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

