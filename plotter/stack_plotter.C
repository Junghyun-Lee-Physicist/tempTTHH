/**
 * CMS Stack Plotter (Smart Color & Label Edition + Yield Legend + Cutflow)
 *
 * Features (unchanged from previous version):
 *   - QCD: red gradient by HT bin (light → dark)
 *   - TTbar: hadronic = blue, semi = teal, 2l = green
 *   - Smart legend: QCD grouped into one entry, short LaTeX labels
 *   - Signal/rare BG: distinct, muted colors
 *
 * NEW in this revision:
 *   - structure_info.yml is now a FLAT list of plottable histograms.
 *     extract_structure.py is the single source of truth for the "what is
 *     plottable" policy (it filters out TTree/TBranch/TDirectory/TGraph
 *     before writing the yaml). The plotter trusts the yaml verbatim.
 *     Older nested-style structure_info.yml files are NO LONGER accepted —
 *     re-run extract_structure.py to migrate.
 *   - Cutflow histograms are now drawn just like any other hist. Their
 *     actual ROOT path is `Tree/cutflow{,_w}` in the current analyzer
 *     (writeTree() leaves the cwd at the "Tree" TDirectory and
 *     hCutFlow->Write() runs immediately afterwards in performAnalysis()).
 *     Bin labels from the analyzer are used as the x-axis tick labels.
 *   - Higgs mass plots (cutStep_<N>_higgs can01/02) are drawn as before;
 *     output file names with spaces are now sanitized to underscores so the
 *     PDFs land at  cutStep_8_higgs_can01.pdf  (space-free, easier to ls).
 *   - Each MC entry in the legend now carries its integrated yield in
 *     parentheses, e.g.  "t#bar{t} (had)   12345.6"
 *   - Data legend entry shows the data yield as a bare integer.
 *   - The MC-sum legend entry "Total MC" is added (yield only) so the total
 *     SF-corrected prediction is readable at a glance.
 *
 * [ Usage ]
 * $ root -l -b -q stack_plotter.C+
 * (Or invoked from scenario_runner.py wrapper.)
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdlib>    // [STEP21] std::getenv — TTHH_PLOT_GROUPING 모드 선택

// ROOT Headers
#include "TFile.h"
#include "TH1F.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TLine.h"
#include "TColor.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TError.h"
#include "TPad.h"
#include "TString.h"
#include "TAxis.h"

// ============================================================================
// 1. DATA STRUCTURES
// ============================================================================

struct SampleInfo {
    std::string name;
    std::string type;
    std::string label;
    std::string colorHex;
    std::vector<std::string> files;
};

struct HistInfo {
    std::string key_path;     // ROOT internal path, e.g. "CutflowKinematics/cutflow_w"
    std::string clean_name;   // sanitized file-system name (no slashes, no spaces)
};

// ============================================================================
// 2. HELPER FUNCTIONS
// ============================================================================

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

void Log(std::string msg) {
    std::cout << "[StackPlotter] " << msg << std::endl;
}

/**
 * Sanitize a path string for use as a filesystem name.
 *   '/' → '_',  ' ' → '_',  any other character kept as-is.
 * (The plotter writes PNGs whose name comes from the histogram path; histogram
 * names like "cutStep_8_higgs can01" would otherwise produce a filename with
 * a space in it.)
 */
std::string SanitizeForFilename(const std::string& s) {
    std::string out = s;
    for (char& c : out) {
        if (c == '/' || c == ' ' || c == '\t') c = '_';
    }
    return out;
}

/**
 * Format a yield number for the legend.
 *   < 1000        → "%.1f"      (e.g. 12.3)
 *   < 1e6         → "%.0f"      (e.g. 12345)
 *   >= 1e6        → "%.2e"      (e.g. 1.23e+06)
 */
std::string FormatYield(double y) {
    char buf[32];
    if (y < 1000.0)        std::snprintf(buf, sizeof(buf), "%.1f",  y);
    else if (y < 1.0e6)    std::snprintf(buf, sizeof(buf), "%.0f",  y);
    else                   std::snprintf(buf, sizeof(buf), "%.2e",  y);
    return std::string(buf);
}

std::vector<SampleInfo> ParseSampleConfig(std::string filename) {
    Log("Parsing SAMPLE config: " + filename);
    std::vector<SampleInfo> samples;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "   [Error] Cannot open config file: " << filename << std::endl;
        return samples;
    }

    std::string line;
    SampleInfo currentSample;
    bool inSamplesBlock = false;
    bool parsingFiles = false;

    while (std::getline(file, line)) {
        size_t commentPos = line.find('#');
        if (line.find("color:") == std::string::npos && commentPos != std::string::npos) {
             line = line.substr(0, commentPos);
        }

        std::string raw = trim(line);
        if (raw.empty()) continue;

        size_t indent = line.find_first_not_of(" ");
        if (indent == std::string::npos) indent = 0;

        if (raw == "samples:") {
            inSamplesBlock = true;
            continue;
        }

        if (inSamplesBlock) {
            if (indent == 2 && raw.back() == ':') {
                if (!currentSample.name.empty()) samples.push_back(currentSample);
                currentSample = SampleInfo();
                currentSample.name = raw.substr(0, raw.size() - 1);
                parsingFiles = false;
            }
            else if (indent > 2) {
                if (raw.rfind("type:", 0) == 0) { currentSample.type = trim(raw.substr(5)); parsingFiles = false; }
                else if (raw.rfind("label:", 0) == 0) { currentSample.label = trim(raw.substr(6)); parsingFiles = false; }
                else if (raw.rfind("color:", 0) == 0) {
                    std::string c = trim(raw.substr(6));
                    c.erase(std::remove(c.begin(), c.end(), '\''), c.end());
                    c.erase(std::remove(c.begin(), c.end(), '\"'), c.end());
                    currentSample.colorHex = c;
                    parsingFiles = false;
                }
                else if (raw.rfind("files:", 0) == 0) { parsingFiles = true; }
                else if (raw.rfind("- ", 0) == 0) {
                    if (parsingFiles) currentSample.files.push_back(trim(raw.substr(2)));
                }
            }
        }
    }
    if (!currentSample.name.empty()) samples.push_back(currentSample);

    std::cout << "   -> Successfully parsed " << samples.size() << " samples." << std::endl;
    return samples;
}

