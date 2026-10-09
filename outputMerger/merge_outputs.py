#!/usr/bin/env python3
"""
================================================================================
SCRIPT: merge_outputs.py   [STEP22, STEP 24 checks 2026-10-06, STEP 26 M / M2 2026-10-09]
================================================================================

[ Purpose ]
AnalyzerOutput 의 flat 레이아웃을 **자동 발견**해서 프로세스별로 hadd 한다.
구 merge_submitter.py 의 하드코딩 process 목록(디렉토리 이름과 어긋나면
조용히 빠짐)을 디렉토리 스캔으로 대체 — 목록 관리 불필요.

[ Input layout ]
    <base>/<proc>/<proc>_<N>.root        (submit_job_FH_Tier3_unified.py 산출)
[ Output ]
    <base>/<proc>.root                   (프로세스당 1개)

발견 규칙: <base> 의 하위 디렉토리 d 중 `d.name + "_*.root"` 파일이 1개
이상 있는 것만 대상. 그 외 하위 디렉토리(재제출 분리 폴더, 작업 폴더 등)는
"패턴 미매칭"으로 리포트만 하고 건드리지 않는다.

[ Usage ]
    # 발견 결과만 확인 (아무것도 실행 안 함)
    python3 merge_outputs.py --base /pnfs/.../AnalyzerOutput_main --list

    # 로컬 8-way 병렬 hadd (완료 대기 + 요약표)
    python3 merge_outputs.py --base .../AnalyzerOutput_main --mode local --jobs 8

    # condor 제출 (프로세스당 1 job)
    python3 merge_outputs.py --base .../AnalyzerOutput_main --mode condor

    # [2026-10-06] job 수 대조: analyzer 제출에 쓴 yml 을 주면, 프로세스마다
    # <proc>_0 .. <proc>_<N-1>.root 가 정확히 있는지 본다 (N = 제출기와 같은
    # 나눔: filelist 줄 수 / files_per_job, Data 는 files_per_job_data).
    # 빠진 번호나 남는 파일(예전 제출의 것)이 있으면 그 프로세스는 합치지 않고,
    # worker 도 입력 파일 수가 N 이 아니면 exit 7 로 멈춘다.
    ... --config AnalyzerConfig/Tier3_2024_FH_unified_main.yml [--filelist-dir DIR]
    ... --allow-incomplete              # 그래도 합친다 (권하지 않음)

    # [2026-10-06] condor merge 의 결과: 프로세스마다 가장 최근 시도의 상태
    # (condor 로그의 return value, job .out 의 [hadd] OK, 합친 파일)
    python3 merge_outputs.py --base ... --report [--config yml]
    # 실패했거나 아직 합치지 않은 프로세스만 다시 (대기·실행 중인 것은 그대로)
    python3 merge_outputs.py --base ... --resubmit --mode condor [--config yml]

    # [STEP 26 M2, 2026-10-09] condor merge job 의 시간 한도: 지금 run 이 --time-limit 시간(기본 3)을
    # 넘으면 hold(이유 'tthh time limit: running <분> min (limit <분> min) with <초> s CPU on
    # <slot@machine>', subcode 4202), 5 분 뒤 같은 merge 가 다른 machine 에서 처음부터 다시(hadd -f
    # 가 파일을 새로 만든다), 모두 3 번 시작까지. 10-09: 입력 599 개를 읽던 merge 하나가 /pnfs 에서
    # 멈춘 채 4 시간 'running'(CPU 20 s). CPU 로는 가르지 못한다: 정상 merge 도 CPU 0.0-0.4 %
    # (파일을 열고 읽는 시간이 대부분; KNU condor_history 10-09) — analyzer job 의 stall guard
    # (CPU < 5 %, STEP 25 K2)를 그대로 쓴 M 은 1 시간 넘게 걸리는 정상 merge 를 hold 했을 것이다.
    ... --time-limit 4                  # 한도(시간)
    ... --stall-guard off               # 그 줄들을 쓰지 않는다
    # 합치기 전에(local·condor, --dry-run 도) condor 큐를 본다(condor_q -af:j Args Arguments). 큐의
    # job(어느 상태든, X 도)이 인자로
    #   - 합칠 파일 <base>/<proc>.root 를 가지면 = 같은 프로세스의 merge: 둘이 함께 돌면 늦게 시작한
    #     쪽이 다른 쪽의 파일을 지우고 다시 쓰고, 옛 시도가 새 시도 뒤에 돌다 멈추면 반쪽 파일이
    #     남는데 --report 는 가장 새 시도만 본다;
    #   - 입력 <base>/<proc>/<proc>_<N>.root 를 가지면 = 그 프로세스의 analyzer job: 출력은 job 이
    #     시작할 때 생기므로 파일 수(--config 대조)는 맞아도 덜 쓴 입력이다 (merge 는 analyzer
    #     --report 100 % 뒤에만)
    # 멈춘다(exit 2, 아무것도 쓰지 않음). (--resubmit 은 pending·held 를 고르지 않으므로 merge 쪽에
    # 걸리는 것은 손으로 고른 --only 나 X 로 남은 job 이다.) 상대 경로 인자는 보지 않는다(그 job 의
    # Iwd 기준); 이 스크립트는 --base 를 절대 경로로 바꿔 쓴다.
    # condor --dry-run 의 work 디렉터리에는 dry_run.txt 를 남기고, --report 는 그 디렉터리의 job 중 로그에
    # 사건이 없는 것(= 제출하지 않은 것)은 시도로 보지 않는다(손으로 condor_submit 한 것은 그 로그가 정한다).
    # condor_submit 이 실패하거나 중단되면(Ctrl-C, SIGTERM) submit_failed.txt: 사건이 없는 job 은 failed.
    # 남긴 것: 같은 base 의 merge_outputs.py 둘을 동시에 돌리면 둘 다 큐 확인을 통과할 수 있다(잠금 없음).

    # 필터 / 재실행 제어
    ... --only 'QCD_*' 'TTbar_*'        # glob, 여러 개 가능
    ... --exclude 'SingleMuon_*'
    ... --skip-existing                 # <proc>.root 이미 있으면 건너뜀
                                        #   (실패한 hadd 가 남긴 반쪽 파일도 건너뛰므로
                                        #    재시도에는 --resubmit 을 쓴다)
    ... --dry-run                       # 실행/제출 직전까지만

[ Notes ]
- 실행 단위는 양 모드 모두 run_one_hadd.sh <indir> <outfile> [cmssw_src|-] [N]
  (cmsenv bootstrap, @filelist 로 argv 한계 회피, 결과 sanity 내장; N 이 있으면
  입력 파일 수 대조).
- condor 템플릿은 submit_hadd_validation.py 와 동일 컨벤션
  (x509userproxy, getenv, MY.WantOS, request_memory).
- 로그/제출 파일: <script_dir>/_merge_workdir/<base명>_<timestamp>/
  (arguments.txt 의 줄 번호 = condor ProcId; --report 가 이것을 읽는다)
- 종료 코드: 0 정상; 1 실패한 merge 가 있음(local, --report); 2 잘못된 인자·환경, 큐에 같은 프로세스의
  merge 나 analyzer job 이 있음, condor 큐를 읽지 못함(condor 제출 때), condor_submit 실패(그 시도는
  --report 에서 failed).
================================================================================
"""

