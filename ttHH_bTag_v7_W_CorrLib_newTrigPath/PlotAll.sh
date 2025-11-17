#!/bin/bash

###############################################
# File: runPlotAll.sh
# Usage: ./runPlotAll.sh
# Purpose:
#   - plotBTagStudy.C 매크로를
#     여러 샘플에 대해 순차적으로 실행
###############################################

##### MC 샘플 목록
SAMPLES=(
##  "QCD"
  "QCD_Pt_15to30"
  "QCD_Pt_30to50"
  "QCD_Pt_50to80"
  "QCD_Pt_80to120"
  "QCD_Pt_120to170"
  "QCD_Pt_170to300"
  "QCD_Pt_300to470"
  "QCD_Pt_470to600"
  "QCD_Pt_600to800"
  "QCD_Pt_800to1000"
  "QCD_Pt_1000to1400"
  "QCD_Pt_1400to1800"
  "QCD_Pt_1800to2400"
  "QCD_Pt_2400to3200"
  "QCD_Pt_3200toInf"

  "tt4b"
  "ttbb"
  "ttHH"
  "ttHtobb"
##  "ttJets"
  "ttTohadronic"
  "TTTo2L2Nu"
  "TTToSemiLeptonic"
  "tttt"
  "tttW"
  "ttWH"
  "ttWW"
  "ttWZ"
  "ttZHto4b"
  "ttZtobb"
  "ttZZto4b"
)

####SAMPLES=(
####  "tt4b"
####  "ttbb"
####  "ttHH"
####  "ttHtobb"
####  "ttJets"
####  "tttt"
####  "tttW"
####  "ttWH"
####  "ttWW"
####  "ttWZ"
####  "ttZHto4b"
####  "ttZtobb"
####  "ttZZto4b"
####)

# 루프를 돌면서 plotBTagStudy 매크로 실행
for sample in "${SAMPLES[@]}"; do
    echo " >>> Running plotting for sample: $sample"
    ##root -l -b -q "produceAllRatioPlots.C(\"$sample\")"
    root -l -b -q "makeBTagReweightPlot.C(\"$sample\")"
    ####root -l -b -q "makeBTagReweightPlot_nor.C(\"$sample\")"

done

echo "All done!"
