// ============================================================================
// Config_TtCatGroup.hh
// ----------------------------------------------------------------------------
// Shared header for ttbar categorization → process key mapping.
//
// Used by:
//   - B-tag SF reweight tool (Config.hh, BTagSFProcessor, makeReweightJSON.cpp)
//   - Main analyzer (ttHHanalyzer_unified.cc)
//
// Single source of truth — change here, both places stay in sync.
//
// NOTE: this lives in `namespace TtCat`, NOT in `class Config`, because
//       Config is already a class in the b-tag tool. Putting these helpers
//       in their own namespace avoids any name clash.
//
// [Ref] ttH AN-19-094 §A.2: per-process b-tag normalization SF
//                            (LF / cc / B = b+2b+bb).
// ============================================================================
#ifndef CONFIG_TTCATGROUP_HH
#define CONFIG_TTCATGROUP_HH

#include <string>
#include <vector>

namespace TtCat {

enum class TtCatGroup {
    kLF = 0,    // tt + light flavour
    kCC = 1,    // tt + cc
    kB  = 2,    // tt + b (= tt+b + tt+2b + tt+bb)
    kNotTT = 3  // non-ttbar event
};

inline const char* TtCatGroupSuffix(TtCatGroup g) {
    switch (g) {
        case TtCatGroup::kLF:    return "_LF";
        case TtCatGroup::kCC:    return "_cc";
        case TtCatGroup::kB:     return "_B";
        case TtCatGroup::kNotTT: return "";
    }
    return "";
}

// genTtbarId encoding (CMS GenTtbarCategorizer):
//   0      -> LF
//   41-45  -> cc
//   51,52  -> 1 add b-jet (1 / >=2 b-hadrons)
//   53-55  -> >=2 add b-jets
//   <0     -> not ttbar (or branch absent)
inline TtCatGroup ClassifyTtCat(int genTtbarId) {
    if (genTtbarId < 0) return TtCatGroup::kNotTT;
    const int catId = genTtbarId % 100;
    if (catId == 0)                       return TtCatGroup::kLF;
    if (catId >= 41 && catId <= 45)       return TtCatGroup::kCC;
    if (catId == 51 || catId == 52 ||
        (catId >= 53 && catId <= 55))     return TtCatGroup::kB;
    return TtCatGroup::kNotTT;
}

inline bool IsInclusiveTtbar(const std::string& sampleName) {
    return sampleName == "TTToHadronic"
        || sampleName == "TTToSemiLeptonic"
        || sampleName == "TTTo2L2Nu";
}

// Process key used in JSON / hist names.
//   - inclusive ttbar: "<sample>_LF" / "_cc" / "_B"
//   - other samples:   "<sample>"
inline std::string MakeProcessKey(const std::string& sampleName, int genTtbarId) {
    if (!IsInclusiveTtbar(sampleName)) return sampleName;
    return sampleName + TtCatGroupSuffix(ClassifyTtCat(genTtbarId));
}

// All possible process keys produced by a given sample.
inline std::vector<std::string> AllProcessKeysForSample(const std::string& sampleName) {
    if (IsInclusiveTtbar(sampleName)) {
        return { sampleName + "_LF", sampleName + "_cc", sampleName + "_B" };
    }
    return { sampleName };
}

} // namespace TtCat

#endif // CONFIG_TTCATGROUP_HH
