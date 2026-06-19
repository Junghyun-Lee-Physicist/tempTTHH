# ttHH (HH→4b) Fully-Hadronic Analyzer

CMS **ttHH, HH→bbbb, fully-hadronic** (ttbar→hadronic) 채널 분석 코드.
현재 baseline은 **Run2 2017 NanoAODv9 UltraLegacy** (~41.48 fb⁻¹), full Run2/Run3
확장을 염두에 둔 era-agnostic 구조를 지향한다.

> 이 문서는 2026-06 리팩토링 이후 상태를 반영한다. 단계별 변경 이력과 롤백
> 방법은 `docs/changes/STEP_*.md` 와 `docs/changes/README.md` 를 참조.

---

## 1. 분석 모드 (Analysis Modes)

analyzer는 `--mode` 로 동작이 갈린다. 모드별 selection 강제 여부는
`kCutSequence`(아래 §5)의 `enforceIn` 비트로, 비-cut 동작(lepton 수집·Higgs
reco)은 `SelectionPolicy` 로 정의된다.

| 모드 | 용도 | selection | SF·stitch |
|---|---|---|---|
| `main` | 본 분석 (최종 yield) | full (ttH AN Tab.55 FH) | 전체 SF + stitch ON |
| `btagtrig` | 파생 도구용 flat skim 생산 | object만 (cut 미강제, muon control 유지) | bootstrap (SF 누락 허용) |
| `prescan` | ΣgenW 등 누산 (보정 입력 생산) | 없음 (accumulator) | stitch-free |
| `debug` | **로컬 단계별 검증** | `main` 과 동일 | `main` 미러 + 단계별 로그 |

**제거된 모드**: `trigsf`(→ standalone `TriggerStudy` 패키지), `validation`.
입력 시 즉시 `[FATAL]` + 대체 경로 안내 후 종료한다.

### debug 모드
`main` 과 완전히 동일하게 동작하면서 weight 조립 / stitch / SF / cutflow /
tree 기록 / event shape / Higgs reco / 경로 env 를 단계별로 로깅한다
(`include/DebugLogger.h`). 다른 모드에서는 모든 hook이 no-op.
```bash
# 처음 N개 이벤트 상세 + 전 이벤트 집계(N/mean/min/max/NaN·Inf) + 종료 요약
TTHH_DEBUG_NEVENTS=20 ./ttHHanalyzer_unified --mode debug \
    --filelist <1-2 파일 list> --output dbg.root \
    --weight 1.0 --year 2017 --dataOrMC MC --sample TTToHadronic
grep '\[dbg\]' <job stdout>     # [dbg][weight] [dbg][stitch] [dbg][sf]
                                  # [dbg][sel] [dbg][evtshape] [dbg][higgsreco]
                                  # [dbg][paths] [dbg][seltable]
```
로컬에서 테스트 파일 1–2개로 각 로직을 검증하는 1차 수단이다 (condor 없이).
condor에서만 드러나는 것(대량 통계, 메모리)은 여전히 condor에서 확인.

---

## 2. 사전 준비 (Pre-requisites)

- 환경: **CMSSW_14_2_1** (ROOT 6 + correctionlib). 과거 el7/CMSSW_10_6 기반에서
  이전됨 — Makefile은 Linux/macOS(g++/clang++) 자동 분기.
- 라이브러리: ROOT, correctionlib, nlohmann/json (header-only, 동봉).
- 보정 입력: jsonpog-integration (CVMFS), GoldenJson, trigger SF JSON,
  b-tag norm reweight JSON, stitch JSON, expandedTtbarId lookup (§4 경로 참조).

```bash
# 예: KISTI/Tier3
cmsrel CMSSW_14_2_1 && cd CMSSW_14_2_1/src && cmsenv
git clone <repo> tempTTHH && cd tempTTHH
source setup.sh        # TNM_PATH / LD_LIBRARY_PATH / ROOT_INCLUDE_PATH 설정 (필수)
```

## 3. 컴파일

