// ============================================================================
// CorrectionsManager.h
//
// 중앙 보정 관리자 (Central Corrections Manager)
// 모든 이벤트 단위 보정을 한 곳에서 로드/평가한다.
//
// 지원 보정 목록 (Supported corrections):
//   1) JEC/JER       — Jet Energy Correction / Resolution
//   2) Pileup        — Pileup reweighting
//   3) B-tag SF      — Shape + Fixed WP (per-jet)
//   4) Golden JSON   — Data only luminosity mask
//   5) Trigger SF    — Hadronic trigger SF (correctionlib JSON)
//                      4D: nbJets × |η| × HT × jet6pT
//                      [UPDATED] ROOT TH2 → correctionlib JSON
//   6) JEC Unc       — JEC Total Uncertainty
//   7) B-tag Norm Reweight — 정규화 보정 (normalization ratio)
//                          1D (nJets) or 2D (nJets × HT)
//                          correctionlib JSON produced by makeReweightJSON
//
// Author: Junghyun Lee
// ============================================================================
#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>

// [중요] ROOT 클래스를 사용하기 위해 포함해야 함
#include <TFile.h>
#include <TH2.h> 

// correctionlib
#include "correction.h"

class CorrectionsManager {
public:
    // ═══════════════════════════════════════════════════════════════════════
    // 생성자 / 소멸자 (Constructor / Destructor)
    // ═══════════════════════════════════════════════════════════════════════
    //
    // runYear: "2016PreVFP_UL", "2016PostVFP_UL", "2017_UL", "2018_UL"
    // dataEra: "B", "C", ..., "F" (only meaningful if isData==true)
    // isData: true->Data, false->MC
    // sampleName: MC 프로세스 이름 (e.g. "TTToHadronic"), b-tag reweight lookup에 사용
    //             Data일 경우 빈 문자열 허용
    CorrectionsManager(const std::string& runYear,
                       const std::string& dataEra,
                       bool isData,
                       const std::string& sampleName = "");
    ~CorrectionsManager();

    // ═══════════════════════════════════════════════════════════════════════
    // Pileup Weight API
    // ═══════════════════════════════════════════════════════════════════════
    double getPUWeight(double nTrueInt,
                       const std::string& var="nominal") const;

    // ═══════════════════════════════════════════════════════════════════════
    // JEC API
    // ═══════════════════════════════════════════════════════════════════════
    double getJEC(
                  double eta,
                  double raw_pt,
                  double area,
		  double rho) const;

    // ═══════════════════════════════════════════════════════════════════════
    // JER Smearing API (두 가지 오버로드)
    // ═══════════════════════════════════════════════════════════════════════

    // Backward-compatible JER smearing API (old call sites)
    double smearJER(double corr_pt,
                    double gen_pt,
                    double eta,
                    double rho,
                    unsigned int eventID,
                    const std::string& syst = "nom") const;
    
    // Preferred API: 결정론적 시드 (deterministic seed) 사용 (run, lumi, event, jetIndex)
    double smearJER(double corr_pt,
                    double eta,
                    double phi,
                    double rho,
                    unsigned int run,
                    unsigned int lumi,
                    unsigned long long event,
                    int jetIndex,
                    double gen_pt = -1.0,
                    double gen_eta = 0.0,
                    double gen_phi = 0.0,
                    const std::string& syst = "nom") const;

    // ═══════════════════════════════════════════════════════════════════════
    // Trigger SF API  [UPDATED: ROOT TH2 → correctionlib JSON]
    //
    // DeriveSF.cpp가 생성한 trigger_sf.json.gz에서 로드.
    //
    // correctionlib JSON 스키마:
    //   correction "triggerSF"     → central SF value
    //   correction "triggerSF_err" → SF uncertainty (error)
    //
    // inputs (correction의 evaluate에 넘기는 인자 순서):
    //   nbJets (int)  : b-tagged jet 개수
    //   eta    (real) : 6th jet |η|
    //   ht     (real) : scalar HT [GeV]
    //   pt     (real) : 6th jet pT [GeV]
    //
    // syst: 0.0 = central, +1.0 = +1σ, -1.0 = -1σ
    //
    // 사용법 (Usage):
    //   double sf   = corrMgr->getTriggerSF(nBJet, jet6Eta, HT, jet6Pt);       // central
    //   double sfUp = corrMgr->getTriggerSF(nBJet, jet6Eta, HT, jet6Pt, +1.0); // +1σ
    //   double sfDn = corrMgr->getTriggerSF(nBJet, jet6Eta, HT, jet6Pt, -1.0); // -1σ
    // ═══════════════════════════════════════════════════════════════════════
    double getTriggerSF(int nbJets, double eta, double ht, double pt,
                        double syst = 0.0) const;

    // ═══════════════════════════════════════════════════════════════════════
    // B-tagging SF API
    // ═══════════════════════════════════════════════════════════════════════
    // Fixed WP 방식 (Medium WP 기준 pass/fail)
    double getBTagSF_FixedWP(int hadFlav, double absEta, double pt,
                             const std::string& wp = "M",
                             const std::string& syst = "central") const;

