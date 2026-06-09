# ttbar stitching — change log & handoff (2026-06)

Scope: per-channel ttbb migration of the ttHH FH Run2 2017 stitching pipeline,
plus the validation status and the deferred work. Companion to
`ttbarCategorization.md` (§10) — read that for the full method; this file is the
"what changed and what's left" summary for the next session/agent.

Files touched this session:
`compute_stitch_factors.py`, `ttbarCategorization.md`.
Not touched (confirmed independent): `consolidate_prescan.py`, output paths.

---

## 1. What changed — per-channel ttbb stitch

### 1.1 The problem that was fixed
The previous `EXP` plan stitched only `ttbb_Hadronic`, leaving the
semi-leptonic and di-leptonic tt+2b as the 5FS inclusive (parton-shower)
modeling. This was a conservative leftover from when SL/DL 4FS ttbb samples
were assumed unavailable. They exist (`ttbb_SemiLeptonic`, `ttbb_2L2Nu`), so
the hadronic-only setup was under-using the dedicated 4FS modeling in two of
three decay channels.

Decay channels are orthogonal phase spaces, so each dedicated sample is
stitched at the granularity of its own decay coverage:
- `ttbb_c` (per channel) → owns tt+2b in channel `c`, anchored to the matching
  inclusive `TTTo_c` with `σ_inc = σ_total × BR_c`, rejecting tt+2b from
  `TTTo_c` only.
- `tt4b` (decay-inclusive) → unchanged: owns tt+nb in all channels, anchored to
  the Had+SL+DL inclusive merge with `σ_total` (no BR).

Net effect: tt+2b is now rejected from **all three** inclusive samples and each
filled by its matching dedicated, instead of from the hadronic channel only.

### 1.2 `compute_stitch_factors.py` edits
- `EXP` plan rewritten to 3-channel ttbb + decay-inclusive tt4b.
- Added branching fractions `BR_SL = 2·W_had·W_lep = 0.43937838` and
  `BR_DL = W_lep² = 0.10621081` (with `W_had = 0.6741`, so
  `BR_HAD = W_had² = 0.45441081`; the three sum to exactly 1).
- Added per-channel dedicated cross sections `SIGMA_TTBB_SL`, `SIGMA_TTBB_DL`
  (BR-scaled defaults from the hadronic 1.452 pb: σ_ttbb_total ≈ 3.195 pb →
  SL ≈ 1.404 pb, DL ≈ 0.339 pb). **σ_dedicated cancels** against the YAML base
  weight, so the absolute value does not bias the result — only YAML↔config
  agreement matters. Substitute official XSDB σ if the YAML uses those.
- Preserved the old behaviour as fallback option `EXP_TTBB_HAD_ONLY`
  (hadronic-only ttbb; SL/DL tt+2b stay as 5FS inclusive). `EXP_HAD_TT4B`
  (hadronic-only tt4b) also retained.
- Updated header docstring, partition-option comment, the analyzer-table note,
  and the JSON `_comment` to describe per-channel anchoring.
- Core logic (`compute_plan`, `build_analyzer_table`) NOT changed — it already
  iterates over an arbitrary number of dedicated samples and reject targets, so
  only the plan dict was extended.
- Output path unchanged: `DerivedCorr/stitchFactors/stitch_factors_2017.json`.

### 1.3 `ttbarCategorization.md` edits (§10)
- §10.3: coverage table extended with `ttbb_SemiLeptonic` (σ_inc = 365.46 pb)
  and `ttbb_2L2Nu` (σ_inc = 88.34 pb) rows; f/r formula made per-channel
  (`f_B,c`, `r_B,c`); BR decomposition and a migration note added.
- §10.4: anchor-JSON example now rejects tt+2b from all three inclusive samples
  and lists ttbb_SL/DL dedicated entries; YAML σ-consistency requirement made
  per-channel.
- §10.5: status table updated (per-channel ttbb = done; full-2017 validation =
  done); new §10.6 "Future work" added (see §3 below).

---

## 2. Validation status (as of 2026-06)

Confirmed correct on the full 2017 set; no blocking issues for central values.

- **`consolidate_prescan.py`** — 39 samples, all jobs `ok/found` equal, every
  hard cross-check passed (`No anomalies`). The expanded §5 partition is exact:
  `tt+nb -> 1b = 0` for every sample (NanoAOD ttCat 2b+ == expanded id 2b+tt+nb).
