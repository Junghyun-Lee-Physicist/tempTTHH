#!/usr/bin/env python3
"""synth_nano.py -- small NanoAOD-like files with plausible values, for the offline smoke test (STEP 24)

    python3 synth_nano.py --year 2017|2024 --kind mc|data [--era X] [--golden <golden json>]
                          [--run-range LO HI] [--n 400] [--seed 1] [--drop BRANCH ...] [--b-low F] -o out.root

Writes only the branches the analyzer reads (ttHHanalyzer_unified.cc requireBranches_ and the object
fields), each with the leaf type it has in the real ntuples of that year (2017: NanoAOD v9 of
ttHH2017UL_fullNano_v20; 2024: v15 of ttHH2024_v15_had_*; from the KNU branch inventories, STEP 24),
and the HLT paths of that kind of file (2017 era B Data: the CSV names; 2017 MC and C-F: the PF names;
2024: all six). Data take (run, LS) from the golden JSON, 10 % of the events outside it. Trees: Events,
LuminosityBlocks (the (run, LS) of the events), Runs (genEventSumw for MC). --drop leaves a branch out
(to test the required-branch check). Values are random but in physical ranges; never physics.
b-tag scores: b jets above the medium WP, the others below 0.08 -- unless [STEP 25 K] --b-low F > 0: then a b jet
is, with probability F/2 each, between the loose and the medium WP or below the loose WP (so the b efficiencies
are inside (0, 1), as the fixed-WP weight needs; the default F = 0 draws no extra random numbers: the files of
the other checks stay what they were).
[STEP 27 O] Tree v1 inputs: MET_phi / PuppiMET_phi, L1PreFiringWeight_Up/_Dn (2017 MC), LHEScaleWeight (9),
PSWeight (4), LHEPdfWeight (103) for MC, and a gen record whose b quarks (t, tbar, H) sit on the b jets and whose
four W quarks sit on other jets (so the jet-parton matching has something to find). Their values come from a SECOND
random stream (seed + 7919), so every value drawn before STEP 27 is the same as it was.
"""
import argparse
import json
import math
import sys

import numpy as np

HLT = {
    "2017": ["HLT_PFHT1050", "HLT_IsoMu27",
             "HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0", "HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5",
             "HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2",
             "HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07", "HLT_PFHT430_SixJet40_BTagCSV_p080",
             "HLT_PFHT380_SixJet32_DoubleBTagCSV_p075"],
    "2024": ["HLT_PFHT1050", "HLT_IsoMu24",
             "HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3",
             "HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5",
             "HLT_PFHT450_SixPFJet36_PNetBTag0p35", "HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50"],
}
FLAGS = {
    "2017": ["Flag_goodVertices", "Flag_globalSuperTightHalo2016Filter", "Flag_HBHENoiseFilter", "Flag_HBHENoiseIsoFilter",
             "Flag_EcalDeadCellTriggerPrimitiveFilter", "Flag_BadPFMuonFilter", "Flag_BadPFMuonDzFilter", "Flag_eeBadScFilter",
             "Flag_ecalBadCalibFilter", "Flag_hfNoisyHitsFilter"],
    "2024": ["Flag_goodVertices", "Flag_globalSuperTightHalo2016Filter", "Flag_EcalDeadCellTriggerPrimitiveFilter",
             "Flag_BadPFMuonFilter", "Flag_BadPFMuonDzFilter", "Flag_hfNoisyHitsFilter", "Flag_eeBadScFilter",
             "Flag_ecalBadCalibFilter"],
}
JET = {"common": ["Jet_pt", "Jet_eta", "Jet_phi", "Jet_mass", "Jet_rawFactor", "Jet_area", "Jet_btagDeepFlavB"],
       "2017": ["Jet_jetId", "Jet_puId"],
       "2024": ["Jet_btagUParTAK4B", "Jet_chHEF", "Jet_neHEF", "Jet_chEmEF", "Jet_neEmEF", "Jet_muEF",
                "Jet_chMultiplicity", "Jet_neMultiplicity"],
       "mc": ["Jet_hadronFlavour", "Jet_partonFlavour", "Jet_genJetIdx"]}
