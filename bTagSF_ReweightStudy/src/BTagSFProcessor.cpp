#define BTagSFProcessor_cxx
#include "BTagSFProcessor.hh"
#include "Config.hh"
#include "Config_TtCatGroup.hh"

#include <TH2.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

// ============================================================================
// Helper: Fatal invariant violation
// ============================================================================
static inline void FATAL_INVARIANT(const char* msg, Long64_t entry) {
    std::cerr << "\n[FATAL][InvariantViolation] entry=" << entry
              << " : " << msg << std::endl;
    std::cerr << "  -> This event should not exist after skimming. Exiting.\n";
    std::exit(1);
}

// ============================================================================
// Constructor / Destructor
// ============================================================================
BTagSFProcessor::BTagSFProcessor() {}

BTagSFProcessor::~BTagSFProcessor()
{
    delete reader;   reader    = nullptr;
    if (inputFile) { inputFile->Close(); delete inputFile; inputFile = nullptr; }
}

// ============================================================================
// File naming
// ============================================================================
void    BTagSFProcessor::setNtupleName(TString name) { ntupleName = name; }
TString BTagSFProcessor::getInputName()  const { return ntupleName + ".root"; }
TString BTagSFProcessor::getOutputName() const {
    return TString(Config::btagOutPrefix) + ntupleName + ".root";
}

// ============================================================================
// Resolve b-tag JSON path (CVMFS first, then local fallback)
// ============================================================================
std::string BTagSFProcessor::resolveBTagJSONPath()
{
    for (const auto& path : Config::btagJSONPaths) {
        if (fs::exists(path)) return path;
    }
    std::cerr << "[BTagSFProcessor][FATAL] No valid b-tag JSON found. Tried:\n";
    for (const auto& p : Config::btagJSONPaths)
        std::cerr << "    " << p << "\n";
    std::exit(1);
    return "";
}

// ============================================================================
// CurrentEventProcessKey
// ----------------------------------------------------------------------------
// Returns the process *group* key for the current event (ttH AN App. A.2.1).
//   - inclusive ttbar: dispatched by genTtbarId (tt+LF / tt+cc / tt+B)
//   - other samples: fixed mapping by sample name (Config_TtCatGroup.hh)
// ============================================================================
std::string BTagSFProcessor::CurrentEventProcessKey(const std::string& sampleName) const
{
    return TtCatGroup::MakeProcessKey(sampleName, reader->GetGenTtbarId());
}

// ============================================================================
// Resolve flavor-aware systematic name (BTV recommendation)
//   c-jet (flavor=4) → only cferr1/2 (otherwise → "central")
//   b/light (flavor=5/0) → everything except cferr (otherwise → "central")
// ============================================================================
std::string BTagSFProcessor::resolveJetSystematic(
    int hadronFlavor, const std::string& eventSystematic)
{
    const bool isCFerr =
        (eventSystematic.find("cferr") != std::string::npos);

    if (hadronFlavor == 4) {
        return isCFerr ? eventSystematic : "central";
    } else {
        return isCFerr ? "central" : eventSystematic;
    }
}

// ============================================================================
// Evaluate single-jet b-tag SF via correctionlib
// ============================================================================
double BTagSFProcessor::evaluateJetBTagSF(
    const std::string& systematic,
    int    hadronFlavor,
    double abseta,
    double pt,
    double discriminant) const
{
    std::vector<correction::Variable::Type> args(btagSlots.totalInputs);

    args[btagSlots.systematic]   = systematic;
    args[btagSlots.flavor]       = hadronFlavor;
    args[btagSlots.abseta]       = abseta;
    args[btagSlots.pt]           = pt;
    args[btagSlots.discriminant] = discriminant;
    if (btagSlots.workingPoint >= 0)
        args[btagSlots.workingPoint] = std::string("shape");

    return btagShapeSFProvider->evaluate(args);
}

