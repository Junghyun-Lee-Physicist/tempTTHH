#ifndef ERACONFIG_H
#define ERACONFIG_H
// =============================================================================
//  EraConfig.h — single source of truth for every YEAR-DEPENDENT constant
// =============================================================================
//  목적 : 연도(2016preVFP/2016postVFP/2017/2018/2024)에 따라 달라지는 값을 한 곳에
//         모은다. 이 파일 밖에서 `if (year == "2017")` 를 쓰지 않는다.
//  대상 : 2018 UL 확장 작업자, 그리고 Run3 확장자.
//  상태 : DECIDED (2026-07-28). 2017 값은 기존 코드에서 그대로 옮겨온 것이며
//         2018 값은 ttHH AN-2022/122 v26 에 근거한다 (아래 각 항목의 출처 주석).
//         2024 (Run 3, NanoAOD v15) 는 2026-10-05 STEP 24 에 더했다: AN 에 2024 가
//         없으므로 값의 출처는 CVMFS jsonpog payload 의 실측(2026-10-02, PLAN §9.2 의
//         2024 열)과 사용자 결정(docs/DECISIONS.md D-2026-10-05-A)이다.
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
        "2016preVFP", "2016postVFP", "2017", "2018", "2024"
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
        "  Supported: 2016preVFP, 2016postVFP, 2017, 2018, 2024.\n"
        "  This is deliberately fatal: every year-dependent value in this\n"
        "  analysis fails SILENTLY when the year is unknown (MC weight 0,\n"
        "  wrong b-tag WP, zero triggered events). Refusing to continue.\n",
        y.c_str(), where);
    tthh::fatalExit(tthh::CONFIG_BAD_RUNINFO);
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
    // [STEP 24] jsonpog 의 2024 era 디렉터리(JME, BTV 의 `2024_Summer24`). GoldenJson/ 과
    //   DerivedCorr/PU/ 의 2024 디렉터리도 이 이름이다.
    if (year == "2024")        return "2024_Summer24";
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
    // [STEP 24] jsonpog 키(yearForCorr 의 값) 그대로면 그 연도: "2024_Summer24" -> "2024"
    for (const auto& v : validYears())
        if (in == yearForCorr(v)) return v;
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
    // [STEP 24] 2024: UParTAK4 (Jet_btagUParTAK4B) — BTV payload 실측(2026-10-02):
    //   POG/BTV/2024_Summer24/btagging.json.gz 의 UParTAK4_wp_values
    //   L 0.0246, M 0.1272, T 0.4648 (XT 0.6298, XXT 0.9739). AN 에는 2024 가 없다.
    if (year == "2024")        return {0.0246f, 0.1272f, 0.4648f};
    fatalYear(year, "btagWP");
}

// -----------------------------------------------------------------------------
// [STEP 24] Run 3 (NanoAOD v15) 인가 — branch 이름과 object 정의가 갈린다
//   (rho `Rho_fixedGridRhoFastjetAll`, MET `PuppiMET_pt`, b-tag `Jet_btagUParTAK4B`,
//    electron `Electron_mvaIso_WP90`, jet ID 는 jetid.json 으로 재계산, PU jet ID 없음).
//   2016-2018 은 지금 코드의 v9 이름 그대로다. 2018 v15 생산(ttHH2018UL_v15_had)은
//   이름이 v15 라서 지금 코드로는 필수 branch 검사(ttHHanalyzer_unified.cc
//   requireBranches_)가 FATAL 로 멈춘다 — 2018 v15 지원은 PLAN §9 Y3 (아직).
// -----------------------------------------------------------------------------
inline bool isRun3(const std::string& year) {
    if (year == "2024") return true;
    if (year == "2016preVFP" || year == "2016postVFP" || year == "2017" || year == "2018") return false;
    fatalYear(year, "isRun3");
}

// PU jet ID (CHS 의 Jet_puId, pT < 50 GeV) — Run 3 의 PUPPI jet 에는 없다 (PLAN §9.2, §7 D8)
inline bool usesPUJetID(const std::string& year) { return !isRun3(year); }

