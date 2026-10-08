#!/usr/bin/env python3
"""btag_eff_maps.py -- the MC b-tag efficiency maps of the fixed-WP method (STEP 25 K; docs/DECISIONS.md D-2026-10-08-A)

    python3 tools/stage7/btag_eff_maps.py --config AnalyzerConfig/Tier3_2024_FH_unified_btagtrig.yml
            --base /pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/AnalyzerOutput_btagtrig_notrig_2024
            [--out DerivedCorr/BTagEff/2024_Summer24/btag_eff_2024.json.gz] [--root-out FILE] [--plots FILE.pdf]
            [--min-neff 50] [--min-neff-bin 10] [--flavours-required b] [--exclude NAME ...] [--no-default-exclude]
            [--allow-missing] [--check-only]

In cmsenv (PyROOT; the correctionlib Python package checks the written file when it is there).

Why: the 2024 b-tag SF of BTV is a fixed-WP SF (UParTAK4_kinfit, b jets; no shape SF yet), applied with BTV's
"method 1a": a jet's weight is P_Data / P_MC of its tag bin, which needs the MC efficiency e(flavour, pT, |eta|) of
each WP in our selection (the analyzer: computeBTagWeightFixedWP_). The analyzer fills, for MC of a fixed-WP year,
BTagEff/h2_<b|c|l>_<all|L|M|T> (selected jets in pT x |eta| at the HT step, weighted by the event weight before the
SFs) and BTagEff/h_nevt (the events filled). This tool sums them per process group and writes the maps.

What it does:
  1. the MC samples of the yml (the submitter's own yml reader; Data by its is_data_name are skipped), each as the
     merged file <base>/<sample>.root (outputMerger/merge_outputs.py), minus the plotter's default exclusions of the
     year (2024: the 4FS TTbb_* and TT4b, not stacked on the inclusive tt -- D-2026-10-05-C; the maps follow the stack)
     and --exclude. A missing file stops (exit 1) unless --allow-missing.
  2. per sample: BTagEff must be there (an output of an executable before commit K has none) and h_nevt must equal
     the HT>500 bin of Tree/cutflow (unweighted; a TH1F: exact below 2^24 events, above it hadd's float rounding is
     allowed, 1e-4 N) -- equal when every job of the merged file filled the histograms. BTagEff/h_wp holds the
     year's WPs x the jobs (bin 4 = jobs): the WPs (bin / bin 4) must be the same in every sample; they go into the
     maps' description ('wp=L:<l>,M:<m>,T:<t>'), which the analyzer compares with its own at load (E52 if not).
  3. the group of the sample (btag_eff_group: the mirror of include/BTagEffGroup.h -- qcd: QCD*, tt: a name starting
     with tt/TT in any case, other: the rest) and "all" (every sample used).
  4. per group, flavour (b = 5, c = 4, light = 0) and bin: e(WP) = sum w (score >= WP) / sum w, if the effective
     number of jets of the bin's denominator, N_eff = (sum w)^2 / sum w^2, is at least --min-neff AND each of the
     three tag bins the weight uses (score < L, L <= score < M, score >= M) has N_eff >= --min-neff-bin; else the
     first of (the group, |eta| summed over the pT bin), (all, the bin), (all, |eta| summed), (all, the whole map)
     that has it (the whole map: each tag bin only > 0). A level is also skipped when its sums are unphysical (not
     0 <= num_T <= num_M <= num_L <= den: negative MC weights in a sparse bin). Why the tag bins: an efficiency at
     or near 0 or 1 gives the jets on the other side of the WP a weight of up to 1/1e-4 -- in the sample the maps come
     from such a bin has no such jets, but other samples weighted with the maps (the 4FS TTbb_*/TT4b with the tt
     maps, a sample left out) do. The tag-bin rule is for the flavours of --flavours-required (default b: the 2024
     payload has a b SF only, so the analyzer weights b jets only; with c / light SFs: --flavours-required b c l);
     the other flavours' maps are made with the N_eff and physical rules only (a NOTE line). All WPs of a bin take
     the same level (e_T <= e_M <= e_L). e is kept in [1e-4, 1 - 1e-4] (the analyzer's clamp).
  5. the correctionlib JSON (schema v2): "btag_eff" (inputs group, flavor, working_point, abseta, pt; the group's
     maps, the unknown group -> "all"; multibinning abseta x pt with clamp) and "btag_eff_groups" (group -> 1 for a
     group of the file, else 0); its description says year=<YYYY> (the submitter's preflight checks it). Then, with
     the correctionlib Python package, every bin centre is evaluated from the written file and compared.
Lines: CONFIG / BASE; SAMPLE <name> group=<g> nevt=<n> ht_step=<n> OK|MISMATCH; EXCLUDED / MISSING / DATA-SKIPPED;
GROUP <g> samples=<n> jets b=<..> c=<..> l=<..>; EFF <g> <flav> <wp> <integrated e> (N_eff ...);
LEVELS <g> <flav> L0=.. L1=.. L2=.. L3=.. L4=.. [(skipped: unphysical n, thin tag bin m)]; WP L=.. M=.. T=..;
WROTE <file>; VERIFY ok|FAIL|skipped; RESULT OK|FAIL.
Output: --out (default DerivedCorr/BTagEff/<year>_Summer24/btag_eff_<year>.json.gz under the repo for 2024);
--root-out: the final maps (eff_<g>_<f>_<wp>), their levels (level_<g>_<f>) and N_eff (neff_<g>_<f>) as TH2D;
--plots: a multipage PDF of the maps (COLZ TEXT). --check-only writes nothing.
Exit: 0 ok; 1 a check failed; 2 bad arguments.
"""
from __future__ import print_function

