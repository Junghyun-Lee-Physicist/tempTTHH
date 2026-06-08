#!/usr/bin/env python3
###############################################################################
# consolidate_prescan.py
# -----------------------------------------------------------------------------
# Post-processor for the ttHH(4b) FH analyzer `prescan` mode.
#
# The prescan mode writes ONE `prescan` TTree row per output ROOT file
# (one job = one input file). This script walks the prescan output area,
# sums every per-job row into a per-sample record, runs the same internal
# cross-checks the analyzer prints, and emits a consolidated summary as
# console tables + JSON (full) + CSV (headline).
#
# Output area layout (produced by submit_job_FH_Tier3_unified.py):
#   <input-base>/<sample>/<sample>_<count>.root      (count = 0,1,2,...)
#   e.g. AnalyzerOutput_prescan/TTToSemiLeptonic/TTToSemiLeptonic_140.root
#
# Each `prescan` row carries (see writePrescanTree() in the analyzer):
#   metadata : sampleName, runYear, dataEra, isData, xsec_used, nFiles
#   events   : nEvents_total, sumGenW_total, sumGenW_pos, sumGenW_neg
#   runs     : genEventSumw_runs, genEventSumw2_runs, genEventCount_runs
#   genTtbarId %% 100 (13 bins) : sumGenW_id_*  and  n_id_*
#   ntuple ttCat_*    (6 bins)  : sumGenW_ttCat_*  and  n_ttCat_*
#
# Usage:
#   cmsenv               # PyROOT must be importable
#   ./consolidate_prescan.py                       # all defaults
#   ./consolidate_prescan.py --input-base <dir> --outdir <dir>
#   ./consolidate_prescan.py --only TTToSemiLeptonic ttbb   # subset
#
# Run inside a CMSSW environment (needs PyROOT).
###############################################################################

from __future__ import annotations

import argparse
import csv
import json
import os
import re
import sys
import time
from dataclasses import dataclass, field
from decimal import Decimal

# --------------------------------------------------------------------------- #
# Configuration defaults (override on the command line)
# --------------------------------------------------------------------------- #
DEFAULT_INPUT_BASE = (
    "/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/AnalyzerOutput_prescan"
)
DEFAULT_OUTDIR = "./prescan_summary"
DEFAULT_TREE = "prescan"
DEFAULT_REL_TOL = 1e-6          # relative tolerance for float cross-checks

# Branch-key partitions (must mirror the analyzer's writePrescanTree()).
ID_KEYS = ["lt0", "0", "41", "42", "43", "44", "45",
           "51", "52", "53", "54", "55", "other"]
TTCAT_KEYS = ["LightFlavour", "AddCjet", "Add1Bjet_1Had",
              "Add1Bjet_2Had", "Add2Bjet", "NoTT"]

# genTtbarId-group -> ntuple ttCat counterpart, for the ttCat-vs-id check.
# (53/54/55 collapse into Add2Bjet; NoTT has no genTtbarId counterpart.)
TTCAT_VS_ID = {
    "LightFlavour":  ["0"],
    "AddCjet":       ["41", "42", "43", "44", "45"],
    "Add1Bjet_1Had": ["51"],
    "Add1Bjet_2Had": ["52"],
    "Add2Bjet":      ["53", "54", "55"],
}

_FNAME_INDEX_RE = re.compile(r"_(\d+)\.root$")


# --------------------------------------------------------------------------- #
# Number formatting — every value is rendered in plain fixed-point decimal,
# never scientific notation, for console / JSON / CSV alike.
# --------------------------------------------------------------------------- #
def fixed_str(x) -> str:
    """Plain fixed-point decimal string for a number.

    Never uses scientific notation. For floats it takes repr()'s shortest
    round-trip form and, only if that carries an exponent, expands it
    exactly via Decimal — so no precision is lost and no noise digits are
    introduced. Integers and bools are returned verbatim.
    """
    if isinstance(x, bool):
        return str(x)
    if isinstance(x, int):
        return str(x)
    s = repr(float(x))
    if "e" in s or "E" in s:
        s = format(Decimal(s), "f")     # exact expansion, fixed-point
    return s


