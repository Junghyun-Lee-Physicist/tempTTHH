#!/bin/bash

MC=(
  "QCD_HT200to300"
  "QCD_HT300to500"
  "QCD_HT500to700"
  "QCD_HT700to1000"
  "QCD_HT1000to1500"
  "QCD_HT1500to2000"
  "QCD_HT2000toInf"

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
