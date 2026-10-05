#!/usr/bin/env python3
"""fake_pog.py -- FAKE jsonpog-integration tree for an offline analyzer smoke test (no CVMFS).

    python3 fake_pog.py <outdir>

The correction NAMES and INPUTS are the ones the analyzer asks for (src/CorrectionsManager.cc) and that
the real payloads have (2017_UL: jsonpog as used since 2025; 2024_Summer24: the 2026-10-02 inventory,
PLAN 9.2). The VALUES are invented (smooth functions of every input, so a wrong input order changes the
result). Never point a real job at this tree.
"""
import gzip
import os
import sys

import correctionlib.schemav2 as cs

PI = 3.141592653589793


def V(n, t="real"):
    return cs.Variable(name=n, type=t)


def F(expr, variables):
    return cs.Formula(nodetype="formula", expression=expr, parser="TFormula", variables=variables)


def C(name, inputs, data, desc="FAKE"):
    return cs.Correction(name=name, version=1, description=desc, inputs=inputs, output=V("out"), data=data)


def cat(inp, items, default=None):
    return cs.Category(nodetype="category", input=inp, default=default,
                       content=[cs.CategoryItem(key=k, value=v) for k, v in items])


def cut(var, lo, hi, inner):
    """inner if lo <= var < hi, else 0"""
    return cs.Binning(nodetype="binning", input=var, edges=[lo, hi], content=[inner], flow=0.0)


def write(path, corrections, compounds=None):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    cset = cs.CorrectionSet(schema_version=2, description="FAKE payload for the offline smoke test -- not physics",
                            corrections=corrections, compound_corrections=compounds or [])
    with gzip.open(path, "wt") as f:
        f.write(cset.model_dump_json(exclude_unset=True) if hasattr(cset, "model_dump_json") else cset.json(exclude_unset=True))
    print("WROTE", path, [c.name for c in corrections], [c.name for c in (compounds or [])])


def compound(name, inputs, stack):
    return cs.CompoundCorrection(name=name, inputs=inputs, output=V("out"), inputs_update=["JetPt"],
                                 input_op="*", output_op="*", stack=stack)


def jme_2017(out):
    tag = "Summer19UL17_V5"
    jin = [V("JetA"), V("JetEta"), V("JetPt"), V("Rho")]
    corrs = [
        C(tag + "_MC_L1FastJet_AK4PFchs", jin, F("1-0.3*x*t/(z+20)", ["JetA", "JetEta", "JetPt", "Rho"])),
        C(tag + "_MC_L2Relative_AK4PFchs", [V("JetEta"), V("JetPt")], F("1+0.02*x*x+5/(y+10)", ["JetEta", "JetPt"])),
        C(tag + "_MC_L3Absolute_AK4PFchs", [V("JetEta"), V("JetPt")], F("1+0*x*y", ["JetEta", "JetPt"])),
        C(tag + "_MC_L2L3Residual_AK4PFchs", [V("JetEta"), V("JetPt")], F("1+0*x*y", ["JetEta", "JetPt"])),
        C("Summer19UL17_JRV2_MC_PtResolution_AK4PFchs", [V("JetEta"), V("JetPt"), V("Rho")],
          F("0.03+0.9/sqrt(y)+0.001*z+0.01*abs(x)", ["JetEta", "JetPt", "Rho"])),
        C("Summer19UL17_JRV2_MC_ScaleFactor_AK4PFchs", [V("JetEta"), V("systematic", "string")],
          cat("systematic", [("nom", F("1.10+0.02*abs(x)", ["JetEta"])), ("up", F("1.15+0.02*abs(x)", ["JetEta"])),
                             ("down", F("1.05+0.02*abs(x)", ["JetEta"]))])),
        C(tag + "_MC_Total_AK4PFchs", [V("JetEta"), V("JetPt")], F("0.01+0.5/(y+10)+0*x", ["JetEta", "JetPt"])),
    ]
    comps = [compound(tag + "_MC_L1L2L3Res_AK4PFchs", jin,
                      [tag + "_MC_L1FastJet_AK4PFchs", tag + "_MC_L2Relative_AK4PFchs",
                       tag + "_MC_L3Absolute_AK4PFchs", tag + "_MC_L2L3Residual_AK4PFchs"])]
    for i, era in enumerate("BCDEF"):
        dtag = "Summer19UL17_Run%s_V5_DATA" % era
        corrs += [
            C(dtag + "_L1FastJet_AK4PFchs", jin, F("1-0.31*x*t/(z+20)", ["JetA", "JetEta", "JetPt", "Rho"])),
            C(dtag + "_L2Relative_AK4PFchs", [V("JetEta"), V("JetPt")], F("1+0.02*x*x+5/(y+10)", ["JetEta", "JetPt"])),
            C(dtag + "_L3Absolute_AK4PFchs", [V("JetEta"), V("JetPt")], F("1+0*x*y", ["JetEta", "JetPt"])),
            C(dtag + "_L2L3Residual_AK4PFchs", [V("JetEta"), V("JetPt")],
              F("%.3f+0.01*x+0*y" % (1.0 + 0.01 * (i + 1)), ["JetEta", "JetPt"])),
        ]
        comps.append(compound(dtag + "_L1L2L3Res_AK4PFchs", jin,
                              [dtag + "_L1FastJet_AK4PFchs", dtag + "_L2Relative_AK4PFchs",
                               dtag + "_L3Absolute_AK4PFchs", dtag + "_L2L3Residual_AK4PFchs"]))
    write(os.path.join(out, "POG/JME/2017_UL/jet_jerc.json.gz"), corrs, comps)