def grouped(x) -> str:
    """fixed_str() with thousands separators on the integer part.

    Used for console tables only; JSON/CSV use fixed_str() without
    separators so the values stay machine-parseable.
    """
    s = fixed_str(x)
    neg = s.startswith("-")
    if neg:
        s = s[1:]
    if "." in s:
        int_part, frac_part = s.split(".", 1)
        out = f"{int(int_part):,}.{frac_part}"
    else:
        out = f"{int(s):,}"
    return ("-" + out) if neg else out


# --------------------------------------------------------------------------- #
# Data model
# --------------------------------------------------------------------------- #
@dataclass
class SampleSummary:
    """Aggregated prescan record for a single sample."""
    sample: str
    is_data: bool = False
    run_year: str = ""
    data_era: str = ""
    xsec_used: float = 0.0          # analyzer base weight (per-sample constant)

    files_found: int = 0
    files_valid: int = 0
    bad_files: list = field(default_factory=list)       # unreadable / wrong row count
    missing_indices: list = field(default_factory=list)  # gaps in 0..max(index)

    n_input_files: int = 0          # Σ of the `nFiles` branch
    n_events: int = 0
    sumGenW: float = 0.0
    sumGenW_pos: float = 0.0
    sumGenW_neg: float = 0.0

    runs_sumW: float = 0.0
    runs_sumW2: float = 0.0
    runs_count: int = 0

    id_n: dict = field(default_factory=lambda: {k: 0 for k in ID_KEYS})
    id_w: dict = field(default_factory=lambda: {k: 0.0 for k in ID_KEYS})
    ttcat_n: dict = field(default_factory=lambda: {k: 0 for k in TTCAT_KEYS})
    ttcat_w: dict = field(default_factory=lambda: {k: 0.0 for k in TTCAT_KEYS})

    warnings: list = field(default_factory=list)

    # ---- derived quantities -------------------------------------------------
    @property
    def skim_attrition_rel(self) -> float | None:
        """(Events - Runs) / Runs. None for data / when the Runs sum is 0."""
        if self.is_data or self.runs_sumW == 0.0:
            return None
        return (self.sumGenW - self.runs_sumW) / self.runs_sumW

    @property
    def eff_yield(self) -> float:
        """xsec_used * Σgenw(Runs) — weighted yield at the analyzer's target
        normalization (equals Xsec*Lumi when xsec_used = Xsec*Lumi/Σgenw)."""
        return self.xsec_used * self.runs_sumW


# --------------------------------------------------------------------------- #
# ROOT I/O
# --------------------------------------------------------------------------- #
def import_root():
    try:
        import ROOT  # noqa
    except Exception as exc:
        sys.exit(f"[FATAL] PyROOT import failed ({exc}).\n"
                 f"        Run this script inside a CMSSW environment (cmsenv).")
    ROOT.gROOT.SetBatch(True)
    ROOT.gErrorIgnoreLevel = ROOT.kError
    return ROOT


# std::string TTree branches. These are intentionally NOT read: PyROOT
# attribute/auto-binding access to std::string branches is a known
# segmentation-fault hazard. The sample identity is recovered from the
# directory name instead, and runYear/dataEra are non-essential here.
STRING_BRANCHES = {"sampleName", "runYear", "dataEra"}


def read_prescan_row(ROOT, path: str, tree_name: str):
    """Open one prescan ROOT file and return its single row as a dict.

    Only numeric scalar branches are read, each via TLeaf::GetValue()
    (proxy-free — the value is copied immediately, so nothing dangles once
    the file is closed). std::string branches are skipped (see above).

    Returns None when the file is missing, a zombie, or does not carry
    exactly one row in `tree_name` (i.e. a crashed / incomplete job).
    """
    if not os.path.isfile(path):
        return None
    f = ROOT.TFile.Open(path, "READ")
    if not f or f.IsZombie():
        if f:
            f.Close()
        return None
    try:
        tree = f.Get(tree_name)
        if not tree or not tree.InheritsFrom("TTree") or tree.GetEntries() != 1:
            return None
        present = {b.GetName() for b in tree.GetListOfBranches()}
        tree.GetEntry(0)
        # Numeric scalars only. GetLeaf(...).GetValue() returns a plain
        # Python float for every /D /I /L /O leaf; older files lacking the
        # n_id_* extension simply contribute fewer keys (reported as absent).
        row: dict = {"_branches": present}
        for name in present:
            if name in STRING_BRANCHES:
                continue
            leaf = tree.GetLeaf(name)
            if leaf:
                row[name] = leaf.GetValue()
        return row
    finally:
        f.Close()


