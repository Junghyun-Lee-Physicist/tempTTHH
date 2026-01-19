#define EventLooper_cxx
#include "EventLooper.hh"
#include <TH2.h>
#include <TStyle.h>
#include <iostream>

void EventLooper::Loop()
{
    if (fChain == 0) return;

    // =========================================================
    // 1. Bin Configuration
    // =========================================================
    const int nBinsHT = 6;
    double HT_bins[nBinsHT + 1] = {500., 600., 700., 800., 1000., 1300., 2500.};
    const int nBinspT = 6;
    double pT_bins[nBinspT + 1] = {40., 45., 50., 60., 70., 90., 150.};

    // =========================================================
    // 2. Sample Identification
    // =========================================================
    TString sampleName = getInputName();
    Ssiz_t rootPos = sampleName.Index(".root");
    if (rootPos != kNPOS) sampleName.Remove(rootPos);
    std::cout << "Sample Name: " << sampleName << std::endl;

    isData = true;
    TString channel = "";
    if (sampleName == "TTTo2L2Nu"){ isData = false; channel = "diLep"; }
    else if(sampleName == "ttTohadronic"){ isData = false; channel = "had"; }
    else if(sampleName == "TTToSemiLeptonic"){ isData = false; channel = "semiLep"; }
    else if(sampleName.Contains("ttJets")) { isData = false; channel = "inclusive"; }

    TString dataSet = "default";
    TString era     = "default";
    if( isData ){
        Ssiz_t underscorePos = sampleName.Index("_");
        if (underscorePos != kNPOS) {
            dataSet = sampleName(0, underscorePos);
            era = sampleName(underscorePos + 1, sampleName.Length() - underscorePos - 1);
        }
    }

    // =========================================================
    // 3. [수정] Load Scale Factors FIRST
    //    파일 충돌 방지를 위해 SF를 먼저 메모리에 로드하고 파일을 닫습니다.
    // =========================================================
    if (applySFMode) {
        std::cout << "[ Mode ] Loading Scale Factors..." << std::endl;
        TFile* sfFile = TFile::Open("ScaleFactors.root");
        if (!sfFile || sfFile->IsZombie()) {
            std::cerr << "Cannot open Scale Factors file: ScaleFactors.root" << std::endl;
            return;
        }

        for (int iBjet = 0; iBjet < 1; ++iBjet) {
            TString sfHistName = Form("ScaleFactors/SF_Bjet%d", iBjet);
            TH2D* tempHist = (TH2D*)sfFile->Get(sfHistName);
            if (!tempHist) {
                std::cerr << "Cannot find SF hist: " << sfHistName << std::endl;
                return;
            }
            // [중요] 히스토그램을 파일에서 분리하여 메모리에만 남김
            sfHist[iBjet] = (TH2D*)tempHist->Clone();
            sfHist[iBjet]->SetDirectory(0); 
        }
        sfFile->Close(); // [중요] SF 파일 닫기
        delete sfFile;
    }

    // =========================================================
    // 4. Initialization (Output File & Tree)
    // =========================================================
    // SF 파일을 닫은 후 Output 파일을 엽니다. 이제 gDirectory는 오직 outputFile입니다.
    TFile* outputFile = new TFile(getOutputName(), "RECREATE");
    outputFile->cd(); // 확실하게 디렉토리 이동

    TTree* outputTree = nullptr;
    Float_t new_weight = 1.0;

    if (applySFMode) {
        std::cout << "[ Mode ] Creating Correction Tree..." << std::endl;
        
        // [중요] CloneTree 실행
        outputTree = fChain->CloneTree(0);
        
        // [핵심 해결책] Tree가 outputFile에 속함을 명시
        outputTree->SetDirectory(outputFile); 
        outputTree->Branch("new_weight", &new_weight, "new_weight/F");
        
        // 메모리 관리 설정 (AutoSave 빈도 조절)
        outputTree->SetAutoSave(10000000); 

        // Validation Histograms
        h_HT_Total     = new TH1D("h_HT_Total", "Total HT;HT [GeV];Events", nBinsHT, HT_bins);
        h_HT_Pass      = new TH1D("h_HT_Pass",  "Passed HT;HT [GeV];Events", nBinsHT, HT_bins);
        h_Jet6PT_Total = new TH1D("h_Jet6PT_Total", "Total 6th Jet p_{T};6th Jet p_{T} [GeV];Events", nBinspT, pT_bins);
        h_Jet6PT_Pass  = new TH1D("h_Jet6PT_Pass",  "Passed 6th Jet p_{T};6th Jet p_{T} [GeV];Events", nBinspT, pT_bins);
    } else {
        std::cout << "[ Mode ] Calculating Trigger Efficiency" << std::endl;
        // Mode 1: Efficiency Hists
        for (int iEta = 0; iEta < 1; ++iEta) {
            for (int iBjet = 0; iBjet < 1; ++iBjet) {
                TString histName_Total = Form("h_Total_Bjet%d", iBjet);
                TString histName_Pass = Form("h_Pass_Bjet%d", iBjet);
                h_Total[iEta][iBjet] = new TH2D(histName_Total, "", nBinsHT, HT_bins, nBinspT, pT_bins);
                h_Pass[iEta][iBjet]  = new TH2D(histName_Pass, "", nBinsHT, HT_bins, nBinspT, pT_bins);
            }
        }
    }

    // =========================================================
    // 5. Event Loop
    // =========================================================
    Long64_t nentries = fChain->GetEntriesFast();
    double Jet6PT = -999.0;
    double weight = 1.0;
    
    for (Long64_t jentry=0; jentry<nentries; jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        fChain->GetEntry(jentry);

        if(!jetPt || jetPt->size() != nJets) continue; // 포인터 안전 점검 추가

        // --- Common Selections ---
        if (!(nMuons == 1 && nElecs == 0)) continue;
        if (nJets < 7) continue; 
        if (!passMETFilters) continue;

        Jet6PT = jetPt->at(5);
        if (Jet6PT <= 40.0) continue;
        if (HT < 500.0) continue;

        // --- Weight Calculation ---
        if (!isData) {
            if (channel == "diLep")        weight = 0.0004761561474 * L1PrefiringWeight * PUWeight * genWeight;
            else if (channel == "had")     weight = 0.000214351205  * L1PrefiringWeight * PUWeight * genWeight;
            else if (channel == "semiLep") weight = 0.0001455793461 * L1PrefiringWeight * PUWeight * genWeight;
            else weight = genWeight * PUWeight * L1PrefiringWeight; 
        } else {
            weight = 1.0;
            if(failGoldenJson) continue;
        }

        // --- Trigger Logic ---
        if (!passTrigger_HLT_IsoMu27) continue;

        const bool fired6J1T = (isData && era=="B") ? passTrigger_6J1T_B : passTrigger_6J1T_CDEF;
        const bool fired6J2T = (isData && era=="B") ? passTrigger_6J2T_B : passTrigger_6J2T_CDEF;
        const bool fired4J3T = (isData && era=="B") ? passTrigger_4J3T_B : passTrigger_4J3T_CDEF;
        const bool firedHT   = passTrigger_HLT_PFHT1050;
        const bool passHadTrig = (fired4J3T || fired6J1T || fired6J2T || firedHT);

        // =====================================================
        // Mode Specific Logic
        // =====================================================
        if (applySFMode) {
            double sf = 1.0;
            int htBin = -1, ptBin = -1;

            for (int i = 0; i < nBinsHT; ++i) {
                if (HT >= HT_bins[i] && HT < HT_bins[i+1]) { htBin = i+1; break; }
            }
            for (int i = 0; i < nBinspT; ++i) {
                if (Jet6PT >= pT_bins[i] && Jet6PT < pT_bins[i+1]) { ptBin = i+1; break; }
            }

            if (htBin != -1 && ptBin != -1) {
                sf = sfHist[0]->GetBinContent(htBin, ptBin);
                if (sf == 0) sf = 1.0;
            }

            if (!isData) new_weight = weight * sf;
            else         new_weight = weight;

            h_HT_Total->Fill(HT, weight);
            h_Jet6PT_Total->Fill(Jet6PT, weight);

            if (passHadTrig) {
                h_HT_Pass->Fill(HT, new_weight);
                h_Jet6PT_Pass->Fill(Jet6PT, new_weight);
            }

            // Tree 채우기
            outputTree->Fill();

        } else {
            h_Total[0][0]->Fill(HT, Jet6PT, weight);
            if (passHadTrig) {
                h_Pass[0][0]->Fill(HT, Jet6PT, weight);
            }
        }
    } 

    std::cout << "Event loop completed." << std::endl;

    // =========================================================
    // 6. Save Output
    // =========================================================
    outputFile->cd(); // 최종적으로 파일로 다시 이동

    if (applySFMode) {
        h_HT_Total->Write();
        h_HT_Pass->Write();
        h_Jet6PT_Total->Write();
        h_Jet6PT_Pass->Write();
        outputTree->Write(); // Tree 저장
    } else {
        h_Total[0][0]->Write();
        h_Pass[0][0]->Write();
    }
    
    outputFile->Close();
    delete outputFile; 
}

