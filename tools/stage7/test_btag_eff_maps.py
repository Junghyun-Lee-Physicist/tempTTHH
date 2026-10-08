#!/usr/bin/env python3
"""test_btag_eff_maps.py -- tools/stage7/btag_eff_maps.py on synthetic merged files, and the group mirror (STEP 25 K)

    python3 tools/stage7/test_btag_eff_maps.py        (after cmsenv: PyROOT, correctionlib; g++ for G)

  G  the Python btag_eff_group == tthh::btagEffGroup (include/BTagEffGroup.h, compiled with g++) on every sample name
     of data/samples_2024.json and of the 2024 ymls, and on edge cases; the expected group of named samples
  M  maps from synthetic merged files (the analyzer's BTagEff binning; values that change with the bin, so a wrong
     content order shows): the efficiencies per group = sum num / sum den; the fallback (a low-N_eff bin takes its
     group's |eta|-summed pT bin, a low-N_eff row the 'all' bin); 'all' = every sample used; the default exclusion
     (TTbb_Hadronic) and the Data sample skipped; the written JSON evaluated with correctionlib (also the unknown
     group -> 'all', clamp); a bin made unphysical by negative weights (num_L > den) skipped, a bin with e_L = 1 (no
     jet below L) skipped; the WPs from h_wp in the description; the submitter's preflight reading of it
     (btag_eff_json_summary: every node, year)
  F  the failures: h_nevt != the HT>500 cutflow bin (MISMATCH, nothing written), a missing merged file (FAIL; OK with
     --allow-missing), a file without BTagEff (NO-BTAGEFF), --check-only writes nothing, a sample with other WPs
Last line: SUMMARY test_btag_eff_maps PASS|FAIL (<n>/<m> checks).
"""
import array
import importlib.util
import json
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.dont_write_bytecode = True
RESULTS = []


def check(name, cond, detail=""):
    RESULTS.append(bool(cond))
    print("CHECK %s: %s" % (name, "PASS" if cond else "FAIL") + ("\n%s" % detail if detail and not cond else ""))


def load(name, rel):
    spec = importlib.util.spec_from_file_location(name, os.path.join(REPO, rel))
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def run(args):
    p = subprocess.run([sys.executable, os.path.join(HERE, "btag_eff_maps.py")] + args, stdout=subprocess.PIPE,
                       stderr=subprocess.STDOUT, universal_newlines=True, cwd=REPO,
                       env=dict(os.environ, PYTHONDONTWRITEBYTECODE="1"))
    return p.returncode, p.stdout


bem = load("tthh_btag_eff_maps", os.path.join("tools", "stage7", "btag_eff_maps.py"))
sub = load("tthh_submitter", "submit_job_FH_Tier3_unified.py")

# ---------------- G: the group mirror ----------------
names = set()
with open(os.path.join(REPO, "data", "samples_2024.json")) as fh:
    names |= {k for k in json.load(fh) if k != "_meta"}
for y in ("main", "prescan", "btagtrig"):
    p = os.path.join(REPO, "AnalyzerConfig", "Tier3_2024_FH_unified_%s.yml" % y)
    names |= {sub.entry_name(s) for s in sub.CondorJobManager.load_yaml_config(None, p)["samples"]}
edge = ["", "t", "T", "tt", "TT", "tT", "Tt", "tH", "ttbar", "QCD", "qcd_HT", "QCDx", "Q", "ST_tW", "TTTT", "tHq",
        "tHW", "WJetsToQQ_HT400to800", "ZZ", "XttY"]
allnames = sorted(names) + edge
gpp = shutil.which("g++")
if gpp:
    with tempfile.TemporaryDirectory(prefix="tthh_grp_") as T:
        src = os.path.join(T, "g.cc")
        with open(src, "w") as fh:
            fh.write('#include "BTagEffGroup.h"\n#include <iostream>\n#include <string>\n'
                     'int main(){ std::string s; while (std::getline(std::cin, s)) std::cout << tthh::btagEffGroup(s) '
                     '<< "\\n"; return 0; }\n')
        exe = os.path.join(T, "g")
        rc = subprocess.call([gpp, "-std=c++17", "-I", os.path.join(REPO, "include"), src, "-o", exe])
        if rc == 0:
            outp = subprocess.run([exe], input="\n".join(allnames) + "\n", stdout=subprocess.PIPE,
                                  universal_newlines=True).stdout.split("\n")[:len(allnames)]
            diff = [(n, c, bem.btag_eff_group(n)) for n, c in zip(allnames, outp) if c != bem.btag_eff_group(n)]
            check("G C++ and Python groups agree on %d names (%d samples of 2024)" % (len(allnames), len(names)),
                  not diff and len(outp) == len(allnames), str(diff[:10]))
        else:
            check("G the C++ group helper compiles", False, "g++ rc=%d" % rc)
