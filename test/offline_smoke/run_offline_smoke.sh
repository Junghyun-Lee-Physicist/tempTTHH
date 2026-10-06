#!/bin/bash
# =============================================================================
#  run_offline_smoke.sh -- the analyzer on synthetic files with FAKE payloads:
#  no CVMFS, no /pnfs, no real ntuple needed (STEP 24)
#
#    /bin/bash test/offline_smoke/run_offline_smoke.sh <built repo> [<older built repo>] [--keep]
#
#  <built repo>: a tempTTHH checkout with ttHHanalyzer_unified built (its
#  include/eventBuffer.h must have the 2017 v9 and 2024 v15 branches, as the
#  STEP 24 header has). With <older built repo> the four 2017 runs are also made
#  with that executable and compared (tools/stage1/y1_compare.py): the
#  container version of the KNU Y1 check.
#  Made in a fresh work directory (printed; removed unless --keep):
#    fake jsonpog tree (fake_pog.py: the real correction NAMES and INPUTS,
#    invented values), synthetic files (synth_nano.py), a PU JSON from the
#    synthetic 2024 MC through tools/stage2/pu_weights.py, a fake 2017 tt+nb
#    lookup for TTbar_Hadronic.
#  Checks (CHECK <name> PASS|FAIL): 2024 MC signal / ttbar without a tt+nb
#  lookup / Data C / Data I run (exit 0, the required-branch line, a non-empty
#  cutflow); the payload names in the log (jet ID, veto map, PU, JEC/JER, the
#  UParTAK4 WPs; the Data JEC for Data) and which 4J3T path a Data file has;
#  the 2024 guards stop with their exit code (--btagsf on, no
#  TTHH_PU_JSON, an unknown PD, no 4J3T branch, a missing jet branch, no
#  HLT_IsoMu24, a prescan without genWeight, a two-file job whose files differ
#  in their branches, [2026-10-06] a job with an unreadable second file (E30),
#  an MC prescan of a file without a Runs tree (E30), a filelist that cannot be
#  opened (exit 1)); [2026-10-06] a stop keeps its exit code when an exit-time
#  handler crashes (crash_teardown.so, with a control: the KNU 139 of smoke_2024
#  dC_mix); a Data file without the DeepJet 4J3T branch runs,
#  and so does a two-file job with one branch set (the [inputs] line: Events
#  entries = the chain's); 2017 runs (and matches the older
#  build); tools/stage3/data_lumi_check.py: missing LS, eras not produced,
#  split LS inside one sample (fine), an LS in two samples (duplicate, exit 1).
#  Exit: 0 all checks pass; 1 a check failed; 2 bad usage.
#  The values are random: the checks are about running and stopping, never physics.
# =============================================================================
set -u
KEEP=0; ARGS=()
for x in "$@"; do [[ "$x" == "--keep" ]] && KEEP=1 || ARGS+=("$x"); done
[[ ${#ARGS[@]} -ge 1 && ${#ARGS[@]} -le 2 ]] || { sed -n '3,35p' "$0" | sed 's/^# \{0,2\}//'; exit 2; }
NEW="$(cd "${ARGS[0]}" && pwd -P)" || exit 2
OLD=""; [[ ${#ARGS[@]} -eq 2 ]] && { OLD="$(cd "${ARGS[1]}" && pwd -P)" || exit 2; }
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
REPO="$(cd "$HERE/../.." && pwd -P)"
for b in "$NEW" $OLD; do [[ -x "$b/ttHHanalyzer_unified" ]] || { echo "ERROR no executable $b/ttHHanalyzer_unified"; exit 2; }; done
W="$(mktemp -d "${TMPDIR:-/tmp}/tthh_offline_smoke.XXXXXX")" || exit 2
echo "WORK $W"
[[ $KEEP -eq 1 ]] || trap 'rm -rf "$W"' EXIT
NPASS=0; NFAIL=0
check() { if [[ "$2" == 1 ]]; then echo "CHECK $1 PASS"; NPASS=$((NPASS+1)); else echo "CHECK $1 FAIL ${3:-}"; NFAIL=$((NFAIL+1)); fi; }

CLIB="$(python3 -c 'import correctionlib,os;print(os.path.join(os.path.dirname(correctionlib.__file__),"lib"))' 2>/dev/null)"
RLIB="$(root-config --libdir 2>/dev/null)"
G17="$REPO/GoldenJson/2017_UL/Cert_294927-306462_13TeV_UL2017_Collisions17_GoldenJSON.txt"
G24="$REPO/GoldenJson/2024_Summer24/Cert_Collisions2024_378981_386951_Golden.json"
IN="$W/in"; mkdir -p "$IN"

# ---- inputs -------------------------------------------------------------------
python3 "$HERE/fake_pog.py" "$W/fakepog" > "$W/fake_pog.log" 2>&1 || { echo "ERROR fake_pog.py"; tail "$W/fake_pog.log"; exit 1; }
S() { python3 "$HERE/synth_nano.py" "$@" >> "$W/synth.log" 2>&1 || { echo "ERROR synth_nano.py $*"; tail -5 "$W/synth.log"; exit 1; }; }
S --year 2017 --kind mc --n 600 --seed 11 -o "$IN/mc17.root"
S --year 2017 --kind data --era F --golden "$G17" --run-range 305040 306460 --n 600 --seed 12 -o "$IN/jetht17F.root"
S --year 2017 --kind data --era F --golden "$G17" --run-range 305040 306460 --n 600 --seed 13 -o "$IN/btag17F.root"
S --year 2017 --kind data --era B --golden "$G17" --run-range 297046 299329 --n 600 --seed 14 -o "$IN/jetht17B.root"
S --year 2024 --kind mc --n 600 --seed 21 -o "$IN/mc24.root"
S --year 2024 --kind data --golden "$G24" --run-range 379415 380238 --n 600 --seed 22 -o "$IN/jetmet24C.root"
S --year 2024 --kind data --golden "$G24" --run-range 386071 386951 --n 600 --seed 23 -o "$IN/jetmet24I.root"
S --year 2024 --kind data --golden "$G24" --run-range 386071 386951 --n 200 --seed 31 \
  --drop HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5 -o "$IN/noDeepJet.root"
S --year 2024 --kind data --golden "$G24" --run-range 386071 386951 --n 200 --seed 32 \
  --drop HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5 HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3 -o "$IN/no4J3T.root"
S --year 2024 --kind mc --n 200 --seed 33 --drop Jet_chHEF -o "$IN/noChHEF.root"
S --year 2024 --kind mc --n 200 --seed 34 --drop HLT_IsoMu24 -o "$IN/noIsoMu24.root"
S --year 2024 --kind mc --n 200 --seed 35 --drop genWeight -o "$IN/noGenWeight.root"
DQM="$REPO/DerivedCorr/PU/2024_Summer24/input"
python3 "$REPO/tools/stage2/pu_weights.py" mcprofile --glob "$IN/mc24.root" \
  --binning-from "$DQM/dataPileupHistogram-2024CDEFGHI_Golden-69200ub.root" > "$W/pu_mcprofile.log" 2>&1
python3 "$REPO/tools/stage2/pu_weights.py" weights --data-dir "$DQM" --mc-record "$W/pu_mcprofile.log" \
  --out "$W/puWeights_2024_FAKE.json" > "$W/pu_weights.log" 2>&1
check "pu_weights.py on the synthetic 2024 MC (mcprofile + weights)" "$(grep -q '^RESULT OK' "$W/pu_weights.log" && echo 1)"
mkdir -p "$W/ttnb2017"
python3 - "$IN/mc17.root" "$W/ttnb2017/ttnb_TTToHadronic.root" > "$W/ttnb.log" 2>&1 <<'PY'
import sys, numpy as np, ROOT
ROOT.gROOT.SetBatch(True)
f = ROOT.TFile.Open(sys.argv[1]); t = f.Get("Events"); rows = []
for i in range(t.GetEntries()):
    t.GetEntry(i)
    if t.genTtbarId in (53, 54, 55) and len(rows) < 40:
        rows.append((int(t.run), int(t.luminosityBlock), int(t.event), int(t.genTtbarId)))
out = ROOT.TFile(sys.argv[2], "RECREATE"); tr = ROOT.TTree("TtNb", "FAKE tt+nb lookup (offline smoke)")
b = {n: np.zeros(1, d) for n, d in (("run", np.uint32), ("luminosityBlock", np.uint32), ("event", np.uint64), ("genTtbarId", np.int32),
                                     ("Expanded_genTtbarId", np.int32), ("nAddBJets", np.int32), ("nAddBJetsMulti", np.int32))}
codes = {"run": "i", "luminosityBlock": "i", "event": "l"}
for n, a in b.items():
    tr.Branch(n, a, "%s/%s" % (n, codes.get(n, "I")))
for k, (r, l, e, g) in enumerate(rows):
    b["run"][0], b["luminosityBlock"][0], b["event"][0], b["genTtbarId"][0] = r, l, e, g
    b["Expanded_genTtbarId"][0] = [61, 62, 71, 72][k % 4]; b["nAddBJets"][0] = 3; b["nAddBJetsMulti"][0] = 3
    tr.Fill()
tr.Write(); out.Close(); print("rows", len(rows))
PY

# ---- one analyzer run: name repo dom sample era year file [ENV=VAL ...] --------
runa() {
  local name="$1" repo="$2" dom="$3" sample="$4" era="$5" year="$6" file="$7"; shift 7
  local out="$W/out/${RTAG:-new}_$name"; mkdir -p "$(dirname "$out")"
  tr ',' '\n' <<< "$file" > "$out.filelist"          # file may be a,b,c (one job, several files)
  local args=(--filelist "$out.filelist" --output "$out.root" --weight 1 --year "$year" --dataOrMC "$dom"
              --sample "$sample" --mode "${MODE:-main}" --trigsf off --btagsf "${BTAGSF:-off}" --btagrw off)
  [[ -n "$era" ]] && args+=(--era "$era")
  ( cd "$repo" &&
    env LD_LIBRARY_PATH="$repo/lib:${RLIB}:${CLIB}:${LD_LIBRARY_PATH:-}" TNM_PATH="$repo" \
        TTHH_JSONPOG_PATH="$W/fakepog" TTHH_GOLDENJSON_PATH="$REPO/GoldenJson" TTHH_TRIGSF_DIR=__NULL__ \
        TTHH_BTAGRW_JSON=__NULL__ STITCH_FACTORS_JSON=__NULL__ EXPANDED_TTBARID_DIR=__NULL__ \
        TTHH_PU_JSON="$W/puWeights_2024_FAKE.json" "$@" \
        "$repo/ttHHanalyzer_unified" "${args[@]}" ) > "$out.log" 2>&1
  echo $? > "$out.rc"
  LAST="$out"
}
ok_run() {   # name: exit 0, the required-branch line, a cutflow with a last step > 0
  local rc; rc=$(cat "$LAST.rc")
  local last; last=$(awk '/^=== CutFlow Summary ===/{f=1;next} f&&/ : /{v=$0} f&&!/ : /{f=0} END{print v}' "$LAST.log" | awk -F' : ' '{print $2}' | awk '{print $1}')
  check "$1" "$([[ $rc == 0 ]] && grep -q '^\[branches\].*present' "$LAST.log" && awk -v x="${last:-0}" 'BEGIN{exit !(x>0)}' && echo 1)" \
        "(exit $rc, last cutflow step '${last:-none}', log $LAST.log)"
}
stops() {    # name exit pattern
  local rc; rc=$(cat "$LAST.rc")
  check "$1" "$([[ $rc == "$2" ]] && grep -q -- "$3" "$LAST.log" && echo 1)" "(exit $rc, want $2 and '$3')"
}

# ---- 2024 ---------------------------------------------------------------------
runa sig24 "$NEW" MC TTHHto4b "" 2024 "$IN/mc24.root"; ok_run "2024 MC signal runs"
runa tt24 "$NEW" MC TTbar_Hadronic "" 2024 "$IN/mc24.root"; ok_run "2024 TTbar_Hadronic runs without a tt+nb lookup (D-2026-10-05-C)"
check "2024 TTbar_Hadronic says the tt+nb split is off" "$(grep -q 'NOT split from 51-55' "$LAST.log" && echo 1)"
check "2024 MC prints the jet ID, veto map and PU payloads" \
      "$(grep -q 'jet ID (2024_Summer24)' "$LAST.log" && grep -q 'jet veto map (2024_Summer24)' "$LAST.log" && grep -q 'PU weights (2024)' "$LAST.log" && echo 1)"
check "2024 MC prints the cleaning counts" "$(grep -q '^\[cleaning\] events .*MET filters fail' "$LAST.log" && echo 1)"
check "2024 MC prints the JEC/JER payload names and the UParTAK4 WPs" \
      "$(grep -q 'JEC/JER (2024_Summer24): .* -> JEC Summer24Prompt24_V1_MC_L1L2L3Res_AK4PFPuppi, JER Summer23BPixPrompt23_RunD_JRV1_MC_PtResolution_AK4PFPuppi + Summer23BPixPrompt23_RunD_JRV1_MC_ScaleFactor_AK4PFPuppi' "$LAST.log" && grep -q '^\[objectJet\] UParTAK4 (Jet_btagUParTAK4B) WP for 2024 : L=0.0246 M=0.1272 T=0.4648' "$LAST.log" && echo 1)"
runa d24C "$NEW" Data JetMET0_Run2024C-MINIv6NANOv15-v1 C 2024 "$IN/jetmet24C.root"; ok_run "2024 Data era C runs"
check "2024 Data uses the Data JEC" "$(grep -q -- '-> JEC Summer24Prompt24_V1_DATA_L1L2L3Res_AK4PFPuppi,' "$LAST.log" && echo 1)"
runa d24I "$NEW" Data JetMET1_Run2024I-MINIv6NANOv15_v2-v2 I 2024 "$IN/jetmet24I.root"; ok_run "2024 Data era I runs"
runa noDJ "$NEW" Data JetMET0_Run2024I-MINIv6NANOv15-v2 I 2024 "$IN/noDeepJet.root"; ok_run "2024 Data without the DeepJet 4J3T branch runs"
check "2024 Data says which 4J3T path the file has" \
      "$(grep -q 'one of {HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3: yes, HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5: no}' "$LAST.log" && echo 1)"
BTAGSF=on runa btag "$NEW" MC TTHHto4b "" 2024 "$IN/mc24.root"; stops "2024 --btagsf on stops (E11)" 11 "btagsf on for runYear=2024"
runa nopu "$NEW" MC TTHHto4b "" 2024 "$IN/mc24.root" TTHH_PU_JSON=; stops "2024 without TTHH_PU_JSON stops (E12)" 12 "TTHH_PU_JSON"
runa pd "$NEW" Data EGamma0_Run2024C-MINIv6NANOv15-v1 C 2024 "$IN/jetmet24C.root"; stops "2024 unknown Data PD stops (E11)" 11 "PD not recognised"
runa no4j "$NEW" Data JetMET0_Run2024I-MINIv6NANOv15-v2 I 2024 "$IN/no4J3T.root"; stops "2024 Data without any 4J3T branch stops (E11)" 11 "one of: HLT_PFHT330PT30"
runa nochf "$NEW" MC TTHHto4b "" 2024 "$IN/noChHEF.root"; stops "2024 MC without Jet_chHEF stops (E11)" 11 "Jet_chHEF   (not in the input file)"
runa noiso "$NEW" MC TTHHto4b "" 2024 "$IN/noIsoMu24.root"; stops "2024 MC without HLT_IsoMu24 stops (E11)" 11 "HLT_IsoMu24   (not in the input file)"
MODE=prescan runa nogw "$NEW" MC TTHHto4b "" 2024 "$IN/noGenWeight.root"; stops "2024 prescan without genWeight stops (E11)" 11 "genWeight   (not in the input file)"
MODE=prescan runa pre24 "$NEW" MC TTHHto4b "" 2024 "$IN/mc24.root"
check "2024 prescan runs" "$([[ $(cat "$LAST.rc") == 0 ]] && grep -q '(prescan): all 5 required branches present' "$LAST.log" && echo 1)" "(exit $(cat "$LAST.rc"))"
runa mix "$NEW" Data JetMET0_Run2024I-MINIv6NANOv15-v2 I 2024 "$IN/jetmet24I.root,$IN/noDeepJet.root"
stops "2024 job of two files with different HLT branches stops (E11)" 11 "do not have the same branches"
runa same "$NEW" Data JetMET0_Run2024C-MINIv6NANOv15-v1 C 2024 "$IN/jetmet24C.root,$IN/jetmet24I.root"; ok_run "2024 job of two files with the same branches runs"
check "2024 two-file job says the branch sets agree" "$(grep -q '^\[branches\] 2 input files, the same' "$LAST.log" && echo 1)"
# [2026-10-06] input completeness: ROOT's TChain skips a file it cannot open with only an error line
check "2024 two-file job: Events entries of the files = the chain's ([inputs])" \
      "$(grep -q '^\[inputs\] 2 input files, Events entries [0-9]* = the chain' "$LAST.log" && echo 1)"
check "2024 one-file job counts its Events entries too ([inputs])" \
      "$(grep -q '^\[inputs\] 1 input file, Events entries [0-9]* = the chain' "$W/out/new_sig24.log" && echo 1)"
printf 'this is not a ROOT file' > "$IN/broken.root"
runa unread "$NEW" MC TTHHto4b "" 2024 "$IN/mc24.root,$IN/broken.root"
stops "2024 job with an unreadable second file stops (E30), not exit 0 on fewer events" 30 "cannot be read"
python3 - "$IN/mc24.root" "$IN/noRuns.root" >> "$W/synth.log" 2>&1 <<'PY'
import sys, ROOT
f = ROOT.TFile.Open(sys.argv[1]); t = f.Get("Events")
o = ROOT.TFile(sys.argv[2], "RECREATE"); c = t.CloneTree(-1, "fast"); c.Write(); o.Close()
PY
MODE=prescan runa noruns "$NEW" MC TTHHto4b "" 2024 "$IN/noRuns.root"
stops "2024 MC prescan of a file without a Runs tree stops (E30), not a smaller Sigma genw" 30 "no Runs tree"

# [2026-10-06] a stop keeps its exit code when the exit-time teardown crashes. crash_teardown.so, loaded before
#   main (LD_PRELOAD), raises SIGSEGV in an exit-time handler when the exit status is non-zero: what ROOT 6.30's
#   end-of-process cleanup did at KNU after std::exit (smoke_2024 dC_mix ended with 139, not 11). The analyzer
#   ends its fatal paths with tthh::fatalExit (no exit-time handlers at all); any other exit() goes through the
#   on_exit handler of main(), which runs before the handlers registered before it (this library's among them).
#   Tested here: two fatal paths, and tnm.cc's argument check for the on_exit path (eventBuffer::read's exit 1
#   in the event loop is not made to happen here). A control first: a plain `return 3` under the library -> 139.
cat > "$W/crash_teardown.c" <<'C'
#define _GNU_SOURCE
#include <signal.h>
#include <stdlib.h>
static void crash(int status, void* arg) { (void)arg; if (status != 0) raise(SIGSEGV); }
__attribute__((constructor)) static void reg(void) { on_exit(crash, 0); }
C
printf 'int main(void) { return 3; }\n' > "$W/ct_control.c"
rawrun() {   # name repo [ENV=VAL ...] -- args: the executable with exactly these arguments (no runa defaults)
  local name="$1" repo="$2"; shift 2
  local envs=(); while [[ $# -gt 0 && "$1" != "--" ]]; do envs+=("$1"); shift; done; shift
  { ( ulimit -c 0; cd "$repo" && env LD_LIBRARY_PATH="$repo/lib:${RLIB}:${CLIB}:${LD_LIBRARY_PATH:-}" TNM_PATH="$repo" \
        ${envs[@]+"${envs[@]}"} "$repo/ttHHanalyzer_unified" "$@" ) > "$W/out/$name.log" 2>&1; } 2>/dev/null
  echo $? > "$W/out/$name.rc"; LAST="$W/out/$name"
}
if ${CC:-gcc} -shared -fPIC -o "$W/crash_teardown.so" "$W/crash_teardown.c" > "$W/crash_teardown.log" 2>&1 &&
   ${CC:-gcc} -o "$W/ct_control" "$W/ct_control.c" >> "$W/crash_teardown.log" 2>&1; then
  CT="LD_PRELOAD=$W/crash_teardown.so"
  { ( ulimit -c 0; env "$CT" "$W/ct_control" ); } 2>/dev/null; rc=$?
  check "crash_teardown.so works: a plain 'return 3' under it ends with 139 (control)" "$([[ $rc == 139 ]] && echo 1)" "(exit $rc)"
  BTAGSF=on runa ctfatal "$NEW" MC TTHHto4b "" 2024 "$IN/mc24.root" "$CT"
  stops "a stop keeps its exit code when the exit-time teardown crashes (E11 --btagsf on; not 139)" 11 "btagsf on for runYear=2024"
  runa ctmix "$NEW" Data JetMET0_Run2024I-MINIv6NANOv15-v2 I 2024 "$IN/jetmet24I.root,$IN/noDeepJet.root" "$CT"
  stops "the same for the two-file E11 (smoke_2024 dC_mix at KNU)" 11 "do not have the same branches"
  ARGS_NOSAMPLE=(--filelist "$W/out/new_sig24.filelist" --output "$W/out/ctarg.root" --weight 1 --year 2024 --dataOrMC MC --mode main)
  rawrun new_ctarg "$NEW" "$CT" -- "${ARGS_NOSAMPLE[@]}"
  stops "an exit() outside them too (tnm.cc, no --sample: exit 1 through the on_exit handler of main())" 1 "Missing mandatory arguments"
  if [[ -n "$OLD" ]]; then
    rawrun old_ctarg "$OLD" "$CT" -- "${ARGS_NOSAMPLE[@]}"
    echo "NOTE the older build ends with $(cat "$LAST.rc") there (139 = the crash in the teardown reached the exit code)"
  fi
else
  check "crash_teardown.so and its control compile (${CC:-gcc})" "" "($W/crash_teardown.log)"
fi
# [2026-10-06] a filelist that cannot be opened: tnm.cc's error() ended with exit 0 (a failed job looked done)
rawrun new_nolist "$NEW" -- --filelist "$W/no_such_filelist.txt" --output "$W/out/nolist.root" --weight 1 --year 2024 \
       --dataOrMC MC --sample TTHHto4b --mode main
stops "a filelist that cannot be opened stops with exit 1 (was 0)" 1 "unable to open file"

# ---- 2017 ---------------------------------------------------------------------
Y1=("mc|MC|TTbar_Hadronic||$IN/mc17.root" "jetht_F|Data|JetHT_Run2017F|F|$IN/jetht17F.root"
    "btag_F|Data|BTagCSV_Run2017F|F|$IN/btag17F.root" "jetht_B|Data|JetHT_Run2017B|B|$IN/jetht17B.root")
for repo in "$NEW" $OLD; do
  tag=$([[ "$repo" == "$NEW" ]] && echo new || echo old); mkdir -p "$W/y1_$tag"
  printf '%s\n' mc jetht_F btag_F jetht_B > "$W/y1_$tag/EXPECTED"
  for spec in "${Y1[@]}"; do
    IFS='|' read -r n dom sample era file <<< "$spec"
    RTAG=$tag runa "y1_$n" "$repo" "$dom" "$sample" "$era" 2017 "$file" EXPANDED_TTBARID_DIR="$W/ttnb2017"
    [[ "$tag" == new ]] && ok_run "2017 $n runs ($sample)"
    cp "$LAST.root" "$W/y1_$tag/$n.root" 2>/dev/null
  done
done
check "2017 TTbar_Hadronic used the tt+nb lookup" "$(grep -q 'loaded rows (map size)      : 40' "$W/out/new_y1_mc.log" && echo 1)"
runa tt17null "$NEW" MC TTbar_Hadronic "" 2017 "$IN/mc17.root"; stops "2017 TTbar_Hadronic without a lookup still stops (E11)" 11 "no tt+nb lookup for 'TTbar_Hadronic'"
if [[ -n "$OLD" ]]; then
  python3 "$REPO/tools/stage1/y1_compare.py" "$W/y1_old" "$W/y1_new" > "$W/y1_compare.log" 2>&1
  check "2017 outputs identical to the older build (y1_compare.py)" "$(grep -q '^RESULT PASS' "$W/y1_compare.log" && echo 1)" "($W/y1_compare.log)"
  grep -E '^(FILE|CUTFLOW|RESULT)' "$W/y1_compare.log" | sed 's/^/    /'
fi

# ---- tools/stage3/data_lumi_check.py --------------------------------------------
FL="$W/fl"; mkdir -p "$FL"
LC() { python3 "$REPO/tools/stage3/data_lumi_check.py" --filelist-dir "$FL" --golden "$G24" "$@"; }
echo "$IN/jetmet24C.root" > "$FL/filelist_JetMET0_Run2024C-MINIv6NANOv15-v1.txt"
echo "$IN/jetmet24I.root" > "$FL/filelist_JetMET0_Run2024I-MINIv6NANOv15-v2.txt"
LC --json-out "$W/lumi" > "$W/lumi.log" 2>&1; rc=$?
check "data_lumi_check: missing golden LS reported, JSON written, exit 0" \
      "$([[ $rc == 0 ]] && grep -q '^PD JetMET0 golden_LS=.* missing_golden_LS=' "$W/lumi.log" && [[ -s "$W/lumi/JetMET0_missing_golden.json" ]] && echo 1)" "(exit $rc)"
check "data_lumi_check: golden runs of eras not produced are listed apart (GOLDEN_OUTSIDE)" \
      "$(grep -q '^GOLDEN_OUTSIDE JetMET0 runs=[1-9]' "$W/lumi.log" && grep -q '^RANGES JetMET0 379' "$W/lumi.log" && echo 1)"
printf '%s\n' "$IN/jetmet24I.root" "$IN/noDeepJet.root" > "$FL/filelist_JetMET0_Run2024I-MINIv6NANOv15-v2.txt"   # one sample, two files
LC > "$W/lumi_split.log" 2>&1; rc=$?
check "data_lumi_check: an LS in two files of one sample is SPLIT_LS, not a duplicate (exit 0)" \
      "$([[ $rc == 0 ]] && grep -q 'JetMET0_Run2024I-MINIv6NANOv15-v2 .*split_LS=[1-9]' "$W/lumi_split.log" && ! grep -q '^DUPLICATE' "$W/lumi_split.log" && echo 1)" "(exit $rc)"
echo "$IN/jetmet24I.root" > "$FL/filelist_JetMET0_Run2024I-MINIv6NANOv15-v2.txt"
echo "$IN/jetmet24I.root" > "$FL/filelist_JetMET0_Run2024I-MINIv6NANOv15_v2-v1.txt"     # the same LS in both era-I samples
LC > "$W/lumi_dup.log" 2>&1; rc=$?
check "data_lumi_check: an LS in two era-I samples -> DUPLICATE, exit 1" \
      "$([[ $rc == 1 ]] && grep -q '^DUPLICATE JetMET0' "$W/lumi_dup.log" && echo 1)" "(exit $rc)"

echo "RESULT: $NPASS PASS, $NFAIL FAIL"
[[ $NFAIL -eq 0 ]]
