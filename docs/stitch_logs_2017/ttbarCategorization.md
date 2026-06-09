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
between the two codebases on 1.28 M MC events. Sections 9–10 extend this with an *expanded* per-event id that separates tt+bbb from tt+4b (which `genTtbarId` cannot — §2.3), its validation, and the prescan → stitching-factor → analyzer-weight chain built on top of it.

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

> **Update (§9).** This per-event limitation is now lifted by an *expanded*
> id derived from MiniAOD, which adds sub-codes `61`/`62` (tt+bbb) and
> `71`/`72` (tt+4b). The NanoAOD five-category scheme below is unchanged and
> remains the validation baseline; the expanded id is layered on top. See §9.

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
- **Wire the analyzer to load** `DerivedCorr/stitchFactors/stitch_factors_2017.json`
  and apply the per-(sample, category) per-event factor (§10.4). The
  factors are computed but not yet applied inside the analyzer.
- **Switch the b-tag normalization reweight key** from NanoAOD `genTtbarId`
  to `expandedTtbarId` so tt+nb is distinguished in the reweight (§10.5),
  after confirming the process-key map covers codes `61–72`.

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

---

## 9. Expanded ttbar-ID — recovering tt+bbb vs tt+4b per event

> This section extends the scheme of §1–8. Sections 1–8 describe the
> **NanoAOD `genTtbarId`** categorization, which (per §2.3) cannot tell
> 3 additional b-jets from ≥4. Here we add a per-event **expanded id**
> that recovers exactly that distinction, validate it, and wire it into
> the analyzer. Nothing in §1–8 changes — the official 5-category branches
> are untouched; the expanded id is an *additional* label.

### 9.1 The problem, restated in one line

`genTtbarId % 100` saturates at `53/54/55` once there are ≥2 additional
b-jets (§2.3). The fully-hadronic ttHH(4b) signal sits on top of the
**tt+≥3b** tail of ttbar, so we need to separate

- **tt+bbb** — exactly 3 additional b-jets, and
- **tt+4b** — 4 or more additional b-jets,

per event. NanoAOD does not carry this. MiniAODv2, however, still has the
full GenJet–b-hadron association available, so the information *exists* one
tier up; it is simply not propagated into NanoAOD.

**Analogy.** NanoAOD is a compressed photo: you can see "a crowd of at
least two people," but you cannot count exactly how many. MiniAOD is the
RAW file. We go back to the RAW once, write down the head-count for the
few events where it matters, and carry that note forward.

### 9.2 Encoding: four new sub-codes 61 / 62 / 71 / 72

We extend the official numbering rather than replace it. The expanded id
keeps the original `genTtbarId` prefix (`value / 100`, e.g. the `2` in
`253`) and only overrides the **last two digits** for tt+≥3b events:

| expanded `%100` | meaning | additional b-jets | b-hadron multiplicity flag |
|---|---|---|---|
| `61` | **tt+bbb** | exactly 3 | `nAddBJetsMulti == 0` |
| `62` | **tt+bbb** | exactly 3 | `nAddBJetsMulti ≥ 1` |
| `71` | **tt+4b**  | ≥ 4        | `nAddBJetsMulti == 0` |
| `72` | **tt+4b**  | ≥ 4        | `nAddBJetsMulti ≥ 1` |

The `1` / `2` split (61 vs 62, 71 vs 72) mirrors the official `53/54/55`
logic — it records whether any additional b-jet contains ≥2 b-hadrons —
so the expanded scheme is a *continuation* of the official one, not a
parallel invention. For stitching we only ever group them as
**tt+bbb = 61 ∪ 62** and **tt+4b = 71 ∪ 72**, and
**tt+nb = 61 ∪ 62 ∪ 71 ∪ 72**.

Everything below `61` is left exactly as NanoAOD reported it (`0`, `41–45`,
`51`, `52`, `53`, `54`, `55`). An event that NanoAOD called `53` but that
truly has 3 additional b-jets becomes `61` (prefix preserved: `253 → 261`).

