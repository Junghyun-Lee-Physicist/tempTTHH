# RUNBOOK — 2017 trigger SF / b-tag norm reweight 재유도

> **목적:** 지금 바로 2017 SF 두 개를 유도해서 control plot 까지 가는 최단 경로.
> **상태:** 2026-07-29 · 코드는 준비됨, **prescan_summary 갱신이 유일한 blocker**.
> **관련:** [`CHANGELOG.md`](CHANGELOG.md) 2026-07-29 (2) · [`STATUS.md`](STATUS.md)

---

## 0. 먼저 — 이것 하나가 막고 있다

repo 의 `prescan_summary/prescan_summary.json` 은 **STEP18 이전 것**이다.

```
생성 시각 : 2026-06-15
샘플 수   : 24 (구 이름: TTToHadronic / ttbb_2L2Nu / tttt / ttHH ...)
```

신규 campaign(`ttHH2017UL_fullNano_v20`) 의 MC 61개 중 **47개가 없다**.
alias 층이 구 이름 17개를 신 이름으로 이어 주므로 ttbar·QCD·주요 rare 는 지금도
돌아가지만, single-top / V+jets / DY / diboson / tHq / tHW / TTW* / TTZToLLNuNu 는
Σgenw 를 모른다 → weight 합성 불가 → **FATAL**.

확인:

```bash
cd $TTHH            # tempTTHH 루트
python3 submit_job_FH_Tier3_unified.py --mode btagtrig --preflight
#   ... [FAIL] prescan coverage   54 problem(s): [...]
```

### 해결

KNU 에서 prescan output 을 consolidate 해서 이 파일을 갈아끼운다:

```bash
# KNU, cmsenv 후
cd $CMSSW_BASE/src/tempTTHH
./consolidate_prescan.py --input-base /pnfs/.../AnalyzerOutput_prescan --outdir prescan_summary
python3 submit_job_FH_Tier3_unified.py --mode btagtrig --preflight   # 다시 PASS 확인
```

> **왜 이걸 건너뛰면 안 되나:** weight = `lumi × σ × BR × k / Σgenw` 다. trigger SF 도
> b-tag reweight 도 **비율**이라 lumi 같은 공통 인수는 상쇄되지만, Σgenw 는 샘플마다
> 다르므로 상쇄되지 않는다. group 안에서 샘플들의 상대 비중이 그대로 비율에 들어간다.
> 그래서 `SampleRegistry` 는 "모르면 1.0" 을 하지 않고 멈춘다.

**임시로 진행하려면**: b-tag reweight 의 minor BG 는 전부 `tt+LF` 그룹에 흡수되므로,
prescan 이 있는 14개(ttbar 7 + QCD 7)만으로도 tt+LF/cc/B/nb 4개 주요 그룹은 유도된다.
`run_all.py` 가 없는 샘플을 자동으로 빼고 **이유와 함께 나열**하니, 그 목록을 보고
판단하면 된다. 단 `ttH` / `ttHH` / `ttZH4b` / `ttZZ4b` 자체 그룹은 그 샘플 없이는 못 만든다.

---

## 1. 환경변수

두 SF 패키지가 **같은 skim** 을 보게 하는 것이 핵심이다.

```bash
export TTHH=/path/to/tempTTHH
export TTHH_BASE=$TTHH                                   # xsec_db / prescan 위치의 기준
export TTHH_SKIM_DIR=/Users/jhlee/ttHH/ntuple/skimmed/gen_tier3   # btagtrig skim (merge 후)
# 선택
# export TTHH_BTAG_JSON=/path/to/POG/BTV/2017_UL/btagging.json.gz
# export TTHH_TRIGSF_JSON=$TTHH/TriggerStudy/trigger_sf.json.gz
# export TTHH_LUMI_FB=42.07        # 기본은 xsec_db `_meta.lumi_fb_inv`
```

`TTHH_BASE` 는 두 패키지가 각각 자기 디렉토리에서 실행되는 것을 전제로 기본값이 `..` 이다.
그 상태로 돌린다면 굳이 설정하지 않아도 된다.

---

## 2. btagtrig skim 생성 (KNU / condor)

