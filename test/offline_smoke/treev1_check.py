#!/usr/bin/env python3
"""treev1_check.py -- [STEP 27 O] the Tree v1 branches of one analyzer output, recomputed and compared (offline smoke)

    python3 treev1_check.py --out <analyzer output.root> --input <its input file> --year 2017|2024 --kind mc|data
                            [--pu-json <file>] [--btv <btagging.json.gz of 2017>] [--pdf on|off]

Recomputes, independently of include/TreeVars.h and include/GenMatch.h (numpy, written from the definitions in their
headers and in AN-2022/122 §6.2):
  * from the tree's own jet vectors (jetPt, jetEta, jetPhi, jetMass, bTagScore): the WP bins, the light/loose counts,
    b/light HT, average masses, max-pT triplet masses, the ΔR/Δη statistics (jj, bb, bj), Fox-Wolfram H0-4 / R1-4
    (jets, b jets), centralities, and the χ² pairing with the AN's jet choice (HH, ZH, ZZ; m_H 125, m_Z 91.2);
  * from the input file (the event with the same event number): metPhi, the jet-quark labels (the hard-process quarks
    and the greedy ΔR < 0.4 matching), PU up/down (correctionlib on Pileup_nTrueInt), the L1 prefiring up/down (2017),
    LHEScaleWeight / PSWeight (and LHEPdfWeight with --pdf on), and for 2017 MC the 16 deepJet-shape source weights
    (BTV's recipe: a c jet takes only cferr1/2, a b or light jet every source but cferr1/2).
Data: the weights are 1, the vectors empty, the labels 0 / -1.
Output: one line per check "TV <name> PASS|FAIL <detail>", then "TV-RESULT <pass> <fail>"; exit 0 when all pass.
Values are synthetic; the checks are about the code doing what its definition says, never physics.
"""
import argparse
import math
import sys

import numpy as np

WP = {"2017": (0.0532, 0.3040, 0.7476), "2024": (0.0246, 0.1272, 0.4648)}
SRC = ["hf", "lf", "hfstats1", "hfstats2", "lfstats1", "lfstats2", "cferr1", "cferr2"]
MH, MZ = 125.0, 91.2


def p4(pt, eta, phi, m):
    px, py, pz = pt * math.cos(phi), pt * math.sin(phi), pt * math.sinh(eta)
    e = math.sqrt(px * px + py * py + pz * pz + m * m)
    return np.array([px, py, pz, e])


def mass(v):
    m2 = v[3] ** 2 - v[0] ** 2 - v[1] ** 2 - v[2] ** 2
    return math.sqrt(m2) if m2 > 0 else -math.sqrt(-m2)


def ptof(v):
    return math.hypot(v[0], v[1])


def pmag(v):
    return math.sqrt(v[0] ** 2 + v[1] ** 2 + v[2] ** 2)


def dphi(a, b):
    d = abs(a - b) % (2 * math.pi)
    return 2 * math.pi - d if d > math.pi else d


def eta_of(v):
    p = pmag(v)
    return 0.5 * math.log((p + v[2]) / (p - v[2])) if p > abs(v[2]) else math.copysign(1e10, v[2])


def phi_of(v):
    return math.atan2(v[1], v[0])


def pairstats(pairs):
    if not pairs:
        return [-1.0] * 7
    rs, es, best = [], [], None
    for a, b in pairs:
        r = math.hypot(dphi(phi_of(a), phi_of(b)), eta_of(a) - eta_of(b))
        de = abs(eta_of(a) - eta_of(b))
        rs.append(r)
        es.append(de)
        if best is None or r < best[0]:
            s = a + b
            best = (r, mass(s), ptof(s))
    return [sum(rs) / len(rs), min(rs), max(rs), sum(es) / len(es), max(es), best[1], best[2]]


