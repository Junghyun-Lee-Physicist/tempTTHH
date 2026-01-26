// CorrectionsManager.h
#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>

// [중요] ROOT 클래스를 사용하기 위해 포함해야 함
#include <TFile.h>
#include <TH2.h> 

// correctionlib (기존 JEC/JER용)
#include "correction.h"

class CorrectionsManager {
public:
    // runYear: "2016PreVFP_UL", "2016PostVFP_UL", "2017_UL", "2018_UL"
    // dataEra: "B", "C", ..., "F" (only meaningful if isData==true)
    // isData: true->Data, false->MC
    CorrectionsManager(const std::string& runYear,
                       const std::string& dataEra,
                       bool isData);
    ~CorrectionsManager(); // [추가] 소멸자 (ROOT 파일 닫기 위해 필요)

    // API
    double getPUWeight(double nTrueInt,
                       const std::string& var="nominal") const;

//    double getJEC(int run,
    double getJEC(
                  double eta,
                  double raw_pt,
                  double area,
		  double rho) const;

//    double smearJER(double corr_pt,
//                    double gen_pt,
//                    double eta,
//                    double rho,
//		    const std::string& syst = "nom") const;
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


    double getTriggerSF(double ht, double jet6pt, double syst = 0.0) const;

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

    // Event-level b-tag weight 계산 (모든 jet에 대해)
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

    bool   passGoldenJSON(int run, int lumi) const;

    double getJECUncertainty(double eta, double pt, double area = 0.0, double rho = 0.0) const;

private:
    void loadJME_();
    void loadPU_();
    void loadBTag_();
    void loadGoldenJSON_();
    void loadJECUncertainty_();
    void loadTrigger_(); // ROOT 파일 로드 함수

    // configuration
    std::string jsonPath;
    std::string goldenJsonPath;
    std::string trigSFPath;
    std::string runYear_;
    std::string dataEra_;
    bool        isData_;

    // corrections
    std::shared_ptr<const correction::CompoundCorrection> jec_MC_;
    std::shared_ptr<const correction::CompoundCorrection> jec_Data_;
    std::shared_ptr<const correction::Correction>         jerRes_;
    std::shared_ptr<const correction::Correction>         jerSF_;
    std::shared_ptr<const correction::Correction>         puCorr_;
    std::shared_ptr<const correction::Correction>         jec_Unc_;
    // B-tag correction objects
    std::shared_ptr<const correction::Correction> btagCorr_shape_;    // deepJet_shape
    std::shared_ptr<const correction::Correction> btagCorr_bc_;       // deepJet_comb (b/c jets)
    std::shared_ptr<const correction::Correction> btagCorr_light_;    // deepJet_incl (light jets)

    TFile* trigSFFile_ = nullptr;
    TH2* hTrigSF_    = nullptr; // TH2F 혹은 TH2D

    // golden JSON mask: run -> list of (lumi_start, lumi_end)
    std::map<int,std::vector<std::pair<int,int>>> goldenMask_;
};
