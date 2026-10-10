#!/usr/bin/env python3
"""make_plots.py -- stack plots and a yield table from the merged main outputs of one analyzer yml (STEP 24)

    python3 plotter/make_plots.py --config AnalyzerConfig/Tier3_2024_FH_unified_main.yml
            --base /pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/AnalyzerOutput_main_notrig_2024
            [--out DIR] [--grouping compact|detailed|both] [--exclude NAME ...] [--no-default-exclude]
            [--include-hist REGEX ...] [--exclude-hist REGEX ...] [--note TEXT] [--allow-missing] [--check-only]
            [--tree-cut EXPR [--tree-label TEXT]]

What it does (in cmsenv: PyROOT and root):
  1. the samples of the yml (the submitter's own yml reader; Data by the submitter's is_data_name), each as the
     merged file <base>/<sample>.root (outputMerger/merge_outputs.py). A missing or unreadable file stops (exit 1)
     unless --allow-missing.
  2. default exclusions by year: 2024 -> TTbb_Hadronic, TTbb_SemiLep, TTbb_DiLep, TT4b (D-2026-10-05-C: no 2024
     stitching, so the 4FS tt+bb and tt+4b samples are not stacked on the inclusive ttbar). --exclude adds more,
     --no-default-exclude drops the defaults.
  3. YIELDS: the weighted cutflow (Tree/cutflow_w) of every sample: per step the MC sum, the Data sum and
     Data/MC; per sample the yields at HT>500, nbjets>=2, nbjets>=4 and the last step (a sample with a zero or
     negative yield at noCut is flagged). Before the skim (the ntuples are skimmed, e.g. 2024 6j20, so the
     analyzer's noCut is after it): per MC sample 'generated' = weight x Sigma genw(Runs) = lumi x xsec x br (the
     submitter's own weight and the prescan summary of common.prescan), the skim efficiency Sigma genw(Events) /
     Sigma genw(Runs), and noCut / (weight x Sigma genw(Events)) -- about the mean PU weight when every main job of
     the sample is in the merged file, clearly lower when jobs are missing (WARN below 0.7 or above 1.3). Data
     have no pre-skim row: the production had no lumi mask, so its input counts (NtupleForge ForgeAudit n_in)
     include non-golden lumisections.
     Exact completeness (MC, 2026-10-06): the unweighted noCut (Tree/cutflow bin 1) of the merged file must equal
     the prescan's event count of the sample (nEvents_total) -- the same filelists, and every MC event reaches noCut.
     A difference (beyond the float rounding of the TH1F summed by hadd: max(2, 1e-5 x N)) is a FLAG, so RESULT
     FAIL: a job or an input file is missing from the merge, a job skipped an input file it could not open (ROOT's
     TChain only prints an error), or the filelists changed between prescan and main. Data have no such reference.
     --check-only stops here.
  4. structure_info.yml from one MC file (TTbar_Hadronic if present): every TH1 (no TH2/TProfile), in file order,
     filtered by --include-hist / --exclude-hist (regex on the key path) -- the format plotter/extract_structure.py
     writes, made with PyROOT here so uproot is not needed. [STEP 25 K] Not the BTagEff/ directory (the inputs of
     the b-tag efficiency maps, MC only).
  5. samples_config.yml, a copy of plotter/stack_plotter.C, and `root -l -b -q stack_plotter.C` per grouping with
     TTHH_PLOT_GROUPING, TTHH_PLOT_LUMI (the yml's lumi_fb_inv), TTHH_PLOT_SQRTS (13.6 for Run 3, else 13),
     TTHH_PLOT_NOTE (year; 2024: '#sigma: provisional'; the SF state read from the base name, e.g. 'trigger SF + b-tag
     SF (fixed WP, b jets) applied' for a 2024 _btagsf base) and
     TTHH_PLOT_MULTIPAGE=1 (all plots also in plots_<grouping>/all_<grouping>.pdf).
  6. --tree-cut EXPR (D-2026-10-06-A): control plots from the event tree instead of the stored histograms. The main
     output keeps every selected event in Tree/Tree with its trigger bits, HT, jets, b-tag scores, event shapes and
     evtWeight, so another trigger or HT requirement on top of the selection needs no new analyzer run. Per sample:
     the events of Tree/Tree passing EXPR (an RDataFrame filter on the tree branches and on the columns of TREE_HISTS,
     e.g. 'passTrigger_HLT_PFHT1050 && HT > 1200'), MC weighted by evtWeight (the run's production weight: base x PU
     x L1 x genW x stitch x the SFs switched on -- what the stored jet / b-tag histograms use; a few stored cut-step
     histograms (cutStep_*_ht, *_njets_raw, *_nbjets_raw) use it without the SFs, the same when the SFs are off as in
     the 2024 first look), Data by 1, fill TREE_HISTS into <out>/tree/<sample>.root (directory Control/); steps 4-5
     then plot those (the YIELDS and checks of 3 stay those of the merged files). TREEYIELD lines (also in
     YIELDS.txt): per sample the events and the weighted sum after EXPR, then MC, Data and Data/MC. --tree-label: a
     line added to the plot note (TLatex; default 'event-tree cut: see YIELDS.txt', also after --note).
     The jet-multiplicity diagnostics (STEP 24 I): besides nJets, the number of selected jets with pT > 40 and > 50 GeV,
     with |eta| < 2.0 and with 2.0 <= |eta| < 2.4, nJets for events with exactly two and with three or more b-tagged
     jets, and the b-tag score of every selected jet (one entry per jet). TREEFLAV lines (also in YIELDS.txt; MC
     trees with the hadFlavs branch): per b-tag score bin of 0.1, all selected jets, the MC sum split by hadron
     flavour (b = 5, c = 4, light = the rest), the Data count and Data/MC -- where in the score the Data/MC moves.
     The tree holds the events after the whole selection of the run (its trigger OR included), so EXPR can only
     narrow it: a path of that OR, a higher HT, more b-tags ...
     [STEP 27 O] --tree-v1: also TREE_HISTS_V1, the Tree v1 ML inputs (χ² pairing, HT^b, average masses, ΔR/Δη
     statistics, Fox-Wolfram, centrality), from trees of an analyzer with Tree v1 only.
Output: --out (default <repo>/condor/plots/<base name>[_tree]_<UTC>/; condor/ is gitignored): YIELDS.txt, the two
yml, plotter_<grouping>.log, plots_<grouping>/*.pdf (and tree/*.root with --tree-cut).
Lines: SAMPLE / EXCLUDED / MISSING, YIELD ..., [TREECUT, TREEYIELD ..., TREEFLAV ...,] HIST n=..., PLOTS <grouping> pdf=<n>
multipage=<path>, RESULT OK|FAIL.
Exit: 0 ok; 1 a check or the plotter failed; 2 bad arguments.
"""
from __future__ import print_function

