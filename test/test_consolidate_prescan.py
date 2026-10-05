#!/usr/bin/env python3
"""Synthetic test of consolidate_prescan.py (no /pnfs, a few KB in a temp dir).

Writes fake one-row `prescan` files with PyROOT and checks that
  * skimmed input (Events < Runs) fails without --skimmed, with a warning that
    names --skimmed, and passes with --skimmed (INFO line, events_le_runs);
  * Events > Runs fails even with --skimmed;
  * a ttCat weight-partition mismatch now carries a warning;
  * a missing job index fails;
  * in every case "No anomalies" is printed exactly when the exit code is 0.

Run from the repo top after cmsenv (needs PyROOT):
    python3 test/test_consolidate_prescan.py
Last lines: SUMMARY test_consolidate_prescan PASS|FAIL (<n> checks).
"""
import json
import os
import subprocess
import sys
import tempfile
from array import array

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(HERE)
TOOL = os.environ.get("CONSOLIDATE_TOOL", os.path.join(REPO, "consolidate_prescan.py"))
sys.dont_write_bytecode = True   # no __pycache__ in the repo
sys.path.insert(0, REPO)
import consolidate_prescan as cp  # noqa: E402  (ID_KEYS, TTCAT_KEYS)

try:
    import ROOT  # noqa: E402
except Exception as exc:  # pragma: no cover
    sys.exit(f"[FATAL] PyROOT import failed ({exc}); run after cmsenv.")
ROOT.gROOT.SetBatch(True)

RESULTS = []


def check(name, cond, detail=""):
    RESULTS.append(bool(cond))
    print(f"CHECK {name}: {'PASS' if cond else 'FAIL'}{('  ' + detail) if detail and not cond else ''}")


def write_row(path, is_data, n_ev, sumw, runs_sumw, ttcat_w_scale=1.0):
    """One prescan row; every event is light flavour (id 0 / ttCat LF)."""
    f = ROOT.TFile(path, "RECREATE")
    t = ROOT.TTree("prescan", "prescan")
    keep = []

    def br(name, val, typ):
        a = array({"D": "d", "L": "q", "I": "i"}[typ], [val])
        keep.append(a)
        t.Branch(name, a, f"{name}/{typ}")

    br("isData", 1 if is_data else 0, "I")
    br("xsec_used", 0.5, "D")
    br("nFiles", 1, "I")
    br("nEvents_total", n_ev, "L")
    br("sumGenW_total", sumw, "D")
    br("sumGenW_pos", sumw, "D")
    br("sumGenW_neg", 0.0, "D")
    br("genEventSumw_runs", 0.0 if is_data else runs_sumw, "D")
    br("genEventSumw2_runs", 0.0 if is_data else runs_sumw, "D")
    br("genEventCount_runs", 0 if is_data else int(round(runs_sumw)), "L")
    for k in cp.ID_KEYS:
        br(f"sumGenW_id_{k}", sumw if k == "0" else 0.0, "D")
        br(f"n_id_{k}", n_ev if k == "0" else 0, "L")
    for k in cp.TTCAT_KEYS:
        br(f"sumGenW_ttCat_{k}", sumw * ttcat_w_scale if k == "LightFlavour" else 0.0, "D")
        br(f"n_ttCat_{k}", n_ev if k == "LightFlavour" else 0, "L")
    t.Fill()
    t.Write()
    f.Close()


def make_base(root, layout):
    """layout: {sample: [(index, kwargs), ...]} -> <root>/<sample>/<sample>_<i>.root"""
    for sample, jobs in layout.items():
        os.makedirs(os.path.join(root, sample), exist_ok=True)
        for idx, kw in jobs:
            write_row(os.path.join(root, sample, f"{sample}_{idx}.root"), **kw)


def run(base, outdir, *extra):
    p = subprocess.run([sys.executable, TOOL, "--input-base", base, "--outdir", outdir, *extra],
                       stdout=subprocess.PIPE, stderr=subprocess.STDOUT, universal_newlines=True)
    return p.returncode, p.stdout


def consistent(tag, rc, out):
    no_anom = "No anomalies" in out
    check(f"{tag}: 'No anomalies' iff exit 0", no_anom == (rc == 0), f"rc={rc} no_anomalies={no_anom}")


SKIM = dict(is_data=False, n_ev=400, sumw=400.0, runs_sumw=1000.0)    # skim% -60
FULL = dict(is_data=False, n_ev=1000, sumw=1000.0, runs_sumw=1000.0)  # unskimmed
DATA = dict(is_data=True, n_ev=500, sumw=0.0, runs_sumw=0.0)