```bash
cmsenv
source setup.sh
make -j4               # 평소 빌드 (-O2 -g): exe ttHHanalyzer_unified, lib libttHH/libEventShape
make -C bTagSF_ReweightStudy   # b-tag SF 도구 (공유 헤더 Config_TtCatGroup.hh)
```
EventShape는 **정적 라이브러리(`lib/libEventShape.a`)로 분리 컴파일**되어 링크된다.
실행파일에는 `$ORIGIN/lib` RPATH가 박혀 `./lib`의 공유 라이브러리를 자동으로 찾는다.

### 빌드 레벨 (build levels)
디버깅 강도를 Makefile 옵션으로 토글한다. **디버그 심볼(`-g`)은 평소에도 항상**
포함되므로(성능 영향 거의 없음, 바이너리만 커짐) gdb로 segfault 줄 번호를 언제든
볼 수 있다. 무거운 진단(AddressSanitizer, `-O0`)만 `DEBUG=1`로 켠다.

| 명령 | 플래그 | 용도 |
|---|---|---|
| `make` | `-O2 -g -Wall` | 평소. 최적화 + 디버그 심볼. gdb backtrace 가능 |
| `make DEBUG=1` | `-O0 -g -Wall -Wextra -fsanitize=address -fno-omit-frame-pointer` | 메모리 오류(overflow·use-after-free·잘못된 free)를 런타임에 정확한 위치로 추적. 2~3배 느림 |
| `make VERBOSE=1` | (위 + 빌드 로그 상세) | 컴파일/링크 명령 출력 |

```bash
# segfault·메모리 오류 추적 (AddressSanitizer)
make clean && make DEBUG=1 -j4
./ttHHanalyzer_unified --mode debug --filelist <list> --output dbg.root \
    --weight 1.0 --year 2017 --dataOrMC MC --sample TTToHadronic
#   → ASan이 잘못된 메모리 접근 시 파일:줄 + 호출 스택을 즉시 출력

# 평소 빌드로 gdb (ASan 없이, -g 덕분에 줄 번호 나옴)
make -j4
gdb --args ./ttHHanalyzer_unified --mode debug --filelist <list> --output dbg.root \
    --weight 1.0 --year 2017 --dataOrMC MC --sample TTToHadronic
# (gdb) run
# (gdb) backtrace      ← 크래시 지점 + 호출 스택
# (gdb) frame N        ← 해당 프레임으로 이동
# (gdb) print 변수명    ← 그 시점 변수값
```

> ASan 빌드(`DEBUG=1`)로 만든 실행파일은 ASan 런타임을 요구한다(cmsenv 환경에
> 포함). Condor 대량 작업은 평소 빌드로, 메모리 버그 추적만 `DEBUG=1` 로컬에서.

## 4. 보정 입력 경로 (env / yml 통제)

절대경로 하드코딩을 제거하고 **env 우선, 미설정 시 코드 내 Tier3 default**
(하위호환) 구조로 통일했다. condor 실행 시 yml `common.path_*` 가 실행
스크립트에 export로 주입된다.

| yml `common.` | 환경변수 | 대상 |
|---|---|---|
| `path_jsonpog` | `TTHH_JSONPOG_PATH` | jsonpog-integration (POG 중앙) |
| `path_goldenjson` | `TTHH_GOLDENJSON_PATH` | GoldenJson 디렉토리 |
| `path_trigsf_dir` | `TTHH_TRIGSF_DIR` | trigger SF JSON 디렉토리 |
| `path_btag_reweight_json` | `TTHH_BTAGRW_JSON` | btagNormReweight.json |
| `path_stitch_json` | `STITCH_FACTORS_JSON` | stitch_factors_2017.json |
| `path_expanded_ttbarid_dir` | `EXPANDED_TTBARID_DIR` | ttnb_* lookup 디렉토리 |

**FATAL exit 맵** (Condor .err에서 식별): 40–43 stitch/expandedTtbarId 로드,
45 빈 process key, 46 b-tag reweight 평가 실패(흔히 **매핑 변경 후 JSON
미재생성**), 47 trigger SF 누락/평가 실패, 48 b-tag reweight JSON 누락,
49 중앙 보정 로드 또는 Data GoldenJSON 누락. `main`/`debug` 는 파생 보정
누락을 FATAL로, `btagtrig` 는 bootstrap(WARN+SF=1)로 처리.

