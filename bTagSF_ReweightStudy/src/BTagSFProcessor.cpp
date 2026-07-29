#define BTagSFProcessor_cxx
#include "BTagSFProcessor.hh"
#include "Config.hh"
#include "Config_TtCatGroup.hh"

#include <TH2.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

// ============================================================================
// Helper: Fatal invariant violation
// ============================================================================
static inline void FATAL_INVARIANT(const char* msg, Long64_t entry) {
    std::cerr << "\n[FATAL][InvariantViolation] entry=" << entry
              << " : " << msg << std::endl;
    std::cerr << "  -> This event should not exist after skimming. Exiting.\n";
    std::exit(1);
}

// ============================================================================
// Constructor / Destructor
// ============================================================================
BTagSFProcessor::BTagSFProcessor() {}

BTagSFProcessor::~BTagSFProcessor()
{
    delete reader;   reader    = nullptr;
    if (inputFile) { inputFile->Close(); delete inputFile; inputFile = nullptr; }
}

// ============================================================================
// File naming
// ============================================================================
void    BTagSFProcessor::setNtupleName(TString name) { ntupleName = name; }
TString BTagSFProcessor::getInputName()  const { return ntupleName + ".root"; }
TString BTagSFProcessor::getOutputName() const {
    return TString(Config::BTagOutPrefix()) + ntupleName + ".root";
}

// ============================================================================
// Resolve b-tag JSON path (CVMFS first, then local fallback)
// ============================================================================
std::string BTagSFProcessor::resolveBTagJSONPath()
{
    for (const auto& path : Config::BTagJSONPaths()) {
        if (fs::exists(path)) return path;
    }
    std::cerr << "[BTagSFProcessor][FATAL] No valid b-tag JSON found. Tried:\n";
    for (const auto& p : Config::BTagJSONPaths())
        std::cerr << "    " << p << "\n";
    std::exit(1);
    return "";
}

// ============================================================================
// CurrentEventProcessKey
// ----------------------------------------------------------------------------
// Returns the process *group* key for the current event (ttH AN App. A.2.1).
//   - inclusive ttbar: dispatched by the ttbar id (tt+LF / tt+cc / tt+B / tt+nb)
//   - other samples: fixed mapping by sample name (Config_TtCatGroup.hh)
//
// [2026-07-29] dispatch 입력을 genTtbarId → **expandedTtbarId** 로 교체.
//
//   왜: NanoAOD `genTtbarId % 100` 은 55 를 넘지 않는다. tt+nb 그룹의 정의역인
//       61/62(tt+bbb) · 71/72(tt+4b) 는 analyzer 가 ttnb lookup 을 붙여
//       `expandedTtbarId` 에 쓴 값에만 존재한다. 원본으로 dispatch 하면
//       tt+nb 그룹에 이벤트가 **한 건도** 들어오지 않는다.
//
//   왜 조용했나: makeReweightJSON 의 "빈 그룹" 경고는 히스토그램의 존재 여부만
//       본다. 히스토그램은 만들어지므로 경고가 안 뜨고, ratio = (sW>0) ?
//       sN/sW : 1.0 규칙에 따라 tt+nb 가 390 bin 전부 1.0 이 된다. 결과 JSON 은
//       8-key 로 멀쩡해 보이고 8번째만 무효다.
// ============================================================================
std::string BTagSFProcessor::CurrentEventProcessKey(const std::string& sampleName) const
{
    return TtCatGroup::MakeProcessKey(sampleName, reader->GetExpandedTtbarId());
}

// ============================================================================
// Resolve flavor-aware systematic name (BTV recommendation)
//   c-jet (flavor=4) → only cferr1/2 (otherwise → "central")
//   b/light (flavor=5/0) → everything except cferr (otherwise → "central")
// ============================================================================
std::string BTagSFProcessor::resolveJetSystematic(
    int hadronFlavor, const std::string& eventSystematic)
{
    const bool isCFerr =
        (eventSystematic.find("cferr") != std::string::npos);

    if (hadronFlavor == 4) {
        return isCFerr ? eventSystematic : "central";
    } else {
        return isCFerr ? "central" : eventSystematic;
    }
}

// ============================================================================
// Evaluate single-jet b-tag SF via correctionlib
// ============================================================================
double BTagSFProcessor::evaluateJetBTagSF(
    const std::string& systematic,
    int    hadronFlavor,
    double abseta,
    double pt,
    double discriminant) const
{
    std::vector<correction::Variable::Type> args(btagSlots.totalInputs);

    args[btagSlots.systematic]   = systematic;
    args[btagSlots.flavor]       = hadronFlavor;
    args[btagSlots.abseta]       = abseta;
    args[btagSlots.pt]           = pt;
    args[btagSlots.discriminant] = discriminant;
    if (btagSlots.workingPoint >= 0)
        args[btagSlots.workingPoint] = std::string("shape");

    return btagShapeSFProvider->evaluate(args);
}

