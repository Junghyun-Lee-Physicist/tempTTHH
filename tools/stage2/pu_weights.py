#!/usr/bin/env python3
"""pu_weights.py -- 2024 pileup weights (STEP 24; PLAN 9.6 D15)

jsonpog has no POG/LUM for 2024 (2026-10-02), so the weights are made here from official inputs:
  data : the DQM pileup histograms of the golden JSON, Collisions24/PileUp/
         dataPileupHistogram-2024CDEFGHI_Golden-{69200,66000,72400}ub.root (nominal, down, up:
         minimum-bias cross section 69.2 mb and -/+4.6 %), copied into DerivedCorr/PU/2024_Summer24/input/
  MC   : Pileup_nTrueInt of the UNSKIMMED Summer24 ZZ pilot (ttHH2024_v15_had_MC_v1_pilot, 4.8 M events).
         Our 2024 production is skimmed (6j20); a jet-count skim keeps high-pileup events more often,
         so its Pileup_nTrueInt is not the MC pileup profile. All Summer24 samples share one premix
         pileup library, so one unskimmed sample gives the profile.

Two steps, so that only a record is made at KNU:
  mcprofile (KNU, reads ntuples) -- the MC profile in the binning of a data histogram, printed as
            PUMC_* lines; the runlog of this step is the record
      python3 tools/stage2/pu_weights.py mcprofile --glob '<pattern>' [--glob ...]
              --binning-from DerivedCorr/PU/2024_Summer24/input/dataPileupHistogram-2024CDEFGHI_Golden-69200ub.root
  weights   (anywhere) -- correctionlib JSON (schema 2) from the three data histograms and a mcprofile
            record; the same shape as jsonpog puWeights: inputs (NumTrueInteractions real, weights
            string nominal/up/down), binning with flow clamp
      python3 tools/stage2/pu_weights.py weights --data-dir DerivedCorr/PU/2024_Summer24/input
              --mc-record runlogs/run_knu_pu_mcprofile_<UTC>.log --out DerivedCorr/PU/2024_Summer24/puWeights_2024.json

Weights: w_i = d_i / m_i with d, m the in-range contents normalized to 1; w_i = 0 where m_i = 0 (no MC
event can be there; the data fraction in such bins is printed as lost_data). Exit: 0 ok; 1 a check
failed (binning mismatch, MC record unreadable, correctionlib read-back differs); 2 bad arguments;
3 (mcprofile) some input files could not be read (the profile is still printed from the others).
"""
from __future__ import print_function

import argparse
import glob
import hashlib
import json
import math
import os
import re
import sys

DQM_NAMES = {"nominal": "dataPileupHistogram-2024CDEFGHI_Golden-69200ub.root",
             "down": "dataPileupHistogram-2024CDEFGHI_Golden-66000ub.root",
             "up": "dataPileupHistogram-2024CDEFGHI_Golden-72400ub.root"}
CORR_NAME = "Collisions24_goldenJSON"


def fmt(x):
    return "%.17g" % x


def edges_md5(edges):
    return hashlib.md5(",".join(fmt(e) for e in edges).encode()).hexdigest()


def import_root():
    import ROOT
    ROOT.gROOT.SetBatch(True)
    return ROOT


def read_th1(ROOT, path, name=None):
    """(edges, in-range contents, underflow, overflow, hist name) of the TH1 in a file
    (the one called name, else 'pileup', else the first TH1)."""
    tf = ROOT.TFile.Open(path)
    if not tf or tf.IsZombie():
        raise IOError("cannot open %s" % path)
    cands = []
    for k in tf.GetListOfKeys():
        o = k.ReadObj()
        if o.InheritsFrom("TH1") and not o.InheritsFrom("TH2"):
            cands.append(o)
    if not cands:
        raise IOError("no TH1 in %s" % path)
    pick = None
    for want in ([name] if name else []) + ["pileup"]:
        pick = next((h for h in cands if h.GetName() == want), None)
        if pick:
            break
    h = pick or cands[0]
    ax = h.GetXaxis()
    n = h.GetNbinsX()
    edges = [ax.GetBinLowEdge(i) for i in range(1, n + 1)] + [ax.GetBinUpEdge(n)]
    cont = [h.GetBinContent(i) for i in range(1, n + 1)]
    res = (edges, cont, h.GetBinContent(0), h.GetBinContent(n + 1), h.GetName())
    tf.Close()
    return res


def check_ntuple(ROOT, path, tree):
    try:
        if os.path.getsize(path) == 0:
            return "size 0"
    except OSError as e:
        return "stat failed (%s)" % e
    try:
        tf = ROOT.TFile.Open(path)
    except Exception as e:
        return "cannot open (%s)" % (str(e).splitlines() or ["?"])[0][:100]
    if not tf or tf.IsZombie():
        return "cannot open (zombie)"
    if tf.TestBit(ROOT.TFile.kRecovered):
        tf.Close()
        return "truncated (ROOT recovered its keys)"
    t = tf.Get(tree)
    why = None if t else "no tree %s" % tree
    tf.Close()
    return why


