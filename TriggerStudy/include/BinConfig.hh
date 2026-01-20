#ifndef BINCONFIG_HH
#define BINCONFIG_HH

// ============================================================================
// BinConfig.hh
//
// Purpose:
//   - Load simple YAML-like configuration (key: value) from config.yaml
//   - Store bin edges for HT / pT / eta and thresholds for nbjets
//   - Provide helpers to build human-readable labels (e.g. "nB2", "nB4p",
//     "Eta_m2p4_to_m1p5") for histogram/map keys
//   - Provide validation and dump utilities so you can confirm parsing
//     immediately at program startup.
//
// IMPORTANT NOTE:
//   - This is NOT a full YAML parser. It supports simple top-level lines like:
//       UseNBjets: true
//       HT_Bins: [500, 600, 700]
//     and ignores comments after '#'. It does not support nested YAML objects.
// ============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <cstdlib>   // exit()
#include <iterator>  // std::next

class BinConfig {
private:
    // ------------------------------------------------------------------------
    // Internal configuration states
    // ------------------------------------------------------------------------
    static bool _UseNBjets;
    static bool _UseEta;

    // Bin "edges" for histogram-style usage:
    //   if edges = [a, b, c]  => Nbins = 2 ( [a,b), [b,c) )
    static std::vector<double> _HT_Bins;
    static std::vector<double> _PT_Bins;
    static std::vector<double> _Eta_Bins;

    // NB_Bins is treated as "threshold list" (integers):
    //   e.g. NB_Bins = [2,3,4]
    //   labels are: nB2, nB3, nB4p (last is overflow category)
    static std::vector<int>    _NB_Bins;

    // If true, Load() will automatically print a detailed dump.
    // You can keep it true while debugging parsing, then set it false later.
    static bool _VerboseDump;

    // ------------------------------------------------------------------------
    // Utility: trim spaces/tabs
    // ------------------------------------------------------------------------
    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t");
        if (std::string::npos == first) return str;
        size_t last = str.find_last_not_of(" \t");
        return str.substr(first, (last - first + 1));
    }

    // ------------------------------------------------------------------------
    // Utility: parse vector from string "[1,2,3]" or "1,2,3"
    //   - It removes '[' and ']' and splits by comma.
    //   - T can be double or int.
    // ------------------------------------------------------------------------
    template <typename T>
    static std::vector<T> parseVector(std::string value) {
        std::vector<T> result;

        // Remove brackets if any
        value.erase(std::remove(value.begin(), value.end(), '['), value.end());
        value.erase(std::remove(value.begin(), value.end(), ']'), value.end());

        std::stringstream ss(value);
        std::string item;
        while (std::getline(ss, item, ',')) {
            item = trim(item);
            if (item.empty()) continue;

            std::stringstream conv(item);
            T val{};
            conv >> val;
            result.push_back(val);
        }
        return result;
    }

    // ------------------------------------------------------------------------
    // Utility: format a double into a filesystem/key-friendly string
    //   - 1.2  -> "1p2"
    //   - -2.4 -> "m2p4"
    //   - decimal point '.' is replaced by 'p'
    //   - minus sign '-' is replaced by 'm'
    //
    // NOTE:
    //   - precision is fixed to 1 decimal place to keep labels stable.
    // ------------------------------------------------------------------------
    static std::string fmt(double val) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << val;
        std::string s = ss.str();
        std::replace(s.begin(), s.end(), '.', 'p');
        std::replace(s.begin(), s.end(), '-', 'm');
        return s;
    }

    // ------------------------------------------------------------------------
    // Validation helpers
    // ------------------------------------------------------------------------
    static bool IsStrictlyIncreasing(const std::vector<double>& v) {
        for (size_t i = 1; i < v.size(); ++i) {
            if (!(v[i] > v[i - 1])) return false;
        }
        return true;
    }

    static bool IsNonDecreasing(const std::vector<int>& v) {
        for (size_t i = 1; i < v.size(); ++i) {
            if (v[i] < v[i - 1]) return false;
        }
        return true;
    }

    // ------------------------------------------------------------------------
    // Pretty-print helpers (for debugging parsing)
    // ------------------------------------------------------------------------
    template <typename T>
    static void PrintVector(std::ostream& os,
                            const std::string& name,
                            const std::vector<T>& v,
                            const std::string& indent = "    ") {
        os << indent << "- " << name << " (size=" << v.size() << "): ";
        for (size_t i = 0; i < v.size(); ++i) {
            os << v[i];
            if (i + 1 < v.size()) os << ", ";
        }
        os << "\n";
    }

    static void PrintEdgesAndRanges(std::ostream& os,
                                    const std::string& name,
                                    const std::vector<double>& edges,
                                    const std::string& indent = "    ",
                                    int precision = 3) {
        os << indent << "- " << name << " edges (Nedges=" << edges.size() << "): ";
        os << std::fixed << std::setprecision(precision);

        for (size_t i = 0; i < edges.size(); ++i) {
            os << edges[i];
            if (i + 1 < edges.size()) os << ", ";
        }
        os << "\n";

        // If edges represent histogram edges, bins are the intervals between edges.
        if (edges.size() >= 2) {
            os << indent << "  " << name << " ranges (Nbins=" << (edges.size() - 1) << "): ";
            for (size_t i = 0; i + 1 < edges.size(); ++i) {
                os << "[" << edges[i] << ", " << edges[i + 1] << ")";
                if (i + 2 < edges.size()) os << " ";
            }
            os << "\n";
        }
    }

