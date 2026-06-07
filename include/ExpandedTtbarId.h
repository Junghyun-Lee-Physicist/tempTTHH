// -*- C++ -*-
// =============================================================================
//  ExpandedTtbarId
// -----------------------------------------------------------------------------
//  Per-event tt+nb (extended ttbar-Id) lookup for the ttHH(4b) analyzer.
//
//  WHY
//  ---
//  The NanoAOD `genTtbarId` branch cannot separate tt+bbb / tt+4b: the official
//  GenTtbarCategorizer collapses every event with >= 2 additional b-jets into
//  sub-code 53/54/55 and never emits a higher code. The analyzer's own decode
//  (computeTtCategoryFromGenTtbarId) therefore tops out at kAdd2Bjet.
//
//  The expanded sub-codes 61/62 (tt+bbb, exactly 3 additional b-jets) and
//  71/72 (tt+4b, >= 4) were produced from MiniAODv2 and extracted, per sample,
//  into a tiny `ttnb_<sample>.root` lookup (TtbarIdHistCompare/extractTtNb).
//  Each row carries (run, luminosityBlock, event, genTtbarId,
//  Expanded_genTtbarId, nAddBJets, nAddBJetsMulti) and -- by construction --
//  ALL rows have nAddBJets >= 3, i.e. Expanded_genTtbarId % 100 in {61,62,71,72}.
//  Validation: the same (run,lumi,event) key matched NanoAOD 1:1 with identical
//  genTtbarId (TtbarIdHistCompare/matchTtbarId), so we can drop MiniAOD entirely
//  and just re-attach the expanded id to NanoAOD events by key.
//
//  WHAT THIS CLASS DOES
//  --------------------
//  Loads one `ttnb_<sample>.root` for the sample being analyzed into a
//  (run,lumi,event) -> {Expanded_genTtbarId, genTtbarId} hash map, then for
//  each event:
//      in map  -> tt+nb; return the stored Expanded_genTtbarId (61/62/71/72)
//      not in  -> return the NanoAOD genTtbarId unchanged
//  (Events with nAddBJets <= 2 satisfy Expanded == genTtbarId, so they are not
//  in the lookup and need no special handling -- membership alone decides.)
//
//  The map key/hash are byte-identical to matchTtbarId's, so the membership
//  decision here reproduces the validated matching exactly. The stored
//  genTtbarId is kept only to self-check that, on a hit, the lookup's
//  genTtbarId equals the NanoAOD genTtbarId of the same key -- a mismatch means
//  the WRONG ttnb file was loaded for this sample (fatal by default).
//
//  STANDALONE: depends only on ROOT (TChain/TFile) + the C++ stdlib, exactly
//  like the rest of TtbarIdHistCompare. No CMSSW, no tnm.h.
// =============================================================================
#ifndef EXPANDEDTTBARID_H
#define EXPANDEDTTBARID_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <unordered_map>

class ExpandedTtbarId {
 public:
  ExpandedTtbarId() = default;

  // --------------------------------------------------------------------------
  // Load the per-sample tt+nb lookup.
  //   path  : ttnb_<sample>.root  (empty -> stays INACTIVE; resolve() then just
  //           returns the NanoAOD id, so this is safe for samples with no
  //           lookup and for Data).
  //   label : sample tag, for log messages only (e.g. "tt4b").
  //   tree  : input tree name (extractTtNb default is "TtNb").
  // Fatal (std::exit) if a non-empty path cannot be opened / has no tree.
  // --------------------------------------------------------------------------
  void load(const std::string& path,
            const std::string& label    = "",
            const std::string& treeName  = "TtNb");