### 9.3 Building the lookup from MiniAOD — `extractTtNb`

The producer is `extractTtNb.cc` in the `TtbarIdHistCompare` tool, run over
**MiniAODv2** (the same provenance as the official NanoAOD categorizer, so
the two see identical generator content). For each sample it writes a small
ROOT file:

```
ttnb_<sampleName>.root          # e.g. ttnb_tt4b.root, ttnb_TTToHadronic.root
└── TtNb  (TTree)
      run                 /i
      luminosityBlock     /i
      event               /l
      genTtbarId          /I    # original NanoAOD value (for the self-check)
      Expanded_genTtbarId /I    # 61/62/71/72 (+ preserved prefix)
      nAddBJets           /I
      nAddBJetsMulti      /I
```

Crucially, the file contains **only the events that need correcting** —
those with `Expanded_genTtbarId % 100 ∈ {61,62,71,72}`, cross-checked
against `nAddBJets ≥ 3` (the producer hard-aborts if those two disagree,
so a written file is internally consistent by construction). Every other
event is *absent*, and the analyzer will simply keep its NanoAOD value.

**Analogy.** This is a **guest list of exceptions**, not a full census.
Only the handful of guests who need a special wristband are on it; everyone
else walks in on their NanoAOD ticket.

### 9.4 Applying the lookup at run time — the `ExpandedTtbarId` class

`ExpandedTtbarId` (`src/ExpandedTtbarId.{h,cc}`) is a standalone, ROOT-only
helper — no CMSSW, no treestream dependency — with three entry points:

- **`load(path, label, tree="TtNb")`** — reads one `ttnb_*.root` into an
  in-memory hash map keyed by `(run, luminosityBlock, event)`.
- **`loadFromDir(dir, sampleKey, tree="TtNb")`** — builds
  `<dir>/ttnb_<sampleKey>.root` automatically and loads it. If the exact
  name is missing it falls back to distinctive token matching
  (`tt4b`, `ttbb_Hadronic`, `TTToSemiLeptonic`, …). If nothing matches it
  goes **INACTIVE** with a loud `tried: <path>` log — *not* fatal, so a
  sample with no tt+nb file just keeps NanoAOD values.
- **`resolve(run, lumi, event, nanoGenId)`** — the per-event call. If the
  key is in the map it returns the expanded id (`61/62/71/72`); otherwise
  it returns `nanoGenId` **unchanged**.

Two safety mechanisms matter:

1. **The key is `(run, luminosityBlock, event)`, all three.** `event` is
   only unique *within* a lumisection, and for MC `run` is always `1`, so
   the triplet is the minimal unambiguous identifier. The hash is FNV, and
   it is **byte-identical** to the one in the validation tool
   (`matchTtbarId.cc`) — so the analyzer reproduces the validated matching
   exactly, not just "equivalently."
   **Analogy.** Two people can share a first name (`event`); you need
   name + date + address (`run, lumi, event`) to be sure you tagged the
   right one.
2. **Per-hit genId self-check.** When a key matches, the stored
   `genTtbarId` must equal the NanoAOD `genTtbarId` the analyzer is holding
   for that event. If they differ, the wrong `ttnb_*.root` was loaded for
   this sample, and the analyzer **aborts (exit 44)** by default
   (downgradable via `setAbortOnGenIdMismatch(false)`).
   **Analogy.** Before handing over the wristband, check the photo on the
   ID matches the face.

### 9.5 Analyzer integration — two independent touch-points

The integration is split so that the per-event label and the normalization
bookkeeping never depend on each other.

**(a) CORE — the per-event branch (active in `main` mode).**
In `process()`, inside the MC block, the analyzer sets

```
_genTtbarIdNano  = _ev->genTtbarId;                       // raw NanoAOD
_expandedTtbarId = _expTtbarId.resolve(_ev->run,
                       _ev->luminosityBlock, _ev->event,
                       _ev->genTtbarId);                   // 61/62/71/72 or unchanged
```