import argparse
import contextlib
import datetime
import importlib.util
import io
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(HERE)
DEFAULT_EXCLUDE = {"2024": ["TTbb_Hadronic", "TTbb_SemiLep", "TTbb_DiLep", "TT4b"]}
SQRTS = {"2016": "13", "2017": "13", "2018": "13", "2022": "13.6", "2023": "13.6", "2024": "13.6"}
YEAR_NOTE = {"2024": "2024 C-I;#sigma: provisional (13.6 TeV)"}
KEY_STEPS = ("HT>500", "nbjets>=2", "nbjets>=4")

# [D-2026-10-06-A] --tree-cut: (name, x-axis title, column, nbins, low, high). The jets are the selected jets in pT
#   order (Tree/Tree jetPt, jetEta); btag_<i> is the i-th highest b-tag score of them (bTagScore: UParTAK4B in 2024,
#   DeepJet in Run 2); a missing entry is -999 (underflow).
TREE_HISTS = (
    [("HT", "H_{T} [GeV]", "HT", 60, 0.0, 3000.0),
     ("nJets", "number of jets", "nJets", 10, 5.5, 15.5),
     ("nbJets", "number of b-tagged jets", "nbJets", 8, -0.5, 7.5),
     ("MET", "p_{T}^{miss} [GeV]", "MET_pt", 30, 0.0, 300.0)]
    + [("jet%d_pt" % i, "jet %d p_{T} [GeV]" % i, "jet_pt_%d" % i, n, 0.0, hi)
       for i, n, hi in ((1, 30, 1500.0), (2, 30, 1200.0), (3, 40, 800.0), (4, 30, 600.0), (5, 25, 500.0), (6, 20, 400.0))]
    + [("jet%d_eta" % i, "jet %d #eta" % i, "jet_eta_%d" % i, 25, -2.5, 2.5) for i in range(1, 7)]
    + [("btag%d" % i, "b-tag score, %s highest" % w, "btag_%d" % i, 20, 0.0, 1.0)
       for i, w in ((1, "1st"), (2, "2nd"), (3, "3rd"), (4, "4th"))]
    + [(c, t, c, 25, 0.0, hi) for c, t, hi in (("aplanarity", "aplanarity (jets)", 0.5), ("sphericity", "sphericity (jets)", 1.0),
                                               ("eventC", "C (jets)", 1.0), ("eventD", "D (jets)", 1.0),
                                               ("bjetAplanarity", "aplanarity (b jets)", 0.5),
                                               ("bjetSphericity", "sphericity (b jets)", 1.0))]
    # [STEP 24 I] where does the nJets slope come from: harder jets only, central / forward jets, per b-tag multiplicity,
    #   and the b-tag score of every selected jet (the selection: pT > 30 GeV, |eta| < 2.4, 6th jet pT > 40 GeV)
    + [("nJets_pt40", "number of jets, p_{T} > 40 GeV", "njet_pt40_", 10, 5.5, 15.5),
       ("nJets_pt50", "number of jets, p_{T} > 50 GeV", "njet_pt50_", 12, 3.5, 15.5),
       ("nJets_central", "number of jets, |#eta| < 2.0", "njet_central_", 13, 2.5, 15.5),
       ("nJets_forward", "number of jets, 2.0 #leq |#eta| < 2.4", "njet_forward_", 7, -0.5, 6.5),
       ("nJets_nb2", "number of jets (n_{b} = 2)", "njet_nb2_", 10, 5.5, 15.5),
       ("nJets_nb3p", "number of jets (n_{b} #geq 3)", "njet_nb3p_", 10, 5.5, 15.5),
       ("btag_alljets", "b-tag score, every selected jet", "bTagScore", 20, 0.0, 1.0)])
