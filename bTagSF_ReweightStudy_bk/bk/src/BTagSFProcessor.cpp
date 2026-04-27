#define BTagSFProcessor_cxx
#include "BTagSFProcessor.hh"
#include "Config.hh"

#include <TH2.h>
#include <TStyle.h>

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <iomanip>

namespace fs = std::filesystem;

// ============================================================================
// Helper: Fatal invariant violation (same pattern as EventLooper)
// ============================================================================
static inline void FATAL(const char* msg, Long64_t entry) {
    std::cerr << "\n[BTagSF][FATAL] entry=" << entry
              << " : " << msg << std::endl;
    std::cerr << "  -> Skimming invariant violated. Exiting.\n" << std::endl;
    std::exit(1);
}

// ============================================================================
// Constructor & Destructor
// ============================================================================
BTagSFProcessor::BTagSFProcessor(int mode) : processingMode(mode) {}

BTagSFProcessor::~BTagSFProcessor() {
    delete reader;
    reader = nullptr;

    if (normFile) {
        normFile->Close();
        delete normFile;
        normFile = nullptr;
    }

    if (inputFile) {
        inputFile->Close();
        delete inputFile;
        inputFile = nullptr;
    }
}

// ============================================================================
// File Name Helpers
// ============================================================================
void BTagSFProcessor::setNtupleName(TString name) { ntupleName = name; }

TString BTagSFProcessor::getInputName() const {
    return ntupleName + ".root";
}

TString BTagSFProcessor::getOutputName() const {
    const std::string& prefix = (processingMode == 0)
        ? BTagConfig::normOutputPrefix
        : BTagConfig::reweightOutputPrefix;
    return TString(prefix) + ntupleName + ".root";
}

// ============================================================================
// Sample Identification
//
// Determines Data/MC status, MC normalization weight, and era.
// Follows the same convention as EventLooper (hardcoded sample names).
// TODO: Replace with SampleWeightRegistry once available.
// ============================================================================
void BTagSFProcessor::identifySample(const TString& sampleName) {
    isData    = true;
    MC_weight = 1.0;
    dataSet   = "default";
    era       = "default";

    // --- MC samples: (cross section × luminosity) / Σ(genEventSumw) ---
    if (sampleName == "TTTo2L2Nu") {
        isData    = false;
        MC_weight = 0.0004761561474;
    }
    else if (sampleName == "TTToHadronic") {
        isData    = false;
        MC_weight = 0.000214351205;
    }
    else if (sampleName == "TTToSemiLeptonic") {
        isData    = false;
        MC_weight = 0.0001455793461;
    }
    else if (sampleName == "TTbarInc") {
        isData    = false;
        MC_weight = 0.0001455793461;   // 필요시 수정
    }

    // --- Data: parse "<DataSet>_<Era>" ---
    if (isData) {
        Ssiz_t underscorePos = sampleName.Index("_");
        if (underscorePos == kNPOS) {
            std::cerr << "[BTagSF][ERROR] Data sample must contain '_': "
                      << sampleName << std::endl;
            std::exit(1);
        }
        dataSet = sampleName(0, underscorePos);
        era     = sampleName(underscorePos + 1,
                             sampleName.Length() - underscorePos - 1);
    }

    std::cout << "  Sample     : " << sampleName << "\n"
              << "  Type       : " << (isData ? "Data" : "MC") << "\n";
    if (!isData) std::cout << "  MC_weight  : " << MC_weight << "\n";
    if (isData)  std::cout << "  DataSet/Era: " << dataSet << " / " << era << "\n";
    std::cout << std::endl;
}

