#!/bin/bash
# =============================================================================
#  treestream_v15check.sh -- the KNU check of the patched treestream fork in one
#  go (workspace RUNBOOK section 20 step 12; STEP 23, 2026-10-04):
#    1. build the library with fresh ROOT dictionaries (rm src/*_dict.cc; make lib)
#    2. test/v15check/run_tests.sh   synthetic files with the real 2024 v15 schema
#    3. test/v15check/run_realfiles.sh   real ntuples: eventBuffer vs PyROOT
#
#    /bin/bash tools/stage0/treestream_v15check.sh <treestream dir> <NtupleForge dir> [mc.root dataC.root dataI.root]
#
#  Without the three files it takes the first file of TTto4Q (MC), JetMET0
#  Run2024C and JetMET0 Run2024I under $KNU_STORE
#  (default /pnfs/knu.ac.kr/data/cms/store/user/junghyun), skipping failed/.
#  git pull/clone of the two checkouts is NOT done here (it needs your ssh key):
#  do it before submitting.
#  Writes under $WORK (default $TMPDIR/tsv15check) and in the treestream dir
#  (lib/, regenerated src/*_dict.cc -- never commit those).
#  ROOT 'Warning in <...>' lines are counted, not printed.
#  Exit: 0 only if the build worked, the synthetic-test harness compiled and
#  every real file gave RESULT PASS. Read the block lines against
#  test/v15check/README.md (T3, T4 and T8 exit 1 by design).
# =============================================================================
set -u
[[ $# -eq 2 || $# -eq 5 ]] || { sed -n '2,22p' "$0" | sed 's/^# \{0,2\}//'; exit 2; }
TS="$(cd "$1" 2>/dev/null && pwd -P)" || { echo "no such treestream dir: $1"; exit 2; }
NF="$(cd "$2" 2>/dev/null && pwd -P)" || { echo "no such NtupleForge dir: $2"; exit 2; }
for f in bin/mkanalyzer.py test/v15check/run_tests.sh test/v15check/run_realfiles.sh; do
  [[ -f "$TS/$f" ]] || { echo "not the patched treestream (missing $f): $TS"; exit 2; }
done
[[ -d "$NF/script/inventory" && -d "$NF/branches" ]] || { echo "not an NtupleForge checkout: $NF"; exit 2; }
WORK="${WORK:-${TMPDIR:-/tmp}/tsv15check}"
mkdir -p "$WORK" || exit 2

filter () {   # drop ROOT warning lines, count them
  awk '/^Warning in </{n++; next} {print; fflush()} END{if (n) printf "(%d ROOT \"Warning in <...>\" lines not shown)\n", n}'
}

echo "treestream  : $TS @ $(git -C "$TS" log --oneline -1 2>/dev/null || echo '?')"
echo "NtupleForge : $NF @ $(git -C "$NF" log --oneline -1 2>/dev/null || echo '?')"
echo "WORK        : $WORK"

echo "== 1. build (fresh dictionaries)"
( cd "$TS" && rm -f src/*_dict.cc && make lib ) > "$WORK/ts_build.log" 2>&1
BRC=$?
echo "make lib rc=$BRC"
ls "$TS/lib/" 2>&1
[[ $BRC -eq 0 ]] || { echo "--- tail of the build log ---"; tail -30 "$WORK/ts_build.log"; }

echo "== 2. synthetic files (test/v15check/run_tests.sh)"
TRC=99
if [[ $BRC -eq 0 ]]; then
  TS="$TS" NF="$NF" WORK="$WORK/tscheck" /bin/bash "$TS/test/v15check/run_tests.sh" 2>&1 | filter
  TRC=${PIPESTATUS[0]}
  echo "run_tests.sh rc=$TRC   (2 = WORK not usable, 3 = the harness did not compile; 0/1 = it ran, read the blocks)"
else
  echo "skipped: no library"
fi

echo "== 3. real files (test/v15check/run_realfiles.sh)"
RRC=99
if [[ $# -eq 5 ]]; then
  MC="$3"; DC="$4"; DI="$5"
else
  B="${KNU_STORE:-/pnfs/knu.ac.kr/data/cms/store/user/junghyun}"
  MC=$(find "$B/ttHH2024_v15_had_MC_v1/TTto4Q_TuneCP5_13p6TeV_powheg-pythia8" -name 'forgedNtuple_*.root' -not -path '*/failed/*' -print -quit 2>/dev/null)
  DC=$(find "$B/ttHH2024_v15_had_Data_v1/JetMET0" -path '*Run2024C*' -name 'forgedNtuple_*.root' -not -path '*/failed/*' -print -quit 2>/dev/null)
  DI=$(find "$B/ttHH2024_v15_had_Data_v1/JetMET0" -path '*Run2024I*' -name 'forgedNtuple_*.root' -not -path '*/failed/*' -print -quit 2>/dev/null)
fi
echo "MC : ${MC:-<none found>}"
echo "DC : ${DC:-<none found>}"
echo "DI : ${DI:-<none found>}"
if [[ $BRC -ne 0 ]]; then
  echo "skipped: no library"
elif [[ -z "$MC" || -z "$DC" || -z "$DI" ]]; then
  echo "skipped: a file was not found"
else
  TS="$TS" WORK="$WORK/tsreal" NMAX="${NMAX:-50}" /bin/bash "$TS/test/v15check/run_realfiles.sh" "$MC" "$DC" "$DI" 2>&1 | filter
  RRC=${PIPESTATUS[0]}
  echo "run_realfiles.sh rc=$RRC"
fi

OK=1
[[ $BRC -eq 0 ]] || OK=0
[[ $TRC -eq 0 || $TRC -eq 1 ]] || OK=0   # 1 = the last grep (T9) found nothing: a finding, read the block
[[ $RRC -eq 0 ]] || OK=0
echo "SUMMARY build_rc=$BRC run_tests_rc=$TRC realfiles_rc=$RRC -> $([[ $OK -eq 1 ]] && echo PASS || echo FAIL)"
[[ $OK -eq 1 ]]
