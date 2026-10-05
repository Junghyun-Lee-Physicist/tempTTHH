#!/usr/bin/env python3
"""y1_compare.py -- Y1: are the analyzer outputs of two runs the same? (STEP 24; PLAN 9.1 Stage 1 (f))

    python3 tools/stage1/y1_compare.py condor/y1/before condor/y1/after

For every <name>.root in either directory: every TH1 (in all sub-directories) is compared with the one
of the same path in the other file, bin by bin including under/overflow, and the entries (NaN equals
only NaN); every TTree by its entries and, per leaf, the count, sum, sum of squares, min, max and the
number of NaN over all entries and elements. A refactoring that does not touch 2017 must give identical
files.

Output: a FILE line per file (histograms identical/differing/only in A/only in B, trees), the first
differing histograms with their largest absolute and relative difference, a CUTFLOW line (the
'cutflow' histogram, unweighted counts) and RESULT.
PASS: every common histogram and tree leaf identical, no histogram or leaf only in A (something that
disappeared), the same tree entries, and the same file names in both directories. Histograms and tree
leaves only in B (new monitoring) are listed, not failed. A file EXPECTED in either directory (one name
per line, written by y1_reference.sh) lists runs that must have a <name>.root in both: a run that
stopped in both directories would otherwise just be absent from the comparison.
Exit 0 PASS, 1 FAIL, 2 bad arguments.
"""
from __future__ import print_function

import os
import sys

SHOW = 20


def walk(d, prefix=""):
    """{path: object} of the TH1 and TTree objects under a TDirectory (highest cycle only)."""
    out = {}
    seen = set()
    for k in d.GetListOfKeys():
        name = k.GetName()
        if name in seen:
            continue
        seen.add(name)
        o = k.ReadObj()
        p = prefix + name
        if o.InheritsFrom("TDirectory"):
            out.update(walk(o, p + "/"))
        elif o.InheritsFrom("TH1") or o.InheritsFrom("TTree"):
            out[p] = o
    return out


def same_value(x, y):
    """Exact equality, with NaN equal only to NaN (abs(x - nan) is nan and max() would hide it)."""
    return x == y or (x != x and y != y)


def tree_digest(t):
    """{branch/leaf: (n, sum, sumsq, min, max, n_nan)} over all entries and elements: what a value change
    would move. Scalars and arrays alike (leaf.GetLen() values per entry)."""
    leaves = [l for l in t.GetListOfLeaves()]
    acc = {}
    for l in leaves:
        acc[l.GetBranch().GetName() + "/" + l.GetName()] = [0, 0.0, 0.0, float("inf"), float("-inf"), 0]
    for i in range(int(t.GetEntries())):
        t.GetEntry(i)
        for l in leaves:
            a = acc[l.GetBranch().GetName() + "/" + l.GetName()]
            for j in range(l.GetLen()):
                v = l.GetValue(j)
                a[0] += 1
                if v != v:
                    a[5] += 1
                    continue
                a[1] += v
                a[2] += v * v
                a[3] = min(a[3], v)
                a[4] = max(a[4], v)
    return {k: tuple(v) for k, v in acc.items()}


def hist_values(h):
    n = (h.GetNbinsX() + 2) * (h.GetNbinsY() + 2 if h.GetDimension() > 1 else 1) * \
        (h.GetNbinsZ() + 2 if h.GetDimension() > 2 else 1)
    return [h.GetBinContent(i) for i in range(n)], h.GetEntries()


