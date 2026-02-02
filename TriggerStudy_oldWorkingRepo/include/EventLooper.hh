#ifndef EventLooper_hh
#define EventLooper_hh

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH2D.h>

// Header file for the classes stored in the TTree if any.

class EventLooper {

    public :
       TTree          *fChain;   //!pointer to the analyzed TTree or TChain
       Int_t           fCurrent; //!current Tree number in a TChain
    
       // Fixed size dimensions of array or collections stored in the TTree if any.
    
       // Declaration of leaf types
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

//       Float_t         jetPt[30];    // 30 is the maximum setted # of jets
//       //Float_t         jetEta[30];
//       //Float_t         bTagScore[30];
//       //UInt_t          eventNumber;
//       //UInt_t          runNumber;
       std::vector<float> *jetPt;

       Float_t         genWeight;
       Float_t         PUWeight;
       Float_t         L1PrefiringWeight;
       Bool_t          failGoldenJson;
       Bool_t          passMETFilters;


       // List of branches
       TBranch        *b_passTrigger_HLT_IsoMu27;   //!
       TBranch        *b_passTrigger_HLT_PFHT1050;   //!
       TBranch        *b_passTrigger_6J1T_B;   //!
       TBranch        *b_passTrigger_6J1T_CDEF;   //!
       TBranch        *b_passTrigger_6J2T_B;   //!
       TBranch        *b_passTrigger_6J2T_CDEF;   //!
       TBranch        *b_passTrigger_4J3T_B;   //!
       TBranch        *b_passTrigger_4J3T_CDEF;   //!
       TBranch        *b_nMuons;   //!
       TBranch        *b_nElecs;   //!
       TBranch        *b_nJets;   //!
       TBranch        *b_nbJets;   //!
       TBranch        *b_HT;   //!
       TBranch        *b_jetPt;   //!
       //TBranch        *b_jetEta;   //!
       //TBranch        *b_bTagScore;   //!
       //TBranch        *b_eventNumber;   //!
       //TBranch        *b_runNumber;   //!
       TBranch        *b_genWeight;
       TBranch        *b_PUWeight;
       TBranch        *b_L1PrefiringWeight;
       TBranch        *b_failGoldenJson;
       TBranch        *b_passMETFilters;


       EventLooper(TTree *tree=0);
       virtual ~EventLooper();
       virtual Int_t    Cut(Long64_t entry);
       virtual Int_t    GetEntry(Long64_t entry);
       virtual Long64_t LoadTree(Long64_t entry);
       virtual void     Init();
       virtual void     Loop();
       virtual bool     Notify();
       virtual void     Show(Long64_t entry = -1);

       void setNtupleName(TString _name = "notDefined");
       TString getInputName();
       TString getOutputName();

    private:

       TString ntupleName = "notDefined";

       // 스케일 팩터 계산을 위한 변수들
       Double_t HT_bins[7];
       Int_t nBinsHT;
       Double_t pT_bins[7];
       Int_t nBinspT;

       // Eta 분할을 켜고 끌 수 있는 스위치
       bool useEtaBinning;

       // Eta bins
       Double_t eta_bins[9];
       Int_t nEtaBins;

       // Number of b-jets bins (범위로 설정)
       Double_t nBjets_bins[3]; // 3, 4 ~ 12
       Int_t nBjetBins;

       // 히스토그램 배열
       // Eta 분할을 사용할 경우 2차원 배열, 아니면 1차원 배열
       TH2D* h_Total[9][4]; // 최대 크기로 선언
       TH2D* h_Pass[9][4];

       // 데이터 여부 확인 변수
       bool isData;

};

#endif

#ifdef EventLooper_cxx
EventLooper::EventLooper(TTree *tree) : fChain(0) 
{
    jetPt = 0;
}

EventLooper::~EventLooper()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t EventLooper::GetEntry(Long64_t entry)
{
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t EventLooper::LoadTree(Long64_t entry)
{
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void EventLooper::setNtupleName(TString _name){
    ntupleName = _name;
}
TString EventLooper::getInputName(){
    return ntupleName + ".root";
}
TString EventLooper::getOutputName(){
    return "output_" + ntupleName + ".root";
}

#endif // #ifdef EventLooper_cxx