## 5. Selection — 선언적 cut 테이블

이벤트 selection은 `ttHHanalyzer_unified.cc` 의 **`kCutSequence`** (선언적 표)
하나로 정의된다. 각 항목은 `{step, label, enforceIn, recordOnlyIfPass, pass,
onAfter}` — selection의 순서·라벨·모드별 강제 여부·부수작업(SF 적용/통계)이
한 화면에 보인다. cut 상수는 `include/SelectionCuts.h` (`namespace Cuts`,
constexpr, AN 출처 주석)에 단일 정의 — 기존 `std::map<string,float>` 의 무음
0-삽입 함정 제거.

현재 baseline = ttH AN-19-094 Table 55 FH: `nJets≥6, jet6 pT>40, HT>500,
nbJets≥2, lepton veto(pT>15), 30<m_qq<250` (DeepJet M=0.3040). SR(7/8/≥9
jets × ≥4 b)은 이후 분류 단계.

## 6. 로컬 실행 (Local Run)

CLI는 `--flag value` 형식 (과거 위치 인자에서 변경). **`--mode` 는 필수**이다
([STEP2] trigsf/validation 제거 시 모드 명시를 강제):
```bash
./ttHHanalyzer_unified \
    --filelist filelistTest/file_ttHH_0.txt --output out_ttHH_0.root \
    --weight 0.00000109763773 --year 2017 --dataOrMC MC --sample ttHH \
    --mode main
```
필수 인자 7개: `--filelist --output --weight --year --dataOrMC --sample --mode`.
하나라도 빠지면 `[Error] Missing mandatory arguments:` 로 종료한다.

> main/debug 모드는 stitch JSON + ttnb lookup 이 준비돼야 한다(없으면 FATAL).
> 로직 흐름만 빠르게 볼 때: `--mode debug` + (b-tag JSON 없으면) `TTHH_SKIP_BTAGRW=1`.

## 7. Condor 실행

```bash
# 현황만 (제출 없이 샘플별 complete/missing + 인덱스)
python3 submit_job_FH_Tier3_unified.py --mode main --report
# 제출 — 마스터 filelist를 N개씩 자동 분할
python3 submit_job_FH_Tier3_unified.py --mode main --files-per-job 5
# 실패 job만 재제출 (별도 출력 디렉토리로)
python3 submit_job_FH_Tier3_unified.py --mode main --files-per-job 5 \
        --resubmit --resubmit-to retry1
```

### 7.1 selection 영역 (region) + SF 토글
`--region` 으로 QCD 억제 1ℓ 제어영역을 선택한다(없으면 FH = lepton veto). output
디렉토리가 영역별로 자동 분리되므로 서로 덮어쓰지 않는다:

| 명령 | selection | output 디렉토리 |
|---|---|---|
| `--mode main` | FH (0 lepton) | `AnalyzerOutput_main/` |
| `--mode main --region muon` | muon 1 + MET>20 | `AnalyzerOutput_main_muon/` |
| `--mode main --region electron` | electron 1 + MET>20 | `AnalyzerOutput_main_electron/` |

SF 적용은 `--trigsf/--btagsf/--btagrw {on,off}` 로 production `evtWeight` 구성을
고른다. **기본값(trigSF on, b-tag shape/reweight off)이 곧 'trigSF만'** 이므로
세 영역에 trigSF 만 적용하려면 추가 인자 없이 그대로 둔다. (tree 의
`evtWeight_btagSF`/`evtWeight_full` tier 는 토글과 무관하게 항상 기록.)

**세 영역 제출 (trigSF 만):**
```bash
python3 submit_job_FH_Tier3_unified.py --mode main --files-per-job 5                    # FH
python3 submit_job_FH_Tier3_unified.py --mode main --region muon --files-per-job 5       # muon CR
python3 submit_job_FH_Tier3_unified.py --mode main --region electron --files-per-job 5   # electron CR
```

