# Decision Log — ttHH(HH→bbbb) Fully-Hadronic Analysis

> **Purpose:** record each significant decision — the choice, why, what else was considered, and whether it still holds — so no one silently reopens a settled question or treats a proposal as settled.
> **Audience:** anyone about to change behavior or unsure whether something is fixed.
> **Status:** living, append-and-supersede · last meaningful update **2026-10-10 (3)** (D-2026-10-10-C PROPOSED: the stitching factor misses p_own — the stitched tt+bb is about 1/9 of its target — and the tt+b/tt+2b owner differs from the AN; no main run uses stitching yet) · 2026-10-10 (2) (D-2026-10-10-B: the direction of the whole FH program — AN-based ML, Tree branches now, uncertainties, the Korean AN, QCD from data after the lepton CRs, SWAN on Run 2 + 2024, full Run 2 + 2024 control plots; D-2026-10-10-A implemented in STEP 27) · 2026-10-10 (D-2026-10-10-A: the fixed-WP weight after BTV's multi-WP guidance — intermediate factor 1, closure per true b jets, comparison weights; c jets after BTV answers) · 2026-10-08 (D-2026-10-08-A: 2024 b-tag SF by the fixed-WP method now, the shape method when BTV's is confirmed; the 2024/2018 control-plot program) · 2026-10-07 (D-2026-10-07-B: 2024 lepton CRs now, lep samples after the storage check, next work; D-2026-10-07-A: the whole 2024 main again with ParkingHH, the first look kept as `_firstlook`) · 2026-10-06 (D-2026-10-06-A: 2024 b-tag paths in ParkingHH, the PD rule, tree control plots) · 2026-10-05 (D-2026-10-05-A: 2024 first-look scope, D14 109.816, D15 PU inputs; D-2026-10-02-E DECIDED; D-2026-10-05-B 2024 MC 4J3T bit PROPOSED; D-2026-10-05-C 2024 without tt+nb lookup; D-2026-10-05-D 2017 v20 removed from KNU) · 2026-10-04 (D-2026-10-04-A: run records are committed) · 2026-10-02 (D-2026-10-02-A…E: v15 analyzer order, eventBuffer policy, event cleaning, answers N0–N8, blinding).
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

## D-2026-10-02-A — v15 analyzer: 2024 first; 2017 after a v15 2017 production; 2018 v15 and v9 only if needed · **DECIDED**

