#!/bin/bash
# ============================================================================
# run_btag_pipeline.sh
#
# B-tag Shape SF Reweighting Pipeline
#
# Step 0: Compute normalization ratios per nJets bin (MC only)
# Step 1: Apply b-tag SF + trigger SF + normalization, produce validation plots
#
# Usage:
#   bash run_btag_pipeline.sh
#
# Prerequisites:
#   - exe_BTagSF compiled
#   - trigger_sf.json.gz available (from trigger SF pipeline)
#   - b-tag JSON accessible (CVMFS or local)
#
# Author: Junghyun Lee
# ============================================================================

set -euo pipefail

EXE="./exe_BTagSF"

# ── MC samples ──
MC_SAMPLES=(
    "TTTo2L2Nu"
    "TTToHadronic"
    "TTToSemiLeptonic"
)

# ── Data samples ──
DATA_SAMPLES=(
    "JetHT_B"
    "JetHT_C"
    "JetHT_D"
    "JetHT_E"
    "JetHT_F"
    "BTagCSV_B"
    "BTagCSV_C"
    "BTagCSV_D"
    "BTagCSV_E"
    "BTagCSV_F"
)

# 실패한 작업을 추적할 배열
FAILED=()

# ── 오류 검사 포함 실행 함수 ──
run_checked() {
    local sample=$1
    local mode=$2
    echo ""
    echo "────────────────────────────────────────────────────────"
    echo "  Running: $EXE $sample $mode"
    echo "────────────────────────────────────────────────────────"

    if ! $EXE "$sample" "$mode"; then
        echo "[ERROR] FAILED: $EXE $sample $mode"
        FAILED+=("$sample mode=$mode")
        return 1
    fi
    return 0
}


echo "╔══════════════════════════════════════════════════════════════╗"
echo "║  B-tag Shape SF Pipeline                                    ║"
echo "╠══════════════════════════════════════════════════════════════╣"
echo "║  Step 0: Normalization ratios (MC only)                     ║"
echo "║  Step 1: Apply reweight (MC + Data)                         ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""

# ============================================================================
# Step 0: Normalization (MC samples only)
#
# 각 MC 샘플에 대해 b-tag SF 적용 전/후 가중치 합(sum of weights)을
# nJets bin별로 계산하여 정규화 비율(ratio)을 산출한다.
#
# 출력: bTagNorm_<sample>.root  (h_nJets_normRatio 히스토그램 포함)
# ============================================================================
echo "═══════════════════════════════════════════════════════════"
echo "  Step 0: Computing normalization ratios"
echo "═══════════════════════════════════════════════════════════"

for sample in "${MC_SAMPLES[@]}"; do
    run_checked "$sample" 0 || true
done

echo ""
echo "  Step 0 complete."
echo ""

# ============================================================================
# Step 1: Reweight + Validation (MC + Data)
#
# MC: b-tag SF × normalization ratio × trigger SF를 적용
# Data: 모든 SF = 1.0 (검증용 비교 히스토그램만 생성)
#
# 출력: bTagReweight_<sample>.root (검증 히스토그램 포함)
# ============================================================================
echo "═══════════════════════════════════════════════════════════"
echo "  Step 1: Applying b-tag reweight"
echo "═══════════════════════════════════════════════════════════"

# MC
for sample in "${MC_SAMPLES[@]}"; do
    run_checked "$sample" 1 || true
done

# Data
for sample in "${DATA_SAMPLES[@]}"; do
    run_checked "$sample" 1 || true
done

# ============================================================================
# Summary
# ============================================================================
echo ""
echo "╔══════════════════════════════════════════════════════════════╗"
echo "║  Pipeline Summary                                           ║"
echo "╠══════════════════════════════════════════════════════════════╣"

if [ ${#FAILED[@]} -eq 0 ]; then
    echo "║  ✓ All jobs completed successfully.                        ║"
else
    echo "║  ✗ Some jobs failed:                                       ║"
    for f in "${FAILED[@]}"; do
        printf "║    - %-52s ║\n" "$f"
    done
fi

echo "╚══════════════════════════════════════════════════════════════╝"
echo ""

# hadd를 통한 Data 머지 (필요 시)
# hadd -f bTagReweight_JetHT.root   bTagReweight_JetHT_*.root
# hadd -f bTagReweight_BTagCSV.root bTagReweight_BTagCSV_*.root

exit ${#FAILED[@]}