// jet ID 를 JME 의 jetid.json 으로 재계산하는가 (v15 에는 Jet_jetId 가 없다).
//   2024: POG/JME/2024_Summer24/jetid.json.gz 의 AK4PUPPI_TightLeptonVeto
//   (= v9 의 `Jet_jetId >= 6`, tight && tightLepVeto 에 해당; Cuts::jetID 주석).
inline bool jetIdFromJson(const std::string& year) { return isRun3(year); }

// -----------------------------------------------------------------------------
// [STEP 24] jet veto map (event veto) — Run 3 는 JME 가 모든 분석에 요구한다.
//   2024: POG/JME/2024_Summer24/jetvetomaps.json.gz, correction
//   `Summer24Prompt24_RunBCDEFGHI_V1`, type `jetvetomap` (파일 설명: data 와 MC 모두에
//   권고하는 map; PLAN §9.6 D11). Run 2 는 쓰지 않는다 (AN 에 없음; hemJes 주석).
//   veto 에 쓰는 jet 의 조건은 ttHHanalyzer_unified.cc 의 jetVetoed_ 에
//   (pT > 15 GeV, tight ID, EM 분율 < 0.9, PF muon 과 ΔR > 0.2 — JERC 권고로 확인할 것).
// -----------------------------------------------------------------------------
struct JetVetoMap { bool active; const char* correction; const char* type; };

inline JetVetoMap jetVetoMap(const std::string& year) {
    if (year == "2024") return {true, "Summer24Prompt24_RunBCDEFGHI_V1", "jetvetomap"};
    if (year == "2016preVFP" || year == "2016postVFP" || year == "2017" || year == "2018")
        return {false, "", ""};
    fatalYear(year, "jetVetoMap");
}

// b-tag shape SF (deepJet_shape) 가 있는가. 2024 BTV 에는 UParTAK4 의 shape SF 가
// 없다(fixed-WP kinfit, b jet 만; PLAN §9.6 D10) → 2024 는 아래의 fixed-WP 방법 (STEP 25 K).
inline bool hasBTagShapeSF(const std::string& year) { return !isRun3(year); }

// -----------------------------------------------------------------------------
// [STEP 25 K] b-tag SF 방법 (docs/DECISIONS.md D-2026-10-08-A: 사용자 결정 2026-10-08 — 2024 는 지금 fixed WP,
//   BTV 의 shape 보정 방법이 확인되면 그것으로 바꾼다)
//   Shape  : Run 2 — deepJet_shape (jet 마다 점수의 SF) + b-tag norm reweight (AN-2022/122). 그대로.
//   FixedWP: 2024 — BTV "method 1a" event weight, 분석의 WP 둘(b jet = 점수 >= M, light jet = 점수 < L)로:
//            jet 마다 그 구간(< L, L–M, >= M)의 확률의 Data/MC 비. P_MC 는 우리 MC 의 효율
//            (tools/stage7/btag_eff_maps.py 가 analyzer 의 BTagEff/ 히스토그램으로 만든 JSON,
//            yml path_btag_eff_json → env TTHH_BTAGEFF_JSON), P_Data = SF × 효율. b-tag 요구 전의 정규화를
//            지키므로 norm reweight 는 쓰지 않는다.
//            payload: 2024 의 b-tag SF 는 btagging_preliminary.json.gz 의 UParTAK4_kinfit 뿐 (b jet, flavour 5;
//            KNU 의 jsonpog 2026-10-08 확인: 2025-09-24 판 그대로). c·light jet 은 BTV 가 줄 때까지 SF 1.
// -----------------------------------------------------------------------------
enum class BTagMethod { Shape, FixedWP };
inline BTagMethod btagMethod(const std::string& year) {
    return isRun3(year) ? BTagMethod::FixedWP : BTagMethod::Shape;
}

