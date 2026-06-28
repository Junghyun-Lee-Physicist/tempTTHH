#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
[임시] 완료 확인 스크립트 — analyzer 수정 없이 "정상 완주 vs 덜 돌다 죽음" 을 구분.

배경:
  기존 submitter 의 완료 판정은 "Tree/<tree> 의 entry > 0" 을 요구한다. 그런데
  lepton CR + QCD 처럼 통과 이벤트가 0 이 정상인 조합은 entry=0 이라 '미완료'로
  오판된다(영원히 resubmit 대상). 반대로 중간에 죽은 job 도 entry=0 이라 둘을
  구분 못 한다.

이 스크립트의 판정 기준 (analyzer 출력 구조 활용):
  - 정상 완주하면 Tree/cutflow 히스토그램의 첫 bin(noCut)에 '처리한 입력 이벤트
    수'가 들어간다(>0). entry 가 0 이어도 cutflow.noCut 은 입력 이벤트 수 그대로.
  - 따라서:
      파일없음/zombie            -> DEAD     (진짜 재제출 필요)
      Tree/cutflow.noCut > 0     -> OK       (정상 완주; entry 0 이어도 complete)
      cutflow 없음 / noCut == 0  -> SUSPECT  (입력 0 이거나 중간에 죽음 — 확인 필요)

  ※ 이건 임시 방편이다. 근본 해결은 analyzer 가 정상 종료 시 마커(예: nProcessed
    스칼라 / exit code)를 남기고 report 가 그것을 집계하는 것 — 다음 작업으로.

사용:
  cmsenv (PyROOT 필요) 후:
    python3 check_done_tmp.py \
        --base /pnfs/.../AnalyzerOutput_main_electron \
        --filelist-dir filelistTest_or_master_dir \
        --files-per-job 5
  또는 단순히 output 디렉토리만 스캔(파일 인덱스 자동 탐색):
    python3 check_done_tmp.py --base /pnfs/.../AnalyzerOutput_main_electron --scan
