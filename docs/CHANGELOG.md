# Changelog — ttHH(HH→bbbb) Fully-Hadronic Analysis

> **Purpose:** one chronological line per change, newest first, linking to the full record. The detail lives in [`changes/STEP_*.md`](changes/); this file is the index, not a copy.
> **Audience:** anyone tracing when and why something changed.
> **Status:** append-only · last meaningful update **2026-06-30**.
> **Links:** decisions [`DECISIONS.md`](DECISIONS.md) · state [`STATUS.md`](STATUS.md).

> Append-only: add new entries at the top; do not rewrite history. "Detail" links point to the full per-step record.

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
