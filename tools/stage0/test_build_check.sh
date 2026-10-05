#!/bin/bash
# =============================================================================
#  test_build_check.sh -- offline tests of tools/stage0/build_check.sh
#  (2026-10-04 (2)). A throw-away repo with a fake Makefile stands in for the
#  analyzer: no compiler, ROOT or CMSSW needed. GNU touch/stat (Linux).
#  Time offsets are set exactly (touch -d @<epoch>, 30 min from the other
#  file): KNU's first run failed B7 because "20 minutes ago" taken a second
#  after the build is 19 whole minutes (10-05). A failing check prints the
#  build_check output it looked at.
#
#    /bin/bash tools/stage0/test_build_check.sh     -> RESULT: <n> PASS, <m> FAIL
# =============================================================================
set -u
SRC="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
W="$(mktemp -d "${TMPDIR:-/tmp}/test_build_check.XXXXXX")" || exit 2
trap 'rm -rf "$W"' EXIT
W="$(cd "$W" && pwd -P)" || exit 2
NP=0; NF=0
ok ()  { NP=$((NP + 1)); echo "PASS $1"; }
bad () { NF=$((NF + 1)); echo "FAIL $1"; }
check () {   # <name> <condition> [output file to show when it fails]
  if eval "$2"; then ok "$1"; else bad "$1"; [[ -n "${3:-}" && -f "$3" ]] && sed 's/^/      | /' "$3"; fi
}
mtime_of () { stat -c %Y "$1"; }
set_mtime () { touch -d "@$2" "$1"; }   # <file> <epoch seconds>

R="$W/repo"; mkdir -p "$R/tools/stage0" "$R/include" "$R/src"
cp "$SRC/build_check.sh" "$R/tools/stage0/"
echo '// eventBuffer' > "$R/include/eventBuffer.h"; echo '// treestream' > "$R/src/treestream.cc"
BC="$R/tools/stage0/build_check.sh"
# fake Makefile; FAKE_MODE = ok | linkfail | nobinary (recipes need tabs: printf)
{
  printf 'FAKE_MODE ?= ok\n'
  printf 'ifdef VERBOSE\n$(info [Log Verbosity] : Verbose log output enabled)\nelse\n$(info [Log Verbosity] : Verbose log output disabled)\nendif\n'
  printf 'all:\n'
  printf '\t@mkdir -p lib && echo lib > lib/libToolsForAnalysis.so && echo "src/x.cc:1:1: warning: unused variable"\n'
  printf 'ifeq ($(FAKE_MODE),ok)\n'
  printf '\t@echo app > ttHHanalyzer_unified && chmod +x ttHHanalyzer_unified\n'
  printf 'ifdef VERBOSE\n'
  printf '\t@echo "[Done] Built shared lib=lib/libToolsForAnalysis.so and apps=ttHHanalyzer_unified"\n'
  printf 'endif\n'
  printf 'endif\n'
  printf 'ifeq ($(FAKE_MODE),linkfail)\n'
  printf '\t@echo "collect2: error: ld returned 1 exit status" && false\n'
  printf 'endif\n'
  printf 'clean:\n'
  printf '\t@rm -rf lib ttHHanalyzer_unified\n'
} > "$R/Makefile"
newest_log () { ls -t "$R"/condor/build/build_*.log 2>/dev/null | head -1; }

# ---- B1 a complete build ------------------------------------------------------------------------
FAKE_MODE=ok /bin/bash "$BC" -j 2 > "$W/b1.out" 2>&1; rc=$?
check "B1 complete build -> exit 0, result OK"   "[[ $rc -eq 0 ]] && grep -q '^make_rc     : 0' '$W/b1.out' && grep -q '^result      : OK' '$W/b1.out'" "$W/b1.out"
check "B1 make log in condor/build/, VERBOSE=1 lines in the last lines" "[[ -s \"\$(newest_log)\" ]] && grep -q '^--- last 5 lines of the make log ---' '$W/b1.out' && grep -q '^\[Done\] Built' '$W/b1.out'"

# ---- B2 the link fails: make's exit code, the first error ----------------------------------------
FAKE_MODE=linkfail /bin/bash "$BC" > "$W/b2.out" 2>&1; rc=$?
check "B2 link failure -> make's exit code, FAIL" "[[ $rc -eq 2 ]] && grep -q '^result      : FAIL (make exited 2; no executable' '$W/b2.out' && grep -q 'collect2: error' '$W/b2.out'" "$W/b2.out"