// ============================================================================
// Init
//
// Phase 1: Open ROOT file, retrieve TTree
// Phase 2: Create NtupleReader + set up additional b-tag branches
// Phase 3: Load correctionlib (b-tag SF + trigger SF)
// Phase 4: [Mode 1] Load normalization ratios from mode 0 output
// ============================================================================
void BTagSFProcessor::Init() {

    // ────────────────────────────────────────────────────────────────────────
    // Phase 1: Open input ROOT file and retrieve TTree
    // ────────────────────────────────────────────────────────────────────────
    TString ntuplePath = Config::inputBaseDir + getInputName();
    std::cout << "Opening: " << ntuplePath << std::endl;

    inputFile = TFile::Open(ntuplePath);
    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "[BTagSF][ERROR] Cannot open " << ntuplePath << std::endl;
        std::exit(1);
    }

    inputFile->GetObject(Config::treePath.c_str(), fChain);
    if (!fChain) {
        std::cerr << "[BTagSF][ERROR] Cannot get TTree: "
                  << Config::treePath << std::endl;
        std::exit(1);
    }

    // ────────────────────────────────────────────────────────────────────────
    // Phase 2: NtupleReader (common branches) + additional b-tag branches
    //
    // NtupleReader handles: triggers, nJets, HT, jetPt, jetEta, weights, etc.
    // We add: bTagScore (discriminant), hadFlavs (hadron flavour)
    // Both read from the SAME TTree, so one GetEntry() loads everything.
    // ────────────────────────────────────────────────────────────────────────
    reader = new NtupleReader(fChain);

    // b-tag 판별값 (discriminant) — DeepJet output score [0, 1]
    bTagScore = nullptr;
    fChain->SetBranchAddress("bTagScore", &bTagScore, &b_bTagScore);

    // 하드론 맛 (hadron flavour) — 0: light/gluon, 4: charm, 5: bottom
    hadronFlavour = nullptr;
    fChain->SetBranchAddress("hadFlavs", &hadronFlavour, &b_hadronFlavour);

    // ────────────────────────────────────────────────────────────────────────
    // Phase 3: Load correctionlib providers
    // ────────────────────────────────────────────────────────────────────────

    // --- B-tag shape SF ---
    // 여러 경로를 순서대로 시도 (CVMFS → 로컬)
    std::string btagJsonUsed;
    for (const auto& path : BTagConfig::btagJsonPaths) {
        if (fs::exists(path)) {
            btagJsonUsed = path;
            break;
        }
    }
    if (btagJsonUsed.empty()) {
        std::cerr << "[BTagSF][ERROR] No b-tag JSON found. Tried:\n";
        for (const auto& p : BTagConfig::btagJsonPaths)
            std::cerr << "  - " << p << "\n";
        std::exit(1);
    }

    std::cout << "Loading b-tag SF JSON: " << btagJsonUsed << std::endl;
    try {
        btagCorrSet   = correction::CorrectionSet::from_file(btagJsonUsed);
        btagSFProvider = btagCorrSet->at(BTagConfig::btagCorrectionKey);
    } catch (const std::exception& e) {
        std::cerr << "[BTagSF][ERROR] Failed to load b-tag SF: "
                  << e.what() << std::endl;
        std::exit(1);
    }

    // b-tag SF 입력 변수 확인 (디버깅용 출력)
    std::cout << "  B-tag SF inputs: ";
    for (const auto& var : btagSFProvider->inputs()) {
        std::cout << var.name() << " ";
    }
    std::cout << std::endl;

    // --- Trigger SF ---
    if (fs::exists(BTagConfig::triggerSFJsonPath)) {
        std::cout << "Loading trigger SF JSON: "
                  << BTagConfig::triggerSFJsonPath << std::endl;
        try {
            trigCorrSet   = correction::CorrectionSet::from_file(
                                BTagConfig::triggerSFJsonPath);
            trigSFProvider = trigCorrSet->at(BTagConfig::triggerSFCorrectionKey);
        } catch (const std::exception& e) {
            std::cerr << "[BTagSF][WARN] Failed to load trigger SF: "
                      << e.what() << "\n"
                      << "  → Trigger SF will default to 1.0\n";
            trigSFProvider = nullptr;
        }
    } else {
        std::cerr << "[BTagSF][WARN] Trigger SF JSON not found: "
                  << BTagConfig::triggerSFJsonPath << "\n"
                  << "  → Trigger SF will default to 1.0\n";
    }

    // ────────────────────────────────────────────────────────────────────────
    // Phase 4: [Mode 1] Load normalization ratios from mode 0 output
    // ────────────────────────────────────────────────────────────────────────
    if (processingMode == 1) {
        TString normPath = TString(BTagConfig::normOutputPrefix)
                         + ntupleName + ".root";
        std::cout << "Loading normalization ratios: " << normPath << std::endl;

        normFile = TFile::Open(normPath, "READ");
        if (!normFile || normFile->IsZombie()) {
            std::cerr << "[BTagSF][ERROR] Cannot open normalization file: "
                      << normPath << "\n"
                      << "  → Run mode 0 first!\n";
            std::exit(1);
        }

        normRatioHist = dynamic_cast<TH1D*>(normFile->Get("h_nJets_normRatio"));
        if (!normRatioHist) {
            std::cerr << "[BTagSF][ERROR] h_nJets_normRatio not found in "
                      << normPath << std::endl;
            std::exit(1);
        }

        // 메모리에 복사하여 normFile을 닫아도 안전하게 사용
        normRatioHist = dynamic_cast<TH1D*>(
            normRatioHist->Clone("h_normRatio_local"));
        normRatioHist->SetDirectory(nullptr);
        normFile->Close();
        delete normFile;
        normFile = nullptr;

        std::cout << "  Normalization ratio histogram loaded ("
                  << normRatioHist->GetNbinsX() << " bins)\n" << std::endl;
    }

    std::cout << "[BTagSF] Initialization complete.\n" << std::endl;
}


