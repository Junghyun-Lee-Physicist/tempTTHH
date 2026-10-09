// ============================================================================
// CorrectionsManager.cc
//
// 구현부 (Implementation).
// 각 load 함수는 생성자에서 호출되며, API 함수는 이벤트 루프에서 호출된다.
//
// [UPDATED] 변경 사항:
//   - loadTrigger_():   ROOT TH2 → correctionlib JSON (trigger_sf.json.gz)
//   - getTriggerSF():   (ht, jet6pt, syst) → (nbJets, eta, ht, pt, syst)
//   - 소멸자:           ROOT 파일 닫기 로직 제거
//
// Author: Junghyun Lee
// ============================================================================
#include "CorrectionsManager.h"
#include "ExitCodes.h"     // [STEP18] canonical exit codes
#include "EraConfig.h"     // [2018] year-keyed constants (single source of truth)
#include "ConfigPath.h"    // [STEP18] cfgpath::resolve (no code-default)
#include <cstdlib>   // [STEP4] std::getenv (fatal exits: tthh::fatalExit, ExitCodes.h)
#include <fstream>
#include <iostream>
#include <filesystem>   // JSON 파일 존재 확인용
#include <algorithm>    // [STEP 24] std::clamp
#include <cmath>
#include <limits>       // [STEP 25 K] quiet_NaN
#include <stdexcept>    // [STEP 25 K]
#include <cstdio>       // [STEP 25 K] std::sscanf (the WPs in the efficiency JSON's description)
#include "BTagEffGroup.h"   // [STEP 25 K] the job's process group for the efficiency-JSON check
#include <TRandom3.h>

// static flag for verbose logging:
static constexpr bool kVerbose = false;

// ============================================================================
// 생성자 (Constructor)
// ============================================================================
// ----------------------------------------------------------------------------
// [STEP4] 환경변수 우선 경로 결정: env가 설정돼 있으면 그 값을, 아니면
// 코드 내 default(Tier3)를 쓴다. 어느 쪽을 썼는지 로그로 명시한다.
// env는 AnalyzerConfig yml의 common.path_* → condor sh의 export →
// 로컬 실행 시엔 쉘에서 직접 export로 통제된다 (docs/changes/STEP_4 참조).
// ----------------------------------------------------------------------------
// [STEP18] envOr() (env-or-hardcoded-default) REMOVED — paths now come
// explicitly from config via cfgpath::resolve() (see ConfigPath.h).

CorrectionsManager::CorrectionsManager(const std::string& runYear,
                                       const std::string& dataEra,
                                       bool isData,
                                       const std::string& sampleName,
                                       bool requireDerivedCorr)
  // [2018] runYear_ 는 아래에서 jsonpog **디렉토리 이름**으로 그대로 쓰인다
  //   (POG/JME/<runYear_>/ 등). 그래서 표기가 하나라도 어긋나면 파일을 못 찾는다.
  //   여기서 정규화해 표준형(`2016preVFP_UL`/`2016postVFP_UL`/`2017_UL`/`2018_UL`)
  //   으로 고정한다. 모르는 연도는 EraConfig 가 FATAL.
  : runYear_(EraConfig::yearForCorr(EraConfig::normalizeYear(runYear)))
  , dataEra_(dataEra)
  , isData_(isData)
  , sampleName_(sampleName)
{
    requireDerived_ = requireDerivedCorr;

    // [STEP18] 경로는 config(yml common.path_*)에서 명시적으로 온다. 코드 default 폐기.
    //   필수: jsonpog (JME/PU, MC·Data 공통), goldenjson (Data 한정)
    //   선택: trigsf / btagrw — config가 null 이면 resolve()가 "" 반환 → 비활성(SF=1)
    //   빈/미설정 env → FATAL(E12); 필수인데 null → FATAL(E13). (ConfigPath.h 참조)
    jsonPath         = cfgpath::resolve("TTHH_JSONPOG_PATH",    "jsonpog-integration",      /*required=*/true);
    goldenJsonPath   = cfgpath::resolve("TTHH_GOLDENJSON_PATH", "GoldenJSON dir",           /*required=*/isData_);
    trigSFPath       = cfgpath::resolve("TTHH_TRIGSF_DIR",      "trigger SF dir",           /*required=*/false);
    btagReweightPath = cfgpath::resolve("TTHH_BTAGRW_JSON",     "b-tag norm reweight JSON", /*required=*/false);
    // [STEP 24] 2024: jsonpog 에 POG/LUM 의 2024 가 없어 우리가 만든 PU weight JSON
    //   (DerivedCorr/PU/2024_Summer24/, tools/stage2/pu_weights.py; docs/DECISIONS.md D-2026-10-05-A).
    //   2016-2018 은 jsonpog 그대로이고 이 env 를 읽지도 않는다(그 연도의 yml·job 은 바뀌지 않는다).
    if (EraConfig::isRun3(EraConfig::normalizeYear(runYear_)))
        puJsonPath = cfgpath::resolve("TTHH_PU_JSON", "2024 PU weight JSON", /*required=*/true);
    // [STEP 25 K] fixed-WP b-tag years (2024; EraConfig::btagMethod): our MC efficiency JSON (yml
    //   path_btag_eff_json; tools/stage7/btag_eff_maps.py). Optional here (null -> "" -> not loaded): --btagsf on
    //   needs it, and the analyzer stops with E52 without it. 2016-2018 (shape method) never read this env.
    if (EraConfig::btagMethod(EraConfig::normalizeYear(runYear_)) == EraConfig::BTagMethod::FixedWP)
        btagEffPath_ = cfgpath::resolve("TTHH_BTAGEFF_JSON", "b-tag efficiency JSON (fixed WP)", /*required=*/false);

    std::cout << "[CorrectionsManager] derived-correction policy: "
              << (requireDerived_ ? "REQUIRED if path given (load fail -> FATAL 50/51)"
                                  : "optional (bootstrap; load fail -> WARN, SF=1)")
              << "; null path -> disabled (SF=1)" << std::endl;

    // [STEP4] 중앙(POG) 보정 입력의 누락/손상은 모드 무관 FATAL(49) —
    // correctionlib의 throw를 여기서 잡아 Condor가 식별 가능한 exit로 변환.
    try {
        loadJME_();          // always load MC JEC/JER; Data only if isData_
        loadRun3Jet_();      // [STEP 24] 2024: jet ID (jetid.json) + jet veto map; Run 2: nothing
        loadPU_();           // always load PU (both MC & Data)
        loadBTag_();         // MC-only b-tag SF (or Data if you wish)
        loadGoldenJSON_();   // only Data
    } catch (const std::exception& e) {
        std::cerr << "[FATAL][CorrectionsManager] central correction load failed: "
                  << e.what() << "\n"
                  << "  -> check TTHH_JSONPOG_PATH / TTHH_GOLDENJSON_PATH / TTHH_PU_JSON (2024) "
                  << "(or yml common.path_*).\n";
        tthh::fatalExit(tthh::CENTRAL_CORR_LOAD_FAIL);
    }

    // 파생 보정 — requireDerived_에 따라 FATAL(47/48) 또는 WARN
    loadTrigger_();
    loadBTagReweight_();
    loadBTagEff_();      // [STEP 25 K] fixed-WP years only (btagEffPath_ is "" otherwise)
}

// ============================================================================
// 소멸자 (Destructor)
//
// [UPDATED] ROOT 파일 닫기 로직 제거됨.
// 이전에는 trigSFFile_ (TFile*)를 닫아야 했으나,
// correctionlib JSON으로 전환하면서 CorrectionSet이 unique_ptr로
// 관리되므로 자동 해제된다.
// ============================================================================
CorrectionsManager::~CorrectionsManager() {
    // unique_ptr<CorrectionSet> 은 자동으로 해제됨
    // (trigSFCSet_, btagReweightCSet_)
}