else:
    print("NOTE g++ not found: the C++ side of G not run")
expect = {"TTHHto4b": "tt", "ttHTobb_had": "tt", "TTZHTo4b": "tt", "TTbar_Hadronic": "tt", "TT4b": "tt",
          "TTTWminus": "tt", "tHq": "other", "tHW": "other", "ST_tW_top_had": "other", "WW": "other",
          "QCD_HT1000to1200": "qcd", "WJetsToQQ_HT400to800": "other", "ZJetsToQQ_HT2500toInf": "other"}
got = {n: bem.btag_eff_group(n) for n in expect}
check("G named samples: tt (tt/TT start, ttH, ttHH, tttt), other (tH, single top, V), qcd", got == expect, str(got))

# ---------------- M / F: synthetic merged files ----------------
import ROOT  # noqa: E402

ROOT.gROOT.SetBatch(True)
ROOT.gErrorIgnoreLevel = ROOT.kError
ROOT.TH1.AddDirectory(False)
PT = [20, 30, 40, 50, 60, 80, 100, 130, 170, 220, 300, 400, 600, 1000]
ETA = [0.0, 0.6, 1.2, 1.8, 2.5]
NPT, NETA = len(PT) - 1, len(ETA) - 1
LABELS = ["noCut", "HadTrigger", "HT>500", "nbjets>=2"]


def frac(fl, wp, i, j, shift):
    """The 'true' efficiency of the synthetic sample in bin (i, j): changes with the bin, decreases with the WP."""
    base = {"b": (0.95, 0.80, 0.60), "c": (0.50, 0.20, 0.05), "l": (0.10, 0.01, 0.001)}[fl][("L", "M", "T").index(wp)]
    return base * (1.0 - 0.004 * i - 0.01 * j) * shift


WP24 = (0.0246, 0.1272, 0.4648)


def write_merged(path, nevt, ht, den_fn, shift=1.0, with_btageff=True, unphys=None, full=None, njobs=3, wps=WP24):
    """den_fn(fl, i, j) -> (sum w, sum w^2) of the denominator; num = frac x den (sum w^2 scaled alike). unphys =
    (fl, i, j): there num_L = 1.2 x den (what negative weights can do in a sparse bin); full = [(fl, i, j)]: there
    num_L = den (e_L = 1: no jet below L). BTagEff/h_wp = the WPs x njobs, bin 4 = njobs (what hadd makes)."""
    f = ROOT.TFile(path, "RECREATE")
    d = f.mkdir("Tree")
    d.cd()
    cf = ROOT.TH1F("cutflow", "", len(LABELS), 0, len(LABELS))
    for k, lab in enumerate(LABELS, 1):
        cf.GetXaxis().SetBinLabel(k, lab)
    cf.SetBinContent(1, ht * 3)
    cf.SetBinContent(2, ht * 2)
    cf.SetBinContent(3, ht)
    cf.SetBinContent(4, ht * 0.5)
    cf.Write()
    if with_btageff:
        e = f.mkdir("BTagEff")
        e.cd()
        hn = ROOT.TH1D("h_nevt", "", 1, 0.0, 1.0)
        hn.SetBinContent(1, nevt)
        hn.Write()
        hw = ROOT.TH1D("h_wp", "", 4, 0.0, 4.0)
        for k, v in enumerate(wps, 1):
            hw.SetBinContent(k, v * njobs)
        hw.SetBinContent(4, njobs)
        hw.Write()
        for fl in ("b", "c", "l"):
            for k in ("all", "L", "M", "T"):
                h = ROOT.TH2D("h2_%s_%s" % (fl, k), "", NPT, array.array("d", PT), NETA, array.array("d", ETA))
                h.Sumw2()
                for i in range(NPT):
                    for j in range(NETA):
                        sw, sw2 = den_fn(fl, i, j)
                        x = 1.0 if k == "all" else frac(fl, k, i, j, shift)
                        if unphys == (fl, i, j) and k == "L":
                            x = 1.2
                        if full and (fl, i, j) in full and k == "L":
                            x = 1.0
                        h.SetBinContent(i + 1, j + 1, sw * x)
                        h.SetBinError(i + 1, j + 1, (sw2 * x) ** 0.5)
                h.Write()
    f.Close()


def den_plenty(fl, i, j):
    n = {"b": 400.0, "c": 300.0, "l": 2000.0}[fl]
    return n, n                       # weight 1 per jet: N_eff = n


