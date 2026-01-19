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
    if (!isData_) {
        const std::string file = jsonPath + "/POG/BTV/" + runYear_ + "/btagging.json.gz";
        if (kVerbose) std::cout << "[loadBTag] Loading BTag from " << file << "\n";
        auto set = correction::CorrectionSet::from_file(file);

        const char* key = "deepJet_shape";
        btagCorr_ = set->at(key);
        if (kVerbose) std::cout << "  -> BTag key  : " << key << "\n";
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

double CorrectionsManager::smearJER(double corr_pt,
                                    double gen_pt,
                                    double eta,
                                    double rho,
                                    const std::string& syst) const
{

    if(isData_) return corr_pt;

    double sf = 0, res = 0;
    // 1) JER scale factor
    try {
      // 1) JER scale factor: now pass both inputs (eta, systematic)
      if (kVerbose) std::cout << "[smearJER] calling jerSF_->evaluate({eta, syst}) -> {"
                         << eta << ", " << syst << "}\n";
      sf = jerSF_->evaluate({eta, syst});
      if (kVerbose) std::cout << "[smearJER] jerSF returned " << sf << "\n";
    }
    catch (const std::exception &e) {
      std::cerr << "[smearJER] ERROR in jerSF_->evaluate: " << e.what() << "\n";
      throw;
    }

    // 2) JER resolution
    try {
      if (kVerbose) std::cout << "[smearJER] calling jerRes_->evaluate({eta, corr_pt, rho})\n";
      res = jerRes_->evaluate({eta, corr_pt, rho});
      if (kVerbose) std::cout << "[smearJER] jerRes returned " << res << "\n";
    }
    catch (const std::exception &e) {
      std::cerr << "[smearJER] ERROR in jerRes_->evaluate: " << e.what() << "\n";
      throw;
    }

    if (kVerbose) std::cout << "[smearJER] corr_pt="<<corr_pt
                            << " gen_pt="<<gen_pt
                            << " eta="<<eta
                            << " rho="<<rho
                            << " sf="<<sf
                            << " res="<<res<<"\n";
    if (gen_pt >= 0) {
        return std::max(0.0, gen_pt + sf*(corr_pt - gen_pt));
    } else {
        double gaus   = TRandom3().Gaus(0,1);
        double factor = 1.0 + std::sqrt(sf*sf - 1.0)*res*gaus;
        return std::max(0.0, corr_pt * factor);
    }
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

double CorrectionsManager::getBTagSF(int hf,
                                     double eta,
                                     double pt,
                                     double disc,
                                     const std::string& sys) const
{
    if (!isData_) return 1.0;  // no SF applied for Data
    if (hf!=5 && hf!=4) hf=0;  // light
    if (kVerbose) std::cout << "[getBTagSF] hf="<<hf
                            << " eta="<<eta
                            << " pt="<<pt
                            << " disc="<<disc
                            << " sys="<<sys<<"\n";
    return btagCorr_->evaluate({sys, hf, std::fabs(eta), pt, disc});
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