//--------------------------------------------------------------------------------------------------
// 1) Jet / MET corrections (JEC + JER)
//--------------------------------------------------------------------------------------------------
void CorrectionsManager::loadJME_() {

    const std::string base = jsonPath + "/POG/JME/" + runYear_ + "/";
    const std::string file = base + "jet_jerc.json.gz";
    if (kVerbose) std::cout << "[loadJME] Loading JME from " << file << "\n";

    auto cset = correction::CorrectionSet::from_file(file);
    if (kVerbose) {
        // compound()가 반환하는 map의 key들(= correction 이름)을 찍어봅니다.
        std::cout << "[loadJME] available corrections:\n";
        for (const auto &kv : cset->compound()) {
            std::cout << "  - " << kv.first << "\n";
        }
    }

    // 1) MC key
    std::string key_mc, key_res, key_sf;
    if (runYear_ == "2016preVFP_UL" || runYear_ == "2016postVFP_UL") {
        key_mc  = "Summer19UL16_V7_MC_L1L2L3Res_AK4PFchs";
        key_res = "Summer19UL16_JRV2_MC_PtResolution_AK4PFchs";
        key_sf  = "Summer19UL16_JRV2_MC_ScaleFactor_AK4PFchs";
    }
    else if (runYear_ == "2017_UL") {
        key_mc  = "Summer19UL17_V5_MC_L1L2L3Res_AK4PFchs";
        key_res = "Summer19UL17_JRV2_MC_PtResolution_AK4PFchs";
        key_sf  = "Summer19UL17_JRV2_MC_ScaleFactor_AK4PFchs";
    }
    else if (runYear_ == "2018_UL") {
        key_mc  = "Summer19UL18_V5_MC_L1L2L3Res_AK4PFchs";
        key_res = "Summer19UL18_JRV2_MC_PtResolution_AK4PFchs";
        key_sf  = "Summer19UL18_JRV2_MC_ScaleFactor_AK4PFchs";
    }
    else if (runYear_ == "2024_Summer24") {
        // [STEP 24] payload 실측(2026-10-02, PLAN §9.2): JEC Summer24Prompt24_V1 (compound
        //   (JetA, JetEta, JetPt, Rho, JetPhi)), JER 는 2023 BPix 의 것(Summer23BPixPrompt23_RunD_JRV1,
        //   SF 입력 (JetEta, JetPt, systematic)) — 이 jet_jerc.json.gz 에 함께 들어 있다.
        key_mc  = "Summer24Prompt24_V1_MC_L1L2L3Res_AK4PFPuppi";
        key_res = "Summer23BPixPrompt23_RunD_JRV1_MC_PtResolution_AK4PFPuppi";
        key_sf  = "Summer23BPixPrompt23_RunD_JRV1_MC_ScaleFactor_AK4PFPuppi";
    }
    else {
        throw std::runtime_error("Unsupported runYear in loadJME_: " + runYear_);
    }


    // 2) 항상 MC용 JEC/JER 로드
    jec_MC_  = cset->compound().at(key_mc);
    jerRes_  = cset->at(key_res);
    jerSF_   = cset->at(key_sf);
    // [STEP 24] 입력 순서를 payload 의 이름에서 (모르는 이름이면 throw -> FATAL 49)
    jecInMC_  = mapJetInputs_(jec_MC_->inputs(), key_mc);
    jerSFIn_  = mapJetInputs_(jerSF_->inputs(), key_sf);
    jerResIn_ = mapJetInputs_(jerRes_->inputs(), key_res);

    if (kVerbose) {
        std::cout
            << "  -> JEC_MC key  : " << key_mc  << "\n"
            << "  -> JER_Res key : " << key_res << "\n"
            << "  -> JER_SF key  : " << key_sf  << "\n";
    }

    // 3) 데이터면 Data용 JEC도 로드
    std::string key_jec_used = key_mc;   // [STEP 24] for the provenance line below
    if (isData_) {
        std::string key_data;
        if (runYear_ == "2016preVFP_UL" || runYear_ == "2016postVFP_UL") {
            std::string eraTag;
            if      (dataEra_ == "B" || dataEra_ == "C" || dataEra_ == "D")
                eraTag = "BCD";
            else if (dataEra_ == "E" || dataEra_ == "F")
                eraTag = "EF";
            else if (dataEra_ == "G" || dataEra_ == "H")
                eraTag = "GH";
            else
                eraTag = dataEra_;
            key_data = "Summer19UL16APV_RunFGH_V7_DATA_L1L2L3Res_AK4PFchs";
        }
        else if (runYear_ == "2017_UL") {
            key_data = "Summer19UL17_Run" + dataEra_ + "_V5_DATA_L1L2L3Res_AK4PFchs";
        }
        else if (runYear_ == "2018_UL") {
            // [2018] Summer19UL18 은 RunA/RunB/RunC/RunD 를 각각 제공한다.
            //   이전 코드는 `(dataEra_=="A") ? "A" : "D"` 로 축약해서 **B, C 에
            //   RunD JEC 를 적용**하고 있었다. era 문자를 그대로 쓴다.
            key_data = "Summer19UL18_Run" + EraConfig::jecDataEraTag("2018", dataEra_)
                     + "_V5_DATA_L1L2L3Res_AK4PFchs";
        }
        else if (runYear_ == "2024_Summer24") {
            // [STEP 24] era 마다 tag 가 따로 없다: residual 이 입력 `run` 으로 갈린다
            key_data = "Summer24Prompt24_V1_DATA_L1L2L3Res_AK4PFPuppi";
        }

        // [2018] Data 인데 키를 못 정했다면 (미지원 연도) 그냥 두면 아래 catch 가
        //   MC JEC 로 조용히 대체해버린다. 명시적으로 끊는다.
        if (key_data.empty()) {
            std::cerr << "\n[FATAL][CorrectionsManager] No Data JEC key for runYear_='"
                      << runYear_ << "'.\n";
            tthh::fatalExit(tthh::CONFIG_BAD_RUNINFO);
        }

        try {
            jec_Data_ = cset->compound().at(key_data);
            jecInData_ = mapJetInputs_(jec_Data_->inputs(), key_data);
            key_jec_used = key_data;
            if (kVerbose)
                std::cout << "  -> JEC_Data key: " << key_data << "\n";
        } catch (const std::exception& e) {
            // [2018] 예전에는 여기서 MC JEC 로 fallback 하고 WARNING 만 찍었다.
            //   Data 에 MC JEC 를 쓰면 jet energy scale 이 통째로 틀리는데
            //   로그 한 줄 말고는 아무 증상이 없다. Data 는 FATAL 로 바꾼다.
            std::cerr << "\n[FATAL][CorrectionsManager] Data JEC key not found: "
                      << key_data << " (" << e.what() << ")\n"
                      << "  Refusing to fall back to MC JEC for Data — that would\n"
                      << "  apply the wrong jet energy scale with no other symptom.\n"
                      << "  Check runYear/eraName and the jet_jerc.json.gz content.\n";
            tthh::fatalExit(tthh::CONFIG_BAD_RUNINFO);
        }
    }
    // [STEP 24] the JEC/JER payload names in every job log (provenance; tools/stage3/smoke_2024.sh reads it)
    std::cout << "[CorrectionsManager] JEC/JER (" << runYear_ << "): " << file << " -> JEC " << key_jec_used
              << ", JER " << key_res << " + " << key_sf << "\n";
}

//--------------------------------------------------------------------------------------------------
// 2) Pileup reweighting
//--------------------------------------------------------------------------------------------------
void CorrectionsManager::loadPU_() {
    if (runYear_ == "2024_Summer24") {
        // [STEP 24] 우리 JSON (생성자에서 TTHH_PU_JSON 으로 받은 경로), correction 이름은
        //   tools/stage2/pu_weights.py 의 CORR_NAME. 입력은 jsonpog 와 같다 (NumTrueInteractions, weights).
        auto cs = correction::CorrectionSet::from_file(puJsonPath);
        puCorr_ = cs->at("Collisions24_goldenJSON");
        std::cout << "[CorrectionsManager] PU weights (2024): " << puJsonPath
                  << " -> Collisions24_goldenJSON" << std::endl;
        return;
    }
    const std::string file = jsonPath + "/POG/LUM/" + runYear_ + "/puWeights.json.gz";
    if (kVerbose) std::cout << "[loadPU] Loading from " << file << "\n";
    auto cset = correction::CorrectionSet::from_file(file);

    std::string key_pu;
    if      (runYear_ == "2016preVFP_UL" || runYear_ == "2016postVFP_UL")
        key_pu = "Collisions16_UltraLegacy_goldenJSON";
    else if (runYear_ == "2017_UL")
        key_pu = "Collisions17_UltraLegacy_goldenJSON";
    else if (runYear_ == "2018_UL")
        key_pu = "Collisions18_UltraLegacy_goldenJSON";
    else
        throw std::runtime_error("Unsupported runYear in loadPU_: " + runYear_);

    puCorr_ = cset->at(key_pu);
    if (kVerbose) std::cout << "  -> PU key: " << key_pu << "\n";
}