// ============================================================================
// Per-event b-tag weight = ∏ SF(jet_i) over all jets
// (flavor-aware systematic per jet)
// ============================================================================
double BTagSFProcessor::computeBTagEventWeight(
    const std::string& eventSystematic) const
{
    double w = 1.0;
    const auto& pt   = reader->GetJetPt();
    const auto& eta  = reader->GetJetEta();
    const auto& disc = reader->GetBTagScore();
    const auto& had  = reader->GetHadronFlavor();

    const size_t nJets = pt.size();
    for (size_t i = 0; i < nJets; ++i) {
        const std::string jetSyst = resolveJetSystematic(had[i], eventSystematic);
        const double sf = evaluateJetBTagSF(
            jetSyst, had[i], std::fabs(eta[i]), pt[i], disc[i]);
        w *= sf;
    }
    return w;
}

// ============================================================================
// Hadronic trigger OR with PD-exclusivity for Data
//   BTagCSV → keep 4J3T  group only
//   JetHT   → keep (6J1T||6J2T||PFHT1050) and reject 4J3T (avoid double count)
//   MC      → OR of everything
// (era=B uses *_B branches; CDEF uses *_CDEF)
// ============================================================================
bool BTagSFProcessor::passHadronicTrigger(
    const TString& dataSet, const TString& era) const
{
    const bool isEraB = (era == "B");
    const bool fired4J3T = isEraB ? reader->GetPassTrigger_4J3T_B()
                                  : reader->GetPassTrigger_4J3T_CDEF();
    const bool fired6J1T = isEraB ? reader->GetPassTrigger_6J1T_B()
                                  : reader->GetPassTrigger_6J1T_CDEF();
    const bool fired6J2T = isEraB ? reader->GetPassTrigger_6J2T_B()
                                  : reader->GetPassTrigger_6J2T_CDEF();
    const bool firedHT   = reader->GetPassTrigger_PFHT1050();

    const bool group_BTagCSV = fired4J3T;
    const bool group_JetHT   = (fired6J1T || fired6J2T || firedHT);

    if (dataSet == "default") {
        // MC
        return group_BTagCSV || group_JetHT;
    }
    if (dataSet.Contains("BTagCSV")) {
        return group_BTagCSV;
    }
    if (dataSet.Contains("JetHT")) {
        return group_JetHT && !group_BTagCSV;
    }
    if (dataSet.Contains("SingleMuon")) {
        return group_BTagCSV || group_JetHT;
    }
    std::cerr << "[BTagSFProcessor][WARN] Unknown dataSet: " << dataSet << "\n";
    return false;
}

// ============================================================================
// Evaluate trigger SF via correctionlib (FATAL on failure)
// ============================================================================
double BTagSFProcessor::evaluateTriggerSF(
    int nBJets, double jet6Eta, double HT, double jet6PT) const
{
    if (!trigSFProvider) {
        std::cerr << "[BTagSFProcessor][FATAL] trigSFProvider not loaded\n";
        std::exit(1);
    }

    try {
        return trigSFProvider->evaluate({
            static_cast<int>(nBJets),
            static_cast<double>(jet6Eta),
            static_cast<double>(HT),
            static_cast<double>(jet6PT)
        });
    } catch (const std::exception& e) {
        std::cerr << "[BTagSFProcessor][FATAL] Trigger SF evaluate failed: "
                  << e.what()
                  << "  (nB=" << nBJets << ", eta=" << jet6Eta
                  << ", HT=" << HT << ", pt=" << jet6PT << ")\n";
        std::exit(1);
    }
    return 1.0;
}

