# Status — ttHH(HH→bbbb) Fully-Hadronic Analysis

> **Purpose:** the single place that answers "where are we right now?" — current state, what is ready, what is pending, and what is OPEN.
> **Audience:** anyone starting a session.
> **Status:** living · last meaningful update **2026-07-10**.
> **Links:** decisions [`DECISIONS.md`](DECISIONS.md) · history [`CHANGELOG.md`](CHANGELOG.md) · exit codes [`reference/ERROR_CODES.md`](reference/ERROR_CODES.md) · path policy [`reference/CONFIG_PATHS.md`](reference/CONFIG_PATHS.md).

## Bottom line

The analyzer + submitter are migrated to the `ttHH2017UL_fullNano_v20` ntuple campaign and use NtupleForge sample names throughout. As of 2026-06-30 the code has a **collision-free numbered exit-code system** (fail-fast, Condor-detectable) and an **explicit correction-path policy** (no silent defaults; `null` = disable; required ≠ null). **Immediate runnable step: `prescan` (MC and Data).** `main`/`btagtrig` need the per-sample prerequisites below before they will run.

## What is ready (DECIDED)

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
| Expanded_genTtbarId (tt+nb) | `null` | **disabled** — `ttnb_<sample>.root` not regenerated |

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

## Known constraints (invariants — see DECISIONS.md)

- No physics-logic change without an Analysis-Note justification.
- `main`/`btagtrig` require each sample in both `xsec_db` and `prescan_summary` (else E20/E21) → prescan first.
- ttbar stitching must be applied identically in the analyzer and the b-tag-SF Pass-1 normalization (mismatch breaks yield closure).
- Process-group keys (`tt+LF/tt+cc/tt+B/tt+nb/ttH/ttHH/ttZH4b/ttZZ4b`) and stitch category labels are load-bearing and are **not** renamed by sample-name changes.