// ============================================================================
// Per-event b-tag weight = ∏ SF(jet_i) over all jets
// (flavor-aware systematic per jet)
// ============================================================================
double BTagSFProcessor::computeBTagEventWeight(
    const std::string& eventSystematic) const
{
    double w = 1.0;
    const auto& pt   = reader->GetJetPt();
    const auto& eta  = reader->GetJetEta();
    const auto& disc = reader->GetBTagScore();
    const auto& had  = reader->GetHadronFlavor();

    const size_t nJets = pt.size();
    for (size_t i = 0; i < nJets; ++i) {
        const std::string jetSyst = resolveJetSystematic(had[i], eventSystematic);
        const double sf = evaluateJetBTagSF(
            jetSyst, had[i], std::fabs(eta[i]), pt[i], disc[i]);
        w *= sf;
    }
    return w;
}

// ============================================================================
// passRegion — 측정 영역의 lepton/MET 조건
// ----------------------------------------------------------------------------
// [2026-07-29] region 은 런타임 env `TTHH_BTAGRW_REGION` 으로 고른다.
//
//   왜 고를 수 있게 했나: b-tag norm reweight 는 trigger SF 와 성격이 다르다.
//   trigger SF 는 orthogonal trigger 가 필요해서 신호영역에서 측정 자체가
//   불가능하지만, b-tag RW 의 제약은 "b-tag 컷 금지"(BTV) 하나뿐이라
//   **적용할 영역 그대로에서 잴 수 있다.** 그러니 "쓸 영역에서 재라" 가 원칙이다.
//
//   FH     : nVetoLeptons == 0                             (main 신호영역)
//   muonCR : nMuons==1 && nElecs==0 && MET_pt > metCutCR    (--region muon)
//   none   : 컷 없음 (구 동작 재현 전용 — FH 와 muon control 이 섞인다)
//
//   ⚠ 산출물 파일명에 region 꼬리표가 붙는다(Config::RegionTag()). 그게 없으면
//     muonCR 유도가 FH 산출물을 말없이 덮어쓴다.
// ============================================================================
bool BTagSFProcessor::passRegion() const
{
    switch (Config::CurrentRegion()) {
        case Config::Region::kFH:
            return reader->GetNVetoLeptons() == 0;
        case Config::Region::kMuonCR:
            return reader->GetNMuons() == 1
                && reader->GetNElecs() == 0
                && reader->GetMET()    > Config::metCutCR;
        case Config::Region::kNone:
            return true;
    }
    return true;
}

// ============================================================================
// Hadronic trigger OR with PD-exclusivity for Data
//   BTagCSV → keep 4J3T  group only
//   JetHT   → keep (6J1T||6J2T||PFHT1050) and reject 4J3T (avoid double count)
//   MC      → OR of everything
// (era=B uses *_B branches; CDEF uses *_CDEF)
// ============================================================================
bool BTagSFProcessor::passHadronicTrigger(
    const TString& dataSet, const TString& era) const
{
    const bool isEraB = (era == "B");
    const bool fired4J3T = isEraB ? reader->GetPassTrigger_4J3T_B()
                                  : reader->GetPassTrigger_4J3T_CDEF();
    const bool fired6J1T = isEraB ? reader->GetPassTrigger_6J1T_B()
                                  : reader->GetPassTrigger_6J1T_CDEF();
    const bool fired6J2T = isEraB ? reader->GetPassTrigger_6J2T_B()
                                  : reader->GetPassTrigger_6J2T_CDEF();
    const bool firedHT   = reader->GetPassTrigger_PFHT1050();

    const bool group_BTagCSV = fired4J3T;
    const bool group_JetHT   = (fired6J1T || fired6J2T || firedHT);

    if (dataSet == "default") {
        // MC
        return group_BTagCSV || group_JetHT;
    }
    if (dataSet.Contains("BTagCSV")) {
        return group_BTagCSV;
    }
    if (dataSet.Contains("JetHT")) {
        return group_JetHT && !group_BTagCSV;
    }
    if (dataSet.Contains("SingleMuon")) {
        return group_BTagCSV || group_JetHT;
    }
    std::cerr << "[BTagSFProcessor][WARN] Unknown dataSet: " << dataSet << "\n";
    return false;
}

// ============================================================================
// Evaluate trigger SF via correctionlib (FATAL on failure)
// ============================================================================
double BTagSFProcessor::evaluateTriggerSF(
    int nBJets, double jet6Eta, double HT, double jet6PT) const
{
    if (!trigSFProvider) {
        std::cerr << "[BTagSFProcessor][FATAL] trigSFProvider not loaded\n";
        std::exit(1);
    }

    try {
        return trigSFProvider->evaluate({
            static_cast<int>(nBJets),
            static_cast<double>(jet6Eta),
            static_cast<double>(HT),
            static_cast<double>(jet6PT)
        });
    } catch (const std::exception& e) {
        std::cerr << "[BTagSFProcessor][FATAL] Trigger SF evaluate failed: "
                  << e.what()
                  << "  (nB=" << nBJets << ", eta=" << jet6Eta
                  << ", HT=" << HT << ", pt=" << jet6PT << ")\n";
        std::exit(1);
    }
    return 1.0;
}

