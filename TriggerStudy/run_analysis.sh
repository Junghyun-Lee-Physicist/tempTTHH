#!/bin/bash

# ========================================================
# Full Trigger SF Analysis Pipeline (Config Driven)
# ========================================================

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

echo ">>> [Step 1] Merging Files..."
hadd -f Data.root output_SingleMuon_*.root # Merge B,C,D,E,F period output
hadd -f ttJets.root output_TTTo2L2Nu.root output_TTToHadronic.root output_TTToSemiLeptonic.root # Merge ttbar outputs

# 2. Derive SF Histograms
echo ">>> [Derive SF] Calculating Scale Factors..."
root -l -b -q "DeriveSF_Hist.cpp"

# 3. Create JSON
echo ">>> [JSON] Generating Correction JSON..."
python3 produceJSON.py
python3 validateJSON.py

# 4. Step 2: Apply SF (Mode 1)
echo ">>> [Step 2] Applying Corrections to MC..."
./exe_TrigStudy TTTo2L2Nu 1 &
./exe_TrigStudy TTToHadronic 1 &
./exe_TrigStudy TTToSemiLeptonic 1 &
wait

echo ">>> [Step 2] Merging Corrected MC..."
hadd -f corrected_ttJets.root corrected_TTTo*.root corrected_ttTo*.root
# Data는 SF 적용이 없지만 포맷 통일을 위해 복사 혹은 validation 용으로 돌림
cp Data.root corrected_Data.root 

# 5. Plotting
echo ">>> [Plotting] Generating Validation Plots..."
root -l -b -q "PlotTriggerEfficiency.cpp"

echo ">>> All Analysis Steps Completed!"