def lum_2017(out):
    write(os.path.join(out, "POG/LUM/2017_UL/puWeights.json.gz"), [
        C("Collisions17_UltraLegacy_goldenJSON", [V("NumTrueInteractions"), V("weights", "string")],
          cat("weights", [("nominal", F("1.2-0.008*x", ["NumTrueInteractions"])),
                          ("up", F("1.25-0.009*x", ["NumTrueInteractions"])),
                          ("down", F("1.15-0.007*x", ["NumTrueInteractions"]))]))])


def btv_2017(out):
    shape_in = [V("systematic", "string"), V("flavor", "int"), V("abseta"), V("pt"), V("discriminant")]
    fl = lambda k: cat("flavor", [(0, F("1+0.1*z-0.02*x+0*y", ["abseta", "pt", "discriminant"])),
                                  (4, F("1+0*x+0*y+0*z", ["abseta", "pt", "discriminant"])),
                                  (5, F("%.2f+0.2*z+0*x+0*y" % k, ["abseta", "pt", "discriminant"]))])
    wp_in = [V("systematic", "string"), V("working_point", "string"), V("flavor", "int"), V("abseta"), V("pt")]
    wp = lambda flavs: cat("working_point", [(w, cat("flavor", [(f, F("0.95+0.01*x+0*y", ["abseta", "pt"])) for f in flavs]))
                                             for w in ("L", "M", "T")])
    write(os.path.join(out, "POG/BTV/2017_UL/btagging.json.gz"), [
        C("deepJet_shape", shape_in, cat("systematic", [("central", fl(1.0))], default=fl(1.0))),
        C("deepJet_comb", wp_in, cat("systematic", [("central", wp([4, 5]))], default=wp([4, 5]))),
        C("deepJet_incl", wp_in, cat("systematic", [("central", wp([0]))], default=wp([0]))),
    ])


