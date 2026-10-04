#!/bin/bash
# =============================================================================
#  runlog.sh -- run ONE step and leave a self-describing record of it.
#  (tempTTHH copy of NtupleForge script/runlog.sh, 2026-10-04; STEP 23)
#
#  Why: long KNU steps (builds, file scans, tests) ran in an interactive shell
#  and reached the AI session only as pasted text, or not at all. This wrapper
#  leaves (a) a log with a header and a footer and (b) one line in a ledger,
#  inside this repo, so "what ran, on which commit, did it work" is answerable
#  from git. tools/runlog/condor_run.sh runs the same wrapper in a condor job.
#
#  Usage (from any directory; the record always goes to <tempTTHH>/runlogs/):
#    /bin/bash tools/runlog/runlog.sh <step> -- <command> [args...]
#
#  <step>  short token, [A-Za-z0-9_.-] only; becomes part of the log name.
#
#  What it leaves
#    runlogs/run_<step>_<UTCstamp>.log
#      header : step, start (UTC), host, user, cwd, git HEAD + number of
#               modified tracked files of this repo (and of the cwd's checkout
#               when that is another repo), the command, CMSSW_BASE, ROOT,
#               python3, and the condor job id when run by condor_run.sh
#      body   : the command's stdout+stderr, live on the terminal too (tee)
#      footer : end (UTC), wall seconds, EXIT code (143/130 when the run was
#               stopped by SIGTERM/SIGINT, e.g. condor_rm or a hold), files of
#               this repo changed during the run (tmp/ condor/ runlogs/ .git/
#               left out; <= 40 listed)
#    runlogs/LEDGER.tsv : one line per run
#      utc_start  step  exit  wall_s  host  git_head  log  outputs
#
#  Exit code = the command's exit code (chain it with && / ||).
#
#  Commit policy (docs/DECISIONS.md D-2026-10-04-A): these logs hold our own
#  programs' output and are committed. A log goes to runlogs/nocommit/
#  (gitignored) and stays out of the ledger when the command line mentions
#  'crab', or when the output turns out to contain a pre-signed URL
#  signature (X-Amz-Signature and the like): CRAB transcripts embed them.
# =============================================================================
set -u
set -o pipefail

REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd -P)"

usage () { sed -n '2,38p' "$0" | sed 's/^# \{0,2\}//'; }

