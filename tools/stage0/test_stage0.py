#!/usr/bin/env python3
"""test_stage0.py -- offline tests of runs_in_lumiblocks.py and branch_signature.py (STEP 23).

Builds small NanoAOD-like files (LuminosityBlocks + Events) in a temporary CRAB-like
layout <base>/<PD>/<request>/<stamp>/0000/forgedNtuple_<n>.root, with known runs,
luminosity blocks, events and branch sets, plus a failed/ copy and a broken file,
then checks every number the two tools print and their exit codes.
Needs PyROOT (cmsenv is enough).

    python3 tools/stage0/test_stage0.py        -> RESULT: <n> PASS, <m> FAIL
"""
from __future__ import print_function

import array
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
NP = [0, 0]


def check(name, cond):
    NP[0 if cond else 1] += 1
    print(("PASS " if cond else "FAIL ") + name)


def make_file(path, lumis, events, extra_branches=()):
    """lumis: list of (run, ls); events: list of runs (one entry per event)."""
    import ROOT
    d = os.path.dirname(path)
    if not os.path.isdir(d):
        os.makedirs(d)
    f = ROOT.TFile(path, "RECREATE")
    lb = ROOT.TTree("LuminosityBlocks", "")
    r = array.array("I", [0])
    l = array.array("I", [0])
    lb.Branch("run", r, "run/i")
    lb.Branch("luminosityBlock", l, "luminosityBlock/i")
    for run, ls in lumis:
        r[0] = run
        l[0] = ls
        lb.Fill()
    ev = ROOT.TTree("Events", "")
    er = array.array("I", [0])
    ev.Branch("run", er, "run/i")
    jet = array.array("f", [0.0])
    ev.Branch("Jet_pt", jet, "Jet_pt/F")
    flags = []
    for b in extra_branches:
        v = array.array("b", [0])
        ev.Branch(b, v, b + "/O")
        flags.append(v)
    for run in events:
        er[0] = run
        jet[0] = 30.0
        ev.Fill()
    f.Write()
    f.Close()


def make_truncated(path):
    """A file ROOT can only open by recovering keys (as after a failed stage-out)."""
    import ROOT
    d = os.path.dirname(path)
    if not os.path.isdir(d):
        os.makedirs(d)
    full = path + ".full"
    f = ROOT.TFile(full, "RECREATE")
    f.SetCompressionLevel(0)
    lb = ROOT.TTree("LuminosityBlocks", "")
    r = array.array("I", [0])
    l = array.array("I", [0])
    lb.Branch("run", r, "run/i")
    lb.Branch("luminosityBlock", l, "luminosityBlock/i")
    for i in range(10):
        r[0] = 380126
        l[0] = 100 + i
        lb.Fill()
    lb.Write()
    t = ROOT.TTree("Events", "")
    er = array.array("I", [0])
    x = array.array("d", [0.0])
    t.Branch("run", er, "run/i")
    t.Branch("x", x, "x/D")
    t.SetAutoSave(-2000000)
    for i in range(600000):
        er[0] = 380126
        x[0] = i * 1.5
        t.Fill()
    f.Write()
    f.Close()
    with open(full, "rb") as fh:
        data = fh.read()
    with open(path, "wb") as fh:
        fh.write(data[:int(len(data) * 0.8)])
    os.remove(full)


def run(args):
    p = subprocess.run([sys.executable] + args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                       universal_newlines=True)
    return p.returncode, p.stdout


