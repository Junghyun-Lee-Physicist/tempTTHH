#!/usr/bin/env python3
"""
run_all.py

Job-pool manager for exe_BTagSF.
- Cross-platform (Linux / macOS) memory-aware concurrency control.
- Keeps exactly N slots filled; dispatches next job the instant one finishes.
- Per-sample stdout/stderr logs under logs/.
- Manager-level log (manager.log) with heartbeats.

Usage:
    python3 run_all.py              # default settings
    python3 run_all.py -j 4         # 4 parallel jobs
    python3 run_all.py -j 1         # sequential (debugging)
    python3 run_all.py --mem 8000   # require 8 GB free before launching

Author: Junghyun Lee
"""

import os
import sys
import time
import subprocess
import platform
import argparse
from datetime import datetime
from pathlib import Path
from typing import List, Dict, Tuple

# ============================================================================
# [1] Sample List  (SingleMuon excluded)
# ============================================================================
SAMPLES: List[str] = [
    # Data: BTagCSV
##    "BTagCSV_B",  "BTagCSV_C",  "BTagCSV_D",  "BTagCSV_E",  "BTagCSV_F",
    # Data: JetHT
##    "JetHT_B",    "JetHT_C",    "JetHT_D",    "JetHT_E",    "JetHT_F",
    # MC: ttbar inclusive
##    "TTbar_Hadronic",  "TTbar_DiLep",  "TTbar_SemiLep",
    # MC: signal & rare
##    "TTHHto4b",  "TT4b",  "ttHTobb",  "TTTT",  "TTTW",
##    "TTWH",  "TTWW",  "TTWZ",  "TTZHTo4b",  "TTZToBB",  "TTZZTo4b",  "ttbb",
    # MC: QCD
##    "QCD_HT200to300",  "QCD_HT300to500",  "QCD_HT500to700",
##    "QCD_HT700to1000",  "QCD_HT1000to1500",  "QCD_HT1500to2000",  "QCD_HT2000toInf",
    "TTZZTo4b"
]

# ============================================================================
# [2] Default Settings
# ============================================================================
EXECUTABLE         = "./exe_BTagSF"
WORKDIR            = "."
MAX_CONCURRENT     = 10
MIN_FREE_MEM_MB    = 4000      # Launch only if MemAvailable > this
ALLOW_ON_UNKNOWN   = True      # If mem can't be read, allow 1 job to avoid deadlock
CHECK_INTERVAL_SEC = 2
RETRIES            = 1         # Number of retries on failure (0 = no retry)
LOG_DIR            = "logs"
MANAGER_LOG        = "manager.log"

# ============================================================================
# [3] Logging
# ============================================================================
def ts() -> str:
    return datetime.now().strftime("%Y-%m-%d %H:%M:%S")

def log(msg: str, end: str = "\n") -> None:
    """Print to stdout and append to manager log file."""
    line = f"[{ts()}] {msg}"
    print(line, end=end, flush=True)
    try:
        with open(MANAGER_LOG, "a", encoding="utf-8") as f:
            f.write(line + ("\n" if end == "\n" else ""))
    except Exception:
        pass

# ============================================================================
# [4] Memory Detection  (cross-platform)
# ============================================================================
def mem_available_mb() -> int:
    """Return available memory in MB.  0 if unknown."""
    # psutil (most reliable if installed)
    try:
        import psutil  # type: ignore
        return psutil.virtual_memory().available // (1024 * 1024)
    except Exception:
        pass

    system = platform.system().lower()

    if system == "linux":
        try:
            with open("/proc/meminfo", "r") as f:
                for line in f:
                    if line.startswith("MemAvailable:"):
                        return int(line.split()[1]) // 1024
        except Exception:
            pass

    elif system == "darwin":
        try:
            page_size = int(
                subprocess.check_output(["sysctl", "-n", "hw.pagesize"]).strip()
            )
            vm = subprocess.check_output(["vm_stat"]).decode()
            free_pages = 0
            for line in vm.splitlines():
                if "Pages free" in line or "Pages inactive" in line:
                    count = (
                        line.split(":")[1]
                        .strip()
                        .strip(".")
                        .replace(".", "")
                        .replace(",", "")
                    )
                    free_pages += int(count)
            return (free_pages * page_size) // (1024 * 1024)
        except Exception:
            pass

    return 0

# ============================================================================
# [5] Launch Decision
# ============================================================================
def can_launch(n_running: int, max_conc: int, min_mem_mb: int) -> Tuple[bool, str]:
    """Return (allowed, reason)."""
    if n_running >= max_conc:
        return False, f"concurrency limit ({n_running}/{max_conc})"

    mem_mb = mem_available_mb()
    if mem_mb == 0:
        if ALLOW_ON_UNKNOWN and n_running == 0:
            return True, "mem unknown, but ALLOW_ON_UNKNOWN and 0 running"
        return False, "mem unknown"

    if mem_mb <= min_mem_mb:
        return False, f"low memory ({mem_mb} MB <= {min_mem_mb} MB)"

    return True, f"ok (free {mem_mb} MB, running {n_running})"

# ============================================================================
# [6] Job Launch & Reap
# ============================================================================
def launch_job(sample: str) -> subprocess.Popen:
    """Launch exe_BTagSF for a single sample, return Popen handle."""
    os.makedirs(LOG_DIR, exist_ok=True)
    out_path = Path(LOG_DIR) / f"{sample}.out"
    err_path = Path(LOG_DIR) / f"{sample}.err"

    outf = open(out_path, "wb")
    errf = open(err_path, "wb")

    cmd = [EXECUTABLE, sample]
    proc = subprocess.Popen(
        cmd,
        cwd=WORKDIR,
        stdout=outf,
        stderr=errf,
    )
    log(f"  [launch] {sample}  (PID {proc.pid})")
    return proc


