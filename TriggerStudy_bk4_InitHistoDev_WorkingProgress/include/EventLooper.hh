#ifndef EventLooper_hh
#define EventLooper_hh

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH2D.h>
#include <TH1D.h>

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <iostream>

// Wrapper class for TTree reading (your project-specific reader)
#include "NtupleReader.hh"

// correctionlib (SF provider)
#include "correction.h"

class EventLooper {
public:
    // ========================================================================
    // Constructor & Destructor
    // ========================================================================
    // applySF = false -> Step 1: calculate efficiency maps
    // applySF = true  -> Step 2: apply scale factors (correctionlib)
    EventLooper(bool applySF = false);
    virtual ~EventLooper();

    // ========================================================================
    // Core Functions
    // ========================================================================
    // Init():
    //   - Open input ROOT file
    //   - Retrieve TTree
    //   - Instantiate NtupleReader
    virtual void Init();

    // Loop():
    //   - Build histogram/map objects using BinConfig
    //   - Main event loop:
    //       - selection
    //       - compute weights
    //       - trigger logic
    //       - fill denominator / numerator
    //       - optionally apply SF (Step 2)
    virtual void Loop();

    // ========================================================================
    // Helper functions for file naming
    // ========================================================================
    void setNtupleName(TString _name);
    TString getInputName();
    TString getOutputName();

private:
    // ========================================================================
    // Configuration
    // ========================================================================
    TString ntupleName = "notDefined";
    bool applySFMode = false;

    // ========================================================================
    // Data / MC bookkeeping
    // ========================================================================
    bool isData = true;

    // ========================================================================
    // Input file / tree / reader
    // ========================================================================
    // Keep the input TFile alive during the entire run.
    // If the file is deleted/closed, the TTree can become invalid.
    TFile* inputFile = nullptr;

    // Raw pointer to TTree (owned by inputFile)
    TTree* fChain = nullptr;

    // NtupleReader is a wrapper that does branch setup and provides getters.
    NtupleReader* reader = nullptr;

    // ========================================================================
    // Histograms & Maps
    // ========================================================================
    // Step 1: Output file pointer
    TFile* outputFile = nullptr;

    // Step 2: Efficiency calculation maps (Denominator/Numerator)
    std::map<TString, TH2D*> map_Total;
    std::map<TString, TH2D*> map_Pass;

    // Step 3: CorrectionLib objects for SF application
    std::unique_ptr<correction::CorrectionSet> cset;
    std::shared_ptr<const correction::Correction> sf_provider;
    
    // Step 4: Histograms for 1D Trigger efficiency for validation
    TH1D* h_HT_Total_noTrigSF        = nullptr;
    TH1D* h_HT_Pass_noTrigSF         = nullptr;
    TH1D* h_HT_Pass_TrigSF           = nullptr;

    TH1D* h_pT_Total_noTrigSF        = nullptr;
    TH1D* h_pT_Pass_noTrigSF         = nullptr;
    TH1D* h_pT_Pass_TrigSF           = nullptr;

    TH1D* h_Eta_Total_noTrigSF       = nullptr;
    TH1D* h_Eta_Pass_noTrigSF        = nullptr;
    TH1D* h_Eta_Pass_TrigSF          = nullptr;
 
    TH1D* h_nbJets_Total_noTrigSF    = nullptr;
    TH1D* h_nbJets_Pass_noTrigSF     = nullptr;
    TH1D* h_nbJets_Pass_TrigSF       = nullptr;
    
    // Step 5: Refactoring helpers for histogram init and fill
    void InitHistograms();
    void FillHisograms(
        double ht, double pt, double eta, int nbJets,
        bool passTrig, double weight, double weightSF
    );
    void SaveAndClose();


};

#endif

