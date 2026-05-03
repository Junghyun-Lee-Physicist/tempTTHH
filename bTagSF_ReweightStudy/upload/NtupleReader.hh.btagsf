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
    Float_t GetEvtWeight() const { return evtWeight; }
    Float_t GetGenWeight() const { return genWeight; }
    Float_t GetPUWeight() const { return PUWeight; }
    Float_t GetPrefireWeight() const { return L1PrefiringWeight; }
    Bool_t  GetFailGoldenJson() const { return failGoldenJson; }
    Bool_t  GetPassMETFilters() const { return passMETFilters; }
    Bool_t  GetPassHadTrig() const { return passHadTrig; }

    // 5. B-tagging variables
    const std::vector<float>& GetBTagScore() const { return *bTagScore; }
    const std::vector<int>&   GetHadronFlavor() const { return *hadFlavs; }

    // 6. ttbar categorization (per-event genTtbarId; -1 if branch absent)
    //    Used by BTagSFProcessor to dispatch inclusive ttbar events into
    //    LF / cc / B process keys (ttH AN App. A.2).
    Int_t  GetGenTtbarId() const { return genTtbarId; }


private:
    TTree          *fChain;   //!

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

    std::vector<float> *bTagScore = nullptr;
    std::vector<int>   *hadFlavs  = nullptr;

    Float_t         evtWeight;
    Float_t         genWeight;
    Float_t         PUWeight;
    Float_t         L1PrefiringWeight;
    Bool_t          failGoldenJson;
    Bool_t          passMETFilters;
    Bool_t          passHadTrig;

    // ttbar category
    Int_t           genTtbarId = -1;   // initialized to "not ttbar"

    // [Branch Pointers] (transient, '//!' tells ROOT not to serialize)
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

    TBranch        *b_bTagScore = nullptr;  //!
    TBranch        *b_hadFlavs  = nullptr;  //!

    TBranch        *b_evtWeight = nullptr;         //!
    TBranch        *b_genWeight = nullptr;         //!
    TBranch        *b_PUWeight = nullptr;          //!
    TBranch        *b_L1PrefiringWeight = nullptr; //!
    TBranch        *b_failGoldenJson = nullptr;    //!
    TBranch        *b_passMETFilters = nullptr;    //!
    TBranch        *b_passHadTrig    = nullptr;    //!

    TBranch        *b_genTtbarId = nullptr;        //!
};

#endif