# --------------------------------------------------------------------------- #
# Aggregation
# --------------------------------------------------------------------------- #
def discover_samples(input_base: str, only: list[str] | None) -> list[str]:
    if not os.path.isdir(input_base):
        sys.exit(f"[FATAL] input base directory not found: {input_base}")
    samples = sorted(d for d in os.listdir(input_base)
                     if os.path.isdir(os.path.join(input_base, d)))
    if only:
        wanted = set(only)
        samples = [s for s in samples if s in wanted]
        for miss in wanted.difference(samples):
            print(f"[WARN] requested sample not found under input base: {miss}")
    return samples


def aggregate_sample(ROOT, input_base: str, sample: str,
                     tree_name: str) -> SampleSummary:
    """Sum every per-job prescan row of one sample into a SampleSummary."""
    summ = SampleSummary(sample=sample)
    sdir = os.path.join(input_base, sample)
    root_files = sorted(fn for fn in os.listdir(sdir) if fn.endswith(".root"))
    summ.files_found = len(root_files)

    seen_indices: list[int] = []
    meta_locked = False

    for fn in root_files:
        m = _FNAME_INDEX_RE.search(fn)
        if m:
            seen_indices.append(int(m.group(1)))

        row = read_prescan_row(ROOT, os.path.join(sdir, fn), tree_name)
        if row is None:
            summ.bad_files.append(fn)
            continue
        summ.files_valid += 1

        # Metadata: taken once; flag intra-sample inconsistency.
        # runYear / dataEra come from std::string branches which are not
        # read (see STRING_BRANCHES); they stay at their dataclass default.
        if not meta_locked:
            summ.is_data = bool(row.get("isData", 0.0))
            summ.xsec_used = float(row.get("xsec_used", 0.0))
            meta_locked = True
        elif bool(row.get("isData", 0.0)) != summ.is_data:
            summ.warnings.append(f"isData inconsistent across files ({fn})")

        # Scalar sums.
        summ.n_input_files += int(row.get("nFiles", 0))
        summ.n_events += int(row.get("nEvents_total", 0))
        summ.sumGenW += float(row.get("sumGenW_total", 0.0))
        summ.sumGenW_pos += float(row.get("sumGenW_pos", 0.0))
        summ.sumGenW_neg += float(row.get("sumGenW_neg", 0.0))
        summ.runs_sumW += float(row.get("genEventSumw_runs", 0.0))
        summ.runs_sumW2 += float(row.get("genEventSumw2_runs", 0.0))
        summ.runs_count += int(row.get("genEventCount_runs", 0))

        # genTtbarId 13-bin partition.
        for k in ID_KEYS:
            summ.id_w[k] += float(row.get(f"sumGenW_id_{k}", 0.0))
            summ.id_n[k] += int(row.get(f"n_id_{k}", 0))
        # ntuple ttCat 6-bin partition.
        for k in TTCAT_KEYS:
            summ.ttcat_w[k] += float(row.get(f"sumGenW_ttCat_{k}", 0.0))
            summ.ttcat_n[k] += int(row.get(f"n_ttCat_{k}", 0))

    # Gap detection: missing job indices in [0, max].
    if seen_indices:
        full = set(range(max(seen_indices) + 1))
        summ.missing_indices = sorted(full.difference(seen_indices))

    return summ


