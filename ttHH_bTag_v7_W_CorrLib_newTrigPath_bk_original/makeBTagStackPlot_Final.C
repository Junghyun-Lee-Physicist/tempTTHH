// makeBTagStackPlot.C
//
// 사용법:
//   root -l -q makeBTagStackPlot.C
//
// 이 매크로는 모든 샘플의 "Validation_<SampleName>.root" 파일들을 읽어서
// SF 미적용, SF 적용, SF+Reweight 적용 각각에 대한 stack plot을 생성합니다.

#include <TFile.h>
#include <TH1F.h>
#include <THStack.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TString.h>
#include <TMath.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TPaveText.h>
#include <TSystem.h>
#include <iostream>
#include <vector>
#include <map>

// 샘플 정보 구조체
struct SampleInfo {
    TString name;
    int color;
    TString displayName;
};

// 샘플 목록과 색상 정의
std::vector<SampleInfo> getSampleList() {
    std::vector<SampleInfo> samples;
    
    // 색상과 표시 이름 설정
    // QCD HT별로 쪼개기
    samples.push_back({"QCD_HT200to300", kYellow-9, "QCD HT200-300"});
    samples.push_back({"QCD_HT300to500", kYellow-7, "QCD HT300-500"});
    samples.push_back({"QCD_HT500to700", kYellow-5, "QCD HT500-700"});
    samples.push_back({"QCD_HT700to1000", kYellow-3, "QCD HT700-1000"});
    samples.push_back({"QCD_HT1000to1500", kYellow, "QCD HT1000-1500"});
    samples.push_back({"QCD_HT1500to2000", kOrange-9, "QCD HT1500-2000"});
    samples.push_back({"QCD_HT2000toInf", kOrange-7, "QCD HT2000-Inf"});

//    samples.push_back({"QCD_Pt_15to30",     kYellow-9, "QCD Pt15-30"});
//    samples.push_back({"QCD_Pt_30to50",     kYellow-8, "QCD Pt30-50"});
//    samples.push_back({"QCD_Pt_50to80",     kYellow-7, "QCD Pt50-80"});
//    samples.push_back({"QCD_Pt_80to120",    kYellow-6, "QCD Pt80-120"});
//    samples.push_back({"QCD_Pt_120to170",   kYellow-5, "QCD Pt120-170"});
//    samples.push_back({"QCD_Pt_170to300",   kYellow-4, "QCD Pt170-300"});
//    samples.push_back({"QCD_Pt_300to470",   kYellow-3, "QCD Pt300-470"});
//    samples.push_back({"QCD_Pt_470to600",   kYellow-2, "QCD Pt470-600"});
//    samples.push_back({"QCD_Pt_600to800",   kYellow-1, "QCD Pt600-800"});
//    samples.push_back({"QCD_Pt_800to1000",  kYellow,   "QCD Pt800-1000"});
//    samples.push_back({"QCD_Pt_1000to1400", kOrange-9, "QCD Pt1000-1400"});
//    samples.push_back({"QCD_Pt_1400to1800", kOrange-7, "QCD Pt1400-1800"});
//    samples.push_back({"QCD_Pt_1800to2400", kOrange-5, "QCD Pt1800-2400"});
//    samples.push_back({"QCD_Pt_2400to3200", kOrange-2, "QCD Pt2400-3200"});
//    samples.push_back({"QCD_Pt_3200toInf",  kOrange,   "QCD Pt3200-Inf"});

    samples.push_back({"ttJets", kRed-7, "t#bar{t}+jets"});
    samples.push_back({"ttbb", kRed-9, "t#bar{t}+b#bar{b}"});
    samples.push_back({"tt4b", kRed-10, "t#bar{t}+4b"});
    samples.push_back({"ttHH", kBlue-9, "t#bar{t}HH"});
    samples.push_back({"ttHtobb", kBlue-7, "t#bar{t}H(b#bar{b})"});
    samples.push_back({"tttt", kGreen-7, "t#bar{t}t#bar{t}"});
    samples.push_back({"tttW", kGreen-5, "t#bar{t}tW"});
    samples.push_back({"ttWH", kCyan-3, "t#bar{t}WH"});
    samples.push_back({"ttWW", kCyan-5, "t#bar{t}WW"});
    samples.push_back({"ttWZ", kCyan-7, "t#bar{t}WZ"});
    samples.push_back({"ttZHto4b", kViolet-5, "t#bar{t}ZH(4b)"});
    samples.push_back({"ttZtobb", kViolet-7, "t#bar{t}Z(b#bar{b})"});
    samples.push_back({"ttZZto4b", kViolet-9, "t#bar{t}ZZ(4b)"});
    
    return samples;
}

