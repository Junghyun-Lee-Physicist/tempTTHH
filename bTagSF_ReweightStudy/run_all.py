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
# [1] Sample List  (SingleMuon excluded — trigger SF 유도 전용)
# ----------------------------------------------------------------------------
# [2026-07-29] 손으로 관리하던 목록(마지막 한 줄만 남고 전부 주석 처리돼 있었다)
#   을 **자동 유도**로 바꿨다. 목록을 손으로 유지하면 두 가지가 생긴다:
#     - 주석 처리된 채로 잊혀서 1개 샘플만 도는 사고 (실제로 그 상태였다)
#     - xsec_db 에 샘플이 추가돼도 여기 반영이 안 되는 표류
#
#   이제 xsec_db 를 진실로 삼고, 실제로 돌릴 수 있는 것만 남긴다:
#     (a) prescan_summary 에 Σgenw 가 있어야 한다  — 없으면 weight 합성 불가
#     (b) $TTHH_SKIM_DIR/<sample>.root 가 있어야 한다 — 없으면 열 수 없음
#   빠지는 샘플은 이유와 함께 출력한다 (조용히 줄어들지 않게).
#
#   --samples 로 명시하면 이 자동 유도를 건너뛴다.
# ============================================================================
HERE      = Path(__file__).resolve().parent
BASE_DIR  = Path(os.environ.get("TTHH_BASE", HERE.parent))
XSEC_DB   = Path(os.environ.get("TTHH_XSEC_DB", BASE_DIR / "data/samples_2017UL.json"))
PRESCAN   = Path(os.environ.get("TTHH_PRESCAN",
                                BASE_DIR / "prescan_summary/prescan_summary.json"))
SKIM_DIR  = os.environ.get("TTHH_SKIM_DIR", "/Users/jhlee/ttHH/ntuple/skimmed/gen_tier3/")

# include/SampleAlias.h 의 표와 **같은 내용**이어야 한다 (canonical -> legacy).
# 2017 prescan_summary 가 STEP18 이전 이름으로 남아 있어서 필요하다.
SAMPLE_ALIAS: Dict[str, str] = {
    "TTbar_Hadronic": "TTToHadronic",   "TTbar_SemiLep": "TTToSemiLeptonic",
    "TTbar_DiLep":    "TTTo2L2Nu",      "TTbb_Hadronic": "ttbb_Hadronic",
    "TTbb_SemiLep":   "ttbb_SemiLeptonic", "TTbb_DiLep":  "ttbb_2L2Nu",
    "TT4b":           "tt4b",           "TTHHto4b":      "ttHH",
    "ttHTobb":        "ttHtobb",        "TTZHTo4b":      "ttZHto4b",
    "TTZZTo4b":       "ttZZto4b",       "TTZToBB":       "ttZtobb",
    "TTWH":           "ttWH",           "TTWW":          "ttWW",
    "TTWZ":           "ttWZ",           "TTTW":          "tttW",
    "TTTT":           "tttt",
}


def _load_json(path: Path, what: str) -> dict:
    if not path.is_file():
        print(f"[FATAL] {what} not found: {path}", file=sys.stderr)
        sys.exit(2)
    import json
    with path.open(encoding="utf-8") as f:
        return json.load(f)


def discover_samples() -> Tuple[List[str], List[Tuple[str, str]]]:
    """Return (runnable, skipped[(sample, reason)])."""
    xsec = _load_json(XSEC_DB, "xsec_db")
    pre  = _load_json(PRESCAN, "prescan summary").get("samples", {})

    runnable: List[str] = []
    skipped:  List[Tuple[str, str]] = []

    for name, rec in xsec.items():
        if name == "_meta" or "_ext" in name:
            continue
        is_data = rec.get("is_data", False) or rec.get("cross_section_fb") is None
        if is_data and name.startswith("SingleMuon"):
            continue                                  # trigger SF 전용
        if not is_data:
            keys = [name, SAMPLE_ALIAS.get(name, "")]
            if not any(k and k in pre for k in keys):
                skipped.append((name, "no prescan (Σgenw unknown)"))
                continue
        if not Path(SKIM_DIR, f"{name}.root").is_file():
            skipped.append((name, "no skim file"))
            continue
        runnable.append(name)

    return runnable, skipped


SAMPLES: List[str] = []   # main() 에서 채운다

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
    parser.add_argument(
        "--samples", nargs="+", default=None,
        help="샘플을 명시 (지정 시 자동 유도를 건너뛴다)",
    )
    parser.add_argument(
        "--dry-run", action="store_true",
        help="어떤 샘플이 돌고 어떤 게 왜 빠지는지만 출력하고 종료",
    )
    args = parser.parse_args()

    max_concurrent  = args.jobs
    min_free_mem_mb = args.mem
    max_retries     = args.retries

    # ── 샘플 결정 ────────────────────────────────────────────────────────
    global SAMPLES
    if args.samples:
        SAMPLES = list(args.samples)
        skipped: List[Tuple[str, str]] = []
    else:
        SAMPLES, skipped = discover_samples()

    log(f"  xsec_db  : {XSEC_DB}")
    log(f"  prescan  : {PRESCAN}")
    log(f"  skim dir : {SKIM_DIR}")
    if skipped:
        log(f"  ── 제외된 샘플 {len(skipped)}개 (조용히 빠지지 않도록 전부 나열) ──")
        for s, why in skipped:
            log(f"     - {s:35s} {why}")
    if not SAMPLES:
        log("[FATAL] 돌릴 샘플이 하나도 없다. 위 제외 사유를 확인할 것.")
        return 2
    log(f"  ── 실행 대상 {len(SAMPLES)}개 ──")
    for s in SAMPLES:
        log(f"     + {s}")
    log("")

    if args.dry_run:
        log("[dry-run] 여기서 종료.")
        log("다음 단계: ./exe_MakeJSON " + " ".join(SAMPLES))
        return 0

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

    # 다음 단계를 **성공한 샘플만으로** 만들어 준다. 실패한 샘플을 그대로
    # 넘기면 makeReweightJSON 이 "[WARN] Cannot open ..., skipping" 만 찍고
    # 그 group 을 조용히 작은 통계로 유도한다.
    if succeeded:
        log("")
        log("다음 단계 (성공한 샘플만):")
        log("  ./exe_MakeJSON " + " ".join(succeeded))

    return 0 if not failed else 1


if __name__ == "__main__":
    sys.exit(main())

