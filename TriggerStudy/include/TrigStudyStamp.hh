#ifndef TRIGSTUDYSTAMP_HH
#define TRIGSTUDYSTAMP_HH
// =============================================================================
// TrigStudyStamp.hh -- the year of a TriggerStudy output file   [STEP 26 L, 2026-10-09]
//
//   EventLooper writes a TNamed "TrigStudyStamp" with the title "year=<YYYY>; reference=<HLT path>" into every
//   output_<sample>.root / validated_<sample>.root. hadd keeps one per input file (as cycles of the same key), so a
//   merged file still shows every input's year. DeriveSF and PlotTriggerEfficiency refuse merged files whose stamps
//   are not this run's (TTHH_YEAR): without it, Step 1 maps of 2017 run through DeriveSF with TTHH_YEAR=2024 gave a
//   JSON tagged year=2024 that the 2024 analyzer would load (independent review of commit L, 2026-10-09).
//   No stamp at all = an output of before STEP 26 = 2017: accepted for 2017 only.
// =============================================================================
#include <TFile.h>
#include <TKey.h>
#include <TNamed.h>

#include <string>

#include "Config.hh"

namespace TrigStudyStamp {

inline std::string Expected() { return "year=" + Config::Year() + "; reference=" + Config::RefTrigger(); }

/// write the stamp into the current directory of `f` (an EventLooper output)
inline void Write(TFile* f) {
    if (!f) return;
    f->cd();
    TNamed s("TrigStudyStamp", Expected().c_str());
    s.Write();
}

/// "" if every stamp of `f` is this run's (or, for 2017, there is none); else what is wrong. nFound: the stamps seen.
inline std::string Problem(TFile* f, int& nFound) {
    nFound = 0;
    if (!f) return "no file";
    const std::string want = Expected();
    std::string other;
    TIter next(f->GetListOfKeys());
    while (TKey* k = static_cast<TKey*>(next())) {
        if (std::string(k->GetName()) != "TrigStudyStamp") continue;
        ++nFound;
        TObject* o = k->ReadObj();
        const std::string t = o ? o->GetTitle() : "?";
        delete o;
        if (t != want && other.empty()) other = t;
    }
    if (!other.empty())
        return "made with '" + other + "', this run is '" + want + "' (TTHH_YEAR; run_analysis.sh --year)";
    if (nFound == 0 && Config::Year() != "2017")
        return "no TrigStudyStamp (an output of before STEP 26, i.e. 2017), this run is '" + want + "'";
    return "";
}

}  // namespace TrigStudyStamp

#endif  // TRIGSTUDYSTAMP_HH
