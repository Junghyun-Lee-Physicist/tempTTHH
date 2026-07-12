#!/usr/bin/env python3
"""
================================================================================
SCRIPT: scenario_runner.py
================================================================================

[ 1. Purpose ]
Wraps the existing single-scenario tooling (create_sample_config.py +
extract_structure.py + stack_plotter.C) so that all 12 validation
scenarios produced by the new condor pipeline can be plotted in one go.

[ 2. Input layout produced by submit_job_FH_Tier3_unified.py ]
    <output_base>/
    ├── baseline/
    │   ├── TTbar_Hadronic/
    │   │   ├── TTbar_Hadronic_0.root
    │   │   ├── TTbar_Hadronic_1.root
    │   │   └── ...
    │   ├── ttHH/...
    │   └── ...37 sample dirs total
    ├── bcut0/
    │   └── ...same 37 sample dirs
    └── ...12 scenarios total

[ 3. What this script does ]
For each scenario:
    (a) build a samples_config_<scen>.yml that lists EVERY split ROOT file
        of every sample in that scenario as a separate sample entry's
        `files:` list (plotter sums them on the fly via GetSummedHist).
    (b) Reuse a single structure_info.yml (we extract it ONCE from a
        representative file in the FIRST scenario, since the histogram
        layout does not depend on the scenario — only the bin contents
        do).
    (c) Create a per-scenario working directory:
            <output_base>/plot_workdir/<scen>/
        copy the two YAMLs into it under the standard names that
        stack_plotter.C expects (`samples_config.yml`,
        `structure_info.yml`).
    (d) Invoke `root -l -b -q stack_plotter.C` from inside the working
        dir. The macro writes plots/ next to the YAMLs, so each
        scenario's plots end up at <output_base>/plot_workdir/<scen>/plots/.

NO existing file is modified. The wrapper imports the helper functions
from the user's existing scripts when possible; for the ROOT macro
(create_sample_config.py is Python so we import; stack_plotter.C is
ROOT C++ so we shell out).

[ 4. Why we don't hadd ]
stack_plotter.C's GetSummedHist accepts a list of files per sample and
sums them on the fly. Skipping hadd:
    - saves disk I/O (writes 12x duplicate copies otherwise),
    - saves wall time,
    - avoids hadd-related corruption / silent truncation risk.
The trade-off is per-plot ROOT file open count goes up; for a typical
sample with ~10 splits this is negligible.

[ 5. Usage ]
    python3 scenario_runner.py \
        --base /pnfs/.../AnalyzerOutput_validation \
        --plotter /path/to/stack_plotter.C \
        --workdir /tmp/ttHH_plotwork                # optional
        --scenarios baseline bcut0                  # optional, default: ALL
        --jobs 4                                    # optional, default: 1

[ 6. Notes ]
- This wrapper assumes the directory layout under <output_base> matches
  the convention produced by submit_job_FH_Tier3_unified.py exactly.
  Mis-named scenarios are reported and skipped (not fatal).
- The structure_info.yml is reused across scenarios. If one scenario
  produces a histogram set that differs from another (extremely
  unlikely given that the analyzer code is identical between
  scenarios), regenerate per-scenario by passing
  `--structure-per-scenario`.
- All paths in the generated samples_config_<scen>.yml are absolute, so
  ROOT can read them from any cwd.
================================================================================
"""

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

# -----------------------------------------------------------------------------
# Sample classification (mirrors the existing create_sample_config.py logic
# so we get matching DATA/MC tags and labels). Kept inline here so this
# wrapper stays self-contained even if the user's helper scripts are not
# importable from the working directory.
# -----------------------------------------------------------------------------
DATA_KEYWORDS = (
    "Data", "SingleMuon", "DoubleMuon", "JetHT", "BTagCSV",
    "EGamma", "MuonEG", "DoubleEG"
)


def classify_process(sample_name: str) -> str:
    """Return 'DATA' or 'MC' from the sample directory name."""
    for kw in DATA_KEYWORDS:
        if kw in sample_name:
            return "DATA"
    return "MC"