# --------------------------------------------------------------------------- #
# Cross-checks (same logic the analyzer prints, now sample-aggregated)
# --------------------------------------------------------------------------- #
def run_crosschecks(summ: SampleSummary, rel_tol: float) -> dict:
    """Return a dict of named pass/fail checks; append human-readable
    messages to summ.warnings for any failure."""
    checks: dict[str, bool] = {}

    def close(a: float, b: float) -> bool:
        scale = max(abs(a), abs(b), 1.0)
        return abs(a - b) <= rel_tol * scale

    # Data carries no genWeight / Runs / genTtbarId structure -> skip.
    if summ.is_data:
        return checks

    # (1) genTtbarId partition is complete: Σ over bins == totals.
    id_n_sum = sum(summ.id_n.values())
    id_w_sum = sum(summ.id_w.values())
    checks["id_partition_count"] = (id_n_sum == summ.n_events)
    checks["id_partition_weight"] = close(id_w_sum, summ.sumGenW)
    if not checks["id_partition_count"]:
        summ.warnings.append(
            f"genTtbarId count partition mismatch: Σn_id={id_n_sum:,} "
            f"vs nEvents={summ.n_events:,}")
    if not checks["id_partition_weight"]:
        summ.warnings.append(
            f"genTtbarId weight partition mismatch: "
            f"ΣsumGenW_id={fixed_str(id_w_sum)} "
            f"vs sumGenW={fixed_str(summ.sumGenW)}")

    # (2) ntuple ttCat_* vs analyzer genTtbarId decode (count + weight).
    for cat, id_group in TTCAT_VS_ID.items():
        n_id = sum(summ.id_n[k] for k in id_group)
        w_id = sum(summ.id_w[k] for k in id_group)
        ok_n = (summ.ttcat_n[cat] == n_id)
        ok_w = close(summ.ttcat_w[cat], w_id)
        checks[f"ttcat_vs_id_count[{cat}]"] = ok_n
        checks[f"ttcat_vs_id_weight[{cat}]"] = ok_w
        if not ok_n:
            summ.warnings.append(
                f"ttCat vs genTtbarId count mismatch [{cat}]: "
                f"n_ttCat={summ.ttcat_n[cat]:,} vs Σn_id={n_id:,}")
        if not ok_w:
            summ.warnings.append(
                f"ttCat vs genTtbarId weight mismatch [{cat}]: "
                f"w_ttCat={fixed_str(summ.ttcat_w[cat])} "
                f"vs Σw_id={fixed_str(w_id)}")

    # (3) Events-tree vs Runs-tree sum (informational; non-zero = skim).
    checks["events_vs_runs_match"] = close(summ.sumGenW, summ.runs_sumW)

    return checks


# --------------------------------------------------------------------------- #
# Reporting
# --------------------------------------------------------------------------- #
def print_headline_table(records: list[tuple[SampleSummary, dict]]) -> None:
    hdr = (f"{'Sample':<24}{'Type':<6}{'Jobs ok/found':<16}"
           f"{'nEvents':>18}{'ΣgenW(Events)':>28}{'ΣgenW(Runs)':>28}"
           f"{'skim%':>10}{'chk':>6}")
    line = "─" * len(hdr)
    print("\n" + line)
    print("  PRESCAN CONSOLIDATED SUMMARY")
    print(line)
    print(hdr)
    print(line)
    for summ, checks in records:
        skim = summ.skim_attrition_rel
        skim_s = "  —" if skim is None else f"{skim * 100:+.4f}"
        chk = "OK" if all(checks.values()) else "FAIL"
        jobs = f"{summ.files_valid}/{summ.files_found}"
        print(f"{summ.sample:<24}{'Data' if summ.is_data else 'MC':<6}"
              f"{jobs:<16}{grouped(summ.n_events):>18}"
              f"{grouped(summ.sumGenW):>28}{grouped(summ.runs_sumW):>28}"
              f"{skim_s:>10}{chk:>6}")
    print(line)


def print_ttbar_category_table(records: list[tuple[SampleSummary, dict]]) -> None:
    """Per-sample genTtbarId raw event counts — the stitching-partition input.
    Only samples with ttbar content (Σ n_id > 0) are listed."""
    cc = lambda s: s.id_n["41"] + s.id_n["42"] + s.id_n["43"] + s.id_n["44"] + s.id_n["45"]
    rows = [s for s, _ in records if sum(s.id_n.values()) > 0]
    if not rows:
        return
    hdr = (f"{'Sample':<24}{'LF(0)':>12}{'cc(41-45)':>12}{'b(51)':>12}"
           f"{'b(52)':>12}{'2b(53)':>12}{'bb(54)':>12}{'4b(55)':>12}"
           f"{'lt0':>10}{'other':>9}")
    line = "─" * len(hdr)
    print("\n" + line)
    print("  genTtbarId %% 100 — RAW EVENT COUNTS (stitching partition)")
    print(line)
    print(hdr)
    print(line)
    for s in rows:
        print(f"{s.sample:<24}{grouped(s.id_n['0']):>12}{grouped(cc(s)):>12}"
              f"{grouped(s.id_n['51']):>12}{grouped(s.id_n['52']):>12}"
              f"{grouped(s.id_n['53']):>12}{grouped(s.id_n['54']):>12}"
              f"{grouped(s.id_n['55']):>12}{grouped(s.id_n['lt0']):>10}"
              f"{grouped(s.id_n['other']):>9}")
    print(line)