**analyzer 를 다시 빌드해야 한다.** 이번에 `nVetoLeptons` branch 가 추가됐고,
downstream 이 그 branch 를 요구한다 (없으면 FATAL — 구 skim 을 쓰는 사고를 막는 장치).

```bash
cd $TTHH && make -j
python3 submit_job_FH_Tier3_unified.py --mode btagtrig --preflight   # 전부 PASS 인지
python3 submit_job_FH_Tier3_unified.py --mode btagtrig
# ... 완료 후
python3 submit_job_FH_Tier3_unified.py --mode btagtrig --report
python3 outputMerger/merge_outputs.py ...    # 샘플별 <sample>.root 로 병합
```

yml 에서 이번에 켠 것:

```yaml
lumi_fb_inv: 42.07
path_stitch_json:          "DerivedCorr/stitchFactors/stitch_factors_2017.json"
path_expanded_ttbarid_dir: "DerivedCorr/expandedTtbarId/2017"
path_trigsf_dir:           null    # 이 실행이 만들어 내는 것 (부트스트랩)
path_btag_reweight_json:   null    # 〃
```

`stitch_factors_2017.json` 과 `ttnb_*.root` 는 **구 이름**(`TTToHadronic` / `ttbb_2L2Nu` /
`tt4b`)으로 되어 있다. `include/SampleAlias.h` 가 신·구를 모두 시도하므로 개명 불필요.
lookup 파일은 tree 이름 `TtNb` 가 파일 안에 박혀 있어 개명하면 오히려 읽을 수 없게 된다.

> 로그에서 이 두 줄을 확인할 것 — 안 보이면 alias 가 안 탄 것이다:
> ```
> [StitchFactors] sample 'TTbar_Hadronic' resolved via legacy alias 'TTToHadronic'
> [ExpandedTtbarId] sample 'TTbar_Hadronic' resolved via legacy alias 'TTToHadronic'
> ```

### 2-1. "2017 ttnb 파일이 옛날 것인데 써도 되나"

**써도 된다.** patch 는 **파일이 아니라 event 를 가리키기** 때문이다.

`ExpandedTtbarId` 의 키는 `(run, luminosityBlock, event)` 다 (`include/ExpandedTtbarId.h`
`struct Key`). ntuple 을 재생산해도 **event 집합이 같으면** 키가 그대로 맞는다.
v18 → v20 은 같은 중앙 dataset 을 branch 만 늘려 다시 ntuplize 한 것이므로 여기 해당한다.

같은 event 집합이라는 근거 — 서로 **독립적인 세 출처가 7개 샘플 전부에서 event 단위로 일치**한다:

| 샘플 | `data/samples_2017UL.json` `nevents` (v20 등록) | patch 매칭 population (`TTHHGenCategoryTools/docs/06` §1) | prescan `runs.genEventCount` (실제 실행) |
|---|---:|---:|---:|
| TTbar_Hadronic | 232,999,999 | 232,999,999 | 232,999,999 |
| TTbar_SemiLep  | 346,052,000 | 346,052,000 | 346,052,000 |
| TTbar_DiLep    | 106,724,000 | 106,724,000 | 106,724,000 |
| TTbb_Hadronic  |   5,694,656 |   5,694,656 |   5,694,656 |
| TTbb_SemiLep   |   7,318,891 |   7,318,891 |   7,318,891 |
| TTbb_DiLep     |   3,472,503 |   3,472,503 |   3,472,503 |
| TT4b           |   9,502,000 |   9,502,000 |   9,502,000 |

그리고 DAS 경로도 같다 — `NtupleForge/crabConfig/config_CPV2017UL_MC.yaml` 의
`/TTToHadronic_TuneCP5_13TeV-powheg-pythia8/RunIISummer20UL17NanoAODv9-106X_mc2017_realistic_v9-**v1**/NANOAODSIM`
가 xsec_db `das_path` 와 문자열까지 동일하다 (`-v1` 재처리 버전까지).

**2018 을 새로 만드는 것은 dataset 이 달라서가 아니라 era 가 달라서다.** 2018 patch 는
2018 dataset 에서 처음 뽑는 것이므로 2017 의 유효성과 무관하다.

