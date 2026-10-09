#!/usr/bin/env python3
"""synth_trigskim.py -- synthetic btagtrig skims for TriggerStudy with a KNOWN Data/MC efficiency ratio (STEP 26 L)

    python3 synth_trigskim.py --year 2024|2017 --xsec-db <samples_YYYY.json> -o <dir>
                              [--n-mc 40000] [--n-data 80000] [--seed 7] [--prescan-out <fake prescan json>]
                              [--variant normal|badB|noIsoMu24|noRef|tiny --sample NAME]

Writes <dir>/<sample>.root (TDirectory Tree, TTree Tree: the branches TriggerStudy/src/NtupleReader.cc reads, with the
leaf types of the analyzer's tree) for the three ttbar MC samples and the Data of the year (2024: every Muon0/1 key of
the xsec_db; 2017: SingleMuon_Run2017B..F, era B with the CSV-path _B slots), and <dir>/expected.json: per sample the
counts TriggerStudy must print ([TrigStudy] line: entries, muon CR, reference, hadronic OR) and the injected ratio.

The model (never physics): every event passes the skim invariants of EventLooper (>= 6 jets, 6th jet pT > 40,
HT >= 500, |eta of the 6th jet| <= 2.4, Data on golden LS); (HT, 6th jet pT, nb category) is drawn bin-uniformly over
the TriggerStudy binning (Config.hh) where kinematically possible (HT >= 6 x the 6th jet pT), so every such bin has
data. The hadronic OR fires in MC with eff(HT, pT) and in Data with r(nb) x eff(HT, pT): the SF TriggerStudy derives
must come out as r(nb) within its errors. 85 % of the events have 1 muon, 5 % an electron (muon CR: 1 muon and 0 electron), 92 % fire the
reference (2024 HLT_IsoMu24, 2017 HLT_IsoMu27). 2024 skims have the _B slots 0 (the analyzer's convention).
--variant writes the one sample --sample (2000 events): badB sets a _B slot in a few events, noIsoMu24 leaves that
branch out, noRef never fires the reference (TriggerStudy must stop on each); tiny: 30 events (DeriveSF must find no
bin to measure).
--prescan-out writes a prescan summary with runs.genEventSumw for the MC samples and its meta.year (the 2024 one is
KNU-local).
"""
import argparse
import json
import math
import os
import re
import sys

import numpy as np

HT_BINS = [500, 550, 600, 700, 800, 1000, 1500, 2500]       # TriggerStudy/include/Config.hh InitBinning()
PT_BINS = [40, 45, 50, 60, 70, 120, 200]
NB_CATS = [(0, 2), (3, 3), (4, 6)]                          # nB0to2, nB3, nB4p (drawn up to 6)
RATIO = {"2024": [0.97, 0.92, 0.86], "2017": [0.95, 0.90, 0.85]}   # injected Data/MC efficiency ratio per nb category
MC = ["TTbar_DiLep", "TTbar_Hadronic", "TTbar_SemiLep"]
DATA17 = ["SingleMuon_Run2017B", "SingleMuon_Run2017C", "SingleMuon_Run2017D", "SingleMuon_Run2017E",
          "SingleMuon_Run2017F"]


def eff_mc(ht, pt):
    """the MC efficiency of the hadronic OR: a turn-on in HT, a milder one in the 6th jet pT (0.35 .. 0.95)"""
    return 0.35 + 0.60 / (1.0 + math.exp(-(ht - 700.0) / 80.0)) * (1.0 - 0.30 * math.exp(-(pt - 40.0) / 15.0))


def draw_kin(rng):
    """(HT, 6th jet pT, nb, category) bin-uniformly over the binning, where HT >= 6 x pT6 is possible"""
    while True:
        i = rng.integers(len(HT_BINS) - 1)
        j = rng.integers(len(PT_BINS) - 1)
        c = rng.integers(len(NB_CATS))
        pt = rng.uniform(PT_BINS[j], PT_BINS[j + 1])
        pt = max(pt, 40.01)
        lo = max(HT_BINS[i], 6.05 * pt)
        hi = HT_BINS[i + 1]
        if lo >= hi - 1e-3:
            continue
        ht = rng.uniform(lo, hi)
        nb = int(rng.integers(NB_CATS[c][0], NB_CATS[c][1] + 1))
        return ht, pt, nb, int(c)