import argparse
import datetime
import gzip
import importlib.util
import json
import math
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
FLAVOURS = (("b", 5), ("c", 4), ("l", 0))
WPS = ("L", "M", "T")
EFF_MIN, EFF_MAX = 1.0e-4, 1.0 - 1.0e-4
GROUPS = ("tt", "qcd", "other")
DEFAULT_OUT = {"2024": "DerivedCorr/BTagEff/2024_Summer24/btag_eff_2024.json.gz"}


def btag_eff_group(sample):
    """KEEP IN SYNC with tthh::btagEffGroup (include/BTagEffGroup.h; test_btag_eff_maps.py compares the two)."""
    if sample.startswith("QCD"):
        return "qcd"
    if len(sample) >= 2 and sample[0] in "tT" and sample[1] in "tT":
        return "tt"
    return "other"


def load_module(name, rel):
    spec = importlib.util.spec_from_file_location(name, os.path.join(REPO, rel))
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


class Sums(object):
    """Per flavour and WP ('all' = the denominator): the bin contents and the sum of w^2 of a pT x |eta| map."""

    def __init__(self, npt, neta):
        self.npt, self.neta = npt, neta
        self.w = {(f, k): [[0.0] * neta for _ in range(npt)] for f, _ in FLAVOURS for k in ("all",) + WPS}
        self.w2 = {(f, k): [[0.0] * neta for _ in range(npt)] for f, _ in FLAVOURS for k in ("all",) + WPS}
        self.n_samples = 0

    def add_hist(self, f, k, h):
        for i in range(self.npt):
            for j in range(self.neta):
                self.w[(f, k)][i][j] += h.GetBinContent(i + 1, j + 1)
                self.w2[(f, k)][i][j] += h.GetBinError(i + 1, j + 1) ** 2

    def add(self, other):
        for key in self.w:
            for i in range(self.npt):
                for j in range(self.neta):
                    self.w[key][i][j] += other.w[key][i][j]
                    self.w2[key][i][j] += other.w2[key][i][j]
        self.n_samples += other.n_samples

    # the three aggregations of the fallback: one bin, one pT bin over |eta|, the whole map
    def cell(self, f, k, i, j):
        return self.w[(f, k)][i][j], self.w2[(f, k)][i][j]

    def row(self, f, k, i):
        return sum(self.w[(f, k)][i]), sum(self.w2[(f, k)][i])

    def whole(self, f, k):
        return sum(map(sum, self.w[(f, k)])), sum(map(sum, self.w2[(f, k)]))


