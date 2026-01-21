#define EventLooper_cxx
#include "EventLooper.hh"
#include "BinConfig.hh"

#include <TH2.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <cstdlib>


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
// Role:
//   - Open input file
//   - Get TTree pointer
//   - Instantiate NtupleReader
// ============================================================================

void EventLooper::Init()
{
    // ------------------------------------------------------------------------
    // Build input file path (your local path pattern)
    // ------------------------------------------------------------------------
    TString ntuplePath = "/Users/jhlee/ttHH/ntuple/skimmed/gen_tier3/" + getInputName();

    // Open input ROOT file and keep it as a member so it stays alive
    inputFile = TFile::Open(ntuplePath);
    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "[EventLooper][ERROR] Cannot open " << ntuplePath << std::endl;
        std::exit(1);
    }

    // Retrieve TTree
    inputFile->GetObject("Tree/Tree", fChain);
    if (!fChain) {
        std::cerr << "[EventLooper][ERROR] Cannot get TTree: Tree/Tree" << std::endl;
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
// Role:
//   - Create output file and histograms/maps
//   - Main event loop over input tree entries
//   - Apply selection, triggers, compute weights
//   - Fill histograms/maps depending on Step 1 or Step 2 mode
// ============================================================================

void EventLooper::Loop()
{
    if (fChain == nullptr) return;

    // ------------------------------------------------------------------------
    // 1) Read binning configuration from BinConfig
    //
    // HT_Bins/PT_Bins are returned as const references to internal vectors.
    // Eta_Bins and NB_Bins are optional depending on UseEta/UseNBjets.
    // ------------------------------------------------------------------------
    const std::vector<double>& HT_Bins = BinConfig::HT_Bins();
    const std::vector<double>& PT_Bins = BinConfig::PT_Bins();

    std::vector<double> Eta_Bins_Val;
    if (BinConfig::UseEta()) {
        // Make a local copy for safe .data() usage even if you later modify it
        Eta_Bins_Val = BinConfig::Eta_Bins();
    }

    std::vector<double> NB_Bins_Val;
    if (BinConfig::UseNBjets() && !BinConfig::NB_Bins().empty()) {
        // Convert NB thresholds into histogram edges:
        //   thresholds [2,3,4] -> edges [2,3,4,5]
        int minB = BinConfig::NB_Bins().front();
        int maxB = BinConfig::NB_Bins().back();
        for (int i = minB; i <= maxB + 1; ++i) {
            NB_Bins_Val.push_back(static_cast<double>(i));
        }
    }

    // ------------------------------------------------------------------------
    // 2) Create output ROOT file and histograms
    // ------------------------------------------------------------------------
    TFile* outputFile = new TFile(getOutputName(), "RECREATE");
    outputFile->cd();

    // Step 2 uses an output tree to store new_weight
    TTree* outputTree = nullptr;
    Float_t new_weight = 1.0f;

    // 1D validation histograms (Total / Pass)
    h_HT_Total = new TH1D("h_HT_Total", "HT;HT [GeV];Events",
                          HT_Bins.size() - 1, HT_Bins.data());
    h_HT_Pass  = new TH1D("h_HT_Pass", "HT;HT [GeV];Events",
                          HT_Bins.size() - 1, HT_Bins.data());

    h_pT_Total = new TH1D("h_pT_Total", "6th Jet pT;pT [GeV];Events",
                          PT_Bins.size() - 1, PT_Bins.data());
    h_pT_Pass  = new TH1D("h_pT_Pass", "6th Jet pT;pT [GeV];Events",
                          PT_Bins.size() - 1, PT_Bins.data());

    // Optional eta histograms
    if (BinConfig::UseEta() && !Eta_Bins_Val.empty()) {
        h_Eta_Total = new TH1D("h_Eta_Total", "6th Jet Eta;Eta;Events",
                               Eta_Bins_Val.size() - 1, Eta_Bins_Val.data());
        h_Eta_Pass  = new TH1D("h_Eta_Pass", "6th Jet Eta;Eta;Events",
                               Eta_Bins_Val.size() - 1, Eta_Bins_Val.data());
    }

    // Optional nbjets histograms
    if (BinConfig::UseNBjets() && !NB_Bins_Val.empty()) {
        h_nbJets_Total = new TH1D("h_nbJets_Total", "nbJets;Number of b-jets;Events",
                                  NB_Bins_Val.size() - 1, NB_Bins_Val.data());
        h_nbJets_Pass  = new TH1D("h_nbJets_Pass", "nbJets;Number of b-jets;Events",
                                  NB_Bins_Val.size() - 1, NB_Bins_Val.data());
    }

    // ------------------------------------------------------------------------
    // 3) Mode-specific setup
    // ------------------------------------------------------------------------
    if (applySFMode) {
        // Step 2: load correctionlib JSON and create SF provider
        std::cout << ">>> [Mode] Step 2: Loading Correction JSON...\n";
        try {
            cset = correction::CorrectionSet::from_file("trigger_sf.json.gz");
            sf_provider = cset->at("triggerSF");
        } catch (std::exception& e) {
            std::cerr << "[EventLooper][ERROR] Failed to load JSON: " << e.what() << std::endl;
            std::exit(1);
        }

        // Clone tree structure for output and add a new branch for the corrected weight
        outputTree = fChain->CloneTree(0);
        outputTree->SetDirectory(outputFile);
        outputTree->Branch("new_weight", &new_weight, "new_weight/F");
    } else {
        // Step 1: pre-create 2D maps for each (nbLabel, etaLabel) category
        std::cout << ">>> [Mode] Step 1: Creating 2D Maps...\n";

        std::vector<std::string> nbLabels  = BinConfig::NB_Labels();
        std::vector<std::string> etaLabels = BinConfig::Eta_Labels();

        for (const auto& nb : nbLabels) {
            for (const auto& eta : etaLabels) {
                TString key = Form("%s_%s", nb.c_str(), eta.c_str());
                map_Total[key] = new TH2D("Total_" + key, key,
                                          HT_Bins.size() - 1, HT_Bins.data(),
                                          PT_Bins.size() - 1, PT_Bins.data());
                map_Pass[key]  = new TH2D("Pass_" + key, key,
                                          HT_Bins.size() - 1, HT_Bins.data(),
                                          PT_Bins.size() - 1, PT_Bins.data());
            }
        }
    }


    // ------------------------------------------------------------------------
    // 4) Determine if the sample is Data or MC, and parse dataset/era if Data
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

    for (Long64_t jentry = 0; jentry < nentries; ++jentry) {
        // Load the current event entry into the reader
        reader->GetEntry(jentry);

        // --------------------------------------------------------------------
        // Invariants (must be guaranteed by the ntuple skimmer)
        // If violated, stop immediately because the input is inconsistent.
        // --------------------------------------------------------------------
        if (reader->GetJetPt().size() != static_cast<size_t>(reader->GetNJets())) {
            FATAL_INVARIANT("JetPt.size() != NJets (branch mismatch)", jentry);
        }
 
        if (reader->GetNJets() < 7) {
            FATAL_INVARIANT("NJets < 7 (should have been rejected in skimming)", jentry);
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

        // --------------------------------------------------------------------
        // Step 2: apply SF and fill output tree (MC only)
        // --------------------------------------------------------------------
        double fillWeight = weight;
        int currentNBJets = reader->GetNBJets();

        if (applySFMode) {
            if (!isData) {
                double sf = 1.0;
                try {
                    // Evaluate SF using correctionlib:
                    // input order must match the JSON definition.
                    sf = sf_provider->evaluate({
                        (int)currentNBJets,
                        (double)Jet6Eta,
                        (double)currentHT,
                        (double)Jet6PT
                    });
                } catch (...) {
                    // In case of out-of-range or evaluation failure, fall back to 1.0
                    sf = 1.0;
                }

                new_weight = static_cast<Float_t>(weight * sf);
                fillWeight = new_weight;
            } else {
                // Data: SF is not applied; keep original weight (usually 1.0)
                new_weight = static_cast<Float_t>(weight);
                fillWeight = weight;
            }

            // Store the corrected weight as a new branch
            outputTree->Fill();
        }

        // --------------------------------------------------------------------
        // Fill validation histograms (Total vs Pass)
        // --------------------------------------------------------------------
        h_HT_Total->Fill(currentHT, weight);
        h_pT_Total->Fill(Jet6PT, weight);
        if (h_Eta_Total)    h_Eta_Total->Fill(Jet6Eta, weight);
        if (h_nbJets_Total) h_nbJets_Total->Fill(currentNBJets, weight);

        if (passHadTrig) {
            // Numerator (Pass):
            // - In Step 2 for MC, use SF-corrected weight (new_weight)
            // - Otherwise, use original weight
            double passW = (applySFMode && !isData) ? new_weight : weight;

            h_HT_Pass->Fill(currentHT, passW);
            h_pT_Pass->Fill(Jet6PT, passW);
            if (h_Eta_Pass)    h_Eta_Pass->Fill(Jet6Eta, passW);
            if (h_nbJets_Pass) h_nbJets_Pass->Fill(currentNBJets, passW);
        }

        // --------------------------------------------------------------------
        // Step 1: fill 2D maps (efficiency calculation)
        // --------------------------------------------------------------------
        if (!applySFMode) {
            std::string nbLabel  = BinConfig::GetNBLabel(currentNBJets);
            std::string etaLabel = BinConfig::GetEtaLabel(Jet6Eta);

            // If either label is invalid/out-of-range, skip map filling
            if (!nbLabel.empty() && !etaLabel.empty()) {
                TString key = Form("%s_%s", nbLabel.c_str(), etaLabel.c_str());
                if (map_Total.count(key)) {
                    map_Total[key]->Fill(currentHT, Jet6PT, weight);
                    if (passHadTrig) map_Pass[key]->Fill(currentHT, Jet6PT, weight);
                }
            }
        }
    }

    // ------------------------------------------------------------------------
    // 6) Finalize & save outputs
    // ------------------------------------------------------------------------
    outputFile->cd();

    h_HT_Total->Write(); h_HT_Pass->Write();
    h_pT_Total->Write(); h_pT_Pass->Write();

    if (h_Eta_Total) {
        h_Eta_Total->Write();
        h_Eta_Pass->Write();
    }

    if (h_nbJets_Total) {
        h_nbJets_Total->Write();
        h_nbJets_Pass->Write();
    }

    if (applySFMode) {
        outputTree->Write();
    } else {
        for (auto& p : map_Total) p.second->Write();
        for (auto& p : map_Pass)  p.second->Write();
    }

    // Close output and clean up
    delete outputFile;
}

// ============================================================================
// Helper functions: naming
// ============================================================================

void EventLooper::setNtupleName(TString _name) { ntupleName = _name; }
TString EventLooper::getInputName() { return ntupleName + ".root"; }

TString EventLooper::getOutputName() {
    // Step 2: corrected_*.root
    // Step 1: output_*.root
    return applySFMode ? "corrected_" + ntupleName + ".root"
                       : "output_" + ntupleName + ".root";
}

