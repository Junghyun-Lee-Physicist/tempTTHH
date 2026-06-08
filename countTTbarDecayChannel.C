// countTTbarDecayChannel.C  —  tt4b decay-channel census (statusFlags = Int_t build)
#include <TFile.h>
#include <TChain.h>
#include <TTreeReader.h>
#include <TTreeReaderArray.h>
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>

void countTTbarDecayChannel(const char* input, bool isList=false, Long64_t maxEvt=-1) {
  TChain ch("Events");
  if (isList) {
    std::ifstream f(input); std::string line; int n=0;
    while (std::getline(f, line)) {
      if (line.empty() || line[0]=='#') continue;
      ch.Add(line.c_str()); ++n;
    }
    std::cout << "[chan] added " << n << " files from list " << input << std::endl;
  } else {
    ch.Add(input);
    std::cout << "[chan] added single file " << input << std::endl;
  }

  TTreeReader r(&ch);
  TTreeReaderArray<Int_t> pdg(r, "GenPart_pdgId");
  TTreeReaderArray<Int_t> mom(r, "GenPart_genPartIdxMother");
  TTreeReaderArray<Int_t> flg(r, "GenPart_statusFlags");   // <-- Int_t in this ntuple

  Long64_t nEvt=0, byNlep[4]={0,0,0,0};
  Long64_t nFullHad=0, nSemi=0, nDilep=0, nNoPromptLep=0;

  while (r.Next()) {
    if (maxEvt>0 && nEvt>=maxEvt) break;
    ++nEvt;
    int nWl=0, nPromptEmu=0;
    const int N = pdg.GetSize();
    for (int i=0; i<N; ++i) {
      int ap = std::abs(pdg[i]);
      bool isPrompt = (flg[i] & (1<<0));
      if ((ap==11 || ap==13) && isPrompt) ++nPromptEmu;
      if (ap==11 || ap==13 || ap==15) {
        int m = mom[i];
        if (m>=0 && m<N && std::abs(pdg[m])==24) ++nWl;
      }
    }
    int b = nWl>3 ? 3 : nWl;
    ++byNlep[b];
    if      (nWl==0) ++nFullHad;
    else if (nWl==1) ++nSemi;
    else             ++nDilep;
    if (nPromptEmu==0) ++nNoPromptLep;
  }

  auto pct=[&](Long64_t x){ return nEvt? 100.0*x/nEvt : 0.0; };
  std::cout << "==================== tt4b decay-channel census ====================\n";
  std::cout << "[chan] events processed = " << nEvt << "\n";
  std::cout << "[chan] W->l legs (l incl tau, mother==W):\n";
  std::cout << "[chan]   0 legs (full-hadronic) = " << byNlep[0] << "  (" << pct(byNlep[0]) << "%)\n";
  std::cout << "[chan]   1 leg  (semi-leptonic) = " << byNlep[1] << "  (" << pct(byNlep[1]) << "%)\n";
  std::cout << "[chan]   2 legs (di-leptonic)   = " << byNlep[2] << "  (" << pct(byNlep[2]) << "%)\n";
  std::cout << "[chan]  >=3 legs (unphysical)   = " << byNlep[3] << "  (" << pct(byNlep[3]) << "%)\n";
  std::cout << "[chan] --- standard buckets (tau = leptonic) ---\n";
  std::cout << "[chan]   FullHadronic = " << nFullHad << "  (" << pct(nFullHad) << "%)\n";
  std::cout << "[chan]   SemiLeptonic = " << nSemi    << "  (" << pct(nSemi)    << "%)\n";
  std::cout << "[chan]   DiLeptonic   = " << nDilep   << "  (" << pct(nDilep)   << "%)\n";
  std::cout << "[chan] (proxy) 0 prompt e/mu = " << nNoPromptLep << "  (" << pct(nNoPromptLep) << "%)\n";
  std::cout << "===================================================================\n";
  std::cout << "[chan] VERDICT: full-had ~46% (semi~44, dilep~10) => DECAY-INCLUSIVE\n";
  std::cout << "[chan]          full-had ~100%                    => HADRONIC-ONLY\n";
  std::cout.flush();
}
