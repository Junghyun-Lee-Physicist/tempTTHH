#!/bin/bash

# ========================================================
# Full Trigger SF Analysis Pipeline (Config Driven)
# ========================================================
#
# [2026-07-29] STEP18 campaign(`ttHH2017UL_fullNano_v20`) 이름으로 갱신.
#   구:  TTTo2L2Nu / TTToHadronic / TTToSemiLeptonic / SingleMuon_B..F
#   신:  TTbar_DiLep / TTbar_Hadronic / TTbar_SemiLep / SingleMuon_Run2017B..F
#
#   샘플 이름은 곧 **입력 파일 이름**이다 ($TTHH_SKIM_DIR/<name>.root). 구 이름을
#   쓰면 파일을 못 열고 즉시 exit(1) 하므로 조용히 틀릴 여지는 없지만, 어차피
#   registry 조회도 campaign 이름 기준이라 여기서 통일한다.
#
# 필요한 환경변수:
#   TTHH_SKIM_DIR   btagtrig skim 이 있는 디렉토리 (미설정 시 Config.hh 기본값)
#   TTHH_BASE       analyzer 프로젝트 루트 (default "..", 여기선 그대로 맞음)
# ========================================================

set -euo pipefail

outputDirName="outputs"

# ── 샘플 목록 (여기 한 곳만 고치면 전 단계에 반영된다) ──────────────────────
MC_SAMPLES=(TTbar_DiLep TTbar_Hadronic TTbar_SemiLep)
DATA_SAMPLES=(SingleMuon_Run2017B SingleMuon_Run2017C SingleMuon_Run2017D \
              SingleMuon_Run2017E SingleMuon_Run2017F)

# ========================================================
# Helper: 병렬 실행 후 모든 프로세스의 종료 코드 검증
# ========================================================
wait_and_check() {
    local pids=("$@")
    local failed=0
    for pid in "${pids[@]}"; do
        if ! wait "$pid"; then
            echo "[ERROR] PID $pid failed" >&2
            failed=1
        fi
    done
    if [[ $failed -ne 0 ]]; then
        echo "[FATAL] One or more processes failed. Aborting pipeline." >&2
        exit 1
    fi
}

# "<prefix><sample>.root" 목록을 stdout 으로 (hadd 인자용)
files_for() {
    local prefix="$1"; shift
    local s
    for s in "$@"; do printf '%s%s.root ' "$prefix" "$s"; done
}

# mode(0=eff, 1=applySF) 로 전 샘플을 병렬 실행
run_all_samples() {
    local mode="$1"
    local pids=()
    for s in "${MC_SAMPLES[@]}" "${DATA_SAMPLES[@]}"; do
        ./exe_TrigStudy "$s" "$mode" & pids+=($!)
    done
    wait_and_check "${pids[@]}"
}

# ========================================================
# 0. Preflight — 조용한 실패의 대부분이 여기서 걸린다
# ========================================================
if [[ ! -x ./exe_TrigStudy ]]; then
    echo "[FATAL] ./exe_TrigStudy not found. Run 'make' first." >&2
    exit 1
fi

# ========================================================
# 1. Step 1: Calculate Efficiency (Mode 0)
# ========================================================
echo ">>> [Step 1] Running EventLooper..."
run_all_samples 0
echo ">>> [Step 1] All EventLooper jobs succeeded."

mkdir -p "$outputDirName"

echo ">>> [Step 1] Merging Files..."
hadd -f output_SingleMuon.root $(files_for output_ "${DATA_SAMPLES[@]}")
hadd -f output_TTbarInc.root   $(files_for output_ "${MC_SAMPLES[@]}")

# ========================================================
# 2. Derive SF Histograms
# ========================================================
echo ">>> [Derive SF] Calculating Scale Factors..."
root -l -b -q "DeriveSF.cpp"

# ========================================================
# 3. Step 2: Apply SF (Mode 1)
# ========================================================
echo ">>> [Step 2] Applying Corrections..."
run_all_samples 1
echo ">>> [Step 2] All validation jobs succeeded."

echo ">>> [Step 2] Merging Corrected Files..."
hadd -f validated_SingleMuon.root $(files_for validated_ "${DATA_SAMPLES[@]}")
hadd -f validated_TTbarInc.root   $(files_for validated_ "${MC_SAMPLES[@]}")

# ========================================================
# 4. Plotting
# ========================================================
echo ">>> [Plotting] Generating Validation Plots..."
root -l -b -q "PlotTriggerEfficiency.cpp"

echo ">>> All Analysis Steps Completed!"