    // Shape Correction 방식 (continuous discriminant)
    double getBTagSF_Shape(int hadFlav, double eta, double pt, double discr,
                           const std::string& syst = "central") const;

    // Event-level b-tag weight 계산 결과 구조체 (모든 jet에 대한 곱)
    struct BTagWeightResult {
        double central;
        double up_hf;           // b/light: hf variation
        double down_hf;
        double up_lf;           // light: lf variation  
        double down_lf;
        double up_cferr1;       // c-jet: cferr variations
        double down_cferr1;
        double up_cferr2;
        double down_cferr2;
        double up_hfstats1;     // Statistical variations
        double down_hfstats1;
        double up_hfstats2;
        double down_hfstats2;
        double up_lfstats1;
        double down_lfstats1;
        double up_lfstats2;
        double down_lfstats2;
    };

    // ═══════════════════════════════════════════════════════════════════════
    // B-tag Normalization Reweight API
    //
    // b-tag shape SF 적용 후 yield 보존을 위한 정규화 비율 (normalization ratio).
    // makeReweightJSON이 생성한 correctionlib JSON에서 로드.
    //
    // JSON 스키마 (correctionlib v2):
    //   correction name = "btagNormReweight"
    //   inputs: systematic (string), process (string), nJets (int), [HT (real)]
    //   output: normalization ratio (real)
    //
    // 사용법 (Usage):
    //   double r = corrMgr->getBTagReweight("central", 8, 750.0);  // 2D
    //   double r = corrMgr->getBTagReweight("central", 8);          // 1D
    //
    // sampleName은 생성자에서 설정됨 (process 인자로 자동 전달)
    // ═══════════════════════════════════════════════════════════════════════
    double getBTagReweight(const std::string& systematic,
                           int nJets,
                           double HT = -1.0) const;

    // ═══════════════════════════════════════════════════════════════════════
    // Golden JSON API (Data only)
    // ═══════════════════════════════════════════════════════════════════════
    bool   passGoldenJSON(int run, int lumi) const;

    // ═══════════════════════════════════════════════════════════════════════
    // JEC Uncertainty API
    // ═══════════════════════════════════════════════════════════════════════
    double getJECUncertainty(double eta, double pt, double area = 0.0, double rho = 0.0) const;

private:
    // ─── 로딩 함수 (Loading functions) ───
    void loadJME_();
    void loadPU_();
    void loadBTag_();
    void loadGoldenJSON_();
    void loadJECUncertainty_();
    void loadTrigger_();            // [UPDATED] correctionlib JSON에서 trigger SF 로드
    void loadBTagReweight_();       // correctionlib JSON에서 정규화 비율 로드

    // ─── 설정 변수 (Configuration) ───
    std::string jsonPath;
    std::string goldenJsonPath;
    std::string trigSFPath;         // trigger_sf.json.gz가 위치한 디렉토리 경로
    std::string btagReweightPath;   // b-tag reweight JSON 전체 파일 경로
    std::string runYear_;
    std::string dataEra_;
    std::string sampleName_;        // MC 프로세스 이름 (process key for reweight lookup)
    bool        isData_;

    // ─── correctionlib 객체 (Correction objects) ───
    std::shared_ptr<const correction::CompoundCorrection> jec_MC_;
    std::shared_ptr<const correction::CompoundCorrection> jec_Data_;
    std::shared_ptr<const correction::Correction>         jerRes_;
    std::shared_ptr<const correction::Correction>         jerSF_;
    std::shared_ptr<const correction::Correction>         puCorr_;
    std::shared_ptr<const correction::Correction>         jec_Unc_;

    // B-tag correction objects (POG correctionlib)
    std::shared_ptr<const correction::Correction> btagCorr_shape_;    // deepJet_shape
    std::shared_ptr<const correction::Correction> btagCorr_bc_;       // deepJet_comb (b/c jets)
    std::shared_ptr<const correction::Correction> btagCorr_light_;    // deepJet_incl (light jets)

    // B-tag normalization reweight (user-derived correctionlib JSON)
    std::unique_ptr<correction::CorrectionSet>    btagReweightCSet_;
    std::shared_ptr<const correction::Correction> btagReweight_;
    bool btagReweightIs2D_ = false;  // JSON이 2D (nJets × HT)인지 1D (nJets only)인지

    // ─── Trigger SF (correctionlib JSON) [UPDATED from ROOT TH2] ───
    //
    // trigger_sf.json.gz를 CorrectionSet으로 로드한 후:
    //   "triggerSF"     → central SF 값을 담은 correction
    //   "triggerSF_err" → SF error 값을 담은 correction (optional)
    //
    // CorrectionSet은 unique_ptr가 소유하고,
    // 개별 Correction은 shared_ptr로 참조한다.
    std::unique_ptr<correction::CorrectionSet>    trigSFCSet_;
    std::shared_ptr<const correction::Correction> trigSFCorr_;       // central SF
    std::shared_ptr<const correction::Correction> trigSFErrCorr_;    // SF error (±1σ용, optional)

    // golden JSON mask: run -> list of (lumi_start, lumi_end)
    std::map<int,std::vector<std::pair<int,int>>> goldenMask_;
};
