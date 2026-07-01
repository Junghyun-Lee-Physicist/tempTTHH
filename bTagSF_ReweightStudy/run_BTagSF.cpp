#include <iostream>
#include <TString.h>

#include "BTagSFProcessor.hh"

// ============================================================================
// run_BTagSF.cpp
//
// Entry point for b-tag shape SF reweighting.
//
// Usage:
//   ./exe_BTagSF <SampleName>
//
// Examples:
//   ./exe_BTagSF TTbar_Hadronic        # MC sample
//   ./exe_BTagSF JetHT_B             # Data sample
//   ./exe_BTagSF QCD_HT500to700      # QCD MC sample
//
// Output:
//   bTagReweight_<SampleName>.root
//
// Author: Junghyun Lee
// ============================================================================

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: ./exe_BTagSF <SampleName>\n\n"
                  << "  SampleName : e.g. TTbar_Hadronic, JetHT_B, QCD_HT500to700\n"
                  << "  Output     : bTagReweight_<SampleName>.root\n";
        return 1;
    }

    TString sampleName = argv[1];
    std::cout << ">>> [main] B-Tag Reweight: " << sampleName << "\n\n";

    auto* processor = new BTagSFProcessor();
    processor->setNtupleName(sampleName);
    processor->Init();
    processor->Loop();
    delete processor;

    return 0;
}