from __future__ import annotations

import argparse
import fnmatch
import importlib.util
import math
import os
import re
import shutil
import signal
import subprocess
import sys
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

SCRIPT_DIR         = Path(__file__).resolve().parent
REPO               = SCRIPT_DIR.parent
WORKROOT           = Path(os.environ.get("TTHH_MERGE_WORKROOT", str(SCRIPT_DIR / "_merge_workdir")))  # env: tests
DEFAULT_RUNNER     = SCRIPT_DIR / "run_one_hadd.sh"
DEFAULT_PROXY_PATH = SCRIPT_DIR / "proxy.cert"
DEFAULT_OS_VERSION = "el9"
DEFAULT_MEMORY     = "4GB"
STAMP_RE           = r"\d{8}-\d{6}"
CONDOR_Q           = os.environ.get("TTHH_CONDOR_Q", "condor_q")             # env: tests (fakes)
CONDOR_SUBMIT      = os.environ.get("TTHH_CONDOR_SUBMIT", "condor_submit")


_SUBMITTER = None


def submitter():
    """submit_job_FH_Tier3_unified.py as a module (its yml reader, is_data_name), loaded once and without writing a
    .pyc next to it"""
    global _SUBMITTER
    if _SUBMITTER is None:
        sys.dont_write_bytecode = True
        spec = importlib.util.spec_from_file_location("tthh_submitter", str(REPO / "submit_job_FH_Tier3_unified.py"))
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _SUBMITTER = mod
    return _SUBMITTER


# -----------------------------------------------------------------------------
# Expected job counts from the analyzer yml (the submitter's own reader)
# -----------------------------------------------------------------------------
def expected_jobs(cfg: Path, filelist_dir: str | None) -> dict:
    """{output dir name: (n_jobs, files_per_job, filelist path)} for every sample of the analyzer yml, chunked as
    submit_job_FH_Tier3_unified.py does: lines of <filelist dir>/<filelist> / files_per_job (Data:
    files_per_job_data), the filelist dir defaulting to filelistTier3[_<year>] like the submitter."""
    sub = submitter()
    conf = sub.CondorJobManager.load_yaml_config(None, str(cfg))
    common = conf["common"]
    year = str(common.get("year", "")).strip()
    fl_dir = Path(filelist_dir or ("filelistTier3" + (f"_{year}" if year and year != "2017" else "")))
    if not fl_dir.is_absolute():
        fl_dir = REPO / fl_dir
    out = {}
    for s in conf["samples"]:
        entry = s if isinstance(s, dict) else {"sample_name": str(s)}
        name = entry["sample_name"]
        outdir = str(entry.get("output_dir", name))       # <base>/<outdir>/<outdir>_<N>.root (submitter)
        fl = fl_dir / entry.get("filelist", f"filelist_{name}.txt")
        if sub.is_data_name(name) and common.get("files_per_job_data") not in (None, ""):
            n = int(common["files_per_job_data"])
        else:
            n = int(common.get("files_per_job", 1) or 1)
        with open(fl) as f:
            lines = [l.strip() for l in f if l.strip() and not l.startswith("#")]
        out[outdir] = (math.ceil(len(lines) / n), n, str(fl))
    return out


def job_indices(d: Path, proc: str):
    """(sorted job indices of <proc>_<N>.root, other names matching <proc>_*.root)"""
    idx, odd = [], []
    pat = re.compile(rf"^{re.escape(proc)}_(\d+)\.root$")
    for f in sorted(d.glob(f"{proc}_*.root")):
        m = pat.match(f.name)
        if m:
            idx.append(int(m.group(1)))
        else:
            odd.append(f.name)
    return sorted(idx), odd


def _short(xs, n=8):
    return f"{xs[:n]}{' ...' if len(xs) > n else ''}"


