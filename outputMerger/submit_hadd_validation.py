#!/usr/bin/env python3
"""
================================================================================
SCRIPT: submit_hadd_validation.py
================================================================================

[ Purpose ]
Submit one condor job per (scenario, sample) pair to hadd the per-file split
ROOT outputs from the validation-mode analyzer runs.

[ Input layout (produced by submit_job_FH_Tier3_unified.py) ]
    <base>/<scenario>/<sample>/<sample>_<n>.root

[ Output (produced by these jobs) ]
    <base>/<scenario>/<sample>.root          ← hadd of all *_<n>.root in dir

[ Why split per (scenario, sample) ]
- Each hadd is independent → embarrassingly parallel
- Job count = 12 scenarios × 37 samples = up to 444 jobs (only existing
  combinations are dispatched)
- Each job is small (a few minutes wall-clock) so condor turn-around is fast
- Failure recovery is trivial: re-run only the missing <sample>.root files

[ Usage ]
    python3 submit_hadd_validation.py
    python3 submit_hadd_validation.py --scenarios baseline bcut0
    python3 submit_hadd_validation.py --skip-existing
    python3 submit_hadd_validation.py --dry-run

[ Discovery ]
At runtime, the script walks <base>/<scenario>/<sample>/ for .root files.
Empty sample directories or scenarios are silently skipped. Dirs whose
hadd target already exists are skipped iff --skip-existing is given.

[ Notes ]
- Mirrors the condor template style of submit_job_FH_Tier3_unified.py
  (x509userproxy, getenv, WantOS, request_memory).
- Tier3 environment assumed: el9 OS, CMSSW + cmsenv on the worker node.
- The hadd runner script is `run_one_hadd.sh` (must be in the same
  directory as this submitter, or specify --runner path).
================================================================================
"""

from __future__ import annotations

import argparse
import os
import subprocess
import sys
import time
from pathlib import Path


# -----------------------------------------------------------------------------
# Defaults — adjust here if KISTI paths or OS conventions change.
# -----------------------------------------------------------------------------
DEFAULT_BASE = Path(
    "/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/AnalyzerOutput_validation"
)
DEFAULT_OS_VERSION = "el9"
DEFAULT_MEMORY     = "4 GB"   # hadd is I/O-bound; 4G is generous

# Resolved at import time. Used as the default search anchor for the
# proxy file ("proxy.cert" sitting next to this script). The CLI flag
# `--proxy` overrides; if the script lives in a subdirectory of the
# analyzer (e.g. ${ANALYZER_DIR}/outputMerger/) the user can still point
# to the analyzer-root proxy with `--proxy ../proxy.cert`.
SCRIPT_DIR        = Path(__file__).resolve().parent
DEFAULT_PROXY_PATH = SCRIPT_DIR / "proxy.cert"


# -----------------------------------------------------------------------------
# Sample-discovery: list (scenario, sample, files[]) tuples
# -----------------------------------------------------------------------------
def discover_jobs(base: Path,
                  scenarios_filter: list[str] | None,
                  samples_filter:   list[str] | None,
                  skip_existing: bool) -> list[dict]:
    """
    Return a list of jobs to dispatch.
    Each job is a dict with keys: scenario, sample, indir, outfile, files.
    """
    jobs: list[dict] = []

    if not base.is_dir():
        print(f"[fatal] base not found: {base}")
        return jobs

    scen_dirs = sorted([d for d in base.iterdir() if d.is_dir()
                        and not d.name.startswith("_")])
    if scenarios_filter:
        scen_dirs = [d for d in scen_dirs if d.name in scenarios_filter]
        missing = set(scenarios_filter) - {d.name for d in scen_dirs}
        if missing:
            print(f"[warn] requested scenarios not found: {sorted(missing)}")

    for sdir in scen_dirs:
        sample_dirs = sorted([d for d in sdir.iterdir() if d.is_dir()])
        if samples_filter:
            sample_dirs = [d for d in sample_dirs if d.name in samples_filter]

        for samp in sample_dirs:
            files = sorted(samp.glob("*.root"))
            # Filter out our own hadd target if present (don't hadd it back
            # into itself when re-running).
            files = [f for f in files if not f.name == f"{samp.name}.root"]
            if not files:
                continue

            outfile = sdir / f"{samp.name}.root"
            if skip_existing and outfile.exists() and outfile.stat().st_size > 0:
                # Optional: also could check that nfile in the .root matches
                # len(files), but that requires opening it — skip the hassle.
                continue

            jobs.append({
                "scenario": sdir.name,
                "sample":   samp.name,
                "indir":    str(samp),
                "outfile":  str(outfile),
                "files":    [str(f) for f in files],
            })

    return jobs


# -----------------------------------------------------------------------------
# Condor file emitters
# -----------------------------------------------------------------------------
def write_arguments_file(jobs: list[dict], path: Path) -> None:
    """One argv line per condor process: <indir> <outfile>"""
    with path.open("w") as f:
        for j in jobs:
            f.write(f"{j['indir']} {j['outfile']}\n")


