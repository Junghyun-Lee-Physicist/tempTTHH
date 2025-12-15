// src/EventLooperWithCorrections.cpp
#define EventLooperWithCorrections_cxx
#include "EventLooperWithCorrections.hh"
#include "BinConfig.hh"

#include <TH2.h>
#include <TFile.h>
#include <TStyle.h>
#include <TMath.h>
#include <iostream>
#include <map>
#include <utility>

// =====================================================
// 파일-스코프: 클로저 디버깅용 누적 카운터(헤더 수정 불필요)
//   - 각 (HT bin, pT6 bin)에서 정수/가중 합을 동시에 기록
// =====================================================
struct EffDbg {
  long long nTot = 0, nPass = 0;   // unweighted counts
  double    wTot = 0., wPass = 0.; // weighted sums
};

static std::map<std::pair<int,int>, EffDbg> gDbgData; // key=(iHT,ipT6)
static std::map<std::pair<int,int>, EffDbg> gDbgMC;
static long long gDataB = 0, gDataCDEF = 0, gMC = 0;

// =====================================================
// Init: (헤더에 존재하는 브랜치/멤버만 사용)
// =====================================================
void EventLooperWithCorrections::Init()
{
  TString ntupleDir  = "/Users/jhlee/ttHH/ntuple/skimmed/";
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

  std::cout << "TFile is opened!!" << std::endl;

  fChain = tree;
  fCurrent = -1;
  fChain->SetMakeClass(1);

  // ---- Trigger branches (헤더에 선언된 것만) ----
  fChain->SetBranchAddress("passTrigger_HLT_IsoMu27", &passTrigger_HLT_IsoMu27, &b_passTrigger_HLT_IsoMu27);
  fChain->SetBranchAddress("passTrigger_HLT_PFHT1050", &passTrigger_HLT_PFHT1050, &b_passTrigger_HLT_PFHT1050);
  fChain->SetBranchAddress("passTrigger_6J1T_B", &passTrigger_6J1T_B, &b_passTrigger_6J1T_B);
  fChain->SetBranchAddress("passTrigger_6J1T_CDEF", &passTrigger_6J1T_CDEF, &b_passTrigger_6J1T_CDEF);
  fChain->SetBranchAddress("passTrigger_6J2T_B", &passTrigger_6J2T_B, &b_passTrigger_6J2T_B);
  fChain->SetBranchAddress("passTrigger_6J2T_CDEF", &passTrigger_6J2T_CDEF, &b_passTrigger_6J2T_CDEF);
  fChain->SetBranchAddress("passTrigger_4J3T_B", &passTrigger_4J3T_B, &b_passTrigger_4J3T_B);
  fChain->SetBranchAddress("passTrigger_4J3T_CDEF", &passTrigger_4J3T_CDEF, &b_passTrigger_4J3T_CDEF);

  // ---- Objects/Counts ----
  fChain->SetBranchAddress("nMuons", &nMuons, &b_nMuons);
  fChain->SetBranchAddress("nElecs", &nElecs, &b_nElecs);
  fChain->SetBranchAddress("nJets",  &nJets,  &b_nJets);
  fChain->SetBranchAddress("HT",     &HT,     &b_HT);
  fChain->SetBranchAddress("jetPt",  &jetPt,  &b_jetPt);

  // ---- Weights/filters ----
  fChain->SetBranchAddress("PUWeight",          &PUWeight, &b_PUWeight);
  fChain->SetBranchAddress("L1PrefiringWeight", &L1PrefiringWeight, &b_L1PrefiringWeight);
  fChain->SetBranchAddress("failGoldenJson",    &failGoldenJson, &b_failGoldenJson);
  fChain->SetBranchAddress("passMETFilters",    &passMETFilters, &b_passMETFilters);

  Notify();
}