def print_anomaly_report(records: list[tuple[SampleSummary, dict]]) -> None:
    flagged = [(s, c) for s, c in records
               if s.bad_files or s.missing_indices or s.warnings]
    if not flagged:
        print("\n  No anomalies: every job valid, every cross-check passed.\n")
        return
    print("\n" + "═" * 78)
    print("  ANOMALY REPORT — review before using these numbers downstream")
    print("═" * 78)
    for s, _ in flagged:
        print(f"\n  [{s.sample}]")
        if s.missing_indices:
            preview = s.missing_indices[:20]
            more = "" if len(s.missing_indices) <= 20 else \
                   f" ... (+{len(s.missing_indices) - 20} more)"
            print(f"    missing job indices ({len(s.missing_indices)}): "
                  f"{preview}{more}")
        if s.bad_files:
            print(f"    unreadable / incomplete files ({len(s.bad_files)}):")
            for bf in s.bad_files[:20]:
                print(f"      - {bf}")
            if len(s.bad_files) > 20:
                print(f"      ... (+{len(s.bad_files) - 20} more)")
        for w in s.warnings:
            print(f"    cross-check: {w}")
    print("═" * 78 + "\n")


# --------------------------------------------------------------------------- #
# Serialization
# --------------------------------------------------------------------------- #
def summary_to_dict(summ: SampleSummary, checks: dict) -> dict:
    return {
        "sample": summ.sample,
        "isData": summ.is_data,
        "runYear": summ.run_year,
        "dataEra": summ.data_era,
        "xsec_used": summ.xsec_used,
        "jobs": {
            "files_found": summ.files_found,
            "files_valid": summ.files_valid,
            "files_bad": len(summ.bad_files),
            "bad_files": summ.bad_files,
            "missing_indices": summ.missing_indices,
        },
        "events": {
            "n_input_files": summ.n_input_files,
            "nEvents_total": summ.n_events,
            "sumGenW_total": summ.sumGenW,
            "sumGenW_pos": summ.sumGenW_pos,
            "sumGenW_neg": summ.sumGenW_neg,
        },
        "runs": {
            "genEventSumw": summ.runs_sumW,
            "genEventSumw2": summ.runs_sumW2,
            "genEventCount": summ.runs_count,
        },
        "skim_attrition_rel": summ.skim_attrition_rel,
        "genTtbarId": {"n": summ.id_n, "sumGenW": summ.id_w},
        "ttCat": {"n": summ.ttcat_n, "sumGenW": summ.ttcat_w},
        "derived": {"eff_yield": summ.eff_yield},
        "crosschecks": checks,
        "crosschecks_all_ok": all(checks.values()),
        "warnings": summ.warnings,
    }


# Sentinel wrapping each float so it can be re-emitted as a bare, plain-
# decimal JSON number after json.dumps(). '@' is never escaped by the JSON
# encoder and the tag cannot collide with any string value in the payload.
_FLOAT_TAG = "@@FLOAT@@"


def _tag_floats(obj):
    """Recursively wrap every float in _FLOAT_TAG markers (ints/bools/str
    untouched) so json.dumps emits them as tagged strings."""
    if isinstance(obj, bool):
        return obj
    if isinstance(obj, float):
        return f"{_FLOAT_TAG}{fixed_str(obj)}{_FLOAT_TAG}"
    if isinstance(obj, dict):
        return {k: _tag_floats(v) for k, v in obj.items()}
    if isinstance(obj, list):
        return [_tag_floats(v) for v in obj]
    return obj


def write_json(path: str, records: list[tuple[SampleSummary, dict]],
               meta: dict) -> None:
    payload = {
        "meta": meta,
        "samples": {s.sample: summary_to_dict(s, c) for s, c in records},
    }
    text = json.dumps(_tag_floats(payload), indent=2)
    # Drop the quotes + tags around each tagged float so it lands in the
    # file as a bare JSON number in plain-decimal (non-scientific) form.
    text = re.sub(rf'"{_FLOAT_TAG}(-?[0-9.]+){_FLOAT_TAG}"', r"\1", text)
    with open(path, "w") as fh:
        fh.write(text + "\n")


def _csv_cell(v):
    """Render one CSV cell: floats as plain fixed-point decimals (no
    scientific notation); ints, strings and bools pass through unchanged."""
    return fixed_str(v) if isinstance(v, float) else v


