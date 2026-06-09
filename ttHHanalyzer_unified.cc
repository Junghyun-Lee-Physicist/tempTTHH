#include "tnm.h"
#include <cmath> 
#include <algorithm>
#include <vector>
#include <map>
#include "TVector3.h"
#include "ttHHanalyzer_unified.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>   // [tt+nb] std::getenv for EXPANDED_TTBARID_DIR override

#include "correction.h"

#include "Logger.h"
using namespace Logger;
 
// ───────────────────────────────────────────────────────────────────────────
// B-tag Event Weight 계산 (Shape Correction 방식)
// ───────────────────────────────────────────────────────────────────────────
void ttHHanalyzer_unified::computeBTagWeight(event* thisEvent) {
    
    // 초기화
    bTagWeight_central_ = 1.0;
    bTagWeight_up_hf_ = 1.0;
    bTagWeight_down_hf_ = 1.0;
    bTagWeight_up_lf_ = 1.0;
    bTagWeight_down_lf_ = 1.0;
    bTagWeight_up_cferr1_ = 1.0;
    bTagWeight_down_cferr1_ = 1.0;
    bTagWeight_up_cferr2_ = 1.0;
    bTagWeight_down_cferr2_ = 1.0;
    
    // Data는 SF 적용하지 않음
    if (_DataOrMC == "Data") return;
    
    // 선택된 모든 jet에 대해 SF 계산
    const auto* jets = thisEvent->getSelJets();
    
    for (const auto* jet : *jets) {
        double pt   = jet->getp4()->Pt();
        double eta  = jet->getp4()->Eta();
        double disc = jet->bTagCSV;  // btagDeepFlavB
        int flav    = jet->hadFlav;  // hadronFlavour
        
        // ═══════════════════════════════════════════════════════════════════
        // Central value
        // ═══════════════════════════════════════════════════════════════════
        double sf_central = corrMgr->getBTagSF_Shape(flav, eta, pt, disc, "central");
        bTagWeight_central_ *= sf_central;
        
        // ═══════════════════════════════════════════════════════════════════
        // Systematic variations
        // ═══════════════════════════════════════════════════════════════════
        
        // b/light jet systematics (c-jet에는 central 적용됨)
        if (flav == 5 || flav == 0) {
            // Heavy flavor (b)
            if (flav == 5) {
                bTagWeight_up_hf_   *= corrMgr->getBTagSF_Shape(flav, eta, pt, disc, "up_hf");
                bTagWeight_down_hf_ *= corrMgr->getBTagSF_Shape(flav, eta, pt, disc, "down_hf");
            } else {
                bTagWeight_up_hf_   *= sf_central;
                bTagWeight_down_hf_ *= sf_central;
            }
            
            // Light flavor
            if (flav == 0) {
                bTagWeight_up_lf_   *= corrMgr->getBTagSF_Shape(flav, eta, pt, disc, "up_lf");
                bTagWeight_down_lf_ *= corrMgr->getBTagSF_Shape(flav, eta, pt, disc, "down_lf");
            } else {
                bTagWeight_up_lf_   *= sf_central;
                bTagWeight_down_lf_ *= sf_central;
            }
            
            // c-jet variations에는 central
            bTagWeight_up_cferr1_   *= sf_central;
            bTagWeight_down_cferr1_ *= sf_central;
            bTagWeight_up_cferr2_   *= sf_central;
            bTagWeight_down_cferr2_ *= sf_central;
        }
        // c-jet systematics
        else if (flav == 4) {
            // c-jet은 cferr만 사용
            bTagWeight_up_cferr1_   *= corrMgr->getBTagSF_Shape(flav, eta, pt, disc, "up_cferr1");
            bTagWeight_down_cferr1_ *= corrMgr->getBTagSF_Shape(flav, eta, pt, disc, "down_cferr1");
            bTagWeight_up_cferr2_   *= corrMgr->getBTagSF_Shape(flav, eta, pt, disc, "up_cferr2");
            bTagWeight_down_cferr2_ *= corrMgr->getBTagSF_Shape(flav, eta, pt, disc, "down_cferr2");
            
            // hf/lf variations에는 central
            bTagWeight_up_hf_   *= sf_central;
            bTagWeight_down_hf_ *= sf_central;
            bTagWeight_up_lf_   *= sf_central;
            bTagWeight_down_lf_ *= sf_central;
        }
    }
    
    if (debugCorrections) {
        std::cout << "[BTagWeight] central=" << bTagWeight_central_
                  << " up_hf=" << bTagWeight_up_hf_
                  << " down_hf=" << bTagWeight_down_hf_
                  << " up_lf=" << bTagWeight_up_lf_
                  << " down_lf=" << bTagWeight_down_lf_
                  << std::endl;
    }
}

void ttHHanalyzer_unified::performAnalysis(){

    if (_analysisMode == AnalysisMode::kPrescan) {
        // ⚠ TEMPORARY mode — migrate to NtupleForge eventually.
        // Skips the full selection/object pipeline; pure gen-level counter.
        runPrescan();
    } else {
        loop(noSys, false);
    }

    _of->file->Close();

}

void ttHHanalyzer_unified::loop(sysName sysType, bool up){


    int nevents = _ev->size();
    // nevents = 100; // DEBUG: removed - run all events for full cross-validation

    //std::cout<<"weight = "<<_weight<<std::endl;  
    std::cout << "Base Weight = " << _baseWeight << std::endl; // [��]
    _SampleWeight = _baseWeight; // Tree ���� Base Weight� �� (�� ��� �� evtWeight ��)

    cout<<endl;
    print("This analyzer commented out [ \"WTF\" log ] in the header, Please check if you want!!!", "magenta", "warning");

    cout<<endl;
    print("--------------------------------------------------------------------------", "b");
    print("Before start, Let's check the analysis information", "b");
    print("Run Year    ----> [  " + _runYear + "  ]", "b");
    print("Data or MC  ----> [  " + _DataOrMC + "  ]", "b");
    print("Sample Name ----> [  " + _sampleName + "  ]", "b");
    
    string checklist = "[ tnm.cc ] & [ analyzer header ] & [ main ] & [ analyzer constructor ]";
    bool exitFlag = false;
    if(_runYear == "nothing"){
        print("RunYear is not defined, Please check the" + checklist, "r", "error");
        exitFlag = true;
    }
    if(_DataOrMC == "nothing"){
        print("Whether Data or MC is not defined, Please check the" + checklist, "r", "error");
        exitFlag = true;
    }
    if(_sampleName == "nothing"){
        print("SampleName is not defined, Please check the" + checklist, "r", "error");
        exitFlag = true;
    }
    print("--------------------------------------------------------------------------", "b");
    cout<<endl;
    if(exitFlag) std::exit(EXIT_FAILURE);

    // ── [stitch] setup report + Condor-catchable prerequisite check ──────────
    // Only when the stitch JSON was loaded (main / btagtrig). trigsf, validation
    // run stitch-free, so this block is skipped there.
    if (_stitch.loaded()) {
        _stitch.printConfigSummary();
        if (_stitch.inPlan() && !_expTtbarId.active()) {
            std::cerr << "\n[FATAL][stitch] sample '" << _sampleName << "' is in the stitch "
                      << "plan (role " << _stitch.role() << ") but the Expanded_genTtbarId "
                      << "lookup is INACTIVE (no ttnb_<sample>.root loaded).\n"
                      << "  tt+nb (61/62/71/72) would never be tagged -> wrong stitch.\n"
                      << "  Provide the lookup (DerivedCorr/expandedTtbarId or "
                      << "$EXPANDED_TTBARID_DIR). Aborting (exit 43).\n" << std::endl;
            std::exit(43);
        }
    }

    if(debugCorrections) std::cout<<"debug : Before begin the entry.."<<std::endl;
    std::string analysisInfo = _runYear + ", " + _DataOrMC + ", " + _sampleName;

    for(int entry=0; entry < nevents; entry++){
        event * currentEvent = new event;
        ////cout << "Processed events: " << entry << endl;
        _ev->read(entry);       // read an event into event buffer
        process(currentEvent, sysType, up);

        if (entry % 10000 == 0){
        //if (entry % 1 == 0){
            print("Processed events of " + analysisInfo + ": " + to_string(entry) ,"c");
            currentEvent->summarize();
        }

        delete currentEvent;
    }

    
    _expTtbarId.printSummary();   // [tt+nb] hit/miss + genTtbarId self-check summary

    // [stitch] per-category Sum(weight) before/after the multiplier, so the log
    // shows exactly what the stitch did to this sample's composition.
    // (main / btagtrig only; skipped when the JSON was not loaded.)
    if (_stitch.loaded()) _stitch.printRunSummary();

    // [stitch] diagnostic — how this sample's events mapped expandedTtbarId%100
    // to the b-tag-reweight processKey. If 61/62/71/72 share the 53/54/55 key,
    // Config_TtCatGroup.hh::MakeProcessKey does NOT yet split tt+nb out of tt+2b
    // for the reweight (see ttbarCategorization.md s11): extend it there if the
    // AN's per-tt+nb b-tag reweight bin is required.
    if (!_btagKeyByExpSub.empty()) {
        std::cout << "[stitch] b-tag reweight processKey by expandedTtbarId%100 "
                  << "(sample=" << _sampleName << "):\n";
        std::string key2b;
        for (const auto& kv : _btagKeyByExpSub) {
            std::cout << "    sub=" << std::setw(3) << kv.first << " -> " << kv.second << "\n";
            if (kv.first >= 53 && kv.first <= 55 && key2b.empty()) key2b = kv.second;
        }
        bool nbSplit = true;
        for (const auto& kv : _btagKeyByExpSub)
            if (kv.first >= 61 && !key2b.empty() && kv.second == key2b) nbSplit = false;
        std::cout << "    -> tt+nb split from tt+2b in the b-tag key: "
                  << (nbSplit ? "YES" : "NO (extend MakeProcessKey if AN requires it)")
                  << "\n" << std::endl;
    }

    if(debugCorrections) std::cout<<"debug : Before [ writeHistos() ]"<<std::endl;
    writeHistos();
    if(debugCorrections) std::cout<<"debug : After writeHistos() & Before writeTree()"<<std::endl;
    writeTree();
    if(debugCorrections) std::cout<<"debug : After writeTree() & Before hcutFlow()"<<std::endl;

    // [��] �� map �� �� �� -> �� �� ���� ��
    std::cout << "=== CutFlow Summary ===" << std::endl;
    for (size_t i = 0; i < _cutStepLabels.size(); ++i) {
        std::cout << _cutStepLabels[i] 
                  << " : " << _cutFlowCount[i] 
                  << " (weighted: " << _cutFlowWeight[i] << ")" 
                  << std::endl;
        
        // hCutFlow ������ �� ���� �� Set�� ��� ��� �� � (����)
        // hCutFlow->SetBinContent(i+1, _cutFlowCount[i]);
    }

    hCutFlow         ->Write();
    if(debugCorrections) std::cout<<"debug : After hCutFlow() & Before SF-aware cutflows"<<std::endl;
    hCutFlow_w       ->Write();
    hCutFlow_w_btagSF->Write();
    hCutFlow_w_full  ->Write();
    if(debugCorrections) std::cout<<"debug : After SF-aware cutflows"<<std::endl;

}

void ttHHanalyzer_unified::createObjects(event * thisEvent, sysName sysType, bool up){

    _ev->fillObjects();
 
    // =================================================================
    // 1. [Definition] 복잡한 HLT 경로를 의미 있는 변수로 변환
    // =================================================================

    // 1-1. Era 확인 (설정된 eraName을 사용)
    bool isEraB = (_era == "B");
    // TODO: 현재는 2017년도 기준 B run과 나머지(C,D,E,F)만 구분하고 있음.
    // 추후 2016, 2018 Data 분석 시 각 연도별/Era별 정확한 HLT Path 존재 여부 및 Prescale 로직 확인 후
    // 코드를 확장해야 함. (CorrectionsManager 등에서 Map 형태로 관리 권장)

    // 1-2. Trigger Mapping (Era에 따른 HLT 경로 선택)
    // (1) 4J3T (QuadJet + TripleBTag) -> BTagCSV 데이터셋의 주력
    bool fired_4J3T = isEraB ? _ev->HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07 
                             : _ev->HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0;

    // (2) 6J (MultiJet + BTag) -> JetHT 데이터셋의 주력 1
    bool fired_6J1T = isEraB ? _ev->HLT_PFHT430_SixJet40_BTagCSV_p080 
                             : _ev->HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5;
                             
    bool fired_6J2T = isEraB ? _ev->HLT_PFHT380_SixJet32_DoubleBTagCSV_p075 
                             : _ev->HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2;

    // (3) HT (Pure HT) -> JetHT 데이터셋의 주력 2
    bool fired_HT   = _ev->HLT_PFHT1050; // Era 공통


    // 1-3. Logical Grouping (논리 그룹 정의)
    // BTagCSV가 가져가야 할 트리거 그룹
    bool group_BTagCSV = fired_4J3T;

    // JetHT가 가져가야 할 트리거 그룹 (6J OR HT)
    bool group_JetHT   = (fired_6J1T || fired_6J2T || fired_HT);


    // =================================================================
    // 2. [Application] 데이터셋별 역할 분담 (Orthogonality Enforcement)
    // =================================================================
    
    bool passHadTrig = false;

    if (_DataOrMC == "MC") {
        // [MC]: 그냥 뭐라도 터지면 다 가져감 (OR)
        passHadTrig = (group_BTagCSV || group_JetHT);
    }
    else { // [Data]
        if (_sampleName.find("BTagCSV") != std::string::npos) {
            // [Rule 1] BTagCSV 데이터셋은 4J3T 그룹만 챙긴다.
            passHadTrig = group_BTagCSV;
        }
        else if (_sampleName.find("JetHT") != std::string::npos) {
            // [Rule 2] JetHT 데이터셋은 자기 그룹(6J or HT)을 챙기되,
            //          만약 4J3T가 같이 터졌으면 BTagCSV에 양보한다. (Veto)
            passHadTrig = (group_JetHT && !group_BTagCSV);
        }
        else if (_sampleName.find("SingleMuon") != std::string::npos) {
	    // [Rule 3] Single muon for Trigger Study
	    // Handle the logic same as MC trigger path
            passHadTrig = (group_BTagCSV || group_JetHT);
        }
        else {
            // 안전장치
             std::cerr << "[ERROR] Unknown Data Sample: " << _sampleName << std::endl;
             std::exit(EXIT_FAILURE);
        }
    }

    // 최종 결과 적용
    thisEvent->setHadTrigger(passHadTrig);
 
    // Set the noise filter
    // https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2#2018_2017_data_and_MC_UL
    thisEvent->setFilter(_ev->Flag_goodVertices &&
                         _ev->Flag_globalSuperTightHalo2016Filter &&
                         _ev->Flag_HBHENoiseFilter &&
                         _ev->Flag_HBHENoiseIsoFilter &&
                         _ev->Flag_EcalDeadCellTriggerPrimitiveFilter &&
                         _ev->Flag_BadPFMuonFilter &&
                         _ev->Flag_BadPFMuonDzFilter &&
                         _ev->Flag_eeBadScFilter &&
                         _ev->Flag_ecalBadCalibFilter
                         );

  
    thisEvent->setPV(_ev->PV_npvsGood);
    std::vector<eventBuffer::GenPart_s> genPart = _ev->GenPart;      
    std::vector<eventBuffer::GenJet_s> genJet = _ev->GenJet;
    std::vector<eventBuffer::Jet_s> jet = _ev->Jet;
    std::vector<eventBuffer::Muon_s> muonT = _ev->Muon;
    std::vector<eventBuffer::Electron_s> ele = _ev->Electron;
////    std::vector<eventBuffer::FatJet_s> boostedJet = _ev->FatJet;
    objectGenPart * currentGenPart; 
    objectBoostedJet * currentBoostedJet;
    objectJet * currentJet;
    objectLep * currentMuon;
    objectLep * currentEle;
////    objectMET * MET = new objectMET(_ev->PuppiMET_pt, 0, _ev->PuppiMET_phi, 0);
    float e = 1., es  = 1., pe = 1., pes = 1.;
    float me = 1., mes = 1., pme = 1.,  pmes = 1.;   
////    thisEvent->setMET(MET);


////    for(int i=0; i < boostedJet.size(); i++){
////       	currentBoostedJet = new objectBoostedJet(boostedJet[i].pt, boostedJet[i].eta, boostedJet[i].phi, boostedJet[i].mass);
////	currentBoostedJet->softDropMass = boostedJet[i].msoftdrop;
////	
////	if(currentBoostedJet->getp4()->Pt() > cut["boostedJetPt"] && fabs(currentBoostedJet->getp4()->Eta()) < fabs(cut["boostedJetEta"])){
////	    //	    if((boostedJet[i].jetId & 4) == true){  	     
////	    thisEvent->selectBoostedJet(currentBoostedJet);	
////	    if(currentBoostedJet->getp4()->Pt() > cut["hadHiggsPt"]){
////		if(boostedJet[i].particleNet_HbbvsQCD > cut["bTagDisc"]){
////		    thisEvent->selectHadronicHiggs(currentBoostedJet);
////		}
////		//	}
////	    }
////	}
////    }
    

    // Leading lepton def
    // But FH channel don't need this..
    // We just use subleading lepton def for veto
    // update in 5th Jan, 2026
////    bool thereIsALeadLepton = false;
////
////    for(int i = 0; i < muonT.size(); i++){
////        // CHECK: 현재 Veto Muon으로 TightID를 사용 중. 일반적으로 Veto 용도로는 LooseID를 권장함.
////        // Muon POG 권장사항 확인 필요 (예: LooseID + LooseIso).
////        // TightID 사용 시 "Loose하지만 가짜는 아닌" 뮤온을 놓쳐서 Hadronic 채널 오염 가능성 있음.
////        if(fabs(muonT[i].eta) < cut["muonEta"] && muonT[i].tightId == true && muonT[i].pfRelIso04_all  < cut["muonIso"]){
////            if(muonT[i].pt > cut["leadMuonPt"]){
////                thereIsALeadLepton = true;
////                break;
////            }
////        }
////    }
////    if(!thereIsALeadLepton){
////        for(int i = 0; i < ele.size(); i++){
////            // CHECK: Electron Veto 역시 WP90(Tight에 가까움) 사용 중. Egamma POG의 Veto WP 권장사항 확인 필요.
////            if(fabs(ele[i].deltaEtaSC + ele[i].eta) < 1.4442 || fabs(ele[i].deltaEtaSC + ele[i].eta) > 1.5660){  //Electrons tracked neither in the barrel nor in the endcap are discarded.
////                if(fabs(ele[i].eta) < cut["eleEta"] && ele[i].mvaFall17V2Iso_WP90 == true && ele[i].pfRelIso03_all  < cut["eleIso"]){ 
////                    if(ele[i].pt > cut["leadElePt"]){
////                        thereIsALeadLepton = true;
////                        break;
////                    }
////                }
////            }
////        }
////    }


// =============================================================
// Lepton Object Definition   
 
    // =============================================================
    // STEP 1: Count VetoLeptons (Executed in all modes)
    // Purpose: To determine lepton veto for the hadronic channel
    // =============================================================
    int nVetoMuons = 0, nVetoEle = 0;

    for(int i = 0; i < muonT.size(); i++){
        if(fabs(muonT[i].eta) < cut["muonEta"] && 
           muonT[i].tightId == true && 
           muonT[i].pfRelIso04_all < cut["muonIso"] &&
           muonT[i].pt > cut["subLeadMuonPt"]) {
            nVetoMuons++;
        }
    }
    for(int i = 0; i < ele.size(); i++){
        if((fabs(ele[i].deltaEtaSC + ele[i].eta) < 1.4442 || 
            fabs(ele[i].deltaEtaSC + ele[i].eta) > 1.5660) &&
           fabs(ele[i].eta) < cut["eleEta"] && 
           ele[i].mvaFall17V2Iso_WP90 == true && 
           //ele[i].pfRelIso03_all < cut["eleIso"] && // We don't need Iso, It's already in ID
           ele[i].pt > cut["subLeadElePt"]) {
            nVetoEle++;
        }
    }
    // Select all leptons which pass loose selection cut
    thisEvent->setnVetoLepton(nVetoMuons + nVetoEle);
 
   
    // =============================================================
    // STEP 2: Lepton Collection (Executed if collectLeptons is true)
    // =============================================================
    if (_policy.collectLeptons) {
 
        // -------------------------------------------------------------------------
        // 2a. Check Lead Lepton Gate
        // Purpose: To save computing resources, we first check if there is at least 
        //          one "Trigger-capable" (High pT, Tight) lepton in the event.
 	// -------------------------------------------------------------------------
        bool hasLeadMuon = false;
        for(int i = 0; i < muonT.size(); i++){
            // Check for Lead Muon (High pT, Tight ID)
            if(fabs(muonT[i].eta) < cut["muonEta"] && 
               muonT[i].tightId == true && 
               muonT[i].pfRelIso04_all < cut["muonIso"] &&
               muonT[i].pt > cut["leadMuonPt"]) { 
                hasLeadMuon = true;
                break;
            }
        }
        
        // Check for Lead Electron
        // (Calculation is done only if needed or generic, but passGate logic controls usage)
        bool hasLeadElectron = false;
        if (!_policy.requireLeadMuonOnly) { // Optimization: Skip if we only care about Muons
            for(int i = 0; i < ele.size(); i++){
                // Check for Lead Electron (High pT, Tight ID)
                if((fabs(ele[i].deltaEtaSC + ele[i].eta) < 1.4442 || 
                    fabs(ele[i].deltaEtaSC + ele[i].eta) > 1.5660) &&
                   fabs(ele[i].eta) < cut["eleEta"] && 
                   ele[i].mvaFall17V2Iso_WP90 == true && 
                   //ele[i].pfRelIso03_all < cut["eleIso"] && // We don't need Iso, It's already in ID
                   ele[i].pt > cut["leadElePt"]) { 
                    hasLeadElectron = true;
                    break;
                }
            }
        }
        
        // -------------------------------------------------------------------------
        // Define PassGate Logic Explicitly
        // -------------------------------------------------------------------------
        bool passGate = false;
    
        if (_policy.requireLeadMuonOnly) {
            // [TriggerSF / Single Muon Mode]
            // STRICT REQUIREMENT: Must have at least one Leading Muon.
            // We ignore Leading Electrons here because they cannot trigger the Muon HLT.
	    // And Collect All Sublead Electron even event don't satify lead electron cut
            passGate = hasLeadMuon;
        } 
        else {
            // [Standard Mode, You can use this for Leptonic Channel]
            // Requirement: Either a Leading Muon OR a Leading Electron exists.
            passGate = hasLeadMuon || hasLeadElectron;
        }

        // -------------------------------------------------------------------------
        // 2b. Actual Lepton Collection
        // -------------------------------------------------------------------------
        if (passGate) {
        
            // [Muon Collection]
            // Collect ALL muons passing the Veto (Sub-lead) threshold.
            // Even if TriggerSF requires a Lead Muon to pass the gate, 
            // we collect softer muons here to check for Dilepton veto later.
            for(int i = 0; i < muonT.size(); i++){
                if(fabs(muonT[i].eta) < cut["muonEta"] && 
                   muonT[i].tightId == true && 
                   muonT[i].pfRelIso04_all < cut["muonIso"] &&
                   muonT[i].pt > cut["subLeadMuonPt"]) { 
                   
                    currentMuon = new objectLep(muonT[i].pt, muonT[i].eta, muonT[i].phi, 0.);
                    currentMuon->charge = muonT[i].charge;
                    currentMuon->miniPFRelIso = muonT[i].miniPFRelIso_all;
                    currentMuon->pfRelIso04 = muonT[i].pfRelIso04_all;
                    thisEvent->selectMuon(currentMuon);
                }
            }
            
            // [Electron Collection]
            // Always collect Veto Electrons (Sub-lead) to ensure 'nElectrons' is correct.
            // This is crucial for vetoing dilepton events even in Single Muon analysis.
            for(int i = 0; i < ele.size(); i++){
                if((fabs(ele[i].deltaEtaSC + ele[i].eta) < 1.4442 || 
                    fabs(ele[i].deltaEtaSC + ele[i].eta) > 1.5660) &&
                   fabs(ele[i].eta) < cut["eleEta"] && 
                   ele[i].mvaFall17V2Iso_WP90 == true && 
                   //ele[i].pfRelIso03_all < cut["eleIso"] && // We don't need Iso, It's already in ID
                   ele[i].pt > cut["subLeadElePt"]) { 
                   
                    currentEle = new objectLep(ele[i].pt, ele[i].eta, ele[i].phi, 0.);
                    currentEle->charge = ele[i].charge;
                    currentEle->miniPFRelIso = ele[i].miniPFRelIso_all;
                    currentEle->pfRelIso03 = ele[i].pfRelIso03_all;
                    thisEvent->selectEle(currentEle);
                }
            }
        
        // Sort leptons by pT (Descending order)
        thisEvent->orderLeptons();
        }
    
    } // End of [ _policy.collectLeptons ] if statement

// End of Lepton Object Definition   
// =============================================================


    float dR = 0., deltaEta = 0., deltaPhi = 0.;
    bool passPuId = false;
    float rho = _ev->fixedGridRhoFastjetAll;
    for (int i = 0; i < (int)jet.size(); ++i) {

        const auto& jetRaw = jet[i];

        // 1. Pre-cuts
        // [UPDATE] 보정 후 기준으로 pT 컷을 적용하기 위해 여기서는 eta/ID만 최소한으로 확인
        if( !(fabs(jetRaw.eta) < cut["jetEta"] && jetRaw.jetId >= cut["jetID"]) ) continue;
 
        // 2. Calculation (지역 변수 사용, Heap 할당 X)
        float ntuplePt  = jetRaw.pt;                // NanoAOD Default (Corrected)
        float rawFactor = jetRaw.rawFactor;
        float rawPt     = ntuplePt * (1.0f - rawFactor);
        float rawMass   = jetRaw.mass * (1.0f - rawFactor);

            // --- A) Re-apply JEC ---
            double jecSF = corrMgr->getJEC(jetRaw.eta, rawPt, jetRaw.area, rho);
            float ptJEC  = rawPt * jecSF;               // 내가 재계산한 pT
            float massJEC = rawMass * jecSF;


            // --- [Validation] On-the-fly Check --- 
            // (JEC 재적용 관련 validation, 재적용 JEC와 ntuple default JEC pT의 차이가 크면 에러 출력)
            float relDiff = -1.f;        
            if (ntuplePt > 0) {
                relDiff = (ptJEC - ntuplePt) / ntuplePt;
            
                // 디버깅용: 차이가 너무 크면 출력
                if (debugCorrections && fabs(relDiff) > 0.01) { 
                     std::cout << "[JEC Diff] Idx:" << i << " Orig:" << ntuplePt << " New:" << ptJEC << std::endl;
                }
            }
        
            // --- B) Apply JER ---
	    double genPt=-1.f, genEta=0.0, genPhi=0.0;
            if (jetRaw.genJetIdx >= 0 && jetRaw.genJetIdx < (int)genJet.size()) {
                const auto& g = genJet[jetRaw.genJetIdx];
                genPt=g.pt; genEta=g.eta; genPhi=g.phi;
            }
            unsigned int run=_ev->run, lumi=_ev->luminosityBlock;
            unsigned long long event=_ev->event;
            int currentJetIdx = i;

            ////double smearedPt = corrMgr->smearJER(ptJEC, genPt, jetRaw.eta, rho,
	    ////                                      _ev->run, _ev->event, _ev->luminosityBlock, "nom");
            const double smearedPt = corrMgr->smearJER(ptJEC, jetRaw.eta, jetRaw.phi, rho,
                                          run, lumi, event, currentJetIdx,
                                          genPt, genEta, genPhi, "nom");

	    const double jerFactor = (ptJEC>0.0) ? (smearedPt/ptJEC) : 1.0;
	    const double massJECJER = massJEC * jerFactor;

        // 3. Final Cuts (on Smeared pT)
        if (smearedPt < cut["jetPt"]) continue;
        
        // PU ID Check (Low pT only)
        passPuId = true;
        if (smearedPt < 50.0 && jetRaw.puId < cut["jetPUid"]) {
             continue; 
        }

        // 4. Create Final Object (단 한 번만 생성)
        objectJet* newJet = new objectJet(
            smearedPt,
            jetRaw.eta,
            jetRaw.phi,
            massJECJER
        );

        // 메타데이터 저장
        newJet->JEC_DiffRatio = relDiff;
        newJet->bTagCSV       = jetRaw.btagDeepFlavB;
        newJet->jetID         = jetRaw.jetId;
        newJet->jetPUid       = jetRaw.puId;
        newJet->passPuId      = passPuId;
        newJet->hadFlav       = jetRaw.hadronFlavour;
        newJet->partonFlav    = jetRaw.partonFlavour;
        newJet->genMatchedPt  = genPt; // 필요하다면
        if (jetRaw.mass > 0.0f) {
            newJet->mass_DiffRatio = (massJEC - jetRaw.mass) / jetRaw.mass;
        } else {
            newJet->mass_DiffRatio = 0.0f;
        }

        // 이벤트에 등�
	// WP에 따른 분류 로직
        thisEvent->selectJet(newJet);
        if (newJet->bTagCSV >= objectJet::valbTagMedium) {
            thisEvent->selectbJet(newJet);
        } 
	else if (newJet->bTagCSV < objectJet::valbTagLoose) {
            thisEvent->selectLightJet(newJet);
        }
////        if (newJet->bTagCSV >= objectJet::valbTagLoose) {
////            thisEvent->selectLoosebJet(newJet);
////        }

    }   // <--- Jet Loop End (여기가 닫혔는지 꼭 확인!)
    
	thisEvent->orderJets();

    // =================================================================
    // GenPart Loop: Memory Leak Fix
    // =================================================================
    for (int i = 0; i < (int)genPart.size(); i++) {
        
        // [중요] 조건 먼저 검사 (조건 불만족 시 객체 생성 안 함 -> 메모리 누수 방지)
        bool isBQuark = (abs(genPart[i].pdgId) == 5);
        bool isHard   = (genPart[i].statusFlags & 256);

        if (!isBQuark || !isHard) continue;

        // 합격한 경우에만 new 할당
        currentGenPart = new objectGenPart(
            genPart[i].pt, genPart[i].eta, genPart[i].phi, genPart[i].mass
        );
        currentGenPart->hasHiggsMother = false;
        currentGenPart->hasTopMother   = false;

        // Mother Tracing (While loop로 간소화)
        int motherInd = genPart[i].genPartIdxMother;
        while (motherInd >= 0 && motherInd < (int)genPart.size()) {
            int mPDG = abs(genPart[motherInd].pdgId);
            bool mStatus = (genPart[motherInd].statusFlags & 256);

            if (mStatus) {
                if (mPDG == 25) currentGenPart->hasHiggsMother = true;
                if (mPDG == 6)  currentGenPart->hasTopMother   = true;
            }
            if (currentGenPart->hasHiggsMother && currentGenPart->hasTopMother) break;
            
            motherInd = genPart[motherInd].genPartIdxMother;
        }

        thisEvent->selectGenPart(currentGenPart);
    } // GenPart Loop End

} // CreateObject Function End
           