std::vector<HistInfo> ParseStructureConfig(std::string filename) {
    Log("Parsing STRUCTURE config: " + filename);
    std::vector<HistInfo> hists;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "   [Error] Cannot open config file: " << filename << std::endl;
        return hists;
    }

    // ── Format expected: flat list under `histograms:` ────────────────────
    //   description: ...
    //   input_file:  ...
    //   total_count: N
    //   histograms:
    //     - key_path: <path>
    //       classname: TH1F
    //       title: <text>
    //       nbins: N
    //       xlow: <float>
    //       xhigh: <float>
    //     - key_path: ...
    //
    // The extract_structure.py stage is responsible for emitting ONLY the
    // histograms that should be plotted; this parser does not apply any
    // further filter. (Single source of truth for "what is plottable" lives
    // in extract_structure.py.)
    //
    // Older nested-style yaml is no longer accepted — regenerate the file
    // with the new extract_structure.py if you see "no histograms found".

    std::string line;
    bool inHistogramsBlock = false;

    while (std::getline(file, line)) {
        std::string raw = trim(line);
        if (raw.empty() || raw[0] == '#') continue;

        if (raw == "histograms:") {
            inHistogramsBlock = true;
            continue;
        }
        if (!inHistogramsBlock) continue;

        // Each entry begins with "- key_path: <value>". We only need
        // key_path; other metadata (classname/title/nbins/xlow/xhigh)
        // is informative but not consumed in the current plotter.
        const std::string keyPrefix = "- key_path:";
        if (raw.rfind(keyPrefix, 0) == 0) {
            std::string path = trim(raw.substr(keyPrefix.size()));
            // strip surrounding single or double quotes if present
            if (path.size() >= 2 &&
                ((path.front() == '\'' && path.back() == '\'') ||
                 (path.front() == '"'  && path.back() == '"'))) {
                path = path.substr(1, path.size() - 2);
            }
            if (path.empty()) continue;

            HistInfo hi;
            hi.key_path = path;
            // ── Sanitize for filesystem use ──
            //   slashes → underscores, spaces → underscores.
            hi.clean_name = SanitizeForFilename(path);
            hists.push_back(hi);
        }
    }

    std::cout << "   -> Found " << hists.size() << " target histograms." << std::endl;
    return hists;
}

// ============================================================================
// 2.5  SMART COLOR & LABEL FUNCTIONS  (unchanged)
// ============================================================================

int GetQCDHTValue(const std::string& name) {
    size_t pos = name.find("HT");
    if (pos == std::string::npos) pos = name.find("Pt");
    if (pos == std::string::npos) return 0;
    pos += 2;
    while (pos < name.size() && !isdigit(name[pos])) pos++;
    std::string numStr;
    while (pos < name.size() && isdigit(name[pos])) numStr += name[pos++];
    return numStr.empty() ? 0 : std::stoi(numStr);
}

