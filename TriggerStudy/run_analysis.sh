#!/bin/bash

# ========================================================
# Full Trigger SF Analysis Pipeline (Config Driven)
# ========================================================

set -euo pipefail

outputDirName="outputs"

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

# ========================================================
# 1. Step 1: Calculate Efficiency (Mode 0)
# ========================================================
echo ">>> [Step 1] Running EventLooper..."

pids=()
./exe_TrigStudy TTTo2L2Nu 0 &          pids+=($!)
./exe_TrigStudy TTToHadronic 0 &        pids+=($!)
./exe_TrigStudy TTToSemiLeptonic 0 &    pids+=($!)
./exe_TrigStudy SingleMuon_B 0 &        pids+=($!)
./exe_TrigStudy SingleMuon_C 0 &        pids+=($!)
./exe_TrigStudy SingleMuon_D 0 &        pids+=($!)
./exe_TrigStudy SingleMuon_E 0 &        pids+=($!)
./exe_TrigStudy SingleMuon_F 0 &        pids+=($!)
wait_and_check "${pids[@]}"

echo ">>> [Step 1] All EventLooper jobs succeeded."

mkdir -p "$outputDirName"

echo ">>> [Step 1] Merging Files..."
hadd -f output_SingleMuon.root output_SingleMuon_*.root
hadd -f output_TTbarInc.root output_TTTo2L2Nu.root output_TTToHadronic.root output_TTToSemiLeptonic.root

# ========================================================
# 2. Derive SF Histograms
# ========================================================
echo ">>> [Derive SF] Calculating Scale Factors..."
root -l -b -q "DeriveSF.cpp"

# ========================================================
# 3. Step 2: Apply SF (Mode 1)
# ========================================================
echo ">>> [Step 2] Applying Corrections..."

pids=()
./exe_TrigStudy TTTo2L2Nu 1 &          pids+=($!)
./exe_TrigStudy TTToHadronic 1 &        pids+=($!)
./exe_TrigStudy TTToSemiLeptonic 1 &    pids+=($!)
./exe_TrigStudy SingleMuon_B 1 &        pids+=($!)
./exe_TrigStudy SingleMuon_C 1 &        pids+=($!)
./exe_TrigStudy SingleMuon_D 1 &        pids+=($!)
./exe_TrigStudy SingleMuon_E 1 &        pids+=($!)
./exe_TrigStudy SingleMuon_F 1 &        pids+=($!)
wait_and_check "${pids[@]}"

echo ">>> [Step 2] All validation jobs succeeded."

echo ">>> [Step 2] Merging Corrected Files..."
hadd -f validated_SingleMuon.root validated_SingleMuon_*.root
hadd -f validated_TTbarInc.root validated_TTTo*.root

# ========================================================
# 4. Plotting
# ========================================================
echo ">>> [Plotting] Generating Validation Plots..."
root -l -b -q "PlotTriggerEfficiency.cpp"

echo ">>> All Analysis Steps Completed!"
