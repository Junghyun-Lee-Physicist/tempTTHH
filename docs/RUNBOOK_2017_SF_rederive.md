# RUNBOOK — 2017 trigger SF / b-tag norm reweight 재유도

> **목적:** 2017 SF 두 개를 유도해서 control plot 까지 가는 최단 경로.
> **상태:** 2026-07-29 · **preflight READY TO SUBMIT** (29 PASS / 0 WARN / 0 FAIL, KNU).
> **관련:** [`CHANGELOG.md`](CHANGELOG.md) 2026-07-29 (2) · [`STATUS.md`](STATUS.md)

## 이 문서에서 쓰는 경로 (KNU, preflight 실측값)

| | |
|---|---|
| analyzer | `/u/user/jhlee/ttHH/CMSSW_14_2_1/src/tempTTHH` |
| prescan output (입력) | `/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/AnalyzerOutput_prescan` |
| btagtrig output | `/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/AnalyzerOutput_btagtrig` |
| main output | `/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/AnalyzerOutput_main` |
| filelist | `<analyzer>/filelistTier3` (76 샘플 / 6,042 파일) |
| condor 작업물·job 로그 | `<analyzer>/condor/filelistTier3_unified_btagtrig` |

```bash
export TTHH=/u/user/jhlee/ttHH/CMSSW_14_2_1/src/tempTTHH
export PNFS=/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH
```

---

## 0. 사전 상태 — 통과 확인됨

```
[PASS] prescan_summary loaded        ... (61 samples)
[PASS] prescan coverage              all 61 MC samples have runs.genEventSumw > 0
[PASS] filelists present             all 76 found, 6042 input files total
[PASS] lumi consistency              42.07
[PASS] path_stitch_json              DerivedCorr/stitchFactors/stitch_factors_2017.json
[PASS] path_expanded_ttbarid_dir     DerivedCorr/expandedTtbarId/2017
PREFLIGHT SUMMARY: 29 PASS, 0 WARN, 0 FAIL  →  READY TO SUBMIT
```

이전에 걸려 있던 blocker(`prescan_summary.json` 이 구 이름 24개짜리)는 해소됐다.
**나중에 prescan 을 다시 돌렸다면** 이 파일을 반드시 갱신하고 preflight 를 다시 볼 것:

```bash
cd $TTHH
./consolidate_prescan.py \
    --input-base $PNFS/AnalyzerOutput_prescan \
    --outdir     prescan_summary
python3 submit_job_FH_Tier3_unified.py --mode btagtrig --preflight
```

> **왜 이게 중요한가:** weight = `lumi × σ × BR × k / Σgenw` 다. trigger SF 도 b-tag
> reweight 도 **비율**이라 lumi 같은 공통 인수는 상쇄되지만, `Σgenw`(= `runs.genEventSumw`)
> 는 샘플마다 다르므로 상쇄되지 않는다. group 안에서 샘플들의 상대 비중이 그대로
> 비율에 들어간다. 그래서 `SampleRegistry` 는 "모르면 1.0" 을 하지 않고 멈춘다.

---

## 1. 환경변수

두 SF 패키지가 **같은 skim** 을 보게 하는 것이 핵심이다.

```bash
export TTHH_BASE=$TTHH                     # xsec_db / prescan 조회의 기준 경로
export TTHH_SKIM_DIR=<btagtrig merge 결과 디렉토리>
# 선택
# export TTHH_BTAG_JSON=/cvmfs/cms.cern.ch/rsync/cms-nanoAOD/jsonpog-integration/POG/BTV/2017_UL/btagging.json.gz
# export TTHH_TRIGSF_JSON=$TTHH/TriggerStudy/trigger_sf.json.gz
# export TTHH_LUMI_FB=42.07                # 기본은 xsec_db `_meta.lumi_fb_inv`
```

두 패키지를 각자 디렉토리(`TriggerStudy/`, `bTagSF_ReweightStudy/`)에서 실행한다면
`TTHH_BASE` 는 기본값 `..` 로 이미 맞으므로 생략해도 된다.

---

## 2. btagtrig skim 생성