**세 영역 report / resubmit** — `--report`/`--resubmit` 에도 **같은 `--region`** 을
붙여야 해당 디렉토리의 현황을 본다(안 붙이면 FH 만 봄):
```bash
# FH
python3 submit_job_FH_Tier3_unified.py --mode main --report
python3 submit_job_FH_Tier3_unified.py --mode main --resubmit --files-per-job 5
# muon CR
python3 submit_job_FH_Tier3_unified.py --mode main --region muon --report
python3 submit_job_FH_Tier3_unified.py --mode main --region muon --resubmit --files-per-job 5
# electron CR
python3 submit_job_FH_Tier3_unified.py --mode main --region electron --report
python3 submit_job_FH_Tier3_unified.py --mode main --region electron --resubmit --files-per-job 5
```

- 샘플당 **마스터 filelist 하나**(`filelist_<sample>.txt`)만 있으면 `--files-per-job
  N` 으로 결정적 분할 — `<sample>_<jobIdx>.root` ↔ chunk가 영구 1:1.
  (per-file split 디렉토리 불필요. `N=1` 이면 한 줄=한 job 기존 동작.)
- ⚠ `--resubmit`/`--report` 는 **원 제출과 같은 `--files-per-job` + 같은 `--region`
  + 같은 SF 인자** 로 호출해야 chunk↔output 매핑과 디렉토리가 일치한다.

### 7.2 Data 샘플 era (자동 추출)
analyzer 는 Data 에 `--era` 를 필수로 요구한다(트리거 PD 분기 등). yml 이
bare-string 샘플이라 era 필드가 없으므로, submitter 가 **샘플명 끝 `_<대문자>`
에서 era 를 자동 추출**한다(`SingleMuon_C`→`C`, `JetHT_E`→`E`, `BTagCSV_B`→`B`).
yml 에 명시적 era 가 있으면 우선. MC 는 거치지 않는다. 추출 불가 시 FATAL.

### 7.3 Data PD 와 phase space 일치
lepton CR(muon/electron)은 hadronic phase space(`nJets≥6`, `HT>500`, **hadronic
trigger 유지**) 위에 'lepton 1 + MET>20' 을 얹어 QCD 만 떨어낸 부분집합이다.
trigger 가 여전히 hadronic 이므로 **Data 도 hadronic PD(JetHT, BTagCSV)** 를 써야
MC 와 trigger·phase space 가 일치한다. SingleMuon PD 는 muon-trigger 데이터라
부적합(MC 와 trigger 경로 불일치). PD-exclusivity(BTagCSV→4J3T,
JetHT→multiJet OR PFHT1050)는 그대로 유지된다 — muon 은 selection cut 이지
trigger 가 아니다.

### 7.4 제출 명령어 자동 기록 (cmd-log)
제출 시 명령어가 condor 디렉토리(region+SF 별 분리)의 `submit_command.txt` 에
타임스탬프와 함께 기록된다. `--report`/`--resubmit` 시 그 파일을 읽어 '원래 이렇게
제출됨' 을 출력하므로, 나중에 어떤 `--files-per-job`/`--region`/SF 로 제출했는지
헷갈릴 때 확인할 수 있다.

### 7.5 완료판정(report)의 한계
files-per-job/filelist 를 맞춰도 거짓 양성/음성이 가능하다(특히 half-written:
job 이 죽어도 entry>0 이면 complete 로 봄 — entry 수 미검사). 상세는
`docs/changes/STEP_15_resubmit_treecheck_region_output.md` §6 참조.
- `AnalyzerConfig/*.yml` + `proxy.cert` 필요. 경로 통제는 §4.
- `proxy.cert`: `voms-proxy-init --voms cms --valid 96:00 --out proxy.cert`
  후 `export X509_USER_PROXY=proxy.cert`.

---

## 8. 전체 워크플로우 (처음부터 끝까지)

아래 순서대로 실행한다. **bootstrap(처음 1회)** 와 **재실행** 을 구분한다.

