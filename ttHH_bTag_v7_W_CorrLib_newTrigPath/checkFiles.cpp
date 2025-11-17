void checkFiles() {
    TString filePath = "/Users/jhlee/Desktop/Work/ttHH/ttHH_bTag_v5_W_CorrLib/ScaleFactors/ReweightApplied/";
    
    // 체크할 샘플들
    std::vector<TString> samples = {
        "QCD_HT200to300", "QCD_HT300to500", "QCD_HT500to700", 
        "QCD_HT700to1000", "QCD_HT1000to1500", "QCD_HT1500to2000", 
        "QCD_HT2000toInf", "ttJets", "tt4b", "ttbb", "ttHH", 
        "ttHtobb", "tttt", "tttW", "ttWH", "ttWW", "ttWZ", 
        "ttZHto4b", "ttZtobb", "ttZZto4b",
        "JetHT_B", "JetHT_C", "JetHT_D", "JetHT_E", "JetHT_F"
    };
    
    std::cout << "Checking ROOT files in: " << filePath << std::endl;
    std::cout << "=======================================" << std::endl;
    
    for (auto& sample : samples) {
        TString fileName = filePath + "Reweight_" + sample + ".root";
        TFile* file = TFile::Open(fileName);
        
        if (!file || file->IsZombie()) {
            std::cout << "ERROR: Cannot open " << fileName << std::endl;
        } else {
            std::cout << "OK: " << sample << std::endl;
            
            // Check for histograms
            TH1D* h_nJets = (TH1D*)file->Get("h_nJets_noSF");
            TH1D* h_HT = (TH1D*)file->Get("h_HT_noSF");
            TH1D* h_jet0 = (TH1D*)file->Get("h_jetPt_noSF_jet0");
            
            if (!h_nJets) std::cout << "  - Missing h_nJets_noSF" << std::endl;
            if (!h_HT) std::cout << "  - Missing h_HT_noSF" << std::endl;
            if (!h_jet0) std::cout << "  - Missing h_jetPt_noSF_jet0" << std::endl;
            
            file->Close();
        }
    }
}
