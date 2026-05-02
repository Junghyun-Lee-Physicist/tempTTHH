#!/usr/bin/env python3
"""
plot_btag_pyroot.py

Per-sample b-tag reweight closure plot (PyROOT version of PlotBTagReweight.C).

Reads bTagReweight_<sample>.root files (produced by exe_BTagSF) and produces
3-tier closure overlay (noSF / withSF / reweighted) + ratio panel for each
of 9 variables, as PDF.

This is the *sample-local* closure (sample-internal yield preservation check).
For the true *group-level* ratio map, see plot_group_ratios.py.

Usage:
    python3 plot_btag_pyroot.py TTToHadronic
    python3 plot_btag_pyroot.py all
    python3 plot_btag_pyroot.py TTToHadronic ttHH tt4b

Author: Junghyun Lee
"""

import os
import sys
import ROOT

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)


# ─── Sample lists ──────────────────────────────────────────────────────────
ALL_MC = [
    "TTToHadronic", "TTTo2L2Nu", "TTToSemiLeptonic",
    "ttHH", "tt4b", "ttHtobb", "tttt", "tttW",
    "ttWH", "ttWW", "ttWZ", "ttZHto4b", "ttZtobb", "ttZZto4b", "ttbb",
    "QCD_HT200to300", "QCD_HT300to500", "QCD_HT500to700",
    "QCD_HT700to1000", "QCD_HT1000to1500", "QCD_HT1500to2000", "QCD_HT2000toInf",
]

ALL_DATA = [
    "BTagCSV_B", "BTagCSV_C", "BTagCSV_D", "BTagCSV_E", "BTagCSV_F",
    "JetHT_B",   "JetHT_C",   "JetHT_D",   "JetHT_E",   "JetHT_F",
]

FILE_PREFIX = "bTagReweight_"
OUT_DIR     = "plots_root"


# ─── Variable definitions ──────────────────────────────────────────────────
# (hist_noSF, hist_withSF, hist_reweighted, plotName, xTitle, xLow, xHigh)
VARIABLES = [
    ("h_nJets_noSF",    "h_nJets_withSF",    "h_nJets_reweighted",
     "nJets",  "nJets",       6.0, 18.0),

    ("h_HT_noSF",       "h_HT_withSF",       "h_HT_reweighted",
     "HT",     "HT [GeV]",  500.0, 2500.0),

    ("h_nbJets_noSF",   "h_nbJets_withSF",   "h_nbJets_reweighted",
     "nbJets", "n_{b-jets}",  0.0, 9.0),

    ("PerJet/h_jetPt_noSF_jet0",  "PerJet/h_jetPt_withSF_jet0",
     "PerJet/h_jetPt_reweighted_jet0",
     "jet0_pT",  "Leading jet p_{T} [GeV]",  30.0, 500.0),

    ("PerJet/h_bTag_noSF_jet0",   "PerJet/h_bTag_withSF_jet0",
     "PerJet/h_bTag_reweighted_jet0",
     "jet0_bTag", "Leading jet DeepJet",  0.0, 1.0),

    ("PerJet/h_jetPt_noSF_jet1",  "PerJet/h_jetPt_withSF_jet1",
     "PerJet/h_jetPt_reweighted_jet1",
     "jet1_pT",  "Sub-leading jet p_{T} [GeV]",  30.0, 500.0),

    ("PerJet/h_bTag_noSF_jet1",   "PerJet/h_bTag_withSF_jet1",
     "PerJet/h_bTag_reweighted_jet1",
     "jet1_bTag", "Sub-leading jet DeepJet",  0.0, 1.0),

    ("PerJet/h_jetPt_noSF_jet5",  "PerJet/h_jetPt_withSF_jet5",
     "PerJet/h_jetPt_reweighted_jet5",
     "jet5_pT",  "6th jet p_{T} [GeV]",  30.0, 250.0),

    ("PerJet/h_bTag_noSF_jet5",   "PerJet/h_bTag_withSF_jet5",
     "PerJet/h_bTag_reweighted_jet5",
     "jet5_bTag", "6th jet DeepJet",  0.0, 1.0),
]


# ─── CMS-style label helpers (kept-alive list for ROOT GC safety) ──────────
_keepalive = []


def draw_cms_label():
    latex = ROOT.TLatex()
    latex.SetNDC()
    latex.SetTextSize(0.060)
    latex.SetTextAlign(11)
    latex.DrawLatex(0.10, 0.925,
        "CMS#scale[0.5]{ }#scale[0.85]{#font[52]{Private work}}")
    _keepalive.append(latex)


