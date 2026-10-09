#!/bin/bash

# ========================================================
# Full Trigger SF Analysis Pipeline (Config Driven)
# ========================================================
#
# [2026-07-29] STEP18 campaign(`ttHH2017UL_fullNano_v20`) 이름으로 갱신.
#   구:  TTTo2L2Nu / TTToHadronic / TTToSemiLeptonic / SingleMuon_B..F
#   신:  TTbar_DiLep / TTbar_Hadronic / TTbar_SemiLep / SingleMuon_Run2017B..F
#
#   샘플 이름은 곧 **입력 파일 이름**이다 ($TTHH_SKIM_DIR/<name>.root). 구 이름을
#   쓰면 파일을 못 열고 즉시 exit(1) 하므로 조용히 틀릴 여지는 없지만, 어차피
#   registry 조회도 campaign 이름 기준이라 여기서 통일한다.
#
# [STEP 26 L, 2026-10-09] bash run_analysis.sh [--year 2017|2024] [--workdir DIR] [--jobs N]
#   --year (default: env TTHH_YEAR, else 2017 = the behaviour before; exported as TTHH_YEAR for the executable and
#   the macros, docs/changes/STEP_26_trigger_sf_2024.md):
#     2017: reference HLT_IsoMu27; Data SingleMuon_Run2017B..F; the outputs next to the code (TriggerStudy/), as before.
#     2024: reference HLT_IsoMu24, the PNet path set (Config.hh Year()); Data = the Muon0/1 samples of the 2024 xsec_db
#           (16, eras C-I); TTHH_SKIM_DIR is required (the merged btagtrig output, AnalyzerOutput_btagtrig_notrig_2024);
#           the xsec_db / prescan default to the 2024 files; the outputs go to TriggerStudy/run_2024/.
#     MC: TTbar_{DiLep,Hadronic,SemiLep} both years.
#   --workdir: where the outputs go. --jobs N: the event loops in batches of N (default 0 = all samples at once, as
#   before; 2024 has 19).
#   Every year: the executable and the macros are called by absolute path from the work directory; the paths of the
#   environment are made absolute first; the ROOT macros get ROOT_INCLUDE_PATH (Config.hh includes
#   ../include/SampleRegistry.hh and SelectionCuts.h since 2026-07-29, which the interpreter did not find from
#   TriggerStudy/); one log per process (log_<mode>_<sample>.txt) with its [TrigStudy] line; each macro must exit 0
#   with RESULT OK as its last RESULT line; the event loops refuse inputs of another year (EventLooper: the
#   reference branch, the xsec_db era, the prescan's year, the Data names, the JSON of Step 2) and DeriveSF /
#   PlotTriggerEfficiency refuse Step 1 / Step 2 outputs of another year (TrigStudyStamp.hh). A DeriveSF that FAILS
#   after deriving keeps its JSON as trigger_sf.json.gz.FAILED and an earlier one as trigger_sf.json.gz.previous.
#
# 필요한 환경변수:
#   TTHH_SKIM_DIR   btagtrig skim 이 있는 디렉토리 (2017: 미설정 시 Config.hh 기본값; 2024: 필수)
#   TTHH_BASE       analyzer 프로젝트 루트 (default: 이 스크립트의 상위 디렉토리)
#   TTHH_XSEC_DB / TTHH_PRESCAN   (default: 2017 은 SampleRegistry 기본값, 2024 는 data/samples_2024.json /
#                                  prescan_summary_2024/prescan_summary.json; set values are used as they are:
#                                  the event loop stops on an xsec_db of another year)
# ========================================================

set -euo pipefail

YEAR="${TTHH_YEAR:-2017}"
WORKDIR=""
NJOBS=0
usage() { echo "usage: $0 [--year 2017|2024] [--workdir DIR] [--jobs N]"; }
while [[ $# -gt 0 ]]; do
    case "$1" in
        --year)    YEAR="${2:?--year needs a value}"; shift 2 ;;
        --workdir) WORKDIR="${2:?--workdir needs a value}"; shift 2 ;;
        --jobs)    NJOBS="${2:?--jobs needs a value}"; shift 2 ;;
        -h|--help) usage; exit 0 ;;
        *) echo "[FATAL] unknown argument: $1" >&2; usage >&2; exit 2 ;;
    esac
done
case "$YEAR" in
    2017|2024) ;;
    *) echo "[FATAL] --year $YEAR: only 2017 and 2024 are implemented (TriggerStudy Config.hh Year())" >&2; exit 2 ;;
