# Decision Log — ttHH(HH→bbbb) Fully-Hadronic Analysis

> **Purpose:** record each significant decision — the choice, why, what else was considered, and whether it still holds — so no one silently reopens a settled question or treats a proposal as settled.
> **Audience:** anyone about to change behavior or unsure whether something is fixed.
> **Status:** living, append-and-supersede · last meaningful update **2026-10-04** (D-2026-10-04-A: run records are committed) · 2026-10-02 (D-2026-10-02-A…E: v15 analyzer order, eventBuffer policy, event cleaning, answers N0–N8, blinding).
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
| N6 | Hide data in some plots? | "왜 저 경우에 data를 숨겨야 해?" | Answered in D-2026-10-02-E (PROPOSED) |
| N7 | b-tag norm reweight regions | "eCR region을 추가하자" | `TTHH_BTAGRW_REGION` gets `eCR` (nElecs == 1, nMuons == 0, MET > `metCutCR`); needs the btagtrig skim to collect electrons without a lead muon (today only with one) — PLAN §9 Stage 7 |
| N8 | 2024 b-tag SF method | "BTV 자료 확인하고 하자" | Read the 2024 BTV payload (PLAN §9 Stage 0) and the BTV documentation before choosing (PLAN §9.6 D10) |

## D-2026-10-02-E — Blinding: no data hidden in preselection or control-region plots; only the signal-sensitive bins of the final discriminant are blinded · **PROPOSED** (AI, 2026-10-02)

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

## D-(historical) — carried invariants · **DECIDED**

- **No physics-logic change without Analysis-Note justification.** Rationale: results must be defensible against the AN.
- **tt+nb separation needs CMSSW-level work.** NanoAODv9 exposes only `genTtbarId Int_t`; full parity needs a custom producer (miniAOD Stage-1 matcher + FlatTable). NanoAOD-tools reconstruction tops out ~97%. Detail in `ttbarCategorization.md`.
- **Inclusive ttbar underestimates the high-b-multiplicity tail** → dedicated NLO 4FS ttbb/tt4b with proper stitching (drop inclusive `genTtbarId%100 ∈ {51–55}`, rescale ttbb). Detail in `ttbarCategorization.md` and `stitch_logs_2017/`.
