#!/usr/bin/env python3
"""
runWithManager.py
- Cross-platform (Linux/macOS) job manager with detailed logging & heartbeats.
- Launches ./exe_bTagStudy {sample} while respecting memory threshold & max concurrency.
- Writes a manager log (manager.log) and per-sample stdout/err logs under logs/.
"""

import os
import sys
import time
import shlex
import subprocess
import platform
from datetime import datetime
from pathlib import Path
from typing import List, Dict, Tuple, Optional

# ======================= EDIT SAMPLES HERE =======================
SAMPLES: List[str] = [

####  "QCD",
####  "QCD_HT200to300",
####  "QCD_HT300to500",
####  "QCD_HT500to700",
####  "QCD_HT700to1000",
####  "QCD_HT1000to1500",
####  "QCD_HT1500to2000",
####  "QCD_HT2000toInf",

  "QCD_Pt_15to30",
  "QCD_Pt_30to50",
  "QCD_Pt_50to80",
  "QCD_Pt_80to120",
  "QCD_Pt_120to170",
  "QCD_Pt_170to300",
  "QCD_Pt_300to470",
  "QCD_Pt_470to600",
  "QCD_Pt_600to800",
  "QCD_Pt_800to1000",
  "QCD_Pt_1000to1400",
  "QCD_Pt_1400to1800",
  "QCD_Pt_1800to2400",
  "QCD_Pt_2400to3200",
  "QCD_Pt_3200toInf",

  "tt4b",
  "ttbb",
  "ttHH",
  "ttHtobb",
#  "ttJets",
  "ttTohadronic",
  "TTTo2L2Nu",
  "TTToSemiLeptonic",
  "tttt",
  "tttW",
  "ttWH",
  "ttWW",
  "ttWZ",
  "ttZHto4b",
  "ttZtobb",
  "ttZZto4b",

#  "Data"
  "JetHT_B",
  "JetHT_C",
  "JetHT_D",
  "JetHT_E",
  "JetHT_F",
  "BTagCSV_B",
  "BTagCSV_C",
  "BTagCSV_D",
  "BTagCSV_E",
  "BTagCSV_F"
]
# ================================================================

# ---------------------- SETTINGS ----------------------
COMMAND_TEMPLATE   = "./exe_bTagStudy {sample}"
WORKDIR            = "."
MAX_CONCURRENT     = 4
MIN_FREE_MEM_MB    = 4000            # Launch only if MemAvailable > this
ALLOW_ON_UNKNOWN   = True            # If mem_available can't be determined (0), allow 1 job to avoid deadlock
CHECK_INTERVAL_SEC = 3
RETRIES            = 1
LOG_DIR            = "logs"
MANAGER_LOG        = "manager.log"   # Manager-level log (decisions, heartbeats)
PRE_COMMAND        = ""              # e.g., 'mkdir -p outputs/{sample}'
POST_COMMAND       = ""              # e.g., 'mv result_{sample}.root outputs/{sample}/'
# ------------------------------------------------------

# ---------------------- LOGGING -----------------------
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
# ------------------------------------------------------

def mem_available_mb() -> int:
    """Cross-platform available memory (MB)."""
    # Try psutil first (the most reliable cross-platform)
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
                        kb = int(line.split()[1])
                        return kb // 1024
        except Exception:
            pass
    elif system == "darwin":  # macOS
        # Fall back to vm_stat + pagesize
        try:
            page_size = int(subprocess.check_output(["sysctl", "-n", "hw.pagesize"]).strip())
            vm = subprocess.check_output(["vm_stat"]).decode()
            free_pages = 0
            # Treat free + inactive as launchable (heuristic)
            for line in vm.splitlines():
                if "Pages free" in line or "Pages inactive" in line:
                    count = line.split(":")[1].strip().strip(".").replace(".", "").replace(",", "")
                    free_pages += int(count)
            free_bytes = free_pages * page_size
            return free_bytes // (1024 * 1024)
        except Exception:
            pass

    # Unknown OS or failed to detect
    return 0

def can_launch_new_job(running: Dict[str, subprocess.Popen]) -> Tuple[bool, str]:
    """Return (decision, reason)."""
    current = len(running)
    if current >= MAX_CONCURRENT:
        return False, f"Max concurrency reached ({current}/{MAX_CONCURRENT})."

    mem_mb = mem_available_mb()
    if mem_mb == 0:
        if ALLOW_ON_UNKNOWN and current == 0:
            return True, "Mem unknown (0 MB), but ALLOW_ON_UNKNOWN=True and no jobs running -> allow 1 launch."
        return False, "Mem unknown (0 MB) and ALLOW_ON_UNKNOWN=False or jobs already running."

    if mem_mb <= MIN_FREE_MEM_MB:
        return False, f"Not enough free mem: {mem_mb}MB <= threshold {MIN_FREE_MEM_MB}MB."

    return True, f"OK to launch (free mem {mem_mb}MB, running {current})."

