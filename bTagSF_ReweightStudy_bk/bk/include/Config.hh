#ifndef CONFIG_HH
#define CONFIG_HH

// ============================================================================
// Config.hh
//
// Centralized configuration for Trigger SF Analysis.
// All settings are defined in pure C++ - no external config files needed.
//
// Structure:
//   Section 1: Path & I/O Settings
//   Section 2: Analysis Flags
//   Section 3: Histogram Binning Definitions
//   Section 4: Utility Functions & Validation
//
// Usage:
//   - Edit the inline constants and InitBinning() function to change settings
//   - Include this header wherever configuration is needed
//   - Call Config::Dump() at startup to verify settings
//
// Author: Junghyun Lee ============================================================================

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <sstream>

// ============================================================================
// NBBin: Flexible nb-jet bin definition
// ============================================================================
struct NBBin {
    int low;   // Lower bound (inclusive)
    int high;  // Upper bound (inclusive), INF for overflow

    static constexpr int INF = 999;

    // Factory methods
    static NBBin Exact(int n)          { return {n, n}; }   // Exactly n
    static NBBin Range(int lo, int hi) { return {lo, hi}; } // [lo, hi]
    static NBBin AtLeast(int n)        { return {n, INF}; } // n or more

    // Core functionality
    bool Contains(int n) const { return n >= low && n <= high; }

    std::string Label() const {
        if (high == INF) return "nB" + std::to_string(low) + "p";
        if (low == high) return "nB" + std::to_string(low);
        return "nB" + std::to_string(low) + "to" + std::to_string(high);
    }

    std::string ToString() const {
        if (high == INF) return std::to_string(low) + "+";
        if (low == high) return std::to_string(low);
        return std::to_string(low) + "-" + std::to_string(high);
    }
};

// ============================================================================
// Config: Central configuration manager
// ============================================================================
class Config {
public:
    // ========================================================================
    // [Section 1] Path & I/O Settings
    // ========================================================================

    // Base directory containing input ntuple files
    static inline const std::string inputBaseDir = 
        "/Users/jhlee/ttHH/ntuple/skimmed/gen_tier3/";

    // TTree path inside ROOT file
    static inline const std::string treePath = "Tree/Tree";

    // ========================================================================
    // [Section 1-A] EventLooper I/O
    // ========================================================================

    // Output file prefixes for EventLooper
    static inline const std::string outPrefix_Step1 = "output_";      // Efficiency maps
    static inline const std::string outPrefix_Step2 = "validated_";   // SF validation

    // ========================================================================
    // [Section 1-B] DeriveSF I/O
    // ========================================================================

    // Input file names for DeriveSF (without path, assumes current directory)
    static inline const std::string sfInputData = "output_SingleMuon.root";
    static inline const std::string sfInputMC   = "output_TTbarInc.root";

    // Output file names for DeriveSF
    static inline const std::string sfOutputRoot = "TriggerSF.root";
    static inline const std::string sfOutputJSON = "trigger_sf.json.gz";

    // ========================================================================
    // [Section 1-C] PlotTriggerEfficiency I/O
    // ========================================================================

    // Validated file names (Step 2 output)
    static inline const std::string validatedData = "validated_SingleMuon.root";
    static inline const std::string validatedMC   = "validated_TTbarInc.root";    

    // ========================================================================
    // [Section 1-D] Sample Registry
    //
    // Central definition of all known samples.
    //   - isData: true for Data, false for MC
    //   - weight: (Xsec * Lumi) / Sum(genEventSumw) for MC, 1.0 for Data
    //
    // Usage:
    //   auto info = Config::GetSampleInfo("TTTo2L2Nu");
    //   if (!info) { /* unknown sample → error */ }
    //   bool isData = info->isData;
    //   double w    = info->weight;
    // ========================================================================

