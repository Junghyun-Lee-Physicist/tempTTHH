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

    double smearJER(double corr_pt,
                    double gen_pt,
                    double eta,
                    double rho,
		    const std::string& syst = "nom") const;

    double getTriggerSF(double ht, double jet6pt, double syst = 0.0) const;

    double getBTagSF(int hf,
                     double eta,
                     double pt,
                     double disc,
                     const std::string& sys="central") const;

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
    std::shared_ptr<const correction::Correction>         btagCorr_;
    std::shared_ptr<const correction::Correction>         jec_Unc_;

    TFile* trigSFFile_ = nullptr;
    TH2* hTrigSF_    = nullptr; // TH2F 혹은 TH2D

    // golden JSON mask: run -> list of (lumi_start, lumi_end)
    std::map<int,std::vector<std::pair<int,int>>> goldenMask_;
};
