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
#  split LS inside one sample (fine), an LS in two samples (duplicate, exit 1);
#  [2026-10-06, D-2026-10-06-A] the 2024 trigger rule per PD: the [trigger]
#  counts = the file's own HLT bits (PyROOT; Data on golden LS, 4J3T PNet or
#  DeepJet); MC and Muon0 take the whole OR, JetMET HLT_PFHT1050, ParkingHH
#  the b-tag paths without it (HadTrigger step = taken); the JetMET and
#  ParkingHH output trees are disjoint and together the Muon0 (whole OR) one.
#  [STEP 25 K, D-2026-10-08-A] the 2024 fixed-WP b-tag weight: the payload line (fake UParTAK4_kinfit of
#  fake_pog.py) and "NOT ready" without the efficiency JSON; --btagsf on stops without it (E52) and without the
#  payload (E40), not for Data; btagtrig MC fills BTagEff, tools/stage7/btag_eff_maps.py makes the maps from it,
#  and a --btagsf on run gives bTagWeight / _up / _down equal to the method-1a product recomputed from Tree/Tree
#  with correctionlib (also for a payload of sources only: central +- their quadrature sum), its closure line is
#  within 2 % of 1 (mc24b: synth_nano --b-low, b efficiencies inside (0, 1)), main --btagsf on = off x bTagWeight
#  for the same events; an unreadable efficiency JSON stops main (E52), not btagtrig; TTHH_BTAGEFF_JSON unset: E12;
#  [10-08 review] one that loads but cannot answer (groups without default, a WP missing, other WPs) stops at load
#  (E52), a payload whose keys only reach a default gives no variation (said twice), the jet-weight counts, and a
#  sample weighted with maps not made from it stays finite (the tool never uses an e of 0 or 1).
#  Exit: 0 all checks pass; 1 a check failed; 2 bad usage.
#  The values are random: the checks are about running and stopping, never physics.
# =============================================================================
set -u
KEEP=0; ARGS=()
for x in "$@"; do [[ "$x" == "--keep" ]] && KEEP=1 || ARGS+=("$x"); done
[[ ${#ARGS[@]} -ge 1 && ${#ARGS[@]} -le 2 ]] || { sed -n '3,40p' "$0" | sed 's/^# \{0,2\}//'; exit 2; }
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
S --year 2024 --kind mc --n 1500 --seed 41 --b-low 0.3 -o "$IN/mc24b.root"     # [STEP 25 K] b efficiencies inside (0, 1)
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
        TTHH_PU_JSON="$W/puWeights_2024_FAKE.json" TTHH_BTAGEFF_JSON=__NULL__ "$@" \
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
check "2024 MC loads the fixed-WP b-tag payload (UParTAK4_kinfit, b jets; up/down found) and says the weight is off" \
      "$(grep -q 'b-tag SF (2024_Summer24): fixed WP (method 1a with L and M), .*/POG/BTV/2024_Summer24/btagging_preliminary.json.gz -> UParTAK4_kinfit (b jets; c and light jets: SF 1, no payload yet); up/down: the payload.s up/down' "$LAST.log" \
            && grep -q '^\[btagSF\] fixed WP (method 1a, L and M; 2024): NOT ready (no efficiency JSON) -> bTagWeight = 1' "$LAST.log" \
            && grep -q 'b-tag efficiency JSON (fixed WP): not given (config null)' "$LAST.log" && echo 1)"
runa d24C "$NEW" Data JetMET0_Run2024C-MINIv6NANOv15-v1 C 2024 "$IN/jetmet24C.root"; ok_run "2024 Data era C runs"
check "2024 Data uses the Data JEC" "$(grep -q -- '-> JEC Summer24Prompt24_V1_DATA_L1L2L3Res_AK4PFPuppi,' "$LAST.log" && echo 1)"
runa d24I "$NEW" Data JetMET1_Run2024I-MINIv6NANOv15_v2-v2 I 2024 "$IN/jetmet24I.root"; ok_run "2024 Data era I runs"
runa noDJ "$NEW" Data JetMET0_Run2024I-MINIv6NANOv15-v2 I 2024 "$IN/noDeepJet.root"; ok_run "2024 Data without the DeepJet 4J3T branch runs"
check "2024 Data says which 4J3T path the file has" \
      "$(grep -q 'one of {HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3: yes, HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5: no}' "$LAST.log" && echo 1)"
# ---- [D-2026-10-06-A] the 2024 hadronic trigger per PD (b-tag paths in ParkingHH, HLT_PFHT1050 in JetMET0/1) ----
tcount() {   # log -> "events HT btag both btag_without_HT taken" of the end-of-job [trigger] line
  sed -nE 's/^\[trigger\] events ([0-9]+): HLT_PFHT1050 ([0-9]+), b-tag paths ([0-9]+), both ([0-9]+), b-tag without HLT_PFHT1050 ([0-9]+); taken ([0-9]+) .*/\1 \2 \3 \4 \5 \6/p' "$1"
}
cfstep() {   # log label -> the unweighted count of that step in the CutFlow Summary
  awk -v s="$2" -F' : ' '$1 == s {split($2, a, " "); print a[1] + 0; exit}' "$1"
}
fbits() {    # file MC|Data -> "events HT btag both OR" from the file's own HLT bits (Data: golden LS only, the 4J3T
             #   PNet or DeepJet; MC: the PNet 4J3T) -- what the analyzer must find. Run in $W (no stray modules).
  ( cd "$W" && python3 - "$1" "$2" "$G24" 2>> "$W/synth.log" <<'PY'
import json, sys, ROOT
fn, kind, gj = sys.argv[1:4]
gold = {int(r): v for r, v in json.load(open(gj)).items()}
f = ROOT.TFile.Open(fn); t = f.Get("Events")
n = ht = b = both = orr = 0
for i in range(t.GetEntries()):
    t.GetEntry(i)
    if kind == "Data" and not any(a <= int(t.luminosityBlock) <= z for a, z in gold.get(int(t.run), [])):
        continue
    h = bool(t.HLT_PFHT1050)
    bb = bool(t.HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3 or t.HLT_PFHT450_SixPFJet36_PNetBTag0p35
              or t.HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50
              or (kind == "Data" and t.HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5))
    n += 1; ht += h; b += bb; both += (h and bb); orr += (h or bb)
print(n, ht, b, both, orr)
PY
  )
}
read -r f_n f_ht f_b f_both f_or <<< "$(fbits "$IN/mc24.root" MC)"
read -r m_n m_ht m_b m_both m_nob m_taken <<< "$(tcount "$W/out/new_sig24.log")"
check "2024 MC [trigger] counts = the file's own bits (HLT_PFHT1050 ${f_ht:-?}, b-tag ${f_b:-?}, both ${f_both:-?} of ${f_n:-?}); MC takes the OR (${f_or:-?})" \
      "$([[ -n "${m_taken:-}" && -n "${f_or:-}" && "$m_n" == "$f_n" && "$m_ht" == "$f_ht" && "$m_b" == "$f_b" && "$m_both" == "$f_both" \
            && "$m_taken" == "$f_or" && "$(cfstep "$W/out/new_sig24.log" HadTrigger)" == "$m_taken" ]] && echo 1)" \
      "(analyzer: ${m_n:-?} ${m_ht:-?} ${m_b:-?} ${m_both:-?} taken ${m_taken:-?}; file: ${f_n:-?} ${f_ht:-?} ${f_b:-?} ${f_both:-?} OR ${f_or:-?})"
read -r g_n g_ht g_b g_both g_or <<< "$(fbits "$IN/jetmet24I.root" Data)"
read -r j_n j_ht j_b j_both j_nob j_taken <<< "$(tcount "$W/out/new_d24I.log")"
check "2024 Data [trigger] counts = the file's own bits on golden LS (${g_n:-?} events: HLT_PFHT1050 ${g_ht:-?}, b-tag incl. DeepJet 4J3T ${g_b:-?}, both ${g_both:-?})" \
      "$([[ -n "${j_n:-}" && -n "${g_n:-}" && "$j_n" == "$g_n" && "$j_ht" == "$g_ht" && "$j_b" == "$g_b" && "$j_both" == "$g_both" ]] && echo 1)" \
      "(analyzer: ${j_n:-?} ${j_ht:-?} ${j_b:-?} ${j_both:-?}; file: ${g_n:-?} ${g_ht:-?} ${g_b:-?} ${g_both:-?})"
check "2024 Data JetMET takes HLT_PFHT1050 only (HadTrigger = taken = ${g_ht:-?})" \
      "$([[ -n "${j_taken:-}" && "$j_taken" == "${g_ht:-x}" && "$(cfstep "$W/out/new_d24I.log" HadTrigger)" == "$j_taken" ]] \
            && grep -q '^\[trigger\] 2024 Data JetMET takes: HLT_PFHT1050 ' "$W/out/new_d24I.log" && echo 1)"
runa pk24I "$NEW" Data ParkingHH_Run2024I-MINIv6NANOv15-v1 I 2024 "$IN/jetmet24I.root"; ok_run "2024 Data ParkingHH runs"
read -r p_n p_ht p_b p_both p_nob p_taken <<< "$(tcount "$LAST.log")"
check "2024 Data ParkingHH takes the b-tag paths without HLT_PFHT1050 (HadTrigger = taken = $(( ${g_b:-0} - ${g_both:-0} )))" \
      "$([[ -n "${p_taken:-}" && -n "${g_b:-}" && "$p_taken" == "$(( g_b - g_both ))" && "$(cfstep "$LAST.log" HadTrigger)" == "$p_taken" ]] \
            && grep -q '^\[trigger\] 2024 Data ParkingHH takes: ' "$LAST.log" && echo 1)"
runa mu24I "$NEW" Data Muon0_Run2024I-MINIv6NANOv15-v1 I 2024 "$IN/jetmet24I.root"
read -r u_n u_ht u_b u_both u_nob u_taken <<< "$(tcount "$LAST.log")"
check "2024 Data Muon0 takes the whole OR (trigger-SF sample; ${g_or:-?})" \
      "$([[ $(cat "$LAST.rc") == 0 && -n "${u_taken:-}" && "$u_taken" == "${g_or:-x}" ]] && echo 1)" "(exit $(cat "$LAST.rc"), taken ${u_taken:-?})"
read -r e_j e_p e_m e_jp e_union <<< "$( cd "$W" && python3 - "$W/out/new_d24I.root" "$W/out/new_pk24I.root" "$W/out/new_mu24I.root" 2>> "$W/synth.log" <<'PY'
import sys, ROOT
def events(fn):      # the selected events of a main output: (run, event) of Tree/Tree
    f = ROOT.TFile.Open(fn); t = f.Get("Tree/Tree"); s = set()
    for i in range(t.GetEntries()):
        t.GetEntry(i); s.add((int(t.runNumber), int(t.eventNumber)))
    f.Close()
    return s
J, P, M = (events(x) for x in sys.argv[1:4])
print(len(J), len(P), len(M), len(J & P), int((J | P) == M))
PY
)"
check "2024 JetMET and ParkingHH select disjoint events whose union is the whole-OR selection (Tree/Tree: ${e_j:-?} + ${e_p:-?} = ${e_m:-?})" \
      "$([[ -n "${e_union:-}" && "$e_jp" == 0 && "$e_union" == 1 && "${e_m:-0}" -gt 0 ]] && echo 1)" "(overlap ${e_jp:-?}, union = Muon0's: ${e_union:-?})"
BTAGSF=on runa btag "$NEW" MC TTHHto4b "" 2024 "$IN/mc24.root"
stops "2024 --btagsf on without the efficiency JSON stops (E52)" 52 "needs our MC efficiency JSON"
# ---- [STEP 25 K] the fixed-WP b-tag weight end to end (D-2026-10-08-A) ----------------------------------------
#   btagtrig MC runs fill BTagEff; tools/stage7/btag_eff_maps.py makes the maps from them; a --btagsf on run then
#   gives bTagWeight = the product over its selected jets of the method-1a weight with the payload SF and the maps --
#   recomputed here from Tree/Tree (jetPt, jetEta, bTagScore, hadFlavs) with correctionlib's Python package.
MODE=btagtrig runa bt_tt "$NEW" MC TTbar_Hadronic "" 2024 "$IN/mc24b.root"; ok_run "2024 btagtrig MC runs (TTbar_Hadronic)"
MODE=btagtrig runa bt_qcd "$NEW" MC QCD_HT1000to1200 "" 2024 "$IN/mc24.root"; ok_run "2024 btagtrig MC runs (QCD_HT1000to1200)"
mkdir -p "$W/beff"
cp "$W/out/new_bt_tt.root" "$W/beff/TTbar_Hadronic.root"; cp "$W/out/new_bt_qcd.root" "$W/beff/QCD_HT1000to1200.root"
printf 'common:\n  year: "2024"\n  analysis_mode: btagtrig\nsamples:\n  - TTbar_Hadronic\n  - QCD_HT1000to1200\n  - %s\n' \
       Muon0_Run2024I-MINIv6NANOv15-v1 > "$W/beff.yml"
EFFJ="$W/beff/btag_eff_2024.json.gz"
python3 "$REPO/tools/stage7/btag_eff_maps.py" --config "$W/beff.yml" --base "$W/beff" --out "$EFFJ" --min-neff 20 --min-neff-bin 3 \
        > "$W/beff.log" 2>&1; rc=$?
check "btag_eff_maps.py on the btagtrig outputs (h_nevt = the HT>500 cutflow bin; JSON written and verified)" \
      "$([[ $rc == 0 ]] && grep -q '^SAMPLE TTbar_Hadronic group=tt nevt=[1-9][0-9]* ht_step=[0-9]* OK' "$W/beff.log" \
            && grep -q '^SAMPLE QCD_HT1000to1200 group=qcd ' "$W/beff.log" && grep -q '^DATA-SKIPPED Muon0' "$W/beff.log" \
            && grep -q '^VERIFY ok' "$W/beff.log" && [[ -s "$EFFJ" ]] && echo 1)" "(exit $rc, $W/beff.log)"
MODE=btagtrig BTAGSF=on runa wp_bt "$NEW" MC TTbar_Hadronic "" 2024 "$IN/mc24b.root" TTHH_BTAGEFF_JSON="$EFFJ"
ok_run "2024 --btagsf on with the efficiency JSON runs (btagtrig, TTbar_Hadronic)"
check "2024 --btagsf on: the weight is ready (group tt), the closure line is printed" \
      "$(grep -q "^\[btagSF\] fixed WP (method 1a, L and M; 2024): ready, efficiency group 'tt'" "$LAST.log" \
            && grep -q '^\[btagSF\] closure at the HT step (no b-tag cut yet): events [1-9]' "$LAST.log" && echo 1)"
CLOS=$(sed -nE 's/^\[btagSF\] closure at the HT step .* = ([0-9.eE+-]+) .*/\1/p' "$LAST.log")
check "2024 closure of the weight on the sample the maps come from: |sum(w x bTagWeight) / sum(w) - 1| < 0.02 (${CLOS:-?})" \
      "$(awk -v c="${CLOS:-0}" 'BEGIN{exit !(c > 0.98 && c < 1.02)}' && echo 1)"
# wpcheck <output> <payload file> <variation: updown | key_up,key_down;...> -> "events b_jets weights!=1 worst"
wpcheck() {
  ( cd "$W" && python3 - "$1" "$2" "$EFFJ" "$3" 2>> "$W/synth.log" <<'PY'
import math, sys, ROOT, correctionlib
out, sfp, effp, var = sys.argv[1:5]
sf = correctionlib.CorrectionSet.from_file(sfp)["UParTAK4_kinfit"]
eff = correctionlib.CorrectionSet.from_file(effp)["btag_eff"]
srcs = None if var == "updown" else [tuple(p.split(",")) for p in var.split(";")]
def sfv(sy, wp, ae, p):                 # the analyzer's getBTagSF_WP: up/down, or central +- the sources in quadrature
    c = sf.evaluate("central", wp, 5, ae, p)
    if sy == "central":
        return c
    if srcs is None:
        return sf.evaluate(sy, wp, 5, ae, p)
    q = math.sqrt(sum((sf.evaluate(k[0 if sy == "up" else 1], wp, 5, ae, p) - c) ** 2 for k in srcs))
    return c + q if sy == "up" else c - q
L, M = 0.0246, 0.1272                               # EraConfig 2024 UParTAK4 WPs
cl = lambda e: min(max(e, 1e-4), 1 - 1e-4)
f = ROOT.TFile.Open(out); t = f.Get("Tree/Tree")
n = nb = 0; worst = 0.0; nonone = 0
for i in range(t.GetEntries()):
    t.GetEntry(i)
    w = {"central": 1.0, "up": 1.0, "down": 1.0}
    for pt, eta, s, fl in zip(t.jetPt, t.jetEta, t.bTagScore, t.hadFlavs):
        if fl != 5:
            continue                                # the 2024 payload has b jets only: weight 1
        nb += 1
        ae, p = min(abs(eta), 2.4999), min(max(pt, 20.0), 599.9)
        eL = cl(eff.evaluate("tt", 5, "L", abs(eta), pt)); eM = min(cl(eff.evaluate("tt", 5, "M", abs(eta), pt)), eL)
        for sy in w:
            sL, sM = sfv(sy, "L", ae, p), sfv(sy, "M", ae, p)
            if s >= M:   wj = sM
            elif s >= L: wj = max(0.0, sL * eL - sM * eM) / (eL - eM) if eL - eM > 1e-6 else 1.0
            else:        wj = max(0.0, 1 - sL * eL) / (1 - eL) if 1 - eL > 1e-6 else 1.0
            w[sy] *= wj
    for sy, br in (("central", "bTagWeight"), ("up", "bTagWeight_up"), ("down", "bTagWeight_down")):
        v = getattr(t, br)
        worst = max(worst, abs(v - w[sy]) / max(abs(w[sy]), 1e-12))
    nonone += abs(t.bTagWeight - 1.0) > 1e-6
    n += 1
print(n, nb, nonone, "%.3g" % worst)
PY
  )
}
read -r wp_n wp_nb wp_non1 wp_worst <<< "$(wpcheck "$LAST.root" "$W/fakepog/POG/BTV/2024_Summer24/btagging_preliminary.json.gz" updown)"
check "2024 bTagWeight / _up / _down of every Tree/Tree event = the method-1a product recomputed from its jets (${wp_n:-?} events, ${wp_nb:-?} b jets, ${wp_non1:-?} weights != 1; largest relative difference ${wp_worst:-?})" \
      "$([[ -n "${wp_worst:-}" && "${wp_n:-0}" -gt 0 && "${wp_non1:-0}" -gt 0 ]] && awk -v x="$wp_worst" 'BEGIN{exit !(x < 2e-6)}' && echo 1)"
# a payload without a total up/down: the analyzer finds the sources (two spellings) and adds them in quadrature
mkdir -p "$W/fakepog_src"; cp -r "$W/fakepog/." "$W/fakepog_src/"
python3 "$HERE/fake_pog.py" --btv-sources "$W/fakepog_src" >> "$W/fake_pog.log" 2>&1
MODE=btagtrig BTAGSF=on runa wp_src "$NEW" MC TTbar_Hadronic "" 2024 "$IN/mc24b.root" TTHH_BTAGEFF_JSON="$EFFJ" \
     TTHH_JSONPOG_PATH="$W/fakepog_src"
ok_run "2024 --btagsf on with a payload of sources only runs"
check "... its load line names the sources it found (up_jes/down_jes, statistic_up/statistic_down)" \
      "$(grep -q 'up/down: sources in quadrature: up_jes/down_jes statistic_up/statistic_down' "$LAST.log" && echo 1)"
read -r ws_n ws_nb ws_non1 ws_worst <<< "$(wpcheck "$LAST.root" "$W/fakepog_src/POG/BTV/2024_Summer24/btagging_preliminary.json.gz" 'up_jes,down_jes;statistic_up,statistic_down')"
check "... and its bTagWeight_up / _down = central +- the sources in quadrature, recomputed (${ws_n:-?} events; ${ws_worst:-?})" \
      "$([[ -n "${ws_worst:-}" && "${ws_n:-0}" -gt 0 ]] && awk -v x="$ws_worst" 'BEGIN{exit !(x < 2e-6)}' && echo 1)"
# the end-of-job counts of the jet weights (maps made from this sample: no weight > 10)
check "2024 the [btagSF] jet-weight line: b jets counted, none above 10 on the sample the maps come from" \
      "$(grep -qE '^\[btagSF\] jets weighted \(b, all events\): [1-9][0-9]*; jet weight > 10: 0; set to 0 \(P_Data < 0\): central [0-9]+, up [0-9]+, down [0-9]+$' "$W/out/new_wp_bt.log" && echo 1)"
# a payload whose systematic category has a default (every key evaluates, none differs from central): no variation,
#   said loudly (load WARN, --btagsf on WARN), up = down = central
mkdir -p "$W/fakepog_def"; cp -r "$W/fakepog/." "$W/fakepog_def/"
python3 "$HERE/fake_pog.py" --btv-default "$W/fakepog_def" >> "$W/fake_pog.log" 2>&1
MODE=btagtrig BTAGSF=on runa wp_def "$NEW" MC TTbar_Hadronic "" 2024 "$IN/mc24b.root" TTHH_BTAGEFF_JSON="$EFFJ" \
     TTHH_JSONPOG_PATH="$W/fakepog_def"
ok_run "2024 --btagsf on with a payload of central + a default runs"
check "... finds no variation (keys that only reach the default do not count) and says so twice (WARN)" \
      "$(grep -q 'up/down: none found (up/down = central)' "$LAST.log" && grep -q 'has no up/down variation this code recognises' "$LAST.log" \
            && grep -q '^\[btagSF\]\[WARN\] --btagsf on: the payload gave no up/down variation' "$LAST.log" && echo 1)"
check "... and bTagWeight_up = bTagWeight_down = bTagWeight in every event" "$( cd "$W" && python3 - "$LAST.root" 2>> "$W/synth.log" <<'PY'
import sys, ROOT
f = ROOT.TFile.Open(sys.argv[1]); t = f.Get("Tree/Tree"); n = bad = 0
for i in range(t.GetEntries()):
    t.GetEntry(i); n += 1
    c, u, d = float(getattr(t, "bTagWeight")), float(getattr(t, "bTagWeight_up")), float(getattr(t, "bTagWeight_down"))
    bad += (u != c or d != c)
print(1 if n > 0 and bad == 0 else "")
PY
)"
# a sample weighted with maps not made from it (QCD: its own b maps would be e_L = e_M = 1 -- mc24 has every b jet above
#   M -- so the tool takes the 'all' maps there): weights stay finite
MODE=btagtrig BTAGSF=on runa wp_qcd "$NEW" MC QCD_HT1000to1200 "" 2024 "$IN/mc24b.root" TTHH_BTAGEFF_JSON="$EFFJ"
ok_run "2024 --btagsf on, QCD_HT1000to1200 on the other synthetic file"
CLQ=$(sed -nE 's/^\[btagSF\] closure at the HT step .* = ([0-9.eE+-]+) .*/\1/p' "$LAST.log")
check "... no jet weight above 10 and a closure in (0.5, 2) (${CLQ:-?}): the maps never use an e of 0 or 1 (reviewer, 10-08)" \
      "$(grep -q 'jet weight > 10: 0;' "$LAST.log" && awk -v c="${CLQ:-0}" 'BEGIN{exit !(c > 0.5 && c < 2)}' && echo 1)"
check "... btag_eff_maps.py took the 'all' maps for qcd b (its own bins have no jet below M)" \
      "$(grep -qE '^LEVELS qcd b L0=0 L1=0 ' "$W/beff.log" && grep -qE '^LEVELS qcd b .*thin tag bin [1-9]' "$W/beff.log" && echo 1)"
# an efficiency JSON that loads but cannot answer: stops at load with E52 (not an abort in the event loop)
python3 - "$EFFJ" "$W" >> "$W/synth.log" 2>&1 <<'PY'
import gzip, json, sys
src, w = sys.argv[1:3]
js = json.load(gzip.open(src, "rt"))
a = json.loads(json.dumps(js)); del a["corrections"][1]["data"]["default"]           # btag_eff_groups: no default
json.dump(a, open(w + "/eff_nodefault.json", "w"))
b = json.loads(json.dumps(js))                                                          # tt b without WP L
tt = [it for it in b["corrections"][0]["data"]["content"] if it["key"] == "tt"][0]["value"]
bb = [it for it in tt["content"] if it["key"] == 5][0]["value"]
bb["content"] = [it for it in bb["content"] if it["key"] != "L"]
json.dump(b, open(w + "/eff_noL.json", "w"))
c = json.loads(json.dumps(js))                                                          # other WPs in the description
c["corrections"][0]["description"] = c["corrections"][0]["description"].replace("wp=L:0.0246,", "wp=L:0.03,")
json.dump(c, open(w + "/eff_otherwp.json", "w"))
PY
runa eff_nd "$NEW" MC ST_tW_top_had "" 2024 "$IN/mc24b.root" TTHH_BTAGEFF_JSON="$W/eff_nodefault.json"
stops "2024 main, efficiency JSON whose groups have no default, a sample of a group not in it: E52 at load" 52 "b-tag efficiency JSON"
runa eff_nl "$NEW" MC TTbar_Hadronic "" 2024 "$IN/mc24b.root" TTHH_BTAGEFF_JSON="$W/eff_noL.json"
stops "2024 main, efficiency JSON without WP L for tt b: E52 at load" 52 "b-tag efficiency JSON"
runa eff_wp "$NEW" MC TTbar_Hadronic "" 2024 "$IN/mc24b.root" TTHH_BTAGEFF_JSON="$W/eff_otherwp.json"
stops "2024 main, efficiency JSON made with other WPs: E52 at load" 52 "made with the WPs L=0.03"
MODE=btagtrig runa eff_nlbt "$NEW" MC TTbar_Hadronic "" 2024 "$IN/mc24b.root" TTHH_BTAGEFF_JSON="$W/eff_noL.json"
ok_run "2024 btagtrig with that JSON runs (bootstrap: WARN, no weight)"
# main: the production weight carries the b-tag weight (--btagsf on vs off, the same events)
runa wp_moff "$NEW" MC TTHHto4b "" 2024 "$IN/mc24b.root" TTHH_BTAGEFF_JSON="$EFFJ"; ok_run "2024 main --btagsf off runs (efficiency JSON given)"
BTAGSF=on runa wp_main "$NEW" MC TTHHto4b "" 2024 "$IN/mc24b.root" TTHH_BTAGEFF_JSON="$EFFJ"; ok_run "2024 main --btagsf on runs"
WPMAIN=$( cd "$W" && python3 - "$W/out/new_wp_moff.root" "$LAST.root" 2>> "$W/synth.log" <<'PY'
import sys, ROOT
def rows(fn):
    f = ROOT.TFile.Open(fn); t = f.Get("Tree/Tree"); d = {}
    for i in range(t.GetEntries()):
        t.GetEntry(i); d[(int(t.runNumber), int(t.eventNumber))] = (t.evtWeight, t.bTagWeight)
    return d
off, on = rows(sys.argv[1]), rows(sys.argv[2])
worst = max(abs(on[k][0] - off[k][0] * on[k][1]) / max(abs(on[k][0]), 1e-12) for k in on) if on else 1.0
same_bw = all(abs(off[k][1] - on[k][1]) < 1e-7 for k in on)       # the branch is filled in both runs
print(len(off), len(on), int(set(off) == set(on) and same_bw), "%.3g" % worst)
PY
)
read -r m_off m_on m_same m_worst <<< "${WPMAIN:-}"
check "2024 main: --btagsf on selects the same events, the same bTagWeight, and evtWeight(on) = evtWeight(off) x bTagWeight (${m_on:-?} events; ${m_worst:-?})" \
      "$([[ "${m_same:-0}" == 1 && "${m_on:-0}" -gt 0 ]] && awk -v x="${m_worst:-1}" 'BEGIN{exit !(x < 2e-6)}' && echo 1)"
check "2024 main keeps BTagEff (h_nevt = the HT>500 step) and Data has none" \
      "$( cd "$W" && python3 - "$LAST.root" "$LAST.log" "$W/out/new_d24C.root" 2>> "$W/synth.log" <<'PY'
import sys, ROOT
f = ROOT.TFile.Open(sys.argv[1]); h = f.Get("BTagEff/h_nevt"); cf = f.Get("Tree/cutflow")
ht = [cf.GetBinContent(i) for i in range(1, cf.GetNbinsX() + 1) if cf.GetXaxis().GetBinLabel(i) == "HT>500"]
d = ROOT.TFile.Open(sys.argv[3])
print(1 if (h and ht and h.GetBinContent(1) == ht[0] and not d.Get("BTagEff")) else "")
PY
)"
BTAGSF=on runa d24on "$NEW" Data JetMET0_Run2024C-MINIv6NANOv15-v1 C 2024 "$IN/jetmet24C.root"; ok_run "2024 Data --btagsf on runs (no b-tag weight for Data, no efficiency JSON needed)"
# the payload missing: --btagsf off runs with a WARN, --btagsf on stops (E40); an unreadable efficiency JSON: main E52
mkdir -p "$W/fakepog_nobtv"; cp -r "$W/fakepog/." "$W/fakepog_nobtv/"
mv "$W/fakepog_nobtv/POG/BTV/2024_Summer24/btagging_preliminary.json.gz" "$W/fakepog_nobtv/POG/BTV/2024_Summer24/moved.json.gz"
runa nobtv "$NEW" MC TTHHto4b "" 2024 "$IN/mc24.root" TTHH_JSONPOG_PATH="$W/fakepog_nobtv"
ok_run "2024 MC without the BTV payload runs with --btagsf off"
check "... and says the payload is not loaded (WARN)" \
      "$(grep -q 'WARN\] b-tag SF (2024_Summer24, fixed WP): .*btagging_preliminary.json.gz -> UParTAK4_kinfit: .* -- not loaded' "$LAST.log" && echo 1)"
