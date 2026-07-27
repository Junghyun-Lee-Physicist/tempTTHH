# Status — ttHH(HH→bbbb) Fully-Hadronic Analysis

> **Purpose:** the single place that answers "where are we right now?" — current state, what is ready, what is pending, and what is OPEN.
> **Audience:** anyone starting a session.
> **Status:** living · last meaningful update **2026-07-26** (2018UL 준비: samples_2018UL.json, --preflight, OPEN #5).
> **Links:** decisions [`DECISIONS.md`](DECISIONS.md) · history [`CHANGELOG.md`](CHANGELOG.md) · exit codes [`reference/ERROR_CODES.md`](reference/ERROR_CODES.md) · path policy [`reference/CONFIG_PATHS.md`](reference/CONFIG_PATHS.md).

## Bottom line

The analyzer + submitter are migrated to the `ttHH2017UL_fullNano_v20` ntuple campaign and use NtupleForge sample names throughout. As of 2026-06-30 the code has a **collision-free numbered exit-code system** (fail-fast, Condor-detectable) and an **explicit correction-path policy** (no silent defaults; `null` = disable; required ≠ null). **Immediate runnable step: `prescan` (MC and Data).** `main`/`btagtrig` need the per-sample prerequisites below before they will run.

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
2. **Verify provisional cross sections:** every `verify_xsec:true` entry in `xsec_db` (e.g. `ST_s_had`, `ST_tW_*`, `WW/WZ/ZZ`) against XSDB / GenXSecAnalyzer before unblinding.
3. **Compile/run validation** of the analyzer and auxiliary tools (BTagSF / TriggerStudy / plotter / outputMerger) in CMSSW_14_2_1 — they have been syntax/brace-checked here, not built. See [`DECISIONS.md`](DECISIONS.md) on the validation method available in this environment.
4. **Verify** `make_filelists.py` merges base + ext datasets (`os.walk`).
5. **2018UL 대응 (OPEN, 2026-07-26 등록 · 2026-07-27 전수 감사로 확장).**
   **실행 계획은 워크스페이스 `RUNBOOK_UL18_to_controlplots.md` §4** (Phase 3 = 이 항목).
   목표가 "UL18 control plot 최단 경로"로 정해졌고, 아래 P0 를 고치지 않으면 **조용히 틀린
   결과**가 나온다. 감사 전문(**17항목** = P0 7 + P0′ 5 + P1 5, file:line)은 runbook §4 표.

   **P0 — 이것 없이 2018 을 돌리면 안 된다:**
   | # | 위치 | 조치 | 안 하면 |
   |---|---|---|---|
   | 1 | `ttHHanalyzer_unified.h:1133-1135` | `2018 → "2018_UL"` + unknown year FATAL | `yearForCorr=""` → E40 |
   | 2 | `include/eventBuffer.h` | 2018 DeepCSV HLT 3개 추가(4곳: 선언/choose/select/initBuffers) | 컴파일 불가 |
   | 3 | `ttHHanalyzer_unified.cc:296-360` | `(year,era)→path` map, 2018 은 JetHT 단독 OR (`:344` veto 제거) | 4J3T event **조용히 폐기** |
   | 4 | `ttHHanalyzer_unified.cc:1198,1227` | L1 prefiring 을 2016/2017 로 gate | 2018 branch 부재 → 기본값 0 → **전 MC weight 0 (무증상)** |
   | 5 | `src/CorrectionsManager.cc:248-250` | golden JSON 파일명 연도별 map (2018 실제 파일 = `Cert_314472-325175_13TeV_Legacy2018_Collisions18_JSON.txt`) | 2018 Data 전부 E41 |
   | 6 | `src/CorrectionsManager.cc:167-168` | `"Summer19UL18_Run"+dataEra_+"_V5_DATA_..."` | Run2018B/C 에 RunD JEC (try/catch 가 MC JEC 로 조용히 fallback) |
   | 7 | `ttHHanalyzer_unified.h:243-245` | DeepJet WP 2018UL ≈ 0.0490/0.2783/0.7100 로 연도 키화 | **신호영역 정의가 틀어짐** |

   **P0' — 배관(코드 아님, 필수):** `--year` allow-list 부재(`src/tnm.cc:273`) · **output 디렉토리에
   연도가 없어 2018 산출물이 2017 과 섞인다**(`submit_job_FH_Tier3_unified.py:158-162`) ·
   `prescan_summary.json` 연도 미분리 · **`make_filelists.py` Data 분기가 2017 하드코딩이라 2018
   Data filelist 가 생성되지 않는다**(`:250,261,263`) · `Tier3_2018_FH_unified_*.yml` 부재.

   **P1 (첫 plot 직후):** HEM15/16 veto **코드 자체가 없음** · trigger SF 기준 `IsoMu27`→`IsoMu24`
   및 `SelectionCuts.h:54` leadMuonPt 29→26 · 6th-jet/HT plateau 재확인 · JEC-unc loader 미호출
   (`CorrectionsManager.cc:65-80` vs `:381-404`, null deref) · lumi 정본(OPEN #6).

   **완료(2026-07-26~27, submitter):** Data era 정규식 `Run\d{4}([A-Z])$` 일반화,
   `--filelist-dir`, `--preflight`. **prescan(2018) 은 `loop()`/`createObjects()` 를 타지 않아
   P0 #1 만 고치면 바로 실행 가능**하다.

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
   - HEM15/16 veto (2018 전용) 적용 + veto map 검증. L1 prefiring 은 2016/2017 전용 → 2018 비활성.
   - 2018 lumi **확정 = 59.56 /fb** (0.84%), `samples_2018UL.json._meta` 반영 완료 (OPEN #6 참조).
   - Data PD 는 non-GT36 선택됨 — ttHH AN 사용 샘플 기준으로 재확인 필요(GT36 대안은 config 주석).
6. **lumi — 값은 확정됐고(2026-07-27) 코드 반영이 일부 남았다 (OPEN).**
   **정본 = LUM POG "Recorded Golden Legacy": 2017 = 42.07 fb⁻¹ (0.82%), 2018 = 59.56 fb⁻¹ (0.84%).**
   출처: [LumiRecommendationsRun2](https://twiki.cern.ch/twiki/bin/view/CMS/LumiRecommendationsRun2)
   (index: [TWikiLUM](https://twiki.cern.ch/twiki/bin/viewauth/CMS/TWikiLUM)), 인용 CMS-PAS-LUM-20-001.
   - **2018: 59.83 → 59.56 정정 완료.** 59.83 은 LUM POG 페이지에 **존재하지 않는 값**이었다.
     `samples_2018UL.json._meta` + 생성기 `build_ul18_from_log.py` 둘 다 반영(재생성 byte-identical 확인).
   - **2017: 42.07 이 옳다는 것이 확정됐다** — 우리 brilcalc 실측 42.0688 과 일치.
     따라서 `AnalyzerConfig/Tier3_2017_FH_unified_{main,btagtrig}.yml` 의
     `common.lumi_fb_inv = 41.48`(**weight 에 실제 사용**, `submit_job_FH_Tier3_unified.py:614`)은
     **확정적으로 틀렸다**. 42.07/41.48 = **1.42%** 만큼 전 MC weight 가 어긋난다.
   - ⏳ **남은 일 (물리 영향, 사용자 승인 대기)**: yml 3개를 42.07 로 고치면
     **main·btagtrig 재생산 + b-tag reweight JSON·stitch factor 재유도**가 필요하다.
     그래서 아직 바꾸지 않았다.
   - 표기 하드코딩 3곳(`stack_plotter.C:639` 41.48, `plot_btag_pyroot.py:106` 41.5,
     beamer `config.tex:31` 41.48)도 미반영 — 그래서 **같은 PDF 안에서 본문 41.48 vs backup 42.07**
     이 여전히 인쇄된다.
   - **전수 목록·절차·재생산 판단 = [`reference/LUMI_SOURCES.md`](reference/LUMI_SOURCES.md)**
     (9개 지점, 반영 여부를 ✅/❌ 로 표시).
   - `--preflight` 가 yml↔`_meta` 불일치를 WARN 으로 보고한다.

## Known constraints (invariants — see DECISIONS.md)

- No physics-logic change without an Analysis-Note justification.
- `main`/`btagtrig` require each sample in both `xsec_db` and `prescan_summary` (else E20/E21) → prescan first.
- ttbar stitching must be applied identically in the analyzer and the b-tag-SF Pass-1 normalization (mismatch breaks yield closure).
- Process-group keys (`tt+LF/tt+cc/tt+B/tt+nb/ttH/ttHH/ttZH4b/ttZZ4b`) and stitch category labels are load-bearing and are **not** renamed by sample-name changes.