def reap_finished(
    running: Dict[str, subprocess.Popen],
) -> List[Tuple[str, int]]:
    """Poll all running jobs; return list of (sample, returncode) for finished ones."""
    finished = []
    for sample, proc in list(running.items()):
        rc = proc.poll()
        if rc is not None:
            finished.append((sample, rc))
            del running[sample]
    return finished

# ============================================================================
# [7] Heartbeat
# ============================================================================
def heartbeat(
    queue: List[str],
    running: Dict[str, subprocess.Popen],
    succeeded: List[str],
    failed: List[Tuple[str, int]],
) -> None:
    mem_mb = mem_available_mb()
    running_str = ", ".join(
        f"{s}(pid={p.pid})" for s, p in running.items()
    ) or "-"
    log(
        f"  [heartbeat] queue={len(queue)} running={len(running)} "
        f"done={len(succeeded)} fail={len(failed)} "
        f"mem={mem_mb}MB | [{running_str}]"
    )

# ============================================================================
# [8] Main Loop
# ============================================================================
def main() -> int:
    parser = argparse.ArgumentParser(description="B-Tag SF job pool manager")
    parser.add_argument(
        "-j", "--jobs", type=int, default=MAX_CONCURRENT,
        help=f"max parallel jobs (default: {MAX_CONCURRENT})",
    )
    parser.add_argument(
        "--mem", type=int, default=MIN_FREE_MEM_MB,
        help=f"min free memory in MB to launch (default: {MIN_FREE_MEM_MB})",
    )
    parser.add_argument(
        "--retries", type=int, default=RETRIES,
        help=f"number of retries per sample (default: {RETRIES})",
    )
    args = parser.parse_args()

    max_concurrent  = args.jobs
    min_free_mem_mb = args.mem
    max_retries     = args.retries

    # ── Preflight checks ──
    if not Path(EXECUTABLE).is_file():
        log(f"[FATAL] {EXECUTABLE} not found. Run 'make' first.")
        return 2

    total = len(SAMPLES)
    log("╔══════════════════════════════════════════════════════════════╗")
    log("║  B-Tag Reweight: Python job pool                            ║")
    log("╠══════════════════════════════════════════════════════════════╣")
    log(f"  Samples        : {total}")
    log(f"  Max parallel   : {max_concurrent}")
    log(f"  Min free mem   : {min_free_mem_mb} MB")
    log(f"  Retries        : {max_retries}")
    log(f"  Log dir        : {LOG_DIR}/")
    log("╚══════════════════════════════════════════════════════════════╝")
    log("")

    # ── State ──
    queue: List[str] = SAMPLES.copy()
    attempts: Dict[str, int] = {s: 0 for s in SAMPLES}
    running: Dict[str, subprocess.Popen] = {}
    succeeded: List[str] = []
    failed: List[Tuple[str, int]] = []

    start_time = time.time()
    heartbeat_interval = 30  # seconds
    last_heartbeat = 0.0

    while queue or running:
        # ── Try to fill vacant slots ──
        launched_any = False
        while queue:
            ok, reason = can_launch(len(running), max_concurrent, min_free_mem_mb)
            if not ok:
                if not launched_any:
                    log(f"  [wait] {reason}  (queue={len(queue)})")
                break

            sample = queue.pop(0)
            attempts[sample] = attempts.get(sample, 0) + 1
            try:
                proc = launch_job(sample)
                running[sample] = proc
                launched_any = True
            except Exception as e:
                log(f"  [launch-error] {sample}: {e}")
                if attempts[sample] <= max_retries:
                    log(f"  [retry] re-queue {sample} ({attempts[sample]}/{max_retries})")
                    queue.append(sample)
                else:
                    failed.append((sample, -999))

        # ── Reap finished jobs ──
        finished = reap_finished(running)
        for sample, rc in finished:
            elapsed = time.time() - start_time
            if rc == 0:
                log(f"  ✓ {sample}  (rc=0, elapsed={elapsed:.0f}s)  "
                    f"[{len(succeeded)+1+len(failed)}/{total}]")
                succeeded.append(sample)
            else:
                log(f"  ✗ {sample}  (rc={rc}, see {LOG_DIR}/{sample}.err)  "
                    f"[{len(succeeded)+len(failed)+1}/{total}]")
                if attempts.get(sample, 0) <= max_retries:
                    log(f"  [retry] re-queue {sample} "
                        f"({attempts[sample]}/{max_retries})")
                    queue.append(sample)
                else:
                    failed.append((sample, rc))

        # ── Heartbeat ──
        now = time.time()
        if now - last_heartbeat > heartbeat_interval:
            heartbeat(queue, running, succeeded, failed)
            last_heartbeat = now

        # ── Sleep if idle ──
        if not finished and not launched_any:
            time.sleep(CHECK_INTERVAL_SEC)

    # ── Summary ──
    elapsed = time.time() - start_time
    log("")
    log("╔══════════════════════════════════════════════════════════════╗")
    log("║  Summary                                                     ║")
    log("╠══════════════════════════════════════════════════════════════╣")
    log(f"  Total elapsed : {elapsed:.1f}s  ({elapsed/60:.1f} min)")
    log(f"  Passed : {len(succeeded)} / {total}")
    log(f"  Failed : {len(failed)} / {total}")
    if failed:
        log("  Failed samples:")
        for s, rc in failed:
            log(f"    - {s}  (rc={rc})")
    log("╚══════════════════════════════════════════════════════════════╝")

    return 0 if not failed else 1


if __name__ == "__main__":
    sys.exit(main())

