// =============================================================================
//  test_SampleRegistry.cc — SampleAlias / SampleRegistry 단위 테스트
// -----------------------------------------------------------------------------
//  ROOT 불필요. 실제 data/samples_2017UL.json + prescan_summary/*.json 을 읽는다.
//
//  빌드 & 실행 (tempTTHH 루트에서):
//      g++ -std=c++17 -I include -I . -o /tmp/test_sr test/test_SampleRegistry.cc
//      TTHH_BASE=. /tmp/test_sr
//
//  이 테스트가 지키는 것
//  ---------------------
//   1. alias 표가 **양방향** 으로 동작한다 (신->구, 구->신).
//   2. `mustBeInStitchPlan()` 이 ttbar 7종으로 **제한**되어 있다.
//      이게 넓어지면 analyzer 가 TTHHto4b 같은 샘플에 대해서도 stitch/lookup
//      부재를 FATAL 로 취급해 버린다 (오탐으로 전 job 사망).
//   3. Data era 가 campaign 이름(`SingleMuon_Run2017B`)에서 한 글자로 뽑힌다.
//      이게 "Run2017B" 로 나오면 Run B 가 CDEF trigger bit 로 평가된다.
//   4. MC weight 가 제출기 공식과 일치한다: lumi × σ_fb × BR × k / Σgenw.
// =============================================================================

#include "SampleAlias.h"
#include "SampleRegistry.hh"

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

namespace {

int g_fail = 0;
int g_pass = 0;

void check(bool ok, const std::string& what) {
    if (ok) { ++g_pass; std::printf("  [ ok ] %s\n", what.c_str()); }
    else    { ++g_fail; std::printf("  [FAIL] %s\n", what.c_str()); }
}

void checkClose(double got, double want, double rtol, const std::string& what) {
    const bool ok = std::fabs(got - want) <= rtol * std::fabs(want);
    if (ok) { ++g_pass; std::printf("  [ ok ] %s  (%.10g)\n", what.c_str(), got); }
    else {
        ++g_fail;
        std::printf("  [FAIL] %s  got=%.10g want=%.10g\n", what.c_str(), got, want);
    }
}

bool contains(const std::vector<std::string>& v, const std::string& s) {
    for (const auto& x : v) if (x == s) return true;
    return false;
}

} // namespace

