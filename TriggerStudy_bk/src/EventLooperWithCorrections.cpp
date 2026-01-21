#define EventLooperWithCorrections_cxx
#include "EventLooperWithCorrections.hh"
#include <TH2.h>
#include <TStyle.h>
#include <iostream>

#include "BinConfig.hh"

void EventLooperWithCorrections::Loop()
{
    if (fChain == 0) return;

    bool noCorrection = false;

    // 입력 파일 이름으로부터 샘플 이름과 데이터 여부를 결정
    TString sampleName = getInputName();
    Ssiz_t rootPos = sampleName.Index(".root");
    if (rootPos != kNPOS) {
        sampleName.Remove(rootPos);
    }
    std::cout << "Sample Name: " << sampleName << std::endl;

    isData = true;
////    if (sampleName == "ttJets") {
////        isData = false;
////        std::cout << "Current Sample: MC ttJets" << std::endl;
////    } else {
////        std::cout << "Current Sample: Data" << std::endl;
////    }
    TString channel = "";
    if (sampleName == "TTTo2L2Nu"){
        isData = false;
        channel = "diLep";
        std::cout << "Current Sample: MC TTTo2L2Nu" << std::endl;
    }
    else if(sampleName == "TTToHadronic"){
        isData = false;
        channel = "had";
        std::cout << "Current Sample: MC TTToHadronic" << std::endl;
    }
    else if(sampleName == "TTToSemiLeptonic"){
        isData = false;
        channel = "semiLep";
        std::cout << "Current Sample: MC TTToSemiLeptonic" << std::endl;
    } else {
        std::cout << "Current Sample: Data" << std::endl;
    }


    TString dataSet = "default";
    TString era     = "default";
    Ssiz_t underscorePos = sampleName.Index("_");
    if( isData ){
        if (underscorePos != kNPOS) {  // kNPOS는 찾지 못했을 때의 값
            dataSet = sampleName(0, underscorePos);
            era = sampleName(underscorePos + 1, sampleName.Length() - underscorePos - 1);
            std::cout<< "  [ EventLooper::Loop() ] : Current Sample --> Data "<< dataSet << ", Era : " << era << std::endl;
        } else {
            std::cout << "  [ EventLooper::Loop() ] : No underscore found in the string!" << std::endl;
            return;
        }
    }


    // Eta 분할을 사용할지 여부 설정
    useEtaBinning = false; // Eta 분할을 사용하지 않음
    // Eta 분할을 사용하지 않지만, 히스토그램 이름에 Eta0를 포함하도록 설정

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

    //// b-제트 수 bins (범위로 설정)
    //const auto& nBjets_bins_vec = BinConfig::getNBJetsBins();
    //nBjetBins = BinConfig::NBJetsBinCount;
    //for (int i = 0; i <= nBjetBins; ++i) {
    //    nBjets_bins[i] = nBjets_bins_vec[i];
    //}

    //// Eta bins 설정 (Eta 분할을 사용하지 않더라도 Eta0을 포함하기 위해)
    //const auto& Eta_bins_vec = BinConfig::getEtaBins();
    //nEtaBins = BinConfig::EtaBinCount;
    //for (int i = 0; i <= nEtaBins; ++i) {
    //    eta_bins[i] = Eta_bins_vec[i];
    //}

    // 스케일 팩터 히스토그램 로드
    TFile* sfFile = TFile::Open("ScaleFactors.root");
    if (!sfFile || sfFile->IsZombie())
    {
        std::cerr << "Cannot open Scale Factors file: ScaleFactors.root" << std::endl;
        return;
    }
    

    for (int iBjet = 0; iBjet < 1; ++iBjet)
    {
        TString sfHistName = Form("ScaleFactors/SF_Bjet%d", iBjet);

        sfHist[iBjet] = (TH2D*)sfFile->Get(sfHistName);
        if (!sfHist[iBjet])
        {
            std::cerr << "Cannot find Scale Factor histogram: " << sfHistName << std::endl;
            return;
        }
    }

    // 출력 파일 설정
    TFile* outputFile = new TFile(getOutputName(), "RECREATE");
    TTree* outputTree = fChain->CloneTree(0); // 빈 트리를 생성


    // 히스토그램 초기화
    h_HT_Total = new TH1D("h_HT_Total", "Total HT;HT [GeV];Events", nBinsHT, HT_bins);
    h_HT_Pass  = new TH1D("h_HT_Pass",  "Passed HT;HT [GeV];Events", nBinsHT, HT_bins);

    h_Jet6PT_Total = new TH1D("h_Jet6PT_Total", "Total 6th Jet p_{T};6th Jet p_{T} [GeV];Events", nBinspT, pT_bins);
    h_Jet6PT_Pass  = new TH1D("h_Jet6PT_Pass",  "Passed 6th Jet p_{T};6th Jet p_{T} [GeV];Events", nBinspT, pT_bins);

    //h_nJets_Total = new TH1I("h_nJets_Total", "Total nJets;nJets;Events", nBjetBins, nBjets_bins);
    //h_nJets_Pass  = new TH1I("h_nJets_Pass",  "Passed nJets;nJets;Events", nBjetBins, nBjets_bins);

    //h_Eta_Total = new TH1D("h_Eta_Total", "Total Eta;Eta;Events", nEtaBins, eta_bins);
    //h_Eta_Pass  = new TH1D("h_Eta_Pass",  "Passed Eta;Eta;Events", nEtaBins, eta_bins);


    // 이벤트 루프
    Long64_t nentries = fChain->GetEntriesFast();
    double Jet6PT = -999.0;
    double Jet6Eta = -999.0;
    double weight = 1.0;
    double sf = 1.0; // 스케일 팩터

    // weight에 스케일 팩터를 적용하기 위해 새로운 브랜치를 생성
    Float_t new_weight;
    TBranch* b_new_weight = outputTree->Branch("new_weight", &new_weight, "new_weight/F");

    for (Long64_t jentry=0; jentry<nentries; jentry++) {

        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        fChain->GetEntry(jentry);

        if(jetPt->size() != nJets) {
            std::cout<<"[ ERROR ] : jet vector size != number of jets"<<std::endl;
            exit(55);
        }

        // 이벤트 선택
	//if( nMuons != 1.0 ) continue;
        if (!(nMuons == 1 && nElecs == 0)) continue;
//        //if (nMuons != 1) continue;
//        if (nJets < 6) continue; // 6번째 제트가 존재하는지 확인
//
//        if(!passMETFilters) continue;
//
////        Jet6PT = jetPt[5];
//        Jet6PT = jetPt->at(5);
//
//        if (Jet6PT < 40.0) continue;
//        //Jet6Eta = jetEta[5];
//
//        if (HT < 500.0 ) continue;

        // 이벤트 선택
        if (nJets < 7) {
            std::cerr << "[ERROR] nJets (" << nJets << ") are smaller than 7.."<< std::endl;
            exit(2);
        }

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



        // 가중치 설정
//        if (!isData) {
//            weight = 3.4410 * L1PrefiringWeight * PUWeight; // ttJets의 가중치 (실제 값으로 수정 필요)
//        } else {
//            weight = 1.0 * weight;
//            if(failGoldenJson) continue;
//        }

        if (!isData) {
            if (channel == "diLep"){
                weight = 0.0004761561474 * L1PrefiringWeight * PUWeight * genWeight;
            }
            else if (channel == "had"){
                weight = 0.000214351205 * L1PrefiringWeight * PUWeight * genWeight;
            }
            else if (channel == "semiLep"){
                weight = 0.0001455793461 * L1PrefiringWeight * PUWeight * genWeight;
            }
        } else {
            weight = 1.0;
            ////if(failGoldenJson) continue;
            if(failGoldenJson) {
                std::cerr << "[ERROR] It did not pass the golden json.."<< std::endl;
                exit(6);
            }
        }


        if (!passTrigger_HLT_IsoMu27) {
            // Tag 실패 → 분모/분자 모두 제외
            continue;
        }


        // Eta bin은 항상 0으로 설정 (Eta 분할을 사용하지 않음)
        int etaBin = 0;

        // b-제트 수 bin 찾기 (범위로 설정)
        //int bjetBin = -1;
        //for (int iBjet = 0; iBjet < nBjetBins; ++iBjet) {
        //    if (nbJets >= nBjets_bins[iBjet] && nbJets < nBjets_bins[iBjet+1]) {
        //        bjetBin = iBjet;
        //        break;
        //    }
        //}
        //if (bjetBin == -1) continue; // 해당 b-제트 bin이 없으면 다음 이벤트로
        int bjetBin = 0;


        // HT와 Jet6PT bin 찾기
        int htBin = -1;
        for (int iHT = 0; iHT < nBinsHT; ++iHT) {
            if (HT >= HT_bins[iHT] && HT < HT_bins[iHT+1]) {
                htBin = iHT+1; // 히스토그램 bin 번호는 1부터 시작
                break;
            }
        }
        if (htBin == -1) continue;


        int ptBin = -1;
        for (int ipT = 0; ipT < nBinspT; ++ipT) {
            if (Jet6PT >= pT_bins[ipT] && Jet6PT < pT_bins[ipT+1]) {
                ptBin = ipT+1;
                break;
            }
        }
        if (ptBin == -1) continue;
      
        // 스케일 팩터 가져오기
        sf = sfHist[bjetBin]->GetBinContent(htBin, ptBin);
        if (sf == 0) sf = 1.0; // 스케일 팩터가 0인 경우 1로 설정

        // 새로운 weight 계산
        if (noCorrection) sf = 1.0;

	if (!isData) {
            new_weight = weight * sf;
	} else {
	    new_weight = weight;
	}
  
	//std::cout<<"Current weight : "<<new_weight<<std::endl;


        // 총 이벤트 히스토그램에 채우기
        h_HT_Total->Fill(HT, weight);
        h_Jet6PT_Total->Fill(Jet6PT, weight);
        //h_nJets_Total->Fill(nbJets, weight);
        //h_Eta_Total->Fill(Jet6Eta, weight);

//        // 트리거 조건 설정 (데이터와 MC 모두 동일하게 처리)
//        Bool_t passHadTrig;
//
//        bool trigJetHT_B = passTrigger_HLT_PFHT1050 || passTrigger_6J1T_B || passTrigger_6J2T_B;
//        bool trigBTagCSV_B = passTrigger_4J3T_B;
//        bool trigJetHT = passTrigger_HLT_PFHT1050 || passTrigger_6J1T_CDEF || passTrigger_6J2T_CDEF;
//        bool trigBTagCSV = passTrigger_4J3T_CDEF;
//
//        // 트리거 조건 설정
//        if (!isData) {
//                //passHadTrig = trigJetHT || trigBTagCSV;
//                passHadTrig = trigJetHT;
//                ;
//        } else {
//                if(era == "B"){
//                    //passHadTrig = trigJetHT_B || trigBTagCSV_B;
//                    passHadTrig = trigJetHT_B;
//                    std::cout<<"  [ EventLooper::Loop() ]  : Trigger for SingleMuon B is setted" << std::endl;
//                } else {
//                    //passHadTrig = trigJetHT || trigBTagCSV;
//                    passHadTrig = trigJetHT;
//                    std::cout<<"  [ EventLooper::Loop() ]  : Trigger for SingleMuon CDEF is setted" << std::endl;
//                }
//        }

        //////////////////////////////////////////////////////////////////
        // Trigger path logic for SF in SingleMuon data-set
        // era에 따른 hadronic OR 정의 (AN과 동일: B는 *_B, 그 외는 *_CDEF 사용)
        //  - Data의 Run B면 *_B 브랜치, 그 외(데이터 C–F, MC 포함)는 *_CDEF 브랜치 사용
        const bool fired6J1T = (isData && era == "B") ? passTrigger_6J1T_B   : passTrigger_6J1T_CDEF;
        const bool fired6J2T = (isData && era == "B") ? passTrigger_6J2T_B   : passTrigger_6J2T_CDEF;
        const bool fired4J3T = (isData && era == "B") ? passTrigger_4J3T_B   : passTrigger_4J3T_CDEF;
        const bool firedHT   =                           passTrigger_HLT_PFHT1050;
        
        const bool passHadTrig = (fired4J3T || fired6J1T || fired6J2T || firedHT);


        if (passHadTrig) {
            // 트리거 통과한 이벤트 히스토그램에 채우기
            h_HT_Pass->Fill(HT, new_weight);
            h_Jet6PT_Pass->Fill(Jet6PT, new_weight);
            //h_nJets_Pass->Fill(nbJets, new_weight);
            //h_Eta_Pass->Fill(Jet6Eta, new_weight);
        }

        // 출력 트리에 이벤트 저장
        outputTree->Fill();
    }

    // 출력 파일에 히스토그램 저장
    outputFile->cd();
    h_HT_Total->Write();
    h_HT_Pass->Write();
    h_Jet6PT_Total->Write();
    h_Jet6PT_Pass->Write();
    //h_nJets_Total->Write();
    //h_nJets_Pass->Write();
    //h_Eta_Total->Write();
    //h_Eta_Pass->Write();

    // 출력 파일에 트리 저장
    outputTree->Write();
    outputFile->Close();

    sfFile->Close();

    std::cout << "Event loop completed with efficiency corrections applied." << std::endl;
}