- **Signed ΣgenW confirmed** — `ΣgenW(Events) ≈ ΣgenW(Runs)` for all MC (skim%
  ≈ 0; e.g. TTToHadronic 73,140,766,669 vs 73,140,765,879, rel ~1e-8). Since the
  Runs `genEventSumw` is signed by definition, this proves the prescan
  accumulates `genWeight` with sign — negative weights are correctly folded into
  the normalization (no |w| bug). This closes the earlier open question.
- **`compute_stitch_factors.py` (EXP)** factors are physically sensible:
  - ttbb r differs per channel: Had 1.090 / SL 0.997 / DL 0.912 (driven by
    f = 0.00419 / 0.00383 / 0.00350). This is the quantitative justification for
    per-channel stitching — reusing the hadronic r for SL/DL would have been
    ~9–20% off per channel.
  - tt4b r = 0.276: dedicated native σ (0.296 pb) is ~3.6× the inclusive-anchor
    prediction (σ_inc·f_4b ≈ 0.082 pb), so anchoring correctly scales it down to
    the NNLO expectation.

---

## 3. Future work (deferred — full detail in `ttbarCategorization.md` §10.6)

Recorded so the next session can pick up without rediscovery. None blocks the
current central-value factors.

1. **Statistical uncertainty on f / r (effective-N).** Central values use signed
   ΣgenW and are correct, but no stat error is propagated. With negative weights
   `N_eff = (Σw)² / Σw²`. Needs:
   - analyzer prescan writer (C++): add per-bin `sumGenW2_id_{i}`
     (`genWeight²` per bin) — only the Runs-level `genEventSumw2` total exists today.
   - `compute_stitch_factors.py`: read per-bin Σw², compute N_eff per
     (sample, category), propagate to f and r, emit `r_err` in the JSON result.
   - Priority because it bites hardest where statistics are thin: `f_4b ≈ 9.8e-5`
     comes from only ~66k inclusive tt+nb events (TTTo2L2Nu ~8.6k), and
     `ttbb_2L2Nu` tt+2b has the fewest of the three ttbb channels (~312k). So
     `r_4b` and `r_DL` carry the largest relative uncertainty.

2. **Analyzer (C++) loads and applies `stitch_factors_2017.json` per event.**
   JSON is ready (`analyzer_perEvent_factor`). Load via the existing
   `CorrectionsManager` convention from `DerivedCorr/stitchFactors/`; map
   `expandedTtbarId % 100` → category via `sub_to_category`; apply
   `final_weight = yaml_base_weight * multiplier[sample][category] * genWeight
   * (PU*btagSF*trigSF)`. Prerequisite: dedicated YAML base weights must use the
   same per-channel σ as the JSON config (σ_dedicated cancels; only agreement matters).

3. **Switch b-tag normalization reweight key from `genTtbarId` to
   `expandedTtbarId`** so tt+nb is distinguished in the reweight (high b-jet
   multiplicity is exactly where tt+nb lives).

---

## 4. How to re-run / change inputs

- **Change a cross section:** edit the σ constants in `compute_stitch_factors.py`
  and re-run it. Do NOT hand-edit the JSON (r is derived: `r = σ_inc·f/σ_ded`,
  so the result block would go stale). No need to re-run the analyzer prescan
  (ΣgenW is σ-independent) or `consolidate_prescan.py` (σ-agnostic).
  - If `σ_inc` changes, also update the inclusive samples' YAML weight (it is the
    anchor). If a `σ_dedicated` changes, also update that sample's YAML σ to match.
- **`consolidate_prescan.py`** is a QA gate only: reads prescan ROOT rows, writes
  `prescan_summary.json/.csv` to `--outdir` (default `./prescan_summary/`). It
  loads no JSON and is unrelated to `DerivedCorr/`.
- **Switch stitch strategy:** set `PARTITION_OPTION` to `EXP` (default,
  3-channel ttbb), `EXP_TTBB_HAD_ONLY` (hadronic-only ttbb fallback), or
  `EXP_HAD_TT4B` (hadronic-only tt4b).

---

## 5. Analyzer application — stitch multiplier + expandedTtbarId-keyed b-tag reweight (2026-06)

Companion section to `ttbarCategorization.md` §11 (full method + analogies).
This is the "what changed and where" summary for the analyzer side.

