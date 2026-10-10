#!/bin/bash
# =============================================================================
#  condor_run.sh -- run one command as a KNU condor job, recorded by runlog.sh.
#  (STEP 23, 2026-10-04)
#
#  Usage (in a cmsenv shell; the job does cmsenv again from the same CMSSW):
#    /bin/bash tools/runlog/condor_run.sh [options] <step> -- <command> [args...]
#
#  Options
#    --cpus N       request_cpus (default 1)
#    --memory M     request_memory, e.g. 4GB (default 4GB)
#    --workdir D    directory the command runs in (default: the current one)
#    --os OS        MY.WantOS (default el9, as submit_job_FH_Tier3_unified.py)
#    --proxy F      x509 proxy to attach (default: <tempTTHH>/proxy.cert when it
#                   exists and has more than 1 h left; otherwise none -- these
#                   jobs read /pnfs as files and need no grid access)
#    --no-proxy     never attach a proxy
#    --source F     source F (relative to the workdir) after cmsenv, e.g.
#                   setup.sh when the command runs the analyzer itself
#    --time-limit H (STEP 26 M2, 2026-10-09; none by default) a run of
#                   more than H hours (e.g. 6 or 1.5) is held ('tthh time
#                   limit: running <min> min (limit <min> min) with <s> s CPU
#                   on <slot@machine>'), 5 min later the job starts again from
#                   the beginning on another machine, 3 starts in all; then it
#                   stays held -- what outputMerger/merge_outputs.py does for
#                   every merge (the same expressions). The time, not the CPU:
#                   jobs that read /pnfs can be healthy below 1 % CPU (the
#                   merges: 0.0-0.4 %), so the analyzer jobs' CPU rule
#                   (--stall-guard of M, removed) cannot tell them from a hung
#                   one. Give H well above the normal run time, and only for a
#                   command that may start again from the beginning: the plots
#                   (make_plots.py writes a new directory), TriggerStudy's
#                   run_analysis.sh, the smoke and synthetic tests, a build
#                   (build_check.sh does make clean) -- not
#                   tools/stage1/y1_reference.sh (it refuses existing outputs,
#                   so a restart fails). Each start leaves its own record (a
#                   stopped one ends with EXIT 143); status.sh shows 143 while
#                   the job is held or idle after a stop, '-' once it runs
#                   again, then the new start's code.
#    --dry-run      write the job files, do not submit
#
#  Memory: 4GB suits the scans and tests; for a build with -j4 give
#  --cpus 4 --memory 12GB (the analyzer jobs of this repo request 2 GB since STEP 27; they use at most ~0.3 GB).
#
#  What it writes: condor/runlog/<step>_<UTCstamp>/   (condor/ is gitignored)
#    payload.sh    cmsenv from $CMSSW_BASE/src, TMPDIR = the job's scratch,
#                  cd <workdir>, [source F], then the command (argv kept exactly)
#    job.sh        cd <workdir>; exec tools/runlog/runlog.sh <step> -- payload.sh
#                  so an environment failure is in the record too: exit 90
#                  (cmsset), 91 (CMSSW area), 92 (scram runtime), 93 (workdir),
#                  94 (--source). Only job.sh's own cd (the shared file system
#                  missing on the worker) fails before the record: job.out.
#    job.sub       KNU conventions copied from submit_job_FH_Tier3_unified.py and
#                  outputMerger/merge_outputs.py: getenv, MY.WantOS,
#                  request_memory, x509userproxy (when attached), the time
#                  limit (--time-limit)
#    job.out/.err/.log   condor's own files; job.log holds the IP addresses and
#                  ports of the schedd and the worker, one more reason condor/
#                  stays out of git
#    cluster.txt, runlog_path.txt, exit_code.txt   read by tools/runlog/status.sh
#  The record that matters is written on the worker by runlog.sh:
#    runlogs/run_<step>_<UTC start>.log and one line in runlogs/LEDGER.tsv.
#
#  The command must not need a terminal, ssh keys or a grid password: do
#  git pull/clone and voms-proxy-init interactively before submitting.
#  Exit: condor_submit's exit code (0 with --dry-run), 2 for bad usage; when
#  condor_submit exits 0 but prints no cluster id, the job is taken as queued
#  (cluster.txt = '?'; look with condor_q) and the exit code is 0.
# =============================================================================
set -u

REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd -P)"
usage () { sed -n '2,69p' "$0" | sed 's/^# \{0,2\}//'; }
die () { echo "condor_run.sh: $*" >&2; exit 2; }