// ============================================================================
// selectObjects — 이벤트 선택 + Trigger SF + B-tag Reweight 적용
//
// Weight 적용 순서 (Weight multiplication order):
//   _evtWeight = baseWeight × PU × L1PreFiring × genWeight
//                             (← process() 함수에서 이미 계산됨)
//   → computeBTagWeight()  로 bTagWeight_central_ 계산됨
//   → selectObjects()에서:
//       Step kNoCut   : _evtWeight (b-tag SF 미포함) 로 cutflow 시작
//       Step kHadTrigger~kHT : 기본 selection cuts
//       Step kNumbJets : b-jet cut 적용 후
//       Step kHadWMass  → kHiggsMass
//       Step kTotal    : _evtWeight에 btagSF × trigSF × btagReweight 곱한 최종 weight
//
// ═══════════════════════════════════════════════════════════════════════════
// 설계 결정 (Design Decision):
//   Trigger SF와 B-tag Reweight는 모든 selection cut을 통과한 후,
//   kTotal step 직전에 _evtWeight에 곱해진다.
//   이유: 두 보정 모두 selection 후의 kinematics (HT, nJets, jet6PT)에 의존하므로
//   cut 이전에 적용하면 의미가 없다.
//   단, cutflow 중간 단계에서는 보정 전 weight를 사용하여 selection efficiency를
//   편향 없이 계산할 수 있도록 한다.
// ═══════════════════════════════════════════════════════════════════════════
bool ttHHanalyzer_unified::selectObjects(event *thisEvent){
     
    // Initialize three diagnostic weight chains to the raw event weight so
    // pre-SF cutflow steps (noCut through HT) are filled consistently.
    _evtWeight_chain_raw    = _evtWeight;
    _evtWeight_chain_btagSF = _evtWeight;
    _evtWeight_chain_full   = _evtWeight;

    // ──────────────────────────────────────────────────────────────────────
    // processStep 람다 (lambda): cutflow 카운트 및 히스토그램 채우기
    // 각 selection cut 단계에서 호출하여 이벤트 수/가중치를 기록한다.
    // ──────────────────────────────────────────────────────────────────────
    auto processStep = [&](CutStep step, float wMassVal) {
        int idx = static_cast<int>(step);
        
        if (idx < _cutFlowCount.size()) {
            _cutFlowCount[idx]  += 1.0;
            _cutFlowWeight[idx] += _evtWeight;   // legacy
        }

        hCutFlow         ->Fill(idx);
        hCutFlow_w       ->Fill(idx, _evtWeight_chain_raw);
        hCutFlow_w_btagSF->Fill(idx, _evtWeight_chain_btagSF);
        hCutFlow_w_full  ->Fill(idx, _evtWeight_chain_full);

        fillCutStepHist(step, thisEvent, wMassVal);
    };


    // ──────────────────────────────────────────────────────────────────────
    // Hadronic W Mass 계산 (W boson mass reconstruction)
    // light jet이 2개 이상이면 light jet 쌍에서, 아니면 전체 jet에서 계산
    // ──────────────────────────────────────────────────────────────────────
    const float wMass = 80.377f;
    float hadWMass = closestMassPair(
        thisEvent->getSelLightJets()->size() >= 2 ? thisEvent->getSelLightJets() : thisEvent->getSelJets(),
        wMass
    );


    // ──────────────────────────────────────────────────────────────────────
    // Higgs Reconstruction (Higgs 재구성)
    //
    // ttHH(4b) 분석에서 H→bb 붕괴를 재구성한다.
    // b-jet 4개를 2쌍으로 나누어 각 쌍의 불변질량(invariant mass)이
    // Higgs 질량 (125 GeV)에 가장 가까운 조합을 χ² 최소화로 선택한다.
    //
    // [최적화] Pair Cache 기반 알고리즘:
    //   1) 모든 b-jet 쌍의 불변질량 + pT합을 O(N²) 으로 미리 계산 (precompute)
    //   2) C(N,4) × 3 pairing 순회 시 TLorentzVector 덧셈을 재사용
    //   → 기존 대비 TLorentzVector 연산량 ~60% 절감
    //
    // 복잡도 (Complexity): O(N²) 캐시 + O(N⁴) 탐색 (기존과 동일)
    //   nbJets=4: 3 pairings (최소)
    //   nbJets=6: 45 pairings
    //   nbJets=8: 210 pairings
    // ──────────────────────────────────────────────────────────────────────
    _minChi2Higgs = cLargeValue;
    _bbMassMin1Higgs = -1.0f;
    _bbMassMin2Higgs = -1.0f;

    auto* bjets = thisEvent->getSelbJets();

    // [Validation] In kValidationStudy mode the b-jet cut may be loosened
    // (e.g. nbJetsCut=2). We still want to run Higgs reco only when there
    // are enough b-jets; when there aren't, leave _minChi2Higgs at the
    // sentinel value (cLargeValue) so downstream fillCutStepHist treats
    // these events as "no Higgs reco available".
    const int hRecoMin =
        (_analysisMode == AnalysisMode::kValidationStudy)
            ? _valCfg.forceHiggsRecoMinBjets
            : 4;  // unchanged behaviour for all other modes
    if (_policy.doHiggsReconstruction) {
        const size_t nb = bjets->size();
        // Also guard against hRecoMin <= 0 (effectively disabled by user).

        if (hRecoMin > 0 && nb >= static_cast<size_t>(hRecoMin)) {
            // ═════════════════════════════════════════════════════════════
            // Step 1: Pair Cache 구축 (Build pair cache)
            //
            // pairMass[i][j] = (bjet_i + bjet_j).M()
            // pairSumPt[i][j] = bjet_i.Pt() + bjet_j.Pt()
            // pairPt[i][j] = (bjet_i + bjet_j).Pt()
            //
            // 대칭(symmetric)이므로 i < j 만 채운다.
            // ═════════════════════════════════════════════════════════════
            struct PairInfo {
                float mass;     // 불변질량 (invariant mass)
                float sumPt;    // pT 합 (sum of individual pTs, for χ² denominator)
                float pairPt;   // 쌍의 pT (pair system pT, for _bpTHiggs)
            };

            // nb × nb 상삼각 행렬 (upper-triangular matrix)
            // flat vector 사용으로 cache locality 향상
            std::vector<PairInfo> pairCache(nb * nb);
            auto idx = [nb](size_t i, size_t j) -> size_t { return i * nb + j; };

            for (size_t i = 0; i < nb; ++i) {
                for (size_t j = i + 1; j < nb; ++j) {
                    TLorentzVector sum = *bjets->at(i)->getp4() + *bjets->at(j)->getp4();
                    pairCache[idx(i,j)].mass   = static_cast<float>(sum.M());
                    pairCache[idx(i,j)].sumPt  = static_cast<float>(
                        bjets->at(i)->getp4()->Pt() + bjets->at(j)->getp4()->Pt());
                    pairCache[idx(i,j)].pairPt = static_cast<float>(sum.Pt());
                }
            }

            // ═════════════════════════════════════════════════════════════
            // Step 2: C(N,4) × 3 pairings 순회 (Iterate all pairings)
            //
            // 4개의 b-jet {i,j,k,l}을 2쌍으로 나누는 방법은 3가지:
            //   Pairing A: (i,j) + (k,l)
            //   Pairing B: (i,k) + (j,l)
            //   Pairing C: (i,l) + (j,k)
            //
            // χ² = (m_pair1 - m_H)² / σ₁ + (m_pair2 - m_H)² / σ₂
            // where σ = √((sumPt / 2) × 0.2)
            //   (20% 분해능 가정, resolution assumption)
            // ═════════════════════════════════════════════════════════════
            auto tryPairing = [&](size_t a, size_t b, size_t c, size_t d) {
                const auto& p1 = pairCache[idx(a, b)];
                const auto& p2 = pairCache[idx(c, d)];

                float chi2 = std::pow(p1.mass - cHiggsMass, 2) / std::pow(p1.sumPt / 2.0f * 0.2f, 0.5f)
                            + std::pow(p2.mass - cHiggsMass, 2) / std::pow(p2.sumPt / 2.0f * 0.2f, 0.5f);

                if (_minChi2Higgs > chi2) {
                    _minChi2Higgs   = chi2;
                    _bbMassMin1Higgs = p1.mass;
                    _bbMassMin2Higgs = p2.mass;
                    _bpTHiggs1       = p1.pairPt;
                    _bpTHiggs2       = p2.pairPt;
                }
            };

            for (size_t i = 0; i < nb - 3; ++i) {
                for (size_t j = i + 1; j < nb - 2; ++j) {
                    for (size_t k = j + 1; k < nb - 1; ++k) {
                        for (size_t l = k + 1; l < nb; ++l) {
                            // 3가지 pairing 시도
                            tryPairing(i, j, k, l);   // (i,j) + (k,l)
                            tryPairing(i, k, j, l);   // (i,k) + (j,l)
                            tryPairing(i, l, j, k);   // (i,l) + (j,k)
                        }
                    }
                }
            }
        } // end if (hRecoMin > 0 && nb >= hRecoMin)
    } // end if doHiggsReconstruction


    // ══════════════════════════════════════════════════════════════════════
    // Selection Cuts (이벤트 선택 조건)
    //
    // 각 단계에서 조건을 충족하지 못하면 false를 반환하여 이벤트를 버린다.
    // processStep()으로 cutflow를 기록한다.
    // ══════════════════════════════════════════════════════════════════════

    // Step 0: No Cut — 모든 이벤트 카운트
    processStep(CutStep::kNoCut, hadWMass);

    // Step 1: Trigger — Hadronic trigger path 통과 여부
    // Trigger Study와 B-tag SF 도출 시에는 trigger cut을 끌 수 있음 (policy 제어)
    if (_policy.applyTriggerCut) { 
        if(cut["trigger"] > 0 && thisEvent->getHadTriggerAccept() == false){
            return false;
        }
    }
    processStep(CutStep::kHadTrigger, hadWMass);

    // Step 2: Noise Filter — MET noise filter 통과 여부
    if(cut["filter"] > 0 && thisEvent->getMETFilter() == false){
        return false;
    }
    processStep(CutStep::kNoiseFilter, hadWMass);

    // Step 3: Primary Vertex — 유효한 primary vertex 존재 여부
    if(cut["pv"] > 0 && thisEvent->getPVvalue() == false){
        return false;
    }
    processStep(CutStep::kPrimaryVertex, hadWMass);

    // Step 4: nJets >= 6 (또는 설정값) — 최소 jet 수 요구
    if(!(thisEvent->getnSelJet() >= cut["nJets"] )){
        return false;
    }
    processStep(CutStep::kNumJets, hadWMass);

    // Step 5: 6th Jet Pt > 40 — 6번째 jet의 pT 하한
    if(!(thisEvent->getSelJets()->at(5)->getp4()->Pt() > cut["6thJetsPT"])){
        return false;
    }
    processStep(CutStep::kSixthJetPt, hadWMass);

    // Step 6: Lepton Veto (nLepton == 0) — 렙톤 거부 조건
    if (_policy.requireSingleMuon) {
        // TriggerSF 도출 모드: 정확히 muon 1개, electron 0개 요구
        if (!(thisEvent->getnSelMuon() == 1 && thisEvent->getnSelElectron() == 0)) {
            return false;
        }
    }
    else if (_policy.applyLeptonVeto) {
        // Main Analysis 모드: lepton veto (nLepton == 0)
        if(!(thisEvent->getnVetoLepton() == cut["nLeptons"])){
            return false;
        }
    }
    processStep(CutStep::kLeptonVeto, hadWMass);

    // (보조 통계 계산: lepton-jet statistics, b-jet-lepton statistics)
    thisEvent->getStatsComb(thisEvent->getSelJets(), thisEvent->getSelLeptons(), ljetStat);
    thisEvent->getStatsComb(thisEvent->getSelbJets(), thisEvent->getSelLeptons(), lbjetStat);

    // Step 7: HT > 500 — Scalar pT sum 하한
    if(!(thisEvent->getSumSelJetScalarpT() > cut["HT"])){
        return false;
    }
    processStep(CutStep::kHT, hadWMass);

    // ══════════════════════════════════════════════════════════════════════
    // [MOVED from post-step-10 to pre-step-8]
    // Event-level corrections applied here — between HT cut (step 7) and
    // nbjet cut (step 8) — so cutflow histograms from step 8 onwards see
    // the SF-corrected MC. This is required to read the b-tag SF / trigger
    // SF / norm reweight effect from the cutflow ratios.
    //
    // Each SF can be individually disabled in kValidationStudy mode via
    // _valCfg toggles; the unmodified main analysis runs the full chain.
    //
    // [ttH AN A.2.1] inclusive ttbar dispatched by genTtbarId; non-ttbar
    // by sample name. See Config_TtCatGroup.hh::MakeProcessKey().
    // ══════════════════════════════════════════════════════════════════════
    triggerSF_        = 1.0f;
    triggerSF_up_     = 1.0f;
    triggerSF_down_   = 1.0f;
    btagNormReweight_ = 1.0f;

    const bool isVal = (_analysisMode == AnalysisMode::kValidationStudy);
    const bool useBtagShape = isVal ? _valCfg.applyBtagShapeSF : true;
    const bool useTrigSF    = isVal ? _valCfg.applyTriggerSF   : true;
    const bool useBtagNorm  = isVal ? _valCfg.applyBtagNormSF  : true;

    // Always populate the 3 chain weights with the FULL meaning, regardless
    // of validation toggles — they are diagnostic, not the production path.
    _evtWeight_chain_raw    = _evtWeight;     // baseline only
    _evtWeight_chain_btagSF = _evtWeight;     // will multiply btagShape below
    _evtWeight_chain_full   = _evtWeight;     // will multiply all SFs below

    if (_DataOrMC != "Data") {

        // ── b-tag shape SF ─────────────────────────────────────────────
        // Both chains get it (even if legacy _evtWeight skips it in
        // validation when toggle is off).
        _evtWeight_chain_btagSF *= bTagWeight_central_;
        _evtWeight_chain_full   *= bTagWeight_central_;
        if (useBtagShape) _evtWeight *= bTagWeight_central_;

        // ── Trigger SF ────────────────────────────────────────────────
        const int    nbjet   = thisEvent->getnSelbJet();
        const double ht      = thisEvent->getSumSelJetScalarpT();
        const double jet6pt  = thisEvent->getSelJets()->at(5)->getp4()->Pt();
        const double jet6eta = thisEvent->getSelJets()->at(5)->getp4()->Eta();

        triggerSF_      = static_cast<float>(corrMgr->getTriggerSF(nbjet, jet6eta, ht, jet6pt,  0.0));
        triggerSF_up_   = static_cast<float>(corrMgr->getTriggerSF(nbjet, jet6eta, ht, jet6pt, +1.0));
        triggerSF_down_ = static_cast<float>(corrMgr->getTriggerSF(nbjet, jet6eta, ht, jet6pt, -1.0));

        _evtWeight_chain_full *= triggerSF_;
        if (useTrigSF) _evtWeight *= triggerSF_;

        // ── b-tag normalization reweight ──────────────────────────────
        // [ttHH AN-2022/122 / ttH AN App. A.2.1] the b-tag-shape SF distorts
        // the per-category normalization, so it is restored per (sample, HF
        // category). Key on the EXPANDED id (not NanoAOD genTtbarId) so tt+nb
        // (61/62/71/72) carries its own reweight bin, distinct from tt+2b
        // (53/54/55) — high b-jet multiplicity is exactly where tt+nb lives.
        // The stitch multiplier above already fixed the MC composition feeding
        // this derivation, so the reweight is computed on the stitched mix.
        const int nJets = thisEvent->getnSelJet();
        const std::string processKey = TtCatGroup::MakeProcessKey(
            _sampleName, _expandedTtbarId);
        if (processKey.empty()) {
            std::cerr << "\n[FATAL][btagRW] MakeProcessKey() returned an EMPTY key for"
                      << " sample='" << _sampleName << "' expandedTtbarId="
                      << _expandedTtbarId << " (sub="
                      << (((_expandedTtbarId % 100) + 100) % 100) << ").\n"
                      << "  Config_TtCatGroup.hh must map this code. Aborting (exit 45)"
                      << " so the Condor job is flagged.\n" << std::endl;
            std::exit(45);
        }
        // diagnostic: remember the (expandedSub -> processKey) mapping once, so
        // the end-of-job log shows whether 61/62/71/72 get keys distinct from 53.
        {
            const int esub = ((_expandedTtbarId % 100) + 100) % 100;
            if (_btagKeyByExpSub.find(esub) == _btagKeyByExpSub.end())
                _btagKeyByExpSub[esub] = processKey;
        }
        btagNormReweight_ = static_cast<float>(
            corrMgr->getBTagReweight("central", processKey, nJets, ht));
        if (!std::isfinite(btagNormReweight_)) {
            std::cerr << "\n[FATAL][btagRW] non-finite reweight (" << btagNormReweight_
                      << ") for processKey='" << processKey << "' nJets=" << nJets
                      << " ht=" << ht << ". Aborting (exit 46).\n" << std::endl;
            std::exit(46);
        }

        _evtWeight_chain_full *= btagNormReweight_;
        if (useBtagNorm) _evtWeight *= btagNormReweight_;

        if (debugCorrections) {
            std::cout << "[selectObjects] Corrections (mode="
                      << analysisModeName(_analysisMode) << "):"
                      << " sample=" << _sampleName
                      << " genTtbarId=" << _ev->genTtbarId
                      << " expandedTtbarId=" << _expandedTtbarId
                      << " processKey=" << processKey
                      << " btagSF=" << bTagWeight_central_
                      << " trigSF=" << triggerSF_
                      << " btagRW=" << btagNormReweight_
                      << " w_raw="    << _evtWeight_chain_raw
                      << " w_btagSF=" << _evtWeight_chain_btagSF
                      << " w_full="   << _evtWeight_chain_full
                      << " _evtWeight=" << _evtWeight
                      << std::endl;
        }
    }


    // ─── b-tag cut sequence — three steps (≥2, ≥3, ≥4) ────────────────
    // Each step is a SEPARATE cut + processStep so the cutflow and per-step
    // distributions show the impact of every increment.
    //
    // In main mode (and other non-validation modes) the loosest cut here
    // is applyBJetCut → ≥2 (per cut["nbJets"]). The tighter steps (≥3,
    // ≥4) are processed for histograms first, then enforced as cuts only
    // for the kTotal "SR" semantics. This way:
    //   - main mode SR is ≥2 b-tags (matches ttH AN baseline)
    //   - looking at cutStep ≥3 / ≥4 histograms tells you what the SR
    //     would look like if tightened
    //
    // In validation mode the legacy single-cut behaviour is preserved.

    if (_analysisMode == AnalysisMode::kValidationStudy) {
        // Validation mode: single cut at _valCfg.nbJetsCut (existing logic)
        const int needNb = _valCfg.nbJetsCut;
        const bool cutEnabled = (_valCfg.nbJetsCut >= 0);
        if (cutEnabled) {
            if (!(thisEvent->getnSelbJet() >= needNb)) return false;
        }
        // Fill all three b-tag step hists (the event passes here, so it
        // is at least at the level the user requested — duplicating into
        // the ≥2/≥3/≥4 step hists is harmless for validation purposes).
        processStep(CutStep::kNumbJets2, hadWMass);
        if (thisEvent->getnSelbJet() >= 3) processStep(CutStep::kNumbJets3, hadWMass);
        if (thisEvent->getnSelbJet() >= 4) processStep(CutStep::kNumbJets4, hadWMass);
    }
    else {
        // Production-mode three-tier b-tag cutflow.
        // Loosest cut first (gates whether we proceed at all in this mode):
        const int needNbBaseline = static_cast<int>(cut["nbJets"]);  // ≥2
        if (_policy.applyBJetCut) {
            if (!(thisEvent->getnSelbJet() >= needNbBaseline)) return false;
        }
        processStep(CutStep::kNumbJets2, hadWMass);

        // ≥3 — record but do NOT cut; the user reads the cutflow ratio
        // at this step to see the next-level effect.
        if (thisEvent->getnSelbJet() >= 3) {
            processStep(CutStep::kNumbJets3, hadWMass);
        }
        // ≥4 — same idea
        if (thisEvent->getnSelbJet() >= 4) {
            processStep(CutStep::kNumbJets4, hadWMass);
        }
    }

    // Step kHadWMass: hadronic W mass window
    {
        const bool hadWEnabled =
            (_analysisMode == AnalysisMode::kValidationStudy)
                ? _valCfg.applyHadWWindow
                : _policy.applyHadWMassCut;
        if (hadWEnabled) {
            if (hadWMass < 30.0f || hadWMass > 250.0f) {
                return false;
            }
        }
    }
    processStep(CutStep::kHadWMass, hadWMass);

    // Step 10: Higgs Mass Window (현재 사용하지 않음, 05 Jan 2026)
    processStep(CutStep::kHiggsMass, hadWMass);



    // Step 11: Total — 최종 이벤트 (모든 cut + 모든 보정 적용)
    processStep(CutStep::kTotal, hadWMass);

   
    return true;
}


