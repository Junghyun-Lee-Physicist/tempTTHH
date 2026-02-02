#define EventLooper_cxx
#include "EventLooper.hh"
#include "Config.hh"

#include <TH2.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <cstdlib>


// ============================================================================
// Helper: Fatal invariant violation
//
// These conditions MUST hold after skimming. If violated, something is wrong
// with the upstream skimming step - abort immediately with diagnostic info.
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
    // Everything else is already initialized by in-class defaults (nullptr, etc.)
}

EventLooper::~EventLooper()
{
    // ------------------------------------------------------------------------
    // Clean up objects created with new
    // ------------------------------------------------------------------------
    if (reader) {
        delete reader;
        reader = nullptr;
    }

    // Close and delete input ROOT file
    if (inputFile) {
        inputFile->Close();
        delete inputFile;
        inputFile = nullptr;
    }

    // NOTE:
    //   Histograms are owned by the output ROOT file after Write(), and the
    //   output file is deleted at the end of Loop(). If you want strict cleanup,
    //   you can also delete hist pointers here, but it is not required for
    //   one-shot executable runs.
}

// ============================================================================
// Init Method
//
// Responsibilities:
//   1. Open input ROOT file
//   2. Retrieve TTree pointer
//   3. Instantiate NtupleReader for branch access
// ============================================================================
void EventLooper::Init()
{

    // ------------------------------------------------------------------------
    // Build input file path using Config settings
    // ------------------------------------------------------------------------
    TString ntuplePath = Config::inputBaseDir + getInputName();

    inputFile = TFile::Open(ntuplePath);
    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "[EventLooper][ERROR] Cannot open " << ntuplePath << std::endl;
        std::exit(1);
    }

    // ------------------------------------------------------------------------
    // Retrieve TTree using configured path
    // ------------------------------------------------------------------------
    inputFile->GetObject(Config::treePath.c_str(), fChain);
    if (!fChain) {
        std::cerr << "[EventLooper][ERROR] Cannot get TTree: " 
                  << Config::treePath << std::endl;
        std::exit(1);
    }

    // Instantiate the NtupleReader wrapper
    // This should set all branches internally and provide getters like:
    //   reader->GetJetPt(), reader->GetHT(), reader->GetPassTrigger_IsoMu27(), ...
    reader = new NtupleReader(fChain);
}

