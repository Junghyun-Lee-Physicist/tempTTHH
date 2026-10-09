# STEP 26 — 2024 trigger SF: TriggerStudy 의 연도 지원과 JSON 의 연도 확인 (커밋 L)

- 날짜: 2026-10-09. 사용자 결정 [`../DECISIONS.md`](../DECISIONS.md) D-2026-10-08-A 의 둘째 부분("2024년과 18년도에 대해 control plot을 FH
  selection과 lepton selection 등을 보면서 trigger SF 및 관련 validation plot까지 확인하면서 기존 17년도 v9 결과랑도 비교"),
  [`../PLAN_v15_2018UL_2024.md`](../PLAN_v15_2018UL_2024.md) §9.1 Stage 6.
- 신규: `TriggerStudy/include/TrigStudyStamp.hh`, `test/trigger_study/{synth_trigskim.py, run_trigstudy_synth.sh}`, 이 문서
- 수정(TriggerStudy): `include/Config.hh`(`Year()`·`IsRun3()`·`RefTrigger()`·`HadronicOR()`·`LumiLabel()`, Dump), `include/NtupleReader.hh`,
  `src/NtupleReader.cc`(`passTrigger_HLT_IsoMu24` 가 있을 때만), `src/EventLooper.cpp`, `DeriveSF.cpp`, `PlotTriggerEfficiency.cpp`,
  `run_analysis.sh`(다시 씀), `README.md`. **`TriggerStudy/Makefile` 은 그대로.**
- 수정(공유·analyzer·제출기): `include/SampleRegistry.hh`(2024 Data 이름, `xsecEra()`·`prescanYear()`), `src/CorrectionsManager.cc`(JSON 의
  연도, E50), `include/ExitCodes.h`(주석), `submit_job_FH_Tier3_unified.py`(preflight 줄, 제출 때 E50), `bTagSF_ReweightStudy/src/BTagSFProcessor.cpp`
  (모르는 Data PD 는 시작 때 FATAL; `Makefile` 그대로)
- 수정(시험·문서): `test/test_SampleRegistry.cc`([6] [7]), `test/test_failure_checks.py`(H), `test/offline_smoke/run_offline_smoke.sh`,
  `test/run_unit_tests.sh`(머리말), `README.md`, `docs/{STATUS.md, CHANGELOG.md, PLAN_v15_2018UL_2024.md}`, `docs/reference/ERROR_CODES.md`;
  워크스페이스 `RUNBOOK_lxplus_2026-09-16.md` §29, `00_START_HERE.md`
- KNU: analyzer 는 **큐가 빈 뒤 다시 빌드**(`src/CorrectionsManager.cc`; K 의 빌드와 한 번에, RUNBOOK §28 4 = §29 2). TriggerStudy 는 따로
  `cd TriggerStudy && make`. 2017 의 출력과 수치는 바뀌지 않는다(§5).

## DECIDED / 실측

### 1. 연도마다 무엇이 다른가 (TriggerStudy, env `TTHH_YEAR`; 없으면 2017 = 전의 동작)

| | 2017 | 2024 |
|---|---|---|
| 기준(직교) trigger | `HLT_IsoMu27` | `HLT_IsoMu24`(prescale 없음, Muon0/1 의 seed; PLAN §9.6 D2) |
| 재는 hadronic OR | `HLT_PFHT1050` \| 6J1T \| 6J2T \| 4J3T; era B 는 CSV 이름의 `_B` slot | `HLT_PFHT1050` \| PNet 6J1T \| 6J2T \| 4J3T(Data 는 4J3T 에 초기 DeepJet 판도) — analyzer 가 `_CDEF` slot 에 넣고 `_B` 는 0 |
| Data | `SingleMuon_Run2017B..F` | xsec_db 의 `Muon0/1_Run2024*` 16 개(btagtrig yml 과 같음) |
| MC | `TTbar_{DiLep,Hadronic,SemiLep}` | 같음 |
| xsec_db / prescan | `data/samples_2017UL.json`, `prescan_summary/` | `data/samples_2024.json`, `prescan_summary_2024/`(KNU) |
| muon CR, offline selection, binning, SF 유도 | 그대로(1 μ & 0 e, `include/SelectionCuts.h`, HT 7 × 6 번째 jet pT 6 × nb 3) | 같음 |
| 산출물 위치(`run_analysis.sh`) | `TriggerStudy/`(전처럼) | `TriggerStudy/run_2024/` |

`Config::Year()` 는 2017·2024 만 받는다(2018 은 기준·경로 묶음을 먼저 써야 하므로 FATAL; 2018 v15 경로 때). 2017 의 EventLooper 식은 전과 같다
(기준 IsoMu27, era B 판정 그대로) — §5 의 bin 단위 비교.

**정의의 세 가지(물리는 바꾸지 않음, 기록):**
- MC 의 4J3T 는 PNet 판만, Data 는 PNet | DeepJet(era C 초기 38 run 은 DeepJet 판뿐; analyzer 의 STEP 24 규칙, D-2026-10-06-A). trigger SF 는 C–I
  전체에서 Data/MC 효율 비이므로 이 차이를 평균으로 흡수한다(PLAN §9.6 D4 의 "MC 에서 어떻게 흉내 낼지" = PNet 판만 + SF). era C 와 D–I 를 따로 본
  SF 비교는 아직 안 했다(확인하지 못한 것).
- 측정 영역의 lead μ 문턱은 btagtrig skim 의 lead-muon gate `Cuts::leadMuonPt = 29`(IsoMu27 의 plateau)를 2024 에도 그대로 둔다 — IsoMu24 의
  plateau 위라 turn-on 편향은 없고 통계만 조금 줄어든다. PLAN N2(2024 turn-on 을 보고 정함)는 열린 채.
- MET cut 은 2017 처럼 없음(`Config::metCut = 0`; FH 적용 영역과 같게).

### 2. 산출물

- `trigger_sf.json.gz`: correctionlib, `triggerSF`(central)·`triggerSF_err`(±1σ 의 폭), 입력 `nbJets`(int), `eta`, `ht`, `pt`(analyzer 의
  `getTriggerSF` 순서), nbJets edge [0, 3, 4, 5]·(ht, pt) multibinning, flow clamp(전과 같음). **description 셋 모두에
  `year=<YYYY>; reference=<HLT>; hadronic OR: ...`** 가 붙는다(새것).
- `TriggerSF.root`(측정·채운 SF·오차 map), DeriveSF 의 PDF(제목에 연도), `Validation_TrigEff_*.pdf`(lumi 표기 `109.8 fb^{-1} (13.6 TeV, 2024)`,
  "muon CR, reference HLT_IsoMu24").
- 로그: 표본·단계마다 `[TrigStudy] <year> <sample> (Data|MC, mode eff|applySF): entries N, muon CR M, <ref> R, hadronic OR P (eff x[, weighted y])`,
  DeriveSF 끝의 `[DeriveSF] year=...: categories ..., bins ...: measured ..., neighbour/extrapolated ..., fit ..., fallback SF=1+-0.5 ...` 와
  `RESULT OK|FAIL`, PlotTriggerEfficiency 의 `RESULT OK|FAIL`, `run_analysis.sh` 의 `RESULT OK (run_analysis <year>)`. macro 는 exit 0/1 을 돌려준다
  (`root -l -b -q` 가 그 값으로 끝남; 컨테이너 ROOT 6.40 에서 확인, KNU 6.30 은 §8 의 합성 시험으로).

### 3. 잘못된 조합은 어디서 멈추나

TriggerStudy(exit 1 또는 RESULT FAIL):
- 2024 인데 skim 에 `passTrigger_HLT_IsoMu24` 가 없음(2017 skim, STEP 24 전 skim) — 그대로면 모든 event 가 기준에서 떨어져 빈 map.
- `TTHH_YEAR` 가 2017(없을 때 포함)인데 skim 에 `passTrigger_HLT_IsoMu24` 가 있음 — analyzer 는 그 branch 를 2017 을 뺀 모든 해에 만든다
  (`ttHHanalyzer_unified.h` 의 tree booking): 2024 skim 을 2017 기준과 2017 weight 로 읽는 것을 막는다(독립 검토 10-09).
- xsec_db 의 `_meta.era` 가 그 해가 아님(2017 이 아닌 해에 `_meta.era` 가 없어도) — ttbar MC 이름은 2017 과 2024 가 같아서, env 에 남은 다른 해의
  `TTHH_XSEC_DB` 가 MC weight 를 조용히 바꿀 수 있었다.
- prescan 의 해(`meta.year`, 없으면 `meta.input_base`: `AnalyzerOutput_prescan` = 2017, `AnalyzerOutput_prescan_<YYYY>`)가 그 해가 아님 — 같은 이유,
  Σ`genEventSumw`.
- Data 표본 이름의 `_Run<YYYY>` 가 그 해가 아님.
- muon CR 이 100 개 이상인데 기준이 한 번도 안 켜짐(깨진 기준 branch).
- 2024 skim 에 `_B` slot 이 켜진 event(analyzer 의 규칙과 다른 skim).
- Step 2 가 읽는 `trigger_sf.json.gz` 의 연도(`year=`, 없으면 2017)가 그 해가 아님.
- DeriveSF·PlotTriggerEfficiency: 입력(merge 된 Step 1·Step 2 출력)의 `TrigStudyStamp`(EventLooper 가 출력마다 쓰는 TNamed
  `year=<Y>; reference=<ref>`; hadd 가 입력마다 하나씩 cycle 로 남김)가 그 해가 아님; 표시가 없으면 STEP 26 전 출력 = 2017 로만 받음.
- DeriveSF: category 하나라도 map 이 없거나 **측정된 bin 이 0 개**(그 category 의 SF 가 전부 외삽이나 1 ± 0.5)면 RESULT FAIL. JSON 은 `.tmp` 로 쓰고
  OK 일 때만 `trigger_sf.json.gz` 로 바꾼다. 유도한 뒤 FAIL 이면 이번 JSON 은 `trigger_sf.json.gz.FAILED`, 있던 것은 `trigger_sf.json.gz.previous`
  (아무것도 지우지 않음); 입력이 없거나 다른 해면 디렉터리를 건드리지 않는다.
- 알려진 빈틈: 손으로 한 hadd 가 표시 없는 옛(2017) 출력과 2024 출력을 섞으면 남는 표시가 2024 뿐이라 잡지 못한다 — `run_analysis.sh` 가 해마다
  자기 표본만 합친다.

analyzer(`CorrectionsManager::loadTrigger_`, MC, `path_trigsf_dir` 가 null 이 아닐 때 모든 mode·모든 `--trigsf`): `triggerSF` description 의 `year=`
가 job 의 해와 다르면, 또는 태그가 없는데(STEP 26 전 = 2017) job 이 2017 이 아니면 **E50**. 로그 `[CorrectionsManager] trigger SF JSON made for year
2024, job year 2024: OK`. 2017 main yml 이 가리키는 KNU 의 2017 JSON(태그 없음)은 그대로 쓰인다.

제출기: preflight 의 `trigger SF JSON` 줄(그 해의 JSON 이면 PASS: 해, up/down 유무, evtWeight 사용 여부), 그리고 제출 때(`--report`/`--status` 는
읽기만) 첫 표본 전에 멈춤(E50, 아무것도 내지 않음): 다른 해·태그 없는 비-2017·입력 순서가 다름(이건 analyzer 가 오류 없이 틀린 SF 를 씀)은 모든
mode 에서, 파일 없음·못 읽음·`triggerSF` 없음은 main/debug 에서(다른 mode 의 job 은 오류 줄과 triggerSF = 1 → WARN).

`bTagSF_ReweightStudy`: 2024 이름이 이제 `SampleRegistry` 를 지나므로(전에는 거기서 FATAL), 이 2016–2018 용 도구는 모르는 Data PD(Muon0 등)를
event 마다 WARN·버림 대신 시작 때 FATAL 로(2024 는 shape reweight 가 없음: fixed WP, D-2026-10-08-A).

### 4. 쓰는 법 (KNU; 명령은 워크스페이스 RUNBOOK §29)

btagtrig 2024 의 merge(RUNBOOK §28 7) 뒤:
`env TTHH_SKIM_DIR=/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/AnalyzerOutput_btagtrig_notrig_2024 bash TriggerStudy/run_analysis.sh --year 2024 --jobs 4`
(condor job; xsec_db·prescan 은 2024 기본값). 결과의 `trigger_sf.json.gz` 를 `DerivedCorr/TriggerSF/2024_Summer24/` 에 두고 2024 main yml 의
`path_trigsf_dir` 을 그 디렉터리로(효율 map 과 함께 `path_btag_eff_json` 도) → main 셋을 `--trigsf on --btagsf on`(출력 이름 `_btagsf`).
**`path_trigsf_dir` 은 JSON 이 생긴 뒤에만 바꾼다**: 경로가 있고 파일이 없으면 `--trigsf off` 인 main job 도 E50 이다(analyzer 는 경로가 있으면
읽는다).

### 5. 시험 (컨테이너: ROOT 6.40 pip, correctionlib 2.9; KNU 는 ROOT 6.30)

- `test/trigger_study/run_trigstudy_synth.sh <빌드> --orig <K2 빌드>`: **32/32**(`--orig` 없이 26). 합성 btagtrig skim(19 + 8 표본, TriggerStudy
  binning 의 칸마다 고르게; Data 의 OR 효율 = MC 효율 × r(nb), 2024 r = 0.97/0.92/0.86, 2017 0.95/0.90/0.85) 에 `run_analysis.sh --year`:
  RESULT OK; `[TrigStudy]` 줄 = 생성기의 수(표본 × 두 단계, 기준 이름 포함); JSON 의 태그; 가능한 117 칸 모두 측정, SF 가 r 을 되찾음(평균
  pull −0.22/−0.17, |pull| < 3 이 99.1 %/100 %; SF/r − 1 평균 −0.0067 ± 0.0052, 0 과 맞음); JSON = TriggerSF.root(최대 차이 2.2e−16, clamp 포함);
  closure(SF 를 곱한 MC 효율/Data 1.0012·0.9995, 안 곱하면 1.102·1.118); PDF 30. 멈춤 9 가지(§3)와 FAIL 처리(아무것도 지우지 않음). **2017 은 K2 빌드와
  bin 단위로 같다**: Step 1 map 96, JSON 값(설명만 다름), TriggerSF.root 27, Step 2 96 히스토그램; 이 DeriveSF·PlotTriggerEfficiency 가 옛
  빌드의(표시 없는) 2017 출력을 받아 같은 JSON 값.
- analyzer 와 이어서: 위 합성 2024 의 DeriveSF JSON 을 `TTHH_TRIGSF_DIR` 로 준 2024 MC job(`mc24.root`, `--trigsf on`) — load 줄 OK, Tree 의
  `triggerSF` 76 event 가 correctionlib 직접 평가와 모두 같음(16 개 값); 2017 JSON 으로는 E50.
- `test/offline_smoke/run_offline_smoke.sh <빌드> <K2 빌드>`: **95/95**(K 의 83 + 12): 가짜 JSON(nbJets·HT·6 번째 jet pT 의 함수, 12 칸 중 7 칸에
  event)로 per-event `triggerSF`/`_up`/`_down` 재계산 일치, evtWeight(on) = evtWeight(off) × triggerSF(5.4e−8), 2017 JSON·태그 없는 JSON 으로 2024
  E50(btagtrig 포함), Data 는 안 읽음, 2017 은 태그 없는 JSON 을 받고 2024 JSON 에 E50; 2017 Y1 출력은 K2 빌드와 같음.
- `test/test_failure_checks.py` **64/64**(ClassAd 모듈 없이 63; H 넷), `test/run_unit_tests.sh` PASS(`test_SampleRegistry` 59: 2024 이름 10, 해 표시 2;
  새 줄들을 옛 정규식에 돌리면 2024 이름 넷이 FAIL), `bTagSF_ReweightStudy` 빌드와 Muon0 이름에 exit 1.
- 고친 것(시험이 찾음): 가짜 JSON 의 정수 edge 는 correctionlib 이 거부("Invalid edge type") — DeriveSF 는 double 로 쓰므로 문제 아님(시험 쪽
  수정).

### 6. 독립 검토 (10-09, 두 번, 반영)

1 차: (1) `TTHH_YEAR` 없이 2024 skim 이 2017 로 조용히 돌고, 그 map 을 `TTHH_YEAR=2024` 의 DeriveSF 가 2024 태그로 내보낼 수 있었음 → §3 의 skim·
xsec_db·Data 이름 확인과 `TrigStudyStamp`. (2) 측정 bin 0 개여도 RESULT OK, JSON 을 먼저 덮어씀 → 측정 0 category FAIL, `.tmp`·rename. (3) 상대
경로가 `cd` 뒤에 틀어짐, 2017 출력 위치가 호출한 곳 → 절대경로, 2017 은 `TriggerStudy/`. (4) `mapfile`(bash 3.2 없음, 실패 무시) → 읽기 loop.
(5) 제출기가 btagtrig 에서도 못 읽는 JSON 에 멈춤 → mode 구분(`any_mode`). (6) 시험 보강(제출 경로, pT 가 변하는 가짜 JSON). (7)
`bTagSF_ReweightStudy` 의 2024 이름 → 시작 때 FATAL; README.
2 차: (1) 새 FAIL 처리가 있던 좋은 JSON 을 지웠음(DerivedCorr 에서 돌리면 production JSON 이 사라짐) → 지우지 않음(§3). (2) prescan 의 해를 안 봄 →
`prescanYear()`. (3) Step 2 가 JSON 의 해를 안 봄 → 확인. 남긴 것: 손 hadd 의 섞임(§3 빈틈), `--jobs` 는 묶음 단위로 기다림, 측정 0 의 FAIL 에
override 없음(`useEta=false` 에서는 거의 없고, 생기면 사람이 볼 일).

### 7. KNU 의 10-09 상태 (사용자 출력)

10-09 13:07 KST: eCR 의 job 넷(2181315.14 ttHToNonbb, 2181319.3·.24 tHW, 2181320.26 TTbar_Hadronic)이 10-08 재시작 뒤 다시 멈춤(cluster314·353·
351·294, 같은 event 근처). 입력 40 파일 모두 dCache `ONLINE`(디스크 사본 있음) — 테이프 문제는 아니고, 노드 8 대에서 같은 자리라 파일이나 그 파일의
pool 쪽. 다음: cms01 에서 40 파일 읽기 시험(`dd`, 파일마다 5 분 제한), 넷 `condor_rm`, 맥 K2 커밋 → KNU pull → `--report` 셋 → 빠진 것
`--resubmit`(stall guard). 읽히지 않는 파일이 있으면 KNU 관리자에게, 그 job 들은 빼고 진행(eCR = control region).

### 8. 다음 (워크스페이스 RUNBOOK §29)

큐가 빈 뒤: 맥 L 커밋 → KNU pull → K+L 빌드(§28 4 와 한 번에)·TriggerStudy `make` → 시험(단위, 실패 확인, 오프라인 smoke, **TriggerStudy 합성
시험: KNU ROOT 6.30 에서 `root -q` 의 exit code 와 hadd 의 TNamed cycle 확인**) → 2024 btagtrig(§28 6–7) → 효율 map(§28 8)과 **TriggerStudy
`--year 2024`** → JSON 둘을 DerivedCorr 에·main yml 두 경로 → main 셋 `--trigsf on --btagsf on` → plot 과 2017 v9 비교.

## 확인하지 못한 것

- KNU 의 ROOT 6.30 에서: `root -l -b -q` 가 macro 의 int 를 exit code 로 돌려주는지, hadd 가 TNamed 를 입력마다 cycle 로 남기는지(컨테이너 6.40 은
  둘 다 확인). RUNBOOK §29 의 합성 시험이 둘을 본다.
- 진짜 2024 btagtrig 의 통계: category 마다 측정 bin 수(특히 nB4p), fallback 이 얼마나 쓰이는지, Data/MC 효율과 SF 의 크기.
- era C(4J3T 의 DeepJet 판 기간)와 D–I 의 SF 차이; 2024 의 μ turn-on 과 문턱(N2).
- lepton CR(μCR) 은 SF 를 유도한 표본과 겹친다(같은 1μ 영역) — 독립 검증은 eCR 과 FH 의 Data/MC(PLAN Stage 6 통과 기준).
