#!/usr/bin/env python3
"""runs_in_lumiblocks.py -- are the given runs in our ntuples? (STEP 23; PLAN section 9.6 D14)

For every file BASE/GLOB it counts, for each requested run,
  * the distinct luminosity blocks of the LuminosityBlocks tree (every LS the
    job processed, before any skim), and
  * the Events entries of that run (after the skim).

    python3 tools/stage0/runs_in_lumiblocks.py --base <dir> --glob '<pattern under base>' --runs 380126 380127 380128

Output lines
    FILES <n> unreadable <m>
    RUN <run> LS_in_LuminosityBlocks=<n> events_after_skim=<n> files_with_run=<n>
    UNREADABLE <file> <reason>        (at most 20)
Exit: 0 every file was read; 3 some files could not be read (the counts
cover the others); 2 bad arguments or no file matched.

A file counts as unreadable -- and none of its numbers are used -- when it
cannot be opened, is truncated (ROOT had to recover its keys, e.g. after a
failed stage-out), lacks one of the two trees, or raises a read error. A
damaged basket in the middle of Events that ROOT only reports on stderr
("Error in <TBranch::GetBasket>") is not detected here.
"""
from __future__ import print_function

import argparse
import collections
import glob
import os
import sys


def first_line(e):
    lines = str(e).splitlines()
    return (lines[0] if lines else repr(e))[:120]


def open_root(ROOT, path, bad):
    """TFile.Open that never raises: newer PyROOT (6.30 at KNU and lxplus) raises
    OSError for an unreadable file instead of returning a null/zombie TFile."""
    try:
        tf = ROOT.TFile.Open(path)
    except Exception as e:  # OSError from the PyROOT pythonization, or anything else
        bad.append((path, "cannot open (%s)" % first_line(e)))
        return None
    if not tf or tf.IsZombie():
        bad.append((path, "cannot open (zombie)"))
        return None
    if tf.TestBit(ROOT.TFile.kRecovered):
        bad.append((path, "truncated (ROOT recovered its keys; e.g. a failed stage-out)"))
        tf.Close()
        return None
    return tf


def scan_file(ROOT, tf, i, want, wantset, lo, hi, one_pass):
    """-> (run -> set of LS, run -> events) of one file; raises on a read error."""
    lb = tf.Get("LuminosityBlocks")
    ev = tf.Get("Events")
    if not lb or not ev:
        raise RuntimeError("no LuminosityBlocks or Events tree")
    f_ls = collections.defaultdict(set)
    f_nev = collections.Counter()
    lb.SetBranchStatus("*", 0)
    lb.SetBranchStatus("run", 1)
    lb.SetBranchStatus("luminosityBlock", 1)
    for e in lb:
        r = int(e.run)
        if r in wantset:
            f_ls[r].add(int(e.luminosityBlock))
    if one_pass:
        # one pass over Events: a histogram with one bin per run in [lo, hi],
        # kept in memory (gROOT), not in the read-only input file
        ROOT.gROOT.cd()
        hname = "h_runs_%d" % i
        h = ROOT.TH1D(hname, "", hi - lo + 1, lo - 0.5, hi + 0.5)
        ev.Project(hname, "run")
        for r in want:
            f_nev[r] += int(round(h.GetBinContent(h.FindBin(r))))
        del h
    else:
        for r in want:
            f_nev[r] += int(ev.GetEntries("run==%d" % r))
    return f_ls, f_nev


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--base", required=True, help="directory the pattern is relative to")
    ap.add_argument("--glob", required=True, help="file pattern under --base (quote it)")
    ap.add_argument("--runs", required=True, nargs="+", type=int, help="run numbers")
    a = ap.parse_args(argv)

    files = sorted(glob.glob(os.path.join(a.base, a.glob)))
    print("BASE %s" % a.base)
    print("GLOB %s" % a.glob)
    print("RUNS %s" % " ".join(str(r) for r in a.runs))
    sys.stdout.flush()
    if not files:
        print("ERROR no file matched %s" % os.path.join(a.base, a.glob))
        return 2

    import ROOT  # imported late: argument errors should not need ROOT
    ROOT.gROOT.SetBatch(True)

    want = sorted(set(a.runs))
    wantset = set(want)
    lo, hi = want[0], want[-1]
    one_pass = (hi - lo) <= 100000
    ls = collections.defaultdict(set)
    nev = collections.Counter()
    nfiles = collections.Counter()
    bad = []
    for i, f in enumerate(files):
        tf = open_root(ROOT, f, bad)
        if tf is None:
            continue
        try:
            f_ls, f_nev = scan_file(ROOT, tf, i, want, wantset, lo, hi, one_pass)
        except Exception as e:  # a read error or a missing tree: the whole file is left out
            bad.append((f, "read error (%s)" % first_line(e)))
            tf.Close()
            continue
        tf.Close()
        for r in want:
            if f_ls[r] or f_nev[r]:
                nfiles[r] += 1
            ls[r] |= f_ls[r]
            nev[r] += f_nev[r]
        if (i + 1) % 200 == 0:
            print("progress %d/%d" % (i + 1, len(files)))
            sys.stdout.flush()

    print("FILES %d unreadable %d" % (len(files), len(bad)))
    for r in want:
        print("RUN %d LS_in_LuminosityBlocks=%d events_after_skim=%d files_with_run=%d"
              % (r, len(ls[r]), nev[r], nfiles[r]))
    for f, why in bad[:20]:
        print("UNREADABLE %s %s" % (f, why))
    if len(bad) > 20:
        print("UNREADABLE ... and %d more" % (len(bad) - 20))
    return 3 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