#### 그래도 기계로 확인할 것 (권장)

논거만 믿지 말 것. 위험한 시나리오는 "입력 dataset 이 달라 키가 안 맞는 경우"인데,
그때 나오는 것은 크래시가 아니라 **miss** 이고 miss 는 원본 `genTtbarId` 를 그대로
돌려준다 → tt+nb 가 조용히 적게 잡힌다. analyzer 안의 `genTtbarId` self-check 는
**matched key 에서만** 돌기 때문에 이 경우를 못 잡는다.

그래서 밖에서 합산 판정한다:

```bash
./tools/check_ttnb_coverage.py condor/filelistTier3_unified_btagtrig/log
```

판정식은 샘플별 **`Σ_jobs(hits) == rows`** 다.

```
sample                  jobs       rows     Σ hits   cover%  mismatch  verdict
TTbar_Hadronic           199     25,097     25,097 100.000%         0  ✓ OK
TT4b                      32  1,882,170  1,882,170 100.000%         0  ✓ OK
```

- **반드시 그 샘플의 전 job 로그를 넣을 것.** job 은 입력 파일 일부만 보므로 job
  하나에서는 `hits < rows` 가 정상이다.
- `cover% < 100` → event 집합이 patch 와 다르다 → patch 재추출 필요.
- `mismatch > 0` → 엉뚱한 patch 가 붙었다 (이건 analyzer 가 이미 FATAL 로 끊는다;
  `_abortOnMismatch = true` 가 기본값).

기대 rows (`06_validation_results.md` §2, 참고용):
`TT4b 1,882,170` · `TTbb_SemiLep 26,544` · `TTbb_Hadronic 23,390` · `TTbb_DiLep 11,238` ·
`TTbar_SemiLep 32,162` · `TTbar_Hadronic 25,097` · `TTbar_DiLep 8,559`

#### 이 논거가 깨지는 유일한 경우

`ext` dataset 추가나 재처리 버전 변경(`-v1` → `-v2`)으로 **event 가 늘어나는** 경우다.
현재 ttbar family 7종에는 `_ext*` 항목이 xsec_db 에 없고 전부 `-v1` 이므로 해당하지 않는다.
나중에 추가된다면 patch 를 그 샘플만 재추출해야 하고, `check_ttnb_coverage.py` 가
바로 그때 `cover% < 100` 으로 잡아 준다.

---

## 3. Trigger SF (muon control)

측정 영역: `IsoMu27` reference + `nMuons==1 && nElecs==0`. 이건 원래대로다.

```bash
cd $TTHH/TriggerStudy
make -j
./run_analysis.sh
#   Step1 (eff) → hadd → DeriveSF.cpp → Step2 (apply) → PlotTriggerEfficiency.cpp
# 산출물: trigger_sf.json.gz, TriggerSF.root
```

샘플 이름은 `run_analysis.sh` 상단 `MC_SAMPLES` / `DATA_SAMPLES` 한 곳에서만 관리한다
(`TTbar_*` 3종 + `SingleMuon_Run2017B..F`).

**확인 포인트**

- 로그 첫 줄에 `Input base dir : ...  [TTHH_SKIM_DIR]` — `[built-in default]` 이면 env 미설정.
- Data 샘플마다 `DataSet=SingleMuon, Era=B` 처럼 **era 가 한 글자**여야 한다.
  `Era=Run2017B` 로 나오면 Run B 가 CDEF trigger bit 로 평가되고, 곧바로
  `[ERROR] Trigger logic mismatch` 로 죽는다.
- `[SampleRegistry] TTbar_Hadronic : MC, weight=...` 의 provenance 에 찍히는
  `sumGenW` 가 신규 prescan 값인지.

---

## 4. b-tag normalization reweight (FH)

측정 영역: skim invariant(≥6 jet, 6th pT>40, HT>500) + hadronic trigger + **lepton veto**,
**b-tag 컷 없음**(BTV 요구사항).

