#ifndef ttHHanalyzer_unified_h
#define ttHHanalyzer_unified_h
#include "tnm.h"
#include <cmath> 
#include <algorithm>
#include <TString.h>
#include <vector>
#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include <iterator>
#include <string>
#include <cctype>
#include <limits>
#include "EventShape/Class/src/EventShape.cc"
#include <TLorentzVector.h>
#include "TRandom3.h"
#include <unordered_map>
//#include <map>
//#include "thhHypothesisCombinatorics.h"
//#include "HypothesisCombinatorics.h"
//#include "include/tthHypothesisCombinatorics.h"
//#include "include/HypothesisCombinatorics.h"
#include <stdexcept>
#include <array>
#include <set>

//#include "fifo_map.hpp" // No need now, I'll update cutflow logic

#include "CorrectionsManager.h"
#include "Config_TtCatGroup.hh"   // ttH AN App. A.2.1 process key mapping (LF/cc/B/ttH/ttHH/...)
#include "ExpandedTtbarId.h"      // [tt+nb] Expanded_genTtbarId per-event lookup
#include "StitchFactors.h"        // [stitch] per-(sample,category) stitch multiplier
#include "DebugLogger.h"          // [STEP2][debug] kDebug 모드 단계별 검증 로거
#include "SelectionCuts.h"        // [STEP3] selection 상수 단일 소스 (cut map 대체)
#include "EventShape/Class/interface/EventShape.h"  // [STEP5] event shape (라이브러리 링크)
#include "HiggsReconstructor.h"   // [STEP6] di-mother(HH/ZH/ZZ) chi2 재구성 클래스
#include <map>

#include <nlohmann/json.hpp>

#include <iostream>
//using namespace ROOT::Math;
using namespace std;

const float cLargeValue = 99999999999.;
const float cEps = 0.000000001; 
const float cHiggsMass = 125.38;
const float cZMass = 91.;


// =============================================================================
// Analysis Mode Definition
// =============================================================================
enum class AnalysisMode {
    kMainAnalysis,           // 본 분석: full selection + 전체 SF + stitch
    kBTagAndTriggerStudy,    // btagtrig: 파생 도구(b-tag SF / trigger SF)용 flat skim 생산
    kDebug,                  // [STEP2] main과 동일 동작 + 단계별 디버그 로그
                             //   (로컬 테스트 파일 1-2개 검증용; DebugLogger.h 참조)
    kPrescan      // [NEW] gen-level Σ genWeight + genTtbarId breakdown
                             //       (no selection, no object building)
                             // ⚠ TEMPORARY — to be migrated to NtupleForge
                             //   (ntuplizer can dump the same info as a
                             //    friend tree alongside the Events tree,
                             //    eliminating the need for the analyzer to
                             //    re-open files and read the Runs tree).
};

// Argument로 받는 문자열 → AnalysisMode class 변환 함수
inline AnalysisMode parseAnalysisMode(const std::string& modeStr) {
    if (modeStr.empty()) {
        throw std::invalid_argument(
            "[FATAL] Analysis mode not specified!\n"
            "        Valid modes: main, btagtrig, prescan, debug"
        );
    }
    if (modeStr == "main"       || modeStr == "MainAnalysis")        return AnalysisMode::kMainAnalysis;
    if (modeStr == "btagtrig"   || modeStr == "BTagAndTriggerStudy") return AnalysisMode::kBTagAndTriggerStudy;
    if (modeStr == "prescan"    || modeStr == "Prescan")             return AnalysisMode::kPrescan;
    if (modeStr == "debug"      || modeStr == "Debug")               return AnalysisMode::kDebug;
    // [STEP2] 제거된 모드는 명시적으로 안내 (조용한 오동작 방지)
    if (modeStr == "trigsf" || modeStr == "TriggerSFStudy" ||
        modeStr == "validation" || modeStr == "ValidationStudy") {
        throw std::invalid_argument(
            "[FATAL] Analysis mode '" + modeStr + "' was REMOVED (2026-06 refactor).\n"
            "        trigsf     -> standalone TriggerStudy package (btagtrig skim 입력)\n"
            "        validation -> 제거됨 (docs/changes/STEP_2 참조)\n"
            "        Valid modes: main, btagtrig, prescan, debug");
    }
    throw std::invalid_argument("[FATAL] Unknown analysis mode: " + modeStr +
                                "\n        Valid modes: main, btagtrig, prescan, debug");
}

inline std::string analysisModeName(AnalysisMode mode) {
    switch (mode) {
        case AnalysisMode::kMainAnalysis:        return "MainAnalysis";
        case AnalysisMode::kBTagAndTriggerStudy: return "BTagAndTriggerStudy";
        case AnalysisMode::kDebug:               return "Debug";
        case AnalysisMode::kPrescan:  return "Prescan";
        default: return "Unknown";
    }
}

// =============================================================================
// [STEP3] Selection 강제(enforce) 모드 비트 — cut 테이블(kCutSequence)에서
// "이 cut이 어느 모드에서 실제 reject로 작동하는가"를 선언한다.
// 비트에 없는 모드에서는 관찰 전용(cutflow 기록만, reject 없음).
// =============================================================================
enum : uint8_t {
    kSelBitMainLike = 0x1,                          // main + debug (debug는 main 미러)
    kSelBitBtagTrig = 0x2,                          // btagtrig (skim 생산)
    kSelEnforceAll  = kSelBitMainLike | kSelBitBtagTrig,
    kSelObserveOnly = 0x0                           // 어떤 모드에서도 cut 아님
};
inline uint8_t selectionModeBit(AnalysisMode m) {
    switch (m) {
        case AnalysisMode::kMainAnalysis:
        case AnalysisMode::kDebug:               return kSelBitMainLike;
        case AnalysisMode::kBTagAndTriggerStudy: return kSelBitBtagTrig;
        default:                                 return 0;  // prescan: selectObjects 미호출
    }
}

// =============================================================================
// SelectionPolicy: 모드별 "비-cut" 동작 정의
// [STEP3] cut on/off 부울 4종(applyTriggerCut/applyLeptonVeto/applyBJetCut/
// applyHadWMassCut)은 cut 테이블의 enforceIn 비트마스크로 흡수 — 여기엔
// object 수집·Higgs reco 등 cut이 아닌 동작만 남는다.
// =============================================================================
struct SelectionPolicy {
    bool collectLeptons;        // lepton 수집 (btagtrig: muon control)
    bool requireLeadMuonOnly;   // lead lepton gate를 muon으로 한정
    bool doHiggsReconstruction; // chi2 Higgs reco 수행 여부

    static SelectionPolicy fromMode(AnalysisMode mode) {
        SelectionPolicy p{false, false, false};
        switch (mode) {
            case AnalysisMode::kMainAnalysis:
            case AnalysisMode::kDebug:   // debug = main 미러
                p.doHiggsReconstruction = true;
                break;
            case AnalysisMode::kBTagAndTriggerStudy:
                p.collectLeptons      = true;
                p.requireLeadMuonOnly = true;
                break;
            case AnalysisMode::kPrescan:
                break;   // selectObjects 미호출 — 전부 false
        }
        return p;
    }

    void print() const {
        std::cout << "\n=== Selection Policy (non-cut behaviours) ===" << std::endl;
        std::cout << "  Collect Leptons:                          " << (collectLeptons ? "YES" : "NO") << std::endl;
        std::cout << "  Require Lead Muon only (Not Lead Elec):   " << (requireLeadMuonOnly ? "YES" : "NO") << std::endl;
        std::cout << "  Higgs Reco:                               " << (doHiggsReconstruction ? "YES" : "NO") << std::endl;
        std::cout << "  (cut 강제 여부는 kCutSequence의 enforceIn 비트 참조)" << std::endl;
        std::cout << "============================================\n" << std::endl;
    }
};
// =============================================================================
// Analysis Mode Definition (End)
// =============================================================================


// [STEP3] 전역 cut map 제거 — include/SelectionCuts.h (namespace Cuts)로 대체.
//         map operator[]의 무음 0-삽입 함정과 타입 손실 제거. 원형은
//         docs/backup_20260611/ttHHanalyzer_unified.h 참조.

class objectPhysics {
 public:
    enum lFlavor{kNA, kEle, kMuon};
    explicit objectPhysics(const float pT, const float eta, const float phi, const float mass = 0){
	_p4.SetPtEtaPhiM(pT, eta, phi, mass);
    }
    
    TLorentzVector * getp4(){
	return &_p4;
    }
    const TLorentzVector * getp4() const {
	return &_p4;
    }

    objectPhysics(){};
    void scale(float JES, bool up = true){
	_pxOffset = JES * _p4.Px();
	_pyOffset = JES * _p4.Py();
	_pzOffset = JES * _p4.Pz();
	_EOffset  = JES * _p4.E();
	if(up){
	    _p4.SetPxPyPzE(_p4.Px()+_pxOffset,_p4.Py()+_pyOffset,_p4.Pz()+_pzOffset, _p4.E()+_EOffset);
	} else {
	    _p4.SetPxPyPzE(_p4.Px()-_pxOffset,_p4.Py()-_pyOffset,_p4.Pz()-_pzOffset, _p4.E()-_EOffset);
	}
    }
    std::vector<float> getOffset(){
	std::vector<float> offset = {_pxOffset,_pyOffset,_pzOffset,_EOffset};
	return offset;
    }
    void subtractp4(const std::vector<float>& offset){
	_p4.SetPxPyPzE(_p4.Px()-offset[0],_p4.Py()-offset[1],_p4.Pz()-offset[2], _p4.E()-offset[3]);
    }
    void addp4(const std::vector<float>& offset){
	_p4.SetPxPyPzE(_p4.Px()+offset[0],_p4.Py()+offset[1],_p4.Pz()+offset[2], _p4.E()+offset[3]);
    }

 private:
    TLorentzVector _p4;
    float _pxOffset = 0., _pyOffset = 0., _pzOffset = 0., _EOffset = 0.;
};


class objectGenPart:public objectPhysics { 
 public:    
    using objectPhysics::objectPhysics;
    bool hasHiggsMother = false;
    bool hasTopMother = false;
    bool matched = false; 
    float dRmatched = 10000.;
};


class objectJet:public objectPhysics {
 public:
    
    using objectPhysics::objectPhysics;

	// --- [Metadata] 분석에 필요한 핵심 정보만 유지 ---
    float bTagCSV  = -99.f;
    int   jetID    = -99;
    int   jetPUid  = -99;
    bool  passPuId = false;
    float mass_DiffRatio = -99.f; // JEC mass validation: (ReCalc - NanoAOD)/NanoAOD

	// MC Truth info
    int hadFlav = -99;
	int partonFlav = -99;
    float  genMatchedPt = -1.f;  // matched gen-jet pT, or -1

    // Working Point
    static constexpr float valbTagTight  = 0.7476; //This is not used
    static constexpr float valbTagMedium = 0.3040;
    static constexpr float valbTagLoose  = 0.0532;

	bool matchedtoHiggs = false;
    bool matchedtoTop   = false;
    float matchedtoHiggsdR = 999.f;
    int minChiHiggsIndex = -1;

	// JEC 재적용 검증
	float JEC_DiffRatio = -1.f;

	virtual ~objectJet() {}
};

class objectMET:public objectPhysics {
 public:
    using objectPhysics::objectPhysics;
};

class objectBoostedJet:public objectPhysics {
 public:
    using objectPhysics::objectPhysics;
    float softDropMass;
};


class objectbJet:public objectPhysics {
 public:
    using objectPhysics::objectPhysics;
};

class objectLightJet:public objectPhysics {
 public:
    using objectPhysics::objectPhysics;
    };

class objectLep:public objectPhysics {
 public:
    using objectPhysics::objectPhysics;
    int charge;
    float miniPFRelIso;
    float pfRelIso03;
    float pfRelIso04;
    lFlavor flavor;
};

class event{
 public:
    event(){
    }

    ~event(){
        // Delete owned objects to prevent memory leaks.
        // _selectJets owns all jet objects; _selectbJets/_selectLightJets/_loosebJets
        // are non-owning subsets of the same pointers.
        for (auto* p : _selectJets) delete p;
        // _selectLeptons owns all lepton objects; _selectMuons/_selectElectrons
        // are non-owning subsets of the same pointers.
        for (auto* p : _selectLeptons) delete p;
        for (auto* p : _selectGenParts) delete p;
        for (auto* p : _selectHadronicHiggses) delete p;
        for (auto* p : _selectBoostedJets) delete p;
        delete _MET;
    }


    struct evShapes{
	float objectP;
	float objectPT;
	float centrality;
    };

    struct maxObjects{
	float maxPT;
	float maxPTmass;
    };

    struct statObjects{
	float dR;
	float meandR;
	float mindR;
	float maxdR;
	float meandEta;
	float mindEta;
	float maxdEta;
	float meandPhi;
	float mindPhi;
	float maxdPhi;
	float mindRMass;
	float mindRpT;
    };

    struct foxWolframObjects{
	float h0;
	float h1;
	float h2;
	float h3;
	float h4;
	float r1;
	float r2;
	float r3;
	float r4;
    };

    EventShape * eventShapeJet, * eventShapeBjet;

    
    void addJet(objectJet * jet){
	_sumJetScalarpT+=fabs(jet->getp4()->Pt());
	_sumJetp4+= * jet->getp4();
	_jets.push_back(jet);
    }
    
    void selectJet(objectJet * jet){
	_sumSelJetScalarpT+=fabs(jet->getp4()->Pt());
	_sumSelJetMass+=jet->getp4()->M(); //
	_sumSelJetp4 += * jet->getp4();
	_selectJets.push_back(jet);
    }
    
    void selectLepton(objectLep * lepton){
	//	_sumLeptonScalarpT+=fabs(lepton->getp4()->Pt());  
	_selectLeptons.push_back(lepton);
    }

    void selectEle(objectLep * ele){
	ele->flavor = objectLep::kEle;
	_sumSelElectronScalarpT+=fabs(ele->getp4()->Pt());  
	_sumSelElectronp4 += *ele->getp4();  
	_selectLeptons.push_back(ele);
	_selectElectrons.push_back(ele);
    }

    void selectMuon(objectLep * muon){
	muon->flavor = objectLep::kMuon;
	_sumSelMuonScalarpT+=fabs(muon->getp4()->Pt());  
	_sumSelMuonp4 += *muon->getp4();  
	_selectLeptons.push_back(muon);
	_selectMuons.push_back(muon);
    }
    

    void selectHadronicHiggs(objectBoostedJet * boostedJet){
	//	_sumSelBoostedJetSoftDropMass+=fabs(boostedJet->softDropMass);
	_sumSelHadronicHiggsScalarpT+=fabs(boostedJet->getp4()->Pt());  
	_sumSelHadronicHiggsMass+=boostedJet->getp4()->M();  
	_sumHadronicHiggsp4 += *boostedJet->getp4();
	_selectHadronicHiggses.push_back(boostedJet);
    }

    void selectBoostedJet(objectBoostedJet * boostedJet){
	//	_sumSelBoostedJetSoftDropMass+=fabs(boostedJet->softDropMass);
	//	_sumSelHadronicHiggsScalarpT+=fabs(boostedJet->getp4()->Pt());  
	//_sumSelHadronicHiggsScalarMass+=fabs(boostedJet->getp4()->M());  
	//_sumHadronicHiggsp4 += *boostedJet->getp4();
	_selectBoostedJets.push_back(boostedJet);
    }


    void selectbJet(objectJet * jet){
	_sumSelbJetScalarpT+=fabs(jet->getp4()->Pt());  
	_sumSelbJetMass+=jet->getp4()->M();  
	_sumSelbJetp4 += *jet->getp4();
        _selectJetsMass.push_back(jet->getp4()->M());
	_selectbJets.push_back(jet);
    }

    void selectLightJet(objectJet * jet){
	_sumSelLightJetScalarpT+=fabs(jet->getp4()->Pt());
	_sumSelLightJetMass+=jet->getp4()->M(); 
	_sumLightJetp4 += *jet->getp4();
        _selectLightJetsMass.push_back(jet->getp4()->M());
	_selectLightJets.push_back(jet);
    }

    void selectLoosebJet(objectJet * jet){
	_loosebJets.push_back(jet);
    }

    void selectGenPart(objectGenPart * genPart){   
	_selectGenParts.push_back(genPart);
    } 


    void setMET(objectMET * met){
	_MET = met;
    }

    void setnVetoLepton(int nVeto){
	_nVetoLepton = nVeto;
    }

    float getSumJetScalarpT(){
	return _sumJetScalarpT;
    }

    float getSumSelJetScalarpT(){
	return _sumSelJetScalarpT;
    }

    float getSumSelJetScalarpT() const {
        return _sumSelJetScalarpT;
    }

    float getSumSelJetpT(){
	return _sumSelJetp4.Pt();
    }

    float getSumSelHadronicHiggsScalarpT(){   
	return _sumSelHadronicHiggsScalarpT;
    }

    float getSumSelbJetScalarpT(){   
	return _sumSelbJetScalarpT;
    }

    float getSumSelbJetpT(){   
	return _sumSelbJetp4.Pt();
    }

    float getSumSelLightJetScalarpT(){   
	return _sumSelLightJetScalarpT;
    }

    float getSumSelLeptonScalarpT(){   
	return _sumSelMuonScalarpT + _sumSelElectronScalarpT;
    }

    float getSumSelLeptonpT(){   
	return _sumSelMuonp4.Pt() + _sumSelElectronp4.Pt();
    }

    float getSelLeptonHT(){   
	return _sumSelJetScalarpT + _sumSelMuonScalarpT + _sumSelElectronScalarpT;
    }

    float getSelLeptonST(){   
	return _sumSelJetScalarpT + _sumSelMuonScalarpT + _sumSelElectronScalarpT + getMET()->getp4()->Pt();
    }

    float getSumSelJetMass(){ 
	return _sumSelJetMass;
    }
    
    float getSumSelHadronicHiggsMass(){  
	return _sumSelHadronicHiggsMass;
    }

    float getSumSelbJetMass(){  
	return _sumSelbJetMass;
    }

    float getSumSelLightJetMass(){  
    	return _sumSelLightJetMass;
    }

    //    float getSelHadronicHiggsSoftDropMass(){
    //	return _selectHadronicHiggses.msoftdrop();
    // }


    float getSelLeptonsMass(){  
	return _sumSelMuonp4.M() + _sumSelElectronp4.M();
    }

    float getSelMuonsMass(){  
	return _sumSelMuonp4.M();
    }

    float getSelMuonsPT(){  
	return _sumSelMuonp4.Pt();
    }

    float getSelMuonsEta(){  
	return _sumSelMuonp4.Eta();
    }

