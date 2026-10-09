#!/usr/bin/env python3
"""Synthetic test of the failure checks added 2026-10-06 (no /pnfs, no condor; a few KB in a temp dir).

  A  plotter/make_plots.py --check-only: the merged unweighted noCut must equal the prescan event count (MC);
     a difference is a FLAG and RESULT FAIL
  B  outputMerger/merge_outputs.py --config: job indices <proc>_0..N-1 against the yml (missing / extra / not in
     the yml / absent), a local merge with the worker's input-count check (exit 7 on a wrong count), --report over a
     local and a condor work directory, the --resubmit selection
  C  submit_job_FH_Tier3_unified.py: why a job is missing (condor user log + job .out + arguments file):
     failed with the return value and its name, rc 0 without the end marker, pending, held, not started, no
     attempt; the report table with the fail / wait columns
  D  plotter/make_plots.py --tree-cut (D-2026-10-06-A): control histograms from Tree/Tree -- the yields after the
     cut (MC by evtWeight, Data by 1), the Control/ histograms in <out>/tree/, only those plotted, a cut on a
     missing branch fails, --tree-label needs --tree-cut; [STEP 24 I] the jet-multiplicity diagnostics (nJets above
     40 / 50 GeV, central / forward, per b-tag multiplicity, the score of every jet) and the TREEFLAV table
  E  [STEP 24 J] submit_job_FH_Tier3_unified.py: the weight inputs of every sample at once (a Data sample needs its
     xsec_db entry too; the submission stops before its first sample, E20, nothing queued), --only (fnmatch patterns),
     and the preflight lines for both (the preflight runs on a copy of the script in the temp dir: its log goes there)
  F  [STEP 25 K] the 2024 fixed-WP b-tag weight in the submitter (D-2026-10-08-A): common.path_btag_eff_json reaches
     every 2024 job as TTHH_BTAGEFF_JSON (null -> __NULL__; absent -> E12; 2017 without the key: not exported);
     --btagsf on makes it required for MC (null -> E13 at submission; Data not); the preflight: null + --btagsf on
     FAIL, a usable JSON PASS with its groups, a JSON of another year FAIL, the key absent FAIL, --btagsf off PASS;
     its reading of the JSON node by node (a groups correction without default, a WP missing)
  G  [STEP 25 K2] the condor stall guard of the submit files (2026-10-08: 7 jobs 'running' 12 h after their /pnfs
     reads hung): periodic_hold / _reason / _subcode / periodic_release / requirements written as stall_guard_exprs(),
     none with --stall-guard off, the preflight line; with a ClassAd module (classad2 or classad: pip install
     htcondor) also their evaluation -- the 10-08 jobs held, healthy or young or idle jobs not, our hold released
     twice at most and only after 5 min, other holds not, the machine of the last run refused (else 'SKIP G ...')

Run from the repo top after cmsenv (needs PyROOT; part B's merge needs hadd):
    python3 test/test_failure_checks.py
Last line: SUMMARY test_failure_checks PASS|FAIL (<n>/<m> checks): 59 checks, 60 with a ClassAd module.
"""
import contextlib
import importlib.util
import io
import json
import os
import shutil
import subprocess
import sys
import tempfile
import time
from array import array

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(HERE)
sys.dont_write_bytecode = True

try:
    import ROOT  # noqa: E402
except Exception as exc:  # pragma: no cover
    sys.exit(f"[FATAL] PyROOT import failed ({exc}); run after cmsenv.")
ROOT.gROOT.SetBatch(True)
ROOT.gErrorIgnoreLevel = ROOT.kError

RESULTS = []
LABELS = ["noCut", "HadTrigger", "nTotal"]


def check(name, cond, detail=""):
    RESULTS.append(bool(cond))
    print(f"CHECK {name}: {'PASS' if cond else 'FAIL'}" + (f"\n{detail}" if detail and not cond else ""))


def load(mod_name, rel):
    spec = importlib.util.spec_from_file_location(mod_name, os.path.join(REPO, rel))
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def write_out(path, n_raw, w_nocut, end_marker=True):
    """A fake analyzer/merged output: Tree/cutflow (unweighted), Tree/cutflow_w, Tree/cutflow_w_full."""
    f = ROOT.TFile(path, "RECREATE")
    d = f.mkdir("Tree")
    d.cd()
    raw = ROOT.TH1F("cutflow", "", len(LABELS), 0, len(LABELS))
    w = ROOT.TH1F("cutflow_w", "", len(LABELS), 0, len(LABELS))
    for i, lab in enumerate(LABELS, 1):
        for h in (raw, w):
            h.GetXaxis().SetBinLabel(i, lab)
    raw.SetBinContent(1, n_raw)
    raw.SetBinContent(2, n_raw * 0.5)
    raw.SetBinContent(3, n_raw * 0.1)
    w.SetBinContent(1, w_nocut)
    w.SetBinContent(2, w_nocut * 0.5)
    w.SetBinContent(3, w_nocut * 0.1)
    raw.Write()
    w.Write()
    if end_marker:
        w.Clone("cutflow_w_full").Write()
    f.Close()


def run(cmd, env=None):
    p = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, universal_newlines=True,
                       cwd=REPO, env=env)
    return p.returncode, p.stdout


def hadd_env():
    env = dict(os.environ, PYTHONDONTWRITEBYTECODE="1")
    if shutil.which("hadd") is None:
        cand = os.path.join(os.path.dirname(ROOT.__file__), "bin")
        if os.path.isfile(os.path.join(cand, "hadd")):
            env["PATH"] = cand + os.pathsep + env.get("PATH", "")
    return env


DATA = "JetMET0_Run2024C-MINIv6NANOv15-v1"

