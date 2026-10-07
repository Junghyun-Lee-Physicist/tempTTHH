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

Run from the repo top after cmsenv (needs PyROOT; part B's merge needs hadd):
    python3 test/test_failure_checks.py
Last line: SUMMARY test_failure_checks PASS|FAIL (<n>/<m> checks).
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

n_fail = RESULTS.count(False)
print(f"SUMMARY test_failure_checks {'PASS' if n_fail == 0 else 'FAIL'} ({len(RESULTS) - n_fail}/{len(RESULTS)} checks)")
sys.exit(0 if n_fail == 0 else 1)
