#define EventLooper_cxx
#include "EventLooper.hh"
#include "Config.hh"

#include <TH2.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>

// ============================================================================
// Helper: Fatal invariant violation
// ============================================================================
static inline void FATAL_INVARIANT(const char* msg, Long64_t entry) {
    std::cerr << "\n[ERROR][InvariantViolation] entry=" << entry
              << " : " << msg << std::endl;
    std::cerr << "  -> This event should not exist after skimming. Exiting.\n" << std::endl;
    std::exit(1);
}

// ============================================================================
// Constructor & Destructor
// ============================================================================
EventLooper::EventLooper(bool applySF)
    : applySFMode(applySF)
{
}

EventLooper::~EventLooper()
{
    if (reader) {
        delete reader;
        reader = nullptr;
    }

    if (inputFile) {
        inputFile->Close();
        delete inputFile;
        inputFile = nullptr;
    }
}

// ============================================================================
// Init Method
// ============================================================================
void EventLooper::Init()
{
    TString ntuplePath = Config::inputBaseDir + getInputName();

    inputFile = TFile::Open(ntuplePath);
    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "[EventLooper][ERROR] Cannot open " << ntuplePath << std::endl;
        std::exit(1);
    }

    inputFile->GetObject(Config::treePath.c_str(), fChain);
    if (!fChain) {
        std::cerr << "[EventLooper][ERROR] Cannot get TTree: " 
                  << Config::treePath << std::endl;
        std::exit(1);
    }

    reader = new NtupleReader(fChain);
}