with tempfile.TemporaryDirectory(prefix="tthh_cons_") as tmp:
    # A: one skimmed MC sample (2 jobs), one unskimmed MC sample, one Data sample.
    a = os.path.join(tmp, "A")
    make_base(a, {"MC_skim": [(0, SKIM), (1, SKIM)], "MC_full": [(0, FULL)], "Data_X": [(0, DATA)]})
    rc, out = run(a, os.path.join(tmp, "outA0"))
    check("A no flag: exit 1", rc == 1, f"rc={rc}")
    check("A no flag: MC_skim flagged with events_vs_runs_match",
          "[MC_skim]" in out and "failed checks: events_vs_runs_match" in out)
    check("A no flag: warning names --skimmed", "rerun with --skimmed" in out)
    check("A no flag: MC_full not flagged", "[MC_full]" not in out)
    consistent("A no flag", rc, out)

    rc, out = run(a, os.path.join(tmp, "outA1"), "--skimmed")
    check("A --skimmed: exit 0", rc == 0, f"rc={rc}\n{out[-1500:]}")
    check("A --skimmed: INFO line 1 of 2 MC", "for 1 of 2 MC samples" in out)
    consistent("A --skimmed", rc, out)
    js = json.load(open(os.path.join(tmp, "outA1", "prescan_summary.json")))
    check("A --skimmed: JSON meta skimmed true, 0 anomalies",
          js["meta"]["skimmed"] is True and js["meta"]["n_samples_with_anomaly"] == 0)
    cc = js["samples"]["MC_skim"]["crosschecks"]
    check("A --skimmed: MC_skim events_le_runs true, no events_vs_runs_match",
          cc.get("events_le_runs") is True and "events_vs_runs_match" not in cc)
    check("A --skimmed: Runs sum kept (2000), skim -0.6",
          abs(js["samples"]["MC_skim"]["runs"]["genEventSumw"] - 2000.0) < 1e-9
          and abs(js["samples"]["MC_skim"]["skim_attrition_rel"] + 0.6) < 1e-12)

    # B: Events > Runs fails even with --skimmed.
    b = os.path.join(tmp, "B")
    make_base(b, {"MC_over": [(0, dict(is_data=False, n_ev=1000, sumw=1100.0, runs_sumw=1000.0))]})
    rc, out = run(b, os.path.join(tmp, "outB"), "--skimmed")
    check("B --skimmed Events>Runs: exit 1 with events_le_runs",
          rc == 1 and "failed checks: events_le_runs" in out and "a job counted" in out)
    consistent("B", rc, out)

    # C: ttCat weight partition off by 1 % (counts fine) -> warning text now.
    c = os.path.join(tmp, "C")
    make_base(c, {"MC_ttw": [(0, dict(FULL, ttcat_w_scale=1.01))]})
    rc, out = run(c, os.path.join(tmp, "outC"))
    check("C ttCat weight mismatch: exit 1 with warning",
          rc == 1 and "ttCat weight partition mismatch" in out)
    consistent("C", rc, out)

    # D: job index 1 missing.
    d = os.path.join(tmp, "D")
    make_base(d, {"MC_gap": [(0, FULL), (2, FULL)]})
    rc, out = run(d, os.path.join(tmp, "outD"))
    check("D missing job index: exit 1", rc == 1 and "missing job indices (1): [1]" in out)
    consistent("D", rc, out)

    # E: unskimmed MC + Data only -> clean, as before.
    e = os.path.join(tmp, "E")
    make_base(e, {"MC_full": [(0, FULL), (1, FULL)], "Data_X": [(0, DATA)]})
    rc, out = run(e, os.path.join(tmp, "outE"))
    check("E unskimmed: exit 0, No anomalies", rc == 0 and "No anomalies" in out, out[-1500:])

    # F: --filelist-dir (2026-10-06): MC files read (sum of nFiles, 1 per fake job) vs filelist lines; Data skipped.
    fl = os.path.join(tmp, "filelists")
    os.makedirs(fl)
    with open(os.path.join(fl, "filelist_MC_full.txt"), "w") as fh:
        fh.write("# comment\nfile_a.root\nfile_b.root\n\n")
    rc, out = run(e, os.path.join(tmp, "outF1"), "--filelist-dir", fl)
    check("F filelist 2 lines = 2 files read: exit 0", rc == 0 and "No anomalies" in out, out[-1500:])
    with open(os.path.join(fl, "filelist_MC_full.txt"), "a") as fh:
        fh.write("file_c.root\n")
    rc, out = run(e, os.path.join(tmp, "outF2"), "--filelist-dir", fl)
    check("F filelist 3 lines, 2 read: exit 1 with files_match_filelist",
          rc == 1 and "failed checks: files_match_filelist" in out and "Runs read in 2 files, the filelist has 3" in out)
    check("F Data sample not compared", "[Data_X]" not in out)
    consistent("F", rc, out)
    rc, out = run(a, os.path.join(tmp, "outF3"), "--skimmed", "--filelist-dir", fl)
    check("F no filelist for MC_skim: exit 1 with a warning", rc == 1 and "no filelist" in out)
    consistent("F no filelist", rc, out)

n_fail = RESULTS.count(False)
print(f"SUMMARY test_consolidate_prescan {'PASS' if n_fail == 0 else 'FAIL'} "
      f"({len(RESULTS) - n_fail}/{len(RESULTS)} checks)")
sys.exit(0 if n_fail == 0 else 1)
