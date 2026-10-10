// =============================================================================
//  test_TreeVars.cc — unit test of include/TreeVars.h and include/GenMatch.h (STEP 27 O, Tree v1; P: top index)
//
//    g++ -std=c++17 -I include test/test_TreeVars.cc src/HiggsReconstructor.cc $(root-config --cflags --libs)
//
//  Each check prints "[PASS] ..." or "[FAIL] ..."; the last line is "PASS <n> / FAIL <m>"; exit 0 only if m = 0.
//  The expected values are computed here independently (brute force, closed forms), never by the code under test.
// =============================================================================
#include <cmath>
#include <cstdio>
#include <functional>
#include <random>
#include <vector>

#include "TLorentzVector.h"
#include "TreeVars.h"
#include "GenMatch.h"
#include "HiggsReconstructor.h"

static int nPass = 0, nFail = 0;
static void check(const char* what, bool ok, const std::string& detail = "") {
    if (ok) { ++nPass; std::printf("[PASS] %s\n", what); }
    else    { ++nFail; std::printf("[FAIL] %s %s\n", what, detail.c_str()); }
}
static bool close(double a, double b, double tol = 1e-5) {
    return std::fabs(a - b) <= tol * std::max(1.0, std::max(std::fabs(a), std::fabs(b)));
}
static TLorentzVector ptEtaPhiM(double pt, double eta, double phi, double m) {
    TLorentzVector v; v.SetPtEtaPhiM(pt, eta, phi, m); return v;
}

