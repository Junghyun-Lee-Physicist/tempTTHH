#ifndef ERACONFIG_H
#define ERACONFIG_H
// =============================================================================
//  EraConfig.h — single source of truth for every YEAR-DEPENDENT constant
// =============================================================================
//  목적 : 연도(2016preVFP/2016postVFP/2017/2018)에 따라 달라지는 값을 한 곳에
//         모은다. 이 파일 밖에서 `if (year == "2017")` 를 쓰지 않는다.
//  대상 : 2018 UL 확장 작업자, 그리고 Run3 확장자.
//  상태 : DECIDED (2026-07-28). 2017 값은 기존 코드에서 그대로 옮겨온 것이며
//         2018 값은 ttHH AN-2022/122 v26 에 근거한다 (아래 각 항목의 출처 주석).
//
//  왜 이 파일이 생겼나
//  -------------------
//  2018 확장 전, 연도 의존 값이 5개 파일에 흩어져 있었고 그중 하나
//  (`ttHHanalyzer_unified.h:1135`)는 `if(year=="2017")` 에 **else 가 없어서**
//  다른 연도를 주면 조용히 빈 문자열이 되어 jsonpog 경로가 깨졌다. 나머지도
//  전부 "2018 을 주면 조용히 틀린 값" 이 되는 구조였다:
//    * L1 prefiring   : 2018 NanoAOD 에 branch 가 없어 0 → **모든 MC weight 0**
//    * DeepJet WP     : 2017 값이 그대로 적용 → **신호영역 정의가 달라짐**
//    * golden JSON    : 파일명에 2017 run range 하드코딩 → 2018 Data 전량 실패
//    * HLT path       : 2017 CSV path 가 2018 에 없어 → **경고 없이 0 event**
//  공통점은 전부 **무증상**이라는 것이다. 그래서 이 파일의 조회 함수는
//  **알 수 없는 연도에 대해 반드시 FATAL** 이다. 기본값으로 넘어가지 않는다.
//
//  참고 : 값을 바꿀 때는 AN 근거를 주석에 갱신한다 (프로젝트 규약).
// =============================================================================

#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cctype>

#include "ExitCodes.h"

