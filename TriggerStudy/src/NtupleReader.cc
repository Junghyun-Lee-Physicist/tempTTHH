/**
 * @file NtupleReader.cc
 * @brief Implementation of the NtupleReader class.
 *
 * ======================================================================================
 * USAGE EXAMPLE
 * ======================================================================================
 *
 * // 0. Include Ntuple Header
 * #include "NtupleReader.hh"
 *
 * void Analyze(TString fileName) {
 * // 1. Open the file and retrieve the TTree
 * TFile* f = TFile::Open(fileName);
 * TTree* t = (TTree*)f->Get("Tree/Tree");
 *
 * // 2. Initialize the NtupleReader
 * NtupleReader reader(t);
 *
 * // 3. Event Loop
 * Long64_t nEntries = t->GetEntries();
 * for (Long64_t i = 0; i < nEntries; ++i) {
 * reader.GetEntry(i); // Load data for the current event
 *
 * // 4. Access variables using Get methods
 * if (reader.GetHT() < 500.0) continue;
 * if (reader.GetNJets() < 4) continue;
 *
 * // Example: Accessing vector elements
 * // Note: Always check vector size before accessing index
 * if (reader.GetJetPt().size() > 0) {
 * double leadingJetPt = reader.GetJetPt().at(0);
 * }
 * }
 *
 * f->Close();
 * }
 * ======================================================================================
 */

#include "NtupleReader.hh"

NtupleReader::NtupleReader(TTree *tree) : fChain(tree) {
    // Safety check: if the tree is null, do nothing
    if (!fChain) return;

    // IMPORTANT: Pointer objects (like std::vector) must be initialized to 0 (nullptr).
    // If not, ROOT might crash or cause memory corruption when connecting branches.
    jetPt = 0;
    jetEta = 0;

    // Disable MakeClass mode to properly handle complex objects like std::vector.
    // MakeClass mode decomposes objects into basic types, which breaks object access.
    fChain->SetMakeClass(0);

    // =========================================================================
    // Connect Branches
    // Syntax: fChain->SetBranchAddress("NameInTree", &Variable, &BranchPointer);
    //
    // Why use &BranchPointer (3rd argument)?
    // 1. Optimization: It stores the branch address directly, bypassing 
    //    string-based lookups (hash map searches) during event processing.
    // 2. Flexibility: Allows calling b_BranchName->GetEntry(i) later if you
    //    only want to read specific branches (Lazy Loading).
    // =========================================================================

    // 1. Triggers
    fChain->SetBranchAddress("passTrigger_HLT_IsoMu27", &passTrigger_HLT_IsoMu27, &b_passTrigger_HLT_IsoMu27);
    fChain->SetBranchAddress("passTrigger_HLT_PFHT1050", &passTrigger_HLT_PFHT1050, &b_passTrigger_HLT_PFHT1050);
    
    fChain->SetBranchAddress("passTrigger_6J1T_B", &passTrigger_6J1T_B, &b_passTrigger_6J1T_B);
    fChain->SetBranchAddress("passTrigger_6J1T_CDEF", &passTrigger_6J1T_CDEF, &b_passTrigger_6J1T_CDEF);
    fChain->SetBranchAddress("passTrigger_6J2T_B", &passTrigger_6J2T_B, &b_passTrigger_6J2T_B);
    fChain->SetBranchAddress("passTrigger_6J2T_CDEF", &passTrigger_6J2T_CDEF, &b_passTrigger_6J2T_CDEF);
    fChain->SetBranchAddress("passTrigger_4J3T_B", &passTrigger_4J3T_B, &b_passTrigger_4J3T_B);
    fChain->SetBranchAddress("passTrigger_4J3T_CDEF", &passTrigger_4J3T_CDEF, &b_passTrigger_4J3T_CDEF);

    // 2. Objects (Counts)
    fChain->SetBranchAddress("nMuons", &nMuons, &b_nMuons);
    fChain->SetBranchAddress("nElecs", &nElecs, &b_nElecs);
    fChain->SetBranchAddress("nJets", &nJets, &b_nJets);
    fChain->SetBranchAddress("nbJets", &nbJets, &b_nbJets);

    // 3. Kinematics
    fChain->SetBranchAddress("HT", &HT, &b_HT);
    fChain->SetBranchAddress("jetPt", &jetPt, &b_jetPt);
    fChain->SetBranchAddress("jetEta", &jetEta, &b_jetEta);

    // 4. Weights & Flags
    fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
    fChain->SetBranchAddress("PUWeight", &PUWeight, &b_PUWeight);
    fChain->SetBranchAddress("L1PrefiringWeight", &L1PrefiringWeight, &b_L1PrefiringWeight);
    fChain->SetBranchAddress("failGoldenJson", &failGoldenJson, &b_failGoldenJson);
    fChain->SetBranchAddress("passMETFilters", &passMETFilters, &b_passMETFilters);
}

NtupleReader::~NtupleReader() {
    // Destructor
    // Note: 'fChain' is typically owned by the TFile in the main loop.
    // We usually do not delete fChain here to avoid double-free errors.
    // ROOT automatically handles the cleanup of vector pointers (jetPt, jetEta) 
    // associated with the branch when the tree is destructed.
}

Int_t NtupleReader::GetEntry(Long64_t entry) {
    // Load the data for the specified entry index.
    if (!fChain) return 0;
    return fChain->GetEntry(entry);
}
