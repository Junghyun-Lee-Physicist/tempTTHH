#define makeBTagWeight_cxx
#include "makeBTagWeight.hh"
#include <TH2.h>
#include <TStyle.h>
#include <iostream>

#include "BinConfig.hh"
#include "BTagCalibrationStandalone.h"

#include "SampleInfoManager.h"

void makeBTagWeight::Loop()
{
    if (fChain == 0) return;

    TH1::AddDirectory(kFALSE);

    // 입력 파일 이름으로부터 샘플 이름과 데이터 여부를 결정
    TString sampleName = getInputName();
    Ssiz_t rootPos = sampleName.Index(".root");
    if (rootPos != kNPOS) {
        sampleName.Remove(rootPos);
    }
    std::cout << "Sample Name: " << sampleName << std::endl;

    // Using sampleInfoManager library to get sample inforation (data or not, weight)

////////////////////////////////////////////////////////////////////////////

// sample info manager
    SampleInfoManager::instance().setCsvFileName("./sampleConfig.csv");
    if (!SampleInfoManager::instance().loadFromCSV()) {
        std::cerr << "Failed to load CSV configuration from: "
                  << SampleInfoManager::instance().getCsvFileName() << std::endl;
        exit(5);
    }

    const SampleInfo* info = SampleInfoManager::instance().getSampleInfo(sampleName.Data());


    isData = info->isData;
    std::cout<< "Data or MC : "<< (info->isData ? "Data" : "MC") << std::endl;
    double baseWeight = info->weight;
    std::cout<< "Weight : "<<baseWeight<<std::endl;

////////////////////////////////////////////////////////////////////////////

    TString dataSet = "default";
    TString era     = "default";
    Ssiz_t underscorePos = sampleName.Index("_");
    if( isData ){
        if (underscorePos != kNPOS) {  // kNPOS는 찾지 못했을 때의 값
            dataSet = sampleName(0, underscorePos);
            era = sampleName(underscorePos + 1, sampleName.Length() - underscorePos - 1);
            std::cout<< "  [ makeBTagWeight::Loop() ] : Current Sample --> Data "<< dataSet << ", Era : " << era << std::endl;
        } else {
            std::cout << "  [ makeBTagWeight::Loop() ] : No underscore found in the string!" << std::endl;
            return;
        }
    }
    
    TH1F* h_nJets_noSF;   
    TH1F* h_nJets_withSF;
    TH1F* h_nbJets_noSF;
    TH1F* h_nbJets_withSF;
    TH1F* h_HT_noSF;
    TH1F* h_HT_withSF;

    std::vector<TH1F*> h_jetPt_noSF_vec;
    std::vector<TH1F*> h_jetPt_withSF_vec;
    std::vector<TH1F*> h_jetEta_noSF_vec;
    std::vector<TH1F*> h_jetEta_withSF_vec;
    std::vector<TH1F*> h_bTag_noSF_vec;
    std::vector<TH1F*> h_bTag_withSF_vec;

    // (A) 히스토그램 생성 (이벤트 레벨, 제트 레벨 통합)
    // 이벤트 레벨
    h_nJets_noSF    = new TH1F("h_nJets_noSF",   "nJets (noSF); nJets; Events",   50, 0, 50);
    h_nJets_withSF  = new TH1F("h_nJets_withSF", "nJets (withSF); nJets; Events", 50, 0, 50);    

    h_nbJets_noSF   = new TH1F("h_nbJets_noSF",   "nbJets (noSF); nbJets; Events",   15, 0, 15);
    h_nbJets_withSF = new TH1F("h_nbJets_withSF", "nbJets (withSF); nbJets; Events", 15, 0, 15);

    h_HT_noSF    = new TH1F("h_HT_noSF",   "HT (noSF);HT [GeV]; Events",   50, 0, 2000);
    h_HT_withSF  = new TH1F("h_HT_withSF", "HT (withSF);HT [GeV]; Events", 50, 0, 2000); 

    const int maxJetsObserved = 12;
    for(int iJet=0; iJet<maxJetsObserved; iJet++){

            TString name_noSF   = Form("h_jetPt_noSF_jet%d", iJet);
            TString title_noSF  = Form("Jet %d pT (noSF); p_{T} [GeV]; Events", iJet);
            h_jetPt_noSF_vec.push_back(new TH1F(name_noSF, title_noSF, 50, 0, 500));

            TString name_withSF   = Form("h_jetPt_withSF_jet%d", iJet);
            TString title_withSF  = Form("Jet %d pT (withSF); p_{T} [GeV]; Events", iJet);
            h_jetPt_withSF_vec.push_back(new TH1F(name_withSF, title_withSF, 50, 0, 500));

            TString etaName_noSF   = Form("h_jetEta_noSF_jet%d", iJet);
            TString etaTitle_noSF  = Form("Jet %d #eta (NoSF);#eta;Events", iJet);
            h_jetEta_noSF_vec.push_back(new TH1F(etaName_noSF, etaTitle_noSF, 50, -2.5, 2.5));

            TString etaName_withSF   = Form("h_jetEta_withSF_jet%d", iJet);
            TString etaTitle_withSF  = Form("Jet %d #eta (WithSF);#eta;Events", iJet);
            h_jetEta_withSF_vec.push_back(new TH1F(etaName_withSF, etaTitle_withSF, 50, -2.5, 2.5));

            TString bTagName_noSF   = Form("h_bTag_noSF_jet%d", iJet);
            TString bTagTitle_noSF  = Form("Jet %d bTagScore (NoSF);bTagScore;Events", iJet);
            h_bTag_noSF_vec.push_back(new TH1F(bTagName_noSF, bTagTitle_noSF, 50, 0, 1.0));

            TString bTagName_withSF   = Form("h_bTag_withSF_jet%d", iJet);
            TString bTagTitle_withSF  = Form("Jet %d bTagScore (WithSF);bTagScore;Events", iJet);
            h_bTag_withSF_vec.push_back(new TH1F(bTagName_withSF, bTagTitle_withSF, 50, 0, 1.0));   

    }    


    ////// Eta 분할을 사용할지 여부 설정 (true: 사용, false: 사용하지 않음)
    ////useEtaBinning = false; // 필요한 경우 이 값을 false로 설정하여 Eta 분할을 끌 수 있습니다.

////    TString TrigSFpath = "ScaleFactors_" + sampleName + ".root";
    TString TrigSFpath = "ScaleFactors.root";
    TFile* sfFile = TFile::Open(TrigSFpath, "READ");

    if (!sfFile || sfFile->IsZombie())
    {
        std::cerr << "Cannot open Scale Factors file: ScaleFactors.root" << std::endl;
        return;
    }

////    for (int iBjet = 0; iBjet < 1; ++iBjet)
////    {
////        TString sfHistName = Form("ScaleFactors/SF_Bjet%d", iBjet);
////
////        sfHist = (TH2D*)sfFile->Get(sfHistName);
////        if (!sfHist)
////        {
////            std::cerr << "Cannot find Scale Factor histogram: " << sfHistName << std::endl;
////            return;
////        }
////    }
    TString sfHistName = "ScaleFactors/SF_Bjet0";
    sfHist_inFile = (TH2D*)sfFile->Get(sfHistName);

    sfHist = static_cast<TH2D*>(sfHist_inFile->Clone("h_sf_local"));
    sfHist->SetDirectory(nullptr);
    sfFile->Close();

    if (!sfHist)
    {
        std::cerr << "Cannot find Scale Factor histogram: " << sfHistName << std::endl;
        return;
    }



    // 변수 구간 설정
    // HT bins
    const auto& HT_bins_vec = BinConfig::getHTBins();
    nBinsHT = BinConfig::HTBinCount;
    for (int i = 0; i <= nBinsHT; ++i) {
        HT_bins[i] = HT_bins_vec[i];
    }

    // pT bins
    const auto& pT_bins_vec = BinConfig::getPTBins();
    nBinspT = BinConfig::PTBinCount;
    for (int i = 0; i <= nBinspT; ++i) {
        pT_bins[i] = pT_bins_vec[i];
    }

        ////sfFile->Close();
    // b-제트 수 bins (범위로 설정)
    // 예: 3, 4, 5~8
    ////const auto& nBjets_bins_vec = BinConfig::getNBJetsBins();
    ////nBjetBins = BinConfig::NBJetsBinCount;
    ////for (int i = 0; i <= nBjetBins; ++i) {
    ////    nBjets_bins[i] = nBjets_bins_vec[i];
    ////}

    ////// Eta bins (필요한 경우만 초기화)
    ////if (useEtaBinning) {
    ////    const auto& eta_bins_vec = BinConfig::getEtaBins();
    ////    nEtaBins = BinConfig::EtaBinCount;
    ////    for (int i = 0; i <= nEtaBins; ++i) {
    ////        eta_bins[i] = eta_bins_vec[i];
    ////    }
    ////} else {
    ////    nEtaBins = 1; // Eta 분할을 사용하지 않을 경우 기본값 설정
    ////}

    ////// 히스토그램 초기화
    ////for (int iEta = 0; iEta < nEtaBins; ++iEta) {
    ////    for (int iBjet = 0; iBjet < nBjetBins; ++iBjet) {
    ////        TString histName_Total, histTitle_Total;
    ////        TString histName_Pass, histTitle_Pass;

    ////        if (useEtaBinning) {
    ////            histName_Total = Form("h_Total_Eta%d_Bjet%d", iEta, iBjet);
    ////            histTitle_Total = Form("Total Events (Eta bin %d, b-jet bin %d);HT [GeV];6th Jet p_{T} [GeV]", iEta, iBjet);

    ////            histName_Pass = Form("h_Pass_Eta%d_Bjet%d", iEta, iBjet);
    ////            histTitle_Pass = Form("Passed Events (Eta bin %d, b-jet bin %d);HT [GeV];6th Jet p_{T} [GeV]", iEta, iBjet);
    ////        } else {
    ////            histName_Total = Form("h_Total_Bjet%d", iBjet);
    ////            histTitle_Total = Form("Total Events (b-jet bin %d);HT [GeV];6th Jet p_{T} [GeV]", iBjet);

    ////            histName_Pass = Form("h_Pass_Bjet%d", iBjet);
    ////            histTitle_Pass = Form("Passed Events (b-jet bin %d);HT [GeV];6th Jet p_{T} [GeV]", iBjet);
    ////        }

    ////        h_Total[iEta][iBjet] = new TH2D(histName_Total, histTitle_Total, nBinsHT, HT_bins, nBinspT, pT_bins);
    ////        h_Pass[iEta][iBjet] = new TH2D(histName_Pass, histTitle_Pass, nBinsHT, HT_bins, nBinspT, pT_bins);
    ////    }
    ////}


    BTagCalibration calib("deepJet", "/Users/jhlee/Desktop/Work/ttHH/ttHH_bTag_v5_W_CorrLib/ScaleFactors/bTag/reshaping_deepJet_106XUL17_v3.csv");

    // OP=RESHAPING, sysType="central" (추가 systematic up/down 이름들 벡터로 넣을 수 있음)
    BTagCalibrationReader reader(
      BTagEntry::OP_RESHAPING,  // OperatingPoint == 3
      "central"                 // sysType (ex: "central", or "down_hf", etc.)
      // { "up_jes", "down_jes", "up_hfstats1", ... } // 필요하다면 추가
    );

    // measurementType: "iterativefit"
    // jetFlavor별(B=5, C=4, UDSG=0) 로드
    reader.load(calib, BTagEntry::FLAV_B,    "iterativefit");
    reader.load(calib, BTagEntry::FLAV_C,    "iterativefit");
    reader.load(calib, BTagEntry::FLAV_UDSG, "iterativefit");


    // 이벤트 루프
    Long64_t nentries = fChain->GetEntriesFast();
    Bool_t passHadTrig = false;
    double Jet6PT = -999.0;
    double Jet6Eta = -999.0;
    double weight = 1.0;
    double btaggingSF = -55555.0;
    if (debug) nentries = numberOfEvt;
    if (debug) std::cout<<"\n-----------------------------------------------------------"<<std::endl;
    for (Long64_t jentry=0; jentry<nentries; jentry++) {
//      for (Long64_t jentry=nentries*0.8; jentry<nentries*0.85; jentry++) {
//      for (Long64_t jentry=0; jentry<nentries*0.6; jentry++) {

        passHadTrig = false;
        Jet6PT = -999.0;
        Jet6Eta = -999.0;
        weight = 1.0;
        btaggingSF = -55555.0;
        
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        fChain->GetEntry(jentry);
       
        if (nJets != jetPt->size()) {
            std::cerr << "[ERROR] nJets (" << nJets << ") does not match jetPt vector size (" 
                     << jetPt->size() << ") at entry " << jentry << std::endl;
            std::cerr << "Exiting program due to data inconsistency." << std::endl;
            exit(1);
        }

	if (debug) std::cout<<"  [ debug ] current entry --> "<<ientry<<std::endl;
 	if (debug) std::cout<<"  [ debug ]     # of jets = "<<nJets<<std::endl;

        if (nMuons != 1) continue;

        // 이벤트 선택
	if (nJets < 7) continue;
//        if (nJets < 6) {
//            std::cerr << "[ERROR] nJets (" << nJets << ") are smaller than 6.."<< std::endl;
//            exit(2);
//        }

        if (!passMETFilters) {
            std::cerr << "[ERROR] It did not pass the noise filters.."<< std::endl;
            exit(3);
        }

        Jet6PT = jetPt->at(5);
        if (Jet6PT <= 40.0) {
            std::cerr << "[ERROR] Jet6th pT (" << Jet6PT << ") are <= 40.0 GeV.."<< std::endl;
            exit(4);
        }


        if (HT < 500.0) {
            std::cerr << "[ERROR] HT (" << HT << ") are < 500.0 GeV.."<< std::endl;
            exit(5);
        }


////        int nBjet = 0;
////        for(int iJ=0; iJ<nJets; iJ++){
////            if(bTagScore->at(iJ) > 0.3040) nBjet++;
////	}
////	if (nBjet < 4) continue;

        // HT와 Jet6PT bin 찾기
        int htBin = -1;
        for (int iHT = 0; iHT < nBinsHT; ++iHT) {
            if (HT >= HT_bins[iHT] && HT < HT_bins[iHT+1]) {
                htBin = iHT+1; // 히스토그램 bin 번호는 1부터 시작
                break;
            }
        }
//        if (htBin == -1) continue;
        if(htBin == -1) {
//////            std::cerr << "[ERROR] Can not find ht Bin.."<< std::endl;
//////            exit(4);
            htBin = nBinsHT;
	}
	
        int ptBin = -1;
        for (int ipT = 0; ipT < nBinspT; ++ipT) {
            if (Jet6PT >= pT_bins[ipT] && Jet6PT < pT_bins[ipT+1]) {
                ptBin = ipT+1;
                break;
            }
        }
//        if (ptBin == -1) continue;
        if(ptBin == -1) {
//////            std::cerr << "[ERROR] Can not find pt Bin.. & pT = ("<< Jet6PT<<")"<< std::endl;
//////            exit(5);
            ptBin = nBinspT;
	}


//        if(isData && failGoldenJson) continue;
        if(isData && failGoldenJson) {
            std::cerr << "[ERROR] Data is not passed Golden Json.. Damn.."<< std::endl;
	    exit(6);
	}


        if(debug) std::cout<<"  [ debug ] Pass all selections!!"<<std::endl;

        // 스케일 팩터 가져오기
        double sf = sfHist->GetBinContent(htBin, ptBin);
        ////if (sf == 0) sf = 1.0; // 스케일 팩터가 0인 경우 1로 설정
        if (Jet6PT >= 90.0 && Jet6PT < 150.0 &&
//            HT      >= 150.0 && HT      < 600.0) {
            HT      >= 150.0 && HT      < 700.0) {
      
	    std::cout<<"  [ WARNING ] Empty bin when HT - " << HT << ", Jet6PT - "<<Jet6PT<<" and sf = "<<sf<<std::endl;
	    std::cout<<"  [ WARNING ] Change sf [ "<<sf<<" ] into -> 1.0"<<std::endl;

            sf = 1.0;  // no-stat bin default
        }

        if(sf == 0) {
            std::cerr << "[ERROR] sf is 0.. then double check sf = ("<< sf <<")"<< std::endl;
	    std::cerr << "[ERROR] and It's pT & HT = " << Jet6PT <<", "<< HT << std::endl;
	    std::cerr << "[ERROR] allocated pT & HT bin = "<< ptBin <<", "<< htBin << std::endl;
	    exit(7);
	}
        //std::cout<<"sf : "<<sf<<std::endl;
 

////	bool trigJetHT_B = passTrigger_HLT_PFHT1050 || passTrigger_6J1T_B || passTrigger_6J2T_B;
////	bool trigBTagCSV_B = passTrigger_4J3T_B;
////	bool trigJetHT = passTrigger_HLT_PFHT1050 || passTrigger_6J1T_CDEF || passTrigger_6J2T_CDEF;
////	bool trigBTagCSV = passTrigger_4J3T_CDEF;
////
////        if (!isData) {
////	    passHadTrig = trigJetHT || trigBTagCSV;
////	    //passHadTrig = trigJetHT;
////
////        } else if ( dataSet == "JetHT") {
////
////                if(era == "B"){
////		    passHadTrig = trigJetHT_B;
////                    //std::cout<<"  [ makeBTagWeight::Loop() ]  : Trigger for SingleMuon B is setted" << std::endl;
////		} else {
////		    passHadTrig = trigJetHT;
////                    //std::cout<<"  [ makeBTagWeight::Loop() ]  : Trigger for SingleMuon CDEF is setted" << std::endl;
////		}
////
////	} else if ( dataSet == "BTagCSV") {
////	        //std::cout<<" Currently BTagCSV is activated!!!!"<<std::endl;
////                if(era == "B"){
////	            //std::cout<<" And period B"<<std::endl;
////                    passHadTrig = !trigJetHT_B && trigBTagCSV_B;
////		} else {
////	            //std::cout<<" And period CEDF"<<std::endl;
////                    passHadTrig = !trigJetHT && trigBTagCSV;
////		}
////	}
////	else {
////	    std::cout<<"  [ ERROR ] : Unknown type is detected.. --> "<<dataSet<<std::endl;
////	    exit(25);
////	}
	////passHadTrig = passTrigger_HLT_PFHT1050;


        // ===== Trigger path for normal JetHT / BTagCSV SF application =====
        // Era split
        const bool fired6J1T = (era=="B")   ? passTrigger_6J1T_B   : passTrigger_6J1T_CDEF;
        const bool fired6J2T = (era=="B")   ? passTrigger_6J2T_B   : passTrigger_6J2T_CDEF;
        const bool fired4J3T = (era=="B")   ? passTrigger_4J3T_B   : passTrigger_4J3T_CDEF;
        const bool firedHT   = /* era-independent */  passTrigger_HLT_PFHT1050;
        
        // OR of all hadronic paths (for SF application on MC and inclusive data logic)
        const bool any6J     = (fired6J1T || fired6J2T);
        const bool passORHad = (fired4J3T || any6J || firedHT);
        
        // Enforce "PD-exclusivity" on *data* so that each PD contributes only with its intended triggers.
        //  - BTagCSV PD : take ONLY 4J3T fired events.
        //  - JetHT   PD : take 6J* fired events, OR PFHT1050-only (i.e., HT fired while neither 6J* nor 4J3T fired).
        bool pdExclusiveRuleOK = true;
        
        if (isData) {
          if      (dataSet == "BTagCSV") {
            pdExclusiveRuleOK = fired4J3T;  // only btag PD triggers
          } else if (dataSet == "JetHT") {
            pdExclusiveRuleOK = ( any6J || (firedHT && !any6J && !fired4J3T) ); // HT-only or 6J*; exclude overlap with BTagCSV
          } else {
            // 안전장치: PD가 둘 중 아니면 분석 중단(원래 코드 정책에 맞춰 처리)
            std::cerr << "[ERROR] Unknown dataSet for trigger routing: " << dataSet << std::endl;
            pdExclusiveRuleOK = false;
          }
        }
    
        // Final decision:
        //  - MC  : OR of all hadronic triggers (passORHad).
        //  - Data: OR-of-all AND PD-exclusive rule.
        bool passHadTrig = isData ? (passORHad && pdExclusiveRuleOK) : passORHad;

        
        if (!passHadTrig) continue;



        ////baseWeight = 1.0;
	btaggingSF = 1.0;
////        int calJet = (nJets < 12) ? nJets : 12;
        if (!isData) {
            for(int iJ=0; iJ<nJets; iJ++){
////            for(int iJ=0; iJ<calJet; iJ++){
                // Flavor 구분
                int hadFlavor = hadFlavs->at(iJ);
                if (debug) std::cout<<"  [ debug ]         jet flavor = "<<hadFlavor<<std::endl;

                BTagEntry::JetFlavor jf = BTagEntry::FLAV_UDSG; // From gluon, or light quarks(u, d, s)
                if      (hadFlavor == 5) jf = BTagEntry::FLAV_B; // From b quark
                else if (hadFlavor == 4) jf = BTagEntry::FLAV_C; // From c quark

                if (debug) std::cout<<"  [ debug ]         revised jet flavor = "<<jf<<std::endl;

                float discVal   = bTagScore->at(iJ);
                float jetPtVal  = jetPt->at(iJ);
                float jetEtaVal = jetEta->at(iJ);

                if (debug) {
		    std::cout<<"  [ debug ]         b-tag discri, pt, eta  = ("
		    <<discVal<<", "<<jetPtVal<<", "<<jetEtaVal<<")"<<std::endl;
		}

                // eval() -> 이 제트에 대한 reshape SF
                double sfVal = reader.eval(jf, jetEtaVal, jetPtVal, discVal);
                if (debug) std::cout<<"  [ debug ]         and it's SF = "<<sfVal<<std::endl;
                btaggingSF *= sfVal; 
            }
        }
	else {
	    btaggingSF = 1.0;
	}
        if(debug) std::cout<<"  [ debug ] Loaded L1pre & PU weight --> "<< L1PrefiringWeight <<", "<<PUWeight<<std::endl;
        double L1NPu = L1PrefiringWeight * PUWeight;

////        if(btaggingSF == 0.0) btaggingSF = 1.0;

////        std::cout<<"btaggingSF = "<<btaggingSF<<std::endl;
	double weightNoSF = baseWeight * sf * L1NPu;
	double weightWithSF = baseWeight * btaggingSF * sf * L1NPu;

        if(debug) std::cout<<"  [ debug ] Will fill value HT = "<<HT<<" with weights NoSF = "<<weightNoSF<<", WithSF = "<<weightWithSF<<std::endl;

        // --------------------
        // (3) 이벤트 레벨 히스토그램 Fill
        // --------------------
        h_nJets_noSF->Fill( nJets,  weightNoSF );
        h_nJets_withSF->Fill( nJets, weightWithSF );

        //h_nbJets_noSF->Fill( nbJets, weightNoSF );
        //h_nbJets_withSF->Fill( nbJets, weightWithSF );

        h_HT_noSF->Fill( HT, weightNoSF );
        h_HT_withSF->Fill( HT, weightWithSF );

        // --------------------
        // (5) 제트 인덱스별 (leading~)
        // --------------------
        int nToFill = (nJets < maxJetsObserved) ? nJets : maxJetsObserved;
	for(int iJ=0; iJ<nToFill; iJ++){
            h_jetPt_noSF_vec[iJ]->Fill( jetPt->at(iJ),  weightNoSF );
            h_jetPt_withSF_vec[iJ]->Fill( jetPt->at(iJ), weightWithSF );

            h_jetEta_noSF_vec[iJ]->Fill( jetEta->at(iJ), weightNoSF );
            h_jetEta_withSF_vec[iJ]->Fill( jetEta->at(iJ), weightWithSF );

            h_bTag_noSF_vec[iJ]->Fill( bTagScore->at(iJ), weightNoSF );
            h_bTag_withSF_vec[iJ]->Fill( bTagScore->at(iJ), weightWithSF );
        }

    }

    if (debug) std::cout<<"-----------------------------------------------------------\n"<<std::endl;

    // Ratio Histogram
    TH1F* h_nJets_ratio = (TH1F*)h_nJets_noSF->Clone("h_nJets_ratio");
    h_nJets_ratio->SetTitle("Normalization ratio (NoSF/WithSF)");
    h_nJets_ratio->Divide(h_nJets_withSF);

    std::cout << "Event loop completed." << std::endl;

    // 히스토그램 저장을 위한 출력 파일
    TString SFpath = "ScaleFactors/bTagReweight/";
    TString tag = "bTagReweight_";
    TFile* outputFile = new TFile( tag + getOutputName(), "RECREATE");
    ////TFile* outputFile = new TFile( "bTagReweight_JetHT_F.root", "RECREATE");
    std::cout<<"Output name --> "<< tag + getOutputName()<<std::endl;

    ////// 히스토그램 저장
    ////for (int iEta = 0; iEta < nEtaBins; ++iEta) {
    ////    for (int iBjet = 0; iBjet < nBjetBins; ++iBjet) {
    ////        outputFile->cd();
    ////        h_Total[iEta][iBjet]->Write();
    ////        h_Pass[iEta][iBjet]->Write();
    ////    }
    ////}

    // 이벤트 레벨
    h_nJets_noSF->Write();   h_nJets_withSF->Write();
    //h_nbJets_noSF->Write();  h_nbJets_withSF->Write();
    h_HT_noSF->Write();      h_HT_withSF->Write();

    h_nJets_ratio->Write();

    // 제트 레벨 (인덱스별)
    for(int iJet=0; iJet<maxJetsObserved; iJet++){
        h_jetPt_noSF_vec[iJet]->Write();
        h_jetPt_withSF_vec[iJet]->Write();

        h_jetEta_noSF_vec[iJet]->Write();
        h_jetEta_withSF_vec[iJet]->Write();

        h_bTag_noSF_vec[iJet]->Write();
        h_bTag_withSF_vec[iJet]->Write();

    }

    outputFile->Close();
}

