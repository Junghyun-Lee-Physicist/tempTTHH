#!/bin/sh
# =============================================================================
# run_one_hadd.sh
# -----------------------------------------------------------------------------
# Single-job hadd runner used by submit_hadd_validation.py.
# Invocation:        run_one_hadd.sh <indir> <outfile> [cmssw_src]
#
# <indir>    : directory containing the per-file split outputs to merge
#              (e.g.  <base>/baseline/TTToHadronic/  )
# <outfile>  : full path of the merged target ROOT file
#              (e.g.  <base>/baseline/TTToHadronic.root  )
#
# Behaviour:
#   * sources cmsenv (worker node has CVMFS)
#   * collects all *.root in <indir> EXCEPT the outfile itself
#   * runs `hadd -f -j 1` (single-thread; outer parallelism is via condor)
#   * exits non-zero if hadd fails or if no input files were found
# =============================================================================
set -e

echo
echo "==============================================================="
echo "  hadd worker"
echo "==============================================================="
echo "  PWD           : ${PWD}"
echo "  hostname      : $(hostname)"
echo "  date          : $(date)"
echo "  argv          : $@"
echo "==============================================================="

if [ $# -lt 2 ] || [ $# -gt 3 ]; then
    echo "[fatal] expected 2-3 args (indir, outfile, [cmssw_src]), got $#"
    exit 2
fi

INDIR="$1"
OUTFILE="$2"
CMSSW_SRC="${3:-}"

# ── ROOT/hadd environment ───────────────────────────────────────────────────
# 정책 [STEP22.1 — 2026-07-12]: 이 스크립트는 환경을 만들지 않는다.
#   local  : 사용자가 cmsenv (또는 자체 ROOT 설치) 로 hadd 를 PATH 에 올려둘 것.
#            없으면 아래에서 명확한 메시지와 함께 실패한다 (자동 복구 없음).
#   condor : merge_outputs.py --cmssw-src <CMSSW>/src 로 경로를 넘기면
#            (3번째 인자) 여기서 cvmfs cmsset + cmsenv 를 수행한다.
# (구 구현의 "hadd 없으면 /tmp 에 scram project" 자동 bootstrap 은 제거 —
#  동시 실행 race 로 2026-07-12 전멸 사례의 원인이었고, 환경 설정 책임은
#  스크립트가 아니라 호출자에게 있다는 정책 결정.)
if [ -n "${CMSSW_SRC}" ]; then
    echo "[env] cmsenv from: ${CMSSW_SRC}"
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    cd "${CMSSW_SRC}"
    eval $(scram runtime -sh)
    cd - >/dev/null
fi

if ! command -v hadd >/dev/null 2>&1; then
    echo "[fatal] hadd not on PATH."
    echo "        local : run cmsenv (or set up your own ROOT) first."
    echo "        condor: pass --cmssw-src <CMSSW>/src to merge_outputs.py."
    exit 3
fi
echo "[env] hadd: $(command -v hadd)"
echo "[env] root: $(command -v root)"

# ── Sanity ───────────────────────────────────────────────────────────────────
if [ ! -d "${INDIR}" ]; then
    echo "[fatal] input directory not found: ${INDIR}"
    exit 4
fi

# Build file list — skip the outfile so re-runs don't try to feed the merged
# file back into itself.
TMPLIST=$(mktemp)
trap 'rm -f "${TMPLIST}"' EXIT

for f in "${INDIR}"/*.root; do
    [ -e "$f" ] || continue
    case "$f" in
        "${OUTFILE}")
            # skip
            ;;
        *)
            echo "$f" >> "${TMPLIST}"
            ;;
    esac
done

NFILES=$(wc -l < "${TMPLIST}")
if [ "${NFILES}" -eq 0 ]; then
    echo "[fatal] no input .root files in ${INDIR}"
    exit 5
fi

echo "[hadd] inputs: ${NFILES} files"
echo "[hadd] target: ${OUTFILE}"
echo "[hadd] first few inputs:"
head -3 "${TMPLIST}"

# ── Make sure target dir exists (it should, since indir is a child of it) ───
OUTDIR=$(dirname "${OUTFILE}")
mkdir -p "${OUTDIR}"

# ── hadd ────────────────────────────────────────────────────────────────────
echo "[hadd] starting"
START=$(date +%s)
# -f       : force overwrite if outfile exists
# -j 1     : single-thread; condor parallelism is across jobs
# @file    : read input list from file (avoids argv length limits for many splits)
hadd -f -j 1 "${OUTFILE}" "@${TMPLIST}"
RC=$?
END=$(date +%s)
ELAPSED=$((END - START))

if [ $RC -ne 0 ]; then
    echo "[fatal] hadd exited with ${RC} after ${ELAPSED}s"
    exit ${RC}
fi

# ── Sanity check on the result ───────────────────────────────────────────────
if [ ! -s "${OUTFILE}" ]; then
    echo "[fatal] outfile is missing or zero-size after hadd: ${OUTFILE}"
    exit 6
fi

OUTSIZE=$(stat -c%s "${OUTFILE}" 2>/dev/null || stat -f%z "${OUTFILE}" 2>/dev/null)
echo "[hadd] OK"
echo "[hadd]   inputs   : ${NFILES} files"
echo "[hadd]   target   : ${OUTFILE}"
echo "[hadd]   size     : ${OUTSIZE} bytes"
echo "[hadd]   elapsed  : ${ELAPSED}s"
echo
exit 0