//--------------------------------------------------------------------------------------------------
// 3) B-tag SF (POG correctionlib: deepJet_shape, deepJet_comb, deepJet_incl)
//--------------------------------------------------------------------------------------------------
void CorrectionsManager::loadBTag_() {
    if (isData_) return; // 데이터에는 b-tag SF 적용하지 않음
    const std::string yr = EraConfig::normalizeYear(runYear_);
    if (EraConfig::btagMethod(yr) == EraConfig::BTagMethod::FixedWP) {
        // [STEP 25 K] 2024: the fixed-WP payload (D-2026-10-08-A). Used only by the fixed-WP b-tag weight, which also
        //   needs our efficiency JSON: a missing file / correction / unknown input / an evaluation that throws is
        //   a WARN here (recorded in btagFixedWPError_), so a run that does not ask for the weight (--btagsf off: the
        //   first look, btagtrig for the efficiency maps) does not depend on the preliminary payload; --btagsf on
        //   then stops with E40 (the analyzer's setSFflags, with this message).
        const EraConfig::BTagFixedWPPayload p = EraConfig::btagFixedWPPayload(yr);
        const std::string file = jsonPath + "/POG/BTV/" + runYear_ + "/" + p.file;
        try {
            auto cset = correction::CorrectionSet::from_file(file);
            btagCorr_fixedWP_ = cset->at(p.correction);
            btagFixedWPbOnly_ = p.bJetsOnly;
            btagFixedWPInNames_.clear();
            for (const auto& v : btagCorr_fixedWP_->inputs()) {
                const std::string& n = v.name();
                if (n != "systematic" && n != "working_point" && n != "flavor" && n != "abseta" && n != "pt")
                    throw std::runtime_error(std::string(p.correction) + " has an input this code does not know: '" +
                                             n + "' (known: systematic, working_point, flavor, abseta, pt)");
                btagFixedWPInNames_.push_back(n);
            }
            // the payload must evaluate on the whole domain the analyzer feeds it (getBTagSF_WP clamps |eta| into
            //   [0, 2.4999] and pT into [20, 599.9]): central at L and M on a grid of that domain (throws otherwise)
            static const double gridEta[] = {0.0, 1.25, 2.4999};
            static const double gridPt[]  = {20.0, 55.0, 599.9};
            static const char*  gridWP[]  = {"L", "M"};
            auto gridEval = [&](const std::string& sy) {
                for (const char* wp : gridWP) for (double ae : gridEta) for (double pt : gridPt) evalFixedWP_(sy, wp, 5, ae, pt);
            };
            gridEval("central");
            auto works = [&](const std::string& sy) {
                try { gridEval(sy); return true; } catch (const std::exception&) { return false; }
            };
            // a variation counts only if it differs from central somewhere on the grid: a 'systematic' category with a
            //   default would let any key evaluate (and give central)
            auto differs = [&](const std::string& sy) {
                for (const char* wp : gridWP) for (double ae : gridEta) for (double pt : gridPt)
                    if (std::abs(evalFixedWP_(sy, wp, 5, ae, pt) - evalFixedWP_("central", wp, 5, ae, pt)) > 1.0e-9)
                        return true;
                return false;
            };
            btagFixedWPSrcKeys_.clear();
            std::string varText;
            if (works("up") && works("down") && (differs("up") || differs("down"))) {
                btagFixedWPVar_ = WPVar::UpDown;
                varText = "the payload's up/down";
            } else {
                // no total: BTV's correlated + uncorrelated split if the payload has both (their quadrature sum is the
                //   total), else the 2026-10-02 inventory's kinfit sources (PLAN 9.2), each as up_<s>/down_<s> or
                //   <s>_up/<s>_down -- never both sets (a breakdown of one would be counted twice)
                auto pairOf = [&](const char* sr) -> std::pair<std::string, std::string> {
                    const std::string a = std::string("up_") + sr, b = std::string("down_") + sr;
                    const std::string c = std::string(sr) + "_up", d = std::string(sr) + "_down";
                    if (works(a) && works(b) && (differs(a) || differs(b))) return {a, b};
                    if (works(c) && works(d) && (differs(c) || differs(d))) return {c, d};
                    return {"", ""};
                };
                const auto pc = pairOf("correlated"), pu = pairOf("uncorrelated");
                if (!pc.first.empty() && !pu.first.empty()) {
                    btagFixedWPSrcKeys_ = {pc, pu};
                } else {
                    static const char* srcs[] = {"fsrdef", "hdamp", "isrdef", "jer", "jes", "mass", "statistic", "tune"};
                    for (const char* sr : srcs) {
                        const auto k = pairOf(sr);
                        if (!k.first.empty()) btagFixedWPSrcKeys_.push_back(k);
                    }
                }
                btagFixedWPVar_ = btagFixedWPSrcKeys_.empty() ? WPVar::None : WPVar::Sources;
                if (btagFixedWPVar_ == WPVar::Sources) {
                    varText = "sources in quadrature:";
                    for (const auto& k : btagFixedWPSrcKeys_) varText += " " + k.first + "/" + k.second;
                } else {
                    varText = "none found (up/down = central)";
                    std::cerr << "[CorrectionsManager][WARN] b-tag SF (" << runYear_ << ", fixed WP): " << p.correction
                              << " has no up/down variation this code recognises (up/down, correlated/uncorrelated, or "
                                 "the kinfit sources, each differing from central): bTagWeight_up/_down = central"
                              << std::endl;
                }
            }
            btagFixedWPError_.clear();
            std::cout << "[CorrectionsManager] b-tag SF (" << runYear_ << "): fixed WP (method 1a with L and M), "
                      << file << " -> " << p.correction
                      << (p.bJetsOnly ? " (b jets; c and light jets: SF 1, no payload yet)" : "")
                      << "; up/down: " << varText << std::endl;
        } catch (const std::exception& e) {
            btagCorr_fixedWP_.reset();
            btagFixedWPInNames_.clear();
            btagFixedWPSrcKeys_.clear();
            btagFixedWPVar_ = WPVar::None;
            btagFixedWPError_ = file + " -> " + p.correction + ": " + e.what();
            std::cerr << "[CorrectionsManager][WARN] b-tag SF (" << runYear_ << ", fixed WP): " << btagFixedWPError_
                      << " -- not loaded: the fixed-WP b-tag weight cannot be made (--btagsf on stops, E"
                      << tthh::CENTRAL_CORR_LOAD_FAIL << ")" << std::endl;
        }
        return;
    }
    if (!EraConfig::hasBTagShapeSF(yr)) {
        // [STEP 24] 2024 BTV 에는 deepJet 도 UParTAK4 의 shape SF 도 없다(PLAN §9.6 D10):
        //   불러오지 않고 getBTagSF_* 는 1 을 돌려준다. --btagsf on 은 analyzer 가 FATAL 로 막는다.
        std::cout << "[CorrectionsManager] b-tag SF: none for " << runYear_
                  << " (no shape SF in the BTV payload; D10) -> 1.0" << std::endl;
        return;
    }

    const std::string file = jsonPath + "/POG/BTV/" + runYear_ + "/btagging.json.gz";
    if (kVerbose) std::cout << "[loadBTag] Loading from " << file << "\n";
    
    auto cset = correction::CorrectionSet::from_file(file);
    
    // Shape correction (continuous discriminant 사용)
    btagCorr_shape_ = cset->at("deepJet_shape");
    
    // Fixed WP corrections
    btagCorr_bc_    = cset->at("deepJet_comb");   // b/c jets
    btagCorr_light_ = cset->at("deepJet_incl");   // light jets
    
    if (kVerbose) {
        std::cout << "  -> Loaded: deepJet_shape, deepJet_comb, deepJet_incl\n";
    }

}

//--------------------------------------------------------------------------------------------------
// 4) Golden JSON  (only for Data)
//--------------------------------------------------------------------------------------------------
void CorrectionsManager::loadGoldenJSON_() {
    if (!isData_) return;

    // [2018] 파일명은 EraConfig 에서 가져온다.
    //   이전에는 run range `Cert_294927-306462_` 가 **문자열에 하드코딩**되어
    //   있고 뒤쪽 suffix 만 연도별로 갈아끼웠다. run range 는 연도마다 다르므로
    //   2017 을 뺀 나머지는 존재하지 않는 파일명을 만들었다. 게다가 unknown
    //   runYear_ 일 때 `return` 만 하고 넘어가서 Data 가 golden 필터 없이
    //   돌 수 있었다 (아래 파일 열기 실패 FATAL 도 우회됨).
    //   EraConfig::normalizeYear() 는 모르는 연도에 FATAL 이다.
    const std::string yearPlain = EraConfig::normalizeYear(runYear_);
    std::string txt = goldenJsonPath + "/" + runYear_ + "/"
                    + EraConfig::goldenJsonFile(yearPlain);

    if (kVerbose) std::cout << "[loadGoldenJSON] Loading from " << txt << "\n";
    std::ifstream in(txt);
    if (!in) {
        if (isData_) {   // [STEP4] Data가 golden 필터 없이 도는 사고 차단
            std::cerr << "[FATAL][CorrectionsManager] GoldenJSON not found for Data: "
                      << txt << "\n"
                      << "  -> TTHH_GOLDENJSON_PATH (yml common.path_goldenjson)를 확인하세요.\n";
            tthh::fatalExit(tthh::GOLDENJSON_DATA_MISSING);
        }
        std::cerr << "[loadGoldenJSON] ERROR opening " << txt << "\n";
        return;
    }
    nlohmann::json j; in >> j;
    for (auto& it : j.items()) {
        int run = std::stoi(it.key());
        for (auto& rng : it.value()) {
            goldenMask_[run].emplace_back(rng[0], rng[1]);
            if (kVerbose)
                std::cout << "  -> JSON run " << run
                          << " [" << rng[0] << "," << rng[1] << "]\n";
        }
    }
}

//--------------------------------------------------------------------------------------------------
// [STEP 24] Run 3 jet ID and jet veto map (2024)
//   jetid.json.gz       : AK4PUPPI_Tight, AK4PUPPI_TightLeptonVeto; inputs (eta, chHEF, neHEF, chEmEF,
//                         neEmEF, muEF, chMultiplicity, neMultiplicity, multiplicity), the last three int
//                         (payload 실측 2026-10-02; 파일 설명 "Run3 Rereco2022CDE", JetID13p6TeV rev 18 —
//                         2024 전용 기준이 따로 없는지는 JME 로 확인할 것, PLAN §9.2)
//   jetvetomaps.json.gz : EraConfig::jetVetoMap(year): correction + type; inputs (type, eta, phi),
//                         eta [-5.191, 5.191] 82 bin, phi [-pi, pi] 72 bin
//--------------------------------------------------------------------------------------------------
void CorrectionsManager::loadRun3Jet_() {
    const std::string year = EraConfig::normalizeYear(runYear_);
    if (EraConfig::jetIdFromJson(year)) {
        const std::string file = jsonPath + "/POG/JME/" + runYear_ + "/jetid.json.gz";
        auto cs = correction::CorrectionSet::from_file(file);
        jetIdTight_         = cs->at("AK4PUPPI_Tight");
        jetIdTightLepVeto_  = cs->at("AK4PUPPI_TightLeptonVeto");
        std::cout << "[CorrectionsManager] jet ID (" << runYear_ << "): " << file
                  << " -> AK4PUPPI_Tight, AK4PUPPI_TightLeptonVeto" << std::endl;
    }
    const EraConfig::JetVetoMap vm = EraConfig::jetVetoMap(year);
    if (vm.active) {
        const std::string file = jsonPath + "/POG/JME/" + runYear_ + "/jetvetomaps.json.gz";
        auto cs = correction::CorrectionSet::from_file(file);
        jetVetoMap_  = cs->at(vm.correction);
        jetVetoType_ = vm.type;
        std::cout << "[CorrectionsManager] jet veto map (" << runYear_ << "): " << file << " -> "
                  << vm.correction << " type " << vm.type << std::endl;
    }
}

