// ============================================================================
// makeReweightJSON.cpp
//
// Aggregates per-sample (Σω_noSF, Σω_withSF) histograms — produced by
// exe_BTagSF — into per-process-group b-tag normalization ratios, and writes
// a single correctionlib-schema-v2 JSON.
//
// Method (ttH AN-19-094 §A.2.1):
//   1. Each sample contributes 1 or 3 process keys (inclusive ttbar → 3).
//   2. For each process *group* (defined in Config_TtCatGroup.hh), sum the
//      sumNoSF and sumWithSF accumulators across all samples in the group.
//   3. ratio[group, syst, bin] = Σ_samples(sumNoSF) / Σ_samples(sumWithSF).
//   4. Write 'systematic → process_group → binning → ratio' JSON.
//
// Input ROOT files (from exe_BTagSF):
//   bTagReweight_<sample>.root
//     └── NormSums/h_sumNoSF_nJets[_HT]_<pkey>_<syst>
//         NormSums/h_sumWithSF_nJets[_HT]_<pkey>_<syst>
//
// Output JSON:
//   <Config::btagReweightJSON>   (path defined in Config.hh)
//   correctionlib schema v2.
//   correction name = "btagNormReweight"
//   inputs: systematic (string), process (string), nJets (int), [HT (real)]
//   output: normalization ratio (real)
//
// Usage:
//   make exe_MakeJSON
//   ./exe_MakeJSON                     # uses all MC samples from Config
//   ./exe_MakeJSON TTbar_Hadronic ttbb   # specific samples only
//
// IMPORTANT:
//   exe_BTagSF and exe_MakeJSON MUST be compiled with the same
//   Config::useHTForReweight setting. Histogram names differ by 1D/2D.
//
// Author: Junghyun Lee
// ============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <map>

#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TString.h>

#include "Config.hh"
#include "Config_TtCatGroup.hh"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

