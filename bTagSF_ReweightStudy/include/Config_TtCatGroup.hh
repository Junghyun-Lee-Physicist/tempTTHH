// ============================================================================
// Config_TtCatGroup.hh
// ----------------------------------------------------------------------------
// Process group mapping for b-tag normalization SF reweighting.
//
// Used by:
//   - B-tag SF reweight tool (Config.hh, BTagSFProcessor, makeReweightJSON.cpp)
//   - Main analyzer (ttHHanalyzer_unified.cc / CorrectionsManager)
//
// === Methodology ===
// ttH AN-19-094 §A.2.1 derives the b-tag normalization SF *per process group*:
//   - 4 main groups: ttH, tt+bb, tt+cc, tt+LF
//   - Minor backgrounds (V+jets, single t, ttV, diboson, QCD) borrow tt+LF
//     "due to poor statistics of simulated events"
//
// In ttHH(4b) we extend this with self-measured groups for high-b-jet
// processes that have sufficient statistics (sample-level check confirms
// ttHH has enough events; QCD HT slices except very high HT do NOT).
//
// Final 5 + 1 process groups:
//   tt+LF      <- inclusive ttbar LF events + ALL minor BG (QCD, V+jets, ...)
//   tt+cc      <- inclusive ttbar cc events
//   tt+B       <- inclusive ttbar B events + ttbb dedicated + tt4b + ttZ→bb
//                 + tttt + ttV+H (medium/high b-jet, fallback to "tt+B")
//   ttH        <- ttHtobb (own measurement, ttH AN App. A.2)
//   ttHH       <- ttHH signal (own measurement; sample stats sufficient)
//   ttZH4b     <- ttZHto4b (high b-jet 4b signature, own group)
//   ttZZ4b     <- ttZZto4b (high b-jet 4b signature, own group)
//
// Measurement region (BTV recommendation):
//   nJets >= 6, HT >= 500, 6th jet pT > 40 (skim invariants).
//   *** NO b-tag selection *** — that's the whole point of the reweight.
//
// Process key dispatch (per event):
//   For inclusive ttbar samples → genTtbarId-based group routing.
//   For other samples → fixed mapping by sample name (declared below).
//
// [Ref] ttH AN-19-094 §A.2.1, ttHH AN-2022/122 §6.1.4 (D.0.5).
// ============================================================================
#ifndef CONFIG_TTCATGROUP_HH
#define CONFIG_TTCATGROUP_HH

#include <string>
#include <vector>

namespace TtCatGroup {

enum class Group {
    kLF = 0,    // tt + light flavour
    kCC = 1,    // tt + cc
    kB  = 2,    // tt + b (= tt+b + tt+2b + tt+bb)
    kNotTT = 3
};

// genTtbarId encoding (CMS GenTtbarCategorizer):
//   0      -> LF
//   41-45  -> cc
//   51,52  -> 1 add b-jet (1 / >=2 b-hadrons)   ─┐
//   53-55  -> >=2 add b-jets                     ├─ all → kB
//   <0     -> not ttbar (or branch absent)      ─┘
inline Group Classify(int genTtbarId) {
    if (genTtbarId < 0) return Group::kNotTT;
    const int catId = genTtbarId % 100;
    if (catId == 0)                       return Group::kLF;
    if (catId >= 41 && catId <= 45)       return Group::kCC;
    if (catId == 51 || catId == 52 ||
        (catId >= 53 && catId <= 55))     return Group::kB;
    return Group::kNotTT;
}

inline bool IsInclusiveTtbar(const std::string& sampleName) {
    return sampleName == "TTToHadronic"
        || sampleName == "TTToSemiLeptonic"
        || sampleName == "TTTo2L2Nu";
}

// ──────────────────────────────────────────────────────────────────────────
// MakeProcessKey: returns the process group key for a given event.
//
// inclusive ttbar (TTToHadronic / SemiLep / 2L2Nu):
//     genTtbarId  →  "tt+LF" / "tt+cc" / "tt+B"
//
// dedicated ttbar HF samples & 4b signal-like processes:
//     fixed mapping (own group or borrowed) — see body.
//
// minor backgrounds (low b-jet env):
//     "tt+LF" (ttH AN App. A.2.1 fallback)
// ──────────────────────────────────────────────────────────────────────────
inline std::string MakeProcessKey(const std::string& sampleName, int genTtbarId) {
    // 1. inclusive ttbar → ttbar category dispatch
    if (IsInclusiveTtbar(sampleName)) {
        switch (Classify(genTtbarId)) {
            case Group::kLF:    return "tt+LF";
            case Group::kCC:    return "tt+cc";
            case Group::kB:     return "tt+B";
            case Group::kNotTT: return "tt+LF";  // safety (shouldn't happen)
        }
    }

    // 2. heavy-b dedicated ttbar samples
    if (sampleName == "ttbb")        return "tt+B";    // dedicated tt+B
    if (sampleName == "tt4b")        return "tt+B";    // 4b ttbar — fallback to tt+B
                                                       //   (own measurement if stats allow,
                                                       //    can be promoted to "tt4b" later)

    // 3. signals & 4-b heavy processes — own measurement
    if (sampleName == "ttHH")        return "ttHH";
    if (sampleName == "ttHtobb")     return "ttH";
    if (sampleName == "ttZHto4b")    return "ttZH4b";
    if (sampleName == "ttZZto4b")    return "ttZZ4b";

    // 4. medium b-jet processes (2b ttV, 4-top) → tt+B borrow
    if (sampleName == "ttZtobb")     return "tt+B";
    if (sampleName == "tttt")        return "tt+B";
    if (sampleName == "ttWH" ||
        sampleName == "ttWW" ||
        sampleName == "ttWZ" ||
        sampleName == "tttW")        return "tt+B";

    // 5. low b-jet minor backgrounds: tt+LF borrow (ttH AN App. A.2.1)
    //    QCD HT slices, V+jets, single t, diboson, ...
    return "tt+LF";
}

// ──────────────────────────────────────────────────────────────────────────
// All process group keys that the JSON should contain.
// Used by makeReweightJSON to build the final 'process' category.
// ──────────────────────────────────────────────────────────────────────────
inline std::vector<std::string> AllProcessGroupKeys() {
    return {
        "tt+LF",
        "tt+cc",
        "tt+B",
        "ttH",
        "ttHH",
        "ttZH4b",
        "ttZZ4b"
    };
}

// ──────────────────────────────────────────────────────────────────────────
// Per-sample: which process keys can a sample contribute to?
// (For inclusive ttbar: 3 groups. For others: 1 group.)
// Used by makeReweightJSON to know which sums to read from each sample file.
// ──────────────────────────────────────────────────────────────────────────
inline std::vector<std::string> ProcessKeysForSample(const std::string& sampleName) {
    if (IsInclusiveTtbar(sampleName)) {
        return { "tt+LF", "tt+cc", "tt+B" };
    }
    // For non-ttbar: a single key, determined by MakeProcessKey with dummy id.
    return { MakeProcessKey(sampleName, -1) };
}

// ──────────────────────────────────────────────────────────────────────────
// Sanitize a process group key for use in ROOT histogram names / JSON keys.
// "tt+LF" → "ttLF"  (drop '+' which is not safe in some contexts)
// ──────────────────────────────────────────────────────────────────────────
inline std::string SanitizeKey(const std::string& key) {
    std::string out;
    out.reserve(key.size());
    for (char c : key) {
        if (c == '+') continue;     // drop '+'
        out.push_back(c);
    }
    return out;
}

} // namespace TtCatGroup

#endif // CONFIG_TTCATGROUP_HH
