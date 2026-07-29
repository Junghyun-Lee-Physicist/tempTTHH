// ============================================================================
//  test_EraConfig.cc — EraConfig.h 단위 테스트 (ROOT/CMSSW 불필요)
//
//  실행:
//      g++ -std=c++17 -I include -o /tmp/test_EraConfig test/test_EraConfig.cc \
//        && /tmp/test_EraConfig
//
//  왜 있나: 여기 있는 값들(b-tag WP, golden JSON 파일명, JEC era 태그,
//  L1 prefiring 적용 여부)은 틀려도 프로그램이 **정상 종료**한다. 히스토그램만
//  조용히 틀리거나 빈다. 그래서 눈으로 보는 대신 assert 로 고정한다.
//  값을 의도적으로 바꿀 때는 이 테스트도 같이 고치고, AN 근거를 주석에 남긴다.
//
//  ⚠ unknown-year FATAL 경로는 std::exit 를 쓰므로 여기서 테스트하지 않는다.
//     test/test_EraConfig_fatal.sh 가 종료코드 11 을 확인한다.
// ============================================================================
#include "EraConfig.h"
#include <cassert>
#include <cmath>
#include <iostream>
int main(){
    // 1) yearForCorr
    assert(EraConfig::yearForCorr("2017")=="2017_UL");
    assert(EraConfig::yearForCorr("2018")=="2018_UL");
    assert(EraConfig::yearForCorr("2016preVFP")=="2016preVFP_UL");

    // 2) normalizeYear round-trips and absorbs the legacy capital-P spelling
    assert(EraConfig::normalizeYear("2018_UL")=="2018");
    assert(EraConfig::normalizeYear("2017_UL")=="2017");
    assert(EraConfig::normalizeYear("2016PreVFP_UL")=="2016preVFP");   // legacy casing
    assert(EraConfig::normalizeYear("2016postVFP")=="2016postVFP");
    for (const auto& y : EraConfig::validYears())
        assert(EraConfig::normalizeYear(EraConfig::yearForCorr(y))==y);

    // 3) AN Table 32 values, exactly
    auto w17=EraConfig::btagWP("2017"), w18=EraConfig::btagWP("2018");
    assert(std::fabs(w17.loose-0.0532f)<1e-7 && std::fabs(w17.medium-0.3040f)<1e-7 && std::fabs(w17.tight-0.7476f)<1e-7);
    assert(std::fabs(w18.loose-0.0490f)<1e-7 && std::fabs(w18.medium-0.2783f)<1e-7 && std::fabs(w18.tight-0.7100f)<1e-7);
    // monotonic L<M<T for every year
    for (const auto& y : EraConfig::validYears()){
        auto w=EraConfig::btagWP(y);
        assert(w.loose<w.medium && w.medium<w.tight);
    }

    // 4) L1 prefiring: 2018 off, others on
    assert(!EraConfig::usesL1Prefiring("2018"));
    assert( EraConfig::usesL1Prefiring("2017"));
    assert( EraConfig::usesL1Prefiring("2016preVFP"));

    // 5) golden JSON names distinct per year-group
    assert(EraConfig::goldenJsonFile("2018").find("Legacy2018")!=std::string::npos);
    assert(EraConfig::goldenJsonFile("2017").find("UL2017")!=std::string::npos);

    // 6) JEC era tag must NOT collapse 2018 B/C onto D (the old bug)
    assert(EraConfig::jecDataEraTag("2018","B")=="B");
    assert(EraConfig::jecDataEraTag("2018","C")=="C");
    assert(EraConfig::jecDataEraTag("2018","A")=="A");
    assert(EraConfig::jecDataEraTag("2018","D")=="D");

    // 7) HEM region geometry (AN p.120)
    assert( EraConfig::inHemRegion("2018",-2.0f,-1.2f));   // inside, inner band
    assert( EraConfig::inHemRegion("2018",-2.8f,-1.2f));   // inside, outer band
    assert(!EraConfig::inHemRegion("2018",-2.0f, 1.2f));   // wrong phi
    assert(!EraConfig::inHemRegion("2018", 2.0f,-1.2f));   // wrong (positive) eta
    assert(!EraConfig::inHemRegion("2018",-1.0f,-1.2f));   // eta above -1.3
    assert(!EraConfig::inHemRegion("2018",-3.2f,-1.2f));   // eta below -3.0
    assert(!EraConfig::inHemRegion("2017",-2.0f,-1.2f));   // never for 2017
    assert(!EraConfig::hemJes("2017").active && EraConfig::hemJes("2018").active);

    std::cout<<"ALL ERACONFIG ASSERTIONS PASSED\n";
    return 0;
}