**Decision (user, 2026-10-02).** The first year taken through the whole analyzer chain on v15 ntuples is **2024**. 2017 waits for a v15 2017 production (today's 2017 ntuples are v9). 2018 v15, and 2017/2018 on v9, are run only if needed (e.g. for the cross-year QCD comparison). User's words: "17년도는 v15로 이후 진행해보자. 지금은 v9 샘플이니. 그리고 일단 18년도 또한 tt4b 등이 없으니 우선 진행은 24년도로 하는게 첫 선택임. 필요한 경우 v9으로 17, 18년도도 해보는걸 고려해 볼 것임".
**Why.** Summer24 v15 has the signal (`TTHH-HHto4B`), `TT4B` and ttbb 4FS (`TTBB*`) as central datasets; 2018 v15 has neither the signal nor `TT4b` (they need the enriched production, NtupleForge track V5).
**Consequence.** [`PLAN_v15_2018UL_2024.md`](PLAN_v15_2018UL_2024.md) §9 replaces the order of its §5 (2018UL first, PROPOSED 10-01). The 2024 tt+nb split and `TT4B` stitching still need a 2024 tt+nb patch (NtupleForge V6 = TTHHGenCategoryTools O5, not started); until then the 2024 plots keep tt+≥3b inside tt+B and do not use `TT4B` (PLAN §7 D6).
**Alternatives considered.** 2018UL first (PLAN 10-01) — superseded for the reason above.

## D-2026-10-02-B — eventBuffer.h is regenerated whenever the input branch set changes; no silent 0 for a branch the analysis reads · **DECIDED** (implementation PROPOSED, PLAN §9 Stage 1)

**Decision (user, 2026-10-02).** "eventBuffer의 경우 branch가 달라지면 때마다 돌려주는게 맞다. branch에 대해 접근할 시 없는 값인데도 0으로 취급되어 돌아가는 경우를 잘 다뤄야 함." Concretely: (1) the generator lives in this repository and the header is regenerated from real NtupleForge ntuples whenever a branch list (`NtupleForge/branches/*.txt`) or the NanoAOD version changes; the header records its inputs (files, branch-set md5) and the analyzer prints them at start; (2) a required-branch list per (year, MC/Data) — every branch the analysis reads — is checked for **every input file**: not chosen, or absent in the file → FATAL with the names; (3) the same check runs in the file-list stage before any job is submitted.
**Why.** Today's header (`mkanalyzer.py` v2.0.3, 2026-06-29) wraps every branch in `if (input->present(b)) select(...) else missingBranches`, so an absent branch keeps the 0 of `initBuffers()` and only a MISSING list is printed; the `_select` FATAL in `src/treestream.cc` is never reached. The v15 renames and removals (`Jet_jetId`, `Jet_puId`, `MET_pt`, `fixedGridRhoFastjetAll`, `Electron_mvaFall17V2Iso_WP90`) therefore become silent zeros (PLAN §2 1–3).
**Alternatives considered.** Hand-insert the new branches into the existing header (PLAN §7 D7's earlier AI recommendation) — rejected by this decision: it repeats the 2018 HLT hand edit (PLAN §2 20) and leaves the generator missing.

## D-2026-10-02-C — Event cleaning is one explicit per-year checklist; nothing implicit · **DECIDED** (values per year PROPOSED until PLAN §9 Stage 0)

**Decision (user, 2026-10-02).** "모든 연도의 event cleaning 작업에 대해서도 잘 고려해야함. 17년도는 prefileing, 18년도는 HEM, 24년도는 아마도 Bpix, Fpix veto가 있을 것이고 golden json이나 pu weight 등도 모두 빠짐없이 해 주어야 한다." The table in PLAN §9.2 is the single list. Every item has a value, a source and a code location per year; an item may be "not applied" only with a reason (AN or POG). Year switches live in `include/EraConfig.h`; the analyzer prints the active cleaning items at start, and the smoke tests report a pass fraction per item.
**Why.** Every year-dependent item that went wrong so far failed silently (EraConfig.h header). A checklist with a value per year makes a missing item visible.
**Recorded with it.** Run 2: no jet veto map (the AN uses none; EraConfig comment) — re-examine before final results; 2018 HEM = JES variation per AN-2022/122 v26 p.118, not a veto. 2024: jet veto map event veto (JME: all Run 3 analyses); whether BPix/FPix come as separate map types is read from the 2024 payload (Stage 0).

## D-2026-10-02-D — Answers N0–N8 (user, 2026-10-02) · **DECIDED** unless marked

| # | Question (AI plan, 10-02) | Answer | Consequence |
|---|---|---|---|
| N0 | Was the 2017 chain run with the current code? | "안돌림" | No 2017 output from this code exists: the 2017 reference outputs for the regression check Y1 must be made before the Stage 1 code changes land (PLAN §9) |
| N1 | 2017 lumi | "42.07로 고치자" | Applied 10-02: `Tier3_2017_FH_unified_{main,prescan}.yml`, `plotter/stack_plotter.C` label, `bTagSF_ReweightStudy/plot_btag{,_pyroot}.py` labels, `compute_stitch_factors.py` (diagnostic only); [`reference/LUMI_SOURCES.md`](reference/LUMI_SOURCES.md). Nothing to re-run (N0) |
| N2 | Lead-lepton thresholds | "trigger SF용도는 따로 두는게 맞는듯" | The trigger-SF lead-μ threshold is its own per-year constant (2024 from the IsoMu24 turn-on); the CR thresholds stay μ > 29, e > 30 for all years |
| N3 | MET | "그렇게 하라" | PF (Type-1) MET for Run 2 (`MET_pt` v9, `PFMET_pt` v15), PUPPI MET (`PuppiMET_pt`) for 2024 |
| N4 | Lepton SFs in the lepton CRs | "그렇게 하라" | Not applied at first (interim); the expected size is stated next to every CR number and measured once with the POG SFs |
| N5 | Data for the lepton CRs | "그렇게 하라" | Hadronic PDs (same hadronic trigger as FH): 2024 `JetMET0/1`; as `QUICK_COMMANDS.txt` already says |
| N6 | Hide data in some plots? | "왜 저 경우에 data를 숨겨야 해?" | Answered in D-2026-10-02-E (DECIDED 2026-10-05) |
| N7 | b-tag norm reweight regions | "eCR region을 추가하자" | `TTHH_BTAGRW_REGION` gets `eCR` (nElecs == 1, nMuons == 0, MET > `metCutCR`); needs the btagtrig skim to collect electrons without a lead muon (today only with one) — PLAN §9 Stage 7 |
| N8 | 2024 b-tag SF method | "BTV 자료 확인하고 하자" | Read the 2024 BTV payload (PLAN §9 Stage 0) and the BTV documentation before choosing (PLAN §9.6 D10) |

## D-2026-10-02-E — Blinding: no data hidden in preselection or control-region plots; only the signal-sensitive bins of the final discriminant are blinded · **DECIDED** (proposed by the AI 2026-10-02; accepted by the user 2026-10-05, D-2026-10-05-A)

**Proposal.** Withdraws the AI's earlier suggestion to hide data in the ≥4b plots (user's question N6: "왜 저 경우에 data를 숨겨야 해?"). Data are shown in every preselection and control-region plot, including nb ≥ 4. Blinding applies later to the bins of the final discriminant (e.g. high BDT/DNN score in the ≥4b categories) where the expected S/B is largest, with the criterion and the unblinding procedure taken from the AN.
**Why.** Blinding exists so that selection choices are not tuned on data where the signal would show. At preselection the SM signal is negligible: σ(ttHH)·BR(HH→4b) = 0.756 fb × 0.339 = 0.256 fb at 13 TeV (`data/samples_2017UL.json`), i.e. about 11 events produced in 2017 (42.07 fb⁻¹) and about 28 in 2024 C–I (109.8 fb⁻¹, 13 TeV σ; the 13.6 TeV σ is higher, D5) before any selection and a few after it, while the background in the same selection is expected to be orders of magnitude larger. The data there cannot reveal the signal, and hiding it removes the most relevant validation region. Stage 8 writes the S/B per cut step and region, so this expectation is checked, not assumed. The 2017 step-8 study already showed nb ≥ 4 data.
**Alternatives considered.** Hide nb ≥ 4 everywhere — rejected for the reason above.

## D-2026-10-04-A — Run records of our own programs are committed; CRAB transcripts are not · **DECIDED**

**Decision (user, 2026-10-04).** "로컬 run 로그나 condor job 로그는 올려도 될텐데 (어차피 우리가 만든 cout이라면 말이야. 저번 메일에도 crab job만 안된다고 하지 않았던가)". The output of our own programs (analyzer builds, unit tests, file scans, test harnesses), run at KNU in a shell or as a condor job, is recorded by `tools/runlog/runlog.sh` as `runlogs/run_<step>_<UTC>.log` plus a line in `runlogs/LEDGER.tsv`, and committed. The AI session then reads the files (exact output, git commit, exit code) instead of pasted text, and the record outlives the session. A command line that mentions `crab` is logged under `runlogs/nocommit/` (gitignored) and left out of the ledger; so is any output that turns out to contain a pre-signed URL signature.
**Why.** CRAB submit transcripts embed pre-signed S3 URLs, i.e. credentials (NtupleForge `script/runlog.sh` header and its `docs/03_DECISIONS.md` D-2026-08-17-no-logs-in-git — the reason that `runlog.sh` already routes them to `nocommit/`); our own `cout` carries none.
**Scope guard.** (1) condor's own files (`condor/runlog/<step>_<UTC>/job.out|err|log`) stay in the gitignored `condor/`: `job.log` holds the IP addresses and ports of the schedd and the worker, and the useful part of `job.out` is already in the runlog. (2) The user's 2026-10-02 rule for the PUBLIC NtupleForge repository (the jsonpog payload inventory and preliminary lumi logs are not committed there; the numbers live in this repository) is unchanged. (3) As a safety net, `runlog.sh` also moves a log to `runlogs/nocommit/` when the output turns out to contain a pre-signed URL signature (a script that calls crab inside).
**Consistency with NtupleForge `docs/03_DECISIONS.md` D-2026-08-17-no-logs-in-git.** Its tier 1 (CRAB transcripts never) is kept as is. Its tier 2 (bulk run logs — local test output, condor job logs, hadd transcripts — not by default) is kept by keeping bulk output out of the record: condor's own files stay in `condor/`, `tools/stage0/build_check.sh` writes the full make output under `condor/build/` and prints a summary, and the record's footer flags a record over 2 MB. What is committed is the step record, as NtupleForge does with `script/runlogs/` since 2026-09-16.
**Alternatives considered.** Copy logs to the Mac with scp, as for the lxplus logs of 10-02 — kept as a fallback, but it is a manual copy every round and leaves nothing in git. Push from inside the condor job — rejected: the worker has no usable ssh key (the user's keys need a passphrase).
**Detail.** `changes/STEP_23_runlog_condor.md`; usage `../tools/runlog/README.md`.

## D-2026-10-05-A — 2024 first look: FH without trigger and b-tag SF, lumi 109.816, data shown except the SR bins; PU weights from the official DQM histograms · **DECIDED**

**Decision (user, 2026-10-05, answers to four questions; "얼른 plot 만들고 싶은데").** (1) The first 2024 plots are the FH region **without
trigger SF and without b-tag SF**; the cross sections are the provisional 13.6 TeV values, marked as such in the sample DB and on the plots.
(2) PLAN §9.6 D14: the 2024 normalization lumi is **109.816 fb⁻¹** (C–I, normtag_PHYSICS). (3) D-2026-10-02-E is accepted: data are shown,
only the signal-region bins are blinded. (4) D15, PU weights: the user asked "pu weight 는 보통 그냥 값이 주어지지 않나?" and supplied the
EOS listing of `Collisions24/PileUp/` and the jsonpog `POG/LUM` listing (no 2024).
**Consequence for D15 (AI, from those facts).** The data side is the official DQM histogram `dataPileupHistogram-2024CDEFGHI_Golden-69200ub.root`
(nominal; 66000 ub and 72400 ub as down/up, i.e. 69.2 mb ±4.6 %), no pileupCalc. Its April lumisection set differs from our 2026-08-04
golden JSON by 0.2 % of the lumi (`changes/STEP_24_stage1_2_2024.md` §2), too little to change the profile. The MC side is
`Pileup_nTrueInt` of the **unskimmed** Summer24 ZZ pilot, because our 2024 production is skimmed (6j20) and a jet-count skim biases the
pileup profile; all Summer24 samples share one premix library. Weights: `tools/stage2/pu_weights.py`, output
`DerivedCorr/PU/2024_Summer24/puWeights_2024.json` in the jsonpog shape; a central jsonpog 2024 file replaces it when it appears.
**Alternatives considered.** pileupCalc on our golden JSON — not needed at a 0.2 % LS difference (kept as the fallback). The MC profile of our
own (skimmed) ntuples — the PLAN's first draft; rejected for the skim bias. The premix scenario's probability table from `SimGeneral/MixingModule`
— the Summer24 scenario name is not known here; the realized distribution of 4.8 M unskimmed events serves the purpose.
**Detail.** `changes/STEP_24_stage1_2_2024.md`; PLAN §9.6 D14, D15.

## D-2026-10-05-B — 2024 MC: the 4J3T trigger bit is the PNet path alone; Data take PNet OR DeepJet · **PROPOSED** (AI, 2026-10-05; used by the first look)

**Proposal.** In 2024 Data the 4J3T path is `..._TriplePFBTagDeepJet_4p5` in the first 38 runs (6.354 fb⁻¹ BRIL, 5.8 % of 109.8 fb⁻¹)
and `..._PNet3BTag_4p3` after, never both in one run ([`reference/LUMI_SOURCES.md`](reference/LUMI_SOURCES.md) §6.3), so a Data event
takes their OR. Summer24 MC carries both bits; it uses the **PNet bit only**, as 2017 MC uses the C–F path names and not the era-B ones
(`ttHHanalyzer_unified.cc` createObjects, 2024 branch). The 6J1T, 6J2T and HT1050 groups are unchanged (one path each).
**Why.** It is the menu that recorded 94 % of the luminosity, and the trigger SF (PLAN §9 Stage 6) is measured with exactly this
definition (Data OR, MC PNet), so the remaining 5.8 % difference goes into the SF. In the first look (no trigger SF, D-2026-10-05-A)
it is a known, small mismodelling of the 4J3T leg only.
**Alternatives considered.** MC takes the OR — rejected: it counts events that no Data run could have, the OR of two b-tag algorithms
is more efficient than either. MC draws the DeepJet bit for 5.8 % of its events (lumi-weighted emulation) — closer, but it adds
randomness to every MC histogram before the SF exists; reconsider in Stage 6 if the SF shows a period dependence.
**Supersede when.** Stage 6 defines the MC emulation together with the trigger SF.

## D-2026-10-05-C — 2024 runs without a tt+nb lookup: Expanded_genTtbarId inactive, tt+≥3b stays in tt+B · **DECIDED** (implements D-2026-10-02-A)

**Decision.** D-2026-10-02-A already fixed that until a 2024 tt+nb patch exists (NtupleForge V6 = TTHHGenCategoryTools O5) the 2024
plots keep tt+≥3b inside tt+B and do not use `TT4B`. The code did not allow it: `ExpandedTtbarId::loadFromDir` stops a sample of the
ttbar stitching set (`TTbar_*`, `TTbb_*`, `TT4b`; `SampleAlias::stitchPlanTable`) with E11 when no `ttnb_<sample>.root` is found, also
for an explicit `null`. Now `EraConfig::hasTtNbLookup(year)` is false for 2024, and a 2024 job with `path_expanded_ttbarid_dir: null`
runs every sample with the lookup **inactive** (NanoAOD `genTtbarId`, categories 51–55; 61/62/71/72 never appear) and says so in its
log. A path given behaves as before (missing file → E11). 2017/2018 are unchanged (lookups exist there; a stitch-set sample without
one is still E11).
**Why.** The first look needs the inclusive ttbar samples; the FATAL protects a production that relies on the tt+nb split, which the
2024 first look explicitly does not.
**Scope guard.** No stitching in 2024 either (`path_stitch_json: null`): the plots must not stack the 4FS `TTbb_*` or `TT4b` samples on
top of the inclusive ttbar (double counting) — `changes/STEP_24_stage1_2_2024.md` §6 lists the first-look sample set.
**Supersede when.** A 2024 tt+nb patch exists → set the path in the 2024 yml and flip `hasTtNbLookup("2024")`.

## D-2026-10-05-D — The 2017 v20 ntuples are gone from KNU: the v9 branch set comes from the 2026-06-29 header; no Y1 on real 2017 files for now · **DECIDED** (user, 2026-10-05)

**Fact.** `/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH2017UL_fullNano_v20` does not exist any more (KNU manifest record
`runlogs/run_knu_eventbuffer_manifest_20261005_094436.log`: `MISSINGBASE 2017`; Y1 `before`: all four inputs missing). User: "지웠음
(2017 은 나중에 v15로)" — 2017 comes back as a v15 production (D-2026-10-02-A).
**Decision.** (1) The new `include/eventBuffer.h` is the union of our v15 productions (2024, 2018) **plus the v9 branch records of the
current header** (2026-06-29, mkanalyzer.py v2.0.3, made from the 2017 v20 files): `tools/stage1/variables_from_header.py` writes them
to `tools/stage1/variables_v9_2017UL_fullNano_v20.txt` (1782 records), and `eventbuffer_manifest.py` merges them by default
(`--extra-variables`, the `mkvariables.py --merge` rules). The analyzer's 2017/2018 v9 code keeps compiling and reads v9 files with the
buffers it had. (2) The real-file Y1 check is not possible now; the 2017 evidence is the container comparison of the old and the new build
on synthetic 2017 files (identical outputs, `changes/STEP_24_stage1_2_2024.md` §8). `tools/stage1/y1_reference.sh` stays for the 2017 v15
production. (3) The committed `filelistTier3/` (2017) lists point to removed files.
**Alternatives considered.** Leave the v9 names out of the header — the Run 2 code would not compile (it reads `Jet_jetId`, `MET_pt`, …).
Make the v9 code conditional — a larger change for a path that has no input today.

## D-2026-10-06-A — 2024 Data: the b-tag paths are in ParkingHH; each PD takes its own paths (JetMET: HLT_PFHT1050, ParkingHH: the b-tag paths without it); control plots from the event tree until ParkingHH is produced · **DECIDED** (user, 2026-10-06: "둘 다", the AI's recommendation)

**Fact.** The 2024 HLT menus of runs 380115 (C), 382913 (F) and 386604 (I) (`hltGetConfiguration run:<N> --full`, `process.datasets`,
lxplus 10-06; tempTTHH `changes/STEP_24_stage1_2_2024.md` §17) record `HLT_PFHT1050` in `JetMET0` and `JetMET1` (and
`ScoutingPFMonitor`) and the four b-tag paths of our OR — 4J3T PNet (`(none)` in run 380115, before it ran) and DeepJet, 6J1T, 6J2T —
**only in `ParkingHH`**, together with the HH→4b path. 2024 Data was produced from JetMET0/1 only, on the belief (AI, STEP 24 code
comment) that JetMET recorded every hadronic path as JetHT did in 2018. So the Data held the b-tag events only when another JetMET path
had fired too, while MC has every HLT bit: the first-look Data/MC deficit below HT ≈ 1000 GeV (§16) is this.
**Decision.** (1) The analyzer takes, per 2024 Data PD, the paths that PD records, and an event in both PDs once, from JetMET — the 2017
BTagCSV/JetHT rule with the roles swapped: JetMET0/1 → `HLT_PFHT1050`; ParkingHH → (4J3T ‖ 6J1T ‖ 6J2T) && !`HLT_PFHT1050`; Muon0/1
(trigger-SF sample) → the whole OR; MC → the whole OR (unchanged; PNet 4J3T, D-2026-10-05-B). The submitter knows `ParkingHH_*` as
Data and its preflight WARNs per era when JetMET and ParkingHH are not both there. (2) **ParkingHH 2024 C–I is to be produced** like
JetMET (NtupleForge, the same skim/slim), after a DAS check that its NanoAOD v15 (`MINIv6NANOv15`) exists — it does (DAS 10-06:
C–I, eight datasets, 2,771 files, 1.91 G events; NtupleForge D-2026-10-06-parkinghh, `config_ttHH2024_v15_had_ParkingHH.yaml`). (3) Until then the control
plots use the trigger the produced Data hold in full: `HLT_PFHT1050` with offline HT > 1200 GeV (its plateau side; provisional value
until Stage 6 measures the turn-on), applied to Data and MC alike **from the event tree of the existing main outputs**
(`plotter/make_plots.py --tree-cut`; the tree holds every selected event with its trigger bits, HT, jets, b-tag scores, weights) — no
new analyzer run. (4) HT > 500 stays the analysis cut (ttH(bb) FH; it sits on the plateau of the HT legs of the b-tag paths,
`include/SelectionCuts.h`); the turn-on of the OR is handled by the trigger SF of Stage 6, measured with Muon0/1.
**Why.** The paths and the offline selection are the ttH ones; what changed in Run 3 is where CMS records them (ParkingHH, a parking PD
with its own rate budget, 2022–). Without the PD rule, adding ParkingHH would double count every event that fired `HLT_PFHT1050` and a
b-tag path.
**Alternatives considered.** Restrict the analysis OR to `HLT_PFHT1050` (JetMET only) — rejected for the analysis: it removes the region
HT 500–1100 GeV, a large part of the signal; kept only for the control plots of (3). Take ParkingHH alone (the b-tag paths) — loses the
high-HT events that fail the online b-tag. A new analyzer mode `--hadtrig ht1050` for the control run — dropped: the tree gives the same
events without a rerun.
**Supersede when.** The ParkingHH production is in: the 2024 main runs again with JetMET + ParkingHH (the full OR), and the control plots
move back to the stored histograms; the 1200 GeV plateau value is replaced by the Stage 6 measurement.

## D-2026-10-07-A — 2024 main again with ParkingHH: the whole main (MC and Data) with the G executable; the first-look outputs kept as `_firstlook` · **DECIDED** (user, 2026-10-07, the AI's recommendation)

**Context.** ParkingHH 2024 is produced (NtupleForge V60: 2,771 / 2,771 jobs finished, none failed) and the G build passed the 2024 smoke
(81/81; tempTTHH `changes/STEP_24_stage1_2_2024.md` §18). D-2026-10-06-A asked for the main to run again with JetMET + ParkingHH (the
whole OR) once ParkingHH was in; this entry is how.
**Decision.** Run the **whole** 2024 main again — MC 1,939 + JetMET 3,352 (new PD rule) + ParkingHH 2,771 = 8,062 jobs — with the G
executable and the main yml of commit H (ParkingHH added), into the same output name `AnalyzerOutput_main_notrig_2024`, after renaming the
first-look output directory and its condor directory to `..._firstlook`. The filelists are made again (the old ones kept as
`filelistTier3_2024_firstlook`); the MC lists must come out identical to the first-look ones, because the prescan sums of weights were
made from them (a difference stops the procedure; `make_plots.py`'s EVENTS check is the second guard).
**Why.** (a) A Data-only rerun into the old directory leaves the old-rule JetMET files in place: a job that fails would be counted as done
by `--report` / `--resubmit` through its old file, silently mixing the two rules. (b) One executable and one filelist set for the whole
main: every output is G. (c) The price is the 1,939 MC jobs, a few hours at KNU. (d) The first look stays readable
(`make_plots.py --base .../AnalyzerOutput_main_notrig_2024_firstlook`).
**Alternatives considered.** Data only (about 6,100 jobs; a Data-only yml, and the old JetMET outputs and merged files removed first) —
rejected for (a). The whole main under a new output name from a yml tag (submitter code) — not needed while a rename does the same.
**Note.** The MC histograms do not depend on the PD rule (MC takes the whole OR before and after), so the MC part reproduces the first look
up to the filelists.

## D-2026-10-07-B — 2024 lepton control regions now, before the W→ℓν/DY samples; next: nJets diagnostics, trigger SF, b-tag method, 2018 v15 · **DECIDED** (user, 2026-10-07)

**Context.** The user's 2017 study: with one lepton and a MET cut (QCD suppressed) Data/MC improves, but the jet multiplicity stays clearly
off. Is that 2017's alone or every year's (Run 3 too)? The 2024 HT1050 control region (tempTTHH `changes/STEP_24_stage1_2_2024.md` §18) shows
the same slope (nJets Data/MC 1.38 at 6 jets, about 2 at 13).
**Decision.** (1) Run the 2024 lepton CRs — `--region muon` and `--region electron`: the lepton veto replaced by one lepton, none of the other
flavour and MET > 20 GeV; trigger and Data PDs as FH — now, with the FH rerun of D-2026-10-07-A, and look at the plots. The 2024 lep samples
(W→ℓν 12, DY 7; PLAN D13) are the goal, but their production is decided after a look at the KNU storage use; until then the CR MC has no W/DY
(shapes first). (2) Next work, all four: jet-multiplicity diagnostics from the event tree (commit I), 2024 trigger SF (Stage 6), the 2024
b-tag method (Stage 7, D10), the 2018 v15 analyzer path (year comparison).
**Why.** The CRs need no new code and answer the question for Run 3 directly; the storage numbers decide whether the lep samples fit now.
**Open.** D13 (lep production) after the storage numbers; D10 after the BTV payload re-check.

## D-2026-10-08-A — 2024 b-tag SF: the fixed-WP method now (BTV method 1a with our L and M, our MC efficiencies, b jets only), the shape method once BTV's is confirmed; then the 2024 and 2018 control plots with trigger and b-tag SF against 2017 v9 · **DECIDED** (user, 2026-10-08)

**Fact (payload).** The jsonpog `POG/BTV/2024_Summer24` directory is unchanged since 2025-09-24 (KNU inventory 2026-10-08, PLAN §9.2): the
UParTAK4 WP values (`btagging.json.gz`), and one SF, `UParTAK4_kinfit` in `btagging_preliminary.json.gz` — fixed WP, **b jets only**
(flavour 5), |η| < 2.5 in one bin, pT 20–600 GeV in 8 bins, its uncertainty split into fsrdef, hdamp, isrdef, jer, jes, mass, statistic,
tune. No shape SF, no c or light SF. The Run 2 method (deepJet shape SF + b-tag norm reweight, AN-2022/122) cannot be carried over (PLAN
§9.6 D10). The control plots so far had neither trigger nor b-tag SF (D-2026-10-05-A, the `_notrig` outputs).
**Decision (user, 2026-10-08: "일단 작업 자체는 fixed WP로 진행하다가 shape correction 방법이 확인되면 그것으로 전환한다. 이 사항은 기록을 분명히
해 두어라").** (1) 2024 takes BTV's fixed-WP event weight, "method 1a", with the two WPs our selection uses — a b jet is score ≥ M
(`selectbJet`), a light jet for the hadronic W is score < L — so a selected jet is in one of three bins and its weight is P_Data / P_MC of
its bin: score ≥ M: SF_M; L ≤ score < M: (SF_L e_L − SF_M e_M) / (e_L − e_M); score < L: (1 − SF_L e_L) / (1 − e_L); the event weight is
the product (`computeBTagWeightFixedWP_`). SF from `UParTAK4_kinfit`; **c and light jets get SF 1** (weight exactly 1) until BTV gives
theirs; up/down = the payload's b-jet variation (its total, else its parts in quadrature). (2) e = **our MC efficiency** per process
group (tt, qcd, other, and all), hadron flavour, WP, pT (13 bins) and |η| (4 bins), measured at the HT step (every cut but the b-tag ones)
of the 2024 btagtrig MC (no trigger and no lepton requirement), the 4FS TTbb/TT4b left out as in the stack; a bin with fewer than 50
effective jets, or a b-jet bin where one of the three tag bins has fewer than 10 (an efficiency at or near 0 or 1 gives the jets on the
other side of a WP a weight up to 10⁴ in a sample weighted with these maps — the 10-08 review), or with sums made unphysical by negative
weights, takes its pT bin over |η|, then the 'all' maps
(`tools/stage7/btag_eff_maps.py`; JSON → yml `path_btag_eff_json` → env `TTHH_BTAGEFF_JSON`). (3) **No b-tag norm reweight in 2024**:
method 1a keeps the pre-tag normalization when the efficiencies fit; every MC job prints the check (Σ w·bTagWeight / Σ w at the HT step,
about 1). (4) **The switch**: when the 2024 shape method is confirmed (the user is reading the BTV documentation; a shape payload in
jsonpog), `EraConfig::btagMethod("2024")` becomes `Shape`, the shape SF and the norm reweight are derived for 2024 as for Run 2, and
this entry is superseded. (5) **The program it serves**: 2024 and 2018 control plots in FH, μCR and eCR with the trigger SF (and its
validation plots: efficiency curves Data vs MC, SF maps, closure) and the b-tag SF, compared with the 2017 v9 results; the control-plot
ratio is the first thing to look at.
**The user's expectation, and where it holds (AI, 2026-10-08).** "The Data/MC ratio will not change much because the SF is fixed-WP
rather than shape." Right for the yields per b-tag multiplicity and for the kinematic distributions: either method corrects the tag
rates at the WPs that define our categories (M for nb, L for the W jets), and method 1a needs no norm reweight. Not right, or not yet,
for (a) the b-tag score distributions (btag_1…4, every jet's score): the fixed-WP weight corrects only the three bins, inside a bin the MC
shape stays, while a shape SF corrects the whole distribution; (b) the mistag part: with nb ≥ 2 in the multijet-rich FH selection many
tagged jets are c or light (the 2024 HT1050 control region: Data/MC 1.46 sitting at intermediate scores, 10-07), and 2024 has no c/light
SF — neither method can correct that until BTV gives one; (c) the comparison with 2017 v9, which used the shape SF + norm reweight:
compare b-tag categories and kinematics, not score shapes; (d) the jet-multiplicity slope is not a b-tag effect.
**Alternatives considered.** Wait for a shape SF — rejected (user): the trigger SF and the control plots go first. Method 1a with M
only — rejected: the W jets are defined by L, so L–M jets are a category of their own. BTV's efficiencies — none for our phase space; ours
from MC is the method's recipe. Keep a norm reweight for 2024 — not needed for method 1a (the closure line watches it). One map per
sample — too few events in most samples; groups by process are the usual compromise, the 'all' maps the fallback.
**Supersede when.** BTV's 2024 shape method is confirmed (→ shape SF + norm reweight, point 4), or BTV adds c/light fixed-WP SFs (→ the
same weight takes them: `EraConfig::btagFixedWPPayload` loses `bJetsOnly`), or the closure in the SR/CR main runs is off by more than
1–2 % (→ maps from the main-run MC, whose BTagEff histograms are filled after the trigger and the lepton requirement).
**Record.** tempTTHH [`changes/STEP_25_btag_fixedWP_2024.md`](changes/STEP_25_btag_fixedWP_2024.md) (commit K: code, tests, commands).

## D-2026-10-10-A — 2024 fixed-WP b-tag weight after BTV's multi-WP guidance (cms-talk, 2026-10): the intermediate factor with a negative numerator becomes 1, closure per number of true b jets, the variants kept as comparison weights; c jets with the b SF only after BTV answers · **DECIDED** (user, 2026-10-10; implemented 10-10 in STEP 27 part N, [`changes/STEP_27_treev1_btv_rule.md`](changes/STEP_27_treev1_btv_rule.md))

**Fact (BTV material, read 10-09; copies on the Mac under `B-tag_SF_related_cms-talk/`).** (a) cms-talk "Corrections when using 3
Working Points" (BTV conveners, 2026-10-07): with several WPs every jet is in exactly one of the mutually exclusive regions (Option A;
N WPs → N−1 intermediate terms); an extra "L not T" term double counts. Our weight uses two WPs (L, M), so Option A and B do not differ
for it. (b) BTV wiki "Recommendations for fixedWP SFs": method 1a is the default; an intermediate factor whose numerator
SF_lo ε_lo − SF_hi ε_hi is negative (tighter SF above the looser one) is **set to unity**; c jets take the b-jet SF and its uncertainty;
efficiencies come from the analysis' MC per flavour, in pT bins like the SFs' and one to a few |η| bins, per process group, **after the
analysis selection without the b-tag requirements**; WP-binned information may be a BDT/DNN input. (c) BTV Calibration news (2026-04-20):
the example code (`btag_sf.py`: `<tagger>_comb` for b and c, `<tagger>_light`, any number of WPs); ttH(bb) Run 3 saw a large normalization
effect for tt+light at high N_b, possibly efficiencies of the ≥ 4-jet region extrapolated to high N_b — proposed fix: efficiencies as a
function of the number of true b jets; tt dilepton efficiencies differ by up to several % between ≥ 0 and ≥ 3 b jets. **Ours (commit K):**
a negative intermediate numerator gives the jet weight **0** (`max(0, …)`, the whole event weight 0); c and light SF 1; maps from the
btagtrig MC (no trigger, no lepton requirement). STEP_25 §3 said the efficiency sensitivity of an untagged jet's weight is "(1 − SF) times,
small"; it is (1 − SF)/(1 − ε)², not small for b jets failing L (1 − ε_L ≈ 0.05: at SF_L = 0.97, ε_L 0.95 → 0.96 moves the weight
1.57 → 1.72). The 2024 FH trigger (ParkingHH) requires PNet b tags online, so offline efficiencies after the trigger can be higher than in
btagtrig. Payload location: a CIEMAT framework issue (2025-10) moved all correction modules, BTV included, to
`/cvmfs/cms-griddata.cern.ch/cat/metadata`; our 10-08 inventory looked only at the jsonpog rsync path (2024_Summer24 unchanged since
2025-09-24) — a newer 2024 payload may exist there (to check).
**Decision (user, 2026-10-10: "5번에 정해 줄 것은 모두 네 권고대로 수용한다"; the comparison histograms "if not too complex or costly, else
later").** (1) An intermediate (L ≤ score < M) factor with a negative numerator becomes **1** (BTV); a negative fail-L numerator
(SF_L ε_L > 1) stays **0** (continuous in the SF; 1 would jump the up variation the wrong way) and goes to BTV as a question. (2) At the HT
step, Σw and Σw·bTagWeight per number of true b jets among the selected jets (hadronFlavour 5: 0, 1, 2, 3, ≥ 4) in `BTagEff/` — 1 per
bin when the maps fit; the deviation is the extrapolation bias BTV warns about. (3) c jets with the b SF: after the payload-location check
and BTV's answer (OPEN). (4) Comparison, cheap: the same jet loop also computes the old-rule weight and a "c jets with the b SF" weight —
two Tree branches and small HT-step histograms (the M-tag multiplicity, the closure of (2)) per variant; comparing the maps themselves
(btagtrig vs main-region maps, a true-b axis) waits for the first SF main run and recomputes the weight from the Tree (it has the jets'
pT, η, score, flavour). (5) The first SF main run uses the btagtrig maps; its `BTagEff/` (after the trigger and the lepton requirement,
BTV's region) makes main-region maps to compare.
**Alternatives considered.** Keep 0 for the intermediate factor — rejected (BTV's explicit rule). c jets with the b SF now — wait for the
payload check and BTV. Maps from the main region before any SF run — would need one more full main run.
**Supersede when.** BTV answers on c jets or the fail-L case; a newer 2024 payload (comb, light, or shape) is found; the true-b closure
shows a bias worth a true-b axis in the maps.
**Record.** This entry; the 2024 ML inputs (WP bins instead of continuous scores while only fixed-WP SFs exist) in
[`PLAN_ML_SYST.md`](PLAN_ML_SYST.md) §5.3; the implementation will be commit N.

## D-2026-10-10-B — The direction of the whole FH program (user, 2026-10-10): ML after the ttHH AN adapted to FH and kept close to Run 2; Tree branches for DNN and GATJA now, in parallel with the SF control plots; systematic and statistical uncertainties prepared now; a Korean analysis note grown with the work; QCD data-driven after the lepton CR agrees; SWAN for training on all of Run 2 + 2024; control plots for full Run 2 and 2024 · **DECIDED** (user, 2026-10-10)

**The user's words (2026-10-10, kept as written).** "1) FH DNN은 이전에 내가 거의 테스트 용도에 가깝게 사용한 것이다. SWAN project의 전체적인 구조를
참조하되 머신러닝 학습 방향은 ttHH AN을 기반으로 FH에 맞게 적용해야 한다. 그리고 ttHH AN또한 Run2 기준이므로 Run3에 필요한 것이 있으면 우리가 최대한
맞게 조정해야 한다. 단 Run2 내용과 너무 달라지면 안되며 억지로 변형할 바엔 Run2 기준에 맞추는게 적절함. 2) ntuple branch는 DNN과 GATJA 학습이
가능하도록 만들어야 한다. 필요한 경우 ntuplizer와 analyzer를 수정해야 한다. trigger SF, b-tag SF 및 reweight, ttbar stitching 등이 적용된 control
plot 작업을 하면서 동시에 이걸 준비하자. 3) 지금은 systematic uncertainty 및 statistical uncertainty 등이 거의 적용이 되지 않았을 것이다. 이들에 대한
작업 준비도 함께 진행되어야 한다. 4) 위 내용들을 기반으로 AN을 작성하면 좋을 것 같다. 당장엔 "내가" 이해할 수 있도록 한국어로 작성하면 좋겠음. 이것도
목표중 하나로 두라. 앞으론 md 문서들과 함께 AN을 만들면서 작업하면서 전체적 흐름과 변화, 이론 등을 모두 알 수 있으면 좋겠음 5) 맥 온라인이 되었다.
못 읽은 문서 및 자료들을 다 읽어보라. 그리고 업데이트도 하고. 6) QCD data driven도 준비해야 한다. 단 이건 lepton selection을 해서 적절한 data/MC
ratio가 나오면 하자. 일단 control plot이 먼저고 병렬적으로 위 사항들을 준비하자. 7) GPU는 SWAN을 이용하면 될 것 같으나 필요한 경우 KNU에 있는 GPU를
이용할 수도 있다. 일단 SWAN이 기본이다. 학습은 모든 Run2, 그리고 지금 만든 2024년 Run3가 기본이다. 8) control plot은 background가 모두 준비되면 full
Run2와 2024년 Run3를 모두 보고 싶다. 백그라운드 샘플이 아직 없으니 할 수는 없지만 적어도 stitching은 못하더라도 analyzer 준비는 가능할듯"

