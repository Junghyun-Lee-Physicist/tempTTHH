#!/usr/bin/env bash
# EraConfig 의 unknown-year 경로가 정말 FATAL(exit 11) 인지 확인한다.
# "모르는 연도는 조용히 기본값으로 넘어가지 않는다" 가 이 파일의 핵심 계약이므로,
# 주석이 아니라 테스트로 고정한다.
set -o pipefail
cd "$(dirname "$0")/.."
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

cat > "$tmp/f.cc" <<'CC'
#include "EraConfig.h"
int main(int argc, char**){
    // argc 로 분기해 컴파일러가 상수 접기로 없애지 못하게 한다
    const std::string bogus = (argc > 99) ? "2017" : "2015";
    (void)EraConfig::yearForCorr(bogus);   // 여기서 exit(11) 이어야 한다
    return 0;                              // 도달하면 실패
}
CC

g++ -std=c++17 -I include -o "$tmp/f" "$tmp/f.cc" || { echo "FAIL: compile"; exit 1; }
out=$("$tmp/f" 2>&1); rc=$?

fail=0
if [ "$rc" -ne 11 ]; then
    echo "FAIL: expected exit 11 (CONFIG_BAD_RUNINFO), got $rc"; fail=1
fi
if ! grep -q "unsupported runYear" <<<"$out"; then
    echo "FAIL: message did not mention the bad year"; fail=1
fi
if [ "$fail" -eq 0 ]; then
    echo "PASS: unknown year -> exit 11 with a diagnostic"
    echo "--- stderr was ---"; echo "$out"
fi
exit "$fail"