    float getSelElectronsMass(){  
	return _sumSelElectronp4.M();
    } 

    float getSelElectronsPT(){  
	return _sumSelElectronp4.Pt();
    } 

    float getSelElectronsEta(){  
	return _sumSelElectronp4.Eta();
    } 

    int getnGenPart(){ 
	return _selectGenParts.size(); 
    }

    int getnSelbJet(){
	return _selectbJets.size(); 
    }

    int getnSelbJet() const {
	return _selectbJets.size(); 
    }

    int getnLightJet(){
    	return _selectLightJets.size(); 
    }

    int getnbLooseJet(){
	return _loosebJets.size(); 
    }

    int getnHadronicHiggs(){
	return _selectHadronicHiggses.size();
    }

    int getnSelJet(){
	return _selectJets.size(); 
    }

    int getnSelJet() const {
	return _selectJets.size(); 
    }

    int getnJet(){
	return _jets.size(); 
    }

    int getnSelElectron(){
	return _selectElectrons.size();
    }

    int getnSelMuon(){
	return _selectMuons.size(); 
    }

    bool orderLeptons(){
	if(_selectLeptons.size() != 2)
	    return false;
	else {
	    objectLep* tmp; 
	    if(_selectLeptons.at(0)->getp4()->Pt() < _selectLeptons.at(1)->getp4()->Pt()){
		//std::cout << "before " << _selectLeptons.at(0)->getp4()->Pt() << " " << _selectLeptons.at(1)->getp4()->Pt();
		tmp = _selectLeptons.at(0);
		_selectLeptons.at(0) = _selectLeptons.at(1);
		_selectLeptons.at(1) = tmp;
		//std::cout << "after " <<  _selectLeptons.at(0)->getp4()->Pt() << " " << _selectLeptons.at(1)->getp4()->Pt();
	    }
	    return true;
	}
    }

    bool orderJets(){
        if(_selectJets.size() < 2){
            return false;
        }
        else {
            std::sort(_selectJets.begin(), _selectJets.end(), [](const auto& jet1, const auto& jet2) {
                return jet1->getp4()->Pt() > jet2->getp4()->Pt();
            });
            return true;
        }
    }

    int getnSelLepton(){
	return _selectLeptons.size();
    }

    int getnVetoLepton(){
	return _nVetoLepton;
    }

    objectMET* getMET(){
	return _MET;
    }

    std::vector<objectGenPart*>* getGenParts(){ 
	return &_selectGenParts;
    }

    std::vector<objectJet*>* getJets(){
	return &_jets;
    }


    std::vector<objectJet*>* getSelJets(){
	return &_selectJets;
    }

    const std::vector<objectJet*>* getSelJets() const {
        return &_selectJets;
    }

    std::vector<float> getSelJetsMass(){
        return _selectJetsMass;
    }

    std::vector<objectBoostedJet*>* getSelHadronicHiggses(){
	return &_selectHadronicHiggses;
    }

    std::vector<objectBoostedJet*>* getSelBoostedJets(){
	return &_selectBoostedJets;
    }

    std::vector<objectJet*>* getSelbJets(){
	return &_selectbJets;
    }

    const std::vector<objectJet*>* getSelbJets() const {
        return &_selectbJets;
    }

    std::vector<objectJet*>* getSelLightJets(){
    	return &_selectLightJets;
    }

    std::vector<float> getSelLightJetsMass(){
        return _selectLightJetsMass;
    }

    std::vector<objectJet*>* getLoosebJets(){
	return &_loosebJets;
    }

    std::vector<objectLep*>* getSelElectrons(){
	return &_selectElectrons;
    }

    std::vector<objectLep*>* getSelMuons(){
	return &_selectMuons;
    }

    std::vector<objectLep*>* getSelLeptons(){
	return &_selectLeptons;
    }

    // Centrality calculation //
    template <class object1, class object2>
	void getCentrality(std::vector<object1*>* cont1, std::vector<object2*>* cont2, evShapes& cent) {
	TLorentzVector obj1P4, obj2P4;
	float sumPT = 0., sumP = 0., centrality = 0.;

	if(cont1->size()  == 0 || cont2->size() == 0){
	}
	for(int oindex=0; oindex < cont1->size(); oindex++){
	    obj1P4 = (*cont1->at(oindex)->getp4());
	    sumPT += obj1P4.Pt();
	    sumP  += obj1P4.P();
	}
	for(int iindex = 0; iindex < cont2->size(); iindex++){
	    obj2P4 = (*cont2->at(iindex)->getp4());
	    sumPT += obj2P4.Pt();
	    sumP  += obj2P4.P();
	}
	centrality = sumPT/sumP;
	cent.objectPT = sumPT;
	cent.objectP  = sumP;
	cent.centrality = centrality;
    }


    // Centrality calculation //                                                                                                                                                
    template <class object1, class object2>
	void getCentralityV2(std::vector<object1*>* cont1, std::vector<object2*>* cont2, evShapes& cent) {
        TLorentzVector obj1P4, obj2P4, sumP4;
        float  centrality = 0.;

        int nObject = 0.;
	if(cont1->size()  == 0 || cont2->size() == 0){
        }
        for(int oindex=0; oindex < cont1->size(); oindex++){
            obj1P4 = (*cont1->at(oindex)->getp4());
            sumP4 += obj1P4;
        }
        for(int iindex = 0; iindex < cont2->size(); iindex++){
            obj2P4 = (*cont2->at(iindex)->getp4());
            sumP4 += obj2P4;
        }
        centrality = sumP4.Pt()/sumP4.P();
        cent.objectPT = sumP4.Pt();
        cent.objectP  = sumP4.P();
	cent.centrality = centrality;
    }


    template <class object1>
	void getMaxPTSame(std::vector<object1*>* cont1,  maxObjects& xxxMaxs) {
	float maxPT = 0., maxPTmass = 0., tmpPT = 0., tmpMass;
	TLorentzVector tmpP4; 
	int nObject = 0.;
	if(cont1->size()  == 0){
	}
	for(int oindex=0; oindex < cont1->size(); oindex++){
	    for(int iindex = oindex+1; iindex < cont1->size(); iindex++){
		for(int mindex = oindex+2; mindex < cont1->size(); mindex++){
		    tmpP4 = (*cont1->at(oindex)->getp4() + *cont1->at(iindex)->getp4() + *cont1->at(mindex)->getp4());
		    tmpPT = tmpP4.Pt();
			//(*cont1->at(oindex)->getp4() + *cont1->at(iindex)->getp4() + *cont1->at(mindex)->getp4()).Pt();
		    tmpMass = tmpP4.M();
			//(*cont1->at(oindex)->getp4() + *cont1->at(iindex)->getp4() + *cont1->at(mindex)->getp4()).M();
		    if(maxPT < tmpPT){
			maxPT= tmpPT;
			maxPTmass = tmpMass;
		    }
		    nObject++;
		}
	    }
	}
	xxxMaxs.maxPT = maxPT;
	xxxMaxs.maxPTmass = maxPTmass;
    }


    template <class object1, class object2>
	void getMaxPTComb(std::vector<object1*>* cont1, std::vector<object2*>* cont2, maxObjects& xyyMaxs) {
	float maxPT = 0., maxPTmass = 0., tmpPT = 0., tmpMass;
	TLorentzVector tmpP4; 
	int nObject = 0.;
	if(cont1->size()  == 0 || cont2->size() == 0){
	}
	for(int oindex=0; oindex < cont1->size(); oindex++){
	    for(int iindex = 0; iindex < cont2->size(); iindex++){
		for(int mindex = iindex+1; mindex < cont2->size(); mindex++){
		    tmpP4 = (*cont1->at(oindex)->getp4() + *cont2->at(iindex)->getp4() + *cont2->at(mindex)->getp4());
		    tmpPT = tmpP4.Pt();
		    tmpMass = tmpP4.M();
		    //  tmpPT = (*cont1->at(oindex)->getp4() + *cont2->at(iindex)->getp4() + *cont2->at(mindex)->getp4()).Pt();
		    //  tmpMass = (*cont1->at(oindex)->getp4() + *cont2->at(iindex)->getp4() + *cont2->at(mindex)->getp4()).M();
		    if(maxPT < tmpPT){
			maxPT= tmpPT;
			maxPTmass = tmpMass;
		    }
		    nObject++;
		}
	    }
	}
	xyyMaxs.maxPT = maxPT;
	xyyMaxs.maxPTmass = maxPTmass;
    } 

    template <class object1, class object2>
	void getStatsComb (std::vector<object1*>* cont1, std::vector<object2*>* cont2, statObjects& statsComb) {
	float mindR = 99999999999., mindEta = 99999999999., mindPhi = 99999999999., tmpdR = 0., tmpdPhi = 0., tmpdEta = 0., tmpMass = 0., tmpPT = 0, sumdEta = 0., sumdPhi = 0., sumdR = 0., maxdR = 0. , maxdPhi = 0., maxdEta = 0., mindRMass = 0., mindRpT = 0.;
	int nObject = 0;

	if(cont1->size()  == 0 || cont2->size() == 0){
	}
	for(int oindex=0; oindex < cont1->size(); oindex++){
	    for(int iindex = 0; iindex < cont2->size(); iindex++){

		tmpdPhi = fabs(cont1->at(oindex)->getp4()->Phi() - cont2->at(iindex)->getp4()->Phi());
		tmpdEta = fabs(cont1->at(oindex)->getp4()->Eta() - cont2->at(iindex)->getp4()->Eta());
		tmpdR   = TMath::Sqrt(tmpdPhi*tmpdPhi + tmpdEta*tmpdEta);
		tmpMass = (*cont1->at(oindex)->getp4() + *cont2->at(iindex)->getp4()).M(); 
		tmpPT   = (*cont1->at(oindex)->getp4() + *cont2->at(iindex)->getp4()).Pt(); 

		sumdR   += tmpdR;
		sumdEta += tmpdEta;
		sumdPhi += tmpdPhi;
		if(mindR > tmpdR){
		    mindR = tmpdR;
		    mindRMass = tmpMass;
 		    mindRpT = tmpPT;
		}
		if(maxdR < tmpdR){
		    maxdR = tmpdR; 
		} 
		if(mindEta > tmpdEta){
		    mindEta = tmpdEta;
		}
		if(maxdEta < tmpdEta){
		    maxdEta = tmpdEta; 
		} 
		if(mindPhi > tmpdPhi){
		    mindPhi = tmpdPhi;
		}
		if(maxdPhi < tmpdPhi){
		    maxdPhi = tmpdPhi; 
		} 
		nObject++;
	    }
	}
	statsComb.dR        = tmpdR;
	statsComb.meandR    = sumdR   / (float) nObject;
	statsComb.meandEta  = sumdEta / (float) nObject;
	statsComb.meandPhi  = sumdPhi / (float) nObject;
	statsComb.mindR     = mindR;
	statsComb.mindEta   = mindEta;
	statsComb.mindPhi   = mindPhi;
	statsComb.maxdR     = maxdR;
	statsComb.maxdEta   = maxdEta;
	statsComb.maxdPhi   = maxdPhi;
	statsComb.mindRpT   = mindRpT;
	statsComb.mindRMass = mindRMass;
    }


    ///Starting to calculate Fox Wolfram moments///
    template <class object>
	void getFoxWolfram (std::vector<object*>* cont, foxWolframObjects& foxwolf){ //, double &h0, double &h1, double &h2, double &h3, double &h4){
	double jetEnergy = 0.0;
	//double costh;
	double h0 = 0.0, h1 = 0.0, h2 = 0.0, h3 = 0.0, h4 = 0.0;	
	double r1 = 0.0, r2 = 0.0, r3 = 0.0, r4 = 0.0;
	//	std::cout << "debug: " << cont->size() << std::endl;

	for (int oindex = 0; oindex < cont->size(); oindex++) {
	    jetEnergy += cont->at(oindex)->getp4()->E();
        }

	for(int oindex = 0; oindex < cont->size()-1; oindex++){
	    for(int iindex = oindex+1; iindex < cont->size(); iindex++){
		double costh = cos(cont->at(oindex)->getp4()->Angle(cont->at(iindex)->getp4()->Vect()));
		double p0 = 1.0;
		double p1 = costh;
		double p2 = 0.5*(3.0*costh*costh - 1.0);
		double p3 = 0.5*(5.0*costh*costh*costh - 3.0*costh);
		double p4 = 0.125*(35.0*costh*costh*costh*costh - 30.0*costh*costh + 3.0);
		double pipj = cont->at(oindex)->getp4()->P() * cont->at(iindex)->getp4()->P();
		h0 += (pipj/(jetEnergy*jetEnergy))*p0;
		h1 += (pipj/(jetEnergy*jetEnergy))*p1;
		h2 += (pipj/(jetEnergy*jetEnergy))*p2;
		h3 += (pipj/(jetEnergy*jetEnergy))*p3;
		h4 += (pipj/(jetEnergy*jetEnergy))*p4;
	    }
	}

	r1 = h1/h0;
	r2 = h2/h0;
	r3 = h3/h0;
	r4 = h4/h0;

	foxwolf.h0 = h0;
	foxwolf.h1 = h1;
	foxwolf.h2 = h2;
	foxwolf.h3 = h3;
	foxwolf.h4 = h4;
	foxwolf.r1 = r1;
	foxwolf.r2 = r2;
	foxwolf.r3 = r3;
	foxwolf.r4 = r4;
    }


    template <class object>
	void getStats (std::vector<object*>* cont, statObjects& stats){
	float mindR = 9999999999, mindEta = 9999999999, mindPhi = 99999999999., tmpdR = 0., tmpdPhi = 0., tmpdEta = 0., tmpMass = 0., tmpPT =0., sumdEta = 0., sumdPhi = 0., sumdR = 0., maxdR = 0. , maxdPhi = 0., maxdEta = 0., mindRMass = 0., mindRpT = 0.;
	int iindex = 0, nObject = 0;
	for(int oindex=0; oindex < cont->size(); oindex++){
	    for(iindex = oindex+1; iindex < cont->size(); iindex++){
		tmpdPhi = fabs(cont->at(iindex)->getp4()->Phi() - cont->at(oindex)->getp4()->Phi());
		tmpdEta = fabs(cont->at(iindex)->getp4()->Eta() - cont->at(oindex)->getp4()->Eta());
		tmpdR = TMath::Sqrt(tmpdPhi*tmpdPhi + tmpdEta*tmpdEta);
		tmpMass = (*cont->at(oindex)->getp4() + *cont->at(iindex)->getp4()).M(); 
		tmpPT   = (*cont->at(oindex)->getp4() + *cont->at(iindex)->getp4()).Pt(); 
		
		sumdR   += tmpdR;
		sumdEta += tmpdEta;
		sumdPhi += tmpdPhi;
		if(mindR > tmpdR){
		    mindR = tmpdR;
		    mindRMass = tmpMass;
		    mindRpT = tmpPT;
		}
		if(maxdR < tmpdR){
		    maxdR = tmpdR; 
		} 
		if(mindEta > tmpdEta){
		    mindEta = tmpdEta;
		}
		if(maxdEta < tmpdEta){
		    maxdEta = tmpdEta; 
		} 
		if(mindPhi > tmpdPhi){
		    mindPhi = tmpdPhi;
		} 
		if(maxdPhi < tmpdPhi){
		    maxdPhi = tmpdPhi; 
		} 
		nObject++;
	    }
	}
	stats.meandR    = sumdR   / (float) nObject;
	stats.meandEta  = sumdEta / (float) nObject;
	stats.meandPhi  = sumdPhi / (float) nObject;
	stats.mindR     = mindR;
	stats.mindEta   = mindEta;
	stats.mindPhi   = mindPhi;
	stats.maxdR     = maxdR;
	stats.maxdEta   = maxdEta;
	stats.maxdPhi   = maxdPhi;
	stats.mindRpT   = mindRpT;
	stats.mindRMass = mindRMass;
    }

    void summarize(){

	//	std::cout << "nJet: " << getnJet() << std::endl;//" jet scalar sum: " << _sumJetScalarpT << std::endl;
	//std::cout << "nSelectedJet: " << getnSelJet() << " jet selected scalar sum: " << _sumSelJetScalarpT << std::endl;
	//std::cout << "nbJet: " << getnSelbJet() << std::endl;
    //	std::cout << "nElectron: "<< getnElectron() << " nMuon: " << getnMuon() << " nLepton: " << getnLepton() << std::endl;
    //	statObjects jetStat;
    //	getStats(getSelJets(), jetStat);
	//std::cout << "min Jet dr: " << jetStat.mindR << std::endl;
    }

    float getbTagSys(){
	return _bTagSysW;
    }

    float getbTagSys() const {
        return _bTagSysW;
    }

    void setbTagSys(float bTagSysWeight){
	_bTagSysW = bTagSysWeight;
    }

    void setTrigger(bool accept){
	_trigger = accept;
    }
    
    bool getTriggerAccept(){
	return _trigger;
    }

    void setMuonTrigger(bool acceptMuon){
	_triggerMuon = acceptMuon;
    }
    
    bool getMuonTriggerAccept(){
	return _triggerMuon;
    }

    void setHadTrigger(bool acceptHad){
        _triggerHad = acceptHad;
    }
    
    bool getHadTriggerAccept(){
        return _triggerHad;
    }


    void setFilter(bool clean){
	_filter = clean;
    }

    bool getMETFilter(){
	return _filter;
    }

    void setPV(int passPV){
	_pv = passPV;
    }

    float getPVvalue(){
	return _pv;
    }

 private:
    std::vector<objectGenPart*>  _selectGenParts; 
    std::vector<objectJet*>                _jets;
    std::vector<objectJet*>               _bjets;
    objectMET*                              _MET = nullptr;
    std::vector<objectLep*>               _muons;
    std::vector<objectLep*>           _electrons; 
    std::vector<objectJet*>          _selectJets;
    std::vector<float>     _selectJetsMass;
    std::vector<objectJet*>         _selectbJets;
    std::vector<objectBoostedJet*>   _selectHadronicHiggses;
    std::vector<objectBoostedJet*>   _selectBoostedJets;
    std::vector<objectJet*>     _selectLightJets;
    std::vector<float>     _selectLightJetsMass;
    std::vector<objectJet*>          _loosebJets;
    std::vector<objectLep*>     _selectElectrons; 
    std::vector<objectLep*>         _selectMuons; 
    std::vector<objectLep*>       _selectLeptons;

    bool _trigger = false, _filter = false;    
    bool _triggerMuon = false, _triggerHad = false;
    int _pv = -1;