def compare_file(ROOT, fa, fb):
    ta, tb = ROOT.TFile.Open(fa), ROOT.TFile.Open(fb)
    if not ta or ta.IsZombie() or not tb or tb.IsZombie():
        return None
    A, B = walk(ta), walk(tb)
    res = {"hist": 0, "same": 0, "diff": [], "onlyA": [], "onlyB": [], "trees": 0, "tree_diff": [], "cutflow": None}
    for p, oa in A.items():
        ob = B.get(p)
        if ob is None:
            res["onlyA"].append(p)
            continue
        if oa.InheritsFrom("TTree"):
            res["trees"] += 1
            if not ob.InheritsFrom("TTree") or oa.GetEntries() != ob.GetEntries():
                res["tree_diff"].append("%s entries %d vs %d" % (p, oa.GetEntries(), ob.GetEntries() if ob.InheritsFrom("TTree") else -1))
                continue
            da, db = tree_digest(oa), tree_digest(ob)
            for k in sorted(da):
                if k not in db:
                    res["tree_diff"].append("%s branch %s only in A" % (p, k))
                elif not all(same_value(x, y) for x, y in zip(da[k], db[k])):
                    res["tree_diff"].append("%s branch %s n/sum/sumsq/min/max/nan %s vs %s" % (p, k, da[k], db[k]))
            res["tree_onlyB"] = res.get("tree_onlyB", []) + ["%s branch %s" % (p, k) for k in sorted(db) if k not in da]
            continue
        res["hist"] += 1
        if not ob.InheritsFrom("TH1"):
            res["diff"].append((p, float("inf"), float("inf")))
            continue
        va, ea = hist_values(oa)
        vb, eb = hist_values(ob)
        if len(va) != len(vb):
            res["diff"].append((p, float("inf"), float("inf")))
            continue
        same = same_value(ea, eb) and all(same_value(x, y) for x, y in zip(va, vb))
        finite = [(x, y) for x, y in zip(va, vb) if x == x and y == y]
        mabs = 0.0
        if not same:
            mabs = max([abs(x - y) for x, y in finite] + [abs(ea - eb) if ea == ea and eb == eb else 0.0])
            if mabs == 0.0:
                mabs = float("nan")   # differs only where a NaN is
        mrel = max([abs(x - y) / abs(x) if x else (0.0 if y == 0 else float("inf")) for x, y in finite] + [0.0])
        if same:
            res["same"] += 1
        else:
            res["diff"].append((p, mabs, mrel))
        if p.split("/")[-1] == "cutflow":
            res["cutflow"] = (same, [x if x != x else int(round(x)) for x in va[1:-1]],
                              [x if x != x else int(round(x)) for x in vb[1:-1]])
    res["onlyB"] = sorted(p for p in B if p not in A)
    ta.Close()
    tb.Close()
    return res


def main(argv=None):
    argv = sys.argv[1:] if argv is None else argv
    if len(argv) != 2 or not all(os.path.isdir(d) for d in argv):
        print(__doc__)
        return 2
    import ROOT
    ROOT.gROOT.SetBatch(True)
    da, db = argv
    fa_names = set(f for f in os.listdir(da) if f.endswith(".root"))
    fb_names = set(f for f in os.listdir(db) if f.endswith(".root"))
    files = sorted(fa_names | fb_names)
    if not files:
        print("ERROR no .root file in %s or %s" % (da, db))
        return 2
    bad = []
    expected = set()
    for d in (da, db):
        p = os.path.join(d, "EXPECTED")
        if os.path.isfile(p):
            with open(p) as f:
                expected.update(l.strip() for l in f if l.strip() and not l.startswith("#"))
    for name in sorted(expected):
        if name + ".root" not in fa_names and name + ".root" not in fb_names:
            print("FILE %s.root EXPECTED but in neither directory (the run stopped in both?)" % name)
            bad.append(name + " expected, absent in both")
    for f in files:
        fb = os.path.join(db, f)
        if f not in fb_names:
            print("FILE %s MISSING in %s" % (f, db))
            bad.append(f + " missing in B")
            continue
        if f not in fa_names:
            print("FILE %s MISSING in %s (a run that wrote no output there?)" % (f, da))
            bad.append(f + " missing in A")
            continue
        r = compare_file(ROOT, os.path.join(da, f), fb)
        if r is None:
            print("FILE %s cannot be opened" % f)
            bad.append(f + " unreadable")
            continue
        print("FILE %s hists=%d identical=%d differ=%d onlyA=%d onlyB=%d trees=%d tree_diff=%d"
              % (f, r["hist"], r["same"], len(r["diff"]), len(r["onlyA"]), len(r["onlyB"]), r["trees"], len(r["tree_diff"])))
        for p, mabs, mrel in sorted(r["diff"], key=lambda x: -x[1])[:SHOW]:
            print("   DIFF %s max_abs=%.6g max_rel=%.3g" % (p, mabs, mrel))
        for p in r["onlyA"][:SHOW]:
            print("   ONLY_A %s" % p)
        for p in r["onlyB"][:SHOW]:
            print("   ONLY_B %s" % p)
        for t in r["tree_diff"][:SHOW]:
            print("   TREE %s" % t)
        for t in r.get("tree_onlyB", [])[:SHOW]:
            print("   TREE_ONLY_B %s" % t)
        if r["cutflow"] is None:
            print("CUTFLOW %s no 'cutflow' histogram" % f)
            bad.append(f + " no cutflow")
        else:
            same, ca, cb = r["cutflow"]
            print("CUTFLOW %s %s A=%s" % (f, "equal" if same else "DIFFER", ca))
            if not same:
                print("CUTFLOW %s        B=%s" % (f, cb))
        if r["diff"] or r["onlyA"] or r["tree_diff"]:
            bad.append(f)
    print("RESULT %s" % ("PASS" if not bad else "FAIL (" + ", ".join(bad) + ")"))
    return 0 if not bad else 1


if __name__ == "__main__":
    sys.exit(main())
