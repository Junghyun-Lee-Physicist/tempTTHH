# ttbar Categorization for the ttHH(HH→4b) Fully-Hadronic Channel

**Personal Analysis Note — JH**
**Run2 2017 NanoAODv9 UltraLegacy, CMSSW_14_2_1**
*Last updated: 2026-04-12*

---

## Abstract

This document specifies the per-event ttbar categorization scheme used by
the ttHH(HH→4b) fully-hadronic analysis. It covers the physics motivation,
the AN-cited algorithm (CMS `GenTtbarCategorizer` plugin via the NanoAOD
`genTtbarId` integer), the independent re-implementation used as a
cross-check, the rationale for collapsing the bbb / 4b categories at the
event level, the technical implementation in NtupleForge and in the
analyzer, and the validation pipeline that proves byte-level parity
between the two codebases on 1.28 M MC events.

---

## 1. Physics motivation

### 1.1 Why categorize ttbar at all

The ttHH(HH→4b) fully-hadronic final state has six b-jets and four light
jets at parton level: two from each top decay (2 × b + W → qq) and four
from the two Higgs decays (H → bb each). After hadronization, parton
shower and detector reconstruction the typical signal event ends up
with 7–10 reconstructed jets and ≥4 b-tagged jets. The dominant
irreducible background is QCD ttbar production with additional heavy-
flavour radiation, in particular **ttbar + b-jets** processes (commonly
written tt+bb, tt+b, tt+2b in CMS notation).

The challenge is that ttbar Monte Carlo is not produced as a single
sample with all possible heavy-flavour multiplicities turned on. Instead
the production splits into:

- **Inclusive ttbar samples** (e.g. `TTToHadronic`, `TTToSemiLeptonic`,
  `TTTo2L2Nu`) generated in the 5-flavour scheme (5FS) where the b-quark
  is treated as massless and additional b-jets come from the parton
  shower (PS).
- **Dedicated ttbb samples** (e.g. `TTbb_4f_FxFx`) generated in the
  4-flavour scheme (4FS) where the b-quark mass is kept and the matrix
  element (ME) explicitly produces extra b-quarks.

Both samples populate the same physical region of phase space (events
with additional b-jets from g→bb splittings). To avoid double-counting
when combining them into a single physics analysis, one has to **stitch**:
take only the heavy-flavour categories from the dedicated 4FS sample and
take only the light-flavour categories from the inclusive 5FS sample,
joined at a per-event boundary in heavy-flavour content.

The categorization machinery described in this note is the tool that
defines that boundary.

### 1.2 What "additional" means

Throughout this note, "additional" refers to b- or c-quarks (and the
hadrons / jets they form) that **do NOT come from a top-quark decay**.
A typical fully-hadronic ttbar event has two b-quarks from the
t → bW vertices; those are *not* "additional". An event with a third
b-quark produced by gluon splitting in the parton shower has one
"additional" b-quark, etc.

The categorisation is performed at gen-jet level, on jets clustered
from generator-level final-state particles in a fixed kinematic
acceptance:

```
gen-jet pT  >  20 GeV
|gen-jet η| <  2.4
```

These cuts match the CMS POG `GenHFHadronMatcher` defaults [Ref. 2] and
are the values used throughout this analysis.

### 1.3 Where in the analysis the labels are used

The per-event ttbar category drives three downstream decisions:

1. **5FS / 4FS sample stitching** at the boundary tt+LF / tt+HF
   (light-flavour vs heavy-flavour). This is the primary use.
2. **Per-category systematics** — for example, separate b-tag SF
   normalisations for tt+bb vs tt+LF, since the b-tagging working
   point is calibrated against different jet flavour fractions in each.
3. **DNN background nodes** — the multi-class background discriminator
   has a tt+nb output node that consumes events from any
   tt + (additional b-jet) category. The bbb / 4b distinction does not
   need to be made at the per-event level because the DNN merges them
   into the same node (see §3.3).

---

## 2. AN reference and the official algorithm

### 2.1 Authority chain

The categorization logic followed by this analysis is anchored to:

1. **ttHH AN-2022/122** [Ref. 4], §3.1 (object & event categorization)
   and §3.4 (sample stitching). These sections cite the CMS POG
   GenHFHadronMatcher → GenTtbarCategorizer plugin chain as the
   official categoriser, and define how the bbb / 4b sub-classes are
   constructed at *sample* level (Option1 / Option2 prescriptions),
   not per event.