// ============================================================================
// Loop Method
//
// Main event processing loop with two operation modes:
//   - Step 1 (applySFMode = false): Create 2D efficiency maps
//   - Step 2 (applySFMode = true):  Apply SF corrections to MC
// ============================================================================
void EventLooper::Loop()
{
    if (fChain == nullptr) return;

    // ========================================================================
    // [Phase 1] Configuration & Initialization
    // ========================================================================

    // Print configuration summary at startup
    Config::Dump();

    const std::vector<double>& HT_Bins = Config::HT_Bins();
    const std::vector<double>& PT_Bins = Config::PT_Bins();

    // Optional: Eta bins
    std::vector<double> Eta_Bins_Val;
    if (Config::useEta) {
        Eta_Bins_Val = Config::Eta_Bins();
    }

    // Optional: NB histogram edges
    std::vector<double> NB_Edges;
    if (Config::useNBjets) {
        NB_Edges = Config::MakeNBEdges();
    }

    // ========================================================================
    // [Phase 2] Output File & Histogram Creation
    // ========================================================================

    TFile* outputFile = new TFile(getOutputName(), "RECREATE");
    outputFile->cd();

    // Initialize all histogram pointers to nullptr
    // W/O SF
    h_HT_Total = nullptr;
    h_HT_Pass = nullptr;
    h_pT_Total = nullptr;
    h_pT_Pass = nullptr;
    h_Eta_Total = nullptr;
    h_Eta_Pass = nullptr;
    h_nbJets_Total = nullptr;
    h_nbJets_Pass = nullptr;
    // W/ SF
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

        std::cout << ">>> [Mode] Step 1: Creating Efficiency Maps\n";

        auto nbLabels  = Config::NB_Labels();
        auto etaLabels = Config::Eta_Labels();

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
    if (applySFMode) {
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
    TString sampleName = getInputName();
    sampleName.ReplaceAll(".root", "");   // strip extension safely for this convention
    std::cout << "Sample Name: " << sampleName << std::endl;
    
    // Default: treat as Data unless it matches known MC names
    isData = true;
    
    TString dataSet = "default";
    TString era     = "default";
    
    // --- Explicit MC identification (same policy as your original code) ---
    double MC_weight = -9999999.9; // (Xsec * Lumi) / Sum(Runs.genEventSumw)
    if (sampleName == "TTTo2L2Nu") {
        isData  = false;
	MC_weight = 0.0004761561474;
    }
    else if (sampleName == "TTToHadronic") {
        isData  = false;
        MC_weight = 0.000214351205;
    }
    else if (sampleName == "TTToSemiLeptonic") {
        isData  = false;
        MC_weight = 0.0001455793461;
    }
    else {
        std::cout << "Current Sample: Data\n";
    }
    

    // --- If Data, parse "<DataSet>_<Era>" ---
    if (isData) {
        Ssiz_t underscorePos = sampleName.Index("_");
        if (underscorePos == kNPOS) {
            std::cerr
                << "[EventLooper::Loop][ERROR] Data sample name must contain '_' : "
                << sampleName << "\n"
                << "  Expected pattern: <DataSet>_<Era> (e.g. SingleMuon_B, JetHT_D)\n";
            std::exit(1);
        }
    
        dataSet = sampleName(0, underscorePos);
        era     = sampleName(underscorePos + 1, sampleName.Length() - underscorePos - 1);
    
        std::cout
            << "  [EventLooper::Loop] DataSet=" << dataSet
            << ", Era=" << era << std::endl;
    }


    // ------------------------------------------------------------------------
    // 5) Main event loop
    // ------------------------------------------------------------------------
    Long64_t nentries = fChain->GetEntriesFast();
    std::cout << ">>> Starting event loop: " << nentries << " entries\n";

    for (Long64_t jentry = 0; jentry < nentries; ++jentry) {
        // Load the current event entry into the reader
        reader->GetEntry(jentry);

        // --------------------------------------------------------------------
        // Progress reporting
        // --------------------------------------------------------------------
        //if (jentry % Config::progressInterval == 0) {
        //    std::cout << "    Processing: " << jentry << " / " << nentries 
        //              << " (" << (100.0 * jentry / nentries) << "%)\n";
        //}

        // --------------------------------------------------------------------
        // Invariants (must be guaranteed by the ntuple skimmer)
        // If violated, stop immediately because the input is inconsistent.
        // --------------------------------------------------------------------
        if (reader->GetJetPt().size() != static_cast<size_t>(reader->GetNJets())) {
            FATAL_INVARIANT("JetPt.size() != NJets (branch mismatch)", jentry);
        }
 
        if (reader->GetNJets() < 6) {
            FATAL_INVARIANT("NJets < 6 (should have been rejected in skimming)", jentry);
        }

        if (!reader->GetPassMETFilters()) {
            FATAL_INVARIANT("Noise filters(=MET filters) failed (should have been rejected in skimming)", jentry);
        }
        
        double Jet6PT = reader->GetJetPt().at(5);
        if (Jet6PT <= 40.0) {
            FATAL_INVARIANT("6th jet pT <= 40 GeV (should have been rejected in skimming)", jentry);
        }
        
        double currentHT = reader->GetHT();
        if (currentHT < 500.0) {
            FATAL_INVARIANT("HT < 500 GeV (should have been rejected in skimming)", jentry);
        }
        
        double Jet6Eta = reader->GetJetEta().at(5);
        if (Jet6Eta < -2.4 || Jet6Eta > 2.4) {
            FATAL_INVARIANT("|eta| > 2.4 for 6th jet (should have been rejected in skimming)", jentry);
        }
        
        //if (reader->GetNBJets() < 3) continue;

        if(isData) {
            // Analyzer output must be passed golden JSON
            if(reader->GetFailGoldenJson() == true){
                FATAL_INVARIANT("Golden JSON failed (should have been rejected in skimming)", jentry);
	        }
        }

        // --------------------------------------------------------------------
        // Analysis selection (allowed to reject via continue)
        // --------------------------------------------------------------------
        // Lepton selection: this is *analysis logic* (you said only this is not enforced upstream)
	    // Demand [ 1 tight lead muon ] + [ Veto sublead electron ]
        if (!(reader->GetNMuons() == 1 && reader->GetNElecs() == 0)) continue;


        // --------------------------------------------------------------------
        // Event weight (Data vs MC)
        // --------------------------------------------------------------------
        double weight = 1.0;
        if (!isData) {
            // MC weight composition (as your code defined)
            weight = reader->GetGenWeight()
                   * reader->GetPUWeight()
                   * reader->GetPrefireWeight()
                   * MC_weight;
        } 
        
        //std::cout << std::scientific << std::setprecision(8); 
        //std::cout<<"MC_weight : "<<MC_weight<<std::endl;
        //std::cout<<"gen weight : "<<reader->GetGenWeight()<<std::endl;
        //std::cout<<"PU weight  : "<<reader->GetPUWeight()<<std::endl;
        //std::cout<<"prefire weight : "<<reader->GetPrefireWeight()<<std::endl;
        //std::cout<<"  --> calculated weight : "<<weight<<std::endl;
        //std::cout<<"weight from tree  : "<<reader->GetEvtWeight()<<std::endl;

        // --------------------------------------------------------------------
        // Trigger logic 
        // --------------------------------------------------------------------
        // Pre-selection trigger
        if (!reader->GetPassTrigger_IsoMu27()) continue;

        // Hadronic trigger logic (target trigger)
        bool passHadTrig = false;

        bool isEraB = ( era == "B" );
	    // MC & CDEF Data --> isEraB = false
	    // B period Data  --> isEraB = true
	    bool fired_4J3T = isEraB ? reader->GetPassTrigger_4J3T_B()
	                         : reader->GetPassTrigger_4J3T_CDEF();
        bool fired_6J1T = isEraB ? reader->GetPassTrigger_6J1T_B()
	                         : reader->GetPassTrigger_6J1T_CDEF();
        bool fired_6J2T = isEraB ? reader->GetPassTrigger_6J2T_B()
	                         : reader->GetPassTrigger_6J2T_CDEF();
        bool fired_HT   = reader->GetPassTrigger_PFHT1050();

        passHadTrig = (fired_4J3T || fired_6J1T || fired_6J2T || fired_HT);

        if(passHadTrig != reader->GetPassHadTrig()){
	        std::cout<<"Trigger logic is weird, Please check"<<std::endl;
	        exit(1);
        }
 
        // ====================================================================
        // SF Calculation (Step 2 only)
        // ====================================================================
        int currentNBJets = reader->GetNBJets();
        double sf = 1.0;

        // Get Trigger SF from json file
        if (applySFMode && !isData && passHadTrig) {
            try {
                // Evaluate SF using correctionlib:
                // input order must match the JSON definition.
                sf = sf_provider->evaluate({
                    static_cast<int>(currentNBJets),
                    static_cast<double>(Jet6Eta),
                    static_cast<double>(currentHT),
                    static_cast<double>(Jet6PT)			
                });
            } catch (...) {
                // In case of out-of-range or evaluation failure, fall back to 1.0
                sf = 1.0;
            }
        }

        // weightWithSFlogic:
        //   - Data: weight=1, sf=1 → weightWithSFlogic=1
        //   - MC, !passHadTrig: sf=1 → weightWithSFlogic=weight
        //   - MC, passHadTrig: sf from JSON → weightWithSFlogic=weight*sf
        double weightWithSFlogic = weight * sf; 
                                           
        // SF safety check
        if(isData && (weight != 1.0 || sf != 1.0) ){
            std::cerr << "\n[ERROR] data's weight and sf must be 1.0" << std::endl;
            std::cerr << "But current weight = " << weight << std::endl;
            std::cerr << "And current sf     = " << sf << std::endl;
            exit(55);
        }
        if(!isData && !passHadTrig && sf != 1.0){
            std::cerr << "\n[ERROR] MC event which did not pass trigger path," << std::endl;
            std::cerr << "It's sf should be 1.0 but current sf is set as = " << sf << std::endl;
            exit(56);
        }

        // --------------------------------------------------------------------
        // Fill validation histograms (Total vs Pass)
        // --------------------------------------------------------------------
        if (applySFMode) {
            // Step 2: Fill both noSF and SF histograms
            //
            // noSF: raw MC weight (for comparison)
            // SF: weight with trigger SF applied (weightWithSFlogic)
            //     - passHadTrig=true: weight*sf
            //     - passHadTrig=false: weight (sf=1)

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

            // --- With SF version (weightWithSFlogic) ---
            if (h_HT_Total_SF) h_HT_Total_SF->Fill(currentHT, weight);
            if (h_pT_Total_SF) h_pT_Total_SF->Fill(Jet6PT, weight);
            if (h_Eta_Total_SF) h_Eta_Total_SF->Fill(Jet6Eta, weight);
            if (h_nbJets_Total_SF) {
                int nbIdx = Config::GetNBBinIndex(currentNBJets);
                if (nbIdx >= 0) h_nbJets_Total_SF->Fill(nbIdx + 0.5, weight);
            }

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

    // ------------------------------------------------------------------------
    // 6) Finalize & save outputs
    // ------------------------------------------------------------------------
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

    // Close output and clean up
    delete outputFile;
}

// ============================================================================
// Helper functions: naming
// ============================================================================

void EventLooper::setNtupleName(TString _name) { ntupleName = _name; }
TString EventLooper::getInputName() { return ntupleName + ".root"; }
TString EventLooper::getOutputName() {
    return applySFMode 
        ? TString(Config::outPrefix_Step2) + ntupleName + ".root"
        : TString(Config::outPrefix_Step1) + ntupleName + ".root";
}


