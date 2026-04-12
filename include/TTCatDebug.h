// ============================================================================
// TTCatDebug.h
//
// Self-contained unified ttbar-categorization debug logger for the analyzer.
// Paired with NtupleForge's ttbarCategorizer.py: both sides write the same
// CSV schema so the two outputs can be sorted + diffed for bit-level
// validation of the categorization logic.
//
// Output path: ./ttcat_ana.csv  (fixed, in the current working directory)
//              Opened lazily on first emit call; header line written once.
//              No environment variables, no configuration.
//
// Usage in ttHHanalyzer_unified.cc:
//
//     #include "TTCatDebug.h"
//
//     // Inside process() after computeTtCategory() is called:
//     {
//         const int  nGP  = genPartCount();
//         const int  nGJ  = genJetCount();
//         const bool hasGen = (nGP > 0 && nGJ > 0);
//         const bool hasTT  = hasGen ? eventHasTTPair() : false;
//
//         const char* codePath = "NOGEN";
//         AddBJetResult r{0, 0, 0, {}};
//         int nCJet   = -1;
//         int nBJetAcc = -1;
//
//         if (!hasGen) {
//             codePath = "FALLBACK";
//         } else if (!hasTT) {
//             codePath = "NOTT";
//         } else {
//             codePath = "GENPART";
//             r = countAdditionalBJetsDetailed();
//             // Recompute nBJetAcc locally to avoid modifying AddBJetResult.
//             nBJetAcc = 0;
//             for (int j = 0; j < nGJ; ++j) {
//                 if (static_cast<int>(_ev->GenJet_hadronFlavour.size()) <= j) break;
//                 if (_ev->GenJet_hadronFlavour[j] != 5) continue;
//                 if (_ev->GenJet_pt[j] < 20.0f) continue;
//                 if (std::fabs(_ev->GenJet_eta[j]) > 2.4f) continue;
//                 ++nBJetAcc;
//             }
//             nCJet = (r.nJets <= 1) ? countAdditionalCJets() : 0;
//         }
//
//         const bool isGP = (std::string(codePath) == "GENPART");
//         const int  genId = (_DataOrMC == "Data") ? -1
//                                                  : (_ev->genTtbarId % 100);
//
//         ttcatdbg::emit(
//             _ev->run, _ev->luminosityBlock, _ev->event,
//             codePath,
//             isGP ? nGP       : 0,
//             isGP ? nGJ       : 0,
//             hasTT,
//             isGP ? r.nHadrons : -1,
//             isGP ? nBJetAcc   : -1,
//             isGP ? r.nJets    : -1,
//             isGP ? r.nMatched : -1,
//             nCJet,
//             r.jetBHMap,
//             genId,
//             ttCatName(static_cast<int>(analyzerCat))
//         );
//     }
//
// Then run the analyzer and compare:
//     sort ttcat_ntu.csv > /tmp/ntu.s
//     sort ttcat_ana.csv > /tmp/ana.s
//     # Strip tag + header so only the event payload remains
//     tail -n +2 /tmp/ntu.s | cut -d, -f2- > /tmp/ntu.payload
//     tail -n +2 /tmp/ana.s | cut -d, -f2- > /tmp/ana.payload
//     diff -q /tmp/ntu.payload /tmp/ana.payload && echo "PERFECT MATCH"
//
// Author: Junghyun Lee (CMS ttHH, fully hadronic)
// ============================================================================
#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ttcatdbg {

// ---------------------------------------------------------------------------
// Singleton file stream. Opens on first access, writes header once, flushes
// per event. Not thread-safe (analyzer is single-threaded).
// ---------------------------------------------------------------------------
inline std::ofstream& stream_() {
    static std::ofstream f;
    static bool tried = false;
    if (!tried) {
        tried = true;
        f.open("ttcat_ana.csv", std::ios::out);
        if (f.is_open()) {
            f << "tag,run,lumi,event,path,nGP,nGJ,hasTT,"
                 "nAddBH,nBJetAcc,nBJetMatched,nBHMatched,nCJet,"
                 "jetBHMap,genTtbarIdMod100,category\n";
            std::cerr << "[ttcat] Debug CSV opened: ./ttcat_ana.csv\n";
        } else {
            std::cerr << "[ttcat] WARNING: failed to open ./ttcat_ana.csv\n";
        }
    }
    return f;
}

// ---------------------------------------------------------------------------
// Build "idx:cnt|idx:cnt|..." sorted by jet index
// ---------------------------------------------------------------------------
inline std::string formatJetBHMap_(const std::unordered_map<int, int>& m) {
    if (m.empty()) return std::string();
    std::vector<std::pair<int, int>> sorted(m.begin(), m.end());
    std::sort(sorted.begin(), sorted.end(),
              [](const std::pair<int, int>& a,
                 const std::pair<int, int>& b) { return a.first < b.first; });
    std::string out;
    out.reserve(sorted.size() * 8);
    for (std::size_t i = 0; i < sorted.size(); ++i) {
        if (i) out += '|';
        out += std::to_string(sorted[i].first);
        out += ':';
        out += std::to_string(sorted[i].second);
    }
    return out;
}

// ---------------------------------------------------------------------------
// Emit one CSV row. Schema:
//   tag,run,lumi,event,path,nGP,nGJ,hasTT,
//   nAddBH,nBJetAcc,nBJetMatched,nBHMatched,nCJet,
//   jetBHMap,genTtbarIdMod100,category
// ---------------------------------------------------------------------------
template <typename RunT, typename LumiT, typename EventT>
inline void emit(RunT run, LumiT lumi, EventT event,
                 const char* codePath,
                 int nGP, int nGJ, bool hasTT,
                 int nAddBH, int nBJetAcc, int nBJetMatched,
                 int nBHMatched, int nCJet,
                 const std::unordered_map<int, int>& jetBHMap,
                 int genTtbarIdMod100,
                 const char* categoryName) {
    std::ofstream& f = stream_();
    if (!f.is_open()) return;

    f << "TTCAT_ANA,"
      << run << ',' << lumi << ',' << event << ','
      << codePath << ','
      << nGP << ',' << nGJ << ',' << (hasTT ? 1 : 0) << ','
      << nAddBH << ',' << nBJetAcc << ',' << nBJetMatched << ','
      << nBHMatched << ',' << nCJet << ','
      << formatJetBHMap_(jetBHMap) << ','
      << genTtbarIdMod100 << ','
      << categoryName << '\n';
    f.flush();
}

}  // namespace ttcatdbg