namespace {
// jetid.json 의 int 입력에는 variant 의 정수 자리를 준다 (correctionlib 판마다 int / int64_t)
using CorrInt = std::variant_alternative_t<0, correction::Variable::Type>;

double evalJetId(const std::shared_ptr<const correction::Correction>& c, const char* what,
                 double eta, double chHEF, double neHEF, double chEmEF, double neEmEF,
                 double muEF, int chMult, int neMult) {
    if (!c) {
        std::cerr << "\n[FATAL][CorrectionsManager] " << what << " asked, but no jet ID payload is loaded "
                  << "for this year (EraConfig::jetIdFromJson is false). Aborting." << std::endl;
        tthh::fatalExit(tthh::CONFIG_BAD_RUNINFO);
    }
    return c->evaluate({std::abs(eta), chHEF, neHEF, chEmEF, neEmEF, muEF,
                        CorrInt(chMult), CorrInt(neMult), CorrInt(chMult + neMult)});
}
}  // namespace

bool CorrectionsManager::passJetIdTight(double eta, double chHEF, double neHEF, double chEmEF,
                                        double neEmEF, double muEF, int chMult, int neMult) const {
    return evalJetId(jetIdTight_, "passJetIdTight", eta, chHEF, neHEF, chEmEF, neEmEF, muEF, chMult, neMult) > 0.5;
}

bool CorrectionsManager::passJetIdTightLepVeto(double eta, double chHEF, double neHEF, double chEmEF,
                                               double neEmEF, double muEF, int chMult, int neMult) const {
    return evalJetId(jetIdTightLepVeto_, "passJetIdTightLepVeto", eta, chHEF, neHEF, chEmEF, neEmEF, muEF,
                     chMult, neMult) > 0.5;
}

