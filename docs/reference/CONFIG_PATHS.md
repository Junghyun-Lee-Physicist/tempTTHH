# Reference — Correction-Input Path Policy

> **Purpose:** define exactly how the six `common.path_*` entries in the analyzer yml are interpreted, so a correction is never silently skipped or silently defaulted.
> **Audience:** anyone editing `AnalyzerConfig/Tier3_2017_FH_unified_*.yml` or running the submitter.
> **Status:** DECIDED · last meaningful update **2026-06-30**.
> **Links:** enforcement (C++) `include/ConfigPath.h` · enforcement (submit-time) `submit_job_FH_Tier3_unified.py` · exit codes `ERROR_CODES.md`.

## Bottom line (the contract)

There is **no code-default path.** A blank value is an error. To run *without* an optional correction you must write `null` explicitly. A required correction may not be `null`.

| yml value (`common.path_*`) | env passed to analyzer | meaning |
|---|---|---|
| `"/real/path"` | `/real/path` | use it; if the file/dir can't be loaded → subsystem FATAL (see `ERROR_CODES.md`) |
| `null` | `__NULL__` (sentinel) | **optional** correction → disabled (runs without it); **required** correction → **FATAL E13** |
| `""` or key missing | (rejected at submit time) | **FATAL E12** — blank is no longer a "use default" signal |

The submitter **always** exports every path env — either a real path or the `__NULL__` sentinel — so the analyzer never has to guess. `cfgpath::resolve()` returns an empty string **only** for the optional-and-disabled case, which each loader treats as "skip this correction".

## Which paths are required

Requirement depends on job type and SF toggles; the submitter computes it before launch:

| Path key | env | Required when |
|---|---|---|
| `path_jsonpog` | `TTHH_JSONPOG_PATH` | **always** (JME + PU are needed for every MC and Data job) |
| `path_goldenjson` | `TTHH_GOLDENJSON_PATH` | the job is **Data** (lumi mask) |
| `path_trigsf_dir` | `TTHH_TRIGSF_DIR` | mode ∈ {`main`,`debug`} **and** `--trigsf on` |
| `path_btag_reweight_json` | `TTHH_BTAGRW_JSON` | mode ∈ {`main`,`debug`} **and** `--btagrw on` |
| `path_stitch_json` | `STITCH_FACTORS_JSON` | never required (optional; `null` = no stitching) |
| `path_expanded_ttbarid_dir` | `EXPANDED_TTBARID_DIR` | never required (optional; `null` = tt+nb lookup inactive) |

## Current values (DECIDED 2026-06-30) and why

In the three `Tier3_2017_FH_unified_*.yml`:

- `path_jsonpog`, `path_goldenjson` — set to explicit paths (formerly the in-code defaults; now stated in config so the config is the single source of truth).
- `path_trigsf_dir`, `path_btag_reweight_json`, `path_stitch_json`, `path_expanded_ttbarid_dir` — set to `null` (**disabled**) because their inputs have **not yet been regenerated** for the `ttHH2017UL_fullNano_v20` dataset.

**Consequence for `main`:** because `--trigsf` defaults to `on`, running `main` with `path_trigsf_dir: null` triggers **E13** at submit time. Until trigger SF is re-derived, run `main` with `--trigsf off` (and `--btagrw off`), or supply real paths.

**OPEN (2026-06-30):** `path_goldenjson` is an absolute path under `/u/user/jhlee/…`; verify it matches the environment you run in. Same caution for any absolute path you add.

## Examples

```yaml
# Disable trigger SF on purpose (e.g. before it is derived):
path_trigsf_dir: null

# Provide a real stitch-factors file:
path_stitch_json: "DerivedCorr/stitchFactors/stitch_factors_2017.json"

# WRONG — empty is a FATAL E12, not a default:
path_jsonpog: ""
```