if [[ $# -lt 3 || "$1" == "-h" || "$1" == "--help" ]]; then usage; exit 2; fi
STEP="$1"; shift
[[ "$1" == "--" ]] || { echo "runlog.sh: expected '--' after the step name" >&2; exit 2; }
shift
[[ $# -ge 1 ]] || { echo "runlog.sh: no command given" >&2; exit 2; }
[[ "$STEP" =~ ^[A-Za-z0-9_.-]+$ ]] || { echo "runlog.sh: step must match [A-Za-z0-9_.-]+" >&2; exit 2; }

CMD=("$@")
ARGVSTR="$(printf '%q ' "${CMD[@]}")"
# Set by condor_run.sh's job.sh for THIS run only; unset so that a runlog.sh
# inside the command does not take them over.
CMDSTR="${RUNLOG_CMD_DISPLAY:-$ARGVSTR}"
JOBDIR_OUT="${RUNLOG_CONDOR_JOBDIR:-}"
unset RUNLOG_CMD_DISPLAY RUNLOG_CONDOR_JOBDIR

LOGDIR="$REPO/runlogs"
NOCOMMIT=0; NOCOMMIT_WHY=""
if [[ "$CMDSTR" == *crab* || "$ARGVSTR" == *crab* ]]; then
  LOGDIR="$REPO/runlogs/nocommit"; NOCOMMIT=1; NOCOMMIT_WHY="command mentions crab"
fi
LEDGER="$REPO/runlogs/LEDGER.tsv"

STAMP="$(date -u +%Y%m%d_%H%M%S)"
LOG=""
if mkdir -p "$LOGDIR" 2>/dev/null && [[ -w "$LOGDIR" ]]; then
  for cand in "$LOGDIR/run_${STEP}_${STAMP}.log" "$LOGDIR/run_${STEP}_${STAMP}_$$.log"; do
    if ( set -C; : > "$cand" ) 2>/dev/null; then LOG="$cand"; break; fi   # noclobber: atomic create
  done
fi
[[ -n "$LOG" ]] || echo "runlog.sh: cannot write under $LOGDIR -- the record is only on stdout" >&2

# out: append stdin to the log (and the terminal); terminal only if no log
out () { if [[ -n "$LOG" ]]; then tee -a "$LOG"; else cat; fi; }

# start mark for the outputs list (kept next to the log, not in /tmp: a condor
# job may inherit a TMPDIR that does not exist on the worker)
MARK=""
if [[ -n "$LOG" ]]; then
  MARK="$LOGDIR/.runlog_mark_${STAMP}_$$"
  : > "$MARK" 2>/dev/null || MARK=""
fi

# ---- environment facts (each guarded: absence is recorded, never fatal) -----
git_info () {   # <dir> -> "<short head> <modified tracked files>"
  local d="$1" h="n/a" n="n/a"
  if git -C "$d" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    h="$(git -C "$d" --no-optional-locks rev-parse --short HEAD 2>/dev/null || echo n/a)"
    n="$(git -C "$d" --no-optional-locks status --porcelain --untracked-files=no 2>/dev/null | wc -l | tr -d ' ')"
  fi
  echo "$h $n"
}
read -r GIT_HEAD GIT_DIRTY <<< "$(git_info "$REPO")"
CWD_GIT=""
CWD_TOP="$(git -C "$PWD" rev-parse --show-toplevel 2>/dev/null || true)"
if [[ -n "$CWD_TOP" ]]; then
  CWD_TOP="$(cd "$CWD_TOP" && pwd -P)"
  if [[ "$CWD_TOP" != "$REPO" ]]; then
    read -r CH CD <<< "$(git_info "$CWD_TOP")"
    CWD_GIT="$CWD_TOP @ $CH   modified_tracked_files: $CD"
  fi
fi
ROOTV="$(command -v root-config >/dev/null 2>&1 && root-config --version 2>/dev/null || echo none)"
PYV="$(command -v python3 >/dev/null 2>&1 && python3 --version 2>&1 || echo none)"
HOST="$(hostname -s 2>/dev/null || uname -n 2>/dev/null || echo unknown)"
CONDOR_ID=""
if [[ -n "${_CONDOR_JOB_AD:-}" && -r "${_CONDOR_JOB_AD}" ]]; then
  CL="$(awk -F' = ' '$1=="ClusterId"{print $2}' "$_CONDOR_JOB_AD" 2>/dev/null)"
  PR="$(awk -F' = ' '$1=="ProcId"{print $2}' "$_CONDOR_JOB_AD" 2>/dev/null)"
  CONDOR_ID="${CL:-?}.${PR:-?}  slot ${_CONDOR_SLOT:-?}"
fi
T0=$(date +%s)

if [[ -n "$JOBDIR_OUT" ]]; then
  rm -f "$JOBDIR_OUT/exit_code.txt" 2>/dev/null     # a restarted (released) job starts clean
  echo "${LOG#$REPO/}" > "$JOBDIR_OUT/runlog_path.txt" 2>/dev/null || true
fi

{
  echo "=== RUNLOG ================================================================"
  echo "step        : $STEP"
  echo "start_utc   : $(date -u +%FT%TZ)"
  echo "host        : $HOST   user: ${USER:-?}"
  echo "cwd         : $PWD"
  echo "repo        : $REPO"
  echo "git_head    : $GIT_HEAD   modified_tracked_files: $GIT_DIRTY"
  [[ -n "$CWD_GIT" ]] && echo "cwd_git     : $CWD_GIT"
  echo "cmd         : $CMDSTR"
  echo "CMSSW_BASE  : ${CMSSW_BASE:-<unset>}"
  echo "root        : $ROOTV"
  echo "python3     : $PYV"
  [[ -n "$CONDOR_ID" ]] && echo "condor_job  : $CONDOR_ID   jobdir: ${JOBDIR_OUT:-?}"
  [[ $NOCOMMIT -eq 1 ]] && echo "NOTE        : $NOCOMMIT_WHY -> log kept under runlogs/nocommit/ (never commit)"
  echo "==========================================================================="
} | out

# ---- run --------------------------------------------------------------------
# The command runs in the background so that a SIGTERM (condor_rm, a hold, an
# eviction) or a SIGINT (Ctrl-C) still lets this script write the footer and
# the ledger line. stdin is handed through on fd 3.
SIGNAL=""
on_sig () {
  SIGNAL="$1"
  if [[ -n "${RUNPID:-}" ]]; then
    pkill -TERM -P "$RUNPID" 2>/dev/null || kill -TERM "$RUNPID" 2>/dev/null || true
  fi
}
trap 'on_sig TERM' TERM
trap 'on_sig INT' INT
exec 3<&0
{ "${CMD[@]}" 2>&1 0<&3 3<&- | out; exit "${PIPESTATUS[0]}"; } &
RUNPID=$!
exec 3<&-
while :; do
  wait "$RUNPID"; RC=$?
  kill -0 "$RUNPID" 2>/dev/null || break
done
trap - TERM INT
if [[ -n "$SIGNAL" ]]; then
  if [[ "$SIGNAL" == TERM ]]; then RC=143; else RC=130; fi
fi

T1=$(date +%s); WALL=$((T1 - T0))

# ---- outputs: files of this repo newer than the start mark -------------------
OUTS=""
if [[ -n "$MARK" ]]; then
  OUTS="$(find "$REPO" \( -path "$REPO/.git" -o -path "$REPO/tmp" -o -path "$REPO/condor" \
                         -o -path "$REPO/runlogs" -o -name __pycache__ \) -prune \
               -o -type f -newer "$MARK" -print 2>/dev/null | sed "s#^$REPO/##" | sort)"
  rm -f "$MARK"
fi
NOUTS=0; [[ -n "$OUTS" ]] && NOUTS=$(printf '%s\n' "$OUTS" | wc -l | tr -d ' ')

{
  echo "==========================================================================="
  echo "end_utc     : $(date -u +%FT%TZ)"
  echo "wall_s      : $WALL"
  [[ -n "$SIGNAL" ]] && echo "stopped     : by SIG$SIGNAL (condor_rm, a hold, an eviction or Ctrl-C)"
  echo "EXIT        : $RC"
  if [[ $NOUTS -gt 0 ]]; then
    echo "outputs (files of this repo changed during the run; tmp/ condor/ runlogs/ left out): $NOUTS"
    printf '%s\n' "$OUTS" | head -40 | while IFS= read -r f; do
      [[ -n "$f" ]] || continue
      sz=$(stat -c %s "$REPO/$f" 2>/dev/null || stat -f %z "$REPO/$f" 2>/dev/null || echo ?)
      printf '  %10s  %s\n' "$sz" "$f"
    done
    [[ $NOUTS -gt 40 ]] && echo "  ... and $((NOUTS - 40)) more"
  else
    echo "outputs     : (no file of this repo changed, apart from tmp/ condor/ runlogs/)"
  fi
  if [[ -n "$LOG" ]]; then
    echo "log         : ${LOG#$REPO/}"
    SZ_KB=$(( $(stat -c %s "$LOG" 2>/dev/null || stat -f %z "$LOG" 2>/dev/null || echo 0) / 1024 ))
    echo "log_kb      : $SZ_KB"
    [[ $SZ_KB -gt 2048 ]] && echo "NOTE        : large record (bulk output). Keep bulk output in a file under condor/ and print a summary (as tools/stage0/build_check.sh does); commit this one only if it is needed"
  else
    echo "log         : <none: could not write under runlogs/>"
  fi
  echo "=== END ==================================================================="
} | out

# ---- credential guard: a pre-signed URL in the output keeps the log out of git
if [[ -n "$LOG" && $NOCOMMIT -eq 0 ]] && grep -qiE 'x-amz-signature|x-amz-credential|awsaccesskeyid|[?&]signature=' "$LOG"; then
  mkdir -p "$REPO/runlogs/nocommit" 2>/dev/null
  NEW="$REPO/runlogs/nocommit/$(basename "$LOG")"
  if mv "$LOG" "$NEW" 2>/dev/null; then
    LOG="$NEW"; NOCOMMIT=1
    echo "NOTE        : the output contains a pre-signed URL signature -> log moved to runlogs/nocommit/ (never commit), no ledger line" | tee -a "$LOG"
    [[ -n "$JOBDIR_OUT" ]] && echo "${LOG#$REPO/}" > "$JOBDIR_OUT/runlog_path.txt" 2>/dev/null
  else
    echo "runlog.sh: WARNING the log contains a pre-signed URL signature and could not be moved: do NOT commit $LOG" >&2
    NOCOMMIT=1
  fi
fi

# ---- ledger (one short line; flock against two jobs ending together on NFS) --
if [[ $NOCOMMIT -eq 0 && -n "$LOG" ]]; then
  OUTS1="$(printf '%s' "$OUTS" | head -40 | tr '\n' ';')"
  [[ $NOUTS -gt 40 ]] && OUTS1="${OUTS1}+$((NOUTS - 40))more"
  LINE="$(printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s' \
    "$(date -u -d "@$T0" +%FT%TZ 2>/dev/null || date -u -r "$T0" +%FT%TZ)" \
    "$STEP" "$RC" "$WALL" "$HOST" "$GIT_HEAD" "${LOG#$REPO/}" "${OUTS1:-none}")"
  write_ledger () {
    [[ -s "$LEDGER" ]] || printf 'utc_start\tstep\texit\twall_s\thost\tgit_head\tlog\toutputs\n' >> "$LEDGER"
    printf '%s\n' "$LINE" >> "$LEDGER"
  }
  if command -v flock >/dev/null 2>&1; then
    ( flock -w 60 9 || true; write_ledger ) 9>>"$REPO/runlogs/.LEDGER.lock"
  else
    write_ledger
  fi
fi

if [[ -n "$JOBDIR_OUT" ]]; then
  echo "$RC" > "$JOBDIR_OUT/exit_code.txt" 2>/dev/null || true
fi

exit "$RC"
