#!/bin/bash

MC=(
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
#  "ttJets"
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

DATA=(
 #"Data"
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

Dir="ScaleFactors/Final"
mkdir -p "${Dir}"
mkdir -p "${Dir}/MC" "${Dir}/Data"

for mc in "${MC[@]}"; do

    mv "Reweight_${mc}.root" "${Dir}/MC/Reweight_${mc}.root"
    
done

for data in "${DATA[@]}"; do

    mv "Reweight_${data}.root" "${Dir}/Data/Reweight_${data}.root"

done

cd ${Dir}/MC
rm -rf Reweight_ttJets.root
hadd Reweight_ttJets.root Reweight_ttTohadronic.root Reweight_TTTo2L2Nu.root Reweight_TTToSemiLeptonic.root

cd -
cd ${Dir}/Data
rm -rf Reweight_Data.root
hadd Reweight_Data.root *.root
