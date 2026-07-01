# Documentation Index — ttHH(HH→bbbb) Fully-Hadronic Analysis

> **Purpose:** the map. Tells any reader (human or AI, with no prior context) which document answers which question, and where to start.
> **Audience:** everyone — read this first.
> **Status:** living index · last meaningful update **2026-06-30**.
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
