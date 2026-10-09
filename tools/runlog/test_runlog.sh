#!/bin/bash
# =============================================================================
#  test_runlog.sh -- offline tests of runlog.sh, condor_run.sh and status.sh
#  (STEP 23, 2026-10-04). No condor, CVMFS or ROOT needed: condor_submit,
#  condor_q, voms-proxy-info and scram are stubs, the "worker" is a local
#  bash that runs the generated job.sh. Works on a throw-away copy of the
#  three scripts in a temporary git repo; the real runlogs/ is not touched.
#
#    /bin/bash tools/runlog/test_runlog.sh        -> RESULT: <n> PASS, <m> FAIL
#
#  2026-10-04 (2), after the first KNU run (69 PASS, 2 FAIL): the work area is
#  taken by its physical path (a TMPDIR through a symlink no longer breaks the
#  path comparisons), the SIGTERM test waits for the command's own line, the
#  stubs are checked before anything could reach a real condor_submit, the
#  throw-away repo is made without the user's git configuration, and T22-T28
#  cover the signal cases (header phase, command tree, nested record, SIGHUP,
#  footer phase, a command that ignores SIGTERM).
#  2026-10-09 (STEP 26 M): T30, the stall guard lines of condor_run.sh's job.sub.
# =============================================================================
set -u
SRC="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
W="$(mktemp -d "${TMPDIR:-/tmp}/test_runlog.XXXXXX")" || exit 2
trap 'rm -rf "$W"' EXIT
W="$(cd "$W" && pwd -P)" || exit 2          # physical: the scripts record physical paths
# nothing of the caller's git or condor context may leak into the runs below
unset GIT_DIR GIT_WORK_TREE GIT_INDEX_FILE RUNLOG_CMD_DISPLAY RUNLOG_CONDOR_JOBDIR RUNLOG_KILL_AFTER \
      _CONDOR_JOB_AD _CONDOR_SCRATCH_DIR _CONDOR_SLOT
NP=0; NF=0
ok ()  { NP=$((NP + 1)); echo "PASS $1"; }
bad () { NF=$((NF + 1)); echo "FAIL $1"; }
check () { if eval "$2"; then ok "$1"; else bad "$1"; fi; }

# ---- a throw-away repo with the scripts -------------------------------------------
R="$W/repo"; mkdir -p "$R/tools/runlog" "$R/tmp" "$R/sub"
cp "$SRC/runlog.sh" "$SRC/condor_run.sh" "$SRC/status.sh" "$R/tools/runlog/"
cp "$SRC/../../.gitignore" "$R/.gitignore"      # the repository's own rules
echo a > "$R/tracked.txt"
( cd "$R" && export GIT_CONFIG_GLOBAL=/dev/null GIT_CONFIG_NOSYSTEM=1 \
  && git init -q && git config user.email t@t && git config user.name t && git add -A \
  && git -c core.hooksPath=/dev/null -c commit.gpgsign=false commit -q --no-verify -m init ) > "$W/git_init.out" 2>&1
RL="$R/tools/runlog/runlog.sh"; CR="$R/tools/runlog/condor_run.sh"; ST="$R/tools/runlog/status.sh"
check "T0 throw-away repo has a commit"  "git -C '$R' rev-parse -q --verify HEAD >/dev/null"

