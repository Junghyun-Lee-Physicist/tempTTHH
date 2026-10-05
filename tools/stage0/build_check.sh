#!/bin/bash
# =============================================================================
#  build_check.sh -- clean build of the analyzer with a short report, or the
#  same report for a build that already ran.  (STEP 23, 2026-10-04)
#
#    /bin/bash tools/stage0/build_check.sh [-j N]             make clean && make -j N VERBOSE=1 (default 4)
#    /bin/bash tools/stage0/build_check.sh --from-log <file>  no build: report on that make log
#
#  Why clean: the object rules of the top-level Makefile depend on the .cc
#  only, so an edit of a header (eventBuffer.h, tnm.h, ttHHanalyzer_unified.h)
#  does not make anything rebuild, and a plain `make` can leave objects built
#  with the old header (docs/PLAN_v15_2018UL_2024.md section 9.3).
#
#  Report: make's exit code (build mode), the number of error lines ('error:'
#  and make's own '***' lines -- Error n, Stop, Interrupt; source lines that
#  the compiler echoes under a diagnostic are not counted) and of 'warning:'
#  lines, the first error with context, the last lines of the make log (an
#  interrupted build can end without any error line), whether the log has the
#  Makefile's "[Done] Built" line (printed with VERBOSE=1), the executable and
#  lib/ with their times, md5 of include/eventBuffer.h and src/treestream.cc,
#  and a closing 'result' line. The full make output goes to
#  condor/build/build_<UTC>.log (gitignored; thousands of warnings are normal).
#  Exit 0 only when the build is shown to be complete:
#    build mode : make exited 0 and the executable ttHHanalyzer_unified exists
#                 (else make's own code when it failed, otherwise 1);
#    --from-log : the log is not empty and has no error line, the executable
#                 exists, and -- a VERBOSE log -- it has the "[Done] Built"
#                 line and the executable is not >10 min older than the log;
#                 -- a quiet log (no way to see the end) -- the executable's
#                 time is within 10 min of the log's last write. Else 1.
#  Bad usage: 2.
#  (Until 2026-10-04 (2) --from-log looked at 'error:' lines only, and the
#  KNU build of 10-03, interrupted after lib/, was reported with EXIT 0.)
# =============================================================================
set -u
APP="ttHHanalyzer_unified"
SELF="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)/$(basename "${BASH_SOURCE[0]}")"
REPO="$(cd "$(dirname "$SELF")/../.." && pwd -P)"