namespace EraConfig {

// -----------------------------------------------------------------------------
// 허용 연도 — 이 목록에 없으면 어디서든 FATAL (P0' #8)
// -----------------------------------------------------------------------------
inline const std::vector<std::string>& validYears() {
    static const std::vector<std::string> v = {
        "2016preVFP", "2016postVFP", "2017", "2018"
    };
    return v;
}

inline bool isValidYear(const std::string& y) {
    for (const auto& v : validYears()) if (v == y) return true;
    return false;
}

[[noreturn]] inline void fatalYear(const std::string& y, const char* where) {
    std::fprintf(stderr,
        "\n[EraConfig] FATAL: unsupported runYear '%s' (asked by %s).\n"
        "  Supported: 2016preVFP, 2016postVFP, 2017, 2018.\n"
        "  This is deliberately fatal: every year-dependent value in this\n"
        "  analysis fails SILENTLY when the year is unknown (MC weight 0,\n"
        "  wrong b-tag WP, zero triggered events). Refusing to continue.\n",
        y.c_str(), where);
    std::exit(tthh::CONFIG_BAD_RUNINFO);
}

// -----------------------------------------------------------------------------
// jsonpog / CorrectionsManager 가 쓰는 연도 키
//   기존 `ttHHanalyzer_unified.h:1135` 의 하드코딩을 대체한다.
//   CorrectionsManager 는 이미 이 4개 값을 모두 처리한다
//   (`include/CorrectionsManager.h:43`, `src/CorrectionsManager.cc:116-199`).
// -----------------------------------------------------------------------------
inline std::string yearForCorr(const std::string& year) {
    if (year == "2016preVFP")  return "2016preVFP_UL";
    if (year == "2016postVFP") return "2016postVFP_UL";
    if (year == "2017")        return "2017_UL";
    if (year == "2018")        return "2018_UL";
    fatalYear(year, "yearForCorr");
}

// -----------------------------------------------------------------------------
// 역방향 정규화 : "2018_UL" / "2016PreVFP_UL" / "2018" -> 표준 연도 문자열
//
//   CorrectionsManager 는 내부적으로 `runYear_` 에 `yearForCorr()` 결과("2018_UL")
//   를 들고 있어서, 연도 조회를 하려면 되돌려야 한다.
//   ⚠ 대소문자까지 흡수하는 이유: 리포지토리 안에 `2016PreVFP_UL`(대문자 P)와
//     `2016preVFP_UL`(소문자 p) 이 섞여 있었다. 디스크의 GoldenJson 디렉토리는
//     소문자인데 CorrectionsManager 의 map 키는 대문자라, 2016 은 어떤 키에도
//     매칭되지 않고 조용히 `return` 해서 golden JSON 없이 돌 수 있었다.
//     여기서 한 번에 흡수하고, 표준형은 소문자 `2016preVFP` 로 통일한다.
// -----------------------------------------------------------------------------
inline std::string normalizeYear(const std::string& in) {
    std::string s = in;
    // "_UL" / "UL_" 접미·접두 제거
    const std::string suf = "_UL";
    if (s.size() > suf.size() && s.compare(s.size()-suf.size(), suf.size(), suf) == 0)
        s.erase(s.size()-suf.size());
    // 대소문자 무시 비교로 표준형 찾기
    auto lower = [](std::string t){ for (auto& c : t) c = (char)std::tolower((unsigned char)c); return t; };
    const std::string sl = lower(s);
    for (const auto& v : validYears())
        if (lower(v) == sl) return v;
    fatalYear(in, "normalizeYear");
}

// -----------------------------------------------------------------------------
// DeepJet b-tag working points
//   출처: ttHH AN-2022/122 v26, Table 32 "DeepJet Tagger Working Point" (p.41).
//   AN 표 전체를 그대로 옮긴다 (2016 두 개 포함) — 부분만 옮기면 다음 사람이
//   다시 AN 을 찾아야 한다.
//
//     WP        2016preVFP  2016postVFP  2017     2018
//     Loose     0.0508      0.0480       0.0532   0.0490
//     Medium    0.2598      0.2489       0.3040   0.2783
//     Tight     0.6502      0.6377       0.7476   0.7100
//
//   ⚠ Medium 이 signal region 정의(b-tag 다중도)를 직접 결정한다. 값을 바꾸면
//     물리 결과가 바뀌므로 반드시 고지 + CHANGELOG 기록.
// -----------------------------------------------------------------------------
struct BTagWP { float loose; float medium; float tight; };

inline BTagWP btagWP(const std::string& year) {
    if (year == "2016preVFP")  return {0.0508f, 0.2598f, 0.6502f};
    if (year == "2016postVFP") return {0.0480f, 0.2489f, 0.6377f};
    if (year == "2017")        return {0.0532f, 0.3040f, 0.7476f};
    if (year == "2018")        return {0.0490f, 0.2783f, 0.7100f};
    fatalYear(year, "btagWP");
}

// -----------------------------------------------------------------------------
// L1 ECAL prefiring
//   출처: ttHH AN-2022/122 v26 p.118 (systematics, L1 prefiring):
//     "Note that this effect does not affect 2018 data-taking era."
//   구현상 더 중요한 이유: 2018 NanoAOD 에는 `L1PreFiringWeight_Nom` branch 가
//   아예 없다. eventBuffer 는 없는 branch 를 조용히 0 으로 두므로
//   (`include/eventBuffer.h:10092-10121`), 연도 게이트 없이 곱하면
//   **모든 MC 이벤트 weight 가 0** 이 된다. 완전 무증상.
// -----------------------------------------------------------------------------
inline bool usesL1Prefiring(const std::string& year) {
    if (year == "2016preVFP" || year == "2016postVFP" || year == "2017") return true;
    if (year == "2018") return false;
    fatalYear(year, "usesL1Prefiring");
}

// -----------------------------------------------------------------------------
// Golden JSON 파일명
//   기존 코드는 `Cert_294927-306462_...` 를 **파일명에 하드코딩**했다
//   (`src/CorrectionsManager.cc:248-250`). run range 는 연도마다 다르므로
//   2018 Data 는 파일을 못 찾아 전량 FATAL(E41) 이었다.
//   디렉토리(`GoldenJson/<yearForCorr>/`)는 이미 연도 키였다.
//   실제 파일명은 리포지토리의 GoldenJson/ 아래 파일과 일치해야 한다.
// -----------------------------------------------------------------------------
inline std::string goldenJsonFile(const std::string& year) {
    if (year == "2016preVFP" || year == "2016postVFP")
        return "Cert_271036-284044_13TeV_Legacy2016_Collisions16_JSON.txt";
    if (year == "2017")
        return "Cert_294927-306462_13TeV_UL2017_Collisions17_GoldenJSON.txt";
    if (year == "2018")
        return "Cert_314472-325175_13TeV_Legacy2018_Collisions18_JSON.txt";
    fatalYear(year, "goldenJsonFile");
}

// -----------------------------------------------------------------------------
// Data JEC 의 era 태그
//   2017 은 era 문자를 그대로 쓴다 (B..F).
//   2018 은 JEC 가 RunA / RunB / RunC / RunD 로 제공된다. 기존 코드
//   (`src/CorrectionsManager.cc:167-168`)는 `(era=="A") ? "A" : "D"` 로
//   **B/C 에 RunD JEC 를 적용**하고 있었다 (그리고 키가 없으면 try/catch 가
//   조용히 MC JEC 로 fallback 했다).
// -----------------------------------------------------------------------------
inline std::string jecDataEraTag(const std::string& year, const std::string& era) {
    if (year == "2016preVFP" || year == "2016postVFP" || year == "2017" ||
        year == "2018")
        return era;              // 연도 불문 era 문자 그대로 — 축약 금지
    fatalYear(year, "jecDataEraTag");
}

// -----------------------------------------------------------------------------
// HEM 15/16 (2018 전용) — **veto 가 아니라 JES 변주**
//   출처: ttHH AN-2022/122 v26 p.118:
//     "we apply an energy variation on 2018 MC samples of 20% for jets with
//      -1.57 < phi < -0.87 and -2.5 < eta < -1.3, and a 35% energy variation
//      for jets with -1.57 < phi < -0.87 and -3.0 < eta < -2.5"
//     "The effect from failure of HEM15/16 ... was found to be negligible"
//
//   ⚠ JME 의 jet veto map (Summer19UL18_V1 / h2hot_ul18_plus_hem1516_plus_hbp2m1)
//     은 **이 분석이 어느 연도에도 쓰지 않는다** (AN 214쪽 전문에 veto map 언급
//     0건, 코드에 loader 없음). 2018 에만 도입하면 2017 과 위상공간이 달라지고,
//     TWiki 자체가 "multiple jets 분석에서는 통계 손실을 고려하라" 고 경고한다
//     (이 분석은 >=6 jet). 최종 결과 전 재검토 항목으로 docs 에 남겼다.
// -----------------------------------------------------------------------------
struct HemJesRegion {
    bool  active;       // 이 연도에 적용되는가
    float phiLo, phiHi;
    float etaLoInner, etaHiInner; float fracInner;   // -2.5 < eta < -1.3 : 20%
    float etaLoOuter, etaHiOuter; float fracOuter;   // -3.0 < eta < -2.5 : 35%
};

inline HemJesRegion hemJes(const std::string& year) {
    if (year == "2018")
        return {true, -1.57f, -0.87f, -2.5f, -1.3f, 0.20f, -3.0f, -2.5f, 0.35f};
    if (year == "2016preVFP" || year == "2016postVFP" || year == "2017")
        return {false, 0, 0, 0, 0, 0, 0, 0, 0};
    fatalYear(year, "hemJes");
}

// jet 이 HEM 영역 안인가 (2D map 히스토그램 채우기 / JES 변주 공용)
inline bool inHemRegion(const std::string& year, float eta, float phi) {
    const HemJesRegion r = hemJes(year);
    if (!r.active) return false;
    if (phi <= r.phiLo || phi >= r.phiHi) return false;
    return (eta > r.etaLoOuter && eta < r.etaHiInner);   // -3.0 < eta < -1.3
}

} // namespace EraConfig

#endif // ERACONFIG_H
