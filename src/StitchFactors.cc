// ============================================================================
//  StitchFactors implementation
// ============================================================================
#include "StitchFactors.h"
#include "ExitCodes.h"   // [STEP18] canonical exit codes

#include <nlohmann/json.hpp>

#include <cstdlib>     // std::exit
#include <fstream>
#include <iostream>
#include <iomanip>

using nlohmann::json;

// ----------------------------------------------------------------------------
void StitchFactors::fatal(const std::string& msg, int code) {
  std::cerr << "\n[FATAL][StitchFactors] " << msg << "\n"
            << "  -> aborting (exit " << code << ") so the Condor job is flagged.\n"
            << std::endl;
  std::exit(code);
}

// ----------------------------------------------------------------------------
void StitchFactors::load(const std::string& jsonPath, const std::string& sampleName) {
  _path   = jsonPath;
  _sample = sampleName;
  _loaded = false;
  _inPlan = false;
  _role.clear();
  _subToCat.clear();
  _byCat.clear();

  std::ifstream f(jsonPath);
  if (!f.good())
    fatal("cannot open stitch-factors JSON '" + jsonPath +
          "'. Set $STITCH_FACTORS_JSON or place it at "
          "DerivedCorr/stitchFactors/stitch_factors_2017.json.", tthh::STITCH_JSON_OPEN_FAIL);

  json j;
  try {
    f >> j;
  } catch (const std::exception& e) {
    fatal("failed to parse '" + jsonPath + "': " + std::string(e.what()), tthh::STITCH_JSON_PARSE_FAIL);
  }

  if (!j.contains("analyzer_perEvent_factor"))
    fatal("'" + jsonPath + "' has no 'analyzer_perEvent_factor' block "
          "(wrong file? re-run compute_stitch_factors.py).", tthh::STITCH_JSON_SCHEMA_FAIL);
  const json& a = j.at("analyzer_perEvent_factor");

  // option string (cosmetic / log only)
  if (j.contains("result") && j.at("result").contains("option"))
    _option = j.at("result").at("option").get<std::string>();
  else if (j.contains("config") && j.at("config").contains("stitch_option"))
    _option = j.at("config").at("stitch_option").get<std::string>();

  // sub_to_category : { "0":"LF", "41":"cc", ..., "61":"ttbbb", "71":"tt4b" }
  if (!a.contains("sub_to_category"))
    fatal("'analyzer_perEvent_factor' has no 'sub_to_category' map.", tthh::STITCH_JSON_SCHEMA_FAIL);
  for (auto it = a.at("sub_to_category").begin();
            it != a.at("sub_to_category").end(); ++it) {
    int sub = 0;
    try { sub = std::stoi(it.key()); }
    catch (...) { fatal("sub_to_category key '" + it.key() + "' is not an int.", tthh::STITCH_JSON_SCHEMA_FAIL); }
    _subToCat[sub] = it.value().get<std::string>();
  }
  if (_subToCat.empty())
    fatal("sub_to_category is empty.", tthh::STITCH_JSON_SCHEMA_FAIL);

  // this sample's multiplier row (absent -> not in plan, multiplier 1)
  if (a.contains("samples") && a.at("samples").contains(_sample)) {
    const json& s = a.at("samples").at(_sample);
    _inPlan = true;
    if (s.contains("role")) _role = s.at("role").get<std::string>();
    if (!s.contains("by_category"))
      fatal("sample '" + _sample + "' is listed but has no 'by_category' row.", tthh::STITCH_JSON_SCHEMA_FAIL);
    for (auto it = s.at("by_category").begin();
              it != s.at("by_category").end(); ++it)
      _byCat[it.key()] = it.value().get<double>();

    // every category the id map can produce must have a multiplier
    for (const auto& kv : _subToCat) {
      if (_byCat.find(kv.second) == _byCat.end())
        fatal("sample '" + _sample + "' (role " + _role + ") is missing a "
              "multiplier for category '" + kv.second + "'.", tthh::STITCH_JSON_SCHEMA_FAIL);
    }
  }

  _loaded = true;
}