def draw_lumi_label():
    latex = ROOT.TLatex()
    latex.SetNDC()
    latex.SetTextSize(0.050)
    latex.SetTextAlign(31)
    latex.DrawLatex(0.92, 0.925, "#bf{41.5 fb^{-1} (13 TeV, 2017)}")
    _keepalive.append(latex)


def draw_sample_label(sample_name):
    pt = ROOT.TPaveText(0.15, 0.72, 0.48, 0.85, "NDC")
    pt.AddText("#it{ttHH} hadronic channel")
    pt.AddText("#it{%s}" % sample_name)
    pt.SetTextSize(0.045)
    pt.SetFillColor(0)
    pt.SetBorderSize(0)
    pt.Draw()
    _keepalive.append(pt)


# ─── Drawing ───────────────────────────────────────────────────────────────
def draw_variable(h_no, h_sf, h_rew,
                  plot_name, x_title, x_low, x_high,
                  sample_name, out_dir):
    """3-tier overlay (noSF/withSF/reweighted) + ratio panel → PDF."""
    if not h_no or not h_sf or not h_rew:
        print("  [WARN] Missing histogram for {} in {}".format(plot_name, sample_name))
        return

    # Detach from input file (so they survive after f.Close())
    h_no.SetDirectory(0)
    h_sf.SetDirectory(0)
    h_rew.SetDirectory(0)

    h_no.Sumw2();  h_sf.Sumw2();  h_rew.Sumw2()
    h_no.GetXaxis().SetRangeUser(x_low, x_high)
    h_sf.GetXaxis().SetRangeUser(x_low, x_high)
    h_rew.GetXaxis().SetRangeUser(x_low, x_high)

    canvas = ROOT.TCanvas("c", "", 800, 800)
    canvas.SetLeftMargin(0.125)
    canvas.Divide(1, 2)

    # ── Upper pad: overlay ────────────────────────────────────────────────
    canvas.cd(1)
    p1 = ROOT.gPad
    p1.SetPad(0.0, 0.35, 1.0, 1.0)
    p1.SetLogy(True)
    p1.SetBottomMargin(0.005)
    p1.SetGridx()

    h_no.SetLineColor(ROOT.kBlack)
    h_no.SetLineWidth(2)
    h_no.SetMarkerStyle(22)
    h_no.SetMarkerColor(ROOT.kBlack)
    h_no.SetMarkerSize(0.9)

    h_sf.SetLineColor(ROOT.kBlue)
    h_sf.SetLineWidth(2)
    h_sf.SetMarkerStyle(20)
    h_sf.SetMarkerColor(ROOT.kBlue)
    h_sf.SetMarkerSize(0.9)

    h_rew.SetLineColor(ROOT.kRed)
    h_rew.SetLineWidth(2)
    h_rew.SetMarkerStyle(23)
    h_rew.SetMarkerColor(ROOT.kRed)
    h_rew.SetMarkerSize(0.9)

    y_max = max(h_no.GetMaximum(), h_sf.GetMaximum(), h_rew.GetMaximum())
    if y_max <= 0:
        print("  [WARN] All-empty histograms for {} in {}".format(plot_name, sample_name))
        canvas.Close()
        return

    h_no.SetMaximum(y_max * 20.0)
    h_no.SetMinimum(max(0.1, y_max * 1e-5))
    h_no.GetYaxis().SetTitle("Events")
    h_no.GetYaxis().SetTitleSize(0.060)
    h_no.GetYaxis().SetTitleOffset(0.75)
    h_no.GetYaxis().SetLabelSize(0.045)
    h_no.GetXaxis().SetTitleSize(0)
    h_no.GetXaxis().SetLabelSize(0)

    h_no.Draw("E")
    h_sf.Draw("E SAME")
    h_rew.Draw("E SAME")

    leg = ROOT.TLegend(0.52, 0.60, 0.88, 0.85)
    leg.AddEntry(h_no,  "#bf{no b-tag SF}",          "pe")
    leg.AddEntry(h_sf,  "#bf{b-tag SF}",             "pe")
    leg.AddEntry(h_rew, "#bf{b-tag SF + reweight}",  "pe")
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)
    leg.SetTextFont(42)
    leg.Draw()
    _keepalive.append(leg)

    draw_cms_label()
    draw_lumi_label()
    draw_sample_label(sample_name)

    # ── Lower pad: ratio to noSF ──────────────────────────────────────────
    canvas.cd(2)
    p2 = ROOT.gPad
    p2.SetPad(0.0, 0.0, 1.0, 0.35)
    p2.SetBottomMargin(0.30)
    p2.SetGridx()
    p2.SetGridy()

    r_sf = h_sf.Clone("r_sf_" + plot_name)
    r_sf.SetDirectory(0)
    r_sf.Divide(h_no)
    r_sf.SetLineColor(ROOT.kBlue)
    r_sf.SetMarkerColor(ROOT.kBlue)
    r_sf.SetMarkerStyle(20)
    r_sf.SetMarkerSize(0.8)

    r_rew = h_rew.Clone("r_rew_" + plot_name)
    r_rew.SetDirectory(0)
    r_rew.Divide(h_no)
    r_rew.SetLineColor(ROOT.kRed)
    r_rew.SetMarkerColor(ROOT.kRed)
    r_rew.SetMarkerStyle(23)
    r_rew.SetMarkerSize(0.8)

    r_sf.SetMinimum(0.7)
    r_sf.SetMaximum(1.6)
    r_sf.GetXaxis().SetTitle(x_title)
    r_sf.GetXaxis().SetTitleSize(0.12)
    r_sf.GetXaxis().SetTitleOffset(1.0)
    r_sf.GetXaxis().SetLabelSize(0.09)
    r_sf.GetXaxis().SetLabelOffset(0.02)
    r_sf.GetYaxis().SetTitle("Ratio to no SF")
    r_sf.GetYaxis().SetTitleSize(0.09)
    r_sf.GetYaxis().SetTitleOffset(0.50)
    r_sf.GetYaxis().SetLabelSize(0.08)
    r_sf.GetYaxis().SetNdivisions(505)

    r_sf.Draw("E")
    r_rew.Draw("E SAME")

    line = ROOT.TLine(x_low, 1.0, x_high, 1.0)
    line.SetLineColor(ROOT.kBlack)
    line.SetLineStyle(2)
    line.Draw()
    _keepalive.append(line)

    # ── Save ──────────────────────────────────────────────────────────────
    pdf_path = "{}/{}_{}.pdf".format(out_dir, sample_name, plot_name.replace("/", "_"))
    canvas.SaveAs(pdf_path)
    canvas.Close()
    _keepalive.clear()   # release per-plot ROOT objects


