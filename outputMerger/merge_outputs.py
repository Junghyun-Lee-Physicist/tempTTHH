#!/usr/bin/env python3
"""
================================================================================
SCRIPT: merge_outputs.py   [STEP22]
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

    # 필터 / 재실행 제어
    ... --only 'QCD_*' 'TTbar_*'        # glob, 여러 개 가능
    ... --exclude 'SingleMuon_*'
    ... --skip-existing                 # <proc>.root 이미 있으면 건너뜀
    ... --dry-run                       # 실행/제출 직전까지만

[ Notes ]
- 실행 단위는 양 모드 모두 run_one_hadd.sh <indir> <outfile>
  (cmsenv bootstrap, @filelist 로 argv 한계 회피, 결과 sanity 내장).
- condor 템플릿은 submit_hadd_validation.py 와 동일 컨벤션
  (x509userproxy, getenv, MY.WantOS, request_memory).
- 로그/제출 파일: <script_dir>/_merge_workdir/<base명>_<timestamp>/
================================================================================
"""

from __future__ import annotations

import argparse
import fnmatch
import os
import shutil
import subprocess
import sys
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

SCRIPT_DIR         = Path(__file__).resolve().parent
DEFAULT_RUNNER     = SCRIPT_DIR / "run_one_hadd.sh"
DEFAULT_PROXY_PATH = SCRIPT_DIR / "proxy.cert"
DEFAULT_OS_VERSION = "el9"
DEFAULT_MEMORY     = "4GB"


# -----------------------------------------------------------------------------
# Discovery
# -----------------------------------------------------------------------------
def discover(base: Path,
             only: list[str] | None,
             exclude: list[str] | None,
             skip_existing: bool):
    """Return (jobs, skipped, unmatched).

    jobs      : [{proc, indir, outfile, nfiles}] — hadd 대상
    skipped   : [(proc, reason)]                 — 필터/기존 output 으로 제외
    unmatched : [dirname]                        — 패턴 미매칭 하위 디렉토리
    """
    jobs, skipped, unmatched = [], [], []
    for d in sorted(p for p in base.iterdir() if p.is_dir()):
        proc = d.name
        files = sorted(d.glob(f"{proc}_*.root"))
        if not files:
            unmatched.append(proc)
            continue
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
        jobs.append({"proc": proc, "indir": str(d),
                     "outfile": str(outfile), "nfiles": len(files)})
    return jobs, skipped, unmatched


def print_discovery(base: Path, jobs, skipped, unmatched):
    name_w = max([len("process")]
                 + [len(j["proc"]) for j in jobs]
                 + [len(p) for p, _ in skipped] + [7])
    bar = "=" * (name_w + 30)
    print("\n" + bar)
    print(f"merge discovery under: {base}")
    print(bar)
    print(f"{'process':<{name_w}}  {'files':>6}  action")
    print("-" * (name_w + 30))
    for j in jobs:
        print(f"{j['proc']:<{name_w}}  {j['nfiles']:>6}  MERGE -> {Path(j['outfile']).name}")
    for p, why in skipped:
        print(f"{p:<{name_w}}  {'-':>6}  skip ({why})")
    print("-" * (name_w + 30))
    print(f"total: merge={len(jobs)}  skip={len(skipped)}  "
          f"unmatched-dirs={len(unmatched)}")
    if unmatched:
        print(f"  [note] '<이름>_*.root' 패턴 미매칭 디렉토리 (미대상): "
              f"{unmatched[:8]}{' ...' if len(unmatched) > 8 else ''}")
    print(bar)


# -----------------------------------------------------------------------------
# Local mode — N-way parallel, wait, summary
# -----------------------------------------------------------------------------
def run_local(jobs, runner: Path, n_workers: int, log_dir: Path) -> int:
    log_dir.mkdir(parents=True, exist_ok=True)

    def one(j):
        t0 = time.time()
        log_path = log_dir / f"{j['proc']}.log"
        with log_path.open("w") as lf:
            proc = subprocess.run([str(runner), j["indir"], j["outfile"]],
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
        print("        재시도: 같은 명령 + --skip-existing (성공분 자동 제외)")
    return 1 if n_fail else 0


# -----------------------------------------------------------------------------
# Condor mode — submit_hadd_validation.py 템플릿 컨벤션
# -----------------------------------------------------------------------------
def write_condor(jobs, runner: Path, workdir: Path, proxy: Path,
                 os_version: str, memory: str,
                 cmssw_src: Path | None) -> Path:
    log_dir = workdir / "logs"
    log_dir.mkdir(parents=True, exist_ok=True)
    args_file = workdir / "arguments.txt"
    # [STEP22.1] cmssw_src 가 있으면 3번째 인자로 — run_one_hadd.sh 가 worker
    # 에서 cmsenv. 없으면 2-인자 (getenv=True 전파에 의존).
    tail = f" {cmssw_src}" if cmssw_src else ""
    with args_file.open("w") as f:
        for j in jobs:
            f.write(f"{j['indir']} {j['outfile']}{tail}\n")
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
        # transfer 불필요 — PNFS 를 worker 가 직접 읽고 쓴다.
        f.write(f"queue args from {args_file}\n")
    return sub


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
                   help="<proc>.root 가 이미 있으면 건너뜀 (재시도용)")
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
    p.add_argument("--os-version", default=DEFAULT_OS_VERSION)
    p.add_argument("--memory", default=DEFAULT_MEMORY)
    args = p.parse_args(argv)

    if not args.base.is_dir():
        print(f"[fatal] --base 가 디렉토리가 아님: {args.base}")
        return 2
    if not args.runner.is_file():
        print(f"[fatal] runner 없음: {args.runner}")
        return 2

    jobs, skipped, unmatched = discover(
        args.base, args.only, args.exclude, args.skip_existing)
    print_discovery(args.base, jobs, skipped, unmatched)

    if args.list:
        return 0
    if not jobs:
        print("[info] 대상 없음 — 종료.")
        return 0
    if args.mode is None:
        print("[fatal] 실행하려면 --mode local|condor 지정 (표만 보려면 --list)")
        return 2

    stamp = time.strftime("%Y%m%d-%H%M%S")
    workdir = SCRIPT_DIR / "_merge_workdir" / f"{args.base.name}_{stamp}"

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
    sub = write_condor(jobs, args.runner, workdir, args.proxy,
                       args.os_version, args.memory, args.cmssw_src)
    print(f"[condor] submit file: {sub}  ({len(jobs)} jobs)")
    print(f"[condor] worker env : "
          + (f"cmsenv from {args.cmssw_src}" if args.cmssw_src
             else "getenv 전파만 (cmsenv 셸에서 제출했는지 확인)"))
    if args.dry_run:
        print("[dry-run] condor_submit 생략.")
        return 0
    subprocess.run(["condor_submit", str(sub)], check=True)
    print("[condor] submitted. 상태: condor_q / 로그:", sub.parent / "logs")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
