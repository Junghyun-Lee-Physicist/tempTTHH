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
- **커밋 M(같은 날, §9):** merge job 의 stall guard(`condor_run.sh` 는 opt-in), 큐에 그 프로세스의 merge·analyzer job 이 있으면 merge 하지 않음 —
  `outputMerger/merge_outputs.py`, `tools/runlog/condor_run.sh`, `test/test_failure_checks.py`(I), `tools/runlog/test_runlog.sh`(T30), 문서.
  Python·bash 만(빌드 없음).
- **커밋 M2(같은 날, §10):** merge 의 guard 를 CPU 기준에서 **시간 한도**로(정상 merge 도 CPU 0.0–0.4 %), `condor_run.sh` 는 `--stall-guard` 대신
  `--time-limit H`; KNU 10-09 저녁의 결과(merge 셋, 사후 확인, plot, K+L 빌드와 시험). 같은 파일들과 문서, Python·bash 만.

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

### 9. 커밋 M (10-09 같은 날): merge job 의 stall guard, 큐에 그 프로세스의 merge·analyzer job 이 있으면 merge 하지 않음

(**M2 가 바꿈, §10:** 아래의 merge·condor_run guard 는 analyzer 의 CPU 기준을 옮긴 것이었는데, 정상 merge 도 CPU 1 % 미만이라 가르지 못한다 —
merge 는 시간 한도, `condor_run.sh` 는 `--time-limit H`. 큐 확인·제출 기록·dry-run·work 디렉터리는 그대로.)

**무엇이 있었나(사용자 출력).** FH merge(cluster 2181958)의 84 개 중 83 개는 끝났는데 2181958.19 = ParkingHH_Run2024F(입력 599, 572 MB)가 15,294 s 동안
CPU 14 s, 출력 73,596 byte 가 21:38 이후 그대로 — hadd 가 /pnfs 입력을 읽다 멈춤(10-08 analyzer job 의 멈춤과 같은 모양). K2 의 stall guard 는
`submit_job_FH_Tier3_unified.py` 가 쓰는 submit 파일에만 있었고 `merge_outputs.py` 의 `merge.sub`, `condor_run.sh` 의 `job.sub` 에는 없었다(AI 가
범위를 좁게 잡음). 사용자가 `condor_rm 2181958.19` 뒤 그 하나만 `--only` 로 다시(cluster 2181963).