// ============================================================================
// Init: open file, build NtupleReader, load correctionlib JSONs
// ============================================================================
void BTagSFProcessor::Init()
{
    // ── Open input file & tree ──
    TString ntuplePath = Config::InputBaseDir() + getInputName();

    inputFile = TFile::Open(ntuplePath);
    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "[BTagSFProcessor][FATAL] Cannot open " << ntuplePath << "\n";
        std::exit(1);
    }

    inputFile->GetObject(Config::treePath.c_str(), fChain);
    if (!fChain) {
        std::cerr << "[BTagSFProcessor][FATAL] Cannot get TTree: "
                  << Config::treePath << "\n";
        std::exit(1);
    }

    reader = new NtupleReader(fChain);

    // ── Load correctionlib: b-tag shape SF ──
    std::string btagJSONPath = resolveBTagJSONPath();
    std::cout << "  [BTagSFProcessor] b-tag JSON: " << btagJSONPath << "\n";

    btagCorrectionSet   = correction::CorrectionSet::from_file(btagJSONPath);
    btagShapeSFProvider = btagCorrectionSet->at(Config::btagCorrectionName);

    // Build input-slot mapping by inspecting JSON schema
    const auto& inputs = btagShapeSFProvider->inputs();
    btagSlots.totalInputs = static_cast<int>(inputs.size());

    std::cout << "  [BTagSFProcessor] " << Config::btagCorrectionName
              << " inputs (" << btagSlots.totalInputs << "): ";
    for (int i = 0; i < btagSlots.totalInputs; ++i) {
        const std::string& name = inputs[i].name();
        std::cout << name << " ";

        if      (name == "systematic")    btagSlots.systematic   = i;
        else if (name == "working_point") btagSlots.workingPoint = i;
        else if (name == "flavor")        btagSlots.flavor       = i;
        else if (name == "abseta")        btagSlots.abseta       = i;
        else if (name == "pt")            btagSlots.pt           = i;
        else if (name == "discriminant")  btagSlots.discriminant = i;
    }
    std::cout << "\n";

    if (btagSlots.systematic < 0 || btagSlots.flavor < 0 ||
        btagSlots.abseta < 0 || btagSlots.pt < 0 || btagSlots.discriminant < 0) {
        std::cerr << "[BTagSFProcessor][FATAL] Missing required input field in "
                  << Config::btagCorrectionName << "\n";
        std::exit(1);
    }

    // ── Load correctionlib: trigger SF ──
    std::cout << "  [BTagSFProcessor] trigger SF JSON: " << Config::TriggerSFJSON() << "\n";

    if (!fs::exists(Config::TriggerSFJSON())) {
        std::cerr << "[BTagSFProcessor][FATAL] Trigger SF JSON not found: "
                  << Config::TriggerSFJSON() << "\n";
        std::exit(1);
    }

    trigCorrectionSet = correction::CorrectionSet::from_file(Config::TriggerSFJSON());
    trigSFProvider    = trigCorrectionSet->at("triggerSF");

    std::cout << "  [BTagSFProcessor] Init complete.\n\n";
}

