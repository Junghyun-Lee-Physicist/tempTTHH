#!/bin/bash

# JetHT datasets
dasgoclient --query="file dataset=/JetHT/Run2017B-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | sed 's|^|root://xrootd-cms.infn.it//|' >> filelist_JetHT_B.txt
dasgoclient --query="file dataset=/JetHT/Run2017C-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | sed 's|^|root://xrootd-cms.infn.it//|' >> filelist_JetHT_C.txt
dasgoclient --query="file dataset=/JetHT/Run2017D-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | sed 's|^|root://xrootd-cms.infn.it//|' >> filelist_JetHT_D.txt
dasgoclient --query="file dataset=/JetHT/Run2017E-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | sed 's|^|root://xrootd-cms.infn.it//|' >> filelist_JetHT_E.txt
dasgoclient --query="file dataset=/JetHT/Run2017F-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | sed 's|^|root://xrootd-cms.infn.it//|' >> filelist_JetHT_F.txt

# BTagCSV datasets
dasgoclient --query="file dataset=/BTagCSV/Run2017B-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | sed 's|^|root://xrootd-cms.infn.it//|' >> filelist_BTagCSV_B.txt
dasgoclient --query="file dataset=/BTagCSV/Run2017C-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | sed 's|^|root://xrootd-cms.infn.it//|' >> filelist_BTagCSV_C.txt
dasgoclient --query="file dataset=/BTagCSV/Run2017D-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | sed 's|^|root://xrootd-cms.infn.it//|' >> filelist_BTagCSV_D.txt
dasgoclient --query="file dataset=/BTagCSV/Run2017E-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | sed 's|^|root://xrootd-cms.infn.it//|' >> filelist_BTagCSV_E.txt
dasgoclient --query="file dataset=/BTagCSV/Run2017F-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | sed 's|^|root://xrootd-cms.infn.it//|' >> filelist_BTagCSV_F.txt