void ttHHanalyzer_unified::motherReco(const TLorentzVector & dPar1p4,const TLorentzVector & dPar2p4, const float mother1mass, float & _minChi2,float & _bbMassMin1){
    float bbMass1, chi2;
    bbMass1 = (dPar1p4+dPar2p4).M();
    chi2 = pow((bbMass1 - mother1mass),2)/pow((dPar1p4.Pt()+dPar2p4.Pt())/2.*0.02,0.5);
    if(_minChi2 > chi2){
	_minChi2      = chi2;
	_bbMassMin1   = bbMass1;
	_bpTHiggs1    = (dPar1p4+dPar2p4).Pt();
    }
} 


void ttHHanalyzer_unified::diMotherReco(const TLorentzVector & dPar1p4,const TLorentzVector & dPar2p4,const TLorentzVector & dPar3p4,const TLorentzVector & dPar4p4, const float mother1mass, const float  mother2mass, float & _minChi2,float & _bbMassMin1, float & _bbMassMin2){
    float bbMass1, bbMass2, chi2;
    bbMass1 = (dPar1p4+dPar2p4).M();
    bbMass2 = (dPar3p4+dPar4p4).M();
    // [FIX] 2nd term denominator 0.02 -> 0.2 corrected. Assuming 20% resolution for both candidates.
    chi2 = pow((bbMass1 - mother1mass),2)/pow((dPar1p4.Pt()+dPar2p4.Pt())/2.*0.2,0.5)
         + pow((bbMass2 - mother2mass),2)/pow((dPar3p4.Pt()+dPar4p4.Pt())/2.*0.2,0.5);
    if(_minChi2 > chi2){
	_minChi2      = chi2;
	_bbMassMin1   = bbMass1;
	_bbMassMin2   = bbMass2;
	_bpTHiggs1    = (dPar1p4+dPar2p4).Pt();
	_bpTHiggs2    = (dPar3p4+dPar4p4).Pt();
    }
} 

void ttHHanalyzer_unified::analyze(event *thisEvent){

    //////std::vector<objectJet*>* bJetsInv = thisEvent->getSelbJets(); 
    //////std::vector<objectJet*>* lbJetsInv = thisEvent->getLoosebJets(); 
    //////std::vector<objectJet*>* jetsInv = thisEvent->getSelJets(); 

    //////std::vector<TVector3> vectorsJet, vectorsBjet;
    //////// Event Shape Calculation & genbjet matching for mother particle
    //////for(int k = 0; k < jetsInv->size(); k++){
    //////     vectorsJet.push_back(jetsInv->at(k)->getp4()->Vect());
    //////}
    //////for(int m = 0; m < bJetsInv->size(); m++){
    //////    vectorsBjet.push_back(bJetsInv->at(m)->getp4()->Vect());
    //////    if(thisEvent->getnGenPart() < 1) continue;
    //////    bJetsInv->at(m)->matchedtoHiggs = false;	    	
    //////    for(auto genParticle: *thisEvent->getGenParts()){
    //////        float dR = bJetsInv->at(m)->getp4()->DeltaR( *genParticle->getp4());
    //////        if(genParticle->hasHiggsMother == true && dR < 0.8 && (fabs(bJetsInv->at(m)->getp4()->Pt() - (*genParticle->getp4()).Pt()) < bJetsInv->at(m)->getp4()->Pt()*0.4) ){
    //////          if(dR > genParticle->dRmatched) {
    //////    	  //std::cout << "this was matched to a closer particle before" << std::endl;
    //////          }else if( genParticle->matched){
    //////    	  //std::cout << "this was matched before" << std::endl;
    //////          }
    //////          bJetsInv->at(m)->matchedtoHiggs   = true;	
    //////          bJetsInv->at(m)->matchedtoHiggsdR = dR;	
    //////          genParticle->matched = true;
    //////          genParticle->dRmatched = dR;
    //////          break;
    //////        }
    //////    } 
    //////    //	std::cout << bJetsInv->at(m)->matchedtoHiggsb << std::endl;  
    //////}

    //////_minChi2Higgs  = cLargeValue;
    //////_minChi2Z      = cLargeValue;
    //////_minChi2HiggsZ = cLargeValue;
    //////_minChi2SHiggsNotMatched = cLargeValue;
    //////_minChi2SHiggsMatched = cLargeValue;
    //////_minChi2HHNotMatched = cLargeValue;
    //////_minChi2HHMatched = cLargeValue;
    //////_bbMassMinSHiggsMatched = -1;
    //////_bbMassMinSHiggsNotMatched = -1;
    //////_bbMassMinHH1Matched = -1;
    //////_bbMassMinHH1NotMatched = -1;
    //////_bbMassMinHH2Matched = -1;
    //////_bbMassMinHH2NotMatched = -1;

    //////float tempminChi2 = cLargeValue, tmpMassMin1HiggsZ = 0., tmpMassMin2HiggsZ = 0.;
    //////float tempMinChi2SHiggs = cLargeValue, tempMinChi2SHiggs_r = cLargeValue, tmpMassMinSHiggs = 0.;
    //////float tempMinChi2SHiggsMatched = cLargeValue, tempMinChi2SHiggsMatched_r = cLargeValue, tmpMassMinSHiggsMatched = 0.;
    //////float tempMinChi2SHiggsNotMatched = cLargeValue, tempMinChi2SHiggsNotMatched_r = cLargeValue, tmpMassMinSHiggsNotMatched = 0.;
    ////////extract H
    //////for( int ibjet1 = 0; ibjet1 < bJetsInv->size(); ibjet1++){
    //////    tempMinChi2SHiggs = cLargeValue;
    //////    bJetsInv->at(ibjet1)->minChiHiggsIndex = -1;
    //////    for( int ibjet2 = 0; ibjet2 < bJetsInv->size(); ibjet2++){
    //////        if( ibjet1 == ibjet2) continue;	   
    //////        tempMinChi2SHiggs_r = tempMinChi2SHiggs;
    //////        motherReco(*bJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4(),cHiggsMass, tempMinChi2SHiggs, tmpMassMinSHiggs);
    //////        if(tempMinChi2SHiggs_r > tempMinChi2SHiggs){
    //////    	bJetsInv->at(ibjet1)->minChiHiggsIndex = ibjet2;
    //////        }
    //////        if(bJetsInv->at(ibjet1)->matchedtoHiggs == true && bJetsInv->at(ibjet2)->matchedtoHiggs == true){
    //////    	motherReco(*bJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4(),cHiggsMass, _minChi2SHiggsMatched, _bbMassMinSHiggsMatched);		
    //////        } else if (bJetsInv->at(ibjet1)->matchedtoHiggs == false && bJetsInv->at(ibjet2)->matchedtoHiggs == false){
    //////    	motherReco(*bJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4(),cHiggsMass, _minChi2SHiggsNotMatched, _bbMassMinSHiggsNotMatched);		
    //////        }
    //////    }
    //////    bJetsInv->at(ibjet1)->minChiHiggs = tempMinChi2SHiggs;
    //////}


    //////// HH & ZZ reco : 4 medium b jet case

    //////if(thisEvent->getnSelbJet() >  3){
    //////    for( int ibjet1 = 0; ibjet1 < bJetsInv->size(); ibjet1++){
    //////        for( int ibjet2 = ibjet1+1; ibjet2 < bJetsInv->size(); ibjet2++){
    //////    	if( ibjet1 == ibjet2) continue;
    //////    	for( int ibjet3 = 1; ibjet3 < bJetsInv->size(); ibjet3++){
    //////    	    if(ibjet1 == ibjet3 || ibjet2 == ibjet3) continue;
    //////    	    for( int ibjet4 = ibjet3+1; ibjet4 < bJetsInv->size(); ibjet4++){
    //////    		if(ibjet1 == ibjet4 || ibjet2 == ibjet4 || ibjet3 == ibjet4 ) continue;		
    //////    		if(bJetsInv->at(ibjet1)->matchedtoHiggs == true && bJetsInv->at(ibjet2)->matchedtoHiggs == true && bJetsInv->at(ibjet3)->matchedtoHiggs == true && bJetsInv->at(ibjet4)->matchedtoHiggs == true){
    //////    		    diMotherReco(*bJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
    //////    				 , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4() 
    //////    				 , cHiggsMass, cHiggsMass, _minChi2HHMatched, _bbMassMinHH1Matched, _bbMassMinHH2Matched);
    //////    		} else {
    //////    		    diMotherReco(*bJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
    //////    				 , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4() 
    //////    				 , cHiggsMass, cHiggsMass, _minChi2HHNotMatched, _bbMassMinHH1NotMatched, _bbMassMinHH2NotMatched);
    //////    		}
    //////    		diMotherReco(*bJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
    //////    			     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4() 
    //////    			     , cHiggsMass, cHiggsMass, _minChi2Higgs, _bbMassMin1Higgs, _bbMassMin2Higgs);
    //////    		diMotherReco(*bJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
    //////    			     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
    //////    			     , cZMass, cZMass, _minChi2Z, _bbMassMin1Z, _bbMassMin2Z);  
    //////    		diMotherReco(*bJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4() //ZH 
    //////    			     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
    //////    			     , cHiggsMass, cZMass, _minChi2HiggsZ, _bbMassMin1HiggsZ, _bbMassMin2HiggsZ); 
    //////    	    }
    //////    	}
    //////        }
    //////    }
    //////    // HH & ZZ reco : 3 medium + 1 loose b jet case
    //////}
    ////else if(thisEvent->getnSelbJet() == 3 && thisEvent->getnbLooseJet() > 3){
    ////    for( int ibjet1 = 0; ibjet1 < lbJetsInv->size(); ibjet1++){
    ////        for( int ibjet2 = 0; ibjet2 < bJetsInv->size(); ibjet2++){
    ////    	if( lbJetsInv->at(ibjet1) == bJetsInv->at(ibjet2)) continue;
    ////    	for( int ibjet3 = 0; ibjet3 < bJetsInv->size(); ibjet3++){
    ////    	    if(lbJetsInv->at(ibjet1) == bJetsInv->at(ibjet3) || ibjet2 == ibjet3) continue;
    ////    	    for( int ibjet4 = ibjet3+1; ibjet4 < bJetsInv->size(); ibjet4++){
    ////    		if(lbJetsInv->at(ibjet1) == bJetsInv->at(ibjet4) || ibjet2 == ibjet4 || ibjet3 == ibjet4 ) continue;		
    ////    		if( bJetsInv->at(ibjet2)->matchedtoHiggs == true && bJetsInv->at(ibjet3)->matchedtoHiggs == true && bJetsInv->at(ibjet4)->matchedtoHiggs == true){
    ////    		    diMotherReco(*lbJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
    ////    				 , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4() 
    ////    				 , cHiggsMass, cHiggsMass, _minChi2HHMatched, _bbMassMinHH1Matched, _bbMassMinHH2Matched);
    ////    		} else {
    ////    		    diMotherReco(*lbJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
    ////    				 , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4() 
    ////    				 , cHiggsMass, cHiggsMass, _minChi2HHNotMatched, _bbMassMinHH1NotMatched, _bbMassMinHH2NotMatched);
    ////    		}

    ////    		diMotherReco(*lbJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
    ////    			     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
    ////    			     , cHiggsMass, cHiggsMass, _minChi2Higgs, _bbMassMin1Higgs, _bbMassMin2Higgs); 
    ////    		diMotherReco(*lbJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
    ////    			     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
    ////    			     , cZMass, cZMass, _minChi2Z, _bbMassMin1Z, _bbMassMin2Z); 
    ////    		// ZH combinatorics 
    ////    		diMotherReco(*lbJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4() // ZH H(lbb)Z(bb)
    ////    			     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
    ////    			     , cHiggsMass, cZMass, _minChi2HiggsZ, _bbMassMin1HiggsZ, _bbMassMin2HiggsZ);  
    ////    		tempminChi2 = _minChi2HiggsZ; tmpMassMin1HiggsZ = _bbMassMin1HiggsZ; tmpMassMin2HiggsZ = _bbMassMin2HiggsZ;
    ////    		diMotherReco(*lbJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4() // ZH Z(lbb)H(bb)
    ////    			     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
    ////    			     , cZMass, cHiggsMass, _minChi2HiggsZ, _bbMassMin2HiggsZ, _bbMassMin1HiggsZ); 
    ////    		// pick the lowest minChi2 for the two combinatorics
    ////    		if(tempminChi2 < _minChi2HiggsZ){
    ////    		    _minChi2HiggsZ = tempminChi2; _bbMassMin1HiggsZ = tmpMassMin1HiggsZ; _bbMassMin2HiggsZ = tmpMassMin2HiggsZ;
    ////    		}
    ////    	    }
    ////    	}
    ////        }
    ////    }
    ////    // HH & ZZ reco : 3 medium b jet + 1 jet case 
    ////}
    ////else if(thisEvent->getnSelbJet() == 3){
    ////    for( int ijet1 = 0; ijet1 < jetsInv->size(); ijet1++){
    ////        for( int ibjet2 = 0; ibjet2 < bJetsInv->size(); ibjet2++){
    ////    	if( jetsInv->at(ijet1) == bJetsInv->at(ibjet2)) continue;
    ////    	for( int ibjet3 = 0; ibjet3 < bJetsInv->size(); ibjet3++){
    ////    	    if(jetsInv->at(ijet1) == bJetsInv->at(ibjet3) || ibjet2 == ibjet3) continue;
    ////    	    for( int ibjet4 = ibjet3+1; ibjet4 < bJetsInv->size(); ibjet4++){
    ////    		if(jetsInv->at(ijet1) == bJetsInv->at(ibjet4) || ibjet2 == ibjet4 || ibjet3 == ibjet4 ) continue;		
    ////    		diMotherReco(*jetsInv->at(ijet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
    ////    			     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
    ////    			     , cHiggsMass, cHiggsMass, _minChi2Higgs, _bbMassMin1Higgs, _bbMassMin2Higgs);
    ////    		diMotherReco(*jetsInv->at(ijet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
    ////    			     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
    ////    			     , cZMass, cZMass, _minChi2Z, _bbMassMin1Z, _bbMassMin2Z);
    ////    		// ZH combinatorics 
    ////    		diMotherReco(*jetsInv->at(ijet1)->getp4(), *bJetsInv->at(ibjet2)->getp4() // ZH H(jb)Z(bb)
    ////    			     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
    ////    			     , cHiggsMass, cZMass, _minChi2HiggsZ, _bbMassMin1HiggsZ, _bbMassMin2HiggsZ);
    ////    		tempminChi2 = _minChi2HiggsZ; tmpMassMin1HiggsZ = _bbMassMin1HiggsZ; tmpMassMin2HiggsZ = _bbMassMin2HiggsZ;
    ////    		diMotherReco(*jetsInv->at(ijet1)->getp4(), *bJetsInv->at(ibjet2)->getp4() // ZH Z(jb)H(bb)
    ////    			     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
    ////    			     , cZMass, cHiggsMass, _minChi2HiggsZ, _bbMassMin2HiggsZ, _bbMassMin1HiggsZ);
    ////    		// pick the lowest minChi2 for the two combinatorics
    ////    		if(tempminChi2 < _minChi2HiggsZ){
    ////    		    _minChi2HiggsZ = tempminChi2; _bbMassMin1HiggsZ = tmpMassMin1HiggsZ; _bbMassMin2HiggsZ = tmpMassMin2HiggsZ;
    ////    		}
    ////    	    }
    ////    	}
    ////        }
    ////    }
    ////}
    

    //    std::map<std::string, float> testVars = HypoComb->GetBestPermutation(getLepP4(thisEvent),getJetP4(thisEvent),getJetCSV(thisEvent),*(thisEvent->getMET()->getp4()));
    //    HypoComb.GetBestPermutation(getLepP4(thisEvent),getJetP4(thisEvent),getJetCSV(thisEvent),*(thisEvent->getMET()->getp4()));
    //    std::cout<< "BLR: " << testVars["Evt_blr"] << std::endl;

    //thisEvent->eventShapeJet  = new EventShape(vectorsJet);
    //thisEvent->eventShapeBjet  = new EventShape(vectorsBjet);
}


