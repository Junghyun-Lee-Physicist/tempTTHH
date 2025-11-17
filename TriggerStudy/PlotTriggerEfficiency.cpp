// PlotTriggerEfficiency.cpp
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TPad.h>
#include <TLine.h>
#include <TROOT.h>
#include <TString.h>
#include <TEfficiency.h>
#include <iostream>
#include <vector>
#include <algorithm>

static void ensureSumw2(TH1* h){ if(h && h->GetSumw2N()==0) h->Sumw2(); }

// binning 완전 일치 확인 (가변 bin 고려)
static bool sameBinning(const TH1D* a, const TH1D* b) {
  if(!a || !b) return false;
  if(a->GetNbinsX()!=b->GetNbinsX()) return false;
  const TArrayD* xa = a->GetXaxis()->GetXbins();
  const TArrayD* xb = b->GetXaxis()->GetXbins();
  if(xa->GetSize()!=xb->GetSize()) return false;
  for (int i=0;i<xa->GetSize();++i)
    if (xa->GetAt(i)!=xb->GetAt(i)) return false;
  return true;
}

// 1D 있으면 그대로 사용, 없으면 2D에서 자동 프로젝션 (X=HT, Y=Jet6PT)
// 새 히스토는 gROOT 소유로 Clone해서 파일 close와 소유권 분리
static std::pair<TH1D*,TH1D*> fetch1D(TFile* f, bool forHT){
  if(!f){ std::cerr<<"[fetch1D] null TFile\n"; return {nullptr,nullptr}; }
  gROOT->cd();

  if(forHT){
    auto* tot = dynamic_cast<TH1D*>(f->Get("h_HT_Total"));
    auto* pas = dynamic_cast<TH1D*>(f->Get("h_HT_Pass"));
    if(tot && pas){
      std::cout<<"[fetch1D:HT] found 1D h_HT_Total/h_HT_Pass\n";
      return { (TH1D*)tot->Clone("h_HT_Total_clone"),
               (TH1D*)pas->Clone("h_HT_Pass_clone") };
    }
  }else{
    auto* tot = dynamic_cast<TH1D*>(f->Get("h_Jet6PT_Total"));
    auto* pas = dynamic_cast<TH1D*>(f->Get("h_Jet6PT_Pass"));
    if(tot && pas){
      std::cout<<"[fetch1D:Jet6] found 1D h_Jet6PT_Total/h_Jet6PT_Pass\n";
      return { (TH1D*)tot->Clone("h_Jet6PT_Total_clone"),
               (TH1D*)pas->Clone("h_Jet6PT_Pass_clone") };
    }
  }

  auto* h2T = dynamic_cast<TH2D*>(f->Get("h_Total_Bjet0"));
  auto* h2P = dynamic_cast<TH2D*>(f->Get("h_Pass_Bjet0"));
  if(h2T && h2P){
    std::cout<<"[fetch1D:"<<(forHT?"HT":"Jet6")<<"] 2D found → project "
             << (forHT?"X(HT)":"Y(Jet6PT)") << "\n";
    if(forHT){
      auto* tot = h2T->ProjectionX("h_HT_Total_auto");
      auto* pas = h2P->ProjectionX("h_HT_Pass_auto");
      gROOT->cd();
      return { (TH1D*)tot->Clone("h_HT_Total_auto_clone"),
               (TH1D*)pas->Clone("h_HT_Pass_auto_clone") };
    }else{
      auto* tot = h2T->ProjectionY("h_Jet6PT_Total_auto");
      auto* pas = h2P->ProjectionY("h_Jet6PT_Pass_auto");
      gROOT->cd();
      return { (TH1D*)tot->Clone("h_Jet6PT_Total_auto_clone"),
               (TH1D*)pas->Clone("h_Jet6PT_Pass_auto_clone") };
    }
  }

  std::cerr<<"[fetch1D:"<<(forHT?"HT":"Jet6")<<"] MISSING histograms.\n";
  return {nullptr,nullptr};
}

