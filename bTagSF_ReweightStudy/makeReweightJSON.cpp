// ============================================================================
// makeReweightJSON.cpp
//
// Aggregates normalization ratios from all bTagReweight_<sample>.root files
// into a single correctionlib-schema-v2 JSON.
//
// The JSON can then be loaded by any downstream analysis code:
//
//   1D (useHTForReweight = false):
//     double ratio = rw->evaluate({"central", "TTToHadronic", 8});
//
//   2D (useHTForReweight = true):
//     double ratio = rw->evaluate({"central", "TTToHadronic", 8, 750.0});
//
// Schema:
//   systematic (Category) → process (Category) → Binning/MultiBinning → ratio
//
// Usage:
//   make exe_MakeJSON
//   ./exe_MakeJSON                        # uses all MC samples from Config
//   ./exe_MakeJSON TTToHadronic tt4b      # specific samples only
//
// Requires: ROOT 6, nlohmann/json (single-include header in include/nlohmann/)
//
// IMPORTANT:
//   This executable and exe_BTagSF MUST be compiled with the same
//   Config::useHTForReweight setting. If they disagree, the histogram
//   names inside the ROOT files will not match and an error will be raised.
//
// Author: Junghyun Lee
// ============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TString.h>

#include "Config.hh"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

int main(int argc, char** argv)
{
    // ── Determine sample list ──
    std::vector<std::string> mcSamples;

    if (argc > 1) {
        // User-specified samples
        for (int i = 1; i < argc; ++i)
            mcSamples.emplace_back(argv[i]);
    } else {
        // All MC from Config registry
        mcSamples = Config::GetMCSampleNames();
    }

    const auto& systematics = Config::btagSystematics;
    const int maxBin  = Config::btagMaxNJetsBin;
    const int nHTBins = Config::getNumHTBins();
    const bool use2D  = Config::useHTForReweight;

    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  makeReweightJSON: correctionlib JSON writer                 ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    std::cout << "  MC samples    : " << mcSamples.size() << "\n";
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
    std::cout << "  Total bins/proc: " << Config::getTotalReweightBins() << "\n";
    std::cout << "  Output        : " << Config::btagReweightJSON << "\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";

    // ── Build nJets edges: [0.0, 1.0, 2.0, ..., maxBin+1.0] ──
    // [중요] correctionlib Binning/MultiBinning은 edge를 반드시 float로 요구.
    //        int를 넣으면 JSON에서 0, 1, 2 (소수점 없음)로 직렬화되어
    //        "Invalid edge type" 에러가 발생한다.
    json nJetsEdges = json::array();
    for (int b = 0; b <= maxBin + 1; ++b) nJetsEdges.push_back(static_cast<double>(b));

    // ── Build HT edges (only used in 2D mode) ──
    json htEdgesJSON = json::array();
    if (use2D) {
        for (double e : Config::reweightHT_edges) htEdgesJSON.push_back(e);
    }

// ── Track which process keys were actually loaded ──
    std::vector<std::string> loadedSamples;

    // ── Build: systematic → process_key → binning/multibinning → ratio ──
    // [ttH AN App. A.2] inclusive ttbar samples produce 3 process keys each
    // (LF / cc / B), other samples produce a single key = sample name.
    // The hist names inside each ROOT file are now:
    //   2D: "NormRatios/h_normRatio_nJets_HT_<pkey>_<syst>"
    //   1D: "NormRatios/h_normRatio_nJets_<pkey>_<syst>"
    json systContent = json::array();

    for (const auto& syst : systematics) {
        json processContent = json::array();

        for (const auto& sample : mcSamples) {
            TString fname = TString(Config::btagOutPrefix) + sample.c_str() + ".root";
            TFile* f = TFile::Open(fname, "READ");
            if (!f || f->IsZombie()) {
                if (syst == "central")
                    std::cerr << "  [WARN] Cannot open " << fname << ", skipping\n";
                if (f) { f->Close(); delete f; }
                continue;
            }

            // For inclusive ttbar: 3 process keys per sample (LF/cc/B)
            // For others: 1 process key (== sample name)
            const auto pkeys = TtCat::AllProcessKeysForSample(sample);

            for (const auto& pkey : pkeys) {
                json binningNode;

                if (use2D) {
                    // ── 2D: Read TH2D, write MultiBinning ──
                    TString hname = "NormRatios/h_normRatio_nJets_HT_"
                                  + TString(pkey.c_str()) + "_"
                                  + TString(syst.c_str());
                    TH2D* h = dynamic_cast<TH2D*>(f->Get(hname));
                    if (!h) {
                        if (syst == "central")
                            std::cerr << "  [WARN] Missing " << hname << " in " << fname
                                      << " (was exe_BTagSF compiled with useHTForReweight=true,"
                                      << " or did this event-class have zero events?)\n";
                        continue;
                    }

                    // Flatten content: nJets row-major, HT fast axis
                    json content = json::array();
                    for (int nj = 0; nj <= maxBin; ++nj) {
                        for (int ht = 0; ht < nHTBins; ++ht) {
                            content.push_back(h->GetBinContent(nj + 1, ht + 1));
                        }
                    }

                    binningNode["nodetype"] = "multibinning";
                    binningNode["inputs"]   = json::array({"nJets", "HT"});
                    binningNode["edges"]    = json::array({nJetsEdges, htEdgesJSON});
                    binningNode["content"]  = content;
                    binningNode["flow"]     = "clamp";

                    if (syst == "central") {
                        double r7  = h->GetBinContent(8, 1);
                        double r10 = h->GetBinContent(11, 1);
                        std::cout << "  " << std::left << std::setw(28) << pkey
                                  << "  r(nJ=7,HT0)=" << std::fixed
                                  << std::setprecision(5) << r7
                                  << "  r(nJ=10,HT0)=" << r10 << "\n";
                        loadedSamples.push_back(pkey);
                    }

                } else {
                    // ── 1D: Read TH1D, write Binning ──
                    TString hname = "NormRatios/h_normRatio_nJets_"
                                  + TString(pkey.c_str()) + "_"
                                  + TString(syst.c_str());
                    TH1D* h = dynamic_cast<TH1D*>(f->Get(hname));
                    if (!h) {
                        if (syst == "central")
                            std::cerr << "  [WARN] Missing " << hname << " in " << fname
                                      << " (was exe_BTagSF compiled with useHTForReweight=false,"
                                      << " or did this event-class have zero events?)\n";
                        continue;
                    }

                    json content = json::array();
                    for (int b = 0; b <= maxBin; ++b) {
                        content.push_back(h->GetBinContent(b + 1));
                    }

                    binningNode["nodetype"] = "binning";
                    binningNode["input"]    = "nJets";
                    binningNode["edges"]    = nJetsEdges;
                    binningNode["content"]  = content;
                    binningNode["flow"]     = "clamp";

                    if (syst == "central") {
                        double r7  = (h->GetNbinsX() >= 8)  ? h->GetBinContent(8)  : 1.0;
                        double r10 = (h->GetNbinsX() >= 11) ? h->GetBinContent(11) : 1.0;
                        std::cout << "  " << std::left << std::setw(28) << pkey
                                  << "  r(nJ=7)=" << std::fixed << std::setprecision(5) << r7
                                  << "  r(nJ=10)=" << r10 << "\n";
                        loadedSamples.push_back(pkey);
                    }
                }

                // Push into JSON: process key, not sample name
                processContent.push_back({{"key", pkey}, {"value", binningNode}});
            } // end pkey loop

            f->Close(); delete f;
        } // end sample loop

        json processCategory;
        processCategory["nodetype"] = "category";
        processCategory["input"]    = "process";
        processCategory["content"]  = processContent;

        systContent.push_back({{"key", syst}, {"value", processCategory}});
    } // end syst loop

    if (loadedSamples.empty()) {
        std::cerr << "\n[FATAL] No samples loaded. Run exe_BTagSF first.\n";
        return 1;
    }

    // ── Assemble correctionlib correction object ──
    json correction;
    correction["name"]        = "btagNormReweight";

    if (use2D) {
        correction["description"] =
            "Per-process, per-(nJets,HT) normalization ratio for b-tag shape SF reweighting. "
            "Usage: finalWeight = btagEventWeight * evaluate({systematic, process, nJets, HT}). "
            "Preserves event yields before b-tag selection per (nJets,HT) bin.";
    } else {
        correction["description"] =
            "Per-process, per-nJets normalization ratio for b-tag shape SF reweighting. "
            "Usage: finalWeight = btagEventWeight * evaluate({systematic, process, nJets}). "
            "Preserves event yields before b-tag selection per nJets bin.";
    }
    correction["version"] = 1;

    // ── Inputs ──
    json inputsArray = json::array({
        {{"name", "systematic"}, {"type", "string"},
         {"description", "Systematic variation: central, up_lf, down_lf, ..."}},
        {{"name", "process"}, {"type", "string"},
         {"description", "MC process name (e.g. TTToHadronic, ttHH)"}},
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
        {"description", "Normalization ratio"}
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
            "B-tag shape SF normalization reweight factors, derived per MC process "
            "per (nJets, HT) bin. Produced by makeReweightJSON.";
    } else {
        root["description"] =
            "B-tag shape SF normalization reweight factors, derived per MC process "
            "per nJets bin. Produced by makeReweightJSON.";
    }
    root["corrections"] = json::array({correction});

    // ── Write output ──
    std::string outPath = Config::btagReweightJSON;
    std::ofstream out(outPath);
    if (!out.is_open()) {
        std::cerr << "[FATAL] Cannot write to " << outPath << "\n";
        return 1;
    }
    out << root.dump(2);
    out.close();

    std::cout << "\n>>> Written: " << outPath
              << " (" << loadedSamples.size() << " processes × "
              << systematics.size() << " systematics)\n\n";

    std::cout << "  Usage in downstream code:\n";
    std::cout << "    auto cs = correction::CorrectionSet::from_file(\""
              << outPath << "\");\n";
    std::cout << "    auto rw = cs->at(\"btagNormReweight\");\n";
    if (use2D) {
        std::cout << "    double r = rw->evaluate({\"central\", \"TTToHadronic\", 8, 750.0});\n";
    } else {
        std::cout << "    double r = rw->evaluate({\"central\", \"TTToHadronic\", 8});\n";
    }
    std::cout << "\n  To compress: gzip " << outPath << "\n";
    std::cout << "  (correctionlib reads .json.gz natively)\n";

    return 0;
}