**analyzer 재빌드 필수.** 이번에 `nVetoLeptons` branch 가 추가됐고, downstream 이
그 branch 를 요구한다 (없으면 FATAL — 구 skim 을 쓰는 사고를 막는 장치).

```bash
cd $TTHH && make -j
python3 submit_job_FH_Tier3_unified.py --mode btagtrig --preflight   # READY 확인
python3 submit_job_FH_Tier3_unified.py --mode btagtrig               # ~6,042 jobs

# 진행/완료 확인
python3 submit_job_FH_Tier3_unified.py --mode btagtrig --report      # 요약 테이블
python3 submit_job_FH_Tier3_unified.py --mode btagtrig --status      # 샘플별 missing idx
python3 submit_job_FH_Tier3_unified.py --mode btagtrig --resubmit    # 미완료만 재큐
```

병합 (`<proc>.root` 하나로):

```bash
python3 outputMerger/merge_outputs.py --base $PNFS/AnalyzerOutput_btagtrig --list
python3 outputMerger/merge_outputs.py --base $PNFS/AnalyzerOutput_btagtrig \
        --mode condor --skip-existing
export TTHH_SKIM_DIR=$PNFS/AnalyzerOutput_btagtrig      # merge 결과 위치에 맞출 것
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
`tt4b`)이다. `include/SampleAlias.h` 가 신·구를 모두 시도하므로 개명 불필요.
lookup 파일은 tree 이름 `TtNb` 가 파일 안에 박혀 있어 개명하면 오히려 못 읽는다.

> job 로그에서 이 두 줄을 확인할 것 — 안 보이면 alias 가 안 탄 것이다:
> ```
> [StitchFactors]   sample 'TTbar_Hadronic' resolved via legacy alias 'TTToHadronic'
> [ExpandedTtbarId] sample 'TTbar_Hadronic' resolved via legacy alias 'TTToHadronic'
> ```

### 2-1. "2017 ttnb 파일이 옛날 것인데 써도 되나" — 써도 된다

patch 는 **파일이 아니라 event 를 가리킨다.** `ExpandedTtbarId` 의 키는
`(run, luminosityBlock, event)` 다 (`include/ExpandedTtbarId.h` `struct Key`).
ntuple 을 재생산해도 **event 집합이 같으면** 키가 그대로 맞는다. v18 → v20 은 같은
중앙 dataset 을 branch 만 늘려 다시 ntuplize 한 것이므로 여기 해당한다.

같은 event 집합이라는 근거 — 독립적인 **세 출처가 7개 샘플 전부에서 event 단위로 일치**:

| 샘플 | `samples_2017UL.json` `nevents` | patch 매칭 population (`TTHHGenCategoryTools/docs/06` §1) | prescan `runs.genEventCount` |
|---|---:|---:|---:|
| TTbar_Hadronic | 232,999,999 | 232,999,999 | 232,999,999 |
| TTbar_SemiLep  | 346,052,000 | 346,052,000 | 346,052,000 |
| TTbar_DiLep    | 106,724,000 | 106,724,000 | 106,724,000 |
| TTbb_Hadronic  |   5,694,656 |   5,694,656 |   5,694,656 |
| TTbb_SemiLep   |   7,318,891 |   7,318,891 |   7,318,891 |
| TTbb_DiLep     |   3,472,503 |   3,472,503 |   3,472,503 |
| TT4b           |   9,502,000 |   9,502,000 |   9,502,000 |

DAS 경로도 같다 — `NtupleForge/crabConfig/config_CPV2017UL_MC.yaml` 의
`/TTToHadronic_.../RunIISummer20UL17NanoAODv9-106X_mc2017_realistic_v9-**v1**/NANOAODSIM`
가 xsec_db `das_path` 와 재처리 버전까지 문자열 동일하다.

**2018 을 새로 만드는 것은 dataset 이 달라서가 아니라 era 가 새것이라서다.**
2017 patch 의 유효성과는 무관하다.

#### 그래도 기계로 확인할 것

위험한 시나리오는 "입력 dataset 이 달라 키가 안 맞는 경우"인데, 그때 나오는 것은
크래시가 아니라 **miss** 이고 miss 는 원본 `genTtbarId` 를 그대로 돌려준다 → tt+nb 가
조용히 적게 잡힌다. analyzer 안의 `genTtbarId` self-check 는 **matched key 에서만**
돌기 때문에 이 경우를 못 잡는다. 그래서 밖에서 합산 판정한다:

```bash
cd $TTHH
./tools/check_ttnb_coverage.py condor/filelistTier3_unified_btagtrig
```

판정식은 샘플별 **`Σ_jobs(hits) == rows`**.

```
sample                  jobs       rows     Σ hits   cover%  mismatch  verdict
TTbar_Hadronic           199     25,097     25,097 100.000%         0  ✓ OK
TT4b                      32  1,882,170  1,882,170 100.000%         0  ✓ OK
```

- job stdout 은 `condor/filelistTier3_unified_btagtrig/tmp_<sample>_<timestamp>/job_*.out`
  에 남는다. 위처럼 상위 디렉토리를 주면 재귀로 전부 훑는다.
- **반드시 그 샘플의 전 job 로그를 넣을 것.** job 은 입력 파일 일부만 보므로 job
  하나에서는 `hits < rows` 가 정상이다.
- `cover% < 100` → event 집합이 patch 와 다르다 → patch 재추출 필요.
- `mismatch > 0` → 엉뚱한 patch 가 붙었다 (analyzer 가 이미 FATAL 로 끊는다).

기대 rows (`06_validation_results.md` §2):
`TT4b 1,882,170` · `TTbb_SemiLep 26,544` · `TTbb_Hadronic 23,390` · `TTbb_DiLep 11,238` ·
`TTbar_SemiLep 32,162` · `TTbar_Hadronic 25,097` · `TTbar_DiLep 8,559`

**이 논거가 깨지는 유일한 경우**는 `ext` dataset 추가나 재처리 버전 변경(`-v1` → `-v2`)으로
event 가 늘어날 때다. ttbar family 7종은 `_ext*` 항목이 없고 전부 `-v1` 이라 해당 없음.
나중에 생기면 위 스크립트가 `cover% < 100` 으로 잡아 준다.

---

## 3. Trigger SF (muon control)

### btagtrig 을 두 번 돌려야 하나 — 아니다, 한 번이면 된다

skim 하나에 **두 집단이 같이 들어 있다**. analyzer 의 btagtrig 모드는

- hadronic trigger 를 **강제하지 않는다** (`kCutSequence` 1번이 `kSelBitMainLike`) — bit 만 저장.
  → trigger 효율의 **분모**가 살아 있다.
- lepton veto 도 **강제하지 않는다** (6번이 `kSelBitMainLike`).
- lead-muon gate 는 lepton 을 *수집할지*만 정하고 **이벤트를 버리지 않는다**
  (`if (passGate) { ...collect... }`). → lead muon 이 없는 FH 이벤트는 `nMuons=0` 으로 통과.

그래서 같은 파일에서 selection 만 달리 걸면 된다:

| 도구 | selection | 역할 |
|---|---|---|
| `TriggerStudy` | `passTrigger_HLT_IsoMu27` + `nMuons==1 && nElecs==0` | SF **측정** 영역 |
| `bTagSF_ReweightStudy` | hadronic trigger + `nVetoLeptons==0` | reweight 유도 = **적용** 영역 |

trigger SF 는 원래 "muon 으로 trigger 한 orthogonal 표본에서 hadronic trigger 효율을
재고, 그 SF 를 FH 에 적용" 하는 방법이다. 측정 영역과 적용 영역이 다른 것이 정상이며,
그래서 skim 을 두 번 만들 이유가 없다.

> `--region muon` 과 헷갈리지 말 것. 그건 QCD 억제용 **1ℓ+MET 제어영역**(main/debug 전용,
> `setRegion()` 이 `main`/`debug` 아니면 무시)이고 trigger SF 측정과 무관하다.

### offline selection 동기화 — 사본을 없앴다

trigger SF 는 offline selection 을 건 뒤 유도하므로, **analyzer 의 baseline 이 바뀌면
측정 영역과 적용 영역이 갈라진다.** 그런데 `EventLooper.cpp` 는 `6` / `40.0` / `500.0` /
`2.4` 를 손으로 옮겨 적은 사본으로 갖고 있었다 (`bTagSF` 도 `500.0` 이 하드코딩).

이제 두 도구 모두 analyzer 의 `include/SelectionCuts.h` 를 직접 include 해서
`Cuts::nJets` / `Cuts::sixthJetPt` / `Cuts::HT` / `Cuts::jetEta` 를 쓴다. baseline 을
고치면 세 곳이 자동으로 따라온다.

### "muon + MET selection 도 trigger SF 에 반영해야 하나"

**nominal 은 아니오.** trigger SF 는 ε_data/ε_MC 를 **(nb, HT, 6th-jet pT)** 로
매개변수화한 값이다. hadronic trigger 의 응답은 그 세 변수로 결정되고 MET 과는 무관하다.
그 factorization 이 성립하는 한 SF 는 어느 영역에도 그대로 옮겨진다 — 애초에
측정(muon CR) 과 적용(FH) 이 다른 영역인 것이 이 방법의 전제다.
측정 영역에 MET cut 을 더하면 통계만 깎여 low-stat bin(`kMinPassData=10`)이 늘어난다.

**다만 factorization 은 약속이 아니라 검사 대상이다.** 그래서 검사할 수단을 열어 뒀다:

- analyzer skim 에 **`MET_pt` branch 추가** (2026-07-29). 이전에는 skim 에 `passMETFilters`
  (bool) 뿐이라 `--region muon`(MET_pt>20) 을 downstream 에서 **재현할 수조차 없었다.**
- `TriggerStudy/include/Config.hh` 의 **`metCut`** (기본 `0.0` = 끔). `20.0` 으로 두면
  측정 영역에 MET cut 이 걸린다. analyzer 의 `metCutCR` 과 **같은 값**을 쓸 것.
- 켰는데 skim 에 branch 가 없으면 FATAL — "MET cut 을 걸고 유도했다"고 믿으면서 실제로는
  안 건 SF 가 나오는 것을 막는다.

closure 보는 법: `metCut=20.0` 으로 빌드 → Step 2(`applySF`) → `PlotTriggerEfficiency.cpp`
에서 SF 적용 후 data/MC 가 맞는지. **이건 systematic 확인용이고, 배포하는 SF 는 `metCut=0`
으로 유도한 것이다.**

> 참고: 진짜 남는 systematic 은 MET 이 아니라 **측정 영역이 ttbar semileptonic 지배,
> 적용 영역(FH)이 QCD-rich** 라는 조성 차이다. nb 로 binning 해서 상당 부분 흡수하지만
> 잔차는 남는다. MET cut 을 추가해도 이건 개선되지 않는다.

측정 영역: `IsoMu27` reference + `nMuons==1 && nElecs==0`.

```bash
cd $TTHH/TriggerStudy
make -j
./run_analysis.sh
#   Step1(eff) → hadd → DeriveSF.cpp → Step2(apply) → PlotTriggerEfficiency.cpp
# 산출물: trigger_sf.json.gz, TriggerSF.root
```

샘플 이름은 `run_analysis.sh` 상단 `MC_SAMPLES` / `DATA_SAMPLES` 한 곳에서만 관리한다
(`TTbar_*` 3종 + `SingleMuon_Run2017B..F`).

**확인 포인트**

- `Input base dir : ...  [TTHH_SKIM_DIR]` — `[built-in default]` 면 env 미설정이다.
- Data 샘플마다 `DataSet=SingleMuon, Era=B` 처럼 **era 가 한 글자**여야 한다.
  `Era=Run2017B` 로 나오면 Run B 를 CDEF trigger bit 로 평가하고 곧바로
  `[ERROR] Trigger logic mismatch` 로 죽는다.
- `[SampleRegistry] TTbar_Hadronic : MC, weight=...` 의 provenance 에 찍히는 `sumGenW` 가
  신규 prescan 값인지.

---

## 4. b-tag normalization reweight (FH)

측정 영역: skim invariant(≥6 jet, 6th pT>40, HT>500) + hadronic trigger + **lepton veto**,
**b-tag 컷 없음**(BTV 요구사항).

```bash
cd $TTHH/bTagSF_ReweightStudy
export TTHH_TRIGSF_JSON=$TTHH/TriggerStudy/trigger_sf.json.gz   # 방금 만든 것을 쓰도록
make -j
python3 run_all.py --dry-run          # 무엇이 돌고 무엇이 왜 빠지는지 먼저
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
  둘을 섞어 유도했다. `Config::requireLeptonVeto` 로 끌 수 있지만 권장하지 않는다.
  `nMuons==0` 으로 대체하면 안 된다 — lead-muon gate 를 못 넘은 soft lepton 이벤트가
  살아남아 main 과 위상공간이 어긋난다.

