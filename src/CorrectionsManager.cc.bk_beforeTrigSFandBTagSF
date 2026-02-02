// CorrectionsManager.cc
#include "CorrectionsManager.h"
#include <fstream>
#include <iostream>
#include <TRandom3.h>

// static flag for verbose logging:
static constexpr bool kVerbose = false;

CorrectionsManager::CorrectionsManager(const std::string& runYear,
                                       const std::string& dataEra,
                                       bool isData)
  : runYear_(runYear)
  , dataEra_(dataEra)
  , isData_(isData)
{
////    jsonPath = "/cvmfs/cms.cern.ch/rsync/cms-nanoAOD/jsonpog-integration"; // lxplus
////    goldenJsonPath = "/afs/cern.ch/user/j/junghyun/ttHH_analysis/CMSSW_14_2_1/src/runii_tthhanalyzerV8/GoldenJson"; // lxplus
////    jsonPath = "/Users/jhlee/correctionLib/corrections/jsonpog-integration"; // Local
////    goldenJsonPath = "/Users/jhlee/tempTTHH/GoldenJson"; // Local
    jsonPath = "/cvmfs/cms.cern.ch/rsync/cms-nanoAOD/jsonpog-integration"; // Tier3
    goldenJsonPath = "/u/user/jhlee/ttHH/CMSSW_14_2_1/src/tempTTHH/GoldenJson"; // Tier3

////    trigSFPath = "/Users/jhlee/tempTTHH/Correction/TriggerSF"; // Local
    trigSFPath = "/u/user/jhlee/ttHH/CMSSW_14_2_1/src/tempTTHH/Correction/TriggerSF"; // Tier3

    std::cout<<"[CorrectionsManager] json library path : "<<jsonPath<<std::endl;

    loadJME_();          // always load MC JEC/JER; Data only if isData_
    loadPU_();           // always load PU (both MC & Data)
    loadBTag_();         // MC-only b-tag SF (or Data if you wish)
    loadGoldenJSON_();   // only Data

    // [중요] Trigger SF 로드
    loadTrigger_();
}

// 소멸자 (ROOT 파일 닫기용, 헤더에 ~CorrectionsManager() 선언 필요)
CorrectionsManager::~CorrectionsManager() {
    if (trigSFFile_) {
        trigSFFile_->Close();
        delete trigSFFile_;
    }
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
        if (runYear_ == "2016PreVFP_UL" || runYear_ == "2016PostVFP_UL")
            key_data = "Summer19UL16_Run" + dataEra_ + "_V7_DATA_L1L2L3Res_AK4PFchs";
        else if (runYear_ == "2017_UL")
            key_data = "Summer19UL17_Run" + dataEra_ + "_V5_DATA_L1L2L3Res_AK4PFchs";
        else /* 2018_UL */
            key_data = "Summer19UL18_Run" + dataEra_ + "_V5_DATA_L1L2L3Res_AK4PFchs";

        jec_Data_ = cset->compound().at(key_data);
        if (kVerbose) std::cout << "  -> JEC_Data key: " << key_data << "\n";
    }
}

//--------------------------------------------------------------------------------------------------
// 2) Pileup weights
//--------------------------------------------------------------------------------------------------
void CorrectionsManager::loadPU_() {

    const std::string file = jsonPath + "/POG/LUM/" + runYear_ + "/puWeights.json.gz";
    if (kVerbose) std::cout << "[loadPU] Loading PU from " << file << "\n";
    auto set = correction::CorrectionSet::from_file(file);

    // determine two‐digit year code
    std::string yearCode =
          (runYear_ == "2016PreVFP_UL"  || runYear_ == "2016PostVFP_UL") ? "16"
        : (runYear_ == "2017_UL") ? "17"
        : (runYear_ == "2018_UL") ? "18"
        : throw std::runtime_error("Unsupported runYear in loadPU_: " + runYear_);

    std::string key = "Collisions" + yearCode + "_UltraLegacy_goldenJSON";
    if (kVerbose) std::cout << "  -> PU key    : " << key << "\n";
    puCorr_ = set->at(key);
}