NJ=4; FROM=""
while [[ $# -gt 0 ]]; do
  case "$1" in
    -j) [[ $# -ge 2 && "$2" =~ ^[1-9][0-9]*$ ]] || { echo "build_check.sh: -j needs a positive integer" >&2; exit 2; }; NJ="$2"; shift 2 ;;
    --from-log) [[ $# -ge 2 && -n "$2" ]] || { echo "build_check.sh: --from-log needs a file name" >&2; exit 2; }; FROM="$2"; shift 2 ;;
    -h|--help) sed -n '2,34p' "$SELF" | sed 's/^# \{0,2\}//'; exit 0 ;;
    *) echo "build_check.sh: unknown argument $1" >&2; exit 2 ;;
  esac
done
if [[ -n "$FROM" ]]; then
  [[ -f "$FROM" && -r "$FROM" ]] || { echo "build_check.sh: not a readable file: $FROM" >&2; exit 2; }
  FROM="$(cd "$(dirname "$FROM")" && pwd -P)/$(basename "$FROM")"     # before the cd below
fi
cd "$REPO" || exit 2

RC=""
if [[ -n "$FROM" ]]; then
  BLOG="$FROM"
  echo "mode        : report on an earlier build log (no build)"
else
  mkdir -p "$REPO/condor/build" || exit 2
  BLOG="$REPO/condor/build/build_$(date -u +%Y%m%d_%H%M%S).log"
  [[ -e "$BLOG" ]] && BLOG="${BLOG%.log}_$$.log"
  echo "mode        : make clean && make -j$NJ VERBOSE=1   (in $REPO)"
  echo "start_utc   : $(date -u +%FT%TZ)"
  { make clean && make -j"$NJ" VERBOSE=1; } > "$BLOG" 2>&1
  RC=$?
  echo "end_utc     : $(date -u +%FT%TZ)"
  echo "make_rc     : $RC"
fi

# error: compiler/linker 'error:' and make's own '*** ...' lines; not counted:
# the source lines a compiler echoes under a diagnostic ("  10 |   code")
ERRPAT='error:|^(make(\[[0-9]+\])?|[^ :]*[Mm]akefile[^ :]*:[0-9]+): \*\*\*'
ECHOPAT='^ *[0-9]* [|] '
NL=$(wc -l < "$BLOG" | tr -d ' ')
NW=$(grep -v -E "$ECHOPAT" "$BLOG" | grep -c 'warning:')
NE=$(grep -v -E "$ECHOPAT" "$BLOG" | grep -c -E "$ERRPAT")
VERB=0; grep -q 'Verbose log output enabled' "$BLOG" && VERB=1
DONE=0; grep -q '^\[Done\] Built' "$BLOG" && DONE=1
echo "make_log    : ${BLOG#$REPO/} ($NL lines)"
echo "warnings    : $NW lines with 'warning:'"
echo "errors      : $NE lines with 'error:' or make's '***'"
if [[ "$NE" -gt 0 ]]; then
  FL=$(PAT="$ERRPAT" ECH="$ECHOPAT" awk '$0 ~ ENVIRON["PAT"] && $0 !~ ENVIRON["ECH"] {print NR; exit}' "$BLOG")
  echo "--- first error (2 lines before, 25 after; line numbers of the log) ---"
  awk -v a=$((FL > 2 ? FL - 2 : 1)) -v b=$((FL + 25)) 'NR >= a && NR <= b {printf "%d: %s\n", NR, $0}' "$BLOG"
  echo "--- end ---"
fi
if [[ $VERB -eq 1 ]]; then
  echo "done_line   : $([[ $DONE -eq 1 ]] && echo yes || echo no) (VERBOSE log: \"[Done] Built\" is the Makefile's last line of a complete build)"
else
  echo "done_line   : - (quiet log: the Makefile prints nothing at the end)"
fi
echo "--- last 5 lines of the make log ---"
tail -n 5 "$BLOG" | cut -c1-300
echo "--- products ---"
ls -l --time-style=full-iso "$APP" 2>&1
if [[ -d lib ]]; then ls -l --time-style=full-iso lib/ | tail -n +2; else echo "lib/: missing"; fi
echo "--- headers (md5) ---"
md5sum include/eventBuffer.h src/treestream.cc 2>&1

# ---- verdict -----------------------------------------------------------------
mtime () { stat -c %Y "$1" 2>/dev/null || stat -f %m "$1" 2>/dev/null || echo 0; }
WHY=()
if [[ -n "$FROM" ]]; then
  [[ -s "$BLOG" ]] || WHY+=("the log is empty")
  [[ "$NE" -gt 0 ]] && WHY+=("the log has error lines")
  [[ $VERB -eq 1 && $DONE -eq 0 ]] && WHY+=("the VERBOSE log has no [Done] line: that build did not finish")
else
  [[ "$RC" -ne 0 ]] && WHY+=("make exited $RC")
fi
if [[ ! -x "$APP" ]]; then
  WHY+=("no executable $APP: the build did not reach the link step (interrupted, or it failed; see the last lines of the log)")
elif [[ -n "$FROM" ]]; then
  d=$(( $(mtime "$APP") - $(mtime "$BLOG") ))
  if [[ $d -lt -600 ]]; then
    WHY+=("$APP is $(( -d / 60 )) min older than the log: it is not the product of that build")
  elif [[ $VERB -eq 0 && $d -gt 600 ]]; then
    WHY+=("$APP is $(( d / 60 )) min newer than the quiet log: it comes from a later build, and the log alone cannot show that its own build finished")
  fi
fi
if [[ ${#WHY[@]} -eq 0 ]]; then
  if [[ -n "$FROM" && $VERB -eq 1 ]]; then
    echo "result      : OK (the log shows a complete build: no error, [Done] line; the executable exists)"
  elif [[ -n "$FROM" ]]; then
    echo "result      : OK (no error in the quiet log; the executable dates from the end of that build)"
  else
    echo "result      : OK (make 0, executable built)"
  fi
  exit 0
fi
J="$(printf '%s; ' "${WHY[@]}")"
echo "result      : FAIL (${J%; })"
if [[ -z "$FROM" && "$RC" -ne 0 ]]; then exit "$RC"; fi
exit 1
