#!/bin/bash

# ========================================================
# Full Trigger SF Analysis Pipeline (Config Driven)
# ========================================================

outputDirName="outputs"

# 1. Step 1: Calculate Efficiency (Mode 0)
echo ">>> [Step 1] Running EventLooper..."
./exe_TrigStudy TTTo2L2Nu 0 &
./exe_TrigStudy TTToHadronic 0 &
./exe_TrigStudy TTToSemiLeptonic 0 &
./exe_TrigStudy SingleMuon_B 0 &
./exe_TrigStudy SingleMuon_C 0 &
./exe_TrigStudy SingleMuon_D 0 &
./exe_TrigStudy SingleMuon_E 0 &
./exe_TrigStudy SingleMuon_F 0 &
wait

mkdir -p outputs

echo ">>> [Step 1] Merging Files..."
hadd -f output_SingleMuon.root output_SingleMuon_*.root # Merge B,C,D,E,F period output
hadd -f output_TTbarInc.root output_TTTo2L2Nu.root output_TTToHadronic.root output_TTToSemiLeptonic.root # Merge ttbar outputs

# 2. Derive SF Histograms
echo ">>> [Derive SF] Calculating Scale Factors..."
root -l -b -q "DeriveSF.cpp"

# 4. Step 2: Apply SF (Mode 1)
echo ">>> [Step 2] Applying Corrections to MC..."
./exe_TrigStudy TTTo2L2Nu 1 &
./exe_TrigStudy TTToHadronic 1 &
./exe_TrigStudy TTToSemiLeptonic 1 &
./exe_TrigStudy SingleMuon_B 1 &
./exe_TrigStudy SingleMuon_C 1 &
./exe_TrigStudy SingleMuon_D 1 &
./exe_TrigStudy SingleMuon_E 1 &
./exe_TrigStudy SingleMuon_F 1 &
wait

echo ">>> [Step 2] Merging Corrected MC..."
hadd -f validated_SingleMuon.root validated_SingleMuon_*.root # Merge B,C,D,E,F period output
hadd -f validated_TTbarInc.root validated_TTTo*.root # Merge ttbar outputs

# 5. Plotting
echo ">>> [Plotting] Generating Validation Plots..."
root -l -b -q "PlotTriggerEfficiency.cpp"

echo ">>> All Analysis Steps Completed!"