and writes **both** into the output Events tree as `genTtbarId/I` and
`expandedTtbarId/I`. This call is gated on *MC vs Data*, **not** on the
analysis mode, so the expanded label is written for every selected MC event
in a normal `--mode main` run. The lookup is loaded once in `main()` from a
directory (default `DerivedCorr/expandedTtbarId/`, overridable by the
`EXPANDED_TTBARID_DIR` environment variable), so the per-sample file is
resolved automatically from the sample name.

**(b) §5 — the prescan ΣgenW partition (active in `prescan` mode).**
In `accumulatePrescanEvent()`, the same `resolve()` is used to route each
event's *generator weight* into the correct bin. tt+nb events are **moved
out** of `53/54/55` into new bins `61/62/71/72`, so after §5 the `53/54/55`
prescan bins hold the **pure tt+2b** remainder. The prescan TTree
(`prescan`) gains eight branches: `sumGenW_id_{61,62,71,72}` and
`n_id_{61,62,71,72}`.

The two are **not redundant** (this is the single most common point of
confusion):

| | CORE branch `expandedTtbarId` | §5 prescan `sumGenW_id_*` |
|---|---|---|
| granularity | one value **per event** | one scalar **sum per sample** |
| population | only events **passing selection** | **all** events, no selection |
| role | tells you *which bin* an event is in (the **numerator**, at plot/reweight time) | tells you *how much generated weight* each bin holds (the **denominator**, for normalization) |

**Analogy.** The branch is the **stamp on each cookie that passed
inspection**; the prescan sum is the **total flour milled** for each recipe,
weighed before any cookie was cut. You need the flour total to know how to
price each cookie — you cannot reconstruct it from the inspected cookies
alone, because the rejects never reached the tray.

### 9.6 Validation chain

The expanded id inherits the four-way validation of §5 for the *base*
categories and adds two checks specific to the lookup:

1. **Match reproducibility** — because the `(run,lumi,event)` key and FNV
   hash are byte-identical to `matchTtbarId.cc`, the set of events the
   analyzer relabels is, by construction, the same set the validation tool
   relabels. Independently re-deriving the lookup with `matchTtbarId`
   reproduces the analyzer's hit list.
2. **genId self-check** (§9.4) — guarantees the loaded file belongs to the
   sample being processed. A wrong-file load is impossible to miss: it
   aborts on the first mismatched event.
3. **Conservation, post-§5** — verified downstream by
   `consolidate_prescan.py` (§10.2): the full id partition still sums to
   the event total once `61/62/71/72` are included, proving the §5 move
   lost and double-counted nothing.

On a 1000-event TTToSemiLeptonic UL17 NanoAODv9 slice the class loaded,
matched, and self-checked cleanly; on the production tt4b run the load
summary reported tt+bbb (`61+62`) and tt+4b (`71+72`) consistent with the
`extractTtNb` totals for that sample.

---

## 10. From categories to weights — prescan, consolidation, stitching

This section documents the path from the §9 prescan output to the
per-event weights the analyzer will use to combine the inclusive ttbar,
4FS ttbb, and dedicated tt4b samples without double-counting.

### 10.1 Why stitching is needed (the double-counting problem)

Three samples all generate tt+heavy-flavour events:

- **inclusive** `TTToHadronic` — every ttbar final state, including some
  tt+bb and tt+nb in its tail;
- **4FS** `ttbb_Hadronic` — a dedicated, higher-statistics tt+bb sample;
- **dedicated** `tt4b` — a tt+≥3b sample.

If we simply added all three, the tt+bb and tt+nb phase space would be
counted **two or three times**. Stitching assigns each gen-level category
to **exactly one** sample and rescales the dedicated samples so their yield
matches what the inclusive sample predicts at NNLO.

**Analogy.** Three bakers each make cookies, but their menus overlap. We
declare: the **generalist** owns plain + chocolate-chip + single-nut
(LF / cc / tt+b); the **bb specialist** owns double-nut (tt+2b); the
**4b specialist** owns triple- and quadruple-nut (tt+bbb / tt+4b). Each
specialist then *adjusts their batch size* (the r-factor) so they bake
exactly as many specialty cookies as the generalist *would have* — no more,
no fewer. Now we can pool all trays with no double-counting.