// ============================================================================
// Loop Method
// ============================================================================
void EventLooper::Loop()
{
    if (fChain == nullptr) return;

    // ========================================================================
    // [Phase 1] Configuration & Initialization
    // ========================================================================
    TH1::SetDefaultSumw2(true);
    Config::Dump();

    const std::vector<double>& HT_Bins = Config::HT_Bins();
    const std::vector<double>& PT_Bins = Config::PT_Bins();

    std::vector<double> Eta_Bins_Val;
    if (Config::useEta) {
        Eta_Bins_Val = Config::Eta_Bins();
    }

    // ========================================================================
    // [Phase 2] Output File & Histogram Creation
    // ========================================================================
    TFile* outputFile = new TFile(getOutputName(), "RECREATE");
    outputFile->cd();

    // Initialize all histogram pointers to nullptr
    h_HT_Total = nullptr;
    h_HT_Pass = nullptr;
    h_pT_Total = nullptr;
    h_pT_Pass = nullptr;
    h_Eta_Total = nullptr;
    h_Eta_Pass = nullptr;
    h_nbJets_Total = nullptr;
    h_nbJets_Pass = nullptr;

    // Step 2 specific histograms (SF applied)
    h_HT_Total_SF = nullptr;
    h_HT_Pass_SF = nullptr;
    h_pT_Total_SF = nullptr;
    h_pT_Pass_SF = nullptr;
    h_Eta_Total_SF = nullptr;
    h_Eta_Pass_SF = nullptr;
    h_nbJets_Total_SF = nullptr;
    h_nbJets_Pass_SF = nullptr;

    // ------------------------------------------------------------------------
    // Create histograms based on mode
    // ------------------------------------------------------------------------
    if (applySFMode) {
        // Step 2: Create both before/after SF histograms for comparison
        // Before SF (raw MC weight)
        h_HT_Total = new TH1D("h_HT_Total_noSF", "HT Total (no SF);HT [GeV];Events",
                              HT_Bins.size() - 1, HT_Bins.data());
        h_HT_Pass  = new TH1D("h_HT_Pass_noSF", "HT Pass (no SF);HT [GeV];Events",
                              HT_Bins.size() - 1, HT_Bins.data());
        h_pT_Total = new TH1D("h_pT_Total_noSF", "6th Jet pT Total (no SF);p_{T} [GeV];Events",
                              PT_Bins.size() - 1, PT_Bins.data());
        h_pT_Pass  = new TH1D("h_pT_Pass_noSF", "6th Jet pT Pass (no SF);p_{T} [GeV];Events",
                              PT_Bins.size() - 1, PT_Bins.data());

        // After SF (with trigger SF applied)
        h_HT_Total_SF = new TH1D("h_HT_Total_SF", "HT Total (with SF);HT [GeV];Events",
                                  HT_Bins.size() - 1, HT_Bins.data());
        h_HT_Pass_SF  = new TH1D("h_HT_Pass_SF", "HT Pass (with SF);HT [GeV];Events",
                                  HT_Bins.size() - 1, HT_Bins.data());
        h_pT_Total_SF = new TH1D("h_pT_Total_SF", "6th Jet pT Total (with SF);p_{T} [GeV];Events",
                                  PT_Bins.size() - 1, PT_Bins.data());
        h_pT_Pass_SF  = new TH1D("h_pT_Pass_SF", "6th Jet pT Pass (with SF);p_{T} [GeV];Events",
                                  PT_Bins.size() - 1, PT_Bins.data());

        // Optional: Eta histograms
        if (Config::useEta && !Eta_Bins_Val.empty()) {
            h_Eta_Total = new TH1D("h_Eta_Total_noSF", "6th Jet #eta Total (no SF);#eta;Events",
                                   Eta_Bins_Val.size() - 1, Eta_Bins_Val.data());
            h_Eta_Pass  = new TH1D("h_Eta_Pass_noSF", "6th Jet #eta Pass (no SF);#eta;Events",
                                   Eta_Bins_Val.size() - 1, Eta_Bins_Val.data());
            h_Eta_Total_SF = new TH1D("h_Eta_Total_SF", "6th Jet #eta Total (with SF);#eta;Events",
                                      Eta_Bins_Val.size() - 1, Eta_Bins_Val.data());
            h_Eta_Pass_SF  = new TH1D("h_Eta_Pass_SF", "6th Jet #eta Pass (with SF);#eta;Events",
                                      Eta_Bins_Val.size() - 1, Eta_Bins_Val.data());
        }

        // Optional: NB-jets histograms
        if (Config::useNBjets && !Config::NB_Bins().empty()) {
            size_t nNBBins = Config::NB_Bins().size();
            h_nbJets_Total = new TH1D("h_nbJets_Total_noSF", "N_{b-jets} Total (no SF);Category;Events",
                                      nNBBins, 0, nNBBins);
            h_nbJets_Pass  = new TH1D("h_nbJets_Pass_noSF", "N_{b-jets} Pass (no SF);Category;Events",
                                      nNBBins, 0, nNBBins);
            h_nbJets_Total_SF = new TH1D("h_nbJets_Total_SF", "N_{b-jets} Total (with SF);Category;Events",
                                         nNBBins, 0, nNBBins);
            h_nbJets_Pass_SF  = new TH1D("h_nbJets_Pass_SF", "N_{b-jets} Pass (with SF);Category;Events",
                                         nNBBins, 0, nNBBins);

            auto labels = Config::NB_Labels();
            for (size_t i = 0; i < labels.size(); ++i) {
                h_nbJets_Total->GetXaxis()->SetBinLabel(i + 1, labels[i].c_str());
                h_nbJets_Pass->GetXaxis()->SetBinLabel(i + 1, labels[i].c_str());
                h_nbJets_Total_SF->GetXaxis()->SetBinLabel(i + 1, labels[i].c_str());
                h_nbJets_Pass_SF->GetXaxis()->SetBinLabel(i + 1, labels[i].c_str());
            }
        }

    } else {
        // Step 1: Only create standard histograms for efficiency map creation
        h_HT_Total = new TH1D("h_HT_Total", "HT Total;HT [GeV];Events",
                              HT_Bins.size() - 1, HT_Bins.data());
        h_HT_Pass  = new TH1D("h_HT_Pass", "HT Pass;HT [GeV];Events",
                              HT_Bins.size() - 1, HT_Bins.data());
        h_pT_Total = new TH1D("h_pT_Total", "6th Jet pT Total;p_{T} [GeV];Events",
                              PT_Bins.size() - 1, PT_Bins.data());
        h_pT_Pass  = new TH1D("h_pT_Pass", "6th Jet pT Pass;p_{T} [GeV];Events",
                              PT_Bins.size() - 1, PT_Bins.data());

        if (Config::useEta && !Eta_Bins_Val.empty()) {
            h_Eta_Total = new TH1D("h_Eta_Total", "6th Jet #eta Total;#eta;Events",
                                   Eta_Bins_Val.size() - 1, Eta_Bins_Val.data());
            h_Eta_Pass  = new TH1D("h_Eta_Pass", "6th Jet #eta Pass;#eta;Events",
                                   Eta_Bins_Val.size() - 1, Eta_Bins_Val.data());
        }

        if (Config::useNBjets && !Config::NB_Bins().empty()) {
            size_t nNBBins = Config::NB_Bins().size();
            h_nbJets_Total = new TH1D("h_nbJets_Total", "N_{b-jets} Total;Category;Events",
                                      nNBBins, 0, nNBBins);
            h_nbJets_Pass  = new TH1D("h_nbJets_Pass", "N_{b-jets} Pass;Category;Events",
                                      nNBBins, 0, nNBBins);

            auto labels = Config::NB_Labels();
            for (size_t i = 0; i < labels.size(); ++i) {
                h_nbJets_Total->GetXaxis()->SetBinLabel(i + 1, labels[i].c_str());
                h_nbJets_Pass->GetXaxis()->SetBinLabel(i + 1, labels[i].c_str());
            }
        }
    }

    // ========================================================================
    // [Phase 3] Mode-Specific Setup
    // ========================================================================
    
    // =====================================================================
    // DEBUG MODE FLAG: Set to TRUE to use ROOT histogram, FALSE for JSON
    // =====================================================================
    bool useDebugSF = false;  // <<< CHANGE THIS TO SWITCH MODES
    
    std::map<std::string, TH2D*> sfHistMap;  // For debug mode
    TFile* sfRootFile = nullptr;
    
    // Debug statistics
    long long nEvtSFApplied = 0;
    long long nEvtSF1 = 0;  // SF=1 (empty bin)
    double sumSF = 0.0;
    
    if (applySFMode) {
        if (useDebugSF) {
            // ----------------------------------------------------------------
            // DEBUG MODE: Load SF from TriggerSF.root histogram
            // ----------------------------------------------------------------
            std::cout << "\n";
            std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
            std::cout << "║  DEBUG MODE: Using ROOT histogram for SF (not JSON)         ║\n";
            std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
            std::cout << "    Loading: " << Config::sfOutputRoot << "\n\n";
            
            sfRootFile = TFile::Open(Config::sfOutputRoot.c_str(), "READ");
            if (!sfRootFile || sfRootFile->IsZombie()) {
                std::cerr << "[ERROR] Cannot open " << Config::sfOutputRoot << "\n";
                std::exit(1);
            }
            
            // Load SF_measured histograms for each category
            auto nbLabels = Config::NB_Labels();
            auto etaLabels = Config::Eta_Labels();
            
            for (const auto& nb : nbLabels) {
                for (const auto& eta : etaLabels) {
                    std::string key = nb + "_" + eta;
                    std::string histName = "Histograms/SF_measured_" + key;
                    
                    TH2D* hSF = dynamic_cast<TH2D*>(sfRootFile->Get(histName.c_str()));
                    if (hSF) {
                        sfHistMap[key] = hSF;
                        
                        // Print histogram info
                        std::cout << "    [LOADED] " << key << "\n";
                        std::cout << "             X-axis (HT): " 
                                  << hSF->GetXaxis()->GetXmin() << " - " 
                                  << hSF->GetXaxis()->GetXmax() 
                                  << " (" << hSF->GetNbinsX() << " bins)\n";
                        std::cout << "             Y-axis (pT): " 
                                  << hSF->GetYaxis()->GetXmin() << " - " 
                                  << hSF->GetYaxis()->GetXmax() 
                                  << " (" << hSF->GetNbinsY() << " bins)\n";
                        
                        // Count non-zero bins
                        int nNonZero = 0;
                        for (int i = 1; i <= hSF->GetNbinsX(); ++i) {
                            for (int j = 1; j <= hSF->GetNbinsY(); ++j) {
                                if (hSF->GetBinContent(i, j) > 0) nNonZero++;
                            }
                        }
                        std::cout << "             Non-zero bins: " << nNonZero 
                                  << " / " << hSF->GetNbinsX() * hSF->GetNbinsY() << "\n\n";
                    } else {
                        std::cout << "    [WARN] Not found: " << histName << "\n";
                    }
                }
            }
            std::cout << "    Total SF histograms loaded: " << sfHistMap.size() << "\n\n";
            
        } else {
            // ----------------------------------------------------------------
            // NORMAL MODE: Load SF from JSON (correctionlib)
            // ----------------------------------------------------------------
            std::cout << ">>> [Mode] Step 2: Applying Scale Factors for Validation\n";
            std::cout << "    Loading: " << Config::sfOutputJSON << "\n";

            try {
                cset = correction::CorrectionSet::from_file(Config::sfOutputJSON);
                sf_provider = cset->at("triggerSF");
            } catch (std::exception& e) {
                std::cerr << "[EventLooper][ERROR] Failed to load SF JSON: " 
                          << e.what() << std::endl;
                std::exit(1);
            }
        }

    } else {
        std::cout << ">>> [Mode] Step 1: Creating Efficiency Maps\n";

        auto nbLabels  = Config::NB_Labels();
        auto etaLabels = Config::Eta_Labels();

        for (const auto& nb : nbLabels) {
            for (const auto& eta : etaLabels) {
                TString key = Form("%s_%s", nb.c_str(), eta.c_str());

                map_Total[key] = new TH2D("Total_" + key, 
                                          "Total: " + key + ";HT [GeV];p_{T} [GeV]",
                                          HT_Bins.size() - 1, HT_Bins.data(),
                                          PT_Bins.size() - 1, PT_Bins.data());

                map_Pass[key] = new TH2D("Pass_" + key,
                                         "Pass: " + key + ";HT [GeV];p_{T} [GeV]",
                                         HT_Bins.size() - 1, HT_Bins.data(),
                                         PT_Bins.size() - 1, PT_Bins.data());
            }
        }

        std::cout << "    Created " << map_Total.size() << " category maps (Total + Pass)\n";
    }

  // ------------------------------------------------------------------------
    // [Phase 4] Determine if the sample is Data or MC, and parse dataset/era if Data
    //
    // Naming convention assumed:
    //   - MC:   TTTo2L2Nu.root, TTToSemiLeptonic.root, TTToHadronic.root
    //   - Data: <DataSet>_<Era>.root   (e.g. SingleMuon_B.root, JetHT_D.root)
    // ------------------------------------------------------------------------
//    TString sampleName = getInputName();
//    sampleName.ReplaceAll(".root", "");   // strip extension safely for this convention
//    std::cout << "Sample Name: " << sampleName << std::endl;
//
//    // Default: treat as Data unless it matches known MC names
//    isData = true;
//
//    TString dataSet = "default";
//    TString era     = "default";
//
//    // --- Explicit MC identification (same policy as your original code) ---
//    double MC_weight = -9999999.9; // (Xsec * Lumi) / Sum(Runs.genEventSumw)
//    if (sampleName == "TTTo2L2Nu") {
//        isData  = false;
//        MC_weight = 0.0004761561474;
//    }
//    else if (sampleName == "TTToHadronic") {
//        isData  = false;
//        MC_weight = 0.000214351205;
//    }
//    else if (sampleName == "TTToSemiLeptonic") {
//        isData  = false;
//        MC_weight = 0.0001455793461;
//    }
//    else {
//        std::cout << "Current Sample: Data\n";
//    }
//
//
//    // --- If Data, parse "<DataSet>_<Era>" ---
//    if (isData) {
//        Ssiz_t underscorePos = sampleName.Index("_");
//        if (underscorePos == kNPOS) {
//            std::cerr
//                << "[EventLooper::Loop][ERROR] Data sample name must contain '_' : "
//                << sampleName << "\n"
//                << "  Expected pattern: <DataSet>_<Era> (e.g. SingleMuon_B, JetHT_D)\n";
//            std::exit(1);
//        }
//
//        dataSet = sampleName(0, underscorePos);
//        era     = sampleName(underscorePos + 1, sampleName.Length() - underscorePos - 1);
//
//        std::cout
//            << "  [EventLooper::Loop] DataSet=" << dataSet
//            << ", Era=" << era << std::endl;
//    }

    // 교체 코드:
    TString sampleName = getInputName();
    sampleName.ReplaceAll(".root", "");
    std::cout << "Sample Name: " << sampleName << std::endl;
    
    const auto* sampleInfo = Config::GetSampleInfo(sampleName.Data());
    if (!sampleInfo) {
        std::cerr << "[EventLooper][FATAL] Unknown sample: " << sampleName << "\n"
                  << "  Register it in Config::SampleRegistry()\n";
        std::exit(1);
    }
    
    isData = sampleInfo->isData;
    double MC_weight = sampleInfo->weight;
    
    TString dataSet = "default";
    TString era     = "default";
    
    if (isData) {
        Ssiz_t underscorePos = sampleName.Index("_");
        if (underscorePos == kNPOS) {
            std::cerr << "[EventLooper][ERROR] Data sample name must contain '_' : "
                      << sampleName << "\n"
                      << "  Expected pattern: <DataSet>_<Era> (e.g. SingleMuon_B)\n";
            std::exit(1);
        }
        dataSet = sampleName(0, underscorePos);
        era     = sampleName(underscorePos + 1, sampleName.Length() - underscorePos - 1);
    
        std::cout << "  [EventLooper] DataSet=" << dataSet << ", Era=" << era << std::endl;
    } else {
        std::cout << "  [EventLooper] MC, weight=" << MC_weight << std::endl;
    }
 
    // ========================================================================
    // [Phase 4] Main Event Loop
    // ========================================================================
    Long64_t nentries = fChain->GetEntries();
    std::cout << ">>> Starting event loop: " << nentries << " entries\n";

    for (Long64_t jentry = 0; jentry < nentries; ++jentry) {
        reader->GetEntry(jentry);

        // Progress reporting
        if (jentry % Config::progressInterval == 0) {
            std::cout << "    Processing: " << jentry << " / " << nentries 
                      << " (" << (100.0 * jentry / nentries) << "%)\n";
        }

        // ====================================================================
        // Skimming Invariant Checks
        // ====================================================================
        if (reader->GetNJets() < 6) {
            FATAL_INVARIANT("NJets < 6", jentry);
        }

        double Jet6PT = reader->GetJetPt().at(5);
        if (Jet6PT <= 40.0) {
            FATAL_INVARIANT("6th jet pT <= 40 GeV", jentry);
        }

        double currentHT = reader->GetHT();
        if (currentHT < 500.0) {
            FATAL_INVARIANT("HT < 500 GeV", jentry);
        }

        double Jet6Eta = reader->GetJetEta().at(5);
        if (Jet6Eta < -2.4 || Jet6Eta > 2.4) {
            FATAL_INVARIANT("|eta| > 2.4 for 6th jet", jentry);
        }

        if (isData && reader->GetFailGoldenJson()) {
            FATAL_INVARIANT("Golden JSON failed", jentry);
        }

        // ====================================================================
        // Analysis Selection
        // ====================================================================
        if (!(reader->GetNMuons() == 1 && reader->GetNElecs() == 0)) continue;

        // ====================================================================
        // Event Weight
        // ====================================================================
        double weight = 1.0;
        if (!isData) {
            weight = reader->GetGenWeight()
                   * reader->GetPUWeight()
                   * reader->GetPrefireWeight()
                   * MC_weight;
        }

        // ====================================================================
        // Trigger Logic
        // ====================================================================
        if (!reader->GetPassTrigger_IsoMu27()) continue;

        bool isEraB = (era == "B");
        bool fired_4J3T = isEraB ? reader->GetPassTrigger_4J3T_B()
                                 : reader->GetPassTrigger_4J3T_CDEF();
        bool fired_6J1T = isEraB ? reader->GetPassTrigger_6J1T_B()
                                 : reader->GetPassTrigger_6J1T_CDEF();
        bool fired_6J2T = isEraB ? reader->GetPassTrigger_6J2T_B()
                                 : reader->GetPassTrigger_6J2T_CDEF();
        bool fired_HT   = reader->GetPassTrigger_PFHT1050();

        bool passHadTrig = (fired_4J3T || fired_6J1T || fired_6J2T || fired_HT);

        if (passHadTrig != reader->GetPassHadTrig()) {
            std::cerr << "[ERROR] Trigger logic mismatch at entry " << jentry << std::endl;
            std::exit(1);
        }

        // ====================================================================
        // SF Calculation (Step 2 only)
        // ====================================================================
        int currentNBJets = reader->GetNBJets();
        double sf = 1.0;

        // Get Trigger SF
        if (applySFMode && !isData && passHadTrig) {
            
            if (useDebugSF) {
                // --------------------------------------------------------------
                // DEBUG MODE: Get SF from ROOT histogram using FindBin
                // --------------------------------------------------------------
                std::string nbLabel = Config::GetNBLabel(currentNBJets);
                std::string etaLabel = Config::GetEtaLabel(Jet6Eta);
                std::string key = nbLabel + "_" + etaLabel;
                
                auto it = sfHistMap.find(key);
                if (it != sfHistMap.end() && it->second) {
                    TH2D* hSF = it->second;
                    
                    // Find bin (note: FindBin returns 0 for underflow, nbins+1 for overflow)
                    int binX = hSF->GetXaxis()->FindBin(currentHT);
                    int binY = hSF->GetYaxis()->FindBin(Jet6PT);
                    
                    // Check overflow/underflow before clamping
                    bool isOverUnderFlow = (binX == 0 || binX > hSF->GetNbinsX() ||
                                            binY == 0 || binY > hSF->GetNbinsY());
                    
                    // Clamp to valid range
                    int nBinsX = hSF->GetNbinsX();
                    int nBinsY = hSF->GetNbinsY();
                    int binX_clamped = std::max(1, std::min(binX, nBinsX));
                    int binY_clamped = std::max(1, std::min(binY, nBinsY));
                    
                    double sfVal = hSF->GetBinContent(binX_clamped, binY_clamped);
                    
                    // If measured SF exists (>0), use it; otherwise sf=1
                    if (sfVal > 0) {
                        sf = sfVal;
                        nEvtSFApplied++;
                        sumSF += sf;
                    } else {
                        sf = 1.0;  // Empty bin → no correction
                        nEvtSF1++;
                    }
                    
                    // Debug logging: first 30 events + every 100k
                    static int debugCount = 0;
                    if (debugCount < 30 || jentry % 100000 == 0) {
                        std::cout << "[DEBUG SF] entry=" << jentry
                                  << " | nB=" << currentNBJets 
                                  << " key=" << key
                                  << " | HT=" << currentHT << " pT=" << Jet6PT
                                  << " | bin=(" << binX << "," << binY << ")";
                        if (isOverUnderFlow) std::cout << " [OVERFLOW/UNDERFLOW]";
                        std::cout << " | sfVal=" << sfVal 
                                  << " → sf=" << sf << "\n";
                        debugCount++;
                    }
                } else {
                    sf = 1.0;
                    nEvtSF1++;
                    static bool warned = false;
                    if (!warned) {
                        std::cout << "[WARN] SF histogram not found for key=" << key << "\n";
                        warned = true;
                    }
                }
                
            } else {
                // --------------------------------------------------------------
                // NORMAL MODE: Get SF from JSON (correctionlib)
                // --------------------------------------------------------------
                try {
                    sf = sf_provider->evaluate({
                        static_cast<int>(currentNBJets),
                        static_cast<double>(Jet6Eta),
                        static_cast<double>(currentHT),
                        static_cast<double>(Jet6PT)
                    });
                //} catch (...) {
		} catch (const std::exception& e) {
                    //sf = 1.0;
		    std::cerr << "\n[FATAL] SF evaluate failed at entry " << jentry << "\n"
                              << "  nbJets=" << currentNBJets
                              << " eta=" << Jet6Eta
                              << " HT=" << currentHT
                              << " pT=" << Jet6PT << "\n"
                              << "  exception: " << e.what() << "\n";
                    std::exit(57);
                }
            }
        }

        // weightWithSFlogic:
        //   - Data: weight=1, sf=1 → weightWithSFlogic=1
        //   - MC, !passHadTrig: sf=1 → weightWithSFlogic=weight
        //   - MC, passHadTrig: sf from histogram/JSON → weightWithSFlogic=weight*sf
        double weightWithSFlogic = weight * sf;

        // Safety checks
        if (isData && (weight != 1.0 || sf != 1.0)) {
            std::cerr << "\n[ERROR] Data's weight and sf must be 1.0" << std::endl;
            std::cerr << "  weight = " << weight << std::endl;
            std::cerr << "  sf     = " << sf << std::endl;
            std::exit(55);
        }
        if (!isData && !passHadTrig && sf != 1.0) {
            std::cerr << "\n[ERROR] MC event which did not pass trigger path," << std::endl;
            std::cerr << "  sf should be 1.0 but current sf = " << sf << std::endl;
            std::exit(56);
        }

        // ====================================================================
        // Fill Histograms
        // ====================================================================
        if (applySFMode) {
            // Step 2: Fill both noSF and SF histograms
            //
            // SF correction logic for efficiency validation:
            //   - Total: NO SF (denominator should be unchanged)
            //   - Pass: SF applied (numerator gets corrected)
            //
            // This gives: eff_SF = Pass_SF / Total = (Pass * SF) / Total
            //                    ≈ eff_MC * SF ≈ eff_Data
            //
            // This is the correct way to validate trigger SF!

            // --- No SF version (raw weight) ---
            if (h_HT_Total) h_HT_Total->Fill(currentHT, weight);
            if (h_pT_Total) h_pT_Total->Fill(Jet6PT, weight);
            if (h_Eta_Total) h_Eta_Total->Fill(Jet6Eta, weight);
            if (h_nbJets_Total) {
                int nbIdx = Config::GetNBBinIndex(currentNBJets);
                if (nbIdx >= 0) h_nbJets_Total->Fill(nbIdx + 0.5, weight);
            }

            if (passHadTrig) {
                if (h_HT_Pass) h_HT_Pass->Fill(currentHT, weight);
                if (h_pT_Pass) h_pT_Pass->Fill(Jet6PT, weight);
                if (h_Eta_Pass) h_Eta_Pass->Fill(Jet6Eta, weight);
                if (h_nbJets_Pass) {
                    int nbIdx = Config::GetNBBinIndex(currentNBJets);
                    if (nbIdx >= 0) h_nbJets_Pass->Fill(nbIdx + 0.5, weight);
                }
            }

            // --- With SF version ---
            // Total: NO SF applied (same as noSF)
            if (h_HT_Total_SF) h_HT_Total_SF->Fill(currentHT, weight);
            if (h_pT_Total_SF) h_pT_Total_SF->Fill(Jet6PT, weight);
            if (h_Eta_Total_SF) h_Eta_Total_SF->Fill(Jet6Eta, weight);
            if (h_nbJets_Total_SF) {
                int nbIdx = Config::GetNBBinIndex(currentNBJets);
                if (nbIdx >= 0) h_nbJets_Total_SF->Fill(nbIdx + 0.5, weight);
            }

            // Pass: SF applied
            if (passHadTrig) {
                if (h_HT_Pass_SF) h_HT_Pass_SF->Fill(currentHT, weightWithSFlogic);
                if (h_pT_Pass_SF) h_pT_Pass_SF->Fill(Jet6PT, weightWithSFlogic);
                if (h_Eta_Pass_SF) h_Eta_Pass_SF->Fill(Jet6Eta, weightWithSFlogic);
                if (h_nbJets_Pass_SF) {
                    int nbIdx = Config::GetNBBinIndex(currentNBJets);
                    if (nbIdx >= 0) h_nbJets_Pass_SF->Fill(nbIdx + 0.5, weightWithSFlogic);
                }
            }

        } else {
            // Step 1: Fill standard histograms + 2D maps
            if (h_HT_Total) h_HT_Total->Fill(currentHT, weight);
            if (h_pT_Total) h_pT_Total->Fill(Jet6PT, weight);
            if (h_Eta_Total) h_Eta_Total->Fill(Jet6Eta, weight);
            if (h_nbJets_Total) {
                int nbIdx = Config::GetNBBinIndex(currentNBJets);
                if (nbIdx >= 0) h_nbJets_Total->Fill(nbIdx + 0.5, weight);
            }

            if (passHadTrig) {
                if (h_HT_Pass) h_HT_Pass->Fill(currentHT, weight);
                if (h_pT_Pass) h_pT_Pass->Fill(Jet6PT, weight);
                if (h_Eta_Pass) h_Eta_Pass->Fill(Jet6Eta, weight);
                if (h_nbJets_Pass) {
                    int nbIdx = Config::GetNBBinIndex(currentNBJets);
                    if (nbIdx >= 0) h_nbJets_Pass->Fill(nbIdx + 0.5, weight);
                }
            }

            // Fill 2D maps
            std::string nbLabel  = Config::GetNBLabel(currentNBJets);
            std::string etaLabel = Config::GetEtaLabel(Jet6Eta);

            if (!nbLabel.empty() && !etaLabel.empty()) {
                TString key = Form("%s_%s", nbLabel.c_str(), etaLabel.c_str());

                auto itTotal = map_Total.find(key);
                auto itPass  = map_Pass.find(key);

                if (itTotal != map_Total.end() && itTotal->second) {
                    itTotal->second->Fill(currentHT, Jet6PT, weight);
                }
                if (passHadTrig && itPass != map_Pass.end() && itPass->second) {
                    itPass->second->Fill(currentHT, Jet6PT, weight);
                }
            }
        }
    }

    // ========================================================================
    // [Phase 5] Finalize & Write Output
    // ========================================================================
    
    // Print SF statistics (debug mode)
    if (applySFMode && useDebugSF && !isData) {
        std::cout << "\n";
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║  DEBUG MODE: SF Application Summary                          ║\n";
        std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
        std::cout << "  Events with SF applied (SF != 1): " << nEvtSFApplied << "\n";
        std::cout << "  Events with SF = 1 (empty bin):   " << nEvtSF1 << "\n";
        if (nEvtSFApplied > 0) {
            std::cout << "  Average SF (when applied):        " << sumSF / nEvtSFApplied << "\n";
        }
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";
    }
    
    std::cout << ">>> Writing output to: " << getOutputName() << "\n";
    outputFile->cd();

    // Write histograms
    if (h_HT_Total) h_HT_Total->Write();
    if (h_HT_Pass)  h_HT_Pass->Write();
    if (h_pT_Total) h_pT_Total->Write();
    if (h_pT_Pass)  h_pT_Pass->Write();
    if (h_Eta_Total) h_Eta_Total->Write();
    if (h_Eta_Pass)  h_Eta_Pass->Write();
    if (h_nbJets_Total) h_nbJets_Total->Write();
    if (h_nbJets_Pass)  h_nbJets_Pass->Write();

    // Step 2 specific: Write SF histograms
    if (applySFMode) {
        if (h_HT_Total_SF) h_HT_Total_SF->Write();
        if (h_HT_Pass_SF)  h_HT_Pass_SF->Write();
        if (h_pT_Total_SF) h_pT_Total_SF->Write();
        if (h_pT_Pass_SF)  h_pT_Pass_SF->Write();
        if (h_Eta_Total_SF) h_Eta_Total_SF->Write();
        if (h_Eta_Pass_SF)  h_Eta_Pass_SF->Write();
        if (h_nbJets_Total_SF) h_nbJets_Total_SF->Write();
        if (h_nbJets_Pass_SF)  h_nbJets_Pass_SF->Write();
    }

    // Step 1 specific: Write 2D maps
    if (!applySFMode) {
        for (auto& p : map_Total) {
            if (p.second) p.second->Write();
        }
        for (auto& p : map_Pass) {
            if (p.second) p.second->Write();
        }
    }

    std::cout << ">>> Done.\n";

    // Cleanup
    if (sfRootFile) {
        sfRootFile->Close();
        delete sfRootFile;
    }
    delete outputFile;
}

// ============================================================================
// Helper Methods
// ============================================================================
void EventLooper::setNtupleName(TString _name) { ntupleName = _name; }
TString EventLooper::getInputName() { return ntupleName + ".root"; }
TString EventLooper::getOutputName() {
    return applySFMode 
        ? TString(Config::outPrefix_Step2) + ntupleName + ".root"
        : TString(Config::outPrefix_Step1) + ntupleName + ".root";
}