void makeBTagWeight::Init()
{
   // TTree 초기화 및 브랜치 설정
   TString ntupleDir  = "/Users/jhlee/ttHH/ntuple/skimmed/";
   //TString ntupleDir  = "/Users/jhlee/Desktop/Work/ttHH/TriggerStudyv2/ntuple/250609/";
   TString ntupleName = getInputName();
   TString ntuplePath = ntupleDir + ntupleName;
   TTree* tree = nullptr;
   
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject( ntuplePath );
      if (!f || !f->IsOpen()) {
         f = new TFile( ntuplePath );
      }
      TDirectory * dir = (TDirectory*)f->Get("Tree");
      
      dir->GetObject("Tree",tree);
   }

   // 브랜치 주소 설정
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("passTrigger_HLT_IsoMu27", &passTrigger_HLT_IsoMu27, &b_passTrigger_HLT_IsoMu27);
   fChain->SetBranchAddress("passTrigger_HLT_PFHT1050", &passTrigger_HLT_PFHT1050, &b_passTrigger_HLT_PFHT1050);
   fChain->SetBranchAddress("passTrigger_6J1T_B", &passTrigger_6J1T_B, &b_passTrigger_6J1T_B);
   fChain->SetBranchAddress("passTrigger_6J1T_CDEF", &passTrigger_6J1T_CDEF, &b_passTrigger_6J1T_CDEF);
   fChain->SetBranchAddress("passTrigger_6J2T_B", &passTrigger_6J2T_B, &b_passTrigger_6J2T_B);
   fChain->SetBranchAddress("passTrigger_6J2T_CDEF", &passTrigger_6J2T_CDEF, &b_passTrigger_6J2T_CDEF);
   fChain->SetBranchAddress("passTrigger_4J3T_B", &passTrigger_4J3T_B, &b_passTrigger_4J3T_B);
   fChain->SetBranchAddress("passTrigger_4J3T_CDEF", &passTrigger_4J3T_CDEF, &b_passTrigger_4J3T_CDEF);
   fChain->SetBranchAddress("nMuons", &nMuons, &b_nMuons);
   //fChain->SetBranchAddress("nElecs", &nElecs, &b_nElecs);
   fChain->SetBranchAddress("nJets", &nJets, &b_nJets);
   //fChain->SetBranchAddress("nbJets", &nbJets, &b_nbJets);
   fChain->SetBranchAddress("HT", &HT, &b_HT);