bool CorrectionsManager::inJetVetoMap(double eta, double phi) const {
    if (!jetVetoMap_) {
        std::cerr << "\n[FATAL][CorrectionsManager] inJetVetoMap asked, but no veto map is loaded for "
                  << runYear_ << ". Aborting." << std::endl;
        tthh::fatalExit(tthh::CONFIG_BAD_RUNINFO);
    }
    // the map's edges are |eta| <= 5.191 and |phi| <= pi: keep the inputs inside
    const double e = std::clamp(eta, -5.19, 5.19);
    const double p = std::clamp(phi, -3.1415, 3.1415);
    return jetVetoMap_->evaluate({jetVetoType_, e, p}) != 0.0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// 5) Trigger SF  [UPDATED: ROOT TH2 → correctionlib JSON]
//
// DeriveSF.cpp가 생성한 trigger_sf.json.gz를 correctionlib으로 로드한다.
//
// JSON 내 correction 목록:
//   "triggerSF"     → central SF 값 (필수)
//   "triggerSF_err" → SF error 값 (optional, systematic ±1σ 계산용)
//
// inputs (evaluate에 넘기는 순서):
//   nbJets (int)  : b-tagged jet 개수
//   eta    (real) : 6th jet |η|  (Config::useEta가 false면 사용되지 않지만 인자는 존재)
//   ht     (real) : scalar HT [GeV]
//   pt     (real) : 6th jet pT [GeV]
//
// [이전 방식과의 차이점]
//   이전: ScaleFactors_YEAR.root에서 TH2 한 장 로드 (HT × jet6pt, nBjets/eta 구분 없음)
//   현재: trigger_sf.json.gz에서 correctionlib 로드 (nbJets × eta × HT × pt, 4D 지원)
//         error 값도 별도 correction으로 포함되어 systematic variation 가능
// ═══════════════════════════════════════════════════════════════════════════════
void CorrectionsManager::loadTrigger_() {
    if (isData_) return; // Data에는 trigger SF 적용하지 않음

    // [파일 경로] trigSFPath 디렉토리 아래의 trigger_sf.json.gz
    // DeriveSF.cpp의 출력 파일명은 Config::sfOutputJSON (= "trigger_sf.json.gz")
    if (trigSFPath.empty()) {   // [STEP18] config null -> trigger SF disabled
        std::cout << "[CorrectionsManager] trigger SF DISABLED (config null) -> SF=1\n";
        return;
    }
    std::string fileName = trigSFPath + "/trigger_sf.json.gz";

    if (kVerbose) std::cout << "[loadTrigger] Loading from " << fileName << "\n";

    // 파일 존재 확인 (filesystem)
    namespace fs = std::filesystem;
    if (!fs::exists(fileName)) {
        if (requireDerived_) {   // [STEP4] main/debug: silent SF=1 진행 금지
            std::cerr << "[FATAL][CorrectionsManager] Trigger SF JSON not found: "
                      << fileName << "\n"
                      << "  -> main/debug 모드는 trigger SF가 필수입니다.\n"
                      << "  -> DeriveSF로 생성하거나 TTHH_TRIGSF_DIR"
                      << " (yml common.path_trigsf_dir)를 확인하세요.\n";
            tthh::fatalExit(tthh::TRIGSF_LOAD_FAIL);
        }
        std::cerr << "[CorrectionsManager][WARN] Trigger SF JSON not found: "
                  << fileName << "\n"
                  << "  -> Trigger SF will NOT be applied (bootstrap mode).\n"
                  << "  -> Run DeriveSF first to generate this file.\n";
        return;
    }

    try {
        // correctionlib은 .json과 .json.gz 모두 지원
        trigSFCSet_ = correction::CorrectionSet::from_file(fileName);

        // (1) Central SF correction (필수)
        trigSFCorr_ = trigSFCSet_->at("triggerSF");

        // (1b) [STEP 26 L, 2026-10-09] year check. DeriveSF writes "year=<YYYY>; reference=<HLT path>; hadronic OR:
        //   ..." into the description of triggerSF since STEP 26. A JSON made for another year is refused: a 2024
        //   job pointed at the 2017 directory would otherwise apply the 2017 SF without any sign. A JSON without the
        //   tag was written before STEP 26, when only 2017 had a trigger SF, so it is accepted for 2017 only.
        {
            const std::string jobYear = EraConfig::normalizeYear(runYear_);
            const std::string desc    = trigSFCorr_->description();
            std::string tagYear;
            const auto p = desc.find("year=");
            if (p != std::string::npos) {
                const auto e = desc.find_first_of("; \t\n", p + 5);
                tagYear = desc.substr(p + 5, e == std::string::npos ? std::string::npos : e - (p + 5));
            }
            const bool sameYear = tagYear.empty() ? (jobYear == "2017") : (tagYear == jobYear);
            if (!sameYear) {
                std::cerr << "[FATAL][CorrectionsManager] trigger SF JSON " << fileName << " was made for "
                          << (tagYear.empty() ? std::string("2017 (no 'year=' tag: written before STEP 26)")
                                              : "year " + tagYear)
                          << ", this job is " << jobYear << ".\n"
                          << "  -> yml common.path_trigsf_dir (TTHH_TRIGSF_DIR) must point at the " << jobYear
                          << " DeriveSF output (TriggerStudy/run_analysis.sh --year " << jobYear << ").\n";
                tthh::fatalExit(tthh::TRIGSF_LOAD_FAIL);
            }
            std::cout << "[CorrectionsManager] trigger SF JSON made for "
                      << (tagYear.empty() ? std::string("2017 (no year tag: written before STEP 26)")
                                          : "year " + tagYear)
                      << ", job year " << jobYear << ": OK\n";
        }

        // (2) SF error correction (optional)
        //     JSON에 "triggerSF_err"이 포함되어 있으면 로드.
        //     없으면 trigSFErrCorr_는 nullptr로 유지되고,
        //     getTriggerSF()에서 syst != 0 요청 시 경고를 출력한다.
        try {
            trigSFErrCorr_ = trigSFCSet_->at("triggerSF_err");
        } catch (const std::exception&) {
            trigSFErrCorr_ = nullptr;
            std::cerr << "[CorrectionsManager][WARN] 'triggerSF_err' correction not found in JSON.\n"
                      << "  -> Systematic variations (±1σ) will NOT be available.\n"
                      << "  -> Update DeriveSF to include error correction in JSON.\n";
        }

        // 로드 성공 로그
        std::cout << "[CorrectionsManager] Loaded trigger SF JSON: " << fileName << "\n"
                  << "  -> 'triggerSF' correction loaded (central SF)\n";
        if (trigSFErrCorr_) {
            std::cout << "  -> 'triggerSF_err' correction loaded (SF error for ±1σ)\n";
        }

        // inputs 목록 출력 (디버깅용)
        if (kVerbose) {
            const auto& inputs = trigSFCorr_->inputs();
            std::cout << "  -> inputs (" << inputs.size() << "): ";
            for (const auto& inp : inputs) {
                std::cout << inp.name() << " ";
            }
            std::cout << "\n";
        }

    } catch (const std::exception& e) {
        if (requireDerived_) {   // [STEP4]
            std::cerr << "[FATAL][CorrectionsManager] Failed to load trigger SF JSON: "
                      << e.what() << "\n";
            tthh::fatalExit(tthh::TRIGSF_LOAD_FAIL);
        }
        std::cerr << "[CorrectionsManager][ERROR] Failed to load trigger SF JSON: "
                  << e.what() << "\n"
                  << "  -> Trigger SF will NOT be applied.\n";
        trigSFCorr_.reset();
        trigSFErrCorr_.reset();
    }
}

//--------------------------------------------------------------------------------------------------
// 6) JEC Uncertainty (only for MC, Normally we do not apply JEC uncertainty into data)
//--------------------------------------------------------------------------------------------------
void CorrectionsManager::loadJECUncertainty_() {
    // 경로 및 파일 설정 (기존 loadJME_와 동일한 파일 사용 가정)
    const std::string base = jsonPath + "/POG/JME/" + runYear_ + "/";
    const std::string file = base + "jet_jerc.json.gz";
    auto cset = correction::CorrectionSet::from_file(file);

    std::string key_unc;
    // Run Year에 따른 Key 설정 (예시 패턴, 실제 JSON 키 확인 필요)
    if (runYear_ == "2016preVFP_UL" || runYear_ == "2016postVFP_UL") {
        key_unc = "Summer19UL16_V7_MC_Total_AK4PFchs"; 
    } else if (runYear_ == "2017_UL") {
        key_unc = "Summer19UL17_V5_MC_Total_AK4PFchs";
    } else if (runYear_ == "2018_UL") {
        key_unc = "Summer19UL18_V5_MC_Total_AK4PFchs";
    } else {
        throw std::runtime_error("Unsupported runYear for JEC Unc: " + runYear_);
    }

    jec_Unc_ = cset->at(key_unc);

    if (kVerbose) {
        std::cout << "[loadJECUnc] key: " << key_unc << "\n";
    }
}

//--------------------------------------------------------------------------------------------------
// 7) B-tag Normalization Reweight
//
// makeReweightJSON이 생성한 correctionlib-schema-v2 JSON을 로드한다.
// JSON 내부 correction 이름: "btagNormReweight"
//
// JSON이 2D (nJets × HT)인지 1D (nJets)인지는 inputs 개수로 자동 판별:
//   - inputs.size() == 3: 1D (systematic, process, nJets)
//   - inputs.size() == 4: 2D (systematic, process, nJets, HT)
//--------------------------------------------------------------------------------------------------
void CorrectionsManager::loadBTagReweight_() {
    // Data에는 적용하지 않음
    if (isData_) return;
    if (btagReweightPath.empty()) {   // [STEP18] config null -> b-tag reweight disabled
        std::cout << "[CorrectionsManager] b-tag norm reweight DISABLED (config null) -> 1.0\n";
        return;
    }

    // 파일 존재 확인 (filesystem)
    namespace fs = std::filesystem;
    if (!fs::exists(btagReweightPath)) {
        if (requireDerived_) {   // [STEP4] main/debug: silent SF=1 진행 금지
            std::cerr << "[FATAL][CorrectionsManager] B-tag reweight JSON not found: "
                      << btagReweightPath << "\n"
                      << "  -> main/debug 모드는 b-tag norm reweight가 필수입니다.\n"
                      << "  -> makeReweightJSON으로 생성하거나 TTHH_BTAGRW_JSON"
                      << " (yml common.path_btag_reweight_json)를 확인하세요.\n";
            tthh::fatalExit(tthh::BTAGRW_LOAD_FAIL);
        }
        std::cerr << "[CorrectionsManager][WARN] B-tag reweight JSON not found: "
                  << btagReweightPath << "\n"
                  << "  -> B-tag normalization reweight will NOT be applied (bootstrap mode).\n"
                  << "  -> Run makeReweightJSON first to generate this file.\n";
        return;
    }

    try {
        // CorrectionSet::from_file은 .json, .json.gz 모두 지원
        btagReweightCSet_ = correction::CorrectionSet::from_file(btagReweightPath);
        btagReweight_     = btagReweightCSet_->at("btagNormReweight");

        // 2D vs 1D 자동 판별: inputs 개수로 결정
        // 1D: {systematic, process, nJets}       → 3개
        // 2D: {systematic, process, nJets, HT}   → 4개
        const auto& inputs = btagReweight_->inputs();
        btagReweightIs2D_ = (inputs.size() >= 4);

        std::cout << "[CorrectionsManager] Loaded b-tag reweight JSON: "
                  << btagReweightPath << "\n"
                  << "  -> Mode: " << (btagReweightIs2D_ ? "2D (nJets × HT)" : "1D (nJets only)")
                  << ", inputs: " << inputs.size() << "\n";

        // sampleName 유효성 검사 (경고만, FATAL은 아님)
        if (sampleName_.empty()) {
            std::cerr << "[CorrectionsManager][WARN] sampleName is empty. "
                      << "getBTagReweight() will return 1.0 unless process name is corrected.\n";
        }

    } catch (const std::exception& e) {
        if (requireDerived_) {   // [STEP4]
            std::cerr << "[FATAL][CorrectionsManager] Failed to load b-tag reweight JSON: "
                      << e.what() << "\n";
            tthh::fatalExit(tthh::BTAGRW_LOAD_FAIL);
        }
        std::cerr << "[CorrectionsManager][ERROR] Failed to load b-tag reweight JSON: "
                  << e.what() << "\n"
                  << "  -> B-tag normalization reweight will NOT be applied.\n";
        btagReweight_.reset();
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// [STEP 25 K] b-tag efficiency JSON of the fixed-WP method (2024; tools/stage7/btag_eff_maps.py)
//   corrections: "btag_eff" (inputs group, flavor, working_point, abseta, pt -> efficiency of our MC) and
//   "btag_eff_groups" (input group -> 1 for a group the maps have, else 0).
//   Not given (yml null): nothing loaded -- the analyzer stops with E52 if --btagsf on asks for the weight.
//   Given but unreadable: E52 in main/debug (requireDerived_), a WARN in btagtrig/prescan (the weight is not
//   used there).
// ═══════════════════════════════════════════════════════════════════════════════
void CorrectionsManager::loadBTagEff_() {
    if (isData_) return;
    if (btagEffPath_.empty()) {
        if (EraConfig::btagMethod(EraConfig::normalizeYear(runYear_)) == EraConfig::BTagMethod::FixedWP)
            std::cout << "[CorrectionsManager] b-tag efficiency JSON (fixed WP): not given (config null) -- "
                         "the fixed-WP b-tag weight cannot be made (--btagsf on stops, E52)\n";
        return;
    }
    try {
        btagEffCSet_   = correction::CorrectionSet::from_file(btagEffPath_);
        btagEff_       = btagEffCSet_->at("btag_eff");
        btagEffGroups_ = btagEffCSet_->at("btag_eff_groups");
        btagEffInNames_.clear();
        for (const auto& v : btagEff_->inputs()) {
            const std::string& n = v.name();
            if (n != "group" && n != "flavor" && n != "working_point" && n != "abseta" && n != "pt")
                throw std::runtime_error("btag_eff has an input this code does not know: '" + n + "'");
            btagEffInNames_.push_back(n);
        }
        // the WPs the maps were made with (tools/stage7/btag_eff_maps.py writes 'wp=L:<l>,M:<m>,T:<t>' into the
        //   description): they must be this year's (EraConfig::btagWP), or the bins of the weight are not the maps'
        const std::string desc = btagEff_->description();
        const auto wpos = desc.find("wp=L:");
        float wl = 0.f, wm = 0.f, wt = 0.f;
        if (wpos == std::string::npos || std::sscanf(desc.c_str() + wpos, "wp=L:%f,M:%f,T:%f", &wl, &wm, &wt) != 3)
            throw std::runtime_error("btag_eff has no 'wp=L:<l>,M:<m>,T:<t>' in its description (not made by "
                                     "tools/stage7/btag_eff_maps.py?)");
        const EraConfig::BTagWP ywp = EraConfig::btagWP(EraConfig::normalizeYear(runYear_));
        if (std::abs(wl - ywp.loose) > 1.0e-5f || std::abs(wm - ywp.medium) > 1.0e-5f || std::abs(wt - ywp.tight) > 1.0e-5f)
            throw std::runtime_error("made with the WPs L=" + std::to_string(wl) + " M=" + std::to_string(wm) + " T=" +
                                     std::to_string(wt) + ", this year's are L=" + std::to_string(ywp.loose) + " M=" +
                                     std::to_string(ywp.medium) + " T=" + std::to_string(ywp.tight));
        // the flavours whose maps passed the tag-bin rule (btag_eff_maps.py --flavours-required; no efficiency at 0 or
        //   1): every flavour the payload gives an SF for must be among them (2024: b)
        if (btagCorr_fixedWP_) {
            const auto fpos = desc.find("flavours_required=");
            const std::string fr = (fpos == std::string::npos) ? std::string()
                                 : desc.substr(fpos + 18, desc.find(';', fpos) == std::string::npos
                                                              ? std::string::npos : desc.find(';', fpos) - fpos - 18);
            const std::vector<std::string> need = btagFixedWPbOnly_ ? std::vector<std::string>{"b"}
                                                                    : std::vector<std::string>{"b", "c", "l"};
            for (const auto& n : need)
                if (fr.find(n) == std::string::npos)
                    throw std::runtime_error("the maps were not checked for flavour '" + n + "' (description "
                                             "flavours_required='" + fr + "'; tools/stage7/btag_eff_maps.py "
                                             "--flavours-required)");
        }
        // every flavour and WP of this job's group and of 'all' must evaluate, inside the maps and outside (clamp),
        //   to an efficiency in [0, 1]; and the groups correction must answer for this job's group (a JSON that loads
        //   but misses a key would otherwise stop the job in the event loop)
        const std::string grp = tthh::btagEffGroup(sampleName_);
        btagEffGroups_->evaluate({grp});
        static const double pts[][2] = {{0.1, 25.0}, {1.3, 80.0}, {2.4, 700.0}, {3.0, 5000.0}};   // (|eta|, pT)
        for (const std::string& g : {grp, std::string("all")})
            for (int f : {5, 4, 0})
                for (const char* wp : {"L", "M", "T"})
                    for (const auto& q : pts) {
                        const double e = evalBTagEff_(g, f, q[0], q[1], wp);
                        if (!(e >= 0.0 && e <= 1.0))
                            throw std::runtime_error("btag_eff(" + g + ", " + std::to_string(f) + ", " + wp + ", |eta| " +
                                                     std::to_string(q[0]) + ", pt " + std::to_string(q[1]) + ") = " +
                                                     std::to_string(e) + ", not in [0, 1]");
                    }
        std::cout << "[CorrectionsManager] b-tag efficiency JSON (fixed WP): " << btagEffPath_
                  << " -> btag_eff, btag_eff_groups (WPs L=" << wl << " M=" << wm << " T=" << wt << ")" << std::endl;
    } catch (const std::exception& ex) {
        btagEff_.reset();
        btagEffGroups_.reset();
        if (requireDerived_) {
            std::cerr << "\n[FATAL][E" << tthh::BTAGEFF_LOAD_FAIL << "][CorrectionsManager] b-tag efficiency JSON "
                      << btagEffPath_ << ": " << ex.what() << "\n"
                      << "  -> make it with tools/stage7/btag_eff_maps.py, or set yml common.path_btag_eff_json\n"
                      << std::endl;
            tthh::fatalExit(tthh::BTAGEFF_LOAD_FAIL);
        }
        std::cerr << "[CorrectionsManager][WARN] b-tag efficiency JSON " << btagEffPath_ << ": " << ex.what()
                  << " -- not loaded (bootstrap mode; the fixed-WP weight is not used here)\n";
    }
}


//--------------------------------------------------------------------------------------------------
//  API implementations
//--------------------------------------------------------------------------------------------------
double CorrectionsManager::getPUWeight(double nTrueInt,
                                       const std::string& var) const
{
    if (kVerbose) std::cout << "[getPUWeight] nTrueInt=" << nTrueInt
                            << " var=" << var << "\n";
    return puCorr_->evaluate({nTrueInt, var});
}

// [STEP 24] payload 의 입력 이름 -> 우리 값의 자리. JEC compound: JetA, JetEta, JetPt, Rho (Run 2),
//   + JetPhi (2024), + run (2024 DATA). JER SF: JetEta, systematic (Run 2), + JetPt (2023 BPix JER).
std::vector<CorrectionsManager::JetIn>
CorrectionsManager::mapJetInputs_(const std::vector<correction::Variable>& vars, const std::string& what) {
    std::vector<JetIn> out;
    for (const auto& v : vars) {
        const std::string n = v.name();
        if      (n == "JetA")       out.push_back(JetIn::A);
        else if (n == "JetEta")     out.push_back(JetIn::Eta);
        else if (n == "JetPt")      out.push_back(JetIn::Pt);
        else if (n == "Rho")        out.push_back(JetIn::Rho);
        else if (n == "JetPhi")     out.push_back(JetIn::Phi);
        else if (n == "run")        out.push_back(v.type() == correction::Variable::VarType::integer ? JetIn::RunInt : JetIn::Run);
        else if (n == "systematic") out.push_back(JetIn::Syst);
        else throw std::runtime_error("unknown input '" + n + "' of " + what);
    }
    return out;
}

double CorrectionsManager::getJEC(
                                  double eta,
                                  double phi,
                                  double raw_pt,
                                  double area,
				  double rho,
                                  unsigned int run) const
{
    if (kVerbose) std::cout << "[getJEC] -----  "
                            << " isData="<<isData_
                            << " eta="<<eta
                            << " raw_pt="<<raw_pt
                            << " area="<<area
			    << " rho="<<rho<<"\n";
    auto &corr = isData_ ? jec_Data_ : jec_MC_;
    const auto &order = isData_ ? jecInData_ : jecInMC_;
    std::vector<correction::Variable::Type> in;
    in.reserve(order.size());
    for (JetIn k : order) {
        switch (k) {
            case JetIn::A:   in.emplace_back(area);   break;
            case JetIn::Eta: in.emplace_back(eta);    break;
            case JetIn::Pt:  in.emplace_back(raw_pt); break;
            case JetIn::Rho: in.emplace_back(rho);    break;
            case JetIn::Phi: in.emplace_back(phi);    break;
            case JetIn::Run: in.emplace_back(static_cast<double>(run)); break;   // payload: run real
            case JetIn::RunInt: in.emplace_back(std::variant_alternative_t<0, correction::Variable::Type>(run)); break;
            case JetIn::Syst: throw std::runtime_error("JEC compound with a systematic input");
        }
    }
    return corr->evaluate(in);
}


// --- Helpers (put in CorrectionsManager.cc top or anonymous namespace) ---
static inline double wrapPhi(double x) {
    while (x >  M_PI) x -= 2.0*M_PI;
    while (x <= -M_PI) x += 2.0*M_PI;
    return x;
}

// splitmix64 for deterministic mixing (good quality + fast)
static inline uint64_t splitmix64(uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

static inline uint32_t makeJetSeed(unsigned int run,
                                  unsigned int lumi,
                                  unsigned long long event,
                                  int jetIndex) {
    uint64_t x = 0;
    x ^= splitmix64(static_cast<uint64_t>(run));
    x ^= splitmix64(static_cast<uint64_t>(lumi) << 1);
    x ^= splitmix64(static_cast<uint64_t>(event) << 2);
    x ^= splitmix64(static_cast<uint64_t>(static_cast<uint32_t>(jetIndex)) << 3);
    return static_cast<uint32_t>(x & 0xFFFFFFFFu);
}


// ============================================================================
//  Preferred API (CMS-like): good-match scaling else stochastic (SF>1 only),
//  with per-jet deterministic seed (run,lumi,event,jetIndex)
// ============================================================================
double CorrectionsManager::smearJER(double corr_pt,
                                   double eta,
                                   double phi,
                                   double rho,
                                   unsigned int run,
                                   unsigned int lumi,
                                   unsigned long long event,
                                   int jetIndex,
                                   double gen_pt,
                                   double gen_eta,
                                   double gen_phi,
                                   const std::string& syst) const
{
    if (isData_) return corr_pt;
    if (corr_pt <= 0.0) return corr_pt;

    const double etaIn = eta;

    double sf = 1.0, res = 0.0;

    // 1) JER scale factor
    try {
        if (kVerbose) std::cout << "[smearJER] jerSF_->evaluate({eta, syst}) -> {"
                                << etaIn << ", " << syst << "}\n";
        // [STEP 24] 입력은 payload 순서대로: Run 2 (JetEta, systematic), 2024 (JetEta, JetPt, systematic)
        std::vector<correction::Variable::Type> in;
        for (JetIn k : jerSFIn_) {
            if      (k == JetIn::Eta)  in.emplace_back(etaIn);
            else if (k == JetIn::Pt)   in.emplace_back(corr_pt);
            else if (k == JetIn::Syst) in.emplace_back(syst);
            else throw std::runtime_error("unexpected input of the JER scale factor");
        }
        sf = jerSF_->evaluate(in);
        if (kVerbose) std::cout << "[smearJER] jerSF returned " << sf << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[smearJER] ERROR in jerSF_->evaluate: " << e.what() << "\n";
        throw;
    }

    // 2) JER resolution
    try {
        if (kVerbose) std::cout << "[smearJER] jerRes_->evaluate({eta, corr_pt, rho}) -> {"
                                << etaIn << ", " << corr_pt << ", " << rho << "}\n";
        // [STEP 24] inputs by the payload's names (Run 2 and 2024: JetEta, JetPt, Rho -- the order as before)
        std::vector<correction::Variable::Type> rin;
        for (JetIn k : jerResIn_) {
            if      (k == JetIn::Eta) rin.emplace_back(etaIn);
            else if (k == JetIn::Pt)  rin.emplace_back(corr_pt);
            else if (k == JetIn::Rho) rin.emplace_back(rho);
            else throw std::runtime_error("unexpected input of the JER resolution");
        }
        res = jerRes_->evaluate(rin);
        if (kVerbose) std::cout << "[smearJER] jerRes returned " << res << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[smearJER] ERROR in jerRes_->evaluate: " << e.what() << "\n";
        throw;
    }

    // 3) Try scaling method first (requires good gen match)
    bool useScaling = false;
    double smeared_pt = corr_pt;

    if (gen_pt > 0.0) {
        double dR = std::hypot(eta - gen_eta, wrapPhi(phi - gen_phi));
        double dPtRel = std::abs(corr_pt - gen_pt);
        bool matched = (dR < 0.2) && (dPtRel < 3.0 * res * corr_pt);

        if (matched) {
            smeared_pt = std::max(0.0,
                gen_pt + sf * (corr_pt - gen_pt));
            useScaling = true;
        }
    }

    // 4) Stochastic smearing (only if SF > 1 and no gen match)
    if (!useScaling) {
        if (sf > 1.0) {
            double sigma = res * std::sqrt(sf * sf - 1.0);
            uint32_t seed = makeJetSeed(run, lumi, event, jetIndex);
            TRandom3 rng(seed);
            smeared_pt = corr_pt * (1.0 + rng.Gaus(0, sigma));
            smeared_pt = std::max(0.0, smeared_pt);
        }
    }

    if (kVerbose) {
        std::cout << "[smearJER] corr_pt=" << corr_pt
                  << " sf=" << sf << " res=" << res
                  << " method=" << (useScaling ? "scaling" : "stochastic")
                  << " -> smeared_pt=" << smeared_pt << "\n";
    }

    return smeared_pt;
}

// ============================================================================
// Backward-compatible overload (uses eventID as seed)
// ============================================================================
double CorrectionsManager::smearJER(double corr_pt,
                                   double gen_pt,
                                   double eta,
                                   double rho,
                                   unsigned int eventID,
                                   const std::string& syst) const
{
    return smearJER(corr_pt, eta,
                    /*phi=*/0.0, rho,
                    /*run=*/0u,
                    /*lumi=*/0u,
                    /*event=*/static_cast<unsigned long long>(eventID),
                    /*jetIndex=*/0,
                    /*gen_pt=*/gen_pt,
                    /*gen_eta=*/0.0,
                    /*gen_phi=*/0.0,
                    syst);
}


// ═══════════════════════════════════════════════════════════════════════════════
// Trigger SF  [UPDATED: correctionlib JSON 기반]
//
// DeriveSF.cpp가 생성한 trigger_sf.json.gz에서 SF를 평가한다.
//
// JSON correction 스키마:
//   "triggerSF"     → evaluate({nbJets, eta, ht, pt}) → central SF
//   "triggerSF_err" → evaluate({nbJets, eta, ht, pt}) → SF error
//
// syst = 0.0: central (SF 그대로)
// syst = +1.0: SF + error (상방 변동)
// syst = -1.0: SF - error (하방 변동)
//
// correctionlib 내부에서 flow="clamp" 설정이 되어 있으므로,
// edge 바깥의 값은 가장 가까운 edge의 값으로 자동 clamping된다.
// 따라서 별도의 범위 검사는 필요 없다.
//
// [이전 API와의 차이]
//   이전: getTriggerSF(double ht, double jet6pt, double syst)
//   현재: getTriggerSF(int nbJets, double eta, double ht, double pt, double syst)
//         → nbJets와 eta가 추가됨 (4D binning 지원)
// ═══════════════════════════════════════════════════════════════════════════════
double CorrectionsManager::getTriggerSF(int nbJets, double eta, double ht, double pt,
                                        double syst) const {
    // Data이거나 correction이 로드되지 않았으면 1.0 반환
    if (isData_ || !trigSFCorr_) return 1.0;

    try {
        // (1) Central SF 평가
        //     inputs 순서: nbJets (int), eta (real), ht (real), pt (real)
        //     이 순서는 DeriveSF.cpp의 BuildCorrectionSet에서 정의한 inputs 배열과 일치해야 함.
        double sf = trigSFCorr_->evaluate({nbJets, eta, ht, pt});

        // (2) Systematic variation (±1σ)
        //     syst != 0이면 error correction에서 uncertainty를 가져온다.
        if (syst != 0.0 && trigSFErrCorr_) {
            double err = trigSFErrCorr_->evaluate({nbJets, eta, ht, pt});
            sf += syst * err;
        }

        // (3) 안전장치: SF가 0 이하면 1.0 처리
        //     물리적으로 trigger SF는 항상 양수여야 한다.
        if (sf <= 0.0) return 1.0;

        return sf;

    } catch (const std::exception& e) {
        // [STEP7.5] main/debug: 평가 실패의 silent 1.0 진행 금지 (요구 12)
        if (requireDerived_) {
            std::cerr << "[FATAL][getTriggerSF] evaluation failed: " << e.what()
                      << "\n  nbJets=" << nbJets << " eta=" << eta
                      << " ht=" << ht << " pt=" << pt << " syst=" << syst
                      << "\n  -> trigger SF JSON과 입력 정의가 맞는지 확인.\n";
            tthh::fatalExit(tthh::TRIGSF_LOAD_FAIL);
        }
        // bootstrap(btagtrig): 첫 수회 에러만 출력 (스팸 방지)
        static int errCount = 0;
        if (errCount++ < 5) {
            std::cerr << "[getTriggerSF] Error: " << e.what()
                      << " nbJets=" << nbJets
                      << " eta=" << eta
                      << " ht=" << ht
                      << " pt=" << pt
                      << " syst=" << syst << std::endl;
        }
        return 1.0;
    }
}


// ───────────────────────────────────────────────────────────────────────────
// Fixed WP 방식: pass/fail 기준
// ───────────────────────────────────────────────────────────────────────────
double CorrectionsManager::getBTagSF_FixedWP(int hadFlav, double absEta, double pt,
                                              const std::string& wp,
                                              const std::string& syst) const {
    if (isData_) return 1.0;
    if (!btagCorr_bc_ || !btagCorr_light_) return 1.0;   // [STEP 24] 그 연도의 payload 에 없다 (2024)
    
    // Flavor 정규화: 5=b, 4=c, 나머지=0(light)
    int flav = hadFlav;
    if (flav != 5 && flav != 4) flav = 0;
    
    // pT 범위 제한 (POG 권장)
    double pt_clamped = std::clamp(pt, 20.0, 1000.0);
    double absEta_clamped = std::clamp(absEta, 0.0, 2.4999);
    
    try {
        if (flav == 5 || flav == 4) {
            // b/c jets: deepJet_comb
            return btagCorr_bc_->evaluate({syst, wp, flav, absEta_clamped, pt_clamped});
        } else {
            // light jets: deepJet_incl
            return btagCorr_light_->evaluate({syst, wp, absEta_clamped, pt_clamped});
        }
    } catch (const std::exception& e) {
        std::cerr << "[getBTagSF_FixedWP] Error: " << e.what()
                  << " flav=" << flav << " absEta=" << absEta_clamped
                  << " pt=" << pt_clamped << " wp=" << wp
                  << " syst=" << syst << std::endl;
        return 1.0;
    }
}

// ───────────────────────────────────────────────────────────────────────────
// [STEP 25 K] fixed-WP method (2024): the payload SF and our MC efficiency
// ───────────────────────────────────────────────────────────────────────────
// the payload's inputs in the order it declares them (load checks the names)
double CorrectionsManager::evalFixedWP_(const std::string& syst, const std::string& wp, int flav,
                                        double absEta, double pt) const {
    std::vector<correction::Variable::Type> args;
    args.reserve(btagFixedWPInNames_.size());
    for (const auto& n : btagFixedWPInNames_) {
        if      (n == "systematic")    args.emplace_back(syst);
        else if (n == "working_point") args.emplace_back(wp);
        else if (n == "flavor")        args.emplace_back(flav);
        else if (n == "abseta")        args.emplace_back(absEta);
        else                           args.emplace_back(pt);           // "pt"
    }
    return btagCorr_fixedWP_->evaluate(args);
}

double CorrectionsManager::getBTagSF_WP(int hadFlav, double absEta, double pt, const std::string& wp,
                                        const std::string& syst) const {
    if (isData_ || !btagCorr_fixedWP_) return 1.0;
    const int flav = (hadFlav == 5 || hadFlav == 4) ? hadFlav : 0;
    if (btagFixedWPbOnly_ && flav != 5) return 1.0;                    // no c / light SF in the payload yet
    // the 2024 kinfit binning (PLAN 9.2): |eta| < 2.5 in one bin, pT 20-600 GeV in 8 bins -> clamp into it
    const double ae = std::clamp(std::abs(absEta), 0.0, 2.4999);
    const double p  = std::clamp(pt, 20.0, 599.9);
    try {
        const double c = evalFixedWP_("central", wp, flav, ae, p);
        if (syst == "central") return c;
        if (syst != "up" && syst != "down")
            throw std::runtime_error("syst must be central, up or down (got '" + syst + "')");
        const bool up = (syst == "up");
        switch (btagFixedWPVar_) {
            case WPVar::UpDown:
                return evalFixedWP_(up ? "up" : "down", wp, flav, ae, p);
            case WPVar::Sources: {
                double s2 = 0.0;
                for (const auto& k : btagFixedWPSrcKeys_) {
                    const double v = evalFixedWP_(up ? k.first : k.second, wp, flav, ae, p);
                    s2 += (v - c) * (v - c);
                }
                return up ? c + std::sqrt(s2) : c - std::sqrt(s2);
            }
            default:
                return c;
        }
    } catch (const std::exception& e) {
        // never a silent 1: the central SF of a b jet is the correction itself
        std::cerr << "\n[FATAL][E" << tthh::CENTRAL_CORR_LOAD_FAIL << "][getBTagSF_WP] " << e.what()
                  << " (flav=" << flav << " |eta|=" << ae << " pt=" << p << " wp=" << wp << " syst=" << syst << ")\n"
                  << std::endl;
        tthh::fatalExit(tthh::CENTRAL_CORR_LOAD_FAIL);
    }
}

// the efficiency of one jet (throws on an evaluation error; getBTagEff turns that into E52)
double CorrectionsManager::evalBTagEff_(const std::string& group, int hadFlav, double absEta, double pt,
                                        const std::string& wp) const {
    const int flav = (hadFlav == 5 || hadFlav == 4) ? hadFlav : 0;
    std::vector<correction::Variable::Type> args;
    args.reserve(btagEffInNames_.size());
    for (const auto& n : btagEffInNames_) {
        if      (n == "group")         args.emplace_back(group);
        else if (n == "flavor")        args.emplace_back(flav);
        else if (n == "working_point") args.emplace_back(wp);
        else if (n == "abseta")        args.emplace_back(std::abs(absEta));
        else                           args.emplace_back(pt);           // "pt" (the maps clamp)
    }
    return btagEff_->evaluate(args);
}

double CorrectionsManager::getBTagEff(const std::string& group, int hadFlav, double absEta, double pt,
                                      const std::string& wp) const {
    if (!btagEff_) return std::numeric_limits<double>::quiet_NaN();
    try {
        return evalBTagEff_(group, hadFlav, absEta, pt, wp);
    } catch (const std::exception& e) {
        // never an abort (an uncaught exception ends the job with 134 and no code) and never a silent value
        std::cerr << "\n[FATAL][E" << tthh::BTAGEFF_LOAD_FAIL << "][getBTagEff] " << btagEffPath_ << ": " << e.what()
                  << " (group=" << group << " flav=" << hadFlav << " |eta|=" << absEta << " pt=" << pt << " wp=" << wp
                  << ")\n" << std::endl;
        tthh::fatalExit(tthh::BTAGEFF_LOAD_FAIL);
    }
}

bool CorrectionsManager::btagEffHasGroup(const std::string& group) const {
    if (!btagEffGroups_) return false;
    try {
        return btagEffGroups_->evaluate({group}) > 0.5;
    } catch (const std::exception& e) {
        std::cerr << "\n[FATAL][E" << tthh::BTAGEFF_LOAD_FAIL << "][btagEffHasGroup] " << btagEffPath_ << ": "
                  << e.what() << " (group=" << group << ")\n" << std::endl;
        tthh::fatalExit(tthh::BTAGEFF_LOAD_FAIL);
    }
}

// ───────────────────────────────────────────────────────────────────────────
// Shape Correction 방식: continuous discriminant
// ───────────────────────────────────────────────────────────────────────────
double CorrectionsManager::getBTagSF_Shape(int hadFlav, double eta, double pt, double discr,
                                            const std::string& syst) const {
    if (isData_) return 1.0;
    if (!btagCorr_shape_) return 1.0;   // [STEP 24] 그 연도의 payload 에 shape SF 가 없다 (2024, D10)
    
    // Flavor 정규화: 5=b, 4=c, 나머지=0(light)
    int flav = hadFlav;
    if (flav != 5 && flav != 4) flav = 0;
    
    // 값 범위 제한 (POG 권장)
    // deepJet_shape expects abseta (|eta|), not signed eta
    double abseta_clamped = std::clamp(std::abs(eta), 0.0, 2.4999);
    double pt_clamped  = std::clamp(pt, 20.0, 1000.0);
    double discr_clamped = std::clamp(discr, 0.0, 1.0);
    
    // ═══════════════════════════════════════════════════════════════════════
    // Systematic flavor 제약 (Systematic flavor constraints)
    //
    // Shape correction에서 각 flavor는 특정 systematic만 사용 가능:
    //   b-jet (5): central, up_hf, down_hf, up_hfstats1, down_hfstats1, 
    //              up_hfstats2, down_hfstats2, up_lfstats1, down_lfstats1,
    //              up_lfstats2, down_lfstats2, up_lf, down_lf
    //   c-jet (4): central, up_cferr1, down_cferr1, up_cferr2, down_cferr2
    //   light (0): b-jet과 동일
    //
    // 잘못된 조합이 들어오면 "central"로 대체 (fallback)
    // ═══════════════════════════════════════════════════════════════════════
    std::string actual_syst = syst;
    
    // c-jet 전용 systematic 처리
    if (flav == 4) {
        // c-jet은 cferr1, cferr2만 사용 가능
        if (syst.find("cferr") == std::string::npos && syst != "central") {
            actual_syst = "central";
        }
    } else {
        // b/light jet은 cferr 사용 불가
        if (syst.find("cferr") != std::string::npos) {
            actual_syst = "central";
        }
    }
    
    try {
        // evaluate(systematic, flavor, eta, pt, discriminator)
        return btagCorr_shape_->evaluate({actual_syst, flav, abseta_clamped,
                                          pt_clamped, discr_clamped});
    } catch (const std::exception& e) {
        std::cerr << "[getBTagSF_Shape] Error: " << e.what()
                  << " syst=" << actual_syst << " flav=" << flav
                  << " abseta=" << abseta_clamped << " pt=" << pt_clamped
                  << " discr=" << discr_clamped << std::endl;
        return 1.0;
    }
}


// ───────────────────────────────────────────────────────────────────────────
// B-tag Normalization Reweight
//
// correctionlib JSON에서 정규화 비율을 조회한다.
//
// 호출 예시:
//   double r = corrMgr->getBTagReweight("central", nJets, HT);
//
// sampleName_이 비어있거나 JSON에 해당 process가 없으면 1.0 반환 (경고 출력).
// ───────────────────────────────────────────────────────────────────────────
double CorrectionsManager::getBTagReweight(const std::string& systematic,
                                           const std::string& processKey,
                                           int nJets,
                                           double HT) const {
    // Data이거나 reweight JSON이 로드되지 않았으면 1.0 반환
    if (isData_ || !btagReweight_) return 1.0;

    // processKey가 비어있으면 lookup 불가 — fallback 1.0
    if (processKey.empty()) {
        static int warnCount = 0;
        if (warnCount++ < 3) {
            std::cerr << "[getBTagReweight] Empty processKey — caller must pass "
                         "TtCatGroup::MakeProcessKey(sampleName, genTtbarId). "
                         "Falling back to 1.0.\n";
        }
        return 1.0;
    }

    try {
        if (btagReweightIs2D_) {
            // ═══════════════════════════════════════════════════════════════
            // 2D 모드 (2D mode): evaluate({systematic, process, nJets, HT})
            // ═══════════════════════════════════════════════════════════════
            // HT가 음수이면 기본값으로 최소 edge 사용 (안전장치)
            double ht_val = (HT >= 0.0) ? HT : 500.0;
            return btagReweight_->evaluate({systematic, processKey,
                                            static_cast<int>(nJets), ht_val});
        } else {
            // ═══════════════════════════════════════════════════════════════
            // 1D 모드 (1D mode): evaluate({systematic, process, nJets})
            // ═══════════════════════════════════════════════════════════════
            return btagReweight_->evaluate({systematic, processKey,
                                            static_cast<int>(nJets)});
        }
    } catch (const std::exception& e) {
        // [STEP7.5] main/debug: 평가 실패의 silent 1.0 진행 금지 (요구 12).
        // 전형적 원인: process 매핑 변경(예: "tt+nb" 신설) 후 JSON 미재생성 —
        // 프로젝트 금지 사항. exe_BTagSF + exe_MakeJSON으로 JSON을 재유도할 것.
        if (requireDerived_) {
            std::cerr << "[FATAL][getBTagReweight] evaluation failed: " << e.what()
                      << "\n  syst=" << systematic << " process=" << processKey
                      << " nJets=" << nJets << " HT=" << HT
                      << "\n  -> processKey가 JSON에 없으면 매핑 변경 후 JSON"
                      << " 재생성 누락입니다 (makeReweightJSON 재실행).\n";
            tthh::fatalExit(tthh::BTAGRW_LOAD_FAIL);
        }
        // bootstrap(btagtrig): 첫 수회 에러만 출력 (스팸 방지)
        static int errCount = 0;
        if (errCount++ < 5) {
            std::cerr << "[getBTagReweight] Error: " << e.what()
                      << " syst=" << systematic
                      << " process=" << processKey
                      << " nJets=" << nJets
                      << " HT=" << HT << std::endl;
        }
        return 1.0;
    }
}


// ============================================================================
// Golden JSON
// ============================================================================
bool CorrectionsManager::passGoldenJSON(int run, int lumi) const {
    if (!isData_) return true; // MC always "passes"
    if (kVerbose) std::cout << "[passGoldenJSON] run="<<run
                            << " lumi="<<lumi<<"\n";
    auto it = goldenMask_.find(run);
    if (it==goldenMask_.end()) return false;
    for (auto &p : it->second) {
        if (lumi>=p.first && lumi<=p.second) return true;
    }
    return false;
}

// ============================================================================
// JEC Uncertainty
// ============================================================================
double CorrectionsManager::getJECUncertainty(double eta, double pt, double area, double rho) const {
    if(isData_) return 0.0;
    return jec_Unc_->evaluate({eta, pt}); 
}