# -----------------------------------------------------------------------------
# Discovery
# -----------------------------------------------------------------------------
def discover(base: Path,
             only: list[str] | None,
             exclude: list[str] | None,
             skip_existing: bool,
             expected: dict | None = None,
             allow_incomplete: bool = False):
    """Return (jobs, skipped, unmatched, absent).

    jobs      : [{proc, indir, outfile, nfiles, expected}] — hadd 대상
    skipped   : [(proc, reason)]                 — 필터/기존 output/job 수 불일치로 제외
    unmatched : [dirname]                        — 패턴 미매칭 하위 디렉토리
    absent    : [proc]                           — yml(--config)에는 있는데 디렉토리가 없음
    """
    jobs, skipped, unmatched = [], [], []
    seen = set()
    for d in sorted(p for p in base.iterdir() if p.is_dir()):
        proc = d.name
        files = sorted(d.glob(f"{proc}_*.root"))
        if not files:
            unmatched.append(proc)
            continue
        seen.add(proc)
        if only and not any(fnmatch.fnmatch(proc, pat) for pat in only):
            skipped.append((proc, "--only 필터"))
            continue
        if exclude and any(fnmatch.fnmatch(proc, pat) for pat in exclude):
            skipped.append((proc, "--exclude 필터"))
            continue
        outfile = base / f"{proc}.root"
        if skip_existing and outfile.exists():
            skipped.append((proc, "output 존재 (--skip-existing)"))
            continue
        n_exp = None
        if expected is not None:
            if proc not in expected:
                skipped.append((proc, "--config 의 yml 에 없는 프로세스"))
                continue
            n_exp = expected[proc][0]
            idx, odd = job_indices(d, proc)
            missing = sorted(set(range(n_exp)) - set(idx))
            extra = [f"{proc}_{i}.root" for i in sorted(set(idx) - set(range(n_exp)))] + odd
            if (missing or extra) and not allow_incomplete:
                why = []
                if missing:
                    why.append(f"job {len(missing)}/{n_exp} 개 없음 {_short(missing)}")
                if extra:
                    why.append(f"남는 파일 {len(extra)} 개 {_short(extra, 4)} (예전 제출?)")
                skipped.append((proc, "INCOMPLETE: " + "; ".join(why)))
                continue
            if missing or extra:
                n_exp = None        # --allow-incomplete: merge what is there, no count check in the worker
        jobs.append({"proc": proc, "indir": str(d),
                     "outfile": str(outfile), "nfiles": len(files), "expected": n_exp})
    absent = []
    if expected is not None:
        for proc in sorted(expected):
            if proc in seen:
                continue
            if only and not any(fnmatch.fnmatch(proc, pat) for pat in only):
                continue
            if exclude and any(fnmatch.fnmatch(proc, pat) for pat in exclude):
                continue
            absent.append(proc)
    return jobs, skipped, unmatched, absent


def print_discovery(base: Path, jobs, skipped, unmatched, absent=()):
    name_w = max([len("process")]
                 + [len(j["proc"]) for j in jobs]
                 + [len(p) for p, _ in skipped] + [len(p) for p in absent] + [7])
    bar = "=" * (name_w + 30)
    print("\n" + bar)
    print(f"merge discovery under: {base}")
    print(bar)
    print(f"{'process':<{name_w}}  {'files':>6}  action")
    print("-" * (name_w + 30))
    for j in jobs:
        chk = f" (= {j['expected']} jobs)" if j.get("expected") is not None else ""
        print(f"{j['proc']:<{name_w}}  {j['nfiles']:>6}  MERGE -> {Path(j['outfile']).name}{chk}")
    for p, why in skipped:
        print(f"{p:<{name_w}}  {'-':>6}  skip ({why})")
    for p in absent:
        print(f"{p:<{name_w}}  {'-':>6}  ABSENT (yml 의 샘플인데 출력 디렉토리가 없음)")
    print("-" * (name_w + 30))
    n_inc = sum(1 for _, why in skipped if why.startswith("INCOMPLETE"))
    print(f"total: merge={len(jobs)}  skip={len(skipped)}  "
          f"unmatched-dirs={len(unmatched)}"
          + (f"  incomplete={n_inc}" if n_inc else "")
          + (f"  absent={len(absent)}" if absent else ""))
    if unmatched:
        print(f"  [note] '<이름>_*.root' 패턴 미매칭 디렉토리 (미대상): "
              f"{unmatched[:8]}{' ...' if len(unmatched) > 8 else ''}")
    if n_inc or absent:
        print("  [check] job 수가 맞지 않는 프로세스는 합치지 않았다: 분석 job 을 --report/--resubmit "
              "으로 채운 뒤 다시 (또는 --allow-incomplete)")
    print(bar)


def runner_args(j, cmssw_src) -> list[str]:
    """run_one_hadd.sh arguments: indir outfile [cmssw_src|-] [expected]"""
    a = [j["indir"], j["outfile"]]
    if cmssw_src or j.get("expected") is not None:
        a.append(str(cmssw_src) if cmssw_src else "-")
    if j.get("expected") is not None:
        a.append(str(j["expected"]))
    return a


# -----------------------------------------------------------------------------
# Local mode — N-way parallel, wait, summary
# -----------------------------------------------------------------------------
def run_local(jobs, runner: Path, n_workers: int, log_dir: Path) -> int:
    log_dir.mkdir(parents=True, exist_ok=True)
    with (log_dir.parent / "arguments.txt").open("w") as f:      # the record --report reads
        for j in jobs:
            f.write(" ".join(runner_args(j, None)) + "\n")

    def one(j):
        t0 = time.time()
        log_path = log_dir / f"{j['proc']}.log"
        with log_path.open("w") as lf:
            proc = subprocess.run([str(runner)] + runner_args(j, None),
                                  stdout=lf, stderr=subprocess.STDOUT)
        dt = time.time() - t0
        out = Path(j["outfile"])
        size = out.stat().st_size if out.exists() else 0
        return (j["proc"], proc.returncode, dt, size, log_path)

    print(f"\n[local] {len(jobs)} merge(s), {n_workers} worker(s) — 완료까지 대기")
    results = []
    with ThreadPoolExecutor(max_workers=n_workers) as ex:
        futs = {ex.submit(one, j): j["proc"] for j in jobs}
        done = 0
        for fut in as_completed(futs):
            r = fut.result()
            results.append(r)
            done += 1
            flag = "OK  " if r[1] == 0 else "FAIL"
            print(f"  [{done}/{len(jobs)}] [{flag}] {r[0]:<28s} "
                  f"{r[2]:7.1f}s  {r[3]/1e6:9.1f} MB")

    n_fail = sum(1 for r in results if r[1] != 0)
    print(f"\n[local] done: {len(results) - n_fail} OK / {n_fail} FAIL"
          f"   (logs: {log_dir}/<proc>.log)")
    if n_fail:
        print("[local] FAILED:", [r[0] for r in results if r[1] != 0])
        print("        재시도: 같은 명령 + --resubmit (실패한 것만; --skip-existing 은 반쪽 파일도 건너뛴다)")
    return 1 if n_fail else 0