def write_csv(path: str, records: list[tuple[SampleSummary, dict]]) -> None:
    cols = ["sample", "type", "files_valid", "files_found",
            "nEvents_total", "sumGenW_Events", "sumGenW_Runs",
            "sumGenW2_Runs", "genEventCount_Runs", "skim_attrition_rel",
            "n_id_LF", "n_id_cc", "n_id_b51", "n_id_b52",
            "n_id_2b53", "n_id_bb54", "n_id_4b55", "n_id_lt0", "n_id_other",
            "xsec_used", "eff_yield"]
    with open(path, "w", newline="") as fh:
        w = csv.writer(fh)
        w.writerow(cols)
        for s, _ in records:
            cc = sum(s.id_n[k] for k in ("41", "42", "43", "44", "45"))
            skim = s.skim_attrition_rel
            row = [
                s.sample, "Data" if s.is_data else "MC",
                s.files_valid, s.files_found,
                s.n_events, s.sumGenW, s.runs_sumW, s.runs_sumW2,
                s.runs_count, "" if skim is None else skim,
                s.id_n["0"], cc, s.id_n["51"], s.id_n["52"],
                s.id_n["53"], s.id_n["54"], s.id_n["55"],
                s.id_n["lt0"], s.id_n["other"],
                s.xsec_used, s.eff_yield,
            ]
            w.writerow([_csv_cell(c) for c in row])


# --------------------------------------------------------------------------- #
# Entry point
# --------------------------------------------------------------------------- #
def parse_args(argv=None) -> argparse.Namespace:
    p = argparse.ArgumentParser(
        description="Consolidate ttHH FH analyzer prescan-mode outputs.")
    p.add_argument("--input-base", default=DEFAULT_INPUT_BASE,
                   help="prescan output base dir (default: %(default)s)")
    p.add_argument("--outdir", default=DEFAULT_OUTDIR,
                   help="directory for summary JSON/CSV (default: %(default)s)")
    p.add_argument("--tree", default=DEFAULT_TREE,
                   help="TTree name inside each output file (default: %(default)s)")
    p.add_argument("--rel-tol", type=float, default=DEFAULT_REL_TOL,
                   help="relative tolerance for float cross-checks "
                        "(default: %(default)s)")
    p.add_argument("--only", nargs="+", metavar="SAMPLE",
                   help="restrict to these sample directories")
    return p.parse_args(argv)


def main(argv=None) -> int:
    args = parse_args(argv)
    ROOT = import_root()

    samples = discover_samples(args.input_base, args.only)
    if not samples:
        sys.exit("[FATAL] no sample directories to process.")
    print(f"[prescan] input base : {args.input_base}")
    print(f"[prescan] samples    : {len(samples)}")

    records: list[tuple[SampleSummary, dict]] = []
    for i, sample in enumerate(samples, 1):
        print(f"  [{i:3d}/{len(samples)}] {sample} ...", flush=True)
        summ = aggregate_sample(ROOT, args.input_base, sample, args.tree)
        checks = run_crosschecks(summ, args.rel_tol)
        records.append((summ, checks))

    print_headline_table(records)
    print_ttbar_category_table(records)
    print_anomaly_report(records)

    os.makedirs(args.outdir, exist_ok=True)
    json_path = os.path.join(args.outdir, "prescan_summary.json")
    csv_path = os.path.join(args.outdir, "prescan_summary.csv")
    meta = {
        "generated": time.strftime("%Y-%m-%d %H:%M:%S"),
        "input_base": args.input_base,
        "tree": args.tree,
        "rel_tol": args.rel_tol,
        "n_samples": len(records),
        "n_samples_with_anomaly": sum(
            1 for s, _ in records
            if s.bad_files or s.missing_indices or s.warnings),
    }
    write_json(json_path, records, meta)
    write_csv(csv_path, records)
    print(f"  Wrote {json_path}")
    print(f"  Wrote {csv_path}\n")

    # Non-zero exit if any sample has a bad/missing job or a failed check —
    # convenient for chaining (e.g. trigger a resubmit pass).
    any_anomaly = any(s.bad_files or s.missing_indices
                      or not all(c.values())
                      for s, c in records)
    return 1 if any_anomaly else 0


if __name__ == "__main__":
    sys.exit(main())
