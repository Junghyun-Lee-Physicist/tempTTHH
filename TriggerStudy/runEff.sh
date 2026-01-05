#!/bin/bash

#./exe_TrigStudy ttJets 0 &
./exe_TrigStudy TTTo2L2Nu 0 &
./exe_TrigStudy ttTohadronic 0 &
./exe_TrigStudy TTToSemiLeptonic 0 &

./exe_TrigStudy SingleMuon_B 0 &
./exe_TrigStudy SingleMuon_C 0 &
./exe_TrigStudy SingleMuon_D 0 &
./exe_TrigStudy SingleMuon_E 0 & 
./exe_TrigStudy SingleMuon_F 0 &

#./exe_TrigStudy JetHT_B
#./exe_TrigStudy JetHT_C
#./exe_TrigStudy JetHT_D
#./exe_TrigStudy JetHT_E
#./exe_TrigStudy JetHT_F
#./exe_TrigStudy BTagCSV_B
#./exe_TrigStudy BTagCSV_C
#./exe_TrigStudy BTagCSV_D
#./exe_TrigStudy BTagCSV_E
#./exe_TrigStudy BTagCSV_F

wait

outputName="260105_Tier3"
PlotterName="TriggerEfficiency.cpp"
mkdir -p output/${outputName}
mv output_*.root output/${outputName}
cp ${PlotterName} output/${outputName}
cd "output/${outputName}"

hadd Data.root output_SingleMuon_*.root
##hadd Data.root output_JetHT_*.root output_BTagCSV_*.root
hadd output_ttJets.root output_TTToSemiLeptonic.root output_TTTo2L2Nu.root output_ttTohadronic.root
cp output_ttJets.root ttJets.root
mkdir -p merger
mv Data.root "merger/"
mv ttJets.root "merger/"
mv ${PlotterName} "merger/"
cd merger
root -l ${PlotterName}
