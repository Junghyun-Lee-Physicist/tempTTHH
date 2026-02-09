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
#include <fstream>
#include <iostream>
#include <filesystem>   // JSON 파일 존재 확인용
#include <TRandom3.h>

// static flag for verbose logging:
static constexpr bool kVerbose = false;

// ============================================================================
// 생성자 (Constructor)
// ============================================================================
CorrectionsManager::CorrectionsManager(const std::string& runYear,
                                       const std::string& dataEra,
                                       bool isData,
                                       const std::string& sampleName)
  : runYear_(runYear)
  , dataEra_(dataEra)
  , isData_(isData)
  , sampleName_(sampleName)
{
////    jsonPath = "/cvmfs/cms.cern.ch/rsync/cms-nanoAOD/jsonpog-integration"; // lxplus
////    goldenJsonPath = "/afs/cern.ch/user/j/junghyun/ttHH_analysis/CMSSW_14_2_1/src/runii_tthhanalyzerV8/GoldenJson"; // lxplus
////    jsonPath = "/Users/jhlee/correctionLib/corrections/jsonpog-integration"; // Local
////    goldenJsonPath = "/Users/jhlee/tempTTHH/GoldenJson"; // Local
    jsonPath = "/cvmfs/cms.cern.ch/rsync/cms-nanoAOD/jsonpog-integration"; // Tier3
    goldenJsonPath = "/u/user/jhlee/ttHH/CMSSW_14_2_1/src/tempTTHH/GoldenJson"; // Tier3

////    trigSFPath = "/Users/jhlee/tempTTHH/Correction/TriggerSF"; // Local
    trigSFPath = "/u/user/jhlee/ttHH/CMSSW_14_2_1/src/tempTTHH/Correction/TriggerSF"; // Tier3

    // b-tag normalization reweight JSON 경로
    // makeReweightJSON이 생성한 파일. 상대경로 또는 절대경로 사용 가능.
    btagReweightPath = "/u/user/jhlee/ttHH/CMSSW_14_2_1/src/tempTTHH/Correction/BTagReweight/btagNormReweight.json"; // Tier3

    std::cout<<"[CorrectionsManager] json library path : "<<jsonPath<<std::endl;

    loadJME_();          // always load MC JEC/JER; Data only if isData_
    loadPU_();           // always load PU (both MC & Data)
    loadBTag_();         // MC-only b-tag SF (or Data if you wish)
    loadGoldenJSON_();   // only Data

    // [UPDATED] Trigger SF 로드 (correctionlib JSON)
    loadTrigger_();

    // B-tag normalization reweight 로드 (MC only)
    loadBTagReweight_();
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
    if (runYear_ == "2016PreVFP_UL" || runYear_ == "2016PostVFP_UL") {
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
    else {
        throw std::runtime_error("Unsupported runYear in loadJME_: " + runYear_);
    }


    // 2) 항상 MC용 JEC/JER 로드
    jec_MC_  = cset->compound().at(key_mc);
    jerRes_  = cset->at(key_res);
    jerSF_   = cset->at(key_sf);

    if (kVerbose) {
        std::cout
            << "  -> JEC_MC key  : " << key_mc  << "\n"
            << "  -> JER_Res key : " << key_res << "\n"
            << "  -> JER_SF key  : " << key_sf  << "\n";
    }

    // 3) 데이터면 Data용 JEC도 로드
    if (isData_) {
        std::string key_data;
        if (runYear_ == "2016PreVFP_UL" || runYear_ == "2016PostVFP_UL") {
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
            std::string tag = (dataEra_ == "A") ? "A" : "D";
            key_data = "Summer19UL18_Run" + tag + "_V5_DATA_L1L2L3Res_AK4PFchs";
        }

        try {
            jec_Data_ = cset->compound().at(key_data);
            if (kVerbose)
                std::cout << "  -> JEC_Data key: " << key_data << "\n";
        } catch (const std::exception& e) {
            std::cerr << "[loadJME] WARNING: Data JEC key " << key_data
                      << " not found (" << e.what() << "). Falling back to MC JEC.\n";
            jec_Data_ = jec_MC_;
        }
    }
}

//--------------------------------------------------------------------------------------------------
// 2) Pileup reweighting
//--------------------------------------------------------------------------------------------------
void CorrectionsManager::loadPU_() {
    const std::string file = jsonPath + "/POG/LUM/" + runYear_ + "/puWeights.json.gz";
    if (kVerbose) std::cout << "[loadPU] Loading from " << file << "\n";
    auto cset = correction::CorrectionSet::from_file(file);

    std::string key_pu;
    if      (runYear_ == "2016PreVFP_UL" || runYear_ == "2016PostVFP_UL")
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

    // build path
    std::map<std::string, std::string> suffixMap = {
        {"2017_UL",        "UL2017_Collisions17"},
        {"2018_UL",        "UL2018_Collisions18"},
        {"2016PreVFP_UL",  "UL2016preVFP_Collisions16"},
        {"2016PostVFP_UL", "UL2016postVFP_Collisions16"}
    };
    auto it = suffixMap.find(runYear_);
    if (it == suffixMap.end()) {
        std::cerr << "[loadGoldenJSON] Unknown runYear_: " << runYear_ << std::endl;
        return;
    }
    std::string suffix = it->second;
    std::string txt = goldenJsonPath + "/"
                    + runYear_ + "/Cert_294927-306462_13TeV_"
                    + suffix + "_GoldenJSON.txt";

    if (kVerbose) std::cout << "[loadGoldenJSON] Loading from " << txt << "\n";
    std::ifstream in(txt);
    if (!in) {
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
    std::string fileName = trigSFPath + "/trigger_sf.json.gz";

    if (kVerbose) std::cout << "[loadTrigger] Loading from " << fileName << "\n";

    // 파일 존재 확인 (filesystem)
    namespace fs = std::filesystem;
    if (!fs::exists(fileName)) {
        std::cerr << "[CorrectionsManager][WARN] Trigger SF JSON not found: "
                  << fileName << "\n"
                  << "  -> Trigger SF will NOT be applied.\n"
                  << "  -> Run DeriveSF first to generate this file.\n";
        return;
    }

    try {
        // correctionlib은 .json과 .json.gz 모두 지원
        trigSFCSet_ = correction::CorrectionSet::from_file(fileName);

        // (1) Central SF correction (필수)
        trigSFCorr_ = trigSFCSet_->at("triggerSF");

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
    if (runYear_ == "2016PreVFP_UL" || runYear_ == "2016PostVFP_UL") {
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

    // 파일 존재 확인 (filesystem)
    namespace fs = std::filesystem;
    if (!fs::exists(btagReweightPath)) {
        std::cerr << "[CorrectionsManager][WARN] B-tag reweight JSON not found: "
                  << btagReweightPath << "\n"
                  << "  -> B-tag normalization reweight will NOT be applied.\n"
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
        std::cerr << "[CorrectionsManager][ERROR] Failed to load b-tag reweight JSON: "
                  << e.what() << "\n"
                  << "  -> B-tag normalization reweight will NOT be applied.\n";
        btagReweight_.reset();
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

//double CorrectionsManager::getJEC(int run,
double CorrectionsManager::getJEC(
                                  double eta,
                                  double raw_pt,
                                  double area,
				  double rho) const
{
    if (kVerbose) std::cout << "[getJEC] -----  "
                            << " isData="<<isData_
                            << " eta="<<eta
                            << " raw_pt="<<raw_pt
                            << " area="<<area
			    << " rho="<<rho<<"\n";
    auto &corr = isData_ ? jec_Data_ : jec_MC_;
    return corr->evaluate({ area, eta, raw_pt, rho});
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
        sf = jerSF_->evaluate({etaIn, syst});
        if (kVerbose) std::cout << "[smearJER] jerSF returned " << sf << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[smearJER] ERROR in jerSF_->evaluate: " << e.what() << "\n";
        throw;
    }

    // 2) JER resolution
    try {
        if (kVerbose) std::cout << "[smearJER] jerRes_->evaluate({eta, corr_pt, rho}) -> {"
                                << etaIn << ", " << corr_pt << ", " << rho << "}\n";
        res = jerRes_->evaluate({etaIn, corr_pt, rho});
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
        // 첫 수회 에러만 출력 (스팸 방지)
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
// Shape Correction 방식: continuous discriminant
// ───────────────────────────────────────────────────────────────────────────
double CorrectionsManager::getBTagSF_Shape(int hadFlav, double eta, double pt, double discr,
                                            const std::string& syst) const {
    if (isData_) return 1.0;
    
    // Flavor 정규화: 5=b, 4=c, 나머지=0(light)
    int flav = hadFlav;
    if (flav != 5 && flav != 4) flav = 0;
    
    // 값 범위 제한 (POG 권장)
    double eta_clamped = std::clamp(eta, -2.4999, 2.4999);
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
        return btagCorr_shape_->evaluate({actual_syst, flav, eta_clamped, 
                                          pt_clamped, discr_clamped});
    } catch (const std::exception& e) {
        std::cerr << "[getBTagSF_Shape] Error: " << e.what()
                  << " syst=" << actual_syst << " flav=" << flav 
                  << " eta=" << eta_clamped << " pt=" << pt_clamped 
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
                                           int nJets,
                                           double HT) const {
    // Data이거나 reweight JSON이 로드되지 않았으면 1.0 반환
    if (isData_ || !btagReweight_) return 1.0;

    // sampleName이 비어있으면 lookup 불가
    if (sampleName_.empty()) return 1.0;

    try {
        if (btagReweightIs2D_) {
            // ═══════════════════════════════════════════════════════════════
            // 2D 모드 (2D mode): evaluate({systematic, process, nJets, HT})
            // ═══════════════════════════════════════════════════════════════
            // HT가 음수이면 기본값으로 최소 edge 사용 (안전장치)
            double ht_val = (HT >= 0.0) ? HT : 500.0;
            return btagReweight_->evaluate({systematic, sampleName_,
                                            static_cast<int>(nJets), ht_val});
        } else {
            // ═══════════════════════════════════════════════════════════════
            // 1D 모드 (1D mode): evaluate({systematic, process, nJets})
            // ═══════════════════════════════════════════════════════════════
            return btagReweight_->evaluate({systematic, sampleName_,
                                            static_cast<int>(nJets)});
        }
    } catch (const std::exception& e) {
        // 첫 수회 에러만 출력 (스팸 방지)
        static int errCount = 0;
        if (errCount++ < 5) {
            std::cerr << "[getBTagReweight] Error: " << e.what()
                      << " syst=" << systematic
                      << " process=" << sampleName_
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