FLAV_BINS = 10      # TREEFLAV: b-tag score bins of 0.1 in [0, 1]
# [STEP 27 O] --tree-v1 (with --tree-cut): the Tree v1 ML inputs (AN-2022/122 Table 43 for FH; tempTTHH docs/PLAN_ML_SYST.md
#   §5.2) -- the Data/MC check of the DNN inputs that the AN shows after the baseline (AN §6.3-6.4). Only trees written by
#   an analyzer with Tree v1 have these columns (an older tree: TREE FAIL with the missing column). -1 = undefined
#   (fewer objects than the quantity needs) lands in the underflow.
TREE_HISTS_V1 = [
    ("invMassHadW", "m_{qq} [GeV] (closest to m_{W}; the QCD-region axis)", "invMassHadW", 44, 30.0, 250.0),
    ("chi2Higgs", "#chi^{2}_{HH} (AN jet choice)", "chi2Higgs", 50, 0.0, 500.0),
    ("invMassH1", "m_{H1} [GeV] (closest to m_{H})", "invMassH1", 30, 0.0, 300.0),
    ("invMassH2", "m_{H2} [GeV]", "invMassH2", 30, 0.0, 300.0),
    ("PTH1", "p_{T}(H1) [GeV]", "PTH1", 30, 0.0, 600.0),
    ("PTH2", "p_{T}(H2) [GeV]", "PTH2", 30, 0.0, 600.0),
    ("chi2Z", "#chi^{2}_{ZZ}", "chi2Z", 50, 0.0, 500.0),
    ("chi2HiggsZ", "#chi^{2}_{ZH}", "chi2HiggsZ", 50, 0.0, 500.0),
    ("bjetHT", "H_{T}^{b} [GeV]", "bjetHT", 40, 0.0, 2000.0),
    ("lightjetHT", "H_{T}^{light} [GeV]", "lightjetHT", 40, 0.0, 2000.0),
    ("lightjetNumber", "number of light jets (score < L)", "lightjetNumber", 12, -0.5, 11.5),
    ("nLooseJets", "number of jets with score #geq L", "nLooseJets", 13, -0.5, 12.5),
    ("jetAverageMass", "m_{j}^{avg} [GeV]", "jetAverageMass", 30, 0.0, 60.0),
    ("bjetAverageMass", "m_{b}^{avg} [GeV]", "bjetAverageMass", 30, 0.0, 60.0),
    ("bjetAverageMassSqr", "(m^{2})_{b}^{avg} [GeV^{2}]", "bjetAverageMassSqr", 30, 0.0, 3000.0),
    ("maxPTmassjjj", "m_{jjj}^{max p_{T}} [GeV]", "maxPTmassjjj", 30, 0.0, 1500.0),
    ("maxPTmassjbb", "m_{jbb}^{max p_{T}} [GeV]", "maxPTmassjbb", 30, 0.0, 1500.0),
    ("averageDeltaRjj", "#DeltaR_{jj}^{avg}", "averageDeltaRjj", 25, 0.0, 5.0),
    ("averageDeltaRbb", "#DeltaR_{bb}^{avg}", "averageDeltaRbb", 25, 0.0, 5.0),
    ("minDeltaRjj", "#DeltaR_{jj}^{min}", "minDeltaRjj", 30, 0.0, 3.0),
    ("minDeltaRbb", "#DeltaR_{bb}^{min}", "minDeltaRbb", 40, 0.0, 4.0),
    ("averageDeltaEtajj", "#Delta#eta_{jj}^{avg}", "averageDeltaEtajj", 30, 0.0, 3.0),
    ("averageDeltaEtabb", "#Delta#eta_{bb}^{avg}", "averageDeltaEtabb", 30, 0.0, 3.0),
    ("maxDeltaEtabb", "#Delta#eta_{bb}^{max}", "maxDeltaEtabb", 30, 0.0, 5.0),
    ("minDeltaRMassbb", "m_{bb}^{min #DeltaR} [GeV]", "minDeltaRMassbb", 30, 0.0, 300.0),
    ("minDeltaRpTbb", "p_{T,bb}^{min #DeltaR} [GeV]", "minDeltaRpTbb", 30, 0.0, 800.0),
    ("H2", "Fox-Wolfram H_{2} (jets)", "H2", 25, 0.0, 1.0),
    ("H3", "Fox-Wolfram H_{3} (jets)", "H3", 25, -0.5, 0.5),
    ("bjetH2", "Fox-Wolfram H_{2} (b jets)", "bjetH2", 25, 0.0, 1.0),
    ("centrality", "centrality (jets)", "centrality", 25, 0.0, 1.0),
    ("bjetCentrality", "centrality (b jets)", "bjetCentrality", 25, 0.0, 1.0),
]


