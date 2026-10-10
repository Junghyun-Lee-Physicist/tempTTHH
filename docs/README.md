# Documentation Index — ttHH(HH→bbbb) Fully-Hadronic Analysis

> **Purpose:** the map. Tells any reader (human or AI, with no prior context) which document answers which question, and where to start.
> **Audience:** everyone — read this first.
> **Status:** living index · last meaningful update **2026-10-10 (2)** (ROADMAP_FH, PLAN_QCD_DD, 한국어 AN `AN_KR/`) · 2026-10-10 (ML·계통 계획 문서 추가) · 2026-10-04 (실행 기록 `runlogs/` 와 `tools/runlog/` 추가) · 2026-10-01 (v15 계획 문서 추가).
> **Links:** every document below.

## Bottom line — where to start

1. **"Where are we right now?"** → [`STATUS.md`](STATUS.md)
2. **"Why is it built this way / what was decided?"** → [`DECISIONS.md`](DECISIONS.md)
3. **"What changed and when?"** → [`CHANGELOG.md`](CHANGELOG.md) (chronological index; per-change detail in [`changes/`](changes/))
4. **"How do I run it?"** → §"Workflow" below + the per-mode yml in `AnalyzerConfig/`.

This repository is worked on **asynchronously by several people and AI threads that share no memory.** These docs are the only shared, persistent source of truth. They follow [`DOCUMENTATION_GUIDELINE.en.md`](DOCUMENTATION_GUIDELINE.en.md) (Korean mirror: [`DOCUMENTATION_GUIDELINE.ko.md`](DOCUMENTATION_GUIDELINE.ko.md)).

## The document set

| Document | Answers | Read it when |
|---|---|---|
| [`STATUS.md`](STATUS.md) | Current state, next steps, open questions | starting any session |
| [`DECISIONS.md`](DECISIONS.md) | Settled decisions + rationale + alternatives | unsure whether something is fixed |
| [`CHANGELOG.md`](CHANGELOG.md) | Chronological list of changes (→ detail in `changes/`) | tracing when/why something changed |
| [`changes/STEP_*.md`](changes/) | Full record of one change (the per-step detail) | you need the deep detail of a step |
| [`reference/ERROR_CODES.md`](reference/ERROR_CODES.md) | Every numbered analyzer/submitter exit code | a Condor job failed |
| [`reference/CONFIG_PATHS.md`](reference/CONFIG_PATHS.md) | How `common.path_*` is interpreted (null/empty/required) | editing yml or seeing E12/E13 |
| [`ttbarCategorization.md`](ttbarCategorization.md) / [`_KR`](ttbarCategorization_KR.md) | tt+B / tt+nb categorization physics | working on ttbar categorization |
| [`stitch_logs_2017/`](stitch_logs_2017/) | Stitch-factor derivation logs + JSON | working on ttbar stitching |
| [`PLAN_v15_2018UL_2024.md`](PLAN_v15_2018UL_2024.md) | v15 ntuple 로 가는 작업 목록과 연도별 한 명령 드라이버 설계(PROPOSED 2026-10-01); **§9: 2024 먼저의 단계별 계획(Stage 0~9, 통과 기준), 연도별 event cleaning 표, eventBuffer 정책, ttbar ID 검증, QCD 비교 정의 (2026-10-02)** | 2024/2018/2017 v15 작업을 시작할 때 |
| [`ROADMAP_FH.md`](ROADMAP_FH.md) | FH 전체의 워크스트림(W1 control plot … W8 한국어 AN), 지금 상태·다음·막는 것, 순서와 의존성, Run 2 를 v15 로 읽기 위한 목록(PROPOSED 2026-10-10) | 무엇을 먼저 할지 정할 때, 큰 그림이 필요할 때 |
| [`PLAN_QCD_DD.md`](PLAN_QCD_DD.md) | QCD multijet data-driven 추정: ttH(bb) FH(AN-19-094 §8.1)의 방법(영역, TF_loose, 검증, 불확도, fit)과 ttHH FH 판, 선행 조건(lepton CR 의 Data/MC)(PROPOSED 2026-10-10) | QCD 추정이나 DNN 의 QCD class 를 손댈 때 |
| [`AN_KR/`](AN_KR/) | **한국어 분석 노트**(XeLaTeX + kotex; `build.sh` 로 PDF): AN-2022/122 구조를 따라 FH·Run 3 판으로, 이론 설명과 우리 구현의 현재 상태(D-2026-10-10-B (4)) | 분석 전체를 이해하거나 설명할 때 |
| [`PLAN_ML_SYST.md`](PLAN_ML_SYST.md) | ML 두 개(GATJA jet 배정, 다중 분류 DNN)와 계통 불확도: 받은 자료(AN-2022/122 v26, GATJA 발표·코드, SWAN 의 FH·DL DNN 코드·표본)의 정리, 지금 `Tree/Tree` 의 빈칸, 순서(PROPOSED 2026-10-10) | ML 입력·branch 를 손대거나 계통을 시작할 때 |
| [`../runlogs/`](../runlogs/) (`LEDGER.tsv`) | 실행 기록: KNU 의 빌드·시험·파일 점검이 어느 커밋에서 어떻게 끝났나(condor job 포함; [D-2026-10-04-A](DECISIONS.md)) | KNU 단계의 결과를 볼 때 |
| [`../tools/runlog/README.md`](../tools/runlog/README.md) | 기록을 남기며 돌리기: `runlog.sh`(지금 셸), `condor_run.sh`(KNU condor job), `status.sh` | 오래 걸리는 단계를 돌리기 전 |
| [`../README.md`](../README.md) | Repo-level build/run entry | first checkout |

## Workflow (the run order)

```
make_filelists.py
  → submit_job_FH_Tier3_unified.py --mode prescan      # collects Σgenw
  → consolidate_prescan.py                              # builds prescan_summary
  → compute_stitch_factors.py                           # builds stitch factors
  → submit_job_FH_Tier3_unified.py --mode {main,btagtrig}
```

**Hard prerequisite:** `main` and `btagtrig` require every sample to be present in **both** `xsec_db` (`data/samples_2017UL.json`) and `prescan_summary`; otherwise the submitter aborts (E20 / E21). Run `prescan` first for any new sample. See [`STATUS.md`](STATUS.md) for what is and isn't ready right now.

## Conventions for editing these docs

- Follow [`DOCUMENTATION_GUIDELINE.en.md`](DOCUMENTATION_GUIDELINE.en.md): bottom line first; label status **DECIDED / PROPOSED / OPEN / DEPRECATED** with a date; one fact in one place (link, don't copy); stable headings.
- New change → add a line to [`CHANGELOG.md`](CHANGELOG.md) and, if substantial, a `changes/STEP_N_*.md`.
- New decision → add to [`DECISIONS.md`](DECISIONS.md). Never silently reopen a DECIDED item.
- End of session → update [`STATUS.md`](STATUS.md).