2. **ttH AN-19-094** [Ref. 5], §6.1.2 — earlier ttH analysis using the
   same plugin chain, used here as cross-reference for hadronic-channel
   topics that the ttHH AN does not cover.
3. **CMS GenTtbarCategorizer plugin source code** [Ref. 1] — the
   ground-truth implementation of the integer encoding of `genTtbarId`,
   read from the CMSSW repository at lines 282–300 of
   `TopQuarkAnalysis/TopTools/plugins/GenTtbarCategorizer.cc`.
4. **CMS GenHFHadronMatcher TWiki** [Ref. 2] — narrative documentation
   of the ghost-clustering procedure that feeds GenTtbarCategorizer.

When ttHH AN and ttH AN disagree, ttHH wins. When the AN and the source
code disagree, the source code wins. This document follows that order.

### 2.2 The `genTtbarId` integer

The CMS `GenTtbarCategorizer` plugin is an EDM producer that runs
upstream of NanoAOD and writes a single integer (`genTtbarId`) into
each event. The integer is a packed representation of the event's heavy-
flavour content. NanoAOD exposes it as `Events.genTtbarId` [Ref. 3].

The relevant decoding rule, taken directly from the plugin source
[Ref. 1, lines 282–300], is:

```
genTtbarId % 100   meaning
─────────────────  ────────────────────────────────────────────────────
        0          tt + LF      no additional heavy-flavour jet
       41 – 45     tt + cc      one or more additional c-jets, no add b-jet
       51          tt +  b      1 additional b-jet, 1 b-hadron in it
       52          tt + 2b      1 additional b-jet, ≥2 b-hadrons in it
                                (collinear g→bb splitting merged into one jet)
       53, 54, 55  tt + bb      ≥2 additional b-jets
       any other   not ttbar    sample is not ttbar (e.g. data, signal,
                                or pileup-only fake event)
```

The hundreds-digit and thousands-digit of `genTtbarId` carry separate
information about c-hadron multiplicity, c-jet count and overlap with
top-decay products; for the present analysis only the modulo-100 part
is used.

### 2.3 What the plugin can NOT tell us

A critical observation that drives the rest of this document:

> **`genTtbarId` does NOT encode the number of additional b-jets when
> there are two or more.** Codes 53, 54, 55 differ in the *b-hadron
> multiplicity inside the leading two b-jets* — not in the b-jet count
> itself.

This is verifiable directly in the plugin source [Ref. 1, lines 282–300]:
the codes 53/54/55 are produced by counting how many b-hadrons are
clustered into the *first* and *second* additional b-jets; the question
"how many additional b-jets are there in total?" never enters the
encoding once the answer is ≥2.

The consequence is that **the bbb / 4b distinction (3 vs ≥4 additional
b-jets) cannot be recovered from `genTtbarId` alone**. The ttHH AN
[Ref. 4] addresses this by defining bbb / 4b at *sample* level
(Option1 / Option2 prescriptions in §3.4), not per event.

This is the central architectural fact that justifies the five-category
schema described in §3.

---

## 3. The five-category schema (Option B′)

### 3.1 Categories

Five mutually exclusive event-level categories:

| #  | Name             | AN equivalent | Definition                                                                |
|----|------------------|---------------|---------------------------------------------------------------------------|
| 0  | `LightFlavour`   | tt + LF       | No additional b- or c-jet                                                 |
| 1  | `AddCjet`        | tt + cc       | ≥1 additional c-jet, no additional b-jet                                  |
| 2  | `Add1Bjet_1Had`  | tt + b        | Exactly 1 additional b-jet, containing exactly 1 b-hadron                 |
| 3  | `Add1Bjet_2Had`  | tt + 2b       | Exactly 1 additional b-jet, containing ≥2 b-hadrons (collinear g→bb)      |
| 4  | `Add2Bjet`       | tt + bb (∪ bbb ∪ 4b) | ≥2 additional b-jets (covers what AN calls bb, bbb, 4b at sample level) |

A non-ttbar event maps to a sentinel `NoTTJets`. An event with missing
GenPart / GenJet input (e.g. data NanoAOD) maps to a sentinel `Unknown`.

### 3.2 Naming convention rationale

The names use explicit `Bjet` and `Had` suffixes instead of the historical
`tt+b` / `tt+2b` notation. The reason is that "2b" in the AN is highly
ambiguous — it can be read as "2 b-jets" (wrong: 1 b-jet) or "2 b-hadrons"
(right). The `Add1Bjet_2Had` name forces the reader to see immediately
that there is **one** additional b-jet containing **two** b-hadrons,
removing the most common source of confusion when reading the code.