```bash
cd $TTHH/bTagSF_ReweightStudy
export TTHH_TRIGSF_JSON=$TTHH/TriggerStudy/trigger_sf.json.gz   # 방금 만든 것을 쓰도록
make -j
python3 run_all.py --dry-run          # 무엇이 돌고 무엇이 왜 빠지는지 먼저 확인
python3 run_all.py -j 8
# 끝나면 마지막 줄에 성공 샘플만으로 만든 다음 명령이 찍힌다:
./exe_MakeJSON <succeeded samples...>
# 산출물: btagNormReweight.json
```

이번에 바뀐 것 (전부 조용히 틀리던 것들):

- process group dispatch 가 `expandedTtbarId` 기준. 원본 `genTtbarId` 는 `%100 ≤ 55` 라
  **tt+nb 그룹에 이벤트가 한 건도 안 들어왔다** (그래도 히스토그램은 만들어져서
  `makeReweightJSON` 의 "빈 그룹" 경고가 안 뜨고, ratio 규칙에 따라 390 bin 전부 1.0).
- `baseWeight` 에 `stitchWeight` 를 곱한다. 안 곱하면 inclusive tt 와 dedicated
  ttbb/tt4b 가 같은 위상공간을 두 번 채운다.
- `nVetoLeptons==0` 을 요구한다. btagtrig skim 은 FH 이벤트와 muon-control 이벤트를
  **함께** 담고 있고(analyzer 가 btagtrig 에서 lepton veto 를 강제하지 않는다), 전에는
  둘을 섞어 유도하고 있었다. `Config::requireLeptonVeto` 로 끌 수 있지만 권장하지 않는다.
  `nMuons==0` 으로 대체하면 안 된다 — lead-muon gate 를 못 넘은 soft lepton 이벤트가
  살아남아 main 과 위상공간이 어긋난다.

**확인 포인트**

- `[SampleRegistry]` 블록의 xsec_db / prescan 경로가 의도한 파일인지.
- Pass 1 요약에서 **`tt+nb` 그룹의 ratio 가 전부 1.000 이 아닌지.** 전부 1.0 이면
  expandedTtbarId 가 안 붙은 것이다 (yml 의 `path_expanded_ttbarid_dir` 확인).
- `exe_MakeJSON` 이 만든 JSON 의 key 가 8개이고, 8번째(`tt+nb`)에 1.0 아닌 값이 있는지.

---

## 5. 유도한 SF 를 main 에 넣고 control plot

```yaml
# AnalyzerConfig/Tier3_2017_FH_unified_main.yml
lumi_fb_inv: 42.07
path_trigsf_dir:           "DerivedCorr/TriggerSF"
path_btag_reweight_json:   "DerivedCorr/bTagReweight/btagNormReweight.json"
path_stitch_json:          "DerivedCorr/stitchFactors/stitch_factors_2017.json"
path_expanded_ttbarid_dir: "DerivedCorr/expandedTtbarId/2017"
```

```bash
cp $TTHH/TriggerStudy/trigger_sf.json.gz              $TTHH/DerivedCorr/TriggerSF/
cp $TTHH/bTagSF_ReweightStudy/btagNormReweight.json   $TTHH/DerivedCorr/bTagReweight/
cd $TTHH
python3 submit_job_FH_Tier3_unified.py --mode main --preflight
python3 submit_job_FH_Tier3_unified.py --mode main --trigsf on --btagsf on --btagrw on
# → merge_outputs.py → plotter
```

---

## 6. 알려진 미해결 (2017 에는 무해)

- `ttHHanalyzer_unified.cc` 의 skim `passTrigger_*` 대입은 여전히 **2017 경로 고정**이다
  (`fillTree()`). 2017 에서는 `passHadTrig` 과 일관되지만, 2018 skim 은 두 값이 어긋나
  TriggerStudy 가 `exit(1)` 한다. 2018 작업 전에 반드시 처리할 것.
- `SelectionCuts.h` 의 `leadMuonPt=29` 는 `IsoMu27` 기준이다. 2018(`IsoMu24`)에서는 26 으로
  내려야 한다 — P1 항목.
- xsec_db 의 `verify_xsec: true` 항목(ST_s_had, ST_tW_*, WW/WZ/ZZ 등)은 unblinding 전
  XSDB/GenXSecAnalyzer 로 확정 필요.
