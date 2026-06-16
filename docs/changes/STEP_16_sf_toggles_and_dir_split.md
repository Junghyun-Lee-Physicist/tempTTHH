# STEP 16 — SF 적용 토글(CLI) + region/SF 조합 output 디렉토리 분기

- 날짜: 2026-06-15
- 요구: (1) production evtWeight 에 trigger SF / b-tag shape SF / b-tag norm
  reweight 각각 on/off 를 CLI 로 선택. (2) submit 시 output 디렉토리를 region +
  SF 조합으로 분기 (main / main_muon / main_muon_notrig ...). v15 위에 누적.
- 변경: `ttHHanalyzer_unified.cc/.h`, `src/tnm.cc`, `include/tnm.h`,
  `submit_job_FH_Tier3_unified.py`

## 1. SF 적용 토글 (CLI --trigsf/--btagsf/--btagrw)
production `evtWeight` 에 각 SF 를 곱할지 선택:
```
evtWeight = base×PU×L1×genW×stitch × (trigSF? × btagShape? × btagNormRW?)
```
- `--trigsf {on,off}` 기본 **on**
- `--btagsf {on,off}` 기본 **off** (1차 stack 은 trigSF 만)
- `--btagrw {on,off}` 기본 **off** (8-group JSON 준비 전; 기존 skip default 와 동치)

**중요**: tree 의 3-tier branch(`evtWeight_btagSF`, `evtWeight_full`)는 토글과
**무관하게 항상 기록**된다. 토글은 production `evtWeight` 한 줄만 바꾼다 — 비교는
언제든 tier 로 가능.

배선: tnm `--trigsf/--btagsf/--btagrw` → `commandLine::{sfTrig,sfBtag,sfBtagRw}`
→ main() `analysis.setSFflags(...)` → `_applyTrigSF/_applyBtagSF/_applyBtagRW`
→ SF 블록에서 `if (_applyXxx) _evtWeight *= ...`. 기존 `_skipBtagReweight` 는
`_applyBtagRW = !skip` 으로 동기화(env TTHH_SKIP_BTAGRW 하위호환 유지; CLI 우선).

## 2. output 디렉토리 = region + SF 조합
`AnalyzerOutput_<mode><region><SF>` 로 분기, 서로 덮어쓰지 않음:
- region: `_muon` / `_electron` (없으면 생략)
- SF: 기본(trig on, btag off, rw off)이면 생략; 벗어난 것만 태그
  - trig off → `_notrig`, btagsf on → `_btagsf`, btagrw on → `_btagrw`

예시:
| 명령 | output 디렉토리 |
|---|---|
| `--mode main` | `AnalyzerOutput_main` |
| `--mode main --region muon` | `AnalyzerOutput_main_muon` |
| `--mode main --region muon --trigsf off` | `AnalyzerOutput_main_muon_notrig` |
| `--mode main --region electron --btagsf on` | `AnalyzerOutput_main_electron_btagsf` |
| `--mode main --btagsf on --btagrw on` | `AnalyzerOutput_main_btagsf_btagrw` |

condor filelist/log 경로도 같은 suffix(`_dir_suffix`)로 분리 — job 충돌 방지.

## 사용 예
```bash
# 기본 FH (trigSF only)
python3 submit_job_FH_Tier3_unified.py --mode main --files-per-job 5
# muon CR
python3 submit_job_FH_Tier3_unified.py --mode main --region muon --files-per-job 5
# muon CR + SF 전혀 없이 (raw)
python3 submit_job_FH_Tier3_unified.py --mode main --region muon --trigsf off --files-per-job 5
# 8-group JSON 준비 후: 전체 SF
python3 submit_job_FH_Tier3_unified.py --mode main --trigsf on --btagsf on --btagrw on --files-per-job 5
```
로컬:
```bash
./ttHHanalyzer_unified --mode main --region muon --trigsf on --btagsf off --btagrw off \
    --filelist <l> --output o.root --weight <w> --year 2017 --dataOrMC MC --sample TTToHadronic
```

## 검증
- 배선 7곳(tnm.h/cc, analyzer setter/호출/3분기, submitter argparse/suffix) 확인.
- output suffix 조합 모의 통과(FH→main, muon→main_muon, notrig/btagsf/btagrw 조합).
- brace balance cc/h 정상. syntax OK.

## 롤백
SF 토글 멤버/ setSFflags/ 3분기 제거, submitter argparse/suffix 원복.