esac
[[ "$NJOBS" =~ ^[0-9]+$ ]] || { echo "[FATAL] --jobs $NJOBS: not a number" >&2; exit 2; }
NJOBS=$((10#$NJOBS))
export TTHH_YEAR="$YEAR"

for tool in root hadd python3; do
    command -v "$tool" > /dev/null || { echo "[FATAL] '$tool' not in PATH (cmsenv?)" >&2; exit 1; }
done

abspath() { case "$1" in /*) printf '%s\n' "$1" ;; *) printf '%s\n' "$PWD/$1" ;; esac; }

TS="$(cd "$(dirname "${BASH_SOURCE[0]}")" > /dev/null && pwd)"   # TriggerStudy/
export TTHH_BASE="$(abspath "${TTHH_BASE:-$TS/..}")"            # the analyzer (SampleRegistry, SelectionCuts)
EXE="$TS/exe_TrigStudy"
# the macros include include/Config.hh -> ../include/SampleRegistry.hh, SelectionCuts.h, <nlohmann/json.hpp>
export ROOT_INCLUDE_PATH="$TS:$TS/include:$TTHH_BASE/include${ROOT_INCLUDE_PATH:+:$ROOT_INCLUDE_PATH}"

outputDirName="outputs"

# ── 샘플 목록 (여기 한 곳만 고치면 전 단계에 반영된다) ──────────────────────
MC_SAMPLES=(TTbar_DiLep TTbar_Hadronic TTbar_SemiLep)
if [[ "$YEAR" == 2024 ]]; then
    : "${TTHH_SKIM_DIR:?[FATAL] 2024: set TTHH_SKIM_DIR to the merged btagtrig output (.../AnalyzerOutput_btagtrig_notrig_2024)}"
    export TTHH_XSEC_DB="${TTHH_XSEC_DB:-$TTHH_BASE/data/samples_2024.json}"
    export TTHH_PRESCAN="${TTHH_PRESCAN:-$TTHH_BASE/prescan_summary_2024/prescan_summary.json}"
    WORKDIR="${WORKDIR:-$TS/run_2024}"
    # Data: every Muon0/1 sample of the 2024 xsec_db (the btagtrig yml has the same 16)
    DATA_LIST="$(python3 -I -c 'import json, sys
d = json.load(open(sys.argv[1]))
print("\n".join(sorted(k for k in d if k.startswith(("Muon0_Run2024", "Muon1_Run2024")))))' "$TTHH_XSEC_DB")" \
        || { echo "[FATAL] cannot read the Data samples from $TTHH_XSEC_DB" >&2; exit 1; }
    DATA_SAMPLES=()
    while IFS= read -r s; do [[ -n "$s" ]] && DATA_SAMPLES+=("$s"); done <<< "$DATA_LIST"
    if [[ ${#DATA_SAMPLES[@]} -eq 0 ]]; then
        echo "[FATAL] no Muon0/1 2024 sample in $TTHH_XSEC_DB" >&2; exit 1
    fi
else
    DATA_SAMPLES=(SingleMuon_Run2017B SingleMuon_Run2017C SingleMuon_Run2017D \
                  SingleMuon_Run2017E SingleMuon_Run2017F)
    WORKDIR="${WORKDIR:-$TS}"        # 2017: the outputs next to the code, as before
fi
# absolute before the cd below (the executable and the macros run in WORKDIR)
[[ -n "${TTHH_SKIM_DIR:-}" ]] && export TTHH_SKIM_DIR="$(abspath "$TTHH_SKIM_DIR")"
[[ -n "${TTHH_XSEC_DB:-}" ]]  && export TTHH_XSEC_DB="$(abspath "$TTHH_XSEC_DB")"
[[ -n "${TTHH_PRESCAN:-}" ]]  && export TTHH_PRESCAN="$(abspath "$TTHH_PRESCAN")"
WORKDIR="$(abspath "$WORKDIR")"

# ========================================================
# Helper: 병렬 실행 후 모든 프로세스의 종료 코드 검증
# ========================================================
wait_and_check() {
    local pids=("$@")
    local failed=0
    for pid in "${pids[@]}"; do
        if ! wait "$pid"; then
            echo "[ERROR] PID $pid failed" >&2
            failed=1
        fi
    done
    if [[ $failed -ne 0 ]]; then
        echo "[FATAL] One or more processes failed. Aborting pipeline (see log_*.txt in $PWD)." >&2
        exit 1
    fi
}

# "<prefix><sample>.root" 목록을 stdout 으로 (hadd 인자용)
files_for() {
    local prefix="$1"; shift
    local s
    for s in "$@"; do printf '%s%s.root ' "$prefix" "$s"; done
}

# mode(0=eff, 1=applySF) 로 전 샘플을 병렬 실행 (--jobs N: N 개씩); one log per process, then its [TrigStudy] line
run_all_samples() {
    local mode="$1"
    local pids=()
    local s
    for s in "${MC_SAMPLES[@]}" "${DATA_SAMPLES[@]}"; do
        "$EXE" "$s" "$mode" > "log_${mode}_${s}.txt" 2>&1 & pids+=($!)
        if [[ "$NJOBS" -gt 0 && ${#pids[@]} -ge "$NJOBS" ]]; then
            wait_and_check "${pids[@]}"; pids=()
        fi
    done
    [[ ${#pids[@]} -eq 0 ]] || wait_and_check "${pids[@]}"
    for s in "${MC_SAMPLES[@]}" "${DATA_SAMPLES[@]}"; do
        grep -h '^\[TrigStudy\]' "log_${mode}_${s}.txt" || { echo "[FATAL] no [TrigStudy] line in log_${mode}_${s}.txt" >&2; exit 1; }
    done
}

# a ROOT macro by absolute path: it must exit 0 and its LAST RESULT line must be RESULT OK
run_macro() {
    local name="$1" rc=0 last
    root -l -b -q "$TS/${name}.cpp" > "${name}.log" 2>&1 || rc=$?
    cat "${name}.log"
    last="$(grep '^RESULT' "${name}.log" | tail -n 1 || true)"
    if [[ $rc -ne 0 || "$last" != "RESULT OK"* ]]; then
        echo "[FATAL] ${name}: exit $rc, last RESULT line '${last:-none}' (${PWD}/${name}.log)" >&2
        exit 1
    fi
}

# ========================================================
# 0. Preflight — 조용한 실패의 대부분이 여기서 걸린다
# ========================================================
if [[ ! -x "$EXE" ]]; then
    echo "[FATAL] $EXE not found. Run 'make' in $TS first." >&2
    exit 1
fi
SKIM="${TTHH_SKIM_DIR:-}"
if [[ -n "$SKIM" ]]; then
    missing=0
    for s in "${MC_SAMPLES[@]}" "${DATA_SAMPLES[@]}"; do
        [[ -f "$SKIM/$s.root" ]] || { echo "[FATAL] input missing: $SKIM/$s.root" >&2; missing=1; }
    done
    [[ $missing -eq 0 ]] || exit 1
fi
mkdir -p "$WORKDIR"
cd "$WORKDIR"
echo ">>> [run_analysis] year $YEAR, work dir $PWD"
echo ">>> [run_analysis] skim dir ${SKIM:-(Config.hh default)}"
echo ">>> [run_analysis] xsec_db ${TTHH_XSEC_DB:-(SampleRegistry default: data/samples_2017UL.json)}, prescan ${TTHH_PRESCAN:-(SampleRegistry default: prescan_summary/prescan_summary.json)}"
echo ">>> [run_analysis] MC (${#MC_SAMPLES[@]}): ${MC_SAMPLES[*]}"
echo ">>> [run_analysis] Data (${#DATA_SAMPLES[@]}): ${DATA_SAMPLES[*]}"
echo ">>> [run_analysis] event loops at a time: $([[ $NJOBS -gt 0 ]] && echo "$NJOBS" || echo "all")"

# ========================================================
# 1. Step 1: Calculate Efficiency (Mode 0)
# ========================================================
echo ">>> [Step 1] Running EventLooper..."
run_all_samples 0
echo ">>> [Step 1] All EventLooper jobs succeeded."

mkdir -p "$outputDirName"

echo ">>> [Step 1] Merging Files..."
hadd -f output_SingleMuon.root $(files_for output_ "${DATA_SAMPLES[@]}")
hadd -f output_TTbarInc.root   $(files_for output_ "${MC_SAMPLES[@]}")

# ========================================================
# 2. Derive SF Histograms
# ========================================================
echo ">>> [Derive SF] Calculating Scale Factors..."
run_macro DeriveSF

# ========================================================
# 3. Step 2: Apply SF (Mode 1)
# ========================================================
echo ">>> [Step 2] Applying Corrections..."
run_all_samples 1
echo ">>> [Step 2] All validation jobs succeeded."

echo ">>> [Step 2] Merging Corrected Files..."
hadd -f validated_SingleMuon.root $(files_for validated_ "${DATA_SAMPLES[@]}")
hadd -f validated_TTbarInc.root   $(files_for validated_ "${MC_SAMPLES[@]}")

# ========================================================
# 4. Plotting
# ========================================================
echo ">>> [Plotting] Generating Validation Plots..."
run_macro PlotTriggerEfficiency

echo ">>> [run_analysis] outputs in $PWD: trigger_sf.json.gz ($(wc -c < trigger_sf.json.gz 2>/dev/null || echo '?') bytes), TriggerSF.root, $(ls -1 *.pdf 2>/dev/null | wc -l) PDF"
echo ">>> All Analysis Steps Completed!"
echo "RESULT OK (run_analysis $YEAR)"