//--------------------------------------------------------------------------------------------------
// 3) b-tag SF  (typically only for MC)
//--------------------------------------------------------------------------------------------------
void CorrectionsManager::loadBTag_() {

    if (isData_) return; // b-tag SF apply MC only

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

void CorrectionsManager::loadTrigger_() {
    if (isData_) return; // 데이터는 SF 적용 안 함

    // [경로 설정] ScaleFactors.root 위치 지정
    // 예: 현재 디렉토리 혹은 특정 경로
    std::string fileName = trigSFPath + "/ScaleFactors_" + runYear_ + ".root"; 
    
    if (kVerbose) std::cout << "[loadTrigger] Opening " << fileName << "\n";

    trigSFFile_ = TFile::Open(fileName.c_str(), "READ");
    if (!trigSFFile_ || trigSFFile_->IsZombie()) {
        std::cerr << "[CorrectionsManager] ERROR: Cannot open " << fileName << std::endl;
        return;
    }

    std::string histName = "SF_Bjet0"; 
    
    hTrigSF_ = (TH2*)trigSFFile_->Get(histName.c_str());
    if (!hTrigSF_) {
        std::cerr << "[CorrectionsManager] ERROR: Histogram '" << histName 
                  << "' not found in " << fileName << std::endl;
        trigSFFile_->Close();
        trigSFFile_ = nullptr;
    } else {
        if (kVerbose) std::cout << "  -> Loaded Trigger SF Histogram: " << histName << "\n";
    }
}

//--------------------------------------------------------------------------------------------------
// 5) JEC Uncertainty (only for MC, Normally we do not apply JEC uncertainty into data)
//--------------------------------------------------------------------------------------------------

void CorrectionsManager::loadJECUncertainty_() {
// 경로 및 파일 설정 (기존 loadJME_와 동일한 파일 사용 가정)
    const std::string base = jsonPath + "/POG/JME/" + runYear_ + "/";
    const std::string file = base + "jet_jerc.json.gz";
    auto cset = correction::CorrectionSet::from_file(file);

    std::string key_unc;
    // Run Year에 따른 Key 설정 (예시 패턴, 실제 JSON 키 확인 필요)
    // JSON 파일 내의 "Total" Uncertainty 키를 찾아서 할당
    if (runYear_ == "2016PreVFP_UL" || runYear_ == "2016PostVFP_UL") {
        key_unc = "Summer19UL16_V7_MC_Total_AK4PFchs"; 
    } else if (runYear_ == "2017_UL") {
        key_unc = "Summer19UL17_V5_MC_Total_AK4PFchs";
    } else if (runYear_ == "2018_UL") {
        key_unc = "Summer19UL18_V5_MC_Total_AK4PFchs";
    }

    try {
        jec_Unc_ = cset->at(key_unc);
        if (kVerbose) std::cout << "  -> JEC_Unc key : " << key_unc << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[CorrectionsManager] Error loading JEC Uncertainty: " << e.what() << std::endl;
        // 필요 시 throw 또는 대체 처리
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
//    if (kVerbose) std::cout << "[getJEC] run="<<run
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

    // (Optional) if your JER JSON uses abseta axis, switch to std::abs(eta)
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

    if (kVerbose) {
        std::cout << "[smearJER] corr_pt=" << corr_pt
                  << " eta=" << eta << " phi=" << phi
                  << " rho=" << rho
                  << " gen_pt=" << gen_pt
                  << " gen_eta=" << gen_eta
                  << " gen_phi=" << gen_phi
                  << " sf=" << sf
                  << " res=" << res << "\n";
    }

    // ---- Decide scaling vs stochastic using "good match" criteria ----
    bool goodMatch = false;
    if (gen_pt > 0.0) {
        const double dEta = eta - gen_eta;
        const double dPhi = wrapPhi(phi - gen_phi);
        const double dR2  = dEta*dEta + dPhi*dPhi;

        // CMS-like: dR < 0.2 AND |pt - genPt| < 3 * res * pt
        if (dR2 < (0.2 * 0.2) && std::abs(corr_pt - gen_pt) < 3.0 * res * corr_pt) {
            goodMatch = true;
        }
    }

    double smearFactor = 1.0;

    if (goodMatch) {
        // Scaling method
        smearFactor = 1.0 + (sf - 1.0) * (corr_pt - gen_pt) / corr_pt;
    } else if (sf > 1.0) {
        // Stochastic method (only if sf > 1)
        const uint32_t seed = makeJetSeed(run, lumi, event, jetIndex);
        TRandom3 rng(seed);

        const double sigma = res * std::sqrt(std::max(sf*sf - 1.0, 0.0));
        smearFactor = 1.0 + rng.Gaus(0.0, sigma);
    } else {
        smearFactor = 1.0; // no smearing if sf <= 1 and no good gen match
    }

    // Protect against negative/too small factor (avoid pathological jets)
    const double minPt = 1e-2;
    const double ptSmeared = corr_pt * smearFactor;
    return std::max(minPt, ptSmeared);
}


// ============================================================================
//  Backward-compatible wrapper (no phi/gen-eta/gen-phi/jetIndex info available)
//  -> cannot do proper dR matching; uses gen_pt>0 as "matched" flag.
//  (Recommend migrating call sites to the preferred API above.)
// ============================================================================
double CorrectionsManager::smearJER(double corr_pt,
                                   double gen_pt,
                                   double eta,
                                   double rho,
                                   unsigned int eventID,
                                   const std::string& syst) const
{
    // Use eventID only + jetIndex=0 (not ideal); treat as "unknown match quality"
    // Here, set gen_eta/gen_phi = 0 and phi = 0 so dR check will likely fail,
    // forcing stochastic unless gen_pt<=0 (or you can bypass dR in this wrapper).
    return smearJER(corr_pt,
                    eta,
                    /*phi=*/0.0,
                    rho,
                    /*run=*/0u,
                    /*lumi=*/0u,
                    /*event=*/static_cast<unsigned long long>(eventID),
                    /*jetIndex=*/0,
                    /*gen_pt=*/gen_pt,
                    /*gen_eta=*/0.0,
                    /*gen_phi=*/0.0,
                    syst);
}


double CorrectionsManager::getTriggerSF(double ht, double jet6pt, double syst) const {
    
    if (isData_ || !hTrigSF_) return 1.0;

    // 1) Bin 찾기
    // HT(X축), Jet6Pt(Y축) 가정 (EventLooper 로직 따름)
    int bin = hTrigSF_->FindBin(ht, jet6pt);

    // 2) 값 가져오기
    double sf = hTrigSF_->GetBinContent(bin);
    double err = hTrigSF_->GetBinError(bin);

    // 3) Systematic 적용 (syst = 1.0이면 +1sigma, -1.0이면 -1sigma)
    if (syst != 0.0) {
        sf += (syst * err);
    }

    // 4) 안전장치 (SF가 0이거나 음수면 1.0 처리 혹은 그대로 반환)
    if (sf <= 0) return 1.0; 

    return sf;
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
            // evaluate(systematic, working_point, flavor, abseta, pt)
            return btagCorr_bc_->evaluate({syst, wp, flav, absEta_clamped, pt_clamped});
        } else {
            // light jets: deepJet_incl
            return btagCorr_light_->evaluate({syst, wp, flav, absEta_clamped, pt_clamped});
        }
    } catch (const std::exception& e) {
        std::cerr << "[getBTagSF_FixedWP] Error: " << e.what() 
                  << " flav=" << flav << " eta=" << absEta_clamped 
                  << " pt=" << pt_clamped << std::endl;
        return 1.0;
    }
}