### 4-1. process group 은 8개 — 무엇이 어떻게 묶이나

`Config_TtCatGroup.hh` `AllProcessGroupKeys()` 가 정본이다.

| group | 무엇이 들어가나 |
|---|---|
| `tt+LF` | genTtbarId 0 · **그리고 minor BG 전부** (QCD, V+jets, single-t, diboson, TTZToBB, TTTT, TTW*, TTTW …) |
| `tt+cc` | 41–45 |
| `tt+B`  | **51, 52, 53, 54, 55** — tt+b · tt+2b · tt+bb 가 **하나로 병합** |
| `tt+nb` | **61, 62** (tt+bbb = tt+3b) **+ 71, 72** (tt+4b) |
| `ttH` | `ttHTobb` |
| `ttHH` | `TTHHto4b` |
| `ttZH4b` | `TTZHTo4b` |
| `ttZZ4b` | `TTZZTo4b` |

**tt+bb 와 tt+3b 는 각각 별개 그룹이 아니다.** tt+bb 는 `tt+B` 안에, tt+3b 는
`tt+nb` 안에 tt+4b 와 함께 들어간다. 병합은 의도된 것이다 — ttH AN-19-094 §A.2.1 이
sub-category 별 MC 통계가 부족해 4개 그룹(ttH / tt+bb / tt+cc / tt+LF)으로만 유도하라고
처방한다. 우리는 거기에 ttHH AN §D.0.5 근거로 `tt+nb` 를, 4b 최종상태 3종을 자체 그룹으로
추가해 8개다.

