#ifndef GENMATCH_H
#define GENMATCH_H
// =============================================================================
//  GenMatch.h — which hard-process quark a reconstructed jet comes from (Tree v1; STEP 27 O, 2026-10-10)
// =============================================================================
//  Purpose : the truth labels of the jet-assignment ML (GATJA: "Higgs / top / other" per b jet, AN-2022/122 §6.2.7;
//            JABDT-like studies, AN §6.2.6) and of reconstruction-efficiency checks (χ² vs truth, AN Table 47).
//  Rule (PROPOSED, docs/PLAN_ML_SYST.md §8 item 4 — the user's decision is open):
//    partons = the quarks (|pdgId| 1-5) that are fromHardProcess (NanoAOD statusFlags bit 8) and isLastCopy (bit 13);
//    origin  = the first ancestor whose |pdgId| differs from the quark's own (copies of the quark are skipped):
//                H (25) and a b quark -> 1 (H→b), t (6) and a b quark -> 2 (t→b), W (24) -> 3 (W→q), Z (23) -> 4 (Z→q);
//              anything else (an additional b of a 4FS tt+bb event, a gluon splitting, ...) is not a parton here;
//    mother  = the GenPart index of the FIRST copy of that ancestor (two b quarks of one Higgs share it);
//    top     = [STEP 27 P] the GenPart index of the first copy of the top the quark comes from: for t→b the mother
//              itself, for W→q the parent of the W's first copy when that is a top (|pdgId| 6), else −1 — so the b and
//              the two light quarks of one hadronic top share it (FH has two hadronic tops; the W index alone cannot
//              tell which top a W quark belongs to, and GenPart is not in the tree);
//    matching: every (jet, parton) pair with ΔR < 0.4 (Δφ folded into [0, π]), taken in increasing ΔR, each jet
//              and each parton at most once (greedy unique matching). The AN's JABDT used ΔR(jet, gen jet) < 0.4
//              with the closest permutation; the 0.4 is the AK4 radius.
//  Output  : per jet a label 0 (none) / 1 / 2 / 3 / 4, the mother index and the top index (−1 for none).
//  Not used for any selection or weight — labels only.
// =============================================================================

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <tuple>
#include <vector>

namespace GenMatch {

enum Label : int { kNone = 0, kHiggsB = 1, kTopB = 2, kWq = 3, kZq = 4 };

struct GenP {   // the GenPart fields the rule reads
    int pdgId; int mother; int statusFlags; float pt, eta, phi;
};
struct Parton { int idx; int label; int motherIdx; int topIdx; float eta, phi; };

constexpr int kFromHardProcess = 1 << 8;
constexpr int kIsLastCopy      = 1 << 13;

inline double deltaPhi(double a, double b) { return std::fabs(std::remainder(a - b, 2.0 * M_PI)); }

// the first copy of particle i (walk up through mothers with the same pdgId)
inline int firstCopy(const std::vector<GenP>& gp, int i) {
    int guard = 0;
    while (i >= 0 && i < static_cast<int>(gp.size())) {
        const int m = gp[i].mother;
        if (m < 0 || m >= static_cast<int>(gp.size()) || gp[m].pdgId != gp[i].pdgId || ++guard > 1000) break;
        i = m;
    }
    return i;
}

inline std::vector<Parton> hardPartons(const std::vector<GenP>& gp) {
    std::vector<Parton> out;
    const int n = static_cast<int>(gp.size());
    for (int i = 0; i < n; ++i) {
        const int apdg = std::abs(gp[i].pdgId);
        if (apdg < 1 || apdg > 5) continue;
        if (!(gp[i].statusFlags & kFromHardProcess) || !(gp[i].statusFlags & kIsLastCopy)) continue;
        // first ancestor with a different |pdgId|
        int m = gp[i].mother, guard = 0;
        while (m >= 0 && m < n && std::abs(gp[m].pdgId) == apdg && ++guard < 1000) m = gp[m].mother;
        if (m < 0 || m >= n) continue;
        const int am = std::abs(gp[m].pdgId);
        int label = kNone;
        if (am == 25 && apdg == 5) label = kHiggsB;
        else if (am == 6 && apdg == 5) label = kTopB;
        else if (am == 24) label = kWq;
        else if (am == 23) label = kZq;
        if (label == kNone) continue;
        const int mFirst = firstCopy(gp, m);
        int top = -1;                                   // [STEP 27 P] the top of a t→b or a t→W→q quark
        if (label == kTopB) {
            top = mFirst;
        } else if (label == kWq) {
            const int wm = gp[mFirst].mother;           // the parent of the W's first copy
            if (wm >= 0 && wm < n && std::abs(gp[wm].pdgId) == 6) top = firstCopy(gp, wm);
        }
        out.push_back({i, label, mFirst, top, gp[i].eta, gp[i].phi});
    }
    return out;
}

// jets: (eta, phi) per jet. label/mother (and top, when given) are resized to the number of jets.
inline void matchJets(const std::vector<std::pair<float, float>>& jets, const std::vector<Parton>& partons,
                      float dRmax, std::vector<int>& label, std::vector<int>& mother,
                      std::vector<int>* top = nullptr) {
    label.assign(jets.size(), kNone);
    mother.assign(jets.size(), -1);
    if (top) top->assign(jets.size(), -1);
    std::vector<std::tuple<double, int, int>> cand;   // (ΔR, jet, parton)
    for (size_t j = 0; j < jets.size(); ++j)
        for (size_t p = 0; p < partons.size(); ++p) {
            const double dphi = deltaPhi(jets[j].second, partons[p].phi);
            // [STEP 27 P] in double, as the Python check does (a float difference could flip a ΔR right at dRmax)
            const double deta = static_cast<double>(jets[j].first) - static_cast<double>(partons[p].eta);
            const double dr = std::sqrt(dphi * dphi + deta * deta);
            if (dr < dRmax) cand.emplace_back(dr, static_cast<int>(j), static_cast<int>(p));
        }
    std::sort(cand.begin(), cand.end());
    std::vector<bool> jetUsed(jets.size(), false), parUsed(partons.size(), false);
    for (const auto& c : cand) {
        const int j = std::get<1>(c), p = std::get<2>(c);
        if (jetUsed[j] || parUsed[p]) continue;
        jetUsed[j] = parUsed[p] = true;
        label[j] = partons[p].label;
        mother[j] = partons[p].motherIdx;
        if (top) (*top)[j] = partons[p].topIdx;
    }
}

}  // namespace GenMatch

#endif  // GENMATCH_H