struct BTagFixedWPPayload { const char* file; const char* correction; bool bJetsOnly; };
inline BTagFixedWPPayload btagFixedWPPayload(const std::string& year) {
    if (year == "2024") return {"btagging_preliminary.json.gz", "UParTAK4_kinfit", true};
    fatalYear(year, "btagFixedWPPayload");
}

// [STEP 24] tt+nb lookup (Expanded_genTtbarId, ttnb_<sample>.root; TTHHGenCategoryTools) 가 그 연도에
//   만들어져 있는가. 2017/2018 은 있다 -> ttbar stitching 집합의 샘플은 lookup 이 없으면 FATAL (기존 그대로).
//   2024 는 아직 없다(Summer24 에 extractTtbarIdPatch 를 돌리지 않았다) -> yml 이 null 이면 모든 샘플이
//   NanoAOD genTtbarId 로 돈다 (tt+nb 61/62/71/72 는 나뉘지 않고 51-55 에 남는다). PROVISIONAL,
//   docs/DECISIONS.md D-2026-10-05-C. 경로를 주면 2017/2018 과 같은 규칙 (없으면 FATAL).
inline bool hasTtNbLookup(const std::string& year) { return !isRun3(year); }

// -----------------------------------------------------------------------------
// [STEP 24] MET filter 목록 (D-2026-10-02-C, PLAN §9.2)
//   Run 2: 지금 코드의 목록 그대로 (MissingETOptionalFiltersRun2, UL 2017/2018).
//   2024 : Run 3 목록 — HBHENoise·HBHENoiseIso 는 빠지고 hfNoisyHits 가 들어간다
//          (목록은 기억 → JME 권고로 확인할 것, PLAN §9.2 의 MET filter 칸).
//   이름은 NanoAOD 의 Flag_* branch 이름이다. analyzer 는 이 목록으로 읽고(이름 -> member
//   표), 필수 branch 검사도 이 목록을 쓴다.
// -----------------------------------------------------------------------------
inline const std::vector<std::string>& metFilters(const std::string& year) {
    static const std::vector<std::string> run2 = {
        "Flag_goodVertices", "Flag_globalSuperTightHalo2016Filter", "Flag_HBHENoiseFilter",
        "Flag_HBHENoiseIsoFilter", "Flag_EcalDeadCellTriggerPrimitiveFilter", "Flag_BadPFMuonFilter",
        "Flag_BadPFMuonDzFilter", "Flag_eeBadScFilter", "Flag_ecalBadCalibFilter"};
    static const std::vector<std::string> run3 = {
        "Flag_goodVertices", "Flag_globalSuperTightHalo2016Filter", "Flag_EcalDeadCellTriggerPrimitiveFilter",
        "Flag_BadPFMuonFilter", "Flag_BadPFMuonDzFilter", "Flag_hfNoisyHitsFilter", "Flag_eeBadScFilter",
        "Flag_ecalBadCalibFilter"};
    return isRun3(year) ? run3 : run2;
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
    // [STEP 24] Run 3 에는 L1 prefiring 보정이 없고 2024 v15 에 branch 도 없다 (PLAN §9.2, 실측)
    if (year == "2024") return false;
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
    // [STEP 24] 2026-08-04 판 (md5 3f8543e8062915c9de97472e7dcf744f; GoldenJson/README.md,
    //   docs/reference/LUMI_SOURCES.md §6.1). 같은 이름의 2024-12-19 판이 EOS 에 남아 있으니 md5 로 확인.
    if (year == "2024")
        return "Cert_Collisions2024_378981_386951_Golden.json";
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
    // [STEP 24] 2024 의 Data JEC(Summer24Prompt24_V1_DATA)는 era 태그가 없고 residual 이
    //   입력 `run` 으로 갈린다 — era 문자는 쓰이지 않지만 그대로 돌려준다.
    if (year == "2024")
        return era;
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
    if (year == "2016preVFP" || year == "2016postVFP" || year == "2017" || year == "2024")
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