void ttHHanalyzer_unified::process(event* thisEvent, sysName sysType, bool up){

    // 1) Golden JSON filter for data
    _failGoldenJson = false;
    if (_DataOrMC == "Data") {
        int run  = _ev->run;
        int lumi = _ev->luminosityBlock;
        if (!corrMgr->passGoldenJSON(run, lumi)) {
            if (debugCorrections) {
                std::cout << "[GoldenJSON] Skipping event: run=" 
                          << run << " lumi=" << lumi << std::endl;
            }
            _failGoldenJson = true;
            return;
        }
        if (debugCorrections) {
            std::cout << "[GoldenJSON] Accepted event: run=" 
                      << run << " lumi=" << lumi << std::endl;
        }
    }

    // [tt+nb] per-event default. Overwritten in the MC block below for tt+nb
    // hits. Data has no genTtbarId branch, so leave -1.
    _genTtbarIdNano  = -1;
    _expandedTtbarId = -1;

    if (debugCorrections) {
	std::cout << "[process] : Current evtWeight="<< _evtWeight << std::endl;
    }

    _PUWeight = 1.0;
    // 2) Pileup reweighting for MC
    if (_DataOrMC != "Data") {
        float nTrue = _ev->Pileup_nTrueInt;
        _PUWeight = corrMgr->getPUWeight(nTrue, "nominal");
        if (debugCorrections) {
            std::cout << "[PU] nTrue=" << nTrue 
                      << " weight=" << _PUWeight;
        }
    }

    // 3) L1 pre-firing weight
    _L1PrefiringWeight = 1.0;
    if (_DataOrMC != "Data") {
	_L1PrefiringWeight = _ev->L1PreFiringWeight_Nom;

        if (debugCorrections) {
	    std::cout << "[L1PreFiring] weight=" << _L1PrefiringWeight <<std::endl;
	}
    }


    // 4) gen weight
    _genWeight = 1.0;
    if (_DataOrMC != "Data") {
        _genWeight = _ev->genWeight;
	if (debugCorrections) {
            std::cout << "[genWeight] weight=" << _genWeight <<std::endl;
        }
    }

    // 5) Build physics objects in thisEvent from the raw buffer
    // Event cleaning, Trigger set (Here, event are not rejected, just define filter, trig, etc...
    // and object definitions
    // ═══════════════════════════════════════════════════════════════════════
    // Step: 물리 객체 생성 (JEC/JER 적용됨)
    // ═══════════════════════════════════════════════════════════════════════
    createObjects(thisEvent, sysType, up);

    // 6) Calculate Event Weight
    _evtWeight = _baseWeight; // base weights are got from json config
    if (_DataOrMC != "Data") {
	_evtWeight *= _PUWeight;
	_evtWeight *= _L1PrefiringWeight;
	_evtWeight *= _genWeight;
	////_evtWeight *= bTagWeight_central_;
        if(debugCorrections) std::cout << "Final Event Weight: " << _evtWeight << std::endl;
    }

    // ═══════════════════════════════════════════════════════════════════════
    // Step: B-tag Weight 계산 (object 단계, selection 전)
    // JEC/JER이 적용된 jet kinematics를 사용
    // ═══════════════════════════════════════════════════════════════════════
    computeBTagWeight(thisEvent);

    // ═══════════════════════════════════════════════════════════════════════
    // tt+jets categorization 4-way validation (runs on ALL MC events,
    // pre-selection).
    //
    // Four estimators of the same per-event label, all computed and
    // compared:
    //
    //   ANA_GENPART  analyzer's own GenPart algorithm
    //                (computeTtCategoryFromGenPart)
    //   ANA_GENID    analyzer's decode of genTtbarId%100
    //                (computeTtCategoryFromGenTtbarId)
    //   NTU_PRIMARY  ntuple ttCat_*       (= ntuplizer's POG path)
    //                (readNtuplePrimaryCategory)
    //   NTU_XVAL     ntuple ttCatXval_*   (= ntuplizer's GenPart path)
    //                (readNtupleXvalCategory)
    //
    // Expected:
    //   ANA_GENID  ≡  NTU_PRIMARY    (both decode the same integer)
    //   ANA_GENPART ≡  NTU_XVAL      (same algorithm in two languages)
    //   ANA_GENID vs ANA_GENPART     ~97% (POG vs GenPart, real diff)
    //
    // Downstream physics never uses ANA_*; it uses officialTtCategory()
    // which reads NTU_PRIMARY by project convention.
    // ═══════════════════════════════════════════════════════════════════════
    if (_DataOrMC != "Data") {

        // ── Branch availability check (first event only) ──
        static bool branchCheckDone = false;
        if (!branchCheckDone) {
            branchCheckDone = true;
            std::cout << "\n[TTCAT_BRANCH_CHECK] ═══════════════════════════════\n";
            std::cout << "  GenPart_pdgId.size()             = " << _ev->GenPart_pdgId.size() << "\n";
            std::cout << "  GenPart_statusFlags.size()       = " << _ev->GenPart_statusFlags.size() << "\n";
            std::cout << "  GenPart_genPartIdxMother.size()  = " << _ev->GenPart_genPartIdxMother.size() << "\n";
            std::cout << "  GenPart_eta.size()               = " << _ev->GenPart_eta.size() << "\n";
            std::cout << "  GenPart_phi.size()               = " << _ev->GenPart_phi.size() << "\n";
            std::cout << "  GenJet_pt.size()                 = " << _ev->GenJet_pt.size() << "\n";
            std::cout << "  GenJet_eta.size()                = " << _ev->GenJet_eta.size() << "\n";
            std::cout << "  GenJet_phi.size()                = " << _ev->GenJet_phi.size() << "\n";
            std::cout << "  GenJet_hadronFlavour.size()      = " << _ev->GenJet_hadronFlavour.size() << "\n";
            std::cout << "  genTtbarId                       = " << _ev->genTtbarId << "\n";
            std::cout << "  ttCatSource (ntuple primary)     = " << _ev->ttCatSource << "\n";
            std::cout << "  ttCatXvalSource (ntuple xval)    = " << _ev->ttCatXvalSource << "\n";
            std::cout << "  ttCat_LightFlavour (ntuple)      = " << _ev->ttCat_LightFlavour << "\n";
            std::cout << "  ttCat_AddCjet (ntuple)           = " << _ev->ttCat_AddCjet << "\n";
            std::cout << "  ttCat_Add1Bjet_1Had (ntuple)     = " << _ev->ttCat_Add1Bjet_1Had << "\n";
            std::cout << "  ttCat_Add1Bjet_2Had (ntuple)     = " << _ev->ttCat_Add1Bjet_2Had << "\n";
            std::cout << "  ttCat_Add2Bjet (ntuple)          = " << _ev->ttCat_Add2Bjet << "\n";
            if (_ev->GenPart_statusFlags.size() == 0) {
                std::cout << "  *** WARNING: GenPart_statusFlags is EMPTY! ***\n"
                          << "  *** isLastCopy cannot be checked -> all events will fall to LightFlavour ***\n";
            }
            if (_ev->GenPart_genPartIdxMother.size() == 0) {
                std::cout << "  *** WARNING: GenPart_genPartIdxMother is EMPTY! ***\n"
                          << "  *** Top ancestor check cannot work. ***\n";
            }
            if (_ev->GenJet_hadronFlavour.size() == 0) {
                std::cout << "  *** WARNING: GenJet_hadronFlavour is EMPTY! ***\n"
                          << "  *** No b-jets can be identified. ***\n";
            }
            std::cout << "[TTCAT_BRANCH_CHECK] ═══════════════════════════════\n\n";
        }

        // ── [tt+nb] attach extended ttbar-Id from the per-sample lookup ──
        //   in lookup -> Expanded_genTtbarId (sub-code 61/62/71/72)
        //   not in    -> NanoAOD genTtbarId unchanged
        // resolve() also self-checks the lookup's genTtbarId against NanoAOD's
        // for this (run,lumi,event); a mismatch means the wrong ttnb file.
        _genTtbarIdNano  = _ev->genTtbarId;
        _expandedTtbarId = _expTtbarId.resolve(_ev->run, _ev->luminosityBlock,
                                               _ev->event, _ev->genTtbarId);

        // ── [stitch] per-(sample,category) MULTIPLIER on top of the YAML base ──
        // Applied HERE — after expandedTtbarId is resolved, before selectObjects
        // seeds the SF chains — so BOTH the main analysis and the btagtrig
        // reweight-derivation see the stitched MC composition.
        //   category = sub_to_category[expandedTtbarId % 100]
        //   inclusive : 1 on kept HF cats / 0 on rejected (filled by a dedicated)
        //   dedicated : r on owned cats / 0 elsewhere
        //   not in plan: 1 (untouched).  A 0 drops the event from THIS sample.
        // See ttbarCategorization.md s10 (stitch) / s11 (this application).
        if (_stitch.inPlan()) {
            const double stitchMult = _stitch.factor(_expandedTtbarId, _evtWeight);
            _evtWeight *= stitchMult;
            if (debugCorrections) {
                std::cout << "[stitch] sample=" << _sampleName
                          << " expandedTtbarId=" << _expandedTtbarId
                          << " cat=" << _stitch.category(_expandedTtbarId)
                          << " mult=" << stitchMult
                          << " -> _evtWeight=" << _evtWeight << std::endl;
            }
        }

        // ── Compute all four estimators ──
        const TtCat anaGenPart = computeTtCategoryFromGenPart();
        const TtCat anaGenId   = computeTtCategoryFromGenTtbarId();
        const TtCat ntuPrim    = readNtuplePrimaryCategory();
        const TtCat ntuXval    = readNtupleXvalCategory();

        const int anaGenPartIdx = static_cast<int>(anaGenPart);
        const int anaGenIdIdx   = static_cast<int>(anaGenId);
        const int ntuPrimIdx    = static_cast<int>(ntuPrim);
        const int ntuXvalIdx    = static_cast<int>(ntuXval);

        // Counters for the two "must agree" pairs
        const bool agreeAnaGenIdVsNtuPrim   = (anaGenIdIdx == ntuPrimIdx);
        const bool agreeAnaGenPartVsNtuXval = (anaGenPartIdx == ntuXvalIdx);

        // Did anything disagree at all?
        const bool anyDisagreement =
            (anaGenPartIdx != anaGenIdIdx) ||
            (anaGenPartIdx != ntuPrimIdx)  ||
            (anaGenPartIdx != ntuXvalIdx)  ||
            (anaGenIdIdx   != ntuPrimIdx)  ||
            (anaGenIdIdx   != ntuXvalIdx)  ||
            (ntuPrimIdx    != ntuXvalIdx);

        // ── Per-event debug print ──
        // First 20 events always; events 20–100 only if a "must agree"
        // pair disagrees, or if the event lands in a heavy-flavour
        // category (which is the interesting working point for ttHH).
        static int dbgCount = 0;
        const bool isHeavyFlav = (anaGenPart == TtCat::kAdd1Bjet1Had ||
                                  anaGenPart == TtCat::kAdd1Bjet2Had ||
                                  anaGenPart == TtCat::kAdd2Bjet);
        const bool mustAgreeBroken =
            !agreeAnaGenIdVsNtuPrim || !agreeAnaGenPartVsNtuXval;
        const bool shouldPrint =
            (dbgCount < 5) ||
            (dbgCount < 10 && (mustAgreeBroken || isHeavyFlav));

        if (shouldPrint) {
            std::cout << "\n===== [ttCatDebug] Event #" << dbgCount
                      << " (run:lumi:evt = " << _ev->run << ":"
                      << _ev->luminosityBlock << ":" << _ev->event
                      << ") =====\n";

            // 1) Sizes
            std::cout << "  [Sizes] GenPart=" << _ev->GenPart_pdgId.size()
                      << " GenJet=" << _ev->GenJet_pt.size()
                      << " GenJet_hadFlav=" << _ev->GenJet_hadronFlavour.size() << "\n";

            // 2) genTtbarId
            std::cout << "  [genTtbarId] " << _ev->genTtbarId
                      << " (mod100=" << (_ev->genTtbarId % 100) << ")\n";

            // 3) ntuple ttCat_* booleans (primary + xval)
            std::cout << "  [ntuple ttCat_* primary] LF=" << _ev->ttCat_LightFlavour
                      << " Cj=" << _ev->ttCat_AddCjet
                      << " 1B1H=" << _ev->ttCat_Add1Bjet_1Had
                      << " 1B2H=" << _ev->ttCat_Add1Bjet_2Had
                      << " 2B+=" << _ev->ttCat_Add2Bjet
                      << " src=" << _ev->ttCatSource << "\n";
            std::cout << "  [ntuple ttCatXval_*]      LF=" << _ev->ttCatXval_LightFlavour
                      << " Cj=" << _ev->ttCatXval_AddCjet
                      << " 1B1H=" << _ev->ttCatXval_Add1Bjet_1Had
                      << " 1B2H=" << _ev->ttCatXval_Add1Bjet_2Had
                      << " 2B+=" << _ev->ttCatXval_Add2Bjet
                      << " src=" << _ev->ttCatXvalSource << "\n";

            // 4) tt pair
            const bool hasTT = eventHasTTPair();
            std::cout << "  [analyzer GenPart] hasTTPair=" << hasTT << "\n";

            // 5) Per-jet B hadron map (from analyzer's algorithm)
            if (hasTT && genPartCount() > 0 && genJetCount() > 0) {
                const auto result = countAdditionalBJetsDetailed();

                // First 5 events: per-particle dump of additional B hadrons
                if (dbgCount < 5) {
                    int bhIdx = 0;
                    for (int i = 0; i < genPartCount(); ++i) {
                        if (!isBHadron(_ev->GenPart_pdgId[i])) continue;
                        if (!((_ev->GenPart_statusFlags[i] >> 13) & 1)) continue;
                        if (hasTopAncestor(i)) continue;
                        std::cout << "    BH#" << bhIdx++
                                  << " GP_idx=" << i
                                  << " pdgId=" << _ev->GenPart_pdgId[i]
                                  << " eta=" << _ev->GenPart_eta[i]
                                  << " phi=" << _ev->GenPart_phi[i] << "\n";
                    }
                }

                std::cout << "  [analyzer GenPart] addBJets=" << result.nJets
                          << " addBH(total)=" << result.nHadrons
                          << " addBH(matched)=" << result.nMatched
                          << " addCJets=" << analyzerAdditionalCJetCount() << "\n";

                if (!result.jetBHMap.empty()) {
                    // Sort for deterministic output
                    std::map<int,int> sortedMap(result.jetBHMap.begin(),
                                                result.jetBHMap.end());
                    std::cout << "  [per-jet BH map]";
                    for (const auto& [jetIdx, bhCount] : sortedMap) {
                        std::cout << " GenJet#" << jetIdx
                                  << "(pT=" << _ev->GenJet_pt[jetIdx]
                                  << ",eta=" << _ev->GenJet_eta[jetIdx]
                                  << "):" << bhCount << "BH";
                    }
                    std::cout << "\n";

                    // 1Bjet → 1Had vs 2Had decision
                    if (result.nJets == 1) {
                        const int singleJetBH = sortedMap.begin()->second;
                        std::cout << "  [1Bjet split] single jet has "
                                  << singleJetBH << " matched BH -> "
                                  << (singleJetBH >= 2 ? "Add1Bjet_2Had" : "Add1Bjet_1Had")
                                  << "\n";
                    }
                }
            }

            // 6) The four estimators side by side
            auto fmt = [](TtCat c) {
                return std::string(ttCatShortName(static_cast<int>(c)));
            };
            std::cout << "  [4-way label] "
                      << "ANA_GENPART=" << fmt(anaGenPart)
                      << "  ANA_GENID=" << fmt(anaGenId)
                      << "  NTU_PRIMARY=" << fmt(ntuPrim)
                      << "  NTU_XVAL=" << fmt(ntuXval)
                      << "\n";

            // 7) Mismatch flags
            if (!agreeAnaGenIdVsNtuPrim) {
                std::cout << "  *** MUST-AGREE BROKEN: ANA_GENID != NTU_PRIMARY"
                          << " (both decode the same int!) ***\n";
            }
            if (!agreeAnaGenPartVsNtuXval) {
                std::cout << "  *** MUST-AGREE BROKEN: ANA_GENPART != NTU_XVAL"
                          << " (same algorithm, two languages!) ***\n";
            }
            if (anaGenPartIdx != anaGenIdIdx) {
                std::cout << "  [info] ANA_GENPART != ANA_GENID"
                          << " (POG vs GenPart, real algorithmic diff)\n";
            }
            if (!anyDisagreement) {
                std::cout << "  [4-way] ALL FOUR AGREE\n";
            }

            ++dbgCount;
        }

        // ── Fill 1D count histograms ──
        _hTtCat_Counts_AnaGenPart->Fill(anaGenPartIdx + 0.5);
        _hTtCat_Counts_AnaGenId  ->Fill(anaGenIdIdx   + 0.5);
        _hTtCat_Counts_NtuPrimary->Fill(ntuPrimIdx    + 0.5);
        _hTtCat_Counts_NtuXval   ->Fill(ntuXvalIdx    + 0.5);

        // ── Fill 2D pair-wise comparisons (6 pairs) ──
        _hTtCat_AnaGenPart_vs_AnaGenId   ->Fill(anaGenPartIdx + 0.5, anaGenIdIdx + 0.5);
        _hTtCat_AnaGenPart_vs_NtuPrimary ->Fill(anaGenPartIdx + 0.5, ntuPrimIdx  + 0.5);
        _hTtCat_AnaGenPart_vs_NtuXval    ->Fill(anaGenPartIdx + 0.5, ntuXvalIdx  + 0.5);
        _hTtCat_AnaGenId_vs_NtuPrimary   ->Fill(anaGenIdIdx   + 0.5, ntuPrimIdx  + 0.5);
        _hTtCat_AnaGenId_vs_NtuXval      ->Fill(anaGenIdIdx   + 0.5, ntuXvalIdx  + 0.5);
        _hTtCat_NtuPrimary_vs_NtuXval    ->Fill(ntuPrimIdx    + 0.5, ntuXvalIdx  + 0.5);

        // ── genTtbarId mod 100 vs analyzer GenPart category ──
        const int gtidMod = _ev->genTtbarId % 100;
        _hTtCat_GenTtbarIdMod100->Fill(gtidMod, anaGenPartIdx + 0.5);
    }

    // 7) Baseline selection + event cleaning, apply trigger path, reconstruct higher objects
    if(!selectObjects(thisEvent)) return;
    _passMETFilters = false;
    _passMETFilters = thisEvent->getMETFilter();
    //if(_passMETFilters != true){
    //    std::cout<<"ERROR : MET filter class does not work properly.. It should reject all of events are not passed filters in [ selectObjects ].."<<std::endl;
    //    exit(555);
    //}

    // 5) Final analysis steps: kinematics, histograms, tree
    analyze(thisEvent);
    fillHistos(thisEvent);
    fillTree(thisEvent);

}


