// -*- C++ -*-
// =============================================================================
//  ExpandedTtbarId.cc  -- see ExpandedTtbarId.h for the rationale.
// =============================================================================
#include "ExpandedTtbarId.h"
#include "ExitCodes.h"   // [STEP18] canonical exit codes
#include "SampleAlias.h" // [alias] 구/신 샘플명 대응 (2017 lookup 은 구 이름)

#include <cstdio>
#include <cstdlib>
#include <fstream>

#include <TChain.h>
#include <RtypesCore.h>

// -----------------------------------------------------------------------------
bool ExpandedTtbarId::fileExists(const std::string& path) {
  std::ifstream f(path.c_str());
  return f.good();
}

// -----------------------------------------------------------------------------
void ExpandedTtbarId::loadFromDir(const std::string& dir,
                                  const std::string& sampleKey,
                                  const std::string& treeName) {
  const std::string sep = (dir.empty() || dir.back() == '/') ? "" : "/";

  // 1) exact: <dir>/ttnb_<sampleKey>.root
  const std::string path = dir + sep + "ttnb_" + sampleKey + ".root";
  if (fileExists(path)) { load(path, sampleKey, treeName); return; }

  // 1b) [alias] 구 이름으로 만들어진 2017 lookup 도 받아준다.
  //     2017 파일은 `ttnb_TTToHadronic.root` 처럼 STEP18 이전 이름이고,
  //     2018 은 이미 신 이름이라 여기 걸리지 않는다. (SampleAlias.h 참조)
  for (const std::string& alt : SampleAlias::candidates(sampleKey)) {
    if (alt == sampleKey) continue;
    const std::string pAlt = dir + sep + "ttnb_" + alt + ".root";
    if (fileExists(pAlt)) {
      std::printf("[ExpandedTtbarId] sample '%s' resolved via legacy alias '%s'\n"
                  "[ExpandedTtbarId]   -> %s\n",
                  sampleKey.c_str(), alt.c_str(), pAlt.c_str());
      load(pAlt, sampleKey, treeName);
      return;
    }
  }

  // 2) fallback: a known, distinctive process token contained in sampleKey.
  //    (longest/most-specific first so TTbb_SemiLep is not shadowed.)
  static const char* kTokens[] = {
      "TTbb_SemiLep", "TTbb_Hadronic", "TTbb_DiLep",
      "TTbar_SemiLep",  "TTbar_Hadronic",  "TTbar_DiLep", "TT4b"};
  for (const char* tk : kTokens) {
    if (sampleKey.find(tk) == std::string::npos) continue;
    const std::string p2 = dir + sep + "ttnb_" + std::string(tk) + ".root";
    if (fileExists(p2)) {
      std::printf("[ExpandedTtbarId] resolved sample '%s' via token '%s'\n",
                  sampleKey.c_str(), tk);
      load(p2, sampleKey, treeName);
      return;
    }
  }

  // 3) nothing found.
  //    [중요] ttbar stitching 집합에 속한 샘플이면 이것은 설정 오류이고,
  //    조용히 넘어가면 tt+nb(61/62/71/72) 이벤트가 **한 건도 생기지 않는다**.
  //    그 결과 downstream b-tag norm reweight 의 tt+nb 그룹이 전 bin 1.0 이 되고,
  //    그건 "정상적인 8-group JSON" 처럼 보인다. 그래서 FATAL 로 끊는다.
  //    (Data / 비-ttbar 샘플은 lookup 이 없는 게 정상이므로 INACTIVE 로 통과.)
  if (SampleAlias::mustBeInStitchPlan(sampleKey)) {
    std::fprintf(stderr,
      "\n[FATAL][E%d][ExpandedTtbarId] no tt+nb lookup for '%s' in dir '%s'\n"
      "  tried: %s\n",
      (int)tthh::CONFIG_BAD_RUNINFO, sampleKey.c_str(), dir.c_str(), path.c_str());
    std::fprintf(stderr, "  also tried legacy aliases:");
    for (const std::string& alt : SampleAlias::candidates(sampleKey))
      if (alt != sampleKey) std::fprintf(stderr, " ttnb_%s.root", alt.c_str());
    std::fprintf(stderr,
      "\n  This sample IS part of the ttbar stitching set, so a missing lookup is a\n"
      "  MISCONFIG, not 'no patch needed'. Continuing would use the raw NanoAOD\n"
      "  genTtbarId, which never exceeds %%100=55 -> the tt+nb categories\n"
      "  (61/62/71/72) would be EMPTY and the derived b-tag norm reweight would\n"
      "  silently contain an all-1.0 tt+nb group.\n"
      "  Fix: point path_expanded_ttbarid_dir at the lookup dir\n"
      "       (2017: TTHHGenCategoryTools/Validation/lookup,\n"
      "        2018: TTHHGenCategoryTools/Validation/lookup2018).\n\n");
    std::exit(tthh::CONFIG_BAD_RUNINFO);
  }

  _active = false;
  _label  = sampleKey;
  std::printf("\n[ExpandedTtbarId] no tt+nb lookup for sample '%s' in dir '%s'\n",
              sampleKey.c_str(), dir.c_str());
  std::printf("[ExpandedTtbarId]   tried: %s\n", path.c_str());
  std::printf("[ExpandedTtbarId]   -> INACTIVE: every event uses NanoAOD genTtbarId.\n");
  std::printf("[ExpandedTtbarId]   (expected for Data and non-ttbar samples)\n\n");
  std::fflush(stdout);
}