// ----------------------------------------------------------------------------
const std::string& StitchFactors::category(int expandedId) const {
  auto it = _subToCat.find(sub100(expandedId));
  if (it == _subToCat.end()) { ++_nUnknownSub; return _unknownCat; }
  return it->second;
}

// ----------------------------------------------------------------------------
double StitchFactors::peek(int expandedId) const {
  if (!_inPlan) return 1.0;
  const std::string& cat = category(expandedId);
  auto it = _byCat.find(cat);
  return (it == _byCat.end()) ? 1.0 : it->second;
}

// ----------------------------------------------------------------------------
double StitchFactors::factor(int expandedId, double weightIn) {
  const std::string& cat = category(expandedId);  // counts unknown subs
  double m = 1.0;
  if (_inPlan) {
    auto it = _byCat.find(cat);
    m = (it == _byCat.end()) ? 1.0 : it->second;
  }
  _catN[cat]    += 1;
  _catWin[cat]  += weightIn;
  _catWout[cat] += weightIn * m;
  return m;
}

// ----------------------------------------------------------------------------
void StitchFactors::printConfigSummary() const {
  std::cout << "\n[StitchFactors] ===================================================\n"
            << "  file    : " << _path   << "\n"
            << "  option  : " << (_option.empty() ? "(none)" : _option) << "\n"
            << "  sample  : " << _sample << "\n";
  if (!_inPlan) {
    std::cout << "  role    : (not in stitch plan) -> multiplier = 1 for every event\n"
              << "[StitchFactors] ===================================================\n"
              << std::endl;
    return;
  }
  std::cout << "  role    : " << _role << "   (multiplier applied ON TOP of YAML weight)\n"
            << "  per-category multiplier:\n";
  for (const auto& kv : _byCat)
    std::cout << "      " << std::left << std::setw(8) << kv.first << " : "
              << std::setprecision(8) << kv.second << "\n";
  std::cout << "  note    : 0 => HF category supplied by another sample (event dropped);\n"
            << "            r => dedicated sample rescaled to its inclusive anchor.\n"
            << "[StitchFactors] ===================================================\n"
            << std::endl;
}

// ----------------------------------------------------------------------------
void StitchFactors::printRunSummary() const {
  std::cout << "\n[StitchFactors] run summary (sample=" << _sample
            << (_inPlan ? (", role=" + _role) : ", not in plan") << ")\n"
            << "  " << std::left << std::setw(10) << "category"
            << std::right << std::setw(14) << "nEvents"
            << std::setw(18) << "SumW_in"
            << std::setw(18) << "SumW_out"
            << std::setw(12) << "applied"  << "\n"
            << "  ----------------------------------------------------------------------\n";
  double tin = 0.0, tout = 0.0; long long tn = 0;
  for (const auto& kv : _catN) {
    const std::string& c = kv.first;
    const double win  = _catWin.count(c)  ? _catWin.at(c)  : 0.0;
    const double wout = _catWout.count(c) ? _catWout.at(c) : 0.0;
    const double app  = (win != 0.0) ? wout / win : 0.0;
    std::cout << "  " << std::left << std::setw(10) << c
              << std::right << std::setw(14) << kv.second
              << std::setw(18) << std::setprecision(8) << win
              << std::setw(18) << std::setprecision(8) << wout
              << std::setw(12) << std::setprecision(5) << app << "\n";
    tin += win; tout += wout; tn += kv.second;
  }
  std::cout << "  ----------------------------------------------------------------------\n"
            << "  " << std::left << std::setw(10) << "TOTAL"
            << std::right << std::setw(14) << tn
            << std::setw(18) << std::setprecision(8) << tin
            << std::setw(18) << std::setprecision(8) << tout
            << std::setw(12) << std::setprecision(5) << (tin != 0.0 ? tout / tin : 0.0) << "\n";
  if (_nUnknownSub > 0)
    std::cout << "  WARNING: " << _nUnknownSub << " event(s) had genTtbarId%100 outside the "
              << "sub_to_category map (counted as 'Unknown', multiplier 1).\n";
  std::cout << "[StitchFactors] ===================================================\n"
            << std::endl;
}