def build_command(sample: str) -> str:
    core = COMMAND_TEMPLATE.format(sample=sample)
    pre = PRE_COMMAND.format(sample=sample) if PRE_COMMAND else ""
    post = POST_COMMAND.format(sample=sample) if POST_COMMAND else ""
    seq = " ; ".join([x for x in (pre, core, post) if x])
    if seq:
        # Shell needed for pre/post; use bash -lc to allow shell features
        return f"bash -lc {shlex.quote(seq)}"
    else:
        return core

def launch_job(sample: str) -> Tuple[str, subprocess.Popen]:
    cmd_str = build_command(sample)
    os.makedirs(LOG_DIR, exist_ok=True)
    out_path = Path(LOG_DIR) / f"{sample}.out"
    err_path = Path(LOG_DIR) / f"{sample}.err"
    outf = open(out_path, "wb")
    errf = open(err_path, "wb")

    log(f"LAUNCH [{sample}]: {cmd_str}")
    proc = subprocess.Popen(
        cmd_str,
        cwd=WORKDIR,
        stdout=outf,
        stderr=errf,
        shell=True,
        executable="/bin/bash",
    )
    log(f"STARTED [{sample}] pid={proc.pid}")
    return sample, proc

def reap_finished(running: Dict[str, subprocess.Popen]) -> List[Tuple[str, int]]:
    finished = []
    to_delete = []
    for sample, proc in running.items():
        ret = proc.poll()
        if ret is not None:
            finished.append((sample, ret))
            to_delete.append(sample)
    for s in to_delete:
        del running[s]
    return finished

def heartbeat(queue: List[str], running: Dict[str, subprocess.Popen]) -> None:
    mem_mb = mem_available_mb()
    running_info = ", ".join([f"{s}(pid={p.pid})" for s, p in running.items()]) or "-"
    log(f"HEARTBEAT: queue={len(queue)} running={len(running)} mem_avail={mem_mb}MB | running_list=[{running_info}]")

def main() -> int:
    log("======== RUN START ========")
    if not SAMPLES:
        log("[ERROR] SAMPLES list is empty. Edit this file and add sample names.")
        return 2

    log(f"[INFO] Loaded {len(SAMPLES)} samples: {SAMPLES}")
    attempts = {s: 0 for s in SAMPLES}
    queue = SAMPLES.copy()
    running: Dict[str, subprocess.Popen] = {}
    succeeded: List[str] = []
    failed: List[Tuple[str, int]] = []

    idle_ticks = 0

    while queue or running:
        decision, reason = can_launch_new_job(running)
        if queue and decision:
            s = queue.pop(0)
            attempts[s] += 1
            try:
                sam, proc = launch_job(s)
                running[sam] = proc
                idle_ticks = 0  # we did some work
            except Exception as e:
                log(f"[LAUNCH-ERROR] {s}: {e}")
                if attempts[s] <= RETRIES:
                    log(f"[RETRY] Re-queue {s} (attempt {attempts[s]}/{RETRIES})")
                    queue.append(s)
                else:
                    failed.append((s, -999))
        else:
            # Couldn't launch (or no queue); tell why.
            if queue:
                log(f"[WAIT] {reason} (queue size={len(queue)})")

        # Reap
        finished = reap_finished(running)
        for s, rc in finished:
            if rc == 0:
                log(f"[DONE] {s} rc=0")
                succeeded.append(s)
            else:
                log(f"[FAIL] {s} rc={rc}")
                if attempts[s] <= RETRIES:
                    log(f"[RETRY] Re-queue {s} (attempt {attempts[s]}/{RETRIES})")
                    queue.append(s)
                else:
                    failed.append((s, rc))

        # Heartbeat
        heartbeat(queue, running)

        # If nothing happened, sleep a bit; else loop immediately
        if not finished and (not queue or not decision):
            idle_ticks += 1
            time.sleep(CHECK_INTERVAL_SEC)
        else:
            idle_ticks = 0

    log("======== RUN END ========")
    # Summary
    log("====== SUMMARY ======")
    log(f"Success: {len(succeeded)}")
    if succeeded:
        log("  " + ", ".join(succeeded))
    log(f"Failed: {len(failed)}")
    if failed:
        for s, rc in failed:
            log(f"  {s} (rc={rc})")
    log("=====================")
    return 0 if not failed else 1

if __name__ == "__main__":
    sys.exit(main())