void ttHHanalyzer_unified::fillHistos(event * thisEvent){

    //    for(int i=0; i < cutflow.size(); i++){
    //	std::cout << "cutflow size: " << cutflow[i] << std::endl; 
	//	hCutFlow->SetBinContent(1, cutflow[0]);
    //    }
    
////    int i = 0; 
////    for (const auto& x : cutflow){
////	//hCutFlow->SetBinContent(i, x.second);
////	i++;
////    }
    
   ////// thisEvent->getCentrality(thisEvent->getSelJets(), thisEvent->getSelbJets(), jbjetCent);
   ////// thisEvent->getCentrality(thisEvent->getSelJets(), thisEvent->getSelLeptons(), jlepCent);
   ////// thisEvent->getStats(thisEvent->getSelJets(), jetStat);
   ////// thisEvent->getStats(thisEvent->getSelbJets(), bjetStat);
   ////// thisEvent->getStatsComb(thisEvent->getSelJets(), thisEvent->getSelbJets(), bjStat);
   ////// thisEvent->getMaxPTComb(thisEvent->getSelJets(), thisEvent->getSelbJets(), jbbMaxs);
   ////// thisEvent->getMaxPTSame(thisEvent->getSelJets(), jjjMaxs);

   ////// thisEvent->getStatsComb(thisEvent->getSelJets(), thisEvent->getSelLeptons(), ljetStat);
   ////// thisEvent->getStatsComb(thisEvent->getSelbJets(), thisEvent->getSelLeptons(), lbjetStat);
   ////// 
   ////// thisEvent->getFoxWolfram(thisEvent->getSelJets(), jetFoxWolfMom);
   ////// thisEvent->getFoxWolfram(thisEvent->getSelbJets(), bjetFoxWolfMom);

   ////// //    std::cout << "Number of Hadronic Higgs: " << thisEvent->getnHadronicHiggs() << std::endl;

   ////// hjetNumber->Fill(thisEvent->getnSelJet(),_weight*thisEvent->getbTagSys());
   ////// hHadronicHiggsNumber->Fill(thisEvent->getnHadronicHiggs(),_weight*thisEvent->getbTagSys());
   ////// hBjetNumber->Fill(thisEvent->getnSelbJet(),_weight*thisEvent->getbTagSys());
   ////// hLightJetNumber->Fill(thisEvent->getnLightJet(),_weight*thisEvent->getbTagSys());

   ////// hjetAverageMass->Fill(thisEvent->getSumSelJetMass()/(float)thisEvent->getnSelJet(),_weight*thisEvent->getbTagSys());
   ////// hHadronicHiggsAverageMass->Fill(thisEvent->getSumSelHadronicHiggsMass()/(float)thisEvent->getnHadronicHiggs(),_weight*thisEvent->getbTagSys());
   ////// hBjetAverageMass->Fill(thisEvent->getSumSelbJetMass()/(float)thisEvent->getnSelbJet(),_weight*thisEvent->getbTagSys());
   ////// hLightJetAverageMass->Fill(thisEvent->getSumSelLightJetMass()/(float)thisEvent->getnLightJet(),_weight*thisEvent->getbTagSys());
   ////// hBjetAverageMassSqr->Fill((thisEvent->getSumSelbJetMass()*thisEvent->getSumSelbJetMass())/(float)thisEvent->getnSelbJet(), _weight*thisEvent->getbTagSys());

   ////// if(thisEvent->getnHadronicHiggs() > 0){ 
   //////     hHadronicHiggsSoftDropMass1->Fill(thisEvent->getSelHadronicHiggses()->at(0)->softDropMass,_weight*thisEvent->getbTagSys());
   ////// }

   ////// if(thisEvent->getnHadronicHiggs() > 1){ 
   //////     hHadronicHiggsSoftDropMass2->Fill(thisEvent->getSelHadronicHiggses()->at(1)->softDropMass,_weight*thisEvent->getbTagSys());
   ////// }


   ////// hjetHT->Fill(thisEvent->getSumSelJetScalarpT(),_weight*thisEvent->getbTagSys());
   ////// hBjetHT->Fill(thisEvent->getSumSelbJetScalarpT(),_weight*thisEvent->getbTagSys());
   ////// hHadronicHiggsHT->Fill(thisEvent->getSumSelHadronicHiggsScalarpT(),_weight*thisEvent->getbTagSys());
   ////// hLightJetHT->Fill(thisEvent->getSumSelLightJetScalarpT(),_weight*thisEvent->getbTagSys());
   ////// hAvgDeltaRjj->Fill(jetStat.meandR,_weight*thisEvent->getbTagSys());
   ////// hAvgDeltaEtajj->Fill(jetStat.meandEta,_weight*thisEvent->getbTagSys());
   ////// hminDeltaRjj->Fill(jetStat.mindR,_weight*thisEvent->getbTagSys());
   ////// hminDeltaRMassjj->Fill(jetStat.mindRMass,_weight*thisEvent->getbTagSys());
   ////// hminDeltaRpTjj->Fill(jetStat.mindRpT,_weight*thisEvent->getbTagSys());
   ////// hAvgDeltaRbb->Fill(bjetStat.meandR,_weight*thisEvent->getbTagSys());
   ////// hAvgDeltaRbj->Fill(bjStat.meandR,_weight*thisEvent->getbTagSys());
   ////// hAvgDeltaEtabj->Fill(bjStat.meandEta,_weight*thisEvent->getbTagSys());
   ////// hminDeltaRbj->Fill(bjStat.mindR,_weight*thisEvent->getbTagSys());
   ////// hminDeltaRMassbj->Fill(bjStat.mindRMass,_weight*thisEvent->getbTagSys());
   ////// hminDeltaRpTbj->Fill(bjStat.mindRpT,_weight*thisEvent->getbTagSys());    
   ////// hAvgDeltaEtabb->Fill(bjetStat.meandEta,_weight*thisEvent->getbTagSys());
   ////// hminDeltaRbb->Fill(bjetStat.mindR,_weight*thisEvent->getbTagSys());
   ////// hminDeltaRMassbb->Fill(bjetStat.mindRMass,_weight*thisEvent->getbTagSys());
   ////// hminDeltaRpTbb->Fill(bjetStat.mindRpT,_weight*thisEvent->getbTagSys());
   ////// hmaxDeltaEtabb->Fill(bjetStat.maxdEta,_weight*thisEvent->getbTagSys());
   ////// hmaxDeltaEtajj->Fill(jetStat.maxdEta,_weight*thisEvent->getbTagSys());
   ////// hAvgDeltaEtabj->Fill(bjStat.meandEta,_weight*thisEvent->getbTagSys());
   ////// hmaxDeltaEtabj->Fill(bjStat.maxdEta,_weight*thisEvent->getbTagSys());
   ////// hmaxPTmassjbb->Fill(jbbMaxs.maxPTmass, _weight*thisEvent->getbTagSys());
   ////// hmaxPTmassjjj->Fill(jjjMaxs.maxPTmass, _weight*thisEvent->getbTagSys());

   ////// hInvMassHSingleMatched->Fill(_bbMassMinSHiggsMatched,_weight*thisEvent->getbTagSys());
   ////// hInvMassHSingleNotMatched->Fill(_bbMassMinSHiggsNotMatched,_weight*thisEvent->getbTagSys());
   ////// hChi2HiggsSingleMatched->Fill(_minChi2SHiggsMatched,_weight*thisEvent->getbTagSys());
   ////// hChi2HiggsSingleNotMatched->Fill(_minChi2SHiggsNotMatched,_weight*thisEvent->getbTagSys());

   ////// hInvMassHH1Matched->Fill(_bbMassMinHH1Matched,_weight*thisEvent->getbTagSys());
   ////// hInvMassHH1NotMatched->Fill(_bbMassMinHH1NotMatched,_weight*thisEvent->getbTagSys());
   ////// hInvMassHH2Matched->Fill(_bbMassMinHH2Matched,_weight*thisEvent->getbTagSys());
   ////// hInvMassHH2NotMatched->Fill(_bbMassMinHH2NotMatched,_weight*thisEvent->getbTagSys());
   ////// hChi2HHMatched->Fill(_minChi2HHMatched,_weight*thisEvent->getbTagSys());
   ////// hChi2HHNotMatched->Fill(_minChi2HHNotMatched,_weight*thisEvent->getbTagSys());


   ////// hInvMassH1->Fill(_bbMassMin1Higgs,_weight*thisEvent->getbTagSys());
   ////// hInvMassH2->Fill(_bbMassMin2Higgs,_weight*thisEvent->getbTagSys());
   ////// hInvMassH1_zoomIn->Fill(_bbMassMin1Higgs,_weight*thisEvent->getbTagSys());
   ////// hInvMassH2_zoomIn->Fill(_bbMassMin2Higgs,_weight*thisEvent->getbTagSys());
   ////// hPTH1->Fill(_bpTHiggs1,_weight*thisEvent->getbTagSys());
   ////// hPTH2->Fill(_bpTHiggs2,_weight*thisEvent->getbTagSys());
   ////// hInvMassZ1->Fill(_bbMassMin1Z,_weight*thisEvent->getbTagSys());
   ////// hInvMassZ2->Fill(_bbMassMin2Z,_weight*thisEvent->getbTagSys());
   ////// hInvMassZ1_zoomIn->Fill(_bbMassMin1Z,_weight*thisEvent->getbTagSys());
   ////// hInvMassZ2_zoomIn->Fill(_bbMassMin2Z,_weight*thisEvent->getbTagSys());
   ////// hInvMassHZ1->Fill(_bbMassMin1HiggsZ,_weight*thisEvent->getbTagSys());
   ////// hInvMassHZ2->Fill(_bbMassMin2HiggsZ,_weight*thisEvent->getbTagSys());
   ////// hInvMassHZ1_zoomIn->Fill(_bbMassMin1HiggsZ,_weight*thisEvent->getbTagSys());
   ////// hInvMassHZ2_zoomIn->Fill(_bbMassMin2HiggsZ,_weight*thisEvent->getbTagSys());

   ////// if(fabs(_bbMassMin1Higgs-cHiggsMass) < fabs(_bbMassMin2Higgs-cHiggsMass)){
   //////     hInvMassH1mChi->Fill(_bbMassMin1Higgs,_weight*thisEvent->getbTagSys());
   //////     hInvMassH2mChi->Fill(_bbMassMin2Higgs,_weight*thisEvent->getbTagSys());
   ////// } else  {
   //////     hInvMassH2mChi->Fill(_bbMassMin1Higgs,_weight*thisEvent->getbTagSys());
   //////     hInvMassH1mChi->Fill(_bbMassMin2Higgs,_weight*thisEvent->getbTagSys());
   ////// }
   ////// hChi2Higgs->Fill(_minChi2Higgs,_weight*thisEvent->getbTagSys());
   ////// hChi2Z->Fill(_minChi2Z,_weight*thisEvent->getbTagSys());
   ////// hChi2HiggsZ->Fill(_minChi2HiggsZ,_weight*thisEvent->getbTagSys());

   ////// 
   ////// hmet->Fill(thisEvent->getMET()->getp4()->Pt(),_weight*thisEvent->getbTagSys());
   ////// //    hmetPhi->Fill(thisEvent->getMET()->getp4()->Phi(),_weight*thisEvent->getbTagSys());
   ////// // hmetEta->Fill(thisEvent->getMET()->getp4()->Eta(),_weight*thisEvent->getbTagSys());
   ////// 
   ////// hAplanarity->Fill(thisEvent->eventShapeJet->getAplanarity(), _weight*thisEvent->getbTagSys());
   ////// hSphericity->Fill(thisEvent->eventShapeJet->getSphericity(), _weight*thisEvent->getbTagSys());
   ////// hTransSphericity->Fill(thisEvent->eventShapeJet->getTransSphericity(), _weight*thisEvent->getbTagSys());
   ////// hCvalue->Fill(thisEvent->eventShapeJet->getC(), _weight*thisEvent->getbTagSys());
   ////// hDvalue->Fill(thisEvent->eventShapeJet->getD(), _weight*thisEvent->getbTagSys());
   ////// hCentralityjb->Fill(jbjetCent.centrality, _weight*thisEvent->getbTagSys());    
   ////// hCentralityjl->Fill(jlepCent.centrality, _weight*thisEvent->getbTagSys());    

   ////// hH0->Fill(jetFoxWolfMom.h0, _weight*thisEvent->getbTagSys());
   ////// hH1->Fill(jetFoxWolfMom.h1, _weight*thisEvent->getbTagSys());
   ////// hH2->Fill(jetFoxWolfMom.h2, _weight*thisEvent->getbTagSys());
   ////// hH3->Fill(jetFoxWolfMom.h3, _weight*thisEvent->getbTagSys());
   ////// hH4->Fill(jetFoxWolfMom.h4, _weight*thisEvent->getbTagSys());
   ////// hR1->Fill(jetFoxWolfMom.r1, _weight*thisEvent->getbTagSys());
   ////// hR2->Fill(jetFoxWolfMom.r2, _weight*thisEvent->getbTagSys());
   ////// hR3->Fill(jetFoxWolfMom.r3, _weight*thisEvent->getbTagSys());
   ////// hR4->Fill(jetFoxWolfMom.r4, _weight*thisEvent->getbTagSys()); 


   ////// hBjetH0->Fill(bjetFoxWolfMom.h0, _weight*thisEvent->getbTagSys());
   ////// hBjetH1->Fill(bjetFoxWolfMom.h1, _weight*thisEvent->getbTagSys());
   ////// hBjetH2->Fill(bjetFoxWolfMom.h2, _weight*thisEvent->getbTagSys());
   ////// hBjetH3->Fill(bjetFoxWolfMom.h3, _weight*thisEvent->getbTagSys());
   ////// hBjetH4->Fill(bjetFoxWolfMom.h4, _weight*thisEvent->getbTagSys());
   ////// hBjetR1->Fill(bjetFoxWolfMom.r1, _weight*thisEvent->getbTagSys());
   ////// hBjetR2->Fill(bjetFoxWolfMom.r2, _weight*thisEvent->getbTagSys());
   ////// hBjetR3->Fill(bjetFoxWolfMom.r3, _weight*thisEvent->getbTagSys());
   ////// hBjetR4->Fill(bjetFoxWolfMom.r4, _weight*thisEvent->getbTagSys()); 


   ////// hBjetAplanarity->Fill(thisEvent->eventShapeBjet->getAplanarity(), _weight*thisEvent->getbTagSys());
   ////// hBjetSphericity->Fill(thisEvent->eventShapeBjet->getSphericity(), _weight*thisEvent->getbTagSys());
   ////// hBjetTransSphericity->Fill(thisEvent->eventShapeBjet->getTransSphericity(), _weight*thisEvent->getbTagSys());
   ////// hBjetCvalue->Fill(thisEvent->eventShapeBjet->getC(), _weight*thisEvent->getbTagSys());
   ////// hBjetDvalue->Fill(thisEvent->eventShapeBjet->getD(), _weight*thisEvent->getbTagSys());


   ////// for(int ih=0; ih < thisEvent->getnSelJet() && ih < nHistsJets; ih++){
   //////     hjetsPTs.at(ih)->Fill(thisEvent->getSelJets()->at(ih)->getp4()->Pt(),_weight*thisEvent->getbTagSys());
   //////     hjetsEtas.at(ih)->Fill(thisEvent->getSelJets()->at(ih)->getp4()->Eta(),_weight*thisEvent->getbTagSys());
   //////     hjetsBTagDisc.at(ih)->Fill(getJetCSV(thisEvent).at(ih),_weight*thisEvent->getbTagSys());
   ////// }
   int nSel = thisEvent->getnSelJet();
   for(int ih = 0; ih < nSel && ih < nHistsJets; ++ih){
     float JEC_DiffRatio = thisEvent->getSelJets()->at(ih)->JEC_DiffRatio;   // nanoAOD JEC 적용 후 pT
     float Mass_DiffRatio = thisEvent->getSelJets()->at(ih)->mass_DiffRatio;

     // [ 직접 JEC 해체 후 최신 버전 재적용한 JEC - NanoAOD orinigal Pt (NanoAOD의 기본 JEC) ] / [ NanoAOD original Pt ]
     h_JEC_DiffRatio.at(ih)->Fill(JEC_DiffRatio, _evtWeight);
     h_JEC_Mass_DiffRatio.at(ih)->Fill(Mass_DiffRatio, _evtWeight);
   }
  
   ////// for(int ih=0; ih < thisEvent->getnSelbJet() && ih < nHistsbJets; ih++){
   //////     hbjetsPTs.at(ih)->Fill(thisEvent->getSelbJets()->at(ih)->getp4()->Pt(),_evtWeight*thisEvent->getbTagSys());
   //////     hbjetsEtas.at(ih)->Fill(thisEvent->getSelbJets()->at(ih)->getp4()->Eta(),_evtWeight*thisEvent->getbTagSys());
   //////     hbjetsBTagDisc.at(ih)->Fill(getbJetCSV(thisEvent).at(ih),_evtWeight*thisEvent->getbTagSys());
   ////// }


   ////// for(int ih=0; ih < thisEvent->getnLightJet() && ih < nHistsLightJets; ih++){
   //////     hLightJetsPTs.at(ih)->Fill(thisEvent->getSelLightJets()->at(ih)->getp4()->Pt(),_evtWeight*thisEvent->getbTagSys());
   //////     hLightJetsEtas.at(ih)->Fill(thisEvent->getSelLightJets()->at(ih)->getp4()->Eta(),_evtWeight*thisEvent->getbTagSys());
   //////     hLightJetsBTagDisc.at(ih)->Fill(getlightJetCSV(thisEvent).at(ih),_evtWeight*thisEvent->getbTagSys());
   ////// }

    /*    hleptonNumber->Fill(thisEvent->getnSelLepton(),_evtWeight*thisEvent->getbTagSys());


    if(thisEvent->getnSelMuon() == 2){
	hDiMuonMass->Fill(thisEvent->getSelMuonsMass(),_evtWeight*thisEvent->getbTagSys());
	hDiMuonPT->Fill(thisEvent->getSelMuonsPT(),_evtWeight*thisEvent->getbTagSys());
	hDiMuonEta->Fill(thisEvent->getSelMuonsEta(),_evtWeight*thisEvent->getbTagSys());
    }

    if(thisEvent->getnSelElectron() == 2){
	hDiElectronMass->Fill(thisEvent->getSelElectronsMass(),_evtWeight*thisEvent->getbTagSys());
	hDiElectronPT->Fill(thisEvent->getSelElectronsPT(),_evtWeight*thisEvent->getbTagSys());
	hDiElectronEta->Fill(thisEvent->getSelElectronsEta(),_evtWeight*thisEvent->getbTagSys());
    }

    hleptonHT->Fill(thisEvent->getSelLeptonHT(),_evtWeight*thisEvent->getbTagSys());
    hST->Fill(thisEvent->getSelLeptonST(),_evtWeight*thisEvent->getbTagSys());
    hLeptonPT1->Fill(thisEvent->getSelLeptons()->at(0)->getp4()->Pt(), _evtWeight*thisEvent->getbTagSys());
    hLeptonEta1->Fill(thisEvent->getSelLeptons()->at(0)->getp4()->Eta(), _evtWeight*thisEvent->getbTagSys());
    hLeptonPT2->Fill(thisEvent->getSelLeptons()->at(1)->getp4()->Pt(), _evtWeight*thisEvent->getbTagSys());
    hLeptonEta2->Fill(thisEvent->getSelLeptons()->at(1)->getp4()->Eta(), _evtWeight*thisEvent->getbTagSys());


    if(thisEvent->getnSelMuon() > 0){
	hMuonPT1->Fill(thisEvent->getSelMuons()->at(0)->getp4()->Pt(), _evtWeight*thisEvent->getbTagSys());
	hMuonEta1->Fill(thisEvent->getSelMuons()->at(0)->getp4()->Eta(), _evtWeight*thisEvent->getbTagSys());
    }
    
    if(thisEvent->getnSelElectron() > 0){
	hElePT1->Fill(thisEvent->getSelElectrons()->at(0)->getp4()->Pt(), _evtWeight*thisEvent->getbTagSys());
	hEleEta1->Fill(thisEvent->getSelElectrons()->at(0)->getp4()->Eta(), _evtWeight*thisEvent->getbTagSys());
    }
    
    if(thisEvent->getnSelMuon() > 1){
	hMuonPT2->Fill(thisEvent->getSelMuons()->at(1)->getp4()->Pt(), _evtWeight*thisEvent->getbTagSys());
	hMuonEta2->Fill(thisEvent->getSelMuons()->at(1)->getp4()->Eta(), _evtWeight*thisEvent->getbTagSys());
    }
    
    if(thisEvent->getnSelElectron() > 1){
	hElePT2->Fill(thisEvent->getSelElectrons()->at(1)->getp4()->Pt(), _evtWeight*thisEvent->getbTagSys());
	hEleEta2->Fill(thisEvent->getSelElectrons()->at(1)->getp4()->Eta(), _evtWeight*thisEvent->getbTagSys());
    } 

    hLepCharge1->Fill(thisEvent->getSelLeptons()->at(0)->charge, _evtWeight*thisEvent->getbTagSys());
    hLepCharge2->Fill(thisEvent->getSelLeptons()->at(1)->charge, _evtWeight*thisEvent->getbTagSys());
    */
}