// -----------------------------------------------------------------------------
void ExpandedTtbarId::load(const std::string& path,
                           const std::string& label,
                           const std::string& treeName) {
  _label = label.empty() ? std::string("sample") : label;
  _path  = path;

  if (path.empty()) {
    _active = false;
    std::printf("[ExpandedTtbarId] DISABLED for %s "
                "(no ttnb lookup file given) -> all events use NanoAOD genTtbarId.\n",
                _label.c_str());
    std::fflush(stdout);
    return;
  }

  std::printf("\n[ExpandedTtbarId] ===== loading tt+nb lookup =====\n");
  std::printf("[ExpandedTtbarId]   label = %s\n", _label.c_str());
  std::printf("[ExpandedTtbarId]   file  = %s\n", path.c_str());
  std::printf("[ExpandedTtbarId]   tree  = %s\n", treeName.c_str());
  std::fflush(stdout);

  TChain in(treeName.c_str());
  if (in.Add(path.c_str()) <= 0) {
    std::fprintf(stderr,
                 "[ExpandedTtbarId] FATAL: cannot add '%s' (tree '%s') to chain.\n",
                 path.c_str(), treeName.c_str());
    std::exit(tthh::EXPTTID_CHAIN_FAIL);
  }

  UInt_t    run = 0, lumi = 0;
  ULong64_t event = 0;
  Int_t     genTtbarId = 0, expandedId = 0, nAddBJets = 0, nAddBJetsMulti = 0;

  in.SetBranchStatus("*", 0);
  for (const char* b : {"run", "luminosityBlock", "event",
                        "genTtbarId", "Expanded_genTtbarId",
                        "nAddBJets", "nAddBJetsMulti"})
    in.SetBranchStatus(b, 1);
  in.SetBranchAddress("run",                 &run);
  in.SetBranchAddress("luminosityBlock",     &lumi);
  in.SetBranchAddress("event",               &event);
  in.SetBranchAddress("genTtbarId",          &genTtbarId);
  in.SetBranchAddress("Expanded_genTtbarId", &expandedId);
  in.SetBranchAddress("nAddBJets",           &nAddBJets);
  in.SetBranchAddress("nAddBJetsMulti",      &nAddBJetsMulti);

  const Long64_t nIn = in.GetEntries();
  if (nIn <= 0) {
    std::fprintf(stderr,
                 "[ExpandedTtbarId] FATAL: tree '%s' in '%s' has %lld entries.\n",
                 treeName.c_str(), path.c_str(), (long long)nIn);
    std::exit(tthh::EXPTTID_TREE_EMPTY);
  }
  std::printf("[ExpandedTtbarId]   entries in lookup = %lld\n", (long long)nIn);
  std::fflush(stdout);

  _map.reserve(static_cast<std::size_t>(nIn) * 2);

  for (Long64_t i = 0; i < nIn; ++i) {
    in.GetEntry(i);

    const int esub    = sub100(expandedId);
    const bool isTtNb = (esub == 61 || esub == 62 || esub == 71 || esub == 72);
    const bool ge3    = (nAddBJets >= 3);
    if (isTtNb != ge3) ++_lSubMismatch;   // extractTtNb guarantees this == 0
    if (ge3) ++_lGe3;

    // The lookup should contain ONLY tt+nb rows. If a stray non-tt+nb row is
    // present, skip it (it would be Expanded==genTtbarId anyway).
    if (!isTtNb) continue;

    const Key k{run, lumi, event};
    auto res = _map.emplace(k, Rec{expandedId, genTtbarId});
    if (!res.second) {
      ++_lDup;
      const Rec& prev = res.first->second;
      const bool sameValue =
          (prev.expanded == expandedId && prev.genId == genTtbarId);
      std::fprintf(stderr,
          "[ExpandedTtbarId] DUPLICATE (run,lumi,event)=(%u,%u,%llu) in %s: "
          "kept Expanded=%d genId=%d, new Expanded=%d genId=%d %s\n",
          run, lumi, (unsigned long long)event, _label.c_str(),
          prev.expanded, prev.genId, expandedId, genTtbarId,
          sameValue ? "(identical, ignored)" : "(CONFLICTING VALUES)");
      if (!sameValue && _abortOnDup) {
        std::fprintf(stderr,
            "[ExpandedTtbarId] FATAL: conflicting duplicate key in lookup '%s'.\n",
            path.c_str());
        std::exit(tthh::EXPTTID_DUP_KEY);
      }
      continue;  // keep the first
    }

    ++_loadRows;
    if      (esub == 61) ++_l61;
    else if (esub == 62) ++_l62;
    else if (esub == 71) ++_l71;
    else if (esub == 72) ++_l72;
  }

  _active = true;

  // ---- load summary: diff these against extractTtNb's log for this sample ----
  std::printf("[ExpandedTtbarId]   loaded rows (map size)      : %lld\n", _loadRows);
  std::printf("[ExpandedTtbarId]   nAddBJets>=3 (cross-check)   : %lld\n", _lGe3);
  std::printf("[ExpandedTtbarId]   tt+bbb (61+62) : %lld    tt+4b (71+72) : %lld\n",
              _l61 + _l62, _l71 + _l72);
  std::printf("[ExpandedTtbarId]     61 : %lld   62(multi) : %lld   71 : %lld   72(multi) : %lld\n",
              _l61, _l62, _l71, _l72);
  if (_lSubMismatch != 0)
    std::printf("[ExpandedTtbarId]   >>> WARNING: sub-code vs nAddBJets disagreed on %lld rows\n",
                _lSubMismatch);
  if (_lDup != 0)
    std::printf("[ExpandedTtbarId]   >>> NOTE: %lld duplicate key(s) seen at load\n", _lDup);
  std::printf("[ExpandedTtbarId]   (these counts must match extractTtNb's summary for '%s')\n",
              _label.c_str());
  std::printf("[ExpandedTtbarId] ===== lookup ready =====\n\n");
  std::fflush(stdout);
}