int main(int argc, char** argv)
{
    // ────────────────────────────────────────────────────────────────────
    // Determine sample list
    // ────────────────────────────────────────────────────────────────────
    std::vector<std::string> mcSamples;
    if (argc > 1) {
        for (int i = 1; i < argc; ++i)
            mcSamples.emplace_back(argv[i]);
    } else {
        mcSamples = Config::GetMCSampleNames();
    }

    const auto& systematics = Config::btagSystematics;
    const int   maxBin      = Config::btagMaxNJetsBin;
    const int   nHTBins     = Config::getNumHTBins();
    const bool  use2D       = Config::useHTForReweight;
    const int   totalBins   = use2D ? (maxBin + 1) * nHTBins : (maxBin + 1);

    const auto allGroups = TtCatGroup::AllProcessGroupKeys();

    // ────────────────────────────────────────────────────────────────────
    // Banner
    // ────────────────────────────────────────────────────────────────────
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  makeReweightJSON: per-process-group ratio aggregator        ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    std::cout << "  Input samples : " << mcSamples.size() << "\n";
    std::cout << "  Systematics   : " << systematics.size() << "\n";
    std::cout << "  Reweight mode : " << (use2D ? "2D (nJets × HT)" : "1D (nJets)") << "\n";
    std::cout << "  nJets bins    : 0 to " << maxBin << "\n";
    if (use2D) {
        std::cout << "  HT bins       : " << nHTBins << "  edges=[";
        for (size_t i = 0; i < Config::reweightHT_edges.size(); ++i) {
            std::cout << Config::reweightHT_edges[i];
            if (i < Config::reweightHT_edges.size() - 1) std::cout << ",";
        }
        std::cout << "]\n";
    }
    std::cout << "  Process groups: " << allGroups.size() << "  (";
    for (size_t i = 0; i < allGroups.size(); ++i) {
        std::cout << allGroups[i];
        if (i + 1 < allGroups.size()) std::cout << ", ";
    }
    std::cout << ")\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";

    // ────────────────────────────────────────────────────────────────────
    // Build edges (correctionlib requires double for Binning/MultiBinning)
    // ────────────────────────────────────────────────────────────────────
    json nJetsEdges = json::array();
    for (int b = 0; b <= maxBin + 1; ++b)
        nJetsEdges.push_back(static_cast<double>(b));

    json htEdgesJSON = json::array();
    if (use2D) {
        for (double e : Config::reweightHT_edges) htEdgesJSON.push_back(e);
    }

    // ────────────────────────────────────────────────────────────────────
    // Phase 1: Group sums across samples
    //
    //   For each (group, syst, bin) accumulate sumNoSF and sumWithSF
    //   from every sample whose process key resolves to that group.
    // ────────────────────────────────────────────────────────────────────
    using BinVec = std::vector<double>;
    std::map<std::string, std::map<std::string, BinVec>> groupSumNoSF;
    std::map<std::string, std::map<std::string, BinVec>> groupSumWithSF;

    for (const auto& g : allGroups) {
        for (const auto& s : systematics) {
            groupSumNoSF  [g][s].assign(totalBins, 0.0);
            groupSumWithSF[g][s].assign(totalBins, 0.0);
        }
    }

    int nReadOK = 0, nReadFail = 0;
    std::map<std::string, std::vector<std::string>> groupSources;  // for reporting

    for (const auto& sample : mcSamples) {
        TString fname = TString(Config::btagOutPrefix) + sample.c_str() + ".root";
        TFile* f = TFile::Open(fname, "READ");
        if (!f || f->IsZombie()) {
            std::cerr << "  [WARN] Cannot open " << fname << ", skipping\n";
            if (f) { f->Close(); delete f; }
            ++nReadFail;
            continue;
        }

        // Process keys this sample contributes to
        const auto pkeys = TtCatGroup::ProcessKeysForSample(sample);

        bool sampleHasAnyHist = false;

        for (const auto& pkey : pkeys) {
            const std::string skey = TtCatGroup::SanitizeKey(pkey);
            bool foundForThisPkey = false;

            for (const auto& syst : systematics) {
                TString hNameNo, hNameWi;
                if (use2D) {
                    hNameNo = "NormSums/h_sumNoSF_nJets_HT_"
                            + TString(skey.c_str()) + "_" + TString(syst.c_str());
                    hNameWi = "NormSums/h_sumWithSF_nJets_HT_"
                            + TString(skey.c_str()) + "_" + TString(syst.c_str());
                } else {
                    hNameNo = "NormSums/h_sumNoSF_nJets_"
                            + TString(skey.c_str()) + "_" + TString(syst.c_str());
                    hNameWi = "NormSums/h_sumWithSF_nJets_"
                            + TString(skey.c_str()) + "_" + TString(syst.c_str());
                }

                if (use2D) {
                    auto* hNo = dynamic_cast<TH2D*>(f->Get(hNameNo));
                    auto* hWi = dynamic_cast<TH2D*>(f->Get(hNameWi));
                    if (!hNo || !hWi) {
                        if (syst == "central") {
                            std::cerr << "  [WARN] " << sample
                                      << " missing " << hNameNo
                                      << " or " << hNameWi << "\n";
                        }
                        continue;
                    }
                    for (int nj = 0; nj <= maxBin; ++nj) {
                        for (int ht = 0; ht < nHTBins; ++ht) {
                            const int idx = nj * nHTBins + ht;
                            groupSumNoSF  [pkey][syst][idx] += hNo->GetBinContent(nj+1, ht+1);
                            groupSumWithSF[pkey][syst][idx] += hWi->GetBinContent(nj+1, ht+1);
                        }
                    }
                    foundForThisPkey = true;
                    sampleHasAnyHist = true;
                } else {
                    auto* hNo = dynamic_cast<TH1D*>(f->Get(hNameNo));
                    auto* hWi = dynamic_cast<TH1D*>(f->Get(hNameWi));
                    if (!hNo || !hWi) {
                        if (syst == "central") {
                            std::cerr << "  [WARN] " << sample
                                      << " missing " << hNameNo
                                      << " or " << hNameWi << "\n";
                        }
                        continue;
                    }
                    for (int b = 0; b <= maxBin; ++b) {
                        groupSumNoSF  [pkey][syst][b] += hNo->GetBinContent(b+1);
                        groupSumWithSF[pkey][syst][b] += hWi->GetBinContent(b+1);
                    }
                    foundForThisPkey = true;
                    sampleHasAnyHist = true;
                }
            } // syst loop

            if (foundForThisPkey) {
                groupSources[pkey].push_back(sample);
            }
        } // pkey loop

        f->Close(); delete f;
        if (sampleHasAnyHist) ++nReadOK; else ++nReadFail;
    } // sample loop

    std::cout << ">>> Read " << nReadOK << " samples successfully, "
              << nReadFail << " failed/empty\n";

    // ── Per-group source breakdown ──
    std::cout << "\n>>> Process group composition:\n";
    for (const auto& g : allGroups) {
        const auto& srcList = groupSources[g];
        std::cout << "  " << std::left << std::setw(10) << g << " <- ";
        if (srcList.empty()) {
            std::cout << "(no samples — group will be all-1.0!)";
        } else {
            for (size_t i = 0; i < srcList.size(); ++i) {
                std::cout << srcList[i];
                if (i + 1 < srcList.size()) std::cout << ", ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";

    // ────────────────────────────────────────────────────────────────────
    // Phase 2: Compute ratios = Σ_noSF / Σ_withSF per (group, syst, bin)
    // ────────────────────────────────────────────────────────────────────
    std::map<std::string, std::map<std::string, BinVec>> groupRatio;
    for (const auto& g : allGroups) {
        for (const auto& s : systematics) {
            groupRatio[g][s].assign(totalBins, 1.0);
            for (int b = 0; b < totalBins; ++b) {
                const double sN = groupSumNoSF  [g][s][b];
                const double sW = groupSumWithSF[g][s][b];
                groupRatio[g][s][b] = (sW > 0.0) ? sN / sW : 1.0;
            }
        }
    }

    // ── Print central ratios for inspection ──
    std::cout << ">>> Central ratios per group (representative bins):\n";
    for (const auto& g : allGroups) {
        // Pick two representative bins to display
        const int sample_idx_7  = use2D ? (7  * nHTBins) : 7;
        const int sample_idx_10 = use2D ? (10 * nHTBins) : 10;
        const double r7  = (sample_idx_7  < totalBins) ? groupRatio[g]["central"][sample_idx_7]  : 1.0;
        const double r10 = (sample_idx_10 < totalBins) ? groupRatio[g]["central"][sample_idx_10] : 1.0;
        std::cout << "  " << std::left << std::setw(10) << g
                  << "  r(nJ=7" << (use2D ? ",HT0" : "")
                  << ")=" << std::fixed << std::setprecision(5) << r7
                  << "  r(nJ=10" << (use2D ? ",HT0" : "")
                  << ")=" << r10 << "\n";
    }
    std::cout << "\n";

    // ────────────────────────────────────────────────────────────────────
    // Phase 3: Build correctionlib JSON (schema v2)
    //
    //   systematic (Category) → process (Category) → Binning/MultiBinning
    // ────────────────────────────────────────────────────────────────────
    json systContent = json::array();

    int nGroupsLoaded = 0;
    for (const auto& g : allGroups) {
        if (!groupSources[g].empty()) ++nGroupsLoaded;
    }
    if (nGroupsLoaded == 0) {
        std::cerr << "\n[FATAL] No process group received any sample data.\n"
                     "        Run exe_BTagSF first to produce the input ROOT files.\n";
        return 1;
    }

    for (const auto& syst : systematics) {
        json processContent = json::array();

        for (const auto& pkey : allGroups) {
            json binningNode;

            json content = json::array();
            if (use2D) {
                for (int nj = 0; nj <= maxBin; ++nj) {
                    for (int ht = 0; ht < nHTBins; ++ht) {
                        const int idx = nj * nHTBins + ht;
                        content.push_back(groupRatio[pkey][syst][idx]);
                    }
                }
                binningNode["nodetype"] = "multibinning";
                binningNode["inputs"]   = json::array({"nJets", "HT"});
                binningNode["edges"]    = json::array({nJetsEdges, htEdgesJSON});
                binningNode["content"]  = content;
                binningNode["flow"]     = "clamp";
            } else {
                for (int b = 0; b <= maxBin; ++b) {
                    content.push_back(groupRatio[pkey][syst][b]);
                }
                binningNode["nodetype"] = "binning";
                binningNode["input"]    = "nJets";
                binningNode["edges"]    = nJetsEdges;
                binningNode["content"]  = content;
                binningNode["flow"]     = "clamp";
            }

            processContent.push_back({{"key", pkey}, {"value", binningNode}});
        }

        json processCategory;
        processCategory["nodetype"] = "category";
        processCategory["input"]    = "process";
        processCategory["content"]  = processContent;

        systContent.push_back({{"key", syst}, {"value", processCategory}});
    }

    // ── Assemble correctionlib correction object ──
    json correction;
    correction["name"]    = "btagNormReweight";
    correction["version"] = 1;

    if (use2D) {
        correction["description"] =
            "Per-process-group, per-(nJets,HT) normalization ratio for b-tag "
            "shape SF reweighting. Process groups: tt+LF, tt+cc, tt+B, ttH, "
            "ttHH, ttZH4b, ttZZ4b. Inclusive ttbar dispatched by genTtbarId. "
            "Minor backgrounds (QCD, V+jets, ...) borrow tt+LF (ttH AN App. A.2.1). "
            "Usage: finalWeight = btagEventWeight * "
            "evaluate({systematic, process, nJets, HT}). "
            "Caller must compute process via TtCatGroup::MakeProcessKey().";
    } else {
        correction["description"] =
            "Per-process-group, per-nJets normalization ratio for b-tag shape SF "
            "reweighting. Process groups: tt+LF, tt+cc, tt+B, ttH, ttHH, ttZH4b, "
            "ttZZ4b. Inclusive ttbar dispatched by genTtbarId. Minor backgrounds "
            "borrow tt+LF (ttH AN App. A.2.1). "
            "Usage: finalWeight = btagEventWeight * "
            "evaluate({systematic, process, nJets}). "
            "Caller must compute process via TtCatGroup::MakeProcessKey().";
    }

    // ── Inputs ──
    json inputsArray = json::array({
        {{"name", "systematic"}, {"type", "string"},
         {"description", "Systematic variation: central, up_lf, down_lf, ..."}},
        {{"name", "process"}, {"type", "string"},
         {"description", "Process group key (one of tt+LF, tt+cc, tt+B, ttH, "
                         "ttHH, ttZH4b, ttZZ4b). Use TtCatGroup::MakeProcessKey()."}},
        {{"name", "nJets"}, {"type", "int"},
         {"description", "Number of jets in the event"}}
    });

    if (use2D) {
        inputsArray.push_back(
            {{"name", "HT"}, {"type", "real"},
             {"description", "Scalar sum of jet pT [GeV]"}}
        );
    }

    correction["inputs"] = inputsArray;

    correction["output"] = {
        {"name", "weight"}, {"type", "real"},
        {"description", "Normalization ratio (preserves yield within process group)"}
    };

    json dataNode;
    dataNode["nodetype"] = "category";
    dataNode["input"]    = "systematic";
    dataNode["content"]  = systContent;
    correction["data"]   = dataNode;

    // ── Root JSON object (correctionlib schema v2) ──
    json root;
    root["schema_version"] = 2;

    if (use2D) {
        root["description"] =
            "B-tag shape SF per-process-group normalization reweight factors, "
            "derived per (nJets, HT) bin via cross-sample sum within each "
            "process group (ttH AN-19-094 App. A.2.1). Produced by makeReweightJSON.";
    } else {
        root["description"] =
            "B-tag shape SF per-process-group normalization reweight factors, "
            "derived per nJets bin via cross-sample sum within each process group "
            "(ttH AN-19-094 App. A.2.1). Produced by makeReweightJSON.";
    }
    root["corrections"] = json::array({correction});

    // ────────────────────────────────────────────────────────────────────
    // Write output
    // ────────────────────────────────────────────────────────────────────
    std::string outPath = Config::btagReweightJSON;
    std::ofstream out(outPath);
    if (!out.is_open()) {
        std::cerr << "[FATAL] Cannot write to " << outPath << "\n";
        return 1;
    }
    out << root.dump(2);
    out.close();

    std::cout << "\n>>> Written: " << outPath
              << " (" << allGroups.size() << " process groups × "
              << systematics.size() << " systematics)\n\n";

    // ────────────────────────────────────────────────────────────────────
    // Usage hints for downstream code
    // ────────────────────────────────────────────────────────────────────
    std::cout << "  Usage in downstream code:\n";
    std::cout << "    auto cs = correction::CorrectionSet::from_file(\""
              << outPath << "\");\n";
    std::cout << "    auto rw = cs->at(\"btagNormReweight\");\n";
    std::cout << "    // process key from sample name + genTtbarId:\n";
    std::cout << "    std::string pkey = TtCatGroup::MakeProcessKey(sampleName, genTtbarId);\n";
    if (use2D) {
        std::cout << "    double r = rw->evaluate({\"central\", pkey, nJets, HT});\n";
    } else {
        std::cout << "    double r = rw->evaluate({\"central\", pkey, nJets});\n";
    }

    std::cout << "\n  To compress: gzip " << outPath << "\n";
    std::cout << "  (correctionlib reads .json.gz natively)\n";

    return 0;
}
