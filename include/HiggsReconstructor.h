#pragma once
// =============================================================================
// HiggsReconstructor — b-jet 조합 기반 di-mother 재구성 (한 번의 sweep)
// -----------------------------------------------------------------------------
// [STEP6] 기존 selectObjects 인라인 HH 재구성(pair cache + C(N,4)×3 pairing)을
// 클래스로 추출하고, di-lepton analyzer의 다중 가설(HH/ZH/ZZ) 능력을 흡수한다.
//
// 핵심 개선 (vs di-lepton diMotherReco):
//   - di-lepton은 가설마다 전체 조합을 "재탐색" (HH 1회 + ZZ 1회 + HZ 1회 = 3중
//     루프 3번). 여기서는 pair cache를 만들고 **조합 sweep 한 번**에 모든 가설의
//     χ²를 동시에 평가한다 — 조합 수가 같으니 비용은 1/3.
//   - 비대칭 가설(ZH)은 pairing마다 (pair1↔pair2) 두 할당을 모두 평가
//     (대칭 가설 HH/ZZ에서는 중복이므로 생략).
//
// 동작 보존: HH 가설의 χ² 정의·갱신 조건·순회 순서는 기존 인라인 코드와
// **비트 동일** (docs/changes/STEP_6 oracle 테스트로 입증).
//   χ²(pair, m) = (m_pair − m)² / √(sumPt/2 × 0.2)      [20% 분해능 가정]
//
// 입력이 4개 미만이면 아무 것도 하지 않는다 (모든 결과 invalid).
// =============================================================================

#include <vector>
#include "TLorentzVector.h"

class HiggsReconstructor {
public:
    // 한 가설의 최적 결과. chi2 < 0 = 유효한 결과 없음 (sentinel).
    struct PairResult {
        float chi2  = -1.f;
        float mass1 = -1.f;   // 첫 후보 질량 (ZH에서는 Z 후보)
        float mass2 = -1.f;   // 둘째 후보 질량 (ZH에서는 H 후보)
        float pt1   = -1.f;   // 첫 후보(쌍) pT
        float pt2   = -1.f;
        bool valid() const { return chi2 >= 0.f; }
    };

    // bjets: 선택된 b-jet들의 p4 포인터 (소유권 없음).
    // mH/mZ: 목표 질량 (analyzer의 cHiggsMass / cZMass 전달).
    // 호출마다 내부 결과는 초기화된다.
    void reconstruct(const std::vector<const TLorentzVector*>& bjets,
                     float mH, float mZ);

    const PairResult& HH() const { return _hh; }   // (mH, mH)
    const PairResult& ZH() const { return _zh; }   // (mZ, mH) — 양방향 할당 평가
    const PairResult& ZZ() const { return _zz; }   // (mZ, mZ)

private:
    struct PairInfo {
        float mass;    // (b_i + b_j).M()
        float sumPt;   // pT_i + pT_j   (χ² 분모용)
        float pairPt;  // (b_i + b_j).Pt()
    };

    // χ² 한 항 — 기존 인라인 코드와 동일 식. double 반환 (두 항을 더한 뒤
    // 한 번만 float로 내려 원본과 비트 동일 — STEP_6 oracle 테스트)
    static double chi2Term(const PairInfo& p, float target);

    // (p1→m1, p2→m2) 할당의 χ²를 계산해 r보다 좋으면 갱신.
    // NaN/Inf χ²는 무시 (기존 코드의 "sentinel > NaN == false" 거동과 동치).
    static void consider(PairResult& r,
                         const PairInfo& p1, const PairInfo& p2,
                         float m1, float m2);

    PairResult _hh, _zh, _zz;
};