int main() {
    std::printf("\n=== [1] SampleAlias: 양방향 해석 ===\n");
    check(SampleAlias::legacy("TTbar_Hadronic")    == "TTToHadronic",  "TTbar_Hadronic -> TTToHadronic");
    check(SampleAlias::canonical("ttbb_2L2Nu")     == "TTbb_DiLep",    "ttbb_2L2Nu -> TTbb_DiLep");
    check(SampleAlias::legacy("TT4b")              == "tt4b",          "TT4b -> tt4b");
    check(SampleAlias::canonical("ttHH")           == "TTHHto4b",      "ttHH -> TTHHto4b (비-stitch alias)");
    check(SampleAlias::legacy("QCD_HT200to300").empty(),               "QCD 는 alias 없음");

    check(contains(SampleAlias::candidates("TTbb_DiLep"), "ttbb_2L2Nu"),
          "candidates(TTbb_DiLep) 에 구 이름 포함");
    check(contains(SampleAlias::candidates("ttbb_2L2Nu"), "TTbb_DiLep"),
          "candidates(ttbb_2L2Nu) 에 신 이름 포함");
    check(SampleAlias::candidates("WW").size() == 1,
          "alias 없는 샘플은 후보 1개");

    std::printf("\n=== [2] mustBeInStitchPlan 은 ttbar 7종으로 제한 ===\n");
    const char* kIn[]  = {"TTbar_Hadronic", "TTbar_SemiLep", "TTbar_DiLep",
                          "TTbb_Hadronic", "TTbb_SemiLep", "TTbb_DiLep", "TT4b"};
    for (const char* s : kIn)
        check(SampleAlias::mustBeInStitchPlan(s), std::string(s) + " 는 stitch plan 대상");
    for (const char* s : kIn)
        check(SampleAlias::mustBeInStitchPlan(SampleAlias::legacy(s)),
              std::string("legacy ") + SampleAlias::legacy(s) + " 도 대상");

    // 여기 걸리면 alias 표를 확장하면서 stitchPlanTable() 과 분리를 깨뜨린 것이다.
    const char* kOut[] = {"TTHHto4b", "ttHTobb", "TTZToBB", "TTTT", "TTWH",
                          "ttHH", "tttt", "QCD_HT200to300", "WW",
                          "SingleMuon_Run2017B"};
    for (const char* s : kOut)
        check(!SampleAlias::mustBeInStitchPlan(s),
              std::string(s) + " 는 stitch plan 대상 아님");

    std::printf("\n=== [3] Data era 추출 (한 글자여야 한다) ===\n");
    {
        const auto& b = SampleRegistry::get("SingleMuon_Run2017B");
        check(b.isData,              "SingleMuon_Run2017B 는 Data");
        check(b.era     == "B",      "era == \"B\"  (not \"Run2017B\")");
        check(b.dataset == "SingleMuon", "dataset == \"SingleMuon\"");
        check(b.weight  == 1.0,      "Data weight == 1.0");

        const auto& f = SampleRegistry::get("JetHT_Run2017F");
        check(f.era == "F" && f.dataset == "JetHT", "JetHT_Run2017F -> (JetHT, F)");

        // 옛 짧은 이름도 입력으로는 받아 준다
        const auto& s = SampleRegistry::get("BTagCSV_D");
        check(s.canonical == "BTagCSV_Run2017D", "BTagCSV_D -> BTagCSV_Run2017D");
        check(s.era == "D", "짧은 이름에서도 era == \"D\"");
    }

    std::printf("\n=== [4] MC weight = lumi × σ_fb × BR × k / Σgenw ===\n");
    {
        // TTbar_Hadronic : σ=831760 fb, BR=0.45441081, k=1, lumi=42.07,
        //                  Σgenw(runs) = 73140765879.470062  (구 이름 prescan 경유)
        const double want = 42.07 * 831760.0 * 0.45441081 * 1.0 / 73140765879.470062;
        checkClose(SampleRegistry::get("TTbar_Hadronic").weight, want, 1e-9,
                   "TTbar_Hadronic weight");

        // 구 이름으로 물어도 같은 값이 나와야 한다
        checkClose(SampleRegistry::get("TTToHadronic").weight, want, 1e-12,
                   "TTToHadronic (legacy) 도 동일 weight");

        check(!SampleRegistry::get("TTbar_Hadronic").isData, "TTbar_Hadronic 은 MC");
    }

    std::printf("\n=== [5] 목록 helper ===\n");
    {
        const auto mc = SampleRegistry::mcSampleNames();
        check(contains(mc, "TTbar_Hadronic"), "mcSampleNames 에 TTbar_Hadronic");
        check(!contains(mc, "SingleMuon_Run2017B"), "mcSampleNames 에 Data 없음");
        for (const auto& s : mc)
            if (s.find("_ext") != std::string::npos) {
                check(false, "mcSampleNames 에 _ext 샘플이 남아 있다: " + s);
                break;
            }

        const auto dt = SampleRegistry::dataSampleNames({"SingleMuon"});
        check(!contains(dt, "SingleMuon_Run2017B"), "dataSampleNames({SingleMuon}) 에서 제외됨");
        check(contains(dt, "JetHT_Run2017B"),       "dataSampleNames 에 JetHT 포함");
        check(contains(dt, "BTagCSV_Run2017B"),     "dataSampleNames 에 BTagCSV 포함");
    }

    std::printf("\n=====================================\n");
    std::printf("  PASS %d / FAIL %d\n", g_pass, g_fail);
    std::printf("=====================================\n\n");
    return g_fail == 0 ? 0 : 1;
}