// 가중 이벤트 대응: 인스턴스 메서드 사용
static TEfficiency* makeEff(TH1D* hPass, TH1D* hTot, const char* name){
  if(!hPass || !hTot) return nullptr;
  if(!sameBinning(hPass,hTot)){
    std::cerr<<"[makeEff] pass/total binning mismatch for "<<name<<"\n";
    return nullptr;
  }
  ensureSumw2(hTot); ensureSumw2(hPass);
  auto* eff = new TEfficiency(*hPass, *hTot);
  eff->SetName(name);
  eff->SetUseWeightedEvents(true);
  return eff;
}

// SF = ε_data / ε_mc (간단 오차 전파)
static TH1D* makeSF(const TEfficiency* eD, const TEfficiency* eM,
                    const char* name, const TH1D* templ){
  if(!eD || !eM || !templ) return nullptr;
  TH1D* sf = (TH1D*)templ->Clone(name);
  sf->Reset("ICESM");
  int nb = templ->GetNbinsX();
  for(int i=1;i<=nb;++i){
    double d = eD->GetEfficiency(i);
    double m = eM->GetEfficiency(i);
    double v = (m>0? d/m : 1.0);

    double eDu = eD->GetEfficiencyErrorUp(i),   eDd = eD->GetEfficiencyErrorLow(i);
    double eMu = eM->GetEfficiencyErrorUp(i),   eMd = eM->GetEfficiencyErrorLow(i);
    double eD  = 0.5*(eDu+eDd), eMerr = 0.5*(eMu+eMd);

    double rel2 = 0.0;
    if(d>0) rel2 += (eD/d)*(eD/d);
    if(m>0) rel2 += (eMerr/m)*(eMerr/m);
    double err = (d>0 && m>0)? v*std::sqrt(rel2) : 0.0;

    sf->SetBinContent(i, v);
    sf->SetBinError  (i, err);
  }
  return sf;
}

// ε_pred = ε_MC_raw × SF  (사후 곱셈)
static TH1D* applySF_toEff(const TEfficiency* eMCraw, const TH1D* sf,
                           const char* name, const TH1D* templ){
  if(!eMCraw || !sf || !templ) return nullptr;
  TH1D* pred = (TH1D*)templ->Clone(name);
  pred->Reset("ICESM");
  int nb = templ->GetNbinsX();
  for(int i=1;i<=nb;++i){
    double e = eMCraw->GetEfficiency(i);
    double s = sf->GetBinContent(i);
    double v = std::clamp(e*s, 0.0, 1.0);
    pred->SetBinContent(i, v);
  }
  return pred;
}

static TH1D* makeRatio(const TH1D* A, const TH1D* B, const char* name){
  if(!A || !B) return nullptr;
  if(!sameBinning(A,B)){
    std::cerr<<"[makeRatio] binning mismatch\n";
    return nullptr;
  }
  TH1D* R = (TH1D*)A->Clone(name);
  R->Reset("ICESM");
  int nb=A->GetNbinsX();
  for(int i=1;i<=nb;++i){
    double a=A->GetBinContent(i), b=B->GetBinContent(i);
    R->SetBinContent(i, b!=0? a/b : 0.0);
  }
  return R;
}