def submitter():
    src = os.path.join(REPO, "submit_job_FH_Tier3_unified.py")
    spec = importlib.util.spec_from_file_location("tthh_submitter", src)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def sf_note(base, year=""):
    """The SF state from the output directory name (the submitter's suffixes: _notrig, _btagsf, _btagrw). [STEP 25 K]
    2024: the b-tag SF is the fixed-WP weight (D-2026-10-08-A; b jets only, c / light jets SF 1)."""
    name = os.path.basename(os.path.normpath(base))
    trig = "_notrig" not in name
    btag = "_btagsf" in name
    on = [x for x, o in (("trigger SF", trig), ("b-tag SF (fixed WP, b jets)" if year == "2024" else "b-tag SF", btag))
          if o]
    off = [x for x, o in (("trigger", trig), ("b-tag", btag)) if not o]
    if not on:
        return "no %s SF" % "/".join(off)
    return " + ".join(on) + " applied" + (", no %s SF" % "/".join(off) if off else "")   # (";" splits note lines)


def cutflow(ROOT, path):
    """(labels, weighted values of Tree/cutflow_w, unweighted noCut of Tree/cutflow or None), or None"""
    f = ROOT.TFile.Open(path)
    if not f or f.IsZombie():
        return None
    try:
        h = f.Get("Tree/cutflow_w")
        if not h:
            return None
        n = h.GetNbinsX()
        raw = f.Get("Tree/cutflow")
        return ([h.GetXaxis().GetBinLabel(i) for i in range(1, n + 1)],
                [h.GetBinContent(i) for i in range(1, n + 1)],
                raw.GetBinContent(1) if raw else None)
    finally:
        f.Close()


def count_differs(main_n, pre_n):
    """True when the merged unweighted noCut and the prescan event count differ by more than the float rounding of a
    TH1F summed by hadd (each job's count is exact below 2^24; the sum of up to a few hundred jobs is rounded to the
    float spacing, a few hundred events at 1e8) -- one input file is always far more than that."""
    return abs(main_n - pre_n) > max(2.0, 1e-5 * pre_n)


def th1_paths(ROOT, path, inc, exc):
    """[(key_path, class, title, nbins, xlow, xhigh)] of the TH1s in the file, in key order"""
    out = []
    f = ROOT.TFile.Open(path)

    def walk(d, prefix):
        for k in d.GetListOfKeys():
            cls = k.GetClassName()
            name = k.GetName()
            if cls.startswith("TDirectory"):
                if prefix + name == "BTagEff":
                    continue        # [STEP 25 K] the b-tag efficiency inputs (MC only; tools/stage7/btag_eff_maps.py)
                walk(k.ReadObj(), prefix + name + "/")
                continue
            if not cls.startswith("TH1"):
                continue
            kp = prefix + name
            if inc and not any(re.search(r, kp) for r in inc):
                continue
            if any(re.search(r, kp) for r in exc):
                continue
            h = k.ReadObj()
            ax = h.GetXaxis()
            out.append((kp, cls, h.GetTitle(), ax.GetNbins(), ax.GetXmin(), ax.GetXmax()))
    walk(f, "")
    f.Close()
    return out