**Decision (user).** (1) The ML follows AN-2022/122 (SL/DL) adapted to FH, with the SWAN FH project only as a structural reference; a Run 3
change is made where Run 3 needs it, and where an adaptation would be forced the Run 2 (AN) definition is kept. (2) The analyzer's tree
carries what the FH DNN and GATJA need (ntuplizer changes only if a branch is missing from the v15 ntuples), prepared in parallel with the
control plots that have the trigger SF, the b-tag SF (and the reweight where it applies) and the ttbar stitching. (3) The systematic and
statistical uncertainties are prepared now. (4) A Korean analysis note is one of the goals; it is written together with the md docs as the
work goes. (5) Read the materials not yet read; update the docs. (6) The QCD multijet estimate from data is prepared, and done once the
lepton-selection control regions give a sensible Data/MC; control plots first. (7) Training on SWAN by default (the KNU GPU if needed), on
all of Run 2 and the 2024 production. (8) Control plots for full Run 2 and 2024 once every background exists; the analyzer is prepared for
those years now, even before stitching is possible.

**What the AI did with it (10-10; each item PROPOSED where it goes beyond the words above).**
- (2) Tree v1 = STEP 27 part O (code, tests): the AN Table 43 inputs that FH has, the χ² pairing with the AN's jet choice for nM ≥ 2, the jet
  ↔ hard-process quark labels (GATJA truth), PU / L1 / scale / PS weights, the Run 2 shape-SF source weights; PDF weights behind
  `--tree-pdf on`. The v15 branch lists of NtupleForge already keep everything this needs (`Jet_*`, `GenPart_*`, `LHEScaleWeight`,
  `LHEPdfWeight`, `PSWeight`, `L1PreFiringWeight_*`, `Pileup_*`; NtupleForge `branches/branch_hadronic_*_v15_MC.txt`), so **no ntuplizer
  change** is needed for Tree v1.
