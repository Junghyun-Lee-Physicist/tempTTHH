#!/usr/bin/env python3
"""test_stage1.py -- offline tests of the STEP 24 tools (needs PyROOT; the manifest and record tests
also need a BUILT treestream fork: --treestream, default ../treestream next to this repository).

    python3 tools/stage1/test_stage1.py [--treestream DIR] [--keep]

Small NanoAOD-like files are written into a fresh temporary directory (printed; removed at the end
unless --keep). Covered: eventbuffer_manifest.py (first readable file per dataset, unique input
names, HLT/L1 pruning, keep-list WARN, CODE_MISSING exit 1, NOINPUT exit 1, type conflicts),
eventbuffer_from_record.py (check-only, write + stamp, tampered record), pu_weights.py (mcprofile with
a bad file, weights + correctionlib read-back, binning mismatch), y1_compare.py (identical, changed
cutflow, histogram only in B, histogram gone, NaN bin, file only in B, tree value, EXPECTED run absent
in both).
Prints PASS/FAIL per check and 'RESULT: <n> PASS, <m> FAIL'. Exit 0 only when all pass.
"""
from __future__ import print_function

import argparse
import array
import hashlib
import json
import math
import os
import random
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
RESULTS = []


def check(name, cond, detail=""):
    RESULTS.append(bool(cond))
    print("%s  %s" % ("PASS" if cond else "FAIL", name))
    if not cond and detail:
        print("      " + str(detail)[-1500:].replace("\n", "\n      "))


def run(cmd, cwd=None, env=None):
    p = subprocess.run(cmd, cwd=cwd, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                       universal_newlines=True)
    return p.returncode, p.stdout


def write_ntuple(ROOT, path, spec, nev=20, seed=1):
    """spec: list of (name, leaflist type code, counter or None, value function(ev, j))."""
    rnd = random.Random(seed)
    f = ROOT.TFile(path, "RECREATE")
    t = ROOT.TTree("Events", "Events")
    bufs = {}
    codes = {"F": "f", "I": "i", "i": "I", "b": "B", "O": "b", "l": "L", "s": "h"}
    for name, code, counter, _ in spec:
        n = 20 if counter else 1
        bufs[name] = array.array(codes[code], [0] * n)
        leaf = "%s[%s]/%s" % (name, counter, code) if counter else "%s/%s" % (name, code)
        t.Branch(name, bufs[name], leaf)
    for ev in range(nev):
        nj = 2 + ev % 4
        for name, code, counter, fn in spec:
            if counter:
                for j in range(nj):
                    bufs[name][j] = fn(ev, j, rnd)
            elif name.startswith("n") and name[1:] and any(s[2] == name for s in spec):
                bufs[name][0] = nj
            else:
                bufs[name][0] = fn(ev, 0, rnd)
        t.Fill()
    t.Write()
    f.Close()