# -----------------------------------------------------------------------------
# Condor mode — submit_hadd_validation.py 템플릿 컨벤션
# -----------------------------------------------------------------------------
def claim_workdir(base: Path) -> Path:
    """[STEP 26 M] a new work directory _merge_workdir/<base name>_<YYYYmmdd-HHMMSS>/ for this attempt alone (made
    here, atomically; a second later when the name is taken): two runs in one second used to share one, the later
    one's files over the earlier one's markers and logs"""
    while True:
        wd = WORKROOT / f"{base.name}_{time.strftime('%Y%m%d-%H%M%S')}"
        try:
            wd.mkdir(parents=True)
            return wd
        except FileExistsError:
            time.sleep(1)


# [STEP 26 M2] the time limit of a merge job. A merge that hangs on /pnfs and a healthy one look alike in CPU (both
#   below 1 %: hadd mostly waits for the files), so not the analyzer jobs' CPU stall guard (STEP 25 K2) but the time of
#   the current run: healthy merges take minutes (the largest, ParkingHH_Run2024F with 599 inputs, under an hour), the
#   one that hung on 10-09 was found after 4 h. A restart is safe: hadd -f writes the file anew from the same inputs.
#   tools/runlog/condor_run.sh --time-limit writes the same expressions (test/test_failure_checks.py part I).
MERGE_TIME_LIMIT_H = 3.0            # default --time-limit
TL_HOLD_SUBCODE = 4202              # HoldReasonSubCode of this periodic_hold (4201 = the analyzer jobs' CPU stall guard)
TL_RELEASE_DELAY_S = 300            # 5 min in hold before the release (the stuck process is killed meanwhile)
TL_MAX_STARTS = 3                   # 3 starts in all = 2 automatic restarts
_TL_RUN = "(time() - EnteredCurrentStatus)"
_TL_CPU = ("(ifThenElse(isUndefined(RemoteUserCpu), 0, RemoteUserCpu)"
           " + ifThenElse(isUndefined(RemoteSysCpu), 0, RemoteSysCpu))")
# not again on the machine of the last run: the same text as submit_job_FH_Tier3_unified.py's stall_guard_exprs()
REQ_NOT_LAST_MACHINE = ("isUndefined(LastRemoteHost) || ((LastRemoteHost != TARGET.Machine) && "
                        "(substr(LastRemoteHost, size(LastRemoteHost) - size(TARGET.Machine) - 1) "
                        '!= strcat("@", TARGET.Machine)))')


def time_limit_exprs(limit_s: int) -> dict:
    """{submit command: ClassAd expression} of the time limit (comment above): hold a run longer than limit_s,
    release that hold after TL_RELEASE_DELAY_S while NumJobStarts < TL_MAX_STARTS, avoid the last machine"""
    return {
        "periodic_hold": f"(JobStatus == 2) && ({_TL_RUN} > {int(limit_s)})",
        "periodic_hold_reason": (f'strcat("tthh time limit: running ", string(int({_TL_RUN} / 60)), " min (limit '
                                 f'{int(limit_s) // 60} min) with ", string(int({_TL_CPU})), " s CPU on ", '
                                 f'ifThenElse(isUndefined(RemoteHost), "?", RemoteHost))'),
        "periodic_hold_subcode": str(TL_HOLD_SUBCODE),
        "periodic_release": (f"(HoldReasonCode == 3) && (HoldReasonSubCode == {TL_HOLD_SUBCODE}) "
                             f"&& (NumJobStarts < {TL_MAX_STARTS}) "
                             f"&& ((time() - EnteredCurrentStatus) > {TL_RELEASE_DELAY_S})"),
        "requirements": REQ_NOT_LAST_MACHINE,
    }


def time_limit_summary(hours: float) -> str:
    return (f"hold after {hours:g} h of the current run (any CPU), release after {TL_RELEASE_DELAY_S // 60} min, "
            f"at most {TL_MAX_STARTS} starts, not again on the machine it last ran on")


def time_limit_lines(hours: float) -> list[str]:
    """the submit-file lines of the time limit"""
    return ([f"# time limit [STEP 26 M2]: {time_limit_summary(hours)}"]
            + [f"{k:<23} = {v}" for k, v in time_limit_exprs(round(hours * 3600)).items()])


def write_condor(jobs, runner: Path, workdir: Path, proxy: Path,
                 os_version: str, memory: str,
                 cmssw_src: Path | None, guard_lines=()) -> Path:
    log_dir = workdir / "logs"
    log_dir.mkdir(parents=True, exist_ok=True)
    args_file = workdir / "arguments.txt"
    # [STEP22.1] cmssw_src 가 있으면 3번째 인자로 — run_one_hadd.sh 가 worker
    # 에서 cmsenv. 없으면 '-' (getenv=True 전파에 의존). 4번째 = 기대 입력 수.
    with args_file.open("w") as f:
        for j in jobs:
            f.write(" ".join(runner_args(j, cmssw_src)) + "\n")
    sub = workdir / "merge.sub"
    with sub.open("w") as f:
        f.write(f"x509userproxy           = {proxy}\n")
        f.write("getenv                  = True\n")
        f.write(f"executable              = {runner}\n")
        f.write("arguments               = $(args)\n")
        f.write(f"output                  = {log_dir}/job.$(ClusterId).$(ProcId).out\n")
        f.write(f"error                   = {log_dir}/job.$(ClusterId).$(ProcId).err\n")
        f.write(f"log                     = {log_dir}/log.$(ClusterId).log\n")
        f.write(f"MY.WantOS               = \"{os_version}\"\n")
        f.write(f"request_memory          = {memory}\n")
        for line in guard_lines:                               # [STEP 26 M2] time_limit_lines()
            f.write(line + "\n")
        # transfer 불필요 — PNFS 를 worker 가 직접 읽고 쓴다.
        f.write(f"queue args from {args_file}\n")
    return sub