// ─────────────────────────────────────────────────────
// (1) RAW 플롯: Data(RAW) vs MC(RAW) + ratio(Data/MC)
// ─────────────────────────────────────────────────────
static void drawRaw(const char* outPdf,
                    TEfficiency* eData, TEfficiency* eMC,
                    const char* xTitle)
{
  if(!eData || !eMC){ std::cerr<<"[drawRaw] null input\n"; return; }
  gStyle->SetOptStat(0);

  // 프레임용 히스토 (MC total hist의 binning 사용)
  const TH1* hTotMC = eMC->GetTotalHistogram(); // owned by TEfficiency
  TH1D* frameH = new TH1D("frameRaw","", hTotMC->GetNbinsX(),
                          hTotMC->GetXaxis()->GetXbins()->GetArray());

  TCanvas c("cRaw","",900,900);
  TPad *p1 = new TPad("p1","",0,0.30,1,1);  p1->SetBottomMargin(0.02); p1->Draw();
  TPad *p2 = new TPad("p2","",0,0.00,1,0.30); p2->SetTopMargin(0.05); p2->SetBottomMargin(0.25); p2->Draw();

  // 상단
  p1->cd();
  frameH->Reset("ICESM");
  frameH->GetYaxis()->SetTitle("Efficiency");
  frameH->GetXaxis()->SetTitle(xTitle);
  frameH->GetYaxis()->SetRangeUser(0.0,1.05);
  frameH->Draw("AXIS");

  eData->SetLineWidth(2); eData->SetMarkerStyle(20);
  eMC  ->SetLineWidth(2); eMC  ->SetMarkerStyle(24);

  eData->Draw("P SAME");
  eMC  ->Draw("P SAME");

  auto* leg = new TLegend(0.55,0.18,0.88,0.40);
  leg->AddEntry(eData, "Data (RAW)", "lp");
  leg->AddEntry(eMC,   "MC (RAW)",   "lp");
  leg->Draw();

  // 하단 ratio = Data/MC
  p2->cd();
  TH1D* hd = (TH1D*)frameH->Clone("hd"); hd->Reset("ICESM");
  TH1D* hm = (TH1D*)frameH->Clone("hm"); hm->Reset("ICESM");
  for (int i=1;i<=hd->GetNbinsX();++i){
    hd->SetBinContent(i, eData->GetEfficiency(i));
    hm->SetBinContent(i, eMC  ->GetEfficiency(i));
  }
  TH1D* R = makeRatio(hd, hm, "R_raw");

  TH1D* frameR = (TH1D*)frameH->Clone("frameR"); frameR->Reset("ICESM");
  frameR->GetYaxis()->SetTitle("Data/MC"); frameR->GetXaxis()->SetTitle(xTitle);
  frameR->GetYaxis()->SetRangeUser(0.2,1.8); frameR->Draw("AXIS");
  if(R){ R->SetMarkerStyle(24); R->Draw("P SAME"); }
  TLine l(frameR->GetXaxis()->GetXmin(),1.0, frameR->GetXaxis()->GetXmax(),1.0);
  l.SetLineStyle(2); l.Draw();

  c.Print(outPdf);

  delete frameH; delete hd; delete hm; delete R; delete frameR;
}