> 더 잘게(예: tt+b / tt+2b / tt+bb 분리) 가려면 `Config_TtCatGroup.hh` 의 `Classify()` 와
> `AllProcessGroupKeys()` 만 고치면 된다 — 유도와 적용이 **이 파일 하나**를 공유하므로
> 양쪽이 자동으로 따라온다. 다만 group 당 통계가 줄어 ratio 가 불안정해지므로 AN 근거가
> 필요하다.

### 4-2. 적용(main) 쪽 정합성 — 확인됨

- `Config_TtCatGroup.hh` 는 **파일이 하나**다. analyzer 의 `Makefile` 이
  `bTagSF_ReweightStudy/include` 를 직접 include 한다(`BTAGSF_INCDIR`). 유도와 적용이
  같은 정의를 문자 그대로 공유하므로 복사본 표류가 불가능하다.
- 적용부(`ttHHanalyzer_unified.cc`)는
  `TtCatGroup::MakeProcessKey(_sampleName, _expandedTtbarId)` 를 쓴다 — **expandedTtbarId**.
  이제 유도부와 같은 입력이다.
  > ⚠ 2026-07-29 이전에는 **적용은 expandedTtbarId, 유도는 genTtbarId** 로 어긋나 있었다.
  > 적용이 `"tt+nb"` 키를 요청하면 JSON 에 그 키가 **있긴 있어서**(전 bin 1.0) 크래시 없이
  > 1.0 이 곱해졌다. 이번에 유도를 expandedTtbarId 로 맞춰 해소했다.