public:
    // ------------------------------------------------------------------------
    // Accessors (read-only)
    // ------------------------------------------------------------------------
    static bool UseNBjets() { return _UseNBjets; }
    static bool UseEta()    { return _UseEta; }

    // These return references to internal vectors (no copy).
    static const std::vector<double>& HT_Bins()  { return _HT_Bins; }
    static const std::vector<double>& PT_Bins()  { return _PT_Bins; }
    static const std::vector<double>& Eta_Bins() { return _Eta_Bins; }
    static const std::vector<int>&    NB_Bins()  { return _NB_Bins; }

    // Control verbose dump behavior from outside if needed.
    static void SetVerboseDump(bool v) { _VerboseDump = v; }

    // ------------------------------------------------------------------------
    // Dynamic Label Finders
    // ------------------------------------------------------------------------
    // NB label rules:
    //   - if UseNBjets() is false: return "nB_Inc"
    //   - if NB_Bins=[2,3,4]:
    //       n==2 -> "nB2"
    //       n==3 -> "nB3"
    //       n>=4 -> "nB4p"  (overflow bin on last entry)
    //   - if value does not match any category (rare), return "" (meaning invalid)
    static std::string GetNBLabel(int n) {
        if (!UseNBjets()) return "nB_Inc";
        for (size_t i = 0; i < _NB_Bins.size(); ++i) {
            int binVal = _NB_Bins[i];

            // Last entry is treated as overflow bin (>= last threshold)
            if (i == _NB_Bins.size() - 1) {
                if (n >= binVal) return "nB" + std::to_string(binVal) + "p";
            } else {
                if (n == binVal) return "nB" + std::to_string(binVal);
            }
        }
        return "";
    }

    // Eta label rules:
    //   - if UseEta() is false: return "Eta_Inc"
    //   - edges define half-open intervals: [edge[i], edge[i+1])
    //   - if eta is outside [front, back): return "" (invalid/out-of-range)
    //   - label is signed (no abs):
    //       "Eta_m2p4_to_m1p5", "Eta_0p0_to_1p5", ...
    static std::string GetEtaLabel(double eta) {
        if (!UseEta()) return "Eta_Inc";

        // Out of configured range -> invalid label
        if (_Eta_Bins.empty() || eta < _Eta_Bins.front() || eta >= _Eta_Bins.back()) return "";

        for (size_t i = 0; i < _Eta_Bins.size() - 1; ++i) {
            if (eta >= _Eta_Bins[i] && eta < _Eta_Bins[i + 1]) {
                return "Eta_" + fmt(_Eta_Bins[i]) + "_to_" + fmt(_Eta_Bins[i + 1]);
            }
        }
        return "";
    }

    // ------------------------------------------------------------------------
    // Initialization Labels (used to pre-create histograms/maps)
    // ------------------------------------------------------------------------
    static std::vector<std::string> NB_Labels() {
        if (!UseNBjets()) return {"nB_Inc"};

        std::vector<std::string> labels;
        for (size_t i = 0; i < _NB_Bins.size(); ++i) {
            if (i == _NB_Bins.size() - 1) {
                labels.push_back("nB" + std::to_string(_NB_Bins[i]) + "p");
            } else {
                labels.push_back("nB" + std::to_string(_NB_Bins[i]));
            }
        }
        return labels;
    }

    static std::vector<std::string> Eta_Labels() {
        if (!UseEta()) return {"Eta_Inc"};

        std::vector<std::string> labels;
        for (size_t i = 0; i < _Eta_Bins.size() - 1; ++i) {
            labels.push_back("Eta_" + fmt(_Eta_Bins[i]) + "_to_" + fmt(_Eta_Bins[i + 1]));
        }
        return labels;
    }

    // ------------------------------------------------------------------------
    // NB helper: convert integer thresholds into histogram edges
    //   Example:
    //     NB_Bins = [2,3,4] -> edges = [2,3,4,5]
    //   which yields bins:
    //     [2,3), [3,4), [4,5)  (then you fill with integer nbjets)
    // ------------------------------------------------------------------------
    static std::vector<double> MakeNBEdges() {
        std::vector<double> edges;
        if (!UseNBjets() || _NB_Bins.empty()) return edges;

        int minB = _NB_Bins.front();
        int maxB = _NB_Bins.back();

        for (int i = minB; i <= maxB + 1; ++i) {
            edges.push_back(static_cast<double>(i));
        }
        return edges;
    }

    // ------------------------------------------------------------------------
    // Validate configuration consistency (edges monotonic, non-empty, etc.)
    // Returns:
    //   - true if "looks sane"
    //   - false if suspicious (warnings are printed)
    // ------------------------------------------------------------------------
    static bool Validate(std::ostream& os = std::cerr) {
        bool ok = true;

        // HT/PT must be valid edge lists for histograms.
        if (_HT_Bins.size() < 2) {
            os << "[BinConfig][WARN] HT_Bins has <2 edges. size=" << _HT_Bins.size() << "\n";
            ok = false;
        } else if (!IsStrictlyIncreasing(_HT_Bins)) {
            os << "[BinConfig][WARN] HT_Bins is not strictly increasing.\n";
            ok = false;
        }

        if (_PT_Bins.size() < 2) {
            os << "[BinConfig][WARN] PT_Bins has <2 edges. size=" << _PT_Bins.size() << "\n";
            ok = false;
        } else if (!IsStrictlyIncreasing(_PT_Bins)) {
            os << "[BinConfig][WARN] PT_Bins is not strictly increasing.\n";
            ok = false;
        }

        // Eta bins are optional.
        if (UseEta()) {
            if (_Eta_Bins.size() < 2) {
                os << "[BinConfig][WARN] UseEta=true but Eta_Bins has <2 edges.\n";
                ok = false;
            } else if (!IsStrictlyIncreasing(_Eta_Bins)) {
                os << "[BinConfig][WARN] Eta_Bins is not strictly increasing.\n";
                ok = false;
            }
        }

        // NB bins are optional.
        if (UseNBjets()) {
            if (_NB_Bins.empty()) {
                os << "[BinConfig][WARN] UseNBjets=true but NB_Bins is empty.\n";
                ok = false;
            } else if (!IsNonDecreasing(_NB_Bins)) {
                os << "[BinConfig][WARN] NB_Bins is not sorted (non-decreasing).\n";
                ok = false;
            }
        }

        return ok;
    }

    // ------------------------------------------------------------------------
    // Dump: print everything you need to confirm parsing + derived info
    //   - prints UseEta/UseNBjets flags
    //   - prints HT/PT/Eta edges and ranges
    //   - prints NB thresholds + derived edges
    //   - prints generated labels
    //   - prints Validate() status
    // ------------------------------------------------------------------------
    static void Dump(std::ostream& os = std::cout) {
        os << "\n>>> [BinConfig] Dump begin\n";
        os << "    - UseNBjets = " << (_UseNBjets ? "true" : "false") << "\n";
        os << "    - UseEta    = " << (_UseEta    ? "true" : "false") << "\n";

        PrintEdgesAndRanges(os, "HT_Bins", _HT_Bins, "    ", 0);
        PrintEdgesAndRanges(os, "PT_Bins", _PT_Bins, "    ", 0);

        if (_UseEta) {
            PrintEdgesAndRanges(os, "Eta_Bins", _Eta_Bins, "    ", 2);
            PrintVector(os, "Eta_Labels", Eta_Labels(), "    ");
        } else {
            os << "    - Eta_Bins disabled\n";
        }

        if (_UseNBjets) {
            PrintVector(os, "NB_Bins (thresholds)", _NB_Bins, "    ");
            auto nbEdges = MakeNBEdges();
            PrintEdgesAndRanges(os, "NB_Bins (derived edges)", nbEdges, "    ", 0);
            PrintVector(os, "NB_Labels", NB_Labels(), "    ");
        } else {
            os << "    - NB_Bins disabled\n";
        }

        bool ok = Validate();
        os << ">>> [BinConfig] Dump end (Validate=" << (ok ? "OK" : "NOT_OK") << ")\n\n";
    }

    // ------------------------------------------------------------------------
    // Load: read config file and fill internal static variables
    // Expected input format (simple, top-level):
    //   UseNBjets: true
    //   UseEta: true
    //   HT_Bins: [500, 600, 700]
    //   PT_Bins: [40, 60, 80]
    //   Eta_Bins: [-2.4, -1.5, 0.0, 1.5, 2.4]
    //   NB_Bins: [2,3,4]
    //
    // Notes:
    //   - Everything after '#' is ignored as a comment.
    //   - Empty lines are skipped.
    //   - Unknown keys are ignored silently (you can add warnings if desired).
    // ------------------------------------------------------------------------
    static void Load(const std::string& filename = "config.yaml") {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "[BinConfig] Error: Cannot open " << filename << std::endl;
            std::exit(1);
        }

        std::string line;
        while (std::getline(file, line)) {
            // Strip inline comments
            if (line.find('#') != std::string::npos) {
                line = line.substr(0, line.find('#'));
            }

            line = trim(line);
            if (line.empty()) continue;

            // Split by ':'
            size_t delim = line.find(':');
            if (delim == std::string::npos) continue;

            std::string key = trim(line.substr(0, delim));
            std::string val = trim(line.substr(delim + 1));

            // Basic key dispatch
            if (key == "UseNBjets") _UseNBjets = (val == "true");
            else if (key == "UseEta")    _UseEta    = (val == "true");
            else if (key == "HT_Bins")   _HT_Bins   = parseVector<double>(val);
            else if (key == "PT_Bins")   _PT_Bins   = parseVector<double>(val);
            else if (key == "Eta_Bins")  _Eta_Bins  = parseVector<double>(val);
            else if (key == "NB_Bins")   _NB_Bins   = parseVector<int>(val);
        }

        std::cout << ">>> [BinConfig] Loaded " << filename << std::endl;
        std::cout << "    NBjets: " << (_UseNBjets ? "ON" : "OFF")
                  << ", Eta: " << (_UseEta ? "ON" : "OFF") << std::endl;

        // Optional detailed dump for immediate parsing verification
        if (_VerboseDump) {
            Dump(std::cout);
        }
    }
};

// --------------------------------------------------------------------------
// Static member definitions (header-only style, C++17 inline variables)
// --------------------------------------------------------------------------
inline bool BinConfig::_UseNBjets = false;
inline bool BinConfig::_UseEta    = false;

inline std::vector<double> BinConfig::_HT_Bins;
inline std::vector<double> BinConfig::_PT_Bins;
inline std::vector<double> BinConfig::_Eta_Bins;
inline std::vector<int>    BinConfig::_NB_Bins;

// Verbose dump default: true for debugging (turn off later if noisy)
inline bool BinConfig::_VerboseDump = true;

#endif