BTAGSF=on runa nobtvon "$NEW" MC TTHHto4b "" 2024 "$IN/mc24.root" TTHH_JSONPOG_PATH="$W/fakepog_nobtv" TTHH_BTAGEFF_JSON="$EFFJ"
stops "2024 --btagsf on without the BTV payload stops (E40)" 40 "the BTV payload SF is not loaded"
printf '{"not": "a correction set"}' > "$W/badeff.json"
runa badeff "$NEW" MC TTHHto4b "" 2024 "$IN/mc24.root" TTHH_BTAGEFF_JSON="$W/badeff.json"
stops "2024 main with an unreadable efficiency JSON stops (E52; a derived correction given but broken)" 52 "b-tag efficiency JSON"
MODE=btagtrig runa badeffbt "$NEW" MC TTHHto4b "" 2024 "$IN/mc24.root" TTHH_BTAGEFF_JSON="$W/badeff.json"
ok_run "2024 btagtrig with an unreadable efficiency JSON runs (bootstrap: WARN)"
runa nopu "$NEW" MC TTHHto4b "" 2024 "$IN/mc24.root" TTHH_PU_JSON=; stops "2024 without TTHH_PU_JSON stops (E12)" 12 "TTHH_PU_JSON"
runa noeff "$NEW" MC TTHHto4b "" 2024 "$IN/mc24.root" TTHH_BTAGEFF_JSON=; stops "2024 without TTHH_BTAGEFF_JSON stops (E12)" 12 "TTHH_BTAGEFF_JSON"
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
  stops "a stop keeps its exit code when the exit-time teardown crashes (E52 --btagsf on; not 139)" 52 "needs our MC efficiency JSON"
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
