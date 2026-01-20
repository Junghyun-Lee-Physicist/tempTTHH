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
    // Step 1: Efficiency calculation maps (Denominator/Numerator)
    std::map<TString, TH2D*> map_Total;
    std::map<TString, TH2D*> map_Pass;

    // Step 2: CorrectionLib objects for SF application
    std::unique_ptr<correction::CorrectionSet> cset;
    std::shared_ptr<const correction::Correction> sf_provider;

    // Step 2: Validation 1D histograms (for comparing Total vs Pass)
    TH1D* h_HT_Total      = nullptr;
    TH1D* h_HT_Pass       = nullptr;
    TH1D* h_pT_Total      = nullptr;
    TH1D* h_pT_Pass       = nullptr;
    TH1D* h_Eta_Total     = nullptr;
    TH1D* h_Eta_Pass      = nullptr;
    TH1D* h_nbJets_Total  = nullptr;
    TH1D* h_nbJets_Pass   = nullptr;
};

#endif