// ─────────────────────────────────────────────────────
// (2) SF 적용 플롯: Data(RAW) vs MC×SF(Pred) + ratio(Data/(MC×SF))
// ─────────────────────────────────────────────────────
static void drawApplied(const char* outPdf,
                        TEfficiency* eData, TH1D* predMC,
                        const char* xTitle)
{
  if(!eData || !predMC){ std::cerr<<"[drawApplied] null input\n"; return; }
  gStyle->SetOptStat(0);

  TCanvas c("cSF","",900,900);
  TPad *p1 = new TPad("p1","",0,0.30,1,1);  p1->SetBottomMargin(0.02); p1->Draw();
  TPad *p2 = new TPad("p2","",0,0.00,1,0.30); p2->SetTopMargin(0.05); p2->SetBottomMargin(0.25); p2->Draw();

  // 상단
  p1->cd();
  TH1D* frame = (TH1D*)predMC->Clone("frame_applied"); frame->Reset("ICESM");
  frame->GetYaxis()->SetTitle("Efficiency"); frame->GetXaxis()->SetTitle(xTitle);
  frame->GetYaxis()->SetRangeUser(0.0,1.05); frame->Draw("AXIS");

  eData->SetLineWidth(2); eData->SetMarkerStyle(20);
  predMC->SetLineWidth(2); predMC->SetMarkerStyle(21);
  predMC->SetLineColor(kGreen+2); predMC->SetMarkerColor(kGreen+2);

  eData->Draw("P SAME");
  predMC->Draw("P SAME");

  auto* leg = new TLegend(0.55,0.18,0.88,0.40);
  leg->AddEntry(eData,  "Data (RAW)", "lp");
  leg->AddEntry(predMC, "MC #times SF (pred)", "lp");
  leg->Draw();

  // 하단 ratio = Data / (MC×SF)
  p2->cd();
  TH1D* hd = (TH1D*)predMC->Clone("hd"); hd->Reset("ICESM");
  for (int i=1;i<=hd->GetNbinsX();++i)
    hd->SetBinContent(i, eData->GetEfficiency(i));

  TH1D* R = (TH1D*)predMC->Clone("R_applied"); R->Reset("ICESM");
  for (int i=1;i<=R->GetNbinsX();++i){
    double a = hd->GetBinContent(i);
    double b = predMC->GetBinContent(i);
    R->SetBinContent(i, (b!=0? a/b : 0.0));
  }
  TH1D* frameR = (TH1D*)predMC->Clone("frameR_applied"); frameR->Reset("ICESM");
  frameR->GetYaxis()->SetTitle("Data/(MC#timesSF)"); frameR->GetXaxis()->SetTitle(xTitle);
  frameR->GetYaxis()->SetRangeUser(0.2,1.8); frameR->Draw("AXIS");

  R->SetMarkerStyle(21);
  R->SetMarkerColor(kGreen+2);
  R->SetLineColor(kGreen+2);
  R->Draw("P SAME");

  TLine l(frameR->GetXaxis()->GetXmin(),1.0, frameR->GetXaxis()->GetXmax(),1.0);
  l.SetLineStyle(2); l.Draw();

  c.Print(outPdf);

  delete frame; delete hd; delete R; delete frameR;
}

