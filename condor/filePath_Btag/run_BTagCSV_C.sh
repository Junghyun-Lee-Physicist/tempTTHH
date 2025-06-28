#!/bin/sh
echo
echo
echo 'START---------------'
echo 'WORKDIR ' ${PWD}
source "/cvmfs/cms.cern.ch/cmsset_default.sh"
cd "/afs/cern.ch/user/j/junghyun/ttHH_analysis/CMSSW_14_2_1/src/runii_tthhanalyzerV8"
cmsenv
echo 'WORKDIR ' ${PWD}
source "/afs/cern.ch/user/j/junghyun/ttHH_analysis/CMSSW_14_2_1/src/runii_tthhanalyzerV8/setup.sh"
eos root://eosuser.cern.ch mkdir -p /eos/user/j/junghyun/ttHH/Btag_el9/BTagCSV_C/
"/afs/cern.ch/user/j/junghyun/ttHH_analysis/CMSSW_14_2_1/src/runii_tthhanalyzerV8/ttHHanalyzer_bTagSF" "$1" "root://eosuser.cern.ch//eos/user/j/junghyun/ttHH/Btag_el9/BTagCSV_C/$2" "$3" "$4" "$5" "$6" "$7"
