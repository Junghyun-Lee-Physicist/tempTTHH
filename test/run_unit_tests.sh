#!/bin/bash
# =============================================================================
#  run_unit_tests.sh -- the three analyzer unit tests (no ROOT needed), one
#  RESULT line each and a SUMMARY.   (STEP 23, 2026-10-04)
#
#    /bin/bash test/run_unit_tests.sh          (from any directory)
#
#  test_EraConfig.cc        year/era tables            -> ALL ERACONFIG ASSERTIONS PASSED
#  test_EraConfig_fatal.sh  an unknown year is exit 11 -> PASS: unknown year -> exit 11 ...
#  test_SampleRegistry.cc   aliases, stitch scope, era letter, MC weight
#                           -> PASS <n> / FAIL 0. It reads data/samples_2017UL.json
#                           and prescan_summary/prescan_summary.json OF THIS
#                           CHECKOUT, so a locally modified prescan summary can
#                           change its result: the md5 and the git state of the
#                           two inputs are printed first.
#  Exit: 0 only if all three pass.
# =============================================================================
set -u
cd "$(dirname "${BASH_SOURCE[0]}")/.." || exit 2
T="$(mktemp -d "${TMPDIR:-/tmp}/tthh_unit.XXXXXX")" || exit 2
trap 'rm -rf "$T"' EXIT
FAIL=0

echo "--- inputs of test_SampleRegistry ---"
for f in data/samples_2017UL.json prescan_summary/prescan_summary.json; do
  st="$(git --no-optional-locks status --porcelain -- "$f" 2>/dev/null | cut -c1-2)"
  printf '%s  %s  git:%s\n' "$(md5sum "$f" 2>/dev/null | cut -d' ' -f1)" "$f" "${st:-clean}"
done

echo "--- test_EraConfig ---"
if g++ -std=c++17 -I include -o "$T/t_era" test/test_EraConfig.cc; then
  OUT="$("$T/t_era" 2>&1)"; RC=$?
  printf '%s\n' "$OUT" | tail -3
  if [[ $RC -eq 0 ]] && grep -q "ALL ERACONFIG ASSERTIONS PASSED" <<< "$OUT"; then
    echo "RESULT test_EraConfig PASS"
  else
    echo "RESULT test_EraConfig FAIL (exit $RC)"; FAIL=1
  fi
else
  echo "RESULT test_EraConfig FAIL (compile)"; FAIL=1
fi

echo "--- test_EraConfig_fatal ---"
OUT="$(/bin/bash test/test_EraConfig_fatal.sh 2>&1)"; RC=$?
printf '%s\n' "$OUT" | head -3
if [[ $RC -eq 0 ]]; then echo "RESULT test_EraConfig_fatal PASS"; else echo "RESULT test_EraConfig_fatal FAIL (exit $RC)"; FAIL=1; fi

echo "--- test_SampleRegistry ---"
if g++ -std=c++17 -I include -I . -I bTagSF_ReweightStudy/include -o "$T/t_sr" test/test_SampleRegistry.cc; then
  OUT="$(TTHH_BASE=. "$T/t_sr" 2>&1)"; RC=$?
  printf '%s\n' "$OUT" | grep -E '\[FAIL\]|PASS [0-9]+ / FAIL' | head -40
  if [[ $RC -eq 0 ]]; then echo "RESULT test_SampleRegistry PASS"; else echo "RESULT test_SampleRegistry FAIL (exit $RC)"; FAIL=1; fi
else
  echo "RESULT test_SampleRegistry FAIL (compile)"; FAIL=1
fi

echo "SUMMARY unit_tests $([[ $FAIL -eq 0 ]] && echo PASS || echo FAIL)"
exit $FAIL
