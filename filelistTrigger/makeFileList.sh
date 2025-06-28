#!/bin/bash

# SingleMuon datasets
dasgoclient --query="file dataset=/SingleMuon/Run2017B-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | sed 's|^|root://xrootd-cms.infn.it//|' >> filelist_SingleMuon_B.txt
dasgoclient --query="file dataset=/SingleMuon/Run2017C-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | sed 's|^|root://xrootd-cms.infn.it//|' >> filelist_SingleMuon_C.txt
dasgoclient --query="file dataset=/SingleMuon/Run2017D-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | sed 's|^|root://xrootd-cms.infn.it//|' >> filelist_SingleMuon_D.txt
dasgoclient --query="file dataset=/SingleMuon/Run2017E-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | sed 's|^|root://xrootd-cms.infn.it//|' >> filelist_SingleMuon_E.txt
dasgoclient --query="file dataset=/SingleMuon/Run2017F-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | sed 's|^|root://xrootd-cms.infn.it//|' >> filelist_SingleMuon_F.txt

