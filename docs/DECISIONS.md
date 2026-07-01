# Decision Log — ttHH(HH→bbbb) Fully-Hadronic Analysis

> **Purpose:** record each significant decision — the choice, why, what else was considered, and whether it still holds — so no one silently reopens a settled question or treats a proposal as settled.
> **Audience:** anyone about to change behavior or unsure whether something is fixed.
> **Status:** living, append-and-supersede · last meaningful update **2026-06-30**.
> **Links:** index [`README.md`](README.md) · current state [`STATUS.md`](STATUS.md) · per-change detail [`changes/`](changes/).

## How to use this log

Each entry has an id `D-<date>-<letter>`, a **Status** (DECIDED / PROPOSED / OPEN / DEPRECATED), the decision, its rationale, and the alternatives weighed. To change a decision, add a new entry that supersedes it and flip the old one to **DEPRECATED** with a pointer — never edit a decision away. Older decisions live in the relevant [`changes/STEP_*.md`](changes/); this log holds the load-bearing ones.

---

## D-2026-06-30-A — NtupleForge campaign names are the project-wide sample key · **DECIDED**

**Decision.** Every component (analyzer, configs, plotter, b-tag-SF tools) keys samples off the NtupleForge campaign-config names (`campaign_ttHH2017UL_fullNano_v20`).
**Why.** The old short aliases did not match the names used in `xsec_db`, which was the root cause of `main`/`btagtrig` xsec-lookup FATALs. One naming scheme removes the mismatch.
**Alternatives considered.** Keep aliases and maintain a translation map — rejected as a permanent drift hazard (two names for one thing).
**Scope guard.** Process-**group** keys (`tt+LF/tt+cc/tt+B/tt+nb/ttH/ttHH/ttZH4b/ttZZ4b`) and stitch category labels (`LF/cc/ttb/tt2b/ttbbb/tt4b`) are **not** sample names and were **not** renamed; they are owned by `Config_TtCatGroup.hh` / `compute_stitch_factors.py`.
**Detail.** `changes/STEP_18_ntupleforge_naming_and_kfactor_cleanup.md`.

## D-2026-06-30-B — k-factors are notes, not a separate numeric field · **DECIDED**

**Decision.** In `xsec_db`, do not carry a separate `kfactor_an_ref` number; set `kfactor=1.0` and record the reference in each entry's `note`.
**Why.** Avoids a second, easily-desynced normalization knob; the weight formula already multiplies a single `kfactor`.
**Alternatives.** Keep `kfactor_an_ref` as an applied factor — rejected (silent double-counting risk).
**Detail.** `changes/STEP_18_…md`.

## D-2026-06-30-C — Numbered, collision-free exit-code system · **DECIDED**

**Decision.** All essential-logic failures terminate via `tthh::ExitCode` (`include/ExitCodes.h`) with a `[FATAL][E<code>]` message; the submitter mirrors the 10–29 band.
**Why.** The previous ad-hoc codes had collisions (`41`, `43` each meant two things) and overused generic `EXIT_FAILURE` (1), so a Condor log could not identify the cause. Banded, stable numbers make every failure diagnosable from the log alone.
**Alternatives.** Keep ad-hoc codes / rely on stderr text only — rejected (not machine-detectable, ambiguous).
**Reference.** [`reference/ERROR_CODES.md`](reference/ERROR_CODES.md) (kept in sync with the header).

## D-2026-06-30-D — Explicit correction-path policy; no code default · **DECIDED**

**Decision.** A blank `common.path_*` is an error (E12); `null` disables an *optional* correction; a *required* correction may not be `null` (E13). The submitter always exports an explicit env (real path or `__NULL__` sentinel); there is no in-code default path.
**Why.** The old "blank → use a hard-coded default" hid mis-configuration and pinned environment-specific paths inside the code. Making config the single source of truth, and forcing an explicit `null` to disable, removes silent skips and silent defaults.
**Alternatives.** Keep blank-means-default (rejected: silent); treat missing key as disabled (rejected: ambiguous vs. a typo).
**Reference.** [`reference/CONFIG_PATHS.md`](reference/CONFIG_PATHS.md); enforced by `include/ConfigPath.h` and the submitter.

## D-2026-06-30-E — New-dataset derived corrections are disabled via `null` for now · **DECIDED (interim)**

**Decision.** `path_trigsf_dir`, `path_btag_reweight_json`, `path_stitch_json`, `path_expanded_ttbarid_dir` are set to `null` in the yml until their inputs are regenerated for `ttHH2017UL_fullNano_v20`.
**Why.** Their existing inputs are for the old dataset/process map; pointing at stale files would silently produce wrong results. `null` makes the disabled state explicit and truthful.
**Consequence.** `main` with the default `--trigsf on` hits E13 until trigger SF is re-derived → run `--trigsf off --btagrw off` in the interim.
**Supersede when.** Each input is regenerated → flip that path to a real value (and record in CHANGELOG). Tracked as OPEN in [`STATUS.md`](STATUS.md).

## D-(historical) — carried invariants · **DECIDED**

- **No physics-logic change without Analysis-Note justification.** Rationale: results must be defensible against the AN.
- **tt+nb separation needs CMSSW-level work.** NanoAODv9 exposes only `genTtbarId Int_t`; full parity needs a custom producer (miniAOD Stage-1 matcher + FlatTable). NanoAOD-tools reconstruction tops out ~97%. Detail in `ttbarCategorization.md`.
- **Inclusive ttbar underestimates the high-b-multiplicity tail** → dedicated NLO 4FS ttbb/tt4b with proper stitching (drop inclusive `genTtbarId%100 ∈ {51–55}`, rescale ttbb). Detail in `ttbarCategorization.md` and `stitch_logs_2017/`.