# ---- B3 make says 0 but there is no executable -----------------------------------------------------
FAKE_MODE=nobinary /bin/bash "$BC" > "$W/b3.out" 2>&1; rc=$?
check "B3 make 0 without executable -> exit 1" "[[ $rc -eq 1 ]] && grep -q '^make_rc     : 0' '$W/b3.out' && grep -q '^result      : FAIL (no executable ttHHanalyzer_unified' '$W/b3.out'" "$W/b3.out"

# ---- B4 --from-log, no error line, executable missing (the KNU build of 10-03) ---------------------
printf 'CorrectionLib flags\nsrc/a.cc:1:1: warning: x\ng++ -c src/b.cc\n' > "$W/interrupted.log"
( cd "$R" && rm -f ttHHanalyzer_unified && mkdir -p lib && echo lib > lib/libToolsForAnalysis.so )
/bin/bash "$BC" --from-log "$W/interrupted.log" > "$W/b4.out" 2>&1; rc=$?
check "B4 from-log, 0 errors, no executable -> exit 1" "[[ $rc -eq 1 ]] && grep -q '^errors      : 0 ' '$W/b4.out' && grep -q '^result      : FAIL (no executable' '$W/b4.out' && grep -q 'g++ -c src/b.cc' '$W/b4.out'" "$W/b4.out"

# ---- B5 the same log after a later build made an executable: still FAIL ------------------------------
FAKE_MODE=ok /bin/bash "$BC" > /dev/null 2>&1
set_mtime "$W/interrupted.log" $(( $(mtime_of "$R/ttHHanalyzer_unified") - 1800 ))   # the executable: 30 min newer
/bin/bash "$BC" --from-log "$W/interrupted.log" > "$W/b5.out" 2>&1; rc=$?
check "B5 quiet log, executable from a later build -> exit 1" "[[ $rc -eq 1 ]] && grep -q '^result      : FAIL (ttHHanalyzer_unified is 30 min newer than the quiet log' '$W/b5.out'" "$W/b5.out"

# ---- B6 --from-log of a complete build -----------------------------------------------------------------
B6LOG="$(newest_log)"
/bin/bash "$BC" --from-log "$B6LOG" > "$W/b6.out" 2>&1; rc=$?
check "B6 from-log of a complete (VERBOSE) build -> exit 0" "[[ $rc -eq 0 ]] && grep -q '^done_line   : yes' '$W/b6.out' && grep -q '^result      : OK (the log shows a complete build' '$W/b6.out'" "$W/b6.out"

# ---- B7 --from-log, the executable is much older than the log ---------------------------------------
set_mtime "$R/ttHHanalyzer_unified" $(( $(mtime_of "$B6LOG") - 1800 ))   # 30 min older than the log
/bin/bash "$BC" --from-log "$B6LOG" > "$W/b7.out" 2>&1; rc=$?
check "B7 from-log, executable 30 min older than the log -> exit 1" "[[ $rc -eq 1 ]] && grep -q '^result      : FAIL (ttHHanalyzer_unified is 30 min older than the log' '$W/b7.out'" "$W/b7.out"
touch "$R/ttHHanalyzer_unified" "$B6LOG"

# ---- B8 --from-log: error lines, make's own '***' lines, an empty log -------------------------------
printf 'src/a.cc:3:5: error: expected ;\nmake: *** [Makefile:9: all] Error 1\n' > "$W/err.log"
/bin/bash "$BC" --from-log "$W/err.log" > "$W/b8a.out" 2>&1; r1=$?
printf 'src/a.cc:1:1: warning: x\nmake: *** [Makefile:205: tmp/ttHHanalyzer_unified.o] Interrupt\n' > "$W/int.log"
/bin/bash "$BC" --from-log "$W/int.log" > "$W/b8b.out" 2>&1; r2=$?
printf 'Makefile:37: *** Please set up ROOTSYS (e.g. source /path/to/thisroot.sh).  Stop.\n' > "$W/stop.log"
/bin/bash "$BC" --from-log "$W/stop.log" > "$W/b8c.out" 2>&1; r3=$?
: > "$W/empty.log"
/bin/bash "$BC" --from-log "$W/empty.log" > "$W/b8d.out" 2>&1; r4=$?
check "B8 error lines -> exit 1, first error shown" "[[ $r1 -eq 1 ]] && grep -q '^--- first error' '$W/b8a.out' && grep -q '^result      : FAIL (the log has error lines)' '$W/b8a.out'"
check "B8 make's Interrupt / Stop lines count as errors" "[[ $r2 -eq 1 && $r3 -eq 1 ]] && grep -q '^errors      : 1 ' '$W/b8b.out' && grep -q '^errors      : 1 ' '$W/b8c.out'"
check "B8 empty log -> exit 1"                   "[[ $r4 -eq 1 ]] && grep -q '^result      : FAIL (the log is empty' '$W/b8d.out'"