### 8.0 컴파일 + 로컬 디버그 (가장 먼저)
```bash
cmsenv && source setup.sh
make -j4                      # analyzer + libEventShape
make -C bTagSF_ReweightStudy  # b-tag SF 도구 (공유 헤더)

# [debug] 테스트 파일 1~2개로 각 로직 검증 (condor 전에 필수)
TTHH_DEBUG_NEVENTS=20 ./ttHHanalyzer_unified --mode debug \
    --filelist filelistTest/file_TTToHadronic_0.txt --output dbg.root \
    --weight 1.0 --year 2017 --dataOrMC MC --sample TTToHadronic | tee dbg.log
grep '\[dbg\]' dbg.log     # weight/stitch/sf/sel/evtshape/higgsreco/paths/seltable
# 확인: [dbg][seltable] integrity: OK, BAD(NaN/Inf) 0건, cutflow 정상
```

### 8.1 prescan — ΣgenW 수집 (xsec_db·stitch 의 입력)
```bash
# (1) prescan 모드로 전 샘플 제출 (selection 없이 genWeight 누산)
python3 submit_job_FH_Tier3_unified.py --mode prescan
#     AnalyzerConfig/Tier3_2017_FH_unified_prescan.yml (analysis_mode: prescan) 사용
#     → ANALYZER_OUTPUT_DIR 아래 prescan TTree 출력 (job=file 1:1)

# (2) prescan ROOT → prescan_summary.json (Σgenw 집계 + runs vs tree 교차검증)
python3 consolidate_prescan.py \
    --input-base <prescan output base dir> \
    --outdir ./prescan_summary
#     → prescan_summary/prescan_summary.json
#        samples[X].runs.genEventSumw  (★ weight·stitch 가 사용)
#        samples[X].events.sumGenW_total (교차검증용; >0.01% 차이 시 경고)
```

### 8.2 stitch factor — r 계산 (xsec_db 를 읽음)
```bash
# compute_stitch_factors.py 는 ANALYZER_OUTPUT_DIR(prescan ROOT)를 직접 스캔하고,
# σ·BR 은 data/samples_2017UL.json(xsec_db)에서 읽는다 (TTHH_XSEC_DB 로 override).
# → submitter 의 base weight 와 같은 db 를 쓰므로 r·base 가 약분되어 yield 보존.
python3 compute_stitch_factors.py
#     입력: ANALYZER_OUTPUT_DIR (코드 상단 상수; 필요 시 수정)
#           data/samples_2017UL.json (env TTHH_XSEC_DB 로 변경 가능)
#     출력: DerivedCorr/stitchFactors/stitch_factors_2017.json
#     ※ db 통일 후 r 값이 이전과 달라짐(예: ttbb_Had 1.09→2.40). base 도 함께
#       바뀌어 base×r 은 보존 — 정상. (ttbb SL/DL 2.2x 과소정규화 해소)
```

### 8.3 main 분석 제출 (weight 자동 합성)
```bash
# 간소 yml(샘플 이름만). weight = lumi×xsec×br/Σgenw 를 submitter 가 합성.
python3 submit_job_FH_Tier3_unified.py --mode main --files-per-job 5
#   ( --mode main 이면 AnalyzerConfig/Tier3_2017_FH_unified_main.yml 자동 선택)
#   filelist 규칙: filelist_<sample_name>.txt
#   data/MC 자동 판정: xsec_db 의 cross_section_fb null 여부
#   경로 통제: yml common.path_* (Step 4) — STITCH_FACTORS_JSON 등

# 현황 확인 / 실패 재제출
python3 submit_job_FH_Tier3_unified.py --mode main --report
python3 submit_job_FH_Tier3_unified.py --mode main --files-per-job 5 \
    --resubmit --resubmit-to retry1
```

### 8.4 cross section 확인 (The Barn)
```
barn/The_Barn.html 을 브라우저로 열기 (같은 위치에 data/ 필요).
드롭다운에서 2017 UL 선택 → data/samples_2017UL.json 의 σ(fb)/br/N/ref/비고 표시.
```

### 8.5 단일-muon 검증 영역 (선택)
```bash
# main 과 동일하되 lepton veto → '정확히 muon 1 + electron 0' (SF 추가 없음)
TTHH_REQUIRE_1MUON=1 ./ttHHanalyzer_unified --mode main \
    --filelist <list> --output mu1.root \
    --weight <w> --year 2017 --dataOrMC MC --sample TTToHadronic
```