# -----------------------------------------------------------------------------
# [STEP 26 M] a merge of the same process already in the condor queue
# -----------------------------------------------------------------------------
_REAL = {}


def _same_dir(a: Path, b: Path) -> bool:
    """the same directory: the same normalised path, else the same real path (cached: thousands of queued analyzer jobs
    of other bases share a few directories, and each realpath is a few lstat on /pnfs)"""
    na, nb = os.path.normpath(str(a)), os.path.normpath(str(b))
    if na == nb:
        return True
    for n in (na, nb):
        if n not in _REAL:
            _REAL[n] = os.path.realpath(n)
    return _REAL[na] == _REAL[nb]


def queue_conflicts(jobs):
    """[STEP 26 M] the jobs of this user's condor queue (any state, also X = removed but not yet gone) that touch these
    merges, read from their arguments (condor_q -af:j Args Arguments):
      merges   {outfile: [job id]}  a job naming <base>/<proc>.root      -- a merge of the same process
      writers  {proc: [job id]}     a job naming <base>/<proc>/<proc>_<N>.root -- an analyzer job of the process still
                                    writing an input (its output exists from its start, so the count check passes)
    -> (merges, writers, None), or (None, None, why) when the queue cannot be read. Relative paths in a job's arguments
    are skipped (their base is that job's Iwd); this script writes absolute ones."""
    by_out, by_proc = {}, {}
    for j in jobs:
        by_out.setdefault(Path(j["outfile"]).name, []).append(j)
        by_proc.setdefault(j["proc"], []).append(j)
    try:
        r = subprocess.run([CONDOR_Q, "-af:j", "Args", "Arguments"], capture_output=True, text=True, timeout=300)
    except FileNotFoundError:
        return None, None, f"{CONDOR_Q} not found"
    except (OSError, subprocess.SubprocessError) as e:
        return None, None, f"{CONDOR_Q}: {e}"
    if r.returncode != 0:
        return None, None, f"{CONDOR_Q} exit {r.returncode}: {(r.stderr or r.stdout).strip()[:300]}"
    merges, writers = {}, {}
    out_re = re.compile(r"^(.+)_\d+\.root$")
    for line in r.stdout.splitlines():
        tok = line.split()
        if len(tok) < 2:
            continue
        jid = tok[0]
        for t in tok[1:]:
            if not t.startswith("/"):
                continue
            tp = Path(t)
            for j in by_out.get(tp.name, ()):
                if _same_dir(tp.parent, Path(j["outfile"]).parent) and jid not in merges.setdefault(j["outfile"], []):
                    merges[j["outfile"]].append(jid)
            m = out_re.match(tp.name)
            if m:
                for j in by_proc.get(m.group(1), ()):
                    if _same_dir(tp.parent, Path(j["indir"])) and jid not in writers.setdefault(j["proc"], []):
                        writers[j["proc"]].append(jid)
    return ({k: v for k, v in merges.items() if v}, {k: v for k, v in writers.items() if v}, None)


# -----------------------------------------------------------------------------
# Report — the newest attempt of every process, from the work directories
# -----------------------------------------------------------------------------
_EV_RE = re.compile(r"^(\d{3}) \((\d+)\.(\d+)\.\d+\) ")


def parse_condor_log(path: Path) -> dict:
    """{(cluster, proc): (state, detail)} from an HTCondor user log; state is queued / running / done / removed /
    held, detail the return value ("rc=N") or the signal of a finished job."""
    st = {}
    cur = None
    try:
        lines = path.read_text(errors="replace").splitlines()
    except OSError:
        return st
    for line in lines:
        m = _EV_RE.match(line)
        if m:
            code, key = m.group(1), (int(m.group(2)), int(m.group(3)))
            cur = None
            if code == "000":
                st[key] = ("queued", "")
            elif code == "001":
                st[key] = ("running", "")
            elif code in ("004", "013"):
                st[key] = ("queued", "")
            elif code == "005":
                st[key] = ("done", "")
                cur = key
            elif code == "009":
                st[key] = ("removed", "")
            elif code == "012":
                st[key] = ("held", "")
                cur = key
            continue
        if cur is None:
            continue
        if line.strip() == "...":
            cur = None
            continue
        state = st[cur][0]
        if state == "done":
            r = re.search(r"return value (-?\d+)", line)
            s = re.search(r"Abnormal termination \(signal (\d+)\)", line)
            if r:
                st[cur] = ("done", f"rc={r.group(1)}")
            elif s:
                st[cur] = ("done", f"signal={s.group(1)}")
        elif state == "held" and not st[cur][1] and line.strip():
            st[cur] = ("held", line.strip()[:80])
    return st


