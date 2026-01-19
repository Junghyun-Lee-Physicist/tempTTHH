#!/bin/bash

# ==========================================
# Preparation: Get Scale Factors from Step 1
# ==========================================
# Step 1의 output 경로를 확인하세요. (이전 스크립트 기준 output/260105_Tier3/merger/)
STEP1_DIR="output/260105_Tier3/merger"

if [ -f "${STEP1_DIR}/ScaleFactors.root" ]; then
    echo ">>> Found ScaleFactors.root in ${STEP1_DIR}. Copying..."
    cp "${STEP1_DIR}/ScaleFactors.root" .
else
    echo ">>> [ERROR] ScaleFactors.root not found in ${STEP1_DIR}!"
    echo ">>> Please run Step 1 (runEff.sh) first or check the path."
    exit 1
fi

# ==========================================
# Step 2: Run EventLooper (Mode 1: Apply SF)
# ==========================================
# Usage: ./exe_TrigStudy [SampleName] [Mode]
# Mode 1 = Apply SF & Save Tree (produces corrected_*.root)

echo ">>> Applying Scale Factors to MC..."
#./exe_TrigStudy ttJets 1 &
./exe_TrigStudy TTTo2L2Nu 1 &
./exe_TrigStudy ttTohadronic 1 &
./exe_TrigStudy TTToSemiLeptonic 1 &

echo ">>> Processing Data (Validation)..."
./exe_TrigStudy SingleMuon_B 1 &
./exe_TrigStudy SingleMuon_C 1 &
./exe_TrigStudy SingleMuon_D 1 & 
./exe_TrigStudy SingleMuon_E 1 &
./exe_TrigStudy SingleMuon_F 1 &

wait
echo ">>> Event Loop Finished."

# ==========================================
# Post-Processing: Merge & Plot
# ==========================================

outputName="260105_TriggerStep02_ApplySFs"
PlotterName="PlotTriggerEfficiency.cpp"

mkdir -p output/${outputName}
mv corrected_*.root output/${outputName}
cp ${PlotterName} output/${outputName}
cp ScaleFactors.root output/${outputName} # Save used SF file for reference

cd "output/${outputName}" || exit

echo ">>> Merging Data..."
hadd -f corrected_Data.root corrected_SingleMuon_*.root

echo ">>> Merging MC..."
hadd -f corrected_ttJets.root corrected_TTToSemiLeptonic.root corrected_TTTo2L2Nu.root corrected_ttTohadronic.root

# Prepare for Plotting
mkdir -p merger
mv corrected_Data.root merger/
mv corrected_ttJets.root merger/
mv ${PlotterName} merger/
cd merger || exit

echo ">>> Generating Verification Plots..."
root -l -b -q ${PlotterName}

echo ">>> Done. Check plots in output/${outputName}/merger/"
