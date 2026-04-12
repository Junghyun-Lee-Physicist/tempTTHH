#ifndef EVENTBUFFER_H
#define EVENTBUFFER_H
//----------------------------------------------------------------------------
// File:        eventBuffer.h
// Description: Analyzer header for ntuples created by TheNtupleMaker
// Created:     Sat Apr 11 23:47:58 2026 by mkanalyzer.py v2.0.3 14-Oct-2020
// Author:      JungHyun Lee
//----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <map>
#include <cassert>
#include "treestream.h"

struct eventBuffer
{
  //--------------------------------------------------------------------------
  // --- Declare variables
  //--------------------------------------------------------------------------
  std::vector<int>	Electron_charge;
  std::vector<int>	Electron_cutBased;
  std::vector<float>	Electron_deltaEtaSC;
  std::vector<float>	Electron_dxy;
  std::vector<float>	Electron_dz;
  std::vector<float>	Electron_eta;
  std::vector<int>	Electron_genPartIdx;
  std::vector<float>	Electron_mass;
  std::vector<float>	Electron_miniPFRelIso_all;
  std::vector<bool>	Electron_mvaFall17V2Iso_WP80;
  std::vector<bool>	Electron_mvaFall17V2Iso_WP90;
  std::vector<float>	Electron_mvaTTH;
  std::vector<float>	Electron_pfRelIso03_all;
  std::vector<float>	Electron_phi;
  std::vector<float>	Electron_pt;
  std::vector<float>	FatJet_area;
  std::vector<float>	FatJet_deepTagMD_H4qvsQCD;
  std::vector<float>	FatJet_deepTagMD_HbbvsQCD;
  std::vector<float>	FatJet_deepTagMD_TvsQCD;
  std::vector<float>	FatJet_deepTagMD_WvsQCD;
  std::vector<float>	FatJet_deepTagMD_ZHbbvsQCD;
  std::vector<float>	FatJet_deepTagMD_ZHccvsQCD;
  std::vector<float>	FatJet_deepTagMD_ZbbvsQCD;
  std::vector<float>	FatJet_deepTagMD_ZvsQCD;
  std::vector<float>	FatJet_deepTagMD_bbvsLight;
  std::vector<float>	FatJet_deepTagMD_ccvsLight;
  std::vector<float>	FatJet_deepTag_H;
  std::vector<float>	FatJet_deepTag_QCD;
  std::vector<float>	FatJet_deepTag_QCDothers;
  std::vector<float>	FatJet_deepTag_TvsQCD;
  std::vector<float>	FatJet_deepTag_WvsQCD;
  std::vector<float>	FatJet_deepTag_ZvsQCD;
  std::vector<float>	FatJet_eta;
  std::vector<int>	FatJet_jetId;
  std::vector<float>	FatJet_mass;
  std::vector<float>	FatJet_msoftdrop;
  std::vector<float>	FatJet_n2b1;
  std::vector<float>	FatJet_n3b1;
  std::vector<float>	FatJet_particleNetMD_QCD;
  std::vector<float>	FatJet_particleNetMD_Xbb;
  std::vector<float>	FatJet_particleNetMD_Xcc;
  std::vector<float>	FatJet_particleNetMD_Xqq;
  std::vector<float>	FatJet_particleNet_HbbvsQCD;
  std::vector<float>	FatJet_particleNet_TvsQCD;
  std::vector<float>	FatJet_particleNet_WvsQCD;
  std::vector<float>	FatJet_particleNet_ZvsQCD;
  std::vector<float>	FatJet_particleNet_mass;
  std::vector<float>	FatJet_phi;
  std::vector<float>	FatJet_pt;
  std::vector<float>	FatJet_rawFactor;
  std::vector<float>	FatJet_tau1;
  std::vector<float>	FatJet_tau2;
  std::vector<float>	FatJet_tau3;
  std::vector<float>	FatJet_tau4;
  std::vector<float>	GenJet_eta;
  std::vector<int>	GenJet_hadronFlavour;
  std::vector<float>	GenJet_mass;
  std::vector<int>	GenJet_partonFlavour;
  std::vector<float>	GenJet_phi;
  std::vector<float>	GenJet_pt;
  std::vector<float>	GenPart_eta;
  std::vector<int>	GenPart_genPartIdxMother;
  std::vector<float>	GenPart_mass;
  std::vector<int>	GenPart_pdgId;
  std::vector<float>	GenPart_phi;
  std::vector<float>	GenPart_pt;
  std::vector<int>	GenPart_status;
  std::vector<int>	GenPart_statusFlags;
  std::vector<float>	Jet_area;
  std::vector<float>	Jet_bRegCorr;
  std::vector<float>	Jet_bRegRes;
  std::vector<float>	Jet_btagDeepFlavB;
  std::vector<float>	Jet_btagDeepFlavCvB;
  std::vector<float>	Jet_btagDeepFlavCvL;
  std::vector<float>	Jet_btagDeepFlavQG;
  std::vector<float>	Jet_eta;
  std::vector<int>	Jet_genJetIdx;
  std::vector<int>	Jet_hadronFlavour;
  std::vector<int>	Jet_jetId;
  std::vector<float>	Jet_mass;
  std::vector<int>	Jet_partonFlavour;
  std::vector<float>	Jet_phi;
  std::vector<float>	Jet_pt;
  std::vector<int>	Jet_puId;
  std::vector<float>	Jet_qgl;
  std::vector<float>	Jet_rawFactor;
  std::vector<float>	LHEPdfWeight;
  std::vector<float>	LHEScaleWeight;
  std::vector<int>	Muon_charge;
  std::vector<float>	Muon_dxy;
  std::vector<float>	Muon_dz;
  std::vector<float>	Muon_eta;
  std::vector<int>	Muon_genPartIdx;
  std::vector<bool>	Muon_looseId;
  std::vector<float>	Muon_mass;
  std::vector<bool>	Muon_mediumId;
  std::vector<float>	Muon_miniPFRelIso_all;
  std::vector<float>	Muon_mvaTTH;
  std::vector<float>	Muon_pfRelIso04_all;
  std::vector<float>	Muon_phi;
  std::vector<float>	Muon_pt;
  std::vector<bool>	Muon_tightId;

  int	nElectron;
  int	nMuon;
  int	nLHEPdfWeight;
  int	nJet;
  int	nFatJet;
  int	nGenJet;
  int	nGenPart;
  int	nLHEScaleWeight;

  bool	Flag_BadPFMuonDzFilter;
  bool	Flag_BadPFMuonFilter;
  bool	Flag_EcalDeadCellTriggerPrimitiveFilter;
  bool	Flag_HBHENoiseFilter;
  bool	Flag_HBHENoiseIsoFilter;
  bool	Flag_ecalBadCalibFilter;
  bool	Flag_eeBadScFilter;
  bool	Flag_globalSuperTightHalo2016Filter;
  bool	Flag_goodVertices;
  float	Generator_weight;
  bool	HLT_AK8PFHT750_TrimMass50;
  bool	HLT_AK8PFHT800_TrimMass50;
  bool	HLT_AK8PFHT850_TrimMass50;
  bool	HLT_AK8PFHT900_TrimMass50;
  bool	HLT_AK8PFJet140;
  bool	HLT_AK8PFJet200;
  bool	HLT_AK8PFJet260;
  bool	HLT_AK8PFJet320;
  bool	HLT_AK8PFJet330_PFAK8BTagCSV_p1;
  bool	HLT_AK8PFJet330_PFAK8BTagCSV_p17;
  bool	HLT_AK8PFJet360_TrimMass30;
  bool	HLT_AK8PFJet380_TrimMass30;
  bool	HLT_AK8PFJet40;
  bool	HLT_AK8PFJet400;
  bool	HLT_AK8PFJet400_TrimMass30;
  bool	HLT_AK8PFJet420_TrimMass30;
  bool	HLT_AK8PFJet450;
  bool	HLT_AK8PFJet500;
  bool	HLT_AK8PFJet550;
  bool	HLT_AK8PFJet60;
  bool	HLT_AK8PFJet80;
  bool	HLT_AK8PFJetFwd140;
  bool	HLT_AK8PFJetFwd200;
  bool	HLT_AK8PFJetFwd260;
  bool	HLT_AK8PFJetFwd320;
  bool	HLT_AK8PFJetFwd40;
  bool	HLT_AK8PFJetFwd400;
  bool	HLT_AK8PFJetFwd450;
  bool	HLT_AK8PFJetFwd500;
  bool	HLT_AK8PFJetFwd60;
  bool	HLT_AK8PFJetFwd80;
  bool	HLT_DiPFJet15_FBEta3_NoCaloMatched;
  bool	HLT_DiPFJet15_NoCaloMatched;
  bool	HLT_DiPFJet25_FBEta3_NoCaloMatched;
  bool	HLT_DiPFJet25_NoCaloMatched;
  bool	HLT_DiPFJetAve100_HFJEC;
  bool	HLT_DiPFJetAve140;
  bool	HLT_DiPFJetAve15_HFJEC;
  bool	HLT_DiPFJetAve160_HFJEC;
  bool	HLT_DiPFJetAve200;
  bool	HLT_DiPFJetAve220_HFJEC;
  bool	HLT_DiPFJetAve25_HFJEC;
  bool	HLT_DiPFJetAve260;
  bool	HLT_DiPFJetAve300_HFJEC;
  bool	HLT_DiPFJetAve320;
  bool	HLT_DiPFJetAve35_HFJEC;
  bool	HLT_DiPFJetAve40;
  bool	HLT_DiPFJetAve400;
  bool	HLT_DiPFJetAve500;
  bool	HLT_DiPFJetAve60;
  bool	HLT_DiPFJetAve60_HFJEC;
  bool	HLT_DiPFJetAve80;
  bool	HLT_DiPFJetAve80_HFJEC;
  bool	HLT_Ele115_CaloIdVT_GsfTrkIdT;
  bool	HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30;
  bool	HLT_Ele135_CaloIdVT_GsfTrkIdT;
  bool	HLT_Ele145_CaloIdVT_GsfTrkIdT;
  bool	HLT_Ele15_IsoVVVL_PFHT450;
  bool	HLT_Ele15_IsoVVVL_PFHT450_CaloBTagCSV_4p5;
  bool	HLT_Ele15_IsoVVVL_PFHT450_PFMET50;
  bool	HLT_Ele15_IsoVVVL_PFHT600;
  bool	HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL;
  bool	HLT_Ele17_CaloIdM_TrackIdM_PFJet30;
  bool	HLT_Ele200_CaloIdVT_GsfTrkIdT;
  bool	HLT_Ele20_WPLoose_Gsf;
  bool	HLT_Ele20_WPTight_Gsf;
  bool	HLT_Ele20_eta2p1_WPLoose_Gsf;
  bool	HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30;
  bool	HLT_Ele23_CaloIdM_TrackIdM_PFJet30;
  bool	HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL;
  bool	HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ;
  bool	HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_CrossL1;
  bool	HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_TightID_CrossL1;
  bool	HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_CrossL1;
  bool	HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_TightID_CrossL1;
  bool	HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_CrossL1;
  bool	HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_TightID_CrossL1;
  bool	HLT_Ele250_CaloIdVT_GsfTrkIdT;
  bool	HLT_Ele27_Ele37_CaloIdL_MW;
  bool	HLT_Ele27_WPTight_Gsf;
  bool	HLT_Ele28_HighEta_SC20_Mass55;
  bool	HLT_Ele28_eta2p1_WPTight_Gsf_HT150;
  bool	HLT_Ele300_CaloIdVT_GsfTrkIdT;
  bool	HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned;
  bool	HLT_Ele32_WPTight_Gsf;
  bool	HLT_Ele32_WPTight_Gsf_L1DoubleEG;
  bool	HLT_Ele35_WPTight_Gsf;
  bool	HLT_Ele35_WPTight_Gsf_L1EGMT;
  bool	HLT_Ele38_WPTight_Gsf;
  bool	HLT_Ele40_WPTight_Gsf;
  bool	HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165;
  bool	HLT_Ele50_IsoVVVL_PFHT450;
  bool	HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30;
  bool	HLT_Ele8_CaloIdM_TrackIdM_PFJet30;
  bool	HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07;
  bool	HLT_IsoMu20;
  bool	HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_CrossL1;
  bool	HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_TightID_CrossL1;
  bool	HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_CrossL1;
  bool	HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_TightID_CrossL1;
  bool	HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_CrossL1;
  bool	HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_TightID_CrossL1;
  bool	HLT_IsoMu24;
  bool	HLT_IsoMu24_eta2p1;
  bool	HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_SingleL1;
  bool	HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_TightID_SingleL1;
  bool	HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1;
  bool	HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1;
  bool	HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_SingleL1;
  bool	HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_TightID_SingleL1;
  bool	HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1;
  bool	HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1;
  bool	HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1;
  bool	HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1;
  bool	HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau50_Trk30_eta2p1_1pr;
  bool	HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_SingleL1;
  bool	HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_TightID_SingleL1;
  bool	HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1;
  bool	HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1;
  bool	HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1;
  bool	HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1;
  bool	HLT_IsoMu27;
  bool	HLT_IsoMu27_LooseChargedIsoPFTau20_SingleL1;
  bool	HLT_IsoMu27_MediumChargedIsoPFTau20_SingleL1;
  bool	HLT_IsoMu27_TightChargedIsoPFTau20_SingleL1;
  bool	HLT_IsoMu30;
  bool	HLT_Mu10_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT350_PFMETNoMu60;
  bool	HLT_Mu12_DoublePFJets100_CaloBTagCSV_p33;
  bool	HLT_Mu12_DoublePFJets200_CaloBTagCSV_p33;
  bool	HLT_Mu12_DoublePFJets350_CaloBTagCSV_p33;
  bool	HLT_Mu12_DoublePFJets40MaxDeta1p6_DoubleCaloBTagCSV_p33;
  bool	HLT_Mu12_DoublePFJets40_CaloBTagCSV_p33;
  bool	HLT_Mu12_DoublePFJets54MaxDeta1p6_DoubleCaloBTagCSV_p33;
  bool	HLT_Mu12_DoublePFJets62MaxDeta1p6_DoubleCaloBTagCSV_p33;
  bool	HLT_Mu12_DoublePhoton20;
  bool	HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL;
  bool	HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ;
  bool	HLT_Mu15_IsoVVVL_PFHT450;
  bool	HLT_Mu15_IsoVVVL_PFHT450_CaloBTagCSV_4p5;
  bool	HLT_Mu15_IsoVVVL_PFHT450_PFMET50;
  bool	HLT_Mu15_IsoVVVL_PFHT600;
  bool	HLT_Mu17;
  bool	HLT_Mu17_Photon30_IsoCaloId;
  bool	HLT_Mu17_TrkIsoVVL;
  bool	HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL;
  bool	HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ;
  bool	HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8;
  bool	HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8;
  bool	HLT_Mu18_Mu9;
  bool	HLT_Mu18_Mu9_DZ;
  bool	HLT_Mu18_Mu9_SameSign;
  bool	HLT_Mu18_Mu9_SameSign_DZ;
  bool	HLT_Mu19;
  bool	HLT_Mu19_TrkIsoVVL;
  bool	HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL;
  bool	HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ;
  bool	HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass3p8;
  bool	HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass8;
  bool	HLT_Mu20;
  bool	HLT_Mu20_Mu10;
  bool	HLT_Mu20_Mu10_DZ;
  bool	HLT_Mu20_Mu10_SameSign;
  bool	HLT_Mu20_Mu10_SameSign_DZ;
  bool	HLT_Mu20_TkMu0_Phi;
  bool	HLT_Mu23_Mu12;
  bool	HLT_Mu23_Mu12_DZ;
  bool	HLT_Mu23_Mu12_SameSign;
  bool	HLT_Mu23_Mu12_SameSign_DZ;
  bool	HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL;
  bool	HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ;
  bool	HLT_Mu25_TkMu0_Onia;
  bool	HLT_Mu25_TkMu0_Phi;
  bool	HLT_Mu27;
  bool	HLT_Mu27_Ele37_CaloIdL_MW;
  bool	HLT_Mu30_TkMu0_Onia;
  bool	HLT_Mu37_Ele27_CaloIdL_MW;
  bool	HLT_Mu37_TkMu27;
  bool	HLT_Mu3_PFJet40;
  bool	HLT_Mu43NoFiltersNoVtx_Photon43_CaloIdL;
  bool	HLT_Mu48NoFiltersNoVtx_Photon48_CaloIdL;
  bool	HLT_Mu50;
  bool	HLT_Mu50_IsoVVVL_PFHT450;
  bool	HLT_Mu55;
  bool	HLT_Mu7p5_L2Mu2_Jpsi;
  bool	HLT_Mu7p5_L2Mu2_Upsilon;
  bool	HLT_Mu7p5_Track2_Jpsi;
  bool	HLT_Mu7p5_Track2_Upsilon;
  bool	HLT_Mu7p5_Track3p5_Jpsi;
  bool	HLT_Mu7p5_Track3p5_Upsilon;
  bool	HLT_Mu7p5_Track7_Jpsi;
  bool	HLT_Mu7p5_Track7_Upsilon;
  bool	HLT_Mu8;
  bool	HLT_Mu8_DiEle12_CaloIdL_TrackIdL;
  bool	HLT_Mu8_DiEle12_CaloIdL_TrackIdL_DZ;
  bool	HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350;
  bool	HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ;
  bool	HLT_Mu8_TrkIsoVVL;
  bool	HLT_Mu8_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT300_PFMETNoMu60;
  bool	HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL;
  bool	HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ;
  bool	HLT_PFHT1050;
  bool	HLT_PFHT180;
  bool	HLT_PFHT250;
  bool	HLT_PFHT300PT30_QuadPFJet_75_60_45_40;
  bool	HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0;
  bool	HLT_PFHT350;
  bool	HLT_PFHT350MinPFJet15;
  bool	HLT_PFHT370;
  bool	HLT_PFHT380_SixJet32_DoubleBTagCSV_p075;
  bool	HLT_PFHT380_SixPFJet32;
  bool	HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2;
  bool	HLT_PFHT380_SixPFJet32_DoublePFBTagDeepCSV_2p2;
  bool	HLT_PFHT430;
  bool	HLT_PFHT430_SixJet40_BTagCSV_p080;
  bool	HLT_PFHT430_SixPFJet40;
  bool	HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5;
  bool	HLT_PFHT500_PFMET100_PFMHT100_IDTight;
  bool	HLT_PFHT500_PFMET110_PFMHT110_IDTight;
  bool	HLT_PFHT510;
  bool	HLT_PFHT590;
  bool	HLT_PFHT680;
  bool	HLT_PFHT700_PFMET85_PFMHT85_IDTight;
  bool	HLT_PFHT700_PFMET95_PFMHT95_IDTight;
  bool	HLT_PFHT780;
  bool	HLT_PFHT800_PFMET75_PFMHT75_IDTight;
  bool	HLT_PFHT800_PFMET85_PFMHT85_IDTight;
  bool	HLT_PFHT890;
  bool	HLT_QuadPFJet103_88_75_15;
  bool	HLT_QuadPFJet103_88_75_15_BTagCSV_p013_VBF2;
  bool	HLT_QuadPFJet103_88_75_15_DoubleBTagCSV_p013_p08_VBF1;
  bool	HLT_QuadPFJet105_88_76_15;
  bool	HLT_QuadPFJet105_88_76_15_BTagCSV_p013_VBF2;
  bool	HLT_QuadPFJet105_90_76_15_DoubleBTagCSV_p013_p08_VBF1;
  bool	HLT_QuadPFJet111_90_80_15;
  bool	HLT_QuadPFJet111_90_80_15_BTagCSV_p013_VBF2;
  bool	HLT_QuadPFJet111_90_80_15_DoubleBTagCSV_p013_p08_VBF1;
  bool	HLT_QuadPFJet98_83_71_15;
  bool	HLT_QuadPFJet98_83_71_15_BTagCSV_p013_VBF2;
  bool	HLT_QuadPFJet98_83_71_15_DoubleBTagCSV_p013_p08_VBF1;
  float	L1PreFiringWeight_Dn;
  float	L1PreFiringWeight_Nom;
  float	L1PreFiringWeight_Up;
  float	MET_covXX;
  float	MET_covXY;
  float	MET_covYY;
  float	MET_phi;
  float	MET_pt;
  float	MET_significance;
  float	PV_chi2;
  float	PV_ndof;
  int	PV_npvs;
  int	PV_npvsGood;
  float	PV_z;
  int	Pileup_nPU;
  float	Pileup_nTrueInt;
  float	PuppiMET_phi;
  float	PuppiMET_phiJERDown;
  float	PuppiMET_phiJERUp;
  float	PuppiMET_phiJESDown;
  float	PuppiMET_phiJESUp;
  float	PuppiMET_phiUnclusteredDown;
  float	PuppiMET_phiUnclusteredUp;
  float	PuppiMET_pt;
  float	PuppiMET_ptJERDown;
  float	PuppiMET_ptJERUp;
  float	PuppiMET_ptJESDown;
  float	PuppiMET_ptJESUp;
  float	PuppiMET_ptUnclusteredDown;
  float	PuppiMET_ptUnclusteredUp;
  float	PuppiMET_sumEt;
  long	event;
  float	fixedGridRhoFastjetAll;
  int	genTtbarId;
  float	genWeight;
  unsigned int	luminosityBlock;
  unsigned int	run;
  int	ttCatSource;
  int	ttCatXvalSource;
  bool	ttCatXval_Add1Bjet_1Had;
  bool	ttCatXval_Add1Bjet_2Had;
  bool	ttCatXval_Add2Bjet;
  bool	ttCatXval_AddCjet;
  bool	ttCatXval_LightFlavour;
  bool	ttCat_Add1Bjet_1Had;
  bool	ttCat_Add1Bjet_2Had;
  bool	ttCat_Add2Bjet;
  bool	ttCat_AddCjet;
  bool	ttCat_LightFlavour;