def merge_attempts(base: Path) -> dict:
    """{proc: (stamp, kind, state, detail, record path)} -- the newest attempt per process over all work
    directories of this base (_merge_workdir/<base name>_<YYYYmmdd-HHMMSS>/)."""
    best = {}
    if not WORKROOT.is_dir():
        return best
    pat = re.compile(rf"^{re.escape(base.name)}_({STAMP_RE})$")
    wds = []
    for d in WORKROOT.iterdir():
        m = pat.match(d.name)
        if m and d.is_dir():
            wds.append((m.group(1), d))
    for stamp, wd in sorted(wds):
        args = wd / "arguments.txt"
        if not args.is_file():
            continue
        dry_run = (wd / "dry_run.txt").is_file()               # [STEP 26 M] --dry-run: no attempt unless submitted by hand
        rows = [l.split() for l in args.read_text().splitlines() if l.strip()]
        rows = [r for r in rows if len(r) >= 2 and Path(r[0]).parent.resolve() == base.resolve()]
        submit_failed = (wd / "submit_failed.txt").is_file()  # [STEP 26 M] condor_submit exited non-zero
        if (wd / "merge.sub").is_file():                       # condor: line number = ProcId
            ev = {}
            for lg in sorted((wd / "logs").glob("log.*.log")):
                ev.update(parse_condor_log(lg))
            clusters = sorted({c for c, _ in ev})
            for pid, r in enumerate(rows):
                proc = Path(r[0]).name
                # the cluster of this work directory (one submission per directory)
                key = next(((c, pid) for c in reversed(clusters) if (c, pid) in ev), None)
                if key is None:                                # (a job with events: its log decides, below)
                    if dry_run:                                # not submitted: not an attempt
                        continue
                    if submit_failed:                          # nothing queued
                        best[proc] = (stamp, "condor", "failed", f"condor_submit failed ({wd / 'submit_failed.txt'})",
                                      str(wd))
                    else:
                        best[proc] = (stamp, "condor", "queued", "no event in the log yet", str(wd))
                    continue
                state, detail = ev[key]
                out = wd / "logs" / f"job.{key[0]}.{key[1]}.out"
                if state == "done" and detail == "rc=0":
                    try:
                        ok_line = "[hadd] OK" in out.read_text(errors="replace")
                    except OSError:
                        ok_line = True                         # no .out (transfer): trust rc + the file below
                    state = "ok" if ok_line else "failed"
                    detail = f"{key[0]}.{key[1]} rc=0" + ("" if ok_line else " but no '[hadd] OK' in the .out")
                elif state == "done":
                    state, detail = "failed", f"{key[0]}.{key[1]} {detail}  ({out})"
                else:
                    detail = f"{key[0]}.{key[1]} {detail}".strip()
                best[proc] = (stamp, "condor", state, detail, str(wd))
        elif not dry_run:                                      # local: logs/<proc>.log
            for r in rows:
                proc = Path(r[0]).name
                lg = wd / "logs" / f"{proc}.log"
                try:
                    ok = "[hadd] OK" in lg.read_text(errors="replace")
                except OSError:
                    ok = False
                best[proc] = (stamp, "local", "ok" if ok else "failed", f"local log {lg}", str(wd))
    return best


def merge_report(base: Path, expected: dict | None, only=None, exclude=None):
    """[(proc, state, detail)]; state ok / failed / held / pending / not-merged / removed"""
    att = merge_attempts(base)
    procs = set(expected) if expected is not None else set(att)
    if expected is None:
        procs |= {d.name for d in base.iterdir() if d.is_dir() and any(d.glob(f"{d.name}_*.root"))}
    rows = []
    for proc in sorted(procs):
        if only and not any(fnmatch.fnmatch(proc, pat) for pat in only):
            continue
        if exclude and any(fnmatch.fnmatch(proc, pat) for pat in exclude):
            continue
        outfile = base / f"{proc}.root"
        a = att.get(proc)
        if a is None:
            rows.append((proc, "not-merged" if not outfile.exists() else "ok?",
                         "no merge record" + (" (the merged file exists)" if outfile.exists() else "")))
            continue
        stamp, kind, state, detail, wd = a
        if state == "ok" and not (outfile.is_file() and outfile.stat().st_size > 0):
            state, detail = "failed", detail + "; merged file missing or empty"
        if state in ("queued", "running"):
            state = "pending"
        rows.append((proc, state, f"[{stamp}] {detail}"))
    return rows


def print_report(base: Path, rows) -> int:
    name_w = max([len("process")] + [len(r[0]) for r in rows])
    bar = "=" * (name_w + 60)
    print("\n" + bar)
    print(f"merge report: {base}   (records: {WORKROOT}/{base.name}_<stamp>/)")
    print(bar)
    for proc, state, detail in rows:
        if state != "ok":
            print(f"{proc:<{name_w}}  {state.upper():<11} {detail}")
    counts = {}
    for _, s, _ in rows:
        counts[s] = counts.get(s, 0) + 1
    print("-" * (name_w + 60))
    print("TOTAL " + "  ".join(f"{k}={v}" for k, v in sorted(counts.items())) + f"  (of {len(rows)})")
    bad = [r for r in rows if r[1] != "ok"]
    if not bad:
        print("  -> every process merged (newest attempt rc=0, '[hadd] OK', file present).")
    else:
        if any(r[1] in ("failed", "not-merged", "removed", "ok?") for r in bad):
            print("  -> 다시: 같은 명령에 --resubmit --mode condor (failed / not-merged / removed 만)")
        if any(r[1] in ("pending", "held") for r in bad):
            print("  -> pending / held 은 condor_q 로 본다 (held 의 이유가 'tthh time limit' 면 5 분 뒤 저절로 다시 —"
                  " 모두 3 번 시작까지; 그 뒤에도 held 면 condor_rm, condor_q 에서 사라진 뒤 --resubmit)")
    print(bar)
    return 0 if not bad else 1