def main():
    try:
        import ROOT  # noqa: F401
    except ImportError:
        print("SKIP: no PyROOT (run inside cmsenv)")
        return 2
    w = tempfile.mkdtemp(prefix="test_stage0.")
    try:
        base = os.path.join(w, "base")
        c = lambda pd, req, n: os.path.join(base, pd, req, "260101_000000", "0000", "forgedNtuple_%d.root" % n)
        # JetMET0 2024C: run 380126 LS 1-3 in file 1 (LS 3 also in file 2), run 380127 LS 1-2 in file 2
        make_file(c("JetMET0", "req_Run2024C", 1), [(380126, 1), (380126, 2), (380126, 3), (379999, 5)],
                  [380126] * 4 + [379999] * 2, ["HLT_A", "HLT_B"])
        make_file(c("JetMET0", "req_Run2024C", 2), [(380126, 3), (380127, 1), (380127, 2)],
                  [380126] * 1 + [380127] * 3, ["HLT_A", "HLT_B"])
        # JetMET1 2024C: run 380127 LS 2 again (same LS from another PD counts once), 380128 has LS but 0 events
        make_file(c("JetMET1", "req_Run2024C", 3), [(380127, 2), (380128, 7), (380128, 8)],
                  [380127] * 2, ["HLT_A"])
        # JetMET0 2024I: another request; branch sets differ inside it (HLT_C only in one file)
        make_file(c("JetMET0", "req_Run2024I", 4), [(386000, 1)], [386000] * 2, ["HLT_A"])
        make_file(c("JetMET0", "req_Run2024I", 5), [(386000, 2)], [386000] * 2, ["HLT_A", "HLT_C"])
        # a failed/ copy one level deeper (must be ignored by the default patterns)
        make_file(os.path.join(base, "JetMET0", "req_Run2024C", "260101_000000", "0000", "failed",
                               "forgedNtuple_9.root"), [(380126, 99)], [380126] * 50, ["HLT_A", "HLT_B"])

        # ---- runs_in_lumiblocks ----
        tool = os.path.join(HERE, "runs_in_lumiblocks.py")
        rc, out = run([tool, "--base", base, "--glob", "JetMET*/*Run2024C*/*/*/forgedNtuple_*.root",
                       "--runs", "380126", "380127", "380128"])
        print(out.rstrip().replace("\n", "\n    | "))
        check("D14 exit 0", rc == 0)
        check("D14 3 files, none unreadable", "FILES 3 unreadable 0" in out)
        check("D14 run 380126: 3 LS, 5 events, 2 files",
              "RUN 380126 LS_in_LuminosityBlocks=3 events_after_skim=5 files_with_run=2" in out)
        check("D14 run 380127: 2 LS (LS 2 counted once), 5 events, 2 files",
              "RUN 380127 LS_in_LuminosityBlocks=2 events_after_skim=5 files_with_run=2" in out)
        check("D14 run 380128: 2 LS, 0 events, 1 file",
              "RUN 380128 LS_in_LuminosityBlocks=2 events_after_skim=0 files_with_run=1" in out)
        # wide run span takes the per-run path; same numbers
        rc, out2 = run([tool, "--base", base, "--glob", "JetMET*/*Run2024C*/*/*/forgedNtuple_*.root",
                        "--runs", "380126", "1"])
        check("D14 wide span (per-run count) same numbers",
              "RUN 380126 LS_in_LuminosityBlocks=3 events_after_skim=5 files_with_run=2" in out2
              and "RUN 1 LS_in_LuminosityBlocks=0 events_after_skim=0 files_with_run=0" in out2)
        rc, out3 = run([tool, "--base", base, "--glob", "nothing/*.root", "--runs", "1"])
        check("D14 no file -> exit 2", rc == 2 and "ERROR no file matched" in out3)
        broken = c("JetMET0", "req_Run2024C", 6)
        with open(broken, "w") as fh:
            fh.write("not a root file")
        rc, out4 = run([tool, "--base", base, "--glob", "JetMET*/*Run2024C*/*/*/forgedNtuple_*.root",
                        "--runs", "380126"])
        check("D14 broken file -> exit 3, listed, others still counted",
              rc == 3 and "FILES 4 unreadable 1" in out4 and "UNREADABLE %s" % broken in out4
              and "RUN 380126 LS_in_LuminosityBlocks=3 events_after_skim=5" in out4)
        os.remove(broken)
        trunc = c("JetMET0", "req_Run2024C", 8)
        make_truncated(trunc)
        rc, out7 = run([tool, "--base", base, "--glob", "JetMET*/*Run2024C*/*/*/forgedNtuple_*.root",
                        "--runs", "380126"])
        check("D14 truncated (recovered) file -> unreadable, its LS/events not used",
              rc == 3 and "FILES 4 unreadable 1" in out7 and "truncated" in out7
              and "RUN 380126 LS_in_LuminosityBlocks=3 events_after_skim=5 files_with_run=2" in out7)
        os.remove(trunc)
        import ROOT
        nolb = c("JetMET0", "req_Run2024C", 9)
        f = ROOT.TFile(nolb, "RECREATE")
        lbt = ROOT.TTree("LuminosityBlocks", "")
        rr = array.array("I", [380126])
        lbt.Branch("run", rr, "run/i")
        lbt.Fill()
        evt = ROOT.TTree("Events", "")
        er = array.array("I", [380126])
        evt.Branch("run", er, "run/i")
        for _ in range(7):
            evt.Fill()
        f.Write()
        f.Close()
        rc, out8 = run([tool, "--base", base, "--glob", "JetMET*/*Run2024C*/*/*/forgedNtuple_*.root",
                        "--runs", "380126"])
        check("D14 read error -> unreadable, none of that file's events counted",
              rc == 3 and "read error" in out8
              and "RUN 380126 LS_in_LuminosityBlocks=3 events_after_skim=5 files_with_run=2" in out8)
        os.remove(nolb)

        # ---- branch_signature ----
        tool = os.path.join(HERE, "branch_signature.py")
        rc, out = run([tool, "--base", base])
        print(out.rstrip().replace("\n", "\n    | "))
        check("D16 exit 0", rc == 0)
        check("D16 JetMET0/req_Run2024C: 2 files, 1 set",
              re.search(r"^DS JetMET0/req_Run2024C +files=2 unreadable=0 branch_sets=1 differing=0$", out, re.M) is not None)
        check("D16 JetMET1/req_Run2024C kept apart from JetMET0",
              re.search(r"^DS JetMET1/req_Run2024C +files=1 unreadable=0 branch_sets=1 differing=0$", out, re.M) is not None)
        check("D16 JetMET0/req_Run2024I: 2 sets, HLT_C in 1 of 2",
              re.search(r"^DS JetMET0/req_Run2024I +files=2 unreadable=0 branch_sets=2 differing=1$", out, re.M) is not None
              and re.search(r"^   HLT_C +in 1 of 2 files$", out, re.M) is not None)
        check("D16 failed/ ignored, SUMMARY",
              "SUMMARY groups=3 with_differences=1 files=5 unreadable=0" in out)
        with open(c("JetMET1", "req_Run2024C", 7), "w") as fh:
            fh.write("broken")
        rc, out5 = run([tool, "--base", base])
        check("D16 broken file -> exit 3, counted in its group",
              rc == 3 and re.search(r"^DS JetMET1/req_Run2024C +files=2 unreadable=1 branch_sets=1", out5, re.M) is not None)
        os.remove(c("JetMET1", "req_Run2024C", 7))
        trunc = c("JetMET0", "req_Run2024I", 8)
        make_truncated(trunc)
        rc, out9 = run([tool, "--base", base])
        check("D16 truncated file -> exit 3, reason given",
              rc == 3 and "truncated" in out9
              and re.search(r"^DS JetMET0/req_Run2024I +files=3 unreadable=1 branch_sets=2", out9, re.M) is not None)
        os.remove(trunc)
        # a second campaign with the same PD/request names: groups must stay apart
        base2 = os.path.join(w, "base_v2")
        make_file(os.path.join(base2, "JetMET0", "req_Run2024C", "260202_000000", "0000", "forgedNtuple_1.root"),
                  [(380126, 1)], [380126], ["HLT_A", "HLT_B", "HLT_NEW"])
        rc, out10 = run([tool, "--base", w, "--glob", "*/*/*/*/*/forgedNtuple_*.root"])
        check("D16 two campaigns under --base stay apart",
              re.search(r"^DS base/JetMET0/req_Run2024C +files=2 unreadable=0 branch_sets=1 differing=0$", out10, re.M) is not None
              and re.search(r"^DS base_v2/JetMET0/req_Run2024C +files=1 unreadable=0 branch_sets=1 differing=0$", out10, re.M) is not None)
        rc, out6 = run([tool, "--base", base, "--group-level", "0"])
        check("D16 --group-level 0 -> exit 2", rc == 2)
    finally:
        shutil.rmtree(w, ignore_errors=True)
    print("RESULT: %d PASS, %d FAIL" % (NP[0], NP[1]))
    return 0 if NP[1] == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
