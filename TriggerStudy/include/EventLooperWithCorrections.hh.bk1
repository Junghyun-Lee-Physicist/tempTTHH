#ifndef EventLooperWithCorrections_hh
#define EventLooperWithCorrections_hh

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH2D.h>

class EventLooperWithCorrections {

    public :
       TTree          *fChain;   //!pointer to the analyzed TTree or TChain
       Int_t           fCurrent; //!current Tree number in a TChain
    

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
       //Int_t           nbJets;
       Float_t         HT;
       Float_t         jetPt[30];    // 30 is the maximum setted # of jets
       //Float_t         jetEta[30];
       //Float_t         bTagScore[30];
       //UInt_t          eventNumber;
       //UInt_t          runNumber;
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
       //TBranch        *b_nbJets;   //!
       TBranch        *b_HT;   //!
       TBranch        *b_jetPt;   //!
       //TBranch        *b_jetEta;   //!
       //TBranch        *b_bTagScore;   //!
       //TBranch        *b_eventNumber;   //!
       //TBranch        *b_runNumber;   //!
       TBranch        *b_PUWeight;
       TBranch        *b_L1PrefiringWeight;
       TBranch        *b_failGoldenJson;
       TBranch        *b_passMETFilters;

    
       EventLooperWithCorrections(TTree *tree=0);
       virtual ~EventLooperWithCorrections();
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

       // 스케일 팩터를 적용하기 위한 변수들
       Double_t HT_bins[7];
       Int_t nBinsHT;
       Double_t pT_bins[7];
       Int_t nBinspT;

       // Number of b-jets bins (범위로 설정)
       Double_t nBjets_bins[3]; // 3, 4 ~ 12
       Int_t nBjetBins;

       // Eta 분할을 사용할지 여부
       bool useEtaBinning;
       Double_t eta_bins[9];
       Int_t nEtaBins;

       // 스케일 팩터 히스토그램
       TH2D* sfHist[4]; // 최대 크기로 선언 (Eta bins x b-jet bins)

       // 데이터 여부 확인 변수
       bool isData;

       // 추가된 히스토그램
       TH1D* h_HT_Total;
       TH1D* h_HT_Pass;
       TH1D* h_Jet6PT_Total;
       TH1D* h_Jet6PT_Pass;
       //TH1I* h_nJets_Total;
       //TH1I* h_nJets_Pass;
       //TH1D* h_Eta_Total;
       //TH1D* h_Eta_Pass;

};

#endif

#ifdef EventLooperWithCorrections_cxx
EventLooperWithCorrections::EventLooperWithCorrections(TTree *tree) : fChain(0) 
{

}

EventLooperWithCorrections::~EventLooperWithCorrections()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t EventLooperWithCorrections::GetEntry(Long64_t entry)
{
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t EventLooperWithCorrections::LoadTree(Long64_t entry)
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

void EventLooperWithCorrections::setNtupleName(TString _name){
    ntupleName = _name;
}
TString EventLooperWithCorrections::getInputName(){
    return ntupleName + ".root";
}
TString EventLooperWithCorrections::getOutputName(){
    return "corrected_" + ntupleName + ".root";
}

#endif // #ifdef EventLooperWithCorrections_cxx