void ttHHanalyzer_unified::writeHistos(){
    _of->file->cd();
    _histoDirs.at(0)->cd();
    for(int ih=0; ih<nHistsJets; ih++){
//        hjetsPTs.at(ih)->Write();
//        hjetsEtas.at(ih)->Write();
//        hjetsBTagDisc.at(ih)->Write();
  
        // JEC-only 원본 pT
        h_JEC_DiffRatio.at(ih)->Write();
        h_JEC_Mass_DiffRatio.at(ih)->Write();

    }

    ////for(int ih=0; ih<nHistsbJets; ih++){
    ////    hbjetsPTs.at(ih)->Write();
    ////    hbjetsEtas.at(ih)->Write();
    ////    hbjetsBTagDisc.at(ih)->Write();
    ////}

    ////for(int ih=0; ih<nHistsLightJets; ih++){
    ////    hLightJetsPTs.at(ih)->Write();
    ////    hLightJetsEtas.at(ih)->Write();
    ////    hLightJetsBTagDisc.at(ih)->Write();
    ////}

    ////hInvMassHadW->Write();

    ////hInvMassHSingleMatched->Write();
    ////hInvMassHSingleNotMatched->Write();
    ////hChi2HiggsSingleNotMatched->Write();
    ////hChi2HiggsSingleMatched->Write();
    ////hInvMassHH1Matched->Write();
    ////hInvMassHH1NotMatched->Write();
    ////hInvMassHH2Matched->Write();
    ////hInvMassHH2NotMatched->Write();
    ////hChi2HHNotMatched->Write();
    ////hChi2HHMatched->Write();

    ////hjetNumber->Write();
    ////hBjetNumber->Write();
    ////hHadronicHiggsNumber->Write();
    ////hLightJetNumber->Write();
    ////hjetAverageMass->Write();
    ////hBjetAverageMass->Write();
    ////hHadronicHiggsAverageMass->Write();
    ////hLightJetAverageMass->Write();
    ////hBjetAverageMassSqr->Write();
    ////hHadronicHiggsSoftDropMass1->Write();
    ////hHadronicHiggsSoftDropMass2->Write();
    ////hjetHT->Write();
    ////hBjetHT->Write();
    ////hHadronicHiggsHT->Write();
    ////hLightJetHT->Write();
    ////hAvgDeltaRjj->Write();
    ////hminDeltaRjj->Write();
    ////hminDeltaRMassjj->Write();
    ////hminDeltaRpTjj->Write();
    ////hAvgDeltaRbb->Write();
    ////hAvgDeltaEtajj->Write();
    ////hAvgDeltaEtabb->Write();
    ////hminDeltaRbb->Write();
    ////hminDeltaRMassbb->Write();
    ////hminDeltaRpTbb->Write();
    ////hmaxDeltaEtabb->Write();
    ////hmaxDeltaEtajj->Write();
    ////hAvgDeltaRbj->Write();
    ////hAvgDeltaEtabj->Write();
    ////hminDeltaRbj->Write();
    ////hminDeltaRMassbj->Write();
    ////hminDeltaRpTbj->Write();
    ////hmaxDeltaEtabj->Write();
    ////hmaxPTmassjbb->Write();
    ////hmaxPTmassjjj->Write();

    ////hPTH1->Write();
    ////hPTH2->Write();
    ////hInvMassH1->Write();
    ////hInvMassH2->Write();
    ////hInvMassH1_zoomIn->Write();
    ////hInvMassH2_zoomIn->Write();
    ////hInvMassH1mChi->Write();
    ////hInvMassH2mChi->Write();
    ////hInvMassHZ1->Write();
    ////hInvMassHZ2->Write();
    ////hInvMassHZ1_zoomIn->Write();
    ////hInvMassHZ2_zoomIn->Write();
    ////hInvMassZ1->Write();
    ////hInvMassZ2->Write();
    ////hInvMassZ1_zoomIn->Write();
    ////hInvMassZ2_zoomIn->Write();
    ////hChi2Higgs->Write();
    ////hChi2HiggsZ->Write();
    ////hChi2Z->Write();


    ////hmet->Write();
    ////// hmetPhi->Write();
    ////// hmetEta->Write();

    ////hAplanarity->Write();
    ////hSphericity->Write();
    ////hTransSphericity->Write();
    ////hCvalue->Write();
    ////hDvalue->Write();
    ////hCentralityjb->Write();
    ////hCentralityjl->Write();

    ////hH0->Write();
    ////hH1->Write();
    ////hH2->Write();
    ////hH3->Write();
    ////hH4->Write();
    ////hR1->Write();
    ////hR2->Write();
    ////hR3->Write();
    ////hR4->Write(); 

    ////hBjetH0->Write();
    ////hBjetH1->Write();
    ////hBjetH2->Write();
    ////hBjetH3->Write();
    ////hBjetH4->Write();
    ////hBjetR1->Write();
    ////hBjetR2->Write();
    ////hBjetR3->Write();
    ////hBjetR4->Write(); 

    ////hBjetAplanarity->Write();
    ////hBjetSphericity->Write();
    ////hBjetTransSphericity->Write();
    ////hBjetCvalue->Write();
    ////hBjetDvalue->Write();

    _histoDirs.at(1)->cd();
    
    /*    hLepCharge1->Write();
    hLepCharge2->Write();

    hleptonNumber->Write();
    hDiMuonMass->Write();
    hDiElectronMass->Write();
    hDiMuonPT->Write();
    hDiElectronPT->Write();
    hDiMuonEta->Write();
    hDiElectronEta->Write();
    hleptonHT->Write();
    hST->Write();
    hLeptonEta1->Write();
    hLeptonPT1->Write();
    hLeptonEta2->Write();
    hLeptonPT2->Write();
    
    hMuonEta1->Write();
    hMuonPT1->Write();
    hEleEta1->Write();
    hElePT1->Write();
    hMuonEta2->Write();
    hMuonPT2->Write();
    hEleEta2->Write();
    hElePT2->Write(); */

    _histoDirs.at(2)->cd();
    for (size_t i = 0; i < _cutStepJetPt.size(); ++i) {
        for (size_t j = 0; j < kNJetsForCutStep; ++j) {
            _cutStepJetPt.at(i).at(j)->Write();
            _cutStepJetEta.at(i).at(j)->Write();
            _cutStepJetPhi.at(i).at(j)->Write();
            _cutStepBTag.at(i).at(j)->Write();
        }
        _cutStepHT.at(i)->Write();
        _cutStepHadWMass.at(i)->Write();
        _cutStepHiggsMass01.at(i)->Write();
        _cutStepHiggsMass02.at(i)->Write();

        // [NEW] Three-chain per-step hists
        _cutStepHT_btagSF       .at(i)->Write();
        _cutStepHT_full         .at(i)->Write();
        _cutStepHadWMass_btagSF .at(i)->Write();
        _cutStepHadWMass_full   .at(i)->Write();
        _cutStepHiggsMass01_btagSF.at(i)->Write();
        _cutStepHiggsMass01_full  .at(i)->Write();
        _cutStepHiggsMass02_btagSF.at(i)->Write();
        _cutStepHiggsMass02_full  .at(i)->Write();
        _cutStepNbJets_raw      .at(i)->Write();
        _cutStepNbJets_btagSF   .at(i)->Write();
        _cutStepNbJets_full     .at(i)->Write();
        _cutStepNjets_raw       .at(i)->Write();
        _cutStepNjets_btagSF    .at(i)->Write();
        _cutStepNjets_full      .at(i)->Write();
    }

    // ── tt+jets categorization validation histograms (write + summary) ──
    _histoDirs.at(3)->cd();
    _hTtCat_Counts_AnaGenPart->Write();
    _hTtCat_Counts_AnaGenId  ->Write();
    _hTtCat_Counts_NtuPrimary->Write();
    _hTtCat_Counts_NtuXval   ->Write();

    _hTtCat_AnaGenPart_vs_AnaGenId  ->Write();
    _hTtCat_AnaGenPart_vs_NtuPrimary->Write();
    _hTtCat_AnaGenPart_vs_NtuXval   ->Write();
    _hTtCat_AnaGenId_vs_NtuPrimary  ->Write();
    _hTtCat_AnaGenId_vs_NtuXval     ->Write();
    _hTtCat_NtuPrimary_vs_NtuXval   ->Write();

    _hTtCat_GenTtbarIdMod100->Write();

    // ── End-of-job summary ──
    //
    // For each estimator: per-category event count (1D).
    // For each pair: diagonal (agreement) and off-diagonal (disagreement) totals,
    // with per-cell off-diagonal listing for the "must-agree" pairs.
    std::cout << "\n[ttCatSummary] ═══════════════════════════════════════════════" << std::endl;
    const double totalEvents = _hTtCat_Counts_AnaGenPart->GetEntries();
    std::cout << "[ttCatSummary] Total MC events processed: "
              << static_cast<long long>(totalEvents) << std::endl;

    // Per-estimator breakdown
    auto dumpCounts = [&](const char* name, TH1F* h) {
        std::cout << "[ttCatSummary] " << name << " breakdown:" << std::endl;
        for (int i = 1; i <= kNTtCat; ++i) {
            const double c = h->GetBinContent(i);
            if (c > 0) {
                std::cout << "  " << ttCatName(i-1) << ": "
                          << static_cast<long long>(c) << std::endl;
            }
        }
    };
    dumpCounts("ANA_GENPART (analyzer GenPart)",   _hTtCat_Counts_AnaGenPart);
    dumpCounts("ANA_GENID   (analyzer genTtbarId)", _hTtCat_Counts_AnaGenId);
    dumpCounts("NTU_PRIMARY (ntuple ttCat_*)",      _hTtCat_Counts_NtuPrimary);
    dumpCounts("NTU_XVAL    (ntuple ttCatXval_*)",  _hTtCat_Counts_NtuXval);

    // Pair-wise agreement summary
    auto pairAgreement = [&](const char* label, TH2F* h, bool listOff) {
        double diag = 0, off = 0;
        for (int ix = 1; ix <= kNTtCat; ++ix) {
            for (int iy = 1; iy <= kNTtCat; ++iy) {
                const double c = h->GetBinContent(ix, iy);
                if (ix == iy) {
                    diag += c;
                } else if (c > 0) {
                    off += c;
                    if (listOff) {
                        std::cout << "    OFF-DIAG: " << ttCatName(ix-1)
                                  << " vs " << ttCatName(iy-1)
                                  << " = " << static_cast<long long>(c) << std::endl;
                    }
                }
            }
        }
        const double total = diag + off;
        std::cout << "[ttCatSummary] " << label
                  << "  agree=" << static_cast<long long>(diag)
                  << "  disagree=" << static_cast<long long>(off);
        if (total > 0) {
            std::cout << std::fixed << std::setprecision(4)
                      << "  agreement=" << (diag / total * 100.0) << "%";
        }
        std::cout << std::endl;
    };

    // The two "must agree" pairs — list every off-diagonal cell.
    std::cout << "[ttCatSummary] --- MUST-AGREE PAIRS (any disagreement = bug) ---" << std::endl;
    pairAgreement("ANA_GENID   vs NTU_PRIMARY",
                  _hTtCat_AnaGenId_vs_NtuPrimary,   /*listOff=*/true);
    pairAgreement("ANA_GENPART vs NTU_XVAL    ",
                  _hTtCat_AnaGenPart_vs_NtuXval,    /*listOff=*/true);

    // The other four pairs — informational only (real algorithmic differences).
    std::cout << "[ttCatSummary] --- INFORMATIONAL PAIRS (real algorithmic diff) ---" << std::endl;
    pairAgreement("ANA_GENPART vs ANA_GENID    ",
                  _hTtCat_AnaGenPart_vs_AnaGenId,   /*listOff=*/false);
    pairAgreement("ANA_GENPART vs NTU_PRIMARY  ",
                  _hTtCat_AnaGenPart_vs_NtuPrimary, /*listOff=*/false);
    pairAgreement("ANA_GENID   vs NTU_XVAL     ",
                  _hTtCat_AnaGenId_vs_NtuXval,      /*listOff=*/false);
    pairAgreement("NTU_PRIMARY vs NTU_XVAL     ",
                  _hTtCat_NtuPrimary_vs_NtuXval,    /*listOff=*/false);

    std::cout << "[ttCatSummary] ═══════════════════════════════════════════════\n"
              << std::endl;
}
void ttHHanalyzer_unified::fillTree(event * thisEvent){

    jetPt.clear();
    jetEta.clear();
    bTagScore.clear();
    hadFlavs.clear();
    partonFlavs.clear();
    jetPUids.clear();

    // For Trigger Path
    passTrigger_HLT_IsoMu27 = _ev->HLT_IsoMu27; // Reference Muon Trigger
    passTrigger_HLT_PFHT1050 = _ev->HLT_PFHT1050;
    //passTrigger_HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59 = _ev->HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59;
    //passTrigger_HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94 = _ev->HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94;
    //passTrigger_HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5 = _ev->HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5;
    passTrigger_6J1T_B    = _ev->HLT_PFHT430_SixJet40_BTagCSV_p080;
    passTrigger_6J1T_CDEF = _ev->HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5;
    passTrigger_6J2T_B    = _ev->HLT_PFHT380_SixJet32_DoubleBTagCSV_p075;
    passTrigger_6J2T_CDEF = _ev->HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2;
    passTrigger_4J3T_B    = _ev->HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07;
    passTrigger_4J3T_CDEF = _ev->HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0;

    nMuons = thisEvent->getnSelMuon();
    nElecs = thisEvent->getnSelElectron();
    nJets = thisEvent->getnSelJet();
    nbJets = thisEvent->getnSelbJet();
    HT = thisEvent->getSumSelJetScalarpT();


    // Fill the jet information [ It will fill the nJets && maximum 30th jets ]
    for (int i = 0; i < nJets; ++i) {

        jetPt.push_back(thisEvent->getSelJets()->at(i)->getp4()->Pt());
        jetEta.push_back(thisEvent->getSelJets()->at(i)->getp4()->Eta());
        bTagScore.push_back(thisEvent->getSelJets()->at(i)->bTagCSV);
        hadFlavs.push_back(thisEvent->getSelJets()->at(i)->hadFlav);
        partonFlavs.push_back(thisEvent->getSelJets()->at(i)->partonFlav);
        jetPUids.push_back(thisEvent->getSelJets()->at(i)->jetPUid);
        
        // In the Event Buffer, you can find the struct jet_s which accesses to defined branch variables in NanoAODv9
        // And then in the .hh files, you also can check variables like  bTagCSV or hadFlav as objectJet class's  member variable
        // Then you can get the info at NanoAODv9 from jet_s structure and put them into the .hh's objectJet class member variable in the iteration of jets
        // Finally here, you can store variables in the objectJet class's member variables into tree
    } 

    eventNumber = _ev->event;
    runNumber = _ev->run;
 
    evtWeight    = _evtWeight;
    SampleWeight = _SampleWeight;
    PUWeight = _PUWeight;
    L1PrefiringWeight = _L1PrefiringWeight;
    genWeight = _genWeight;
    bTagWeight = bTagWeight_central_;
    failGoldenJson = _failGoldenJson;
    passMETFilters = _passMETFilters;
    passHadTrig = thisEvent->getHadTriggerAccept();

////////////////////////////////////////////////////////////////////////////////////////
   
    //////bjetPT1 = thisEvent->getSelJets()->at(0)->getp4()->Pt();
    //////bjetPT2 = thisEvent->getSelJets()->at(1)->getp4()->Pt();
    //////bjetPT3 = thisEvent->getSelJets()->at(2)->getp4()->Pt();
    //////bjetPT4 = thisEvent->getSelJets()->at(3)->getp4()->Pt();
    //////bjetEta1 = thisEvent->getSelJets()->at(0)->getp4()->Eta();
    //////bjetEta2 = thisEvent->getSelJets()->at(1)->getp4()->Eta();
    //////bjetEta3 = thisEvent->getSelJets()->at(2)->getp4()->Eta();
    //////bjetEta4 = thisEvent->getSelJets()->at(3)->getp4()->Eta();
    //////bjetBTagDisc1 = getJetCSV(thisEvent).at(0);
    //////bjetBTagDisc2 = getJetCSV(thisEvent).at(1);
    //////bjetBTagDisc3 = getJetCSV(thisEvent).at(2);
    //////bjetBTagDisc4 = getJetCSV(thisEvent).at(3);


    //////bbjetPT1 = thisEvent->getSelbJets()->at(0)->getp4()->Pt();
    //////bbjetPT2 = thisEvent->getSelbJets()->at(1)->getp4()->Pt();
    //////bbjetPT3 = thisEvent->getSelbJets()->at(2)->getp4()->Pt();
    //////bbjetEta1 = thisEvent->getSelbJets()->at(0)->getp4()->Eta();
    //////bbjetEta2 = thisEvent->getSelbJets()->at(1)->getp4()->Eta();
    //////bbjetEta3 = thisEvent->getSelbJets()->at(2)->getp4()->Eta();
    //////bbjetPhi1 = thisEvent->getSelbJets()->at(0)->getp4()->Phi();
    //////bbjetPhi2 = thisEvent->getSelbJets()->at(1)->getp4()->Phi();
    //////bbjetPhi3 = thisEvent->getSelbJets()->at(2)->getp4()->Phi();
    //////bbjetBTagDisc1 = getbJetCSV(thisEvent).at(0);
    //////bbjetBTagDisc2 = getbJetCSV(thisEvent).at(1);
    //////bbjetBTagDisc3 = getbJetCSV(thisEvent).at(2);
    //////bbjetHiggsMatched1 = thisEvent->getSelbJets()->at(0)->matchedtoHiggs;
    //////bbjetHiggsMatched2 = thisEvent->getSelbJets()->at(1)->matchedtoHiggs;
    //////bbjetHiggsMatched3 = thisEvent->getSelbJets()->at(2)->matchedtoHiggs;
    //////bbjetHiggsMatcheddR1 = thisEvent->getSelbJets()->at(0)->matchedtoHiggsdR;
    //////bbjetHiggsMatcheddR2 = thisEvent->getSelbJets()->at(1)->matchedtoHiggsdR;
    //////bbjetHiggsMatcheddR3 = thisEvent->getSelbJets()->at(2)->matchedtoHiggsdR;
    //////bbjetMinChiHiggsIndex1 = thisEvent->getSelbJets()->at(0)->minChiHiggsIndex;
    //////bbjetMinChiHiggsIndex2 = thisEvent->getSelbJets()->at(1)->minChiHiggsIndex;
    //////bbjetMinChiHiggsIndex3 = thisEvent->getSelbJets()->at(2)->minChiHiggsIndex;


    //////if(thisEvent->getnSelJet() > 4){
    //////	bjetPT5 = thisEvent->getSelJets()->at(4)->getp4()->Pt();
    //////    bjetEta5 = thisEvent->getSelJets()->at(4)->getp4()->Eta();
    //////    bjetBTagDisc5 = getJetCSV(thisEvent).at(4);
    //////} else{
    //////    bjetPT5 = -6;
    //////    bjetEta5 = -6;
    //////    bjetBTagDisc5 = -6;
    //////}
    //////
    //////if(thisEvent->getnSelJet() > 5){
    //////    bjetPT6 = thisEvent->getSelJets()->at(5)->getp4()->Pt();
    //////    bjetEta6 = thisEvent->getSelJets()->at(5)->getp4()->Eta();
    //////    bjetBTagDisc6 = getJetCSV(thisEvent).at(5);
    //////} else{
    //////    bjetPT6 = -6;
    //////    bjetEta6 = -6;
    //////    bjetBTagDisc6 = -6;
    //////}


    //////if(thisEvent->getnSelJet() > 6){
    //////    bjetPT7 = thisEvent->getSelJets()->at(6)->getp4()->Pt();
    //////    bjetEta7 = thisEvent->getSelJets()->at(6)->getp4()->Eta();
    //////    bjetBTagDisc7 = getJetCSV(thisEvent).at(6);
    //////} else{
    //////    bjetPT7 = -6;
    //////    bjetEta7 = -6;
    //////    bjetBTagDisc7 = -6;
    //////}

    //////if(thisEvent->getnSelJet() > 7){
    //////    bjetPT8 = thisEvent->getSelJets()->at(7)->getp4()->Pt();
    //////    bjetEta8 = thisEvent->getSelJets()->at(7)->getp4()->Eta();
    //////    bjetBTagDisc8 = getJetCSV(thisEvent).at(7);
    //////} else{
    //////    bjetPT8 = -6;
    //////    bjetEta8 = -6;
    //////    bjetBTagDisc8 = -6;
    //////}

    //////if(thisEvent->getnSelJet() > 8){
    //////    bjetPT9 = thisEvent->getSelJets()->at(8)->getp4()->Pt();
    //////    bjetEta9 = thisEvent->getSelJets()->at(8)->getp4()->Eta();
    //////    bjetBTagDisc9 = getJetCSV(thisEvent).at(8);
    //////} else{
    //////    bjetPT9 = -6;
    //////    bjetEta9 = -6;
    //////    bjetBTagDisc9 = -6;
    //////}

    //////if(thisEvent->getnSelJet() > 9){
    //////    bjetPT10 = thisEvent->getSelJets()->at(9)->getp4()->Pt();
    //////    bjetEta10 = thisEvent->getSelJets()->at(9)->getp4()->Eta();
    //////    bjetBTagDisc10 = getJetCSV(thisEvent).at(9);
    //////} else{
    //////    bjetPT10 = -6;
    //////    bjetEta10 = -6;
    //////    bjetBTagDisc10 = -6;
    //////}

    //////if(thisEvent->getnSelJet() > 10){
    //////    bjetPT11 = thisEvent->getSelJets()->at(10)->getp4()->Pt();
    //////    bjetEta11 = thisEvent->getSelJets()->at(10)->getp4()->Eta();
    //////    bjetBTagDisc11 = getJetCSV(thisEvent).at(10);
    //////} else{
    //////    bjetPT11 = -6;
    //////    bjetEta11 = -6;
    //////    bjetBTagDisc11 = -6;
    //////}

    //////if(thisEvent->getnSelJet() > 11){
    //////    bjetPT12 = thisEvent->getSelJets()->at(11)->getp4()->Pt();
    //////    bjetEta12 = thisEvent->getSelJets()->at(11)->getp4()->Eta();
    //////    bjetBTagDisc12 = getJetCSV(thisEvent).at(11);
    //////} else{
    //////    bjetPT12 = -6;
    //////    bjetEta12 = -6;
    //////    bjetBTagDisc12 = -6;
    //////}

    //////// # b-jet
    //////if(thisEvent->getnSelbJet() > 3){
    //////    bbjetPT4 = thisEvent->getSelbJets()->at(3)->getp4()->Pt();
    //////    bbjetEta4 = thisEvent->getSelbJets()->at(3)->getp4()->Eta();
    //////    bbjetPhi4 = thisEvent->getSelbJets()->at(3)->getp4()->Phi();
    //////    bbjetBTagDisc4 = getbJetCSV(thisEvent).at(3);
    //////    bbjetHiggsMatched4 = thisEvent->getSelbJets()->at(3)->matchedtoHiggs;
    //////    bbjetHiggsMatcheddR4 = thisEvent->getSelbJets()->at(3)->matchedtoHiggsdR;
    //////    bbjetMinChiHiggsIndex4 = thisEvent->getSelbJets()->at(3)->minChiHiggsIndex;
    //////} else {
    //////    bbjetPT4 = -6;
    //////    bbjetEta4 = -6;
    //////    bbjetPhi4 = -6;
    //////    bbjetBTagDisc4 = -6;
    //////    bbjetHiggsMatched4 = 0;
    //////    bbjetHiggsMatcheddR4 = -6;
    //////    bbjetMinChiHiggsIndex4 = -6;
    //////}
    //////
    //////if(thisEvent->getnSelbJet() > 4){
    //////    bbjetPT5 = thisEvent->getSelbJets()->at(4)->getp4()->Pt();
    //////    bbjetEta5 = thisEvent->getSelbJets()->at(4)->getp4()->Eta();
    //////    bbjetPhi5 = thisEvent->getSelbJets()->at(4)->getp4()->Phi();
    //////    bbjetBTagDisc5 = getbJetCSV(thisEvent).at(4);
    //////    bbjetHiggsMatched5 = thisEvent->getSelbJets()->at(4)->matchedtoHiggs;
    //////    bbjetHiggsMatcheddR5 = thisEvent->getSelbJets()->at(4)->matchedtoHiggsdR;
    //////    bbjetMinChiHiggsIndex5 = thisEvent->getSelbJets()->at(4)->minChiHiggsIndex;
    //////} else {
    //////    bbjetPT5 = -6;
    //////    bbjetEta5 = -6;
    //////    bbjetPhi5 = -6;
    //////    bbjetBTagDisc5 = -6;
    //////    bbjetHiggsMatched5 = 0;
    //////    bbjetHiggsMatcheddR5 = -6;
    //////    bbjetMinChiHiggsIndex5 = -6;
    //////}

    //////if(thisEvent->getnSelbJet() > 5){
    //////    bbjetPT6 = thisEvent->getSelbJets()->at(5)->getp4()->Pt();
    //////    bbjetEta6 = thisEvent->getSelbJets()->at(5)->getp4()->Eta();
    //////    bbjetPhi6 = thisEvent->getSelbJets()->at(5)->getp4()->Phi();
    //////    bbjetBTagDisc6 = getbJetCSV(thisEvent).at(5);
    //////    bbjetHiggsMatched6 = thisEvent->getSelbJets()->at(5)->matchedtoHiggs;
    //////    bbjetHiggsMatcheddR6 = thisEvent->getSelbJets()->at(5)->matchedtoHiggsdR;
    //////    bbjetMinChiHiggsIndex6 = thisEvent->getSelbJets()->at(5)->minChiHiggsIndex;
    //////} else {
    //////    bbjetPT6 = -6;
    //////    bbjetEta6 = -6;
    //////    bbjetPhi6 = -6;
    //////    bbjetBTagDisc6 = -6;
    //////    bbjetHiggsMatched6 = 0;
    //////    bbjetHiggsMatcheddR6 = -6;
    //////    bbjetMinChiHiggsIndex6 = -6;
    //////}

    //////if(thisEvent->getnSelbJet() > 6){
    //////    bbjetPT7 = thisEvent->getSelbJets()->at(6)->getp4()->Pt();
    //////    bbjetEta7 = thisEvent->getSelbJets()->at(6)->getp4()->Eta();
    //////    bbjetPhi7 = thisEvent->getSelbJets()->at(6)->getp4()->Phi();
    //////    bbjetBTagDisc7 = getbJetCSV(thisEvent).at(6);
    //////    bbjetHiggsMatched7 = thisEvent->getSelbJets()->at(6)->matchedtoHiggs;
    //////    bbjetHiggsMatcheddR7 = thisEvent->getSelbJets()->at(6)->matchedtoHiggsdR;
    //////} else {
    //////    bbjetPT7 = -6;
    //////    bbjetEta7 = -6;
    //////    bbjetPhi7 = -6;
    //////    bbjetBTagDisc7 = -6;
    //////    bbjetHiggsMatched7 = 0;
    //////    bbjetHiggsMatcheddR7 = -6;
    //////}


    //////if(thisEvent->getnSelbJet() > 7){
    //////    bbjetPT8 = thisEvent->getSelbJets()->at(7)->getp4()->Pt();
    //////    bbjetEta8 = thisEvent->getSelbJets()->at(7)->getp4()->Eta();
    //////    bbjetPhi8 = thisEvent->getSelbJets()->at(7)->getp4()->Phi();
    //////    bbjetBTagDisc8 = getbJetCSV(thisEvent).at(7);
    //////    bbjetHiggsMatched8 = thisEvent->getSelbJets()->at(7)->matchedtoHiggs;
    //////    bbjetHiggsMatcheddR8 = thisEvent->getSelbJets()->at(7)->matchedtoHiggsdR;
    //////} else {
    //////    bbjetPT8 = -6;
    //////    bbjetEta8 = -6;
    //////    bbjetPhi8 = -6;
    //////    bbjetBTagDisc8 = -6;
    //////    bbjetHiggsMatched8 = 0;
    //////    bbjetHiggsMatcheddR8 = 0;
    //////}


    //////if(thisEvent->getnLightJet() > 0){
    //////	blightjetPT1 = thisEvent->getSelLightJets()->at(0)->getp4()->Pt();
    //////    blightjetEta1 = thisEvent->getSelLightJets()->at(0)->getp4()->Eta();
    //////    blightjetBTagDisc1 = getlightJetCSV(thisEvent).at(0);
    //////} else{
    //////    blightjetPT1 = -6;
    //////    blightjetEta1 = -6;
    //////    blightjetBTagDisc1 = -6;
    //////}

    //////if(thisEvent->getnLightJet() > 1){
    //////	blightjetPT2 = thisEvent->getSelLightJets()->at(1)->getp4()->Pt();
    //////    blightjetEta2 = thisEvent->getSelLightJets()->at(1)->getp4()->Eta();
    //////    blightjetBTagDisc2 = getlightJetCSV(thisEvent).at(1);
    //////} else{
    //////    blightjetPT2 = -6;
    //////    blightjetEta2 = -6;
    //////    blightjetBTagDisc2 = -6;
    //////}

    //////if(thisEvent->getnLightJet() > 2){
    //////	blightjetPT3 = thisEvent->getSelLightJets()->at(2)->getp4()->Pt();
    //////    blightjetEta3 = thisEvent->getSelLightJets()->at(2)->getp4()->Eta();
    //////    blightjetBTagDisc3 = getlightJetCSV(thisEvent).at(2);
    //////} else{
    //////    blightjetPT3 = -6;
    //////    blightjetEta3 = -6;
    //////    blightjetBTagDisc3 = -6;
    //////}

    //////if(thisEvent->getnLightJet() > 3){
    //////	blightjetPT4 = thisEvent->getSelLightJets()->at(3)->getp4()->Pt();
    //////    blightjetEta4 = thisEvent->getSelLightJets()->at(3)->getp4()->Eta();
    //////    blightjetBTagDisc4 = getlightJetCSV(thisEvent).at(3);
    //////} else{
    //////    blightjetPT4 = -6;
    //////    blightjetEta4 = -6;
    //////    blightjetBTagDisc4 = -6;
    //////}

    //////if(thisEvent->getnLightJet() > 4){
    //////	blightjetPT5 = thisEvent->getSelLightJets()->at(4)->getp4()->Pt();
    //////    blightjetEta5 = thisEvent->getSelLightJets()->at(4)->getp4()->Eta();
    //////    blightjetBTagDisc5 = getlightJetCSV(thisEvent).at(4);
    //////} else{
    //////    blightjetPT5 = -6;
    //////    blightjetEta5 = -6;
    //////    blightjetBTagDisc5 = -6;
    //////}

    //////if(thisEvent->getnLightJet() > 5){
    //////	blightjetPT6 = thisEvent->getSelLightJets()->at(5)->getp4()->Pt();
    //////    blightjetEta6 = thisEvent->getSelLightJets()->at(5)->getp4()->Eta();
    //////    blightjetBTagDisc6 = getlightJetCSV(thisEvent).at(5);
    //////} else{
    //////    blightjetPT6 = -6;
    //////    blightjetEta6 = -6;
    //////    blightjetBTagDisc6 = -6;
    //////}


    //////bweight= _weight;
    //////bjetAverageMass = thisEvent->getSumSelJetMass()/thisEvent->getnSelJet();
    //////bbJetAverageMass = thisEvent->getSumSelbJetMass()/thisEvent->getnSelbJet();
    //////blightJetAverageMass = thisEvent->getSumSelLightJetMass()/thisEvent->getnLightJet();
    //////bbJetAverageMassSqr = (thisEvent->getSumSelbJetMass()*thisEvent->getSumSelbJetMass())/thisEvent->getnSelbJet();
    //////bmet = thisEvent->getMET()->getp4()->Pt();
    //////baverageDeltaRjj = jetStat.meandR;
    //////baverageDeltaRbb = bjetStat.meandR;
    //////baverageDeltaRbj = bjStat.meandR;
    //////baverageDeltaEtajj = jetStat.meandEta;
    //////baverageDeltaEtabb = bjetStat.meandEta;
    //////baverageDeltaEtabj = bjStat.meandEta;
    //////bminDeltaRjj = jetStat.mindR;
    //////bminDeltaRbb = bjetStat.mindR;
    //////bminDeltaRbj = bjStat.mindR;
    //////bmaxDeltaEtabb = bjetStat.maxdEta;
    //////bmaxDeltaEtajj = jetStat.maxdEta;
    //////bmaxDeltaEtabj = bjStat.maxdEta;
    //////bminDeltaRMassjj = jetStat.mindRMass;
    //////bminDeltaRMassbb = bjetStat.mindRMass;
    //////bminDeltaRMassbj = bjStat.mindRMass;
    //////bminDeltaRpTjj = jetStat.mindRpT;
    //////bminDeltaRpTbb = bjetStat.mindRpT;
    //////bminDeltaRpTbj = bjStat.mindRpT;
    //////bmaxPTmassjjj = jjjMaxs.maxPTmass;
    //////bmaxPTmassjbb = jbbMaxs.maxPTmass;
    //////bH0 = jetFoxWolfMom.h0;
    //////bH1 = jetFoxWolfMom.h1;
    //////bH2 = jetFoxWolfMom.h2;
    //////bH3 = jetFoxWolfMom.h3;
    //////bH4 = jetFoxWolfMom.h4;
    //////bbH0 = bjetFoxWolfMom.h0;
    //////bbH1 = bjetFoxWolfMom.h1;
    //////bbH2 = bjetFoxWolfMom.h2;
    //////bbH3 = bjetFoxWolfMom.h3;
    //////bbH4 = bjetFoxWolfMom.h4;
    //////bR1 = jetFoxWolfMom.r1;
    //////bR2 = jetFoxWolfMom.r2;
    //////bR3 = jetFoxWolfMom.r3;
    //////bR4 = jetFoxWolfMom.r4;
    //////bbR1 = bjetFoxWolfMom.r1;
    //////bbR2 = bjetFoxWolfMom.r2;
    //////bbR3 = bjetFoxWolfMom.r3;
    //////bbR4 = bjetFoxWolfMom.r4;

    //////bjetHT = thisEvent->getSumSelJetScalarpT();  
    //////bbjetHT = thisEvent->getSumSelbJetScalarpT();
    //////blightjetHT = thisEvent->getSumSelLightJetScalarpT();
    //////bjetNumber = thisEvent->getnSelJet();
    //////bbjetNumber = thisEvent->getnSelbJet();
    //////blightjetNumber = thisEvent->getnLightJet();
    //////binvMassZ1 = _bbMassMin1Z; //ZZ
    //////binvMassZ2 = _bbMassMin2Z;
    //////bchi2Z = _minChi2Z;
    //////binvMassH1 = _bbMassMin1Higgs; //HH
    //////binvMassH2 = _bbMassMin2Higgs;
    //////bchi2Higgs = _minChi2Higgs;
    //////bchi2HiggsZ = _minChi2HiggsZ; //ZH
    //////binvMassHiggsZ1 = _bbMassMin1HiggsZ;
    //////binvMassHiggsZ2 = _bbMassMin2HiggsZ;
    //////bPTH1 = _bpTHiggs1;
    //////bPTH2 = _bpTHiggs2;


    //////bcentralityjb = jbjetCent.centrality; 
    //////bcentralityjl = jlepCent.centrality; 
    //////baplanarity = thisEvent->eventShapeJet->getAplanarity();
    //////bsphericity = thisEvent->eventShapeJet->getSphericity();
    //////btransSphericity = thisEvent->eventShapeJet->getTransSphericity();
    //////bcValue = thisEvent->eventShapeJet->getC();
    //////bdValue = thisEvent->eventShapeJet->getD();
    //////bbaplanarity = thisEvent->eventShapeBjet->getAplanarity();
    //////bbsphericity = thisEvent->eventShapeBjet->getSphericity();
    //////bbtransSphericity = thisEvent->eventShapeBjet->getTransSphericity();
    //////bbcValue = thisEvent->eventShapeBjet->getC();
    //////bbdValue = thisEvent->eventShapeBjet->getD();

    ///////*    bleptonPT1 = thisEvent->getSelLeptons()->at(0)->getp4()->Pt();
    //////bleptonPT2 = thisEvent->getSelLeptons()->at(1)->getp4()->Pt();
    //////bleptonEta1 = thisEvent->getSelLeptons()->at(0)->getp4()->Eta();
    //////bleptonEta2 = thisEvent->getSelLeptons()->at(1)->getp4()->Eta();
    //////bleptonCharge1 = thisEvent->getSelLeptons()->at(0)->charge;
    //////bleptonCharge2 = thisEvent->getSelLeptons()->at(1)->charge;
    //////bleptonHT = thisEvent->getSelLeptonHT();
    //////bST = thisEvent->getSelLeptonST();


    //////if(thisEvent->getnSelMuon() > 0){
    //////    bmuonPT1 = thisEvent->getSelMuons()->at(0)->getp4()->Pt();
    //////    bmuonEta1 = thisEvent->getSelMuons()->at(0)->getp4()->Eta();
    //////} else {
    //////    bmuonPT1 = -6;
    //////    bmuonEta1 = -6;
    //////}

    //////if(thisEvent->getnSelMuon() > 1){
    //////    bmuonPT2 = thisEvent->getSelMuons()->at(1)->getp4()->Pt();
    //////    bmuonEta2 = thisEvent->getSelMuons()->at(1)->getp4()->Eta();
    //////    bdiMuonMass = thisEvent->getSelMuonsMass();
    //////} else {
    //////    bmuonPT2 = -6;
    //////    bmuonEta2 = -6;
    //////    bdiMuonMass = -6;
    //////} 

    //////if(thisEvent->getnSelElectron() > 0){
    //////    belePT1 = thisEvent->getSelElectrons()->at(0)->getp4()->Pt();
    //////    beleEta1 = thisEvent->getSelElectrons()->at(0)->getp4()->Eta();
    //////} else {
    //////    belePT1 = -6;
    //////    beleEta1 = -6;
    //////}

    //////if(thisEvent->getnSelElectron() > 1){
    //////    belePT2 = thisEvent->getSelElectrons()->at(1)->getp4()->Pt();
    //////    beleEta2 = thisEvent->getSelElectrons()->at(1)->getp4()->Eta();
    //////    bdiElectronMass = thisEvent->getSelElectronsMass();
    //////} else {
    //////    belePT2 = -6;
    //////    beleEta2 = -6;
    //////    bdiElectronMass = -6;
    //////    } */

    
    _inputTree->Fill();
}