- 2D(nJets×HT) 여부는 JSON 의 input 개수로 자동 판정된다
  (`btagReweightIs2D_ = inputs.size() >= 4`). 유도 쪽 `Config::useHTForReweight` 와 자동 정합.
  단 **`exe_BTagSF` 와 `exe_MakeJSON` 은 같은 플래그로 빌드**해야 한다.
- 실패가 조용하지 않다: 빈 key → exit 45 · JSON 에 key 없음 → main 에서 exit
  (`requireDerived_`) · non-finite → exit 46.

**확인 포인트**

- `[SampleRegistry]` 블록의 xsec_db / prescan 경로가 의도한 파일인지.
- `exe_MakeJSON` 의 `>>> Process group composition:` 에서 8개 group 이 각각 어떤 샘플을
  받았는지. `(no samples — group will be all-1.0!)` 이 뜨면 그 group 은 무효다.
- `>>> Central ratios per group` 에서 **`tt+nb` 가 정확히 `1.00000` 이 아닌지.**
  1.0 이면 `[WARN] ... Σ_withSF 가 모든 bin 에서 0` 이 함께 떴을 것이다
  (2026-07-29 추가). §2-1 의 coverage 검사부터 볼 것.
- analyzer(main) job 로그 끝의 `(expandedSub → processKey)` 진단에서 61/62/71/72 가
  53 과 **다른** key(`tt+nb`)를 받았는지.

