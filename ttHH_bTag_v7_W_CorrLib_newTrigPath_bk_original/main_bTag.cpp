#include <iostream>
#include <TString.h>
#include <TTree.h>
#include <vector>
#include <cmath>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

#include "makeBTagWeight.hh"
#include "makeBTagWeight_reweight.hh"
#include "makeBTagWeight_Final.hh"

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: ./exe_TrigStudy [SampleName]" << std::endl;
        return 1;
    }

    TString sampleName = argv[1];
    TString mode = argv[2];
    bool useEfficiencyDrawing = false;
    bool Debug = false;
    int debug_nofEvt = 1000;

    if(mode=="Step01_Calc_ReWgt");
    else if(mode=="Step02_Apply_ReWgt");
    else if(mode=="Step03_Validation");
    else {
	std::cout<<"[ERROR] : Unknown mode!! please check the mode argument"<<std::endl;
	exit(2);
    }
    std::cout<<"[main] Current mode --> "<<mode<<std::endl;
 
    makeBTagWeight* looper = new makeBTagWeight();
//    makeBTagWeight_reweight* looper = new makeBTagWeight_reweight();
//    makeBTagWeight_Final* looper = new makeBTagWeight_Final(); // b-jet and lepton veto

    looper->setMode(mode);

    looper->setNtupleName(sampleName);
    looper->setDebug(Debug, debug_nofEvt);
    looper->Init();
    looper->Loop();
    delete looper;

    return 0;
}
