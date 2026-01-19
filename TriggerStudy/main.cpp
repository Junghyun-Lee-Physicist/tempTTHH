#include <iostream>
#include <TString.h>
#include <TTree.h>

#include "EventLooper.hh"
#include "EventLooperWithCorrections.hh"

int main(int argc, char** argv)
{

    if (argc < 2) {
        std::cerr << "Usage: ./exe_TrigStudy [SampleName] [Mode (0=CalcEff, 1=ApplySF)]" << std::endl;
        return 1;
    }

    TString sampleName = argv[1];
    
    // Default mode: 0 (Efficiency Calculation)
    bool applySF = false;
    if (argc >= 3) {
       applySF = (std::stoi(argv[2]) != 0);
    }

    if (applySF) {
        std::cout << ">>> Running Step 02: Apply Scale Factors & Correction" << std::endl;
    } else {
        std::cout << ">>> Running Step 01: Calculate Trigger Efficiency" << std::endl;
    }

    // Unified class usage
    EventLooper* looper = new EventLooper(0, applySF);
    looper->setNtupleName(sampleName);
    looper->Init();
    looper->Loop();
    delete looper;


    return 0;
}