### 3.3 Why five categories and not seven

Three independent reasons all point to the same conclusion:

1. **The plugin cannot distinguish them.** As shown in §2.3, the
   GenTtbarCategorizer integer does not encode the additional b-jet
   count once the count is ≥2. Splitting the AN's tt+bb into bb / bbb / 4b
   would require a re-implementation that goes *beyond* the AN-cited
   tool — and any such re-implementation would not match the AN
   definition of those categories.

2. **The AN constructs bbb / 4b at sample level.** ttHH AN §3.4
   [Ref. 4] defines the bbb / 4b prescriptions (Option1 / Option2)
   based on which 4FS sample is being used and its generator-level
   filter, not from per-event GenHFHadronMatcher labelling. Per-event
   categorization at this resolution is therefore not what the AN asks
   for.

3. **The downstream DNN merges them anyway.** The ttHH multi-class
   background DNN has a single tt+nb output node that consumes
   tt+b, tt+2b, tt+bb, tt+bbb, tt+4b together. Distinguishing bbb from
   4b at the event level produces no analysis-level benefit.

The five-category schema is therefore both *necessary* (the integer
cannot give us more) and *sufficient* (the AN and the analysis design
do not need more).

---

## 4. Two algorithms, two branch sets

### 4.1 The two algorithms

**Algorithm A — POG path (`decode_genttbarid`)**

Read the NanoAOD `genTtbarId` integer and map `genTtbarId % 100` onto the
five categories using the table in §2.2. This is a stateless, ~10-line
function. It is what the AN [Ref. 4, §3.1] cites as the official method.
On data NanoAOD, where `genTtbarId` is absent, the function returns
`NoTTJets` (with source code 3 = `NO_GENTTBARID`).

**Algorithm B — GenPart path (`_categorize_genpart_xval`)**

Re-derive the same five-category label from raw GenPart and GenJet
information, without touching `genTtbarId`. The procedure:

1. Walk all `GenPart` entries and collect those that are b-hadrons
   (PDG-ID test on the digit pattern), are last copies (status flag bit
   13 set), and do **not** have a top quark in their mother chain
   within 30 generations of ancestry.
2. Walk all `GenJet` entries and collect those with `hadronFlavour == 5`,
   `pT > 20 GeV`, `|η| < 2.4`. These are the "additional gen b-jets" in
   acceptance.
3. For each additional b-hadron, find the closest gen b-jet in
   ΔR < 0.4. If a match is found, increment that jet's b-hadron count.
   At the end, `len(jet_bh_map)` is the number of additional b-jets
   (`nBJets`) and the values are the per-jet b-hadron counts.
4. Apply the decision tree:
   ```
   nBJets >= 2          → kAdd2Bjet
   nBJets == 1, 1 had   → kAdd1Bjet_1Had
   nBJets == 1, ≥2 had  → kAdd1Bjet_2Had
   nBJets == 0          → check for additional c-jets (same procedure
                          with PDG-ID 4 hadrons and hadronFlavour==4)
                          if any matched → kAddCjet
                          else            → kLightFlavour
   ```

This algorithm uses a different strategy than the POG ghost-clustering
(it does ΔR-matching to ordinary gen-jets after the fact, while POG
embeds the b-hadrons as ghosts in the jet clustering itself). The two
paths are therefore **genuinely independent estimators** of the same
quantity. Their agreement on heavy-flavour signal samples is high
(>97%) and on light-flavour-dominated inclusive ttbar samples is lower
(~73%). The disagreement comes from a known weakness of the GenPart
walker — see §6.

### 4.2 The two branch sets

NtupleForge writes both algorithms' results into the slimmed ntuple, in
two parallel branch namespaces:

```
Primary set (genTtbarId path)         Cross-check set (GenPart path)
─────────────────────────────────     ──────────────────────────────────
ttCat_LightFlavour     : Bool         ttCatXval_LightFlavour    : Bool
ttCat_AddCjet          : Bool         ttCatXval_AddCjet         : Bool
ttCat_Add1Bjet_1Had    : Bool         ttCatXval_Add1Bjet_1Had   : Bool
ttCat_Add1Bjet_2Had    : Bool         ttCatXval_Add1Bjet_2Had   : Bool
ttCat_Add2Bjet         : Bool         ttCatXval_Add2Bjet        : Bool
ttCatSource            : Int          ttCatXvalSource           : Int
```