"""
from __future__ import annotations

import argparse
import os
import re
import sys
from pathlib import Path

try:
    import ROOT
    ROOT.gErrorIgnoreLevel = ROOT.kError  # Warning(자동 recover 등) 억제
    ROOT.gROOT.SetBatch(True)
except Exception as e:  # noqa: BLE001
    print(f"[FATAL] PyROOT import 실패 — cmsenv 안에서 실행하세요. ({e})")
    sys.exit(2)


# 분석 tree 가 들어있는 디렉토리 이름 (analyzer output 구조)
TREE_DIR = "Tree"
CUTFLOW_NAME = "cutflow"   # Tree/cutflow (raw count). noCut = bin 1.


def classify(root_path: str) -> tuple[str, str]:
    """한 output ROOT 파일을 분류. return (status, detail).

    status: OK | DEAD | SUSPECT
    """
    if not os.path.isfile(root_path):
        return "DEAD", "file absent"

    f = ROOT.TFile.Open(root_path, "READ")
    try:
        if not f or f.IsZombie():
            return "DEAD", "zombie / unopenable"
        # 자동 복구된 파일은 비정상 종료였다는 신호 — SUSPECT 로 본다
        if f.TestBit(ROOT.TFile.kRecovered):
            return "SUSPECT", "ROOT recovered (abnormal close)"

        d = f.Get(TREE_DIR)
        if not d or not d.InheritsFrom("TDirectory"):
            return "SUSPECT", f"no '{TREE_DIR}/' directory"

        cf = d.Get(CUTFLOW_NAME)
        if not cf or not cf.InheritsFrom("TH1"):
            return "SUSPECT", f"no '{TREE_DIR}/{CUTFLOW_NAME}' hist"

        nocut = cf.GetBinContent(1)   # bin 1 = noCut
        if nocut > 0:
            # 정상 완주. tree entry 는 0 이어도 됨(CR 통과 0 등).
            tree = d.Get("Tree")
            ent = int(tree.GetEntries()) if (tree and tree.InheritsFrom("TTree")) else -1
            return "OK", f"noCut={int(nocut)} treeEntries={ent}"
        else:
            return "SUSPECT", "cutflow.noCut == 0 (input 0 또는 중간 종료)"
    finally:
        if f:
            f.Close()


def find_indices_by_scan(sample_dir: str) -> list[int]:
    """output 디렉토리에서 <sample>_<idx>.root 의 idx 들을 모은다."""
    idxs = []
    if not os.path.isdir(sample_dir):
        return idxs
    pat = re.compile(r"_(\d+)\.root$")
    for name in os.listdir(sample_dir):
        m = pat.search(name)
        if m:
            idxs.append(int(m.group(1)))
    return sorted(idxs)


def main() -> None:
    ap = argparse.ArgumentParser(description="[임시] cutflow 기반 완료 확인")
    ap.add_argument("--base", required=True,
                    help="AnalyzerOutput_<...> 경로 (region/SF 포함된 디렉토리)")
    ap.add_argument("--sample", default="",
                    help="특정 샘플만 (생략 시 base 하위 모든 샘플 디렉토리)")
    ap.add_argument("--scan", action="store_true",
                    help="output 디렉토리에 실제 존재하는 _<idx>.root 만 스캔"
                         " (filelist 없이 빠르게 현황 보기)")
    ap.add_argument("--show-ok", action="store_true",
                    help="OK 인 파일도 한 줄씩 출력 (기본은 요약만)")
    args = ap.parse_args()

    base = args.base.rstrip("/")
    if not os.path.isdir(base):
        print(f"[FATAL] base 디렉토리 없음: {base}")
        sys.exit(2)

    # 대상 샘플 디렉토리 목록
    if args.sample:
        samples = [args.sample]
    else:
        samples = sorted(
            d for d in os.listdir(base)
            if os.path.isdir(os.path.join(base, d))
        )

    grand = {"OK": 0, "DEAD": 0, "SUSPECT": 0}
    print(f"\n=== [임시 완료확인] base = {base} ===")
    print("(판정: OK=정상완주(entry0 허용)  DEAD=파일없음/깨짐  "
          "SUSPECT=cutflow빔/noCut0/recover)\n")

    for s in samples:
        sdir = os.path.join(base, s)
        idxs = find_indices_by_scan(sdir)
        if not idxs:
            print(f"  {s:22s} : (output 파일 없음)")
            continue

        cnt = {"OK": 0, "DEAD": 0, "SUSPECT": 0}
        bad_lines = []
        for i in idxs:
            rp = os.path.join(sdir, f"{s}_{i}.root")
            status, detail = classify(rp)
            cnt[status] += 1
            grand[status] += 1
            if status != "OK":
                bad_lines.append(f"      idx {i:4d}  [{status}]  {detail}")
            elif args.show_ok:
                bad_lines.append(f"      idx {i:4d}  [OK]    {detail}")

        tag = "" if (cnt["DEAD"] == 0 and cnt["SUSPECT"] == 0) else "  <-- 확인필요"
        print(f"  {s:22s} : total={len(idxs):4d}  "
              f"OK={cnt['OK']:4d}  DEAD={cnt['DEAD']:3d}  "
              f"SUSPECT={cnt['SUSPECT']:3d}{tag}")
        for ln in bad_lines:
            print(ln)

    print(f"\n=== 합계: OK={grand['OK']}  DEAD={grand['DEAD']}  "
          f"SUSPECT={grand['SUSPECT']} ===")
    print("  - OK      : 정상 완주(통과 이벤트 0 이어도 정상). 재제출 불필요.")
    print("  - DEAD    : 파일 없음/깨짐 → 재제출 필요.")
    print("  - SUSPECT : cutflow 비었거나 noCut=0 → 입력 0 이거나 중간 종료."
          " .out/.err 로그 확인 권장.\n")


if __name__ == "__main__":
    main()
