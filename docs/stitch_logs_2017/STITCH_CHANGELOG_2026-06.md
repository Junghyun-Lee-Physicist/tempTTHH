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
