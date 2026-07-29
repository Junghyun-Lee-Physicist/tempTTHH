#!/usr/bin/env python3
###############################################################################
# check_ttnb_coverage.py
# -----------------------------------------------------------------------------
# tt+nb patch(ttnb lookup) 가 이번 ntuple 에 **전부** 적용됐는지 판정한다.
#
# 왜 필요한가
# -----------
# patch 는 `(run, luminosityBlock, event)` 로 키가 잡힌다. 그래서 ntuple 을
# 재생산해도(v18 -> v20 처럼 branch 만 늘린 경우) event 집합이 같으면 그대로
# 유효하다. 반대로 입력 dataset 이 바뀌면 키가 안 맞는데, 그때 나타나는 증상은
# 크래시가 아니라 **miss** 다. miss 는 원본 genTtbarId 를 그대로 돌려주므로
# tt+nb(61/62/71/72) 가 조용히 적게 잡히고, 그 아래 b-tag norm reweight 의
# tt+nb 그룹이 통계 부족한 채로 "정상처럼" 유도된다.
#
# analyzer 안의 genTtbarId self-check 는 **matched key 에서만** 돌기 때문에 이
# 경우를 잡지 못한다. 그래서 밖에서 합산 판정이 필요하다.
#
# 판정식
# ------
#     Σ_jobs(hits)  ==  rows          (샘플별, 전 job 합산)
#
# job 하나만 보면 hits < rows 가 정상이다 (job 은 입력 파일 일부만 본다).
# 반드시 한 샘플의 **모든** job 로그를 넣어야 한다.
#
# 사용법
# ------
#   ./tools/check_ttnb_coverage.py condor/filelistTier3_unified_btagtrig/log
#   ./tools/check_ttnb_coverage.py logdir1 logdir2 ... [--glob '*.out']
#
# 종료 코드: 0 = 전 샘플 coverage 100% & mismatch 0, 1 = 문제 있음.
###############################################################################

from __future__ import annotations

import argparse
import re
import sys
from collections import defaultdict
from pathlib import Path

# analyzer 가 printSummary() 에서 찍는 줄:
#   [TTNB_COVERAGE] sample=TTbar_Hadronic rows=25097 hits=131 miss=... calls=... mismatch=0
LINE_RE = re.compile(
    r"\[TTNB_COVERAGE\]\s+sample=(?P<sample>\S+)\s+rows=(?P<rows>\d+)\s+"
    r"hits=(?P<hits>\d+)\s+miss=(?P<miss>\d+)\s+calls=(?P<calls>\d+)\s+"
    r"mismatch=(?P<mismatch>\d+)"
)


def scan(paths: list[Path], pattern: str):
    """Return {sample: {"rows": set, "hits": int, "mismatch": int, "jobs": int}}."""
    agg: dict[str, dict] = defaultdict(
        lambda: {"rows": set(), "hits": 0, "mismatch": 0, "jobs": 0, "calls": 0}
    )
    n_files = 0

    for p in paths:
        files = sorted(p.rglob(pattern)) if p.is_dir() else [p]
        for f in files:
            try:
                text = f.read_text(errors="replace")
            except OSError:
                continue
            found = False
            for m in LINE_RE.finditer(text):
                found = True
                s = agg[m["sample"]]
                s["rows"].add(int(m["rows"]))
                s["hits"] += int(m["hits"])
                s["calls"] += int(m["calls"])
                s["mismatch"] += int(m["mismatch"])
                s["jobs"] += 1
            if found:
                n_files += 1

    return agg, n_files


def main() -> int:
    ap = argparse.ArgumentParser(
        description="tt+nb patch coverage 판정 (Σ hits == rows)"
    )
    ap.add_argument("paths", nargs="+", type=Path, help="로그 파일 또는 디렉토리")
    ap.add_argument("--glob", default="*.out", help="디렉토리 스캔 패턴 (기본 *.out)")
    ap.add_argument(
        "--tolerate", type=float, default=0.0,
        help="허용 미적용 비율 [%%] (기본 0 = 완전 일치 요구)",
    )
    args = ap.parse_args()

    agg, n_files = scan(args.paths, args.glob)

    if not agg:
        print("[FATAL] '[TTNB_COVERAGE]' 줄을 하나도 못 찾았다.", file=sys.stderr)
        print("  - analyzer 가 이 줄을 찍는 버전인지 확인 "
              "(src/ExpandedTtbarId.cc, 2026-07-29 이후)", file=sys.stderr)
        print(f"  - 스캔 패턴이 맞는지 확인 (--glob {args.glob!r})", file=sys.stderr)
        return 1

    print(f"scanned {n_files} log file(s)\n")
    hdr = f"{'sample':22s} {'jobs':>5s} {'rows':>10s} {'Σ hits':>10s} {'cover%':>8s} {'mismatch':>9s}  verdict"
    print(hdr)
    print("-" * len(hdr))

    bad = False
    for sample in sorted(agg):
        s = agg[sample]

        # rows 는 lookup 파일의 성질이므로 전 job 에서 같아야 한다.
        if len(s["rows"]) != 1:
            print(f"{sample:22s} {s['jobs']:5d} {'MIXED':>10s} {s['hits']:10d} "
                  f"{'-':>8s} {s['mismatch']:9d}  ✗ rows 값이 job 마다 다르다 "
                  f"{sorted(s['rows'])} -- 서로 다른 lookup 파일이 섞였다")
            bad = True
            continue

        rows = next(iter(s["rows"]))
        cover = 100.0 * s["hits"] / rows if rows else 0.0

        verdict = "✓ OK"
        if s["mismatch"] != 0:
            verdict = "✗ genTtbarId mismatch -- 이 샘플의 patch 가 아니다"
            bad = True
        elif s["hits"] > rows:
            verdict = "✗ hits > rows -- 중복 key / event 중복 처리"
            bad = True
        elif cover < 100.0 - args.tolerate:
            missing = rows - s["hits"]
            verdict = (f"✗ {missing:,} row 미적용 -- job 누락이거나 "
                       f"입력 dataset 이 patch 와 다르다")
            bad = True

        print(f"{sample:22s} {s['jobs']:5d} {rows:10,d} {s['hits']:10,d} "
              f"{cover:7.3f}% {s['mismatch']:9d}  {verdict}")

    print()
    if bad:
        print("RESULT: 문제 있음.")
        print("  · coverage < 100%  → 먼저 **전 job 로그를 넣었는지** 확인할 것.")
        print("    (job 하나만 보면 hits < rows 가 정상이다.)")
        print("    전부 넣었는데도 모자라면, 이번 ntuple 의 event 집합이 patch 를")
        print("    뽑을 때의 집합과 다르다는 뜻이다 → patch 재추출 필요.")
        print("  · mismatch > 0    → 샘플에 엉뚱한 patch 파일이 붙었다.")
        return 1

    print("RESULT: 전 샘플 coverage 100%, mismatch 0 — patch 가 이 ntuple 에 완전 적용됨.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