def plot_one_sample(sample_name):
    """Draw all VARIABLES for one sample."""
    fname = FILE_PREFIX + sample_name + ".root"
    f = ROOT.TFile.Open(fname, "READ")
    if not f or f.IsZombie():
        print("  [WARN] Cannot open " + fname)
        if f:
            f.Close()
        return

    print(">>> Plotting: " + sample_name)

    if not os.path.exists(OUT_DIR):
        os.makedirs(OUT_DIR)

    nDrawn = 0
    for vd in VARIABLES:
        h_no_name, h_sf_name, h_rew_name, plot_name, x_title, x_low, x_high = vd

        h_no  = f.Get(h_no_name)
        h_sf  = f.Get(h_sf_name)
        h_rew = f.Get(h_rew_name)

        if not h_no or not h_sf or not h_rew:
            continue

        draw_variable(h_no, h_sf, h_rew,
                      plot_name, x_title, x_low, x_high,
                      sample_name, OUT_DIR)
        nDrawn += 1

    f.Close()
    print("  → {} PDFs in {}/".format(nDrawn, OUT_DIR))


# ─── Main ──────────────────────────────────────────────────────────────────
def main():
    if len(sys.argv) < 2:
        print("Usage: python3 plot_btag_pyroot.py <sample|all> [<sample> ...]")
        print("  e.g.: python3 plot_btag_pyroot.py TTToHadronic")
        print("        python3 plot_btag_pyroot.py all")
        print("        python3 plot_btag_pyroot.py TTToHadronic ttHH tt4b")
        sys.exit(1)

    samples = []
    for arg in sys.argv[1:]:
        if arg == "all":
            samples.extend(ALL_MC + ALL_DATA)
        else:
            samples.append(arg)

    print(">>> plot_btag_pyroot.py: {} sample(s)".format(len(samples)))
    print("")

    for s in samples:
        plot_one_sample(s)

    print("\n>>> Done. PDFs in {}/".format(OUT_DIR))


if __name__ == "__main__":
    main()
