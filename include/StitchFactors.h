#ifndef STITCHFACTORS_H
#define STITCHFACTORS_H
// ============================================================================
//  StitchFactors  --  load + apply the ttbar stitching multiplier
// ----------------------------------------------------------------------------
//  Reads DerivedCorr/stitchFactors/stitch_factors_2017.json (produced by
//  compute_stitch_factors.py) and exposes, per MC event, the per-(sample,
//  HF-category) MULTIPLIER that is applied ON TOP of the YAML base weight:
//
//      final_weight = yaml_base_weight[sample]
//                   * multiplier[sample][category]      <-- this class
//                   * genWeight * (PU * btagSF * trigSF)
//
//  category = sub_to_category[ expandedTtbarId % 100 ]
//    inclusive sample : 1 on the HF cats it keeps, 0 on the cats rejected
//                       (each rejected cat is filled by a dedicated sample)
//    dedicated sample : r on the HF cats it owns, 0 elsewhere
//    sample not listed: 1 everywhere (untouched)
//
//  A 0 multiplier means "this event's HF category is supplied by another
//  sample" -- i.e. the event is dropped from THIS sample. This is exactly the
//  keep/reject mask of the stitch; see ttbarCategorization.md s10.
//
//  Design mirrors ExpandedTtbarId: load(path, sampleName); cheap per-event
//  lookups; fatal-exit (non-zero) on any structural problem so a Condor job
//  fails loudly instead of writing silently-wrong histograms.
// ============================================================================
#include <string>
#include <map>

class StitchFactors {
 public:
  // Load the JSON and keep the multiplier row for `sampleName`. Fatal-exit on
  // a missing file, parse error, or a malformed `analyzer_perEvent_factor`
  // block. A sample that is not listed in the plan -> inPlan()==false and
  // factor()==1 forever (sub_to_category is still loaded so category() works).
  void load(const std::string& jsonPath, const std::string& sampleName);

  bool                loaded()     const { return _loaded; }
  bool                inPlan()     const { return _inPlan; }   // inclusive or dedicated
  const std::string&  option()     const { return _option; }
  const std::string&  role()       const { return _role; }    // "inclusive"/"dedicated"/""
  const std::string&  sampleName() const { return _sample; }

  // HF category string for an expanded id (the full expandedTtbarId is fine;
  // %100 is taken internally). Returns the canonical category used as the
  // b-tag-reweight / stitch key. Unknown sub-codes -> "Unknown" (counted).
  const std::string&  category(int expandedId) const;

  // Per-(sample,category) MULTIPLIER for this event. id = expandedTtbarId.
  // Accumulates per-category bookkeeping (weightIn = pre-multiplier weight) so
  // printRunSummary() can show what the stitch did. Not-in-plan -> 1.
  double factor(int expandedId, double weightIn);

  // Multiplier with no bookkeeping (dry lookup).
  double peek(int expandedId) const;

  void printConfigSummary() const;  // call right after load()
  void printRunSummary()    const;  // call from writeHistos()/destructor

 private:
  [[noreturn]] static void fatal(const std::string& msg, int code);
  static int sub100(int v) { return ((v % 100) + 100) % 100; }

  bool        _loaded = false;
  bool        _inPlan = false;
  std::string _path, _sample, _option, _role;

  std::map<int, std::string>    _subToCat;   // 0->LF, 41..45->cc, ... 71/72->tt4b
  std::map<std::string, double> _byCat;      // this sample's multiplier per category

  // ---- end-of-job bookkeeping ----
  mutable std::map<std::string, long long> _catN;     // events seen per category
  mutable std::map<std::string, double>    _catWin;   // Sum weight before multiplier
  mutable std::map<std::string, double>    _catWout;  // Sum weight after  multiplier
  mutable long long _nUnknownSub = 0;                 // genTtbarId%100 not in the map
  std::string _unknownCat = "Unknown";
};

#endif // STITCHFACTORS_H