// ============================================================================
// computeBTagEventWeight
//
// BTV Wiki: ω_event = Π_{i}^{N_jets} SF(D_i, pT_i, η_i)
//
// 모든 선택된 제트(selected jet)에 대해 per-jet SF를 곱하여
// 이벤트 단위(event-level) b-tag weight를 계산한다.
//
// 입력 순서는 correctionlib JSON 스키마에 의해 결정되며,
// deepJet_shape의 일반적 순서: systematic, flavor, abseta, pt, discriminant
// ============================================================================
double BTagSFProcessor::computeBTagEventWeight(
    const std::string& systematic) const
{
    if (isData || !btagSFProvider) return 1.0;

    const int nJets = reader->GetNJets();

    // 안전 검사: 벡터 크기 일치 확인
    if (!bTagScore || !hadronFlavour ||
        static_cast<int>(bTagScore->size()) != nJets ||
        static_cast<int>(hadronFlavour->size()) != nJets) {
        return 1.0;
    }

    double eventWeight = 1.0;

    for (int iJet = 0; iJet < nJets; ++iJet) {
        const int    flavor       = hadronFlavour->at(iJet);
        const double absEta       = std::abs(reader->GetJetEta().at(iJet));
        const double pt           = reader->GetJetPt().at(iJet);
        const double discriminant = bTagScore->at(iJet);

        // BTV systematic 규칙:
        //   - b/light 제트: "central", "up_lf", "down_hf", "up_hfstats1", etc.
        //   - c 제트: "central", "up_cferr1", "down_cferr2" ONLY
        //
        // "central"일 때는 모든 맛(flavor)에 동일하게 적용한다.
        // Systematic variation일 때는 flavor에 따라 분기가 필요하다.
        // (현재 구현은 central만 지원; systematic 확장 시 여기에 분기 추가)

        try {
            double jetSF = btagSFProvider->evaluate({
                systematic,                         // systematic variation
                static_cast<int>(flavor),           // hadron flavour (0, 4, 5)
                absEta,                             // |η|
                pt,                                 // pT [GeV]
                discriminant                        // b-tag score [0, 1]
            });
            eventWeight *= jetSF;
        } catch (const std::exception& e) {
            // correctionlib 평가 실패 시 해당 제트의 SF를 1.0으로 처리
            // 첫 10건만 경고 출력 (로그 폭주 방지)
            static int warnCount = 0;
            if (warnCount < 10) {
                std::cerr << "[BTagSF][WARN] b-tag SF evaluation failed: "
                          << e.what() << "\n"
                          << "  jet " << iJet
                          << " | flavor=" << flavor
                          << " abseta=" << absEta
                          << " pt=" << pt
                          << " disc=" << discriminant << "\n";
                ++warnCount;
            }
            // SF = 1.0 for this jet (no correction)
        }
    }

    return eventWeight;
}


// ============================================================================
// getTriggerSF
//
// 우리 파이프라인에서 도출한 trigger SF를 correctionlib JSON에서 평가한다.
// 입력 순서: nbJets, eta, ht, pt (EventLooper와 동일)
// Data이거나 provider가 없으면 1.0을 반환한다.
// ============================================================================
double BTagSFProcessor::getTriggerSF(
    int nbJets, double jet6Eta, double ht, double jet6Pt) const
{
    if (isData || !trigSFProvider) return 1.0;

    try {
        return trigSFProvider->evaluate({
            static_cast<int>(nbJets),
            jet6Eta,
            ht,
            jet6Pt
        });
    } catch (const std::exception& e) {
        static int warnCount = 0;
        if (warnCount < 10) {
            std::cerr << "[BTagSF][WARN] Trigger SF evaluation failed: "
                      << e.what() << "\n";
            ++warnCount;
        }
        return 1.0;
    }
}