// ============================================================================
// Loop (Two-Pass)
// ----------------------------------------------------------------------------
// Pass 1: For each event, dispatch to one of the per-event process group keys
//         (ttH AN App. A.2.1). Accumulate per-(group, syst, bin) sumNoSF and
//         sumWithSF. Output as TH1D/TH2D under "NormSums/" so makeReweightJSON
//         can sum across samples and compute the final per-group ratio.
//
// Pass 2: Validation 3-tier histograms (noSF / withSF / reweighted). Since the
//         final per-group ratio is computed only after group-sum (in
//         makeReweightJSON), Pass 2 here keeps reweighted = withSF as a
//         placeholder. True closure is verified downstream after JSON load.
// ============================================================================
void BTagSFProcessor::Loop()
{
    if (!fChain) return;

    // ====================================================================
    // Phase 1: Configuration & Sample ID
    // ====================================================================
    Config::Dump();

    TString sampleName = ntupleName;
    std::cout << "Sample Name: " << sampleName << "\n";

    // [2026-07-29] weight/era 조회를 SampleRegistry 로 위임 (analyzer 제출기와
    //   동일한 xsec_db + prescan_summary, 동일한 공식). 모르는 샘플은 그 안에서
    //   FATAL 이므로 여기 null 검사는 형식적으로만 남긴다.
    const auto* sampleInfo = Config::GetSampleInfo(sampleName.Data());

    isData = sampleInfo->isData;
    const double xsecWeight = sampleInfo->weight;
    std::cout << "  Type   : " << (isData ? "Data" : "MC") << "\n";
    std::cout << "  Weight : " << xsecWeight << "\n";

    // ── DataSet / Era ────────────────────────────────────────────────────
    //   이전 코드는 "첫 '_' 뒤 전부" 였다. campaign 이름(`JetHT_Run2017B`)에서는
    //   era 가 "Run2017B" 가 되어 passHadronicTrigger() 의 `era=="B"` 가 항상
    //   거짓 → Run B 를 CDEF trigger bit 로 평가한다. SampleRegistry 가
    //   `Run\d{4}([A-Z])$` 로 한 글자 era 를 뽑아 준다.
    TString dataSet = "default";
    TString era     = "default";
    if (isData) {
        dataSet = sampleInfo->dataset.c_str();
        era     = sampleInfo->era.c_str();
        std::cout << "  DataSet: " << dataSet << ", Era: " << era << "\n";
    }

    // ── skim preflight : region 이 요구하는 branch ────────────────────────
    //   Data/MC 공통. 컷이 조용히 무시되면 "그 영역에서 유도했다"고 믿으면서
    //   실제로는 다른 영역의 숫자가 나온다. 그래서 부재는 전부 FATAL 이다.
    std::cout << "  Region : " << Config::RegionName()
              << "   (env TTHH_BTAGRW_REGION; 산출물 꼬리표 '"
              << Config::RegionTag() << "')\n";

    if (Config::CurrentRegion() == Config::Region::kFH && !reader->HasNVetoLeptons()) {
        std::cerr <<
          "\n[BTagSFProcessor][FATAL] region=FH 인데 skim 에 'nVetoLeptons' branch 가 없다: "
          << getInputName() << "\n"
          "  main 의 lepton veto 값을 읽을 수 없다. nMuons==0 으로 대체하는 fallback 은\n"
          "  두지 않는다 -- 그건 lead-muon gate 를 통과하지 못한 soft-lepton 이벤트를\n"
          "  살려 두어, main 과 다른 위상공간에서 reweight 를 유도하게 만든다 (경고 없이).\n"
          "  Fix: 이 branch 를 쓰는 analyzer 로 btagtrig skim 을 다시 만들 것\n"
          "       (ttHHanalyzer_unified.h 의 nVetoLeptons branch, 2026-07-29).\n";
        std::exit(1);
    }
    if (Config::CurrentRegion() == Config::Region::kMuonCR && !reader->HasMET()) {
        std::cerr <<
          "\n[BTagSFProcessor][FATAL] region=muonCR 인데 skim 에 'MET_pt' branch 가 없다: "
          << getInputName() << "\n"
          "  MET cut(>" << Config::metCutCR << " GeV)을 걸 수 없다. 그냥 넘어가면\n"
          "  '제어영역에서 유도했다'고 믿으면서 실제로는 MET cut 없는 1-muon 영역의\n"
          "  숫자가 나온다.\n"
          "  Fix: MET_pt branch 를 쓰는 analyzer 로 btagtrig skim 을 다시 만들 것\n"
          "       (ttHHanalyzer_unified.h, 2026-07-29).\n";
        std::exit(1);
    }
    if (Config::CurrentRegion() == Config::Region::kNone) {
        std::cerr <<
          "\n[BTagSFProcessor][WARN] region=none — lepton/MET 컷 없이 유도한다.\n"
          "  btagtrig skim 은 FH 이벤트와 muon control 이벤트를 **함께** 담으므로\n"
          "  둘이 섞인 위상공간에서 reweight 가 나온다. 이건 어느 분석 영역에도\n"
          "  대응하지 않는다. 2026-07-29 이전 동작 재현 목적이 아니면 쓰지 말 것.\n";
    }

    // ── skim preflight (MC) ──────────────────────────────────────────────
    //   여기서 막지 않으면 두 가지가 **조용히** 틀린다:
    //     (1) expandedTtbarId 부재 → tt+nb 그룹 전 bin 1.0
    //     (2) stitchWeight 부재    → inclusive tt 와 dedicated ttbb/tt4b 이중계수
    //   둘 다 크래시 없이 "그럴듯한" JSON 을 만든다. 그래서 FATAL 이다.
    if (!isData) {
        if (!reader->HasExpandedTtbarId()) {
            std::cerr <<
              "\n[BTagSFProcessor][FATAL] skim has no 'expandedTtbarId' branch: "
              << getInputName() << "\n"
              "  Process groups would be dispatched by the raw NanoAOD genTtbarId,\n"
              "  whose %100 never exceeds 55 -> the tt+nb group (61/62/71/72) would be\n"
              "  EMPTY and the derived JSON would carry an all-1.0 tt+nb group that\n"
              "  looks like a valid 8th key.\n"
              "  Fix: re-run the analyzer in btagtrig mode with\n"
              "       path_expanded_ttbarid_dir pointing at the ttnb lookup dir.\n";
            std::exit(1);
        }
        if (!reader->HasStitchWeight() && TtCatGroup::IsTtbarFamily(sampleName.Data())) {
            std::cerr <<
              "\n[BTagSFProcessor][FATAL] skim has no 'stitchWeight' branch: "
              << getInputName() << "\n"
              "  This sample is part of the ttbar stitching set, so without the\n"
              "  multiplier the inclusive ttbar and the dedicated ttbb/tt4b samples\n"
              "  both fill the same phase space (tt+B double counting) and the\n"
              "  per-group ratio comes out wrong -- silently.\n"
              "  Fix: re-run the analyzer in btagtrig mode with path_stitch_json set.\n";
            std::exit(1);
        }
    }

    const Long64_t nEntries = fChain->GetEntries();
    std::cout << "\n>>> Total entries: " << nEntries << "\n\n";

    const int maxNJetsBin   = Config::btagMaxNJetsBin;
    const int totalBins     = Config::getTotalReweightBins();
    const int nHTBins       = Config::getNumHTBins();
    const auto& systematics = Config::btagSystematics;

    std::cout << "  Reweight mode  : "
              << (Config::useHTForReweight ? "2D (nJets × HT)" : "1D (nJets)")
              << "\n";
    std::cout << "  Total flat bins: " << totalBins << "\n";
    if (Config::useHTForReweight)
        std::cout << "  nHTBins        : " << nHTBins << "\n";
    std::cout << "\n";

    // ====================================================================
    // Phase 2: Pass 1 — Per-process accumulators of (Σω_noSF, Σω_withSF)
    //
    // Storage: sumNoSF[pkey][syst][flatBin]
    // Group sum across samples is done later in makeReweightJSON.
    // ====================================================================
    const std::vector<std::string> processKeys =
        TtCatGroup::ProcessKeysForSample(sampleName.Data());

    std::map<std::string, std::map<std::string, std::vector<double>>> sumNoSF;
    std::map<std::string, std::map<std::string, std::vector<double>>> sumWithSF;

    for (const auto& pkey : processKeys) {
        for (const auto& syst : systematics) {
            sumNoSF  [pkey][syst].assign(totalBins, 0.0);
            sumWithSF[pkey][syst].assign(totalBins, 0.0);
        }
    }

    if (!isData) {
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║  Pass 1: Accumulating per-process (Σω_noSF, Σω_withSF)       ║\n";
        std::cout << "║  Process groups for this sample: ";
        for (size_t i = 0; i < processKeys.size(); ++i) {
            std::cout << processKeys[i];
            if (i + 1 < processKeys.size()) std::cout << ", ";
        }
        std::cout << "\n╚══════════════════════════════════════════════════════════════╝\n";

        for (Long64_t j = 0; j < nEntries; ++j) {
            reader->GetEntry(j);

            if (j % Config::progressInterval == 0) {
                std::cout << "    [Pass 1] " << j << " / " << nEntries
                          << " (" << std::fixed << std::setprecision(1)
                          << 100.0 * j / nEntries << "%)\n";
            }

            // Skim invariants (NO b-tag selection — BTV requirement)
            if (reader->GetNJets() < Config::minNJets)
                FATAL_INVARIANT("NJets < Cuts::nJets", j);
            if (reader->GetJetPt().at(Cuts::nJets - 1) <= Cuts::sixthJetPt)
                FATAL_INVARIANT("Nth jet pT <= Cuts::sixthJetPt", j);
            if (reader->GetHT() < Cuts::HT)
                FATAL_INVARIANT("HT < Cuts::HT", j);
            if (!reader->GetPassMETFilters())
                FATAL_INVARIANT("MET filters", j);

            if (!passHadronicTrigger(dataSet, era)) continue;

            // [2026-07-29] region selection (Config::CurrentRegion(), env
            //   TTHH_BTAGRW_REGION). b-tag RW 는 trigger SF 와 달리 적용 영역
            //   그대로에서 잴 수 있으므로, 쓸 영역을 골라 유도한다.
            if (!passRegion()) continue;

            // Per-event process key dispatch
            const std::string pkey = CurrentEventProcessKey(sampleName.Data());

            // Defensive (genTtbarId edge case)
            if (sumNoSF.find(pkey) == sumNoSF.end()) {
                for (const auto& syst : systematics) {
                    sumNoSF  [pkey][syst].assign(totalBins, 0.0);
                    sumWithSF[pkey][syst].assign(totalBins, 0.0);
                }
            }

            // [2026-07-29] stitchWeight 를 곱한다. analyzer 가 이벤트별로 이미
            //   결정해 skim 에 실어 둔 값이므로, 여기서 재계산하지 않고 그대로
            //   따라간다 (analyzer 와 Pass-1 이 서로 다른 MC 조성을 보면 안 된다).
            //   inclusive 는 owned category 밖에서 0, dedicated 는 owned 안에서 r.
            //   0 인 이벤트는 자연히 양쪽 합에서 빠진다.
            const double baseWeight =
                reader->GetGenWeight()
                * reader->GetPUWeight()
                * reader->GetPrefireWeight()
                * reader->GetStitchWeight()
                * xsecWeight;

            const double currentHT = reader->GetHT();
            const double jet6PT    = reader->GetJetPt().at(5);
            const double jet6Eta   = reader->GetJetEta().at(5);
            const int    nBJets    = reader->GetNBJets();

            const double trigSF = evaluateTriggerSF(nBJets, jet6Eta, currentHT, jet6PT);
            const double weightNoSFVal = baseWeight * trigSF;

            const int flatBin = Config::getReweightFlatIndex(
                reader->GetNJets(), currentHT);

            for (const auto& syst : systematics) {
                double btagEvtW = 1.0;
                try {
                    btagEvtW = computeBTagEventWeight(syst);
                } catch (const std::exception& e) {
                    if (j < 5) {
                        std::cerr << "[WARN] btag SF failed for syst=" << syst
                                  << " entry=" << j << ": " << e.what() << "\n";
                    }
                    btagEvtW = 1.0;
                }
                sumNoSF  [pkey][syst][flatBin] += weightNoSFVal;
                sumWithSF[pkey][syst][flatBin] += weightNoSFVal * btagEvtW;
            }
        }

        // Print central-only summary per process key
        std::cout << "\n  Per-process accumulators (central):\n";
        for (const auto& pkey : processKeys) {
            if (sumNoSF.find(pkey) == sumNoSF.end()) continue;
            std::cout << "  ── " << pkey << " ──\n";
            std::cout << "  bin  | sumNoSF        | sumWithSF      | local r (preview)\n";
            std::cout << "  -----+----------------+----------------+---------\n";
            for (int b = 0; b < totalBins; ++b) {
                double sN = sumNoSF[pkey]["central"][b];
                double sW = sumWithSF[pkey]["central"][b];
                if (sN == 0.0 && sW == 0.0) continue;
                double r = (sW > 0.0) ? sN / sW : 1.0;
                std::cout << "  " << std::setw(4) << b
                          << " | " << std::setw(14) << std::setprecision(4) << sN
                          << " | " << std::setw(14) << sW
                          << " | " << std::setprecision(6) << r << "\n";
            }
        }
        std::cout << "\n";
    }

    // ====================================================================
    // Phase 3: Create Output & Save Per-Process Sums
    //
    // For each (pkey, syst): write sumNoSF and sumWithSF as separate hists
    // under "NormSums/". makeReweightJSON.cpp will read these and compute
    // group-summed ratios.
    //
    // Naming:
    //   2D: h_sumNoSF_nJets_HT_<pkey_sanitized>_<syst>
    //       h_sumWithSF_nJets_HT_<pkey_sanitized>_<syst>
    //   1D: h_sumNoSF_nJets_<pkey_sanitized>_<syst>
    //       h_sumWithSF_nJets_<pkey_sanitized>_<syst>
    // ====================================================================
    TFile* outputFile = new TFile(getOutputName(), "RECREATE");
    outputFile->cd();

    if (!isData) {
        outputFile->mkdir("NormSums");
        outputFile->cd("NormSums");

        if (Config::useHTForReweight) {
            std::vector<double> nJetsEdges;
            for (int i = 0; i <= maxNJetsBin + 1; ++i)
                nJetsEdges.push_back(static_cast<double>(i) - 0.5);

            const auto& htEdges = Config::reweightHT_edges;

            for (const auto& pkey : processKeys) {
                if (sumNoSF.find(pkey) == sumNoSF.end()) continue;
                const std::string skey = TtCatGroup::SanitizeKey(pkey);

                for (const auto& syst : systematics) {
                    TString hNameNo = "h_sumNoSF_nJets_HT_"
                                    + TString(skey.c_str()) + "_"
                                    + TString(syst.c_str());
                    auto* hNo = new TH2D(hNameNo,
                        hNameNo + ";nJets;HT [GeV];Sum w (noSF)",
                        maxNJetsBin + 1, nJetsEdges.data(),
                        nHTBins, htEdges.data());

                    TString hNameWi = "h_sumWithSF_nJets_HT_"
                                    + TString(skey.c_str()) + "_"
                                    + TString(syst.c_str());
                    auto* hWi = new TH2D(hNameWi,
                        hNameWi + ";nJets;HT [GeV];Sum w (withSF)",
                        maxNJetsBin + 1, nJetsEdges.data(),
                        nHTBins, htEdges.data());

                    for (int nj = 0; nj <= maxNJetsBin; ++nj) {
                        for (int ht = 0; ht < nHTBins; ++ht) {
                            int flatIdx = nj * nHTBins + ht;
                            hNo->SetBinContent(nj + 1, ht + 1,
                                               sumNoSF[pkey][syst][flatIdx]);
                            hWi->SetBinContent(nj + 1, ht + 1,
                                               sumWithSF[pkey][syst][flatIdx]);
                        }
                    }
                    hNo->Write(); hWi->Write();
                }
            }
        } else {
            for (const auto& pkey : processKeys) {
                if (sumNoSF.find(pkey) == sumNoSF.end()) continue;
                const std::string skey = TtCatGroup::SanitizeKey(pkey);

                for (const auto& syst : systematics) {
                    TString hNameNo = "h_sumNoSF_nJets_"
                                    + TString(skey.c_str()) + "_"
                                    + TString(syst.c_str());
                    auto* hNo = new TH1D(hNameNo, hNameNo + ";nJets;Sum w (noSF)",
                                         maxNJetsBin + 1, -0.5, maxNJetsBin + 0.5);

                    TString hNameWi = "h_sumWithSF_nJets_"
                                    + TString(skey.c_str()) + "_"
                                    + TString(syst.c_str());
                    auto* hWi = new TH1D(hNameWi, hNameWi + ";nJets;Sum w (withSF)",
                                         maxNJetsBin + 1, -0.5, maxNJetsBin + 0.5);

                    for (int b = 0; b <= maxNJetsBin; ++b) {
                        hNo->SetBinContent(b + 1, sumNoSF[pkey][syst][b]);
                        hWi->SetBinContent(b + 1, sumWithSF[pkey][syst][b]);
                    }
                    hNo->Write(); hWi->Write();
                }
            }
        }
        outputFile->cd();
    }

    // ── Validation histograms (using Config definitions) ──
    const int maxJets = Config::btagMaxJetsForPerJetHist;

    auto* h_nJets_noSF       = new TH1D("h_nJets_noSF",      ";nJets;Events",
        Config::histNJets_nBins, Config::histNJets_Low, Config::histNJets_High);
    auto* h_nJets_withSF     = new TH1D("h_nJets_withSF",    ";nJets;Events",
        Config::histNJets_nBins, Config::histNJets_Low, Config::histNJets_High);
    auto* h_nJets_reweighted = new TH1D("h_nJets_reweighted", ";nJets;Events",
        Config::histNJets_nBins, Config::histNJets_Low, Config::histNJets_High);

    auto* h_HT_noSF       = new TH1D("h_HT_noSF",      ";HT [GeV];Events",
        Config::histHT_nBins, Config::histHT_Low, Config::histHT_High);
    auto* h_HT_withSF     = new TH1D("h_HT_withSF",    ";HT [GeV];Events",
        Config::histHT_nBins, Config::histHT_Low, Config::histHT_High);
    auto* h_HT_reweighted = new TH1D("h_HT_reweighted", ";HT [GeV];Events",
        Config::histHT_nBins, Config::histHT_Low, Config::histHT_High);

    auto* h_nbJets_noSF       = new TH1D("h_nbJets_noSF",      ";n_{b-jets};Events",
        Config::histNBJets_nBins, Config::histNBJets_Low, Config::histNBJets_High);
    auto* h_nbJets_withSF     = new TH1D("h_nbJets_withSF",    ";n_{b-jets};Events",
        Config::histNBJets_nBins, Config::histNBJets_Low, Config::histNBJets_High);
    auto* h_nbJets_reweighted = new TH1D("h_nbJets_reweighted", ";n_{b-jets};Events",
        Config::histNBJets_nBins, Config::histNBJets_Low, Config::histNBJets_High);

    std::vector<TH1D*> h_jetPt_noSF(maxJets), h_jetPt_withSF(maxJets), h_jetPt_reweighted(maxJets);
    std::vector<TH1D*> h_bTag_noSF(maxJets),  h_bTag_withSF(maxJets),  h_bTag_reweighted(maxJets);

    for (int j = 0; j < maxJets; ++j) {
        h_jetPt_noSF[j]       = new TH1D(Form("h_jetPt_noSF_jet%d", j),
            Form("Jet %d p_{T} (no SF);p_{T} [GeV];Events", j),
            Config::histJetPt_nBins, Config::histJetPt_Low, Config::histJetPt_High);
        h_jetPt_withSF[j]     = new TH1D(Form("h_jetPt_withSF_jet%d", j),
            Form("Jet %d p_{T} (with SF);p_{T} [GeV];Events", j),
            Config::histJetPt_nBins, Config::histJetPt_Low, Config::histJetPt_High);
        h_jetPt_reweighted[j] = new TH1D(Form("h_jetPt_reweighted_jet%d", j),
            Form("Jet %d p_{T} (reweighted);p_{T} [GeV];Events", j),
            Config::histJetPt_nBins, Config::histJetPt_Low, Config::histJetPt_High);

        h_bTag_noSF[j]       = new TH1D(Form("h_bTag_noSF_jet%d", j),
            Form("Jet %d DeepJet (no SF);discriminant;Events", j),
            Config::histBTag_nBins, Config::histBTag_Low, Config::histBTag_High);
        h_bTag_withSF[j]     = new TH1D(Form("h_bTag_withSF_jet%d", j),
            Form("Jet %d DeepJet (with SF);discriminant;Events", j),
            Config::histBTag_nBins, Config::histBTag_Low, Config::histBTag_High);
        h_bTag_reweighted[j] = new TH1D(Form("h_bTag_reweighted_jet%d", j),
            Form("Jet %d DeepJet (reweighted);discriminant;Events", j),
            Config::histBTag_nBins, Config::histBTag_Low, Config::histBTag_High);
    }

    // ====================================================================
    // Phase 4: Pass 2 — Validation hist fill (noSF / withSF / reweighted)
    //
    // Note: per-group ratio is finalized in makeReweightJSON.cpp after
    //       cross-sample sum, so here `reweighted` uses a placeholder
    //       (locally computed sample-only ratio). True closure plot must
    //       be made downstream after JSON load (or use plot_btag.py with
    //       the JSON loaded for proper reweight).
    // ====================================================================
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  Pass 2: Filling validation histograms (3-tier)              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";

    long long nProcessed = 0, nPassed = 0;

    for (Long64_t j = 0; j < nEntries; ++j) {
        reader->GetEntry(j);

        if (j % Config::progressInterval == 0) {
            std::cout << "    [Pass 2] " << j << " / " << nEntries
                      << " (" << std::fixed << std::setprecision(1)
                      << 100.0 * j / nEntries << "%)\n";
        }

        ++nProcessed;

        if (reader->GetNJets() < Config::minNJets)
            FATAL_INVARIANT("NJets < Cuts::nJets", j);
        if (reader->GetJetPt().at(Cuts::nJets - 1) <= Cuts::sixthJetPt)
            FATAL_INVARIANT("Nth jet pT <= Cuts::sixthJetPt", j);
        if (reader->GetHT() < Cuts::HT)
            FATAL_INVARIANT("HT < Cuts::HT", j);
        if (!reader->GetPassMETFilters())
            FATAL_INVARIANT("MET filters", j);
        if (isData && reader->GetFailGoldenJson())
            FATAL_INVARIANT("Golden JSON failed", j);

        if (!passHadronicTrigger(dataSet, era)) continue;

        // [2026-07-29] Pass 1 과 **같은 선택**이어야 한다 (검증 히스토그램이
        //   유도에 쓴 위상공간과 달라지면 closure 가 의미를 잃는다).
        if (!passRegion()) continue;
        ++nPassed;

        const int    nJets     = reader->GetNJets();
        const int    nBJets    = reader->GetNBJets();
        const double currentHT = reader->GetHT();
        const double jet6PT    = reader->GetJetPt().at(5);
        const double jet6Eta   = reader->GetJetEta().at(5);

        // ── Base weight ──
        double baseWeight = 1.0;
        if (!isData) {
            // [2026-07-29] Pass 1 과 **같은 정의** 여야 한다. 한쪽만 stitch 를
            //   곱하면 검증 히스토그램이 유도에 쓴 조성과 달라진다.
            baseWeight = reader->GetGenWeight()
                       * reader->GetPUWeight()
                       * reader->GetPrefireWeight()
                       * reader->GetStitchWeight()
                       * xsecWeight;
        }

        // ── Trigger SF ──
        double trigSF = 1.0;
        if (!isData) {
            trigSF = evaluateTriggerSF(nBJets, jet6Eta, currentHT, jet6PT);
        }

        // ── B-tag event weight (central) ──
        double btagEvtWeight = 1.0;
        if (!isData) {
            try {
                btagEvtWeight = computeBTagEventWeight("central");
            } catch (const std::exception& e) {
                if (j < 5)
                    std::cerr << "[WARN] btag SF entry=" << j << ": " << e.what() << "\n";
                btagEvtWeight = 1.0;
            }
        }

        // ── Sample-local ratio for in-sample closure preview ──
        // (true per-group ratio is computed in makeReweightJSON after group sum)
        double btagNormRatio = 1.0;
        if (!isData) {
            const std::string pkey = CurrentEventProcessKey(sampleName.Data());
            const int flatBin = Config::getReweightFlatIndex(nJets, currentHT);
            auto it = sumNoSF.find(pkey);
            if (it != sumNoSF.end()) {
                const double sN = it->second.at("central").at(flatBin);
                const double sW = sumWithSF[pkey]["central"][flatBin];
                btagNormRatio = (sW > 0.0) ? sN / sW : 1.0;
            }
        }

        const double wNoSF       = baseWeight * trigSF;
        const double wWithSF     = baseWeight * trigSF * btagEvtWeight;
        const double wReweighted = baseWeight * trigSF * btagEvtWeight * btagNormRatio;

        h_nJets_noSF->Fill(nJets, wNoSF);
        h_nJets_withSF->Fill(nJets, wWithSF);
        h_nJets_reweighted->Fill(nJets, wReweighted);

        h_HT_noSF->Fill(currentHT, wNoSF);
        h_HT_withSF->Fill(currentHT, wWithSF);
        h_HT_reweighted->Fill(currentHT, wReweighted);

        h_nbJets_noSF->Fill(nBJets, wNoSF);
        h_nbJets_withSF->Fill(nBJets, wWithSF);
        h_nbJets_reweighted->Fill(nBJets, wReweighted);

        const int nToFill = std::min(nJets, maxJets);
        const auto& ptVec   = reader->GetJetPt();
        const auto& btagVec = reader->GetBTagScore();

        for (int jj = 0; jj < nToFill; ++jj) {
            h_jetPt_noSF[jj]->Fill(ptVec.at(jj), wNoSF);
            h_jetPt_withSF[jj]->Fill(ptVec.at(jj), wWithSF);
            h_jetPt_reweighted[jj]->Fill(ptVec.at(jj), wReweighted);

            h_bTag_noSF[jj]->Fill(btagVec.at(jj), wNoSF);
            h_bTag_withSF[jj]->Fill(btagVec.at(jj), wWithSF);
            h_bTag_reweighted[jj]->Fill(btagVec.at(jj), wReweighted);
        }
    }

    // ====================================================================
    // Phase 5: Summary & Write
    // ====================================================================
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  B-Tag Reweight Summary                                      ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    std::cout << "  Events processed : " << nProcessed << "\n";
    std::cout << "  Events passed    : " << nPassed << "\n";

    if (!isData) {
        double totNoSF = h_nJets_noSF->Integral();
        double totReW  = h_nJets_reweighted->Integral();
        double dev = (totNoSF > 0) ? std::abs(totReW - totNoSF) / totNoSF * 100.0 : 0.0;

        std::cout << std::fixed << std::setprecision(4);
        std::cout << "  Σ weight (noSF)      : " << totNoSF << "\n";
        std::cout << "  Σ weight (withSF)    : " << h_nJets_withSF->Integral() << "\n";
        std::cout << "  Σ weight (reweighted): " << totReW << "\n";
        std::cout << "  Deviation (in-sample): " << std::setprecision(4) << dev << " %"
                  << (dev > 1.0 ? "  [WARNING]" : "  [OK]") << "\n";
        std::cout << "  Note: this is a sample-local closure preview. The true\n"
                     "  per-group closure is verified after group-sum in makeReweightJSON\n"
                     "  and JSON-based application downstream.\n";
    }
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";

    // Write
    std::cout << ">>> Writing output to: " << getOutputName() << "\n";
    outputFile->cd();

    h_nJets_noSF->Write();    h_nJets_withSF->Write();    h_nJets_reweighted->Write();
    h_HT_noSF->Write();       h_HT_withSF->Write();       h_HT_reweighted->Write();
    h_nbJets_noSF->Write();   h_nbJets_withSF->Write();   h_nbJets_reweighted->Write();

    outputFile->mkdir("PerJet");
    outputFile->cd("PerJet");
    for (int jj = 0; jj < maxJets; ++jj) {
        h_jetPt_noSF[jj]->Write();    h_jetPt_withSF[jj]->Write();    h_jetPt_reweighted[jj]->Write();
        h_bTag_noSF[jj]->Write();     h_bTag_withSF[jj]->Write();     h_bTag_reweighted[jj]->Write();
    }

    outputFile->cd();
    std::cout << ">>> Done.\n";
    delete outputFile;
}
