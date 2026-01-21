#!/bin/bash

# ==========================================
# Step 1: Run EventLooper (Mode 0: Calc Eff)
# ==========================================
# Usage: ./exe_TrigStudy [SampleName] [Mode]
# Mode 0 = Calculate Trigger Efficiency (Output 2D Hists)
# Mode 1 = Apply SF & Save Tree

# [옵션 설정] 
# 0 = false, 1 = true
USE_NBJET=0
USE_ETA=1

echo ">>> Processing MC Samples..."
# ./exe_TrigStudy [Sample] [Mode=0] [nbJet=0] [Eta=0]
# 0 0 0 --> Setting for derive Trigger SF before calculate b-jet SF
# 0 1 0 --> Setting for derive normal Trigger SF
./exe_TrigStudy TTTo2L2Nu 0 ${USE_NBJET} ${USE_ETA} &
./exe_TrigStudy TTToHadronic 0 ${USE_NBJET} ${USE_ETA} &
./exe_TrigStudy TTToSemiLeptonic 0 ${USE_NBJET} ${USE_ETA} &

echo ">>> Processing Data Samples..."
./exe_TrigStudy SingleMuon_B 0 ${USE_NBJET} ${USE_ETA} &
./exe_TrigStudy SingleMuon_C 0 ${USE_NBJET} ${USE_ETA} &
./exe_TrigStudy SingleMuon_D 0 ${USE_NBJET} ${USE_ETA} &
./exe_TrigStudy SingleMuon_E 0 ${USE_NBJET} ${USE_ETA} & 
./exe_TrigStudy SingleMuon_F 0 ${USE_NBJET} ${USE_ETA} &

wait
echo ">>> Event Loop Finished."

# ==========================================
# Step 2: Merge & Calculate SF
# ==========================================

outputName="260119_Tier3"
PlotterName="DeriveSF_Hist.cpp"

mkdir -p output/${outputName}
mv output_*.root output/${outputName}
cp ${PlotterName} output/${outputName}

cd "output/${outputName}" || exit

echo ">>> Merging Data..."
hadd -f Data.root output_SingleMuon_*.root

echo ">>> Merging MC..."
hadd -f output_ttJets.root output_TTToSemiLeptonic.root output_TTTo2L2Nu.root output_TTToHadronic.root
cp output_ttJets.root ttJets.root

# Prepare for Plotting
mkdir -p merger
mv Data.root merger/
mv ttJets.root merger/
mv ${PlotterName} merger/

cd merger || exit

echo ">>> Calculating Scale Factors..."

# 1. 파일 이름에서 확장자(.cpp)를 제거하여 함수 이름 추출
# 예: DeriveSF_Hist.cpp -> DeriveSF_Hist
FuncName=${PlotterName%.cpp}

# 2. 명확하게 분리해서 실행
# -e ".L 파일명" : 파일을 로드함
# -e "함수명(인자)" : 로드된 함수를 실행함
root -l -b -q -e ".L ${PlotterName}" -e "${FuncName}(${USE_NBJET}, ${USE_ETA})"


echo ">>> Done. Check 'ScaleFactors.root' in output/${outputName}/merger/"