void DrawCMSLabel()
{
    double x = 0.10, y = 0.925, fontSize = 0.060;
    std::string extraText = "Private work";

    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(fontSize);
    latex.SetTextAlign(11);
    TString text = Form("CMS#scale[0.5]{ }#scale[0.85]{#font[52]{%s}}", extraText.c_str());
    latex.DrawLatex(x, y, text);
}

void DrawCurrentSample()
{
    double x = 0.623, y = 0.925, fontSize = 0.050;
    std::string extraText = "2017 UL NanoAODv9";

    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(fontSize);
    latex.SetTextAlign(11);
    TString text = Form("#bf{%s}", extraText.c_str());
    latex.DrawLatex(x, y, text);
}

void DrawText(TString sfType)
{
    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.04); // 글자 크기 줄임
    latex.SetTextAlign(11);
    // 한 줄로 표시
    TString text = Form("#it{ttHH} hadronic channel, %s", sfType.Data());
    latex.DrawLatex(0.15, 0.80, text);
}

// Stack plot 생성 함수
void makeStackPlot_Final(TString varName, TString histNameBase, TString xTitle, 
                   double xMin, double xMax, TString outDir = "plots", TString jetIndex = "")
{
    // 샘플 목록 가져오기
    std::vector<SampleInfo> samples = getSampleList();
    
    // 경로 설정
    TString path = "ScaleFactors/Final";

    
    // Data 파일 열기
    TFile* fData = TFile::Open(path + "/Data/Validation_Data.root");
    if (!fData || fData->IsZombie()) {
        std::cerr << "Cannot open Data file!" << std::endl;
        return;
    }
    
    // SF 타입별로 stack 생성
    std::vector<TString> sfTypes = {"reweight"};
    std::vector<TString> sfLabels = {"SF+Reweight applied"};
    
    for (int iType = 0; iType < sfTypes.size(); ++iType) {
        TString sfType = sfTypes[iType];
        TString sfLabel = sfLabels[iType];
        
        // THStack 생성
        THStack* stack = new THStack("stack", "");
        
        // 범례 생성 - 위치와 크기 조정
        TLegend* leg = new TLegend(0.60, 0.35, 0.88, 0.88);
        leg->SetBorderSize(0);
        leg->SetTextFont(42);
        leg->SetTextSize(0.030); // 글자 크기 줄임
        leg->SetFillStyle(0);
        
        // Data 히스토그램 가져오기 (reweight 사용)
        TString dataHistName;
        if (jetIndex != "") {
            dataHistName = histNameBase + "_reweight_" + jetIndex;
        } else {
            dataHistName = histNameBase + "_reweight";
        }
        TH1F* hData = (TH1F*) fData->Get(dataHistName);
        if (!hData) {
            std::cerr << "Cannot find data histogram: " << dataHistName << std::endl;
            continue;
        }
        hData->SetMarkerStyle(20);
        hData->SetMarkerSize(1.2);
        hData->SetLineColor(kBlack);
        hData->GetXaxis()->SetRangeUser(xMin, xMax);
        
        // 각 샘플의 히스토그램 가져오기
        std::vector<TH1F*> histograms;
        std::vector<double> integrals; // 이벤트 개수 저장
        double totalMC = 0;
        
        for (auto& sample : samples) {
            TString fileName = path + "/MC/Validation_" + sample.name + ".root";
            TFile* fSample = TFile::Open(fileName);
            
            if (!fSample || fSample->IsZombie()) {
                std::cerr << "Cannot open file: " << fileName << std::endl;
                continue;
            }
            
            TString histName;
            if (jetIndex != "") {
                histName = histNameBase + "_" + sfType + "_" + jetIndex;
            } else {
                histName = histNameBase + "_" + sfType;
            }
            TH1F* hist = (TH1F*) fSample->Get(histName);
            
            if (!hist) {
                std::cerr << "Cannot find histogram: " << histName << " in " << fileName << std::endl;
                fSample->Close();
                continue;
            }
            
            // 히스토그램 복사 (파일이 닫혀도 유지되도록)
            hist = (TH1F*) hist->Clone();
            hist->SetDirectory(0);
            
            // 색상 설정
            hist->SetFillColor(sample.color);
            hist->SetLineColor(kBlack);
            hist->SetLineWidth(1);
            
            // Stack에 추가
            stack->Add(hist);
            histograms.push_back(hist);
            
            // 이벤트 수 저장
            double integral = hist->Integral();
            integrals.push_back(integral);
            totalMC += integral;
            
            fSample->Close();
        }
        
        // 범례에 추가 (역순으로 - stack 순서와 맞추기 위해)
        // Data 이벤트 수 계산
        double dataIntegral = hData->Integral();
        leg->AddEntry(hData, Form("Data (%.0f)", dataIntegral), "lep");
        
        for (int i = samples.size()-1; i >= 0; --i) {
            if (i < histograms.size() && i < integrals.size()) {
                // 이벤트 수가 1000 이상이면 과학적 표기법, 아니면 정수로 표시
                TString format = integrals[i] >= 1000 ? "%.1e" : "%.0f";
                leg->AddEntry(histograms[i], 
                            Form(("%s (" + format + ")").Data(), samples[i].displayName.Data(), integrals[i]), 
                            "f");
            }
        }
        
        // Canvas 생성
        TCanvas* c = new TCanvas("c", "", 800, 800);
        c->SetLeftMargin(0.125);
        c->Divide(1, 2);
        
        // 상단 패드: Stack plot
        c->cd(1);
        gPad->SetPad(0.0, 0.35, 1.0, 1.0);
        gPad->SetLogy(1);
        gPad->SetBottomMargin(0.02); // 약간의 마진
        gPad->SetTopMargin(0.08);
        gPad->SetRightMargin(0.05);
        gPad->SetLeftMargin(0.125);
        gPad->SetGridx();
        gPad->Draw();
        
        // Stack 그리기
        stack->Draw("HIST");
        stack->GetXaxis()->SetRangeUser(xMin, xMax);
        stack->GetYaxis()->SetTitle("# of Events (Log-scale)");
        stack->GetYaxis()->SetTitleSize(0.055);
        stack->GetYaxis()->SetTitleOffset(0.80);
        stack->GetYaxis()->SetLabelSize(0.04);
        stack->GetXaxis()->SetTitleSize(0);
        stack->GetXaxis()->SetLabelSize(0);
        
        // 최대값 설정
        double yMax = std::max(stack->GetMaximum(), hData->GetMaximum());
        stack->SetMaximum(yMax * 10.0);
        stack->SetMinimum(0.1);
        
        // Data 그리기
        hData->Draw("E SAME");
        
        // 범례와 라벨 그리기
        leg->Draw();
        DrawText(sfLabel);
        DrawCMSLabel();
        DrawCurrentSample();
        
        // 하단 패드: Data/MC ratio
        c->cd(2);
        gPad->SetPad(0.0, 0.0, 1.0, 0.35);
        gPad->SetBottomMargin(0.35);
        gPad->SetTopMargin(0.02);
        gPad->SetRightMargin(0.05);
        gPad->SetLeftMargin(0.125);
        gPad->SetGridx();
        gPad->SetGridy();
        gPad->Draw();
        
        // MC 총합 히스토그램 만들기
        TH1F* hMCTotal = nullptr;
        for (auto* hist : histograms) {
            if (!hMCTotal) {
                hMCTotal = (TH1F*) hist->Clone("hMCTotal");
                hMCTotal->SetDirectory(0);
            } else {
                hMCTotal->Add(hist);
            }
        }
        
        // Ratio 계산
        if (hMCTotal && hMCTotal->Integral() > 0) {
            TH1F* hRatio = (TH1F*) hData->Clone("hRatio");
            hRatio->SetDirectory(0);
            hRatio->Divide(hMCTotal);
            
            // Ratio plot 스타일 설정
            hRatio->SetMarkerStyle(20);
            hRatio->SetMarkerSize(1.2);
            hRatio->SetLineColor(kBlack);
            hRatio->GetXaxis()->SetTitle(xTitle);
            hRatio->GetXaxis()->SetTitleOffset(1.1);
            hRatio->GetXaxis()->SetTitleSize(0.12);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetLabelOffset(0.03);
            hRatio->GetXaxis()->SetRangeUser(xMin, xMax);
            hRatio->GetYaxis()->SetTitle("Data/MC");
            hRatio->GetYaxis()->SetTitleOffset(0.52);
            hRatio->GetYaxis()->SetTitleSize(0.09);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetLabelOffset(0.01);
            hRatio->GetYaxis()->SetNdivisions(505);
            hRatio->SetMinimum(0.5);
//            hRatio->SetMaximum(1.5);
            hRatio->SetMaximum(5.0);
            
            hRatio->Draw("E");
            
            // Unity line
            TLine* line = new TLine(xMin, 1.0, xMax, 1.0);
            line->SetLineStyle(2);
            line->SetLineColor(kRed);
            line->Draw();
            
       //     delete hRatio;
        }
        
        // PDF 저장
        TString outFileName = Form("%s/BTagStackPlot_%s_%s.pdf", 
                                  outDir.Data(), varName.Data(), sfType.Data());
        c->SaveAs(outFileName);
        std::cout << "Saved: " << outFileName << std::endl;
        
        // 메모리 정리
        delete c;
        delete stack;
        delete leg;
        delete hMCTotal;
        for (auto* hist : histograms) {
            delete hist;
        }
    }
    
    fData->Close();
}