# ---- stubs --------------------------------------------------------------------
S="$W/stub"; mkdir -p "$S"
cat > "$S/condor_submit" <<'EOF'
#!/bin/bash
echo "$1" >> "$STUB_LOG/submitted.txt"
echo "Submitting job(s)."
[[ "${STUB_SUBMIT_NOID:-0}" == 1 ]] && exit 0
echo "1 job(s) submitted to cluster ${STUB_CLUSTER:-777}."
EOF
cat > "$S/condor_q" <<'EOF'
#!/bin/bash
# condor_q <cluster> -af <attr>
[[ "${STUB_Q_FAIL:-0}" == 1 ]] && { echo "Failed to fetch ads from schedd" >&2; exit 1; }
c="$1"; attr="$3"
f="$STUB_LOG/q_$c"
[[ -f "$f" ]] || exit 0
if [[ "$attr" == "JobStatus" ]]; then cut -d' ' -f1 "$f"; else cut -d' ' -f2- "$f"; fi
EOF
cat > "$S/voms-proxy-info" <<'EOF'
#!/bin/bash
echo "${STUB_PROXY_LEFT:-0}"
EOF
cat > "$S/scram" <<'EOF'
#!/bin/bash
[[ "${STUB_SCRAM_FAIL:-0}" == 1 ]] && { echo "scram: fake failure" >&2; exit 1; }
echo 'export FAKE_CMSENV_DONE=1; export CMSSW_BASE='"$(cd .. && pwd)"';'
EOF
chmod +x "$S"/*
mkdir -p "$W/cmssw/src"
echo '# fake cmsset_default.sh' > "$W/cmsset.sh"
export PATH="$S:$PATH" STUB_LOG="$W" CONDOR_RUN_CMSSET="$W/cmsset.sh"
export CMSSW_BASE="$W/cmssw"
# The stubs must be what a child bash finds (a noexec TMPDIR, a BASH_ENV that
# edits PATH or an exported shell function would let a real one through): stop.
for c in condor_submit condor_q voms-proxy-info scram; do
  got="$(/bin/bash -c "command -v $c" 2>/dev/null | tail -n 1)"
  if [[ "$got" != "$S/$c" ]]; then
    echo "ABORT: a child bash finds '$c' as '${got:-nothing}', not the stub $S/$c."
    echo "       Causes: TMPDIR on a noexec mount, a BASH_ENV that changes PATH, an exported function '$c'."
    echo "       For a noexec TMPDIR:  mkdir -p \$HOME/tmp_test && TMPDIR=\$HOME/tmp_test /bin/bash $0"
    echo "RESULT: $NP PASS, $((NF + 1)) FAIL (aborted before the condor tests)"
    exit 2
  fi
done

# simulate a condor worker running the job.sh of one job dir
run_worker () {   # <jobdir> [extra env assignments...]
  local jd="$1"; shift
  mkdir -p "$W/scratch"
  printf 'ClusterId = %s\nProcId = 0\n' "$(cat "$jd/cluster.txt" 2>/dev/null || echo 0)" > "$W/jobad"
  ( cd "$W/scratch" && env -u RUNLOG_CMD_DISPLAY _CONDOR_JOB_AD="$W/jobad" _CONDOR_SCRATCH_DIR="$W/scratch" \
      _CONDOR_SLOT=slot1_1 TMPDIR=/nonexistent/tmp "$@" /bin/bash "$jd/job.sh" > "$jd/job.out" 2> "$jd/job.err" )
}

# ---- T1 runlog: header, body, footer, ledger --------------------------------------------
( cd "$R/sub" && /bin/bash "$RL" t1_step -- /bin/bash -c 'echo hello-body; echo err-line >&2' ) > "$W/t1.out" 2>&1; rc=$?
L1="$(ls "$R"/runlogs/run_t1_step_*.log 2>/dev/null | head -1)"
check "T1 exit 0"                        "[[ $rc -eq 0 ]]"
check "T1 log written"                   "[[ -s '$L1' ]]"
check "T1 header step/cwd/git"           "grep -q '^step        : t1_step' '$L1' && grep -q '^cwd         : $R/sub' '$L1' && grep -qE '^git_head    : [0-9a-f]{7}' '$L1'"
check "T1 body has stdout and stderr"    "grep -q hello-body '$L1' && grep -q err-line '$L1'"
check "T1 footer EXIT 0"                 "grep -q '^EXIT        : 0' '$L1'"
check "T1 ledger header + line"          "[[ \$(wc -l < '$R/runlogs/LEDGER.tsv') -eq 2 ]] && tail -1 '$R/runlogs/LEDGER.tsv' | awk -F'\t' '\$2==\"t1_step\" && \$3==0 {f=1} END{exit !f}'"
check "T1 terminal copy"                 "grep -q hello-body '$W/t1.out'"

# ---- T2 exit code of a failing command ----------------------------------------------------
/bin/bash "$RL" t2_fail -- /bin/bash -c 'echo before; exit 7' > /dev/null 2>&1; rc=$?
L2="$(ls "$R"/runlogs/run_t2_fail_*.log | head -1)"
check "T2 wrapper exit = 7"              "[[ $rc -eq 7 ]]"
check "T2 footer EXIT 7, ledger 7"       "grep -q '^EXIT        : 7' '$L2' && tail -1 '$R/runlogs/LEDGER.tsv' | awk -F'\t' '\$3==7{f=1} END{exit !f}'"

# ---- T3 crab goes to nocommit and not to the ledger ---------------------------------------------
n0=$(wc -l < "$R/runlogs/LEDGER.tsv")
/bin/bash "$RL" t3_crab -- /bin/bash -c 'echo crab status -d x' > /dev/null 2>&1
check "T3 crab log under nocommit"      "ls '$R'/runlogs/nocommit/run_t3_crab_*.log >/dev/null 2>&1 && ! ls '$R'/runlogs/run_t3_crab_*.log >/dev/null 2>&1"
check "T3 no ledger line"                "[[ \$(wc -l < '$R/runlogs/LEDGER.tsv') -eq $n0 ]]"
check "T3 nocommit is ignored by git"    "( cd '$R' && git check-ignore -q runlogs/nocommit/x.log )"

# ---- T4 outputs list: repo files only, tmp/ condor/ runlogs/ left out -----------------------
/bin/bash "$RL" t4_out -- /bin/bash -c "echo x > '$R/made.txt'; echo y > '$R/tmp/obj.o'; mkdir -p '$R/condor'; echo z > '$R/condor/c.txt'" > /dev/null 2>&1
L4="$(ls "$R"/runlogs/run_t4_out_*.log | head -1)"
check "T4 made.txt listed"               "grep -qE '^ +[0-9]+  made.txt' '$L4'"
check "T4 tmp/ and condor/ not listed"   "! sed -n '/^outputs/,/^log /p' '$L4' | grep -qE 'tmp/obj.o|condor/c.txt'"
check "T4 log line is the relative path" "grep -qE '^log         : runlogs/run_t4_out_[0-9_]+\.log\$' '$L4'"
check "T4 no mark file left"             "! ls '$R'/runlogs/.runlog_mark_* >/dev/null 2>&1"

# ---- T5 bad usage ---------------------------------------------------------------------------------
/bin/bash "$RL" 'bad step' -- true > /dev/null 2>&1; r1=$?
/bin/bash "$RL" t5 true x > /dev/null 2>&1; r2=$?
/bin/bash "$CR" t5 -- > /dev/null 2>&1; r3=$?
( unset CMSSW_BASE; /bin/bash "$CR" t5 -- true ) > "$W/t5.out" 2>&1; r4=$?
/bin/bash "$CR" --workdir /no/such/dir t5 -- true > /dev/null 2>&1; r5=$?
/bin/bash "$CR" --cpus 0 t5 -- true > /dev/null 2>&1; r6=$?
check "T5 runlog bad step -> 2"          "[[ $r1 -eq 2 ]]"
check "T5 runlog missing -- -> 2"        "[[ $r2 -eq 2 ]]"
check "T5 condor_run no command -> 2"    "[[ $r3 -eq 2 ]]"
check "T5 condor_run no cmsenv -> 2 + hint" "[[ $r4 -eq 2 ]] && grep -q cmsenv '$W/t5.out'"
check "T5 condor_run bad workdir -> 2"   "[[ $r5 -eq 2 ]]"
check "T5 condor_run --cpus 0 -> 2"      "[[ $r6 -eq 2 ]]"

# ---- T6 dry run: files and submit file -----------------------------------------------------------
( cd "$R" && /bin/bash "$CR" --dry-run --cpus 4 --memory 8GB t6_dry -- /bin/bash -c 'echo hi' ) > "$W/t6.out" 2>&1; rc=$?
J6="$(ls -d "$R"/condor/runlog/t6_dry_* | head -1)"
check "T6 dry run exit 0"                "[[ $rc -eq 0 ]]"
check "T6 job files"                     "[[ -x '$J6/job.sh' && -x '$J6/payload.sh' && -s '$J6/job.sub' ]]"
check "T6 submit file: KNU conventions"  "grep -q '^getenv *= True' '$J6/job.sub' && grep -q '^MY.WantOS *= \"el9\"' '$J6/job.sub' && grep -q '^request_memory *= 8GB' '$J6/job.sub' && grep -q '^request_cpus *= 4' '$J6/job.sub' && grep -q '^queue 1' '$J6/job.sub'"
check "T6 no proxy attached"             "! grep -q x509userproxy '$J6/job.sub' && grep -q 'proxy    : none' '$W/t6.out'"
check "T6 not submitted"                 "[[ ! -f '$W/submitted.txt' && ! -f '$J6/cluster.txt' ]]"
check "T6 condor/ is gitignored"         "( cd '$R' && git check-ignore -q condor/runlog/x )"

# ---- T7 submit + worker: argv kept exactly, cwd, TMPDIR, record ------------------------------------
cat > "$R/sub/argv.py" <<'EOF'
import json, os, sys
print("ARGV " + json.dumps(sys.argv[1:]))
print("CWD " + os.getcwd())
print("TMPDIR " + os.environ.get("TMPDIR", ""))
print("CMSENV " + os.environ.get("FAKE_CMSENV_DONE", "0"))
EOF
export STUB_CLUSTER=4242
( cd "$R/sub" && /bin/bash "$CR" t7_argv -- python3 argv.py 'two words' '$HOME' '*' "it's" 'a"b' '' ) > "$W/t7.out" 2>&1; rc=$?
J7="$(ls -d "$R"/condor/runlog/t7_argv_* | head -1)"
check "T7 submit exit 0, cluster recorded" "[[ $rc -eq 0 && \$(cat '$J7/cluster.txt') == 4242 ]] && grep -q 'submitted: cluster 4242' '$W/t7.out'"
check "T7 condor_submit got job.sub"     "grep -q '$J7/job.sub' '$W/submitted.txt'"
run_worker "$J7"; wrc=$?
L7="$R/$(cat "$J7/runlog_path.txt" 2>/dev/null)"
check "T7 worker exit 0"                 "[[ $wrc -eq 0 && \$(cat '$J7/exit_code.txt') == 0 ]]"
check "T7 record written"                "[[ -s '$L7' ]] && grep -q '^EXIT        : 0' '$L7'"
cat > "$W/t7.expect" <<'EOF'
ARGV ["two words", "$HOME", "*", "it's", "a\"b", ""]
EOF
check "T7 argv kept exactly"             "grep -qxF -f '$W/t7.expect' '$L7'"
check "T7 cwd = submit dir"              "grep -q '^CWD $R/sub\$' '$L7' && grep -q '^cwd         : $R/sub' '$L7'"
check "T7 TMPDIR = job scratch"          "grep -q '^TMPDIR $W/scratch\$' '$L7'"
check "T7 cmsenv done on worker"         "grep -q '^CMSENV 1' '$L7' && grep -q 'cmsenv ok' '$L7'"
check "T7 header shows real command"     "grep -q '^cmd         : python3 argv.py' '$L7'"
check "T7 header condor id"              "grep -q '^condor_job  : 4242.0' '$L7'"
check "T7 ledger line"                   "tail -1 '$R/runlogs/LEDGER.tsv' | awk -F'\t' '\$2==\"t7_argv\" && \$3==0 {f=1} END{exit !f}'"

# ---- T8 environment failures are recorded with their own exit codes -------------------------------
export STUB_CLUSTER=5000
( cd "$R" && /bin/bash "$CR" t8_nocms -- true ) > /dev/null 2>&1
J8="$(ls -d "$R"/condor/runlog/t8_nocms_* | head -1)"
run_worker "$J8" CONDOR_RUN_CMSSET=/no/such/cmsset.sh
check "T8 missing cmsset -> exit 90 in the record" "[[ \$(cat '$J8/exit_code.txt') == 90 ]] && grep -q 'FATAL: cannot source' '$R/'\$(cat '$J8/runlog_path.txt')"
( cd "$R" && /bin/bash "$CR" t8_scram -- true ) > /dev/null 2>&1
J8b="$(ls -d "$R"/condor/runlog/t8_scram_* | head -1)"
run_worker "$J8b" STUB_SCRAM_FAIL=1
check "T8 scram failure -> exit 92"     "[[ \$(cat '$J8b/exit_code.txt') == 92 ]]"

# ---- T9 proxy rules ---------------------------------------------------------------------------------
echo fake > "$R/proxy.cert"
( cd "$R" && STUB_PROXY_LEFT=86400 /bin/bash "$CR" --dry-run t9_p1 -- true ) > /dev/null 2>&1
J9a="$(ls -d "$R"/condor/runlog/t9_p1_* | head -1)"
( cd "$R" && STUB_PROXY_LEFT=10 /bin/bash "$CR" --dry-run t9_p2 -- true ) > "$W/t9b.out" 2>&1
J9b="$(ls -d "$R"/condor/runlog/t9_p2_* | head -1)"
( cd "$R" && STUB_PROXY_LEFT=86400 /bin/bash "$CR" --dry-run --no-proxy t9_p3 -- true ) > /dev/null 2>&1
J9c="$(ls -d "$R"/condor/runlog/t9_p3_* | head -1)"
( cd "$R" && STUB_PROXY_LEFT=10 /bin/bash "$CR" --dry-run --proxy "$R/proxy.cert" t9_p4 -- true ) > /dev/null 2>&1; r9d=$?
check "T9 valid proxy attached"          "grep -q '^x509userproxy *= $R/proxy.cert' '$J9a/job.sub'"
check "T9 expired proxy not attached"    "! grep -q x509userproxy '$J9b/job.sub' && grep -q 'expired or unreadable' '$W/t9b.out'"
check "T9 --no-proxy"                    "! grep -q x509userproxy '$J9c/job.sub'"
check "T9 explicit expired --proxy -> 2" "[[ $r9d -eq 2 ]]"

# ---- T10 status.sh ---------------------------------------------------------------------------------
export STUB_CLUSTER=6000
( cd "$R" && /bin/bash "$CR" t10_idle -- true ) > /dev/null 2>&1
echo "5 memory limit exceeded" > "$W/q_6000"
/bin/bash "$ST" > "$W/t10.out" 2>&1
check "T10 done job shows exit"          "grep -E '^t7_argv_[0-9_]+ +4242 +done +0 +runlogs/run_t7_argv_' '$W/t10.out' >/dev/null"
check "T10 held job + HoldReason"        "grep -qE '^t10_idle_[0-9_]+ +6000 +held' '$W/t10.out' && grep -q 'HoldReason: memory limit exceeded' '$W/t10.out'"
check "T10 dry run = not-submitted"      "grep -qE '^t6_dry_[0-9_]+ +- +not-submitted' '$W/t10.out'"
rm -f "$W/q_6000"
/bin/bash "$ST" > "$W/t10b.out" 2>&1
check "T10 left the queue without exit = gone" "grep -qE '^t10_idle_[0-9_]+ +6000 +gone' '$W/t10b.out'"

# ---- T11 ledger under concurrent writers -------------------------------------------------------------
n0=$(wc -l < "$R/runlogs/LEDGER.tsv")
for i in $(seq 1 12); do /bin/bash "$RL" "t11_par$i" -- /bin/bash -c 'sleep 0.2' > /dev/null 2>&1 & done; wait
n1=$(wc -l < "$R/runlogs/LEDGER.tsv")
check "T11 12 parallel runs -> 12 ledger lines" "[[ \$((n1 - n0)) -eq 12 ]]"
check "T11 every ledger line has 8 fields" "awk -F'\t' 'NF!=8{b++} END{exit b>0}' '$R/runlogs/LEDGER.tsv'"

# ---- T12 runlogs/ unwritable: the command still runs, record on stdout only ----------------------------
R2="$W/ro"; mkdir -p "$R2/tools/runlog" "$R2/runlogs"; cp "$SRC/runlog.sh" "$R2/tools/runlog/"; chmod 555 "$R2/runlogs"
/bin/bash "$R2/tools/runlog/runlog.sh" t12 -- /bin/bash -c 'echo still-runs; exit 3' > "$W/t12.out" 2>&1; rc=$?
chmod 755 "$R2/runlogs"
if [[ $(id -u) -eq 0 ]]; then
  ok "T12 skipped (running as root: chmod does not block writes)"
else
  check "T12 exit 3, output on stdout, warning" "[[ $rc -eq 3 ]] && grep -q still-runs '$W/t12.out' && grep -q 'cannot write under' '$W/t12.out'"
fi


# ---- T13 SIGTERM (condor_rm, a hold) still writes the footer and the ledger line ----------------------
n0=$(wc -l < "$R/runlogs/LEDGER.tsv")
/bin/bash "$RL" t13_term -- /bin/bash -c 'echo started; sleep 30; echo never' > "$W/t13.out" 2>&1 &
P13=$!
# wait for the command's own line (-x): the header's cmd line also contains the word
for _ in $(seq 1 300); do grep -qx started "$W/t13.out" 2>/dev/null && break; sleep 0.1; done
kill -TERM "$P13"; wait "$P13"; rc=$?
L13="$(ls "$R"/runlogs/run_t13_term_*.log | head -1)"
check "T13 exit 143 after SIGTERM"       "[[ $rc -eq 143 ]]"
check "T13 footer: stopped + EXIT 143"   "grep -q '^stopped     : by SIGTERM' '$L13' && grep -q '^EXIT        : 143' '$L13' && ! grep -qx never '$L13'"
check "T13 ledger line written"          "[[ \$(( \$(wc -l < '$R/runlogs/LEDGER.tsv') - n0 )) -eq 1 ]] && tail -1 '$R/runlogs/LEDGER.tsv' | awk -F'\t' '\$2==\"t13_term\" && \$3==143 {f=1} END{exit !f}'"
check "T13 no mark file left"            "! ls '$R'/runlogs/.runlog_mark_* >/dev/null 2>&1"

# ---- T14 a runlog.sh inside a condor job does not take over the job's record ------------------------
export STUB_CLUSTER=7100
( cd "$R" && /bin/bash "$CR" t14_outer -- /bin/bash "$RL" t14_inner -- /bin/bash -c 'exit 4' ) > /dev/null 2>&1
J14="$(ls -d "$R"/condor/runlog/t14_outer_* | head -1)"
run_worker "$J14"
check "T14 job record = the outer log"   "[[ \$(cat '$J14/runlog_path.txt') == runlogs/run_t14_outer_* ]]"
check "T14 exit code = outer (4 passed up)" "[[ \$(cat '$J14/exit_code.txt') == 4 ]]"
LI="$(ls "$R"/runlogs/run_t14_inner_*.log | head -1)"
check "T14 inner log shows its own command" "grep -q '^cmd         : /bin/bash -c exit\\\\ 4' '$LI'"

# ---- T15 credential guard: a pre-signed URL in the output keeps the log out of git -----------------
n0=$(wc -l < "$R/runlogs/LEDGER.tsv")
/bin/bash "$RL" t15_sig -- /bin/bash -c 'echo "PUT https://s3.example/b/k?X-Amz-Credential=A%2F2026&X-Amz-Signature=deadbeef"' > /dev/null 2>&1
check "T15 log moved to nocommit, no ledger line" "ls '$R'/runlogs/nocommit/run_t15_sig_*.log >/dev/null 2>&1 && ! ls '$R'/runlogs/run_t15_sig_*.log >/dev/null 2>&1 && [[ \$(wc -l < '$R/runlogs/LEDGER.tsv') -eq $n0 ]]"

# ---- T16 status.sh orders by submission time, not by step name --------------------------------------
mkdir -p "$R/condor/runlog/aaa_20200101_000000" "$R/condor/runlog/zzz_20200101_000001"
/bin/bash "$ST" 3 > "$W/t16.out" 2>&1
check "T16 newest 3 by time (old aaa/zzz left out)" "! grep -qE '^(aaa|zzz)_' '$W/t16.out' && [[ \$(grep -cE '_[0-9]{8}_[0-9]{6}' '$W/t16.out') -eq 3 ]]"
/bin/bash "$ST" 100 > "$W/t16b.out" 2>&1
check "T16 oldest first"                 "[[ \$(sed -n 2p '$W/t16b.out' | cut -d' ' -f1) == aaa_20200101_000000 ]] && [[ \$(sed -n 3p '$W/t16b.out' | cut -d' ' -f1) == zzz_20200101_000001 ]]"
rm -rf "$R/condor/runlog/aaa_20200101_000000" "$R/condor/runlog/zzz_20200101_000001"

# ---- T17 condor_submit exit 0 without a cluster id: taken as queued, not as a failure ---------------
( cd "$R" && STUB_SUBMIT_NOID=1 /bin/bash "$CR" t17_noid -- true ) > "$W/t17.out" 2>&1; rc=$?
J17="$(ls -d "$R"/condor/runlog/t17_noid_* | head -1)"
check "T17 exit 0, cluster '?', warning" "[[ $rc -eq 0 && \$(cat '$J17/cluster.txt') == '?' ]] && grep -q 'no cluster id' '$W/t17.out' && ! grep -q FAILED '$W/t17.out'"

# ---- T18 condor_q failing does not turn queued jobs into 'gone' ---------------------------------------
STUB_Q_FAIL=1 /bin/bash "$ST" > "$W/t18.out" 2>&1
check "T18 condor_q failure -> unknown"  "grep -qE '^t10_idle_[0-9_]+ +6000 +unknown\\(condor_q' '$W/t18.out'"

# ---- T19 a very long command line: shown truncated, run in full ------------------------------------------
LONG=(); for i in $(seq 1 600); do LONG+=("/pnfs/knu.ac.kr/data/cms/store/user/x/file_$i.root"); done
export STUB_CLUSTER=7200
( cd "$R/sub" && /bin/bash "$CR" t19_long -- python3 -c 'import sys; print("NARGS", len(sys.argv) - 1)' "${LONG[@]}" ) > /dev/null 2>&1
J19="$(ls -d "$R"/condor/runlog/t19_long_* | head -1)"
run_worker "$J19"
L19="$R/$(cat "$J19/runlog_path.txt" 2>/dev/null)"
check "T19 all 600 arguments reached the command" "grep -q '^NARGS 600' '$L19'"
check "T19 header command truncated with a pointer" "grep -q '^cmd .*characters; full command in condor/runlog/t19_long_' '$L19'"

# ---- T20 --source: sourced after cmsenv in the workdir; failure is exit 94 ----------------------------
echo 'export FROM_SETUP=yes' > "$R/sub/mysetup.sh"
export STUB_CLUSTER=7300
( cd "$R/sub" && /bin/bash "$CR" --source mysetup.sh t20_src -- /bin/bash -c 'echo "SETUP=$FROM_SETUP"' ) > /dev/null 2>&1
J20="$(ls -d "$R"/condor/runlog/t20_src_* | head -1)"
run_worker "$J20"
check "T20 --source applied"             "grep -q '^SETUP=yes' '$R/'\$(cat '$J20/runlog_path.txt')"
( cd "$R/sub" && /bin/bash "$CR" --source nosuch.sh t20_bad -- true ) > /dev/null 2>&1; rc=$?
check "T20 --source missing file -> 2"  "[[ $rc -eq 2 ]]"
echo 'return 1' > "$R/sub/failsetup.sh"
( cd "$R/sub" && /bin/bash "$CR" --source failsetup.sh t20_fail -- true ) > /dev/null 2>&1
J20b="$(ls -d "$R"/condor/runlog/t20_fail_* | head -1)"
run_worker "$J20b"
check "T20 --source failing -> exit 94 in the record" "[[ \$(cat '$J20b/exit_code.txt') == 94 ]]"

# ---- T21 a record over 2 MB gets a note in the footer ------------------------------------------------------
/bin/bash "$RL" t21_big -- python3 -c 'print(("x" * 99 + "\n") * 30000, end="")' > /dev/null 2>&1
L21="$(ls "$R"/runlogs/run_t21_big_*.log | head -1)"
check "T21 log_kb line and large-record note" "grep -qE '^log_kb      : [0-9]+' '$L21' && grep -q '^NOTE        : large record' '$L21'"
check "T21 small record has no note"      "! grep -q 'large record' '$L1'"

# ---- T22 SIGTERM while the header is being written: footer, ledger, command not started ------------------
# a slow root-config (the header asks it for the ROOT version) holds runlog.sh in the header phase
mkdir -p "$W/slowbin"
cat > "$W/slowbin/root-config" <<EOF
#!/bin/bash
: > "$W/t22_in_header"; sleep 2; echo 6.99.99
EOF
chmod +x "$W/slowbin/root-config"
n0=$(wc -l < "$R/runlogs/LEDGER.tsv")
PATH="$W/slowbin:$PATH" /bin/bash "$RL" t22_early -- /bin/bash -c ": > '$W/t22_ran'" > "$W/t22.out" 2>&1 &
P22=$!
for _ in $(seq 1 300); do [[ -e "$W/t22_in_header" ]] && break; sleep 0.1; done
kill -TERM "$P22"; wait "$P22"; rc=$?
L22="$(ls "$R"/runlogs/run_t22_early_*.log 2>/dev/null | head -1)"
check "T22 exit 143, command not started" "[[ $rc -eq 143 && ! -e '$W/t22_ran' ]]"
check "T22 footer says before the command started" "grep -q '^stopped     : by SIGTERM before the command started' '$L22' && grep -q '^EXIT        : 143' '$L22'"
check "T22 ledger line, no mark file"    "[[ \$(( \$(wc -l < '$R/runlogs/LEDGER.tsv') - n0 )) -eq 1 ]] && tail -1 '$R/runlogs/LEDGER.tsv' | awk -F'\t' '\$2==\"t22_early\" && \$3==143 {f=1} END{exit !f}' && ! ls '$R'/runlogs/.runlog_mark_* >/dev/null 2>&1"

# ---- T23 submitted from a path through a symlink: the job files use the physical paths ----------------
ln -s "$R" "$W/link"
( cd "$W/link/sub" && /bin/bash "$W/link/tools/runlog/condor_run.sh" --dry-run t23_link -- true ) > /dev/null 2>&1; rc=$?
J23="$(ls -d "$R"/condor/runlog/t23_link_* 2>/dev/null | head -1)"
Q23="$(printf '%q' "$R/sub")"      # job.sh holds the path %q-quoted
check "T23 job under the physical repo, cd to the physical workdir" "[[ $rc -eq 0 ]] && grep -q '^executable *= $R/condor/runlog/t23_link_' '$J23/job.sub' && grep -qF -- 'cd $Q23 ||' '$J23/job.sh'"

wait_line () {   # <file> <exact line>: wait up to 30 s for it
  local _
  for _ in $(seq 1 300); do grep -qx -- "$2" "$1" 2>/dev/null && return 0; sleep 0.1; done
  return 1
}

# ---- T24 SIGTERM reaches what the command started, not only the command itself -----------------------
/bin/bash "$RL" t24_tree -- /bin/bash -c "( sleep 2; : > '$W/t24_late' ) & echo started; wait" > "$W/t24.out" 2>&1 &
P24=$!
wait_line "$W/t24.out" started
kill -TERM "$P24"; wait "$P24"; rc=$?
sleep 3
check "T24 the command's child is stopped too"  "[[ $rc -eq 143 && ! -e '$W/t24_late' ]]"

# ---- T25 a runlog.sh inside the command still completes its own record when the outer one is stopped ---
/bin/bash "$RL" t25_outer -- /bin/bash "$RL" t25_inner -- /bin/bash -c 'echo started; sleep 30; echo never' > "$W/t25.out" 2>&1 &
P25=$!
wait_line "$W/t25.out" started
kill -TERM "$P25"; wait "$P25"; rc=$?
L25o="$(ls "$R"/runlogs/run_t25_outer_*.log | head -1)"; L25i="$(ls "$R"/runlogs/run_t25_inner_*.log | head -1)"
check "T25 inner record: footer and EXIT 143"   "grep -q '^stopped     : by SIGTERM' '$L25i' && grep -q '^EXIT        : 143' '$L25i' && ! grep -qx never '$L25i'"
check "T25 outer record: has the inner footer, EXIT 143" "[[ $rc -eq 143 ]] && grep -q '^log         : runlogs/run_t25_inner_' '$L25o' && grep -q '^EXIT        : 143' '$L25o'"

# ---- T26 SIGHUP (the ssh session was lost) -------------------------------------------------------------
# under nohup SIGHUP is ignored from the start and bash cannot catch it: nothing to test then
hup_ignored () { local m; m="$(awk '/^SigIgn:/{print $2}' /proc/$$/status 2>/dev/null)"; [[ -n "$m" ]] && (( (16#$m & 1) != 0 )); }
if hup_ignored; then
  ok "T26 skipped (SIGHUP is ignored in this shell, e.g. under nohup)"
else
/bin/bash "$RL" t26_hup -- /bin/bash -c 'echo started; sleep 30; echo never' > "$W/t26.out" 2>&1 &
P26=$!
wait_line "$W/t26.out" started
kill -HUP "$P26"; wait "$P26"; rc=$?
L26="$(ls "$R"/runlogs/run_t26_hup_*.log | head -1)"
check "T26 SIGHUP: footer, EXIT 129, ledger"    "[[ $rc -eq 129 ]] && grep -q '^stopped     : by SIGHUP' '$L26' && grep -q '^EXIT        : 129' '$L26' && ! grep -qx never '$L26' && tail -1 '$R/runlogs/LEDGER.tsv' | awk -F'\t' '\$2==\"t26_hup\" && \$3==129 {f=1} END{exit !f}'"
fi

# ---- T27 a signal after the command has ended: the record is completed, with the command's exit code ----
# a slow find (the footer's list of changed files) holds runlog.sh after the command
REALFIND="$(command -v find)"
mkdir -p "$W/slowfind"
printf '#!/bin/bash\n: > %q; sleep 2; exec %q "$@"\n' "$W/t27_in_footer" "$REALFIND" > "$W/slowfind/find"
chmod +x "$W/slowfind/find"
PATH="$W/slowfind:$PATH" /bin/bash "$RL" t27_late -- /bin/bash -c 'echo body; exit 5' > "$W/t27.out" 2>&1 &
P27=$!
for _ in $(seq 1 300); do [[ -e "$W/t27_in_footer" ]] && break; sleep 0.1; done
kill -TERM "$P27"; wait "$P27"; rc=$?
L27="$(ls "$R"/runlogs/run_t27_late_*.log | head -1)"
check "T27 signal in the footer phase: footer, ledger, the command's exit 5" "[[ $rc -eq 5 ]] && grep -q '^EXIT        : 5' '$L27' && ! grep -q '^stopped' '$L27' && tail -1 '$R/runlogs/LEDGER.tsv' | awk -F'\t' '\$2==\"t27_late\" && \$3==5 {f=1} END{exit !f}'"

# ---- T28 a command that ignores SIGTERM: SIGKILL after RUNLOG_KILL_AFTER s, the record is still complete --
/bin/bash -c "RUNLOG_KILL_AFTER=1 exec /bin/bash '$RL' t28_stubborn -- /bin/bash -c 'trap \"\" TERM; echo started; sleep 30; echo never'" > "$W/t28.out" 2>&1 &
P28=$!
wait_line "$W/t28.out" started
t0=$(date +%s); kill -TERM "$P28"; wait "$P28"; rc=$?; t1=$(date +%s)
L28="$(ls "$R"/runlogs/run_t28_stubborn_*.log | head -1)"
check "T28 SIGTERM ignored by the command: SIGKILL, footer, EXIT 143 within seconds" "[[ $rc -eq 143 && $((t1 - t0)) -le 10 ]] && grep -q '^stopped     : by SIGTERM' '$L28' && grep -q '^EXIT        : 143' '$L28' && ! grep -qx never '$L28'"

# ---- T29 no procps (pgrep, ps) on PATH: the command is still found (through /proc) and stopped ------------
if [[ -r /proc/$$/task/$$/children ]]; then
  mkdir -p "$W/nops"
  for f in /usr/bin/* /bin/*; do
    b="${f##*/}"; case "$b" in pgrep|pkill|ps|pidof|pidwait) continue ;; esac
    [[ -e "$W/nops/$b" ]] || ln -s "$f" "$W/nops/$b" 2>/dev/null
  done
  PATH="$S:$W/nops" /bin/bash "$RL" t29_nops -- /bin/bash -c "echo \$\$ > '$W/t29.pid'; echo started; sleep 30; echo never" > "$W/t29.out" 2>&1 &
  P29=$!
  wait_line "$W/t29.out" started
  kill -TERM "$P29"; wait "$P29"; rc=$?
  L29="$(ls "$R"/runlogs/run_t29_nops_*.log | head -1)"
  check "T29 without pgrep/ps: stopped through /proc, EXIT 143" "[[ $rc -eq 143 ]] && grep -q '^EXIT        : 143' '$L29' && ! grep -q '^NOTE        : no pgrep' '$L29' && ! kill -0 \$(cat '$W/t29.pid') 2>/dev/null"