def cmd_mcprofile(a):
    ROOT = import_root()
    try:
        edges, _, _, _, hname = read_th1(ROOT, a.binning_from)
    except IOError as e:
        print("ERROR binning: %s" % e)
        return 2
    files = []
    for g in a.glob:
        files += [f for f in glob.glob(g) if "/failed/" not in f and "/log/" not in f]
    files = sorted(set(files))
    if not files:
        print("ERROR no file matched %s" % " ".join(a.glob))
        return 2
    good, bad = [], []
    for f in files:
        why = check_ntuple(ROOT, f, a.tree)
        (bad.append((f, why)) if why else good.append(f))
    for f, why in bad:
        print("BADFILE %s %s" % (f, why))
    if not good:
        print("ERROR no readable file")
        return 2
    chain = ROOT.TChain(a.tree)
    for f in good:
        chain.Add(f)
    import array
    ROOT.gROOT.cd()
    h = ROOT.TH1D("pumc", "", len(edges) - 1, array.array("d", edges))
    h.SetDirectory(ROOT.gROOT)
    ntot = chain.GetEntries()
    nent = chain.Draw("%s>>pumc" % a.branch, "", "goff")
    n = h.GetNbinsX()
    if nent <= 0 or nent != ntot:
        print("ERROR Draw of %s gave %d rows for %d entries of the chain (no such branch, or a file stopped the chain)"
              % (a.branch, nent, ntot))
        return 1
    counts = [h.GetBinContent(i) for i in range(1, n + 1)]
    print("PUMC_FILES good=%d bad=%d events=%d branch=%s tree=%s" % (len(good), len(bad), nent, a.branch, a.tree))
    uni = all(abs((edges[i + 1] - edges[i]) - (edges[1] - edges[0])) < 1e-9 for i in range(len(edges) - 1))
    print("PUMC_BINNING nbins=%d first=%s last=%s uniform=%s edges_md5=%s source=%s:%s"
          % (n, fmt(edges[0]), fmt(edges[-1]), "yes" if uni else "no", edges_md5(edges), a.binning_from, hname))
    print("PUMC_STATS entries=%d underflow=%s overflow=%s mean=%.6f rms=%.6f"
          % (h.GetEntries(), fmt(h.GetBinContent(0)), fmt(h.GetBinContent(n + 1)), h.GetMean(), h.GetRMS()))
    print("PUMC_EDGES " + ",".join(fmt(e) for e in edges))
    print("PUMC_COUNTS " + ",".join(fmt(c) for c in counts))
    print("RESULT %s" % ("OK" if not bad else "OK_WITH_BAD_FILES (%d unreadable, left out)" % len(bad)))
    return 3 if bad else 0


def read_mc_record(path):
    edges = counts = None
    meta = {}
    with open(path, encoding="utf-8", errors="replace") as f:
        for l in f:
            if l.startswith("PUMC_EDGES "):
                edges = [float(x) for x in l.split(None, 1)[1].strip().split(",")]
            elif l.startswith("PUMC_COUNTS "):
                counts = [float(x) for x in l.split(None, 1)[1].strip().split(",")]
            elif l.startswith(("PUMC_FILES ", "PUMC_STATS ")):
                for kv in l.split()[1:]:
                    if "=" in kv:
                        k, v = kv.split("=", 1)
                        meta[k] = v
    if edges is None or counts is None or len(edges) != len(counts) + 1:
        raise ValueError("no PUMC_EDGES/PUMC_COUNTS pair in %s" % path)
    return edges, counts, meta


def make_weights(dnorm, mnorm):
    return [d / m if m > 0 else 0.0 for d, m in zip(dnorm, mnorm)]


def normalize(c):
    s = float(sum(c))
    if s <= 0:
        raise ValueError("histogram with no in-range content")
    return [x / s for x in c]


def correction_json(edges, weights, description):
    content = []
    for key in ("nominal", "up", "down"):
        content.append({"key": key, "value": {"nodetype": "binning", "input": "NumTrueInteractions",
                                              "edges": [float(e) for e in edges],
                                              "content": [float(w) for w in weights[key]], "flow": "clamp"}})
    return {"schema_version": 2,
            "description": description,
            "corrections": [{"name": CORR_NAME,
                             "description": description,
                             "version": 1,
                             "inputs": [{"name": "NumTrueInteractions", "type": "real",
                                         "description": "Pileup_nTrueInt"},
                                        {"name": "weights", "type": "string",
                                         "description": "nominal, up, down (minimum-bias 69.2 mb, +/-4.6 %)"}],
                             "output": {"name": "weight", "type": "real", "description": "data/MC pileup weight"},
                             "data": {"nodetype": "category", "input": "weights", "content": content}}]}