# -----------------------------------------------------------------------------
# YAML emission. We hand-write rather than depend on PyYAML so the wrapper
# runs on bare CMSSW environments without extra pip installs.
# -----------------------------------------------------------------------------
def emit_samples_yaml(samples: dict, out_path: Path) -> None:
    """Write a samples_config.yml in the format ParseSampleConfig expects.

    Format (matches stack_plotter.C parser at line 100-130):
        samples:
          ttHH:
            type: MC
            path: /abs/path/to/scenario/ttHH
            files:
              - /abs/path/to/scenario/ttHH/ttHH_0.root
              - /abs/path/to/scenario/ttHH/ttHH_1.root
              ...
            label: ttHH
            color: '#332288'
    """
    lines = []
    lines.append("description: 'Auto-generated by scenario_runner.py'")
    lines.append("samples:")
    for name, info in samples.items():
        lines.append(f"  {name}:")
        lines.append(f"    type: {info['type']}")
        lines.append(f"    path: {info['path']}")
        lines.append("    files:")
        for f in info["files"]:
            lines.append(f"      - {f}")
        lines.append(f"    label: {info['label']}")
        lines.append(f"    color: '{info['color']}'")
    out_path.write_text("\n".join(lines) + "\n")


# -----------------------------------------------------------------------------
# Color palette (mirrors create_sample_config.py; the plotter actually uses
# GetSmartColor() which ignores this field at the moment, but we emit it for
# future compatibility once GetSmartColor is retired).
# -----------------------------------------------------------------------------
COLOR_PALETTE = (
    "#332288", "#88CCEE", "#44AA99", "#117733", "#999933",
    "#DDCC77", "#CC6677", "#882255", "#AA4499", "#DDDDDD",
)


def color_for(idx: int, is_data: bool) -> str:
    if is_data:
        return "#000000"
    return COLOR_PALETTE[idx % len(COLOR_PALETTE)]


# -----------------------------------------------------------------------------
# Core: scan one scenario directory and return the samples dict
# -----------------------------------------------------------------------------
def scan_scenario(scenario_dir: Path) -> dict:
    """Return {sample_name -> {type, path, files, label, color}} for one scenario."""
    samples: dict = {}
    sample_dirs = sorted([d for d in scenario_dir.iterdir() if d.is_dir()])
    color_idx = 0
    for sdir in sample_dirs:
        files = sorted(str(p) for p in sdir.glob("*.root"))
        if not files:
            print(f"    [warn] {sdir.name}: no .root files, skipping")
            continue
        proc_type = classify_process(sdir.name)
        is_data = (proc_type == "DATA")
        samples[sdir.name] = {
            "type":  proc_type,
            "path":  str(sdir),
            "files": files,
            "label": "Data" if is_data else sdir.name,
            "color": color_for(color_idx, is_data),
        }
        color_idx += 1
    return samples


