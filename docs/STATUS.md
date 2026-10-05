# Status — ttHH(HH→bbbb) Fully-Hadronic Analysis

> **Purpose:** the single place that answers "where are we right now?" — current state, what is ready, what is pending, and what is OPEN.
> **Audience:** anyone starting a session.
> **Status:** living · last meaningful update **2026-10-05** (STEP 24 커밋 B: 진짜 eventBuffer.h 와 2024 analyzer·제출기, smoke_2024 · 묶음 1: 2024 eventBuffer·PU 도구와 입력, Y1 도구; D-2026-10-05-A · 묶음 2: analyzer 의 2024 코드는 진짜 header 와 함께 커밋 대기, Stage 3 도구·yml·임시 σ, 오프라인 smoke 31/31; D-2026-10-05-B·C) · 2026-10-04 (KNU 의 긴 단계는 condor job, 실행 기록은 `runlogs/` 에 커밋 — STEP 23) · 2026-10-03 (Stage 0 의 payload·lumi 실측 반영, eventBuffer 생성기 = 사용자 fork treestream 점검, 그 패치 커밋 `8be42e8`; KNU 의 pull 과 clean 빌드) · 2026-10-02 (2024 먼저 결정, 단계별 계획 [`PLAN_v15_2018UL_2024.md`](PLAN_v15_2018UL_2024.md) §9, 2017 lumi 42.07 반영) · 그 전 2026-10-01 (v15 계획 PROPOSED) · 2026-07-26 (2018UL 준비: samples_2018UL.json, --preflight, OPEN #5).
> **Links:** decisions [`DECISIONS.md`](DECISIONS.md) · history [`CHANGELOG.md`](CHANGELOG.md) · exit codes [`reference/ERROR_CODES.md`](reference/ERROR_CODES.md) · path policy [`reference/CONFIG_PATHS.md`](reference/CONFIG_PATHS.md).

## Bottom line

The analyzer + submitter are migrated to the `ttHH2017UL_fullNano_v20` ntuple campaign and use NtupleForge sample names throughout. As of 2026-06-30 the code has a **collision-free numbered exit-code system** (fail-fast, Condor-detectable) and an **explicit correction-path policy** (no silent defaults; `null` = disable; required ≠ null). **Immediate runnable step: `prescan` (MC and Data).** `main`/`btagtrig` need the per-sample prerequisites below before they will run.

**2026-10-01 (PROPOSED):** NtupleForge 의 v15 ntuple(2018UL 생산 끝, 2024 는 95 % 쯤)로 가는 작업 목록·순서·연도별 드라이버 설계는
[`PLAN_v15_2018UL_2024.md`](PLAN_v15_2018UL_2024.md). 그 §2 의 조사: 지금 코드는 v15 에서 `Jet_jetId` 부재로 0 event 이고 rho·MET·electron ID 가
0 으로 읽히며(무증상), `make_filelists.py` 는 CRAB `failed/` 사본을 넣고, 출력 디렉터리에 연도가 없어 2017 출력을 덮어쓴다. 그 계획의 A0~A2
전에는 2018/2024 job 을 내지 않는다. 아래 OPEN 6 의 P0 7 건은 여전히 빌드 전이다.

**2026-10-02 (DECIDED, 사용자):** v15 의 첫 대상은 **2024** 다([`DECISIONS.md`](DECISIONS.md) D-2026-10-02-A; 2017 은 v15 재생산 뒤, 2018 v15 와
v9 는 필요할 때). 단계(Stage 0~9)와 통과 기준, 연도별 event cleaning 표, eventBuffer 정책, ttbar ID 검증, QCD 비교의 정의는
[`PLAN_v15_2018UL_2024.md`](PLAN_v15_2018UL_2024.md) §9. **지금은 Stage 0**(확인만: 2024 payload·lumi·HLT prescale 은 lxplus, eventBuffer
생성기 찾기와 지금 HEAD 의 빌드는 KNU; 워크스페이스 RUNBOOK §20). 같은 날 2017 lumi 를 main·prescan yml 과 plot 표기에서 42.07 로 고쳤다
(D-2026-10-02-D N1; 41.48 로 만든 2017 산출물은 없다). 2024 의 tt+nb 분할과 `TT4B` 는 2024 patch(NtupleForge V6 = TTHHGenCategoryTools O5,
미착수) 뒤다.
**10-02 Stage 0b (lxplus brilcalc) 끝:** 2024 C–I lumi 109.816 fb⁻¹(normtag_PHYSICS; PdmV 표와 같음), golden JSON 은 2026-08-04 판
(md5 `3f8543e8…`), 2024 hadronic·muon 후보 trigger 는 모두 prescale 없음(4J3T 는 초기 DeepJet 판과 PNet 판의 OR). 숫자와 해석:
[`reference/LUMI_SOURCES.md`](reference/LUMI_SOURCES.md) §6, 남은 결정 PLAN §9.6 D4·D14. 남은 Stage 0: payload 목록(a), KNU 의 생성기 찾기와 빌드(c).
**10-03 Stage 0 정리:** (a) payload 실측을 PLAN §9.2 의 2024 열에 — UParTAK4 WP, **b-tag shape SF 없음**(fixed-WP kinfit b SF 뿐, D10), veto map
`Summer24Prompt24_RunBCDEFGHI_V1` 의 `jetvetomap`(D11), jet ID `AK4PUPPI_Tight`, JEC `Summer24Prompt24_V1`, JER `Summer23BPixPrompt23_RunD_JRV1`,
**LUM 2024 없음 → PU weight 직접**(D15). (b) 4b: PHYSICS 유효 lumi 109.157(IsoMu24), HLT 기록 없는 run 380126–380128(LUMI_SOURCES §6.4, D14).
(c) 생성기는 사용자 fork treestream `forTTHH_v1` — 10-03 점검에서 조용히 틀리는 경우 일곱을 재현하고 패치와 시험을 워크스페이스
`treestream_review_2026-10-03/` 에 둠(적용은 사용자; PLAN §9.3), 2024 HLT branch 가 era 마다 달라 chain 이 죽을 수 있음(D16). 남은 것:
KNU 의 빌드·시험·확인 셋(RUNBOOK §20 8~12).
**10-03 (2):** 패치는 사용자가 fork 에 커밋(`8be42e8`, 패치와 22 파일 모두 같음). KNU 의 이 저장소는 `c1178fe5`(07-29)에 있고, KNU 에서만 고친
`AnalyzerConfig/Tier3_2017_FH_unified_main.yml` 을 들어오는 `1aa450ef`(2017 lumi 42.07)도 바꾸므로 `git pull --ff-only` 가 거절됐다 → 백업 뒤
그 파일만 stash·pull·pop(RUNBOOK §20 6). KNU 에는 커밋되지 않은 2017 산출물 수정도 있다(`DerivedCorr/stitchFactors/stitch_factors_2017.json`,
`prescan_summary/prescan_summary.{csv,json}`) — 2017 회귀 기준 출력(Y1) 전에 커밋할지 되돌릴지 정한다(사용자). 최상위 `Makefile` 은 header
의존성이 없어 header 를 바꾼 뒤에는 `make clean && make -j4`(PLAN §9.3). 남은 것: lxplus §20 5·11, KNU §20 6·9·10·12.
**10-04:** KNU 의 오래 걸리는 단계는 condor job 으로 돌리고(`tools/runlog/condor_run.sh`), 기록 `runlogs/run_<step>_<UTC>.log` +
`runlogs/LEDGER.tsv` 를 커밋한다([`DECISIONS.md`](DECISIONS.md) D-2026-10-04-A, [`changes/STEP_23_runlog_condor.md`](changes/STEP_23_runlog_condor.md);
오프라인 시험 71·19, 독립 검토 반영). Stage 0 의 남은 KNU 단계(단위 시험, D14, D16, treestream 시험)도 이 방식으로: 워크스페이스 RUNBOOK §20 13.
RUNBOOK §20 9·10 의 heredoc 은 새 PyROOT 에서 깨진 파일 하나에 죽어서 스크립트(`tools/stage0/`)로 바꿨다.
**10-04 (2):** KNU 가 `90feebc0` 을 받았다(4 개의 로컬 수정은 그대로). 시험 69/2(두 줄 확인 중)·19/19. **10-03 의 빌드는 끝나지 않았다**:
`lib/` 는 있고 `ttHHanalyzer_unified` 는 없음(링크 전에 끊김) → condor 로 다시 빌드(RUNBOOK §20 13 (d) 의 대안). 그 기록의 `EXIT : 0` 은
`build_check.sh` 의 빈틈이었고 고쳤다(끝난 빌드만 0). `runlog.sh` 의 signal 처리와 시험(83 check)도 보강 —
[`changes/STEP_23_runlog_condor.md`](changes/STEP_23_runlog_condor.md) "2026-10-04 (2)". **10-05 의 condor job**(기록 `runlogs/`,
KNU 커밋 `39936117`): `knu_build` EXIT 0(cluster300, 20 분, 실행 파일 02:52 KST), `knu_unittests` EXIT 0(`PASS 47 / FAIL 0`, KNU 판
`prescan_summary.json` 으로), `knu_d14_runs` EXIT 0 — run 380126/380127/380128 이 우리 2024C 출력에 있다: LS 991/533/393, skim 뒤 event
708,132/307,634/179,727, 파일 93/44/30(269 파일 중; D14 의 입력), `knu_d16_branchsig` 는 도는 중(중간 출력은 LUMI_SOURCES §6.4 와 맞음:
Run2024C 파일의 일부에만 PNet 4J3T branch). `39936117` 에는 기록 말고도 KNU 의 2017 로컬 수정 넷(결정 1 이 커밋 쪽으로 됨 — 사용자 확인
대기), `filelistTier3/`(문서가 말하는 2017 기준 목록이라 그대로 둠)와 빌드 산출물·작업 파일 15 개가 들어갔다 → 산출물은 `.gitignore` 에
더하고 KNU 에서 추적만 끊는다(사용자 결정 10-05; RUNBOOK §20 14). analyzer: 2017 은 지금 실행 파일로 돌 수 있다; 2024 는 PLAN §9
Stage 1(eventBuffer 재생성과 필수 branch 검사)·Stage 2(`EraConfig` 2024) 뒤 — 사용자 결정(10-05): 다음은 Stage 1·2.
**10-05 (STEP 24 묶음 1, 도구만):** Stage 0 의 KNU 기록(`c45304aa`): D16 Data 32 dataset 7,787 파일 못 읽은 것 0, 차이는 HLT 뿐; treestream
확인 SUMMARY PASS 두 번; 시험 83/0·18/0. 사용자 결정 [`DECISIONS.md`](DECISIONS.md) D-2026-10-05-A: 첫 plot 은 2024 FH, trigger·b-tag SF 없이,
σ 는 임시값 표시; lumi 109.816(D14); data 는 SR bin 만 가림(D-2026-10-02-E DECIDED); PU 는 DQM 의 공식 2024CDEFGHI 히스토그램, MC 분포는 skim
없는 ZZ 파일럿(D15). 묶음 1: eventBuffer 합집합의 KNU 기록 도구(header 는 맥에서 그 기록으로), PU weight 도구, 2017 회귀 기준(Y1) 도구와 시험
40(독립 검토 9 건 반영), 2024 golden JSON(8월 판, md5 확인)과 PU 입력 셋 — [`changes/STEP_24_stage1_2_2024.md`](changes/STEP_24_stage1_2_2024.md); KNU 단계는 워크스페이스
RUNBOOK §21. MC branch 스캔 끝(`406fa531`): 60 dataset 19,111 파일 모두 dataset 당 branch 집합 하나, TTbar_Hadronic 의 출력 하나가 크기 0
(NtupleForge A29).
**10-05 (STEP 24 묶음 2):** analyzer 의 2024 코드(EraConfig 2024, JEC/JER 입력을 payload 이름으로, jet ID·veto map·우리 PU JSON, v15 이름,
2024 HLT·PD·MET filter, 필수 branch 검사와 여러 파일 job 의 branch 집합 검사, 2024 의 tt+nb lookup 없이 — D-2026-10-05-C)와 제출기(2024 PD·era,
`path_pu_json`, 연도가 붙는 출력·condor·filelist 디렉터리, 2024 preflight). Stage 3: `tools/stage3/make_filelists_v15.py`, `data_lumi_check.py`,
`data/samples_2024.json`(**임시 13.6 TeV σ**, 60 MC 모두, 출처·신뢰도 표기), 2024 prescan·main yml. 컨테이너 시험: 빌드 OK, unit PASS,
`test_stage1` 42/42, 새 오프라인 smoke 31/31(2017 은 옛 빌드와 출력 동일); 독립 검토 11 건 반영. C++·제출기·`test_EraConfig.cc` 는 KNU 기록으로
만들 진짜 `eventBuffer.h`·PU JSON 과 **함께** 커밋한다(그 전에 KNU 에서 빌드하지 말 것: Y1 `before` 는 지금 실행 파일로) — STEP 24 §6–9.
Y1 스크립트 수정: TTbar_Hadronic 은 2017 tt+nb lookup 이 있어야 돈다(없으면 E11 로 MC 가 비교에서 빠졌을 것).
**10-05 (KNU 첫 기록, `595b4676`):** PU MC 분포 OK → `DerivedCorr/PU/2024_Summer24/puWeights_2024.json`; 2024 filelist 92 key OK; **2017 v20 은
KNU 에서 지워짐**(manifest `MISSINGBASE`, Y1 입력 없음; 사용자 확인) → D-2026-10-05-D: v9 branch 는 2026-06-29 header 에서
(`tools/stage1/variables_v9_2017UL_fullNano_v20.txt`, manifest 의 `--extra-variables`), 실제 2017 Y1 은 지금은 없음(컨테이너 비교로 대신).
다음: manifest 다시(RUNBOOK §22) → 맥에서 header → 커밋 B → KNU 빌드·2024 smoke → prescan → main → plot.
**10-05 (STEP 24 커밋 B):** manifest 다시(`66ea1301`, `RESULT OK`) → 진짜 `include/eventBuffer.h`(764 record, md5 기록과 같음, stamp)와
묶음 2 의 C++·제출기를 함께. 더한 것: 로그 줄 셋(tagger 이름, JEC/JER payload 이름, 4J3T yes/no — 물리 없음), 제출기의
`files_per_job_data`(2024 main: MC 10, Data 1; prescan 20), 진짜 파일 smoke `tools/stage3/smoke_2024.sh`. 컨테이너: 빌드 OK, unit PASS,
`test_stage1` 48/48, offline smoke 34/34(2017 은 옛 빌드와 같음), smoke_2024 합성 입력 69/69, 독립 검토 지적 셋 반영 — STEP 24 §11. 다음(RUNBOOK §23): KNU clean
빌드 → smoke_2024 → proxy → prescan(983 job) → consolidate → main(5,291 job, `--trigsf off --btagsf off --btagrw off`) → merge → plot.

## What is ready (DECIDED)

- **output merge (STEP 22, 2026-07-12):** `outputMerger/merge_outputs.py` — base 디렉토리 자동 발견 hadd. local 병렬(`--jobs N`, 요약표) / condor 모드, `--skip-existing` 재시도. 상세: [`changes/STEP_22_merge_outputs_autodiscovery.md`](changes/STEP_22_merge_outputs_autodiscovery.md).
- **plotter (STEP 21, 2026-07-10):** 전체 61 MC 샘플 등록, compact/detailed 2-모드 그룹핑(env `TTHH_PLOT_GROUPING` / runner `--grouping`), exact-name 매칭(구 substring 오배정 수정), cutflow legend = 최종 cut 수율, PDF 출력. 상세: [`changes/STEP_21_plotter_full_samples_two_mode_grouping.md`](changes/STEP_21_plotter_full_samples_two_mode_grouping.md).
- **report 요약 테이블 / --status (STEP 20, 2026-07-10):** `--report` = 전체 요약 테이블(done/miss/done% + TOTAL), `--status` = 샘플별 상세(missing idx). 조회는 read-only(인자 파일 truncate 부작용 수정), tmp 디렉토리는 job 을 실제 큐잉할 때만 생성. 상세: [`changes/STEP_20_report_table_status_readonly.md`](changes/STEP_20_report_table_status_readonly.md).
- **report/resubmit 완료판정 (STEP 19, 2026-07-10):** `--report`/`--resubmit` 은 이제 종료 마커(`cutflow_w_full`) 기준으로 판정한다. selection 통과 0건인 정상 job(저-HT WJets/QCD)의 거짓 missing → 영구 재제출 루프와 half-written 거짓 complete 를 동시에 해소. 기존 output 에 소급 적용 — 재실행 없이 `--report` 재실행. 상세: [`changes/STEP_19_completion_marker_and_ttcat_note.md`](changes/STEP_19_completion_marker_and_ttcat_note.md). **주의: analyzer(`ttHHanalyzer_unified.cc` — non-tt ttCatSummary 게이트) 재빌드 필요.**
- **Naming:** all components key off NtupleForge campaign names (see [`DECISIONS.md`](DECISIONS.md) D-2026-06-30-A and `changes/STEP_18_*.md`). yml↔xsec_db verified consistent (0 missing MC, 0 null-xsec MC); process-group keys (8) intact.
- **Normalization inputs:** `data/samples_2017UL.json` (xsec_db, 91 entries) updated; k-factors folded to notes (`kfactor=1.0`).
- **Exit codes:** `include/ExitCodes.h` is the source of truth; mirrored in [`reference/ERROR_CODES.md`](reference/ERROR_CODES.md). Submitter mirrors the 10–29 band.
- **Path policy:** `include/ConfigPath.h` + submitter enforce the contract in [`reference/CONFIG_PATHS.md`](reference/CONFIG_PATHS.md). yml `common.path_*` updated accordingly.

## Current correction state (DECIDED 2026-06-30)

| Correction | yml value | State |
|---|---|---|
| jsonpog (JME/PU/b-tag SF) | real path | active (CVMFS) |
| golden JSON (Data lumi mask) | real path | active for Data — **OPEN:** verify absolute path |
| trigger SF | `null` | **disabled** — not re-derived for new dataset |
| b-tag norm reweight | `null` | **disabled** — `_skipBtagReweight`; tt+nb-key JSON not regenerated |
| ttbar stitching | `null` | **disabled** — stitch factors not re-derived for new dataset |
| Expanded_genTtbarId (tt+nb) | `null` | **disabled** — `ttnb_<sample>.root` not regenerated. **2026-07-27:** this analyzer-side runtime lookup is now the **explicit interim contract** — the plan to move the role into NtupleForge (baked-in branch) is DEFERRED until after the UL18 control plots (see `NtupleForge/docs/01_STATUS.md` and workspace `00_CONTEXT…md` §1). For 2018 it gets re-enabled by pointing `path_expanded_ttbarid_dir` at `DerivedCorr/expandedTtbarId/2018` (patches must be extracted with the legacy names `ttnb_<projectKey>.root` / tree `TtNb`). |

Because `--trigsf` defaults to `on`, running `main` as-is hits **E13** (required path is null). Until trigger SF is re-derived, run `main` with `--trigsf off --btagrw off`, or provide real paths.

## Pending / next steps (OPEN)

1. **Regenerate per-dataset derived inputs** for `ttHH2017UL_fullNano_v20`, then flip the corresponding yml paths from `null` to real:
   - `DerivedCorr/expandedTtbarId/ttnb_<sample>.root` (tt+nb sub-codes 61/62/71/72).
   - stitch factors (`compute_stitch_factors.py`) — consistency required between analyzer and `BTagSFProcessor` Pass-1.
   - trigger SF (re-derive at ≥6-jet baseline, nbjet axis ≥0 post-stitching).
   - b-tag norm reweight 8-group JSON with tt+nb keys.
2. **`ExpandedTtbarId` loader: accept BOTH patch tree names** (registered 2026-07-28, from
   `TTHHGenCategoryTools`). Today the loader hard-defaults to `ttnb_<sampleKey>.root` / tree
   `TtNb`, and the 2018 patches are being produced with exactly that convention on purpose, so
   **nothing is broken right now**. The cleanup is to make `loadFromDir()` try `TtbarIdPatch`
   first and fall back to `TtNb`, so the newer producer-side convention
   (`ttbarIdPatch_<KEY>.root` / `TtbarIdPatch`, TTHHGenCategoryTools D12) can be adopted without
   a flag day.
   - **Do NOT simply flip the default.** The 2017 patches in
     `TTHHGenCategoryTools/Validation/lookup/` carry the tree name **`TtNb` inside the file**, so
     renaming files does not help — flipping the default would make 2017 unreadable and force a
     re-extraction from KNU Tier3.
   - Failure mode if this is ever gotten wrong is **silent**: the loader goes INACTIVE (not
     fatal) and the analyzer uses the raw NanoAOD `genTtbarId`, i.e. plots come out with the
     tt+nb extension **not applied**. Any change here must be verified by checking that
     `ExpandedTtbarId::active()` is true and `size()` matches the patch row count.
   - Counterpart record: `TTHHGenCategoryTools/docs/07_analyzer_integration.md` §4 and
     `docs/01_status.md` O2.

3. **Verify provisional cross sections:** every `verify_xsec:true` entry in `xsec_db` (e.g. `ST_s_had`, `ST_tW_*`, `WW/WZ/ZZ`) against XSDB / GenXSecAnalyzer before unblinding.
4. **Compile/run validation** of the analyzer and auxiliary tools (BTagSF / TriggerStudy / plotter / outputMerger) in CMSSW_14_2_1 — they have been syntax/brace-checked here, not built. See [`DECISIONS.md`](DECISIONS.md) on the validation method available in this environment.
5. **Verify** `make_filelists.py` merges base + ext datasets (`os.walk`).
6. **2018UL 대응 (OPEN, 2026-07-26 등록 · 2026-07-27 전수 감사로 확장).**
   **실행 계획은 워크스페이스 `RUNBOOK_UL18_to_controlplots.md` §4** (Phase 3 = 이 항목).
   목표가 "UL18 control plot 최단 경로"로 정해졌고, 아래 P0 를 고치지 않으면 **조용히 틀린
   결과**가 나온다. 감사 전문(**17항목** = P0 7 + P0′ 5 + P1 5, file:line)은 runbook §4 표.

   **P0 — 코드 수정 7건 완료 (2026-07-29). 단, 아직 빌드/실행 검증 전이다.**
   연도 의존 값은 전부 새 파일 **`include/EraConfig.h`** 로 모았다 (이 파일 밖에서
   `if (year=="2017")` 를 쓰지 않는 것이 규약). 조회 함수는 **모르는 연도에 exit 11**.
   상세 근거·AN 인용은 `CHANGELOG.md` 2026-07-29 항목.
   | # | 위치 | 조치 | 안 하면 | 상태 |
   |---|---|---|---|---|
   | 1 | `ttHHanalyzer_unified.h:1133` | `EraConfig::yearForCorr()` + unknown year FATAL | `yearForCorr=""` → E40 | ✅ |
   | 2 | `include/eventBuffer.h` | 2018 DeepCSV HLT 3개 추가 (**5곳**: 선언/`choose`/`select`/`output->add`/`initBuffers`) | 컴파일 불가 | ✅ |
   | 3 | `ttHHanalyzer_unified.cc:~305` | 연도별 path set, 2018 은 JetHT 단독 OR (veto 제거) + `requireTriggerBranches2018_()` | 4J3T event **조용히 폐기** | ✅ |
   | 4 | `ttHHanalyzer_unified.cc:~1217` | `EraConfig::usesL1Prefiring()` 로 gate + 적용 연도에서 0 이면 FATAL | 2018 branch 부재 → 0 → **전 MC weight 0 (무증상)** | ✅ |
   | 5 | `src/CorrectionsManager.cc:~256` | `EraConfig::goldenJsonFile()` (하드코딩 run range 제거) | 2018 Data 전부 E41 / unknown year 는 golden 없이 통과 | ✅ |
   | 6 | `src/CorrectionsManager.cc:~171` | era 문자 그대로 + **Data 의 MC-JEC fallback 폐지 → FATAL** | Run2018B/C 에 RunD JEC (조용한 fallback) | ✅ |
   | 7 | `ttHHanalyzer_unified.h:243` | `objectJet::configureBtagWP()` 주입, 2018 = 0.0490/0.2783/0.7100 (AN Table 32); static 초기값 **NaN** + 미주입 시 FATAL | **신호영역 정의가 틀어짐** | ✅ |

   **7건 전부의 공통 성질**: 고치지 않았을 때 크래시가 아니라 **무증상 오답**이었다. 그래서
   각 항목에 분기만 넣지 않고 "틀린 상태를 FATAL 로 만드는 장치"를 같이 넣었다.

   **곁가지 수정**: `2016PreVFP_UL`(코드 키) vs `2016preVFP_UL`(디스크 디렉토리) 표기 불일치.
   `runYear_` 는 경로 성분으로 그대로 쓰이므로 2016 은 어느 쪽이든 깨져 있었고, golden JSON 은
   조용히 `return` 했다. 생성자에서 `EraConfig::normalizeYear()` 로 정규화. **2016 은 여전히
   미검증이고 trigger path set 도 미정이라 명시적 FATAL** 이다.

   **검증 상태 (중요):**
   - ✅ `EraConfig.h` 단위 테스트 통과 — ROOT 불필요, 즉시 재실행 가능:
     `g++ -std=c++17 -I include -o /tmp/t test/test_EraConfig.cc && /tmp/t`
     그리고 `./test/test_EraConfig_fatal.sh` (unknown year → exit 11).
   - ❌ **`make` 미수행** (이 환경에 ROOT 없음). lxplus/KNU CMSSW_14_2_1 에서 필요.
   - ❌ **2017 재현성 미확인.** 2017 경로·WP·prefiring 로직은 전부 보존했으므로 결과가
     이전과 **동일해야 한다**. 2018 을 믿기 전에 이걸 먼저 확인할 것.

   **P0' — 배관(코드 아님, 필수):** `--year` allow-list 부재(`src/tnm.cc:273`) · **output 디렉토리에
   연도가 없어 2018 산출물이 2017 과 섞인다**(`submit_job_FH_Tier3_unified.py:158-162`) ·
   `prescan_summary.json` 연도 미분리 · **`make_filelists.py` Data 분기가 2017 하드코딩이라 2018
   Data filelist 가 생성되지 않는다**(`:250,261,263`) · `Tier3_2018_FH_unified_*.yml` 부재.

   **HEM15/16 — 방법론 결정 (2026-07-28): AN 추종, veto map 미사용.**
   AN-2022/122 v26 p.118 은 HEM 을 **veto 가 아니라 2018 MC 의 JES 변주**로 다룬다
   (`-1.57<φ<-0.87` & `-2.5<η<-1.3` → 20%, `-3.0<η<-2.5` → 35%) 그리고 그 영향이
   "negligible" 이라고 적는다. AN 214쪽 전문에 **jet veto map 언급은 0건**이고 코드에도
   loader 가 없다. 2018 에만 veto map 을 도입하면 2017 과 위상공간이 달라지고, JME TWiki
   자체가 다중 jet 분석의 통계 손실을 경고한다(이 분석은 ≥6 jet).
   → 값과 근거는 `EraConfig::hemJes()` / `inHemRegion()` 에 넣어 뒀다. **적용 코드는 아직
   없다** (P0′ 이후). 재검토 항목: JME `Summer19UL18_V1` /
   `h2hot_ul18_plus_hem1516_plus_hbp2m1` — 최종 결과 전 확인.
   요청된 **jet η-φ 2D map** 은 경험적 확인용으로 output histogram 에 넣을 예정(미구현).

   **P1 (첫 plot 직후):** HEM JES 변주 **적용 코드 미구현**(위 결정 참조) · trigger SF 기준 `IsoMu27`→`IsoMu24`
   및 `SelectionCuts.h:54` leadMuonPt 29→26 · 6th-jet/HT plateau 재확인 · JEC-unc loader 미호출
   (`CorrectionsManager.cc:65-80` vs `:381-404`, null deref) · lumi 정본(OPEN #6).

   **완료(2026-07-26~27, submitter):** Data era 정규식 `Run\d{4}([A-Z])$` 일반화,
   `--filelist-dir`, `--preflight`. **prescan(2018) 은 `loop()`/`createObjects()` 를 타지 않아
   P0 #1 만 고치면 바로 실행 가능**하다 → **P0 #1 완료됨 (2026-07-29)**. 남은 blocker 는
   코드가 아니라 배관(P0′): 특히 `make_filelists.py` 의 2018 Data 분기와 **output 디렉토리
   연도 분리** (없으면 2018 산출물이 2017 과 섞인다).

   (이하 원래 기록)
   `data/samples_2018UL.json` 준비 완료
   (85 샘플, NtupleForge `config_ttHH2018UL.yaml` 과 1:1). 오늘 들어간 submitter 변경은
   §"What is ready" 가 아니라 여기에 기록한다 — 아래 참조.
   - **prescan(2018) 은 지금 실행 가능하다.** `runPrescan()` 은 `loop()`/`createObjects()` 를
     타지 않으므로(`ttHHanalyzer_unified.cc:110-116`) 2017 전용 trigger 로직을 **전혀 거치지
     않는다**. 필요한 것은 Events 의 `genWeight`/`genTtbarId`/`run`/`lumi`/`event` 와 `Runs` tree 뿐.
   - **main/btagtrig(2018) 은 trigger PD 로직 확장이 선행되어야 한다 (blocker).**
     현 로직(`ttHHanalyzer_unified.cc` ~L295–360)은 2017 전용 orthogonality
     (4J3T→BTagCSV / 6J·HT→JetHT + veto)다. BTagCSV PD 는 2018 에 존재하지 않음이 DAS 로
     확정되었고 해당 path 들이 JetHT 에 있으므로 2018 은 **JetHT 단독 OR** 이어야 한다.
     **정정(2026-07-26 감사)**: JetHT 자체는 인식되는 PD 이므로 "즉시 FATAL" 이 아니다 —
     더 위험한 건 **조용한 실패**다. 2017 전용 HLT branch 들이 2018 NanoAOD 에 없으면
     eventBuffer 가 0(false)로 두므로 `passHadTrig` 가 항상 false 가 되어 **경고 없이 0 event**
     가 된다. (코드 TODO: L297 — era map)
   - **2018 HLT path 실측 확정 (2026-07-27, UL18 NanoAODv9 파일 직접 확인).** 존재하는 것:
     `HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5` (4J3T 대응),
     `HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94` (6J2T 대응),
     `HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59` (6J1T 대응), `HLT_PFHT1050`,
     `HLT_IsoMu24`, `HLT_IsoMu27`. **존재하지 않는 것**: 2017 CSV 계열 6개 전부
     (`HLT_PFHT300PT30_..._TriplePFBTagCSV_3p0`, `HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2`,
     `HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5`, `HLT_HT300PT30_QuadJet_..._TripeCSV_p07`,
     `HLT_PFHT380_SixJet32_DoubleBTagCSV_p075`, `HLT_PFHT430_SixJet40_BTagCSV_p080`).
     → era map 작성 시 이 대응표를 그대로 쓰면 된다. 근거 로그: NtupleForge
     `docs/02_CHANGELOG.md` 2026-07-27 항목.
   - **완료(2026-07-26, submitter):** Data era 추출 정규식을 `Run2017([A-Z])$` →
     `Run\d{4}([A-Z])$` 로 일반화 (기존 정규식은 `JetHT_Run2018A` 를 못 잡아 FATAL 이었다).
     `--filelist-dir` 옵션 추가(연도별 filelist 디렉토리; `make_filelists.py 2018` →
     `filelistTier3_2018/`). `--preflight` 읽기 전용 사전 점검 추가.
   - ~~HEM15/16 veto (2018 전용) 적용 + veto map 검증.~~ → **방법론 결정됨 (위 "HEM15/16" 참조):
     veto 가 아니라 JES 변주. veto map 미사용.** L1 prefiring 은 2016/2017 전용 → 2018 비활성
     (**P0 #4 로 구현 완료**).
   - 2018 lumi **확정 = 59.56 /fb** (0.84%), `samples_2018UL.json._meta` 반영 완료 (OPEN #6 참조).
   - Data PD 는 non-GT36 선택됨 — ttHH AN 사용 샘플 기준으로 재확인 필요(GT36 대안은 config 주석).
7. **lumi — 값은 확정됐고(2026-07-27) 2017 코드 반영은 2026-10-02 에 끝났다. 남은 것은 beamer 표기와 2018 brilcalc (OPEN).**
   **정본 = LUM POG "Recorded Golden Legacy": 2017 = 42.07 fb⁻¹ (0.82%), 2018 = 59.56 fb⁻¹ (0.84%).**
   출처: [LumiRecommendationsRun2](https://twiki.cern.ch/twiki/bin/view/CMS/LumiRecommendationsRun2)
   (index: [TWikiLUM](https://twiki.cern.ch/twiki/bin/viewauth/CMS/TWikiLUM)), 인용 CMS-PAS-LUM-20-001.
   - **2018: 59.83 → 59.56 정정 완료.** 59.83 은 LUM POG 페이지에 **존재하지 않는 값**이었다.
     `samples_2018UL.json._meta` + 생성기 `build_ul18_from_log.py` 둘 다 반영(재생성 byte-identical 확인).
   - **2017: 42.07 이 옳다는 것이 확정됐다** — 우리 brilcalc 실측 42.0688 과 일치.
     따라서 `AnalyzerConfig/Tier3_2017_FH_unified_{main,btagtrig}.yml` 의
     `common.lumi_fb_inv = 41.48`(**weight 에 실제 사용**, `submit_job_FH_Tier3_unified.py:614`)은
     **확정적으로 틀렸다**. 42.07/41.48 = **1.42%** 만큼 전 MC weight 가 어긋난다.
   - ✅ **2026-10-02 반영 (사용자 결정 N1, [`DECISIONS.md`](DECISIONS.md) D-2026-10-02-D)**: main·prescan yml 을 42.07 로
     (btagtrig 는 07-29), 표기 `stack_plotter.C:639` 42.07, `plot_btag{,_pyroot}.py` "42.1 fb^{-1}", `compute_stitch_factors.py`
     의 `LUMI_PB_INV` 42070(진단용, stitch 배수에서는 상쇄). 이 코드로 만든 2017 산출물이 없어(N0) 재생산할 것도 없다.
   - ❌ 남음: beamer `config.tex:31` 41.48(다른 저장소) — 그래서 **같은 PDF 안에서 본문 41.48 vs backup 42.07** 이 여전히
     인쇄된다. 2018 brilcalc 실측(X14).
   - **전수 목록·절차·재생산 판단 = [`reference/LUMI_SOURCES.md`](reference/LUMI_SOURCES.md)**
     (9개 지점, 반영 여부를 ✅/❌ 로 표시).
   - `--preflight` 가 yml↔`_meta` 불일치를 WARN 으로 보고한다.

## Known constraints (invariants — see DECISIONS.md)

- No physics-logic change without an Analysis-Note justification.
- `main`/`btagtrig` require each sample in both `xsec_db` and `prescan_summary` (else E20/E21) → prescan first.
- ttbar stitching must be applied identically in the analyzer and the b-tag-SF Pass-1 normalization (mismatch breaks yield closure).
- Process-group keys (`tt+LF/tt+cc/tt+B/tt+nb/ttH/ttHH/ttZH4b/ttZZ4b`) and stitch category labels are load-bearing and are **not** renamed by sample-name changes.
