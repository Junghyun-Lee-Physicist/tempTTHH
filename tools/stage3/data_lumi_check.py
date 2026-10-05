#!/usr/bin/env python3
"""data_lumi_check.py -- which golden lumisections do our Data ntuples hold? (STEP 24; PLAN 9 Stage 3, D14)

KNU job (reads /pnfs). For each primary dataset (PD), from the LuminosityBlocks tree of every file in
its filelists (the tree lists every lumisection (LS) the CRAB job processed, before the skim):

    python3 tools/stage3/data_lumi_check.py --filelist-dir filelistTier3_2024
            --golden GoldenJson/2024_Summer24/Cert_Collisions2024_378981_386951_Golden.json
            [--pd JetMET0 JetMET1] [--run-range LO HI] [--json-out condor/lumi_2024] [--max-missing-lines 400]

  * a sample is a filelist_<PD>_Run<year><era>....txt; the PD is the part before '_Run'
  * DUPLICATE: an LS in two samples of the PD (2024 era I has two datasets per PD:
    <PD>_Run2024I-MINIv6NANOv15-v* and <PD>_Run2024I-MINIv6NANOv15_v2-v*). Its events would be counted
    twice: exit 1. An LS in two files of ONE sample is normal (a lumisection split over two input
    NanoAOD files) and only counted (SPLIT_LS)
  * per sample: files, unreadable, LS, run range
  * per PD against the golden JSON, inside the run range of the eras present (each era: from its first to
    its last processed run; --run-range LO HI instead): golden LS, processed golden LS, MISSING golden
    LS (jobs that failed or files left out: the data lumi is lower than the golden-JSON lumi by their
    lumi; printed as run:[first,last] ranges); golden runs outside those ranges are listed apart
    (GOLDEN_OUTSIDE: an era we did not produce -- 2024 era B -- or the first/last runs of an era that
    are missing entirely: look at the list); processed LS outside the golden JSON (harmless, masked)
  * --json-out DIR: <PD>_processed_golden.json and <PD>_missing_golden.json (CMS JSON format), for
    brilcalc on lxplus: brilcalc lumi -i <PD>_missing_golden.json --normtag <PHYSICS normtag> -u /fb
An unreadable file counts as missing (its LS are not in the processed set) and is listed.
Exit: 0 no duplicate and every file read; 1 a duplicate or an unreadable file; 2 bad arguments.
"""
from __future__ import print_function

import argparse
import collections
import json
import os
import re
import sys

SAMPLE_RE = re.compile(r"^filelist_(?P<pd>[A-Za-z0-9]+)_Run(?P<year>\d{4})(?P<era>[A-Z])(?P<rest>.*)\.txt$")


def first_line(e):
    lines = str(e).splitlines()
    return (lines[0] if lines else repr(e))[:120]


def open_root(ROOT, path, bad):
    try:
        tf = ROOT.TFile.Open(path)
    except Exception as e:
        bad.append((path, "cannot open (%s)" % first_line(e)))
        return None
    if not tf or tf.IsZombie():
        bad.append((path, "cannot open (zombie)"))
        return None
    if tf.TestBit(ROOT.TFile.kRecovered):
        bad.append((path, "truncated (ROOT recovered its keys)"))
        tf.Close()
        return None
    return tf


def file_ls(ROOT, path, bad):
    tf = open_root(ROOT, path, bad)
    if tf is None:
        return None
    try:
        lb = tf.Get("LuminosityBlocks")
        if not lb:
            raise RuntimeError("no LuminosityBlocks tree")
        lb.SetBranchStatus("*", 0)
        lb.SetBranchStatus("run", 1)
        lb.SetBranchStatus("luminosityBlock", 1)
        out = set()
        for e in lb:
            out.add((int(e.run), int(e.luminosityBlock)))
        return out
    except Exception as e:
        bad.append((path, "read error (%s)" % first_line(e)))
        return None
    finally:
        tf.Close()


def golden_set(path):
    with open(path) as f:
        j = json.load(f)
    s = set()
    for run, ranges in j.items():
        for lo, hi in ranges:
            for ls in range(int(lo), int(hi) + 1):
                s.add((int(run), ls))
    return s