  // --------------------------------------------------------------------------
  // Resolve + load from the project lookup directory by sample key:
  //     <dir>/ttnb_<sampleKey>.root
  // (e.g. dir="DerivedCorr/expandedTtbarId", sampleKey="tt4b").
  // If that exact file is missing, fall back to a known process token that
  // appears in sampleKey (tt4b, ttbb_*, TTTo*). If nothing is found, stays
  // INACTIVE and logs the path it tried (so a misconfig is visible at once).
  // --------------------------------------------------------------------------
  void loadFromDir(const std::string& dir,
                   const std::string& sampleKey,
                   const std::string& treeName = "TtNb");

  bool        active() const { return _active; }
  std::size_t size()   const { return _map.size(); }

  // --------------------------------------------------------------------------
  // Per-event resolution. Returns the effective ttbar-Id:
  //   tt+nb hit  -> Expanded_genTtbarId (full value, sub-code in {61,62,71,72})
  //   miss/inactive -> nanoGenTtbarId (unchanged)
  // Side effects: updates hit/miss counters, prints the first N hits, and
  // self-checks the stored vs NanoAOD genTtbarId on each hit.
  // --------------------------------------------------------------------------
  int resolve(unsigned int run, unsigned int lumi,
              unsigned long long event, int nanoGenTtbarId);

  // True iff the most recent resolve() landed in the tt+nb lookup.
  bool lastWasTtNb() const { return _lastHit; }

  // End-of-run report (call from writeHistos() and/or the destructor).
  void printSummary() const;

  // ---- tunables (sane defaults; set before the event loop) ----
  void setDebugPrints(long n)          { _debugMax        = n; }
  void setAbortOnDuplicate(bool a)     { _abortOnDup      = a; }
  void setAbortOnGenIdMismatch(bool a) { _abortOnMismatch = a; }

 private:
  // Identical layout + FNV mix as TtbarIdHistCompare/matchTtbarId.cc, so the
  // (run,lumi,event) membership decision is reproduced bit-for-bit.
  struct Key {
    unsigned int       run;
    unsigned int       lumi;
    unsigned long long event;
    bool operator==(const Key& o) const noexcept {
      return run == o.run && lumi == o.lumi && event == o.event;
    }
  };
  struct KeyHash {
    std::size_t operator()(const Key& k) const noexcept {
      std::uint64_t h = 1469598103934665603ULL;
      h = (h ^ static_cast<std::uint64_t>(k.run))   * 1099511628211ULL;
      h = (h ^ static_cast<std::uint64_t>(k.lumi))  * 1099511628211ULL;
      h = (h ^ static_cast<std::uint64_t>(k.event)) * 1099511628211ULL;
      return std::hash<std::uint64_t>{}(h);
    }
  };
  struct Rec {
    int expanded;   // Expanded_genTtbarId (full value)
    int genId;      // genTtbarId stored in the lookup (for the self-check)
  };

  static int sub100(int v) { return ((v % 100) + 100) % 100; }
  static bool fileExists(const std::string& path);

  std::unordered_map<Key, Rec, KeyHash> _map;
  bool        _active = false;
  std::string _label;
  std::string _path;

  // ---- load-time counters (mirror extractTtNb's summary; must match it) ----
  long long _loadRows      = 0;   // rows inserted
  long long _l61 = 0, _l62 = 0, _l71 = 0, _l72 = 0;
  long long _lGe3          = 0;   // nAddBJets >= 3 cross-check
  long long _lSubMismatch  = 0;   // sub in {61,62,71,72} XOR nAddBJets>=3 (->0)
  long long _lDup          = 0;   // duplicate (run,lumi,event) seen at load

  // ---- resolve-time counters ----
  long long _nCalls        = 0;
  long long _nHits         = 0;
  long long _nMiss         = 0;
  long long _h61 = 0, _h62 = 0, _h71 = 0, _h72 = 0;
  long long _nGenIdMismatch = 0;

  bool _lastHit = false;

  long _debugMax     = 20;
  long _debugShown   = 0;
  long _mismatchShown = 0;

  bool _abortOnDup      = true;
  bool _abortOnMismatch = true;
};

#endif  // EXPANDEDTTBARID_H