# -----------------------------------------------------------------------------
# structure_info.yml extraction.
#
# Strategy:
#   (1) If the user's extract_structure.py is importable, invoke it as a
#       subprocess so its filtering/CLI flags govern the result. This keeps
#       the scenario_runner from re-implementing the policy (the policy
#       belongs in extract_structure.py per single-source-of-truth).
#   (2) Otherwise fall back to a minimal inline extractor that emits the
#       same flat yaml shape.
# -----------------------------------------------------------------------------
def extract_structure_yaml(reference_root_file: Path,
                           out_path: Path,
                           helper_dir: Path | None = None) -> bool:
    """
    Produce a structure_info.yml describing the plottable histograms inside
    the given ROOT file. Returns True on success, False on failure.
    """
    # ── (1) Prefer the user's helper script ───────────────────────────────
    if helper_dir and (helper_dir / "extract_structure.py").exists():
        helper_script = helper_dir / "extract_structure.py"
        cmd = [sys.executable, str(helper_script),
               "--input",  str(reference_root_file),
               "--output", str(out_path)]
        print(f"    invoking helper: {' '.join(cmd)}")
        try:
            proc = subprocess.run(
                cmd,
                stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                text=True, check=False
            )
            if proc.returncode == 0 and out_path.exists():
                return True
            print(f"    [warn] helper exited {proc.returncode}; tail:")
            for ln in proc.stdout.splitlines()[-10:]:
                print(f"      {ln}")
        except Exception as e:
            print(f"    [warn] failed to invoke helper: {e}")

    # ── (2) Fallback: inline minimal extractor ────────────────────────────
    print("    using inline structure extractor (fallback)")
    try:
        import uproot
    except ImportError:
        print("    [error] uproot not installed: pip install uproot")
        return False

    # Mirror the policy in extract_structure.py: keep TH1*, skip everything
    # structural / non-stackable.
    ALWAYS_SKIP = ("TTree", "TBranch", "TLeaf", "TNtuple",
                   "TDirectoryFile", "TDirectory", "TList", "TKey",
                   "TGraph")
    KEEP_1D = ("TH1F", "TH1D", "TH1S", "TH1I", "TH1C", "TH1")

    def is_keep(cls: str) -> bool:
        if any(cls.startswith(p) for p in ALWAYS_SKIP):
            return False
        return any(cls.startswith(p) for p in KEEP_1D)

    items: list[dict] = []

    def walk(node, prefix=""):
        for key in node.keys(cycle=False):
            clean = key.split(";")[0]
            try:
                obj = node[clean]
            except Exception:
                continue
            cls = getattr(obj, "classname", obj.__class__.__name__)
            if cls.startswith(("TDirectoryFile", "TDirectory")):
                walk(obj, prefix + clean + "/")
                continue
            if not is_keep(cls):
                continue
            title = ""
            try:
                title = obj.title or ""
            except Exception:
                pass
            nbins = xlow = xhigh = None
            try:
                edges = obj.axis(0).edges()
                nbins = len(edges) - 1
                xlow = float(edges[0]); xhigh = float(edges[-1])
            except Exception:
                pass
            items.append({
                "key_path": (prefix + clean).lstrip("/"),
                "classname": cls,
                "title": title,
                "nbins": nbins,
                "xlow": xlow,
                "xhigh": xhigh,
            })

    with uproot.open(str(reference_root_file)) as f:
        walk(f)

    yaml_text = _flat_struct_to_yaml(items, reference_root_file)
    out_path.write_text(yaml_text)
    return True


def _flat_struct_to_yaml(items: list[dict], ref_file: Path) -> str:
    """Emit the same flat yaml format as extract_structure.py."""
    def quote(value):
        if value is None:
            return "null"
        if isinstance(value, bool):
            return "true" if value else "false"
        if isinstance(value, (int, float)):
            return f"{value}"
        s = str(value)
        if s == "":
            return "''"
        needs_quote = any(c in s for c in ":#[]{},&*!|>'\"%@`") or \
                      s.lower() in ("yes", "no", "true", "false", "null", "~")
        if needs_quote:
            return "'" + s.replace("'", "''") + "'"
        return s

    lines = [
        f"description: {quote(f'Plottable histograms extracted from {ref_file}')}",
        f"input_file: {quote(str(ref_file))}",
        f"total_count: {len(items)}",
        "histograms:",
    ]
    for h in items:
        lines.append(f"  - key_path: {quote(h['key_path'])}")
        lines.append(f"    classname: {quote(h['classname'])}")
        lines.append(f"    title: {quote(h['title'])}")
        lines.append(f"    nbins: {quote(h['nbins'])}")
        lines.append(f"    xlow: {quote(h['xlow'])}")
        lines.append(f"    xhigh: {quote(h['xhigh'])}")
    return "\n".join(lines) + "\n"


