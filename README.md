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
| `btagtrig` | 파생 도구용 flat skim 생산 | 공통 cut 만 강제(noise filter, PV, njets≥6, 6번째 jet pT>40, HT>500 — `kSelEnforceAll`); hadronic trigger·lepton veto·b-tag·HadW 는 기록만(muon control 유지) | bootstrap (SF 누락 허용) |
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

> **header 를 바꾼 뒤에는 `make clean && make -j4`** (2026-10-04, STEP 23). Makefile 의 목적 파일 규칙은 `.cc` 에만 의존해서
> `include/eventBuffer.h`·`tnm.h`·`ttHHanalyzer_unified.h` 가 바뀐 것을 모른다 — 그냥 `make` 는 아무 말 없이 옛 header 로 만든
> 목적 파일을 쓴다. `tools/stage0/build_check.sh` 가 같은 clean 빌드를 하고 짧은 보고(rc, warning/error 수, 첫 error, md5)를 남긴다.

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

절대경로 하드코딩을 제거했고 **코드 안의 기본 경로는 없다**(STEP 18, 2026-06-30). condor 실행 시 yml
`common.path_*` 가 실행 스크립트에 export 로 주입된다. 빈 값·없는 키는 E12, 꼭 필요한 보정이 `null` 이면 E13,
선택 보정의 `null` 은 "끔"(`__NULL__`)이다. 정책 전체는 [`docs/reference/CONFIG_PATHS.md`](docs/reference/CONFIG_PATHS.md).

| yml `common.` | 환경변수 | 대상 |
|---|---|---|
| `path_jsonpog` | `TTHH_JSONPOG_PATH` | jsonpog-integration (POG 중앙) |
| `path_goldenjson` | `TTHH_GOLDENJSON_PATH` | GoldenJson 디렉토리 |
| `path_trigsf_dir` | `TTHH_TRIGSF_DIR` | trigger SF JSON 디렉토리(`trigger_sf.json.gz`, TriggerStudy 의 DeriveSF; [STEP 26 L] 그 해의 것이어야 한다 — description 의 `year=`, 없으면 2017; 다른 해면 MC job E50·제출 때 E50. 경로가 있으면 `--trigsf off` 에서도 읽으므로 JSON 이 생긴 뒤에만 넣는다) |
| `path_btag_reweight_json` | `TTHH_BTAGRW_JSON` | btagNormReweight.json |
| `path_stitch_json` | `STITCH_FACTORS_JSON` | stitch_factors_2017.json |
| `path_expanded_ttbarid_dir` | `EXPANDED_TTBARID_DIR` | ttnb_* lookup 디렉토리 |
| `path_pu_json` | `TTHH_PU_JSON` | 2024 PU weight JSON(2024 의 모든 job; 2016–2018 은 jsonpog 의 PU 라 읽지 않는다) |
| `path_btag_eff_json` | `TTHH_BTAGEFF_JSON` | [STEP 25 K] 2024 fixed-WP b-tag 의 우리 MC 효율 map(`tools/stage7/btag_eff_maps.py`; 2024 의 모든 job 이 키를 읽고, `--btagsf on` 의 MC 에 필수; 2016–2018 은 shape SF 라 없음) |

**FATAL exit 맵**(job `.err` 의 `[FATAL]` 줄; 번호는 2026-06-30 에 바뀌었다 — 표 전체는
[`docs/reference/ERROR_CODES.md`](docs/reference/ERROR_CODES.md)): 12·13 경로 정책, 40 중앙 보정(JME·PU·b-tag SF) 로드, 41 Data
golden JSON, 50 trigger SF JSON 없음·깨짐·평가 실패, 51 b-tag reweight JSON 없음·깨짐·평가 실패(흔히 **매핑 변경 뒤 JSON 을 다시
만들지 않음**), 52 2024 b-tag 효율 JSON 깨짐(main/debug) 또는 `--btagsf on` 인데 없음(40 은 그 BTV payload 가 없을 때), 60–62 stitch JSON, 63 stitch 계획의 샘플에 tt+nb lookup 없음, 70–73 tt+nb lookup, 80 빈 process key, 81 유한하지 않은
reweight. 옛 번호(45·46·47·48·49, 40–43)는 그 문서의 History note 에 대응표가 있다. `main`/`debug` 는 경로가 주어진 파생 보정의
로드 실패를 FATAL 로, `btagtrig` 는 bootstrap(WARN, SF=1)으로 처리한다.

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
# 제출 전 필수 — 읽기 전용 사전 점검 (제출·디렉토리 생성 없음, FAIL 시 exit 1)
python3 submit_job_FH_Tier3_unified.py --mode main --preflight
# 현황만 (제출 없이 샘플별 complete/missing + 인덱스)
python3 submit_job_FH_Tier3_unified.py --mode main --report
# 제출 — 마스터 filelist를 N개씩 자동 분할
python3 submit_job_FH_Tier3_unified.py --mode main --files-per-job 5
# 실패 job만 재제출 (별도 출력 디렉토리로)
python3 submit_job_FH_Tier3_unified.py --mode main --files-per-job 5 \
        --resubmit --resubmit-to retry1