void EventLooperWithCorrections::Init()
{
   // TTree 초기화 및 브랜치 설정
   ////TString ntupleDir  = "/Users/jhlee/ttHH/ntuple/skimmed/";
   TString ntupleDir  = "/Users/jhlee/ttHH/ntuple/skimmed/gen_tier3/";
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
   fChain->SetBranchAddress("nElecs", &nElecs, &b_nElecs);
   fChain->SetBranchAddress("nJets", &nJets, &b_nJets);
   //fChain->SetBranchAddress("nbJets", &nbJets, &b_nbJets);
   fChain->SetBranchAddress("HT", &HT, &b_HT);
////   fChain->SetBranchAddress("jetPt", jetPt, &b_jetPt);
   fChain->SetBranchAddress("jetPt", &jetPt, &b_jetPt);

   //fChain->SetBranchAddress("jetEta", jetEta, &b_jetEta);
   //fChain->SetBranchAddress("bTagScore", bTagScore, &b_bTagScore);
   //fChain->SetBranchAddress("eventNumber", &eventNumber, &b_eventNumber);
   //fChain->SetBranchAddress("runNumber", &runNumber, &b_runNumber);
   fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
   fChain->SetBranchAddress("PUWeight", &PUWeight, &b_PUWeight);
   fChain->SetBranchAddress("L1PrefiringWeight", &L1PrefiringWeight, &b_L1PrefiringWeight);
   fChain->SetBranchAddress("failGoldenJson", &failGoldenJson, &b_failGoldenJson);
   fChain->SetBranchAddress("passMETFilters", &passMETFilters, &b_passMETFilters);


   Notify();
}

bool EventLooperWithCorrections::Notify()
{
   return true;
}

void EventLooperWithCorrections::Show(Long64_t entry)
{
   if (!fChain) return;
   fChain->Show(entry);
}

Int_t EventLooperWithCorrections::Cut(Long64_t entry)
{
   return 1;
}