  //--------------------------------------------------------------------------
  // --- Structs can be filled by calling fill(), or individual fill
  // --- methods, e.g., fillElectrons()
  // --- after the call to read(...)
  //----------- --------------------------------------------------------------
  struct Electron_s
  {
    int	charge;
    int	cutBased;
    float	deltaEtaSC;
    float	dxy;
    float	dz;
    float	eta;
    float	mass;
    float	miniPFRelIso_all;
    bool	mvaFall17V2Iso_WP80;
    bool	mvaFall17V2Iso_WP90;
    float	mvaTTH;
    float	pfRelIso03_all;
    float	phi;
    float	pt;
    int	genPartIdx;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "Electron" << std::endl;
      sprintf(r, "  %-32s: %f\n", "charge", ( double)charge); os << r;
      sprintf(r, "  %-32s: %f\n", "cutBased", ( double)cutBased); os << r;
      sprintf(r, "  %-32s: %f\n", "deltaEtaSC", ( double)deltaEtaSC); os << r;
      sprintf(r, "  %-32s: %f\n", "dxy", ( double)dxy); os << r;
      sprintf(r, "  %-32s: %f\n", "dz", ( double)dz); os << r;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "miniPFRelIso_all", ( double)miniPFRelIso_all); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaFall17V2Iso_WP80", ( double)mvaFall17V2Iso_WP80); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaFall17V2Iso_WP90", ( double)mvaFall17V2Iso_WP90); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaTTH", ( double)mvaTTH); os << r;
      sprintf(r, "  %-32s: %f\n", "pfRelIso03_all", ( double)pfRelIso03_all); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      sprintf(r, "  %-32s: %f\n", "genPartIdx", ( double)genPartIdx); os << r;
      return os;
    }
  };

  struct FatJet_s
  {
    float	area;
    float	deepTagMD_H4qvsQCD;
    float	deepTagMD_HbbvsQCD;
    float	deepTagMD_TvsQCD;
    float	deepTagMD_WvsQCD;
    float	deepTagMD_ZHbbvsQCD;
    float	deepTagMD_ZHccvsQCD;
    float	deepTagMD_ZbbvsQCD;
    float	deepTagMD_ZvsQCD;
    float	deepTagMD_bbvsLight;
    float	deepTagMD_ccvsLight;
    float	deepTag_H;
    float	deepTag_QCD;
    float	deepTag_QCDothers;
    float	deepTag_TvsQCD;
    float	deepTag_WvsQCD;
    float	deepTag_ZvsQCD;
    float	eta;
    int	jetId;
    float	mass;
    float	msoftdrop;
    float	n2b1;
    float	n3b1;
    float	particleNetMD_QCD;
    float	particleNetMD_Xbb;
    float	particleNetMD_Xcc;
    float	particleNetMD_Xqq;
    float	particleNet_HbbvsQCD;
    float	particleNet_TvsQCD;
    float	particleNet_WvsQCD;
    float	particleNet_ZvsQCD;
    float	particleNet_mass;
    float	phi;
    float	pt;
    float	rawFactor;
    float	tau1;
    float	tau2;
    float	tau3;
    float	tau4;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "FatJet" << std::endl;
      sprintf(r, "  %-32s: %f\n", "area", ( double)area); os << r;
      sprintf(r, "  %-32s: %f\n", "deepTagMD_H4qvsQCD", ( double)deepTagMD_H4qvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "deepTagMD_HbbvsQCD", ( double)deepTagMD_HbbvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "deepTagMD_TvsQCD", ( double)deepTagMD_TvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "deepTagMD_WvsQCD", ( double)deepTagMD_WvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "deepTagMD_ZHbbvsQCD", ( double)deepTagMD_ZHbbvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "deepTagMD_ZHccvsQCD", ( double)deepTagMD_ZHccvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "deepTagMD_ZbbvsQCD", ( double)deepTagMD_ZbbvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "deepTagMD_ZvsQCD", ( double)deepTagMD_ZvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "deepTagMD_bbvsLight", ( double)deepTagMD_bbvsLight); os << r;
      sprintf(r, "  %-32s: %f\n", "deepTagMD_ccvsLight", ( double)deepTagMD_ccvsLight); os << r;
      sprintf(r, "  %-32s: %f\n", "deepTag_H", ( double)deepTag_H); os << r;
      sprintf(r, "  %-32s: %f\n", "deepTag_QCD", ( double)deepTag_QCD); os << r;
      sprintf(r, "  %-32s: %f\n", "deepTag_QCDothers", ( double)deepTag_QCDothers); os << r;
      sprintf(r, "  %-32s: %f\n", "deepTag_TvsQCD", ( double)deepTag_TvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "deepTag_WvsQCD", ( double)deepTag_WvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "deepTag_ZvsQCD", ( double)deepTag_ZvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "jetId", ( double)jetId); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "msoftdrop", ( double)msoftdrop); os << r;
      sprintf(r, "  %-32s: %f\n", "n2b1", ( double)n2b1); os << r;
      sprintf(r, "  %-32s: %f\n", "n3b1", ( double)n3b1); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNetMD_QCD", ( double)particleNetMD_QCD); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNetMD_Xbb", ( double)particleNetMD_Xbb); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNetMD_Xcc", ( double)particleNetMD_Xcc); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNetMD_Xqq", ( double)particleNetMD_Xqq); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNet_HbbvsQCD", ( double)particleNet_HbbvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNet_TvsQCD", ( double)particleNet_TvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNet_WvsQCD", ( double)particleNet_WvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNet_ZvsQCD", ( double)particleNet_ZvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNet_mass", ( double)particleNet_mass); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      sprintf(r, "  %-32s: %f\n", "rawFactor", ( double)rawFactor); os << r;
      sprintf(r, "  %-32s: %f\n", "tau1", ( double)tau1); os << r;
      sprintf(r, "  %-32s: %f\n", "tau2", ( double)tau2); os << r;
      sprintf(r, "  %-32s: %f\n", "tau3", ( double)tau3); os << r;
      sprintf(r, "  %-32s: %f\n", "tau4", ( double)tau4); os << r;
      return os;
    }
  };

  struct GenJet_s
  {
    float	eta;
    int	hadronFlavour;
    float	mass;
    int	partonFlavour;
    float	phi;
    float	pt;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "GenJet" << std::endl;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "hadronFlavour", ( double)hadronFlavour); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "partonFlavour", ( double)partonFlavour); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      return os;
    }
  };

  struct GenPart_s
  {
    float	eta;
    int	genPartIdxMother;
    float	mass;
    int	pdgId;
    float	phi;
    float	pt;
    int	status;
    int	statusFlags;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "GenPart" << std::endl;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "genPartIdxMother", ( double)genPartIdxMother); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "pdgId", ( double)pdgId); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      sprintf(r, "  %-32s: %f\n", "status", ( double)status); os << r;
      sprintf(r, "  %-32s: %f\n", "statusFlags", ( double)statusFlags); os << r;
      return os;
    }
  };

  struct Jet_s
  {
    float	area;
    float	bRegCorr;
    float	bRegRes;
    float	btagDeepFlavB;
    float	btagDeepFlavCvB;
    float	btagDeepFlavCvL;
    float	btagDeepFlavQG;
    float	eta;
    int	jetId;
    float	mass;
    float	phi;
    float	pt;
    int	puId;
    float	qgl;
    float	rawFactor;
    int	genJetIdx;
    int	hadronFlavour;
    int	partonFlavour;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "Jet" << std::endl;
      sprintf(r, "  %-32s: %f\n", "area", ( double)area); os << r;
      sprintf(r, "  %-32s: %f\n", "bRegCorr", ( double)bRegCorr); os << r;
      sprintf(r, "  %-32s: %f\n", "bRegRes", ( double)bRegRes); os << r;
      sprintf(r, "  %-32s: %f\n", "btagDeepFlavB", ( double)btagDeepFlavB); os << r;
      sprintf(r, "  %-32s: %f\n", "btagDeepFlavCvB", ( double)btagDeepFlavCvB); os << r;
      sprintf(r, "  %-32s: %f\n", "btagDeepFlavCvL", ( double)btagDeepFlavCvL); os << r;
      sprintf(r, "  %-32s: %f\n", "btagDeepFlavQG", ( double)btagDeepFlavQG); os << r;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "jetId", ( double)jetId); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      sprintf(r, "  %-32s: %f\n", "puId", ( double)puId); os << r;
      sprintf(r, "  %-32s: %f\n", "qgl", ( double)qgl); os << r;
      sprintf(r, "  %-32s: %f\n", "rawFactor", ( double)rawFactor); os << r;
      sprintf(r, "  %-32s: %f\n", "genJetIdx", ( double)genJetIdx); os << r;
      sprintf(r, "  %-32s: %f\n", "hadronFlavour", ( double)hadronFlavour); os << r;
      sprintf(r, "  %-32s: %f\n", "partonFlavour", ( double)partonFlavour); os << r;
      return os;
    }
  };

  struct Muon_s
  {
    int	charge;
    float	dxy;
    float	dz;
    float	eta;
    bool	looseId;
    float	mass;
    bool	mediumId;
    float	miniPFRelIso_all;
    float	mvaTTH;
    float	pfRelIso04_all;
    float	phi;
    float	pt;
    bool	tightId;
    int	genPartIdx;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "Muon" << std::endl;
      sprintf(r, "  %-32s: %f\n", "charge", ( double)charge); os << r;
      sprintf(r, "  %-32s: %f\n", "dxy", ( double)dxy); os << r;
      sprintf(r, "  %-32s: %f\n", "dz", ( double)dz); os << r;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "looseId", ( double)looseId); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "mediumId", ( double)mediumId); os << r;
      sprintf(r, "  %-32s: %f\n", "miniPFRelIso_all", ( double)miniPFRelIso_all); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaTTH", ( double)mvaTTH); os << r;
      sprintf(r, "  %-32s: %f\n", "pfRelIso04_all", ( double)pfRelIso04_all); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      sprintf(r, "  %-32s: %f\n", "tightId", ( double)tightId); os << r;
      sprintf(r, "  %-32s: %f\n", "genPartIdx", ( double)genPartIdx); os << r;
      return os;
    }
  };


  void fillElectrons()
  {
    Electron.resize(Electron_charge.size());
    for(unsigned int i=0; i < Electron.size(); ++i)
      {
        Electron[i].charge	= (Electron_charge.size() > i) ? Electron_charge[i] : 0;
        Electron[i].cutBased	= (Electron_cutBased.size() > i) ? Electron_cutBased[i] : 0;
        Electron[i].deltaEtaSC	= (Electron_deltaEtaSC.size() > i) ? Electron_deltaEtaSC[i] : 0;
        Electron[i].dxy	= (Electron_dxy.size() > i) ? Electron_dxy[i] : 0;
        Electron[i].dz	= (Electron_dz.size() > i) ? Electron_dz[i] : 0;
        Electron[i].eta	= (Electron_eta.size() > i) ? Electron_eta[i] : 0;
        Electron[i].mass	= (Electron_mass.size() > i) ? Electron_mass[i] : 0;
        Electron[i].miniPFRelIso_all	= (Electron_miniPFRelIso_all.size() > i) ? Electron_miniPFRelIso_all[i] : 0;
        Electron[i].mvaFall17V2Iso_WP80	= (Electron_mvaFall17V2Iso_WP80.size() > i) ? (bool)Electron_mvaFall17V2Iso_WP80[i] : 0;
        Electron[i].mvaFall17V2Iso_WP90	= (Electron_mvaFall17V2Iso_WP90.size() > i) ? (bool)Electron_mvaFall17V2Iso_WP90[i] : 0;
        Electron[i].mvaTTH	= (Electron_mvaTTH.size() > i) ? Electron_mvaTTH[i] : 0;
        Electron[i].pfRelIso03_all	= (Electron_pfRelIso03_all.size() > i) ? Electron_pfRelIso03_all[i] : 0;
        Electron[i].phi	= (Electron_phi.size() > i) ? Electron_phi[i] : 0;
        Electron[i].pt	= (Electron_pt.size() > i) ? Electron_pt[i] : 0;
        Electron[i].genPartIdx	= (Electron_genPartIdx.size() > i) ? Electron_genPartIdx[i] : 0;
      }
  }

  void fillFatJets()
  {
    FatJet.resize(FatJet_area.size());
    for(unsigned int i=0; i < FatJet.size(); ++i)
      {
        FatJet[i].area	= (FatJet_area.size() > i) ? FatJet_area[i] : 0;
        FatJet[i].deepTagMD_H4qvsQCD	= (FatJet_deepTagMD_H4qvsQCD.size() > i) ? FatJet_deepTagMD_H4qvsQCD[i] : 0;
        FatJet[i].deepTagMD_HbbvsQCD	= (FatJet_deepTagMD_HbbvsQCD.size() > i) ? FatJet_deepTagMD_HbbvsQCD[i] : 0;
        FatJet[i].deepTagMD_TvsQCD	= (FatJet_deepTagMD_TvsQCD.size() > i) ? FatJet_deepTagMD_TvsQCD[i] : 0;
        FatJet[i].deepTagMD_WvsQCD	= (FatJet_deepTagMD_WvsQCD.size() > i) ? FatJet_deepTagMD_WvsQCD[i] : 0;
        FatJet[i].deepTagMD_ZHbbvsQCD	= (FatJet_deepTagMD_ZHbbvsQCD.size() > i) ? FatJet_deepTagMD_ZHbbvsQCD[i] : 0;
        FatJet[i].deepTagMD_ZHccvsQCD	= (FatJet_deepTagMD_ZHccvsQCD.size() > i) ? FatJet_deepTagMD_ZHccvsQCD[i] : 0;
        FatJet[i].deepTagMD_ZbbvsQCD	= (FatJet_deepTagMD_ZbbvsQCD.size() > i) ? FatJet_deepTagMD_ZbbvsQCD[i] : 0;
        FatJet[i].deepTagMD_ZvsQCD	= (FatJet_deepTagMD_ZvsQCD.size() > i) ? FatJet_deepTagMD_ZvsQCD[i] : 0;
        FatJet[i].deepTagMD_bbvsLight	= (FatJet_deepTagMD_bbvsLight.size() > i) ? FatJet_deepTagMD_bbvsLight[i] : 0;
        FatJet[i].deepTagMD_ccvsLight	= (FatJet_deepTagMD_ccvsLight.size() > i) ? FatJet_deepTagMD_ccvsLight[i] : 0;
        FatJet[i].deepTag_H	= (FatJet_deepTag_H.size() > i) ? FatJet_deepTag_H[i] : 0;
        FatJet[i].deepTag_QCD	= (FatJet_deepTag_QCD.size() > i) ? FatJet_deepTag_QCD[i] : 0;
        FatJet[i].deepTag_QCDothers	= (FatJet_deepTag_QCDothers.size() > i) ? FatJet_deepTag_QCDothers[i] : 0;
        FatJet[i].deepTag_TvsQCD	= (FatJet_deepTag_TvsQCD.size() > i) ? FatJet_deepTag_TvsQCD[i] : 0;
        FatJet[i].deepTag_WvsQCD	= (FatJet_deepTag_WvsQCD.size() > i) ? FatJet_deepTag_WvsQCD[i] : 0;
        FatJet[i].deepTag_ZvsQCD	= (FatJet_deepTag_ZvsQCD.size() > i) ? FatJet_deepTag_ZvsQCD[i] : 0;
        FatJet[i].eta	= (FatJet_eta.size() > i) ? FatJet_eta[i] : 0;
        FatJet[i].jetId	= (FatJet_jetId.size() > i) ? FatJet_jetId[i] : 0;
        FatJet[i].mass	= (FatJet_mass.size() > i) ? FatJet_mass[i] : 0;
        FatJet[i].msoftdrop	= (FatJet_msoftdrop.size() > i) ? FatJet_msoftdrop[i] : 0;
        FatJet[i].n2b1	= (FatJet_n2b1.size() > i) ? FatJet_n2b1[i] : 0;
        FatJet[i].n3b1	= (FatJet_n3b1.size() > i) ? FatJet_n3b1[i] : 0;
        FatJet[i].particleNetMD_QCD	= (FatJet_particleNetMD_QCD.size() > i) ? FatJet_particleNetMD_QCD[i] : 0;
        FatJet[i].particleNetMD_Xbb	= (FatJet_particleNetMD_Xbb.size() > i) ? FatJet_particleNetMD_Xbb[i] : 0;
        FatJet[i].particleNetMD_Xcc	= (FatJet_particleNetMD_Xcc.size() > i) ? FatJet_particleNetMD_Xcc[i] : 0;
        FatJet[i].particleNetMD_Xqq	= (FatJet_particleNetMD_Xqq.size() > i) ? FatJet_particleNetMD_Xqq[i] : 0;
        FatJet[i].particleNet_HbbvsQCD	= (FatJet_particleNet_HbbvsQCD.size() > i) ? FatJet_particleNet_HbbvsQCD[i] : 0;
        FatJet[i].particleNet_TvsQCD	= (FatJet_particleNet_TvsQCD.size() > i) ? FatJet_particleNet_TvsQCD[i] : 0;
        FatJet[i].particleNet_WvsQCD	= (FatJet_particleNet_WvsQCD.size() > i) ? FatJet_particleNet_WvsQCD[i] : 0;
        FatJet[i].particleNet_ZvsQCD	= (FatJet_particleNet_ZvsQCD.size() > i) ? FatJet_particleNet_ZvsQCD[i] : 0;
        FatJet[i].particleNet_mass	= (FatJet_particleNet_mass.size() > i) ? FatJet_particleNet_mass[i] : 0;
        FatJet[i].phi	= (FatJet_phi.size() > i) ? FatJet_phi[i] : 0;
        FatJet[i].pt	= (FatJet_pt.size() > i) ? FatJet_pt[i] : 0;
        FatJet[i].rawFactor	= (FatJet_rawFactor.size() > i) ? FatJet_rawFactor[i] : 0;
        FatJet[i].tau1	= (FatJet_tau1.size() > i) ? FatJet_tau1[i] : 0;
        FatJet[i].tau2	= (FatJet_tau2.size() > i) ? FatJet_tau2[i] : 0;
        FatJet[i].tau3	= (FatJet_tau3.size() > i) ? FatJet_tau3[i] : 0;
        FatJet[i].tau4	= (FatJet_tau4.size() > i) ? FatJet_tau4[i] : 0;
      }
  }

  void fillGenJets()
  {
    GenJet.resize(GenJet_eta.size());
    for(unsigned int i=0; i < GenJet.size(); ++i)
      {
        GenJet[i].eta	= (GenJet_eta.size() > i) ? GenJet_eta[i] : 0;
        GenJet[i].hadronFlavour	= (GenJet_hadronFlavour.size() > i) ? GenJet_hadronFlavour[i] : 0;
        GenJet[i].mass	= (GenJet_mass.size() > i) ? GenJet_mass[i] : 0;
        GenJet[i].partonFlavour	= (GenJet_partonFlavour.size() > i) ? GenJet_partonFlavour[i] : 0;
        GenJet[i].phi	= (GenJet_phi.size() > i) ? GenJet_phi[i] : 0;
        GenJet[i].pt	= (GenJet_pt.size() > i) ? GenJet_pt[i] : 0;
      }
  }

  void fillGenParts()
  {
    GenPart.resize(GenPart_eta.size());
    for(unsigned int i=0; i < GenPart.size(); ++i)
      {
        GenPart[i].eta	= (GenPart_eta.size() > i) ? GenPart_eta[i] : 0;
        GenPart[i].genPartIdxMother	= (GenPart_genPartIdxMother.size() > i) ? GenPart_genPartIdxMother[i] : 0;
        GenPart[i].mass	= (GenPart_mass.size() > i) ? GenPart_mass[i] : 0;
        GenPart[i].pdgId	= (GenPart_pdgId.size() > i) ? GenPart_pdgId[i] : 0;
        GenPart[i].phi	= (GenPart_phi.size() > i) ? GenPart_phi[i] : 0;
        GenPart[i].pt	= (GenPart_pt.size() > i) ? GenPart_pt[i] : 0;
        GenPart[i].status	= (GenPart_status.size() > i) ? GenPart_status[i] : 0;
        GenPart[i].statusFlags	= (GenPart_statusFlags.size() > i) ? GenPart_statusFlags[i] : 0;
      }
  }

  void fillJets()
  {
    Jet.resize(Jet_area.size());
    for(unsigned int i=0; i < Jet.size(); ++i)
      {
        Jet[i].area	= (Jet_area.size() > i) ? Jet_area[i] : 0;
        Jet[i].bRegCorr	= (Jet_bRegCorr.size() > i) ? Jet_bRegCorr[i] : 0;
        Jet[i].bRegRes	= (Jet_bRegRes.size() > i) ? Jet_bRegRes[i] : 0;
        Jet[i].btagDeepFlavB	= (Jet_btagDeepFlavB.size() > i) ? Jet_btagDeepFlavB[i] : 0;
        Jet[i].btagDeepFlavCvB	= (Jet_btagDeepFlavCvB.size() > i) ? Jet_btagDeepFlavCvB[i] : 0;
        Jet[i].btagDeepFlavCvL	= (Jet_btagDeepFlavCvL.size() > i) ? Jet_btagDeepFlavCvL[i] : 0;
        Jet[i].btagDeepFlavQG	= (Jet_btagDeepFlavQG.size() > i) ? Jet_btagDeepFlavQG[i] : 0;
        Jet[i].eta	= (Jet_eta.size() > i) ? Jet_eta[i] : 0;
        Jet[i].jetId	= (Jet_jetId.size() > i) ? Jet_jetId[i] : 0;
        Jet[i].mass	= (Jet_mass.size() > i) ? Jet_mass[i] : 0;
        Jet[i].phi	= (Jet_phi.size() > i) ? Jet_phi[i] : 0;
        Jet[i].pt	= (Jet_pt.size() > i) ? Jet_pt[i] : 0;
        Jet[i].puId	= (Jet_puId.size() > i) ? Jet_puId[i] : 0;
        Jet[i].qgl	= (Jet_qgl.size() > i) ? Jet_qgl[i] : 0;
        Jet[i].rawFactor	= (Jet_rawFactor.size() > i) ? Jet_rawFactor[i] : 0;
        Jet[i].genJetIdx	= (Jet_genJetIdx.size() > i) ? Jet_genJetIdx[i] : 0;
        Jet[i].hadronFlavour	= (Jet_hadronFlavour.size() > i) ? Jet_hadronFlavour[i] : 0;
        Jet[i].partonFlavour	= (Jet_partonFlavour.size() > i) ? Jet_partonFlavour[i] : 0;
      }
  }

  void fillMuons()
  {
    Muon.resize(Muon_charge.size());
    for(unsigned int i=0; i < Muon.size(); ++i)
      {
        Muon[i].charge	= (Muon_charge.size() > i) ? Muon_charge[i] : 0;
        Muon[i].dxy	= (Muon_dxy.size() > i) ? Muon_dxy[i] : 0;
        Muon[i].dz	= (Muon_dz.size() > i) ? Muon_dz[i] : 0;
        Muon[i].eta	= (Muon_eta.size() > i) ? Muon_eta[i] : 0;
        Muon[i].looseId	= (Muon_looseId.size() > i) ? (bool)Muon_looseId[i] : 0;
        Muon[i].mass	= (Muon_mass.size() > i) ? Muon_mass[i] : 0;
        Muon[i].mediumId	= (Muon_mediumId.size() > i) ? (bool)Muon_mediumId[i] : 0;
        Muon[i].miniPFRelIso_all	= (Muon_miniPFRelIso_all.size() > i) ? Muon_miniPFRelIso_all[i] : 0;
        Muon[i].mvaTTH	= (Muon_mvaTTH.size() > i) ? Muon_mvaTTH[i] : 0;
        Muon[i].pfRelIso04_all	= (Muon_pfRelIso04_all.size() > i) ? Muon_pfRelIso04_all[i] : 0;
        Muon[i].phi	= (Muon_phi.size() > i) ? Muon_phi[i] : 0;
        Muon[i].pt	= (Muon_pt.size() > i) ? Muon_pt[i] : 0;
        Muon[i].tightId	= (Muon_tightId.size() > i) ? (bool)Muon_tightId[i] : 0;
        Muon[i].genPartIdx	= (Muon_genPartIdx.size() > i) ? Muon_genPartIdx[i] : 0;
      }
  }


  std::vector<eventBuffer::Electron_s> Electron;
  std::vector<eventBuffer::FatJet_s> FatJet;
  std::vector<eventBuffer::GenJet_s> GenJet;
  std::vector<eventBuffer::GenPart_s> GenPart;
  std::vector<eventBuffer::Jet_s> Jet;
  std::vector<eventBuffer::Muon_s> Muon;

  void fillObjects()
  {
    fillElectrons();
    fillFatJets();
    fillGenJets();
    fillGenParts();
    fillJets();
    fillMuons();
  }

   //--------------------------------------------------------------------------
  // Save objects for which the select function was called
  void saveObjects()
  {
    int n = 0;

    n = 0;
    try
      {
         n = indexmap["Electron"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["Electron"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            Electron_charge[i]	= Electron_charge[j];
            Electron_cutBased[i]	= Electron_cutBased[j];
            Electron_deltaEtaSC[i]	= Electron_deltaEtaSC[j];
            Electron_dxy[i]	= Electron_dxy[j];
            Electron_dz[i]	= Electron_dz[j];
            Electron_eta[i]	= Electron_eta[j];
            Electron_mass[i]	= Electron_mass[j];
            Electron_miniPFRelIso_all[i]	= Electron_miniPFRelIso_all[j];
            Electron_mvaFall17V2Iso_WP80[i]	= Electron_mvaFall17V2Iso_WP80[j];
            Electron_mvaFall17V2Iso_WP90[i]	= Electron_mvaFall17V2Iso_WP90[j];
            Electron_mvaTTH[i]	= Electron_mvaTTH[j];
            Electron_pfRelIso03_all[i]	= Electron_pfRelIso03_all[j];
            Electron_phi[i]	= Electron_phi[j];
            Electron_pt[i]	= Electron_pt[j];
            Electron_genPartIdx[i]	= Electron_genPartIdx[j];
          }
      }
    nElectron = n;

    n = 0;
    try
      {
         n = indexmap["FatJet"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["FatJet"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            FatJet_area[i]	= FatJet_area[j];
            FatJet_deepTagMD_H4qvsQCD[i]	= FatJet_deepTagMD_H4qvsQCD[j];
            FatJet_deepTagMD_HbbvsQCD[i]	= FatJet_deepTagMD_HbbvsQCD[j];
            FatJet_deepTagMD_TvsQCD[i]	= FatJet_deepTagMD_TvsQCD[j];
            FatJet_deepTagMD_WvsQCD[i]	= FatJet_deepTagMD_WvsQCD[j];
            FatJet_deepTagMD_ZHbbvsQCD[i]	= FatJet_deepTagMD_ZHbbvsQCD[j];
            FatJet_deepTagMD_ZHccvsQCD[i]	= FatJet_deepTagMD_ZHccvsQCD[j];
            FatJet_deepTagMD_ZbbvsQCD[i]	= FatJet_deepTagMD_ZbbvsQCD[j];
            FatJet_deepTagMD_ZvsQCD[i]	= FatJet_deepTagMD_ZvsQCD[j];
            FatJet_deepTagMD_bbvsLight[i]	= FatJet_deepTagMD_bbvsLight[j];
            FatJet_deepTagMD_ccvsLight[i]	= FatJet_deepTagMD_ccvsLight[j];
            FatJet_deepTag_H[i]	= FatJet_deepTag_H[j];
            FatJet_deepTag_QCD[i]	= FatJet_deepTag_QCD[j];
            FatJet_deepTag_QCDothers[i]	= FatJet_deepTag_QCDothers[j];
            FatJet_deepTag_TvsQCD[i]	= FatJet_deepTag_TvsQCD[j];
            FatJet_deepTag_WvsQCD[i]	= FatJet_deepTag_WvsQCD[j];
            FatJet_deepTag_ZvsQCD[i]	= FatJet_deepTag_ZvsQCD[j];
            FatJet_eta[i]	= FatJet_eta[j];
            FatJet_jetId[i]	= FatJet_jetId[j];
            FatJet_mass[i]	= FatJet_mass[j];
            FatJet_msoftdrop[i]	= FatJet_msoftdrop[j];
            FatJet_n2b1[i]	= FatJet_n2b1[j];
            FatJet_n3b1[i]	= FatJet_n3b1[j];
            FatJet_particleNetMD_QCD[i]	= FatJet_particleNetMD_QCD[j];
            FatJet_particleNetMD_Xbb[i]	= FatJet_particleNetMD_Xbb[j];
            FatJet_particleNetMD_Xcc[i]	= FatJet_particleNetMD_Xcc[j];
            FatJet_particleNetMD_Xqq[i]	= FatJet_particleNetMD_Xqq[j];
            FatJet_particleNet_HbbvsQCD[i]	= FatJet_particleNet_HbbvsQCD[j];
            FatJet_particleNet_TvsQCD[i]	= FatJet_particleNet_TvsQCD[j];
            FatJet_particleNet_WvsQCD[i]	= FatJet_particleNet_WvsQCD[j];
            FatJet_particleNet_ZvsQCD[i]	= FatJet_particleNet_ZvsQCD[j];
            FatJet_particleNet_mass[i]	= FatJet_particleNet_mass[j];
            FatJet_phi[i]	= FatJet_phi[j];
            FatJet_pt[i]	= FatJet_pt[j];
            FatJet_rawFactor[i]	= FatJet_rawFactor[j];
            FatJet_tau1[i]	= FatJet_tau1[j];
            FatJet_tau2[i]	= FatJet_tau2[j];
            FatJet_tau3[i]	= FatJet_tau3[j];
            FatJet_tau4[i]	= FatJet_tau4[j];
          }
      }
    nFatJet = n;

    n = 0;
    try
      {
         n = indexmap["GenJet"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["GenJet"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            GenJet_eta[i]	= GenJet_eta[j];
            GenJet_hadronFlavour[i]	= GenJet_hadronFlavour[j];
            GenJet_mass[i]	= GenJet_mass[j];
            GenJet_partonFlavour[i]	= GenJet_partonFlavour[j];
            GenJet_phi[i]	= GenJet_phi[j];
            GenJet_pt[i]	= GenJet_pt[j];
          }
      }
    nGenJet = n;

    n = 0;
    try
      {
         n = indexmap["GenPart"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["GenPart"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            GenPart_eta[i]	= GenPart_eta[j];
            GenPart_genPartIdxMother[i]	= GenPart_genPartIdxMother[j];
            GenPart_mass[i]	= GenPart_mass[j];
            GenPart_pdgId[i]	= GenPart_pdgId[j];
            GenPart_phi[i]	= GenPart_phi[j];
            GenPart_pt[i]	= GenPart_pt[j];
            GenPart_status[i]	= GenPart_status[j];
            GenPart_statusFlags[i]	= GenPart_statusFlags[j];
          }
      }
    nGenPart = n;

    n = 0;
    try
      {
         n = indexmap["Jet"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["Jet"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            Jet_area[i]	= Jet_area[j];
            Jet_bRegCorr[i]	= Jet_bRegCorr[j];
            Jet_bRegRes[i]	= Jet_bRegRes[j];
            Jet_btagDeepFlavB[i]	= Jet_btagDeepFlavB[j];
            Jet_btagDeepFlavCvB[i]	= Jet_btagDeepFlavCvB[j];
            Jet_btagDeepFlavCvL[i]	= Jet_btagDeepFlavCvL[j];
            Jet_btagDeepFlavQG[i]	= Jet_btagDeepFlavQG[j];
            Jet_eta[i]	= Jet_eta[j];
            Jet_jetId[i]	= Jet_jetId[j];
            Jet_mass[i]	= Jet_mass[j];
            Jet_phi[i]	= Jet_phi[j];
            Jet_pt[i]	= Jet_pt[j];
            Jet_puId[i]	= Jet_puId[j];
            Jet_qgl[i]	= Jet_qgl[j];
            Jet_rawFactor[i]	= Jet_rawFactor[j];
            Jet_genJetIdx[i]	= Jet_genJetIdx[j];
            Jet_hadronFlavour[i]	= Jet_hadronFlavour[j];
            Jet_partonFlavour[i]	= Jet_partonFlavour[j];
          }
      }
    nJet = n;

    n = 0;
    try
      {
         n = indexmap["Muon"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["Muon"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            Muon_charge[i]	= Muon_charge[j];
            Muon_dxy[i]	= Muon_dxy[j];
            Muon_dz[i]	= Muon_dz[j];
            Muon_eta[i]	= Muon_eta[j];
            Muon_looseId[i]	= Muon_looseId[j];
            Muon_mass[i]	= Muon_mass[j];
            Muon_mediumId[i]	= Muon_mediumId[j];
            Muon_miniPFRelIso_all[i]	= Muon_miniPFRelIso_all[j];
            Muon_mvaTTH[i]	= Muon_mvaTTH[j];
            Muon_pfRelIso04_all[i]	= Muon_pfRelIso04_all[j];
            Muon_phi[i]	= Muon_phi[j];
            Muon_pt[i]	= Muon_pt[j];
            Muon_tightId[i]	= Muon_tightId[j];
            Muon_genPartIdx[i]	= Muon_genPartIdx[j];
          }
      }
    nMuon = n;
  }

  //--------------------------------------------------------------------------
  // A read-only buffer 
  eventBuffer() : input(0), output(0), choose(std::map<std::string, bool>()) {}
  eventBuffer(itreestream& stream, std::string varlist="")
  : input(&stream),
    output(0),
    choose(std::map<std::string, bool>())
  {
    if ( !input->good() ) 
      {
        std::cout << "eventBuffer - please check stream!" 
                  << std::endl;
	    exit(0);
      }

    initBuffers();
    
    // default is to select all branches      
    bool DEFAULT = varlist == "";
    choose["Events/Electron_charge"]	= DEFAULT;
    choose["Events/Electron_cutBased"]	= DEFAULT;
    choose["Events/Electron_deltaEtaSC"]	= DEFAULT;
    choose["Events/Electron_dxy"]	= DEFAULT;
    choose["Events/Electron_dz"]	= DEFAULT;
    choose["Events/Electron_eta"]	= DEFAULT;
    choose["Events/Electron_genPartIdx"]	= DEFAULT;
    choose["Events/Electron_mass"]	= DEFAULT;
    choose["Events/Electron_miniPFRelIso_all"]	= DEFAULT;
    choose["Events/Electron_mvaFall17V2Iso_WP80"]	= DEFAULT;
    choose["Events/Electron_mvaFall17V2Iso_WP90"]	= DEFAULT;
    choose["Events/Electron_mvaTTH"]	= DEFAULT;
    choose["Events/Electron_pfRelIso03_all"]	= DEFAULT;
    choose["Events/Electron_phi"]	= DEFAULT;
    choose["Events/Electron_pt"]	= DEFAULT;
    choose["Events/FatJet_area"]	= DEFAULT;
    choose["Events/FatJet_deepTagMD_H4qvsQCD"]	= DEFAULT;
    choose["Events/FatJet_deepTagMD_HbbvsQCD"]	= DEFAULT;
    choose["Events/FatJet_deepTagMD_TvsQCD"]	= DEFAULT;
    choose["Events/FatJet_deepTagMD_WvsQCD"]	= DEFAULT;
    choose["Events/FatJet_deepTagMD_ZHbbvsQCD"]	= DEFAULT;
    choose["Events/FatJet_deepTagMD_ZHccvsQCD"]	= DEFAULT;
    choose["Events/FatJet_deepTagMD_ZbbvsQCD"]	= DEFAULT;
    choose["Events/FatJet_deepTagMD_ZvsQCD"]	= DEFAULT;
    choose["Events/FatJet_deepTagMD_bbvsLight"]	= DEFAULT;
    choose["Events/FatJet_deepTagMD_ccvsLight"]	= DEFAULT;
    choose["Events/FatJet_deepTag_H"]	= DEFAULT;
    choose["Events/FatJet_deepTag_QCD"]	= DEFAULT;
    choose["Events/FatJet_deepTag_QCDothers"]	= DEFAULT;
    choose["Events/FatJet_deepTag_TvsQCD"]	= DEFAULT;
    choose["Events/FatJet_deepTag_WvsQCD"]	= DEFAULT;
    choose["Events/FatJet_deepTag_ZvsQCD"]	= DEFAULT;
    choose["Events/FatJet_eta"]	= DEFAULT;
    choose["Events/FatJet_jetId"]	= DEFAULT;
    choose["Events/FatJet_mass"]	= DEFAULT;
    choose["Events/FatJet_msoftdrop"]	= DEFAULT;
    choose["Events/FatJet_n2b1"]	= DEFAULT;
    choose["Events/FatJet_n3b1"]	= DEFAULT;
    choose["Events/FatJet_particleNetMD_QCD"]	= DEFAULT;
    choose["Events/FatJet_particleNetMD_Xbb"]	= DEFAULT;
    choose["Events/FatJet_particleNetMD_Xcc"]	= DEFAULT;
    choose["Events/FatJet_particleNetMD_Xqq"]	= DEFAULT;
    choose["Events/FatJet_particleNet_HbbvsQCD"]	= DEFAULT;
    choose["Events/FatJet_particleNet_TvsQCD"]	= DEFAULT;
    choose["Events/FatJet_particleNet_WvsQCD"]	= DEFAULT;
    choose["Events/FatJet_particleNet_ZvsQCD"]	= DEFAULT;
    choose["Events/FatJet_particleNet_mass"]	= DEFAULT;
    choose["Events/FatJet_phi"]	= DEFAULT;
    choose["Events/FatJet_pt"]	= DEFAULT;
    choose["Events/FatJet_rawFactor"]	= DEFAULT;
    choose["Events/FatJet_tau1"]	= DEFAULT;
    choose["Events/FatJet_tau2"]	= DEFAULT;
    choose["Events/FatJet_tau3"]	= DEFAULT;
    choose["Events/FatJet_tau4"]	= DEFAULT;
    choose["Events/Flag_BadPFMuonDzFilter"]	= DEFAULT;
    choose["Events/Flag_BadPFMuonFilter"]	= DEFAULT;
    choose["Events/Flag_EcalDeadCellTriggerPrimitiveFilter"]	= DEFAULT;
    choose["Events/Flag_HBHENoiseFilter"]	= DEFAULT;
    choose["Events/Flag_HBHENoiseIsoFilter"]	= DEFAULT;
    choose["Events/Flag_ecalBadCalibFilter"]	= DEFAULT;
    choose["Events/Flag_eeBadScFilter"]	= DEFAULT;
    choose["Events/Flag_globalSuperTightHalo2016Filter"]	= DEFAULT;
    choose["Events/Flag_goodVertices"]	= DEFAULT;
    choose["Events/GenJet_eta"]	= DEFAULT;
    choose["Events/GenJet_hadronFlavour"]	= DEFAULT;
    choose["Events/GenJet_mass"]	= DEFAULT;
    choose["Events/GenJet_partonFlavour"]	= DEFAULT;
    choose["Events/GenJet_phi"]	= DEFAULT;
    choose["Events/GenJet_pt"]	= DEFAULT;
    choose["Events/GenPart_eta"]	= DEFAULT;
    choose["Events/GenPart_genPartIdxMother"]	= DEFAULT;
    choose["Events/GenPart_mass"]	= DEFAULT;
    choose["Events/GenPart_pdgId"]	= DEFAULT;
    choose["Events/GenPart_phi"]	= DEFAULT;
    choose["Events/GenPart_pt"]	= DEFAULT;
    choose["Events/GenPart_status"]	= DEFAULT;
    choose["Events/GenPart_statusFlags"]	= DEFAULT;
    choose["Events/Generator_weight"]	= DEFAULT;
    choose["Events/HLT_AK8PFHT750_TrimMass50"]	= DEFAULT;
    choose["Events/HLT_AK8PFHT800_TrimMass50"]	= DEFAULT;
    choose["Events/HLT_AK8PFHT850_TrimMass50"]	= DEFAULT;
    choose["Events/HLT_AK8PFHT900_TrimMass50"]	= DEFAULT;
    choose["Events/HLT_AK8PFJet140"]	= DEFAULT;
    choose["Events/HLT_AK8PFJet200"]	= DEFAULT;
    choose["Events/HLT_AK8PFJet260"]	= DEFAULT;
    choose["Events/HLT_AK8PFJet320"]	= DEFAULT;
    choose["Events/HLT_AK8PFJet330_PFAK8BTagCSV_p1"]	= DEFAULT;
    choose["Events/HLT_AK8PFJet330_PFAK8BTagCSV_p17"]	= DEFAULT;
    choose["Events/HLT_AK8PFJet360_TrimMass30"]	= DEFAULT;
    choose["Events/HLT_AK8PFJet380_TrimMass30"]	= DEFAULT;
    choose["Events/HLT_AK8PFJet40"]	= DEFAULT;
    choose["Events/HLT_AK8PFJet400"]	= DEFAULT;
    choose["Events/HLT_AK8PFJet400_TrimMass30"]	= DEFAULT;
    choose["Events/HLT_AK8PFJet420_TrimMass30"]	= DEFAULT;
    choose["Events/HLT_AK8PFJet450"]	= DEFAULT;
    choose["Events/HLT_AK8PFJet500"]	= DEFAULT;
    choose["Events/HLT_AK8PFJet550"]	= DEFAULT;
    choose["Events/HLT_AK8PFJet60"]	= DEFAULT;
    choose["Events/HLT_AK8PFJet80"]	= DEFAULT;
    choose["Events/HLT_AK8PFJetFwd140"]	= DEFAULT;
    choose["Events/HLT_AK8PFJetFwd200"]	= DEFAULT;
    choose["Events/HLT_AK8PFJetFwd260"]	= DEFAULT;
    choose["Events/HLT_AK8PFJetFwd320"]	= DEFAULT;
    choose["Events/HLT_AK8PFJetFwd40"]	= DEFAULT;
    choose["Events/HLT_AK8PFJetFwd400"]	= DEFAULT;
    choose["Events/HLT_AK8PFJetFwd450"]	= DEFAULT;
    choose["Events/HLT_AK8PFJetFwd500"]	= DEFAULT;
    choose["Events/HLT_AK8PFJetFwd60"]	= DEFAULT;
    choose["Events/HLT_AK8PFJetFwd80"]	= DEFAULT;
    choose["Events/HLT_DiPFJet15_FBEta3_NoCaloMatched"]	= DEFAULT;
    choose["Events/HLT_DiPFJet15_NoCaloMatched"]	= DEFAULT;
    choose["Events/HLT_DiPFJet25_FBEta3_NoCaloMatched"]	= DEFAULT;
    choose["Events/HLT_DiPFJet25_NoCaloMatched"]	= DEFAULT;
    choose["Events/HLT_DiPFJetAve100_HFJEC"]	= DEFAULT;
    choose["Events/HLT_DiPFJetAve140"]	= DEFAULT;
    choose["Events/HLT_DiPFJetAve15_HFJEC"]	= DEFAULT;
    choose["Events/HLT_DiPFJetAve160_HFJEC"]	= DEFAULT;
    choose["Events/HLT_DiPFJetAve200"]	= DEFAULT;
    choose["Events/HLT_DiPFJetAve220_HFJEC"]	= DEFAULT;
    choose["Events/HLT_DiPFJetAve25_HFJEC"]	= DEFAULT;
    choose["Events/HLT_DiPFJetAve260"]	= DEFAULT;
    choose["Events/HLT_DiPFJetAve300_HFJEC"]	= DEFAULT;
    choose["Events/HLT_DiPFJetAve320"]	= DEFAULT;
    choose["Events/HLT_DiPFJetAve35_HFJEC"]	= DEFAULT;
    choose["Events/HLT_DiPFJetAve40"]	= DEFAULT;
    choose["Events/HLT_DiPFJetAve400"]	= DEFAULT;
    choose["Events/HLT_DiPFJetAve500"]	= DEFAULT;
    choose["Events/HLT_DiPFJetAve60"]	= DEFAULT;
    choose["Events/HLT_DiPFJetAve60_HFJEC"]	= DEFAULT;
    choose["Events/HLT_DiPFJetAve80"]	= DEFAULT;
    choose["Events/HLT_DiPFJetAve80_HFJEC"]	= DEFAULT;
    choose["Events/HLT_Ele115_CaloIdVT_GsfTrkIdT"]	= DEFAULT;
    choose["Events/HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30"]	= DEFAULT;
    choose["Events/HLT_Ele135_CaloIdVT_GsfTrkIdT"]	= DEFAULT;
    choose["Events/HLT_Ele145_CaloIdVT_GsfTrkIdT"]	= DEFAULT;
    choose["Events/HLT_Ele15_IsoVVVL_PFHT450"]	= DEFAULT;
    choose["Events/HLT_Ele15_IsoVVVL_PFHT450_CaloBTagCSV_4p5"]	= DEFAULT;
    choose["Events/HLT_Ele15_IsoVVVL_PFHT450_PFMET50"]	= DEFAULT;
    choose["Events/HLT_Ele15_IsoVVVL_PFHT600"]	= DEFAULT;
    choose["Events/HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL"]	= DEFAULT;
    choose["Events/HLT_Ele17_CaloIdM_TrackIdM_PFJet30"]	= DEFAULT;
    choose["Events/HLT_Ele200_CaloIdVT_GsfTrkIdT"]	= DEFAULT;
    choose["Events/HLT_Ele20_WPLoose_Gsf"]	= DEFAULT;
    choose["Events/HLT_Ele20_WPTight_Gsf"]	= DEFAULT;
    choose["Events/HLT_Ele20_eta2p1_WPLoose_Gsf"]	= DEFAULT;
    choose["Events/HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30"]	= DEFAULT;
    choose["Events/HLT_Ele23_CaloIdM_TrackIdM_PFJet30"]	= DEFAULT;
    choose["Events/HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL"]	= DEFAULT;
    choose["Events/HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ"]	= DEFAULT;
    choose["Events/HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_CrossL1"]	= DEFAULT;
    choose["Events/HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_TightID_CrossL1"]	= DEFAULT;
    choose["Events/HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_CrossL1"]	= DEFAULT;
    choose["Events/HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_TightID_CrossL1"]	= DEFAULT;
    choose["Events/HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_CrossL1"]	= DEFAULT;
    choose["Events/HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_TightID_CrossL1"]	= DEFAULT;
    choose["Events/HLT_Ele250_CaloIdVT_GsfTrkIdT"]	= DEFAULT;
    choose["Events/HLT_Ele27_Ele37_CaloIdL_MW"]	= DEFAULT;
    choose["Events/HLT_Ele27_WPTight_Gsf"]	= DEFAULT;
    choose["Events/HLT_Ele28_HighEta_SC20_Mass55"]	= DEFAULT;
    choose["Events/HLT_Ele28_eta2p1_WPTight_Gsf_HT150"]	= DEFAULT;
    choose["Events/HLT_Ele300_CaloIdVT_GsfTrkIdT"]	= DEFAULT;
    choose["Events/HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned"]	= DEFAULT;
    choose["Events/HLT_Ele32_WPTight_Gsf"]	= DEFAULT;
    choose["Events/HLT_Ele32_WPTight_Gsf_L1DoubleEG"]	= DEFAULT;
    choose["Events/HLT_Ele35_WPTight_Gsf"]	= DEFAULT;
    choose["Events/HLT_Ele35_WPTight_Gsf_L1EGMT"]	= DEFAULT;
    choose["Events/HLT_Ele38_WPTight_Gsf"]	= DEFAULT;
    choose["Events/HLT_Ele40_WPTight_Gsf"]	= DEFAULT;
    choose["Events/HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165"]	= DEFAULT;
    choose["Events/HLT_Ele50_IsoVVVL_PFHT450"]	= DEFAULT;
    choose["Events/HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30"]	= DEFAULT;
    choose["Events/HLT_Ele8_CaloIdM_TrackIdM_PFJet30"]	= DEFAULT;
    choose["Events/HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07"]	= DEFAULT;
    choose["Events/HLT_IsoMu20"]	= DEFAULT;
    choose["Events/HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_CrossL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_TightID_CrossL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_CrossL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_TightID_CrossL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_CrossL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_TightID_CrossL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu24"]	= DEFAULT;
    choose["Events/HLT_IsoMu24_eta2p1"]	= DEFAULT;
    choose["Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_SingleL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_TightID_SingleL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_SingleL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_TightID_SingleL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau50_Trk30_eta2p1_1pr"]	= DEFAULT;
    choose["Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_SingleL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_TightID_SingleL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu27"]	= DEFAULT;
    choose["Events/HLT_IsoMu27_LooseChargedIsoPFTau20_SingleL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu27_MediumChargedIsoPFTau20_SingleL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu27_TightChargedIsoPFTau20_SingleL1"]	= DEFAULT;
    choose["Events/HLT_IsoMu30"]	= DEFAULT;
    choose["Events/HLT_Mu10_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT350_PFMETNoMu60"]	= DEFAULT;
    choose["Events/HLT_Mu12_DoublePFJets100_CaloBTagCSV_p33"]	= DEFAULT;
    choose["Events/HLT_Mu12_DoublePFJets200_CaloBTagCSV_p33"]	= DEFAULT;
    choose["Events/HLT_Mu12_DoublePFJets350_CaloBTagCSV_p33"]	= DEFAULT;
    choose["Events/HLT_Mu12_DoublePFJets40MaxDeta1p6_DoubleCaloBTagCSV_p33"]	= DEFAULT;
    choose["Events/HLT_Mu12_DoublePFJets40_CaloBTagCSV_p33"]	= DEFAULT;
    choose["Events/HLT_Mu12_DoublePFJets54MaxDeta1p6_DoubleCaloBTagCSV_p33"]	= DEFAULT;
    choose["Events/HLT_Mu12_DoublePFJets62MaxDeta1p6_DoubleCaloBTagCSV_p33"]	= DEFAULT;
    choose["Events/HLT_Mu12_DoublePhoton20"]	= DEFAULT;
    choose["Events/HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL"]	= DEFAULT;
    choose["Events/HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ"]	= DEFAULT;
    choose["Events/HLT_Mu15_IsoVVVL_PFHT450"]	= DEFAULT;
    choose["Events/HLT_Mu15_IsoVVVL_PFHT450_CaloBTagCSV_4p5"]	= DEFAULT;
    choose["Events/HLT_Mu15_IsoVVVL_PFHT450_PFMET50"]	= DEFAULT;
    choose["Events/HLT_Mu15_IsoVVVL_PFHT600"]	= DEFAULT;
    choose["Events/HLT_Mu17"]	= DEFAULT;
    choose["Events/HLT_Mu17_Photon30_IsoCaloId"]	= DEFAULT;
    choose["Events/HLT_Mu17_TrkIsoVVL"]	= DEFAULT;
    choose["Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL"]	= DEFAULT;
    choose["Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ"]	= DEFAULT;
    choose["Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8"]	= DEFAULT;
    choose["Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8"]	= DEFAULT;
    choose["Events/HLT_Mu18_Mu9"]	= DEFAULT;
    choose["Events/HLT_Mu18_Mu9_DZ"]	= DEFAULT;
    choose["Events/HLT_Mu18_Mu9_SameSign"]	= DEFAULT;
    choose["Events/HLT_Mu18_Mu9_SameSign_DZ"]	= DEFAULT;
    choose["Events/HLT_Mu19"]	= DEFAULT;
    choose["Events/HLT_Mu19_TrkIsoVVL"]	= DEFAULT;
    choose["Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL"]	= DEFAULT;
    choose["Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ"]	= DEFAULT;
    choose["Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass3p8"]	= DEFAULT;
    choose["Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass8"]	= DEFAULT;
    choose["Events/HLT_Mu20"]	= DEFAULT;
    choose["Events/HLT_Mu20_Mu10"]	= DEFAULT;
    choose["Events/HLT_Mu20_Mu10_DZ"]	= DEFAULT;
    choose["Events/HLT_Mu20_Mu10_SameSign"]	= DEFAULT;
    choose["Events/HLT_Mu20_Mu10_SameSign_DZ"]	= DEFAULT;
    choose["Events/HLT_Mu20_TkMu0_Phi"]	= DEFAULT;
    choose["Events/HLT_Mu23_Mu12"]	= DEFAULT;
    choose["Events/HLT_Mu23_Mu12_DZ"]	= DEFAULT;
    choose["Events/HLT_Mu23_Mu12_SameSign"]	= DEFAULT;
    choose["Events/HLT_Mu23_Mu12_SameSign_DZ"]	= DEFAULT;
    choose["Events/HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL"]	= DEFAULT;
    choose["Events/HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ"]	= DEFAULT;
    choose["Events/HLT_Mu25_TkMu0_Onia"]	= DEFAULT;
    choose["Events/HLT_Mu25_TkMu0_Phi"]	= DEFAULT;
    choose["Events/HLT_Mu27"]	= DEFAULT;
    choose["Events/HLT_Mu27_Ele37_CaloIdL_MW"]	= DEFAULT;
    choose["Events/HLT_Mu30_TkMu0_Onia"]	= DEFAULT;
    choose["Events/HLT_Mu37_Ele27_CaloIdL_MW"]	= DEFAULT;
    choose["Events/HLT_Mu37_TkMu27"]	= DEFAULT;
    choose["Events/HLT_Mu3_PFJet40"]	= DEFAULT;
    choose["Events/HLT_Mu43NoFiltersNoVtx_Photon43_CaloIdL"]	= DEFAULT;
    choose["Events/HLT_Mu48NoFiltersNoVtx_Photon48_CaloIdL"]	= DEFAULT;
    choose["Events/HLT_Mu50"]	= DEFAULT;
    choose["Events/HLT_Mu50_IsoVVVL_PFHT450"]	= DEFAULT;
    choose["Events/HLT_Mu55"]	= DEFAULT;
    choose["Events/HLT_Mu7p5_L2Mu2_Jpsi"]	= DEFAULT;
    choose["Events/HLT_Mu7p5_L2Mu2_Upsilon"]	= DEFAULT;
    choose["Events/HLT_Mu7p5_Track2_Jpsi"]	= DEFAULT;
    choose["Events/HLT_Mu7p5_Track2_Upsilon"]	= DEFAULT;
    choose["Events/HLT_Mu7p5_Track3p5_Jpsi"]	= DEFAULT;
    choose["Events/HLT_Mu7p5_Track3p5_Upsilon"]	= DEFAULT;
    choose["Events/HLT_Mu7p5_Track7_Jpsi"]	= DEFAULT;
    choose["Events/HLT_Mu7p5_Track7_Upsilon"]	= DEFAULT;
    choose["Events/HLT_Mu8"]	= DEFAULT;
    choose["Events/HLT_Mu8_DiEle12_CaloIdL_TrackIdL"]	= DEFAULT;
    choose["Events/HLT_Mu8_DiEle12_CaloIdL_TrackIdL_DZ"]	= DEFAULT;
    choose["Events/HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350"]	= DEFAULT;
    choose["Events/HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ"]	= DEFAULT;
    choose["Events/HLT_Mu8_TrkIsoVVL"]	= DEFAULT;
    choose["Events/HLT_Mu8_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT300_PFMETNoMu60"]	= DEFAULT;
    choose["Events/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL"]	= DEFAULT;
    choose["Events/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ"]	= DEFAULT;
    choose["Events/HLT_PFHT1050"]	= DEFAULT;
    choose["Events/HLT_PFHT180"]	= DEFAULT;
    choose["Events/HLT_PFHT250"]	= DEFAULT;
    choose["Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40"]	= DEFAULT;
    choose["Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0"]	= DEFAULT;
    choose["Events/HLT_PFHT350"]	= DEFAULT;
    choose["Events/HLT_PFHT350MinPFJet15"]	= DEFAULT;
    choose["Events/HLT_PFHT370"]	= DEFAULT;
    choose["Events/HLT_PFHT380_SixJet32_DoubleBTagCSV_p075"]	= DEFAULT;
    choose["Events/HLT_PFHT380_SixPFJet32"]	= DEFAULT;
    choose["Events/HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2"]	= DEFAULT;
    choose["Events/HLT_PFHT380_SixPFJet32_DoublePFBTagDeepCSV_2p2"]	= DEFAULT;
    choose["Events/HLT_PFHT430"]	= DEFAULT;
    choose["Events/HLT_PFHT430_SixJet40_BTagCSV_p080"]	= DEFAULT;
    choose["Events/HLT_PFHT430_SixPFJet40"]	= DEFAULT;
    choose["Events/HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5"]	= DEFAULT;
    choose["Events/HLT_PFHT500_PFMET100_PFMHT100_IDTight"]	= DEFAULT;
    choose["Events/HLT_PFHT500_PFMET110_PFMHT110_IDTight"]	= DEFAULT;
    choose["Events/HLT_PFHT510"]	= DEFAULT;
    choose["Events/HLT_PFHT590"]	= DEFAULT;
    choose["Events/HLT_PFHT680"]	= DEFAULT;
    choose["Events/HLT_PFHT700_PFMET85_PFMHT85_IDTight"]	= DEFAULT;
    choose["Events/HLT_PFHT700_PFMET95_PFMHT95_IDTight"]	= DEFAULT;
    choose["Events/HLT_PFHT780"]	= DEFAULT;
    choose["Events/HLT_PFHT800_PFMET75_PFMHT75_IDTight"]	= DEFAULT;
    choose["Events/HLT_PFHT800_PFMET85_PFMHT85_IDTight"]	= DEFAULT;
    choose["Events/HLT_PFHT890"]	= DEFAULT;
    choose["Events/HLT_QuadPFJet103_88_75_15"]	= DEFAULT;
    choose["Events/HLT_QuadPFJet103_88_75_15_BTagCSV_p013_VBF2"]	= DEFAULT;
    choose["Events/HLT_QuadPFJet103_88_75_15_DoubleBTagCSV_p013_p08_VBF1"]	= DEFAULT;
    choose["Events/HLT_QuadPFJet105_88_76_15"]	= DEFAULT;
    choose["Events/HLT_QuadPFJet105_88_76_15_BTagCSV_p013_VBF2"]	= DEFAULT;
    choose["Events/HLT_QuadPFJet105_90_76_15_DoubleBTagCSV_p013_p08_VBF1"]	= DEFAULT;
    choose["Events/HLT_QuadPFJet111_90_80_15"]	= DEFAULT;
    choose["Events/HLT_QuadPFJet111_90_80_15_BTagCSV_p013_VBF2"]	= DEFAULT;
    choose["Events/HLT_QuadPFJet111_90_80_15_DoubleBTagCSV_p013_p08_VBF1"]	= DEFAULT;
    choose["Events/HLT_QuadPFJet98_83_71_15"]	= DEFAULT;
    choose["Events/HLT_QuadPFJet98_83_71_15_BTagCSV_p013_VBF2"]	= DEFAULT;
    choose["Events/HLT_QuadPFJet98_83_71_15_DoubleBTagCSV_p013_p08_VBF1"]	= DEFAULT;
    choose["Events/Jet_area"]	= DEFAULT;
    choose["Events/Jet_bRegCorr"]	= DEFAULT;
    choose["Events/Jet_bRegRes"]	= DEFAULT;
    choose["Events/Jet_btagDeepFlavB"]	= DEFAULT;
    choose["Events/Jet_btagDeepFlavCvB"]	= DEFAULT;
    choose["Events/Jet_btagDeepFlavCvL"]	= DEFAULT;
    choose["Events/Jet_btagDeepFlavQG"]	= DEFAULT;
    choose["Events/Jet_eta"]	= DEFAULT;
    choose["Events/Jet_genJetIdx"]	= DEFAULT;
    choose["Events/Jet_hadronFlavour"]	= DEFAULT;
    choose["Events/Jet_jetId"]	= DEFAULT;
    choose["Events/Jet_mass"]	= DEFAULT;
    choose["Events/Jet_partonFlavour"]	= DEFAULT;
    choose["Events/Jet_phi"]	= DEFAULT;
    choose["Events/Jet_pt"]	= DEFAULT;
    choose["Events/Jet_puId"]	= DEFAULT;
    choose["Events/Jet_qgl"]	= DEFAULT;
    choose["Events/Jet_rawFactor"]	= DEFAULT;
    choose["Events/L1PreFiringWeight_Dn"]	= DEFAULT;
    choose["Events/L1PreFiringWeight_Nom"]	= DEFAULT;
    choose["Events/L1PreFiringWeight_Up"]	= DEFAULT;
    choose["Events/LHEPdfWeight"]	= DEFAULT;
    choose["Events/LHEScaleWeight"]	= DEFAULT;
    choose["Events/MET_covXX"]	= DEFAULT;
    choose["Events/MET_covXY"]	= DEFAULT;
    choose["Events/MET_covYY"]	= DEFAULT;
    choose["Events/MET_phi"]	= DEFAULT;
    choose["Events/MET_pt"]	= DEFAULT;
    choose["Events/MET_significance"]	= DEFAULT;
    choose["Events/Muon_charge"]	= DEFAULT;
    choose["Events/Muon_dxy"]	= DEFAULT;
    choose["Events/Muon_dz"]	= DEFAULT;
    choose["Events/Muon_eta"]	= DEFAULT;
    choose["Events/Muon_genPartIdx"]	= DEFAULT;
    choose["Events/Muon_looseId"]	= DEFAULT;
    choose["Events/Muon_mass"]	= DEFAULT;
    choose["Events/Muon_mediumId"]	= DEFAULT;
    choose["Events/Muon_miniPFRelIso_all"]	= DEFAULT;
    choose["Events/Muon_mvaTTH"]	= DEFAULT;
    choose["Events/Muon_pfRelIso04_all"]	= DEFAULT;
    choose["Events/Muon_phi"]	= DEFAULT;
    choose["Events/Muon_pt"]	= DEFAULT;
    choose["Events/Muon_tightId"]	= DEFAULT;
    choose["Events/PV_chi2"]	= DEFAULT;
    choose["Events/PV_ndof"]	= DEFAULT;
    choose["Events/PV_npvs"]	= DEFAULT;
    choose["Events/PV_npvsGood"]	= DEFAULT;
    choose["Events/PV_z"]	= DEFAULT;
    choose["Events/Pileup_nPU"]	= DEFAULT;
    choose["Events/Pileup_nTrueInt"]	= DEFAULT;
    choose["Events/PuppiMET_phi"]	= DEFAULT;
    choose["Events/PuppiMET_phiJERDown"]	= DEFAULT;
    choose["Events/PuppiMET_phiJERUp"]	= DEFAULT;
    choose["Events/PuppiMET_phiJESDown"]	= DEFAULT;
    choose["Events/PuppiMET_phiJESUp"]	= DEFAULT;
    choose["Events/PuppiMET_phiUnclusteredDown"]	= DEFAULT;
    choose["Events/PuppiMET_phiUnclusteredUp"]	= DEFAULT;
    choose["Events/PuppiMET_pt"]	= DEFAULT;
    choose["Events/PuppiMET_ptJERDown"]	= DEFAULT;
    choose["Events/PuppiMET_ptJERUp"]	= DEFAULT;
    choose["Events/PuppiMET_ptJESDown"]	= DEFAULT;
    choose["Events/PuppiMET_ptJESUp"]	= DEFAULT;
    choose["Events/PuppiMET_ptUnclusteredDown"]	= DEFAULT;
    choose["Events/PuppiMET_ptUnclusteredUp"]	= DEFAULT;
    choose["Events/PuppiMET_sumEt"]	= DEFAULT;
    choose["Events/event"]	= DEFAULT;
    choose["Events/fixedGridRhoFastjetAll"]	= DEFAULT;
    choose["Events/genTtbarId"]	= DEFAULT;
    choose["Events/genWeight"]	= DEFAULT;
    choose["Events/luminosityBlock"]	= DEFAULT;
    choose["Events/run"]	= DEFAULT;
    choose["Events/ttCatSource"]	= DEFAULT;
    choose["Events/ttCatXvalSource"]	= DEFAULT;
    choose["Events/ttCatXval_Add1Bjet_1Had"]	= DEFAULT;
    choose["Events/ttCatXval_Add1Bjet_2Had"]	= DEFAULT;
    choose["Events/ttCatXval_Add2Bjet"]	= DEFAULT;
    choose["Events/ttCatXval_AddCjet"]	= DEFAULT;
    choose["Events/ttCatXval_LightFlavour"]	= DEFAULT;
    choose["Events/ttCat_Add1Bjet_1Had"]	= DEFAULT;
    choose["Events/ttCat_Add1Bjet_2Had"]	= DEFAULT;
    choose["Events/ttCat_Add2Bjet"]	= DEFAULT;
    choose["Events/ttCat_AddCjet"]	= DEFAULT;
    choose["Events/ttCat_LightFlavour"]	= DEFAULT;

    if ( DEFAULT )
      {
        std::cout << std::endl
                  << "eventBuffer - All branches selected"
                  << std::endl;
      }
    else
      {
        std::cout << "eventBuffer - branches selected:"
                  << std::endl;      
        std::istringstream sin(varlist);
        while ( sin )
          {
            std::string key;
            sin >> key;
            if ( sin )
              {
		        std::map<std::string, bool>::iterator it;
		        for(it = choose.begin(); it != choose.end(); it++)
		          {
		            if ( it->first.length() > key.length() )
		              {
			            if ( it->first.substr(0, key.size()) == key )
			              {
			                choose[it->first] = true;
			              }
		              }
                  }
              }
          }
      }
    std::vector<std::string> successBranches;
    std::vector<std::string> missingBranches;
    if ( choose["Events/Electron_charge"] )
      if (input->present("Events/Electron_charge")) { Electron_charge.resize(23); input->select("Events/Electron_charge", Electron_charge); Electron_charge.clear(); successBranches.push_back("Events/Electron_charge"); } else { missingBranches.push_back("Events/Electron_charge"); }
    if ( choose["Events/Electron_cutBased"] )
      if (input->present("Events/Electron_cutBased")) { Electron_cutBased.resize(23); input->select("Events/Electron_cutBased", Electron_cutBased); Electron_cutBased.clear(); successBranches.push_back("Events/Electron_cutBased"); } else { missingBranches.push_back("Events/Electron_cutBased"); }
    if ( choose["Events/Electron_deltaEtaSC"] )
      if (input->present("Events/Electron_deltaEtaSC")) { Electron_deltaEtaSC.resize(23); input->select("Events/Electron_deltaEtaSC", Electron_deltaEtaSC); Electron_deltaEtaSC.clear(); successBranches.push_back("Events/Electron_deltaEtaSC"); } else { missingBranches.push_back("Events/Electron_deltaEtaSC"); }
    if ( choose["Events/Electron_dxy"] )
      if (input->present("Events/Electron_dxy")) { Electron_dxy.resize(23); input->select("Events/Electron_dxy", Electron_dxy); Electron_dxy.clear(); successBranches.push_back("Events/Electron_dxy"); } else { missingBranches.push_back("Events/Electron_dxy"); }
    if ( choose["Events/Electron_dz"] )
      if (input->present("Events/Electron_dz")) { Electron_dz.resize(23); input->select("Events/Electron_dz", Electron_dz); Electron_dz.clear(); successBranches.push_back("Events/Electron_dz"); } else { missingBranches.push_back("Events/Electron_dz"); }
    if ( choose["Events/Electron_eta"] )
      if (input->present("Events/Electron_eta")) { Electron_eta.resize(23); input->select("Events/Electron_eta", Electron_eta); Electron_eta.clear(); successBranches.push_back("Events/Electron_eta"); } else { missingBranches.push_back("Events/Electron_eta"); }
    if ( choose["Events/Electron_genPartIdx"] )
      if (input->present("Events/Electron_genPartIdx")) { Electron_genPartIdx.resize(23); input->select("Events/Electron_genPartIdx", Electron_genPartIdx); Electron_genPartIdx.clear(); successBranches.push_back("Events/Electron_genPartIdx"); } else { missingBranches.push_back("Events/Electron_genPartIdx"); }
    if ( choose["Events/Electron_mass"] )
      if (input->present("Events/Electron_mass")) { Electron_mass.resize(23); input->select("Events/Electron_mass", Electron_mass); Electron_mass.clear(); successBranches.push_back("Events/Electron_mass"); } else { missingBranches.push_back("Events/Electron_mass"); }
    if ( choose["Events/Electron_miniPFRelIso_all"] )
      if (input->present("Events/Electron_miniPFRelIso_all")) { Electron_miniPFRelIso_all.resize(23); input->select("Events/Electron_miniPFRelIso_all", Electron_miniPFRelIso_all); Electron_miniPFRelIso_all.clear(); successBranches.push_back("Events/Electron_miniPFRelIso_all"); } else { missingBranches.push_back("Events/Electron_miniPFRelIso_all"); }
    if ( choose["Events/Electron_mvaFall17V2Iso_WP80"] )
      if (input->present("Events/Electron_mvaFall17V2Iso_WP80")) { Electron_mvaFall17V2Iso_WP80.resize(23); input->select("Events/Electron_mvaFall17V2Iso_WP80", Electron_mvaFall17V2Iso_WP80); Electron_mvaFall17V2Iso_WP80.clear(); successBranches.push_back("Events/Electron_mvaFall17V2Iso_WP80"); } else { missingBranches.push_back("Events/Electron_mvaFall17V2Iso_WP80"); }
    if ( choose["Events/Electron_mvaFall17V2Iso_WP90"] )
      if (input->present("Events/Electron_mvaFall17V2Iso_WP90")) { Electron_mvaFall17V2Iso_WP90.resize(23); input->select("Events/Electron_mvaFall17V2Iso_WP90", Electron_mvaFall17V2Iso_WP90); Electron_mvaFall17V2Iso_WP90.clear(); successBranches.push_back("Events/Electron_mvaFall17V2Iso_WP90"); } else { missingBranches.push_back("Events/Electron_mvaFall17V2Iso_WP90"); }
    if ( choose["Events/Electron_mvaTTH"] )
      if (input->present("Events/Electron_mvaTTH")) { Electron_mvaTTH.resize(23); input->select("Events/Electron_mvaTTH", Electron_mvaTTH); Electron_mvaTTH.clear(); successBranches.push_back("Events/Electron_mvaTTH"); } else { missingBranches.push_back("Events/Electron_mvaTTH"); }
    if ( choose["Events/Electron_pfRelIso03_all"] )
      if (input->present("Events/Electron_pfRelIso03_all")) { Electron_pfRelIso03_all.resize(23); input->select("Events/Electron_pfRelIso03_all", Electron_pfRelIso03_all); Electron_pfRelIso03_all.clear(); successBranches.push_back("Events/Electron_pfRelIso03_all"); } else { missingBranches.push_back("Events/Electron_pfRelIso03_all"); }
    if ( choose["Events/Electron_phi"] )
      if (input->present("Events/Electron_phi")) { Electron_phi.resize(23); input->select("Events/Electron_phi", Electron_phi); Electron_phi.clear(); successBranches.push_back("Events/Electron_phi"); } else { missingBranches.push_back("Events/Electron_phi"); }
    if ( choose["Events/Electron_pt"] )
      if (input->present("Events/Electron_pt")) { Electron_pt.resize(23); input->select("Events/Electron_pt", Electron_pt); Electron_pt.clear(); successBranches.push_back("Events/Electron_pt"); } else { missingBranches.push_back("Events/Electron_pt"); }
    if ( choose["Events/FatJet_area"] )
      if (input->present("Events/FatJet_area")) { FatJet_area.resize(21); input->select("Events/FatJet_area", FatJet_area); FatJet_area.clear(); successBranches.push_back("Events/FatJet_area"); } else { missingBranches.push_back("Events/FatJet_area"); }
    if ( choose["Events/FatJet_deepTagMD_H4qvsQCD"] )
      if (input->present("Events/FatJet_deepTagMD_H4qvsQCD")) { FatJet_deepTagMD_H4qvsQCD.resize(21); input->select("Events/FatJet_deepTagMD_H4qvsQCD", FatJet_deepTagMD_H4qvsQCD); FatJet_deepTagMD_H4qvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTagMD_H4qvsQCD"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_H4qvsQCD"); }
    if ( choose["Events/FatJet_deepTagMD_HbbvsQCD"] )
      if (input->present("Events/FatJet_deepTagMD_HbbvsQCD")) { FatJet_deepTagMD_HbbvsQCD.resize(21); input->select("Events/FatJet_deepTagMD_HbbvsQCD", FatJet_deepTagMD_HbbvsQCD); FatJet_deepTagMD_HbbvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTagMD_HbbvsQCD"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_HbbvsQCD"); }
    if ( choose["Events/FatJet_deepTagMD_TvsQCD"] )
      if (input->present("Events/FatJet_deepTagMD_TvsQCD")) { FatJet_deepTagMD_TvsQCD.resize(21); input->select("Events/FatJet_deepTagMD_TvsQCD", FatJet_deepTagMD_TvsQCD); FatJet_deepTagMD_TvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTagMD_TvsQCD"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_TvsQCD"); }
    if ( choose["Events/FatJet_deepTagMD_WvsQCD"] )
      if (input->present("Events/FatJet_deepTagMD_WvsQCD")) { FatJet_deepTagMD_WvsQCD.resize(21); input->select("Events/FatJet_deepTagMD_WvsQCD", FatJet_deepTagMD_WvsQCD); FatJet_deepTagMD_WvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTagMD_WvsQCD"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_WvsQCD"); }
    if ( choose["Events/FatJet_deepTagMD_ZHbbvsQCD"] )
      if (input->present("Events/FatJet_deepTagMD_ZHbbvsQCD")) { FatJet_deepTagMD_ZHbbvsQCD.resize(21); input->select("Events/FatJet_deepTagMD_ZHbbvsQCD", FatJet_deepTagMD_ZHbbvsQCD); FatJet_deepTagMD_ZHbbvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTagMD_ZHbbvsQCD"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_ZHbbvsQCD"); }
    if ( choose["Events/FatJet_deepTagMD_ZHccvsQCD"] )
      if (input->present("Events/FatJet_deepTagMD_ZHccvsQCD")) { FatJet_deepTagMD_ZHccvsQCD.resize(21); input->select("Events/FatJet_deepTagMD_ZHccvsQCD", FatJet_deepTagMD_ZHccvsQCD); FatJet_deepTagMD_ZHccvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTagMD_ZHccvsQCD"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_ZHccvsQCD"); }
    if ( choose["Events/FatJet_deepTagMD_ZbbvsQCD"] )
      if (input->present("Events/FatJet_deepTagMD_ZbbvsQCD")) { FatJet_deepTagMD_ZbbvsQCD.resize(21); input->select("Events/FatJet_deepTagMD_ZbbvsQCD", FatJet_deepTagMD_ZbbvsQCD); FatJet_deepTagMD_ZbbvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTagMD_ZbbvsQCD"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_ZbbvsQCD"); }
    if ( choose["Events/FatJet_deepTagMD_ZvsQCD"] )
      if (input->present("Events/FatJet_deepTagMD_ZvsQCD")) { FatJet_deepTagMD_ZvsQCD.resize(21); input->select("Events/FatJet_deepTagMD_ZvsQCD", FatJet_deepTagMD_ZvsQCD); FatJet_deepTagMD_ZvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTagMD_ZvsQCD"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_ZvsQCD"); }
    if ( choose["Events/FatJet_deepTagMD_bbvsLight"] )
      if (input->present("Events/FatJet_deepTagMD_bbvsLight")) { FatJet_deepTagMD_bbvsLight.resize(21); input->select("Events/FatJet_deepTagMD_bbvsLight", FatJet_deepTagMD_bbvsLight); FatJet_deepTagMD_bbvsLight.clear(); successBranches.push_back("Events/FatJet_deepTagMD_bbvsLight"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_bbvsLight"); }
    if ( choose["Events/FatJet_deepTagMD_ccvsLight"] )
      if (input->present("Events/FatJet_deepTagMD_ccvsLight")) { FatJet_deepTagMD_ccvsLight.resize(21); input->select("Events/FatJet_deepTagMD_ccvsLight", FatJet_deepTagMD_ccvsLight); FatJet_deepTagMD_ccvsLight.clear(); successBranches.push_back("Events/FatJet_deepTagMD_ccvsLight"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_ccvsLight"); }
    if ( choose["Events/FatJet_deepTag_H"] )
      if (input->present("Events/FatJet_deepTag_H")) { FatJet_deepTag_H.resize(21); input->select("Events/FatJet_deepTag_H", FatJet_deepTag_H); FatJet_deepTag_H.clear(); successBranches.push_back("Events/FatJet_deepTag_H"); } else { missingBranches.push_back("Events/FatJet_deepTag_H"); }
    if ( choose["Events/FatJet_deepTag_QCD"] )
      if (input->present("Events/FatJet_deepTag_QCD")) { FatJet_deepTag_QCD.resize(21); input->select("Events/FatJet_deepTag_QCD", FatJet_deepTag_QCD); FatJet_deepTag_QCD.clear(); successBranches.push_back("Events/FatJet_deepTag_QCD"); } else { missingBranches.push_back("Events/FatJet_deepTag_QCD"); }
    if ( choose["Events/FatJet_deepTag_QCDothers"] )
      if (input->present("Events/FatJet_deepTag_QCDothers")) { FatJet_deepTag_QCDothers.resize(21); input->select("Events/FatJet_deepTag_QCDothers", FatJet_deepTag_QCDothers); FatJet_deepTag_QCDothers.clear(); successBranches.push_back("Events/FatJet_deepTag_QCDothers"); } else { missingBranches.push_back("Events/FatJet_deepTag_QCDothers"); }
    if ( choose["Events/FatJet_deepTag_TvsQCD"] )
      if (input->present("Events/FatJet_deepTag_TvsQCD")) { FatJet_deepTag_TvsQCD.resize(21); input->select("Events/FatJet_deepTag_TvsQCD", FatJet_deepTag_TvsQCD); FatJet_deepTag_TvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTag_TvsQCD"); } else { missingBranches.push_back("Events/FatJet_deepTag_TvsQCD"); }
    if ( choose["Events/FatJet_deepTag_WvsQCD"] )
      if (input->present("Events/FatJet_deepTag_WvsQCD")) { FatJet_deepTag_WvsQCD.resize(21); input->select("Events/FatJet_deepTag_WvsQCD", FatJet_deepTag_WvsQCD); FatJet_deepTag_WvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTag_WvsQCD"); } else { missingBranches.push_back("Events/FatJet_deepTag_WvsQCD"); }
    if ( choose["Events/FatJet_deepTag_ZvsQCD"] )
      if (input->present("Events/FatJet_deepTag_ZvsQCD")) { FatJet_deepTag_ZvsQCD.resize(21); input->select("Events/FatJet_deepTag_ZvsQCD", FatJet_deepTag_ZvsQCD); FatJet_deepTag_ZvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTag_ZvsQCD"); } else { missingBranches.push_back("Events/FatJet_deepTag_ZvsQCD"); }
    if ( choose["Events/FatJet_eta"] )
      if (input->present("Events/FatJet_eta")) { FatJet_eta.resize(21); input->select("Events/FatJet_eta", FatJet_eta); FatJet_eta.clear(); successBranches.push_back("Events/FatJet_eta"); } else { missingBranches.push_back("Events/FatJet_eta"); }
    if ( choose["Events/FatJet_jetId"] )
      if (input->present("Events/FatJet_jetId")) { FatJet_jetId.resize(21); input->select("Events/FatJet_jetId", FatJet_jetId); FatJet_jetId.clear(); successBranches.push_back("Events/FatJet_jetId"); } else { missingBranches.push_back("Events/FatJet_jetId"); }
    if ( choose["Events/FatJet_mass"] )
      if (input->present("Events/FatJet_mass")) { FatJet_mass.resize(21); input->select("Events/FatJet_mass", FatJet_mass); FatJet_mass.clear(); successBranches.push_back("Events/FatJet_mass"); } else { missingBranches.push_back("Events/FatJet_mass"); }
    if ( choose["Events/FatJet_msoftdrop"] )
      if (input->present("Events/FatJet_msoftdrop")) { FatJet_msoftdrop.resize(21); input->select("Events/FatJet_msoftdrop", FatJet_msoftdrop); FatJet_msoftdrop.clear(); successBranches.push_back("Events/FatJet_msoftdrop"); } else { missingBranches.push_back("Events/FatJet_msoftdrop"); }
    if ( choose["Events/FatJet_n2b1"] )
      if (input->present("Events/FatJet_n2b1")) { FatJet_n2b1.resize(21); input->select("Events/FatJet_n2b1", FatJet_n2b1); FatJet_n2b1.clear(); successBranches.push_back("Events/FatJet_n2b1"); } else { missingBranches.push_back("Events/FatJet_n2b1"); }
    if ( choose["Events/FatJet_n3b1"] )
      if (input->present("Events/FatJet_n3b1")) { FatJet_n3b1.resize(21); input->select("Events/FatJet_n3b1", FatJet_n3b1); FatJet_n3b1.clear(); successBranches.push_back("Events/FatJet_n3b1"); } else { missingBranches.push_back("Events/FatJet_n3b1"); }
    if ( choose["Events/FatJet_particleNetMD_QCD"] )
      if (input->present("Events/FatJet_particleNetMD_QCD")) { FatJet_particleNetMD_QCD.resize(21); input->select("Events/FatJet_particleNetMD_QCD", FatJet_particleNetMD_QCD); FatJet_particleNetMD_QCD.clear(); successBranches.push_back("Events/FatJet_particleNetMD_QCD"); } else { missingBranches.push_back("Events/FatJet_particleNetMD_QCD"); }
    if ( choose["Events/FatJet_particleNetMD_Xbb"] )
      if (input->present("Events/FatJet_particleNetMD_Xbb")) { FatJet_particleNetMD_Xbb.resize(21); input->select("Events/FatJet_particleNetMD_Xbb", FatJet_particleNetMD_Xbb); FatJet_particleNetMD_Xbb.clear(); successBranches.push_back("Events/FatJet_particleNetMD_Xbb"); } else { missingBranches.push_back("Events/FatJet_particleNetMD_Xbb"); }
    if ( choose["Events/FatJet_particleNetMD_Xcc"] )
      if (input->present("Events/FatJet_particleNetMD_Xcc")) { FatJet_particleNetMD_Xcc.resize(21); input->select("Events/FatJet_particleNetMD_Xcc", FatJet_particleNetMD_Xcc); FatJet_particleNetMD_Xcc.clear(); successBranches.push_back("Events/FatJet_particleNetMD_Xcc"); } else { missingBranches.push_back("Events/FatJet_particleNetMD_Xcc"); }
    if ( choose["Events/FatJet_particleNetMD_Xqq"] )
      if (input->present("Events/FatJet_particleNetMD_Xqq")) { FatJet_particleNetMD_Xqq.resize(21); input->select("Events/FatJet_particleNetMD_Xqq", FatJet_particleNetMD_Xqq); FatJet_particleNetMD_Xqq.clear(); successBranches.push_back("Events/FatJet_particleNetMD_Xqq"); } else { missingBranches.push_back("Events/FatJet_particleNetMD_Xqq"); }
    if ( choose["Events/FatJet_particleNet_HbbvsQCD"] )
      if (input->present("Events/FatJet_particleNet_HbbvsQCD")) { FatJet_particleNet_HbbvsQCD.resize(21); input->select("Events/FatJet_particleNet_HbbvsQCD", FatJet_particleNet_HbbvsQCD); FatJet_particleNet_HbbvsQCD.clear(); successBranches.push_back("Events/FatJet_particleNet_HbbvsQCD"); } else { missingBranches.push_back("Events/FatJet_particleNet_HbbvsQCD"); }
    if ( choose["Events/FatJet_particleNet_TvsQCD"] )
      if (input->present("Events/FatJet_particleNet_TvsQCD")) { FatJet_particleNet_TvsQCD.resize(21); input->select("Events/FatJet_particleNet_TvsQCD", FatJet_particleNet_TvsQCD); FatJet_particleNet_TvsQCD.clear(); successBranches.push_back("Events/FatJet_particleNet_TvsQCD"); } else { missingBranches.push_back("Events/FatJet_particleNet_TvsQCD"); }
    if ( choose["Events/FatJet_particleNet_WvsQCD"] )
      if (input->present("Events/FatJet_particleNet_WvsQCD")) { FatJet_particleNet_WvsQCD.resize(21); input->select("Events/FatJet_particleNet_WvsQCD", FatJet_particleNet_WvsQCD); FatJet_particleNet_WvsQCD.clear(); successBranches.push_back("Events/FatJet_particleNet_WvsQCD"); } else { missingBranches.push_back("Events/FatJet_particleNet_WvsQCD"); }
    if ( choose["Events/FatJet_particleNet_ZvsQCD"] )
      if (input->present("Events/FatJet_particleNet_ZvsQCD")) { FatJet_particleNet_ZvsQCD.resize(21); input->select("Events/FatJet_particleNet_ZvsQCD", FatJet_particleNet_ZvsQCD); FatJet_particleNet_ZvsQCD.clear(); successBranches.push_back("Events/FatJet_particleNet_ZvsQCD"); } else { missingBranches.push_back("Events/FatJet_particleNet_ZvsQCD"); }
    if ( choose["Events/FatJet_particleNet_mass"] )
      if (input->present("Events/FatJet_particleNet_mass")) { FatJet_particleNet_mass.resize(21); input->select("Events/FatJet_particleNet_mass", FatJet_particleNet_mass); FatJet_particleNet_mass.clear(); successBranches.push_back("Events/FatJet_particleNet_mass"); } else { missingBranches.push_back("Events/FatJet_particleNet_mass"); }
    if ( choose["Events/FatJet_phi"] )
      if (input->present("Events/FatJet_phi")) { FatJet_phi.resize(21); input->select("Events/FatJet_phi", FatJet_phi); FatJet_phi.clear(); successBranches.push_back("Events/FatJet_phi"); } else { missingBranches.push_back("Events/FatJet_phi"); }
    if ( choose["Events/FatJet_pt"] )
      if (input->present("Events/FatJet_pt")) { FatJet_pt.resize(21); input->select("Events/FatJet_pt", FatJet_pt); FatJet_pt.clear(); successBranches.push_back("Events/FatJet_pt"); } else { missingBranches.push_back("Events/FatJet_pt"); }
    if ( choose["Events/FatJet_rawFactor"] )
      if (input->present("Events/FatJet_rawFactor")) { FatJet_rawFactor.resize(21); input->select("Events/FatJet_rawFactor", FatJet_rawFactor); FatJet_rawFactor.clear(); successBranches.push_back("Events/FatJet_rawFactor"); } else { missingBranches.push_back("Events/FatJet_rawFactor"); }
    if ( choose["Events/FatJet_tau1"] )
      if (input->present("Events/FatJet_tau1")) { FatJet_tau1.resize(21); input->select("Events/FatJet_tau1", FatJet_tau1); FatJet_tau1.clear(); successBranches.push_back("Events/FatJet_tau1"); } else { missingBranches.push_back("Events/FatJet_tau1"); }
    if ( choose["Events/FatJet_tau2"] )
      if (input->present("Events/FatJet_tau2")) { FatJet_tau2.resize(21); input->select("Events/FatJet_tau2", FatJet_tau2); FatJet_tau2.clear(); successBranches.push_back("Events/FatJet_tau2"); } else { missingBranches.push_back("Events/FatJet_tau2"); }
    if ( choose["Events/FatJet_tau3"] )
      if (input->present("Events/FatJet_tau3")) { FatJet_tau3.resize(21); input->select("Events/FatJet_tau3", FatJet_tau3); FatJet_tau3.clear(); successBranches.push_back("Events/FatJet_tau3"); } else { missingBranches.push_back("Events/FatJet_tau3"); }
    if ( choose["Events/FatJet_tau4"] )
      if (input->present("Events/FatJet_tau4")) { FatJet_tau4.resize(21); input->select("Events/FatJet_tau4", FatJet_tau4); FatJet_tau4.clear(); successBranches.push_back("Events/FatJet_tau4"); } else { missingBranches.push_back("Events/FatJet_tau4"); }
    if ( choose["Events/Flag_BadPFMuonDzFilter"] )
      if (input->present("Events/Flag_BadPFMuonDzFilter")) { input->select("Events/Flag_BadPFMuonDzFilter", Flag_BadPFMuonDzFilter); successBranches.push_back("Events/Flag_BadPFMuonDzFilter"); } else { missingBranches.push_back("Events/Flag_BadPFMuonDzFilter"); }
    if ( choose["Events/Flag_BadPFMuonFilter"] )
      if (input->present("Events/Flag_BadPFMuonFilter")) { input->select("Events/Flag_BadPFMuonFilter", Flag_BadPFMuonFilter); successBranches.push_back("Events/Flag_BadPFMuonFilter"); } else { missingBranches.push_back("Events/Flag_BadPFMuonFilter"); }
    if ( choose["Events/Flag_EcalDeadCellTriggerPrimitiveFilter"] )
      if (input->present("Events/Flag_EcalDeadCellTriggerPrimitiveFilter")) { input->select("Events/Flag_EcalDeadCellTriggerPrimitiveFilter", Flag_EcalDeadCellTriggerPrimitiveFilter); successBranches.push_back("Events/Flag_EcalDeadCellTriggerPrimitiveFilter"); } else { missingBranches.push_back("Events/Flag_EcalDeadCellTriggerPrimitiveFilter"); }
    if ( choose["Events/Flag_HBHENoiseFilter"] )
      if (input->present("Events/Flag_HBHENoiseFilter")) { input->select("Events/Flag_HBHENoiseFilter", Flag_HBHENoiseFilter); successBranches.push_back("Events/Flag_HBHENoiseFilter"); } else { missingBranches.push_back("Events/Flag_HBHENoiseFilter"); }
    if ( choose["Events/Flag_HBHENoiseIsoFilter"] )
      if (input->present("Events/Flag_HBHENoiseIsoFilter")) { input->select("Events/Flag_HBHENoiseIsoFilter", Flag_HBHENoiseIsoFilter); successBranches.push_back("Events/Flag_HBHENoiseIsoFilter"); } else { missingBranches.push_back("Events/Flag_HBHENoiseIsoFilter"); }
    if ( choose["Events/Flag_ecalBadCalibFilter"] )
      if (input->present("Events/Flag_ecalBadCalibFilter")) { input->select("Events/Flag_ecalBadCalibFilter", Flag_ecalBadCalibFilter); successBranches.push_back("Events/Flag_ecalBadCalibFilter"); } else { missingBranches.push_back("Events/Flag_ecalBadCalibFilter"); }
    if ( choose["Events/Flag_eeBadScFilter"] )
      if (input->present("Events/Flag_eeBadScFilter")) { input->select("Events/Flag_eeBadScFilter", Flag_eeBadScFilter); successBranches.push_back("Events/Flag_eeBadScFilter"); } else { missingBranches.push_back("Events/Flag_eeBadScFilter"); }
    if ( choose["Events/Flag_globalSuperTightHalo2016Filter"] )
      if (input->present("Events/Flag_globalSuperTightHalo2016Filter")) { input->select("Events/Flag_globalSuperTightHalo2016Filter", Flag_globalSuperTightHalo2016Filter); successBranches.push_back("Events/Flag_globalSuperTightHalo2016Filter"); } else { missingBranches.push_back("Events/Flag_globalSuperTightHalo2016Filter"); }
    if ( choose["Events/Flag_goodVertices"] )
      if (input->present("Events/Flag_goodVertices")) { input->select("Events/Flag_goodVertices", Flag_goodVertices); successBranches.push_back("Events/Flag_goodVertices"); } else { missingBranches.push_back("Events/Flag_goodVertices"); }
    if ( choose["Events/GenJet_eta"] )
      if (input->present("Events/GenJet_eta")) { GenJet_eta.resize(50); input->select("Events/GenJet_eta", GenJet_eta); GenJet_eta.clear(); successBranches.push_back("Events/GenJet_eta"); } else { missingBranches.push_back("Events/GenJet_eta"); }
    if ( choose["Events/GenJet_hadronFlavour"] )
      if (input->present("Events/GenJet_hadronFlavour")) { GenJet_hadronFlavour.resize(50); input->select("Events/GenJet_hadronFlavour", GenJet_hadronFlavour); GenJet_hadronFlavour.clear(); successBranches.push_back("Events/GenJet_hadronFlavour"); } else { missingBranches.push_back("Events/GenJet_hadronFlavour"); }
    if ( choose["Events/GenJet_mass"] )
      if (input->present("Events/GenJet_mass")) { GenJet_mass.resize(50); input->select("Events/GenJet_mass", GenJet_mass); GenJet_mass.clear(); successBranches.push_back("Events/GenJet_mass"); } else { missingBranches.push_back("Events/GenJet_mass"); }
    if ( choose["Events/GenJet_partonFlavour"] )
      if (input->present("Events/GenJet_partonFlavour")) { GenJet_partonFlavour.resize(50); input->select("Events/GenJet_partonFlavour", GenJet_partonFlavour); GenJet_partonFlavour.clear(); successBranches.push_back("Events/GenJet_partonFlavour"); } else { missingBranches.push_back("Events/GenJet_partonFlavour"); }
    if ( choose["Events/GenJet_phi"] )
      if (input->present("Events/GenJet_phi")) { GenJet_phi.resize(50); input->select("Events/GenJet_phi", GenJet_phi); GenJet_phi.clear(); successBranches.push_back("Events/GenJet_phi"); } else { missingBranches.push_back("Events/GenJet_phi"); }
    if ( choose["Events/GenJet_pt"] )
      if (input->present("Events/GenJet_pt")) { GenJet_pt.resize(50); input->select("Events/GenJet_pt", GenJet_pt); GenJet_pt.clear(); successBranches.push_back("Events/GenJet_pt"); } else { missingBranches.push_back("Events/GenJet_pt"); }
    if ( choose["Events/GenPart_eta"] )
      if (input->present("Events/GenPart_eta")) { GenPart_eta.resize(233); input->select("Events/GenPart_eta", GenPart_eta); GenPart_eta.clear(); successBranches.push_back("Events/GenPart_eta"); } else { missingBranches.push_back("Events/GenPart_eta"); }
    if ( choose["Events/GenPart_genPartIdxMother"] )
      if (input->present("Events/GenPart_genPartIdxMother")) { GenPart_genPartIdxMother.resize(233); input->select("Events/GenPart_genPartIdxMother", GenPart_genPartIdxMother); GenPart_genPartIdxMother.clear(); successBranches.push_back("Events/GenPart_genPartIdxMother"); } else { missingBranches.push_back("Events/GenPart_genPartIdxMother"); }
    if ( choose["Events/GenPart_mass"] )
      if (input->present("Events/GenPart_mass")) { GenPart_mass.resize(233); input->select("Events/GenPart_mass", GenPart_mass); GenPart_mass.clear(); successBranches.push_back("Events/GenPart_mass"); } else { missingBranches.push_back("Events/GenPart_mass"); }
    if ( choose["Events/GenPart_pdgId"] )
      if (input->present("Events/GenPart_pdgId")) { GenPart_pdgId.resize(233); input->select("Events/GenPart_pdgId", GenPart_pdgId); GenPart_pdgId.clear(); successBranches.push_back("Events/GenPart_pdgId"); } else { missingBranches.push_back("Events/GenPart_pdgId"); }
    if ( choose["Events/GenPart_phi"] )
      if (input->present("Events/GenPart_phi")) { GenPart_phi.resize(233); input->select("Events/GenPart_phi", GenPart_phi); GenPart_phi.clear(); successBranches.push_back("Events/GenPart_phi"); } else { missingBranches.push_back("Events/GenPart_phi"); }
    if ( choose["Events/GenPart_pt"] )
      if (input->present("Events/GenPart_pt")) { GenPart_pt.resize(233); input->select("Events/GenPart_pt", GenPart_pt); GenPart_pt.clear(); successBranches.push_back("Events/GenPart_pt"); } else { missingBranches.push_back("Events/GenPart_pt"); }
    if ( choose["Events/GenPart_status"] )
      if (input->present("Events/GenPart_status")) { GenPart_status.resize(233); input->select("Events/GenPart_status", GenPart_status); GenPart_status.clear(); successBranches.push_back("Events/GenPart_status"); } else { missingBranches.push_back("Events/GenPart_status"); }
    if ( choose["Events/GenPart_statusFlags"] )
      if (input->present("Events/GenPart_statusFlags")) { GenPart_statusFlags.resize(233); input->select("Events/GenPart_statusFlags", GenPart_statusFlags); GenPart_statusFlags.clear(); successBranches.push_back("Events/GenPart_statusFlags"); } else { missingBranches.push_back("Events/GenPart_statusFlags"); }
    if ( choose["Events/Generator_weight"] )
      if (input->present("Events/Generator_weight")) { input->select("Events/Generator_weight", Generator_weight); successBranches.push_back("Events/Generator_weight"); } else { missingBranches.push_back("Events/Generator_weight"); }
    if ( choose["Events/HLT_AK8PFHT750_TrimMass50"] )
      if (input->present("Events/HLT_AK8PFHT750_TrimMass50")) { input->select("Events/HLT_AK8PFHT750_TrimMass50", HLT_AK8PFHT750_TrimMass50); successBranches.push_back("Events/HLT_AK8PFHT750_TrimMass50"); } else { missingBranches.push_back("Events/HLT_AK8PFHT750_TrimMass50"); }
    if ( choose["Events/HLT_AK8PFHT800_TrimMass50"] )
      if (input->present("Events/HLT_AK8PFHT800_TrimMass50")) { input->select("Events/HLT_AK8PFHT800_TrimMass50", HLT_AK8PFHT800_TrimMass50); successBranches.push_back("Events/HLT_AK8PFHT800_TrimMass50"); } else { missingBranches.push_back("Events/HLT_AK8PFHT800_TrimMass50"); }
    if ( choose["Events/HLT_AK8PFHT850_TrimMass50"] )
      if (input->present("Events/HLT_AK8PFHT850_TrimMass50")) { input->select("Events/HLT_AK8PFHT850_TrimMass50", HLT_AK8PFHT850_TrimMass50); successBranches.push_back("Events/HLT_AK8PFHT850_TrimMass50"); } else { missingBranches.push_back("Events/HLT_AK8PFHT850_TrimMass50"); }
    if ( choose["Events/HLT_AK8PFHT900_TrimMass50"] )
      if (input->present("Events/HLT_AK8PFHT900_TrimMass50")) { input->select("Events/HLT_AK8PFHT900_TrimMass50", HLT_AK8PFHT900_TrimMass50); successBranches.push_back("Events/HLT_AK8PFHT900_TrimMass50"); } else { missingBranches.push_back("Events/HLT_AK8PFHT900_TrimMass50"); }
    if ( choose["Events/HLT_AK8PFJet140"] )
      if (input->present("Events/HLT_AK8PFJet140")) { input->select("Events/HLT_AK8PFJet140", HLT_AK8PFJet140); successBranches.push_back("Events/HLT_AK8PFJet140"); } else { missingBranches.push_back("Events/HLT_AK8PFJet140"); }
    if ( choose["Events/HLT_AK8PFJet200"] )
      if (input->present("Events/HLT_AK8PFJet200")) { input->select("Events/HLT_AK8PFJet200", HLT_AK8PFJet200); successBranches.push_back("Events/HLT_AK8PFJet200"); } else { missingBranches.push_back("Events/HLT_AK8PFJet200"); }
    if ( choose["Events/HLT_AK8PFJet260"] )
      if (input->present("Events/HLT_AK8PFJet260")) { input->select("Events/HLT_AK8PFJet260", HLT_AK8PFJet260); successBranches.push_back("Events/HLT_AK8PFJet260"); } else { missingBranches.push_back("Events/HLT_AK8PFJet260"); }
    if ( choose["Events/HLT_AK8PFJet320"] )
      if (input->present("Events/HLT_AK8PFJet320")) { input->select("Events/HLT_AK8PFJet320", HLT_AK8PFJet320); successBranches.push_back("Events/HLT_AK8PFJet320"); } else { missingBranches.push_back("Events/HLT_AK8PFJet320"); }
    if ( choose["Events/HLT_AK8PFJet330_PFAK8BTagCSV_p1"] )
      if (input->present("Events/HLT_AK8PFJet330_PFAK8BTagCSV_p1")) { input->select("Events/HLT_AK8PFJet330_PFAK8BTagCSV_p1", HLT_AK8PFJet330_PFAK8BTagCSV_p1); successBranches.push_back("Events/HLT_AK8PFJet330_PFAK8BTagCSV_p1"); } else { missingBranches.push_back("Events/HLT_AK8PFJet330_PFAK8BTagCSV_p1"); }
    if ( choose["Events/HLT_AK8PFJet330_PFAK8BTagCSV_p17"] )
      if (input->present("Events/HLT_AK8PFJet330_PFAK8BTagCSV_p17")) { input->select("Events/HLT_AK8PFJet330_PFAK8BTagCSV_p17", HLT_AK8PFJet330_PFAK8BTagCSV_p17); successBranches.push_back("Events/HLT_AK8PFJet330_PFAK8BTagCSV_p17"); } else { missingBranches.push_back("Events/HLT_AK8PFJet330_PFAK8BTagCSV_p17"); }
    if ( choose["Events/HLT_AK8PFJet360_TrimMass30"] )
      if (input->present("Events/HLT_AK8PFJet360_TrimMass30")) { input->select("Events/HLT_AK8PFJet360_TrimMass30", HLT_AK8PFJet360_TrimMass30); successBranches.push_back("Events/HLT_AK8PFJet360_TrimMass30"); } else { missingBranches.push_back("Events/HLT_AK8PFJet360_TrimMass30"); }
    if ( choose["Events/HLT_AK8PFJet380_TrimMass30"] )
      if (input->present("Events/HLT_AK8PFJet380_TrimMass30")) { input->select("Events/HLT_AK8PFJet380_TrimMass30", HLT_AK8PFJet380_TrimMass30); successBranches.push_back("Events/HLT_AK8PFJet380_TrimMass30"); } else { missingBranches.push_back("Events/HLT_AK8PFJet380_TrimMass30"); }
    if ( choose["Events/HLT_AK8PFJet40"] )
      if (input->present("Events/HLT_AK8PFJet40")) { input->select("Events/HLT_AK8PFJet40", HLT_AK8PFJet40); successBranches.push_back("Events/HLT_AK8PFJet40"); } else { missingBranches.push_back("Events/HLT_AK8PFJet40"); }
    if ( choose["Events/HLT_AK8PFJet400"] )
      if (input->present("Events/HLT_AK8PFJet400")) { input->select("Events/HLT_AK8PFJet400", HLT_AK8PFJet400); successBranches.push_back("Events/HLT_AK8PFJet400"); } else { missingBranches.push_back("Events/HLT_AK8PFJet400"); }
    if ( choose["Events/HLT_AK8PFJet400_TrimMass30"] )
      if (input->present("Events/HLT_AK8PFJet400_TrimMass30")) { input->select("Events/HLT_AK8PFJet400_TrimMass30", HLT_AK8PFJet400_TrimMass30); successBranches.push_back("Events/HLT_AK8PFJet400_TrimMass30"); } else { missingBranches.push_back("Events/HLT_AK8PFJet400_TrimMass30"); }
    if ( choose["Events/HLT_AK8PFJet420_TrimMass30"] )
      if (input->present("Events/HLT_AK8PFJet420_TrimMass30")) { input->select("Events/HLT_AK8PFJet420_TrimMass30", HLT_AK8PFJet420_TrimMass30); successBranches.push_back("Events/HLT_AK8PFJet420_TrimMass30"); } else { missingBranches.push_back("Events/HLT_AK8PFJet420_TrimMass30"); }
    if ( choose["Events/HLT_AK8PFJet450"] )
      if (input->present("Events/HLT_AK8PFJet450")) { input->select("Events/HLT_AK8PFJet450", HLT_AK8PFJet450); successBranches.push_back("Events/HLT_AK8PFJet450"); } else { missingBranches.push_back("Events/HLT_AK8PFJet450"); }
    if ( choose["Events/HLT_AK8PFJet500"] )
      if (input->present("Events/HLT_AK8PFJet500")) { input->select("Events/HLT_AK8PFJet500", HLT_AK8PFJet500); successBranches.push_back("Events/HLT_AK8PFJet500"); } else { missingBranches.push_back("Events/HLT_AK8PFJet500"); }
    if ( choose["Events/HLT_AK8PFJet550"] )
      if (input->present("Events/HLT_AK8PFJet550")) { input->select("Events/HLT_AK8PFJet550", HLT_AK8PFJet550); successBranches.push_back("Events/HLT_AK8PFJet550"); } else { missingBranches.push_back("Events/HLT_AK8PFJet550"); }
    if ( choose["Events/HLT_AK8PFJet60"] )
      if (input->present("Events/HLT_AK8PFJet60")) { input->select("Events/HLT_AK8PFJet60", HLT_AK8PFJet60); successBranches.push_back("Events/HLT_AK8PFJet60"); } else { missingBranches.push_back("Events/HLT_AK8PFJet60"); }
    if ( choose["Events/HLT_AK8PFJet80"] )
      if (input->present("Events/HLT_AK8PFJet80")) { input->select("Events/HLT_AK8PFJet80", HLT_AK8PFJet80); successBranches.push_back("Events/HLT_AK8PFJet80"); } else { missingBranches.push_back("Events/HLT_AK8PFJet80"); }
    if ( choose["Events/HLT_AK8PFJetFwd140"] )
      if (input->present("Events/HLT_AK8PFJetFwd140")) { input->select("Events/HLT_AK8PFJetFwd140", HLT_AK8PFJetFwd140); successBranches.push_back("Events/HLT_AK8PFJetFwd140"); } else { missingBranches.push_back("Events/HLT_AK8PFJetFwd140"); }
    if ( choose["Events/HLT_AK8PFJetFwd200"] )
      if (input->present("Events/HLT_AK8PFJetFwd200")) { input->select("Events/HLT_AK8PFJetFwd200", HLT_AK8PFJetFwd200); successBranches.push_back("Events/HLT_AK8PFJetFwd200"); } else { missingBranches.push_back("Events/HLT_AK8PFJetFwd200"); }
    if ( choose["Events/HLT_AK8PFJetFwd260"] )
      if (input->present("Events/HLT_AK8PFJetFwd260")) { input->select("Events/HLT_AK8PFJetFwd260", HLT_AK8PFJetFwd260); successBranches.push_back("Events/HLT_AK8PFJetFwd260"); } else { missingBranches.push_back("Events/HLT_AK8PFJetFwd260"); }
    if ( choose["Events/HLT_AK8PFJetFwd320"] )
      if (input->present("Events/HLT_AK8PFJetFwd320")) { input->select("Events/HLT_AK8PFJetFwd320", HLT_AK8PFJetFwd320); successBranches.push_back("Events/HLT_AK8PFJetFwd320"); } else { missingBranches.push_back("Events/HLT_AK8PFJetFwd320"); }
    if ( choose["Events/HLT_AK8PFJetFwd40"] )
      if (input->present("Events/HLT_AK8PFJetFwd40")) { input->select("Events/HLT_AK8PFJetFwd40", HLT_AK8PFJetFwd40); successBranches.push_back("Events/HLT_AK8PFJetFwd40"); } else { missingBranches.push_back("Events/HLT_AK8PFJetFwd40"); }
    if ( choose["Events/HLT_AK8PFJetFwd400"] )
      if (input->present("Events/HLT_AK8PFJetFwd400")) { input->select("Events/HLT_AK8PFJetFwd400", HLT_AK8PFJetFwd400); successBranches.push_back("Events/HLT_AK8PFJetFwd400"); } else { missingBranches.push_back("Events/HLT_AK8PFJetFwd400"); }
    if ( choose["Events/HLT_AK8PFJetFwd450"] )
      if (input->present("Events/HLT_AK8PFJetFwd450")) { input->select("Events/HLT_AK8PFJetFwd450", HLT_AK8PFJetFwd450); successBranches.push_back("Events/HLT_AK8PFJetFwd450"); } else { missingBranches.push_back("Events/HLT_AK8PFJetFwd450"); }
    if ( choose["Events/HLT_AK8PFJetFwd500"] )
      if (input->present("Events/HLT_AK8PFJetFwd500")) { input->select("Events/HLT_AK8PFJetFwd500", HLT_AK8PFJetFwd500); successBranches.push_back("Events/HLT_AK8PFJetFwd500"); } else { missingBranches.push_back("Events/HLT_AK8PFJetFwd500"); }
    if ( choose["Events/HLT_AK8PFJetFwd60"] )
      if (input->present("Events/HLT_AK8PFJetFwd60")) { input->select("Events/HLT_AK8PFJetFwd60", HLT_AK8PFJetFwd60); successBranches.push_back("Events/HLT_AK8PFJetFwd60"); } else { missingBranches.push_back("Events/HLT_AK8PFJetFwd60"); }
    if ( choose["Events/HLT_AK8PFJetFwd80"] )
      if (input->present("Events/HLT_AK8PFJetFwd80")) { input->select("Events/HLT_AK8PFJetFwd80", HLT_AK8PFJetFwd80); successBranches.push_back("Events/HLT_AK8PFJetFwd80"); } else { missingBranches.push_back("Events/HLT_AK8PFJetFwd80"); }
    if ( choose["Events/HLT_DiPFJet15_FBEta3_NoCaloMatched"] )
      if (input->present("Events/HLT_DiPFJet15_FBEta3_NoCaloMatched")) { input->select("Events/HLT_DiPFJet15_FBEta3_NoCaloMatched", HLT_DiPFJet15_FBEta3_NoCaloMatched); successBranches.push_back("Events/HLT_DiPFJet15_FBEta3_NoCaloMatched"); } else { missingBranches.push_back("Events/HLT_DiPFJet15_FBEta3_NoCaloMatched"); }
    if ( choose["Events/HLT_DiPFJet15_NoCaloMatched"] )
      if (input->present("Events/HLT_DiPFJet15_NoCaloMatched")) { input->select("Events/HLT_DiPFJet15_NoCaloMatched", HLT_DiPFJet15_NoCaloMatched); successBranches.push_back("Events/HLT_DiPFJet15_NoCaloMatched"); } else { missingBranches.push_back("Events/HLT_DiPFJet15_NoCaloMatched"); }
    if ( choose["Events/HLT_DiPFJet25_FBEta3_NoCaloMatched"] )
      if (input->present("Events/HLT_DiPFJet25_FBEta3_NoCaloMatched")) { input->select("Events/HLT_DiPFJet25_FBEta3_NoCaloMatched", HLT_DiPFJet25_FBEta3_NoCaloMatched); successBranches.push_back("Events/HLT_DiPFJet25_FBEta3_NoCaloMatched"); } else { missingBranches.push_back("Events/HLT_DiPFJet25_FBEta3_NoCaloMatched"); }
    if ( choose["Events/HLT_DiPFJet25_NoCaloMatched"] )
      if (input->present("Events/HLT_DiPFJet25_NoCaloMatched")) { input->select("Events/HLT_DiPFJet25_NoCaloMatched", HLT_DiPFJet25_NoCaloMatched); successBranches.push_back("Events/HLT_DiPFJet25_NoCaloMatched"); } else { missingBranches.push_back("Events/HLT_DiPFJet25_NoCaloMatched"); }
    if ( choose["Events/HLT_DiPFJetAve100_HFJEC"] )
      if (input->present("Events/HLT_DiPFJetAve100_HFJEC")) { input->select("Events/HLT_DiPFJetAve100_HFJEC", HLT_DiPFJetAve100_HFJEC); successBranches.push_back("Events/HLT_DiPFJetAve100_HFJEC"); } else { missingBranches.push_back("Events/HLT_DiPFJetAve100_HFJEC"); }
    if ( choose["Events/HLT_DiPFJetAve140"] )
      if (input->present("Events/HLT_DiPFJetAve140")) { input->select("Events/HLT_DiPFJetAve140", HLT_DiPFJetAve140); successBranches.push_back("Events/HLT_DiPFJetAve140"); } else { missingBranches.push_back("Events/HLT_DiPFJetAve140"); }
    if ( choose["Events/HLT_DiPFJetAve15_HFJEC"] )
      if (input->present("Events/HLT_DiPFJetAve15_HFJEC")) { input->select("Events/HLT_DiPFJetAve15_HFJEC", HLT_DiPFJetAve15_HFJEC); successBranches.push_back("Events/HLT_DiPFJetAve15_HFJEC"); } else { missingBranches.push_back("Events/HLT_DiPFJetAve15_HFJEC"); }
    if ( choose["Events/HLT_DiPFJetAve160_HFJEC"] )
      if (input->present("Events/HLT_DiPFJetAve160_HFJEC")) { input->select("Events/HLT_DiPFJetAve160_HFJEC", HLT_DiPFJetAve160_HFJEC); successBranches.push_back("Events/HLT_DiPFJetAve160_HFJEC"); } else { missingBranches.push_back("Events/HLT_DiPFJetAve160_HFJEC"); }
    if ( choose["Events/HLT_DiPFJetAve200"] )
      if (input->present("Events/HLT_DiPFJetAve200")) { input->select("Events/HLT_DiPFJetAve200", HLT_DiPFJetAve200); successBranches.push_back("Events/HLT_DiPFJetAve200"); } else { missingBranches.push_back("Events/HLT_DiPFJetAve200"); }
    if ( choose["Events/HLT_DiPFJetAve220_HFJEC"] )
      if (input->present("Events/HLT_DiPFJetAve220_HFJEC")) { input->select("Events/HLT_DiPFJetAve220_HFJEC", HLT_DiPFJetAve220_HFJEC); successBranches.push_back("Events/HLT_DiPFJetAve220_HFJEC"); } else { missingBranches.push_back("Events/HLT_DiPFJetAve220_HFJEC"); }
    if ( choose["Events/HLT_DiPFJetAve25_HFJEC"] )
      if (input->present("Events/HLT_DiPFJetAve25_HFJEC")) { input->select("Events/HLT_DiPFJetAve25_HFJEC", HLT_DiPFJetAve25_HFJEC); successBranches.push_back("Events/HLT_DiPFJetAve25_HFJEC"); } else { missingBranches.push_back("Events/HLT_DiPFJetAve25_HFJEC"); }
    if ( choose["Events/HLT_DiPFJetAve260"] )
      if (input->present("Events/HLT_DiPFJetAve260")) { input->select("Events/HLT_DiPFJetAve260", HLT_DiPFJetAve260); successBranches.push_back("Events/HLT_DiPFJetAve260"); } else { missingBranches.push_back("Events/HLT_DiPFJetAve260"); }
    if ( choose["Events/HLT_DiPFJetAve300_HFJEC"] )
      if (input->present("Events/HLT_DiPFJetAve300_HFJEC")) { input->select("Events/HLT_DiPFJetAve300_HFJEC", HLT_DiPFJetAve300_HFJEC); successBranches.push_back("Events/HLT_DiPFJetAve300_HFJEC"); } else { missingBranches.push_back("Events/HLT_DiPFJetAve300_HFJEC"); }
    if ( choose["Events/HLT_DiPFJetAve320"] )
      if (input->present("Events/HLT_DiPFJetAve320")) { input->select("Events/HLT_DiPFJetAve320", HLT_DiPFJetAve320); successBranches.push_back("Events/HLT_DiPFJetAve320"); } else { missingBranches.push_back("Events/HLT_DiPFJetAve320"); }
    if ( choose["Events/HLT_DiPFJetAve35_HFJEC"] )
      if (input->present("Events/HLT_DiPFJetAve35_HFJEC")) { input->select("Events/HLT_DiPFJetAve35_HFJEC", HLT_DiPFJetAve35_HFJEC); successBranches.push_back("Events/HLT_DiPFJetAve35_HFJEC"); } else { missingBranches.push_back("Events/HLT_DiPFJetAve35_HFJEC"); }
    if ( choose["Events/HLT_DiPFJetAve40"] )
      if (input->present("Events/HLT_DiPFJetAve40")) { input->select("Events/HLT_DiPFJetAve40", HLT_DiPFJetAve40); successBranches.push_back("Events/HLT_DiPFJetAve40"); } else { missingBranches.push_back("Events/HLT_DiPFJetAve40"); }
    if ( choose["Events/HLT_DiPFJetAve400"] )
      if (input->present("Events/HLT_DiPFJetAve400")) { input->select("Events/HLT_DiPFJetAve400", HLT_DiPFJetAve400); successBranches.push_back("Events/HLT_DiPFJetAve400"); } else { missingBranches.push_back("Events/HLT_DiPFJetAve400"); }
    if ( choose["Events/HLT_DiPFJetAve500"] )
      if (input->present("Events/HLT_DiPFJetAve500")) { input->select("Events/HLT_DiPFJetAve500", HLT_DiPFJetAve500); successBranches.push_back("Events/HLT_DiPFJetAve500"); } else { missingBranches.push_back("Events/HLT_DiPFJetAve500"); }
    if ( choose["Events/HLT_DiPFJetAve60"] )
      if (input->present("Events/HLT_DiPFJetAve60")) { input->select("Events/HLT_DiPFJetAve60", HLT_DiPFJetAve60); successBranches.push_back("Events/HLT_DiPFJetAve60"); } else { missingBranches.push_back("Events/HLT_DiPFJetAve60"); }
    if ( choose["Events/HLT_DiPFJetAve60_HFJEC"] )
      if (input->present("Events/HLT_DiPFJetAve60_HFJEC")) { input->select("Events/HLT_DiPFJetAve60_HFJEC", HLT_DiPFJetAve60_HFJEC); successBranches.push_back("Events/HLT_DiPFJetAve60_HFJEC"); } else { missingBranches.push_back("Events/HLT_DiPFJetAve60_HFJEC"); }
    if ( choose["Events/HLT_DiPFJetAve80"] )
      if (input->present("Events/HLT_DiPFJetAve80")) { input->select("Events/HLT_DiPFJetAve80", HLT_DiPFJetAve80); successBranches.push_back("Events/HLT_DiPFJetAve80"); } else { missingBranches.push_back("Events/HLT_DiPFJetAve80"); }
    if ( choose["Events/HLT_DiPFJetAve80_HFJEC"] )
      if (input->present("Events/HLT_DiPFJetAve80_HFJEC")) { input->select("Events/HLT_DiPFJetAve80_HFJEC", HLT_DiPFJetAve80_HFJEC); successBranches.push_back("Events/HLT_DiPFJetAve80_HFJEC"); } else { missingBranches.push_back("Events/HLT_DiPFJetAve80_HFJEC"); }
    if ( choose["Events/HLT_Ele115_CaloIdVT_GsfTrkIdT"] )
      if (input->present("Events/HLT_Ele115_CaloIdVT_GsfTrkIdT")) { input->select("Events/HLT_Ele115_CaloIdVT_GsfTrkIdT", HLT_Ele115_CaloIdVT_GsfTrkIdT); successBranches.push_back("Events/HLT_Ele115_CaloIdVT_GsfTrkIdT"); } else { missingBranches.push_back("Events/HLT_Ele115_CaloIdVT_GsfTrkIdT"); }
    if ( choose["Events/HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30"] )
      if (input->present("Events/HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30")) { input->select("Events/HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30", HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30); successBranches.push_back("Events/HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30"); } else { missingBranches.push_back("Events/HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30"); }
    if ( choose["Events/HLT_Ele135_CaloIdVT_GsfTrkIdT"] )
      if (input->present("Events/HLT_Ele135_CaloIdVT_GsfTrkIdT")) { input->select("Events/HLT_Ele135_CaloIdVT_GsfTrkIdT", HLT_Ele135_CaloIdVT_GsfTrkIdT); successBranches.push_back("Events/HLT_Ele135_CaloIdVT_GsfTrkIdT"); } else { missingBranches.push_back("Events/HLT_Ele135_CaloIdVT_GsfTrkIdT"); }
    if ( choose["Events/HLT_Ele145_CaloIdVT_GsfTrkIdT"] )
      if (input->present("Events/HLT_Ele145_CaloIdVT_GsfTrkIdT")) { input->select("Events/HLT_Ele145_CaloIdVT_GsfTrkIdT", HLT_Ele145_CaloIdVT_GsfTrkIdT); successBranches.push_back("Events/HLT_Ele145_CaloIdVT_GsfTrkIdT"); } else { missingBranches.push_back("Events/HLT_Ele145_CaloIdVT_GsfTrkIdT"); }
    if ( choose["Events/HLT_Ele15_IsoVVVL_PFHT450"] )
      if (input->present("Events/HLT_Ele15_IsoVVVL_PFHT450")) { input->select("Events/HLT_Ele15_IsoVVVL_PFHT450", HLT_Ele15_IsoVVVL_PFHT450); successBranches.push_back("Events/HLT_Ele15_IsoVVVL_PFHT450"); } else { missingBranches.push_back("Events/HLT_Ele15_IsoVVVL_PFHT450"); }
    if ( choose["Events/HLT_Ele15_IsoVVVL_PFHT450_CaloBTagCSV_4p5"] )
      if (input->present("Events/HLT_Ele15_IsoVVVL_PFHT450_CaloBTagCSV_4p5")) { input->select("Events/HLT_Ele15_IsoVVVL_PFHT450_CaloBTagCSV_4p5", HLT_Ele15_IsoVVVL_PFHT450_CaloBTagCSV_4p5); successBranches.push_back("Events/HLT_Ele15_IsoVVVL_PFHT450_CaloBTagCSV_4p5"); } else { missingBranches.push_back("Events/HLT_Ele15_IsoVVVL_PFHT450_CaloBTagCSV_4p5"); }
    if ( choose["Events/HLT_Ele15_IsoVVVL_PFHT450_PFMET50"] )
      if (input->present("Events/HLT_Ele15_IsoVVVL_PFHT450_PFMET50")) { input->select("Events/HLT_Ele15_IsoVVVL_PFHT450_PFMET50", HLT_Ele15_IsoVVVL_PFHT450_PFMET50); successBranches.push_back("Events/HLT_Ele15_IsoVVVL_PFHT450_PFMET50"); } else { missingBranches.push_back("Events/HLT_Ele15_IsoVVVL_PFHT450_PFMET50"); }
    if ( choose["Events/HLT_Ele15_IsoVVVL_PFHT600"] )
      if (input->present("Events/HLT_Ele15_IsoVVVL_PFHT600")) { input->select("Events/HLT_Ele15_IsoVVVL_PFHT600", HLT_Ele15_IsoVVVL_PFHT600); successBranches.push_back("Events/HLT_Ele15_IsoVVVL_PFHT600"); } else { missingBranches.push_back("Events/HLT_Ele15_IsoVVVL_PFHT600"); }
    if ( choose["Events/HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL"] )
      if (input->present("Events/HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL")) { input->select("Events/HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL", HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL); successBranches.push_back("Events/HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL"); } else { missingBranches.push_back("Events/HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL"); }
    if ( choose["Events/HLT_Ele17_CaloIdM_TrackIdM_PFJet30"] )
      if (input->present("Events/HLT_Ele17_CaloIdM_TrackIdM_PFJet30")) { input->select("Events/HLT_Ele17_CaloIdM_TrackIdM_PFJet30", HLT_Ele17_CaloIdM_TrackIdM_PFJet30); successBranches.push_back("Events/HLT_Ele17_CaloIdM_TrackIdM_PFJet30"); } else { missingBranches.push_back("Events/HLT_Ele17_CaloIdM_TrackIdM_PFJet30"); }
    if ( choose["Events/HLT_Ele200_CaloIdVT_GsfTrkIdT"] )
      if (input->present("Events/HLT_Ele200_CaloIdVT_GsfTrkIdT")) { input->select("Events/HLT_Ele200_CaloIdVT_GsfTrkIdT", HLT_Ele200_CaloIdVT_GsfTrkIdT); successBranches.push_back("Events/HLT_Ele200_CaloIdVT_GsfTrkIdT"); } else { missingBranches.push_back("Events/HLT_Ele200_CaloIdVT_GsfTrkIdT"); }
    if ( choose["Events/HLT_Ele20_WPLoose_Gsf"] )
      if (input->present("Events/HLT_Ele20_WPLoose_Gsf")) { input->select("Events/HLT_Ele20_WPLoose_Gsf", HLT_Ele20_WPLoose_Gsf); successBranches.push_back("Events/HLT_Ele20_WPLoose_Gsf"); } else { missingBranches.push_back("Events/HLT_Ele20_WPLoose_Gsf"); }
    if ( choose["Events/HLT_Ele20_WPTight_Gsf"] )
      if (input->present("Events/HLT_Ele20_WPTight_Gsf")) { input->select("Events/HLT_Ele20_WPTight_Gsf", HLT_Ele20_WPTight_Gsf); successBranches.push_back("Events/HLT_Ele20_WPTight_Gsf"); } else { missingBranches.push_back("Events/HLT_Ele20_WPTight_Gsf"); }
    if ( choose["Events/HLT_Ele20_eta2p1_WPLoose_Gsf"] )
      if (input->present("Events/HLT_Ele20_eta2p1_WPLoose_Gsf")) { input->select("Events/HLT_Ele20_eta2p1_WPLoose_Gsf", HLT_Ele20_eta2p1_WPLoose_Gsf); successBranches.push_back("Events/HLT_Ele20_eta2p1_WPLoose_Gsf"); } else { missingBranches.push_back("Events/HLT_Ele20_eta2p1_WPLoose_Gsf"); }
    if ( choose["Events/HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30"] )
      if (input->present("Events/HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30")) { input->select("Events/HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30", HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30); successBranches.push_back("Events/HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30"); } else { missingBranches.push_back("Events/HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30"); }
    if ( choose["Events/HLT_Ele23_CaloIdM_TrackIdM_PFJet30"] )
      if (input->present("Events/HLT_Ele23_CaloIdM_TrackIdM_PFJet30")) { input->select("Events/HLT_Ele23_CaloIdM_TrackIdM_PFJet30", HLT_Ele23_CaloIdM_TrackIdM_PFJet30); successBranches.push_back("Events/HLT_Ele23_CaloIdM_TrackIdM_PFJet30"); } else { missingBranches.push_back("Events/HLT_Ele23_CaloIdM_TrackIdM_PFJet30"); }
    if ( choose["Events/HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL"] )
      if (input->present("Events/HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL")) { input->select("Events/HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL", HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL); successBranches.push_back("Events/HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL"); } else { missingBranches.push_back("Events/HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL"); }
    if ( choose["Events/HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ"] )
      if (input->present("Events/HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ")) { input->select("Events/HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ", HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ); successBranches.push_back("Events/HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ"); } else { missingBranches.push_back("Events/HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ"); }
    if ( choose["Events/HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_CrossL1"] )
      if (input->present("Events/HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_CrossL1")) { input->select("Events/HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_CrossL1", HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_CrossL1); successBranches.push_back("Events/HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_CrossL1"); } else { missingBranches.push_back("Events/HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_CrossL1"); }
    if ( choose["Events/HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_TightID_CrossL1"] )
      if (input->present("Events/HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_TightID_CrossL1")) { input->select("Events/HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_TightID_CrossL1", HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_TightID_CrossL1); successBranches.push_back("Events/HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_TightID_CrossL1"); } else { missingBranches.push_back("Events/HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_TightID_CrossL1"); }
    if ( choose["Events/HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_CrossL1"] )
      if (input->present("Events/HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_CrossL1")) { input->select("Events/HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_CrossL1", HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_CrossL1); successBranches.push_back("Events/HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_CrossL1"); } else { missingBranches.push_back("Events/HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_CrossL1"); }
    if ( choose["Events/HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_TightID_CrossL1"] )
      if (input->present("Events/HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_TightID_CrossL1")) { input->select("Events/HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_TightID_CrossL1", HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_TightID_CrossL1); successBranches.push_back("Events/HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_TightID_CrossL1"); } else { missingBranches.push_back("Events/HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_TightID_CrossL1"); }
    if ( choose["Events/HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_CrossL1"] )
      if (input->present("Events/HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_CrossL1")) { input->select("Events/HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_CrossL1", HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_CrossL1); successBranches.push_back("Events/HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_CrossL1"); } else { missingBranches.push_back("Events/HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_CrossL1"); }
    if ( choose["Events/HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_TightID_CrossL1"] )
      if (input->present("Events/HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_TightID_CrossL1")) { input->select("Events/HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_TightID_CrossL1", HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_TightID_CrossL1); successBranches.push_back("Events/HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_TightID_CrossL1"); } else { missingBranches.push_back("Events/HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_TightID_CrossL1"); }
    if ( choose["Events/HLT_Ele250_CaloIdVT_GsfTrkIdT"] )
      if (input->present("Events/HLT_Ele250_CaloIdVT_GsfTrkIdT")) { input->select("Events/HLT_Ele250_CaloIdVT_GsfTrkIdT", HLT_Ele250_CaloIdVT_GsfTrkIdT); successBranches.push_back("Events/HLT_Ele250_CaloIdVT_GsfTrkIdT"); } else { missingBranches.push_back("Events/HLT_Ele250_CaloIdVT_GsfTrkIdT"); }
    if ( choose["Events/HLT_Ele27_Ele37_CaloIdL_MW"] )
      if (input->present("Events/HLT_Ele27_Ele37_CaloIdL_MW")) { input->select("Events/HLT_Ele27_Ele37_CaloIdL_MW", HLT_Ele27_Ele37_CaloIdL_MW); successBranches.push_back("Events/HLT_Ele27_Ele37_CaloIdL_MW"); } else { missingBranches.push_back("Events/HLT_Ele27_Ele37_CaloIdL_MW"); }
    if ( choose["Events/HLT_Ele27_WPTight_Gsf"] )
      if (input->present("Events/HLT_Ele27_WPTight_Gsf")) { input->select("Events/HLT_Ele27_WPTight_Gsf", HLT_Ele27_WPTight_Gsf); successBranches.push_back("Events/HLT_Ele27_WPTight_Gsf"); } else { missingBranches.push_back("Events/HLT_Ele27_WPTight_Gsf"); }
    if ( choose["Events/HLT_Ele28_HighEta_SC20_Mass55"] )
      if (input->present("Events/HLT_Ele28_HighEta_SC20_Mass55")) { input->select("Events/HLT_Ele28_HighEta_SC20_Mass55", HLT_Ele28_HighEta_SC20_Mass55); successBranches.push_back("Events/HLT_Ele28_HighEta_SC20_Mass55"); } else { missingBranches.push_back("Events/HLT_Ele28_HighEta_SC20_Mass55"); }
    if ( choose["Events/HLT_Ele28_eta2p1_WPTight_Gsf_HT150"] )
      if (input->present("Events/HLT_Ele28_eta2p1_WPTight_Gsf_HT150")) { input->select("Events/HLT_Ele28_eta2p1_WPTight_Gsf_HT150", HLT_Ele28_eta2p1_WPTight_Gsf_HT150); successBranches.push_back("Events/HLT_Ele28_eta2p1_WPTight_Gsf_HT150"); } else { missingBranches.push_back("Events/HLT_Ele28_eta2p1_WPTight_Gsf_HT150"); }
    if ( choose["Events/HLT_Ele300_CaloIdVT_GsfTrkIdT"] )
      if (input->present("Events/HLT_Ele300_CaloIdVT_GsfTrkIdT")) { input->select("Events/HLT_Ele300_CaloIdVT_GsfTrkIdT", HLT_Ele300_CaloIdVT_GsfTrkIdT); successBranches.push_back("Events/HLT_Ele300_CaloIdVT_GsfTrkIdT"); } else { missingBranches.push_back("Events/HLT_Ele300_CaloIdVT_GsfTrkIdT"); }
    if ( choose["Events/HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned"] )
      if (input->present("Events/HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned")) { input->select("Events/HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned", HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned); successBranches.push_back("Events/HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned"); } else { missingBranches.push_back("Events/HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned"); }
    if ( choose["Events/HLT_Ele32_WPTight_Gsf"] )
      if (input->present("Events/HLT_Ele32_WPTight_Gsf")) { input->select("Events/HLT_Ele32_WPTight_Gsf", HLT_Ele32_WPTight_Gsf); successBranches.push_back("Events/HLT_Ele32_WPTight_Gsf"); } else { missingBranches.push_back("Events/HLT_Ele32_WPTight_Gsf"); }
    if ( choose["Events/HLT_Ele32_WPTight_Gsf_L1DoubleEG"] )
      if (input->present("Events/HLT_Ele32_WPTight_Gsf_L1DoubleEG")) { input->select("Events/HLT_Ele32_WPTight_Gsf_L1DoubleEG", HLT_Ele32_WPTight_Gsf_L1DoubleEG); successBranches.push_back("Events/HLT_Ele32_WPTight_Gsf_L1DoubleEG"); } else { missingBranches.push_back("Events/HLT_Ele32_WPTight_Gsf_L1DoubleEG"); }
    if ( choose["Events/HLT_Ele35_WPTight_Gsf"] )
      if (input->present("Events/HLT_Ele35_WPTight_Gsf")) { input->select("Events/HLT_Ele35_WPTight_Gsf", HLT_Ele35_WPTight_Gsf); successBranches.push_back("Events/HLT_Ele35_WPTight_Gsf"); } else { missingBranches.push_back("Events/HLT_Ele35_WPTight_Gsf"); }
    if ( choose["Events/HLT_Ele35_WPTight_Gsf_L1EGMT"] )
      if (input->present("Events/HLT_Ele35_WPTight_Gsf_L1EGMT")) { input->select("Events/HLT_Ele35_WPTight_Gsf_L1EGMT", HLT_Ele35_WPTight_Gsf_L1EGMT); successBranches.push_back("Events/HLT_Ele35_WPTight_Gsf_L1EGMT"); } else { missingBranches.push_back("Events/HLT_Ele35_WPTight_Gsf_L1EGMT"); }
    if ( choose["Events/HLT_Ele38_WPTight_Gsf"] )
      if (input->present("Events/HLT_Ele38_WPTight_Gsf")) { input->select("Events/HLT_Ele38_WPTight_Gsf", HLT_Ele38_WPTight_Gsf); successBranches.push_back("Events/HLT_Ele38_WPTight_Gsf"); } else { missingBranches.push_back("Events/HLT_Ele38_WPTight_Gsf"); }
    if ( choose["Events/HLT_Ele40_WPTight_Gsf"] )
      if (input->present("Events/HLT_Ele40_WPTight_Gsf")) { input->select("Events/HLT_Ele40_WPTight_Gsf", HLT_Ele40_WPTight_Gsf); successBranches.push_back("Events/HLT_Ele40_WPTight_Gsf"); } else { missingBranches.push_back("Events/HLT_Ele40_WPTight_Gsf"); }
    if ( choose["Events/HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165"] )
      if (input->present("Events/HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165")) { input->select("Events/HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165", HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165); successBranches.push_back("Events/HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165"); } else { missingBranches.push_back("Events/HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165"); }
    if ( choose["Events/HLT_Ele50_IsoVVVL_PFHT450"] )
      if (input->present("Events/HLT_Ele50_IsoVVVL_PFHT450")) { input->select("Events/HLT_Ele50_IsoVVVL_PFHT450", HLT_Ele50_IsoVVVL_PFHT450); successBranches.push_back("Events/HLT_Ele50_IsoVVVL_PFHT450"); } else { missingBranches.push_back("Events/HLT_Ele50_IsoVVVL_PFHT450"); }
    if ( choose["Events/HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30"] )
      if (input->present("Events/HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30")) { input->select("Events/HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30", HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30); successBranches.push_back("Events/HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30"); } else { missingBranches.push_back("Events/HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30"); }
    if ( choose["Events/HLT_Ele8_CaloIdM_TrackIdM_PFJet30"] )
      if (input->present("Events/HLT_Ele8_CaloIdM_TrackIdM_PFJet30")) { input->select("Events/HLT_Ele8_CaloIdM_TrackIdM_PFJet30", HLT_Ele8_CaloIdM_TrackIdM_PFJet30); successBranches.push_back("Events/HLT_Ele8_CaloIdM_TrackIdM_PFJet30"); } else { missingBranches.push_back("Events/HLT_Ele8_CaloIdM_TrackIdM_PFJet30"); }
    if ( choose["Events/HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07"] )
      if (input->present("Events/HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07")) { input->select("Events/HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07", HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07); successBranches.push_back("Events/HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07"); } else { missingBranches.push_back("Events/HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07"); }
    if ( choose["Events/HLT_IsoMu20"] )
      if (input->present("Events/HLT_IsoMu20")) { input->select("Events/HLT_IsoMu20", HLT_IsoMu20); successBranches.push_back("Events/HLT_IsoMu20"); } else { missingBranches.push_back("Events/HLT_IsoMu20"); }
    if ( choose["Events/HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_CrossL1"] )
      if (input->present("Events/HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_CrossL1")) { input->select("Events/HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_CrossL1", HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_CrossL1); successBranches.push_back("Events/HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_CrossL1"); } else { missingBranches.push_back("Events/HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_CrossL1"); }
    if ( choose["Events/HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_TightID_CrossL1"] )
      if (input->present("Events/HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_TightID_CrossL1")) { input->select("Events/HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_TightID_CrossL1", HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_TightID_CrossL1); successBranches.push_back("Events/HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_TightID_CrossL1"); } else { missingBranches.push_back("Events/HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_TightID_CrossL1"); }
    if ( choose["Events/HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_CrossL1"] )
      if (input->present("Events/HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_CrossL1")) { input->select("Events/HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_CrossL1", HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_CrossL1); successBranches.push_back("Events/HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_CrossL1"); } else { missingBranches.push_back("Events/HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_CrossL1"); }
    if ( choose["Events/HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_TightID_CrossL1"] )
      if (input->present("Events/HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_TightID_CrossL1")) { input->select("Events/HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_TightID_CrossL1", HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_TightID_CrossL1); successBranches.push_back("Events/HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_TightID_CrossL1"); } else { missingBranches.push_back("Events/HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_TightID_CrossL1"); }
    if ( choose["Events/HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_CrossL1"] )
      if (input->present("Events/HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_CrossL1")) { input->select("Events/HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_CrossL1", HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_CrossL1); successBranches.push_back("Events/HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_CrossL1"); } else { missingBranches.push_back("Events/HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_CrossL1"); }
    if ( choose["Events/HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_TightID_CrossL1"] )
      if (input->present("Events/HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_TightID_CrossL1")) { input->select("Events/HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_TightID_CrossL1", HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_TightID_CrossL1); successBranches.push_back("Events/HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_TightID_CrossL1"); } else { missingBranches.push_back("Events/HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_TightID_CrossL1"); }
    if ( choose["Events/HLT_IsoMu24"] )
      if (input->present("Events/HLT_IsoMu24")) { input->select("Events/HLT_IsoMu24", HLT_IsoMu24); successBranches.push_back("Events/HLT_IsoMu24"); } else { missingBranches.push_back("Events/HLT_IsoMu24"); }
    if ( choose["Events/HLT_IsoMu24_eta2p1"] )
      if (input->present("Events/HLT_IsoMu24_eta2p1")) { input->select("Events/HLT_IsoMu24_eta2p1", HLT_IsoMu24_eta2p1); successBranches.push_back("Events/HLT_IsoMu24_eta2p1"); } else { missingBranches.push_back("Events/HLT_IsoMu24_eta2p1"); }
    if ( choose["Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_SingleL1"] )
      if (input->present("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_SingleL1")) { input->select("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_SingleL1", HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_SingleL1); successBranches.push_back("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_SingleL1"); } else { missingBranches.push_back("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_SingleL1"); }
    if ( choose["Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_TightID_SingleL1"] )
      if (input->present("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_TightID_SingleL1")) { input->select("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_TightID_SingleL1", HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_TightID_SingleL1); successBranches.push_back("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_TightID_SingleL1"); } else { missingBranches.push_back("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_TightID_SingleL1"); }
    if ( choose["Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1"] )
      if (input->present("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1")) { input->select("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1", HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1); successBranches.push_back("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1"); } else { missingBranches.push_back("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1"); }
    if ( choose["Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1"] )
      if (input->present("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1")) { input->select("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1", HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1); successBranches.push_back("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1"); } else { missingBranches.push_back("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1"); }
    if ( choose["Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_SingleL1"] )
      if (input->present("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_SingleL1")) { input->select("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_SingleL1", HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_SingleL1); successBranches.push_back("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_SingleL1"); } else { missingBranches.push_back("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_SingleL1"); }
    if ( choose["Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_TightID_SingleL1"] )
      if (input->present("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_TightID_SingleL1")) { input->select("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_TightID_SingleL1", HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_TightID_SingleL1); successBranches.push_back("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_TightID_SingleL1"); } else { missingBranches.push_back("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_TightID_SingleL1"); }
    if ( choose["Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1"] )
      if (input->present("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1")) { input->select("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1", HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1); successBranches.push_back("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1"); } else { missingBranches.push_back("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1"); }
    if ( choose["Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1"] )
      if (input->present("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1")) { input->select("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1", HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1); successBranches.push_back("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1"); } else { missingBranches.push_back("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1"); }
    if ( choose["Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1"] )
      if (input->present("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1")) { input->select("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1", HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1); successBranches.push_back("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1"); } else { missingBranches.push_back("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1"); }
    if ( choose["Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1"] )
      if (input->present("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1")) { input->select("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1", HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1); successBranches.push_back("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1"); } else { missingBranches.push_back("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1"); }
    if ( choose["Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau50_Trk30_eta2p1_1pr"] )
      if (input->present("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau50_Trk30_eta2p1_1pr")) { input->select("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau50_Trk30_eta2p1_1pr", HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau50_Trk30_eta2p1_1pr); successBranches.push_back("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau50_Trk30_eta2p1_1pr"); } else { missingBranches.push_back("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau50_Trk30_eta2p1_1pr"); }
    if ( choose["Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_SingleL1"] )
      if (input->present("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_SingleL1")) { input->select("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_SingleL1", HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_SingleL1); successBranches.push_back("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_SingleL1"); } else { missingBranches.push_back("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_SingleL1"); }
    if ( choose["Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_TightID_SingleL1"] )
      if (input->present("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_TightID_SingleL1")) { input->select("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_TightID_SingleL1", HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_TightID_SingleL1); successBranches.push_back("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_TightID_SingleL1"); } else { missingBranches.push_back("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_TightID_SingleL1"); }
    if ( choose["Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1"] )
      if (input->present("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1")) { input->select("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1", HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1); successBranches.push_back("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1"); } else { missingBranches.push_back("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1"); }
    if ( choose["Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1"] )
      if (input->present("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1")) { input->select("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1", HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1); successBranches.push_back("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1"); } else { missingBranches.push_back("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1"); }
    if ( choose["Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1"] )
      if (input->present("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1")) { input->select("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1", HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1); successBranches.push_back("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1"); } else { missingBranches.push_back("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1"); }
    if ( choose["Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1"] )
      if (input->present("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1")) { input->select("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1", HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1); successBranches.push_back("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1"); } else { missingBranches.push_back("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1"); }
    if ( choose["Events/HLT_IsoMu27"] )
      if (input->present("Events/HLT_IsoMu27")) { input->select("Events/HLT_IsoMu27", HLT_IsoMu27); successBranches.push_back("Events/HLT_IsoMu27"); } else { missingBranches.push_back("Events/HLT_IsoMu27"); }
    if ( choose["Events/HLT_IsoMu27_LooseChargedIsoPFTau20_SingleL1"] )
      if (input->present("Events/HLT_IsoMu27_LooseChargedIsoPFTau20_SingleL1")) { input->select("Events/HLT_IsoMu27_LooseChargedIsoPFTau20_SingleL1", HLT_IsoMu27_LooseChargedIsoPFTau20_SingleL1); successBranches.push_back("Events/HLT_IsoMu27_LooseChargedIsoPFTau20_SingleL1"); } else { missingBranches.push_back("Events/HLT_IsoMu27_LooseChargedIsoPFTau20_SingleL1"); }
    if ( choose["Events/HLT_IsoMu27_MediumChargedIsoPFTau20_SingleL1"] )
      if (input->present("Events/HLT_IsoMu27_MediumChargedIsoPFTau20_SingleL1")) { input->select("Events/HLT_IsoMu27_MediumChargedIsoPFTau20_SingleL1", HLT_IsoMu27_MediumChargedIsoPFTau20_SingleL1); successBranches.push_back("Events/HLT_IsoMu27_MediumChargedIsoPFTau20_SingleL1"); } else { missingBranches.push_back("Events/HLT_IsoMu27_MediumChargedIsoPFTau20_SingleL1"); }
    if ( choose["Events/HLT_IsoMu27_TightChargedIsoPFTau20_SingleL1"] )
      if (input->present("Events/HLT_IsoMu27_TightChargedIsoPFTau20_SingleL1")) { input->select("Events/HLT_IsoMu27_TightChargedIsoPFTau20_SingleL1", HLT_IsoMu27_TightChargedIsoPFTau20_SingleL1); successBranches.push_back("Events/HLT_IsoMu27_TightChargedIsoPFTau20_SingleL1"); } else { missingBranches.push_back("Events/HLT_IsoMu27_TightChargedIsoPFTau20_SingleL1"); }
    if ( choose["Events/HLT_IsoMu30"] )
      if (input->present("Events/HLT_IsoMu30")) { input->select("Events/HLT_IsoMu30", HLT_IsoMu30); successBranches.push_back("Events/HLT_IsoMu30"); } else { missingBranches.push_back("Events/HLT_IsoMu30"); }
    if ( choose["Events/HLT_Mu10_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT350_PFMETNoMu60"] )
      if (input->present("Events/HLT_Mu10_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT350_PFMETNoMu60")) { input->select("Events/HLT_Mu10_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT350_PFMETNoMu60", HLT_Mu10_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT350_PFMETNoMu60); successBranches.push_back("Events/HLT_Mu10_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT350_PFMETNoMu60"); } else { missingBranches.push_back("Events/HLT_Mu10_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT350_PFMETNoMu60"); }
    if ( choose["Events/HLT_Mu12_DoublePFJets100_CaloBTagCSV_p33"] )
      if (input->present("Events/HLT_Mu12_DoublePFJets100_CaloBTagCSV_p33")) { input->select("Events/HLT_Mu12_DoublePFJets100_CaloBTagCSV_p33", HLT_Mu12_DoublePFJets100_CaloBTagCSV_p33); successBranches.push_back("Events/HLT_Mu12_DoublePFJets100_CaloBTagCSV_p33"); } else { missingBranches.push_back("Events/HLT_Mu12_DoublePFJets100_CaloBTagCSV_p33"); }
    if ( choose["Events/HLT_Mu12_DoublePFJets200_CaloBTagCSV_p33"] )
      if (input->present("Events/HLT_Mu12_DoublePFJets200_CaloBTagCSV_p33")) { input->select("Events/HLT_Mu12_DoublePFJets200_CaloBTagCSV_p33", HLT_Mu12_DoublePFJets200_CaloBTagCSV_p33); successBranches.push_back("Events/HLT_Mu12_DoublePFJets200_CaloBTagCSV_p33"); } else { missingBranches.push_back("Events/HLT_Mu12_DoublePFJets200_CaloBTagCSV_p33"); }
    if ( choose["Events/HLT_Mu12_DoublePFJets350_CaloBTagCSV_p33"] )
      if (input->present("Events/HLT_Mu12_DoublePFJets350_CaloBTagCSV_p33")) { input->select("Events/HLT_Mu12_DoublePFJets350_CaloBTagCSV_p33", HLT_Mu12_DoublePFJets350_CaloBTagCSV_p33); successBranches.push_back("Events/HLT_Mu12_DoublePFJets350_CaloBTagCSV_p33"); } else { missingBranches.push_back("Events/HLT_Mu12_DoublePFJets350_CaloBTagCSV_p33"); }
    if ( choose["Events/HLT_Mu12_DoublePFJets40MaxDeta1p6_DoubleCaloBTagCSV_p33"] )
      if (input->present("Events/HLT_Mu12_DoublePFJets40MaxDeta1p6_DoubleCaloBTagCSV_p33")) { input->select("Events/HLT_Mu12_DoublePFJets40MaxDeta1p6_DoubleCaloBTagCSV_p33", HLT_Mu12_DoublePFJets40MaxDeta1p6_DoubleCaloBTagCSV_p33); successBranches.push_back("Events/HLT_Mu12_DoublePFJets40MaxDeta1p6_DoubleCaloBTagCSV_p33"); } else { missingBranches.push_back("Events/HLT_Mu12_DoublePFJets40MaxDeta1p6_DoubleCaloBTagCSV_p33"); }
    if ( choose["Events/HLT_Mu12_DoublePFJets40_CaloBTagCSV_p33"] )
      if (input->present("Events/HLT_Mu12_DoublePFJets40_CaloBTagCSV_p33")) { input->select("Events/HLT_Mu12_DoublePFJets40_CaloBTagCSV_p33", HLT_Mu12_DoublePFJets40_CaloBTagCSV_p33); successBranches.push_back("Events/HLT_Mu12_DoublePFJets40_CaloBTagCSV_p33"); } else { missingBranches.push_back("Events/HLT_Mu12_DoublePFJets40_CaloBTagCSV_p33"); }
    if ( choose["Events/HLT_Mu12_DoublePFJets54MaxDeta1p6_DoubleCaloBTagCSV_p33"] )
      if (input->present("Events/HLT_Mu12_DoublePFJets54MaxDeta1p6_DoubleCaloBTagCSV_p33")) { input->select("Events/HLT_Mu12_DoublePFJets54MaxDeta1p6_DoubleCaloBTagCSV_p33", HLT_Mu12_DoublePFJets54MaxDeta1p6_DoubleCaloBTagCSV_p33); successBranches.push_back("Events/HLT_Mu12_DoublePFJets54MaxDeta1p6_DoubleCaloBTagCSV_p33"); } else { missingBranches.push_back("Events/HLT_Mu12_DoublePFJets54MaxDeta1p6_DoubleCaloBTagCSV_p33"); }
    if ( choose["Events/HLT_Mu12_DoublePFJets62MaxDeta1p6_DoubleCaloBTagCSV_p33"] )
      if (input->present("Events/HLT_Mu12_DoublePFJets62MaxDeta1p6_DoubleCaloBTagCSV_p33")) { input->select("Events/HLT_Mu12_DoublePFJets62MaxDeta1p6_DoubleCaloBTagCSV_p33", HLT_Mu12_DoublePFJets62MaxDeta1p6_DoubleCaloBTagCSV_p33); successBranches.push_back("Events/HLT_Mu12_DoublePFJets62MaxDeta1p6_DoubleCaloBTagCSV_p33"); } else { missingBranches.push_back("Events/HLT_Mu12_DoublePFJets62MaxDeta1p6_DoubleCaloBTagCSV_p33"); }
    if ( choose["Events/HLT_Mu12_DoublePhoton20"] )
      if (input->present("Events/HLT_Mu12_DoublePhoton20")) { input->select("Events/HLT_Mu12_DoublePhoton20", HLT_Mu12_DoublePhoton20); successBranches.push_back("Events/HLT_Mu12_DoublePhoton20"); } else { missingBranches.push_back("Events/HLT_Mu12_DoublePhoton20"); }
    if ( choose["Events/HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL"] )
      if (input->present("Events/HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL")) { input->select("Events/HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL", HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL); successBranches.push_back("Events/HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL"); } else { missingBranches.push_back("Events/HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL"); }
    if ( choose["Events/HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ"] )
      if (input->present("Events/HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ")) { input->select("Events/HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ", HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ); successBranches.push_back("Events/HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ"); } else { missingBranches.push_back("Events/HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ"); }
    if ( choose["Events/HLT_Mu15_IsoVVVL_PFHT450"] )
      if (input->present("Events/HLT_Mu15_IsoVVVL_PFHT450")) { input->select("Events/HLT_Mu15_IsoVVVL_PFHT450", HLT_Mu15_IsoVVVL_PFHT450); successBranches.push_back("Events/HLT_Mu15_IsoVVVL_PFHT450"); } else { missingBranches.push_back("Events/HLT_Mu15_IsoVVVL_PFHT450"); }
    if ( choose["Events/HLT_Mu15_IsoVVVL_PFHT450_CaloBTagCSV_4p5"] )
      if (input->present("Events/HLT_Mu15_IsoVVVL_PFHT450_CaloBTagCSV_4p5")) { input->select("Events/HLT_Mu15_IsoVVVL_PFHT450_CaloBTagCSV_4p5", HLT_Mu15_IsoVVVL_PFHT450_CaloBTagCSV_4p5); successBranches.push_back("Events/HLT_Mu15_IsoVVVL_PFHT450_CaloBTagCSV_4p5"); } else { missingBranches.push_back("Events/HLT_Mu15_IsoVVVL_PFHT450_CaloBTagCSV_4p5"); }
    if ( choose["Events/HLT_Mu15_IsoVVVL_PFHT450_PFMET50"] )
      if (input->present("Events/HLT_Mu15_IsoVVVL_PFHT450_PFMET50")) { input->select("Events/HLT_Mu15_IsoVVVL_PFHT450_PFMET50", HLT_Mu15_IsoVVVL_PFHT450_PFMET50); successBranches.push_back("Events/HLT_Mu15_IsoVVVL_PFHT450_PFMET50"); } else { missingBranches.push_back("Events/HLT_Mu15_IsoVVVL_PFHT450_PFMET50"); }
    if ( choose["Events/HLT_Mu15_IsoVVVL_PFHT600"] )
      if (input->present("Events/HLT_Mu15_IsoVVVL_PFHT600")) { input->select("Events/HLT_Mu15_IsoVVVL_PFHT600", HLT_Mu15_IsoVVVL_PFHT600); successBranches.push_back("Events/HLT_Mu15_IsoVVVL_PFHT600"); } else { missingBranches.push_back("Events/HLT_Mu15_IsoVVVL_PFHT600"); }
    if ( choose["Events/HLT_Mu17"] )
      if (input->present("Events/HLT_Mu17")) { input->select("Events/HLT_Mu17", HLT_Mu17); successBranches.push_back("Events/HLT_Mu17"); } else { missingBranches.push_back("Events/HLT_Mu17"); }
    if ( choose["Events/HLT_Mu17_Photon30_IsoCaloId"] )
      if (input->present("Events/HLT_Mu17_Photon30_IsoCaloId")) { input->select("Events/HLT_Mu17_Photon30_IsoCaloId", HLT_Mu17_Photon30_IsoCaloId); successBranches.push_back("Events/HLT_Mu17_Photon30_IsoCaloId"); } else { missingBranches.push_back("Events/HLT_Mu17_Photon30_IsoCaloId"); }
    if ( choose["Events/HLT_Mu17_TrkIsoVVL"] )
      if (input->present("Events/HLT_Mu17_TrkIsoVVL")) { input->select("Events/HLT_Mu17_TrkIsoVVL", HLT_Mu17_TrkIsoVVL); successBranches.push_back("Events/HLT_Mu17_TrkIsoVVL"); } else { missingBranches.push_back("Events/HLT_Mu17_TrkIsoVVL"); }
    if ( choose["Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL"] )
      if (input->present("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL")) { input->select("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL", HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL); successBranches.push_back("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL"); } else { missingBranches.push_back("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL"); }
    if ( choose["Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ"] )
      if (input->present("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ")) { input->select("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ", HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ); successBranches.push_back("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ"); } else { missingBranches.push_back("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ"); }
    if ( choose["Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8"] )
      if (input->present("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8")) { input->select("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8", HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8); successBranches.push_back("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8"); } else { missingBranches.push_back("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8"); }
    if ( choose["Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8"] )
      if (input->present("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8")) { input->select("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8", HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8); successBranches.push_back("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8"); } else { missingBranches.push_back("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8"); }
    if ( choose["Events/HLT_Mu18_Mu9"] )
      if (input->present("Events/HLT_Mu18_Mu9")) { input->select("Events/HLT_Mu18_Mu9", HLT_Mu18_Mu9); successBranches.push_back("Events/HLT_Mu18_Mu9"); } else { missingBranches.push_back("Events/HLT_Mu18_Mu9"); }
    if ( choose["Events/HLT_Mu18_Mu9_DZ"] )
      if (input->present("Events/HLT_Mu18_Mu9_DZ")) { input->select("Events/HLT_Mu18_Mu9_DZ", HLT_Mu18_Mu9_DZ); successBranches.push_back("Events/HLT_Mu18_Mu9_DZ"); } else { missingBranches.push_back("Events/HLT_Mu18_Mu9_DZ"); }
    if ( choose["Events/HLT_Mu18_Mu9_SameSign"] )
      if (input->present("Events/HLT_Mu18_Mu9_SameSign")) { input->select("Events/HLT_Mu18_Mu9_SameSign", HLT_Mu18_Mu9_SameSign); successBranches.push_back("Events/HLT_Mu18_Mu9_SameSign"); } else { missingBranches.push_back("Events/HLT_Mu18_Mu9_SameSign"); }
    if ( choose["Events/HLT_Mu18_Mu9_SameSign_DZ"] )
      if (input->present("Events/HLT_Mu18_Mu9_SameSign_DZ")) { input->select("Events/HLT_Mu18_Mu9_SameSign_DZ", HLT_Mu18_Mu9_SameSign_DZ); successBranches.push_back("Events/HLT_Mu18_Mu9_SameSign_DZ"); } else { missingBranches.push_back("Events/HLT_Mu18_Mu9_SameSign_DZ"); }
    if ( choose["Events/HLT_Mu19"] )
      if (input->present("Events/HLT_Mu19")) { input->select("Events/HLT_Mu19", HLT_Mu19); successBranches.push_back("Events/HLT_Mu19"); } else { missingBranches.push_back("Events/HLT_Mu19"); }
    if ( choose["Events/HLT_Mu19_TrkIsoVVL"] )
      if (input->present("Events/HLT_Mu19_TrkIsoVVL")) { input->select("Events/HLT_Mu19_TrkIsoVVL", HLT_Mu19_TrkIsoVVL); successBranches.push_back("Events/HLT_Mu19_TrkIsoVVL"); } else { missingBranches.push_back("Events/HLT_Mu19_TrkIsoVVL"); }
    if ( choose["Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL"] )
      if (input->present("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL")) { input->select("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL", HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL); successBranches.push_back("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL"); } else { missingBranches.push_back("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL"); }
    if ( choose["Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ"] )
      if (input->present("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ")) { input->select("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ", HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ); successBranches.push_back("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ"); } else { missingBranches.push_back("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ"); }
    if ( choose["Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass3p8"] )
      if (input->present("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass3p8")) { input->select("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass3p8", HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass3p8); successBranches.push_back("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass3p8"); } else { missingBranches.push_back("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass3p8"); }
    if ( choose["Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass8"] )
      if (input->present("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass8")) { input->select("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass8", HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass8); successBranches.push_back("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass8"); } else { missingBranches.push_back("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass8"); }
    if ( choose["Events/HLT_Mu20"] )
      if (input->present("Events/HLT_Mu20")) { input->select("Events/HLT_Mu20", HLT_Mu20); successBranches.push_back("Events/HLT_Mu20"); } else { missingBranches.push_back("Events/HLT_Mu20"); }
    if ( choose["Events/HLT_Mu20_Mu10"] )
      if (input->present("Events/HLT_Mu20_Mu10")) { input->select("Events/HLT_Mu20_Mu10", HLT_Mu20_Mu10); successBranches.push_back("Events/HLT_Mu20_Mu10"); } else { missingBranches.push_back("Events/HLT_Mu20_Mu10"); }
    if ( choose["Events/HLT_Mu20_Mu10_DZ"] )
      if (input->present("Events/HLT_Mu20_Mu10_DZ")) { input->select("Events/HLT_Mu20_Mu10_DZ", HLT_Mu20_Mu10_DZ); successBranches.push_back("Events/HLT_Mu20_Mu10_DZ"); } else { missingBranches.push_back("Events/HLT_Mu20_Mu10_DZ"); }
    if ( choose["Events/HLT_Mu20_Mu10_SameSign"] )
      if (input->present("Events/HLT_Mu20_Mu10_SameSign")) { input->select("Events/HLT_Mu20_Mu10_SameSign", HLT_Mu20_Mu10_SameSign); successBranches.push_back("Events/HLT_Mu20_Mu10_SameSign"); } else { missingBranches.push_back("Events/HLT_Mu20_Mu10_SameSign"); }
    if ( choose["Events/HLT_Mu20_Mu10_SameSign_DZ"] )
      if (input->present("Events/HLT_Mu20_Mu10_SameSign_DZ")) { input->select("Events/HLT_Mu20_Mu10_SameSign_DZ", HLT_Mu20_Mu10_SameSign_DZ); successBranches.push_back("Events/HLT_Mu20_Mu10_SameSign_DZ"); } else { missingBranches.push_back("Events/HLT_Mu20_Mu10_SameSign_DZ"); }
    if ( choose["Events/HLT_Mu20_TkMu0_Phi"] )
      if (input->present("Events/HLT_Mu20_TkMu0_Phi")) { input->select("Events/HLT_Mu20_TkMu0_Phi", HLT_Mu20_TkMu0_Phi); successBranches.push_back("Events/HLT_Mu20_TkMu0_Phi"); } else { missingBranches.push_back("Events/HLT_Mu20_TkMu0_Phi"); }
    if ( choose["Events/HLT_Mu23_Mu12"] )
      if (input->present("Events/HLT_Mu23_Mu12")) { input->select("Events/HLT_Mu23_Mu12", HLT_Mu23_Mu12); successBranches.push_back("Events/HLT_Mu23_Mu12"); } else { missingBranches.push_back("Events/HLT_Mu23_Mu12"); }
    if ( choose["Events/HLT_Mu23_Mu12_DZ"] )
      if (input->present("Events/HLT_Mu23_Mu12_DZ")) { input->select("Events/HLT_Mu23_Mu12_DZ", HLT_Mu23_Mu12_DZ); successBranches.push_back("Events/HLT_Mu23_Mu12_DZ"); } else { missingBranches.push_back("Events/HLT_Mu23_Mu12_DZ"); }
    if ( choose["Events/HLT_Mu23_Mu12_SameSign"] )
      if (input->present("Events/HLT_Mu23_Mu12_SameSign")) { input->select("Events/HLT_Mu23_Mu12_SameSign", HLT_Mu23_Mu12_SameSign); successBranches.push_back("Events/HLT_Mu23_Mu12_SameSign"); } else { missingBranches.push_back("Events/HLT_Mu23_Mu12_SameSign"); }
    if ( choose["Events/HLT_Mu23_Mu12_SameSign_DZ"] )
      if (input->present("Events/HLT_Mu23_Mu12_SameSign_DZ")) { input->select("Events/HLT_Mu23_Mu12_SameSign_DZ", HLT_Mu23_Mu12_SameSign_DZ); successBranches.push_back("Events/HLT_Mu23_Mu12_SameSign_DZ"); } else { missingBranches.push_back("Events/HLT_Mu23_Mu12_SameSign_DZ"); }
    if ( choose["Events/HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL"] )
      if (input->present("Events/HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL")) { input->select("Events/HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL", HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL); successBranches.push_back("Events/HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL"); } else { missingBranches.push_back("Events/HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL"); }
    if ( choose["Events/HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ"] )
      if (input->present("Events/HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ")) { input->select("Events/HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ", HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ); successBranches.push_back("Events/HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ"); } else { missingBranches.push_back("Events/HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ"); }
    if ( choose["Events/HLT_Mu25_TkMu0_Onia"] )
      if (input->present("Events/HLT_Mu25_TkMu0_Onia")) { input->select("Events/HLT_Mu25_TkMu0_Onia", HLT_Mu25_TkMu0_Onia); successBranches.push_back("Events/HLT_Mu25_TkMu0_Onia"); } else { missingBranches.push_back("Events/HLT_Mu25_TkMu0_Onia"); }
    if ( choose["Events/HLT_Mu25_TkMu0_Phi"] )
      if (input->present("Events/HLT_Mu25_TkMu0_Phi")) { input->select("Events/HLT_Mu25_TkMu0_Phi", HLT_Mu25_TkMu0_Phi); successBranches.push_back("Events/HLT_Mu25_TkMu0_Phi"); } else { missingBranches.push_back("Events/HLT_Mu25_TkMu0_Phi"); }
    if ( choose["Events/HLT_Mu27"] )
      if (input->present("Events/HLT_Mu27")) { input->select("Events/HLT_Mu27", HLT_Mu27); successBranches.push_back("Events/HLT_Mu27"); } else { missingBranches.push_back("Events/HLT_Mu27"); }
    if ( choose["Events/HLT_Mu27_Ele37_CaloIdL_MW"] )
      if (input->present("Events/HLT_Mu27_Ele37_CaloIdL_MW")) { input->select("Events/HLT_Mu27_Ele37_CaloIdL_MW", HLT_Mu27_Ele37_CaloIdL_MW); successBranches.push_back("Events/HLT_Mu27_Ele37_CaloIdL_MW"); } else { missingBranches.push_back("Events/HLT_Mu27_Ele37_CaloIdL_MW"); }
    if ( choose["Events/HLT_Mu30_TkMu0_Onia"] )
      if (input->present("Events/HLT_Mu30_TkMu0_Onia")) { input->select("Events/HLT_Mu30_TkMu0_Onia", HLT_Mu30_TkMu0_Onia); successBranches.push_back("Events/HLT_Mu30_TkMu0_Onia"); } else { missingBranches.push_back("Events/HLT_Mu30_TkMu0_Onia"); }
    if ( choose["Events/HLT_Mu37_Ele27_CaloIdL_MW"] )
      if (input->present("Events/HLT_Mu37_Ele27_CaloIdL_MW")) { input->select("Events/HLT_Mu37_Ele27_CaloIdL_MW", HLT_Mu37_Ele27_CaloIdL_MW); successBranches.push_back("Events/HLT_Mu37_Ele27_CaloIdL_MW"); } else { missingBranches.push_back("Events/HLT_Mu37_Ele27_CaloIdL_MW"); }
    if ( choose["Events/HLT_Mu37_TkMu27"] )
      if (input->present("Events/HLT_Mu37_TkMu27")) { input->select("Events/HLT_Mu37_TkMu27", HLT_Mu37_TkMu27); successBranches.push_back("Events/HLT_Mu37_TkMu27"); } else { missingBranches.push_back("Events/HLT_Mu37_TkMu27"); }
    if ( choose["Events/HLT_Mu3_PFJet40"] )
      if (input->present("Events/HLT_Mu3_PFJet40")) { input->select("Events/HLT_Mu3_PFJet40", HLT_Mu3_PFJet40); successBranches.push_back("Events/HLT_Mu3_PFJet40"); } else { missingBranches.push_back("Events/HLT_Mu3_PFJet40"); }
    if ( choose["Events/HLT_Mu43NoFiltersNoVtx_Photon43_CaloIdL"] )
      if (input->present("Events/HLT_Mu43NoFiltersNoVtx_Photon43_CaloIdL")) { input->select("Events/HLT_Mu43NoFiltersNoVtx_Photon43_CaloIdL", HLT_Mu43NoFiltersNoVtx_Photon43_CaloIdL); successBranches.push_back("Events/HLT_Mu43NoFiltersNoVtx_Photon43_CaloIdL"); } else { missingBranches.push_back("Events/HLT_Mu43NoFiltersNoVtx_Photon43_CaloIdL"); }
    if ( choose["Events/HLT_Mu48NoFiltersNoVtx_Photon48_CaloIdL"] )
      if (input->present("Events/HLT_Mu48NoFiltersNoVtx_Photon48_CaloIdL")) { input->select("Events/HLT_Mu48NoFiltersNoVtx_Photon48_CaloIdL", HLT_Mu48NoFiltersNoVtx_Photon48_CaloIdL); successBranches.push_back("Events/HLT_Mu48NoFiltersNoVtx_Photon48_CaloIdL"); } else { missingBranches.push_back("Events/HLT_Mu48NoFiltersNoVtx_Photon48_CaloIdL"); }
    if ( choose["Events/HLT_Mu50"] )
      if (input->present("Events/HLT_Mu50")) { input->select("Events/HLT_Mu50", HLT_Mu50); successBranches.push_back("Events/HLT_Mu50"); } else { missingBranches.push_back("Events/HLT_Mu50"); }
    if ( choose["Events/HLT_Mu50_IsoVVVL_PFHT450"] )
      if (input->present("Events/HLT_Mu50_IsoVVVL_PFHT450")) { input->select("Events/HLT_Mu50_IsoVVVL_PFHT450", HLT_Mu50_IsoVVVL_PFHT450); successBranches.push_back("Events/HLT_Mu50_IsoVVVL_PFHT450"); } else { missingBranches.push_back("Events/HLT_Mu50_IsoVVVL_PFHT450"); }
    if ( choose["Events/HLT_Mu55"] )
      if (input->present("Events/HLT_Mu55")) { input->select("Events/HLT_Mu55", HLT_Mu55); successBranches.push_back("Events/HLT_Mu55"); } else { missingBranches.push_back("Events/HLT_Mu55"); }
    if ( choose["Events/HLT_Mu7p5_L2Mu2_Jpsi"] )
      if (input->present("Events/HLT_Mu7p5_L2Mu2_Jpsi")) { input->select("Events/HLT_Mu7p5_L2Mu2_Jpsi", HLT_Mu7p5_L2Mu2_Jpsi); successBranches.push_back("Events/HLT_Mu7p5_L2Mu2_Jpsi"); } else { missingBranches.push_back("Events/HLT_Mu7p5_L2Mu2_Jpsi"); }
    if ( choose["Events/HLT_Mu7p5_L2Mu2_Upsilon"] )
      if (input->present("Events/HLT_Mu7p5_L2Mu2_Upsilon")) { input->select("Events/HLT_Mu7p5_L2Mu2_Upsilon", HLT_Mu7p5_L2Mu2_Upsilon); successBranches.push_back("Events/HLT_Mu7p5_L2Mu2_Upsilon"); } else { missingBranches.push_back("Events/HLT_Mu7p5_L2Mu2_Upsilon"); }
    if ( choose["Events/HLT_Mu7p5_Track2_Jpsi"] )
      if (input->present("Events/HLT_Mu7p5_Track2_Jpsi")) { input->select("Events/HLT_Mu7p5_Track2_Jpsi", HLT_Mu7p5_Track2_Jpsi); successBranches.push_back("Events/HLT_Mu7p5_Track2_Jpsi"); } else { missingBranches.push_back("Events/HLT_Mu7p5_Track2_Jpsi"); }
    if ( choose["Events/HLT_Mu7p5_Track2_Upsilon"] )
      if (input->present("Events/HLT_Mu7p5_Track2_Upsilon")) { input->select("Events/HLT_Mu7p5_Track2_Upsilon", HLT_Mu7p5_Track2_Upsilon); successBranches.push_back("Events/HLT_Mu7p5_Track2_Upsilon"); } else { missingBranches.push_back("Events/HLT_Mu7p5_Track2_Upsilon"); }
    if ( choose["Events/HLT_Mu7p5_Track3p5_Jpsi"] )
      if (input->present("Events/HLT_Mu7p5_Track3p5_Jpsi")) { input->select("Events/HLT_Mu7p5_Track3p5_Jpsi", HLT_Mu7p5_Track3p5_Jpsi); successBranches.push_back("Events/HLT_Mu7p5_Track3p5_Jpsi"); } else { missingBranches.push_back("Events/HLT_Mu7p5_Track3p5_Jpsi"); }
    if ( choose["Events/HLT_Mu7p5_Track3p5_Upsilon"] )
      if (input->present("Events/HLT_Mu7p5_Track3p5_Upsilon")) { input->select("Events/HLT_Mu7p5_Track3p5_Upsilon", HLT_Mu7p5_Track3p5_Upsilon); successBranches.push_back("Events/HLT_Mu7p5_Track3p5_Upsilon"); } else { missingBranches.push_back("Events/HLT_Mu7p5_Track3p5_Upsilon"); }
    if ( choose["Events/HLT_Mu7p5_Track7_Jpsi"] )
      if (input->present("Events/HLT_Mu7p5_Track7_Jpsi")) { input->select("Events/HLT_Mu7p5_Track7_Jpsi", HLT_Mu7p5_Track7_Jpsi); successBranches.push_back("Events/HLT_Mu7p5_Track7_Jpsi"); } else { missingBranches.push_back("Events/HLT_Mu7p5_Track7_Jpsi"); }
    if ( choose["Events/HLT_Mu7p5_Track7_Upsilon"] )
      if (input->present("Events/HLT_Mu7p5_Track7_Upsilon")) { input->select("Events/HLT_Mu7p5_Track7_Upsilon", HLT_Mu7p5_Track7_Upsilon); successBranches.push_back("Events/HLT_Mu7p5_Track7_Upsilon"); } else { missingBranches.push_back("Events/HLT_Mu7p5_Track7_Upsilon"); }
    if ( choose["Events/HLT_Mu8"] )
      if (input->present("Events/HLT_Mu8")) { input->select("Events/HLT_Mu8", HLT_Mu8); successBranches.push_back("Events/HLT_Mu8"); } else { missingBranches.push_back("Events/HLT_Mu8"); }
    if ( choose["Events/HLT_Mu8_DiEle12_CaloIdL_TrackIdL"] )
      if (input->present("Events/HLT_Mu8_DiEle12_CaloIdL_TrackIdL")) { input->select("Events/HLT_Mu8_DiEle12_CaloIdL_TrackIdL", HLT_Mu8_DiEle12_CaloIdL_TrackIdL); successBranches.push_back("Events/HLT_Mu8_DiEle12_CaloIdL_TrackIdL"); } else { missingBranches.push_back("Events/HLT_Mu8_DiEle12_CaloIdL_TrackIdL"); }
    if ( choose["Events/HLT_Mu8_DiEle12_CaloIdL_TrackIdL_DZ"] )
      if (input->present("Events/HLT_Mu8_DiEle12_CaloIdL_TrackIdL_DZ")) { input->select("Events/HLT_Mu8_DiEle12_CaloIdL_TrackIdL_DZ", HLT_Mu8_DiEle12_CaloIdL_TrackIdL_DZ); successBranches.push_back("Events/HLT_Mu8_DiEle12_CaloIdL_TrackIdL_DZ"); } else { missingBranches.push_back("Events/HLT_Mu8_DiEle12_CaloIdL_TrackIdL_DZ"); }
    if ( choose["Events/HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350"] )
      if (input->present("Events/HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350")) { input->select("Events/HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350", HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350); successBranches.push_back("Events/HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350"); } else { missingBranches.push_back("Events/HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350"); }
    if ( choose["Events/HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ"] )
      if (input->present("Events/HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ")) { input->select("Events/HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ", HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ); successBranches.push_back("Events/HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ"); } else { missingBranches.push_back("Events/HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ"); }
    if ( choose["Events/HLT_Mu8_TrkIsoVVL"] )
      if (input->present("Events/HLT_Mu8_TrkIsoVVL")) { input->select("Events/HLT_Mu8_TrkIsoVVL", HLT_Mu8_TrkIsoVVL); successBranches.push_back("Events/HLT_Mu8_TrkIsoVVL"); } else { missingBranches.push_back("Events/HLT_Mu8_TrkIsoVVL"); }
    if ( choose["Events/HLT_Mu8_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT300_PFMETNoMu60"] )
      if (input->present("Events/HLT_Mu8_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT300_PFMETNoMu60")) { input->select("Events/HLT_Mu8_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT300_PFMETNoMu60", HLT_Mu8_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT300_PFMETNoMu60); successBranches.push_back("Events/HLT_Mu8_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT300_PFMETNoMu60"); } else { missingBranches.push_back("Events/HLT_Mu8_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT300_PFMETNoMu60"); }
    if ( choose["Events/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL"] )
      if (input->present("Events/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL")) { input->select("Events/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL", HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL); successBranches.push_back("Events/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL"); } else { missingBranches.push_back("Events/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL"); }
    if ( choose["Events/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ"] )
      if (input->present("Events/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ")) { input->select("Events/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ", HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ); successBranches.push_back("Events/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ"); } else { missingBranches.push_back("Events/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ"); }
    if ( choose["Events/HLT_PFHT1050"] )
      if (input->present("Events/HLT_PFHT1050")) { input->select("Events/HLT_PFHT1050", HLT_PFHT1050); successBranches.push_back("Events/HLT_PFHT1050"); } else { missingBranches.push_back("Events/HLT_PFHT1050"); }
    if ( choose["Events/HLT_PFHT180"] )
      if (input->present("Events/HLT_PFHT180")) { input->select("Events/HLT_PFHT180", HLT_PFHT180); successBranches.push_back("Events/HLT_PFHT180"); } else { missingBranches.push_back("Events/HLT_PFHT180"); }
    if ( choose["Events/HLT_PFHT250"] )
      if (input->present("Events/HLT_PFHT250")) { input->select("Events/HLT_PFHT250", HLT_PFHT250); successBranches.push_back("Events/HLT_PFHT250"); } else { missingBranches.push_back("Events/HLT_PFHT250"); }
    if ( choose["Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40"] )
      if (input->present("Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40")) { input->select("Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40", HLT_PFHT300PT30_QuadPFJet_75_60_45_40); successBranches.push_back("Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40"); } else { missingBranches.push_back("Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40"); }
    if ( choose["Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0"] )
      if (input->present("Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0")) { input->select("Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0", HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0); successBranches.push_back("Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0"); } else { missingBranches.push_back("Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0"); }
    if ( choose["Events/HLT_PFHT350"] )
      if (input->present("Events/HLT_PFHT350")) { input->select("Events/HLT_PFHT350", HLT_PFHT350); successBranches.push_back("Events/HLT_PFHT350"); } else { missingBranches.push_back("Events/HLT_PFHT350"); }
    if ( choose["Events/HLT_PFHT350MinPFJet15"] )
      if (input->present("Events/HLT_PFHT350MinPFJet15")) { input->select("Events/HLT_PFHT350MinPFJet15", HLT_PFHT350MinPFJet15); successBranches.push_back("Events/HLT_PFHT350MinPFJet15"); } else { missingBranches.push_back("Events/HLT_PFHT350MinPFJet15"); }
    if ( choose["Events/HLT_PFHT370"] )
      if (input->present("Events/HLT_PFHT370")) { input->select("Events/HLT_PFHT370", HLT_PFHT370); successBranches.push_back("Events/HLT_PFHT370"); } else { missingBranches.push_back("Events/HLT_PFHT370"); }
    if ( choose["Events/HLT_PFHT380_SixJet32_DoubleBTagCSV_p075"] )
      if (input->present("Events/HLT_PFHT380_SixJet32_DoubleBTagCSV_p075")) { input->select("Events/HLT_PFHT380_SixJet32_DoubleBTagCSV_p075", HLT_PFHT380_SixJet32_DoubleBTagCSV_p075); successBranches.push_back("Events/HLT_PFHT380_SixJet32_DoubleBTagCSV_p075"); } else { missingBranches.push_back("Events/HLT_PFHT380_SixJet32_DoubleBTagCSV_p075"); }
    if ( choose["Events/HLT_PFHT380_SixPFJet32"] )
      if (input->present("Events/HLT_PFHT380_SixPFJet32")) { input->select("Events/HLT_PFHT380_SixPFJet32", HLT_PFHT380_SixPFJet32); successBranches.push_back("Events/HLT_PFHT380_SixPFJet32"); } else { missingBranches.push_back("Events/HLT_PFHT380_SixPFJet32"); }
    if ( choose["Events/HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2"] )
      if (input->present("Events/HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2")) { input->select("Events/HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2", HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2); successBranches.push_back("Events/HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2"); } else { missingBranches.push_back("Events/HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2"); }
    if ( choose["Events/HLT_PFHT380_SixPFJet32_DoublePFBTagDeepCSV_2p2"] )
      if (input->present("Events/HLT_PFHT380_SixPFJet32_DoublePFBTagDeepCSV_2p2")) { input->select("Events/HLT_PFHT380_SixPFJet32_DoublePFBTagDeepCSV_2p2", HLT_PFHT380_SixPFJet32_DoublePFBTagDeepCSV_2p2); successBranches.push_back("Events/HLT_PFHT380_SixPFJet32_DoublePFBTagDeepCSV_2p2"); } else { missingBranches.push_back("Events/HLT_PFHT380_SixPFJet32_DoublePFBTagDeepCSV_2p2"); }
    if ( choose["Events/HLT_PFHT430"] )
      if (input->present("Events/HLT_PFHT430")) { input->select("Events/HLT_PFHT430", HLT_PFHT430); successBranches.push_back("Events/HLT_PFHT430"); } else { missingBranches.push_back("Events/HLT_PFHT430"); }
    if ( choose["Events/HLT_PFHT430_SixJet40_BTagCSV_p080"] )
      if (input->present("Events/HLT_PFHT430_SixJet40_BTagCSV_p080")) { input->select("Events/HLT_PFHT430_SixJet40_BTagCSV_p080", HLT_PFHT430_SixJet40_BTagCSV_p080); successBranches.push_back("Events/HLT_PFHT430_SixJet40_BTagCSV_p080"); } else { missingBranches.push_back("Events/HLT_PFHT430_SixJet40_BTagCSV_p080"); }
    if ( choose["Events/HLT_PFHT430_SixPFJet40"] )
      if (input->present("Events/HLT_PFHT430_SixPFJet40")) { input->select("Events/HLT_PFHT430_SixPFJet40", HLT_PFHT430_SixPFJet40); successBranches.push_back("Events/HLT_PFHT430_SixPFJet40"); } else { missingBranches.push_back("Events/HLT_PFHT430_SixPFJet40"); }
    if ( choose["Events/HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5"] )
      if (input->present("Events/HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5")) { input->select("Events/HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5", HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5); successBranches.push_back("Events/HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5"); } else { missingBranches.push_back("Events/HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5"); }
    if ( choose["Events/HLT_PFHT500_PFMET100_PFMHT100_IDTight"] )
      if (input->present("Events/HLT_PFHT500_PFMET100_PFMHT100_IDTight")) { input->select("Events/HLT_PFHT500_PFMET100_PFMHT100_IDTight", HLT_PFHT500_PFMET100_PFMHT100_IDTight); successBranches.push_back("Events/HLT_PFHT500_PFMET100_PFMHT100_IDTight"); } else { missingBranches.push_back("Events/HLT_PFHT500_PFMET100_PFMHT100_IDTight"); }
    if ( choose["Events/HLT_PFHT500_PFMET110_PFMHT110_IDTight"] )
      if (input->present("Events/HLT_PFHT500_PFMET110_PFMHT110_IDTight")) { input->select("Events/HLT_PFHT500_PFMET110_PFMHT110_IDTight", HLT_PFHT500_PFMET110_PFMHT110_IDTight); successBranches.push_back("Events/HLT_PFHT500_PFMET110_PFMHT110_IDTight"); } else { missingBranches.push_back("Events/HLT_PFHT500_PFMET110_PFMHT110_IDTight"); }
    if ( choose["Events/HLT_PFHT510"] )
      if (input->present("Events/HLT_PFHT510")) { input->select("Events/HLT_PFHT510", HLT_PFHT510); successBranches.push_back("Events/HLT_PFHT510"); } else { missingBranches.push_back("Events/HLT_PFHT510"); }
    if ( choose["Events/HLT_PFHT590"] )
      if (input->present("Events/HLT_PFHT590")) { input->select("Events/HLT_PFHT590", HLT_PFHT590); successBranches.push_back("Events/HLT_PFHT590"); } else { missingBranches.push_back("Events/HLT_PFHT590"); }
    if ( choose["Events/HLT_PFHT680"] )
      if (input->present("Events/HLT_PFHT680")) { input->select("Events/HLT_PFHT680", HLT_PFHT680); successBranches.push_back("Events/HLT_PFHT680"); } else { missingBranches.push_back("Events/HLT_PFHT680"); }
    if ( choose["Events/HLT_PFHT700_PFMET85_PFMHT85_IDTight"] )
      if (input->present("Events/HLT_PFHT700_PFMET85_PFMHT85_IDTight")) { input->select("Events/HLT_PFHT700_PFMET85_PFMHT85_IDTight", HLT_PFHT700_PFMET85_PFMHT85_IDTight); successBranches.push_back("Events/HLT_PFHT700_PFMET85_PFMHT85_IDTight"); } else { missingBranches.push_back("Events/HLT_PFHT700_PFMET85_PFMHT85_IDTight"); }
    if ( choose["Events/HLT_PFHT700_PFMET95_PFMHT95_IDTight"] )
      if (input->present("Events/HLT_PFHT700_PFMET95_PFMHT95_IDTight")) { input->select("Events/HLT_PFHT700_PFMET95_PFMHT95_IDTight", HLT_PFHT700_PFMET95_PFMHT95_IDTight); successBranches.push_back("Events/HLT_PFHT700_PFMET95_PFMHT95_IDTight"); } else { missingBranches.push_back("Events/HLT_PFHT700_PFMET95_PFMHT95_IDTight"); }
    if ( choose["Events/HLT_PFHT780"] )
      if (input->present("Events/HLT_PFHT780")) { input->select("Events/HLT_PFHT780", HLT_PFHT780); successBranches.push_back("Events/HLT_PFHT780"); } else { missingBranches.push_back("Events/HLT_PFHT780"); }
    if ( choose["Events/HLT_PFHT800_PFMET75_PFMHT75_IDTight"] )
      if (input->present("Events/HLT_PFHT800_PFMET75_PFMHT75_IDTight")) { input->select("Events/HLT_PFHT800_PFMET75_PFMHT75_IDTight", HLT_PFHT800_PFMET75_PFMHT75_IDTight); successBranches.push_back("Events/HLT_PFHT800_PFMET75_PFMHT75_IDTight"); } else { missingBranches.push_back("Events/HLT_PFHT800_PFMET75_PFMHT75_IDTight"); }
    if ( choose["Events/HLT_PFHT800_PFMET85_PFMHT85_IDTight"] )
      if (input->present("Events/HLT_PFHT800_PFMET85_PFMHT85_IDTight")) { input->select("Events/HLT_PFHT800_PFMET85_PFMHT85_IDTight", HLT_PFHT800_PFMET85_PFMHT85_IDTight); successBranches.push_back("Events/HLT_PFHT800_PFMET85_PFMHT85_IDTight"); } else { missingBranches.push_back("Events/HLT_PFHT800_PFMET85_PFMHT85_IDTight"); }
    if ( choose["Events/HLT_PFHT890"] )
      if (input->present("Events/HLT_PFHT890")) { input->select("Events/HLT_PFHT890", HLT_PFHT890); successBranches.push_back("Events/HLT_PFHT890"); } else { missingBranches.push_back("Events/HLT_PFHT890"); }
    if ( choose["Events/HLT_QuadPFJet103_88_75_15"] )
      if (input->present("Events/HLT_QuadPFJet103_88_75_15")) { input->select("Events/HLT_QuadPFJet103_88_75_15", HLT_QuadPFJet103_88_75_15); successBranches.push_back("Events/HLT_QuadPFJet103_88_75_15"); } else { missingBranches.push_back("Events/HLT_QuadPFJet103_88_75_15"); }
    if ( choose["Events/HLT_QuadPFJet103_88_75_15_BTagCSV_p013_VBF2"] )
      if (input->present("Events/HLT_QuadPFJet103_88_75_15_BTagCSV_p013_VBF2")) { input->select("Events/HLT_QuadPFJet103_88_75_15_BTagCSV_p013_VBF2", HLT_QuadPFJet103_88_75_15_BTagCSV_p013_VBF2); successBranches.push_back("Events/HLT_QuadPFJet103_88_75_15_BTagCSV_p013_VBF2"); } else { missingBranches.push_back("Events/HLT_QuadPFJet103_88_75_15_BTagCSV_p013_VBF2"); }
    if ( choose["Events/HLT_QuadPFJet103_88_75_15_DoubleBTagCSV_p013_p08_VBF1"] )
      if (input->present("Events/HLT_QuadPFJet103_88_75_15_DoubleBTagCSV_p013_p08_VBF1")) { input->select("Events/HLT_QuadPFJet103_88_75_15_DoubleBTagCSV_p013_p08_VBF1", HLT_QuadPFJet103_88_75_15_DoubleBTagCSV_p013_p08_VBF1); successBranches.push_back("Events/HLT_QuadPFJet103_88_75_15_DoubleBTagCSV_p013_p08_VBF1"); } else { missingBranches.push_back("Events/HLT_QuadPFJet103_88_75_15_DoubleBTagCSV_p013_p08_VBF1"); }
    if ( choose["Events/HLT_QuadPFJet105_88_76_15"] )
      if (input->present("Events/HLT_QuadPFJet105_88_76_15")) { input->select("Events/HLT_QuadPFJet105_88_76_15", HLT_QuadPFJet105_88_76_15); successBranches.push_back("Events/HLT_QuadPFJet105_88_76_15"); } else { missingBranches.push_back("Events/HLT_QuadPFJet105_88_76_15"); }
    if ( choose["Events/HLT_QuadPFJet105_88_76_15_BTagCSV_p013_VBF2"] )
      if (input->present("Events/HLT_QuadPFJet105_88_76_15_BTagCSV_p013_VBF2")) { input->select("Events/HLT_QuadPFJet105_88_76_15_BTagCSV_p013_VBF2", HLT_QuadPFJet105_88_76_15_BTagCSV_p013_VBF2); successBranches.push_back("Events/HLT_QuadPFJet105_88_76_15_BTagCSV_p013_VBF2"); } else { missingBranches.push_back("Events/HLT_QuadPFJet105_88_76_15_BTagCSV_p013_VBF2"); }
    if ( choose["Events/HLT_QuadPFJet105_90_76_15_DoubleBTagCSV_p013_p08_VBF1"] )
      if (input->present("Events/HLT_QuadPFJet105_90_76_15_DoubleBTagCSV_p013_p08_VBF1")) { input->select("Events/HLT_QuadPFJet105_90_76_15_DoubleBTagCSV_p013_p08_VBF1", HLT_QuadPFJet105_90_76_15_DoubleBTagCSV_p013_p08_VBF1); successBranches.push_back("Events/HLT_QuadPFJet105_90_76_15_DoubleBTagCSV_p013_p08_VBF1"); } else { missingBranches.push_back("Events/HLT_QuadPFJet105_90_76_15_DoubleBTagCSV_p013_p08_VBF1"); }
    if ( choose["Events/HLT_QuadPFJet111_90_80_15"] )
      if (input->present("Events/HLT_QuadPFJet111_90_80_15")) { input->select("Events/HLT_QuadPFJet111_90_80_15", HLT_QuadPFJet111_90_80_15); successBranches.push_back("Events/HLT_QuadPFJet111_90_80_15"); } else { missingBranches.push_back("Events/HLT_QuadPFJet111_90_80_15"); }
    if ( choose["Events/HLT_QuadPFJet111_90_80_15_BTagCSV_p013_VBF2"] )
      if (input->present("Events/HLT_QuadPFJet111_90_80_15_BTagCSV_p013_VBF2")) { input->select("Events/HLT_QuadPFJet111_90_80_15_BTagCSV_p013_VBF2", HLT_QuadPFJet111_90_80_15_BTagCSV_p013_VBF2); successBranches.push_back("Events/HLT_QuadPFJet111_90_80_15_BTagCSV_p013_VBF2"); } else { missingBranches.push_back("Events/HLT_QuadPFJet111_90_80_15_BTagCSV_p013_VBF2"); }
    if ( choose["Events/HLT_QuadPFJet111_90_80_15_DoubleBTagCSV_p013_p08_VBF1"] )
      if (input->present("Events/HLT_QuadPFJet111_90_80_15_DoubleBTagCSV_p013_p08_VBF1")) { input->select("Events/HLT_QuadPFJet111_90_80_15_DoubleBTagCSV_p013_p08_VBF1", HLT_QuadPFJet111_90_80_15_DoubleBTagCSV_p013_p08_VBF1); successBranches.push_back("Events/HLT_QuadPFJet111_90_80_15_DoubleBTagCSV_p013_p08_VBF1"); } else { missingBranches.push_back("Events/HLT_QuadPFJet111_90_80_15_DoubleBTagCSV_p013_p08_VBF1"); }
    if ( choose["Events/HLT_QuadPFJet98_83_71_15"] )
      if (input->present("Events/HLT_QuadPFJet98_83_71_15")) { input->select("Events/HLT_QuadPFJet98_83_71_15", HLT_QuadPFJet98_83_71_15); successBranches.push_back("Events/HLT_QuadPFJet98_83_71_15"); } else { missingBranches.push_back("Events/HLT_QuadPFJet98_83_71_15"); }
    if ( choose["Events/HLT_QuadPFJet98_83_71_15_BTagCSV_p013_VBF2"] )
      if (input->present("Events/HLT_QuadPFJet98_83_71_15_BTagCSV_p013_VBF2")) { input->select("Events/HLT_QuadPFJet98_83_71_15_BTagCSV_p013_VBF2", HLT_QuadPFJet98_83_71_15_BTagCSV_p013_VBF2); successBranches.push_back("Events/HLT_QuadPFJet98_83_71_15_BTagCSV_p013_VBF2"); } else { missingBranches.push_back("Events/HLT_QuadPFJet98_83_71_15_BTagCSV_p013_VBF2"); }
    if ( choose["Events/HLT_QuadPFJet98_83_71_15_DoubleBTagCSV_p013_p08_VBF1"] )
      if (input->present("Events/HLT_QuadPFJet98_83_71_15_DoubleBTagCSV_p013_p08_VBF1")) { input->select("Events/HLT_QuadPFJet98_83_71_15_DoubleBTagCSV_p013_p08_VBF1", HLT_QuadPFJet98_83_71_15_DoubleBTagCSV_p013_p08_VBF1); successBranches.push_back("Events/HLT_QuadPFJet98_83_71_15_DoubleBTagCSV_p013_p08_VBF1"); } else { missingBranches.push_back("Events/HLT_QuadPFJet98_83_71_15_DoubleBTagCSV_p013_p08_VBF1"); }
    if ( choose["Events/Jet_area"] )
      if (input->present("Events/Jet_area")) { Jet_area.resize(62); input->select("Events/Jet_area", Jet_area); Jet_area.clear(); successBranches.push_back("Events/Jet_area"); } else { missingBranches.push_back("Events/Jet_area"); }
    if ( choose["Events/Jet_bRegCorr"] )
      if (input->present("Events/Jet_bRegCorr")) { Jet_bRegCorr.resize(62); input->select("Events/Jet_bRegCorr", Jet_bRegCorr); Jet_bRegCorr.clear(); successBranches.push_back("Events/Jet_bRegCorr"); } else { missingBranches.push_back("Events/Jet_bRegCorr"); }
    if ( choose["Events/Jet_bRegRes"] )
      if (input->present("Events/Jet_bRegRes")) { Jet_bRegRes.resize(62); input->select("Events/Jet_bRegRes", Jet_bRegRes); Jet_bRegRes.clear(); successBranches.push_back("Events/Jet_bRegRes"); } else { missingBranches.push_back("Events/Jet_bRegRes"); }
    if ( choose["Events/Jet_btagDeepFlavB"] )
      if (input->present("Events/Jet_btagDeepFlavB")) { Jet_btagDeepFlavB.resize(62); input->select("Events/Jet_btagDeepFlavB", Jet_btagDeepFlavB); Jet_btagDeepFlavB.clear(); successBranches.push_back("Events/Jet_btagDeepFlavB"); } else { missingBranches.push_back("Events/Jet_btagDeepFlavB"); }
    if ( choose["Events/Jet_btagDeepFlavCvB"] )
      if (input->present("Events/Jet_btagDeepFlavCvB")) { Jet_btagDeepFlavCvB.resize(62); input->select("Events/Jet_btagDeepFlavCvB", Jet_btagDeepFlavCvB); Jet_btagDeepFlavCvB.clear(); successBranches.push_back("Events/Jet_btagDeepFlavCvB"); } else { missingBranches.push_back("Events/Jet_btagDeepFlavCvB"); }
    if ( choose["Events/Jet_btagDeepFlavCvL"] )
      if (input->present("Events/Jet_btagDeepFlavCvL")) { Jet_btagDeepFlavCvL.resize(62); input->select("Events/Jet_btagDeepFlavCvL", Jet_btagDeepFlavCvL); Jet_btagDeepFlavCvL.clear(); successBranches.push_back("Events/Jet_btagDeepFlavCvL"); } else { missingBranches.push_back("Events/Jet_btagDeepFlavCvL"); }
    if ( choose["Events/Jet_btagDeepFlavQG"] )
      if (input->present("Events/Jet_btagDeepFlavQG")) { Jet_btagDeepFlavQG.resize(62); input->select("Events/Jet_btagDeepFlavQG", Jet_btagDeepFlavQG); Jet_btagDeepFlavQG.clear(); successBranches.push_back("Events/Jet_btagDeepFlavQG"); } else { missingBranches.push_back("Events/Jet_btagDeepFlavQG"); }
    if ( choose["Events/Jet_eta"] )
      if (input->present("Events/Jet_eta")) { Jet_eta.resize(62); input->select("Events/Jet_eta", Jet_eta); Jet_eta.clear(); successBranches.push_back("Events/Jet_eta"); } else { missingBranches.push_back("Events/Jet_eta"); }
    if ( choose["Events/Jet_genJetIdx"] )
      if (input->present("Events/Jet_genJetIdx")) { Jet_genJetIdx.resize(62); input->select("Events/Jet_genJetIdx", Jet_genJetIdx); Jet_genJetIdx.clear(); successBranches.push_back("Events/Jet_genJetIdx"); } else { missingBranches.push_back("Events/Jet_genJetIdx"); }
    if ( choose["Events/Jet_hadronFlavour"] )
      if (input->present("Events/Jet_hadronFlavour")) { Jet_hadronFlavour.resize(62); input->select("Events/Jet_hadronFlavour", Jet_hadronFlavour); Jet_hadronFlavour.clear(); successBranches.push_back("Events/Jet_hadronFlavour"); } else { missingBranches.push_back("Events/Jet_hadronFlavour"); }
    if ( choose["Events/Jet_jetId"] )
      if (input->present("Events/Jet_jetId")) { Jet_jetId.resize(62); input->select("Events/Jet_jetId", Jet_jetId); Jet_jetId.clear(); successBranches.push_back("Events/Jet_jetId"); } else { missingBranches.push_back("Events/Jet_jetId"); }
    if ( choose["Events/Jet_mass"] )
      if (input->present("Events/Jet_mass")) { Jet_mass.resize(62); input->select("Events/Jet_mass", Jet_mass); Jet_mass.clear(); successBranches.push_back("Events/Jet_mass"); } else { missingBranches.push_back("Events/Jet_mass"); }
    if ( choose["Events/Jet_partonFlavour"] )
      if (input->present("Events/Jet_partonFlavour")) { Jet_partonFlavour.resize(62); input->select("Events/Jet_partonFlavour", Jet_partonFlavour); Jet_partonFlavour.clear(); successBranches.push_back("Events/Jet_partonFlavour"); } else { missingBranches.push_back("Events/Jet_partonFlavour"); }
    if ( choose["Events/Jet_phi"] )
      if (input->present("Events/Jet_phi")) { Jet_phi.resize(62); input->select("Events/Jet_phi", Jet_phi); Jet_phi.clear(); successBranches.push_back("Events/Jet_phi"); } else { missingBranches.push_back("Events/Jet_phi"); }
    if ( choose["Events/Jet_pt"] )
      if (input->present("Events/Jet_pt")) { Jet_pt.resize(62); input->select("Events/Jet_pt", Jet_pt); Jet_pt.clear(); successBranches.push_back("Events/Jet_pt"); } else { missingBranches.push_back("Events/Jet_pt"); }
    if ( choose["Events/Jet_puId"] )
      if (input->present("Events/Jet_puId")) { Jet_puId.resize(62); input->select("Events/Jet_puId", Jet_puId); Jet_puId.clear(); successBranches.push_back("Events/Jet_puId"); } else { missingBranches.push_back("Events/Jet_puId"); }
    if ( choose["Events/Jet_qgl"] )
      if (input->present("Events/Jet_qgl")) { Jet_qgl.resize(62); input->select("Events/Jet_qgl", Jet_qgl); Jet_qgl.clear(); successBranches.push_back("Events/Jet_qgl"); } else { missingBranches.push_back("Events/Jet_qgl"); }
    if ( choose["Events/Jet_rawFactor"] )
      if (input->present("Events/Jet_rawFactor")) { Jet_rawFactor.resize(62); input->select("Events/Jet_rawFactor", Jet_rawFactor); Jet_rawFactor.clear(); successBranches.push_back("Events/Jet_rawFactor"); } else { missingBranches.push_back("Events/Jet_rawFactor"); }
    if ( choose["Events/L1PreFiringWeight_Dn"] )
      if (input->present("Events/L1PreFiringWeight_Dn")) { input->select("Events/L1PreFiringWeight_Dn", L1PreFiringWeight_Dn); successBranches.push_back("Events/L1PreFiringWeight_Dn"); } else { missingBranches.push_back("Events/L1PreFiringWeight_Dn"); }
    if ( choose["Events/L1PreFiringWeight_Nom"] )
      if (input->present("Events/L1PreFiringWeight_Nom")) { input->select("Events/L1PreFiringWeight_Nom", L1PreFiringWeight_Nom); successBranches.push_back("Events/L1PreFiringWeight_Nom"); } else { missingBranches.push_back("Events/L1PreFiringWeight_Nom"); }
    if ( choose["Events/L1PreFiringWeight_Up"] )
      if (input->present("Events/L1PreFiringWeight_Up")) { input->select("Events/L1PreFiringWeight_Up", L1PreFiringWeight_Up); successBranches.push_back("Events/L1PreFiringWeight_Up"); } else { missingBranches.push_back("Events/L1PreFiringWeight_Up"); }
    if ( choose["Events/LHEPdfWeight"] )
      if (input->present("Events/LHEPdfWeight")) { LHEPdfWeight.resize(147); input->select("Events/LHEPdfWeight", LHEPdfWeight); LHEPdfWeight.clear(); successBranches.push_back("Events/LHEPdfWeight"); } else { missingBranches.push_back("Events/LHEPdfWeight"); }
    if ( choose["Events/LHEScaleWeight"] )
      if (input->present("Events/LHEScaleWeight")) { LHEScaleWeight.resize(25); input->select("Events/LHEScaleWeight", LHEScaleWeight); LHEScaleWeight.clear(); successBranches.push_back("Events/LHEScaleWeight"); } else { missingBranches.push_back("Events/LHEScaleWeight"); }
    if ( choose["Events/MET_covXX"] )
      if (input->present("Events/MET_covXX")) { input->select("Events/MET_covXX", MET_covXX); successBranches.push_back("Events/MET_covXX"); } else { missingBranches.push_back("Events/MET_covXX"); }
    if ( choose["Events/MET_covXY"] )
      if (input->present("Events/MET_covXY")) { input->select("Events/MET_covXY", MET_covXY); successBranches.push_back("Events/MET_covXY"); } else { missingBranches.push_back("Events/MET_covXY"); }
    if ( choose["Events/MET_covYY"] )
      if (input->present("Events/MET_covYY")) { input->select("Events/MET_covYY", MET_covYY); successBranches.push_back("Events/MET_covYY"); } else { missingBranches.push_back("Events/MET_covYY"); }
    if ( choose["Events/MET_phi"] )
      if (input->present("Events/MET_phi")) { input->select("Events/MET_phi", MET_phi); successBranches.push_back("Events/MET_phi"); } else { missingBranches.push_back("Events/MET_phi"); }
    if ( choose["Events/MET_pt"] )
      if (input->present("Events/MET_pt")) { input->select("Events/MET_pt", MET_pt); successBranches.push_back("Events/MET_pt"); } else { missingBranches.push_back("Events/MET_pt"); }
    if ( choose["Events/MET_significance"] )
      if (input->present("Events/MET_significance")) { input->select("Events/MET_significance", MET_significance); successBranches.push_back("Events/MET_significance"); } else { missingBranches.push_back("Events/MET_significance"); }
    if ( choose["Events/Muon_charge"] )
      if (input->present("Events/Muon_charge")) { Muon_charge.resize(37); input->select("Events/Muon_charge", Muon_charge); Muon_charge.clear(); successBranches.push_back("Events/Muon_charge"); } else { missingBranches.push_back("Events/Muon_charge"); }
    if ( choose["Events/Muon_dxy"] )
      if (input->present("Events/Muon_dxy")) { Muon_dxy.resize(37); input->select("Events/Muon_dxy", Muon_dxy); Muon_dxy.clear(); successBranches.push_back("Events/Muon_dxy"); } else { missingBranches.push_back("Events/Muon_dxy"); }
    if ( choose["Events/Muon_dz"] )
      if (input->present("Events/Muon_dz")) { Muon_dz.resize(37); input->select("Events/Muon_dz", Muon_dz); Muon_dz.clear(); successBranches.push_back("Events/Muon_dz"); } else { missingBranches.push_back("Events/Muon_dz"); }
    if ( choose["Events/Muon_eta"] )
      if (input->present("Events/Muon_eta")) { Muon_eta.resize(37); input->select("Events/Muon_eta", Muon_eta); Muon_eta.clear(); successBranches.push_back("Events/Muon_eta"); } else { missingBranches.push_back("Events/Muon_eta"); }
    if ( choose["Events/Muon_genPartIdx"] )
      if (input->present("Events/Muon_genPartIdx")) { Muon_genPartIdx.resize(37); input->select("Events/Muon_genPartIdx", Muon_genPartIdx); Muon_genPartIdx.clear(); successBranches.push_back("Events/Muon_genPartIdx"); } else { missingBranches.push_back("Events/Muon_genPartIdx"); }
    if ( choose["Events/Muon_looseId"] )
      if (input->present("Events/Muon_looseId")) { Muon_looseId.resize(37); input->select("Events/Muon_looseId", Muon_looseId); Muon_looseId.clear(); successBranches.push_back("Events/Muon_looseId"); } else { missingBranches.push_back("Events/Muon_looseId"); }
    if ( choose["Events/Muon_mass"] )
      if (input->present("Events/Muon_mass")) { Muon_mass.resize(37); input->select("Events/Muon_mass", Muon_mass); Muon_mass.clear(); successBranches.push_back("Events/Muon_mass"); } else { missingBranches.push_back("Events/Muon_mass"); }
    if ( choose["Events/Muon_mediumId"] )
      if (input->present("Events/Muon_mediumId")) { Muon_mediumId.resize(37); input->select("Events/Muon_mediumId", Muon_mediumId); Muon_mediumId.clear(); successBranches.push_back("Events/Muon_mediumId"); } else { missingBranches.push_back("Events/Muon_mediumId"); }
    if ( choose["Events/Muon_miniPFRelIso_all"] )
      if (input->present("Events/Muon_miniPFRelIso_all")) { Muon_miniPFRelIso_all.resize(37); input->select("Events/Muon_miniPFRelIso_all", Muon_miniPFRelIso_all); Muon_miniPFRelIso_all.clear(); successBranches.push_back("Events/Muon_miniPFRelIso_all"); } else { missingBranches.push_back("Events/Muon_miniPFRelIso_all"); }
    if ( choose["Events/Muon_mvaTTH"] )
      if (input->present("Events/Muon_mvaTTH")) { Muon_mvaTTH.resize(37); input->select("Events/Muon_mvaTTH", Muon_mvaTTH); Muon_mvaTTH.clear(); successBranches.push_back("Events/Muon_mvaTTH"); } else { missingBranches.push_back("Events/Muon_mvaTTH"); }
    if ( choose["Events/Muon_pfRelIso04_all"] )
      if (input->present("Events/Muon_pfRelIso04_all")) { Muon_pfRelIso04_all.resize(37); input->select("Events/Muon_pfRelIso04_all", Muon_pfRelIso04_all); Muon_pfRelIso04_all.clear(); successBranches.push_back("Events/Muon_pfRelIso04_all"); } else { missingBranches.push_back("Events/Muon_pfRelIso04_all"); }
    if ( choose["Events/Muon_phi"] )
      if (input->present("Events/Muon_phi")) { Muon_phi.resize(37); input->select("Events/Muon_phi", Muon_phi); Muon_phi.clear(); successBranches.push_back("Events/Muon_phi"); } else { missingBranches.push_back("Events/Muon_phi"); }
    if ( choose["Events/Muon_pt"] )
      if (input->present("Events/Muon_pt")) { Muon_pt.resize(37); input->select("Events/Muon_pt", Muon_pt); Muon_pt.clear(); successBranches.push_back("Events/Muon_pt"); } else { missingBranches.push_back("Events/Muon_pt"); }
    if ( choose["Events/Muon_tightId"] )
      if (input->present("Events/Muon_tightId")) { Muon_tightId.resize(37); input->select("Events/Muon_tightId", Muon_tightId); Muon_tightId.clear(); successBranches.push_back("Events/Muon_tightId"); } else { missingBranches.push_back("Events/Muon_tightId"); }
    if ( choose["Events/PV_chi2"] )
      if (input->present("Events/PV_chi2")) { input->select("Events/PV_chi2", PV_chi2); successBranches.push_back("Events/PV_chi2"); } else { missingBranches.push_back("Events/PV_chi2"); }
    if ( choose["Events/PV_ndof"] )
      if (input->present("Events/PV_ndof")) { input->select("Events/PV_ndof", PV_ndof); successBranches.push_back("Events/PV_ndof"); } else { missingBranches.push_back("Events/PV_ndof"); }
    if ( choose["Events/PV_npvs"] )
      if (input->present("Events/PV_npvs")) { input->select("Events/PV_npvs", PV_npvs); successBranches.push_back("Events/PV_npvs"); } else { missingBranches.push_back("Events/PV_npvs"); }
    if ( choose["Events/PV_npvsGood"] )
      if (input->present("Events/PV_npvsGood")) { input->select("Events/PV_npvsGood", PV_npvsGood); successBranches.push_back("Events/PV_npvsGood"); } else { missingBranches.push_back("Events/PV_npvsGood"); }
    if ( choose["Events/PV_z"] )
      if (input->present("Events/PV_z")) { input->select("Events/PV_z", PV_z); successBranches.push_back("Events/PV_z"); } else { missingBranches.push_back("Events/PV_z"); }
    if ( choose["Events/Pileup_nPU"] )
      if (input->present("Events/Pileup_nPU")) { input->select("Events/Pileup_nPU", Pileup_nPU); successBranches.push_back("Events/Pileup_nPU"); } else { missingBranches.push_back("Events/Pileup_nPU"); }
    if ( choose["Events/Pileup_nTrueInt"] )
      if (input->present("Events/Pileup_nTrueInt")) { input->select("Events/Pileup_nTrueInt", Pileup_nTrueInt); successBranches.push_back("Events/Pileup_nTrueInt"); } else { missingBranches.push_back("Events/Pileup_nTrueInt"); }
    if ( choose["Events/PuppiMET_phi"] )
      if (input->present("Events/PuppiMET_phi")) { input->select("Events/PuppiMET_phi", PuppiMET_phi); successBranches.push_back("Events/PuppiMET_phi"); } else { missingBranches.push_back("Events/PuppiMET_phi"); }
    if ( choose["Events/PuppiMET_phiJERDown"] )
      if (input->present("Events/PuppiMET_phiJERDown")) { input->select("Events/PuppiMET_phiJERDown", PuppiMET_phiJERDown); successBranches.push_back("Events/PuppiMET_phiJERDown"); } else { missingBranches.push_back("Events/PuppiMET_phiJERDown"); }
    if ( choose["Events/PuppiMET_phiJERUp"] )
      if (input->present("Events/PuppiMET_phiJERUp")) { input->select("Events/PuppiMET_phiJERUp", PuppiMET_phiJERUp); successBranches.push_back("Events/PuppiMET_phiJERUp"); } else { missingBranches.push_back("Events/PuppiMET_phiJERUp"); }
    if ( choose["Events/PuppiMET_phiJESDown"] )
      if (input->present("Events/PuppiMET_phiJESDown")) { input->select("Events/PuppiMET_phiJESDown", PuppiMET_phiJESDown); successBranches.push_back("Events/PuppiMET_phiJESDown"); } else { missingBranches.push_back("Events/PuppiMET_phiJESDown"); }
    if ( choose["Events/PuppiMET_phiJESUp"] )
      if (input->present("Events/PuppiMET_phiJESUp")) { input->select("Events/PuppiMET_phiJESUp", PuppiMET_phiJESUp); successBranches.push_back("Events/PuppiMET_phiJESUp"); } else { missingBranches.push_back("Events/PuppiMET_phiJESUp"); }
    if ( choose["Events/PuppiMET_phiUnclusteredDown"] )
      if (input->present("Events/PuppiMET_phiUnclusteredDown")) { input->select("Events/PuppiMET_phiUnclusteredDown", PuppiMET_phiUnclusteredDown); successBranches.push_back("Events/PuppiMET_phiUnclusteredDown"); } else { missingBranches.push_back("Events/PuppiMET_phiUnclusteredDown"); }
    if ( choose["Events/PuppiMET_phiUnclusteredUp"] )
      if (input->present("Events/PuppiMET_phiUnclusteredUp")) { input->select("Events/PuppiMET_phiUnclusteredUp", PuppiMET_phiUnclusteredUp); successBranches.push_back("Events/PuppiMET_phiUnclusteredUp"); } else { missingBranches.push_back("Events/PuppiMET_phiUnclusteredUp"); }
    if ( choose["Events/PuppiMET_pt"] )
      if (input->present("Events/PuppiMET_pt")) { input->select("Events/PuppiMET_pt", PuppiMET_pt); successBranches.push_back("Events/PuppiMET_pt"); } else { missingBranches.push_back("Events/PuppiMET_pt"); }
    if ( choose["Events/PuppiMET_ptJERDown"] )
      if (input->present("Events/PuppiMET_ptJERDown")) { input->select("Events/PuppiMET_ptJERDown", PuppiMET_ptJERDown); successBranches.push_back("Events/PuppiMET_ptJERDown"); } else { missingBranches.push_back("Events/PuppiMET_ptJERDown"); }
    if ( choose["Events/PuppiMET_ptJERUp"] )
      if (input->present("Events/PuppiMET_ptJERUp")) { input->select("Events/PuppiMET_ptJERUp", PuppiMET_ptJERUp); successBranches.push_back("Events/PuppiMET_ptJERUp"); } else { missingBranches.push_back("Events/PuppiMET_ptJERUp"); }
    if ( choose["Events/PuppiMET_ptJESDown"] )
      if (input->present("Events/PuppiMET_ptJESDown")) { input->select("Events/PuppiMET_ptJESDown", PuppiMET_ptJESDown); successBranches.push_back("Events/PuppiMET_ptJESDown"); } else { missingBranches.push_back("Events/PuppiMET_ptJESDown"); }
    if ( choose["Events/PuppiMET_ptJESUp"] )
      if (input->present("Events/PuppiMET_ptJESUp")) { input->select("Events/PuppiMET_ptJESUp", PuppiMET_ptJESUp); successBranches.push_back("Events/PuppiMET_ptJESUp"); } else { missingBranches.push_back("Events/PuppiMET_ptJESUp"); }
    if ( choose["Events/PuppiMET_ptUnclusteredDown"] )
      if (input->present("Events/PuppiMET_ptUnclusteredDown")) { input->select("Events/PuppiMET_ptUnclusteredDown", PuppiMET_ptUnclusteredDown); successBranches.push_back("Events/PuppiMET_ptUnclusteredDown"); } else { missingBranches.push_back("Events/PuppiMET_ptUnclusteredDown"); }
    if ( choose["Events/PuppiMET_ptUnclusteredUp"] )
      if (input->present("Events/PuppiMET_ptUnclusteredUp")) { input->select("Events/PuppiMET_ptUnclusteredUp", PuppiMET_ptUnclusteredUp); successBranches.push_back("Events/PuppiMET_ptUnclusteredUp"); } else { missingBranches.push_back("Events/PuppiMET_ptUnclusteredUp"); }
    if ( choose["Events/PuppiMET_sumEt"] )
      if (input->present("Events/PuppiMET_sumEt")) { input->select("Events/PuppiMET_sumEt", PuppiMET_sumEt); successBranches.push_back("Events/PuppiMET_sumEt"); } else { missingBranches.push_back("Events/PuppiMET_sumEt"); }
    if ( choose["Events/event"] )
      if (input->present("Events/event")) { input->select("Events/event", event); successBranches.push_back("Events/event"); } else { missingBranches.push_back("Events/event"); }
    if ( choose["Events/fixedGridRhoFastjetAll"] )
      if (input->present("Events/fixedGridRhoFastjetAll")) { input->select("Events/fixedGridRhoFastjetAll", fixedGridRhoFastjetAll); successBranches.push_back("Events/fixedGridRhoFastjetAll"); } else { missingBranches.push_back("Events/fixedGridRhoFastjetAll"); }
    if ( choose["Events/genTtbarId"] )
      if (input->present("Events/genTtbarId")) { input->select("Events/genTtbarId", genTtbarId); successBranches.push_back("Events/genTtbarId"); } else { missingBranches.push_back("Events/genTtbarId"); }
    if ( choose["Events/genWeight"] )
      if (input->present("Events/genWeight")) { input->select("Events/genWeight", genWeight); successBranches.push_back("Events/genWeight"); } else { missingBranches.push_back("Events/genWeight"); }
    if ( choose["Events/luminosityBlock"] )
      if (input->present("Events/luminosityBlock")) { input->select("Events/luminosityBlock", luminosityBlock); successBranches.push_back("Events/luminosityBlock"); } else { missingBranches.push_back("Events/luminosityBlock"); }
    if ( choose["Events/run"] )
      if (input->present("Events/run")) { input->select("Events/run", run); successBranches.push_back("Events/run"); } else { missingBranches.push_back("Events/run"); }
    if ( choose["Events/ttCatSource"] )
      if (input->present("Events/ttCatSource")) { input->select("Events/ttCatSource", ttCatSource); successBranches.push_back("Events/ttCatSource"); } else { missingBranches.push_back("Events/ttCatSource"); }
    if ( choose["Events/ttCatXvalSource"] )
      if (input->present("Events/ttCatXvalSource")) { input->select("Events/ttCatXvalSource", ttCatXvalSource); successBranches.push_back("Events/ttCatXvalSource"); } else { missingBranches.push_back("Events/ttCatXvalSource"); }
    if ( choose["Events/ttCatXval_Add1Bjet_1Had"] )
      if (input->present("Events/ttCatXval_Add1Bjet_1Had")) { input->select("Events/ttCatXval_Add1Bjet_1Had", ttCatXval_Add1Bjet_1Had); successBranches.push_back("Events/ttCatXval_Add1Bjet_1Had"); } else { missingBranches.push_back("Events/ttCatXval_Add1Bjet_1Had"); }
    if ( choose["Events/ttCatXval_Add1Bjet_2Had"] )
      if (input->present("Events/ttCatXval_Add1Bjet_2Had")) { input->select("Events/ttCatXval_Add1Bjet_2Had", ttCatXval_Add1Bjet_2Had); successBranches.push_back("Events/ttCatXval_Add1Bjet_2Had"); } else { missingBranches.push_back("Events/ttCatXval_Add1Bjet_2Had"); }
    if ( choose["Events/ttCatXval_Add2Bjet"] )
      if (input->present("Events/ttCatXval_Add2Bjet")) { input->select("Events/ttCatXval_Add2Bjet", ttCatXval_Add2Bjet); successBranches.push_back("Events/ttCatXval_Add2Bjet"); } else { missingBranches.push_back("Events/ttCatXval_Add2Bjet"); }
    if ( choose["Events/ttCatXval_AddCjet"] )
      if (input->present("Events/ttCatXval_AddCjet")) { input->select("Events/ttCatXval_AddCjet", ttCatXval_AddCjet); successBranches.push_back("Events/ttCatXval_AddCjet"); } else { missingBranches.push_back("Events/ttCatXval_AddCjet"); }
    if ( choose["Events/ttCatXval_LightFlavour"] )
      if (input->present("Events/ttCatXval_LightFlavour")) { input->select("Events/ttCatXval_LightFlavour", ttCatXval_LightFlavour); successBranches.push_back("Events/ttCatXval_LightFlavour"); } else { missingBranches.push_back("Events/ttCatXval_LightFlavour"); }
    if ( choose["Events/ttCat_Add1Bjet_1Had"] )
      if (input->present("Events/ttCat_Add1Bjet_1Had")) { input->select("Events/ttCat_Add1Bjet_1Had", ttCat_Add1Bjet_1Had); successBranches.push_back("Events/ttCat_Add1Bjet_1Had"); } else { missingBranches.push_back("Events/ttCat_Add1Bjet_1Had"); }
    if ( choose["Events/ttCat_Add1Bjet_2Had"] )
      if (input->present("Events/ttCat_Add1Bjet_2Had")) { input->select("Events/ttCat_Add1Bjet_2Had", ttCat_Add1Bjet_2Had); successBranches.push_back("Events/ttCat_Add1Bjet_2Had"); } else { missingBranches.push_back("Events/ttCat_Add1Bjet_2Had"); }
    if ( choose["Events/ttCat_Add2Bjet"] )
      if (input->present("Events/ttCat_Add2Bjet")) { input->select("Events/ttCat_Add2Bjet", ttCat_Add2Bjet); successBranches.push_back("Events/ttCat_Add2Bjet"); } else { missingBranches.push_back("Events/ttCat_Add2Bjet"); }
    if ( choose["Events/ttCat_AddCjet"] )
      if (input->present("Events/ttCat_AddCjet")) { input->select("Events/ttCat_AddCjet", ttCat_AddCjet); successBranches.push_back("Events/ttCat_AddCjet"); } else { missingBranches.push_back("Events/ttCat_AddCjet"); }
    if ( choose["Events/ttCat_LightFlavour"] )
      if (input->present("Events/ttCat_LightFlavour")) { input->select("Events/ttCat_LightFlavour", ttCat_LightFlavour); successBranches.push_back("Events/ttCat_LightFlavour"); } else { missingBranches.push_back("Events/ttCat_LightFlavour"); }


    // --- Branch Access Report ---
    std::cout << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "  eventBuffer Branch Access Report" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "  [OK]      " << successBranches.size()
              << " branches connected" << std::endl;
    std::cout << "  [MISSING] " << missingBranches.size()
              << " branches not found in file" << std::endl;
    if ( missingBranches.size() > 0 )
      {
        std::cout << std::endl;
        std::cout << "  Missing branches (skipped, filled with 0/empty):"
                  << std::endl;
        for (size_t i = 0; i < missingBranches.size(); ++i)
          {
            std::cout << "    - " << missingBranches[i] << std::endl;
          }
        std::cout << std::endl;
        std::cout << "  NOTE: Missing branches are expected when using a"
                  << std::endl;
        std::cout << "  Super-Set variables.txt across Data/MC or different"
                  << std::endl;
        std::cout << "  data-taking periods. Scalars default to 0,"
                  << std::endl;
        std::cout << "  vectors remain empty (size=0)." << std::endl;
      }
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;
  }

  // A write-only buffer
  eventBuffer(otreestream& stream)
  : input(0),
    output(&stream)
  {
    initBuffers();

    output->add("nElectron", 	nElectron);
    output->add("nMuon", 	nMuon);
    output->add("nLHEPdfWeight", 	nLHEPdfWeight);
    output->add("nJet", 	nJet);
    output->add("nFatJet", 	nFatJet);
    output->add("nGenJet", 	nGenJet);
    output->add("nGenPart", 	nGenPart);
    output->add("nLHEScaleWeight", 	nLHEScaleWeight);
  
    output->add("Events/Electron_charge[nElectron]", 	Electron_charge);
    output->add("Events/Electron_cutBased[nElectron]", 	Electron_cutBased);
    output->add("Events/Electron_deltaEtaSC[nElectron]",
                 Electron_deltaEtaSC);
    output->add("Events/Electron_dxy[nElectron]", 	Electron_dxy);
    output->add("Events/Electron_dz[nElectron]", 	Electron_dz);
    output->add("Events/Electron_eta[nElectron]", 	Electron_eta);
    output->add("Events/Electron_genPartIdx[nElectron]",
                 Electron_genPartIdx);
    output->add("Events/Electron_mass[nElectron]", 	Electron_mass);
    output->add("Events/Electron_miniPFRelIso_all[nElectron]",
                 Electron_miniPFRelIso_all);
    output->add("Events/Electron_mvaFall17V2Iso_WP80[nElectron]",
                 Electron_mvaFall17V2Iso_WP80);
    output->add("Events/Electron_mvaFall17V2Iso_WP90[nElectron]",
                 Electron_mvaFall17V2Iso_WP90);
    output->add("Events/Electron_mvaTTH[nElectron]", 	Electron_mvaTTH);
    output->add("Events/Electron_pfRelIso03_all[nElectron]",
                 Electron_pfRelIso03_all);
    output->add("Events/Electron_phi[nElectron]", 	Electron_phi);
    output->add("Events/Electron_pt[nElectron]", 	Electron_pt);
    output->add("Events/FatJet_area[nFatJet]", 	FatJet_area);
    output->add("Events/FatJet_deepTagMD_H4qvsQCD[nFatJet]",
                 FatJet_deepTagMD_H4qvsQCD);
    output->add("Events/FatJet_deepTagMD_HbbvsQCD[nFatJet]",
                 FatJet_deepTagMD_HbbvsQCD);
    output->add("Events/FatJet_deepTagMD_TvsQCD[nFatJet]",
                 FatJet_deepTagMD_TvsQCD);
    output->add("Events/FatJet_deepTagMD_WvsQCD[nFatJet]",
                 FatJet_deepTagMD_WvsQCD);
    output->add("Events/FatJet_deepTagMD_ZHbbvsQCD[nFatJet]",
                 FatJet_deepTagMD_ZHbbvsQCD);
    output->add("Events/FatJet_deepTagMD_ZHccvsQCD[nFatJet]",
                 FatJet_deepTagMD_ZHccvsQCD);
    output->add("Events/FatJet_deepTagMD_ZbbvsQCD[nFatJet]",
                 FatJet_deepTagMD_ZbbvsQCD);
    output->add("Events/FatJet_deepTagMD_ZvsQCD[nFatJet]",
                 FatJet_deepTagMD_ZvsQCD);
    output->add("Events/FatJet_deepTagMD_bbvsLight[nFatJet]",
                 FatJet_deepTagMD_bbvsLight);
    output->add("Events/FatJet_deepTagMD_ccvsLight[nFatJet]",
                 FatJet_deepTagMD_ccvsLight);
    output->add("Events/FatJet_deepTag_H[nFatJet]", 	FatJet_deepTag_H);
    output->add("Events/FatJet_deepTag_QCD[nFatJet]", 	FatJet_deepTag_QCD);
    output->add("Events/FatJet_deepTag_QCDothers[nFatJet]",
                 FatJet_deepTag_QCDothers);
    output->add("Events/FatJet_deepTag_TvsQCD[nFatJet]",
                 FatJet_deepTag_TvsQCD);
    output->add("Events/FatJet_deepTag_WvsQCD[nFatJet]",
                 FatJet_deepTag_WvsQCD);
    output->add("Events/FatJet_deepTag_ZvsQCD[nFatJet]",
                 FatJet_deepTag_ZvsQCD);
    output->add("Events/FatJet_eta[nFatJet]", 	FatJet_eta);
    output->add("Events/FatJet_jetId[nFatJet]", 	FatJet_jetId);
    output->add("Events/FatJet_mass[nFatJet]", 	FatJet_mass);
    output->add("Events/FatJet_msoftdrop[nFatJet]", 	FatJet_msoftdrop);
    output->add("Events/FatJet_n2b1[nFatJet]", 	FatJet_n2b1);
    output->add("Events/FatJet_n3b1[nFatJet]", 	FatJet_n3b1);
    output->add("Events/FatJet_particleNetMD_QCD[nFatJet]",
                 FatJet_particleNetMD_QCD);
    output->add("Events/FatJet_particleNetMD_Xbb[nFatJet]",
                 FatJet_particleNetMD_Xbb);
    output->add("Events/FatJet_particleNetMD_Xcc[nFatJet]",
                 FatJet_particleNetMD_Xcc);
    output->add("Events/FatJet_particleNetMD_Xqq[nFatJet]",
                 FatJet_particleNetMD_Xqq);
    output->add("Events/FatJet_particleNet_HbbvsQCD[nFatJet]",
                 FatJet_particleNet_HbbvsQCD);
    output->add("Events/FatJet_particleNet_TvsQCD[nFatJet]",
                 FatJet_particleNet_TvsQCD);
    output->add("Events/FatJet_particleNet_WvsQCD[nFatJet]",
                 FatJet_particleNet_WvsQCD);
    output->add("Events/FatJet_particleNet_ZvsQCD[nFatJet]",
                 FatJet_particleNet_ZvsQCD);
    output->add("Events/FatJet_particleNet_mass[nFatJet]",
                 FatJet_particleNet_mass);
    output->add("Events/FatJet_phi[nFatJet]", 	FatJet_phi);
    output->add("Events/FatJet_pt[nFatJet]", 	FatJet_pt);
    output->add("Events/FatJet_rawFactor[nFatJet]", 	FatJet_rawFactor);
    output->add("Events/FatJet_tau1[nFatJet]", 	FatJet_tau1);
    output->add("Events/FatJet_tau2[nFatJet]", 	FatJet_tau2);
    output->add("Events/FatJet_tau3[nFatJet]", 	FatJet_tau3);
    output->add("Events/FatJet_tau4[nFatJet]", 	FatJet_tau4);
    output->add("Events/Flag_BadPFMuonDzFilter", 	Flag_BadPFMuonDzFilter);
    output->add("Events/Flag_BadPFMuonFilter", 	Flag_BadPFMuonFilter);
    output->add("Events/Flag_EcalDeadCellTriggerPrimitiveFilter",
                 Flag_EcalDeadCellTriggerPrimitiveFilter);
    output->add("Events/Flag_HBHENoiseFilter", 	Flag_HBHENoiseFilter);
    output->add("Events/Flag_HBHENoiseIsoFilter", 	Flag_HBHENoiseIsoFilter);
    output->add("Events/Flag_ecalBadCalibFilter", 	Flag_ecalBadCalibFilter);
    output->add("Events/Flag_eeBadScFilter", 	Flag_eeBadScFilter);
    output->add("Events/Flag_globalSuperTightHalo2016Filter",
                 Flag_globalSuperTightHalo2016Filter);
    output->add("Events/Flag_goodVertices", 	Flag_goodVertices);
    output->add("Events/GenJet_eta[nGenJet]", 	GenJet_eta);
    output->add("Events/GenJet_hadronFlavour[nGenJet]",
                 GenJet_hadronFlavour);
    output->add("Events/GenJet_mass[nGenJet]", 	GenJet_mass);
    output->add("Events/GenJet_partonFlavour[nGenJet]",
                 GenJet_partonFlavour);
    output->add("Events/GenJet_phi[nGenJet]", 	GenJet_phi);
    output->add("Events/GenJet_pt[nGenJet]", 	GenJet_pt);
    output->add("Events/GenPart_eta[nGenPart]", 	GenPart_eta);
    output->add("Events/GenPart_genPartIdxMother[nGenPart]",
                 GenPart_genPartIdxMother);
    output->add("Events/GenPart_mass[nGenPart]", 	GenPart_mass);
    output->add("Events/GenPart_pdgId[nGenPart]", 	GenPart_pdgId);
    output->add("Events/GenPart_phi[nGenPart]", 	GenPart_phi);
    output->add("Events/GenPart_pt[nGenPart]", 	GenPart_pt);
    output->add("Events/GenPart_status[nGenPart]", 	GenPart_status);
    output->add("Events/GenPart_statusFlags[nGenPart]",
                 GenPart_statusFlags);
    output->add("Events/Generator_weight", 	Generator_weight);
    output->add("Events/HLT_AK8PFHT750_TrimMass50",
                 HLT_AK8PFHT750_TrimMass50);
    output->add("Events/HLT_AK8PFHT800_TrimMass50",
                 HLT_AK8PFHT800_TrimMass50);
    output->add("Events/HLT_AK8PFHT850_TrimMass50",
                 HLT_AK8PFHT850_TrimMass50);
    output->add("Events/HLT_AK8PFHT900_TrimMass50",
                 HLT_AK8PFHT900_TrimMass50);
    output->add("Events/HLT_AK8PFJet140", 	HLT_AK8PFJet140);
    output->add("Events/HLT_AK8PFJet200", 	HLT_AK8PFJet200);
    output->add("Events/HLT_AK8PFJet260", 	HLT_AK8PFJet260);
    output->add("Events/HLT_AK8PFJet320", 	HLT_AK8PFJet320);
    output->add("Events/HLT_AK8PFJet330_PFAK8BTagCSV_p1",
                 HLT_AK8PFJet330_PFAK8BTagCSV_p1);
    output->add("Events/HLT_AK8PFJet330_PFAK8BTagCSV_p17",
                 HLT_AK8PFJet330_PFAK8BTagCSV_p17);
    output->add("Events/HLT_AK8PFJet360_TrimMass30",
                 HLT_AK8PFJet360_TrimMass30);
    output->add("Events/HLT_AK8PFJet380_TrimMass30",
                 HLT_AK8PFJet380_TrimMass30);
    output->add("Events/HLT_AK8PFJet40", 	HLT_AK8PFJet40);
    output->add("Events/HLT_AK8PFJet400", 	HLT_AK8PFJet400);
    output->add("Events/HLT_AK8PFJet400_TrimMass30",
                 HLT_AK8PFJet400_TrimMass30);
    output->add("Events/HLT_AK8PFJet420_TrimMass30",
                 HLT_AK8PFJet420_TrimMass30);
    output->add("Events/HLT_AK8PFJet450", 	HLT_AK8PFJet450);
    output->add("Events/HLT_AK8PFJet500", 	HLT_AK8PFJet500);
    output->add("Events/HLT_AK8PFJet550", 	HLT_AK8PFJet550);
    output->add("Events/HLT_AK8PFJet60", 	HLT_AK8PFJet60);
    output->add("Events/HLT_AK8PFJet80", 	HLT_AK8PFJet80);
    output->add("Events/HLT_AK8PFJetFwd140", 	HLT_AK8PFJetFwd140);
    output->add("Events/HLT_AK8PFJetFwd200", 	HLT_AK8PFJetFwd200);
    output->add("Events/HLT_AK8PFJetFwd260", 	HLT_AK8PFJetFwd260);
    output->add("Events/HLT_AK8PFJetFwd320", 	HLT_AK8PFJetFwd320);
    output->add("Events/HLT_AK8PFJetFwd40", 	HLT_AK8PFJetFwd40);
    output->add("Events/HLT_AK8PFJetFwd400", 	HLT_AK8PFJetFwd400);
    output->add("Events/HLT_AK8PFJetFwd450", 	HLT_AK8PFJetFwd450);
    output->add("Events/HLT_AK8PFJetFwd500", 	HLT_AK8PFJetFwd500);
    output->add("Events/HLT_AK8PFJetFwd60", 	HLT_AK8PFJetFwd60);
    output->add("Events/HLT_AK8PFJetFwd80", 	HLT_AK8PFJetFwd80);
    output->add("Events/HLT_DiPFJet15_FBEta3_NoCaloMatched",
                 HLT_DiPFJet15_FBEta3_NoCaloMatched);
    output->add("Events/HLT_DiPFJet15_NoCaloMatched",
                 HLT_DiPFJet15_NoCaloMatched);
    output->add("Events/HLT_DiPFJet25_FBEta3_NoCaloMatched",
                 HLT_DiPFJet25_FBEta3_NoCaloMatched);
    output->add("Events/HLT_DiPFJet25_NoCaloMatched",
                 HLT_DiPFJet25_NoCaloMatched);
    output->add("Events/HLT_DiPFJetAve100_HFJEC", 	HLT_DiPFJetAve100_HFJEC);
    output->add("Events/HLT_DiPFJetAve140", 	HLT_DiPFJetAve140);
    output->add("Events/HLT_DiPFJetAve15_HFJEC", 	HLT_DiPFJetAve15_HFJEC);
    output->add("Events/HLT_DiPFJetAve160_HFJEC", 	HLT_DiPFJetAve160_HFJEC);
    output->add("Events/HLT_DiPFJetAve200", 	HLT_DiPFJetAve200);
    output->add("Events/HLT_DiPFJetAve220_HFJEC", 	HLT_DiPFJetAve220_HFJEC);
    output->add("Events/HLT_DiPFJetAve25_HFJEC", 	HLT_DiPFJetAve25_HFJEC);
    output->add("Events/HLT_DiPFJetAve260", 	HLT_DiPFJetAve260);
    output->add("Events/HLT_DiPFJetAve300_HFJEC", 	HLT_DiPFJetAve300_HFJEC);
    output->add("Events/HLT_DiPFJetAve320", 	HLT_DiPFJetAve320);
    output->add("Events/HLT_DiPFJetAve35_HFJEC", 	HLT_DiPFJetAve35_HFJEC);
    output->add("Events/HLT_DiPFJetAve40", 	HLT_DiPFJetAve40);
    output->add("Events/HLT_DiPFJetAve400", 	HLT_DiPFJetAve400);
    output->add("Events/HLT_DiPFJetAve500", 	HLT_DiPFJetAve500);
    output->add("Events/HLT_DiPFJetAve60", 	HLT_DiPFJetAve60);
    output->add("Events/HLT_DiPFJetAve60_HFJEC", 	HLT_DiPFJetAve60_HFJEC);
    output->add("Events/HLT_DiPFJetAve80", 	HLT_DiPFJetAve80);
    output->add("Events/HLT_DiPFJetAve80_HFJEC", 	HLT_DiPFJetAve80_HFJEC);
    output->add("Events/HLT_Ele115_CaloIdVT_GsfTrkIdT",
                 HLT_Ele115_CaloIdVT_GsfTrkIdT);
    output->add("Events/HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30",
                 HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30);
    output->add("Events/HLT_Ele135_CaloIdVT_GsfTrkIdT",
                 HLT_Ele135_CaloIdVT_GsfTrkIdT);
    output->add("Events/HLT_Ele145_CaloIdVT_GsfTrkIdT",
                 HLT_Ele145_CaloIdVT_GsfTrkIdT);
    output->add("Events/HLT_Ele15_IsoVVVL_PFHT450",
                 HLT_Ele15_IsoVVVL_PFHT450);
    output->add("Events/HLT_Ele15_IsoVVVL_PFHT450_CaloBTagCSV_4p5",
                 HLT_Ele15_IsoVVVL_PFHT450_CaloBTagCSV_4p5);
    output->add("Events/HLT_Ele15_IsoVVVL_PFHT450_PFMET50",
                 HLT_Ele15_IsoVVVL_PFHT450_PFMET50);
    output->add("Events/HLT_Ele15_IsoVVVL_PFHT600",
                 HLT_Ele15_IsoVVVL_PFHT600);
    output->add("Events/HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL",
                 HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL);
    output->add("Events/HLT_Ele17_CaloIdM_TrackIdM_PFJet30",
                 HLT_Ele17_CaloIdM_TrackIdM_PFJet30);
    output->add("Events/HLT_Ele200_CaloIdVT_GsfTrkIdT",
                 HLT_Ele200_CaloIdVT_GsfTrkIdT);
    output->add("Events/HLT_Ele20_WPLoose_Gsf", 	HLT_Ele20_WPLoose_Gsf);
    output->add("Events/HLT_Ele20_WPTight_Gsf", 	HLT_Ele20_WPTight_Gsf);
    output->add("Events/HLT_Ele20_eta2p1_WPLoose_Gsf",
                 HLT_Ele20_eta2p1_WPLoose_Gsf);
    output->add("Events/HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30",
                 HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30);
    output->add("Events/HLT_Ele23_CaloIdM_TrackIdM_PFJet30",
                 HLT_Ele23_CaloIdM_TrackIdM_PFJet30);
    output->add("Events/HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL",
                 HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL);
    output->add("Events/HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ",
                 HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ);
    output->add("Events/HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_CrossL1",
                 HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_CrossL1);
    output->add("Events/HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_TightID_CrossL1",
                 HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_TightID_CrossL1);
    output->add("Events/HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_CrossL1",
                 HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_CrossL1);
    output->add("Events/HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_TightID_CrossL1",
                 HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_TightID_CrossL1);
    output->add("Events/HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_CrossL1",
                 HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_CrossL1);
    output->add("Events/HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_TightID_CrossL1",
                 HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_TightID_CrossL1);
    output->add("Events/HLT_Ele250_CaloIdVT_GsfTrkIdT",
                 HLT_Ele250_CaloIdVT_GsfTrkIdT);
    output->add("Events/HLT_Ele27_Ele37_CaloIdL_MW",
                 HLT_Ele27_Ele37_CaloIdL_MW);
    output->add("Events/HLT_Ele27_WPTight_Gsf", 	HLT_Ele27_WPTight_Gsf);
    output->add("Events/HLT_Ele28_HighEta_SC20_Mass55",
                 HLT_Ele28_HighEta_SC20_Mass55);
    output->add("Events/HLT_Ele28_eta2p1_WPTight_Gsf_HT150",
                 HLT_Ele28_eta2p1_WPTight_Gsf_HT150);
    output->add("Events/HLT_Ele300_CaloIdVT_GsfTrkIdT",
                 HLT_Ele300_CaloIdVT_GsfTrkIdT);
    output->add("Events/HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned",
                 HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned);
    output->add("Events/HLT_Ele32_WPTight_Gsf", 	HLT_Ele32_WPTight_Gsf);
    output->add("Events/HLT_Ele32_WPTight_Gsf_L1DoubleEG",
                 HLT_Ele32_WPTight_Gsf_L1DoubleEG);
    output->add("Events/HLT_Ele35_WPTight_Gsf", 	HLT_Ele35_WPTight_Gsf);
    output->add("Events/HLT_Ele35_WPTight_Gsf_L1EGMT",
                 HLT_Ele35_WPTight_Gsf_L1EGMT);
    output->add("Events/HLT_Ele38_WPTight_Gsf", 	HLT_Ele38_WPTight_Gsf);
    output->add("Events/HLT_Ele40_WPTight_Gsf", 	HLT_Ele40_WPTight_Gsf);
    output->add("Events/HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165",
                 HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165);
    output->add("Events/HLT_Ele50_IsoVVVL_PFHT450",
                 HLT_Ele50_IsoVVVL_PFHT450);
    output->add("Events/HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30",
                 HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30);
    output->add("Events/HLT_Ele8_CaloIdM_TrackIdM_PFJet30",
                 HLT_Ele8_CaloIdM_TrackIdM_PFJet30);
    output->add("Events/HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07",
                 HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07);
    output->add("Events/HLT_IsoMu20", 	HLT_IsoMu20);
    output->add("Events/HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_CrossL1",
                 HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_CrossL1);
    output->add("Events/HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_TightID_CrossL1",
                 HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_TightID_CrossL1);
    output->add("Events/HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_CrossL1",
                 HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_CrossL1);
    output->add("Events/HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_TightID_CrossL1",
                 HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_TightID_CrossL1);
    output->add("Events/HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_CrossL1",
                 HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_CrossL1);
    output->add("Events/HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_TightID_CrossL1",
                 HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_TightID_CrossL1);
    output->add("Events/HLT_IsoMu24", 	HLT_IsoMu24);
    output->add("Events/HLT_IsoMu24_eta2p1", 	HLT_IsoMu24_eta2p1);
    output->add("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_SingleL1",
                 HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_SingleL1);
    output->add("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_TightID_SingleL1",
                 HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_TightID_SingleL1);
    output->add("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1",
                 HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1);
    output->add("Events/HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1",
                 HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1);
    output->add("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_SingleL1",
                 HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_SingleL1);
    output->add("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_TightID_SingleL1",
                 HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_TightID_SingleL1);
    output->add("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1",
                 HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1);
    output->add("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1",
                 HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1);
    output->add("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1",
                 HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1);
    output->add("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1",
                 HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1);
    output->add("Events/HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau50_Trk30_eta2p1_1pr",
                 HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau50_Trk30_eta2p1_1pr);
    output->add("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_SingleL1",
                 HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_SingleL1);
    output->add("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_TightID_SingleL1",
                 HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_TightID_SingleL1);
    output->add("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1",
                 HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1);
    output->add("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1",
                 HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1);
    output->add("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1",
                 HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1);
    output->add("Events/HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1",
                 HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1);
    output->add("Events/HLT_IsoMu27", 	HLT_IsoMu27);
    output->add("Events/HLT_IsoMu27_LooseChargedIsoPFTau20_SingleL1",
                 HLT_IsoMu27_LooseChargedIsoPFTau20_SingleL1);
    output->add("Events/HLT_IsoMu27_MediumChargedIsoPFTau20_SingleL1",
                 HLT_IsoMu27_MediumChargedIsoPFTau20_SingleL1);
    output->add("Events/HLT_IsoMu27_TightChargedIsoPFTau20_SingleL1",
                 HLT_IsoMu27_TightChargedIsoPFTau20_SingleL1);
    output->add("Events/HLT_IsoMu30", 	HLT_IsoMu30);
    output->add("Events/HLT_Mu10_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT350_PFMETNoMu60",
                 HLT_Mu10_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT350_PFMETNoMu60);
    output->add("Events/HLT_Mu12_DoublePFJets100_CaloBTagCSV_p33",
                 HLT_Mu12_DoublePFJets100_CaloBTagCSV_p33);
    output->add("Events/HLT_Mu12_DoublePFJets200_CaloBTagCSV_p33",
                 HLT_Mu12_DoublePFJets200_CaloBTagCSV_p33);
    output->add("Events/HLT_Mu12_DoublePFJets350_CaloBTagCSV_p33",
                 HLT_Mu12_DoublePFJets350_CaloBTagCSV_p33);
    output->add("Events/HLT_Mu12_DoublePFJets40MaxDeta1p6_DoubleCaloBTagCSV_p33",
                 HLT_Mu12_DoublePFJets40MaxDeta1p6_DoubleCaloBTagCSV_p33);
    output->add("Events/HLT_Mu12_DoublePFJets40_CaloBTagCSV_p33",
                 HLT_Mu12_DoublePFJets40_CaloBTagCSV_p33);
    output->add("Events/HLT_Mu12_DoublePFJets54MaxDeta1p6_DoubleCaloBTagCSV_p33",
                 HLT_Mu12_DoublePFJets54MaxDeta1p6_DoubleCaloBTagCSV_p33);
    output->add("Events/HLT_Mu12_DoublePFJets62MaxDeta1p6_DoubleCaloBTagCSV_p33",
                 HLT_Mu12_DoublePFJets62MaxDeta1p6_DoubleCaloBTagCSV_p33);
    output->add("Events/HLT_Mu12_DoublePhoton20", 	HLT_Mu12_DoublePhoton20);
    output->add("Events/HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL",
                 HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL);
    output->add("Events/HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ",
                 HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ);
    output->add("Events/HLT_Mu15_IsoVVVL_PFHT450",
                 HLT_Mu15_IsoVVVL_PFHT450);
    output->add("Events/HLT_Mu15_IsoVVVL_PFHT450_CaloBTagCSV_4p5",
                 HLT_Mu15_IsoVVVL_PFHT450_CaloBTagCSV_4p5);
    output->add("Events/HLT_Mu15_IsoVVVL_PFHT450_PFMET50",
                 HLT_Mu15_IsoVVVL_PFHT450_PFMET50);
    output->add("Events/HLT_Mu15_IsoVVVL_PFHT600",
                 HLT_Mu15_IsoVVVL_PFHT600);
    output->add("Events/HLT_Mu17", 	HLT_Mu17);
    output->add("Events/HLT_Mu17_Photon30_IsoCaloId",
                 HLT_Mu17_Photon30_IsoCaloId);
    output->add("Events/HLT_Mu17_TrkIsoVVL", 	HLT_Mu17_TrkIsoVVL);
    output->add("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL",
                 HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL);
    output->add("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ",
                 HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ);
    output->add("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8",
                 HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8);
    output->add("Events/HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8",
                 HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8);
    output->add("Events/HLT_Mu18_Mu9", 	HLT_Mu18_Mu9);
    output->add("Events/HLT_Mu18_Mu9_DZ", 	HLT_Mu18_Mu9_DZ);
    output->add("Events/HLT_Mu18_Mu9_SameSign", 	HLT_Mu18_Mu9_SameSign);
    output->add("Events/HLT_Mu18_Mu9_SameSign_DZ",
                 HLT_Mu18_Mu9_SameSign_DZ);
    output->add("Events/HLT_Mu19", 	HLT_Mu19);
    output->add("Events/HLT_Mu19_TrkIsoVVL", 	HLT_Mu19_TrkIsoVVL);
    output->add("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL",
                 HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL);
    output->add("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ",
                 HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ);
    output->add("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass3p8",
                 HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass3p8);
    output->add("Events/HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass8",
                 HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass8);
    output->add("Events/HLT_Mu20", 	HLT_Mu20);
    output->add("Events/HLT_Mu20_Mu10", 	HLT_Mu20_Mu10);
    output->add("Events/HLT_Mu20_Mu10_DZ", 	HLT_Mu20_Mu10_DZ);
    output->add("Events/HLT_Mu20_Mu10_SameSign", 	HLT_Mu20_Mu10_SameSign);
    output->add("Events/HLT_Mu20_Mu10_SameSign_DZ",
                 HLT_Mu20_Mu10_SameSign_DZ);
    output->add("Events/HLT_Mu20_TkMu0_Phi", 	HLT_Mu20_TkMu0_Phi);
    output->add("Events/HLT_Mu23_Mu12", 	HLT_Mu23_Mu12);
    output->add("Events/HLT_Mu23_Mu12_DZ", 	HLT_Mu23_Mu12_DZ);
    output->add("Events/HLT_Mu23_Mu12_SameSign", 	HLT_Mu23_Mu12_SameSign);
    output->add("Events/HLT_Mu23_Mu12_SameSign_DZ",
                 HLT_Mu23_Mu12_SameSign_DZ);
    output->add("Events/HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL",
                 HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL);
    output->add("Events/HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ",
                 HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ);
    output->add("Events/HLT_Mu25_TkMu0_Onia", 	HLT_Mu25_TkMu0_Onia);
    output->add("Events/HLT_Mu25_TkMu0_Phi", 	HLT_Mu25_TkMu0_Phi);
    output->add("Events/HLT_Mu27", 	HLT_Mu27);
    output->add("Events/HLT_Mu27_Ele37_CaloIdL_MW",
                 HLT_Mu27_Ele37_CaloIdL_MW);
    output->add("Events/HLT_Mu30_TkMu0_Onia", 	HLT_Mu30_TkMu0_Onia);
    output->add("Events/HLT_Mu37_Ele27_CaloIdL_MW",
                 HLT_Mu37_Ele27_CaloIdL_MW);
    output->add("Events/HLT_Mu37_TkMu27", 	HLT_Mu37_TkMu27);
    output->add("Events/HLT_Mu3_PFJet40", 	HLT_Mu3_PFJet40);
    output->add("Events/HLT_Mu43NoFiltersNoVtx_Photon43_CaloIdL",
                 HLT_Mu43NoFiltersNoVtx_Photon43_CaloIdL);
    output->add("Events/HLT_Mu48NoFiltersNoVtx_Photon48_CaloIdL",
                 HLT_Mu48NoFiltersNoVtx_Photon48_CaloIdL);
    output->add("Events/HLT_Mu50", 	HLT_Mu50);
    output->add("Events/HLT_Mu50_IsoVVVL_PFHT450",
                 HLT_Mu50_IsoVVVL_PFHT450);
    output->add("Events/HLT_Mu55", 	HLT_Mu55);
    output->add("Events/HLT_Mu7p5_L2Mu2_Jpsi", 	HLT_Mu7p5_L2Mu2_Jpsi);
    output->add("Events/HLT_Mu7p5_L2Mu2_Upsilon", 	HLT_Mu7p5_L2Mu2_Upsilon);
    output->add("Events/HLT_Mu7p5_Track2_Jpsi", 	HLT_Mu7p5_Track2_Jpsi);
    output->add("Events/HLT_Mu7p5_Track2_Upsilon",
                 HLT_Mu7p5_Track2_Upsilon);
    output->add("Events/HLT_Mu7p5_Track3p5_Jpsi", 	HLT_Mu7p5_Track3p5_Jpsi);
    output->add("Events/HLT_Mu7p5_Track3p5_Upsilon",
                 HLT_Mu7p5_Track3p5_Upsilon);
    output->add("Events/HLT_Mu7p5_Track7_Jpsi", 	HLT_Mu7p5_Track7_Jpsi);
    output->add("Events/HLT_Mu7p5_Track7_Upsilon",
                 HLT_Mu7p5_Track7_Upsilon);
    output->add("Events/HLT_Mu8", 	HLT_Mu8);
    output->add("Events/HLT_Mu8_DiEle12_CaloIdL_TrackIdL",
                 HLT_Mu8_DiEle12_CaloIdL_TrackIdL);
    output->add("Events/HLT_Mu8_DiEle12_CaloIdL_TrackIdL_DZ",
                 HLT_Mu8_DiEle12_CaloIdL_TrackIdL_DZ);
    output->add("Events/HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350",
                 HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350);
    output->add("Events/HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ",
                 HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ);
    output->add("Events/HLT_Mu8_TrkIsoVVL", 	HLT_Mu8_TrkIsoVVL);
    output->add("Events/HLT_Mu8_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT300_PFMETNoMu60",
                 HLT_Mu8_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT300_PFMETNoMu60);
    output->add("Events/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL",
                 HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL);
    output->add("Events/HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ",
                 HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ);
    output->add("Events/HLT_PFHT1050", 	HLT_PFHT1050);
    output->add("Events/HLT_PFHT180", 	HLT_PFHT180);
    output->add("Events/HLT_PFHT250", 	HLT_PFHT250);
    output->add("Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40",
                 HLT_PFHT300PT30_QuadPFJet_75_60_45_40);
    output->add("Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0",
                 HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0);
    output->add("Events/HLT_PFHT350", 	HLT_PFHT350);
    output->add("Events/HLT_PFHT350MinPFJet15", 	HLT_PFHT350MinPFJet15);
    output->add("Events/HLT_PFHT370", 	HLT_PFHT370);
    output->add("Events/HLT_PFHT380_SixJet32_DoubleBTagCSV_p075",
                 HLT_PFHT380_SixJet32_DoubleBTagCSV_p075);
    output->add("Events/HLT_PFHT380_SixPFJet32", 	HLT_PFHT380_SixPFJet32);
    output->add("Events/HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2",
                 HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2);
    output->add("Events/HLT_PFHT380_SixPFJet32_DoublePFBTagDeepCSV_2p2",
                 HLT_PFHT380_SixPFJet32_DoublePFBTagDeepCSV_2p2);
    output->add("Events/HLT_PFHT430", 	HLT_PFHT430);
    output->add("Events/HLT_PFHT430_SixJet40_BTagCSV_p080",
                 HLT_PFHT430_SixJet40_BTagCSV_p080);
    output->add("Events/HLT_PFHT430_SixPFJet40", 	HLT_PFHT430_SixPFJet40);
    output->add("Events/HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5",
                 HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5);
    output->add("Events/HLT_PFHT500_PFMET100_PFMHT100_IDTight",
                 HLT_PFHT500_PFMET100_PFMHT100_IDTight);
    output->add("Events/HLT_PFHT500_PFMET110_PFMHT110_IDTight",
                 HLT_PFHT500_PFMET110_PFMHT110_IDTight);
    output->add("Events/HLT_PFHT510", 	HLT_PFHT510);
    output->add("Events/HLT_PFHT590", 	HLT_PFHT590);
    output->add("Events/HLT_PFHT680", 	HLT_PFHT680);
    output->add("Events/HLT_PFHT700_PFMET85_PFMHT85_IDTight",
                 HLT_PFHT700_PFMET85_PFMHT85_IDTight);
    output->add("Events/HLT_PFHT700_PFMET95_PFMHT95_IDTight",
                 HLT_PFHT700_PFMET95_PFMHT95_IDTight);
    output->add("Events/HLT_PFHT780", 	HLT_PFHT780);
    output->add("Events/HLT_PFHT800_PFMET75_PFMHT75_IDTight",
                 HLT_PFHT800_PFMET75_PFMHT75_IDTight);
    output->add("Events/HLT_PFHT800_PFMET85_PFMHT85_IDTight",
                 HLT_PFHT800_PFMET85_PFMHT85_IDTight);
    output->add("Events/HLT_PFHT890", 	HLT_PFHT890);
    output->add("Events/HLT_QuadPFJet103_88_75_15",
                 HLT_QuadPFJet103_88_75_15);
    output->add("Events/HLT_QuadPFJet103_88_75_15_BTagCSV_p013_VBF2",
                 HLT_QuadPFJet103_88_75_15_BTagCSV_p013_VBF2);
    output->add("Events/HLT_QuadPFJet103_88_75_15_DoubleBTagCSV_p013_p08_VBF1",
                 HLT_QuadPFJet103_88_75_15_DoubleBTagCSV_p013_p08_VBF1);
    output->add("Events/HLT_QuadPFJet105_88_76_15",
                 HLT_QuadPFJet105_88_76_15);
    output->add("Events/HLT_QuadPFJet105_88_76_15_BTagCSV_p013_VBF2",
                 HLT_QuadPFJet105_88_76_15_BTagCSV_p013_VBF2);
    output->add("Events/HLT_QuadPFJet105_90_76_15_DoubleBTagCSV_p013_p08_VBF1",
                 HLT_QuadPFJet105_90_76_15_DoubleBTagCSV_p013_p08_VBF1);
    output->add("Events/HLT_QuadPFJet111_90_80_15",
                 HLT_QuadPFJet111_90_80_15);
    output->add("Events/HLT_QuadPFJet111_90_80_15_BTagCSV_p013_VBF2",
                 HLT_QuadPFJet111_90_80_15_BTagCSV_p013_VBF2);
    output->add("Events/HLT_QuadPFJet111_90_80_15_DoubleBTagCSV_p013_p08_VBF1",
                 HLT_QuadPFJet111_90_80_15_DoubleBTagCSV_p013_p08_VBF1);
    output->add("Events/HLT_QuadPFJet98_83_71_15",
                 HLT_QuadPFJet98_83_71_15);
    output->add("Events/HLT_QuadPFJet98_83_71_15_BTagCSV_p013_VBF2",
                 HLT_QuadPFJet98_83_71_15_BTagCSV_p013_VBF2);
    output->add("Events/HLT_QuadPFJet98_83_71_15_DoubleBTagCSV_p013_p08_VBF1",
                 HLT_QuadPFJet98_83_71_15_DoubleBTagCSV_p013_p08_VBF1);
    output->add("Events/Jet_area[nJet]", 	Jet_area);
    output->add("Events/Jet_bRegCorr[nJet]", 	Jet_bRegCorr);
    output->add("Events/Jet_bRegRes[nJet]", 	Jet_bRegRes);
    output->add("Events/Jet_btagDeepFlavB[nJet]", 	Jet_btagDeepFlavB);
    output->add("Events/Jet_btagDeepFlavCvB[nJet]", 	Jet_btagDeepFlavCvB);
    output->add("Events/Jet_btagDeepFlavCvL[nJet]", 	Jet_btagDeepFlavCvL);
    output->add("Events/Jet_btagDeepFlavQG[nJet]", 	Jet_btagDeepFlavQG);
    output->add("Events/Jet_eta[nJet]", 	Jet_eta);
    output->add("Events/Jet_genJetIdx[nJet]", 	Jet_genJetIdx);
    output->add("Events/Jet_hadronFlavour[nJet]", 	Jet_hadronFlavour);
    output->add("Events/Jet_jetId[nJet]", 	Jet_jetId);
    output->add("Events/Jet_mass[nJet]", 	Jet_mass);
    output->add("Events/Jet_partonFlavour[nJet]", 	Jet_partonFlavour);
    output->add("Events/Jet_phi[nJet]", 	Jet_phi);
    output->add("Events/Jet_pt[nJet]", 	Jet_pt);
    output->add("Events/Jet_puId[nJet]", 	Jet_puId);
    output->add("Events/Jet_qgl[nJet]", 	Jet_qgl);
    output->add("Events/Jet_rawFactor[nJet]", 	Jet_rawFactor);
    output->add("Events/L1PreFiringWeight_Dn", 	L1PreFiringWeight_Dn);
    output->add("Events/L1PreFiringWeight_Nom", 	L1PreFiringWeight_Nom);
    output->add("Events/L1PreFiringWeight_Up", 	L1PreFiringWeight_Up);
    output->add("Events/LHEPdfWeight[nLHEPdfWeight]", 	LHEPdfWeight);
    output->add("Events/LHEScaleWeight[nLHEScaleWeight]", 	LHEScaleWeight);
    output->add("Events/MET_covXX", 	MET_covXX);
    output->add("Events/MET_covXY", 	MET_covXY);
    output->add("Events/MET_covYY", 	MET_covYY);
    output->add("Events/MET_phi", 	MET_phi);
    output->add("Events/MET_pt", 	MET_pt);
    output->add("Events/MET_significance", 	MET_significance);
    output->add("Events/Muon_charge[nMuon]", 	Muon_charge);
    output->add("Events/Muon_dxy[nMuon]", 	Muon_dxy);
    output->add("Events/Muon_dz[nMuon]", 	Muon_dz);
    output->add("Events/Muon_eta[nMuon]", 	Muon_eta);
    output->add("Events/Muon_genPartIdx[nMuon]", 	Muon_genPartIdx);
    output->add("Events/Muon_looseId[nMuon]", 	Muon_looseId);
    output->add("Events/Muon_mass[nMuon]", 	Muon_mass);
    output->add("Events/Muon_mediumId[nMuon]", 	Muon_mediumId);
    output->add("Events/Muon_miniPFRelIso_all[nMuon]",
                 Muon_miniPFRelIso_all);
    output->add("Events/Muon_mvaTTH[nMuon]", 	Muon_mvaTTH);
    output->add("Events/Muon_pfRelIso04_all[nMuon]", 	Muon_pfRelIso04_all);
    output->add("Events/Muon_phi[nMuon]", 	Muon_phi);
    output->add("Events/Muon_pt[nMuon]", 	Muon_pt);
    output->add("Events/Muon_tightId[nMuon]", 	Muon_tightId);
    output->add("Events/PV_chi2", 	PV_chi2);
    output->add("Events/PV_ndof", 	PV_ndof);
    output->add("Events/PV_npvs", 	PV_npvs);
    output->add("Events/PV_npvsGood", 	PV_npvsGood);
    output->add("Events/PV_z", 	PV_z);
    output->add("Events/Pileup_nPU", 	Pileup_nPU);
    output->add("Events/Pileup_nTrueInt", 	Pileup_nTrueInt);
    output->add("Events/PuppiMET_phi", 	PuppiMET_phi);
    output->add("Events/PuppiMET_phiJERDown", 	PuppiMET_phiJERDown);
    output->add("Events/PuppiMET_phiJERUp", 	PuppiMET_phiJERUp);
    output->add("Events/PuppiMET_phiJESDown", 	PuppiMET_phiJESDown);
    output->add("Events/PuppiMET_phiJESUp", 	PuppiMET_phiJESUp);
    output->add("Events/PuppiMET_phiUnclusteredDown",
                 PuppiMET_phiUnclusteredDown);
    output->add("Events/PuppiMET_phiUnclusteredUp",
                 PuppiMET_phiUnclusteredUp);
    output->add("Events/PuppiMET_pt", 	PuppiMET_pt);
    output->add("Events/PuppiMET_ptJERDown", 	PuppiMET_ptJERDown);
    output->add("Events/PuppiMET_ptJERUp", 	PuppiMET_ptJERUp);
    output->add("Events/PuppiMET_ptJESDown", 	PuppiMET_ptJESDown);
    output->add("Events/PuppiMET_ptJESUp", 	PuppiMET_ptJESUp);
    output->add("Events/PuppiMET_ptUnclusteredDown",
                 PuppiMET_ptUnclusteredDown);
    output->add("Events/PuppiMET_ptUnclusteredUp",
                 PuppiMET_ptUnclusteredUp);
    output->add("Events/PuppiMET_sumEt", 	PuppiMET_sumEt);
    output->add("Events/event", 	event);
    output->add("Events/fixedGridRhoFastjetAll", 	fixedGridRhoFastjetAll);
    output->add("Events/genTtbarId", 	genTtbarId);
    output->add("Events/genWeight", 	genWeight);
    output->add("Events/luminosityBlock", 	luminosityBlock);
    output->add("Events/run", 	run);
    output->add("Events/ttCatSource", 	ttCatSource);
    output->add("Events/ttCatXvalSource", 	ttCatXvalSource);
    output->add("Events/ttCatXval_Add1Bjet_1Had", 	ttCatXval_Add1Bjet_1Had);
    output->add("Events/ttCatXval_Add1Bjet_2Had", 	ttCatXval_Add1Bjet_2Had);
    output->add("Events/ttCatXval_Add2Bjet", 	ttCatXval_Add2Bjet);
    output->add("Events/ttCatXval_AddCjet", 	ttCatXval_AddCjet);
    output->add("Events/ttCatXval_LightFlavour", 	ttCatXval_LightFlavour);
    output->add("Events/ttCat_Add1Bjet_1Had", 	ttCat_Add1Bjet_1Had);
    output->add("Events/ttCat_Add1Bjet_2Had", 	ttCat_Add1Bjet_2Had);
    output->add("Events/ttCat_Add2Bjet", 	ttCat_Add2Bjet);
    output->add("Events/ttCat_AddCjet", 	ttCat_AddCjet);
    output->add("Events/ttCat_LightFlavour", 	ttCat_LightFlavour);

  }

  void initBuffers()
  {
    Flag_BadPFMuonDzFilter	= 0;
    Flag_BadPFMuonFilter	= 0;
    Flag_EcalDeadCellTriggerPrimitiveFilter	= 0;
    Flag_HBHENoiseFilter	= 0;
    Flag_HBHENoiseIsoFilter	= 0;
    Flag_ecalBadCalibFilter	= 0;
    Flag_eeBadScFilter	= 0;
    Flag_globalSuperTightHalo2016Filter	= 0;
    Flag_goodVertices	= 0;
    Generator_weight	= 0;
    HLT_AK8PFHT750_TrimMass50	= 0;
    HLT_AK8PFHT800_TrimMass50	= 0;
    HLT_AK8PFHT850_TrimMass50	= 0;
    HLT_AK8PFHT900_TrimMass50	= 0;
    HLT_AK8PFJet140	= 0;
    HLT_AK8PFJet200	= 0;
    HLT_AK8PFJet260	= 0;
    HLT_AK8PFJet320	= 0;
    HLT_AK8PFJet330_PFAK8BTagCSV_p1	= 0;
    HLT_AK8PFJet330_PFAK8BTagCSV_p17	= 0;
    HLT_AK8PFJet360_TrimMass30	= 0;
    HLT_AK8PFJet380_TrimMass30	= 0;
    HLT_AK8PFJet40	= 0;
    HLT_AK8PFJet400	= 0;
    HLT_AK8PFJet400_TrimMass30	= 0;
    HLT_AK8PFJet420_TrimMass30	= 0;
    HLT_AK8PFJet450	= 0;
    HLT_AK8PFJet500	= 0;
    HLT_AK8PFJet550	= 0;
    HLT_AK8PFJet60	= 0;
    HLT_AK8PFJet80	= 0;
    HLT_AK8PFJetFwd140	= 0;
    HLT_AK8PFJetFwd200	= 0;
    HLT_AK8PFJetFwd260	= 0;
    HLT_AK8PFJetFwd320	= 0;
    HLT_AK8PFJetFwd40	= 0;
    HLT_AK8PFJetFwd400	= 0;
    HLT_AK8PFJetFwd450	= 0;
    HLT_AK8PFJetFwd500	= 0;
    HLT_AK8PFJetFwd60	= 0;
    HLT_AK8PFJetFwd80	= 0;
    HLT_DiPFJet15_FBEta3_NoCaloMatched	= 0;
    HLT_DiPFJet15_NoCaloMatched	= 0;
    HLT_DiPFJet25_FBEta3_NoCaloMatched	= 0;
    HLT_DiPFJet25_NoCaloMatched	= 0;
    HLT_DiPFJetAve100_HFJEC	= 0;
    HLT_DiPFJetAve140	= 0;
    HLT_DiPFJetAve15_HFJEC	= 0;
    HLT_DiPFJetAve160_HFJEC	= 0;
    HLT_DiPFJetAve200	= 0;
    HLT_DiPFJetAve220_HFJEC	= 0;
    HLT_DiPFJetAve25_HFJEC	= 0;
    HLT_DiPFJetAve260	= 0;
    HLT_DiPFJetAve300_HFJEC	= 0;
    HLT_DiPFJetAve320	= 0;
    HLT_DiPFJetAve35_HFJEC	= 0;
    HLT_DiPFJetAve40	= 0;
    HLT_DiPFJetAve400	= 0;
    HLT_DiPFJetAve500	= 0;
    HLT_DiPFJetAve60	= 0;
    HLT_DiPFJetAve60_HFJEC	= 0;
    HLT_DiPFJetAve80	= 0;
    HLT_DiPFJetAve80_HFJEC	= 0;
    HLT_Ele115_CaloIdVT_GsfTrkIdT	= 0;
    HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30	= 0;
    HLT_Ele135_CaloIdVT_GsfTrkIdT	= 0;
    HLT_Ele145_CaloIdVT_GsfTrkIdT	= 0;
    HLT_Ele15_IsoVVVL_PFHT450	= 0;
    HLT_Ele15_IsoVVVL_PFHT450_CaloBTagCSV_4p5	= 0;
    HLT_Ele15_IsoVVVL_PFHT450_PFMET50	= 0;
    HLT_Ele15_IsoVVVL_PFHT600	= 0;
    HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL	= 0;
    HLT_Ele17_CaloIdM_TrackIdM_PFJet30	= 0;
    HLT_Ele200_CaloIdVT_GsfTrkIdT	= 0;
    HLT_Ele20_WPLoose_Gsf	= 0;
    HLT_Ele20_WPTight_Gsf	= 0;
    HLT_Ele20_eta2p1_WPLoose_Gsf	= 0;
    HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30	= 0;
    HLT_Ele23_CaloIdM_TrackIdM_PFJet30	= 0;
    HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL	= 0;
    HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ	= 0;
    HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_CrossL1	= 0;
    HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_TightID_CrossL1	= 0;
    HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_CrossL1	= 0;
    HLT_Ele24_eta2p1_WPTight_Gsf_MediumChargedIsoPFTau30_eta2p1_TightID_CrossL1	= 0;
    HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_CrossL1	= 0;
    HLT_Ele24_eta2p1_WPTight_Gsf_TightChargedIsoPFTau30_eta2p1_TightID_CrossL1	= 0;
    HLT_Ele250_CaloIdVT_GsfTrkIdT	= 0;
    HLT_Ele27_Ele37_CaloIdL_MW	= 0;
    HLT_Ele27_WPTight_Gsf	= 0;
    HLT_Ele28_HighEta_SC20_Mass55	= 0;
    HLT_Ele28_eta2p1_WPTight_Gsf_HT150	= 0;
    HLT_Ele300_CaloIdVT_GsfTrkIdT	= 0;
    HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned	= 0;
    HLT_Ele32_WPTight_Gsf	= 0;
    HLT_Ele32_WPTight_Gsf_L1DoubleEG	= 0;
    HLT_Ele35_WPTight_Gsf	= 0;
    HLT_Ele35_WPTight_Gsf_L1EGMT	= 0;
    HLT_Ele38_WPTight_Gsf	= 0;
    HLT_Ele40_WPTight_Gsf	= 0;
    HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165	= 0;
    HLT_Ele50_IsoVVVL_PFHT450	= 0;
    HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30	= 0;
    HLT_Ele8_CaloIdM_TrackIdM_PFJet30	= 0;
    HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07	= 0;
    HLT_IsoMu20	= 0;
    HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_CrossL1	= 0;
    HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_TightID_CrossL1	= 0;
    HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_CrossL1	= 0;
    HLT_IsoMu20_eta2p1_MediumChargedIsoPFTau27_eta2p1_TightID_CrossL1	= 0;
    HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_CrossL1	= 0;
    HLT_IsoMu20_eta2p1_TightChargedIsoPFTau27_eta2p1_TightID_CrossL1	= 0;
    HLT_IsoMu24	= 0;
    HLT_IsoMu24_eta2p1	= 0;
    HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_SingleL1	= 0;
    HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau20_TightID_SingleL1	= 0;
    HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1	= 0;
    HLT_IsoMu24_eta2p1_LooseChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1	= 0;
    HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_SingleL1	= 0;
    HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau20_TightID_SingleL1	= 0;
    HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1	= 0;
    HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1	= 0;
    HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1	= 0;
    HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1	= 0;
    HLT_IsoMu24_eta2p1_MediumChargedIsoPFTau50_Trk30_eta2p1_1pr	= 0;
    HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_SingleL1	= 0;
    HLT_IsoMu24_eta2p1_TightChargedIsoPFTau20_TightID_SingleL1	= 0;
    HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg_CrossL1	= 0;
    HLT_IsoMu24_eta2p1_TightChargedIsoPFTau35_Trk1_eta2p1_Reg_CrossL1	= 0;
    HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg_CrossL1	= 0;
    HLT_IsoMu24_eta2p1_TightChargedIsoPFTau40_Trk1_eta2p1_Reg_CrossL1	= 0;
    HLT_IsoMu27	= 0;
    HLT_IsoMu27_LooseChargedIsoPFTau20_SingleL1	= 0;
    HLT_IsoMu27_MediumChargedIsoPFTau20_SingleL1	= 0;
    HLT_IsoMu27_TightChargedIsoPFTau20_SingleL1	= 0;
    HLT_IsoMu30	= 0;
    HLT_Mu10_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT350_PFMETNoMu60	= 0;
    HLT_Mu12_DoublePFJets100_CaloBTagCSV_p33	= 0;
    HLT_Mu12_DoublePFJets200_CaloBTagCSV_p33	= 0;
    HLT_Mu12_DoublePFJets350_CaloBTagCSV_p33	= 0;
    HLT_Mu12_DoublePFJets40MaxDeta1p6_DoubleCaloBTagCSV_p33	= 0;
    HLT_Mu12_DoublePFJets40_CaloBTagCSV_p33	= 0;
    HLT_Mu12_DoublePFJets54MaxDeta1p6_DoubleCaloBTagCSV_p33	= 0;
    HLT_Mu12_DoublePFJets62MaxDeta1p6_DoubleCaloBTagCSV_p33	= 0;
    HLT_Mu12_DoublePhoton20	= 0;
    HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL	= 0;
    HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ	= 0;
    HLT_Mu15_IsoVVVL_PFHT450	= 0;
    HLT_Mu15_IsoVVVL_PFHT450_CaloBTagCSV_4p5	= 0;
    HLT_Mu15_IsoVVVL_PFHT450_PFMET50	= 0;
    HLT_Mu15_IsoVVVL_PFHT600	= 0;
    HLT_Mu17	= 0;
    HLT_Mu17_Photon30_IsoCaloId	= 0;
    HLT_Mu17_TrkIsoVVL	= 0;
    HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL	= 0;
    HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ	= 0;
    HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8	= 0;
    HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8	= 0;
    HLT_Mu18_Mu9	= 0;
    HLT_Mu18_Mu9_DZ	= 0;
    HLT_Mu18_Mu9_SameSign	= 0;
    HLT_Mu18_Mu9_SameSign_DZ	= 0;
    HLT_Mu19	= 0;
    HLT_Mu19_TrkIsoVVL	= 0;
    HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL	= 0;
    HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ	= 0;
    HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass3p8	= 0;
    HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass8	= 0;
    HLT_Mu20	= 0;
    HLT_Mu20_Mu10	= 0;
    HLT_Mu20_Mu10_DZ	= 0;
    HLT_Mu20_Mu10_SameSign	= 0;
    HLT_Mu20_Mu10_SameSign_DZ	= 0;
    HLT_Mu20_TkMu0_Phi	= 0;
    HLT_Mu23_Mu12	= 0;
    HLT_Mu23_Mu12_DZ	= 0;
    HLT_Mu23_Mu12_SameSign	= 0;
    HLT_Mu23_Mu12_SameSign_DZ	= 0;
    HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL	= 0;
    HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ	= 0;
    HLT_Mu25_TkMu0_Onia	= 0;
    HLT_Mu25_TkMu0_Phi	= 0;
    HLT_Mu27	= 0;
    HLT_Mu27_Ele37_CaloIdL_MW	= 0;
    HLT_Mu30_TkMu0_Onia	= 0;
    HLT_Mu37_Ele27_CaloIdL_MW	= 0;
    HLT_Mu37_TkMu27	= 0;
    HLT_Mu3_PFJet40	= 0;
    HLT_Mu43NoFiltersNoVtx_Photon43_CaloIdL	= 0;
    HLT_Mu48NoFiltersNoVtx_Photon48_CaloIdL	= 0;
    HLT_Mu50	= 0;
    HLT_Mu50_IsoVVVL_PFHT450	= 0;
    HLT_Mu55	= 0;
    HLT_Mu7p5_L2Mu2_Jpsi	= 0;
    HLT_Mu7p5_L2Mu2_Upsilon	= 0;
    HLT_Mu7p5_Track2_Jpsi	= 0;
    HLT_Mu7p5_Track2_Upsilon	= 0;
    HLT_Mu7p5_Track3p5_Jpsi	= 0;
    HLT_Mu7p5_Track3p5_Upsilon	= 0;
    HLT_Mu7p5_Track7_Jpsi	= 0;
    HLT_Mu7p5_Track7_Upsilon	= 0;
    HLT_Mu8	= 0;
    HLT_Mu8_DiEle12_CaloIdL_TrackIdL	= 0;
    HLT_Mu8_DiEle12_CaloIdL_TrackIdL_DZ	= 0;
    HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350	= 0;
    HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ	= 0;
    HLT_Mu8_TrkIsoVVL	= 0;
    HLT_Mu8_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT300_PFMETNoMu60	= 0;
    HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL	= 0;
    HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ	= 0;
    HLT_PFHT1050	= 0;
    HLT_PFHT180	= 0;
    HLT_PFHT250	= 0;
    HLT_PFHT300PT30_QuadPFJet_75_60_45_40	= 0;
    HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0	= 0;
    HLT_PFHT350	= 0;
    HLT_PFHT350MinPFJet15	= 0;
    HLT_PFHT370	= 0;
    HLT_PFHT380_SixJet32_DoubleBTagCSV_p075	= 0;
    HLT_PFHT380_SixPFJet32	= 0;
    HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2	= 0;
    HLT_PFHT380_SixPFJet32_DoublePFBTagDeepCSV_2p2	= 0;
    HLT_PFHT430	= 0;
    HLT_PFHT430_SixJet40_BTagCSV_p080	= 0;
    HLT_PFHT430_SixPFJet40	= 0;
    HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5	= 0;
    HLT_PFHT500_PFMET100_PFMHT100_IDTight	= 0;
    HLT_PFHT500_PFMET110_PFMHT110_IDTight	= 0;
    HLT_PFHT510	= 0;
    HLT_PFHT590	= 0;
    HLT_PFHT680	= 0;
    HLT_PFHT700_PFMET85_PFMHT85_IDTight	= 0;
    HLT_PFHT700_PFMET95_PFMHT95_IDTight	= 0;
    HLT_PFHT780	= 0;
    HLT_PFHT800_PFMET75_PFMHT75_IDTight	= 0;
    HLT_PFHT800_PFMET85_PFMHT85_IDTight	= 0;
    HLT_PFHT890	= 0;
    HLT_QuadPFJet103_88_75_15	= 0;
    HLT_QuadPFJet103_88_75_15_BTagCSV_p013_VBF2	= 0;
    HLT_QuadPFJet103_88_75_15_DoubleBTagCSV_p013_p08_VBF1	= 0;
    HLT_QuadPFJet105_88_76_15	= 0;
    HLT_QuadPFJet105_88_76_15_BTagCSV_p013_VBF2	= 0;
    HLT_QuadPFJet105_90_76_15_DoubleBTagCSV_p013_p08_VBF1	= 0;
    HLT_QuadPFJet111_90_80_15	= 0;
    HLT_QuadPFJet111_90_80_15_BTagCSV_p013_VBF2	= 0;
    HLT_QuadPFJet111_90_80_15_DoubleBTagCSV_p013_p08_VBF1	= 0;
    HLT_QuadPFJet98_83_71_15	= 0;
    HLT_QuadPFJet98_83_71_15_BTagCSV_p013_VBF2	= 0;
    HLT_QuadPFJet98_83_71_15_DoubleBTagCSV_p013_p08_VBF1	= 0;
    L1PreFiringWeight_Dn	= 0;
    L1PreFiringWeight_Nom	= 0;
    L1PreFiringWeight_Up	= 0;
    MET_covXX	= 0;
    MET_covXY	= 0;
    MET_covYY	= 0;
    MET_phi	= 0;
    MET_pt	= 0;
    MET_significance	= 0;
    PV_chi2	= 0;
    PV_ndof	= 0;
    PV_npvs	= 0;
    PV_npvsGood	= 0;
    PV_z	= 0;
    Pileup_nPU	= 0;
    Pileup_nTrueInt	= 0;
    PuppiMET_phi	= 0;
    PuppiMET_phiJERDown	= 0;
    PuppiMET_phiJERUp	= 0;
    PuppiMET_phiJESDown	= 0;
    PuppiMET_phiJESUp	= 0;
    PuppiMET_phiUnclusteredDown	= 0;
    PuppiMET_phiUnclusteredUp	= 0;
    PuppiMET_pt	= 0;
    PuppiMET_ptJERDown	= 0;
    PuppiMET_ptJERUp	= 0;
    PuppiMET_ptJESDown	= 0;
    PuppiMET_ptJESUp	= 0;
    PuppiMET_ptUnclusteredDown	= 0;
    PuppiMET_ptUnclusteredUp	= 0;
    PuppiMET_sumEt	= 0;
    event	= 0;
    fixedGridRhoFastjetAll	= 0;
    genTtbarId	= 0;
    genWeight	= 0;
    luminosityBlock	= 0;
    run	= 0;
    ttCatSource	= 0;
    ttCatXvalSource	= 0;
    ttCatXval_Add1Bjet_1Had	= 0;
    ttCatXval_Add1Bjet_2Had	= 0;
    ttCatXval_Add2Bjet	= 0;
    ttCatXval_AddCjet	= 0;
    ttCatXval_LightFlavour	= 0;
    ttCat_Add1Bjet_1Had	= 0;
    ttCat_Add1Bjet_2Had	= 0;
    ttCat_Add2Bjet	= 0;
    ttCat_AddCjet	= 0;
    ttCat_LightFlavour	= 0;
    Electron	= std::vector<eventBuffer::Electron_s>(23);
    FatJet	= std::vector<eventBuffer::FatJet_s>(21);
    GenJet	= std::vector<eventBuffer::GenJet_s>(50);
    GenPart	= std::vector<eventBuffer::GenPart_s>(233);
    Jet	= std::vector<eventBuffer::Jet_s>(62);
    Muon	= std::vector<eventBuffer::Muon_s>(37);

  }
      
  void read(int entry)
  {
    if ( !input ) 
      { 
        std::cout << "** eventBuffer::read - first  call read-only constructor!"
                  << std::endl;
        assert(0);
      }
    input->read(entry);

    // clear indexmap
    for(std::map<std::string, std::vector<int> >::iterator
    item=indexmap.begin(); 
    item != indexmap.end();
    ++item)
    item->second.clear();
  }

  void select(std::string objname)
  {
    indexmap[objname] = std::vector<int>();
  }

  void select(std::string objname, int index)
  {
    try
     {
       indexmap[objname].push_back(index);
     }
    catch (...)
     {
       std::cout << "** eventBuffer::select - first call select(""" 
                 << objname << """)" 
                 << std::endl;
       assert(0);
    }
  }

 void ls()
 {
   if( input ) input->ls();
 }

 int size()
 {
   if( input ) 
     return input->size();
   else
     return 0;
 }

 void close()
 {
   if( input )   input->close();
   if( output ) output->close();
 }

 // --- indexmap keeps track of which objects have been flagged for selection
 std::map<std::string, std::vector<int> > indexmap;

 // to read events
 itreestream* input;

 // to write events
 otreestream* output;

 // switches for choosing branches
 std::map<std::string, bool> choose;

}; 
#endif
