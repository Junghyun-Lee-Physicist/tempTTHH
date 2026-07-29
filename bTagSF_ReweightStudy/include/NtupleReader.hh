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

    // [2026-07-29] main 의 lepton veto 정의 (`nVetoLeptons == 0` 이 FH).
    //   nMuons/nElecs 를 대신 쓰면 안 된다: btagtrig skim 은 lead-muon gate 를
    //   통과한 이벤트에서만 lepton 을 수집하므로 nMuons==0 은 soft lepton
    //   이벤트를 걸러내지 못한다 → main 과 다른 위상공간에서 유도하게 된다.
    Int_t  GetNVetoLeptons() const { return nVetoLeptons; }
    bool   HasNVetoLeptons() const { return b_nVetoLeptons != nullptr; }
    Int_t  GetNJets() const { return nJets; }
    Int_t  GetNBJets() const { return nbJets; }

    // 3. Kinematics
    Float_t GetHT() const { return HT; }

    // [2026-07-29] MET_pt — muonCR region(`nMuons==1 && nElecs==0 && MET>20`) 용.
    //   구 skim 에는 없으므로 branch 부재 시 -1. muonCR 을 고른 경우에만 쓰이며,
    //   그때 부재는 FATAL (컷이 조용히 무시되면 안 된다).
    Float_t GetMET() const { return MET_pt; }
    bool    HasMET() const { return b_MET_pt != nullptr; }
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

    // ── [2026-07-29] tt+nb 확장 id ────────────────────────────────────────
    //   analyzer 는 skim 에 **두 개**를 쓴다:
    //     genTtbarId       원본 NanoAOD 값. %100 이 최대 55 다.
    //     expandedTtbarId  ttnb lookup 이 붙은 값. 61/62(tt+bbb), 71/72(tt+4b).
    //   process group 분류는 **expandedTtbarId** 를 써야 한다. 원본을 쓰면
    //   61/62/71/72 가 한 건도 나오지 않아 tt+nb 그룹이 전 bin 1.0 이 되고
    //   (makeReweightJSON 의 "빈 그룹" 경고는 히스토그램 존재 여부만 보므로
    //   발동하지 않는다) 8-key JSON 처럼 보이지만 8번째가 무효가 된다.
    //   branch 가 없으면 -1 → 호출부가 FATAL 로 끊는다 (조용한 fallback 금지).
    Int_t  GetExpandedTtbarId() const { return expandedTtbarId; }
    bool   HasExpandedTtbarId() const { return b_expandedTtbarId != nullptr; }

    // ── [2026-07-29] ttbar stitching multiplier ───────────────────────────
    //   analyzer 가 이벤트별로 적용한 배수 (inclusive: 1 또는 0, dedicated:
    //   r 또는 0). b-tag norm reweight 는 process group 별 **합의 비율**이므로,
    //   이 배수를 곱하지 않으면 inclusive ttbar 와 dedicated ttbb/tt4b 가
    //   같은 위상공간을 두 번 채워(tt+B 이중계수) 비율이 틀어진다.
    //   stitching 이 비활성인 run 에서는 analyzer 가 1.0 을 쓰므로 무해하다.
    Float_t GetStitchWeight() const { return stitchWeight; }
    bool    HasStitchWeight() const { return b_stitchWeight != nullptr; }


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
    Int_t           nVetoLeptons = -1;   // -1 = branch 부재 (호출부가 FATAL)
    Int_t           nJets;
    Int_t           nbJets;

    Float_t         HT;
    Float_t         MET_pt = -1.f;   // -1 = branch 부재
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
    Int_t           genTtbarId      = -1;   // initialized to "not ttbar"
    Int_t           expandedTtbarId = -1;   // ttnb lookup 적용값 (61/62/71/72 포함)
    Float_t         stitchWeight    = 1.0f; // analyzer 가 적용한 stitch 배수

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
    TBranch        *b_nVetoLeptons = nullptr;  //!
    TBranch        *b_nJets = nullptr;    //!
    TBranch        *b_nbJets = nullptr;   //!

    TBranch        *b_HT = nullptr;       //!
    TBranch        *b_MET_pt = nullptr;   //!
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

    TBranch        *b_genTtbarId      = nullptr;   //!
    TBranch        *b_expandedTtbarId = nullptr;   //!
    TBranch        *b_stitchWeight    = nullptr;   //!
};

#endif
