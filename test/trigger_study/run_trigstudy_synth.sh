#!/bin/bash
# =============================================================================
#  run_trigstudy_synth.sh -- TriggerStudy on synthetic btagtrig skims with a KNOWN Data/MC efficiency ratio
#  (STEP 26 L): no /pnfs, no analyzer run needed
#
#    /bin/bash test/trigger_study/run_trigstudy_synth.sh <built repo> [--year 2024|2017|both] [--orig <older built repo>]
#                                                        [--keep]
#
#  <built repo>: a tempTTHH checkout whose TriggerStudy/exe_TrigStudy is built (cd TriggerStudy && make). Needs root,
#  hadd, PyROOT (cmsenv) and correctionlib's Python module. --year: default both. --orig <older built repo>: a checkout
#  of before STEP 26 with its exe_TrigStudy built: the 2017 outputs of the two are compared (the 2017 behaviour must
#  not change). Made in a fresh work directory (printed; removed unless --keep): the skims (synth_trigskim.py:
#  every event in the TriggerStudy binning, the hadronic OR firing in Data with r(nb) x the MC efficiency), then
#  TriggerStudy/run_analysis.sh --year <Y> --workdir <work>/run_<Y> on them.
#  Checks (CHECK <name> PASS|FAIL), per year: run_analysis.sh ends with RESULT OK; the [TrigStudy] line of every
#  sample in both steps = the generator's counts (entries, muon CR, reference, hadronic OR; the reference of the
#  year); the JSON carries year=<Y> and the reference (triggerSF, triggerSF_err, the set); every possible bin is
#  measured and its SF = the injected r(nb) within errors (pulls); the JSON = TriggerSF.root (SF_ and SF_error_ at
#  every bin centre, and clamped outside); closure: the MC efficiency with the SF = the Data one within 1 %, without
#  it off by the injected ratio. 2024 also: a skim with a _B slot set stops (exit 1), one without
#  passTrigger_HLT_IsoMu24 stops (exit 1), TTHH_YEAR=2018 stops (exit 1); inputs of another year stop (exit 1):
#  TTHH_YEAR unset on a 2024 skim, the 2017 xsec_db or the 2017 prescan with TTHH_YEAR=2024, a 2017 Data name in a
#  2024 run; a sample whose reference never fires stops; DeriveSF on maps without one measured bin ends with RESULT FAIL
#  and exit 1, keeps its JSON as .FAILED and an earlier one as .previous; DeriveSF without inputs touches nothing. Both
#  years: the 2017 Step 1 / Step 2 outputs through DeriveSF / PlotTriggerEfficiency with TTHH_YEAR=2024 end with RESULT
#  FAIL (the TrigStudyStamp). With --orig (2017): the Step 1 maps, the JSON values (its descriptions differ: the year
#  tag) and the Step 2 histograms are identical to the older build's, and this DeriveSF / PlotTriggerEfficiency take
#  the older build's outputs (no stamp) for 2017.
#  Exit: 0 all checks pass; 1 a check failed; 2 bad usage.
#  The values are random: the checks are about the derivation chain, never physics.
# =============================================================================
set -u
KEEP=0; YEARS="2024 2017"; NEW=""; ORIG=""
while [[ $# -gt 0 ]]; do
  case "$1" in
    --keep) KEEP=1; shift ;;
    --year) case "${2:-}" in 2024|2017) YEARS="$2" ;; both) YEARS="2024 2017" ;; *) echo "bad --year '${2:-}'"; exit 2 ;; esac; shift 2 ;;
    --orig) ORIG="$(cd "${2:?--orig needs a directory}" && pwd -P)" || exit 2; shift 2 ;;
    -h|--help) sed -n '3,32p' "$0" | sed 's/^# \{0,2\}//'; exit 0 ;;
    *) [[ -z "$NEW" ]] || { echo "unexpected argument: $1"; exit 2; }; NEW="$(cd "$1" && pwd -P)" || exit 2; shift ;;
  esac