def neff(sw, sw2):
    return (sw * sw / sw2) if (sw > 0 and sw2 > 0) else 0.0


def clamp_eff(e):
    return min(max(e, EFF_MIN), EFF_MAX)


def tag_bins(den, nums):
    """The three tag bins the weight uses, from the cumulative sums: [(sum w, sum w^2)] of score < L, L <= score < M,
    score >= M. den = (sum w, sum w^2) of all jets, nums = [(sum w, sum w^2) of score >= WP] for L, M, T."""
    (d, d2), (l, l2), (m, m2) = den, nums[0], nums[1]
    return [(d - l, d2 - l2), (l - m, l2 - m2), (m, m2)]


def build_maps(sums, min_neff, min_neff_bin=0.0, required=("b",)):
    """{group: {f: {"eff": {wp: [[e]]}, "level": [[lvl]], "neff": [[n]], "unphys": n, "thin": n}}} with the fallback of
    the docstring (4). A level is taken when its denominator is > 0 with N_eff >= min_neff, its numbers are physical
    (0 <= num_T <= num_M <= num_L <= den; negative MC weights can break it in a sparse bin), and each of the three tag
    bins of the weight has N_eff >= min_neff_bin (the whole map: each > 0) -- an efficiency at or near 0 or 1 would give
    the jets on the other side of the WP a weight of up to 1/1e-4 in a sample weighted with these maps. Skipped
    levels are counted ("unphys", "thin"). The tag-bin rule applies to the flavours in `required` only (the others: the
    N_eff and physical rules). A flavour without any usable level (not even the whole 'all' map) is returned in
    `empty`."""
    out, empty = {}, []
    allg = sums["all"]

    def agg(src, how, f, k, i, j):
        return getattr(src, how)(f, k, *((i, j) if how == "cell" else (i,) if how == "row" else ()))

    for g, s in sums.items():
        out[g] = {}
        for f, _ in FLAVOURS:
            npt, neta = s.npt, s.neta
            eff = {wp: [[0.0] * neta for _ in range(npt)] for wp in WPS}
            lvl = [[-1] * neta for _ in range(npt)]
            nef = [[0.0] * neta for _ in range(npt)]
            unphys = thin = 0
            for i in range(npt):
                for j in range(neta):
                    nef[i][j] = neff(*s.cell(f, "all", i, j))
                    cands = [(s, "cell"), (s, "row"), (allg, "cell"), (allg, "row"), (allg, "whole")]
                    if g == "all":
                        cands = [(allg, "cell"), (allg, "row"), (allg, "whole")]
                    for li, (src, how) in enumerate(cands):
                        den = agg(src, how, f, "all", i, j)
                        if not (den[0] > 0 and (neff(*den) >= min_neff or how == "whole")):
                            continue
                        nums = [agg(src, how, f, wp, i, j) for wp in WPS]                # L, M, T
                        if not (0.0 <= nums[2][0] <= nums[1][0] <= nums[0][0] <= den[0]):
                            unphys += 1
                            continue
                        bins = tag_bins(den, nums)
                        if f not in required:
                            ok_bins = True
                        elif how == "whole":
                            ok_bins = all(b[0] > 0 for b in bins)
                        else:
                            ok_bins = all(neff(*b) >= min_neff_bin and b[0] > 0 for b in bins)
                        if not ok_bins:
                            thin += 1
                            continue
                        for wp, nu in zip(WPS, nums):
                            eff[wp][i][j] = clamp_eff(nu[0] / den[0])
                        # the levels of the docstring: 0 bin, 1 group |eta|-summed, 2 all bin, 3 all |eta|-summed,
                        #   4 all whole map ('all' itself: 0, 3, 4)
                        lvl[i][j] = li if g != "all" else (0, 3, 4)[li]
                        break
                    if lvl[i][j] < 0:
                        empty.append((g, f))
            out[g][f] = {"eff": eff, "level": lvl, "neff": nef, "unphys": unphys, "thin": thin}
    return out, sorted(set(empty))


