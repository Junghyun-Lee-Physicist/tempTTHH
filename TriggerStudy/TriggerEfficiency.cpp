#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TGraphAsymmErrors.h>
#include <TEfficiency.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <iostream>
#include <vector>
#include <string>

//#include "/Users/jhlee/Desktop/Work/ttHH/TriggerStudyv2/include/BinConfig.hh"
//#include "/afs/cern.ch/user/j/junghyun/ttHH_analysis/CMSSW_14_2_1/src/TrigNbTag/TriggerStudy/include/BinConfig.hh"
#include "/Users/jhlee/ttHH/TriggerStudy/include/BinConfig.hh"

void TriggerEfficiency()
{

    // Eta 분할을 사용할지 여부 설정 (EventLooper와 일치시켜야 함)
    bool useEtaBinning = false; // EventLooper에서 설정한 값과 동일하게 설정

    // Data and MC files
    TString dataFileName = "Data.root";
    TString mcFileName = "ttJets.root";

    // Open data file
    TFile* dataFile = TFile::Open(dataFileName);
    if (!dataFile || dataFile->IsZombie())
    {
        std::cerr << "Cannot open data file: " << dataFileName << std::endl;
        return;
    }

    // Open MC file
    TFile* mcFile = TFile::Open(mcFileName);
    if (!mcFile || mcFile->IsZombie())
    {
        std::cerr << "Cannot open MC file: " << mcFileName << std::endl;
        return;
    }

    // Define binning for HT and Jet6PT
    const auto& HT_bins_vec = BinConfig::getHTBins();
    Int_t nBinsHT = BinConfig::HTBinCount;
    Double_t HT_bins[nBinsHT];
    for (int i = 0; i <= nBinsHT; ++i) {
        HT_bins[i] = HT_bins_vec[i];
    }

    // pT bins
    const auto& pT_bins_vec = BinConfig::getPTBins();
    Int_t nBinspT = BinConfig::PTBinCount;
    Double_t pT_bins[nBinspT];
    for (int i = 0; i <= nBinspT; ++i) {
        pT_bins[i] = pT_bins_vec[i];
    }

    // b-제트 수 bins (범위로 설정)
    // 예: 3, 4, 5~8
    const auto& nBjets_bins_vec = BinConfig::getNBJetsBins();
    Int_t nBjetBins = BinConfig::NBJetsBinCount;
    Double_t nBjets_bins[nBjetBins];
    for (int i = 0; i <= nBjetBins; ++i) {
        nBjets_bins[i] = nBjets_bins_vec[i];
    }

    // Eta bins
    Int_t nEtaBins = BinConfig::EtaBinCount;
    Double_t eta_bins[nEtaBins];
    if (useEtaBinning) {
        const auto& eta_bins_vec = BinConfig::getEtaBins();
        for (int i = 0; i <= nEtaBins; ++i) {
            eta_bins[i] = eta_bins_vec[i];
        }
    } else {
        nEtaBins = 1; // Eta 분할을 사용하지 않을 경우 기본값 설정
    }

    // Create output ROOT file
    TFile* outputFile = new TFile("ScaleFactors.root", "RECREATE");

    // 히스토그램을 저장할 디렉토리 생성 (옵션)
    outputFile->mkdir("ScaleFactors");
    outputFile->cd("ScaleFactors");

    // Eta 분할 여부에 따라 루프 설정
    int etaLoopStart = 0;
    int etaLoopEnd = useEtaBinning ? nEtaBins : 1; // Eta 분할을 사용하지 않을 경우 루프를 한 번만 돔

    // Loop over eta bins
    for (int iEta = etaLoopStart; iEta < etaLoopEnd; ++iEta)
    {
        // Loop over b-jet bins
        for (int iBjet = 0; iBjet < 1; ++iBjet)
        {
            // Construct histogram names
            TString histName_Total, histName_Pass;
            if (useEtaBinning)
            {
                histName_Total = Form("h_Total_Eta%d_Bjet%d", iEta, iBjet);
                histName_Pass = Form("h_Pass_Eta%d_Bjet%d", iEta, iBjet);
            }
            else
            {
                histName_Total = Form("h_Total_Bjet%d", iBjet);
                histName_Pass = Form("h_Pass_Bjet%d", iBjet);
            }

            // Retrieve histograms from Data file
            TH2D* data_Total = (TH2D*)dataFile->Get(histName_Total);
            TH2D* data_Pass = (TH2D*)dataFile->Get(histName_Pass);

            // Retrieve histograms from MC file
            TH2D* mc_Total = (TH2D*)mcFile->Get(histName_Total);
            TH2D* mc_Pass = (TH2D*)mcFile->Get(histName_Pass);


	    bool debugEntries = true;
	    if(debugEntries){
                int dataTotal = data_Total->GetEntries();
                int dataPass = data_Pass->GetEntries();
                int mcTotal = mc_Total->GetEntries();
                int mcPass = mc_Pass->GetEntries();
                std::cout << "\n[ Debug ]  -----> eta [" << iEta <<"] Bjet ["<< iBjet <<"]"<<std::endl;
                std::cout << "[ Debug ] : Current data total entries : "<<dataTotal<<std::endl;
                std::cout << "[ Debug ] : Current data pass entries : "<<dataPass<<std::endl;
                std::cout << "[ Debug ] : Current mc total entries : "<<mcTotal<<std::endl;
                std::cout << "[ Debug ] : Current mc pass entries : "<<mcPass<<std::endl;
                std::cout<<""<<std::endl;
	    }

            if (!data_Total || !data_Pass)
            {
                std::cerr << "Cannot find data histograms: " << histName_Total << " or " << histName_Pass << std::endl;
                continue;
            }

            if (!mc_Total || !mc_Pass)
            {
                std::cerr << "Cannot find MC histograms: " << histName_Total << " or " << histName_Pass << std::endl;
                continue;
            }


            // Calculate efficiencies
            TString effDataName, effMCName, sfName;
            if (useEtaBinning)
            {
                effDataName = Form("effData_Eta%d_Bjet%d", iEta, iBjet);
                effMCName = Form("effMC_Eta%d_Bjet%d", iEta, iBjet);
                //effDataName = Form("h_Pass_Eta%d_Bjet%d", iEta, iBjet);
                //effMCName = Form("h_Pass_Eta%d_Bjet%d", iEta, iBjet);
		sfName = Form("SF_Eta%d_Bjet%d", iEta, iBjet);
            }
            else
            {
                effDataName = Form("effData_Bjet%d", iBjet);
                effMCName = Form("effMC_Bjet%d", iBjet);
		sfName = Form("SF_Bjet%d", iBjet);
            }

            TH2D* effData = (TH2D*)data_Pass->Clone(effDataName);
            effData->Divide(data_Total);

            TH2D* effMC = (TH2D*)mc_Pass->Clone(effMCName);
            effMC->Divide(mc_Total);

            // Calculate Scale Factors
            TH2D* sfHist = (TH2D*)effData->Clone(sfName);
            sfHist->Divide(effMC);

            //// Set underflow and overflow bins to appropriate values (optional)
            //for (int ix = 0; ix <= sfHist->GetNbinsX()+1; ++ix)
            //{
            //    for (int iy = 0; iy <= sfHist->GetNbinsY()+1; ++iy)
            //    {
            //        if (ix == 0 || iy == 0 || ix == sfHist->GetNbinsX()+1 || iy == sfHist->GetNbinsY()+1)
            //        {
            //            sfHist->SetBinContent(ix, iy, 1.0);
            //            sfHist->SetBinError(ix, iy, 0.0);
            //        }
            //    }
            //}

            // Save the SF histogram to the output file
            outputFile->cd("ScaleFactors");
            sfHist->Write();

            // 이제 플롯을 생성할 때 각 bin을 일정한 간격으로 표시하고, x축에 범위를 표시하며, bin content 값을 중앙에 표시합니다.

            // HT와 Jet6PT의 binning 정보를 문자열로 저장
            std::vector<std::string> xBinLabels;
            std::vector<std::string> yBinLabels;
            for (int i = 0; i < nBinsHT; ++i)
            {
                xBinLabels.push_back(Form("%.0f-%.0f", HT_bins[i], HT_bins[i+1]));
            }
            for (int i = 0; i < nBinspT; ++i)
            {
                yBinLabels.push_back(Form("%.0f-%.0f", pT_bins[i], pT_bins[i+1]));
            }

            // 일정한 binning을 가진 히스토그램 생성
            TH2D* sfHistUniform = new TH2D(Form("SFUniform_Eta%d_Bjet%d", iEta, iBjet),
                                           sfHist->GetTitle(),
                                           nBinsHT, 0, nBinsHT,
                                           nBinspT, 0, nBinspT);

            // 원본 히스토그램의 내용을 새로운 히스토그램에 복사
            for (int ix = 1; ix <= nBinsHT; ++ix)
            {
                for (int iy = 1; iy <= nBinspT; ++iy)
                {
                    Double_t content = sfHist->GetBinContent(ix, iy);
                    Double_t error = sfHist->GetBinError(ix, iy);
                    sfHistUniform->SetBinContent(ix, iy, content);
                    sfHistUniform->SetBinError(ix, iy, error);
                }
            }

            // x축과 y축의 bin label 설정
            for (int ix = 1; ix <= nBinsHT; ++ix)
            {
                sfHistUniform->GetXaxis()->SetBinLabel(ix, xBinLabels[ix-1].c_str());
            }
            for (int iy = 1; iy <= nBinspT; ++iy)
            {
                sfHistUniform->GetYaxis()->SetBinLabel(iy, yBinLabels[iy-1].c_str());
            }

            // 히스토그램 그리기
            TCanvas* c = new TCanvas("c", "Scale Factor", 1200, 800); // 캔버스 크기 조정
            sfHistUniform->SetStats(0); // 통계 상자 제거
            sfHistUniform->GetZaxis()->SetRangeUser(0.5, 1.2); // Z축 범위 설정
            sfHistUniform->GetXaxis()->SetTitle("HT [GeV]");
            sfHistUniform->GetYaxis()->SetTitle("6th Jet p_{T} [GeV]");
            sfHistUniform->GetXaxis()->SetLabelSize(0.035);
            sfHistUniform->GetYaxis()->SetLabelSize(0.035);
            // sfHistUniform->GetXaxis()->LabelsOption("v"); // x축 라벨 수직으로 표시 (주석 처리 또는 제거)
            sfHistUniform->Draw("COLZ");

            // bin content 값을 중앙에 표시
            sfHistUniform->Draw("TEXT SAME");

            // 캔버스 여백 조정
            c->SetBottomMargin(0.15);
            c->SetLeftMargin(0.15);
            c->SetRightMargin(0.15);

            // 제목 설정
            sfHistUniform->SetTitle(Form("Scale Factor%s (b-jets %d)",
                useEtaBinning ? Form(" (Eta %.1f to %.1f)", eta_bins[iEta], eta_bins[iEta+1]) : "",
                (int)nBjets_bins[iBjet]));
//sfHistUniform->SetTitle("TriggerSF for calculating b-tag re-wgt");

            // x축 라벨을 가로로 표시하기 위해 LabelsOption 설정 제거 또는 "h"로 설정
            sfHistUniform->GetXaxis()->LabelsOption("h"); // 또는 이 라인을 제거
  
            sfHistUniform->Write();

            // Save the canvas as an image
            TString canvasName;
            if (useEtaBinning)
                canvasName = Form("ScaleFactor_Eta%d_Bjet%d.pdf", iEta, iBjet);
            else
                canvasName = Form("ScaleFactor_Bjet%d.pdf", iBjet);
            c->SaveAs(canvasName);

            // Clean up
            delete c;
            delete effData;
            delete effMC;
            delete sfHist;
            delete sfHistUniform;
        }
    }

    // Close files
    dataFile->Close();
    mcFile->Close();
    outputFile->Close();
}