def foxwolfram(vs):
    if len(vs) < 2:
        return [-1.0] * 5, [-1.0] * 5
    s = sum(v[3] for v in vs) ** 2
    h = [0.0] * 5
    for a in vs:
        for b in vs:
            pa, pb = pmag(a), pmag(b)
            c = 1.0 if a is b else max(-1.0, min(1.0, float(np.dot(a[:3], b[:3])) / (pa * pb)))
            leg = [1.0, c, 0.5 * (3 * c * c - 1), 0.5 * (5 * c ** 3 - 3 * c), 0.125 * (35 * c ** 4 - 30 * c * c + 3)]
            for l in range(5):
                h[l] += pa * pb * leg[l] / s
    return h, [-1.0] + [h[l] / h[0] for l in range(1, 5)]


def chi2term(a, b, target):
    s = a + b
    return (mass(s) - target) ** 2 / math.sqrt((ptof(a) + ptof(b)) / 2.0 * 0.2)


def best_pairing(cands, ts):
    """smallest χ² over every 4-subset of the jets in cands and every pairing; ts = (t1, t2) (asymmetric: both ways)"""
    best = None
    n = len(cands)
    for i in range(n):
        for j in range(i + 1, n):
            for k in range(j + 1, n):
                for l in range(k + 1, n):
                    q = [cands[i], cands[j], cands[k], cands[l]]
                    for (x, y), (z, w) in (((0, 1), (2, 3)), ((0, 2), (1, 3)), ((0, 3), (1, 2))):
                        for p1, p2 in (((x, y), (z, w)), ((z, w), (x, y))):
                            c = chi2term(q[p1[0]], q[p1[1]], ts[0]) + chi2term(q[p2[0]], q[p2[1]], ts[1])
                            if best is None or c < best[0]:
                                best = (c, mass(q[p1[0]] + q[p1[1]]), mass(q[p2[0]] + q[p2[1]]),
                                        ptof(q[p1[0]] + q[p1[1]]), ptof(q[p2[0]] + q[p2[1]]))
    return best


def chi2an(vs, score, wl, wm):
    M = [v for v, s in zip(vs, score) if s >= wm]
    Lo = [v for v, s in zip(vs, score) if wl <= s < wm]
    Li = [v for v, s in zip(vs, score) if s < wl]
    out = {"hh": None, "zh": None, "zz": None}
    if len(M) < 2:
        return out
    sets = []
    if len(M) >= 4:
        sets = [M]
    else:
        need = 4 - len(M)
        import itertools
        if len(Lo) >= need:
            sets = [M + list(c) for c in itertools.combinations(Lo, need)]
        else:
            sets = [M + Lo + list(c) for c in itertools.combinations(Li, need - len(Lo))]
    for s in sets:
        for key, ts in (("hh", (MH, MH)), ("zz", (MZ, MZ)), ("zh", (MZ, MH))):
            b = best_pairing(s, ts)
            if b is not None and (out[key] is None or b[0] < out[key][0]):
                out[key] = b
    return out


def hard_partons(pdg, mom, flags, eta, phi):
    out = []
    n = len(pdg)
    for i in range(n):
        a = abs(pdg[i])
        if a < 1 or a > 5 or not (flags[i] & 256) or not (flags[i] & 8192):
            continue
        m = mom[i]
        while 0 <= m < n and abs(pdg[m]) == a:
            m = mom[m]
        if not (0 <= m < n):
            continue
        am = abs(pdg[m])
        lab = 1 if (am == 25 and a == 5) else 2 if (am == 6 and a == 5) else 3 if am == 24 else 4 if am == 23 else 0
        if lab == 0:
            continue
        f = m
        while 0 <= mom[f] < n and pdg[mom[f]] == pdg[f]:
            f = mom[f]
        # [STEP 27 P] the top: t->b = the mother's first copy; W->q = the first copy of the W's parent if it is a top
        top = -1
        if lab == 2:
            top = f
        elif lab == 3 and 0 <= mom[f] < n and abs(pdg[mom[f]]) == 6:
            top = mom[f]
            while 0 <= mom[top] < n and pdg[mom[top]] == pdg[top]:
                top = mom[top]
        out.append((lab, f, eta[i], phi[i], top))
    return out


