#!/bin/sh
# =============================================================================
# run_one_hadd.sh
# -----------------------------------------------------------------------------
# Single-job hadd runner used by submit_hadd_validation.py.
# Invocation:        run_one_hadd.sh <indir> <outfile> [cmssw_src|-] [expected]
#
# <indir>    : directory containing the per-file split outputs to merge
#              (e.g.  <base>/baseline/TTToHadronic/  )
# <outfile>  : full path of the merged target ROOT file
#              (e.g.  <base>/baseline/TTToHadronic.root  )
# cmssw_src  : CMSSW src to cmsenv from on the worker; '-' = none
# expected   : [2026-10-06] the number of analyzer jobs of this process
#              (merge_outputs.py --config); a different number of inputs
#              stops the merge (exit 7) instead of merging a short or
#              doubled set
#
# Behaviour:
#   * sources cmsenv (worker node has CVMFS)
#   * collects <indir>/<name>_*.root (<name> = basename of <indir>, the
#     analyzer's <sample>_<N>.root) EXCEPT the outfile itself
#   * runs `hadd -f -j 1` (single-thread; outer parallelism is via condor)
#   * exits non-zero if hadd fails, if no input files were found, or if
#     their number is not <expected>
#   * [STEP 27 P, 2026-10-10] exits 8 before hadd when the inputs' Tree/Tree
#     branch sets differ (outputs of two analyzer builds, e.g. jobs resubmitted
#     after a rebuild): hadd itself does not fail then -- an older file first
#     gives a merged tree without the new branches, a newer file first drops
#     every entry of the older files ("One of the export top level branches ...
#     is not present in the import TTree") while the histograms still add up
# Exit codes: 2 usage, 3 hadd/root not on PATH, 4 no input directory, 5 no
#   input files, 6 empty result, 7 input count != expected, 8 Tree/Tree branch
#   sets differ (or an input cannot be opened), 9 the branch-set check did not
#   run, else hadd's own
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

if [ $# -lt 2 ] || [ $# -gt 4 ]; then
    echo "[fatal] expected 2-4 args (indir, outfile, [cmssw_src|-], [expected]), got $#"
    exit 2
fi

INDIR="$1"
OUTFILE="$2"
CMSSW_SRC="${3:-}"
if [ "${CMSSW_SRC}" = "-" ]; then CMSSW_SRC=""; fi
EXPECTED="${4:-}"

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

# Build file list — only <name>_*.root (the analyzer's <sample>_<N>.root),
# and skip the outfile so re-runs don't try to feed the merged file back into
# itself.
NAME=$(basename "${INDIR}")
TMPLIST=$(mktemp)
trap 'rm -f "${TMPLIST}"' EXIT

for f in "${INDIR}"/"${NAME}"_*.root; do
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
    echo "[fatal] no input ${NAME}_*.root files in ${INDIR}"
    exit 5
fi
if [ -n "${EXPECTED}" ] && [ "${NFILES}" -ne "${EXPECTED}" ]; then
    echo "[fatal] ${NFILES} input files, ${EXPECTED} expected (the analyzer jobs of this process):"
    echo "        a job output is missing, or files of another submission are in ${INDIR}"
    exit 7
fi

# ── Tree/Tree branch sets [STEP 27 P] ──────────────────────────────────────
#   one set for all inputs (files without a Tree/Tree count as one set of their own); see the header
if ! command -v root >/dev/null 2>&1; then
    echo "[fatal] root not on PATH (needed for the Tree/Tree branch-set check)"
    exit 3
fi
SCHEMA_DIR=$(mktemp -d)
trap 'rm -f "${TMPLIST}"; [ -n "${SCHEMA_DIR}" ] && [ -d "${SCHEMA_DIR}" ] && rm -rf -- "${SCHEMA_DIR}"' EXIT
cat > "${SCHEMA_DIR}/tthh_tree_schema.C" <<'MACRO'
#include <TFile.h>
#include <TTree.h>
#include <TObjArray.h>
#include <TSystem.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <string>
void tthh_tree_schema(const char* list) {
    std::ifstream in(list);
    std::string path, refPath;
    std::set<std::string> ref;
    bool haveRef = false;
    int n = 0, nBad = 0;
    while (std::getline(in, path)) {
        if (path.empty()) continue;
        ++n;
        std::unique_ptr<TFile> f(TFile::Open(path.c_str(), "READ"));
        if (!f || f->IsZombie()) {
            std::cout << "[schema] cannot open " << path << std::endl;
            gSystem->Exit(8);
        }
        std::set<std::string> names;
        TTree* t = nullptr;
        f->GetObject("Tree/Tree", t);
        if (t) {
            TObjArray* br = t->GetListOfBranches();
            for (int i = 0; i < br->GetEntriesFast(); ++i) names.insert(br->At(i)->GetName());
        } else {
            names.insert("(no Tree/Tree)");
        }
        if (!haveRef) { ref = names; refPath = path; haveRef = true; continue; }
        if (names == ref) continue;
        if (++nBad <= 3) {
            std::cout << "[schema] DIFFERS: " << path << " vs " << refPath << ":";
            int shown = 0;
            for (const auto& x : names) if (!ref.count(x) && shown++ < 6) std::cout << " +" << x;
            for (const auto& x : ref) if (!names.count(x) && shown++ < 12) std::cout << " -" << x;
            std::cout << std::endl;
        }
    }
    if (nBad) {
        std::cout << "[schema] " << nBad << " of " << n << " inputs have another Tree/Tree branch set than " << refPath
                  << std::endl;
        gSystem->Exit(8);
    }
    std::cout << "[schema] OK: " << n << " inputs, one Tree/Tree branch set (" << ref.size() << " branches)" << std::endl;
}
MACRO
RCS=0
root -l -b -q "${SCHEMA_DIR}/tthh_tree_schema.C(\"${TMPLIST}\")" > "${SCHEMA_DIR}/out.txt" 2>&1 || RCS=$?
grep '^\[schema\]' "${SCHEMA_DIR}/out.txt" || true
if [ "${RCS}" -eq 8 ]; then
    echo "[fatal] the inputs in ${INDIR} come from analyzer builds with different Tree/Tree branches"
    echo "        (or one cannot be opened): run the jobs of this process again with one executable, then merge"
    exit 8
fi
if [ "${RCS}" -ne 0 ] || ! grep -q '^\[schema\] OK' "${SCHEMA_DIR}/out.txt"; then
    echo "[fatal] the Tree/Tree branch-set check did not run (root exit ${RCS}):"
    tail -5 "${SCHEMA_DIR}/out.txt"
    exit 9
fi

echo "[hadd] inputs: ${NFILES} files${EXPECTED:+ (= ${EXPECTED} expected)}"
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