int GetSmartColor(const std::string& name) {
    // ── QCD : ONE flat colour (was a 9-step red gradient by HT bin) ──
    // QCD multijet is a single physics process here; the HT slices are just an
    // MC generation convenience. A single block keeps the (QCD-dominated) stack
    // readable. [changed 2026-06]
    if (name.find("QCD") != std::string::npos)
        return TColor::GetColor("#C0392B");          // flat brick red

    // ── dedicated heavy-flavour ttbar (the stitched tt+B / tt+nb pieces) ──
    // Checked BEFORE the inclusive "TTTo" / ttH / ttZ / ttW branches.
    // (Names like "ttbb_*"/"tt4b" do not contain "TTTo"/"ttH"/"ttW"/"ttZ", so
    //  there is no collision; ttHTobb has no "ttbb" substring.)
    //   ttbb_* (dedicated tt+2b, 4FS NLO)  → violet family by decay channel
    //   tt4b   (dedicated tt+nb, LO)        → one bold standout colour
    if (name.find("ttbb") != std::string::npos || name.find("ttBB") != std::string::npos) {
        if (name.find("Hadronic") != std::string::npos) return TColor::GetColor("#8E44AD"); // purple
        if (name.find("SemiLep")  != std::string::npos) return TColor::GetColor("#BB6BD9"); // light purple
        if (name.find("2L2Nu")    != std::string::npos) return TColor::GetColor("#D7BDE2"); // pale purple
        return TColor::GetColor("#8E44AD");
    }
    if (name.find("tt4b") != std::string::npos || name.find("TT4b") != std::string::npos)
        return TColor::GetColor("#16A085");          // teal — tt+nb standout

    // ── inclusive ttbar by decay channel : BLUE family (visually grouped) ──
    if (name.find("TTTo") != std::string::npos || name.find("TTto") != std::string::npos) {
        if (name.find("Hadronic") != std::string::npos || name.find("hadronic") != std::string::npos)
            return TColor::GetColor("#2C5AA0");      // dark blue  (had)
        if (name.find("SemiLep")  != std::string::npos || name.find("semilep") != std::string::npos)
            return TColor::GetColor("#5B8FD4");      // mid blue   (SL)
        if (name.find("2L2Nu")    != std::string::npos || name.find("2l2nu") != std::string::npos)
            return TColor::GetColor("#A8CBEF");      // light blue (2l)
        return TColor::GetColor("#5B8FD4");
    }
    if (name.find("ttHH") != std::string::npos || name.find("TTHH") != std::string::npos)
        return TColor::GetColor("#FF8C00");          // orange — signal
    if (name.find("TTTT") != std::string::npos || name.find("TTTT") != std::string::npos)
        return TColor::GetColor("#34495E");          // dark slate (moved off purple)
    if (name.find("ttH")  != std::string::npos || name.find("TTH") != std::string::npos)
        return TColor::GetColor("#F1C40F");          // yellow
    if (name.find("TTZ") != std::string::npos || name.find("ttZ") != std::string::npos)
        return TColor::GetColor("#27AE60");          // green
    if (name.find("TTW") != std::string::npos || name.find("ttW") != std::string::npos)
        return TColor::GetColor("#D4A03C");          // sand
    if (name.find("WJets") != std::string::npos)
        return TColor::GetColor("#1ABC9C");
    if (name.find("DYJets") != std::string::npos || name.find("DY") != std::string::npos)
        return TColor::GetColor("#E67E22");
    if (name.find("SingleTop") != std::string::npos || name.find("ST_") != std::string::npos)
        return TColor::GetColor("#95A5A6");
    if (name.find("ZZ") != std::string::npos || name.find("WZ") != std::string::npos ||
        name.find("WW") != std::string::npos || name.find("VV") != std::string::npos)
        return TColor::GetColor("#85C1E9");
    return TColor::GetColor("#BDC3C7");
}

std::string StripCMSSuffix(const std::string& raw) {
    std::string s = raw;
    const char* suffixes[] = {"_TuneCP5", "_13TeV", "_powheg", "_pythia8",
                              "_madgraph", "_amcatnlo", "_MLM", "_FxFx",
                              "_PSweights", "_hdamp", "_ext"};
    for (const char* suf : suffixes) {
        size_t p = s.find(suf);
        if (p != std::string::npos) s = s.substr(0, p);
    }
    return s;
}

std::string ShortenLabel(const std::string& rawName) {
    std::string name = StripCMSSuffix(rawName);
    if (name.find("QCD") != std::string::npos) return "QCD";
    // dedicated heavy-flavour ttbar — keep the three ttbb channels distinct
    // (different legend entries + different colours) and tt4b on its own.
    if (name.find("ttbb") != std::string::npos || name.find("ttBB") != std::string::npos) {
        if (name.find("Hadronic") != std::string::npos) return "t#bar{t}b#bar{b} (had)";
        if (name.find("SemiLep")  != std::string::npos) return "t#bar{t}b#bar{b} (SL)";
        if (name.find("2L2Nu")    != std::string::npos) return "t#bar{t}b#bar{b} (2l)";
        return "t#bar{t}b#bar{b}";
    }
    if (name.find("tt4b") != std::string::npos || name.find("TT4b") != std::string::npos)
        return "t#bar{t}+4b";
    if (name.find("TTTo") != std::string::npos || name.find("TTto") != std::string::npos) {
        if (name.find("Hadronic") != std::string::npos || name.find("hadronic") != std::string::npos)
            return "t#bar{t} (had)";
        if (name.find("SemiLep")  != std::string::npos || name.find("semilep") != std::string::npos)
            return "t#bar{t} (SL)";
        if (name.find("2L2Nu")    != std::string::npos || name.find("2l2nu") != std::string::npos)
            return "t#bar{t} (2l)";
        return "t#bar{t}";
    }
    if (name.find("ttHH") != std::string::npos || name.find("TTHH") != std::string::npos)
        return "t#bar{t}HH";
    if (name.find("TTTT") != std::string::npos || name.find("TTTT") != std::string::npos)
        return "t#bar{t}t#bar{t}";
    if (name.find("ttH")  != std::string::npos || name.find("TTH") != std::string::npos)
        return "t#bar{t}H";
    if (name.find("TTZ")  != std::string::npos || name.find("ttZ") != std::string::npos)
        return "t#bar{t}Z";
    if (name.find("TTW")  != std::string::npos || name.find("ttW") != std::string::npos)
        return "t#bar{t}W";
    if (name.find("WJets")     != std::string::npos) return "W+jets";
    if (name.find("DYJets")    != std::string::npos) return "DY+jets";
    if (name.find("SingleTop") != std::string::npos || name.find("ST_") != std::string::npos)
        return "Single t";
    if (name.find("ZZ") != std::string::npos || name.find("WZ") != std::string::npos ||
        name.find("WW") != std::string::npos || name.find("VV") != std::string::npos)
        return "VV";
    if (name.length() > 18) return name.substr(0, 18);
    return name;
}

