#ifndef EVENTBUFFER_H
#define EVENTBUFFER_H
// [STEP 24] provenance: printed by the analyzer at start (ttHHanalyzer_unified.cc main)
#define TTHH_EVENTBUFFER_STAMP "treestream 8be42e8; variables md5 605e8320ea853256035e41068075807e; record run_knu_eventbuffer_manifest_20261005_105905.log"
//----------------------------------------------------------------------------
// File:        eventBuffer.h
// Description: Analyzer header for ntuples created by TheNtupleMaker
// Created:     by mkanalyzer.py (treestream 8be42e8) from include/eventBuffer_variables.txt
// Author:      generated: tools/stage1/eventbuffer_from_record.py, record run_knu_eventbuffer_manifest_20261005_105905.log
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
#include <set>
#include "treestream.h"

struct eventBuffer
{
  //--------------------------------------------------------------------------
  // --- Declare variables
  //--------------------------------------------------------------------------
  std::vector<float>	CorrT1METJet_area;
  std::vector<float>	CorrT1METJet_eta;
  std::vector<float>	CorrT1METJet_muonSubtrFactor;
  std::vector<float>	CorrT1METJet_phi;
  std::vector<float>	CorrT1METJet_rawPt;
  std::vector<int>	Electron_charge;
  std::vector<int>	Electron_cleanmask;
  std::vector<bool>	Electron_convVeto;
  std::vector<int>	Electron_cutBased;
  std::vector<bool>	Electron_cutBased_HEEP;
  std::vector<float>	Electron_dEscaleDown;
  std::vector<float>	Electron_dEscaleUp;
  std::vector<float>	Electron_dEsigmaDown;
  std::vector<float>	Electron_dEsigmaUp;
  std::vector<float>	Electron_deltaEtaSC;
  std::vector<float>	Electron_dr03EcalRecHitSumEt;
  std::vector<float>	Electron_dr03HcalDepth1TowerSumEt;
  std::vector<float>	Electron_dr03TkSumPt;
  std::vector<float>	Electron_dr03TkSumPtHEEP;
  std::vector<float>	Electron_dxy;
  std::vector<float>	Electron_dxyErr;
  std::vector<float>	Electron_dz;
  std::vector<float>	Electron_dzErr;
  std::vector<float>	Electron_eCorr;
  std::vector<float>	Electron_eInvMinusPInv;
  std::vector<float>	Electron_energyErr;
  std::vector<float>	Electron_eta;
  std::vector<int>	Electron_genPartFlav;
  std::vector<int>	Electron_genPartIdx;
  std::vector<float>	Electron_hoe;
  std::vector<float>	Electron_ip3d;
  std::vector<bool>	Electron_isEB;
  std::vector<bool>	Electron_isPFcand;
  std::vector<int>	Electron_jetIdx;
  std::vector<int>	Electron_jetNDauCharged;
  std::vector<float>	Electron_jetPtRelv2;
  std::vector<float>	Electron_jetRelIso;
  std::vector<int>	Electron_lostHits;
  std::vector<float>	Electron_mass;
  std::vector<float>	Electron_miniPFRelIso_all;
  std::vector<float>	Electron_miniPFRelIso_chg;
  std::vector<float>	Electron_mvaFall17V2Iso;
  std::vector<bool>	Electron_mvaFall17V2Iso_WP80;
  std::vector<bool>	Electron_mvaFall17V2Iso_WP90;
  std::vector<bool>	Electron_mvaFall17V2Iso_WPL;
  std::vector<float>	Electron_mvaFall17V2noIso;
  std::vector<bool>	Electron_mvaFall17V2noIso_WP80;
  std::vector<bool>	Electron_mvaFall17V2noIso_WP90;
  std::vector<bool>	Electron_mvaFall17V2noIso_WPL;
  std::vector<float>	Electron_mvaIso;
  std::vector<bool>	Electron_mvaIso_WP80;
  std::vector<bool>	Electron_mvaIso_WP90;
  std::vector<bool>	Electron_mvaIso_WPL;
  std::vector<float>	Electron_mvaNoIso;
  std::vector<bool>	Electron_mvaNoIso_WP80;
  std::vector<bool>	Electron_mvaNoIso_WP90;
  std::vector<bool>	Electron_mvaNoIso_WPL;
  std::vector<float>	Electron_mvaTTH;
  std::vector<int>	Electron_pdgId;
  std::vector<float>	Electron_pfRelIso03_all;
  std::vector<float>	Electron_pfRelIso03_chg;
  std::vector<float>	Electron_pfRelIso04_all;
  std::vector<float>	Electron_phi;
  std::vector<int>	Electron_photonIdx;
  std::vector<float>	Electron_promptMVA;
  std::vector<float>	Electron_pt;
  std::vector<float>	Electron_r9;
  std::vector<float>	Electron_scEtOverPt;
  std::vector<int>	Electron_seedGain;
  std::vector<float>	Electron_sieie;
  std::vector<float>	Electron_sip3d;
  std::vector<float>	Electron_superclusterEta;
  std::vector<int>	Electron_tightCharge;
  std::vector<int>	Electron_vidNestedWPBitmap;
  std::vector<int>	Electron_vidNestedWPBitmapHEEP;
  std::vector<float>	FatJet_area;
  std::vector<float>	FatJet_btagCSVV2;
  std::vector<float>	FatJet_btagDDBvLV2;
  std::vector<float>	FatJet_btagDDCvBV2;
  std::vector<float>	FatJet_btagDDCvLV2;
  std::vector<float>	FatJet_btagDeepB;
  std::vector<float>	FatJet_btagHbb;
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
  std::vector<int>	FatJet_electronIdx3SJ;
  std::vector<float>	FatJet_eta;
  std::vector<int>	FatJet_genJetAK8Idx;
  std::vector<int>	FatJet_hadronFlavour;
  std::vector<int>	FatJet_jetId;
  std::vector<float>	FatJet_lsf3;
  std::vector<float>	FatJet_mass;
  std::vector<float>	FatJet_msoftdrop;
  std::vector<int>	FatJet_muonIdx3SJ;
  std::vector<float>	FatJet_n2b1;
  std::vector<float>	FatJet_n3b1;
  std::vector<int>	FatJet_nBHadrons;
  std::vector<int>	FatJet_nCHadrons;
  std::vector<int>	FatJet_nConstituents;
  std::vector<float>	FatJet_particleNetMD_QCD;
  std::vector<float>	FatJet_particleNetMD_Xbb;
  std::vector<float>	FatJet_particleNetMD_Xcc;
  std::vector<float>	FatJet_particleNetMD_Xqq;
  std::vector<float>	FatJet_particleNet_H4qvsQCD;
  std::vector<float>	FatJet_particleNet_HbbvsQCD;
  std::vector<float>	FatJet_particleNet_HccvsQCD;
  std::vector<float>	FatJet_particleNet_QCD;
  std::vector<float>	FatJet_particleNet_TvsQCD;
  std::vector<float>	FatJet_particleNet_WvsQCD;
  std::vector<float>	FatJet_particleNet_ZvsQCD;
  std::vector<float>	FatJet_particleNet_mass;
  std::vector<float>	FatJet_phi;
  std::vector<float>	FatJet_pt;
  std::vector<float>	FatJet_rawFactor;
  std::vector<int>	FatJet_subJetIdx1;
  std::vector<int>	FatJet_subJetIdx2;
  std::vector<float>	FatJet_tau1;
  std::vector<float>	FatJet_tau2;
  std::vector<float>	FatJet_tau3;
  std::vector<float>	FatJet_tau4;
  std::vector<float>	FsrPhoton_dROverEt2;
  std::vector<float>	FsrPhoton_eta;
  std::vector<int>	FsrPhoton_muonIdx;
  std::vector<float>	FsrPhoton_phi;
  std::vector<float>	FsrPhoton_pt;
  std::vector<float>	FsrPhoton_relIso03;
  std::vector<float>	GenDressedLepton_eta;
  std::vector<bool>	GenDressedLepton_hasTauAnc;
  std::vector<float>	GenDressedLepton_mass;
  std::vector<int>	GenDressedLepton_pdgId;
  std::vector<float>	GenDressedLepton_phi;
  std::vector<float>	GenDressedLepton_pt;
  std::vector<float>	GenIsolatedPhoton_eta;
  std::vector<float>	GenIsolatedPhoton_mass;
  std::vector<float>	GenIsolatedPhoton_phi;
  std::vector<float>	GenIsolatedPhoton_pt;
  std::vector<float>	GenJetAK8_eta;
  std::vector<int>	GenJetAK8_hadronFlavour;
  std::vector<float>	GenJetAK8_mass;
  std::vector<int>	GenJetAK8_partonFlavour;
  std::vector<float>	GenJetAK8_phi;
  std::vector<float>	GenJetAK8_pt;
  std::vector<float>	GenJet_eta;
  std::vector<int>	GenJet_hadronFlavour;
  std::vector<float>	GenJet_mass;
  std::vector<int>	GenJet_nBHadrons;
  std::vector<int>	GenJet_nCHadrons;
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
  std::vector<int>	GenVisTau_charge;
  std::vector<float>	GenVisTau_eta;
  std::vector<int>	GenVisTau_genPartIdxMother;
  std::vector<float>	GenVisTau_mass;
  std::vector<float>	GenVisTau_phi;
  std::vector<float>	GenVisTau_pt;
  std::vector<int>	GenVisTau_status;
  std::vector<int>	IsoTrack_charge;
  std::vector<float>	IsoTrack_dxy;
  std::vector<float>	IsoTrack_dz;
  std::vector<float>	IsoTrack_eta;
  std::vector<int>	IsoTrack_fromPV;
  std::vector<bool>	IsoTrack_isFromLostTrack;
  std::vector<bool>	IsoTrack_isHighPurityTrack;
  std::vector<bool>	IsoTrack_isPFcand;
  std::vector<float>	IsoTrack_miniPFRelIso_all;
  std::vector<float>	IsoTrack_miniPFRelIso_chg;
  std::vector<int>	IsoTrack_pdgId;
  std::vector<float>	IsoTrack_pfRelIso03_all;
  std::vector<float>	IsoTrack_pfRelIso03_chg;
  std::vector<float>	IsoTrack_phi;
  std::vector<float>	IsoTrack_pt;
  std::vector<float>	Jet_PNetRegPtRawCorr;
  std::vector<float>	Jet_PNetRegPtRawCorrNeutrino;
  std::vector<float>	Jet_PNetRegPtRawRes;
  std::vector<float>	Jet_UParTAK4RegPtRawCorr;
  std::vector<float>	Jet_UParTAK4RegPtRawCorrNeutrino;
  std::vector<float>	Jet_UParTAK4RegPtRawRes;
  std::vector<float>	Jet_UParTAK4V1RegPtRawCorr;
  std::vector<float>	Jet_UParTAK4V1RegPtRawCorrNeutrino;
  std::vector<float>	Jet_UParTAK4V1RegPtRawRes;
  std::vector<float>	Jet_area;
  std::vector<float>	Jet_bRegCorr;
  std::vector<float>	Jet_bRegRes;
  std::vector<float>	Jet_btagCSVV2;
  std::vector<float>	Jet_btagDeepB;
  std::vector<float>	Jet_btagDeepCvB;
  std::vector<float>	Jet_btagDeepCvL;
  std::vector<float>	Jet_btagDeepFlavB;
  std::vector<float>	Jet_btagDeepFlavCvB;
  std::vector<float>	Jet_btagDeepFlavCvL;
  std::vector<float>	Jet_btagDeepFlavQG;
  std::vector<float>	Jet_btagPNetB;
  std::vector<float>	Jet_btagPNetCvB;
  std::vector<float>	Jet_btagPNetCvL;
  std::vector<float>	Jet_btagPNetQvG;
  std::vector<float>	Jet_btagUParTAK4B;
  std::vector<float>	Jet_btagUParTAK4CvB;
  std::vector<float>	Jet_btagUParTAK4CvL;
  std::vector<float>	Jet_btagUParTAK4QvG;
  std::vector<float>	Jet_cRegCorr;
  std::vector<float>	Jet_cRegRes;
  std::vector<float>	Jet_chEmEF;
  std::vector<float>	Jet_chFPV0EF;
  std::vector<float>	Jet_chHEF;
  std::vector<int>	Jet_chMultiplicity;
  std::vector<int>	Jet_cleanmask;
  std::vector<int>	Jet_electronIdx1;
  std::vector<int>	Jet_electronIdx2;
  std::vector<float>	Jet_eta;
  std::vector<int>	Jet_genJetIdx;
  std::vector<int>	Jet_hadronFlavour;
  std::vector<float>	Jet_hfEmEF;
  std::vector<float>	Jet_hfHEF;
  std::vector<int>	Jet_hfadjacentEtaStripsSize;
  std::vector<int>	Jet_hfcentralEtaStripSize;
  std::vector<float>	Jet_hfsigmaEtaEta;
  std::vector<float>	Jet_hfsigmaPhiPhi;
  std::vector<int>	Jet_jetId;
  std::vector<float>	Jet_mass;
  std::vector<float>	Jet_muEF;
  std::vector<int>	Jet_muonIdx1;
  std::vector<int>	Jet_muonIdx2;
  std::vector<float>	Jet_muonSubtrFactor;
  std::vector<int>	Jet_nConstituents;
  std::vector<int>	Jet_nElectrons;
  std::vector<int>	Jet_nMuons;
  std::vector<float>	Jet_neEmEF;
  std::vector<float>	Jet_neHEF;
  std::vector<int>	Jet_neMultiplicity;
  std::vector<int>	Jet_partonFlavour;
  std::vector<float>	Jet_phi;
  std::vector<float>	Jet_pt;
  std::vector<int>	Jet_puId;
  std::vector<float>	Jet_puIdDisc;
  std::vector<float>	Jet_qgl;
  std::vector<float>	Jet_rawFactor;
  std::vector<float>	LHEPart_eta;
  std::vector<float>	LHEPart_incomingpz;
  std::vector<float>	LHEPart_mass;
  std::vector<int>	LHEPart_pdgId;
  std::vector<float>	LHEPart_phi;
  std::vector<float>	LHEPart_pt;
  std::vector<int>	LHEPart_spin;
  std::vector<int>	LHEPart_status;
  std::vector<float>	LHEPdfWeight;
  std::vector<float>	LHEReweightingWeight;
  std::vector<float>	LHEScaleWeight;
  std::vector<float>	LowPtElectron_ID;
  std::vector<int>	LowPtElectron_charge;
  std::vector<bool>	LowPtElectron_convVeto;
  std::vector<float>	LowPtElectron_convVtxRadius;
  std::vector<int>	LowPtElectron_convWP;
  std::vector<float>	LowPtElectron_deltaEtaSC;
  std::vector<float>	LowPtElectron_dxy;
  std::vector<float>	LowPtElectron_dxyErr;
  std::vector<float>	LowPtElectron_dz;
  std::vector<float>	LowPtElectron_dzErr;
  std::vector<float>	LowPtElectron_eInvMinusPInv;
  std::vector<float>	LowPtElectron_embeddedID;
  std::vector<float>	LowPtElectron_energyErr;
  std::vector<float>	LowPtElectron_eta;
  std::vector<int>	LowPtElectron_genPartFlav;
  std::vector<int>	LowPtElectron_genPartIdx;
  std::vector<float>	LowPtElectron_hoe;
  std::vector<int>	LowPtElectron_lostHits;
  std::vector<float>	LowPtElectron_mass;
  std::vector<float>	LowPtElectron_miniPFRelIso_all;
  std::vector<float>	LowPtElectron_miniPFRelIso_chg;
  std::vector<int>	LowPtElectron_pdgId;
  std::vector<float>	LowPtElectron_phi;
  std::vector<float>	LowPtElectron_pt;
  std::vector<float>	LowPtElectron_ptbiased;
  std::vector<float>	LowPtElectron_r9;
  std::vector<float>	LowPtElectron_scEtOverPt;
  std::vector<float>	LowPtElectron_sieie;
  std::vector<float>	LowPtElectron_unbiased;
  std::vector<int>	Muon_charge;
  std::vector<int>	Muon_cleanmask;
  std::vector<float>	Muon_dxy;
  std::vector<float>	Muon_dxyErr;
  std::vector<float>	Muon_dxybs;
  std::vector<float>	Muon_dz;
  std::vector<float>	Muon_dzErr;
  std::vector<float>	Muon_eta;
  std::vector<int>	Muon_fsrPhotonIdx;
  std::vector<int>	Muon_genPartFlav;
  std::vector<int>	Muon_genPartIdx;
  std::vector<int>	Muon_highPtId;
  std::vector<bool>	Muon_highPurity;
  std::vector<bool>	Muon_inTimeMuon;
  std::vector<float>	Muon_ip3d;
  std::vector<bool>	Muon_isGlobal;
  std::vector<bool>	Muon_isPFcand;
  std::vector<bool>	Muon_isStandalone;
  std::vector<bool>	Muon_isTracker;
  std::vector<int>	Muon_jetIdx;
  std::vector<int>	Muon_jetNDauCharged;
  std::vector<float>	Muon_jetPtRelv2;
  std::vector<float>	Muon_jetRelIso;
  std::vector<bool>	Muon_looseId;
  std::vector<float>	Muon_mass;
  std::vector<bool>	Muon_mediumId;
  std::vector<bool>	Muon_mediumPromptId;
  std::vector<int>	Muon_miniIsoId;
  std::vector<float>	Muon_miniPFRelIso_all;
  std::vector<float>	Muon_miniPFRelIso_chg;
  std::vector<int>	Muon_multiIsoId;
  std::vector<int>	Muon_mvaId;
  std::vector<float>	Muon_mvaLowPt;
  std::vector<int>	Muon_mvaLowPtId;
  std::vector<float>	Muon_mvaMuID;
  std::vector<int>	Muon_mvaMuID_WP;
  std::vector<float>	Muon_mvaTTH;
  std::vector<int>	Muon_nStations;
  std::vector<int>	Muon_nTrackerLayers;
  std::vector<int>	Muon_pdgId;
  std::vector<int>	Muon_pfIsoId;
  std::vector<float>	Muon_pfRelIso03_all;
  std::vector<float>	Muon_pfRelIso03_chg;
  std::vector<float>	Muon_pfRelIso04_all;
  std::vector<float>	Muon_phi;
  std::vector<float>	Muon_promptMVA;
  std::vector<float>	Muon_pt;
  std::vector<float>	Muon_ptErr;
  std::vector<int>	Muon_puppiIsoId;
  std::vector<float>	Muon_segmentComp;
  std::vector<float>	Muon_sip3d;
  std::vector<bool>	Muon_softId;
  std::vector<float>	Muon_softMva;
  std::vector<bool>	Muon_softMvaId;
  std::vector<int>	Muon_tightCharge;
  std::vector<bool>	Muon_tightId;
  std::vector<int>	Muon_tkIsoId;
  std::vector<float>	Muon_tkRelIso;
  std::vector<bool>	Muon_triggerIdLoose;
  std::vector<float>	Muon_tunepRelPt;
  std::vector<float>	OtherPV_z;
  std::vector<int>	PPSLocalTrack_decRPId;
  std::vector<int>	PPSLocalTrack_multiRPProtonIdx;
  std::vector<int>	PPSLocalTrack_rpType;
  std::vector<int>	PPSLocalTrack_singleRPProtonIdx;
  std::vector<float>	PPSLocalTrack_time;
  std::vector<float>	PPSLocalTrack_timeUnc;
  std::vector<float>	PPSLocalTrack_x;
  std::vector<float>	PPSLocalTrack_y;
  std::vector<float>	PSWeight;
  std::vector<int>	Photon_charge;
  std::vector<int>	Photon_cleanmask;
  std::vector<int>	Photon_cutBased;
  std::vector<int>	Photon_cutBased_Fall17V1Bitmap;
  std::vector<float>	Photon_dEscaleDown;
  std::vector<float>	Photon_dEscaleUp;
  std::vector<float>	Photon_dEsigmaDown;
  std::vector<float>	Photon_dEsigmaUp;
  std::vector<float>	Photon_eCorr;
  std::vector<int>	Photon_electronIdx;
  std::vector<bool>	Photon_electronVeto;
  std::vector<float>	Photon_energyErr;
  std::vector<float>	Photon_eta;
  std::vector<int>	Photon_genPartFlav;
  std::vector<int>	Photon_genPartIdx;
  std::vector<float>	Photon_hoe;
  std::vector<bool>	Photon_isScEtaEB;
  std::vector<bool>	Photon_isScEtaEE;
  std::vector<int>	Photon_jetIdx;
  std::vector<float>	Photon_mass;
  std::vector<float>	Photon_mvaID;
  std::vector<float>	Photon_mvaID_Fall17V1p1;
  std::vector<bool>	Photon_mvaID_WP80;
  std::vector<bool>	Photon_mvaID_WP90;
  std::vector<int>	Photon_pdgId;
  std::vector<float>	Photon_pfRelIso03_all;
  std::vector<float>	Photon_pfRelIso03_chg;
  std::vector<float>	Photon_phi;
  std::vector<bool>	Photon_pixelSeed;
  std::vector<float>	Photon_pt;
  std::vector<float>	Photon_r9;
  std::vector<int>	Photon_seedGain;
  std::vector<float>	Photon_sieie;
  std::vector<int>	Photon_vidNestedWPBitmap;
  std::vector<int>	Proton_multiRP_arm;
  std::vector<float>	Proton_multiRP_t;
  std::vector<float>	Proton_multiRP_thetaX;
  std::vector<float>	Proton_multiRP_thetaY;
  std::vector<float>	Proton_multiRP_time;
  std::vector<float>	Proton_multiRP_timeUnc;
  std::vector<float>	Proton_multiRP_xi;
  std::vector<int>	Proton_singleRP_decRPId;
  std::vector<float>	Proton_singleRP_thetaY;
  std::vector<float>	Proton_singleRP_xi;
  std::vector<int>	SV_charge;
  std::vector<float>	SV_chi2;
  std::vector<float>	SV_dlen;
  std::vector<float>	SV_dlenSig;
  std::vector<float>	SV_dxy;
  std::vector<float>	SV_dxySig;
  std::vector<float>	SV_eta;
  std::vector<float>	SV_mass;
  std::vector<float>	SV_ndof;
  std::vector<int>	SV_ntracks;
  std::vector<float>	SV_pAngle;
  std::vector<float>	SV_phi;
  std::vector<float>	SV_pt;
  std::vector<float>	SV_x;
  std::vector<float>	SV_y;
  std::vector<float>	SV_z;
  std::vector<float>	SoftActivityJet_eta;
  std::vector<float>	SoftActivityJet_phi;
  std::vector<float>	SoftActivityJet_pt;
  std::vector<float>	SubGenJetAK8_eta;
  std::vector<float>	SubGenJetAK8_mass;
  std::vector<float>	SubGenJetAK8_phi;
  std::vector<float>	SubGenJetAK8_pt;
  std::vector<float>	SubJet_btagCSVV2;
  std::vector<float>	SubJet_btagDeepB;
  std::vector<float>	SubJet_eta;
  std::vector<int>	SubJet_hadronFlavour;
  std::vector<float>	SubJet_mass;
  std::vector<float>	SubJet_n2b1;
  std::vector<float>	SubJet_n3b1;
  std::vector<int>	SubJet_nBHadrons;
  std::vector<int>	SubJet_nCHadrons;
  std::vector<float>	SubJet_phi;
  std::vector<float>	SubJet_pt;
  std::vector<float>	SubJet_rawFactor;
  std::vector<float>	SubJet_tau1;
  std::vector<float>	SubJet_tau2;
  std::vector<float>	SubJet_tau3;
  std::vector<float>	SubJet_tau4;
  std::vector<int>	Tau_charge;
  std::vector<float>	Tau_chargedIso;
  std::vector<int>	Tau_cleanmask;
  std::vector<int>	Tau_decayMode;
  std::vector<float>	Tau_dxy;
  std::vector<float>	Tau_dz;
  std::vector<float>	Tau_eta;
  std::vector<int>	Tau_genPartFlav;
  std::vector<int>	Tau_genPartIdx;
  std::vector<bool>	Tau_idAntiEleDeadECal;
  std::vector<int>	Tau_idAntiMu;
  std::vector<bool>	Tau_idDecayModeOldDMs;
  std::vector<int>	Tau_idDeepTau2017v2p1VSe;
  std::vector<int>	Tau_idDeepTau2017v2p1VSjet;
  std::vector<int>	Tau_idDeepTau2017v2p1VSmu;
  std::vector<int>	Tau_jetIdx;
  std::vector<float>	Tau_leadTkDeltaEta;
  std::vector<float>	Tau_leadTkDeltaPhi;
  std::vector<float>	Tau_leadTkPtOverTauPt;
  std::vector<float>	Tau_mass;
  std::vector<float>	Tau_neutralIso;
  std::vector<float>	Tau_phi;
  std::vector<float>	Tau_photonsOutsideSignalCone;
  std::vector<float>	Tau_pt;
  std::vector<float>	Tau_puCorr;
  std::vector<float>	Tau_rawDeepTau2017v2p1VSe;
  std::vector<float>	Tau_rawDeepTau2017v2p1VSjet;
  std::vector<float>	Tau_rawDeepTau2017v2p1VSmu;
  std::vector<float>	Tau_rawIso;
  std::vector<float>	Tau_rawIsodR03;
  std::vector<float>	TrigObj_eta;
  std::vector<int>	TrigObj_filterBits;
  std::vector<int>	TrigObj_id;
  std::vector<int>	TrigObj_l1charge;
  std::vector<int>	TrigObj_l1iso;
  std::vector<float>	TrigObj_l1pt;
  std::vector<float>	TrigObj_l1pt_2;
  std::vector<float>	TrigObj_l2pt;
  std::vector<float>	TrigObj_phi;
  std::vector<float>	TrigObj_pt;
  std::vector<int>	boostedTau_charge;
  std::vector<float>	boostedTau_chargedIso;
  std::vector<int>	boostedTau_decayMode;
  std::vector<float>	boostedTau_eta;
  std::vector<int>	boostedTau_genPartFlav;
  std::vector<int>	boostedTau_genPartIdx;
  std::vector<int>	boostedTau_idAntiEle2018;
  std::vector<int>	boostedTau_idAntiMu;
  std::vector<int>	boostedTau_idMVAnewDM2017v2;
  std::vector<int>	boostedTau_idMVAoldDM2017v2;
  std::vector<int>	boostedTau_idMVAoldDMdR032017v2;
  std::vector<int>	boostedTau_jetIdx;
  std::vector<float>	boostedTau_leadTkDeltaEta;
  std::vector<float>	boostedTau_leadTkDeltaPhi;
  std::vector<float>	boostedTau_leadTkPtOverTauPt;
  std::vector<float>	boostedTau_mass;
  std::vector<float>	boostedTau_neutralIso;
  std::vector<float>	boostedTau_phi;
  std::vector<float>	boostedTau_photonsOutsideSignalCone;
  std::vector<float>	boostedTau_pt;
  std::vector<float>	boostedTau_puCorr;
  std::vector<float>	boostedTau_rawAntiEle2018;
  std::vector<int>	boostedTau_rawAntiEleCat2018;
  std::vector<float>	boostedTau_rawIso;
  std::vector<float>	boostedTau_rawIsodR03;
  std::vector<float>	boostedTau_rawMVAnewDM2017v2;
  std::vector<float>	boostedTau_rawMVAoldDM2017v2;
  std::vector<float>	boostedTau_rawMVAoldDMdR032017v2;

  int	nCorrT1METJet;
  int	nElectron;
  int	nFatJet;
  int	nFsrPhoton;
  int	nGenDressedLepton;
  int	nGenIsolatedPhoton;
  int	nGenJet;
  int	nGenJetAK8;
  int	nGenPart;
  int	nGenVisTau;
  int	nIsoTrack;
  int	nJet;
  int	nLHEPart;
  int	nLHEPdfWeight;
  int	nLHEReweightingWeight;
  int	nLHEScaleWeight;
  int	nLowPtElectron;
  int	nMuon;
  int	nOtherPV;
  int	nPPSLocalTrack;
  int	nPSWeight;
  int	nPhoton;
  int	nProton_multiRP;
  int	nProton_singleRP;
  int	nSV;
  int	nSoftActivityJet;
  int	nSubGenJetAK8;
  int	nSubJet;
  int	nTau;
  int	nTrigObj;
  int	nboostedTau;

  float	CaloMET_phi;
  float	CaloMET_pt;
  float	CaloMET_sumEt;
  float	ChsMET_phi;
  float	ChsMET_pt;
  float	ChsMET_sumEt;
  float	DeepMETResolutionTune_phi;
  float	DeepMETResolutionTune_pt;
  float	DeepMETResponseTune_phi;
  float	DeepMETResponseTune_pt;
  bool	Flag_BadChargedCandidateFilter;
  bool	Flag_BadChargedCandidateFilter_pRECO;
  bool	Flag_BadChargedCandidateSummer16Filter;
  bool	Flag_BadChargedCandidateSummer16Filter_pRECO;
  bool	Flag_BadPFMuonDzFilter;
  bool	Flag_BadPFMuonDzFilter_pRECO;
  bool	Flag_BadPFMuonFilter;
  bool	Flag_BadPFMuonFilter_pRECO;
  bool	Flag_BadPFMuonSummer16Filter;
  bool	Flag_BadPFMuonSummer16Filter_pRECO;
  bool	Flag_CSCTightHalo2015Filter;
  bool	Flag_CSCTightHalo2015Filter_pRECO;
  bool	Flag_CSCTightHaloFilter;
  bool	Flag_CSCTightHaloFilter_pRECO;
  bool	Flag_CSCTightHaloTrkMuUnvetoFilter;
  bool	Flag_CSCTightHaloTrkMuUnvetoFilter_pRECO;
  bool	Flag_EcalDeadCellBoundaryEnergyFilter;
  bool	Flag_EcalDeadCellBoundaryEnergyFilter_pRECO;
  bool	Flag_EcalDeadCellTriggerPrimitiveFilter;
  bool	Flag_EcalDeadCellTriggerPrimitiveFilter_pRECO;
  bool	Flag_HBHENoiseFilter;
  bool	Flag_HBHENoiseFilter_pRECO;
  bool	Flag_HBHENoiseIsoFilter;
  bool	Flag_HBHENoiseIsoFilter_pRECO;
  bool	Flag_HcalStripHaloFilter;
  bool	Flag_HcalStripHaloFilter_pRECO;
  bool	Flag_METFilters;
  bool	Flag_METFilters_pRECO;
  bool	Flag_chargedHadronTrackResolutionFilter;
  bool	Flag_chargedHadronTrackResolutionFilter_pRECO;
  bool	Flag_ecalBadCalibFilter;
  bool	Flag_ecalBadCalibFilter_pRECO;
  bool	Flag_ecalLaserCorrFilter;
  bool	Flag_ecalLaserCorrFilter_pRECO;
  bool	Flag_eeBadScFilter;
  bool	Flag_eeBadScFilter_pRECO;
  bool	Flag_globalSuperTightHalo2016Filter;
  bool	Flag_globalSuperTightHalo2016Filter_pRECO;
  bool	Flag_globalTightHalo2016Filter;
  bool	Flag_globalTightHalo2016Filter_pRECO;
  bool	Flag_goodVertices;
  bool	Flag_goodVertices_pRECO;
  bool	Flag_hcalLaserEventFilter;
  bool	Flag_hcalLaserEventFilter_pRECO;
  bool	Flag_hfNoisyHitsFilter;
  bool	Flag_hfNoisyHitsFilter_pRECO;
  bool	Flag_muonBadTrackFilter;
  bool	Flag_muonBadTrackFilter_pRECO;
  bool	Flag_trkPOGFilters;
  bool	Flag_trkPOGFilters_pRECO;
  bool	Flag_trkPOG_logErrorTooManyClusters;
  bool	Flag_trkPOG_logErrorTooManyClusters_pRECO;
  bool	Flag_trkPOG_manystripclus53X;
  bool	Flag_trkPOG_manystripclus53X_pRECO;
  bool	Flag_trkPOG_toomanystripclus53X;
  bool	Flag_trkPOG_toomanystripclus53X_pRECO;
  float	GenMET_phi;
  float	GenMET_pt;
  float	GenVtx_t0;
  float	GenVtx_x;
  float	GenVtx_y;
  float	GenVtx_z;
  float	Generator_binvar;
  int	Generator_id1;
  int	Generator_id2;
  float	Generator_scalePDF;
  float	Generator_weight;
  float	Generator_x1;
  float	Generator_x2;
  float	Generator_xpdf1;
  float	Generator_xpdf2;
  bool	HLT_Ele30_WPTight_Gsf;
  bool	HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07;
  bool	HLT_IsoMu24;
  bool	HLT_IsoMu27;
  bool	HLT_PFHT1050;
  bool	HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p55;
  bool	HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p60;
  bool	HLT_PFHT280_QuadPFJet35_PNet2BTagMean0p60;
  bool	HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0;
  bool	HLT_PFHT330PT30_QuadPFJet_75_60_45_40;
  bool	HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_2p0;
  bool	HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3;
  bool	HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5;
  bool	HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5;
  bool	HLT_PFHT340_QuadPFJet70_50_40_40_PNet2BTagMean0p70;
  bool	HLT_PFHT380_SixJet32_DoubleBTagCSV_p075;
  bool	HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2;
  bool	HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_4p3;
  bool	HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_5p6;
  bool	HLT_PFHT400_SixPFJet32;
  bool	HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94;
  bool	HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50;
  bool	HLT_PFHT430_SixJet40_BTagCSV_p080;
  bool	HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5;
  bool	HLT_PFHT450_SixPFJet36;
  bool	HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59;
  bool	HLT_PFHT450_SixPFJet36_PNetBTag0p35;
  bool	HLTriggerFinalPath;
  bool	HLTriggerFirstPath;
  float	HTXS_Higgs_pt;
  float	HTXS_Higgs_y;
  int	HTXS_njets25;
  int	HTXS_njets30;
  int	HTXS_stage1_1_cat_pTjet25GeV;
  int	HTXS_stage1_1_cat_pTjet30GeV;
  int	HTXS_stage1_1_fine_cat_pTjet25GeV;
  int	HTXS_stage1_1_fine_cat_pTjet30GeV;
  int	HTXS_stage1_2_cat_pTjet25GeV;
  int	HTXS_stage1_2_cat_pTjet30GeV;
  int	HTXS_stage1_2_fine_cat_pTjet25GeV;
  int	HTXS_stage1_2_fine_cat_pTjet30GeV;
  int	HTXS_stage_0;
  int	HTXS_stage_1_pTjet25;
  int	HTXS_stage_1_pTjet30;
  float	L1PreFiringWeight_Dn;
  float	L1PreFiringWeight_ECAL_Dn;
  float	L1PreFiringWeight_ECAL_Nom;
  float	L1PreFiringWeight_ECAL_Up;
  float	L1PreFiringWeight_Muon_Nom;
  float	L1PreFiringWeight_Muon_StatDn;
  float	L1PreFiringWeight_Muon_StatUp;
  float	L1PreFiringWeight_Muon_SystDn;
  float	L1PreFiringWeight_Muon_SystUp;
  float	L1PreFiringWeight_Nom;
  float	L1PreFiringWeight_Up;
  bool	L1Reco_step;
  bool	L1simulation_step;
  float	LHEWeight_originalXWGTUP;
  float	LHE_AlphaS;
  float	LHE_HT;
  float	LHE_HTIncoming;
  int	LHE_Nb;
  int	LHE_Nc;
  int	LHE_Nglu;
  int	LHE_Njets;
  int	LHE_NpLO;
  int	LHE_NpNLO;
  int	LHE_Nuds;
  float	LHE_Vpt;
  float	MET_MetUnclustEnUpDeltaX;
  float	MET_MetUnclustEnUpDeltaY;
  float	MET_covXX;
  float	MET_covXY;
  float	MET_covYY;
  float	MET_fiducialGenPhi;
  float	MET_fiducialGenPt;
  float	MET_phi;
  float	MET_pt;
  float	MET_significance;
  float	MET_sumEt;
  float	MET_sumPtUnclustered;
  float	PFMET_covXX;
  float	PFMET_covXY;
  float	PFMET_covYY;
  float	PFMET_phi;
  float	PFMET_phiUnclusteredDown;
  float	PFMET_phiUnclusteredUp;
  float	PFMET_pt;
  float	PFMET_ptUnclusteredDown;
  float	PFMET_ptUnclusteredUp;
  float	PFMET_significance;
  float	PFMET_sumEt;
  float	PFMET_sumPtUnclustered;
  float	PV_chi2;
  float	PV_ndof;
  int	PV_npvs;
  int	PV_npvsGood;
  float	PV_score;
  float	PV_x;
  float	PV_y;
  float	PV_z;
  float	Pileup_gpudensity;
  int	Pileup_nPU;
  float	Pileup_nTrueInt;
  float	Pileup_pudensity;
  int	Pileup_sumEOOT;
  int	Pileup_sumLOOT;
  float	PuppiMET_covXX;
  float	PuppiMET_covXY;
  float	PuppiMET_covYY;
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
  float	PuppiMET_significance;
  float	PuppiMET_sumEt;
  float	PuppiMET_sumPtUnclustered;
  float	RawMET_phi;
  float	RawMET_pt;
  float	RawMET_sumEt;
  float	RawPFMET_phi;
  float	RawPFMET_pt;
  float	RawPFMET_sumEt;
  float	RawPuppiMET_phi;
  float	RawPuppiMET_pt;
  float	RawPuppiMET_sumEt;
  float	Rho_fixedGridRhoAll;
  float	Rho_fixedGridRhoFastjetAll;
  float	Rho_fixedGridRhoFastjetCentral;
  float	Rho_fixedGridRhoFastjetCentralCalo;
  float	Rho_fixedGridRhoFastjetCentralChargedPileUp;
  float	Rho_fixedGridRhoFastjetCentralNeutral;
  float	SoftActivityJetHT;
  float	SoftActivityJetHT10;
  float	SoftActivityJetHT2;
  float	SoftActivityJetHT5;
  int	SoftActivityJetNjets10;
  int	SoftActivityJetNjets2;
  int	SoftActivityJetNjets5;
  float	TkMET_phi;
  float	TkMET_pt;
  float	TkMET_sumEt;
  float	btagWeight_CSVV2;
  float	btagWeight_DeepCSVB;
  long	event;
  float	fixedGridRhoFastjetAll;
  float	fixedGridRhoFastjetCentral;
  float	fixedGridRhoFastjetCentralCalo;
  float	fixedGridRhoFastjetCentralChargedPileUp;
  float	fixedGridRhoFastjetCentralNeutral;
  int	genTtbarId;
  float	genWeight;
  unsigned int	luminosityBlock;
  unsigned int	run;

  //--------------------------------------------------------------------------
  // --- Structs can be filled by calling fill(), or individual fill
  // --- methods, e.g., fillElectrons()
  // --- after the call to read(...)
  //----------- --------------------------------------------------------------
  struct CorrT1METJet_s
  {
    float	area;
    float	eta;
    float	muonSubtrFactor;
    float	phi;
    float	rawPt;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "CorrT1METJet" << std::endl;
      sprintf(r, "  %-32s: %f\n", "area", ( double)area); os << r;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "muonSubtrFactor", ( double)muonSubtrFactor); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "rawPt", ( double)rawPt); os << r;
      return os;
    }
  };

  struct Electron_s
  {
    int	charge;
    bool	convVeto;
    int	cutBased;
    float	deltaEtaSC;
    float	dxy;
    float	dz;
    float	eta;
    float	ip3d;
    bool	isEB;
    int	jetIdx;
    int	lostHits;
    float	mass;
    float	miniPFRelIso_all;
    float	miniPFRelIso_chg;
    float	mvaIso;
    bool	mvaIso_WP80;
    bool	mvaIso_WP90;
    float	mvaNoIso;
    bool	mvaNoIso_WP80;
    bool	mvaNoIso_WP90;
    int	pdgId;
    float	pfRelIso03_all;
    float	pfRelIso03_chg;
    float	pfRelIso04_all;
    float	phi;
    float	promptMVA;
    float	pt;
    float	r9;
    float	scEtOverPt;
    int	seedGain;
    float	sip3d;
    float	superclusterEta;
    int	tightCharge;
    int	vidNestedWPBitmap;
    int	genPartFlav;
    int	genPartIdx;
    bool	mvaIso_WPL;
    bool	mvaNoIso_WPL;
    int	cleanmask;
    bool	cutBased_HEEP;
    float	dEscaleDown;
    float	dEscaleUp;
    float	dEsigmaDown;
    float	dEsigmaUp;
    float	dr03EcalRecHitSumEt;
    float	dr03HcalDepth1TowerSumEt;
    float	dr03TkSumPt;
    float	dr03TkSumPtHEEP;
    float	dxyErr;
    float	dzErr;
    float	eCorr;
    float	eInvMinusPInv;
    float	energyErr;
    float	hoe;
    bool	isPFcand;
    int	jetNDauCharged;
    float	jetPtRelv2;
    float	jetRelIso;
    float	mvaFall17V2Iso;
    bool	mvaFall17V2Iso_WP80;
    bool	mvaFall17V2Iso_WP90;
    bool	mvaFall17V2Iso_WPL;
    float	mvaFall17V2noIso;
    bool	mvaFall17V2noIso_WP80;
    bool	mvaFall17V2noIso_WP90;
    bool	mvaFall17V2noIso_WPL;
    float	mvaTTH;
    int	photonIdx;
    float	sieie;
    int	vidNestedWPBitmapHEEP;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "Electron" << std::endl;
      sprintf(r, "  %-32s: %f\n", "charge", ( double)charge); os << r;
      sprintf(r, "  %-32s: %f\n", "convVeto", ( double)convVeto); os << r;
      sprintf(r, "  %-32s: %f\n", "cutBased", ( double)cutBased); os << r;
      sprintf(r, "  %-32s: %f\n", "deltaEtaSC", ( double)deltaEtaSC); os << r;
      sprintf(r, "  %-32s: %f\n", "dxy", ( double)dxy); os << r;
      sprintf(r, "  %-32s: %f\n", "dz", ( double)dz); os << r;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "ip3d", ( double)ip3d); os << r;
      sprintf(r, "  %-32s: %f\n", "isEB", ( double)isEB); os << r;
      sprintf(r, "  %-32s: %f\n", "jetIdx", ( double)jetIdx); os << r;
      sprintf(r, "  %-32s: %f\n", "lostHits", ( double)lostHits); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "miniPFRelIso_all", ( double)miniPFRelIso_all); os << r;
      sprintf(r, "  %-32s: %f\n", "miniPFRelIso_chg", ( double)miniPFRelIso_chg); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaIso", ( double)mvaIso); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaIso_WP80", ( double)mvaIso_WP80); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaIso_WP90", ( double)mvaIso_WP90); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaNoIso", ( double)mvaNoIso); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaNoIso_WP80", ( double)mvaNoIso_WP80); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaNoIso_WP90", ( double)mvaNoIso_WP90); os << r;
      sprintf(r, "  %-32s: %f\n", "pdgId", ( double)pdgId); os << r;
      sprintf(r, "  %-32s: %f\n", "pfRelIso03_all", ( double)pfRelIso03_all); os << r;
      sprintf(r, "  %-32s: %f\n", "pfRelIso03_chg", ( double)pfRelIso03_chg); os << r;
      sprintf(r, "  %-32s: %f\n", "pfRelIso04_all", ( double)pfRelIso04_all); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "promptMVA", ( double)promptMVA); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      sprintf(r, "  %-32s: %f\n", "r9", ( double)r9); os << r;
      sprintf(r, "  %-32s: %f\n", "scEtOverPt", ( double)scEtOverPt); os << r;
      sprintf(r, "  %-32s: %f\n", "seedGain", ( double)seedGain); os << r;
      sprintf(r, "  %-32s: %f\n", "sip3d", ( double)sip3d); os << r;
      sprintf(r, "  %-32s: %f\n", "superclusterEta", ( double)superclusterEta); os << r;
      sprintf(r, "  %-32s: %f\n", "tightCharge", ( double)tightCharge); os << r;
      sprintf(r, "  %-32s: %f\n", "vidNestedWPBitmap", ( double)vidNestedWPBitmap); os << r;
      sprintf(r, "  %-32s: %f\n", "genPartFlav", ( double)genPartFlav); os << r;
      sprintf(r, "  %-32s: %f\n", "genPartIdx", ( double)genPartIdx); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaIso_WPL", ( double)mvaIso_WPL); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaNoIso_WPL", ( double)mvaNoIso_WPL); os << r;
      sprintf(r, "  %-32s: %f\n", "cleanmask", ( double)cleanmask); os << r;
      sprintf(r, "  %-32s: %f\n", "cutBased_HEEP", ( double)cutBased_HEEP); os << r;
      sprintf(r, "  %-32s: %f\n", "dEscaleDown", ( double)dEscaleDown); os << r;
      sprintf(r, "  %-32s: %f\n", "dEscaleUp", ( double)dEscaleUp); os << r;
      sprintf(r, "  %-32s: %f\n", "dEsigmaDown", ( double)dEsigmaDown); os << r;
      sprintf(r, "  %-32s: %f\n", "dEsigmaUp", ( double)dEsigmaUp); os << r;
      sprintf(r, "  %-32s: %f\n", "dr03EcalRecHitSumEt", ( double)dr03EcalRecHitSumEt); os << r;
      sprintf(r, "  %-32s: %f\n", "dr03HcalDepth1TowerSumEt", ( double)dr03HcalDepth1TowerSumEt); os << r;
      sprintf(r, "  %-32s: %f\n", "dr03TkSumPt", ( double)dr03TkSumPt); os << r;
      sprintf(r, "  %-32s: %f\n", "dr03TkSumPtHEEP", ( double)dr03TkSumPtHEEP); os << r;
      sprintf(r, "  %-32s: %f\n", "dxyErr", ( double)dxyErr); os << r;
      sprintf(r, "  %-32s: %f\n", "dzErr", ( double)dzErr); os << r;
      sprintf(r, "  %-32s: %f\n", "eCorr", ( double)eCorr); os << r;
      sprintf(r, "  %-32s: %f\n", "eInvMinusPInv", ( double)eInvMinusPInv); os << r;
      sprintf(r, "  %-32s: %f\n", "energyErr", ( double)energyErr); os << r;
      sprintf(r, "  %-32s: %f\n", "hoe", ( double)hoe); os << r;
      sprintf(r, "  %-32s: %f\n", "isPFcand", ( double)isPFcand); os << r;
      sprintf(r, "  %-32s: %f\n", "jetNDauCharged", ( double)jetNDauCharged); os << r;
      sprintf(r, "  %-32s: %f\n", "jetPtRelv2", ( double)jetPtRelv2); os << r;
      sprintf(r, "  %-32s: %f\n", "jetRelIso", ( double)jetRelIso); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaFall17V2Iso", ( double)mvaFall17V2Iso); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaFall17V2Iso_WP80", ( double)mvaFall17V2Iso_WP80); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaFall17V2Iso_WP90", ( double)mvaFall17V2Iso_WP90); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaFall17V2Iso_WPL", ( double)mvaFall17V2Iso_WPL); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaFall17V2noIso", ( double)mvaFall17V2noIso); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaFall17V2noIso_WP80", ( double)mvaFall17V2noIso_WP80); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaFall17V2noIso_WP90", ( double)mvaFall17V2noIso_WP90); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaFall17V2noIso_WPL", ( double)mvaFall17V2noIso_WPL); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaTTH", ( double)mvaTTH); os << r;
      sprintf(r, "  %-32s: %f\n", "photonIdx", ( double)photonIdx); os << r;
      sprintf(r, "  %-32s: %f\n", "sieie", ( double)sieie); os << r;
      sprintf(r, "  %-32s: %f\n", "vidNestedWPBitmapHEEP", ( double)vidNestedWPBitmapHEEP); os << r;
      return os;
    }
  };

  struct FatJet_s
  {
    float	area;
    float	btagCSVV2;
    float	btagDDBvLV2;
    float	btagDDCvBV2;
    float	btagDDCvLV2;
    float	btagDeepB;
    float	btagHbb;
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
    int	electronIdx3SJ;
    float	eta;
    int	genJetAK8Idx;
    int	hadronFlavour;
    int	jetId;
    float	lsf3;
    float	mass;
    float	msoftdrop;
    int	muonIdx3SJ;
    float	n2b1;
    float	n3b1;
    int	nBHadrons;
    int	nCHadrons;
    int	nConstituents;
    float	particleNetMD_QCD;
    float	particleNetMD_Xbb;
    float	particleNetMD_Xcc;
    float	particleNetMD_Xqq;
    float	particleNet_H4qvsQCD;
    float	particleNet_HbbvsQCD;
    float	particleNet_HccvsQCD;
    float	particleNet_QCD;
    float	particleNet_TvsQCD;
    float	particleNet_WvsQCD;
    float	particleNet_ZvsQCD;
    float	particleNet_mass;
    float	phi;
    float	pt;
    float	rawFactor;
    int	subJetIdx1;
    int	subJetIdx2;
    float	tau1;
    float	tau2;
    float	tau3;
    float	tau4;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "FatJet" << std::endl;
      sprintf(r, "  %-32s: %f\n", "area", ( double)area); os << r;
      sprintf(r, "  %-32s: %f\n", "btagCSVV2", ( double)btagCSVV2); os << r;
      sprintf(r, "  %-32s: %f\n", "btagDDBvLV2", ( double)btagDDBvLV2); os << r;
      sprintf(r, "  %-32s: %f\n", "btagDDCvBV2", ( double)btagDDCvBV2); os << r;
      sprintf(r, "  %-32s: %f\n", "btagDDCvLV2", ( double)btagDDCvLV2); os << r;
      sprintf(r, "  %-32s: %f\n", "btagDeepB", ( double)btagDeepB); os << r;
      sprintf(r, "  %-32s: %f\n", "btagHbb", ( double)btagHbb); os << r;
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
      sprintf(r, "  %-32s: %f\n", "electronIdx3SJ", ( double)electronIdx3SJ); os << r;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "genJetAK8Idx", ( double)genJetAK8Idx); os << r;
      sprintf(r, "  %-32s: %f\n", "hadronFlavour", ( double)hadronFlavour); os << r;
      sprintf(r, "  %-32s: %f\n", "jetId", ( double)jetId); os << r;
      sprintf(r, "  %-32s: %f\n", "lsf3", ( double)lsf3); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "msoftdrop", ( double)msoftdrop); os << r;
      sprintf(r, "  %-32s: %f\n", "muonIdx3SJ", ( double)muonIdx3SJ); os << r;
      sprintf(r, "  %-32s: %f\n", "n2b1", ( double)n2b1); os << r;
      sprintf(r, "  %-32s: %f\n", "n3b1", ( double)n3b1); os << r;
      sprintf(r, "  %-32s: %f\n", "nBHadrons", ( double)nBHadrons); os << r;
      sprintf(r, "  %-32s: %f\n", "nCHadrons", ( double)nCHadrons); os << r;
      sprintf(r, "  %-32s: %f\n", "nConstituents", ( double)nConstituents); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNetMD_QCD", ( double)particleNetMD_QCD); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNetMD_Xbb", ( double)particleNetMD_Xbb); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNetMD_Xcc", ( double)particleNetMD_Xcc); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNetMD_Xqq", ( double)particleNetMD_Xqq); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNet_H4qvsQCD", ( double)particleNet_H4qvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNet_HbbvsQCD", ( double)particleNet_HbbvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNet_HccvsQCD", ( double)particleNet_HccvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNet_QCD", ( double)particleNet_QCD); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNet_TvsQCD", ( double)particleNet_TvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNet_WvsQCD", ( double)particleNet_WvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNet_ZvsQCD", ( double)particleNet_ZvsQCD); os << r;
      sprintf(r, "  %-32s: %f\n", "particleNet_mass", ( double)particleNet_mass); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      sprintf(r, "  %-32s: %f\n", "rawFactor", ( double)rawFactor); os << r;
      sprintf(r, "  %-32s: %f\n", "subJetIdx1", ( double)subJetIdx1); os << r;
      sprintf(r, "  %-32s: %f\n", "subJetIdx2", ( double)subJetIdx2); os << r;
      sprintf(r, "  %-32s: %f\n", "tau1", ( double)tau1); os << r;
      sprintf(r, "  %-32s: %f\n", "tau2", ( double)tau2); os << r;
      sprintf(r, "  %-32s: %f\n", "tau3", ( double)tau3); os << r;
      sprintf(r, "  %-32s: %f\n", "tau4", ( double)tau4); os << r;
      return os;
    }
  };

  struct FsrPhoton_s
  {
    float	dROverEt2;
    float	eta;
    int	muonIdx;
    float	phi;
    float	pt;
    float	relIso03;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "FsrPhoton" << std::endl;
      sprintf(r, "  %-32s: %f\n", "dROverEt2", ( double)dROverEt2); os << r;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "muonIdx", ( double)muonIdx); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      sprintf(r, "  %-32s: %f\n", "relIso03", ( double)relIso03); os << r;
      return os;
    }
  };

  struct GenDressedLepton_s
  {
    float	eta;
    bool	hasTauAnc;
    float	mass;
    int	pdgId;
    float	phi;
    float	pt;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "GenDressedLepton" << std::endl;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "hasTauAnc", ( double)hasTauAnc); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "pdgId", ( double)pdgId); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      return os;
    }
  };

  struct GenIsolatedPhoton_s
  {
    float	eta;
    float	mass;
    float	phi;
    float	pt;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "GenIsolatedPhoton" << std::endl;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      return os;
    }
  };

  struct GenJet_s
  {
    float	eta;
    int	hadronFlavour;
    float	mass;
    int	nBHadrons;
    int	nCHadrons;
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
      sprintf(r, "  %-32s: %f\n", "nBHadrons", ( double)nBHadrons); os << r;
      sprintf(r, "  %-32s: %f\n", "nCHadrons", ( double)nCHadrons); os << r;
      sprintf(r, "  %-32s: %f\n", "partonFlavour", ( double)partonFlavour); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      return os;
    }
  };

  struct GenJetAK8_s
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
      os << "GenJetAK8" << std::endl;
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

  struct GenVisTau_s
  {
    int	charge;
    float	eta;
    int	genPartIdxMother;
    float	mass;
    float	phi;
    float	pt;
    int	status;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "GenVisTau" << std::endl;
      sprintf(r, "  %-32s: %f\n", "charge", ( double)charge); os << r;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "genPartIdxMother", ( double)genPartIdxMother); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      sprintf(r, "  %-32s: %f\n", "status", ( double)status); os << r;
      return os;
    }
  };

  struct IsoTrack_s
  {
    int	charge;
    float	dxy;
    float	dz;
    float	eta;
    int	fromPV;
    bool	isFromLostTrack;
    bool	isHighPurityTrack;
    bool	isPFcand;
    float	miniPFRelIso_all;
    float	miniPFRelIso_chg;
    int	pdgId;
    float	pfRelIso03_all;
    float	pfRelIso03_chg;
    float	phi;
    float	pt;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "IsoTrack" << std::endl;
      sprintf(r, "  %-32s: %f\n", "charge", ( double)charge); os << r;
      sprintf(r, "  %-32s: %f\n", "dxy", ( double)dxy); os << r;
      sprintf(r, "  %-32s: %f\n", "dz", ( double)dz); os << r;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "fromPV", ( double)fromPV); os << r;
      sprintf(r, "  %-32s: %f\n", "isFromLostTrack", ( double)isFromLostTrack); os << r;
      sprintf(r, "  %-32s: %f\n", "isHighPurityTrack", ( double)isHighPurityTrack); os << r;
      sprintf(r, "  %-32s: %f\n", "isPFcand", ( double)isPFcand); os << r;
      sprintf(r, "  %-32s: %f\n", "miniPFRelIso_all", ( double)miniPFRelIso_all); os << r;
      sprintf(r, "  %-32s: %f\n", "miniPFRelIso_chg", ( double)miniPFRelIso_chg); os << r;
      sprintf(r, "  %-32s: %f\n", "pdgId", ( double)pdgId); os << r;
      sprintf(r, "  %-32s: %f\n", "pfRelIso03_all", ( double)pfRelIso03_all); os << r;
      sprintf(r, "  %-32s: %f\n", "pfRelIso03_chg", ( double)pfRelIso03_chg); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      return os;
    }
  };

  struct Jet_s
  {
    float	PNetRegPtRawCorr;
    float	PNetRegPtRawCorrNeutrino;
    float	PNetRegPtRawRes;
    float	UParTAK4RegPtRawCorr;
    float	UParTAK4RegPtRawCorrNeutrino;
    float	UParTAK4RegPtRawRes;
    float	UParTAK4V1RegPtRawCorr;
    float	UParTAK4V1RegPtRawCorrNeutrino;
    float	UParTAK4V1RegPtRawRes;
    float	area;
    float	btagDeepFlavB;
    float	btagDeepFlavCvB;
    float	btagDeepFlavCvL;
    float	btagDeepFlavQG;
    float	btagPNetB;
    float	btagPNetCvB;
    float	btagPNetCvL;
    float	btagPNetQvG;
    float	btagUParTAK4B;
    float	btagUParTAK4CvB;
    float	btagUParTAK4CvL;
    float	btagUParTAK4QvG;
    float	chEmEF;
    float	chHEF;
    int	chMultiplicity;
    int	electronIdx1;
    int	electronIdx2;
    float	eta;
    float	hfEmEF;
    float	hfHEF;
    float	mass;
    float	muEF;
    int	muonIdx1;
    int	muonIdx2;
    float	muonSubtrFactor;
    int	nConstituents;
    int	nElectrons;
    int	nMuons;
    float	neEmEF;
    float	neHEF;
    int	neMultiplicity;
    float	phi;
    float	pt;
    float	puIdDisc;
    float	rawFactor;
    int	genJetIdx;
    int	hadronFlavour;
    int	partonFlavour;
    float	bRegCorr;
    float	bRegRes;
    float	btagCSVV2;
    float	btagDeepB;
    float	btagDeepCvB;
    float	btagDeepCvL;
    float	cRegCorr;
    float	cRegRes;
    float	chFPV0EF;
    int	cleanmask;
    int	hfadjacentEtaStripsSize;
    int	hfcentralEtaStripSize;
    float	hfsigmaEtaEta;
    float	hfsigmaPhiPhi;
    int	jetId;
    int	puId;
    float	qgl;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "Jet" << std::endl;
      sprintf(r, "  %-32s: %f\n", "PNetRegPtRawCorr", ( double)PNetRegPtRawCorr); os << r;
      sprintf(r, "  %-32s: %f\n", "PNetRegPtRawCorrNeutrino", ( double)PNetRegPtRawCorrNeutrino); os << r;
      sprintf(r, "  %-32s: %f\n", "PNetRegPtRawRes", ( double)PNetRegPtRawRes); os << r;
      sprintf(r, "  %-32s: %f\n", "UParTAK4RegPtRawCorr", ( double)UParTAK4RegPtRawCorr); os << r;
      sprintf(r, "  %-32s: %f\n", "UParTAK4RegPtRawCorrNeutrino", ( double)UParTAK4RegPtRawCorrNeutrino); os << r;
      sprintf(r, "  %-32s: %f\n", "UParTAK4RegPtRawRes", ( double)UParTAK4RegPtRawRes); os << r;
      sprintf(r, "  %-32s: %f\n", "UParTAK4V1RegPtRawCorr", ( double)UParTAK4V1RegPtRawCorr); os << r;
      sprintf(r, "  %-32s: %f\n", "UParTAK4V1RegPtRawCorrNeutrino", ( double)UParTAK4V1RegPtRawCorrNeutrino); os << r;
      sprintf(r, "  %-32s: %f\n", "UParTAK4V1RegPtRawRes", ( double)UParTAK4V1RegPtRawRes); os << r;
      sprintf(r, "  %-32s: %f\n", "area", ( double)area); os << r;
      sprintf(r, "  %-32s: %f\n", "btagDeepFlavB", ( double)btagDeepFlavB); os << r;
      sprintf(r, "  %-32s: %f\n", "btagDeepFlavCvB", ( double)btagDeepFlavCvB); os << r;
      sprintf(r, "  %-32s: %f\n", "btagDeepFlavCvL", ( double)btagDeepFlavCvL); os << r;
      sprintf(r, "  %-32s: %f\n", "btagDeepFlavQG", ( double)btagDeepFlavQG); os << r;
      sprintf(r, "  %-32s: %f\n", "btagPNetB", ( double)btagPNetB); os << r;
      sprintf(r, "  %-32s: %f\n", "btagPNetCvB", ( double)btagPNetCvB); os << r;
      sprintf(r, "  %-32s: %f\n", "btagPNetCvL", ( double)btagPNetCvL); os << r;
      sprintf(r, "  %-32s: %f\n", "btagPNetQvG", ( double)btagPNetQvG); os << r;
      sprintf(r, "  %-32s: %f\n", "btagUParTAK4B", ( double)btagUParTAK4B); os << r;
      sprintf(r, "  %-32s: %f\n", "btagUParTAK4CvB", ( double)btagUParTAK4CvB); os << r;
      sprintf(r, "  %-32s: %f\n", "btagUParTAK4CvL", ( double)btagUParTAK4CvL); os << r;
      sprintf(r, "  %-32s: %f\n", "btagUParTAK4QvG", ( double)btagUParTAK4QvG); os << r;
      sprintf(r, "  %-32s: %f\n", "chEmEF", ( double)chEmEF); os << r;
      sprintf(r, "  %-32s: %f\n", "chHEF", ( double)chHEF); os << r;
      sprintf(r, "  %-32s: %f\n", "chMultiplicity", ( double)chMultiplicity); os << r;
      sprintf(r, "  %-32s: %f\n", "electronIdx1", ( double)electronIdx1); os << r;
      sprintf(r, "  %-32s: %f\n", "electronIdx2", ( double)electronIdx2); os << r;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "hfEmEF", ( double)hfEmEF); os << r;
      sprintf(r, "  %-32s: %f\n", "hfHEF", ( double)hfHEF); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "muEF", ( double)muEF); os << r;
      sprintf(r, "  %-32s: %f\n", "muonIdx1", ( double)muonIdx1); os << r;
      sprintf(r, "  %-32s: %f\n", "muonIdx2", ( double)muonIdx2); os << r;
      sprintf(r, "  %-32s: %f\n", "muonSubtrFactor", ( double)muonSubtrFactor); os << r;
      sprintf(r, "  %-32s: %f\n", "nConstituents", ( double)nConstituents); os << r;
      sprintf(r, "  %-32s: %f\n", "nElectrons", ( double)nElectrons); os << r;
      sprintf(r, "  %-32s: %f\n", "nMuons", ( double)nMuons); os << r;
      sprintf(r, "  %-32s: %f\n", "neEmEF", ( double)neEmEF); os << r;
      sprintf(r, "  %-32s: %f\n", "neHEF", ( double)neHEF); os << r;
      sprintf(r, "  %-32s: %f\n", "neMultiplicity", ( double)neMultiplicity); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      sprintf(r, "  %-32s: %f\n", "puIdDisc", ( double)puIdDisc); os << r;
      sprintf(r, "  %-32s: %f\n", "rawFactor", ( double)rawFactor); os << r;
      sprintf(r, "  %-32s: %f\n", "genJetIdx", ( double)genJetIdx); os << r;
      sprintf(r, "  %-32s: %f\n", "hadronFlavour", ( double)hadronFlavour); os << r;
      sprintf(r, "  %-32s: %f\n", "partonFlavour", ( double)partonFlavour); os << r;
      sprintf(r, "  %-32s: %f\n", "bRegCorr", ( double)bRegCorr); os << r;
      sprintf(r, "  %-32s: %f\n", "bRegRes", ( double)bRegRes); os << r;
      sprintf(r, "  %-32s: %f\n", "btagCSVV2", ( double)btagCSVV2); os << r;
      sprintf(r, "  %-32s: %f\n", "btagDeepB", ( double)btagDeepB); os << r;
      sprintf(r, "  %-32s: %f\n", "btagDeepCvB", ( double)btagDeepCvB); os << r;
      sprintf(r, "  %-32s: %f\n", "btagDeepCvL", ( double)btagDeepCvL); os << r;
      sprintf(r, "  %-32s: %f\n", "cRegCorr", ( double)cRegCorr); os << r;
      sprintf(r, "  %-32s: %f\n", "cRegRes", ( double)cRegRes); os << r;
      sprintf(r, "  %-32s: %f\n", "chFPV0EF", ( double)chFPV0EF); os << r;
      sprintf(r, "  %-32s: %f\n", "cleanmask", ( double)cleanmask); os << r;
      sprintf(r, "  %-32s: %f\n", "hfadjacentEtaStripsSize", ( double)hfadjacentEtaStripsSize); os << r;
      sprintf(r, "  %-32s: %f\n", "hfcentralEtaStripSize", ( double)hfcentralEtaStripSize); os << r;
      sprintf(r, "  %-32s: %f\n", "hfsigmaEtaEta", ( double)hfsigmaEtaEta); os << r;
      sprintf(r, "  %-32s: %f\n", "hfsigmaPhiPhi", ( double)hfsigmaPhiPhi); os << r;
      sprintf(r, "  %-32s: %f\n", "jetId", ( double)jetId); os << r;
      sprintf(r, "  %-32s: %f\n", "puId", ( double)puId); os << r;
      sprintf(r, "  %-32s: %f\n", "qgl", ( double)qgl); os << r;
      return os;
    }
  };

  struct LHEPart_s
  {
    float	eta;
    float	incomingpz;
    float	mass;
    int	pdgId;
    float	phi;
    float	pt;
    int	spin;
    int	status;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "LHEPart" << std::endl;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "incomingpz", ( double)incomingpz); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "pdgId", ( double)pdgId); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      sprintf(r, "  %-32s: %f\n", "spin", ( double)spin); os << r;
      sprintf(r, "  %-32s: %f\n", "status", ( double)status); os << r;
      return os;
    }
  };

  struct LowPtElectron_s
  {
    float	ID;
    int	charge;
    bool	convVeto;
    float	convVtxRadius;
    int	convWP;
    float	deltaEtaSC;
    float	dxy;
    float	dxyErr;
    float	dz;
    float	dzErr;
    float	eInvMinusPInv;
    float	embeddedID;
    float	energyErr;
    float	eta;
    int	genPartFlav;
    int	genPartIdx;
    float	hoe;
    int	lostHits;
    float	mass;
    float	miniPFRelIso_all;
    float	miniPFRelIso_chg;
    int	pdgId;
    float	phi;
    float	pt;
    float	ptbiased;
    float	r9;
    float	scEtOverPt;
    float	sieie;
    float	unbiased;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "LowPtElectron" << std::endl;
      sprintf(r, "  %-32s: %f\n", "ID", ( double)ID); os << r;
      sprintf(r, "  %-32s: %f\n", "charge", ( double)charge); os << r;
      sprintf(r, "  %-32s: %f\n", "convVeto", ( double)convVeto); os << r;
      sprintf(r, "  %-32s: %f\n", "convVtxRadius", ( double)convVtxRadius); os << r;
      sprintf(r, "  %-32s: %f\n", "convWP", ( double)convWP); os << r;
      sprintf(r, "  %-32s: %f\n", "deltaEtaSC", ( double)deltaEtaSC); os << r;
      sprintf(r, "  %-32s: %f\n", "dxy", ( double)dxy); os << r;
      sprintf(r, "  %-32s: %f\n", "dxyErr", ( double)dxyErr); os << r;
      sprintf(r, "  %-32s: %f\n", "dz", ( double)dz); os << r;
      sprintf(r, "  %-32s: %f\n", "dzErr", ( double)dzErr); os << r;
      sprintf(r, "  %-32s: %f\n", "eInvMinusPInv", ( double)eInvMinusPInv); os << r;
      sprintf(r, "  %-32s: %f\n", "embeddedID", ( double)embeddedID); os << r;
      sprintf(r, "  %-32s: %f\n", "energyErr", ( double)energyErr); os << r;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "genPartFlav", ( double)genPartFlav); os << r;
      sprintf(r, "  %-32s: %f\n", "genPartIdx", ( double)genPartIdx); os << r;
      sprintf(r, "  %-32s: %f\n", "hoe", ( double)hoe); os << r;
      sprintf(r, "  %-32s: %f\n", "lostHits", ( double)lostHits); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "miniPFRelIso_all", ( double)miniPFRelIso_all); os << r;
      sprintf(r, "  %-32s: %f\n", "miniPFRelIso_chg", ( double)miniPFRelIso_chg); os << r;
      sprintf(r, "  %-32s: %f\n", "pdgId", ( double)pdgId); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      sprintf(r, "  %-32s: %f\n", "ptbiased", ( double)ptbiased); os << r;
      sprintf(r, "  %-32s: %f\n", "r9", ( double)r9); os << r;
      sprintf(r, "  %-32s: %f\n", "scEtOverPt", ( double)scEtOverPt); os << r;
      sprintf(r, "  %-32s: %f\n", "sieie", ( double)sieie); os << r;
      sprintf(r, "  %-32s: %f\n", "unbiased", ( double)unbiased); os << r;
      return os;
    }
  };

  struct Muon_s
  {
    int	charge;
    float	dxy;
    float	dz;
    float	eta;
    int	highPtId;
    float	ip3d;
    bool	isGlobal;
    bool	isPFcand;
    bool	isTracker;
    int	jetIdx;
    bool	looseId;
    float	mass;
    bool	mediumId;
    bool	mediumPromptId;
    int	miniIsoId;
    float	miniPFRelIso_all;
    float	miniPFRelIso_chg;
    float	mvaMuID;
    int	mvaMuID_WP;
    int	nStations;
    int	nTrackerLayers;
    int	pdgId;
    int	pfIsoId;
    float	pfRelIso03_all;
    float	pfRelIso03_chg;
    float	pfRelIso04_all;
    float	phi;
    float	promptMVA;
    float	pt;
    float	ptErr;
    float	sip3d;
    int	tightCharge;
    bool	tightId;
    int	tkIsoId;
    float	tkRelIso;
    int	genPartFlav;
    int	genPartIdx;
    int	cleanmask;
    float	dxyErr;
    float	dxybs;
    float	dzErr;
    int	fsrPhotonIdx;
    bool	highPurity;
    bool	inTimeMuon;
    bool	isStandalone;
    int	jetNDauCharged;
    float	jetPtRelv2;
    float	jetRelIso;
    int	multiIsoId;
    int	mvaId;
    float	mvaLowPt;
    int	mvaLowPtId;
    float	mvaTTH;
    int	puppiIsoId;
    float	segmentComp;
    bool	softId;
    float	softMva;
    bool	softMvaId;
    bool	triggerIdLoose;
    float	tunepRelPt;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "Muon" << std::endl;
      sprintf(r, "  %-32s: %f\n", "charge", ( double)charge); os << r;
      sprintf(r, "  %-32s: %f\n", "dxy", ( double)dxy); os << r;
      sprintf(r, "  %-32s: %f\n", "dz", ( double)dz); os << r;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "highPtId", ( double)highPtId); os << r;
      sprintf(r, "  %-32s: %f\n", "ip3d", ( double)ip3d); os << r;
      sprintf(r, "  %-32s: %f\n", "isGlobal", ( double)isGlobal); os << r;
      sprintf(r, "  %-32s: %f\n", "isPFcand", ( double)isPFcand); os << r;
      sprintf(r, "  %-32s: %f\n", "isTracker", ( double)isTracker); os << r;
      sprintf(r, "  %-32s: %f\n", "jetIdx", ( double)jetIdx); os << r;
      sprintf(r, "  %-32s: %f\n", "looseId", ( double)looseId); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "mediumId", ( double)mediumId); os << r;
      sprintf(r, "  %-32s: %f\n", "mediumPromptId", ( double)mediumPromptId); os << r;
      sprintf(r, "  %-32s: %f\n", "miniIsoId", ( double)miniIsoId); os << r;
      sprintf(r, "  %-32s: %f\n", "miniPFRelIso_all", ( double)miniPFRelIso_all); os << r;
      sprintf(r, "  %-32s: %f\n", "miniPFRelIso_chg", ( double)miniPFRelIso_chg); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaMuID", ( double)mvaMuID); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaMuID_WP", ( double)mvaMuID_WP); os << r;
      sprintf(r, "  %-32s: %f\n", "nStations", ( double)nStations); os << r;
      sprintf(r, "  %-32s: %f\n", "nTrackerLayers", ( double)nTrackerLayers); os << r;
      sprintf(r, "  %-32s: %f\n", "pdgId", ( double)pdgId); os << r;
      sprintf(r, "  %-32s: %f\n", "pfIsoId", ( double)pfIsoId); os << r;
      sprintf(r, "  %-32s: %f\n", "pfRelIso03_all", ( double)pfRelIso03_all); os << r;
      sprintf(r, "  %-32s: %f\n", "pfRelIso03_chg", ( double)pfRelIso03_chg); os << r;
      sprintf(r, "  %-32s: %f\n", "pfRelIso04_all", ( double)pfRelIso04_all); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "promptMVA", ( double)promptMVA); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      sprintf(r, "  %-32s: %f\n", "ptErr", ( double)ptErr); os << r;
      sprintf(r, "  %-32s: %f\n", "sip3d", ( double)sip3d); os << r;
      sprintf(r, "  %-32s: %f\n", "tightCharge", ( double)tightCharge); os << r;
      sprintf(r, "  %-32s: %f\n", "tightId", ( double)tightId); os << r;
      sprintf(r, "  %-32s: %f\n", "tkIsoId", ( double)tkIsoId); os << r;
      sprintf(r, "  %-32s: %f\n", "tkRelIso", ( double)tkRelIso); os << r;
      sprintf(r, "  %-32s: %f\n", "genPartFlav", ( double)genPartFlav); os << r;
      sprintf(r, "  %-32s: %f\n", "genPartIdx", ( double)genPartIdx); os << r;
      sprintf(r, "  %-32s: %f\n", "cleanmask", ( double)cleanmask); os << r;
      sprintf(r, "  %-32s: %f\n", "dxyErr", ( double)dxyErr); os << r;
      sprintf(r, "  %-32s: %f\n", "dxybs", ( double)dxybs); os << r;
      sprintf(r, "  %-32s: %f\n", "dzErr", ( double)dzErr); os << r;
      sprintf(r, "  %-32s: %f\n", "fsrPhotonIdx", ( double)fsrPhotonIdx); os << r;
      sprintf(r, "  %-32s: %f\n", "highPurity", ( double)highPurity); os << r;
      sprintf(r, "  %-32s: %f\n", "inTimeMuon", ( double)inTimeMuon); os << r;
      sprintf(r, "  %-32s: %f\n", "isStandalone", ( double)isStandalone); os << r;
      sprintf(r, "  %-32s: %f\n", "jetNDauCharged", ( double)jetNDauCharged); os << r;
      sprintf(r, "  %-32s: %f\n", "jetPtRelv2", ( double)jetPtRelv2); os << r;
      sprintf(r, "  %-32s: %f\n", "jetRelIso", ( double)jetRelIso); os << r;
      sprintf(r, "  %-32s: %f\n", "multiIsoId", ( double)multiIsoId); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaId", ( double)mvaId); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaLowPt", ( double)mvaLowPt); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaLowPtId", ( double)mvaLowPtId); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaTTH", ( double)mvaTTH); os << r;
      sprintf(r, "  %-32s: %f\n", "puppiIsoId", ( double)puppiIsoId); os << r;
      sprintf(r, "  %-32s: %f\n", "segmentComp", ( double)segmentComp); os << r;
      sprintf(r, "  %-32s: %f\n", "softId", ( double)softId); os << r;
      sprintf(r, "  %-32s: %f\n", "softMva", ( double)softMva); os << r;
      sprintf(r, "  %-32s: %f\n", "softMvaId", ( double)softMvaId); os << r;
      sprintf(r, "  %-32s: %f\n", "triggerIdLoose", ( double)triggerIdLoose); os << r;
      sprintf(r, "  %-32s: %f\n", "tunepRelPt", ( double)tunepRelPt); os << r;
      return os;
    }
  };

  struct PPSLocalTrack_s
  {
    int	decRPId;
    int	multiRPProtonIdx;
    int	rpType;
    int	singleRPProtonIdx;
    float	time;
    float	timeUnc;
    float	x;
    float	y;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "PPSLocalTrack" << std::endl;
      sprintf(r, "  %-32s: %f\n", "decRPId", ( double)decRPId); os << r;
      sprintf(r, "  %-32s: %f\n", "multiRPProtonIdx", ( double)multiRPProtonIdx); os << r;
      sprintf(r, "  %-32s: %f\n", "rpType", ( double)rpType); os << r;
      sprintf(r, "  %-32s: %f\n", "singleRPProtonIdx", ( double)singleRPProtonIdx); os << r;
      sprintf(r, "  %-32s: %f\n", "time", ( double)time); os << r;
      sprintf(r, "  %-32s: %f\n", "timeUnc", ( double)timeUnc); os << r;
      sprintf(r, "  %-32s: %f\n", "x", ( double)x); os << r;
      sprintf(r, "  %-32s: %f\n", "y", ( double)y); os << r;
      return os;
    }
  };

  struct Photon_s
  {
    int	charge;
    int	cleanmask;
    int	cutBased;
    int	cutBased_Fall17V1Bitmap;
    float	dEscaleDown;
    float	dEscaleUp;
    float	dEsigmaDown;
    float	dEsigmaUp;
    float	eCorr;
    int	electronIdx;
    bool	electronVeto;
    float	energyErr;
    float	eta;
    int	genPartFlav;
    int	genPartIdx;
    float	hoe;
    bool	isScEtaEB;
    bool	isScEtaEE;
    int	jetIdx;
    float	mass;
    float	mvaID;
    float	mvaID_Fall17V1p1;
    bool	mvaID_WP80;
    bool	mvaID_WP90;
    int	pdgId;
    float	pfRelIso03_all;
    float	pfRelIso03_chg;
    float	phi;
    bool	pixelSeed;
    float	pt;
    float	r9;
    int	seedGain;
    float	sieie;
    int	vidNestedWPBitmap;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "Photon" << std::endl;
      sprintf(r, "  %-32s: %f\n", "charge", ( double)charge); os << r;
      sprintf(r, "  %-32s: %f\n", "cleanmask", ( double)cleanmask); os << r;
      sprintf(r, "  %-32s: %f\n", "cutBased", ( double)cutBased); os << r;
      sprintf(r, "  %-32s: %f\n", "cutBased_Fall17V1Bitmap", ( double)cutBased_Fall17V1Bitmap); os << r;
      sprintf(r, "  %-32s: %f\n", "dEscaleDown", ( double)dEscaleDown); os << r;
      sprintf(r, "  %-32s: %f\n", "dEscaleUp", ( double)dEscaleUp); os << r;
      sprintf(r, "  %-32s: %f\n", "dEsigmaDown", ( double)dEsigmaDown); os << r;
      sprintf(r, "  %-32s: %f\n", "dEsigmaUp", ( double)dEsigmaUp); os << r;
      sprintf(r, "  %-32s: %f\n", "eCorr", ( double)eCorr); os << r;
      sprintf(r, "  %-32s: %f\n", "electronIdx", ( double)electronIdx); os << r;
      sprintf(r, "  %-32s: %f\n", "electronVeto", ( double)electronVeto); os << r;
      sprintf(r, "  %-32s: %f\n", "energyErr", ( double)energyErr); os << r;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "genPartFlav", ( double)genPartFlav); os << r;
      sprintf(r, "  %-32s: %f\n", "genPartIdx", ( double)genPartIdx); os << r;
      sprintf(r, "  %-32s: %f\n", "hoe", ( double)hoe); os << r;
      sprintf(r, "  %-32s: %f\n", "isScEtaEB", ( double)isScEtaEB); os << r;
      sprintf(r, "  %-32s: %f\n", "isScEtaEE", ( double)isScEtaEE); os << r;
      sprintf(r, "  %-32s: %f\n", "jetIdx", ( double)jetIdx); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaID", ( double)mvaID); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaID_Fall17V1p1", ( double)mvaID_Fall17V1p1); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaID_WP80", ( double)mvaID_WP80); os << r;
      sprintf(r, "  %-32s: %f\n", "mvaID_WP90", ( double)mvaID_WP90); os << r;
      sprintf(r, "  %-32s: %f\n", "pdgId", ( double)pdgId); os << r;
      sprintf(r, "  %-32s: %f\n", "pfRelIso03_all", ( double)pfRelIso03_all); os << r;
      sprintf(r, "  %-32s: %f\n", "pfRelIso03_chg", ( double)pfRelIso03_chg); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pixelSeed", ( double)pixelSeed); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      sprintf(r, "  %-32s: %f\n", "r9", ( double)r9); os << r;
      sprintf(r, "  %-32s: %f\n", "seedGain", ( double)seedGain); os << r;
      sprintf(r, "  %-32s: %f\n", "sieie", ( double)sieie); os << r;
      sprintf(r, "  %-32s: %f\n", "vidNestedWPBitmap", ( double)vidNestedWPBitmap); os << r;
      return os;
    }
  };

  struct Proton_s
  {
    int	multiRP_arm;
    float	multiRP_t;
    float	multiRP_thetaX;
    float	multiRP_thetaY;
    float	multiRP_time;
    float	multiRP_timeUnc;
    float	multiRP_xi;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "Proton" << std::endl;
      sprintf(r, "  %-32s: %f\n", "multiRP_arm", ( double)multiRP_arm); os << r;
      sprintf(r, "  %-32s: %f\n", "multiRP_t", ( double)multiRP_t); os << r;
      sprintf(r, "  %-32s: %f\n", "multiRP_thetaX", ( double)multiRP_thetaX); os << r;
      sprintf(r, "  %-32s: %f\n", "multiRP_thetaY", ( double)multiRP_thetaY); os << r;
      sprintf(r, "  %-32s: %f\n", "multiRP_time", ( double)multiRP_time); os << r;
      sprintf(r, "  %-32s: %f\n", "multiRP_timeUnc", ( double)multiRP_timeUnc); os << r;
      sprintf(r, "  %-32s: %f\n", "multiRP_xi", ( double)multiRP_xi); os << r;
      return os;
    }
  };

  struct SV_s
  {
    int	charge;
    float	chi2;
    float	dlen;
    float	dlenSig;
    float	dxy;
    float	dxySig;
    float	eta;
    float	mass;
    float	ndof;
    int	ntracks;
    float	pAngle;
    float	phi;
    float	pt;
    float	x;
    float	y;
    float	z;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "SV" << std::endl;
      sprintf(r, "  %-32s: %f\n", "charge", ( double)charge); os << r;
      sprintf(r, "  %-32s: %f\n", "chi2", ( double)chi2); os << r;
      sprintf(r, "  %-32s: %f\n", "dlen", ( double)dlen); os << r;
      sprintf(r, "  %-32s: %f\n", "dlenSig", ( double)dlenSig); os << r;
      sprintf(r, "  %-32s: %f\n", "dxy", ( double)dxy); os << r;
      sprintf(r, "  %-32s: %f\n", "dxySig", ( double)dxySig); os << r;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "ndof", ( double)ndof); os << r;
      sprintf(r, "  %-32s: %f\n", "ntracks", ( double)ntracks); os << r;
      sprintf(r, "  %-32s: %f\n", "pAngle", ( double)pAngle); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      sprintf(r, "  %-32s: %f\n", "x", ( double)x); os << r;
      sprintf(r, "  %-32s: %f\n", "y", ( double)y); os << r;
      sprintf(r, "  %-32s: %f\n", "z", ( double)z); os << r;
      return os;
    }
  };

  struct SoftActivityJet_s
  {
    float	eta;
    float	phi;
    float	pt;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "SoftActivityJet" << std::endl;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      return os;
    }
  };

  struct SubGenJetAK8_s
  {
    float	eta;
    float	mass;
    float	phi;
    float	pt;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "SubGenJetAK8" << std::endl;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      return os;
    }
  };

  struct SubJet_s
  {
    float	btagCSVV2;
    float	btagDeepB;
    float	eta;
    int	hadronFlavour;
    float	mass;
    float	n2b1;
    float	n3b1;
    int	nBHadrons;
    int	nCHadrons;
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
      os << "SubJet" << std::endl;
      sprintf(r, "  %-32s: %f\n", "btagCSVV2", ( double)btagCSVV2); os << r;
      sprintf(r, "  %-32s: %f\n", "btagDeepB", ( double)btagDeepB); os << r;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "hadronFlavour", ( double)hadronFlavour); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "n2b1", ( double)n2b1); os << r;
      sprintf(r, "  %-32s: %f\n", "n3b1", ( double)n3b1); os << r;
      sprintf(r, "  %-32s: %f\n", "nBHadrons", ( double)nBHadrons); os << r;
      sprintf(r, "  %-32s: %f\n", "nCHadrons", ( double)nCHadrons); os << r;
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

  struct Tau_s
  {
    int	charge;
    float	chargedIso;
    int	cleanmask;
    int	decayMode;
    float	dxy;
    float	dz;
    float	eta;
    int	genPartFlav;
    int	genPartIdx;
    bool	idAntiEleDeadECal;
    int	idAntiMu;
    bool	idDecayModeOldDMs;
    int	idDeepTau2017v2p1VSe;
    int	idDeepTau2017v2p1VSjet;
    int	idDeepTau2017v2p1VSmu;
    int	jetIdx;
    float	leadTkDeltaEta;
    float	leadTkDeltaPhi;
    float	leadTkPtOverTauPt;
    float	mass;
    float	neutralIso;
    float	phi;
    float	photonsOutsideSignalCone;
    float	pt;
    float	puCorr;
    float	rawDeepTau2017v2p1VSe;
    float	rawDeepTau2017v2p1VSjet;
    float	rawDeepTau2017v2p1VSmu;
    float	rawIso;
    float	rawIsodR03;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "Tau" << std::endl;
      sprintf(r, "  %-32s: %f\n", "charge", ( double)charge); os << r;
      sprintf(r, "  %-32s: %f\n", "chargedIso", ( double)chargedIso); os << r;
      sprintf(r, "  %-32s: %f\n", "cleanmask", ( double)cleanmask); os << r;
      sprintf(r, "  %-32s: %f\n", "decayMode", ( double)decayMode); os << r;
      sprintf(r, "  %-32s: %f\n", "dxy", ( double)dxy); os << r;
      sprintf(r, "  %-32s: %f\n", "dz", ( double)dz); os << r;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "genPartFlav", ( double)genPartFlav); os << r;
      sprintf(r, "  %-32s: %f\n", "genPartIdx", ( double)genPartIdx); os << r;
      sprintf(r, "  %-32s: %f\n", "idAntiEleDeadECal", ( double)idAntiEleDeadECal); os << r;
      sprintf(r, "  %-32s: %f\n", "idAntiMu", ( double)idAntiMu); os << r;
      sprintf(r, "  %-32s: %f\n", "idDecayModeOldDMs", ( double)idDecayModeOldDMs); os << r;
      sprintf(r, "  %-32s: %f\n", "idDeepTau2017v2p1VSe", ( double)idDeepTau2017v2p1VSe); os << r;
      sprintf(r, "  %-32s: %f\n", "idDeepTau2017v2p1VSjet", ( double)idDeepTau2017v2p1VSjet); os << r;
      sprintf(r, "  %-32s: %f\n", "idDeepTau2017v2p1VSmu", ( double)idDeepTau2017v2p1VSmu); os << r;
      sprintf(r, "  %-32s: %f\n", "jetIdx", ( double)jetIdx); os << r;
      sprintf(r, "  %-32s: %f\n", "leadTkDeltaEta", ( double)leadTkDeltaEta); os << r;
      sprintf(r, "  %-32s: %f\n", "leadTkDeltaPhi", ( double)leadTkDeltaPhi); os << r;
      sprintf(r, "  %-32s: %f\n", "leadTkPtOverTauPt", ( double)leadTkPtOverTauPt); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "neutralIso", ( double)neutralIso); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "photonsOutsideSignalCone", ( double)photonsOutsideSignalCone); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      sprintf(r, "  %-32s: %f\n", "puCorr", ( double)puCorr); os << r;
      sprintf(r, "  %-32s: %f\n", "rawDeepTau2017v2p1VSe", ( double)rawDeepTau2017v2p1VSe); os << r;
      sprintf(r, "  %-32s: %f\n", "rawDeepTau2017v2p1VSjet", ( double)rawDeepTau2017v2p1VSjet); os << r;
      sprintf(r, "  %-32s: %f\n", "rawDeepTau2017v2p1VSmu", ( double)rawDeepTau2017v2p1VSmu); os << r;
      sprintf(r, "  %-32s: %f\n", "rawIso", ( double)rawIso); os << r;
      sprintf(r, "  %-32s: %f\n", "rawIsodR03", ( double)rawIsodR03); os << r;
      return os;
    }
  };

  struct TrigObj_s
  {
    float	eta;
    int	filterBits;
    int	id;
    int	l1charge;
    int	l1iso;
    float	l1pt;
    float	l1pt_2;
    float	l2pt;
    float	phi;
    float	pt;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "TrigObj" << std::endl;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "filterBits", ( double)filterBits); os << r;
      sprintf(r, "  %-32s: %f\n", "id", ( double)id); os << r;
      sprintf(r, "  %-32s: %f\n", "l1charge", ( double)l1charge); os << r;
      sprintf(r, "  %-32s: %f\n", "l1iso", ( double)l1iso); os << r;
      sprintf(r, "  %-32s: %f\n", "l1pt", ( double)l1pt); os << r;
      sprintf(r, "  %-32s: %f\n", "l1pt_2", ( double)l1pt_2); os << r;
      sprintf(r, "  %-32s: %f\n", "l2pt", ( double)l2pt); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      return os;
    }
  };

  struct boostedTau_s
  {
    int	charge;
    float	chargedIso;
    int	decayMode;
    float	eta;
    int	genPartFlav;
    int	genPartIdx;
    int	idAntiEle2018;
    int	idAntiMu;
    int	idMVAnewDM2017v2;
    int	idMVAoldDM2017v2;
    int	idMVAoldDMdR032017v2;
    int	jetIdx;
    float	leadTkDeltaEta;
    float	leadTkDeltaPhi;
    float	leadTkPtOverTauPt;
    float	mass;
    float	neutralIso;
    float	phi;
    float	photonsOutsideSignalCone;
    float	pt;
    float	puCorr;
    float	rawAntiEle2018;
    int	rawAntiEleCat2018;
    float	rawIso;
    float	rawIsodR03;
    float	rawMVAnewDM2017v2;
    float	rawMVAoldDM2017v2;
    float	rawMVAoldDMdR032017v2;

    std::ostream& operator<<(std::ostream& os)
    {
      char r[1024];
      os << "boostedTau" << std::endl;
      sprintf(r, "  %-32s: %f\n", "charge", ( double)charge); os << r;
      sprintf(r, "  %-32s: %f\n", "chargedIso", ( double)chargedIso); os << r;
      sprintf(r, "  %-32s: %f\n", "decayMode", ( double)decayMode); os << r;
      sprintf(r, "  %-32s: %f\n", "eta", ( double)eta); os << r;
      sprintf(r, "  %-32s: %f\n", "genPartFlav", ( double)genPartFlav); os << r;
      sprintf(r, "  %-32s: %f\n", "genPartIdx", ( double)genPartIdx); os << r;
      sprintf(r, "  %-32s: %f\n", "idAntiEle2018", ( double)idAntiEle2018); os << r;
      sprintf(r, "  %-32s: %f\n", "idAntiMu", ( double)idAntiMu); os << r;
      sprintf(r, "  %-32s: %f\n", "idMVAnewDM2017v2", ( double)idMVAnewDM2017v2); os << r;
      sprintf(r, "  %-32s: %f\n", "idMVAoldDM2017v2", ( double)idMVAoldDM2017v2); os << r;
      sprintf(r, "  %-32s: %f\n", "idMVAoldDMdR032017v2", ( double)idMVAoldDMdR032017v2); os << r;
      sprintf(r, "  %-32s: %f\n", "jetIdx", ( double)jetIdx); os << r;
      sprintf(r, "  %-32s: %f\n", "leadTkDeltaEta", ( double)leadTkDeltaEta); os << r;
      sprintf(r, "  %-32s: %f\n", "leadTkDeltaPhi", ( double)leadTkDeltaPhi); os << r;
      sprintf(r, "  %-32s: %f\n", "leadTkPtOverTauPt", ( double)leadTkPtOverTauPt); os << r;
      sprintf(r, "  %-32s: %f\n", "mass", ( double)mass); os << r;
      sprintf(r, "  %-32s: %f\n", "neutralIso", ( double)neutralIso); os << r;
      sprintf(r, "  %-32s: %f\n", "phi", ( double)phi); os << r;
      sprintf(r, "  %-32s: %f\n", "photonsOutsideSignalCone", ( double)photonsOutsideSignalCone); os << r;
      sprintf(r, "  %-32s: %f\n", "pt", ( double)pt); os << r;
      sprintf(r, "  %-32s: %f\n", "puCorr", ( double)puCorr); os << r;
      sprintf(r, "  %-32s: %f\n", "rawAntiEle2018", ( double)rawAntiEle2018); os << r;
      sprintf(r, "  %-32s: %f\n", "rawAntiEleCat2018", ( double)rawAntiEleCat2018); os << r;
      sprintf(r, "  %-32s: %f\n", "rawIso", ( double)rawIso); os << r;
      sprintf(r, "  %-32s: %f\n", "rawIsodR03", ( double)rawIsodR03); os << r;
      sprintf(r, "  %-32s: %f\n", "rawMVAnewDM2017v2", ( double)rawMVAnewDM2017v2); os << r;
      sprintf(r, "  %-32s: %f\n", "rawMVAoldDM2017v2", ( double)rawMVAoldDM2017v2); os << r;
      sprintf(r, "  %-32s: %f\n", "rawMVAoldDMdR032017v2", ( double)rawMVAoldDMdR032017v2); os << r;
      return os;
    }
  };


  void fillCorrT1METJets()
  {
    size_t nobj_ = 0;
    if ( CorrT1METJet_area.size() > nobj_ ) nobj_ = CorrT1METJet_area.size();
    if ( CorrT1METJet_eta.size() > nobj_ ) nobj_ = CorrT1METJet_eta.size();
    if ( CorrT1METJet_muonSubtrFactor.size() > nobj_ ) nobj_ = CorrT1METJet_muonSubtrFactor.size();
    if ( CorrT1METJet_phi.size() > nobj_ ) nobj_ = CorrT1METJet_phi.size();
    if ( CorrT1METJet_rawPt.size() > nobj_ ) nobj_ = CorrT1METJet_rawPt.size();
    CorrT1METJet.resize(nobj_);
    for(unsigned int i=0; i < CorrT1METJet.size(); ++i)
      {
        CorrT1METJet[i].area	= (CorrT1METJet_area.size() > i) ? CorrT1METJet_area[i] : 0;
        CorrT1METJet[i].eta	= (CorrT1METJet_eta.size() > i) ? CorrT1METJet_eta[i] : 0;
        CorrT1METJet[i].muonSubtrFactor	= (CorrT1METJet_muonSubtrFactor.size() > i) ? CorrT1METJet_muonSubtrFactor[i] : 0;
        CorrT1METJet[i].phi	= (CorrT1METJet_phi.size() > i) ? CorrT1METJet_phi[i] : 0;
        CorrT1METJet[i].rawPt	= (CorrT1METJet_rawPt.size() > i) ? CorrT1METJet_rawPt[i] : 0;
      }
  }

  void fillElectrons()
  {
    size_t nobj_ = 0;
    if ( Electron_charge.size() > nobj_ ) nobj_ = Electron_charge.size();
    if ( Electron_convVeto.size() > nobj_ ) nobj_ = Electron_convVeto.size();
    if ( Electron_cutBased.size() > nobj_ ) nobj_ = Electron_cutBased.size();
    if ( Electron_deltaEtaSC.size() > nobj_ ) nobj_ = Electron_deltaEtaSC.size();
    if ( Electron_dxy.size() > nobj_ ) nobj_ = Electron_dxy.size();
    if ( Electron_dz.size() > nobj_ ) nobj_ = Electron_dz.size();
    if ( Electron_eta.size() > nobj_ ) nobj_ = Electron_eta.size();
    if ( Electron_ip3d.size() > nobj_ ) nobj_ = Electron_ip3d.size();
    if ( Electron_isEB.size() > nobj_ ) nobj_ = Electron_isEB.size();
    if ( Electron_jetIdx.size() > nobj_ ) nobj_ = Electron_jetIdx.size();
    if ( Electron_lostHits.size() > nobj_ ) nobj_ = Electron_lostHits.size();
    if ( Electron_mass.size() > nobj_ ) nobj_ = Electron_mass.size();
    if ( Electron_miniPFRelIso_all.size() > nobj_ ) nobj_ = Electron_miniPFRelIso_all.size();
    if ( Electron_miniPFRelIso_chg.size() > nobj_ ) nobj_ = Electron_miniPFRelIso_chg.size();
    if ( Electron_mvaIso.size() > nobj_ ) nobj_ = Electron_mvaIso.size();
    if ( Electron_mvaIso_WP80.size() > nobj_ ) nobj_ = Electron_mvaIso_WP80.size();
    if ( Electron_mvaIso_WP90.size() > nobj_ ) nobj_ = Electron_mvaIso_WP90.size();
    if ( Electron_mvaNoIso.size() > nobj_ ) nobj_ = Electron_mvaNoIso.size();
    if ( Electron_mvaNoIso_WP80.size() > nobj_ ) nobj_ = Electron_mvaNoIso_WP80.size();
    if ( Electron_mvaNoIso_WP90.size() > nobj_ ) nobj_ = Electron_mvaNoIso_WP90.size();
    if ( Electron_pdgId.size() > nobj_ ) nobj_ = Electron_pdgId.size();
    if ( Electron_pfRelIso03_all.size() > nobj_ ) nobj_ = Electron_pfRelIso03_all.size();
    if ( Electron_pfRelIso03_chg.size() > nobj_ ) nobj_ = Electron_pfRelIso03_chg.size();
    if ( Electron_pfRelIso04_all.size() > nobj_ ) nobj_ = Electron_pfRelIso04_all.size();
    if ( Electron_phi.size() > nobj_ ) nobj_ = Electron_phi.size();
    if ( Electron_promptMVA.size() > nobj_ ) nobj_ = Electron_promptMVA.size();
    if ( Electron_pt.size() > nobj_ ) nobj_ = Electron_pt.size();
    if ( Electron_r9.size() > nobj_ ) nobj_ = Electron_r9.size();
    if ( Electron_scEtOverPt.size() > nobj_ ) nobj_ = Electron_scEtOverPt.size();
    if ( Electron_seedGain.size() > nobj_ ) nobj_ = Electron_seedGain.size();
    if ( Electron_sip3d.size() > nobj_ ) nobj_ = Electron_sip3d.size();
    if ( Electron_superclusterEta.size() > nobj_ ) nobj_ = Electron_superclusterEta.size();
    if ( Electron_tightCharge.size() > nobj_ ) nobj_ = Electron_tightCharge.size();
    if ( Electron_vidNestedWPBitmap.size() > nobj_ ) nobj_ = Electron_vidNestedWPBitmap.size();
    if ( Electron_genPartFlav.size() > nobj_ ) nobj_ = Electron_genPartFlav.size();
    if ( Electron_genPartIdx.size() > nobj_ ) nobj_ = Electron_genPartIdx.size();
    if ( Electron_mvaIso_WPL.size() > nobj_ ) nobj_ = Electron_mvaIso_WPL.size();
    if ( Electron_mvaNoIso_WPL.size() > nobj_ ) nobj_ = Electron_mvaNoIso_WPL.size();
    if ( Electron_cleanmask.size() > nobj_ ) nobj_ = Electron_cleanmask.size();
    if ( Electron_cutBased_HEEP.size() > nobj_ ) nobj_ = Electron_cutBased_HEEP.size();
    if ( Electron_dEscaleDown.size() > nobj_ ) nobj_ = Electron_dEscaleDown.size();
    if ( Electron_dEscaleUp.size() > nobj_ ) nobj_ = Electron_dEscaleUp.size();
    if ( Electron_dEsigmaDown.size() > nobj_ ) nobj_ = Electron_dEsigmaDown.size();
    if ( Electron_dEsigmaUp.size() > nobj_ ) nobj_ = Electron_dEsigmaUp.size();
    if ( Electron_dr03EcalRecHitSumEt.size() > nobj_ ) nobj_ = Electron_dr03EcalRecHitSumEt.size();
    if ( Electron_dr03HcalDepth1TowerSumEt.size() > nobj_ ) nobj_ = Electron_dr03HcalDepth1TowerSumEt.size();
    if ( Electron_dr03TkSumPt.size() > nobj_ ) nobj_ = Electron_dr03TkSumPt.size();
    if ( Electron_dr03TkSumPtHEEP.size() > nobj_ ) nobj_ = Electron_dr03TkSumPtHEEP.size();
    if ( Electron_dxyErr.size() > nobj_ ) nobj_ = Electron_dxyErr.size();
    if ( Electron_dzErr.size() > nobj_ ) nobj_ = Electron_dzErr.size();
    if ( Electron_eCorr.size() > nobj_ ) nobj_ = Electron_eCorr.size();
    if ( Electron_eInvMinusPInv.size() > nobj_ ) nobj_ = Electron_eInvMinusPInv.size();
    if ( Electron_energyErr.size() > nobj_ ) nobj_ = Electron_energyErr.size();
    if ( Electron_hoe.size() > nobj_ ) nobj_ = Electron_hoe.size();
    if ( Electron_isPFcand.size() > nobj_ ) nobj_ = Electron_isPFcand.size();
    if ( Electron_jetNDauCharged.size() > nobj_ ) nobj_ = Electron_jetNDauCharged.size();
    if ( Electron_jetPtRelv2.size() > nobj_ ) nobj_ = Electron_jetPtRelv2.size();
    if ( Electron_jetRelIso.size() > nobj_ ) nobj_ = Electron_jetRelIso.size();
    if ( Electron_mvaFall17V2Iso.size() > nobj_ ) nobj_ = Electron_mvaFall17V2Iso.size();
    if ( Electron_mvaFall17V2Iso_WP80.size() > nobj_ ) nobj_ = Electron_mvaFall17V2Iso_WP80.size();
    if ( Electron_mvaFall17V2Iso_WP90.size() > nobj_ ) nobj_ = Electron_mvaFall17V2Iso_WP90.size();
    if ( Electron_mvaFall17V2Iso_WPL.size() > nobj_ ) nobj_ = Electron_mvaFall17V2Iso_WPL.size();
    if ( Electron_mvaFall17V2noIso.size() > nobj_ ) nobj_ = Electron_mvaFall17V2noIso.size();
    if ( Electron_mvaFall17V2noIso_WP80.size() > nobj_ ) nobj_ = Electron_mvaFall17V2noIso_WP80.size();
    if ( Electron_mvaFall17V2noIso_WP90.size() > nobj_ ) nobj_ = Electron_mvaFall17V2noIso_WP90.size();
    if ( Electron_mvaFall17V2noIso_WPL.size() > nobj_ ) nobj_ = Electron_mvaFall17V2noIso_WPL.size();
    if ( Electron_mvaTTH.size() > nobj_ ) nobj_ = Electron_mvaTTH.size();
    if ( Electron_photonIdx.size() > nobj_ ) nobj_ = Electron_photonIdx.size();
    if ( Electron_sieie.size() > nobj_ ) nobj_ = Electron_sieie.size();
    if ( Electron_vidNestedWPBitmapHEEP.size() > nobj_ ) nobj_ = Electron_vidNestedWPBitmapHEEP.size();
    Electron.resize(nobj_);
    for(unsigned int i=0; i < Electron.size(); ++i)
      {
        Electron[i].charge	= (Electron_charge.size() > i) ? Electron_charge[i] : 0;
        Electron[i].convVeto	= (Electron_convVeto.size() > i) ? (bool)Electron_convVeto[i] : 0;
        Electron[i].cutBased	= (Electron_cutBased.size() > i) ? Electron_cutBased[i] : 0;
        Electron[i].deltaEtaSC	= (Electron_deltaEtaSC.size() > i) ? Electron_deltaEtaSC[i] : 0;
        Electron[i].dxy	= (Electron_dxy.size() > i) ? Electron_dxy[i] : 0;
        Electron[i].dz	= (Electron_dz.size() > i) ? Electron_dz[i] : 0;
        Electron[i].eta	= (Electron_eta.size() > i) ? Electron_eta[i] : 0;
        Electron[i].ip3d	= (Electron_ip3d.size() > i) ? Electron_ip3d[i] : 0;
        Electron[i].isEB	= (Electron_isEB.size() > i) ? (bool)Electron_isEB[i] : 0;
        Electron[i].jetIdx	= (Electron_jetIdx.size() > i) ? Electron_jetIdx[i] : 0;
        Electron[i].lostHits	= (Electron_lostHits.size() > i) ? Electron_lostHits[i] : 0;
        Electron[i].mass	= (Electron_mass.size() > i) ? Electron_mass[i] : 0;
        Electron[i].miniPFRelIso_all	= (Electron_miniPFRelIso_all.size() > i) ? Electron_miniPFRelIso_all[i] : 0;
        Electron[i].miniPFRelIso_chg	= (Electron_miniPFRelIso_chg.size() > i) ? Electron_miniPFRelIso_chg[i] : 0;
        Electron[i].mvaIso	= (Electron_mvaIso.size() > i) ? Electron_mvaIso[i] : 0;
        Electron[i].mvaIso_WP80	= (Electron_mvaIso_WP80.size() > i) ? (bool)Electron_mvaIso_WP80[i] : 0;
        Electron[i].mvaIso_WP90	= (Electron_mvaIso_WP90.size() > i) ? (bool)Electron_mvaIso_WP90[i] : 0;
        Electron[i].mvaNoIso	= (Electron_mvaNoIso.size() > i) ? Electron_mvaNoIso[i] : 0;
        Electron[i].mvaNoIso_WP80	= (Electron_mvaNoIso_WP80.size() > i) ? (bool)Electron_mvaNoIso_WP80[i] : 0;
        Electron[i].mvaNoIso_WP90	= (Electron_mvaNoIso_WP90.size() > i) ? (bool)Electron_mvaNoIso_WP90[i] : 0;
        Electron[i].pdgId	= (Electron_pdgId.size() > i) ? Electron_pdgId[i] : 0;
        Electron[i].pfRelIso03_all	= (Electron_pfRelIso03_all.size() > i) ? Electron_pfRelIso03_all[i] : 0;
        Electron[i].pfRelIso03_chg	= (Electron_pfRelIso03_chg.size() > i) ? Electron_pfRelIso03_chg[i] : 0;
        Electron[i].pfRelIso04_all	= (Electron_pfRelIso04_all.size() > i) ? Electron_pfRelIso04_all[i] : 0;
        Electron[i].phi	= (Electron_phi.size() > i) ? Electron_phi[i] : 0;
        Electron[i].promptMVA	= (Electron_promptMVA.size() > i) ? Electron_promptMVA[i] : 0;
        Electron[i].pt	= (Electron_pt.size() > i) ? Electron_pt[i] : 0;
        Electron[i].r9	= (Electron_r9.size() > i) ? Electron_r9[i] : 0;
        Electron[i].scEtOverPt	= (Electron_scEtOverPt.size() > i) ? Electron_scEtOverPt[i] : 0;
        Electron[i].seedGain	= (Electron_seedGain.size() > i) ? Electron_seedGain[i] : 0;
        Electron[i].sip3d	= (Electron_sip3d.size() > i) ? Electron_sip3d[i] : 0;
        Electron[i].superclusterEta	= (Electron_superclusterEta.size() > i) ? Electron_superclusterEta[i] : 0;
        Electron[i].tightCharge	= (Electron_tightCharge.size() > i) ? Electron_tightCharge[i] : 0;
        Electron[i].vidNestedWPBitmap	= (Electron_vidNestedWPBitmap.size() > i) ? Electron_vidNestedWPBitmap[i] : 0;
        Electron[i].genPartFlav	= (Electron_genPartFlav.size() > i) ? Electron_genPartFlav[i] : 0;
        Electron[i].genPartIdx	= (Electron_genPartIdx.size() > i) ? Electron_genPartIdx[i] : 0;
        Electron[i].mvaIso_WPL	= (Electron_mvaIso_WPL.size() > i) ? (bool)Electron_mvaIso_WPL[i] : 0;
        Electron[i].mvaNoIso_WPL	= (Electron_mvaNoIso_WPL.size() > i) ? (bool)Electron_mvaNoIso_WPL[i] : 0;
        Electron[i].cleanmask	= (Electron_cleanmask.size() > i) ? Electron_cleanmask[i] : 0;
        Electron[i].cutBased_HEEP	= (Electron_cutBased_HEEP.size() > i) ? (bool)Electron_cutBased_HEEP[i] : 0;
        Electron[i].dEscaleDown	= (Electron_dEscaleDown.size() > i) ? Electron_dEscaleDown[i] : 0;
        Electron[i].dEscaleUp	= (Electron_dEscaleUp.size() > i) ? Electron_dEscaleUp[i] : 0;
        Electron[i].dEsigmaDown	= (Electron_dEsigmaDown.size() > i) ? Electron_dEsigmaDown[i] : 0;
        Electron[i].dEsigmaUp	= (Electron_dEsigmaUp.size() > i) ? Electron_dEsigmaUp[i] : 0;
        Electron[i].dr03EcalRecHitSumEt	= (Electron_dr03EcalRecHitSumEt.size() > i) ? Electron_dr03EcalRecHitSumEt[i] : 0;
        Electron[i].dr03HcalDepth1TowerSumEt	= (Electron_dr03HcalDepth1TowerSumEt.size() > i) ? Electron_dr03HcalDepth1TowerSumEt[i] : 0;
        Electron[i].dr03TkSumPt	= (Electron_dr03TkSumPt.size() > i) ? Electron_dr03TkSumPt[i] : 0;
        Electron[i].dr03TkSumPtHEEP	= (Electron_dr03TkSumPtHEEP.size() > i) ? Electron_dr03TkSumPtHEEP[i] : 0;
        Electron[i].dxyErr	= (Electron_dxyErr.size() > i) ? Electron_dxyErr[i] : 0;
        Electron[i].dzErr	= (Electron_dzErr.size() > i) ? Electron_dzErr[i] : 0;
        Electron[i].eCorr	= (Electron_eCorr.size() > i) ? Electron_eCorr[i] : 0;
        Electron[i].eInvMinusPInv	= (Electron_eInvMinusPInv.size() > i) ? Electron_eInvMinusPInv[i] : 0;
        Electron[i].energyErr	= (Electron_energyErr.size() > i) ? Electron_energyErr[i] : 0;
        Electron[i].hoe	= (Electron_hoe.size() > i) ? Electron_hoe[i] : 0;
        Electron[i].isPFcand	= (Electron_isPFcand.size() > i) ? (bool)Electron_isPFcand[i] : 0;
        Electron[i].jetNDauCharged	= (Electron_jetNDauCharged.size() > i) ? Electron_jetNDauCharged[i] : 0;
        Electron[i].jetPtRelv2	= (Electron_jetPtRelv2.size() > i) ? Electron_jetPtRelv2[i] : 0;
        Electron[i].jetRelIso	= (Electron_jetRelIso.size() > i) ? Electron_jetRelIso[i] : 0;
        Electron[i].mvaFall17V2Iso	= (Electron_mvaFall17V2Iso.size() > i) ? Electron_mvaFall17V2Iso[i] : 0;
        Electron[i].mvaFall17V2Iso_WP80	= (Electron_mvaFall17V2Iso_WP80.size() > i) ? (bool)Electron_mvaFall17V2Iso_WP80[i] : 0;
        Electron[i].mvaFall17V2Iso_WP90	= (Electron_mvaFall17V2Iso_WP90.size() > i) ? (bool)Electron_mvaFall17V2Iso_WP90[i] : 0;
        Electron[i].mvaFall17V2Iso_WPL	= (Electron_mvaFall17V2Iso_WPL.size() > i) ? (bool)Electron_mvaFall17V2Iso_WPL[i] : 0;
        Electron[i].mvaFall17V2noIso	= (Electron_mvaFall17V2noIso.size() > i) ? Electron_mvaFall17V2noIso[i] : 0;
        Electron[i].mvaFall17V2noIso_WP80	= (Electron_mvaFall17V2noIso_WP80.size() > i) ? (bool)Electron_mvaFall17V2noIso_WP80[i] : 0;
        Electron[i].mvaFall17V2noIso_WP90	= (Electron_mvaFall17V2noIso_WP90.size() > i) ? (bool)Electron_mvaFall17V2noIso_WP90[i] : 0;
        Electron[i].mvaFall17V2noIso_WPL	= (Electron_mvaFall17V2noIso_WPL.size() > i) ? (bool)Electron_mvaFall17V2noIso_WPL[i] : 0;
        Electron[i].mvaTTH	= (Electron_mvaTTH.size() > i) ? Electron_mvaTTH[i] : 0;
        Electron[i].photonIdx	= (Electron_photonIdx.size() > i) ? Electron_photonIdx[i] : 0;
        Electron[i].sieie	= (Electron_sieie.size() > i) ? Electron_sieie[i] : 0;
        Electron[i].vidNestedWPBitmapHEEP	= (Electron_vidNestedWPBitmapHEEP.size() > i) ? Electron_vidNestedWPBitmapHEEP[i] : 0;
      }
  }

  void fillFatJets()
  {
    size_t nobj_ = 0;
    if ( FatJet_area.size() > nobj_ ) nobj_ = FatJet_area.size();
    if ( FatJet_btagCSVV2.size() > nobj_ ) nobj_ = FatJet_btagCSVV2.size();
    if ( FatJet_btagDDBvLV2.size() > nobj_ ) nobj_ = FatJet_btagDDBvLV2.size();
    if ( FatJet_btagDDCvBV2.size() > nobj_ ) nobj_ = FatJet_btagDDCvBV2.size();
    if ( FatJet_btagDDCvLV2.size() > nobj_ ) nobj_ = FatJet_btagDDCvLV2.size();
    if ( FatJet_btagDeepB.size() > nobj_ ) nobj_ = FatJet_btagDeepB.size();
    if ( FatJet_btagHbb.size() > nobj_ ) nobj_ = FatJet_btagHbb.size();
    if ( FatJet_deepTagMD_H4qvsQCD.size() > nobj_ ) nobj_ = FatJet_deepTagMD_H4qvsQCD.size();
    if ( FatJet_deepTagMD_HbbvsQCD.size() > nobj_ ) nobj_ = FatJet_deepTagMD_HbbvsQCD.size();
    if ( FatJet_deepTagMD_TvsQCD.size() > nobj_ ) nobj_ = FatJet_deepTagMD_TvsQCD.size();
    if ( FatJet_deepTagMD_WvsQCD.size() > nobj_ ) nobj_ = FatJet_deepTagMD_WvsQCD.size();
    if ( FatJet_deepTagMD_ZHbbvsQCD.size() > nobj_ ) nobj_ = FatJet_deepTagMD_ZHbbvsQCD.size();
    if ( FatJet_deepTagMD_ZHccvsQCD.size() > nobj_ ) nobj_ = FatJet_deepTagMD_ZHccvsQCD.size();
    if ( FatJet_deepTagMD_ZbbvsQCD.size() > nobj_ ) nobj_ = FatJet_deepTagMD_ZbbvsQCD.size();
    if ( FatJet_deepTagMD_ZvsQCD.size() > nobj_ ) nobj_ = FatJet_deepTagMD_ZvsQCD.size();
    if ( FatJet_deepTagMD_bbvsLight.size() > nobj_ ) nobj_ = FatJet_deepTagMD_bbvsLight.size();
    if ( FatJet_deepTagMD_ccvsLight.size() > nobj_ ) nobj_ = FatJet_deepTagMD_ccvsLight.size();
    if ( FatJet_deepTag_H.size() > nobj_ ) nobj_ = FatJet_deepTag_H.size();
    if ( FatJet_deepTag_QCD.size() > nobj_ ) nobj_ = FatJet_deepTag_QCD.size();
    if ( FatJet_deepTag_QCDothers.size() > nobj_ ) nobj_ = FatJet_deepTag_QCDothers.size();
    if ( FatJet_deepTag_TvsQCD.size() > nobj_ ) nobj_ = FatJet_deepTag_TvsQCD.size();
    if ( FatJet_deepTag_WvsQCD.size() > nobj_ ) nobj_ = FatJet_deepTag_WvsQCD.size();
    if ( FatJet_deepTag_ZvsQCD.size() > nobj_ ) nobj_ = FatJet_deepTag_ZvsQCD.size();
    if ( FatJet_electronIdx3SJ.size() > nobj_ ) nobj_ = FatJet_electronIdx3SJ.size();
    if ( FatJet_eta.size() > nobj_ ) nobj_ = FatJet_eta.size();
    if ( FatJet_genJetAK8Idx.size() > nobj_ ) nobj_ = FatJet_genJetAK8Idx.size();
    if ( FatJet_hadronFlavour.size() > nobj_ ) nobj_ = FatJet_hadronFlavour.size();
    if ( FatJet_jetId.size() > nobj_ ) nobj_ = FatJet_jetId.size();
    if ( FatJet_lsf3.size() > nobj_ ) nobj_ = FatJet_lsf3.size();
    if ( FatJet_mass.size() > nobj_ ) nobj_ = FatJet_mass.size();
    if ( FatJet_msoftdrop.size() > nobj_ ) nobj_ = FatJet_msoftdrop.size();
    if ( FatJet_muonIdx3SJ.size() > nobj_ ) nobj_ = FatJet_muonIdx3SJ.size();
    if ( FatJet_n2b1.size() > nobj_ ) nobj_ = FatJet_n2b1.size();
    if ( FatJet_n3b1.size() > nobj_ ) nobj_ = FatJet_n3b1.size();
    if ( FatJet_nBHadrons.size() > nobj_ ) nobj_ = FatJet_nBHadrons.size();
    if ( FatJet_nCHadrons.size() > nobj_ ) nobj_ = FatJet_nCHadrons.size();
    if ( FatJet_nConstituents.size() > nobj_ ) nobj_ = FatJet_nConstituents.size();
    if ( FatJet_particleNetMD_QCD.size() > nobj_ ) nobj_ = FatJet_particleNetMD_QCD.size();
    if ( FatJet_particleNetMD_Xbb.size() > nobj_ ) nobj_ = FatJet_particleNetMD_Xbb.size();
    if ( FatJet_particleNetMD_Xcc.size() > nobj_ ) nobj_ = FatJet_particleNetMD_Xcc.size();
    if ( FatJet_particleNetMD_Xqq.size() > nobj_ ) nobj_ = FatJet_particleNetMD_Xqq.size();
    if ( FatJet_particleNet_H4qvsQCD.size() > nobj_ ) nobj_ = FatJet_particleNet_H4qvsQCD.size();
    if ( FatJet_particleNet_HbbvsQCD.size() > nobj_ ) nobj_ = FatJet_particleNet_HbbvsQCD.size();
    if ( FatJet_particleNet_HccvsQCD.size() > nobj_ ) nobj_ = FatJet_particleNet_HccvsQCD.size();
    if ( FatJet_particleNet_QCD.size() > nobj_ ) nobj_ = FatJet_particleNet_QCD.size();
    if ( FatJet_particleNet_TvsQCD.size() > nobj_ ) nobj_ = FatJet_particleNet_TvsQCD.size();
    if ( FatJet_particleNet_WvsQCD.size() > nobj_ ) nobj_ = FatJet_particleNet_WvsQCD.size();
    if ( FatJet_particleNet_ZvsQCD.size() > nobj_ ) nobj_ = FatJet_particleNet_ZvsQCD.size();
    if ( FatJet_particleNet_mass.size() > nobj_ ) nobj_ = FatJet_particleNet_mass.size();
    if ( FatJet_phi.size() > nobj_ ) nobj_ = FatJet_phi.size();
    if ( FatJet_pt.size() > nobj_ ) nobj_ = FatJet_pt.size();
    if ( FatJet_rawFactor.size() > nobj_ ) nobj_ = FatJet_rawFactor.size();
    if ( FatJet_subJetIdx1.size() > nobj_ ) nobj_ = FatJet_subJetIdx1.size();
    if ( FatJet_subJetIdx2.size() > nobj_ ) nobj_ = FatJet_subJetIdx2.size();
    if ( FatJet_tau1.size() > nobj_ ) nobj_ = FatJet_tau1.size();
    if ( FatJet_tau2.size() > nobj_ ) nobj_ = FatJet_tau2.size();
    if ( FatJet_tau3.size() > nobj_ ) nobj_ = FatJet_tau3.size();
    if ( FatJet_tau4.size() > nobj_ ) nobj_ = FatJet_tau4.size();
    FatJet.resize(nobj_);
    for(unsigned int i=0; i < FatJet.size(); ++i)
      {
        FatJet[i].area	= (FatJet_area.size() > i) ? FatJet_area[i] : 0;
        FatJet[i].btagCSVV2	= (FatJet_btagCSVV2.size() > i) ? FatJet_btagCSVV2[i] : 0;
        FatJet[i].btagDDBvLV2	= (FatJet_btagDDBvLV2.size() > i) ? FatJet_btagDDBvLV2[i] : 0;
        FatJet[i].btagDDCvBV2	= (FatJet_btagDDCvBV2.size() > i) ? FatJet_btagDDCvBV2[i] : 0;
        FatJet[i].btagDDCvLV2	= (FatJet_btagDDCvLV2.size() > i) ? FatJet_btagDDCvLV2[i] : 0;
        FatJet[i].btagDeepB	= (FatJet_btagDeepB.size() > i) ? FatJet_btagDeepB[i] : 0;
        FatJet[i].btagHbb	= (FatJet_btagHbb.size() > i) ? FatJet_btagHbb[i] : 0;
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
        FatJet[i].electronIdx3SJ	= (FatJet_electronIdx3SJ.size() > i) ? FatJet_electronIdx3SJ[i] : 0;
        FatJet[i].eta	= (FatJet_eta.size() > i) ? FatJet_eta[i] : 0;
        FatJet[i].genJetAK8Idx	= (FatJet_genJetAK8Idx.size() > i) ? FatJet_genJetAK8Idx[i] : 0;
        FatJet[i].hadronFlavour	= (FatJet_hadronFlavour.size() > i) ? FatJet_hadronFlavour[i] : 0;
        FatJet[i].jetId	= (FatJet_jetId.size() > i) ? FatJet_jetId[i] : 0;
        FatJet[i].lsf3	= (FatJet_lsf3.size() > i) ? FatJet_lsf3[i] : 0;
        FatJet[i].mass	= (FatJet_mass.size() > i) ? FatJet_mass[i] : 0;
        FatJet[i].msoftdrop	= (FatJet_msoftdrop.size() > i) ? FatJet_msoftdrop[i] : 0;
        FatJet[i].muonIdx3SJ	= (FatJet_muonIdx3SJ.size() > i) ? FatJet_muonIdx3SJ[i] : 0;
        FatJet[i].n2b1	= (FatJet_n2b1.size() > i) ? FatJet_n2b1[i] : 0;
        FatJet[i].n3b1	= (FatJet_n3b1.size() > i) ? FatJet_n3b1[i] : 0;
        FatJet[i].nBHadrons	= (FatJet_nBHadrons.size() > i) ? FatJet_nBHadrons[i] : 0;
        FatJet[i].nCHadrons	= (FatJet_nCHadrons.size() > i) ? FatJet_nCHadrons[i] : 0;
        FatJet[i].nConstituents	= (FatJet_nConstituents.size() > i) ? FatJet_nConstituents[i] : 0;
        FatJet[i].particleNetMD_QCD	= (FatJet_particleNetMD_QCD.size() > i) ? FatJet_particleNetMD_QCD[i] : 0;
        FatJet[i].particleNetMD_Xbb	= (FatJet_particleNetMD_Xbb.size() > i) ? FatJet_particleNetMD_Xbb[i] : 0;
        FatJet[i].particleNetMD_Xcc	= (FatJet_particleNetMD_Xcc.size() > i) ? FatJet_particleNetMD_Xcc[i] : 0;
        FatJet[i].particleNetMD_Xqq	= (FatJet_particleNetMD_Xqq.size() > i) ? FatJet_particleNetMD_Xqq[i] : 0;
        FatJet[i].particleNet_H4qvsQCD	= (FatJet_particleNet_H4qvsQCD.size() > i) ? FatJet_particleNet_H4qvsQCD[i] : 0;
        FatJet[i].particleNet_HbbvsQCD	= (FatJet_particleNet_HbbvsQCD.size() > i) ? FatJet_particleNet_HbbvsQCD[i] : 0;
        FatJet[i].particleNet_HccvsQCD	= (FatJet_particleNet_HccvsQCD.size() > i) ? FatJet_particleNet_HccvsQCD[i] : 0;
        FatJet[i].particleNet_QCD	= (FatJet_particleNet_QCD.size() > i) ? FatJet_particleNet_QCD[i] : 0;
        FatJet[i].particleNet_TvsQCD	= (FatJet_particleNet_TvsQCD.size() > i) ? FatJet_particleNet_TvsQCD[i] : 0;
        FatJet[i].particleNet_WvsQCD	= (FatJet_particleNet_WvsQCD.size() > i) ? FatJet_particleNet_WvsQCD[i] : 0;
        FatJet[i].particleNet_ZvsQCD	= (FatJet_particleNet_ZvsQCD.size() > i) ? FatJet_particleNet_ZvsQCD[i] : 0;
        FatJet[i].particleNet_mass	= (FatJet_particleNet_mass.size() > i) ? FatJet_particleNet_mass[i] : 0;
        FatJet[i].phi	= (FatJet_phi.size() > i) ? FatJet_phi[i] : 0;
        FatJet[i].pt	= (FatJet_pt.size() > i) ? FatJet_pt[i] : 0;
        FatJet[i].rawFactor	= (FatJet_rawFactor.size() > i) ? FatJet_rawFactor[i] : 0;
        FatJet[i].subJetIdx1	= (FatJet_subJetIdx1.size() > i) ? FatJet_subJetIdx1[i] : 0;
        FatJet[i].subJetIdx2	= (FatJet_subJetIdx2.size() > i) ? FatJet_subJetIdx2[i] : 0;
        FatJet[i].tau1	= (FatJet_tau1.size() > i) ? FatJet_tau1[i] : 0;
        FatJet[i].tau2	= (FatJet_tau2.size() > i) ? FatJet_tau2[i] : 0;
        FatJet[i].tau3	= (FatJet_tau3.size() > i) ? FatJet_tau3[i] : 0;
        FatJet[i].tau4	= (FatJet_tau4.size() > i) ? FatJet_tau4[i] : 0;
      }
  }

  void fillFsrPhotons()
  {
    size_t nobj_ = 0;
    if ( FsrPhoton_dROverEt2.size() > nobj_ ) nobj_ = FsrPhoton_dROverEt2.size();
    if ( FsrPhoton_eta.size() > nobj_ ) nobj_ = FsrPhoton_eta.size();
    if ( FsrPhoton_muonIdx.size() > nobj_ ) nobj_ = FsrPhoton_muonIdx.size();
    if ( FsrPhoton_phi.size() > nobj_ ) nobj_ = FsrPhoton_phi.size();
    if ( FsrPhoton_pt.size() > nobj_ ) nobj_ = FsrPhoton_pt.size();
    if ( FsrPhoton_relIso03.size() > nobj_ ) nobj_ = FsrPhoton_relIso03.size();
    FsrPhoton.resize(nobj_);
    for(unsigned int i=0; i < FsrPhoton.size(); ++i)
      {
        FsrPhoton[i].dROverEt2	= (FsrPhoton_dROverEt2.size() > i) ? FsrPhoton_dROverEt2[i] : 0;
        FsrPhoton[i].eta	= (FsrPhoton_eta.size() > i) ? FsrPhoton_eta[i] : 0;
        FsrPhoton[i].muonIdx	= (FsrPhoton_muonIdx.size() > i) ? FsrPhoton_muonIdx[i] : 0;
        FsrPhoton[i].phi	= (FsrPhoton_phi.size() > i) ? FsrPhoton_phi[i] : 0;
        FsrPhoton[i].pt	= (FsrPhoton_pt.size() > i) ? FsrPhoton_pt[i] : 0;
        FsrPhoton[i].relIso03	= (FsrPhoton_relIso03.size() > i) ? FsrPhoton_relIso03[i] : 0;
      }
  }

  void fillGenDressedLeptons()
  {
    size_t nobj_ = 0;
    if ( GenDressedLepton_eta.size() > nobj_ ) nobj_ = GenDressedLepton_eta.size();
    if ( GenDressedLepton_hasTauAnc.size() > nobj_ ) nobj_ = GenDressedLepton_hasTauAnc.size();
    if ( GenDressedLepton_mass.size() > nobj_ ) nobj_ = GenDressedLepton_mass.size();
    if ( GenDressedLepton_pdgId.size() > nobj_ ) nobj_ = GenDressedLepton_pdgId.size();
    if ( GenDressedLepton_phi.size() > nobj_ ) nobj_ = GenDressedLepton_phi.size();
    if ( GenDressedLepton_pt.size() > nobj_ ) nobj_ = GenDressedLepton_pt.size();
    GenDressedLepton.resize(nobj_);
    for(unsigned int i=0; i < GenDressedLepton.size(); ++i)
      {
        GenDressedLepton[i].eta	= (GenDressedLepton_eta.size() > i) ? GenDressedLepton_eta[i] : 0;
        GenDressedLepton[i].hasTauAnc	= (GenDressedLepton_hasTauAnc.size() > i) ? (bool)GenDressedLepton_hasTauAnc[i] : 0;
        GenDressedLepton[i].mass	= (GenDressedLepton_mass.size() > i) ? GenDressedLepton_mass[i] : 0;
        GenDressedLepton[i].pdgId	= (GenDressedLepton_pdgId.size() > i) ? GenDressedLepton_pdgId[i] : 0;
        GenDressedLepton[i].phi	= (GenDressedLepton_phi.size() > i) ? GenDressedLepton_phi[i] : 0;
        GenDressedLepton[i].pt	= (GenDressedLepton_pt.size() > i) ? GenDressedLepton_pt[i] : 0;
      }
  }

  void fillGenIsolatedPhotons()
  {
    size_t nobj_ = 0;
    if ( GenIsolatedPhoton_eta.size() > nobj_ ) nobj_ = GenIsolatedPhoton_eta.size();
    if ( GenIsolatedPhoton_mass.size() > nobj_ ) nobj_ = GenIsolatedPhoton_mass.size();
    if ( GenIsolatedPhoton_phi.size() > nobj_ ) nobj_ = GenIsolatedPhoton_phi.size();
    if ( GenIsolatedPhoton_pt.size() > nobj_ ) nobj_ = GenIsolatedPhoton_pt.size();
    GenIsolatedPhoton.resize(nobj_);
    for(unsigned int i=0; i < GenIsolatedPhoton.size(); ++i)
      {
        GenIsolatedPhoton[i].eta	= (GenIsolatedPhoton_eta.size() > i) ? GenIsolatedPhoton_eta[i] : 0;
        GenIsolatedPhoton[i].mass	= (GenIsolatedPhoton_mass.size() > i) ? GenIsolatedPhoton_mass[i] : 0;
        GenIsolatedPhoton[i].phi	= (GenIsolatedPhoton_phi.size() > i) ? GenIsolatedPhoton_phi[i] : 0;
        GenIsolatedPhoton[i].pt	= (GenIsolatedPhoton_pt.size() > i) ? GenIsolatedPhoton_pt[i] : 0;
      }
  }

  void fillGenJets()
  {
    size_t nobj_ = 0;
    if ( GenJet_eta.size() > nobj_ ) nobj_ = GenJet_eta.size();
    if ( GenJet_hadronFlavour.size() > nobj_ ) nobj_ = GenJet_hadronFlavour.size();
    if ( GenJet_mass.size() > nobj_ ) nobj_ = GenJet_mass.size();
    if ( GenJet_nBHadrons.size() > nobj_ ) nobj_ = GenJet_nBHadrons.size();
    if ( GenJet_nCHadrons.size() > nobj_ ) nobj_ = GenJet_nCHadrons.size();
    if ( GenJet_partonFlavour.size() > nobj_ ) nobj_ = GenJet_partonFlavour.size();
    if ( GenJet_phi.size() > nobj_ ) nobj_ = GenJet_phi.size();
    if ( GenJet_pt.size() > nobj_ ) nobj_ = GenJet_pt.size();
    GenJet.resize(nobj_);
    for(unsigned int i=0; i < GenJet.size(); ++i)
      {
        GenJet[i].eta	= (GenJet_eta.size() > i) ? GenJet_eta[i] : 0;
        GenJet[i].hadronFlavour	= (GenJet_hadronFlavour.size() > i) ? GenJet_hadronFlavour[i] : 0;
        GenJet[i].mass	= (GenJet_mass.size() > i) ? GenJet_mass[i] : 0;
        GenJet[i].nBHadrons	= (GenJet_nBHadrons.size() > i) ? GenJet_nBHadrons[i] : 0;
        GenJet[i].nCHadrons	= (GenJet_nCHadrons.size() > i) ? GenJet_nCHadrons[i] : 0;
        GenJet[i].partonFlavour	= (GenJet_partonFlavour.size() > i) ? GenJet_partonFlavour[i] : 0;
        GenJet[i].phi	= (GenJet_phi.size() > i) ? GenJet_phi[i] : 0;
        GenJet[i].pt	= (GenJet_pt.size() > i) ? GenJet_pt[i] : 0;
      }
  }

  void fillGenJetAK8s()
  {
    size_t nobj_ = 0;
    if ( GenJetAK8_eta.size() > nobj_ ) nobj_ = GenJetAK8_eta.size();
    if ( GenJetAK8_hadronFlavour.size() > nobj_ ) nobj_ = GenJetAK8_hadronFlavour.size();
    if ( GenJetAK8_mass.size() > nobj_ ) nobj_ = GenJetAK8_mass.size();
    if ( GenJetAK8_partonFlavour.size() > nobj_ ) nobj_ = GenJetAK8_partonFlavour.size();
    if ( GenJetAK8_phi.size() > nobj_ ) nobj_ = GenJetAK8_phi.size();
    if ( GenJetAK8_pt.size() > nobj_ ) nobj_ = GenJetAK8_pt.size();
    GenJetAK8.resize(nobj_);
    for(unsigned int i=0; i < GenJetAK8.size(); ++i)
      {
        GenJetAK8[i].eta	= (GenJetAK8_eta.size() > i) ? GenJetAK8_eta[i] : 0;
        GenJetAK8[i].hadronFlavour	= (GenJetAK8_hadronFlavour.size() > i) ? GenJetAK8_hadronFlavour[i] : 0;
        GenJetAK8[i].mass	= (GenJetAK8_mass.size() > i) ? GenJetAK8_mass[i] : 0;
        GenJetAK8[i].partonFlavour	= (GenJetAK8_partonFlavour.size() > i) ? GenJetAK8_partonFlavour[i] : 0;
        GenJetAK8[i].phi	= (GenJetAK8_phi.size() > i) ? GenJetAK8_phi[i] : 0;
        GenJetAK8[i].pt	= (GenJetAK8_pt.size() > i) ? GenJetAK8_pt[i] : 0;
      }
  }

  void fillGenParts()
  {
    size_t nobj_ = 0;
    if ( GenPart_eta.size() > nobj_ ) nobj_ = GenPart_eta.size();
    if ( GenPart_genPartIdxMother.size() > nobj_ ) nobj_ = GenPart_genPartIdxMother.size();
    if ( GenPart_mass.size() > nobj_ ) nobj_ = GenPart_mass.size();
    if ( GenPart_pdgId.size() > nobj_ ) nobj_ = GenPart_pdgId.size();
    if ( GenPart_phi.size() > nobj_ ) nobj_ = GenPart_phi.size();
    if ( GenPart_pt.size() > nobj_ ) nobj_ = GenPart_pt.size();
    if ( GenPart_status.size() > nobj_ ) nobj_ = GenPart_status.size();
    if ( GenPart_statusFlags.size() > nobj_ ) nobj_ = GenPart_statusFlags.size();
    GenPart.resize(nobj_);
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

  void fillGenVisTaus()
  {
    size_t nobj_ = 0;
    if ( GenVisTau_charge.size() > nobj_ ) nobj_ = GenVisTau_charge.size();
    if ( GenVisTau_eta.size() > nobj_ ) nobj_ = GenVisTau_eta.size();
    if ( GenVisTau_genPartIdxMother.size() > nobj_ ) nobj_ = GenVisTau_genPartIdxMother.size();
    if ( GenVisTau_mass.size() > nobj_ ) nobj_ = GenVisTau_mass.size();
    if ( GenVisTau_phi.size() > nobj_ ) nobj_ = GenVisTau_phi.size();
    if ( GenVisTau_pt.size() > nobj_ ) nobj_ = GenVisTau_pt.size();
    if ( GenVisTau_status.size() > nobj_ ) nobj_ = GenVisTau_status.size();
    GenVisTau.resize(nobj_);
    for(unsigned int i=0; i < GenVisTau.size(); ++i)
      {
        GenVisTau[i].charge	= (GenVisTau_charge.size() > i) ? GenVisTau_charge[i] : 0;
        GenVisTau[i].eta	= (GenVisTau_eta.size() > i) ? GenVisTau_eta[i] : 0;
        GenVisTau[i].genPartIdxMother	= (GenVisTau_genPartIdxMother.size() > i) ? GenVisTau_genPartIdxMother[i] : 0;
        GenVisTau[i].mass	= (GenVisTau_mass.size() > i) ? GenVisTau_mass[i] : 0;
        GenVisTau[i].phi	= (GenVisTau_phi.size() > i) ? GenVisTau_phi[i] : 0;
        GenVisTau[i].pt	= (GenVisTau_pt.size() > i) ? GenVisTau_pt[i] : 0;
        GenVisTau[i].status	= (GenVisTau_status.size() > i) ? GenVisTau_status[i] : 0;
      }
  }

  void fillIsoTracks()
  {
    size_t nobj_ = 0;
    if ( IsoTrack_charge.size() > nobj_ ) nobj_ = IsoTrack_charge.size();
    if ( IsoTrack_dxy.size() > nobj_ ) nobj_ = IsoTrack_dxy.size();
    if ( IsoTrack_dz.size() > nobj_ ) nobj_ = IsoTrack_dz.size();
    if ( IsoTrack_eta.size() > nobj_ ) nobj_ = IsoTrack_eta.size();
    if ( IsoTrack_fromPV.size() > nobj_ ) nobj_ = IsoTrack_fromPV.size();
    if ( IsoTrack_isFromLostTrack.size() > nobj_ ) nobj_ = IsoTrack_isFromLostTrack.size();
    if ( IsoTrack_isHighPurityTrack.size() > nobj_ ) nobj_ = IsoTrack_isHighPurityTrack.size();
    if ( IsoTrack_isPFcand.size() > nobj_ ) nobj_ = IsoTrack_isPFcand.size();
    if ( IsoTrack_miniPFRelIso_all.size() > nobj_ ) nobj_ = IsoTrack_miniPFRelIso_all.size();
    if ( IsoTrack_miniPFRelIso_chg.size() > nobj_ ) nobj_ = IsoTrack_miniPFRelIso_chg.size();
    if ( IsoTrack_pdgId.size() > nobj_ ) nobj_ = IsoTrack_pdgId.size();
    if ( IsoTrack_pfRelIso03_all.size() > nobj_ ) nobj_ = IsoTrack_pfRelIso03_all.size();
    if ( IsoTrack_pfRelIso03_chg.size() > nobj_ ) nobj_ = IsoTrack_pfRelIso03_chg.size();
    if ( IsoTrack_phi.size() > nobj_ ) nobj_ = IsoTrack_phi.size();
    if ( IsoTrack_pt.size() > nobj_ ) nobj_ = IsoTrack_pt.size();
    IsoTrack.resize(nobj_);
    for(unsigned int i=0; i < IsoTrack.size(); ++i)
      {
        IsoTrack[i].charge	= (IsoTrack_charge.size() > i) ? IsoTrack_charge[i] : 0;
        IsoTrack[i].dxy	= (IsoTrack_dxy.size() > i) ? IsoTrack_dxy[i] : 0;
        IsoTrack[i].dz	= (IsoTrack_dz.size() > i) ? IsoTrack_dz[i] : 0;
        IsoTrack[i].eta	= (IsoTrack_eta.size() > i) ? IsoTrack_eta[i] : 0;
        IsoTrack[i].fromPV	= (IsoTrack_fromPV.size() > i) ? IsoTrack_fromPV[i] : 0;
        IsoTrack[i].isFromLostTrack	= (IsoTrack_isFromLostTrack.size() > i) ? (bool)IsoTrack_isFromLostTrack[i] : 0;
        IsoTrack[i].isHighPurityTrack	= (IsoTrack_isHighPurityTrack.size() > i) ? (bool)IsoTrack_isHighPurityTrack[i] : 0;
        IsoTrack[i].isPFcand	= (IsoTrack_isPFcand.size() > i) ? (bool)IsoTrack_isPFcand[i] : 0;
        IsoTrack[i].miniPFRelIso_all	= (IsoTrack_miniPFRelIso_all.size() > i) ? IsoTrack_miniPFRelIso_all[i] : 0;
        IsoTrack[i].miniPFRelIso_chg	= (IsoTrack_miniPFRelIso_chg.size() > i) ? IsoTrack_miniPFRelIso_chg[i] : 0;
        IsoTrack[i].pdgId	= (IsoTrack_pdgId.size() > i) ? IsoTrack_pdgId[i] : 0;
        IsoTrack[i].pfRelIso03_all	= (IsoTrack_pfRelIso03_all.size() > i) ? IsoTrack_pfRelIso03_all[i] : 0;
        IsoTrack[i].pfRelIso03_chg	= (IsoTrack_pfRelIso03_chg.size() > i) ? IsoTrack_pfRelIso03_chg[i] : 0;
        IsoTrack[i].phi	= (IsoTrack_phi.size() > i) ? IsoTrack_phi[i] : 0;
        IsoTrack[i].pt	= (IsoTrack_pt.size() > i) ? IsoTrack_pt[i] : 0;
      }
  }

  void fillJets()
  {
    size_t nobj_ = 0;
    if ( Jet_PNetRegPtRawCorr.size() > nobj_ ) nobj_ = Jet_PNetRegPtRawCorr.size();
    if ( Jet_PNetRegPtRawCorrNeutrino.size() > nobj_ ) nobj_ = Jet_PNetRegPtRawCorrNeutrino.size();
    if ( Jet_PNetRegPtRawRes.size() > nobj_ ) nobj_ = Jet_PNetRegPtRawRes.size();
    if ( Jet_UParTAK4RegPtRawCorr.size() > nobj_ ) nobj_ = Jet_UParTAK4RegPtRawCorr.size();
    if ( Jet_UParTAK4RegPtRawCorrNeutrino.size() > nobj_ ) nobj_ = Jet_UParTAK4RegPtRawCorrNeutrino.size();
    if ( Jet_UParTAK4RegPtRawRes.size() > nobj_ ) nobj_ = Jet_UParTAK4RegPtRawRes.size();
    if ( Jet_UParTAK4V1RegPtRawCorr.size() > nobj_ ) nobj_ = Jet_UParTAK4V1RegPtRawCorr.size();
    if ( Jet_UParTAK4V1RegPtRawCorrNeutrino.size() > nobj_ ) nobj_ = Jet_UParTAK4V1RegPtRawCorrNeutrino.size();
    if ( Jet_UParTAK4V1RegPtRawRes.size() > nobj_ ) nobj_ = Jet_UParTAK4V1RegPtRawRes.size();
    if ( Jet_area.size() > nobj_ ) nobj_ = Jet_area.size();
    if ( Jet_btagDeepFlavB.size() > nobj_ ) nobj_ = Jet_btagDeepFlavB.size();
    if ( Jet_btagDeepFlavCvB.size() > nobj_ ) nobj_ = Jet_btagDeepFlavCvB.size();
    if ( Jet_btagDeepFlavCvL.size() > nobj_ ) nobj_ = Jet_btagDeepFlavCvL.size();
    if ( Jet_btagDeepFlavQG.size() > nobj_ ) nobj_ = Jet_btagDeepFlavQG.size();
    if ( Jet_btagPNetB.size() > nobj_ ) nobj_ = Jet_btagPNetB.size();
    if ( Jet_btagPNetCvB.size() > nobj_ ) nobj_ = Jet_btagPNetCvB.size();
    if ( Jet_btagPNetCvL.size() > nobj_ ) nobj_ = Jet_btagPNetCvL.size();
    if ( Jet_btagPNetQvG.size() > nobj_ ) nobj_ = Jet_btagPNetQvG.size();
    if ( Jet_btagUParTAK4B.size() > nobj_ ) nobj_ = Jet_btagUParTAK4B.size();
    if ( Jet_btagUParTAK4CvB.size() > nobj_ ) nobj_ = Jet_btagUParTAK4CvB.size();
    if ( Jet_btagUParTAK4CvL.size() > nobj_ ) nobj_ = Jet_btagUParTAK4CvL.size();
    if ( Jet_btagUParTAK4QvG.size() > nobj_ ) nobj_ = Jet_btagUParTAK4QvG.size();
    if ( Jet_chEmEF.size() > nobj_ ) nobj_ = Jet_chEmEF.size();
    if ( Jet_chHEF.size() > nobj_ ) nobj_ = Jet_chHEF.size();
    if ( Jet_chMultiplicity.size() > nobj_ ) nobj_ = Jet_chMultiplicity.size();
    if ( Jet_electronIdx1.size() > nobj_ ) nobj_ = Jet_electronIdx1.size();
    if ( Jet_electronIdx2.size() > nobj_ ) nobj_ = Jet_electronIdx2.size();
    if ( Jet_eta.size() > nobj_ ) nobj_ = Jet_eta.size();
    if ( Jet_hfEmEF.size() > nobj_ ) nobj_ = Jet_hfEmEF.size();
    if ( Jet_hfHEF.size() > nobj_ ) nobj_ = Jet_hfHEF.size();
    if ( Jet_mass.size() > nobj_ ) nobj_ = Jet_mass.size();
    if ( Jet_muEF.size() > nobj_ ) nobj_ = Jet_muEF.size();
    if ( Jet_muonIdx1.size() > nobj_ ) nobj_ = Jet_muonIdx1.size();
    if ( Jet_muonIdx2.size() > nobj_ ) nobj_ = Jet_muonIdx2.size();
    if ( Jet_muonSubtrFactor.size() > nobj_ ) nobj_ = Jet_muonSubtrFactor.size();
    if ( Jet_nConstituents.size() > nobj_ ) nobj_ = Jet_nConstituents.size();
    if ( Jet_nElectrons.size() > nobj_ ) nobj_ = Jet_nElectrons.size();
    if ( Jet_nMuons.size() > nobj_ ) nobj_ = Jet_nMuons.size();
    if ( Jet_neEmEF.size() > nobj_ ) nobj_ = Jet_neEmEF.size();
    if ( Jet_neHEF.size() > nobj_ ) nobj_ = Jet_neHEF.size();
    if ( Jet_neMultiplicity.size() > nobj_ ) nobj_ = Jet_neMultiplicity.size();
    if ( Jet_phi.size() > nobj_ ) nobj_ = Jet_phi.size();
    if ( Jet_pt.size() > nobj_ ) nobj_ = Jet_pt.size();
    if ( Jet_puIdDisc.size() > nobj_ ) nobj_ = Jet_puIdDisc.size();
    if ( Jet_rawFactor.size() > nobj_ ) nobj_ = Jet_rawFactor.size();
    if ( Jet_genJetIdx.size() > nobj_ ) nobj_ = Jet_genJetIdx.size();
    if ( Jet_hadronFlavour.size() > nobj_ ) nobj_ = Jet_hadronFlavour.size();
    if ( Jet_partonFlavour.size() > nobj_ ) nobj_ = Jet_partonFlavour.size();
    if ( Jet_bRegCorr.size() > nobj_ ) nobj_ = Jet_bRegCorr.size();
    if ( Jet_bRegRes.size() > nobj_ ) nobj_ = Jet_bRegRes.size();
    if ( Jet_btagCSVV2.size() > nobj_ ) nobj_ = Jet_btagCSVV2.size();
    if ( Jet_btagDeepB.size() > nobj_ ) nobj_ = Jet_btagDeepB.size();
    if ( Jet_btagDeepCvB.size() > nobj_ ) nobj_ = Jet_btagDeepCvB.size();
    if ( Jet_btagDeepCvL.size() > nobj_ ) nobj_ = Jet_btagDeepCvL.size();
    if ( Jet_cRegCorr.size() > nobj_ ) nobj_ = Jet_cRegCorr.size();
    if ( Jet_cRegRes.size() > nobj_ ) nobj_ = Jet_cRegRes.size();
    if ( Jet_chFPV0EF.size() > nobj_ ) nobj_ = Jet_chFPV0EF.size();
    if ( Jet_cleanmask.size() > nobj_ ) nobj_ = Jet_cleanmask.size();
    if ( Jet_hfadjacentEtaStripsSize.size() > nobj_ ) nobj_ = Jet_hfadjacentEtaStripsSize.size();
    if ( Jet_hfcentralEtaStripSize.size() > nobj_ ) nobj_ = Jet_hfcentralEtaStripSize.size();
    if ( Jet_hfsigmaEtaEta.size() > nobj_ ) nobj_ = Jet_hfsigmaEtaEta.size();
    if ( Jet_hfsigmaPhiPhi.size() > nobj_ ) nobj_ = Jet_hfsigmaPhiPhi.size();
    if ( Jet_jetId.size() > nobj_ ) nobj_ = Jet_jetId.size();
    if ( Jet_puId.size() > nobj_ ) nobj_ = Jet_puId.size();
    if ( Jet_qgl.size() > nobj_ ) nobj_ = Jet_qgl.size();
    Jet.resize(nobj_);
    for(unsigned int i=0; i < Jet.size(); ++i)
      {
        Jet[i].PNetRegPtRawCorr	= (Jet_PNetRegPtRawCorr.size() > i) ? Jet_PNetRegPtRawCorr[i] : 0;
        Jet[i].PNetRegPtRawCorrNeutrino	= (Jet_PNetRegPtRawCorrNeutrino.size() > i) ? Jet_PNetRegPtRawCorrNeutrino[i] : 0;
        Jet[i].PNetRegPtRawRes	= (Jet_PNetRegPtRawRes.size() > i) ? Jet_PNetRegPtRawRes[i] : 0;
        Jet[i].UParTAK4RegPtRawCorr	= (Jet_UParTAK4RegPtRawCorr.size() > i) ? Jet_UParTAK4RegPtRawCorr[i] : 0;
        Jet[i].UParTAK4RegPtRawCorrNeutrino	= (Jet_UParTAK4RegPtRawCorrNeutrino.size() > i) ? Jet_UParTAK4RegPtRawCorrNeutrino[i] : 0;
        Jet[i].UParTAK4RegPtRawRes	= (Jet_UParTAK4RegPtRawRes.size() > i) ? Jet_UParTAK4RegPtRawRes[i] : 0;
        Jet[i].UParTAK4V1RegPtRawCorr	= (Jet_UParTAK4V1RegPtRawCorr.size() > i) ? Jet_UParTAK4V1RegPtRawCorr[i] : 0;
        Jet[i].UParTAK4V1RegPtRawCorrNeutrino	= (Jet_UParTAK4V1RegPtRawCorrNeutrino.size() > i) ? Jet_UParTAK4V1RegPtRawCorrNeutrino[i] : 0;
        Jet[i].UParTAK4V1RegPtRawRes	= (Jet_UParTAK4V1RegPtRawRes.size() > i) ? Jet_UParTAK4V1RegPtRawRes[i] : 0;
        Jet[i].area	= (Jet_area.size() > i) ? Jet_area[i] : 0;
        Jet[i].btagDeepFlavB	= (Jet_btagDeepFlavB.size() > i) ? Jet_btagDeepFlavB[i] : 0;
        Jet[i].btagDeepFlavCvB	= (Jet_btagDeepFlavCvB.size() > i) ? Jet_btagDeepFlavCvB[i] : 0;
        Jet[i].btagDeepFlavCvL	= (Jet_btagDeepFlavCvL.size() > i) ? Jet_btagDeepFlavCvL[i] : 0;
        Jet[i].btagDeepFlavQG	= (Jet_btagDeepFlavQG.size() > i) ? Jet_btagDeepFlavQG[i] : 0;
        Jet[i].btagPNetB	= (Jet_btagPNetB.size() > i) ? Jet_btagPNetB[i] : 0;
        Jet[i].btagPNetCvB	= (Jet_btagPNetCvB.size() > i) ? Jet_btagPNetCvB[i] : 0;
        Jet[i].btagPNetCvL	= (Jet_btagPNetCvL.size() > i) ? Jet_btagPNetCvL[i] : 0;
        Jet[i].btagPNetQvG	= (Jet_btagPNetQvG.size() > i) ? Jet_btagPNetQvG[i] : 0;
        Jet[i].btagUParTAK4B	= (Jet_btagUParTAK4B.size() > i) ? Jet_btagUParTAK4B[i] : 0;
        Jet[i].btagUParTAK4CvB	= (Jet_btagUParTAK4CvB.size() > i) ? Jet_btagUParTAK4CvB[i] : 0;
        Jet[i].btagUParTAK4CvL	= (Jet_btagUParTAK4CvL.size() > i) ? Jet_btagUParTAK4CvL[i] : 0;
        Jet[i].btagUParTAK4QvG	= (Jet_btagUParTAK4QvG.size() > i) ? Jet_btagUParTAK4QvG[i] : 0;
        Jet[i].chEmEF	= (Jet_chEmEF.size() > i) ? Jet_chEmEF[i] : 0;
        Jet[i].chHEF	= (Jet_chHEF.size() > i) ? Jet_chHEF[i] : 0;
        Jet[i].chMultiplicity	= (Jet_chMultiplicity.size() > i) ? Jet_chMultiplicity[i] : 0;
        Jet[i].electronIdx1	= (Jet_electronIdx1.size() > i) ? Jet_electronIdx1[i] : 0;
        Jet[i].electronIdx2	= (Jet_electronIdx2.size() > i) ? Jet_electronIdx2[i] : 0;
        Jet[i].eta	= (Jet_eta.size() > i) ? Jet_eta[i] : 0;
        Jet[i].hfEmEF	= (Jet_hfEmEF.size() > i) ? Jet_hfEmEF[i] : 0;
        Jet[i].hfHEF	= (Jet_hfHEF.size() > i) ? Jet_hfHEF[i] : 0;
        Jet[i].mass	= (Jet_mass.size() > i) ? Jet_mass[i] : 0;
        Jet[i].muEF	= (Jet_muEF.size() > i) ? Jet_muEF[i] : 0;
        Jet[i].muonIdx1	= (Jet_muonIdx1.size() > i) ? Jet_muonIdx1[i] : 0;
        Jet[i].muonIdx2	= (Jet_muonIdx2.size() > i) ? Jet_muonIdx2[i] : 0;
        Jet[i].muonSubtrFactor	= (Jet_muonSubtrFactor.size() > i) ? Jet_muonSubtrFactor[i] : 0;
        Jet[i].nConstituents	= (Jet_nConstituents.size() > i) ? Jet_nConstituents[i] : 0;
        Jet[i].nElectrons	= (Jet_nElectrons.size() > i) ? Jet_nElectrons[i] : 0;
        Jet[i].nMuons	= (Jet_nMuons.size() > i) ? Jet_nMuons[i] : 0;
        Jet[i].neEmEF	= (Jet_neEmEF.size() > i) ? Jet_neEmEF[i] : 0;
        Jet[i].neHEF	= (Jet_neHEF.size() > i) ? Jet_neHEF[i] : 0;
        Jet[i].neMultiplicity	= (Jet_neMultiplicity.size() > i) ? Jet_neMultiplicity[i] : 0;
        Jet[i].phi	= (Jet_phi.size() > i) ? Jet_phi[i] : 0;
        Jet[i].pt	= (Jet_pt.size() > i) ? Jet_pt[i] : 0;
        Jet[i].puIdDisc	= (Jet_puIdDisc.size() > i) ? Jet_puIdDisc[i] : 0;
        Jet[i].rawFactor	= (Jet_rawFactor.size() > i) ? Jet_rawFactor[i] : 0;
        Jet[i].genJetIdx	= (Jet_genJetIdx.size() > i) ? Jet_genJetIdx[i] : 0;
        Jet[i].hadronFlavour	= (Jet_hadronFlavour.size() > i) ? Jet_hadronFlavour[i] : 0;
        Jet[i].partonFlavour	= (Jet_partonFlavour.size() > i) ? Jet_partonFlavour[i] : 0;
        Jet[i].bRegCorr	= (Jet_bRegCorr.size() > i) ? Jet_bRegCorr[i] : 0;
        Jet[i].bRegRes	= (Jet_bRegRes.size() > i) ? Jet_bRegRes[i] : 0;
        Jet[i].btagCSVV2	= (Jet_btagCSVV2.size() > i) ? Jet_btagCSVV2[i] : 0;
        Jet[i].btagDeepB	= (Jet_btagDeepB.size() > i) ? Jet_btagDeepB[i] : 0;
        Jet[i].btagDeepCvB	= (Jet_btagDeepCvB.size() > i) ? Jet_btagDeepCvB[i] : 0;
        Jet[i].btagDeepCvL	= (Jet_btagDeepCvL.size() > i) ? Jet_btagDeepCvL[i] : 0;
        Jet[i].cRegCorr	= (Jet_cRegCorr.size() > i) ? Jet_cRegCorr[i] : 0;
        Jet[i].cRegRes	= (Jet_cRegRes.size() > i) ? Jet_cRegRes[i] : 0;
        Jet[i].chFPV0EF	= (Jet_chFPV0EF.size() > i) ? Jet_chFPV0EF[i] : 0;
        Jet[i].cleanmask	= (Jet_cleanmask.size() > i) ? Jet_cleanmask[i] : 0;
        Jet[i].hfadjacentEtaStripsSize	= (Jet_hfadjacentEtaStripsSize.size() > i) ? Jet_hfadjacentEtaStripsSize[i] : 0;
        Jet[i].hfcentralEtaStripSize	= (Jet_hfcentralEtaStripSize.size() > i) ? Jet_hfcentralEtaStripSize[i] : 0;
        Jet[i].hfsigmaEtaEta	= (Jet_hfsigmaEtaEta.size() > i) ? Jet_hfsigmaEtaEta[i] : 0;
        Jet[i].hfsigmaPhiPhi	= (Jet_hfsigmaPhiPhi.size() > i) ? Jet_hfsigmaPhiPhi[i] : 0;
        Jet[i].jetId	= (Jet_jetId.size() > i) ? Jet_jetId[i] : 0;
        Jet[i].puId	= (Jet_puId.size() > i) ? Jet_puId[i] : 0;
        Jet[i].qgl	= (Jet_qgl.size() > i) ? Jet_qgl[i] : 0;
      }
  }

  void fillLHEParts()
  {
    size_t nobj_ = 0;
    if ( LHEPart_eta.size() > nobj_ ) nobj_ = LHEPart_eta.size();
    if ( LHEPart_incomingpz.size() > nobj_ ) nobj_ = LHEPart_incomingpz.size();
    if ( LHEPart_mass.size() > nobj_ ) nobj_ = LHEPart_mass.size();
    if ( LHEPart_pdgId.size() > nobj_ ) nobj_ = LHEPart_pdgId.size();
    if ( LHEPart_phi.size() > nobj_ ) nobj_ = LHEPart_phi.size();
    if ( LHEPart_pt.size() > nobj_ ) nobj_ = LHEPart_pt.size();
    if ( LHEPart_spin.size() > nobj_ ) nobj_ = LHEPart_spin.size();
    if ( LHEPart_status.size() > nobj_ ) nobj_ = LHEPart_status.size();
    LHEPart.resize(nobj_);
    for(unsigned int i=0; i < LHEPart.size(); ++i)
      {
        LHEPart[i].eta	= (LHEPart_eta.size() > i) ? LHEPart_eta[i] : 0;
        LHEPart[i].incomingpz	= (LHEPart_incomingpz.size() > i) ? LHEPart_incomingpz[i] : 0;
        LHEPart[i].mass	= (LHEPart_mass.size() > i) ? LHEPart_mass[i] : 0;
        LHEPart[i].pdgId	= (LHEPart_pdgId.size() > i) ? LHEPart_pdgId[i] : 0;
        LHEPart[i].phi	= (LHEPart_phi.size() > i) ? LHEPart_phi[i] : 0;
        LHEPart[i].pt	= (LHEPart_pt.size() > i) ? LHEPart_pt[i] : 0;
        LHEPart[i].spin	= (LHEPart_spin.size() > i) ? LHEPart_spin[i] : 0;
        LHEPart[i].status	= (LHEPart_status.size() > i) ? LHEPart_status[i] : 0;
      }
  }

  void fillLowPtElectrons()
  {
    size_t nobj_ = 0;
    if ( LowPtElectron_ID.size() > nobj_ ) nobj_ = LowPtElectron_ID.size();
    if ( LowPtElectron_charge.size() > nobj_ ) nobj_ = LowPtElectron_charge.size();
    if ( LowPtElectron_convVeto.size() > nobj_ ) nobj_ = LowPtElectron_convVeto.size();
    if ( LowPtElectron_convVtxRadius.size() > nobj_ ) nobj_ = LowPtElectron_convVtxRadius.size();
    if ( LowPtElectron_convWP.size() > nobj_ ) nobj_ = LowPtElectron_convWP.size();
    if ( LowPtElectron_deltaEtaSC.size() > nobj_ ) nobj_ = LowPtElectron_deltaEtaSC.size();
    if ( LowPtElectron_dxy.size() > nobj_ ) nobj_ = LowPtElectron_dxy.size();
    if ( LowPtElectron_dxyErr.size() > nobj_ ) nobj_ = LowPtElectron_dxyErr.size();
    if ( LowPtElectron_dz.size() > nobj_ ) nobj_ = LowPtElectron_dz.size();
    if ( LowPtElectron_dzErr.size() > nobj_ ) nobj_ = LowPtElectron_dzErr.size();
    if ( LowPtElectron_eInvMinusPInv.size() > nobj_ ) nobj_ = LowPtElectron_eInvMinusPInv.size();
    if ( LowPtElectron_embeddedID.size() > nobj_ ) nobj_ = LowPtElectron_embeddedID.size();
    if ( LowPtElectron_energyErr.size() > nobj_ ) nobj_ = LowPtElectron_energyErr.size();
    if ( LowPtElectron_eta.size() > nobj_ ) nobj_ = LowPtElectron_eta.size();
    if ( LowPtElectron_genPartFlav.size() > nobj_ ) nobj_ = LowPtElectron_genPartFlav.size();
    if ( LowPtElectron_genPartIdx.size() > nobj_ ) nobj_ = LowPtElectron_genPartIdx.size();
    if ( LowPtElectron_hoe.size() > nobj_ ) nobj_ = LowPtElectron_hoe.size();
    if ( LowPtElectron_lostHits.size() > nobj_ ) nobj_ = LowPtElectron_lostHits.size();
    if ( LowPtElectron_mass.size() > nobj_ ) nobj_ = LowPtElectron_mass.size();
    if ( LowPtElectron_miniPFRelIso_all.size() > nobj_ ) nobj_ = LowPtElectron_miniPFRelIso_all.size();
    if ( LowPtElectron_miniPFRelIso_chg.size() > nobj_ ) nobj_ = LowPtElectron_miniPFRelIso_chg.size();
    if ( LowPtElectron_pdgId.size() > nobj_ ) nobj_ = LowPtElectron_pdgId.size();
    if ( LowPtElectron_phi.size() > nobj_ ) nobj_ = LowPtElectron_phi.size();
    if ( LowPtElectron_pt.size() > nobj_ ) nobj_ = LowPtElectron_pt.size();
    if ( LowPtElectron_ptbiased.size() > nobj_ ) nobj_ = LowPtElectron_ptbiased.size();
    if ( LowPtElectron_r9.size() > nobj_ ) nobj_ = LowPtElectron_r9.size();
    if ( LowPtElectron_scEtOverPt.size() > nobj_ ) nobj_ = LowPtElectron_scEtOverPt.size();
    if ( LowPtElectron_sieie.size() > nobj_ ) nobj_ = LowPtElectron_sieie.size();
    if ( LowPtElectron_unbiased.size() > nobj_ ) nobj_ = LowPtElectron_unbiased.size();
    LowPtElectron.resize(nobj_);
    for(unsigned int i=0; i < LowPtElectron.size(); ++i)
      {
        LowPtElectron[i].ID	= (LowPtElectron_ID.size() > i) ? LowPtElectron_ID[i] : 0;
        LowPtElectron[i].charge	= (LowPtElectron_charge.size() > i) ? LowPtElectron_charge[i] : 0;
        LowPtElectron[i].convVeto	= (LowPtElectron_convVeto.size() > i) ? (bool)LowPtElectron_convVeto[i] : 0;
        LowPtElectron[i].convVtxRadius	= (LowPtElectron_convVtxRadius.size() > i) ? LowPtElectron_convVtxRadius[i] : 0;
        LowPtElectron[i].convWP	= (LowPtElectron_convWP.size() > i) ? LowPtElectron_convWP[i] : 0;
        LowPtElectron[i].deltaEtaSC	= (LowPtElectron_deltaEtaSC.size() > i) ? LowPtElectron_deltaEtaSC[i] : 0;
        LowPtElectron[i].dxy	= (LowPtElectron_dxy.size() > i) ? LowPtElectron_dxy[i] : 0;
        LowPtElectron[i].dxyErr	= (LowPtElectron_dxyErr.size() > i) ? LowPtElectron_dxyErr[i] : 0;
        LowPtElectron[i].dz	= (LowPtElectron_dz.size() > i) ? LowPtElectron_dz[i] : 0;
        LowPtElectron[i].dzErr	= (LowPtElectron_dzErr.size() > i) ? LowPtElectron_dzErr[i] : 0;
        LowPtElectron[i].eInvMinusPInv	= (LowPtElectron_eInvMinusPInv.size() > i) ? LowPtElectron_eInvMinusPInv[i] : 0;
        LowPtElectron[i].embeddedID	= (LowPtElectron_embeddedID.size() > i) ? LowPtElectron_embeddedID[i] : 0;
        LowPtElectron[i].energyErr	= (LowPtElectron_energyErr.size() > i) ? LowPtElectron_energyErr[i] : 0;
        LowPtElectron[i].eta	= (LowPtElectron_eta.size() > i) ? LowPtElectron_eta[i] : 0;
        LowPtElectron[i].genPartFlav	= (LowPtElectron_genPartFlav.size() > i) ? LowPtElectron_genPartFlav[i] : 0;
        LowPtElectron[i].genPartIdx	= (LowPtElectron_genPartIdx.size() > i) ? LowPtElectron_genPartIdx[i] : 0;
        LowPtElectron[i].hoe	= (LowPtElectron_hoe.size() > i) ? LowPtElectron_hoe[i] : 0;
        LowPtElectron[i].lostHits	= (LowPtElectron_lostHits.size() > i) ? LowPtElectron_lostHits[i] : 0;
        LowPtElectron[i].mass	= (LowPtElectron_mass.size() > i) ? LowPtElectron_mass[i] : 0;
        LowPtElectron[i].miniPFRelIso_all	= (LowPtElectron_miniPFRelIso_all.size() > i) ? LowPtElectron_miniPFRelIso_all[i] : 0;
        LowPtElectron[i].miniPFRelIso_chg	= (LowPtElectron_miniPFRelIso_chg.size() > i) ? LowPtElectron_miniPFRelIso_chg[i] : 0;
        LowPtElectron[i].pdgId	= (LowPtElectron_pdgId.size() > i) ? LowPtElectron_pdgId[i] : 0;
        LowPtElectron[i].phi	= (LowPtElectron_phi.size() > i) ? LowPtElectron_phi[i] : 0;
        LowPtElectron[i].pt	= (LowPtElectron_pt.size() > i) ? LowPtElectron_pt[i] : 0;
        LowPtElectron[i].ptbiased	= (LowPtElectron_ptbiased.size() > i) ? LowPtElectron_ptbiased[i] : 0;
        LowPtElectron[i].r9	= (LowPtElectron_r9.size() > i) ? LowPtElectron_r9[i] : 0;
        LowPtElectron[i].scEtOverPt	= (LowPtElectron_scEtOverPt.size() > i) ? LowPtElectron_scEtOverPt[i] : 0;
        LowPtElectron[i].sieie	= (LowPtElectron_sieie.size() > i) ? LowPtElectron_sieie[i] : 0;
        LowPtElectron[i].unbiased	= (LowPtElectron_unbiased.size() > i) ? LowPtElectron_unbiased[i] : 0;
      }
  }

  void fillMuons()
  {
    size_t nobj_ = 0;
    if ( Muon_charge.size() > nobj_ ) nobj_ = Muon_charge.size();
    if ( Muon_dxy.size() > nobj_ ) nobj_ = Muon_dxy.size();
    if ( Muon_dz.size() > nobj_ ) nobj_ = Muon_dz.size();
    if ( Muon_eta.size() > nobj_ ) nobj_ = Muon_eta.size();
    if ( Muon_highPtId.size() > nobj_ ) nobj_ = Muon_highPtId.size();
    if ( Muon_ip3d.size() > nobj_ ) nobj_ = Muon_ip3d.size();
    if ( Muon_isGlobal.size() > nobj_ ) nobj_ = Muon_isGlobal.size();
    if ( Muon_isPFcand.size() > nobj_ ) nobj_ = Muon_isPFcand.size();
    if ( Muon_isTracker.size() > nobj_ ) nobj_ = Muon_isTracker.size();
    if ( Muon_jetIdx.size() > nobj_ ) nobj_ = Muon_jetIdx.size();
    if ( Muon_looseId.size() > nobj_ ) nobj_ = Muon_looseId.size();
    if ( Muon_mass.size() > nobj_ ) nobj_ = Muon_mass.size();
    if ( Muon_mediumId.size() > nobj_ ) nobj_ = Muon_mediumId.size();
    if ( Muon_mediumPromptId.size() > nobj_ ) nobj_ = Muon_mediumPromptId.size();
    if ( Muon_miniIsoId.size() > nobj_ ) nobj_ = Muon_miniIsoId.size();
    if ( Muon_miniPFRelIso_all.size() > nobj_ ) nobj_ = Muon_miniPFRelIso_all.size();
    if ( Muon_miniPFRelIso_chg.size() > nobj_ ) nobj_ = Muon_miniPFRelIso_chg.size();
    if ( Muon_mvaMuID.size() > nobj_ ) nobj_ = Muon_mvaMuID.size();
    if ( Muon_mvaMuID_WP.size() > nobj_ ) nobj_ = Muon_mvaMuID_WP.size();
    if ( Muon_nStations.size() > nobj_ ) nobj_ = Muon_nStations.size();
    if ( Muon_nTrackerLayers.size() > nobj_ ) nobj_ = Muon_nTrackerLayers.size();
    if ( Muon_pdgId.size() > nobj_ ) nobj_ = Muon_pdgId.size();
    if ( Muon_pfIsoId.size() > nobj_ ) nobj_ = Muon_pfIsoId.size();
    if ( Muon_pfRelIso03_all.size() > nobj_ ) nobj_ = Muon_pfRelIso03_all.size();
    if ( Muon_pfRelIso03_chg.size() > nobj_ ) nobj_ = Muon_pfRelIso03_chg.size();
    if ( Muon_pfRelIso04_all.size() > nobj_ ) nobj_ = Muon_pfRelIso04_all.size();
    if ( Muon_phi.size() > nobj_ ) nobj_ = Muon_phi.size();
    if ( Muon_promptMVA.size() > nobj_ ) nobj_ = Muon_promptMVA.size();
    if ( Muon_pt.size() > nobj_ ) nobj_ = Muon_pt.size();
    if ( Muon_ptErr.size() > nobj_ ) nobj_ = Muon_ptErr.size();
    if ( Muon_sip3d.size() > nobj_ ) nobj_ = Muon_sip3d.size();
    if ( Muon_tightCharge.size() > nobj_ ) nobj_ = Muon_tightCharge.size();
    if ( Muon_tightId.size() > nobj_ ) nobj_ = Muon_tightId.size();
    if ( Muon_tkIsoId.size() > nobj_ ) nobj_ = Muon_tkIsoId.size();
    if ( Muon_tkRelIso.size() > nobj_ ) nobj_ = Muon_tkRelIso.size();
    if ( Muon_genPartFlav.size() > nobj_ ) nobj_ = Muon_genPartFlav.size();
    if ( Muon_genPartIdx.size() > nobj_ ) nobj_ = Muon_genPartIdx.size();
    if ( Muon_cleanmask.size() > nobj_ ) nobj_ = Muon_cleanmask.size();
    if ( Muon_dxyErr.size() > nobj_ ) nobj_ = Muon_dxyErr.size();
    if ( Muon_dxybs.size() > nobj_ ) nobj_ = Muon_dxybs.size();
    if ( Muon_dzErr.size() > nobj_ ) nobj_ = Muon_dzErr.size();
    if ( Muon_fsrPhotonIdx.size() > nobj_ ) nobj_ = Muon_fsrPhotonIdx.size();
    if ( Muon_highPurity.size() > nobj_ ) nobj_ = Muon_highPurity.size();
    if ( Muon_inTimeMuon.size() > nobj_ ) nobj_ = Muon_inTimeMuon.size();
    if ( Muon_isStandalone.size() > nobj_ ) nobj_ = Muon_isStandalone.size();
    if ( Muon_jetNDauCharged.size() > nobj_ ) nobj_ = Muon_jetNDauCharged.size();
    if ( Muon_jetPtRelv2.size() > nobj_ ) nobj_ = Muon_jetPtRelv2.size();
    if ( Muon_jetRelIso.size() > nobj_ ) nobj_ = Muon_jetRelIso.size();
    if ( Muon_multiIsoId.size() > nobj_ ) nobj_ = Muon_multiIsoId.size();
    if ( Muon_mvaId.size() > nobj_ ) nobj_ = Muon_mvaId.size();
    if ( Muon_mvaLowPt.size() > nobj_ ) nobj_ = Muon_mvaLowPt.size();
    if ( Muon_mvaLowPtId.size() > nobj_ ) nobj_ = Muon_mvaLowPtId.size();
    if ( Muon_mvaTTH.size() > nobj_ ) nobj_ = Muon_mvaTTH.size();
    if ( Muon_puppiIsoId.size() > nobj_ ) nobj_ = Muon_puppiIsoId.size();
    if ( Muon_segmentComp.size() > nobj_ ) nobj_ = Muon_segmentComp.size();
    if ( Muon_softId.size() > nobj_ ) nobj_ = Muon_softId.size();
    if ( Muon_softMva.size() > nobj_ ) nobj_ = Muon_softMva.size();
    if ( Muon_softMvaId.size() > nobj_ ) nobj_ = Muon_softMvaId.size();
    if ( Muon_triggerIdLoose.size() > nobj_ ) nobj_ = Muon_triggerIdLoose.size();
    if ( Muon_tunepRelPt.size() > nobj_ ) nobj_ = Muon_tunepRelPt.size();
    Muon.resize(nobj_);
    for(unsigned int i=0; i < Muon.size(); ++i)
      {
        Muon[i].charge	= (Muon_charge.size() > i) ? Muon_charge[i] : 0;
        Muon[i].dxy	= (Muon_dxy.size() > i) ? Muon_dxy[i] : 0;
        Muon[i].dz	= (Muon_dz.size() > i) ? Muon_dz[i] : 0;
        Muon[i].eta	= (Muon_eta.size() > i) ? Muon_eta[i] : 0;
        Muon[i].highPtId	= (Muon_highPtId.size() > i) ? Muon_highPtId[i] : 0;
        Muon[i].ip3d	= (Muon_ip3d.size() > i) ? Muon_ip3d[i] : 0;
        Muon[i].isGlobal	= (Muon_isGlobal.size() > i) ? (bool)Muon_isGlobal[i] : 0;
        Muon[i].isPFcand	= (Muon_isPFcand.size() > i) ? (bool)Muon_isPFcand[i] : 0;
        Muon[i].isTracker	= (Muon_isTracker.size() > i) ? (bool)Muon_isTracker[i] : 0;
        Muon[i].jetIdx	= (Muon_jetIdx.size() > i) ? Muon_jetIdx[i] : 0;
        Muon[i].looseId	= (Muon_looseId.size() > i) ? (bool)Muon_looseId[i] : 0;
        Muon[i].mass	= (Muon_mass.size() > i) ? Muon_mass[i] : 0;
        Muon[i].mediumId	= (Muon_mediumId.size() > i) ? (bool)Muon_mediumId[i] : 0;
        Muon[i].mediumPromptId	= (Muon_mediumPromptId.size() > i) ? (bool)Muon_mediumPromptId[i] : 0;
        Muon[i].miniIsoId	= (Muon_miniIsoId.size() > i) ? Muon_miniIsoId[i] : 0;
        Muon[i].miniPFRelIso_all	= (Muon_miniPFRelIso_all.size() > i) ? Muon_miniPFRelIso_all[i] : 0;
        Muon[i].miniPFRelIso_chg	= (Muon_miniPFRelIso_chg.size() > i) ? Muon_miniPFRelIso_chg[i] : 0;
        Muon[i].mvaMuID	= (Muon_mvaMuID.size() > i) ? Muon_mvaMuID[i] : 0;
        Muon[i].mvaMuID_WP	= (Muon_mvaMuID_WP.size() > i) ? Muon_mvaMuID_WP[i] : 0;
        Muon[i].nStations	= (Muon_nStations.size() > i) ? Muon_nStations[i] : 0;
        Muon[i].nTrackerLayers	= (Muon_nTrackerLayers.size() > i) ? Muon_nTrackerLayers[i] : 0;
        Muon[i].pdgId	= (Muon_pdgId.size() > i) ? Muon_pdgId[i] : 0;
        Muon[i].pfIsoId	= (Muon_pfIsoId.size() > i) ? Muon_pfIsoId[i] : 0;
        Muon[i].pfRelIso03_all	= (Muon_pfRelIso03_all.size() > i) ? Muon_pfRelIso03_all[i] : 0;
        Muon[i].pfRelIso03_chg	= (Muon_pfRelIso03_chg.size() > i) ? Muon_pfRelIso03_chg[i] : 0;
        Muon[i].pfRelIso04_all	= (Muon_pfRelIso04_all.size() > i) ? Muon_pfRelIso04_all[i] : 0;
        Muon[i].phi	= (Muon_phi.size() > i) ? Muon_phi[i] : 0;
        Muon[i].promptMVA	= (Muon_promptMVA.size() > i) ? Muon_promptMVA[i] : 0;
        Muon[i].pt	= (Muon_pt.size() > i) ? Muon_pt[i] : 0;
        Muon[i].ptErr	= (Muon_ptErr.size() > i) ? Muon_ptErr[i] : 0;
        Muon[i].sip3d	= (Muon_sip3d.size() > i) ? Muon_sip3d[i] : 0;
        Muon[i].tightCharge	= (Muon_tightCharge.size() > i) ? Muon_tightCharge[i] : 0;
        Muon[i].tightId	= (Muon_tightId.size() > i) ? (bool)Muon_tightId[i] : 0;
        Muon[i].tkIsoId	= (Muon_tkIsoId.size() > i) ? Muon_tkIsoId[i] : 0;
        Muon[i].tkRelIso	= (Muon_tkRelIso.size() > i) ? Muon_tkRelIso[i] : 0;
        Muon[i].genPartFlav	= (Muon_genPartFlav.size() > i) ? Muon_genPartFlav[i] : 0;
        Muon[i].genPartIdx	= (Muon_genPartIdx.size() > i) ? Muon_genPartIdx[i] : 0;
        Muon[i].cleanmask	= (Muon_cleanmask.size() > i) ? Muon_cleanmask[i] : 0;
        Muon[i].dxyErr	= (Muon_dxyErr.size() > i) ? Muon_dxyErr[i] : 0;
        Muon[i].dxybs	= (Muon_dxybs.size() > i) ? Muon_dxybs[i] : 0;
        Muon[i].dzErr	= (Muon_dzErr.size() > i) ? Muon_dzErr[i] : 0;
        Muon[i].fsrPhotonIdx	= (Muon_fsrPhotonIdx.size() > i) ? Muon_fsrPhotonIdx[i] : 0;
        Muon[i].highPurity	= (Muon_highPurity.size() > i) ? (bool)Muon_highPurity[i] : 0;
        Muon[i].inTimeMuon	= (Muon_inTimeMuon.size() > i) ? (bool)Muon_inTimeMuon[i] : 0;
        Muon[i].isStandalone	= (Muon_isStandalone.size() > i) ? (bool)Muon_isStandalone[i] : 0;
        Muon[i].jetNDauCharged	= (Muon_jetNDauCharged.size() > i) ? Muon_jetNDauCharged[i] : 0;
        Muon[i].jetPtRelv2	= (Muon_jetPtRelv2.size() > i) ? Muon_jetPtRelv2[i] : 0;
        Muon[i].jetRelIso	= (Muon_jetRelIso.size() > i) ? Muon_jetRelIso[i] : 0;
        Muon[i].multiIsoId	= (Muon_multiIsoId.size() > i) ? Muon_multiIsoId[i] : 0;
        Muon[i].mvaId	= (Muon_mvaId.size() > i) ? Muon_mvaId[i] : 0;
        Muon[i].mvaLowPt	= (Muon_mvaLowPt.size() > i) ? Muon_mvaLowPt[i] : 0;
        Muon[i].mvaLowPtId	= (Muon_mvaLowPtId.size() > i) ? Muon_mvaLowPtId[i] : 0;
        Muon[i].mvaTTH	= (Muon_mvaTTH.size() > i) ? Muon_mvaTTH[i] : 0;
        Muon[i].puppiIsoId	= (Muon_puppiIsoId.size() > i) ? Muon_puppiIsoId[i] : 0;
        Muon[i].segmentComp	= (Muon_segmentComp.size() > i) ? Muon_segmentComp[i] : 0;
        Muon[i].softId	= (Muon_softId.size() > i) ? (bool)Muon_softId[i] : 0;
        Muon[i].softMva	= (Muon_softMva.size() > i) ? Muon_softMva[i] : 0;
        Muon[i].softMvaId	= (Muon_softMvaId.size() > i) ? (bool)Muon_softMvaId[i] : 0;
        Muon[i].triggerIdLoose	= (Muon_triggerIdLoose.size() > i) ? (bool)Muon_triggerIdLoose[i] : 0;
        Muon[i].tunepRelPt	= (Muon_tunepRelPt.size() > i) ? Muon_tunepRelPt[i] : 0;
      }
  }

  void fillPPSLocalTracks()
  {
    size_t nobj_ = 0;
    if ( PPSLocalTrack_decRPId.size() > nobj_ ) nobj_ = PPSLocalTrack_decRPId.size();
    if ( PPSLocalTrack_multiRPProtonIdx.size() > nobj_ ) nobj_ = PPSLocalTrack_multiRPProtonIdx.size();
    if ( PPSLocalTrack_rpType.size() > nobj_ ) nobj_ = PPSLocalTrack_rpType.size();
    if ( PPSLocalTrack_singleRPProtonIdx.size() > nobj_ ) nobj_ = PPSLocalTrack_singleRPProtonIdx.size();
    if ( PPSLocalTrack_time.size() > nobj_ ) nobj_ = PPSLocalTrack_time.size();
    if ( PPSLocalTrack_timeUnc.size() > nobj_ ) nobj_ = PPSLocalTrack_timeUnc.size();
    if ( PPSLocalTrack_x.size() > nobj_ ) nobj_ = PPSLocalTrack_x.size();
    if ( PPSLocalTrack_y.size() > nobj_ ) nobj_ = PPSLocalTrack_y.size();
    PPSLocalTrack.resize(nobj_);
    for(unsigned int i=0; i < PPSLocalTrack.size(); ++i)
      {
        PPSLocalTrack[i].decRPId	= (PPSLocalTrack_decRPId.size() > i) ? PPSLocalTrack_decRPId[i] : 0;
        PPSLocalTrack[i].multiRPProtonIdx	= (PPSLocalTrack_multiRPProtonIdx.size() > i) ? PPSLocalTrack_multiRPProtonIdx[i] : 0;
        PPSLocalTrack[i].rpType	= (PPSLocalTrack_rpType.size() > i) ? PPSLocalTrack_rpType[i] : 0;
        PPSLocalTrack[i].singleRPProtonIdx	= (PPSLocalTrack_singleRPProtonIdx.size() > i) ? PPSLocalTrack_singleRPProtonIdx[i] : 0;
        PPSLocalTrack[i].time	= (PPSLocalTrack_time.size() > i) ? PPSLocalTrack_time[i] : 0;
        PPSLocalTrack[i].timeUnc	= (PPSLocalTrack_timeUnc.size() > i) ? PPSLocalTrack_timeUnc[i] : 0;
        PPSLocalTrack[i].x	= (PPSLocalTrack_x.size() > i) ? PPSLocalTrack_x[i] : 0;
        PPSLocalTrack[i].y	= (PPSLocalTrack_y.size() > i) ? PPSLocalTrack_y[i] : 0;
      }
  }

  void fillPhotons()
  {
    size_t nobj_ = 0;
    if ( Photon_charge.size() > nobj_ ) nobj_ = Photon_charge.size();
    if ( Photon_cleanmask.size() > nobj_ ) nobj_ = Photon_cleanmask.size();
    if ( Photon_cutBased.size() > nobj_ ) nobj_ = Photon_cutBased.size();
    if ( Photon_cutBased_Fall17V1Bitmap.size() > nobj_ ) nobj_ = Photon_cutBased_Fall17V1Bitmap.size();
    if ( Photon_dEscaleDown.size() > nobj_ ) nobj_ = Photon_dEscaleDown.size();
    if ( Photon_dEscaleUp.size() > nobj_ ) nobj_ = Photon_dEscaleUp.size();
    if ( Photon_dEsigmaDown.size() > nobj_ ) nobj_ = Photon_dEsigmaDown.size();
    if ( Photon_dEsigmaUp.size() > nobj_ ) nobj_ = Photon_dEsigmaUp.size();
    if ( Photon_eCorr.size() > nobj_ ) nobj_ = Photon_eCorr.size();
    if ( Photon_electronIdx.size() > nobj_ ) nobj_ = Photon_electronIdx.size();
    if ( Photon_electronVeto.size() > nobj_ ) nobj_ = Photon_electronVeto.size();
    if ( Photon_energyErr.size() > nobj_ ) nobj_ = Photon_energyErr.size();
    if ( Photon_eta.size() > nobj_ ) nobj_ = Photon_eta.size();
    if ( Photon_genPartFlav.size() > nobj_ ) nobj_ = Photon_genPartFlav.size();
    if ( Photon_genPartIdx.size() > nobj_ ) nobj_ = Photon_genPartIdx.size();
    if ( Photon_hoe.size() > nobj_ ) nobj_ = Photon_hoe.size();
    if ( Photon_isScEtaEB.size() > nobj_ ) nobj_ = Photon_isScEtaEB.size();
    if ( Photon_isScEtaEE.size() > nobj_ ) nobj_ = Photon_isScEtaEE.size();
    if ( Photon_jetIdx.size() > nobj_ ) nobj_ = Photon_jetIdx.size();
    if ( Photon_mass.size() > nobj_ ) nobj_ = Photon_mass.size();
    if ( Photon_mvaID.size() > nobj_ ) nobj_ = Photon_mvaID.size();
    if ( Photon_mvaID_Fall17V1p1.size() > nobj_ ) nobj_ = Photon_mvaID_Fall17V1p1.size();
    if ( Photon_mvaID_WP80.size() > nobj_ ) nobj_ = Photon_mvaID_WP80.size();
    if ( Photon_mvaID_WP90.size() > nobj_ ) nobj_ = Photon_mvaID_WP90.size();
    if ( Photon_pdgId.size() > nobj_ ) nobj_ = Photon_pdgId.size();
    if ( Photon_pfRelIso03_all.size() > nobj_ ) nobj_ = Photon_pfRelIso03_all.size();
    if ( Photon_pfRelIso03_chg.size() > nobj_ ) nobj_ = Photon_pfRelIso03_chg.size();
    if ( Photon_phi.size() > nobj_ ) nobj_ = Photon_phi.size();
    if ( Photon_pixelSeed.size() > nobj_ ) nobj_ = Photon_pixelSeed.size();
    if ( Photon_pt.size() > nobj_ ) nobj_ = Photon_pt.size();
    if ( Photon_r9.size() > nobj_ ) nobj_ = Photon_r9.size();
    if ( Photon_seedGain.size() > nobj_ ) nobj_ = Photon_seedGain.size();
    if ( Photon_sieie.size() > nobj_ ) nobj_ = Photon_sieie.size();
    if ( Photon_vidNestedWPBitmap.size() > nobj_ ) nobj_ = Photon_vidNestedWPBitmap.size();
    Photon.resize(nobj_);
    for(unsigned int i=0; i < Photon.size(); ++i)
      {
        Photon[i].charge	= (Photon_charge.size() > i) ? Photon_charge[i] : 0;
        Photon[i].cleanmask	= (Photon_cleanmask.size() > i) ? Photon_cleanmask[i] : 0;
        Photon[i].cutBased	= (Photon_cutBased.size() > i) ? Photon_cutBased[i] : 0;
        Photon[i].cutBased_Fall17V1Bitmap	= (Photon_cutBased_Fall17V1Bitmap.size() > i) ? Photon_cutBased_Fall17V1Bitmap[i] : 0;
        Photon[i].dEscaleDown	= (Photon_dEscaleDown.size() > i) ? Photon_dEscaleDown[i] : 0;
        Photon[i].dEscaleUp	= (Photon_dEscaleUp.size() > i) ? Photon_dEscaleUp[i] : 0;
        Photon[i].dEsigmaDown	= (Photon_dEsigmaDown.size() > i) ? Photon_dEsigmaDown[i] : 0;
        Photon[i].dEsigmaUp	= (Photon_dEsigmaUp.size() > i) ? Photon_dEsigmaUp[i] : 0;
        Photon[i].eCorr	= (Photon_eCorr.size() > i) ? Photon_eCorr[i] : 0;
        Photon[i].electronIdx	= (Photon_electronIdx.size() > i) ? Photon_electronIdx[i] : 0;
        Photon[i].electronVeto	= (Photon_electronVeto.size() > i) ? (bool)Photon_electronVeto[i] : 0;
        Photon[i].energyErr	= (Photon_energyErr.size() > i) ? Photon_energyErr[i] : 0;
        Photon[i].eta	= (Photon_eta.size() > i) ? Photon_eta[i] : 0;
        Photon[i].genPartFlav	= (Photon_genPartFlav.size() > i) ? Photon_genPartFlav[i] : 0;
        Photon[i].genPartIdx	= (Photon_genPartIdx.size() > i) ? Photon_genPartIdx[i] : 0;
        Photon[i].hoe	= (Photon_hoe.size() > i) ? Photon_hoe[i] : 0;
        Photon[i].isScEtaEB	= (Photon_isScEtaEB.size() > i) ? (bool)Photon_isScEtaEB[i] : 0;
        Photon[i].isScEtaEE	= (Photon_isScEtaEE.size() > i) ? (bool)Photon_isScEtaEE[i] : 0;
        Photon[i].jetIdx	= (Photon_jetIdx.size() > i) ? Photon_jetIdx[i] : 0;
        Photon[i].mass	= (Photon_mass.size() > i) ? Photon_mass[i] : 0;
        Photon[i].mvaID	= (Photon_mvaID.size() > i) ? Photon_mvaID[i] : 0;
        Photon[i].mvaID_Fall17V1p1	= (Photon_mvaID_Fall17V1p1.size() > i) ? Photon_mvaID_Fall17V1p1[i] : 0;
        Photon[i].mvaID_WP80	= (Photon_mvaID_WP80.size() > i) ? (bool)Photon_mvaID_WP80[i] : 0;
        Photon[i].mvaID_WP90	= (Photon_mvaID_WP90.size() > i) ? (bool)Photon_mvaID_WP90[i] : 0;
        Photon[i].pdgId	= (Photon_pdgId.size() > i) ? Photon_pdgId[i] : 0;
        Photon[i].pfRelIso03_all	= (Photon_pfRelIso03_all.size() > i) ? Photon_pfRelIso03_all[i] : 0;
        Photon[i].pfRelIso03_chg	= (Photon_pfRelIso03_chg.size() > i) ? Photon_pfRelIso03_chg[i] : 0;
        Photon[i].phi	= (Photon_phi.size() > i) ? Photon_phi[i] : 0;
        Photon[i].pixelSeed	= (Photon_pixelSeed.size() > i) ? (bool)Photon_pixelSeed[i] : 0;
        Photon[i].pt	= (Photon_pt.size() > i) ? Photon_pt[i] : 0;
        Photon[i].r9	= (Photon_r9.size() > i) ? Photon_r9[i] : 0;
        Photon[i].seedGain	= (Photon_seedGain.size() > i) ? Photon_seedGain[i] : 0;
        Photon[i].sieie	= (Photon_sieie.size() > i) ? Photon_sieie[i] : 0;
        Photon[i].vidNestedWPBitmap	= (Photon_vidNestedWPBitmap.size() > i) ? Photon_vidNestedWPBitmap[i] : 0;
      }
  }

  void fillProtons()
  {
    size_t nobj_ = 0;
    if ( Proton_multiRP_arm.size() > nobj_ ) nobj_ = Proton_multiRP_arm.size();
    if ( Proton_multiRP_t.size() > nobj_ ) nobj_ = Proton_multiRP_t.size();
    if ( Proton_multiRP_thetaX.size() > nobj_ ) nobj_ = Proton_multiRP_thetaX.size();
    if ( Proton_multiRP_thetaY.size() > nobj_ ) nobj_ = Proton_multiRP_thetaY.size();
    if ( Proton_multiRP_time.size() > nobj_ ) nobj_ = Proton_multiRP_time.size();
    if ( Proton_multiRP_timeUnc.size() > nobj_ ) nobj_ = Proton_multiRP_timeUnc.size();
    if ( Proton_multiRP_xi.size() > nobj_ ) nobj_ = Proton_multiRP_xi.size();
    Proton.resize(nobj_);
    for(unsigned int i=0; i < Proton.size(); ++i)
      {
        Proton[i].multiRP_arm	= (Proton_multiRP_arm.size() > i) ? Proton_multiRP_arm[i] : 0;
        Proton[i].multiRP_t	= (Proton_multiRP_t.size() > i) ? Proton_multiRP_t[i] : 0;
        Proton[i].multiRP_thetaX	= (Proton_multiRP_thetaX.size() > i) ? Proton_multiRP_thetaX[i] : 0;
        Proton[i].multiRP_thetaY	= (Proton_multiRP_thetaY.size() > i) ? Proton_multiRP_thetaY[i] : 0;
        Proton[i].multiRP_time	= (Proton_multiRP_time.size() > i) ? Proton_multiRP_time[i] : 0;
        Proton[i].multiRP_timeUnc	= (Proton_multiRP_timeUnc.size() > i) ? Proton_multiRP_timeUnc[i] : 0;
        Proton[i].multiRP_xi	= (Proton_multiRP_xi.size() > i) ? Proton_multiRP_xi[i] : 0;
      }
  }

  void fillSVs()
  {
    size_t nobj_ = 0;
    if ( SV_charge.size() > nobj_ ) nobj_ = SV_charge.size();
    if ( SV_chi2.size() > nobj_ ) nobj_ = SV_chi2.size();
    if ( SV_dlen.size() > nobj_ ) nobj_ = SV_dlen.size();
    if ( SV_dlenSig.size() > nobj_ ) nobj_ = SV_dlenSig.size();
    if ( SV_dxy.size() > nobj_ ) nobj_ = SV_dxy.size();
    if ( SV_dxySig.size() > nobj_ ) nobj_ = SV_dxySig.size();
    if ( SV_eta.size() > nobj_ ) nobj_ = SV_eta.size();
    if ( SV_mass.size() > nobj_ ) nobj_ = SV_mass.size();
    if ( SV_ndof.size() > nobj_ ) nobj_ = SV_ndof.size();
    if ( SV_ntracks.size() > nobj_ ) nobj_ = SV_ntracks.size();
    if ( SV_pAngle.size() > nobj_ ) nobj_ = SV_pAngle.size();
    if ( SV_phi.size() > nobj_ ) nobj_ = SV_phi.size();
    if ( SV_pt.size() > nobj_ ) nobj_ = SV_pt.size();
    if ( SV_x.size() > nobj_ ) nobj_ = SV_x.size();
    if ( SV_y.size() > nobj_ ) nobj_ = SV_y.size();
    if ( SV_z.size() > nobj_ ) nobj_ = SV_z.size();
    SV.resize(nobj_);
    for(unsigned int i=0; i < SV.size(); ++i)
      {
        SV[i].charge	= (SV_charge.size() > i) ? SV_charge[i] : 0;
        SV[i].chi2	= (SV_chi2.size() > i) ? SV_chi2[i] : 0;
        SV[i].dlen	= (SV_dlen.size() > i) ? SV_dlen[i] : 0;
        SV[i].dlenSig	= (SV_dlenSig.size() > i) ? SV_dlenSig[i] : 0;
        SV[i].dxy	= (SV_dxy.size() > i) ? SV_dxy[i] : 0;
        SV[i].dxySig	= (SV_dxySig.size() > i) ? SV_dxySig[i] : 0;
        SV[i].eta	= (SV_eta.size() > i) ? SV_eta[i] : 0;
        SV[i].mass	= (SV_mass.size() > i) ? SV_mass[i] : 0;
        SV[i].ndof	= (SV_ndof.size() > i) ? SV_ndof[i] : 0;
        SV[i].ntracks	= (SV_ntracks.size() > i) ? SV_ntracks[i] : 0;
        SV[i].pAngle	= (SV_pAngle.size() > i) ? SV_pAngle[i] : 0;
        SV[i].phi	= (SV_phi.size() > i) ? SV_phi[i] : 0;
        SV[i].pt	= (SV_pt.size() > i) ? SV_pt[i] : 0;
        SV[i].x	= (SV_x.size() > i) ? SV_x[i] : 0;
        SV[i].y	= (SV_y.size() > i) ? SV_y[i] : 0;
        SV[i].z	= (SV_z.size() > i) ? SV_z[i] : 0;
      }
  }

  void fillSoftActivityJets()
  {
    size_t nobj_ = 0;
    if ( SoftActivityJet_eta.size() > nobj_ ) nobj_ = SoftActivityJet_eta.size();
    if ( SoftActivityJet_phi.size() > nobj_ ) nobj_ = SoftActivityJet_phi.size();
    if ( SoftActivityJet_pt.size() > nobj_ ) nobj_ = SoftActivityJet_pt.size();
    SoftActivityJet.resize(nobj_);
    for(unsigned int i=0; i < SoftActivityJet.size(); ++i)
      {
        SoftActivityJet[i].eta	= (SoftActivityJet_eta.size() > i) ? SoftActivityJet_eta[i] : 0;
        SoftActivityJet[i].phi	= (SoftActivityJet_phi.size() > i) ? SoftActivityJet_phi[i] : 0;
        SoftActivityJet[i].pt	= (SoftActivityJet_pt.size() > i) ? SoftActivityJet_pt[i] : 0;
      }
  }

  void fillSubGenJetAK8s()
  {
    size_t nobj_ = 0;
    if ( SubGenJetAK8_eta.size() > nobj_ ) nobj_ = SubGenJetAK8_eta.size();
    if ( SubGenJetAK8_mass.size() > nobj_ ) nobj_ = SubGenJetAK8_mass.size();
    if ( SubGenJetAK8_phi.size() > nobj_ ) nobj_ = SubGenJetAK8_phi.size();
    if ( SubGenJetAK8_pt.size() > nobj_ ) nobj_ = SubGenJetAK8_pt.size();
    SubGenJetAK8.resize(nobj_);
    for(unsigned int i=0; i < SubGenJetAK8.size(); ++i)
      {
        SubGenJetAK8[i].eta	= (SubGenJetAK8_eta.size() > i) ? SubGenJetAK8_eta[i] : 0;
        SubGenJetAK8[i].mass	= (SubGenJetAK8_mass.size() > i) ? SubGenJetAK8_mass[i] : 0;
        SubGenJetAK8[i].phi	= (SubGenJetAK8_phi.size() > i) ? SubGenJetAK8_phi[i] : 0;
        SubGenJetAK8[i].pt	= (SubGenJetAK8_pt.size() > i) ? SubGenJetAK8_pt[i] : 0;
      }
  }

  void fillSubJets()
  {
    size_t nobj_ = 0;
    if ( SubJet_btagCSVV2.size() > nobj_ ) nobj_ = SubJet_btagCSVV2.size();
    if ( SubJet_btagDeepB.size() > nobj_ ) nobj_ = SubJet_btagDeepB.size();
    if ( SubJet_eta.size() > nobj_ ) nobj_ = SubJet_eta.size();
    if ( SubJet_hadronFlavour.size() > nobj_ ) nobj_ = SubJet_hadronFlavour.size();
    if ( SubJet_mass.size() > nobj_ ) nobj_ = SubJet_mass.size();
    if ( SubJet_n2b1.size() > nobj_ ) nobj_ = SubJet_n2b1.size();
    if ( SubJet_n3b1.size() > nobj_ ) nobj_ = SubJet_n3b1.size();
    if ( SubJet_nBHadrons.size() > nobj_ ) nobj_ = SubJet_nBHadrons.size();
    if ( SubJet_nCHadrons.size() > nobj_ ) nobj_ = SubJet_nCHadrons.size();
    if ( SubJet_phi.size() > nobj_ ) nobj_ = SubJet_phi.size();
    if ( SubJet_pt.size() > nobj_ ) nobj_ = SubJet_pt.size();
    if ( SubJet_rawFactor.size() > nobj_ ) nobj_ = SubJet_rawFactor.size();
    if ( SubJet_tau1.size() > nobj_ ) nobj_ = SubJet_tau1.size();
    if ( SubJet_tau2.size() > nobj_ ) nobj_ = SubJet_tau2.size();
    if ( SubJet_tau3.size() > nobj_ ) nobj_ = SubJet_tau3.size();
    if ( SubJet_tau4.size() > nobj_ ) nobj_ = SubJet_tau4.size();
    SubJet.resize(nobj_);
    for(unsigned int i=0; i < SubJet.size(); ++i)
      {
        SubJet[i].btagCSVV2	= (SubJet_btagCSVV2.size() > i) ? SubJet_btagCSVV2[i] : 0;
        SubJet[i].btagDeepB	= (SubJet_btagDeepB.size() > i) ? SubJet_btagDeepB[i] : 0;
        SubJet[i].eta	= (SubJet_eta.size() > i) ? SubJet_eta[i] : 0;
        SubJet[i].hadronFlavour	= (SubJet_hadronFlavour.size() > i) ? SubJet_hadronFlavour[i] : 0;
        SubJet[i].mass	= (SubJet_mass.size() > i) ? SubJet_mass[i] : 0;
        SubJet[i].n2b1	= (SubJet_n2b1.size() > i) ? SubJet_n2b1[i] : 0;
        SubJet[i].n3b1	= (SubJet_n3b1.size() > i) ? SubJet_n3b1[i] : 0;
        SubJet[i].nBHadrons	= (SubJet_nBHadrons.size() > i) ? SubJet_nBHadrons[i] : 0;
        SubJet[i].nCHadrons	= (SubJet_nCHadrons.size() > i) ? SubJet_nCHadrons[i] : 0;
        SubJet[i].phi	= (SubJet_phi.size() > i) ? SubJet_phi[i] : 0;
        SubJet[i].pt	= (SubJet_pt.size() > i) ? SubJet_pt[i] : 0;
        SubJet[i].rawFactor	= (SubJet_rawFactor.size() > i) ? SubJet_rawFactor[i] : 0;
        SubJet[i].tau1	= (SubJet_tau1.size() > i) ? SubJet_tau1[i] : 0;
        SubJet[i].tau2	= (SubJet_tau2.size() > i) ? SubJet_tau2[i] : 0;
        SubJet[i].tau3	= (SubJet_tau3.size() > i) ? SubJet_tau3[i] : 0;
        SubJet[i].tau4	= (SubJet_tau4.size() > i) ? SubJet_tau4[i] : 0;
      }
  }

  void fillTaus()
  {
    size_t nobj_ = 0;
    if ( Tau_charge.size() > nobj_ ) nobj_ = Tau_charge.size();
    if ( Tau_chargedIso.size() > nobj_ ) nobj_ = Tau_chargedIso.size();
    if ( Tau_cleanmask.size() > nobj_ ) nobj_ = Tau_cleanmask.size();
    if ( Tau_decayMode.size() > nobj_ ) nobj_ = Tau_decayMode.size();
    if ( Tau_dxy.size() > nobj_ ) nobj_ = Tau_dxy.size();
    if ( Tau_dz.size() > nobj_ ) nobj_ = Tau_dz.size();
    if ( Tau_eta.size() > nobj_ ) nobj_ = Tau_eta.size();
    if ( Tau_genPartFlav.size() > nobj_ ) nobj_ = Tau_genPartFlav.size();
    if ( Tau_genPartIdx.size() > nobj_ ) nobj_ = Tau_genPartIdx.size();
    if ( Tau_idAntiEleDeadECal.size() > nobj_ ) nobj_ = Tau_idAntiEleDeadECal.size();
    if ( Tau_idAntiMu.size() > nobj_ ) nobj_ = Tau_idAntiMu.size();
    if ( Tau_idDecayModeOldDMs.size() > nobj_ ) nobj_ = Tau_idDecayModeOldDMs.size();
    if ( Tau_idDeepTau2017v2p1VSe.size() > nobj_ ) nobj_ = Tau_idDeepTau2017v2p1VSe.size();
    if ( Tau_idDeepTau2017v2p1VSjet.size() > nobj_ ) nobj_ = Tau_idDeepTau2017v2p1VSjet.size();
    if ( Tau_idDeepTau2017v2p1VSmu.size() > nobj_ ) nobj_ = Tau_idDeepTau2017v2p1VSmu.size();
    if ( Tau_jetIdx.size() > nobj_ ) nobj_ = Tau_jetIdx.size();
    if ( Tau_leadTkDeltaEta.size() > nobj_ ) nobj_ = Tau_leadTkDeltaEta.size();
    if ( Tau_leadTkDeltaPhi.size() > nobj_ ) nobj_ = Tau_leadTkDeltaPhi.size();
    if ( Tau_leadTkPtOverTauPt.size() > nobj_ ) nobj_ = Tau_leadTkPtOverTauPt.size();
    if ( Tau_mass.size() > nobj_ ) nobj_ = Tau_mass.size();
    if ( Tau_neutralIso.size() > nobj_ ) nobj_ = Tau_neutralIso.size();
    if ( Tau_phi.size() > nobj_ ) nobj_ = Tau_phi.size();
    if ( Tau_photonsOutsideSignalCone.size() > nobj_ ) nobj_ = Tau_photonsOutsideSignalCone.size();
    if ( Tau_pt.size() > nobj_ ) nobj_ = Tau_pt.size();
    if ( Tau_puCorr.size() > nobj_ ) nobj_ = Tau_puCorr.size();
    if ( Tau_rawDeepTau2017v2p1VSe.size() > nobj_ ) nobj_ = Tau_rawDeepTau2017v2p1VSe.size();
    if ( Tau_rawDeepTau2017v2p1VSjet.size() > nobj_ ) nobj_ = Tau_rawDeepTau2017v2p1VSjet.size();
    if ( Tau_rawDeepTau2017v2p1VSmu.size() > nobj_ ) nobj_ = Tau_rawDeepTau2017v2p1VSmu.size();
    if ( Tau_rawIso.size() > nobj_ ) nobj_ = Tau_rawIso.size();
    if ( Tau_rawIsodR03.size() > nobj_ ) nobj_ = Tau_rawIsodR03.size();
    Tau.resize(nobj_);
    for(unsigned int i=0; i < Tau.size(); ++i)
      {
        Tau[i].charge	= (Tau_charge.size() > i) ? Tau_charge[i] : 0;
        Tau[i].chargedIso	= (Tau_chargedIso.size() > i) ? Tau_chargedIso[i] : 0;
        Tau[i].cleanmask	= (Tau_cleanmask.size() > i) ? Tau_cleanmask[i] : 0;
        Tau[i].decayMode	= (Tau_decayMode.size() > i) ? Tau_decayMode[i] : 0;
        Tau[i].dxy	= (Tau_dxy.size() > i) ? Tau_dxy[i] : 0;
        Tau[i].dz	= (Tau_dz.size() > i) ? Tau_dz[i] : 0;
        Tau[i].eta	= (Tau_eta.size() > i) ? Tau_eta[i] : 0;
        Tau[i].genPartFlav	= (Tau_genPartFlav.size() > i) ? Tau_genPartFlav[i] : 0;
        Tau[i].genPartIdx	= (Tau_genPartIdx.size() > i) ? Tau_genPartIdx[i] : 0;
        Tau[i].idAntiEleDeadECal	= (Tau_idAntiEleDeadECal.size() > i) ? (bool)Tau_idAntiEleDeadECal[i] : 0;
        Tau[i].idAntiMu	= (Tau_idAntiMu.size() > i) ? Tau_idAntiMu[i] : 0;
        Tau[i].idDecayModeOldDMs	= (Tau_idDecayModeOldDMs.size() > i) ? (bool)Tau_idDecayModeOldDMs[i] : 0;
        Tau[i].idDeepTau2017v2p1VSe	= (Tau_idDeepTau2017v2p1VSe.size() > i) ? Tau_idDeepTau2017v2p1VSe[i] : 0;
        Tau[i].idDeepTau2017v2p1VSjet	= (Tau_idDeepTau2017v2p1VSjet.size() > i) ? Tau_idDeepTau2017v2p1VSjet[i] : 0;
        Tau[i].idDeepTau2017v2p1VSmu	= (Tau_idDeepTau2017v2p1VSmu.size() > i) ? Tau_idDeepTau2017v2p1VSmu[i] : 0;
        Tau[i].jetIdx	= (Tau_jetIdx.size() > i) ? Tau_jetIdx[i] : 0;
        Tau[i].leadTkDeltaEta	= (Tau_leadTkDeltaEta.size() > i) ? Tau_leadTkDeltaEta[i] : 0;
        Tau[i].leadTkDeltaPhi	= (Tau_leadTkDeltaPhi.size() > i) ? Tau_leadTkDeltaPhi[i] : 0;
        Tau[i].leadTkPtOverTauPt	= (Tau_leadTkPtOverTauPt.size() > i) ? Tau_leadTkPtOverTauPt[i] : 0;
        Tau[i].mass	= (Tau_mass.size() > i) ? Tau_mass[i] : 0;
        Tau[i].neutralIso	= (Tau_neutralIso.size() > i) ? Tau_neutralIso[i] : 0;
        Tau[i].phi	= (Tau_phi.size() > i) ? Tau_phi[i] : 0;
        Tau[i].photonsOutsideSignalCone	= (Tau_photonsOutsideSignalCone.size() > i) ? Tau_photonsOutsideSignalCone[i] : 0;
        Tau[i].pt	= (Tau_pt.size() > i) ? Tau_pt[i] : 0;
        Tau[i].puCorr	= (Tau_puCorr.size() > i) ? Tau_puCorr[i] : 0;
        Tau[i].rawDeepTau2017v2p1VSe	= (Tau_rawDeepTau2017v2p1VSe.size() > i) ? Tau_rawDeepTau2017v2p1VSe[i] : 0;
        Tau[i].rawDeepTau2017v2p1VSjet	= (Tau_rawDeepTau2017v2p1VSjet.size() > i) ? Tau_rawDeepTau2017v2p1VSjet[i] : 0;
        Tau[i].rawDeepTau2017v2p1VSmu	= (Tau_rawDeepTau2017v2p1VSmu.size() > i) ? Tau_rawDeepTau2017v2p1VSmu[i] : 0;
        Tau[i].rawIso	= (Tau_rawIso.size() > i) ? Tau_rawIso[i] : 0;
        Tau[i].rawIsodR03	= (Tau_rawIsodR03.size() > i) ? Tau_rawIsodR03[i] : 0;
      }
  }

  void fillTrigObjs()
  {
    size_t nobj_ = 0;
    if ( TrigObj_eta.size() > nobj_ ) nobj_ = TrigObj_eta.size();
    if ( TrigObj_filterBits.size() > nobj_ ) nobj_ = TrigObj_filterBits.size();
    if ( TrigObj_id.size() > nobj_ ) nobj_ = TrigObj_id.size();
    if ( TrigObj_l1charge.size() > nobj_ ) nobj_ = TrigObj_l1charge.size();
    if ( TrigObj_l1iso.size() > nobj_ ) nobj_ = TrigObj_l1iso.size();
    if ( TrigObj_l1pt.size() > nobj_ ) nobj_ = TrigObj_l1pt.size();
    if ( TrigObj_l1pt_2.size() > nobj_ ) nobj_ = TrigObj_l1pt_2.size();
    if ( TrigObj_l2pt.size() > nobj_ ) nobj_ = TrigObj_l2pt.size();
    if ( TrigObj_phi.size() > nobj_ ) nobj_ = TrigObj_phi.size();
    if ( TrigObj_pt.size() > nobj_ ) nobj_ = TrigObj_pt.size();
    TrigObj.resize(nobj_);
    for(unsigned int i=0; i < TrigObj.size(); ++i)
      {
        TrigObj[i].eta	= (TrigObj_eta.size() > i) ? TrigObj_eta[i] : 0;
        TrigObj[i].filterBits	= (TrigObj_filterBits.size() > i) ? TrigObj_filterBits[i] : 0;
        TrigObj[i].id	= (TrigObj_id.size() > i) ? TrigObj_id[i] : 0;
        TrigObj[i].l1charge	= (TrigObj_l1charge.size() > i) ? TrigObj_l1charge[i] : 0;
        TrigObj[i].l1iso	= (TrigObj_l1iso.size() > i) ? TrigObj_l1iso[i] : 0;
        TrigObj[i].l1pt	= (TrigObj_l1pt.size() > i) ? TrigObj_l1pt[i] : 0;
        TrigObj[i].l1pt_2	= (TrigObj_l1pt_2.size() > i) ? TrigObj_l1pt_2[i] : 0;
        TrigObj[i].l2pt	= (TrigObj_l2pt.size() > i) ? TrigObj_l2pt[i] : 0;
        TrigObj[i].phi	= (TrigObj_phi.size() > i) ? TrigObj_phi[i] : 0;
        TrigObj[i].pt	= (TrigObj_pt.size() > i) ? TrigObj_pt[i] : 0;
      }
  }

  void fillboostedTaus()
  {
    size_t nobj_ = 0;
    if ( boostedTau_charge.size() > nobj_ ) nobj_ = boostedTau_charge.size();
    if ( boostedTau_chargedIso.size() > nobj_ ) nobj_ = boostedTau_chargedIso.size();
    if ( boostedTau_decayMode.size() > nobj_ ) nobj_ = boostedTau_decayMode.size();
    if ( boostedTau_eta.size() > nobj_ ) nobj_ = boostedTau_eta.size();
    if ( boostedTau_genPartFlav.size() > nobj_ ) nobj_ = boostedTau_genPartFlav.size();
    if ( boostedTau_genPartIdx.size() > nobj_ ) nobj_ = boostedTau_genPartIdx.size();
    if ( boostedTau_idAntiEle2018.size() > nobj_ ) nobj_ = boostedTau_idAntiEle2018.size();
    if ( boostedTau_idAntiMu.size() > nobj_ ) nobj_ = boostedTau_idAntiMu.size();
    if ( boostedTau_idMVAnewDM2017v2.size() > nobj_ ) nobj_ = boostedTau_idMVAnewDM2017v2.size();
    if ( boostedTau_idMVAoldDM2017v2.size() > nobj_ ) nobj_ = boostedTau_idMVAoldDM2017v2.size();
    if ( boostedTau_idMVAoldDMdR032017v2.size() > nobj_ ) nobj_ = boostedTau_idMVAoldDMdR032017v2.size();
    if ( boostedTau_jetIdx.size() > nobj_ ) nobj_ = boostedTau_jetIdx.size();
    if ( boostedTau_leadTkDeltaEta.size() > nobj_ ) nobj_ = boostedTau_leadTkDeltaEta.size();
    if ( boostedTau_leadTkDeltaPhi.size() > nobj_ ) nobj_ = boostedTau_leadTkDeltaPhi.size();
    if ( boostedTau_leadTkPtOverTauPt.size() > nobj_ ) nobj_ = boostedTau_leadTkPtOverTauPt.size();
    if ( boostedTau_mass.size() > nobj_ ) nobj_ = boostedTau_mass.size();
    if ( boostedTau_neutralIso.size() > nobj_ ) nobj_ = boostedTau_neutralIso.size();
    if ( boostedTau_phi.size() > nobj_ ) nobj_ = boostedTau_phi.size();
    if ( boostedTau_photonsOutsideSignalCone.size() > nobj_ ) nobj_ = boostedTau_photonsOutsideSignalCone.size();
    if ( boostedTau_pt.size() > nobj_ ) nobj_ = boostedTau_pt.size();
    if ( boostedTau_puCorr.size() > nobj_ ) nobj_ = boostedTau_puCorr.size();
    if ( boostedTau_rawAntiEle2018.size() > nobj_ ) nobj_ = boostedTau_rawAntiEle2018.size();
    if ( boostedTau_rawAntiEleCat2018.size() > nobj_ ) nobj_ = boostedTau_rawAntiEleCat2018.size();
    if ( boostedTau_rawIso.size() > nobj_ ) nobj_ = boostedTau_rawIso.size();
    if ( boostedTau_rawIsodR03.size() > nobj_ ) nobj_ = boostedTau_rawIsodR03.size();
    if ( boostedTau_rawMVAnewDM2017v2.size() > nobj_ ) nobj_ = boostedTau_rawMVAnewDM2017v2.size();
    if ( boostedTau_rawMVAoldDM2017v2.size() > nobj_ ) nobj_ = boostedTau_rawMVAoldDM2017v2.size();
    if ( boostedTau_rawMVAoldDMdR032017v2.size() > nobj_ ) nobj_ = boostedTau_rawMVAoldDMdR032017v2.size();
    boostedTau.resize(nobj_);
    for(unsigned int i=0; i < boostedTau.size(); ++i)
      {
        boostedTau[i].charge	= (boostedTau_charge.size() > i) ? boostedTau_charge[i] : 0;
        boostedTau[i].chargedIso	= (boostedTau_chargedIso.size() > i) ? boostedTau_chargedIso[i] : 0;
        boostedTau[i].decayMode	= (boostedTau_decayMode.size() > i) ? boostedTau_decayMode[i] : 0;
        boostedTau[i].eta	= (boostedTau_eta.size() > i) ? boostedTau_eta[i] : 0;
        boostedTau[i].genPartFlav	= (boostedTau_genPartFlav.size() > i) ? boostedTau_genPartFlav[i] : 0;
        boostedTau[i].genPartIdx	= (boostedTau_genPartIdx.size() > i) ? boostedTau_genPartIdx[i] : 0;
        boostedTau[i].idAntiEle2018	= (boostedTau_idAntiEle2018.size() > i) ? boostedTau_idAntiEle2018[i] : 0;
        boostedTau[i].idAntiMu	= (boostedTau_idAntiMu.size() > i) ? boostedTau_idAntiMu[i] : 0;
        boostedTau[i].idMVAnewDM2017v2	= (boostedTau_idMVAnewDM2017v2.size() > i) ? boostedTau_idMVAnewDM2017v2[i] : 0;
        boostedTau[i].idMVAoldDM2017v2	= (boostedTau_idMVAoldDM2017v2.size() > i) ? boostedTau_idMVAoldDM2017v2[i] : 0;
        boostedTau[i].idMVAoldDMdR032017v2	= (boostedTau_idMVAoldDMdR032017v2.size() > i) ? boostedTau_idMVAoldDMdR032017v2[i] : 0;
        boostedTau[i].jetIdx	= (boostedTau_jetIdx.size() > i) ? boostedTau_jetIdx[i] : 0;
        boostedTau[i].leadTkDeltaEta	= (boostedTau_leadTkDeltaEta.size() > i) ? boostedTau_leadTkDeltaEta[i] : 0;
        boostedTau[i].leadTkDeltaPhi	= (boostedTau_leadTkDeltaPhi.size() > i) ? boostedTau_leadTkDeltaPhi[i] : 0;
        boostedTau[i].leadTkPtOverTauPt	= (boostedTau_leadTkPtOverTauPt.size() > i) ? boostedTau_leadTkPtOverTauPt[i] : 0;
        boostedTau[i].mass	= (boostedTau_mass.size() > i) ? boostedTau_mass[i] : 0;
        boostedTau[i].neutralIso	= (boostedTau_neutralIso.size() > i) ? boostedTau_neutralIso[i] : 0;
        boostedTau[i].phi	= (boostedTau_phi.size() > i) ? boostedTau_phi[i] : 0;
        boostedTau[i].photonsOutsideSignalCone	= (boostedTau_photonsOutsideSignalCone.size() > i) ? boostedTau_photonsOutsideSignalCone[i] : 0;
        boostedTau[i].pt	= (boostedTau_pt.size() > i) ? boostedTau_pt[i] : 0;
        boostedTau[i].puCorr	= (boostedTau_puCorr.size() > i) ? boostedTau_puCorr[i] : 0;
        boostedTau[i].rawAntiEle2018	= (boostedTau_rawAntiEle2018.size() > i) ? boostedTau_rawAntiEle2018[i] : 0;
        boostedTau[i].rawAntiEleCat2018	= (boostedTau_rawAntiEleCat2018.size() > i) ? boostedTau_rawAntiEleCat2018[i] : 0;
        boostedTau[i].rawIso	= (boostedTau_rawIso.size() > i) ? boostedTau_rawIso[i] : 0;
        boostedTau[i].rawIsodR03	= (boostedTau_rawIsodR03.size() > i) ? boostedTau_rawIsodR03[i] : 0;
        boostedTau[i].rawMVAnewDM2017v2	= (boostedTau_rawMVAnewDM2017v2.size() > i) ? boostedTau_rawMVAnewDM2017v2[i] : 0;
        boostedTau[i].rawMVAoldDM2017v2	= (boostedTau_rawMVAoldDM2017v2.size() > i) ? boostedTau_rawMVAoldDM2017v2[i] : 0;
        boostedTau[i].rawMVAoldDMdR032017v2	= (boostedTau_rawMVAoldDMdR032017v2.size() > i) ? boostedTau_rawMVAoldDMdR032017v2[i] : 0;
      }
  }


  std::vector<eventBuffer::CorrT1METJet_s> CorrT1METJet;
  std::vector<eventBuffer::Electron_s> Electron;
  std::vector<eventBuffer::FatJet_s> FatJet;
  std::vector<eventBuffer::FsrPhoton_s> FsrPhoton;
  std::vector<eventBuffer::GenDressedLepton_s> GenDressedLepton;
  std::vector<eventBuffer::GenIsolatedPhoton_s> GenIsolatedPhoton;
  std::vector<eventBuffer::GenJet_s> GenJet;
  std::vector<eventBuffer::GenJetAK8_s> GenJetAK8;
  std::vector<eventBuffer::GenPart_s> GenPart;
  std::vector<eventBuffer::GenVisTau_s> GenVisTau;
  std::vector<eventBuffer::IsoTrack_s> IsoTrack;
  std::vector<eventBuffer::Jet_s> Jet;
  std::vector<eventBuffer::LHEPart_s> LHEPart;
  std::vector<eventBuffer::LowPtElectron_s> LowPtElectron;
  std::vector<eventBuffer::Muon_s> Muon;
  std::vector<eventBuffer::PPSLocalTrack_s> PPSLocalTrack;
  std::vector<eventBuffer::Photon_s> Photon;
  std::vector<eventBuffer::Proton_s> Proton;
  std::vector<eventBuffer::SV_s> SV;
  std::vector<eventBuffer::SoftActivityJet_s> SoftActivityJet;
  std::vector<eventBuffer::SubGenJetAK8_s> SubGenJetAK8;
  std::vector<eventBuffer::SubJet_s> SubJet;
  std::vector<eventBuffer::Tau_s> Tau;
  std::vector<eventBuffer::TrigObj_s> TrigObj;
  std::vector<eventBuffer::boostedTau_s> boostedTau;

  void fillObjects()
  {
    fillCorrT1METJets();
    fillElectrons();
    fillFatJets();
    fillFsrPhotons();
    fillGenDressedLeptons();
    fillGenIsolatedPhotons();
    fillGenJets();
    fillGenJetAK8s();
    fillGenParts();
    fillGenVisTaus();
    fillIsoTracks();
    fillJets();
    fillLHEParts();
    fillLowPtElectrons();
    fillMuons();
    fillPPSLocalTracks();
    fillPhotons();
    fillProtons();
    fillSVs();
    fillSoftActivityJets();
    fillSubGenJetAK8s();
    fillSubJets();
    fillTaus();
    fillTrigObjs();
    fillboostedTaus();
  }

   //--------------------------------------------------------------------------
  // Save objects for which the select function was called
  void saveObjects()
  {
    int n = 0;

    n = 0;
    try
      {
         n = indexmap["CorrT1METJet"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["CorrT1METJet"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            CorrT1METJet_area[i]	= CorrT1METJet_area[j];
            CorrT1METJet_eta[i]	= CorrT1METJet_eta[j];
            CorrT1METJet_muonSubtrFactor[i]	= CorrT1METJet_muonSubtrFactor[j];
            CorrT1METJet_phi[i]	= CorrT1METJet_phi[j];
            CorrT1METJet_rawPt[i]	= CorrT1METJet_rawPt[j];
          }
      }
    nCorrT1METJet = n;

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
            Electron_convVeto[i]	= Electron_convVeto[j];
            Electron_cutBased[i]	= Electron_cutBased[j];
            Electron_deltaEtaSC[i]	= Electron_deltaEtaSC[j];
            Electron_dxy[i]	= Electron_dxy[j];
            Electron_dz[i]	= Electron_dz[j];
            Electron_eta[i]	= Electron_eta[j];
            Electron_ip3d[i]	= Electron_ip3d[j];
            Electron_isEB[i]	= Electron_isEB[j];
            Electron_jetIdx[i]	= Electron_jetIdx[j];
            Electron_lostHits[i]	= Electron_lostHits[j];
            Electron_mass[i]	= Electron_mass[j];
            Electron_miniPFRelIso_all[i]	= Electron_miniPFRelIso_all[j];
            Electron_miniPFRelIso_chg[i]	= Electron_miniPFRelIso_chg[j];
            Electron_mvaIso[i]	= Electron_mvaIso[j];
            Electron_mvaIso_WP80[i]	= Electron_mvaIso_WP80[j];
            Electron_mvaIso_WP90[i]	= Electron_mvaIso_WP90[j];
            Electron_mvaNoIso[i]	= Electron_mvaNoIso[j];
            Electron_mvaNoIso_WP80[i]	= Electron_mvaNoIso_WP80[j];
            Electron_mvaNoIso_WP90[i]	= Electron_mvaNoIso_WP90[j];
            Electron_pdgId[i]	= Electron_pdgId[j];
            Electron_pfRelIso03_all[i]	= Electron_pfRelIso03_all[j];
            Electron_pfRelIso03_chg[i]	= Electron_pfRelIso03_chg[j];
            Electron_pfRelIso04_all[i]	= Electron_pfRelIso04_all[j];
            Electron_phi[i]	= Electron_phi[j];
            Electron_promptMVA[i]	= Electron_promptMVA[j];
            Electron_pt[i]	= Electron_pt[j];
            Electron_r9[i]	= Electron_r9[j];
            Electron_scEtOverPt[i]	= Electron_scEtOverPt[j];
            Electron_seedGain[i]	= Electron_seedGain[j];
            Electron_sip3d[i]	= Electron_sip3d[j];
            Electron_superclusterEta[i]	= Electron_superclusterEta[j];
            Electron_tightCharge[i]	= Electron_tightCharge[j];
            Electron_vidNestedWPBitmap[i]	= Electron_vidNestedWPBitmap[j];
            Electron_genPartFlav[i]	= Electron_genPartFlav[j];
            Electron_genPartIdx[i]	= Electron_genPartIdx[j];
            Electron_mvaIso_WPL[i]	= Electron_mvaIso_WPL[j];
            Electron_mvaNoIso_WPL[i]	= Electron_mvaNoIso_WPL[j];
            Electron_cleanmask[i]	= Electron_cleanmask[j];
            Electron_cutBased_HEEP[i]	= Electron_cutBased_HEEP[j];
            Electron_dEscaleDown[i]	= Electron_dEscaleDown[j];
            Electron_dEscaleUp[i]	= Electron_dEscaleUp[j];
            Electron_dEsigmaDown[i]	= Electron_dEsigmaDown[j];
            Electron_dEsigmaUp[i]	= Electron_dEsigmaUp[j];
            Electron_dr03EcalRecHitSumEt[i]	= Electron_dr03EcalRecHitSumEt[j];
            Electron_dr03HcalDepth1TowerSumEt[i]	= Electron_dr03HcalDepth1TowerSumEt[j];
            Electron_dr03TkSumPt[i]	= Electron_dr03TkSumPt[j];
            Electron_dr03TkSumPtHEEP[i]	= Electron_dr03TkSumPtHEEP[j];
            Electron_dxyErr[i]	= Electron_dxyErr[j];
            Electron_dzErr[i]	= Electron_dzErr[j];
            Electron_eCorr[i]	= Electron_eCorr[j];
            Electron_eInvMinusPInv[i]	= Electron_eInvMinusPInv[j];
            Electron_energyErr[i]	= Electron_energyErr[j];
            Electron_hoe[i]	= Electron_hoe[j];
            Electron_isPFcand[i]	= Electron_isPFcand[j];
            Electron_jetNDauCharged[i]	= Electron_jetNDauCharged[j];
            Electron_jetPtRelv2[i]	= Electron_jetPtRelv2[j];
            Electron_jetRelIso[i]	= Electron_jetRelIso[j];
            Electron_mvaFall17V2Iso[i]	= Electron_mvaFall17V2Iso[j];
            Electron_mvaFall17V2Iso_WP80[i]	= Electron_mvaFall17V2Iso_WP80[j];
            Electron_mvaFall17V2Iso_WP90[i]	= Electron_mvaFall17V2Iso_WP90[j];
            Electron_mvaFall17V2Iso_WPL[i]	= Electron_mvaFall17V2Iso_WPL[j];
            Electron_mvaFall17V2noIso[i]	= Electron_mvaFall17V2noIso[j];
            Electron_mvaFall17V2noIso_WP80[i]	= Electron_mvaFall17V2noIso_WP80[j];
            Electron_mvaFall17V2noIso_WP90[i]	= Electron_mvaFall17V2noIso_WP90[j];
            Electron_mvaFall17V2noIso_WPL[i]	= Electron_mvaFall17V2noIso_WPL[j];
            Electron_mvaTTH[i]	= Electron_mvaTTH[j];
            Electron_photonIdx[i]	= Electron_photonIdx[j];
            Electron_sieie[i]	= Electron_sieie[j];
            Electron_vidNestedWPBitmapHEEP[i]	= Electron_vidNestedWPBitmapHEEP[j];
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
            FatJet_btagCSVV2[i]	= FatJet_btagCSVV2[j];
            FatJet_btagDDBvLV2[i]	= FatJet_btagDDBvLV2[j];
            FatJet_btagDDCvBV2[i]	= FatJet_btagDDCvBV2[j];
            FatJet_btagDDCvLV2[i]	= FatJet_btagDDCvLV2[j];
            FatJet_btagDeepB[i]	= FatJet_btagDeepB[j];
            FatJet_btagHbb[i]	= FatJet_btagHbb[j];
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
            FatJet_electronIdx3SJ[i]	= FatJet_electronIdx3SJ[j];
            FatJet_eta[i]	= FatJet_eta[j];
            FatJet_genJetAK8Idx[i]	= FatJet_genJetAK8Idx[j];
            FatJet_hadronFlavour[i]	= FatJet_hadronFlavour[j];
            FatJet_jetId[i]	= FatJet_jetId[j];
            FatJet_lsf3[i]	= FatJet_lsf3[j];
            FatJet_mass[i]	= FatJet_mass[j];
            FatJet_msoftdrop[i]	= FatJet_msoftdrop[j];
            FatJet_muonIdx3SJ[i]	= FatJet_muonIdx3SJ[j];
            FatJet_n2b1[i]	= FatJet_n2b1[j];
            FatJet_n3b1[i]	= FatJet_n3b1[j];
            FatJet_nBHadrons[i]	= FatJet_nBHadrons[j];
            FatJet_nCHadrons[i]	= FatJet_nCHadrons[j];
            FatJet_nConstituents[i]	= FatJet_nConstituents[j];
            FatJet_particleNetMD_QCD[i]	= FatJet_particleNetMD_QCD[j];
            FatJet_particleNetMD_Xbb[i]	= FatJet_particleNetMD_Xbb[j];
            FatJet_particleNetMD_Xcc[i]	= FatJet_particleNetMD_Xcc[j];
            FatJet_particleNetMD_Xqq[i]	= FatJet_particleNetMD_Xqq[j];
            FatJet_particleNet_H4qvsQCD[i]	= FatJet_particleNet_H4qvsQCD[j];
            FatJet_particleNet_HbbvsQCD[i]	= FatJet_particleNet_HbbvsQCD[j];
            FatJet_particleNet_HccvsQCD[i]	= FatJet_particleNet_HccvsQCD[j];
            FatJet_particleNet_QCD[i]	= FatJet_particleNet_QCD[j];
            FatJet_particleNet_TvsQCD[i]	= FatJet_particleNet_TvsQCD[j];
            FatJet_particleNet_WvsQCD[i]	= FatJet_particleNet_WvsQCD[j];
            FatJet_particleNet_ZvsQCD[i]	= FatJet_particleNet_ZvsQCD[j];
            FatJet_particleNet_mass[i]	= FatJet_particleNet_mass[j];
            FatJet_phi[i]	= FatJet_phi[j];
            FatJet_pt[i]	= FatJet_pt[j];
            FatJet_rawFactor[i]	= FatJet_rawFactor[j];
            FatJet_subJetIdx1[i]	= FatJet_subJetIdx1[j];
            FatJet_subJetIdx2[i]	= FatJet_subJetIdx2[j];
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
         n = indexmap["FsrPhoton"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["FsrPhoton"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            FsrPhoton_dROverEt2[i]	= FsrPhoton_dROverEt2[j];
            FsrPhoton_eta[i]	= FsrPhoton_eta[j];
            FsrPhoton_muonIdx[i]	= FsrPhoton_muonIdx[j];
            FsrPhoton_phi[i]	= FsrPhoton_phi[j];
            FsrPhoton_pt[i]	= FsrPhoton_pt[j];
            FsrPhoton_relIso03[i]	= FsrPhoton_relIso03[j];
          }
      }
    nFsrPhoton = n;

    n = 0;
    try
      {
         n = indexmap["GenDressedLepton"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["GenDressedLepton"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            GenDressedLepton_eta[i]	= GenDressedLepton_eta[j];
            GenDressedLepton_hasTauAnc[i]	= GenDressedLepton_hasTauAnc[j];
            GenDressedLepton_mass[i]	= GenDressedLepton_mass[j];
            GenDressedLepton_pdgId[i]	= GenDressedLepton_pdgId[j];
            GenDressedLepton_phi[i]	= GenDressedLepton_phi[j];
            GenDressedLepton_pt[i]	= GenDressedLepton_pt[j];
          }
      }
    nGenDressedLepton = n;

    n = 0;
    try
      {
         n = indexmap["GenIsolatedPhoton"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["GenIsolatedPhoton"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            GenIsolatedPhoton_eta[i]	= GenIsolatedPhoton_eta[j];
            GenIsolatedPhoton_mass[i]	= GenIsolatedPhoton_mass[j];
            GenIsolatedPhoton_phi[i]	= GenIsolatedPhoton_phi[j];
            GenIsolatedPhoton_pt[i]	= GenIsolatedPhoton_pt[j];
          }
      }
    nGenIsolatedPhoton = n;

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
            GenJet_nBHadrons[i]	= GenJet_nBHadrons[j];
            GenJet_nCHadrons[i]	= GenJet_nCHadrons[j];
            GenJet_partonFlavour[i]	= GenJet_partonFlavour[j];
            GenJet_phi[i]	= GenJet_phi[j];
            GenJet_pt[i]	= GenJet_pt[j];
          }
      }
    nGenJet = n;

    n = 0;
    try
      {
         n = indexmap["GenJetAK8"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["GenJetAK8"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            GenJetAK8_eta[i]	= GenJetAK8_eta[j];
            GenJetAK8_hadronFlavour[i]	= GenJetAK8_hadronFlavour[j];
            GenJetAK8_mass[i]	= GenJetAK8_mass[j];
            GenJetAK8_partonFlavour[i]	= GenJetAK8_partonFlavour[j];
            GenJetAK8_phi[i]	= GenJetAK8_phi[j];
            GenJetAK8_pt[i]	= GenJetAK8_pt[j];
          }
      }
    nGenJetAK8 = n;

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
         n = indexmap["GenVisTau"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["GenVisTau"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            GenVisTau_charge[i]	= GenVisTau_charge[j];
            GenVisTau_eta[i]	= GenVisTau_eta[j];
            GenVisTau_genPartIdxMother[i]	= GenVisTau_genPartIdxMother[j];
            GenVisTau_mass[i]	= GenVisTau_mass[j];
            GenVisTau_phi[i]	= GenVisTau_phi[j];
            GenVisTau_pt[i]	= GenVisTau_pt[j];
            GenVisTau_status[i]	= GenVisTau_status[j];
          }
      }
    nGenVisTau = n;

    n = 0;
    try
      {
         n = indexmap["IsoTrack"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["IsoTrack"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            IsoTrack_charge[i]	= IsoTrack_charge[j];
            IsoTrack_dxy[i]	= IsoTrack_dxy[j];
            IsoTrack_dz[i]	= IsoTrack_dz[j];
            IsoTrack_eta[i]	= IsoTrack_eta[j];
            IsoTrack_fromPV[i]	= IsoTrack_fromPV[j];
            IsoTrack_isFromLostTrack[i]	= IsoTrack_isFromLostTrack[j];
            IsoTrack_isHighPurityTrack[i]	= IsoTrack_isHighPurityTrack[j];
            IsoTrack_isPFcand[i]	= IsoTrack_isPFcand[j];
            IsoTrack_miniPFRelIso_all[i]	= IsoTrack_miniPFRelIso_all[j];
            IsoTrack_miniPFRelIso_chg[i]	= IsoTrack_miniPFRelIso_chg[j];
            IsoTrack_pdgId[i]	= IsoTrack_pdgId[j];
            IsoTrack_pfRelIso03_all[i]	= IsoTrack_pfRelIso03_all[j];
            IsoTrack_pfRelIso03_chg[i]	= IsoTrack_pfRelIso03_chg[j];
            IsoTrack_phi[i]	= IsoTrack_phi[j];
            IsoTrack_pt[i]	= IsoTrack_pt[j];
          }
      }
    nIsoTrack = n;

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
            Jet_PNetRegPtRawCorr[i]	= Jet_PNetRegPtRawCorr[j];
            Jet_PNetRegPtRawCorrNeutrino[i]	= Jet_PNetRegPtRawCorrNeutrino[j];
            Jet_PNetRegPtRawRes[i]	= Jet_PNetRegPtRawRes[j];
            Jet_UParTAK4RegPtRawCorr[i]	= Jet_UParTAK4RegPtRawCorr[j];
            Jet_UParTAK4RegPtRawCorrNeutrino[i]	= Jet_UParTAK4RegPtRawCorrNeutrino[j];
            Jet_UParTAK4RegPtRawRes[i]	= Jet_UParTAK4RegPtRawRes[j];
            Jet_UParTAK4V1RegPtRawCorr[i]	= Jet_UParTAK4V1RegPtRawCorr[j];
            Jet_UParTAK4V1RegPtRawCorrNeutrino[i]	= Jet_UParTAK4V1RegPtRawCorrNeutrino[j];
            Jet_UParTAK4V1RegPtRawRes[i]	= Jet_UParTAK4V1RegPtRawRes[j];
            Jet_area[i]	= Jet_area[j];
            Jet_btagDeepFlavB[i]	= Jet_btagDeepFlavB[j];
            Jet_btagDeepFlavCvB[i]	= Jet_btagDeepFlavCvB[j];
            Jet_btagDeepFlavCvL[i]	= Jet_btagDeepFlavCvL[j];
            Jet_btagDeepFlavQG[i]	= Jet_btagDeepFlavQG[j];
            Jet_btagPNetB[i]	= Jet_btagPNetB[j];
            Jet_btagPNetCvB[i]	= Jet_btagPNetCvB[j];
            Jet_btagPNetCvL[i]	= Jet_btagPNetCvL[j];
            Jet_btagPNetQvG[i]	= Jet_btagPNetQvG[j];
            Jet_btagUParTAK4B[i]	= Jet_btagUParTAK4B[j];
            Jet_btagUParTAK4CvB[i]	= Jet_btagUParTAK4CvB[j];
            Jet_btagUParTAK4CvL[i]	= Jet_btagUParTAK4CvL[j];
            Jet_btagUParTAK4QvG[i]	= Jet_btagUParTAK4QvG[j];
            Jet_chEmEF[i]	= Jet_chEmEF[j];
            Jet_chHEF[i]	= Jet_chHEF[j];
            Jet_chMultiplicity[i]	= Jet_chMultiplicity[j];
            Jet_electronIdx1[i]	= Jet_electronIdx1[j];
            Jet_electronIdx2[i]	= Jet_electronIdx2[j];
            Jet_eta[i]	= Jet_eta[j];
            Jet_hfEmEF[i]	= Jet_hfEmEF[j];
            Jet_hfHEF[i]	= Jet_hfHEF[j];
            Jet_mass[i]	= Jet_mass[j];
            Jet_muEF[i]	= Jet_muEF[j];
            Jet_muonIdx1[i]	= Jet_muonIdx1[j];
            Jet_muonIdx2[i]	= Jet_muonIdx2[j];
            Jet_muonSubtrFactor[i]	= Jet_muonSubtrFactor[j];
            Jet_nConstituents[i]	= Jet_nConstituents[j];
            Jet_nElectrons[i]	= Jet_nElectrons[j];
            Jet_nMuons[i]	= Jet_nMuons[j];
            Jet_neEmEF[i]	= Jet_neEmEF[j];
            Jet_neHEF[i]	= Jet_neHEF[j];
            Jet_neMultiplicity[i]	= Jet_neMultiplicity[j];
            Jet_phi[i]	= Jet_phi[j];
            Jet_pt[i]	= Jet_pt[j];
            Jet_puIdDisc[i]	= Jet_puIdDisc[j];
            Jet_rawFactor[i]	= Jet_rawFactor[j];
            Jet_genJetIdx[i]	= Jet_genJetIdx[j];
            Jet_hadronFlavour[i]	= Jet_hadronFlavour[j];
            Jet_partonFlavour[i]	= Jet_partonFlavour[j];
            Jet_bRegCorr[i]	= Jet_bRegCorr[j];
            Jet_bRegRes[i]	= Jet_bRegRes[j];
            Jet_btagCSVV2[i]	= Jet_btagCSVV2[j];
            Jet_btagDeepB[i]	= Jet_btagDeepB[j];
            Jet_btagDeepCvB[i]	= Jet_btagDeepCvB[j];
            Jet_btagDeepCvL[i]	= Jet_btagDeepCvL[j];
            Jet_cRegCorr[i]	= Jet_cRegCorr[j];
            Jet_cRegRes[i]	= Jet_cRegRes[j];
            Jet_chFPV0EF[i]	= Jet_chFPV0EF[j];
            Jet_cleanmask[i]	= Jet_cleanmask[j];
            Jet_hfadjacentEtaStripsSize[i]	= Jet_hfadjacentEtaStripsSize[j];
            Jet_hfcentralEtaStripSize[i]	= Jet_hfcentralEtaStripSize[j];
            Jet_hfsigmaEtaEta[i]	= Jet_hfsigmaEtaEta[j];
            Jet_hfsigmaPhiPhi[i]	= Jet_hfsigmaPhiPhi[j];
            Jet_jetId[i]	= Jet_jetId[j];
            Jet_puId[i]	= Jet_puId[j];
            Jet_qgl[i]	= Jet_qgl[j];
          }
      }
    nJet = n;

    n = 0;
    try
      {
         n = indexmap["LHEPart"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["LHEPart"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            LHEPart_eta[i]	= LHEPart_eta[j];
            LHEPart_incomingpz[i]	= LHEPart_incomingpz[j];
            LHEPart_mass[i]	= LHEPart_mass[j];
            LHEPart_pdgId[i]	= LHEPart_pdgId[j];
            LHEPart_phi[i]	= LHEPart_phi[j];
            LHEPart_pt[i]	= LHEPart_pt[j];
            LHEPart_spin[i]	= LHEPart_spin[j];
            LHEPart_status[i]	= LHEPart_status[j];
          }
      }
    nLHEPart = n;

    n = 0;
    try
      {
         n = indexmap["LowPtElectron"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["LowPtElectron"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            LowPtElectron_ID[i]	= LowPtElectron_ID[j];
            LowPtElectron_charge[i]	= LowPtElectron_charge[j];
            LowPtElectron_convVeto[i]	= LowPtElectron_convVeto[j];
            LowPtElectron_convVtxRadius[i]	= LowPtElectron_convVtxRadius[j];
            LowPtElectron_convWP[i]	= LowPtElectron_convWP[j];
            LowPtElectron_deltaEtaSC[i]	= LowPtElectron_deltaEtaSC[j];
            LowPtElectron_dxy[i]	= LowPtElectron_dxy[j];
            LowPtElectron_dxyErr[i]	= LowPtElectron_dxyErr[j];
            LowPtElectron_dz[i]	= LowPtElectron_dz[j];
            LowPtElectron_dzErr[i]	= LowPtElectron_dzErr[j];
            LowPtElectron_eInvMinusPInv[i]	= LowPtElectron_eInvMinusPInv[j];
            LowPtElectron_embeddedID[i]	= LowPtElectron_embeddedID[j];
            LowPtElectron_energyErr[i]	= LowPtElectron_energyErr[j];
            LowPtElectron_eta[i]	= LowPtElectron_eta[j];
            LowPtElectron_genPartFlav[i]	= LowPtElectron_genPartFlav[j];
            LowPtElectron_genPartIdx[i]	= LowPtElectron_genPartIdx[j];
            LowPtElectron_hoe[i]	= LowPtElectron_hoe[j];
            LowPtElectron_lostHits[i]	= LowPtElectron_lostHits[j];
            LowPtElectron_mass[i]	= LowPtElectron_mass[j];
            LowPtElectron_miniPFRelIso_all[i]	= LowPtElectron_miniPFRelIso_all[j];
            LowPtElectron_miniPFRelIso_chg[i]	= LowPtElectron_miniPFRelIso_chg[j];
            LowPtElectron_pdgId[i]	= LowPtElectron_pdgId[j];
            LowPtElectron_phi[i]	= LowPtElectron_phi[j];
            LowPtElectron_pt[i]	= LowPtElectron_pt[j];
            LowPtElectron_ptbiased[i]	= LowPtElectron_ptbiased[j];
            LowPtElectron_r9[i]	= LowPtElectron_r9[j];
            LowPtElectron_scEtOverPt[i]	= LowPtElectron_scEtOverPt[j];
            LowPtElectron_sieie[i]	= LowPtElectron_sieie[j];
            LowPtElectron_unbiased[i]	= LowPtElectron_unbiased[j];
          }
      }
    nLowPtElectron = n;

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
            Muon_highPtId[i]	= Muon_highPtId[j];
            Muon_ip3d[i]	= Muon_ip3d[j];
            Muon_isGlobal[i]	= Muon_isGlobal[j];
            Muon_isPFcand[i]	= Muon_isPFcand[j];
            Muon_isTracker[i]	= Muon_isTracker[j];
            Muon_jetIdx[i]	= Muon_jetIdx[j];
            Muon_looseId[i]	= Muon_looseId[j];
            Muon_mass[i]	= Muon_mass[j];
            Muon_mediumId[i]	= Muon_mediumId[j];
            Muon_mediumPromptId[i]	= Muon_mediumPromptId[j];
            Muon_miniIsoId[i]	= Muon_miniIsoId[j];
            Muon_miniPFRelIso_all[i]	= Muon_miniPFRelIso_all[j];
            Muon_miniPFRelIso_chg[i]	= Muon_miniPFRelIso_chg[j];
            Muon_mvaMuID[i]	= Muon_mvaMuID[j];
            Muon_mvaMuID_WP[i]	= Muon_mvaMuID_WP[j];
            Muon_nStations[i]	= Muon_nStations[j];
            Muon_nTrackerLayers[i]	= Muon_nTrackerLayers[j];
            Muon_pdgId[i]	= Muon_pdgId[j];
            Muon_pfIsoId[i]	= Muon_pfIsoId[j];
            Muon_pfRelIso03_all[i]	= Muon_pfRelIso03_all[j];
            Muon_pfRelIso03_chg[i]	= Muon_pfRelIso03_chg[j];
            Muon_pfRelIso04_all[i]	= Muon_pfRelIso04_all[j];
            Muon_phi[i]	= Muon_phi[j];
            Muon_promptMVA[i]	= Muon_promptMVA[j];
            Muon_pt[i]	= Muon_pt[j];
            Muon_ptErr[i]	= Muon_ptErr[j];
            Muon_sip3d[i]	= Muon_sip3d[j];
            Muon_tightCharge[i]	= Muon_tightCharge[j];
            Muon_tightId[i]	= Muon_tightId[j];
            Muon_tkIsoId[i]	= Muon_tkIsoId[j];
            Muon_tkRelIso[i]	= Muon_tkRelIso[j];
            Muon_genPartFlav[i]	= Muon_genPartFlav[j];
            Muon_genPartIdx[i]	= Muon_genPartIdx[j];
            Muon_cleanmask[i]	= Muon_cleanmask[j];
            Muon_dxyErr[i]	= Muon_dxyErr[j];
            Muon_dxybs[i]	= Muon_dxybs[j];
            Muon_dzErr[i]	= Muon_dzErr[j];
            Muon_fsrPhotonIdx[i]	= Muon_fsrPhotonIdx[j];
            Muon_highPurity[i]	= Muon_highPurity[j];
            Muon_inTimeMuon[i]	= Muon_inTimeMuon[j];
            Muon_isStandalone[i]	= Muon_isStandalone[j];
            Muon_jetNDauCharged[i]	= Muon_jetNDauCharged[j];
            Muon_jetPtRelv2[i]	= Muon_jetPtRelv2[j];
            Muon_jetRelIso[i]	= Muon_jetRelIso[j];
            Muon_multiIsoId[i]	= Muon_multiIsoId[j];
            Muon_mvaId[i]	= Muon_mvaId[j];
            Muon_mvaLowPt[i]	= Muon_mvaLowPt[j];
            Muon_mvaLowPtId[i]	= Muon_mvaLowPtId[j];
            Muon_mvaTTH[i]	= Muon_mvaTTH[j];
            Muon_puppiIsoId[i]	= Muon_puppiIsoId[j];
            Muon_segmentComp[i]	= Muon_segmentComp[j];
            Muon_softId[i]	= Muon_softId[j];
            Muon_softMva[i]	= Muon_softMva[j];
            Muon_softMvaId[i]	= Muon_softMvaId[j];
            Muon_triggerIdLoose[i]	= Muon_triggerIdLoose[j];
            Muon_tunepRelPt[i]	= Muon_tunepRelPt[j];
          }
      }
    nMuon = n;

    n = 0;
    try
      {
         n = indexmap["PPSLocalTrack"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["PPSLocalTrack"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            PPSLocalTrack_decRPId[i]	= PPSLocalTrack_decRPId[j];
            PPSLocalTrack_multiRPProtonIdx[i]	= PPSLocalTrack_multiRPProtonIdx[j];
            PPSLocalTrack_rpType[i]	= PPSLocalTrack_rpType[j];
            PPSLocalTrack_singleRPProtonIdx[i]	= PPSLocalTrack_singleRPProtonIdx[j];
            PPSLocalTrack_time[i]	= PPSLocalTrack_time[j];
            PPSLocalTrack_timeUnc[i]	= PPSLocalTrack_timeUnc[j];
            PPSLocalTrack_x[i]	= PPSLocalTrack_x[j];
            PPSLocalTrack_y[i]	= PPSLocalTrack_y[j];
          }
      }
    nPPSLocalTrack = n;

    n = 0;
    try
      {
         n = indexmap["Photon"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["Photon"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            Photon_charge[i]	= Photon_charge[j];
            Photon_cleanmask[i]	= Photon_cleanmask[j];
            Photon_cutBased[i]	= Photon_cutBased[j];
            Photon_cutBased_Fall17V1Bitmap[i]	= Photon_cutBased_Fall17V1Bitmap[j];
            Photon_dEscaleDown[i]	= Photon_dEscaleDown[j];
            Photon_dEscaleUp[i]	= Photon_dEscaleUp[j];
            Photon_dEsigmaDown[i]	= Photon_dEsigmaDown[j];
            Photon_dEsigmaUp[i]	= Photon_dEsigmaUp[j];
            Photon_eCorr[i]	= Photon_eCorr[j];
            Photon_electronIdx[i]	= Photon_electronIdx[j];
            Photon_electronVeto[i]	= Photon_electronVeto[j];
            Photon_energyErr[i]	= Photon_energyErr[j];
            Photon_eta[i]	= Photon_eta[j];
            Photon_genPartFlav[i]	= Photon_genPartFlav[j];
            Photon_genPartIdx[i]	= Photon_genPartIdx[j];
            Photon_hoe[i]	= Photon_hoe[j];
            Photon_isScEtaEB[i]	= Photon_isScEtaEB[j];
            Photon_isScEtaEE[i]	= Photon_isScEtaEE[j];
            Photon_jetIdx[i]	= Photon_jetIdx[j];
            Photon_mass[i]	= Photon_mass[j];
            Photon_mvaID[i]	= Photon_mvaID[j];
            Photon_mvaID_Fall17V1p1[i]	= Photon_mvaID_Fall17V1p1[j];
            Photon_mvaID_WP80[i]	= Photon_mvaID_WP80[j];
            Photon_mvaID_WP90[i]	= Photon_mvaID_WP90[j];
            Photon_pdgId[i]	= Photon_pdgId[j];
            Photon_pfRelIso03_all[i]	= Photon_pfRelIso03_all[j];
            Photon_pfRelIso03_chg[i]	= Photon_pfRelIso03_chg[j];
            Photon_phi[i]	= Photon_phi[j];
            Photon_pixelSeed[i]	= Photon_pixelSeed[j];
            Photon_pt[i]	= Photon_pt[j];
            Photon_r9[i]	= Photon_r9[j];
            Photon_seedGain[i]	= Photon_seedGain[j];
            Photon_sieie[i]	= Photon_sieie[j];
            Photon_vidNestedWPBitmap[i]	= Photon_vidNestedWPBitmap[j];
          }
      }
    nPhoton = n;

    n = 0;
    try
      {
         n = indexmap["Proton"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["Proton"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            Proton_multiRP_arm[i]	= Proton_multiRP_arm[j];
            Proton_multiRP_t[i]	= Proton_multiRP_t[j];
            Proton_multiRP_thetaX[i]	= Proton_multiRP_thetaX[j];
            Proton_multiRP_thetaY[i]	= Proton_multiRP_thetaY[j];
            Proton_multiRP_time[i]	= Proton_multiRP_time[j];
            Proton_multiRP_timeUnc[i]	= Proton_multiRP_timeUnc[j];
            Proton_multiRP_xi[i]	= Proton_multiRP_xi[j];
          }
      }
    nProton_multiRP = n;

    n = 0;
    try
      {
         n = indexmap["SV"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["SV"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            SV_charge[i]	= SV_charge[j];
            SV_chi2[i]	= SV_chi2[j];
            SV_dlen[i]	= SV_dlen[j];
            SV_dlenSig[i]	= SV_dlenSig[j];
            SV_dxy[i]	= SV_dxy[j];
            SV_dxySig[i]	= SV_dxySig[j];
            SV_eta[i]	= SV_eta[j];
            SV_mass[i]	= SV_mass[j];
            SV_ndof[i]	= SV_ndof[j];
            SV_ntracks[i]	= SV_ntracks[j];
            SV_pAngle[i]	= SV_pAngle[j];
            SV_phi[i]	= SV_phi[j];
            SV_pt[i]	= SV_pt[j];
            SV_x[i]	= SV_x[j];
            SV_y[i]	= SV_y[j];
            SV_z[i]	= SV_z[j];
          }
      }
    nSV = n;

    n = 0;
    try
      {
         n = indexmap["SoftActivityJet"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["SoftActivityJet"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            SoftActivityJet_eta[i]	= SoftActivityJet_eta[j];
            SoftActivityJet_phi[i]	= SoftActivityJet_phi[j];
            SoftActivityJet_pt[i]	= SoftActivityJet_pt[j];
          }
      }
    nSoftActivityJet = n;

    n = 0;
    try
      {
         n = indexmap["SubGenJetAK8"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["SubGenJetAK8"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            SubGenJetAK8_eta[i]	= SubGenJetAK8_eta[j];
            SubGenJetAK8_mass[i]	= SubGenJetAK8_mass[j];
            SubGenJetAK8_phi[i]	= SubGenJetAK8_phi[j];
            SubGenJetAK8_pt[i]	= SubGenJetAK8_pt[j];
          }
      }
    nSubGenJetAK8 = n;

    n = 0;
    try
      {
         n = indexmap["SubJet"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["SubJet"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            SubJet_btagCSVV2[i]	= SubJet_btagCSVV2[j];
            SubJet_btagDeepB[i]	= SubJet_btagDeepB[j];
            SubJet_eta[i]	= SubJet_eta[j];
            SubJet_hadronFlavour[i]	= SubJet_hadronFlavour[j];
            SubJet_mass[i]	= SubJet_mass[j];
            SubJet_n2b1[i]	= SubJet_n2b1[j];
            SubJet_n3b1[i]	= SubJet_n3b1[j];
            SubJet_nBHadrons[i]	= SubJet_nBHadrons[j];
            SubJet_nCHadrons[i]	= SubJet_nCHadrons[j];
            SubJet_phi[i]	= SubJet_phi[j];
            SubJet_pt[i]	= SubJet_pt[j];
            SubJet_rawFactor[i]	= SubJet_rawFactor[j];
            SubJet_tau1[i]	= SubJet_tau1[j];
            SubJet_tau2[i]	= SubJet_tau2[j];
            SubJet_tau3[i]	= SubJet_tau3[j];
            SubJet_tau4[i]	= SubJet_tau4[j];
          }
      }
    nSubJet = n;

    n = 0;
    try
      {
         n = indexmap["Tau"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["Tau"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            Tau_charge[i]	= Tau_charge[j];
            Tau_chargedIso[i]	= Tau_chargedIso[j];
            Tau_cleanmask[i]	= Tau_cleanmask[j];
            Tau_decayMode[i]	= Tau_decayMode[j];
            Tau_dxy[i]	= Tau_dxy[j];
            Tau_dz[i]	= Tau_dz[j];
            Tau_eta[i]	= Tau_eta[j];
            Tau_genPartFlav[i]	= Tau_genPartFlav[j];
            Tau_genPartIdx[i]	= Tau_genPartIdx[j];
            Tau_idAntiEleDeadECal[i]	= Tau_idAntiEleDeadECal[j];
            Tau_idAntiMu[i]	= Tau_idAntiMu[j];
            Tau_idDecayModeOldDMs[i]	= Tau_idDecayModeOldDMs[j];
            Tau_idDeepTau2017v2p1VSe[i]	= Tau_idDeepTau2017v2p1VSe[j];
            Tau_idDeepTau2017v2p1VSjet[i]	= Tau_idDeepTau2017v2p1VSjet[j];
            Tau_idDeepTau2017v2p1VSmu[i]	= Tau_idDeepTau2017v2p1VSmu[j];
            Tau_jetIdx[i]	= Tau_jetIdx[j];
            Tau_leadTkDeltaEta[i]	= Tau_leadTkDeltaEta[j];
            Tau_leadTkDeltaPhi[i]	= Tau_leadTkDeltaPhi[j];
            Tau_leadTkPtOverTauPt[i]	= Tau_leadTkPtOverTauPt[j];
            Tau_mass[i]	= Tau_mass[j];
            Tau_neutralIso[i]	= Tau_neutralIso[j];
            Tau_phi[i]	= Tau_phi[j];
            Tau_photonsOutsideSignalCone[i]	= Tau_photonsOutsideSignalCone[j];
            Tau_pt[i]	= Tau_pt[j];
            Tau_puCorr[i]	= Tau_puCorr[j];
            Tau_rawDeepTau2017v2p1VSe[i]	= Tau_rawDeepTau2017v2p1VSe[j];
            Tau_rawDeepTau2017v2p1VSjet[i]	= Tau_rawDeepTau2017v2p1VSjet[j];
            Tau_rawDeepTau2017v2p1VSmu[i]	= Tau_rawDeepTau2017v2p1VSmu[j];
            Tau_rawIso[i]	= Tau_rawIso[j];
            Tau_rawIsodR03[i]	= Tau_rawIsodR03[j];
          }
      }
    nTau = n;

    n = 0;
    try
      {
         n = indexmap["TrigObj"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["TrigObj"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            TrigObj_eta[i]	= TrigObj_eta[j];
            TrigObj_filterBits[i]	= TrigObj_filterBits[j];
            TrigObj_id[i]	= TrigObj_id[j];
            TrigObj_l1charge[i]	= TrigObj_l1charge[j];
            TrigObj_l1iso[i]	= TrigObj_l1iso[j];
            TrigObj_l1pt[i]	= TrigObj_l1pt[j];
            TrigObj_l1pt_2[i]	= TrigObj_l1pt_2[j];
            TrigObj_l2pt[i]	= TrigObj_l2pt[j];
            TrigObj_phi[i]	= TrigObj_phi[j];
            TrigObj_pt[i]	= TrigObj_pt[j];
          }
      }
    nTrigObj = n;

    n = 0;
    try
      {
         n = indexmap["boostedTau"].size();
      }
    catch (...)
      {}
    if ( n > 0 )
      {
        std::vector<int>& index = indexmap["boostedTau"];
        for(int i=0; i < n; ++i)
          {
            int j = index[i];
            boostedTau_charge[i]	= boostedTau_charge[j];
            boostedTau_chargedIso[i]	= boostedTau_chargedIso[j];
            boostedTau_decayMode[i]	= boostedTau_decayMode[j];
            boostedTau_eta[i]	= boostedTau_eta[j];
            boostedTau_genPartFlav[i]	= boostedTau_genPartFlav[j];
            boostedTau_genPartIdx[i]	= boostedTau_genPartIdx[j];
            boostedTau_idAntiEle2018[i]	= boostedTau_idAntiEle2018[j];
            boostedTau_idAntiMu[i]	= boostedTau_idAntiMu[j];
            boostedTau_idMVAnewDM2017v2[i]	= boostedTau_idMVAnewDM2017v2[j];
            boostedTau_idMVAoldDM2017v2[i]	= boostedTau_idMVAoldDM2017v2[j];
            boostedTau_idMVAoldDMdR032017v2[i]	= boostedTau_idMVAoldDMdR032017v2[j];
            boostedTau_jetIdx[i]	= boostedTau_jetIdx[j];
            boostedTau_leadTkDeltaEta[i]	= boostedTau_leadTkDeltaEta[j];
            boostedTau_leadTkDeltaPhi[i]	= boostedTau_leadTkDeltaPhi[j];
            boostedTau_leadTkPtOverTauPt[i]	= boostedTau_leadTkPtOverTauPt[j];
            boostedTau_mass[i]	= boostedTau_mass[j];
            boostedTau_neutralIso[i]	= boostedTau_neutralIso[j];
            boostedTau_phi[i]	= boostedTau_phi[j];
            boostedTau_photonsOutsideSignalCone[i]	= boostedTau_photonsOutsideSignalCone[j];
            boostedTau_pt[i]	= boostedTau_pt[j];
            boostedTau_puCorr[i]	= boostedTau_puCorr[j];
            boostedTau_rawAntiEle2018[i]	= boostedTau_rawAntiEle2018[j];
            boostedTau_rawAntiEleCat2018[i]	= boostedTau_rawAntiEleCat2018[j];
            boostedTau_rawIso[i]	= boostedTau_rawIso[j];
            boostedTau_rawIsodR03[i]	= boostedTau_rawIsodR03[j];
            boostedTau_rawMVAnewDM2017v2[i]	= boostedTau_rawMVAnewDM2017v2[j];
            boostedTau_rawMVAoldDM2017v2[i]	= boostedTau_rawMVAoldDM2017v2[j];
            boostedTau_rawMVAoldDMdR032017v2[i]	= boostedTau_rawMVAoldDMdR032017v2[j];
          }
      }
    nboostedTau = n;
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
	    exit(1);
      }

    initBuffers();
    
    // default is to select all branches      
    bool DEFAULT = varlist == "";
    choose["Events/CaloMET_phi"]	= DEFAULT;
    choose["Events/CaloMET_pt"]	= DEFAULT;
    choose["Events/CaloMET_sumEt"]	= DEFAULT;
    choose["Events/ChsMET_phi"]	= DEFAULT;
    choose["Events/ChsMET_pt"]	= DEFAULT;
    choose["Events/ChsMET_sumEt"]	= DEFAULT;
    choose["Events/CorrT1METJet_area"]	= DEFAULT;
    choose["Events/CorrT1METJet_eta"]	= DEFAULT;
    choose["Events/CorrT1METJet_muonSubtrFactor"]	= DEFAULT;
    choose["Events/CorrT1METJet_phi"]	= DEFAULT;
    choose["Events/CorrT1METJet_rawPt"]	= DEFAULT;
    choose["Events/DeepMETResolutionTune_phi"]	= DEFAULT;
    choose["Events/DeepMETResolutionTune_pt"]	= DEFAULT;
    choose["Events/DeepMETResponseTune_phi"]	= DEFAULT;
    choose["Events/DeepMETResponseTune_pt"]	= DEFAULT;
    choose["Events/Electron_charge"]	= DEFAULT;
    choose["Events/Electron_cleanmask"]	= DEFAULT;
    choose["Events/Electron_convVeto"]	= DEFAULT;
    choose["Events/Electron_cutBased"]	= DEFAULT;
    choose["Events/Electron_cutBased_HEEP"]	= DEFAULT;
    choose["Events/Electron_dEscaleDown"]	= DEFAULT;
    choose["Events/Electron_dEscaleUp"]	= DEFAULT;
    choose["Events/Electron_dEsigmaDown"]	= DEFAULT;
    choose["Events/Electron_dEsigmaUp"]	= DEFAULT;
    choose["Events/Electron_deltaEtaSC"]	= DEFAULT;
    choose["Events/Electron_dr03EcalRecHitSumEt"]	= DEFAULT;
    choose["Events/Electron_dr03HcalDepth1TowerSumEt"]	= DEFAULT;
    choose["Events/Electron_dr03TkSumPt"]	= DEFAULT;
    choose["Events/Electron_dr03TkSumPtHEEP"]	= DEFAULT;
    choose["Events/Electron_dxy"]	= DEFAULT;
    choose["Events/Electron_dxyErr"]	= DEFAULT;
    choose["Events/Electron_dz"]	= DEFAULT;
    choose["Events/Electron_dzErr"]	= DEFAULT;
    choose["Events/Electron_eCorr"]	= DEFAULT;
    choose["Events/Electron_eInvMinusPInv"]	= DEFAULT;
    choose["Events/Electron_energyErr"]	= DEFAULT;
    choose["Events/Electron_eta"]	= DEFAULT;
    choose["Events/Electron_genPartFlav"]	= DEFAULT;
    choose["Events/Electron_genPartIdx"]	= DEFAULT;
    choose["Events/Electron_hoe"]	= DEFAULT;
    choose["Events/Electron_ip3d"]	= DEFAULT;
    choose["Events/Electron_isEB"]	= DEFAULT;
    choose["Events/Electron_isPFcand"]	= DEFAULT;
    choose["Events/Electron_jetIdx"]	= DEFAULT;
    choose["Events/Electron_jetNDauCharged"]	= DEFAULT;
    choose["Events/Electron_jetPtRelv2"]	= DEFAULT;
    choose["Events/Electron_jetRelIso"]	= DEFAULT;
    choose["Events/Electron_lostHits"]	= DEFAULT;
    choose["Events/Electron_mass"]	= DEFAULT;
    choose["Events/Electron_miniPFRelIso_all"]	= DEFAULT;
    choose["Events/Electron_miniPFRelIso_chg"]	= DEFAULT;
    choose["Events/Electron_mvaFall17V2Iso"]	= DEFAULT;
    choose["Events/Electron_mvaFall17V2Iso_WP80"]	= DEFAULT;
    choose["Events/Electron_mvaFall17V2Iso_WP90"]	= DEFAULT;
    choose["Events/Electron_mvaFall17V2Iso_WPL"]	= DEFAULT;
    choose["Events/Electron_mvaFall17V2noIso"]	= DEFAULT;
    choose["Events/Electron_mvaFall17V2noIso_WP80"]	= DEFAULT;
    choose["Events/Electron_mvaFall17V2noIso_WP90"]	= DEFAULT;
    choose["Events/Electron_mvaFall17V2noIso_WPL"]	= DEFAULT;
    choose["Events/Electron_mvaIso"]	= DEFAULT;
    choose["Events/Electron_mvaIso_WP80"]	= DEFAULT;
    choose["Events/Electron_mvaIso_WP90"]	= DEFAULT;
    choose["Events/Electron_mvaIso_WPL"]	= DEFAULT;
    choose["Events/Electron_mvaNoIso"]	= DEFAULT;
    choose["Events/Electron_mvaNoIso_WP80"]	= DEFAULT;
    choose["Events/Electron_mvaNoIso_WP90"]	= DEFAULT;
    choose["Events/Electron_mvaNoIso_WPL"]	= DEFAULT;
    choose["Events/Electron_mvaTTH"]	= DEFAULT;
    choose["Events/Electron_pdgId"]	= DEFAULT;
    choose["Events/Electron_pfRelIso03_all"]	= DEFAULT;
    choose["Events/Electron_pfRelIso03_chg"]	= DEFAULT;
    choose["Events/Electron_pfRelIso04_all"]	= DEFAULT;
    choose["Events/Electron_phi"]	= DEFAULT;
    choose["Events/Electron_photonIdx"]	= DEFAULT;
    choose["Events/Electron_promptMVA"]	= DEFAULT;
    choose["Events/Electron_pt"]	= DEFAULT;
    choose["Events/Electron_r9"]	= DEFAULT;
    choose["Events/Electron_scEtOverPt"]	= DEFAULT;
    choose["Events/Electron_seedGain"]	= DEFAULT;
    choose["Events/Electron_sieie"]	= DEFAULT;
    choose["Events/Electron_sip3d"]	= DEFAULT;
    choose["Events/Electron_superclusterEta"]	= DEFAULT;
    choose["Events/Electron_tightCharge"]	= DEFAULT;
    choose["Events/Electron_vidNestedWPBitmap"]	= DEFAULT;
    choose["Events/Electron_vidNestedWPBitmapHEEP"]	= DEFAULT;
    choose["Events/FatJet_area"]	= DEFAULT;
    choose["Events/FatJet_btagCSVV2"]	= DEFAULT;
    choose["Events/FatJet_btagDDBvLV2"]	= DEFAULT;
    choose["Events/FatJet_btagDDCvBV2"]	= DEFAULT;
    choose["Events/FatJet_btagDDCvLV2"]	= DEFAULT;
    choose["Events/FatJet_btagDeepB"]	= DEFAULT;
    choose["Events/FatJet_btagHbb"]	= DEFAULT;
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
    choose["Events/FatJet_electronIdx3SJ"]	= DEFAULT;
    choose["Events/FatJet_eta"]	= DEFAULT;
    choose["Events/FatJet_genJetAK8Idx"]	= DEFAULT;
    choose["Events/FatJet_hadronFlavour"]	= DEFAULT;
    choose["Events/FatJet_jetId"]	= DEFAULT;
    choose["Events/FatJet_lsf3"]	= DEFAULT;
    choose["Events/FatJet_mass"]	= DEFAULT;
    choose["Events/FatJet_msoftdrop"]	= DEFAULT;
    choose["Events/FatJet_muonIdx3SJ"]	= DEFAULT;
    choose["Events/FatJet_n2b1"]	= DEFAULT;
    choose["Events/FatJet_n3b1"]	= DEFAULT;
    choose["Events/FatJet_nBHadrons"]	= DEFAULT;
    choose["Events/FatJet_nCHadrons"]	= DEFAULT;
    choose["Events/FatJet_nConstituents"]	= DEFAULT;
    choose["Events/FatJet_particleNetMD_QCD"]	= DEFAULT;
    choose["Events/FatJet_particleNetMD_Xbb"]	= DEFAULT;
    choose["Events/FatJet_particleNetMD_Xcc"]	= DEFAULT;
    choose["Events/FatJet_particleNetMD_Xqq"]	= DEFAULT;
    choose["Events/FatJet_particleNet_H4qvsQCD"]	= DEFAULT;
    choose["Events/FatJet_particleNet_HbbvsQCD"]	= DEFAULT;
    choose["Events/FatJet_particleNet_HccvsQCD"]	= DEFAULT;
    choose["Events/FatJet_particleNet_QCD"]	= DEFAULT;
    choose["Events/FatJet_particleNet_TvsQCD"]	= DEFAULT;
    choose["Events/FatJet_particleNet_WvsQCD"]	= DEFAULT;
    choose["Events/FatJet_particleNet_ZvsQCD"]	= DEFAULT;
    choose["Events/FatJet_particleNet_mass"]	= DEFAULT;
    choose["Events/FatJet_phi"]	= DEFAULT;
    choose["Events/FatJet_pt"]	= DEFAULT;
    choose["Events/FatJet_rawFactor"]	= DEFAULT;
    choose["Events/FatJet_subJetIdx1"]	= DEFAULT;
    choose["Events/FatJet_subJetIdx2"]	= DEFAULT;
    choose["Events/FatJet_tau1"]	= DEFAULT;
    choose["Events/FatJet_tau2"]	= DEFAULT;
    choose["Events/FatJet_tau3"]	= DEFAULT;
    choose["Events/FatJet_tau4"]	= DEFAULT;
    choose["Events/Flag_BadChargedCandidateFilter"]	= DEFAULT;
    choose["Events/Flag_BadChargedCandidateFilter_pRECO"]	= DEFAULT;
    choose["Events/Flag_BadChargedCandidateSummer16Filter"]	= DEFAULT;
    choose["Events/Flag_BadChargedCandidateSummer16Filter_pRECO"]	= DEFAULT;
    choose["Events/Flag_BadPFMuonDzFilter"]	= DEFAULT;
    choose["Events/Flag_BadPFMuonDzFilter_pRECO"]	= DEFAULT;
    choose["Events/Flag_BadPFMuonFilter"]	= DEFAULT;
    choose["Events/Flag_BadPFMuonFilter_pRECO"]	= DEFAULT;
    choose["Events/Flag_BadPFMuonSummer16Filter"]	= DEFAULT;
    choose["Events/Flag_BadPFMuonSummer16Filter_pRECO"]	= DEFAULT;
    choose["Events/Flag_CSCTightHalo2015Filter"]	= DEFAULT;
    choose["Events/Flag_CSCTightHalo2015Filter_pRECO"]	= DEFAULT;
    choose["Events/Flag_CSCTightHaloFilter"]	= DEFAULT;
    choose["Events/Flag_CSCTightHaloFilter_pRECO"]	= DEFAULT;
    choose["Events/Flag_CSCTightHaloTrkMuUnvetoFilter"]	= DEFAULT;
    choose["Events/Flag_CSCTightHaloTrkMuUnvetoFilter_pRECO"]	= DEFAULT;
    choose["Events/Flag_EcalDeadCellBoundaryEnergyFilter"]	= DEFAULT;
    choose["Events/Flag_EcalDeadCellBoundaryEnergyFilter_pRECO"]	= DEFAULT;
    choose["Events/Flag_EcalDeadCellTriggerPrimitiveFilter"]	= DEFAULT;
    choose["Events/Flag_EcalDeadCellTriggerPrimitiveFilter_pRECO"]	= DEFAULT;
    choose["Events/Flag_HBHENoiseFilter"]	= DEFAULT;
    choose["Events/Flag_HBHENoiseFilter_pRECO"]	= DEFAULT;
    choose["Events/Flag_HBHENoiseIsoFilter"]	= DEFAULT;
    choose["Events/Flag_HBHENoiseIsoFilter_pRECO"]	= DEFAULT;
    choose["Events/Flag_HcalStripHaloFilter"]	= DEFAULT;
    choose["Events/Flag_HcalStripHaloFilter_pRECO"]	= DEFAULT;
    choose["Events/Flag_METFilters"]	= DEFAULT;
    choose["Events/Flag_METFilters_pRECO"]	= DEFAULT;
    choose["Events/Flag_chargedHadronTrackResolutionFilter"]	= DEFAULT;
    choose["Events/Flag_chargedHadronTrackResolutionFilter_pRECO"]	= DEFAULT;
    choose["Events/Flag_ecalBadCalibFilter"]	= DEFAULT;
    choose["Events/Flag_ecalBadCalibFilter_pRECO"]	= DEFAULT;
    choose["Events/Flag_ecalLaserCorrFilter"]	= DEFAULT;
    choose["Events/Flag_ecalLaserCorrFilter_pRECO"]	= DEFAULT;
    choose["Events/Flag_eeBadScFilter"]	= DEFAULT;
    choose["Events/Flag_eeBadScFilter_pRECO"]	= DEFAULT;
    choose["Events/Flag_globalSuperTightHalo2016Filter"]	= DEFAULT;
    choose["Events/Flag_globalSuperTightHalo2016Filter_pRECO"]	= DEFAULT;
    choose["Events/Flag_globalTightHalo2016Filter"]	= DEFAULT;
    choose["Events/Flag_globalTightHalo2016Filter_pRECO"]	= DEFAULT;
    choose["Events/Flag_goodVertices"]	= DEFAULT;
    choose["Events/Flag_goodVertices_pRECO"]	= DEFAULT;
    choose["Events/Flag_hcalLaserEventFilter"]	= DEFAULT;
    choose["Events/Flag_hcalLaserEventFilter_pRECO"]	= DEFAULT;
    choose["Events/Flag_hfNoisyHitsFilter"]	= DEFAULT;
    choose["Events/Flag_hfNoisyHitsFilter_pRECO"]	= DEFAULT;
    choose["Events/Flag_muonBadTrackFilter"]	= DEFAULT;
    choose["Events/Flag_muonBadTrackFilter_pRECO"]	= DEFAULT;
    choose["Events/Flag_trkPOGFilters"]	= DEFAULT;
    choose["Events/Flag_trkPOGFilters_pRECO"]	= DEFAULT;
    choose["Events/Flag_trkPOG_logErrorTooManyClusters"]	= DEFAULT;
    choose["Events/Flag_trkPOG_logErrorTooManyClusters_pRECO"]	= DEFAULT;
    choose["Events/Flag_trkPOG_manystripclus53X"]	= DEFAULT;
    choose["Events/Flag_trkPOG_manystripclus53X_pRECO"]	= DEFAULT;
    choose["Events/Flag_trkPOG_toomanystripclus53X"]	= DEFAULT;
    choose["Events/Flag_trkPOG_toomanystripclus53X_pRECO"]	= DEFAULT;
    choose["Events/FsrPhoton_dROverEt2"]	= DEFAULT;
    choose["Events/FsrPhoton_eta"]	= DEFAULT;
    choose["Events/FsrPhoton_muonIdx"]	= DEFAULT;
    choose["Events/FsrPhoton_phi"]	= DEFAULT;
    choose["Events/FsrPhoton_pt"]	= DEFAULT;
    choose["Events/FsrPhoton_relIso03"]	= DEFAULT;
    choose["Events/GenDressedLepton_eta"]	= DEFAULT;
    choose["Events/GenDressedLepton_hasTauAnc"]	= DEFAULT;
    choose["Events/GenDressedLepton_mass"]	= DEFAULT;
    choose["Events/GenDressedLepton_pdgId"]	= DEFAULT;
    choose["Events/GenDressedLepton_phi"]	= DEFAULT;
    choose["Events/GenDressedLepton_pt"]	= DEFAULT;
    choose["Events/GenIsolatedPhoton_eta"]	= DEFAULT;
    choose["Events/GenIsolatedPhoton_mass"]	= DEFAULT;
    choose["Events/GenIsolatedPhoton_phi"]	= DEFAULT;
    choose["Events/GenIsolatedPhoton_pt"]	= DEFAULT;
    choose["Events/GenJetAK8_eta"]	= DEFAULT;
    choose["Events/GenJetAK8_hadronFlavour"]	= DEFAULT;
    choose["Events/GenJetAK8_mass"]	= DEFAULT;
    choose["Events/GenJetAK8_partonFlavour"]	= DEFAULT;
    choose["Events/GenJetAK8_phi"]	= DEFAULT;
    choose["Events/GenJetAK8_pt"]	= DEFAULT;
    choose["Events/GenJet_eta"]	= DEFAULT;
    choose["Events/GenJet_hadronFlavour"]	= DEFAULT;
    choose["Events/GenJet_mass"]	= DEFAULT;
    choose["Events/GenJet_nBHadrons"]	= DEFAULT;
    choose["Events/GenJet_nCHadrons"]	= DEFAULT;
    choose["Events/GenJet_partonFlavour"]	= DEFAULT;
    choose["Events/GenJet_phi"]	= DEFAULT;
    choose["Events/GenJet_pt"]	= DEFAULT;
    choose["Events/GenMET_phi"]	= DEFAULT;
    choose["Events/GenMET_pt"]	= DEFAULT;
    choose["Events/GenPart_eta"]	= DEFAULT;
    choose["Events/GenPart_genPartIdxMother"]	= DEFAULT;
    choose["Events/GenPart_mass"]	= DEFAULT;
    choose["Events/GenPart_pdgId"]	= DEFAULT;
    choose["Events/GenPart_phi"]	= DEFAULT;
    choose["Events/GenPart_pt"]	= DEFAULT;
    choose["Events/GenPart_status"]	= DEFAULT;
    choose["Events/GenPart_statusFlags"]	= DEFAULT;
    choose["Events/GenVisTau_charge"]	= DEFAULT;
    choose["Events/GenVisTau_eta"]	= DEFAULT;
    choose["Events/GenVisTau_genPartIdxMother"]	= DEFAULT;
    choose["Events/GenVisTau_mass"]	= DEFAULT;
    choose["Events/GenVisTau_phi"]	= DEFAULT;
    choose["Events/GenVisTau_pt"]	= DEFAULT;
    choose["Events/GenVisTau_status"]	= DEFAULT;
    choose["Events/GenVtx_t0"]	= DEFAULT;
    choose["Events/GenVtx_x"]	= DEFAULT;
    choose["Events/GenVtx_y"]	= DEFAULT;
    choose["Events/GenVtx_z"]	= DEFAULT;
    choose["Events/Generator_binvar"]	= DEFAULT;
    choose["Events/Generator_id1"]	= DEFAULT;
    choose["Events/Generator_id2"]	= DEFAULT;
    choose["Events/Generator_scalePDF"]	= DEFAULT;
    choose["Events/Generator_weight"]	= DEFAULT;
    choose["Events/Generator_x1"]	= DEFAULT;
    choose["Events/Generator_x2"]	= DEFAULT;
    choose["Events/Generator_xpdf1"]	= DEFAULT;
    choose["Events/Generator_xpdf2"]	= DEFAULT;
    choose["Events/HLT_Ele30_WPTight_Gsf"]	= DEFAULT;
    choose["Events/HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07"]	= DEFAULT;
    choose["Events/HLT_IsoMu24"]	= DEFAULT;
    choose["Events/HLT_IsoMu27"]	= DEFAULT;
    choose["Events/HLT_PFHT1050"]	= DEFAULT;
    choose["Events/HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p55"]	= DEFAULT;
    choose["Events/HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p60"]	= DEFAULT;
    choose["Events/HLT_PFHT280_QuadPFJet35_PNet2BTagMean0p60"]	= DEFAULT;
    choose["Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0"]	= DEFAULT;
    choose["Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40"]	= DEFAULT;
    choose["Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_2p0"]	= DEFAULT;
    choose["Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3"]	= DEFAULT;
    choose["Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5"]	= DEFAULT;
    choose["Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5"]	= DEFAULT;
    choose["Events/HLT_PFHT340_QuadPFJet70_50_40_40_PNet2BTagMean0p70"]	= DEFAULT;
    choose["Events/HLT_PFHT380_SixJet32_DoubleBTagCSV_p075"]	= DEFAULT;
    choose["Events/HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2"]	= DEFAULT;
    choose["Events/HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_4p3"]	= DEFAULT;
    choose["Events/HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_5p6"]	= DEFAULT;
    choose["Events/HLT_PFHT400_SixPFJet32"]	= DEFAULT;
    choose["Events/HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94"]	= DEFAULT;
    choose["Events/HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50"]	= DEFAULT;
    choose["Events/HLT_PFHT430_SixJet40_BTagCSV_p080"]	= DEFAULT;
    choose["Events/HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5"]	= DEFAULT;
    choose["Events/HLT_PFHT450_SixPFJet36"]	= DEFAULT;
    choose["Events/HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59"]	= DEFAULT;
    choose["Events/HLT_PFHT450_SixPFJet36_PNetBTag0p35"]	= DEFAULT;
    choose["Events/HLTriggerFinalPath"]	= DEFAULT;
    choose["Events/HLTriggerFirstPath"]	= DEFAULT;
    choose["Events/HTXS_Higgs_pt"]	= DEFAULT;
    choose["Events/HTXS_Higgs_y"]	= DEFAULT;
    choose["Events/HTXS_njets25"]	= DEFAULT;
    choose["Events/HTXS_njets30"]	= DEFAULT;
    choose["Events/HTXS_stage1_1_cat_pTjet25GeV"]	= DEFAULT;
    choose["Events/HTXS_stage1_1_cat_pTjet30GeV"]	= DEFAULT;
    choose["Events/HTXS_stage1_1_fine_cat_pTjet25GeV"]	= DEFAULT;
    choose["Events/HTXS_stage1_1_fine_cat_pTjet30GeV"]	= DEFAULT;
    choose["Events/HTXS_stage1_2_cat_pTjet25GeV"]	= DEFAULT;
    choose["Events/HTXS_stage1_2_cat_pTjet30GeV"]	= DEFAULT;
    choose["Events/HTXS_stage1_2_fine_cat_pTjet25GeV"]	= DEFAULT;
    choose["Events/HTXS_stage1_2_fine_cat_pTjet30GeV"]	= DEFAULT;
    choose["Events/HTXS_stage_0"]	= DEFAULT;
    choose["Events/HTXS_stage_1_pTjet25"]	= DEFAULT;
    choose["Events/HTXS_stage_1_pTjet30"]	= DEFAULT;
    choose["Events/IsoTrack_charge"]	= DEFAULT;
    choose["Events/IsoTrack_dxy"]	= DEFAULT;
    choose["Events/IsoTrack_dz"]	= DEFAULT;
    choose["Events/IsoTrack_eta"]	= DEFAULT;
    choose["Events/IsoTrack_fromPV"]	= DEFAULT;
    choose["Events/IsoTrack_isFromLostTrack"]	= DEFAULT;
    choose["Events/IsoTrack_isHighPurityTrack"]	= DEFAULT;
    choose["Events/IsoTrack_isPFcand"]	= DEFAULT;
    choose["Events/IsoTrack_miniPFRelIso_all"]	= DEFAULT;
    choose["Events/IsoTrack_miniPFRelIso_chg"]	= DEFAULT;
    choose["Events/IsoTrack_pdgId"]	= DEFAULT;
    choose["Events/IsoTrack_pfRelIso03_all"]	= DEFAULT;
    choose["Events/IsoTrack_pfRelIso03_chg"]	= DEFAULT;
    choose["Events/IsoTrack_phi"]	= DEFAULT;
    choose["Events/IsoTrack_pt"]	= DEFAULT;
    choose["Events/Jet_PNetRegPtRawCorr"]	= DEFAULT;
    choose["Events/Jet_PNetRegPtRawCorrNeutrino"]	= DEFAULT;
    choose["Events/Jet_PNetRegPtRawRes"]	= DEFAULT;
    choose["Events/Jet_UParTAK4RegPtRawCorr"]	= DEFAULT;
    choose["Events/Jet_UParTAK4RegPtRawCorrNeutrino"]	= DEFAULT;
    choose["Events/Jet_UParTAK4RegPtRawRes"]	= DEFAULT;
    choose["Events/Jet_UParTAK4V1RegPtRawCorr"]	= DEFAULT;
    choose["Events/Jet_UParTAK4V1RegPtRawCorrNeutrino"]	= DEFAULT;
    choose["Events/Jet_UParTAK4V1RegPtRawRes"]	= DEFAULT;
    choose["Events/Jet_area"]	= DEFAULT;
    choose["Events/Jet_bRegCorr"]	= DEFAULT;
    choose["Events/Jet_bRegRes"]	= DEFAULT;
    choose["Events/Jet_btagCSVV2"]	= DEFAULT;
    choose["Events/Jet_btagDeepB"]	= DEFAULT;
    choose["Events/Jet_btagDeepCvB"]	= DEFAULT;
    choose["Events/Jet_btagDeepCvL"]	= DEFAULT;
    choose["Events/Jet_btagDeepFlavB"]	= DEFAULT;
    choose["Events/Jet_btagDeepFlavCvB"]	= DEFAULT;
    choose["Events/Jet_btagDeepFlavCvL"]	= DEFAULT;
    choose["Events/Jet_btagDeepFlavQG"]	= DEFAULT;
    choose["Events/Jet_btagPNetB"]	= DEFAULT;
    choose["Events/Jet_btagPNetCvB"]	= DEFAULT;
    choose["Events/Jet_btagPNetCvL"]	= DEFAULT;
    choose["Events/Jet_btagPNetQvG"]	= DEFAULT;
    choose["Events/Jet_btagUParTAK4B"]	= DEFAULT;
    choose["Events/Jet_btagUParTAK4CvB"]	= DEFAULT;
    choose["Events/Jet_btagUParTAK4CvL"]	= DEFAULT;
    choose["Events/Jet_btagUParTAK4QvG"]	= DEFAULT;
    choose["Events/Jet_cRegCorr"]	= DEFAULT;
    choose["Events/Jet_cRegRes"]	= DEFAULT;
    choose["Events/Jet_chEmEF"]	= DEFAULT;
    choose["Events/Jet_chFPV0EF"]	= DEFAULT;
    choose["Events/Jet_chHEF"]	= DEFAULT;
    choose["Events/Jet_chMultiplicity"]	= DEFAULT;
    choose["Events/Jet_cleanmask"]	= DEFAULT;
    choose["Events/Jet_electronIdx1"]	= DEFAULT;
    choose["Events/Jet_electronIdx2"]	= DEFAULT;
    choose["Events/Jet_eta"]	= DEFAULT;
    choose["Events/Jet_genJetIdx"]	= DEFAULT;
    choose["Events/Jet_hadronFlavour"]	= DEFAULT;
    choose["Events/Jet_hfEmEF"]	= DEFAULT;
    choose["Events/Jet_hfHEF"]	= DEFAULT;
    choose["Events/Jet_hfadjacentEtaStripsSize"]	= DEFAULT;
    choose["Events/Jet_hfcentralEtaStripSize"]	= DEFAULT;
    choose["Events/Jet_hfsigmaEtaEta"]	= DEFAULT;
    choose["Events/Jet_hfsigmaPhiPhi"]	= DEFAULT;
    choose["Events/Jet_jetId"]	= DEFAULT;
    choose["Events/Jet_mass"]	= DEFAULT;
    choose["Events/Jet_muEF"]	= DEFAULT;
    choose["Events/Jet_muonIdx1"]	= DEFAULT;
    choose["Events/Jet_muonIdx2"]	= DEFAULT;
    choose["Events/Jet_muonSubtrFactor"]	= DEFAULT;
    choose["Events/Jet_nConstituents"]	= DEFAULT;
    choose["Events/Jet_nElectrons"]	= DEFAULT;
    choose["Events/Jet_nMuons"]	= DEFAULT;
    choose["Events/Jet_neEmEF"]	= DEFAULT;
    choose["Events/Jet_neHEF"]	= DEFAULT;
    choose["Events/Jet_neMultiplicity"]	= DEFAULT;
    choose["Events/Jet_partonFlavour"]	= DEFAULT;
    choose["Events/Jet_phi"]	= DEFAULT;
    choose["Events/Jet_pt"]	= DEFAULT;
    choose["Events/Jet_puId"]	= DEFAULT;
    choose["Events/Jet_puIdDisc"]	= DEFAULT;
    choose["Events/Jet_qgl"]	= DEFAULT;
    choose["Events/Jet_rawFactor"]	= DEFAULT;
    choose["Events/L1PreFiringWeight_Dn"]	= DEFAULT;
    choose["Events/L1PreFiringWeight_ECAL_Dn"]	= DEFAULT;
    choose["Events/L1PreFiringWeight_ECAL_Nom"]	= DEFAULT;
    choose["Events/L1PreFiringWeight_ECAL_Up"]	= DEFAULT;
    choose["Events/L1PreFiringWeight_Muon_Nom"]	= DEFAULT;
    choose["Events/L1PreFiringWeight_Muon_StatDn"]	= DEFAULT;
    choose["Events/L1PreFiringWeight_Muon_StatUp"]	= DEFAULT;
    choose["Events/L1PreFiringWeight_Muon_SystDn"]	= DEFAULT;
    choose["Events/L1PreFiringWeight_Muon_SystUp"]	= DEFAULT;
    choose["Events/L1PreFiringWeight_Nom"]	= DEFAULT;
    choose["Events/L1PreFiringWeight_Up"]	= DEFAULT;
    choose["Events/L1Reco_step"]	= DEFAULT;
    choose["Events/L1simulation_step"]	= DEFAULT;
    choose["Events/LHEPart_eta"]	= DEFAULT;
    choose["Events/LHEPart_incomingpz"]	= DEFAULT;
    choose["Events/LHEPart_mass"]	= DEFAULT;
    choose["Events/LHEPart_pdgId"]	= DEFAULT;
    choose["Events/LHEPart_phi"]	= DEFAULT;
    choose["Events/LHEPart_pt"]	= DEFAULT;
    choose["Events/LHEPart_spin"]	= DEFAULT;
    choose["Events/LHEPart_status"]	= DEFAULT;
    choose["Events/LHEPdfWeight"]	= DEFAULT;
    choose["Events/LHEReweightingWeight"]	= DEFAULT;
    choose["Events/LHEScaleWeight"]	= DEFAULT;
    choose["Events/LHEWeight_originalXWGTUP"]	= DEFAULT;
    choose["Events/LHE_AlphaS"]	= DEFAULT;
    choose["Events/LHE_HT"]	= DEFAULT;
    choose["Events/LHE_HTIncoming"]	= DEFAULT;
    choose["Events/LHE_Nb"]	= DEFAULT;
    choose["Events/LHE_Nc"]	= DEFAULT;
    choose["Events/LHE_Nglu"]	= DEFAULT;
    choose["Events/LHE_Njets"]	= DEFAULT;
    choose["Events/LHE_NpLO"]	= DEFAULT;
    choose["Events/LHE_NpNLO"]	= DEFAULT;
    choose["Events/LHE_Nuds"]	= DEFAULT;
    choose["Events/LHE_Vpt"]	= DEFAULT;
    choose["Events/LowPtElectron_ID"]	= DEFAULT;
    choose["Events/LowPtElectron_charge"]	= DEFAULT;
    choose["Events/LowPtElectron_convVeto"]	= DEFAULT;
    choose["Events/LowPtElectron_convVtxRadius"]	= DEFAULT;
    choose["Events/LowPtElectron_convWP"]	= DEFAULT;
    choose["Events/LowPtElectron_deltaEtaSC"]	= DEFAULT;
    choose["Events/LowPtElectron_dxy"]	= DEFAULT;
    choose["Events/LowPtElectron_dxyErr"]	= DEFAULT;
    choose["Events/LowPtElectron_dz"]	= DEFAULT;
    choose["Events/LowPtElectron_dzErr"]	= DEFAULT;
    choose["Events/LowPtElectron_eInvMinusPInv"]	= DEFAULT;
    choose["Events/LowPtElectron_embeddedID"]	= DEFAULT;
    choose["Events/LowPtElectron_energyErr"]	= DEFAULT;
    choose["Events/LowPtElectron_eta"]	= DEFAULT;
    choose["Events/LowPtElectron_genPartFlav"]	= DEFAULT;
    choose["Events/LowPtElectron_genPartIdx"]	= DEFAULT;
    choose["Events/LowPtElectron_hoe"]	= DEFAULT;
    choose["Events/LowPtElectron_lostHits"]	= DEFAULT;
    choose["Events/LowPtElectron_mass"]	= DEFAULT;
    choose["Events/LowPtElectron_miniPFRelIso_all"]	= DEFAULT;
    choose["Events/LowPtElectron_miniPFRelIso_chg"]	= DEFAULT;
    choose["Events/LowPtElectron_pdgId"]	= DEFAULT;
    choose["Events/LowPtElectron_phi"]	= DEFAULT;
    choose["Events/LowPtElectron_pt"]	= DEFAULT;
    choose["Events/LowPtElectron_ptbiased"]	= DEFAULT;
    choose["Events/LowPtElectron_r9"]	= DEFAULT;
    choose["Events/LowPtElectron_scEtOverPt"]	= DEFAULT;
    choose["Events/LowPtElectron_sieie"]	= DEFAULT;
    choose["Events/LowPtElectron_unbiased"]	= DEFAULT;
    choose["Events/MET_MetUnclustEnUpDeltaX"]	= DEFAULT;
    choose["Events/MET_MetUnclustEnUpDeltaY"]	= DEFAULT;
    choose["Events/MET_covXX"]	= DEFAULT;
    choose["Events/MET_covXY"]	= DEFAULT;
    choose["Events/MET_covYY"]	= DEFAULT;
    choose["Events/MET_fiducialGenPhi"]	= DEFAULT;
    choose["Events/MET_fiducialGenPt"]	= DEFAULT;
    choose["Events/MET_phi"]	= DEFAULT;
    choose["Events/MET_pt"]	= DEFAULT;
    choose["Events/MET_significance"]	= DEFAULT;
    choose["Events/MET_sumEt"]	= DEFAULT;
    choose["Events/MET_sumPtUnclustered"]	= DEFAULT;
    choose["Events/Muon_charge"]	= DEFAULT;
    choose["Events/Muon_cleanmask"]	= DEFAULT;
    choose["Events/Muon_dxy"]	= DEFAULT;
    choose["Events/Muon_dxyErr"]	= DEFAULT;
    choose["Events/Muon_dxybs"]	= DEFAULT;
    choose["Events/Muon_dz"]	= DEFAULT;
    choose["Events/Muon_dzErr"]	= DEFAULT;
    choose["Events/Muon_eta"]	= DEFAULT;
    choose["Events/Muon_fsrPhotonIdx"]	= DEFAULT;
    choose["Events/Muon_genPartFlav"]	= DEFAULT;
    choose["Events/Muon_genPartIdx"]	= DEFAULT;
    choose["Events/Muon_highPtId"]	= DEFAULT;
    choose["Events/Muon_highPurity"]	= DEFAULT;
    choose["Events/Muon_inTimeMuon"]	= DEFAULT;
    choose["Events/Muon_ip3d"]	= DEFAULT;
    choose["Events/Muon_isGlobal"]	= DEFAULT;
    choose["Events/Muon_isPFcand"]	= DEFAULT;
    choose["Events/Muon_isStandalone"]	= DEFAULT;
    choose["Events/Muon_isTracker"]	= DEFAULT;
    choose["Events/Muon_jetIdx"]	= DEFAULT;
    choose["Events/Muon_jetNDauCharged"]	= DEFAULT;
    choose["Events/Muon_jetPtRelv2"]	= DEFAULT;
    choose["Events/Muon_jetRelIso"]	= DEFAULT;
    choose["Events/Muon_looseId"]	= DEFAULT;
    choose["Events/Muon_mass"]	= DEFAULT;
    choose["Events/Muon_mediumId"]	= DEFAULT;
    choose["Events/Muon_mediumPromptId"]	= DEFAULT;
    choose["Events/Muon_miniIsoId"]	= DEFAULT;
    choose["Events/Muon_miniPFRelIso_all"]	= DEFAULT;
    choose["Events/Muon_miniPFRelIso_chg"]	= DEFAULT;
    choose["Events/Muon_multiIsoId"]	= DEFAULT;
    choose["Events/Muon_mvaId"]	= DEFAULT;
    choose["Events/Muon_mvaLowPt"]	= DEFAULT;
    choose["Events/Muon_mvaLowPtId"]	= DEFAULT;
    choose["Events/Muon_mvaMuID"]	= DEFAULT;
    choose["Events/Muon_mvaMuID_WP"]	= DEFAULT;
    choose["Events/Muon_mvaTTH"]	= DEFAULT;
    choose["Events/Muon_nStations"]	= DEFAULT;
    choose["Events/Muon_nTrackerLayers"]	= DEFAULT;
    choose["Events/Muon_pdgId"]	= DEFAULT;
    choose["Events/Muon_pfIsoId"]	= DEFAULT;
    choose["Events/Muon_pfRelIso03_all"]	= DEFAULT;
    choose["Events/Muon_pfRelIso03_chg"]	= DEFAULT;
    choose["Events/Muon_pfRelIso04_all"]	= DEFAULT;
    choose["Events/Muon_phi"]	= DEFAULT;
    choose["Events/Muon_promptMVA"]	= DEFAULT;
    choose["Events/Muon_pt"]	= DEFAULT;
    choose["Events/Muon_ptErr"]	= DEFAULT;
    choose["Events/Muon_puppiIsoId"]	= DEFAULT;
    choose["Events/Muon_segmentComp"]	= DEFAULT;
    choose["Events/Muon_sip3d"]	= DEFAULT;
    choose["Events/Muon_softId"]	= DEFAULT;
    choose["Events/Muon_softMva"]	= DEFAULT;
    choose["Events/Muon_softMvaId"]	= DEFAULT;
    choose["Events/Muon_tightCharge"]	= DEFAULT;
    choose["Events/Muon_tightId"]	= DEFAULT;
    choose["Events/Muon_tkIsoId"]	= DEFAULT;
    choose["Events/Muon_tkRelIso"]	= DEFAULT;
    choose["Events/Muon_triggerIdLoose"]	= DEFAULT;
    choose["Events/Muon_tunepRelPt"]	= DEFAULT;
    choose["Events/OtherPV_z"]	= DEFAULT;
    choose["Events/PFMET_covXX"]	= DEFAULT;
    choose["Events/PFMET_covXY"]	= DEFAULT;
    choose["Events/PFMET_covYY"]	= DEFAULT;
    choose["Events/PFMET_phi"]	= DEFAULT;
    choose["Events/PFMET_phiUnclusteredDown"]	= DEFAULT;
    choose["Events/PFMET_phiUnclusteredUp"]	= DEFAULT;
    choose["Events/PFMET_pt"]	= DEFAULT;
    choose["Events/PFMET_ptUnclusteredDown"]	= DEFAULT;
    choose["Events/PFMET_ptUnclusteredUp"]	= DEFAULT;
    choose["Events/PFMET_significance"]	= DEFAULT;
    choose["Events/PFMET_sumEt"]	= DEFAULT;
    choose["Events/PFMET_sumPtUnclustered"]	= DEFAULT;
    choose["Events/PPSLocalTrack_decRPId"]	= DEFAULT;
    choose["Events/PPSLocalTrack_multiRPProtonIdx"]	= DEFAULT;
    choose["Events/PPSLocalTrack_rpType"]	= DEFAULT;
    choose["Events/PPSLocalTrack_singleRPProtonIdx"]	= DEFAULT;
    choose["Events/PPSLocalTrack_time"]	= DEFAULT;
    choose["Events/PPSLocalTrack_timeUnc"]	= DEFAULT;
    choose["Events/PPSLocalTrack_x"]	= DEFAULT;
    choose["Events/PPSLocalTrack_y"]	= DEFAULT;
    choose["Events/PSWeight"]	= DEFAULT;
    choose["Events/PV_chi2"]	= DEFAULT;
    choose["Events/PV_ndof"]	= DEFAULT;
    choose["Events/PV_npvs"]	= DEFAULT;
    choose["Events/PV_npvsGood"]	= DEFAULT;
    choose["Events/PV_score"]	= DEFAULT;
    choose["Events/PV_x"]	= DEFAULT;
    choose["Events/PV_y"]	= DEFAULT;
    choose["Events/PV_z"]	= DEFAULT;
    choose["Events/Photon_charge"]	= DEFAULT;
    choose["Events/Photon_cleanmask"]	= DEFAULT;
    choose["Events/Photon_cutBased"]	= DEFAULT;
    choose["Events/Photon_cutBased_Fall17V1Bitmap"]	= DEFAULT;
    choose["Events/Photon_dEscaleDown"]	= DEFAULT;
    choose["Events/Photon_dEscaleUp"]	= DEFAULT;
    choose["Events/Photon_dEsigmaDown"]	= DEFAULT;
    choose["Events/Photon_dEsigmaUp"]	= DEFAULT;
    choose["Events/Photon_eCorr"]	= DEFAULT;
    choose["Events/Photon_electronIdx"]	= DEFAULT;
    choose["Events/Photon_electronVeto"]	= DEFAULT;
    choose["Events/Photon_energyErr"]	= DEFAULT;
    choose["Events/Photon_eta"]	= DEFAULT;
    choose["Events/Photon_genPartFlav"]	= DEFAULT;
    choose["Events/Photon_genPartIdx"]	= DEFAULT;
    choose["Events/Photon_hoe"]	= DEFAULT;
    choose["Events/Photon_isScEtaEB"]	= DEFAULT;
    choose["Events/Photon_isScEtaEE"]	= DEFAULT;
    choose["Events/Photon_jetIdx"]	= DEFAULT;
    choose["Events/Photon_mass"]	= DEFAULT;
    choose["Events/Photon_mvaID"]	= DEFAULT;
    choose["Events/Photon_mvaID_Fall17V1p1"]	= DEFAULT;
    choose["Events/Photon_mvaID_WP80"]	= DEFAULT;
    choose["Events/Photon_mvaID_WP90"]	= DEFAULT;
    choose["Events/Photon_pdgId"]	= DEFAULT;
    choose["Events/Photon_pfRelIso03_all"]	= DEFAULT;
    choose["Events/Photon_pfRelIso03_chg"]	= DEFAULT;
    choose["Events/Photon_phi"]	= DEFAULT;
    choose["Events/Photon_pixelSeed"]	= DEFAULT;
    choose["Events/Photon_pt"]	= DEFAULT;
    choose["Events/Photon_r9"]	= DEFAULT;
    choose["Events/Photon_seedGain"]	= DEFAULT;
    choose["Events/Photon_sieie"]	= DEFAULT;
    choose["Events/Photon_vidNestedWPBitmap"]	= DEFAULT;
    choose["Events/Pileup_gpudensity"]	= DEFAULT;
    choose["Events/Pileup_nPU"]	= DEFAULT;
    choose["Events/Pileup_nTrueInt"]	= DEFAULT;
    choose["Events/Pileup_pudensity"]	= DEFAULT;
    choose["Events/Pileup_sumEOOT"]	= DEFAULT;
    choose["Events/Pileup_sumLOOT"]	= DEFAULT;
    choose["Events/Proton_multiRP_arm"]	= DEFAULT;
    choose["Events/Proton_multiRP_t"]	= DEFAULT;
    choose["Events/Proton_multiRP_thetaX"]	= DEFAULT;
    choose["Events/Proton_multiRP_thetaY"]	= DEFAULT;
    choose["Events/Proton_multiRP_time"]	= DEFAULT;
    choose["Events/Proton_multiRP_timeUnc"]	= DEFAULT;
    choose["Events/Proton_multiRP_xi"]	= DEFAULT;
    choose["Events/Proton_singleRP_decRPId"]	= DEFAULT;
    choose["Events/Proton_singleRP_thetaY"]	= DEFAULT;
    choose["Events/Proton_singleRP_xi"]	= DEFAULT;
    choose["Events/PuppiMET_covXX"]	= DEFAULT;
    choose["Events/PuppiMET_covXY"]	= DEFAULT;
    choose["Events/PuppiMET_covYY"]	= DEFAULT;
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
    choose["Events/PuppiMET_significance"]	= DEFAULT;
    choose["Events/PuppiMET_sumEt"]	= DEFAULT;
    choose["Events/PuppiMET_sumPtUnclustered"]	= DEFAULT;
    choose["Events/RawMET_phi"]	= DEFAULT;
    choose["Events/RawMET_pt"]	= DEFAULT;
    choose["Events/RawMET_sumEt"]	= DEFAULT;
    choose["Events/RawPFMET_phi"]	= DEFAULT;
    choose["Events/RawPFMET_pt"]	= DEFAULT;
    choose["Events/RawPFMET_sumEt"]	= DEFAULT;
    choose["Events/RawPuppiMET_phi"]	= DEFAULT;
    choose["Events/RawPuppiMET_pt"]	= DEFAULT;
    choose["Events/RawPuppiMET_sumEt"]	= DEFAULT;
    choose["Events/Rho_fixedGridRhoAll"]	= DEFAULT;
    choose["Events/Rho_fixedGridRhoFastjetAll"]	= DEFAULT;
    choose["Events/Rho_fixedGridRhoFastjetCentral"]	= DEFAULT;
    choose["Events/Rho_fixedGridRhoFastjetCentralCalo"]	= DEFAULT;
    choose["Events/Rho_fixedGridRhoFastjetCentralChargedPileUp"]	= DEFAULT;
    choose["Events/Rho_fixedGridRhoFastjetCentralNeutral"]	= DEFAULT;
    choose["Events/SV_charge"]	= DEFAULT;
    choose["Events/SV_chi2"]	= DEFAULT;
    choose["Events/SV_dlen"]	= DEFAULT;
    choose["Events/SV_dlenSig"]	= DEFAULT;
    choose["Events/SV_dxy"]	= DEFAULT;
    choose["Events/SV_dxySig"]	= DEFAULT;
    choose["Events/SV_eta"]	= DEFAULT;
    choose["Events/SV_mass"]	= DEFAULT;
    choose["Events/SV_ndof"]	= DEFAULT;
    choose["Events/SV_ntracks"]	= DEFAULT;
    choose["Events/SV_pAngle"]	= DEFAULT;
    choose["Events/SV_phi"]	= DEFAULT;
    choose["Events/SV_pt"]	= DEFAULT;
    choose["Events/SV_x"]	= DEFAULT;
    choose["Events/SV_y"]	= DEFAULT;
    choose["Events/SV_z"]	= DEFAULT;
    choose["Events/SoftActivityJetHT"]	= DEFAULT;
    choose["Events/SoftActivityJetHT10"]	= DEFAULT;
    choose["Events/SoftActivityJetHT2"]	= DEFAULT;
    choose["Events/SoftActivityJetHT5"]	= DEFAULT;
    choose["Events/SoftActivityJetNjets10"]	= DEFAULT;
    choose["Events/SoftActivityJetNjets2"]	= DEFAULT;
    choose["Events/SoftActivityJetNjets5"]	= DEFAULT;
    choose["Events/SoftActivityJet_eta"]	= DEFAULT;
    choose["Events/SoftActivityJet_phi"]	= DEFAULT;
    choose["Events/SoftActivityJet_pt"]	= DEFAULT;
    choose["Events/SubGenJetAK8_eta"]	= DEFAULT;
    choose["Events/SubGenJetAK8_mass"]	= DEFAULT;
    choose["Events/SubGenJetAK8_phi"]	= DEFAULT;
    choose["Events/SubGenJetAK8_pt"]	= DEFAULT;
    choose["Events/SubJet_btagCSVV2"]	= DEFAULT;
    choose["Events/SubJet_btagDeepB"]	= DEFAULT;
    choose["Events/SubJet_eta"]	= DEFAULT;
    choose["Events/SubJet_hadronFlavour"]	= DEFAULT;
    choose["Events/SubJet_mass"]	= DEFAULT;
    choose["Events/SubJet_n2b1"]	= DEFAULT;
    choose["Events/SubJet_n3b1"]	= DEFAULT;
    choose["Events/SubJet_nBHadrons"]	= DEFAULT;
    choose["Events/SubJet_nCHadrons"]	= DEFAULT;
    choose["Events/SubJet_phi"]	= DEFAULT;
    choose["Events/SubJet_pt"]	= DEFAULT;
    choose["Events/SubJet_rawFactor"]	= DEFAULT;
    choose["Events/SubJet_tau1"]	= DEFAULT;
    choose["Events/SubJet_tau2"]	= DEFAULT;
    choose["Events/SubJet_tau3"]	= DEFAULT;
    choose["Events/SubJet_tau4"]	= DEFAULT;
    choose["Events/Tau_charge"]	= DEFAULT;
    choose["Events/Tau_chargedIso"]	= DEFAULT;
    choose["Events/Tau_cleanmask"]	= DEFAULT;
    choose["Events/Tau_decayMode"]	= DEFAULT;
    choose["Events/Tau_dxy"]	= DEFAULT;
    choose["Events/Tau_dz"]	= DEFAULT;
    choose["Events/Tau_eta"]	= DEFAULT;
    choose["Events/Tau_genPartFlav"]	= DEFAULT;
    choose["Events/Tau_genPartIdx"]	= DEFAULT;
    choose["Events/Tau_idAntiEleDeadECal"]	= DEFAULT;
    choose["Events/Tau_idAntiMu"]	= DEFAULT;
    choose["Events/Tau_idDecayModeOldDMs"]	= DEFAULT;
    choose["Events/Tau_idDeepTau2017v2p1VSe"]	= DEFAULT;
    choose["Events/Tau_idDeepTau2017v2p1VSjet"]	= DEFAULT;
    choose["Events/Tau_idDeepTau2017v2p1VSmu"]	= DEFAULT;
    choose["Events/Tau_jetIdx"]	= DEFAULT;
    choose["Events/Tau_leadTkDeltaEta"]	= DEFAULT;
    choose["Events/Tau_leadTkDeltaPhi"]	= DEFAULT;
    choose["Events/Tau_leadTkPtOverTauPt"]	= DEFAULT;
    choose["Events/Tau_mass"]	= DEFAULT;
    choose["Events/Tau_neutralIso"]	= DEFAULT;
    choose["Events/Tau_phi"]	= DEFAULT;
    choose["Events/Tau_photonsOutsideSignalCone"]	= DEFAULT;
    choose["Events/Tau_pt"]	= DEFAULT;
    choose["Events/Tau_puCorr"]	= DEFAULT;
    choose["Events/Tau_rawDeepTau2017v2p1VSe"]	= DEFAULT;
    choose["Events/Tau_rawDeepTau2017v2p1VSjet"]	= DEFAULT;
    choose["Events/Tau_rawDeepTau2017v2p1VSmu"]	= DEFAULT;
    choose["Events/Tau_rawIso"]	= DEFAULT;
    choose["Events/Tau_rawIsodR03"]	= DEFAULT;
    choose["Events/TkMET_phi"]	= DEFAULT;
    choose["Events/TkMET_pt"]	= DEFAULT;
    choose["Events/TkMET_sumEt"]	= DEFAULT;
    choose["Events/TrigObj_eta"]	= DEFAULT;
    choose["Events/TrigObj_filterBits"]	= DEFAULT;
    choose["Events/TrigObj_id"]	= DEFAULT;
    choose["Events/TrigObj_l1charge"]	= DEFAULT;
    choose["Events/TrigObj_l1iso"]	= DEFAULT;
    choose["Events/TrigObj_l1pt"]	= DEFAULT;
    choose["Events/TrigObj_l1pt_2"]	= DEFAULT;
    choose["Events/TrigObj_l2pt"]	= DEFAULT;
    choose["Events/TrigObj_phi"]	= DEFAULT;
    choose["Events/TrigObj_pt"]	= DEFAULT;
    choose["Events/boostedTau_charge"]	= DEFAULT;
    choose["Events/boostedTau_chargedIso"]	= DEFAULT;
    choose["Events/boostedTau_decayMode"]	= DEFAULT;
    choose["Events/boostedTau_eta"]	= DEFAULT;
    choose["Events/boostedTau_genPartFlav"]	= DEFAULT;
    choose["Events/boostedTau_genPartIdx"]	= DEFAULT;
    choose["Events/boostedTau_idAntiEle2018"]	= DEFAULT;
    choose["Events/boostedTau_idAntiMu"]	= DEFAULT;
    choose["Events/boostedTau_idMVAnewDM2017v2"]	= DEFAULT;
    choose["Events/boostedTau_idMVAoldDM2017v2"]	= DEFAULT;
    choose["Events/boostedTau_idMVAoldDMdR032017v2"]	= DEFAULT;
    choose["Events/boostedTau_jetIdx"]	= DEFAULT;
    choose["Events/boostedTau_leadTkDeltaEta"]	= DEFAULT;
    choose["Events/boostedTau_leadTkDeltaPhi"]	= DEFAULT;
    choose["Events/boostedTau_leadTkPtOverTauPt"]	= DEFAULT;
    choose["Events/boostedTau_mass"]	= DEFAULT;
    choose["Events/boostedTau_neutralIso"]	= DEFAULT;
    choose["Events/boostedTau_phi"]	= DEFAULT;
    choose["Events/boostedTau_photonsOutsideSignalCone"]	= DEFAULT;
    choose["Events/boostedTau_pt"]	= DEFAULT;
    choose["Events/boostedTau_puCorr"]	= DEFAULT;
    choose["Events/boostedTau_rawAntiEle2018"]	= DEFAULT;
    choose["Events/boostedTau_rawAntiEleCat2018"]	= DEFAULT;
    choose["Events/boostedTau_rawIso"]	= DEFAULT;
    choose["Events/boostedTau_rawIsodR03"]	= DEFAULT;
    choose["Events/boostedTau_rawMVAnewDM2017v2"]	= DEFAULT;
    choose["Events/boostedTau_rawMVAoldDM2017v2"]	= DEFAULT;
    choose["Events/boostedTau_rawMVAoldDMdR032017v2"]	= DEFAULT;
    choose["Events/btagWeight_CSVV2"]	= DEFAULT;
    choose["Events/btagWeight_DeepCSVB"]	= DEFAULT;
    choose["Events/event"]	= DEFAULT;
    choose["Events/fixedGridRhoFastjetAll"]	= DEFAULT;
    choose["Events/fixedGridRhoFastjetCentral"]	= DEFAULT;
    choose["Events/fixedGridRhoFastjetCentralCalo"]	= DEFAULT;
    choose["Events/fixedGridRhoFastjetCentralChargedPileUp"]	= DEFAULT;
    choose["Events/fixedGridRhoFastjetCentralNeutral"]	= DEFAULT;
    choose["Events/genTtbarId"]	= DEFAULT;
    choose["Events/genWeight"]	= DEFAULT;
    choose["Events/luminosityBlock"]	= DEFAULT;
    choose["Events/run"]	= DEFAULT;

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
                    // a key selects a branch if it is the start of (or equal to)
                    // the full name 'Events/Jet_pt' or the short name 'Jet_pt'
                    const std::string& full = it->first;
                    std::string::size_type slash = full.rfind('/');
                    std::string shortname = (slash == std::string::npos)
                                            ? full : full.substr(slash + 1);
                    if ( full.compare(0, key.size(), key) == 0 ||
                         shortname.compare(0, key.size(), key) == 0 )
                      it->second = true;
                  }
              }
          }
      }
    successBranches.clear();
    missingBranches.clear();
    std::set<std::string> usedCounters;   // leaf counters of the arrays bound below
    if ( choose["Events/CaloMET_phi"] ) {
      if (input->present("Events/CaloMET_phi")) { input->select("Events/CaloMET_phi", CaloMET_phi); successBranches.push_back("Events/CaloMET_phi"); } else { missingBranches.push_back("Events/CaloMET_phi"); }
    }
    if ( choose["Events/CaloMET_pt"] ) {
      if (input->present("Events/CaloMET_pt")) { input->select("Events/CaloMET_pt", CaloMET_pt); successBranches.push_back("Events/CaloMET_pt"); } else { missingBranches.push_back("Events/CaloMET_pt"); }
    }
    if ( choose["Events/CaloMET_sumEt"] ) {
      if (input->present("Events/CaloMET_sumEt")) { input->select("Events/CaloMET_sumEt", CaloMET_sumEt); successBranches.push_back("Events/CaloMET_sumEt"); } else { missingBranches.push_back("Events/CaloMET_sumEt"); }
    }
    if ( choose["Events/ChsMET_phi"] ) {
      if (input->present("Events/ChsMET_phi")) { input->select("Events/ChsMET_phi", ChsMET_phi); successBranches.push_back("Events/ChsMET_phi"); } else { missingBranches.push_back("Events/ChsMET_phi"); }
    }
    if ( choose["Events/ChsMET_pt"] ) {
      if (input->present("Events/ChsMET_pt")) { input->select("Events/ChsMET_pt", ChsMET_pt); successBranches.push_back("Events/ChsMET_pt"); } else { missingBranches.push_back("Events/ChsMET_pt"); }
    }
    if ( choose["Events/ChsMET_sumEt"] ) {
      if (input->present("Events/ChsMET_sumEt")) { input->select("Events/ChsMET_sumEt", ChsMET_sumEt); successBranches.push_back("Events/ChsMET_sumEt"); } else { missingBranches.push_back("Events/ChsMET_sumEt"); }
    }
    if ( choose["Events/CorrT1METJet_area"] ) {
      if (input->present("Events/CorrT1METJet_area")) { CorrT1METJet_area.resize(83); input->select("Events/CorrT1METJet_area", CorrT1METJet_area); CorrT1METJet_area.clear(); successBranches.push_back("Events/CorrT1METJet_area"); usedCounters.insert("nCorrT1METJet"); } else { missingBranches.push_back("Events/CorrT1METJet_area"); }
    }
    if ( choose["Events/CorrT1METJet_eta"] ) {
      if (input->present("Events/CorrT1METJet_eta")) { CorrT1METJet_eta.resize(83); input->select("Events/CorrT1METJet_eta", CorrT1METJet_eta); CorrT1METJet_eta.clear(); successBranches.push_back("Events/CorrT1METJet_eta"); usedCounters.insert("nCorrT1METJet"); } else { missingBranches.push_back("Events/CorrT1METJet_eta"); }
    }
    if ( choose["Events/CorrT1METJet_muonSubtrFactor"] ) {
      if (input->present("Events/CorrT1METJet_muonSubtrFactor")) { CorrT1METJet_muonSubtrFactor.resize(83); input->select("Events/CorrT1METJet_muonSubtrFactor", CorrT1METJet_muonSubtrFactor); CorrT1METJet_muonSubtrFactor.clear(); successBranches.push_back("Events/CorrT1METJet_muonSubtrFactor"); usedCounters.insert("nCorrT1METJet"); } else { missingBranches.push_back("Events/CorrT1METJet_muonSubtrFactor"); }
    }
    if ( choose["Events/CorrT1METJet_phi"] ) {
      if (input->present("Events/CorrT1METJet_phi")) { CorrT1METJet_phi.resize(83); input->select("Events/CorrT1METJet_phi", CorrT1METJet_phi); CorrT1METJet_phi.clear(); successBranches.push_back("Events/CorrT1METJet_phi"); usedCounters.insert("nCorrT1METJet"); } else { missingBranches.push_back("Events/CorrT1METJet_phi"); }
    }
    if ( choose["Events/CorrT1METJet_rawPt"] ) {
      if (input->present("Events/CorrT1METJet_rawPt")) { CorrT1METJet_rawPt.resize(83); input->select("Events/CorrT1METJet_rawPt", CorrT1METJet_rawPt); CorrT1METJet_rawPt.clear(); successBranches.push_back("Events/CorrT1METJet_rawPt"); usedCounters.insert("nCorrT1METJet"); } else { missingBranches.push_back("Events/CorrT1METJet_rawPt"); }
    }
    if ( choose["Events/DeepMETResolutionTune_phi"] ) {
      if (input->present("Events/DeepMETResolutionTune_phi")) { input->select("Events/DeepMETResolutionTune_phi", DeepMETResolutionTune_phi); successBranches.push_back("Events/DeepMETResolutionTune_phi"); } else { missingBranches.push_back("Events/DeepMETResolutionTune_phi"); }
    }
    if ( choose["Events/DeepMETResolutionTune_pt"] ) {
      if (input->present("Events/DeepMETResolutionTune_pt")) { input->select("Events/DeepMETResolutionTune_pt", DeepMETResolutionTune_pt); successBranches.push_back("Events/DeepMETResolutionTune_pt"); } else { missingBranches.push_back("Events/DeepMETResolutionTune_pt"); }
    }
    if ( choose["Events/DeepMETResponseTune_phi"] ) {
      if (input->present("Events/DeepMETResponseTune_phi")) { input->select("Events/DeepMETResponseTune_phi", DeepMETResponseTune_phi); successBranches.push_back("Events/DeepMETResponseTune_phi"); } else { missingBranches.push_back("Events/DeepMETResponseTune_phi"); }
    }
    if ( choose["Events/DeepMETResponseTune_pt"] ) {
      if (input->present("Events/DeepMETResponseTune_pt")) { input->select("Events/DeepMETResponseTune_pt", DeepMETResponseTune_pt); successBranches.push_back("Events/DeepMETResponseTune_pt"); } else { missingBranches.push_back("Events/DeepMETResponseTune_pt"); }
    }
    if ( choose["Events/Electron_charge"] ) {
      if (input->present("Events/Electron_charge")) { Electron_charge.resize(49); input->select("Events/Electron_charge", Electron_charge); Electron_charge.clear(); successBranches.push_back("Events/Electron_charge"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_charge"); }
    }
    if ( choose["Events/Electron_cleanmask"] ) {
      if (input->present("Events/Electron_cleanmask")) { Electron_cleanmask.resize(49); input->select("Events/Electron_cleanmask", Electron_cleanmask); Electron_cleanmask.clear(); successBranches.push_back("Events/Electron_cleanmask"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_cleanmask"); }
    }
    if ( choose["Events/Electron_convVeto"] ) {
      if (input->present("Events/Electron_convVeto")) { Electron_convVeto.resize(49); input->select("Events/Electron_convVeto", Electron_convVeto); Electron_convVeto.clear(); successBranches.push_back("Events/Electron_convVeto"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_convVeto"); }
    }
    if ( choose["Events/Electron_cutBased"] ) {
      if (input->present("Events/Electron_cutBased")) { Electron_cutBased.resize(49); input->select("Events/Electron_cutBased", Electron_cutBased); Electron_cutBased.clear(); successBranches.push_back("Events/Electron_cutBased"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_cutBased"); }
    }
    if ( choose["Events/Electron_cutBased_HEEP"] ) {
      if (input->present("Events/Electron_cutBased_HEEP")) { Electron_cutBased_HEEP.resize(49); input->select("Events/Electron_cutBased_HEEP", Electron_cutBased_HEEP); Electron_cutBased_HEEP.clear(); successBranches.push_back("Events/Electron_cutBased_HEEP"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_cutBased_HEEP"); }
    }
    if ( choose["Events/Electron_dEscaleDown"] ) {
      if (input->present("Events/Electron_dEscaleDown")) { Electron_dEscaleDown.resize(49); input->select("Events/Electron_dEscaleDown", Electron_dEscaleDown); Electron_dEscaleDown.clear(); successBranches.push_back("Events/Electron_dEscaleDown"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_dEscaleDown"); }
    }
    if ( choose["Events/Electron_dEscaleUp"] ) {
      if (input->present("Events/Electron_dEscaleUp")) { Electron_dEscaleUp.resize(49); input->select("Events/Electron_dEscaleUp", Electron_dEscaleUp); Electron_dEscaleUp.clear(); successBranches.push_back("Events/Electron_dEscaleUp"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_dEscaleUp"); }
    }
    if ( choose["Events/Electron_dEsigmaDown"] ) {
      if (input->present("Events/Electron_dEsigmaDown")) { Electron_dEsigmaDown.resize(49); input->select("Events/Electron_dEsigmaDown", Electron_dEsigmaDown); Electron_dEsigmaDown.clear(); successBranches.push_back("Events/Electron_dEsigmaDown"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_dEsigmaDown"); }
    }
    if ( choose["Events/Electron_dEsigmaUp"] ) {
      if (input->present("Events/Electron_dEsigmaUp")) { Electron_dEsigmaUp.resize(49); input->select("Events/Electron_dEsigmaUp", Electron_dEsigmaUp); Electron_dEsigmaUp.clear(); successBranches.push_back("Events/Electron_dEsigmaUp"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_dEsigmaUp"); }
    }
    if ( choose["Events/Electron_deltaEtaSC"] ) {
      if (input->present("Events/Electron_deltaEtaSC")) { Electron_deltaEtaSC.resize(49); input->select("Events/Electron_deltaEtaSC", Electron_deltaEtaSC); Electron_deltaEtaSC.clear(); successBranches.push_back("Events/Electron_deltaEtaSC"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_deltaEtaSC"); }
    }
    if ( choose["Events/Electron_dr03EcalRecHitSumEt"] ) {
      if (input->present("Events/Electron_dr03EcalRecHitSumEt")) { Electron_dr03EcalRecHitSumEt.resize(49); input->select("Events/Electron_dr03EcalRecHitSumEt", Electron_dr03EcalRecHitSumEt); Electron_dr03EcalRecHitSumEt.clear(); successBranches.push_back("Events/Electron_dr03EcalRecHitSumEt"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_dr03EcalRecHitSumEt"); }
    }
    if ( choose["Events/Electron_dr03HcalDepth1TowerSumEt"] ) {
      if (input->present("Events/Electron_dr03HcalDepth1TowerSumEt")) { Electron_dr03HcalDepth1TowerSumEt.resize(49); input->select("Events/Electron_dr03HcalDepth1TowerSumEt", Electron_dr03HcalDepth1TowerSumEt); Electron_dr03HcalDepth1TowerSumEt.clear(); successBranches.push_back("Events/Electron_dr03HcalDepth1TowerSumEt"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_dr03HcalDepth1TowerSumEt"); }
    }
    if ( choose["Events/Electron_dr03TkSumPt"] ) {
      if (input->present("Events/Electron_dr03TkSumPt")) { Electron_dr03TkSumPt.resize(49); input->select("Events/Electron_dr03TkSumPt", Electron_dr03TkSumPt); Electron_dr03TkSumPt.clear(); successBranches.push_back("Events/Electron_dr03TkSumPt"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_dr03TkSumPt"); }
    }
    if ( choose["Events/Electron_dr03TkSumPtHEEP"] ) {
      if (input->present("Events/Electron_dr03TkSumPtHEEP")) { Electron_dr03TkSumPtHEEP.resize(49); input->select("Events/Electron_dr03TkSumPtHEEP", Electron_dr03TkSumPtHEEP); Electron_dr03TkSumPtHEEP.clear(); successBranches.push_back("Events/Electron_dr03TkSumPtHEEP"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_dr03TkSumPtHEEP"); }
    }
    if ( choose["Events/Electron_dxy"] ) {
      if (input->present("Events/Electron_dxy")) { Electron_dxy.resize(49); input->select("Events/Electron_dxy", Electron_dxy); Electron_dxy.clear(); successBranches.push_back("Events/Electron_dxy"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_dxy"); }
    }
    if ( choose["Events/Electron_dxyErr"] ) {
      if (input->present("Events/Electron_dxyErr")) { Electron_dxyErr.resize(49); input->select("Events/Electron_dxyErr", Electron_dxyErr); Electron_dxyErr.clear(); successBranches.push_back("Events/Electron_dxyErr"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_dxyErr"); }
    }
    if ( choose["Events/Electron_dz"] ) {
      if (input->present("Events/Electron_dz")) { Electron_dz.resize(49); input->select("Events/Electron_dz", Electron_dz); Electron_dz.clear(); successBranches.push_back("Events/Electron_dz"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_dz"); }
    }
    if ( choose["Events/Electron_dzErr"] ) {
      if (input->present("Events/Electron_dzErr")) { Electron_dzErr.resize(49); input->select("Events/Electron_dzErr", Electron_dzErr); Electron_dzErr.clear(); successBranches.push_back("Events/Electron_dzErr"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_dzErr"); }
    }
    if ( choose["Events/Electron_eCorr"] ) {
      if (input->present("Events/Electron_eCorr")) { Electron_eCorr.resize(49); input->select("Events/Electron_eCorr", Electron_eCorr); Electron_eCorr.clear(); successBranches.push_back("Events/Electron_eCorr"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_eCorr"); }
    }
    if ( choose["Events/Electron_eInvMinusPInv"] ) {
      if (input->present("Events/Electron_eInvMinusPInv")) { Electron_eInvMinusPInv.resize(49); input->select("Events/Electron_eInvMinusPInv", Electron_eInvMinusPInv); Electron_eInvMinusPInv.clear(); successBranches.push_back("Events/Electron_eInvMinusPInv"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_eInvMinusPInv"); }
    }
    if ( choose["Events/Electron_energyErr"] ) {
      if (input->present("Events/Electron_energyErr")) { Electron_energyErr.resize(49); input->select("Events/Electron_energyErr", Electron_energyErr); Electron_energyErr.clear(); successBranches.push_back("Events/Electron_energyErr"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_energyErr"); }
    }
    if ( choose["Events/Electron_eta"] ) {
      if (input->present("Events/Electron_eta")) { Electron_eta.resize(49); input->select("Events/Electron_eta", Electron_eta); Electron_eta.clear(); successBranches.push_back("Events/Electron_eta"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_eta"); }
    }
    if ( choose["Events/Electron_genPartFlav"] ) {
      if (input->present("Events/Electron_genPartFlav")) { Electron_genPartFlav.resize(49); input->select("Events/Electron_genPartFlav", Electron_genPartFlav); Electron_genPartFlav.clear(); successBranches.push_back("Events/Electron_genPartFlav"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_genPartFlav"); }
    }
    if ( choose["Events/Electron_genPartIdx"] ) {
      if (input->present("Events/Electron_genPartIdx")) { Electron_genPartIdx.resize(49); input->select("Events/Electron_genPartIdx", Electron_genPartIdx); Electron_genPartIdx.clear(); successBranches.push_back("Events/Electron_genPartIdx"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_genPartIdx"); }
    }
    if ( choose["Events/Electron_hoe"] ) {
      if (input->present("Events/Electron_hoe")) { Electron_hoe.resize(49); input->select("Events/Electron_hoe", Electron_hoe); Electron_hoe.clear(); successBranches.push_back("Events/Electron_hoe"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_hoe"); }
    }
    if ( choose["Events/Electron_ip3d"] ) {
      if (input->present("Events/Electron_ip3d")) { Electron_ip3d.resize(49); input->select("Events/Electron_ip3d", Electron_ip3d); Electron_ip3d.clear(); successBranches.push_back("Events/Electron_ip3d"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_ip3d"); }
    }
    if ( choose["Events/Electron_isEB"] ) {
      if (input->present("Events/Electron_isEB")) { Electron_isEB.resize(49); input->select("Events/Electron_isEB", Electron_isEB); Electron_isEB.clear(); successBranches.push_back("Events/Electron_isEB"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_isEB"); }
    }
    if ( choose["Events/Electron_isPFcand"] ) {
      if (input->present("Events/Electron_isPFcand")) { Electron_isPFcand.resize(49); input->select("Events/Electron_isPFcand", Electron_isPFcand); Electron_isPFcand.clear(); successBranches.push_back("Events/Electron_isPFcand"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_isPFcand"); }
    }
    if ( choose["Events/Electron_jetIdx"] ) {
      if (input->present("Events/Electron_jetIdx")) { Electron_jetIdx.resize(49); input->select("Events/Electron_jetIdx", Electron_jetIdx); Electron_jetIdx.clear(); successBranches.push_back("Events/Electron_jetIdx"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_jetIdx"); }
    }
    if ( choose["Events/Electron_jetNDauCharged"] ) {
      if (input->present("Events/Electron_jetNDauCharged")) { Electron_jetNDauCharged.resize(49); input->select("Events/Electron_jetNDauCharged", Electron_jetNDauCharged); Electron_jetNDauCharged.clear(); successBranches.push_back("Events/Electron_jetNDauCharged"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_jetNDauCharged"); }
    }
    if ( choose["Events/Electron_jetPtRelv2"] ) {
      if (input->present("Events/Electron_jetPtRelv2")) { Electron_jetPtRelv2.resize(49); input->select("Events/Electron_jetPtRelv2", Electron_jetPtRelv2); Electron_jetPtRelv2.clear(); successBranches.push_back("Events/Electron_jetPtRelv2"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_jetPtRelv2"); }
    }
    if ( choose["Events/Electron_jetRelIso"] ) {
      if (input->present("Events/Electron_jetRelIso")) { Electron_jetRelIso.resize(49); input->select("Events/Electron_jetRelIso", Electron_jetRelIso); Electron_jetRelIso.clear(); successBranches.push_back("Events/Electron_jetRelIso"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_jetRelIso"); }
    }
    if ( choose["Events/Electron_lostHits"] ) {
      if (input->present("Events/Electron_lostHits")) { Electron_lostHits.resize(49); input->select("Events/Electron_lostHits", Electron_lostHits); Electron_lostHits.clear(); successBranches.push_back("Events/Electron_lostHits"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_lostHits"); }
    }
    if ( choose["Events/Electron_mass"] ) {
      if (input->present("Events/Electron_mass")) { Electron_mass.resize(49); input->select("Events/Electron_mass", Electron_mass); Electron_mass.clear(); successBranches.push_back("Events/Electron_mass"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_mass"); }
    }
    if ( choose["Events/Electron_miniPFRelIso_all"] ) {
      if (input->present("Events/Electron_miniPFRelIso_all")) { Electron_miniPFRelIso_all.resize(49); input->select("Events/Electron_miniPFRelIso_all", Electron_miniPFRelIso_all); Electron_miniPFRelIso_all.clear(); successBranches.push_back("Events/Electron_miniPFRelIso_all"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_miniPFRelIso_all"); }
    }
    if ( choose["Events/Electron_miniPFRelIso_chg"] ) {
      if (input->present("Events/Electron_miniPFRelIso_chg")) { Electron_miniPFRelIso_chg.resize(49); input->select("Events/Electron_miniPFRelIso_chg", Electron_miniPFRelIso_chg); Electron_miniPFRelIso_chg.clear(); successBranches.push_back("Events/Electron_miniPFRelIso_chg"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_miniPFRelIso_chg"); }
    }
    if ( choose["Events/Electron_mvaFall17V2Iso"] ) {
      if (input->present("Events/Electron_mvaFall17V2Iso")) { Electron_mvaFall17V2Iso.resize(49); input->select("Events/Electron_mvaFall17V2Iso", Electron_mvaFall17V2Iso); Electron_mvaFall17V2Iso.clear(); successBranches.push_back("Events/Electron_mvaFall17V2Iso"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_mvaFall17V2Iso"); }
    }
    if ( choose["Events/Electron_mvaFall17V2Iso_WP80"] ) {
      if (input->present("Events/Electron_mvaFall17V2Iso_WP80")) { Electron_mvaFall17V2Iso_WP80.resize(49); input->select("Events/Electron_mvaFall17V2Iso_WP80", Electron_mvaFall17V2Iso_WP80); Electron_mvaFall17V2Iso_WP80.clear(); successBranches.push_back("Events/Electron_mvaFall17V2Iso_WP80"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_mvaFall17V2Iso_WP80"); }
    }
    if ( choose["Events/Electron_mvaFall17V2Iso_WP90"] ) {
      if (input->present("Events/Electron_mvaFall17V2Iso_WP90")) { Electron_mvaFall17V2Iso_WP90.resize(49); input->select("Events/Electron_mvaFall17V2Iso_WP90", Electron_mvaFall17V2Iso_WP90); Electron_mvaFall17V2Iso_WP90.clear(); successBranches.push_back("Events/Electron_mvaFall17V2Iso_WP90"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_mvaFall17V2Iso_WP90"); }
    }
    if ( choose["Events/Electron_mvaFall17V2Iso_WPL"] ) {
      if (input->present("Events/Electron_mvaFall17V2Iso_WPL")) { Electron_mvaFall17V2Iso_WPL.resize(49); input->select("Events/Electron_mvaFall17V2Iso_WPL", Electron_mvaFall17V2Iso_WPL); Electron_mvaFall17V2Iso_WPL.clear(); successBranches.push_back("Events/Electron_mvaFall17V2Iso_WPL"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_mvaFall17V2Iso_WPL"); }
    }
    if ( choose["Events/Electron_mvaFall17V2noIso"] ) {
      if (input->present("Events/Electron_mvaFall17V2noIso")) { Electron_mvaFall17V2noIso.resize(49); input->select("Events/Electron_mvaFall17V2noIso", Electron_mvaFall17V2noIso); Electron_mvaFall17V2noIso.clear(); successBranches.push_back("Events/Electron_mvaFall17V2noIso"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_mvaFall17V2noIso"); }
    }
    if ( choose["Events/Electron_mvaFall17V2noIso_WP80"] ) {
      if (input->present("Events/Electron_mvaFall17V2noIso_WP80")) { Electron_mvaFall17V2noIso_WP80.resize(49); input->select("Events/Electron_mvaFall17V2noIso_WP80", Electron_mvaFall17V2noIso_WP80); Electron_mvaFall17V2noIso_WP80.clear(); successBranches.push_back("Events/Electron_mvaFall17V2noIso_WP80"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_mvaFall17V2noIso_WP80"); }
    }
    if ( choose["Events/Electron_mvaFall17V2noIso_WP90"] ) {
      if (input->present("Events/Electron_mvaFall17V2noIso_WP90")) { Electron_mvaFall17V2noIso_WP90.resize(49); input->select("Events/Electron_mvaFall17V2noIso_WP90", Electron_mvaFall17V2noIso_WP90); Electron_mvaFall17V2noIso_WP90.clear(); successBranches.push_back("Events/Electron_mvaFall17V2noIso_WP90"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_mvaFall17V2noIso_WP90"); }
    }
    if ( choose["Events/Electron_mvaFall17V2noIso_WPL"] ) {
      if (input->present("Events/Electron_mvaFall17V2noIso_WPL")) { Electron_mvaFall17V2noIso_WPL.resize(49); input->select("Events/Electron_mvaFall17V2noIso_WPL", Electron_mvaFall17V2noIso_WPL); Electron_mvaFall17V2noIso_WPL.clear(); successBranches.push_back("Events/Electron_mvaFall17V2noIso_WPL"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_mvaFall17V2noIso_WPL"); }
    }
    if ( choose["Events/Electron_mvaIso"] ) {
      if (input->present("Events/Electron_mvaIso")) { Electron_mvaIso.resize(49); input->select("Events/Electron_mvaIso", Electron_mvaIso); Electron_mvaIso.clear(); successBranches.push_back("Events/Electron_mvaIso"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_mvaIso"); }
    }
    if ( choose["Events/Electron_mvaIso_WP80"] ) {
      if (input->present("Events/Electron_mvaIso_WP80")) { Electron_mvaIso_WP80.resize(49); input->select("Events/Electron_mvaIso_WP80", Electron_mvaIso_WP80); Electron_mvaIso_WP80.clear(); successBranches.push_back("Events/Electron_mvaIso_WP80"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_mvaIso_WP80"); }
    }
    if ( choose["Events/Electron_mvaIso_WP90"] ) {
      if (input->present("Events/Electron_mvaIso_WP90")) { Electron_mvaIso_WP90.resize(49); input->select("Events/Electron_mvaIso_WP90", Electron_mvaIso_WP90); Electron_mvaIso_WP90.clear(); successBranches.push_back("Events/Electron_mvaIso_WP90"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_mvaIso_WP90"); }
    }
    if ( choose["Events/Electron_mvaIso_WPL"] ) {
      if (input->present("Events/Electron_mvaIso_WPL")) { Electron_mvaIso_WPL.resize(49); input->select("Events/Electron_mvaIso_WPL", Electron_mvaIso_WPL); Electron_mvaIso_WPL.clear(); successBranches.push_back("Events/Electron_mvaIso_WPL"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_mvaIso_WPL"); }
    }
    if ( choose["Events/Electron_mvaNoIso"] ) {
      if (input->present("Events/Electron_mvaNoIso")) { Electron_mvaNoIso.resize(49); input->select("Events/Electron_mvaNoIso", Electron_mvaNoIso); Electron_mvaNoIso.clear(); successBranches.push_back("Events/Electron_mvaNoIso"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_mvaNoIso"); }
    }
    if ( choose["Events/Electron_mvaNoIso_WP80"] ) {
      if (input->present("Events/Electron_mvaNoIso_WP80")) { Electron_mvaNoIso_WP80.resize(49); input->select("Events/Electron_mvaNoIso_WP80", Electron_mvaNoIso_WP80); Electron_mvaNoIso_WP80.clear(); successBranches.push_back("Events/Electron_mvaNoIso_WP80"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_mvaNoIso_WP80"); }
    }
    if ( choose["Events/Electron_mvaNoIso_WP90"] ) {
      if (input->present("Events/Electron_mvaNoIso_WP90")) { Electron_mvaNoIso_WP90.resize(49); input->select("Events/Electron_mvaNoIso_WP90", Electron_mvaNoIso_WP90); Electron_mvaNoIso_WP90.clear(); successBranches.push_back("Events/Electron_mvaNoIso_WP90"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_mvaNoIso_WP90"); }
    }
    if ( choose["Events/Electron_mvaNoIso_WPL"] ) {
      if (input->present("Events/Electron_mvaNoIso_WPL")) { Electron_mvaNoIso_WPL.resize(49); input->select("Events/Electron_mvaNoIso_WPL", Electron_mvaNoIso_WPL); Electron_mvaNoIso_WPL.clear(); successBranches.push_back("Events/Electron_mvaNoIso_WPL"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_mvaNoIso_WPL"); }
    }
    if ( choose["Events/Electron_mvaTTH"] ) {
      if (input->present("Events/Electron_mvaTTH")) { Electron_mvaTTH.resize(49); input->select("Events/Electron_mvaTTH", Electron_mvaTTH); Electron_mvaTTH.clear(); successBranches.push_back("Events/Electron_mvaTTH"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_mvaTTH"); }
    }
    if ( choose["Events/Electron_pdgId"] ) {
      if (input->present("Events/Electron_pdgId")) { Electron_pdgId.resize(49); input->select("Events/Electron_pdgId", Electron_pdgId); Electron_pdgId.clear(); successBranches.push_back("Events/Electron_pdgId"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_pdgId"); }
    }
    if ( choose["Events/Electron_pfRelIso03_all"] ) {
      if (input->present("Events/Electron_pfRelIso03_all")) { Electron_pfRelIso03_all.resize(49); input->select("Events/Electron_pfRelIso03_all", Electron_pfRelIso03_all); Electron_pfRelIso03_all.clear(); successBranches.push_back("Events/Electron_pfRelIso03_all"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_pfRelIso03_all"); }
    }
    if ( choose["Events/Electron_pfRelIso03_chg"] ) {
      if (input->present("Events/Electron_pfRelIso03_chg")) { Electron_pfRelIso03_chg.resize(49); input->select("Events/Electron_pfRelIso03_chg", Electron_pfRelIso03_chg); Electron_pfRelIso03_chg.clear(); successBranches.push_back("Events/Electron_pfRelIso03_chg"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_pfRelIso03_chg"); }
    }
    if ( choose["Events/Electron_pfRelIso04_all"] ) {
      if (input->present("Events/Electron_pfRelIso04_all")) { Electron_pfRelIso04_all.resize(49); input->select("Events/Electron_pfRelIso04_all", Electron_pfRelIso04_all); Electron_pfRelIso04_all.clear(); successBranches.push_back("Events/Electron_pfRelIso04_all"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_pfRelIso04_all"); }
    }
    if ( choose["Events/Electron_phi"] ) {
      if (input->present("Events/Electron_phi")) { Electron_phi.resize(49); input->select("Events/Electron_phi", Electron_phi); Electron_phi.clear(); successBranches.push_back("Events/Electron_phi"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_phi"); }
    }
    if ( choose["Events/Electron_photonIdx"] ) {
      if (input->present("Events/Electron_photonIdx")) { Electron_photonIdx.resize(49); input->select("Events/Electron_photonIdx", Electron_photonIdx); Electron_photonIdx.clear(); successBranches.push_back("Events/Electron_photonIdx"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_photonIdx"); }
    }
    if ( choose["Events/Electron_promptMVA"] ) {
      if (input->present("Events/Electron_promptMVA")) { Electron_promptMVA.resize(49); input->select("Events/Electron_promptMVA", Electron_promptMVA); Electron_promptMVA.clear(); successBranches.push_back("Events/Electron_promptMVA"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_promptMVA"); }
    }
    if ( choose["Events/Electron_pt"] ) {
      if (input->present("Events/Electron_pt")) { Electron_pt.resize(49); input->select("Events/Electron_pt", Electron_pt); Electron_pt.clear(); successBranches.push_back("Events/Electron_pt"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_pt"); }
    }
    if ( choose["Events/Electron_r9"] ) {
      if (input->present("Events/Electron_r9")) { Electron_r9.resize(49); input->select("Events/Electron_r9", Electron_r9); Electron_r9.clear(); successBranches.push_back("Events/Electron_r9"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_r9"); }
    }
    if ( choose["Events/Electron_scEtOverPt"] ) {
      if (input->present("Events/Electron_scEtOverPt")) { Electron_scEtOverPt.resize(49); input->select("Events/Electron_scEtOverPt", Electron_scEtOverPt); Electron_scEtOverPt.clear(); successBranches.push_back("Events/Electron_scEtOverPt"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_scEtOverPt"); }
    }
    if ( choose["Events/Electron_seedGain"] ) {
      if (input->present("Events/Electron_seedGain")) { Electron_seedGain.resize(49); input->select("Events/Electron_seedGain", Electron_seedGain); Electron_seedGain.clear(); successBranches.push_back("Events/Electron_seedGain"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_seedGain"); }
    }
    if ( choose["Events/Electron_sieie"] ) {
      if (input->present("Events/Electron_sieie")) { Electron_sieie.resize(49); input->select("Events/Electron_sieie", Electron_sieie); Electron_sieie.clear(); successBranches.push_back("Events/Electron_sieie"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_sieie"); }
    }
    if ( choose["Events/Electron_sip3d"] ) {
      if (input->present("Events/Electron_sip3d")) { Electron_sip3d.resize(49); input->select("Events/Electron_sip3d", Electron_sip3d); Electron_sip3d.clear(); successBranches.push_back("Events/Electron_sip3d"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_sip3d"); }
    }
    if ( choose["Events/Electron_superclusterEta"] ) {
      if (input->present("Events/Electron_superclusterEta")) { Electron_superclusterEta.resize(49); input->select("Events/Electron_superclusterEta", Electron_superclusterEta); Electron_superclusterEta.clear(); successBranches.push_back("Events/Electron_superclusterEta"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_superclusterEta"); }
    }
    if ( choose["Events/Electron_tightCharge"] ) {
      if (input->present("Events/Electron_tightCharge")) { Electron_tightCharge.resize(49); input->select("Events/Electron_tightCharge", Electron_tightCharge); Electron_tightCharge.clear(); successBranches.push_back("Events/Electron_tightCharge"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_tightCharge"); }
    }
    if ( choose["Events/Electron_vidNestedWPBitmap"] ) {
      if (input->present("Events/Electron_vidNestedWPBitmap")) { Electron_vidNestedWPBitmap.resize(49); input->select("Events/Electron_vidNestedWPBitmap", Electron_vidNestedWPBitmap); Electron_vidNestedWPBitmap.clear(); successBranches.push_back("Events/Electron_vidNestedWPBitmap"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_vidNestedWPBitmap"); }
    }
    if ( choose["Events/Electron_vidNestedWPBitmapHEEP"] ) {
      if (input->present("Events/Electron_vidNestedWPBitmapHEEP")) { Electron_vidNestedWPBitmapHEEP.resize(49); input->select("Events/Electron_vidNestedWPBitmapHEEP", Electron_vidNestedWPBitmapHEEP); Electron_vidNestedWPBitmapHEEP.clear(); successBranches.push_back("Events/Electron_vidNestedWPBitmapHEEP"); usedCounters.insert("nElectron"); } else { missingBranches.push_back("Events/Electron_vidNestedWPBitmapHEEP"); }
    }
    if ( choose["Events/FatJet_area"] ) {
      if (input->present("Events/FatJet_area")) { FatJet_area.resize(43); input->select("Events/FatJet_area", FatJet_area); FatJet_area.clear(); successBranches.push_back("Events/FatJet_area"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_area"); }
    }
    if ( choose["Events/FatJet_btagCSVV2"] ) {
      if (input->present("Events/FatJet_btagCSVV2")) { FatJet_btagCSVV2.resize(43); input->select("Events/FatJet_btagCSVV2", FatJet_btagCSVV2); FatJet_btagCSVV2.clear(); successBranches.push_back("Events/FatJet_btagCSVV2"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_btagCSVV2"); }
    }
    if ( choose["Events/FatJet_btagDDBvLV2"] ) {
      if (input->present("Events/FatJet_btagDDBvLV2")) { FatJet_btagDDBvLV2.resize(43); input->select("Events/FatJet_btagDDBvLV2", FatJet_btagDDBvLV2); FatJet_btagDDBvLV2.clear(); successBranches.push_back("Events/FatJet_btagDDBvLV2"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_btagDDBvLV2"); }
    }
    if ( choose["Events/FatJet_btagDDCvBV2"] ) {
      if (input->present("Events/FatJet_btagDDCvBV2")) { FatJet_btagDDCvBV2.resize(43); input->select("Events/FatJet_btagDDCvBV2", FatJet_btagDDCvBV2); FatJet_btagDDCvBV2.clear(); successBranches.push_back("Events/FatJet_btagDDCvBV2"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_btagDDCvBV2"); }
    }
    if ( choose["Events/FatJet_btagDDCvLV2"] ) {
      if (input->present("Events/FatJet_btagDDCvLV2")) { FatJet_btagDDCvLV2.resize(43); input->select("Events/FatJet_btagDDCvLV2", FatJet_btagDDCvLV2); FatJet_btagDDCvLV2.clear(); successBranches.push_back("Events/FatJet_btagDDCvLV2"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_btagDDCvLV2"); }
    }
    if ( choose["Events/FatJet_btagDeepB"] ) {
      if (input->present("Events/FatJet_btagDeepB")) { FatJet_btagDeepB.resize(43); input->select("Events/FatJet_btagDeepB", FatJet_btagDeepB); FatJet_btagDeepB.clear(); successBranches.push_back("Events/FatJet_btagDeepB"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_btagDeepB"); }
    }
    if ( choose["Events/FatJet_btagHbb"] ) {
      if (input->present("Events/FatJet_btagHbb")) { FatJet_btagHbb.resize(43); input->select("Events/FatJet_btagHbb", FatJet_btagHbb); FatJet_btagHbb.clear(); successBranches.push_back("Events/FatJet_btagHbb"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_btagHbb"); }
    }
    if ( choose["Events/FatJet_deepTagMD_H4qvsQCD"] ) {
      if (input->present("Events/FatJet_deepTagMD_H4qvsQCD")) { FatJet_deepTagMD_H4qvsQCD.resize(43); input->select("Events/FatJet_deepTagMD_H4qvsQCD", FatJet_deepTagMD_H4qvsQCD); FatJet_deepTagMD_H4qvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTagMD_H4qvsQCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_H4qvsQCD"); }
    }
    if ( choose["Events/FatJet_deepTagMD_HbbvsQCD"] ) {
      if (input->present("Events/FatJet_deepTagMD_HbbvsQCD")) { FatJet_deepTagMD_HbbvsQCD.resize(43); input->select("Events/FatJet_deepTagMD_HbbvsQCD", FatJet_deepTagMD_HbbvsQCD); FatJet_deepTagMD_HbbvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTagMD_HbbvsQCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_HbbvsQCD"); }
    }
    if ( choose["Events/FatJet_deepTagMD_TvsQCD"] ) {
      if (input->present("Events/FatJet_deepTagMD_TvsQCD")) { FatJet_deepTagMD_TvsQCD.resize(43); input->select("Events/FatJet_deepTagMD_TvsQCD", FatJet_deepTagMD_TvsQCD); FatJet_deepTagMD_TvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTagMD_TvsQCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_TvsQCD"); }
    }
    if ( choose["Events/FatJet_deepTagMD_WvsQCD"] ) {
      if (input->present("Events/FatJet_deepTagMD_WvsQCD")) { FatJet_deepTagMD_WvsQCD.resize(43); input->select("Events/FatJet_deepTagMD_WvsQCD", FatJet_deepTagMD_WvsQCD); FatJet_deepTagMD_WvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTagMD_WvsQCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_WvsQCD"); }
    }
    if ( choose["Events/FatJet_deepTagMD_ZHbbvsQCD"] ) {
      if (input->present("Events/FatJet_deepTagMD_ZHbbvsQCD")) { FatJet_deepTagMD_ZHbbvsQCD.resize(43); input->select("Events/FatJet_deepTagMD_ZHbbvsQCD", FatJet_deepTagMD_ZHbbvsQCD); FatJet_deepTagMD_ZHbbvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTagMD_ZHbbvsQCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_ZHbbvsQCD"); }
    }
    if ( choose["Events/FatJet_deepTagMD_ZHccvsQCD"] ) {
      if (input->present("Events/FatJet_deepTagMD_ZHccvsQCD")) { FatJet_deepTagMD_ZHccvsQCD.resize(43); input->select("Events/FatJet_deepTagMD_ZHccvsQCD", FatJet_deepTagMD_ZHccvsQCD); FatJet_deepTagMD_ZHccvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTagMD_ZHccvsQCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_ZHccvsQCD"); }
    }
    if ( choose["Events/FatJet_deepTagMD_ZbbvsQCD"] ) {
      if (input->present("Events/FatJet_deepTagMD_ZbbvsQCD")) { FatJet_deepTagMD_ZbbvsQCD.resize(43); input->select("Events/FatJet_deepTagMD_ZbbvsQCD", FatJet_deepTagMD_ZbbvsQCD); FatJet_deepTagMD_ZbbvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTagMD_ZbbvsQCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_ZbbvsQCD"); }
    }
    if ( choose["Events/FatJet_deepTagMD_ZvsQCD"] ) {
      if (input->present("Events/FatJet_deepTagMD_ZvsQCD")) { FatJet_deepTagMD_ZvsQCD.resize(43); input->select("Events/FatJet_deepTagMD_ZvsQCD", FatJet_deepTagMD_ZvsQCD); FatJet_deepTagMD_ZvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTagMD_ZvsQCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_ZvsQCD"); }
    }
    if ( choose["Events/FatJet_deepTagMD_bbvsLight"] ) {
      if (input->present("Events/FatJet_deepTagMD_bbvsLight")) { FatJet_deepTagMD_bbvsLight.resize(43); input->select("Events/FatJet_deepTagMD_bbvsLight", FatJet_deepTagMD_bbvsLight); FatJet_deepTagMD_bbvsLight.clear(); successBranches.push_back("Events/FatJet_deepTagMD_bbvsLight"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_bbvsLight"); }
    }
    if ( choose["Events/FatJet_deepTagMD_ccvsLight"] ) {
      if (input->present("Events/FatJet_deepTagMD_ccvsLight")) { FatJet_deepTagMD_ccvsLight.resize(43); input->select("Events/FatJet_deepTagMD_ccvsLight", FatJet_deepTagMD_ccvsLight); FatJet_deepTagMD_ccvsLight.clear(); successBranches.push_back("Events/FatJet_deepTagMD_ccvsLight"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_deepTagMD_ccvsLight"); }
    }
    if ( choose["Events/FatJet_deepTag_H"] ) {
      if (input->present("Events/FatJet_deepTag_H")) { FatJet_deepTag_H.resize(43); input->select("Events/FatJet_deepTag_H", FatJet_deepTag_H); FatJet_deepTag_H.clear(); successBranches.push_back("Events/FatJet_deepTag_H"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_deepTag_H"); }
    }
    if ( choose["Events/FatJet_deepTag_QCD"] ) {
      if (input->present("Events/FatJet_deepTag_QCD")) { FatJet_deepTag_QCD.resize(43); input->select("Events/FatJet_deepTag_QCD", FatJet_deepTag_QCD); FatJet_deepTag_QCD.clear(); successBranches.push_back("Events/FatJet_deepTag_QCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_deepTag_QCD"); }
    }
    if ( choose["Events/FatJet_deepTag_QCDothers"] ) {
      if (input->present("Events/FatJet_deepTag_QCDothers")) { FatJet_deepTag_QCDothers.resize(43); input->select("Events/FatJet_deepTag_QCDothers", FatJet_deepTag_QCDothers); FatJet_deepTag_QCDothers.clear(); successBranches.push_back("Events/FatJet_deepTag_QCDothers"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_deepTag_QCDothers"); }
    }
    if ( choose["Events/FatJet_deepTag_TvsQCD"] ) {
      if (input->present("Events/FatJet_deepTag_TvsQCD")) { FatJet_deepTag_TvsQCD.resize(43); input->select("Events/FatJet_deepTag_TvsQCD", FatJet_deepTag_TvsQCD); FatJet_deepTag_TvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTag_TvsQCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_deepTag_TvsQCD"); }
    }
    if ( choose["Events/FatJet_deepTag_WvsQCD"] ) {
      if (input->present("Events/FatJet_deepTag_WvsQCD")) { FatJet_deepTag_WvsQCD.resize(43); input->select("Events/FatJet_deepTag_WvsQCD", FatJet_deepTag_WvsQCD); FatJet_deepTag_WvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTag_WvsQCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_deepTag_WvsQCD"); }
    }
    if ( choose["Events/FatJet_deepTag_ZvsQCD"] ) {
      if (input->present("Events/FatJet_deepTag_ZvsQCD")) { FatJet_deepTag_ZvsQCD.resize(43); input->select("Events/FatJet_deepTag_ZvsQCD", FatJet_deepTag_ZvsQCD); FatJet_deepTag_ZvsQCD.clear(); successBranches.push_back("Events/FatJet_deepTag_ZvsQCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_deepTag_ZvsQCD"); }
    }
    if ( choose["Events/FatJet_electronIdx3SJ"] ) {
      if (input->present("Events/FatJet_electronIdx3SJ")) { FatJet_electronIdx3SJ.resize(43); input->select("Events/FatJet_electronIdx3SJ", FatJet_electronIdx3SJ); FatJet_electronIdx3SJ.clear(); successBranches.push_back("Events/FatJet_electronIdx3SJ"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_electronIdx3SJ"); }
    }
    if ( choose["Events/FatJet_eta"] ) {
      if (input->present("Events/FatJet_eta")) { FatJet_eta.resize(43); input->select("Events/FatJet_eta", FatJet_eta); FatJet_eta.clear(); successBranches.push_back("Events/FatJet_eta"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_eta"); }
    }
    if ( choose["Events/FatJet_genJetAK8Idx"] ) {
      if (input->present("Events/FatJet_genJetAK8Idx")) { FatJet_genJetAK8Idx.resize(43); input->select("Events/FatJet_genJetAK8Idx", FatJet_genJetAK8Idx); FatJet_genJetAK8Idx.clear(); successBranches.push_back("Events/FatJet_genJetAK8Idx"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_genJetAK8Idx"); }
    }
    if ( choose["Events/FatJet_hadronFlavour"] ) {
      if (input->present("Events/FatJet_hadronFlavour")) { FatJet_hadronFlavour.resize(43); input->select("Events/FatJet_hadronFlavour", FatJet_hadronFlavour); FatJet_hadronFlavour.clear(); successBranches.push_back("Events/FatJet_hadronFlavour"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_hadronFlavour"); }
    }
    if ( choose["Events/FatJet_jetId"] ) {
      if (input->present("Events/FatJet_jetId")) { FatJet_jetId.resize(43); input->select("Events/FatJet_jetId", FatJet_jetId); FatJet_jetId.clear(); successBranches.push_back("Events/FatJet_jetId"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_jetId"); }
    }
    if ( choose["Events/FatJet_lsf3"] ) {
      if (input->present("Events/FatJet_lsf3")) { FatJet_lsf3.resize(43); input->select("Events/FatJet_lsf3", FatJet_lsf3); FatJet_lsf3.clear(); successBranches.push_back("Events/FatJet_lsf3"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_lsf3"); }
    }
    if ( choose["Events/FatJet_mass"] ) {
      if (input->present("Events/FatJet_mass")) { FatJet_mass.resize(43); input->select("Events/FatJet_mass", FatJet_mass); FatJet_mass.clear(); successBranches.push_back("Events/FatJet_mass"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_mass"); }
    }
    if ( choose["Events/FatJet_msoftdrop"] ) {
      if (input->present("Events/FatJet_msoftdrop")) { FatJet_msoftdrop.resize(43); input->select("Events/FatJet_msoftdrop", FatJet_msoftdrop); FatJet_msoftdrop.clear(); successBranches.push_back("Events/FatJet_msoftdrop"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_msoftdrop"); }
    }
    if ( choose["Events/FatJet_muonIdx3SJ"] ) {
      if (input->present("Events/FatJet_muonIdx3SJ")) { FatJet_muonIdx3SJ.resize(43); input->select("Events/FatJet_muonIdx3SJ", FatJet_muonIdx3SJ); FatJet_muonIdx3SJ.clear(); successBranches.push_back("Events/FatJet_muonIdx3SJ"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_muonIdx3SJ"); }
    }
    if ( choose["Events/FatJet_n2b1"] ) {
      if (input->present("Events/FatJet_n2b1")) { FatJet_n2b1.resize(43); input->select("Events/FatJet_n2b1", FatJet_n2b1); FatJet_n2b1.clear(); successBranches.push_back("Events/FatJet_n2b1"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_n2b1"); }
    }
    if ( choose["Events/FatJet_n3b1"] ) {
      if (input->present("Events/FatJet_n3b1")) { FatJet_n3b1.resize(43); input->select("Events/FatJet_n3b1", FatJet_n3b1); FatJet_n3b1.clear(); successBranches.push_back("Events/FatJet_n3b1"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_n3b1"); }
    }
    if ( choose["Events/FatJet_nBHadrons"] ) {
      if (input->present("Events/FatJet_nBHadrons")) { FatJet_nBHadrons.resize(43); input->select("Events/FatJet_nBHadrons", FatJet_nBHadrons); FatJet_nBHadrons.clear(); successBranches.push_back("Events/FatJet_nBHadrons"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_nBHadrons"); }
    }
    if ( choose["Events/FatJet_nCHadrons"] ) {
      if (input->present("Events/FatJet_nCHadrons")) { FatJet_nCHadrons.resize(43); input->select("Events/FatJet_nCHadrons", FatJet_nCHadrons); FatJet_nCHadrons.clear(); successBranches.push_back("Events/FatJet_nCHadrons"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_nCHadrons"); }
    }
    if ( choose["Events/FatJet_nConstituents"] ) {
      if (input->present("Events/FatJet_nConstituents")) { FatJet_nConstituents.resize(43); input->select("Events/FatJet_nConstituents", FatJet_nConstituents); FatJet_nConstituents.clear(); successBranches.push_back("Events/FatJet_nConstituents"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_nConstituents"); }
    }
    if ( choose["Events/FatJet_particleNetMD_QCD"] ) {
      if (input->present("Events/FatJet_particleNetMD_QCD")) { FatJet_particleNetMD_QCD.resize(43); input->select("Events/FatJet_particleNetMD_QCD", FatJet_particleNetMD_QCD); FatJet_particleNetMD_QCD.clear(); successBranches.push_back("Events/FatJet_particleNetMD_QCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_particleNetMD_QCD"); }
    }
    if ( choose["Events/FatJet_particleNetMD_Xbb"] ) {
      if (input->present("Events/FatJet_particleNetMD_Xbb")) { FatJet_particleNetMD_Xbb.resize(43); input->select("Events/FatJet_particleNetMD_Xbb", FatJet_particleNetMD_Xbb); FatJet_particleNetMD_Xbb.clear(); successBranches.push_back("Events/FatJet_particleNetMD_Xbb"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_particleNetMD_Xbb"); }
    }
    if ( choose["Events/FatJet_particleNetMD_Xcc"] ) {
      if (input->present("Events/FatJet_particleNetMD_Xcc")) { FatJet_particleNetMD_Xcc.resize(43); input->select("Events/FatJet_particleNetMD_Xcc", FatJet_particleNetMD_Xcc); FatJet_particleNetMD_Xcc.clear(); successBranches.push_back("Events/FatJet_particleNetMD_Xcc"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_particleNetMD_Xcc"); }
    }
    if ( choose["Events/FatJet_particleNetMD_Xqq"] ) {
      if (input->present("Events/FatJet_particleNetMD_Xqq")) { FatJet_particleNetMD_Xqq.resize(43); input->select("Events/FatJet_particleNetMD_Xqq", FatJet_particleNetMD_Xqq); FatJet_particleNetMD_Xqq.clear(); successBranches.push_back("Events/FatJet_particleNetMD_Xqq"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_particleNetMD_Xqq"); }
    }
    if ( choose["Events/FatJet_particleNet_H4qvsQCD"] ) {
      if (input->present("Events/FatJet_particleNet_H4qvsQCD")) { FatJet_particleNet_H4qvsQCD.resize(43); input->select("Events/FatJet_particleNet_H4qvsQCD", FatJet_particleNet_H4qvsQCD); FatJet_particleNet_H4qvsQCD.clear(); successBranches.push_back("Events/FatJet_particleNet_H4qvsQCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_particleNet_H4qvsQCD"); }
    }
    if ( choose["Events/FatJet_particleNet_HbbvsQCD"] ) {
      if (input->present("Events/FatJet_particleNet_HbbvsQCD")) { FatJet_particleNet_HbbvsQCD.resize(43); input->select("Events/FatJet_particleNet_HbbvsQCD", FatJet_particleNet_HbbvsQCD); FatJet_particleNet_HbbvsQCD.clear(); successBranches.push_back("Events/FatJet_particleNet_HbbvsQCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_particleNet_HbbvsQCD"); }
    }
    if ( choose["Events/FatJet_particleNet_HccvsQCD"] ) {
      if (input->present("Events/FatJet_particleNet_HccvsQCD")) { FatJet_particleNet_HccvsQCD.resize(43); input->select("Events/FatJet_particleNet_HccvsQCD", FatJet_particleNet_HccvsQCD); FatJet_particleNet_HccvsQCD.clear(); successBranches.push_back("Events/FatJet_particleNet_HccvsQCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_particleNet_HccvsQCD"); }
    }
    if ( choose["Events/FatJet_particleNet_QCD"] ) {
      if (input->present("Events/FatJet_particleNet_QCD")) { FatJet_particleNet_QCD.resize(43); input->select("Events/FatJet_particleNet_QCD", FatJet_particleNet_QCD); FatJet_particleNet_QCD.clear(); successBranches.push_back("Events/FatJet_particleNet_QCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_particleNet_QCD"); }
    }
    if ( choose["Events/FatJet_particleNet_TvsQCD"] ) {
      if (input->present("Events/FatJet_particleNet_TvsQCD")) { FatJet_particleNet_TvsQCD.resize(43); input->select("Events/FatJet_particleNet_TvsQCD", FatJet_particleNet_TvsQCD); FatJet_particleNet_TvsQCD.clear(); successBranches.push_back("Events/FatJet_particleNet_TvsQCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_particleNet_TvsQCD"); }
    }
    if ( choose["Events/FatJet_particleNet_WvsQCD"] ) {
      if (input->present("Events/FatJet_particleNet_WvsQCD")) { FatJet_particleNet_WvsQCD.resize(43); input->select("Events/FatJet_particleNet_WvsQCD", FatJet_particleNet_WvsQCD); FatJet_particleNet_WvsQCD.clear(); successBranches.push_back("Events/FatJet_particleNet_WvsQCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_particleNet_WvsQCD"); }
    }
    if ( choose["Events/FatJet_particleNet_ZvsQCD"] ) {
      if (input->present("Events/FatJet_particleNet_ZvsQCD")) { FatJet_particleNet_ZvsQCD.resize(43); input->select("Events/FatJet_particleNet_ZvsQCD", FatJet_particleNet_ZvsQCD); FatJet_particleNet_ZvsQCD.clear(); successBranches.push_back("Events/FatJet_particleNet_ZvsQCD"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_particleNet_ZvsQCD"); }
    }
    if ( choose["Events/FatJet_particleNet_mass"] ) {
      if (input->present("Events/FatJet_particleNet_mass")) { FatJet_particleNet_mass.resize(43); input->select("Events/FatJet_particleNet_mass", FatJet_particleNet_mass); FatJet_particleNet_mass.clear(); successBranches.push_back("Events/FatJet_particleNet_mass"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_particleNet_mass"); }
    }
    if ( choose["Events/FatJet_phi"] ) {
      if (input->present("Events/FatJet_phi")) { FatJet_phi.resize(43); input->select("Events/FatJet_phi", FatJet_phi); FatJet_phi.clear(); successBranches.push_back("Events/FatJet_phi"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_phi"); }
    }
    if ( choose["Events/FatJet_pt"] ) {
      if (input->present("Events/FatJet_pt")) { FatJet_pt.resize(43); input->select("Events/FatJet_pt", FatJet_pt); FatJet_pt.clear(); successBranches.push_back("Events/FatJet_pt"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_pt"); }
    }
    if ( choose["Events/FatJet_rawFactor"] ) {
      if (input->present("Events/FatJet_rawFactor")) { FatJet_rawFactor.resize(43); input->select("Events/FatJet_rawFactor", FatJet_rawFactor); FatJet_rawFactor.clear(); successBranches.push_back("Events/FatJet_rawFactor"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_rawFactor"); }
    }
    if ( choose["Events/FatJet_subJetIdx1"] ) {
      if (input->present("Events/FatJet_subJetIdx1")) { FatJet_subJetIdx1.resize(43); input->select("Events/FatJet_subJetIdx1", FatJet_subJetIdx1); FatJet_subJetIdx1.clear(); successBranches.push_back("Events/FatJet_subJetIdx1"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_subJetIdx1"); }
    }
    if ( choose["Events/FatJet_subJetIdx2"] ) {
      if (input->present("Events/FatJet_subJetIdx2")) { FatJet_subJetIdx2.resize(43); input->select("Events/FatJet_subJetIdx2", FatJet_subJetIdx2); FatJet_subJetIdx2.clear(); successBranches.push_back("Events/FatJet_subJetIdx2"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_subJetIdx2"); }
    }
    if ( choose["Events/FatJet_tau1"] ) {
      if (input->present("Events/FatJet_tau1")) { FatJet_tau1.resize(43); input->select("Events/FatJet_tau1", FatJet_tau1); FatJet_tau1.clear(); successBranches.push_back("Events/FatJet_tau1"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_tau1"); }
    }
    if ( choose["Events/FatJet_tau2"] ) {
      if (input->present("Events/FatJet_tau2")) { FatJet_tau2.resize(43); input->select("Events/FatJet_tau2", FatJet_tau2); FatJet_tau2.clear(); successBranches.push_back("Events/FatJet_tau2"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_tau2"); }
    }
    if ( choose["Events/FatJet_tau3"] ) {
      if (input->present("Events/FatJet_tau3")) { FatJet_tau3.resize(43); input->select("Events/FatJet_tau3", FatJet_tau3); FatJet_tau3.clear(); successBranches.push_back("Events/FatJet_tau3"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_tau3"); }
    }
    if ( choose["Events/FatJet_tau4"] ) {
      if (input->present("Events/FatJet_tau4")) { FatJet_tau4.resize(43); input->select("Events/FatJet_tau4", FatJet_tau4); FatJet_tau4.clear(); successBranches.push_back("Events/FatJet_tau4"); usedCounters.insert("nFatJet"); } else { missingBranches.push_back("Events/FatJet_tau4"); }
    }
    if ( choose["Events/Flag_BadChargedCandidateFilter"] ) {
      if (input->present("Events/Flag_BadChargedCandidateFilter")) { input->select("Events/Flag_BadChargedCandidateFilter", Flag_BadChargedCandidateFilter); successBranches.push_back("Events/Flag_BadChargedCandidateFilter"); } else { missingBranches.push_back("Events/Flag_BadChargedCandidateFilter"); }
    }
    if ( choose["Events/Flag_BadChargedCandidateFilter_pRECO"] ) {
      if (input->present("Events/Flag_BadChargedCandidateFilter_pRECO")) { input->select("Events/Flag_BadChargedCandidateFilter_pRECO", Flag_BadChargedCandidateFilter_pRECO); successBranches.push_back("Events/Flag_BadChargedCandidateFilter_pRECO"); } else { missingBranches.push_back("Events/Flag_BadChargedCandidateFilter_pRECO"); }
    }
    if ( choose["Events/Flag_BadChargedCandidateSummer16Filter"] ) {
      if (input->present("Events/Flag_BadChargedCandidateSummer16Filter")) { input->select("Events/Flag_BadChargedCandidateSummer16Filter", Flag_BadChargedCandidateSummer16Filter); successBranches.push_back("Events/Flag_BadChargedCandidateSummer16Filter"); } else { missingBranches.push_back("Events/Flag_BadChargedCandidateSummer16Filter"); }
    }
    if ( choose["Events/Flag_BadChargedCandidateSummer16Filter_pRECO"] ) {
      if (input->present("Events/Flag_BadChargedCandidateSummer16Filter_pRECO")) { input->select("Events/Flag_BadChargedCandidateSummer16Filter_pRECO", Flag_BadChargedCandidateSummer16Filter_pRECO); successBranches.push_back("Events/Flag_BadChargedCandidateSummer16Filter_pRECO"); } else { missingBranches.push_back("Events/Flag_BadChargedCandidateSummer16Filter_pRECO"); }
    }
    if ( choose["Events/Flag_BadPFMuonDzFilter"] ) {
      if (input->present("Events/Flag_BadPFMuonDzFilter")) { input->select("Events/Flag_BadPFMuonDzFilter", Flag_BadPFMuonDzFilter); successBranches.push_back("Events/Flag_BadPFMuonDzFilter"); } else { missingBranches.push_back("Events/Flag_BadPFMuonDzFilter"); }
    }
    if ( choose["Events/Flag_BadPFMuonDzFilter_pRECO"] ) {
      if (input->present("Events/Flag_BadPFMuonDzFilter_pRECO")) { input->select("Events/Flag_BadPFMuonDzFilter_pRECO", Flag_BadPFMuonDzFilter_pRECO); successBranches.push_back("Events/Flag_BadPFMuonDzFilter_pRECO"); } else { missingBranches.push_back("Events/Flag_BadPFMuonDzFilter_pRECO"); }
    }
    if ( choose["Events/Flag_BadPFMuonFilter"] ) {
      if (input->present("Events/Flag_BadPFMuonFilter")) { input->select("Events/Flag_BadPFMuonFilter", Flag_BadPFMuonFilter); successBranches.push_back("Events/Flag_BadPFMuonFilter"); } else { missingBranches.push_back("Events/Flag_BadPFMuonFilter"); }
    }
    if ( choose["Events/Flag_BadPFMuonFilter_pRECO"] ) {
      if (input->present("Events/Flag_BadPFMuonFilter_pRECO")) { input->select("Events/Flag_BadPFMuonFilter_pRECO", Flag_BadPFMuonFilter_pRECO); successBranches.push_back("Events/Flag_BadPFMuonFilter_pRECO"); } else { missingBranches.push_back("Events/Flag_BadPFMuonFilter_pRECO"); }
    }
    if ( choose["Events/Flag_BadPFMuonSummer16Filter"] ) {
      if (input->present("Events/Flag_BadPFMuonSummer16Filter")) { input->select("Events/Flag_BadPFMuonSummer16Filter", Flag_BadPFMuonSummer16Filter); successBranches.push_back("Events/Flag_BadPFMuonSummer16Filter"); } else { missingBranches.push_back("Events/Flag_BadPFMuonSummer16Filter"); }
    }
    if ( choose["Events/Flag_BadPFMuonSummer16Filter_pRECO"] ) {
      if (input->present("Events/Flag_BadPFMuonSummer16Filter_pRECO")) { input->select("Events/Flag_BadPFMuonSummer16Filter_pRECO", Flag_BadPFMuonSummer16Filter_pRECO); successBranches.push_back("Events/Flag_BadPFMuonSummer16Filter_pRECO"); } else { missingBranches.push_back("Events/Flag_BadPFMuonSummer16Filter_pRECO"); }
    }
    if ( choose["Events/Flag_CSCTightHalo2015Filter"] ) {
      if (input->present("Events/Flag_CSCTightHalo2015Filter")) { input->select("Events/Flag_CSCTightHalo2015Filter", Flag_CSCTightHalo2015Filter); successBranches.push_back("Events/Flag_CSCTightHalo2015Filter"); } else { missingBranches.push_back("Events/Flag_CSCTightHalo2015Filter"); }
    }
    if ( choose["Events/Flag_CSCTightHalo2015Filter_pRECO"] ) {
      if (input->present("Events/Flag_CSCTightHalo2015Filter_pRECO")) { input->select("Events/Flag_CSCTightHalo2015Filter_pRECO", Flag_CSCTightHalo2015Filter_pRECO); successBranches.push_back("Events/Flag_CSCTightHalo2015Filter_pRECO"); } else { missingBranches.push_back("Events/Flag_CSCTightHalo2015Filter_pRECO"); }
    }
    if ( choose["Events/Flag_CSCTightHaloFilter"] ) {
      if (input->present("Events/Flag_CSCTightHaloFilter")) { input->select("Events/Flag_CSCTightHaloFilter", Flag_CSCTightHaloFilter); successBranches.push_back("Events/Flag_CSCTightHaloFilter"); } else { missingBranches.push_back("Events/Flag_CSCTightHaloFilter"); }
    }
    if ( choose["Events/Flag_CSCTightHaloFilter_pRECO"] ) {
      if (input->present("Events/Flag_CSCTightHaloFilter_pRECO")) { input->select("Events/Flag_CSCTightHaloFilter_pRECO", Flag_CSCTightHaloFilter_pRECO); successBranches.push_back("Events/Flag_CSCTightHaloFilter_pRECO"); } else { missingBranches.push_back("Events/Flag_CSCTightHaloFilter_pRECO"); }
    }
    if ( choose["Events/Flag_CSCTightHaloTrkMuUnvetoFilter"] ) {
      if (input->present("Events/Flag_CSCTightHaloTrkMuUnvetoFilter")) { input->select("Events/Flag_CSCTightHaloTrkMuUnvetoFilter", Flag_CSCTightHaloTrkMuUnvetoFilter); successBranches.push_back("Events/Flag_CSCTightHaloTrkMuUnvetoFilter"); } else { missingBranches.push_back("Events/Flag_CSCTightHaloTrkMuUnvetoFilter"); }
    }
    if ( choose["Events/Flag_CSCTightHaloTrkMuUnvetoFilter_pRECO"] ) {
      if (input->present("Events/Flag_CSCTightHaloTrkMuUnvetoFilter_pRECO")) { input->select("Events/Flag_CSCTightHaloTrkMuUnvetoFilter_pRECO", Flag_CSCTightHaloTrkMuUnvetoFilter_pRECO); successBranches.push_back("Events/Flag_CSCTightHaloTrkMuUnvetoFilter_pRECO"); } else { missingBranches.push_back("Events/Flag_CSCTightHaloTrkMuUnvetoFilter_pRECO"); }
    }
    if ( choose["Events/Flag_EcalDeadCellBoundaryEnergyFilter"] ) {
      if (input->present("Events/Flag_EcalDeadCellBoundaryEnergyFilter")) { input->select("Events/Flag_EcalDeadCellBoundaryEnergyFilter", Flag_EcalDeadCellBoundaryEnergyFilter); successBranches.push_back("Events/Flag_EcalDeadCellBoundaryEnergyFilter"); } else { missingBranches.push_back("Events/Flag_EcalDeadCellBoundaryEnergyFilter"); }
    }
    if ( choose["Events/Flag_EcalDeadCellBoundaryEnergyFilter_pRECO"] ) {
      if (input->present("Events/Flag_EcalDeadCellBoundaryEnergyFilter_pRECO")) { input->select("Events/Flag_EcalDeadCellBoundaryEnergyFilter_pRECO", Flag_EcalDeadCellBoundaryEnergyFilter_pRECO); successBranches.push_back("Events/Flag_EcalDeadCellBoundaryEnergyFilter_pRECO"); } else { missingBranches.push_back("Events/Flag_EcalDeadCellBoundaryEnergyFilter_pRECO"); }
    }
    if ( choose["Events/Flag_EcalDeadCellTriggerPrimitiveFilter"] ) {
      if (input->present("Events/Flag_EcalDeadCellTriggerPrimitiveFilter")) { input->select("Events/Flag_EcalDeadCellTriggerPrimitiveFilter", Flag_EcalDeadCellTriggerPrimitiveFilter); successBranches.push_back("Events/Flag_EcalDeadCellTriggerPrimitiveFilter"); } else { missingBranches.push_back("Events/Flag_EcalDeadCellTriggerPrimitiveFilter"); }
    }
    if ( choose["Events/Flag_EcalDeadCellTriggerPrimitiveFilter_pRECO"] ) {
      if (input->present("Events/Flag_EcalDeadCellTriggerPrimitiveFilter_pRECO")) { input->select("Events/Flag_EcalDeadCellTriggerPrimitiveFilter_pRECO", Flag_EcalDeadCellTriggerPrimitiveFilter_pRECO); successBranches.push_back("Events/Flag_EcalDeadCellTriggerPrimitiveFilter_pRECO"); } else { missingBranches.push_back("Events/Flag_EcalDeadCellTriggerPrimitiveFilter_pRECO"); }
    }
    if ( choose["Events/Flag_HBHENoiseFilter"] ) {
      if (input->present("Events/Flag_HBHENoiseFilter")) { input->select("Events/Flag_HBHENoiseFilter", Flag_HBHENoiseFilter); successBranches.push_back("Events/Flag_HBHENoiseFilter"); } else { missingBranches.push_back("Events/Flag_HBHENoiseFilter"); }
    }
    if ( choose["Events/Flag_HBHENoiseFilter_pRECO"] ) {
      if (input->present("Events/Flag_HBHENoiseFilter_pRECO")) { input->select("Events/Flag_HBHENoiseFilter_pRECO", Flag_HBHENoiseFilter_pRECO); successBranches.push_back("Events/Flag_HBHENoiseFilter_pRECO"); } else { missingBranches.push_back("Events/Flag_HBHENoiseFilter_pRECO"); }
    }
    if ( choose["Events/Flag_HBHENoiseIsoFilter"] ) {
      if (input->present("Events/Flag_HBHENoiseIsoFilter")) { input->select("Events/Flag_HBHENoiseIsoFilter", Flag_HBHENoiseIsoFilter); successBranches.push_back("Events/Flag_HBHENoiseIsoFilter"); } else { missingBranches.push_back("Events/Flag_HBHENoiseIsoFilter"); }
    }
    if ( choose["Events/Flag_HBHENoiseIsoFilter_pRECO"] ) {
      if (input->present("Events/Flag_HBHENoiseIsoFilter_pRECO")) { input->select("Events/Flag_HBHENoiseIsoFilter_pRECO", Flag_HBHENoiseIsoFilter_pRECO); successBranches.push_back("Events/Flag_HBHENoiseIsoFilter_pRECO"); } else { missingBranches.push_back("Events/Flag_HBHENoiseIsoFilter_pRECO"); }
    }
    if ( choose["Events/Flag_HcalStripHaloFilter"] ) {
      if (input->present("Events/Flag_HcalStripHaloFilter")) { input->select("Events/Flag_HcalStripHaloFilter", Flag_HcalStripHaloFilter); successBranches.push_back("Events/Flag_HcalStripHaloFilter"); } else { missingBranches.push_back("Events/Flag_HcalStripHaloFilter"); }
    }
    if ( choose["Events/Flag_HcalStripHaloFilter_pRECO"] ) {
      if (input->present("Events/Flag_HcalStripHaloFilter_pRECO")) { input->select("Events/Flag_HcalStripHaloFilter_pRECO", Flag_HcalStripHaloFilter_pRECO); successBranches.push_back("Events/Flag_HcalStripHaloFilter_pRECO"); } else { missingBranches.push_back("Events/Flag_HcalStripHaloFilter_pRECO"); }
    }
    if ( choose["Events/Flag_METFilters"] ) {
      if (input->present("Events/Flag_METFilters")) { input->select("Events/Flag_METFilters", Flag_METFilters); successBranches.push_back("Events/Flag_METFilters"); } else { missingBranches.push_back("Events/Flag_METFilters"); }
    }
    if ( choose["Events/Flag_METFilters_pRECO"] ) {
      if (input->present("Events/Flag_METFilters_pRECO")) { input->select("Events/Flag_METFilters_pRECO", Flag_METFilters_pRECO); successBranches.push_back("Events/Flag_METFilters_pRECO"); } else { missingBranches.push_back("Events/Flag_METFilters_pRECO"); }
    }
    if ( choose["Events/Flag_chargedHadronTrackResolutionFilter"] ) {
      if (input->present("Events/Flag_chargedHadronTrackResolutionFilter")) { input->select("Events/Flag_chargedHadronTrackResolutionFilter", Flag_chargedHadronTrackResolutionFilter); successBranches.push_back("Events/Flag_chargedHadronTrackResolutionFilter"); } else { missingBranches.push_back("Events/Flag_chargedHadronTrackResolutionFilter"); }
    }
    if ( choose["Events/Flag_chargedHadronTrackResolutionFilter_pRECO"] ) {
      if (input->present("Events/Flag_chargedHadronTrackResolutionFilter_pRECO")) { input->select("Events/Flag_chargedHadronTrackResolutionFilter_pRECO", Flag_chargedHadronTrackResolutionFilter_pRECO); successBranches.push_back("Events/Flag_chargedHadronTrackResolutionFilter_pRECO"); } else { missingBranches.push_back("Events/Flag_chargedHadronTrackResolutionFilter_pRECO"); }
    }
    if ( choose["Events/Flag_ecalBadCalibFilter"] ) {
      if (input->present("Events/Flag_ecalBadCalibFilter")) { input->select("Events/Flag_ecalBadCalibFilter", Flag_ecalBadCalibFilter); successBranches.push_back("Events/Flag_ecalBadCalibFilter"); } else { missingBranches.push_back("Events/Flag_ecalBadCalibFilter"); }
    }
    if ( choose["Events/Flag_ecalBadCalibFilter_pRECO"] ) {
      if (input->present("Events/Flag_ecalBadCalibFilter_pRECO")) { input->select("Events/Flag_ecalBadCalibFilter_pRECO", Flag_ecalBadCalibFilter_pRECO); successBranches.push_back("Events/Flag_ecalBadCalibFilter_pRECO"); } else { missingBranches.push_back("Events/Flag_ecalBadCalibFilter_pRECO"); }
    }
    if ( choose["Events/Flag_ecalLaserCorrFilter"] ) {
      if (input->present("Events/Flag_ecalLaserCorrFilter")) { input->select("Events/Flag_ecalLaserCorrFilter", Flag_ecalLaserCorrFilter); successBranches.push_back("Events/Flag_ecalLaserCorrFilter"); } else { missingBranches.push_back("Events/Flag_ecalLaserCorrFilter"); }
    }
    if ( choose["Events/Flag_ecalLaserCorrFilter_pRECO"] ) {
      if (input->present("Events/Flag_ecalLaserCorrFilter_pRECO")) { input->select("Events/Flag_ecalLaserCorrFilter_pRECO", Flag_ecalLaserCorrFilter_pRECO); successBranches.push_back("Events/Flag_ecalLaserCorrFilter_pRECO"); } else { missingBranches.push_back("Events/Flag_ecalLaserCorrFilter_pRECO"); }
    }
    if ( choose["Events/Flag_eeBadScFilter"] ) {
      if (input->present("Events/Flag_eeBadScFilter")) { input->select("Events/Flag_eeBadScFilter", Flag_eeBadScFilter); successBranches.push_back("Events/Flag_eeBadScFilter"); } else { missingBranches.push_back("Events/Flag_eeBadScFilter"); }
    }
    if ( choose["Events/Flag_eeBadScFilter_pRECO"] ) {
      if (input->present("Events/Flag_eeBadScFilter_pRECO")) { input->select("Events/Flag_eeBadScFilter_pRECO", Flag_eeBadScFilter_pRECO); successBranches.push_back("Events/Flag_eeBadScFilter_pRECO"); } else { missingBranches.push_back("Events/Flag_eeBadScFilter_pRECO"); }
    }
    if ( choose["Events/Flag_globalSuperTightHalo2016Filter"] ) {
      if (input->present("Events/Flag_globalSuperTightHalo2016Filter")) { input->select("Events/Flag_globalSuperTightHalo2016Filter", Flag_globalSuperTightHalo2016Filter); successBranches.push_back("Events/Flag_globalSuperTightHalo2016Filter"); } else { missingBranches.push_back("Events/Flag_globalSuperTightHalo2016Filter"); }
    }
    if ( choose["Events/Flag_globalSuperTightHalo2016Filter_pRECO"] ) {
      if (input->present("Events/Flag_globalSuperTightHalo2016Filter_pRECO")) { input->select("Events/Flag_globalSuperTightHalo2016Filter_pRECO", Flag_globalSuperTightHalo2016Filter_pRECO); successBranches.push_back("Events/Flag_globalSuperTightHalo2016Filter_pRECO"); } else { missingBranches.push_back("Events/Flag_globalSuperTightHalo2016Filter_pRECO"); }
    }
    if ( choose["Events/Flag_globalTightHalo2016Filter"] ) {
      if (input->present("Events/Flag_globalTightHalo2016Filter")) { input->select("Events/Flag_globalTightHalo2016Filter", Flag_globalTightHalo2016Filter); successBranches.push_back("Events/Flag_globalTightHalo2016Filter"); } else { missingBranches.push_back("Events/Flag_globalTightHalo2016Filter"); }
    }
    if ( choose["Events/Flag_globalTightHalo2016Filter_pRECO"] ) {
      if (input->present("Events/Flag_globalTightHalo2016Filter_pRECO")) { input->select("Events/Flag_globalTightHalo2016Filter_pRECO", Flag_globalTightHalo2016Filter_pRECO); successBranches.push_back("Events/Flag_globalTightHalo2016Filter_pRECO"); } else { missingBranches.push_back("Events/Flag_globalTightHalo2016Filter_pRECO"); }
    }
    if ( choose["Events/Flag_goodVertices"] ) {
      if (input->present("Events/Flag_goodVertices")) { input->select("Events/Flag_goodVertices", Flag_goodVertices); successBranches.push_back("Events/Flag_goodVertices"); } else { missingBranches.push_back("Events/Flag_goodVertices"); }
    }
    if ( choose["Events/Flag_goodVertices_pRECO"] ) {
      if (input->present("Events/Flag_goodVertices_pRECO")) { input->select("Events/Flag_goodVertices_pRECO", Flag_goodVertices_pRECO); successBranches.push_back("Events/Flag_goodVertices_pRECO"); } else { missingBranches.push_back("Events/Flag_goodVertices_pRECO"); }
    }
    if ( choose["Events/Flag_hcalLaserEventFilter"] ) {
      if (input->present("Events/Flag_hcalLaserEventFilter")) { input->select("Events/Flag_hcalLaserEventFilter", Flag_hcalLaserEventFilter); successBranches.push_back("Events/Flag_hcalLaserEventFilter"); } else { missingBranches.push_back("Events/Flag_hcalLaserEventFilter"); }
    }
    if ( choose["Events/Flag_hcalLaserEventFilter_pRECO"] ) {
      if (input->present("Events/Flag_hcalLaserEventFilter_pRECO")) { input->select("Events/Flag_hcalLaserEventFilter_pRECO", Flag_hcalLaserEventFilter_pRECO); successBranches.push_back("Events/Flag_hcalLaserEventFilter_pRECO"); } else { missingBranches.push_back("Events/Flag_hcalLaserEventFilter_pRECO"); }
    }
    if ( choose["Events/Flag_hfNoisyHitsFilter"] ) {
      if (input->present("Events/Flag_hfNoisyHitsFilter")) { input->select("Events/Flag_hfNoisyHitsFilter", Flag_hfNoisyHitsFilter); successBranches.push_back("Events/Flag_hfNoisyHitsFilter"); } else { missingBranches.push_back("Events/Flag_hfNoisyHitsFilter"); }
    }
    if ( choose["Events/Flag_hfNoisyHitsFilter_pRECO"] ) {
      if (input->present("Events/Flag_hfNoisyHitsFilter_pRECO")) { input->select("Events/Flag_hfNoisyHitsFilter_pRECO", Flag_hfNoisyHitsFilter_pRECO); successBranches.push_back("Events/Flag_hfNoisyHitsFilter_pRECO"); } else { missingBranches.push_back("Events/Flag_hfNoisyHitsFilter_pRECO"); }
    }
    if ( choose["Events/Flag_muonBadTrackFilter"] ) {
      if (input->present("Events/Flag_muonBadTrackFilter")) { input->select("Events/Flag_muonBadTrackFilter", Flag_muonBadTrackFilter); successBranches.push_back("Events/Flag_muonBadTrackFilter"); } else { missingBranches.push_back("Events/Flag_muonBadTrackFilter"); }
    }
    if ( choose["Events/Flag_muonBadTrackFilter_pRECO"] ) {
      if (input->present("Events/Flag_muonBadTrackFilter_pRECO")) { input->select("Events/Flag_muonBadTrackFilter_pRECO", Flag_muonBadTrackFilter_pRECO); successBranches.push_back("Events/Flag_muonBadTrackFilter_pRECO"); } else { missingBranches.push_back("Events/Flag_muonBadTrackFilter_pRECO"); }
    }
    if ( choose["Events/Flag_trkPOGFilters"] ) {
      if (input->present("Events/Flag_trkPOGFilters")) { input->select("Events/Flag_trkPOGFilters", Flag_trkPOGFilters); successBranches.push_back("Events/Flag_trkPOGFilters"); } else { missingBranches.push_back("Events/Flag_trkPOGFilters"); }
    }
    if ( choose["Events/Flag_trkPOGFilters_pRECO"] ) {
      if (input->present("Events/Flag_trkPOGFilters_pRECO")) { input->select("Events/Flag_trkPOGFilters_pRECO", Flag_trkPOGFilters_pRECO); successBranches.push_back("Events/Flag_trkPOGFilters_pRECO"); } else { missingBranches.push_back("Events/Flag_trkPOGFilters_pRECO"); }
    }
    if ( choose["Events/Flag_trkPOG_logErrorTooManyClusters"] ) {
      if (input->present("Events/Flag_trkPOG_logErrorTooManyClusters")) { input->select("Events/Flag_trkPOG_logErrorTooManyClusters", Flag_trkPOG_logErrorTooManyClusters); successBranches.push_back("Events/Flag_trkPOG_logErrorTooManyClusters"); } else { missingBranches.push_back("Events/Flag_trkPOG_logErrorTooManyClusters"); }
    }
    if ( choose["Events/Flag_trkPOG_logErrorTooManyClusters_pRECO"] ) {
      if (input->present("Events/Flag_trkPOG_logErrorTooManyClusters_pRECO")) { input->select("Events/Flag_trkPOG_logErrorTooManyClusters_pRECO", Flag_trkPOG_logErrorTooManyClusters_pRECO); successBranches.push_back("Events/Flag_trkPOG_logErrorTooManyClusters_pRECO"); } else { missingBranches.push_back("Events/Flag_trkPOG_logErrorTooManyClusters_pRECO"); }
    }
    if ( choose["Events/Flag_trkPOG_manystripclus53X"] ) {
      if (input->present("Events/Flag_trkPOG_manystripclus53X")) { input->select("Events/Flag_trkPOG_manystripclus53X", Flag_trkPOG_manystripclus53X); successBranches.push_back("Events/Flag_trkPOG_manystripclus53X"); } else { missingBranches.push_back("Events/Flag_trkPOG_manystripclus53X"); }
    }
    if ( choose["Events/Flag_trkPOG_manystripclus53X_pRECO"] ) {
      if (input->present("Events/Flag_trkPOG_manystripclus53X_pRECO")) { input->select("Events/Flag_trkPOG_manystripclus53X_pRECO", Flag_trkPOG_manystripclus53X_pRECO); successBranches.push_back("Events/Flag_trkPOG_manystripclus53X_pRECO"); } else { missingBranches.push_back("Events/Flag_trkPOG_manystripclus53X_pRECO"); }
    }
    if ( choose["Events/Flag_trkPOG_toomanystripclus53X"] ) {
      if (input->present("Events/Flag_trkPOG_toomanystripclus53X")) { input->select("Events/Flag_trkPOG_toomanystripclus53X", Flag_trkPOG_toomanystripclus53X); successBranches.push_back("Events/Flag_trkPOG_toomanystripclus53X"); } else { missingBranches.push_back("Events/Flag_trkPOG_toomanystripclus53X"); }
    }
    if ( choose["Events/Flag_trkPOG_toomanystripclus53X_pRECO"] ) {
      if (input->present("Events/Flag_trkPOG_toomanystripclus53X_pRECO")) { input->select("Events/Flag_trkPOG_toomanystripclus53X_pRECO", Flag_trkPOG_toomanystripclus53X_pRECO); successBranches.push_back("Events/Flag_trkPOG_toomanystripclus53X_pRECO"); } else { missingBranches.push_back("Events/Flag_trkPOG_toomanystripclus53X_pRECO"); }
    }
    if ( choose["Events/FsrPhoton_dROverEt2"] ) {
      if (input->present("Events/FsrPhoton_dROverEt2")) { FsrPhoton_dROverEt2.resize(31); input->select("Events/FsrPhoton_dROverEt2", FsrPhoton_dROverEt2); FsrPhoton_dROverEt2.clear(); successBranches.push_back("Events/FsrPhoton_dROverEt2"); usedCounters.insert("nFsrPhoton"); } else { missingBranches.push_back("Events/FsrPhoton_dROverEt2"); }
    }
    if ( choose["Events/FsrPhoton_eta"] ) {
      if (input->present("Events/FsrPhoton_eta")) { FsrPhoton_eta.resize(31); input->select("Events/FsrPhoton_eta", FsrPhoton_eta); FsrPhoton_eta.clear(); successBranches.push_back("Events/FsrPhoton_eta"); usedCounters.insert("nFsrPhoton"); } else { missingBranches.push_back("Events/FsrPhoton_eta"); }
    }
    if ( choose["Events/FsrPhoton_muonIdx"] ) {
      if (input->present("Events/FsrPhoton_muonIdx")) { FsrPhoton_muonIdx.resize(31); input->select("Events/FsrPhoton_muonIdx", FsrPhoton_muonIdx); FsrPhoton_muonIdx.clear(); successBranches.push_back("Events/FsrPhoton_muonIdx"); usedCounters.insert("nFsrPhoton"); } else { missingBranches.push_back("Events/FsrPhoton_muonIdx"); }
    }
    if ( choose["Events/FsrPhoton_phi"] ) {
      if (input->present("Events/FsrPhoton_phi")) { FsrPhoton_phi.resize(31); input->select("Events/FsrPhoton_phi", FsrPhoton_phi); FsrPhoton_phi.clear(); successBranches.push_back("Events/FsrPhoton_phi"); usedCounters.insert("nFsrPhoton"); } else { missingBranches.push_back("Events/FsrPhoton_phi"); }
    }
    if ( choose["Events/FsrPhoton_pt"] ) {
      if (input->present("Events/FsrPhoton_pt")) { FsrPhoton_pt.resize(31); input->select("Events/FsrPhoton_pt", FsrPhoton_pt); FsrPhoton_pt.clear(); successBranches.push_back("Events/FsrPhoton_pt"); usedCounters.insert("nFsrPhoton"); } else { missingBranches.push_back("Events/FsrPhoton_pt"); }
    }
    if ( choose["Events/FsrPhoton_relIso03"] ) {
      if (input->present("Events/FsrPhoton_relIso03")) { FsrPhoton_relIso03.resize(31); input->select("Events/FsrPhoton_relIso03", FsrPhoton_relIso03); FsrPhoton_relIso03.clear(); successBranches.push_back("Events/FsrPhoton_relIso03"); usedCounters.insert("nFsrPhoton"); } else { missingBranches.push_back("Events/FsrPhoton_relIso03"); }
    }
    if ( choose["Events/GenDressedLepton_eta"] ) {
      if (input->present("Events/GenDressedLepton_eta")) { GenDressedLepton_eta.resize(28); input->select("Events/GenDressedLepton_eta", GenDressedLepton_eta); GenDressedLepton_eta.clear(); successBranches.push_back("Events/GenDressedLepton_eta"); usedCounters.insert("nGenDressedLepton"); } else { missingBranches.push_back("Events/GenDressedLepton_eta"); }
    }
    if ( choose["Events/GenDressedLepton_hasTauAnc"] ) {
      if (input->present("Events/GenDressedLepton_hasTauAnc")) { GenDressedLepton_hasTauAnc.resize(28); input->select("Events/GenDressedLepton_hasTauAnc", GenDressedLepton_hasTauAnc); GenDressedLepton_hasTauAnc.clear(); successBranches.push_back("Events/GenDressedLepton_hasTauAnc"); usedCounters.insert("nGenDressedLepton"); } else { missingBranches.push_back("Events/GenDressedLepton_hasTauAnc"); }
    }
    if ( choose["Events/GenDressedLepton_mass"] ) {
      if (input->present("Events/GenDressedLepton_mass")) { GenDressedLepton_mass.resize(28); input->select("Events/GenDressedLepton_mass", GenDressedLepton_mass); GenDressedLepton_mass.clear(); successBranches.push_back("Events/GenDressedLepton_mass"); usedCounters.insert("nGenDressedLepton"); } else { missingBranches.push_back("Events/GenDressedLepton_mass"); }
    }
    if ( choose["Events/GenDressedLepton_pdgId"] ) {
      if (input->present("Events/GenDressedLepton_pdgId")) { GenDressedLepton_pdgId.resize(28); input->select("Events/GenDressedLepton_pdgId", GenDressedLepton_pdgId); GenDressedLepton_pdgId.clear(); successBranches.push_back("Events/GenDressedLepton_pdgId"); usedCounters.insert("nGenDressedLepton"); } else { missingBranches.push_back("Events/GenDressedLepton_pdgId"); }
    }
    if ( choose["Events/GenDressedLepton_phi"] ) {
      if (input->present("Events/GenDressedLepton_phi")) { GenDressedLepton_phi.resize(28); input->select("Events/GenDressedLepton_phi", GenDressedLepton_phi); GenDressedLepton_phi.clear(); successBranches.push_back("Events/GenDressedLepton_phi"); usedCounters.insert("nGenDressedLepton"); } else { missingBranches.push_back("Events/GenDressedLepton_phi"); }
    }
    if ( choose["Events/GenDressedLepton_pt"] ) {
      if (input->present("Events/GenDressedLepton_pt")) { GenDressedLepton_pt.resize(28); input->select("Events/GenDressedLepton_pt", GenDressedLepton_pt); GenDressedLepton_pt.clear(); successBranches.push_back("Events/GenDressedLepton_pt"); usedCounters.insert("nGenDressedLepton"); } else { missingBranches.push_back("Events/GenDressedLepton_pt"); }
    }
    if ( choose["Events/GenIsolatedPhoton_eta"] ) {
      if (input->present("Events/GenIsolatedPhoton_eta")) { GenIsolatedPhoton_eta.resize(28); input->select("Events/GenIsolatedPhoton_eta", GenIsolatedPhoton_eta); GenIsolatedPhoton_eta.clear(); successBranches.push_back("Events/GenIsolatedPhoton_eta"); usedCounters.insert("nGenIsolatedPhoton"); } else { missingBranches.push_back("Events/GenIsolatedPhoton_eta"); }
    }
    if ( choose["Events/GenIsolatedPhoton_mass"] ) {
      if (input->present("Events/GenIsolatedPhoton_mass")) { GenIsolatedPhoton_mass.resize(28); input->select("Events/GenIsolatedPhoton_mass", GenIsolatedPhoton_mass); GenIsolatedPhoton_mass.clear(); successBranches.push_back("Events/GenIsolatedPhoton_mass"); usedCounters.insert("nGenIsolatedPhoton"); } else { missingBranches.push_back("Events/GenIsolatedPhoton_mass"); }
    }
    if ( choose["Events/GenIsolatedPhoton_phi"] ) {
      if (input->present("Events/GenIsolatedPhoton_phi")) { GenIsolatedPhoton_phi.resize(28); input->select("Events/GenIsolatedPhoton_phi", GenIsolatedPhoton_phi); GenIsolatedPhoton_phi.clear(); successBranches.push_back("Events/GenIsolatedPhoton_phi"); usedCounters.insert("nGenIsolatedPhoton"); } else { missingBranches.push_back("Events/GenIsolatedPhoton_phi"); }
    }
    if ( choose["Events/GenIsolatedPhoton_pt"] ) {
      if (input->present("Events/GenIsolatedPhoton_pt")) { GenIsolatedPhoton_pt.resize(28); input->select("Events/GenIsolatedPhoton_pt", GenIsolatedPhoton_pt); GenIsolatedPhoton_pt.clear(); successBranches.push_back("Events/GenIsolatedPhoton_pt"); usedCounters.insert("nGenIsolatedPhoton"); } else { missingBranches.push_back("Events/GenIsolatedPhoton_pt"); }
    }
    if ( choose["Events/GenJetAK8_eta"] ) {
      if (input->present("Events/GenJetAK8_eta")) { GenJetAK8_eta.resize(46); input->select("Events/GenJetAK8_eta", GenJetAK8_eta); GenJetAK8_eta.clear(); successBranches.push_back("Events/GenJetAK8_eta"); usedCounters.insert("nGenJetAK8"); } else { missingBranches.push_back("Events/GenJetAK8_eta"); }
    }
    if ( choose["Events/GenJetAK8_hadronFlavour"] ) {
      if (input->present("Events/GenJetAK8_hadronFlavour")) { GenJetAK8_hadronFlavour.resize(46); input->select("Events/GenJetAK8_hadronFlavour", GenJetAK8_hadronFlavour); GenJetAK8_hadronFlavour.clear(); successBranches.push_back("Events/GenJetAK8_hadronFlavour"); usedCounters.insert("nGenJetAK8"); } else { missingBranches.push_back("Events/GenJetAK8_hadronFlavour"); }
    }
    if ( choose["Events/GenJetAK8_mass"] ) {
      if (input->present("Events/GenJetAK8_mass")) { GenJetAK8_mass.resize(46); input->select("Events/GenJetAK8_mass", GenJetAK8_mass); GenJetAK8_mass.clear(); successBranches.push_back("Events/GenJetAK8_mass"); usedCounters.insert("nGenJetAK8"); } else { missingBranches.push_back("Events/GenJetAK8_mass"); }
    }
    if ( choose["Events/GenJetAK8_partonFlavour"] ) {
      if (input->present("Events/GenJetAK8_partonFlavour")) { GenJetAK8_partonFlavour.resize(46); input->select("Events/GenJetAK8_partonFlavour", GenJetAK8_partonFlavour); GenJetAK8_partonFlavour.clear(); successBranches.push_back("Events/GenJetAK8_partonFlavour"); usedCounters.insert("nGenJetAK8"); } else { missingBranches.push_back("Events/GenJetAK8_partonFlavour"); }
    }
    if ( choose["Events/GenJetAK8_phi"] ) {
      if (input->present("Events/GenJetAK8_phi")) { GenJetAK8_phi.resize(46); input->select("Events/GenJetAK8_phi", GenJetAK8_phi); GenJetAK8_phi.clear(); successBranches.push_back("Events/GenJetAK8_phi"); usedCounters.insert("nGenJetAK8"); } else { missingBranches.push_back("Events/GenJetAK8_phi"); }
    }
    if ( choose["Events/GenJetAK8_pt"] ) {
      if (input->present("Events/GenJetAK8_pt")) { GenJetAK8_pt.resize(46); input->select("Events/GenJetAK8_pt", GenJetAK8_pt); GenJetAK8_pt.clear(); successBranches.push_back("Events/GenJetAK8_pt"); usedCounters.insert("nGenJetAK8"); } else { missingBranches.push_back("Events/GenJetAK8_pt"); }
    }
    if ( choose["Events/GenJet_eta"] ) {
      if (input->present("Events/GenJet_eta")) { GenJet_eta.resize(81); input->select("Events/GenJet_eta", GenJet_eta); GenJet_eta.clear(); successBranches.push_back("Events/GenJet_eta"); usedCounters.insert("nGenJet"); } else { missingBranches.push_back("Events/GenJet_eta"); }
    }
    if ( choose["Events/GenJet_hadronFlavour"] ) {
      if (input->present("Events/GenJet_hadronFlavour")) { GenJet_hadronFlavour.resize(81); input->select("Events/GenJet_hadronFlavour", GenJet_hadronFlavour); GenJet_hadronFlavour.clear(); successBranches.push_back("Events/GenJet_hadronFlavour"); usedCounters.insert("nGenJet"); } else { missingBranches.push_back("Events/GenJet_hadronFlavour"); }
    }
    if ( choose["Events/GenJet_mass"] ) {
      if (input->present("Events/GenJet_mass")) { GenJet_mass.resize(81); input->select("Events/GenJet_mass", GenJet_mass); GenJet_mass.clear(); successBranches.push_back("Events/GenJet_mass"); usedCounters.insert("nGenJet"); } else { missingBranches.push_back("Events/GenJet_mass"); }
    }
    if ( choose["Events/GenJet_nBHadrons"] ) {
      if (input->present("Events/GenJet_nBHadrons")) { GenJet_nBHadrons.resize(81); input->select("Events/GenJet_nBHadrons", GenJet_nBHadrons); GenJet_nBHadrons.clear(); successBranches.push_back("Events/GenJet_nBHadrons"); usedCounters.insert("nGenJet"); } else { missingBranches.push_back("Events/GenJet_nBHadrons"); }
    }
    if ( choose["Events/GenJet_nCHadrons"] ) {
      if (input->present("Events/GenJet_nCHadrons")) { GenJet_nCHadrons.resize(81); input->select("Events/GenJet_nCHadrons", GenJet_nCHadrons); GenJet_nCHadrons.clear(); successBranches.push_back("Events/GenJet_nCHadrons"); usedCounters.insert("nGenJet"); } else { missingBranches.push_back("Events/GenJet_nCHadrons"); }
    }
    if ( choose["Events/GenJet_partonFlavour"] ) {
      if (input->present("Events/GenJet_partonFlavour")) { GenJet_partonFlavour.resize(81); input->select("Events/GenJet_partonFlavour", GenJet_partonFlavour); GenJet_partonFlavour.clear(); successBranches.push_back("Events/GenJet_partonFlavour"); usedCounters.insert("nGenJet"); } else { missingBranches.push_back("Events/GenJet_partonFlavour"); }
    }
    if ( choose["Events/GenJet_phi"] ) {
      if (input->present("Events/GenJet_phi")) { GenJet_phi.resize(81); input->select("Events/GenJet_phi", GenJet_phi); GenJet_phi.clear(); successBranches.push_back("Events/GenJet_phi"); usedCounters.insert("nGenJet"); } else { missingBranches.push_back("Events/GenJet_phi"); }
    }
    if ( choose["Events/GenJet_pt"] ) {
      if (input->present("Events/GenJet_pt")) { GenJet_pt.resize(81); input->select("Events/GenJet_pt", GenJet_pt); GenJet_pt.clear(); successBranches.push_back("Events/GenJet_pt"); usedCounters.insert("nGenJet"); } else { missingBranches.push_back("Events/GenJet_pt"); }
    }
    if ( choose["Events/GenMET_phi"] ) {
      if (input->present("Events/GenMET_phi")) { input->select("Events/GenMET_phi", GenMET_phi); successBranches.push_back("Events/GenMET_phi"); } else { missingBranches.push_back("Events/GenMET_phi"); }
    }
    if ( choose["Events/GenMET_pt"] ) {
      if (input->present("Events/GenMET_pt")) { input->select("Events/GenMET_pt", GenMET_pt); successBranches.push_back("Events/GenMET_pt"); } else { missingBranches.push_back("Events/GenMET_pt"); }
    }
    if ( choose["Events/GenPart_eta"] ) {
      if (input->present("Events/GenPart_eta")) { GenPart_eta.resize(295); input->select("Events/GenPart_eta", GenPart_eta); GenPart_eta.clear(); successBranches.push_back("Events/GenPart_eta"); usedCounters.insert("nGenPart"); } else { missingBranches.push_back("Events/GenPart_eta"); }
    }
    if ( choose["Events/GenPart_genPartIdxMother"] ) {
      if (input->present("Events/GenPart_genPartIdxMother")) { GenPart_genPartIdxMother.resize(295); input->select("Events/GenPart_genPartIdxMother", GenPart_genPartIdxMother); GenPart_genPartIdxMother.clear(); successBranches.push_back("Events/GenPart_genPartIdxMother"); usedCounters.insert("nGenPart"); } else { missingBranches.push_back("Events/GenPart_genPartIdxMother"); }
    }
    if ( choose["Events/GenPart_mass"] ) {
      if (input->present("Events/GenPart_mass")) { GenPart_mass.resize(295); input->select("Events/GenPart_mass", GenPart_mass); GenPart_mass.clear(); successBranches.push_back("Events/GenPart_mass"); usedCounters.insert("nGenPart"); } else { missingBranches.push_back("Events/GenPart_mass"); }
    }
    if ( choose["Events/GenPart_pdgId"] ) {
      if (input->present("Events/GenPart_pdgId")) { GenPart_pdgId.resize(295); input->select("Events/GenPart_pdgId", GenPart_pdgId); GenPart_pdgId.clear(); successBranches.push_back("Events/GenPart_pdgId"); usedCounters.insert("nGenPart"); } else { missingBranches.push_back("Events/GenPart_pdgId"); }
    }
    if ( choose["Events/GenPart_phi"] ) {
      if (input->present("Events/GenPart_phi")) { GenPart_phi.resize(295); input->select("Events/GenPart_phi", GenPart_phi); GenPart_phi.clear(); successBranches.push_back("Events/GenPart_phi"); usedCounters.insert("nGenPart"); } else { missingBranches.push_back("Events/GenPart_phi"); }
    }
    if ( choose["Events/GenPart_pt"] ) {
      if (input->present("Events/GenPart_pt")) { GenPart_pt.resize(295); input->select("Events/GenPart_pt", GenPart_pt); GenPart_pt.clear(); successBranches.push_back("Events/GenPart_pt"); usedCounters.insert("nGenPart"); } else { missingBranches.push_back("Events/GenPart_pt"); }
    }
    if ( choose["Events/GenPart_status"] ) {
      if (input->present("Events/GenPart_status")) { GenPart_status.resize(295); input->select("Events/GenPart_status", GenPart_status); GenPart_status.clear(); successBranches.push_back("Events/GenPart_status"); usedCounters.insert("nGenPart"); } else { missingBranches.push_back("Events/GenPart_status"); }
    }
    if ( choose["Events/GenPart_statusFlags"] ) {
      if (input->present("Events/GenPart_statusFlags")) { GenPart_statusFlags.resize(295); input->select("Events/GenPart_statusFlags", GenPart_statusFlags); GenPart_statusFlags.clear(); successBranches.push_back("Events/GenPart_statusFlags"); usedCounters.insert("nGenPart"); } else { missingBranches.push_back("Events/GenPart_statusFlags"); }
    }
    if ( choose["Events/GenVisTau_charge"] ) {
      if (input->present("Events/GenVisTau_charge")) { GenVisTau_charge.resize(28); input->select("Events/GenVisTau_charge", GenVisTau_charge); GenVisTau_charge.clear(); successBranches.push_back("Events/GenVisTau_charge"); usedCounters.insert("nGenVisTau"); } else { missingBranches.push_back("Events/GenVisTau_charge"); }
    }
    if ( choose["Events/GenVisTau_eta"] ) {
      if (input->present("Events/GenVisTau_eta")) { GenVisTau_eta.resize(28); input->select("Events/GenVisTau_eta", GenVisTau_eta); GenVisTau_eta.clear(); successBranches.push_back("Events/GenVisTau_eta"); usedCounters.insert("nGenVisTau"); } else { missingBranches.push_back("Events/GenVisTau_eta"); }
    }
    if ( choose["Events/GenVisTau_genPartIdxMother"] ) {
      if (input->present("Events/GenVisTau_genPartIdxMother")) { GenVisTau_genPartIdxMother.resize(28); input->select("Events/GenVisTau_genPartIdxMother", GenVisTau_genPartIdxMother); GenVisTau_genPartIdxMother.clear(); successBranches.push_back("Events/GenVisTau_genPartIdxMother"); usedCounters.insert("nGenVisTau"); } else { missingBranches.push_back("Events/GenVisTau_genPartIdxMother"); }
    }
    if ( choose["Events/GenVisTau_mass"] ) {
      if (input->present("Events/GenVisTau_mass")) { GenVisTau_mass.resize(28); input->select("Events/GenVisTau_mass", GenVisTau_mass); GenVisTau_mass.clear(); successBranches.push_back("Events/GenVisTau_mass"); usedCounters.insert("nGenVisTau"); } else { missingBranches.push_back("Events/GenVisTau_mass"); }
    }
    if ( choose["Events/GenVisTau_phi"] ) {
      if (input->present("Events/GenVisTau_phi")) { GenVisTau_phi.resize(28); input->select("Events/GenVisTau_phi", GenVisTau_phi); GenVisTau_phi.clear(); successBranches.push_back("Events/GenVisTau_phi"); usedCounters.insert("nGenVisTau"); } else { missingBranches.push_back("Events/GenVisTau_phi"); }
    }
    if ( choose["Events/GenVisTau_pt"] ) {
      if (input->present("Events/GenVisTau_pt")) { GenVisTau_pt.resize(28); input->select("Events/GenVisTau_pt", GenVisTau_pt); GenVisTau_pt.clear(); successBranches.push_back("Events/GenVisTau_pt"); usedCounters.insert("nGenVisTau"); } else { missingBranches.push_back("Events/GenVisTau_pt"); }
    }
    if ( choose["Events/GenVisTau_status"] ) {
      if (input->present("Events/GenVisTau_status")) { GenVisTau_status.resize(28); input->select("Events/GenVisTau_status", GenVisTau_status); GenVisTau_status.clear(); successBranches.push_back("Events/GenVisTau_status"); usedCounters.insert("nGenVisTau"); } else { missingBranches.push_back("Events/GenVisTau_status"); }
    }
    if ( choose["Events/GenVtx_t0"] ) {
      if (input->present("Events/GenVtx_t0")) { input->select("Events/GenVtx_t0", GenVtx_t0); successBranches.push_back("Events/GenVtx_t0"); } else { missingBranches.push_back("Events/GenVtx_t0"); }
    }
    if ( choose["Events/GenVtx_x"] ) {
      if (input->present("Events/GenVtx_x")) { input->select("Events/GenVtx_x", GenVtx_x); successBranches.push_back("Events/GenVtx_x"); } else { missingBranches.push_back("Events/GenVtx_x"); }
    }
    if ( choose["Events/GenVtx_y"] ) {
      if (input->present("Events/GenVtx_y")) { input->select("Events/GenVtx_y", GenVtx_y); successBranches.push_back("Events/GenVtx_y"); } else { missingBranches.push_back("Events/GenVtx_y"); }
    }
    if ( choose["Events/GenVtx_z"] ) {
      if (input->present("Events/GenVtx_z")) { input->select("Events/GenVtx_z", GenVtx_z); successBranches.push_back("Events/GenVtx_z"); } else { missingBranches.push_back("Events/GenVtx_z"); }
    }
    if ( choose["Events/Generator_binvar"] ) {
      if (input->present("Events/Generator_binvar")) { input->select("Events/Generator_binvar", Generator_binvar); successBranches.push_back("Events/Generator_binvar"); } else { missingBranches.push_back("Events/Generator_binvar"); }
    }
    if ( choose["Events/Generator_id1"] ) {
      if (input->present("Events/Generator_id1")) { input->select("Events/Generator_id1", Generator_id1); successBranches.push_back("Events/Generator_id1"); } else { missingBranches.push_back("Events/Generator_id1"); }
    }
    if ( choose["Events/Generator_id2"] ) {
      if (input->present("Events/Generator_id2")) { input->select("Events/Generator_id2", Generator_id2); successBranches.push_back("Events/Generator_id2"); } else { missingBranches.push_back("Events/Generator_id2"); }
    }
    if ( choose["Events/Generator_scalePDF"] ) {
      if (input->present("Events/Generator_scalePDF")) { input->select("Events/Generator_scalePDF", Generator_scalePDF); successBranches.push_back("Events/Generator_scalePDF"); } else { missingBranches.push_back("Events/Generator_scalePDF"); }
    }
    if ( choose["Events/Generator_weight"] ) {
      if (input->present("Events/Generator_weight")) { input->select("Events/Generator_weight", Generator_weight); successBranches.push_back("Events/Generator_weight"); } else { missingBranches.push_back("Events/Generator_weight"); }
    }
    if ( choose["Events/Generator_x1"] ) {
      if (input->present("Events/Generator_x1")) { input->select("Events/Generator_x1", Generator_x1); successBranches.push_back("Events/Generator_x1"); } else { missingBranches.push_back("Events/Generator_x1"); }
    }
    if ( choose["Events/Generator_x2"] ) {
      if (input->present("Events/Generator_x2")) { input->select("Events/Generator_x2", Generator_x2); successBranches.push_back("Events/Generator_x2"); } else { missingBranches.push_back("Events/Generator_x2"); }
    }
    if ( choose["Events/Generator_xpdf1"] ) {
      if (input->present("Events/Generator_xpdf1")) { input->select("Events/Generator_xpdf1", Generator_xpdf1); successBranches.push_back("Events/Generator_xpdf1"); } else { missingBranches.push_back("Events/Generator_xpdf1"); }
    }
    if ( choose["Events/Generator_xpdf2"] ) {
      if (input->present("Events/Generator_xpdf2")) { input->select("Events/Generator_xpdf2", Generator_xpdf2); successBranches.push_back("Events/Generator_xpdf2"); } else { missingBranches.push_back("Events/Generator_xpdf2"); }
    }
    if ( choose["Events/HLT_Ele30_WPTight_Gsf"] ) {
      if (input->present("Events/HLT_Ele30_WPTight_Gsf")) { input->select("Events/HLT_Ele30_WPTight_Gsf", HLT_Ele30_WPTight_Gsf); successBranches.push_back("Events/HLT_Ele30_WPTight_Gsf"); } else { missingBranches.push_back("Events/HLT_Ele30_WPTight_Gsf"); }
    }
    if ( choose["Events/HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07"] ) {
      if (input->present("Events/HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07")) { input->select("Events/HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07", HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07); successBranches.push_back("Events/HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07"); } else { missingBranches.push_back("Events/HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07"); }
    }
    if ( choose["Events/HLT_IsoMu24"] ) {
      if (input->present("Events/HLT_IsoMu24")) { input->select("Events/HLT_IsoMu24", HLT_IsoMu24); successBranches.push_back("Events/HLT_IsoMu24"); } else { missingBranches.push_back("Events/HLT_IsoMu24"); }
    }
    if ( choose["Events/HLT_IsoMu27"] ) {
      if (input->present("Events/HLT_IsoMu27")) { input->select("Events/HLT_IsoMu27", HLT_IsoMu27); successBranches.push_back("Events/HLT_IsoMu27"); } else { missingBranches.push_back("Events/HLT_IsoMu27"); }
    }
    if ( choose["Events/HLT_PFHT1050"] ) {
      if (input->present("Events/HLT_PFHT1050")) { input->select("Events/HLT_PFHT1050", HLT_PFHT1050); successBranches.push_back("Events/HLT_PFHT1050"); } else { missingBranches.push_back("Events/HLT_PFHT1050"); }
    }
    if ( choose["Events/HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p55"] ) {
      if (input->present("Events/HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p55")) { input->select("Events/HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p55", HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p55); successBranches.push_back("Events/HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p55"); } else { missingBranches.push_back("Events/HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p55"); }
    }
    if ( choose["Events/HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p60"] ) {
      if (input->present("Events/HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p60")) { input->select("Events/HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p60", HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p60); successBranches.push_back("Events/HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p60"); } else { missingBranches.push_back("Events/HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p60"); }
    }
    if ( choose["Events/HLT_PFHT280_QuadPFJet35_PNet2BTagMean0p60"] ) {
      if (input->present("Events/HLT_PFHT280_QuadPFJet35_PNet2BTagMean0p60")) { input->select("Events/HLT_PFHT280_QuadPFJet35_PNet2BTagMean0p60", HLT_PFHT280_QuadPFJet35_PNet2BTagMean0p60); successBranches.push_back("Events/HLT_PFHT280_QuadPFJet35_PNet2BTagMean0p60"); } else { missingBranches.push_back("Events/HLT_PFHT280_QuadPFJet35_PNet2BTagMean0p60"); }
    }
    if ( choose["Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0"] ) {
      if (input->present("Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0")) { input->select("Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0", HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0); successBranches.push_back("Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0"); } else { missingBranches.push_back("Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0"); }
    }
    if ( choose["Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40"] ) {
      if (input->present("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40")) { input->select("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40", HLT_PFHT330PT30_QuadPFJet_75_60_45_40); successBranches.push_back("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40"); } else { missingBranches.push_back("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40"); }
    }
    if ( choose["Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_2p0"] ) {
      if (input->present("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_2p0")) { input->select("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_2p0", HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_2p0); successBranches.push_back("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_2p0"); } else { missingBranches.push_back("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_2p0"); }
    }
    if ( choose["Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3"] ) {
      if (input->present("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3")) { input->select("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3", HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3); successBranches.push_back("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3"); } else { missingBranches.push_back("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3"); }
    }
    if ( choose["Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5"] ) {
      if (input->present("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5")) { input->select("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5", HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5); successBranches.push_back("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5"); } else { missingBranches.push_back("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5"); }
    }
    if ( choose["Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5"] ) {
      if (input->present("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5")) { input->select("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5", HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5); successBranches.push_back("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5"); } else { missingBranches.push_back("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5"); }
    }
    if ( choose["Events/HLT_PFHT340_QuadPFJet70_50_40_40_PNet2BTagMean0p70"] ) {
      if (input->present("Events/HLT_PFHT340_QuadPFJet70_50_40_40_PNet2BTagMean0p70")) { input->select("Events/HLT_PFHT340_QuadPFJet70_50_40_40_PNet2BTagMean0p70", HLT_PFHT340_QuadPFJet70_50_40_40_PNet2BTagMean0p70); successBranches.push_back("Events/HLT_PFHT340_QuadPFJet70_50_40_40_PNet2BTagMean0p70"); } else { missingBranches.push_back("Events/HLT_PFHT340_QuadPFJet70_50_40_40_PNet2BTagMean0p70"); }
    }
    if ( choose["Events/HLT_PFHT380_SixJet32_DoubleBTagCSV_p075"] ) {
      if (input->present("Events/HLT_PFHT380_SixJet32_DoubleBTagCSV_p075")) { input->select("Events/HLT_PFHT380_SixJet32_DoubleBTagCSV_p075", HLT_PFHT380_SixJet32_DoubleBTagCSV_p075); successBranches.push_back("Events/HLT_PFHT380_SixJet32_DoubleBTagCSV_p075"); } else { missingBranches.push_back("Events/HLT_PFHT380_SixJet32_DoubleBTagCSV_p075"); }
    }
    if ( choose["Events/HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2"] ) {
      if (input->present("Events/HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2")) { input->select("Events/HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2", HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2); successBranches.push_back("Events/HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2"); } else { missingBranches.push_back("Events/HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2"); }
    }
    if ( choose["Events/HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_4p3"] ) {
      if (input->present("Events/HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_4p3")) { input->select("Events/HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_4p3", HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_4p3); successBranches.push_back("Events/HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_4p3"); } else { missingBranches.push_back("Events/HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_4p3"); }
    }
    if ( choose["Events/HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_5p6"] ) {
      if (input->present("Events/HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_5p6")) { input->select("Events/HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_5p6", HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_5p6); successBranches.push_back("Events/HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_5p6"); } else { missingBranches.push_back("Events/HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_5p6"); }
    }
    if ( choose["Events/HLT_PFHT400_SixPFJet32"] ) {
      if (input->present("Events/HLT_PFHT400_SixPFJet32")) { input->select("Events/HLT_PFHT400_SixPFJet32", HLT_PFHT400_SixPFJet32); successBranches.push_back("Events/HLT_PFHT400_SixPFJet32"); } else { missingBranches.push_back("Events/HLT_PFHT400_SixPFJet32"); }
    }
    if ( choose["Events/HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94"] ) {
      if (input->present("Events/HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94")) { input->select("Events/HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94", HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94); successBranches.push_back("Events/HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94"); } else { missingBranches.push_back("Events/HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94"); }
    }
    if ( choose["Events/HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50"] ) {
      if (input->present("Events/HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50")) { input->select("Events/HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50", HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50); successBranches.push_back("Events/HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50"); } else { missingBranches.push_back("Events/HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50"); }
    }
    if ( choose["Events/HLT_PFHT430_SixJet40_BTagCSV_p080"] ) {
      if (input->present("Events/HLT_PFHT430_SixJet40_BTagCSV_p080")) { input->select("Events/HLT_PFHT430_SixJet40_BTagCSV_p080", HLT_PFHT430_SixJet40_BTagCSV_p080); successBranches.push_back("Events/HLT_PFHT430_SixJet40_BTagCSV_p080"); } else { missingBranches.push_back("Events/HLT_PFHT430_SixJet40_BTagCSV_p080"); }
    }
    if ( choose["Events/HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5"] ) {
      if (input->present("Events/HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5")) { input->select("Events/HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5", HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5); successBranches.push_back("Events/HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5"); } else { missingBranches.push_back("Events/HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5"); }
    }
    if ( choose["Events/HLT_PFHT450_SixPFJet36"] ) {
      if (input->present("Events/HLT_PFHT450_SixPFJet36")) { input->select("Events/HLT_PFHT450_SixPFJet36", HLT_PFHT450_SixPFJet36); successBranches.push_back("Events/HLT_PFHT450_SixPFJet36"); } else { missingBranches.push_back("Events/HLT_PFHT450_SixPFJet36"); }
    }
    if ( choose["Events/HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59"] ) {
      if (input->present("Events/HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59")) { input->select("Events/HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59", HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59); successBranches.push_back("Events/HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59"); } else { missingBranches.push_back("Events/HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59"); }
    }
    if ( choose["Events/HLT_PFHT450_SixPFJet36_PNetBTag0p35"] ) {
      if (input->present("Events/HLT_PFHT450_SixPFJet36_PNetBTag0p35")) { input->select("Events/HLT_PFHT450_SixPFJet36_PNetBTag0p35", HLT_PFHT450_SixPFJet36_PNetBTag0p35); successBranches.push_back("Events/HLT_PFHT450_SixPFJet36_PNetBTag0p35"); } else { missingBranches.push_back("Events/HLT_PFHT450_SixPFJet36_PNetBTag0p35"); }
    }
    if ( choose["Events/HLTriggerFinalPath"] ) {
      if (input->present("Events/HLTriggerFinalPath")) { input->select("Events/HLTriggerFinalPath", HLTriggerFinalPath); successBranches.push_back("Events/HLTriggerFinalPath"); } else { missingBranches.push_back("Events/HLTriggerFinalPath"); }
    }
    if ( choose["Events/HLTriggerFirstPath"] ) {
      if (input->present("Events/HLTriggerFirstPath")) { input->select("Events/HLTriggerFirstPath", HLTriggerFirstPath); successBranches.push_back("Events/HLTriggerFirstPath"); } else { missingBranches.push_back("Events/HLTriggerFirstPath"); }
    }
    if ( choose["Events/HTXS_Higgs_pt"] ) {
      if (input->present("Events/HTXS_Higgs_pt")) { input->select("Events/HTXS_Higgs_pt", HTXS_Higgs_pt); successBranches.push_back("Events/HTXS_Higgs_pt"); } else { missingBranches.push_back("Events/HTXS_Higgs_pt"); }
    }
    if ( choose["Events/HTXS_Higgs_y"] ) {
      if (input->present("Events/HTXS_Higgs_y")) { input->select("Events/HTXS_Higgs_y", HTXS_Higgs_y); successBranches.push_back("Events/HTXS_Higgs_y"); } else { missingBranches.push_back("Events/HTXS_Higgs_y"); }
    }
    if ( choose["Events/HTXS_njets25"] ) {
      if (input->present("Events/HTXS_njets25")) { input->select("Events/HTXS_njets25", HTXS_njets25); successBranches.push_back("Events/HTXS_njets25"); } else { missingBranches.push_back("Events/HTXS_njets25"); }
    }
    if ( choose["Events/HTXS_njets30"] ) {
      if (input->present("Events/HTXS_njets30")) { input->select("Events/HTXS_njets30", HTXS_njets30); successBranches.push_back("Events/HTXS_njets30"); } else { missingBranches.push_back("Events/HTXS_njets30"); }
    }
    if ( choose["Events/HTXS_stage1_1_cat_pTjet25GeV"] ) {
      if (input->present("Events/HTXS_stage1_1_cat_pTjet25GeV")) { input->select("Events/HTXS_stage1_1_cat_pTjet25GeV", HTXS_stage1_1_cat_pTjet25GeV); successBranches.push_back("Events/HTXS_stage1_1_cat_pTjet25GeV"); } else { missingBranches.push_back("Events/HTXS_stage1_1_cat_pTjet25GeV"); }
    }
    if ( choose["Events/HTXS_stage1_1_cat_pTjet30GeV"] ) {
      if (input->present("Events/HTXS_stage1_1_cat_pTjet30GeV")) { input->select("Events/HTXS_stage1_1_cat_pTjet30GeV", HTXS_stage1_1_cat_pTjet30GeV); successBranches.push_back("Events/HTXS_stage1_1_cat_pTjet30GeV"); } else { missingBranches.push_back("Events/HTXS_stage1_1_cat_pTjet30GeV"); }
    }
    if ( choose["Events/HTXS_stage1_1_fine_cat_pTjet25GeV"] ) {
      if (input->present("Events/HTXS_stage1_1_fine_cat_pTjet25GeV")) { input->select("Events/HTXS_stage1_1_fine_cat_pTjet25GeV", HTXS_stage1_1_fine_cat_pTjet25GeV); successBranches.push_back("Events/HTXS_stage1_1_fine_cat_pTjet25GeV"); } else { missingBranches.push_back("Events/HTXS_stage1_1_fine_cat_pTjet25GeV"); }
    }
    if ( choose["Events/HTXS_stage1_1_fine_cat_pTjet30GeV"] ) {
      if (input->present("Events/HTXS_stage1_1_fine_cat_pTjet30GeV")) { input->select("Events/HTXS_stage1_1_fine_cat_pTjet30GeV", HTXS_stage1_1_fine_cat_pTjet30GeV); successBranches.push_back("Events/HTXS_stage1_1_fine_cat_pTjet30GeV"); } else { missingBranches.push_back("Events/HTXS_stage1_1_fine_cat_pTjet30GeV"); }
    }
    if ( choose["Events/HTXS_stage1_2_cat_pTjet25GeV"] ) {
      if (input->present("Events/HTXS_stage1_2_cat_pTjet25GeV")) { input->select("Events/HTXS_stage1_2_cat_pTjet25GeV", HTXS_stage1_2_cat_pTjet25GeV); successBranches.push_back("Events/HTXS_stage1_2_cat_pTjet25GeV"); } else { missingBranches.push_back("Events/HTXS_stage1_2_cat_pTjet25GeV"); }
    }
    if ( choose["Events/HTXS_stage1_2_cat_pTjet30GeV"] ) {
      if (input->present("Events/HTXS_stage1_2_cat_pTjet30GeV")) { input->select("Events/HTXS_stage1_2_cat_pTjet30GeV", HTXS_stage1_2_cat_pTjet30GeV); successBranches.push_back("Events/HTXS_stage1_2_cat_pTjet30GeV"); } else { missingBranches.push_back("Events/HTXS_stage1_2_cat_pTjet30GeV"); }
    }
    if ( choose["Events/HTXS_stage1_2_fine_cat_pTjet25GeV"] ) {
      if (input->present("Events/HTXS_stage1_2_fine_cat_pTjet25GeV")) { input->select("Events/HTXS_stage1_2_fine_cat_pTjet25GeV", HTXS_stage1_2_fine_cat_pTjet25GeV); successBranches.push_back("Events/HTXS_stage1_2_fine_cat_pTjet25GeV"); } else { missingBranches.push_back("Events/HTXS_stage1_2_fine_cat_pTjet25GeV"); }
    }
    if ( choose["Events/HTXS_stage1_2_fine_cat_pTjet30GeV"] ) {
      if (input->present("Events/HTXS_stage1_2_fine_cat_pTjet30GeV")) { input->select("Events/HTXS_stage1_2_fine_cat_pTjet30GeV", HTXS_stage1_2_fine_cat_pTjet30GeV); successBranches.push_back("Events/HTXS_stage1_2_fine_cat_pTjet30GeV"); } else { missingBranches.push_back("Events/HTXS_stage1_2_fine_cat_pTjet30GeV"); }
    }
    if ( choose["Events/HTXS_stage_0"] ) {
      if (input->present("Events/HTXS_stage_0")) { input->select("Events/HTXS_stage_0", HTXS_stage_0); successBranches.push_back("Events/HTXS_stage_0"); } else { missingBranches.push_back("Events/HTXS_stage_0"); }
    }
    if ( choose["Events/HTXS_stage_1_pTjet25"] ) {
      if (input->present("Events/HTXS_stage_1_pTjet25")) { input->select("Events/HTXS_stage_1_pTjet25", HTXS_stage_1_pTjet25); successBranches.push_back("Events/HTXS_stage_1_pTjet25"); } else { missingBranches.push_back("Events/HTXS_stage_1_pTjet25"); }
    }
    if ( choose["Events/HTXS_stage_1_pTjet30"] ) {
      if (input->present("Events/HTXS_stage_1_pTjet30")) { input->select("Events/HTXS_stage_1_pTjet30", HTXS_stage_1_pTjet30); successBranches.push_back("Events/HTXS_stage_1_pTjet30"); } else { missingBranches.push_back("Events/HTXS_stage_1_pTjet30"); }
    }
    if ( choose["Events/IsoTrack_charge"] ) {
      if (input->present("Events/IsoTrack_charge")) { IsoTrack_charge.resize(66); input->select("Events/IsoTrack_charge", IsoTrack_charge); IsoTrack_charge.clear(); successBranches.push_back("Events/IsoTrack_charge"); usedCounters.insert("nIsoTrack"); } else { missingBranches.push_back("Events/IsoTrack_charge"); }
    }
    if ( choose["Events/IsoTrack_dxy"] ) {
      if (input->present("Events/IsoTrack_dxy")) { IsoTrack_dxy.resize(66); input->select("Events/IsoTrack_dxy", IsoTrack_dxy); IsoTrack_dxy.clear(); successBranches.push_back("Events/IsoTrack_dxy"); usedCounters.insert("nIsoTrack"); } else { missingBranches.push_back("Events/IsoTrack_dxy"); }
    }
    if ( choose["Events/IsoTrack_dz"] ) {
      if (input->present("Events/IsoTrack_dz")) { IsoTrack_dz.resize(66); input->select("Events/IsoTrack_dz", IsoTrack_dz); IsoTrack_dz.clear(); successBranches.push_back("Events/IsoTrack_dz"); usedCounters.insert("nIsoTrack"); } else { missingBranches.push_back("Events/IsoTrack_dz"); }
    }
    if ( choose["Events/IsoTrack_eta"] ) {
      if (input->present("Events/IsoTrack_eta")) { IsoTrack_eta.resize(66); input->select("Events/IsoTrack_eta", IsoTrack_eta); IsoTrack_eta.clear(); successBranches.push_back("Events/IsoTrack_eta"); usedCounters.insert("nIsoTrack"); } else { missingBranches.push_back("Events/IsoTrack_eta"); }
    }
    if ( choose["Events/IsoTrack_fromPV"] ) {
      if (input->present("Events/IsoTrack_fromPV")) { IsoTrack_fromPV.resize(66); input->select("Events/IsoTrack_fromPV", IsoTrack_fromPV); IsoTrack_fromPV.clear(); successBranches.push_back("Events/IsoTrack_fromPV"); usedCounters.insert("nIsoTrack"); } else { missingBranches.push_back("Events/IsoTrack_fromPV"); }
    }
    if ( choose["Events/IsoTrack_isFromLostTrack"] ) {
      if (input->present("Events/IsoTrack_isFromLostTrack")) { IsoTrack_isFromLostTrack.resize(66); input->select("Events/IsoTrack_isFromLostTrack", IsoTrack_isFromLostTrack); IsoTrack_isFromLostTrack.clear(); successBranches.push_back("Events/IsoTrack_isFromLostTrack"); usedCounters.insert("nIsoTrack"); } else { missingBranches.push_back("Events/IsoTrack_isFromLostTrack"); }
    }
    if ( choose["Events/IsoTrack_isHighPurityTrack"] ) {
      if (input->present("Events/IsoTrack_isHighPurityTrack")) { IsoTrack_isHighPurityTrack.resize(66); input->select("Events/IsoTrack_isHighPurityTrack", IsoTrack_isHighPurityTrack); IsoTrack_isHighPurityTrack.clear(); successBranches.push_back("Events/IsoTrack_isHighPurityTrack"); usedCounters.insert("nIsoTrack"); } else { missingBranches.push_back("Events/IsoTrack_isHighPurityTrack"); }
    }
    if ( choose["Events/IsoTrack_isPFcand"] ) {
      if (input->present("Events/IsoTrack_isPFcand")) { IsoTrack_isPFcand.resize(66); input->select("Events/IsoTrack_isPFcand", IsoTrack_isPFcand); IsoTrack_isPFcand.clear(); successBranches.push_back("Events/IsoTrack_isPFcand"); usedCounters.insert("nIsoTrack"); } else { missingBranches.push_back("Events/IsoTrack_isPFcand"); }
    }
    if ( choose["Events/IsoTrack_miniPFRelIso_all"] ) {
      if (input->present("Events/IsoTrack_miniPFRelIso_all")) { IsoTrack_miniPFRelIso_all.resize(66); input->select("Events/IsoTrack_miniPFRelIso_all", IsoTrack_miniPFRelIso_all); IsoTrack_miniPFRelIso_all.clear(); successBranches.push_back("Events/IsoTrack_miniPFRelIso_all"); usedCounters.insert("nIsoTrack"); } else { missingBranches.push_back("Events/IsoTrack_miniPFRelIso_all"); }
    }
    if ( choose["Events/IsoTrack_miniPFRelIso_chg"] ) {
      if (input->present("Events/IsoTrack_miniPFRelIso_chg")) { IsoTrack_miniPFRelIso_chg.resize(66); input->select("Events/IsoTrack_miniPFRelIso_chg", IsoTrack_miniPFRelIso_chg); IsoTrack_miniPFRelIso_chg.clear(); successBranches.push_back("Events/IsoTrack_miniPFRelIso_chg"); usedCounters.insert("nIsoTrack"); } else { missingBranches.push_back("Events/IsoTrack_miniPFRelIso_chg"); }
    }
    if ( choose["Events/IsoTrack_pdgId"] ) {
      if (input->present("Events/IsoTrack_pdgId")) { IsoTrack_pdgId.resize(66); input->select("Events/IsoTrack_pdgId", IsoTrack_pdgId); IsoTrack_pdgId.clear(); successBranches.push_back("Events/IsoTrack_pdgId"); usedCounters.insert("nIsoTrack"); } else { missingBranches.push_back("Events/IsoTrack_pdgId"); }
    }
    if ( choose["Events/IsoTrack_pfRelIso03_all"] ) {
      if (input->present("Events/IsoTrack_pfRelIso03_all")) { IsoTrack_pfRelIso03_all.resize(66); input->select("Events/IsoTrack_pfRelIso03_all", IsoTrack_pfRelIso03_all); IsoTrack_pfRelIso03_all.clear(); successBranches.push_back("Events/IsoTrack_pfRelIso03_all"); usedCounters.insert("nIsoTrack"); } else { missingBranches.push_back("Events/IsoTrack_pfRelIso03_all"); }
    }
    if ( choose["Events/IsoTrack_pfRelIso03_chg"] ) {
      if (input->present("Events/IsoTrack_pfRelIso03_chg")) { IsoTrack_pfRelIso03_chg.resize(66); input->select("Events/IsoTrack_pfRelIso03_chg", IsoTrack_pfRelIso03_chg); IsoTrack_pfRelIso03_chg.clear(); successBranches.push_back("Events/IsoTrack_pfRelIso03_chg"); usedCounters.insert("nIsoTrack"); } else { missingBranches.push_back("Events/IsoTrack_pfRelIso03_chg"); }
    }
    if ( choose["Events/IsoTrack_phi"] ) {
      if (input->present("Events/IsoTrack_phi")) { IsoTrack_phi.resize(66); input->select("Events/IsoTrack_phi", IsoTrack_phi); IsoTrack_phi.clear(); successBranches.push_back("Events/IsoTrack_phi"); usedCounters.insert("nIsoTrack"); } else { missingBranches.push_back("Events/IsoTrack_phi"); }
    }
    if ( choose["Events/IsoTrack_pt"] ) {
      if (input->present("Events/IsoTrack_pt")) { IsoTrack_pt.resize(66); input->select("Events/IsoTrack_pt", IsoTrack_pt); IsoTrack_pt.clear(); successBranches.push_back("Events/IsoTrack_pt"); usedCounters.insert("nIsoTrack"); } else { missingBranches.push_back("Events/IsoTrack_pt"); }
    }
    if ( choose["Events/Jet_PNetRegPtRawCorr"] ) {
      if (input->present("Events/Jet_PNetRegPtRawCorr")) { Jet_PNetRegPtRawCorr.resize(128); input->select("Events/Jet_PNetRegPtRawCorr", Jet_PNetRegPtRawCorr); Jet_PNetRegPtRawCorr.clear(); successBranches.push_back("Events/Jet_PNetRegPtRawCorr"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_PNetRegPtRawCorr"); }
    }
    if ( choose["Events/Jet_PNetRegPtRawCorrNeutrino"] ) {
      if (input->present("Events/Jet_PNetRegPtRawCorrNeutrino")) { Jet_PNetRegPtRawCorrNeutrino.resize(128); input->select("Events/Jet_PNetRegPtRawCorrNeutrino", Jet_PNetRegPtRawCorrNeutrino); Jet_PNetRegPtRawCorrNeutrino.clear(); successBranches.push_back("Events/Jet_PNetRegPtRawCorrNeutrino"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_PNetRegPtRawCorrNeutrino"); }
    }
    if ( choose["Events/Jet_PNetRegPtRawRes"] ) {
      if (input->present("Events/Jet_PNetRegPtRawRes")) { Jet_PNetRegPtRawRes.resize(128); input->select("Events/Jet_PNetRegPtRawRes", Jet_PNetRegPtRawRes); Jet_PNetRegPtRawRes.clear(); successBranches.push_back("Events/Jet_PNetRegPtRawRes"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_PNetRegPtRawRes"); }
    }
    if ( choose["Events/Jet_UParTAK4RegPtRawCorr"] ) {
      if (input->present("Events/Jet_UParTAK4RegPtRawCorr")) { Jet_UParTAK4RegPtRawCorr.resize(128); input->select("Events/Jet_UParTAK4RegPtRawCorr", Jet_UParTAK4RegPtRawCorr); Jet_UParTAK4RegPtRawCorr.clear(); successBranches.push_back("Events/Jet_UParTAK4RegPtRawCorr"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_UParTAK4RegPtRawCorr"); }
    }
    if ( choose["Events/Jet_UParTAK4RegPtRawCorrNeutrino"] ) {
      if (input->present("Events/Jet_UParTAK4RegPtRawCorrNeutrino")) { Jet_UParTAK4RegPtRawCorrNeutrino.resize(128); input->select("Events/Jet_UParTAK4RegPtRawCorrNeutrino", Jet_UParTAK4RegPtRawCorrNeutrino); Jet_UParTAK4RegPtRawCorrNeutrino.clear(); successBranches.push_back("Events/Jet_UParTAK4RegPtRawCorrNeutrino"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_UParTAK4RegPtRawCorrNeutrino"); }
    }
    if ( choose["Events/Jet_UParTAK4RegPtRawRes"] ) {
      if (input->present("Events/Jet_UParTAK4RegPtRawRes")) { Jet_UParTAK4RegPtRawRes.resize(128); input->select("Events/Jet_UParTAK4RegPtRawRes", Jet_UParTAK4RegPtRawRes); Jet_UParTAK4RegPtRawRes.clear(); successBranches.push_back("Events/Jet_UParTAK4RegPtRawRes"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_UParTAK4RegPtRawRes"); }
    }
    if ( choose["Events/Jet_UParTAK4V1RegPtRawCorr"] ) {
      if (input->present("Events/Jet_UParTAK4V1RegPtRawCorr")) { Jet_UParTAK4V1RegPtRawCorr.resize(128); input->select("Events/Jet_UParTAK4V1RegPtRawCorr", Jet_UParTAK4V1RegPtRawCorr); Jet_UParTAK4V1RegPtRawCorr.clear(); successBranches.push_back("Events/Jet_UParTAK4V1RegPtRawCorr"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_UParTAK4V1RegPtRawCorr"); }
    }
    if ( choose["Events/Jet_UParTAK4V1RegPtRawCorrNeutrino"] ) {
      if (input->present("Events/Jet_UParTAK4V1RegPtRawCorrNeutrino")) { Jet_UParTAK4V1RegPtRawCorrNeutrino.resize(128); input->select("Events/Jet_UParTAK4V1RegPtRawCorrNeutrino", Jet_UParTAK4V1RegPtRawCorrNeutrino); Jet_UParTAK4V1RegPtRawCorrNeutrino.clear(); successBranches.push_back("Events/Jet_UParTAK4V1RegPtRawCorrNeutrino"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_UParTAK4V1RegPtRawCorrNeutrino"); }
    }
    if ( choose["Events/Jet_UParTAK4V1RegPtRawRes"] ) {
      if (input->present("Events/Jet_UParTAK4V1RegPtRawRes")) { Jet_UParTAK4V1RegPtRawRes.resize(128); input->select("Events/Jet_UParTAK4V1RegPtRawRes", Jet_UParTAK4V1RegPtRawRes); Jet_UParTAK4V1RegPtRawRes.clear(); successBranches.push_back("Events/Jet_UParTAK4V1RegPtRawRes"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_UParTAK4V1RegPtRawRes"); }
    }
    if ( choose["Events/Jet_area"] ) {
      if (input->present("Events/Jet_area")) { Jet_area.resize(128); input->select("Events/Jet_area", Jet_area); Jet_area.clear(); successBranches.push_back("Events/Jet_area"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_area"); }
    }
    if ( choose["Events/Jet_bRegCorr"] ) {
      if (input->present("Events/Jet_bRegCorr")) { Jet_bRegCorr.resize(128); input->select("Events/Jet_bRegCorr", Jet_bRegCorr); Jet_bRegCorr.clear(); successBranches.push_back("Events/Jet_bRegCorr"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_bRegCorr"); }
    }
    if ( choose["Events/Jet_bRegRes"] ) {
      if (input->present("Events/Jet_bRegRes")) { Jet_bRegRes.resize(128); input->select("Events/Jet_bRegRes", Jet_bRegRes); Jet_bRegRes.clear(); successBranches.push_back("Events/Jet_bRegRes"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_bRegRes"); }
    }
    if ( choose["Events/Jet_btagCSVV2"] ) {
      if (input->present("Events/Jet_btagCSVV2")) { Jet_btagCSVV2.resize(128); input->select("Events/Jet_btagCSVV2", Jet_btagCSVV2); Jet_btagCSVV2.clear(); successBranches.push_back("Events/Jet_btagCSVV2"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_btagCSVV2"); }
    }
    if ( choose["Events/Jet_btagDeepB"] ) {
      if (input->present("Events/Jet_btagDeepB")) { Jet_btagDeepB.resize(128); input->select("Events/Jet_btagDeepB", Jet_btagDeepB); Jet_btagDeepB.clear(); successBranches.push_back("Events/Jet_btagDeepB"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_btagDeepB"); }
    }
    if ( choose["Events/Jet_btagDeepCvB"] ) {
      if (input->present("Events/Jet_btagDeepCvB")) { Jet_btagDeepCvB.resize(128); input->select("Events/Jet_btagDeepCvB", Jet_btagDeepCvB); Jet_btagDeepCvB.clear(); successBranches.push_back("Events/Jet_btagDeepCvB"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_btagDeepCvB"); }
    }
    if ( choose["Events/Jet_btagDeepCvL"] ) {
      if (input->present("Events/Jet_btagDeepCvL")) { Jet_btagDeepCvL.resize(128); input->select("Events/Jet_btagDeepCvL", Jet_btagDeepCvL); Jet_btagDeepCvL.clear(); successBranches.push_back("Events/Jet_btagDeepCvL"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_btagDeepCvL"); }
    }
    if ( choose["Events/Jet_btagDeepFlavB"] ) {
      if (input->present("Events/Jet_btagDeepFlavB")) { Jet_btagDeepFlavB.resize(128); input->select("Events/Jet_btagDeepFlavB", Jet_btagDeepFlavB); Jet_btagDeepFlavB.clear(); successBranches.push_back("Events/Jet_btagDeepFlavB"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_btagDeepFlavB"); }
    }
    if ( choose["Events/Jet_btagDeepFlavCvB"] ) {
      if (input->present("Events/Jet_btagDeepFlavCvB")) { Jet_btagDeepFlavCvB.resize(128); input->select("Events/Jet_btagDeepFlavCvB", Jet_btagDeepFlavCvB); Jet_btagDeepFlavCvB.clear(); successBranches.push_back("Events/Jet_btagDeepFlavCvB"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_btagDeepFlavCvB"); }
    }
    if ( choose["Events/Jet_btagDeepFlavCvL"] ) {
      if (input->present("Events/Jet_btagDeepFlavCvL")) { Jet_btagDeepFlavCvL.resize(128); input->select("Events/Jet_btagDeepFlavCvL", Jet_btagDeepFlavCvL); Jet_btagDeepFlavCvL.clear(); successBranches.push_back("Events/Jet_btagDeepFlavCvL"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_btagDeepFlavCvL"); }
    }
    if ( choose["Events/Jet_btagDeepFlavQG"] ) {
      if (input->present("Events/Jet_btagDeepFlavQG")) { Jet_btagDeepFlavQG.resize(128); input->select("Events/Jet_btagDeepFlavQG", Jet_btagDeepFlavQG); Jet_btagDeepFlavQG.clear(); successBranches.push_back("Events/Jet_btagDeepFlavQG"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_btagDeepFlavQG"); }
    }
    if ( choose["Events/Jet_btagPNetB"] ) {
      if (input->present("Events/Jet_btagPNetB")) { Jet_btagPNetB.resize(128); input->select("Events/Jet_btagPNetB", Jet_btagPNetB); Jet_btagPNetB.clear(); successBranches.push_back("Events/Jet_btagPNetB"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_btagPNetB"); }
    }
    if ( choose["Events/Jet_btagPNetCvB"] ) {
      if (input->present("Events/Jet_btagPNetCvB")) { Jet_btagPNetCvB.resize(128); input->select("Events/Jet_btagPNetCvB", Jet_btagPNetCvB); Jet_btagPNetCvB.clear(); successBranches.push_back("Events/Jet_btagPNetCvB"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_btagPNetCvB"); }
    }
    if ( choose["Events/Jet_btagPNetCvL"] ) {
      if (input->present("Events/Jet_btagPNetCvL")) { Jet_btagPNetCvL.resize(128); input->select("Events/Jet_btagPNetCvL", Jet_btagPNetCvL); Jet_btagPNetCvL.clear(); successBranches.push_back("Events/Jet_btagPNetCvL"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_btagPNetCvL"); }
    }
    if ( choose["Events/Jet_btagPNetQvG"] ) {
      if (input->present("Events/Jet_btagPNetQvG")) { Jet_btagPNetQvG.resize(128); input->select("Events/Jet_btagPNetQvG", Jet_btagPNetQvG); Jet_btagPNetQvG.clear(); successBranches.push_back("Events/Jet_btagPNetQvG"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_btagPNetQvG"); }
    }
    if ( choose["Events/Jet_btagUParTAK4B"] ) {
      if (input->present("Events/Jet_btagUParTAK4B")) { Jet_btagUParTAK4B.resize(128); input->select("Events/Jet_btagUParTAK4B", Jet_btagUParTAK4B); Jet_btagUParTAK4B.clear(); successBranches.push_back("Events/Jet_btagUParTAK4B"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_btagUParTAK4B"); }
    }
    if ( choose["Events/Jet_btagUParTAK4CvB"] ) {
      if (input->present("Events/Jet_btagUParTAK4CvB")) { Jet_btagUParTAK4CvB.resize(128); input->select("Events/Jet_btagUParTAK4CvB", Jet_btagUParTAK4CvB); Jet_btagUParTAK4CvB.clear(); successBranches.push_back("Events/Jet_btagUParTAK4CvB"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_btagUParTAK4CvB"); }
    }
    if ( choose["Events/Jet_btagUParTAK4CvL"] ) {
      if (input->present("Events/Jet_btagUParTAK4CvL")) { Jet_btagUParTAK4CvL.resize(128); input->select("Events/Jet_btagUParTAK4CvL", Jet_btagUParTAK4CvL); Jet_btagUParTAK4CvL.clear(); successBranches.push_back("Events/Jet_btagUParTAK4CvL"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_btagUParTAK4CvL"); }
    }
    if ( choose["Events/Jet_btagUParTAK4QvG"] ) {
      if (input->present("Events/Jet_btagUParTAK4QvG")) { Jet_btagUParTAK4QvG.resize(128); input->select("Events/Jet_btagUParTAK4QvG", Jet_btagUParTAK4QvG); Jet_btagUParTAK4QvG.clear(); successBranches.push_back("Events/Jet_btagUParTAK4QvG"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_btagUParTAK4QvG"); }
    }
    if ( choose["Events/Jet_cRegCorr"] ) {
      if (input->present("Events/Jet_cRegCorr")) { Jet_cRegCorr.resize(128); input->select("Events/Jet_cRegCorr", Jet_cRegCorr); Jet_cRegCorr.clear(); successBranches.push_back("Events/Jet_cRegCorr"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_cRegCorr"); }
    }
    if ( choose["Events/Jet_cRegRes"] ) {
      if (input->present("Events/Jet_cRegRes")) { Jet_cRegRes.resize(128); input->select("Events/Jet_cRegRes", Jet_cRegRes); Jet_cRegRes.clear(); successBranches.push_back("Events/Jet_cRegRes"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_cRegRes"); }
    }
    if ( choose["Events/Jet_chEmEF"] ) {
      if (input->present("Events/Jet_chEmEF")) { Jet_chEmEF.resize(128); input->select("Events/Jet_chEmEF", Jet_chEmEF); Jet_chEmEF.clear(); successBranches.push_back("Events/Jet_chEmEF"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_chEmEF"); }
    }
    if ( choose["Events/Jet_chFPV0EF"] ) {
      if (input->present("Events/Jet_chFPV0EF")) { Jet_chFPV0EF.resize(128); input->select("Events/Jet_chFPV0EF", Jet_chFPV0EF); Jet_chFPV0EF.clear(); successBranches.push_back("Events/Jet_chFPV0EF"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_chFPV0EF"); }
    }
    if ( choose["Events/Jet_chHEF"] ) {
      if (input->present("Events/Jet_chHEF")) { Jet_chHEF.resize(128); input->select("Events/Jet_chHEF", Jet_chHEF); Jet_chHEF.clear(); successBranches.push_back("Events/Jet_chHEF"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_chHEF"); }
    }
    if ( choose["Events/Jet_chMultiplicity"] ) {
      if (input->present("Events/Jet_chMultiplicity")) { Jet_chMultiplicity.resize(128); input->select("Events/Jet_chMultiplicity", Jet_chMultiplicity); Jet_chMultiplicity.clear(); successBranches.push_back("Events/Jet_chMultiplicity"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_chMultiplicity"); }
    }
    if ( choose["Events/Jet_cleanmask"] ) {
      if (input->present("Events/Jet_cleanmask")) { Jet_cleanmask.resize(128); input->select("Events/Jet_cleanmask", Jet_cleanmask); Jet_cleanmask.clear(); successBranches.push_back("Events/Jet_cleanmask"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_cleanmask"); }
    }
    if ( choose["Events/Jet_electronIdx1"] ) {
      if (input->present("Events/Jet_electronIdx1")) { Jet_electronIdx1.resize(128); input->select("Events/Jet_electronIdx1", Jet_electronIdx1); Jet_electronIdx1.clear(); successBranches.push_back("Events/Jet_electronIdx1"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_electronIdx1"); }
    }
    if ( choose["Events/Jet_electronIdx2"] ) {
      if (input->present("Events/Jet_electronIdx2")) { Jet_electronIdx2.resize(128); input->select("Events/Jet_electronIdx2", Jet_electronIdx2); Jet_electronIdx2.clear(); successBranches.push_back("Events/Jet_electronIdx2"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_electronIdx2"); }
    }
    if ( choose["Events/Jet_eta"] ) {
      if (input->present("Events/Jet_eta")) { Jet_eta.resize(128); input->select("Events/Jet_eta", Jet_eta); Jet_eta.clear(); successBranches.push_back("Events/Jet_eta"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_eta"); }
    }
    if ( choose["Events/Jet_genJetIdx"] ) {
      if (input->present("Events/Jet_genJetIdx")) { Jet_genJetIdx.resize(128); input->select("Events/Jet_genJetIdx", Jet_genJetIdx); Jet_genJetIdx.clear(); successBranches.push_back("Events/Jet_genJetIdx"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_genJetIdx"); }
    }
    if ( choose["Events/Jet_hadronFlavour"] ) {
      if (input->present("Events/Jet_hadronFlavour")) { Jet_hadronFlavour.resize(128); input->select("Events/Jet_hadronFlavour", Jet_hadronFlavour); Jet_hadronFlavour.clear(); successBranches.push_back("Events/Jet_hadronFlavour"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_hadronFlavour"); }
    }
    if ( choose["Events/Jet_hfEmEF"] ) {
      if (input->present("Events/Jet_hfEmEF")) { Jet_hfEmEF.resize(128); input->select("Events/Jet_hfEmEF", Jet_hfEmEF); Jet_hfEmEF.clear(); successBranches.push_back("Events/Jet_hfEmEF"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_hfEmEF"); }
    }
    if ( choose["Events/Jet_hfHEF"] ) {
      if (input->present("Events/Jet_hfHEF")) { Jet_hfHEF.resize(128); input->select("Events/Jet_hfHEF", Jet_hfHEF); Jet_hfHEF.clear(); successBranches.push_back("Events/Jet_hfHEF"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_hfHEF"); }
    }
    if ( choose["Events/Jet_hfadjacentEtaStripsSize"] ) {
      if (input->present("Events/Jet_hfadjacentEtaStripsSize")) { Jet_hfadjacentEtaStripsSize.resize(128); input->select("Events/Jet_hfadjacentEtaStripsSize", Jet_hfadjacentEtaStripsSize); Jet_hfadjacentEtaStripsSize.clear(); successBranches.push_back("Events/Jet_hfadjacentEtaStripsSize"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_hfadjacentEtaStripsSize"); }
    }
    if ( choose["Events/Jet_hfcentralEtaStripSize"] ) {
      if (input->present("Events/Jet_hfcentralEtaStripSize")) { Jet_hfcentralEtaStripSize.resize(128); input->select("Events/Jet_hfcentralEtaStripSize", Jet_hfcentralEtaStripSize); Jet_hfcentralEtaStripSize.clear(); successBranches.push_back("Events/Jet_hfcentralEtaStripSize"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_hfcentralEtaStripSize"); }
    }
    if ( choose["Events/Jet_hfsigmaEtaEta"] ) {
      if (input->present("Events/Jet_hfsigmaEtaEta")) { Jet_hfsigmaEtaEta.resize(128); input->select("Events/Jet_hfsigmaEtaEta", Jet_hfsigmaEtaEta); Jet_hfsigmaEtaEta.clear(); successBranches.push_back("Events/Jet_hfsigmaEtaEta"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_hfsigmaEtaEta"); }
    }
    if ( choose["Events/Jet_hfsigmaPhiPhi"] ) {
      if (input->present("Events/Jet_hfsigmaPhiPhi")) { Jet_hfsigmaPhiPhi.resize(128); input->select("Events/Jet_hfsigmaPhiPhi", Jet_hfsigmaPhiPhi); Jet_hfsigmaPhiPhi.clear(); successBranches.push_back("Events/Jet_hfsigmaPhiPhi"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_hfsigmaPhiPhi"); }
    }
    if ( choose["Events/Jet_jetId"] ) {
      if (input->present("Events/Jet_jetId")) { Jet_jetId.resize(128); input->select("Events/Jet_jetId", Jet_jetId); Jet_jetId.clear(); successBranches.push_back("Events/Jet_jetId"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_jetId"); }
    }
    if ( choose["Events/Jet_mass"] ) {
      if (input->present("Events/Jet_mass")) { Jet_mass.resize(128); input->select("Events/Jet_mass", Jet_mass); Jet_mass.clear(); successBranches.push_back("Events/Jet_mass"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_mass"); }
    }
    if ( choose["Events/Jet_muEF"] ) {
      if (input->present("Events/Jet_muEF")) { Jet_muEF.resize(128); input->select("Events/Jet_muEF", Jet_muEF); Jet_muEF.clear(); successBranches.push_back("Events/Jet_muEF"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_muEF"); }
    }
    if ( choose["Events/Jet_muonIdx1"] ) {
      if (input->present("Events/Jet_muonIdx1")) { Jet_muonIdx1.resize(128); input->select("Events/Jet_muonIdx1", Jet_muonIdx1); Jet_muonIdx1.clear(); successBranches.push_back("Events/Jet_muonIdx1"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_muonIdx1"); }
    }
    if ( choose["Events/Jet_muonIdx2"] ) {
      if (input->present("Events/Jet_muonIdx2")) { Jet_muonIdx2.resize(128); input->select("Events/Jet_muonIdx2", Jet_muonIdx2); Jet_muonIdx2.clear(); successBranches.push_back("Events/Jet_muonIdx2"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_muonIdx2"); }
    }
    if ( choose["Events/Jet_muonSubtrFactor"] ) {
      if (input->present("Events/Jet_muonSubtrFactor")) { Jet_muonSubtrFactor.resize(128); input->select("Events/Jet_muonSubtrFactor", Jet_muonSubtrFactor); Jet_muonSubtrFactor.clear(); successBranches.push_back("Events/Jet_muonSubtrFactor"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_muonSubtrFactor"); }
    }
    if ( choose["Events/Jet_nConstituents"] ) {
      if (input->present("Events/Jet_nConstituents")) { Jet_nConstituents.resize(128); input->select("Events/Jet_nConstituents", Jet_nConstituents); Jet_nConstituents.clear(); successBranches.push_back("Events/Jet_nConstituents"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_nConstituents"); }
    }
    if ( choose["Events/Jet_nElectrons"] ) {
      if (input->present("Events/Jet_nElectrons")) { Jet_nElectrons.resize(128); input->select("Events/Jet_nElectrons", Jet_nElectrons); Jet_nElectrons.clear(); successBranches.push_back("Events/Jet_nElectrons"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_nElectrons"); }
    }
    if ( choose["Events/Jet_nMuons"] ) {
      if (input->present("Events/Jet_nMuons")) { Jet_nMuons.resize(128); input->select("Events/Jet_nMuons", Jet_nMuons); Jet_nMuons.clear(); successBranches.push_back("Events/Jet_nMuons"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_nMuons"); }
    }
    if ( choose["Events/Jet_neEmEF"] ) {
      if (input->present("Events/Jet_neEmEF")) { Jet_neEmEF.resize(128); input->select("Events/Jet_neEmEF", Jet_neEmEF); Jet_neEmEF.clear(); successBranches.push_back("Events/Jet_neEmEF"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_neEmEF"); }
    }
    if ( choose["Events/Jet_neHEF"] ) {
      if (input->present("Events/Jet_neHEF")) { Jet_neHEF.resize(128); input->select("Events/Jet_neHEF", Jet_neHEF); Jet_neHEF.clear(); successBranches.push_back("Events/Jet_neHEF"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_neHEF"); }
    }
    if ( choose["Events/Jet_neMultiplicity"] ) {
      if (input->present("Events/Jet_neMultiplicity")) { Jet_neMultiplicity.resize(128); input->select("Events/Jet_neMultiplicity", Jet_neMultiplicity); Jet_neMultiplicity.clear(); successBranches.push_back("Events/Jet_neMultiplicity"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_neMultiplicity"); }
    }
    if ( choose["Events/Jet_partonFlavour"] ) {
      if (input->present("Events/Jet_partonFlavour")) { Jet_partonFlavour.resize(128); input->select("Events/Jet_partonFlavour", Jet_partonFlavour); Jet_partonFlavour.clear(); successBranches.push_back("Events/Jet_partonFlavour"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_partonFlavour"); }
    }
    if ( choose["Events/Jet_phi"] ) {
      if (input->present("Events/Jet_phi")) { Jet_phi.resize(128); input->select("Events/Jet_phi", Jet_phi); Jet_phi.clear(); successBranches.push_back("Events/Jet_phi"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_phi"); }
    }
    if ( choose["Events/Jet_pt"] ) {
      if (input->present("Events/Jet_pt")) { Jet_pt.resize(128); input->select("Events/Jet_pt", Jet_pt); Jet_pt.clear(); successBranches.push_back("Events/Jet_pt"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_pt"); }
    }
    if ( choose["Events/Jet_puId"] ) {
      if (input->present("Events/Jet_puId")) { Jet_puId.resize(128); input->select("Events/Jet_puId", Jet_puId); Jet_puId.clear(); successBranches.push_back("Events/Jet_puId"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_puId"); }
    }
    if ( choose["Events/Jet_puIdDisc"] ) {
      if (input->present("Events/Jet_puIdDisc")) { Jet_puIdDisc.resize(128); input->select("Events/Jet_puIdDisc", Jet_puIdDisc); Jet_puIdDisc.clear(); successBranches.push_back("Events/Jet_puIdDisc"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_puIdDisc"); }
    }
    if ( choose["Events/Jet_qgl"] ) {
      if (input->present("Events/Jet_qgl")) { Jet_qgl.resize(128); input->select("Events/Jet_qgl", Jet_qgl); Jet_qgl.clear(); successBranches.push_back("Events/Jet_qgl"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_qgl"); }
    }
    if ( choose["Events/Jet_rawFactor"] ) {
      if (input->present("Events/Jet_rawFactor")) { Jet_rawFactor.resize(128); input->select("Events/Jet_rawFactor", Jet_rawFactor); Jet_rawFactor.clear(); successBranches.push_back("Events/Jet_rawFactor"); usedCounters.insert("nJet"); } else { missingBranches.push_back("Events/Jet_rawFactor"); }
    }
    if ( choose["Events/L1PreFiringWeight_Dn"] ) {
      if (input->present("Events/L1PreFiringWeight_Dn")) { input->select("Events/L1PreFiringWeight_Dn", L1PreFiringWeight_Dn); successBranches.push_back("Events/L1PreFiringWeight_Dn"); } else { missingBranches.push_back("Events/L1PreFiringWeight_Dn"); }
    }
    if ( choose["Events/L1PreFiringWeight_ECAL_Dn"] ) {
      if (input->present("Events/L1PreFiringWeight_ECAL_Dn")) { input->select("Events/L1PreFiringWeight_ECAL_Dn", L1PreFiringWeight_ECAL_Dn); successBranches.push_back("Events/L1PreFiringWeight_ECAL_Dn"); } else { missingBranches.push_back("Events/L1PreFiringWeight_ECAL_Dn"); }
    }
    if ( choose["Events/L1PreFiringWeight_ECAL_Nom"] ) {
      if (input->present("Events/L1PreFiringWeight_ECAL_Nom")) { input->select("Events/L1PreFiringWeight_ECAL_Nom", L1PreFiringWeight_ECAL_Nom); successBranches.push_back("Events/L1PreFiringWeight_ECAL_Nom"); } else { missingBranches.push_back("Events/L1PreFiringWeight_ECAL_Nom"); }
    }
    if ( choose["Events/L1PreFiringWeight_ECAL_Up"] ) {
      if (input->present("Events/L1PreFiringWeight_ECAL_Up")) { input->select("Events/L1PreFiringWeight_ECAL_Up", L1PreFiringWeight_ECAL_Up); successBranches.push_back("Events/L1PreFiringWeight_ECAL_Up"); } else { missingBranches.push_back("Events/L1PreFiringWeight_ECAL_Up"); }
    }
    if ( choose["Events/L1PreFiringWeight_Muon_Nom"] ) {
      if (input->present("Events/L1PreFiringWeight_Muon_Nom")) { input->select("Events/L1PreFiringWeight_Muon_Nom", L1PreFiringWeight_Muon_Nom); successBranches.push_back("Events/L1PreFiringWeight_Muon_Nom"); } else { missingBranches.push_back("Events/L1PreFiringWeight_Muon_Nom"); }
    }
    if ( choose["Events/L1PreFiringWeight_Muon_StatDn"] ) {
      if (input->present("Events/L1PreFiringWeight_Muon_StatDn")) { input->select("Events/L1PreFiringWeight_Muon_StatDn", L1PreFiringWeight_Muon_StatDn); successBranches.push_back("Events/L1PreFiringWeight_Muon_StatDn"); } else { missingBranches.push_back("Events/L1PreFiringWeight_Muon_StatDn"); }
    }
    if ( choose["Events/L1PreFiringWeight_Muon_StatUp"] ) {
      if (input->present("Events/L1PreFiringWeight_Muon_StatUp")) { input->select("Events/L1PreFiringWeight_Muon_StatUp", L1PreFiringWeight_Muon_StatUp); successBranches.push_back("Events/L1PreFiringWeight_Muon_StatUp"); } else { missingBranches.push_back("Events/L1PreFiringWeight_Muon_StatUp"); }
    }
    if ( choose["Events/L1PreFiringWeight_Muon_SystDn"] ) {
      if (input->present("Events/L1PreFiringWeight_Muon_SystDn")) { input->select("Events/L1PreFiringWeight_Muon_SystDn", L1PreFiringWeight_Muon_SystDn); successBranches.push_back("Events/L1PreFiringWeight_Muon_SystDn"); } else { missingBranches.push_back("Events/L1PreFiringWeight_Muon_SystDn"); }
    }
    if ( choose["Events/L1PreFiringWeight_Muon_SystUp"] ) {
      if (input->present("Events/L1PreFiringWeight_Muon_SystUp")) { input->select("Events/L1PreFiringWeight_Muon_SystUp", L1PreFiringWeight_Muon_SystUp); successBranches.push_back("Events/L1PreFiringWeight_Muon_SystUp"); } else { missingBranches.push_back("Events/L1PreFiringWeight_Muon_SystUp"); }
    }
    if ( choose["Events/L1PreFiringWeight_Nom"] ) {
      if (input->present("Events/L1PreFiringWeight_Nom")) { input->select("Events/L1PreFiringWeight_Nom", L1PreFiringWeight_Nom); successBranches.push_back("Events/L1PreFiringWeight_Nom"); } else { missingBranches.push_back("Events/L1PreFiringWeight_Nom"); }
    }
    if ( choose["Events/L1PreFiringWeight_Up"] ) {
      if (input->present("Events/L1PreFiringWeight_Up")) { input->select("Events/L1PreFiringWeight_Up", L1PreFiringWeight_Up); successBranches.push_back("Events/L1PreFiringWeight_Up"); } else { missingBranches.push_back("Events/L1PreFiringWeight_Up"); }
    }
    if ( choose["Events/L1Reco_step"] ) {
      if (input->present("Events/L1Reco_step")) { input->select("Events/L1Reco_step", L1Reco_step); successBranches.push_back("Events/L1Reco_step"); } else { missingBranches.push_back("Events/L1Reco_step"); }
    }
    if ( choose["Events/L1simulation_step"] ) {
      if (input->present("Events/L1simulation_step")) { input->select("Events/L1simulation_step", L1simulation_step); successBranches.push_back("Events/L1simulation_step"); } else { missingBranches.push_back("Events/L1simulation_step"); }
    }
    if ( choose["Events/LHEPart_eta"] ) {
      if (input->present("Events/LHEPart_eta")) { LHEPart_eta.resize(49); input->select("Events/LHEPart_eta", LHEPart_eta); LHEPart_eta.clear(); successBranches.push_back("Events/LHEPart_eta"); usedCounters.insert("nLHEPart"); } else { missingBranches.push_back("Events/LHEPart_eta"); }
    }
    if ( choose["Events/LHEPart_incomingpz"] ) {
      if (input->present("Events/LHEPart_incomingpz")) { LHEPart_incomingpz.resize(49); input->select("Events/LHEPart_incomingpz", LHEPart_incomingpz); LHEPart_incomingpz.clear(); successBranches.push_back("Events/LHEPart_incomingpz"); usedCounters.insert("nLHEPart"); } else { missingBranches.push_back("Events/LHEPart_incomingpz"); }
    }
    if ( choose["Events/LHEPart_mass"] ) {
      if (input->present("Events/LHEPart_mass")) { LHEPart_mass.resize(49); input->select("Events/LHEPart_mass", LHEPart_mass); LHEPart_mass.clear(); successBranches.push_back("Events/LHEPart_mass"); usedCounters.insert("nLHEPart"); } else { missingBranches.push_back("Events/LHEPart_mass"); }
    }
    if ( choose["Events/LHEPart_pdgId"] ) {
      if (input->present("Events/LHEPart_pdgId")) { LHEPart_pdgId.resize(49); input->select("Events/LHEPart_pdgId", LHEPart_pdgId); LHEPart_pdgId.clear(); successBranches.push_back("Events/LHEPart_pdgId"); usedCounters.insert("nLHEPart"); } else { missingBranches.push_back("Events/LHEPart_pdgId"); }
    }
    if ( choose["Events/LHEPart_phi"] ) {
      if (input->present("Events/LHEPart_phi")) { LHEPart_phi.resize(49); input->select("Events/LHEPart_phi", LHEPart_phi); LHEPart_phi.clear(); successBranches.push_back("Events/LHEPart_phi"); usedCounters.insert("nLHEPart"); } else { missingBranches.push_back("Events/LHEPart_phi"); }
    }
    if ( choose["Events/LHEPart_pt"] ) {
      if (input->present("Events/LHEPart_pt")) { LHEPart_pt.resize(49); input->select("Events/LHEPart_pt", LHEPart_pt); LHEPart_pt.clear(); successBranches.push_back("Events/LHEPart_pt"); usedCounters.insert("nLHEPart"); } else { missingBranches.push_back("Events/LHEPart_pt"); }
    }
    if ( choose["Events/LHEPart_spin"] ) {
      if (input->present("Events/LHEPart_spin")) { LHEPart_spin.resize(49); input->select("Events/LHEPart_spin", LHEPart_spin); LHEPart_spin.clear(); successBranches.push_back("Events/LHEPart_spin"); usedCounters.insert("nLHEPart"); } else { missingBranches.push_back("Events/LHEPart_spin"); }
    }
    if ( choose["Events/LHEPart_status"] ) {
      if (input->present("Events/LHEPart_status")) { LHEPart_status.resize(49); input->select("Events/LHEPart_status", LHEPart_status); LHEPart_status.clear(); successBranches.push_back("Events/LHEPart_status"); usedCounters.insert("nLHEPart"); } else { missingBranches.push_back("Events/LHEPart_status"); }
    }
    if ( choose["Events/LHEPdfWeight"] ) {
      if (input->present("Events/LHEPdfWeight")) { LHEPdfWeight.resize(199); input->select("Events/LHEPdfWeight", LHEPdfWeight); LHEPdfWeight.clear(); successBranches.push_back("Events/LHEPdfWeight"); usedCounters.insert("nLHEPdfWeight"); } else { missingBranches.push_back("Events/LHEPdfWeight"); }
    }
    if ( choose["Events/LHEReweightingWeight"] ) {
      if (input->present("Events/LHEReweightingWeight")) { LHEReweightingWeight.resize(144); input->select("Events/LHEReweightingWeight", LHEReweightingWeight); LHEReweightingWeight.clear(); successBranches.push_back("Events/LHEReweightingWeight"); usedCounters.insert("nLHEReweightingWeight"); } else { missingBranches.push_back("Events/LHEReweightingWeight"); }
    }
    if ( choose["Events/LHEScaleWeight"] ) {
      if (input->present("Events/LHEScaleWeight")) { LHEScaleWeight.resize(49); input->select("Events/LHEScaleWeight", LHEScaleWeight); LHEScaleWeight.clear(); successBranches.push_back("Events/LHEScaleWeight"); usedCounters.insert("nLHEScaleWeight"); } else { missingBranches.push_back("Events/LHEScaleWeight"); }
    }
    if ( choose["Events/LHEWeight_originalXWGTUP"] ) {
      if (input->present("Events/LHEWeight_originalXWGTUP")) { input->select("Events/LHEWeight_originalXWGTUP", LHEWeight_originalXWGTUP); successBranches.push_back("Events/LHEWeight_originalXWGTUP"); } else { missingBranches.push_back("Events/LHEWeight_originalXWGTUP"); }
    }
    if ( choose["Events/LHE_AlphaS"] ) {
      if (input->present("Events/LHE_AlphaS")) { input->select("Events/LHE_AlphaS", LHE_AlphaS); successBranches.push_back("Events/LHE_AlphaS"); } else { missingBranches.push_back("Events/LHE_AlphaS"); }
    }
    if ( choose["Events/LHE_HT"] ) {
      if (input->present("Events/LHE_HT")) { input->select("Events/LHE_HT", LHE_HT); successBranches.push_back("Events/LHE_HT"); } else { missingBranches.push_back("Events/LHE_HT"); }
    }
    if ( choose["Events/LHE_HTIncoming"] ) {
      if (input->present("Events/LHE_HTIncoming")) { input->select("Events/LHE_HTIncoming", LHE_HTIncoming); successBranches.push_back("Events/LHE_HTIncoming"); } else { missingBranches.push_back("Events/LHE_HTIncoming"); }
    }
    if ( choose["Events/LHE_Nb"] ) {
      if (input->present("Events/LHE_Nb")) { input->select("Events/LHE_Nb", LHE_Nb); successBranches.push_back("Events/LHE_Nb"); } else { missingBranches.push_back("Events/LHE_Nb"); }
    }
    if ( choose["Events/LHE_Nc"] ) {
      if (input->present("Events/LHE_Nc")) { input->select("Events/LHE_Nc", LHE_Nc); successBranches.push_back("Events/LHE_Nc"); } else { missingBranches.push_back("Events/LHE_Nc"); }
    }
    if ( choose["Events/LHE_Nglu"] ) {
      if (input->present("Events/LHE_Nglu")) { input->select("Events/LHE_Nglu", LHE_Nglu); successBranches.push_back("Events/LHE_Nglu"); } else { missingBranches.push_back("Events/LHE_Nglu"); }
    }
    if ( choose["Events/LHE_Njets"] ) {
      if (input->present("Events/LHE_Njets")) { input->select("Events/LHE_Njets", LHE_Njets); successBranches.push_back("Events/LHE_Njets"); } else { missingBranches.push_back("Events/LHE_Njets"); }
    }
    if ( choose["Events/LHE_NpLO"] ) {
      if (input->present("Events/LHE_NpLO")) { input->select("Events/LHE_NpLO", LHE_NpLO); successBranches.push_back("Events/LHE_NpLO"); } else { missingBranches.push_back("Events/LHE_NpLO"); }
    }
    if ( choose["Events/LHE_NpNLO"] ) {
      if (input->present("Events/LHE_NpNLO")) { input->select("Events/LHE_NpNLO", LHE_NpNLO); successBranches.push_back("Events/LHE_NpNLO"); } else { missingBranches.push_back("Events/LHE_NpNLO"); }
    }
    if ( choose["Events/LHE_Nuds"] ) {
      if (input->present("Events/LHE_Nuds")) { input->select("Events/LHE_Nuds", LHE_Nuds); successBranches.push_back("Events/LHE_Nuds"); } else { missingBranches.push_back("Events/LHE_Nuds"); }
    }
    if ( choose["Events/LHE_Vpt"] ) {
      if (input->present("Events/LHE_Vpt")) { input->select("Events/LHE_Vpt", LHE_Vpt); successBranches.push_back("Events/LHE_Vpt"); } else { missingBranches.push_back("Events/LHE_Vpt"); }
    }
    if ( choose["Events/LowPtElectron_ID"] ) {
      if (input->present("Events/LowPtElectron_ID")) { LowPtElectron_ID.resize(46); input->select("Events/LowPtElectron_ID", LowPtElectron_ID); LowPtElectron_ID.clear(); successBranches.push_back("Events/LowPtElectron_ID"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_ID"); }
    }
    if ( choose["Events/LowPtElectron_charge"] ) {
      if (input->present("Events/LowPtElectron_charge")) { LowPtElectron_charge.resize(46); input->select("Events/LowPtElectron_charge", LowPtElectron_charge); LowPtElectron_charge.clear(); successBranches.push_back("Events/LowPtElectron_charge"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_charge"); }
    }
    if ( choose["Events/LowPtElectron_convVeto"] ) {
      if (input->present("Events/LowPtElectron_convVeto")) { LowPtElectron_convVeto.resize(46); input->select("Events/LowPtElectron_convVeto", LowPtElectron_convVeto); LowPtElectron_convVeto.clear(); successBranches.push_back("Events/LowPtElectron_convVeto"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_convVeto"); }
    }
    if ( choose["Events/LowPtElectron_convVtxRadius"] ) {
      if (input->present("Events/LowPtElectron_convVtxRadius")) { LowPtElectron_convVtxRadius.resize(46); input->select("Events/LowPtElectron_convVtxRadius", LowPtElectron_convVtxRadius); LowPtElectron_convVtxRadius.clear(); successBranches.push_back("Events/LowPtElectron_convVtxRadius"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_convVtxRadius"); }
    }
    if ( choose["Events/LowPtElectron_convWP"] ) {
      if (input->present("Events/LowPtElectron_convWP")) { LowPtElectron_convWP.resize(46); input->select("Events/LowPtElectron_convWP", LowPtElectron_convWP); LowPtElectron_convWP.clear(); successBranches.push_back("Events/LowPtElectron_convWP"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_convWP"); }
    }
    if ( choose["Events/LowPtElectron_deltaEtaSC"] ) {
      if (input->present("Events/LowPtElectron_deltaEtaSC")) { LowPtElectron_deltaEtaSC.resize(46); input->select("Events/LowPtElectron_deltaEtaSC", LowPtElectron_deltaEtaSC); LowPtElectron_deltaEtaSC.clear(); successBranches.push_back("Events/LowPtElectron_deltaEtaSC"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_deltaEtaSC"); }
    }
    if ( choose["Events/LowPtElectron_dxy"] ) {
      if (input->present("Events/LowPtElectron_dxy")) { LowPtElectron_dxy.resize(46); input->select("Events/LowPtElectron_dxy", LowPtElectron_dxy); LowPtElectron_dxy.clear(); successBranches.push_back("Events/LowPtElectron_dxy"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_dxy"); }
    }
    if ( choose["Events/LowPtElectron_dxyErr"] ) {
      if (input->present("Events/LowPtElectron_dxyErr")) { LowPtElectron_dxyErr.resize(46); input->select("Events/LowPtElectron_dxyErr", LowPtElectron_dxyErr); LowPtElectron_dxyErr.clear(); successBranches.push_back("Events/LowPtElectron_dxyErr"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_dxyErr"); }
    }
    if ( choose["Events/LowPtElectron_dz"] ) {
      if (input->present("Events/LowPtElectron_dz")) { LowPtElectron_dz.resize(46); input->select("Events/LowPtElectron_dz", LowPtElectron_dz); LowPtElectron_dz.clear(); successBranches.push_back("Events/LowPtElectron_dz"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_dz"); }
    }
    if ( choose["Events/LowPtElectron_dzErr"] ) {
      if (input->present("Events/LowPtElectron_dzErr")) { LowPtElectron_dzErr.resize(46); input->select("Events/LowPtElectron_dzErr", LowPtElectron_dzErr); LowPtElectron_dzErr.clear(); successBranches.push_back("Events/LowPtElectron_dzErr"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_dzErr"); }
    }
    if ( choose["Events/LowPtElectron_eInvMinusPInv"] ) {
      if (input->present("Events/LowPtElectron_eInvMinusPInv")) { LowPtElectron_eInvMinusPInv.resize(46); input->select("Events/LowPtElectron_eInvMinusPInv", LowPtElectron_eInvMinusPInv); LowPtElectron_eInvMinusPInv.clear(); successBranches.push_back("Events/LowPtElectron_eInvMinusPInv"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_eInvMinusPInv"); }
    }
    if ( choose["Events/LowPtElectron_embeddedID"] ) {
      if (input->present("Events/LowPtElectron_embeddedID")) { LowPtElectron_embeddedID.resize(46); input->select("Events/LowPtElectron_embeddedID", LowPtElectron_embeddedID); LowPtElectron_embeddedID.clear(); successBranches.push_back("Events/LowPtElectron_embeddedID"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_embeddedID"); }
    }
    if ( choose["Events/LowPtElectron_energyErr"] ) {
      if (input->present("Events/LowPtElectron_energyErr")) { LowPtElectron_energyErr.resize(46); input->select("Events/LowPtElectron_energyErr", LowPtElectron_energyErr); LowPtElectron_energyErr.clear(); successBranches.push_back("Events/LowPtElectron_energyErr"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_energyErr"); }
    }
    if ( choose["Events/LowPtElectron_eta"] ) {
      if (input->present("Events/LowPtElectron_eta")) { LowPtElectron_eta.resize(46); input->select("Events/LowPtElectron_eta", LowPtElectron_eta); LowPtElectron_eta.clear(); successBranches.push_back("Events/LowPtElectron_eta"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_eta"); }
    }
    if ( choose["Events/LowPtElectron_genPartFlav"] ) {
      if (input->present("Events/LowPtElectron_genPartFlav")) { LowPtElectron_genPartFlav.resize(46); input->select("Events/LowPtElectron_genPartFlav", LowPtElectron_genPartFlav); LowPtElectron_genPartFlav.clear(); successBranches.push_back("Events/LowPtElectron_genPartFlav"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_genPartFlav"); }
    }
    if ( choose["Events/LowPtElectron_genPartIdx"] ) {
      if (input->present("Events/LowPtElectron_genPartIdx")) { LowPtElectron_genPartIdx.resize(46); input->select("Events/LowPtElectron_genPartIdx", LowPtElectron_genPartIdx); LowPtElectron_genPartIdx.clear(); successBranches.push_back("Events/LowPtElectron_genPartIdx"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_genPartIdx"); }
    }
    if ( choose["Events/LowPtElectron_hoe"] ) {
      if (input->present("Events/LowPtElectron_hoe")) { LowPtElectron_hoe.resize(46); input->select("Events/LowPtElectron_hoe", LowPtElectron_hoe); LowPtElectron_hoe.clear(); successBranches.push_back("Events/LowPtElectron_hoe"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_hoe"); }
    }
    if ( choose["Events/LowPtElectron_lostHits"] ) {
      if (input->present("Events/LowPtElectron_lostHits")) { LowPtElectron_lostHits.resize(46); input->select("Events/LowPtElectron_lostHits", LowPtElectron_lostHits); LowPtElectron_lostHits.clear(); successBranches.push_back("Events/LowPtElectron_lostHits"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_lostHits"); }
    }
    if ( choose["Events/LowPtElectron_mass"] ) {
      if (input->present("Events/LowPtElectron_mass")) { LowPtElectron_mass.resize(46); input->select("Events/LowPtElectron_mass", LowPtElectron_mass); LowPtElectron_mass.clear(); successBranches.push_back("Events/LowPtElectron_mass"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_mass"); }
    }
    if ( choose["Events/LowPtElectron_miniPFRelIso_all"] ) {
      if (input->present("Events/LowPtElectron_miniPFRelIso_all")) { LowPtElectron_miniPFRelIso_all.resize(46); input->select("Events/LowPtElectron_miniPFRelIso_all", LowPtElectron_miniPFRelIso_all); LowPtElectron_miniPFRelIso_all.clear(); successBranches.push_back("Events/LowPtElectron_miniPFRelIso_all"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_miniPFRelIso_all"); }
    }
    if ( choose["Events/LowPtElectron_miniPFRelIso_chg"] ) {
      if (input->present("Events/LowPtElectron_miniPFRelIso_chg")) { LowPtElectron_miniPFRelIso_chg.resize(46); input->select("Events/LowPtElectron_miniPFRelIso_chg", LowPtElectron_miniPFRelIso_chg); LowPtElectron_miniPFRelIso_chg.clear(); successBranches.push_back("Events/LowPtElectron_miniPFRelIso_chg"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_miniPFRelIso_chg"); }
    }
    if ( choose["Events/LowPtElectron_pdgId"] ) {
      if (input->present("Events/LowPtElectron_pdgId")) { LowPtElectron_pdgId.resize(46); input->select("Events/LowPtElectron_pdgId", LowPtElectron_pdgId); LowPtElectron_pdgId.clear(); successBranches.push_back("Events/LowPtElectron_pdgId"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_pdgId"); }
    }
    if ( choose["Events/LowPtElectron_phi"] ) {
      if (input->present("Events/LowPtElectron_phi")) { LowPtElectron_phi.resize(46); input->select("Events/LowPtElectron_phi", LowPtElectron_phi); LowPtElectron_phi.clear(); successBranches.push_back("Events/LowPtElectron_phi"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_phi"); }
    }
    if ( choose["Events/LowPtElectron_pt"] ) {
      if (input->present("Events/LowPtElectron_pt")) { LowPtElectron_pt.resize(46); input->select("Events/LowPtElectron_pt", LowPtElectron_pt); LowPtElectron_pt.clear(); successBranches.push_back("Events/LowPtElectron_pt"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_pt"); }
    }
    if ( choose["Events/LowPtElectron_ptbiased"] ) {
      if (input->present("Events/LowPtElectron_ptbiased")) { LowPtElectron_ptbiased.resize(46); input->select("Events/LowPtElectron_ptbiased", LowPtElectron_ptbiased); LowPtElectron_ptbiased.clear(); successBranches.push_back("Events/LowPtElectron_ptbiased"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_ptbiased"); }
    }
    if ( choose["Events/LowPtElectron_r9"] ) {
      if (input->present("Events/LowPtElectron_r9")) { LowPtElectron_r9.resize(46); input->select("Events/LowPtElectron_r9", LowPtElectron_r9); LowPtElectron_r9.clear(); successBranches.push_back("Events/LowPtElectron_r9"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_r9"); }
    }
    if ( choose["Events/LowPtElectron_scEtOverPt"] ) {
      if (input->present("Events/LowPtElectron_scEtOverPt")) { LowPtElectron_scEtOverPt.resize(46); input->select("Events/LowPtElectron_scEtOverPt", LowPtElectron_scEtOverPt); LowPtElectron_scEtOverPt.clear(); successBranches.push_back("Events/LowPtElectron_scEtOverPt"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_scEtOverPt"); }
    }
    if ( choose["Events/LowPtElectron_sieie"] ) {
      if (input->present("Events/LowPtElectron_sieie")) { LowPtElectron_sieie.resize(46); input->select("Events/LowPtElectron_sieie", LowPtElectron_sieie); LowPtElectron_sieie.clear(); successBranches.push_back("Events/LowPtElectron_sieie"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_sieie"); }
    }
    if ( choose["Events/LowPtElectron_unbiased"] ) {
      if (input->present("Events/LowPtElectron_unbiased")) { LowPtElectron_unbiased.resize(46); input->select("Events/LowPtElectron_unbiased", LowPtElectron_unbiased); LowPtElectron_unbiased.clear(); successBranches.push_back("Events/LowPtElectron_unbiased"); usedCounters.insert("nLowPtElectron"); } else { missingBranches.push_back("Events/LowPtElectron_unbiased"); }
    }
    if ( choose["Events/MET_MetUnclustEnUpDeltaX"] ) {
      if (input->present("Events/MET_MetUnclustEnUpDeltaX")) { input->select("Events/MET_MetUnclustEnUpDeltaX", MET_MetUnclustEnUpDeltaX); successBranches.push_back("Events/MET_MetUnclustEnUpDeltaX"); } else { missingBranches.push_back("Events/MET_MetUnclustEnUpDeltaX"); }
    }
    if ( choose["Events/MET_MetUnclustEnUpDeltaY"] ) {
      if (input->present("Events/MET_MetUnclustEnUpDeltaY")) { input->select("Events/MET_MetUnclustEnUpDeltaY", MET_MetUnclustEnUpDeltaY); successBranches.push_back("Events/MET_MetUnclustEnUpDeltaY"); } else { missingBranches.push_back("Events/MET_MetUnclustEnUpDeltaY"); }
    }
    if ( choose["Events/MET_covXX"] ) {
      if (input->present("Events/MET_covXX")) { input->select("Events/MET_covXX", MET_covXX); successBranches.push_back("Events/MET_covXX"); } else { missingBranches.push_back("Events/MET_covXX"); }
    }
    if ( choose["Events/MET_covXY"] ) {
      if (input->present("Events/MET_covXY")) { input->select("Events/MET_covXY", MET_covXY); successBranches.push_back("Events/MET_covXY"); } else { missingBranches.push_back("Events/MET_covXY"); }
    }
    if ( choose["Events/MET_covYY"] ) {
      if (input->present("Events/MET_covYY")) { input->select("Events/MET_covYY", MET_covYY); successBranches.push_back("Events/MET_covYY"); } else { missingBranches.push_back("Events/MET_covYY"); }
    }
    if ( choose["Events/MET_fiducialGenPhi"] ) {
      if (input->present("Events/MET_fiducialGenPhi")) { input->select("Events/MET_fiducialGenPhi", MET_fiducialGenPhi); successBranches.push_back("Events/MET_fiducialGenPhi"); } else { missingBranches.push_back("Events/MET_fiducialGenPhi"); }
    }
    if ( choose["Events/MET_fiducialGenPt"] ) {
      if (input->present("Events/MET_fiducialGenPt")) { input->select("Events/MET_fiducialGenPt", MET_fiducialGenPt); successBranches.push_back("Events/MET_fiducialGenPt"); } else { missingBranches.push_back("Events/MET_fiducialGenPt"); }
    }
    if ( choose["Events/MET_phi"] ) {
      if (input->present("Events/MET_phi")) { input->select("Events/MET_phi", MET_phi); successBranches.push_back("Events/MET_phi"); } else { missingBranches.push_back("Events/MET_phi"); }
    }
    if ( choose["Events/MET_pt"] ) {
      if (input->present("Events/MET_pt")) { input->select("Events/MET_pt", MET_pt); successBranches.push_back("Events/MET_pt"); } else { missingBranches.push_back("Events/MET_pt"); }
    }
    if ( choose["Events/MET_significance"] ) {
      if (input->present("Events/MET_significance")) { input->select("Events/MET_significance", MET_significance); successBranches.push_back("Events/MET_significance"); } else { missingBranches.push_back("Events/MET_significance"); }
    }
    if ( choose["Events/MET_sumEt"] ) {
      if (input->present("Events/MET_sumEt")) { input->select("Events/MET_sumEt", MET_sumEt); successBranches.push_back("Events/MET_sumEt"); } else { missingBranches.push_back("Events/MET_sumEt"); }
    }
    if ( choose["Events/MET_sumPtUnclustered"] ) {
      if (input->present("Events/MET_sumPtUnclustered")) { input->select("Events/MET_sumPtUnclustered", MET_sumPtUnclustered); successBranches.push_back("Events/MET_sumPtUnclustered"); } else { missingBranches.push_back("Events/MET_sumPtUnclustered"); }
    }
    if ( choose["Events/Muon_charge"] ) {
      if (input->present("Events/Muon_charge")) { Muon_charge.resize(65); input->select("Events/Muon_charge", Muon_charge); Muon_charge.clear(); successBranches.push_back("Events/Muon_charge"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_charge"); }
    }
    if ( choose["Events/Muon_cleanmask"] ) {
      if (input->present("Events/Muon_cleanmask")) { Muon_cleanmask.resize(65); input->select("Events/Muon_cleanmask", Muon_cleanmask); Muon_cleanmask.clear(); successBranches.push_back("Events/Muon_cleanmask"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_cleanmask"); }
    }
    if ( choose["Events/Muon_dxy"] ) {
      if (input->present("Events/Muon_dxy")) { Muon_dxy.resize(65); input->select("Events/Muon_dxy", Muon_dxy); Muon_dxy.clear(); successBranches.push_back("Events/Muon_dxy"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_dxy"); }
    }
    if ( choose["Events/Muon_dxyErr"] ) {
      if (input->present("Events/Muon_dxyErr")) { Muon_dxyErr.resize(65); input->select("Events/Muon_dxyErr", Muon_dxyErr); Muon_dxyErr.clear(); successBranches.push_back("Events/Muon_dxyErr"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_dxyErr"); }
    }
    if ( choose["Events/Muon_dxybs"] ) {
      if (input->present("Events/Muon_dxybs")) { Muon_dxybs.resize(65); input->select("Events/Muon_dxybs", Muon_dxybs); Muon_dxybs.clear(); successBranches.push_back("Events/Muon_dxybs"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_dxybs"); }
    }
    if ( choose["Events/Muon_dz"] ) {
      if (input->present("Events/Muon_dz")) { Muon_dz.resize(65); input->select("Events/Muon_dz", Muon_dz); Muon_dz.clear(); successBranches.push_back("Events/Muon_dz"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_dz"); }
    }
    if ( choose["Events/Muon_dzErr"] ) {
      if (input->present("Events/Muon_dzErr")) { Muon_dzErr.resize(65); input->select("Events/Muon_dzErr", Muon_dzErr); Muon_dzErr.clear(); successBranches.push_back("Events/Muon_dzErr"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_dzErr"); }
    }
    if ( choose["Events/Muon_eta"] ) {
      if (input->present("Events/Muon_eta")) { Muon_eta.resize(65); input->select("Events/Muon_eta", Muon_eta); Muon_eta.clear(); successBranches.push_back("Events/Muon_eta"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_eta"); }
    }
    if ( choose["Events/Muon_fsrPhotonIdx"] ) {
      if (input->present("Events/Muon_fsrPhotonIdx")) { Muon_fsrPhotonIdx.resize(65); input->select("Events/Muon_fsrPhotonIdx", Muon_fsrPhotonIdx); Muon_fsrPhotonIdx.clear(); successBranches.push_back("Events/Muon_fsrPhotonIdx"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_fsrPhotonIdx"); }
    }
    if ( choose["Events/Muon_genPartFlav"] ) {
      if (input->present("Events/Muon_genPartFlav")) { Muon_genPartFlav.resize(65); input->select("Events/Muon_genPartFlav", Muon_genPartFlav); Muon_genPartFlav.clear(); successBranches.push_back("Events/Muon_genPartFlav"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_genPartFlav"); }
    }
    if ( choose["Events/Muon_genPartIdx"] ) {
      if (input->present("Events/Muon_genPartIdx")) { Muon_genPartIdx.resize(65); input->select("Events/Muon_genPartIdx", Muon_genPartIdx); Muon_genPartIdx.clear(); successBranches.push_back("Events/Muon_genPartIdx"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_genPartIdx"); }
    }
    if ( choose["Events/Muon_highPtId"] ) {
      if (input->present("Events/Muon_highPtId")) { Muon_highPtId.resize(65); input->select("Events/Muon_highPtId", Muon_highPtId); Muon_highPtId.clear(); successBranches.push_back("Events/Muon_highPtId"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_highPtId"); }
    }
    if ( choose["Events/Muon_highPurity"] ) {
      if (input->present("Events/Muon_highPurity")) { Muon_highPurity.resize(65); input->select("Events/Muon_highPurity", Muon_highPurity); Muon_highPurity.clear(); successBranches.push_back("Events/Muon_highPurity"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_highPurity"); }
    }
    if ( choose["Events/Muon_inTimeMuon"] ) {
      if (input->present("Events/Muon_inTimeMuon")) { Muon_inTimeMuon.resize(65); input->select("Events/Muon_inTimeMuon", Muon_inTimeMuon); Muon_inTimeMuon.clear(); successBranches.push_back("Events/Muon_inTimeMuon"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_inTimeMuon"); }
    }
    if ( choose["Events/Muon_ip3d"] ) {
      if (input->present("Events/Muon_ip3d")) { Muon_ip3d.resize(65); input->select("Events/Muon_ip3d", Muon_ip3d); Muon_ip3d.clear(); successBranches.push_back("Events/Muon_ip3d"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_ip3d"); }
    }
    if ( choose["Events/Muon_isGlobal"] ) {
      if (input->present("Events/Muon_isGlobal")) { Muon_isGlobal.resize(65); input->select("Events/Muon_isGlobal", Muon_isGlobal); Muon_isGlobal.clear(); successBranches.push_back("Events/Muon_isGlobal"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_isGlobal"); }
    }
    if ( choose["Events/Muon_isPFcand"] ) {
      if (input->present("Events/Muon_isPFcand")) { Muon_isPFcand.resize(65); input->select("Events/Muon_isPFcand", Muon_isPFcand); Muon_isPFcand.clear(); successBranches.push_back("Events/Muon_isPFcand"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_isPFcand"); }
    }
    if ( choose["Events/Muon_isStandalone"] ) {
      if (input->present("Events/Muon_isStandalone")) { Muon_isStandalone.resize(65); input->select("Events/Muon_isStandalone", Muon_isStandalone); Muon_isStandalone.clear(); successBranches.push_back("Events/Muon_isStandalone"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_isStandalone"); }
    }
    if ( choose["Events/Muon_isTracker"] ) {
      if (input->present("Events/Muon_isTracker")) { Muon_isTracker.resize(65); input->select("Events/Muon_isTracker", Muon_isTracker); Muon_isTracker.clear(); successBranches.push_back("Events/Muon_isTracker"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_isTracker"); }
    }
    if ( choose["Events/Muon_jetIdx"] ) {
      if (input->present("Events/Muon_jetIdx")) { Muon_jetIdx.resize(65); input->select("Events/Muon_jetIdx", Muon_jetIdx); Muon_jetIdx.clear(); successBranches.push_back("Events/Muon_jetIdx"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_jetIdx"); }
    }
    if ( choose["Events/Muon_jetNDauCharged"] ) {
      if (input->present("Events/Muon_jetNDauCharged")) { Muon_jetNDauCharged.resize(65); input->select("Events/Muon_jetNDauCharged", Muon_jetNDauCharged); Muon_jetNDauCharged.clear(); successBranches.push_back("Events/Muon_jetNDauCharged"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_jetNDauCharged"); }
    }
    if ( choose["Events/Muon_jetPtRelv2"] ) {
      if (input->present("Events/Muon_jetPtRelv2")) { Muon_jetPtRelv2.resize(65); input->select("Events/Muon_jetPtRelv2", Muon_jetPtRelv2); Muon_jetPtRelv2.clear(); successBranches.push_back("Events/Muon_jetPtRelv2"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_jetPtRelv2"); }
    }
    if ( choose["Events/Muon_jetRelIso"] ) {
      if (input->present("Events/Muon_jetRelIso")) { Muon_jetRelIso.resize(65); input->select("Events/Muon_jetRelIso", Muon_jetRelIso); Muon_jetRelIso.clear(); successBranches.push_back("Events/Muon_jetRelIso"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_jetRelIso"); }
    }
    if ( choose["Events/Muon_looseId"] ) {
      if (input->present("Events/Muon_looseId")) { Muon_looseId.resize(65); input->select("Events/Muon_looseId", Muon_looseId); Muon_looseId.clear(); successBranches.push_back("Events/Muon_looseId"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_looseId"); }
    }
    if ( choose["Events/Muon_mass"] ) {
      if (input->present("Events/Muon_mass")) { Muon_mass.resize(65); input->select("Events/Muon_mass", Muon_mass); Muon_mass.clear(); successBranches.push_back("Events/Muon_mass"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_mass"); }
    }
    if ( choose["Events/Muon_mediumId"] ) {
      if (input->present("Events/Muon_mediumId")) { Muon_mediumId.resize(65); input->select("Events/Muon_mediumId", Muon_mediumId); Muon_mediumId.clear(); successBranches.push_back("Events/Muon_mediumId"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_mediumId"); }
    }
    if ( choose["Events/Muon_mediumPromptId"] ) {
      if (input->present("Events/Muon_mediumPromptId")) { Muon_mediumPromptId.resize(65); input->select("Events/Muon_mediumPromptId", Muon_mediumPromptId); Muon_mediumPromptId.clear(); successBranches.push_back("Events/Muon_mediumPromptId"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_mediumPromptId"); }
    }
    if ( choose["Events/Muon_miniIsoId"] ) {
      if (input->present("Events/Muon_miniIsoId")) { Muon_miniIsoId.resize(65); input->select("Events/Muon_miniIsoId", Muon_miniIsoId); Muon_miniIsoId.clear(); successBranches.push_back("Events/Muon_miniIsoId"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_miniIsoId"); }
    }
    if ( choose["Events/Muon_miniPFRelIso_all"] ) {
      if (input->present("Events/Muon_miniPFRelIso_all")) { Muon_miniPFRelIso_all.resize(65); input->select("Events/Muon_miniPFRelIso_all", Muon_miniPFRelIso_all); Muon_miniPFRelIso_all.clear(); successBranches.push_back("Events/Muon_miniPFRelIso_all"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_miniPFRelIso_all"); }
    }
    if ( choose["Events/Muon_miniPFRelIso_chg"] ) {
      if (input->present("Events/Muon_miniPFRelIso_chg")) { Muon_miniPFRelIso_chg.resize(65); input->select("Events/Muon_miniPFRelIso_chg", Muon_miniPFRelIso_chg); Muon_miniPFRelIso_chg.clear(); successBranches.push_back("Events/Muon_miniPFRelIso_chg"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_miniPFRelIso_chg"); }
    }
    if ( choose["Events/Muon_multiIsoId"] ) {
      if (input->present("Events/Muon_multiIsoId")) { Muon_multiIsoId.resize(65); input->select("Events/Muon_multiIsoId", Muon_multiIsoId); Muon_multiIsoId.clear(); successBranches.push_back("Events/Muon_multiIsoId"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_multiIsoId"); }
    }
    if ( choose["Events/Muon_mvaId"] ) {
      if (input->present("Events/Muon_mvaId")) { Muon_mvaId.resize(65); input->select("Events/Muon_mvaId", Muon_mvaId); Muon_mvaId.clear(); successBranches.push_back("Events/Muon_mvaId"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_mvaId"); }
    }
    if ( choose["Events/Muon_mvaLowPt"] ) {
      if (input->present("Events/Muon_mvaLowPt")) { Muon_mvaLowPt.resize(65); input->select("Events/Muon_mvaLowPt", Muon_mvaLowPt); Muon_mvaLowPt.clear(); successBranches.push_back("Events/Muon_mvaLowPt"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_mvaLowPt"); }
    }
    if ( choose["Events/Muon_mvaLowPtId"] ) {
      if (input->present("Events/Muon_mvaLowPtId")) { Muon_mvaLowPtId.resize(65); input->select("Events/Muon_mvaLowPtId", Muon_mvaLowPtId); Muon_mvaLowPtId.clear(); successBranches.push_back("Events/Muon_mvaLowPtId"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_mvaLowPtId"); }
    }
    if ( choose["Events/Muon_mvaMuID"] ) {
      if (input->present("Events/Muon_mvaMuID")) { Muon_mvaMuID.resize(65); input->select("Events/Muon_mvaMuID", Muon_mvaMuID); Muon_mvaMuID.clear(); successBranches.push_back("Events/Muon_mvaMuID"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_mvaMuID"); }
    }
    if ( choose["Events/Muon_mvaMuID_WP"] ) {
      if (input->present("Events/Muon_mvaMuID_WP")) { Muon_mvaMuID_WP.resize(65); input->select("Events/Muon_mvaMuID_WP", Muon_mvaMuID_WP); Muon_mvaMuID_WP.clear(); successBranches.push_back("Events/Muon_mvaMuID_WP"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_mvaMuID_WP"); }
    }
    if ( choose["Events/Muon_mvaTTH"] ) {
      if (input->present("Events/Muon_mvaTTH")) { Muon_mvaTTH.resize(65); input->select("Events/Muon_mvaTTH", Muon_mvaTTH); Muon_mvaTTH.clear(); successBranches.push_back("Events/Muon_mvaTTH"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_mvaTTH"); }
    }
    if ( choose["Events/Muon_nStations"] ) {
      if (input->present("Events/Muon_nStations")) { Muon_nStations.resize(65); input->select("Events/Muon_nStations", Muon_nStations); Muon_nStations.clear(); successBranches.push_back("Events/Muon_nStations"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_nStations"); }
    }
    if ( choose["Events/Muon_nTrackerLayers"] ) {
      if (input->present("Events/Muon_nTrackerLayers")) { Muon_nTrackerLayers.resize(65); input->select("Events/Muon_nTrackerLayers", Muon_nTrackerLayers); Muon_nTrackerLayers.clear(); successBranches.push_back("Events/Muon_nTrackerLayers"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_nTrackerLayers"); }
    }
    if ( choose["Events/Muon_pdgId"] ) {
      if (input->present("Events/Muon_pdgId")) { Muon_pdgId.resize(65); input->select("Events/Muon_pdgId", Muon_pdgId); Muon_pdgId.clear(); successBranches.push_back("Events/Muon_pdgId"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_pdgId"); }
    }
    if ( choose["Events/Muon_pfIsoId"] ) {
      if (input->present("Events/Muon_pfIsoId")) { Muon_pfIsoId.resize(65); input->select("Events/Muon_pfIsoId", Muon_pfIsoId); Muon_pfIsoId.clear(); successBranches.push_back("Events/Muon_pfIsoId"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_pfIsoId"); }
    }
    if ( choose["Events/Muon_pfRelIso03_all"] ) {
      if (input->present("Events/Muon_pfRelIso03_all")) { Muon_pfRelIso03_all.resize(65); input->select("Events/Muon_pfRelIso03_all", Muon_pfRelIso03_all); Muon_pfRelIso03_all.clear(); successBranches.push_back("Events/Muon_pfRelIso03_all"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_pfRelIso03_all"); }
    }
    if ( choose["Events/Muon_pfRelIso03_chg"] ) {
      if (input->present("Events/Muon_pfRelIso03_chg")) { Muon_pfRelIso03_chg.resize(65); input->select("Events/Muon_pfRelIso03_chg", Muon_pfRelIso03_chg); Muon_pfRelIso03_chg.clear(); successBranches.push_back("Events/Muon_pfRelIso03_chg"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_pfRelIso03_chg"); }
    }
    if ( choose["Events/Muon_pfRelIso04_all"] ) {
      if (input->present("Events/Muon_pfRelIso04_all")) { Muon_pfRelIso04_all.resize(65); input->select("Events/Muon_pfRelIso04_all", Muon_pfRelIso04_all); Muon_pfRelIso04_all.clear(); successBranches.push_back("Events/Muon_pfRelIso04_all"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_pfRelIso04_all"); }
    }
    if ( choose["Events/Muon_phi"] ) {
      if (input->present("Events/Muon_phi")) { Muon_phi.resize(65); input->select("Events/Muon_phi", Muon_phi); Muon_phi.clear(); successBranches.push_back("Events/Muon_phi"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_phi"); }
    }
    if ( choose["Events/Muon_promptMVA"] ) {
      if (input->present("Events/Muon_promptMVA")) { Muon_promptMVA.resize(65); input->select("Events/Muon_promptMVA", Muon_promptMVA); Muon_promptMVA.clear(); successBranches.push_back("Events/Muon_promptMVA"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_promptMVA"); }
    }
    if ( choose["Events/Muon_pt"] ) {
      if (input->present("Events/Muon_pt")) { Muon_pt.resize(65); input->select("Events/Muon_pt", Muon_pt); Muon_pt.clear(); successBranches.push_back("Events/Muon_pt"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_pt"); }
    }
    if ( choose["Events/Muon_ptErr"] ) {
      if (input->present("Events/Muon_ptErr")) { Muon_ptErr.resize(65); input->select("Events/Muon_ptErr", Muon_ptErr); Muon_ptErr.clear(); successBranches.push_back("Events/Muon_ptErr"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_ptErr"); }
    }
    if ( choose["Events/Muon_puppiIsoId"] ) {
      if (input->present("Events/Muon_puppiIsoId")) { Muon_puppiIsoId.resize(65); input->select("Events/Muon_puppiIsoId", Muon_puppiIsoId); Muon_puppiIsoId.clear(); successBranches.push_back("Events/Muon_puppiIsoId"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_puppiIsoId"); }
    }
    if ( choose["Events/Muon_segmentComp"] ) {
      if (input->present("Events/Muon_segmentComp")) { Muon_segmentComp.resize(65); input->select("Events/Muon_segmentComp", Muon_segmentComp); Muon_segmentComp.clear(); successBranches.push_back("Events/Muon_segmentComp"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_segmentComp"); }
    }
    if ( choose["Events/Muon_sip3d"] ) {
      if (input->present("Events/Muon_sip3d")) { Muon_sip3d.resize(65); input->select("Events/Muon_sip3d", Muon_sip3d); Muon_sip3d.clear(); successBranches.push_back("Events/Muon_sip3d"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_sip3d"); }
    }
    if ( choose["Events/Muon_softId"] ) {
      if (input->present("Events/Muon_softId")) { Muon_softId.resize(65); input->select("Events/Muon_softId", Muon_softId); Muon_softId.clear(); successBranches.push_back("Events/Muon_softId"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_softId"); }
    }
    if ( choose["Events/Muon_softMva"] ) {
      if (input->present("Events/Muon_softMva")) { Muon_softMva.resize(65); input->select("Events/Muon_softMva", Muon_softMva); Muon_softMva.clear(); successBranches.push_back("Events/Muon_softMva"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_softMva"); }
    }
    if ( choose["Events/Muon_softMvaId"] ) {
      if (input->present("Events/Muon_softMvaId")) { Muon_softMvaId.resize(65); input->select("Events/Muon_softMvaId", Muon_softMvaId); Muon_softMvaId.clear(); successBranches.push_back("Events/Muon_softMvaId"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_softMvaId"); }
    }
    if ( choose["Events/Muon_tightCharge"] ) {
      if (input->present("Events/Muon_tightCharge")) { Muon_tightCharge.resize(65); input->select("Events/Muon_tightCharge", Muon_tightCharge); Muon_tightCharge.clear(); successBranches.push_back("Events/Muon_tightCharge"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_tightCharge"); }
    }
    if ( choose["Events/Muon_tightId"] ) {
      if (input->present("Events/Muon_tightId")) { Muon_tightId.resize(65); input->select("Events/Muon_tightId", Muon_tightId); Muon_tightId.clear(); successBranches.push_back("Events/Muon_tightId"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_tightId"); }
    }
    if ( choose["Events/Muon_tkIsoId"] ) {
      if (input->present("Events/Muon_tkIsoId")) { Muon_tkIsoId.resize(65); input->select("Events/Muon_tkIsoId", Muon_tkIsoId); Muon_tkIsoId.clear(); successBranches.push_back("Events/Muon_tkIsoId"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_tkIsoId"); }
    }
    if ( choose["Events/Muon_tkRelIso"] ) {
      if (input->present("Events/Muon_tkRelIso")) { Muon_tkRelIso.resize(65); input->select("Events/Muon_tkRelIso", Muon_tkRelIso); Muon_tkRelIso.clear(); successBranches.push_back("Events/Muon_tkRelIso"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_tkRelIso"); }
    }
    if ( choose["Events/Muon_triggerIdLoose"] ) {
      if (input->present("Events/Muon_triggerIdLoose")) { Muon_triggerIdLoose.resize(65); input->select("Events/Muon_triggerIdLoose", Muon_triggerIdLoose); Muon_triggerIdLoose.clear(); successBranches.push_back("Events/Muon_triggerIdLoose"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_triggerIdLoose"); }
    }
    if ( choose["Events/Muon_tunepRelPt"] ) {
      if (input->present("Events/Muon_tunepRelPt")) { Muon_tunepRelPt.resize(65); input->select("Events/Muon_tunepRelPt", Muon_tunepRelPt); Muon_tunepRelPt.clear(); successBranches.push_back("Events/Muon_tunepRelPt"); usedCounters.insert("nMuon"); } else { missingBranches.push_back("Events/Muon_tunepRelPt"); }
    }
    if ( choose["Events/OtherPV_z"] ) {
      if (input->present("Events/OtherPV_z")) { OtherPV_z.resize(31); input->select("Events/OtherPV_z", OtherPV_z); OtherPV_z.clear(); successBranches.push_back("Events/OtherPV_z"); usedCounters.insert("nOtherPV"); } else { missingBranches.push_back("Events/OtherPV_z"); }
    }
    if ( choose["Events/PFMET_covXX"] ) {
      if (input->present("Events/PFMET_covXX")) { input->select("Events/PFMET_covXX", PFMET_covXX); successBranches.push_back("Events/PFMET_covXX"); } else { missingBranches.push_back("Events/PFMET_covXX"); }
    }
    if ( choose["Events/PFMET_covXY"] ) {
      if (input->present("Events/PFMET_covXY")) { input->select("Events/PFMET_covXY", PFMET_covXY); successBranches.push_back("Events/PFMET_covXY"); } else { missingBranches.push_back("Events/PFMET_covXY"); }
    }
    if ( choose["Events/PFMET_covYY"] ) {
      if (input->present("Events/PFMET_covYY")) { input->select("Events/PFMET_covYY", PFMET_covYY); successBranches.push_back("Events/PFMET_covYY"); } else { missingBranches.push_back("Events/PFMET_covYY"); }
    }
    if ( choose["Events/PFMET_phi"] ) {
      if (input->present("Events/PFMET_phi")) { input->select("Events/PFMET_phi", PFMET_phi); successBranches.push_back("Events/PFMET_phi"); } else { missingBranches.push_back("Events/PFMET_phi"); }
    }
    if ( choose["Events/PFMET_phiUnclusteredDown"] ) {
      if (input->present("Events/PFMET_phiUnclusteredDown")) { input->select("Events/PFMET_phiUnclusteredDown", PFMET_phiUnclusteredDown); successBranches.push_back("Events/PFMET_phiUnclusteredDown"); } else { missingBranches.push_back("Events/PFMET_phiUnclusteredDown"); }
    }
    if ( choose["Events/PFMET_phiUnclusteredUp"] ) {
      if (input->present("Events/PFMET_phiUnclusteredUp")) { input->select("Events/PFMET_phiUnclusteredUp", PFMET_phiUnclusteredUp); successBranches.push_back("Events/PFMET_phiUnclusteredUp"); } else { missingBranches.push_back("Events/PFMET_phiUnclusteredUp"); }
    }
    if ( choose["Events/PFMET_pt"] ) {
      if (input->present("Events/PFMET_pt")) { input->select("Events/PFMET_pt", PFMET_pt); successBranches.push_back("Events/PFMET_pt"); } else { missingBranches.push_back("Events/PFMET_pt"); }
    }
    if ( choose["Events/PFMET_ptUnclusteredDown"] ) {
      if (input->present("Events/PFMET_ptUnclusteredDown")) { input->select("Events/PFMET_ptUnclusteredDown", PFMET_ptUnclusteredDown); successBranches.push_back("Events/PFMET_ptUnclusteredDown"); } else { missingBranches.push_back("Events/PFMET_ptUnclusteredDown"); }
    }
    if ( choose["Events/PFMET_ptUnclusteredUp"] ) {
      if (input->present("Events/PFMET_ptUnclusteredUp")) { input->select("Events/PFMET_ptUnclusteredUp", PFMET_ptUnclusteredUp); successBranches.push_back("Events/PFMET_ptUnclusteredUp"); } else { missingBranches.push_back("Events/PFMET_ptUnclusteredUp"); }
    }
    if ( choose["Events/PFMET_significance"] ) {
      if (input->present("Events/PFMET_significance")) { input->select("Events/PFMET_significance", PFMET_significance); successBranches.push_back("Events/PFMET_significance"); } else { missingBranches.push_back("Events/PFMET_significance"); }
    }
    if ( choose["Events/PFMET_sumEt"] ) {
      if (input->present("Events/PFMET_sumEt")) { input->select("Events/PFMET_sumEt", PFMET_sumEt); successBranches.push_back("Events/PFMET_sumEt"); } else { missingBranches.push_back("Events/PFMET_sumEt"); }
    }
    if ( choose["Events/PFMET_sumPtUnclustered"] ) {
      if (input->present("Events/PFMET_sumPtUnclustered")) { input->select("Events/PFMET_sumPtUnclustered", PFMET_sumPtUnclustered); successBranches.push_back("Events/PFMET_sumPtUnclustered"); } else { missingBranches.push_back("Events/PFMET_sumPtUnclustered"); }
    }
    if ( choose["Events/PPSLocalTrack_decRPId"] ) {
      if (input->present("Events/PPSLocalTrack_decRPId")) { PPSLocalTrack_decRPId.resize(65); input->select("Events/PPSLocalTrack_decRPId", PPSLocalTrack_decRPId); PPSLocalTrack_decRPId.clear(); successBranches.push_back("Events/PPSLocalTrack_decRPId"); usedCounters.insert("nPPSLocalTrack"); } else { missingBranches.push_back("Events/PPSLocalTrack_decRPId"); }
    }
    if ( choose["Events/PPSLocalTrack_multiRPProtonIdx"] ) {
      if (input->present("Events/PPSLocalTrack_multiRPProtonIdx")) { PPSLocalTrack_multiRPProtonIdx.resize(65); input->select("Events/PPSLocalTrack_multiRPProtonIdx", PPSLocalTrack_multiRPProtonIdx); PPSLocalTrack_multiRPProtonIdx.clear(); successBranches.push_back("Events/PPSLocalTrack_multiRPProtonIdx"); usedCounters.insert("nPPSLocalTrack"); } else { missingBranches.push_back("Events/PPSLocalTrack_multiRPProtonIdx"); }
    }
    if ( choose["Events/PPSLocalTrack_rpType"] ) {
      if (input->present("Events/PPSLocalTrack_rpType")) { PPSLocalTrack_rpType.resize(65); input->select("Events/PPSLocalTrack_rpType", PPSLocalTrack_rpType); PPSLocalTrack_rpType.clear(); successBranches.push_back("Events/PPSLocalTrack_rpType"); usedCounters.insert("nPPSLocalTrack"); } else { missingBranches.push_back("Events/PPSLocalTrack_rpType"); }
    }
    if ( choose["Events/PPSLocalTrack_singleRPProtonIdx"] ) {
      if (input->present("Events/PPSLocalTrack_singleRPProtonIdx")) { PPSLocalTrack_singleRPProtonIdx.resize(65); input->select("Events/PPSLocalTrack_singleRPProtonIdx", PPSLocalTrack_singleRPProtonIdx); PPSLocalTrack_singleRPProtonIdx.clear(); successBranches.push_back("Events/PPSLocalTrack_singleRPProtonIdx"); usedCounters.insert("nPPSLocalTrack"); } else { missingBranches.push_back("Events/PPSLocalTrack_singleRPProtonIdx"); }
    }
    if ( choose["Events/PPSLocalTrack_time"] ) {
      if (input->present("Events/PPSLocalTrack_time")) { PPSLocalTrack_time.resize(65); input->select("Events/PPSLocalTrack_time", PPSLocalTrack_time); PPSLocalTrack_time.clear(); successBranches.push_back("Events/PPSLocalTrack_time"); usedCounters.insert("nPPSLocalTrack"); } else { missingBranches.push_back("Events/PPSLocalTrack_time"); }
    }
    if ( choose["Events/PPSLocalTrack_timeUnc"] ) {
      if (input->present("Events/PPSLocalTrack_timeUnc")) { PPSLocalTrack_timeUnc.resize(65); input->select("Events/PPSLocalTrack_timeUnc", PPSLocalTrack_timeUnc); PPSLocalTrack_timeUnc.clear(); successBranches.push_back("Events/PPSLocalTrack_timeUnc"); usedCounters.insert("nPPSLocalTrack"); } else { missingBranches.push_back("Events/PPSLocalTrack_timeUnc"); }
    }
    if ( choose["Events/PPSLocalTrack_x"] ) {
      if (input->present("Events/PPSLocalTrack_x")) { PPSLocalTrack_x.resize(65); input->select("Events/PPSLocalTrack_x", PPSLocalTrack_x); PPSLocalTrack_x.clear(); successBranches.push_back("Events/PPSLocalTrack_x"); usedCounters.insert("nPPSLocalTrack"); } else { missingBranches.push_back("Events/PPSLocalTrack_x"); }
    }
    if ( choose["Events/PPSLocalTrack_y"] ) {
      if (input->present("Events/PPSLocalTrack_y")) { PPSLocalTrack_y.resize(65); input->select("Events/PPSLocalTrack_y", PPSLocalTrack_y); PPSLocalTrack_y.clear(); successBranches.push_back("Events/PPSLocalTrack_y"); usedCounters.insert("nPPSLocalTrack"); } else { missingBranches.push_back("Events/PPSLocalTrack_y"); }
    }
    if ( choose["Events/PSWeight"] ) {
      if (input->present("Events/PSWeight")) { PSWeight.resize(36); input->select("Events/PSWeight", PSWeight); PSWeight.clear(); successBranches.push_back("Events/PSWeight"); usedCounters.insert("nPSWeight"); } else { missingBranches.push_back("Events/PSWeight"); }
    }
    if ( choose["Events/PV_chi2"] ) {
      if (input->present("Events/PV_chi2")) { input->select("Events/PV_chi2", PV_chi2); successBranches.push_back("Events/PV_chi2"); } else { missingBranches.push_back("Events/PV_chi2"); }
    }
    if ( choose["Events/PV_ndof"] ) {
      if (input->present("Events/PV_ndof")) { input->select("Events/PV_ndof", PV_ndof); successBranches.push_back("Events/PV_ndof"); } else { missingBranches.push_back("Events/PV_ndof"); }
    }
    if ( choose["Events/PV_npvs"] ) {
      if (input->present("Events/PV_npvs")) { input->select("Events/PV_npvs", PV_npvs); successBranches.push_back("Events/PV_npvs"); } else { missingBranches.push_back("Events/PV_npvs"); }
    }
    if ( choose["Events/PV_npvsGood"] ) {
      if (input->present("Events/PV_npvsGood")) { input->select("Events/PV_npvsGood", PV_npvsGood); successBranches.push_back("Events/PV_npvsGood"); } else { missingBranches.push_back("Events/PV_npvsGood"); }
    }
    if ( choose["Events/PV_score"] ) {
      if (input->present("Events/PV_score")) { input->select("Events/PV_score", PV_score); successBranches.push_back("Events/PV_score"); } else { missingBranches.push_back("Events/PV_score"); }
    }
    if ( choose["Events/PV_x"] ) {
      if (input->present("Events/PV_x")) { input->select("Events/PV_x", PV_x); successBranches.push_back("Events/PV_x"); } else { missingBranches.push_back("Events/PV_x"); }
    }
    if ( choose["Events/PV_y"] ) {
      if (input->present("Events/PV_y")) { input->select("Events/PV_y", PV_y); successBranches.push_back("Events/PV_y"); } else { missingBranches.push_back("Events/PV_y"); }
    }
    if ( choose["Events/PV_z"] ) {
      if (input->present("Events/PV_z")) { input->select("Events/PV_z", PV_z); successBranches.push_back("Events/PV_z"); } else { missingBranches.push_back("Events/PV_z"); }
    }
    if ( choose["Events/Photon_charge"] ) {
      if (input->present("Events/Photon_charge")) { Photon_charge.resize(50); input->select("Events/Photon_charge", Photon_charge); Photon_charge.clear(); successBranches.push_back("Events/Photon_charge"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_charge"); }
    }
    if ( choose["Events/Photon_cleanmask"] ) {
      if (input->present("Events/Photon_cleanmask")) { Photon_cleanmask.resize(50); input->select("Events/Photon_cleanmask", Photon_cleanmask); Photon_cleanmask.clear(); successBranches.push_back("Events/Photon_cleanmask"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_cleanmask"); }
    }
    if ( choose["Events/Photon_cutBased"] ) {
      if (input->present("Events/Photon_cutBased")) { Photon_cutBased.resize(50); input->select("Events/Photon_cutBased", Photon_cutBased); Photon_cutBased.clear(); successBranches.push_back("Events/Photon_cutBased"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_cutBased"); }
    }
    if ( choose["Events/Photon_cutBased_Fall17V1Bitmap"] ) {
      if (input->present("Events/Photon_cutBased_Fall17V1Bitmap")) { Photon_cutBased_Fall17V1Bitmap.resize(50); input->select("Events/Photon_cutBased_Fall17V1Bitmap", Photon_cutBased_Fall17V1Bitmap); Photon_cutBased_Fall17V1Bitmap.clear(); successBranches.push_back("Events/Photon_cutBased_Fall17V1Bitmap"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_cutBased_Fall17V1Bitmap"); }
    }
    if ( choose["Events/Photon_dEscaleDown"] ) {
      if (input->present("Events/Photon_dEscaleDown")) { Photon_dEscaleDown.resize(50); input->select("Events/Photon_dEscaleDown", Photon_dEscaleDown); Photon_dEscaleDown.clear(); successBranches.push_back("Events/Photon_dEscaleDown"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_dEscaleDown"); }
    }
    if ( choose["Events/Photon_dEscaleUp"] ) {
      if (input->present("Events/Photon_dEscaleUp")) { Photon_dEscaleUp.resize(50); input->select("Events/Photon_dEscaleUp", Photon_dEscaleUp); Photon_dEscaleUp.clear(); successBranches.push_back("Events/Photon_dEscaleUp"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_dEscaleUp"); }
    }
    if ( choose["Events/Photon_dEsigmaDown"] ) {
      if (input->present("Events/Photon_dEsigmaDown")) { Photon_dEsigmaDown.resize(50); input->select("Events/Photon_dEsigmaDown", Photon_dEsigmaDown); Photon_dEsigmaDown.clear(); successBranches.push_back("Events/Photon_dEsigmaDown"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_dEsigmaDown"); }
    }
    if ( choose["Events/Photon_dEsigmaUp"] ) {
      if (input->present("Events/Photon_dEsigmaUp")) { Photon_dEsigmaUp.resize(50); input->select("Events/Photon_dEsigmaUp", Photon_dEsigmaUp); Photon_dEsigmaUp.clear(); successBranches.push_back("Events/Photon_dEsigmaUp"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_dEsigmaUp"); }
    }
    if ( choose["Events/Photon_eCorr"] ) {
      if (input->present("Events/Photon_eCorr")) { Photon_eCorr.resize(50); input->select("Events/Photon_eCorr", Photon_eCorr); Photon_eCorr.clear(); successBranches.push_back("Events/Photon_eCorr"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_eCorr"); }
    }
    if ( choose["Events/Photon_electronIdx"] ) {
      if (input->present("Events/Photon_electronIdx")) { Photon_electronIdx.resize(50); input->select("Events/Photon_electronIdx", Photon_electronIdx); Photon_electronIdx.clear(); successBranches.push_back("Events/Photon_electronIdx"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_electronIdx"); }
    }
    if ( choose["Events/Photon_electronVeto"] ) {
      if (input->present("Events/Photon_electronVeto")) { Photon_electronVeto.resize(50); input->select("Events/Photon_electronVeto", Photon_electronVeto); Photon_electronVeto.clear(); successBranches.push_back("Events/Photon_electronVeto"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_electronVeto"); }
    }
    if ( choose["Events/Photon_energyErr"] ) {
      if (input->present("Events/Photon_energyErr")) { Photon_energyErr.resize(50); input->select("Events/Photon_energyErr", Photon_energyErr); Photon_energyErr.clear(); successBranches.push_back("Events/Photon_energyErr"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_energyErr"); }
    }
    if ( choose["Events/Photon_eta"] ) {
      if (input->present("Events/Photon_eta")) { Photon_eta.resize(50); input->select("Events/Photon_eta", Photon_eta); Photon_eta.clear(); successBranches.push_back("Events/Photon_eta"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_eta"); }
    }
    if ( choose["Events/Photon_genPartFlav"] ) {
      if (input->present("Events/Photon_genPartFlav")) { Photon_genPartFlav.resize(50); input->select("Events/Photon_genPartFlav", Photon_genPartFlav); Photon_genPartFlav.clear(); successBranches.push_back("Events/Photon_genPartFlav"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_genPartFlav"); }
    }
    if ( choose["Events/Photon_genPartIdx"] ) {
      if (input->present("Events/Photon_genPartIdx")) { Photon_genPartIdx.resize(50); input->select("Events/Photon_genPartIdx", Photon_genPartIdx); Photon_genPartIdx.clear(); successBranches.push_back("Events/Photon_genPartIdx"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_genPartIdx"); }
    }
    if ( choose["Events/Photon_hoe"] ) {
      if (input->present("Events/Photon_hoe")) { Photon_hoe.resize(50); input->select("Events/Photon_hoe", Photon_hoe); Photon_hoe.clear(); successBranches.push_back("Events/Photon_hoe"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_hoe"); }
    }
    if ( choose["Events/Photon_isScEtaEB"] ) {
      if (input->present("Events/Photon_isScEtaEB")) { Photon_isScEtaEB.resize(50); input->select("Events/Photon_isScEtaEB", Photon_isScEtaEB); Photon_isScEtaEB.clear(); successBranches.push_back("Events/Photon_isScEtaEB"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_isScEtaEB"); }
    }
    if ( choose["Events/Photon_isScEtaEE"] ) {
      if (input->present("Events/Photon_isScEtaEE")) { Photon_isScEtaEE.resize(50); input->select("Events/Photon_isScEtaEE", Photon_isScEtaEE); Photon_isScEtaEE.clear(); successBranches.push_back("Events/Photon_isScEtaEE"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_isScEtaEE"); }
    }
    if ( choose["Events/Photon_jetIdx"] ) {
      if (input->present("Events/Photon_jetIdx")) { Photon_jetIdx.resize(50); input->select("Events/Photon_jetIdx", Photon_jetIdx); Photon_jetIdx.clear(); successBranches.push_back("Events/Photon_jetIdx"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_jetIdx"); }
    }
    if ( choose["Events/Photon_mass"] ) {
      if (input->present("Events/Photon_mass")) { Photon_mass.resize(50); input->select("Events/Photon_mass", Photon_mass); Photon_mass.clear(); successBranches.push_back("Events/Photon_mass"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_mass"); }
    }
    if ( choose["Events/Photon_mvaID"] ) {
      if (input->present("Events/Photon_mvaID")) { Photon_mvaID.resize(50); input->select("Events/Photon_mvaID", Photon_mvaID); Photon_mvaID.clear(); successBranches.push_back("Events/Photon_mvaID"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_mvaID"); }
    }
    if ( choose["Events/Photon_mvaID_Fall17V1p1"] ) {
      if (input->present("Events/Photon_mvaID_Fall17V1p1")) { Photon_mvaID_Fall17V1p1.resize(50); input->select("Events/Photon_mvaID_Fall17V1p1", Photon_mvaID_Fall17V1p1); Photon_mvaID_Fall17V1p1.clear(); successBranches.push_back("Events/Photon_mvaID_Fall17V1p1"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_mvaID_Fall17V1p1"); }
    }
    if ( choose["Events/Photon_mvaID_WP80"] ) {
      if (input->present("Events/Photon_mvaID_WP80")) { Photon_mvaID_WP80.resize(50); input->select("Events/Photon_mvaID_WP80", Photon_mvaID_WP80); Photon_mvaID_WP80.clear(); successBranches.push_back("Events/Photon_mvaID_WP80"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_mvaID_WP80"); }
    }
    if ( choose["Events/Photon_mvaID_WP90"] ) {
      if (input->present("Events/Photon_mvaID_WP90")) { Photon_mvaID_WP90.resize(50); input->select("Events/Photon_mvaID_WP90", Photon_mvaID_WP90); Photon_mvaID_WP90.clear(); successBranches.push_back("Events/Photon_mvaID_WP90"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_mvaID_WP90"); }
    }
    if ( choose["Events/Photon_pdgId"] ) {
      if (input->present("Events/Photon_pdgId")) { Photon_pdgId.resize(50); input->select("Events/Photon_pdgId", Photon_pdgId); Photon_pdgId.clear(); successBranches.push_back("Events/Photon_pdgId"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_pdgId"); }
    }
    if ( choose["Events/Photon_pfRelIso03_all"] ) {
      if (input->present("Events/Photon_pfRelIso03_all")) { Photon_pfRelIso03_all.resize(50); input->select("Events/Photon_pfRelIso03_all", Photon_pfRelIso03_all); Photon_pfRelIso03_all.clear(); successBranches.push_back("Events/Photon_pfRelIso03_all"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_pfRelIso03_all"); }
    }
    if ( choose["Events/Photon_pfRelIso03_chg"] ) {
      if (input->present("Events/Photon_pfRelIso03_chg")) { Photon_pfRelIso03_chg.resize(50); input->select("Events/Photon_pfRelIso03_chg", Photon_pfRelIso03_chg); Photon_pfRelIso03_chg.clear(); successBranches.push_back("Events/Photon_pfRelIso03_chg"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_pfRelIso03_chg"); }
    }
    if ( choose["Events/Photon_phi"] ) {
      if (input->present("Events/Photon_phi")) { Photon_phi.resize(50); input->select("Events/Photon_phi", Photon_phi); Photon_phi.clear(); successBranches.push_back("Events/Photon_phi"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_phi"); }
    }
    if ( choose["Events/Photon_pixelSeed"] ) {
      if (input->present("Events/Photon_pixelSeed")) { Photon_pixelSeed.resize(50); input->select("Events/Photon_pixelSeed", Photon_pixelSeed); Photon_pixelSeed.clear(); successBranches.push_back("Events/Photon_pixelSeed"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_pixelSeed"); }
    }
    if ( choose["Events/Photon_pt"] ) {
      if (input->present("Events/Photon_pt")) { Photon_pt.resize(50); input->select("Events/Photon_pt", Photon_pt); Photon_pt.clear(); successBranches.push_back("Events/Photon_pt"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_pt"); }
    }
    if ( choose["Events/Photon_r9"] ) {
      if (input->present("Events/Photon_r9")) { Photon_r9.resize(50); input->select("Events/Photon_r9", Photon_r9); Photon_r9.clear(); successBranches.push_back("Events/Photon_r9"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_r9"); }
    }
    if ( choose["Events/Photon_seedGain"] ) {
      if (input->present("Events/Photon_seedGain")) { Photon_seedGain.resize(50); input->select("Events/Photon_seedGain", Photon_seedGain); Photon_seedGain.clear(); successBranches.push_back("Events/Photon_seedGain"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_seedGain"); }
    }
    if ( choose["Events/Photon_sieie"] ) {
      if (input->present("Events/Photon_sieie")) { Photon_sieie.resize(50); input->select("Events/Photon_sieie", Photon_sieie); Photon_sieie.clear(); successBranches.push_back("Events/Photon_sieie"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_sieie"); }
    }
    if ( choose["Events/Photon_vidNestedWPBitmap"] ) {
      if (input->present("Events/Photon_vidNestedWPBitmap")) { Photon_vidNestedWPBitmap.resize(50); input->select("Events/Photon_vidNestedWPBitmap", Photon_vidNestedWPBitmap); Photon_vidNestedWPBitmap.clear(); successBranches.push_back("Events/Photon_vidNestedWPBitmap"); usedCounters.insert("nPhoton"); } else { missingBranches.push_back("Events/Photon_vidNestedWPBitmap"); }
    }
    if ( choose["Events/Pileup_gpudensity"] ) {
      if (input->present("Events/Pileup_gpudensity")) { input->select("Events/Pileup_gpudensity", Pileup_gpudensity); successBranches.push_back("Events/Pileup_gpudensity"); } else { missingBranches.push_back("Events/Pileup_gpudensity"); }
    }
    if ( choose["Events/Pileup_nPU"] ) {
      if (input->present("Events/Pileup_nPU")) { input->select("Events/Pileup_nPU", Pileup_nPU); successBranches.push_back("Events/Pileup_nPU"); } else { missingBranches.push_back("Events/Pileup_nPU"); }
    }
    if ( choose["Events/Pileup_nTrueInt"] ) {
      if (input->present("Events/Pileup_nTrueInt")) { input->select("Events/Pileup_nTrueInt", Pileup_nTrueInt); successBranches.push_back("Events/Pileup_nTrueInt"); } else { missingBranches.push_back("Events/Pileup_nTrueInt"); }
    }
    if ( choose["Events/Pileup_pudensity"] ) {
      if (input->present("Events/Pileup_pudensity")) { input->select("Events/Pileup_pudensity", Pileup_pudensity); successBranches.push_back("Events/Pileup_pudensity"); } else { missingBranches.push_back("Events/Pileup_pudensity"); }
    }
    if ( choose["Events/Pileup_sumEOOT"] ) {
      if (input->present("Events/Pileup_sumEOOT")) { input->select("Events/Pileup_sumEOOT", Pileup_sumEOOT); successBranches.push_back("Events/Pileup_sumEOOT"); } else { missingBranches.push_back("Events/Pileup_sumEOOT"); }
    }
    if ( choose["Events/Pileup_sumLOOT"] ) {
      if (input->present("Events/Pileup_sumLOOT")) { input->select("Events/Pileup_sumLOOT", Pileup_sumLOOT); successBranches.push_back("Events/Pileup_sumLOOT"); } else { missingBranches.push_back("Events/Pileup_sumLOOT"); }
    }
    if ( choose["Events/Proton_multiRP_arm"] ) {
      if (input->present("Events/Proton_multiRP_arm")) { Proton_multiRP_arm.resize(28); input->select("Events/Proton_multiRP_arm", Proton_multiRP_arm); Proton_multiRP_arm.clear(); successBranches.push_back("Events/Proton_multiRP_arm"); usedCounters.insert("nProton_multiRP"); } else { missingBranches.push_back("Events/Proton_multiRP_arm"); }
    }
    if ( choose["Events/Proton_multiRP_t"] ) {
      if (input->present("Events/Proton_multiRP_t")) { Proton_multiRP_t.resize(28); input->select("Events/Proton_multiRP_t", Proton_multiRP_t); Proton_multiRP_t.clear(); successBranches.push_back("Events/Proton_multiRP_t"); usedCounters.insert("nProton_multiRP"); } else { missingBranches.push_back("Events/Proton_multiRP_t"); }
    }
    if ( choose["Events/Proton_multiRP_thetaX"] ) {
      if (input->present("Events/Proton_multiRP_thetaX")) { Proton_multiRP_thetaX.resize(28); input->select("Events/Proton_multiRP_thetaX", Proton_multiRP_thetaX); Proton_multiRP_thetaX.clear(); successBranches.push_back("Events/Proton_multiRP_thetaX"); usedCounters.insert("nProton_multiRP"); } else { missingBranches.push_back("Events/Proton_multiRP_thetaX"); }
    }
    if ( choose["Events/Proton_multiRP_thetaY"] ) {
      if (input->present("Events/Proton_multiRP_thetaY")) { Proton_multiRP_thetaY.resize(28); input->select("Events/Proton_multiRP_thetaY", Proton_multiRP_thetaY); Proton_multiRP_thetaY.clear(); successBranches.push_back("Events/Proton_multiRP_thetaY"); usedCounters.insert("nProton_multiRP"); } else { missingBranches.push_back("Events/Proton_multiRP_thetaY"); }
    }
    if ( choose["Events/Proton_multiRP_time"] ) {
      if (input->present("Events/Proton_multiRP_time")) { Proton_multiRP_time.resize(28); input->select("Events/Proton_multiRP_time", Proton_multiRP_time); Proton_multiRP_time.clear(); successBranches.push_back("Events/Proton_multiRP_time"); usedCounters.insert("nProton_multiRP"); } else { missingBranches.push_back("Events/Proton_multiRP_time"); }
    }
    if ( choose["Events/Proton_multiRP_timeUnc"] ) {
      if (input->present("Events/Proton_multiRP_timeUnc")) { Proton_multiRP_timeUnc.resize(28); input->select("Events/Proton_multiRP_timeUnc", Proton_multiRP_timeUnc); Proton_multiRP_timeUnc.clear(); successBranches.push_back("Events/Proton_multiRP_timeUnc"); usedCounters.insert("nProton_multiRP"); } else { missingBranches.push_back("Events/Proton_multiRP_timeUnc"); }
    }
    if ( choose["Events/Proton_multiRP_xi"] ) {
      if (input->present("Events/Proton_multiRP_xi")) { Proton_multiRP_xi.resize(28); input->select("Events/Proton_multiRP_xi", Proton_multiRP_xi); Proton_multiRP_xi.clear(); successBranches.push_back("Events/Proton_multiRP_xi"); usedCounters.insert("nProton_multiRP"); } else { missingBranches.push_back("Events/Proton_multiRP_xi"); }
    }
    if ( choose["Events/Proton_singleRP_decRPId"] ) {
      if (input->present("Events/Proton_singleRP_decRPId")) { Proton_singleRP_decRPId.resize(65); input->select("Events/Proton_singleRP_decRPId", Proton_singleRP_decRPId); Proton_singleRP_decRPId.clear(); successBranches.push_back("Events/Proton_singleRP_decRPId"); usedCounters.insert("nProton_singleRP"); } else { missingBranches.push_back("Events/Proton_singleRP_decRPId"); }
    }
    if ( choose["Events/Proton_singleRP_thetaY"] ) {
      if (input->present("Events/Proton_singleRP_thetaY")) { Proton_singleRP_thetaY.resize(65); input->select("Events/Proton_singleRP_thetaY", Proton_singleRP_thetaY); Proton_singleRP_thetaY.clear(); successBranches.push_back("Events/Proton_singleRP_thetaY"); usedCounters.insert("nProton_singleRP"); } else { missingBranches.push_back("Events/Proton_singleRP_thetaY"); }
    }
    if ( choose["Events/Proton_singleRP_xi"] ) {
      if (input->present("Events/Proton_singleRP_xi")) { Proton_singleRP_xi.resize(65); input->select("Events/Proton_singleRP_xi", Proton_singleRP_xi); Proton_singleRP_xi.clear(); successBranches.push_back("Events/Proton_singleRP_xi"); usedCounters.insert("nProton_singleRP"); } else { missingBranches.push_back("Events/Proton_singleRP_xi"); }
    }
    if ( choose["Events/PuppiMET_covXX"] ) {
      if (input->present("Events/PuppiMET_covXX")) { input->select("Events/PuppiMET_covXX", PuppiMET_covXX); successBranches.push_back("Events/PuppiMET_covXX"); } else { missingBranches.push_back("Events/PuppiMET_covXX"); }
    }
    if ( choose["Events/PuppiMET_covXY"] ) {
      if (input->present("Events/PuppiMET_covXY")) { input->select("Events/PuppiMET_covXY", PuppiMET_covXY); successBranches.push_back("Events/PuppiMET_covXY"); } else { missingBranches.push_back("Events/PuppiMET_covXY"); }
    }
    if ( choose["Events/PuppiMET_covYY"] ) {
      if (input->present("Events/PuppiMET_covYY")) { input->select("Events/PuppiMET_covYY", PuppiMET_covYY); successBranches.push_back("Events/PuppiMET_covYY"); } else { missingBranches.push_back("Events/PuppiMET_covYY"); }
    }
    if ( choose["Events/PuppiMET_phi"] ) {
      if (input->present("Events/PuppiMET_phi")) { input->select("Events/PuppiMET_phi", PuppiMET_phi); successBranches.push_back("Events/PuppiMET_phi"); } else { missingBranches.push_back("Events/PuppiMET_phi"); }
    }
    if ( choose["Events/PuppiMET_phiJERDown"] ) {
      if (input->present("Events/PuppiMET_phiJERDown")) { input->select("Events/PuppiMET_phiJERDown", PuppiMET_phiJERDown); successBranches.push_back("Events/PuppiMET_phiJERDown"); } else { missingBranches.push_back("Events/PuppiMET_phiJERDown"); }
    }
    if ( choose["Events/PuppiMET_phiJERUp"] ) {
      if (input->present("Events/PuppiMET_phiJERUp")) { input->select("Events/PuppiMET_phiJERUp", PuppiMET_phiJERUp); successBranches.push_back("Events/PuppiMET_phiJERUp"); } else { missingBranches.push_back("Events/PuppiMET_phiJERUp"); }
    }
    if ( choose["Events/PuppiMET_phiJESDown"] ) {
      if (input->present("Events/PuppiMET_phiJESDown")) { input->select("Events/PuppiMET_phiJESDown", PuppiMET_phiJESDown); successBranches.push_back("Events/PuppiMET_phiJESDown"); } else { missingBranches.push_back("Events/PuppiMET_phiJESDown"); }
    }
    if ( choose["Events/PuppiMET_phiJESUp"] ) {
      if (input->present("Events/PuppiMET_phiJESUp")) { input->select("Events/PuppiMET_phiJESUp", PuppiMET_phiJESUp); successBranches.push_back("Events/PuppiMET_phiJESUp"); } else { missingBranches.push_back("Events/PuppiMET_phiJESUp"); }
    }
    if ( choose["Events/PuppiMET_phiUnclusteredDown"] ) {
      if (input->present("Events/PuppiMET_phiUnclusteredDown")) { input->select("Events/PuppiMET_phiUnclusteredDown", PuppiMET_phiUnclusteredDown); successBranches.push_back("Events/PuppiMET_phiUnclusteredDown"); } else { missingBranches.push_back("Events/PuppiMET_phiUnclusteredDown"); }
    }
    if ( choose["Events/PuppiMET_phiUnclusteredUp"] ) {
      if (input->present("Events/PuppiMET_phiUnclusteredUp")) { input->select("Events/PuppiMET_phiUnclusteredUp", PuppiMET_phiUnclusteredUp); successBranches.push_back("Events/PuppiMET_phiUnclusteredUp"); } else { missingBranches.push_back("Events/PuppiMET_phiUnclusteredUp"); }
    }
    if ( choose["Events/PuppiMET_pt"] ) {
      if (input->present("Events/PuppiMET_pt")) { input->select("Events/PuppiMET_pt", PuppiMET_pt); successBranches.push_back("Events/PuppiMET_pt"); } else { missingBranches.push_back("Events/PuppiMET_pt"); }
    }
    if ( choose["Events/PuppiMET_ptJERDown"] ) {
      if (input->present("Events/PuppiMET_ptJERDown")) { input->select("Events/PuppiMET_ptJERDown", PuppiMET_ptJERDown); successBranches.push_back("Events/PuppiMET_ptJERDown"); } else { missingBranches.push_back("Events/PuppiMET_ptJERDown"); }
    }
    if ( choose["Events/PuppiMET_ptJERUp"] ) {
      if (input->present("Events/PuppiMET_ptJERUp")) { input->select("Events/PuppiMET_ptJERUp", PuppiMET_ptJERUp); successBranches.push_back("Events/PuppiMET_ptJERUp"); } else { missingBranches.push_back("Events/PuppiMET_ptJERUp"); }
    }
    if ( choose["Events/PuppiMET_ptJESDown"] ) {
      if (input->present("Events/PuppiMET_ptJESDown")) { input->select("Events/PuppiMET_ptJESDown", PuppiMET_ptJESDown); successBranches.push_back("Events/PuppiMET_ptJESDown"); } else { missingBranches.push_back("Events/PuppiMET_ptJESDown"); }
    }
    if ( choose["Events/PuppiMET_ptJESUp"] ) {
      if (input->present("Events/PuppiMET_ptJESUp")) { input->select("Events/PuppiMET_ptJESUp", PuppiMET_ptJESUp); successBranches.push_back("Events/PuppiMET_ptJESUp"); } else { missingBranches.push_back("Events/PuppiMET_ptJESUp"); }
    }
    if ( choose["Events/PuppiMET_ptUnclusteredDown"] ) {
      if (input->present("Events/PuppiMET_ptUnclusteredDown")) { input->select("Events/PuppiMET_ptUnclusteredDown", PuppiMET_ptUnclusteredDown); successBranches.push_back("Events/PuppiMET_ptUnclusteredDown"); } else { missingBranches.push_back("Events/PuppiMET_ptUnclusteredDown"); }
    }
    if ( choose["Events/PuppiMET_ptUnclusteredUp"] ) {
      if (input->present("Events/PuppiMET_ptUnclusteredUp")) { input->select("Events/PuppiMET_ptUnclusteredUp", PuppiMET_ptUnclusteredUp); successBranches.push_back("Events/PuppiMET_ptUnclusteredUp"); } else { missingBranches.push_back("Events/PuppiMET_ptUnclusteredUp"); }
    }
    if ( choose["Events/PuppiMET_significance"] ) {
      if (input->present("Events/PuppiMET_significance")) { input->select("Events/PuppiMET_significance", PuppiMET_significance); successBranches.push_back("Events/PuppiMET_significance"); } else { missingBranches.push_back("Events/PuppiMET_significance"); }
    }
    if ( choose["Events/PuppiMET_sumEt"] ) {
      if (input->present("Events/PuppiMET_sumEt")) { input->select("Events/PuppiMET_sumEt", PuppiMET_sumEt); successBranches.push_back("Events/PuppiMET_sumEt"); } else { missingBranches.push_back("Events/PuppiMET_sumEt"); }
    }
    if ( choose["Events/PuppiMET_sumPtUnclustered"] ) {
      if (input->present("Events/PuppiMET_sumPtUnclustered")) { input->select("Events/PuppiMET_sumPtUnclustered", PuppiMET_sumPtUnclustered); successBranches.push_back("Events/PuppiMET_sumPtUnclustered"); } else { missingBranches.push_back("Events/PuppiMET_sumPtUnclustered"); }
    }
    if ( choose["Events/RawMET_phi"] ) {
      if (input->present("Events/RawMET_phi")) { input->select("Events/RawMET_phi", RawMET_phi); successBranches.push_back("Events/RawMET_phi"); } else { missingBranches.push_back("Events/RawMET_phi"); }
    }
    if ( choose["Events/RawMET_pt"] ) {
      if (input->present("Events/RawMET_pt")) { input->select("Events/RawMET_pt", RawMET_pt); successBranches.push_back("Events/RawMET_pt"); } else { missingBranches.push_back("Events/RawMET_pt"); }
    }
    if ( choose["Events/RawMET_sumEt"] ) {
      if (input->present("Events/RawMET_sumEt")) { input->select("Events/RawMET_sumEt", RawMET_sumEt); successBranches.push_back("Events/RawMET_sumEt"); } else { missingBranches.push_back("Events/RawMET_sumEt"); }
    }
    if ( choose["Events/RawPFMET_phi"] ) {
      if (input->present("Events/RawPFMET_phi")) { input->select("Events/RawPFMET_phi", RawPFMET_phi); successBranches.push_back("Events/RawPFMET_phi"); } else { missingBranches.push_back("Events/RawPFMET_phi"); }
    }
    if ( choose["Events/RawPFMET_pt"] ) {
      if (input->present("Events/RawPFMET_pt")) { input->select("Events/RawPFMET_pt", RawPFMET_pt); successBranches.push_back("Events/RawPFMET_pt"); } else { missingBranches.push_back("Events/RawPFMET_pt"); }
    }
    if ( choose["Events/RawPFMET_sumEt"] ) {
      if (input->present("Events/RawPFMET_sumEt")) { input->select("Events/RawPFMET_sumEt", RawPFMET_sumEt); successBranches.push_back("Events/RawPFMET_sumEt"); } else { missingBranches.push_back("Events/RawPFMET_sumEt"); }
    }
    if ( choose["Events/RawPuppiMET_phi"] ) {
      if (input->present("Events/RawPuppiMET_phi")) { input->select("Events/RawPuppiMET_phi", RawPuppiMET_phi); successBranches.push_back("Events/RawPuppiMET_phi"); } else { missingBranches.push_back("Events/RawPuppiMET_phi"); }
    }
    if ( choose["Events/RawPuppiMET_pt"] ) {
      if (input->present("Events/RawPuppiMET_pt")) { input->select("Events/RawPuppiMET_pt", RawPuppiMET_pt); successBranches.push_back("Events/RawPuppiMET_pt"); } else { missingBranches.push_back("Events/RawPuppiMET_pt"); }
    }
    if ( choose["Events/RawPuppiMET_sumEt"] ) {
      if (input->present("Events/RawPuppiMET_sumEt")) { input->select("Events/RawPuppiMET_sumEt", RawPuppiMET_sumEt); successBranches.push_back("Events/RawPuppiMET_sumEt"); } else { missingBranches.push_back("Events/RawPuppiMET_sumEt"); }
    }
    if ( choose["Events/Rho_fixedGridRhoAll"] ) {
      if (input->present("Events/Rho_fixedGridRhoAll")) { input->select("Events/Rho_fixedGridRhoAll", Rho_fixedGridRhoAll); successBranches.push_back("Events/Rho_fixedGridRhoAll"); } else { missingBranches.push_back("Events/Rho_fixedGridRhoAll"); }
    }
    if ( choose["Events/Rho_fixedGridRhoFastjetAll"] ) {
      if (input->present("Events/Rho_fixedGridRhoFastjetAll")) { input->select("Events/Rho_fixedGridRhoFastjetAll", Rho_fixedGridRhoFastjetAll); successBranches.push_back("Events/Rho_fixedGridRhoFastjetAll"); } else { missingBranches.push_back("Events/Rho_fixedGridRhoFastjetAll"); }
    }
    if ( choose["Events/Rho_fixedGridRhoFastjetCentral"] ) {
      if (input->present("Events/Rho_fixedGridRhoFastjetCentral")) { input->select("Events/Rho_fixedGridRhoFastjetCentral", Rho_fixedGridRhoFastjetCentral); successBranches.push_back("Events/Rho_fixedGridRhoFastjetCentral"); } else { missingBranches.push_back("Events/Rho_fixedGridRhoFastjetCentral"); }
    }
    if ( choose["Events/Rho_fixedGridRhoFastjetCentralCalo"] ) {
      if (input->present("Events/Rho_fixedGridRhoFastjetCentralCalo")) { input->select("Events/Rho_fixedGridRhoFastjetCentralCalo", Rho_fixedGridRhoFastjetCentralCalo); successBranches.push_back("Events/Rho_fixedGridRhoFastjetCentralCalo"); } else { missingBranches.push_back("Events/Rho_fixedGridRhoFastjetCentralCalo"); }
    }
    if ( choose["Events/Rho_fixedGridRhoFastjetCentralChargedPileUp"] ) {
      if (input->present("Events/Rho_fixedGridRhoFastjetCentralChargedPileUp")) { input->select("Events/Rho_fixedGridRhoFastjetCentralChargedPileUp", Rho_fixedGridRhoFastjetCentralChargedPileUp); successBranches.push_back("Events/Rho_fixedGridRhoFastjetCentralChargedPileUp"); } else { missingBranches.push_back("Events/Rho_fixedGridRhoFastjetCentralChargedPileUp"); }
    }
    if ( choose["Events/Rho_fixedGridRhoFastjetCentralNeutral"] ) {
      if (input->present("Events/Rho_fixedGridRhoFastjetCentralNeutral")) { input->select("Events/Rho_fixedGridRhoFastjetCentralNeutral", Rho_fixedGridRhoFastjetCentralNeutral); successBranches.push_back("Events/Rho_fixedGridRhoFastjetCentralNeutral"); } else { missingBranches.push_back("Events/Rho_fixedGridRhoFastjetCentralNeutral"); }
    }
    if ( choose["Events/SV_charge"] ) {
      if (input->present("Events/SV_charge")) { SV_charge.resize(74); input->select("Events/SV_charge", SV_charge); SV_charge.clear(); successBranches.push_back("Events/SV_charge"); usedCounters.insert("nSV"); } else { missingBranches.push_back("Events/SV_charge"); }
    }
    if ( choose["Events/SV_chi2"] ) {
      if (input->present("Events/SV_chi2")) { SV_chi2.resize(74); input->select("Events/SV_chi2", SV_chi2); SV_chi2.clear(); successBranches.push_back("Events/SV_chi2"); usedCounters.insert("nSV"); } else { missingBranches.push_back("Events/SV_chi2"); }
    }
    if ( choose["Events/SV_dlen"] ) {
      if (input->present("Events/SV_dlen")) { SV_dlen.resize(74); input->select("Events/SV_dlen", SV_dlen); SV_dlen.clear(); successBranches.push_back("Events/SV_dlen"); usedCounters.insert("nSV"); } else { missingBranches.push_back("Events/SV_dlen"); }
    }
    if ( choose["Events/SV_dlenSig"] ) {
      if (input->present("Events/SV_dlenSig")) { SV_dlenSig.resize(74); input->select("Events/SV_dlenSig", SV_dlenSig); SV_dlenSig.clear(); successBranches.push_back("Events/SV_dlenSig"); usedCounters.insert("nSV"); } else { missingBranches.push_back("Events/SV_dlenSig"); }
    }
    if ( choose["Events/SV_dxy"] ) {
      if (input->present("Events/SV_dxy")) { SV_dxy.resize(74); input->select("Events/SV_dxy", SV_dxy); SV_dxy.clear(); successBranches.push_back("Events/SV_dxy"); usedCounters.insert("nSV"); } else { missingBranches.push_back("Events/SV_dxy"); }
    }
    if ( choose["Events/SV_dxySig"] ) {
      if (input->present("Events/SV_dxySig")) { SV_dxySig.resize(74); input->select("Events/SV_dxySig", SV_dxySig); SV_dxySig.clear(); successBranches.push_back("Events/SV_dxySig"); usedCounters.insert("nSV"); } else { missingBranches.push_back("Events/SV_dxySig"); }
    }
    if ( choose["Events/SV_eta"] ) {
      if (input->present("Events/SV_eta")) { SV_eta.resize(74); input->select("Events/SV_eta", SV_eta); SV_eta.clear(); successBranches.push_back("Events/SV_eta"); usedCounters.insert("nSV"); } else { missingBranches.push_back("Events/SV_eta"); }
    }
    if ( choose["Events/SV_mass"] ) {
      if (input->present("Events/SV_mass")) { SV_mass.resize(74); input->select("Events/SV_mass", SV_mass); SV_mass.clear(); successBranches.push_back("Events/SV_mass"); usedCounters.insert("nSV"); } else { missingBranches.push_back("Events/SV_mass"); }
    }
    if ( choose["Events/SV_ndof"] ) {
      if (input->present("Events/SV_ndof")) { SV_ndof.resize(74); input->select("Events/SV_ndof", SV_ndof); SV_ndof.clear(); successBranches.push_back("Events/SV_ndof"); usedCounters.insert("nSV"); } else { missingBranches.push_back("Events/SV_ndof"); }
    }
    if ( choose["Events/SV_ntracks"] ) {
      if (input->present("Events/SV_ntracks")) { SV_ntracks.resize(74); input->select("Events/SV_ntracks", SV_ntracks); SV_ntracks.clear(); successBranches.push_back("Events/SV_ntracks"); usedCounters.insert("nSV"); } else { missingBranches.push_back("Events/SV_ntracks"); }
    }
    if ( choose["Events/SV_pAngle"] ) {
      if (input->present("Events/SV_pAngle")) { SV_pAngle.resize(74); input->select("Events/SV_pAngle", SV_pAngle); SV_pAngle.clear(); successBranches.push_back("Events/SV_pAngle"); usedCounters.insert("nSV"); } else { missingBranches.push_back("Events/SV_pAngle"); }
    }
    if ( choose["Events/SV_phi"] ) {
      if (input->present("Events/SV_phi")) { SV_phi.resize(74); input->select("Events/SV_phi", SV_phi); SV_phi.clear(); successBranches.push_back("Events/SV_phi"); usedCounters.insert("nSV"); } else { missingBranches.push_back("Events/SV_phi"); }
    }
    if ( choose["Events/SV_pt"] ) {
      if (input->present("Events/SV_pt")) { SV_pt.resize(74); input->select("Events/SV_pt", SV_pt); SV_pt.clear(); successBranches.push_back("Events/SV_pt"); usedCounters.insert("nSV"); } else { missingBranches.push_back("Events/SV_pt"); }
    }
    if ( choose["Events/SV_x"] ) {
      if (input->present("Events/SV_x")) { SV_x.resize(74); input->select("Events/SV_x", SV_x); SV_x.clear(); successBranches.push_back("Events/SV_x"); usedCounters.insert("nSV"); } else { missingBranches.push_back("Events/SV_x"); }
    }
    if ( choose["Events/SV_y"] ) {
      if (input->present("Events/SV_y")) { SV_y.resize(74); input->select("Events/SV_y", SV_y); SV_y.clear(); successBranches.push_back("Events/SV_y"); usedCounters.insert("nSV"); } else { missingBranches.push_back("Events/SV_y"); }
    }
    if ( choose["Events/SV_z"] ) {
      if (input->present("Events/SV_z")) { SV_z.resize(74); input->select("Events/SV_z", SV_z); SV_z.clear(); successBranches.push_back("Events/SV_z"); usedCounters.insert("nSV"); } else { missingBranches.push_back("Events/SV_z"); }
    }
    if ( choose["Events/SoftActivityJetHT"] ) {
      if (input->present("Events/SoftActivityJetHT")) { input->select("Events/SoftActivityJetHT", SoftActivityJetHT); successBranches.push_back("Events/SoftActivityJetHT"); } else { missingBranches.push_back("Events/SoftActivityJetHT"); }
    }
    if ( choose["Events/SoftActivityJetHT10"] ) {
      if (input->present("Events/SoftActivityJetHT10")) { input->select("Events/SoftActivityJetHT10", SoftActivityJetHT10); successBranches.push_back("Events/SoftActivityJetHT10"); } else { missingBranches.push_back("Events/SoftActivityJetHT10"); }
    }
    if ( choose["Events/SoftActivityJetHT2"] ) {
      if (input->present("Events/SoftActivityJetHT2")) { input->select("Events/SoftActivityJetHT2", SoftActivityJetHT2); successBranches.push_back("Events/SoftActivityJetHT2"); } else { missingBranches.push_back("Events/SoftActivityJetHT2"); }
    }
    if ( choose["Events/SoftActivityJetHT5"] ) {
      if (input->present("Events/SoftActivityJetHT5")) { input->select("Events/SoftActivityJetHT5", SoftActivityJetHT5); successBranches.push_back("Events/SoftActivityJetHT5"); } else { missingBranches.push_back("Events/SoftActivityJetHT5"); }
    }
    if ( choose["Events/SoftActivityJetNjets10"] ) {
      if (input->present("Events/SoftActivityJetNjets10")) { input->select("Events/SoftActivityJetNjets10", SoftActivityJetNjets10); successBranches.push_back("Events/SoftActivityJetNjets10"); } else { missingBranches.push_back("Events/SoftActivityJetNjets10"); }
    }
    if ( choose["Events/SoftActivityJetNjets2"] ) {
      if (input->present("Events/SoftActivityJetNjets2")) { input->select("Events/SoftActivityJetNjets2", SoftActivityJetNjets2); successBranches.push_back("Events/SoftActivityJetNjets2"); } else { missingBranches.push_back("Events/SoftActivityJetNjets2"); }
    }
    if ( choose["Events/SoftActivityJetNjets5"] ) {
      if (input->present("Events/SoftActivityJetNjets5")) { input->select("Events/SoftActivityJetNjets5", SoftActivityJetNjets5); successBranches.push_back("Events/SoftActivityJetNjets5"); } else { missingBranches.push_back("Events/SoftActivityJetNjets5"); }
    }
    if ( choose["Events/SoftActivityJet_eta"] ) {
      if (input->present("Events/SoftActivityJet_eta")) { SoftActivityJet_eta.resize(40); input->select("Events/SoftActivityJet_eta", SoftActivityJet_eta); SoftActivityJet_eta.clear(); successBranches.push_back("Events/SoftActivityJet_eta"); usedCounters.insert("nSoftActivityJet"); } else { missingBranches.push_back("Events/SoftActivityJet_eta"); }
    }
    if ( choose["Events/SoftActivityJet_phi"] ) {
      if (input->present("Events/SoftActivityJet_phi")) { SoftActivityJet_phi.resize(40); input->select("Events/SoftActivityJet_phi", SoftActivityJet_phi); SoftActivityJet_phi.clear(); successBranches.push_back("Events/SoftActivityJet_phi"); usedCounters.insert("nSoftActivityJet"); } else { missingBranches.push_back("Events/SoftActivityJet_phi"); }
    }
    if ( choose["Events/SoftActivityJet_pt"] ) {
      if (input->present("Events/SoftActivityJet_pt")) { SoftActivityJet_pt.resize(40); input->select("Events/SoftActivityJet_pt", SoftActivityJet_pt); SoftActivityJet_pt.clear(); successBranches.push_back("Events/SoftActivityJet_pt"); usedCounters.insert("nSoftActivityJet"); } else { missingBranches.push_back("Events/SoftActivityJet_pt"); }
    }
    if ( choose["Events/SubGenJetAK8_eta"] ) {
      if (input->present("Events/SubGenJetAK8_eta")) { SubGenJetAK8_eta.resize(64); input->select("Events/SubGenJetAK8_eta", SubGenJetAK8_eta); SubGenJetAK8_eta.clear(); successBranches.push_back("Events/SubGenJetAK8_eta"); usedCounters.insert("nSubGenJetAK8"); } else { missingBranches.push_back("Events/SubGenJetAK8_eta"); }
    }
    if ( choose["Events/SubGenJetAK8_mass"] ) {
      if (input->present("Events/SubGenJetAK8_mass")) { SubGenJetAK8_mass.resize(64); input->select("Events/SubGenJetAK8_mass", SubGenJetAK8_mass); SubGenJetAK8_mass.clear(); successBranches.push_back("Events/SubGenJetAK8_mass"); usedCounters.insert("nSubGenJetAK8"); } else { missingBranches.push_back("Events/SubGenJetAK8_mass"); }
    }
    if ( choose["Events/SubGenJetAK8_phi"] ) {
      if (input->present("Events/SubGenJetAK8_phi")) { SubGenJetAK8_phi.resize(64); input->select("Events/SubGenJetAK8_phi", SubGenJetAK8_phi); SubGenJetAK8_phi.clear(); successBranches.push_back("Events/SubGenJetAK8_phi"); usedCounters.insert("nSubGenJetAK8"); } else { missingBranches.push_back("Events/SubGenJetAK8_phi"); }
    }
    if ( choose["Events/SubGenJetAK8_pt"] ) {
      if (input->present("Events/SubGenJetAK8_pt")) { SubGenJetAK8_pt.resize(64); input->select("Events/SubGenJetAK8_pt", SubGenJetAK8_pt); SubGenJetAK8_pt.clear(); successBranches.push_back("Events/SubGenJetAK8_pt"); usedCounters.insert("nSubGenJetAK8"); } else { missingBranches.push_back("Events/SubGenJetAK8_pt"); }
    }
    if ( choose["Events/SubJet_btagCSVV2"] ) {
      if (input->present("Events/SubJet_btagCSVV2")) { SubJet_btagCSVV2.resize(54); input->select("Events/SubJet_btagCSVV2", SubJet_btagCSVV2); SubJet_btagCSVV2.clear(); successBranches.push_back("Events/SubJet_btagCSVV2"); usedCounters.insert("nSubJet"); } else { missingBranches.push_back("Events/SubJet_btagCSVV2"); }
    }
    if ( choose["Events/SubJet_btagDeepB"] ) {
      if (input->present("Events/SubJet_btagDeepB")) { SubJet_btagDeepB.resize(54); input->select("Events/SubJet_btagDeepB", SubJet_btagDeepB); SubJet_btagDeepB.clear(); successBranches.push_back("Events/SubJet_btagDeepB"); usedCounters.insert("nSubJet"); } else { missingBranches.push_back("Events/SubJet_btagDeepB"); }
    }
    if ( choose["Events/SubJet_eta"] ) {
      if (input->present("Events/SubJet_eta")) { SubJet_eta.resize(54); input->select("Events/SubJet_eta", SubJet_eta); SubJet_eta.clear(); successBranches.push_back("Events/SubJet_eta"); usedCounters.insert("nSubJet"); } else { missingBranches.push_back("Events/SubJet_eta"); }
    }
    if ( choose["Events/SubJet_hadronFlavour"] ) {
      if (input->present("Events/SubJet_hadronFlavour")) { SubJet_hadronFlavour.resize(54); input->select("Events/SubJet_hadronFlavour", SubJet_hadronFlavour); SubJet_hadronFlavour.clear(); successBranches.push_back("Events/SubJet_hadronFlavour"); usedCounters.insert("nSubJet"); } else { missingBranches.push_back("Events/SubJet_hadronFlavour"); }
    }
    if ( choose["Events/SubJet_mass"] ) {
      if (input->present("Events/SubJet_mass")) { SubJet_mass.resize(54); input->select("Events/SubJet_mass", SubJet_mass); SubJet_mass.clear(); successBranches.push_back("Events/SubJet_mass"); usedCounters.insert("nSubJet"); } else { missingBranches.push_back("Events/SubJet_mass"); }
    }
    if ( choose["Events/SubJet_n2b1"] ) {
      if (input->present("Events/SubJet_n2b1")) { SubJet_n2b1.resize(54); input->select("Events/SubJet_n2b1", SubJet_n2b1); SubJet_n2b1.clear(); successBranches.push_back("Events/SubJet_n2b1"); usedCounters.insert("nSubJet"); } else { missingBranches.push_back("Events/SubJet_n2b1"); }
    }
    if ( choose["Events/SubJet_n3b1"] ) {
      if (input->present("Events/SubJet_n3b1")) { SubJet_n3b1.resize(54); input->select("Events/SubJet_n3b1", SubJet_n3b1); SubJet_n3b1.clear(); successBranches.push_back("Events/SubJet_n3b1"); usedCounters.insert("nSubJet"); } else { missingBranches.push_back("Events/SubJet_n3b1"); }
    }
    if ( choose["Events/SubJet_nBHadrons"] ) {
      if (input->present("Events/SubJet_nBHadrons")) { SubJet_nBHadrons.resize(54); input->select("Events/SubJet_nBHadrons", SubJet_nBHadrons); SubJet_nBHadrons.clear(); successBranches.push_back("Events/SubJet_nBHadrons"); usedCounters.insert("nSubJet"); } else { missingBranches.push_back("Events/SubJet_nBHadrons"); }
    }
    if ( choose["Events/SubJet_nCHadrons"] ) {
      if (input->present("Events/SubJet_nCHadrons")) { SubJet_nCHadrons.resize(54); input->select("Events/SubJet_nCHadrons", SubJet_nCHadrons); SubJet_nCHadrons.clear(); successBranches.push_back("Events/SubJet_nCHadrons"); usedCounters.insert("nSubJet"); } else { missingBranches.push_back("Events/SubJet_nCHadrons"); }
    }
    if ( choose["Events/SubJet_phi"] ) {
      if (input->present("Events/SubJet_phi")) { SubJet_phi.resize(54); input->select("Events/SubJet_phi", SubJet_phi); SubJet_phi.clear(); successBranches.push_back("Events/SubJet_phi"); usedCounters.insert("nSubJet"); } else { missingBranches.push_back("Events/SubJet_phi"); }
    }
    if ( choose["Events/SubJet_pt"] ) {
      if (input->present("Events/SubJet_pt")) { SubJet_pt.resize(54); input->select("Events/SubJet_pt", SubJet_pt); SubJet_pt.clear(); successBranches.push_back("Events/SubJet_pt"); usedCounters.insert("nSubJet"); } else { missingBranches.push_back("Events/SubJet_pt"); }
    }
    if ( choose["Events/SubJet_rawFactor"] ) {
      if (input->present("Events/SubJet_rawFactor")) { SubJet_rawFactor.resize(54); input->select("Events/SubJet_rawFactor", SubJet_rawFactor); SubJet_rawFactor.clear(); successBranches.push_back("Events/SubJet_rawFactor"); usedCounters.insert("nSubJet"); } else { missingBranches.push_back("Events/SubJet_rawFactor"); }
    }
    if ( choose["Events/SubJet_tau1"] ) {
      if (input->present("Events/SubJet_tau1")) { SubJet_tau1.resize(54); input->select("Events/SubJet_tau1", SubJet_tau1); SubJet_tau1.clear(); successBranches.push_back("Events/SubJet_tau1"); usedCounters.insert("nSubJet"); } else { missingBranches.push_back("Events/SubJet_tau1"); }
    }
    if ( choose["Events/SubJet_tau2"] ) {
      if (input->present("Events/SubJet_tau2")) { SubJet_tau2.resize(54); input->select("Events/SubJet_tau2", SubJet_tau2); SubJet_tau2.clear(); successBranches.push_back("Events/SubJet_tau2"); usedCounters.insert("nSubJet"); } else { missingBranches.push_back("Events/SubJet_tau2"); }
    }
    if ( choose["Events/SubJet_tau3"] ) {
      if (input->present("Events/SubJet_tau3")) { SubJet_tau3.resize(54); input->select("Events/SubJet_tau3", SubJet_tau3); SubJet_tau3.clear(); successBranches.push_back("Events/SubJet_tau3"); usedCounters.insert("nSubJet"); } else { missingBranches.push_back("Events/SubJet_tau3"); }
    }
    if ( choose["Events/SubJet_tau4"] ) {
      if (input->present("Events/SubJet_tau4")) { SubJet_tau4.resize(54); input->select("Events/SubJet_tau4", SubJet_tau4); SubJet_tau4.clear(); successBranches.push_back("Events/SubJet_tau4"); usedCounters.insert("nSubJet"); } else { missingBranches.push_back("Events/SubJet_tau4"); }
    }
    if ( choose["Events/Tau_charge"] ) {
      if (input->present("Events/Tau_charge")) { Tau_charge.resize(40); input->select("Events/Tau_charge", Tau_charge); Tau_charge.clear(); successBranches.push_back("Events/Tau_charge"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_charge"); }
    }
    if ( choose["Events/Tau_chargedIso"] ) {
      if (input->present("Events/Tau_chargedIso")) { Tau_chargedIso.resize(40); input->select("Events/Tau_chargedIso", Tau_chargedIso); Tau_chargedIso.clear(); successBranches.push_back("Events/Tau_chargedIso"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_chargedIso"); }
    }
    if ( choose["Events/Tau_cleanmask"] ) {
      if (input->present("Events/Tau_cleanmask")) { Tau_cleanmask.resize(40); input->select("Events/Tau_cleanmask", Tau_cleanmask); Tau_cleanmask.clear(); successBranches.push_back("Events/Tau_cleanmask"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_cleanmask"); }
    }
    if ( choose["Events/Tau_decayMode"] ) {
      if (input->present("Events/Tau_decayMode")) { Tau_decayMode.resize(40); input->select("Events/Tau_decayMode", Tau_decayMode); Tau_decayMode.clear(); successBranches.push_back("Events/Tau_decayMode"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_decayMode"); }
    }
    if ( choose["Events/Tau_dxy"] ) {
      if (input->present("Events/Tau_dxy")) { Tau_dxy.resize(40); input->select("Events/Tau_dxy", Tau_dxy); Tau_dxy.clear(); successBranches.push_back("Events/Tau_dxy"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_dxy"); }
    }
    if ( choose["Events/Tau_dz"] ) {
      if (input->present("Events/Tau_dz")) { Tau_dz.resize(40); input->select("Events/Tau_dz", Tau_dz); Tau_dz.clear(); successBranches.push_back("Events/Tau_dz"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_dz"); }
    }
    if ( choose["Events/Tau_eta"] ) {
      if (input->present("Events/Tau_eta")) { Tau_eta.resize(40); input->select("Events/Tau_eta", Tau_eta); Tau_eta.clear(); successBranches.push_back("Events/Tau_eta"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_eta"); }
    }
    if ( choose["Events/Tau_genPartFlav"] ) {
      if (input->present("Events/Tau_genPartFlav")) { Tau_genPartFlav.resize(40); input->select("Events/Tau_genPartFlav", Tau_genPartFlav); Tau_genPartFlav.clear(); successBranches.push_back("Events/Tau_genPartFlav"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_genPartFlav"); }
    }
    if ( choose["Events/Tau_genPartIdx"] ) {
      if (input->present("Events/Tau_genPartIdx")) { Tau_genPartIdx.resize(40); input->select("Events/Tau_genPartIdx", Tau_genPartIdx); Tau_genPartIdx.clear(); successBranches.push_back("Events/Tau_genPartIdx"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_genPartIdx"); }
    }
    if ( choose["Events/Tau_idAntiEleDeadECal"] ) {
      if (input->present("Events/Tau_idAntiEleDeadECal")) { Tau_idAntiEleDeadECal.resize(40); input->select("Events/Tau_idAntiEleDeadECal", Tau_idAntiEleDeadECal); Tau_idAntiEleDeadECal.clear(); successBranches.push_back("Events/Tau_idAntiEleDeadECal"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_idAntiEleDeadECal"); }
    }
    if ( choose["Events/Tau_idAntiMu"] ) {
      if (input->present("Events/Tau_idAntiMu")) { Tau_idAntiMu.resize(40); input->select("Events/Tau_idAntiMu", Tau_idAntiMu); Tau_idAntiMu.clear(); successBranches.push_back("Events/Tau_idAntiMu"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_idAntiMu"); }
    }
    if ( choose["Events/Tau_idDecayModeOldDMs"] ) {
      if (input->present("Events/Tau_idDecayModeOldDMs")) { Tau_idDecayModeOldDMs.resize(40); input->select("Events/Tau_idDecayModeOldDMs", Tau_idDecayModeOldDMs); Tau_idDecayModeOldDMs.clear(); successBranches.push_back("Events/Tau_idDecayModeOldDMs"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_idDecayModeOldDMs"); }
    }
    if ( choose["Events/Tau_idDeepTau2017v2p1VSe"] ) {
      if (input->present("Events/Tau_idDeepTau2017v2p1VSe")) { Tau_idDeepTau2017v2p1VSe.resize(40); input->select("Events/Tau_idDeepTau2017v2p1VSe", Tau_idDeepTau2017v2p1VSe); Tau_idDeepTau2017v2p1VSe.clear(); successBranches.push_back("Events/Tau_idDeepTau2017v2p1VSe"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_idDeepTau2017v2p1VSe"); }
    }
    if ( choose["Events/Tau_idDeepTau2017v2p1VSjet"] ) {
      if (input->present("Events/Tau_idDeepTau2017v2p1VSjet")) { Tau_idDeepTau2017v2p1VSjet.resize(40); input->select("Events/Tau_idDeepTau2017v2p1VSjet", Tau_idDeepTau2017v2p1VSjet); Tau_idDeepTau2017v2p1VSjet.clear(); successBranches.push_back("Events/Tau_idDeepTau2017v2p1VSjet"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_idDeepTau2017v2p1VSjet"); }
    }
    if ( choose["Events/Tau_idDeepTau2017v2p1VSmu"] ) {
      if (input->present("Events/Tau_idDeepTau2017v2p1VSmu")) { Tau_idDeepTau2017v2p1VSmu.resize(40); input->select("Events/Tau_idDeepTau2017v2p1VSmu", Tau_idDeepTau2017v2p1VSmu); Tau_idDeepTau2017v2p1VSmu.clear(); successBranches.push_back("Events/Tau_idDeepTau2017v2p1VSmu"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_idDeepTau2017v2p1VSmu"); }
    }
    if ( choose["Events/Tau_jetIdx"] ) {
      if (input->present("Events/Tau_jetIdx")) { Tau_jetIdx.resize(40); input->select("Events/Tau_jetIdx", Tau_jetIdx); Tau_jetIdx.clear(); successBranches.push_back("Events/Tau_jetIdx"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_jetIdx"); }
    }
    if ( choose["Events/Tau_leadTkDeltaEta"] ) {
      if (input->present("Events/Tau_leadTkDeltaEta")) { Tau_leadTkDeltaEta.resize(40); input->select("Events/Tau_leadTkDeltaEta", Tau_leadTkDeltaEta); Tau_leadTkDeltaEta.clear(); successBranches.push_back("Events/Tau_leadTkDeltaEta"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_leadTkDeltaEta"); }
    }
    if ( choose["Events/Tau_leadTkDeltaPhi"] ) {
      if (input->present("Events/Tau_leadTkDeltaPhi")) { Tau_leadTkDeltaPhi.resize(40); input->select("Events/Tau_leadTkDeltaPhi", Tau_leadTkDeltaPhi); Tau_leadTkDeltaPhi.clear(); successBranches.push_back("Events/Tau_leadTkDeltaPhi"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_leadTkDeltaPhi"); }
    }
    if ( choose["Events/Tau_leadTkPtOverTauPt"] ) {
      if (input->present("Events/Tau_leadTkPtOverTauPt")) { Tau_leadTkPtOverTauPt.resize(40); input->select("Events/Tau_leadTkPtOverTauPt", Tau_leadTkPtOverTauPt); Tau_leadTkPtOverTauPt.clear(); successBranches.push_back("Events/Tau_leadTkPtOverTauPt"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_leadTkPtOverTauPt"); }
    }
    if ( choose["Events/Tau_mass"] ) {
      if (input->present("Events/Tau_mass")) { Tau_mass.resize(40); input->select("Events/Tau_mass", Tau_mass); Tau_mass.clear(); successBranches.push_back("Events/Tau_mass"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_mass"); }
    }
    if ( choose["Events/Tau_neutralIso"] ) {
      if (input->present("Events/Tau_neutralIso")) { Tau_neutralIso.resize(40); input->select("Events/Tau_neutralIso", Tau_neutralIso); Tau_neutralIso.clear(); successBranches.push_back("Events/Tau_neutralIso"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_neutralIso"); }
    }
    if ( choose["Events/Tau_phi"] ) {
      if (input->present("Events/Tau_phi")) { Tau_phi.resize(40); input->select("Events/Tau_phi", Tau_phi); Tau_phi.clear(); successBranches.push_back("Events/Tau_phi"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_phi"); }
    }
    if ( choose["Events/Tau_photonsOutsideSignalCone"] ) {
      if (input->present("Events/Tau_photonsOutsideSignalCone")) { Tau_photonsOutsideSignalCone.resize(40); input->select("Events/Tau_photonsOutsideSignalCone", Tau_photonsOutsideSignalCone); Tau_photonsOutsideSignalCone.clear(); successBranches.push_back("Events/Tau_photonsOutsideSignalCone"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_photonsOutsideSignalCone"); }
    }
    if ( choose["Events/Tau_pt"] ) {
      if (input->present("Events/Tau_pt")) { Tau_pt.resize(40); input->select("Events/Tau_pt", Tau_pt); Tau_pt.clear(); successBranches.push_back("Events/Tau_pt"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_pt"); }
    }
    if ( choose["Events/Tau_puCorr"] ) {
      if (input->present("Events/Tau_puCorr")) { Tau_puCorr.resize(40); input->select("Events/Tau_puCorr", Tau_puCorr); Tau_puCorr.clear(); successBranches.push_back("Events/Tau_puCorr"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_puCorr"); }
    }
    if ( choose["Events/Tau_rawDeepTau2017v2p1VSe"] ) {
      if (input->present("Events/Tau_rawDeepTau2017v2p1VSe")) { Tau_rawDeepTau2017v2p1VSe.resize(40); input->select("Events/Tau_rawDeepTau2017v2p1VSe", Tau_rawDeepTau2017v2p1VSe); Tau_rawDeepTau2017v2p1VSe.clear(); successBranches.push_back("Events/Tau_rawDeepTau2017v2p1VSe"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_rawDeepTau2017v2p1VSe"); }
    }
    if ( choose["Events/Tau_rawDeepTau2017v2p1VSjet"] ) {
      if (input->present("Events/Tau_rawDeepTau2017v2p1VSjet")) { Tau_rawDeepTau2017v2p1VSjet.resize(40); input->select("Events/Tau_rawDeepTau2017v2p1VSjet", Tau_rawDeepTau2017v2p1VSjet); Tau_rawDeepTau2017v2p1VSjet.clear(); successBranches.push_back("Events/Tau_rawDeepTau2017v2p1VSjet"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_rawDeepTau2017v2p1VSjet"); }
    }
    if ( choose["Events/Tau_rawDeepTau2017v2p1VSmu"] ) {
      if (input->present("Events/Tau_rawDeepTau2017v2p1VSmu")) { Tau_rawDeepTau2017v2p1VSmu.resize(40); input->select("Events/Tau_rawDeepTau2017v2p1VSmu", Tau_rawDeepTau2017v2p1VSmu); Tau_rawDeepTau2017v2p1VSmu.clear(); successBranches.push_back("Events/Tau_rawDeepTau2017v2p1VSmu"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_rawDeepTau2017v2p1VSmu"); }
    }
    if ( choose["Events/Tau_rawIso"] ) {
      if (input->present("Events/Tau_rawIso")) { Tau_rawIso.resize(40); input->select("Events/Tau_rawIso", Tau_rawIso); Tau_rawIso.clear(); successBranches.push_back("Events/Tau_rawIso"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_rawIso"); }
    }
    if ( choose["Events/Tau_rawIsodR03"] ) {
      if (input->present("Events/Tau_rawIsodR03")) { Tau_rawIsodR03.resize(40); input->select("Events/Tau_rawIsodR03", Tau_rawIsodR03); Tau_rawIsodR03.clear(); successBranches.push_back("Events/Tau_rawIsodR03"); usedCounters.insert("nTau"); } else { missingBranches.push_back("Events/Tau_rawIsodR03"); }
    }
    if ( choose["Events/TkMET_phi"] ) {
      if (input->present("Events/TkMET_phi")) { input->select("Events/TkMET_phi", TkMET_phi); successBranches.push_back("Events/TkMET_phi"); } else { missingBranches.push_back("Events/TkMET_phi"); }
    }
    if ( choose["Events/TkMET_pt"] ) {
      if (input->present("Events/TkMET_pt")) { input->select("Events/TkMET_pt", TkMET_pt); successBranches.push_back("Events/TkMET_pt"); } else { missingBranches.push_back("Events/TkMET_pt"); }
    }
    if ( choose["Events/TkMET_sumEt"] ) {
      if (input->present("Events/TkMET_sumEt")) { input->select("Events/TkMET_sumEt", TkMET_sumEt); successBranches.push_back("Events/TkMET_sumEt"); } else { missingBranches.push_back("Events/TkMET_sumEt"); }
    }
    if ( choose["Events/TrigObj_eta"] ) {
      if (input->present("Events/TrigObj_eta")) { TrigObj_eta.resize(118); input->select("Events/TrigObj_eta", TrigObj_eta); TrigObj_eta.clear(); successBranches.push_back("Events/TrigObj_eta"); usedCounters.insert("nTrigObj"); } else { missingBranches.push_back("Events/TrigObj_eta"); }
    }
    if ( choose["Events/TrigObj_filterBits"] ) {
      if (input->present("Events/TrigObj_filterBits")) { TrigObj_filterBits.resize(118); input->select("Events/TrigObj_filterBits", TrigObj_filterBits); TrigObj_filterBits.clear(); successBranches.push_back("Events/TrigObj_filterBits"); usedCounters.insert("nTrigObj"); } else { missingBranches.push_back("Events/TrigObj_filterBits"); }
    }
    if ( choose["Events/TrigObj_id"] ) {
      if (input->present("Events/TrigObj_id")) { TrigObj_id.resize(118); input->select("Events/TrigObj_id", TrigObj_id); TrigObj_id.clear(); successBranches.push_back("Events/TrigObj_id"); usedCounters.insert("nTrigObj"); } else { missingBranches.push_back("Events/TrigObj_id"); }
    }
    if ( choose["Events/TrigObj_l1charge"] ) {
      if (input->present("Events/TrigObj_l1charge")) { TrigObj_l1charge.resize(118); input->select("Events/TrigObj_l1charge", TrigObj_l1charge); TrigObj_l1charge.clear(); successBranches.push_back("Events/TrigObj_l1charge"); usedCounters.insert("nTrigObj"); } else { missingBranches.push_back("Events/TrigObj_l1charge"); }
    }
    if ( choose["Events/TrigObj_l1iso"] ) {
      if (input->present("Events/TrigObj_l1iso")) { TrigObj_l1iso.resize(118); input->select("Events/TrigObj_l1iso", TrigObj_l1iso); TrigObj_l1iso.clear(); successBranches.push_back("Events/TrigObj_l1iso"); usedCounters.insert("nTrigObj"); } else { missingBranches.push_back("Events/TrigObj_l1iso"); }
    }
    if ( choose["Events/TrigObj_l1pt"] ) {
      if (input->present("Events/TrigObj_l1pt")) { TrigObj_l1pt.resize(118); input->select("Events/TrigObj_l1pt", TrigObj_l1pt); TrigObj_l1pt.clear(); successBranches.push_back("Events/TrigObj_l1pt"); usedCounters.insert("nTrigObj"); } else { missingBranches.push_back("Events/TrigObj_l1pt"); }
    }
    if ( choose["Events/TrigObj_l1pt_2"] ) {
      if (input->present("Events/TrigObj_l1pt_2")) { TrigObj_l1pt_2.resize(118); input->select("Events/TrigObj_l1pt_2", TrigObj_l1pt_2); TrigObj_l1pt_2.clear(); successBranches.push_back("Events/TrigObj_l1pt_2"); usedCounters.insert("nTrigObj"); } else { missingBranches.push_back("Events/TrigObj_l1pt_2"); }
    }
    if ( choose["Events/TrigObj_l2pt"] ) {
      if (input->present("Events/TrigObj_l2pt")) { TrigObj_l2pt.resize(118); input->select("Events/TrigObj_l2pt", TrigObj_l2pt); TrigObj_l2pt.clear(); successBranches.push_back("Events/TrigObj_l2pt"); usedCounters.insert("nTrigObj"); } else { missingBranches.push_back("Events/TrigObj_l2pt"); }
    }
    if ( choose["Events/TrigObj_phi"] ) {
      if (input->present("Events/TrigObj_phi")) { TrigObj_phi.resize(118); input->select("Events/TrigObj_phi", TrigObj_phi); TrigObj_phi.clear(); successBranches.push_back("Events/TrigObj_phi"); usedCounters.insert("nTrigObj"); } else { missingBranches.push_back("Events/TrigObj_phi"); }
    }
    if ( choose["Events/TrigObj_pt"] ) {
      if (input->present("Events/TrigObj_pt")) { TrigObj_pt.resize(118); input->select("Events/TrigObj_pt", TrigObj_pt); TrigObj_pt.clear(); successBranches.push_back("Events/TrigObj_pt"); usedCounters.insert("nTrigObj"); } else { missingBranches.push_back("Events/TrigObj_pt"); }
    }
    if ( choose["Events/boostedTau_charge"] ) {
      if (input->present("Events/boostedTau_charge")) { boostedTau_charge.resize(37); input->select("Events/boostedTau_charge", boostedTau_charge); boostedTau_charge.clear(); successBranches.push_back("Events/boostedTau_charge"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_charge"); }
    }
    if ( choose["Events/boostedTau_chargedIso"] ) {
      if (input->present("Events/boostedTau_chargedIso")) { boostedTau_chargedIso.resize(37); input->select("Events/boostedTau_chargedIso", boostedTau_chargedIso); boostedTau_chargedIso.clear(); successBranches.push_back("Events/boostedTau_chargedIso"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_chargedIso"); }
    }
    if ( choose["Events/boostedTau_decayMode"] ) {
      if (input->present("Events/boostedTau_decayMode")) { boostedTau_decayMode.resize(37); input->select("Events/boostedTau_decayMode", boostedTau_decayMode); boostedTau_decayMode.clear(); successBranches.push_back("Events/boostedTau_decayMode"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_decayMode"); }
    }
    if ( choose["Events/boostedTau_eta"] ) {
      if (input->present("Events/boostedTau_eta")) { boostedTau_eta.resize(37); input->select("Events/boostedTau_eta", boostedTau_eta); boostedTau_eta.clear(); successBranches.push_back("Events/boostedTau_eta"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_eta"); }
    }
    if ( choose["Events/boostedTau_genPartFlav"] ) {
      if (input->present("Events/boostedTau_genPartFlav")) { boostedTau_genPartFlav.resize(37); input->select("Events/boostedTau_genPartFlav", boostedTau_genPartFlav); boostedTau_genPartFlav.clear(); successBranches.push_back("Events/boostedTau_genPartFlav"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_genPartFlav"); }
    }
    if ( choose["Events/boostedTau_genPartIdx"] ) {
      if (input->present("Events/boostedTau_genPartIdx")) { boostedTau_genPartIdx.resize(37); input->select("Events/boostedTau_genPartIdx", boostedTau_genPartIdx); boostedTau_genPartIdx.clear(); successBranches.push_back("Events/boostedTau_genPartIdx"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_genPartIdx"); }
    }
    if ( choose["Events/boostedTau_idAntiEle2018"] ) {
      if (input->present("Events/boostedTau_idAntiEle2018")) { boostedTau_idAntiEle2018.resize(37); input->select("Events/boostedTau_idAntiEle2018", boostedTau_idAntiEle2018); boostedTau_idAntiEle2018.clear(); successBranches.push_back("Events/boostedTau_idAntiEle2018"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_idAntiEle2018"); }
    }
    if ( choose["Events/boostedTau_idAntiMu"] ) {
      if (input->present("Events/boostedTau_idAntiMu")) { boostedTau_idAntiMu.resize(37); input->select("Events/boostedTau_idAntiMu", boostedTau_idAntiMu); boostedTau_idAntiMu.clear(); successBranches.push_back("Events/boostedTau_idAntiMu"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_idAntiMu"); }
    }
    if ( choose["Events/boostedTau_idMVAnewDM2017v2"] ) {
      if (input->present("Events/boostedTau_idMVAnewDM2017v2")) { boostedTau_idMVAnewDM2017v2.resize(37); input->select("Events/boostedTau_idMVAnewDM2017v2", boostedTau_idMVAnewDM2017v2); boostedTau_idMVAnewDM2017v2.clear(); successBranches.push_back("Events/boostedTau_idMVAnewDM2017v2"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_idMVAnewDM2017v2"); }
    }
    if ( choose["Events/boostedTau_idMVAoldDM2017v2"] ) {
      if (input->present("Events/boostedTau_idMVAoldDM2017v2")) { boostedTau_idMVAoldDM2017v2.resize(37); input->select("Events/boostedTau_idMVAoldDM2017v2", boostedTau_idMVAoldDM2017v2); boostedTau_idMVAoldDM2017v2.clear(); successBranches.push_back("Events/boostedTau_idMVAoldDM2017v2"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_idMVAoldDM2017v2"); }
    }
    if ( choose["Events/boostedTau_idMVAoldDMdR032017v2"] ) {
      if (input->present("Events/boostedTau_idMVAoldDMdR032017v2")) { boostedTau_idMVAoldDMdR032017v2.resize(37); input->select("Events/boostedTau_idMVAoldDMdR032017v2", boostedTau_idMVAoldDMdR032017v2); boostedTau_idMVAoldDMdR032017v2.clear(); successBranches.push_back("Events/boostedTau_idMVAoldDMdR032017v2"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_idMVAoldDMdR032017v2"); }
    }
    if ( choose["Events/boostedTau_jetIdx"] ) {
      if (input->present("Events/boostedTau_jetIdx")) { boostedTau_jetIdx.resize(37); input->select("Events/boostedTau_jetIdx", boostedTau_jetIdx); boostedTau_jetIdx.clear(); successBranches.push_back("Events/boostedTau_jetIdx"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_jetIdx"); }
    }
    if ( choose["Events/boostedTau_leadTkDeltaEta"] ) {
      if (input->present("Events/boostedTau_leadTkDeltaEta")) { boostedTau_leadTkDeltaEta.resize(37); input->select("Events/boostedTau_leadTkDeltaEta", boostedTau_leadTkDeltaEta); boostedTau_leadTkDeltaEta.clear(); successBranches.push_back("Events/boostedTau_leadTkDeltaEta"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_leadTkDeltaEta"); }
    }
    if ( choose["Events/boostedTau_leadTkDeltaPhi"] ) {
      if (input->present("Events/boostedTau_leadTkDeltaPhi")) { boostedTau_leadTkDeltaPhi.resize(37); input->select("Events/boostedTau_leadTkDeltaPhi", boostedTau_leadTkDeltaPhi); boostedTau_leadTkDeltaPhi.clear(); successBranches.push_back("Events/boostedTau_leadTkDeltaPhi"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_leadTkDeltaPhi"); }
    }
    if ( choose["Events/boostedTau_leadTkPtOverTauPt"] ) {
      if (input->present("Events/boostedTau_leadTkPtOverTauPt")) { boostedTau_leadTkPtOverTauPt.resize(37); input->select("Events/boostedTau_leadTkPtOverTauPt", boostedTau_leadTkPtOverTauPt); boostedTau_leadTkPtOverTauPt.clear(); successBranches.push_back("Events/boostedTau_leadTkPtOverTauPt"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_leadTkPtOverTauPt"); }
    }
    if ( choose["Events/boostedTau_mass"] ) {
      if (input->present("Events/boostedTau_mass")) { boostedTau_mass.resize(37); input->select("Events/boostedTau_mass", boostedTau_mass); boostedTau_mass.clear(); successBranches.push_back("Events/boostedTau_mass"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_mass"); }
    }
    if ( choose["Events/boostedTau_neutralIso"] ) {
      if (input->present("Events/boostedTau_neutralIso")) { boostedTau_neutralIso.resize(37); input->select("Events/boostedTau_neutralIso", boostedTau_neutralIso); boostedTau_neutralIso.clear(); successBranches.push_back("Events/boostedTau_neutralIso"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_neutralIso"); }
    }
    if ( choose["Events/boostedTau_phi"] ) {
      if (input->present("Events/boostedTau_phi")) { boostedTau_phi.resize(37); input->select("Events/boostedTau_phi", boostedTau_phi); boostedTau_phi.clear(); successBranches.push_back("Events/boostedTau_phi"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_phi"); }
    }
    if ( choose["Events/boostedTau_photonsOutsideSignalCone"] ) {
      if (input->present("Events/boostedTau_photonsOutsideSignalCone")) { boostedTau_photonsOutsideSignalCone.resize(37); input->select("Events/boostedTau_photonsOutsideSignalCone", boostedTau_photonsOutsideSignalCone); boostedTau_photonsOutsideSignalCone.clear(); successBranches.push_back("Events/boostedTau_photonsOutsideSignalCone"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_photonsOutsideSignalCone"); }
    }
    if ( choose["Events/boostedTau_pt"] ) {
      if (input->present("Events/boostedTau_pt")) { boostedTau_pt.resize(37); input->select("Events/boostedTau_pt", boostedTau_pt); boostedTau_pt.clear(); successBranches.push_back("Events/boostedTau_pt"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_pt"); }
    }
    if ( choose["Events/boostedTau_puCorr"] ) {
      if (input->present("Events/boostedTau_puCorr")) { boostedTau_puCorr.resize(37); input->select("Events/boostedTau_puCorr", boostedTau_puCorr); boostedTau_puCorr.clear(); successBranches.push_back("Events/boostedTau_puCorr"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_puCorr"); }
    }
    if ( choose["Events/boostedTau_rawAntiEle2018"] ) {
      if (input->present("Events/boostedTau_rawAntiEle2018")) { boostedTau_rawAntiEle2018.resize(37); input->select("Events/boostedTau_rawAntiEle2018", boostedTau_rawAntiEle2018); boostedTau_rawAntiEle2018.clear(); successBranches.push_back("Events/boostedTau_rawAntiEle2018"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_rawAntiEle2018"); }
    }
    if ( choose["Events/boostedTau_rawAntiEleCat2018"] ) {
      if (input->present("Events/boostedTau_rawAntiEleCat2018")) { boostedTau_rawAntiEleCat2018.resize(37); input->select("Events/boostedTau_rawAntiEleCat2018", boostedTau_rawAntiEleCat2018); boostedTau_rawAntiEleCat2018.clear(); successBranches.push_back("Events/boostedTau_rawAntiEleCat2018"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_rawAntiEleCat2018"); }
    }
    if ( choose["Events/boostedTau_rawIso"] ) {
      if (input->present("Events/boostedTau_rawIso")) { boostedTau_rawIso.resize(37); input->select("Events/boostedTau_rawIso", boostedTau_rawIso); boostedTau_rawIso.clear(); successBranches.push_back("Events/boostedTau_rawIso"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_rawIso"); }
    }
    if ( choose["Events/boostedTau_rawIsodR03"] ) {
      if (input->present("Events/boostedTau_rawIsodR03")) { boostedTau_rawIsodR03.resize(37); input->select("Events/boostedTau_rawIsodR03", boostedTau_rawIsodR03); boostedTau_rawIsodR03.clear(); successBranches.push_back("Events/boostedTau_rawIsodR03"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_rawIsodR03"); }
    }
    if ( choose["Events/boostedTau_rawMVAnewDM2017v2"] ) {
      if (input->present("Events/boostedTau_rawMVAnewDM2017v2")) { boostedTau_rawMVAnewDM2017v2.resize(37); input->select("Events/boostedTau_rawMVAnewDM2017v2", boostedTau_rawMVAnewDM2017v2); boostedTau_rawMVAnewDM2017v2.clear(); successBranches.push_back("Events/boostedTau_rawMVAnewDM2017v2"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_rawMVAnewDM2017v2"); }
    }
    if ( choose["Events/boostedTau_rawMVAoldDM2017v2"] ) {
      if (input->present("Events/boostedTau_rawMVAoldDM2017v2")) { boostedTau_rawMVAoldDM2017v2.resize(37); input->select("Events/boostedTau_rawMVAoldDM2017v2", boostedTau_rawMVAoldDM2017v2); boostedTau_rawMVAoldDM2017v2.clear(); successBranches.push_back("Events/boostedTau_rawMVAoldDM2017v2"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_rawMVAoldDM2017v2"); }
    }
    if ( choose["Events/boostedTau_rawMVAoldDMdR032017v2"] ) {
      if (input->present("Events/boostedTau_rawMVAoldDMdR032017v2")) { boostedTau_rawMVAoldDMdR032017v2.resize(37); input->select("Events/boostedTau_rawMVAoldDMdR032017v2", boostedTau_rawMVAoldDMdR032017v2); boostedTau_rawMVAoldDMdR032017v2.clear(); successBranches.push_back("Events/boostedTau_rawMVAoldDMdR032017v2"); usedCounters.insert("nboostedTau"); } else { missingBranches.push_back("Events/boostedTau_rawMVAoldDMdR032017v2"); }
    }
    if ( choose["Events/btagWeight_CSVV2"] ) {
      if (input->present("Events/btagWeight_CSVV2")) { input->select("Events/btagWeight_CSVV2", btagWeight_CSVV2); successBranches.push_back("Events/btagWeight_CSVV2"); } else { missingBranches.push_back("Events/btagWeight_CSVV2"); }
    }
    if ( choose["Events/btagWeight_DeepCSVB"] ) {
      if (input->present("Events/btagWeight_DeepCSVB")) { input->select("Events/btagWeight_DeepCSVB", btagWeight_DeepCSVB); successBranches.push_back("Events/btagWeight_DeepCSVB"); } else { missingBranches.push_back("Events/btagWeight_DeepCSVB"); }
    }
    if ( choose["Events/event"] ) {
      if (input->present("Events/event")) { input->select("Events/event", event); successBranches.push_back("Events/event"); } else { missingBranches.push_back("Events/event"); }
    }
    if ( choose["Events/fixedGridRhoFastjetAll"] ) {
      if (input->present("Events/fixedGridRhoFastjetAll")) { input->select("Events/fixedGridRhoFastjetAll", fixedGridRhoFastjetAll); successBranches.push_back("Events/fixedGridRhoFastjetAll"); } else { missingBranches.push_back("Events/fixedGridRhoFastjetAll"); }
    }
    if ( choose["Events/fixedGridRhoFastjetCentral"] ) {
      if (input->present("Events/fixedGridRhoFastjetCentral")) { input->select("Events/fixedGridRhoFastjetCentral", fixedGridRhoFastjetCentral); successBranches.push_back("Events/fixedGridRhoFastjetCentral"); } else { missingBranches.push_back("Events/fixedGridRhoFastjetCentral"); }
    }
    if ( choose["Events/fixedGridRhoFastjetCentralCalo"] ) {
      if (input->present("Events/fixedGridRhoFastjetCentralCalo")) { input->select("Events/fixedGridRhoFastjetCentralCalo", fixedGridRhoFastjetCentralCalo); successBranches.push_back("Events/fixedGridRhoFastjetCentralCalo"); } else { missingBranches.push_back("Events/fixedGridRhoFastjetCentralCalo"); }
    }
    if ( choose["Events/fixedGridRhoFastjetCentralChargedPileUp"] ) {
      if (input->present("Events/fixedGridRhoFastjetCentralChargedPileUp")) { input->select("Events/fixedGridRhoFastjetCentralChargedPileUp", fixedGridRhoFastjetCentralChargedPileUp); successBranches.push_back("Events/fixedGridRhoFastjetCentralChargedPileUp"); } else { missingBranches.push_back("Events/fixedGridRhoFastjetCentralChargedPileUp"); }
    }
    if ( choose["Events/fixedGridRhoFastjetCentralNeutral"] ) {
      if (input->present("Events/fixedGridRhoFastjetCentralNeutral")) { input->select("Events/fixedGridRhoFastjetCentralNeutral", fixedGridRhoFastjetCentralNeutral); successBranches.push_back("Events/fixedGridRhoFastjetCentralNeutral"); } else { missingBranches.push_back("Events/fixedGridRhoFastjetCentralNeutral"); }
    }
    if ( choose["Events/genTtbarId"] ) {
      if (input->present("Events/genTtbarId")) { input->select("Events/genTtbarId", genTtbarId); successBranches.push_back("Events/genTtbarId"); } else { missingBranches.push_back("Events/genTtbarId"); }
    }
    if ( choose["Events/genWeight"] ) {
      if (input->present("Events/genWeight")) { input->select("Events/genWeight", genWeight); successBranches.push_back("Events/genWeight"); } else { missingBranches.push_back("Events/genWeight"); }
    }
    if ( choose["Events/luminosityBlock"] ) {
      if (input->present("Events/luminosityBlock")) { input->select("Events/luminosityBlock", luminosityBlock); successBranches.push_back("Events/luminosityBlock"); } else { missingBranches.push_back("Events/luminosityBlock"); }
    }
    if ( choose["Events/run"] ) {
      if (input->present("Events/run")) { input->select("Events/run", run); successBranches.push_back("Events/run"); } else { missingBranches.push_back("Events/run"); }
    }
    if ( usedCounters.count("nCorrT1METJet") && input->present("Events/nCorrT1METJet") ) input->select("Events/nCorrT1METJet", nCorrT1METJet);
    if ( usedCounters.count("nElectron") && input->present("Events/nElectron") ) input->select("Events/nElectron", nElectron);
    if ( usedCounters.count("nFatJet") && input->present("Events/nFatJet") ) input->select("Events/nFatJet", nFatJet);
    if ( usedCounters.count("nFsrPhoton") && input->present("Events/nFsrPhoton") ) input->select("Events/nFsrPhoton", nFsrPhoton);
    if ( usedCounters.count("nGenDressedLepton") && input->present("Events/nGenDressedLepton") ) input->select("Events/nGenDressedLepton", nGenDressedLepton);
    if ( usedCounters.count("nGenIsolatedPhoton") && input->present("Events/nGenIsolatedPhoton") ) input->select("Events/nGenIsolatedPhoton", nGenIsolatedPhoton);
    if ( usedCounters.count("nGenJet") && input->present("Events/nGenJet") ) input->select("Events/nGenJet", nGenJet);
    if ( usedCounters.count("nGenJetAK8") && input->present("Events/nGenJetAK8") ) input->select("Events/nGenJetAK8", nGenJetAK8);
    if ( usedCounters.count("nGenPart") && input->present("Events/nGenPart") ) input->select("Events/nGenPart", nGenPart);
    if ( usedCounters.count("nGenVisTau") && input->present("Events/nGenVisTau") ) input->select("Events/nGenVisTau", nGenVisTau);
    if ( usedCounters.count("nIsoTrack") && input->present("Events/nIsoTrack") ) input->select("Events/nIsoTrack", nIsoTrack);
    if ( usedCounters.count("nJet") && input->present("Events/nJet") ) input->select("Events/nJet", nJet);
    if ( usedCounters.count("nLHEPart") && input->present("Events/nLHEPart") ) input->select("Events/nLHEPart", nLHEPart);
    if ( usedCounters.count("nLHEPdfWeight") && input->present("Events/nLHEPdfWeight") ) input->select("Events/nLHEPdfWeight", nLHEPdfWeight);
    if ( usedCounters.count("nLHEReweightingWeight") && input->present("Events/nLHEReweightingWeight") ) input->select("Events/nLHEReweightingWeight", nLHEReweightingWeight);
    if ( usedCounters.count("nLHEScaleWeight") && input->present("Events/nLHEScaleWeight") ) input->select("Events/nLHEScaleWeight", nLHEScaleWeight);
    if ( usedCounters.count("nLowPtElectron") && input->present("Events/nLowPtElectron") ) input->select("Events/nLowPtElectron", nLowPtElectron);
    if ( usedCounters.count("nMuon") && input->present("Events/nMuon") ) input->select("Events/nMuon", nMuon);
    if ( usedCounters.count("nOtherPV") && input->present("Events/nOtherPV") ) input->select("Events/nOtherPV", nOtherPV);
    if ( usedCounters.count("nPPSLocalTrack") && input->present("Events/nPPSLocalTrack") ) input->select("Events/nPPSLocalTrack", nPPSLocalTrack);
    if ( usedCounters.count("nPSWeight") && input->present("Events/nPSWeight") ) input->select("Events/nPSWeight", nPSWeight);
    if ( usedCounters.count("nPhoton") && input->present("Events/nPhoton") ) input->select("Events/nPhoton", nPhoton);
    if ( usedCounters.count("nProton_multiRP") && input->present("Events/nProton_multiRP") ) input->select("Events/nProton_multiRP", nProton_multiRP);
    if ( usedCounters.count("nProton_singleRP") && input->present("Events/nProton_singleRP") ) input->select("Events/nProton_singleRP", nProton_singleRP);
    if ( usedCounters.count("nSV") && input->present("Events/nSV") ) input->select("Events/nSV", nSV);
    if ( usedCounters.count("nSoftActivityJet") && input->present("Events/nSoftActivityJet") ) input->select("Events/nSoftActivityJet", nSoftActivityJet);
    if ( usedCounters.count("nSubGenJetAK8") && input->present("Events/nSubGenJetAK8") ) input->select("Events/nSubGenJetAK8", nSubGenJetAK8);
    if ( usedCounters.count("nSubJet") && input->present("Events/nSubJet") ) input->select("Events/nSubJet", nSubJet);
    if ( usedCounters.count("nTau") && input->present("Events/nTau") ) input->select("Events/nTau", nTau);
    if ( usedCounters.count("nTrigObj") && input->present("Events/nTrigObj") ) input->select("Events/nTrigObj", nTrigObj);
    if ( usedCounters.count("nboostedTau") && input->present("Events/nboostedTau") ) input->select("Events/nboostedTau", nboostedTau);


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

    output->add("nCorrT1METJet", 	nCorrT1METJet);
    output->add("nElectron", 	nElectron);
    output->add("nFatJet", 	nFatJet);
    output->add("nFsrPhoton", 	nFsrPhoton);
    output->add("nGenDressedLepton", 	nGenDressedLepton);
    output->add("nGenIsolatedPhoton", 	nGenIsolatedPhoton);
    output->add("nGenJet", 	nGenJet);
    output->add("nGenJetAK8", 	nGenJetAK8);
    output->add("nGenPart", 	nGenPart);
    output->add("nGenVisTau", 	nGenVisTau);
    output->add("nIsoTrack", 	nIsoTrack);
    output->add("nJet", 	nJet);
    output->add("nLHEPart", 	nLHEPart);
    output->add("nLHEPdfWeight", 	nLHEPdfWeight);
    output->add("nLHEReweightingWeight", 	nLHEReweightingWeight);
    output->add("nLHEScaleWeight", 	nLHEScaleWeight);
    output->add("nLowPtElectron", 	nLowPtElectron);
    output->add("nMuon", 	nMuon);
    output->add("nOtherPV", 	nOtherPV);
    output->add("nPPSLocalTrack", 	nPPSLocalTrack);
    output->add("nPSWeight", 	nPSWeight);
    output->add("nPhoton", 	nPhoton);
    output->add("nProton_multiRP", 	nProton_multiRP);
    output->add("nProton_singleRP", 	nProton_singleRP);
    output->add("nSV", 	nSV);
    output->add("nSoftActivityJet", 	nSoftActivityJet);
    output->add("nSubGenJetAK8", 	nSubGenJetAK8);
    output->add("nSubJet", 	nSubJet);
    output->add("nTau", 	nTau);
    output->add("nTrigObj", 	nTrigObj);
    output->add("nboostedTau", 	nboostedTau);
  
    output->add("Events/CaloMET_phi", 	CaloMET_phi);
    output->add("Events/CaloMET_pt", 	CaloMET_pt);
    output->add("Events/CaloMET_sumEt", 	CaloMET_sumEt);
    output->add("Events/ChsMET_phi", 	ChsMET_phi);
    output->add("Events/ChsMET_pt", 	ChsMET_pt);
    output->add("Events/ChsMET_sumEt", 	ChsMET_sumEt);
    output->add("Events/CorrT1METJet_area[nCorrT1METJet]",
                 CorrT1METJet_area);
    output->add("Events/CorrT1METJet_eta[nCorrT1METJet]",
                 CorrT1METJet_eta);
    output->add("Events/CorrT1METJet_muonSubtrFactor[nCorrT1METJet]",
                 CorrT1METJet_muonSubtrFactor);
    output->add("Events/CorrT1METJet_phi[nCorrT1METJet]",
                 CorrT1METJet_phi);
    output->add("Events/CorrT1METJet_rawPt[nCorrT1METJet]",
                 CorrT1METJet_rawPt);
    output->add("Events/DeepMETResolutionTune_phi",
                 DeepMETResolutionTune_phi);
    output->add("Events/DeepMETResolutionTune_pt",
                 DeepMETResolutionTune_pt);
    output->add("Events/DeepMETResponseTune_phi", 	DeepMETResponseTune_phi);
    output->add("Events/DeepMETResponseTune_pt", 	DeepMETResponseTune_pt);
    output->add("Events/Electron_charge[nElectron]", 	Electron_charge);
    output->add("Events/Electron_cleanmask[nElectron]",
                 Electron_cleanmask);
    output->add("Events/Electron_convVeto[nElectron]", 	Electron_convVeto);
    output->add("Events/Electron_cutBased[nElectron]", 	Electron_cutBased);
    output->add("Events/Electron_cutBased_HEEP[nElectron]",
                 Electron_cutBased_HEEP);
    output->add("Events/Electron_dEscaleDown[nElectron]",
                 Electron_dEscaleDown);
    output->add("Events/Electron_dEscaleUp[nElectron]",
                 Electron_dEscaleUp);
    output->add("Events/Electron_dEsigmaDown[nElectron]",
                 Electron_dEsigmaDown);
    output->add("Events/Electron_dEsigmaUp[nElectron]",
                 Electron_dEsigmaUp);
    output->add("Events/Electron_deltaEtaSC[nElectron]",
                 Electron_deltaEtaSC);
    output->add("Events/Electron_dr03EcalRecHitSumEt[nElectron]",
                 Electron_dr03EcalRecHitSumEt);
    output->add("Events/Electron_dr03HcalDepth1TowerSumEt[nElectron]",
                 Electron_dr03HcalDepth1TowerSumEt);
    output->add("Events/Electron_dr03TkSumPt[nElectron]",
                 Electron_dr03TkSumPt);
    output->add("Events/Electron_dr03TkSumPtHEEP[nElectron]",
                 Electron_dr03TkSumPtHEEP);
    output->add("Events/Electron_dxy[nElectron]", 	Electron_dxy);
    output->add("Events/Electron_dxyErr[nElectron]", 	Electron_dxyErr);
    output->add("Events/Electron_dz[nElectron]", 	Electron_dz);
    output->add("Events/Electron_dzErr[nElectron]", 	Electron_dzErr);
    output->add("Events/Electron_eCorr[nElectron]", 	Electron_eCorr);
    output->add("Events/Electron_eInvMinusPInv[nElectron]",
                 Electron_eInvMinusPInv);
    output->add("Events/Electron_energyErr[nElectron]",
                 Electron_energyErr);
    output->add("Events/Electron_eta[nElectron]", 	Electron_eta);
    output->add("Events/Electron_genPartFlav[nElectron]",
                 Electron_genPartFlav);
    output->add("Events/Electron_genPartIdx[nElectron]",
                 Electron_genPartIdx);
    output->add("Events/Electron_hoe[nElectron]", 	Electron_hoe);
    output->add("Events/Electron_ip3d[nElectron]", 	Electron_ip3d);
    output->add("Events/Electron_isEB[nElectron]", 	Electron_isEB);
    output->add("Events/Electron_isPFcand[nElectron]", 	Electron_isPFcand);
    output->add("Events/Electron_jetIdx[nElectron]", 	Electron_jetIdx);
    output->add("Events/Electron_jetNDauCharged[nElectron]",
                 Electron_jetNDauCharged);
    output->add("Events/Electron_jetPtRelv2[nElectron]",
                 Electron_jetPtRelv2);
    output->add("Events/Electron_jetRelIso[nElectron]",
                 Electron_jetRelIso);
    output->add("Events/Electron_lostHits[nElectron]", 	Electron_lostHits);
    output->add("Events/Electron_mass[nElectron]", 	Electron_mass);
    output->add("Events/Electron_miniPFRelIso_all[nElectron]",
                 Electron_miniPFRelIso_all);
    output->add("Events/Electron_miniPFRelIso_chg[nElectron]",
                 Electron_miniPFRelIso_chg);
    output->add("Events/Electron_mvaFall17V2Iso[nElectron]",
                 Electron_mvaFall17V2Iso);
    output->add("Events/Electron_mvaFall17V2Iso_WP80[nElectron]",
                 Electron_mvaFall17V2Iso_WP80);
    output->add("Events/Electron_mvaFall17V2Iso_WP90[nElectron]",
                 Electron_mvaFall17V2Iso_WP90);
    output->add("Events/Electron_mvaFall17V2Iso_WPL[nElectron]",
                 Electron_mvaFall17V2Iso_WPL);
    output->add("Events/Electron_mvaFall17V2noIso[nElectron]",
                 Electron_mvaFall17V2noIso);
    output->add("Events/Electron_mvaFall17V2noIso_WP80[nElectron]",
                 Electron_mvaFall17V2noIso_WP80);
    output->add("Events/Electron_mvaFall17V2noIso_WP90[nElectron]",
                 Electron_mvaFall17V2noIso_WP90);
    output->add("Events/Electron_mvaFall17V2noIso_WPL[nElectron]",
                 Electron_mvaFall17V2noIso_WPL);
    output->add("Events/Electron_mvaIso[nElectron]", 	Electron_mvaIso);
    output->add("Events/Electron_mvaIso_WP80[nElectron]",
                 Electron_mvaIso_WP80);
    output->add("Events/Electron_mvaIso_WP90[nElectron]",
                 Electron_mvaIso_WP90);
    output->add("Events/Electron_mvaIso_WPL[nElectron]",
                 Electron_mvaIso_WPL);
    output->add("Events/Electron_mvaNoIso[nElectron]", 	Electron_mvaNoIso);
    output->add("Events/Electron_mvaNoIso_WP80[nElectron]",
                 Electron_mvaNoIso_WP80);
    output->add("Events/Electron_mvaNoIso_WP90[nElectron]",
                 Electron_mvaNoIso_WP90);
    output->add("Events/Electron_mvaNoIso_WPL[nElectron]",
                 Electron_mvaNoIso_WPL);
    output->add("Events/Electron_mvaTTH[nElectron]", 	Electron_mvaTTH);
    output->add("Events/Electron_pdgId[nElectron]", 	Electron_pdgId);
    output->add("Events/Electron_pfRelIso03_all[nElectron]",
                 Electron_pfRelIso03_all);
    output->add("Events/Electron_pfRelIso03_chg[nElectron]",
                 Electron_pfRelIso03_chg);
    output->add("Events/Electron_pfRelIso04_all[nElectron]",
                 Electron_pfRelIso04_all);
    output->add("Events/Electron_phi[nElectron]", 	Electron_phi);
    output->add("Events/Electron_photonIdx[nElectron]",
                 Electron_photonIdx);
    output->add("Events/Electron_promptMVA[nElectron]",
                 Electron_promptMVA);
    output->add("Events/Electron_pt[nElectron]", 	Electron_pt);
    output->add("Events/Electron_r9[nElectron]", 	Electron_r9);
    output->add("Events/Electron_scEtOverPt[nElectron]",
                 Electron_scEtOverPt);
    output->add("Events/Electron_seedGain[nElectron]", 	Electron_seedGain);
    output->add("Events/Electron_sieie[nElectron]", 	Electron_sieie);
    output->add("Events/Electron_sip3d[nElectron]", 	Electron_sip3d);
    output->add("Events/Electron_superclusterEta[nElectron]",
                 Electron_superclusterEta);
    output->add("Events/Electron_tightCharge[nElectron]",
                 Electron_tightCharge);
    output->add("Events/Electron_vidNestedWPBitmap[nElectron]",
                 Electron_vidNestedWPBitmap);
    output->add("Events/Electron_vidNestedWPBitmapHEEP[nElectron]",
                 Electron_vidNestedWPBitmapHEEP);
    output->add("Events/FatJet_area[nFatJet]", 	FatJet_area);
    output->add("Events/FatJet_btagCSVV2[nFatJet]", 	FatJet_btagCSVV2);
    output->add("Events/FatJet_btagDDBvLV2[nFatJet]", 	FatJet_btagDDBvLV2);
    output->add("Events/FatJet_btagDDCvBV2[nFatJet]", 	FatJet_btagDDCvBV2);
    output->add("Events/FatJet_btagDDCvLV2[nFatJet]", 	FatJet_btagDDCvLV2);
    output->add("Events/FatJet_btagDeepB[nFatJet]", 	FatJet_btagDeepB);
    output->add("Events/FatJet_btagHbb[nFatJet]", 	FatJet_btagHbb);
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
    output->add("Events/FatJet_electronIdx3SJ[nFatJet]",
                 FatJet_electronIdx3SJ);
    output->add("Events/FatJet_eta[nFatJet]", 	FatJet_eta);
    output->add("Events/FatJet_genJetAK8Idx[nFatJet]",
                 FatJet_genJetAK8Idx);
    output->add("Events/FatJet_hadronFlavour[nFatJet]",
                 FatJet_hadronFlavour);
    output->add("Events/FatJet_jetId[nFatJet]", 	FatJet_jetId);
    output->add("Events/FatJet_lsf3[nFatJet]", 	FatJet_lsf3);
    output->add("Events/FatJet_mass[nFatJet]", 	FatJet_mass);
    output->add("Events/FatJet_msoftdrop[nFatJet]", 	FatJet_msoftdrop);
    output->add("Events/FatJet_muonIdx3SJ[nFatJet]", 	FatJet_muonIdx3SJ);
    output->add("Events/FatJet_n2b1[nFatJet]", 	FatJet_n2b1);
    output->add("Events/FatJet_n3b1[nFatJet]", 	FatJet_n3b1);
    output->add("Events/FatJet_nBHadrons[nFatJet]", 	FatJet_nBHadrons);
    output->add("Events/FatJet_nCHadrons[nFatJet]", 	FatJet_nCHadrons);
    output->add("Events/FatJet_nConstituents[nFatJet]",
                 FatJet_nConstituents);
    output->add("Events/FatJet_particleNetMD_QCD[nFatJet]",
                 FatJet_particleNetMD_QCD);
    output->add("Events/FatJet_particleNetMD_Xbb[nFatJet]",
                 FatJet_particleNetMD_Xbb);
    output->add("Events/FatJet_particleNetMD_Xcc[nFatJet]",
                 FatJet_particleNetMD_Xcc);
    output->add("Events/FatJet_particleNetMD_Xqq[nFatJet]",
                 FatJet_particleNetMD_Xqq);
    output->add("Events/FatJet_particleNet_H4qvsQCD[nFatJet]",
                 FatJet_particleNet_H4qvsQCD);
    output->add("Events/FatJet_particleNet_HbbvsQCD[nFatJet]",
                 FatJet_particleNet_HbbvsQCD);
    output->add("Events/FatJet_particleNet_HccvsQCD[nFatJet]",
                 FatJet_particleNet_HccvsQCD);
    output->add("Events/FatJet_particleNet_QCD[nFatJet]",
                 FatJet_particleNet_QCD);
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
    output->add("Events/FatJet_subJetIdx1[nFatJet]", 	FatJet_subJetIdx1);
    output->add("Events/FatJet_subJetIdx2[nFatJet]", 	FatJet_subJetIdx2);
    output->add("Events/FatJet_tau1[nFatJet]", 	FatJet_tau1);
    output->add("Events/FatJet_tau2[nFatJet]", 	FatJet_tau2);
    output->add("Events/FatJet_tau3[nFatJet]", 	FatJet_tau3);
    output->add("Events/FatJet_tau4[nFatJet]", 	FatJet_tau4);
    output->add("Events/Flag_BadChargedCandidateFilter",
                 Flag_BadChargedCandidateFilter);
    output->add("Events/Flag_BadChargedCandidateFilter_pRECO",
                 Flag_BadChargedCandidateFilter_pRECO);
    output->add("Events/Flag_BadChargedCandidateSummer16Filter",
                 Flag_BadChargedCandidateSummer16Filter);
    output->add("Events/Flag_BadChargedCandidateSummer16Filter_pRECO",
                 Flag_BadChargedCandidateSummer16Filter_pRECO);
    output->add("Events/Flag_BadPFMuonDzFilter", 	Flag_BadPFMuonDzFilter);
    output->add("Events/Flag_BadPFMuonDzFilter_pRECO",
                 Flag_BadPFMuonDzFilter_pRECO);
    output->add("Events/Flag_BadPFMuonFilter", 	Flag_BadPFMuonFilter);
    output->add("Events/Flag_BadPFMuonFilter_pRECO",
                 Flag_BadPFMuonFilter_pRECO);
    output->add("Events/Flag_BadPFMuonSummer16Filter",
                 Flag_BadPFMuonSummer16Filter);
    output->add("Events/Flag_BadPFMuonSummer16Filter_pRECO",
                 Flag_BadPFMuonSummer16Filter_pRECO);
    output->add("Events/Flag_CSCTightHalo2015Filter",
                 Flag_CSCTightHalo2015Filter);
    output->add("Events/Flag_CSCTightHalo2015Filter_pRECO",
                 Flag_CSCTightHalo2015Filter_pRECO);
    output->add("Events/Flag_CSCTightHaloFilter", 	Flag_CSCTightHaloFilter);
    output->add("Events/Flag_CSCTightHaloFilter_pRECO",
                 Flag_CSCTightHaloFilter_pRECO);
    output->add("Events/Flag_CSCTightHaloTrkMuUnvetoFilter",
                 Flag_CSCTightHaloTrkMuUnvetoFilter);
    output->add("Events/Flag_CSCTightHaloTrkMuUnvetoFilter_pRECO",
                 Flag_CSCTightHaloTrkMuUnvetoFilter_pRECO);
    output->add("Events/Flag_EcalDeadCellBoundaryEnergyFilter",
                 Flag_EcalDeadCellBoundaryEnergyFilter);
    output->add("Events/Flag_EcalDeadCellBoundaryEnergyFilter_pRECO",
                 Flag_EcalDeadCellBoundaryEnergyFilter_pRECO);
    output->add("Events/Flag_EcalDeadCellTriggerPrimitiveFilter",
                 Flag_EcalDeadCellTriggerPrimitiveFilter);
    output->add("Events/Flag_EcalDeadCellTriggerPrimitiveFilter_pRECO",
                 Flag_EcalDeadCellTriggerPrimitiveFilter_pRECO);
    output->add("Events/Flag_HBHENoiseFilter", 	Flag_HBHENoiseFilter);
    output->add("Events/Flag_HBHENoiseFilter_pRECO",
                 Flag_HBHENoiseFilter_pRECO);
    output->add("Events/Flag_HBHENoiseIsoFilter", 	Flag_HBHENoiseIsoFilter);
    output->add("Events/Flag_HBHENoiseIsoFilter_pRECO",
                 Flag_HBHENoiseIsoFilter_pRECO);
    output->add("Events/Flag_HcalStripHaloFilter",
                 Flag_HcalStripHaloFilter);
    output->add("Events/Flag_HcalStripHaloFilter_pRECO",
                 Flag_HcalStripHaloFilter_pRECO);
    output->add("Events/Flag_METFilters", 	Flag_METFilters);
    output->add("Events/Flag_METFilters_pRECO", 	Flag_METFilters_pRECO);
    output->add("Events/Flag_chargedHadronTrackResolutionFilter",
                 Flag_chargedHadronTrackResolutionFilter);
    output->add("Events/Flag_chargedHadronTrackResolutionFilter_pRECO",
                 Flag_chargedHadronTrackResolutionFilter_pRECO);
    output->add("Events/Flag_ecalBadCalibFilter", 	Flag_ecalBadCalibFilter);
    output->add("Events/Flag_ecalBadCalibFilter_pRECO",
                 Flag_ecalBadCalibFilter_pRECO);
    output->add("Events/Flag_ecalLaserCorrFilter",
                 Flag_ecalLaserCorrFilter);
    output->add("Events/Flag_ecalLaserCorrFilter_pRECO",
                 Flag_ecalLaserCorrFilter_pRECO);
    output->add("Events/Flag_eeBadScFilter", 	Flag_eeBadScFilter);
    output->add("Events/Flag_eeBadScFilter_pRECO",
                 Flag_eeBadScFilter_pRECO);
    output->add("Events/Flag_globalSuperTightHalo2016Filter",
                 Flag_globalSuperTightHalo2016Filter);
    output->add("Events/Flag_globalSuperTightHalo2016Filter_pRECO",
                 Flag_globalSuperTightHalo2016Filter_pRECO);
    output->add("Events/Flag_globalTightHalo2016Filter",
                 Flag_globalTightHalo2016Filter);
    output->add("Events/Flag_globalTightHalo2016Filter_pRECO",
                 Flag_globalTightHalo2016Filter_pRECO);
    output->add("Events/Flag_goodVertices", 	Flag_goodVertices);
    output->add("Events/Flag_goodVertices_pRECO", 	Flag_goodVertices_pRECO);
    output->add("Events/Flag_hcalLaserEventFilter",
                 Flag_hcalLaserEventFilter);
    output->add("Events/Flag_hcalLaserEventFilter_pRECO",
                 Flag_hcalLaserEventFilter_pRECO);
    output->add("Events/Flag_hfNoisyHitsFilter", 	Flag_hfNoisyHitsFilter);
    output->add("Events/Flag_hfNoisyHitsFilter_pRECO",
                 Flag_hfNoisyHitsFilter_pRECO);
    output->add("Events/Flag_muonBadTrackFilter", 	Flag_muonBadTrackFilter);
    output->add("Events/Flag_muonBadTrackFilter_pRECO",
                 Flag_muonBadTrackFilter_pRECO);
    output->add("Events/Flag_trkPOGFilters", 	Flag_trkPOGFilters);
    output->add("Events/Flag_trkPOGFilters_pRECO",
                 Flag_trkPOGFilters_pRECO);
    output->add("Events/Flag_trkPOG_logErrorTooManyClusters",
                 Flag_trkPOG_logErrorTooManyClusters);
    output->add("Events/Flag_trkPOG_logErrorTooManyClusters_pRECO",
                 Flag_trkPOG_logErrorTooManyClusters_pRECO);
    output->add("Events/Flag_trkPOG_manystripclus53X",
                 Flag_trkPOG_manystripclus53X);
    output->add("Events/Flag_trkPOG_manystripclus53X_pRECO",
                 Flag_trkPOG_manystripclus53X_pRECO);
    output->add("Events/Flag_trkPOG_toomanystripclus53X",
                 Flag_trkPOG_toomanystripclus53X);
    output->add("Events/Flag_trkPOG_toomanystripclus53X_pRECO",
                 Flag_trkPOG_toomanystripclus53X_pRECO);
    output->add("Events/FsrPhoton_dROverEt2[nFsrPhoton]",
                 FsrPhoton_dROverEt2);
    output->add("Events/FsrPhoton_eta[nFsrPhoton]", 	FsrPhoton_eta);
    output->add("Events/FsrPhoton_muonIdx[nFsrPhoton]", 	FsrPhoton_muonIdx);
    output->add("Events/FsrPhoton_phi[nFsrPhoton]", 	FsrPhoton_phi);
    output->add("Events/FsrPhoton_pt[nFsrPhoton]", 	FsrPhoton_pt);
    output->add("Events/FsrPhoton_relIso03[nFsrPhoton]",
                 FsrPhoton_relIso03);
    output->add("Events/GenDressedLepton_eta[nGenDressedLepton]",
                 GenDressedLepton_eta);
    output->add("Events/GenDressedLepton_hasTauAnc[nGenDressedLepton]",
                 GenDressedLepton_hasTauAnc);
    output->add("Events/GenDressedLepton_mass[nGenDressedLepton]",
                 GenDressedLepton_mass);
    output->add("Events/GenDressedLepton_pdgId[nGenDressedLepton]",
                 GenDressedLepton_pdgId);
    output->add("Events/GenDressedLepton_phi[nGenDressedLepton]",
                 GenDressedLepton_phi);
    output->add("Events/GenDressedLepton_pt[nGenDressedLepton]",
                 GenDressedLepton_pt);
    output->add("Events/GenIsolatedPhoton_eta[nGenIsolatedPhoton]",
                 GenIsolatedPhoton_eta);
    output->add("Events/GenIsolatedPhoton_mass[nGenIsolatedPhoton]",
                 GenIsolatedPhoton_mass);
    output->add("Events/GenIsolatedPhoton_phi[nGenIsolatedPhoton]",
                 GenIsolatedPhoton_phi);
    output->add("Events/GenIsolatedPhoton_pt[nGenIsolatedPhoton]",
                 GenIsolatedPhoton_pt);
    output->add("Events/GenJetAK8_eta[nGenJetAK8]", 	GenJetAK8_eta);
    output->add("Events/GenJetAK8_hadronFlavour[nGenJetAK8]",
                 GenJetAK8_hadronFlavour);
    output->add("Events/GenJetAK8_mass[nGenJetAK8]", 	GenJetAK8_mass);
    output->add("Events/GenJetAK8_partonFlavour[nGenJetAK8]",
                 GenJetAK8_partonFlavour);
    output->add("Events/GenJetAK8_phi[nGenJetAK8]", 	GenJetAK8_phi);
    output->add("Events/GenJetAK8_pt[nGenJetAK8]", 	GenJetAK8_pt);
    output->add("Events/GenJet_eta[nGenJet]", 	GenJet_eta);
    output->add("Events/GenJet_hadronFlavour[nGenJet]",
                 GenJet_hadronFlavour);
    output->add("Events/GenJet_mass[nGenJet]", 	GenJet_mass);
    output->add("Events/GenJet_nBHadrons[nGenJet]", 	GenJet_nBHadrons);
    output->add("Events/GenJet_nCHadrons[nGenJet]", 	GenJet_nCHadrons);
    output->add("Events/GenJet_partonFlavour[nGenJet]",
                 GenJet_partonFlavour);
    output->add("Events/GenJet_phi[nGenJet]", 	GenJet_phi);
    output->add("Events/GenJet_pt[nGenJet]", 	GenJet_pt);
    output->add("Events/GenMET_phi", 	GenMET_phi);
    output->add("Events/GenMET_pt", 	GenMET_pt);
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
    output->add("Events/GenVisTau_charge[nGenVisTau]", 	GenVisTau_charge);
    output->add("Events/GenVisTau_eta[nGenVisTau]", 	GenVisTau_eta);
    output->add("Events/GenVisTau_genPartIdxMother[nGenVisTau]",
                 GenVisTau_genPartIdxMother);
    output->add("Events/GenVisTau_mass[nGenVisTau]", 	GenVisTau_mass);
    output->add("Events/GenVisTau_phi[nGenVisTau]", 	GenVisTau_phi);
    output->add("Events/GenVisTau_pt[nGenVisTau]", 	GenVisTau_pt);
    output->add("Events/GenVisTau_status[nGenVisTau]", 	GenVisTau_status);
    output->add("Events/GenVtx_t0", 	GenVtx_t0);
    output->add("Events/GenVtx_x", 	GenVtx_x);
    output->add("Events/GenVtx_y", 	GenVtx_y);
    output->add("Events/GenVtx_z", 	GenVtx_z);
    output->add("Events/Generator_binvar", 	Generator_binvar);
    output->add("Events/Generator_id1", 	Generator_id1);
    output->add("Events/Generator_id2", 	Generator_id2);
    output->add("Events/Generator_scalePDF", 	Generator_scalePDF);
    output->add("Events/Generator_weight", 	Generator_weight);
    output->add("Events/Generator_x1", 	Generator_x1);
    output->add("Events/Generator_x2", 	Generator_x2);
    output->add("Events/Generator_xpdf1", 	Generator_xpdf1);
    output->add("Events/Generator_xpdf2", 	Generator_xpdf2);
    output->add("Events/HLT_Ele30_WPTight_Gsf", 	HLT_Ele30_WPTight_Gsf);
    output->add("Events/HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07",
                 HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07);
    output->add("Events/HLT_IsoMu24", 	HLT_IsoMu24);
    output->add("Events/HLT_IsoMu27", 	HLT_IsoMu27);
    output->add("Events/HLT_PFHT1050", 	HLT_PFHT1050);
    output->add("Events/HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p55",
                 HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p55);
    output->add("Events/HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p60",
                 HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p60);
    output->add("Events/HLT_PFHT280_QuadPFJet35_PNet2BTagMean0p60",
                 HLT_PFHT280_QuadPFJet35_PNet2BTagMean0p60);
    output->add("Events/HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0",
                 HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0);
    output->add("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40",
                 HLT_PFHT330PT30_QuadPFJet_75_60_45_40);
    output->add("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_2p0",
                 HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_2p0);
    output->add("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3",
                 HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3);
    output->add("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5",
                 HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5);
    output->add("Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5",
                 HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5);
    output->add("Events/HLT_PFHT340_QuadPFJet70_50_40_40_PNet2BTagMean0p70",
                 HLT_PFHT340_QuadPFJet70_50_40_40_PNet2BTagMean0p70);
    output->add("Events/HLT_PFHT380_SixJet32_DoubleBTagCSV_p075",
                 HLT_PFHT380_SixJet32_DoubleBTagCSV_p075);
    output->add("Events/HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2",
                 HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2);
    output->add("Events/HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_4p3",
                 HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_4p3);
    output->add("Events/HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_5p6",
                 HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_5p6);
    output->add("Events/HLT_PFHT400_SixPFJet32", 	HLT_PFHT400_SixPFJet32);
    output->add("Events/HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94",
                 HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94);
    output->add("Events/HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50",
                 HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50);
    output->add("Events/HLT_PFHT430_SixJet40_BTagCSV_p080",
                 HLT_PFHT430_SixJet40_BTagCSV_p080);
    output->add("Events/HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5",
                 HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5);
    output->add("Events/HLT_PFHT450_SixPFJet36", 	HLT_PFHT450_SixPFJet36);
    output->add("Events/HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59",
                 HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59);
    output->add("Events/HLT_PFHT450_SixPFJet36_PNetBTag0p35",
                 HLT_PFHT450_SixPFJet36_PNetBTag0p35);
    output->add("Events/HLTriggerFinalPath", 	HLTriggerFinalPath);
    output->add("Events/HLTriggerFirstPath", 	HLTriggerFirstPath);
    output->add("Events/HTXS_Higgs_pt", 	HTXS_Higgs_pt);
    output->add("Events/HTXS_Higgs_y", 	HTXS_Higgs_y);
    output->add("Events/HTXS_njets25", 	HTXS_njets25);
    output->add("Events/HTXS_njets30", 	HTXS_njets30);
    output->add("Events/HTXS_stage1_1_cat_pTjet25GeV",
                 HTXS_stage1_1_cat_pTjet25GeV);
    output->add("Events/HTXS_stage1_1_cat_pTjet30GeV",
                 HTXS_stage1_1_cat_pTjet30GeV);
    output->add("Events/HTXS_stage1_1_fine_cat_pTjet25GeV",
                 HTXS_stage1_1_fine_cat_pTjet25GeV);
    output->add("Events/HTXS_stage1_1_fine_cat_pTjet30GeV",
                 HTXS_stage1_1_fine_cat_pTjet30GeV);
    output->add("Events/HTXS_stage1_2_cat_pTjet25GeV",
                 HTXS_stage1_2_cat_pTjet25GeV);
    output->add("Events/HTXS_stage1_2_cat_pTjet30GeV",
                 HTXS_stage1_2_cat_pTjet30GeV);
    output->add("Events/HTXS_stage1_2_fine_cat_pTjet25GeV",
                 HTXS_stage1_2_fine_cat_pTjet25GeV);
    output->add("Events/HTXS_stage1_2_fine_cat_pTjet30GeV",
                 HTXS_stage1_2_fine_cat_pTjet30GeV);
    output->add("Events/HTXS_stage_0", 	HTXS_stage_0);
    output->add("Events/HTXS_stage_1_pTjet25", 	HTXS_stage_1_pTjet25);
    output->add("Events/HTXS_stage_1_pTjet30", 	HTXS_stage_1_pTjet30);
    output->add("Events/IsoTrack_charge[nIsoTrack]", 	IsoTrack_charge);
    output->add("Events/IsoTrack_dxy[nIsoTrack]", 	IsoTrack_dxy);
    output->add("Events/IsoTrack_dz[nIsoTrack]", 	IsoTrack_dz);
    output->add("Events/IsoTrack_eta[nIsoTrack]", 	IsoTrack_eta);
    output->add("Events/IsoTrack_fromPV[nIsoTrack]", 	IsoTrack_fromPV);
    output->add("Events/IsoTrack_isFromLostTrack[nIsoTrack]",
                 IsoTrack_isFromLostTrack);
    output->add("Events/IsoTrack_isHighPurityTrack[nIsoTrack]",
                 IsoTrack_isHighPurityTrack);
    output->add("Events/IsoTrack_isPFcand[nIsoTrack]", 	IsoTrack_isPFcand);
    output->add("Events/IsoTrack_miniPFRelIso_all[nIsoTrack]",
                 IsoTrack_miniPFRelIso_all);
    output->add("Events/IsoTrack_miniPFRelIso_chg[nIsoTrack]",
                 IsoTrack_miniPFRelIso_chg);
    output->add("Events/IsoTrack_pdgId[nIsoTrack]", 	IsoTrack_pdgId);
    output->add("Events/IsoTrack_pfRelIso03_all[nIsoTrack]",
                 IsoTrack_pfRelIso03_all);
    output->add("Events/IsoTrack_pfRelIso03_chg[nIsoTrack]",
                 IsoTrack_pfRelIso03_chg);
    output->add("Events/IsoTrack_phi[nIsoTrack]", 	IsoTrack_phi);
    output->add("Events/IsoTrack_pt[nIsoTrack]", 	IsoTrack_pt);
    output->add("Events/Jet_PNetRegPtRawCorr[nJet]", 	Jet_PNetRegPtRawCorr);
    output->add("Events/Jet_PNetRegPtRawCorrNeutrino[nJet]",
                 Jet_PNetRegPtRawCorrNeutrino);
    output->add("Events/Jet_PNetRegPtRawRes[nJet]", 	Jet_PNetRegPtRawRes);
    output->add("Events/Jet_UParTAK4RegPtRawCorr[nJet]",
                 Jet_UParTAK4RegPtRawCorr);
    output->add("Events/Jet_UParTAK4RegPtRawCorrNeutrino[nJet]",
                 Jet_UParTAK4RegPtRawCorrNeutrino);
    output->add("Events/Jet_UParTAK4RegPtRawRes[nJet]",
                 Jet_UParTAK4RegPtRawRes);
    output->add("Events/Jet_UParTAK4V1RegPtRawCorr[nJet]",
                 Jet_UParTAK4V1RegPtRawCorr);
    output->add("Events/Jet_UParTAK4V1RegPtRawCorrNeutrino[nJet]",
                 Jet_UParTAK4V1RegPtRawCorrNeutrino);
    output->add("Events/Jet_UParTAK4V1RegPtRawRes[nJet]",
                 Jet_UParTAK4V1RegPtRawRes);
    output->add("Events/Jet_area[nJet]", 	Jet_area);
    output->add("Events/Jet_bRegCorr[nJet]", 	Jet_bRegCorr);
    output->add("Events/Jet_bRegRes[nJet]", 	Jet_bRegRes);
    output->add("Events/Jet_btagCSVV2[nJet]", 	Jet_btagCSVV2);
    output->add("Events/Jet_btagDeepB[nJet]", 	Jet_btagDeepB);
    output->add("Events/Jet_btagDeepCvB[nJet]", 	Jet_btagDeepCvB);
    output->add("Events/Jet_btagDeepCvL[nJet]", 	Jet_btagDeepCvL);
    output->add("Events/Jet_btagDeepFlavB[nJet]", 	Jet_btagDeepFlavB);
    output->add("Events/Jet_btagDeepFlavCvB[nJet]", 	Jet_btagDeepFlavCvB);
    output->add("Events/Jet_btagDeepFlavCvL[nJet]", 	Jet_btagDeepFlavCvL);
    output->add("Events/Jet_btagDeepFlavQG[nJet]", 	Jet_btagDeepFlavQG);
    output->add("Events/Jet_btagPNetB[nJet]", 	Jet_btagPNetB);
    output->add("Events/Jet_btagPNetCvB[nJet]", 	Jet_btagPNetCvB);
    output->add("Events/Jet_btagPNetCvL[nJet]", 	Jet_btagPNetCvL);
    output->add("Events/Jet_btagPNetQvG[nJet]", 	Jet_btagPNetQvG);
    output->add("Events/Jet_btagUParTAK4B[nJet]", 	Jet_btagUParTAK4B);
    output->add("Events/Jet_btagUParTAK4CvB[nJet]", 	Jet_btagUParTAK4CvB);
    output->add("Events/Jet_btagUParTAK4CvL[nJet]", 	Jet_btagUParTAK4CvL);
    output->add("Events/Jet_btagUParTAK4QvG[nJet]", 	Jet_btagUParTAK4QvG);
    output->add("Events/Jet_cRegCorr[nJet]", 	Jet_cRegCorr);
    output->add("Events/Jet_cRegRes[nJet]", 	Jet_cRegRes);
    output->add("Events/Jet_chEmEF[nJet]", 	Jet_chEmEF);
    output->add("Events/Jet_chFPV0EF[nJet]", 	Jet_chFPV0EF);
    output->add("Events/Jet_chHEF[nJet]", 	Jet_chHEF);
    output->add("Events/Jet_chMultiplicity[nJet]", 	Jet_chMultiplicity);
    output->add("Events/Jet_cleanmask[nJet]", 	Jet_cleanmask);
    output->add("Events/Jet_electronIdx1[nJet]", 	Jet_electronIdx1);
    output->add("Events/Jet_electronIdx2[nJet]", 	Jet_electronIdx2);
    output->add("Events/Jet_eta[nJet]", 	Jet_eta);
    output->add("Events/Jet_genJetIdx[nJet]", 	Jet_genJetIdx);
    output->add("Events/Jet_hadronFlavour[nJet]", 	Jet_hadronFlavour);
    output->add("Events/Jet_hfEmEF[nJet]", 	Jet_hfEmEF);
    output->add("Events/Jet_hfHEF[nJet]", 	Jet_hfHEF);
    output->add("Events/Jet_hfadjacentEtaStripsSize[nJet]",
                 Jet_hfadjacentEtaStripsSize);
    output->add("Events/Jet_hfcentralEtaStripSize[nJet]",
                 Jet_hfcentralEtaStripSize);
    output->add("Events/Jet_hfsigmaEtaEta[nJet]", 	Jet_hfsigmaEtaEta);
    output->add("Events/Jet_hfsigmaPhiPhi[nJet]", 	Jet_hfsigmaPhiPhi);
    output->add("Events/Jet_jetId[nJet]", 	Jet_jetId);
    output->add("Events/Jet_mass[nJet]", 	Jet_mass);
    output->add("Events/Jet_muEF[nJet]", 	Jet_muEF);
    output->add("Events/Jet_muonIdx1[nJet]", 	Jet_muonIdx1);
    output->add("Events/Jet_muonIdx2[nJet]", 	Jet_muonIdx2);
    output->add("Events/Jet_muonSubtrFactor[nJet]", 	Jet_muonSubtrFactor);
    output->add("Events/Jet_nConstituents[nJet]", 	Jet_nConstituents);
    output->add("Events/Jet_nElectrons[nJet]", 	Jet_nElectrons);
    output->add("Events/Jet_nMuons[nJet]", 	Jet_nMuons);
    output->add("Events/Jet_neEmEF[nJet]", 	Jet_neEmEF);
    output->add("Events/Jet_neHEF[nJet]", 	Jet_neHEF);
    output->add("Events/Jet_neMultiplicity[nJet]", 	Jet_neMultiplicity);
    output->add("Events/Jet_partonFlavour[nJet]", 	Jet_partonFlavour);
    output->add("Events/Jet_phi[nJet]", 	Jet_phi);
    output->add("Events/Jet_pt[nJet]", 	Jet_pt);
    output->add("Events/Jet_puId[nJet]", 	Jet_puId);
    output->add("Events/Jet_puIdDisc[nJet]", 	Jet_puIdDisc);
    output->add("Events/Jet_qgl[nJet]", 	Jet_qgl);
    output->add("Events/Jet_rawFactor[nJet]", 	Jet_rawFactor);
    output->add("Events/L1PreFiringWeight_Dn", 	L1PreFiringWeight_Dn);
    output->add("Events/L1PreFiringWeight_ECAL_Dn",
                 L1PreFiringWeight_ECAL_Dn);
    output->add("Events/L1PreFiringWeight_ECAL_Nom",
                 L1PreFiringWeight_ECAL_Nom);
    output->add("Events/L1PreFiringWeight_ECAL_Up",
                 L1PreFiringWeight_ECAL_Up);
    output->add("Events/L1PreFiringWeight_Muon_Nom",
                 L1PreFiringWeight_Muon_Nom);
    output->add("Events/L1PreFiringWeight_Muon_StatDn",
                 L1PreFiringWeight_Muon_StatDn);
    output->add("Events/L1PreFiringWeight_Muon_StatUp",
                 L1PreFiringWeight_Muon_StatUp);
    output->add("Events/L1PreFiringWeight_Muon_SystDn",
                 L1PreFiringWeight_Muon_SystDn);
    output->add("Events/L1PreFiringWeight_Muon_SystUp",
                 L1PreFiringWeight_Muon_SystUp);
    output->add("Events/L1PreFiringWeight_Nom", 	L1PreFiringWeight_Nom);
    output->add("Events/L1PreFiringWeight_Up", 	L1PreFiringWeight_Up);
    output->add("Events/L1Reco_step", 	L1Reco_step);
    output->add("Events/L1simulation_step", 	L1simulation_step);
    output->add("Events/LHEPart_eta[nLHEPart]", 	LHEPart_eta);
    output->add("Events/LHEPart_incomingpz[nLHEPart]", 	LHEPart_incomingpz);
    output->add("Events/LHEPart_mass[nLHEPart]", 	LHEPart_mass);
    output->add("Events/LHEPart_pdgId[nLHEPart]", 	LHEPart_pdgId);
    output->add("Events/LHEPart_phi[nLHEPart]", 	LHEPart_phi);
    output->add("Events/LHEPart_pt[nLHEPart]", 	LHEPart_pt);
    output->add("Events/LHEPart_spin[nLHEPart]", 	LHEPart_spin);
    output->add("Events/LHEPart_status[nLHEPart]", 	LHEPart_status);
    output->add("Events/LHEPdfWeight[nLHEPdfWeight]", 	LHEPdfWeight);
    output->add("Events/LHEReweightingWeight[nLHEReweightingWeight]",
                 LHEReweightingWeight);
    output->add("Events/LHEScaleWeight[nLHEScaleWeight]", 	LHEScaleWeight);
    output->add("Events/LHEWeight_originalXWGTUP",
                 LHEWeight_originalXWGTUP);
    output->add("Events/LHE_AlphaS", 	LHE_AlphaS);
    output->add("Events/LHE_HT", 	LHE_HT);
    output->add("Events/LHE_HTIncoming", 	LHE_HTIncoming);
    output->add("Events/LHE_Nb", 	LHE_Nb);
    output->add("Events/LHE_Nc", 	LHE_Nc);
    output->add("Events/LHE_Nglu", 	LHE_Nglu);
    output->add("Events/LHE_Njets", 	LHE_Njets);
    output->add("Events/LHE_NpLO", 	LHE_NpLO);
    output->add("Events/LHE_NpNLO", 	LHE_NpNLO);
    output->add("Events/LHE_Nuds", 	LHE_Nuds);
    output->add("Events/LHE_Vpt", 	LHE_Vpt);
    output->add("Events/LowPtElectron_ID[nLowPtElectron]",
                 LowPtElectron_ID);
    output->add("Events/LowPtElectron_charge[nLowPtElectron]",
                 LowPtElectron_charge);
    output->add("Events/LowPtElectron_convVeto[nLowPtElectron]",
                 LowPtElectron_convVeto);
    output->add("Events/LowPtElectron_convVtxRadius[nLowPtElectron]",
                 LowPtElectron_convVtxRadius);
    output->add("Events/LowPtElectron_convWP[nLowPtElectron]",
                 LowPtElectron_convWP);
    output->add("Events/LowPtElectron_deltaEtaSC[nLowPtElectron]",
                 LowPtElectron_deltaEtaSC);
    output->add("Events/LowPtElectron_dxy[nLowPtElectron]",
                 LowPtElectron_dxy);
    output->add("Events/LowPtElectron_dxyErr[nLowPtElectron]",
                 LowPtElectron_dxyErr);
    output->add("Events/LowPtElectron_dz[nLowPtElectron]",
                 LowPtElectron_dz);
    output->add("Events/LowPtElectron_dzErr[nLowPtElectron]",
                 LowPtElectron_dzErr);
    output->add("Events/LowPtElectron_eInvMinusPInv[nLowPtElectron]",
                 LowPtElectron_eInvMinusPInv);
    output->add("Events/LowPtElectron_embeddedID[nLowPtElectron]",
                 LowPtElectron_embeddedID);
    output->add("Events/LowPtElectron_energyErr[nLowPtElectron]",
                 LowPtElectron_energyErr);
    output->add("Events/LowPtElectron_eta[nLowPtElectron]",
                 LowPtElectron_eta);
    output->add("Events/LowPtElectron_genPartFlav[nLowPtElectron]",
                 LowPtElectron_genPartFlav);
    output->add("Events/LowPtElectron_genPartIdx[nLowPtElectron]",
                 LowPtElectron_genPartIdx);
    output->add("Events/LowPtElectron_hoe[nLowPtElectron]",
                 LowPtElectron_hoe);
    output->add("Events/LowPtElectron_lostHits[nLowPtElectron]",
                 LowPtElectron_lostHits);
    output->add("Events/LowPtElectron_mass[nLowPtElectron]",
                 LowPtElectron_mass);
    output->add("Events/LowPtElectron_miniPFRelIso_all[nLowPtElectron]",
                 LowPtElectron_miniPFRelIso_all);
    output->add("Events/LowPtElectron_miniPFRelIso_chg[nLowPtElectron]",
                 LowPtElectron_miniPFRelIso_chg);
    output->add("Events/LowPtElectron_pdgId[nLowPtElectron]",
                 LowPtElectron_pdgId);
    output->add("Events/LowPtElectron_phi[nLowPtElectron]",
                 LowPtElectron_phi);
    output->add("Events/LowPtElectron_pt[nLowPtElectron]",
                 LowPtElectron_pt);
    output->add("Events/LowPtElectron_ptbiased[nLowPtElectron]",
                 LowPtElectron_ptbiased);
    output->add("Events/LowPtElectron_r9[nLowPtElectron]",
                 LowPtElectron_r9);
    output->add("Events/LowPtElectron_scEtOverPt[nLowPtElectron]",
                 LowPtElectron_scEtOverPt);
    output->add("Events/LowPtElectron_sieie[nLowPtElectron]",
                 LowPtElectron_sieie);
    output->add("Events/LowPtElectron_unbiased[nLowPtElectron]",
                 LowPtElectron_unbiased);
    output->add("Events/MET_MetUnclustEnUpDeltaX",
                 MET_MetUnclustEnUpDeltaX);
    output->add("Events/MET_MetUnclustEnUpDeltaY",
                 MET_MetUnclustEnUpDeltaY);
    output->add("Events/MET_covXX", 	MET_covXX);
    output->add("Events/MET_covXY", 	MET_covXY);
    output->add("Events/MET_covYY", 	MET_covYY);
    output->add("Events/MET_fiducialGenPhi", 	MET_fiducialGenPhi);
    output->add("Events/MET_fiducialGenPt", 	MET_fiducialGenPt);
    output->add("Events/MET_phi", 	MET_phi);
    output->add("Events/MET_pt", 	MET_pt);
    output->add("Events/MET_significance", 	MET_significance);
    output->add("Events/MET_sumEt", 	MET_sumEt);
    output->add("Events/MET_sumPtUnclustered", 	MET_sumPtUnclustered);
    output->add("Events/Muon_charge[nMuon]", 	Muon_charge);
    output->add("Events/Muon_cleanmask[nMuon]", 	Muon_cleanmask);
    output->add("Events/Muon_dxy[nMuon]", 	Muon_dxy);
    output->add("Events/Muon_dxyErr[nMuon]", 	Muon_dxyErr);
    output->add("Events/Muon_dxybs[nMuon]", 	Muon_dxybs);
    output->add("Events/Muon_dz[nMuon]", 	Muon_dz);
    output->add("Events/Muon_dzErr[nMuon]", 	Muon_dzErr);
    output->add("Events/Muon_eta[nMuon]", 	Muon_eta);
    output->add("Events/Muon_fsrPhotonIdx[nMuon]", 	Muon_fsrPhotonIdx);
    output->add("Events/Muon_genPartFlav[nMuon]", 	Muon_genPartFlav);
    output->add("Events/Muon_genPartIdx[nMuon]", 	Muon_genPartIdx);
    output->add("Events/Muon_highPtId[nMuon]", 	Muon_highPtId);
    output->add("Events/Muon_highPurity[nMuon]", 	Muon_highPurity);
    output->add("Events/Muon_inTimeMuon[nMuon]", 	Muon_inTimeMuon);
    output->add("Events/Muon_ip3d[nMuon]", 	Muon_ip3d);
    output->add("Events/Muon_isGlobal[nMuon]", 	Muon_isGlobal);
    output->add("Events/Muon_isPFcand[nMuon]", 	Muon_isPFcand);
    output->add("Events/Muon_isStandalone[nMuon]", 	Muon_isStandalone);
    output->add("Events/Muon_isTracker[nMuon]", 	Muon_isTracker);
    output->add("Events/Muon_jetIdx[nMuon]", 	Muon_jetIdx);
    output->add("Events/Muon_jetNDauCharged[nMuon]", 	Muon_jetNDauCharged);
    output->add("Events/Muon_jetPtRelv2[nMuon]", 	Muon_jetPtRelv2);
    output->add("Events/Muon_jetRelIso[nMuon]", 	Muon_jetRelIso);
    output->add("Events/Muon_looseId[nMuon]", 	Muon_looseId);
    output->add("Events/Muon_mass[nMuon]", 	Muon_mass);
    output->add("Events/Muon_mediumId[nMuon]", 	Muon_mediumId);
    output->add("Events/Muon_mediumPromptId[nMuon]", 	Muon_mediumPromptId);
    output->add("Events/Muon_miniIsoId[nMuon]", 	Muon_miniIsoId);
    output->add("Events/Muon_miniPFRelIso_all[nMuon]",
                 Muon_miniPFRelIso_all);
    output->add("Events/Muon_miniPFRelIso_chg[nMuon]",
                 Muon_miniPFRelIso_chg);
    output->add("Events/Muon_multiIsoId[nMuon]", 	Muon_multiIsoId);
    output->add("Events/Muon_mvaId[nMuon]", 	Muon_mvaId);
    output->add("Events/Muon_mvaLowPt[nMuon]", 	Muon_mvaLowPt);
    output->add("Events/Muon_mvaLowPtId[nMuon]", 	Muon_mvaLowPtId);
    output->add("Events/Muon_mvaMuID[nMuon]", 	Muon_mvaMuID);
    output->add("Events/Muon_mvaMuID_WP[nMuon]", 	Muon_mvaMuID_WP);
    output->add("Events/Muon_mvaTTH[nMuon]", 	Muon_mvaTTH);
    output->add("Events/Muon_nStations[nMuon]", 	Muon_nStations);
    output->add("Events/Muon_nTrackerLayers[nMuon]", 	Muon_nTrackerLayers);
    output->add("Events/Muon_pdgId[nMuon]", 	Muon_pdgId);
    output->add("Events/Muon_pfIsoId[nMuon]", 	Muon_pfIsoId);
    output->add("Events/Muon_pfRelIso03_all[nMuon]", 	Muon_pfRelIso03_all);
    output->add("Events/Muon_pfRelIso03_chg[nMuon]", 	Muon_pfRelIso03_chg);
    output->add("Events/Muon_pfRelIso04_all[nMuon]", 	Muon_pfRelIso04_all);
    output->add("Events/Muon_phi[nMuon]", 	Muon_phi);
    output->add("Events/Muon_promptMVA[nMuon]", 	Muon_promptMVA);
    output->add("Events/Muon_pt[nMuon]", 	Muon_pt);
    output->add("Events/Muon_ptErr[nMuon]", 	Muon_ptErr);
    output->add("Events/Muon_puppiIsoId[nMuon]", 	Muon_puppiIsoId);
    output->add("Events/Muon_segmentComp[nMuon]", 	Muon_segmentComp);
    output->add("Events/Muon_sip3d[nMuon]", 	Muon_sip3d);
    output->add("Events/Muon_softId[nMuon]", 	Muon_softId);
    output->add("Events/Muon_softMva[nMuon]", 	Muon_softMva);
    output->add("Events/Muon_softMvaId[nMuon]", 	Muon_softMvaId);
    output->add("Events/Muon_tightCharge[nMuon]", 	Muon_tightCharge);
    output->add("Events/Muon_tightId[nMuon]", 	Muon_tightId);
    output->add("Events/Muon_tkIsoId[nMuon]", 	Muon_tkIsoId);
    output->add("Events/Muon_tkRelIso[nMuon]", 	Muon_tkRelIso);
    output->add("Events/Muon_triggerIdLoose[nMuon]", 	Muon_triggerIdLoose);
    output->add("Events/Muon_tunepRelPt[nMuon]", 	Muon_tunepRelPt);
    output->add("Events/OtherPV_z[nOtherPV]", 	OtherPV_z);
    output->add("Events/PFMET_covXX", 	PFMET_covXX);
    output->add("Events/PFMET_covXY", 	PFMET_covXY);
    output->add("Events/PFMET_covYY", 	PFMET_covYY);
    output->add("Events/PFMET_phi", 	PFMET_phi);
    output->add("Events/PFMET_phiUnclusteredDown",
                 PFMET_phiUnclusteredDown);
    output->add("Events/PFMET_phiUnclusteredUp", 	PFMET_phiUnclusteredUp);
    output->add("Events/PFMET_pt", 	PFMET_pt);
    output->add("Events/PFMET_ptUnclusteredDown", 	PFMET_ptUnclusteredDown);
    output->add("Events/PFMET_ptUnclusteredUp", 	PFMET_ptUnclusteredUp);
    output->add("Events/PFMET_significance", 	PFMET_significance);
    output->add("Events/PFMET_sumEt", 	PFMET_sumEt);
    output->add("Events/PFMET_sumPtUnclustered", 	PFMET_sumPtUnclustered);
    output->add("Events/PPSLocalTrack_decRPId[nPPSLocalTrack]",
                 PPSLocalTrack_decRPId);
    output->add("Events/PPSLocalTrack_multiRPProtonIdx[nPPSLocalTrack]",
                 PPSLocalTrack_multiRPProtonIdx);
    output->add("Events/PPSLocalTrack_rpType[nPPSLocalTrack]",
                 PPSLocalTrack_rpType);
    output->add("Events/PPSLocalTrack_singleRPProtonIdx[nPPSLocalTrack]",
                 PPSLocalTrack_singleRPProtonIdx);
    output->add("Events/PPSLocalTrack_time[nPPSLocalTrack]",
                 PPSLocalTrack_time);
    output->add("Events/PPSLocalTrack_timeUnc[nPPSLocalTrack]",
                 PPSLocalTrack_timeUnc);
    output->add("Events/PPSLocalTrack_x[nPPSLocalTrack]", 	PPSLocalTrack_x);
    output->add("Events/PPSLocalTrack_y[nPPSLocalTrack]", 	PPSLocalTrack_y);
    output->add("Events/PSWeight[nPSWeight]", 	PSWeight);
    output->add("Events/PV_chi2", 	PV_chi2);
    output->add("Events/PV_ndof", 	PV_ndof);
    output->add("Events/PV_npvs", 	PV_npvs);
    output->add("Events/PV_npvsGood", 	PV_npvsGood);
    output->add("Events/PV_score", 	PV_score);
    output->add("Events/PV_x", 	PV_x);
    output->add("Events/PV_y", 	PV_y);
    output->add("Events/PV_z", 	PV_z);
    output->add("Events/Photon_charge[nPhoton]", 	Photon_charge);
    output->add("Events/Photon_cleanmask[nPhoton]", 	Photon_cleanmask);
    output->add("Events/Photon_cutBased[nPhoton]", 	Photon_cutBased);
    output->add("Events/Photon_cutBased_Fall17V1Bitmap[nPhoton]",
                 Photon_cutBased_Fall17V1Bitmap);
    output->add("Events/Photon_dEscaleDown[nPhoton]", 	Photon_dEscaleDown);
    output->add("Events/Photon_dEscaleUp[nPhoton]", 	Photon_dEscaleUp);
    output->add("Events/Photon_dEsigmaDown[nPhoton]", 	Photon_dEsigmaDown);
    output->add("Events/Photon_dEsigmaUp[nPhoton]", 	Photon_dEsigmaUp);
    output->add("Events/Photon_eCorr[nPhoton]", 	Photon_eCorr);
    output->add("Events/Photon_electronIdx[nPhoton]", 	Photon_electronIdx);
    output->add("Events/Photon_electronVeto[nPhoton]",
                 Photon_electronVeto);
    output->add("Events/Photon_energyErr[nPhoton]", 	Photon_energyErr);
    output->add("Events/Photon_eta[nPhoton]", 	Photon_eta);
    output->add("Events/Photon_genPartFlav[nPhoton]", 	Photon_genPartFlav);
    output->add("Events/Photon_genPartIdx[nPhoton]", 	Photon_genPartIdx);
    output->add("Events/Photon_hoe[nPhoton]", 	Photon_hoe);
    output->add("Events/Photon_isScEtaEB[nPhoton]", 	Photon_isScEtaEB);
    output->add("Events/Photon_isScEtaEE[nPhoton]", 	Photon_isScEtaEE);
    output->add("Events/Photon_jetIdx[nPhoton]", 	Photon_jetIdx);
    output->add("Events/Photon_mass[nPhoton]", 	Photon_mass);
    output->add("Events/Photon_mvaID[nPhoton]", 	Photon_mvaID);
    output->add("Events/Photon_mvaID_Fall17V1p1[nPhoton]",
                 Photon_mvaID_Fall17V1p1);
    output->add("Events/Photon_mvaID_WP80[nPhoton]", 	Photon_mvaID_WP80);
    output->add("Events/Photon_mvaID_WP90[nPhoton]", 	Photon_mvaID_WP90);
    output->add("Events/Photon_pdgId[nPhoton]", 	Photon_pdgId);
    output->add("Events/Photon_pfRelIso03_all[nPhoton]",
                 Photon_pfRelIso03_all);
    output->add("Events/Photon_pfRelIso03_chg[nPhoton]",
                 Photon_pfRelIso03_chg);
    output->add("Events/Photon_phi[nPhoton]", 	Photon_phi);
    output->add("Events/Photon_pixelSeed[nPhoton]", 	Photon_pixelSeed);
    output->add("Events/Photon_pt[nPhoton]", 	Photon_pt);
    output->add("Events/Photon_r9[nPhoton]", 	Photon_r9);
    output->add("Events/Photon_seedGain[nPhoton]", 	Photon_seedGain);
    output->add("Events/Photon_sieie[nPhoton]", 	Photon_sieie);
    output->add("Events/Photon_vidNestedWPBitmap[nPhoton]",
                 Photon_vidNestedWPBitmap);
    output->add("Events/Pileup_gpudensity", 	Pileup_gpudensity);
    output->add("Events/Pileup_nPU", 	Pileup_nPU);
    output->add("Events/Pileup_nTrueInt", 	Pileup_nTrueInt);
    output->add("Events/Pileup_pudensity", 	Pileup_pudensity);
    output->add("Events/Pileup_sumEOOT", 	Pileup_sumEOOT);
    output->add("Events/Pileup_sumLOOT", 	Pileup_sumLOOT);
    output->add("Events/Proton_multiRP_arm[nProton_multiRP]",
                 Proton_multiRP_arm);
    output->add("Events/Proton_multiRP_t[nProton_multiRP]",
                 Proton_multiRP_t);
    output->add("Events/Proton_multiRP_thetaX[nProton_multiRP]",
                 Proton_multiRP_thetaX);
    output->add("Events/Proton_multiRP_thetaY[nProton_multiRP]",
                 Proton_multiRP_thetaY);
    output->add("Events/Proton_multiRP_time[nProton_multiRP]",
                 Proton_multiRP_time);
    output->add("Events/Proton_multiRP_timeUnc[nProton_multiRP]",
                 Proton_multiRP_timeUnc);
    output->add("Events/Proton_multiRP_xi[nProton_multiRP]",
                 Proton_multiRP_xi);
    output->add("Events/Proton_singleRP_decRPId[nProton_singleRP]",
                 Proton_singleRP_decRPId);
    output->add("Events/Proton_singleRP_thetaY[nProton_singleRP]",
                 Proton_singleRP_thetaY);
    output->add("Events/Proton_singleRP_xi[nProton_singleRP]",
                 Proton_singleRP_xi);
    output->add("Events/PuppiMET_covXX", 	PuppiMET_covXX);
    output->add("Events/PuppiMET_covXY", 	PuppiMET_covXY);
    output->add("Events/PuppiMET_covYY", 	PuppiMET_covYY);
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
    output->add("Events/PuppiMET_significance", 	PuppiMET_significance);
    output->add("Events/PuppiMET_sumEt", 	PuppiMET_sumEt);
    output->add("Events/PuppiMET_sumPtUnclustered",
                 PuppiMET_sumPtUnclustered);
    output->add("Events/RawMET_phi", 	RawMET_phi);
    output->add("Events/RawMET_pt", 	RawMET_pt);
    output->add("Events/RawMET_sumEt", 	RawMET_sumEt);
    output->add("Events/RawPFMET_phi", 	RawPFMET_phi);
    output->add("Events/RawPFMET_pt", 	RawPFMET_pt);
    output->add("Events/RawPFMET_sumEt", 	RawPFMET_sumEt);
    output->add("Events/RawPuppiMET_phi", 	RawPuppiMET_phi);
    output->add("Events/RawPuppiMET_pt", 	RawPuppiMET_pt);
    output->add("Events/RawPuppiMET_sumEt", 	RawPuppiMET_sumEt);
    output->add("Events/Rho_fixedGridRhoAll", 	Rho_fixedGridRhoAll);
    output->add("Events/Rho_fixedGridRhoFastjetAll",
                 Rho_fixedGridRhoFastjetAll);
    output->add("Events/Rho_fixedGridRhoFastjetCentral",
                 Rho_fixedGridRhoFastjetCentral);
    output->add("Events/Rho_fixedGridRhoFastjetCentralCalo",
                 Rho_fixedGridRhoFastjetCentralCalo);
    output->add("Events/Rho_fixedGridRhoFastjetCentralChargedPileUp",
                 Rho_fixedGridRhoFastjetCentralChargedPileUp);
    output->add("Events/Rho_fixedGridRhoFastjetCentralNeutral",
                 Rho_fixedGridRhoFastjetCentralNeutral);
    output->add("Events/SV_charge[nSV]", 	SV_charge);
    output->add("Events/SV_chi2[nSV]", 	SV_chi2);
    output->add("Events/SV_dlen[nSV]", 	SV_dlen);
    output->add("Events/SV_dlenSig[nSV]", 	SV_dlenSig);
    output->add("Events/SV_dxy[nSV]", 	SV_dxy);
    output->add("Events/SV_dxySig[nSV]", 	SV_dxySig);
    output->add("Events/SV_eta[nSV]", 	SV_eta);
    output->add("Events/SV_mass[nSV]", 	SV_mass);
    output->add("Events/SV_ndof[nSV]", 	SV_ndof);
    output->add("Events/SV_ntracks[nSV]", 	SV_ntracks);
    output->add("Events/SV_pAngle[nSV]", 	SV_pAngle);
    output->add("Events/SV_phi[nSV]", 	SV_phi);
    output->add("Events/SV_pt[nSV]", 	SV_pt);
    output->add("Events/SV_x[nSV]", 	SV_x);
    output->add("Events/SV_y[nSV]", 	SV_y);
    output->add("Events/SV_z[nSV]", 	SV_z);
    output->add("Events/SoftActivityJetHT", 	SoftActivityJetHT);
    output->add("Events/SoftActivityJetHT10", 	SoftActivityJetHT10);
    output->add("Events/SoftActivityJetHT2", 	SoftActivityJetHT2);
    output->add("Events/SoftActivityJetHT5", 	SoftActivityJetHT5);
    output->add("Events/SoftActivityJetNjets10", 	SoftActivityJetNjets10);
    output->add("Events/SoftActivityJetNjets2", 	SoftActivityJetNjets2);
    output->add("Events/SoftActivityJetNjets5", 	SoftActivityJetNjets5);
    output->add("Events/SoftActivityJet_eta[nSoftActivityJet]",
                 SoftActivityJet_eta);
    output->add("Events/SoftActivityJet_phi[nSoftActivityJet]",
                 SoftActivityJet_phi);
    output->add("Events/SoftActivityJet_pt[nSoftActivityJet]",
                 SoftActivityJet_pt);
    output->add("Events/SubGenJetAK8_eta[nSubGenJetAK8]",
                 SubGenJetAK8_eta);
    output->add("Events/SubGenJetAK8_mass[nSubGenJetAK8]",
                 SubGenJetAK8_mass);
    output->add("Events/SubGenJetAK8_phi[nSubGenJetAK8]",
                 SubGenJetAK8_phi);
    output->add("Events/SubGenJetAK8_pt[nSubGenJetAK8]", 	SubGenJetAK8_pt);
    output->add("Events/SubJet_btagCSVV2[nSubJet]", 	SubJet_btagCSVV2);
    output->add("Events/SubJet_btagDeepB[nSubJet]", 	SubJet_btagDeepB);
    output->add("Events/SubJet_eta[nSubJet]", 	SubJet_eta);
    output->add("Events/SubJet_hadronFlavour[nSubJet]",
                 SubJet_hadronFlavour);
    output->add("Events/SubJet_mass[nSubJet]", 	SubJet_mass);
    output->add("Events/SubJet_n2b1[nSubJet]", 	SubJet_n2b1);
    output->add("Events/SubJet_n3b1[nSubJet]", 	SubJet_n3b1);
    output->add("Events/SubJet_nBHadrons[nSubJet]", 	SubJet_nBHadrons);
    output->add("Events/SubJet_nCHadrons[nSubJet]", 	SubJet_nCHadrons);
    output->add("Events/SubJet_phi[nSubJet]", 	SubJet_phi);
    output->add("Events/SubJet_pt[nSubJet]", 	SubJet_pt);
    output->add("Events/SubJet_rawFactor[nSubJet]", 	SubJet_rawFactor);
    output->add("Events/SubJet_tau1[nSubJet]", 	SubJet_tau1);
    output->add("Events/SubJet_tau2[nSubJet]", 	SubJet_tau2);
    output->add("Events/SubJet_tau3[nSubJet]", 	SubJet_tau3);
    output->add("Events/SubJet_tau4[nSubJet]", 	SubJet_tau4);
    output->add("Events/Tau_charge[nTau]", 	Tau_charge);
    output->add("Events/Tau_chargedIso[nTau]", 	Tau_chargedIso);
    output->add("Events/Tau_cleanmask[nTau]", 	Tau_cleanmask);
    output->add("Events/Tau_decayMode[nTau]", 	Tau_decayMode);
    output->add("Events/Tau_dxy[nTau]", 	Tau_dxy);
    output->add("Events/Tau_dz[nTau]", 	Tau_dz);
    output->add("Events/Tau_eta[nTau]", 	Tau_eta);
    output->add("Events/Tau_genPartFlav[nTau]", 	Tau_genPartFlav);
    output->add("Events/Tau_genPartIdx[nTau]", 	Tau_genPartIdx);
    output->add("Events/Tau_idAntiEleDeadECal[nTau]",
                 Tau_idAntiEleDeadECal);
    output->add("Events/Tau_idAntiMu[nTau]", 	Tau_idAntiMu);
    output->add("Events/Tau_idDecayModeOldDMs[nTau]",
                 Tau_idDecayModeOldDMs);
    output->add("Events/Tau_idDeepTau2017v2p1VSe[nTau]",
                 Tau_idDeepTau2017v2p1VSe);
    output->add("Events/Tau_idDeepTau2017v2p1VSjet[nTau]",
                 Tau_idDeepTau2017v2p1VSjet);
    output->add("Events/Tau_idDeepTau2017v2p1VSmu[nTau]",
                 Tau_idDeepTau2017v2p1VSmu);
    output->add("Events/Tau_jetIdx[nTau]", 	Tau_jetIdx);
    output->add("Events/Tau_leadTkDeltaEta[nTau]", 	Tau_leadTkDeltaEta);
    output->add("Events/Tau_leadTkDeltaPhi[nTau]", 	Tau_leadTkDeltaPhi);
    output->add("Events/Tau_leadTkPtOverTauPt[nTau]",
                 Tau_leadTkPtOverTauPt);
    output->add("Events/Tau_mass[nTau]", 	Tau_mass);
    output->add("Events/Tau_neutralIso[nTau]", 	Tau_neutralIso);
    output->add("Events/Tau_phi[nTau]", 	Tau_phi);
    output->add("Events/Tau_photonsOutsideSignalCone[nTau]",
                 Tau_photonsOutsideSignalCone);
    output->add("Events/Tau_pt[nTau]", 	Tau_pt);
    output->add("Events/Tau_puCorr[nTau]", 	Tau_puCorr);
    output->add("Events/Tau_rawDeepTau2017v2p1VSe[nTau]",
                 Tau_rawDeepTau2017v2p1VSe);
    output->add("Events/Tau_rawDeepTau2017v2p1VSjet[nTau]",
                 Tau_rawDeepTau2017v2p1VSjet);
    output->add("Events/Tau_rawDeepTau2017v2p1VSmu[nTau]",
                 Tau_rawDeepTau2017v2p1VSmu);
    output->add("Events/Tau_rawIso[nTau]", 	Tau_rawIso);
    output->add("Events/Tau_rawIsodR03[nTau]", 	Tau_rawIsodR03);
    output->add("Events/TkMET_phi", 	TkMET_phi);
    output->add("Events/TkMET_pt", 	TkMET_pt);
    output->add("Events/TkMET_sumEt", 	TkMET_sumEt);
    output->add("Events/TrigObj_eta[nTrigObj]", 	TrigObj_eta);
    output->add("Events/TrigObj_filterBits[nTrigObj]", 	TrigObj_filterBits);
    output->add("Events/TrigObj_id[nTrigObj]", 	TrigObj_id);
    output->add("Events/TrigObj_l1charge[nTrigObj]", 	TrigObj_l1charge);
    output->add("Events/TrigObj_l1iso[nTrigObj]", 	TrigObj_l1iso);
    output->add("Events/TrigObj_l1pt[nTrigObj]", 	TrigObj_l1pt);
    output->add("Events/TrigObj_l1pt_2[nTrigObj]", 	TrigObj_l1pt_2);
    output->add("Events/TrigObj_l2pt[nTrigObj]", 	TrigObj_l2pt);
    output->add("Events/TrigObj_phi[nTrigObj]", 	TrigObj_phi);
    output->add("Events/TrigObj_pt[nTrigObj]", 	TrigObj_pt);
    output->add("Events/boostedTau_charge[nboostedTau]",
                 boostedTau_charge);
    output->add("Events/boostedTau_chargedIso[nboostedTau]",
                 boostedTau_chargedIso);
    output->add("Events/boostedTau_decayMode[nboostedTau]",
                 boostedTau_decayMode);
    output->add("Events/boostedTau_eta[nboostedTau]", 	boostedTau_eta);
    output->add("Events/boostedTau_genPartFlav[nboostedTau]",
                 boostedTau_genPartFlav);
    output->add("Events/boostedTau_genPartIdx[nboostedTau]",
                 boostedTau_genPartIdx);
    output->add("Events/boostedTau_idAntiEle2018[nboostedTau]",
                 boostedTau_idAntiEle2018);
    output->add("Events/boostedTau_idAntiMu[nboostedTau]",
                 boostedTau_idAntiMu);
    output->add("Events/boostedTau_idMVAnewDM2017v2[nboostedTau]",
                 boostedTau_idMVAnewDM2017v2);
    output->add("Events/boostedTau_idMVAoldDM2017v2[nboostedTau]",
                 boostedTau_idMVAoldDM2017v2);
    output->add("Events/boostedTau_idMVAoldDMdR032017v2[nboostedTau]",
                 boostedTau_idMVAoldDMdR032017v2);
    output->add("Events/boostedTau_jetIdx[nboostedTau]",
                 boostedTau_jetIdx);
    output->add("Events/boostedTau_leadTkDeltaEta[nboostedTau]",
                 boostedTau_leadTkDeltaEta);
    output->add("Events/boostedTau_leadTkDeltaPhi[nboostedTau]",
                 boostedTau_leadTkDeltaPhi);
    output->add("Events/boostedTau_leadTkPtOverTauPt[nboostedTau]",
                 boostedTau_leadTkPtOverTauPt);
    output->add("Events/boostedTau_mass[nboostedTau]", 	boostedTau_mass);
    output->add("Events/boostedTau_neutralIso[nboostedTau]",
                 boostedTau_neutralIso);
    output->add("Events/boostedTau_phi[nboostedTau]", 	boostedTau_phi);
    output->add("Events/boostedTau_photonsOutsideSignalCone[nboostedTau]",
                 boostedTau_photonsOutsideSignalCone);
    output->add("Events/boostedTau_pt[nboostedTau]", 	boostedTau_pt);
    output->add("Events/boostedTau_puCorr[nboostedTau]",
                 boostedTau_puCorr);
    output->add("Events/boostedTau_rawAntiEle2018[nboostedTau]",
                 boostedTau_rawAntiEle2018);
    output->add("Events/boostedTau_rawAntiEleCat2018[nboostedTau]",
                 boostedTau_rawAntiEleCat2018);
    output->add("Events/boostedTau_rawIso[nboostedTau]",
                 boostedTau_rawIso);
    output->add("Events/boostedTau_rawIsodR03[nboostedTau]",
                 boostedTau_rawIsodR03);
    output->add("Events/boostedTau_rawMVAnewDM2017v2[nboostedTau]",
                 boostedTau_rawMVAnewDM2017v2);
    output->add("Events/boostedTau_rawMVAoldDM2017v2[nboostedTau]",
                 boostedTau_rawMVAoldDM2017v2);
    output->add("Events/boostedTau_rawMVAoldDMdR032017v2[nboostedTau]",
                 boostedTau_rawMVAoldDMdR032017v2);
    output->add("Events/btagWeight_CSVV2", 	btagWeight_CSVV2);
    output->add("Events/btagWeight_DeepCSVB", 	btagWeight_DeepCSVB);
    output->add("Events/event", 	event);
    output->add("Events/fixedGridRhoFastjetAll", 	fixedGridRhoFastjetAll);
    output->add("Events/fixedGridRhoFastjetCentral",
                 fixedGridRhoFastjetCentral);
    output->add("Events/fixedGridRhoFastjetCentralCalo",
                 fixedGridRhoFastjetCentralCalo);
    output->add("Events/fixedGridRhoFastjetCentralChargedPileUp",
                 fixedGridRhoFastjetCentralChargedPileUp);
    output->add("Events/fixedGridRhoFastjetCentralNeutral",
                 fixedGridRhoFastjetCentralNeutral);
    output->add("Events/genTtbarId", 	genTtbarId);
    output->add("Events/genWeight", 	genWeight);
    output->add("Events/luminosityBlock", 	luminosityBlock);
    output->add("Events/run", 	run);

  }

  void initBuffers()
  {
    nCorrT1METJet	= 0;
    nElectron	= 0;
    nFatJet	= 0;
    nFsrPhoton	= 0;
    nGenDressedLepton	= 0;
    nGenIsolatedPhoton	= 0;
    nGenJet	= 0;
    nGenJetAK8	= 0;
    nGenPart	= 0;
    nGenVisTau	= 0;
    nIsoTrack	= 0;
    nJet	= 0;
    nLHEPart	= 0;
    nLHEPdfWeight	= 0;
    nLHEReweightingWeight	= 0;
    nLHEScaleWeight	= 0;
    nLowPtElectron	= 0;
    nMuon	= 0;
    nOtherPV	= 0;
    nPPSLocalTrack	= 0;
    nPSWeight	= 0;
    nPhoton	= 0;
    nProton_multiRP	= 0;
    nProton_singleRP	= 0;
    nSV	= 0;
    nSoftActivityJet	= 0;
    nSubGenJetAK8	= 0;
    nSubJet	= 0;
    nTau	= 0;
    nTrigObj	= 0;
    nboostedTau	= 0;
    CaloMET_phi	= 0;
    CaloMET_pt	= 0;
    CaloMET_sumEt	= 0;
    ChsMET_phi	= 0;
    ChsMET_pt	= 0;
    ChsMET_sumEt	= 0;
    DeepMETResolutionTune_phi	= 0;
    DeepMETResolutionTune_pt	= 0;
    DeepMETResponseTune_phi	= 0;
    DeepMETResponseTune_pt	= 0;
    Flag_BadChargedCandidateFilter	= 0;
    Flag_BadChargedCandidateFilter_pRECO	= 0;
    Flag_BadChargedCandidateSummer16Filter	= 0;
    Flag_BadChargedCandidateSummer16Filter_pRECO	= 0;
    Flag_BadPFMuonDzFilter	= 0;
    Flag_BadPFMuonDzFilter_pRECO	= 0;
    Flag_BadPFMuonFilter	= 0;
    Flag_BadPFMuonFilter_pRECO	= 0;
    Flag_BadPFMuonSummer16Filter	= 0;
    Flag_BadPFMuonSummer16Filter_pRECO	= 0;
    Flag_CSCTightHalo2015Filter	= 0;
    Flag_CSCTightHalo2015Filter_pRECO	= 0;
    Flag_CSCTightHaloFilter	= 0;
    Flag_CSCTightHaloFilter_pRECO	= 0;
    Flag_CSCTightHaloTrkMuUnvetoFilter	= 0;
    Flag_CSCTightHaloTrkMuUnvetoFilter_pRECO	= 0;
    Flag_EcalDeadCellBoundaryEnergyFilter	= 0;
    Flag_EcalDeadCellBoundaryEnergyFilter_pRECO	= 0;
    Flag_EcalDeadCellTriggerPrimitiveFilter	= 0;
    Flag_EcalDeadCellTriggerPrimitiveFilter_pRECO	= 0;
    Flag_HBHENoiseFilter	= 0;
    Flag_HBHENoiseFilter_pRECO	= 0;
    Flag_HBHENoiseIsoFilter	= 0;
    Flag_HBHENoiseIsoFilter_pRECO	= 0;
    Flag_HcalStripHaloFilter	= 0;
    Flag_HcalStripHaloFilter_pRECO	= 0;
    Flag_METFilters	= 0;
    Flag_METFilters_pRECO	= 0;
    Flag_chargedHadronTrackResolutionFilter	= 0;
    Flag_chargedHadronTrackResolutionFilter_pRECO	= 0;
    Flag_ecalBadCalibFilter	= 0;
    Flag_ecalBadCalibFilter_pRECO	= 0;
    Flag_ecalLaserCorrFilter	= 0;
    Flag_ecalLaserCorrFilter_pRECO	= 0;
    Flag_eeBadScFilter	= 0;
    Flag_eeBadScFilter_pRECO	= 0;
    Flag_globalSuperTightHalo2016Filter	= 0;
    Flag_globalSuperTightHalo2016Filter_pRECO	= 0;
    Flag_globalTightHalo2016Filter	= 0;
    Flag_globalTightHalo2016Filter_pRECO	= 0;
    Flag_goodVertices	= 0;
    Flag_goodVertices_pRECO	= 0;
    Flag_hcalLaserEventFilter	= 0;
    Flag_hcalLaserEventFilter_pRECO	= 0;
    Flag_hfNoisyHitsFilter	= 0;
    Flag_hfNoisyHitsFilter_pRECO	= 0;
    Flag_muonBadTrackFilter	= 0;
    Flag_muonBadTrackFilter_pRECO	= 0;
    Flag_trkPOGFilters	= 0;
    Flag_trkPOGFilters_pRECO	= 0;
    Flag_trkPOG_logErrorTooManyClusters	= 0;
    Flag_trkPOG_logErrorTooManyClusters_pRECO	= 0;
    Flag_trkPOG_manystripclus53X	= 0;
    Flag_trkPOG_manystripclus53X_pRECO	= 0;
    Flag_trkPOG_toomanystripclus53X	= 0;
    Flag_trkPOG_toomanystripclus53X_pRECO	= 0;
    GenMET_phi	= 0;
    GenMET_pt	= 0;
    GenVtx_t0	= 0;
    GenVtx_x	= 0;
    GenVtx_y	= 0;
    GenVtx_z	= 0;
    Generator_binvar	= 0;
    Generator_id1	= 0;
    Generator_id2	= 0;
    Generator_scalePDF	= 0;
    Generator_weight	= 0;
    Generator_x1	= 0;
    Generator_x2	= 0;
    Generator_xpdf1	= 0;
    Generator_xpdf2	= 0;
    HLT_Ele30_WPTight_Gsf	= 0;
    HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07	= 0;
    HLT_IsoMu24	= 0;
    HLT_IsoMu27	= 0;
    HLT_PFHT1050	= 0;
    HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p55	= 0;
    HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p60	= 0;
    HLT_PFHT280_QuadPFJet35_PNet2BTagMean0p60	= 0;
    HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0	= 0;
    HLT_PFHT330PT30_QuadPFJet_75_60_45_40	= 0;
    HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_2p0	= 0;
    HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3	= 0;
    HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5	= 0;
    HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5	= 0;
    HLT_PFHT340_QuadPFJet70_50_40_40_PNet2BTagMean0p70	= 0;
    HLT_PFHT380_SixJet32_DoubleBTagCSV_p075	= 0;
    HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2	= 0;
    HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_4p3	= 0;
    HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_5p6	= 0;
    HLT_PFHT400_SixPFJet32	= 0;
    HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94	= 0;
    HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50	= 0;
    HLT_PFHT430_SixJet40_BTagCSV_p080	= 0;
    HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5	= 0;
    HLT_PFHT450_SixPFJet36	= 0;
    HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59	= 0;
    HLT_PFHT450_SixPFJet36_PNetBTag0p35	= 0;
    HLTriggerFinalPath	= 0;
    HLTriggerFirstPath	= 0;
    HTXS_Higgs_pt	= 0;
    HTXS_Higgs_y	= 0;
    HTXS_njets25	= 0;
    HTXS_njets30	= 0;
    HTXS_stage1_1_cat_pTjet25GeV	= 0;
    HTXS_stage1_1_cat_pTjet30GeV	= 0;
    HTXS_stage1_1_fine_cat_pTjet25GeV	= 0;
    HTXS_stage1_1_fine_cat_pTjet30GeV	= 0;
    HTXS_stage1_2_cat_pTjet25GeV	= 0;
    HTXS_stage1_2_cat_pTjet30GeV	= 0;
    HTXS_stage1_2_fine_cat_pTjet25GeV	= 0;
    HTXS_stage1_2_fine_cat_pTjet30GeV	= 0;
    HTXS_stage_0	= 0;
    HTXS_stage_1_pTjet25	= 0;
    HTXS_stage_1_pTjet30	= 0;
    L1PreFiringWeight_Dn	= 0;
    L1PreFiringWeight_ECAL_Dn	= 0;
    L1PreFiringWeight_ECAL_Nom	= 0;
    L1PreFiringWeight_ECAL_Up	= 0;
    L1PreFiringWeight_Muon_Nom	= 0;
    L1PreFiringWeight_Muon_StatDn	= 0;
    L1PreFiringWeight_Muon_StatUp	= 0;
    L1PreFiringWeight_Muon_SystDn	= 0;
    L1PreFiringWeight_Muon_SystUp	= 0;
    L1PreFiringWeight_Nom	= 0;
    L1PreFiringWeight_Up	= 0;
    L1Reco_step	= 0;
    L1simulation_step	= 0;
    LHEWeight_originalXWGTUP	= 0;
    LHE_AlphaS	= 0;
    LHE_HT	= 0;
    LHE_HTIncoming	= 0;
    LHE_Nb	= 0;
    LHE_Nc	= 0;
    LHE_Nglu	= 0;
    LHE_Njets	= 0;
    LHE_NpLO	= 0;
    LHE_NpNLO	= 0;
    LHE_Nuds	= 0;
    LHE_Vpt	= 0;
    MET_MetUnclustEnUpDeltaX	= 0;
    MET_MetUnclustEnUpDeltaY	= 0;
    MET_covXX	= 0;
    MET_covXY	= 0;
    MET_covYY	= 0;
    MET_fiducialGenPhi	= 0;
    MET_fiducialGenPt	= 0;
    MET_phi	= 0;
    MET_pt	= 0;
    MET_significance	= 0;
    MET_sumEt	= 0;
    MET_sumPtUnclustered	= 0;
    PFMET_covXX	= 0;
    PFMET_covXY	= 0;
    PFMET_covYY	= 0;
    PFMET_phi	= 0;
    PFMET_phiUnclusteredDown	= 0;
    PFMET_phiUnclusteredUp	= 0;
    PFMET_pt	= 0;
    PFMET_ptUnclusteredDown	= 0;
    PFMET_ptUnclusteredUp	= 0;
    PFMET_significance	= 0;
    PFMET_sumEt	= 0;
    PFMET_sumPtUnclustered	= 0;
    PV_chi2	= 0;
    PV_ndof	= 0;
    PV_npvs	= 0;
    PV_npvsGood	= 0;
    PV_score	= 0;
    PV_x	= 0;
    PV_y	= 0;
    PV_z	= 0;
    Pileup_gpudensity	= 0;
    Pileup_nPU	= 0;
    Pileup_nTrueInt	= 0;
    Pileup_pudensity	= 0;
    Pileup_sumEOOT	= 0;
    Pileup_sumLOOT	= 0;
    PuppiMET_covXX	= 0;
    PuppiMET_covXY	= 0;
    PuppiMET_covYY	= 0;
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
    PuppiMET_significance	= 0;
    PuppiMET_sumEt	= 0;
    PuppiMET_sumPtUnclustered	= 0;
    RawMET_phi	= 0;
    RawMET_pt	= 0;
    RawMET_sumEt	= 0;
    RawPFMET_phi	= 0;
    RawPFMET_pt	= 0;
    RawPFMET_sumEt	= 0;
    RawPuppiMET_phi	= 0;
    RawPuppiMET_pt	= 0;
    RawPuppiMET_sumEt	= 0;
    Rho_fixedGridRhoAll	= 0;
    Rho_fixedGridRhoFastjetAll	= 0;
    Rho_fixedGridRhoFastjetCentral	= 0;
    Rho_fixedGridRhoFastjetCentralCalo	= 0;
    Rho_fixedGridRhoFastjetCentralChargedPileUp	= 0;
    Rho_fixedGridRhoFastjetCentralNeutral	= 0;
    SoftActivityJetHT	= 0;
    SoftActivityJetHT10	= 0;
    SoftActivityJetHT2	= 0;
    SoftActivityJetHT5	= 0;
    SoftActivityJetNjets10	= 0;
    SoftActivityJetNjets2	= 0;
    SoftActivityJetNjets5	= 0;
    TkMET_phi	= 0;
    TkMET_pt	= 0;
    TkMET_sumEt	= 0;
    btagWeight_CSVV2	= 0;
    btagWeight_DeepCSVB	= 0;
    event	= 0;
    fixedGridRhoFastjetAll	= 0;
    fixedGridRhoFastjetCentral	= 0;
    fixedGridRhoFastjetCentralCalo	= 0;
    fixedGridRhoFastjetCentralChargedPileUp	= 0;
    fixedGridRhoFastjetCentralNeutral	= 0;
    genTtbarId	= 0;
    genWeight	= 0;
    luminosityBlock	= 0;
    run	= 0;
    CorrT1METJet.clear();	CorrT1METJet.reserve(83);
    Electron.clear();	Electron.reserve(49);
    FatJet.clear();	FatJet.reserve(43);
    FsrPhoton.clear();	FsrPhoton.reserve(31);
    GenDressedLepton.clear();	GenDressedLepton.reserve(28);
    GenIsolatedPhoton.clear();	GenIsolatedPhoton.reserve(28);
    GenJet.clear();	GenJet.reserve(81);
    GenJetAK8.clear();	GenJetAK8.reserve(46);
    GenPart.clear();	GenPart.reserve(295);
    GenVisTau.clear();	GenVisTau.reserve(28);
    IsoTrack.clear();	IsoTrack.reserve(66);
    Jet.clear();	Jet.reserve(128);
    LHEPart.clear();	LHEPart.reserve(49);
    LowPtElectron.clear();	LowPtElectron.reserve(46);
    Muon.clear();	Muon.reserve(65);
    PPSLocalTrack.clear();	PPSLocalTrack.reserve(65);
    Photon.clear();	Photon.reserve(50);
    Proton.clear();	Proton.reserve(28);
    SV.clear();	SV.reserve(74);
    SoftActivityJet.clear();	SoftActivityJet.reserve(40);
    SubGenJetAK8.clear();	SubGenJetAK8.reserve(64);
    SubJet.clear();	SubJet.reserve(54);
    Tau.clear();	Tau.reserve(40);
    TrigObj.clear();	TrigObj.reserve(118);
    boostedTau.clear();	boostedTau.reserve(37);

  }
      
  void read(int entry)
  {
    if ( !input ) 
      { 
        std::cout << "** eventBuffer::read - first  call read-only constructor!"
                  << std::endl;
        assert(0);
      }
    // a negative value: the entry could not be loaded (e.g. a file of the
    // chain cannot be opened); the buffers would keep the previous values
    if ( input->read(entry) < 0 )
      {
        std::cout << "** eventBuffer::read - cannot load entry " << entry
                  << "; stopping" << std::endl;
        exit(1);
      }

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

 // filled by the read-only constructor: branches bound / absent in the
 // FIRST file of the stream (presence is decided once, from that file)
 std::vector<std::string> successBranches;
 std::vector<std::string> missingBranches;

}; 
#endif
