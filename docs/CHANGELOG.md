# Changelog — ttHH(HH→bbbb) Fully-Hadronic Analysis

> **Purpose:** one chronological line per change, newest first, linking to the full record. The detail lives in [`changes/STEP_*.md`](changes/); this file is the index, not a copy.
> **Audience:** anyone tracing when and why something changed.
> **Status:** append-only · last meaningful update **2026-07-27**.
> **Links:** decisions [`DECISIONS.md`](DECISIONS.md) · state [`STATUS.md`](STATUS.md).

> Append-only: add new entries at the top; do not rewrite history. "Detail" links point to the full per-step record.

## 2026-07-27 (3) — [값 확정] 2018 lumi **59.83 → 59.56** (LUM POG 원문 대조)

사용자가 LUM POG TWiki 원문(PDF 2편)을 제공 → 표를 직접 대조해 값을 확정했다.

**출처** (문서 전반에 링크로 명시):
[TWikiLUM](https://twiki.cern.ch/twiki/bin/viewauth/CMS/TWikiLUM) (index) ·
[LumiRecommendationsRun2](https://twiki.cern.ch/twiki/bin/view/CMS/LumiRecommendationsRun2) (정본 표) ·
인용 **CMS-PAS-LUM-20-001**.

**정본 = "Recorded Golden Legacy"** 행: **2017 = 42.07** (0.82 %), **2018 = 59.56** (0.84 %).
PreLegacy(42.12 / 59.47)는 쓰지 않는다 — 우리 샘플은 UltraLegacy 재처리이므로 Legacy
certification 이 맞는 짝이다.

- **59.83 은 그 페이지에 존재하지 않는 값이었다.** 잠정 placeholder 였고 이제 정정됐다:
  `data/samples_2018UL.json._meta` + 생성기 `NtupleForge/script/build_ul18_from_log.py`
  (재생성 후 **byte-identical** 확인 — 값이 되돌아가지 않는다), RUNBOOK 의 yml 생성 스니펫 3곳
  (`lumi_fb_inv: 59.56`) 과 stitch 스니펫(`LUMI_PB_INV = 59560.0`), beamer backup 표
  (재생성 → 각주 `L=59.56`, **PRELIMINARY 마커 0개**).
- `_meta` 에 2017 과 같은 스키마로 lumi 블록 신설: 불확도 0.84 %, PreLegacy 59.47,
  Golden JSON 파일명(`Legacy_2018/Cert_314472-325175_...`), brilcalc 명령,
  citation, 그리고 **2018-only combine nuisance `lumi_13TeV_15161718_l = 1.0084`**.
  `lumi_brilcalc_result_fb_inv` 는 **null** — TWiki 가 요구하는 "분석 고유 certified JSON 으로
  재실행"을 아직 안 했다(2017 은 42.0688 로 확인됨).
- **2017 은 값이 아니라 코드가 문제였음이 확정됐다**: `_meta` 의 42.07 이 옳고,
  `AnalyzerConfig/Tier3_2017_FH_unified_{main,btagtrig}.yml` 의 **41.48 이 틀렸다**
  (전 MC weight 1.42 % 오차). 고치면 **main·btagtrig 재생산 + b-tag reweight JSON·stitch
  factor 재유도**가 필요하므로 **사용자 승인 대기**로 남겼다 — 이번 변경에 포함하지 않았다.
- `reference/LUMI_SOURCES.md` 전면 개편: BLUF 에 정본 값, §1 references, §2 LUM POG 표 발췌 +
  combine nuisance, §3 을 **9개 지점 × 반영 여부(✅/❌)** 표로. OPEN #6 도 "값 확정 / 코드 일부
  미반영"으로 재작성.

## 2026-07-27 (4) — [문서] 진행 중 CRAB 부분 산출물용 로컬 검증 워크북 신설

`WORKBOOK_local_test_partial_CRAB.md` (워크스페이스 루트). 두 캠페인이 **끝나기 전에** 이미
stage-out 된 파일만으로 할 수 있는 검증을 순서대로 정리했다.

- **핵심 근거**: `matchTtbarId` 는 `unmatched > 0` 이어도 **exit 0** 을 낸다
  (`tools/matchTtbarId.cc:476-481`). 즉 부분 산출물로 **정확성**(byte-identity + 확장 불변식)은
  지금 완전히 검증 가능하고, 검증 못 하는 것은 **완결성**(nevents 대조)뿐이다.
- 테스트 A(파일별 불변식 매크로) · B(filelist + **중복 제출 가드의 첫 실제 EOS 실행**) ·
  C(extend↔중앙 NanoAOD 매칭, 가장 작은 `ttbb_2L2Nu` = nano 6파일/4.79 M 부터) ·
  D(`forgedNtuple_*.root` sanity) · E(patch 추출 구조만) + 판정 요약표 + 로그 6종.
- **경고 명시**: 부분 patch 를 `DerivedCorr/expandedTtbarId/2018` 에 복사하면 빠진 event 가
  조용히 "확장 안 됨"으로 처리되어 **틀린 물리 결과**가 나온다. `/tmp` + `PARTIAL_DO_NOT_USE_`
  접두사로 강제.
- 하위 에이전트 검증에서 잡아 고친 것: `$?` 가 **tee 의 상태**라 모든 실패가 0 으로 보이던 문제
  (→ `${PIPESTATUS[0]}`, 6곳), `ls bin/` 기대 목록에 없는 `compareExtendToCentral`,
  `genTtbarId` 부재를 전 MC 에 FAIL 로 처리해 QCD/WJets/ST/TTHH 가 전부 가짜 FAIL 나던 게이트
  (→ ttbar-parented 샘플만 FAIL, 나머지는 info), ntuple 쪽 `timestamps` 열에 extend 전용
  "1이 아니면 중단" 규칙을 적용하면 안 된다는 주의(JetHT 4 task / `_ext1` 2 task 는 정상).

## 2026-07-27 (2) — [문서·정리] 재현성 감사: 깨진 명령 수정, 유령 문서 제거, .gitignore 보강

코드 로직 변경 **없음**. 하위 에이전트로 전 저장소 문서를 실제 CLI 와 대조한 결과의 반영이다.

**깨져 있던 명령 (복붙하면 실패했다) — 이 저장소 몫:**

- `README.md` §7.0 은 `AnalyzerConfig/Tier3_2018_FH_unified_prescan.yml` 을 이미 있는 것처럼
  가리켰다. `AnalyzerConfig/` 에는 `Tier3_2017_*` 3개뿐 → **파생 스니펫이 있는 RUNBOOK §5 를
  가리키도록 수정**.
- `README.md` "Validation plotter — `scripts/plot_ttcat_validation.py`" 절 전체가 **존재하지 않는
  스크립트**를 4가지 호출 예시까지 곁들여 기존 도구처럼 설명하고 있었다(`tempTTHH/scripts/`
  디렉토리 자체가 없다). **PROPOSED 사양으로 강등** + 당장 쓸 수 있는 `root -l` 대안 병기.
- `README.md` 의 `README_ntuplizer.md` 참조 → 실제 경로 `../NtupleForge/README.md`.
- RUNBOOK 쪽(워크스페이스): `compute_stitch_factors.py` 는 **argparse 가 아예 없는데**
  `--xsec-db/--prescan/--out` 을 주는 명령이 실려 있었다 — 플래그가 조용히 무시되고 2017 상수로
  돌아 **`stitch_factors_2017.json` 을 덮어쓴다**. USER CONFIG 를 고치는 스니펫으로 교체.
  `consolidate_prescan.py` 도 출력이 항상 `<outdir>/prescan_summary.json` 이고 `--input-base`
  기본값이 2017 이라 인자 없이 부르면 **2017 요약을 덮어쓴다** → `--input-base`·`--outdir` 필수
  명시로 교체. `merge_outputs.py`·`scenario_runner.py` 의 `required=True` 인 `--base`
  (그리고 `--plotter`) 누락도 실제 경로로 채웠다 — `--trigsf off` 면 output 디렉토리가
  `AnalyzerOutput_main_notrig` 임을 명시.

**숫자 정정**: 연도 의존 감사 항목 수를 **13 → 17**(P0 7 + P0′ 5 + P1 5)로 통일
(`CHANGELOG.md`·`STATUS.md`·RUNBOOK 헤더가 13 이라 하고 §4 표는 17개를 나열하고 있었다).

**연도 혼입 경고를 문서에 명시**: `submit_job_FH_Tier3_unified.py:158-160` 의
`path_output_base` 에 연도 성분이 없어 2018 산출물이 2017 과 같은 `AnalyzerOutput_*` 에 섞인다
(P0′ #9). README §7.0 과 RUNBOOK merge 단계 양쪽에 경고 박스 추가.

**`.gitignore` 보강**: `__pycache__/`·`*.py[cod]`(이 저장소에만 없어서 `tempTTHH/__pycache__`,
`python/ttHHmodules/__pycache__` 가 매번 untracked 로 떴다), `*.out`/`*.err`/`manager.log`/
`logs/`, `prescan_summary_20*/`. 블랭킷 `*.root`/`*.pdf` 규칙이 산출물까지 막는다는 주의도 명기.
정리: `__pycache__` 2곳, `.DS_Store` 삭제.
**미처리(사용자 판단 필요)**: `bTagSF_ReweightStudy/logs/` 에 condor `.out`/`.err` **64개(412 KB)
가 이미 커밋되어 있다** — 새 규칙은 추적 중인 파일을 untrack 하지 않으므로
`git rm -r --cached bTagSF_ReweightStudy/logs` 가 별도로 필요하다.

## 2026-07-27 — [문서] 2018UL 연도 의존 지점 전수 감사 (17항목, 코드 무변경)

- 목표 변경(사용자): **UL18 control plot 최단 경로**. 실행 문서 = 워크스페이스
  `RUNBOOK_UL18_to_controlplots.md` (구 `RUNBOOK_UL18_step1.md` 는 SUPERSEDED).
- analyzer/보정/스터디/플로터 전체를 grep 하여 연도 의존 지점 **17개**를 file:line 으로 확정하고
  P0(7)/P0'(5)/P1(5) 로 분류 → STATUS OPEN #5 에 표로 등재. **코드는 아직 바꾸지 않았다.**
- 특히 **조용히 틀리는 3건**을 확인: ① `L1PreFiringWeight_Nom` 은 2018 NanoAODv9 에 없고
  eventBuffer 가 스칼라를 0 으로 초기화하므로 `_evtWeight *= 0` → **전 MC weight 0, 경고 없음**
  ② 2017 전용 `JetHT && !BTagCSV` orthogonality veto 가 2018(BTagCSV PD 부재)에서 4J3T 발화
  event 를 폐기 ③ DeepJet WP 가 2017UL 상수라 2018 신호영역 정의가 틀어짐.
- 또한 **2018 Data 는 현재 filelist 조차 만들 수 없음**을 확인(`make_filelists.py` Data 분기가
  `Run2017` 하드코딩) → control plot 에 Data 를 넣으려면 이 일반화가 선행 필수.
- 그리고 **output 디렉토리에 연도 성분이 없어**(`submit_job_FH_Tier3_unified.py:158-162`)
  2018 산출물이 2017 과 같은 pnfs 디렉토리에 섞인다 — 2018 제출 전 반드시 수정.

## 2026-07-26 — [문서] reference/LUMI_SOURCES.md 신설 (lumi 산재 지점 전수 목록, 값 무변경)

- lumi 불일치를 추적하다 **lumi 가 3개 저장소 7개 지점**에 산재해 있음을 확인 → 신규
  [`reference/LUMI_SOURCES.md`](reference/LUMI_SOURCES.md) 에 전수 목록·성격(정본/fallback/
  하드코딩/자동생성)·영향·2018 에 아직 없는 것·변경 절차 체크리스트·재생산 판단을 정리했다.
- 드러난 모순 3건(값은 **바꾸지 않았다**): ① weight 사용값 41.48(yml) vs 표기값 42.07(xsec_db
  `_meta`, brilcalc 42.0688) → 1.42% ② 같은 발표자료 PDF 안에 41.48(본문)과 42.07(backup 표)
  동시 인쇄 ③ b-tag study plot 에만 세 번째 값 41.5.
- 사용자 결정: **brilcalc 재실행 후 2017·2018 을 한 번에 통일** (STATUS OPEN #6).

## 2026-07-26 — submitter: --preflight / --filelist-dir 추가, Data era 정규식 일반화 (**동작 변경**)

- **`--preflight` (신규, 읽기 전용).** 제출·디렉토리 생성·proxy 확인을 일절 하지 않고
  yml 스키마 / 실행파일 / 보정 경로 null 정책 / xsec_db·prescan_summary 커버리지 /
  filelist 존재·개수 / Data era 추출 / output·condor 디렉토리 쓰기권한 / `proxy.cert` 를
  점검하고 `preflight_<mode><suffix>_<timestamp>.log` 를 남긴다. FAIL 이 하나라도 있으면
  exit 1. 점검은 **제출 때와 동일한 파서**(`load_yaml_config`)와 동일한 prescan 스키마
  (`samples` 하위)를 쓴다.
- **`--filelist-dir` (신규).** master filelist 디렉토리를 인자로 받는다(기본 `filelistTier3`).
  연도별 분리(`make_filelists.py 2018` → `filelistTier3_2018/`)를 쓰기 위한 것.
- **[동작 변경] Data era 추출 정규식 `Run2017([A-Z])$` → `Run\d{4}([A-Z])$`.**
  기존 정규식은 2018 키(`JetHT_Run2018A`)를 잡지 못했고, fallback `_([A-Z])$` 도 'A' 앞이
  '8' 이라 실패해 **FATAL** 로 끝났다. 이제 2016/2017/2018/Run3 키 모두 era 를 추출한다.
  2017 키(`JetHT_Run2017B`)의 결과는 이전과 동일하다(회귀 없음).
- `make_filelists.py` era 인자화: `python3 make_filelists.py [2017|2018] [SAMPLE_DIR]`.
  기존에는 SAMPLE_DIR/OUTPUT_DIR 이 2017 고정이라 다른 연도로 쓰면 커밋된
  `filelistTier3/` 를 덮어썼다. 인자 없으면 2017 = 기존 동작 그대로.

## 2026-07-26 — make_filelists.py: ntuple 파일명 forgedNtuple 대응 (이중 매칭)

- NtupleForge 가 산출 ntuple 이름을 `slimmedNtuple.root` → **`forgedNtuple.root`** 로 rename(D-F, 2026-07-26). 이에 맞춰 `make_filelists.py` 에 `NTUPLE_PREFIXES = ("forgedNtuple", "slimmedNtuple")` 를 도입하고 `find_root_files()` 가 **두 prefix 를 모두** 수집하게 했다.
- **왜 이중 매칭인가**: 현재 Tier-3 에 있는 `ttHH2017UL_fullNano_v20` 생산물은 실제로 `slimmedNtuple_*.root` 다. forged 전용으로 바꾸면 기존 2017 filelist 재생성이 즉시 깨진다. 전 캠페인 재생산 완료 후에만 legacy prefix 를 제거할 것.
- analyzer 의 `TFile::Open` 은 filelist 의 경로를 그대로 열기 때문에 파일명 하드코딩이 없다 — 이 스크립트가 유일한 결합 지점이다.

## 2026-07-26 — [문서] xsec_db 참고문헌 오류 2건 발견 (값 무변경, OPEN)

ttH/ttHH AN 과 샘플 교차검증 중 `data/samples_2017UL.json`(→ 2018UL 로 그대로 복사됨)에서 발견. **값은 바꾸지 않았다** — 출처 확정은 사용자 판단 사항:

- `TTTT`/`TTWW`/`TTWH`/`TTWZ`/`TTTW` 의 `xsec_ref` 가 `"ttHH AN Tab.9"` 인데, AN-2022/122 Table 9 는 ttHH·ttH·tt+jets·tt+bb·tt+4b·ttZ·ttZZ·ttZH **8행뿐**이며 AN 본문에 ttWW/tttt/multi-top 언급이 없다 → 참고문헌이 틀렸거나 값의 실제 출처가 다름(XSDB 추정).
- `TTZToBB` = 861 fb (ref: AN Tab.16) 이지만 **AN Tab.9 의 ttZ = 841 fb** — 어느 정의/표가 맞는지 확인 필요.
- STATUS OPEN #2("verify provisional cross sections")의 구체 항목으로 편입.

## 2026-07-26 — [문서] 2018 trigger PD 로직 변경 필요성 확인 (코드 무변경, FUTURE)

- DAS 광역 스캔 결과 **BTagCSV PD 는 2018 에 존재하지 않음이 확정**(전 tier·전 status 0건;
  2018 PD 통합에서 BTagCSV/HTMHT/FSQJet/HighPt* 삭제, EGamma 신설. `/BTag*/Run2018*/` 는
  BTagMu = BTV muon-tag 캘리브레이션 PD 만 반환).
- **영향**: 현재 `ttHHanalyzer_unified.cc` ~L295–360 의 orthogonality 로직
  (4J3T→BTagCSV / 6J·HT→JetHT + veto)은 2017 전용이다. 2018 은 해당 path 들이 모두
  JetHT 에 있으므로 **JetHT 단독 OR** 로 축약해야 하며, 2018 path 이름도 다르다
  (`…TriplePFBTagDeepCSV_4p5` 등). 또한 미인식 Data PD 는 E`CONFIG_BAD_RUNINFO` 로
  즉시 종료하므로, 2018 config 투입 전에 이 로직을 연도/era map 으로 확장해야 한다
  (코드에 이미 TODO 존재: L297).
- 지금은 **문서화만** 했다 — 코드 변경 없음. 실행 계획은 workspace
  `00_CONTEXT_ExpandedTtbarId_NtupleForge_Migration.md` Track D5.

## 2026-07-26 — data/samples_2018UL.json 추가 (2018UL 확장 준비, 코드 무변경)

- `data/samples_2018UL.json` 신규 (85 샘플 = 77 MC + 8 Data): das_path/nevents/nfiles 는 DAS 스캔
  (`NtupleForge/script/das_ul18_scan_20260726_1657.log`), xsec/BR/kfactor/refs 는 `samples_2017UL.json`
  에서 복사 (13 TeV 동일값). 생성기: `NtupleForge/script/build_ul18_from_log.py`.
- OPEN 항목이 파일 `_meta`/`note` 에 명시됨: lumi 59.83 /fb 는 잠정(사용자 확정 예정),
  `frac_neg_weight=null`(UL18 prescan 후 재산출), Data 는 non-GT36 선택(XPOG 권장 확인 필요),
  **BTagCSV PD 는 2018 에 부재**(2018 PD 개편) — FH data 는 JetHT 로 커버.
- analyzer 코드는 변경 없음 (2018 캠페인 실행은 별도 STEP 으로).

## 2026-07-12 — STEP 22.1: merge env 정책 — bootstrap 제거, 명시적 cmsenv

- 실전 첫 실행에서 76/76 전멸 — cmsenv 없는 셸의 local 병렬 실행이 `run_one_hadd.sh` 의 `/tmp/CMSSW_14_2_1` 자동 bootstrap 을 8-way 로 race 시킨 것이 원인. **정책 결정: 스크립트는 환경을 만들지 않는다** — bootstrap 전체 삭제. local 은 사용자가 cmsenv/ROOT 준비(없으면 preflight 에서 1회 명확히 실패), condor 는 `--cmssw-src`(기본: 제출 셸 `$CMSSW_BASE/src` 자동)로 worker 가 cmsenv. runner 는 optional 3번째 인자 `[cmssw_src]` 수용, hadd 미존재 시 exit 3 + 안내.
- Detail: [`changes/STEP_22_merge_outputs_autodiscovery.md`](changes/STEP_22_merge_outputs_autodiscovery.md) 후속 섹션.

## 2026-07-12 — STEP 22: merge_outputs.py — AnalyzerOutput 자동 발견 hadd (local 병렬/condor)

- 신규 `outputMerger/merge_outputs.py`: `<base>/<proc>/<proc>_*.root` → `<base>/<proc>.root` merge 를 **디렉토리 자동 발견**으로 수행 — 구 merge_submitter.py 의 하드코딩 목록(stale 이름으로 다수 샘플이 조용히 누락될 상태) 제거. `--mode local --jobs N`(병렬 + 완료 대기 + OK/FAIL 요약 + exit 전파) / `--mode condor`(proc 당 1 job, submit_hadd_validation 템플릿), `--only/--exclude/--skip-existing/--list/--dry-run`. 실행 단위는 검증된 `run_one_hadd.sh` 재사용.
- 검증: 74개 실디렉토리명 가짜 트리 + stub runner 로 발견/필터/병렬/실패복구/condor 생성 end-to-end 테스트 전부 통과. 실제 hadd 는 Tier3 에서 `--list` → `--dry-run` 순 확인 권장.
- Detail: [`changes/STEP_22_merge_outputs_autodiscovery.md`](changes/STEP_22_merge_outputs_autodiscovery.md).

## 2026-07-10 — STEP 21: plotter 전체 샘플 + compact/detailed 2-모드 그룹핑 + cutflow legend 수율 + PDF

- `plotter/samples_config.yml`: MC 24 → **61** (ttHToNonbb, tH, ttW/ttZ 잔여, single-top, VV, V+jets, DY 등 37개 추가; 미존재 파일은 자동 skip).
- `plotter/stack_plotter.C`: 그룹핑을 substring → **exact-name 테이블**로 교체 — 구 라우팅의 오배정(TTWW/TTWZ/TTWH/TTZH*/TTZZ*/TTTW 가 전부 "tt+V" 흡수) 수정. env `TTHH_PLOT_GROUPING` 으로 **compact(MC 10줄, 기본)/detailed(MC 13줄)** 2-모드; TTZHTo4b/TTZZTo4b 는 두 모드 모두 별도 줄(`tt+ZH/ZZ(4b)`). cutflow legend 수율을 Integral(≈noCut 지배) → **최종 cut bin(nTotal)** 으로 (`LegendYield`), legend 에 "yields after final cut" 헤더. 출력 PNG → **PDF**, 모드별 `plots_<grouping>/` 분리.
- `plotter/scenario_runner.py`: `--grouping compact|detailed|both` 추가(env 전달, 모드별 로그/PDF 카운트).
- 검증: 그룹핑 함수 g++ 실컴파일 전수 테스트(61 샘플 × 2 모드 + 회귀 방어) ALL PASSED. ROOT 렌더링은 로컬 1회 확인 권장.
- Detail: [`changes/STEP_21_plotter_full_samples_two_mode_grouping.md`](changes/STEP_21_plotter_full_samples_two_mode_grouping.md).

## 2026-07-10 — STEP 20: --report 요약 테이블 + --status 상세 + 조회 read-only + lazy tmp

- `submit_job_FH_Tier3_unified.py`: `--report` 는 이제 CRAB 스타일 요약 테이블(`sample|jobs|done|miss|done%` + TOTAL)만 출력하고, 구 상세 출력(weight + missing idx 목록)은 신규 `--status` 로 이동. 조회 모드를 완전 read-only 화 — report 가 `arguments_<sample>.txt` 를 빈 파일로 truncate 하던 부작용 수정, output dir mkdir/chmod skip. tmp 디렉토리는 lazy 생성으로 전환: 첫 per-job filelist 를 쓸 때만 생성 → 모든 호출이 샘플마다 빈 `tmp_<sample>_<ts>/` 를 만들던 리터 제거 (빈 tmp 존재 = 그 invocation 에서 재큐 0건이던 원인 규명 포함). 판정 기준은 STEP 19 종료 마커 그대로.
- Detail: [`changes/STEP_20_report_table_status_readonly.md`](changes/STEP_20_report_table_status_readonly.md).

## 2026-07-10 — STEP 19: report/resubmit 완료판정 = 종료 마커 + non-tt ttCat 요약 게이트

- `submit_job_FH_Tier3_unified.py` `_output_is_complete` (non-prescan): "non-empty TTree" 기준 → **`cutflow_w_full` 종료 마커** 기준으로 교체. selection 을 아무 이벤트도 통과 못한 정상 job(저-HT WJets/QCD 에서 구조적 발생)의 거짓 missing(→ 영구 재제출 루프)과, STEP_15 §6 의 고위험 half-written 거짓 complete 를 동시에 해소. 기존 output 에 소급 적용 — 재실행 없이 `--report` 재실행으로 충분. prescan 분기 불변.
- `ttHHanalyzer_unified.cc` `printTtCatSummary`: pair 비교(ANA_GENPART vs ANA_GENID)를 `IsTtbarFamily` 로 게이트. non-tt 샘플은 genTtbarId 디코드가 noTT 를 표현할 수 없어(NanoAOD 가 모든 MC 에 gtid≥0 기록) agreement 0% 가 정의상 결과 — "~97% 기대" 라벨 대신 SKIPPED 사유를 출력. 진단 stdout 만 변경, 물리/branch 무영향. **analyzer 재빌드 필요.**
- Detail: [`changes/STEP_19_completion_marker_and_ttcat_note.md`](changes/STEP_19_completion_marker_and_ttcat_note.md).

## 2026-06-30 — fix: make_filelists TTbar/ttbb disk-directory keys

- `make_filelists.py` `SAMPLE_MAP`: corrected 6 keys (on-disk dataset dir names) that STEP 18 had wrongly renamed to the short_name form (`TTbar_Hadronic_…`, `TTbb_4f_TTbar_…`). Real dirs are `TTToHadronic_…` / `TTToSemiLeptonic_…` / `TTTo2L2Nu_…` and `TTbb_4f_TTTo*_…`; this fixes the 6 `[MISSING] Directory not found` errors. short_names (values, = project keys) unchanged, so analyzer/xsec_db/yml/group-map are unaffected. Restores the module's own documented invariant ("on-disk primary dataset 이름은 불변").

## 2026-06-30 — STEP 18 (part C): exit-code system + correction-path policy + docs

- Added `include/ExitCodes.h` (canonical, collision-free exit codes) and `include/ConfigPath.h` (path resolver with no code default). Remapped all scattered exit codes in `ttHHanalyzer_unified.cc`, `src/CorrectionsManager.cc`, `src/ExpandedTtbarId.cc`, `src/StitchFactors.cc` to the new scheme; fixed the `41`/`43` collisions. → see [`DECISIONS.md`](DECISIONS.md) D-2026-06-30-C.
- Replaced the submitter's blank-means-default path export with the explicit-policy loop (E12/E13, `__NULL__` sentinel, always-export); numbered the xsec/prescan FATALs (E20/E21); taught the yml loader `null`→None. → D-2026-06-30-D.
- Updated `AnalyzerConfig/Tier3_2017_FH_unified_{prescan,main,btagtrig}.yml` `common.path_*` (jsonpog/golden explicit; the four derived corrections `null`=disabled). → D-2026-06-30-E.
- Created the guideline-conformant doc set: [`README.md`](README.md), [`STATUS.md`](STATUS.md), [`DECISIONS.md`](DECISIONS.md), this file, [`reference/ERROR_CODES.md`](reference/ERROR_CODES.md), [`reference/CONFIG_PATHS.md`](reference/CONFIG_PATHS.md); copied the documentation guideline into `docs/`.
- Detail: [`changes/STEP_18_ntupleforge_naming_and_kfactor_cleanup.md`](changes/STEP_18_ntupleforge_naming_and_kfactor_cleanup.md) (part C section).

## 2026-06-30 — STEP 18 (parts A–B): NtupleForge naming + k-factor cleanup

- Adopted NtupleForge campaign names as the project-wide sample key (analyzer/config/plotter/b-tag-SF). k-factors folded into notes (`kfactor=1.0`). → D-2026-06-30-A, D-2026-06-30-B.
- Detail: [`changes/STEP_18_ntupleforge_naming_and_kfactor_cleanup.md`](changes/STEP_18_ntupleforge_naming_and_kfactor_cleanup.md).

## 2026-06-29 — STEP 17 — full-NanoAOD dataset + categorization

- Migrated the analyzer to the `ttHH2017UL_fullNano_v20` campaign and to standard `genTtbarId` decode. Detail: [`changes/STEP_17_fullNano_dataset_and_categorization.md`](changes/STEP_17_fullNano_dataset_and_categorization.md).

## Earlier — STEP 0–16

See the per-step records in [`changes/`](changes/) (indexed by [`changes/README.md`](changes/README.md)). Notable: STEP 11 (xsec_db single source), STEP 12 (fb units), STEP 14 (3-tier weight + MET-CR + norm check), STEP 16 (SF toggles + output-dir split), STEP 4 (paths/yml FATAL — superseded in part by STEP 18 part C path policy).

## 2026-07-06 — Data era-check fix (STEP_18 follow-up #2)
- **Bug**: all Data jobs in `main` mode died with `[ERROR] eraName mismatch ... eraName: D, sampleName: BTagCSV_Run2017D` followed by a segfault. Root cause: `ttHHanalyzer_unified.h::extractEraFromSampleName()` still assumed the *legacy* Data naming (`JetHT_B`, single-letter last token). STEP_18 renamed Data samples to `<PD>_Run2017<E>`, so extraction returned `""` → mismatch → FATAL. MC does not take this branch, hence "MC runs fine, Data dies".
- **Fix 1**: `extractEraFromSampleName()` now recognizes `Run20YY<E>` (era-agnostic: 2016/2017/2018; tolerant of `_ext1` suffixes) with the legacy single-letter form kept as fallback. Unit-tested against 13 old/new/edge sample names.
- **Fix 2**: the two runinfo exits (`MC must not define era`, `era mismatch`) now emit `[FATAL][E11]` (tthh::CONFIG_BAD_RUNINFO) and use `std::_Exit` — `std::exit` triggered a ROOT-teardown segfault that polluted the exit status (139 instead of a canonical code). `ExitCodes.h` is now included by the header directly.
- Files: `ttHHanalyzer_unified.h` only. Rebuild required (`make`).
