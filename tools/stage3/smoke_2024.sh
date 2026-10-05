#!/bin/bash
# =============================================================================
#  smoke_2024.sh -- the built analyzer on real 2024 v15 files, before the 2024
#  prescan and main submission (STEP 24; PLAN 9 Stage 3)
#
#  At KNU, in the container after cmsenv, in tempTTHH (after the clean build):
#    /bin/bash tools/runlog/condor_run.sh --memory 12GB smoke_2024 -- /bin/bash tools/stage3/smoke_2024.sh
#  Options: [--config <yml>] (default AnalyzerConfig/Tier3_2024_FH_unified_main.yml)
#           [--prescan-config <yml>] (default AnalyzerConfig/Tier3_2024_FH_unified_prescan.yml)
#           [--filelist-dir <dir>] (default filelistTier3_2024)
#
#  Runs (outputs in a scratch directory under $TMPDIR; nothing is written to
#  /pnfs; the analyzer logs are copied to condor/smoke_2024_<UTC>/, gitignored):
#    mc_sig    main     TTHHto4b                              MC signal
#    mc_tt     main     TTbar_Hadronic                        MC, no tt+nb lookup in 2024 (D-2026-10-05-C)
#    dC_pnet   main     JetMET0_Run2024C-MINIv6NANOv15-v1     Data era C, a file WITH the PNet 4J3T path
#    dC_nopnet main     JetMET0_Run2024C-MINIv6NANOv15-v1     Data era C, a file WITHOUT it (if one exists)
#    dI        main     JetMET1_Run2024I-MINIv6NANOv15_v2-v2  Data era I (the second era-I dataset)
#    dC_mix    main     the two era-C files in one job        must stop: E11, different branch sets
#    pre_sig   prescan  TTHHto4b (the mc_sig file)            the prescan path
#    mc_multi  main     QCD_HT200to400, job 0 of the main yml's chunks     several files in one MC job
#    pre_multi prescan  QCD_HT200to400, job 0 of the prescan yml's chunks  the same for the prescan
#  Files: from <filelist-dir>/filelist_<sample>.txt, the one at the 10th
#  percentile of the sizes (small, but not a short tail job). Data: only a file
#  with at least half of its LS (LuminosityBlocks tree) in the 2024 golden JSON
#  -- the analyzer skips non-golden LS before it reads the objects, so a file
#  without golden LS would show nothing (2024 Data was made without a lumi
#  mask). Era C: files are opened in that order (then the smaller ones) until
#  one with and one without the PNet 4J3T branch are found (at most 200).
#  mc_multi/pre_multi: the first files_per_job lines of
#  filelist_QCD_HT200to400.txt, i.e. the submitter's job 0 (small files, ~22 MB).
#  Environment: the condor job's -- setup.sh, the correction paths of the yml
#  read with the submitter's own yml reader and env map (null -> __NULL__), and
#  the first-look flags --trigsf off --btagsf off --btagrw off (D-2026-10-05-A).
#  Checks (CHECK <run>: <what> PASS|FAIL): exit 0; the [eventBuffer] line is the
#  stamp of this checkout's include/eventBuffer.h (the executable was built
#  from it); the [branches] line; the yml paths were used (jsonpog, PU JSON);
#  the 2024 payload names (jet ID, veto map, PU, JEC/JER -- the Data JEC for
#  Data --, the UParTAK4 WPs); main: cutflow counts at HadTrigger, njets>=6 and
#  HT>500 > 0 (and nbjets>=2 for MC), the output has the end marker
#  cutflow_w_full; TTbar_Hadronic says the tt+nb split is off; the era-C runs
#  name the 4J3T path of their file; dC_mix stops with E11; prescan: the
#  output has the one-row prescan tree; mc_multi/pre_multi: the job's files
#  have one branch set ([branches] N input files, the same ...).
#  Prints: FILE <run> MB=<size> entries=<Events entries> pnet=<y/n> deepjet=<y/n>
#               lumis=<LS> golden=<golden LS> <path>   (lumis/golden '-' for MC)
#          TIMING <run> entries=<n> wall_s=<s> ev_per_s=<x> kB_per_ev=<y>
#          CUTFLOW <run> <label>=<count> ...;  CLEANING <run> <the [cleaning] line>
#  Exit: 0 all checks pass; 1 a check failed; 2 bad usage or a missing input.
# =============================================================================
set -u
REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd -P)"
CFG="AnalyzerConfig/Tier3_2024_FH_unified_main.yml"; PCFG="AnalyzerConfig/Tier3_2024_FH_unified_prescan.yml"
FLD="filelistTier3_2024"
while [[ $# -gt 0 ]]; do
  case "$1" in
    --config)       [[ $# -ge 2 ]] || exit 2; CFG="$2"; shift 2 ;;
    --prescan-config) [[ $# -ge 2 ]] || exit 2; PCFG="$2"; shift 2 ;;
    --filelist-dir) [[ $# -ge 2 ]] || exit 2; FLD="$2"; shift 2 ;;
    -h|--help)      sed -n '3,49p' "$0" | sed 's/^# \{0,2\}//'; exit 0 ;;
    *)              echo "smoke_2024.sh: unknown argument $1 (see --help)"; exit 2 ;;
  esac
done
cd "$REPO" || exit 2
[[ "$CFG" == /* ]] || CFG="$REPO/$CFG"
[[ "$PCFG" == /* ]] || PCFG="$REPO/$PCFG"
[[ "$FLD" == /* ]] || FLD="$REPO/$FLD"
EXE="$REPO/ttHHanalyzer_unified"
[[ -x "$EXE" ]] || { echo "ERROR no executable $EXE (build first: tools/stage0/build_check.sh)"; exit 2; }
[[ -f "$CFG" ]] || { echo "ERROR no config $CFG"; exit 2; }
[[ -f "$PCFG" ]] || { echo "ERROR no config $PCFG"; exit 2; }
[[ -d "$FLD" ]] || { echo "ERROR no filelist directory $FLD (tools/stage3/make_filelists_v15.py --year 2024)"; exit 2; }
python3 -c 'import ROOT' > /dev/null 2>&1 || { echo "ERROR no PyROOT (cmsenv first)"; exit 2; }
STAMP="$(sed -n 's/^#define TTHH_EVENTBUFFER_STAMP "\(.*\)"$/\1/p' "$REPO/include/eventBuffer.h")"
[[ -n "$STAMP" ]] || { echo "ERROR include/eventBuffer.h has no TTHH_EVENTBUFFER_STAMP (not a STEP 24 header)"; exit 2; }
W="$(mktemp -d "${TMPDIR:-/tmp}/tthh_smoke2024.XXXXXX")" || exit 2
trap 'rm -rf "$W"' EXIT
KEEPLOG="$REPO/condor/smoke_2024_$(date -u +%Y%m%d_%H%M%S)"
mkdir -p "$KEEPLOG" || exit 2
echo "WORK $W"
echo "LOGS $KEEPLOG"
echo "EXE  $EXE ($(stat -c '%y' "$EXE" | cut -c1-19), md5 $(md5sum < "$EXE" | cut -c1-32))"
echo "HEADER stamp: $STAMP"
echo "CONFIG $CFG (prescan: $PCFG)"

# ---- the job environment: setup.sh, then the yml paths as the submitter maps them ----------------------
set +u; source "$REPO/setup.sh" > /dev/null; set -u     # setup.sh reads unset variables (PYTHONPATH, ...)
python3 - "$REPO" "$CFG" "$PCFG" > "$W/env.sh" <<'PY' || { echo "ERROR reading the config:"; cat "$W/env.sh"; exit 2; }
import ast, importlib.util, os, shlex, sys
repo, cfg, pcfg = sys.argv[1], sys.argv[2], sys.argv[3]
src = os.path.join(repo, "submit_job_FH_Tier3_unified.py")
spec = importlib.util.spec_from_file_location("tthh_submitter", src)
mod = importlib.util.module_from_spec(spec)
spec.loader.exec_module(mod)
common = mod.CondorJobManager.load_yaml_config(None, cfg)["common"]
emap = None     # CondorJobManager.parse_config_entry's map, read from its source (no second copy)
for node in ast.walk(ast.parse(open(src).read())):
    if isinstance(node, ast.Assign) and any(isinstance(t, ast.Name) and t.id == "path_env_map" for t in node.targets):
        emap = ast.literal_eval(node.value)
if not emap:
    print("no path_env_map in %s" % src); sys.exit(1)
if str(common.get("year", "")).strip() != "2024":
    print("%s is not a 2024 config (year %r)" % (cfg, common.get("year"))); sys.exit(1)
for key, env in emap.items():
    v = common.get(key, "")
    v = mod.NULL_SENTINEL if v is None else str(v).strip()
    if not v:
        print("%s is empty or missing in %s (the submitter stops with E12)" % (key, cfg)); sys.exit(1)
    print("export %s=%s" % (env, shlex.quote(v)))
class _NoCli:   # the submitter's files-per-job rule (CondorJobManager._files_per_job) without a CLI value
    cli_files_per_job = None
pcommon = mod.CondorJobManager.load_yaml_config(None, pcfg)["common"]
print("SMOKE_FPJ_MC=%d" % mod.CondorJobManager._files_per_job(_NoCli(), common, False))
print("SMOKE_FPJ_PRESCAN=%d" % mod.CondorJobManager._files_per_job(_NoCli(), pcommon, False))
PY
source "$W/env.sh"
sed -n 's/^export /[paths] /p' "$W/env.sh"
echo "[files per job] main MC $SMOKE_FPJ_MC, prescan $SMOKE_FPJ_PRESCAN"
G24="$TTHH_GOLDENJSON_PATH/2024_Summer24/Cert_Collisions2024_378981_386951_Golden.json"   # what the analyzer reads
[[ -f "$G24" ]] || { echo "ERROR no 2024 golden JSON $G24"; exit 2; }

NPASS=0; NFAIL=0; FAILED_RUNS=()
check() {   # run what ok [detail]
  if [[ "$3" == 1 ]]; then echo "CHECK $1: $2 PASS"; NPASS=$((NPASS+1))
  else echo "CHECK $1: $2 FAIL ${4:-}"; NFAIL=$((NFAIL+1)); FAILED_RUNS+=("$1"); fi
}

# ---- the files ----------------------------------------------------------------------------------------
# pick.py <filelist> [--golden <json>] [--pnet-both]: prints 'path size entries pnet deepjet lumis golden' (one
# line; two for --pnet-both: a file with the PNet 4J3T branch and one without -- the second line is missing when
# no such file is found). With --golden (Data) only files with >= 1 golden LS and >= half of their LS golden.
cat > "$W/pick.py" <<'PY'
import json, os, sys
import ROOT
ROOT.gErrorIgnoreLevel = ROOT.kError
PNET = "HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3"
DJ = "HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5"
args = sys.argv[1:]
both = "--pnet-both" in args
golden = None
if "--golden" in args:
    golden = {int(r): [(int(lo), int(hi)) for lo, hi in v]
              for r, v in json.load(open(args[args.index("--golden") + 1])).items()}
paths = [l.strip() for l in open(args[0]) if l.strip() and not l.startswith("#")]
sz = []
for p in paths:
    try:
        sz.append((os.stat(p).st_size, p))
    except OSError:
        pass
if not sz:
    sys.exit(1)
sz.sort()
i0 = len(sz) // 10
order = sz[i0:] + sz[:i0][::-1]       # from the 10th percentile up, then the smaller ones
def lumis(f):
    lb = f.Get("LuminosityBlocks")
    if not lb:
        return -1, -1
    lb.SetBranchStatus("*", 0)
    lb.SetBranchStatus("run", 1)
    lb.SetBranchStatus("luminosityBlock", 1)
    n = g = 0
    for e in lb:
        n += 1
        r, l = int(e.run), int(e.luminosityBlock)
        if any(lo <= l <= hi for lo, hi in golden.get(r, ())):
            g += 1
    return n, g
def info(size, p):
    f = ROOT.TFile.Open(p)
    t = f.Get("Events") if f and not f.IsZombie() else None
    if not t:
        return None
    nl, ng = lumis(f) if golden is not None else ("-", "-")
    r = (p, size, int(t.GetEntries()), "y" if t.GetBranch(PNET) else "n", "y" if t.GetBranch(DJ) else "n", nl, ng)
    f.Close()
    return r
def usable(r):
    return r is not None and (golden is None or (r[6] >= 1 and 2 * r[6] >= r[5]))
FMT = "%s %d %d %s %s %s %s"
found, opened, few = {}, 0, 0
for size, p in order[:200]:
    r = info(size, p)
    opened += 1
    if not usable(r):
        few += r is not None
        continue
    key = r[3] if both else "any"
    if key not in found:
        found[key] = r
    if len(found) == (2 if both else 1):
        break
print("PICK %s opened=%d skipped_few_golden_LS=%d" % (os.path.basename(args[0]), opened, few), file=sys.stderr)
if not found:
    sys.exit(1)
for key in (("y", "n") if both else ("any",)):
    if key in found:
        print(FMT % found[key])
PY
declare -A FPATH FSIZE FENT
note_file() {   # run "path size entries pnet deepjet lumis golden"
  local run="$1" p s e pn dj nl ng; read -r p s e pn dj nl ng <<< "$2"
  FPATH[$run]="$p"; FSIZE[$run]="$s"; FENT[$run]="$e"
  echo "FILE $run MB=$(awk -v s="$s" 'BEGIN{printf "%.1f", s/1e6}') entries=$e pnet=$pn deepjet=$dj lumis=$nl golden=$ng $p"
}
pick() { python3 "$W/pick.py" "$FLD/filelist_$1.txt" "${@:2}" 2>> "$W/pick.err"; }
L="$(pick TTHHto4b)" || { cat "$W/pick.err"; echo "ERROR no readable file in filelist_TTHHto4b.txt"; exit 2; }
note_file mc_sig "$L"; note_file pre_sig "$L"
L="$(pick TTbar_Hadronic)" || { cat "$W/pick.err"; echo "ERROR no readable file in filelist_TTbar_Hadronic.txt"; exit 2; }
note_file mc_tt "$L"
SC=JetMET0_Run2024C-MINIv6NANOv15-v1
mapfile -t LC < <(pick "$SC" --golden "$G24" --pnet-both)
HAVE_PNET=0; HAVE_NOPNET=0
for l in "${LC[@]}"; do
  read -r _ _ _ pn _ <<< "$l"
  if [[ "$pn" == y ]]; then note_file dC_pnet "$l"; HAVE_PNET=1; else note_file dC_nopnet "$l"; HAVE_NOPNET=1; fi
done
SI=JetMET1_Run2024I-MINIv6NANOv15_v2-v2
L="$(pick "$SI" --golden "$G24")" || { cat "$W/pick.err"; echo "ERROR no file with golden LS in filelist_$SI.txt"; exit 2; }
note_file dI "$L"
cat "$W/pick.err"
[[ $HAVE_PNET == 1 ]] || { echo "ERROR no era-C file with the PNet 4J3T branch and golden LS in filelist_$SC.txt"; exit 2; }
[[ $HAVE_NOPNET == 1 ]] || echo "NOTE no era-C file without the PNet 4J3T branch found: dC_nopnet and dC_mix are skipped"
chunk0() {   # sample n -> the first n files of its filelist, comma-separated (the submitter's job 0)
  grep -v '^#' "$FLD/filelist_$1.txt" | sed '/^[[:space:]]*$/d' | head -n "$2" | paste -sd, -
}
SM=QCD_HT200to400
CH_MAIN="$(chunk0 "$SM" "$SMOKE_FPJ_MC")"; CH_PRE="$(chunk0 "$SM" "$SMOKE_FPJ_PRESCAN")"
[[ -n "$CH_MAIN" && -n "$CH_PRE" ]] || { echo "ERROR no files in filelist_$SM.txt"; exit 2; }
N_MAIN=$(tr ',' '\n' <<< "$CH_MAIN" | wc -l); N_PRE=$(tr ',' '\n' <<< "$CH_PRE" | wc -l)
sumfiles() {   # file,file,... -> 'entries bytes' of all of them
  python3 - "$1" <<'PY'
import os, sys, ROOT
ROOT.gErrorIgnoreLevel = ROOT.kError
n = b = 0
for p in sys.argv[1].split(","):
    b += os.stat(p).st_size
    f = ROOT.TFile.Open(p)
    t = f.Get("Events") if f and not f.IsZombie() else None
    n += int(t.GetEntries()) if t else 0
    if f:
        f.Close()
print(n, b)
PY
}
read -r FENT[mc_multi] FSIZE[mc_multi] <<< "$(sumfiles "$CH_MAIN")"
read -r FENT[pre_multi] FSIZE[pre_multi] <<< "$(sumfiles "$CH_PRE")"
echo "FILES mc_multi $SM job 0: $N_MAIN file(s), entries=${FENT[mc_multi]} MB=$(awk -v s="${FSIZE[mc_multi]}" 'BEGIN{printf "%.1f", s/1e6}');" \
     "pre_multi job 0: $N_PRE file(s), entries=${FENT[pre_multi]} MB=$(awk -v s="${FSIZE[pre_multi]}" 'BEGIN{printf "%.1f", s/1e6}')"

# ---- one analyzer run ---------------------------------------------------------------------------------
runa() {   # run mode dataOrMC sample era file[,file]
  local run="$1" mode="$2" dom="$3" sample="$4" era="$5" files="$6"
  local out="$W/$run"
  tr ',' '\n' <<< "$files" > "$out.filelist"
  local args=(--filelist "$out.filelist" --output "$out.root" --weight 1 --year 2024 --dataOrMC "$dom"
              --sample "$sample" --mode "$mode" --trigsf off --btagsf off --btagrw off)
  [[ -n "$era" ]] && args+=(--era "$era")
  echo "RUN $run: $mode $dom $sample${era:+ era $era} ($(tr '\n' ' ' < "$out.filelist"| wc -w) file(s))"
  local t0 t1
  t0=$(date +%s.%N)
  ( cd "$REPO" && "$EXE" "${args[@]}" ) > "$out.log" 2>&1
  echo $? > "$out.rc"
  t1=$(date +%s.%N)
  cp "$out.log" "$KEEPLOG/$run.log"
  LAST="$out"; LRUN="$run"
  if [[ -n "${FENT[$run]:-}" ]]; then
    awk -v a="$t0" -v b="$t1" -v n="${FENT[$run]}" -v s="${FSIZE[$run]}" -v r="$run" 'BEGIN{
      w=b-a; printf "TIMING %s entries=%d wall_s=%.1f ev_per_s=%.0f kB_per_ev=%.2f\n", r, n, w, (w>0?n/w:0), (n>0?s/n/1e3:0)}'
  fi
}
cutval() {   # label -> the count of that cutflow step in the last log (empty if none)
  awk -v L="$1" '/^=== CutFlow Summary ===/{f=1;next} f&&/ : /{ if (index($0, L " : ")==1) { split($0,a," : "); split(a[2],b," "); print b[1]; exit } next } f{exit}' "$LAST.log"
}
complete() {   # mode: the submitter's completeness rule (resubmit/report) on the last output
  python3 - "$LAST.root" "$1" <<'PY'
import sys, ROOT
ROOT.gErrorIgnoreLevel = ROOT.kError
f = ROOT.TFile.Open(sys.argv[1])
if not f or f.IsZombie():
    sys.exit(1)
if sys.argv[2] == "prescan":
    t = f.Get("prescan")
    sys.exit(0 if (t and t.InheritsFrom("TTree") and t.GetEntries() == 1) else 1)
def has(d, depth=0):
    if depth > 4:
        return False
    for k in d.GetListOfKeys():
        if k.GetName() == "cutflow_w_full":
            return True
        if k.GetClassName().startswith("TDirectory") and has(k.ReadObj(), depth + 1):
            return True
    return False
sys.exit(0 if has(f) else 1)
PY
}
common_checks() {   # dataOrMC label(analysis code|prescan)
  local dom="$1" label="$2" rc; rc=$(cat "$LAST.rc")
  check "$LRUN" "exit 0" "$([[ $rc == 0 ]] && echo 1)" "(exit $rc)"
  check "$LRUN" "built from this checkout's header (eventBuffer stamp)" "$(grep -qF "[eventBuffer] $STAMP" "$LAST.log" && echo 1)" \
        "(the log says: $(grep -m1 '^\[eventBuffer\]' "$LAST.log"))"
  check "$LRUN" "required branches present" \
        "$(grep -q "^\[branches\] runYear=2024 $dom ($label): all [0-9]* required branches present" "$LAST.log" && echo 1)"
  check "$LRUN" "the yml paths were used (jsonpog, PU JSON)" \
        "$(grep -qF "[cfgpath] jsonpog-integration = $TTHH_JSONPOG_PATH " "$LAST.log" && grep -qF "[cfgpath] 2024 PU weight JSON = $TTHH_PU_JSON " "$LAST.log" && echo 1)"
}
main_checks() {   # dataOrMC
  local dom="$1" jec=MC; [[ "$dom" == Data ]] && jec=DATA
  common_checks "$dom" "analysis code"
  check "$LRUN" "2024 payloads (jet ID, veto map, PU, JEC $jec, JER, UParTAK4 WPs)" "$(
    grep -q 'jet ID (2024_Summer24): .* -> AK4PUPPI_Tight, AK4PUPPI_TightLeptonVeto' "$LAST.log" &&
    grep -q 'jet veto map (2024_Summer24): .* -> Summer24Prompt24_RunBCDEFGHI_V1 type jetvetomap' "$LAST.log" &&
    grep -q 'PU weights (2024): .* -> Collisions24_goldenJSON' "$LAST.log" &&
    grep -q -- "-> JEC Summer24Prompt24_V1_${jec}_L1L2L3Res_AK4PFPuppi, JER Summer23BPixPrompt23_RunD_JRV1_MC_PtResolution_AK4PFPuppi + Summer23BPixPrompt23_RunD_JRV1_MC_ScaleFactor_AK4PFPuppi" "$LAST.log" &&
    grep -q '^\[objectJet\] UParTAK4 (Jet_btagUParTAK4B) WP for 2024 : L=0.0246 M=0.1272 T=0.4648' "$LAST.log" && echo 1)"
  local steps=(HadTrigger "njets>=6" "HT>500"); [[ "$dom" == MC ]] && steps+=("nbjets>=2")
  local s v
  for s in "${steps[@]}"; do
    v="$(cutval "$s")"
    check "$LRUN" "cutflow $s > 0" "$(awk -v x="${v:-0}" 'BEGIN{exit !(x>0)}' && echo 1)" "(count '${v:-none}')"
  done
  echo "CUTFLOW $LRUN$(awk '/^=== CutFlow Summary ===/{f=1;next} f&&/ : /{split($0,a," : "); split(a[2],b," "); printf " %s=%s", a[1], b[1]; next} f{exit}' "$LAST.log")"
  local cl; cl="$(grep -m1 '^\[cleaning\] events' "$LAST.log")"; echo "CLEANING $LRUN ${cl:-(no [cleaning] line)}"
  check "$LRUN" "output complete (end marker cutflow_w_full)" "$(complete main && echo 1)"
}

# ---- 2024 MC ------------------------------------------------------------------------------------------
runa mc_sig main MC TTHHto4b "" "${FPATH[mc_sig]}"; main_checks MC
runa mc_tt main MC TTbar_Hadronic "" "${FPATH[mc_tt]}"; main_checks MC
check mc_tt "the tt+nb split is off in 2024 (D-2026-10-05-C)" "$(grep -q 'NOT split from 51-55' "$LAST.log" && echo 1)"
runa pre_sig prescan MC TTHHto4b "" "${FPATH[pre_sig]}"
common_checks MC prescan
check pre_sig "output complete (one-row prescan tree)" "$(complete prescan && echo 1)"
grep -m1 '^\[Prescan\] Runs tree totals' "$LAST.log" | sed 's/^/PRESCAN pre_sig /'
multi_line() {   # n: the requireSameBranchSet_ line of a job of n files
  if [[ $1 -gt 1 ]]; then
    check "$LRUN" "the $1 files of the job have one branch set" \
          "$(grep -q "^\[branches\] $1 input files, the same [0-9]* selected branches in each\." "$LAST.log" && echo 1)"
  fi
}
runa mc_multi main MC "$SM" "" "$CH_MAIN"
common_checks MC "analysis code"
multi_line "$N_MAIN"
check mc_multi "output complete (end marker cutflow_w_full)" "$(complete main && echo 1)"
echo "CUTFLOW $LRUN$(awk '/^=== CutFlow Summary ===/{f=1;next} f&&/ : /{split($0,a," : "); split(a[2],b," "); printf " %s=%s", a[1], b[1]; next} f{exit}' "$LAST.log")"
runa pre_multi prescan MC "$SM" "" "$CH_PRE"
common_checks MC prescan
multi_line "$N_PRE"
check pre_multi "the Runs tree of all $N_PRE files is read" "$(grep -q "^\[Prescan\] Runs tree: scanning $N_PRE input file(s)" "$LAST.log" && echo 1)"
check pre_multi "output complete (one-row prescan tree)" "$(complete prescan && echo 1)"
grep -m1 '^\[Prescan\] Runs tree totals' "$LAST.log" | sed 's/^/PRESCAN pre_multi /'

# ---- 2024 Data ----------------------------------------------------------------------------------------
P4="HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3"
runa dC_pnet main Data "$SC" C "${FPATH[dC_pnet]}"; main_checks Data
check dC_pnet "the [branches] line says the file has the PNet 4J3T path" "$(grep -q "one of {$P4: yes" "$LAST.log" && echo 1)"
if [[ $HAVE_NOPNET == 1 ]]; then
  runa dC_nopnet main Data "$SC" C "${FPATH[dC_nopnet]}"; main_checks Data
  check dC_nopnet "the [branches] line says the file has no PNet 4J3T path" "$(grep -q "one of {$P4: no, .*DeepJet_4p5: yes}" "$LAST.log" && echo 1)"
  runa dC_mix main Data "$SC" C "${FPATH[dC_pnet]},${FPATH[dC_nopnet]}"
  check dC_mix "two era-C files with different HLT branches in one job stop (E11)" \
        "$([[ $(cat "$LAST.rc") == 11 ]] && grep -q 'do not have the same branches' "$LAST.log" && echo 1)" "(exit $(cat "$LAST.rc"))"
fi
runa dI main Data "$SI" I "${FPATH[dI]}"; main_checks Data

# ---- the end ------------------------------------------------------------------------------------------
if [[ ${#FAILED_RUNS[@]} -gt 0 ]]; then
  for r in $(printf '%s\n' "${FAILED_RUNS[@]}" | sort -u); do
    echo "---- last lines of the $r log ($KEEPLOG/$r.log) ----"
    grep -v '^    - Events/' "$W/$r.log" | tail -25
  done
fi
echo "RESULT: $NPASS PASS, $NFAIL FAIL"
[[ $NFAIL -eq 0 ]]