def to_json(lsset):
    """{run: [[first, last], ...]} of a set of (run, LS)"""
    byrun = collections.defaultdict(list)
    for r, l in sorted(lsset):
        byrun[r].append(l)
    out = {}
    for r, lss in byrun.items():
        ranges = []
        for l in lss:
            if ranges and l == ranges[-1][1] + 1:
                ranges[-1][1] = l
            else:
                ranges.append([l, l])
        out[str(r)] = ranges
    return out


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--filelist-dir", required=True)
    ap.add_argument("--golden", required=True)
    ap.add_argument("--pd", nargs="*", help="only these PDs (default: every Data sample in the directory)")
    ap.add_argument("--run-range", nargs=2, type=int, help="expected golden runs [LO, HI] (default: per era present)")
    ap.add_argument("--json-out")
    ap.add_argument("--max-missing-lines", type=int, default=400)
    a = ap.parse_args(argv)
    if not os.path.isdir(a.filelist_dir) or not os.path.isfile(a.golden):
        print("ERROR no --filelist-dir %s or --golden %s" % (a.filelist_dir, a.golden))
        return 2
    samples = collections.defaultdict(list)    # pd -> [(sample, filelist)]
    for fn in sorted(os.listdir(a.filelist_dir)):
        m = SAMPLE_RE.match(fn)
        if not m or (a.pd and m.group("pd") not in a.pd):
            continue
        samples[m.group("pd")].append((fn[len("filelist_"):-len(".txt")], os.path.join(a.filelist_dir, fn)))
    if not samples:
        print("ERROR no Data filelist (filelist_<PD>_Run<year><era>...txt) in %s" % a.filelist_dir)
        return 2
    import ROOT
    ROOT.gROOT.SetBatch(True)
    gold = golden_set(a.golden)
    print("GOLDEN %s LS=%d runs=%d" % (a.golden, len(gold), len(set(r for r, _ in gold))))
    bad_all = []
    dup_all = 0
    for pd in sorted(samples):
        seen = {}                  # (run, ls) -> (sample, file index)
        processed = set()
        dups = collections.Counter()
        split = collections.Counter()
        dup_examples = []
        era_runs = collections.defaultdict(set)
        for sample, fl in samples[pd]:
            era = SAMPLE_RE.match("filelist_%s.txt" % sample).group("era")
            with open(fl) as f:
                files = [l.strip() for l in f if l.strip() and not l.startswith("#")]
            bad = []
            s_ls = set()
            for i, p in enumerate(files):
                ls = file_ls(ROOT, p, bad)
                if ls is None:
                    continue
                for k in ls:
                    if k not in seen:
                        seen[k] = (sample, i)
                    elif seen[k][0] == sample:
                        if seen[k][1] != i:
                            split[sample] += 1
                    else:
                        dups[(seen[k][0], sample)] += 1
                        if len(dup_examples) < 10:
                            dup_examples.append("%d:%d in %s:%d and %s:%d" % (k[0], k[1], seen[k][0], seen[k][1], sample, i))
                s_ls |= ls
                if (i + 1) % 500 == 0:
                    print("progress %s %d/%d" % (sample, i + 1, len(files)))
                    sys.stdout.flush()
            processed |= s_ls
            runs = sorted(set(r for r, _ in s_ls))
            era_runs[era] |= set(runs)
            print("SAMPLE %s files=%d unreadable=%d LS=%d runs=%d first=%s last=%s split_LS=%d"
                  % (sample, len(files), len(bad), len(s_ls), len(runs), runs[0] if runs else "-",
                     runs[-1] if runs else "-", split[sample]))
            for p, why in bad[:20]:
                print("UNREADABLE %s %s" % (p, why))
            bad_all += bad
        for (s1, s2), n in sorted(dups.items()):
            print("DUPLICATE %s LS=%d between %s and %s" % (pd, n, s1, s2))
        for d in dup_examples:
            print("   e.g. %s" % d)
        dup_all += sum(dups.values())
        if a.run_range:
            ranges = [tuple(a.run_range)]
        else:
            ranges = [(min(r), max(r)) for e, r in sorted(era_runs.items()) if r]
        print("RANGES %s %s" % (pd, " ".join("%d-%d" % x for x in ranges) or "-"))
        inside = lambda r: any(lo <= r <= hi for lo, hi in ranges)
        g_pd = set(k for k in gold if inside(k[0]))
        g_out = gold - g_pd
        have = processed & g_pd
        miss = g_pd - processed
        outside = processed - gold
        print("PD %s golden_LS=%d processed_golden_LS=%d missing_golden_LS=%d (%.3f %%) processed_outside_golden=%d"
              % (pd, len(g_pd), len(have), len(miss), 100.0 * len(miss) / max(1, len(g_pd)), len(outside)))
        out_runs = sorted(set(r for r, _ in g_out))
        print("GOLDEN_OUTSIDE %s runs=%d LS=%d runs: %s" % (pd, len(out_runs), len(g_out),
              " ".join(str(r) for r in out_runs[:60]) + (" ..." if len(out_runs) > 60 else "")))
        mj = to_json(miss)
        lines = ["%s:%s" % (r, json.dumps(v)) for r, v in sorted(mj.items(), key=lambda x: int(x[0]))]
        print("BEGIN MISSING %s runs=%d" % (pd, len(lines)))
        for l in lines[:a.max_missing_lines]:
            print(l)
        if len(lines) > a.max_missing_lines:
            print("... %d more runs (full list: --json-out)" % (len(lines) - a.max_missing_lines))
        print("END MISSING %s" % pd)
        if a.json_out:
            os.makedirs(a.json_out, exist_ok=True)
            for name, st in (("processed_golden", have), ("missing_golden", miss)):
                p = os.path.join(a.json_out, "%s_%s.json" % (pd, name))
                with open(p, "w") as f:
                    json.dump(to_json(st), f, sort_keys=True)
                print("WROTE %s" % p)
        sys.stdout.flush()
    ok = (dup_all == 0 and not bad_all)
    print("RESULT %s" % ("OK" if ok else "FAIL (%d duplicate LS, %d unreadable files)" % (dup_all, len(bad_all))))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
