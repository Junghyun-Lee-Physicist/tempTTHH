#!/usr/bin/env python3
"""branch_signature.py -- do all files of one dataset have the same branches? (STEP 23; PLAN section 9.3, D16)

treestream decides the branch set from the FIRST file of a chain: a branch only
in later files reads as 0, a branch missing from a later file stops the job.
This lists, per group of files (by default the CRAB request directory, three
levels above the file: <base>/<PD>/<request>/<stamp>/<NNNN>/<file>; the group
is named by its path under --base), the number of distinct branch sets and
every branch that is not in all files of the group.

    python3 tools/stage0/branch_signature.py --base <dir> [--glob '*/*/*/*/forgedNtuple_*.root']
                                             [--tree Events] [--group-level 3]

Output lines
    DS <group path under base> files=<n> unreadable=<n> branch_sets=<n> differing=<n>
       <branch>  in <k> of <n> files            (only when branch_sets > 1)
    UNREADABLE <file> <reason>                    (at most 20)
    SUMMARY groups=<n> with_differences=<n> files=<n> unreadable=<n>
Exit: 0 every file was read (differences are findings, see SUMMARY);
3 some files could not be read (cannot open, truncated -- ROOT had to
recover its keys --, or no such tree); 2 bad arguments or no file matched.
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


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--base", required=True, help="directory the pattern is relative to")
    ap.add_argument("--glob", default="*/*/*/*/forgedNtuple_*.root",
                    help="file pattern under --base (default: CRAB layout PD/request/stamp/NNNN/file)")
    ap.add_argument("--tree", default="Events")
    ap.add_argument("--group-level", type=int, default=3,
                    help="group files by the directory this many levels above the file "
                         "(1 = the file's own directory; default 3 = the CRAB request directory)")
    a = ap.parse_args(argv)
    if a.group_level < 1:
        print("ERROR --group-level must be >= 1")
        return 2

    files = sorted(glob.glob(os.path.join(a.base, a.glob)))
    print("BASE %s" % a.base)
    print("GLOB %s" % a.glob)
    print("TREE %s  group-level %d" % (a.tree, a.group_level))
    sys.stdout.flush()
    if not files:
        print("ERROR no file matched %s" % os.path.join(a.base, a.glob))
        return 2

    import ROOT
    ROOT.gROOT.SetBatch(True)

    groups = collections.OrderedDict()
    for f in files:
        # the group directory named by its whole path under --base (e.g. <PD>/<request>),
        # so equal request names in different primary datasets or campaigns stay apart
        g = f
        for _ in range(a.group_level):
            g = os.path.dirname(g)
        key = os.path.relpath(g, a.base)
        groups.setdefault(key, []).append(f)

    n_bad = 0
    n_diff = 0
    bad_list = []
    for key, flist in groups.items():
        sigs = collections.Counter()
        union, inter, bad = set(), None, 0
        for f in flist:
            why = None
            try:
                tf = ROOT.TFile.Open(f)
            except Exception as e:  # newer PyROOT raises OSError for an unreadable file
                tf, why = None, "cannot open (%s)" % first_line(e)
            if why is None and (not tf or tf.IsZombie()):
                why = "cannot open (zombie)"
            if why is None and tf.TestBit(ROOT.TFile.kRecovered):
                why = "truncated (ROOT recovered its keys; e.g. a failed stage-out)"
            t = tf.Get(a.tree) if why is None else None
            if why is None and not t:
                why = "no tree %s" % a.tree
            if why is not None:
                bad += 1
                bad_list.append((f, why))
                if tf:
                    tf.Close()
                continue
            names = frozenset(b.GetName() for b in t.GetListOfBranches())
            tf.Close()
            sigs[names] += 1
            union |= names
            inter = names if inter is None else (inter & names)
        diff = sorted(union - (inter or set()))
        good = len(flist) - bad
        n_bad += bad
        if len(sigs) > 1:
            n_diff += 1
        print("DS %-45s files=%d unreadable=%d branch_sets=%d differing=%d"
              % (key, len(flist), bad, len(sigs), len(diff)))
        for b in diff:
            k = sum(n for s, n in sigs.items() if b in s)
            print("   %-55s in %d of %d files" % (b, k, good))
        sys.stdout.flush()

    for f, why in bad_list[:20]:
        print("UNREADABLE %s %s" % (f, why))
    if len(bad_list) > 20:
        print("UNREADABLE ... and %d more" % (len(bad_list) - 20))
    print("SUMMARY groups=%d with_differences=%d files=%d unreadable=%d"
          % (len(groups), n_diff, len(files), n_bad))
    return 3 if n_bad else 0


if __name__ == "__main__":
    sys.exit(main())