    struct SampleInfo {
        bool   isData;
        double weight;
    };

private:
    static const std::map<std::string, SampleInfo>& SampleRegistry() {
        static const std::map<std::string, SampleInfo> registry = {
            // Data
            {"SingleMuon_B",       {true,  1.0}},
            {"SingleMuon_C",       {true,  1.0}},
            {"SingleMuon_D",       {true,  1.0}},
            {"SingleMuon_E",       {true,  1.0}},
            {"SingleMuon_F",       {true,  1.0}},
            {"BTagCSV_B",          {true,  1.0}},
            {"BTagCSV_C",          {true,  1.0}},
            {"BTagCSV_D",          {true,  1.0}},
            {"BTagCSV_E",          {true,  1.0}},
            {"BTagCSV_F",          {true,  1.0}},
            {"JetHT_B",            {true,  1.0}},
            {"JetHT_C",            {true,  1.0}},
            {"JetHT_D",            {true,  1.0}},
            {"JetHT_E",            {true,  1.0}},
            {"JetHT_F",            {true,  1.0}},
            // MC: ttbar inclusive
            {"TTToHadronic",       {false, 0.000214351205}},
            {"TTTo2L2Nu",          {false, 0.0004761561474}},
            {"TTToSemiLeptonic",   {false, 0.0001455793461}},
            // MC: ttHH signal & rare
            {"ttHH",               {false, 0.00000109763773}},
            {"tt4b",               {false, 0.001292157441}},
            {"ttHtobb",            {false, 0.003125301546}},
            {"tttt",               {false, 0.00399317683}},
            {"tttW",               {false, 0.00008453854444}},
            {"ttWH",               {false, 0.000131699}},
            {"ttWW",               {false, 0.0004155131232}},
            {"ttWZ",               {false, 0.0002901229714}},
            {"ttZHto4b",           {false, 0.00000111247369}},
            {"ttZtobb",            {false, 0.006587820794}},
            {"ttZZto4b",           {false, 0.0000003824366722}},
            {"ttbb",               {false, 0.0005295497645}},
            // MC: QCD
            {"QCD_HT200to300",     {false, 1071.943332}},
            {"QCD_HT300to500",     {false, 243.1813795}},
            {"QCD_HT500to700",     {false, 20.77575731}},
            {"QCD_HT700to1000",    {false, 5.58692197}},
            {"QCD_HT1000to1500",   {false, 3.285809224}},
            {"QCD_HT1500to2000",   {false, 0.3658958167}},
            {"QCD_HT2000toInf",    {false, 0.1606282808}},
        };
        return registry;
    }

public:
    // Returns pointer to SampleInfo if found, nullptr if unknown sample
    static const SampleInfo* GetSampleInfo(const std::string& name) {
        const auto& reg = SampleRegistry();
        auto it = reg.find(name);
        return (it != reg.end()) ? &it->second : nullptr;
    }

    // ========================================================================
    // [Section 2] Analysis Flags
    // ========================================================================

    // Enable nb-jet categorization in histogram/map keys
    static inline const bool useNBjets = true;

    // Enable eta categorization in histogram/map keys
    static inline const bool useEta = false;

    // Enable Trigger SF fit or extrapolation method to fill empty bin
    static inline const bool useFitInterpolation = false;

    // Enable verbose output during event loop
    static inline const bool verbose = false;

    // Progress report interval (number of events)
    static inline const Long64_t progressInterval = 10000000;

    // ========================================================================
    // [Section 2-A] Statistical Thresholds for SF Derivation
    // ========================================================================

    // [Threshold: Data Statistics]
    // Minimum number of Data events passing the trigger required in a single bin
    // to calculate a measured Scale Factor.
    //
    // Why:
    //   If the number of passing events is too low (e.g., < 20), the statistical
    //   uncertainty becomes very large, and the Clopper-Pearson interval calculation
    //   may yield unstable results.
    // Action:
    //   If nPass(Data) < kMinPassData, the bin is treated as "empty/low-stat"
    //   and will be filled using the configured extrapolation method (Fit/Neighbor).
    // [Threshold: Data Statistics]
    // 4 b-jet 등 통계가 부족한 분석에서는 Coverage 확보를 위해 
    // 다소 공격적으로 10 정도로 낮추는 것이 유리함 (오차 ~30% 허용).
    static inline const double kMinPassData = 10.0;