// ============================================================================
// 2.6  CMS-STYLE PHYSICS-GROUP MERGE  [added 2026-06, reworked STEP21 2026-07]
// ----------------------------------------------------------------------------
// Collapse the ~61 individual samples into physics groups so the stack is
// readable. One merged histogram per group → each group is ONE contiguous
// block in the stack and ONE legend row.
//
// [STEP21] 두 가지 그룹핑 모드 (env TTHH_PLOT_GROUPING 로 선택):
//   compact  (기본) : MC 10줄 — ttH+tH 통합, tt+X(rare) 통합, V+jets/VV 통합
//   detailed        : MC 13줄 — tH / tt+VV,VH / 3t,4t / V+jets / VV 분리
// TTZHTo4b/TTZZTo4b (signal 유사 4b 배경) 는 **두 모드 모두** 별도 줄.
//
// [STEP21] 매칭은 substring → **exact-name 테이블** + HT-slice prefix 규칙으로
// 교체. 기존 substring 라우팅은 (a) 충돌 순서 주석 관리가 필요했고 (b) 실제
// 오배정이 있었다: TTWW/TTWZ(tt+VV), TTWH/TTZH*(tt+VH), TTTW(3top) 가 전부
// "t#bar{t}+V" 로 묶임. 구 hadd 파일명(tt4b/ttHH/ttbb_* 등)은 alias 로 유지.
// 미배정 샘플은 Other 로 가고 stderr 경고 1회 출력 (안전망).
// ============================================================================
struct ProcessGroup { std::string key; std::string label; const char* colorHex; };

// ── grouping mode (stack_plotter() 에서 env 로 설정) ─────────────────────────
static std::string gGroupingMode = "compact";   // "compact" | "detailed"

// fine key: 물리적으로 정확한 최소 단위 (detailed 모드의 그룹과 동일)
static std::string GetFineProcessKey(const std::string& name) {
    // ── exact-name table (condor 샘플명 + 구 hadd 별칭) ──
    static const std::map<std::string, std::string> kExact = {
        // signal
        {"TTHHto4b", "ttHH"}, {"ttHH", "ttHH"},
        // ttbar inclusive (decay channel)
        {"TTbar_Hadronic", "ttbar"}, {"TTbar_SemiLep", "ttbar"}, {"TTbar_DiLep", "ttbar"},
        // tt+bb dedicated
        {"TTbb_Hadronic", "ttbb"}, {"TTbb_SemiLep", "ttbb"}, {"TTbb_DiLep", "ttbb"},
        // tt+4b
        {"TT4b", "tt4b"}, {"tt4b", "tt4b"},
        // ttH
        {"ttHTobb", "ttH"}, {"ttHToNonbb", "ttH"},
        // tH
        {"tHq", "tH"}, {"tHW", "tH"},
        // tt + single V (진짜 ttV 만)
        {"TTZToBB", "ttV"}, {"TTZToLLNuNu", "ttV"},
        {"TTWJetsToQQ", "ttV"}, {"TTWJetsToLNu", "ttV"},
        // tt + ZH/ZZ → 4b : signal 유사 배경, 두 모드 모두 별도 줄
        {"TTZHTo4b", "ttZHZZ"}, {"TTZZTo4b", "ttZHZZ"},
        // tt + VV / VH (나머지)
        {"TTWW", "ttVVVH"}, {"TTWZ", "ttVVVH"}, {"TTWH", "ttVVVH"},
        // 3-top / 4-top
        {"TTTT", "tttx"}, {"TTTW", "tttx"},
        // single top
        {"ST_t_top", "singletop"}, {"ST_t_antitop", "singletop"},
        {"ST_tW_top", "singletop"}, {"ST_tW_antitop", "singletop"},
        {"ST_s_lep", "singletop"}, {"ST_s_had", "singletop"},
        // diboson
        {"WW", "vv"}, {"WZ", "vv"}, {"ZZ", "vv"},
    };
    auto it = kExact.find(name);
    if (it != kExact.end()) return it->second;

    // ── HT-slice prefix rules (샘플군 전체가 같은 그룹) ──
    static const std::vector<std::pair<std::string, std::string>> kPrefix = {
        {"QCD_HT",            "QCD"},
        {"WJetsToLNu_HT",     "vjets"},
        {"WJetsToQQ_HT",      "vjets"},
        {"ZJetsToQQ_HT",      "vjets"},
        {"DYJetsToLL_M50_HT", "vjets"},
    };
    for (const auto& p : kPrefix)
        if (name.rfind(p.first, 0) == 0) return p.second;

    // 미배정 → Other (경고 1회)
    static std::set<std::string> warned;
    if (warned.insert(name).second)
        std::cerr << "\n[StackPlotter][warn] sample '" << name
                  << "' not in grouping table -> 'Other'" << std::endl;
    return "other";
}