MUON = ["Muon_pt", "Muon_eta", "Muon_phi", "Muon_tightId", "Muon_pfRelIso04_all", "Muon_charge", "Muon_miniPFRelIso_all",
        "Muon_isPFcand"]
ELE = {"common": ["Electron_pt", "Electron_eta", "Electron_phi", "Electron_deltaEtaSC", "Electron_charge",
                  "Electron_miniPFRelIso_all", "Electron_pfRelIso03_all"],
       "2017": ["Electron_mvaFall17V2Iso_WP90"], "2024": ["Electron_mvaIso_WP90"]}
GENJ = ["GenJet_pt", "GenJet_eta", "GenJet_phi", "GenJet_mass", "GenJet_hadronFlavour"]
GENP = ["GenPart_pt", "GenPart_eta", "GenPart_phi", "GenPart_mass", "GenPart_pdgId", "GenPart_statusFlags",
        "GenPart_genPartIdxMother"]
SCAL = {"common": ["run", "luminosityBlock", "event", "PV_npvsGood"],
        "2017": ["fixedGridRhoFastjetAll", "MET_pt", "MET_phi", "PuppiMET_pt", "PuppiMET_phi"],
        "2024": ["Rho_fixedGridRhoFastjetAll", "PuppiMET_pt", "PuppiMET_phi"],
        "mc": ["genWeight", "Pileup_nTrueInt", "genTtbarId"],
        "mc2017": ["L1PreFiringWeight_Nom", "L1PreFiringWeight_Up", "L1PreFiringWeight_Dn"]}
# [STEP 27 O] the theory-weight vectors of MC (NanoAOD: a float array with its own counter) and their lengths here
THEORY = {"LHEScaleWeight": ("nLHEScaleWeight", 9), "PSWeight": ("nPSWeight", 4), "LHEPdfWeight": ("nLHEPdfWeight", 103)}

# leaf types (the KNU inventories); anything not listed in YEAR_TYPES is in COMMON_TYPES
COMMON_TYPES = {"run": "UInt_t", "luminosityBlock": "UInt_t", "event": "ULong64_t", "genTtbarId": "Int_t",
                "Muon_charge": "Int_t", "Electron_charge": "Int_t", "GenPart_pdgId": "Int_t",
                "Muon_tightId": "Bool_t", "Muon_isPFcand": "Bool_t", "Electron_mvaFall17V2Iso_WP90": "Bool_t",
                "Electron_mvaIso_WP90": "Bool_t"}
YEAR_TYPES = {
    "2017": {"counter": "UInt_t", "Jet_jetId": "Int_t", "Jet_puId": "Int_t", "Jet_hadronFlavour": "Int_t",
             "Jet_partonFlavour": "Int_t", "Jet_genJetIdx": "Int_t", "GenJet_hadronFlavour": "UChar_t",
             "GenPart_statusFlags": "Int_t", "GenPart_genPartIdxMother": "Int_t", "PV_npvsGood": "Int_t"},
    "2024": {"counter": "Int_t", "Jet_hadronFlavour": "UChar_t", "Jet_partonFlavour": "Short_t", "Jet_genJetIdx": "Short_t",
             "Jet_chMultiplicity": "UChar_t", "Jet_neMultiplicity": "UChar_t", "GenJet_hadronFlavour": "UChar_t",
             "GenPart_statusFlags": "UShort_t", "GenPart_genPartIdxMother": "Short_t", "PV_npvsGood": "UChar_t"},
}
ERA_B_ONLY = {"HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07", "HLT_PFHT430_SixJet40_BTagCSV_p080",
              "HLT_PFHT380_SixJet32_DoubleBTagCSV_p075"}
NOT_ERA_B = {"HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0", "HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5",
             "HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2"}


def leaf_type(year, name):
    """(ROOT type name, counter or None) of a branch"""
    yt = YEAR_TYPES[year]
    counter = None
    if name in THEORY:
        return "Float_t", THEORY[name][0]
    if "_" in name and not name.startswith(("HLT_", "Flag_", "Pileup_", "PV_", "L1PreFiring", "Rho_", "MET_",
                                             "PuppiMET_", "fixedGrid", "gen")):
        counter = "n" + name.split("_")[0]
    t = yt.get(name) or COMMON_TYPES.get(name)
    if t is None:
        t = "Bool_t" if name.startswith(("HLT_", "Flag_")) else "Float_t"
    return t, counter


