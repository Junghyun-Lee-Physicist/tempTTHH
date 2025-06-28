// CorrectionsManager.h
#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include "correction.h"
#include <nlohmann/json.hpp>

class CorrectionsManager {
public:
    // runYear: "2016PreVFP_UL", "2016PostVFP_UL", "2017_UL", "2018_UL"
    // dataEra: "B", "C", ..., "F" (only meaningful if isData==true)
    // isData: true->Data, false->MC
    CorrectionsManager(const std::string& runYear,
                       const std::string& dataEra,
                       bool isData);

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

    double getBTagSF(int hf,
                     double eta,
                     double pt,
                     double disc,
                     const std::string& sys="central") const;
    bool   passGoldenJSON(int run, int lumi) const;

private:
    void loadJME_();
    void loadPU_();
    void loadBTag_();
    void loadGoldenJSON_();

    // configuration
    std::string jsonPath;
    std::string goldenJsonPath;
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

    // golden JSON mask: run -> list of (lumi_start, lumi_end)
    std::map<int,std::vector<std::pair<int,int>>> goldenMask_;
};
