#include <iostream>
#include <TString.h>
#include <TTree.h>

#include "EventLooper.hh"
#include "BinConfig.hh"

int main(int argc, char** argv)
{
    // ------------------------------------------------------------------------
    // 1) Load configuration FIRST
    //
    // BinConfig is used later for histogram binning and label generation.
    // If parsing fails or bins are wrong, you want to know it immediately.
    // ------------------------------------------------------------------------
    BinConfig::Load("config.yaml");

    // Optional: hard-fail if config is suspicious
    // if (!BinConfig::Validate()) return 1;

    // ------------------------------------------------------------------------
    // 2) Parse command-line arguments
    //
    // Usage:
    //   ./exe_TrigStudy [SampleName] [Mode]
    //
    // Mode:
    //   0 -> Step 1: calculate efficiency maps (denominator/numerator)
    //   1 -> Step 2: apply SF from JSON (correctionlib)
    // ------------------------------------------------------------------------
    if (argc < 2) {
        std::cerr << "Usage: ./exe_TrigStudy [SampleName] [Mode]\n";
        std::cerr << "  - Mode: 0 = Calc Eff (Step1), 1 = Apply SF (Step2)\n";
        return 1;
    }

    TString sampleName = argv[1];

    // Arg 2: Mode (Default 0)
    bool applySF = false;
    if (argc >= 3) {
        applySF = (std::stoi(argv[2]) != 0);
    }

    if (applySF) std::cout << ">>> [Main] Mode: Step 2 (Apply Scale Factors)\n";
    else         std::cout << ">>> [Main] Mode: Step 1 (Calculate Efficiency)\n";

    // ------------------------------------------------------------------------
    // 3) Create EventLooper and run
    //
    // The looper reads the input ROOT file, loops over events, applies
    // selections/triggers, and fills histograms/maps depending on the mode.
    // ------------------------------------------------------------------------
    EventLooper* looper = new EventLooper(applySF);
    looper->setNtupleName(sampleName);
    looper->Init();
    looper->Loop();
    delete looper;

    return 0;
}

