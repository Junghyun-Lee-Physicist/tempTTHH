#!/bin/bash
# =============================================================================
#  status.sh -- one line per job submitted by tools/runlog/condor_run.sh,
#  oldest first by submission time.   (STEP 23, 2026-10-04)
#
#    /bin/bash tools/runlog/status.sh [N]     the newest N jobs (default 20)
#
#  Columns: job dir (step_UTCstamp), condor cluster, state, exit code, record.
#  state:
#    idle / running / held / ...  the job is in the queue (from condor_q); a
#                     held job also prints condor's HoldReason, and may show
#                     an exit code (143) from the run that condor stopped
#    done             not in the queue, the job wrote its exit code
#    gone(no exit)    not in the queue, the record was started but has no exit
#                     code (killed hard): look at the record and at
#                     condor/runlog/<dir>/job.out and job.err
#    gone             not in the queue, no record at all: the job died before
#                     runlog.sh (e.g. the shared file system was missing on the
#                     worker): condor/runlog/<dir>/job.out and job.err
#    not-submitted    --dry-run or a failed submission
#    unknown(...)     condor_q failed or is missing, or no cluster id
# =============================================================================
set -u
REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd -P)"
N="${1:-20}"
[[ "$N" =~ ^[0-9]+$ ]] || { echo "usage: status.sh [N]" >&2; exit 2; }
BASE="$REPO/condor/runlog"
[[ -d "$BASE" ]] || { echo "no condor_run.sh jobs yet ($BASE does not exist)"; exit 0; }

HAVE_Q=0; command -v condor_q >/dev/null 2>&1 && HAVE_Q=1
queue_state () {   # cluster -> idle|running|held|... ; "" when not in the queue; unknown(...) when we cannot tell
  [[ "$1" =~ ^[0-9]+$ ]] || { echo "unknown(no cluster id)"; return; }
  [[ $HAVE_Q -eq 1 ]] || { echo "unknown(no condor_q)"; return; }
  local st
  st="$(condor_q "$1" -af JobStatus 2>/dev/null)" || { echo "unknown(condor_q failed)"; return; }
  case "$(printf '%s\n' "$st" | head -1)" in
    1) echo idle ;; 2) echo running ;; 3) echo removed ;; 4) echo completed ;;
    5) echo held ;; 6) echo transferring ;; 7) echo suspended ;; *) echo "" ;;
  esac
}

printf '%-44s %-9s %-14s %-5s %s\n' "job (step_UTC)" "cluster" "state" "exit" "record"
# order by the UTC stamp in the directory name, not by the step name
ls -1 "$BASE" 2>/dev/null \
  | sed -nE 's/^(.*)_([0-9]{8}_[0-9]{6})(_[0-9]+)?$/\2\3 &/p' | sort | tail -n "$N" | cut -d' ' -f2- \
  | while IFS= read -r d; do
  J="$BASE/$d"; [[ -d "$J" ]] || continue
  CL="-"; [[ -s "$J/cluster.txt" ]] && CL="$(cat "$J/cluster.txt")"
  EX="-"; [[ -s "$J/exit_code.txt" ]] && EX="$(cat "$J/exit_code.txt")"
  REC="-"; [[ -s "$J/runlog_path.txt" ]] && REC="$(cat "$J/runlog_path.txt")"
  if [[ "$CL" == "-" ]]; then
    ST="not-submitted"
  else
    ST="$(queue_state "$CL")"
    if [[ -z "$ST" ]]; then
      if [[ "$EX" != "-" ]]; then ST="done"
      elif [[ "$REC" != "-" ]]; then ST="gone(no exit)"
      else ST="gone"; fi
    fi
  fi
  printf '%-44s %-9s %-14s %-5s %s\n' "$d" "$CL" "$ST" "$EX" "$REC"
  if [[ "$ST" == "held" ]]; then
    echo "    HoldReason: $(condor_q "$CL" -af HoldReason 2>/dev/null | head -1)"
  fi
done
