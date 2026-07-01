// ============================================================================
// Config_TtCatGroup.hh
// ----------------------------------------------------------------------------
// Process group mapping + sample stitching helper for ttHH(4b) FH analysis.
//
// Used by:
//   - B-tag SF reweight tool (Config.hh, BTagSFProcessor, makeReweightJSON.cpp)
//   - Main analyzer (ttHHanalyzer_unified.cc / CorrectionsManager)
//   - (planned) Stack plotter (when stitching is integrated downstream)
//
// === Methodology ===
// ttH AN-19-094 §A.2.1 derives the b-tag normalization SF *per process group*:
//   - 4 main groups: ttH, tt+bb, tt+cc, tt+LF
//   - Minor backgrounds (V+jets, single t, ttV, diboson, QCD) borrow tt+LF
//     "due to poor statistics of simulated events"
//
// In ttHH(4b) we extend this with self-measured groups for high-b-jet
// processes that have sufficient statistics (sample-level check confirms
// ttHH has enough events; QCD HT slices except very high HT do NOT).
//
// Final 4 + 3 process groups (= 7 total):
//   ─ Main 4 (ttH AN A.2.1) ─
//   tt+LF      <- inclusive ttbar LF events + ALL minor BG
//                 (QCD, V+jets, single t, diboson, ttV {TTZToBB, TTTT,
//                  TTWH/WW/WZ, TTTW} — all per ttH AN A.2.1 strict reading)
//   tt+cc      <- inclusive ttbar cc events
//   tt+B       <- inclusive ttbar B events (genTtbarId 51/52/53-55)
//                 + ttbb dedicated (4FS NLO)
//                 + tt4b dedicated (LO; see note below on possible promotion)
//   ttH        <- ttHTobb (own measurement, ttH AN App. A.2)
//   ─ ttHH-extension 3 (own measurement, ttHH AN §6.1.4 / D.0.x) ─
//   ttHH       <- ttHH signal (D.0.1 own closure)
//   ttZH4b     <- TTZHTo4b (4-b final state; own group by analogy with ttHH)
//   ttZZ4b     <- TTZZTo4b (4-b final state; own group by analogy with ttHH)
//
// === FUTURE UPDATE TRIGGERS ===
//   These mappings may evolve as the ttHH FH analysis matures:
//     [tt4b]              → may be promoted from "tt+B" to its own "tt4b"
//                            group if ttHH FH DNN treats it as a separate
//                            output node (cf. ttHH AN §7.1 SL Option 1/2,
//                            §D.0.5 closure — currently FH-undefined).
//     [ttZH4b / ttZZ4b]   → may be merged back into "tt+LF" (ttH AN strict)
//                            if ttHH FH DNN does not separate them.
//                            Decision pending Data/MC validation.
//     [TTZToBB / TTTT /
//      TTWH / TTWW / TTWZ
//      / TTTW]            → currently in "tt+LF" per ttH AN A.2.1 strict.
//                            May move to "tt+B" if Data/MC mismodelling
//                            appears in 4b-rich phase space (small yield,
//                            so impact expected to be sub-percent).
//
// All update triggers are mirrored in docs/DECISIONS.md (ADR-001) and
// the project README (TBA).
//
// Measurement region (BTV recommendation):
//   nJets >= 6, HT >= 500, 6th jet pT > 40 (skim invariants).
//   *** NO b-tag selection *** — that's the whole point of the reweight.
//
// Process key dispatch (per event):
//   For inclusive ttbar samples → genTtbarId-based group routing.
//   For other samples → fixed mapping by sample name (declared below).
//
// [Ref] ttH AN-19-094 §A.2.1 (b-tag SF patch processes & minor BG fallback);
//       ttHH AN-2022/122 §3.4 (sample stitching SL Opt1/Opt2/DL),
//                        §6.1.4 / D.0.5 (own closure validation),
//                        §7.1, §7.3 (DNN classification reference).
// ============================================================================
#ifndef CONFIG_TTCATGROUP_HH
#define CONFIG_TTCATGROUP_HH

#include <string>
#include <vector>