def spec_v9():
    return [("run", "i", None, lambda e, j, r: 1), ("luminosityBlock", "i", None, lambda e, j, r: 1 + e // 10),
            ("event", "l", None, lambda e, j, r: e), ("nJet", "i", None, None),
            ("Jet_pt", "F", "nJet", lambda e, j, r: 30.0 + j), ("Jet_jetId", "I", "nJet", lambda e, j, r: 6),
            ("PV_npvsGood", "I", None, lambda e, j, r: 20), ("MET_pt", "F", None, lambda e, j, r: 40.0),
            ("HLT_PFHT1050", "O", None, lambda e, j, r: e % 2), ("HLT_IsoMu27", "O", None, lambda e, j, r: 0),
            ("HLT_Old_v", "O", None, lambda e, j, r: 1), ("L1_SingleMu", "O", None, lambda e, j, r: 1),
            ("Flag_goodVertices", "O", None, lambda e, j, r: 1)]


def spec_v15():
    return [("run", "i", None, lambda e, j, r: 2), ("luminosityBlock", "i", None, lambda e, j, r: 1 + e // 10),
            ("event", "l", None, lambda e, j, r: e), ("nJet", "i", None, None),
            ("Jet_pt", "F", "nJet", lambda e, j, r: 31.0 + j), ("Jet_chMultiplicity", "b", "nJet", lambda e, j, r: 5),
            ("PV_npvsGood", "b", None, lambda e, j, r: 30), ("PuppiMET_pt", "F", None, lambda e, j, r: 35.0),
            ("Pileup_nTrueInt", "F", None, lambda e, j, r: max(0.5, r.gauss(45.0, 12.0))),
            ("HLT_PFHT1050", "O", None, lambda e, j, r: 1), ("HLT_New", "O", None, lambda e, j, r: 0),
            ("L1_Other", "O", None, lambda e, j, r: 1), ("Flag_goodVertices", "O", None, lambda e, j, r: 1)]


def crab_file(base, pd, req, name="forgedNtuple_1.root"):
    d = os.path.join(base, pd, req, "260101_000000", "0000")
    os.makedirs(d, exist_ok=True)
    return os.path.join(d, name)


def write_hist(ROOT, path, nbins, lo, hi, mean, sigma, name="pileup", total=1e6):
    f = ROOT.TFile(path, "RECREATE")
    h = ROOT.TH1D(name, name, nbins, lo, hi)
    for i in range(1, nbins + 1):
        x = h.GetBinCenter(i)
        h.SetBinContent(i, total * math.exp(-0.5 * ((x - mean) / sigma) ** 2))
    h.Write()
    f.Close()


def test_manifest(ROOT, ts, W):
    b1, b2, b3 = os.path.join(W, "b1"), os.path.join(W, "b2"), os.path.join(W, "b3")
    f1 = crab_file(b1, "PDa", "reqA")
    write_ntuple(ROOT, f1, spec_v9(), seed=1)
    open(crab_file(b1, "PDa", "reqA", "forgedNtuple_0.root"), "w").close()       # 0 byte, sorted first
    os.makedirs(os.path.join(os.path.dirname(f1), "failed"), exist_ok=True)
    open(os.path.join(os.path.dirname(f1), "failed", "forgedNtuple_7.root"), "w").write("junk")
    f2 = crab_file(b2, "PDb", "reqB")
    write_ntuple(ROOT, f2, spec_v15(), seed=2)
    src = os.path.join(W, "src.cc")
    with open(src, "w") as f:
        f.write('bool a = _ev->HLT_PFHT1050; auto b = "Events/HLT_IsoMu27"; // HLT_Comment_only\n')
    keep = os.path.join(W, "keep.txt")
    with open(keep, "w") as f:
        f.write("# test\nHLT_PFHT1050\nHLT_IsoMu27  # ref\nHLT_Absent\n")
    base_args = [sys.executable, os.path.join(REPO, "tools", "stage1", "eventbuffer_manifest.py"), "--treestream", ts,
                 "--keep", keep, "--source", src]
    # a second, older CRAB task of the v15 dataset with one more branch: group level 2 takes a file of each task
    f2old = os.path.join(b2, "PDb", "reqB", "250101_000000", "0000", "forgedNtuple_1.root")
    os.makedirs(os.path.dirname(f2old), exist_ok=True)
    write_ntuple(ROOT, f2old, spec_v15() + [("Jet_oldTaskOnly", "F", "nJet", lambda e, j, r: 1.0)], seed=3)
    rc, out = run(base_args + ["--base", "v9=" + b1, "--base", "v15=" + b2, "--work", os.path.join(W, "w1")])
    log1 = os.path.join(W, "manifest1.log")
    with open(log1, "w") as f:
        f.write(out)
    check("manifest: exit 0", rc == 0, out[-2500:])
    inputs = re.findall(r"^INPUT \d+ (\S+) (\S+) branches=(\d+) (\S+)$", out, re.M)
    check("manifest: one input per CRAB task, the readable file", len(inputs) == 3 and inputs[0][3] == f1
          and sorted(i[3] for i in inputs[1:]) == sorted([f2, f2old]), inputs)
    check("manifest: the 0-byte file is skipped with a reason",
          re.search(r"^SKIP v9 PDa/reqA/260101_000000 \S+forgedNtuple_0\.root ", out, re.M) is not None, out[:1500])
    check("manifest: failed/ is not looked at", "failed/forgedNtuple_7" not in out)
    links = sorted(os.listdir(os.path.join(W, "w1", "inputs")))
    check("manifest: unique link names although the files are all forgedNtuple_1.root",
          links == ["000_v9__PDa__reqA__260101_000000.root", "001_v15__PDb__reqB__250101_000000.root",
                    "002_v15__PDb__reqB__260101_000000.root"], links)
    m = re.search(r"^PRUNE records_in=(\d+) kept=(\d+) dropped_HLT=(\d+) dropped_L1=(\d+) kept_menu=(\d+)$", out, re.M)
    check("manifest: HLT_Old_v, HLT_New, L1_SingleMu, L1_Other dropped; 2 HLT kept",
          m is not None and m.group(3) == "2" and m.group(4) == "2" and m.group(5) == "2", m and m.groups())
    check("manifest: keep-list name in no input -> KEEP_ABSENT", "KEEP_ABSENT HLT_Absent" in out)
    check("manifest: analyzer HLT names found, none missing (comments ignored)",
          "CODE_HLT names=2 missing=0" in out, out[-1500:])
    check("manifest: type conflict summarized per type", re.search(r"^CONFLICT PV_npvsGood: (int x1, uchar x2|uchar x2, int x1) -> int$", out, re.M) is not None,
          [l for l in out.splitlines() if l.startswith("CONFLICT")])
    blk = re.search(r"^BEGIN VARIABLES ([0-9a-f]{32})\n(.*?)^END VARIABLES$", out, re.M | re.S)
    check("manifest: variables block with its md5",
          blk is not None and hashlib.md5(blk.group(2).encode()).hexdigest() == blk.group(1))
    if blk:
        names = [l.split("/")[-2] for l in blk.group(2).splitlines() if l and not l.startswith(("#", "Tree")) and "/" in l]
        check("manifest: block has the v9 and v15 branches (both tasks) and only the kept HLT",
              {"Jet_jetId", "Jet_chMultiplicity", "MET_pt", "PuppiMET_pt", "HLT_PFHT1050", "HLT_IsoMu27", "Jet_oldTaskOnly"} <= set(names)
              and not any(n.startswith("L1_") or n in ("HLT_Old_v", "HLT_New") for n in names), names)
    check("manifest: header generated (md5 without Created/Author)", re.search(r"^HEADER rc=0 lines=\d+ md5_normalized=[0-9a-f]{32}", out, re.M) is not None)
    check("manifest: RESULT OK", "RESULT OK" in out)

    rc, outm = run(base_args + ["--base", "v9=" + b1, "--base", "none=" + os.path.join(W, "nope"), "--work", os.path.join(W, "w4")])
    check("manifest: a missing base -> MISSINGBASE, exit 1", rc == 1 and "MISSINGBASE none " in outm and "RESULT FAIL" in outm, outm[-800:])
    os.makedirs(os.path.join(W, "emptybase", "PD", "req"), exist_ok=True)
    rc, oute = run(base_args + ["--base", "v9=" + b1, "--base", "e=" + os.path.join(W, "emptybase"), "--work", os.path.join(W, "w5")])
    check("manifest: a base that matches no file -> EMPTYBASE, exit 1", rc == 1 and "EMPTYBASE e " in oute, oute[-800:])
    rc, outr = run(base_args + ["--base", "v9=" + os.path.relpath(b1, W), "--base", "v15=" + os.path.relpath(b2, W),
                                "--work", os.path.join(W, "w6")], cwd=W)
    check("manifest: relative --base works (links point to absolute paths)", rc == 0 and "RESULT OK" in outr, outr[-1200:])
    with open(src, "w") as f:
        f.write("bool a = _ev->HLT_PFHT1050 || _ev->HLT_NotThere;\n")
    rc, out2 = run(base_args + ["--base", "v9=" + b1, "--work", os.path.join(W, "w2")])
    check("manifest: an analyzer HLT name not kept -> CODE_MISSING, exit 1",
          rc == 1 and "CODE_MISSING HLT_NotThere" in out2 and "RESULT FAIL" in out2, out2[-1200:])
    open(crab_file(b3, "PDc", "reqC", "forgedNtuple_3.root"), "w").close()
    with open(src, "w") as f:
        f.write("bool a = _ev->HLT_PFHT1050;\n")
    rc, out3 = run(base_args + ["--base", "v9=" + b1, "--base", "bad=" + b3, "--work", os.path.join(W, "w3")])
    check("manifest: a task without a readable file -> NOINPUT, exit 1", rc == 1 and "NOINPUT bad PDc/reqC/260101_000000" in out3, out3[-1200:])
    log3 = os.path.join(W, "manifest3.log")
    with open(log3, "w") as f:
        f.write(out3)
    return log1, log3


def test_record(ts, W, log1, log3):
    tool = [sys.executable, os.path.join(REPO, "tools", "stage1", "eventbuffer_from_record.py")]
    rc, out = run(tool + ["--record", log1, "--treestream", ts, "--check-only"])
    check("record: check-only reproduces the header", rc == 0 and "RESULT OK (check only" in out, out)
    repo = os.path.join(W, "repo")
    os.makedirs(os.path.join(repo, "include"))
    os.makedirs(os.path.join(repo, "src"))
    rc, out = run(tool + ["--record", log1, "--treestream", ts, "--repo", repo])
    check("record: write mode exit 0", rc == 0 and "RESULT OK" in out, out)
    hdr = open(os.path.join(repo, "include", "eventBuffer.h")).read() if rc == 0 else ""
    vmd5 = re.search(r"^BEGIN VARIABLES ([0-9a-f]{32})", open(log1).read(), re.M).group(1)
    check("record: stamp after the include guard, with the variables md5",
          re.search(r'#define EVENTBUFFER_H\n.*\n#define TTHH_EVENTBUFFER_STAMP "treestream \S+; variables md5 %s; record manifest1\.log"' % vmd5, hdr) is not None,
          hdr[:600])
    check("record: Created/Author lines fixed (no time, no user)",
          re.search(r"^// Created:     by mkanalyzer\.py", hdr, re.M) is not None and "generated: tools/stage1/eventbuffer_from_record.py" in hdr)
    vt = os.path.join(repo, "include", "eventBuffer_variables.txt")
    check("record: eventBuffer_variables.txt = the record block",
          os.path.isfile(vt) and hashlib.md5(open(vt, "rb").read()).hexdigest() == vmd5)
    same = all(os.path.isfile(os.path.join(repo, d, f)) and
               open(os.path.join(repo, d, f), "rb").read() == open(os.path.join(ts, d, f), "rb").read()
               for d, f in (("src", "treestream.cc"), ("include", "treestream.h")))
    check("record: treestream.cc/.h copied from the fork", same)
    bad = os.path.join(W, "tampered.log")
    txt = open(log1).read()
    txt = re.sub(r"(BEGIN VARIABLES [0-9a-f]{32}\n(?:.*\n)*?)(float/Events/Jet_pt/)", r"\1double/Events/Jet_pt/", txt, count=1)
    with open(bad, "w") as f:
        f.write(txt)
    rc, out = run(tool + ["--record", bad, "--treestream", ts, "--check-only"])
    check("record: an edited block is refused (exit 1)", rc == 1 and "variables md5 differs" in out, out)
    rc, out = run(tool + ["--record", log3, "--treestream", ts, "--check-only"])
    check("record: the record of a failed manifest run is refused (exit 2)", rc == 2 and "RESULT OK" in out, out)
    clone = os.path.join(W, "tsclone")
    rcc, outc = run(["git", "clone", "-q", ts, clone])
    if rcc == 0:
        with open(os.path.join(clone, "src", "treestream.cc"), "a") as f:
            f.write("// local edit\n")
        rc, out = run(tool + ["--record", log1, "--treestream", clone, "--check-only"])
        check("record: a fork with local changes in treestream.cc is refused (exit 1)", rc == 1 and "local changes" in out, out)
    else:
        check("record: (could not clone the fork for the local-change test)", False, outc)


def test_pu(ROOT, W):
    tool = [sys.executable, os.path.join(REPO, "tools", "stage2", "pu_weights.py")]
    dd = os.path.join(W, "pudata")
    os.makedirs(dd)
    names = {"nominal": "dataPileupHistogram-2024CDEFGHI_Golden-69200ub.root",
             "down": "dataPileupHistogram-2024CDEFGHI_Golden-66000ub.root",
             "up": "dataPileupHistogram-2024CDEFGHI_Golden-72400ub.root"}
    write_hist(ROOT, os.path.join(dd, names["nominal"]), 100, 0, 100, 50.0, 8.5)
    write_hist(ROOT, os.path.join(dd, names["down"]), 100, 0, 100, 47.7, 8.1)
    write_hist(ROOT, os.path.join(dd, names["up"]), 100, 0, 100, 52.3, 8.9)
    mc = os.path.join(W, "mc")
    for k in range(2):
        write_ntuple(ROOT, crab_file(mc, "ZZ", "ZZ", "forgedNtuple_%d.root" % (k + 1)), spec_v15(), nev=4000, seed=10 + k)
    open(crab_file(mc, "ZZ", "ZZ", "forgedNtuple_9.root"), "w").close()
    rc, out = run(tool + ["mcprofile", "--glob", os.path.join(mc, "*/*/*/*/*.root"), "--binning-from", os.path.join(dd, names["nominal"])])
    rec = os.path.join(W, "pumc.log")
    with open(rec, "w") as f:
        f.write(out)
    check("pu mcprofile: a 0-byte file -> BADFILE, exit 3, profile still made",
          rc == 3 and re.search(r"^BADFILE \S+forgedNtuple_9\.root size 0$", out, re.M) is not None
          and "PUMC_FILES good=2 bad=1 events=8000" in out, out[-1500:])
    cnt = re.search(r"^PUMC_COUNTS (.*)$", out, re.M)
    st = re.search(r"^PUMC_STATS entries=(\d+) underflow=(\S+) overflow=(\S+)", out, re.M)
    tot = sum(float(x) for x in cnt.group(1).split(",")) + float(st.group(2)) + float(st.group(3)) if cnt and st else -1
    check("pu mcprofile: counts + under + overflow = events", abs(tot - 8000) < 1e-9, tot)
    rc, outb = run(tool + ["mcprofile", "--glob", os.path.join(mc, "*/*/*/*/*.root"), "--binning-from",
                           os.path.join(dd, names["nominal"]), "--branch", "Pileup_noSuchBranch"])
    check("pu mcprofile: a branch the files do not have -> exit 1, no profile", rc == 1 and "ERROR Draw" in outb
          and "PUMC_COUNTS" not in outb, outb[-800:])
    js = os.path.join(W, "pu.json")
    rc, out = run(tool + ["weights", "--data-dir", dd, "--mc-record", rec, "--out", js])
    rb = re.search(r"^READBACK correctionlib \S+ max_abs_diff=(\S+) ", out, re.M)
    check("pu weights: exit 0, correctionlib read-back within 1e-12",
          rc == 0 and rb is not None and float(rb.group(1)) <= 1e-12 and "RESULT OK" in out, out[-1500:])
    if rc == 0:
        j = json.load(open(js))
        c = j["corrections"][0]
        keys = [x["key"] for x in c["data"]["content"]]
        b = c["data"]["content"][0]["value"]
        check("pu weights: jsonpog shape (inputs, nominal/up/down, clamp, 101 edges)",
              [i["name"] for i in c["inputs"]] == ["NumTrueInteractions", "weights"] and keys == ["nominal", "up", "down"]
              and b["flow"] == "clamp" and len(b["edges"]) == 101 and len(b["content"]) == 100, (keys, b.get("flow")))
        m = re.search(r"^PUW nominal\s+data_mean=\S+ max_w=\S+ closure_sum_mw=(\S+) lost_data=(\S+)$", out, re.M)
        check("pu weights: sum m*w = 1 - lost data", m is not None and abs(float(m.group(1)) - (1 - float(m.group(2)))) < 1e-9,
              m and m.groups())
    dd2 = os.path.join(W, "pudata2")
    shutil.copytree(dd, dd2)
    write_hist(ROOT, os.path.join(dd2, names["up"]), 50, 0, 100, 52.3, 8.9)
    rc, out = run(tool + ["weights", "--data-dir", dd2, "--mc-record", rec, "--out", os.path.join(W, "pu2.json")])
    check("pu weights: a data histogram in another binning -> exit 1", rc == 1 and "binning of data up differs" in out, out[-800:])


def write_output(ROOT, path, cutflow, extra=True, drop=None, nan_bin=False, tree_shift=0.0):
    f = ROOT.TFile(path, "RECREATE")
    d = f.mkdir("hists")
    d.cd()
    h = ROOT.TH1F("cutflow", "", len(cutflow), 0, len(cutflow))
    for i, v in enumerate(cutflow):
        h.SetBinContent(i + 1, v)
    h.Write()
    for n in ("ht", "nb"):
        if n == drop:
            continue
        g = ROOT.TH1F(n, "", 10, 0, 10)
        g.Fill(3)
        if nan_bin and n == "ht":
            g.SetBinContent(5, float("nan"))
        g.Write()
    if extra:
        x = ROOT.TH1F("new_monitor", "", 3, 0, 3)
        x.Write()
    f.cd()
    t = ROOT.TTree("tree", "tree")
    v = array.array("f", [0])
    t.Branch("x", v, "x/F")
    for i in range(5):
        v[0] = i + (tree_shift if i == 2 else 0.0)
        t.Fill()
    t.Write()
    f.Close()


def test_y1(ROOT, W):
    tool = [sys.executable, os.path.join(REPO, "tools", "stage1", "y1_compare.py")]
    a, b, c, d, e, g, k = (os.path.join(W, x) for x in ("ya", "yb", "yc", "yd", "ye", "yg", "yk"))
    for x in (a, b, c, d, e, g, k):
        os.makedirs(x)
    write_output(ROOT, os.path.join(a, "mc.root"), [10, 8, 5], extra=False)
    write_output(ROOT, os.path.join(b, "mc.root"), [10, 8, 5], extra=True)
    rc, out = run(tool + [a, b])
    check("y1: identical histograms, one new only in B -> PASS", rc == 0 and "RESULT PASS" in out and "ONLY_B hists/new_monitor" in out, out)
    write_output(ROOT, os.path.join(c, "mc.root"), [10, 7, 5], extra=False)
    rc, out = run(tool + [a, c])
    check("y1: a changed cutflow bin -> FAIL with both cutflows", rc == 1 and "CUTFLOW mc.root DIFFER A=[10, 8, 5]" in out and "B=[10, 7, 5]" in out, out)
    write_output(ROOT, os.path.join(d, "mc.root"), [10, 8, 5], extra=False, drop="nb")
    rc, out = run(tool + [a, d])
    check("y1: a histogram gone in B -> FAIL (ONLY_A)", rc == 1 and "ONLY_A hists/nb" in out, out)
    write_output(ROOT, os.path.join(e, "mc.root"), [10, 8, 5], extra=False, nan_bin=True)
    rc, out = run(tool + [a, e])
    check("y1: a NaN bin in B -> FAIL", rc == 1 and "DIFF hists/ht" in out, out)
    write_output(ROOT, os.path.join(g, "mc.root"), [10, 8, 5], extra=False)
    write_output(ROOT, os.path.join(g, "data.root"), [10, 8, 5], extra=False)
    rc, out = run(tool + [a, g])
    check("y1: a file only in B (A wrote no output) -> FAIL", rc == 1 and "data.root missing in A" in out, out)
    write_output(ROOT, os.path.join(k, "mc.root"), [10, 8, 5], extra=False, tree_shift=0.5)
    rc, out = run(tool + [a, k])
    check("y1: a tree value changed, same entries -> FAIL", rc == 1 and "TREE tree branch x/x" in out, out)
    # EXPECTED (y1_reference.sh): a run that stopped in both directories must not pass unnoticed
    m, n = os.path.join(W, "ym"), os.path.join(W, "yn")
    for x in (m, n):
        os.makedirs(x)
        write_output(ROOT, os.path.join(x, "data.root"), [10, 8, 5], extra=False)
        with open(os.path.join(x, "EXPECTED"), "w") as f:
            f.write("mc\ndata\n")
    rc, out = run(tool + [m, n])
    check("y1: an EXPECTED run absent in both directories -> FAIL", rc == 1 and "mc.root EXPECTED but in neither" in out, out)
    write_output(ROOT, os.path.join(m, "mc.root"), [10, 8, 5], extra=False)
    write_output(ROOT, os.path.join(n, "mc.root"), [10, 8, 5], extra=False)
    rc, out = run(tool + [m, n])
    check("y1: every EXPECTED run present and identical -> PASS", rc == 0 and "RESULT PASS" in out, out)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--treestream", default=os.path.join(os.path.dirname(REPO), "treestream"))
    ap.add_argument("--keep", action="store_true")
    a = ap.parse_args()
    import ROOT
    ROOT.gROOT.SetBatch(True)
    W = tempfile.mkdtemp(prefix="test_stage1_")
    print("WORK %s" % W)
    ts = os.path.abspath(a.treestream)
    have_ts = os.path.isfile(os.path.join(ts, "bin", "mkeventbuffer.py")) and \
        any(n.startswith("libtreestream") for n in os.listdir(os.path.join(ts, "lib"))) if os.path.isdir(os.path.join(ts, "lib")) else False
    if have_ts:
        log1, log3 = test_manifest(ROOT, ts, W)
        test_record(ts, W, log1, log3)
    else:
        check("treestream fork (built) found at %s" % ts, False, "give --treestream <built fork checkout>")
    test_pu(ROOT, W)
    test_y1(ROOT, W)
    n_ok = sum(RESULTS)
    print("RESULT: %d PASS, %d FAIL" % (n_ok, len(RESULTS) - n_ok))
    if not a.keep:
        shutil.rmtree(W, ignore_errors=True)
    return 0 if n_ok == len(RESULTS) else 1


if __name__ == "__main__":
    sys.exit(main())
