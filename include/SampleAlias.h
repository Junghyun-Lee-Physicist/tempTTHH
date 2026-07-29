#ifndef SAMPLEALIAS_H
#define SAMPLEALIAS_H
// =============================================================================
//  SampleAlias.h — 구/신 샘플 이름 대응표 (단일 소스)
// =============================================================================
//  왜 필요한가
//  -----------
//  STEP18 에서 샘플 이름을 NtupleForge campaign 규칙으로 바꿨다:
//      TTToHadronic     -> TTbar_Hadronic
//      ttbb_2L2Nu       -> TTbb_DiLep
//      tt4b             -> TT4b        ...
//  그런데 **샘플 이름을 키로 조회하는 파생 산출물**은 구 이름으로 만들어져
//  있었고, 그 조회는 하나도 예외를 던지지 않는다:
//
//    1. `ExpandedTtbarId`  : `ttnb_<name>.root` 를 못 찾으면 INACTIVE
//                            -> 원본 genTtbarId 사용 -> tt+nb 카테고리 소멸
//    2. `StitchFactors`    : `samples[<name>]` 이 없으면 multiplier = 1.0
//                            -> stitching 미적용 -> tt+B 이중계수
//    3. 다운스트림 SF 패키지의 sample registry
//
//  1, 2 는 **완전 무증상**이다. 크래시도 경고도 없이 물리만 틀린다. 특히 2 는
//  inclusive ttbar 와 dedicated ttbb/tt4b 가 같은 위상공간을 두 번 채운다.
//
//  그래서 이름 대응을 여기 한 곳에 두고, 조회하는 쪽은 반드시
//  `candidates()` 를 돌려 신·구 이름을 모두 시도하게 한다.
//
//  ⚠ 이 표는 **읽기 호환성**을 위한 것이다. 새로 만드는 산출물은 항상 신 이름을
//    써야 한다. 2018 lookup(`Validation/lookup2018/`)은 이미 신 이름이라
//    alias 를 타지 않는다.
//
//  ⚠ 이 표는 "못 찾음"을 "찾음"으로 바꿔주지 않는다. 못 찾았을 때 조용히
//    넘어가는 걸 막는 것은 각 조회 지점의 책임이다 (아래 각 사용처 주석 참조).
// =============================================================================

#include <string>
#include <vector>

namespace SampleAlias {

// ─────────────────────────────────────────────────────────────────────────────
// (1) ttbar stitching 집합 — 신(canonical) -> 구(legacy)
//     이 7개는 stitch_factors_2017.json 과 Validation/lookup/ttnb_*.root 의
//     키이기도 하다. **이 목록만** mustBeInStitchPlan() 을 참이 되게 한다.
//     여기에 다른 샘플을 넣으면 analyzer 가 그 샘플에 대해서도 stitch/lookup
//     부재를 FATAL 로 취급하게 되므로 절대 확장하지 말 것.
// ─────────────────────────────────────────────────────────────────────────────
inline const std::vector<std::pair<std::string, std::string>>& stitchPlanTable() {
    static const std::vector<std::pair<std::string, std::string>> t = {
        {"TTbar_Hadronic", "TTToHadronic"},
        {"TTbar_SemiLep",  "TTToSemiLeptonic"},
        {"TTbar_DiLep",    "TTTo2L2Nu"},
        {"TTbb_Hadronic",  "ttbb_Hadronic"},
        {"TTbb_SemiLep",   "ttbb_SemiLeptonic"},
        {"TTbb_DiLep",     "ttbb_2L2Nu"},
        {"TT4b",           "tt4b"},
    };
    return t;
}

// ─────────────────────────────────────────────────────────────────────────────
// (2) 이름 해석용 **전체** 대응표 = (1) + stitching 과 무관한 개명들.
//     아래쪽 항목들은 2017 prescan_summary.json 이 STEP18 이전 이름으로 남아
//     있어서 필요하다. 순수 조회용이며 물리적 의미는 없다.
//     ⚠ 여기 추가해도 mustBeInStitchPlan() 은 영향을 받지 않는다 — 의도된 분리.
// ─────────────────────────────────────────────────────────────────────────────
inline const std::vector<std::pair<std::string, std::string>>& table() {
    static const std::vector<std::pair<std::string, std::string>> t = [] {
        std::vector<std::pair<std::string, std::string>> v = stitchPlanTable();
        const std::vector<std::pair<std::string, std::string>> extra = {
            {"TTHHto4b",  "ttHH"},
            {"ttHTobb",   "ttHtobb"},
            {"TTZHTo4b",  "ttZHto4b"},
            {"TTZZTo4b",  "ttZZto4b"},
            {"TTZToBB",   "ttZtobb"},
            {"TTWH",      "ttWH"},
            {"TTWW",      "ttWW"},
            {"TTWZ",      "ttWZ"},
            {"TTTW",      "tttW"},
            {"TTTT",      "tttt"},
        };
        v.insert(v.end(), extra.begin(), extra.end());
        return v;
    }();
    return t;
}

// name 의 legacy 대응 이름 (없으면 빈 문자열)
inline std::string legacy(const std::string& name) {
    for (const auto& kv : table()) if (kv.first == name) return kv.second;
    return "";
}

// name 의 canonical 대응 이름 (없으면 빈 문자열) — 역방향 조회용
inline std::string canonical(const std::string& name) {
    for (const auto& kv : table()) if (kv.second == name) return kv.first;
    return "";
}

// 조회 시 시도해야 하는 이름들: [원본, 반대쪽 표기] (중복 없음)
inline std::vector<std::string> candidates(const std::string& name) {
    std::vector<std::string> out{name};
    const std::string l = legacy(name);
    if (!l.empty() && l != name) out.push_back(l);
    const std::string c = canonical(name);
    if (!c.empty() && c != name) out.push_back(c);
    return out;
}

// 이 샘플이 ttbar stitching 계획에 반드시 들어가야 하는가.
//   여기에 해당하는데 stitch/lookup 조회가 실패하면 그건 설정 오류이지
//   "이 샘플은 원래 없음" 이 아니다. 호출부가 FATAL 을 낼 근거로 쓴다.
inline bool mustBeInStitchPlan(const std::string& name) {
    for (const auto& kv : stitchPlanTable())     // ← table() 아님. 확장 금지.
        if (kv.first == name || kv.second == name) return true;
    return false;
}

} // namespace SampleAlias

#endif // SAMPLEALIAS_H
