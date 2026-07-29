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
 *
 * // Example: B-tagging info
 * for (int j = 0; j < reader.GetNJets(); ++j) {
 *     double disc = reader.GetBTagScore().at(j);
 *     int    flav = reader.GetHadronFlavor().at(j);
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
    bTagScore = 0;
    hadFlavs  = 0;

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
    // [2026-07-29] main 의 lepton veto 정의. 구 skim 에는 없으므로 조건부.
    if (fChain->GetBranch("nVetoLeptons")) {
        fChain->SetBranchAddress("nVetoLeptons", &nVetoLeptons, &b_nVetoLeptons);
    }
    fChain->SetBranchAddress("nJets", &nJets, &b_nJets);
    fChain->SetBranchAddress("nbJets", &nbJets, &b_nbJets);

    // 3. Kinematics
    fChain->SetBranchAddress("HT", &HT, &b_HT);
    fChain->SetBranchAddress("jetPt", &jetPt, &b_jetPt);
    fChain->SetBranchAddress("jetEta", &jetEta, &b_jetEta);

    // 4. B-tagging
    fChain->SetBranchAddress("bTagScore", &bTagScore, &b_bTagScore);
    fChain->SetBranchAddress("hadFlavs",  &hadFlavs,  &b_hadFlavs);

    // 5. Weights & Flags
    fChain->SetBranchAddress("evtWeight", &evtWeight, &b_evtWeight);
    fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
    fChain->SetBranchAddress("PUWeight", &PUWeight, &b_PUWeight);
    fChain->SetBranchAddress("L1PrefiringWeight", &L1PrefiringWeight, &b_L1PrefiringWeight);
    fChain->SetBranchAddress("failGoldenJson", &failGoldenJson, &b_failGoldenJson);
    fChain->SetBranchAddress("passMETFilters", &passMETFilters, &b_passMETFilters);
    fChain->SetBranchAddress("passHadTrig", &passHadTrig, &b_passHadTrig);

    // ttbar categorization (only present in MC ntuples — Data ntuples skip silently).
    // If the branch doesn't exist, ROOT will print a warning and genTtbarId
    // stays at its initial value (-1, meaning "not ttbar"). That's the desired
    // behaviour: non-ttbar samples and Data both pass IsInclusiveTtbar()=false
    // downstream and use sample name as process key.
    if (fChain->GetBranch("genTtbarId")) {
        fChain->SetBranchAddress("genTtbarId", &genTtbarId, &b_genTtbarId);
    }

    // [2026-07-29] tt+nb 확장 id. MC 에만 존재.
    //   존재 여부를 Has...() 로 노출해, 부재 시 호출부가 조용히 genTtbarId 로
    //   되돌아가는 대신 명시적으로 판단하게 한다. (되돌아가면 tt+nb 그룹이
    //   전 bin 1.0 이 되고, 그건 정상적인 8-group JSON 처럼 보인다.)
    if (fChain->GetBranch("expandedTtbarId")) {
        fChain->SetBranchAddress("expandedTtbarId", &expandedTtbarId, &b_expandedTtbarId);
    }

    // [2026-07-29] stitching 배수. MC 에만 존재.
    //   부재 시 1.0 을 유지 = "stitching 미적용 skim". 호출부에서 ttbar family
    //   샘플이면 FATAL 로 끊는다 (그 조합은 tt+B 이중계수를 뜻하므로).
    if (fChain->GetBranch("stitchWeight")) {
        fChain->SetBranchAddress("stitchWeight", &stitchWeight, &b_stitchWeight);
    }
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