# -----------------------------------------------------------------------------
def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(
        description="AnalyzerOutput flat 레이아웃 자동 발견 + hadd (local/condor)")
    p.add_argument("--base", type=Path, required=True,
                   help="AnalyzerOutput_main / _btagtrig 등 base 디렉토리")
    p.add_argument("--mode", choices=["local", "condor"], default=None,
                   help="local(병렬 hadd, 대기+요약) / condor(1 proc = 1 job)")
    p.add_argument("--jobs", type=int, default=4,
                   help="[local] 동시 hadd 개수 (기본 4)")
    p.add_argument("--only", nargs="*", default=None,
                   help="glob 필터 — 매칭되는 프로세스만 (예: 'QCD_*' 'TTbar_*')")
    p.add_argument("--exclude", nargs="*", default=None,
                   help="glob 필터 — 매칭되는 프로세스 제외")
    p.add_argument("--skip-existing", action="store_true",
                   help="<proc>.root 가 이미 있으면 건너뜀 (실패한 hadd 의 반쪽 파일도 건너뛴다: "
                        "재시도는 --resubmit)")
    p.add_argument("--config", type=Path, default=None,
                   help="analyzer 제출에 쓴 yml: 프로세스마다 job 수(<proc>_0..N-1)를 대조하고, "
                        "맞지 않으면 합치지 않는다 (worker 도 입력 수 대조)")
    p.add_argument("--filelist-dir", default=None,
                   help="--config 의 filelist 디렉토리 (기본: 제출기와 같이 filelistTier3[_<year>])")
    p.add_argument("--allow-incomplete", action="store_true",
                   help="--config 대조가 맞지 않아도 있는 파일을 합친다 (권하지 않음)")
    p.add_argument("--report", action="store_true",
                   help="합치기 결과: 프로세스마다 가장 최근 시도의 상태 (exit 1 = 정상 아닌 것이 있음)")
    p.add_argument("--resubmit", action="store_true",
                   help="--report 에서 failed / not-merged / removed 인 프로세스만 다시 합친다 "
                        "(--mode 필요; pending·held 는 건드리지 않음)")
    p.add_argument("--list", action="store_true",
                   help="발견 결과 표만 출력하고 종료")
    p.add_argument("--dry-run", action="store_true",
                   help="실행/제출 직전까지만 (condor 는 .sub 생성까지)")
    p.add_argument("--runner", type=Path, default=DEFAULT_RUNNER,
                   help=f"hadd 실행 스크립트 (기본: {DEFAULT_RUNNER})")
    p.add_argument("--proxy", type=Path, default=DEFAULT_PROXY_PATH,
                   help="[condor] x509 proxy 경로")
    p.add_argument("--cmssw-src", type=Path,
                   default=(Path(os.environ["CMSSW_BASE"]) / "src"
                            if os.environ.get("CMSSW_BASE") else None),
                   help="[condor] worker 에서 cmsenv 할 CMSSW src 경로 "
                        "(기본: 제출 셸의 $CMSSW_BASE/src — cmsenv 된 셸에서 "
                        "제출하면 자동. 미지정+미cmsenv 이면 job 은 getenv 에 "
                        "의존하며, hadd 없으면 명확히 실패)")
    p.add_argument("--stall-guard", choices=["on", "off"], default="on",
                   help="[condor] merge job 의 시간 한도 (STEP 26 M2; 기본 on): 지금 run 이 --time-limit 을 넘으면 hold, "
                        "5 분 뒤 다른 machine 에서 처음부터 다시, 모두 3 번 시작까지. CPU 로는 보지 않는다 — 정상 "
                        "merge 도 CPU 1 %% 미만")
    p.add_argument("--time-limit", type=float, default=MERGE_TIME_LIMIT_H, metavar="HOURS",
                   help=f"[condor] 그 한도, 시간 (기본 {MERGE_TIME_LIMIT_H:g}; 정상 merge 는 몇 분, 가장 큰 것도 1 시간 안)")
    p.add_argument("--os-version", default=DEFAULT_OS_VERSION)
    p.add_argument("--memory", default=DEFAULT_MEMORY)
    args = p.parse_args(argv)

    args.base = Path(os.path.abspath(args.base))     # [STEP 26 M] absolute in arguments.txt and the queue check
    if not args.base.is_dir():
        print(f"[fatal] --base 가 디렉토리가 아님: {args.base}")
        return 2
    if not args.runner.is_file():
        print(f"[fatal] runner 없음: {args.runner}")
        return 2
    if args.report and args.resubmit:
        print("[fatal] --report 와 --resubmit 은 따로 쓴다")
        return 2
    if not (math.isfinite(args.time_limit) and round(args.time_limit * 3600) >= 60):
        print(f"[fatal] --time-limit 은 0 보다 큰 시간이어야 한다, 1 분 이상 (받은 값 {args.time_limit:g}; 끄려면 "
              "--stall-guard off)")
        return 2

    expected = None
    if args.config is not None:
        cfg = args.config if args.config.is_file() else REPO / args.config    # as given, or relative to the repo
        try:
            expected = expected_jobs(cfg, args.filelist_dir)
        except (OSError, KeyError, ValueError) as e:
            print(f"[fatal] --config {args.config}: {e}")
            return 2
        print(f"[config] {cfg}: {len(expected)} samples, "
              f"{sum(v[0] for v in expected.values())} analyzer jobs expected")

    if args.report or args.resubmit:
        rows = merge_report(args.base, expected, args.only, args.exclude)
        rc = print_report(args.base, rows)
        if args.report:
            return rc
        redo = sorted(r[0] for r in rows if r[1] in ("failed", "not-merged", "removed", "ok?"))
        if not redo:
            print("[resubmit] 다시 합칠 프로세스 없음.")
            return 0
        print(f"[resubmit] {len(redo)} process(es): {_short(redo)}")
        args.only = redo
        args.skip_existing = False

    jobs, skipped, unmatched, absent = discover(
        args.base, args.only, args.exclude, args.skip_existing, expected, args.allow_incomplete)
    print_discovery(args.base, jobs, skipped, unmatched, absent)

    if args.list:
        return 0
    if not jobs:
        print("[info] 대상 없음 — 종료.")
        return 0
    if args.mode is None:
        print("[fatal] 실행하려면 --mode local|condor 지정 (표만 보려면 --list)")
        return 2

    # [STEP 26 M] no second merge of a process whose merge is in the queue (docstring); local and condor, --dry-run too
    merges, writers, why = queue_conflicts(jobs)
    if merges is None:
        if args.mode == "condor" and not args.dry_run:
            print(f"[fatal] condor 큐를 읽지 못함 ({why}): 이 프로세스들의 merge·analyzer job 이 큐에 있는지 모르는 "
                  "채로는 제출하지 않는다 — 잠시 뒤 다시")
            return 2
        print(f"[note] condor 큐를 읽지 못함 ({why}): 같은 프로세스의 merge·analyzer job 이 큐에 있는지 확인은 건너뜀")
    elif merges or writers:
        if merges:
            print(f"[fatal] 합칠 파일 {len(merges)} 개를 이미 큐의 merge job 이 쓰고 있다 — 같은 프로세스의 merge 둘이 "
                  "함께 돌면 반쪽 파일이 남아도 --report 는 ok 라고 할 수 있다:")
            for o in sorted(merges):
                print(f"        {Path(o).name}: job {' '.join(merges[o])}")
            print("        -> 그 job 이 끝나기를 기다리거나, condor_rm <job> 뒤 condor_q 에서 사라진 것을 보고(X 로 남으면 "
                  "condor_rm -forcex <job>) 같은 명령을 다시 (--resubmit 은 removed·failed 를 고른다)")
        if writers:
            print(f"[fatal] {len(writers)} 프로세스의 analyzer job 이 아직 큐에 있다 — 출력 <proc>_<N>.root 는 job 이 "
                  "시작할 때 생기므로 개수가 맞아도 덜 쓴 입력이다:")
            for pr in sorted(writers):
                ids = writers[pr]
                print(f"        {pr}: job {' '.join(ids[:5])}" + (f" (+{len(ids) - 5})" if len(ids) > 5 else ""))
            print("        -> 그 job 들이 끝나고 analyzer 의 --report 가 100 % 인 뒤에 merge (멈춘 job 은 condor_rm — X 로 "
                  "남으면 condor_rm -forcex — 뒤 analyzer --resubmit)")
        return 2
    else:
        print(f"[queue] 이 {len(jobs)} 프로세스의 merge·analyzer job 은 condor 큐에 없음")

    if args.mode == "local":
        # [STEP22.1] preflight: local 모드의 ROOT/hadd 환경은 사용자 책임 —
        # 스크립트는 환경을 만들지 않는다. 없으면 worker 76개가 같은 이유로
        # 실패하기 전에 여기서 한 번만 명확히 실패한다.
        if shutil.which("hadd") is None:
            print("[fatal] hadd 가 PATH 에 없음 — cmsenv 또는 자체 ROOT 환경을 "
                  "설정한 뒤 실행하세요. (condor 모드는 --cmssw-src 로 worker "
                  "에서 cmsenv 하도록 지정 가능)")
            return 2
        if args.dry_run:
            print("[dry-run] local 실행 생략.")
            return 0
        workdir = claim_workdir(args.base)
        return run_local(jobs, args.runner, max(1, args.jobs),
                         workdir / "logs")

    # condor
    if not args.proxy.is_file():
        print(f"[fatal] proxy 없음: {args.proxy}\n"
              f"        voms-proxy-init --voms cms --valid 168:00 후\n"
              f"        cp $(voms-proxy-info --path) {args.proxy}")
        return 2
    if args.cmssw_src and not args.cmssw_src.is_dir():
        print(f"[fatal] --cmssw-src 가 디렉토리가 아님: {args.cmssw_src}")
        return 2
    guard_lines = time_limit_lines(args.time_limit) if args.stall_guard == "on" else []
    def _term(signum, frame):                                  # SIGTERM (runlog.sh, a lost ssh) as an interrupt
        raise KeyboardInterrupt(f"signal {signum}")
    old_term = signal.signal(signal.SIGTERM, _term)
    workdir = None
    try:
        workdir = claim_workdir(args.base)
        if args.dry_run:                                        # first: a --dry-run directory is never an attempt
            (workdir / "dry_run.txt").write_text("--dry-run: not submitted; --report skips its jobs without an event\n")
        sub = write_condor(jobs, args.runner, workdir, args.proxy,
                           args.os_version, args.memory, args.cmssw_src, guard_lines)
        print(f"[condor] submit file: {sub}  ({len(jobs)} jobs)")
        print("[condor] time limit  : " + (guard_lines[0].split(": ", 1)[1] if guard_lines
                                           else "off (--stall-guard off): 멈춘 merge 는 'running' 으로 남는다"))
        print(f"[condor] worker env : "
              + (f"cmsenv from {args.cmssw_src}" if args.cmssw_src
                 else "getenv 전파만 (cmsenv 셸에서 제출했는지 확인)"))
        if args.dry_run:
            print("[dry-run] condor_submit 생략 (이 work 디렉터리는 --report 가 보지 않는다).")
            return 0
        try:
            rc = subprocess.run([CONDOR_SUBMIT, str(sub)]).returncode
        except OSError as e:
            print(f"[condor] {CONDOR_SUBMIT}: {e}")
            rc = 127
    except KeyboardInterrupt as e:                             # [STEP 26 M] else the attempt reads 'pending' forever
        why = str(e) or "SIGINT"
        if workdir is not None and not args.dry_run:
            (workdir / "submit_failed.txt").write_text(f"submission interrupted ({why}) at "
                                                       f"{time.strftime('%Y-%m-%d %H:%M:%S')}\n")
        print(f"\n[fatal] 제출이 중단됨 ({why}): 큐에 들어간 것이 없으면 --report 는 이 시도를 failed 로 보고 --resubmit 이 "
              "다시 고른다 (condor_q 로 확인; 들어간 job 은 그 로그가 정한다)")
        return 143 if why.startswith("signal") else 130
    finally:
        signal.signal(signal.SIGTERM, old_term)
    if rc != 0:
        # [STEP 26 M] without this the work directory (arguments.txt, merge.sub, no log) reads as 'pending' forever
        (workdir / "submit_failed.txt").write_text(f"condor_submit exit {rc} at {time.strftime('%Y-%m-%d %H:%M:%S')}\n")
        print(f"[fatal] condor_submit exit {rc}: 큐에 들어간 것이 없으면 --report 는 이 시도를 failed 로 보고 "
              "--resubmit 이 다시 고른다 (condor_q 로 확인; 그래도 들어간 job 은 그 로그가 정한다)")
        return 2
    print("[condor] submitted. 상태: --report (또는 condor_q) / 로그:", sub.parent / "logs")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