def correction_json(maps, groups, pt_edges, eta_edges, desc):
    def multibin(g, f, wp):
        e = maps[g][f]["eff"][wp]
        # correctionlib multibinning: the content is flattened with the LAST input varying fastest (abseta outer, pt
        #   inner); the test evaluates the written file to keep this honest
        content = [e[i][j] for j in range(len(eta_edges) - 1) for i in range(len(pt_edges) - 1)]
        return {"nodetype": "multibinning", "inputs": ["abseta", "pt"], "edges": [list(eta_edges), list(pt_edges)],
                "content": content, "flow": "clamp"}

    def group_node(g):
        return {"nodetype": "category", "input": "flavor",
                "content": [{"key": fl, "value": {"nodetype": "category", "input": "working_point",
                                                  "content": [{"key": wp, "value": multibin(g, f, wp)} for wp in WPS]}}
                            for f, fl in FLAVOURS]}

    eff = {"name": "btag_eff", "version": 1,
           "description": "MC efficiency of a selected jet to pass a b-tag WP, per process group (tt, qcd, other; all "
                          "= every sample used, also the default for an unknown group), hadron flavour (5, 4, 0), WP "
                          "(L, M, T), |eta| and pT (clamped into the map). " + desc,
           "inputs": [{"name": "group", "type": "string", "description": "BTagEffGroup.h: tt, qcd, other, or all"},
                      {"name": "flavor", "type": "int", "description": "hadron flavour: 5, 4, 0"},
                      {"name": "working_point", "type": "string", "description": "L, M, T"},
                      {"name": "abseta", "type": "real", "description": "|eta| of the jet"},
                      {"name": "pt", "type": "real", "description": "pT of the jet [GeV]"}],
           "output": {"name": "efficiency", "type": "real"},
           "data": {"nodetype": "category", "input": "group",
                    "content": [{"key": g, "value": group_node(g)} for g in groups],
                    "default": group_node("all")}}
    grp = {"name": "btag_eff_groups", "version": 1,
           "description": "1 for a group the maps of btag_eff have, else 0. " + desc,
           "inputs": [{"name": "group", "type": "string"}],
           "output": {"name": "has_group", "type": "real"},
           "data": {"nodetype": "category", "input": "group",
                    "content": [{"key": g, "value": 1.0} for g in groups], "default": 0.0}}
    return {"schema_version": 2, "description": "ttHH b-tag efficiency maps. " + desc, "corrections": [eff, grp]}


