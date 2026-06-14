#pragma once
// =============================================================================
// DebugLogger — kDebug 모드 전용 경량 디버그 로거 (header-only)
// -----------------------------------------------------------------------------
// 목적: 로컬에서 테스트 파일 1-2개로 analyzer의 각 단계(weight 조립, stitch,
//       SF 적용, cutflow, tree 기록)가 정상 동작하는지 확인할 수 있도록
//       단계별 로그와 종료 요약(summary)을 출력한다.
//
// 사용 규약:
//   - kDebug 모드에서만 enable() 된다. 다른 모드에서는 모든 호출이 no-op이며
//     비용은 bool 체크 1회뿐 (hot loop 성능 영향 무시 가능).
//   - 이벤트 단위 상세 출력은 처음 N개 이벤트만 (기본 10개,
//     환경변수 TTHH_DEBUG_NEVENTS 로 조정).
//   - 모든 kv() 값은 전체 이벤트에 대해 집계(개수/합/최소/최대/비정상 수)되어
//     종료 시 summary() 표로 출력 — 비정상(NaN/Inf) 검출 포함.
//   - 이후 워크플로우 스텝에서 새 기능을 추가할 때마다 해당 스텝의 검증
//     hook을 이 로거로 추가한다 (스텝별 stage 이름 사용 권장,
//     예: "yml-path", "evtshape", "higgsreco").
//
// 출력 prefix: [dbg][<stage>] — grep으로 stage별 분리 가능.
// =============================================================================

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <map>

class DebugLogger {
public:
    // ── 활성화 ──────────────────────────────────────────────────────────
    // nPrint: 이벤트 단위 상세 출력을 할 이벤트 수 (이후엔 집계만).
    void enable(long nPrint = 10) {
        _on = true;
        _nPrint = (nPrint > 0 ? nPrint : 10);
        std::printf("[dbg] DebugLogger ENABLED — first %ld events verbose, "
                    "full-run accumulation + end-of-job summary\n", _nPrint);
    }
    bool on() const { return _on; }

    // ── 이벤트 경계 ─────────────────────────────────────────────────────
    // process() 시작에서 호출. 이벤트 카운터를 증가시킨다.
    void nextEvent() {
        if (!_on) return;
        ++_iEvent;
        if (verbose()) std::printf("[dbg][evt] ── event #%lld ──\n", _iEvent);
    }
    bool verbose() const { return _on && _iEvent <= _nPrint; }

    // ── 단계별 키-값 기록 ───────────────────────────────────────────────
    // verbose 구간에서는 즉시 출력, 전체 구간에서는 집계.
    void kv(const char* stage, const char* key, double v) {
        if (!_on) return;
        if (verbose())
            std::printf("[dbg][%s] evt#%lld %s = %.6g\n", stage, _iEvent, key, v);
        Acc& a = _acc[std::string(stage) + "/" + key];
        if (!std::isfinite(v)) { ++a.nBad; return; }
        ++a.n; a.sum += v;
        if (v < a.min) a.min = v;
        if (v > a.max) a.max = v;
    }

    // 자유 형식 메시지 (verbose 구간만)
    void msg(const char* stage, const std::string& s) {
        if (verbose()) std::printf("[dbg][%s] evt#%lld %s\n", stage, _iEvent, s.c_str());
    }

    // ── cutflow 기록 ────────────────────────────────────────────────────
    // processStep 람다에서 호출: 통과한 cut 단계를 집계 + verbose 출력.
    // (히스토그램 hCutFlow와 독립적으로 stdout에서 cutflow를 재구성해
    //  히스토 채움 로직 자체를 교차 검증하는 용도)
    void cut(int idx, const char* label, double w) {
        if (!_on) return;
        if (idx >= static_cast<int>(_cutN.size())) {
            _cutN.resize(idx + 1, 0);
            _cutW.resize(idx + 1, 0.0);
            _cutLabel.resize(idx + 1);
        }
        ++_cutN[idx];
        _cutW[idx] += w;
        if (_cutLabel[idx].empty()) _cutLabel[idx] = label;
        if (verbose()) std::printf("[dbg][sel] evt#%lld pass step %d (%s)\n",
                                   _iEvent, idx, label);
    }

    // ── 종료 요약 ───────────────────────────────────────────────────────
    void summary() const {
        if (!_on) return;
        std::printf("\n[dbg] ════════ DebugLogger end-of-job summary ════════\n");
        std::printf("[dbg] processed events (nextEvent calls): %lld\n", _iEvent);

        std::printf("[dbg] ── cutflow (count / Σweight at pass) ──\n");
        for (size_t i = 0; i < _cutN.size(); ++i) {
            if (_cutLabel[i].empty() && _cutN[i] == 0) continue;
            std::printf("[dbg][sel]  step %2zu %-22s N=%-10lld sumW=%.6g\n",
                        i, _cutLabel[i].c_str(), _cutN[i], _cutW[i]);
        }

        std::printf("[dbg] ── tracked values (stage/key: N, mean, min, max, nonfinite) ──\n");
        for (const auto& p : _acc) {
            const Acc& a = p.second;
            const double mean = (a.n > 0 ? a.sum / a.n : 0.0);
            std::printf("[dbg][acc]  %-32s N=%-10lld mean=%-12.6g min=%-12.6g max=%-12.6g BAD=%lld\n",
                        p.first.c_str(), a.n, mean,
                        (a.n > 0 ? a.min : 0.0), (a.n > 0 ? a.max : 0.0), a.nBad);
            // 비정상값(NaN/Inf)이 하나라도 있으면 눈에 띄게 경고
            if (a.nBad > 0)
                std::printf("[dbg][acc]  ^^ WARNING: %lld non-finite value(s) in %s\n",
                            a.nBad, p.first.c_str());
        }
        std::printf("[dbg] ═══════════════════════════════════════════════\n\n");
    }

private:
    struct Acc {
        long long n = 0, nBad = 0;
        double sum = 0.0, min = 1e300, max = -1e300;
    };

    bool _on = false;
    long _nPrint = 10;
    long long _iEvent = 0;

    std::map<std::string, Acc> _acc;       // stage/key 별 집계
    std::vector<long long>   _cutN;        // cut step 별 통과 수
    std::vector<double>      _cutW;        // cut step 별 Σweight
    std::vector<std::string> _cutLabel;    // cut step 라벨
};