### 의존 관계 요약
```
make → [debug 검증] → prescan 제출 → consolidate_prescan
                                          ↓ (prescan_summary.json: runs.genEventSumw)
                          data/samples_2017UL.json (xsec_db, σ·BR 단일 소스)
                                          ↓
                    ┌─────────────────────┴─────────────────────┐
          compute_stitch_factors.py                    submitter base weight
          (xsec_db + prescan ROOT)                     (xsec_db + prescan_summary)
                    ↓                                            ↓
          stitch_factors_2017.json  ──(STITCH_FACTORS_JSON)──→  main 분석
```

> 핵심: `data/samples_2017UL.json`(σ·BR) 과 `prescan_summary.json`(Σgenw) 이
> 두 개의 단일 소스. weight 와 stitch r 이 **같은 σ** 를 쓰므로 정규화가 일관된다.

---

## tt+jets Event Categorization & 4-way Validation

The analyzer follows the project rule that **NtupleForge is the single
source of truth for ttbar categorization**. Downstream physics
(stitching, hist split, DNN routing, systematics) reads its label from
the ntuple's `ttCat_*` branches via `officialTtCategory()` and never
recomputes anything by itself.

That said, the analyzer *does* run the categorization logic again
during validation, so that any drift between the C++ and Python
implementations is caught immediately. There are four estimators of
the same per-event label, and the analyzer compares all six pair-wise
combinations on every MC event:

| Name          | Source                          | Algorithm                                  |
|---------------|---------------------------------|--------------------------------------------|
| `ANA_GENPART` | analyzer (C++)                  | own GenPart-based algorithm                |
| `ANA_GENID`   | analyzer (C++)                  | decode of `genTtbarId%100`                 |
| `NTU_PRIMARY` | ntuple `ttCat_*` branches       | ntuplizer's `decode_genttbarid` (POG path) |
| `NTU_XVAL`    | ntuple `ttCatXval_*` branches   | ntuplizer's GenPart algorithm (Python)     |

Two of the six pairs MUST agree at the byte level — the same algorithm
written in two languages, or the same integer decoded in two languages:

```
ANA_GENID   == NTU_PRIMARY    (both decode the same genTtbarId integer)
ANA_GENPART == NTU_XVAL       (same GenPart algorithm, two languages)
```

Any disagreement on either of these pairs is a regression in
ntuplizer ↔ analyzer parity and must be fixed before the production
sample is trusted. The remaining four pairs reflect a real algorithmic
difference (POG ghost-clustering vs our GenPart walker) and are
expected to disagree at the few-percent (signal) to few-tens-of-percent
(ttbar inclusive) level, depending on sample composition.

### Categories

Five mutually exclusive event-level categories, encoded by the
`TtCat` enum in `ttHHanalyzer_unified.h`:

