#ifndef NtupleReader_hh
#define NtupleReader_hh

#include <TTree.h>
#include <TChain.h>
#include <vector>
#include <iostream>

class NtupleReader {
public:
    NtupleReader(TTree *tree);
    virtual ~NtupleReader();

    Int_t GetEntry(Long64_t entry);

    // --- Accessor Functions (Get...) ---
    
    // 1. Triggers
    bool   GetPassTrigger_IsoMu27() const { return passTrigger_HLT_IsoMu27; }
    bool   GetPassTrigger_PFHT1050() const { return passTrigger_HLT_PFHT1050; }
    bool   GetPassTrigger_6J1T_B() const { return passTrigger_6J1T_B; }
    bool   GetPassTrigger_6J1T_CDEF() const { return passTrigger_6J1T_CDEF; }
    bool   GetPassTrigger_6J2T_B() const { return passTrigger_6J2T_B; }
    bool   GetPassTrigger_6J2T_CDEF() const { return passTrigger_6J2T_CDEF; }
    bool   GetPassTrigger_4J3T_B() const { return passTrigger_4J3T_B; }
    bool   GetPassTrigger_4J3T_CDEF() const { return passTrigger_4J3T_CDEF; }

    // 2. Objects
    Int_t  GetNMuons() const { return nMuons; }
    Int_t  GetNElecs() const { return nElecs; }
    Int_t  GetNJets() const { return nJets; }
    Int_t  GetNBJets() const { return nbJets; }

    // 3. Kinematics
    Float_t GetHT() const { return HT; }
    const std::vector<float>& GetJetPt() const { return *jetPt; }
    const std::vector<float>& GetJetEta() const { return *jetEta; }

    // 4. Weights & Flags
    Float_t GetGenWeight() const { return genWeight; }
    Float_t GetPUWeight() const { return PUWeight; }
    Float_t GetPrefireWeight() const { return L1PrefiringWeight; }
    Bool_t  GetFailGoldenJson() const { return failGoldenJson; }
    Bool_t  GetPassMETFilters() const { return passMETFilters; }

private:
    TTree          *fChain;   //! pointer to the analyzed TTree or TChain

    // --- Data Members ---
    Bool_t          passTrigger_HLT_IsoMu27;
    Bool_t          passTrigger_HLT_PFHT1050;
    Bool_t          passTrigger_6J1T_B;
    Bool_t          passTrigger_6J1T_CDEF;
    Bool_t          passTrigger_6J2T_B;
    Bool_t          passTrigger_6J2T_CDEF;
    Bool_t          passTrigger_4J3T_B;
    Bool_t          passTrigger_4J3T_CDEF;

    Int_t           nMuons;
    Int_t           nElecs;
    Int_t           nJets;
    Int_t           nbJets;

    Float_t         HT;
    std::vector<float> *jetPt = nullptr;
    std::vector<float> *jetEta = nullptr;

    Float_t         genWeight;
    Float_t         PUWeight;
    Float_t         L1PrefiringWeight;
    Bool_t          failGoldenJson;
    Bool_t          passMETFilters;

    // =========================================================================
    // [Branch Pointers]
    // 1. Speed: Passing these to SetBranchAddress allows ROOT to bypass 
    //    string lookups (internal map search), speeding up initialization.
    // 2. Flexibility: Enables calling b_Var->GetEntry(i) to read ONLY specific 
    //    branches instead of the full tree (Lazy Loading).
    //
    // Note on '//!': This tells ROOT's I/O system NOT to save these pointers 
    // to the output file (transient), avoiding invalid memory address issues.
    // =========================================================================
    
    TBranch        *b_passTrigger_HLT_IsoMu27 = nullptr;   //!
    TBranch        *b_passTrigger_HLT_PFHT1050 = nullptr;  //!
    TBranch        *b_passTrigger_6J1T_B = nullptr;        //!
    TBranch        *b_passTrigger_6J1T_CDEF = nullptr;     //!
    TBranch        *b_passTrigger_6J2T_B = nullptr;        //!
    TBranch        *b_passTrigger_6J2T_CDEF = nullptr;     //!
    TBranch        *b_passTrigger_4J3T_B = nullptr;        //!
    TBranch        *b_passTrigger_4J3T_CDEF = nullptr;     //!

    TBranch        *b_nMuons = nullptr;   //!
    TBranch        *b_nElecs = nullptr;   //!
    TBranch        *b_nJets = nullptr;    //!
    TBranch        *b_nbJets = nullptr;   //!

    TBranch        *b_HT = nullptr;       //!
    TBranch        *b_jetPt = nullptr;    //!
    TBranch        *b_jetEta = nullptr;   //!

    TBranch        *b_genWeight = nullptr;        //!
    TBranch        *b_PUWeight = nullptr;         //!
    TBranch        *b_L1PrefiringWeight = nullptr;//!
    TBranch        *b_failGoldenJson = nullptr;   //!
    TBranch        *b_passMETFilters = nullptr;   //!
};

#endif