Total: 12 branches per event. On any ttbar event exactly one of the
five Bool branches is set in each set; on a non-ttbar event all five
are False and the source integer indicates the reason.

The `ttCatSource` / `ttCatXvalSource` integers carry diagnostic info:

| Source | Meaning |
|--------|---------|
| 0      | `GENTTBARID` — branch set was filled from the genTtbarId path |
| 0      | `GENPART`    — branch set was filled from the GenPart path     |
| 2      | `NO_TTBAR`   — event has no ttbar pair, all five Bool are False |
| 3      | `NO_GENTTBARID` / `NO_GENINFO` — required input absent (data) |

The two source codes "0" mean different things in the two namespaces
because they refer to different algorithms; this is documented in the
NtupleForge README.

### 4.3 Which one downstream physics uses

**Always `ttCat_*` (the genTtbarId path).** Reasons:

- It is what the AN [Ref. 4, §3.1] cites.
- It is the output of the official CMS plugin chain
  GenHFHadronMatcher → GenTtbarCategorizer that is used by every
  ttbar-related CMS analysis.
- The ghost-clustering procedure POG uses internally [Ref. 2] is more
  robust than the GenPart walker against the parton-history pruning
  that NanoAOD applies.

The cross-check set `ttCatXval_*` is **never** read by any downstream
physics decision. Its sole purpose is to be compared with `ttCat_*` (and
with the analyzer's recomputation of both algorithms — see §5) so that
any drift between the two implementations is detected before it affects
physics. The architectural rule is:

> **Removing `ttCatXval_*` would not change a single physics result.**

If this rule is ever violated, the validation infrastructure has been
misused.

---

## 5. Four-way validation

### 5.1 The four estimators

To detect any logic drift between the C++ analyzer and the Python
ntuplizer as early as possible, the analyzer computes (or reads) **four
independent estimators of the same per-event label** on every MC event:

| Name          | Source                          | Algorithm                              |
|---------------|---------------------------------|----------------------------------------|
| `ANA_GENPART` | analyzer C++                    | own GenPart algorithm                  |
| `ANA_GENID`   | analyzer C++                    | decode of `genTtbarId%100`             |
| `NTU_PRIMARY` | ntuple `ttCat_*` branches       | ntuplizer's `decode_genttbarid` (Python) |
| `NTU_XVAL`    | ntuple `ttCatXval_*` branches   | ntuplizer's GenPart algorithm (Python) |

The C++ analyzer never reads any single branch and stops there; it
always computes both ANA_* values from scratch and reads both NTU_*
values from the ntuple, then compares all six pair-wise combinations.

### 5.2 The two must-agree pairs

Out of the six C(4,2) pairs, two are *byte-equivalent by construction*:

```
ANA_GENID   ≡  NTU_PRIMARY    (both decode the same integer)
ANA_GENPART ≡  NTU_XVAL       (same algorithm written in two languages)
```

Any disagreement on either of these pairs is a regression, not a
physics effect. There is no meaningful sense in which the C++ and the
Python implementations of "decode the same integer" can disagree at the
event level — if they do, it is a bug. Likewise, the two GenPart
implementations (C++ in the analyzer, Python in the ntuplizer) follow
the exact same recipe in §4.1 step-by-step, so byte-level agreement is
the success criterion, not the goal.

The other four pairs are *informational*: they reflect the real
algorithmic difference between POG ghost-clustering and our GenPart
walker. Their agreement depends on sample composition and is the
subject of §6.

### 5.3 Validation output

For each MC event the analyzer:

1. Computes `ANA_GENPART` and `ANA_GENID` directly from the event's
   GenPart / GenJet vectors and `genTtbarId` integer.
2. Reads `NTU_PRIMARY` from `ttCat_*` and `NTU_XVAL` from `ttCatXval_*`.
3. Fills four 1D count histograms (one per estimator) and six 2D
   confusion-matrix histograms (one per pair) into a `TtCatValidation/`
   directory inside the output ROOT file.
4. Fills a 60-bin × 5-bin TH2 of `genTtbarId%100` vs `ANA_GENPART` so
   any anomalous POG code can be spotted.
5. For the first 20 events, and for any later event where a must-agree
   pair disagrees or where the category is heavy-flavour, prints a
   per-event debug dump to stdout: the four labels side by side, the
   per-jet b-hadron map, and explicit `MUST-AGREE BROKEN` markers.

At the end of the job, the analyzer prints a summary: per-estimator
event counts, per-pair agreement percentages (with explicit listing of
every off-diagonal cell of the must-agree pairs), and the full
breakdown of the informational pairs.

### 5.4 Validation result on TTToHadronic

Run on the full 2017 NanoAODv9 UL TTToHadronic sample (1,280,000 events
processed):

```
[ttCatSummary] --- MUST-AGREE PAIRS (any disagreement = bug) ---
ANA_GENID   vs NTU_PRIMARY    agree=1,280,000   disagree=0   100.0000%
ANA_GENPART vs NTU_XVAL       agree=1,280,000   disagree=0   100.0000%

[ttCatSummary] --- INFORMATIONAL PAIRS (real algorithmic diff) ---
ANA_GENPART vs ANA_GENID      agree=  929,370   disagree=350,630   72.6070%
ANA_GENPART vs NTU_PRIMARY    agree=  929,370   disagree=350,630   72.6070%
ANA_GENID   vs NTU_XVAL       agree=  929,370   disagree=350,630   72.6070%
NTU_PRIMARY vs NTU_XVAL       agree=  929,370   disagree=350,630   72.6070%
```

Both must-agree pairs are perfect (zero off-diagonal cells out of 1.28 M
events). The four informational pairs all show *exactly the same*
counts: 929,370 / 350,630. This is itself a strong self-consistency
check — it can only be true if `ANA_GENID ≡ NTU_PRIMARY` and
`ANA_GENPART ≡ NTU_XVAL` hold simultaneously, since the four pairs
partition into two equivalence classes that must produce identical
confusion matrices when those equivalences are exact.

**Conclusion: ntuplizer ↔ analyzer parity is established at the byte
level on 1.28 M events.**

---

## 6. The POG vs GenPart difference

### 6.1 Observed pattern

The four informational pairs in §5.4 show ~73% agreement, much lower
than the ~97% observed previously on the TTHHTo4b signal sample. The
disagreement is concentrated in events where:

- `genTtbarId % 100 == 0` (POG says no additional heavy flavour)
- the GenPart algorithm finds 1–2 additional b-jets

That is, the GenPart algorithm sees b-jet activity that POG does not
record. In every observed mismatch the direction is the same: GenPart
finds *more* additional b-jets than POG, never fewer.

### 6.2 Why this happens

Two contributing factors:

1. **Pruned GenPart history.** NanoAOD stores `prunedGenParticles`,
   not the full Pythia history. The mother-chain walker
   `_has_top_ancestor` follows `GenPart_genPartIdxMother` up to 30
   generations looking for a top quark; if the chain has been pruned
   (typical case for parton-shower hadrons whose intermediate states
   are dropped), the walker fails to find the top and the b-hadron is
   labelled "additional" when it actually came from a top decay.

2. **POG uses ghost-clustering, not ΔR matching.** The
   GenHFHadronMatcher procedure [Ref. 2] embeds b-hadrons into the jet
   clustering itself as zero-momentum "ghost" particles, so the
   association of a b-hadron to a jet uses the full jet algorithm (with
   recombination and area effects), not a simple ΔR < 0.4 cone. This is
   especially relevant for the topology where one b-jet from the top
   decay overlaps an additional b-jet from g → bb splitting.

The combination of these two effects makes POG correctly identify
top-decay b-jets that the GenPart walker mis-labels as additional, in
events where the parton history is short (typical of inclusive ttbar
samples like TTToHadronic).

### 6.3 Why this is not a problem

Three reasons:

1. **Downstream physics uses the POG path.** The ~27% disagreement
   never reaches a histogram cut, a weight, or a stitching boundary.
2. **The AN cites POG.** ttHH AN [Ref. 4, §3.1] explicitly cites the
   GenHFHadronMatcher / GenTtbarCategorizer chain as the official tool.
   We follow what the AN says.
3. **The disagreement is a known property of NanoAOD GenPart, not a
   bug in our code.** The GenPart re-implementation and the POG plugin
   answer the same question with two different inputs (pruned GenPart
   vs full ghost-clustering); they are not expected to give the same
   answer in every case.

If a future version of the analysis ever switches its primary path
*away* from POG (which is not currently planned), the GenPart walker
would need to be improved. Possible improvements include:

- Increasing the mother-chain walk depth from 30 to 50.
- Cross-checking `isHardProcess` and `fromHardProcess` status flags
  before declaring a b-hadron "additional".
- Following first-copy ancestors instead of last-copy.
- Switching to ghost-clustering on `GenJetAK4` ghost-augmented
  collections, which would require running our own jet clustering
  off-line.

None of these are needed for the present analysis.

---

## 7. Technical implementation

### 7.1 NtupleForge (Python)

**File:** `modules/ttbarCategorizer.py` (~970 lines). Operational
documentation in [Ref. 6].

Key entry points and helpers:

| Symbol | Role |
|--------|------|
| `TtbarCategorizer` (class) | NanoAOD-tools `Module` subclass that runs once per event |
| `decode_genttbarid(int)` | Algorithm A — five-category mapping from `genTtbarId%100` |
| `_categorize_genpart_xval(event, has_genpart, has_genjet)` | Algorithm B — GenPart-based re-derivation, returns one of the five category names or `None` |
| `_is_b_hadron(pdgId)` / `_is_c_hadron(pdgId)` | PDG-ID tests on the meson / baryon digit pattern |
| `_has_top_ancestor(event, idx, nGP)` | 30-generation mother-chain walk, looks for `abs(pdgId)==6` |
| `_delta_r2(η1, φ1, η2, φ2)` | ΔR² with φ wrap-around |
| `make_default_module()` + `MODULES` | Factory used by `run_postproc.py` driver |

The module's `analyze(event)` method:

1. Eagerly checks branch availability in `beginFile` (replaces the
   earlier lazy `hasattr` approach which raised `RuntimeError` on data
   NanoAOD).
2. Runs `decode_genttbarid()` and writes the five `ttCat_*` Bool branches
   plus the `ttCatSource` integer.
3. Runs `_categorize_genpart_xval()` and writes the five `ttCatXval_*`
   Bool branches plus the `ttCatXvalSource` integer.
4. Optionally appends a row to `ttcat_debug.csv` if `--ttcat-debug-csv`
   is set (off by default).

The two algorithms run independently and unconditionally on every
ttbar event, in every mode. There is no fast path that skips one or
the other.

**Constants** (single source of truth, `modules/ttbarCategorizer.py`):

```python
GEN_JET_PT_MIN  = 20.0    # GeV
GEN_JET_ETA_MAX = 2.4
DR_MATCH_MAX    = 0.4
```

**Compatibility helpers** in `modules/_nanoaod_compat.py`:

- `to_int(x)` — handles the NanoAOD `UChar_t` quirk where
  `event.GenJet_hadronFlavour[j]` returns a `bytes` object instead of
  an integer.
- `safe_len(arr, branch_name)` — handles raw `TTreeReaderArray` objects
  that do not implement `__len__`, with a `GetSize()` fallback.

### 7.2 Analyzer (C++)

**Files:** `ttHHanalyzer_unified.h`, `ttHHanalyzer_unified.cc`.
Operational documentation in [Ref. 7].

**Enum** (`ttHHanalyzer_unified.h`):

```cpp
enum class TtCat {
    kLightFlavour, kAddCjet, kAdd1Bjet1Had, kAdd1Bjet2Had,
    kAdd2Bjet, kNoTTJets, kUnknown, kNCategories
};
```

**Categorization API** (`ttHHanalyzer_unified.h`):

| Method | Algorithm | Reads |
|--------|-----------|-------|
| `computeTtCategoryFromGenPart()` | own GenPart walker | `GenPart_*`, `GenJet_*` |
| `computeTtCategoryFromGenTtbarId()` | decode of `genTtbarId%100` | `genTtbarId` |
| `readNtuplePrimaryCategory()` | none — pure read | `ttCat_*`, `ttCatSource` |
| `readNtupleXvalCategory()` | none — pure read | `ttCatXval_*`, `ttCatXvalSource` |
| `officialTtCategory()` | wraps `readNtuplePrimaryCategory()` | as above |

The GenPart walker is byte-equivalent to the Python implementation in
`ttbarCategorizer.py`: same 30-generation mother walk, same ΔR² < 0.16
matching, same `isLastCopy` bit test (bit 13 of `statusFlags`), same
GEN_JET_PT_MIN / GEN_JET_ETA_MAX constants.

**Validation block** in `ttHHanalyzer_unified.cc`, inside `process()`,
guarded by `if (_DataOrMC != "Data")`:

1. First-event branch availability dump (`[TTCAT_BRANCH_CHECK]`).
2. Compute four estimators.
3. Per-event debug print (first 20 events, plus heavy-flavour or
   must-agree-broken events up to 100 events).
4. Fill 4 + 6 + 1 = 11 histograms in `TtCatValidation/`.

**Histogram declarations** in the private section of the header
(lines around 1650–1690):

```cpp
TH1F* _hTtCat_Counts_AnaGenPart  = nullptr;
TH1F* _hTtCat_Counts_AnaGenId    = nullptr;
TH1F* _hTtCat_Counts_NtuPrimary  = nullptr;
TH1F* _hTtCat_Counts_NtuXval     = nullptr;

TH2F* _hTtCat_AnaGenPart_vs_AnaGenId    = nullptr;
TH2F* _hTtCat_AnaGenPart_vs_NtuPrimary  = nullptr;
TH2F* _hTtCat_AnaGenPart_vs_NtuXval     = nullptr;  // MUST-AGREE
TH2F* _hTtCat_AnaGenId_vs_NtuPrimary    = nullptr;  // MUST-AGREE
TH2F* _hTtCat_AnaGenId_vs_NtuXval       = nullptr;
TH2F* _hTtCat_NtuPrimary_vs_NtuXval     = nullptr;

TH2F* _hTtCat_GenTtbarIdMod100          = nullptr;
```

**End-of-job summary** in `endjob()`: writes the 11 histograms and
prints per-pair agreement statistics with explicit off-diagonal
listing for the must-agree pairs.

### 7.3 Plotting

**File:** `scripts/plot_ttcat_validation.py` (PyROOT, no extra deps).

Reads `TtCatValidation/` from the analyzer output ROOT file and renders
all 11 histograms to PNG. Auto-marks must-agree pairs in their plot
title (`MUST-AGREE OK` / `MUST-AGREE BROKEN`). Produces individual
plots plus a one-page 4 × 3 summary grid.

CLI:
```bash
python scripts/plot_ttcat_validation.py output.root [-o plots/]
       [--normalize {none,row,col}] [--log-counts]
```

### 7.4 Treestream / eventBuffer

The `eventBuffer.h` header used by the analyzer is auto-generated by
treestream from a `variables.txt` file. The 12 ttCat-related branches
must be listed in that file:

```
Bool_t  ttCat_LightFlavour
Bool_t  ttCat_AddCjet
Bool_t  ttCat_Add1Bjet_1Had
Bool_t  ttCat_Add1Bjet_2Had
Bool_t  ttCat_Add2Bjet
Int_t   ttCatSource
Bool_t  ttCatXval_LightFlavour
Bool_t  ttCatXval_AddCjet
Bool_t  ttCatXval_Add1Bjet_1Had
Bool_t  ttCatXval_Add1Bjet_2Had
Bool_t  ttCatXval_Add2Bjet
Int_t   ttCatXvalSource
```

`eventBuffer.h` itself is never edited by hand; the analyzer accesses
the branches via `_ev->ttCat_LightFlavour` etc. as treestream emits
them.

---

## 8. Operational notes

### 8.1 Running the validation

On every new ntuple production, run the analyzer on a representative
ttbar sample (typically TTToHadronic for the inclusive case and
TTHHTo4b for the signal case) and check:

1. **stdout** — both must-agree pairs report `100.0000%` and
   `disagree=0` in the end-of-job summary.
2. **`TtCatValidation/` ROOT histograms** — both must-agree pair plots
   are diagonal-only.
3. **`scripts/plot_ttcat_validation.py`** — `pair_AnaGenId_vs_NtuPrimary.png`
   and `pair_AnaGenPart_vs_NtuXval.png` show `MUST-AGREE OK` in the
   title.

If any of these three checks fails, do not use the production sample
until the parity is restored.

### 8.2 Known TODOs

- The branch name `ttCatXval_*` (`Xval` = "cross-validation") is opaque
  to anyone who has not read this document. It will be renamed to
  something more descriptive (e.g. `ttCatGenPart_*`) in the next major
  refactor, in coordination with treestream and the analyzer.
- A `validateTtCat` runtime flag will be added to the analyzer
  (default `false`). With the flag off, production runs will skip the
  four-way comparison and call only `officialTtCategory()`, recovering
  the per-event GenPart loop cost. With the flag on (validating a new
  ntuple production, or after touching the categorizer), the full
  comparison runs.
- The legacy `TTCatDebug.h` header (a stand-alone CSV-diff helper from
  the previous validation era) is no longer used by the current
  analyzer and should be moved to a `legacy/` sub-directory or removed
  on the next cleanup pass.

### 8.3 What NOT to do

- **Do not re-implement categorization in the analyzer for production
  use.** The analyzer's `computeTtCategoryFromGenPart()` and
  `computeTtCategoryFromGenTtbarId()` exist for validation only.
  Production decisions go through `officialTtCategory()`, period.
- **Do not modify `eventBuffer.h` by hand.** Add or remove branches in
  `variables.txt` and re-run treestream.
- **Do not silently change the GEN_JET_PT_MIN, GEN_JET_ETA_MAX or
  DR_MATCH_MAX constants.** They appear in two places (Python and C++)
  and any change must update both. If the constants change, the
  ntuple must be re-produced because the `ttCat*_` branches are
  derived from them.

---

## References

1. **CMS `GenTtbarCategorizer` plugin source code.**
   `TopQuarkAnalysis/TopTools/plugins/GenTtbarCategorizer.cc`, CMSSW
   official repository.
   <https://github.com/cms-sw/cmssw/blob/master/TopQuarkAnalysis/TopTools/plugins/GenTtbarCategorizer.cc>
   The `genTtbarId` integer encoding rule used by `decode_genttbarid()`
   is implemented at lines ~282–300 of this file.
   *Cited in: §2.1, §2.2, §2.3.*

2. **CMS Public TWiki — GenHFHadronMatcher.**
   "Generator-level identification of heavy-flavour jets in top-quark
   events", CMS Public Wiki.
   <https://twiki.cern.ch/twiki/bin/view/CMSPublic/GenHFHadronMatcher>
   Narrative documentation of the ghost-clustering procedure that
   feeds `GenTtbarCategorizer`. Source of the gen-jet acceptance
   defaults (pT > 20 GeV, |η| < 2.4) used throughout this analysis.
   *Cited in: §1.2, §2.1, §4.3, §6.2.*

3. **CMS Internal TWiki — NanoAOD ttbar generator-level info.**
   "Top quark Monte Carlo generator information in NanoAOD",
   `CMS.TopModGen`, CMS Internal Wiki.
   <https://twiki.cern.ch/twiki/bin/view/CMS/TopModGen>
   Documents the `genTtbarId` branch as exposed in NanoAODv9
   UltraLegacy and the upstream EDM producer that fills it.
   *Cited in: §2.2 (NanoAOD branch source).*

4. **ttHH analysis note (primary).**
   CMS AN-2022/122, *"Search for ttHH production in the four-bottom
   final state"*, version v26.
   Primary AN cited by this analysis. The relevant sections for the
   present document are §3.1 (object & event categorization), §3.2
   (selection definitions feeding the categorization), and §3.4
   (5FS / 4FS sample stitching, Option1 / Option2 prescriptions for
   bbb / 4b sample-level construction).
   Local files: `ttHH_AN_AN2022_122_v26-1-151.pdf`,
   `ttHH_AN_AN2022_122_v26-152-253.pdf`.
   *Cited in: §2.1, §2.3, §3.3, §4.3, §6.3.*

5. **ttH analysis note (reference).**
   CMS AN-2019/094, *"Search for ttH production with H → bb"*,
   version v20.
   Earlier ttH analysis using the same plugin chain. Used in this
   document as cross-reference for hadronic-channel topics that the
   ttHH AN does not cover. The relevant section is §6.1.2 (ttbar+jets
   categorization for the ttH(H→bb) analysis).
   Local files: `AN2019_094_v20_ttHAnalysis5494.pdf`,
   `AN2019_094_v20_ttHAnalysis95145.pdf`,
   `AN2019_094_v20_ttHAnalysis-146-301.pdf`,
   `AN2019_094_v20_ttHAnalysis302333.pdf`,
   `AN2019_094_v20_ttHAnalysis344398.pdf`.
   *Cited in: §2.1.*

6. **NtupleForge README.** `README_ntuplizer.md`, in the NtupleForge
   repository, sections "tt+jets Event Categorizer", "References",
   "Future TODO". Canonical operational documentation for the
   ntuplizer-side categorization module and the 12 ttCat-related
   branches it writes.
   *Cited in: §7.1 (operational details), §8.2 (TODO list cross-link).*

7. **ttHH Analyzer README.** `README_analyzer.md`, in the ttHH Analyzer
   repository, section "tt+jets Event Categorization & 4-way Validation".
   Canonical operational documentation for the analyzer-side validation
   block, the `TtCatValidation/` ROOT histograms, and the
   `scripts/plot_ttcat_validation.py` plotter.
   *Cited in: §5.3 (validation output), §7.2–§7.3 (implementation),
   §8.1 (validation procedure).*

---

*End of document.*
