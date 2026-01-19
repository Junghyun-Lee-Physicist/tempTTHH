#!/bin/bash

# ==========================================
# Step 1: Run EventLooper (Mode 0: Calc Eff)
# ==========================================
# Usage: ./exe_TrigStudy [SampleName] [Mode]
# Mode 0 = Calculate Trigger Efficiency (Output 2D Hists)
# Mode 1 = Apply SF & Save Tree

echo ">>> Processing MC Samples..."
./exe_TrigStudy ttJets 0 &       # (Optionally include if you have the inclusive sample)
./exe_TrigStudy TTTo2L2Nu 0 &
./exe_TrigStudy ttTohadronic 0 &
./exe_TrigStudy TTToSemiLeptonic 0 &

echo ">>> Processing Data Samples..."
./exe_TrigStudy SingleMuon_B 0 &
./exe_TrigStudy SingleMuon_C 0 &
./exe_TrigStudy SingleMuon_D 0 &
./exe_TrigStudy SingleMuon_E 0 & 
./exe_TrigStudy SingleMuon_F 0 &

wait
echo ">>> Event Loop Finished."

# ==========================================
# Step 2: Merge & Calculate SF
# ==========================================

outputName="260105_Tier3"
PlotterName="TriggerEfficiency.cpp"

mkdir -p output/${outputName}
mv output_*.root output/${outputName}
cp ${PlotterName} output/${outputName}

cd "output/${outputName}" || exit

echo ">>> Merging Data..."
hadd -f Data.root output_SingleMuon_*.root

echo ">>> Merging MC..."
# ttJets가 inclusive 샘플이 없다면 3개 채널 합치기
hadd -f output_ttJets.root output_TTToSemiLeptonic.root output_TTTo2L2Nu.root output_ttTohadronic.root
cp output_ttJets.root ttJets.root

# Prepare for Plotting
mkdir -p merger
mv Data.root merger/
mv ttJets.root merger/
mv ${PlotterName} merger/

cd merger || exit

echo ">>> Calculating Scale Factors..."
root -l -b -q ${PlotterName}

echo ">>> Done. Check 'ScaleFactors.root' in output/${outputName}/merger/"
