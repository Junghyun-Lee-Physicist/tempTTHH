#!/bin/sh
# =============================================================================
# run_one_hadd.sh
# -----------------------------------------------------------------------------
# Single-job hadd runner used by submit_hadd_validation.py.
# Condor invokes:    run_one_hadd.sh <indir> <outfile>
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

if [ $# -ne 2 ]; then
    echo "[fatal] expected 2 args (indir, outfile), got $#"
    exit 2
fi

INDIR="$1"
OUTFILE="$2"

# ── CMSSW environment ────────────────────────────────────────────────────────
echo "[env] sourcing cmsset_default.sh"
source /cvmfs/cms.cern.ch/cmsset_default.sh

# ── Verify ROOT/hadd availability ────────────────────────────────────────────
if ! command -v hadd >/dev/null 2>&1; then
    echo "[env] hadd not on PATH; bootstrapping a CMSSW area"
    # Use a CMSSW release available on cvmfs to get ROOT into PATH.
    # Adjust the release here if KISTI uses a different default.
    cd /tmp
    if [ ! -d CMSSW_14_2_1 ]; then
        scram project CMSSW CMSSW_14_2_1
    fi
    cd CMSSW_14_2_1/src
    eval $(scram runtime -sh)
fi

if ! command -v hadd >/dev/null 2>&1; then
    echo "[fatal] hadd still not available after env setup"
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