CODE = {"Float_t": ("F", np.float32), "Double_t": ("D", np.float64), "Int_t": ("I", np.int32), "UInt_t": ("i", np.uint32),
        "Short_t": ("S", np.int16), "UShort_t": ("s", np.uint16), "Char_t": ("B", np.int8), "UChar_t": ("b", np.uint8),
        "Bool_t": ("O", np.bool_), "Long64_t": ("L", np.int64), "ULong64_t": ("l", np.uint64)}
MAXN = 64


def golden_pairs(path):
    with open(path) as f:
        j = json.load(f)
    pairs = []
    for run, ranges in j.items():
        for lo, hi in ranges:
            pairs.append((int(run), int(lo), int(hi)))
    return pairs


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument("--year", required=True, choices=["2017", "2024"])
    ap.add_argument("--kind", required=True, choices=["mc", "data"])
    ap.add_argument("--era", default="")
    ap.add_argument("--golden")
    ap.add_argument("--run-range", nargs=2, type=int, help="data: only runs in [lo, hi]")
    ap.add_argument("--n", type=int, default=400)
    ap.add_argument("--seed", type=int, default=1)
    ap.add_argument("--drop", nargs="*", default=[])
    ap.add_argument("--b-low", type=float, default=0.0, help="[STEP 25 K] fraction of b jets below the medium WP")
    ap.add_argument("-o", required=True)
    a = ap.parse_args(argv)

    import ROOT
    ROOT.gROOT.SetBatch(True)

    mc = a.kind == "mc"
    y = a.year
    if y == "2017" and not mc and not a.era:
        sys.exit("2017 data needs --era (era B has other HLT names)")

    def ttype(name):
        if y == "2017" and name in (NOT_ERA_B if (not mc and a.era == "B") else ERA_B_ONLY):
            return None, None             # not in the menu of this kind of 2017 file
        return leaf_type(y, name)
    names = list(SCAL["common"]) + SCAL[y] + (SCAL["mc"] if mc else []) + (SCAL["mc2017"] if mc and y == "2017" else [])
    names += [h for h in HLT[y]] + FLAGS[y]
    names += JET["common"] + JET[y] + (JET["mc"] if mc else []) + MUON + ELE["common"] + ELE[y]
    if mc:
        names += GENJ + GENP + list(THEORY)
    rng = np.random.default_rng(a.seed)
    rng2 = np.random.default_rng(a.seed + 7919)   # [STEP 27 O] the second stream (the first keeps its draws)

    out = ROOT.TFile(a.o, "RECREATE")
    tree = ROOT.TTree("Events", "Events")
    bufs = {}
    counters = {}
    skipped = []
    for n in names:
        if n in a.drop:
            skipped.append(n + "(dropped)")
            continue
        typ, cnt = ttype(n)
        if typ is None:
            skipped.append(n)
            continue
        code, dt = CODE[typ]
        if cnt:
            if cnt not in counters:
                ccode, cdt = CODE[YEAR_TYPES[y]["counter"]]
                counters[cnt] = np.zeros(1, dtype=cdt)
                tree.Branch(cnt, counters[cnt], "%s/%s" % (cnt, ccode))
            bufs[n] = np.zeros(max(MAXN, THEORY.get(n, (None, 0))[1]), dtype=dt)
            tree.Branch(n, bufs[n], "%s[%s]/%s" % (n, cnt, code))
        else:
            bufs[n] = np.zeros(1, dtype=dt)
            tree.Branch(n, bufs[n], "%s/%s" % (n, code))

    def setv(n, vals):
        if n in bufs:
            arr = bufs[n]
            if len(arr) == 1 and not hasattr(vals, "__len__"):
                arr[0] = vals
            else:
                arr[:len(vals)] = vals

    def setc(c, k):
        if c in counters:
            counters[c][0] = k

    pairs = golden_pairs(a.golden) if (not mc and a.golden) else []
    if pairs and a.run_range:
        pairs = [p for p in pairs if a.run_range[0] <= p[0] <= a.run_range[1]]
    if not mc and not pairs:
        sys.exit("data needs --golden (and runs in --run-range)")

    sumw = 0.0
    sumw2 = 0.0
    lumis = set()
    hi_wp = 0.3 if y == "2017" else 0.2          # above the medium WP of DeepJet 2017 (0.3040) / UParT 2024 (0.1272)
    for ev in range(a.n):
        # ---- event-level
        if mc:
            run, lumi = 1, 1 + ev // 100
        else:
            r, lo, hi = pairs[rng.integers(len(pairs))]
            run, lumi = r, int(rng.integers(lo, hi + 1))
            if rng.random() < 0.10:
                lumi = hi + 1000 + ev          # outside the golden JSON
        setv("run", run)
        setv("luminosityBlock", lumi)
        lumis.add((run, lumi))
        setv("event", 1000 + ev)
        setv("PV_npvsGood", int(rng.integers(5, 60)))
        rho = float(rng.uniform(8, 40))
        setv("fixedGridRhoFastjetAll", rho)
        setv("Rho_fixedGridRhoFastjetAll", rho)
        setv("MET_pt", float(rng.exponential(40)))
        setv("PuppiMET_pt", float(rng.exponential(35)))
        for h in HLT[y]:
            setv(h, bool(rng.random() < (0.25 if h == "HLT_PFHT1050" else 0.55)))
        for fl in FLAGS[y]:
            setv(fl, bool(rng.random() > 0.02))
        # ---- jets
        nj = int(rng.integers(4, 12))
        pt = np.sort(rng.uniform(0.8, 1.2, nj) * (30 + 260 * np.exp(-0.28 * np.arange(nj))))[::-1]
        eta = rng.uniform(-2.7, 2.7, nj)
        if rng.random() < 0.3:
            eta[-1] = rng.uniform(2.7, 4.8) * (1 if rng.random() < 0.5 else -1)   # a forward jet
        if rng.random() < 0.05:                                                 # a jet in the 2024 fake veto-map cell
            eta[0], phi0 = 1.3, 2.3
        else:
            phi0 = None
        phi = rng.uniform(-math.pi, math.pi, nj)
        if phi0 is not None:
            phi[0] = phi0
        nb = int(rng.integers(1, 6))
        isb = np.zeros(nj, dtype=bool)
        isb[rng.choice(nj, size=min(nb, nj), replace=False)] = True
        disc = np.where(isb, rng.uniform(hi_wp + 0.05, 1.0, nj), rng.uniform(0.0, 0.08, nj))
        if a.b_low > 0:                              # [STEP 25 K] (only then: the default draws stay the same)
            wl, wm = {"2017": (0.0532, 0.3040), "2024": (0.0246, 0.1272)}[y]   # loose / medium WP of the year
            u = rng.random(nj)
            disc = np.where(isb & (u < a.b_low / 2), rng.uniform(wl, wm, nj),
                            np.where(isb & (u >= a.b_low / 2) & (u < a.b_low), rng.uniform(0.0, wl, nj), disc))
        setc("nJet", nj)
        setv("Jet_pt", pt)
        setv("Jet_eta", eta)
        setv("Jet_phi", phi)
        setv("Jet_mass", pt * rng.uniform(0.05, 0.18, nj))
        setv("Jet_rawFactor", rng.uniform(0.02, 0.15, nj))
        setv("Jet_area", rng.uniform(0.42, 0.58, nj))
        setv("Jet_btagDeepFlavB", disc)
        setv("Jet_btagUParTAK4B", disc)
        setv("Jet_jetId", np.where(rng.random(nj) < 0.92, 6, 2))
        setv("Jet_puId", np.where(rng.random(nj) < 0.9, 7, 0))
        chHEF = rng.uniform(0.15, 0.75, nj)
        neHEF = np.where(rng.random(nj) < 0.05, 0.995, rng.uniform(0.02, 0.3, nj))   # 5 % fail tight
        chEmEF = np.where(rng.random(nj) < 0.03, 0.85, rng.uniform(0.0, 0.2, nj))   # 3 % fail lepton veto
        setv("Jet_chHEF", chHEF)
        setv("Jet_neHEF", neHEF)
        setv("Jet_chEmEF", chEmEF)
        setv("Jet_neEmEF", rng.uniform(0.0, 0.3, nj))
        setv("Jet_muEF", rng.uniform(0.0, 0.05, nj))
        setv("Jet_chMultiplicity", rng.integers(1, 30, nj))
        setv("Jet_neMultiplicity", rng.integers(0, 20, nj))
        # ---- leptons
        nm = int(rng.choice([0, 0, 0, 0, 1, 1, 2]))
        setc("nMuon", nm)
        setv("Muon_pt", rng.uniform(5, 80, nm))
        setv("Muon_eta", rng.uniform(-2.5, 2.5, nm))
        setv("Muon_phi", rng.uniform(-math.pi, math.pi, nm))
        setv("Muon_tightId", rng.random(nm) < 0.8)
        setv("Muon_isPFcand", rng.random(nm) < 0.95)
        setv("Muon_pfRelIso04_all", rng.exponential(0.12, nm))
        setv("Muon_miniPFRelIso_all", rng.exponential(0.1, nm))
        setv("Muon_charge", rng.choice([-1, 1], nm))
        ne = int(rng.choice([0, 0, 0, 0, 0, 1, 1]))
        setc("nElectron", ne)
        setv("Electron_pt", rng.uniform(5, 80, ne))
        setv("Electron_eta", rng.uniform(-2.6, 2.6, ne))
        setv("Electron_phi", rng.uniform(-math.pi, math.pi, ne))
        setv("Electron_deltaEtaSC", rng.uniform(-0.05, 0.05, ne))
        setv("Electron_charge", rng.choice([-1, 1], ne))
        setv("Electron_miniPFRelIso_all", rng.exponential(0.1, ne))
        setv("Electron_pfRelIso03_all", rng.exponential(0.1, ne))
        setv("Electron_mvaFall17V2Iso_WP90", rng.random(ne) < 0.8)
        setv("Electron_mvaIso_WP90", rng.random(ne) < 0.8)
        # ---- MC truth
        if mc:
            w = float(rng.uniform(0.9, 1.1)) * (-1 if rng.random() < 0.05 else 1)
            sumw += w
            sumw2 += w * w
            setv("genWeight", w)
            setv("Pileup_nTrueInt", float(rng.uniform(10, 70)))
            setv("genTtbarId", int(rng.choice([0, 0, 51, 52, 53, 54, 55, 41, 42, 43, 44, 45])))
            setv("L1PreFiringWeight_Nom", float(rng.uniform(0.95, 1.0)))
            gidx = np.where(rng.random(nj) < 0.85, np.arange(nj), -1)
            setv("Jet_genJetIdx", gidx)
            hf = np.where(isb, 5, np.where(rng.random(nj) < 0.15, 4, 0))
            setv("Jet_hadronFlavour", hf)
            setv("Jet_partonFlavour", np.where(hf == 5, 5, np.where(hf == 4, 4, 21)))
            setc("nGenJet", nj)
            setv("GenJet_pt", pt * (1 - 0.08) * rng.uniform(0.9, 1.1, nj))
            setv("GenJet_eta", eta + rng.normal(0, 0.02, nj))
            setv("GenJet_phi", phi + rng.normal(0, 0.02, nj))
            setv("GenJet_mass", pt * 0.1)
            setv("GenJet_hadronFlavour", hf)
            # t, tbar, H; b from each top and two b from H (statusFlags bit 8 = isHardProcess)
            pdg = [6, -6, 25, 5, -5, 5, -5, 24, -24]
            mom = [-1, -1, -1, 0, 1, 2, 2, 0, 1]
            ng = len(pdg)
            setc("nGenPart", ng)
            setv("GenPart_pdgId", pdg)
            setv("GenPart_genPartIdxMother", mom)
            setv("GenPart_statusFlags", [256 + 8192] * ng)
            setv("GenPart_pt", rng.uniform(20, 300, ng))
            setv("GenPart_eta", rng.uniform(-2.5, 2.5, ng))
            setv("GenPart_phi", rng.uniform(-math.pi, math.pi, ng))
            setv("GenPart_mass", [172.5, 172.5, 125.0, 4.8, 4.8, 4.8, 4.8, 80.4, 80.4])
            # [STEP 27 O] (second stream only) the four W quarks, and the quarks put on jets: b from t -> 1st b jet,
            #   b from tbar -> 4th, the H b quarks -> 2nd and 3rd; the W quarks -> the first non-b jets
            gpt = np.concatenate([bufs["GenPart_pt"][:ng].astype(float), rng2.uniform(20, 200, 4)])
            geta = np.concatenate([bufs["GenPart_eta"][:ng].astype(float), rng2.uniform(-2.5, 2.5, 4)])
            gphi = np.concatenate([bufs["GenPart_phi"][:ng].astype(float), rng2.uniform(-math.pi, math.pi, 4)])
            bj = [i for i in range(nj) if isb[i]]
            oj = [i for i in range(nj) if not isb[i]]
            for q, k in ((3, 0), (5, 1), (6, 2), (4, 3)):
                if k < len(bj):
                    geta[q] = eta[bj[k]] + rng2.normal(0, 0.03)
                    gphi[q] = phi[bj[k]] + rng2.normal(0, 0.03)
            for q, k in ((9, 0), (10, 1), (11, 2), (12, 3)):
                if k < len(oj):
                    geta[q] = eta[oj[k]] + rng2.normal(0, 0.03)
                    gphi[q] = phi[oj[k]] + rng2.normal(0, 0.03)
            gphi = (gphi + math.pi) % (2 * math.pi) - math.pi
            setc("nGenPart", ng + 4)
            setv("GenPart_pdgId", pdg + [2, -1, 1, -2])
            setv("GenPart_genPartIdxMother", mom + [7, 7, 8, 8])
            setv("GenPart_statusFlags", [256 + 8192] * (ng + 4))
            setv("GenPart_pt", gpt)
            setv("GenPart_eta", geta)
            setv("GenPart_phi", gphi)
            setv("GenPart_mass", [172.5, 172.5, 125.0, 4.8, 4.8, 4.8, 4.8, 80.4, 80.4, 0.0, 0.0, 0.0, 0.0])
            setv("L1PreFiringWeight_Up", float(bufs["L1PreFiringWeight_Nom"][0]) * 1.01 if "L1PreFiringWeight_Nom" in bufs else 1.0)
            setv("L1PreFiringWeight_Dn", float(bufs["L1PreFiringWeight_Nom"][0]) * 0.99 if "L1PreFiringWeight_Nom" in bufs else 1.0)
            for tn, (tc, tl) in THEORY.items():
                setc(tc, tl)
                setv(tn, rng2.uniform(0.8, 1.2, tl))
        setv("MET_phi", float(rng2.uniform(-math.pi, math.pi)))
        setv("PuppiMET_phi", float(rng2.uniform(-math.pi, math.pi)))
        tree.Fill()
    tree.Write()
    lbt = ROOT.TTree("LuminosityBlocks", "LuminosityBlocks")
    l_run = np.zeros(1, dtype=np.uint32)
    l_ls = np.zeros(1, dtype=np.uint32)
    lbt.Branch("run", l_run, "run/i")
    lbt.Branch("luminosityBlock", l_ls, "luminosityBlock/i")
    for r, l in sorted(lumis):
        l_run[0], l_ls[0] = r, l
        lbt.Fill()
    lbt.Write()
    runs = ROOT.TTree("Runs", "Runs")
    r_run = np.zeros(1, dtype=np.uint32)
    runs.Branch("run", r_run, "run/i")
    if mc:
        r_cnt = np.array([a.n], dtype=np.int64)
        r_sw = np.array([sumw], dtype=np.float64)
        r_sw2 = np.array([sumw2], dtype=np.float64)
        runs.Branch("genEventCount", r_cnt, "genEventCount/L")
        runs.Branch("genEventSumw", r_sw, "genEventSumw/D")
        runs.Branch("genEventSumw2", r_sw2, "genEventSumw2/D")
    r_run[0] = 1
    runs.Fill()
    runs.Write()
    out.Close()
    print("WROTE %s events=%d branches=%d counters=%s skipped(not in this kind of file)=%s"
          % (a.o, a.n, len(bufs), sorted(counters), skipped))
    return 0


if __name__ == "__main__":
    sys.exit(main())