// 메인 함수
void makeBTagStackPlot_Final()
{
    // 스타일 설정
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    
    // 출력 디렉토리 생성
    TString outDir = "plots";
    if (gSystem->AccessPathName(outDir.Data())) {
        gSystem->mkdir(outDir, kTRUE);
    }
    
    // 각 변수에 대해 stack plot 생성
    
    // nJets
    makeStackPlot_Final("nJets", "h_nJets", "Number of Jets", 6, 15, outDir);
    
    // HT
    makeStackPlot_Final("HT", "h_HT", "H_{T} [GeV]", 500, 2000, outDir);
    
    // jet0 pT
    makeStackPlot_Final("jet0_pT", "h_jetPt", "Leading Jet p_{T} [GeV]", 50, 400, outDir, "jet0");
    
    // jet0 bTag
    makeStackPlot_Final("jet0_bTag", "h_bTag", "Leading Jet b-tag Discriminant", 0.0, 1.0, outDir, "jet0");
 
    // jet0 eta
    makeStackPlot_Final("jet0_eta", "h_jetEta", "Leading Jet #eta", -2.5, 2.5, outDir, "jet0");

    // jet1 pT
    makeStackPlot_Final("jet1_pT", "h_jetPt", "Subleading Jet p_{T} [GeV]", 50, 400, outDir, "jet1");
    
    // jet1 bTag
    makeStackPlot_Final("jet1_bTag", "h_bTag", "Subleading Jet b-tag Discriminant", 0.0, 1.0, outDir, "jet1");
    
    // jet1 eta
    makeStackPlot_Final("jet1_eta", "h_jetEta", "Subleading Jet #eta", -2.5, 2.5, outDir, "jet1");

    
    std::cout << "\nAll stack plots created successfully!" << std::endl;
}
