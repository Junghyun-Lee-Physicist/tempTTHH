#include <TFile.h>
#include <TH1D.h>
#include <TEfficiency.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TRatioPlot.h>
#include <vector>
#include <iostream>
#include "include/BinConfig.hh"

void PlotTriggerEfficiency() {
    // Config 로드 (타이틀이나 축 범위 등에 활용 가능)
    BinConfig::Load("config.yaml");

    // 파일 열기
    TFile* dataFile = TFile::Open("corrected_Data.root"); 
    TFile* mcFile   = TFile::Open("corrected_ttJets.root"); 

    if(!dataFile || !mcFile) {
        std::cerr << "Error: Corrected output files not found!" << std::endl;
        std::cerr << "Did you run ./exe_TrigStudy [Sample] 1 ?" << std::endl;
        return;
    }

    // 그릴 변수 목록 (EventLooper에서 저장한 이름과 일치해야 함)
    std::vector<std::string> vars = {"HT", "pT", "Eta", "nbJets"};
    
    for(const auto& var : vars) {
        // 히스토그램 가져오기
        TH1D* h_data_pass  = (TH1D*)dataFile->Get(("h_"+var+"_Pass").c_str());
        TH1D* h_data_total = (TH1D*)dataFile->Get(("h_"+var+"_Total").c_str());
        TH1D* h_mc_pass    = (TH1D*)mcFile->Get(("h_"+var+"_Pass").c_str());
        TH1D* h_mc_total   = (TH1D*)mcFile->Get(("h_"+var+"_Total").c_str());

        // 히스토그램 존재 여부 확인 (Config에 따라 Eta/nbJets는 없을 수도 있음)
        if(!h_data_pass || !h_mc_pass) {
            std::cout << "[Info] Missing hists for var: " << var << " (Maybe disabled in config?)" << std::endl;
            continue;
        }

        // 1. Efficiency 계산 (RatioPlot용으로 TH1D 사용)
        // Clone을 떠서 원본 보존
        TH1D* eff_Data = (TH1D*)h_data_pass->Clone(("eff_Data_"+var).c_str());
        eff_Data->Divide(h_data_pass, h_data_total, 1, 1, "B"); // "B" for Binomial errors
        
        TH1D* eff_MC = (TH1D*)h_mc_pass->Clone(("eff_MC_"+var).c_str());
        eff_MC->Divide(h_mc_pass, h_mc_total, 1, 1, "B");

        // 스타일링
        eff_Data->SetMarkerStyle(20);
        eff_Data->SetMarkerColor(kBlack);
        eff_Data->SetLineColor(kBlack);
        eff_Data->SetTitle(var.c_str());
        eff_Data->SetStats(0);

        eff_MC->SetMarkerStyle(21);
        eff_MC->SetMarkerColor(kRed);
        eff_MC->SetLineColor(kRed);
        eff_MC->SetStats(0);

        // 2. 캔버스 생성 및 그리기
        TCanvas* c = new TCanvas(("c_Val_"+var).c_str(), var.c_str(), 800, 800);
        
        // TRatioPlot (Top: Efficiencies, Bottom: Ratio)
        TRatioPlot* rp = new TRatioPlot(eff_MC, eff_Data); // MC / Data ? Or Data / MC?
        // 보통 Data/MC 비교에서는 Data 점을 MC 선/점 위에 그립니다.
        // TRatioPlot(h1, h2) -> h1/h2 ratio를 그립니다.
        // 우리는 Data/MC Ratio를 보고 싶다면 TRatioPlot(eff_Data, eff_MC)가 맞습니다.
        // 하지만 여기선 Corrected MC가 Data와 얼마나 잘 맞는지를 보므로,
        // 위쪽 패드에 둘 다 그리고, 아래쪽 패드에 Ratio를 그립니다.
        
        rp->SetH1DrawOpt("PE");
        rp->SetH2DrawOpt("PE");
        rp->Draw();

        // 축 및 타이틀 설정
        rp->GetLowerRefGraph()->SetMinimum(0.5);
        rp->GetLowerRefGraph()->SetMaximum(1.5);
        rp->GetLowerRefYaxis()->SetTitle("MC / Data"); // 순서 주의 (생성자 인자 순서)
        rp->GetUpperRefYaxis()->SetTitle("Trigger Efficiency");
        
        // Legend 추가
        rp->GetUpperPad()->cd();
        TLegend* leg = new TLegend(0.6, 0.2, 0.85, 0.4);
        leg->AddEntry(eff_Data, "Data", "lp");
        leg->AddEntry(eff_MC, "MC (SF Applied)", "lp");
        leg->Draw();

        // 저장
        c->SaveAs(("Validation_Eff_" + var + ".pdf").c_str());
        
        delete c;
    }
    std::cout << ">>> All validation plots created." << std::endl;
}