void ttHHanalyzer_unified::writeTree(){
    _of->file->cd();
    _treeDirs.at(0)->cd();
    _inputTree->Write();
    //    _inputTree->Delete();
    
}

// ============================================================================
// [kPrescan] mode — gen-level accumulators
//
// ⚠ TEMPORARY IMPLEMENTATION.
// This logic should eventually live inside NtupleForge so that the
// per-sample Σ genWeight, the per-(genTtbarId %% 100) breakdown, and
// the NanoAOD Runs tree numbers are written into the produced ntuple
// as a tiny friend tree alongside the Events tree. That removes the
// need to re-open files in the analyzer just to read the Runs tree,
// and guarantees consistency between skim-stage counting and the
// downstream normalisation.
//
// AN refs:
//   ttHH AN-2022/122 §3.3   (SL Option1 — gen-level partition for stitching)
//   ttHH AN-2022/122 §3.4   (DL — gen-level 4b exclusion)
//   ttH  AN-19-094  §6.2.1  (FH/SL/DL — NNLO+NNLL anchoring procedure)
//   ttH  AN-19-094  App. C  (rate/shape uncertainty treatment)
// ============================================================================

void ttHHanalyzer_unified::runPrescan() {
    print("--------------------------------------------------------------------------", "b");
    print("[Prescan] Running kPrescan mode — no selection applied.", "b");
    print("[Prescan] Sample: " + _sampleName, "b");
    print("--------------------------------------------------------------------------", "b");

    // 1) Read the Runs tree(s) BEFORE the Events loop.
    //    Uses a fresh TFile per input file; never touches eventBuffer's TFile.
    readRunsTreeSums();

    // 2) Events loop — gen-level scalars only (no createObjects / selectObjects).
    const long long nEv = _ev->size();
    std::cout << "[Prescan] Events tree: " << nEv << " entries" << std::endl;
    for (long long i = 0; i < nEv; ++i) {
        _ev->read(i);
        accumulatePrescanEvent();

        if (i % 200000 == 0) {
            std::cout << "[Prescan] processed " << i << " / " << nEv << std::endl;
        }
    }

    // 3) Dump one-row TTree + console summary.
    writePrescanTree();
}

// ----------------------------------------------------------------------------

void ttHHanalyzer_unified::readRunsTreeSums() {
    // Acquire the input file list via itreestream's public accessor.
    // (See treestream.h: std::vector<std::string> itreestream::filenames();)
    const std::vector<std::string> fnames = _ev->input->filenames();
    std::cout << "[Prescan] Runs tree: scanning " << fnames.size()
              << " input file(s)" << std::endl;

    _prescan_runs_sumW          = 0.0;
    _prescan_runs_sumW2         = 0.0;
    _prescan_runs_count         = 0;
    _prescan_nFilesProcessed    = 0;

    for (const auto& fn : fnames) {
        // Fresh TFile, disjoint from eventBuffer's file handle.
        std::unique_ptr<TFile> f(TFile::Open(fn.c_str(), "READ"));
        if (!f || f->IsZombie()) {
            std::cerr << "[Prescan][WARN] cannot open: " << fn << std::endl;
            continue;
        }

        TTree* runs = nullptr;
        f->GetObject("Runs", runs);
        if (!runs) {
            // Data NanoAOD has no Runs/genEventSumw — that's fine.
            if (_DataOrMC != "Data") {
                std::cerr << "[Prescan][WARN] no Runs tree in: " << fn << std::endl;
            }
            continue;
        }

        Double_t  sumW_one   = 0.0;
        Double_t  sumW2_one  = 0.0;
        Long64_t count_one  = 0;

        if (runs->GetBranch("genEventSumw"))
            runs->SetBranchAddress("genEventSumw",  &sumW_one);
        if (runs->GetBranch("genEventSumw2"))
            runs->SetBranchAddress("genEventSumw2", &sumW2_one);
        if (runs->GetBranch("genEventCount"))
            runs->SetBranchAddress("genEventCount", &count_one);

        const Long64_t nR = runs->GetEntries();
        for (Long64_t r = 0; r < nR; ++r) {
            // Reset before each entry in case some branches are absent
            sumW_one = 0.0; sumW2_one = 0.0; count_one = 0;
            runs->GetEntry(r);
            _prescan_runs_sumW  += sumW_one;
            _prescan_runs_sumW2 += sumW2_one;
            _prescan_runs_count += count_one;
        }
        ++_prescan_nFilesProcessed;
        // unique_ptr<TFile>::Close() happens here at scope exit.
    }

    std::cout << "[Prescan] Runs tree totals: "
              << "Σgenw=" << _prescan_runs_sumW
              << "  Σgenw²=" << _prescan_runs_sumW2
              << "  count=" << _prescan_runs_count
              << "  (over " << _prescan_nFilesProcessed << " files)" << std::endl;
}

// ----------------------------------------------------------------------------

void ttHHanalyzer_unified::accumulatePrescanEvent() {
    // Treat Data as gw=1 so nEvents_total is still meaningful.
    const double gw = (_DataOrMC != "Data") ? _ev->genWeight : 1.0;

    ++_prescan_nEvents_total;
    _prescan_sumGW_total += gw;
    if      (gw > 0) _prescan_sumGW_pos += gw;
    else if (gw < 0) _prescan_sumGW_neg += gw;

    if (_DataOrMC == "Data") return;

    // --- 11-bin raw genTtbarId %% 100 (stitching-partition input) ---
    //     Σgenw bins and unweighted-count bins are accumulated together.
    const int gtid = _ev->genTtbarId;
    // [tt+nb] resolve the expanded id; tt+nb events (sub 61/62/71/72) are routed
    // to their own bins and REMOVED from the 53/54/55 bins, so 53/54/55 become
    // the pure tt+bb remainder. Lookup is injected in main() regardless of mode.
    const int eid  = _expTtbarId.resolve(_ev->run, _ev->luminosityBlock,
                                         _ev->event, gtid);
    const int esub = ((eid % 100) + 100) % 100;
    if (gtid < 0) {
        _prescan_sumGW_id_lt0 += gw; ++_prescan_n_id_lt0;
    } else if (esub == 61) { _prescan_sumGW_id_61 += gw; ++_prescan_n_id_61;
    } else if (esub == 62) { _prescan_sumGW_id_62 += gw; ++_prescan_n_id_62;
    } else if (esub == 71) { _prescan_sumGW_id_71 += gw; ++_prescan_n_id_71;
    } else if (esub == 72) { _prescan_sumGW_id_72 += gw; ++_prescan_n_id_72;
    } else {
        switch (gtid % 100) {
            case  0: _prescan_sumGW_id_0     += gw; ++_prescan_n_id_0;     break;
            case 41: _prescan_sumGW_id_41    += gw; ++_prescan_n_id_41;    break;
            case 42: _prescan_sumGW_id_42    += gw; ++_prescan_n_id_42;    break;
            case 43: _prescan_sumGW_id_43    += gw; ++_prescan_n_id_43;    break;
            case 44: _prescan_sumGW_id_44    += gw; ++_prescan_n_id_44;    break;
            case 45: _prescan_sumGW_id_45    += gw; ++_prescan_n_id_45;    break;
            case 51: _prescan_sumGW_id_51    += gw; ++_prescan_n_id_51;    break;
            case 52: _prescan_sumGW_id_52    += gw; ++_prescan_n_id_52;    break;
            case 53: _prescan_sumGW_id_53    += gw; ++_prescan_n_id_53;    break;
            case 54: _prescan_sumGW_id_54    += gw; ++_prescan_n_id_54;    break;
            case 55: _prescan_sumGW_id_55    += gw; ++_prescan_n_id_55;    break;
            default: _prescan_sumGW_id_other += gw; ++_prescan_n_id_other; break;
        }
    }

    // --- 5-bucket ttCat (ntuple primary) cross-check ---
    // Sanity: sum over id-bins must equal the corresponding ttCat-bucket sum
    // (within float rounding). If not, ntuplizer↔analyzer categorization
    // disagreement — see ttHHanalyzer_unified.h::readNtuplePrimaryCategory.
    const TtCat cat = readNtuplePrimaryCategory();
    switch (cat) {
        case TtCat::kLightFlavour:
            _prescan_sumGW_ttCat_LF   += gw; ++_prescan_n_ttCat_LF;   break;
        case TtCat::kAddCjet:
            _prescan_sumGW_ttCat_Cj   += gw; ++_prescan_n_ttCat_Cj;   break;
        case TtCat::kAdd1Bjet1Had:
            _prescan_sumGW_ttCat_1B1H += gw; ++_prescan_n_ttCat_1B1H; break;
        case TtCat::kAdd1Bjet2Had:
            _prescan_sumGW_ttCat_1B2H += gw; ++_prescan_n_ttCat_1B2H; break;
        case TtCat::kAdd2Bjet:
            _prescan_sumGW_ttCat_2B   += gw; ++_prescan_n_ttCat_2B;   break;
        case TtCat::kNoTTJets:
            _prescan_sumGW_ttCat_NoTT += gw; ++_prescan_n_ttCat_NoTT; break;
        default: break;   // kUnknown → leave uncounted (will show up as diff)
    }
}

