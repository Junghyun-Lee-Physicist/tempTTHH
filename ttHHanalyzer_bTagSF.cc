#include "tnm.h"
#include <cmath> 
#include <algorithm>
#include <vector>
#include <map>
#include "TVector3.h"
#include "ttHHanalyzer_bTagSF.h"
#include <iostream>

#include "correction.h"

#include "Logger.h"
using namespace Logger;
 
void ttHHanalyzer_bTagSF::performAnalysis(){

    loop(noSys, false);
    _of->file->Close();
}

void ttHHanalyzer_bTagSF::loop(sysName sysType, bool up){

    int nevents = _ev->size();
////    nevents = 1000;

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

    hCutFlow->Write();
    if(debugCorrections) std::cout<<"debug : After hCutFlow() & Before hCutFlow_w()"<<std::endl;
    hCutFlow_w->Write();
    if(debugCorrections) std::cout<<"debug : After hCutFlow_w()"<<std::endl;

}

void ttHHanalyzer_bTagSF::createObjects(event * thisEvent, sysName sysType, bool up){

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
    int nVetoMuons = 0, nVetoEle = 0;
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
// Lepton definition for BTagSF
    bool thereIsALeadLepton = false;

    for(int i = 0; i < muonT.size(); i++){
        // CHECK: 현재 Veto Muon으로 TightID를 사용 중. 일반적으로 Veto 용도로는 LooseID를 권장함.
        // Muon POG 권장사항 확인 필요 (예: LooseID + LooseIso).
        // TightID 사용 시 "Loose하지만 가짜는 아닌" 뮤온을 놓쳐서 Hadronic 채널 오염 가능성 있음.
        if(fabs(muonT[i].eta) < cut["muonEta"] && muonT[i].tightId == true && muonT[i].pfRelIso04_all  < cut["muonIso"]){
            if(muonT[i].pt > cut["leadMuonPt"]){
                thereIsALeadLepton = true;
                break;
            }
        }
    }
    if(!thereIsALeadLepton){
        for(int i = 0; i < ele.size(); i++){
            // CHECK: Electron Veto 역시 WP90(Tight에 가까움) 사용 중. Egamma POG의 Veto WP 권장사항 확인 필요.
            if(fabs(ele[i].deltaEtaSC + ele[i].eta) < 1.4442 || fabs(ele[i].deltaEtaSC + ele[i].eta) > 1.5660){  //Electrons tracked neither in the barrel nor in the endcap are discarded.
                if(fabs(ele[i].eta) < cut["eleEta"] && ele[i].mvaFall17V2Iso_WP90 == true && ele[i].pfRelIso03_all  < cut["eleIso"]){ 
                    if(ele[i].pt > cut["leadElePt"]){
                        thereIsALeadLepton = true;
                        break;
                    }
                }
            }
        }
    }

    for(int i = 0; i < muonT.size(); i++){
        if(fabs(muonT[i].eta) < cut["muonEta"] && muonT[i].tightId == true && muonT[i].pfRelIso04_all  < cut["muonIso"]){
            if(muonT[i].pt > cut["subLeadMuonPt"]){
                nVetoMuons += 1;
            }
        }
    }
    for(int i = 0; i < ele.size(); i++){
        if(fabs(ele[i].deltaEtaSC + ele[i].eta) < 1.4442 || fabs(ele[i].deltaEtaSC + ele[i].eta) > 1.5660){  //Electrons tracked neither in the barrel nor in the endcap are discarded.
            if(fabs(ele[i].eta) < cut["eleEta"] && ele[i].mvaFall17V2Iso_WP90 == true && ele[i].pfRelIso03_all  < cut["eleIso"]){ 
                if(ele[i].pt > cut["subLeadElePt"]){
                    nVetoEle += 1;
                }
            }
        }
    }


    // Leading lepton def
    // But FH channel don't need this..
    // We just use subleading lepton def for veto
    // update in 5th Jan, 2026
     if(thereIsALeadLepton){ //we can add all leptons passing to the sublead selection to our containers
         for(int i = 0; i < muonT.size(); i++){
             if(fabs(muonT[i].eta) < cut["muonEta"] && muonT[i].tightId == true && muonT[i].pfRelIso04_all < cut["muonIso"]){
             //	    if(fabs(muonT[i].eta) < cut["muonEta"] && muonT[i].mvaTTH > 0.15 && muonT[i].pfRelIso04_all  < cut["muonIso"]){	
         	if(muonT[i].pt > cut["subLeadMuonPt"]){
         	    currentMuon = new objectLep(muonT[i].pt, muonT[i].eta, muonT[i].phi, 0.);
         	    currentMuon->charge = muonT[i].charge;
         	    currentMuon->miniPFRelIso = muonT[i].miniPFRelIso_all;
         	    currentMuon->pfRelIso04 = muonT[i].pfRelIso04_all;
         	    thisEvent->selectMuon(currentMuon);
         	}
             }
         }
         for(int i = 0; i < ele.size(); i++){
             if(fabs(ele[i].deltaEtaSC + ele[i].eta) < 1.4442 || fabs(ele[i].deltaEtaSC + ele[i].eta) > 1.5660){  //Electrons tracked neither in the barrel nor in the endcap are discarded.
         	      if(fabs(ele[i].eta) < cut["eleEta"] && ele[i].mvaFall17V2Iso_WP90 == true && ele[i].pfRelIso03_all  < cut["eleIso"]){ 
                  if(ele[i].pt > cut["subLeadElePt"]){
         		currentEle = new objectLep(ele[i].pt, ele[i].eta, ele[i].phi, 0.);	 
         		currentEle->charge = ele[i].charge;
         		currentEle->miniPFRelIso = ele[i].miniPFRelIso_all;
         		currentEle->pfRelIso03 = ele[i].pfRelIso03_all;
         		thisEvent->selectEle(currentEle);
         	      }
               }
         	}
         }
     }
    thisEvent->orderLeptons();
    thisEvent->setnVetoLepton(nVetoMuons + nVetoEle);


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
            float genPt = -1.f;
            if (jetRaw.genJetIdx >= 0 && jetRaw.genJetIdx < (int)genJet.size()) {
                genPt = genJet[jetRaw.genJetIdx].pt;
            }

            double smearedPt = corrMgr->smearJER(ptJEC, genPt, jetRaw.eta, rho, "nom");

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
            massJEC
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
           



bool ttHHanalyzer_bTagSF::selectObjects(event *thisEvent){
     
    auto processStep = [&](CutStep step, float wMassVal) {
        int idx = static_cast<int>(step);
        
        if (idx < _cutFlowCount.size()) {
            _cutFlowCount[idx] += 1.0;
            _cutFlowWeight[idx] += _evtWeight; // [��] _weight -> _evtWeight
        }

        hCutFlow->Fill(idx); 
        hCutFlow_w->Fill(idx, _evtWeight);

        fillCutStepHist(step, thisEvent, wMassVal);
    };


    // ----------------------------------------------------
    // �� � �� �� ��
    // ----------------------------------------------------

    const float wMass = 80.377f;
    float hadWMass = closestMassPair(
        thisEvent->getSelLightJets()->size() >= 2 ? thisEvent->getSelLightJets() : thisEvent->getSelJets(),
        wMass
    );

    // Higgs Reconstruction (Histogram ����)
    _minChi2Higgs = cLargeValue;
    _bbMassMin1Higgs = -1.0f;
    _bbMassMin2Higgs = -1.0f;


    auto* bjets = thisEvent->getSelbJets();
    // ttHH(bb) Hadronic Channel 연구이므로, Higgs -> bb 붕괴를 재구성하기 위해
    // 최소 4개의 b-jet이 존재해야만 Higgs Pair Candidate를 생성할 수 있음.
    // 따라서 아래 조건문(size >= 4)은 분석의 필수 조건임.
    if (bjets->size() >= 4) {
        for (size_t i = 0; i < bjets->size() - 3; ++i) {
            for (size_t j = i + 1; j < bjets->size() - 2; ++j) {
                for (size_t k = j + 1; k < bjets->size() - 1; ++k) {
                    for (size_t l = k + 1; l < bjets->size(); ++l) {
                        diMotherReco(*bjets->at(i)->getp4(), *bjets->at(j)->getp4(),
                                     *bjets->at(k)->getp4(), *bjets->at(l)->getp4(),
                                     cHiggsMass, cHiggsMass, _minChi2Higgs, _bbMassMin1Higgs, _bbMassMin2Higgs);
                        diMotherReco(*bjets->at(i)->getp4(), *bjets->at(k)->getp4(),
                                     *bjets->at(j)->getp4(), *bjets->at(l)->getp4(),
                                     cHiggsMass, cHiggsMass, _minChi2Higgs, _bbMassMin1Higgs, _bbMassMin2Higgs);
                        diMotherReco(*bjets->at(i)->getp4(), *bjets->at(l)->getp4(),
                                     *bjets->at(j)->getp4(), *bjets->at(k)->getp4(),
                                     cHiggsMass, cHiggsMass, _minChi2Higgs, _bbMassMin1Higgs, _bbMassMin2Higgs);
                    }
                }
            }
        }
    }

    // This is the logic of choosing single higgs mass
    // It could be right but we need to think about it..
    // in 5th Jan 2026
////    float higgsMass = -1.0f;
////    if (_bbMassMin1Higgs > 0.0f && _bbMassMin2Higgs > 0.0f) {
////        higgsMass = (std::fabs(_bbMassMin1Higgs - cHiggsMass) < std::fabs(_bbMassMin2Higgs - cHiggsMass))
////                        ? _bbMassMin1Higgs
////                        : _bbMassMin2Higgs;
////    }

    // Step 0: No Cut
    processStep(CutStep::kNoCut, hadWMass);

    // Step 1: Trigger    
// Commented out for BTagSF
////    if(cut["trigger"] > 0 && thisEvent->getHadTriggerAccept() == false){
////        return false;
////    }
    processStep(CutStep::kHadTrigger, hadWMass);

    // Step 2: Noise Filter
    if(cut["filter"] > 0 && thisEvent->getMETFilter() == false){
        return false;
    }
    processStep(CutStep::kNoiseFilter, hadWMass);

    ////////if(cut["trigger"] > 0 && thisEvent->getMuonTriggerAccept() == false)
    ////////{
    ////////    return false;
    ////////}
    ////////cutflow["MuonTrigger"]+=1;                 
    ////////hCutFlow->Fill("MuonTrigger",1);
    ////////hCutFlow_w->Fill("MuonTrigger",_weight);

    // Step 3: Primary Vertex
    if(cut["pv"] > 0 && thisEvent->getPVvalue() == false){
        return false;
    }
    processStep(CutStep::kPrimaryVertex, hadWMass);


    // Step 4: nJets >= 7
    if(!(thisEvent->getnSelJet() >= cut["nJets"] )){
        return false;
    }
    processStep(CutStep::kNumJets, hadWMass);

    // Step 5: 6th Jet Pt > 40
    if(!(thisEvent->getSelJets()->at(5)->getp4()->Pt() > cut["6thJetsPT"])){
        return false;
    }
    processStep(CutStep::kSixthJetPt, hadWMass);

    // Step 6: Lepton Veto (nLepton == 0)
// Commented out for BTagSF
////    if(!(thisEvent->getnVetoLepton() == cut["nLeptons"])){
////        return false;
////    }
    processStep(CutStep::kLeptonVeto, hadWMass);

    // (��: �� �� �� ��)
    thisEvent->getStatsComb(thisEvent->getSelJets(), thisEvent->getSelLeptons(), ljetStat);
    thisEvent->getStatsComb(thisEvent->getSelbJets(), thisEvent->getSelLeptons(), lbjetStat);

    // Step 7: HT > 500
    if(!(thisEvent->getSumSelJetScalarpT() > cut["HT"])){
        return false;
    }
    processStep(CutStep::kHT, hadWMass);


    // Step 8: nbJets >= 4
// Commented out for BTagSF
////    if(!(thisEvent->getnSelbJet() >= cut["nbJets"] )){
////        return false;
////    }
    processStep(CutStep::kNumbJets, hadWMass);


    // Step 9: Hadronic W Mass
// Commented out for BTagSF
////    if (hadWMass < 0.0f || hadWMass > 250.0f || hadWMass < 30.0f) {
////        return false;
////    }
    processStep(CutStep::kHadWMass, hadWMass);


    // Step 10: Higgs Mass Window (�� ���� � ���� pass)
    // We do not use higgs window right now.. in 05th Jan 2026
////    const float higgsMassMin = 90.0f;
////    const float higgsMassMax = 160.0f;
////    if (_bbMassMin1Higgs < higgsMassMin || _bbMassMin1Higgs > higgsMassMax ||
////        _bbMassMin2Higgs < higgsMassMin || _bbMassMin2Higgs > higgsMassMax) {
////        return false;
////    }
////    cutflow["HiggsMassWindow"]+=1;
////    hCutFlow->Fill("HiggsMassWindow",1);
////    hCutFlow_w->Fill("HiggsMassWindow",_weight);
////    fillCutStepHist(CutStep::kHiggsMass, thisEvent, hadWMass);
    processStep(CutStep::kHiggsMass, hadWMass);

    
    // Step 11: Total
    processStep(CutStep::kTotal, hadWMass);
	
   
    return true;
}


void ttHHanalyzer_bTagSF::motherReco(const TLorentzVector & dPar1p4,const TLorentzVector & dPar2p4, const float mother1mass, float & _minChi2,float & _bbMassMin1){
    float bbMass1, chi2;
    bbMass1 = (dPar1p4+dPar2p4).M();
    chi2 = pow((bbMass1 - mother1mass),2)/pow((dPar1p4.Pt()+dPar2p4.Pt())/2.*0.02,0.5);
    if(_minChi2 > chi2){
	_minChi2      = chi2;
	_bbMassMin1   = bbMass1;
	_bpTHiggs1    = (dPar1p4+dPar2p4).Pt();
    }
} 


void ttHHanalyzer_bTagSF::diMotherReco(const TLorentzVector & dPar1p4,const TLorentzVector & dPar2p4,const TLorentzVector & dPar3p4,const TLorentzVector & dPar4p4, const float mother1mass, const float  mother2mass, float & _minChi2,float & _bbMassMin1, float & _bbMassMin2){
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

void ttHHanalyzer_bTagSF::analyze(event *thisEvent){

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


void ttHHanalyzer_bTagSF::process(event* thisEvent, sysName sysType, bool up){

    _evtWeight = _baseWeight;

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

    if (_DataOrMC != "Data") {
	
        _evtWeight *= (_PUWeight * _L1PrefiringWeight * _genWeight);
        if(debugCorrections) std::cout << "Final Event Weight: " << _evtWeight << std::endl;
    }

    // 5) Build physics objects in thisEvent from the raw buffer
    createObjects(thisEvent, sysType, up);

    if(!selectObjects(thisEvent))  return;
////    selectObjects(thisEvent);
    _passMETFilters = false;
    _passMETFilters = thisEvent->getMETFilter();

    if(_passMETFilters != true){
        std::cout<<"ERROR : filter is diff!!"<<std::endl;
        exit(555);
    }

    bool metFilters = _ev->Flag_goodVertices &&
                         _ev->Flag_globalSuperTightHalo2016Filter &&
                         _ev->Flag_HBHENoiseFilter &&
                         _ev->Flag_HBHENoiseIsoFilter &&
                         _ev->Flag_EcalDeadCellTriggerPrimitiveFilter &&
                         _ev->Flag_BadPFMuonFilter &&
                         _ev->Flag_BadPFMuonDzFilter &&
                         _ev->Flag_eeBadScFilter &&
                         _ev->Flag_ecalBadCalibFilter;
    if(_passMETFilters != metFilters){
	    std::cout<<"ERROR : filter is diff!!"<<std::endl;
	    exit(555);
    }

    // 5) Final analysis steps: kinematics, histograms, tree
    analyze(thisEvent);
    fillHistos(thisEvent);
    fillTree(thisEvent);

}


void ttHHanalyzer_bTagSF::fillHistos(event * thisEvent){

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



void ttHHanalyzer_bTagSF::writeHistos(){
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
        _cutStepJetPt.at(i)->Write();
        _cutStepJetEta.at(i)->Write();
        _cutStepJetPhi.at(i)->Write();
        _cutStepHT.at(i)->Write();
        _cutStepBTag.at(i)->Write();
        _cutStepHadWMass.at(i)->Write();
        _cutStepHiggsMass01.at(i)->Write();
        _cutStepHiggsMass02.at(i)->Write();
    }
}
void ttHHanalyzer_bTagSF::fillTree(event * thisEvent){

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
 
    SampleWeight = _SampleWeight;
    PUWeight = _PUWeight;
    L1PrefiringWeight = _L1PrefiringWeight;
    genWeight = _genWeight;
    failGoldenJson = _failGoldenJson;
    passMETFilters = _passMETFilters;

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
    //////////passHadTrig = thisEvent->getHadTriggerAccept();

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

void ttHHanalyzer_bTagSF::writeTree(){
    _of->file->cd();
    _treeDirs.at(0)->cd();
    _inputTree->Write();
    //    _inputTree->Delete();
    
}

//----------------------------------------------------------------------------
int main(int argc, char** argv){
    commandLine cl(argc, argv);
    vector<string> filenames = fileNames(cl.filelist);
    double weight = cl.externalweight;   // Get global weight 

    std::cout << "\n--------- Check arugments ---------------------------------------\n" << std::endl;
    std::cout << "  - [ output file name ] --> " << cl.outputfilename << std::endl;
    std::cout << "  - [ runYear -string- ] --> " << cl.runYear << std::endl;
    std::cout << "  - [ DataOrMC -string- ] --> " << cl.DataOrMC << std::endl;
    std::cout << "  - [ sampleName ] --> " << cl.sampleName << std::endl;
    std::cout << "  - [ eraName ] --> " << cl.eraName << std::endl;
    std::cout << "\n--------- Check arugments ---------------------------------------\n" << std::endl;


    // Create tree reader
    itreestream stream(filenames, "Events");
    if ( !stream.good() ) error("can't read root input files");

    eventBuffer ev(stream);
    std::cout << " Output filename: " << cl.outputfilename << std::endl;
    ////ttHHanalyzer_base analysis(cl.outputfilename, &ev, weight, true)
  
    // If you want to check or modify arguments,
    // Please check the [ src/tnm.cc ]
    // Arguments structure --> filelist, outputDirName, weight, Year, Data or MC, sampleName

    bool debugVerbose = false;

    ttHHanalyzer_bTagSF analysis(cl.outputfilename, &ev, weight, true, cl.runYear, cl.DataOrMC, cl.sampleName, cl.eraName, debugVerbose);

    if(debugVerbose) std::cout<<"debug : Before [ performAnalysis ] in main() function"<<std::endl;
    analysis.performAnalysis();
    if(debugVerbose) std::cout<<"debug : After [ performAnalysis() ] and Before [ ev.close() ].. in main() function"<<std::endl;

    ev.close();
    if(debugVerbose) std::cout<<"debug : After [ ev.close() ] in main() function"<<std::endl;

    //of.close();
    if(debugVerbose) std::cout<<"debug : End of main() function"<<std::endl;

    return 0;
}