// -----------------------------------------------------------------------------
int ExpandedTtbarId::resolve(unsigned int run, unsigned int lumi,
                             unsigned long long event, int nanoGenTtbarId) {
  ++_nCalls;
  _lastHit = false;

  if (!_active) { ++_nMiss; return nanoGenTtbarId; }

  auto it = _map.find(Key{run, lumi, event});
  if (it == _map.end()) { ++_nMiss; return nanoGenTtbarId; }

  // ---- tt+nb hit ----
  _lastHit = true;
  ++_nHits;
  const Rec& r = it->second;
  const int esub = sub100(r.expanded);
  if      (esub == 61) ++_h61;
  else if (esub == 62) ++_h62;
  else if (esub == 71) ++_h71;
  else if (esub == 72) ++_h72;

  // Self-check: lookup genTtbarId must equal the NanoAOD genTtbarId of this
  // (run,lumi,event). A mismatch means the wrong ttnb file is loaded.
  if (r.genId != nanoGenTtbarId) {
    ++_nGenIdMismatch;
    if (_mismatchShown < _debugMax) {
      ++_mismatchShown;
      std::fprintf(stderr,
          "[ExpandedTtbarId] genTtbarId MISMATCH @ (run,lumi,evt)=(%u,%u,%llu): "
          "lookup genId=%d, NanoAOD genId=%d (Expanded=%d). "
          "Wrong ttnb file for this sample?\n",
          run, lumi, (unsigned long long)event,
          r.genId, nanoGenTtbarId, r.expanded);
      std::fflush(stderr);
    }
    if (_abortOnMismatch) {
      std::fprintf(stderr,
          "[ExpandedTtbarId] FATAL: lookup '%s' does not correspond to this "
          "sample (genTtbarId mismatch on a matched key).\n", _path.c_str());
      std::exit(tthh::EXPTTID_SAMPLE_MISMATCH);
    }
  }

  if (_debugShown < _debugMax) {
    ++_debugShown;
    std::printf("[ExpandedTtbarId] tt+nb #%ld (run:lumi:evt = %u:%u:%llu): "
                "genTtbarId %d (sub %d) -> Expanded %d (sub %d)\n",
                _debugShown, run, lumi, (unsigned long long)event,
                nanoGenTtbarId, sub100(nanoGenTtbarId), r.expanded, esub);
    std::fflush(stdout);
  }

  return r.expanded;
}