### 5.1 Core logic of the change
The stitching JSON (`analyzer_perEvent_factor`, from §10) is now **consumed per
MC event** in both `main` and `btagtrig`:

```
final_weight = yaml_base_weight
             × PU × L1prefire × genWeight
             × stitch_multiplier[sample][ sub_to_category[expandedTtbarId%100] ]   ← NEW
             × btagShapeSF × triggerSF × btagNormReweight
```

- inclusive `TTTo_c`: multiplier 1 on LF/cc/tt+b, **0** on tt+2b/tt+nb (those
  are supplied by the dedicated samples).
- dedicated `ttbb_c`: `r_B,c` on tt+2b, 0 elsewhere. `tt4b`: `r_4b` on
  tt+bbb/tt+4b, 0 elsewhere.
- samples not in the plan: multiplier 1 (untouched).

The **b-tag normalization reweight** is re-keyed from NanoAOD `genTtbarId` to
the **expanded** id, so tt+nb (61/62/71/72) gets its own reweight bin separate
from tt+2b (53/54/55) — per the ttHH/ttH prescription that the b-tag *shape* SF
is renormalized per process × HF category. The stitch multiplier is applied
before `selectObjects()`, so the reweight derivation in `btagtrig` sees the
stitched composition.

Failure modes abort the job (non-zero exit) so Condor flags them instead of
producing silently-wrong histograms (exit codes 40–46; see §11.5).

### 5.2 Files / parts modified
New files:
- `src/StitchFactors.{h,cc}` — loads the JSON for the current sample
  (`nlohmann/json`); `factor(expandedId, w)` returns the per-(sample,category)
  multiplier and books per-category Σw before/after; `category()`,
  `printConfigSummary()`, `printRunSummary()`; fatal-exit on a missing/garbled
  file or a malformed `analyzer_perEvent_factor` block. Mirrors `ExpandedTtbarId`.

`ttHHanalyzer_unified.h` (3 edits):
- `#include "StitchFactors.h"` (+ `#include <map>`).
- members `StitchFactors _stitch;` and `std::map<int,std::string> _btagKeyByExpSub;`
  (b-tag key diagnostic).
- setter `setStitchFactorsFile(path)`.

`ttHHanalyzer_unified.cc` (5 edits):
- `main()` — load the JSON for non-`prescan` modes (env `STITCH_FACTORS_JSON`,
  default `DerivedCorr/stitchFactors/stitch_factors_2017.json`). `prescan` skipped
  (it produces the JSON's inputs).
- `process()` — after `expandedTtbarId` is resolved, multiply `_evtWeight` by
  `_stitch.factor(_expandedTtbarId, _evtWeight)` when the sample is in the plan.
- `selectObjects()` — b-tag reweight key switched
  `MakeProcessKey(_sampleName, _ev->genTtbarId)` → `(_sampleName, _expandedTtbarId)`;
  added empty-key fatal (45), non-finite-reweight fatal (46), and recording of
  the `expandedSub → processKey` map.
- `loop()` startup — `_stitch.printConfigSummary()` + fatal (43) if a stitch-plan
  sample has an INACTIVE Expanded_genTtbarId lookup.
- `loop()` end — `_stitch.printRunSummary()` + dump of the `sub → processKey` map
  with a `tt+nb split: YES/NO` diagnostic.

Unified diff: `stitch_apply.diff`. Core stitch logic (`compute_plan` /
`build_analyzer_table`) and the JSON are unchanged.

### 5.3 External dependency to confirm (not in this code)
`Config_TtCatGroup.hh::MakeProcessKey` must map `61/62` and `71/72` to process
keys **distinct** from `53/54/55` for the per-tt+nb b-tag reweight to be real.
The analyzer logs whether this holds (`tt+nb split: YES/NO`); if `NO`, extend
`MakeProcessKey` and re-derive the reweight in `btagtrig`. No source for this
header was available, so the integration keys on the expanded id and verifies at
runtime rather than hard-coding a mapping.

### 5.4 Prerequisite before the first stitched stack (from §10.7.3)
Dedicated ttbb YAML base weights must match the config σ (σ_dedicated cancels —
only YAML↔config agreement matters): `ttbb_Hadronic` 5.296e-4,
`ttbb_SemiLeptonic` 3.787e-4, `ttbb_2L2Nu` 8.837e-4. Inclusive and `tt4b` are
already consistent.
