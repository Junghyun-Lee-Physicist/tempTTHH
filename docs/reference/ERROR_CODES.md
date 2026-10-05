# Reference — Analyzer Exit Codes

> **Purpose:** the single, authoritative list of every numbered exit code the analyzer (and submitter) can terminate with, so a failed Condor job is diagnosable from its log alone.
> **Audience:** anyone debugging a failed job; anyone adding a new fail-fast check.
> **Status:** DECIDED · last meaningful update **2026-10-06** (codes of the tools around the analyzer, last section; the submitter's `--report`/`--status` read this table) · 2026-06-30 (the banded table).
> **Links:** code source of truth `include/ExitCodes.h` · path policy `CONFIG_PATHS.md` · workflow `../README.md`.

## Bottom line

Every essential-logic failure calls `std::exit(<code>)` (C++) or `_fatal(<code>, …)` (Python submitter) and prints a line beginning `[FATAL][E<code>]`. To find why a job died:

```bash
grep -nE '\[FATAL\]\[E[0-9]+\]' <condor_job>.log     # message + code
# or, from the Condor scheduler, inspect the job ExitCode directly.
```

`include/ExitCodes.h` (`namespace tthh`, `enum ExitCode`) is the **source of truth** for the numbers; this document is the human-readable mirror. **The two must be kept in sync.** Numbers are **stable** — a meaning is never reassigned; new failures take the next free number in the right band.

## Code bands

| Band | Subsystem |
|---|---|
| `0` | success |
| `10–19` | configuration / CLI / path wiring |
| `20–29` | normalization inputs (xsec_db, prescan) — also emitted by the submitter |
| `30–39` | input data (ntuple / `Events` tree) |
| `40–49` | central (POG) corrections — JME / PU / b-tag SF / golden JSON |
| `50–59` | derived corrections — trigger SF, b-tag normalization reweight |
| `60–69` | ttbar stitching |
| `70–79` | `Expanded_genTtbarId` (tt+nb) lookup |
| `80–89` | per-event physics integrity |

## Full table

| Code | Name (`tthh::…`) | Meaning | Emitted by |
|---|---|---|---|
| 0  | `OK` | success | — |
| 10 | `CONFIG_BAD_MODE` | unknown / removed `analysis_mode` | `ttHHanalyzer_unified.cc` (`main`) |
| 11 | `CONFIG_BAD_RUNINFO` | `runYear` / `DataOrMC` / `sampleName` not set | `ttHHanalyzer_unified.cc` |
| 12 | `CONFIG_PATH_ENV_MISSING` | a required correction-path env is unset/empty (no code default) | `include/ConfigPath.h`; submitter |
| 13 | `CONFIG_PATH_NULL_REQUIRED` | a **required** correction path was set to `null` | `include/ConfigPath.h`; submitter |
| 20 | `XSEC_DB_MISSING` | sample absent from `xsec_db` | submitter (`_compute_base_weight`) |
| 21 | `PRESCAN_MISSING` | sample absent from / invalid in `prescan_summary` | submitter (`_compute_base_weight`) |
| 30 | `INPUT_OPEN_FAIL` | cannot read input ntuple / `Events` tree; since 2026-10-06 also: any file of the job unreadable, the files' `Events` entries ≠ the chain's, an MC prescan file without its `Runs` tree / `genEventSumw` | `ttHHanalyzer_unified.cc` |
| 40 | `CENTRAL_CORR_LOAD_FAIL` | JME/PU/b-tag-SF correctionlib load failed | `src/CorrectionsManager.cc` |
| 41 | `GOLDENJSON_DATA_MISSING` | Data lumi-mask (golden JSON) missing | `src/CorrectionsManager.cc` |
| 50 | `TRIGSF_LOAD_FAIL` | trigger SF JSON missing/corrupt while required | `src/CorrectionsManager.cc` |
| 51 | `BTAGRW_LOAD_FAIL` | b-tag norm reweight JSON missing/corrupt while required | `src/CorrectionsManager.cc` |
| 60 | `STITCH_JSON_OPEN_FAIL` | stitch-factors JSON cannot be opened | `src/StitchFactors.cc` |
| 61 | `STITCH_JSON_PARSE_FAIL` | stitch-factors JSON parse error | `src/StitchFactors.cc` |
| 62 | `STITCH_JSON_SCHEMA_FAIL` | stitch-factors JSON schema/contents invalid | `src/StitchFactors.cc` |
| 63 | `STITCH_EXPTTID_INACTIVE` | sample is in the stitch plan but the tt+nb lookup is inactive | `ttHHanalyzer_unified.cc` |
| 70 | `EXPTTID_CHAIN_FAIL` | cannot add `ttnb_<sample>.root` to the lookup chain | `src/ExpandedTtbarId.cc` |
| 71 | `EXPTTID_TREE_EMPTY` | lookup tree has 0 entries | `src/ExpandedTtbarId.cc` |
| 72 | `EXPTTID_DUP_KEY` | conflicting duplicate `(run,lumi,event)` key | `src/ExpandedTtbarId.cc` |
| 73 | `EXPTTID_SAMPLE_MISMATCH` | lookup does not correspond to this sample | `src/ExpandedTtbarId.cc` |
| 80 | `PROCESSKEY_EMPTY` | `MakeProcessKey()` returned an empty key | `ttHHanalyzer_unified.cc` |
| 81 | `REWEIGHT_NONFINITE` | non-finite b-tag normalization reweight | `ttHHanalyzer_unified.cc` |

## History note (DECIDED 2026-06-30)

Before 2026-06-30 the analyzer used an ad-hoc set of codes (40–49) with two **collisions** (`41` and `43` each meant two different things) and a generic `EXIT_FAILURE` (1) for several distinct failures. This table replaced that with a collision-free, subsystem-banded scheme. Old→new remap, for anyone reading historical logs: central-corr `49`→`40`, golden `49`→`41`, trigsf `47`→`50`, btagrw `48`/`46`→`51`, stitch open `40`→`60`, stitch parse `41`→`61`, stitch schema `42`/`43`→`62`, analyzer stitch-inactive `43`→`63`, expttid `41/42/43/44`→`70/71/72/73`, process-key `45`→`80`, non-finite `46`→`81`.

## Adding a new code (contributor rule)

1. Pick the next free number in the correct band in `include/ExitCodes.h`.
2. Add the row to the table above with the same name, meaning, and emitter.
3. Emit it as `std::exit(tthh::<NAME>)` with a message starting `\n[FATAL][E<code>] …`.
4. Never reuse or renumber an existing code.

## Codes of the tools around the analyzer (2026-10-06)

Not in `include/ExitCodes.h`; listed so a failed job or step is readable from its return value. The submitter's
`--report` counts a missing job as `fail` from the condor log and `--status` prints the name from the table above, or
from this list:

| Code | Where | Meaning |
|---|---|---|
| 1 | analyzer (treestream `fatal()`, `eventBuffer::read`) | treestream stopped: a read error (`GetEntry < 0`, `readbranch - I/O error`), a missing branch, the first input file unreadable, or `** eventBuffer::read - cannot load entry` (the chain could not load an entry) — the reason is the last lines of the job's `.err` / `.out` |
| 127 | shell / condor | the executable was not found (e.g. a job started while a rebuild had removed it) |
| 134 / 137 / 139 / 143 | shell / condor | SIGABRT / SIGKILL (often memory) / SIGSEGV / SIGTERM (stopped) |
| 2 / 3 / 4 / 5 / 6 / 7 | `outputMerger/run_one_hadd.sh` | bad arguments / no `hadd` / no input directory / no `<proc>_*.root` / empty output / **number of inputs is not the expected number of jobs** (`merge_outputs.py --config`) |
| 0 / 1 / 2 | `outputMerger/merge_outputs.py` | ok / a merge failed (local) or `--report` found a process not merged / bad arguments or environment |
| 0 / 1 / 2 | `plotter/make_plots.py` | ok / a check (`MISSING`, `FLAG` incl. the `EVENTS` count) or the plotter failed / bad arguments |
| 0 / 1 | `consolidate_prescan.py` | no anomaly / at least one (bad or missing job, failed check, warning) — the same samples as its anomaly report |
| 1 | `submit_job_FH_Tier3_unified.py --preflight` | at least one FAIL row |
| any | `tools/runlog/runlog.sh` (`EXIT :` line) | the command's own code; 143 / 130 / 129 when the run was stopped (TERM / INT / HUP) |