// -----------------------------------------------------------------------------
void ExpandedTtbarId::printSummary() const {
  std::printf("\n[ExpandedTtbarId] ===== summary (%s) =====\n",
              _label.empty() ? "sample" : _label.c_str());
  if (!_active) {
    std::printf("[ExpandedTtbarId]   lookup INACTIVE -> every event used NanoAOD genTtbarId\n");
    std::printf("[ExpandedTtbarId]   resolve() calls : %lld (all returned NanoAOD id)\n", _nCalls);
    std::printf("[ExpandedTtbarId] =================================\n\n");
    std::fflush(stdout);
    return;
  }
  std::printf("[ExpandedTtbarId]   lookup rows                  : %lld\n", _loadRows);
  std::printf("[ExpandedTtbarId]   resolve() calls              : %lld\n", _nCalls);
  std::printf("[ExpandedTtbarId]   tt+nb hits                   : %lld\n", _nHits);
  std::printf("[ExpandedTtbarId]   misses (NanoAOD id used)     : %lld\n", _nMiss);
  std::printf("[ExpandedTtbarId]     hits by Expanded sub-code  : 61=%lld 62=%lld 71=%lld 72=%lld\n",
              _h61, _h62, _h71, _h72);
  std::printf("[ExpandedTtbarId]     tt+bbb (61+62) : %lld    tt+4b (71+72) : %lld\n",
              _h61 + _h62, _h71 + _h72);
  if (_nGenIdMismatch != 0)
    std::printf("[ExpandedTtbarId]   >>> WARNING: %lld genTtbarId mismatch(es) on hits "
                "(wrong lookup file?)\n", _nGenIdMismatch);
  else
    std::printf("[ExpandedTtbarId]   genTtbarId self-check        : OK (0 mismatches on hits)\n");

  // Hits cannot exceed the number of tt+nb rows; if the analysis runs the full
  // sample and applies no upstream skim, hits == lookup rows is the expectation.
  if (_nHits > _loadRows)
    std::printf("[ExpandedTtbarId]   >>> WARNING: hits (%lld) > lookup rows (%lld) -- "
                "duplicate keys or repeated events?\n", _nHits, _loadRows);

  // ── [2026-07-29] coverage 를 job 밖에서 합산할 수 있게 기계가 읽는 줄 하나 ──
  //
  //   왜 여기서 FATAL 을 못 내는가: condor job 은 입력 파일 **일부**만 본다.
  //   따라서 job 하나에서는 hits < rows 가 정상이다. coverage 는 한 샘플의
  //   전 job 을 합쳐야만 판정된다.
  //
  //   왜 판정이 필요한가: patch 는 (run,lumi,event) 로 키가 잡힌다. 그래서
  //   ntuple 재생산(v18 -> v20) 처럼 **event 집합이 같은** 변경에는 영향이
  //   없지만, 입력 dataset 자체가 달라지면 키가 안 맞는다. 그때 나타나는 것은
  //   크래시가 아니라 **miss** 이고, miss 는 원본 genTtbarId 를 그대로 돌려주므로
  //   tt+nb 가 조용히 적게 잡힌다. genTtbarId self-check 는 matched key 에서만
  //   돌기 때문에 이 경우를 못 잡는다.
  //
  //   판정 기준:  Σ_jobs(hits) == rows   (전 job 합산)
  //   확인 도구:  tools/check_ttnb_coverage.py  (이 줄을 grep 해서 합산)
  std::printf("[TTNB_COVERAGE] sample=%s rows=%lld hits=%lld miss=%lld "
              "calls=%lld mismatch=%lld\n",
              _label.empty() ? "sample" : _label.c_str(),
              _loadRows, _nHits, _nMiss, _nCalls, _nGenIdMismatch);

  std::printf("[ExpandedTtbarId] =================================\n\n");
  std::fflush(stdout);
}
