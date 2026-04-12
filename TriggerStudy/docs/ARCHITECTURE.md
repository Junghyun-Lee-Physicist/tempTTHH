# Architecture & Methodology

Deep technical reference for the ttHH hadronic trigger SF pipeline. This document is the
authoritative source for *why* the code is structured the way it is — methodology, design
decisions, and non-obvious invariants. For build/run instructions see [`../README.md`](../README.md).

---

## 1. High-Level Pipeline

```
┌─────────────────────────────────────────────────────────────────┐
│                        Config.hh (central)                      │
│  - SampleRegistry: {sample_name → (isData, lumi-weight)}        │
│  - Binning: HT, 6th-jet pT, η, NB-jets categories               │
│  - Flags & statistical thresholds                               │
└──────────────────────────┬──────────────────────────────────────┘
                           │ included by all components
        ┌──────────────────┼──────────────────┐
        │                  │                  │
        ▼                  ▼                  ▼
  EventLooper        DeriveSF.cpp      PlotTriggerEfficiency.cpp
  (compiled)         (ROOT macro)      (ROOT macro)
        │
   ┌────┴─────┐
   │ Mode 0   │  output_<sample>.root  ─┐
   │ (Step 1) │   (2D HT×pT maps per    │
   └──────────┘    NB×η category)       │
        ▲                               │
        │                               ▼
        │                          DeriveSF.cpp
        │                          ┌────────────────────────────┐
        │                          │ - Clopper-Pearson (Data)   │
        │                          │ - N_eff binomial (MC)      │
        │                          │ - Fill empty bins (fit or  │
        │                          │   nearest extrapolation)   │
        │                          │ - Build correctionlib JSON │
        │                          └────────┬───────────────────┘
        │                                   │
        │                                   ▼
        │                          TriggerSF.root (validation)
        │                          trigger_sf.json.gz (correctionlib v2)
        │                                   │
   ┌────┴─────┐                             │
   │ Mode 1   │ ◄─── correctionlib loads ───┘
   │ (Step 2) │
   └──────────┘  validated_<sample>.root
                  (1D _noSF + _SF histograms)
                                   │
                                   ▼
                  PlotTriggerEfficiency.cpp
                  → Validation_TrigEff_<var>.pdf
                  (Data vs MC-no-SF vs MC-with-SF + ratio)
```

---

## 2. Trigger SF Measurement Strategy

### 2-A. Orthogonal Reference Trigger

The hadronic trigger efficiency is measured in a **single-muon orthogonal control region**:

```
selection: nMuons == 1 && nElecs == 0 && HLT_IsoMu27
```

`HLT_IsoMu27` is statistically independent of the hadronic trigger paths under study, so
selecting on it gives an unbiased denominator for measuring the hadronic OR efficiency.
This is the standard CMS approach for hadronic trigger SF derivation in lepton+jets
or all-hadronic ttbar analyses.

### 2-B. Hadronic Trigger OR (Era-Aware)

```
passHadTrig = (4J3T) || (6J1T) || (6J2T) || PFHT1050
```

The `4J3T`/`6J1T`/`6J2T` paths come in two era variants (`_B` and `_CDEF`) reflecting
the 2017 trigger menu change between Run B and Run CD. `PFHT1050` is era-independent.

Era is parsed from the data sample name (`SingleMuon_B` → `era="B"`). MC has no era
information and falls into the `_CDEF` branch by default (since `isEraB = (era == "B")`
evaluates false for `era == "default"`). **This is a known asymmetry**: MC trigger bits
are evaluated against the CDEF menu only. If a future analysis needs era-mixed MC,
this branching must be revisited.

### 2-C. Cross-Check Invariant

The OR is recomputed in `EventLooper::Loop()` and compared against `reader->GetPassHadTrig()`
(an OR pre-computed in the ntuple). Mismatch → `exit(1)`. This catches both ntuple-side
bugs and any drift between the ntuple's trigger logic and the analysis-side definition.

### 2-D. Categorization Dimensions

The SF is derived as a function of (HT, 6th-jet pT) in a 2D map, separately for each
**(NB-category × η-category)** key. With current defaults (`useNBjets=true`, `useEta=false`),
this yields 3 × 1 = 3 SF maps:

- `nB0to2_Eta_Inc`
- `nB3_Eta_Inc`
- `nB4p_Eta_Inc`