| Category         | Definition                                             |
|------------------|--------------------------------------------------------|
| `LightFlavour`   | No additional b- or c-jet                              |
| `AddCjet`        | ≥1 additional c-jet, no additional b-jet               |
| `Add1Bjet_1Had`  | 1 additional b-jet containing exactly 1 b-hadron       |
| `Add1Bjet_2Had`  | 1 additional b-jet containing ≥2 b-hadrons (collinear g→bb) |
| `Add2Bjet`       | ≥2 additional b-jets (covers AN's bb / bbb / 4b)       |

The bbb / 4b split that some older notes mention is **not** present
here. The CMS `GenTtbarCategorizer` plugin (which produces the
NanoAOD `genTtbarId` integer we read) does not encode the additional
b-jet *count* — it encodes the b-hadron multiplicity inside the
leading two b-jets only. The ttHH AN reconstructs bbb / 4b at *sample*
level (Option1 / Option2 in §3.4), not per event, and the downstream
DNN merges them into a single tt+nb output node. So a per-event five
category schema matches both the available information in `genTtbarId`
and the downstream analysis design.

### Output histograms (in `TtCatValidation/` directory)

The analyzer writes 11 histograms into the `TtCatValidation/`
sub-directory of every output ROOT file:

1. **Four 1D count plots** (`TH1F`): `ttCat_Counts_AnaGenPart`,
   `ttCat_Counts_AnaGenId`, `ttCat_Counts_NtuPrimary`,
   `ttCat_Counts_NtuXval`. Per-category event count for each estimator.

2. **Six 2D pair-wise confusion matrices** (`TH2F`):
   `ttCat_<X>_vs_<Y>` for all C(4,2)=6 pairs of estimators. The two
   must-agree pairs are `ttCat_AnaGenId_vs_NtuPrimary` and
   `ttCat_AnaGenPart_vs_NtuXval`; their off-diagonal sum must be zero.

3. **One 2D `genTtbarId` distribution** (`TH2F`):
   `ttCat_GenTtbarIdMod100`. 60 bins of `genTtbarId%100` on X (so each
   POG code 0 / 41–45 / 51–55 lives in its own column), analyzer
   GenPart category on Y. Useful for spotting any POG code that the
   analyzer's GenPart algorithm is mishandling.

The end-of-job summary printout (in the analyzer log) lists the
per-pair agreement percentages and lists every off-diagonal cell of
the two must-agree pairs explicitly.

### Important notes

- The four-way validation runs on **all MC events** before selection,
  not just selected events. This is intentional — we want to catch
  parity drift across the full kinematic phase space, not just the
  small fraction that survives the analysis cuts.
- Data events take the early-return path: `_DataOrMC == "Data"` skips
  the entire validation block, so the per-event cost is zero on data.
- Downstream physics never reads any `ANA_*` value; only
  `officialTtCategory()` (= `NTU_PRIMARY`) is used. Removing the four
  validation estimators would not change a single physics result.

### Validation plotter — `scripts/plot_ttcat_validation.py`

A standalone PyROOT script that reads a single analyzer output file
and renders all 11 histograms in `TtCatValidation/` to PNG. Zero
external dependencies — `cmsenv` already provides ROOT.

```bash
cmsenv
python scripts/plot_ttcat_validation.py output_TTToHadronic.root
python scripts/plot_ttcat_validation.py output_TTToHadronic.root -o plots/
python scripts/plot_ttcat_validation.py output_TTToHadronic.root --normalize row
python scripts/plot_ttcat_validation.py output_TTToHadronic.root --log-counts
```

Output: 4 individual count PNGs, 1 grouped count overlay, 6 pair-wise
confusion matrices (the two must-agree pairs are auto-marked in their
title with `MUST-AGREE OK` or `MUST-AGREE BROKEN`), 1 `genTtbarId`
distribution, and 1 one-page `summary.png` with everything in a 4×3
grid. See the script's docstring for details.

**What "good" looks like:** both must-agree pair plots are
diagonal-only with `agreement = 100.0000%`. If a must-agree pair shows
any off-diagonal entry, stop and debug ntuplizer ↔ analyzer parity
before trusting the sample.

### Legacy: `TTCatDebug.h`

`TTCatDebug.h` is a stand-alone header from the previous validation
era. It writes per-event categorization diagnostics to a fixed CSV
file (`ttcat_ana.csv`) using the same schema as NtupleForge's old
`ttcat_ntu.csv`, so that the two files can be compared with shell
`sort` + `diff` for byte-level parity checks.

This was the right tool when ntuples carried only one categorization
result and the second algorithm had to be re-derived offline. **It is
no longer used by the current analyzer** (no `#include "TTCatDebug.h"`
anywhere in `ttHHanalyzer_unified.{h,cc}`, no call to `ttcatdbg::emit`)
and is superseded by the in-memory four-way comparison + the
`TtCatValidation/` ROOT histograms described above. The current
pipeline does the same byte-level parity check as the old CSV diff,
but inside the same event loop, using histogram off-diagonal cells
instead of an external `diff` invocation.

The header is kept in the repository for now as a reference and as an
emergency fallback if a future debugging session needs raw per-event
CSV output. It should be moved to a `legacy/` sub-directory (or
removed entirely) on the next cleanup pass.

### References

- **CMS `GenTtbarCategorizer.cc`** —
  [TopQuarkAnalysis/TopTools/plugins/GenTtbarCategorizer.cc](https://github.com/cms-sw/cmssw/blob/master/TopQuarkAnalysis/TopTools/plugins/GenTtbarCategorizer.cc).
  The integer encoding rule (lines ~282–300) is what `decode_genttbarid()`
  inverts.
- **CMS GenHFHadronMatcher TWiki** —
  <https://twiki.cern.ch/twiki/bin/view/CMSPublic/GenHFHadronMatcher>.
  Describes the ghost-clustering procedure that feeds
  `GenTtbarCategorizer`.
- **CMS NanoAOD `genTtbarId` documentation** —
  <https://twiki.cern.ch/twiki/bin/view/CMS/TopModGen>.
- **ttHH AN-2022/122**, §3.1 (object & event categorisation) and §3.4
  (5FS / 4FS sample stitching). The AN cites the GenHFHadronMatcher /
  GenTtbarCategorizer plugin chain as the official categoriser.
- **ttH AN-19-094**, §6.1.2 — earlier reference on the same plugin
  chain for the ttH analysis.
- **NtupleForge README** (`README_ntuplizer.md`) — canonical
  description of where the `ttCat_*` and `ttCatXval_*` branches come
  from. Read it before touching the analyzer's categorization code.

### Known TODOs

- The branch name `ttCatXval_*` (`Xval` = "cross-validation") is opaque.
  Will be renamed in coordination with the ntuplizer in the next major
  refactor; the analyzer functions `readNtupleXvalCategory` and the
  member histograms `_hTtCat_*_NtuXval` will rename together.
- Add a `validateTtCat` runtime flag (default `false`). With the flag
  off, production runs skip the four-way comparison and just call
  `officialTtCategory()`, recovering the per-event GenPart loop cost.
  With the flag on (validating a new ntuple production, or after
  touching the categorizer), the full comparison runs.
- Move (or delete) `TTCatDebug.h` to a `legacy/` sub-directory.

---

## 확장 가이드 (Extension Guide)

### 새 분석 모드 추가
1. `ttHHanalyzer_unified.h` 의 `enum class AnalysisMode` 에 멤버 추가.
2. `parseAnalysisMode()` / `analysisModeName()` 에 분기 추가.
3. `SelectionPolicy::fromMode()` 에 비-cut 동작 설정.
4. `selectionModeBit()` 에 cut 강제 비트 매핑 (cut을 강제할 모드면).
5. cut 강제 여부는 `kCutSequence` 각 항목의 `enforceIn` 비트로 제어
   (정책 bool 추가 불필요 — 테이블이 흡수).

### 새 selection cut 추가
1. 상수는 `include/SelectionCuts.h` (`namespace Cuts`) 에 AN 출처 주석과 함께.
2. `ttHHanalyzer_unified.h` 의 `CutStep` enum + `_cutStepLabels` 에 단계 추가.
3. `kCutSequence` 에 `{step, label, enforceIn, recordOnlyIfPass, pass, onAfter}`
   항목 추가 (pass는 capture-less 람다). 라벨은 `_cutStepLabels` 와 1:1.
4. debug 모드의 `[dbg][seltable]` 무결성 검사가 enum↔테이블↔라벨 1:1을
   런타임 확인한다.

### 새 보정(correction) 경로 추가
`src/CorrectionsManager.cc` 에서 `envOr("TTHH_...", default, what)` 패턴으로
경로를 받고, submitter의 `path_env_map` 과 yml `common.path_*` 에 키 추가.

### 카테고리(process group) 매핑 변경
`bTagSF_ReweightStudy/include/Config_TtCatGroup.hh` (analyzer·도구 **공유**
single source) 의 `MakeProcessKey` 만 수정. ⚠ 매핑 변경 후 반드시
`exe_BTagSF` → `exe_MakeJSON` 으로 JSON 재생성 (안 하면 main이 새 키를 옛
JSON에서 못 찾아 exit 46).

---

## Bug Fixes Applied
- **Memory leak in `event` class**: Added destructor to clean up heap-allocated physics objects (`objectJet`, `objectLep`, `objectGenPart`, etc.) that were previously leaked on every event
- **Arrow operator on `std::array`**: Fixed `writeHistos()` where `->Write()` was called on `std::array<TH1F*, 6>` instead of individual elements

---

