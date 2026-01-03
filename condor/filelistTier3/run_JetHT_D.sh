#!/bin/sh
echo
echo
echo 'START---------------'
echo 'WORKDIR ' ${PWD}
source "/cvmfs/cms.cern.ch/cmsset_default.sh"
cd "/u/user/jhlee/ttHH/CMSSW_14_2_1/src/tempTTHH"
cmsenv
echo 'WORKDIR ' ${PWD}
source "/u/user/jhlee/ttHH/CMSSW_14_2_1/src/tempTTHH/setup.sh"
mkdir -p /pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/AnalyzerOutput/JetHT_D/
"/u/user/jhlee/ttHH/CMSSW_14_2_1/src/tempTTHH/ttHHanalyzer_bTagSF" "$1" "/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/AnalyzerOutput/JetHT_D/$2" "$3" "$4" "$5" "$6" "$7"