def jme_2024(out):
    mtag = "Summer24Prompt24_V1_MC"
    dtag = "Summer24Prompt24_V1_DATA"
    jtag = "Summer23BPixPrompt23_RunD_JRV1_MC"
    mc_in = [V("JetA"), V("JetEta"), V("JetPt"), V("Rho"), V("JetPhi")]
    da_in = mc_in + [V("run")]
    corrs = [
        C(mtag + "_L1FastJet_AK4PFPuppi", [V("JetA"), V("JetEta"), V("JetPt"), V("Rho")],
          F("1-0.2*x*t/(z+20)", ["JetA", "JetEta", "JetPt", "Rho"])),
        C(mtag + "_L2Relative_AK4PFPuppi", [V("JetEta"), V("JetPhi"), V("JetPt")],
          F("1+0.02*x*x+0.01*cos(y)+5/(z+10)", ["JetEta", "JetPhi", "JetPt"])),
        C(mtag + "_L3Absolute_AK4PFPuppi", [V("JetEta"), V("JetPt")], F("1+0*x*y", ["JetEta", "JetPt"])),
        C(mtag + "_L2L3Residual_AK4PFPuppi", [V("JetEta"), V("JetPt")], F("1+0*x*y", ["JetEta", "JetPt"])),
        C(dtag + "_L1FastJet_AK4PFPuppi", [V("JetA"), V("JetEta"), V("JetPt"), V("Rho")],
          F("1-0.21*x*t/(z+20)", ["JetA", "JetEta", "JetPt", "Rho"])),
        C(dtag + "_L2Relative_AK4PFPuppi", [V("JetEta"), V("JetPhi"), V("JetPt")],
          F("1+0.02*x*x+0.01*cos(y)+5/(z+10)", ["JetEta", "JetPhi", "JetPt"])),
        C(dtag + "_L3Absolute_AK4PFPuppi", [V("JetEta"), V("JetPt")], F("1+0*x*y", ["JetEta", "JetPt"])),
        C(dtag + "_L2L3Residual_AK4PFPuppi", [V("run"), V("JetEta"), V("JetPt")],
          cs.Binning(nodetype="binning", input="run", edges=[355000.0, 383000.0, 400000.0], flow="clamp",
                     content=[F("1.01+0.005*x+0*y", ["JetEta", "JetPt"]), F("1.03+0.005*x+0*y", ["JetEta", "JetPt"])])),
        C(jtag + "_PtResolution_AK4PFPuppi", [V("JetEta"), V("JetPt"), V("Rho")],
          F("0.03+0.9/sqrt(y)+0.001*z+0.01*abs(x)", ["JetEta", "JetPt", "Rho"])),
        C(jtag + "_ScaleFactor_AK4PFPuppi", [V("JetEta"), V("JetPt"), V("systematic", "string")],
          cat("systematic", [("nom", F("1.10+0.02*abs(x)+0.00005*y", ["JetEta", "JetPt"])),
                             ("up", F("1.15+0.02*abs(x)+0.00005*y", ["JetEta", "JetPt"])),
                             ("down", F("1.05+0.02*abs(x)+0.00005*y", ["JetEta", "JetPt"]))])),
        C(mtag + "_Total_AK4PFPuppi", [V("JetEta"), V("JetPt")], F("0.01+0.5/(y+10)+0*x", ["JetEta", "JetPt"])),
    ]
    comps = [
        compound(mtag + "_L1L2L3Res_AK4PFPuppi", mc_in,
                 [mtag + "_L1FastJet_AK4PFPuppi", mtag + "_L2Relative_AK4PFPuppi", mtag + "_L3Absolute_AK4PFPuppi",
                  mtag + "_L2L3Residual_AK4PFPuppi"]),
        compound(dtag + "_L1L2L3Res_AK4PFPuppi", da_in,
                 [dtag + "_L1FastJet_AK4PFPuppi", dtag + "_L2Relative_AK4PFPuppi", dtag + "_L3Absolute_AK4PFPuppi",
                  dtag + "_L2L3Residual_AK4PFPuppi"]),
    ]
    write(os.path.join(out, "POG/JME/2024_Summer24/jet_jerc.json.gz"), corrs, comps)

    # jet ID (the 2024 inventory: eta real, five fractions real, three multiplicities int)
    jid_in = [V("eta"), V("chHEF"), V("neHEF"), V("chEmEF"), V("neEmEF"), V("muEF"),
              V("chMultiplicity", "int"), V("neMultiplicity", "int"), V("multiplicity", "int")]
    BIG = 1.0e9

    def central(lepveto):
        inner = 1.0
        if lepveto:
            inner = cut("muEF", -1.0, 0.8, cut("chEmEF", -1.0, 0.8, inner))
        return cut("neHEF", -1.0, 0.99, cut("neEmEF", -1.0, 0.9, cut("multiplicity", 2, BIG,
                   cut("chHEF", 0.01, BIG, cut("chMultiplicity", 1, BIG, inner)))))

    def jid(lepveto):
        return cs.Binning(nodetype="binning", input="eta", edges=[0.0, 2.6, 2.7, 3.0, 5.2], flow="error", content=[
            central(lepveto),
            cut("neEmEF", -1.0, 0.99, cut("neHEF", -1.0, 0.9, 1.0)),
            cut("neHEF", -1.0, 0.9999, 1.0),
            cut("neEmEF", -1.0, 0.9, cut("neMultiplicity", 2, BIG, 1.0)),
        ])
    write(os.path.join(out, "POG/JME/2024_Summer24/jetid.json.gz"),
          [C("AK4PUPPI_Tight", jid_in, jid(False)), C("AK4PUPPI_TightLeptonVeto", jid_in, jid(True))])

    # veto map: one hot cell, eta [1.0, 1.6) x phi [2.0, 2.6)
    vm = cs.MultiBinning(nodetype="multibinning", inputs=["eta", "phi"],
                         edges=[[-5.191, 1.0, 1.6, 5.191], [-PI, 2.0, 2.6, PI]],
                         content=[0.0, 0.0, 0.0, 0.0, 100.0, 0.0, 0.0, 0.0, 0.0], flow="error")
    write(os.path.join(out, "POG/JME/2024_Summer24/jetvetomaps.json.gz"),
          [C("Summer24Prompt24_RunBCDEFGHI_V1", [V("type", "string"), V("eta"), V("phi")],
             cat("type", [("jetvetomap", vm), ("jetvetomap_all", vm)]))])


def main(argv):
    if len(argv) != 1:
        print(__doc__)
        return 2
    out = argv[0]
    jme_2017(out)
    lum_2017(out)
    btv_2017(out)
    jme_2024(out)
    with open(os.path.join(out, "FAKE_README.txt"), "w") as f:
        f.write("FAKE correction payloads for the offline smoke test (fake_pog.py). Not physics.\n")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