// fine key → (mode 별) 그룹 정의. label/색은 여기 한 곳에서만 관리.
ProcessGroup GetProcessGroup(const std::string& name) {
    const std::string fine = GetFineProcessKey(name);

    // detailed: fine key = 그룹 (MC 13줄)
    static const std::map<std::string, ProcessGroup> kDetailed = {
        {"ttHH",     {"ttHH",     "t#bar{t}HH",           "#FF8C00"}},
        {"QCD",      {"QCD",      "QCD",                  "#C0392B"}},
        {"ttbar",    {"ttbar",    "t#bar{t}",             "#2C5AA0"}},
        {"ttbb",     {"ttbb",     "t#bar{t}b#bar{b}",     "#8E44AD"}},
        {"tt4b",     {"tt4b",     "t#bar{t}+4b",          "#16A085"}},
        {"ttH",      {"ttH",      "t#bar{t}H",            "#F1C40F"}},
        {"tH",       {"tH",       "tH",                   "#B7950B"}},
        {"ttV",      {"ttV",      "t#bar{t}+V",           "#27AE60"}},
        {"ttZHZZ",   {"ttZHZZ",   "t#bar{t}+ZH/ZZ(4b)",   "#17A2B8"}},
        {"ttVVVH",   {"ttVVVH",   "t#bar{t}+VV/VH",       "#52BE80"}},
        {"tttx",     {"tttx",     "3t/4t",                "#34495E"}},
        {"singletop",{"singletop","single t",             "#7F8C8D"}},
        {"vjets",    {"vjets",    "V+jets",               "#5DADE2"}},
        {"vv",       {"vv",       "VV",                   "#AF7AC5"}},
        {"other",    {"Other",    "Other",                "#BDC3C7"}},
    };
    // compact: 일부 fine key 를 통합 (MC 10줄; ttZHZZ 는 유지)
    static const std::map<std::string, ProcessGroup> kCompactMerge = {
        {"ttH",      {"ttHtH",    "t#bar{t}H+tH",         "#F1C40F"}},
        {"tH",       {"ttHtH",    "t#bar{t}H+tH",         "#F1C40F"}},
        {"ttVVVH",   {"rare",     "t#bar{t}+X (rare)",    "#34495E"}},
        {"tttx",     {"rare",     "t#bar{t}+X (rare)",    "#34495E"}},
        {"vjets",    {"vjetsvv",  "V+jets/VV",            "#5DADE2"}},
        {"vv",       {"vjetsvv",  "V+jets/VV",            "#5DADE2"}},
    };

    if (gGroupingMode == "compact") {
        auto m = kCompactMerge.find(fine);
        if (m != kCompactMerge.end()) return m->second;
    }
    auto d = kDetailed.find(fine);
    if (d != kDetailed.end()) return d->second;
    return kDetailed.at("other");
}

/**
 * Pretty-print an x-axis title from a hist's internal path.
 *   "CutflowKinematics/cutflow_w"        → "cut step"
 *   "CutflowKinematics/cutflow"          → "cut step"
 *   "CutflowKinematics/cutStep_8_ht"     → "H_{T} [GeV] (step 8)"
 *   "CutflowKinematics/cutStep_8_hadW"   → "had W mass [GeV] (step 8)"
 *   "CutflowKinematics/cutStep_8_higgs can01"
 *                                         → "Higgs candidate 1 mass [GeV] (step 8)"
 *   any other path                        → returned as-is
 *
 * Used only as the x-axis title; the file-name still uses clean_name so this
 * cosmetic remap does not impact saved file paths.
 */
/**
 * [STEP21] cutflow 히스토그램 판별 (경로 tail 기준).
 * analyzer 의 cutflow 계열: cutflow / cutflow_w / cutflow_w_btagSF /
 * cutflow_w_full (구명 hCutFlow*).  legend yield 와 ratio 패널 설정에 공용.
 */
bool IsCutflowHist(const std::string& path) {
    auto slash = path.find_last_of('/');
    std::string tail = (slash == std::string::npos) ? path : path.substr(slash + 1);
    return tail == "cutflow" || tail == "cutflow_w"
        || tail == "cutflow_w_btagSF" || tail == "cutflow_w_full"
        || tail == "hCutFlow" || tail == "hCutFlow_w";
}

/**
 * [STEP21] legend 에 쓸 yield.
 *   일반 hist : Integral(0, N+1)  — 그 selection 단계의 수율 (기존 동작)
 *   cutflow   : 마지막 bin(GetBinContent(N)) — 최종 cut(nTotal) 이후 수율.
 * 기존에는 cutflow 도 Integral 이라 "noCut+각 step 의 합"(noCut 지배 →
 * 사실상 cut 전 total 로 보임)이 표시됐다. 그룹 정렬도 이 yield 를 쓰므로
 * cutflow 플롯의 stack/legend 순서도 최종 수율 기준으로 정렬된다.
 */
double LegendYield(const TH1F* h, bool isCutflow) {
    if (!h) return 0.0;
    if (isCutflow) return h->GetBinContent(h->GetNbinsX());
    return h->Integral(0, h->GetNbinsX() + 1);
}

std::string PrettyAxisTitle(const std::string& path) {
    // grab the last path component
    auto slash = path.find_last_of('/');
    std::string tail = (slash == std::string::npos) ? path : path.substr(slash + 1);

    // Cutflow histograms.  In the current analyzer they live at
    //   Tree/cutflow      (← actual location: writeTree() leaves the cwd
    //   Tree/cutflow_w     at the "Tree" TDirectory in performAnalysis(),
    //                      and hCutFlow->Write() is called immediately
    //                      afterwards).
    // We treat the histograms generically by their tail name so the plotter
    // stays correct even if the analyzer relocates them later.
    if (tail == "cutflow" || tail == "cutflow_w" || tail == "hCutFlow"
            || tail == "hCutFlow_w") {
        return "cut step";
    }

    // cutStep_<N>_<var>
    if (tail.rfind("cutStep_", 0) == 0) {
        size_t p1 = tail.find('_');                    // _ after cutStep
        size_t p2 = tail.find('_', p1 + 1);            // _ after step number
        if (p1 != std::string::npos && p2 != std::string::npos) {
            std::string stepNum = tail.substr(p1 + 1, p2 - p1 - 1);
            std::string varTail = tail.substr(p2 + 1);
            std::string varPretty;
            if      (varTail == "ht")       varPretty = "H_{T} [GeV]";
            else if (varTail == "hadW")     varPretty = "had W mass [GeV]";
            else if (varTail.rfind("higgs can01", 0) == 0
                  || varTail == "higgs_can01")
                varPretty = "Higgs candidate 1 mass [GeV]";
            else if (varTail.rfind("higgs can02", 0) == 0
                  || varTail == "higgs_can02")
                varPretty = "Higgs candidate 2 mass [GeV]";
            else if (varTail.rfind("jet", 0) == 0)  varPretty = varTail;
            else                                    varPretty = varTail;
            return varPretty + " (step " + stepNum + ")";
        }
    }
    return path;
}