    // [Threshold: MC Statistics]
    // Minimum effective sample size (Neff) of MC events passing the trigger.
    // Formula: Neff = (SumW)^2 / Sum(W^2)
    //
    // Why:
    //   MC events are weighted. A single event with a very high weight could
    //   falsely inflate the efficiency if we simply counted raw events.
    //   Neff approximates the number of unweighted events that would yield
    //   the same statistical power.
    // Action:
    //   If Neff(MC) < kMinNeffMC, the MC efficiency is considered unreliable,
    //   and the bin will be filled using the extrapolation method.
    // [Threshold: MC Statistics]
    // MC는 통계가 충분해야 하므로 보수적으로 20 유지 (혹은 더 높여도 됨).
    // MC 통계가 깨지면 SF 분모가 흔들려 전체가 불안정해짐.
    static inline const double kMinNeffMC = 20.0;

    // ========================================================================
    // [Section 3] Histogram Binning Definitions
    // ========================================================================

private:
    static inline bool _initialized = false;

    // Bin edge containers
    static inline std::vector<double> _htBins;
    static inline std::vector<double> _ptBins;
    static inline std::vector<double> _etaBins;
    static inline std::vector<NBBin>  _nbBins;

    // Internal utility: Format double for labels
    static std::string Fmt(double val) {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(1) << val;
        std::string s = ss.str();
        std::replace(s.begin(), s.end(), '.', 'p');
        std::replace(s.begin(), s.end(), '-', 'm');
        return s;
    }

    // Validation helpers
    static bool IsStrictlyIncreasing(const std::vector<double>& v) {
        for (size_t i = 1; i < v.size(); ++i) {
            if (v[i] <= v[i - 1]) return false;
        }
        return true;
    }

    static bool ValidateNBBins(std::ostream& os) {
        for (size_t i = 0; i < _nbBins.size(); ++i) {
            for (size_t j = i + 1; j < _nbBins.size(); ++j) {
                for (int test = 0; test <= 10; ++test) {
                    if (_nbBins[i].Contains(test) && _nbBins[j].Contains(test)) {
                        os << "[Config][ERROR] NB bins overlap at value " << test
                           << " (" << _nbBins[i].ToString() << " and "
                           << _nbBins[j].ToString() << ")\n";
                        return false;
                    }
                }
            }
        }
        return true;
    }

public:
    // ========================================================================
    // [Section 3 continued] Binning Initialization
    // 
    // >>> EDIT THIS FUNCTION TO CHANGE BINNING <<<
    // ========================================================================
    static void InitBinning() {
        // HT bins [GeV]
        //_htBins = {500, 600, 700, 800, 1000, 1300, 2500}; // bk. original binning
        _htBins = {500, 550, 600, 700, 800, 1000, 1500, 2500};

        // 6th jet pT bins [GeV]
        //_ptBins = {40, 45, 50, 60, 70, 90, 150};
        _ptBins = {40, 45, 50, 60, 70, 120, 200};

        // -------------------------------------------------------------------
        // Eta bins (signed, not |eta|)
        // Used for: categorization when useEta = true
        // Labels auto-generated: "Eta_m2p4_to_m1p2", "Eta_0p0_to_1p2", etc.
        // -------------------------------------------------------------------
        _etaBins = {-2.4, -1.2, 0.0, 1.2, 2.4};

        // -------------------------------------------------------------------
        // NB (number of b-jets) bins
        // Flexible definition supporting:
        //   - NBBin::Exact(n)      : exactly n b-jets
        //   - NBBin::Range(lo, hi) : lo to hi b-jets (inclusive)
        //   - NBBin::AtLeast(n)    : n or more b-jets (overflow)
        //
        // Example configurations:
        //
        // Config A: Merged low bins
        //   _nbBins = {
        //       NBBin::Range(0, 2),  // 0-2 -> "nB0to2"
        //       NBBin::Exact(3),     // 3   -> "nB3"
        //       NBBin::AtLeast(4)    // 4+  -> "nB4p"
        //   };
        //
        // Config B: Original style (exact + overflow)
        //   _nbBins = {
        //       NBBin::Exact(2),     // 2   -> "nB2"
        //       NBBin::Exact(3),     // 3   -> "nB3"
        //       NBBin::AtLeast(4)    // 4+  -> "nB4p"
        //   };
        //
        // Config C: Different ranges
        //   _nbBins = {
        //       NBBin::Range(0, 1),  // 0-1 -> "nB0to1"
        //       NBBin::Range(2, 3),  // 2-3 -> "nB2to3"
        //       NBBin::AtLeast(4)    // 4+  -> "nB4p"
        //   };
        // -------------------------------------------------------------------
        _nbBins = {
            NBBin::Range(0, 2),   // 0-2 b-jets -> "nB0to2"
            NBBin::Exact(3),      // exactly 3  -> "nB3"
            NBBin::AtLeast(4)     // 4 or more  -> "nB4p"
        };

        _initialized = true;
    }

