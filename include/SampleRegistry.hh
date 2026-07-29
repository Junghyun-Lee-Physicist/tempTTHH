#ifndef SAMPLEREGISTRY_HH
#define SAMPLEREGISTRY_HH
// =============================================================================
//  SampleRegistry.hh — 다운스트림 SF 도구용 sample 메타데이터 단일 소스
// =============================================================================
//  무엇을 대체하는가
//  ------------------
//  `TriggerStudy/include/Config.hh` 와 `bTagSF_ReweightStudy/include/Config.hh`
//  에 각각 박혀 있던 `SampleRegistry()` 하드코딩 표를 대체한다. 그 표는
//    (a) 구 dataset(STEP18 이전) 기준으로 계산된 magic number 였고,
//    (b) 두 파일에 **복사본**으로 존재했으며,
//    (c) `const inline` 이라 값 하나 고치는 데 재빌드가 필요했다.
//
//  왜 이게 조용한 버그였는가
//  --------------------------
//  weight = lumi × σ × BR / Σgenw 이다. campaign 이 `ttHH2017UL_fullNano_v20`
//  으로 바뀌면서 Σgenw(= `runs.genEventSumw`)가 전부 달라졌는데 표는 그대로였다.
//  trigger SF 도 b-tag norm reweight 도 **비율**이라 공통 인수(lumi)는 상쇄되지만,
//  Σgenw 는 샘플마다 다르므로 상쇄되지 않는다. 즉 group 안에서 샘플들의 **상대
//  비중**이 틀어지고, 그 결과는 크래시 없이 "그럴듯한 숫자"로 나온다.
//
//  그래서 여기서는 analyzer 제출기(`submit_job_FH_Tier3_unified.py`
//  `_compute_base_weight()`)와 **완전히 같은 두 파일에서, 같은 공식으로**
//  런타임에 합성한다. 재빌드 없이 prescan 만 갱신하면 따라온다.
//
//      w = lumi_fb_inv × cross_section_fb × br × kfactor / runs.genEventSumw
//      Data (cross_section_fb == null) → w = 1.0
//
//  경로 (전부 환경변수로 override 가능)
//  ------------------------------------
//    TTHH_BASE      analyzer 프로젝트 루트         (default: "..")
//    TTHH_XSEC_DB   xsec_db json                  (default: $TTHH_BASE/data/samples_2017UL.json)
//    TTHH_PRESCAN   prescan summary json          (default: $TTHH_BASE/prescan_summary/prescan_summary.json)
//    TTHH_LUMI_FB   lumi override [fb^-1]         (default: xsec_db `_meta.lumi_fb_inv`)
//
//  이름 규칙
//  ----------
//  canonical = NtupleForge campaign 이름 (`TTbar_Hadronic`, `SingleMuon_Run2017B`).
//  구 이름(`TTToHadronic`)과 짧은 Data 이름(`SingleMuon_B`)도 **입력으로는** 받아
//  주되(SampleAlias.h + expandDataName()), 못 찾으면 조용히 넘어가지 않고 FATAL.
//
//  ⚠ 설계 원칙: 이 헤더의 모든 실패 경로는 exit(1) 이다. "모르는 샘플 → weight 1.0"
//    같은 관대한 기본값을 두지 않는다. 이 도구들이 만드는 산출물(SF/reweight JSON)은
//    이후 전 분석에 곱해지므로, 조용히 틀린 것보다 멈추는 편이 압도적으로 싸다.
// =============================================================================

#include "SampleAlias.h"

#include <nlohmann/json.hpp>

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>