// ============================================================================
// 3. DRAWING LOGIC
// ============================================================================

void DrawCMSLabel(double lumi) {
    TLatex latex;
    latex.SetNDC();
    latex.SetTextFont(61); latex.SetTextSize(0.055);
    latex.DrawLatex(0.16, 0.93, "CMS");
    latex.SetTextFont(52); latex.SetTextSize(0.045);
    latex.DrawLatex(0.26, 0.93, "Preliminary");
    latex.SetTextFont(42); latex.SetTextSize(0.045); latex.SetTextAlign(31);
    latex.DrawLatex(0.96, 0.93, Form("%.1f fb^{-1} (13 TeV)", lumi));
}

TH1F* GetSummedHist(const SampleInfo& sample, const std::string& histPath) {
    TH1F* hTotal = nullptr;
    gErrorIgnoreLevel = kError;

    for (const auto& fname : sample.files) {
        TFile* f = TFile::Open(fname.c_str(), "READ");
        if (!f || f->IsZombie()) { if (f) delete f; continue; }

        TH1F* h = (TH1F*)f->Get(histPath.c_str());
        if (!h && histPath.front() == '/') h = (TH1F*)f->Get(histPath.substr(1).c_str());

        if (h) {
            if (!hTotal) {
                hTotal = (TH1F*)h->Clone();
                hTotal->SetDirectory(0);
            } else {
                hTotal->Add(h);
            }
        }
        delete f;
    }

    if (hTotal) {
        if (sample.type == "DATA") {
            hTotal->SetMarkerStyle(20); hTotal->SetMarkerSize(1.0); hTotal->SetLineColor(kBlack);
        } else {
            hTotal->SetLineColor(kBlack); hTotal->SetLineWidth(1);
        }
    }
    return hTotal;
}

// ============================================================================
// 4. MAIN ENTRY POINT
// ============================================================================