# -----------------------------------------------------------------------------
# Per-scenario worker
# -----------------------------------------------------------------------------
def run_scenario(scenario_name: str,
                 scenario_dir: Path,
                 workdir_root: Path,
                 plotter_path: Path,
                 structure_yml_master: Path | None,
                 helper_dir: Path | None,
                 dry_run: bool,
                 groupings: list[str] | None = None) -> tuple[str, bool, str]:
    """
    Process one scenario end-to-end. Returns (name, success, message).
    """
    print(f"\n[{scenario_name}] scanning {scenario_dir}")
    samples = scan_scenario(scenario_dir)
    if not samples:
        msg = "no sample directories with .root files"
        print(f"    [error] {msg}")
        return (scenario_name, False, msg)
    n_data = sum(1 for s in samples.values() if s["type"] == "DATA")
    n_mc   = sum(1 for s in samples.values() if s["type"] == "MC")
    print(f"    found: {len(samples)} samples ({n_data} Data, {n_mc} MC), "
          f"total files: {sum(len(s['files']) for s in samples.values())}")

    # Set up scenario workdir
    workdir = workdir_root / scenario_name
    workdir.mkdir(parents=True, exist_ok=True)
    samples_yml = workdir / "samples_config.yml"
    structure_yml = workdir / "structure_info.yml"

    emit_samples_yaml(samples, samples_yml)
    print(f"    wrote {samples_yml}")

    # Structure: reuse master if available, otherwise extract from first MC file
    if structure_yml_master and structure_yml_master.exists():
        shutil.copy(structure_yml_master, structure_yml)
        print(f"    reused master {structure_yml_master.name}")
    else:
        # pick a representative MC sample to extract from
        ref_file = None
        for name in ("TTbar_Hadronic", "TTHHto4b", "TT4b"):
            if name in samples and samples[name]["files"]:
                ref_file = Path(samples[name]["files"][0])
                break
        if ref_file is None:
            for s in samples.values():
                if s["type"] == "MC" and s["files"]:
                    ref_file = Path(s["files"][0])
                    break
        if ref_file is None:
            msg = "no MC sample found to extract structure from"
            print(f"    [error] {msg}")
            return (scenario_name, False, msg)

        print(f"    extracting structure from {ref_file}")
        ok = extract_structure_yaml(ref_file, structure_yml, helper_dir)
        if not ok:
            return (scenario_name, False, "structure extraction failed")

    if dry_run:
        print("    [dry-run] skipping plotter invocation")
        return (scenario_name, True, "dry-run OK")

    # Invoke stack_plotter.C from inside the workdir
    # We copy the plotter into the workdir so its `#include` (if any) and
    # relative paths are predictable. Using a copy keeps the original
    # untouched.
    plotter_copy = workdir / plotter_path.name
    shutil.copy(plotter_path, plotter_copy)

    # [STEP21] grouping 모드별 실행 — env TTHH_PLOT_GROUPING 로 plotter 에
    # 전달, output 은 plots_<grouping>/ 에 분리 저장 (PDF).
    groupings = groupings or ["compact"]
    counts = []
    for grp in groupings:
        env = dict(os.environ, TTHH_PLOT_GROUPING=grp)
        cmd = ["root", "-l", "-b", "-q", plotter_copy.name]
        print(f"    running: cd {workdir} && TTHH_PLOT_GROUPING={grp} {' '.join(cmd)}")
        proc = subprocess.run(
            cmd, cwd=str(workdir), env=env,
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True
        )
        log_path = workdir / f"plotter_{grp}.log"
        log_path.write_text(proc.stdout)
        if proc.returncode != 0:
            tail = "\n".join(proc.stdout.splitlines()[-25:])
            msg = f"plotter({grp}) exited {proc.returncode}; tail:\n{tail}"
            print(f"    [error] {msg}")
            return (scenario_name, False, msg)
        plot_dir = workdir / f"plots_{grp}"
        n = len(list(plot_dir.glob("*.pdf"))) if plot_dir.exists() else 0
        counts.append(f"{grp}:{n}")
    msg = f"OK — plots ({', '.join(counts)}) in {workdir}/plots_<grouping>/"
    print(f"    [{scenario_name}] {msg}")
    return (scenario_name, True, msg)