```

`--preflight`(2026-07-26 신설)가 보는 것: yml 스키마(**제출과 동일한 파서**로 파싱) ·
`analysis_mode` 와 `--mode` 일치 · 실행파일 빌드 여부 · **보정 경로 null 정책**(키 누락→E12,
필수인데 null→E13 를 미리 예측) · `xsec_db`/`prescan_summary` 커버리지(MC 별 xsec·Σgenw>0; 2026-10-07 부터 Data 도
`xsec_db` 항목(`cross_section_fb` null)) · filelist 존재·개수·job 수 추정 · **Data era 추출 가능 여부** · output·condor 디렉토리 쓰기권한 ·
`proxy.cert` 존재와 나이 · lumi 값 일관성(yml ↔ xsec_db `_meta`).
로그: `preflight_<mode><suffix>_<timestamp>.log`.

**모든 표본의 weight 입력을 먼저(2026-10-07, STEP 24 J).** 제출기는 첫 표본을 내기 전에 모든 표본의 `xsec_db` 항목(MC 와 Data)과
MC 의 prescan 기록을 한 번에 보고, 하나라도 없으면 아무것도 내지 않고 E20/E21 로 멈춘다(`--resubmit`·`--report`·`--status` 도). 그 전에는
그 표본에서 멈춰 앞의 표본만 큐에 들어갔다(10-07: 2024 main 84 표본 중 76).

**`--only PATTERN[,PATTERN...]`(2026-10-07, STEP 24 J).** yml 의 표본 중 이름이 맞는 것만(fnmatch `*` `?` `[..]`, 정확한 이름도; 예
`--only 'ParkingHH_*'`) — 제출·`--resubmit`·`--report`·`--status`·`--preflight` 모두. 출력 base 와 condor 디렉토리는 yml 전체와 같고 condor
파일은 표본마다라, 돌고 있는 다른 표본을 건드리지 않고 표본을 더할 때 쓴다. 아무 표본에도 맞지 않는 패턴은 오류(제출 exit 2, preflight FAIL).

**stall guard(2026-10-08, STEP 25 K2; `--stall-guard on|off`, 기본 on).** condor 는 프로세스가 살아 있는지만 보고 진행은 모른다 — 10-08 에 입력
읽기(/pnfs)에서 멈춘 job 7 개가 12 시간 'running' 이었다(CPU 2–320 s). 그래서 모든 submit 파일(제출·`--resubmit`)에: 지금 run 이 1 시간을 넘고
CPU(user + system)가 그 시간의 5 % 미만이면 `periodic_hold`(이유 `tthh stall guard: running <분> min with <초> s CPU on <slot@machine>`,
HoldReasonCode 3, subcode 4201), 그 hold 만 5 분 뒤 `periodic_release`(같은 job 이 처음부터 다시, 같은 출력 — analyzer 가 파일을 새로 만든다;
모두 3 번 시작까지, 그 뒤에는 held 로 남아 `--report` 의 wait·`--status` 의 이유에 보임), 다시 시작할 때 마지막에 돌던 machine 은 피함
(`requirements`). preflight 에 `condor stall guard` 줄. **출력 파일 수는 끝난 job 수가 아니다**: analyzer 는 시작할 때 출력 파일을 만든다 —
완료는 `--report`(종료 마커 `cutflow_w_full`), 큐에 job 이 있는 동안 `--resubmit` 은 하지 않는다(도는 job 도 다시 낸다).
**merge 와 `condor_run.sh` job(2026-10-09, STEP 26 M·M2).** 10-09 에 입력 599 개를 읽던 merge 하나가 /pnfs 에서 멈춘 채 4 시간 'running'(CPU 20 s)
— guard 가 analyzer 의 submit 파일에만 있었다. 그런데 정상 merge 도 CPU 가 0.0–0.4 %(hadd 는 파일을 열고 읽는 시간이 대부분; KNU condor_history)라
analyzer 의 CPU 기준으로는 멈춘 것과 정상을 가르지 못한다 → merge 는 **시간 한도**(M2): 지금 run 이 `--time-limit` 시간(기본 3; 정상 merge 는 몇 분,
가장 큰 ParkingHH F 도 1 시간 안)을 넘으면 hold(이유 `tthh time limit: running <분> min (limit <분> min) with <초> s CPU on <slot@machine>`,
subcode 4202), 5 분 뒤 다른 machine 에서 처음부터 다시(`hadd -f` 가 파일을 새로 만든다), 3 번 시작까지; `--stall-guard off` 로 끔.
`tools/runlog/condor_run.sh` 는 `--time-limit H` 를 줄 때만 같은 줄(처음부터 다시 해도 되는 명령에만: plot·TriggerStudy·smoke·합성 시험·빌드;
`y1_reference.sh` 는 있던 출력을 거절한다). 그리고 `merge_outputs.py` 는 합치기 전에 condor 큐를 보고, 큐의 job 이 합칠 `<proc>.root`(같은 프로세스의
merge) 나 입력 `<proc>/<proc>_<N>.root`(그 프로세스의 analyzer job — 출력은 시작 때 생기므로 개수는 맞아도 덜 쓴 입력)를 인자로 가지면(X 포함) 멈춘다
(exit 2): merge 둘이 함께 돌면 반쪽 파일이 남아도 `--report`(가장 새 시도만 봄)가 ok 라고 할 수 있다. 다시 낼 때 `--skip-existing` 은 쓰지 않는다
(반쪽 파일도 건너뜀).

### 7.0 다른 연도(2018 UL) 실행 — 현재 **차단 상태**

**2018 은 아직 돌릴 수 없다.** 연도 의존 지점 전수 감사(2026-07-27) 결과 P0 7건 +
배관 5건을 먼저 고쳐야 하며, 그중 셋은 **조용히 틀린 결과**를 낸다(L1 prefiring 이 2018 에
없어 전 MC weight 가 0 / 2017 전용 trigger PD veto / DeepJet WP 가 2017 값). 전체 목록·조치·
근거는 [`docs/STATUS.md`](docs/STATUS.md) **OPEN #5** 와 워크스페이스
`RUNBOOK_UL18_to_controlplots.md` §4 에 있다.

준비된 것과 실행 순서(예정):

```bash
# 준비 완료: data/samples_2018UL.json (85 샘플, NtupleForge config 와 1:1)
# (1) 연도별 filelist — 2017 것을 덮어쓰지 않는다 (era 인자 필수)
python3 make_filelists.py 2018 /pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH2018UL_fullNano_v1
#     -> filelistTier3_2018/   (인자 생략 시 2017 + filelistTier3/)
#     ⚠ 한계: Data 분기는 아직 2017 하드코딩(`Run2017`)이라 2018 Data filelist 는 생성되지 않는다.