### 10.2 Step 1 — consolidate and QA the prescan: `consolidate_prescan.py`

The analyzer's `prescan` mode writes one `prescan` TTree row per output
file. `consolidate_prescan.py` walks the output area, sums every per-job row
into a per-sample record, and runs cross-checks. It is the **bookkeeping
gate**, not the physics step.

Updated for the expanded scheme, it now:

- reads the 17-bin id partition including `61/62/71/72` (older pre-§5 files
  contribute 0 to those bins);
- enforces, as **hard** checks: the full id partition sums to the event
  total (this now *also* proves the §5 move conserved events); the NanoAOD
  ttCat partition sums to the total; and LF / cc match 1:1 between the two
  labelings (tt+nb never lands in LF/cc);
- reports, as **informational**, an *expanded reconciliation*: the NanoAOD
  `ttCat` 2b+ count vs the expanded `id` (pure-2b + tt+nb). A small nonzero
  difference is expected — it is the handful of tt+nb events the official
  categorizer had labelled 1b, an algorithmic (official-vs-ghost-matching)
  difference, not an error;
- emits `prescan_summary.json` (full) and `prescan_summary.csv` (headline,
  with `n_id_bbb61/bbb62/4b71/4b72` columns).

Run it inside CMSSW (PyROOT); a non-zero exit flags any bad/missing job or
failed hard check, convenient for chaining a resubmit.

### 10.3 Step 2 — compute the factors: `compute_stitch_factors.py`

This is the physics step. It re-aggregates the `prescan` rows (now via the
correct tree name `prescan`) and computes, **per dedicated sample**, the
rescale factor that anchors it to the inclusive NNLO prediction.

**Decay channel and heavy-flavour content are orthogonal axes.** ttbar splits
two independent ways: by W-decay channel (full-hadronic / semi-leptonic /
di-leptonic — mutually exclusive, no overlap) and by HF content (LF / cc /
tt+b / tt+2b / tt+nb — where inclusive and the dedicated samples overlap).
Stitching resolves only the HF overlap; **all three decay channels are kept
and stacked**. The "reject" below removes HF *categories*, never a decay
channel — semi-leptonic and di-leptonic ttbar that survive the FH selection
are real background and are not discarded.

**Which channels each dedicated sample covers (confirmed by a gen census +
sample inventory).** A decay-channel census on the `tt4b` NanoAOD (counting
W→ℓν legs by mother-is-W) returned full-hadronic 45.6%, semi-leptonic 43.8%,
di-leptonic 10.6% — i.e. **`tt4b` is decay-inclusive** (covers all three
channels). For ttbb, **per-channel 4FS samples exist for all three W-decay
channels** (`ttbb_Hadronic`, `ttbb_SemiLeptonic`, `ttbb_2L2Nu`), so each is
stitched into its own channel. This fixes the anchoring:

| dedicated sample | covers channels | owns HF cats | anchored to | σ_inc used |
|---|---|---|---|---|
| `ttbb_Hadronic` | hadronic only | tt+2b (53,54,55) | `TTToHadronic` | σ_total × BR_had = 377.96 pb |
| `ttbb_SemiLeptonic` | semi-lep only | tt+2b (53,54,55) | `TTToSemiLeptonic` | σ_total × BR_SL = 365.46 pb |
| `ttbb_2L2Nu` | di-lep only | tt+2b (53,54,55) | `TTTo2L2Nu` | σ_total × BR_DL = 88.34 pb |
| `tt4b` (decay-incl.) | Had + SL + DL | tt+bbb, tt+4b (61,62,71,72) | `TTToHadronic` + `TTToSemiLeptonic` + `TTTo2L2Nu` (merged) | σ_total = 831.76 pb (no BR) |

with W-decay branching fractions BR_had = 0.6741² = 0.45441081,
BR_SL = 2·0.6741·0.3259 = 0.43937838, BR_DL = 0.3259² = 0.10621081 (Σ = 1).

