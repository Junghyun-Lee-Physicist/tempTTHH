#ifndef TREEVARS_H
#define TREEVARS_H
// =============================================================================
//  TreeVars.h — event variables of the ML tree (Tree v1; STEP 27 O, 2026-10-10)
// =============================================================================
//  Purpose : the DNN / GATJA input variables of AN-2022/122 v26 (Table 43, §6.2) that need the jets of an
//            event, computed from four-vectors only, so a unit test can check them without the analyzer
//            (test/test_TreeVars.cc) and the export tool can recompute them in Python from the jet vectors.
//  Source  : AN-2022/122 v26 §6.2.1 (χ² pairing, eq. 3-5, the jet choice of l.744-751, the mass ordering of
//            l.754-757), §6.2.3 (centrality, eq. 12), §6.2.4 (Fox-Wolfram, eq. 13-16), Table 43 (names).
//  Status  : PROPOSED with the Tree v1 plan (docs/PLAN_ML_SYST.md §5.2). The old helpers of
//            ttHHanalyzer_unified.h (event::getStats, getStatsComb, getFoxWolfram, getMaxPTSame/Comb,
//            getCentrality) are left as they were; they are not used for the tree. Differences, all on purpose:
//              * Δφ is folded into [0, π] (the old helpers used |φ1 − φ2|, so a pair across ±π had a wrong ΔR —
//                and so did the old FHSample ΔR columns made with them);
//              * Fox-Wolfram sums over all ordered pairs INCLUDING i = j (AN l.804-805: "the same jet combination
//                is considered as well") and divides by s = (Σ E)²  — the AN does not define s; (Σ E)² is the
//                e+e− convention (s = E_vis²). The old helper summed i < j only;
//              * the b-jet × other-jet statistics ("bj") pair a b jet with a jet that is NOT a b jet; the old
//                getStatsComb(jets, bjets) also paired a b jet with itself (ΔR = 0);
//              * m_jjj^{max pT} loops over distinct triplets (the old getMaxPTSame could take one jet twice);
//                m_jbb^{max pT} takes a jet that is not one of the two b jets;
//              * (m²)_b^avg = Σ m² / n (the AN symbol); the old code wrote (Σ m)² / n;
//              * a quantity that is undefined for the event (too few objects) is −1, never NaN.
//            The χ² term itself is the one of HiggsReconstructor (bit-identical to the 2026-06 code,
//            (m − m_X)² / √(0.1 Σ pT)); the AN's σ ("a function of JER") has no number in the AN — OPEN.
// =============================================================================

#include <algorithm>
#include <cmath>
#include <vector>

#include "TLorentzVector.h"
#include "HiggsReconstructor.h"

