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
#      footer : end (UTC), wall seconds, EXIT code (143/130/129 when stopped by
#               SIGTERM/SIGINT/SIGHUP: condor_rm, a hold, Ctrl-C, ssh lost), files of
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

# ---- signals: caught from here on ---------------------------------------------
# SIGTERM (condor_rm, a hold, an eviction), SIGINT (Ctrl-C) and SIGHUP (the ssh
# session was lost) do not cost the record its footer and ledger line:
#  - before the command has started: the command is not started;
#  - while it runs: SIGTERM goes to the command and everything under it; the
#    copier that writes the log ignores it and takes the rest of the output
#    (also the footer of a runlog.sh inside the command);
#  - after the command has ended: ignored until the record is complete.
# What still runs RUNLOG_KILL_AFTER seconds (default 5) after the SIGTERM gets
# SIGKILL, so that a process that ignores SIGTERM cannot hold the record open
# (a runlog.sh inside the command must finish its own record within that time).
# Under nohup SIGHUP stays ignored (bash cannot catch a signal ignored at start).
# (Until 2026-10-04 (2): TERM/INT only, caught only around the command, and
# passed only to the command's own process.)
SIGNAL=""; RUNPID=""; WATCHDOG=""; NOT_STARTED=0; STOP_NOTE=""
KILL_AFTER="${RUNLOG_KILL_AFTER:-5}"; [[ "$KILL_AFTER" =~ ^[0-9]+$ ]] || KILL_AFTER=5
kids_of () {     # the direct children of $1 (procps pgrep, else /proc)
  if command -v pgrep >/dev/null 2>&1; then pgrep -P "$1" 2>/dev/null
  else cat /proc/"$1"/task/*/children 2>/dev/null | tr ' ' '\n' | grep -E '^[0-9]+$'; fi
}
proc_stat () {   # <pid> <1 = state | 2 = ppid>   (/proc, else ps)
  if [[ -r /proc/$1/stat ]]; then sed -E 's/^.*\) //' "/proc/$1/stat" 2>/dev/null | cut -d' ' -f"$2"
  elif [[ "$2" == 1 ]]; then ps -o stat= -p "$1" 2>/dev/null
  else ps -o ppid= -p "$1" 2>/dev/null | tr -d ' '; fi
}
tree_pids () {   # the pids under $1 (not $1 itself), each parent before its children
  local c
  for c in $(kids_of "$1"); do echo "$c"; tree_pids "$c"; done
}
stop_cmd () {    # SIGTERM to everything under the run subshell
  local _ pids="" st
  for _ in $(seq 1 40); do       # the subshell may not have started the command yet
    pids="$(tree_pids "$RUNPID")"
    [[ -n "$pids" ]] && break
    kill -0 "$RUNPID" 2>/dev/null || return 0     # it has ended
    st="$(proc_stat "$RUNPID" 1)"
    [[ "$st" == Z* ]] && return 0                 # it has ended, not yet reaped
    sleep 0.05
  done
  if [[ -z "$pids" ]]; then
    kill -TERM "$RUNPID" 2>/dev/null
    if ! command -v pgrep >/dev/null 2>&1 && [[ ! -r /proc/$$/task/$$/children ]]; then
      STOP_NOTE="no pgrep and no /proc children list here: only the run subshell was stopped, the command may have run on"
    fi
    return 0
  fi
  # shellcheck disable=SC2086
  kill -TERM $pids 2>/dev/null
  sleep 0.2                      # and what was forked in between
  pids="$(tree_pids "$RUNPID")"
  # shellcheck disable=SC2086
  [[ -n "$pids" ]] && kill -TERM $pids 2>/dev/null
  if [[ -z "$WATCHDOG" ]]; then  # SIGKILL to what is still there after KILL_AFTER s
    ( trap '' INT HUP
      sleep "$KILL_AFTER"
      [[ "$(proc_stat "$RUNPID" 2)" == "$$" ]] || exit 0
      pids="$(tree_pids "$RUNPID")"
      # shellcheck disable=SC2086
      [[ -n "$pids" ]] && kill -KILL $pids 2>/dev/null
      exit 0 ) &
    WATCHDOG=$!
  fi
  return 0
}
on_sig () {
  [[ -n "$SIGNAL" ]] || SIGNAL="$1"
  [[ -n "$RUNPID" ]] && stop_cmd
  return 0
}
trap 'on_sig TERM' TERM
trap 'on_sig INT' INT
trap 'on_sig HUP' HUP

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

# out: append stdin to the log (and the terminal); terminal only if no log.
# It ignores TERM/INT/HUP: a stopped run still gets all of its output written.
out () { trap '' TERM INT HUP; if [[ -n "$LOG" ]]; then tee -a "$LOG"; else cat; fi; }

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
# the ledger line (traps: see "signals" above). stdin is handed through on fd 3.
RC=0
if [[ -z "$SIGNAL" ]]; then
  exec 3<&0
  # the subshell checks SIGNAL again (its copy is taken at the fork): a signal
  # that came after the check above means the command is not started either
  { [[ -n "$SIGNAL" ]] && exit 199; "${CMD[@]}" 2>&1 0<&3 3<&- | out; exit "${PIPESTATUS[0]}"; } &
  RUNPID=$!
  exec 3<&-
  [[ -n "$SIGNAL" ]] && stop_cmd     # the signal came while the command was being started
  while :; do
    wait "$RUNPID"; RC=$?
    kill -0 "$RUNPID" 2>/dev/null || break
  done
fi
trap '' TERM INT HUP                 # the command has ended: finish the record whatever comes
if [[ -n "$WATCHDOG" ]]; then        # the watchdog and its sleep
  WK="$(kids_of "$WATCHDOG")"
  # shellcheck disable=SC2086
  kill "$WATCHDOG" $WK 2>/dev/null
fi
[[ -n "$SIGNAL" && -n "$RUNPID" && $RC -eq 199 ]] && NOT_STARTED=1   # the subshell's own check
case "$SIGNAL" in TERM) RC=143 ;; INT) RC=130 ;; HUP) RC=129 ;; esac

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
  if [[ -n "$SIGNAL" ]]; then
    if [[ -n "$RUNPID" && $NOT_STARTED -eq 0 ]]; then
      echo "stopped     : by SIG$SIGNAL (condor_rm, a hold, an eviction, Ctrl-C or a lost ssh session)"
    else
      echo "stopped     : by SIG$SIGNAL before the command started (condor_rm, a hold, an eviction, Ctrl-C or a lost ssh session)"
    fi
  fi
  [[ -n "$STOP_NOTE" ]] && echo "NOTE        : $STOP_NOTE"
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
