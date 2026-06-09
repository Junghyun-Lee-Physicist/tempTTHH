# Stitching output logs — 2017 UL, option EXP (2026-06)

Captured run logs from the ttHH FH ttbar stitching pipeline. These are the
console outputs that produced `stitch_factors_2017.json`. Verbatim, for the
record / reproducibility.

## Files

| file | produced by | what it is |
|---|---|---|
| `consolidate_prescan.log` | `consolidate_prescan.py` | QA gate over the prescan output: per-sample job counts, ΣgenW(Events) vs ΣgenW(Runs), expanded §5 partition raw counts, and the ttCat-vs-expanded-id cross-check. All checks passed. |
| `compute_stitch_factors.log` | `compute_stitch_factors.py` | Physics step: config echo, sample/ΣgenW summary, genTtbarId%100 ΣgenW breakdown, per-dedicated f/r anchoring, reject map, and the per-event multiplier table. |

(The actual numeric product is `stitch_factors_2017.json`, written to
`DerivedCorr/stitchFactors/`.)

## Headline results (option EXP, 3-channel ttbb)

Per-dedicated stitch factors `r` (multiply each sample's YAML base weight):

| dedicated | anchor | σ_inc (pb) | f = owned/all | r |
|---|---|---|---|---|
| `ttbb_Hadronic` | TTToHadronic | 377.96 (×BR_had) | 0.004187 | **1.090005** |
| `ttbb_SemiLeptonic` | TTToSemiLeptonic | 365.46 (×BR_SL) | 0.003829 | **0.996736** |
| `ttbb_2L2Nu` | TTTo2L2Nu | 88.34 (×BR_DL) | 0.003504 | **0.912125** |
| `tt4b` | Had+SL+DL merge | 831.76 (no BR) | 0.000098 | **0.275917** |

Reject map: all three inclusive (`TTToHadronic`, `TTToSemiLeptonic`,
`TTTo2L2Nu`) reject `tt2b`, `ttbbb`, `tt4b` — each filled by the matching
dedicated.

## Validation highlights (from consolidate)

- 39 samples, all `Jobs ok/found` equal; `No anomalies: every job valid, every
  cross-check passed`.
- `ΣgenW(Events) ≈ ΣgenW(Runs)` for all MC (skim% ≈ 0) → confirms prescan
  accumulates **signed** genWeight; negative weights correctly folded in.
- Expanded §5 partition exact: `tt+nb -> 1b = 0` for every sample.
- `tt+nb/2b+` ratio physical: inclusive ~0.022–0.025, ttbb ~0.035–0.040,
  tt4b 0.351.

## Provenance / caveats

- Source: `/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/AnalyzerOutput_prescan`
- Lumi: 41480.0 pb⁻¹ (2017 UL). σ_inc total: 831.76 pb (NNLO+NNLL).
- σ_ttbb per channel (BR-scaled): Had 1.452 / SL ~1.404 / DL ~0.339 pb;
  σ_tt4b 0.296 pb. σ_dedicated cancels in r — only YAML↔config agreement matters.
- Deferred: statistical uncertainty on f/r (per-bin Σw², N_eff) is NOT in these
  logs — see `ttbarCategorization.md` §10.6 / `STITCH_CHANGELOG_2026-06.md` §3.