// ─────────────────────────────────────────────────────
// 엔트리 포인트
//   root -l -b -q 'PlotTriggerEfficiency.cpp("corrected_Data.root","corrected_ttJets.root","corrected_ttJets.root")'
// ─────────────────────────────────────────────────────
void PlotTriggerEfficiency(const char* dataFile,
                           const char* mcFile,
                           const char* mcApplyFile = "")
{
  gROOT->SetBatch(kTRUE);

  TFile* fD = TFile::Open(dataFile,"READ");
  TFile* fM = TFile::Open(mcFile,"READ");
  if(!fD || !fM || fD->IsZombie() || fM->IsZombie()){
    std::cerr<<"[ERR] cannot open data/mc files\n"; return;
  }
  TFile* fMA = nullptr;
  if(mcApplyFile && TString(mcApplyFile).Length()>0){
    fMA = TFile::Open(mcApplyFile,"READ");
    if(!fMA || fMA->IsZombie()){
      std::cerr<<"[WARN] mcApply open failed, fallback to mc\n"; fMA = fM;
    }
  } else fMA = fM;

  struct Axis { bool forHT; const char* xt; const char* sfroot; const char* sfn; };
  std::vector<Axis> axes = {
    {true,  "H_{T} [GeV]",         "SF_HT.root",     "SF_HT"},
    {false, "6th jet p_{T} [GeV]", "SF_Jet6PT.root", "SF_Jet6PT"}
  };

  for(const auto& ax: axes){
    std::cout<<"\n==== Axis: "<<(ax.forHT?"HT":"Jet6PT")<<" ====\n";
    auto [hDT, hDP] = fetch1D(fD, ax.forHT);
    auto [hMT, hMP] = fetch1D(fM, ax.forHT);
    if(!hDT || !hDP || !hMT || !hMP){
      std::cerr<<"[ERR] Missing histograms for "<<(ax.forHT?"HT":"Jet6PT")<<". Skip axis.\n";
      delete hDT; delete hDP; delete hMT; delete hMP;
      continue;
    }
    if( !sameBinning(hDT,hDP) || !sameBinning(hDT,hMT) || !sameBinning(hDT,hMP) ){
      std::cerr<<"[ERR] Binning mismatch among Data/MC "<<(ax.forHT?"HT":"Jet6PT")<<".\n";
      delete hDT; delete hDP; delete hMT; delete hMP;
      continue;
    }

    // RAW 효율
    auto* eD = makeEff(hDP,hDT, ax.forHT? "effData_HT":"effData_Jet6");
    auto* eM = makeEff(hMP,hMT, ax.forHT? "effMC_HT"  :"effMC_Jet6");
    if(!eD || !eM){
      std::cerr<<"[ERR] TEfficiency make failed for "<<(ax.forHT?"HT":"Jet6PT")<<"\n";
      delete hDT; delete hDP; delete hMT; delete hMP;
      delete eD; delete eM;
      continue;
    }

    // SF 저장
    TH1D* sf = makeSF(eD,eM, ax.sfn, hDT);
    if(!sf){
      std::cerr<<"[ERR] SF build failed\n";
      delete hDT; delete hDP; delete hMT; delete hMP;
      delete eD; delete eM;
      continue;
    }
    { TFile fout(ax.sfroot,"RECREATE"); sf->Write(); std::cout<<"[OUT] wrote "<<ax.sfroot<<" ("<<ax.sfn<<")\n"; }

    // 적용 대상 MC
    auto [hAT, hAP] = fetch1D(fMA, ax.forHT);
    if(!hAT || !hAP || !sameBinning(hDT,hAT) || !sameBinning(hDT,hAP)){
      std::cerr<<"[ERR] Apply-MC hist missing or binning mismatch for "<<(ax.forHT?"HT":"Jet6PT")<<". Skip plots.\n";
      delete hDT; delete hDP; delete hMT; delete hMP; delete hAT; delete hAP;
      delete eD; delete eM; delete sf;
      continue;
    }
    auto* eMA = makeEff(hAP,hAT, ax.forHT? "effMC_apply_HT":"effMC_apply_Jet6");
    if(!eMA){
      std::cerr<<"[ERR] TEfficiency make failed (apply)\n";
      delete hDT; delete hDP; delete hMT; delete hMP; delete hAT; delete hAP;
      delete eD; delete eM; delete sf;
      continue;
    }

    // ε_pred = ε_MC_apply_raw × SF
    TH1D* epsPred = applySF_toEff(eMA, sf, ax.forHT? "epsPred_HT":"epsPred_Jet6", hAT);
    if(!epsPred){
      std::cerr<<"[ERR] epsPred build failed\n";
      delete hDT; delete hDP; delete hMT; delete hMP; delete hAT; delete hAP;
      delete eD; delete eM; delete eMA; delete sf;
      continue;
    }

    // (A) RAW 플롯
    {
      TString rawPdf = TString::Format("TriggerEfficiency_Raw_%s.pdf", ax.forHT ? "HT" : "Jet6PT");
      drawRaw(rawPdf, eD, eMA, ax.xt);
      std::cout<<"[OUT] wrote "<<rawPdf<<"\n";
    }

    // (B) SF 적용 플롯
    {
      TString appliedPdf = TString::Format("TriggerEfficiency_SFApplied_%s.pdf", ax.forHT ? "HT" : "Jet6PT");
      drawApplied(appliedPdf, eD, epsPred, ax.xt);
      std::cout<<"[OUT] wrote "<<appliedPdf<<"\n";
    }

    // 정리
    delete hDT; delete hDP; delete hMT; delete hMP; delete hAT; delete hAP;
    delete eD; delete eM; delete eMA; delete sf; delete epsPred;
  }

  fD->Close(); fM->Close(); if(fMA && fMA!=fM) fMA->Close();
  std::cout<<"\n[Done] PlotTriggerEfficiency.\n";
}
