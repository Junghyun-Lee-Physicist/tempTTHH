///////////////////////////////////////////////////////////////////////////
// applyDeepJetSF.C
// ----------------
// - Standalone 예시 코드 (CMSSW X), btagDeepFlavB(DeepJet) 분포 reshaping SF를
//   reshaping_deepJet_106XUL17_v3.csv를 통해 적용.
//
// - 1) "BTagCalibrationStandalone.h"/".cpp" 2개 파일을 같은 디렉토리에 둔다
// - 2) "reshaping_deepJet_106XUL17_v3.csv" 파일도 같은 디렉토리에 둔다
// - 3) "myNanoAOD.root" (예시) 파일에 "Events" 트리가 있고,
//    Jet_pt, Jet_eta, Jet_btagDeepFlavB, Jet_hadronFlavour 브랜치가 있다고 가정
//
// 실행:
//   root -l -b -q 'applyDeepJetSF.C'
///////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <vector>
#include <cmath>      // for fabs
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

// (1) BTagCalibrationStandalone 헤더/구현체 include
#include "BTagCalibrationStandalone.h"
#include "BTagCalibrationStandalone.cpp"

void applyDeepJetSF()
{
  // -----------------------------------------------------------------------
  // 0. 입력 ROOT 파일 (NanoAOD 예시) 열기
  //    Events라는 TTree를 가져온다고 가정
  // -----------------------------------------------------------------------
  TFile* fIn = TFile::Open("sample/output_ttJets.root");
  if (!fIn || fIn->IsZombie()) {
    std::cerr << "[ERROR] Cannot open input file\n";
    return;
  }

  TDirectory* dir = (TDirectory*)fIn->Get("Tree");
  TTree* tree = dynamic_cast<TTree*>( dir->Get("Tree") );
  if (!tree) {
    std::cerr << "[ERROR] TTree 'Events' not found in file\n";
    return;
  }

  // -----------------------------------------------------------------------
  // 1. TTree 브랜치 연결 (Jet_pt, Jet_eta, Jet_btagDeepFlavB, Jet_hadronFlavour 등)
  //    - NanoAOD라면 Jet_ prefix를 갖는다.
  // -----------------------------------------------------------------------
  // 고정 배열 크기와 추가 변수 선언
  const int MAX_JETS = 30;
  float jetPt[MAX_JETS];
  float jetEta[MAX_JETS];
  float bTagScore[MAX_JETS];
  int hadFlavs[MAX_JETS];
  int nJet = 0;
  
  // SetBranchAddress로 연결
  tree->SetBranchAddress("jetPt", jetPt);
  tree->SetBranchAddress("jetEta", jetEta);
  tree->SetBranchAddress("bTagScore", bTagScore);
  tree->SetBranchAddress("hadFlavs", hadFlavs);
  tree->SetBranchAddress("nJets", &nJet);


  // -----------------------------------------------------------------------
  // 2. BTagCalibration / BTagCalibrationReader 초기화
  // -----------------------------------------------------------------------
  //  - 첫 번째 인자 "deepJet"는 tagger 이름(아무 문자열이나 가능)
  //  - 두 번째 인자:  reshaping_deepJet_106XUL17_v3.csv
  BTagCalibration calib("deepJet", "reshaping_deepJet_106XUL17_v3.csv");

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

  // -----------------------------------------------------------------------
  // 3. 출력 히스토그램 준비 (예시)
  // -----------------------------------------------------------------------
  // DeepJet 스코어(before SF vs after SF) 예시
  TH1F* hDeepJet_noSF = new TH1F("hDeepJet_noSF","DeepJet Score (noSF)",50,0,1);
  TH1F* hDeepJet_withSF = new TH1F("hDeepJet_withSF","DeepJet Score (with SF)",50,0,1);

  // -----------------------------------------------------------------------
  // 4. 이벤트 루프
  // -----------------------------------------------------------------------
  const Long64_t nEntries = tree->GetEntries();
  std::cout << "[INFO] #Events = " << nEntries << std::endl;

  for (Long64_t i=0; i<nEntries; i++)
  {
    tree->GetEntry(i);

    // nJet을 초과하지 않도록 제어
    if (nJet > MAX_JETS) {
        std::cerr << "[WARNING] nJet exceeds MAX_JETS, truncating\n";
        nJet = MAX_JETS;
    }

    // -- (A) 임시로 '원래' 이벤트 weight = 1.0 (추후 genWeight, PUweight, ... 곱 가능)
    double eventWeight_noSF = 1.0;
    double eventWeight_withSF = 1.0;

    // -- (B) 제트 루프
    for (size_t j = 0; j < nJet; j++)
    {
      float pt   = jetPt[j];
      float eta  = jetEta[j];
      float disc = bTagScore[j];   // DeepJet score
      int hadFlv = hadFlavs[j];

      //Jet flavor 변환(b=5,c=4,udsg=0)
      BTagEntry::JetFlavor jf = BTagEntry::FLAV_UDSG; 
      if      (hadFlv == 5) jf = BTagEntry::FLAV_B;
      else if (hadFlv == 4) jf = BTagEntry::FLAV_C;
      else                  jf = BTagEntry::FLAV_UDSG; // 0,1,2 => UDSG

      // noSF (그냥 히스토그램에 채우기)
      // "스코어 자체" 분포를 채우지만, 일단 weight=1
      if(j==0) hDeepJet_noSF->Fill(disc, eventWeight_noSF);

      // shaping SF 가져오기
      double sf = reader.eval(jf, std::fabs(eta), pt, disc);
      // eventWeight에 곱
      eventWeight_withSF *= sf;
    }

    // (C) 모든 jet의 SF를 곱한 최종 eventWeight
    //     (만약 여러 개 레벨 weight가 있으면 더 곱할 수도 있음)
    //     여기서는 demo로 "DeepJet SF"만 적용했다고 치자

    // DeepJet 분포 (withSF)도 한번 임의로 채워보자.
    //  - "shaping" SF는 제트의 분포를 변경시키는 개념이긴 하지만
    //    여기서는 흔히 event-level 가중치로도 구현 가능함(POG 추천).
    //  - 보통은 "모든 jet 스코어"를 2D/3D로 보정하거나, 
    //    or pseudo-분포 weight로 씀.
    // 여기서는 단순화해서 "임의로 첫 번째 jet score"만 히스토그램에 채운다고 예시.
    if (nJet > 0) {
      float disc0 = bTagScore[0];
      hDeepJet_withSF->Fill(disc0, eventWeight_withSF);
    }
  } // event loop

  // -----------------------------------------------------------------------
  // 5. 결과 저장
  // -----------------------------------------------------------------------
  TFile* fOut = new TFile("result_withDeepJetSF.root","RECREATE");
  hDeepJet_noSF->Write();
  hDeepJet_withSF->Write();
  fOut->Close();

  fIn->Close();

  std::cout << "[INFO] Done. Output = result_withDeepJetSF.root" << std::endl;
}
