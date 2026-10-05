# 2024 pileup weights (STEP 24, 2026-10-05; PLAN section 9.6 D15)

jsonpog has no `POG/LUM` for 2024 (eras up to `2023_Summer23BPix`, checked at KNU and lxplus 2026-10-02 and 10-05), so the
weights are made here from official inputs with `tools/stage2/pu_weights.py`.

## Data: `input/` (DQM, copied 2026-10-05)

`/eos/user/c/cmsdqm/www/CAF/certification/Collisions24/PileUp/`, files dated 10 April 2026 (EOS listing), TH1D `pileup`,
100 bins in [0, 100), contents in /ub (integral 1.09994e11 = 109.994 /fb):

| file | use | md5 | mean | rms |
|---|---|---|---|---|
| `dataPileupHistogram-2024CDEFGHI_Golden-69200ub.root` | nominal (minimum-bias 69.2 mb) | `3233aec8caf8071977697646750b8bea` | 50.03 | 8.48 |
| `dataPileupHistogram-2024CDEFGHI_Golden-66000ub.root` | down (-4.6 %) | `a1fa9578d2e0bc1db6851902a7d55c8d` | 47.72 | 8.09 |
| `dataPileupHistogram-2024CDEFGHI_Golden-72400ub.root` | up (+4.6 %) | `904cd0dbbd1500a8e799d5a3f4668362` | 52.35 | 8.87 |

`CDEFGHI` because our Data are 2024 C-I. The histograms predate our golden JSON (2026-08-04 version). Check of the
lumisections, 2026-10-05, with `PileUp/pileup_JSON-2024CDEFGHI_Golden.txt` of the same directory (21,722,297 byte; the
per-LS input of these histograms; kept in the workspace `lxplus_logs/pu2024/`, not here): against our golden JSON
restricted to C-I (run 379412-387121), 281,560 LS are common; 465 LS in 53 runs (0.182 /fb, 0.17 % of the lumi) are
only in the April file and 102 LS in 13 runs (about 0.04 /fb, 0.03 %) only in ours. 12 of those 13 runs are the runs
where brilcalc said the golden JSON is wider than the normtag (docs/reference/LUMI_SOURCES.md section 6.2), i.e. LS
without luminosity; the 13th is run 380238 (25 LS), absent from the April file. A 0.2 % difference in the LS set does
not change the shape of the profile at a visible level: the official histograms are used as they are (no pileupCalc).

## MC: Pileup_nTrueInt of the unskimmed Summer24 ZZ pilot

Our 2024 production is skimmed (6j20: six jets with stored pT > 20 GeV and |eta| < 2.5). A jet-count skim keeps
high-pileup events more often, so the Pileup_nTrueInt of our ntuples is not the MC pileup profile. The ZZ pilot
(`ttHH2024_v15_had_MC_v1_pilot`, NtupleForge `config_ttHH2024_v15_had_pilotMC.yaml`, no skim, 76 files, 4,800,000
events = DAS) is read instead; all Summer24 samples share one premix pileup library. KNU step:
`python3 tools/stage2/pu_weights.py mcprofile ...` (record in `runlogs/`), then
`python3 tools/stage2/pu_weights.py weights ... --out DerivedCorr/PU/2024_Summer24/puWeights_2024.json`.

## Output: `puWeights_2024.json` (after the KNU step)

correctionlib schema 2, the shape of jsonpog `puWeights.json`: correction `Collisions24_goldenJSON`, inputs
`NumTrueInteractions` (real) and `weights` (`nominal`, `up`, `down`), binning with flow `clamp`. w = data/MC with both
normalized in range; 0 where the MC bin is empty. The analyzer reads it through `path_pu_json` (yml) /
`TTHH_PU_JSON` (env) for 2024 only; 2016-2018 keep jsonpog.