with tempfile.TemporaryDirectory(prefix="tthh_fail_") as T:
    # ---------------- common fixtures: yml, xsec db, prescan summary, filelists ----------------
    fl = os.path.join(T, "fl")
    os.makedirs(fl)
    for name, n in (("SampleA", 5), ("SampleB", 4), ("SampleC", 2), (DATA, 3)):
        with open(os.path.join(fl, f"filelist_{name}.txt"), "w") as fh:
            fh.write("# list\n" + "".join(f"/store/x/{name}_{i}.root\n" for i in range(n)))
    with open(os.path.join(T, "xsec.json"), "w") as fh:
        json.dump({"_meta": {"lumi_fb_inv": 10.0},
                   "SampleA": {"cross_section_fb": 100.0, "br": 1.0, "kfactor": 1.0},
                   "SampleB": {"cross_section_fb": 60.0, "br": 1.0, "kfactor": 1.0},
                   "SampleC": {"cross_section_fb": 10.0, "br": 1.0, "kfactor": 1.0},
                   DATA: {"cross_section_fb": None}}, fh)
    with open(os.path.join(T, "prescan.json"), "w") as fh:
        json.dump({"samples": {
            "SampleA": {"runs": {"genEventSumw": 1000.0}, "events": {"sumGenW_total": 500.0, "nEvents_total": 500}},
            "SampleB": {"runs": {"genEventSumw": 600.0}, "events": {"sumGenW_total": 300.0, "nEvents_total": 300}},
            "SampleC": {"runs": {"genEventSumw": 100.0}, "events": {"sumGenW_total": 50.0, "nEvents_total": 50}}}},
            fh)

    def write_cfg(path, samples):
        with open(path, "w") as fh:
            fh.write('common:\n  year: "2024"\n  analysis_mode: main\n  lumi_fb_inv: 10.0\n'
                     f'  xsec_db: "{T}/xsec.json"\n  prescan: "{T}/prescan.json"\n'
                     "  files_per_job: 2\n  files_per_job_data: 1\nsamples:\n"
                     + "".join(f"  - {s}\n" for s in samples))

    cfg = os.path.join(T, "cfg.yml")
    write_cfg(cfg, ["SampleA", "SampleB", DATA])
    cfg_c = os.path.join(T, "cfg_c.yml")
    write_cfg(cfg_c, ["SampleA", "SampleB", "SampleC", DATA])

    # ---------------- A: make_plots --check-only, exact MC event count ----------------
    base = os.path.join(T, "merged")
    os.makedirs(base)
    # weight = lumi x xsec / sumw(Runs): A 10*100/1000 = 1.0, B 10*60/600 = 1.0 -> expected noCut = sumGenW(Events)
    write_out(os.path.join(base, "SampleA.root"), 500, 500.0)
    write_out(os.path.join(base, "SampleB.root"), 290, 300.0)        # 10 events short of the prescan
    write_out(os.path.join(base, DATA + ".root"), 1000, 1000.0)
    mp = [sys.executable, os.path.join(REPO, "plotter", "make_plots.py"), "--config", cfg, "--base", base,
          "--out", os.path.join(T, "plots"), "--check-only"]
    rc, out = run(mp)
    check("A short sample: RESULT FAIL, exit 1", rc == 1 and "RESULT FAIL (1 flagged" in out, out[-2500:])
    check("A FLAG names SampleB 290 vs 300", "FLAG SampleB: merged noCut 290 events, prescan 300" in out, out[-2500:])
    check("A EVENTS line: 2 of 2, 1 differ", "EVENTS MC 2 of 2 samples" in out and "1 differ (FLAG)" in out)
    write_out(os.path.join(base, "SampleB.root"), 300, 300.0)
    rc, out = run(mp)
    check("A complete: RESULT OK, all equal", rc == 0 and "RESULT OK (check only)" in out and "all equal" in out,
          out[-2500:])
    mpm = load("tthh_make_plots", "plotter/make_plots.py")
    check("A float tolerance: 2.4e8 +- 500 equal, -1 file differs",
          not mpm.count_differs(243825725.0 + 500, 243825725) and mpm.count_differs(243825725.0 - 316000, 243825725))

    # ---------------- B: merge_outputs.py ----------------
    mb = os.path.join(T, "mbase")
    for name, idxs in (("SampleA", [0, 1, 2]), ("SampleB", [0]), (DATA, [0, 1, 2, 3]), ("Other", [0])):
        os.makedirs(os.path.join(mb, name))
        for i in idxs:
            write_out(os.path.join(mb, name, f"{name}_{i}.root"), 10 * (i + 1), 10.0 * (i + 1))
    env = hadd_env()
    env["TTHH_MERGE_WORKROOT"] = os.path.join(T, "workroot")
    mo = [sys.executable, os.path.join(REPO, "outputMerger", "merge_outputs.py"), "--base", mb,
          "--config", cfg_c, "--filelist-dir", fl]
    rc, out = run(mo + ["--list"], env)
    check("B --list: SampleA 3 = 3 expected", "MERGE -> SampleA.root (= 3 jobs)" in out, out)
    check("B --list: SampleB missing job 1", "INCOMPLETE: job 1/2 개 없음 [1]" in out, out)
    check("B --list: data extra file idx 3", f"{DATA}_3.root" in out and "남는 파일 1 개" in out, out)
    check("B --list: Other not in the yml", "--config 의 yml 에 없는 프로세스" in out, out)
    check("B --list: SampleC absent", "ABSENT" in out and "SampleC" in out, out)
    if shutil.which("hadd", path=env.get("PATH")):
        rc, out = run(mo + ["--mode", "local", "--only", "SampleA"], env)
        merged = os.path.join(mb, "SampleA.root")
        ok = rc == 0 and os.path.isfile(merged)
        n = -1
        if ok:
            f = ROOT.TFile.Open(merged)
            n = f.Get("Tree/cutflow").GetBinContent(1)
            f.Close()
        check("B local merge of SampleA: exit 0, noCut 10+20+30 = 60", ok and n == 60, out[-2000:])
        rc2, out2 = run(["/bin/bash", os.path.join(REPO, "outputMerger", "run_one_hadd.sh"),
                         os.path.join(mb, "SampleA"), os.path.join(T, "x.root"), "-", "4"], env)
        check("B worker: 3 inputs, 4 expected -> exit 7", rc2 == 7 and "3 input files, 4 expected" in out2, out2[-800:])
    else:
        print("NOTE hadd not on PATH: the local merge checks of B are skipped")
    # a condor work directory written after the local one: SampleB merged ok, the data merge failed with rc 7
    wd = os.path.join(T, "workroot", "mbase_20991231-235959")
    os.makedirs(os.path.join(wd, "logs"))
    open(os.path.join(wd, "merge.sub"), "w").close()
    with open(os.path.join(wd, "arguments.txt"), "w") as fh:
        fh.write(f"{mb}/SampleB {mb}/SampleB.root /cms/src 2\n{mb}/{DATA} {mb}/{DATA}.root /cms/src 3\n")
    with open(os.path.join(wd, "logs", "log.777.log"), "w") as fh:
        fh.write("000 (777.000.000) 2099-12-31 23:59:59 Job submitted from host: <1.2.3.4>\n...\n"
                 "000 (777.001.000) 2099-12-31 23:59:59 Job submitted from host: <1.2.3.4>\n...\n"
                 "001 (777.000.000) 2099-12-31 23:59:59 Job executing on host: <1.2.3.5>\n...\n"
                 "005 (777.000.000) 2099-12-31 23:59:59 Job terminated.\n"
                 "\t(1) Normal termination (return value 0)\n\t\tUsr 0 00:00:01, Sys 0 00:00:00\n...\n"
                 "001 (777.001.000) 2099-12-31 23:59:59 Job executing on host: <1.2.3.5>\n...\n"
                 "005 (777.001.000) 2099-12-31 23:59:59 Job terminated.\n"
                 "\t(1) Normal termination (return value 7)\n...\n")
    with open(os.path.join(wd, "logs", "job.777.0.out"), "w") as fh:
        fh.write("[hadd] OK\n")
    write_out(os.path.join(mb, "SampleB.root"), 10, 10.0)
    rc, out = run(mo + ["--report"], env)
    check("B --report: exit 1 (not all ok)", rc == 1, out)
    check("B --report: data merge FAILED rc=7", f"{DATA}" in out and "FAILED" in out and "777.1 rc=7" in out, out)
    check("B --report: SampleC NOT-MERGED", "NOT-MERGED" in out and "SampleC" in out, out)
    check("B --report: SampleB not listed as a problem", "SampleB " not in out.split("TOTAL")[0], out)
    rc, out = run(mo + ["--resubmit", "--mode", "local", "--dry-run"], env)
    check("B --resubmit: the failed data merge and SampleC", "[resubmit] 2 process(es)" in out, out)

    # ---------------- C: submitter, why a job is missing ----------------
    sub = load("tthh_submitter", "submit_job_FH_Tier3_unified.py")
    cd = os.path.join(T, "condor")
    tmp1 = os.path.join(cd, "tmp_SampleA_20261006-000000")
    tmp2 = os.path.join(cd, "tmp_SampleA_20261006-010000")
    os.makedirs(tmp1)
    os.makedirs(tmp2)
    outp = os.path.join(T, "out", "SampleA") + "/"
    for p, idx in ((1, 1), (2, 2), (3, 3), (6, 6)):
        with open(os.path.join(tmp1, f"job_SampleA.100.{p}.out"), "w") as fh:
            fh.write("\n--------- Check arguments ----\n\n"
                     f"  - [ output file name ] --> {outp}SampleA_{idx}.root\n")
    ev = ["000 (100.{p:03d}.000) 10/06 00:00:00 Job submitted from host: <h>\n...\n".format(p=p) for p in (1, 2, 3, 6)]
    ev += ["001 (100.001.000) 10/06 00:01:00 Job executing on host: <h>\n...\n",
           "005 (100.001.000) 10/06 00:02:00 Job terminated.\n\t(1) Normal termination (return value 30)\n...\n",
           "001 (100.002.000) 10/06 00:01:00 Job executing on host: <h>\n...\n",
           "001 (100.003.000) 10/06 00:01:00 Job executing on host: <h>\n...\n",
           "012 (100.003.000) 10/06 00:03:00 Job was held.\n\tError from slot1: memory limit\n\tCode 34 Subcode 0\n...\n",
           "001 (100.006.000) 10/06 00:01:00 Job executing on host: <h>\n...\n",
           "005 (100.006.000) 10/06 00:02:00 Job terminated.\n\t(1) Normal termination (return value 0)\n...\n"]
    with open(os.path.join(cd, "log_SampleA.100.log"), "w") as fh:
        fh.write("".join(ev))
    with open(os.path.join(cd, "log_SampleA.200.log"), "w") as fh:          # the last resubmission: idx 4, not started
        fh.write("000 (200.000.000) 10/06 01:00:00 Job submitted from host: <h>\n...\n")
    with open(os.path.join(cd, "arguments_SampleA.txt"), "w") as fh:
        fh.write(f"--filelist {tmp2}/filelist_SampleA_4.txt --output {outp}SampleA_4.root --weight 1 --year 2024 \n")
    obj = object.__new__(sub.CondorJobManager)
    obj.output_dir = obj.sample_output_dir = "SampleA"
    obj.path_output = outp
    obj.condor_files_path = cd
    obj.arg_list_file = os.path.join(cd, "arguments_SampleA.txt")
    obj.analyzer_path = REPO
    d = obj._diagnose_missing([1, 2, 3, 4, 5, 6])
    check("C idx 1: failed, return value 30 INPUT_OPEN_FAIL, .err path",
          d[1][0] == "failed" and "return value 30 INPUT_OPEN_FAIL" in d[1][1]
          and "error_SampleA.100.1.err" in d[1][1], str(d[1]))
    check("C idx 2: pending (running)", d[2][0] == "pending" and "running" in d[2][1], str(d[2]))
    check("C idx 3: held with the reason", d[3][0] == "held" and "memory limit" in d[3][1], str(d[3]))
    check("C idx 4: pending from the arguments file (newest cluster 200, proc 0)",
          d[4][0] == "pending" and d[4][1].startswith("200.0"), str(d[4]))
    check("C idx 5: notrun", d[5][0] == "notrun", str(d[5]))
    check("C idx 6: rc 0 without the end marker -> failed", d[6][0] == "failed" and "no end marker" in d[6][1],
          str(d[6]))
    obj._report_rows = [("SampleA", 7, 1, 6, 2, 3), ("SampleB", 2, 2, 0, 0, 0)]
    obj.AnalyzerMode, obj._dir_suffix = "main", "_notrig"
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        obj._print_report_table()
    t = buf.getvalue()
    check("C table: fail / wait columns and totals",
          "fail" in t and "wait" in t and "<-- failed" in t and "wait 3" in t, t)

    # ---------------- D: make_plots --tree-cut, control plots from Tree/Tree [D-2026-10-06-A] ----------------
    def write_tree_out(path, n_raw, w_nocut, events):
        """write_out plus Tree/Tree (the analyzer's columns that --tree-cut reads): events = [(HLT_PFHT1050, HT, w)]"""
        write_out(path, n_raw, w_nocut)
        f = ROOT.TFile(path, "UPDATE")
        f.cd("Tree")
        t = ROOT.TTree("Tree", "tree for dnn inputs")
        c_trig = array("b", [0])
        cf = {c: array("f", [0.0]) for c in ("HT", "evtWeight", "MET_pt", "aplanarity", "sphericity", "eventC",
                                               "eventD", "bjetAplanarity", "bjetSphericity")}
        ci = {c: array("i", [0]) for c in ("nJets", "nbJets")}
        vec = {c: ROOT.std.vector("float")() for c in ("jetPt", "jetEta", "bTagScore")}
        flav = ROOT.std.vector("int")()     # [STEP 24 I] hadron flavour of the selected jets (the analyzer's hadFlavs)
        t.Branch("passTrigger_HLT_PFHT1050", c_trig, "passTrigger_HLT_PFHT1050/O")
        for c, a_ in cf.items():
            t.Branch(c, a_, c + "/F")
        for c, a_ in ci.items():
            t.Branch(c, a_, c + "/I")
        for c, v in vec.items():
            t.Branch(c, v)
        t.Branch("hadFlavs", flav)
        for trig, ht, w in events:
            c_trig[0] = 1 if trig else 0
            for c, a_ in cf.items():
                a_[0] = {"HT": ht, "evtWeight": w, "MET_pt": 50.0}.get(c, 0.2)
            ci["nJets"][0], ci["nbJets"][0] = 6, 2
            for v in vec.values():
                v.clear()
            flav.clear()
            for i in range(6):
                vec["jetPt"].push_back(ht / 6.0 * (1.5 - 0.2 * i))
                vec["jetEta"].push_back(0.1 * i)
                vec["bTagScore"].push_back(0.1 * (i + 1))
                flav.push_back(5 if i < 2 else 4 if i == 2 else 0)      # scores 0.1, 0.2: b; 0.3: c; 0.4-0.6: light
            t.Fill()
        t.Write()
        f.Close()

    tb = os.path.join(T, "tbase")
    os.makedirs(tb)
    write_tree_out(os.path.join(tb, "SampleA.root"), 500, 500.0,
                   [(True, 1300.0, 2.0), (True, 1100.0, 2.0), (False, 1500.0, 2.0), (True, 2000.0, 2.0)])
    write_tree_out(os.path.join(tb, "SampleB.root"), 300, 300.0,
                   [(True, 1250.0, 0.5), (True, 1201.0, 0.5), (True, 1200.0, 0.5)])
    write_tree_out(os.path.join(tb, DATA + ".root"), 1000, 1000.0,          # Data: weight 1, not the 7 stored
                   [(True, 1500.0, 1.0), (True, 900.0, 1.0), (False, 1300.0, 1.0), (True, 1210.0, 7.0)])
    tout = os.path.join(T, "tplots")
    mt = [sys.executable, os.path.join(REPO, "plotter", "make_plots.py"), "--config", cfg, "--base", tb, "--out", tout,
          "--grouping", "compact", "--tree-cut", "passTrigger_HLT_PFHT1050 && HT > 1200",
          "--tree-label", "HLT_PFHT1050, H_{T} > 1200 GeV"]
    rc, out = run(mt, hadd_env())
    check("D TREEYIELD per sample (MC weighted by evtWeight, Data by 1)",
          "TREEYIELD MC   SampleA" in out and "events=2 sumw=4.00" in out.split("TREEYIELD MC   SampleA")[1].split("\n")[0]
          and "events=2 sumw=1.00" in out.split("TREEYIELD MC   SampleB")[1].split("\n")[0]
          and "events=2 sumw=2.00" in out.split("TREEYIELD DATA " + DATA)[1].split("\n")[0], out[-3000:])
    check("D TREEYIELD total MC 5.0, Data 2, Data/MC 0.400",
          "TREEYIELD total MC=5.0 Data=2 Data/MC=0.400" in out, out[-3000:])
    hf = ROOT.TFile.Open(os.path.join(tout, "tree", "SampleA.root"))
    hht = hf.Get("Control/HT") if hf else None
    check("D tree/SampleA.root: Control/HT, TH1F, 2 entries, integral 4, x title",
          bool(hht) and hht.InheritsFrom("TH1F") and int(hht.GetEntries()) == 2 and abs(hht.Integral() - 4.0) < 1e-6
          and hht.GetXaxis().GetTitle() == "H_{T} [GeV]",
          str((bool(hht), hht.GetEntries() if hht else None, hht.Integral() if hht else None)))
    # [STEP 24 I] the jet-multiplicity diagnostics: SampleA has 2 events (weight 2) with 6 jets, all pT > 100 GeV,
    #   |eta| <= 0.5, nbJets 2 -> every nJets variant at 6 (forward: 0), nb >= 3 empty, 12 jet scores of weight 2
    def integ_at(name, x):
        h = hf.Get("Control/" + name) if hf else None
        return (round(h.GetBinContent(h.FindBin(x)), 6), round(h.Integral(), 6)) if h else None
    diag = {n: integ_at(n, x) for n, x in (("nJets_pt40", 6), ("nJets_pt50", 6), ("nJets_central", 6), ("nJets_forward", 0),
                                          ("nJets_nb2", 6), ("nJets_nb3p", 6))}
    hb = hf.Get("Control/btag_alljets") if hf else None
    check("D the nJets diagnostics: pT > 40 / 50, central, forward, nb = 2 at 6 jets (weight 4), nb >= 3 empty",
          all(diag[n] == (4.0, 4.0) for n in ("nJets_pt40", "nJets_pt50", "nJets_central", "nJets_forward", "nJets_nb2"))
          and diag["nJets_nb3p"] == (0.0, 0.0), str(diag))
    check("D btag_alljets: one entry per selected jet (12, integral 24)",
          bool(hb) and int(hb.GetEntries()) == 12 and abs(hb.Integral() - 24.0) < 1e-6,
          str((hb.GetEntries(), hb.Integral()) if hb else None))
    if hf:
        hf.Close()
    rows = {l.split()[1]: l.split()[2:] for l in out.splitlines() if l.startswith("TREEFLAV ") and "-" in l.split()[1]}
    check("D TREEFLAV: per score bin MC 5 (b / c / light from hadFlavs), Data 2, Data/MC 0.400",
          rows.get("0.1-0.2") == ["5.00", "100.0%", "0.0%", "0.0%", "2", "0.400"]
          and rows.get("0.3-0.4") == ["5.00", "0.0%", "100.0%", "0.0%", "2", "0.400"]
          and rows.get("0.5-0.6") == ["5.00", "0.0%", "0.0%", "100.0%", "2", "0.400"]
          and rows.get("0.0-0.1") == ["0.00", "-", "-", "-", "0", "nan"] and len(rows) == 10, str(rows))
    with open(os.path.join(tout, "YIELDS.txt")) as fh:
        ytxt = fh.read()
    check("D YIELDS.txt keeps the merged cutflow and adds TREECUT / TREEYIELD / TREEFLAV",
          "YIELD noCut" in ytxt and "TREECUT passTrigger_HLT_PFHT1050 && HT > 1200" in ytxt and "TREEYIELD total" in ytxt
          and "TREEFLAV 0.1-0.2" in ytxt)
    with open(os.path.join(tout, "structure_info.yml")) as fh:
        sinfo = fh.read()
    check("D structure_info: only the Control/ histograms (%d), from the tree files" % len(mpm.TREE_HISTS),
          sinfo.count("- key_path: Control/") == len(mpm.TREE_HISTS) and "Tree/cutflow" not in sinfo
          and os.path.join(tout, "tree") in sinfo)
    if shutil.which("root") is not None:
        check("D the plotter ran on them (PLOTS compact rc=0, RESULT OK)",
              rc == 0 and "PLOTS compact rc=0" in out and "RESULT OK" in out, out[-3000:])
    if shutil.which("root") is not None:      # a relative --out (resolved before make_plots' chdir; review 10-06)
        rel = os.path.relpath(os.path.join(T, "tplots_rel"), REPO)
        mtr = [x if x != tout else rel for x in mt]
        rcr, outr = run(mtr[:mtr.index("--tree-cut")] + ["--tree-cut", "passTrigger_HLT_PFHT1050 && btag_1 > 0.5"],
                        hadd_env())
        check("D a relative --out and a cut on a derived column (btag_1): plots made, RESULT OK",
              rcr == 0 and "PLOTS compact rc=0" in outr and "RESULT OK" in outr
              and "TREEYIELD total MC=7.5 Data=3 Data/MC=0.400" in outr, outr[-3000:])
    rc2, out2 = run(mt[:mt.index("--tree-cut")] + ["--tree-cut", "noSuchBranch > 1"], hadd_env())
    check("D a cut on a branch the tree does not have: TREE FAIL, exit 1",
          rc2 == 1 and "TREE FAIL SampleA" in out2 and "RESULT FAIL (--tree-cut)" in out2, out2[-2000:])
    rc3, out3 = run(mt[:mt.index("--tree-cut")] + ["--tree-label", "x"], hadd_env())
    check("D --tree-label without --tree-cut: exit 2", rc3 == 2 and "needs --tree-cut" in out3, out3[-500:])

    # ---------------- E: [STEP 24 J] the weight inputs of every sample at once, --only ----------------
    #   2026-10-07: the 2024 main run queued 76 of its 84 samples, then stopped (E20) at the first ParkingHH sample,
    #   which had no xsec_db entry -- the preflight checked the MC samples only
    PK = "ParkingHH_Run2024C-MINIv6NANOv15-v1"
    ents = [{"sample_name": n} for n in ("SampleA", "SampleB", DATA, PK)]
    sel, un = sub.select_samples(ents, "ParkingHH_*")
    check("E --only 'ParkingHH_*': the ParkingHH entry, no unmatched pattern",
          [sub.entry_name(e) for e in sel] == [PK] and un == [], str((sel, un)))
    sel, un = sub.select_samples(ents, " SampleA, Sample? ,Nope* ")
    check("E --only ' SampleA, Sample? ,Nope* ': SampleA and SampleB in yml order, 'Nope*' unmatched",
          [sub.entry_name(e) for e in sel] == ["SampleA", "SampleB"] and un == ["Nope*"], str((sel, un)))
    sel, un = sub.select_samples(ents, "")
    check("E no --only: every entry", len(sel) == 4 and un == [], str((sel, un)))
    with open(os.path.join(T, "xsec.json")) as fh:
        dbj = json.load(fh)
    with open(os.path.join(T, "prescan.json")) as fh:
        prj = json.load(fh)["samples"]
    probs = sub.weight_input_problems(ents + [{"sample_name": "SampleZ"}, {"sample_name": "SampleW", "weight": 2.0}],
                                      dbj, prj)
    check("E weight inputs: the Data sample without its xsec_db entry, the MC sample without both; a yml weight "
          "needs neither",
          probs == [("db_data", PK, "not in xsec_db"), ("db", "SampleZ", ""), ("prescan", "SampleZ", "")], str(probs))
    dbj2 = dict(dbj, **{PK: {"cross_section_fb": 5.0}, "SampleB": {"cross_section_fb": None}})
    probs2 = sub.weight_input_problems(ents, dbj2, prj)
    check("E weight inputs: an MC name with a null cross section, a Data name with one",
          [p[:2] for p in probs2] == [("db", "SampleB"), ("db_data", PK)], str(probs2))
    check("E weight inputs: none needed in the prescan mode", sub.weight_input_problems(ents, {}, {}, True) == [])
    common_t = {"xsec_db": os.path.join(T, "xsec.json"), "prescan": os.path.join(T, "prescan.json")}
    err, code = io.StringIO(), None
    with contextlib.redirect_stderr(err):
        try:
            object.__new__(sub.CondorJobManager)._check_weight_inputs(ents, common_t, "main")
        except SystemExit as e:
            code = e.code
    check("E the submission stops before its first sample: E20, the sample named, nothing submitted",
          code == 20 and f"[E20] {PK}" in err.getvalue() and "nothing submitted" in err.getvalue(),
          f"code={code}\n{err.getvalue()}")
    check("E ... and goes on when every sample has its inputs, or in the prescan mode",
          object.__new__(sub.CondorJobManager)._check_weight_inputs(ents[:3], common_t, "main") is None
          and object.__new__(sub.CondorJobManager)._check_weight_inputs(ents, common_t, "prescan") is None)
    # the preflight, on a copy of the script (its log is written next to the script); the fixture yml is 2024
    sd = os.path.join(T, "subcopy")
    os.makedirs(sd)
    shutil.copy(os.path.join(REPO, "submit_job_FH_Tier3_unified.py"), sd)
    with open(os.path.join(fl, f"filelist_{PK}.txt"), "w") as fh:
        fh.write("/store/x/pk_0.root\n/store/x/pk_1.root\n")
    cfg_pk = os.path.join(T, "cfg_pk.yml")
    write_cfg(cfg_pk, ["SampleA", "SampleB", DATA, PK])
    envp = dict(os.environ, PYTHONDONTWRITEBYTECODE="1",
                PYTHONPATH=os.pathsep.join(p for p in (os.path.join(REPO, "python", "ttHHmodules"),
                                                       os.environ.get("PYTHONPATH", "")) if p))
    pf = [sys.executable, os.path.join(sd, "submit_job_FH_Tier3_unified.py"), "--preflight", "--config", cfg_pk,
          "--filelist-dir", fl, "--trigsf", "off"]
    rc, out = run(pf, envp)
    check("E preflight: the Data sample without its xsec_db entry is a FAIL, the MC samples pass",
          "[FAIL] xsec_db coverage (Data)" in out and f"{PK}(not in xsec_db)" in out
          and "[PASS] xsec_db coverage " in out and "all 2 MC samples present" in out, out[-3000:])
    rc, out = run(pf + ["--only", "ParkingHH_*"], envp)
    check("E preflight --only 'ParkingHH_*': 1 of 4 samples, their checks and job count; the era check (whole yml) "
          "finds JetMET and ParkingHH in era C",
          "[PASS] --only" in out and f"1 of 4 samples: {PK}" in out and "Per-sample checks (1 samples" in out
          and "~2 jobs (files=2," in out and "by era" not in out and "[PASS] 2024 Data PDs" in out, out[-3000:])
    rc, out = run(pf + ["--only", "Nope*"], envp)
    check("E preflight --only matching no sample: FAIL, exit 1", rc == 1 and "[FAIL] --only" in out
          and "pattern(s) matching no yml sample: ['Nope*'] (0 of 4 selected" in out, out[-2000:])
    with open(os.path.join(T, "xsec_pk.json"), "w") as fh:
        json.dump(dict(dbj, **{PK: {"cross_section_fb": None}}), fh)
    cfg_pk2 = os.path.join(T, "cfg_pk2.yml")
    with open(cfg_pk) as fh, open(cfg_pk2, "w") as fo:
        fo.write(fh.read().replace(os.path.join(T, "xsec.json"), os.path.join(T, "xsec_pk.json")))
    rc, out = run([cfg_pk2 if x == cfg_pk else x for x in pf], envp)
    check("E preflight with the entry added: all 2 Data samples present with cross_section_fb null",
          "[PASS] xsec_db coverage (Data)" in out and "all 2 Data samples present with cross_section_fb null" in out
          and "[FAIL] xsec_db coverage" not in out, out[-3000:])
    # the submission's loop gets the selected entries only, after the check of their weight inputs
    #   (process_config_file with the per-sample steps replaced: no condor, no directories)
    def loop_entries(cfg_path, only):
        o = object.__new__(sub.CondorJobManager)
        o.config_file_path, o._cli_only = cfg_path, only
        o.report_only = o.resubmit_only = o.report_verbose = False
        o.condor_files_path = os.path.join(T, "condor_loop")
        os.makedirs(o.condor_files_path, exist_ok=True)
        seen = []
        o.parse_config_entry = lambda e, c: seen.append(sub.entry_name(e))
        o.prepare_output_directory = o.setup_and_submit_job = lambda: None
        e_buf, code_ = io.StringIO(), None
        with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(e_buf):
            try:
                o.process_config_file()
            except SystemExit as e:
                code_ = e.code
        return seen, code_, e_buf.getvalue()
    seen, code_, _ = loop_entries(cfg_pk2, "ParkingHH_*")
    seen_all, code_all, err_all = loop_entries(cfg_pk, "")
    check("E submission: --only gives the loop the ParkingHH entry only; without the xsec_db entry nothing reaches the "
          "loop (E20)", seen == [PK] and code_ is None and seen_all == [] and code_all == 20 and PK in err_all,
          str((seen, code_, seen_all, code_all)))

    # ---------------- F: [STEP 25 K] the 2024 fixed-WP b-tag weight in the submitter ----------------
    bem = load("tthh_btag_eff_maps", os.path.join("tools", "stage7", "btag_eff_maps.py"))
    PTE, ETE = [20.0, 50.0, 1000.0], [0.0, 1.2, 2.5]

    def eff_json(path, year, breakage=None):
        """a JSON as tools/stage7/btag_eff_maps.py writes it (groups tt, all); breakage: 'nodefault' (btag_eff_groups
        without its default), 'noL' (the tt b node without WP L)"""
        maps = {g: {f: {"eff": {wp: [[0.5, 0.4], [0.3, 0.2]] for wp in bem.WPS}} for f, _ in bem.FLAVOURS}
                for g in ("tt", "all")}
        js = bem.correction_json(maps, ["tt", "all"], PTE, ETE,
                                 f"year={year}; wp=L:0.0246,M:0.1272,T:0.4648; flavours_required=b; test")
        if breakage == "nodefault":
            del js["corrections"][1]["data"]["default"]
        if breakage == "noL":
            tt = js["corrections"][0]["data"]["content"][0]["value"]
            b = [it for it in tt["content"] if it["key"] == 5][0]["value"]
            b["content"] = [it for it in b["content"] if it["key"] != "L"]
        with open(path, "w") as fh:
            json.dump(js, fh)
    eff24, eff17 = os.path.join(T, "eff24.json"), os.path.join(T, "eff17.json")
    eff24nd, eff24nl = os.path.join(T, "eff24_nodefault.json"), os.path.join(T, "eff24_noL.json")
    eff_json(eff24, "2024")
    eff_json(eff17, "2017")
    eff_json(eff24nd, "2024", "nodefault")
    eff_json(eff24nl, "2024", "noL")
    p_nd, _ = sub.btag_eff_json_summary(eff24nd, "2024")
    p_nl, _ = sub.btag_eff_json_summary(eff24nl, "2024")
    check("F the preflight reading of the JSON finds a missing groups default and a missing WP (they would stop jobs)",
          any("btag_eff_groups has no default" in x for x in p_nd) and any("[tt][5]: no key ['L']" in x for x in p_nl)
          and sub.btag_eff_json_summary(eff24, "2024")[0] == [], str((p_nd, p_nl)))
    pu = os.path.join(T, "pu.json")
    with open(pu, "w") as fh:
        fh.write("{}")

    def cfg24(path, eff_line, samples=("SampleA", "SampleB")):
        with open(path, "w") as fh:
            fh.write('common:\n  year: "2024"\n  analysis_mode: main\n  lumi_fb_inv: 10.0\n'
                     f'  xsec_db: "{T}/xsec.json"\n  prescan: "{T}/prescan.json"\n'
                     "  files_per_job: 2\n  files_per_job_data: 1\n"
                     f'  path_jsonpog: "{T}"\n  path_goldenjson: "{T}"\n  path_pu_json: "{pu}"\n'
                     "  path_trigsf_dir: null\n  path_btag_reweight_json: null\n  path_stitch_json: null\n"
                     "  path_expanded_ttbarid_dir: null\n" + eff_line + "samples:\n"
                     + "".join(f"  - {x}\n" for x in samples))
    cf_null, cf_eff, cf_e17, cf_none = (os.path.join(T, f"cfg_f{k}.yml") for k in range(4))
    cfg24(cf_null, "  path_btag_eff_json: null\n")
    cfg24(cf_eff, f'  path_btag_eff_json: "{eff24}"\n')
    cfg24(cf_e17, f'  path_btag_eff_json: "{eff17}"\n')
    cfg24(cf_none, "")
    pff = [sys.executable, os.path.join(sd, "submit_job_FH_Tier3_unified.py"), "--preflight", "--filelist-dir", fl,
           "--trigsf", "off"]
    rc, out = run(pff + ["--config", cf_null, "--btagsf", "on"], envp)
    check("F preflight --btagsf on with path_btag_eff_json null: FAIL (required for the MC samples, E13)",
          "[FAIL] path_btag_eff_json" in out and "null but REQUIRED" in out and "btagsf=on" in out, out[-3000:])
    rc, out = run(pff + ["--config", cf_eff, "--btagsf", "on"], envp)
    check("F preflight --btagsf on with a usable efficiency JSON: PASS lines (the file, its groups, the method)",
          "[PASS] path_btag_eff_json" in out and "[PASS] 2024 b-tag efficiency JSON" in out and "(groups tt all)" in out
          and "[PASS] 2024 --btagsf" in out and "the fixed-WP b-tag weight" in out
          and "[FAIL] path_btag_eff_json" not in out, out[-3000:])
    rc, out = run(pff + ["--config", cf_e17, "--btagsf", "on"], envp)
    check("F preflight: an efficiency JSON made for 2017 is a FAIL (every MC job E52)",
          "[FAIL] 2024 b-tag efficiency JSON" in out and "made for year 2017, the yml is 2024" in out
          and "every MC job E52" in out, out[-3000:])
    rc, out = run(pff + ["--config", cf_none, "--btagsf", "off"], envp)
    check("F preflight 2024 without the key: FAIL (KEY MISSING, E12 in every job)",
          "[FAIL] path_btag_eff_json" in out and "KEY MISSING" in out, out[-3000:])
    rc, out = run(pff + ["--config", cf_null, "--btagsf", "off"], envp)
    check("F preflight --btagsf off with null: PASS (disabled), --btagsf off",
          "[PASS] path_btag_eff_json" in out and "null -> disabled" in out and "[PASS] 2024 --btagsf" in out
          and "[FAIL] path_btag_eff_json" not in out, out[-3000:])
    # the submission: what reaches the job's environment (parse_config_entry, no condor)
    def job_env(common, name, btagsf):
        o = object.__new__(sub.CondorJobManager)
        o.AnalyzerMode, o._cli_trigsf, o._cli_btagsf, o._cli_btagrw = "main", "off", btagsf, "off"
        o.report_only = o.report_verbose = False
        o.cli_files_per_job = None
        o.path_output_base, o.condor_files_path, o.time_info = os.path.join(T, "outb"), os.path.join(T, "cnd"), "t0"
        e_buf, code_ = io.StringIO(), None
        with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(e_buf):
            try:
                o.parse_config_entry({"sample_name": name}, common)
            except SystemExit as e:
                code_ = e.code
        return getattr(o, "env_exports", {}), code_, e_buf.getvalue()
    c24 = sub.CondorJobManager.load_yaml_config(None, cf_null)["common"]
    env_a, code_a, _ = job_env(c24, "SampleA", "off")
    env_b, code_b, err_b = job_env(c24, "SampleA", "on")
    env_d, code_d, _ = job_env(c24, DATA, "on")
    c24e = sub.CondorJobManager.load_yaml_config(None, cf_eff)["common"]
    env_e, code_e, _ = job_env(c24e, "SampleA", "on")
    c17 = dict(c24, year="2017")
    del c17["path_btag_eff_json"], c17["path_pu_json"]
    env_17, code_17, _ = job_env(c17, "SampleA", "off")
    c24n = sub.CondorJobManager.load_yaml_config(None, cf_none)["common"]
    env_n, code_n, err_n = job_env(c24n, "SampleA", "off")
    check("F submission: null -> TTHH_BTAGEFF_JSON=__NULL__ (--btagsf off); --btagsf on: MC E13, Data still __NULL__; "
          "a real path is exported; 2017 without the key exports nothing; 2024 without the key E12",
          code_a is None and env_a.get("TTHH_BTAGEFF_JSON") == "__NULL__"
          and code_b == 13 and "path_btag_eff_json is REQUIRED" in err_b and "btagsf=on" in err_b
          and code_d is None and env_d.get("TTHH_BTAGEFF_JSON") == "__NULL__"
          and code_e is None and env_e.get("TTHH_BTAGEFF_JSON") == eff24
          and code_17 is None and "TTHH_BTAGEFF_JSON" not in env_17
          and code_n == 12 and "path_btag_eff_json" in err_n,
          str((code_a, env_a.get("TTHH_BTAGEFF_JSON"), code_b, code_d, code_e, code_17, code_n, err_b[-300:])))

    # ---------------- G: [STEP 25 K2] the condor stall guard of the submit files ----------------
    #   2026-10-08: the last 7 jobs of the 2024 lepton-CR runs stayed 'running' 12 h after their /pnfs reads hung
    #   (2-320 s of CPU in 42,000-47,000 s); condor saw live processes and did nothing
    def sub_file(guard):
        o = object.__new__(sub.CondorJobManager)
        if guard is not None:
            o._cli_stall_guard = guard
        gd = os.path.join(T, "guard")
        os.makedirs(gd, exist_ok=True)
        o.output_dir, o.os_version, o.memorySize = "SampleA", "el9", "12 GB"
        o.condor_submit_name = os.path.join(gd, f"SampleA_{guard}_condor.sub")
        o.proxy_path, o.script_name = os.path.join(gd, "proxy.cert"), os.path.join(gd, "run_SampleA.sh")
        o.tmp_folder, o.condor_files_path = os.path.join(gd, "tmp_SampleA_t0"), gd
        o.arg_list_file = os.path.join(gd, "arguments_SampleA.txt")
        o.write_condor_submission_file()
        with open(o.condor_submit_name) as fh:
            return fh.read()
    txt_on, txt_off, txt_def = sub_file("on"), sub_file("off"), sub_file(None)
    kv = {l.split("=", 1)[0].strip(): l.split("=", 1)[1].strip() for l in txt_on.splitlines()
          if "=" in l and not l.startswith("#") and not l.startswith("queue")}
    ex = sub.stall_guard_exprs()
    check("G submit file: periodic_hold / _reason / _subcode / periodic_release / requirements as stall_guard_exprs(), "
          "the comment line with the thresholds, the queue statement last",
          sorted(ex) == ["periodic_hold", "periodic_hold_reason", "periodic_hold_subcode", "periodic_release",
                         "requirements"]
          and all(kv.get(k) == v for k, v in ex.items())
          and "# stall guard [STEP 25 K2]: hold after 1 h with CPU < 5% of the run time, release after 5 min, at most "
              "3 starts" in txt_on
          and txt_on.rstrip().splitlines()[-1].startswith("queue args from "), txt_on)
    check("G --stall-guard off: none of those lines (the rest as before); an object without the attribute: on",
          not any(k in txt_off for k in ("periodic_", "requirements", "stall guard"))
          and [l for l in txt_on.splitlines() if not l.startswith(("periodic_", "requirements", "#"))]
          == txt_off.splitlines() and txt_def == txt_on, txt_off)
    rc, out = run(pf, envp)
    rc2, out2 = run(pf + ["--stall-guard", "off"], envp)
    check("G preflight: '[PASS] condor stall guard' with the thresholds; --stall-guard off: a WARN",
          "[PASS] condor stall guard" in out and "hold after 1 h with CPU < 5% of the run time" in out
          and "[WARN] condor stall guard" in out2 and "off (--stall-guard off)" in out2, out[-1500:] + out2[-1500:])
    ca = None
    for modname in ("classad2", "classad"):
        try:
            ca = importlib.import_module(modname)
            break
        except Exception:
            ca = None
    if ca is None:
        print("SKIP G ClassAd evaluation of the stall guard (no classad2 / classad module; pip install htcondor)")
    else:
        now = int(time.time())

        def ev(key, **attrs):
            a = ca.ClassAd(attrs)
            a["X"] = ca.ExprTree(ex[key])
            return a.eval("X")
        hrs = lambda h: now - int(h * 3600)
        got = {
            "hold 12.6 h / 149 s CPU": ev("periodic_hold", JobStatus=2, EnteredCurrentStatus=hrs(12.6),
                                         RemoteUserCpu=146.0, RemoteSysCpu=3.0) is True,
            "hold 1.5 h / CPU undefined": ev("periodic_hold", JobStatus=2, EnteredCurrentStatus=hrs(1.5)) is True,
            "no hold 3 h at 90 % CPU": ev("periodic_hold", JobStatus=2, EnteredCurrentStatus=hrs(3),
                                          RemoteUserCpu=9720.0, RemoteSysCpu=60.0) is False,
            "no hold 3 h at 10 % CPU": ev("periodic_hold", JobStatus=2, EnteredCurrentStatus=hrs(3),
                                          RemoteUserCpu=1080.0, RemoteSysCpu=0.0) is False,
            "no hold 0.8 h stuck": ev("periodic_hold", JobStatus=2, EnteredCurrentStatus=hrs(0.8),
                                      RemoteUserCpu=2.0, RemoteSysCpu=0.0) is False,
            "hold 1.1 h stuck (2181320.26: 2 s)": ev("periodic_hold", JobStatus=2, EnteredCurrentStatus=hrs(1.1),
                                                     RemoteUserCpu=2.0, RemoteSysCpu=0.0) is True,
            "TTbar_Hadronic_1 (320 s): not at 1.5 h, at 2 h": (
                ev("periodic_hold", JobStatus=2, EnteredCurrentStatus=hrs(1.5), RemoteUserCpu=320.0,
                   RemoteSysCpu=0.0) is False
                and ev("periodic_hold", JobStatus=2, EnteredCurrentStatus=hrs(2.0), RemoteUserCpu=320.0,
                       RemoteSysCpu=0.0) is True),
            "no hold idle 5 h": ev("periodic_hold", JobStatus=1, EnteredCurrentStatus=hrs(5)) is False,
            "reason": str(ev("periodic_hold_reason", JobStatus=2, EnteredCurrentStatus=hrs(12.6), RemoteUserCpu=146.0,
                             RemoteSysCpu=3.0, RemoteHost="slot1_1@cluster333.knu.ac.kr")).startswith(
                "tthh stall guard: running 75") and str(ev("periodic_hold_reason", JobStatus=2,
                                                          EnteredCurrentStatus=hrs(12.6), RemoteUserCpu=146.0,
                                                          RemoteSysCpu=3.0, RemoteHost="slot1_1@cluster333.knu.ac.kr")
                                                       ).endswith(" min with 149 s CPU on slot1_1@cluster333.knu.ac.kr"),
            "release 1st / 2nd": all(ev("periodic_release", HoldReasonCode=3, HoldReasonSubCode=4201, NumJobStarts=n,
                                        EnteredCurrentStatus=now - 400) is True for n in (1, 2)),
            "no release 3rd": ev("periodic_release", HoldReasonCode=3, HoldReasonSubCode=4201, NumJobStarts=3,
                                 EnteredCurrentStatus=now - 400) is False,
            "no release before 5 min": ev("periodic_release", HoldReasonCode=3, HoldReasonSubCode=4201,
                                          NumJobStarts=1, EnteredCurrentStatus=now - 100) is False,
            "no release of other holds": all(ev("periodic_release", HoldReasonCode=c, HoldReasonSubCode=s,
                                                NumJobStarts=1, EnteredCurrentStatus=now - 400) is False
                                             for c, s in ((1, 0), (3, 0), (34, 0))),
        }
        job = ca.ClassAd({"LastRemoteHost": "slot1_5@cluster333.knu.ac.kr"})
        job["Requirements"] = ca.ExprTree(ex["requirements"])
        fresh = ca.ClassAd()
        fresh["Requirements"] = ca.ExprTree(ex["requirements"])
        mach = lambda m: ca.ClassAd({"Machine": m})
        got["requirements: not the last machine, others and a first run yes"] = (
            mach("cluster333.knu.ac.kr").matches(job) is False and mach("cluster348.knu.ac.kr").matches(job) is True
            and mach("cluster33.knu.ac.kr").matches(job) is True and mach("cluster333.knu.ac.kr").matches(fresh) is True)
        check(f"G ClassAd evaluation ({ca.__name__}): the 10-08 jobs held, healthy / young / idle jobs not; our hold "
              "released twice at most and after 5 min, other holds not; the last machine refused",
              all(got.values()), str({k: v for k, v in got.items() if not v}))

n_fail = RESULTS.count(False)
print(f"SUMMARY test_failure_checks {'PASS' if n_fail == 0 else 'FAIL'} ({len(RESULTS) - n_fail}/{len(RESULTS)} checks)")
sys.exit(0 if n_fail == 0 else 1)
