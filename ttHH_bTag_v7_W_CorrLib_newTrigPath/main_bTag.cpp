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
    if (argc < 1) {
        std::cerr << "Usage: ./exe_TrigStudy [SampleName]" << std::endl;
        return 1;
    }

    TString sampleName = argv[1];
    bool useEfficiencyDrawing = false;
    bool Debug = false;
    int debug_nofEvt = 1000;

//    makeBTagWeight* looper = new makeBTagWeight();
//    makeBTagWeight_reweight* looper = new makeBTagWeight_reweight();
    makeBTagWeight_Final* looper = new makeBTagWeight_Final(); // b-jet and lepton veto

    looper->setNtupleName(sampleName);
    looper->setDebug(Debug, debug_nofEvt);
    looper->Init();
    looper->Loop();
    delete looper;

    return 0;
}