int main() {
    using namespace TreeVars;

    // ── 1. Δφ fold ──────────────────────────────────────────────────────────
    check("deltaPhi(3.1, -3.1) = 2π - 6.2", close(deltaPhi(3.1, -3.1), 2 * M_PI - 6.2, 1e-12));
    check("deltaPhi(0.3, 0.1) = 0.2", close(deltaPhi(0.3, 0.1), 0.2, 1e-12));
    {
        TLorentzVector a = ptEtaPhiM(50, 0.5, 3.10, 5), b = ptEtaPhiM(40, 0.2, -3.10, 5);
        const double expect = std::sqrt(std::pow(2 * M_PI - 6.2, 2) + 0.09);
        check("deltaR across ±π uses the folded Δφ", close(deltaR(a, b), expect, 1e-9));
        P4s v = {&a, &b};
        const PairStats ps = pairStatsSame(v);
        check("pairStatsSame of a pair across ±π: min ΔR = folded value (old helper gave 6.2…)",
              close(ps.minDeltaR, expect, 1e-6) && ps.nPairs == 1);
    }

    // ── 2. pair statistics against brute force on random jets ───────────────
    std::mt19937 rng(12345);
    std::uniform_real_distribution<double> uPt(30, 300), uEta(-2.4, 2.4), uPhi(-M_PI, M_PI), uM(2, 25);
    std::vector<TLorentzVector> J;
    for (int i = 0; i < 9; ++i) J.push_back(ptEtaPhiM(uPt(rng), uEta(rng), uPhi(rng), uM(rng)));
    P4s all; for (auto& j : J) all.push_back(&j);
    {
        double sR = 0, sE = 0, mn = 1e9, mx = -1, mxE = -1, mM = 0, mP = 0; int n = 0;
        for (size_t i = 0; i < J.size(); ++i)
            for (size_t k = i + 1; k < J.size(); ++k) {
                double dphi = std::fabs(J[i].Phi() - J[k].Phi()); if (dphi > M_PI) dphi = 2 * M_PI - dphi;
                const double deta = std::fabs(J[i].Eta() - J[k].Eta()), r = std::hypot(dphi, deta);
                sR += r; sE += deta; ++n; mx = std::max(mx, r); mxE = std::max(mxE, deta);
                if (r < mn) { mn = r; mM = (J[i] + J[k]).M(); mP = (J[i] + J[k]).Pt(); }
            }
        const PairStats ps = pairStatsSame(all);
        check("pairStatsSame = brute force (mean/min/max ΔR, mean/max Δη, mass and pT of the min-ΔR pair)",
              ps.nPairs == n && close(ps.averageDeltaR, sR / n) && close(ps.minDeltaR, mn) && close(ps.maxDeltaR, mx) &&
              close(ps.averageDeltaEta, sE / n) && close(ps.maxDeltaEta, mxE) && close(ps.minDeltaRMass, mM, 1e-4) &&
              close(ps.minDeltaRpT, mP, 1e-4));
        P4s a = {all[0], all[1], all[2]}, b = {all[3], all[4]};
        const PairStats pc = pairStatsCross(a, b);
        check("pairStatsCross counts |a| x |b| pairs", pc.nPairs == 6);
        P4s none;
        const PairStats p0 = pairStatsSame(none);
        check("no pair -> every statistic -1, nPairs 0", p0.nPairs == 0 && p0.minDeltaR == -1.f && p0.averageDeltaR == -1.f);
    }

    // ── 3. Fox-Wolfram (AN eq. 16, i = j included, s = (ΣE)²) ────────────────
    {
        TLorentzVector a = ptEtaPhiM(100, 0, 0, 0), b = ptEtaPhiM(100, 0, M_PI, 0);   // back to back, massless
        P4s v = {&a, &b};
        const FoxWolfram fw = foxWolfram(v);
        check("FW back-to-back massless pair: H0 = 1, H1 = 0, H2 = 1, H3 = 0, H4 = 1",
              close(fw.H[0], 1) && std::fabs(fw.H[1]) < 1e-6 && close(fw.H[2], 1) && std::fabs(fw.H[3]) < 1e-6 && close(fw.H[4], 1));
        // general: H0 = (Σ|p|)² / (ΣE)², and H_l = brute-force double sum
        double sp = 0, se = 0; for (auto* p : all) { sp += p->P(); se += p->E(); }
        const FoxWolfram g = foxWolfram(all);
        double h2 = 0;
        for (auto* p : all) for (auto* q : all) {
            const double c = (p == q) ? 1.0 : p->Vect().Dot(q->Vect()) / (p->P() * q->P());
            h2 += p->P() * q->P() * 0.5 * (3 * c * c - 1) / (se * se);
        }
        check("FW of 9 random jets: H0 = (Σ|p|)²/(ΣE)², H2 = brute-force double sum, R2 = H2/H0",
              close(g.H[0], sp * sp / (se * se)) && close(g.H[2], h2) && close(g.R[2], h2 / (sp * sp / (se * se))));
        P4s one = {all[0]};
        check("FW of one object: -1 (undefined)", foxWolfram(one).H[0] == -1.f);
    }

    // ── 4. centrality, HT, masses ───────────────────────────────────────────
    {
        double spt = 0, sp = 0, sm = 0, sm2 = 0;
        for (auto* p : all) { spt += p->Pt(); sp += p->P(); sm += p->M(); sm2 += p->M() * p->M(); }
        check("centrality = Σ pT / Σ |p|", close(centrality(all), spt / sp));
        check("scalarHT = Σ pT", close(scalarHT(all), spt, 1e-6));
        check("averageMass = Σ m / n, averageMassSqr = Σ m² / n (AN (m²)^avg, not (Σ m)²/n)",
              close(averageMass(all), sm / 9) && close(averageMassSqr(all), sm2 / 9));
        double best = -1, mass = -1;
        for (size_t i = 0; i < J.size(); ++i) for (size_t j = i + 1; j < J.size(); ++j) for (size_t k = j + 1; k < J.size(); ++k) {
            const TLorentzVector s = J[i] + J[j] + J[k]; if (s.Pt() > best) { best = s.Pt(); mass = s.M(); } }
        check("maxPTmassjjj = brute force over distinct triplets", close(maxPTmassjjj(all), mass, 1e-5));
        std::vector<bool> isB = {true, false, true, false, false, true, false, false, false};
        best = -1; mass = -1;
        for (size_t x = 0; x < 9; ++x) for (size_t y = x + 1; y < 9; ++y) {
            if (!isB[x] || !isB[y]) continue;
            for (size_t j = 0; j < 9; ++j) { if (j == x || j == y) continue;
                const TLorentzVector s = J[x] + J[y] + J[j]; if (s.Pt() > best) { best = s.Pt(); mass = s.M(); } } }
        check("maxPTmassjbb = brute force (two distinct b jets + another jet)", close(maxPTmassjbb(all, isB), mass, 1e-5));
    }

    // ── 5. χ² with the AN jet choice ────────────────────────────────────────
    const float L = 0.05f, M = 0.30f, mH = 125.f, mZ = 91.2f;
    auto brute = [&](const std::vector<int>& idx) {   // smallest χ²_HH over every 4-subset of idx and pairing
        double best = 1e30;
        const size_t n = idx.size();
        for (size_t a = 0; a < n; ++a) for (size_t b = a + 1; b < n; ++b) for (size_t c = b + 1; c < n; ++c) for (size_t d = c + 1; d < n; ++d) {
            const int q[4] = {idx[a], idx[b], idx[c], idx[d]};
            const int pr[3][4] = {{0, 1, 2, 3}, {0, 2, 1, 3}, {0, 3, 1, 2}};
            for (auto& P : pr) {
                auto term = [&](int i, int j) {
                    const TLorentzVector s = J[q[i]] + J[q[j]];
                    const float m = static_cast<float>(s.M()), spt = static_cast<float>(J[q[i]].Pt() + J[q[j]].Pt());
                    return std::pow(m - mH, 2) / std::pow(spt / 2.0f * 0.2f, 0.5f);
                };
                const float x = static_cast<float>(term(P[0], P[1]) + term(P[2], P[3]));
                if (x < best) best = x;
            }
        }
        return best;
    };
    {
        // nM = 5: equal to HiggsReconstructor on the b jets (the cut-flow reconstruction), bit for bit
        std::vector<float> s = {0.9f, 0.01f, 0.8f, 0.4f, 0.02f, 0.95f, 0.1f, 0.5f, 0.03f};   // M: 0,2,3,5,7
        const Chi2Result r = chi2AN(all, s, L, M, mH, mZ);
        HiggsReconstructor hr;
        P4s b = {all[0], all[2], all[3], all[5], all[7]};
        hr.reconstruct(b, mH, mZ);
        check("nM = 5: χ²_HH, χ²_ZZ, χ²_ZH bit-identical to HiggsReconstructor on the b jets",
              r.nM == 5 && r.chi2HH == hr.HH().chi2 && r.chi2ZZ == hr.ZZ().chi2 && r.chi2ZH == hr.ZH().chi2);
        check("nM = 5: χ²_HH = brute force", close(r.chi2HH, brute({0, 2, 3, 5, 7}), 1e-6));
        check("HH masses ordered: |mH1 - 125| <= |mH2 - 125|; ZZ likewise for 91.2",
              std::fabs(r.mH1 - mH) <= std::fabs(r.mH2 - mH) && std::fabs(r.mZ1 - mZ) <= std::fabs(r.mZ2 - mZ));
        check("the pair masses are those of the best pairing (as a set)",
              (close(r.mH1, hr.HH().mass1) && close(r.mH2, hr.HH().mass2)) || (close(r.mH1, hr.HH().mass2) && close(r.mH2, hr.HH().mass1)));
    }
    {
        // nM = 3 with loose jets 3 and 7: the 4th jet is one of the loose ones (AN), never a light one
        std::vector<float> s = {0.9f, 0.01f, 0.8f, 0.1f, 0.02f, 0.95f, 0.01f, 0.2f, 0.03f};   // M: 0,2,5; Lo: 3,7
        const Chi2Result r = chi2AN(all, s, L, M, mH, mZ);
        const double e = std::min(brute({0, 2, 5, 3}), brute({0, 2, 5, 7}));
        check("nM = 3 with loose jets: best over the loose 4th jet only", r.nM == 3 && r.nCandSets == 2 && close(r.chi2HH, e, 1e-6));
    }
    {
        // nM = 3, no loose: the 4th among the jets below L
        std::vector<float> s = {0.9f, 0.01f, 0.8f, 0.01f, 0.02f, 0.95f, 0.01f, 0.03f, 0.03f};  // M: 0,2,5; Li: 1,3,4,6,7,8
        const Chi2Result r = chi2AN(all, s, L, M, mH, mZ);
        double e = 1e30; for (int k : {1, 3, 4, 6, 7, 8}) e = std::min(e, brute({0, 2, 5, k}));
        check("nM = 3 without loose: best over the light 4th jet", r.nCandSets == 6 && close(r.chi2HH, e, 1e-6));
    }
    {
        // nM = 2, one loose (3): it and one light jet (our extension)
        std::vector<float> s = {0.9f, 0.01f, 0.8f, 0.1f, 0.02f, 0.01f, 0.01f, 0.03f, 0.04f};   // M: 0,2; Lo: 3; Li: 1,4,5,6,7,8
        const Chi2Result r = chi2AN(all, s, L, M, mH, mZ);
        double e = 1e30; for (int k : {1, 4, 5, 6, 7, 8}) e = std::min(e, brute({0, 2, 3, k}));
        check("nM = 2 with one loose: the loose jet plus the best light jet", r.nCandSets == 6 && close(r.chi2HH, e, 1e-6));
    }
    {
        std::vector<float> s(9, 0.01f); s[0] = 0.9f;                                            // nM = 1
        const Chi2Result r = chi2AN(all, s, L, M, mH, mZ);
        check("nM = 1: no reconstruction (-1)", r.chi2HH == -1.f && r.nCandSets == 0);
    }

    // ── 6. gen matching ─────────────────────────────────────────────────────
    {
        using namespace GenMatch;
        const int HP = kFromHardProcess, LC = kIsLastCopy;
        std::vector<GenP> gp = {
            /*0*/ {21, -1, 0, 0, 0, 0},                 // incoming gluon
            /*1*/ {25, 0, HP, 300, 0.1f, 0.1f},         // H, first copy
            /*2*/ {25, 1, HP | LC, 300, 0.1f, 0.1f},    // H, last copy
            /*3*/ {5, 2, HP, 150, 0.5f, 0.3f},          // b from H (first copy)
            /*4*/ {5, 3, HP | LC, 140, 0.52f, 0.31f},   // its last copy
            /*5*/ {-5, 2, HP | LC, 120, -0.4f, -0.2f},  // anti-b from H
            /*6*/ {6, 0, HP, 400, 1.0f, 2.0f},          // top
            /*7*/ {5, 6, HP | LC, 100, 1.3f, 2.4f},     // b from top
            /*8*/ {24, 6, HP | LC, 200, 0.9f, 1.8f},    // W+ from top
            /*9*/ {2, 8, HP | LC, 80, 0.7f, 1.5f},      // u from W
            /*10*/{-1, 8, HP | LC, 70, 1.1f, 1.95f},    // d-bar from W
            /*11*/{5, 0, LC, 60, -2.0f, 3.1f},          // a b NOT from the hard process (gluon splitting): no parton
            /*12*/{23, 0, HP | LC, 150, -1.0f, -2.9f},  // Z
            /*13*/{-5, 12, HP | LC, 75, -1.2f, 3.10f},  // b-bar from Z, near φ = +π
        };
        const std::vector<Parton> P = hardPartons(gp);
        int nH = 0, nT = 0, nW = 0, nZ = 0;
        for (const auto& p : P) { nH += p.label == kHiggsB; nT += p.label == kTopB; nW += p.label == kWq; nZ += p.label == kZq; }
        check("hardPartons: 2 H→b (last copies), 1 t→b, 2 W→q, 1 Z→b; the non-hard-process b is not a parton",
              P.size() == 6 && nH == 2 && nT == 1 && nW == 2 && nZ == 1);
        bool sameMother = false;
        for (const auto& p : P) if (p.idx == 4) for (const auto& q : P) if (q.idx == 5) sameMother = (p.motherIdx == 1 && q.motherIdx == 1);
        check("the two b of the H share mother = the first copy of the H (index 1)", sameMother);
        // jets: near b(H) 4, near b(t) 7, near u 9, near nothing, near the Z b across ±π, and a second jet near b(H) 4
        std::vector<std::pair<float, float>> jets = {{0.53f, 0.30f}, {1.31f, 2.41f}, {0.69f, 1.52f}, {2.2f, -1.0f},
                                                     {-1.21f, -3.12f}, {0.60f, 0.35f}};
        std::vector<int> lab, mom;
        matchJets(jets, P, 0.4f, lab, mom);
        check("labels: jet0 H→b (closest), jet1 t→b, jet2 W→q, jet3 none, jet4 Z→b across ±π, jet5 none (its b is taken)",
              lab == std::vector<int>({kHiggsB, kTopB, kWq, kNone, kZq, kNone}),
              "got " + [&] { std::string s; for (int x : lab) s += std::to_string(x) + " "; return s; }());
        check("mothers: H first copy 1, top 6, W 8, -1, Z 12, -1", mom == std::vector<int>({1, 6, 8, -1, 12, -1}));
        // [STEP 27 P] the top index: the b of the top and the two W quarks share the top's first copy (6); H, Z: -1
        bool topOk = true;
        for (const auto& p : P)
            topOk = topOk && (p.topIdx == ((p.label == kTopB || p.label == kWq) ? 6 : -1));
        check("top index of the partons: t->b and both W->q = 6 (the top's first copy), H->b and Z->b = -1", topOk);
        std::vector<int> lab2, mom2, top2;
        matchJets(jets, P, 0.4f, lab2, mom2, &top2);
        check("matchJets with the top output: the same labels and mothers; tops -1, 6, 6, -1, -1, -1",
              lab2 == lab && mom2 == mom && top2 == std::vector<int>({-1, 6, 6, -1, -1, -1}));
        // a W whose parent is not a top (e.g. a W of a Higgs decay): W->q label, top -1
        std::vector<GenP> gw = {{25, -1, HP | LC, 300, 0, 0}, {24, 0, HP | LC, 100, 0.2f, 0.2f},
                                {2, 1, HP | LC, 50, 0.3f, 0.3f}};
        const std::vector<Parton> PW = hardPartons(gw);
        check("a W->q quark whose W does not come from a top: label 3, top -1",
              PW.size() == 1 && PW[0].label == kWq && PW[0].topIdx == -1);
    }

    std::printf("PASS %d / FAIL %d\n", nPass, nFail);
    return nFail == 0 ? 0 : 1;
}