    float  _sumJetScalarpT=0., _sumSelJetScalarpT=0., _sumSelbJetScalarpT=0.,_sumSelHadronicHiggsScalarpT=0., _sumSelLightJetScalarpT=0., _sumSelMuonScalarpT=0., _sumSelElectronScalarpT=0., _sumSelJetMass=0., _sumSelbJetMass=0., _sumSelHadronicHiggsMass=0., _sumSelLightJetMass=0., _bTagSysW = 1. , _sumSelHadronicHiggsSoftDropMass=0;

    //    int nJet = 0, nbJet = 0, nSelJet = 0, nSelbJet = 0;
    int _nVetoLepton = 0;
    TLorentzVector _sumJetp4, _sumSelJetp4, _sumSelbJetp4, _sumHadronicHiggsp4, _sumLightJetp4, _sumSelMuonp4, _sumSelElectronp4; 
};

// [STEP2] ValidationConfig struct 제거됨 — kValidationStudy 모드 삭제.
//         원형은 docs/backup_20260611/ttHHanalyzer_unified.h 참조.


class ttHHanalyzer_unified {
 public:
    enum sysName { kJES, kJER, kbTag, noSys };
    ttHHanalyzer_unified(
        const std::string & cl, 
        eventBuffer * ev, 
        float weight,           
        bool systematics,       
        std::string runYear,    
        std::string DataOrMC,   
        std::string sampleName, 
        std::string era,        
        bool debug,             
        AnalysisMode mode       
    ) :
        // Analysis Mode and Policy Initializer
        _analysisMode(mode),
        _policy(SelectionPolicy::fromMode(mode))
    {
	//_weight = weight;
	_baseWeight = weight; // 데이터셋 공통 상수 (CrossSection * Lumi / SumGenWeight)
	_ev = ev;
	_cl = cl;
	// We need to add in future
	_sys = false;

	_of = new outputFile(_cl);
	_runYear = runYear;
	_DataOrMC = DataOrMC;
	_sampleName = sampleName;
	_era = trimWhitespace(era);

    // [STEP2][debug] kDebug 모드: 디버그 로거 활성화.
    // 이벤트 단위 상세 출력 수는 환경변수 TTHH_DEBUG_NEVENTS (기본 10).
    if (_analysisMode == AnalysisMode::kDebug) {
        long nDbg = 10;
        if (const char* sEnv = std::getenv("TTHH_DEBUG_NEVENTS")) nDbg = std::atol(sEnv);
        _dbg.enable(nDbg);
    }

    // [muon-val] env 옵션 — main/debug에서만 활성.
    if (_analysisMode == AnalysisMode::kMainAnalysis ||
        _analysisMode == AnalysisMode::kDebug) {
        if (const char* sEnv = std::getenv("TTHH_REQUIRE_1MUON")) {
            _require1Muon = (std::atoi(sEnv) != 0);
        }
        if (_require1Muon) {
            _policy.collectLeptons      = true;   // muon 수집 필요
            _policy.requireLeadMuonOnly = true;   // muon gate만 (electron 무시)
            std::cout << "[muon-val] TTHH_REQUIRE_1MUON=1 -> lepton veto를 "
                         "'정확히 muon 1 + electron 0' 으로 대체 (SF 추가 없음)\n";
        }

        // [lepton-CR] 영역은 CLI --region 으로 setRegion() 통해 주입 (생성자 후).

        // [debug-only] b-tag norm reweight 우회 토글
        if (const char* sEnv = std::getenv("TTHH_SKIP_BTAGRW")) {
            _skipBtagReweight = (std::atoi(sEnv) != 0);
        }
        if (_skipBtagReweight) {
            std::cout << "[debug-only] TTHH_SKIP_BTAGRW=1 -> b-tag norm reweight "
                         "건너뜀 (btagNormReweight=1.0). ⚠ 정식 분석 아님: "
                         "8-group JSON 준비 후 반드시 끌 것.\n";
        }
    }
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "  Initializing ttHH Analyzer" << std::endl;
    std::cout << "  Mode: " << analysisModeName(_analysisMode) << std::endl;
    _policy.print();

	if (_era == "noEra" || _era == "-") {
        _era.clear();
    }

    std::string sampleEra = extractEraFromSampleName(_sampleName);
    if (_DataOrMC == "MC") {
        if (!_era.empty()) {
            std::cerr << "[ERROR] MC samples must not define an eraName. Provided eraName: " << _era << std::endl;
            std::exit(EXIT_FAILURE);
        }
    } else if (_DataOrMC == "Data") {
        if (_era.empty() || sampleEra.empty() || sampleEra != _era) {
            std::cerr << "[ERROR] eraName mismatch between config and sampleName. "
                      << "eraName: " << _era << ", sampleName: " << _sampleName << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    std::string yearForCorr = "";
    bool isData = false;
    if(_runYear == "2017") yearForCorr = "2017_UL";
	if(_DataOrMC == "Data") isData = true;

    // [변경] sampleName을 4번째 인자로 전달 → process별 b-tag reweight 조회용
    // [STEP4] main/debug는 파생 보정(trigger SF, b-tag norm RW) 필수 —
    // 누락 시 CorrectionsManager가 FATAL(47/48)로 종료한다.
    // btagtrig/prescan은 부트스트랩 허용 (그 보정의 입력을 만드는 모드).
    const bool requireDerived = (_analysisMode == AnalysisMode::kMainAnalysis ||
                                 _analysisMode == AnalysisMode::kDebug);
    corrMgr = new CorrectionsManager(yearForCorr, _era, isData, _sampleName, requireDerived);

	debugCorrections = debug;

	initHistograms();	
	initTree();
       	std::string dummy = "";

    std::cout << "========================================" << std::endl;
    }


    // [tt+nb] Load the per-sample extended-ttbarId lookup (ttnb_<sample>.root).
    // Empty path -> lookup INACTIVE (every event uses NanoAOD genTtbarId), which
    // is safe for Data and for samples without a lookup. label = sampleName, used
    // for log messages and the per-hit genTtbarId self-check.
    void setExpandedTtbarIdFile(const std::string& path) {
        _expTtbarId.load(path, _sampleName);
    }

    // [tt+nb] Resolve the lookup from the project directory by sampleName:
    //   <dir>/ttnb_<sampleName>.root   (default dir DerivedCorr/expandedTtbarId).
    // Missing file -> INACTIVE (logged). This is the normal entry point.
    void setExpandedTtbarIdDir(const std::string& dir) {
        _expTtbarId.loadFromDir(dir, _sampleName);
    }

    // [stitch] Load the ttbar stitching multiplier JSON for this sample.
    //   stitch_factors_2017.json (compute_stitch_factors.py). Fatal-exit on a
    //   missing/garbled file (Condor-catchable). Call only for non-prescan modes
    //   (prescan produces the inputs to this JSON, so it need not exist yet).
    void setStitchFactorsFile(const std::string& path) {
        _stitch.load(path, _sampleName);
    }

    ~ttHHanalyzer_unified() {
        if(debugCorrections) std::cout<<"debug : Before [ delete corrMgr ] in the ~ttHHanalyzer_unified()"<<std::endl;	    
	delete corrMgr;
        if(debugCorrections) std::cout<<"debug : After [ delete corrMgr ] in the ~ttHHanalyzer_unified()"<<std::endl;	    
    }

    void createObjects(event*,sysName,bool);
    bool selectObjects(event*);
    void analyze(event*);
    void process(event*, sysName, bool);
    void loop(sysName, bool);
    void performAnalysis();
    void fillHistos(event * thisevent);
    void writeHistos();
    void fillTree(event * thisevent);
    void writeTree();
   
    std::vector<TH1D*> h_JEC_DiffRatio;
    std::vector<TH1D*> h_JEC_Mass_DiffRatio; // Mass 변경 확인용

    TH1F * hmet,* hmetPhi, *hmetEta, *hAvgDeltaRjj, *hAvgDeltaRbb,*hAvgDeltaRbj, *hAvgDeltaEtajj, *hAvgDeltaEtabb, *hAvgDeltaEtabj, *hminDeltaRjj, *hminDeltaRbb, *hminDeltaRbj,  *hminDeltaRpTjj, *hminDeltaRpTbb, *hminDeltaRpTbj, *hminDeltaRMassjj, *hminDeltaRMassbb,*hminDeltaRMassbj, *hmaxDeltaEtajj, *hmaxDeltaEtabb, *hmaxDeltaEtabj, *hmaxPTmassjbb, *hmaxPTmassjjj, *hjetAverageMass, *hBjetAverageMass, *hHadronicHiggsAverageMass, *hLightJetAverageMass, *hBjetAverageMassSqr, *hHadronicHiggsSoftDropMass1, *hHadronicHiggsSoftDropMass2, *hjetHT, *hBjetHT, *hHadronicHiggsHT, *hLightJetHT, *hjetNumber, *hBjetNumber, *hHadronicHiggsNumber, *hLightJetNumber, *hInvMassHadW, *hInvMassZ1, *hInvMassZ2,*hInvMassZ1_zoomIn, *hInvMassZ2_zoomIn, *hInvMassHSingleMatched,*hInvMassHSingleNotMatched ,*hChi2HiggsSingleNotMatched, *hChi2HiggsSingleMatched , *hInvMassH1, *hInvMassH2,*hInvMassH1_zoomIn, *hInvMassH2_zoomIn, *hInvMassHZ1, *hInvMassHZ2, *hInvMassHZ1_zoomIn, *hInvMassHZ2_zoomIn, *hInvMassH1mChi, *hInvMassH2mChi,*hPTH1, *hPTH2, *hChi2Higgs, *hChi2HiggsZ, *hChi2HadW, *hChi2Z, *hAplanarity, *hSphericity, *hTransSphericity, *hCvalue, *hDvalue, *hBjetAplanarity, *hBjetSphericity, *hBjetTransSphericity ,*hBjetCvalue, *hBjetDvalue, *hCentralityjl, *hCentralityjb, *hleptonNumber, *hLeptonPT1, *hMuonPT1, *hElePT1, *hLeptonPhi1, *hMuonPhi1, *hElePhi1, *hLeptonEta1, *hMuonEta1, *hEleEta1, *hLeptonPT2, *hMuonPT2, *hElePT2, *hLeptonPhi2, *hMuonPhi2, *hElePhi2, *hLeptonEta2, *hMuonEta2, *hEleEta2, *hLepCharge1, *hLepCharge2, *hleptonHT, *hST, *hDiMuonMass, *hDiElectronMass, *hDiMuonPT, *hDiElectronPT, *hDiMuonEta, *hDiElectronEta, *hH0, *hH1, *hH2, *hH3, *hH4, *hR1, *hR2, * hR3, *hR4, *hBjetH0, *hBjetH1, *hBjetH2, *hBjetH3, *hBjetH4, *hBjetR1, *hBjetR2, * hBjetR3, *hBjetR4, *hCutFlow, *hCutFlow_w,
    *hCutFlow_w_btagSF, *hCutFlow_w_full,    // [NEW] SF-aware cutflows
	*hInvMassHH1Matched,
	*hInvMassHH1NotMatched,
	*hInvMassHH2Matched,
        *hInvMassHH2NotMatched,
        *hChi2HHNotMatched,
        *hChi2HHMatched;


// No need now, I'll update cutflow logic


 private: 
    bool _sys;
    //float _weight;
    float _evtWeight;
    // ── Per-chain event weights (new, for SF-aware cutflow) ───────────
    // Filled inside selectObjects between step kHT and kNumbJets2.
    // _evtWeight stays as the legacy per-step weight (post-Step8 SFs);
    // these three additional fields drive the new parallel cutflows.
    double _evtWeight_chain_raw      = 1.0;  // baseline weight only
    double _evtWeight_chain_btagSF   = 1.0;  // + b-tag shape SF
    double _evtWeight_chain_full     = 1.0;  // + trigSF + btagNormRW
    float _baseWeight; // [추가] 데이터셋 공통 상수 (CrossSection * Lumi / SumGenWeight)
    float _SampleWeight;
    float _PUWeight;
    float _L1PrefiringWeight;
    float _genWeight;
    bool  _failGoldenJson;
    bool  _passMETFilters;
    std::string _DataOrMC, _runYear, _sampleName, _era;

    // ── [tt+nb] extended ttbar-Id ────────────────────────────────────────
    ExpandedTtbarId _expTtbarId;        // per-sample lookup; inactive until load()
    StitchFactors   _stitch;            // [stitch] per-(sample,category) multiplier
    std::map<int,std::string> _btagKeyByExpSub;  // [stitch] diag: expandedSub -> b-tag processKey
    Int_t _genTtbarIdNano  = -1;        // NanoAOD genTtbarId (mirror, for output tree)
    Int_t _expandedTtbarId = -1;        // resolved id: Expanded if in lookup, else nano
    // [STEP6] Higgs 재구성기 + 추가 가설(ZH/ZZ) 결과 — selectObjects에서 채움.
    // HH 결과는 기존 멤버(_minChi2Higgs 등)로 들어가 하위 호환 유지.
    HiggsReconstructor _higgsReco;
    Float_t _hr_chi2ZH = -1.f, _hr_mZcandZH = -1.f, _hr_mHcandZH = -1.f;
    Float_t _hr_chi2ZZ = -1.f, _hr_mZ1ZZ    = -1.f, _hr_mZ2ZZ    = -1.f;

    // [STEP5] Event shape 변수 — analyze()에서 selected jets / b-jets로 계산,
    // output tree branch로 기록 (DNN 입력 후보). -1 = 계산 불가(객체 부족).
    Float_t _es_aplanarity      = -1.f, _es_sphericity      = -1.f;
    Float_t _es_transSphericity = -1.f, _es_C = -1.f, _es_D = -1.f;
    Float_t _es_bjetAplanarity      = -1.f, _es_bjetSphericity = -1.f;
    Float_t _es_bjetTransSphericity = -1.f, _es_bjetC = -1.f, _es_bjetD = -1.f;

    Float_t _stitchWeight  = 1.0f;      // [stitch] per-event stitch multiplier written to
                                        // the output tree. 1.0 for Data / non-plan samples /
                                        // modes where stitching is gated off (trigsf, ...).
                                        // Downstream tools rebuild the clean stitched base:
                                        //   base = genWeight*PU*L1Prefire*xsec * stitchWeight
                                        // (evtWeight already has btagSF/trigSF/normRW in it).
    // Analysis Mode Variable Declaration
    AnalysisMode _analysisMode;
    SelectionPolicy _policy;    

// ═══════════════════════════════════════════════════════════════════════
// [kPrescan] Gen-level accumulators
// ⚠ TEMPORARY — to be migrated to NtupleForge (Runs friend tree).
// ⚠ Active only when _analysisMode == kPrescan.
//
// AN ref: ttHH AN-2022/122 §3.3, ttH AN-19-094 §6.2.1.
//         Stitching factors r_B, r_4b require gen-level Σ genWeight
//         per genTtbarId %% 100 bin — partition decision is made by
//         post-processing on hadded prescan trees.
// ═══════════════════════════════════════════════════════════════════════

// Runs tree sums (NanoAOD original, skim-independent)
Double_t   _prescan_runs_sumW    = 0.0;
Double_t   _prescan_runs_sumW2   = 0.0;
Long64_t  _prescan_runs_count   = 0;
Int_t      _prescan_nFilesProcessed = 0;

// Events tree direct sum (== Runs sumW iff no skim was applied upstream)
Long64_t  _prescan_nEvents_total = 0;
Double_t  _prescan_sumGW_total = 0.0;
Double_t  _prescan_sumGW_pos   = 0.0;
Double_t  _prescan_sumGW_neg   = 0.0;

// 11-bin raw by genTtbarId %% 100 (the actual stitching-decision input)
Double_t _prescan_sumGW_id_lt0   = 0.0;   // id < 0 or branch absent
Double_t _prescan_sumGW_id_0     = 0.0;   // tt+LF
Double_t _prescan_sumGW_id_41    = 0.0;
Double_t _prescan_sumGW_id_42    = 0.0;
Double_t _prescan_sumGW_id_43    = 0.0;
Double_t _prescan_sumGW_id_44    = 0.0;
Double_t _prescan_sumGW_id_45    = 0.0;   // tt+cc (41-45)
Double_t _prescan_sumGW_id_51    = 0.0;   // tt+b (1 b-jet, 1 b-hadron)
Double_t _prescan_sumGW_id_52    = 0.0;   // tt+b (1 b-jet, ≥2 b-hadrons)
Double_t _prescan_sumGW_id_53    = 0.0;   // tt+2b
Double_t _prescan_sumGW_id_54    = 0.0;   // tt+bb
Double_t _prescan_sumGW_id_55    = 0.0;   // tt+4b
// [tt+nb] expanded partition (events moved out of 53/54/55 by the lookup)
Double_t _prescan_sumGW_id_61    = 0.0;   // tt+bbb
Double_t _prescan_sumGW_id_62    = 0.0;   // tt+bbb (multi)
Double_t _prescan_sumGW_id_71    = 0.0;   // tt+4b
Double_t _prescan_sumGW_id_72    = 0.0;   // tt+4b  (multi)
Double_t _prescan_sumGW_id_other = 0.0;   // safety net

// 11-bin raw COUNT by genTtbarId %% 100 — unweighted companion to the
// _prescan_sumGW_id_* bins above. Same partition; needed for the
// stitching-partition event-count bookkeeping (entries per category).
Long64_t _prescan_n_id_lt0   = 0;   // id < 0 or branch absent
Long64_t _prescan_n_id_0     = 0;   // tt+LF
Long64_t _prescan_n_id_41    = 0;
Long64_t _prescan_n_id_42    = 0;
Long64_t _prescan_n_id_43    = 0;
Long64_t _prescan_n_id_44    = 0;
Long64_t _prescan_n_id_45    = 0;   // tt+cc (41-45)
Long64_t _prescan_n_id_51    = 0;   // tt+b (1 b-jet, 1 b-hadron)
Long64_t _prescan_n_id_52    = 0;   // tt+b (1 b-jet, ≥2 b-hadrons)
Long64_t _prescan_n_id_53    = 0;   // tt+2b
Long64_t _prescan_n_id_54    = 0;   // tt+bb
Long64_t _prescan_n_id_55    = 0;   // tt+4b
// [tt+nb] expanded partition counts
Long64_t _prescan_n_id_61    = 0;   // tt+bbb
Long64_t _prescan_n_id_62    = 0;   // tt+bbb (multi)
Long64_t _prescan_n_id_71    = 0;   // tt+4b
Long64_t _prescan_n_id_72    = 0;   // tt+4b  (multi)
Long64_t _prescan_n_id_other = 0;   // safety net

// 5-bucket ntuple ttCat_* cross-check (weighted + raw count)
Double_t _prescan_sumGW_ttCat_LF    = 0.0;
Double_t _prescan_sumGW_ttCat_Cj    = 0.0;
Double_t _prescan_sumGW_ttCat_1B1H  = 0.0;
Double_t _prescan_sumGW_ttCat_1B2H  = 0.0;
Double_t _prescan_sumGW_ttCat_2B    = 0.0;
Double_t _prescan_sumGW_ttCat_NoTT  = 0.0;

Long64_t _prescan_n_ttCat_LF    = 0;
Long64_t _prescan_n_ttCat_Cj    = 0;
Long64_t _prescan_n_ttCat_1B1H  = 0;
Long64_t _prescan_n_ttCat_1B2H  = 0;
Long64_t _prescan_n_ttCat_2B    = 0;
Long64_t _prescan_n_ttCat_NoTT  = 0;

// Helpers
void runPrescan();
void readRunsTreeSums();
void accumulatePrescanEvent();
void writePrescanTree();


    // [STEP2][debug] kDebug 모드 단계별 검증 로거 (다른 모드에선 no-op)
    DebugLogger _dbg;

    // [muon-val] offline single-muon validation 옵션 (env TTHH_REQUIRE_1MUON=1).
    // main/debug에서만 의미. 켜지면 lepton veto cut(step kLeptonVeto)이
    //   "정확히 muon 1개 + electron 0개" 요구로 바뀐다 (그 외 selection·weight는
    //   불변; 별도 SF 없음). AN trigger 측정 영역(1μ+FH baseline)을 offline에서
    //   흉내내는 용도 — 사용자 지시(별도 모드 만들지 않음).
    bool _require1Muon = false;

    // [lepton-CR] QCD 억제용 1-lepton 제어영역 (CLI --region, setRegion()로 주입).
    //   ""       : 기본 FH (lepton veto = 0 lepton)
    //   "muon"     : 정확히 muon 1 + electron 0, 그리고 MET_pt > metCutCR
    //   "electron" : 정확히 electron 1 + muon 0, 그리고 MET_pt > metCutCR
    // QCD multijet 은 진짜 lepton·MET 가 거의 없으므로 1ℓ+MET 요구로 크게 준다.
    // SF 추가 없음 (trigger SF 등 기존 체인 그대로). main/debug 에서만 활성.
    std::string _lepCRmode = "";          // "", "muon", "electron"
    static constexpr float metCutCR = 20.0f;   // GeV — 1ℓ CR 의 MET 하한
public:
    // [lepton-CR] CLI --region 값 주입 (main()에서 생성자 후 호출). main/debug 에서만 효과.
    void setRegion(const std::string& r) {
        if (r.empty()) return;
        if (r != "muon" && r != "electron") {
            std::cout << "[lepton-CR][WARN] --region '" << r
                      << "' 무시 (muon|electron 만 허용)\n";
            return;
        }
        if (_analysisMode != AnalysisMode::kMainAnalysis &&
            _analysisMode != AnalysisMode::kDebug) {
            std::cout << "[lepton-CR][WARN] --region 은 main/debug 에서만 적용 (무시)\n";
            return;
        }
        _lepCRmode = r;
        _policy.collectLeptons = true;   // lepton 수집 필요
        std::cout << "[lepton-CR] --region=" << _lepCRmode
                  << " -> lepton veto를 '정확히 " << _lepCRmode
                  << " 1개 + 반대 flavor 0개 + MET_pt>" << metCutCR
                  << " GeV' 로 대체 (QCD 억제, SF 추가 없음)\n";
    }
private:

    // [debug-only] b-tag norm reweight 임시 우회 (env TTHH_SKIP_BTAGRW=1).
    // ⚠ 정식 분석 금지 — 8-group JSON(tt+nb 포함)이 아직 없을 때, 나머지
    //   흐름(stitch/trigSF/btagShape/selection)이 끝까지 도는지 확인하는 용도.
    //   켜지면 btagNormReweight_=1.0 으로 두고 getBTagReweight() 호출을 건너뛴다
    //   (= reweight 미적용). 끄면(기본) 기존대로 동작 + 키 없으면 FATAL 46.
    bool _skipBtagReweight = false;

    TH1D * _hJES, * _hbJES, *_hbJetEff, *_hJetEff, *_hSysbTagM ;

    static const int nHistsJets = 12; // ideal # of final state --> 10
    static const int nHistsbJets = 8; // ideal # of final state --> 6
    static const int nHistsLightJets = 6; // ideal # of final state --> 4
    std::vector<TH1F*> hjetsPTs, hjetsEtas, hbjetsPTs, hbjetsEtas, hLightJetsPTs, hLightJetsEtas, hjetsBTagDisc, hbjetsBTagDisc, hLightJetsBTagDisc;
    event::evShapes jlepCent, jbjetCent;
    event::maxObjects jbbMaxs, jjjMaxs;
    event::statObjects jetStat, bjetStat, bjStat, ljetStat, lbjetStat, genPbjetStat; 
    event::foxWolframObjects jetFoxWolfMom, bjetFoxWolfMom;
    std::string _cl;
    eventBuffer * _ev;

    // For corrections////////////////
    CorrectionsManager *corrMgr;      
    bool debugCorrections;         

    // B-tag weight 저장용 (Tree branch)
    float bTagWeight_central_;
    float bTagWeight_up_hf_;
    float bTagWeight_down_hf_;
    float bTagWeight_up_lf_;
    float bTagWeight_down_lf_;
    float bTagWeight_up_cferr1_;
    float bTagWeight_down_cferr1_;
    float bTagWeight_up_cferr2_;
    float bTagWeight_down_cferr2_;
    // B-tag weight 계산 함수
    void computeBTagWeight(event* thisEvent);

    // ═══════════════════════════════════════════════════════════════════════
    // [NEW] Trigger SF & B-tag Normalization Reweight
    // selectObjects() 내에서 계산되어 저장됨.
    // Tree branch로 출력하여 downstream에서 weight 분리 가능.
    // ═══════════════════════════════════════════════════════════════════════
    float triggerSF_          = 1.0f;   // Trigger Scale Factor (HT × jet6PT 기반)
    float triggerSF_up_       = 1.0f;   // Trigger SF +1σ variation
    float triggerSF_down_     = 1.0f;   // Trigger SF -1σ variation
    float btagNormReweight_   = 1.0f;   // B-tag 정규화 비율 (normalization ratio)


    outputFile * _of;
    float _bbMassMinSHiggsNotMatched, _bbMassMinSHiggsMatched, _minChi2SHiggsNotMatched = 999999999. , _minChi2SHiggsMatched = 999999999.; 
    float _bbMassMinHH1NotMatched, _bbMassMinHH1Matched,_bbMassMinHH2NotMatched, _bbMassMinHH2Matched, _minChi2HHNotMatched = 999999999. , _minChi2HHMatched = 999999999.; 

    float _bbMassMin1Higgs, _bbMassMin2Higgs, _minChi2Higgs = 999999999.;
    float _bpTHiggs1, _bpTHiggs2;
    float _bbMassMin1HiggsZ, _bbMassMin2HiggsZ, _minChi2HiggsZ = 999999999.;
    float _bbMassMin1Z, _bbMassMin2Z, _minChi2Z = 999999999.;
    TRandom3 _rand;

    enum class CutStep {
        kNoCut = 0,         //  0  no selection
        kHadTrigger,        //  1  hadronic trigger fired
        kNoiseFilter,       //  2  MET filter
        kPrimaryVertex,     //  3  ≥1 primary vertex
        kNumJets,           //  4  ≥6 jets  (baseline)
        kSixthJetPt,        //  5  jet6 pT > 40
        kLeptonVeto,        //  6  no veto lepton
        kHT,                //  7  HT ≥ 500
        kNumbJets2,         //  8  ≥2 b-tags  (ttH baseline)
        kNumbJets3,         //  9  ≥3 b-tags
        kNumbJets4,         // 10  ≥4 b-tags  (SR boundary)
        kHadWMass,          // 11  30 < hadW < 250
        kHiggsMass,         // 12  Higgs mass window (currently off in main)
        kTotal              // 13  total
    };

    // ─────────────────────────────────────────────────────────────────────
    // [STEP3] 선언적 cut 테이블 — selection 시퀀스의 단일 정의
    //   enforceIn        : cut으로 강제되는 모드 비트 (그 외 모드에선 관찰만)
    //   recordOnlyIfPass : true = 조건 충족 시에만 cutflow 기록 (≥3/≥4 관찰 단계)
    //   pass             : 통과 조건 (nullptr = 항상 통과; 기록 전용 단계)
    //   onAfter          : 해당 단계 생존 직후의 부수 작업 (통계/SF 적용)
    // 실제 테이블(kCutSequence)은 ttHHanalyzer_unified.cc의 selectObjects 위에
    // 정의 — selection 전체가 거기서 한 화면의 표로 보인다.
    // ─────────────────────────────────────────────────────────────────────
    struct CutDef {
        CutStep      step;
        const char*  label;             // _cutStepLabels와 동일해야 (사람용)
        uint8_t      enforceIn;
        bool         recordOnlyIfPass;
        bool (*pass)(ttHHanalyzer_unified&, event&, float wMass);
        void (*onAfter)(ttHHanalyzer_unified&, event&);
    };
    static const std::vector<CutDef> kCutSequence;
    void applyEventScaleFactors(event* thisEvent);  // [STEP3] SF 블록 (HT 통과 직후)
    void computeLeptonJetStats(event* thisEvent);   // [STEP3] lepton-jet 통계

    // cutStepLabels must match above CutStep!!!
    const std::vector<std::string> _cutStepLabels = {
        "noCut",
        "HadTrigger",
        "noiseFilter",
        "pv>=1",
        "njets>=6",
        "6thJetsPT>40",
        "nlepton==0",
        "HT>500",
        "nbjets>=2",
        "nbjets>=3",
        "nbjets>=4",
        "30<HadW<250",
        "HiggsMassWindow",
        "nTotal"
    };


    // [수정] 라벨 벡터는 const static 혹은 생성자에서 초기화하도록 변경 권장하나,
    // 기존 구조를 존중하여 멤버 변수로 유지하되 초기화 방식만 바꿉니다.
    // [수정] cutStep별 상위 6개 jet kinematics 히스토그램
    // 기존: vector<TH1F*>  (leading jet만)
    // 변경: vector<array<TH1F*, 6>>  (상위 6개 jet)
    static constexpr size_t kNJetsForCutStep = 6;

    std::vector<std::array<TH1F*, kNJetsForCutStep>> _cutStepJetPt;
    std::vector<std::array<TH1F*, kNJetsForCutStep>> _cutStepJetEta;
    std::vector<std::array<TH1F*, kNJetsForCutStep>> _cutStepJetPhi;
    std::vector<TH1F*> _cutStepHT;
    std::vector<std::array<TH1F*, kNJetsForCutStep>> _cutStepBTag;
    std::vector<TH1F*> _cutStepHadWMass;
    std::vector<TH1F*> _cutStepHiggsMass01;
    std::vector<TH1F*> _cutStepHiggsMass02;

    // [NEW] Per-step multiplicities — n(b-)jets distribution at each step
    // Three parallel sets, one per weight chain (raw / btagSF / full).
    std::vector<TH1F*> _cutStepNbJets_raw;
    std::vector<TH1F*> _cutStepNbJets_btagSF;
    std::vector<TH1F*> _cutStepNbJets_full;
    std::vector<TH1F*> _cutStepNjets_raw;
    std::vector<TH1F*> _cutStepNjets_btagSF;
    std::vector<TH1F*> _cutStepNjets_full;

    // [NEW] Per-step kinematics — second / third weight-chain copies of
    // the existing _cutStepHT / _cutStepHadWMass / _cutStepHiggsMass01/02
    // / _cutStepBTag / _cutStepJetPt etc. Only the WEIGHT differs at fill;
    // we keep them as independent TH1Fs so the plotter can pick them up
    // by suffix (_btagSF, _full).
    //
    // To avoid doubling code volume, we reuse arrays of identical shape:
    std::vector<TH1F*> _cutStepHT_btagSF;
    std::vector<TH1F*> _cutStepHT_full;
    std::vector<TH1F*> _cutStepHadWMass_btagSF;
    std::vector<TH1F*> _cutStepHadWMass_full;
    std::vector<TH1F*> _cutStepHiggsMass01_btagSF;
    std::vector<TH1F*> _cutStepHiggsMass01_full;
    std::vector<TH1F*> _cutStepHiggsMass02_btagSF;
    std::vector<TH1F*> _cutStepHiggsMass02_full;
    // Per-jet (1..6) bTag, pT, eta, phi: keep only the existing _raw set
    // for now — these are heavy (6 jets × N steps × 4 hists = 24×N).
    // The user can extend to btagSF/full later if needed.

    // [추가] 카운터용 벡터 (Map 대신 사용)
    std::vector<double> _cutFlowCount;  // 갯수 (No Weight)
    std::vector<double> _cutFlowWeight; // 가중치 적용 (Weight)

    // ═══════════════════════════════════════════════════════════════════════
    // tt+jets categorization — Option B' five-category schema
    // ═══════════════════════════════════════════════════════════════════════
    //
    // The five mutually exclusive categories follow the AN-2022/122 §3.1
    // and AN-19-094 §6.1.2 definitions but use explicit Bjet/Hadron suffixes
    // to remove the historical ambiguity of names like "tt+2b" (where the
    // "2" refers to b-hadrons inside one jet, not to the number of b-jets).
    //
    //   kLightFlavour   tt+LF   no additional b- or c-jets
    //   kAddCjet        tt+cc   ≥1 additional c-jet, no additional b-jet
    //   kAdd1Bjet1Had   tt+b    1 additional b-jet containing 1 b-hadron
    //   kAdd1Bjet2Had   tt+2b   1 additional b-jet containing ≥2 b-hadrons
    //                            (collinear g→bb merged into one jet)
    //   kAdd2Bjet       tt+bb   ≥2 additional b-jets (covers AN's bb,bbb,4b)
    //
    // The bbb/4b split is NOT recovered here because:
    //   1. The CMS GenTtbarCategorizer plugin (the AN-cited tool, used by
    //      the genTtbarId branch) cannot distinguish them — it only encodes
    //      hadron multiplicity in the leading two b-jets, not jet count.
    //   2. The ttHH AN constructs bbb/4b at sample level (Option1/Option2),
    //      not from per-event GenHFHadronMatcher labelling.
    //   3. The DNN merges them into one tt+nb output node anyway.
    //
    // Acceptance for additional b-jets / c-jets: pT > 20 GeV, |η| < 2.4,
    // ΔR(hadron, gen-jet) < 0.4. These match the ntuplizer constants.
    //
    // ─── Algorithms used here ───────────────────────────────────────────
    //
    // The analyzer implements TWO independent estimators of the category,
    // and reads two more from the ntuple. All four are compared in
    // validation histograms; the decision the analyzer uses downstream
    // (stitching, hist split) is the ntuple "primary" branch ttCat_*.
    //
    //   ANA_GENPART  : analyzer's own GenPart-based algorithm.
    //                  Bit-for-bit equivalent of the ntuplizer's
    //                  _categorize_genpart_xval() function. Computed from
    //                  raw GenPart_*/GenJet_* branches.
    //
    //   ANA_GENID    : decode of genTtbarId%100. Same logic as the
    //                  ntuplizer's decode_genttbarid() function.
    //
    //   NTU_PRIMARY  : ttCat_* branches written by the ntuplizer
    //                  (genTtbarId-based, AN-cited POG path).
    //                  This is what downstream stitching uses.
    //
    //   NTU_XVAL     : ttCatXval_* branches written by the ntuplizer
    //                  (its own GenPart algorithm, validation only).
    //
    // Expected agreement:
    //   ANA_GENID  ≡  NTU_PRIMARY     (both decode the same integer,
    //                                  byte-identical)
    //   ANA_GENPART ≡  NTU_XVAL       (same algorithm in two languages,
    //                                  byte-identical modulo float order)
    //   ANA_GENID vs ANA_GENPART      ~97% (POG vs GenPart, real
    //                                  algorithmic difference)
    // ═══════════════════════════════════════════════════════════════════════
    enum class TtCat {
        kLightFlavour = 0,
        kAddCjet,
        kAdd1Bjet1Had,
        kAdd1Bjet2Had,
        kAdd2Bjet,
        kNoTTJets,
        kUnknown,
        kNCategories
    };

    static constexpr int kNTtCat = static_cast<int>(TtCat::kNCategories);

    static const char* ttCatName(int idx) {
        static const char* names[] = {
            "LightFlavour",
            "AddCjet",
            "Add1Bjet_1Had",
            "Add1Bjet_2Had",
            "Add2Bjet",
            "NoTTJets",
            "Unknown"
        };
        if (idx >= 0 && idx < kNTtCat) return names[idx];
        return "OutOfRange";
    }

    // Compact alias used in cross-validation tables.
    static const char* ttCatShortName(int idx) {
        static const char* names[] = {
            "LF", "Cj", "1B1H", "1B2H", "2B+", "noTT", "?"
        };
        if (idx >= 0 && idx < kNTtCat) return names[idx];
        return "??";
    }

    // --- helper: is PDG ID a B hadron? ---
    static bool isBHadron(int pdgId) {
        int aid = std::abs(pdgId);
        if (aid < 100) return false;
        if (aid < 1000) return (aid / 100) == 5;
        if (aid < 10000) return (aid / 1000) == 5;
        int base = aid % 10000;
        if (base >= 100 && base < 1000) return (base / 100) == 5;
        if (base >= 1000 && base < 10000) return (base / 1000) == 5;
        return false;
    }

    // --- helper: is PDG ID a C hadron (D meson / charm baryon)? ---
    static bool isCHadron(int pdgId) {
        if (isBHadron(pdgId)) return false;
        int aid = std::abs(pdgId);
        if (aid < 100) return false;
        if (aid < 1000) return (aid / 100) == 4;
        if (aid < 10000) return (aid / 1000) == 4;
        int base = aid % 10000;
        if (base >= 100 && base < 1000) return (base / 100) == 4;
        if (base >= 1000 && base < 10000) return (base / 1000) == 4;
        return false;
    }

    // --- helper: deltaR^2 ---
    static float deltaR2(float eta1, float phi1, float eta2, float phi2) {
        float deta = eta1 - eta2;
        float dphi = phi1 - phi2;
        while (dphi > M_PI)  dphi -= 2.0f * M_PI;
        while (dphi < -M_PI) dphi += 2.0f * M_PI;
        return deta * deta + dphi * dphi;
    }

    // --- helper: get actual GenPart/GenJet counts from vector sizes ---
    // NOTE: treestream fills std::vector members per event but does NOT
    // update the int counter members (nGenPart, nGenJet, etc.).
    // Always use vector .size() instead of the counter variables.
    int genPartCount() const { return static_cast<int>(_ev->GenPart_pdgId.size()); }
    int genJetCount()  const { return static_cast<int>(_ev->GenJet_pt.size()); }

    // --- helper: check top ancestor in GenPart chain ---
    bool hasTopAncestor(int idx) const {
        int cur = idx;
        int nGP = genPartCount();
        for (int d = 0; d < 30; ++d) {
            int mother = _ev->GenPart_genPartIdxMother[cur];
            if (mother < 0 || mother >= nGP) return false;
            if (std::abs(_ev->GenPart_pdgId[mother]) == 6) return true;
            cur = mother;
        }
        return false;
    }

    // --- helper: event has tt-bar pair? ---
    bool eventHasTTPair() const {
        bool found_t = false, found_tbar = false;
        for (int i = 0; i < genPartCount(); ++i) {
            int pid = _ev->GenPart_pdgId[i];
            if (pid == 6) found_t = true;
            else if (pid == -6) found_tbar = true;
            if (found_t && found_tbar) return true;
        }
        return false;
    }

    // --- count additional b-jets (not from top decay) ---
    // Returns per-jet B hadron map for correct b/2b distinction
    struct AddBJetResult {
        int nJets;      // unique additional b-jets (matched to GenJets)
        int nHadrons;   // total additional B hadrons (incl. unmatched)
        int nMatched;   // B hadrons successfully DR-matched to GenJets
        // Per-jet B hadron count: key=GenJet index, value=# matched B hadrons
        std::unordered_map<int, int> jetBHMap;
    };

    AddBJetResult countAdditionalBJetsDetailed() const {
        static constexpr float GEN_JET_PT_MIN  = 20.0f;
        static constexpr float GEN_JET_ETA_MAX = 2.4f;
        static constexpr float DR_MATCH_MAX2   = 0.4f * 0.4f;

        AddBJetResult result{0, 0, 0, {}};

        // Step 1: collect additional (non-top-ancestor) last-copy B hadrons
        struct BHadronInfo { float eta; float phi; int pdgId; int gpIdx; };
        std::vector<BHadronInfo> addBH;
        for (int i = 0; i < genPartCount(); ++i) {
            if (!isBHadron(_ev->GenPart_pdgId[i])) continue;
            if (!((_ev->GenPart_statusFlags[i] >> 13) & 1)) continue;   // isLastCopy
            if (hasTopAncestor(i)) continue;
            addBH.push_back({_ev->GenPart_eta[i], _ev->GenPart_phi[i],
                             _ev->GenPart_pdgId[i], i});
        }
        result.nHadrons = static_cast<int>(addBH.size());
        if (addBH.empty()) return result;

        // Step 2: gen b-jets in acceptance
        std::vector<int> bjetIdx;
        for (int j = 0; j < genJetCount(); ++j) {
            if (static_cast<int>(_ev->GenJet_hadronFlavour.size()) <= j) break;
            if (_ev->GenJet_hadronFlavour[j] != 5) continue;
            if (_ev->GenJet_pt[j] < GEN_JET_PT_MIN) continue;
            if (std::fabs(_ev->GenJet_eta[j]) > GEN_JET_ETA_MAX) continue;
            bjetIdx.push_back(j);
        }
        if (bjetIdx.empty()) return result;

        // Step 3: DR match each B hadron to closest b-GenJet
        // Build per-jet B hadron map
        std::unordered_map<int, int> jetBHMap;
        int nMatchedBH = 0;
        for (const auto& bh : addBH) {
            float bestDR2 = DR_MATCH_MAX2;
            int bestJ = -1;
            for (int j : bjetIdx) {
                float dr2 = deltaR2(bh.eta, bh.phi,
                                    _ev->GenJet_eta[j], _ev->GenJet_phi[j]);
                if (dr2 < bestDR2) { bestDR2 = dr2; bestJ = j; }
            }
            if (bestJ >= 0) {
                jetBHMap[bestJ]++;
                nMatchedBH++;
            }
        }

        result.nJets = static_cast<int>(jetBHMap.size());
        result.nMatched = nMatchedBH;
        result.jetBHMap = std::move(jetBHMap);
        return result;
    }

    int countAdditionalBJets() const { return countAdditionalBJetsDetailed().nJets; }

    // --- count additional c-jets (not from top decay) ---
    int countAdditionalCJets() const {
        static constexpr float GEN_JET_PT_MIN  = 20.0f;
        static constexpr float GEN_JET_ETA_MAX = 2.4f;
        static constexpr float DR_MATCH_MAX2   = 0.4f * 0.4f;

        std::vector<std::pair<float,float>> addCH;
        for (int i = 0; i < genPartCount(); ++i) {
            if (!isCHadron(_ev->GenPart_pdgId[i])) continue;
            if (!((_ev->GenPart_statusFlags[i] >> 13) & 1)) continue;
            if (hasTopAncestor(i)) continue;
            addCH.emplace_back(_ev->GenPart_eta[i], _ev->GenPart_phi[i]);
        }
        if (addCH.empty()) return 0;

        std::vector<int> cjetIdx;
        for (int j = 0; j < genJetCount(); ++j) {
            if (_ev->GenJet_hadronFlavour[j] != 4) continue;
            if (_ev->GenJet_pt[j] < GEN_JET_PT_MIN) continue;
            if (std::fabs(_ev->GenJet_eta[j]) > GEN_JET_ETA_MAX) continue;
            cjetIdx.push_back(j);
        }
        if (cjetIdx.empty()) return 0;

        std::set<int> matched;
        for (auto& [ch_eta, ch_phi] : addCH) {
            float bestDR2 = DR_MATCH_MAX2;
            int bestJ = -1;
            for (int j : cjetIdx) {
                float dr2 = deltaR2(ch_eta, ch_phi,
                                    _ev->GenJet_eta[j], _ev->GenJet_phi[j]);
                if (dr2 < bestDR2) { bestDR2 = dr2; bestJ = j; }
            }
            if (bestJ >= 0) matched.insert(bestJ);
        }
        return static_cast<int>(matched.size());
    }

    // ──────────────────────────────────────────────────────────────────
    // Algorithm 1 — analyzer's own GenPart-based categorization
    //
    // Bit-for-bit equivalent of the ntuplizer's _categorize_genpart_xval()
    // function. Operates on raw GenPart_*/GenJet_* branches only; does
    // NOT touch genTtbarId or any ttCat[Xval]_* branch.
    //
    // The decision tree differs from the old 7-category version only in
    // that codes 53/54/55 (≥2 additional b-jets) are no longer split into
    // bb/bbb/4b — they collapse into kAdd2Bjet. See the enum docstring
    // above for the rationale.
    //
    // Returns kNoTTJets for non-tt events and kUnknown if the input
    // GenPart/GenJet vectors are empty (e.g. data NanoAOD).
    // ──────────────────────────────────────────────────────────────────
    TtCat computeTtCategoryFromGenPart() const {
        if (_DataOrMC == "Data") return TtCat::kNoTTJets;

        if (genPartCount() == 0 || genJetCount() == 0) {
            return TtCat::kUnknown;
        }
        if (!eventHasTTPair()) return TtCat::kNoTTJets;

        const auto result = countAdditionalBJetsDetailed();
        const int nBJets = result.nJets;

        // ≥2 additional b-jets — single bucket (no bbb/4b split)
        if (nBJets >= 2) return TtCat::kAdd2Bjet;

        // Exactly 1 additional b-jet — split by per-jet hadron count.
        // Use the per-jet B hadron count (not the total over all hadrons),
        // which is the same convention the ntuplizer encodes via
        // genTtbarId codes 51 (1 hadron) vs 52 (≥2 hadrons in one jet).
        if (nBJets == 1) {
            const int singleJetBHCount = result.jetBHMap.begin()->second;
            if (singleJetBHCount >= 2) return TtCat::kAdd1Bjet2Had;
            return TtCat::kAdd1Bjet1Had;
        }

        // No additional b-jets — check c-jets, else light flavour.
        if (countAdditionalCJets() > 0) return TtCat::kAddCjet;
        return TtCat::kLightFlavour;
    }

    // ──────────────────────────────────────────────────────────────────
    // Algorithm 2 — decode of the genTtbarId integer (POG path)
    //
    // Equivalent to the ntuplizer's decode_genttbarid() function and to
    // CMS GenTtbarCategorizer.cc lines 282-300.
    //
    //   genTtbarId %% 100   meaning
    //   0                   tt+LF (no additional heavy flavour jet)
    //   41-45               tt+cc (one or more additional c-jets)
    //   51                  1 add b-jet, 1 b-hadron in it
    //   52                  1 add b-jet, ≥2 b-hadrons in it (collinear g→bb)
    //   53,54,55            ≥2 add b-jets (jet count NOT recoverable)
    //   any other / -1      not ttbar
    // ──────────────────────────────────────────────────────────────────
    TtCat computeTtCategoryFromGenTtbarId() const {
        const int gtid = _ev->genTtbarId;
        if (gtid < 0) return TtCat::kNoTTJets;
        const int catId = gtid % 100;
        if (catId == 0)  return TtCat::kLightFlavour;
        if (catId >= 41 && catId <= 45) return TtCat::kAddCjet;
        if (catId == 51) return TtCat::kAdd1Bjet1Had;
        if (catId == 52) return TtCat::kAdd1Bjet2Had;
        if (catId >= 53 && catId <= 55) return TtCat::kAdd2Bjet;
        return TtCat::kNoTTJets;
    }

    // ──────────────────────────────────────────────────────────────────
    // Read the ntuple's two ttCat branch sets.
    //
    // These are the values written by the ntuplizer's TtbarCategorizer
    // module. They are the "single source of truth" for downstream
    // stitching and hist split — the analyzer never overrides them.
    //
    // For each set, exactly one of the five Bool branches is True on
    // ttbar events; on non-ttbar events all five are False and the
    // event maps to kNoTTJets here.
    // ──────────────────────────────────────────────────────────────────
    TtCat readNtuplePrimaryCategory() const {
        if (_ev->ttCat_LightFlavour)  return TtCat::kLightFlavour;
        if (_ev->ttCat_AddCjet)       return TtCat::kAddCjet;
        if (_ev->ttCat_Add1Bjet_1Had) return TtCat::kAdd1Bjet1Had;
        if (_ev->ttCat_Add1Bjet_2Had) return TtCat::kAdd1Bjet2Had;
        if (_ev->ttCat_Add2Bjet)      return TtCat::kAdd2Bjet;
        // All five False: depends on source code.
        // 0 = GENTTBARID (impossible if ttbar — would have set a Bool)
        // 2 = NO_TTBAR
        // 3 = NO_GENTTBARID (data, or missing branch)
        if (_ev->ttCatSource == 2) return TtCat::kNoTTJets;
        return TtCat::kUnknown;
    }

    TtCat readNtupleXvalCategory() const {
        if (_ev->ttCatXval_LightFlavour)  return TtCat::kLightFlavour;
        if (_ev->ttCatXval_AddCjet)       return TtCat::kAddCjet;
        if (_ev->ttCatXval_Add1Bjet_1Had) return TtCat::kAdd1Bjet1Had;
        if (_ev->ttCatXval_Add1Bjet_2Had) return TtCat::kAdd1Bjet2Had;
        if (_ev->ttCatXval_Add2Bjet)      return TtCat::kAdd2Bjet;
        if (_ev->ttCatXvalSource == 2) return TtCat::kNoTTJets;
        return TtCat::kUnknown;
    }

    // The "official" category the analyzer uses for downstream decisions.
    // By project convention this is always the ntuple primary set —
    // never the analyzer's own GenPart computation, which exists only
    // as a sanity check.
    TtCat officialTtCategory() const {
        return readNtuplePrimaryCategory();
    }

    // ──────────────────────────────────────────────────────────────────
    // Diagnostic helpers (used by the validation block & debug print).
    // These never feed downstream physics — they only provide context
    // for the per-event debug dump and the validation histograms.
    // ──────────────────────────────────────────────────────────────────
    int analyzerAdditionalBJetCount() const {
        if (genPartCount() == 0 || genJetCount() == 0) return -1;
        return countAdditionalBJets();
    }

    int analyzerAdditionalBHadronCount() const {
        if (genPartCount() == 0 || genJetCount() == 0) return -1;
        return countAdditionalBJetsDetailed().nHadrons;
    }

    int analyzerAdditionalCJetCount() const {
        if (genPartCount() == 0 || genJetCount() == 0) return -1;
        return countAdditionalCJets();
    }

    // ──────────────────────────────────────────────────────────────────
    // tt+jets categorization validation histograms
    //
    // Four estimators of the same event-level label are compared
    // pair-wise. See enum docstring above for the labels' meaning.
    //
    //   ANA_GENPART  analyzer's GenPart algorithm
    //   ANA_GENID    analyzer's decode of genTtbarId
    //   NTU_PRIMARY  ntuple ttCat_*       (= ntuplizer's genTtbarId path)
    //   NTU_XVAL     ntuple ttCatXval_*   (= ntuplizer's GenPart path)
    //
    // The six 2D histograms below cover all C(4,2)=6 pairings; the
    // diagonals reveal where each pair agrees and the off-diagonal
    // entries reveal exactly which categories disagree. The four
    // 1D histograms hold per-category event counts for each estimator.
    // ──────────────────────────────────────────────────────────────────

    // 1D event counts per estimator
    TH1F* _hTtCat_Counts_AnaGenPart  = nullptr;
    TH1F* _hTtCat_Counts_AnaGenId    = nullptr;
    TH1F* _hTtCat_Counts_NtuPrimary  = nullptr;
    TH1F* _hTtCat_Counts_NtuXval     = nullptr;

    // 2D pair-wise comparisons (6 pairs)
    TH2F* _hTtCat_AnaGenPart_vs_AnaGenId    = nullptr;
    TH2F* _hTtCat_AnaGenPart_vs_NtuPrimary  = nullptr;
    TH2F* _hTtCat_AnaGenPart_vs_NtuXval     = nullptr;  // expected: diagonal
    TH2F* _hTtCat_AnaGenId_vs_NtuPrimary    = nullptr;  // expected: diagonal
    TH2F* _hTtCat_AnaGenId_vs_NtuXval       = nullptr;
    TH2F* _hTtCat_NtuPrimary_vs_NtuXval     = nullptr;

    // genTtbarId mod 100 distribution split by analyzer category
    TH2F* _hTtCat_GenTtbarIdMod100         = nullptr;

    // L1 prefiring 보정
    const correction::Correction *prefireJetCorr = nullptr;
    const correction::Correction *prefirePhotonCorr = nullptr;
    // JER 보정
    const correction::Correction *jerResCorr = nullptr;
    const correction::Correction *jerSFCorr = nullptr;
  
    // Golden JSON (data 럼이섹션 머스크)
    static bool goldenLoaded;
    static std::map<int,std::vector<std::pair<int,int>>> goldenLumiList;

    static std::string trimWhitespace(std::string value) {
        value.erase(value.begin(), std::find_if(value.begin(), value.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        value.erase(std::find_if(value.rbegin(), value.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), value.end());
        return value;
    }

    static std::string extractEraFromSampleName(const std::string& sampleName) {
        std::size_t pos = sampleName.rfind('_');
        if (pos == std::string::npos || pos + 1 >= sampleName.size()) {
            return "";
        }
        std::string suffix = sampleName.substr(pos + 1);
        if (suffix.size() != 1 || !std::isalpha(static_cast<unsigned char>(suffix[0]))) {
            return "";
        }
        return suffix;
    }


    void diMotherReco(const TLorentzVector & dPar1p4,const TLorentzVector & dPar2p4,const TLorentzVector & dPar3p4,const TLorentzVector & dPar4p4, const float mother1mass, const float  mother2mass, float & _minChi2,float & _bbMassMin1, float & _bbMassMin2);

    float closestMassPair(const std::vector<objectJet*>* jets, float targetMass) const {
        if (!jets || jets->size() < 2) {
            return -1.0f;
        }
        float bestMass = -1.0f;
        float bestDiff = std::numeric_limits<float>::max();
        for (size_t i = 0; i < jets->size(); ++i) {
            for (size_t j = i + 1; j < jets->size(); ++j) {
                TLorentzVector pair = *(*jets)[i]->getp4() + *(*jets)[j]->getp4();
                float mass = pair.M();
                float diff = std::fabs(mass - targetMass);
                if (diff < bestDiff) {
                    bestDiff = diff;
                    bestMass = mass;
                }
            }
        }
        return bestMass;
    }

    void fillCutStepHist(CutStep step, const event* thisEvent, float hadWMass) {
        const size_t idx = static_cast<size_t>(step);
        if (idx >= _cutStepJetPt.size()) {
            return;
        }
        const auto* jets  = thisEvent->getSelJets();

        // Three weight chains — see PATCH H4 for definitions.
        // The legacy `_evtWeight * getbTagSys()` factor is preserved for
        // _cutStepBTag (shape-systematic histograms used elsewhere). The new
        // per-chain hists use the explicit chain weights.
        const float wRaw    = static_cast<float>(_evtWeight_chain_raw);
        const float wBtagSF = static_cast<float>(_evtWeight_chain_btagSF);
        const float wFull   = static_cast<float>(_evtWeight_chain_full);

        // Legacy (kept as-is so the rest of the code that depends on it
        // does not break). This is what the existing _cutStepBTag /
        // _cutStepJet* arrays receive.
        const float weightLegacy = _evtWeight * thisEvent->getbTagSys();

        if (!jets->empty()) {
            const size_t nFill = std::min(jets->size(), kNJetsForCutStep);
            for (size_t j = 0; j < nFill; ++j) {
                _cutStepJetPt .at(idx).at(j)->Fill(jets->at(j)->getp4()->Pt(),  weightLegacy);
                _cutStepJetEta.at(idx).at(j)->Fill(jets->at(j)->getp4()->Eta(), weightLegacy);
                _cutStepJetPhi.at(idx).at(j)->Fill(jets->at(j)->getp4()->Phi(), weightLegacy);
                _cutStepBTag  .at(idx).at(j)->Fill(jets->at(j)->bTagCSV,        weightLegacy);
            }
        }

        const double ht = thisEvent->getSumSelJetScalarpT();
        _cutStepHT       .at(idx)->Fill(ht, wRaw);
        _cutStepHT_btagSF.at(idx)->Fill(ht, wBtagSF);
        _cutStepHT_full  .at(idx)->Fill(ht, wFull);

        if (hadWMass > 0.0f) {
            _cutStepHadWMass       .at(idx)->Fill(hadWMass, wRaw);
            _cutStepHadWMass_btagSF.at(idx)->Fill(hadWMass, wBtagSF);
            _cutStepHadWMass_full  .at(idx)->Fill(hadWMass, wFull);
        }

        if (_bbMassMin1Higgs > 0.0f) {
            _cutStepHiggsMass01       .at(idx)->Fill(_bbMassMin1Higgs, wRaw);
            _cutStepHiggsMass01_btagSF.at(idx)->Fill(_bbMassMin1Higgs, wBtagSF);
            _cutStepHiggsMass01_full  .at(idx)->Fill(_bbMassMin1Higgs, wFull);
        }
        if (_bbMassMin2Higgs > 0.0f) {
            _cutStepHiggsMass02       .at(idx)->Fill(_bbMassMin2Higgs, wRaw);
            _cutStepHiggsMass02_btagSF.at(idx)->Fill(_bbMassMin2Higgs, wBtagSF);
            _cutStepHiggsMass02_full  .at(idx)->Fill(_bbMassMin2Higgs, wFull);
        }

        // [NEW] Multiplicities at this step — one entry per event into
        // each of the 3 chains.
        const int nbj = thisEvent->getnSelbJet();
        const int nj  = thisEvent->getnSelJet();
        _cutStepNbJets_raw   .at(idx)->Fill(nbj, wRaw);
        _cutStepNbJets_btagSF.at(idx)->Fill(nbj, wBtagSF);
        _cutStepNbJets_full  .at(idx)->Fill(nbj, wFull);
        _cutStepNjets_raw    .at(idx)->Fill(nj,  wRaw);
        _cutStepNjets_btagSF .at(idx)->Fill(nj,  wBtagSF);
        _cutStepNjets_full   .at(idx)->Fill(nj,  wFull);
    }

    /*    std::vector<double> getJetCutFlow(event *thisevent){
	int jetCounter = 0;
	std::vector<double> jetCutFlow;
        for(int i = 0; i < _ev->size(); i++){
	    jetCounter += 1;
            jetCutFlow.push_back(jetCounter);
	}
        return jetCutFlow;
	}*/


    std::vector<double> getJetCSV(event *thisevent){
	std::vector<double> jetCSV;
        for(int i = 0; i < thisevent->getnSelJet(); i++){
            jetCSV.push_back(thisevent->getSelJets()->at(i)->bTagCSV);
	}
        return jetCSV;
    }

    std::vector<double> getbJetCSV(event *thisevent){
	std::vector<double> bjetCSV;
        for(int i = 0; i < thisevent->getnSelbJet(); i++){
            bjetCSV.push_back(thisevent->getSelbJets()->at(i)->bTagCSV);
	}
        return bjetCSV;
    }

    std::vector<double> getlightJetCSV(event *thisevent){
	std::vector<double> lightjetCSV;
        for(int i = 0; i < thisevent->getnLightJet(); i++){
            lightjetCSV.push_back(thisevent->getSelLightJets()->at(i)->bTagCSV);
	}
        return lightjetCSV;
    }

    std::vector<TLorentzVector> getJetP4(event *thisevent){
	std::vector<TLorentzVector> jetP4;
	jetP4.reserve(thisevent->getnSelJet());
        for(const auto thisJet: *thisevent->getSelJets()){
            jetP4.push_back(*thisJet->getp4());
	}
        return jetP4;
    }

    std::vector<TLorentzVector> getLepP4(event *thisevent){
	std::vector<TLorentzVector> lepP4;
	lepP4.reserve(thisevent->getnSelLepton());
        for(const auto thisLep: *thisevent->getSelLeptons()){
            lepP4.push_back(*thisLep->getp4());
	}
        return lepP4;
    }
    

    /*    float getBTagValue(event *thisevent, int jetNo){
	float bTagValue;
	if(thisevent->getnSelJet() <= jetNo-1){
	    return -1;
	}

	if(bool(thisevent->getSelJets()->at(jetNo-1)->bTag & (1<<2)) == true){
	    bTagValue = 0.99;
	} else if(bool(thisevent->getSelJets()->at(jetNo-1)->bTag & (1<<1)) == true){
	    bTagValue = 0.66;
	} else if(bool(thisevent->getSelJets()->at(jetNo-1)->bTag & (1<<0)) == true){
	    bTagValue = 0.33;
	} else {
	    bTagValue = 0;
	}
	return bTagValue;
	}*/


    float getSysJES(TH1* hSys, float pT){
	return hSys->GetBinContent(hSys->FindBin(pT));
    } 

    float getSysJER(float sigma){
	return _rand.Gaus(sigma/2., sigma);
    } 

    void initSys(){
	int npTbin = 9;
	float sysbTagM[] = {0.01, 0.01, 0.01, 0.01, 0.01, 0.016, 0.018, 0.023, 0.046 };
	float pTBinEdges[] = { 30, 50, 70, 100, 140, 200, 300, 600, 1000, 3000 };
	_hbJetEff = new TH1D("bjeteff","bjet efficiency", npTbin, pTBinEdges);
	_hJetEff = new TH1D("jeteff","jet efficiency", npTbin, pTBinEdges);
	_hSysbTagM = new TH1D("bTagMSys","btag medium systematics", npTbin, pTBinEdges);
	int nbinsx = _hSysbTagM->GetXaxis()->GetNbins();
	for(int bind = 1; bind < nbinsx+1; bind++){
	    _hSysbTagM->SetBinContent(bind, sysbTagM[bind-1]);
	}
    }

    void getbJetEffMap(){
	_hbJetEff->Divide(_hJetEff);
    }

    std::vector<TDirectory *> _histoDirs; 
    std::vector<TDirectory *> _treeDirs; 

    void initHistograms(sysName sysType = noSys, bool up = false){

        hjetsPTs.resize(nHistsJets); hjetsEtas.resize(nHistsJets); hbjetsPTs.resize(nHistsbJets); hbjetsEtas.resize(nHistsbJets); hLightJetsPTs.resize(nHistsLightJets), hLightJetsEtas.resize(nHistsLightJets), hjetsBTagDisc.resize(nHistsJets), hbjetsBTagDisc.resize(nHistsbJets), hLightJetsBTagDisc.resize(nHistsLightJets);


	TString trail = "";
	if(sysType == kbTag){
	    if(up) trail += "btag_up";
	    else trail += "btag_down";
	} else if(sysType == kJES){
	    if(up) trail += "JES_up";
	    else trail += "JES_down";
	}else if(sysType == kJER){
	    if(up) trail += "JER_up";
	    else trail += "JER_down";
	}

	_of->file->cd();
	std::vector<TDirectory *> tmpDirs; 	
	TDirectory *jet = _of->file->mkdir("jet"+trail);
	tmpDirs.push_back(jet); 
	jet->cd();

	hmet = new TH1F("met"+trail, "MET"+trail, 50, 0, 500);
	hmetPhi = new TH1F("metPhi"+trail, "MET #phi"+trail, 50, -5, 5);
	hmetEta = new TH1F("metEta"+trail, "MET #eta"+trail, 50, -5, 5);


	const int nBins = 50;
	const std::pair<float, float> etaRange = {-3.2, 3.2};
	const std::array<float , 8> MaxJetPtRanges = { 3000.0, 2000.0, 1200.0, 900.0, 600.0, 500.0, 400.0, 300.0 };
	for(int i=0; i < nHistsJets; i++){

            float MaxJetPtRange = MaxJetPtRanges[std::min(i, 7)]; // Use 7th array element for i >= 7
            // What is above comment meaning?
	    // if i is bigger or equal to 7, MaxPtRange will be 300.0
	
            h_JEC_DiffRatio.push_back(new TH1D(
                TString::Format("h_JEC_DiffRatio_%d", i), "JEC Validation: (ReCalc - NanoAOD)/NanoAOD; p_{T} [GeV]; entries", 
                100, -0.05, 0.05
	    ));
            h_JEC_Mass_DiffRatio.push_back(new TH1D(
                TString::Format("h_JEC_Mass_DiffRatio_%d", i),
                "JEC Mass Validation: (ReCalc - NanoAOD)/NanoAOD; Mass [GeV]; entries",
                100, -0.05, 0.05
	    ));
     
            hjetsPTs.at(i)  = new TH1F(TString::Format("jetPT%d",(i+1))+trail, TString::Format("jet%d p_{T} [GeV]",i+1)+trail, nBins, 0.0, MaxJetPtRange);
            hjetsEtas.at(i) = new TH1F(TString::Format("jetEta%d",(i+1))+trail, TString::Format("jet%d #eta",i+1)+trail, nBins, etaRange.first, etaRange.second);

	    hjetsBTagDisc.at(i)  = new TH1F(TString::Format("jetBTagDisc%d",(i+1))+trail, TString::Format("jet%d btagDisc" ,i+1)+trail, nBins, 0, 1);
	}

	const std::array<float , 8> MaxBJetPtRanges = { 2500.0, 2000.0, 1200.0, 700.0, 400.0, 250.0, 200.0, 100.0 };
	for(int i=0; i < nHistsbJets; i++){

            float MaxBJetPtRange = MaxBJetPtRanges[std::min(i, 7)];

            hbjetsPTs.at(i) = new TH1F(TString::Format("bjetPT%d",(i+1))+trail, TString::Format("bjet%d p_{T} [GeV]",i+1)+trail, nBins, 0.0, MaxBJetPtRange);
            hbjetsEtas.at(i) = new TH1F(TString::Format("bjetEta%d",(i+1))+trail, TString::Format("bjet%d #eta",i+1)+trail, nBins, etaRange.first, etaRange.second);
	    hbjetsBTagDisc.at(i)  = new TH1F(TString::Format("bjetBTagDisc%d",(i+1))+trail, TString::Format("bjet%d btagDisc" ,i+1)+trail, nBins, 0, 1);
	}

        const std::array<float , 6> MaxLightJetPtRanges = { 2500.0, 1500.0, 800.0, 600.0, 400.0, 250.0 };
	for(int i=0; i < nHistsLightJets; i++){

            float MaxLightJetPtRange = MaxLightJetPtRanges[std::min(i, 5)];

            hLightJetsPTs.at(i) = new TH1F(TString::Format("lightJetPT%d",(i+1))+trail, TString::Format("lightJet%d p_{T} [GeV]",i+1)+trail, nBins, 0.0, MaxLightJetPtRange);
            hLightJetsEtas.at(i) = new TH1F(TString::Format("lightJetEta%d",(i+1))+trail, TString::Format("lightJet%d #eta",i+1)+trail, nBins, etaRange.first, etaRange.second);
	    hLightJetsBTagDisc.at(i)  = new TH1F(TString::Format("lightJetBTagDisc%d",(i+1))+trail, TString::Format("lightJet%d btagDisc" ,i+1)+trail, nBins, 0, 1);
	}
   
	hAvgDeltaRjj = new TH1F("deltaRavgjj"+trail, "#DeltaR_{jj}^{avg}"+trail, 50, 0, 5);
	hAvgDeltaRbb = new TH1F("deltaRavgbb"+trail, "#DeltaR_{bb}^{avg}"+trail, 50, 0, 5.5);
	hAvgDeltaRbj = new TH1F("deltaRavgbj"+trail, "#DeltaR_{bj}^{avg}"+trail, 50, 0, 5.5);
	hAvgDeltaEtajj = new TH1F("deltaEtaavgjj"+trail, "#Delta#eta_{jj}^{avg}"+trail, 50, 0, 3);
	hAvgDeltaEtabb = new TH1F("deltaEtaavgbb"+trail, "#Delta#eta_{bb}^{avg}"+trail, 50, 0, 3.5);
	hAvgDeltaEtabj = new TH1F("deltaEtaavgbj"+trail, "#Delta#eta_{bj}^{avg}"+trail, 50, 0, 3.5);
	hminDeltaRjj = new TH1F("deltaRminjj"+trail, "#DeltaR_{jj}^{min}"+trail, 50, 0, 2.5);
	hminDeltaRbb = new TH1F("deltaRminbb"+trail, "#DeltaR_{bb}^{min}"+trail, 50, 0, 4);
	hminDeltaRbj = new TH1F("deltaRminbj"+trail, "#DeltaR_{bj}^{min}"+trail, 50, 0, 4);
	hminDeltaRpTjj = new TH1F("pTdeltaRminjj"+trail, "#DeltaR_{jj, p_{T}}^{min}"+trail, 50, 0, 2500);
	hminDeltaRpTbb = new TH1F("pTdeltaRminbb"+trail, "#DeltaR_{bb, p_{T}}^{min}"+trail, 50, 0, 2500);
	hminDeltaRpTbj = new TH1F("pTdeltaRminbj"+trail, "#DeltaR_{bj, p_{T}}^{min}"+trail, 50, 0, 5000);
	hminDeltaRMassjj = new TH1F("massDeltaRminjj"+trail, "#DeltaR_{jj, mass}^{min}"+trail, 50, 0, 1000);
	hminDeltaRMassbb = new TH1F("massDeltaRminbb"+trail, "#DeltaR_{bb, mass}^{min}"+trail, 50, 0, 2000);
	hminDeltaRMassbj = new TH1F("massDeltaRminbj"+trail, "#DeltaR_{bj, mass}^{min}"+trail, 50, 0, 800);
	hmaxDeltaEtabb = new TH1F("deltaEtamaxbb"+trail, "#Delta#eta_{bb}^{max}"+trail, 50, 0, 5);
	hmaxDeltaEtajj = new TH1F("deltaEtamaxjj"+trail, "#Delta#eta_{jj}^{max}"+trail, 50, 0, 5);
	hmaxDeltaEtabj = new TH1F("deltaEtamaxbj"+trail, "#Delta#eta_{bj}^{max}"+trail, 50, 0, 5);
	hmaxPTmassjbb = new TH1F("maxPTmassjbb"+trail, "m_{jbb}^{max p_{T}}"+trail, 50, 0, 5000); 
	hmaxPTmassjjj = new TH1F("maxPTmassjjj"+trail, "m_{jjj}^{max p_{T}}"+trail, 50, 0, 6000); 
	hjetAverageMass = new TH1F("jetAvgMass"+trail, "m_{j}^{avg}"+trail, 50, 0, 100);
	hBjetAverageMass = new TH1F("jetBAvgMass"+trail, "m_{b}^{avg}"+trail, 50, 0, 150);
	hHadronicHiggsAverageMass = new TH1F("higgsHadAvgMass"+trail, "m_{H_{had}}^{avg}"+trail, 50, 0, 60);
	hLightJetAverageMass = new TH1F("jetLightAvgMass"+trail, "m_{light}^{avg}"+trail, 50, 0, 100);
	hBjetAverageMassSqr = new TH1F("jetBAvgMassSqr"+trail, "(m^{2})_{b}^{avg}"+trail, 50, 0, 80000);
	hHadronicHiggsSoftDropMass1 = new TH1F("higgsHadSoftDropMass1"+trail, "msoftdrop_{H_{had}}"+trail, 50, 0, 400);
	hHadronicHiggsSoftDropMass2 = new TH1F("higgsHadSoftDropMass2"+trail, "msoftdrop_{H_{had}}"+trail, 50, 0, 300);
	hjetHT = new TH1F("jetHT"+trail, "H_{T} [GeV]"+trail, 50, 0, 6000);
	hBjetHT = new TH1F("jetBHT"+trail, "H_{T}^{b} [GeV]"+trail, 50, 0, 4000); 
	hHadronicHiggsHT = new TH1F("jetHadronicHiggsHT"+trail, "H_{T}^{H_{had}} [GeV]"+trail, 50, 0, 4000); 
	hLightJetHT = new TH1F("jetLightHT"+trail, "H_{T}^{light} [GeV]"+trail, 50, 0, 3000); 
	hjetNumber = new TH1F("jetNumber"+trail, "N_{jet}"+trail, 17, 5, 22);
	hBjetNumber = new TH1F("jetBNumber"+trail, "N_{bjet}"+trail, 15, 3, 18); 
	hHadronicHiggsNumber = new TH1F("jetHadronicHiggsNumber"+trail, "N_{H_{had}}"+trail, 8, 2, 10); 
	hLightJetNumber = new TH1F("jetLightNumber"+trail, "N_{lightJet}"+trail, 15, 0, 15); 
	hInvMassHadW = new TH1F("invMass_hadW"+trail, "m_{W,had}"+trail, 50, 0, 2000);
	hInvMassZ1 = new TH1F("invMass_Z1"+trail, "m_{Z,1} [GeV]"+trail, 50, 0, 3000);
	hInvMassZ2 = new TH1F("invMass_Z2"+trail, "m_{Z,2} [GeV]"+trail, 50, 0, 1500);
        hInvMassZ1_zoomIn = new TH1F("invMass_zoomIn_Z1"+trail, "m_{Z,1} [GeV]"+trail, 100, 0, 500);
        hInvMassZ2_zoomIn = new TH1F("invMass_zoomIn_Z2"+trail, "m_{Z,2} [GeV]"+trail, 100, 0, 500);
	hInvMassH1 = new TH1F("invMass_Higgs1"+trail, "m_{H,1} [GeV]"+trail, 50, 0, 3000);
	hInvMassH2 = new TH1F("invMass_Higgs2"+trail, "m_{H,2} [GeV]"+trail, 50, 0, 1500);
	hInvMassH1_zoomIn = new TH1F("invMass_zoomIn_Higgs1"+trail, "m_{H,1} [GeV]"+trail, 100, 0, 500);
	hInvMassH2_zoomIn = new TH1F("invMass_zoomIn_Higgs2"+trail, "m_{H,2} [GeV]"+trail, 100, 0, 500);
	hInvMassH1mChi = new TH1F("invMass_Higgs1_mChi"+trail, "m_{H,1} min(#chi^{2})"+trail, 50, 0, 400000);
	hInvMassH2mChi = new TH1F("invMass_Higgs2_mChi"+trail, "m_{H,2} min(#chi^{2})"+trail, 50, 0, 400000);

	hPTH1 = new TH1F("pT_Higgs1"+trail, "p_{T(H,1)} [GeV]"+trail, 50, 0, 2500);
	hPTH2 = new TH1F("pT_Higgs2"+trail, "p_{T(H,2)} [GeV]"+trail, 50, 0, 2500);
	hInvMassHZ1 = new TH1F("invMass_HiggsZ1"+trail, "m^{Z}_{H,1} [GeV]"+trail, 50, 0, 3000);
	hInvMassHZ2 = new TH1F("invMass_HiggsZ2"+trail, "m^{Z}_{H,2} [GeV]"+trail, 50, 0, 1500);
        hInvMassHZ1_zoomIn = new TH1F("invMass_zoomIn_HiggsZ1"+trail, "m_{Z}_{H,1} [GeV]"+trail, 100, 0, 500);
        hInvMassHZ2_zoomIn = new TH1F("invMass_zoomIn_HiggsZ2"+trail, "m_{Z}_{H,2} [GeV]"+trail, 100, 0, 500);
	hChi2Higgs = new TH1F("chi2Higgs"+trail, "#chi^{2}_{HH}"+trail, 50, 0, 400000);
	hChi2Z = new TH1F("chi2Z"+trail, "#chi^{2}_{ZZ}"+trail, 50, 0, 400000);
	hChi2HiggsZ = new TH1F("chi2HiggsZ"+trail, "#chi^{2}_{HZ}"+trail, 50, 0, 400000);
	//hChi2HadW = new TH1F("chi2HadW"+trail, "#chi^{2}_{W,had}"+trail, 50, 0, 400);  
	hInvMassHSingleMatched = new TH1F("invMass_HiggsMatched"+trail, "m_{H,matched} [GeV]"+trail, 50, 0, 500); 
        hInvMassHSingleNotMatched  = new TH1F("invMass_HiggsNotMatched"+trail, "m_{H,unmatched} [GeV]"+trail, 50, 0, 500); 
        hChi2HiggsSingleNotMatched = new TH1F("chi2HiggsNotMatched"+trail, "#chi^{2}_{H,unmatched}"+trail, 50, 0, 10);
        hChi2HiggsSingleMatched =new TH1F("chi2HiggsMatched"+trail, "#chi^{2}_{H,matched}"+trail, 50, 0, 10);

	hInvMassHH1Matched = new TH1F("invMass_HH1Matched"+trail, "m_{H1,matched} [GeV]"+trail, 50, 0, 500); 
        hInvMassHH1NotMatched  = new TH1F("invMass_HH1NotMatched"+trail, "m_{H1,unmatched} [GeV]"+trail, 50, 0, 500); 
	hInvMassHH2Matched = new TH1F("invMass_HH2Matched"+trail, "m_{H2,matched} [GeV]"+trail, 50, 0, 500); 
        hInvMassHH2NotMatched  = new TH1F("invMass_HH2NotMatched"+trail, "m_{H2,unmatched} [GeV]"+trail, 50, 0, 500); 
        hChi2HHNotMatched = new TH1F("chi2HHNotMatched"+trail, "#chi^{2}_{H,unmatched}"+trail, 50, 0, 10);
        hChi2HHMatched =new TH1F("chi2HHMatched"+trail, "#chi^{2}_{H,matched}"+trail, 50, 0, 10);


	hAplanarity = new TH1F("aplanarity"+trail, "A"+trail, 50, 0, 0.5);  
	hSphericity = new TH1F("sphericity"+trail, "S"+trail, 50, 0, 1);  
	hTransSphericity = new TH1F("transSphericity"+trail, "S_{#perp}"+trail, 50, 0, 1);  
	hCvalue = new TH1F("C"+trail, "C value"+trail, 50, 0, 1);
	hDvalue = new TH1F("D"+trail, "D value"+trail, 50, 0, 1); 
	hCentralityjb = new TH1F("centralityjb"+trail, "centrality_{jb}"+trail, 50, 0, 1); 
	hCentralityjl = new TH1F("centralityjl"+trail, "centrality_{jl}"+trail, 50, 0, 1); 
	
	hH0 = new TH1F("H0"+trail, "H_{0}"+trail, 50, 0.2, 0.45); 
	hH1 = new TH1F("H1"+trail, "H_{1}"+trail, 50, -0.2, 0.45); 
	hH2 = new TH1F("H2"+trail, "H_{2}"+trail, 50, -0.2, 0.3); 
	hH3 = new TH1F("H3"+trail, "H_{3}"+trail, 50, -0.2, 0.3); 
	hH4 = new TH1F("H4"+trail, "H_{4}"+trail, 50, -0.2, 0.3); 

	hR1 = new TH1F("R1"+trail, "R_{1}"+trail, 50, 0, 1); 
	hR2 = new TH1F("R2"+trail, "R_{2}"+trail, 50, 0, 1); 
	hR3 = new TH1F("R3"+trail, "R_{3}"+trail, 50, 0, 1); 
	hR4 = new TH1F("R4"+trail, "R_{4}"+trail, 50, 0, 1); 

	hBjetH0 = new TH1F("H0_bjet"+trail, "H_{0,bjet}"+trail, 50, -0.2, 0.45); 
	hBjetH1 = new TH1F("H1_bjet"+trail, "H_{1,bjet}"+trail, 50, -0.2, 0.45); 
	hBjetH2 = new TH1F("H2_bjet"+trail, "H_{2,bjet}"+trail, 50, -0.2, 0.3); 
	hBjetH3 = new TH1F("H3_bjet"+trail, "H_{3,bjet}"+trail, 50, -0.2, 0.3); 
	hBjetH4 = new TH1F("H4_bjet"+trail, "H_{4,bjet}"+trail, 50, -0.2, 0.3); 

	hBjetR1 = new TH1F("R1_bjet"+trail, "R_{1,bjet}"+trail, 50, 0, 1); 
	hBjetR2 = new TH1F("R2_bjet"+trail, "R_{2,bjet}"+trail, 50, 0, 1); 
	hBjetR3 = new TH1F("R3_bjet"+trail, "R_{3,bjet}"+trail, 50, 0, 1); 
	hBjetR4 = new TH1F("R4_bjet"+trail, "R_{4,bjet}"+trail, 50, 0, 1); 

	hBjetAplanarity = new TH1F("aplanarity_bjet"+trail, "A_{bjet}"+trail, 50, 0, 0.5);  
	hBjetSphericity = new TH1F("sphericity_bjet"+trail, "S_{bjet}"+trail, 50, 0, 1);  
	hBjetTransSphericity = new TH1F("transSphericity_bjet"+trail, "S_{#perp, bjet}"+trail, 50, 0, 1);  
	hBjetCvalue = new TH1F("C_bjet"+trail, "C value_{bjet}"+trail, 50, 0, 1);
	hBjetDvalue = new TH1F("D_bjet"+trail, "D value_{bjet}"+trail, 50, 0, 1); 
  
	_of->file->cd();
	TDirectory *lepton = _of->file->mkdir("Lepton"+trail);
	tmpDirs.push_back(lepton);
	lepton->cd();

	hLepCharge1 = new TH1F("lepCharge1"+trail, "lepCh1"+trail, 4, -2, 2);
	hLepCharge2 = new TH1F("lepCharge2"+trail, "lepCh2"+trail, 4, -2, 2);
	    
	hleptonNumber = new TH1F("lepNumber"+trail, "N_{lep}"+trail, 4, 0, 4);
	hleptonHT = new TH1F("leptonHT"+trail, "H_{T}^{lep} [GeV]"+trail, 50, 0, 2000);
	hST = new TH1F("ST"+trail, "S_{T} [GeV]"+trail, 50, 0, 2000);
	hDiMuonMass = new TH1F("diMuonMass"+trail, "m_{#mu#mu} [GeV]"+trail, 50, 0, 200);
	hDiElectronMass = new TH1F("diEleMass"+trail, "m_{ee} [GeV]"+trail, 50, 0, 200);
	hDiMuonPT = new TH1F("diMuonPT"+trail, "p_{T, #mu#mu} [GeV]"+trail, 50, 0, 400);
	hDiElectronPT = new TH1F("diElePT"+trail, "p_{T, ee} [GeV]"+trail, 50, 0, 400);
	hDiMuonEta = new TH1F("diMuonEta"+trail, "#eta_{#mu#mu}"+trail, 50, -3, 3);
	hDiElectronEta = new TH1F("diEleEta"+trail, "#eta_{ee}"+trail, 50, -3, 3);

	hLeptonPT1 = new TH1F("leptonPT1"+trail, "lepton p_{T,1}"+trail, 50, 0, 400);
	hMuonPT1 = new TH1F("muonPT1"+trail, "muon p_{T,1}"+trail, 50, 0, 400);
	hElePT1 = new TH1F("elePT1"+trail, "ele p_{T,1}"+trail, 50, 0, 400);

	hLeptonPhi1 = new TH1F("leptonPhi1"+trail, "lepton #phi_{1}"+trail, 50, -4, 4);
	hMuonPhi1 = new TH1F("muonPhi1"+trail, "muon #phi_{1}"+trail, 50, -4, 4);
	hElePhi1 = new TH1F("elePhi1"+trail, "ele #phi_{1}"+trail, 50, -4, 4);

	hLeptonEta1 = new TH1F("leptonEta1"+trail, "lepton #eta_{1}"+trail, 50, -3, 3);
	hMuonEta1 = new TH1F("muonEta1"+trail, "muon #eta_{1}"+trail, 50, -3, 3);
	hEleEta1 = new TH1F("eleEta1"+trail, "ele #eta_{1}"+trail, 50, -3, 3); 

	hLeptonPT2 = new TH1F("leptonPT2"+trail, "lepton p_{T,2}"+trail, 50, 0, 250);
	hMuonPT2 = new TH1F("muonPT2"+trail, "muon p_{T,2}"+trail, 50, 0, 250);
	hElePT2 = new TH1F("elePT2"+trail, "ele p_{T,2}"+trail, 50, 0, 250);
	hLeptonPhi2 = new TH1F("leptonPhi2"+trail, "lepton #phi_{2}"+trail, 50, -4, 4);
	hMuonPhi2 = new TH1F("muonPhi2"+trail, "muon #phi_{2}"+trail, 50, -4, 4);
	hElePhi2 = new TH1F("elePhi2"+trail, "ele #phi_{2}"+trail, 50, -4, 4);
	hLeptonEta2 = new TH1F("leptonEta2"+trail, "lepton #eta_{2}"+trail, 50, -3, 3);
	hMuonEta2 = new TH1F("muonEta2"+trail, "muon #eta_{2}"+trail, 50, -3, 3);
	hEleEta2 = new TH1F("eleEta2"+trail, "ele #eta_{2}"+trail, 50, -3, 3); 

        _of->file->cd();
        TDirectory *cutflowDir = _of->file->mkdir("CutflowKinematics"+trail);
        tmpDirs.push_back(cutflowDir);
        cutflowDir->cd();

        const size_t cutStepCount = static_cast<size_t>(CutStep::kTotal) + 1;
        // [추가] 여기서 카운터 벡터 초기화 (0.0으로 채움)
        // resize 대신 assign을 쓰면 크기 변경과 동시에 값 초기화가 보장됩니다.
        _cutFlowCount.assign(cutStepCount, 0.0);
        _cutFlowWeight.assign(cutStepCount, 0.0);
	// 히스토그램 벡터 리사이즈
        // [수정] array<TH1F*, 6>의 각 원소를 nullptr로 초기화
        {
            std::array<TH1F*, kNJetsForCutStep> nullArr{};  // value-init → nullptr
            _cutStepJetPt.assign(cutStepCount, nullArr);
            _cutStepJetEta.assign(cutStepCount, nullArr);
            _cutStepJetPhi.assign(cutStepCount, nullArr);
            _cutStepBTag.assign(cutStepCount, nullArr);
        }
        _cutStepHT.resize(cutStepCount);
        _cutStepHadWMass.resize(cutStepCount);
        _cutStepHiggsMass01.resize(cutStepCount);
        _cutStepHiggsMass02.resize(cutStepCount);
        _cutStepHT_btagSF.resize(cutStepCount);
        _cutStepHT_full.resize(cutStepCount);
        _cutStepHadWMass_btagSF.resize(cutStepCount);
        _cutStepHadWMass_full.resize(cutStepCount);
        _cutStepHiggsMass01_btagSF.resize(cutStepCount);
        _cutStepHiggsMass01_full.resize(cutStepCount);
        _cutStepHiggsMass02_btagSF.resize(cutStepCount);
        _cutStepHiggsMass02_full.resize(cutStepCount);
        _cutStepNbJets_raw.resize(cutStepCount);
        _cutStepNbJets_btagSF.resize(cutStepCount);
        _cutStepNbJets_full.resize(cutStepCount);
        _cutStepNjets_raw.resize(cutStepCount);
        _cutStepNjets_btagSF.resize(cutStepCount);
        _cutStepNjets_full.resize(cutStepCount);


        // hCutFlow 히스토그램 생성 (라벨링 포함)
        hCutFlow          = new TH1F("cutflow",          "N_{cutFlow}",        cutStepCount, 0, cutStepCount);
        hCutFlow_w        = new TH1F("cutflow_w",        "N_{w (raw)}",        cutStepCount, 0, cutStepCount);
        hCutFlow_w_btagSF = new TH1F("cutflow_w_btagSF", "N_{w (btag SF)}",    cutStepCount, 0, cutStepCount);
        hCutFlow_w_full   = new TH1F("cutflow_w_full",   "N_{w (full SF+RW)}", cutStepCount, 0, cutStepCount);
        hCutFlow         ->Sumw2();
        hCutFlow_w       ->Sumw2();
        hCutFlow_w_btagSF->Sumw2();
        hCutFlow_w_full  ->Sumw2();

        for (size_t i = 0; i < cutStepCount; ++i) {
            const TString titleSuffix = TString::Format(" (%s)", _cutStepLabels.at(i).c_str());

            // [수정] 상위 6개 jet에 대해 pT, eta, phi, bTag 히스토그램 생성
            for (size_t j = 0; j < kNJetsForCutStep; ++j) {
                const TString jetLabel = TString::Format("jet%zu", j + 1);  // jet1 ~ jet6
                const TString jetTitle = TString::Format("jet_{%zu}", j + 1);

                _cutStepJetPt.at(i).at(j) = new TH1F(
                    TString::Format("cutStep_%zu_%s_Pt", i, jetLabel.Data()),
                    jetTitle + " p_{T} [GeV]" + titleSuffix, 50, 0, 2000);

                _cutStepJetEta.at(i).at(j) = new TH1F(
                    TString::Format("cutStep_%zu_%s_Eta", i, jetLabel.Data()),
                    jetTitle + " #eta" + titleSuffix, 50, -3, 3);

                _cutStepJetPhi.at(i).at(j) = new TH1F(
                    TString::Format("cutStep_%zu_%s_Phi", i, jetLabel.Data()),
                    jetTitle + " #phi" + titleSuffix, 50, -3.2, 3.2);

                _cutStepBTag.at(i).at(j) = new TH1F(
                    TString::Format("cutStep_%zu_%s_BTag", i, jetLabel.Data()),
                    jetTitle + " b-tag disc." + titleSuffix, 50, 0, 1);
            }

            // HT, hadW, Higgs mass는 이벤트 단위 → 기존대로 1개
            _cutStepHT.at(i) = new TH1F(TString::Format("cutStep_%zu_ht", i),
                                        "H_{T} [GeV]"+titleSuffix, 50, 0, 4000);
            _cutStepHT_btagSF.at(i) = new TH1F(TString::Format("cutStep_%zu_ht_btagSF", i),
                                               "H_{T} [GeV] (btagSF)"+titleSuffix, 50, 0, 4000);
            _cutStepHT_full.at(i) = new TH1F(TString::Format("cutStep_%zu_ht_full", i),
                                             "H_{T} [GeV] (full)"+titleSuffix, 50, 0, 4000);

            _cutStepHadWMass.at(i) = new TH1F(TString::Format("cutStep_%zu_hadW", i),
                                              "m_{W,had} [GeV]"+titleSuffix, 50, 0, 300);
            _cutStepHadWMass_btagSF.at(i) = new TH1F(TString::Format("cutStep_%zu_hadW_btagSF", i),
                                                     "m_{W,had} [GeV] (btagSF)"+titleSuffix, 50, 0, 300);
            _cutStepHadWMass_full.at(i) = new TH1F(TString::Format("cutStep_%zu_hadW_full", i),
                                                   "m_{W,had} [GeV] (full)"+titleSuffix, 50, 0, 300);

            _cutStepHiggsMass01.at(i) = new TH1F(TString::Format("cutStep_%zu_higgs can01", i),
                                               "m_{bb} closest to Higgs 1st candidate [GeV]"+titleSuffix, 50, 0, 300);
            _cutStepHiggsMass01_btagSF.at(i) = new TH1F(TString::Format("cutStep_%zu_higgs can01_btagSF", i),
                                                        "m_{bb} closest to Higgs 1st candidate [GeV] (btagSF)"+titleSuffix, 50, 0, 300);
            _cutStepHiggsMass01_full.at(i) = new TH1F(TString::Format("cutStep_%zu_higgs can01_full", i),
                                                      "m_{bb} closest to Higgs 1st candidate [GeV] (full)"+titleSuffix, 50, 0, 300);

            _cutStepHiggsMass02.at(i) = new TH1F(TString::Format("cutStep_%zu_higgs can02", i),
                                               "m_{bb} closest to Higgs 2nd candidate [GeV]"+titleSuffix, 50, 0, 300);
            _cutStepHiggsMass02_btagSF.at(i) = new TH1F(TString::Format("cutStep_%zu_higgs can02_btagSF", i),
                                                        "m_{bb} closest to Higgs 2nd candidate [GeV] (btagSF)"+titleSuffix, 50, 0, 300);
            _cutStepHiggsMass02_full.at(i) = new TH1F(TString::Format("cutStep_%zu_higgs can02_full", i),
                                                      "m_{bb} closest to Higgs 2nd candidate [GeV] (full)"+titleSuffix, 50, 0, 300);

            _cutStepNbJets_raw.at(i) = new TH1F(TString::Format("cutStep_%zu_nbjets_raw", i),
                                                "nbjets (raw)"+titleSuffix, 15, -0.5, 14.5);
            _cutStepNbJets_btagSF.at(i) = new TH1F(TString::Format("cutStep_%zu_nbjets_btagSF", i),
                                                   "nbjets (btagSF)"+titleSuffix, 15, -0.5, 14.5);
            _cutStepNbJets_full.at(i) = new TH1F(TString::Format("cutStep_%zu_nbjets_full", i),
                                                 "nbjets (full)"+titleSuffix, 15, -0.5, 14.5);
            _cutStepNjets_raw.at(i) = new TH1F(TString::Format("cutStep_%zu_njets_raw", i),
                                               "njets (raw)"+titleSuffix, 25, -0.5, 24.5);
            _cutStepNjets_btagSF.at(i) = new TH1F(TString::Format("cutStep_%zu_njets_btagSF", i),
                                                  "njets (btagSF)"+titleSuffix, 25, -0.5, 24.5);
            _cutStepNjets_full.at(i) = new TH1F(TString::Format("cutStep_%zu_njets_full", i),
                                                "njets (full)"+titleSuffix, 25, -0.5, 24.5);

            // hCutFlow 축 라벨 설정         
	    if (i < _cutStepLabels.size()) {
                hCutFlow->GetXaxis()->SetBinLabel(i + 1, _cutStepLabels[i].c_str());
                hCutFlow_w->GetXaxis()->SetBinLabel(i + 1, _cutStepLabels[i].c_str());
                hCutFlow_w_btagSF->GetXaxis()->SetBinLabel(i + 1, _cutStepLabels[i].c_str());
                hCutFlow_w_full->GetXaxis()->SetBinLabel(i + 1, _cutStepLabels[i].c_str());
            }

        }

        // ═══════════════════════════════════════════════════════════════
        // tt+jets categorization validation histograms
        //
        // Four estimators of the same per-event label, six pair-wise
        // 2D histograms, four 1D count histograms, plus a genTtbarId
        // mod 100 distribution. See header docstring for layout.
        // ═══════════════════════════════════════════════════════════════
        _of->file->cd();
        TDirectory *ttCatDir = _of->file->mkdir("TtCatValidation"+trail);
        tmpDirs.push_back(ttCatDir);
        ttCatDir->cd();

        // Helper lambdas to keep the labelled-axis boilerplate compact.
        auto labelAxisX = [&](TH1* h){
            for (int i = 0; i < kNTtCat; ++i)
                h->GetXaxis()->SetBinLabel(i+1, ttCatName(i));
        };
        auto labelAxisY = [&](TH2* h){
            for (int i = 0; i < kNTtCat; ++i)
                h->GetYaxis()->SetBinLabel(i+1, ttCatName(i));
        };
        auto makeCounts = [&](const char* name, const char* title) {
            auto* h = new TH1F(name, title, kNTtCat, 0, kNTtCat);
            labelAxisX(h);
            return h;
        };
        auto makePair = [&](const char* name, const char* xTitle, const char* yTitle) {
            TString title = TString::Format(";%s;%s", xTitle, yTitle);
            auto* h = new TH2F(name, title, kNTtCat, 0, kNTtCat, kNTtCat, 0, kNTtCat);
            labelAxisX(h);
            labelAxisY(h);
            return h;
        };

        // ── 1D event counts per estimator ──
        _hTtCat_Counts_AnaGenPart = makeCounts(
            "ttCat_Counts_AnaGenPart",
            "Events per category (analyzer GenPart);Category;Events");
        _hTtCat_Counts_AnaGenId = makeCounts(
            "ttCat_Counts_AnaGenId",
            "Events per category (analyzer genTtbarId decode);Category;Events");
        _hTtCat_Counts_NtuPrimary = makeCounts(
            "ttCat_Counts_NtuPrimary",
            "Events per category (ntuple ttCat_*);Category;Events");
        _hTtCat_Counts_NtuXval = makeCounts(
            "ttCat_Counts_NtuXval",
            "Events per category (ntuple ttCatXval_*);Category;Events");

        // ── 2D pair-wise comparisons (6 pairs) ──
        // Naming: <X>_vs_<Y>  →  X = rows, Y = columns when viewed as
        // a confusion matrix. Diagonal = agreement, off-diagonal = disagreement.

        _hTtCat_AnaGenPart_vs_AnaGenId = makePair(
            "ttCat_AnaGenPart_vs_AnaGenId",
            "Analyzer GenPart", "Analyzer genTtbarId decode");

        _hTtCat_AnaGenPart_vs_NtuPrimary = makePair(
            "ttCat_AnaGenPart_vs_NtuPrimary",
            "Analyzer GenPart", "Ntuple ttCat_* (primary)");

        _hTtCat_AnaGenPart_vs_NtuXval = makePair(
            "ttCat_AnaGenPart_vs_NtuXval",
            "Analyzer GenPart", "Ntuple ttCatXval_*");

        _hTtCat_AnaGenId_vs_NtuPrimary = makePair(
            "ttCat_AnaGenId_vs_NtuPrimary",
            "Analyzer genTtbarId decode", "Ntuple ttCat_* (primary)");

        _hTtCat_AnaGenId_vs_NtuXval = makePair(
            "ttCat_AnaGenId_vs_NtuXval",
            "Analyzer genTtbarId decode", "Ntuple ttCatXval_*");

        _hTtCat_NtuPrimary_vs_NtuXval = makePair(
            "ttCat_NtuPrimary_vs_NtuXval",
            "Ntuple ttCat_* (primary)", "Ntuple ttCatXval_*");

        // ── genTtbarId mod 100 distribution split by analyzer category ──
        _hTtCat_GenTtbarIdMod100 = new TH2F(
            "ttCat_GenTtbarIdMod100",
            "genTtbarId mod 100 vs analyzer GenPart category;genTtbarId %% 100;Analyzer GenPart category",
            60, -0.5, 59.5, kNTtCat, 0, kNTtCat);
        labelAxisY(_hTtCat_GenTtbarIdMod100);

	_histoDirs = tmpDirs;
    }


    TTree * _inputTree;
    float bjetPT1, bjetPT2, bjetPT3, bjetPT4, bjetPT5, bjetPT6, bjetPT7, bjetPT8, bjetPT9, bjetPT10, bjetPT11, bjetPT12;
    float bbjetPT1, bbjetPT2, bbjetPT3, bbjetPT4, bbjetPT5, bbjetPT6, bbjetPT7, bbjetPT8;
    float bjetEta1, bjetEta2, bjetEta3, bjetEta4, bjetEta5, bjetEta6, bjetEta7, bjetEta8, bjetEta9, bjetEta10, bjetEta11, bjetEta12;
    float bbjetEta1, bbjetEta2, bbjetEta3, bbjetEta4, bbjetEta5, bbjetEta6, bbjetEta7, bbjetEta8;
    float bbjetPhi1, bbjetPhi2, bbjetPhi3, bbjetPhi4, bbjetPhi5, bbjetPhi6, bbjetPhi7, bbjetPhi8;
    float blightjetPT1, blightjetPT2, blightjetPT3, blightjetPT4, blightjetPT5, blightjetPT6;
    float blightjetEta1, blightjetEta2, blightjetEta3, blightjetEta4, blightjetEta5, blightjetEta6;
    float bjetBTagDisc1, bjetBTagDisc2, bjetBTagDisc3, bjetBTagDisc4, bjetBTagDisc5, bjetBTagDisc6, bjetBTagDisc7, bjetBTagDisc8, bjetBTagDisc9, bjetBTagDisc10, bjetBTagDisc11, bjetBTagDisc12; 
    
    float bbjetBTagDisc1,bbjetBTagDisc2, bbjetBTagDisc3, bbjetBTagDisc4, bbjetBTagDisc5, bbjetBTagDisc6, bbjetBTagDisc7, bbjetBTagDisc8;
    float blightjetBTagDisc1, blightjetBTagDisc2, blightjetBTagDisc3, blightjetBTagDisc4, blightjetBTagDisc5, blightjetBTagDisc6;
    float bmet, bmetPhi, bmetEta;
    float baverageDeltaRjj, baverageDeltaRbb, baverageDeltaEtajj, baverageDeltaEtabb, bminDeltaRjj, bminDeltaRbb, bmaxDeltaEtajj, bmaxDeltaEtabb;
    float bjetAverageMass, bbJetAverageMass, blightJetAverageMass, bbJetAverageMassSqr;
    float bjetHT, bbjetHT, blightjetHT;
    float binvMassHadW, binvMassZ1, binvMassZ2, binvMassH1, binvMassH2, bchi2Higgs, bchi2HiggsZ, bchi2HadW, bchi2Z, binvMassHiggsZ1, binvMassHiggsZ2;
    float bPTH1, bPTH2, bweight;
    float baplanarity, bsphericity, btransSphericity, bcValue, bdValue, bbaplanarity, bcentralityjb, bcentralityjl, bbsphericity, bbtransSphericity, bbcValue, bbdValue;
    float bleptonEta1, bmuonEta1, beleEta1, bleptonPT1, bmuonPT1, belePT1, bleptonEta2, bmuonEta2, beleEta2, bleptonPT2, bmuonPT2, belePT2;
    float bdiElectronMass, bdiMuonMass, bleptonHT, bST, bleptonCharge1, bleptonCharge2;
    float bH0, bH1, bH2, bH3, bH4, bbH0, bbH1, bbH2, bbH3, bbH4;
    float bR1, bR2, bR3, bR4, bbR1, bbR2, bbR3, bbR4;
    float bmaxPTmassjbb, bmaxPTmassjjj, bminDeltaRpTbb, bminDeltaRpTjj, bminDeltaRpTbj, bminDeltaRMassjj, bminDeltaRMassbj, bminDeltaRMassbb, baverageDeltaRbj,  baverageDeltaEtabj, bminDeltaRbj, bmaxDeltaEtabj;
    float bbjetHiggsMatched1, bbjetHiggsMatched2, bbjetHiggsMatched3, bbjetHiggsMatched4, bbjetHiggsMatched5, bbjetHiggsMatched6,  bbjetHiggsMatched7, bbjetHiggsMatched8;
    float bbjetHiggsMatcheddR1, bbjetHiggsMatcheddR2, bbjetHiggsMatcheddR3, bbjetHiggsMatcheddR4, bbjetHiggsMatcheddR5, bbjetHiggsMatcheddR6,  bbjetHiggsMatcheddR7,  bbjetHiggsMatcheddR8; 
    float bbjetMinChiHiggsIndex1, bbjetMinChiHiggsIndex2, bbjetMinChiHiggsIndex3, bbjetMinChiHiggsIndex4, bbjetMinChiHiggsIndex5, bbjetMinChiHiggsIndex6, bbjetMinChiHiggsIndex7, bbjetMinChiHiggsIndex8;
 

    // Variables for Trigger Path                                                        
    bool passTrigger_HLT_IsoMu27; // Reference Muon Trigger to Calculate efficiency & SFs
    bool passTrigger_HLT_PFHT1050;                                                      
    //bool passTrigger_HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59;                          
    //bool passTrigger_HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94;                   
    //bool passTrigger_HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5;     
    bool passTrigger_6J1T_B;
    bool passTrigger_6J1T_CDEF;
    bool passTrigger_6J2T_B;
    bool passTrigger_6J2T_CDEF;
    bool passTrigger_4J3T_B;
    bool passTrigger_4J3T_CDEF;

    int nMuons;                                                                          
    int nElecs;
    int nJets;                                                                          
    int nbJets;                                                                         
    float HT;                                                                           

    std::vector<float> jetPt;
    std::vector<float> jetEta;
    std::vector<float> bTagScore;
    std::vector<int> jetPUids;

    // Variables for B tag correction
    std::vector<int> hadFlavs;
    std::vector<int> partonFlavs;

    unsigned int eventNumber;
    unsigned int runNumber;
     
    float evtWeight;          // [3-tier] base×PU×L1×genW×stitch×trigSF (b-tag SF/RW 없음)
    float evtWeight_btagSF;   // [3-tier] evtWeight × btagShape SF
    float evtWeight_full;     // [3-tier] evtWeight_btagSF × btagNormReweight
    float SampleWeight;
    float PUWeight;
    float L1PrefiringWeight;
    float genWeight;
    float bTagWeight;
    bool  failGoldenJson;
    bool  passMETFilters;
    bool  passHadTrig;


    int bjetNumber, bbjetNumber, blightjetNumber; 
    void initTree(sysName sysType = noSys, bool up = false){
	_of->file->cd();
	std::vector<TDirectory*> tmpDirs;
	TString trail = "";
	if(sysType == kbTag){
	    if(up) trail += "_btag_up";
	    else trail += "_btag_down";
	} else if(sysType == kJES){
	    if(up) trail += "_JES_up";
	    else trail += "_JES_down";
	} else if(sysType == kJER){
	    if(up) trail += "_JER_up";
	    else trail += "_JER_down";
	}
	
	TDirectory *tree = _of->file->mkdir("Tree"+trail);
	tree->cd();
	tmpDirs.push_back(tree);
	
        _inputTree = new  TTree("Tree","tree for dnn inputs");


        // Branch for Trigger Path                                                        
        _inputTree->Branch("passTrigger_HLT_IsoMu27", &passTrigger_HLT_IsoMu27, "passTrigger_HLT_IsoMu27/O");
        _inputTree->Branch("passTrigger_HLT_PFHT1050", &passTrigger_HLT_PFHT1050, "passTrigger_HLT_PFHT1050/O");
        _inputTree->Branch("passTrigger_6J1T_B", &passTrigger_6J1T_B, "passTrigger_6J1T_B/O");
        _inputTree->Branch("passTrigger_6J1T_CDEF", &passTrigger_6J1T_CDEF, "passTrigger_6J1T_CDEF/O");
        _inputTree->Branch("passTrigger_6J2T_B", &passTrigger_6J2T_B, "passTrigger_6J2T_B/O");
        _inputTree->Branch("passTrigger_6J2T_CDEF", &passTrigger_6J2T_CDEF, "passTrigger_6J2T_CDEF/O");
        _inputTree->Branch("passTrigger_4J3T_B", &passTrigger_4J3T_B, "passTrigger_4J3T_B/O");
        _inputTree->Branch("passTrigger_4J3T_CDEF", &passTrigger_4J3T_CDEF, "passTrigger_4J3T_CDEF/O");

        _inputTree->Branch("nMuons", &nMuons, "nMuons/I");
        _inputTree->Branch("nElecs", &nElecs, "nElecs/I");
        _inputTree->Branch("nJets", &nJets, "nJets/I");
        _inputTree->Branch("nbJets", &nbJets, "nbJets/I");
        _inputTree->Branch("HT", &HT, "HT/F");
        _inputTree->Branch("jetPt", &jetPt);
        _inputTree->Branch("jetEta", &jetEta);
        _inputTree->Branch("bTagScore", &bTagScore);
        _inputTree->Branch("jetPUids", &jetPUids);

        // Branch for B tagging correction
        _inputTree->Branch("hadFlavs", &hadFlavs);
	_inputTree->Branch("partonFlavs", &partonFlavs);

	_inputTree->Branch("eventNumber", &eventNumber, "eventNumber/i");
        _inputTree->Branch("runNumber", &runNumber, "runNumber/i");

	_inputTree->Branch("evtWeight", &evtWeight, "evtWeight/F");
	// [3-tier] b-tag SF/reweight 영향 비교용. stack plotter가 셋 중 선택:
	//   evtWeight(trigSF만) / evtWeight_btagSF(+shape) / evtWeight_full(+normRW)
	_inputTree->Branch("evtWeight_btagSF", &evtWeight_btagSF, "evtWeight_btagSF/F");
	_inputTree->Branch("evtWeight_full",   &evtWeight_full,   "evtWeight_full/F");
	_inputTree->Branch("SampleWeight", &SampleWeight, "SampleWeight/F");
	_inputTree->Branch("PUWeight", &PUWeight, "PUWeight/F");
	_inputTree->Branch("L1PrefiringWeight", &L1PrefiringWeight, "L1PrefiringWeight/F");
	_inputTree->Branch("genWeight", &genWeight, "genWeight/F");
	// [tt+nb] NanoAOD genTtbarId and the tt+nb-expanded id, stored per event.
	// Downstream stitching / b-jet reweight bins on expandedTtbarId%100
	// (which now includes 61/62=tt+bbb, 71/72=tt+4b). Both are -1 on non-MC.
	_inputTree->Branch("genTtbarId",      &_genTtbarIdNano,  "genTtbarId/I");
	_inputTree->Branch("expandedTtbarId", &_expandedTtbarId, "expandedTtbarId/I");
	// [stitch] per-event stitch multiplier (1.0 if not applicable). Lets
	// derivative tools (b-tag reweight / trigger SF) rebuild the stitched
	// pre-SF base weight without duplicating StitchFactors logic.
	_inputTree->Branch("stitchWeight",    &_stitchWeight,    "stitchWeight/F");
	// [STEP5] event shape branch (jets / b-jets)
	_inputTree->Branch("aplanarity",          &_es_aplanarity,          "aplanarity/F");
	_inputTree->Branch("sphericity",          &_es_sphericity,          "sphericity/F");
	_inputTree->Branch("transSphericity",     &_es_transSphericity,     "transSphericity/F");
	_inputTree->Branch("eventC",              &_es_C,                   "eventC/F");
	_inputTree->Branch("eventD",              &_es_D,                   "eventD/F");
	_inputTree->Branch("bjetAplanarity",      &_es_bjetAplanarity,      "bjetAplanarity/F");
	_inputTree->Branch("bjetSphericity",      &_es_bjetSphericity,      "bjetSphericity/F");
	_inputTree->Branch("bjetTransSphericity", &_es_bjetTransSphericity, "bjetTransSphericity/F");
	_inputTree->Branch("bjetEventC",          &_es_bjetC,               "bjetEventC/F");
	_inputTree->Branch("bjetEventD",          &_es_bjetD,               "bjetEventD/F");
	// [STEP6] 추가 di-mother 가설 (HH는 기존 branch/hist 경로 유지)
	_inputTree->Branch("chi2ZH",   &_hr_chi2ZH,   "chi2ZH/F");
	_inputTree->Branch("mZcandZH", &_hr_mZcandZH, "mZcandZH/F");
	_inputTree->Branch("mHcandZH", &_hr_mHcandZH, "mHcandZH/F");
	_inputTree->Branch("chi2ZZ",   &_hr_chi2ZZ,   "chi2ZZ/F");
	_inputTree->Branch("mZ1ZZ",    &_hr_mZ1ZZ,    "mZ1ZZ/F");
	_inputTree->Branch("mZ2ZZ",    &_hr_mZ2ZZ,    "mZ2ZZ/F");
	_inputTree->Branch("bTagWeight", &bTagWeight_central_, "bTagWeight/F");
	_inputTree->Branch("failGoldenJson", &failGoldenJson, "failGoldenJson/O");
	_inputTree->Branch("passMETFilters", &passMETFilters, "passMETFilters/O");
	_inputTree->Branch("passHadTrig", &passHadTrig, "passHadTrig/O");

        // ──────────────────────────────────────────────────────────────────────────
        // [NEW] Trigger SF & B-tag Normalization Reweight branches
        // ──────────────────────────────────────────────────────────────────────────
        _inputTree->Branch("triggerSF",          &triggerSF_,          "triggerSF/F");
        _inputTree->Branch("triggerSF_up",       &triggerSF_up_,       "triggerSF_up/F");
        _inputTree->Branch("triggerSF_down",     &triggerSF_down_,     "triggerSF_down/F");
        _inputTree->Branch("btagNormReweight",   &btagNormReweight_,   "btagNormReweight/F");

	_treeDirs = tmpDirs;
    }
};	
#endif