void EventLooper::Init()
{
   // [경로 설정] 본인의 환경에 맞게 수정하세요
   TString ntupleDir  = "/Users/jhlee/ttHH/ntuple/skimmed/gen_tier3/";
   TString ntupleName = getInputName(); 
   TString ntuplePath = ntupleDir + ntupleName;
   
   std::cout << "ntuple path : " << ntuplePath << std::endl;

   TFile* f = TFile::Open(ntuplePath);
   if (!f || f->IsZombie()) {
       std::cerr << "[Error] Cannot open " << ntuplePath << std::endl;
       return;
   }

   TTree* tree = nullptr;
   f->GetObject("Tree/Tree", tree);
   if (!tree) {
       std::cerr << "[Error] No TTree 'Tree' in file" << std::endl;
       f->Close();
       return;
   }

   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   
   // [매우 중요] vector 브랜치 처리를 위해 MakeClass 비활성화
   fChain->SetMakeClass(0); 

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
   fChain->SetBranchAddress("HT", &HT, &b_HT);
   fChain->SetBranchAddress("jetPt", &jetPt, &b_jetPt);
   fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
   fChain->SetBranchAddress("PUWeight", &PUWeight, &b_PUWeight);
   fChain->SetBranchAddress("L1PrefiringWeight", &L1PrefiringWeight, &b_L1PrefiringWeight);
   fChain->SetBranchAddress("failGoldenJson", &failGoldenJson, &b_failGoldenJson);
   fChain->SetBranchAddress("passMETFilters", &passMETFilters, &b_passMETFilters);

   Notify();
}

// 나머지 함수는 그대로 유지
bool EventLooper::Notify() { return true; }
void EventLooper::Show(Long64_t entry) { if (!fChain) return; fChain->Show(entry); }
Int_t EventLooper::Cut(Long64_t entry) { return 1; }
