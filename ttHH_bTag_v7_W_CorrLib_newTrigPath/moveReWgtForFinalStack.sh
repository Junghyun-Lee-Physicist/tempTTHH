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
  "TTToHadronic"
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

    mv "Validation_${mc}.root" "${Dir}/MC/Validation_${mc}.root"
    
done

for data in "${DATA[@]}"; do

    mv "Validation_${data}.root" "${Dir}/Data/Validation_${data}.root"

done

cd ${Dir}/MC
rm -rf Validation_ttJets.root
hadd Validation_ttJets.root Validation_TTToHadronic.root Validation_TTTo2L2Nu.root Validation_TTToSemiLeptonic.root

cd -
cd ${Dir}/Data
rm -rf Validation_Data.root
hadd Validation_Data.root *.root