**다시 merge 해도 겹치지 않는 이유(코드로 확인).** (1) hadd 는 입력 `<proc>/<proc>_<N>.root` 를 읽기만 한다. (2) `run_one_hadd.sh` 는 `hadd -f`(`-a` 아님):
ROOT 의 RECREATE 는 있던 파일을 지우고 새로 만든다 — 반쪽 파일의 내용은 넘어오지 않는다. (3) `--config` 의 job 번호 대조, worker 의 입력 수 대조(exit 7).
(4) `--report` 는 프로세스마다 가장 새 시도. 옛 프로세스가 노드에 남았다 깨어나도 쥔 것은 지워진 옛 파일(다른 PNFS-ID)이라 새 파일에 쓰지 못한다.
남은 위험 둘: 같은 프로세스의 merge job 둘이 함께 큐에 있을 때(옛 시도가 새 시도 뒤에 돌면 좋은 파일을 지우고 다시 쓰고, 그게 멈추면 반쪽이 남는데
`--report` 는 새 시도만 본다; `--resubmit` 은 pending·held 를 고르지 않지만 손으로 고른 `--only` 와 X 로 남은 job 은 막지 못했다), 그리고 그
프로세스의 analyzer job 이 아직 큐에 있을 때(출력은 job 이 시작할 때 생겨 `--config` 의 개수 대조를 통과한다 — 지금까지는 "merge 는 analyzer
`--report` 100 % 뒤에만" 이라는 사람의 규칙뿐). 다시 낼 때 `--skip-existing` 은 반쪽 파일도 건너뛴다(그래서 쓰지 않는다).

**바꾼 것.**
- `outputMerger/merge_outputs.py`: `merge.sub` 에 제출기의 `stall_guard_exprs()` 다섯 줄과 설명 줄(`--stall-guard on|off`, 기본 on). 식은 제출기에서
  읽어 한 곳에만 있다(읽지 못하면 아무것도 쓰기 전에 exit 2). 합치기 전에(local·condor·`--dry-run`) `condor_q -af:j Args Arguments`(env
  `TTHH_CONDOR_Q`·`TTHH_CONDOR_SUBMIT` 은 시험용): 큐(어느 상태든, X 도)의 job 이 합칠 `<base>/<proc>.root`(merge) 나 `<base>/<proc>/<proc>_<N>.root`
  (analyzer job)를 인자로 가지면 exit 2 와 job 번호·할 일(같은 이름이 같은 디렉터리에 있으면 링크로 닿아도; 상대 경로 인자는 그 job 의 Iwd 기준이라
  보지 않고, `--base` 는 절대 경로로 바꿔 쓴다). 다른 base 의 같은 프로세스, 같은 base 의 다른 프로세스는 걸리지 않는다. 큐를 못 읽으면 condor 제출은
  exit 2, local·`--dry-run` 은 note 뒤 계속. condor_submit 이 실패하거나 중단되면(Ctrl-C; SIGTERM 은 runlog.sh 가 ssh 가 끊길 때 보냄; 전에는
  traceback) work 디렉터리에 `submit_failed.txt` → 로그에 사건이 없는 job 은 `--report` 의 failed(전에는 영원히 pending), `--resubmit` 이 다시 고름
  (그래도 큐에 들어간 job 은 자기 로그가 정한다). `--dry-run` 의 work 디렉터리에는 먼저 `dry_run.txt` — 로그에 사건이 없는(제출하지 않은) job 은
  `--report` 가 시도로 보지 않는다(전에는 pending 으로 남아 `--resubmit` 도 고르지 않았다; 그 `merge.sub` 를 손으로 제출하면 그 로그가 정한다). 시도마다
  work 디렉터리 하나(이름이 있으면 1 초 뒤; 전에는 같은 초의 두 실행이 한 디렉터리를 나눠 썼다). 제출기를 읽을 때 `.pyc` 를 쓰지 않음. `--report` 의
  held 안내.
- `tools/runlog/condor_run.sh`: `--stall-guard on` 일 때만 `job.sub` 에 같은 다섯 줄(**기본 off**: 명령이 무엇이든 돌리므로 — /pnfs 훑기(파일 목록,
  branch signature, lumi 확인)는 건강해도 몇 시간 CPU 5 % 미만일 수 있어 1 시간마다 죽고 3 번 뒤 held 로 남으며, `tools/stage1/y1_reference.sh` 는 있던
  출력을 거절해 다시 시작하면 실패한다). 켜는 곳: plot, TriggerStudy `run_analysis.sh`, smoke·합성 시험, 빌드. bash 라 식을 글자로 가진다(시험이
  제출기와 같은지 본다). 시작마다 기록 하나(멈춘 것은 EXIT 143; status.sh 는 held·idle 동안 143, 다시 돌면 `-`).
- 시험: `test/test_failure_checks.py` I 열셋 → 76/76(ClassAd 모듈이 있으면 77). condor_q·condor_submit 은 가짜(절대 경로 env 와 PATH 맨 앞); 가짜를
  실행할 수 없으면(noexec TMPDIR) I 의 condor 실행은 하지 않고 FAIL 하나 — KNU 에서도 진짜 condor_submit 에 닿지 않는다. B 의 merge 도 가짜 condor_q.
  `tools/runlog/test_runlog.sh` T30 둘 → 85/85. 옛 두 스크립트로 돌리면 I 와 T30 이 FAIL(시험이 실제로 잡는지 확인).
- 문서: `README.md`(stall guard, §7.6, §8.3b, 시험 수), `tools/runlog/README.md`, `outputMerger/README.md`(머리말), `docs/reference/ERROR_CODES.md`,
  `docs/{STATUS,CHANGELOG}.md`.

**독립 검토(10-09, 반영).** (1) I 의 on/off 비교가 work 디렉터리 이름(초 단위)에 따라 흔들림 → 이름을 지우고 비교, 시도마다 디렉터리 하나. (2)
`condor_run.sh` 의 기본 on 은 /pnfs 훑기를 죽일 수 있음 → 기본 off, 켜는 곳을 적음. (3) "다시 해도 되는 명령" 에 `y1_reference.sh` 가 아님 → 적음.
(4) status.sh 의 143 설명이 틀림 → 고침. (5) condor `--dry-run` 이 영원히 pending → `dry_run.txt`. (6) `submit_failed.txt` 가 큐에 들어간 job 의 로그를
가림 → 사건이 없는 job 에만. (7) 상대 경로 → `--base` 절대, 상대 인자는 보지 않음. (8) 시험의 안전이 PATH 순서에만 기댐 → `TTHH_CONDOR_SUBMIT`·
`TTHH_CONDOR_Q` 절대 경로, 가짜를 실행할 수 없으면 I 를 하지 않음. 그리고 검토의 제안대로 그 프로세스의 analyzer job 도 확인.
2 차(고친 것 확인, 아홉 모두 됨; 진짜 condor_q·condor_submit 자리에 기록하는 가짜를 두고 네 경우 — 정상, noexec, 옛 스크립트, test_runlog — 모두 0 번
불림): (N1) `--dry-run` 디렉터리의 `merge.sub` 를 손으로 제출하면 `--report` 가 못 봄 → 사건이 없는 job 만 숨김. (N2) condor_submit 중의 Ctrl-C·
SIGTERM 은 표시 없이 traceback → 표시하고 exit 130/143. (N3) analyzer job 안내에 `-forcex`. 남긴 것(N4): 같은 base 의 `merge_outputs.py` 둘을 동시에
돌리면 둘 다 큐 확인을 통과할 수 있다(잠금 없음; 사람이 한 번에 하나씩 낸다).

**KNU.** Python·bash 만 — 다시 빌드하지 않고, 도는 job 과 상관없이 pull 해도 된다(merge job 은 그대로인 `run_one_hadd.sh` 를, condor_run job 은 자기
job 디렉터리의 `job.sh`·`payload.sh` 와 그대로인 `runlog.sh` 를 쓴다). btagtrig 의 merge 전에 받는다(워크스페이스 RUNBOOK §30).

**확인하지 못한 것(M).** KNU 의 condor_q 가 merge job 의 인자를 `Args`·`Arguments` 중 어디에 보이는지(둘 다 읽으므로 어느 쪽이든 된다) — 첫 실제
확인은 RUNBOOK §30 의 `--dry-run`(큐에 merge 나 analyzer job 이 있는 프로세스를 고르면 exit 2). 건강한 merge 의 CPU 비율과 5 % 문턱의 거리(사용자의
`condor_history` 출력으로). schedd 가 cms01 하나라는 가정(다른 submit host 에서 낸 job 은 보이지 않는다).

### 10. 커밋 M2 (10-09 같은 날): merge 의 guard 는 시간 한도, KNU 10-09 저녁의 결과

**무엇을 봤나(사용자 출력, KNU 22:38–23:38 KST).** M 의 확인용 `condor_history 2181958`(FH merge 84 개)의 CPU 비율 낮은 순: 2181958.39 49 s 동안
0 s, .47 15 s 동안 0 s, **.19(멈춘 것) 15,582 s 동안 20 s = 0.1 %**, .74 249 s 동안 1 s, .76 238 s 동안 1 s — 정상 merge 도 0.0–0.4 %. hadd 는 /pnfs 에서
파일을 열고 읽는 시간이 대부분이다. 그러니 analyzer 의 기준(1 h 넘게 CPU < 5 %, STEP 25 K2: analyzer 는 25–41 %)을 옮긴 M 의 merge guard 는 멈춘
merge 와 정상 merge 를 가르지 못하고, 1 시간 넘게 걸리는 정상 merge 를 hold·처음부터 다시(3 번 뒤 held)로 만든다(AI 가 merge 의 CPU 를 확인하지 않고
옮김). M 은 KNU 에서 merge 를 하나도 내기 전에 고친다.

**바꾼 것.**
- `outputMerger/merge_outputs.py`: merge 의 guard = **시간 한도** `time_limit_exprs()` — `periodic_hold` 는 지금 run 이 `--time-limit` 시간(기본 3)을
  넘을 때(CPU 와 상관없이), 이유 `tthh time limit: running <분> min (limit <분> min) with <초> s CPU on <slot@machine>`, subcode **4202**(4201 = analyzer 의
  CPU guard), release 는 그 hold 만 5 분 뒤 3 번 시작까지, `requirements` 는 마지막 machine 을 피함(제출기와 같은 글자). `--stall-guard off` 로 끔,
  `--time-limit 0` 등은 exit 2. **기본 3 시간 — 확정**(10-09 사용자 출력: merge work 디렉터리의 condor 로그로 잰 merge 405 개): 정상 404 개 중
  가장 긴 것 2,082 s(ParkingHH_Run2024G, CPU 0.8 %), 그다음 808 s 이하; 다시 한 ParkingHH F 534 s(CPU 3.9 %); 멈춘 2181958.19 는 15,581 s. 3 시간은
  가장 긴 정상의 5 배쯤이고, 멈춘 것은 3 시간에 잡힌다. 정상 merge 의 CPU 는 0.0–25 %(대부분 4 % 미만). 이제 guard 를 위해 제출기 module 을 읽지
  않는다(`--config` 때만).
- `tools/runlog/condor_run.sh`: `--stall-guard on|off`(M) 를 없애고(주면 exit 2 와 안내) **`--time-limit H`**(기본 없음; H 는 6·1.5 처럼) — 같은 다섯
  식(시험이 `time_limit_exprs()` 와 같은지 본다). 같은 로그로 잰 condor_run job 38 개: /pnfs 훑기는 길고 CPU 가 낮다(`knu_du_store` 54,079 s·0.5 %,
  `knu_p8_2024_MC` 13,382 s·12.5 %, `knu_d16_branchsig` 10,380 s·0.8 %) — 이것들에는 한도를 주지 않는다(M 의 CPU guard 를 기본으로 켰다면 죽었을
  것: 검토의 지적이 맞았다). plot 500–777 s(CPU 13–79 %) → `--time-limit 3`; 빌드 1,219 s; TriggerStudy 는 아직 모름 → 처음에는 `--time-limit 6`.
- 시험: `test/test_failure_checks.py` I 를 시간 한도로(기본 3 h = 10800 s·subcode 4202, `--time-limit 1.5` = 5400 s·90 min, off, 0 은 exit 2,
  condor_run 의 3·1.5 h 가 같은 식, 기본 없음, 0·문자·`--stall-guard` 는 exit 2) — 76/76; ClassAd 모듈이 있으면 + 식 평가(3.1 h 면 CPU 0 %·90 % 모두
  hold, 2.9 h·idle 은 아님, 이유 글자, release 두 번까지·5 분 뒤·이 hold 만) = 78/78(컨테이너: HTCondor 25.14 `classad2`). `tools/runlog/test_runlog.sh`
  T30 둘(85/85). HTCondor 24.0 의 condor_submit 논리로 `merge.sub` 를 펼치면 다섯 식이 그대로 job ad 에 들어가고 `Requirements` 는 기본 조건과 AND,
  인자는 `Args` 에 — M 의 큐 확인이 읽는 곳.

**KNU 10-09 저녁의 결과(사용자 출력).**
- merge: FH·μCR·eCR 모두 `--report` 84/84. 다시 한 ParkingHH_Run2024F: 입력 599(bad 0)의 Tree entries 합 2,804,824 = 합친 파일, noCut 합 121,340,087 대
  121,340,056(차이 31 = 2.6×10⁻⁷, TH1F 의 float) → 겹침·빠짐 없음(§9 의 설명을 실측으로 확인).
- `make_plots.py --check-only` 셋: `EVENTS MC 56 of 56 ... all equal`, 표본마다 noCut/exp 0.995–1.010, `RESULT OK`. Data/MC(SF 없음): FH 1.30(lepton
  조건 뒤)·1.44(nb ≥ 2)·2.02(≥ 3)·2.18(≥ 4)·1.40(nTotal); μCR 0.87·0.86·1.19·1.41·0.84; eCR 0.80·0.77·1.07·1.19·0.75. FH 의 MC 는 nTotal 에서 78 % 가
  LO QCD(nb ≥ 4 에서도 78 %), 20 % 가 tt. 읽기: CR 의 낮은 nb 에서 1 미만 = MC 의 hadronic trigger 효율(trigger SF 가 고칠 것; μCR 은 SF 를 재는 영역이라
  독립 검증은 eCR·FH)과 lepton ID/iso SF 없음(N4; e 가 μ 보다 0.09 낮은 것은 그 크기와 맞음); nb 가 늘수록 커지는 것은 세 영역 공통(b-tag SF, c·light
  mistag SF 없음, tt+HF 정규화); FH 의 nb ≥ 3 의 두 배는 QCD 모델링 — SF 만으로는 닫히지 않을 것.
- plot 셋(condor 2181964–6, 15 분 안) `RESULT OK`, 맥 `plots_2024_full/` 9 파일(compact·detailed PDF 와 YIELDS 셋).
- K+L 빌드(2181962) exit 0, TriggerStudy 빌드, 단위 PASS, 실패 확인 63/63(M 뒤 76/76), `test_btag_eff_maps` 26/26, smoke_2024 106/106(`BTAGSF` 셋),
  **TriggerStudy 합성 26/26 — KNU ROOT 6.30 에서 `root -q` 의 exit code 와 hadd 의 TNamed cycle 이 맞음**(아래 '확인하지 못한 것' 의 첫 항목 닫힘).
- M 은 `d66d98da`(KNU pull, 시험 76/76·85/85). M 의 `--dry-run` 확인은 2181963 이 이미 끝나 exit 0(맞음).

## 확인하지 못한 것

- ~~KNU 의 ROOT 6.30 에서: `root -l -b -q` 가 macro 의 int 를 exit code 로 돌려주는지, hadd 가 TNamed 를 입력마다 cycle 로 남기는지~~ —
  **10-09 확인**: KNU 의 합성 시험 26/26(§10).
- 진짜 2024 btagtrig 의 통계: category 마다 측정 bin 수(특히 nB4p), fallback 이 얼마나 쓰이는지, Data/MC 효율과 SF 의 크기.
- era C(4J3T 의 DeepJet 판 기간)와 D–I 의 SF 차이; 2024 의 μ turn-on 과 문턱(N2).
- lepton CR(μCR) 은 SF 를 유도한 표본과 겹친다(같은 1μ 영역) — 독립 검증은 eCR 과 FH 의 Data/MC(PLAN Stage 6 통과 기준).