- (1), (7) the ML plan: [`PLAN_ML_SYST.md`](PLAN_ML_SYST.md) §9 (FH DNN after the SL DNN of AN §7.1 with a QCD class from the
  ttH(bb) FH training region, AN-19-094 §8.1; GATJA's FH version). **A dependency to know:** the Run 2 v15 signal (`TTHHTo4b`) and `TT4b`,
  `TTZHTo4b`, `TTZZTo4b`, `THW` do not exist centrally in NanoAODv15 (NtupleForge `docs/ttHH/04_mc_request_2026-09.md` §1: requested
  2026-09-14, no answer recorded; the enriched private production runs in parallel, D-2026-09-17-run2-v15-two-tracks) — "all of Run 2" as
  training data waits for them; 2024 has every sample centrally.
- (6) [`PLAN_QCD_DD.md`](PLAN_QCD_DD.md): the ttH(bb) FH method (AN-19-094 §8.1: b-tag regions TR/CR/SR × the m_qq window, the loose-jet
  kinematic correction, free normalisations) as the starting point for ttHH FH, with the prerequisite of (6).
- (4) [`AN_KR/`](AN_KR/) (LaTeX, XeLaTeX + kotex): the analysis note in Korean, its structure from AN-2022/122, and the FH and Run 3
  adaptations; built PDF in the Mac workspace `AN_KR_pdf/`.
- (8) the Run 2 v15 analyzer path (2016preVFP, 2016postVFP, 2017, 2018 with v15 names; the analyzer reads Run 2 with the v9 names today):
  the work list in [`ROADMAP_FH.md`](ROADMAP_FH.md) W2; 2018 v15 first (its ntuples exist), 2016/2017 v15 after their production.
- The whole order: [`ROADMAP_FH.md`](ROADMAP_FH.md).
**Alternatives considered.** Keep the SWAN FH DNN (V3, 13 classes, no b-tag inputs) as the baseline — rejected by the user (test level).
A new FH-only variable set — rejected: AN-based with the minimum Run 3 change. Wait for the control plots before adding branches — rejected:
one SF main run should give the plots and the first training sample.
**Supersede when.** The user changes any of the eight points; the Run 2 v15 signal question is answered (central or enriched).

## D-2026-10-10-C — ttbar stitching: normalise each dedicated sample over the categories it owns (the factor p_own is missing today), and settle who owns tt+b / tt+2b (genTtbarId 51, 52) · **PROPOSED** (AI, 2026-10-10; found while writing [`AN_KR/`](AN_KR/) ch. 2)

**Finding.** `compute_stitch_factors.py` `compute_plan` sets r = σ_inc·f/σ_ded with f = Σgenw_anchor(owned)/Σgenw_anchor(all), and
`build_analyzer_table` applies r to the **owned** categories of the dedicated sample only (0 elsewhere). The dedicated sample's base weight is
L·σ_ded/Σgenw_ded(**all**) (`Runs genEventSumw`), so the owned yield it gives is **L·σ_inc·f·p_own**, with p_own = Σgenw_ded(owned)/Σgenw_ded(all) —
not L·σ_inc·f, which is the stated target ("r rescales the dedicated sample to its inclusive anchor", "every (decay-channel × HF-category)
cell is filled once": the `compute_plan` docstring, [`ttbarCategorization_KR.md`](ttbarCategorization_KR.md) §10.3–10.4). The 2017 prescan
(`prescan_summary/prescan_summary.json`, genTtbarId sumGenW) gives p_own(53–55) = **0.114** (`TTbb_Hadronic`), **0.108** (`TTbb_SemiLep`),
**0.103** (`TTbb_DiLep`) — about 45 % of each 4FS sample is genTtbarId 0 and about 31 % is 51 (counts 2,545,506 and 1,774,721 of 5,694,656 in
`TTbb_Hadronic`). So the stitched tt+bb (53–55) is about **1/9** of the 5FS prediction it is meant to reproduce. For `TT4b` the owned codes
61/62/71/72 need the tt+nb lookup; the committed 2017 prescan has none of them, and the committed `stitch_factors_2017.json` has f_4b = r_4b = 0
(and the old lumi 41480 pb⁻¹, which does not enter r).
**Second point.** EXP leaves 51/52 (tt+b, tt+2b in the AN's naming) in the 5FS inclusive. AN-2022/122 takes tt+b, tt+2b and tt+bb from the 4FS
ttbb sample (App. D, PDF p.187/195/203: tt(SL) only tt+lf, tt+cc; ttbb(SL) only tt+b, tt+2b, tt+bb; tt4b only tt+bbb, tt+4b), as does the ttH(bb)
analysis (AN-19-094 v20 PDF p.89: tt+B from the 4FS sample, its yield matched to the 5FS tt+B, the whole tt normalised to 831.76 pb) and our
legacy options A/B and the Hbb talk (2026-07-30, p.21). No record says why EXP changed it.
**Where it matters now.** No main run applies stitching: `path_stitch_json: null` in the 2017 main/prescan and in all 2024 ymls
(D-2026-10-05-C). The 2017 **btagtrig** yml reads `stitch_factors_2017.json` (b-tag norm-reweight derivation); there r is a constant per
(sample, category), so a norm-RW ratio changes only through the mix inside a group (tt+B = 51/52 from 5FS + 53–55 from 4FS). The stitched
control plots of D-2026-10-10-B (2) wait for this.
**Proposal.** (a) r' = σ_inc·f/(σ_ded·p_own): the owned cell of each dedicated sample is normalised to the inclusive (5FS, 831.76 pb NNLO+NNLL)
prediction of that cell and takes its shape from the dedicated sample — the ttH(bb) prescription and the documented intent. (b) Ownership as
in the AN: 4FS ttbb_c owns 51–55 (minus the tt+nb codes), tt4b owns 61/62/71/72, the 5FS inclusive keeps LF and cc (the user decides between
this and keeping EXP's 51/52 in 5FS). (c) A generator-level closure test in the tool: for every (decay channel × category) cell the stitched
Σw equals the inclusive Σw, and the stitched total equals the inclusive total. (d) Then: the 2017 stitch JSON again (lumi 42.07, with the tt+nb
lookup), the 2017 b-tag norm-reweight JSON again with it, and 2024 stitching after the tt+nb patch (NtupleForge V6).
**Alternatives considered.** Normalise the 4FS sample with its own cross section (r = 1 on owned categories, 5FS owned categories rejected):
gives the 4FS prediction for tt+B (AN-19-094 Table 61: 4FS tt+B 21.34 pb vs 5FS 17.75 pb); the tt+B normalisation then rests on the 4FS
σ — possible, but it is not what our tool documents, and the FH-only fit cannot constrain tt+B (AN-19-094 PDF p.243: 50 % lnN). Keep the code
— rejected: it is not a choice but an error of a factor p_own.
**Supersede when.** The user picks (b); the closure test passes on the 2017 prescan.

## D-(historical) — carried invariants · **DECIDED**

- **No physics-logic change without Analysis-Note justification.** Rationale: results must be defensible against the AN.
- **tt+nb separation needs CMSSW-level work.** NanoAODv9 exposes only `genTtbarId Int_t`; full parity needs a custom producer (miniAOD Stage-1 matcher + FlatTable). NanoAOD-tools reconstruction tops out ~97%. Detail in `ttbarCategorization.md`.
- **Inclusive ttbar underestimates the high-b-multiplicity tail** → dedicated NLO 4FS ttbb/tt4b with proper stitching (drop inclusive `genTtbarId%100 ∈ {51–55}`, rescale ttbb). Detail in `ttbarCategorization.md` and `stitch_logs_2017/`.