# ---- B9 bad usage; an empty --from-log value must not start a build --------------------------------
n_logs=$(ls "$R"/condor/build/ | wc -l); t_app=$(stat -c %Y "$R/ttHHanalyzer_unified")
/bin/bash "$BC" -j 0 > /dev/null 2>&1; r1=$?
/bin/bash "$BC" --from-log /no/such/file > /dev/null 2>&1; r2=$?
/bin/bash "$BC" --bogus > /dev/null 2>&1; r3=$?
/bin/bash "$BC" --from-log "" > /dev/null 2>&1; r4=$?
/bin/bash "$BC" --from-log "$W" > /dev/null 2>&1; r5=$?
check "B9 bad arguments -> exit 2"           "[[ $r1 -eq 2 && $r2 -eq 2 && $r3 -eq 2 && $r5 -eq 2 ]]"
check "B9 --from-log '' -> exit 2, no build" "[[ $r4 -eq 2 && \$(ls '$R'/condor/build/ | wc -l) -eq $n_logs && \$(stat -c %Y '$R/ttHHanalyzer_unified') -eq $t_app ]]"

# ---- B10 run from other directories: -h, and a relative --from-log path -------------------------------
mkdir -p "$W/elsewhere"; cp "$B6LOG" "$W/elsewhere/rel.log"; touch "$R/ttHHanalyzer_unified" "$W/elsewhere/rel.log"
( cd "$R/tools/stage0" && /bin/bash build_check.sh -h ) > "$W/b10a.out" 2>&1; r1=$?
( cd "$W/elsewhere" && /bin/bash ../repo/tools/stage0/build_check.sh --from-log rel.log ) > "$W/b10b.out" 2>&1; r2=$?
check "B10 -h from another directory"         "[[ $r1 -eq 0 ]] && grep -q 'build_check.sh -- clean build of the analyzer' '$W/b10a.out' && ! grep -q '^set -u' '$W/b10a.out'"
check "B10 relative --from-log path"          "[[ $r2 -eq 0 ]] && grep -q '^result      : OK' '$W/b10b.out'"

# ---- B11 a source line echoed under a warning is not an error -----------------------------------------
printf 'src/x.cc:10:5: warning: unused variable [-Wunused-variable]\n   10 |   Double_t ***p; // error: not really ***\n      |              ^\n' > "$W/echo.log"
touch "$R/ttHHanalyzer_unified" "$W/echo.log"
/bin/bash "$BC" --from-log "$W/echo.log" > "$W/b11.out" 2>&1; rc=$?
check "B11 echoed source line with '***'/'error:' is not counted" "[[ $rc -eq 0 ]] && grep -q '^errors      : 0 ' '$W/b11.out' && grep -q '^warnings    : 1 ' '$W/b11.out'" "$W/b11.out"

# ---- B12 VERBOSE logs: no [Done] line = not finished; with it, a later executable is fine ----------------
printf '[Log Verbosity] : Verbose log output enabled\nsrc/a.cc:1:1: warning: x\n' > "$W/v_nodone.log"
touch "$R/ttHHanalyzer_unified" "$W/v_nodone.log"
/bin/bash "$BC" --from-log "$W/v_nodone.log" > "$W/b12a.out" 2>&1; r1=$?
cp "$B6LOG" "$W/v_done.log"; touch "$R/ttHHanalyzer_unified"; set_mtime "$W/v_done.log" $(( $(mtime_of "$R/ttHHanalyzer_unified") - 1800 ))
/bin/bash "$BC" --from-log "$W/v_done.log" > "$W/b12b.out" 2>&1; r2=$?
check "B12 VERBOSE log without [Done] -> exit 1" "[[ $r1 -eq 1 ]] && grep -q '^done_line   : no' '$W/b12a.out' && grep -q 'no \[Done\] line' '$W/b12a.out'" "$W/b12a.out"
check "B12 VERBOSE log with [Done], later executable -> exit 0" "[[ $r2 -eq 0 ]] && grep -q '^result      : OK (the log shows a complete build' '$W/b12b.out'" "$W/b12b.out"

echo "RESULT: $NP PASS, $NF FAIL"
[[ $NF -eq 0 ]]