def jets(rng, ht, pt6):
    """jet pT (sorted, the 6th = pt6, sum = HT) and eta: 5 leading jets share HT - pt6 - extras, each >= pt6"""
    extras = [rng.uniform(30.0, pt6) for _ in range(int(rng.integers(0, 3)))]
    if ht - pt6 - sum(extras) < 5.0 * pt6 + 1.0:
        extras = []
    rest = ht - pt6 - sum(extras) - 5.0 * pt6
    w = rng.dirichlet(np.ones(5))
    lead = sorted((pt6 + rest * x for x in w), reverse=True)
    pts = lead + [pt6] + sorted(extras, reverse=True)
    etas = [float(rng.uniform(-2.3, 2.3)) for _ in pts]
    return pts, etas


def write_sample(path, year, kind, era, n, seed, variant="normal"):
    import ROOT
    rng = np.random.default_rng(seed)
    f = ROOT.TFile(path, "RECREATE")
    d = f.mkdir("Tree")
    d.cd()
    t = ROOT.TTree("Tree", "synthetic btagtrig skim (TriggerStudy test, STEP 26 L)")
    bools = ["passTrigger_HLT_IsoMu27", "passTrigger_HLT_PFHT1050", "passTrigger_6J1T_B", "passTrigger_6J1T_CDEF",
             "passTrigger_6J2T_B", "passTrigger_6J2T_CDEF", "passTrigger_4J3T_B", "passTrigger_4J3T_CDEF",
             "failGoldenJson", "passMETFilters", "passHadTrig"]
    if year == "2024" and variant != "noIsoMu24":
        bools.insert(1, "passTrigger_HLT_IsoMu24")
    ints = ["nMuons", "nElecs", "nJets", "nbJets"]
    floats = ["HT", "MET_pt", "evtWeight", "genWeight", "PUWeight", "L1PrefiringWeight"]
    buf = {}
    for b in bools:
        buf[b] = np.zeros(1, dtype=np.bool_)
        t.Branch(b, buf[b], b + "/O")
    for b in ints:
        buf[b] = np.zeros(1, dtype=np.int32)
        t.Branch(b, buf[b], b + "/I")
    for b in floats:
        buf[b] = np.zeros(1, dtype=np.float32)
        t.Branch(b, buf[b], b + "/F")
    vpt, veta = ROOT.std.vector("float")(), ROOT.std.vector("float")()
    t.Branch("jetPt", vpt)
    t.Branch("jetEta", veta)
    isData = (kind == "data")
    useB = (year == "2017" and isData and era == "B")
    ratio = RATIO[year]
    ref = "passTrigger_HLT_IsoMu24" if year == "2024" else "passTrigger_HLT_IsoMu27"
    cnt = {"entries": n, "muonCR": 0, "ref": 0, "had": 0}
    for k in range(n):
        ht, pt6, nb, c = draw_kin(rng)
        pts, etas = jets(rng, ht, pt6)
        nmu = 1 if rng.random() < 0.85 else int(rng.choice([0, 2]))
        nel = 1 if rng.random() < 0.05 else 0
        isRef = (rng.random() < 0.92) and variant != "noRef"
        e = eff_mc(ht, pt6) * (ratio[c] if isData else 1.0)
        passHad = rng.random() < e
        for b in bools:
            buf[b][0] = False
        if passHad:
            pfht = (ht > 1050.0 and rng.random() < 0.7)
            j1, j2, j3 = (rng.random() < 0.5), (rng.random() < 0.5), (rng.random() < 0.5)
            if not (pfht or j1 or j2 or j3):
                j1 = True
            buf["passTrigger_HLT_PFHT1050"][0] = pfht
            sl = "_B" if useB else "_CDEF"
            buf["passTrigger_6J1T" + sl][0] = j1
            buf["passTrigger_6J2T" + sl][0] = j2
            buf["passTrigger_4J3T" + sl][0] = j3
        if variant == "badB" and k % 97 == 5:
            buf["passTrigger_6J1T_B"][0] = True           # against the 2024 convention: TriggerStudy must stop
        if ref in buf:
            buf[ref][0] = isRef
        buf["passMETFilters"][0] = True
        buf["passHadTrig"][0] = passHad
        buf["nMuons"][0], buf["nElecs"][0] = nmu, nel
        buf["nJets"][0], buf["nbJets"][0] = len(pts), nb
        buf["HT"][0], buf["MET_pt"][0] = ht, rng.uniform(0.0, 100.0)
        buf["evtWeight"][0] = 1.0
        buf["genWeight"][0] = 1.0 if (isData or rng.random() < 0.97) else -1.0
        buf["PUWeight"][0] = 1.0 if isData else rng.uniform(0.7, 1.3)
        buf["L1PrefiringWeight"][0] = 1.0
        vpt.clear()
        veta.clear()
        for p, h in zip(pts, etas):
            vpt.push_back(p)
            veta.push_back(h)
        t.Fill()
        if nmu == 1 and nel == 0:
            cnt["muonCR"] += 1
            if isRef:
                cnt["ref"] += 1
                cnt["had"] += int(passHad)
    t.Write()
    f.Close()
    return cnt


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--year", required=True, choices=["2017", "2024"])
    ap.add_argument("--xsec-db", required=True)
    ap.add_argument("-o", "--out", required=True)
    ap.add_argument("--n-mc", type=int, default=40000, help="events per MC sample")
    ap.add_argument("--n-data", type=int, default=80000, help="Data events in total (split over the Data samples)")
    ap.add_argument("--seed", type=int, default=7)
    ap.add_argument("--prescan-out", default="")
    ap.add_argument("--variant", default="normal", choices=["normal", "badB", "noIsoMu24", "noRef", "tiny"])
    ap.add_argument("--sample", default="", help="--variant: the one sample to write")
    a = ap.parse_args()
    db = json.load(open(a.xsec_db))
    if a.year == "2024":
        data = sorted(k for k in db if k.startswith(("Muon0_Run2024", "Muon1_Run2024")))
    else:
        data = [k for k in DATA17 if k in db] or DATA17
    if not data:
        sys.exit("[FATAL] no Data sample of %s in %s" % (a.year, a.xsec_db))
    os.makedirs(a.out, exist_ok=True)
    if a.variant != "normal":
        if a.sample not in data and a.sample not in MC:
            sys.exit("[FATAL] --variant %s needs --sample <one of the samples of %s>" % (a.variant, a.year))
        kind = "mc" if a.sample in MC else "data"
        m = re.search(r"_Run\d{4}([A-Z])(?:$|-)", a.sample)
        cnt = write_sample(os.path.join(a.out, a.sample + ".root"), a.year, kind, m.group(1) if m else "",
                           30 if a.variant == "tiny" else 2000, a.seed, a.variant)
        print("WROTE %s %s %s" % (a.variant, a.sample, json.dumps(cnt)))
        return 0
    exp = {"year": a.year, "ratio": RATIO[a.year], "samples": {}}
    for i, s in enumerate(MC):
        exp["samples"][s] = dict(write_sample(os.path.join(a.out, s + ".root"), a.year, "mc", "", a.n_mc,
                                              a.seed * 1000 + i), kind="MC")
    nd = max(a.n_data // len(data), 200)
    for i, s in enumerate(data):
        m = re.search(r"_Run\d{4}([A-Z])(?:$|-)", s)
        exp["samples"][s] = dict(write_sample(os.path.join(a.out, s + ".root"), a.year, "data", m.group(1) if m else "",
                                              nd, a.seed * 1000 + 100 + i), kind="Data")
    with open(os.path.join(a.out, "expected.json"), "w") as fh:
        json.dump(exp, fh, indent=1)
    if a.prescan_out:
        pre = {"samples": {s: {"runs": {"genEventSumw": 1.0e9 * (1 + i)}} for i, s in enumerate(MC)},
               "meta": {"year": a.year, "input_base": "(synthetic)/AnalyzerOutput_prescan_%s" % a.year,
                        "note": "FAKE prescan summary (test/trigger_study/synth_trigskim.py)"}}
        with open(a.prescan_out, "w") as fh:
            json.dump(pre, fh, indent=1)
    print("WROTE %d MC + %d Data samples in %s (expected.json)" % (len(MC), len(data), a.out))
    return 0


if __name__ == "__main__":
    sys.exit(main())