    // ========================================================================
    // [Section 4] Accessors
    // ========================================================================
    static void Init() { if (!_initialized) InitBinning(); }

    static const std::vector<double>& HT_Bins()  { Init(); return _htBins; }
    static const std::vector<double>& PT_Bins()  { Init(); return _ptBins; }
    static const std::vector<double>& Eta_Bins() { Init(); return _etaBins; }
    static const std::vector<NBBin>&  NB_Bins()  { Init(); return _nbBins; }

    // ========================================================================
    // [Section 4] Label Generators
    // ========================================================================

    static std::string GetNBLabel(int n) {
        Init();
        if (!useNBjets) return "nB_Inc";
        for (const auto& bin : _nbBins) {
            if (bin.Contains(n)) return bin.Label();
        }
        return "";
    }

    static std::string GetEtaLabel(double eta) {
        Init();
        if (!useEta) return "Eta_Inc";
        for (size_t i = 0; i + 1 < _etaBins.size(); ++i) {
            if (eta >= _etaBins[i] && eta < _etaBins[i + 1]) {
                return "Eta_" + Fmt(_etaBins[i]) + "_to_" + Fmt(_etaBins[i + 1]);
            }
        }
        return "";
    }

    static std::vector<std::string> NB_Labels() {
        Init();
        if (!useNBjets) return {"nB_Inc"};
        std::vector<std::string> labels;
        labels.reserve(_nbBins.size());
        for (const auto& bin : _nbBins) {
            labels.push_back(bin.Label());
        }
        return labels;
    }

    static std::vector<std::string> Eta_Labels() {
        Init();
        if (!useEta) return {"Eta_Inc"};
        std::vector<std::string> labels;
        labels.reserve(_etaBins.size() - 1);
        for (size_t i = 0; i + 1 < _etaBins.size(); ++i) {
            labels.push_back("Eta_" + Fmt(_etaBins[i]) + "_to_" + Fmt(_etaBins[i + 1]));
        }
        return labels;
    }

    // ========================================================================
    // [Section 4] Histogram Utilities
    // ========================================================================

    static std::vector<double> MakeNBEdges() {
        Init();
        if (!useNBjets || _nbBins.empty()) return {};

        std::vector<double> edges;
        edges.reserve(_nbBins.size() + 1);
        for (const auto& bin : _nbBins) {
            edges.push_back(static_cast<double>(bin.low));
        }

        const auto& last = _nbBins.back();
        edges.push_back(last.high == NBBin::INF 
                        ? static_cast<double>(last.low + 1)
                        : static_cast<double>(last.high + 1));
        return edges;
    }

    static int GetNBBinIndex(int n) {
        Init();
        if (!useNBjets) return 0;
        for (size_t i = 0; i < _nbBins.size(); ++i) {
            if (_nbBins[i].Contains(n)) return static_cast<int>(i);
        }
        return -1;
    }

    // ========================================================================
    // [Section 4] Validation
    // ========================================================================
    static bool Validate(std::ostream& os = std::cerr) {
        Init();
        bool ok = true;

        if (_htBins.size() < 2) {
            os << "[Config][WARN] HT_Bins has < 2 edges\n";
            ok = false;
        } else if (!IsStrictlyIncreasing(_htBins)) {
            os << "[Config][WARN] HT_Bins not strictly increasing\n";
            ok = false;
        }

        if (_ptBins.size() < 2) {
            os << "[Config][WARN] PT_Bins has < 2 edges\n";
            ok = false;
        } else if (!IsStrictlyIncreasing(_ptBins)) {
            os << "[Config][WARN] PT_Bins not strictly increasing\n";
            ok = false;
        }

        if (useEta) {
            if (_etaBins.size() < 2) {
                os << "[Config][WARN] useEta=true but Eta_Bins has < 2 edges\n";
                ok = false;
            } else if (!IsStrictlyIncreasing(_etaBins)) {
                os << "[Config][WARN] Eta_Bins not strictly increasing\n";
                ok = false;
            }
        }

        if (useNBjets) {
            if (_nbBins.empty()) {
                os << "[Config][WARN] useNBjets=true but NB_Bins is empty\n";
                ok = false;
            } else if (!ValidateNBBins(os)) {
                ok = false;
            }
        }

        return ok;
    }