namespace TtCatGroup {

// ────────────────────────────────────────────────────────────────────────────
// Internal 4-bucket categorization (event-level, for inclusive ttbar).
// Coarser than the analyzer's 5-cat TtCat enum (kLightFlavour / kAddCjet /
// kAdd1Bjet1Had / kAdd1Bjet2Had / kAdd2Bjet); the 3 b-jet categories collapse
// into kB here because b-tag SF derivation lacks per-sub-category statistics
// (see ttH AN A.2.1).
// ────────────────────────────────────────────────────────────────────────────
// [STEP7.5] kNB 추가: expandedTtbarId의 61/62(tt+bbb)·71/72(tt+4b)를 받는
// tt+nb 그룹. ttHH AN §D.0.5가 tt4b를 별도 normalization 유닛으로 다루는 것이
// 근거. 기존 멤버 값은 보존(코드/JSON 호환).
enum class Group {
    kLF = 0,    // tt + light flavour
    kCC = 1,    // tt + cc
    kB  = 2,    // tt + b (= tt+b + tt+2b + tt+bb merged)
    kNotTT = 3,
    kNB    = 4   // [STEP7.5] tt+bbb / tt+4b (expandedTtbarId 61,62,71,72)
};

// genTtbarId encoding (CMS GenTtbarCategorizer plugin):
//   0      -> LF
//   41-45  -> cc
//   51,52  -> 1 add b-jet (1 / >=2 b-hadrons)   ─┐
//   53-55  -> >=2 add b-jets                     ├─ all → kB
//   <0     -> not ttbar (or branch absent)      ─┘
//
// LIMITATION: catId 53-55 cannot distinguish tt+bb / tt+bbb / tt+4b — the
// plugin only encodes hadron multiplicity in the leading two b-jets, not
// the total add-b-jet count. To separate these, a sample-level approach is
// required (cf. ttHH AN §3.4: dedicated tt+4b LO MC = "Option 1",
// dedicated tt+bb 4FS NLO MC = "Option 2"). The DL channel additionally
// uses a per-event nGenAddBJets-based exclusion ("4 b-jet only excluded
// from inclusive ttbar"), which is NOT recoverable from genTtbarId alone
// and currently NOT IMPLEMENTED in this analyzer.
// [STEP7.5] expandedTtbarId(61/62=tt+bbb, 71/72=tt+4b)까지 인식하도록 확장.
// NanoAOD genTtbarId(≤55)만 들어오는 기존 호출 경로의 동작은 불변 —
// 61-72는 ExtendedTtbarId 산출물에서만 나온다. 이전엔 61-72가 kNotTT로
// 떨어져 inclusive ttbar에서 "tt+LF"로 미스라우트되는 버그가 있었다
// (stitch가 weight를 0으로 만들어 yield 영향은 없었지만 키는 틀렸음).
inline Group Classify(int genTtbarId) {
    if (genTtbarId < 0) return Group::kNotTT;
    const int catId = genTtbarId % 100;
    if (catId == 0)                       return Group::kLF;
    if (catId >= 41 && catId <= 45)       return Group::kCC;
    if (catId == 51 || catId == 52 ||
        (catId >= 53 && catId <= 55))     return Group::kB;
    if (catId == 61 || catId == 62 ||
        catId == 71 || catId == 72)       return Group::kNB;   // [STEP7.5]
    return Group::kNotTT;
}

// Era-aware note: this list assumes 2017 UL sample naming. When extending
// to other eras (2016 PreVFP/PostVFP, 2018, Run3) verify that inclusive
// ttbar names match (e.g. "TTbar_Hadronic_2018" suffix conventions) and
// extend this function accordingly. See docs/ARCHITECTURE.md §"Era policy".
inline bool IsInclusiveTtbar(const std::string& sampleName) {
    return sampleName == "TTbar_Hadronic"
        || sampleName == "TTbar_SemiLep"
        || sampleName == "TTbar_DiLep";
}

// [STEP7.5] dedicated ttbar HF 샘플 — per-channel 명명(현 main.yml) 포함.
// ⚠ 기존 코드는 "ttbb"만 알아서 TTbb_Hadronic 등 per-channel 샘플이
//   default("tt+LF")로 미스라우트되는 라이브 버그가 있었다 (STEP_7.5 문서).
inline bool IsDedicatedTtbarHF(const std::string& sampleName) {
    return sampleName == "ttbb"                 // 구(통합) 명명 호환
        || sampleName == "TTbb_Hadronic"
        || sampleName == "TTbb_SemiLep"
        || sampleName == "TTbb_DiLep"
        || sampleName == "TT4b";
}

// inclusive + dedicated 전체 — 이 family는 전부 HF category로 디스패치한다
// (sample 고정 키 금지). ttH AN §A.2 "include the contributions from all tt
// decay channels" = decay channel은 병합되고 HF category가 키라는 처방.
inline bool IsTtbarFamily(const std::string& sampleName) {
    return IsInclusiveTtbar(sampleName) || IsDedicatedTtbarHF(sampleName);
}

// ──────────────────────────────────────────────────────────────────────────
// MakeProcessKey: returns the process group key for a given event.
//
//   inclusive ttbar (TTbar_Hadronic / SemiLep / 2L2Nu):
//       genTtbarId  →  "tt+LF" / "tt+cc" / "tt+B"
//
//   dedicated ttbar HF samples & 4b signal-like processes:
//       fixed mapping (own group or borrowed) — see body.
//
//   minor backgrounds (low b-jet env) and ttV/4-top:
//       "tt+LF" — ttH AN A.2.1 strict reading.
//       Yields are sub-percent in the 4b SR; revisit if Data/MC validation
//       reveals visible mismodelling (see "FUTURE UPDATE TRIGGERS" header
//       comment).
// ──────────────────────────────────────────────────────────────────────────
inline std::string MakeProcessKey(const std::string& sampleName, int genTtbarId) {
    // 1. [STEP7.5] ttbar family 전체(inclusive + dedicated ttbb/tt4b)를
    //    HF category로 디스패치한다. 호출자는 가능하면 expandedTtbarId를
    //    전달할 것 (61-72 분리는 expanded id에서만 가능).
    //    근거: ttH AN §A.2 (category가 키, decay channel 병합) +
    //          ttHH AN §D.0.4 (ttbb는 owned portion만 사용) +
    //          ttHH AN §D.0.5 (tt4b는 별도 normalization 유닛 → "tt+nb").
    //    stitch가 0으로 만드는 비-owned 이벤트도 자기 category 키를 받는다
    //    (weight 0이라 yield 무영향; 유도/적용 키 일관성 유지가 목적).
    if (IsTtbarFamily(sampleName)) {
        switch (Classify(genTtbarId)) {
            case Group::kLF:    return "tt+LF";
            case Group::kCC:    return "tt+cc";
            case Group::kB:     return "tt+B";
            case Group::kNB:    return "tt+nb";  // [STEP7.5] tt+bbb / tt+4b
            case Group::kNotTT: return "tt+LF";  // safety (shouldn't happen)
        }
    }

    // 3. signals & 4-b heavy processes — own measurement
    //    ttHH/ttZH4b/ttZZ4b: own group by 4-b final state analogy; ttHH AN
    //    DNN classification (§7.x) treats them as separate categories. May
    //    be merged back to "tt+LF" if FH DNN doesn't separate them and
    //    yield impact remains sub-percent. See FUTURE UPDATE TRIGGERS.
    if (sampleName == "TTHHto4b")        return "ttHH";
    if (sampleName == "ttHTobb")     return "ttH";
    if (sampleName == "TTZHTo4b")    return "ttZH4b";
    if (sampleName == "TTZZTo4b")    return "ttZZ4b";

    // 4. low b-jet minor backgrounds + ttV/4-top → tt+LF
    //    Per ttH AN A.2.1: "Due to the poor statistics of simulated events
    //    for the minor backgrounds, those events are corrected with the
    //    values derived for the tt+LF process."
    //    Falls through to the default below.

    // 5. default: tt+LF (per ttH AN A.2.1)
    //    Includes: QCD HT slices, V+jets, single t, diboson, TTZToBB, TTTT,
    //              TTWH, TTWW, TTWZ, TTTW, and any unknown future sample.
    return "tt+LF";
}

// ──────────────────────────────────────────────────────────────────────────
// All process group keys that the JSON should contain.
// Used by makeReweightJSON to build the final 'process' category.
// ──────────────────────────────────────────────────────────────────────────
inline std::vector<std::string> AllProcessGroupKeys() {
    return {
        "tt+LF",
        "tt+cc",
        "tt+B",
        "tt+nb",   // [STEP7.5] tt+bbb / tt+4b — ttHH AN D.0.5 (tt4b 별도 유닛)
        "ttH",
        "ttHH",
        "ttZH4b",
        "ttZZ4b"
    };
}

// ──────────────────────────────────────────────────────────────────────────
// Per-sample: which process keys can a sample contribute to?
// (For inclusive ttbar: 3 groups. For others: 1 group.)
// Used by makeReweightJSON to know which sums to read from each sample file.
// ──────────────────────────────────────────────────────────────────────────
inline std::vector<std::string> ProcessKeysForSample(const std::string& sampleName) {
    // [STEP7.5] ttbar family는 4개 category 어디에도 기여할 수 있다
    // (expandedTtbarId 디스패치; stitch-zero 이벤트는 합산에 0 기여).
    if (IsTtbarFamily(sampleName)) {
        return { "tt+LF", "tt+cc", "tt+B", "tt+nb" };
    }
    // For non-ttbar: a single key, determined by MakeProcessKey with dummy id.
    return { MakeProcessKey(sampleName, -1) };
}

// ──────────────────────────────────────────────────────────────────────────
// Sanitize a process group key for use in ROOT histogram names / JSON keys.
// "tt+LF" → "ttLF"  (drop '+' which is not safe in some contexts)
// ──────────────────────────────────────────────────────────────────────────
inline std::string SanitizeKey(const std::string& key) {
    std::string out;
    out.reserve(key.size());
    for (char c : key) {
        if (c == '+') continue;     // drop '+'
        out.push_back(c);
    }
    return out;
}

// ============================================================================
// Sample stitching for inclusive ttbar ↔ heavy-flavour dedicated samples.
// ----------------------------------------------------------------------------
// Without stitching, a tt+bb-like event present in BOTH the inclusive ttbar
// MC (genTtbarId 53-55) AND a dedicated heavy-flavour MC (ttbb 4FS NLO,
// or tt4b LO) is double-counted. Stitching = giving such events weight 0
// in one of the two sources so the final yield is correct.
//
// References (ttHH AN-2022/122 §3.4):
//   SL Option 1: use LO tt+4b sample for tt+nb class;
//                exclude ≥2 add-b events from inclusive ttbar.
//                (ttbb dedicated is unused.)
//   SL Option 2: use NLO 4FS tt+bb sample for tt+nb class;
//                exclude ≥2 add-b events from inclusive ttbar.
//                (tt4b dedicated is unused.)
//   DL          : single tt+jets class;
//                 exclude *only the 4-b-jet portion* (gen-level) from
//                 inclusive ttbar, then add tt+4b.
//
// FH channel handling (this analysis):
//   ttHH AN §3.4 does not explicitly cover FH. We follow the SL Option
//   methodology because:
//     (a) DL-style "4 b-jet only" exclusion requires a per-event
//         nGenAddBJets count, which is NOT available from genTtbarId
//         alone (53-55 collapses tt+bb/tt+bbb/tt+4b — see Classify() note).
//     (b) FH stats are comparable to or larger than SL.
//   Choice between Option 1 and Option 2 remains open: we will pick the
//   one with better Data/MC kinematic agreement during validation
//   (see docs/DECISIONS.md ADR-002 and TBA README §"Stitching policy").
//
// Implementation note on weight composition:
//   This helper returns a multiplicative factor in {0.0, 1.0}. Apply it
//   to ω_base before any other SF (b-tag, trigger, top-pT, ...). Because
//   all downstream factors are multiplicative, an event with stitching
//   weight 0 is automatically excluded from every histogram, sum, and
//   b-tag SF normalization ratio with no further bookkeeping required.
// ============================================================================

enum class StitchingMode {
    kNoStitch  = 0,   // no stitching — current default; double counts ≥2 add-b events.
                      //                Use for raw Data/MC validation only.
    kOption1   = 1,   // ttHH AN §3.4 SL Option 1: tt+4b LO + (inclusive minus ≥2 add-b).
                      //                ttbb dedicated UNUSED.
    kOption2   = 2,   // ttHH AN §3.4 SL Option 2: tt+bb 4FS NLO + (inclusive minus ≥2 add-b).
                      //                tt4b dedicated UNUSED.
    kInclOnly  = 3    // inclusive ttbar only; both ttbb & tt4b dedicated UNUSED.
                      //                Simplest baseline; loses dedicated HF shape.
};

// ──────────────────────────────────────────────────────────────────────────
// StitchingWeight: returns a multiplicative weight ∈ {0.0, 1.0} that
// removes the double-counted region according to the chosen mode.
//
//   sampleName  — current event's sample name (as registered in Config).
//   genTtbarId  — current event's genTtbarId (for inclusive ttbar) or -1.
//   mode        — one of StitchingMode values above.
//
// Behaviour:
//   - For non-ttbar / non-dedicated-HF samples: always returns 1.0.
//   - For inclusive ttbar: returns 0.0 in modes Option1/Option2 if the
//     event is in Group::kB (≥2 add-b region), else 1.0.
//   - For ttbb dedicated: returns 0.0 in modes Option1 / kInclOnly.
//   - For tt4b dedicated: returns 0.0 in modes Option2 / kInclOnly.
//
// LIMITATION (DL-style mode not provided):
//   ttHH AN §3.4 DL prescription excludes only the gen-level 4-b portion
//   from inclusive ttbar, but Classify() cannot distinguish 4-b from
//   2-b/3-b within Group::kB. A future StitchingMode::kDL would require
//   an additional per-event nGenAddBJets branch; not implemented here.
// ──────────────────────────────────────────────────────────────────────────
inline double StitchingWeight(const std::string& sampleName,
                              int genTtbarId,
                              StitchingMode mode)
{
    if (mode == StitchingMode::kNoStitch) return 1.0;

    const bool isIncl = IsInclusiveTtbar(sampleName);
    const bool isTtbb = (sampleName == "ttbb");
    const bool isTt4b = (sampleName == "TT4b");

    // Inclusive ttbar: drop the ≥2 add-b region in stitched modes.
    // (Coarser than DL's "4-b only" — see LIMITATION above.)
    if (isIncl) {
        const bool inAddBRegion = (Classify(genTtbarId) == Group::kB);
        switch (mode) {
            case StitchingMode::kOption1:
            case StitchingMode::kOption2:
                return inAddBRegion ? 0.0 : 1.0;
            case StitchingMode::kInclOnly:
                return 1.0;     // keep all inclusive events
            default:
                return 1.0;
        }
    }

    // Dedicated HF MCs: keep / drop per mode.
    if (isTtbb) {
        switch (mode) {
            case StitchingMode::kOption1:   return 0.0;   // ttbb unused
            case StitchingMode::kOption2:   return 1.0;   // ttbb is the HF source
            case StitchingMode::kInclOnly:  return 0.0;   // dedicated unused
            default:                        return 1.0;
        }
    }
    if (isTt4b) {
        switch (mode) {
            case StitchingMode::kOption1:   return 1.0;   // tt4b is the HF source
            case StitchingMode::kOption2:   return 0.0;   // tt4b unused
            case StitchingMode::kInclOnly:  return 0.0;   // dedicated unused
            default:                        return 1.0;
        }
    }

    // All other samples (signals, ttV, QCD, V+jets, ...) — pass through.
    return 1.0;
}

// ──────────────────────────────────────────────────────────────────────────
// Human-readable label for StitchingMode (for log lines and output paths).
// Suggested usage: when running in two condor modes for validation, embed
// the label into output filenames (e.g. bTagReweight_Option1_<sample>.root).
// ──────────────────────────────────────────────────────────────────────────
inline std::string StitchingModeLabel(StitchingMode mode) {
    switch (mode) {
        case StitchingMode::kNoStitch: return "NoStitch";
        case StitchingMode::kOption1:  return "Option1";
        case StitchingMode::kOption2:  return "Option2";
        case StitchingMode::kInclOnly: return "InclOnly";
    }
    return "Unknown";
}

} // namespace TtCatGroup

#endif // CONFIG_TTCATGROUP_HH

