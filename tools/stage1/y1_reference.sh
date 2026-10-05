#!/bin/bash
# =============================================================================
#  y1_reference.sh -- Y1: analyzer outputs on four fixed 2017 files, to compare
#  before and after a code change (STEP 24; PLAN section 9.1 Stage 1 (f), Y1)
#
#    /bin/bash tools/stage1/y1_reference.sh <tag>          e.g. before | after
#
#  Runs the executable on disk (./ttHHanalyzer_unified, whatever it was built
#  from) in main mode, FH region, every SF off, weight 1, with the correction
#  paths of AnalyzerConfig/Tier3_2017_FH_unified_main.yml (jsonpog on CVMFS,
#  GoldenJson/ of this repository; the derived corrections null) except the
#  tt+nb lookup dir, which is the one of the btagtrig yml
#  (DerivedCorr/expandedTtbarId/2017: TTbar_Hadronic is in the ttbar stitching
#  set and stops with E11 without its lookup), on
#     mc       TTbar_Hadronic    2017 v20 MC
#     jetht_F  JetHT_Run2017F    Data: the 6J/HT group, 4J3T events vetoed
#     btag_F   BTagCSV_Run2017F  Data: the 4J3T group
#     jetht_B  JetHT_Run2017B    Data: the era-B path names
#  Output condor/y1/<tag>/<name>.root and <name>.log (condor/ is gitignored) and
#  condor/y1/<tag>/EXPECTED (the four names: y1_compare.py fails when one of
#  them has no .root in either directory, e.g. a run that stopped in both);
#  the CutFlow Summary of each run and the md5 of the executable are printed.
#  Run it with the current executable BEFORE the new build (tag before) and
#  again after it (tag after); then
#     python3 tools/stage1/y1_compare.py condor/y1/before condor/y1/after
#  Exit: 0 all four runs exit 0; else the first non-zero exit code of a run;
#  2 bad usage, no executable, no 2017 tt+nb lookup, or condor/y1/<tag>
#  already holds outputs.
# =============================================================================
set -u
[[ $# -eq 1 && "$1" =~ ^[A-Za-z0-9_.-]+$ ]] || { sed -n '3,28p' "$0" | sed 's/^# \{0,2\}//'; exit 2; }
TAG="$1"
REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd -P)"
cd "$REPO" || exit 2
EXE="$REPO/ttHHanalyzer_unified"
[[ -x "$EXE" ]] || { echo "ERROR no executable $EXE (build first, or run 'before' on the old build)"; exit 2; }
OUT="$REPO/condor/y1/$TAG"
if compgen -G "$OUT/*.root" > /dev/null; then
  echo "ERROR $OUT already has outputs: use another tag (they are kept for comparison)"; exit 2
fi
mkdir -p "$OUT" || exit 2
printf '%s\n' mc jetht_F btag_F jetht_B > "$OUT/EXPECTED"

S=/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH2017UL_fullNano_v20
RUNS=(
  "mc|MC|TTbar_Hadronic||$S/TTToHadronic_TuneCP5_13TeV-powheg-pythia8/TTbar_Hadronic/260622_074646/0000/slimmedNtuple_100.root"
  "jetht_F|Data|JetHT_Run2017F|F|$S/JetHT/JetHT_Run2017F/260622_074935/0000/slimmedNtuple_37.root"
  "btag_F|Data|BTagCSV_Run2017F|F|$S/BTagCSV/BTagCSV_Run2017F/260622_074946/0000/slimmedNtuple_66.root"
  "jetht_B|Data|JetHT_Run2017B|B|$S/JetHT/JetHT_Run2017B/260622_074927/0000/slimmedNtuple_4.root"
)

# the paths of AnalyzerConfig/Tier3_2017_FH_unified_main.yml; every derived correction null
export TTHH_JSONPOG_PATH=/cvmfs/cms.cern.ch/rsync/cms-nanoAOD/jsonpog-integration
export TTHH_GOLDENJSON_PATH="$REPO/GoldenJson"
export TTHH_TRIGSF_DIR=__NULL__ TTHH_BTAGRW_JSON=__NULL__ STITCH_FACTORS_JSON=__NULL__
# the tt+nb lookups (gitignored *.root, copied to KNU; docs/CHANGELOG.md): without them TTbar_Hadronic is E11
export EXPANDED_TTBARID_DIR="$REPO/DerivedCorr/expandedTtbarId/2017"
if [[ ! -s "$EXPANDED_TTBARID_DIR/ttnb_TTToHadronic.root" && ! -s "$EXPANDED_TTBARID_DIR/ttnb_TTbar_Hadronic.root" ]]; then
  echo "ERROR no tt+nb lookup for TTbar_Hadronic in $EXPANDED_TTBARID_DIR (ttnb_TTToHadronic.root): copy the 2017 lookups there first"
  exit 2
fi
export TTHH_PU_JSON=__NULL__          # [STEP 24] read only for 2024; the 2017 build before STEP 24 ignores it
set +u; source "$REPO/setup.sh" > /dev/null; set -u

echo "tag         : $TAG"
echo "executable  : $(md5sum "$EXE" | cut -d' ' -f1)  $(ls -l --time-style=+%FT%T "$EXE" | awk '{print $6}')"
echo "libraries   : $(md5sum lib/*.so 2>/dev/null | awk '{printf "%s %s  ", $1, $2}')"
echo "eventBuffer : $(md5sum include/eventBuffer.h | cut -d' ' -f1) (the header on disk; the executable may be older)"
echo "tt+nb dir   : $EXPANDED_TTBARID_DIR"
FIRST=0
for r in "${RUNS[@]}"; do
  IFS='|' read -r name dom sample era file <<< "$r"
  echo "== $name ($sample)"
  if [[ ! -s "$file" ]]; then echo "ERROR input missing or empty: $file"; [[ $FIRST -eq 0 ]] && FIRST=2; continue; fi
  echo "$file" > "$OUT/$name.filelist"
  args=(--filelist "$OUT/$name.filelist" --output "$OUT/$name.root" --weight 1 --year 2017
        --dataOrMC "$dom" --sample "$sample" --mode main --trigsf off --btagsf off --btagrw off)
  [[ -n "$era" ]] && args+=(--era "$era")
  t0=$(date +%s)
  "$EXE" "${args[@]}" > "$OUT/$name.log" 2>&1
  rc=$?
  echo "rc=$rc  wall_s=$(( $(date +%s) - t0 ))  log=condor/y1/$TAG/$name.log"
  grep -A 15 '^=== CutFlow Summary ===' "$OUT/$name.log" | sed -n '2,15p'
  if [[ $rc -ne 0 ]]; then
    echo "--- last lines of the log ---"; tail -n 15 "$OUT/$name.log"
    [[ $FIRST -eq 0 ]] && FIRST=$rc
  fi
done
echo "outputs     : condor/y1/$TAG/"
exit $FIRST