// ============================================================================
// Init: open file, build NtupleReader, load correctionlib JSONs
// ============================================================================
void BTagSFProcessor::Init()
{
    // ── Open input file & tree ──
    TString ntuplePath = Config::inputBaseDir + getInputName();

    inputFile = TFile::Open(ntuplePath);
    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "[BTagSFProcessor][FATAL] Cannot open " << ntuplePath << "\n";
        std::exit(1);
    }

    inputFile->GetObject(Config::treePath.c_str(), fChain);
    if (!fChain) {
        std::cerr << "[BTagSFProcessor][FATAL] Cannot get TTree: "
                  << Config::treePath << "\n";
        std::exit(1);
    }

    reader = new NtupleReader(fChain);

    // ── Load correctionlib: b-tag shape SF ──
    std::string btagJSONPath = resolveBTagJSONPath();
    std::cout << "  [BTagSFProcessor] b-tag JSON: " << btagJSONPath << "\n";

    btagCorrectionSet   = correction::CorrectionSet::from_file(btagJSONPath);
    btagShapeSFProvider = btagCorrectionSet->at(Config::btagCorrectionName);

    // Build input-slot mapping by inspecting JSON schema
    const auto& inputs = btagShapeSFProvider->inputs();
    btagSlots.totalInputs = static_cast<int>(inputs.size());

    std::cout << "  [BTagSFProcessor] " << Config::btagCorrectionName
              << " inputs (" << btagSlots.totalInputs << "): ";
    for (int i = 0; i < btagSlots.totalInputs; ++i) {
        const std::string& name = inputs[i].name();
        std::cout << name << " ";

        if      (name == "systematic")    btagSlots.systematic   = i;
        else if (name == "working_point") btagSlots.workingPoint = i;
        else if (name == "flavor")        btagSlots.flavor       = i;
        else if (name == "abseta")        btagSlots.abseta       = i;
        else if (name == "pt")            btagSlots.pt           = i;
        else if (name == "discriminant")  btagSlots.discriminant = i;
    }
    std::cout << "\n";

    if (btagSlots.systematic < 0 || btagSlots.flavor < 0 ||
        btagSlots.abseta < 0 || btagSlots.pt < 0 || btagSlots.discriminant < 0) {
        std::cerr << "[BTagSFProcessor][FATAL] Missing required input field in "
                  << Config::btagCorrectionName << "\n";
        std::exit(1);
    }

    // ── Load correctionlib: trigger SF ──
    std::cout << "  [BTagSFProcessor] trigger SF JSON: " << Config::sfOutputJSON << "\n";

    if (!fs::exists(Config::sfOutputJSON)) {
        std::cerr << "[BTagSFProcessor][FATAL] Trigger SF JSON not found: "
                  << Config::sfOutputJSON << "\n";
        std::exit(1);
    }

    trigCorrectionSet = correction::CorrectionSet::from_file(Config::sfOutputJSON);
    trigSFProvider    = trigCorrectionSet->at("triggerSF");

    std::cout << "  [BTagSFProcessor] Init complete.\n\n";
}