The anchor phase space must match the dedicated sample's coverage, or the
denominator is inconsistent: each per-channel ttbb is anchored to its matching
inclusive (BR_c multiplies σ); the decay-inclusive `tt4b` is anchored to the
full inclusive merge (no BR factor). Because the W-decay channels are orthogonal
(§ above), this per-channel anchoring is exactly the ttHH AN prescription — §3.3
SL Option1 (gen-level partition) and §3.4 DL gen-level 4b exclusion both keep
SL/DL and remove the overlapping gen category from them.

> **History / migration note (2026-06).** An earlier revision stitched only
> `ttbb_Hadronic`, leaving SL/DL tt+2b as the 5FS inclusive (`TTToSemiLeptonic`
> and `TTTo2L2Nu` kept their tt+2b). That was a conservative leftover from when
> the SL/DL 4FS ttbb samples were assumed unavailable. With all three samples
> confirmed, `EXP` now rejects tt+2b from **all three** inclusive samples and
> fills each with the matching `ttbb_c`. The old behaviour is preserved as the
> config option `EXP_TTBB_HAD_ONLY` (fallback only). The σ_dedicated for SL/DL
> defaults to BR-scaling the hadronic 1.452 pb (σ_ttbb_total ≈ 3.195 pb → SL
> 1.404 pb, DL 0.339 pb) and **cancels** against the YAML weight, so only the
> YAML-vs-config consistency matters, not the absolute value (use XSDB σ if the
> YAML uses those).

**The factors**, computed per dedicated sample *d*:

```
f_d  = Σgenw_anchor(d, owned cats) / Σgenw_anchor(d, all)
r_d  = σ_inc(d) · f_d / σ_dedicated(d)
```

concretely:

```
# per-channel ttbb (c = Had / SL / DL), each anchored to its own inclusive:
f_B,c = Σgenw_c(53,54,55)          / Σgenw_c(all)            # channel-c anchor
r_B,c = (σ_total · BR_c) · f_B,c   / σ_ttbb,c

# decay-inclusive tt4b, anchored to the three-channel inclusive merge:
f_4b  = Σgenw_HadSLDL(61,62,71,72) / Σgenw_HadSLDL(all)      # all-channel anchor
r_4b  = σ_total · f_4b             / σ_tt4b
```

`f_4b` is built from the **real** tt+nb bins `61/62/71/72` (the point of §9),
summed over the **three-channel inclusive merge** (because `tt4b` covers all
three). Each `f_B,c` is measured in the single matching inclusive `TTTo_c`. The
dedicated LHE σ (σ_ttbb,c, σ_tt4b) cancels against the dedicated sample's
standard YAML normalization (next step), so its absolute value does not bias the
result.

(If only `ttbb_Hadronic` is available — no SL/DL 4FS ttbb — switch the config
option to `EXP_TTBB_HAD_ONLY`, which stitches ttbb in the hadronic channel only
and leaves SL/DL tt+2b as the 5FS inclusive fallback. If your `tt4b` is instead
hadronic-only — census ~100% full-hadronic — use `EXP_HAD_TT4B`.)

### 10.4 Step 3 — the analyzer config: a per-(sample, category) multiplier JSON

**Why not bake it into the per-sample YAML `weight`?** Because stitching is
**per HF category within a sample**: each `TTTo_c` must *keep* its LF/cc/tt+b
events but *reject* its tt+2b and tt+nb, where the rejected tt+2b is filled by
the matching per-channel `ttbb_c` and the rejected tt+nb by the decay-inclusive
`tt4b`. One scalar per sample cannot express "keep these categories, drop
those." A **separate JSON** is the right shape.

`compute_stitch_factors.py` emits `analyzer_perEvent_factor`: a per-(sample,
category) **multiplier applied on top of the existing YAML base weight**.

