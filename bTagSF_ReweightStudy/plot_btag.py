#!/usr/bin/env python3
"""
plot_btag.py

Read plotdata/<sample>.json files (produced by PlotBTagReweight.C)
and generate matplotlib plots as PNG.

Usage:
    python3 plot_btag.py TTbar_Hadronic
    python3 plot_btag.py all
    python3 plot_btag.py TTbar_Hadronic ttHH tt4b

Author: Junghyun Lee
"""

import json
import sys
import os
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from pathlib import Path

# ── All samples ──
ALL_MC = [
    "TTbar_Hadronic", "TTbar_DiLep", "TTbar_SemiLep",
    "TTHHto4b", "TT4b", "ttHTobb", "TTTT", "TTTW",
    "TTWH", "TTWW", "TTWZ", "TTZHTo4b", "TTZToBB", "TTZZTo4b", "ttbb",
    "QCD_HT200to300", "QCD_HT300to500", "QCD_HT500to700",
    "QCD_HT700to1000", "QCD_HT1000to1500", "QCD_HT1500to2000", "QCD_HT2000toInf",
]
ALL_DATA = [
    "BTagCSV_B", "BTagCSV_C", "BTagCSV_D", "BTagCSV_E", "BTagCSV_F",
    "JetHT_B",   "JetHT_C",   "JetHT_D",   "JetHT_E",   "JetHT_F",
]

PLOTDATA_DIR = "plotdata"
OUTPUT_DIR   = "plots_py"

# ── Plot axis labels ──
AXIS_LABELS = {
    "nJets":     "nJets",
    "HT":        r"$H_T$ [GeV]",
    "nbJets":    r"$n_{b\text{-jets}}$",
    "jet0_pT":   r"Leading jet $p_T$ [GeV]",
    "jet0_bTag": "Leading jet DeepJet",
    "jet1_pT":   r"Sub-leading jet $p_T$ [GeV]",
    "jet1_bTag": "Sub-leading jet DeepJet",
    "jet5_pT":   r"6th jet $p_T$ [GeV]",
    "jet5_bTag": "6th jet DeepJet",
}


def plot_one_variable(sample: str, varname: str, vardata: dict, outdir: str):
    """Draw overlay + ratio for one variable."""
    edges_noSF = np.array(vardata["noSF"]["edges"])
    centers = 0.5 * (edges_noSF[:-1] + edges_noSF[1:])

    y_noSF = np.array(vardata["noSF"]["bins"])
    y_SF   = np.array(vardata["withSF"]["bins"])
    y_ReW  = np.array(vardata["reweighted"]["bins"])

    e_noSF = np.array(vardata["noSF"]["errors"])
    e_SF   = np.array(vardata["withSF"]["errors"])
    e_ReW  = np.array(vardata["reweighted"]["errors"])

    # Ratio = SF/noSF, ReW/noSF  (safe division)
    with np.errstate(divide="ignore", invalid="ignore"):
        r_SF  = np.where(y_noSF > 0, y_SF  / y_noSF, 1.0)
        r_ReW = np.where(y_noSF > 0, y_ReW / y_noSF, 1.0)
        # Error propagation (simplified)
        re_SF  = np.where(y_noSF > 0, e_SF  / y_noSF, 0.0)
        re_ReW = np.where(y_noSF > 0, e_ReW / y_noSF, 0.0)

    fig, (ax_top, ax_bot) = plt.subplots(
        2, 1, figsize=(8, 7), gridspec_kw={"height_ratios": [3, 1]},
        sharex=True
    )
    fig.subplots_adjust(hspace=0.05)

    # ── Upper: overlay ──
    ax_top.errorbar(centers, y_noSF, yerr=e_noSF, fmt="s", color="black",
                    markersize=4, label="no b-tag SF")
    ax_top.errorbar(centers, y_SF,   yerr=e_SF,   fmt="o", color="blue",
                    markersize=4, label="b-tag SF")
    ax_top.errorbar(centers, y_ReW,  yerr=e_ReW,  fmt="^", color="red",
                    markersize=4, label="b-tag SF + reweight")

    ax_top.set_yscale("log")
    ymax = max(y_noSF.max(), y_SF.max(), y_ReW.max())
    ax_top.set_ylim(bottom=max(0.1, ymax * 1e-5), top=ymax * 20)
    ax_top.set_ylabel("Events")
    ax_top.legend(loc="upper right", frameon=False)
    ax_top.set_title(f"{sample}", fontsize=13, loc="left", fontstyle="italic")
    ax_top.text(1.0, 1.02, r"41.5 fb$^{-1}$ (13 TeV, 2017)",
                transform=ax_top.transAxes, ha="right", fontsize=10)

    # ── Lower: ratio ──
    ax_bot.errorbar(centers, r_SF,  yerr=re_SF,  fmt="o", color="blue",
                    markersize=4, label="SF / noSF")
    ax_bot.errorbar(centers, r_ReW, yerr=re_ReW, fmt="^", color="red",
                    markersize=4, label="SF+reweight / noSF")
    ax_bot.axhline(1.0, color="black", linestyle="--", linewidth=0.8)
    ax_bot.set_ylim(0.7, 1.6)
    ax_bot.set_ylabel("Ratio to no SF")
    ax_bot.set_xlabel(AXIS_LABELS.get(varname, varname))
    ax_bot.legend(loc="upper right", frameon=False, fontsize=8)

    outpath = os.path.join(outdir, f"{sample}_{varname}.png")
    fig.savefig(outpath, dpi=150, bbox_inches="tight")
    plt.close(fig)


def plot_one_sample(sample: str):
    """Read JSON and plot all variables for one sample."""
    jsonpath = os.path.join(PLOTDATA_DIR, f"{sample}.json")
    if not os.path.exists(jsonpath):
        print(f"  [WARN] {jsonpath} not found, skipping")
        return

    with open(jsonpath) as f:
        data = json.load(f)

    os.makedirs(OUTPUT_DIR, exist_ok=True)

    for varname, vardata in data["variables"].items():
        plot_one_variable(sample, varname, vardata, OUTPUT_DIR)

    print(f"  ✓ {sample}: {len(data['variables'])} plots → {OUTPUT_DIR}/")


def main():
    if len(sys.argv) < 2:
        print("Usage: python3 plot_btag.py <sample|all>")
        print("  e.g.: python3 plot_btag.py TTbar_Hadronic")
        print("        python3 plot_btag.py all")
        sys.exit(1)

    samples = []
    for arg in sys.argv[1:]:
        if arg == "all":
            samples.extend(ALL_MC + ALL_DATA)
        else:
            samples.append(arg)

    print(f">>> plot_btag.py: {len(samples)} samples\n")
    for s in samples:
        plot_one_sample(s)

    print(f"\n>>> Done. PNGs in {OUTPUT_DIR}/")


if __name__ == "__main__":
    main()