---

## 5. 유도한 SF 를 main 에 넣고 control plot

```bash
mkdir -p $TTHH/DerivedCorr/TriggerSF $TTHH/DerivedCorr/bTagReweight
cp $TTHH/TriggerStudy/trigger_sf.json.gz            $TTHH/DerivedCorr/TriggerSF/
cp $TTHH/bTagSF_ReweightStudy/btagNormReweight.json $TTHH/DerivedCorr/bTagReweight/
```

```yaml
# AnalyzerConfig/Tier3_2017_FH_unified_main.yml
lumi_fb_inv: 42.07                 # ← btagtrig 과 맞출 것 (아직 41.48 이면 고칠 것)
path_trigsf_dir:           "DerivedCorr/TriggerSF"
path_btag_reweight_json:   "DerivedCorr/bTagReweight/btagNormReweight.json"
path_stitch_json:          "DerivedCorr/stitchFactors/stitch_factors_2017.json"
path_expanded_ttbarid_dir: "DerivedCorr/expandedTtbarId/2017"
```

```bash
cd $TTHH
python3 submit_job_FH_Tier3_unified.py --mode main --preflight
python3 submit_job_FH_Tier3_unified.py --mode main --trigsf on --btagsf on --btagrw on
# 주의: SF 토글이 output 디렉토리 이름에 붙는다 →
#   $PNFS/AnalyzerOutput_main_btagsf_btagrw
python3 outputMerger/merge_outputs.py --base $PNFS/AnalyzerOutput_main_btagsf_btagrw \
        --mode condor --skip-existing
# → plotter
```

---

## 6. 알려진 미해결 (2017 에는 무해)

- `ttHHanalyzer_unified.cc` 의 skim `passTrigger_*` 대입은 여전히 **2017 경로 고정**이다
  (`fillTree()`). 2017 에서는 `passHadTrig` 과 일관되지만, 2018 skim 은 두 값이 어긋나
  TriggerStudy 가 `exit(1)` 한다. 2018 작업 전에 반드시 처리할 것.
- `SelectionCuts.h` 의 `leadMuonPt=29` 는 `IsoMu27` 기준이다. 2018(`IsoMu24`)에서는 26 으로
  내려야 한다 — P1.
- xsec_db 의 `verify_xsec: true` 항목(ST_s_had, ST_tW_*, WW/WZ/ZZ 등)은 unblinding 전
  XSDB/GenXSecAnalyzer 로 확정 필요.
- ROOT 의존 코드(`EventLooper.cpp`, `BTagSFProcessor.cpp`, analyzer)는 이 변경 이후
  **아직 빌드 검증 전**이다. 위 `make -j` 가 첫 빌드다.