void stack_plotter() {
    std::cout << std::unitbuf;
    std::cout << "\n===========================================" << std::endl;
    std::cout << "   CMS Stack Plotter Initialized" << std::endl;
    std::cout << "===========================================\n" << std::endl;

    double LUMI = 41.48;  // Lumi for 2017 UL

    // ── [STEP21] grouping 모드 선택 ────────────────────────────────────────
    // env TTHH_PLOT_GROUPING = "compact"(기본) | "detailed"
    //   compact  : MC 10줄 (ttH+tH, tt+X rare, V+jets/VV 통합)
    //   detailed : MC 13줄 (tH, tt+VV/VH, 3t/4t, V+jets, VV 분리)
    // 두 모드 output 이 서로 덮어쓰지 않게 디렉토리를 분리한다.
    if (const char* env = std::getenv("TTHH_PLOT_GROUPING")) {
        gGroupingMode = env;
        if (gGroupingMode != "compact" && gGroupingMode != "detailed") {
            std::cerr << "[StackPlotter][FATAL] TTHH_PLOT_GROUPING='"
                      << gGroupingMode << "' (allowed: compact | detailed)"
                      << std::endl;
            gSystem->Exit(1);
        }
    }
    std::string outDir = "plots_" + gGroupingMode;
    Log("Grouping mode: " + gGroupingMode + "  (output -> " + outDir + "/)");

    // ── stack ordering policy ──────────────────────────────────────────────
    // The stack is ALWAYS grouped + ordered by yield; this flag only chooses
    // which end the largest group goes (the data/MC total & ratio are identical
    // either way — only small-component VISIBILITY on the log axis changes).
    //
    //   true  → largest group at the BOTTOM   (standard CMS convention; on a
    //           log axis a single dominant process — QCD here — fills the frame
    //           and the small groups stacked on top become invisible slivers).
    //   false → smallest group at the BOTTOM  (log-visibility ordering: each
    //           small group sits on a small cumulative base so log(1+y/C) is
    //           appreciable and it shows as its own band near the bottom).
    //
    // For a QCD-dominated (~80%) spectrum, false makes tt / tt+HF / signal
    // actually visible. Legend stays in descending-yield order regardless.
    const bool kStackLargestAtBottom = false;

    // Overlay the signal (ttHH) as a scaled LINE on top of the stack so the
    // tiny signal (here ~4 events) is visible without distorting the stack.
    // 0 → off; otherwise the multiplicative factor (e.g. 1e5).
    const double kSignalOverlayScale = 0.0;
    const std::string kSignalGroupKey = "ttHH";

    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    gROOT->SetBatch(kTRUE);

    if (gSystem->AccessPathName(outDir.c_str())) {
        gSystem->mkdir(outDir.c_str(), kTRUE);
    }

    auto samples = ParseSampleConfig("samples_config.yml");
    if (samples.empty()) return;
    auto hists = ParseStructureConfig("structure_info.yml");
    if (hists.empty()) return;

    Log("Starting Plotting Loop...");

    int total = hists.size();
    int count = 0;

    for (const auto& hInfo : hists) {
        count++;
        std::cout << "\r[Processing " << count << "/" << total << "] "
                  << std::left << std::setw(60) << hInfo.key_path << std::flush;

        THStack* hs = new THStack("hs", "");
        TH1F* hData = nullptr;
        TH1F* hMcSum = nullptr;

        // [STEP21] cutflow 여부는 loop 전체(legend yield, ratio 패널)에서 공용
        const bool isCutflow = IsCutflowHist(hInfo.key_path);

        // ── Legend (slightly taller to accommodate yields + Total MC row) ──
        TLegend* leg = new TLegend(0.42, 0.55, 0.93, 0.89);
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->SetTextSize(0.034);
        leg->SetTextFont(42);
        leg->SetNColumns(2);
        // [STEP21] cutflow 플롯의 legend 수치는 '최종 cut 이후' 수율임을 명시
        if (isCutflow) leg->SetHeader("yields after final cut");

        bool hasData = false;
        bool hasMC = false;

        // ── CMS-style physics-group merge ──────────────────────────────────
        // Sum every sample of a group into ONE histogram. Result: each group is
        // a single contiguous block in the stack and a single legend row.
        struct GroupAcc {
            TH1F* hist = nullptr;     // merged histogram (owns a clone)
            double yield = 0.0;       // integral incl. under/overflow
            std::string label;        // legend label (LaTeX)
        };
        std::map<std::string, GroupAcc> groupMap;   // group key → accumulator

        for (const auto& sample : samples) {
            TH1F* h = GetSummedHist(sample, hInfo.key_path);
            if (!h) continue;

            if (sample.type == "DATA") {
                if (!hData) { hData = (TH1F*)h->Clone("hData"); hData->SetDirectory(0); }
                else hData->Add(h);
                hasData = true; delete h;
                continue;
            }

            // ── MC: route into its physics group ──
            const ProcessGroup pg = GetProcessGroup(sample.name);
            const double thisYield = LegendYield(h, isCutflow);   // [STEP21] cutflow=최종 bin

            auto it = groupMap.find(pg.key);
            if (it == groupMap.end()) {
                TH1F* gh = (TH1F*)h->Clone(("g_" + pg.key).c_str());
                gh->SetDirectory(0);
                gh->SetFillColor(TColor::GetColor(pg.colorHex));
                gh->SetLineColor(kBlack);
                gh->SetLineWidth(1);
                groupMap[pg.key] = GroupAcc{ gh, thisYield, pg.label };
            } else {
                it->second.hist->Add(h);
                it->second.yield += thisYield;
            }

            if (!hMcSum) { hMcSum = (TH1F*)h->Clone("hMcSum"); hMcSum->SetDirectory(0); }
            else hMcSum->Add(h);
            hasMC = true;
            delete h;   // group keeps its own clone; per-sample hist no longer needed
        }

        // Order groups by TOTAL yield (descending). CMS convention: the largest
        // background sits at the BOTTOM of the stack, so we Add() largest first
        // (THStack draws the first-added histogram at the bottom).
        std::vector<GroupAcc> groupsByYield;
        groupsByYield.reserve(groupMap.size());
        for (auto& kv : groupMap) groupsByYield.push_back(kv.second);
        std::sort(groupsByYield.begin(), groupsByYield.end(),
                  [](const GroupAcc& a, const GroupAcc& b) { return a.yield > b.yield; });

        // THStack draws the first-added histogram at the BOTTOM. groupsByYield
        // is sorted descending (largest first), which we keep for the legend.
        //   largest-at-bottom → add largest first (forward iterate)
        //   smallest-at-bottom→ add smallest first (reverse iterate)
        if (kStackLargestAtBottom) {
            for (auto it = groupsByYield.begin(); it != groupsByYield.end(); ++it)
                hs->Add(it->hist);
        } else {
            for (auto it = groupsByYield.rbegin(); it != groupsByYield.rend(); ++it)
                hs->Add(it->hist);
        }

        // ── Build legend ───────────────────────────────────────────────────
        // Data first, then MC groups by descending yield (most-contributing
        // first → "who matters" is read top-down), then a Total-MC row.
        if (hasData && hData) {
            const double dataYield = LegendYield(hData, isCutflow);
            leg->AddEntry(hData,
                          Form("Data   %s", FormatYield(dataYield).c_str()),
                          "lp");
        }
        for (const auto& g : groupsByYield) {
            leg->AddEntry(g.hist,
                          Form("%s   %s", g.label.c_str(),
                                          FormatYield(g.yield).c_str()),
                          "f");
        }
        if (hasMC && hMcSum) {
            const double mcYield = LegendYield(hMcSum, isCutflow);
            leg->AddEntry((TObject*)nullptr,
                          Form("Total MC   %s",
                               FormatYield(mcYield).c_str()),
                          "");
        }

        if (!hasMC) {
            delete hs; delete leg; if(hData) delete hData; if(hMcSum) delete hMcSum;
            std::cout << " -> [Skip] No MC." << std::endl;
            continue;
        }

        double yMaxMC = (hMcSum) ? hMcSum->GetMaximum() : 0.0;
        double yMaxData = (hasData && hData) ? hData->GetMaximum() : 0.0;
        double globalMax = std::max(yMaxMC, yMaxData);

        if (globalMax <= 0.0) {
            delete hs; delete leg; if(hData) delete hData; if(hMcSum) delete hMcSum;
            for (auto& kv : groupMap) delete kv.second.hist;
            std::cout << " -> [Skip] Empty (Max=0)." << std::endl;
            continue;
        }

        // --- Draw ---
        TCanvas* c = new TCanvas("c", "", 800, 800);

        TPad* pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
        pad1->SetBottomMargin(0.02); pad1->SetTopMargin(0.08);
        pad1->SetLeftMargin(0.15); pad1->SetRightMargin(0.05);
        pad1->Draw();

        TPad* pad2 = new TPad("pad2", "pad2", 0, 0.0, 1, 0.3);
        pad2->SetTopMargin(0.02); pad2->SetBottomMargin(0.35);
        pad2->SetLeftMargin(0.15); pad2->SetRightMargin(0.05);
        pad2->Draw();

        pad1->cd();
        pad1->SetLogy(kTRUE);

        hs->SetMinimum(0.1);
        hs->SetMaximum(globalMax * 500.0);

        hs->Draw("HIST");

        if (hs->GetYaxis()) {
            hs->GetYaxis()->SetTitle("Events");
            hs->GetYaxis()->SetTitleSize(0.06); hs->GetYaxis()->SetTitleOffset(1.1); hs->GetYaxis()->SetLabelSize(0.05);
        }
        if (hs->GetXaxis()) {
            hs->GetXaxis()->SetLabelSize(0);
        }

        if (hasData) hData->Draw("SAME EP");

        // ── optional signal overlay (scaled line) ──
        // Draw the signal group (default ttHH) as a thick line scaled ×N so the
        // tiny signal is visible on top of the stack without changing it.
        TH1F* hSigOverlay = nullptr;
        if (kSignalOverlayScale > 0.0) {
            auto sit = groupMap.find(kSignalGroupKey);
            if (sit != groupMap.end() && sit->second.hist) {
                hSigOverlay = (TH1F*)sit->second.hist->Clone("hSigOverlay");
                hSigOverlay->SetDirectory(0);
                hSigOverlay->Scale(kSignalOverlayScale);
                hSigOverlay->SetFillStyle(0);
                hSigOverlay->SetLineColor(TColor::GetColor("#FF8C00"));
                hSigOverlay->SetLineWidth(3);
                hSigOverlay->Draw("HIST SAME");
                leg->AddEntry(hSigOverlay,
                              Form("%s #times%g",
                                   sit->second.label.c_str(), kSignalOverlayScale),
                              "l");
            }
        }

        leg->Draw(); DrawCMSLabel(LUMI);

        // Detect whether this is a cutflow plot — used to widen the ratio
        // range a touch (cutflows can swing more in early bins where stats
        // are dominated by a few events) and to propagate per-bin labels
        // from the histograms onto the ratio panel.
        // [STEP21] isCutflow 는 loop 상단에서 IsCutflowHist() 로 1회 계산 — 재사용.

        pad2->cd(); pad2->SetGridy();
        if (hasData && hMcSum) {
            TH1F* hRatio = (TH1F*)hData->Clone("hRatio");
            hRatio->Divide(hMcSum);

            hRatio->SetTitle(""); hRatio->SetMarkerStyle(20); hRatio->SetMarkerSize(0.8);

            hRatio->GetYaxis()->SetTitle("Data / Pred."); hRatio->GetYaxis()->SetNdivisions(505);
            hRatio->GetYaxis()->SetTitleSize(0.12); hRatio->GetYaxis()->SetTitleOffset(0.5); hRatio->GetYaxis()->SetLabelSize(0.1);

            // Cutflow: keep the wider panel; other vars: 0.0–2.0 as before.
            if (isCutflow) hRatio->GetYaxis()->SetRangeUser(0.0, 2.5);
            else           hRatio->GetYaxis()->SetRangeUser(0.0, 2.0);

            hRatio->GetXaxis()->SetTitle(PrettyAxisTitle(hInfo.key_path).c_str());
            hRatio->GetXaxis()->SetTitleSize(0.14); hRatio->GetXaxis()->SetTitleOffset(1.0); hRatio->GetXaxis()->SetLabelSize(0.10);

            // Cutflow histograms have per-bin labels assigned by the analyzer
            // (see ttHHanalyzer_unified.h::initHistograms _cutStepLabels).
            // Force-show them on the ratio panel x-axis for readability.
            if (isCutflow) {
                hRatio->GetXaxis()->LabelsOption("v");   // vertical
                hRatio->GetXaxis()->SetLabelSize(0.085);
            }

            hRatio->Draw("EP");
            TLine* line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineStyle(2); line->SetLineColor(kRed); line->Draw();
        }

        c->SaveAs(Form("%s/%s.pdf", outDir.c_str(), hInfo.clean_name.c_str()));   // [STEP21] PDF(벡터)

        delete c; delete hMcSum; if(hData) delete hData;
        if (hSigOverlay) delete hSigOverlay;
        // THStack does not own its histograms; free the per-group clones we made.
        delete hs;
        for (auto& kv : groupMap) delete kv.second.hist;
    }

    std::cout << "\n\n[Success] All plots saved to '" << outDir << "' directory." << std::endl;
}