// ───────────────────────────────────────────────────────────────────────────
// Shape Correction 방식: continuous discriminant
// ───────────────────────────────────────────────────────────────────────────
double CorrectionsManager::getBTagSF_Shape(int hadFlav, double eta, double pt, 
                                            double discr, const std::string& syst) const {
    if (isData_) return 1.0;
    if (!btagCorr_shape_) return 1.0;
    
    // Flavor 정규화
    int flav = hadFlav;
    if (flav != 5 && flav != 4) flav = 0;
    
    // 범위 제한
    double pt_clamped = std::clamp(pt, 20.0, 1000.0);
    double eta_clamped = std::clamp(std::fabs(eta), 0.0, 2.4999);
    double discr_clamped = std::clamp(discr, 0.0, 1.0);
    
    // ═══════════════════════════════════════════════════════════════════════
    // 중요: c-jet과 b/light jet의 systematic 이름이 다름!
    // ═══════════════════════════════════════════════════════════════════════
    std::string actual_syst = syst;
    
    // c-jet 전용 systematic 처리
    if (flav == 4) {
        // c-jet은 cferr1, cferr2만 사용 가능
        // 다른 systematic 요청 시 central 반환
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


bool CorrectionsManager::passGoldenJSON(int run, int lumi) const {
    if (!isData_) return true; // MC always “passes”
    if (kVerbose) std::cout << "[passGoldenJSON] run="<<run
                            << " lumi="<<lumi<<"\n";
    auto it = goldenMask_.find(run);
    if (it==goldenMask_.end()) return false;
    for (auto &p : it->second) {
        if (lumi>=p.first && lumi<=p.second) return true;
    }
    return false;
}

// [추가] Uncertainty 값 반환 (Input 순서는 JSON 파일에 정의된 inputs 순서 확인 필요: 보통 Eta, Pt)
double CorrectionsManager::getJECUncertainty(double eta, double pt, double area, double rho) const {
    if(isData_) return 0.0; // 데이터는 JEC Uncertainty 적용 안함 (보통)
    
    // correctionlib의 inputs 순서가 [eta, pt] 인지 확인 필요. 
    // 예제 코드: inputs = [JetEta, JetPt]
    return jec_Unc_->evaluate({eta, pt}); 
}