namespace SampleRegistry {

// ─────────────────────────────────────────────────────────────────────────────
struct Info {
    bool        isData = false;
    double      weight = 1.0;   // MC: lumi*σ*BR*k/Σgenw   Data: 1.0
    std::string dataset;        // Data 만: "SingleMuon" / "JetHT" / "BTagCSV"
    std::string era;            // Data 만: "B".."F"  (MC 는 빈 문자열)
    std::string canonical;      // 조회에 실제로 쓰인 canonical 이름
    std::string provenance;     // weight 유도 근거 (로그용)
};

namespace detail {

[[noreturn]] inline void fatal(const std::string& msg) {
    std::cerr << "\n[FATAL][SampleRegistry] " << msg << "\n" << std::endl;
    std::exit(1);
}

inline std::string env(const char* key, const std::string& fallback) {
    const char* v = std::getenv(key);
    return (v && *v) ? std::string(v) : fallback;
}

inline std::string baseDir() { return env("TTHH_BASE", ".."); }

inline nlohmann::json readJson(const std::string& path, const char* what) {
    std::ifstream f(path);
    if (!f.is_open())
        fatal(std::string(what) + " not found: " + path +
              "\n  Set the corresponding env var (TTHH_BASE / TTHH_XSEC_DB / TTHH_PRESCAN).");
    try {
        nlohmann::json j;
        f >> j;
        return j;
    } catch (const std::exception& e) {
        fatal(std::string(what) + " is not valid JSON: " + path + "\n  " + e.what());
    }
}

// ── Data 짧은 이름 보정 : "SingleMuon_B" -> "SingleMuon_Run2017B" ───────────
//   두 SF 패키지의 옛 스크립트가 짧은 이름을 쓴다. 새 skim 은 campaign 이름을
//   쓰므로, 입력 호환을 위해 확장 후보를 만들어 준다.
inline std::vector<std::string> expandDataName(const std::string& name) {
    static const std::regex kShort(R"(^([A-Za-z]+)_([A-Z])$)");
    std::smatch m;
    if (!std::regex_match(name, m, kShort)) return {};
    // 연도는 xsec_db 에 실제로 존재하는 키로 확정한다 (여기서는 후보만 나열).
    return {m[1].str() + "_Run2017" + m[2].str(),
            m[1].str() + "_Run2018" + m[2].str()};
}

// ── Data era 추출 : "SingleMuon_Run2017B" -> dataset="SingleMuon", era="B" ──
//   제출기(`Run\d{4}([A-Z])$`)와 동일한 정규식을 쓴다. 이전 다운스트림 코드는
//   "첫 '_' 에서 자르기" 였고, 그러면 era 가 "Run2017B" 가 되어 `era=="B"` 비교가
//   항상 거짓 → Run B 이벤트를 CDEF trigger bit 로 평가 → passHadTrig 불일치로
//   exit(1) 이었다.
inline bool parseDataName(const std::string& name,
                          std::string& dataset, std::string& era) {
    static const std::regex kRe(R"(^(.+)_Run\d{4}([A-Z])$)");
    std::smatch m;
    if (std::regex_match(name, m, kRe)) { dataset = m[1]; era = m[2]; return true; }
    static const std::regex kShort(R"(^(.+)_([A-Z])$)");
    if (std::regex_match(name, m, kShort)) { dataset = m[1]; era = m[2]; return true; }
    return false;
}

// ── 실제 로딩 ───────────────────────────────────────────────────────────────
struct Db {
    nlohmann::json xsec;
    nlohmann::json prescan;   // 이미 `["samples"]` 로 내려간 객체
    double         lumi = 0.0;
    std::string    xsecPath, prescanPath;
};

inline const Db& db() {
    static Db d = [] {
        Db out;
        out.xsecPath    = env("TTHH_XSEC_DB", baseDir() + "/data/samples_2017UL.json");
        out.prescanPath = env("TTHH_PRESCAN",
                              baseDir() + "/prescan_summary/prescan_summary.json");

        out.xsec = readJson(out.xsecPath, "xsec_db");

        nlohmann::json pre = readJson(out.prescanPath, "prescan summary");
        if (!pre.contains("samples"))
            fatal("prescan summary has no 'samples' object: " + out.prescanPath +
                  "\n  Regenerate it with consolidate_prescan.py.");
        out.prescan = pre["samples"];

        const char* lumiEnv = std::getenv("TTHH_LUMI_FB");
        if (lumiEnv && *lumiEnv) {
            out.lumi = std::atof(lumiEnv);
        } else {
            if (!out.xsec.contains("_meta") || !out.xsec["_meta"].contains("lumi_fb_inv"))
                fatal("xsec_db has no `_meta.lumi_fb_inv` and TTHH_LUMI_FB is unset: "
                      + out.xsecPath);
            out.lumi = out.xsec["_meta"]["lumi_fb_inv"].get<double>();
        }
        if (out.lumi <= 0.0)
            fatal("lumi_fb_inv must be > 0 (got " + std::to_string(out.lumi) + ").");

        std::cout << "[SampleRegistry] xsec_db  : " << out.xsecPath  << "\n"
                  << "[SampleRegistry] prescan  : " << out.prescanPath << "\n"
                  << "[SampleRegistry] lumi     : " << out.lumi << " fb^-1\n";
        return out;
    }();
    return d;
}

// name 을 xsec_db 에 실제로 존재하는 키로 해석한다 (alias / 짧은 Data 이름 포함).
inline std::string resolveKey(const std::string& name, std::vector<std::string>& tried) {
    auto push = [&](const std::string& k) {
        for (const auto& t : tried) if (t == k) return;
        tried.push_back(k);
    };
    for (const auto& c : SampleAlias::candidates(name)) push(c);
    for (const auto& c : expandDataName(name))           push(c);

    for (const auto& k : tried)
        if (db().xsec.contains(k)) return k;
    return "";
}

inline Info build(const std::string& requested, const std::string& key) {
    const nlohmann::json& rec = db().xsec.at(key);

    Info info;
    info.canonical = key;

    const bool isData = rec.value("is_data", false) ||
                        rec.value("cross_section_fb", nlohmann::json()).is_null();
    info.isData = isData;

    if (isData) {
        info.weight = 1.0;
        if (!parseDataName(key, info.dataset, info.era))
            fatal("Data sample '" + key + "' does not match <PD>_Run<YYYY><E>.\n"
                  "  The hadronic trigger bit set (B vs CDEF) is selected from the era\n"
                  "  letter, so an unparseable name would silently evaluate Run B with\n"
                  "  the CDEF bits.");
        info.provenance = "data (weight=1)";
        return info;
    }

    // ── MC : 제출기와 동일한 공식 ──────────────────────────────────────────
    // xsec_db 와 prescan 은 **서로 다른 시점에 만들어진 별개 파일**이라 이름
    // 규칙이 어긋날 수 있다 (xsec_db 는 STEP18 신 이름, 2017 prescan 은 구 이름).
    // 그래서 키를 각각 독립적으로 해석한다.
    std::string preKey;
    for (const auto& c : SampleAlias::candidates(key))
        if (db().prescan.contains(c)) { preKey = c; break; }
    if (preKey.empty()) {
        std::string tried;
        for (const auto& c : SampleAlias::candidates(key))
            tried += (tried.empty() ? "" : ", ") + c;
        fatal("MC sample '" + key + "' (requested as '" + requested +
              "') is not in the prescan summary (" + db().prescanPath + ").\n"
              "  tried keys: " + tried + "\n"
              "  Σgenw is unknown, so its weight cannot be synthesised. Neither a\n"
              "  guessed value nor 1.0 is acceptable here: the b-tag reweight and the\n"
              "  trigger SF are ratios of SUMS over a process group, so a wrong Σgenw\n"
              "  silently re-weights that sample's share of the group.\n"
              "  Fix: run prescan for this sample, then consolidate_prescan.py.");
    }
    if (preKey != key)
        std::cout << "[SampleRegistry]   prescan key via legacy alias: '"
                  << preKey << "'\n";

    const nlohmann::json& prec = db().prescan.at(preKey);
    if (!prec.contains("runs") || !prec["runs"].contains("genEventSumw"))
        fatal("prescan record for '" + preKey + "' has no runs.genEventSumw.");

    const double sumw = prec["runs"]["genEventSumw"].get<double>();
    if (sumw <= 0.0)
        fatal("prescan for '" + preKey + "': runs.genEventSumw = " +
              std::to_string(sumw) + " <= 0 (invalid prescan).");

    const double xsecFb = rec.at("cross_section_fb").get<double>();
    const double br     = rec.contains("br") && !rec["br"].is_null()
                          ? rec["br"].get<double>() : 1.0;
    const double kfac   = rec.contains("kfactor") && !rec["kfactor"].is_null()
                          ? rec["kfactor"].get<double>() : 1.0;

    info.weight = db().lumi * xsecFb * br * kfac / sumw;

    // runs vs tree 합 불일치는 제출기와 같은 기준으로 경고만 (사용은 runs)
    if (prec.contains("events") && prec["events"].contains("sumGenW_total")) {
        const double sumwTree = prec["events"]["sumGenW_total"].get<double>();
        if (sumwTree > 0.0 && std::abs(sumw - sumwTree) / sumw > 1e-4)
            std::cerr << "[SampleRegistry][warn] " << key << ": runs.genEventSumw="
                      << sumw << " vs events.sumGenW_total=" << sumwTree
                      << " differ > 0.01% (using runs)\n";
    }

    info.provenance = "xsec_fb=" + std::to_string(xsecFb) +
                      " * br="   + std::to_string(br) +
                      " * k="    + std::to_string(kfac) +
                      " * lumi=" + std::to_string(db().lumi) +
                      " / sumGenW=" + std::to_string(sumw);
    return info;
}

inline std::map<std::string, Info>& cache() {
    static std::map<std::string, Info> c;
    return c;
}

} // namespace detail

// ─────────────────────────────────────────────────────────────────────────────
/// 샘플 정보 조회. 모르는 샘플이면 **FATAL** (nullptr 을 돌려주지 않는다).
inline const Info& get(const std::string& name) {
    auto it = detail::cache().find(name);
    if (it != detail::cache().end()) return it->second;

    std::vector<std::string> tried;
    const std::string key = detail::resolveKey(name, tried);
    if (key.empty()) {
        std::string list;
        for (const auto& t : tried) list += (list.empty() ? "" : ", ") + t;
        detail::fatal("unknown sample '" + name + "'.\n"
                      "  tried keys: " + list + "\n"
                      "  Not present in xsec_db (" + detail::db().xsecPath + ").\n"
                      "  Sample names must follow the NtupleForge campaign convention\n"
                      "  (e.g. TTbar_Hadronic, TTbb_DiLep, SingleMuon_Run2017B).");
    }

    Info info = detail::build(name, key);
    if (info.canonical != name)
        std::cout << "[SampleRegistry] '" << name << "' resolved to '"
                  << info.canonical << "'\n";
    std::cout << "[SampleRegistry] " << name << " : "
              << (info.isData ? "Data" : "MC") << ", weight=" << info.weight
              << "  (" << info.provenance << ")\n";

    return detail::cache().emplace(name, std::move(info)).first->second;
}

/// xsec_db 에 등록된 MC 샘플 전체 (campaign 이름 기준, `_ext*` 제외).
inline std::vector<std::string> mcSampleNames() {
    std::vector<std::string> out;
    for (auto it = detail::db().xsec.begin(); it != detail::db().xsec.end(); ++it) {
        if (it.key() == "_meta") continue;
        // `*_ext1/_ext2` 는 별도 dataset 이 아니라 base 의 확장이다.
        // make_filelists.py 가 base filelist 에 합치므로 여기서는 제외한다.
        if (it.key().find("_ext") != std::string::npos) continue;
        const auto& rec = it.value();
        const bool isData = rec.value("is_data", false) ||
                            rec.value("cross_section_fb", nlohmann::json()).is_null();
        if (!isData) out.push_back(it.key());
    }
    return out;
}

/// Data 샘플 전체. `excludePD` 에 들어간 primary dataset 은 제외
/// (예: b-tag reweight 는 SingleMuon 을 쓰지 않는다).
inline std::vector<std::string> dataSampleNames(
        const std::vector<std::string>& excludePD = {}) {
    std::vector<std::string> out;
    for (auto it = detail::db().xsec.begin(); it != detail::db().xsec.end(); ++it) {
        if (it.key() == "_meta") continue;
        const auto& rec = it.value();
        const bool isData = rec.value("is_data", false) ||
                            rec.value("cross_section_fb", nlohmann::json()).is_null();
        if (!isData) continue;
        bool skip = false;
        for (const auto& pd : excludePD)
            if (it.key().rfind(pd, 0) == 0) { skip = true; break; }
        if (!skip) out.push_back(it.key());
    }
    return out;
}

} // namespace SampleRegistry

#endif // SAMPLEREGISTRY_HH