def match(jets, partons):
    cand = []
    for j, (je, jp) in enumerate(jets):
        for p, (lab, f, pe, pp, _top) in enumerate(partons):
            dr = math.hypot(dphi(jp, pp), je - pe)
            if dr < 0.4:
                cand.append((dr, j, p))
    cand.sort()
    lab, mo, tp = [0] * len(jets), [-1] * len(jets), [-1] * len(jets)
    uj, up = set(), set()
    for dr, j, p in cand:
        if j in uj or p in up:
            continue
        uj.add(j)
        up.add(p)
        lab[j], mo[j], tp[j] = partons[p][0], partons[p][1], partons[p][4]
    return lab, mo, tp


def close(a, b, rel=2e-4, absol=2e-4):
    return abs(a - b) <= max(absol, rel * max(abs(a), abs(b)))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", required=True)
    ap.add_argument("--input", required=True)
    ap.add_argument("--year", required=True, choices=["2017", "2024"])
    ap.add_argument("--kind", required=True, choices=["mc", "data"])
    ap.add_argument("--pu-json")
    ap.add_argument("--btv")
    ap.add_argument("--pdf", default="off")
    a = ap.parse_args()
    import ROOT
    ROOT.gROOT.SetBatch(True)
    mc = a.kind == "mc"
    wl, wm, wt = WP[a.year]
    res = {"pass": 0, "fail": 0}

    def check(name, ok, detail=""):
        res["pass" if ok else "fail"] += 1
        print("TV %s %s %s" % (name, "PASS" if ok else "FAIL", detail))

    f = ROOT.TFile.Open(a.out)
    t = f.Get("Tree/Tree")
    fin = ROOT.TFile.Open(a.input)
    tin = fin.Get("Events")
    evidx = {}
    for i in range(tin.GetEntries()):
        tin.GetEntry(i)
        evidx[int(tin.event)] = i
    names = {b.GetName() for b in t.GetListOfBranches()}
    need = ["jetPhi", "jetMass", "jetBTagWP", "jetGenMatch", "jetGenMotherIdx", "jetGenTopIdx", "lightjetNumber",
            "nLooseJets", "bjetHT",
            "lightjetHT", "jetAverageMass", "bjetAverageMass", "lightjetAverageMass", "bjetAverageMassSqr", "maxPTmassjjj",
            "maxPTmassjbb", "centrality", "bjetCentrality", "chi2Higgs", "invMassH1", "invMassH2", "PTH1", "PTH2",
            "chi2HiggsZ", "invMassHiggsZ1", "invMassHiggsZ2", "chi2Z", "invMassZ1", "invMassZ2", "metPhi", "invMassHadW",
            "PUWeight_up",
            "PUWeight_down", "L1PrefiringWeight_up", "L1PrefiringWeight_down", "LHEScaleWeight", "PSWeight"]
    need += ["%s%s" % (q, t_) for t_ in ("jj", "bb", "bj") for q in ("averageDeltaR", "minDeltaR", "maxDeltaR",
             "averageDeltaEta", "maxDeltaEta", "minDeltaRMass", "minDeltaRpT")]
    need += ["H%d" % l for l in range(5)] + ["bjetH%d" % l for l in range(5)] + ["R%d" % l for l in range(1, 5)] + \
            ["bjetR%d" % l for l in range(1, 5)]
    shape = ["bTagWeight_%s_%s" % (s, d) for s in SRC for d in ("up", "down")]
    missing = [n for n in need if n not in names]
    check("branches: every Tree v1 branch is booked (%d)" % len(need), not missing, str(missing[:5]))
    if a.year == "2017":
        check("2017: the 16 shape-source branches are booked, the fixed-WP comparison ones are not",
              all(n in names for n in shape) and "bTagWeight_cAsB" not in names)
    else:
        check("2024: the fixed-WP comparison branches are booked, the shape-source ones are not",
              "bTagWeight_oldRule" in names and "bTagWeight_cAsB" in names and not any(n in names for n in shape))
    check("LHEPdfWeight booked only with --tree-pdf on (%s)" % a.pdf, ("LHEPdfWeight" in names) == (a.pdf == "on"))
    if missing:
        print("TV-RESULT %d %d" % (res["pass"], res["fail"]))
        return 1
    pu = None
    if mc and a.pu_json:
        import correctionlib
        cs = correctionlib.CorrectionSet.from_file(a.pu_json)
        pu = cs[list(cs.keys())[0]]
    btv = None
    if mc and a.btv and a.year == "2017":
        import correctionlib
        btv = correctionlib.CorrectionSet.from_file(a.btv)["deepJet_shape"]
    bad = {}
    nev = njets_lab = ntop_link = 0
    labcount = [0] * 5
    for i in range(t.GetEntries()):
        t.GetEntry(i)
        nev += 1
        n = t.nJets

        def flag(k, ok):
            if not ok:
                bad[k] = bad.get(k, 0) + 1
        pts, etas, phis, ms, sc = list(t.jetPt), list(t.jetEta), list(t.jetPhi), list(t.jetMass), list(t.bTagScore)
        flag("vector lengths = nJets", all(len(x) == n for x in (pts, etas, phis, ms, sc, t.jetBTagWP, t.jetGenMatch,
                                                                  t.jetGenMotherIdx, t.jetGenTopIdx)))
        vs = [p4(pt, e, ph, m) for pt, e, ph, m in zip(pts, etas, phis, ms)]
        isb = [s >= wm for s in sc]
        B = [v for v, b in zip(vs, isb) if b]
        NB = [v for v, b in zip(vs, isb) if not b]
        LI = [v for v, s in zip(vs, sc) if s < wl]
        flag("jetBTagWP bins", list(t.jetBTagWP) == [0 if s < wl else 1 if s < wm else 2 if s < wt else 3 for s in sc])
        flag("lightjetNumber, nLooseJets", t.lightjetNumber == len(LI) and t.nLooseJets == sum(s >= wl for s in sc))
        flag("bjetHT, lightjetHT", close(t.bjetHT, sum(ptof(v) for v in B)) and close(t.lightjetHT, sum(ptof(v) for v in LI)))
        am = lambda L: sum(mass(v) for v in L) / len(L) if L else -1.0
        am2 = lambda L: sum(mass(v) ** 2 for v in L) / len(L) if L else -1.0
        flag("average masses (jets, b, light, b squared)", close(t.jetAverageMass, am(vs)) and close(t.bjetAverageMass, am(B))
             and close(t.lightjetAverageMass, am(LI)) and close(t.bjetAverageMassSqr, am2(B), 5e-4))
        best = (-1.0, -1.0)
        for x in range(n):
            for y in range(x + 1, n):
                for z in range(y + 1, n):
                    s = vs[x] + vs[y] + vs[z]
                    if ptof(s) > best[0]:
                        best = (ptof(s), mass(s))
        bestb = (-1.0, -1.0)
        for x in range(n):
            for y in range(x + 1, n):
                if not (isb[x] and isb[y]):
                    continue
                for z in range(n):
                    if z in (x, y):
                        continue
                    s = vs[x] + vs[y] + vs[z]
                    if ptof(s) > bestb[0]:
                        bestb = (ptof(s), mass(s))
        flag("maxPTmassjjj, maxPTmassjbb", close(t.maxPTmassjjj, best[1], 5e-4) and close(t.maxPTmassjbb, bestb[1], 5e-4))
        for tag, pairs in (("jj", [(vs[x], vs[y]) for x in range(n) for y in range(x + 1, n)]),
                           ("bb", [(B[x], B[y]) for x in range(len(B)) for y in range(x + 1, len(B))]),
                           ("bj", [(b, o) for b in B for o in NB])):
            exp = pairstats(pairs)
            got = [getattr(t, q + tag) for q in ("averageDeltaR", "minDeltaR", "maxDeltaR", "averageDeltaEta",
                                                 "maxDeltaEta", "minDeltaRMass", "minDeltaRpT")]
            flag("ΔR/Δη statistics " + tag, all(close(g, e, 5e-4, 5e-4) for g, e in zip(got, exp)))
        hj, rj = foxwolfram(vs)
        hb, rb = foxwolfram(B)
        flag("Fox-Wolfram jets", all(close(getattr(t, "H%d" % l), hj[l], 5e-4) for l in range(5)) and
             all(close(getattr(t, "R%d" % l), rj[l], 5e-4) for l in range(1, 5)))
        flag("Fox-Wolfram b jets", all(close(getattr(t, "bjetH%d" % l), hb[l], 5e-4) for l in range(5)) and
             all(close(getattr(t, "bjetR%d" % l), rb[l], 5e-4) for l in range(1, 5)))
        cand = LI if len(LI) >= 2 else vs          # m_qq: light jets (score < L) when two, else all (closestMassPair)
        mw = -1.0
        for x in range(len(cand)):
            for y in range(x + 1, len(cand)):
                m_ = mass(cand[x] + cand[y])
                if mw < 0 or abs(m_ - 80.377) < abs(mw - 80.377):
                    mw = m_
        flag("invMassHadW (m_qq) = recomputed", close(t.invMassHadW, mw, 5e-4))
        cen = lambda L: sum(ptof(v) for v in L) / sum(pmag(v) for v in L) if L else -1.0
        flag("centrality (jets, b jets)", close(t.centrality, cen(vs)) and close(t.bjetCentrality, cen(B)))
        c = chi2an(vs, sc, wl, wm)
        if c["hh"] is None:
            flag("chi2 undefined -> -1", t.chi2Higgs == -1 and t.chi2HiggsZ == -1 and t.chi2Z == -1)
        else:
            hh, zh, zz = c["hh"], c["zh"], c["zz"]
            m1, m2, p1, p2 = (hh[1], hh[2], hh[3], hh[4]) if abs(hh[1] - MH) <= abs(hh[2] - MH) else (hh[2], hh[1], hh[4], hh[3])
            z1, z2 = (zz[1], zz[2]) if abs(zz[1] - MZ) <= abs(zz[2] - MZ) else (zz[2], zz[1])
            ok = (close(t.chi2Higgs, hh[0], 1e-3, 1e-3) and close(t.invMassH1, m1, 1e-3) and close(t.invMassH2, m2, 1e-3)
                  and close(t.PTH1, p1, 1e-3) and close(t.PTH2, p2, 1e-3) and close(t.chi2Z, zz[0], 1e-3, 1e-3)
                  and close(t.invMassZ1, z1, 1e-3) and close(t.invMassZ2, z2, 1e-3) and close(t.chi2HiggsZ, zh[0], 1e-3, 1e-3)
                  and close(t.invMassHiggsZ2, zh[1], 1e-3) and close(t.invMassHiggsZ1, zh[2], 1e-3))
            flag("chi2 pairing (HH, ZZ, ZH; masses ordered)", ok)
        j = evidx.get(int(t.eventNumber))
        if j is None:
            flag("input event found", False)
            continue
        tin.GetEntry(j)
        metphi = tin.PuppiMET_phi if a.year == "2024" else tin.MET_phi
        flag("metPhi = the input's", close(t.metPhi, metphi, 1e-6, 1e-6))
        if mc:
            ng = tin.nGenPart
            pdg = [int(tin.GenPart_pdgId[k]) for k in range(ng)]
            mom = [int(tin.GenPart_genPartIdxMother[k]) for k in range(ng)]
            fl = [int(tin.GenPart_statusFlags[k]) for k in range(ng)]
            ge = [float(tin.GenPart_eta[k]) for k in range(ng)]
            gph = [float(tin.GenPart_phi[k]) for k in range(ng)]
            lab, mo, tp = match(list(zip(etas, phis)), hard_partons(pdg, mom, fl, ge, gph))
            flag("jet-quark labels and mothers = recomputed", list(t.jetGenMatch) == lab and list(t.jetGenMotherIdx) == mo)
            flag("jet top indices = recomputed (t->b and t->W->q share the top)", list(t.jetGenTopIdx) == tp)
            ntop_link += sum(1 for x in tp if x >= 0)
            for x in lab:
                labcount[x] += 1
            njets_lab += sum(1 for x in lab if x)
            if pu is not None:
                nt = float(tin.Pileup_nTrueInt)
                flag("PU up/down = correctionlib", close(t.PUWeight_up, pu.evaluate(nt, "up"), 1e-5) and
                     close(t.PUWeight_down, pu.evaluate(nt, "down"), 1e-5))
            if a.year == "2017":
                flag("L1 prefiring up/down = the input's", close(t.L1PrefiringWeight_up, tin.L1PreFiringWeight_Up, 1e-6) and
                     close(t.L1PrefiringWeight_down, tin.L1PreFiringWeight_Dn, 1e-6))
            else:
                flag("L1 prefiring up/down = 1 (2024)", t.L1PrefiringWeight_up == 1 and t.L1PrefiringWeight_down == 1)
            flag("LHEScaleWeight, PSWeight = the input's vectors",
                 list(t.LHEScaleWeight) == [tin.LHEScaleWeight[k] for k in range(tin.nLHEScaleWeight)] and
                 list(t.PSWeight) == [tin.PSWeight[k] for k in range(tin.nPSWeight)])
            if a.pdf == "on":
                flag("LHEPdfWeight = the input's vector",
                     list(t.LHEPdfWeight) == [tin.LHEPdfWeight[k] for k in range(tin.nLHEPdfWeight)])
            if btv is not None:
                for si, src in enumerate(SRC):
                    for d in ("up", "down"):
                        w = 1.0
                        for pt, e, s, hf in zip(pts, etas, sc, t.hadFlavs):
                            f_ = hf if hf in (4, 5) else 0
                            key = "%s_%s" % (d, src) if ((f_ == 4) == (si >= 6)) else "central"
                            w *= btv.evaluate(key, f_, min(abs(e), 2.4999), min(max(pt, 20.0), 1000.0), min(max(s, 0.0), 1.0))
                        flag("shape source weights = BTV recipe (correctionlib)",
                             close(getattr(t, "bTagWeight_%s_%s" % (src, d)), w, 2e-5))
        else:
            # [STEP 27 P review] also the L1 up/down, the top index, LHEPdfWeight (when booked) and the shape-source weights
            shp = [n_ for n_ in names if n_.startswith("bTagWeight_") and (n_.endswith("_up") or n_.endswith("_down"))
                   and n_ not in ("bTagWeight_up", "bTagWeight_down")]
            flag("Data: labels 0 / -1 / -1, weights 1, vectors empty",
                 all(x == 0 for x in t.jetGenMatch) and all(x == -1 for x in t.jetGenMotherIdx)
                 and all(x == -1 for x in t.jetGenTopIdx) and t.PUWeight_up == 1
                 and t.PUWeight_down == 1 and t.L1PrefiringWeight_up == 1 and t.L1PrefiringWeight_down == 1
                 and len(t.LHEScaleWeight) == 0 and len(t.PSWeight) == 0
                 and ("LHEPdfWeight" not in names or len(t.LHEPdfWeight) == 0)
                 and all(getattr(t, n_) == 1 for n_ in shp))
    check("events read", nev > 0, "(%d)" % nev)
    for k in sorted(bad):
        check(k, False, "(%d of %d events)" % (bad[k], nev))
    check("every per-event comparison agrees", not bad, "(%d events)" % nev)
    if mc:
        check("the matching found H->b, t->b and W->q jets (labels %s)" % labcount, labcount[1] > 0 and labcount[2] > 0
              and labcount[3] > 0)
        check("jets linked to a top (t->b, t->W->q): %d" % ntop_link, ntop_link > 0)
    print("TV-RESULT %d %d" % (res["pass"], res["fail"]))
    return 0 if res["fail"] == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