# (2) 2018 yml 을 먼저 만든다 — AnalyzerConfig/ 에는 Tier3_2017_* 3개만 있고
#     Tier3_2018_* 은 아직 없다. 2017 yml 에서 파생시키는 스니펫은
#     워크스페이스 RUNBOOK_UL18_to_controlplots.md §5 (Phase 4) 에 있다(정본).
#     그 다음 prescan (P0 #1 runYear 매핑만 고치면 실행 가능 —
#     prescan 은 loop()/createObjects() 를 타지 않아 trigger·WP 문제와 무관하다)
python3 submit_job_FH_Tier3_unified.py --mode prescan \
        --config AnalyzerConfig/Tier3_2018_FH_unified_prescan.yml \
        --filelist-dir filelistTier3_2018 --preflight

# (3) main 은 P0 전부 완료 후. 첫 plot 은 SF 없이:
#     --trigsf off --btagrw off
```

> ⚠️ (2026-10-06 갱신) 출력·condor 디렉토리의 연도 성분은 STEP 24 에서 들어갔다: yml `common.year` 가 2017 이 아니면
> `AnalyzerOutput_<mode><suffix>_<year>`, `condor/filelistTier3_unified_<mode><suffix>_<year>`, filelist 기본값
> `filelistTier3_<year>` 다(2017 은 예전 경로 그대로). `consolidate_prescan.py` 는 기본값이 2017 경로라
> `--input-base`·`--outdir` 를 반드시 명시한다(P0′ #10). 2018 의 analyzer 지원(v15)은 아직이다(PLAN §9 Y3).

`--filelist-dir`(2026-07-26 신설)로 연도별 filelist 디렉토리를 고른다.

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
**[STEP 25 K] 2024 의 `--btagsf on`** 은 shape SF 가 아니라 fixed-WP weight(BTV method 1a, 우리 L·M; D-2026-10-08-A)다: MC 에 yml
`path_btag_eff_json`(우리 효율 map; 2024 btagtrig MC 에서 `python3 tools/stage7/btag_eff_maps.py --config AnalyzerConfig/Tier3_2024_FH_unified_btagtrig.yml --base <btagtrig 출력>`)이
있어야 하고(없으면 E13/E52), 2024 는 `--btagrw` 를 쓰지 않는다(method 1a 가 정규화를 지킨다; job 로그의 `[btagSF] closure` 줄). MC 출력의
`BTagEff/` 가 그 도구의 입력이다. BTV 의 2024 shape 방법이 확인되면 그것으로 바꾼다(`EraConfig::btagMethod`).

**[STEP 27] Tree v1 과 job 크기** — `Tree/Tree` 에 ML 입력·jet 정답 표지(`jetGenMatch`, `jetGenMotherIdx`, `jetGenTopIdx`)·계통 weight 가 있다
(`docs/changes/STEP_27_treev1_btv_rule.md`, 목록은 `docs/AN_KR/` 부록 A). analyzer·제출기 옵션: `--tree-v1 on|off`(기본 on; off 는 Tree v1 을 빼서
tree 를 절반 아래로 — ML 입력이 필요 없는 btagtrig·lepton CR 에서), `--tree-pdf on|off`(기본 off; `LHEPdfWeight`, Tree v1 이 켜져 있어야 함),
제출기 `--memory`(condor `request_memory`, 기본 2 GB). **한 production 의 job(재제출 포함)은 한 실행 파일로**: 다시 빌드한 뒤 옛 production 의
job 을 재제출하면 `Tree/Tree` branch 가 달라지고, merge(`outputMerger/run_one_hadd.sh`)가 exit 8 로 멈춘다(hadd 는 그런 입력에서 branch 나 사건을
조용히 잃는다).

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
- [STEP 24] yml `common.files_per_job`(MC; Data 도 아래가 없으면) 와 `common.files_per_job_data`(Data 만). 순서:
  `--files-per-job` > (Data) `files_per_job_data` > `files_per_job` > 1. 2024 Data 는 1 이어야 한다(era C 파일마다 HLT branch 가
  다르고, 여러 파일 job 은 E11) — preflight FAIL, 제출에서도 그 샘플을 건너뛴다. 값을 yml 에 두면 `--report`/`--resubmit` 이 같은 수로 나눈다
  (2024 yml 은 그렇게 되어 있으니 `--files-per-job` 을 주지 않는다).

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

### 7.5 완료판정(report)과 실패 원인 (2026-10-06 갱신)
`--report`/`--status`/`--resubmit` 의 완료 = output 에 끝 표시가 있음(main: `cutflow_w_full`, STEP19; prescan: 한 행
`prescan` tree). half-written(job 이 쓰다 죽음)은 끝 표시가 없어 miss 로 잡힌다. 2026-10-06 부터 miss 를 둘로 나눈다:
`fail` = 그 job 의 가장 최근 시도가 0 이 아닌 return value(또는 signal, condor_rm, rc 0 인데 끝 표시 없음)로 끝남,
`wait` = 아직 큐에 있음(idle/running/held). `--status` 는 빠진 job 마다 cluster.proc, return value 와 이름
(`docs/reference/ERROR_CODES.md`), `.err` 경로를 찍는다. 근거: 샘플의 condor 로그 `log_<sample>.<cluster>.log`, job 의
`.out`(analyzer 가 `[ output file name ] -->` 를 찍음), 마지막 제출의 `arguments_<sample>.txt`.
**입력 파일을 건너뛴 job**(ROOT TChain 은 못 여는 파일을 오류만 찍고 건너뛰어 exit 0 이 된다): 2026-10-06(커밋 E) 부터
analyzer 가 시작할 때 job 의 모든 파일을 열고 entry 를 세어, 못 여는 파일·합이 다른 경우 E30 으로 멈춘다(`[inputs]` 줄). 그 전
빌드의 출력은 merge 뒤 `plotter/make_plots.py --check-only` 의 `EVENTS` 줄(MC: merge 된 noCut = prescan event 수)과
`consolidate_prescan.py --filelist-dir`(prescan 이 읽은 파일 수 = filelist 줄 수)로 확인한다. 옛 기록:
`docs/changes/STEP_15_resubmit_treecheck_region_output.md` §6, 2026-10-06 점검 `docs/changes/STEP_24_stage1_2_2024.md` §14.
- `AnalyzerConfig/*.yml` + `proxy.cert` 필요. 경로 통제는 §4.
- `proxy.cert`: `voms-proxy-init --voms cms --valid 96:00 --out proxy.cert`
  후 `export X509_USER_PROXY=proxy.cert`.

---

### 7.6 오래 걸리는 점검·빌드를 condor job 으로, 기록은 커밋 (`tools/runlog`, 2026-10-04)

```bash
# KNU, cmsenv 한 셸에서. job 이 worker 에서 cmsenv 하고 지금 디렉터리에서 명령을 그대로 돌린다
/bin/bash tools/runlog/condor_run.sh <step> -- <명령> [인자...]
/bin/bash tools/runlog/status.sh                         # cluster, 상태, exit code, 기록 파일
git pull --ff-only && git add runlogs && git commit -m "runlogs: ..." && git push   # 기록 올리기 (switch 뒤)
```

기록은 `runlogs/run_<step>_<UTC>.log`(머리: 시각·host·git HEAD·명령·CMSSW·ROOT, 본문: 출력, 꼬리: **EXIT**)와
`runlogs/LEDGER.tsv`. 우리 프로그램의 출력이라 커밋한다(`docs/DECISIONS.md` D-2026-10-04-A; crab 명령은 `runlogs/nocommit/`).
같은 기록을 지금 셸에서: `/bin/bash tools/runlog/runlog.sh <step> -- <명령>`. 메모리 기본 4GB — `-j4` 빌드는
`--cpus 4 --memory 12GB`, analyzer 자체를 돌리면 `--source setup.sh`. `--time-limit H`(STEP 26 M2; 기본 없음): 지금 run 이 H 시간을 넘으면 hold,
5 분 뒤 처음부터 다시(3 번까지) — 정상 시간보다 넉넉히, 처음부터 다시 해도 되는 명령(plot·TriggerStudy·smoke·빌드)에만.
자세히: [`tools/runlog/README.md`](tools/runlog/README.md).

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
#     skim 된 입력(2024: NtupleForge 6j20)이면 --skimmed 를 붙인다: tree < runs 는 skim 이라
#     정보(INFO)로만, tree ≤ runs 만 검사. 없으면 tree = runs 를 검사(skim 없는 2017).
#     exit 0 ⇔ "No anomalies" (빠진/깨진 job, 실패한 검사, 경고가 하나도 없음)
#     MC 의 읽은 파일 수 대조: --filelist-dir filelistTier3_<year> (prescan 이 못 연 파일을 잡는다)
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

### 8.3b merge 와 수율 표·plot (STEP 24)
```bash
# merge: 분석 yml 을 주면 프로세스마다 job 번호 <proc>_0..N-1 을 대조(빠짐·남는 파일이면 합치지 않음)
python3 outputMerger/merge_outputs.py --base <AnalyzerOutput_...> --config <분석 yml> --list
python3 outputMerger/merge_outputs.py --base <AnalyzerOutput_...> --config <분석 yml> --mode condor --proxy proxy.cert
python3 outputMerger/merge_outputs.py --base <AnalyzerOutput_...> --config <분석 yml> --report     # 프로세스마다 최근 시도
python3 outputMerger/merge_outputs.py --base <AnalyzerOutput_...> --config <분석 yml> --resubmit --mode condor --proxy proxy.cert
# [STEP 26 M·M2] merge job 에는 시간 한도(기본 3 h, --time-limit H, --stall-guard off 로 끔); 큐에 그 프로세스의 merge 나 analyzer job 이 있으면 합치지 않는다(exit 2)
# 수율 표와 완결성 (WARN·EVENTS·FLAG 줄까지 본다), 그다음 plot
python3 plotter/make_plots.py --config <분석 yml> --base <AnalyzerOutput_...> --check-only
python3 plotter/make_plots.py --config <분석 yml> --base <AnalyzerOutput_...>
# [D-2026-10-06-A] event tree(Tree/Tree)에서 다시 고른 control plot: 저장된 히스토그램 대신 고른 event 로 26 개를 만든다
python3 plotter/make_plots.py --config <분석 yml> --base <AnalyzerOutput_...> \
    --tree-cut 'passTrigger_HLT_PFHT1050 && HT > 1200' --tree-label 'HLT PFHT1050, H_{T} > 1200 GeV'
```
`merge_outputs.py` 종료: 0 정상, 1 실패한 merge 가 있음(local, `--report`), 2 인자·환경. worker(`run_one_hadd.sh`)의
종료 코드는 `docs/reference/ERROR_CODES.md` 끝. `--tree-cut` 의 식은 tree 의 branch 와 파생 열(`jet_pt_<i>`, `btag_<i>` …)의
RDataFrame 식이고, tree 는 그 run 의 selection(trigger OR 포함)을 통과한 event 라 더 좁히는 것만 된다(make_plots docstring 6).
2024 Data 의 trigger 는 PD 마다 다르다: JetMET0/1 은 `HLT_PFHT1050`, ParkingHH 는 b-tag 경로 가운데 `HLT_PFHT1050` 이 아닌 것
(D-2026-10-06-A). 시험: `python3 test/test_failure_checks.py`(76, ClassAd 모듈이 있으면 78), `python3 test/test_consolidate_prescan.py`(24),
`python3 tools/stage7/test_btag_eff_maps.py`(26; STEP 25 K), `bash test/trigger_study/run_trigstudy_synth.sh $PWD`(26; STEP 26 L: TriggerStudy 를
합성 skim 으로 — `TriggerStudy/exe_TrigStudy` 를 먼저 빌드).

**[STEP 26 L] trigger SF (TriggerStudy):** 연도는 env `TTHH_YEAR`(없으면 2017); 2024 는
`env TTHH_SKIM_DIR=<merge 된 btagtrig 출력> bash TriggerStudy/run_analysis.sh --year 2024 --jobs 4` → `TriggerStudy/run_2024/trigger_sf.json.gz`
(기준 `HLT_IsoMu24`, Data Muon0/1). 상세 `TriggerStudy/README.md` 0 절, [`docs/changes/STEP_26_trigger_sf_2024.md`](docs/changes/STEP_26_trigger_sf_2024.md).

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

### Validation plotter — **DOES NOT EXIST (PROPOSED)**

> **[2026-07-27 정정]** 이 절은 `scripts/plot_ttcat_validation.py` 를 기존 도구처럼
> 설명하고 있었으나 **그 파일도, `tempTTHH/scripts/` 디렉토리도 존재하지 않는다.**
> 아래는 만들 때의 사양으로만 남긴다 — 명령을 그대로 복사하면 실패한다.

**PROPOSED 사양**: analyzer output 1개를 읽어 `TtCatValidation/` 의 11개 히스토그램을
PNG 로 렌더하는 standalone 스크립트. 산출: count PNG 4장, grouped count overlay 1장,
pair-wise confusion matrix 6장(must-agree 2쌍은 제목에 `MUST-AGREE OK`/`BROKEN` 자동
표기), `genTtbarId` 분포 1장, 4×3 `summary.png` 1장.

**"좋다"의 정의**: must-agree 2쌍이 대각 성분만 갖고 `agreement = 100.0000%`.
off-diagonal 이 하나라도 있으면 멈추고 ntuplizer ↔ analyzer parity 를 디버그한다.

지금 당장 히스토그램을 보려면 ROOT 로 직접 열면 된다:

```bash
root -l output_TTToHadronic.root
# root [1] TtCatValidation->cd(); .ls
```

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
- **NtupleForge README** (`../NtupleForge/README.md`; 상세는
  `../NtupleForge/docs/04_architecture.md`) — canonical
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
JSON에서 못 찾아 exit 51 — `[FATAL][getBTagReweight] evaluation failed`; 빈 key 는 80, non-finite 는 81).

---

## Bug Fixes Applied
- **Memory leak in `event` class**: Added destructor to clean up heap-allocated physics objects (`objectJet`, `objectLep`, `objectGenPart`, etc.) that were previously leaked on every event
- **Arrow operator on `std::array`**: Fixed `writeHistos()` where `->Write()` was called on `std::array<TH1F*, 6>` instead of individual elements

---