```json
"analyzer_perEvent_factor": {
  "application": "final_weight = yaml_base_weight[sample] * multiplier[sample][category] * genWeight * (PU*btagSF*trigSF)",
  "sub_to_category": { "0":"LF", "41":"cc", ..., "61":"ttbbb", "71":"tt4b" },
  "categories": ["LF","cc","ttb","tt2b","ttbbb","tt4b"],
  "samples": {
    "TTToHadronic":      { "role":"inclusive", "by_category": { "LF":1, "cc":1, "ttb":1, "tt2b":0, "ttbbb":0, "tt4b":0 } },
    "TTToSemiLeptonic":  { "role":"inclusive", "by_category": { "LF":1, "cc":1, "ttb":1, "tt2b":0, "ttbbb":0, "tt4b":0 } },
    "TTTo2L2Nu":         { "role":"inclusive", "by_category": { "LF":1, "cc":1, "ttb":1, "tt2b":0, "ttbbb":0, "tt4b":0 } },
    "ttbb_Hadronic":     { "role":"dedicated", "r":r_B_Had, "by_category": { "tt2b":r_B_Had, "...":"rest 0" } },
    "ttbb_SemiLeptonic": { "role":"dedicated", "r":r_B_SL,  "by_category": { "tt2b":r_B_SL,  "...":"rest 0" } },
    "ttbb_2L2Nu":        { "role":"dedicated", "r":r_B_DL,  "by_category": { "tt2b":r_B_DL,  "...":"rest 0" } },
    "tt4b":              { "role":"dedicated", "r":r_4b,    "by_category": { "ttbbb":r_4b, "tt4b":r_4b, "...":"rest 0" } }
  }
}
```

Reading it:

- **inclusive samples** carry `1` on the categories they keep and `0` on the
  categories rejected (filled by a dedicated sample). All three `TTTo_c` reject
  tt+2b **and** tt+nb: tt+2b is filled by the matching per-channel `ttbb_c`, and
  tt+nb by the decay-inclusive `tt4b`. Each keeps only LF/cc/tt+b.
  Inclusive is the anchor — never rescaled, hence multiplier `1`, not an r.
- **dedicated samples** carry their rescale factor `r` on the categories they
  own, `0` elsewhere. The three `ttbb_c` each carry `r_B,c` on tt+2b (note the r
  differs per channel — different f and Σgenw); `tt4b` carries `r_4b` on tt+nb.
- **samples not listed** (QCD, ttV, signal, …) are untouched — multiplier `1`.

The multiplier is applied **on top of** the standard YAML normalization, so the
per-channel base weights you already have remain the single source of base
normalization and the JSON adds only the keep/reject mask and the r rescale.
One consistency requirement: the dedicated samples' YAML weights must be the
standard `Lumi·σ_dedicated/Σgenw`, using the **same** σ_dedicated as the config
— per-channel `σ_ttbb,Had = 1.452 pb`, `σ_ttbb,SL ≈ 1.404 pb`,
`σ_ttbb,DL ≈ 0.339 pb` (BR-scaled defaults; substitute official XSDB σ if your
YAML uses those), and `σ_tt4b = 0.296 pb`. σ_dedicated cancels in `base · r`
regardless of its value, so the only thing that matters is that the YAML σ and
the config σ agree channel-by-channel.
`compute_stitch_factors.py` also prints the full `Lumi·σ_inc·f/Σ_dedicated` per
dedicated as a cross-check against `YAML_weight · r`.

Recommended location: `DerivedCorr/stitchFactors/stitch_factors_2017.json`,
alongside the trigger-SF and b-tag JSONs the `CorrectionsManager` already loads.

**How the analyzer consumes it (planned).** Per MC event:
`cat = sub_to_category[expandedTtbarId % 100]`; if `sampleName` is listed,
`weight = YAML_weight × multiplier[sampleName][cat] × genWeight × SFs` (a `0`
drops the event); otherwise the YAML weight is used unchanged. The C++ that
loads and applies the JSON is the next implementation step (§10.5).

### 10.5 What is and isn't wired yet