def cmd_weights(a):
    ROOT = import_root()
    paths = {k: (getattr(a, "data_" + k) or os.path.join(a.data_dir, DQM_NAMES[k])) for k in ("nominal", "up", "down")}
    data = {}
    for k, p in paths.items():
        try:
            data[k] = read_th1(ROOT, p)
        except IOError as e:
            print("ERROR data %s: %s" % (k, e))
            return 2
        with open(p, "rb") as f:
            md5 = hashlib.md5(f.read()).hexdigest()
        e, c, u, o, n = data[k]
        tot = sum(c) + u + o
        print("DATA %-7s %s hist=%s nbins=%d md5=%s underflow_frac=%.3g overflow_frac=%.3g"
              % (k, os.path.basename(p), n, len(c), md5, u / tot if tot else 0, o / tot if tot else 0))
    try:
        medges, mcounts, meta = read_mc_record(a.mc_record)
    except (OSError, ValueError) as e:
        print("ERROR mc record: %s" % e)
        return 1
    print("MC record=%s events=%s files=%s mean=%s" % (os.path.basename(a.mc_record), meta.get("events", "?"),
                                                      meta.get("good", "?"), meta.get("mean", "?")))
    bad = []
    for k in ("nominal", "up", "down"):
        if [fmt(x) for x in data[k][0]] != [fmt(x) for x in medges]:
            bad.append("binning of data %s differs from the MC record" % k)
    if bad:
        print("RESULT FAIL (" + "; ".join(bad) + ")")
        return 1
    try:
        m = normalize(mcounts)
        dn = {k: normalize(data[k][1]) for k in ("nominal", "up", "down")}
    except ValueError as e:
        print("RESULT FAIL (%s)" % e)
        return 1
    weights = {}
    for k in ("nominal", "up", "down"):
        d = dn[k]
        w = make_weights(d, m)
        weights[k] = w
        lost = sum(di for di, mi in zip(d, m) if mi <= 0)
        closure = sum(wi * mi for wi, mi in zip(w, m))
        dmean = sum(0.5 * (medges[i] + medges[i + 1]) * d[i] for i in range(len(d)))
        print("PUW %-7s data_mean=%.4f max_w=%.4g closure_sum_mw=%.9f lost_data=%.9f"
              % (k, dmean, max(w), closure, lost))
    mmean = sum(0.5 * (medges[i] + medges[i + 1]) * m[i] for i in range(len(m)))
    print("PUW mc_mean=%.4f" % mmean)
    desc = ("2024 C-I pileup weights, tempTTHH tools/stage2/pu_weights.py (STEP 24, PLAN 9.6 D15). "
            "data: DQM Collisions24/PileUp %s (nominal), %s (down), %s (up); MC: Pileup_nTrueInt of the "
            "unskimmed Summer24 ZZ pilot, %s events (record %s). w = data/MC, both normalized in range; "
            "0 where the MC bin is empty." % (DQM_NAMES["nominal"], DQM_NAMES["down"], DQM_NAMES["up"],
                                              meta.get("events", "?"), os.path.basename(a.mc_record)))
    js = correction_json(medges, weights, desc)
    text = json.dumps(js, indent=1) + "\n"
    with open(a.out, "w") as f:
        f.write(text)
    print("WROTE %s md5=%s" % (a.out, hashlib.md5(text.encode()).hexdigest()))
    try:
        import correctionlib
    except ImportError:
        print("NOTE correctionlib not importable here: read-back check skipped")
        print("RESULT OK")
        return 0
    cs = correctionlib.CorrectionSet.from_file(a.out)
    c = cs[CORR_NAME]
    worst = 0.0
    for k in ("nominal", "up", "down"):
        for i in range(len(medges) - 1):
            x = 0.5 * (medges[i] + medges[i + 1])
            worst = max(worst, abs(c.evaluate(x, k) - weights[k][i]))
    over = c.evaluate(medges[-1] + 10.0, "nominal")
    # correctionlib's JSON parser may round a decimal 1 ulp away from Python's repr: allow 1e-12
    print("READBACK correctionlib %s max_abs_diff=%.3g clamp_above=%.6g (last bin %.6g)"
          % (correctionlib.__version__, worst, over, weights["nominal"][-1]))
    ok = worst <= 1e-12 * max(1.0, max(max(w) for w in weights.values())) and \
        abs(over - weights["nominal"][-1]) <= 1e-12 * max(1.0, abs(weights["nominal"][-1]))
    print("RESULT %s" % ("OK" if ok else "FAIL (correctionlib read-back differs)"))
    return 0 if ok else 1


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = ap.add_subparsers(dest="cmd")
    p1 = sub.add_parser("mcprofile")
    p1.add_argument("--glob", action="append", required=True)
    p1.add_argument("--binning-from", required=True)
    p1.add_argument("--tree", default="Events")
    p1.add_argument("--branch", default="Pileup_nTrueInt")
    p2 = sub.add_parser("weights")
    p2.add_argument("--data-dir", default="DerivedCorr/PU/2024_Summer24/input")
    p2.add_argument("--data-nominal", default="")
    p2.add_argument("--data-up", default="")
    p2.add_argument("--data-down", default="")
    p2.add_argument("--mc-record", required=True)
    p2.add_argument("--out", required=True)
    a = ap.parse_args(argv)
    if a.cmd == "mcprofile":
        return cmd_mcprofile(a)
    if a.cmd == "weights":
        return cmd_weights(a)
    ap.print_help()
    return 2


if __name__ == "__main__":
    sys.exit(main())