QLOW = (3, 0)       # qcd b: a low-N_eff bin (pT 50-60, |eta| 0-0.6) inside a well-filled row -> level 1
QROW = 5            # qcd b: the whole pT row 80-100 low -> level 2 ('all' bin)


def den_qcd(fl, i, j):
    if fl == "b" and (i, j) == QLOW:
        return 10.0, 10.0
    if fl == "b" and i == QROW:
        return 5.0, 5.0
    return den_plenty(fl, i, j)


DATA = "JetMET0_Run2024C-MINIv6NANOv15-v1"
with tempfile.TemporaryDirectory(prefix="tthh_beff_") as T:
    base = os.path.join(T, "merged")
    os.makedirs(base)
    FULL = ("b", 4, 0)    # tt b: num_L = den in one bin (e_L = 1) -> a thin tag bin (score < L empty): level 1
    FULLC = ("c", 6, 1)   # tt c: the same for c -- used as it is (c not in --flavours-required), unless asked for
    write_merged(os.path.join(base, "TTbar_Hadronic.root"), 500, 500, den_plenty, 1.0, full=[FULL, FULLC])
    write_merged(os.path.join(base, "QCD_HT1000to1200.root"), 300, 300, den_qcd, 0.9)
    UNPHYS = ("b", 2, 1)  # other b: num_L = 1.2 x den in one bin -> skipped (level 1: its pT bin over |eta|)
    write_merged(os.path.join(base, "ST_tW_top_had.root"), 200, 200, den_plenty, 0.95, unphys=UNPHYS)
    write_merged(os.path.join(base, "TTbb_Hadronic.root"), 100, 100, den_plenty, 0.5)   # excluded by default (2024)
    cfg = os.path.join(T, "cfg.yml")
    with open(cfg, "w") as fh:
        fh.write('common:\n  year: "2024"\n  analysis_mode: btagtrig\nsamples:\n'
                 + "".join("  - %s\n" % s for s in ("TTbar_Hadronic", "QCD_HT1000to1200", "ST_tW_top_had",
                                                     "TTbb_Hadronic", DATA)))
    out = os.path.join(T, "eff.json.gz")
    rc, o = run(["--config", cfg, "--base", base, "--out", out, "--root-out", os.path.join(T, "maps.root"),
                 "--plots", os.path.join(T, "maps.pdf")])
    check("M run: exit 0, WROTE, VERIFY ok, RESULT OK", rc == 0 and ("WROTE %s" % out) in o and "VERIFY ok" in o
          and o.rstrip().endswith("RESULT OK"), o[-3000:])
    check("M the default exclusion and the Data sample", "EXCLUDED TTbb_Hadronic (D-2026-10-05-C" in o
          and ("DATA-SKIPPED %s" % DATA) in o, o[-3000:])
    check("M SAMPLE lines: group, nevt = HT step, OK",
          "SAMPLE TTbar_Hadronic group=tt nevt=500 ht_step=500 OK" in o
          and "SAMPLE QCD_HT1000to1200 group=qcd nevt=300 ht_step=300 OK" in o
          and "SAMPLE ST_tW_top_had group=other nevt=200 ht_step=200 OK" in o, o[-3000:])
    check("M GROUP lines: tt / qcd / other one sample each, all 3",
          "GROUP tt samples=1" in o and "GROUP qcd samples=1" in o and "GROUP other samples=1" in o
          and "GROUP all samples=3" in o, o[-3000:])
    import correctionlib
    cs = correctionlib.CorrectionSet.from_file(out)
    c = cs["btag_eff"]

    def ev(g, fl, wp, i, j):
        return c.evaluate(g, fl, wp, 0.5 * (ETA[j] + ETA[j + 1]), 0.5 * (PT[i] + PT[i + 1]))

    bad = []
    for (i, j) in ((0, 0), (4, 2), (12, 3), (7, 1)):
        for fl, fn in (("b", 5), ("c", 4), ("l", 0)):
            for wp in ("L", "M", "T"):
                if (fl, i, j) in (FULL, FULLC):
                    continue
                exp = frac(fl, wp, i, j, 1.0)
                v = ev("tt", fn, wp, i, j)
                if abs(v - exp) > 1e-12:
                    bad.append(("tt", fl, wp, i, j, v, exp))
    check("M tt: e = the sample's num / den in each bin (pT x |eta| order kept)", not bad, str(bad[:5]))
    # 'all' = sum num / sum den over the three samples
    i, j = 6, 2
    dens = [(den_plenty("b", i, j)[0], 1.0), (den_qcd("b", i, j)[0], 0.9), (den_plenty("b", i, j)[0], 0.95)]
    exp_all = sum(d * frac("b", "M", i, j, s) for d, s in dens) / sum(d for d, _ in dens)
    check("M all: sum num / sum den over the samples used (TTbb_Hadronic not in it)",
          abs(ev("all", 5, "M", i, j) - exp_all) < 1e-12, "%r vs %r" % (ev("all", 5, "M", i, j), exp_all))
    # fallback 1: the qcd low bin takes the qcd row (pT 50-60) summed over |eta|
    i, j = QLOW
    row = [den_qcd("b", i, jj)[0] for jj in range(NETA)]
    exp_row = sum(r * frac("b", "M", i, jj, 0.9) for jj, r in enumerate(row)) / sum(row)
    check("M fallback level 1: a low-N_eff qcd bin = its pT bin summed over |eta|",
          abs(ev("qcd", 5, "M", i, j) - exp_row) < 1e-12 and abs(ev("qcd", 5, "M", i, 1) - frac("b", "M", i, 1, 0.9))
          < 1e-12, "%r vs %r" % (ev("qcd", 5, "M", i, j), exp_row))
    # fallback 2: the qcd low row takes the 'all' bin
    i, j = QROW, 2
    check("M fallback level 2: a low-N_eff qcd row = the 'all' bin",
          abs(ev("qcd", 5, "M", i, j) - ev("all", 5, "M", i, j)) < 1e-12
          and abs(ev("qcd", 5, "M", i, j) - frac("b", "M", i, j, 0.9)) > 1e-6, "")
    check("M LEVELS qcd b: 1 bin at level 1, 4 at level 2, the rest 0",
          "LEVELS qcd b L0=47 L1=1 L2=4 L3=0 L4=0" in o, [l for l in o.splitlines() if l.startswith("LEVELS qcd b")])
    i, j = UNPHYS[1], UNPHYS[2]
    rowL = (sum(400.0 * frac("b", "L", i, jj, 0.95) for jj in range(NETA) if jj != j) + 1.2 * 400.0) / (4 * 400.0)
    check("M an unphysical bin (num_L > den; negative weights) is skipped: its pT bin over |eta| (level 1), counted",
          abs(ev("other", 5, "L", i, j) - rowL) < 1e-12 and abs(ev("other", 5, "L", i, j + 1)
                                                               - frac("b", "L", i, j + 1, 0.95)) < 1e-12
          and "LEVELS other b L0=51 L1=1 L2=0 L3=0 L4=0 (skipped: unphysical 1, thin tag bin 0)" in o,
          "%r vs %r; %s" % (ev("other", 5, "L", i, j), rowL,
                            [l for l in o.splitlines() if l.startswith("LEVELS other b")]))
    i, j = FULL[1], FULL[2]
    rowLb = (sum(400.0 * frac("b", "L", i, jj, 1.0) for jj in range(NETA) if jj != j) + 400.0) / (4 * 400.0)
    check("M e_L = 1 in a b bin (no jet below L: a jet there in another sample would weigh 1e4) is not used: its pT bin "
          "over |eta| (level 1), counted as a thin tag bin",
          abs(ev("tt", 5, "L", i, j) - rowLb) < 1e-12 and ev("tt", 5, "L", i, j) < 0.99
          and "LEVELS tt b L0=51 L1=1 L2=0 L3=0 L4=0 (skipped: unphysical 0, thin tag bin 1)" in o,
          "%r vs %r; %s" % (ev("tt", 5, "L", i, j), rowLb, [l for l in o.splitlines() if l.startswith("LEVELS tt b")]))
    ic, jc = FULLC[1], FULLC[2]
    check("M ... for c (not in --flavours-required: 2024 has a b SF only) the bin is used as it is (e_L at the clamp), "
          "with a NOTE",
          abs(ev("tt", 4, "L", ic, jc) - (1.0 - 1.0e-4)) < 1e-12 and "LEVELS tt c L0=52 " in o
          and "NOTE c, l: the tag-bin rule not applied" in o and "flavours_required=b;" in c.description,
          "%r; %s" % (ev("tt", 4, "L", ic, jc), [l for l in o.splitlines() if l.startswith(("LEVELS tt c", "NOTE"))]))
    check("M the WPs (h_wp / jobs) are printed and written into the description",
          "WP L=0.0246 M=0.1272 T=0.4648" in o and "wp=L:0.0246,M:0.1272,T:0.4648" in c.description, c.description[:200])
    check("M unknown group -> 'all'; btag_eff_groups 1 / 0",
          ev("zzz", 5, "M", 3, 1) == ev("all", 5, "M", 3, 1) and cs["btag_eff_groups"].evaluate("qcd") == 1.0
          and cs["btag_eff_groups"].evaluate("zzz") == 0.0)
    check("M clamp: pT 5 / 5000 GeV, |eta| 3 inside the map", 0.0 < c.evaluate("tt", 5, "M", 3.0, 5000.0) < 1.0
          and c.evaluate("tt", 5, "M", 0.1, 5.0) == ev("tt", 5, "M", 0, 0))
    probs, info = sub.btag_eff_json_summary(out, "2024")
    probs17, _ = sub.btag_eff_json_summary(out, "2017")
    check("M the submitter's preflight reading: no problem for 2024 (groups tt qcd other all), a year mismatch for 2017",
          probs == [] and info["groups"] == ["tt", "qcd", "other", "all"]
          and any("made for year 2024" in x for x in probs17), str((probs, info["groups"], probs17)))
    check("M --root-out / --plots written", os.path.isfile(os.path.join(T, "maps.root"))
          and os.path.getsize(os.path.join(T, "maps.pdf")) > 1000)
    outc = os.path.join(T, "eff_bcl.json.gz")
    rc, oc = run(["--config", cfg, "--base", base, "--out", outc, "--flavours-required", "b", "c", "l"])
    vc = correctionlib.CorrectionSet.from_file(outc)["btag_eff"].evaluate("tt", 4, "L", 0.5 * (ETA[jc] + ETA[jc + 1]),
                                                                         0.5 * (PT[ic] + PT[ic + 1]))
    check("M --flavours-required b c l: the c bin with e_L = 1 is skipped too (level 1), no NOTE",
          rc == 0 and vc < 0.99 and "LEVELS tt c L0=51 L1=1 " in oc and "NOTE" not in oc, oc[-1500:])

    # ---------------- F: failures ----------------
    out2 = os.path.join(T, "eff2.json.gz")
    write_merged(os.path.join(base, "ST_tW_top_had.root"), 199, 200, den_plenty, 0.95, unphys=UNPHYS)   # 1 job short
    rc, o = run(["--config", cfg, "--base", base, "--out", out2])
    check("F h_nevt 199 vs HT step 200: MISMATCH, exit 1, nothing written",
          rc == 1 and "SAMPLE ST_tW_top_had group=other nevt=199 ht_step=200 MISMATCH" in o
          and not os.path.exists(out2) and "RESULT FAIL" in o, o[-2000:])
    os.remove(os.path.join(base, "ST_tW_top_had.root"))
    rc, o = run(["--config", cfg, "--base", base, "--out", out2])
    check("F a missing merged file: MISSING, exit 1", rc == 1 and "MISSING ST_tW_top_had" in o, o[-2000:])
    rc, o = run(["--config", cfg, "--base", base, "--out", out2, "--allow-missing"])
    check("F ... with --allow-missing: written from the other samples", rc == 0 and os.path.exists(out2)
          and "GROUP all samples=2" in o and "GROUP other" not in o, o[-2000:])
    write_merged(os.path.join(base, "ST_tW_top_had.root"), 200, 200, den_plenty, 0.95, with_btageff=False)
    out3 = os.path.join(T, "eff3.json.gz")
    rc, o = run(["--config", cfg, "--base", base, "--out", out3])
    check("F a file without BTagEff (an older executable): NO-BTAGEFF, exit 1", rc == 1 and "NO-BTAGEFF" in o
          and not os.path.exists(out3), o[-2000:])
    write_merged(os.path.join(base, "ST_tW_top_had.root"), 200, 200, den_plenty, 0.95, unphys=UNPHYS)
    rc, o = run(["--config", cfg, "--base", base, "--out", out3, "--check-only"])
    check("F --check-only: RESULT OK, nothing written", rc == 0 and "RESULT OK (check only)" in o
          and not os.path.exists(out3), o[-2000:])
    write_merged(os.path.join(base, "ST_tW_top_had.root"), 200, 200, den_plenty, 0.95, unphys=UNPHYS,
                 wps=(0.0246, 0.1300, 0.4648))
    rc, o = run(["--config", cfg, "--base", base, "--out", out3])
    check("F a sample made with other WPs: WP differ, exit 1, nothing written",
          rc == 1 and "SAMPLE ST_tW_top_had WP" in o and "differ from the first sample's" in o
          and not os.path.exists(out3), o[-2000:])

n_fail = RESULTS.count(False)
print("SUMMARY test_btag_eff_maps %s (%d/%d checks)" % ("PASS" if n_fail == 0 else "FAIL", len(RESULTS) - n_fail,
                                                        len(RESULTS)))
sys.exit(0 if n_fail == 0 else 1)