done
[[ -n "$NEW" ]] || { sed -n '3,32p' "$0" | sed 's/^# \{0,2\}//'; exit 2; }
for b in "$NEW" $ORIG; do [[ -x "$b/TriggerStudy/exe_TrigStudy" ]] || { echo "ERROR no $b/TriggerStudy/exe_TrigStudy (make in TriggerStudy)"; exit 2; }; done
for tool in root hadd python3; do command -v "$tool" > /dev/null || { echo "ERROR '$tool' not in PATH (cmsenv?)"; exit 2; }; done
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
W="$(mktemp -d "${TMPDIR:-/tmp}/tthh_trigstudy_synth.XXXXXX")" || exit 2
echo "WORK $W"
[[ $KEEP -eq 1 ]] || trap 'rm -rf "$W"' EXIT
NPASS=0; NFAIL=0
check() { if [[ "$2" == 1 ]]; then echo "CHECK $1 PASS"; NPASS=$((NPASS+1)); else echo "CHECK $1 FAIL ${3:-}"; NFAIL=$((NFAIL+1)); fi; }
PY() { ( cd "$W" && python3 - "$@" 2>> "$W/py.log" ); }     # Python from the work dir (no repo modules on its path)

for Y in $YEARS; do
  SK="$W/skim_$Y"; RUN="$W/run_$Y"
  if [[ "$Y" == 2024 ]]; then
    DB="$NEW/data/samples_2024.json"; PRE="$SK/prescan_fake.json"; REF=HLT_IsoMu24; NMC=40000; NDATA=80000
  else
    DB="$NEW/data/samples_2017UL.json"; PRE="$NEW/prescan_summary/prescan_summary.json"; REF=HLT_IsoMu27; NMC=40000; NDATA=80000
  fi
  python3 "$HERE/synth_trigskim.py" --year "$Y" --xsec-db "$DB" -o "$SK" --n-mc $NMC --n-data $NDATA --seed 7 \
          $([[ "$Y" == 2024 ]] && echo --prescan-out "$PRE") > "$W/synth_$Y.log" 2>&1 \
    || { echo "ERROR synth_trigskim.py $Y"; tail -5 "$W/synth_$Y.log"; exit 1; }
  ( export TTHH_SKIM_DIR="$SK" TTHH_XSEC_DB="$DB" TTHH_PRESCAN="$PRE" TTHH_BASE="$NEW"
    bash "$NEW/TriggerStudy/run_analysis.sh" --year "$Y" --workdir "$RUN" ) > "$W/run_$Y.log" 2>&1
  rc=$?
  check "$Y run_analysis.sh ends with RESULT OK" "$([[ $rc == 0 ]] && grep -q "^RESULT OK (run_analysis $Y)" "$W/run_$Y.log" && echo 1)" "(exit $rc, $W/run_$Y.log)"
  # the [TrigStudy] lines against the generator's counts
  CNT=$(PY "$W/run_$Y.log" "$SK/expected.json" "$Y" "$REF" <<'PY'
import json, re, sys
log, exp, year, ref = sys.argv[1:5]
e = json.load(open(exp))["samples"]
rx = re.compile(r"^\[TrigStudy\] (\d{4}) (\S+) \((Data|MC), mode (eff|applySF)\): entries (\d+), muon CR (\d+), (\S+) (\d+), "
                r"hadronic OR (\d+) ")
got = {}
for line in open(log):
    m = rx.match(line)
    if m:
        got[(m.group(2), m.group(4))] = (m.group(1), m.group(3), int(m.group(5)), int(m.group(6)), m.group(7), int(m.group(8)), int(m.group(9)))
bad = []
for s, c in e.items():
    for mode in ("eff", "applySF"):
        g = got.get((s, mode))
        want = (year, c["kind"], c["entries"], c["muonCR"], ref, c["ref"], c["had"])
        if g != want:
            bad.append("%s/%s: %s != %s" % (s, mode, g, want))
print(len(e), len(got), len(bad), "; ".join(bad[:3]))
PY
)
  read -r c_ns c_ng c_bad c_txt <<< "${CNT:-}"
  check "$Y [TrigStudy] lines of all ${c_ns:-?} samples x 2 steps = the generator's counts (reference $REF)" \
        "$([[ "${c_bad:-1}" == 0 && "${c_ng:-0}" -gt 0 ]] && echo 1)" "(${c_ng:-?} lines; ${c_txt:-})"
  # the JSON: the year tag, the reference, both corrections; the SF recovery; JSON = TriggerSF.root; the closure
  RES=$(PY "$RUN" "$SK/expected.json" "$Y" "$REF" <<'PY'
import gzip, json, math, sys
run, exp, year, ref = sys.argv[1:5]
out = {}
js = json.load(gzip.open(run + "/trigger_sf.json.gz", "rt"))
corr = {c["name"]: c for c in js["corrections"]}
tag = "year=%s; reference=%s" % (year, ref)
out["tag"] = int(all(tag in d for d in (js["description"], corr.get("triggerSF", {}).get("description", ""),
                                         corr.get("triggerSF_err", {}).get("description", "")))
                 and [i["name"] for i in corr["triggerSF"]["inputs"]] == ["nbJets", "eta", "ht", "pt"])
import ROOT
ROOT.gROOT.SetBatch(True)
r = json.load(open(exp))["ratio"]
HT = [500, 550, 600, 700, 800, 1000, 1500, 2500]; PT = [40, 45, 50, 60, 70, 120, 200]
keys = ["nB0to2_Eta_Inc", "nB3_Eta_Inc", "nB4p_Eta_Inc"]
f = ROOT.TFile.Open(run + "/TriggerSF.root")
pulls, nposs, nmeas, maxdiff = [], 0, 0, 0.0
import correctionlib
cs = correctionlib.CorrectionSet.from_file(run + "/trigger_sf.json.gz")
sf, er = cs["triggerSF"], cs["triggerSF_err"]
nbrep = [1, 3, 5]
for k, key in enumerate(keys):
    hm = f.Get("Histograms/SF_measured_" + key); hf = f.Get("Histograms/SF_" + key); he = f.Get("Histograms/SF_error_" + key)
    for i in range(1, len(HT)):
        for j in range(1, len(PT)):
            possible = 6.05 * PT[j - 1] < HT[i]
            nposs += possible
            v, e = hm.GetBinContent(i, j), hm.GetBinError(i, j)
            if possible and v > 0 and e > 0:
                nmeas += 1
                pulls.append((v - r[k]) / e)
            x, y = hf.GetXaxis().GetBinCenter(i), hf.GetYaxis().GetBinCenter(j)
            maxdiff = max(maxdiff, abs(sf.evaluate(nbrep[k], 0.0, x, y) - hf.GetBinContent(i, j)),
                          abs(er.evaluate(nbrep[k], 0.0, x, y) - he.GetBinContent(i, j)))
    # flow clamp: below the first and above the last bin = the edge bins
    for (x, y, i, j) in ((400.0, 30.0, 1, 1), (3000.0, 300.0, len(HT) - 1, len(PT) - 1)):
        maxdiff = max(maxdiff, abs(sf.evaluate(nbrep[k], 0.0, x, y) - hf.GetBinContent(i, j)))
mean = sum(pulls) / len(pulls) if pulls else 99.0
f3 = sum(1 for p in pulls if abs(p) < 3.0) / len(pulls) if pulls else 0.0
out.update(nposs=nposs, nmeas=nmeas, mean="%.3f" % mean, f3="%.3f" % f3, maxdiff="%.2g" % maxdiff)
def eff(fn, suffix):
    g = ROOT.TFile.Open(run + "/" + fn)
    p, t = g.Get("h_HT_Pass_" + suffix).Integral(), g.Get("h_HT_Total_" + suffix).Integral()
    return p / t if t > 0 else 0.0
ed = eff("validated_SingleMuon.root", "noSF")
out.update(clos_sf="%.4f" % (eff("validated_TTbarInc.root", "SF") / ed), clos_nosf="%.4f" % (eff("validated_TTbarInc.root", "noSF") / ed))
print(" ".join("%s=%s" % kv for kv in out.items()))
PY
)
  getv() { sed -nE "s/.*\<$1=([^ ]*).*/\1/p" <<< "${RES:-}"; }
  check "$Y JSON: year=$Y; reference=$REF in triggerSF, triggerSF_err and the set; inputs (nbJets, eta, ht, pt)" "$([[ "$(getv tag)" == 1 ]] && echo 1)" "(${RES:-no result; $W/py.log})"
  check "$Y every possible bin measured ($(getv nmeas) of $(getv nposs)) and SF = the injected r(nb) (mean pull $(getv mean), |pull| < 3: $(getv f3))" \
        "$([[ -n "$(getv nposs)" && "$(getv nmeas)" == "$(getv nposs)" ]] && awk -v m="$(getv mean)" -v f="$(getv f3)" 'BEGIN{exit !(m > -0.35 && m < 0.35 && f >= 0.97)}' && echo 1)"
  check "$Y JSON = TriggerSF.root (SF_ and SF_error_ at every bin centre, clamped outside; largest difference $(getv maxdiff))" \
        "$(awk -v d="$(getv maxdiff)" 'BEGIN{exit !(d != "" && d < 1e-6)}' && echo 1)"
  check "$Y closure: MC efficiency with the SF / Data = $(getv clos_sf) (within 1 %), without it $(getv clos_nosf) (off by the injected ratio)" \
        "$(awk -v a="$(getv clos_sf)" -v b="$(getv clos_nosf)" 'BEGIN{exit !(a > 0.99 && a < 1.01 && (b > 1.04 || b < 0.96))}' && echo 1)"
  NPDF=$(ls -1 "$RUN"/*.pdf 2>/dev/null | wc -l)
  check "$Y plots made ($NPDF PDF; DeriveSF maps and the validation plots)" "$([[ $NPDF -ge 10 ]] && echo 1)"

  if [[ "$Y" == 2024 ]]; then
    # the guards: a _B slot set, no passTrigger_HLT_IsoMu24, a year not implemented
    S0="Muon0_Run2024C-MINIv6NANOv15-v1"
    for v in badB noIsoMu24; do
      python3 "$HERE/synth_trigskim.py" --year 2024 --xsec-db "$DB" -o "$W/skim_$v" --variant "$v" --sample "$S0" >> "$W/synth_$Y.log" 2>&1
      mkdir -p "$W/g_$v"
      ( cd "$W/g_$v" && TTHH_YEAR=2024 TTHH_SKIM_DIR="$W/skim_$v" TTHH_XSEC_DB="$DB" TTHH_PRESCAN="$PRE" TTHH_BASE="$NEW" \
        "$NEW/TriggerStudy/exe_TrigStudy" "$S0" 0 ) > "$W/g_$v.log" 2>&1
      echo $? > "$W/g_$v.rc"
    done
    check "2024 a skim with a _B slot set stops (exit 1, the convention named)" \
          "$([[ $(cat "$W/g_badB.rc") == 1 ]] && grep -q 'a _B trigger slot is set in a 2024 skim' "$W/g_badB.log" && echo 1)" "(exit $(cat "$W/g_badB.rc"))"
    check "2024 a skim without passTrigger_HLT_IsoMu24 stops (exit 1), not empty maps" \
          "$([[ $(cat "$W/g_noIsoMu24.rc") == 1 ]] && grep -q "needs the branch 'passTrigger_HLT_IsoMu24'" "$W/g_noIsoMu24.log" && echo 1)" "(exit $(cat "$W/g_noIsoMu24.rc"))"
    mkdir -p "$W/g_2018"
    ( cd "$W/g_2018" && TTHH_YEAR=2018 TTHH_SKIM_DIR="$SK" TTHH_XSEC_DB="$DB" TTHH_PRESCAN="$PRE" TTHH_BASE="$NEW" \
      "$NEW/TriggerStudy/exe_TrigStudy" TTbar_DiLep 0 ) > "$W/g_2018.log" 2>&1; rc=$?
    check "TTHH_YEAR=2018 stops (exit 1: only 2017 and 2024 are implemented)" \
          "$([[ $rc == 1 ]] && grep -q "TTHH_YEAR='2018': only 2017 and 2024 are implemented" "$W/g_2018.log" && echo 1)" "(exit $rc)"
    # [review 10-09] inputs of another year: TTHH_YEAR unset on a 2024 skim; the 2017 xsec_db with TTHH_YEAR=2024;
    #   a Data name of 2017 in a 2024 run; a reference that never fires
    gx() {   # name year(- = unset) skimdir db prescan sample -> exit code in $W/gx_<name>.rc, log $W/gx_<name>.log
      local n="$1" y="$2" sk="$3" db="$4" pre="$5" smp="$6"; mkdir -p "$W/gx_$n"
      ( cd "$W/gx_$n" && unset TTHH_YEAR && { [[ "$y" == - ]] || export TTHH_YEAR="$y"; } &&
        TTHH_SKIM_DIR="$sk" TTHH_XSEC_DB="$db" TTHH_PRESCAN="$pre" TTHH_BASE="$NEW" "$NEW/TriggerStudy/exe_TrigStudy" "$smp" 0 ) \
        > "$W/gx_$n.log" 2>&1
      echo $? > "$W/gx_$n.rc"
    }
    gx unset - "$SK" "$NEW/data/samples_2017UL.json" "$NEW/prescan_summary/prescan_summary.json" TTbar_DiLep
    check "2024 skim with TTHH_YEAR unset (= 2017) stops (exit 1: it has passTrigger_HLT_IsoMu24), not a 2017 evaluation" \
          "$([[ $(cat "$W/gx_unset.rc") == 1 ]] && grep -q "(unset: the default) but .* has 'passTrigger_HLT_IsoMu24'" "$W/gx_unset.log" && echo 1)" "(exit $(cat "$W/gx_unset.rc"))"
    gx db17 2024 "$SK" "$NEW/data/samples_2017UL.json" "$NEW/prescan_summary/prescan_summary.json" TTbar_DiLep
    check "TTHH_YEAR=2024 with the 2017 xsec_db stops (exit 1: _meta.era 2017UL), not 2017 MC weights" \
          "$([[ $(cat "$W/gx_db17.rc") == 1 ]] && grep -q "is for era '2017UL' (_meta.era)" "$W/gx_db17.log" && echo 1)" "(exit $(cat "$W/gx_db17.rc"))"
    gx pre17 2024 "$SK" "$DB" "$NEW/prescan_summary/prescan_summary.json" TTbar_DiLep
    check "TTHH_YEAR=2024 with the 2024 xsec_db but the 2017 prescan stops (exit 1: meta.input_base), not 2017 genEventSumw" \
          "$([[ $(cat "$W/gx_pre17.rc") == 1 ]] && grep -q "prescan summary .* is of '2017' (meta.year, else meta.input_base)" "$W/gx_pre17.log" && echo 1)" "(exit $(cat "$W/gx_pre17.rc"))"
    mkdir -p "$W/skim_g4"; cp "$SK/$S0.root" "$W/skim_g4/SingleMuon_Run2017B.root"
    PY "$DB" "$W/db_g4.json" <<'PY'
import json, sys
d = json.load(open(sys.argv[1])); d["SingleMuon_Run2017B"] = {"cross_section_fb": None}
json.dump(d, open(sys.argv[2], "w"))
PY
    gx g4 2024 "$W/skim_g4" "$W/db_g4.json" "$PRE" SingleMuon_Run2017B
    check "a Data sample of 2017 in a 2024 run stops (exit 1)" \
          "$([[ $(cat "$W/gx_g4.rc") == 1 ]] && grep -q "but the Data sample 'SingleMuon_Run2017B' is of 2017" "$W/gx_g4.log" && echo 1)" "(exit $(cat "$W/gx_g4.rc"))"
    python3 "$HERE/synth_trigskim.py" --year 2024 --xsec-db "$DB" -o "$W/skim_noRef" --variant noRef --sample "$S0" >> "$W/synth_$Y.log" 2>&1
    gx noref 2024 "$W/skim_noRef" "$DB" "$PRE" "$S0"
    check "2024 a sample whose reference never fires stops (exit 1), not empty maps" \
          "$([[ $(cat "$W/gx_noref.rc") == 1 ]] && grep -q "muon-CR events, none fired the reference HLT_IsoMu24" "$W/gx_noref.log" && echo 1)" "(exit $(cat "$W/gx_noref.rc"))"
    # DeriveSF on maps with too few events: RESULT FAIL, exit 1, no trigger_sf.json.gz (an earlier one removed), the
    #   JSON of the run kept as .FAILED
    for smp in "$S0" TTbar_DiLep; do
      python3 "$HERE/synth_trigskim.py" --year 2024 --xsec-db "$DB" -o "$W/skim_tiny" --variant tiny --sample "$smp" >> "$W/synth_$Y.log" 2>&1
      gx "tiny_$smp" 2024 "$W/skim_tiny" "$DB" "$PRE" "$smp"
    done
    mkdir -p "$W/d_tiny"
    cp "$W/gx_tiny_$S0/output_$S0.root" "$W/d_tiny/output_SingleMuon.root"
    cp "$W/gx_tiny_TTbar_DiLep/output_TTbar_DiLep.root" "$W/d_tiny/output_TTbarInc.root"
    echo earlier > "$W/d_tiny/trigger_sf.json.gz"
    ( cd "$W/d_tiny" && TTHH_YEAR=2024 ROOT_INCLUDE_PATH="$NEW/TriggerStudy:$NEW/TriggerStudy/include:$NEW/include" \
      TTHH_BASE="$NEW" root -l -b -q "$NEW/TriggerStudy/DeriveSF.cpp" ) > "$W/d_tiny.log" 2>&1; rc=$?
    check "DeriveSF on maps without one measured bin: RESULT FAIL, exit 1; its JSON kept as .FAILED, an earlier one moved to .previous (nothing deleted)" \
          "$([[ $rc == 1 ]] && grep -q '^RESULT FAIL (DeriveSF 2024: no measured bin in nB0to2_Eta_Inc nB3_Eta_Inc nB4p_Eta_Inc' "$W/d_tiny.log" \
             && [[ ! -e "$W/d_tiny/trigger_sf.json.gz" && -s "$W/d_tiny/trigger_sf.json.gz.FAILED" ]] \
             && [[ "$(cat "$W/d_tiny/trigger_sf.json.gz.previous" 2>/dev/null)" == earlier ]] && echo 1)" "(exit $rc, $W/d_tiny.log)"
    # a DeriveSF that fails before deriving (no inputs) leaves the directory as it was (a production JSON there stays)
    mkdir -p "$W/d_none"; echo production > "$W/d_none/trigger_sf.json.gz"
    ( cd "$W/d_none" && TTHH_YEAR=2024 ROOT_INCLUDE_PATH="$NEW/TriggerStudy:$NEW/TriggerStudy/include:$NEW/include" \
      TTHH_BASE="$NEW" root -l -b -q "$NEW/TriggerStudy/DeriveSF.cpp" ) > "$W/d_none.log" 2>&1; rc=$?
    check "DeriveSF without its inputs: RESULT FAIL, exit 1, the trigger_sf.json.gz already there untouched" \
          "$([[ $rc == 1 ]] && grep -q '^RESULT FAIL (DeriveSF 2024: cannot open output_SingleMuon.root)' "$W/d_none.log" \
             && [[ "$(cat "$W/d_none/trigger_sf.json.gz")" == production && ! -e "$W/d_none/trigger_sf.json.gz.previous" ]] && echo 1)" "(exit $rc)"
  fi

  if [[ "$Y" == 2017 && -n "$ORIG" ]]; then
    # the older build on the same skims: Step 1, DeriveSF, Step 2 (by hand: its run_analysis.sh runs next to the code)
    O="$W/orig_2017"; mkdir -p "$O"
    SAMPLES=$(PY "$SK/expected.json" <<'PY'
import json, sys
print(" ".join(json.load(open(sys.argv[1]))["samples"]))
PY
)
    ok=1
    ( cd "$O" && export TTHH_SKIM_DIR="$SK" TTHH_XSEC_DB="$DB" TTHH_PRESCAN="$PRE" TTHH_BASE="$ORIG"
      export ROOT_INCLUDE_PATH="$ORIG/TriggerStudy:$ORIG/TriggerStudy/include:$ORIG/include${ROOT_INCLUDE_PATH:+:$ROOT_INCLUDE_PATH}"
      for s in $SAMPLES; do "$ORIG/TriggerStudy/exe_TrigStudy" "$s" 0 > "log_0_$s.txt" 2>&1 || exit 1; done
      hadd -f output_SingleMuon.root output_SingleMuon_Run2017?.root > hadd.log 2>&1 || exit 1
      hadd -f output_TTbarInc.root output_TTbar_DiLep.root output_TTbar_Hadronic.root output_TTbar_SemiLep.root >> hadd.log 2>&1 || exit 1
      root -l -b -q "$ORIG/TriggerStudy/DeriveSF.cpp" > DeriveSF.log 2>&1 || exit 1
      for s in $SAMPLES; do "$ORIG/TriggerStudy/exe_TrigStudy" "$s" 1 > "log_1_$s.txt" 2>&1 || exit 1; done
      hadd -f validated_SingleMuon.root validated_SingleMuon_Run2017?.root >> hadd.log 2>&1 || exit 1
      hadd -f validated_TTbarInc.root validated_TTbar_DiLep.root validated_TTbar_Hadronic.root validated_TTbar_SemiLep.root >> hadd.log 2>&1 || exit 1
    ) > "$W/orig_2017.log" 2>&1 || ok=0
    check "2017 the older build runs the same chain on these skims" "$ok" "($W/orig_2017.log, $O)"
    CMP=$(PY "$RUN" "$O" "$SAMPLES" <<'PY'
import gzip, json, sys
import ROOT
ROOT.gROOT.SetBatch(True)
new, old, samples = sys.argv[1], sys.argv[2], sys.argv[3].split()
def hists(fn):
    f = ROOT.TFile.Open(fn); d = {}
    def walk(dd, pre):
        for k in dd.GetListOfKeys():
            o = k.ReadObj(); name = pre + k.GetName()
            if o.InheritsFrom("TDirectory"):
                walk(o, name + "/")
            elif o.InheritsFrom("TH1"):
                n = (o.GetNbinsX() + 2) * (o.GetNbinsY() + 2) * (o.GetNbinsZ() + 2)
                d[name] = [(o.GetBinContent(i), o.GetBinError(i)) for i in range(n)]
    walk(f, "")
    return d
def same(a, b):
    ha, hb = hists(a), hists(b)
    return set(ha) == set(hb) and all(ha[k] == hb[k] for k in ha), len(ha)
s1 = [same("%s/output_%s.root" % (new, s), "%s/output_%s.root" % (old, s)) for s in samples]
s2 = [same("%s/validated_%s.root" % (new, s), "%s/validated_%s.root" % (old, s)) for s in samples]
def data_nodes(fn):
    js = json.load(gzip.open(fn, "rt"))
    return [(c["name"], c["inputs"] and [i["name"] for i in c["inputs"]], c["output"]["name"], c["data"]) for c in js["corrections"]]
jn, jo = data_nodes(new + "/trigger_sf.json.gz"), data_nodes(old + "/trigger_sf.json.gz")
sfr = same(new + "/TriggerSF.root", old + "/TriggerSF.root")
print(int(all(x[0] for x in s1)), sum(x[1] for x in s1), int(all(x[0] for x in s2)), sum(x[1] for x in s2),
      int(jn == jo), len(jn), int(sfr[0]), sfr[1])
PY
)
    read -r m1 n1 m2 n2 mj nj ms ns <<< "${CMP:-}"
    check "2017 Step 1 maps identical to the older build (${n1:-?} histograms in the output_ files)" "$([[ "${m1:-0}" == 1 ]] && echo 1)"
    check "2017 JSON values identical (${nj:-?} corrections: inputs, output, data; the descriptions differ: the year tag)" "$([[ "${mj:-0}" == 1 ]] && echo 1)"
    check "2017 TriggerSF.root identical (${ns:-?} histograms; the plot titles are not in it)" "$([[ "${ms:-0}" == 1 ]] && echo 1)"
    check "2017 Step 2 histograms identical (${n2:-?} histograms in the validated_ files)" "$([[ "${m2:-0}" == 1 ]] && echo 1)"
    # the outputs of the older build have no TrigStudyStamp (before STEP 26): this DeriveSF and PlotTriggerEfficiency
    #   take them for 2017 (the KNU 2017 Step 1 outputs) and give the older build's JSON values
    mkdir -p "$W/d_old"
    for f in output_SingleMuon output_TTbarInc validated_SingleMuon validated_TTbarInc; do cp "$O/$f.root" "$W/d_old/"; done
    for m in DeriveSF PlotTriggerEfficiency; do
      ( cd "$W/d_old" && TTHH_YEAR=2017 ROOT_INCLUDE_PATH="$NEW/TriggerStudy:$NEW/TriggerStudy/include:$NEW/include" \
        TTHH_BASE="$NEW" root -l -b -q "$NEW/TriggerStudy/$m.cpp" ) > "$W/d_old_$m.log" 2>&1
      echo $? > "$W/d_old_$m.rc"
    done
    SAMEJ=$(PY "$W/d_old/trigger_sf.json.gz" "$O/trigger_sf.json.gz" <<'PY'
import gzip, json, sys
try:
    a, b = (json.load(gzip.open(x, "rt")) for x in sys.argv[1:3])
    print(int([c["data"] for c in a["corrections"]] == [c["data"] for c in b["corrections"]]))
except Exception:
    print(0)
PY
)
    check "2017 Step 1 / Step 2 outputs of the older build (no stamp): DeriveSF and PlotTriggerEfficiency RESULT OK, the same JSON values" \
          "$([[ $(cat "$W/d_old_DeriveSF.rc") == 0 && $(cat "$W/d_old_PlotTriggerEfficiency.rc") == 0 && "${SAMEJ:-0}" == 1 ]] \
             && grep -q 'Step 1 stamps (year=2017; reference=HLT_IsoMu27): Data 0 file(s) ok, MC 0 file(s) ok' "$W/d_old_DeriveSF.log" && echo 1)" \
          "(exit $(cat "$W/d_old_DeriveSF.rc") / $(cat "$W/d_old_PlotTriggerEfficiency.rc"), same JSON ${SAMEJ:-?})"
  fi
done

# [review 10-09] Step 1 / Step 2 outputs of 2017 through the macros with TTHH_YEAR=2024: refused (TrigStudyStamp)
if [[ -f "$W/run_2017/output_SingleMuon.root" && " $YEARS " == *" 2024 "* ]]; then
  mkdir -p "$W/d_mix"
  for f in output_SingleMuon output_TTbarInc validated_SingleMuon validated_TTbarInc; do cp "$W/run_2017/$f.root" "$W/d_mix/"; done
  for m in DeriveSF PlotTriggerEfficiency; do
    ( cd "$W/d_mix" && TTHH_YEAR=2024 ROOT_INCLUDE_PATH="$NEW/TriggerStudy:$NEW/TriggerStudy/include:$NEW/include" \
      TTHH_BASE="$NEW" root -l -b -q "$NEW/TriggerStudy/$m.cpp" ) > "$W/d_mix_$m.log" 2>&1
    echo $? > "$W/d_mix_$m.rc"
  done
  check "the 2017 Step 1 maps through DeriveSF with TTHH_YEAR=2024: RESULT FAIL, exit 1, no JSON (TrigStudyStamp)" \
        "$([[ $(cat "$W/d_mix_DeriveSF.rc") == 1 ]] && grep -q "made with 'year=2017; reference=HLT_IsoMu27', this run is 'year=2024; reference=HLT_IsoMu24'" "$W/d_mix_DeriveSF.log" \
           && grep -q '^RESULT FAIL (DeriveSF 2024' "$W/d_mix_DeriveSF.log" && [[ ! -e "$W/d_mix/trigger_sf.json.gz" ]] && echo 1)" "($W/d_mix_DeriveSF.log)"
  check "... and its Step 2 histograms through PlotTriggerEfficiency: RESULT FAIL, exit 1" \
        "$([[ $(cat "$W/d_mix_PlotTriggerEfficiency.rc") == 1 ]] && grep -q "^RESULT FAIL (PlotTriggerEfficiency 2024: not this year's Step 2 output)" "$W/d_mix_PlotTriggerEfficiency.log" && echo 1)"
fi

echo "RESULT: $NPASS PASS, $NFAIL FAIL"
[[ $NFAIL -eq 0 ]]