def verify(path, maps, groups, pt_edges, eta_edges):
    """Evaluate every bin centre of the written file with the correctionlib Python package. (ok, text)."""
    try:
        import correctionlib
    except Exception as exc:  # pragma: no cover
        return None, "skipped (no correctionlib Python package: %s)" % exc
    cset = correctionlib.CorrectionSet.from_file(path)
    c, cg = cset["btag_eff"], cset["btag_eff_groups"]
    worst, n = 0.0, 0
    for g in groups:
        if cg.evaluate(g) != 1.0:
            return False, "btag_eff_groups(%s) != 1" % g
        for f, fl in FLAVOURS:
            for wp in WPS:
                for i in range(len(pt_edges) - 1):
                    for j in range(len(eta_edges) - 1):
                        pt = 0.5 * (pt_edges[i] + pt_edges[i + 1])
                        ae = 0.5 * (eta_edges[j] + eta_edges[j + 1])
                        v = c.evaluate(g, fl, wp, ae, pt)
                        worst = max(worst, abs(v - maps[g][f]["eff"][wp][i][j]))
                        n += 1
    if cg.evaluate("no-such-group") != 0.0:
        return False, "btag_eff_groups(unknown) != 0"
    v_unknown = c.evaluate("no-such-group", 5, "M", 0.3, 50.0)
    v_all = c.evaluate("all", 5, "M", 0.3, 50.0)
    if v_unknown != v_all:
        return False, "an unknown group does not give the 'all' maps (%g vs %g)" % (v_unknown, v_all)
    v_lo, v_hi = c.evaluate("all", 5, "M", 0.3, 1.0), c.evaluate("all", 5, "M", 9.0, 1.0e5)
    if not (EFF_MIN <= v_lo <= EFF_MAX and EFF_MIN <= v_hi <= EFF_MAX):
        return False, "out-of-map values not clamped (%g, %g)" % (v_lo, v_hi)
    if worst > 1.0e-12:
        return False, "%d bin centres evaluated, largest difference %g" % (n, worst)
    return True, "%d bin centres evaluated, identical; unknown group -> 'all'; clamp ok" % n


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--config", required=True)
    ap.add_argument("--base", required=True)
    ap.add_argument("--out")
    ap.add_argument("--root-out")
    ap.add_argument("--plots")
    ap.add_argument("--min-neff", type=float, default=50.0)
    ap.add_argument("--min-neff-bin", type=float, default=10.0)
    ap.add_argument("--flavours-required", nargs="+", default=["b"], choices=["b", "c", "l"],
                    help="flavours whose maps must pass the tag-bin rule (default b: the 2024 payload has b only)")
    ap.add_argument("--exclude", nargs="*", default=[])
    ap.add_argument("--no-default-exclude", action="store_true")
    ap.add_argument("--allow-missing", action="store_true")
    ap.add_argument("--check-only", action="store_true")
    a = ap.parse_args(argv)

    cfg = a.config if os.path.isabs(a.config) else os.path.join(REPO, a.config)
    if not os.path.isfile(cfg) or not os.path.isdir(a.base):
        print("ERROR no config %s or no base directory %s" % (cfg, a.base))
        return 2
    if a.min_neff < 0 or a.min_neff_bin < 0:
        print("ERROR --min-neff and --min-neff-bin must be >= 0")
        return 2
    sub = load_module("tthh_submitter", "submit_job_FH_Tier3_unified.py")
    mp = load_module("tthh_make_plots", os.path.join("plotter", "make_plots.py"))
    conf = sub.CondorJobManager.load_yaml_config(None, cfg)
    year = str(conf["common"].get("year", "")).strip()
    out = a.out or DEFAULT_OUT.get(year)
    if not out and not a.check_only:
        print("ERROR no default output for year '%s': give --out" % year)
        return 2
    if out and not os.path.isabs(out):
        out = os.path.join(REPO, out)
    names = [sub.entry_name(s) for s in conf["samples"]]
    excl = set(a.exclude) | (set() if a.no_default_exclude else set(mp.DEFAULT_EXCLUDE.get(year, [])))
    print("CONFIG %s year=%s samples=%d" % (cfg, year, len(names)))
    print("BASE %s" % os.path.abspath(a.base))

    import ROOT
    ROOT.gROOT.SetBatch(True)
    ROOT.gErrorIgnoreLevel = ROOT.kError
    ROOT.TH1.AddDirectory(False)

    sums, bad, used = {}, [], []
    pt_edges = eta_edges = None
    wp_ref = None
    for n in names:
        if sub.is_data_name(n):
            print("DATA-SKIPPED %s" % n)
            continue
        if n in excl:
            why = ("D-2026-10-05-C: not stacked on the inclusive tt in %s" % year
                   if n in mp.DEFAULT_EXCLUDE.get(year, []) else "--exclude")
            print("EXCLUDED %s (%s)" % (n, why))
            continue
        p = os.path.join(a.base, n + ".root")
        if not os.path.isfile(p):
            print("MISSING %s %s" % (n, p))
            if not a.allow_missing:
                bad.append(n)
            continue
        f = ROOT.TFile.Open(p)
        if not f or f.IsZombie():
            print("MISSING %s %s (cannot open)" % (n, p))
            bad.append(n)
            continue
        try:
            hn, cf, hw = f.Get("BTagEff/h_nevt"), f.Get("Tree/cutflow"), f.Get("BTagEff/h_wp")
            hs = {(fl, k): f.Get("BTagEff/h2_%s_%s" % (fl, k)) for fl, _ in FLAVOURS for k in ("all",) + WPS}
            if not hn or not hw or any(not h for h in hs.values()):
                print("SAMPLE %s NO-BTAGEFF (BTagEff/h_nevt, h_wp or h2_* absent: an output of an executable before "
                      "commit K, or not a fixed-WP year)" % n)
                bad.append(n)
                continue
            njobs = hw.GetBinContent(4)
            wps = tuple(hw.GetBinContent(k) / njobs for k in (1, 2, 3)) if njobs > 0 else None
            if wps is None or (wp_ref is not None and any(abs(x - y) > 1.0e-6 for x, y in zip(wps, wp_ref))):
                print("SAMPLE %s WP %s differ from the first sample's %s" % (n, wps, wp_ref))
                bad.append(n)
                continue
            wp_ref = wps
            ht = None
            if cf:
                ax = cf.GetXaxis()
                for i in range(1, cf.GetNbinsX() + 1):
                    if ax.GetBinLabel(i) == "HT>500":
                        ht = cf.GetBinContent(i)
            nevt = hn.GetBinContent(1)
            # Tree/cutflow is a TH1F: its integer counts are exact below 2^24, and hadd's float sums round above
            ok = ht is not None and abs(nevt - ht) <= (0.0 if ht < 2.0 ** 24 else 1.0e-4 * ht)
            g = btag_eff_group(n)
            print("SAMPLE %s group=%s nevt=%d ht_step=%s %s" % (n, g, nevt, "-" if ht is None else "%d" % ht,
                                                                  "OK" if ok else "MISMATCH"))
            if not ok:
                bad.append(n)
                continue
            h0 = hs[("b", "all")]
            pe = [h0.GetXaxis().GetBinLowEdge(i) for i in range(1, h0.GetNbinsX() + 2)]
            ee = [h0.GetYaxis().GetBinLowEdge(i) for i in range(1, h0.GetNbinsY() + 2)]
            if pt_edges is None:
                pt_edges, eta_edges = pe, ee
            elif (pe, ee) != (pt_edges, eta_edges):
                print("SAMPLE %s BINNING differs from the first sample's" % n)
                bad.append(n)
                continue
            s = Sums(len(pt_edges) - 1, len(eta_edges) - 1)
            for (fl, k), h in hs.items():
                s.add_hist(fl, k, h)
            s.n_samples = 1
            for key in (g, "all"):
                sums.setdefault(key, Sums(s.npt, s.neta)).add(s)
            used.append(n)
        finally:
            f.Close()

    if not used:
        print("RESULT FAIL (no MC sample with BTagEff histograms)")
        return 1
    groups = [g for g in GROUPS if g in sums] + ["all"]
    for g in groups:
        s = sums[g]
        print("GROUP %s samples=%d jets(sum w) %s" % (g, s.n_samples, " ".join(
            "%s=%.4g" % (fl, s.whole(fl, "all")[0]) for fl, _ in FLAVOURS)))
    maps, empty = build_maps(sums, a.min_neff, a.min_neff_bin, tuple(a.flavours_required))
    rest = [fl for fl, _ in FLAVOURS if fl not in a.flavours_required]
    if rest:
        print("NOTE %s: the tag-bin rule not applied (not in --flavours-required; the 2024 payload has a b SF only, so "
              "the analyzer gives these jets weight 1 and does not read their maps)" % ", ".join(rest))
    if empty:
        for g, fl in empty:
            print("EMPTY %s %s: no usable level for this flavour (no jets, unphysical sums, or a tag bin empty even in "
                  "the whole 'all' map) -- no map" % (g, fl))
        print("RESULT FAIL (flavours without jets)")
        return 1
    for g in groups:
        s = sums[g]
        for fl, _ in FLAVOURS:
            den = s.whole(fl, "all")
            print("EFF %s %s %s (N_eff %.4g)" % (g, fl, " ".join(
                "%s=%.4f" % (wp, s.whole(fl, wp)[0] / den[0] if den[0] > 0 else float("nan")) for wp in WPS),
                neff(*den)))
            lv = [x for row in maps[g][fl]["level"] for x in row]
            m = maps[g][fl]
            print("LEVELS %s %s %s" % (g, fl, " ".join("L%d=%d" % (k, lv.count(k)) for k in range(5)))
                  + (" (skipped: unphysical %d, thin tag bin %d)" % (m["unphys"], m["thin"])
                     if (m["unphys"] or m["thin"]) else ""))
    print("WP L=%.6g M=%.6g T=%.6g (BTagEff/h_wp; the same in every sample)" % wp_ref)

    stamp = datetime.datetime.now(datetime.timezone.utc).strftime("%Y-%m-%dT%H:%MZ")
    desc = ("year=%s; wp=L:%.6g,M:%.6g,T:%.6g; method=fixed WP 1a (L, M), D-2026-10-08-A; samples=%d (%s); base=%s; "
            "config=%s; min_neff=%g; min_neff_bin=%g; flavours_required=%s; made=%s by tools/stage7/btag_eff_maps.py" % (
                year, wp_ref[0], wp_ref[1], wp_ref[2], len(used), " ".join(
                    "%s:%d" % (g, sums[g].n_samples) for g in groups if g != "all"), os.path.abspath(a.base),
                os.path.relpath(cfg, REPO) if cfg.startswith(REPO) else cfg, a.min_neff, a.min_neff_bin,
                ",".join(a.flavours_required), stamp))
    if bad:
        print("PROBLEMS %s" % " ".join(bad))
    if a.check_only:
        print("RESULT %s (check only)" % ("FAIL" if bad else "OK"))
        return 1 if bad else 0
    if bad:
        print("RESULT FAIL (%d sample(s) above; nothing written)" % len(bad))
        return 1

    js = correction_json(maps, groups, pt_edges, eta_edges, desc)
    os.makedirs(os.path.dirname(out) or ".", exist_ok=True)
    tmp = out + ".tmp"
    with (gzip.open(tmp, "wt") if out.endswith(".gz") else open(tmp, "w")) as fh:
        json.dump(js, fh, indent=None, separators=(",", ":"))
    os.replace(tmp, out)
    print("WROTE %s" % out)
    ok, text = verify(out, maps, groups, pt_edges, eta_edges)
    print("VERIFY %s %s" % ({True: "ok", False: "FAIL", None: "skipped"}[ok], text))
    probs, _ = sub.btag_eff_json_summary(out, year)
    if probs:
        print("PREFLIGHT-CHECK FAIL %s" % "; ".join(probs))
        ok = False

    if a.root_out or a.plots:
        import array
        pe, ee = array.array("d", pt_edges), array.array("d", eta_edges)
        hists = []
        for g in groups:
            for fl, _ in FLAVOURS:
                m = maps[g][fl]
                for kind, wp in [("eff", w) for w in WPS] + [("level", None), ("neff", None)]:
                    name = "%s_%s_%s%s" % (kind, g, fl, "_" + wp if wp else "")
                    h = ROOT.TH2D(name, "%s %s %s jets%s;p_{T} [GeV];|#eta|" % (kind, g, fl, ", " + wp if wp else ""),
                                  len(pe) - 1, pe, len(ee) - 1, ee)
                    for i in range(len(pe) - 1):
                        for j in range(len(ee) - 1):
                            v = m["eff"][wp][i][j] if kind == "eff" else m["level"][i][j] if kind == "level" \
                                else m["neff"][i][j]
                            h.SetBinContent(i + 1, j + 1, v)
                    hists.append(h)
        if a.root_out:
            fo = ROOT.TFile(a.root_out, "RECREATE")
            for h in hists:
                h.Write()
            fo.Close()
            print("WROTE %s" % a.root_out)
        if a.plots:
            ROOT.gStyle.SetOptStat(0)
            ROOT.gStyle.SetPaintTextFormat(".3f")
            c = ROOT.TCanvas("c", "", 1000, 700)
            c.SetLogx(True)
            c.SetRightMargin(0.14)
            effh = [h for h in hists if h.GetName().startswith("eff_")]
            for k, h in enumerate(effh):
                h.SetMinimum(0.0)
                h.SetMaximum(1.0)
                h.Draw("COLZ TEXT")
                c.Print(a.plots + ("(" if k == 0 else ")" if k == len(effh) - 1 else ""), "pdf")
            print("WROTE %s (%d pages)" % (a.plots, len(effh)))
    print("RESULT %s" % ("OK" if ok is not False else "FAIL"))
    return 0 if ok is not False else 1


if __name__ == "__main__":
    sys.exit(main())