CPUS=1; MEM="4GB"; WORKDIR="$PWD"; WANTOS="el9"; PROXY_OPT=""; NOPROXY=0; DRY=0; SRCFILE=""; TLIMIT=""
while [[ $# -gt 0 ]]; do
  case "$1" in
    -h|--help) usage; exit 0 ;;
    --cpus)    [[ $# -ge 2 ]] || die "--cpus needs a value"; CPUS="$2"; shift 2 ;;
    --memory)  [[ $# -ge 2 ]] || die "--memory needs a value"; MEM="$2"; shift 2 ;;
    --workdir) [[ $# -ge 2 ]] || die "--workdir needs a value"; WORKDIR="$2"; shift 2 ;;
    --os)      [[ $# -ge 2 ]] || die "--os needs a value"; WANTOS="$2"; shift 2 ;;
    --proxy)   [[ $# -ge 2 ]] || die "--proxy needs a value"; PROXY_OPT="$2"; shift 2 ;;
    --no-proxy) NOPROXY=1; shift ;;
    --source)  [[ $# -ge 2 ]] || die "--source needs a file"; SRCFILE="$2"; shift 2 ;;
    --time-limit) [[ $# -ge 2 ]] || die "--time-limit needs hours"; TLIMIT="$2"; shift 2 ;;
    --stall-guard) die "--stall-guard is gone (STEP 26 M2): healthy /pnfs readers can sit below 5 % CPU -- use --time-limit HOURS" ;;
    --dry-run) DRY=1; shift ;;
    --) die "missing <step> before '--'" ;;
    -*) die "unknown option $1 (see --help)" ;;
    *) break ;;
  esac
done
[[ $# -ge 3 ]] || { usage; exit 2; }
STEP="$1"; shift
[[ "$STEP" =~ ^[A-Za-z0-9_.-]+$ ]] || die "step must match [A-Za-z0-9_.-]+"
[[ "$1" == "--" ]] || die "expected '--' after the step name"
shift
[[ $# -ge 1 ]] || die "no command given"
CMD=("$@")

[[ "$CPUS" =~ ^[1-9][0-9]*$ ]] || die "--cpus must be a positive integer"
[[ "$MEM" =~ ^[0-9]+\ ?[KkMmGgTt]?[Bb]?$ ]] || die "--memory looks wrong: $MEM (e.g. 4GB)"
[[ "$WANTOS" =~ ^[A-Za-z0-9._-]+$ ]] || die "--os looks wrong: $WANTOS"
[[ -d "$WORKDIR" ]] || die "--workdir does not exist: $WORKDIR"
WORKDIR="$(cd "$WORKDIR" && pwd -P)"
if [[ -n "$SRCFILE" ]]; then
  [[ -f "$WORKDIR/$SRCFILE" || ( "$SRCFILE" == /* && -f "$SRCFILE" ) ]] || die "--source $SRCFILE: no such file (relative to $WORKDIR)"
fi
[[ -n "${CMSSW_BASE:-}" && -d "${CMSSW_BASE}/src" ]] \
  || die "CMSSW_BASE is not set: run 'cmsenv' in your CMSSW area first (the job repeats it from there)"
CMSSW_SRC="$(cd "$CMSSW_BASE/src" && pwd -P)"
[[ $DRY -eq 1 ]] || command -v condor_submit >/dev/null 2>&1 || die "condor_submit not found on this host"

# ---- proxy -------------------------------------------------------------------
PROXY=""; PROXY_NOTE=""
if [[ $NOPROXY -eq 0 ]]; then
  cand="${PROXY_OPT:-$REPO/proxy.cert}"
  if [[ -f "$cand" ]]; then
    left="$(voms-proxy-info -file "$cand" -timeleft 2>/dev/null | tail -1 | tr -d ' ')"
    if [[ "$left" =~ ^[0-9]+$ && "$left" -gt 3600 ]]; then
      PROXY="$(cd "$(dirname "$cand")" && pwd -P)/$(basename "$cand")"
      PROXY_NOTE="attached ($cand, ${left} s left)"
    elif [[ -n "$PROXY_OPT" ]]; then
      die "--proxy $PROXY_OPT: expired, unreadable or voms-proxy-info missing (timeleft='${left}')"
    else
      PROXY_NOTE="none ($cand expired or unreadable; not needed for these jobs)"
    fi
  elif [[ -n "$PROXY_OPT" ]]; then
    die "--proxy $PROXY_OPT: no such file"
  else
    PROXY_NOTE="none (no $cand; not needed for these jobs)"
  fi
else
  PROXY_NOTE="none (--no-proxy)"
fi

# ---- time limit (STEP 26 M2) ---------------------------------------------------
# The same text as time_limit_exprs() / time_limit_summary() of
# outputMerger/merge_outputs.py; test/test_failure_checks.py part I checks that
# they stay equal.
TL_S=""
if [[ -n "$TLIMIT" ]]; then
  [[ "$TLIMIT" =~ ^[0-9]+([.][0-9]+)?$ ]] || die "--time-limit needs a number of hours, e.g. 6 or 1.5 (got '$TLIMIT')"
  TL_S="$(awk -v h="$TLIMIT" 'BEGIN { printf "%d", h * 3600 + 0.5 }')"
  [[ "$TL_S" -ge 60 ]] || die "--time-limit $TLIMIT is less than a minute"
  TL_H="$(awk -v h="$TLIMIT" 'BEGIN { printf "%g", h }')"
  G_SUMMARY="hold after ${TL_H} h of the current run (any CPU), release after 5 min, at most 3 starts, not again on the machine it last ran on"
  G_CPU='(ifThenElse(isUndefined(RemoteUserCpu), 0, RemoteUserCpu) + ifThenElse(isUndefined(RemoteSysCpu), 0, RemoteSysCpu))'
  G_RUN='(time() - EnteredCurrentStatus)'
  G_HOLD="(JobStatus == 2) && (${G_RUN} > ${TL_S})"
  G_REASON="strcat(\"tthh time limit: running \", string(int(${G_RUN} / 60)), \" min (limit $((TL_S / 60)) min) with \", string(int(${G_CPU})), \" s CPU on \", ifThenElse(isUndefined(RemoteHost), \"?\", RemoteHost))"
  G_SUBCODE='4202'
  G_RELEASE='(HoldReasonCode == 3) && (HoldReasonSubCode == 4202) && (NumJobStarts < 3) && ((time() - EnteredCurrentStatus) > 300)'
  G_REQ='isUndefined(LastRemoteHost) || ((LastRemoteHost != TARGET.Machine) && (substr(LastRemoteHost, size(LastRemoteHost) - size(TARGET.Machine) - 1) != strcat("@", TARGET.Machine)))'
fi

# ---- job directory -------------------------------------------------------------
STAMP="$(date -u +%Y%m%d_%H%M%S)"
JOBDIR="$REPO/condor/runlog/${STEP}_${STAMP}"
n=1; while [[ -e "$JOBDIR" ]]; do n=$((n + 1)); JOBDIR="$REPO/condor/runlog/${STEP}_${STAMP}_$n"; done
mkdir -p "$JOBDIR" || die "cannot create $JOBDIR"
REL="${JOBDIR#$REPO/}"

q () { printf '%q' "$1"; }
CMDQ="$(printf '%q ' "${CMD[@]}")"
# what the record shows as the command (an environment variable has a size
# limit; the full command is always in payload.sh)
DISP="$CMDQ"
if [[ ${#DISP} -gt 4000 ]]; then DISP="${DISP:0:4000} ... (${#CMDQ} characters; full command in $REL/payload.sh)"; fi
SRCLINE=""
if [[ -n "$SRCFILE" ]]; then
  SRCLINE="source $(q "$SRCFILE") || { echo \"[condor_run] FATAL: --source failed:\" $(q "$SRCFILE"); exit 94; }"
fi

cat > "$JOBDIR/payload.sh" <<EOF
#!/bin/bash
# generated by tools/runlog/condor_run.sh at $(date -u +%FT%TZ) -- step $STEP
# Runs inside runlog.sh on the worker. Exit 90-94 = the environment, not the command.
CMSSET="\${CONDOR_RUN_CMSSET:-/cvmfs/cms.cern.ch/cmsset_default.sh}"
echo "[condor_run] worker \$(hostname -s 2>/dev/null || uname -n)  scratch \${_CONDOR_SCRATCH_DIR:-<none>}"
source "\$CMSSET" || { echo "[condor_run] FATAL: cannot source \$CMSSET"; exit 90; }
cd $(q "$CMSSW_SRC") || { echo "[condor_run] FATAL: cannot cd to the CMSSW area" $(q "$CMSSW_SRC"); exit 91; }
SCRAMENV="\$(scram runtime -sh)" || { echo "[condor_run] FATAL: scram runtime -sh failed in" $(q "$CMSSW_SRC"); exit 92; }
eval "\$SCRAMENV"
export TMPDIR="\${_CONDOR_SCRATCH_DIR:-/tmp}"
cd $(q "$WORKDIR") || { echo "[condor_run] FATAL: cannot cd to the workdir" $(q "$WORKDIR"); exit 93; }
$SRCLINE
echo "[condor_run] cmsenv ok: CMSSW_BASE=\$CMSSW_BASE  root \$(root-config --version 2>/dev/null || echo none)  \$(python3 --version 2>&1)  TMPDIR=\$TMPDIR"
printf '[condor_run] running in %s: %s\n' "\$PWD" $(q "$DISP")
echo "----------------------------------------------------------------------------"
$CMDQ
EOF

cat > "$JOBDIR/job.sh" <<EOF
#!/bin/bash
# generated by tools/runlog/condor_run.sh at $(date -u +%FT%TZ) -- step $STEP
cd $(q "$WORKDIR") || { echo "[condor_run] FATAL: cannot cd to the workdir (shared file system missing on this worker?)" $(q "$WORKDIR"); exit 93; }
export RUNLOG_CMD_DISPLAY=$(q "$DISP")
export RUNLOG_CONDOR_JOBDIR=$(q "$JOBDIR")
exec /bin/bash $(q "$REPO/tools/runlog/runlog.sh") $(q "$STEP") -- /bin/bash $(q "$JOBDIR/payload.sh")
EOF
chmod 755 "$JOBDIR/payload.sh" "$JOBDIR/job.sh"

{
  echo "executable              = $JOBDIR/job.sh"
  echo "getenv                  = True"
  echo "output                  = $JOBDIR/job.out"
  echo "error                   = $JOBDIR/job.err"
  echo "log                     = $JOBDIR/job.log"
  echo "MY.WantOS               = \"$WANTOS\""
  echo "request_memory          = $MEM"
  [[ "$CPUS" -gt 1 ]] && echo "request_cpus            = $CPUS"
  [[ -n "$PROXY" ]] && echo "x509userproxy           = $PROXY"
  if [[ -n "$TL_S" ]]; then
    echo "# time limit [STEP 26 M2]: $G_SUMMARY"
    echo "periodic_hold           = $G_HOLD"
    echo "periodic_hold_reason    = $G_REASON"
    echo "periodic_hold_subcode   = $G_SUBCODE"
    echo "periodic_release        = $G_RELEASE"
    echo "requirements            = $G_REQ"
  fi
  echo "queue 1"
} > "$JOBDIR/job.sub"

echo "[condor_run] step     : $STEP"
echo "[condor_run] command  : $CMDQ"
echo "[condor_run] workdir  : $WORKDIR"
echo "[condor_run] cmsenv   : $CMSSW_SRC"
echo "[condor_run] request  : cpus $CPUS, memory $MEM, os $WANTOS"
[[ -n "$SRCFILE" ]] && echo "[condor_run] source   : $SRCFILE (after cmsenv, in the workdir)"
echo "[condor_run] proxy    : $PROXY_NOTE"
if [[ -n "$TL_S" ]]; then echo "[condor_run] time limit: $G_SUMMARY"
else echo "[condor_run] time limit: none (a command that hangs stays 'running'; --time-limit HOURS for one that may start again)"; fi
echo "[condor_run] job dir  : $REL/"
if [[ $DRY -eq 1 ]]; then
  echo "[condor_run] --dry-run: not submitted (submit with: condor_submit $REL/job.sub)"
  exit 0
fi

OUT="$(condor_submit "$JOBDIR/job.sub" 2>&1)"; RC=$?
printf '%s\n' "$OUT" | sed 's/^/[condor_submit] /'
CL="$(printf '%s\n' "$OUT" | sed -n 's/.*submitted to cluster \([0-9][0-9]*\).*/\1/p' | tail -1)"
if [[ $RC -eq 0 ]]; then
  if [[ -n "$CL" ]]; then
    echo "$CL" > "$JOBDIR/cluster.txt"
    echo "[condor_run] submitted: cluster $CL"
    echo "[condor_run] watch    : condor_q $CL    |   tail -f $REL/job.out"
  else
    echo "?" > "$JOBDIR/cluster.txt"
    echo "[condor_run] condor_submit exited 0 but printed no cluster id: taken as queued -- check with condor_q (do not resubmit blindly)"
  fi
  echo "[condor_run] record   : runlogs/run_${STEP}_<UTC start>.log + runlogs/LEDGER.tsv (written by the job)"
  echo "[condor_run] all jobs : /bin/bash tools/runlog/status.sh"
else
  echo "[condor_run] submission FAILED (exit $RC) -- nothing queued; the job files stay in $REL/"
fi
exit $RC
