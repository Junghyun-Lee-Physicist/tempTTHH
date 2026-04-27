// ============================================================================
// run_BTagSF.cpp
//
// Driver program for b-tag shape SF reweighting.
// Same pattern as TriggerSFCalculator.cpp for the trigger SF pipeline.
//
// Usage:
//   ./exe_BTagSF <sampleName> <mode>
//
//   sampleName : ROOT file name without extension (e.g., TTTo2L2Nu, JetHT_B)
//   mode       : 0 = Compute normalization ratios (first pass)
//                1 = Apply b-tag SF + normalization (second pass)
//
// Example:
//   ./exe_BTagSF TTTo2L2Nu 0       # Step 1: normalization
//   ./exe_BTagSF TTTo2L2Nu 1       # Step 2: reweight + validation
//
// Build (example):
//   g++ -O2 -std=c++17 \
//       -I$(shell correction config --incdir) \
//       run_BTagSF.cpp BTagSFProcessor.cpp NtupleReader.cc \
//       $(shell root-config --cflags --libs) \
//       $(shell correction config --ldflags) \
//       -o exe_BTagSF
//
// Author: Junghyun Lee
// ============================================================================

#include "BTagSFProcessor.hh"
#include <iostream>
#include <cstdlib>
#include <string>

int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cerr << "\n  Usage: " << argv[0] << " <sampleName> <mode>\n\n"
                  << "  sampleName : e.g., TTTo2L2Nu, JetHT_B\n"
                  << "  mode       : 0 = normalization, 1 = reweight\n\n";
        return 1;
    }

    const std::string sampleName = argv[1];
    const int mode = std::atoi(argv[2]);

    if (mode != 0 && mode != 1) {
        std::cerr << "[ERROR] mode must be 0 or 1, got: " << mode << std::endl;
        return 1;
    }

    std::cout << "\n"
              << "════════════════════════════════════════════════════════════\n"
              << "  B-tag Shape SF Processor\n"
              << "  Sample : " << sampleName << "\n"
              << "  Mode   : " << mode
              << (mode == 0 ? " (Normalization)" : " (Reweight)") << "\n"
              << "════════════════════════════════════════════════════════════\n"
              << std::endl;

    BTagSFProcessor processor(mode);
    processor.setNtupleName(sampleName.c_str());
    processor.Init();
    processor.Loop();

    std::cout << "\n  Done.\n" << std::endl;
    return 0;
}