| piece | status |
|---|---|
| per-event `expandedTtbarId` branch (`main`) | **done** (§9.5a) |
| §5 prescan ΣgenW partition into `61/62/71/72` | **done** (§9.5b) |
| `consolidate_prescan.py` QA over the expanded bins | **done** (§10.2) |
| `compute_stitch_factors.py` → r_B, r_4b, per-event factors, JSON | **done** (§10.3–10.4) |
| per-channel ttbb stitch (Had/SL/DL each anchored to its own inclusive) | **done** (2026-06; §10.3, see changelog) |
| `consolidate_prescan.py` validated on full 2017 set (39 samples, all checks pass, signed-ΣgenW confirmed) | **done** (2026-06) |
| statistical uncertainty on f / r (effective-N via per-bin Σw²) | **TODO** (§10.6) |
| analyzer **loads** `stitch_factors_2017.json` and applies it per event | **TODO** (§10.6) |
| b-tag normalization reweight keyed on `expandedTtbarId` (currently uses NanoAOD `genTtbarId`, so tt+nb is not yet distinguished in the reweight) | **TODO** |
| stitched data/MC stack | **downstream / TODO** |

The b-tag reweight point matters: high b-jet multiplicity is exactly where
tt+nb lives, so once stitching is applied the reweight key
(`MakeProcessKey(sampleName, genTtbarId)`) should switch to
`expandedTtbarId`, after confirming the process-key map recognizes the
`61–72` codes.

### 10.6 Future work (TODO, prioritized)

These are deferred items, recorded so the next session/agent can pick them up
without rediscovery. None blocks the current central-value stitch factors —
they are correct as computed (validated 2026-06).

1. **Statistical uncertainty on `f` and `r` (effective-N).**
   The central values use signed ΣgenW and are correct, but no statistical
   error is propagated. With negative weights the effective sample size is
   `N_eff = (Σw)² / Σw²`, smaller than the raw count. To produce errors on
   `f = Σw_owned / Σw_all` (and hence on `r`):
   - **analyzer (C++) prescan writer** — add a per-bin sum-of-squares branch
     `sumGenW2_id_{i}` alongside the existing `sumGenW_id_{i}` (fill with
     `genWeight*genWeight` in the same bin). Currently only the Runs-level
     `genEventSumw2` total exists, which is not enough for per-category errors.
   - **`compute_stitch_factors.py`** — read the new per-bin Σw², compute
     `N_eff` per (sample, category), and propagate a binomial-on-weighted
     error to `f` and `r`; emit `r_err` into the JSON `result` block.
   - **Why it matters here:** the diagnostic numbers show this bites hardest in
     low-statistics anchors — `f_4b ≈ 9.8e-5` is built from only ~66k inclusive
     tt+nb events (TTTo2L2Nu contributes ~8.6k), and `ttbb_2L2Nu` tt+2b has the
     fewest events of the three ttbb channels (~312k). So `r_4b` and `r_DL`
     carry the largest relative uncertainty and should be quoted with errors
     before being used in a final fit.

2. **Analyzer (C++) loads and applies `stitch_factors_2017.json` per event.**
   The JSON (`analyzer_perEvent_factor` block) is ready; the analyzer must:
   - load it via the same `CorrectionsManager` convention used for the
     trigger-SF / b-tag JSONs, from `DerivedCorr/stitchFactors/`;
   - map `expandedTtbarId % 100` → category via `sub_to_category`;
   - apply `final_weight = yaml_base_weight[sample] * multiplier[sample][category]
     * genWeight * (PU*btagSF*trigSF)`;
   - **prerequisite:** the dedicated samples' YAML base weights must use the same
     per-channel σ as the JSON config (`σ_ttbb_Had=1.452`, `σ_ttbb_SL≈1.404`,
     `σ_ttbb_DL≈0.339`, `σ_tt4b=0.296` pb). σ_dedicated cancels, so only
     YAML↔config agreement matters — verify before first stitched stack.

3. **Switch the b-tag normalization reweight key to `expandedTtbarId`** (see
   table note above) so tt+nb is distinguished in the reweight.

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