    // ========================================================================
    // [Section 4] Debug Dump
    // ========================================================================
    static void Dump(std::ostream& os = std::cout) {
        Init();

        os << "\n";
        os << "╔══════════════════════════════════════════════════════════════╗\n";
        os << "║              Configuration Summary                           ║\n";
        os << "╠══════════════════════════════════════════════════════════════╣\n";

        // Section 1: Paths
        os << "║ [Section 1] Path & I/O Settings                              ║\n";
        os << "╟──────────────────────────────────────────────────────────────╢\n";
        os << "  Input base dir : " << inputBaseDir << "\n";
        os << "  Tree path      : " << treePath << "\n";
        os << "  EventLooper out: Step1=\"" << outPrefix_Step1 
           << "\", Step2=\"" << outPrefix_Step2 << "\"\n";
        os << "  DeriveSF input : Data=\"" << sfInputData 
           << "\", MC=\"" << sfInputMC << "\"\n";
        os << "  DeriveSF output: ROOT=\"" << sfOutputRoot 
           << "\", JSON=\"" << sfOutputJSON << "\"\n";

        // Section 2: Flags
        os << "╟──────────────────────────────────────────────────────────────╢\n";
        os << "║ [Section 2] Analysis Flags                                   ║\n";
        os << "╟──────────────────────────────────────────────────────────────╢\n";
	os << "  useFitInterpolation : " << (useFitInterpolation ? "true" : "false") << "\n";
        os << "  useNBjets           : " << (useNBjets ? "true" : "false") << "\n";
        os << "  useEta              : " << (useEta ? "true" : "false") << "\n";
        os << "  verbose             : " << (verbose ? "true" : "false") << "\n";
        os << "  progressInterval    : " << progressInterval << "\n";
        os << "  kMinPassData        : " << kMinPassData << " (events)\n";
        os << "  kMinNeffMC          : " << kMinNeffMC << " (Neff)\n";

        // Section 3: Binning
        os << "╟──────────────────────────────────────────────────────────────╢\n";
        os << "║ [Section 3] Histogram Binning                                ║\n";
        os << "╟──────────────────────────────────────────────────────────────╢\n";

        os << "  HT_Bins  (" << _htBins.size() - 1 << " bins): [";
        for (size_t i = 0; i < _htBins.size(); ++i) {
            os << _htBins[i];
            if (i + 1 < _htBins.size()) os << ", ";
        }
        os << "] GeV\n";

        os << "  PT_Bins  (" << _ptBins.size() - 1 << " bins): [";
        for (size_t i = 0; i < _ptBins.size(); ++i) {
            os << _ptBins[i];
            if (i + 1 < _ptBins.size()) os << ", ";
        }
        os << "] GeV\n";

        if (useEta) {
            os << "  Eta_Bins (" << _etaBins.size() - 1 << " bins): [";
            for (size_t i = 0; i < _etaBins.size(); ++i) {
                os << _etaBins[i];
                if (i + 1 < _etaBins.size()) os << ", ";
            }
            os << "]\n";
        } else {
            os << "  Eta_Bins : disabled (useEta=false)\n";
        }

        if (useNBjets) {
            os << "  NB_Bins  (" << _nbBins.size() << " categories):\n";
            for (size_t i = 0; i < _nbBins.size(); ++i) {
                const auto& bin = _nbBins[i];
                os << "    [" << i << "] " << std::setw(5) << bin.ToString() 
                   << " -> \"" << bin.Label() << "\"\n";
            }
        } else {
            os << "  NB_Bins  : disabled (useNBjets=false)\n";
        }

        os << "╟──────────────────────────────────────────────────────────────╢\n";
        bool ok = Validate(os);
        os << "║ Validation: " << (ok ? "PASSED ✓" : "FAILED ✗") 
           << "                                          ║\n";
        os << "╚══════════════════════════════════════════════════════════════╝\n\n";
    }
};

#endif