// =====================================================
// Loop: 트리거 판정 + 히스토 Fill(지역 히스토) + 디버깅 요약
//   - 헤더/다른 코드 변경 없이 트리거/히스토 부분만 관리
// =====================================================
void EventLooperWithCorrections::Loop()
{
  if (fChain == 0) return;

  // -----------------------------------------
  // 입력 이름 파싱 (기존 로직 유지)
  // -----------------------------------------
  TString sampleName = getInputName();
  Ssiz_t rootPos = sampleName.Index(".root");
  if (rootPos != kNPOS) sampleName.Remove(rootPos);
  std::cout << "Sample Name: " << sampleName << std::endl;

  isData = true;
  TString channel = "";
  if (sampleName == "TTTo2L2Nu"){
    isData = false; channel = "diLep";
    std::cout << "Current Sample: MC TTTo2L2Nu" << std::endl;
  } else if (sampleName == "ttTohadronic"){
    isData = false; channel = "had";
    std::cout << "Current Sample: MC ttTohadronic" << std::endl;
  } else if (sampleName == "TTToSemiLeptonic"){
    isData = false; channel = "semiLep";
    std::cout << "Current Sample: MC TTToSemiLeptonic" << std::endl;
  } else {
    std::cout << "Current Sample: Data" << std::endl;
  }

  TString dataSet = "default";
  TString era     = "default";
  Ssiz_t underscorePos = sampleName.Index("_");
  if (isData) {
    if (underscorePos != kNPOS) {
      dataSet = sampleName(0, underscorePos);
      era     = sampleName(underscorePos + 1, sampleName.Length() - underscorePos - 1);
      std::cout<< "  [ EventLooperWithCorrections::Loop() ] : Current Sample --> Data "
               << dataSet << ", Era : " << era << std::endl;
    } else {
      std::cout << "  [ EventLooperWithCorrections::Loop() ] : No underscore found in the string!" << std::endl;
      return;
    }
  }

  // -----------------------------------------
  // bin 경계 캐시 (기존 BinConfig 사용)
  // -----------------------------------------
  const auto& HT_bins_vec = BinConfig::getHTBins();
  nBinsHT = BinConfig::HTBinCount;
  for (int i=0;i<=nBinsHT;++i) HT_bins[i] = HT_bins_vec[i];

  const auto& pT_bins_vec = BinConfig::getPTBins();
  nBinspT = BinConfig::PTBinCount;
  for (int i=0;i<=nBinspT;++i) pT_bins[i] = pT_bins_vec[i];

  // Eta/b-jet 분할은 꺼둠(히스토 1×1)
  useEtaBinning = false;

  // -----------------------------------------
  // 지역 히스토그램 예약 (헤더에 의존하지 않음)
  // -----------------------------------------
  TH2D* hTotal[1][1] = {{nullptr}};
  TH2D* hPass [1][1] = {{nullptr}};

  {
    int iEta = 0, iBjet = 0;
    TString histName_Total = Form("h_Total_Bjet%d", iBjet);
    TString histTitle_Total = Form("Total Events (b-jet bin %d);HT [GeV];6th Jet p_{T} [GeV]", iBjet);
    TString histName_Pass  = Form("h_Pass_Bjet%d",  iBjet);
    TString histTitle_Pass = Form("Passed Events (b-jet bin %d);HT [GeV];6th Jet p_{T} [GeV]", iBjet);
    hTotal[iEta][iBjet] = new TH2D(histName_Total, histTitle_Total, nBinsHT, HT_bins, nBinspT, pT_bins);
    hPass [iEta][iBjet] = new TH2D(histName_Pass,  histTitle_Pass,  nBinsHT, HT_bins, nBinspT, pT_bins);
  }

  // -----------------------------------------
  // 메인 루프
  // -----------------------------------------
  Long64_t nentries = fChain->GetEntriesFast();
  std::cout << "Entries: " << nentries << std::endl;

  // 모드:
  //  true  → SingleMuon에서 SF "측정"(Tag&Probe). 루프 내부 SF 곱하지 않음.
  //  false → "적용/검증". (권장) 루프 내부 SF 곱하지 말고, 플롯 코드에서 사후 곱셈 사용.
  const bool makeSFMode = true;
  const bool kApplySFInsideLooper = false; // post-multiplication 사용 시 false 유지(권장)

  for (Long64_t jentry=0; jentry<nentries; ++jentry) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    fChain->GetEntry(jentry);

    // ---------------- 베이스라인 (기존 유지) ----------------
    if (jetPt->size() != nJets) {
      std::cout << "[ ERROR ] : jet vector size != number of jets" << std::endl;
      exit(55);
    }

    if (!(nMuons == 1 && nElecs == 0)) continue;

    if (nJets < 7) continue;
//    if (nJets < 6) {
//      std::cerr << "[ERROR] nJets (" << nJets << ") are smaller than 6.." << std::endl;
//      exit(2);
//    }

    if (!passMETFilters) {
      std::cerr << "[ERROR] It did not pass the noise filters.." << std::endl;
      exit(3);
    }

    double Jet6PT = jetPt->at(5);
    if (Jet6PT <= 40.0) {
      std::cerr << "[ERROR] Jet6th pT (" << Jet6PT << ") are <= 40.0 GeV.." << std::endl;
      exit(4);
    }

    if (HT < 500.0) {
      std::cerr << "[ERROR] HT (" << HT << ") are < 500.0 GeV.." << std::endl;
      exit(5);
    }

    double weight = 1.0;
    if (!isData) {
      if      (channel == "diLep")   weight = 0.1419595905 * L1PrefiringWeight * PUWeight;
      else if (channel == "had")     weight = 0.06745986328 * L1PrefiringWeight * PUWeight;
      else if (channel == "semiLep") weight = 0.01049544017 * L1PrefiringWeight * PUWeight;
      else                           weight = L1PrefiringWeight * PUWeight;
    } else {
      if (failGoldenJson) {
        std::cerr << "[ERROR] It did not pass the golden json.." << std::endl;
        exit(6);
      }
    }

    // ---------------- era 카운트(디버그) ----------------
    if (isData) { if (era=="B") ++gDataB; else ++gDataCDEF; } else ++gMC;

    // ---------------- hadronic OR 정의 (AN과 동일) ----------------
    const bool fired6J1T = (isData && era=="B") ? passTrigger_6J1T_B   : passTrigger_6J1T_CDEF;
    const bool fired6J2T = (isData && era=="B") ? passTrigger_6J2T_B   : passTrigger_6J2T_CDEF;
    const bool fired4J3T = (isData && era=="B") ? passTrigger_4J3T_B   : passTrigger_4J3T_CDEF;
    const bool firedHT   =                          passTrigger_HLT_PFHT1050;
    const bool passORHad = (fired4J3T || fired6J1T || fired6J2T || firedHT);

    // ---------------- SingleMuon Tag (SF 측정 모드에서만 강제) ----------------
    if (makeSFMode) {
      if (!passTrigger_HLT_IsoMu27) {
        // Tag 실패 → 분모/분자 모두 제외
        continue;
      }
    }

    // ---------------- 히스토 채우기 (지역 히스토) ----------------
    int etaIdx = 0; // 분할 OFF (1x1)
    int nbIdx  = 0;

    if (hTotal[etaIdx][nbIdx]) hTotal[etaIdx][nbIdx]->Fill(HT, Jet6PT, weight);

    double passWeight = weight;

    // (비권장) 루프 내부에서 SF 곱하기 — TEfficiency 제약 때문에 기본 OFF
    if (!makeSFMode && !isData && kApplySFInsideLooper) {
       //double sf = lookupSF(HT, Jet6PT); // (필요시) 당신 SF 룩업 호출
       //passWeight *= sf;
    }

    if (passORHad) {
      if (hPass[etaIdx][nbIdx]) hPass[etaIdx][nbIdx]->Fill(HT, Jet6PT, passWeight);
    }

    // ---------------- 디버그 누적 (bin별 정리; 헤더 의존 X) ----------------
    int iHT  = hTotal[etaIdx][nbIdx]->GetXaxis()->FindBin(HT);
    int ipT6 = hTotal[etaIdx][nbIdx]->GetYaxis()->FindBin(Jet6PT);
    auto& M = isData ? gDbgData : gDbgMC;
    EffDbg& cell = M[{iHT, ipT6}];
    cell.nTot += 1;
    cell.wTot += weight;
    if (passORHad) {
      cell.nPass += 1;
      cell.wPass += passWeight;
    }

    // (선택) 특정 구간 상세 프린트 — 필요 시 주석 해제
    /*
    if (HT>500 && HT<700 && Jet6PT>40 && Jet6PT<60) {
      std::cout << "[DBG] HT="<<HT<<" pT6="<<Jet6PT
                << " isData="<<isData
                << " era="<<era
                << " Tag="<<passTrigger_HLT_IsoMu27
                << " 4J3T="<<fired4J3T
                << " 6J1T="<<fired6J1T
                << " 6J2T="<<fired6J2T
                << " HT1050="<<firedHT
                << " OR="<<passORHad
                << " w="<<weight
                << std::endl;
    }
    */
  } // end entries loop

  std::cout << "Event loop completed." << std::endl;

  // -----------------------------------------
  // 디버그 요약 출력 (파일 저장 전) — bin별 가중 효율/정수 카운트 비교
  // -----------------------------------------
  auto dump = [](const char* tag,
                 const std::map<std::pair<int,int>, EffDbg>& M,
                 TH2D* hRef)
  {
    std::cout<<"["<<tag<<"] per-bin: (iHT,ipT6)  nPass/nTot  wPass/wTot  eff_w  @centers\n";
    for (auto& kv : M) {
      int iHT  = kv.first.first;
      int ipT6 = kv.first.second;
      const EffDbg& c = kv.second;
      double effw = (c.wTot>0) ? (c.wPass/c.wTot) : -1.;
      double htC  = hRef->GetXaxis()->GetBinCenter(iHT);
      double ptC  = hRef->GetYaxis()->GetBinCenter(ipT6);
      std::cout<<"  ("<<iHT<<","<<ipT6<<") "
               << c.nPass<<"/"<<c.nTot << "   "
               << c.wPass<<"/"<<c.wTot << "   "
               << effw << "   "
               << "[HT~"<<htC<<", pT6~"<<ptC<<"]\n";
    }
  };

  TH2D* hRef = hTotal[0][0];
  std::cout << "\n=== DBG SUMMARY ===\n";
  std::cout << "Data era counts: B=" << gDataB << "  CDEF=" << gDataCDEF << "\n";
  std::cout << "MC events: " << gMC << "\n";
  if (hRef) { dump("DATA", gDbgData, hRef); dump("MC  ", gDbgMC, hRef); }
  std::cout << "=== END DBG ===\n";

  // -----------------------------------------
  // 히스토 저장 (지역 히스토를 파일에 기록)
  // -----------------------------------------
  TFile* outputFile = new TFile(getOutputName(), "RECREATE");
  {
    int iEta=0, iBjet=0;
    outputFile->cd();
    if (hTotal[iEta][iBjet]) hTotal[iEta][iBjet]->Write();
    if (hPass [iEta][iBjet]) hPass [iEta][iBjet]->Write();
  }
  outputFile->Close();

  // 지역 히스토 소멸(선택)
  int iEta=0, iBjet=0;
  delete hTotal[iEta][iBjet]; hTotal[iEta][iBjet]=nullptr;
  delete hPass [iEta][iBjet]; hPass [iEta][iBjet]=nullptr;
}

// 유지 보일러플레이트
bool EventLooperWithCorrections::Notify() { return true; }
void EventLooperWithCorrections::Show(Long64_t entry) { if (!fChain) return; fChain->Show(entry); }
Int_t EventLooperWithCorrections::Cut(Long64_t) { return 1; }