def tree_hists(ROOT, path, out_path, cut, is_data, v1=False):
    """[D-2026-10-06-A] TREE_HISTS from Tree/Tree of path for the events passing cut, into out_path (directory
    Control/, TH1F like the analyzer's histograms). Returns (events, weighted sum, flavour table input) or the reason
    it could not. The table input [STEP 24 I]: {"all": per score bin the weighted count of every selected jet} and, for
    MC with the hadFlavs branch, the same for "b", "c", "light"."""
    f = ROOT.TFile.Open(path)
    if not f or f.IsZombie():
        return "cannot open %s" % path
    t = f.Get("Tree/Tree")
    ok = bool(t) and t.InheritsFrom("TTree")
    has_flav = bool(ok and t.GetBranch("hadFlavs"))
    f.Close()
    if not ok:
        return "no Tree/Tree in %s" % path
    try:
        # the derived columns first (lazy), so that the cut may use them too (e.g. 'btag_3 > 0.5')
        d = ROOT.RDataFrame("Tree/Tree", path).Define("w_tree_", "1.0" if is_data else "(double)evtWeight")
        for i in range(1, 7):
            d = d.Define("jet_pt_%d" % i, "jetPt.size() >= %d ? (double)jetPt[%d] : -999." % (i, i - 1))
            d = d.Define("jet_eta_%d" % i, "jetEta.size() >= %d ? (double)jetEta[%d] : -999." % (i, i - 1))
        d = d.Define("btag_desc_", "ROOT::VecOps::Reverse(ROOT::VecOps::Sort(bTagScore))")
        for i in range(1, 5):
            d = d.Define("btag_%d" % i, "btag_desc_.size() >= %d ? (double)btag_desc_[%d] : -999." % (i, i - 1))
        # [STEP 24 I] jet-multiplicity diagnostics (TREE_HISTS); -999 = not this b-tag multiplicity (underflow)
        d = (d.Define("njet_pt40_", "(int)ROOT::VecOps::Sum(jetPt > 40.f)")
              .Define("njet_pt50_", "(int)ROOT::VecOps::Sum(jetPt > 50.f)")
              .Define("njet_central_", "(int)ROOT::VecOps::Sum(ROOT::VecOps::abs(jetEta) < 2.0f)")
              .Define("njet_forward_", "(int)ROOT::VecOps::Sum(ROOT::VecOps::abs(jetEta) >= 2.0f)")
              .Define("njet_nb2_", "nbJets == 2 ? (double)nJets : -999.")
              .Define("njet_nb3p_", "nbJets >= 3 ? (double)nJets : -999."))
        if has_flav and not is_data:
            d = (d.Define("btag_flav_b_", "bTagScore[hadFlavs == 5]")
                  .Define("btag_flav_c_", "bTagScore[hadFlavs == 4]")
                  .Define("btag_flav_l_", "bTagScore[hadFlavs != 5 && hadFlavs != 4]"))
        d = d.Filter(cut, "tree_cut")
        booked = [(name, title, d.Histo1D(ROOT.RDF.TH1DModel("tree_" + name, "", nb, lo, hi), col, "w_tree_"))
                  for name, title, col, nb, lo, hi in TREE_HISTS + (TREE_HISTS_V1 if v1 else [])]
        flav = {"all": d.Histo1D(ROOT.RDF.TH1DModel("flav_all_", "", FLAV_BINS, 0.0, 1.0), "bTagScore", "w_tree_")}
        if has_flav and not is_data:
            for k, col in (("b", "btag_flav_b_"), ("c", "btag_flav_c_"), ("light", "btag_flav_l_")):
                flav[k] = d.Histo1D(ROOT.RDF.TH1DModel("flav_%s_" % k, "", FLAV_BINS, 0.0, 1.0), col, "w_tree_")
        n_ev, sumw = d.Count(), d.Sum("w_tree_")
        n_ev, sumw = int(n_ev.GetValue()), float(sumw.GetValue())   # one event loop for everything booked
        # a score of exactly 1 belongs to the last bin (the overflow of [0, 1))
        flav = {k: [h.GetValue().GetBinContent(b) for b in range(1, FLAV_BINS)]
                + [h.GetValue().GetBinContent(FLAV_BINS) + h.GetValue().GetBinContent(FLAV_BINS + 1)]
                for k, h in flav.items()}
    except Exception as e:      # a bad expression (cling; its own error lines are on stderr) or a missing branch
        lines = [x.strip() for x in str(e).strip().splitlines() if x.strip()]
        return "cut '%s' or the tree columns: %s" % (cut, " | ".join(lines[:3] + lines[-1:]) if lines else repr(e))
    o = ROOT.TFile(out_path, "RECREATE")
    o.mkdir("Control").cd()
    for name, title, h in booked:
        hd = h.GetValue()
        hf = ROOT.TH1F(name, title, hd.GetNbinsX(), hd.GetXaxis().GetXmin(), hd.GetXaxis().GetXmax())
        hf.Sumw2()
        for b in range(hd.GetNbinsX() + 2):
            hf.SetBinContent(b, hd.GetBinContent(b))
            hf.SetBinError(b, hd.GetBinError(b))
        hf.SetEntries(hd.GetEntries())
        hf.GetXaxis().SetTitle(title)
        hf.Write()
    o.Close()
    return n_ev, sumw, flav


def flavour_table(mc_flav, data_flav):
    """[STEP 24 I] TREEFLAV lines: per b-tag score bin, every selected jet -- the MC sum and its split by hadron flavour,
    the Data count, Data/MC. mc_flav: the tree_hists tables of the MC samples; data_flav: those of the Data samples."""
    if not mc_flav:
        return []
    with_flav = [m for m in mc_flav if "b" in m]
    out = ["TREEFLAV score    %12s %7s %7s %7s %10s %8s   (every selected jet; MC split by hadron flavour%s)"
           % ("MC", "b", "c", "light", "Data", "Data/MC",
              "" if len(with_flav) == len(mc_flav) else ", %d of %d MC samples have it" % (len(with_flav), len(mc_flav)))]
    for i in range(FLAV_BINS):
        m = sum(t["all"][i] for t in mc_flav)
        fl = [sum(t[k][i] for t in with_flav) for k in ("b", "c", "light")]
        tot = sum(fl)
        dat = sum(t["all"][i] for t in data_flav)
        out.append("TREEFLAV %.1f-%.1f %12.2f %s %10.0f %8s"
                   % (i / float(FLAV_BINS), (i + 1) / float(FLAV_BINS), m,
                      " ".join("%6.1f%%" % (100.0 * x / tot) if tot > 0 else "%7s" % "-" for x in fl), dat,
                      "%.3f" % (dat / m) if m > 0 else "nan"))
    return out