def write_submit_file(submit_path: Path,
                      runner: Path,
                      args_file: Path,
                      log_dir: Path,
                      proxy: Path,
                      os_version: str,
                      memory: str) -> None:
    """Write the condor submit description."""
    with submit_path.open("w") as f:
        f.write(f"x509userproxy           = {proxy}\n")
        f.write("getenv                  = True\n")
        f.write(f"executable              = {runner}\n")
        f.write("arguments               = $(args)\n")
        f.write(f"output                  = "
                f"{log_dir}/job.$(ClusterId).$(ProcId).out\n")
        f.write(f"error                   = "
                f"{log_dir}/job.$(ClusterId).$(ProcId).err\n")
        f.write(f"log                     = "
                f"{log_dir}/log.$(ClusterId).log\n")
        f.write(f"MY.WantOS               = \"{os_version}\"\n")
        f.write(f"request_memory          = {memory}\n")
        # transfer not needed — we read/write to PNFS directly via xrootd
        # mount; the worker node sees the same path.
        f.write(f"queue args from {args_file}\n")


# -----------------------------------------------------------------------------
# Driver
# -----------------------------------------------------------------------------
def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(
        description="Submit per-(scenario, sample) hadd jobs to condor"
    )
    p.add_argument("--base", type=Path, default=DEFAULT_BASE,
                   help=f"Output base of validation runs "
                        f"(default: {DEFAULT_BASE})")
    p.add_argument("--scenarios", nargs="*", default=None,
                   help="Restrict to given scenario names "
                        "(default: all subdirs of --base)")
    p.add_argument("--samples", nargs="*", default=None,
                   help="Restrict to given sample names")
    p.add_argument("--skip-existing", action="store_true",
                   help="Skip a job if its <sample>.root already exists")
    p.add_argument("--workdir", type=Path, default=None,
                   help="Where to put argv list, submit file, logs. "
                        "Default: <basedir-of-this-script>/condor_hadd_<timestamp>/")
    p.add_argument("--runner", type=Path, default=None,
                   help="Path to run_one_hadd.sh "
                        "(default: same dir as this script)")
    p.add_argument("--proxy", type=Path, default=DEFAULT_PROXY_PATH,
                   help=f"Path to proxy.cert (default: {DEFAULT_PROXY_PATH})")
    p.add_argument("--os-version", default=DEFAULT_OS_VERSION,
                   help=f"WantOS value (default: {DEFAULT_OS_VERSION})")
    p.add_argument("--memory", default=DEFAULT_MEMORY,
                   help=f"request_memory (default: {DEFAULT_MEMORY})")
    p.add_argument("--dry-run", action="store_true",
                   help="Generate submit/argv files but do not run condor_submit")
    args = p.parse_args(argv)

    # SCRIPT_DIR is the resolved directory of this submitter script
    # (defined at module load time). The default runner sits next to it.
    runner = args.runner if args.runner else (SCRIPT_DIR / "run_one_hadd.sh")
    if not runner.is_file():
        print(f"[fatal] runner not found: {runner}")
        return 2
    # ensure runner is executable
    if not os.access(runner, os.X_OK):
        try:
            runner.chmod(0o755)
        except Exception as e:
            print(f"[warn] could not chmod runner: {e}")

    if not args.proxy.is_file():
        print(f"[warn] proxy not found at {args.proxy} — submit may fail")

    # ── Discover jobs ────────────────────────────────────────────────────
    print(f"[discover] scanning {args.base}")
    jobs = discover_jobs(args.base, args.scenarios, args.samples,
                         args.skip_existing)
    if not jobs:
        print("[fatal] no jobs to submit (after filtering)")
        return 2

    # Show a summary
    by_scen: dict[str, int] = {}
    for j in jobs:
        by_scen[j["scenario"]] = by_scen.get(j["scenario"], 0) + 1
    print(f"[discover] {len(jobs)} jobs across {len(by_scen)} scenarios:")
    for scen, n in sorted(by_scen.items()):
        print(f"             {scen:20s}  {n:3d} samples")

    # ── Prepare workdir ──────────────────────────────────────────────────
    timestamp = time.strftime("%Y%m%d-%H%M%S")
    workdir = args.workdir if args.workdir else \
              (SCRIPT_DIR / f"condor_hadd_{timestamp}")
    workdir.mkdir(parents=True, exist_ok=True)
    log_dir = workdir / "logs"
    log_dir.mkdir(parents=True, exist_ok=True)

    args_file   = workdir / "arguments.txt"
    submit_file = workdir / "hadd.sub"

    write_arguments_file(jobs, args_file)
    write_submit_file(submit_file, runner, args_file, log_dir,
                      args.proxy, args.os_version, args.memory)

    print(f"[setup] workdir: {workdir}")
    print(f"[setup] argv list:    {args_file}  ({len(jobs)} lines)")
    print(f"[setup] submit file:  {submit_file}")

    if args.dry_run:
        print("[dry-run] not invoking condor_submit; inspect the files above.")
        return 0

    # ── Submit ───────────────────────────────────────────────────────────
    print("[submit] running condor_submit ...")
    proc = subprocess.run(
        ["condor_submit", str(submit_file)],
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True
    )
    print(proc.stdout)
    if proc.returncode != 0:
        print(f"[fatal] condor_submit exited {proc.returncode}")
        return proc.returncode

    print(f"[done] {len(jobs)} jobs submitted.")
    print(f"       monitor:    condor_q $USER")
    print(f"       inspect:    ls -la {log_dir}")
    print(f"       on success: validate with the watch script:")
    print(f"                   ls {args.base}/*/*.root | wc -l   "
          f"# expect {len(jobs)}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