else
  ok "T29 skipped (no /proc children list on this system)"
fi

# ---- T30 [STEP 26 M] the stall guard in job.sub: off by default, --stall-guard on, a bad value -------------------
# (that the five expressions equal the analyzer jobs' ones: test/test_failure_checks.py part I)
( cd "$R" && /bin/bash "$CR" --dry-run --stall-guard on t30_on -- true ) > "$W/t30.out" 2>&1; r30a=$?
( cd "$R" && /bin/bash "$CR" --dry-run t30_def -- true ) > "$W/t30b.out" 2>&1; r30b=$?
( cd "$R" && /bin/bash "$CR" --dry-run --stall-guard yes t30_bad -- true ) > /dev/null 2>&1; r30c=$?
J30="$(ls -d "$R"/condor/runlog/t30_on_* | head -1)"; J30b="$(ls -d "$R"/condor/runlog/t30_def_* | head -1)"
n30=$(grep -cE '^(periodic_hold|periodic_hold_reason|periodic_hold_subcode|periodic_release|requirements) +=' "$J30/job.sub" 2>/dev/null); n30=${n30:--1}
n30b=$(grep -cE '^(periodic_|requirements)' "$J30b/job.sub" 2>/dev/null); n30b=${n30b:--1}
check "T30 --stall-guard on: five lines, subcode 4201, queue 1 last, the note" "[[ $r30a -eq 0 && $n30 -eq 5 ]] && grep -q '^periodic_hold_subcode *= 4201\$' '$J30/job.sub' && [[ \"\$(tail -1 '$J30/job.sub')\" == 'queue 1' ]] && grep -q 'stall guard: on (hold after 1 h' '$W/t30.out'"
check "T30 default off: none of them, the rest the same; a bad value -> 2" "[[ $r30b -eq 0 && $n30b -eq 0 && $r30c -eq 2 ]] && diff <(grep -vE '^(periodic_|requirements|#)' '$J30/job.sub' | sed 's#t30_on_[0-9_]*#X#g') <(sed 's#t30_def_[0-9_]*#X#g' '$J30b/job.sub') > /dev/null && grep -q 'stall guard: off' '$W/t30b.out'"

echo "RESULT: $NP PASS, $NF FAIL"
[[ $NF -eq 0 ]]