// ============================================================================
// getNormRatio
//
// BTV Wiki: r = Σω_before / Σω_after  (per nJets bin)
// mode 0에서 계산한 ratio 히스토그램에서 해당 bin 값을 반환한다.
// ============================================================================
double BTagSFProcessor::getNormRatio(int nJets) const {
    if (!normRatioHist) return 1.0;

    // 히스토그램 범위 밖이면 가장 가까운 유효 bin으로 clamp
    int bin = normRatioHist->FindBin(static_cast<double>(nJets));
    bin = std::max(1, std::min(bin, normRatioHist->GetNbinsX()));

    double ratio = normRatioHist->GetBinContent(bin);

    // ratio가 비정상적이면 (0 이하 또는 음수) 보정 없음
    if (ratio <= 0.0 || std::isnan(ratio) || std::isinf(ratio)) {
        return 1.0;
    }
    return ratio;
}


// ============================================================================
// Loop
//
// 메인 이벤트 루프 (Main Event Loop)
//
// Mode 0: 정규화 비율 계산 (Normalization Ratio Computation)
//   - b-tag SF 적용 전/후의 이벤트 가중치 합(sum of weights)을 nJets bin별로 누적
//   - ratio = Σ(base weight) / Σ(base weight × btagSF)
//
// Mode 1: b-tag SF + trigger SF + 정규화 적용, 검증 히스토그램 생성
//   - noSF:        base weight × trigSF
//   - withBTagSF:  base weight × trigSF × btagSF
//   - reweighted:  base weight × trigSF × btagSF × normRatio
// ============================================================================
void BTagSFProcessor::Loop() {
    if (!fChain) return;

    // ════════════════════════════════════════════════════════════════════════
    // Phase 1: Sample Identification
    // ════════════════════════════════════════════════════════════════════════
    TString sampleName = getInputName();
    sampleName.ReplaceAll(".root", "");

    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  BTagSFProcessor :: Mode " << processingMode
              << (processingMode == 0 ? " (Normalization)" : " (Reweight)")
              << "                       ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";

    identifySample(sampleName);

    // 가중치 오류 합산을 위해 Sumw2 전역 활성화
    TH1::SetDefaultSumw2(true);

    // ════════════════════════════════════════════════════════════════════════
    // Phase 2: Create Output File & Histograms
    // ════════════════════════════════════════════════════════════════════════
    TFile* outputFile = new TFile(getOutputName(), "RECREATE");
    outputFile->cd();

    // ────────────────────────────────────────────────────────────────────────
    // Mode 0: 정규화를 위한 nJets 히스토그램 (weight sum before / after b-tag SF)
    // ────────────────────────────────────────────────────────────────────────
    TH1D* h_nJets_sumBefore = nullptr;   // Σ(baseWeight × trigSF)
    TH1D* h_nJets_sumAfter  = nullptr;   // Σ(baseWeight × trigSF × btagSF)

    if (processingMode == 0) {
        h_nJets_sumBefore = new TH1D(
            "h_nJets_sumBefore",
            "Sum of weights before b-tag SF;nJets;#Sigma w",
            BTagConfig::nJetsNBins,
            BTagConfig::nJetsBinMin,
            BTagConfig::nJetsBinMax);

        h_nJets_sumAfter = new TH1D(
            "h_nJets_sumAfter",
            "Sum of weights after b-tag SF;nJets;#Sigma w",
            BTagConfig::nJetsNBins,
            BTagConfig::nJetsBinMin,
            BTagConfig::nJetsBinMax);
    }

    // ────────────────────────────────────────────────────────────────────────
    // Mode 1: 검증 히스토그램 (Validation Histograms)
    //   세 가지 가중치 레벨로 동일 변수를 채운다:
    //     (a) noSF      : base × trigSF           (b-tag 보정 없음)
    //     (b) withBTagSF: base × trigSF × btagSF  (정규화 없음)
    //     (c) reweighted: base × trigSF × btagSF × normRatio (최종)
    // ────────────────────────────────────────────────────────────────────────

    // 이벤트 레벨 (Event-level)
    TH1D* h_nJets_noSF      = nullptr;
    TH1D* h_nJets_withSF    = nullptr;
    TH1D* h_nJets_reweight  = nullptr;
    TH1D* h_nbJets_noSF     = nullptr;
    TH1D* h_nbJets_withSF   = nullptr;
    TH1D* h_nbJets_reweight = nullptr;
    TH1D* h_HT_noSF         = nullptr;
    TH1D* h_HT_withSF       = nullptr;
    TH1D* h_HT_reweight     = nullptr;

    // 제트별 히스토그램 벡터 (Per-jet vectors)
    std::vector<TH1D*> v_jetPt_noSF,  v_jetPt_withSF,  v_jetPt_reweight;
    std::vector<TH1D*> v_jetEta_noSF, v_jetEta_withSF, v_jetEta_reweight;
    std::vector<TH1D*> v_bTag_noSF,   v_bTag_withSF,   v_bTag_reweight;

    if (processingMode == 1) {
        // --- 이벤트 레벨 ---
        h_nJets_noSF = new TH1D("h_nJets_noSF",
            "nJets (no b-tag SF);nJets;Events",
            BTagConfig::nJetsNBins, BTagConfig::nJetsBinMin, BTagConfig::nJetsBinMax);
        h_nJets_withSF = new TH1D("h_nJets_withSF",
            "nJets (with b-tag SF);nJets;Events",
            BTagConfig::nJetsNBins, BTagConfig::nJetsBinMin, BTagConfig::nJetsBinMax);
        h_nJets_reweight = new TH1D("h_nJets_reweight",
            "nJets (b-tag SF + norm);nJets;Events",
            BTagConfig::nJetsNBins, BTagConfig::nJetsBinMin, BTagConfig::nJetsBinMax);

        h_nbJets_noSF = new TH1D("h_nbJets_noSF",
            "nb-jets (no b-tag SF);nb-jets;Events", 10, 0, 10);
        h_nbJets_withSF = new TH1D("h_nbJets_withSF",
            "nb-jets (with b-tag SF);nb-jets;Events", 10, 0, 10);
        h_nbJets_reweight = new TH1D("h_nbJets_reweight",
            "nb-jets (b-tag SF + norm);nb-jets;Events", 10, 0, 10);

        h_HT_noSF = new TH1D("h_HT_noSF",
            "HT (no b-tag SF);HT [GeV];Events", 50, 0, 2500);
        h_HT_withSF = new TH1D("h_HT_withSF",
            "HT (with b-tag SF);HT [GeV];Events", 50, 0, 2500);
        h_HT_reweight = new TH1D("h_HT_reweight",
            "HT (b-tag SF + norm);HT [GeV];Events", 50, 0, 2500);

        // --- 제트 레벨 (leading ~ maxJetsForHist) ---
        const int maxJ = BTagConfig::maxJetsForHist;
        v_jetPt_noSF.resize(maxJ);   v_jetPt_withSF.resize(maxJ);   v_jetPt_reweight.resize(maxJ);
        v_jetEta_noSF.resize(maxJ);  v_jetEta_withSF.resize(maxJ);  v_jetEta_reweight.resize(maxJ);
        v_bTag_noSF.resize(maxJ);    v_bTag_withSF.resize(maxJ);    v_bTag_reweight.resize(maxJ);

        for (int j = 0; j < maxJ; ++j) {
            v_jetPt_noSF[j] = new TH1D(
                Form("h_jetPt_noSF_jet%d", j),
                Form("Jet %d p_{T} (no b-tag SF);p_{T} [GeV];Events", j),
                50, 0, 500);
            v_jetPt_withSF[j] = new TH1D(
                Form("h_jetPt_withSF_jet%d", j),
                Form("Jet %d p_{T} (with b-tag SF);p_{T} [GeV];Events", j),
                50, 0, 500);
            v_jetPt_reweight[j] = new TH1D(
                Form("h_jetPt_reweight_jet%d", j),
                Form("Jet %d p_{T} (b-tag SF + norm);p_{T} [GeV];Events", j),
                50, 0, 500);

            v_jetEta_noSF[j] = new TH1D(
                Form("h_jetEta_noSF_jet%d", j),
                Form("Jet %d #eta (no b-tag SF);#eta;Events", j),
                50, -2.5, 2.5);
            v_jetEta_withSF[j] = new TH1D(
                Form("h_jetEta_withSF_jet%d", j),
                Form("Jet %d #eta (with b-tag SF);#eta;Events", j),
                50, -2.5, 2.5);
            v_jetEta_reweight[j] = new TH1D(
                Form("h_jetEta_reweight_jet%d", j),
                Form("Jet %d #eta (b-tag SF + norm);#eta;Events", j),
                50, -2.5, 2.5);

            v_bTag_noSF[j] = new TH1D(
                Form("h_bTag_noSF_jet%d", j),
                Form("Jet %d b-tag score (no b-tag SF);DeepJet;Events", j),
                50, 0, 1.0);
            v_bTag_withSF[j] = new TH1D(
                Form("h_bTag_withSF_jet%d", j),
                Form("Jet %d b-tag score (with b-tag SF);DeepJet;Events", j),
                50, 0, 1.0);
            v_bTag_reweight[j] = new TH1D(
                Form("h_bTag_reweight_jet%d", j),
                Form("Jet %d b-tag score (b-tag SF + norm);DeepJet;Events", j),
                50, 0, 1.0);
        }
    }

    // ════════════════════════════════════════════════════════════════════════
    // Phase 3: Main Event Loop
    // ════════════════════════════════════════════════════════════════════════
    const Long64_t nEntries = fChain->GetEntries();
    std::cout << ">>> Starting event loop: " << nEntries << " entries\n";

    // 디버깅 통계
    long long nProcessed   = 0;
    long long nPassTrigger = 0;

    for (Long64_t jentry = 0; jentry < nEntries; ++jentry) {
        reader->GetEntry(jentry);  // NtupleReader + 추가 branch 모두 로드

        // 진행 상황 보고 (Progress reporting)
        if (jentry % Config::progressInterval == 0) {
            std::cout << "    Processing: " << jentry << " / " << nEntries
                      << " (" << std::fixed << std::setprecision(1)
                      << (100.0 * jentry / nEntries) << "%)\n";
        }

        // ════════════════════════════════════════════════════════════════
        // Skimming Invariant Checks
        // ════════════════════════════════════════════════════════════════
        const int nJets = reader->GetNJets();
        if (nJets < 6) FATAL("nJets < 6", jentry);

        const double currentHT = reader->GetHT();
        if (currentHT < 500.0) FATAL("HT < 500 GeV", jentry);

        const double jet6Pt = reader->GetJetPt().at(5);
        if (jet6Pt <= 40.0) FATAL("6th jet pT <= 40 GeV", jentry);

        if (!reader->GetPassMETFilters()) FATAL("MET filters failed", jentry);

        if (isData && reader->GetFailGoldenJson())
            FATAL("Golden JSON failed for Data", jentry);

        // bTagScore / hadronFlavour 벡터 크기 검증
        if (!bTagScore || !hadronFlavour ||
            static_cast<int>(bTagScore->size()) != nJets ||
            static_cast<int>(hadronFlavour->size()) != nJets) {
            FATAL("bTagScore or hadFlavs size mismatch with nJets", jentry);
        }

        // ════════════════════════════════════════════════════════════════
        // Trigger Logic (hadronic triggers)
        //
        // b-tag SF는 분석 위상 공간(analysis phase space)에서 적용되므로
        // hadronic trigger를 통과한 이벤트만 선택한다.
        // (EventLooper의 muon reference trigger와는 다른 로직)
        // ════════════════════════════════════════════════════════════════
        const bool isEraB = (era == "B");

        const bool fired6J1T = isEraB ? reader->GetPassTrigger_6J1T_B()
                                      : reader->GetPassTrigger_6J1T_CDEF();
        const bool fired6J2T = isEraB ? reader->GetPassTrigger_6J2T_B()
                                      : reader->GetPassTrigger_6J2T_CDEF();
        const bool fired4J3T = isEraB ? reader->GetPassTrigger_4J3T_B()
                                      : reader->GetPassTrigger_4J3T_CDEF();
        const bool firedHT   = reader->GetPassTrigger_PFHT1050();

        const bool any6J     = fired6J1T || fired6J2T;
        const bool passORHad = fired4J3T || any6J || firedHT;

        // Data PD-exclusive rule (이중 계수 방지)
        bool pdExclusiveOK = true;
        if (isData) {
            if (dataSet == "BTagCSV") {
                pdExclusiveOK = fired4J3T;
            } else if (dataSet == "JetHT") {
                pdExclusiveOK = any6J || (firedHT && !any6J && !fired4J3T);
            } else {
                std::cerr << "[BTagSF][ERROR] Unknown dataset: "
                          << dataSet << std::endl;
                std::exit(1);
            }
        }

        const bool passHadTrig = isData ? (passORHad && pdExclusiveOK)
                                        : passORHad;

        if (!passHadTrig) continue;
        ++nPassTrigger;

        // ════════════════════════════════════════════════════════════════
        // Compute Event Weights
        // ════════════════════════════════════════════════════════════════

        // --- Base weight (b-tag SF 제외, trigger SF 포함) ---
        double baseWeight = 1.0;
        if (!isData) {
            baseWeight = reader->GetGenWeight()
                       * reader->GetPUWeight()
                       * reader->GetPrefireWeight()
                       * MC_weight;
        }

        // --- Trigger SF ---
        const int    nbJets  = reader->GetNBJets();
        const double jet6Eta = reader->GetJetEta().at(5);
        const double trigSF  = getTriggerSF(nbJets, jet6Eta, currentHT, jet6Pt);

        // Base weight에 trigger SF를 포함 (이것이 "b-tag 이전" 기준 가중치)
        const double weightBeforeBTag = baseWeight * trigSF;

        // --- B-tag event weight ---
        // BTV Wiki: ω_event = Π_i SF(D_i, pT_i, η_i)
        const double btagEventWeight = computeBTagEventWeight(
                                            BTagConfig::defaultSystematic);

        const double weightAfterBTag = weightBeforeBTag * btagEventWeight;

        // ════════════════════════════════════════════════════════════════
        // Mode-Specific Logic
        // ════════════════════════════════════════════════════════════════

        if (processingMode == 0) {
            // ──────────────────────────────────────────────────────────
            // Mode 0: Accumulate sums for normalization ratio
            //
            // BTV Wiki:
            //   r = Σ(weight before b-tag SF) / Σ(weight after b-tag SF)
            //   measured per nJets bin
            // ──────────────────────────────────────────────────────────
            h_nJets_sumBefore->Fill(nJets, weightBeforeBTag);
            h_nJets_sumAfter->Fill(nJets,  weightAfterBTag);

        } else {
            // ──────────────────────────────────────────────────────────
            // Mode 1: Apply normalization + fill validation histograms
            // ──────────────────────────────────────────────────────────
            const double normRatio     = getNormRatio(nJets);
            const double weightFinal   = weightAfterBTag * normRatio;

            // 세 가지 가중치 레벨:
            //   noSF     = weightBeforeBTag           (b-tag 보정 없음)
            //   withSF   = weightAfterBTag            (b-tag SF 적용, 정규화 없음)
            //   reweight = weightAfterBTag * normRatio (최종 보정)

            // --- Event-level histograms ---
            h_nJets_noSF->Fill(nJets,  weightBeforeBTag);
            h_nJets_withSF->Fill(nJets, weightAfterBTag);
            h_nJets_reweight->Fill(nJets, weightFinal);

            h_nbJets_noSF->Fill(nbJets,  weightBeforeBTag);
            h_nbJets_withSF->Fill(nbJets, weightAfterBTag);
            h_nbJets_reweight->Fill(nbJets, weightFinal);

            h_HT_noSF->Fill(currentHT,  weightBeforeBTag);
            h_HT_withSF->Fill(currentHT, weightAfterBTag);
            h_HT_reweight->Fill(currentHT, weightFinal);

            // --- Per-jet histograms ---
            const int nFill = std::min(nJets, BTagConfig::maxJetsForHist);
            for (int j = 0; j < nFill; ++j) {
                const double pt  = reader->GetJetPt().at(j);
                const double eta = reader->GetJetEta().at(j);
                const double disc = bTagScore->at(j);

                v_jetPt_noSF[j]->Fill(pt,    weightBeforeBTag);
                v_jetPt_withSF[j]->Fill(pt,   weightAfterBTag);
                v_jetPt_reweight[j]->Fill(pt,  weightFinal);

                v_jetEta_noSF[j]->Fill(eta,   weightBeforeBTag);
                v_jetEta_withSF[j]->Fill(eta,  weightAfterBTag);
                v_jetEta_reweight[j]->Fill(eta, weightFinal);

                v_bTag_noSF[j]->Fill(disc,    weightBeforeBTag);
                v_bTag_withSF[j]->Fill(disc,   weightAfterBTag);
                v_bTag_reweight[j]->Fill(disc,  weightFinal);
            }
        }

        ++nProcessed;
    }

    // ════════════════════════════════════════════════════════════════════════
    // Phase 4: Finalize
    // ════════════════════════════════════════════════════════════════════════
    std::cout << "\n  Events processed : " << nProcessed
              << "\n  Passed trigger   : " << nPassTrigger << "\n\n";

    outputFile->cd();

    if (processingMode == 0) {
        // ────────────────────────────────────────────────────────────────
        // Mode 0: Compute normalization ratio and save
        // ────────────────────────────────────────────────────────────────
        TH1D* h_normRatio = dynamic_cast<TH1D*>(
            h_nJets_sumBefore->Clone("h_nJets_normRatio"));
        h_normRatio->SetTitle("Normalization ratio (before/after b-tag SF);nJets;r");
        h_normRatio->Divide(h_nJets_sumAfter);

        // 요약 출력
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║  Normalization Ratios (r = Σw_before / Σw_after)            ║\n";
        std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
        std::cout << "  nJets   Σw_before         Σw_after          ratio\n";
        std::cout << "  ─────   ──────────────    ──────────────    ──────────\n";

        for (int bin = 1; bin <= h_normRatio->GetNbinsX(); ++bin) {
            double before = h_nJets_sumBefore->GetBinContent(bin);
            double after  = h_nJets_sumAfter->GetBinContent(bin);
            double ratio  = h_normRatio->GetBinContent(bin);

            if (before == 0 && after == 0) continue;

            int nJetsVal = static_cast<int>(
                h_normRatio->GetXaxis()->GetBinCenter(bin));
            std::cout << "  " << std::setw(5) << nJetsVal
                      << "   " << std::setw(14) << std::fixed
                      << std::setprecision(4) << before
                      << "    " << std::setw(14) << after
                      << "    " << std::setw(10) << std::setprecision(6)
                      << ratio << "\n";
        }
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";

        // 히스토그램 저장
        h_nJets_sumBefore->Write();
        h_nJets_sumAfter->Write();
        h_normRatio->Write();

    } else {
        // ────────────────────────────────────────────────────────────────
        // Mode 1: 검증 히스토그램 저장
        // ────────────────────────────────────────────────────────────────

        // Yield 보존 검증 출력
        if (h_nJets_noSF && h_nJets_reweight) {
            double totalNoSF = h_nJets_noSF->Integral();
            double totalReW  = h_nJets_reweight->Integral();
            double deltaPercent = (totalNoSF != 0)
                ? (totalReW / totalNoSF - 1.0) * 100.0 : 0.0;

            std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
            std::cout << "║  Yield Preservation Check                                   ║\n";
            std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
            std::cout << "  Σw (no b-tag SF)  : " << std::fixed
                      << std::setprecision(4) << totalNoSF << "\n";
            std::cout << "  Σw (reweighted)   : " << totalReW << "\n";
            std::cout << "  Δ (relative)      : " << std::setprecision(4)
                      << deltaPercent << " %\n";
            std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";
        }

        // 이벤트 레벨 히스토그램 저장
        h_nJets_noSF->Write();      h_nJets_withSF->Write();      h_nJets_reweight->Write();
        h_nbJets_noSF->Write();     h_nbJets_withSF->Write();     h_nbJets_reweight->Write();
        h_HT_noSF->Write();         h_HT_withSF->Write();         h_HT_reweight->Write();

        // 정규화 비율 히스토그램도 함께 저장 (참고용)
        if (normRatioHist) normRatioHist->Write();

        // 제트 레벨 히스토그램 저장
        for (int j = 0; j < BTagConfig::maxJetsForHist; ++j) {
            v_jetPt_noSF[j]->Write();   v_jetPt_withSF[j]->Write();   v_jetPt_reweight[j]->Write();
            v_jetEta_noSF[j]->Write();  v_jetEta_withSF[j]->Write();  v_jetEta_reweight[j]->Write();
            v_bTag_noSF[j]->Write();    v_bTag_withSF[j]->Write();    v_bTag_reweight[j]->Write();
        }
    }

    std::cout << ">>> Output saved: " << getOutputName() << "\n";
    delete outputFile;
}
