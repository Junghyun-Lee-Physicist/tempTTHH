#!/bin/bash
# =============================================================================
#  build_check.sh -- clean build of the analyzer with a short report, or the
#  same report for a build that already ran.  (STEP 23, 2026-10-04)
#
#    /bin/bash tools/stage0/build_check.sh [-j N]             make clean && make -j N (default 4)
#    /bin/bash tools/stage0/build_check.sh --from-log <file>  no build: report on that make log
#
#  Why clean: the object rules of the top-level Makefile depend on the .cc
#  only, so an edit of a header (eventBuffer.h, tnm.h, ttHHanalyzer_unified.h)
#  does not make anything rebuild, and a plain `make` can leave objects built
#  with the old header (docs/PLAN_v15_2018UL_2024.md section 9.3).
#
#  Report: make's exit code (build mode), the number of 'warning:' and
#  'error:' lines, the first error with context, the executable and lib/ with
#  their times, md5 of include/eventBuffer.h and src/treestream.cc.
#  The full make output goes to condor/build/build_<UTC>.log (gitignored;
#  thousands of warning lines from the generated eventBuffer.h are normal).
#  Exit: make's exit code (build mode); --from-log: 1 if the log has 'error:'
#  lines, else 0.
# =============================================================================
set -u
REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd -P)"
cd "$REPO" || exit 2

NJ=4; FROM=""
while [[ $# -gt 0 ]]; do
  case "$1" in
    -j) [[ $# -ge 2 && "$2" =~ ^[1-9][0-9]*$ ]] || { echo "build_check.sh: -j needs a positive integer" >&2; exit 2; }; NJ="$2"; shift 2 ;;
    --from-log) [[ $# -ge 2 ]] || { echo "build_check.sh: --from-log needs a file" >&2; exit 2; }; FROM="$2"; shift 2 ;;
    -h|--help) sed -n '2,21p' "$0" | sed 's/^# \{0,2\}//'; exit 0 ;;
    *) echo "build_check.sh: unknown argument $1" >&2; exit 2 ;;
  esac
done

RC=""
if [[ -n "$FROM" ]]; then
  [[ -r "$FROM" ]] || { echo "build_check.sh: cannot read $FROM" >&2; exit 2; }
  BLOG="$FROM"
  echo "mode        : report on an earlier build log (no build)"
else
  mkdir -p "$REPO/condor/build" || exit 2
  BLOG="$REPO/condor/build/build_$(date -u +%Y%m%d_%H%M%S).log"
  [[ -e "$BLOG" ]] && BLOG="${BLOG%.log}_$$.log"
  echo "mode        : make clean && make -j$NJ   (in $REPO)"
  echo "start_utc   : $(date -u +%FT%TZ)"
  { make clean && make -j"$NJ"; } > "$BLOG" 2>&1
  RC=$?
  echo "end_utc     : $(date -u +%FT%TZ)"
  echo "make_rc     : $RC"
fi

NL=$(wc -l < "$BLOG" | tr -d ' ')
NW=$(grep -c 'warning:' "$BLOG")
NE=$(grep -c -E 'error:|Error [0-9]+' "$BLOG")
echo "make_log    : ${BLOG#$REPO/} ($NL lines)"
echo "warnings    : $NW lines with 'warning:'"
echo "errors      : $NE lines with 'error:' or 'Error <n>'"
if [[ "$NE" -gt 0 ]]; then
  echo "--- first error (2 lines before, 25 after) ---"
  grep -n -m1 -B2 -A25 -E 'error:|Error [0-9]+' "$BLOG"
  echo "--- end ---"
fi
echo "--- products ---"
ls -l --time-style=full-iso ttHHanalyzer_unified 2>&1
if [[ -d lib ]]; then ls -l --time-style=full-iso lib/ | tail -n +2; else echo "lib/: missing"; fi
echo "--- headers (md5) ---"
md5sum include/eventBuffer.h src/treestream.cc 2>&1

if [[ -n "$FROM" ]]; then
  [[ "$NE" -gt 0 ]] && exit 1
  exit 0
fi
exit "$RC"
