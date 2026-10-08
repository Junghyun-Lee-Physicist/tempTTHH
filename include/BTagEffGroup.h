#ifndef TTHH_BTAGEFFGROUP_H
#define TTHH_BTAGEFFGROUP_H
// ============================================================================
// BTagEffGroup.h -- [STEP 25 K] the process group of an MC sample for the b-tag efficiency maps
//   of the fixed-WP method (EraConfig::BTagMethod::FixedWP, 2024; docs/DECISIONS.md D-2026-10-08-A).
//
//   The efficiency of a jet to pass a WP depends on its flavour, pT and |eta| first, and on the
//   process through the jet's environment (BTV's advice: measure it in the analysis' phase space,
//   per process where they differ). Three groups, by the sample name:
//     "qcd"   : QCD_*                      (multijet)
//     "tt"    : a name starting with tt/TT (ttbar, tt+X, ttH, ttHH, ttV, ttVV, tttt, ...)
//     "other" : everything else            (single top, tH, V+jets, VV, ...)
//   The map JSON (tools/stage7/btag_eff_maps.py) has these three and "all" (every MC sample),
//   its default for a group it does not know.
//   KEEP IN SYNC with btag_eff_group() in tools/stage7/btag_eff_maps.py (the test there checks both
//   on the 2024 sample list).
// ============================================================================
#include <string>

namespace tthh {

inline std::string btagEffGroup(const std::string& sample) {
    if (sample.rfind("QCD", 0) == 0) return "qcd";
    if (sample.size() >= 2 && (sample[0] == 't' || sample[0] == 'T') && (sample[1] == 't' || sample[1] == 'T'))
        return "tt";
    return "other";
}

}  // namespace tthh

#endif // TTHH_BTAGEFFGROUP_H
