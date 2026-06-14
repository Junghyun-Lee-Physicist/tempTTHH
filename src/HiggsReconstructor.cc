// =============================================================================
// HiggsReconstructor 구현 — 상세 설계는 include/HiggsReconstructor.h 참조
// [STEP6] docs/changes/STEP_6_higgs_reconstructor.md
// =============================================================================
#include "HiggsReconstructor.h"

#include <cmath>

// χ² 한 항: 기존 selectObjects 인라인 코드의 식 그대로 — **double로 반환**한다.
// 원본은 두 항(double)을 더한 뒤 한 번만 float로 내리므로, 항마다 float로
// 캐스팅하면 마지막 비트가 달라진다 (oracle 테스트에서 실측 — STEP_6 문서).
double HiggsReconstructor::chi2Term(const PairInfo& p, float target) {
    return std::pow(p.mass - target, 2) / std::pow(p.sumPt / 2.0f * 0.2f, 0.5f);
}

void HiggsReconstructor::consider(PairResult& r,
                                  const PairInfo& p1, const PairInfo& p2,
                                  float m1, float m2) {
    // double 합 → 단일 float 캐스팅 (원본 `float chi2 = powA + powB;`와 동일)
    const float chi2 = static_cast<float>(chi2Term(p1, m1) + chi2Term(p2, m2));
    if (!std::isfinite(chi2)) return;               // NaN/Inf 보호 (원본 동치)
    if (!r.valid() || chi2 < r.chi2) {              // 엄격 비교 — 동률은 최초 유지 (원본 동일)
        r.chi2  = chi2;
        r.mass1 = p1.mass;
        r.mass2 = p2.mass;
        r.pt1   = p1.pairPt;
        r.pt2   = p2.pairPt;
    }
}

void HiggsReconstructor::reconstruct(
        const std::vector<const TLorentzVector*>& bjets,
        float mH, float mZ) {

    _hh = PairResult{};
    _zh = PairResult{};
    _zz = PairResult{};

    const size_t nb = bjets.size();
    if (nb < 4) return;   // 조합 불가 — 모든 결과 invalid 유지

    // ─────────────────────────────────────────────────────────────────────
    // Step 1: pair cache (상삼각) — 기존 인라인 코드와 동일 구조.
    // flat (nb×nb) 벡터로 cache locality 확보.
    // ─────────────────────────────────────────────────────────────────────
    std::vector<PairInfo> pairCache(nb * nb);
    auto idx = [nb](size_t i, size_t j) -> size_t { return i * nb + j; };

    for (size_t i = 0; i < nb; ++i) {
        for (size_t j = i + 1; j < nb; ++j) {
            const TLorentzVector sum = *bjets[i] + *bjets[j];
            pairCache[idx(i, j)].mass   = static_cast<float>(sum.M());
            pairCache[idx(i, j)].sumPt  = static_cast<float>(
                bjets[i]->Pt() + bjets[j]->Pt());
            pairCache[idx(i, j)].pairPt = static_cast<float>(sum.Pt());
        }
    }

    // ─────────────────────────────────────────────────────────────────────
    // Step 2: C(N,4) × 3 pairings 한 번의 sweep — 모든 가설 동시 평가.
    // 순회 순서는 기존 인라인 코드와 동일: (i,j)+(k,l) → (i,k)+(j,l) → (i,l)+(j,k)
    // ─────────────────────────────────────────────────────────────────────
    auto tryPairing = [&](size_t a, size_t b, size_t c, size_t d) {
        const PairInfo& p1 = pairCache[idx(a, b)];
        const PairInfo& p2 = pairCache[idx(c, d)];

        consider(_hh, p1, p2, mH, mH);   // HH (대칭)
        consider(_zz, p1, p2, mZ, mZ);   // ZZ (대칭)
        consider(_zh, p1, p2, mZ, mH);   // ZH — 비대칭: 두 할당 모두
        consider(_zh, p2, p1, mZ, mH);   //      (pair2를 Z로 보는 경우)
    };

    for (size_t i = 0; i < nb - 3; ++i)
        for (size_t j = i + 1; j < nb - 2; ++j)
            for (size_t k = j + 1; k < nb - 1; ++k)
                for (size_t l = k + 1; l < nb; ++l) {
                    tryPairing(i, j, k, l);
                    tryPairing(i, k, j, l);
                    tryPairing(i, l, j, k);
                }
}