// ============================================================================
// Loop (Two-Pass)
// ----------------------------------------------------------------------------
// Pass 1: For each event, dispatch to one of the per-event process group keys
//         (ttH AN App. A.2.1). Accumulate per-(group, syst, bin) sumNoSF and
//         sumWithSF. Output as TH1D/TH2D under "NormSums/" so makeReweightJSON
//         can sum across samples and compute the final per-group ratio.
//
// Pass 2: Validation 3-tier histograms (noSF / withSF / reweighted). Since the
//         final per-group ratio is computed only after group-sum (in
//         makeReweightJSON), Pass 2 here keeps reweighted = withSF as a
//         placeholder. True closure is verified downstream after JSON load.
// ============================================================================
void BTagSFProcessor::Loop()
{
    if (!fChain) return;

    // ====================================================================
    // Phase 1: Configuration & Sample ID
    // ====================================================================
    Config::Dump();

    TString sampleName = ntupleName;
    std::cout << "Sample Name: " << sampleName << "\n";

    const auto* sampleInfo = Config::GetSampleInfo(sampleName.Data());
    if (!sampleInfo) {
        std::cerr << "[BTagSFProcessor][FATAL] Unknown sample: " << sampleName
                  << "\n  Register it in Config::SampleRegistry()\n";
        std::exit(1);
    }

    isData = sampleInfo->isData;
    const double xsecWeight = sampleInfo->weight;
    std::cout << "  Type   : " << (isData ? "Data" : "MC") << "\n";
    std::cout << "  Weight : " << xsecWeight << "\n";

    // Parse DataSet / Era for Data
    TString dataSet = "default";
    TString era     = "default";
    if (isData) {
        Ssiz_t pos = sampleName.Index("_");
        if (pos == kNPOS) {
            std::cerr << "[BTagSFProcessor][FATAL] Data name must be <DataSet>_<Era>\n";
            std::exit(1);
        }
        dataSet = sampleName(0, pos);
        era     = sampleName(pos + 1, sampleName.Length() - pos - 1);
        std::cout << "  DataSet: " << dataSet << ", Era: " << era << "\n";
    }

    const Long64_t nEntries = fChain->GetEntries();
    std::cout << "\n>>> Total entries: " << nEntries << "\n\n";

    const int maxNJetsBin   = Config::btagMaxNJetsBin;
    const int totalBins     = Config::getTotalReweightBins();
    const int nHTBins       = Config::getNumHTBins();
    const auto& systematics = Config::btagSystematics;

    std::cout << "  Reweight mode  : "
              << (Config::useHTForReweight ? "2D (nJets × HT)" : "1D (nJets)")
              << "\n";
    std::cout << "  Total flat bins: " << totalBins << "\n";
    if (Config::useHTForReweight)
        std::cout << "  nHTBins        : " << nHTBins << "\n";
    std::cout << "\n";

    // ====================================================================
    // Phase 2: Pass 1 — Per-process accumulators of (Σω_noSF, Σω_withSF)
    //
    // Storage: sumNoSF[pkey][syst][flatBin]
    // Group sum across samples is done later in makeReweightJSON.
    // ====================================================================
    const std::vector<std::string> processKeys =
        TtCatGroup::ProcessKeysForSample(sampleName.Data());

    std::map<std::string, std::map<std::string, std::vector<double>>> sumNoSF;
    std::map<std::string, std::map<std::string, std::vector<double>>> sumWithSF;

    for (const auto& pkey : processKeys) {
        for (const auto& syst : systematics) {
            sumNoSF  [pkey][syst].assign(totalBins, 0.0);
            sumWithSF[pkey][syst].assign(totalBins, 0.0);
        }
    }

    if (!isData) {
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║  Pass 1: Accumulating per-process (Σω_noSF, Σω_withSF)       ║\n";
        std::cout << "║  Process groups for this sample: ";
        for (size_t i = 0; i < processKeys.size(); ++i) {
            std::cout << processKeys[i];
            if (i + 1 < processKeys.size()) std::cout << ", ";
        }
        std::cout << "\n╚══════════════════════════════════════════════════════════════╝\n";

        for (Long64_t j = 0; j < nEntries; ++j) {
            reader->GetEntry(j);

            if (j % Config::progressInterval == 0) {
                std::cout << "    [Pass 1] " << j << " / " << nEntries
                          << " (" << std::fixed << std::setprecision(1)
                          << 100.0 * j / nEntries << "%)\n";
            }

            // Skim invariants (NO b-tag selection — BTV requirement)
            if (reader->GetNJets() < Config::minNJets)
                FATAL_INVARIANT("NJets < 6", j);
            if (reader->GetJetPt().at(5) <= 40.0)
                FATAL_INVARIANT("6th jet pT <= 40", j);
            if (reader->GetHT() < 500.0)
                FATAL_INVARIANT("HT < 500", j);
            if (!reader->GetPassMETFilters())
                FATAL_INVARIANT("MET filters", j);

            if (!passHadronicTrigger(dataSet, era)) continue;

            // Per-event process key dispatch
            const std::string pkey = CurrentEventProcessKey(sampleName.Data());

            // Defensive (genTtbarId edge case)
            if (sumNoSF.find(pkey) == sumNoSF.end()) {
                for (const auto& syst : systematics) {
                    sumNoSF  [pkey][syst].assign(totalBins, 0.0);
                    sumWithSF[pkey][syst].assign(totalBins, 0.0);
                }
            }

            const double baseWeight =
                reader->GetGenWeight()
                * reader->GetPUWeight()
                * reader->GetPrefireWeight()
                * xsecWeight;

            const double currentHT = reader->GetHT();
            const double jet6PT    = reader->GetJetPt().at(5);
            const double jet6Eta   = reader->GetJetEta().at(5);
            const int    nBJets    = reader->GetNBJets();

            const double trigSF = evaluateTriggerSF(nBJets, jet6Eta, currentHT, jet6PT);
            const double weightNoSFVal = baseWeight * trigSF;

            const int flatBin = Config::getReweightFlatIndex(
                reader->GetNJets(), currentHT);

            for (const auto& syst : systematics) {
                double btagEvtW = 1.0;
                try {
                    btagEvtW = computeBTagEventWeight(syst);
                } catch (const std::exception& e) {
                    if (j < 5) {
                        std::cerr << "[WARN] btag SF failed for syst=" << syst
                                  << " entry=" << j << ": " << e.what() << "\n";
                    }
                    btagEvtW = 1.0;
                }
                sumNoSF  [pkey][syst][flatBin] += weightNoSFVal;
                sumWithSF[pkey][syst][flatBin] += weightNoSFVal * btagEvtW;
            }
        }

        // Print central-only summary per process key
        std::cout << "\n  Per-process accumulators (central):\n";
        for (const auto& pkey : processKeys) {
            if (sumNoSF.find(pkey) == sumNoSF.end()) continue;
            std::cout << "  ── " << pkey << " ──\n";
            std::cout << "  bin  | sumNoSF        | sumWithSF      | local r (preview)\n";
            std::cout << "  -----+----------------+----------------+---------\n";
            for (int b = 0; b < totalBins; ++b) {
                double sN = sumNoSF[pkey]["central"][b];
                double sW = sumWithSF[pkey]["central"][b];
                if (sN == 0.0 && sW == 0.0) continue;
                double r = (sW > 0.0) ? sN / sW : 1.0;
                std::cout << "  " << std::setw(4) << b
                          << " | " << std::setw(14) << std::setprecision(4) << sN
                          << " | " << std::setw(14) << sW
                          << " | " << std::setprecision(6) << r << "\n";
            }
        }
        std::cout << "\n";
    }

    // ====================================================================
    // Phase 3: Create Output & Save Per-Process Sums
    //
    // For each (pkey, syst): write sumNoSF and sumWithSF as separate hists
    // under "NormSums/". makeReweightJSON.cpp will read these and compute
    // group-summed ratios.
    //
    // Naming:
    //   2D: h_sumNoSF_nJets_HT_<pkey_sanitized>_<syst>
    //       h_sumWithSF_nJets_HT_<pkey_sanitized>_<syst>
    //   1D: h_sumNoSF_nJets_<pkey_sanitized>_<syst>
    //       h_sumWithSF_nJets_<pkey_sanitized>_<syst>
    // ====================================================================
    TFile* outputFile = new TFile(getOutputName(), "RECREATE");
    outputFile->cd();

    if (!isData) {
        outputFile->mkdir("NormSums");
        outputFile->cd("NormSums");

        if (Config::useHTForReweight) {
            std::vector<double> nJetsEdges;
            for (int i = 0; i <= maxNJetsBin + 1; ++i)
                nJetsEdges.push_back(static_cast<double>(i) - 0.5);

            const auto& htEdges = Config::reweightHT_edges;

            for (const auto& pkey : processKeys) {
                if (sumNoSF.find(pkey) == sumNoSF.end()) continue;
                const std::string skey = TtCatGroup::SanitizeKey(pkey);

                for (const auto& syst : systematics) {
                    TString hNameNo = "h_sumNoSF_nJets_HT_"
                                    + TString(skey.c_str()) + "_"
                                    + TString(syst.c_str());
                    auto* hNo = new TH2D(hNameNo,
                        hNameNo + ";nJets;HT [GeV];Sum w (noSF)",
                        maxNJetsBin + 1, nJetsEdges.data(),
                        nHTBins, htEdges.data());

                    TString hNameWi = "h_sumWithSF_nJets_HT_"
                                    + TString(skey.c_str()) + "_"
                                    + TString(syst.c_str());
                    auto* hWi = new TH2D(hNameWi,
                        hNameWi + ";nJets;HT [GeV];Sum w (withSF)",
                        maxNJetsBin + 1, nJetsEdges.data(),
                        nHTBins, htEdges.data());

                    for (int nj = 0; nj <= maxNJetsBin; ++nj) {
                        for (int ht = 0; ht < nHTBins; ++ht) {
                            int flatIdx = nj * nHTBins + ht;
                            hNo->SetBinContent(nj + 1, ht + 1,
                                               sumNoSF[pkey][syst][flatIdx]);
                            hWi->SetBinContent(nj + 1, ht + 1,
                                               sumWithSF[pkey][syst][flatIdx]);
                        }
                    }
                    hNo->Write(); hWi->Write();
                }
            }
        } else {
            for (const auto& pkey : processKeys) {
                if (sumNoSF.find(pkey) == sumNoSF.end()) continue;
                const std::string skey = TtCatGroup::SanitizeKey(pkey);

                for (const auto& syst : systematics) {
                    TString hNameNo = "h_sumNoSF_nJets_"
                                    + TString(skey.c_str()) + "_"
                                    + TString(syst.c_str());
                    auto* hNo = new TH1D(hNameNo, hNameNo + ";nJets;Sum w (noSF)",
                                         maxNJetsBin + 1, -0.5, maxNJetsBin + 0.5);

                    TString hNameWi = "h_sumWithSF_nJets_"
                                    + TString(skey.c_str()) + "_"
                                    + TString(syst.c_str());
                    auto* hWi = new TH1D(hNameWi, hNameWi + ";nJets;Sum w (withSF)",
                                         maxNJetsBin + 1, -0.5, maxNJetsBin + 0.5);

                    for (int b = 0; b <= maxNJetsBin; ++b) {
                        hNo->SetBinContent(b + 1, sumNoSF[pkey][syst][b]);
                        hWi->SetBinContent(b + 1, sumWithSF[pkey][syst][b]);
                    }
                    hNo->Write(); hWi->Write();
                }
            }
        }
        outputFile->cd();
    }

    // ── Validation histograms (using Config definitions) ──
    const int maxJets = Config::btagMaxJetsForPerJetHist;

    auto* h_nJets_noSF       = new TH1D("h_nJets_noSF",      ";nJets;Events",
        Config::histNJets_nBins, Config::histNJets_Low, Config::histNJets_High);
    auto* h_nJets_withSF     = new TH1D("h_nJets_withSF",    ";nJets;Events",
        Config::histNJets_nBins, Config::histNJets_Low, Config::histNJets_High);
    auto* h_nJets_reweighted = new TH1D("h_nJets_reweighted", ";nJets;Events",
        Config::histNJets_nBins, Config::histNJets_Low, Config::histNJets_High);

    auto* h_HT_noSF       = new TH1D("h_HT_noSF",      ";HT [GeV];Events",
        Config::histHT_nBins, Config::histHT_Low, Config::histHT_High);
    auto* h_HT_withSF     = new TH1D("h_HT_withSF",    ";HT [GeV];Events",
        Config::histHT_nBins, Config::histHT_Low, Config::histHT_High);
    auto* h_HT_reweighted = new TH1D("h_HT_reweighted", ";HT [GeV];Events",
        Config::histHT_nBins, Config::histHT_Low, Config::histHT_High);

    auto* h_nbJets_noSF       = new TH1D("h_nbJets_noSF",      ";n_{b-jets};Events",
        Config::histNBJets_nBins, Config::histNBJets_Low, Config::histNBJets_High);
    auto* h_nbJets_withSF     = new TH1D("h_nbJets_withSF",    ";n_{b-jets};Events",
        Config::histNBJets_nBins, Config::histNBJets_Low, Config::histNBJets_High);
    auto* h_nbJets_reweighted = new TH1D("h_nbJets_reweighted", ";n_{b-jets};Events",
        Config::histNBJets_nBins, Config::histNBJets_Low, Config::histNBJets_High);

    std::vector<TH1D*> h_jetPt_noSF(maxJets), h_jetPt_withSF(maxJets), h_jetPt_reweighted(maxJets);
    std::vector<TH1D*> h_bTag_noSF(maxJets),  h_bTag_withSF(maxJets),  h_bTag_reweighted(maxJets);

    for (int j = 0; j < maxJets; ++j) {
        h_jetPt_noSF[j]       = new TH1D(Form("h_jetPt_noSF_jet%d", j),
            Form("Jet %d p_{T} (no SF);p_{T} [GeV];Events", j),
            Config::histJetPt_nBins, Config::histJetPt_Low, Config::histJetPt_High);
        h_jetPt_withSF[j]     = new TH1D(Form("h_jetPt_withSF_jet%d", j),
            Form("Jet %d p_{T} (with SF);p_{T} [GeV];Events", j),
            Config::histJetPt_nBins, Config::histJetPt_Low, Config::histJetPt_High);
        h_jetPt_reweighted[j] = new TH1D(Form("h_jetPt_reweighted_jet%d", j),
            Form("Jet %d p_{T} (reweighted);p_{T} [GeV];Events", j),
            Config::histJetPt_nBins, Config::histJetPt_Low, Config::histJetPt_High);

        h_bTag_noSF[j]       = new TH1D(Form("h_bTag_noSF_jet%d", j),
            Form("Jet %d DeepJet (no SF);discriminant;Events", j),
            Config::histBTag_nBins, Config::histBTag_Low, Config::histBTag_High);
        h_bTag_withSF[j]     = new TH1D(Form("h_bTag_withSF_jet%d", j),
            Form("Jet %d DeepJet (with SF);discriminant;Events", j),
            Config::histBTag_nBins, Config::histBTag_Low, Config::histBTag_High);
        h_bTag_reweighted[j] = new TH1D(Form("h_bTag_reweighted_jet%d", j),
            Form("Jet %d DeepJet (reweighted);discriminant;Events", j),
            Config::histBTag_nBins, Config::histBTag_Low, Config::histBTag_High);
    }

    // ====================================================================
    // Phase 4: Pass 2 — Validation hist fill (noSF / withSF / reweighted)
    //
    // Note: per-group ratio is finalized in makeReweightJSON.cpp after
    //       cross-sample sum, so here `reweighted` uses a placeholder
    //       (locally computed sample-only ratio). True closure plot must
    //       be made downstream after JSON load (or use plot_btag.py with
    //       the JSON loaded for proper reweight).
    // ====================================================================
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  Pass 2: Filling validation histograms (3-tier)              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";

    long long nProcessed = 0, nPassed = 0;

    for (Long64_t j = 0; j < nEntries; ++j) {
        reader->GetEntry(j);

        if (j % Config::progressInterval == 0) {
            std::cout << "    [Pass 2] " << j << " / " << nEntries
                      << " (" << std::fixed << std::setprecision(1)
                      << 100.0 * j / nEntries << "%)\n";
        }

        ++nProcessed;

        if (reader->GetNJets() < Config::minNJets)
            FATAL_INVARIANT("NJets < 6", j);
        if (reader->GetJetPt().at(5) <= 40.0)
            FATAL_INVARIANT("6th jet pT <= 40", j);
        if (reader->GetHT() < 500.0)
            FATAL_INVARIANT("HT < 500", j);
        if (!reader->GetPassMETFilters())
            FATAL_INVARIANT("MET filters", j);
        if (isData && reader->GetFailGoldenJson())
            FATAL_INVARIANT("Golden JSON failed", j);

        if (!passHadronicTrigger(dataSet, era)) continue;
        ++nPassed;

        const int    nJets     = reader->GetNJets();
        const int    nBJets    = reader->GetNBJets();
        const double currentHT = reader->GetHT();
        const double jet6PT    = reader->GetJetPt().at(5);
        const double jet6Eta   = reader->GetJetEta().at(5);

        // ── Base weight ──
        double baseWeight = 1.0;
        if (!isData) {
            baseWeight = reader->GetGenWeight()
                       * reader->GetPUWeight()
                       * reader->GetPrefireWeight()
                       * xsecWeight;
        }

        // ── Trigger SF ──
        double trigSF = 1.0;
        if (!isData) {
            trigSF = evaluateTriggerSF(nBJets, jet6Eta, currentHT, jet6PT);
        }

        // ── B-tag event weight (central) ──
        double btagEvtWeight = 1.0;
        if (!isData) {
            try {
                btagEvtWeight = computeBTagEventWeight("central");
            } catch (const std::exception& e) {
                if (j < 5)
                    std::cerr << "[WARN] btag SF entry=" << j << ": " << e.what() << "\n";
                btagEvtWeight = 1.0;
            }
        }

        // ── Sample-local ratio for in-sample closure preview ──
        // (true per-group ratio is computed in makeReweightJSON after group sum)
        double btagNormRatio = 1.0;
        if (!isData) {
            const std::string pkey = CurrentEventProcessKey(sampleName.Data());
            const int flatBin = Config::getReweightFlatIndex(nJets, currentHT);
            auto it = sumNoSF.find(pkey);
            if (it != sumNoSF.end()) {
                const double sN = it->second.at("central").at(flatBin);
                const double sW = sumWithSF[pkey]["central"][flatBin];
                btagNormRatio = (sW > 0.0) ? sN / sW : 1.0;
            }
        }

        const double wNoSF       = baseWeight * trigSF;
        const double wWithSF     = baseWeight * trigSF * btagEvtWeight;
        const double wReweighted = baseWeight * trigSF * btagEvtWeight * btagNormRatio;

        h_nJets_noSF->Fill(nJets, wNoSF);
        h_nJets_withSF->Fill(nJets, wWithSF);
        h_nJets_reweighted->Fill(nJets, wReweighted);

        h_HT_noSF->Fill(currentHT, wNoSF);
        h_HT_withSF->Fill(currentHT, wWithSF);
        h_HT_reweighted->Fill(currentHT, wReweighted);

        h_nbJets_noSF->Fill(nBJets, wNoSF);
        h_nbJets_withSF->Fill(nBJets, wWithSF);
        h_nbJets_reweighted->Fill(nBJets, wReweighted);

        const int nToFill = std::min(nJets, maxJets);
        const auto& ptVec   = reader->GetJetPt();
        const auto& btagVec = reader->GetBTagScore();

        for (int jj = 0; jj < nToFill; ++jj) {
            h_jetPt_noSF[jj]->Fill(ptVec.at(jj), wNoSF);
            h_jetPt_withSF[jj]->Fill(ptVec.at(jj), wWithSF);
            h_jetPt_reweighted[jj]->Fill(ptVec.at(jj), wReweighted);

            h_bTag_noSF[jj]->Fill(btagVec.at(jj), wNoSF);
            h_bTag_withSF[jj]->Fill(btagVec.at(jj), wWithSF);
            h_bTag_reweighted[jj]->Fill(btagVec.at(jj), wReweighted);
        }
    }

    // ====================================================================
    // Phase 5: Summary & Write
    // ====================================================================
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  B-Tag Reweight Summary                                      ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    std::cout << "  Events processed : " << nProcessed << "\n";
    std::cout << "  Events passed    : " << nPassed << "\n";

    if (!isData) {
        double totNoSF = h_nJets_noSF->Integral();
        double totReW  = h_nJets_reweighted->Integral();
        double dev = (totNoSF > 0) ? std::abs(totReW - totNoSF) / totNoSF * 100.0 : 0.0;

        std::cout << std::fixed << std::setprecision(4);
        std::cout << "  Σ weight (noSF)      : " << totNoSF << "\n";
        std::cout << "  Σ weight (withSF)    : " << h_nJets_withSF->Integral() << "\n";
        std::cout << "  Σ weight (reweighted): " << totReW << "\n";
        std::cout << "  Deviation (in-sample): " << std::setprecision(4) << dev << " %"
                  << (dev > 1.0 ? "  [WARNING]" : "  [OK]") << "\n";
        std::cout << "  Note: this is a sample-local closure preview. The true\n"
                     "  per-group closure is verified after group-sum in makeReweightJSON\n"
                     "  and JSON-based application downstream.\n";
    }
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";

    // Write
    std::cout << ">>> Writing output to: " << getOutputName() << "\n";
    outputFile->cd();

    h_nJets_noSF->Write();    h_nJets_withSF->Write();    h_nJets_reweighted->Write();
    h_HT_noSF->Write();       h_HT_withSF->Write();       h_HT_reweighted->Write();
    h_nbJets_noSF->Write();   h_nbJets_withSF->Write();   h_nbJets_reweighted->Write();

    outputFile->mkdir("PerJet");
    outputFile->cd("PerJet");
    for (int jj = 0; jj < maxJets; ++jj) {
        h_jetPt_noSF[jj]->Write();    h_jetPt_withSF[jj]->Write();    h_jetPt_reweighted[jj]->Write();
        h_bTag_noSF[jj]->Write();     h_bTag_withSF[jj]->Write();     h_bTag_reweighted[jj]->Write();
    }

    outputFile->cd();
    std::cout << ">>> Done.\n";
    delete outputFile;
}
