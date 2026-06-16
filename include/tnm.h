#ifndef TNM_H
#define TNM_H

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

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH1F.h"
#include "TMath.h"
#include "TString.h"
#include "TStyle.h"
#include "TApplication.h"

#include "eventBuffer.h"

struct outputFile
{
  outputFile(std::string filename);
  outputFile(std::string filename, eventBuffer& ev, int savecount=50000); 
  void write(double weight=1);
  void count(std::string cond, double w=1);
  void close();

  std::string filename_;  
  TFile* file;
  TTree* tree;
  TH1F*  hist_;
  TBranch* b_weight_;
  double     weight_;
  int    entry_;
  int    SAVECOUNT_;
  eventBuffer* ev_;
};

struct commandLine
{
    commandLine();
    commandLine(int argc, char** argv);
    ~commandLine() {}
    std::string progname;
    std::string filelist;
    std::string outputfilename;
    double externalweight; //Gamze
    std::string runYear; //Gamze  
    //    int runYear; //Gamze  
    std::string DataOrMC; //Gamze
    std::string sampleName; //Gamze
    std::string eraName; //Gamze
    std::string analysisMode;  // NEW: Add this member
    std::string region;        // [lepton-CR] "", "muon", "electron" (QCD 억제 1ℓ+MET CR)
    // [SF toggle] production evtWeight 에 각 SF 를 곱할지. "on"/"off" (기본 아래 참조).
    //   trigsf : trigger SF        (기본 on)
    //   btagsf : b-tag shape SF    (기본 off — 1차 stack 은 trigSF 만)
    //   btagrw : b-tag norm reweight (기본 off — 8-group JSON 준비 전)
    // tree 의 evtWeight_btagSF / evtWeight_full tier 는 토글과 무관하게 항상 기록.
    std::string sfTrig;        // "", "on", "off"
    std::string sfBtag;        // "", "on", "off"
    std::string sfBtagRw;      // "", "on", "off"

    // ── [Validation Study] optional fields ─────────────────────────────
    // [STEP8] --val-* 필드 제거 — kValidationStudy 모드 삭제(STEP 2)의 잔재 정리.


    
  void decode(int argc, char** argv);
};

///
struct matchedPair
{
  int first;
  int second;
  double distance;
  bool operator<(const matchedPair& o) const 
  { return this->distance < o.distance; }
};

/// Collect together standard attributes and permit pT-sorting.
struct ptThing
{
  ptThing();
  ptThing(int index_, int id_,
          double pt_, double eta_, double phi_, std::string name_="");
  ~ptThing();

  /// Copy constructor.
  ptThing(const ptThing& rhs);

  /// Assignment. 
  ptThing& operator=(const ptThing& rhs);

  /** Find $|Delta R = \sqrt{\Delta\phi^2+\Delta\eta^2}$ between this
      PtThing and the given.
  */
  double deltaR(ptThing& thing);

  /// Compare direction of this PtThing with another using deltaR.
  bool matches(ptThing& thing, double drcut=0.4);

  int index;
  int id;
  double pt;
  double eta;
  double phi;
  std::string name;

  /// Map for additional variables.
  std::map<std::string, double> var;

  /// To sort PtThings in descending pt.
  bool operator<(const ptThing& o) const { return o.pt < this->pt; }
};


void error(std::string message);
std::string strip(std::string line);
std::vector<std::string> split(std::string str);
std::string change(std::string str,
		   std::string oldstr,
		   std::string newstr);
std::string nameonly(std::string filename);
std::string shell(std::string cmd);

/// Read ntuple filenames from file list
std::vector<std::string> fileNames(std::string filelist);

double deltaPhi(double phi1, double phi2);

///
double deltaR(double eta1, double phi1, double eta2, double phi2);
///
//std::vector<matchedPair> deltaR(std::vector<ptThing>& v1, 
//				std::vector<ptThing>& v2);
///
void setStyle();

///
std::string particleName(int pdgid);

#endif