////   fChain->SetBranchAddress("jetPt", jetPt, &b_jetPt);
////   fChain->SetBranchAddress("jetEta", jetEta, &b_jetEta);
////   fChain->SetBranchAddress("bTagScore", bTagScore, &b_bTagScore);
////   fChain->SetBranchAddress("hadFlavs", hadFlavs, &b_hadFlavs);
   fChain->SetBranchAddress("jetPt", &jetPt, &b_jetPt);
   fChain->SetBranchAddress("jetEta", &jetEta, &b_jetEta);
   fChain->SetBranchAddress("bTagScore", &bTagScore, &b_bTagScore);
   fChain->SetBranchAddress("hadFlavs", &hadFlavs, &b_hadFlavs);

   //fChain->SetBranchAddress("partonFlavs", partonFlavs, &b_partonFlavs);
  // fChain->SetBranchAddress("eventNumber", &eventNumber, &b_eventNumber);
   //fChain->SetBranchAddress("runNumber", &runNumber, &b_runNumber);   
   fChain->SetBranchAddress("PUWeight", &PUWeight, &b_PUWeight);
   fChain->SetBranchAddress("L1PrefiringWeight", &L1PrefiringWeight, &b_L1PrefiringWeight);
   fChain->SetBranchAddress("failGoldenJson", &failGoldenJson, &b_failGoldenJson);
   fChain->SetBranchAddress("passMETFilters", &passMETFilters, &b_passMETFilters);

   Notify();
}

bool makeBTagWeight::Notify()
{
   return true;
}

void makeBTagWeight::Show(Long64_t entry)
{
   if (!fChain) return;
   fChain->Show(entry);
}

Int_t makeBTagWeight::Cut(Long64_t entry)
{
   return 1;
}