The 6th-jet pT binning is motivated by the 6-jet trigger thresholds; HT binning is motivated
by the PFHT1050 turn-on. NB categorization is critical because b-tagged trigger paths
(`6J2T`, `4J3T`) have very different turn-ons depending on the available b-jet count.

---

## 3. Statistical Treatment

### 3-A. Efficiency Errors

Two regimes, handled differently in `DeriveSF.cpp::computeEfficiency()`:

**Data (unweighted):** Clopper-Pearson interval at 1σ confidence (68.27%) via
`TEfficiency::ClopperPearson(n, k, level, false/true)`. The half-width
`0.5 * (hi - lo)` is used as a symmetric error. This is robust at low statistics and
near the 0/1 boundaries where Gaussian errors break down.

**MC (weighted):** Binomial error using effective sample size:

```
N_eff = (Σw)² / Σ(w²)         ← from totalErr = √Σw²
σ(eff) = √( eff × (1 - eff) / N_eff )
```

The earlier (commented-out) approach of standard error propagation
`eff × √((σ_p/p)² + (σ_t/t)²)` was abandoned because it underestimates the error
when the same events appear in both numerator and denominator (correlated). The
N_eff binomial form correctly captures the binomial nature of the trigger decision
while accounting for event weighting.

This same logic is mirrored in `PlotTriggerEfficiency.cpp::ComputeEfficiency()` so
that validation error bars are consistent with derivation errors.

### 3-B. Low-Statistics Bin Handling

A bin is flagged as "low-stat" and excluded from the measured-SF set if:

```
nPass(Data)       <  kMinPassData    (default: 10)
N_eff(MC, pass)   <  kMinNeffMC      (default: 20)
```

Rationale documented in `Config.hh`: in 4-b-jet phase space, Data statistics are sparse
enough that 10 is the practical floor (~30% relative error tolerated). MC threshold is
kept conservative because MC entering the SF denominator with poor statistics will
destabilize all downstream bins.

Low-stat bins are filled by interpolation (next section).

### 3-C. Empty-Bin Filling

Two strategies, toggled by `Config::useFitInterpolation`:

**Option 1: 2D Quadratic Fit in Log-Space (`useFitInterpolation = true`)**

`DeriveSF.cpp::fitQuadratic2DLog()` fits

```
log(SF) = a₀ + a₁·x + a₂·y + a₃·x·y + a₄·x² + a₅·y²
```

where `x, y` are HT and pT normalized to `[-1, 1]` over the analysis range. The fit uses
`TGraph2DErrors` + `TF2` with 6 parameters. Log-space ensures the prediction stays
positive. The full 6×6 covariance matrix is preserved (`FitModel2D::cov`), enabling
proper uncertainty propagation via `evalLogUnc()`:

```
σ²(log SF) = bᵀ · Cov · b,    b = basis vector at (x, y)
```

The predicted SF and uncertainty for each empty bin are computed by exponentiating.
Falls back to neighbor interpolation if the fit fails or returns non-finite values.

**Option 2: Nearest Extrapolation (`useFitInterpolation = false`, current default)**

`fillByNearestExtrapolation()` walks one bin down (lower pT, same HT) first, then one bin
left (lower HT, same pT). Simpler and more conservative — preferred when fit quality
isn't trustworthy across all categories.

**Final fallback:** SF = 1.0 ± 0.5 (50% uncertainty), used only if neither approach yields
a valid value.

**Relative-error floor:**
```
relErrFloor = max(0.05, median(measured relative errors))
```
applied to all filled bins. Prevents fits in well-populated regions from producing
artificially small uncertainties in extrapolated regions.

---

## 4. correctionlib JSON Schema

Generated by `DeriveSF.cpp::JSONBuilder::BuildCorrectionSet()`. Schema version 2,
`multibinning` nodetype, with the lookup signature consumed by EventLooper:

```cpp
sf_provider->evaluate({
    static_cast<int>(currentNBJets),    // input 1: nbJets count
    static_cast<double>(Jet6Eta),       // input 2: 6th jet η
    static_cast<double>(currentHT),     // input 3: HT
    static_cast<double>(Jet6PT)         // input 4: 6th jet pT
});
```

The correction key is `"triggerSF"`. Internally, `nbJets` and `eta` select among the
NB×η category keys; `(HT, pT)` perform the multibinning lookup with `flow: clamp`
(out-of-range values pinned to the edge bin).