namespace TreeVars {

using P4s = std::vector<const TLorentzVector*>;

// |Δφ| folded into [0, π]
inline double deltaPhi(double a, double b) {
    return std::fabs(std::remainder(a - b, 2.0 * M_PI));
}
inline double deltaR(const TLorentzVector& a, const TLorentzVector& b) {
    const double dphi = deltaPhi(a.Phi(), b.Phi()), deta = a.Eta() - b.Eta();
    return std::sqrt(dphi * dphi + deta * deta);
}

// statistics over object pairs: ΔR (mean, min, max), |Δη| (mean, max), and the mass and pT of the pair with
// the smallest ΔR. −1 when there is no pair.
struct PairStats {
    float averageDeltaR = -1.f, minDeltaR = -1.f, maxDeltaR = -1.f;
    float averageDeltaEta = -1.f, maxDeltaEta = -1.f;
    float minDeltaRMass = -1.f, minDeltaRpT = -1.f;
    int   nPairs = 0;
};

namespace detail {
struct PairAcc {
    double sumR = 0, sumEta = 0, minR = 1e30, maxR = -1, maxEta = -1, mMass = -1, mPt = -1;
    int n = 0;
    void add(const TLorentzVector& a, const TLorentzVector& b) {
        const double r = deltaR(a, b), deta = std::fabs(a.Eta() - b.Eta());
        sumR += r; sumEta += deta; ++n;
        if (r < minR) { minR = r; const TLorentzVector s = a + b; mMass = s.M(); mPt = s.Pt(); }
        if (r > maxR) maxR = r;
        if (deta > maxEta) maxEta = deta;
    }
    PairStats result() const {
        PairStats p;
        if (n == 0) return p;
        p.nPairs = n;
        p.averageDeltaR = static_cast<float>(sumR / n);
        p.minDeltaR = static_cast<float>(minR);
        p.maxDeltaR = static_cast<float>(maxR);
        p.averageDeltaEta = static_cast<float>(sumEta / n);
        p.maxDeltaEta = static_cast<float>(maxEta);
        p.minDeltaRMass = static_cast<float>(mMass);
        p.minDeltaRpT = static_cast<float>(mPt);
        return p;
    }
};
}  // namespace detail

// all unordered pairs i < j of one collection
inline PairStats pairStatsSame(const P4s& v) {
    detail::PairAcc a;
    for (size_t i = 0; i < v.size(); ++i)
        for (size_t j = i + 1; j < v.size(); ++j) a.add(*v[i], *v[j]);
    return a.result();
}
// all pairs (a_i, b_j) of two disjoint collections
inline PairStats pairStatsCross(const P4s& va, const P4s& vb) {
    detail::PairAcc a;
    for (const auto* x : va)
        for (const auto* y : vb) a.add(*x, *y);
    return a.result();
}

// Fox-Wolfram moments H0..H4 (AN eq. 16) and R_l = H_l / H0 (l = 1..4; R[0] unused). −1 for fewer than 2 objects.
struct FoxWolfram {
    float H[5] = {-1.f, -1.f, -1.f, -1.f, -1.f};
    float R[5] = {-1.f, -1.f, -1.f, -1.f, -1.f};
};
inline FoxWolfram foxWolfram(const P4s& v) {
    FoxWolfram fw;
    if (v.size() < 2) return fw;
    double sumE = 0.0;
    for (const auto* p : v) sumE += p->E();
    const double s = sumE * sumE;
    if (!(s > 0.0)) return fw;
    double h[5] = {0, 0, 0, 0, 0};
    for (size_t i = 0; i < v.size(); ++i)
        for (size_t j = 0; j < v.size(); ++j) {
            const double pipj = v[i]->P() * v[j]->P();
            double c = 1.0;                                    // i = j: Ω = 0, P_l(1) = 1
            if (i != j && pipj > 0.0)
                c = std::max(-1.0, std::min(1.0, v[i]->Vect().Dot(v[j]->Vect()) / pipj));
            const double c2 = c * c;
            const double P[5] = {1.0, c, 0.5 * (3.0 * c2 - 1.0), 0.5 * (5.0 * c2 * c - 3.0 * c),
                                 0.125 * (35.0 * c2 * c2 - 30.0 * c2 + 3.0)};
            for (int l = 0; l < 5; ++l) h[l] += pipj * P[l] / s;
        }
    for (int l = 0; l < 5; ++l) fw.H[l] = static_cast<float>(h[l]);
    for (int l = 1; l < 5; ++l) fw.R[l] = (h[0] != 0.0) ? static_cast<float>(h[l] / h[0]) : -1.f;
    return fw;
}

// centrality (AN eq. 12, jets only in FH): Σ pT / Σ |p|. −1 for no object.
inline float centrality(const P4s& v) {
    double spt = 0.0, sp = 0.0;
    for (const auto* p : v) { spt += p->Pt(); sp += p->P(); }
    return (v.empty() || !(sp > 0.0)) ? -1.f : static_cast<float>(spt / sp);
}
inline float scalarHT(const P4s& v) {
    double s = 0.0;
    for (const auto* p : v) s += p->Pt();
    return static_cast<float>(s);
}
inline float averageMass(const P4s& v) {
    if (v.empty()) return -1.f;
    double s = 0.0;
    for (const auto* p : v) s += p->M();
    return static_cast<float>(s / v.size());
}
inline float averageMassSqr(const P4s& v) {
    if (v.empty()) return -1.f;
    double s = 0.0;
    for (const auto* p : v) s += p->M() * p->M();
    return static_cast<float>(s / v.size());
}

// m_jjj^{max pT}: the mass of the triplet (i < j < k) with the largest vector pT. −1 for fewer than 3 jets.
inline float maxPTmassjjj(const P4s& v) {
    double bestPt = -1.0, mass = -1.0;
    for (size_t i = 0; i < v.size(); ++i)
        for (size_t j = i + 1; j < v.size(); ++j)
            for (size_t k = j + 1; k < v.size(); ++k) {
                const TLorentzVector s = *v[i] + *v[j] + *v[k];
                if (s.Pt() > bestPt) { bestPt = s.Pt(); mass = s.M(); }
            }
    return static_cast<float>(mass);
}
// m_jbb^{max pT}: two distinct b jets and one other jet (not one of the two). isB[i] says whether jets[i] is a
//   b jet. −1 when there is no such triplet.
inline float maxPTmassjbb(const P4s& jets, const std::vector<bool>& isB) {
    double bestPt = -1.0, mass = -1.0;
    const size_t n = jets.size();
    for (size_t a = 0; a < n; ++a) {
        if (!isB[a]) continue;
        for (size_t b = a + 1; b < n; ++b) {
            if (!isB[b]) continue;
            for (size_t j = 0; j < n; ++j) {
                if (j == a || j == b) continue;
                const TLorentzVector s = *jets[a] + *jets[b] + *jets[j];
                if (s.Pt() > bestPt) { bestPt = s.Pt(); mass = s.M(); }
            }
        }
    }
    return static_cast<float>(mass);
}

// ── χ² pairing with the AN's choice of the four jets (§6.2.1, l.744-751) ─────────────────────────────────────
//   nM ≥ 4 : every 4 of the b (score ≥ M) jets — the same as the cut-flow reconstruction (HiggsReconstructor on
//            the b jets), so the values are bit-identical there;
//   nM = 3 : the three b jets + the 4th among the loose-not-medium jets (L ≤ score < M), or among the jets below
//            L when there is no loose one (AN);
//   nM = 2 : (OUR extension, for the QCD training/control regions of the ttH(bb) FH method, AN-19-094 §8.1) the
//            two b jets + two among the loose-not-medium jets; with only one such jet, it and one jet below L;
//            with none, two jets below L;
//   nM < 2 : none (−1).
//   A loop over every candidate choice and every pairing keeps the smallest χ² per hypothesis. Masses (AN
//   l.754-757): HH: mH1 / mH2 = the pair mass closest / second closest to m_H (pT likewise); ZZ: closest /
//   second to m_Z; ZH: the H candidate and the Z candidate.
struct Chi2Result {
    float chi2HH = -1.f, mH1 = -1.f, mH2 = -1.f, ptH1 = -1.f, ptH2 = -1.f;
    float chi2ZH = -1.f, mZH_H = -1.f, mZH_Z = -1.f;
    float chi2ZZ = -1.f, mZ1 = -1.f, mZ2 = -1.f;
    int   nM = 0;            // the b jets of the event
    int   nCandSets = 0;     // the 4-jet sets looked at (0 = none)
};

namespace detail {
inline void combinations(const std::vector<int>& pool, int k, size_t start, std::vector<int>& cur,
                         std::vector<std::vector<int>>& out) {
    if (static_cast<int>(cur.size()) == k) { out.push_back(cur); return; }
    for (size_t i = start; i < pool.size(); ++i) {
        cur.push_back(pool[i]);
        combinations(pool, k, i + 1, cur, out);
        cur.pop_back();
    }
}
}  // namespace detail

inline Chi2Result chi2AN(const P4s& jets, const std::vector<float>& score, float wpL, float wpM,
                         float mH, float mZ) {
    Chi2Result r;
    std::vector<int> M, Lo, Li;                     // b (≥ M), loose-not-medium [L, M), below L
    for (size_t i = 0; i < jets.size(); ++i) {
        if (score[i] >= wpM) M.push_back(static_cast<int>(i));
        else if (score[i] >= wpL) Lo.push_back(static_cast<int>(i));
        else Li.push_back(static_cast<int>(i));
    }
    r.nM = static_cast<int>(M.size());
    std::vector<std::vector<int>> sets;             // each: the jet indices that enter one reconstruct() call
    if (M.size() >= 4) {
        sets.push_back(M);                          // reconstruct() loops over every 4 of them itself
    } else if (M.size() >= 2) {
        const int need = 4 - static_cast<int>(M.size());
        std::vector<std::vector<int>> extra;
        std::vector<int> cur;
        if (static_cast<int>(Lo.size()) >= need) {
            detail::combinations(Lo, need, 0, cur, extra);
        } else {
            const int needLi = need - static_cast<int>(Lo.size());
            std::vector<std::vector<int>> fromLi;
            detail::combinations(Li, needLi, 0, cur, fromLi);
            for (const auto& c : fromLi) {
                std::vector<int> e = Lo;
                e.insert(e.end(), c.begin(), c.end());
                extra.push_back(e);
            }
        }
        for (const auto& e : extra) {
            std::vector<int> s = M;
            s.insert(s.end(), e.begin(), e.end());
            sets.push_back(s);
        }
    }
    HiggsReconstructor reco;
    HiggsReconstructor::PairResult hh, zh, zz;
    for (const auto& s : sets) {
        P4s p;
        for (int i : s) p.push_back(jets[i]);
        reco.reconstruct(p, mH, mZ);
        auto keep = [](HiggsReconstructor::PairResult& best, const HiggsReconstructor::PairResult& c) {
            if (c.valid() && (!best.valid() || c.chi2 < best.chi2)) best = c;
        };
        keep(hh, reco.HH()); keep(zh, reco.ZH()); keep(zz, reco.ZZ());
        ++r.nCandSets;
    }
    if (hh.valid()) {
        r.chi2HH = hh.chi2;
        const bool firstCloser = std::fabs(hh.mass1 - mH) <= std::fabs(hh.mass2 - mH);
        r.mH1 = firstCloser ? hh.mass1 : hh.mass2;  r.mH2 = firstCloser ? hh.mass2 : hh.mass1;
        r.ptH1 = firstCloser ? hh.pt1 : hh.pt2;     r.ptH2 = firstCloser ? hh.pt2 : hh.pt1;
    }
    if (zz.valid()) {
        r.chi2ZZ = zz.chi2;
        const bool firstCloser = std::fabs(zz.mass1 - mZ) <= std::fabs(zz.mass2 - mZ);
        r.mZ1 = firstCloser ? zz.mass1 : zz.mass2;  r.mZ2 = firstCloser ? zz.mass2 : zz.mass1;
    }
    if (zh.valid()) {                               // HiggsReconstructor: mass1 = the Z candidate, mass2 = the H one
        r.chi2ZH = zh.chi2; r.mZH_Z = zh.mass1; r.mZH_H = zh.mass2;
    }
    return r;
}

}  // namespace TreeVars

#endif  // TREEVARS_H