# -----------------------------------------------------------------------------
# Driver
# -----------------------------------------------------------------------------
def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(
        description="Run stack plotter across all validation scenarios"
    )
    p.add_argument("--base", required=True, type=Path,
                   help="Output base from condor: AnalyzerOutput_validation")
    p.add_argument("--plotter", required=True, type=Path,
                   help="Path to stack_plotter.C")
    p.add_argument("--workdir", default=None, type=Path,
                   help="Working dir root (default: <base>/_plot_workdir)")
    p.add_argument("--scenarios", nargs="*", default=None,
                   help="Subset of scenario names; default: all subdirs of base")
    p.add_argument("--helpers", default=None, type=Path,
                   help="Directory containing create_sample_config.py and "
                        "extract_structure.py (for code reuse). "
                        "Default: same directory as --plotter.")
    p.add_argument("--structure-per-scenario", action="store_true",
                   help="Re-extract structure_info.yml for every scenario "
                        "instead of reusing the first one's")
    p.add_argument("--jobs", type=int, default=1,
                   help="Parallel scenarios (uses ThreadPool; ROOT itself "
                        "is single-threaded per scenario)")
    p.add_argument("--dry-run", action="store_true",
                   help="Build YAMLs but do not invoke ROOT")
    p.add_argument("--grouping", default="compact",
                   choices=["compact", "detailed", "both"],
                   help="[STEP21] legend 그룹핑: compact(MC 10줄, 기본) / "
                        "detailed(MC 13줄) / both(둘 다 생성; "
                        "plots_compact/, plots_detailed/ 분리 저장)")
    args = p.parse_args(argv)

    if not args.base.is_dir():
        print(f"[fatal] --base does not exist: {args.base}")
        return 2
    if not args.plotter.is_file():
        print(f"[fatal] --plotter not found: {args.plotter}")
        return 2

    helpers = args.helpers if args.helpers else args.plotter.parent
    workdir_root = args.workdir if args.workdir else (args.base / "_plot_workdir")
    workdir_root.mkdir(parents=True, exist_ok=True)

    # Auto-discover scenarios = direct subdirs that contain at least one
    # sample-looking subdir.
    if args.scenarios:
        scenarios = list(args.scenarios)
    else:
        scenarios = sorted([
            d.name for d in args.base.iterdir()
            if d.is_dir() and not d.name.startswith("_")
        ])
    if not scenarios:
        print(f"[fatal] no scenarios found under {args.base}")
        return 2

    print("=" * 70)
    print(f"  base       : {args.base}")
    print(f"  plotter    : {args.plotter}")
    print(f"  workdir    : {workdir_root}")
    print(f"  scenarios  : {scenarios}")
    print(f"  jobs       : {args.jobs}")
    print(f"  grouping   : {args.grouping}")
    print(f"  dry-run    : {args.dry_run}")
    print("=" * 70)

    # Master structure: extract once from the FIRST scenario, reuse elsewhere
    master_struct = None
    groupings = (["compact", "detailed"] if args.grouping == "both"
                 else [args.grouping])

    if not args.structure_per_scenario:
        first = args.base / scenarios[0]
        if first.is_dir():
            print(f"\n[master] extracting shared structure_info.yml from {first}")
            samples_first = scan_scenario(first)
            ref_file = None
            for nm in ("TTbar_Hadronic", "TTHHto4b", "TT4b"):
                if nm in samples_first and samples_first[nm]["files"]:
                    ref_file = Path(samples_first[nm]["files"][0])
                    break
            if ref_file is None:
                for s in samples_first.values():
                    if s["type"] == "MC" and s["files"]:
                        ref_file = Path(s["files"][0]); break
            if ref_file is not None:
                master_struct = workdir_root / "structure_info.master.yml"
                ok = extract_structure_yaml(ref_file, master_struct, helpers)
                if ok:
                    print(f"[master] {master_struct}")
                else:
                    print("[master] extraction failed; will retry per scenario")
                    master_struct = None
            else:
                print("[master] no MC ref file; will retry per scenario")

    # Run scenarios
    tasks = []
    if args.jobs > 1:
        with ThreadPoolExecutor(max_workers=args.jobs) as ex:
            futs = {
                ex.submit(run_scenario,
                          name, args.base / name, workdir_root,
                          args.plotter, master_struct, helpers,
                          args.dry_run, groupings): name
                for name in scenarios
                if (args.base / name).is_dir()
            }
            for fut in as_completed(futs):
                tasks.append(fut.result())
    else:
        for name in scenarios:
            sdir = args.base / name
            if not sdir.is_dir():
                print(f"\n[skip] {name}: not a directory")
                tasks.append((name, False, "not a directory"))
                continue
            tasks.append(run_scenario(
                name, sdir, workdir_root,
                args.plotter, master_struct, helpers,
                args.dry_run, groupings))

    # Summary
    print("\n" + "=" * 70)
    print("  Scenario runner summary")
    print("=" * 70)
    n_ok = sum(1 for _, ok, _ in tasks if ok)
    n_bad = len(tasks) - n_ok
    for name, ok, msg in sorted(tasks):
        flag = "OK " if ok else "FAIL"
        print(f"  [{flag}] {name:20s} {msg}")
    print(f"  {n_ok}/{len(tasks)} scenarios succeeded "
          f"({n_bad} failed)")
    print(f"  outputs under: {workdir_root}/<scenario>/plots_<grouping>/")
    return 0 if n_bad == 0 else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