**Critical constraint:** the EventLooper `evaluate()` call must match the
`BuildCorrectionSet` schema exactly. If you change `useNBjets` or `useEta`, the schema
shape changes and the call signature in `EventLooper.cpp` must be updated in lockstep.
This is currently a manual coordination point — a future improvement would generate
both from a single source of truth.

JSON is gzipped via zlib (`WriteGzipJSON`) to keep the file small (~kB scale).

---

## 5. SF Application: The Numerator-Only Rule

This is the most easily-mistaken part of the entire pipeline. The relevant code in
`EventLooper::Loop()` (Step 2 fill block):

```cpp
// Total: NO SF applied
h_HT_Total_SF->Fill(currentHT, weight);

// Pass: SF applied
if (passHadTrig)
    h_HT_Pass_SF->Fill(currentHT, weight * sf);
```

**Why:** the SF is defined as a *correction to the efficiency*, not to the yield.
Mathematically:

```
eff_corrected = (Pass × SF) / Total
              = (Pass / Total) × SF
              = eff_MC × SF
              ≈ eff_Data    ← by construction of SF = eff_Data / eff_MC
```

If SF were applied to both numerator and denominator, it would cancel:

```
(Pass × SF) / (Total × SF) = Pass / Total = eff_MC   ← wrong, no correction
```

Equivalently, applying SF to Total alone would *anti-correct* the efficiency. The only
correct location is the numerator. The same rule holds for any "selection efficiency
SF" — never multiply both legs.

This is enforced by code structure (separate `_noSF` and `_SF` histograms) and by
inline comments. The validation plot from `PlotTriggerEfficiency.cpp` provides the
final sanity check: the `MC (with SF)` curve should overlap `Data` within ratio
panel error bars.

---

## 6. Sample Registry Pattern

`Config::SampleRegistry()` is a private static `std::map<string, SampleInfo>` accessed via
`Config::GetSampleInfo(name)`. Each entry stores:

```cpp
struct SampleInfo { bool isData; double weight; };
```

The `weight` for MC is pre-computed `(σ × L) / Σ(genEventSumw)` — the per-event scaling
needed to normalize an MC sample to the data luminosity. Hardcoded in the registry
because these values are stable across analysis iterations and should not be
re-derived in every run.

**Architectural significance:** this pattern means EventLooper takes only a sample
*name* on the command line. All knowledge about whether it's data or MC, what the weight
is, what era to assign — comes from a single source. New samples are added in exactly
one place. The previous version (commented out in `EventLooper.cpp` Phase 4) used a
hard-coded `if/else` chain inside the looper, which scaled poorly.

Registry is also used by Step 1 (no SF needed but `isData` controls the weight branch)
and is the single source of truth for the `run_analysis.sh` sample list (though
that list is currently maintained in the shell script independently — keeping these in
sync is a manual chore).

---

## 7. Per-Event Weight Composition

Computed in `EventLooper::Loop()`:

```cpp
weight = (isData)
    ? 1.0
    : reader->GetGenWeight()
    * reader->GetPUWeight()
    * reader->GetPrefireWeight()
    * MC_weight;       // ← from Config::SampleRegistry
```

Note that `reader->GetEvtWeight()` (a pre-computed total weight in the ntuple) is
**deliberately not used**. EventLooper assembles the weight from components for
explicit control. If the ntuple's `evtWeight` ever drifts from this product
(e.g., includes a b-tag SF or trigger SF that we want to apply separately),
this code stays correct. The trade-off is that any new component added to the
ntuple weight definition is invisible here until manually added.

---

## 8. Defensive Invariants (Fail-Fast Catalog)

The code is intentionally fail-fast — every invariant is checked and any violation
exits immediately with a unique code so the failure point is identifiable from logs.

| Check | Location | Exit code |
|---|---|---|
| `nJets >= 6` | event loop | 1 (`FATAL_INVARIANT`) |
| 6th jet `pT > 40 GeV` | event loop | 1 |
| `HT >= 500 GeV` | event loop | 1 |
| 6th jet `|η| <= 2.4` | event loop | 1 |
| Data: `passGoldenJson` | event loop | 1 |
| Trigger OR mismatch (recomputed vs ntuple) | event loop | 1 |
| Unknown sample name | Phase 4 | 1 |
| Input file open failure | `Init()` | 1 |
| TTree not found | `Init()` | 1 |
| SF JSON load failure | Phase 3 | 1 |
| Data with `weight != 1` or `sf != 1` | fill block | 55 |
| MC, `!passHadTrig`, `sf != 1` | fill block | 56 |
| `correctionlib::evaluate` exception | Step 2 SF lookup | 57 |