// ----------------------------------------------------------------------------

void ttHHanalyzer_unified::writePrescanTree() {
    _of->file->cd();

    TTree* nct = new TTree("prescan",
        "Per-sample gen-level normalization check (1 row per file; "
        "hadd-friendly — post-process to compute r_B, r_4b).");

    // String branches need local std::string variables addressable from TBranch
    std::string sName  = _sampleName;
    std::string rYear  = _runYear;
    std::string dEra   = _era;
    bool        isData = (_DataOrMC == "Data");
    double      xsec_used = _baseWeight;   // analyzer's effective per-event base

    nct->Branch("sampleName",      &sName);
    nct->Branch("runYear",         &rYear);
    nct->Branch("dataEra",         &dEra);
    nct->Branch("isData",          &isData,            "isData/O");
    nct->Branch("xsec_used",       &xsec_used,         "xsec_used/D");
    nct->Branch("nFiles",          &_prescan_nFilesProcessed, "nFiles/I");

    // Events tree direct sums (skim-affected)
    nct->Branch("nEvents_total",   &_prescan_nEvents_total, "nEvents_total/L");
    nct->Branch("sumGenW_total",   &_prescan_sumGW_total,   "sumGenW_total/D");
    nct->Branch("sumGenW_pos",     &_prescan_sumGW_pos,     "sumGenW_pos/D");
    nct->Branch("sumGenW_neg",     &_prescan_sumGW_neg,     "sumGenW_neg/D");

    // NanoAOD Runs tree (skim-independent)
    nct->Branch("genEventSumw_runs",  &_prescan_runs_sumW,  "genEventSumw_runs/D");
    nct->Branch("genEventSumw2_runs", &_prescan_runs_sumW2, "genEventSumw2_runs/D");
    nct->Branch("genEventCount_runs", &_prescan_runs_count, "genEventCount_runs/L");

    // Cross-check: skim attrition (Events - Runs). Usually ≤ 0 (skim removed events).
    double diff = _prescan_sumGW_total - _prescan_runs_sumW;
    nct->Branch("diff_eventsVsRuns",  &diff,             "diff_eventsVsRuns/D");

    // 11-bin raw genTtbarId %% 100
    nct->Branch("sumGenW_id_lt0",   &_prescan_sumGW_id_lt0,   "sumGenW_id_lt0/D");
    nct->Branch("sumGenW_id_0",     &_prescan_sumGW_id_0,     "sumGenW_id_0/D");
    nct->Branch("sumGenW_id_41",    &_prescan_sumGW_id_41,    "sumGenW_id_41/D");
    nct->Branch("sumGenW_id_42",    &_prescan_sumGW_id_42,    "sumGenW_id_42/D");
    nct->Branch("sumGenW_id_43",    &_prescan_sumGW_id_43,    "sumGenW_id_43/D");
    nct->Branch("sumGenW_id_44",    &_prescan_sumGW_id_44,    "sumGenW_id_44/D");
    nct->Branch("sumGenW_id_45",    &_prescan_sumGW_id_45,    "sumGenW_id_45/D");
    nct->Branch("sumGenW_id_51",    &_prescan_sumGW_id_51,    "sumGenW_id_51/D");
    nct->Branch("sumGenW_id_52",    &_prescan_sumGW_id_52,    "sumGenW_id_52/D");
    nct->Branch("sumGenW_id_53",    &_prescan_sumGW_id_53,    "sumGenW_id_53/D");
    nct->Branch("sumGenW_id_54",    &_prescan_sumGW_id_54,    "sumGenW_id_54/D");
    nct->Branch("sumGenW_id_55",    &_prescan_sumGW_id_55,    "sumGenW_id_55/D");
    // [tt+nb] expanded partition (tt+nb removed from 53/54/55 above)
    nct->Branch("sumGenW_id_61",    &_prescan_sumGW_id_61,    "sumGenW_id_61/D");
    nct->Branch("sumGenW_id_62",    &_prescan_sumGW_id_62,    "sumGenW_id_62/D");
    nct->Branch("sumGenW_id_71",    &_prescan_sumGW_id_71,    "sumGenW_id_71/D");
    nct->Branch("sumGenW_id_72",    &_prescan_sumGW_id_72,    "sumGenW_id_72/D");
    nct->Branch("sumGenW_id_other", &_prescan_sumGW_id_other, "sumGenW_id_other/D");

    // 11-bin raw genTtbarId %% 100 — unweighted event counts
    // (companion to sumGenW_id_*; per-category entry counts for stitching bookkeeping)
    nct->Branch("n_id_lt0",   &_prescan_n_id_lt0,   "n_id_lt0/L");
    nct->Branch("n_id_0",     &_prescan_n_id_0,     "n_id_0/L");
    nct->Branch("n_id_41",    &_prescan_n_id_41,    "n_id_41/L");
    nct->Branch("n_id_42",    &_prescan_n_id_42,    "n_id_42/L");
    nct->Branch("n_id_43",    &_prescan_n_id_43,    "n_id_43/L");
    nct->Branch("n_id_44",    &_prescan_n_id_44,    "n_id_44/L");
    nct->Branch("n_id_45",    &_prescan_n_id_45,    "n_id_45/L");
    nct->Branch("n_id_51",    &_prescan_n_id_51,    "n_id_51/L");
    nct->Branch("n_id_52",    &_prescan_n_id_52,    "n_id_52/L");
    nct->Branch("n_id_53",    &_prescan_n_id_53,    "n_id_53/L");
    nct->Branch("n_id_54",    &_prescan_n_id_54,    "n_id_54/L");
    nct->Branch("n_id_55",    &_prescan_n_id_55,    "n_id_55/L");
    // [tt+nb] expanded partition counts
    nct->Branch("n_id_61",    &_prescan_n_id_61,    "n_id_61/L");
    nct->Branch("n_id_62",    &_prescan_n_id_62,    "n_id_62/L");
    nct->Branch("n_id_71",    &_prescan_n_id_71,    "n_id_71/L");
    nct->Branch("n_id_72",    &_prescan_n_id_72,    "n_id_72/L");
    nct->Branch("n_id_other", &_prescan_n_id_other, "n_id_other/L");

    // 5-bucket ttCat (ntuple primary) — Σ genWeight
    nct->Branch("sumGenW_ttCat_LightFlavour",  &_prescan_sumGW_ttCat_LF,   "sumGenW_ttCat_LightFlavour/D");
    nct->Branch("sumGenW_ttCat_AddCjet",       &_prescan_sumGW_ttCat_Cj,   "sumGenW_ttCat_AddCjet/D");
    nct->Branch("sumGenW_ttCat_Add1Bjet_1Had", &_prescan_sumGW_ttCat_1B1H, "sumGenW_ttCat_Add1Bjet_1Had/D");
    nct->Branch("sumGenW_ttCat_Add1Bjet_2Had", &_prescan_sumGW_ttCat_1B2H, "sumGenW_ttCat_Add1Bjet_2Had/D");
    nct->Branch("sumGenW_ttCat_Add2Bjet",      &_prescan_sumGW_ttCat_2B,   "sumGenW_ttCat_Add2Bjet/D");
    nct->Branch("sumGenW_ttCat_NoTT",          &_prescan_sumGW_ttCat_NoTT, "sumGenW_ttCat_NoTT/D");

    // 5-bucket ttCat — unweighted counts
    nct->Branch("n_ttCat_LightFlavour",  &_prescan_n_ttCat_LF,   "n_ttCat_LightFlavour/L");
    nct->Branch("n_ttCat_AddCjet",       &_prescan_n_ttCat_Cj,   "n_ttCat_AddCjet/L");
    nct->Branch("n_ttCat_Add1Bjet_1Had", &_prescan_n_ttCat_1B1H, "n_ttCat_Add1Bjet_1Had/L");
    nct->Branch("n_ttCat_Add1Bjet_2Had", &_prescan_n_ttCat_1B2H, "n_ttCat_Add1Bjet_2Had/L");
    nct->Branch("n_ttCat_Add2Bjet",      &_prescan_n_ttCat_2B,   "n_ttCat_Add2Bjet/L");
    nct->Branch("n_ttCat_NoTT",          &_prescan_n_ttCat_NoTT, "n_ttCat_NoTT/L");

    nct->Fill();
    nct->Write();

    // --- Console summary ---
    std::cout << "\n══════════════════════════════════════════════════════════════════\n";
    std::cout << "[Prescan] Summary — " << _sampleName << "\n";
    std::cout << "------------------------------------------------------------------\n";
    std::cout << "  nEvents (Events tree)  : " << _prescan_nEvents_total << "\n";
    std::cout << "  Σgenw  (Events tree)   : " << _prescan_sumGW_total
              << "   (pos=" << _prescan_sumGW_pos
              << ", neg=" << _prescan_sumGW_neg << ")\n";
    std::cout << "  Σgenw  (Runs tree)     : " << _prescan_runs_sumW << "\n";
    std::cout << "  Σgenw² (Runs tree)     : " << _prescan_runs_sumW2 << "\n";
    std::cout << "  count  (Runs tree)     : " << _prescan_runs_count << "\n";
    std::cout << "  diff (Events - Runs)   : " << diff
              << "   ← non-zero = skim attrition (expected if ntuplizer skimmed)\n";

    if (_DataOrMC != "Data") {
        const double sum_cc = _prescan_sumGW_id_41 + _prescan_sumGW_id_42 + _prescan_sumGW_id_43
                            + _prescan_sumGW_id_44 + _prescan_sumGW_id_45;
        std::cout << "  --- by genTtbarId %% 100 (raw, weighted) ---\n";
        std::cout << "    id < 0    (no/none)  : " << _prescan_sumGW_id_lt0   << "\n";
        std::cout << "    id =  0   (LF)       : " << _prescan_sumGW_id_0     << "\n";
        std::cout << "    id 41-45  (cc total) : " << sum_cc << "\n";
        std::cout << "    id = 51   (1b/1H)    : " << _prescan_sumGW_id_51    << "\n";
        std::cout << "    id = 52   (1b/2H)    : " << _prescan_sumGW_id_52    << "\n";
        std::cout << "    id = 53   (2b)       : " << _prescan_sumGW_id_53    << "\n";
        std::cout << "    id = 54   (bb)       : " << _prescan_sumGW_id_54    << "\n";
        std::cout << "    id = 55   (4b)       : " << _prescan_sumGW_id_55    << "\n";
        std::cout << "    id  other            : " << _prescan_sumGW_id_other << "\n";
        const Long64_t sum_n_cc = _prescan_n_id_41 + _prescan_n_id_42 + _prescan_n_id_43
                                + _prescan_n_id_44 + _prescan_n_id_45;
        std::cout << "  --- by genTtbarId %% 100 (raw, unweighted event counts) ---\n";
        std::cout << "    n id < 0  (no/none)  : " << _prescan_n_id_lt0   << "\n";
        std::cout << "    n id =  0 (LF)       : " << _prescan_n_id_0     << "\n";
        std::cout << "    n id 41-45 (cc total): " << sum_n_cc << "\n";
        std::cout << "    n id = 51 (1b/1H)    : " << _prescan_n_id_51    << "\n";
        std::cout << "    n id = 52 (1b/2H)    : " << _prescan_n_id_52    << "\n";
        std::cout << "    n id = 53 (2b)       : " << _prescan_n_id_53    << "\n";
        std::cout << "    n id = 54 (bb)       : " << _prescan_n_id_54    << "\n";
        std::cout << "    n id = 55 (4b)       : " << _prescan_n_id_55    << "\n";
        std::cout << "    n id  other          : " << _prescan_n_id_other << "\n";
        std::cout << "  --- ntuple ttCat cross-check (Σ genw should match id sums) ---\n";
        std::cout << "    LF       : " << _prescan_sumGW_ttCat_LF
                  << "   (n=" << _prescan_n_ttCat_LF << ")\n";
        std::cout << "    AddCjet  : " << _prescan_sumGW_ttCat_Cj
                  << "   (n=" << _prescan_n_ttCat_Cj << ")\n";
        std::cout << "    1B/1H    : " << _prescan_sumGW_ttCat_1B1H
                  << "   (n=" << _prescan_n_ttCat_1B1H << ")\n";
        std::cout << "    1B/2H    : " << _prescan_sumGW_ttCat_1B2H
                  << "   (n=" << _prescan_n_ttCat_1B2H << ")\n";
        std::cout << "    Add2Bjet : " << _prescan_sumGW_ttCat_2B
                  << "   (n=" << _prescan_n_ttCat_2B << ")\n";
        std::cout << "    NoTT     : " << _prescan_sumGW_ttCat_NoTT
                  << "   (n=" << _prescan_n_ttCat_NoTT << ")\n";

        // Inline cross-check warnings (loose tolerance — float rounding)
        const double tol = 1e-3 * std::max(1.0, std::abs(_prescan_sumGW_total));
        if (std::abs(_prescan_sumGW_id_0 - _prescan_sumGW_ttCat_LF) > tol) {
            std::cerr << "[Prescan][WARN] id=0 (" << _prescan_sumGW_id_0
                      << ") != ttCat_LF (" << _prescan_sumGW_ttCat_LF << ")\n";
        }
        if (std::abs(sum_cc - _prescan_sumGW_ttCat_Cj) > tol) {
            std::cerr << "[Prescan][WARN] id∈41-45 (" << sum_cc
                      << ") != ttCat_AddCjet (" << _prescan_sumGW_ttCat_Cj << ")\n";
        }
        if (std::abs(_prescan_sumGW_id_51 - _prescan_sumGW_ttCat_1B1H) > tol) {
            std::cerr << "[Prescan][WARN] id=51 vs ttCat_1B1H drift\n";
        }
        if (std::abs(_prescan_sumGW_id_52 - _prescan_sumGW_ttCat_1B2H) > tol) {
            std::cerr << "[Prescan][WARN] id=52 vs ttCat_1B2H drift\n";
        }
        const double sum_2b_or_more = _prescan_sumGW_id_53 + _prescan_sumGW_id_54 + _prescan_sumGW_id_55;
        if (std::abs(sum_2b_or_more - _prescan_sumGW_ttCat_2B) > tol) {
            std::cerr << "[Prescan][WARN] id∈53-55 (" << sum_2b_or_more
                      << ") != ttCat_Add2Bjet (" << _prescan_sumGW_ttCat_2B << ")\n";
        }
    }
    std::cout << "══════════════════════════════════════════════════════════════════\n";
}


// =============================================================================
// main() — entry point for the ttHH(4b) FH analyzer
// -----------------------------------------------------------------------------
// All argument parsing is done by tnm.cc::commandLine::decode, including the
// optional --val-* flags used by the kValidationStudy mode. The fields read
// here are added in tnm.h (see commandLine struct) and parsed in tnm.cc
// (see decode()).
//
// For non-validation modes the val* fields hold their default values (which
// reproduce kMainAnalysis behaviour exactly), so the validation block below
// is harmless even when entered defensively.
// =============================================================================
int main(int argc, char** argv){

    commandLine cl(argc, argv);
    vector<string> filenames = fileNames(cl.filelist);
    double weight = cl.externalweight;   // global per-sample weight

    // ─────────────────────────────────────────────────────────────────────
    // 1. Parse and validate analysis mode
    // ─────────────────────────────────────────────────────────────────────
    AnalysisMode mode;
    try {
        mode = parseAnalysisMode(cl.analysisMode);
    }
    catch (const std::invalid_argument& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // ─────────────────────────────────────────────────────────────────────
    // 2. Echo arguments for log
    // ─────────────────────────────────────────────────────────────────────
    std::cout << "\n--------- Check arguments ---------------------------------------\n" << std::endl;
    std::cout << "  - [ output file name ] --> " << cl.outputfilename << std::endl;
    std::cout << "  - [ runYear -string- ] --> " << cl.runYear << std::endl;
    std::cout << "  - [ DataOrMC -string- ] --> " << cl.DataOrMC << std::endl;
    std::cout << "  - [ sampleName ] --> " << cl.sampleName << std::endl;
    std::cout << "  - [ eraName ] --> " << cl.eraName << std::endl;
    std::cout << "  - [ analysisMode ] --> " << analysisModeName(mode) << std::endl;
    std::cout << "\n--------- Check arguments ---------------------------------------\n" << std::endl;

    // ─────────────────────────────────────────────────────────────────────
    // 3. Open input ntuple stream
    // ─────────────────────────────────────────────────────────────────────
    itreestream stream(filenames, "Events");
    if ( !stream.good() ) error("can't read root input files");
    eventBuffer ev(stream);
    std::cout << " Output filename: " << cl.outputfilename << std::endl;

    // ─────────────────────────────────────────────────────────────────────
    // 4. Construct the analyzer instance
    //    constructor signature:
    //      (outFile, eventBuffer, weight, sysToggle, year, dataOrMC,
    //       sampleName, era, debug, mode)
    // ─────────────────────────────────────────────────────────────────────
    bool debugVerbose = false;
    ttHHanalyzer_unified analysis(
        cl.outputfilename,
        &ev,
        weight,
        true,               // legacy systematics toggle
        cl.runYear,
        cl.DataOrMC,
        cl.sampleName,
        cl.eraName,
        debugVerbose,
        mode
    );

    // ─────────────────────────────────────────────────────────────────────
    // [tt+nb] Load the per-sample Expanded_genTtbarId lookup from the project's
    // DerivedCorr/expandedTtbarId/ directory: it picks ttnb_<sampleName>.root
    // automatically. Override the directory with EXPANDED_TTBARID_DIR (e.g. an
    // absolute path on a worker). Samples with no matching file -> INACTIVE
    // (NanoAOD genTtbarId used; correct for Data and non-ttbar samples).
    // Active in BOTH kPrescan and the main loop (it is set before
    // performAnalysis), so the prescan partition sees the same lookup.
    // ─────────────────────────────────────────────────────────────────────
    {
        const char* d = std::getenv("EXPANDED_TTBARID_DIR");
        analysis.setExpandedTtbarIdDir(d ? std::string(d)
                                         : std::string("DerivedCorr/expandedTtbarId"));
    }

    // ─────────────────────────────────────────────────────────────────────
    // [stitch] Load the ttbar stitching multiplier JSON ONLY for the two modes
    // that should see the stitched composition: main (final yields) and btagtrig
    // (b-tag norm reweight context). trigsf / validation / prescan are left
    // stitch-free on purpose:
    //   - prescan PRODUCES the inputs this JSON is computed from;
    //   - the TRIGGER SF must be derived (trigsf) on an un-stitched sample so it
    //     stays a pure efficiency ratio, independent of the stitch/JSON;
    //   - validation toggles its own SFs.
    // Override path with $STITCH_FACTORS_JSON. Missing/garbled -> fatal-exit.
    if (mode == AnalysisMode::kMainAnalysis ||
        mode == AnalysisMode::kBTagAndTriggerStudy) {
        const char* sj = std::getenv("STITCH_FACTORS_JSON");
        analysis.setStitchFactorsFile(
            sj ? std::string(sj)
               : std::string("DerivedCorr/stitchFactors/stitch_factors_2017.json"));
    }

    // ─────────────────────────────────────────────────────────────────────
    // 5. [Validation Study] — push --val-* flags into the analyzer
    //
    // tnm.cc::commandLine::decode has already read every --val-* flag and
    // stored them in cl.val* fields. Defaults reproduce kMainAnalysis
    // behaviour, so a user who passes no flags still gets a sane run.
    //
    // Block runs only when --mode validation; ignored otherwise.
    // ─────────────────────────────────────────────────────────────────────
    if (mode == AnalysisMode::kValidationStudy) {
        ValidationConfig vcfg;
        vcfg.scenarioName            = cl.valScenario;
        vcfg.nbJetsCut               = cl.valNbJetsCut;
        vcfg.forceHiggsRecoMinBjets  = cl.valHRecoMin;
        vcfg.applyHadWWindow         = (cl.valApplyHadW      != 0);
        vcfg.applyHiggsWindow        = (cl.valApplyHiggsWin  != 0);
        vcfg.tightenJet8             = (cl.valTightenJet8    != 0);
        vcfg.applyBtagShapeSF        = (cl.valApplyBtagShape != 0);
        vcfg.applyBtagNormSF         = (cl.valApplyBtagNorm  != 0);
        vcfg.applyTriggerSF          = (cl.valApplyTrig      != 0);
        vcfg.applyTopPtSF            = (cl.valApplyTopPt     != 0);
        vcfg.ttHVRStyle              = (cl.valTtHVRStyle     != 0);

        // ttH VR preset overrides individual fields when requested.
        // Preserve the user-supplied scenario tag for log/output naming.
        if (vcfg.ttHVRStyle) {
            const std::string preservedName = vcfg.scenarioName;
            vcfg.applyTtHVRPreset();   // resets fields, sets scenarioName="ttHVR"
            if (!preservedName.empty() && preservedName != "default") {
                vcfg.scenarioName = preservedName;
            }
        }

        // Push effective config; setValidationConfig prints it to stdout.
        analysis.setValidationConfig(vcfg);
    }

    // ─────────────────────────────────────────────────────────────────────
    // 6. Run the analysis
    // ─────────────────────────────────────────────────────────────────────
    if(debugVerbose) std::cout<<"debug : Before [ performAnalysis ] in main() function"<<std::endl;
    analysis.performAnalysis();
    if(debugVerbose) std::cout<<"debug : After [ performAnalysis() ] and Before [ ev.close() ].. in main() function"<<std::endl;
    ev.close();
    if(debugVerbose) std::cout<<"debug : After [ ev.close() ] in main() function"<<std::endl;

    return 0;
}