def yq(v):
    """yaml scalar"""
    if isinstance(v, (int, float)):
        return repr(v)
    s = str(v)
    if s == "" or any(c in s for c in ":#[]{},&*!|>'\"%@`") or s.lower() in ("yes", "no", "true", "false", "null", "~"):
        return "'" + s.replace("'", "''") + "'"
    return s


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--config", required=True)
    ap.add_argument("--base", required=True)
    ap.add_argument("--out")
    ap.add_argument("--grouping", default="both", choices=["compact", "detailed", "both"])
    ap.add_argument("--exclude", nargs="*", default=[])
    ap.add_argument("--no-default-exclude", action="store_true")
    ap.add_argument("--include-hist", nargs="*", default=[])
    ap.add_argument("--exclude-hist", nargs="*", default=[])
    ap.add_argument("--note")
    ap.add_argument("--allow-missing", action="store_true")
    ap.add_argument("--check-only", action="store_true")
    ap.add_argument("--tree-cut", help="control plots from Tree/Tree: the events passing this RDataFrame expression "
                                       "(docstring 6)")
    ap.add_argument("--tree-label", help="with --tree-cut: one more line of the plot note (TLatex)")
    ap.add_argument("--tree-v1", action="store_true", help="with --tree-cut: also the Tree v1 ML inputs (TREE_HISTS_V1)")
    a = ap.parse_args(argv)
    if a.tree_v1 and not a.tree_cut:
        print("ERROR --tree-v1 needs --tree-cut (use --tree-cut 'true' for every event of the tree)")
        return 2
    if a.tree_label and not a.tree_cut:
        print("ERROR --tree-label needs --tree-cut")
        return 2
    if a.out:
        a.out = os.path.abspath(a.out)     # before the chdir below; the plotter runs in it and reads the tree/ files
    if a.tree_cut and not a.tree_label:
        a.tree_label = "event-tree cut: see YIELDS.txt"   # the expression itself is not TLatex-safe (its '_')

    cfg = a.config if os.path.isabs(a.config) else os.path.join(REPO, a.config)
    if not os.path.isfile(cfg) or not os.path.isdir(a.base):
        print("ERROR no config %s or no base directory %s" % (cfg, a.base))
        return 2
    sub = submitter()
    conf = sub.CondorJobManager.load_yaml_config(None, cfg)
    common = conf["common"]
    year = str(common.get("year", "")).strip()
    try:
        lumi = float(common["lumi_fb_inv"])
    except (KeyError, TypeError, ValueError):
        print("ERROR %s has no numeric common.lumi_fb_inv" % cfg)
        return 2
    names = [(s["sample_name"] if isinstance(s, dict) else str(s)) for s in conf["samples"]]
    excl = set(a.exclude) | (set() if a.no_default_exclude else set(DEFAULT_EXCLUDE.get(year, [])))
    sqrts = SQRTS.get(year, "13")
    note = ";".join(x for x in ((a.note,) if a.note is not None else (YEAR_NOTE.get(year, year), sf_note(a.base, year)))
                    + ((a.tree_label,) if a.tree_cut else ()) if x)
    print("CONFIG %s year=%s lumi=%.3f sqrt(s)=%s TeV samples=%d" % (cfg, year, lumi, sqrts, len(names)))
    print("BASE %s" % a.base)
    print("NOTE %s" % note)

    import ROOT
    ROOT.gROOT.SetBatch(True)
    ROOT.gErrorIgnoreLevel = ROOT.kError
    samples, bad = [], []
    for n in names:
        kind = "DATA" if sub.is_data_name(n) else "MC"
        if n in excl:
            why = "D-2026-10-05-C: not stacked on the inclusive ttbar in %s" % year if n in DEFAULT_EXCLUDE.get(year, []) \
                else "--exclude"
            print("EXCLUDED %s (%s)" % (n, why))
            continue
        p = os.path.join(a.base, n + ".root")
        cf = cutflow(ROOT, p) if os.path.isfile(p) else None
        if cf is None:
            print("MISSING %s %s (%s)" % (kind, p, "no file" if not os.path.isfile(p) else "no Tree/cutflow_w"))
            bad.append(n)
            continue
        samples.append((n, kind, p, cf))
        print("SAMPLE %-4s %-45s %s" % (kind, n, p))
    if bad and not a.allow_missing:
        print("RESULT FAIL (%d sample(s) missing: %s; merge them, or --allow-missing)" % (len(bad), " ".join(bad[:8])))
        return 1
    mc = [s for s in samples if s[1] == "MC"]
    data = [s for s in samples if s[1] == "DATA"]
    if not mc:
        print("RESULT FAIL (no MC sample)")
        return 1
    labels = mc[0][3][0]
    for s in samples:
        if s[3][0] != labels:
            print("RESULT FAIL (%s has other cutflow bins: %s)" % (s[0], s[3][0]))
            return 1

    # ---- before the skim (MC): the submitter's weight x the prescan sums -------------------------------------
    gen = {}
    pre_events = {}
    os.chdir(REPO)     # the submitter's loaders open common.xsec_db and common.prescan relative to the repository
    calc = object.__new__(sub.CondorJobManager)
    for n, kind, p, cf in mc:
        try:
            with contextlib.redirect_stdout(io.StringIO()):
                w, _ = calc._compute_base_weight(n, common)
                rec = calc._load_prescan(common["prescan"])[n]
            sr, se = float(rec["runs"]["genEventSumw"]), float(rec["events"]["sumGenW_total"])
            gen[n] = (w * sr, (se / sr) if sr else float("nan"), w * se)
            pre_events[n] = int(rec["events"]["nEvents_total"])
        except (SystemExit, Exception):     # no prescan summary / sample in it: no pre-skim numbers for it
            gen[n] = None
    have_gen = [n for n in gen if gen[n] is not None]

    # ---- yields ---------------------------------------------------------------------------------------------
    lines = []
    W = lines.append
    msum = [sum(s[3][1][i] for s in mc) for i in range(len(labels))]
    dsum = [sum(s[3][1][i] for s in data) for i in range(len(labels))]
    W("YIELDS %s (Tree/cutflow_w; MC %d samples, Data %d; lumi %.3f fb-1)" % (os.path.basename(os.path.normpath(a.base)),
                                                                              len(mc), len(data), lumi))
    W("YIELD %-16s %14s %14s %8s" % ("step", "MC", "Data", "Data/MC"))
    if have_gen:
        W("YIELD %-16s %14.1f %14s %8s   (before the skim; MC samples with a prescan record: %d of %d)"
          % ("generated", sum(gen[n][0] for n in have_gen), "-", "-", len(have_gen), len(mc)))
    for i, lab in enumerate(labels):
        r = (dsum[i] / msum[i]) if msum[i] > 0 else float("nan")
        W("YIELD %-16s %14.1f %14.0f %8.3f" % (lab, msum[i], dsum[i], r))
    idx = {lab: i for i, lab in enumerate(labels)}
    cols = [c for c in KEY_STEPS if c in idx] + [labels[-1]]
    W("SAMPLEYIELD %-30s %12s %8s %12s %9s " % ("sample (MC, by yield at %s)" % cols[0], "generated", "skimEff", "noCut",
                                                 "noCut/exp") + " ".join("%12s" % c for c in cols))
    flagged, warned = [], []
    for s in sorted(mc, key=lambda s: -s[3][1][idx[cols[0]]]):
        v = s[3][1]
        if not v[0] > 0:
            flagged.append(s[0])
        g = gen.get(s[0])
        ratio = (v[0] / g[2]) if (g and g[2] > 0) else float("nan")
        if g and not (0.7 <= ratio <= 1.3):
            warned.append((s[0], ratio))
        W("SAMPLEYIELD %-30s %12s %8s %12.1f %9s " % (s[0], "%.1f" % g[0] if g else "-", "%.3f" % g[1] if g else "-", v[0],
                                                     "%.3f" % ratio if g else "-") + " ".join("%12.2f" % v[idx[c]] for c in cols))
    for n in flagged:
        W("FLAG %s: noCut yield <= 0 (weight, xsec or prescan problem?)" % n)
    # exact completeness (MC): merged unweighted noCut == the prescan's event count (docstring 3)
    evchk = [(s[0], s[3][2], pre_events[s[0]]) for s in mc if s[0] in pre_events and s[3][2] is not None]
    evbad = [(n, m, pr) for n, m, pr in evchk if count_differs(m, pr)]
    W("EVENTS MC %d of %d samples: merged noCut (unweighted) vs the prescan event count -- %s"
      % (len(evchk), len(mc), "all equal" if not evbad else "%d differ (FLAG)" % len(evbad)))
    for n, m, pr in evbad:
        W("FLAG %s: merged noCut %d events, prescan %d (%+.3f%%): a job or input file missing from the merge, a job "
          "that skipped an input file it could not open, or other filelists than at prescan"
          % (n, m, pr, 100.0 * (m - pr) / pr if pr else float("nan")))
        if n not in flagged:
            flagged.append(n)
    for n, r in warned:
        W("WARN %s: noCut / (weight x Sigma genw(Events)) = %.3f -- about the mean PU weight when the merged file holds "
          "every main job once: %s" % (n, r, "jobs missing from the merge?" if r < 1 else "jobs merged twice?"))
    out = a.out or os.path.join(REPO, "condor", "plots",
                                "%s%s_%s" % (os.path.basename(os.path.normpath(a.base)), "_tree" if a.tree_cut else "",
                                             datetime.datetime.utcnow().strftime("%Y%m%d_%H%M%S")))
    os.makedirs(out, exist_ok=True)
    with open(os.path.join(out, "YIELDS.txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    print("\n".join(lines))
    print("OUT %s" % out)
    wtag = (", %d WARN" % len(warned)) if warned else ""
    if a.check_only:
        print("RESULT %s (check only%s)" % ("OK" if not flagged else "FAIL (%d flagged)" % len(flagged), wtag))
        return 0 if not flagged else 1

    # ---- [D-2026-10-06-A] --tree-cut: control histograms from Tree/Tree (docstring 6) --------------------------------
    plot_samples = samples
    if a.tree_cut:
        tdir = os.path.join(out, "tree")
        os.makedirs(tdir, exist_ok=True)
        tl = ["TREECUT %s (Tree/Tree of each merged file; MC weighted by evtWeight, Data by 1)" % a.tree_cut]
        plot_samples, tot = [], {"MC": 0.0, "DATA": 0.0}
        flav_tab = {"MC": [], "DATA": []}
        for n, kind, p, cf in samples:
            tp = os.path.join(tdir, n + ".root")
            r = tree_hists(ROOT, p, tp, a.tree_cut, kind == "DATA", a.tree_v1)
            if isinstance(r, str):
                print("\n".join(tl))
                print("TREE FAIL %s: %s" % (n, r))
                print("RESULT FAIL (--tree-cut)")
                return 1
            tl.append("TREEYIELD %-4s %-45s events=%d sumw=%.2f" % (kind, n, r[0], r[1]))
            tot[kind] += r[1]
            flav_tab[kind].append(r[2])
            plot_samples.append((n, kind, tp, cf))
        tl.append("TREEYIELD total MC=%.1f Data=%.0f Data/MC=%s" % (
            tot["MC"], tot["DATA"], "%.3f" % (tot["DATA"] / tot["MC"]) if tot["MC"] > 0 else "nan"))
        tl += flavour_table(flav_tab["MC"], flav_tab["DATA"])
        print("\n".join(tl))
        with open(os.path.join(out, "YIELDS.txt"), "a") as f:
            f.write("\n".join(tl) + "\n")

    # ---- structure_info.yml and samples_config.yml --------------------------------------------------------------
    pmc = [s for s in plot_samples if s[1] == "MC"]
    ref = next((s[2] for s in pmc if s[0] == "TTbar_Hadronic"), pmc[0][2])
    hists = th1_paths(ROOT, ref, a.include_hist, a.exclude_hist)
    print("HIST n=%d from %s" % (len(hists), ref))
    if not hists:
        print("RESULT FAIL (no histogram selected)")
        return 1
    with open(os.path.join(out, "structure_info.yml"), "w") as f:
        f.write("description: %s\ninput_file: %s\ntotal_count: %d\nhistograms:\n"
                % (yq("Plottable histograms extracted from %s (plotter/make_plots.py)" % ref), yq(ref), len(hists)))
        for kp, cls, title, nb, lo, hi in hists:
            f.write("  - key_path: %s\n    classname: %s\n    title: %s\n    nbins: %d\n    xlow: %s\n    xhigh: %s\n"
                    % (yq(kp), cls, yq(title), nb, repr(float(lo)), repr(float(hi))))
    with open(os.path.join(out, "samples_config.yml"), "w") as f:
        f.write("description: 'plotter/make_plots.py: %s'\nsamples:\n" % os.path.basename(cfg))
        for n, kind, p, _ in plot_samples:
            f.write("  %s:\n    type: %s\n    path: %s\n    files:\n      - %s\n    label: %s\n    color: '%s'\n"
                    % (n, kind, os.path.dirname(p), p, "Data" if kind == "DATA" else n,
                       "#000000" if kind == "DATA" else "#BDC3C7"))
    shutil.copy(os.path.join(HERE, "stack_plotter.C"), os.path.join(out, "stack_plotter.C"))

    # ---- the plotter --------------------------------------------------------------------------------------------
    ok = True
    for grp in (["compact", "detailed"] if a.grouping == "both" else [a.grouping]):
        env = dict(os.environ, TTHH_PLOT_GROUPING=grp, TTHH_PLOT_LUMI="%.6g" % lumi, TTHH_PLOT_SQRTS=sqrts,
                   TTHH_PLOT_NOTE=note, TTHH_PLOT_MULTIPAGE="1")
        log = os.path.join(out, "plotter_%s.log" % grp)
        with open(log, "w") as lf:
            rc = subprocess.call(["root", "-l", "-b", "-q", "stack_plotter.C"], cwd=out, env=env,
                                 stdout=lf, stderr=subprocess.STDOUT)
        pdir = os.path.join(out, "plots_" + grp)
        pdfs = [x for x in os.listdir(pdir) if x.endswith(".pdf")] if os.path.isdir(pdir) else []
        multi = os.path.join(pdir, "all_%s.pdf" % grp)
        n_single = len([x for x in pdfs if not x.startswith("all_")])
        good = rc == 0 and n_single > 0 and os.path.isfile(multi)
        print("PLOTS %s rc=%d pdf=%d multipage=%s log=%s" % (grp, rc, n_single, multi if os.path.isfile(multi) else "-", log))
        if not good:
            ok = False
            with open(log) as lf:
                tail = lf.read().splitlines()[-15:]
            print("\n".join("    " + t for t in tail))
        warn = [l.strip() for l in open(log) if "not in grouping table" in l]
        for w in sorted(set(warn)):
            print("GROUPING_WARN %s" % w)
    print("RESULT %s%s" % ("OK" if ok and not flagged else "FAIL" + (" (plotter)" if not ok else " (%d flagged)" % len(flagged)),
                           (" (%d WARN)" % len(warned)) if warned else ""))
    return 0 if ok and not flagged else 1


if __name__ == "__main__":
    sys.exit(main())