The first five checks duplicate cuts that should already be applied during ntuple
skimming. Their presence here is intentional **double-bookkeeping**: a regression in
the skimmer (or use of an unskimmed ntuple by mistake) is caught immediately rather
than silently producing biased results.

---

## 9. Debug Mode (`useDebugSF`)

A hardcoded flag in `EventLooper::Loop()`:

```cpp
bool useDebugSF = false;  // toggle to bypass correctionlib
```

When `true`, Step 2 loads SF values directly from `TriggerSF.root` histograms
(`Histograms/SF_measured_<key>`) using `TH2D::FindBin` lookup, bypassing
correctionlib entirely. Used to isolate bugs:

- If `useDebugSF=true` validation looks correct but `useDebugSF=false` does not
  → bug is in the JSON build (`JSONBuilder` or schema mismatch)
- If both are wrong → bug is upstream (binning, methodology, fill logic)

Includes its own statistics tracking (`nEvtSFApplied`, `nEvtSF1`, `sumSF`) and a
verbose per-event debug log for the first 30 events plus every 100k.

This is a *developer* feature, not a production mode. The flag lives in source
(not Config) intentionally — flipping it should require recompilation and
deliberate intent.

---

## 10. Build System Notes

`Makefile` highlights worth knowing:

- **OS auto-detection** (`uname -s`): Darwin → clang++, else g++.
- **Auto dependency tracking** via `-MMD -MP` → `.d` files. Header changes correctly
  trigger recompilation. No stale-build problems.
- **Out-of-source build** under `tmp/` preserving source directory layout
  (`src/foo.cc` → `tmp/src/foo.o`).
- **correctionlib integration**: `correction config --cflags/--ldflags` for compile/link.
  On macOS, an additional rpath is auto-derived via Python (`python3 -c "import
  correctionlib; ..."`) to handle the dyld lookup that brew correctionlib doesn't
  set up automatically.
- **Single executable target**: only `exe_TrigStudy` is built. `DeriveSF.cpp` and
  `PlotTriggerEfficiency.cpp` are ROOT macros, run via `root -l -b -q` (interpreted).
  ACLiC compilation (`+` suffix) is *not* used in `run_analysis.sh` — the macros are
  fast enough interpreted and avoid the build complexity.

`make info` dumps all internal variables for debugging.

---

## 11. Coordination Points (Manual Sync Required)

Several places in the codebase must be kept consistent by hand. Future refactor targets:

1. **Sample list**: defined in `Config::SampleRegistry()` *and* iterated in
   `run_analysis.sh`. Adding a sample requires editing both.
2. **correctionlib schema**: `JSONBuilder::BuildCorrectionSet()` in `DeriveSF.cpp`
   defines the input layout; `EventLooper::Loop()`'s `sf_provider->evaluate({...})`
   call must match it. Schema-changing flag flips (`useNBjets`, `useEta`) require
   inspection of both files.
3. **Histogram naming**: `EventLooper` creates `h_<var>_Total/Pass_noSF/SF`;
   `PlotTriggerEfficiency` reads them by exact string match. Renaming requires
   touching both.
4. **Trigger bit list**: `NtupleReader` exposes 8 trigger bits; `EventLooper`
   composes the OR. Adding a new HLT path means updating the ntuple producer,
   `NtupleReader` (.hh + .cc), and `EventLooper`.

---

## 12. Glossary

| Term | Meaning |
|---|---|
| **Orthogonal trigger** | A trigger statistically independent of the trigger under study, used to select an unbiased sample for efficiency measurement |
| **Reference trigger** | Synonym for orthogonal trigger in the SF context |
| **Turn-on** | The region of phase space where a trigger's efficiency rises from 0 to its plateau |
| **N_eff** | Effective sample size for weighted events: `(Σw)²/Σ(w²)`. Equals the unweighted N when all weights are equal |
| **Clopper-Pearson** | An exact binomial confidence interval, robust at low N and near 0/1 boundaries |
| **correctionlib** | The CMS standard library for serializing and evaluating correction factors (efficiency SFs, JECs, etc.) as versioned JSON |
| **multibinning** | A correctionlib node type that performs N-dimensional binned lookup |
| **Skim** | A pre-filtering pass over ntuples that applies loose analysis cuts to reduce file size |
| **Skim invariant** | A cut that *must* hold by virtue of how the skim was built; checked defensively in case of mis-skimmed input |
