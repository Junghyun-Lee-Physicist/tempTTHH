# STEP 24 — Stage 1·2 (2024): eventBuffer 를 우리 ntuple 의 합집합으로, PU weight 를 공식 입력으로, 2017 회귀 기준(Y1)

- 날짜: 2026-10-05. 묶음 1 = **도구와 입력 파일만**(analyzer 의 C++ 는 그대로, §1–5). 묶음 2 = analyzer 의 2024 코드, Stage 3 의 도구와
  설정, 컨테이너 시험(§6–9). 묶음 2 의 C++ 는 KNU 기록으로 만든 진짜 `include/eventBuffer.h` 와 **함께** 커밋한다(지금의 v9 header 로는
  2024 이름이 없어 빌드되지 않는다, §9).
- 신규(묶음 1): `tools/stage1/{eventbuffer_manifest.py, eventbuffer_from_record.py, hlt_keep.txt, y1_reference.sh, y1_compare.py,
  test_stage1.py}`, `tools/stage2/pu_weights.py`, `GoldenJson/2024_Summer24/Cert_Collisions2024_378981_386951_Golden.json`,
  `DerivedCorr/PU/2024_Summer24/{README.md, input/dataPileupHistogram-2024CDEFGHI_Golden-{66000,69200,72400}ub.root}`
- 수정(묶음 1): `GoldenJson/README.md`(2024 판), `docs/DECISIONS.md`(D-2026-10-05-A, D-2026-10-02-E 를 DECIDED 로), `docs/STATUS.md`,
  `docs/CHANGELOG.md`, `docs/PLAN_v15_2018UL_2024.md` §9.6(D14·D15·D16 의 결론)
- 신규(묶음 2): `tools/stage3/{make_filelists_v15.py, data_lumi_check.py}`, `test/offline_smoke/{fake_pog.py, synth_nano.py,
  run_offline_smoke.sh}`, `data/samples_2024.json`(**임시 σ**), `AnalyzerConfig/Tier3_2024_FH_unified_{prescan,main}.yml`
- 수정(묶음 2): `include/EraConfig.h`, `include/CorrectionsManager.h`, `src/CorrectionsManager.cc`, `ttHHanalyzer_unified.{h,cc}`,
  `submit_job_FH_Tier3_unified.py`, `test/test_EraConfig.cc`(2024), `tools/stage1/{y1_reference.sh, y1_compare.py, test_stage1.py}`(§3),
  `docs/DECISIONS.md`(D-2026-10-05-B, -C)
- 커밋 B(§11): `include/eventBuffer.h`(KNU 기록에서), `include/eventBuffer_variables.txt`(신규), `include/treestream.h`, `src/treestream.cc`,
  묶음 2 의 C++·제출기·`test/test_EraConfig.cc`, 로그 줄 셋, 제출기의 `files_per_job_data`, `tools/stage3/smoke_2024.sh`(신규),
  `test/offline_smoke/run_offline_smoke.sh`(+3 check), 2024 yml 의 files per job
- 커밋 C(§12–13): `ttHHanalyzer_unified.cc`(`requireSameBranchSet_` 이 job 의 파일을 직접 비교), `tools/stage3/smoke_2024.sh`(오래된 실행 파일이면
  시작하지 않음, exit 127 설명), `plotter/make_plots.py`(신규), `plotter/stack_plotter.C`(표기·여러 쪽 PDF·2024 그룹·파일 한 번 열기),
  `submit_job_FH_Tier3_unified.py`(`--report` 진행 줄), `consolidate_prescan.py`(`--skimmed`; 경고·anomaly 보고·exit code 가 같은 판정),
  `test/test_consolidate_prescan.py`(신규), `README.md` §8.1·`docs/ttbarCategorization.md` §10.2(그 옵션)
- 커밋 D·E(§14, 실패 확인; D = Python, 빌드 없음, E = C++ 둘): `ttHHanalyzer_unified.cc`(E: 못 여는 입력 파일 E30, 파일별
  `Events` entry 합 = chain, MC prescan 의 Runs 없음 E30), `test/offline_smoke/run_offline_smoke.sh`(E: +4 check),
  `plotter/make_plots.py`(MC 정확 개수 `EVENTS`/FLAG, RESULT 에 WARN 수),
  `outputMerger/merge_outputs.py`(`--config` job 수 대조, `--report`/`--resubmit`), `outputMerger/run_one_hadd.sh`(입력 수 대조
  exit 7, `<proc>_*.root` 만), `consolidate_prescan.py`(`--filelist-dir`), `submit_job_FH_Tier3_unified.py`(`--report` 의
  fail/wait, `--status` 의 원인), `docs/reference/ERROR_CODES.md`(도구의 코드), `README.md` §7.5·§8.1·§8.3b,
  `test/test_failure_checks.py`(신규), `test/test_consolidate_prescan.py`(+6)
- 커밋 E·F(§15, C++; E 는 D 와 함께 커밋되지 않고 맥에 남아 있었다 → F 와 한 커밋): E 는 위 D·E 줄의 `ttHHanalyzer_unified.cc`·
  `run_offline_smoke.sh`(+4); F 는 `include/ExitCodes.h`(`tthh::fatalExit`), `ttHHanalyzer_unified.{cc,h}`(fatal 경로, `main()` 의 `on_exit`,
  FATAL 줄의 번호), `src/{CorrectionsManager,ExpandedTtbarId,StitchFactors,tnm}.cc`, `include/{EraConfig,ConfigPath,ExpandedTtbarId}.h`,
  `test/test_EraConfig.cc`(주석), `test/offline_smoke/run_offline_smoke.sh`(+5 check), `submit_job_FH_Tier3_unified.py`(139 의 설명),
  `docs/reference/ERROR_CODES.md`, `README.md`·`docs/RUNBOOK_2017_SF_rederive.md`(옛 exit 45/46)
- 결정: [`../DECISIONS.md`](../DECISIONS.md) D-2026-10-05-A (사용자 10-05: 첫 plot 의 범위, D14, blinding; D15 의 방법), D-2026-10-05-B
  (2024 MC 의 4J3T 는 PNet 경로만, PROPOSED), D-2026-10-05-C (2024 는 tt+nb lookup 없이, D-2026-10-02-A 의 구현)
- 배경: 사용자(10-05) "얼른 plot 만들고 싶은데" → 첫 plot 은 2024 FH, trigger·b-tag SF 없이, σ 는 임시값 표시(D-2026-10-05-A). 그
  전에 PLAN §9 Stage 1(eventBuffer 와 무음 0)과 Stage 2(2024 연도 설정)가 있어야 analyzer 가 2024 파일을 읽는다.

## DECIDED / 실측

### 1. eventBuffer: KNU 에서 실제 파일로 목록을, 맥에서 header 를 (D16)

- **입력**: KNU 의 우리 생산 다섯(2024 MC·Data, 2018 v15 MC·Data, 2017 v20) 의 **CRAB task 디렉터리(`<PD>/<request>/<YYMMDD_hhmmss>`)마다
  처음으로 깨끗이 열리는 파일 하나**(`failed/`·`log/` 제외, 크기 0·zombie·recovered·`Events` 없음은 건너뛰고 기록; task 당 5 개까지 시도).
  dataset 에 task 가 여럿이면(resubmit 의 새 task, 남은 옛 task) 모두에서 하나씩 — 합집합이 넓어질 뿐 빠지는 것은 없다(검토 4). D16 의 파일마다 스캔(2024 Data
  32 dataset, 7,787 파일)은 dataset 안의 차이가 HLT branch 뿐임을 보였다(C 의 PNet 4J3T 등 7 개, I 의 2 개) — HLT 는 아래에서 잘리므로 dataset
  당 한 파일이면 객체·Flag·weight branch 의 합집합이 된다. MC 의 같은 스캔(`runlogs/run_knu_d16_branchsig_mc_20261005_063612.log`, 3,179 s, EXIT 3)은 60 dataset, 19,111 파일 모두
  dataset 당 branch 집합 하나, 못 연 파일 하나(크기 0, 아래 '확인하지 못한 것'). (사용자가 실수로 한 번 더 낸 같은 job 은 `condor_rm` 으로
  멈췄고 기록이 `stopped : by SIGTERM`, EXIT 143 으로 남았다 — STEP 23 의 signal 처리가 실제로 동작한 첫 경우.)
- **CRAB 파일 이름이 겹친다**: fork 의 `mkvariables.py --merge` 는 입력을 basename 으로 구분한다(`file_branches[label]`,
  `label = os.path.basename(fname)`) — `forgedNtuple_1.root` 가 dataset 마다 있으므로 그대로 넘기면 앞 파일이 조용히 지워진다. 그래서
  입력마다 `NNN_<tag>__<dataset>.root` 이름의 symlink 를 만들어 넘긴다(시험 "unique link names").
- **HLT·L1 자르기**: 합집합에서 `HLT_*`·`L1_*` 는 `tools/stage1/hlt_keep.txt`(2017·2018 의 analyzer 경로, 2024 D4 OR, IsoMu24·Ele30,
  2024 비교 경로; 27 개)만 남긴다. 이유는 PLAN §9.3: 메뉴가 파일마다 달라 chain 의 첫 파일이 branch 집합을 정한다. analyzer 소스가 읽는
  HLT 이름(`_ev->HLT_x`, `"Events/HLT_x"`)이 목록에 없으면 exit 1(header 에 없으면 빌드가 안 된다). 합성 시험(인벤토리 + slim 목록,
  10 파일)에서 합집합 2,158 → 764 record(HLT 838, L1 556 을 뺌), header 14,184 → 9,067 줄.
- **기록과 header 를 나눈다**: KNU job 은 잘린 `variables.txt` 를 runlog 안에(`BEGIN VARIABLES <md5>` … `END VARIABLES`) 남기고 header 를 한 번
  만들어 md5 만 적는다(`Created:`·`Author:` 줄을 뺀 md5 — 시각과 `getent passwd` 에 따라 다른 줄). KNU 에서는 runlogs 만 커밋한다는 규칙
  (D-2026-10-04-A) 그대로, header 는 맥에서 `eventbuffer_from_record.py` 가 같은 fork(`8be42e8`)로 그 기록에서 다시 만들고, md5 가 같아야
  쓴다. 같은 입력이면 같은 header 다: `mkanalyzer.py` 는 counter 를 정렬해 돌고 dict 는 입력 순서이므로 `PYTHONHASHSEED` 를 바꿔도 md5 가
  같았다(컨테이너, 10-05). 써 넣을 때 `Created:`·`Author:` 줄을 고정 문구로 바꾸고 include guard 뒤에
  `#define TTHH_EVENTBUFFER_STAMP "treestream <commit>; variables md5 <md5>; record <file>"` 를 넣는다(analyzer 가 시작할 때 찍는 것은 묶음 2).
  `include/eventBuffer_variables.txt`(그 입력)와 fork 의 `src/treestream.cc`·`include/treestream.h`(D16; A14 의 읽기 오류 정지는 treestream.cc
  에 있다)도 함께 쓴다.
- **linkdef**: `include/linkdef.h` 는 손으로 둔 struct 17 개의 `#pragma link` 를 가진다. 합집합이 2017 v20 의 객체를 모두 포함하므로 그 17
  개는 새 header 에도 있다 — 묶음 2 의 빌드에서 `rootcling` 으로 확인한다.

### 2. PU weight (D15)

- **data**: DQM 의 공식 히스토그램 `Collisions24/PileUp/dataPileupHistogram-2024CDEFGHI_Golden-{69200,66000,72400}ub.root`(중앙, down, up;
  TH1D `pileup`, 100 bin [0,100), 평균 50.03 / 47.72 / 52.35). jsonpog 의 `POG/LUM` 에 2024 가 없다(KNU 10-05 `ls`: 2016·2017·2018 UL,
  2022 Summer22(EE), 2023 Summer23(BPix)). pileupCalc 은 쓰지 않는다.
- **LS 확인(10-05)**: 히스토그램은 4월 10일 판이라 우리 golden JSON(8월 4일 판)보다 앞선다. 같은 디렉터리의 per-LS 입력
  `pileup_JSON-2024CDEFGHI_Golden.txt` 와 우리 JSON(C–I)을 비교: 공통 281,560 LS; 4월 판에만 465 LS(53 run, 0.182 fb⁻¹, 0.17 %), 우리에만
  102 LS(13 run, 0.04 fb⁻¹ 쯤, 0.03 %) — 13 run 가운데 12 개는 brilcalc 이 "JSON 이 normtag 보다 넓다" 고 한 run(LUMI_SOURCES §6.2, lumi 없는 LS),
  하나는 run 380238(25 LS, 4월 판에 없음). 0.2 % 의 LS 차이로는 분포 모양이 보이게 바뀌지 않는다 → 공식 히스토그램 그대로.
  (그 파일의 lumi 합 109.994 − 4월 판에만 있는 0.182 ≈ 우리 normtag_PHYSICS 109.816: 같은 lumi 값이다.)
- **MC**: 우리 2024 MC 는 6j20 skim 이다. jet 수 skim 은 PU 가 높은 event 를 더 남기므로 우리 ntuple 의 `Pileup_nTrueInt` 는 MC PU 분포가
  아니다(PLAN §9.6 D15 의 원안 "우리 출력에 있음" 을 고친다). 대신 skim 없는 ZZ 파일럿(`ttHH2024_v15_had_MC_v1_pilot`, 76 파일, 4,800,000
  event = DAS, NtupleForge V43)을 쓴다. Summer24 는 모든 표본이 premix PU library 하나를 쓴다.
- **도구**: `tools/stage2/pu_weights.py mcprofile`(KNU: data 히스토그램의 binning 으로 MC 분포, `PUMC_*` 줄이 기록) →
  `pu_weights.py weights`(어디서나: correctionlib schema 2, jsonpog `puWeights.json` 의 모양 — correction `Collisions24_goldenJSON`, 입력
  `NumTrueInteractions`·`weights`(nominal/up/down), flow clamp; w = data/MC(범위 안에서 각각 1 로 정규화), MC bin 이 비면 0 과 그 bin 의 data
  분율(`lost_data`)을 출력; correctionlib 으로 다시 읽어 1e-12 안에서 같은지 확인 — correctionlib 의 JSON 파서가 십진수를 Python repr 과 1 ulp
  다르게 읽을 수 있어 정확 일치는 요구하지 않는다).

### 3. Y1: 2017 의 회귀 기준

- `tools/stage1/y1_reference.sh <tag>` 가 **지금 디스크의 실행 파일**로 2017 파일 넷(TTbar_Hadronic MC, JetHT F, BTagCSV F, JetHT B — 6J/HT
  쪽과 4J3T 쪽 PD 규칙, era B 의 옛 경로 이름)을 main·FH·SF 모두 off·weight 1 로 돌린다(경로는 2017 main yml 과 같게, 파생 보정은 null).
  묶음 1 을 받은 뒤 **빌드 전에** `before`, 묶음 2 의 빌드 뒤에 `after`; `y1_compare.py` 는 모든 TH1 을 bin 단위로(over/underflow 포함)
  비교한다: 공통 히스토그램이 모두 같고, A 에만 있는 것이 없고, tree entries 가 같으면 PASS(B 에만 있는 새 감시용 히스토그램은 목록만).
- prescan·stitch 를 쓰지 않으므로 KNU 의 2017 로컬 수정(`39936117` 에 들어간 stitch factor·prescan summary; 결정 1)과 무관하다.
- **(묶음 2 의 컨테이너 시험에서 찾음)** TTbar_Hadronic 은 ttbar stitching 집합이라 tt+nb lookup 이 없으면 E11 로 멈춘다 — `null` 이어도.
  처음 판의 `EXPANDED_TTBARID_DIR=__NULL__` 로는 before·after 둘 다 mc 가 멈춰, 비교에서 MC 가 **조용히 빠진다**. 그래서 lookup 디렉터리는
  btagtrig yml 과 같은 `DerivedCorr/expandedTtbarId/2017`(KNU 에 복사된 `ttnb_TTToHadronic.root`; `*.root` 라 git 에 없다, 없으면 exit 2),
  그리고 `condor/y1/<tag>/EXPECTED`(네 이름)를 쓰고 `y1_compare.py` 는 EXPECTED 의 이름이 **어느 쪽에도 없으면 FAIL** 이다.

### 4. 시험

`tools/stage1/test_stage1.py` (ROOT + 빌드된 fork): 40 check — manifest(task 마다 한 파일과 옛 task 의 branch, 크기 0 건너뜀, `failed/` 무시,
이름 겹침, HLT/L1 자르기, keep 의 WARN, 주석 속 HLT 이름 무시, 형 충돌 요약, 기록 블록의 md5, 없는 base·빈 base exit 1, 상대 경로 base,
CODE_MISSING exit 1, NOINPUT exit 1), record(header 재현, 쓰기와 stamp, 고정 provenance 줄, variables 사본, treestream 사본, 고친 블록
거절, 실패한 manifest 의 기록 거절, 로컬 수정이 있는 fork 거절), PU(크기 0 → BADFILE exit 3, 합 = event 수, 없는 branch exit 1, JSON 모양,
read-back, Σ m·w = 1 − lost, binning 불일치 exit 1), Y1(같음 PASS, cutflow 한 bin FAIL, B 에만 있음 PASS, 없어짐 FAIL, NaN bin FAIL,
B 에만 있는 파일 FAIL, tree 값만 바뀜 FAIL, EXPECTED 의 run 이 양쪽에 없음 FAIL·모두 있음 PASS). 컨테이너(ROOT 6.40, fork `8be42e8` 를
새 dictionary 로 빌드) 42/42 PASS. KNU(ROOT 6.30)는 RUNBOOK §21 의 첫 단계.

### 5. 독립 검토 (10-05, 반영)

9 건, 7 건은 작은 fixture 로 재현: (1) `from_record` 가 `RESULT FAIL` 로 끝난 manifest 기록도 받았다 → `RESULT OK` 가 없으면 거절;
(2) 없는 base·아무 파일도 안 맞는 base 가 exit 0 → 둘 다 FAIL(`MISSINGBASE`, `EMPTYBASE`); (3) 상대 경로 `--base` 의 symlink 가 깨졌다 →
절대 경로로; (4) group level 3(`<PD>/<request>`)은 task 시각이 여럿이면 가장 옛 task 의 파일만 골랐다 → 기본 2(task 마다); (5) PU
`mcprofile` 이 branch 가 없는 파일에도 `RESULT OK`(events=-1) → `Draw` 의 행 수 = chain entries 를 확인, `weights` 의 정규화 실패는 exit 1;
(6) `y1_compare` 가 NaN bin 을 같다고 봤다(`max` 가 nan 을 숨김) → NaN 은 NaN 하고만 같게; (7) A 에만 보던 파일 목록 → 두 디렉터리의
합집합, 한쪽에만 있으면 FAIL; (8) tree 는 entries 만 비교 → leaf 마다 개수·합·제곱합·최소·최대·NaN 수; (9) `from_record` 가 fork 의
로컬 수정을 보지 않고 treestream.cc/.h 를 복사 → `bin/`·treestream.cc/.h 에 수정이 있으면 거절.

### 6. 묶음 2: analyzer 의 2024 (Stage 1 c, Stage 2)

- **연도 설정** (`include/EraConfig.h`, 2017/2018 의 값은 그대로): `2024` → 보정 디렉터리 `2024_Summer24`; b-tag UParTAK4 L/M/T
  0.0246 / 0.1272 / 0.4648 (BTV payload, PLAN §9.2); `isRun3`; PU jet ID 없음(PUPPI); jet ID 는 JME `jetid.json` 의
  `AK4PUPPI_TightLeptonVeto`(v9 의 `Jet_jetId >= 6` 과 같은 수준; v15 에 `Jet_jetId` 가 없다); jet veto map `Summer24Prompt24_RunBCDEFGHI_V1`,
  type `jetvetomap`; b-tag shape SF 없음(`--btagsf on` 은 E11, D10); MET filter 는 Run 3 목록 8 개(HBHE 둘이 빠지고 hfNoisyHits);
  L1 prefiring 없음; golden JSON 2026-08-04 판; `hasTtNbLookup` 거짓(D-2026-10-05-C).
- **보정** (`CorrectionsManager`): JEC compound·JER SF·JER resolution 의 입력을 **payload 가 선언한 이름으로** 맞춘다(Run 2 는 예전 순서
  그대로; 2024 MC 는 `JetPhi`, DATA 는 `run` 이 더 있다 — `run` 이 int 로 선언된 payload 면 정수로 준다). 2024 PU 는 `TTHH_PU_JSON`
  (`Collisions24_goldenJSON`, 없으면 E12; Run 2 는 이 env 를 읽지 않는다). jet ID 와 veto map 은 2024 에서만 불러온다.
- **analyzer** (`ttHHanalyzer_unified.{h,cc}`):
  - 필수 branch: 그 연도의 코드가 읽는 branch 가 header 에 있고(`choose`) 파일에 있어야 한다. 첫 event 에서 이름을 보이고 E11 로 멈춘다
    (D-2026-10-02-B). 2024 Data 는 4J3T 두 경로 중 하나만 있어도 된다. trigger-SF 기준 경로(2017 `HLT_IsoMu27`, 2018·2024 `HLT_IsoMu24`)와 MC 의
    `GenJet_hadronFlavour` 도 목록에 있다(검토 2). prescan 에도 같은 검사(run/LS/event, MC 의 genWeight·genTtbarId; 검토 3).
  - **job 의 입력 파일이 여럿이면 모두 같은 branch 집합**이어야 한다(`requireSameBranchSet_`, 아니면 E11). treestream 은 첫 파일의 집합을
    job 전체에 쓴다: 첫 파일에 없는 branch 는 다음 파일에서도 0, 첫 파일에만 있는 branch 는 다음 파일에서 exit 1. 2024 C 의 Data 는 파일마다
    HLT branch 가 다르다(D16 스캔: JetMET0 C 의 `..._PNet3BTag_4p3` 는 160 파일 중 46) — 검토 1. 그래서 2024 yml 은 `files_per_job: 1`,
    preflight 는 2024 Data 에 1 보다 크면 FAIL.
  - trigger: 2024 Data 4J3T = PNet OR DeepJet, MC = PNet(D-2026-10-05-B, PROPOSED); 6J1T·6J2T 는 PNet 경로, HT1050 그대로. 2024 Data 의 PD 는
    JetMET0/1·Muon0/1 만(OR, veto 없음 — 두 PD 는 event 를 나눌 뿐 경로를 나누지 않는다), 다른 PD 는 E11.
  - 청소: MET filter 는 연도 목록(이름 → member 표), 2024 는 같은 단계에 jet veto map 의 event veto(`jetVetoed_`: pT > 15, tight ID, EM 분율
    < 0.9, PF muon 과 ΔR > 0.2 — JERC 권고를 기억으로 옮긴 것, 확인할 것). 끝에 `[cleaning] events N: MET filters fail a, jet veto map b`.
  - v15 이름: `Rho_fixedGridRhoFastjetAll`, `PuppiMET_pt`, `Jet_btagUParTAK4B`, `Electron_mvaIso_WP90`. jet ID 는 |η| 컷 먼저, 그다음 JSON.
  - 출력 tree: `passTrigger_HLT_IsoMu24` 는 2018·2024 에서만 book(2017 tree 는 예전 그대로).
  - 시작할 때 header 의 stamp(`TTHH_EVENTBUFFER_STAMP`)를 찍는다.
- **2017/2018 에 바뀌는 것**: selection·weight 는 없다(§8 의 옛/새 빌드 비교, 검토의 줄 단위 확인). 실패하는 방식만 바뀐다: 읽는 branch 가
  없으면 0 대신 E11(2018 의 v15 입력은 이제 멈춘다 — PLAN §9 Y3), 여러 파일 job 의 branch 집합이 다르면 E11. 2018 의 출력·condor 디렉터리는
  `_2018` 이 붙는다(§7).

### 7. 묶음 2: Stage 3 (filelist, Data lumi, σ, yml, 제출기)

- `tools/stage3/make_filelists_v15.py --year 2024`: CRAB 배치 `<base>/<PD>/<key>/<YYMMDD_hhmmss>/<NNNN>/forgedNtuple_<job>.root` 에서 key(=
  NtupleForge key = 우리 샘플 이름) 마다 `filelistTier3_2024/filelist_<key>.txt`(절대 경로, job 번호 순)와 `MANIFEST.tsv`. 크기 0 파일은 빼고
  적는다(A29), `--exclude` 목록의 파일도. key 하나에 task 가 둘이면 FAIL(같은 job 이 두 번 들어갈 수 있다; `--allow-multi-task KEY`),
  `--forge-config` 의 key 가 디스크에 없으면 FAIL(MISSING).
- `tools/stage3/data_lumi_check.py`: Data filelist 의 `LuminosityBlocks`(skim 전의 모든 LS)로 PD 마다 golden LS 가 다 있는지 —
  처리된 era 의 run 범위 안에서(2024 B 처럼 만들지 않은 era 는 `GOLDEN_OUTSIDE` 로 따로; 검토 4) 빠진 LS 를 run:[범위] 로, brilcalc 용 JSON
  (`--json-out`). 같은 LS 가 PD 의 **두 샘플**에 있으면 DUPLICATE(2024 I 의 두 dataset; 그 event 가 두 번 세어진다) → exit 1. 한 샘플의 두
  파일에 있는 LS 는 정상(입력 NanoAOD 파일 경계에 걸친 LS: `split_LS`, 검토 5).
- `data/samples_2024.json` — **임시 σ (D-2026-10-05-A)**. 60 MC 모두 값이 있다; 출처는 항목마다 `xsec_ref`, 신뢰도 `confidence`, 2017 과 같은
  규약(`case`: inclusive_x_br / sample_effective). 높음: ttbar 923.6 pb(Top WG NNLO+NNLL), ttH 570 fb·tHq·tHW(Higgs WG 13.6 TeV note
  arXiv:2402.09955), tW 87.9 pb(aN3LO), ttHH 0.860 fb(ATLAS arXiv:2603.13113 이 인용한 NLO). 중간: QCD HT 8 bin(2022/23 XSDB 값,
  HT 경계 같음), V→qq(Summer24 GenXSecAnalyzer, 공개 저장소), ttV·ttVV(XSDB, 공개 저장소), diboson(13.6 TeV 논문의 이론값), t/s-channel
  (Top WG 값, 저장소의 사본으로만 읽음). 낮음: 13 TeV 값을 비율로 올린 다섯(TTbb 셋 ×1.1076, TT4b, TTTW±) — 모두 첫 plot 에 쌓지 않는 것.
  XSDB(로그인)와 CERN twiki(robots)는 읽지 못했다. BR: W→had 0.6741(PDG 2024), H→bb 0.5824, Z→bb 0.1512. Data 32 개는 null.
- yml: `AnalyzerConfig/Tier3_2024_FH_unified_{prescan,main}.yml` — lumi 109.816, xsec_db `data/samples_2024.json`, prescan
  `prescan_summary_2024/`, files_per_job 1, `path_pu_json`, 파생 보정 모두 null. prescan 은 MC 60, main 은 MC 60 + JetMET0/1 16.
- 제출기: Data 이름 규칙에 2024 PD(`JetMET0/1`, `Muon0/1`, `EGamma0/1` 를 이름 앞에서), era 는 `Run2024C-...` 처럼 뒤가 이어지는 이름에서도;
  `path_pu_json` → `TTHH_PU_JSON`(2024 만 필수, 키가 없는 2016–2018 yml 은 그대로); **출력·condor 디렉터리에 연도**(2017 만 예전 그대로:
  README §7 의 P0′ #9 — 샘플 이름이 연도마다 같아 2024 job 이 2017 디렉터리에 쓰고 `--resubmit`/`--report` 가 2017 파일을 완료로 셌을 것);
  filelist 디렉터리 기본값도 `filelistTier3_<year>`(2017 은 `filelistTier3`); 2024 preflight 는 `--btagsf on`, JetMET/Muon 이 아닌 Data,
  Data 와 files_per_job > 1 을 FAIL. 2017 의 preflight 출력(main, prescan, btagtrig)은 옛 제출기와 글자 단위로 같다.

### 8. 묶음 2 의 시험 (컨테이너)

- 빌드: ROOT 6.40(pip) + correctionlib 2.9, header 는 합성 파일(인벤토리에서 만든 2017·2018·2024 파일)의 합집합으로 만든 **임시본**
  (진짜는 KNU 기록에서). `MAKE_RC=0`, 경고 27 개 = 옛 빌드와 같은 목록.
- `test/run_unit_tests.sh`: EraConfig(2024 블록 추가)·fatal·SampleRegistry PASS. `tools/stage1/test_stage1.py` 42/42.
- `test/offline_smoke/run_offline_smoke.sh <새 빌드> <옛 빌드>`(새로 만든 도구): CVMFS·/pnfs 없이, 진짜 correction **이름과 입력**에 지어낸 값의
  payload(`fake_pog.py`), 그 연도의 진짜 leaf 형과 HLT 구성을 가진 합성 파일(`synth_nano.py`), 합성 MC 로 `pu_weights.py` 를 거친 PU JSON.
  **31/31 PASS**: 2024 MC(signal, lookup 없는 TTbar)·Data C·I 가 돌고 payload 와 청소 수를 찍는다; 2024 의 멈춤 여덟(`--btagsf on`,
  `TTHH_PU_JSON` 없음, 모르는 PD, 4J3T 둘 다 없음, jet branch 없음, `HLT_IsoMu24` 없음, genWeight 없는 prescan, branch 집합이 다른 두 파일 job)이
  제 exit code 로; 2017 넷이 돌고 **옛 빌드와 출력이 같다**(히스토그램 620 개와 tree, `y1_compare.py` PASS); 2017 TTbar 는 lookup 이 없으면
  여전히 E11; `data_lumi_check.py` 의 네 경우. 값은 무작위라 물리가 아니라 "돌고 멈추는 것"의 시험이다.
- 독립 검토(에이전트, 10-05): 2017/2018 을 바꾸는 것은 찾지 못함(MET filter 9 개, 전자 ID, jet ID, JEC 입력 순서를 줄 단위로 확인; 옛/새
  제출기의 2017 condor 파일 동일). 지적 11 건 → 반영: (1) 여러 파일 job 의 HLT branch 차이(위 §6), (2) IsoMu·GenJet_hadronFlavour 를 필수
  목록에, (3) prescan 검사, (4) lumi 점검이 만들지 않은 era 를 빠진 것으로, (5) 한 샘플 안의 LS 중복은 정상, (6) filelist 의 상대 경로,
  (7) repo 의 header 가 아직 v9 → 진짜 header 와 함께 커밋(§9), (8) 2017 tree 에 새 branch → 2017 에선 book 안 함, (9) 2018 디렉터리·
  filelist 기본값·2024 preflight, (10) JEC `run` 형과 JER resolution 입력을 이름으로, (11) 자잘한 것 셋(smoke 의 cutflow 마지막 줄, 오류 문구에
  `TTHH_PU_JSON`).

### 9. 커밋을 둘로 나누는 이유와 순서

- 지금 커밋(A): 도구·문서·입력·yml·σ 표·smoke 도구 — 빌드와 무관하다. 묶음 2 의 C++ 와 제출기, `test/test_EraConfig.cc`(2024 단언), 그리고 KNU
  기록으로 만들 `include/eventBuffer.h`·`eventBuffer_variables.txt`·`src/treestream.cc`·`include/treestream.h`·
  `DerivedCorr/PU/2024_Summer24/puWeights_2024.json` 은 **다음 커밋(B)** 에 함께. A 와 B 사이에 KNU 에서 빌드하면(특히 `build_check.sh` 의
  `make clean`) Y1 `before` 의 옛 실행 파일이 없어지고 새 코드는 v9 header 로 빌드되지 않는다. B 의 파일은 그때까지 맥의
  `NtuplizerDev/Claude outputs/STEP24_batch2_hold/` 에(같은 상대 경로, md5 표와 함께).
- 순서: A 커밋 → KNU: Y1 `before`(옛 실행 파일), manifest, PU MC 분포, P8 둘, 2024 filelist, (filelist 뒤) Data lumi 점검 → 기록 커밋 → 맥(AI): header·PU
  JSON 을 기록에서 만들고 B 를 놓는다 → B 커밋 → KNU: clean 빌드, 시험, Y1 `after` 와 비교, 2024 파일 하나씩 smoke → prescan → main →
  merge → plot (RUNBOOK §21).

### 10. KNU 첫 기록 (10-05 18:44 KST, 커밋 `595b4676`)과 그 뒤

- `test_stage1` 42/42(ROOT 6.30). PU MC 분포: ZZ 파일럿 76 파일 4,800,000 event, 평균 45.42(rms 9.54), `RESULT OK` → 이 기록으로
  `DerivedCorr/PU/2024_Summer24/puWeights_2024.json`(md5 `6d805b4d216783ff4e275071685957d9`): data 평균 50.03(nominal; up 52.35,
  down 47.72), 잃는 data 0.004 %, 가장 큰 weight 83.5(nominal, PU ≈ 70 의 꼬리: MC 가 거의 없는 곳), read-back 1.8e-15.
- filelist: exit 0 — key 92 개(MC 60, Data 32) 모두 파일, task 하나씩. 기록은 명령의 `crabConfig` 경로 때문에 `runlogs/nocommit/` 로
  갔다(runlog.sh 의 규칙: 명령에 `crab` 이 있으면) → `--forge-dir ../NtupleForge` 를 더해 명령에서 그 단어를 뺐다.
- **manifest: `MISSINGBASE 2017`, Y1 `before`: 네 입력 모두 없음** — 2017 v20 이 KNU 에서 지워졌다(사용자 확인) →
  D-2026-10-05-D. 이 기록의 합집합(142 파일, 302 record)으로 header 를 만들면 v9 이름(`Jet_jetId`, `Jet_puId`, `MET_pt`,
  `fixedGridRhoFastjetAll`, `Electron_mvaFall17V2Iso_WP90`, 2017 HLT 넷)이 없어 analyzer 가 컴파일되지 않는다(컨테이너에서 확인).
  그래서: `tools/stage1/variables_from_header.py`(지금 header 의 select·선언·resize·`output->add` 에서 record 를 다시 씀; 1782 개, 문제 0)
  → `tools/stage1/variables_v9_2017UL_fullNano_v20.txt`, manifest 는 기본으로 이것을 `--extra-variables` 로 합친다(mkvariables 의
  규칙; 2017 base 는 기본 목록에서 뺐다). 컨테이너 모의(기록의 302 + v9 1782): 새 이름 1561, 형 충돌 41(v15 의 short/uchar/ushort 대
  v9 int → int, `event` 는 ulong64 그대로), record 764, keep 27 개 모두, 새 코드가 읽는 HLT 16 개 모두 있음. 시험 48(+6: 병합 넷,
  변환기 둘).
- P8 둘과 Data lumi 점검은 도는 중(P8 기록도 명령의 `crabConfig` 로 `nocommit/`).

### 11. 커밋 B: 진짜 header 와 2024 analyzer (10-05 저녁)

- **기록**: `runlogs/run_knu_eventbuffer_manifest_20261005_105905.log`(KNU `66ea1301`; `RESULT OK`, 146 s). base 넷(2024 MC 60 dataset
  19,111 파일·Data 32·7,787, 2018 v15 MC 42·Data 8) + v9 record(`EXTRA ... records=1782 new=1357 type_conflicts=41 count_unified=54`; 형 충돌은
  short→int 12, uchar→int 27, ushort→int 1, `event` long64/ulong64 → ulong64 1). treestream 은 파일의 leaf 형으로 읽고 header 의 형으로 바꿔
  넘긴다(fork 의 iotype/srctype, `run_knu_treestream_v15check_20261005_055524.log` 의 실제 파일 시험 PASS).
- **header**: `eventbuffer_from_record.py --check-only` 다음 쓰기(fork `8be42e8`, `bin/`·treestream.cc/.h 수정 없음): variables md5
  `605e8320ea853256035e41068075807e`(764 record) = 기록, header md5_normalized `14da4cbd08def3372a5b68ff9200faf1`(9,052 줄) = 기록.
  쓴 파일: `include/eventBuffer.h` `bdc221e811c335e57d2d71aff16b7cf9`(9,054 줄, stamp
  `treestream 8be42e8; variables md5 605e8320...; record run_knu_eventbuffer_manifest_20261005_105905.log`),
  `include/eventBuffer_variables.txt` `605e8320...`, `src/treestream.cc` `c4a5873c5cc63f1c3e10f8bf7c597fbc`, `include/treestream.h`
  `c0cfff1974cdf4bdb79925599956c17c`. §10 의 컨테이너 모의 header 와 `Created:`·`Author:`·stamp 줄 밖에서 같다. `include/linkdef.h` 의 struct
  17 개는 그대로 있다(rootcling 이 빌드에서 통과).
- **C++**: 맥 `STEP24_batch2_hold/` 의 일곱(§6)에 **로그만** 바꾼 것 셋(selection·weight 는 그대로):
  (a) `[objectJet] <tagger> WP for <year>` — 2024 는 `UParTAK4 (Jet_btagUParTAK4B)`(예전 줄은 모든 연도에 "DeepJet"; 값은 처음부터 UParTAK4 의
  것); (b) `CorrectionsManager::loadJME_` 끝에 `[CorrectionsManager] JEC/JER (<year>): <file> -> JEC <key>, JER <resolution> + <SF>` (Data 는 Data
  JEC key) — 모든 job 로그에 JEC/JER payload 이름이 남는다; (c) `[branches]` 줄이 any-of 묶음의 이름마다 `: yes/no`(2024 Data 파일의 4J3T
  경로: `one of {..._PNet3BTag_4p3: yes, ..._TriplePFBTagDeepJet_4p5: no}`).
- **제출기**: `common.files_per_job_data`(Data 에만; `_files_per_job`: `--files-per-job` > Data 의 `files_per_job_data` > `files_per_job` > 1).
  2024 Data 에 1 보다 크면 preflight FAIL 에 더해 제출 때에도 그 샘플을 건너뛴다(오류 한 줄, 다른 샘플은 그대로). preflight 의 job 수는
  샘플마다 나눈 합(1 file/job 이면 예전과 같은 수; 2017 main preflight 는 글자 단위로 같다; `--files-per-job 3` 이면 예전 178 → 맞는 수 228).
  2024 yml: prescan `files_per_job: 20`(MC 60, 19,110 파일 → 983 job, 1.86 TB), main `files_per_job: 10`·`files_per_job_data: 1`(MC 1,939 + Data
  3,352 job). MC 는 dataset 마다 branch 집합이 하나다(§1 의 MC 스캔).
- **`tools/stage3/smoke_2024.sh`**(신규): 진짜 2024 파일로, condor job 과 같은 환경(`setup.sh`, yml 의 경로를 제출기 자신의 reader 와 env map 으로
  — map 은 제출기 소스에서 `ast` 로 읽어 사본이 없다): `mc_sig`(TTHHto4b), `mc_tt`(TTbar_Hadronic), `pre_sig`(prescan), `dC_pnet`·`dC_nopnet`(era C 의
  PNet 4J3T branch 가 있는/없는 파일; 파일을 열어 찾는다), `dC_mix`(둘을 한 job → E11), `dI`(JetMET1 I `_v2-v2`), `mc_multi`·`pre_multi`
  (QCD_HT200to400 의 job 0 = filelist 의 처음 files_per_job 줄: 여러 파일 job). 파일은 크기의 10 번째 백분위; Data 는 LS 의 절반 이상이 golden 인
  파일만(2024 Data 는 lumi mask 없이 만들었고 analyzer 는 golden 이 아닌 LS 를 객체를 읽기 전에 건너뛴다 — golden LS 가 없는 파일이면 아무것도 안
  보인다; era I 의 끝 run 들은 golden 밖).
  check: exit 0, stamp(**실행 파일이 이 checkout 의 header 로 빌드되었나**), `[branches]`, yml 의 jsonpog·PU 경로, payload 이름(jet ID, veto map,
  PU, JEC MC/DATA·JER, UParTAK4 WP), cutflow HadTrigger·njets≥6·HT>500 > 0(MC 는 nbjets≥2 도), 출력의 끝 표지(`cutflow_w_full`; prescan 은 한 행
  `prescan` tree — 제출기의 완료 판정과 같은 규칙), TTbar 의 tt+nb 꺼짐, era C 의 4J3T yes/no. `TIMING`(event/s, kB/event)으로 생산 시간을
  어림한다. 합성 입력으로 69/69(golden LS 가 없는 Data 파일은 건너뜀; PNet 없는 파일이 없으면 둘을 건너뜀; PU JSON 이 없으면 FAIL 과 log 꼬리 —
  확인).
- **독립 검토(10-05 저녁, 오늘 바뀐 것)**: C++ 셋과 제출기는 문제 없음(2017 main·prescan·btagtrig preflight 는 옛 제출기와 바이트 단위로 같다;
  submit·`--report`·`--status`·`--resubmit` 이 같은 `parse_config_entry` 로 나눈다; prescan 20 file/job 도 한 행 규칙을 지킨다). 지적 셋: (1) smoke 가
  Data 를 크기로만 골라 golden LS 가 없는 파일이면 거짓 FAIL → golden 조건(위); (2) MC files per job 의 근거 — MC 스캔 기록
  `runlogs/run_knu_d16_branchsig_mc_20261005_063612.log`(60 dataset 모두 branch 집합 하나)는 맥 저장소에 있다(검토한 사본에 없었을 뿐), 그래도
  production 의 나눔을 그대로 시험하도록 `mc_multi`·`pre_multi` 를 더함; (3) `--files-per-job 0` 이면 FAIL 뒤에 PASS 줄이 찍힘 → 찍지 않는다.
- **시험(컨테이너, 마지막 빌드 `cb.HIUj`)**: clean 빌드 `MAKE_RC=0`; `run_unit_tests.sh` PASS; `test_stage1` 48/48; offline smoke **34/34**(+3: JEC/JER·WP 줄,
  Data JEC, 4J3T yes/no), 2017 넷은 옛 빌드와 같다(히스토그램 620 개 + tree, `y1_compare.py` PASS). 2017 의 실제 파일 Y1 은 없다(D-2026-10-05-D).

### 12. KNU: 커밋 B 의 빌드·smoke, prescan 제출; plot 도구; 여러 파일 job 의 검사 고침 (10-05 밤)

- **빌드·단위 시험**: `knu_build_b` EXIT 0(cluster 2180633, 125 s, 실행 파일 21:12 KST), `knu_unittests_b` PASS(셋).
- **smoke 1 차(EXIT 1, 0/69)**: smoke job 을 빌드 job 과 같은 때에 냈다(12:10:04, 12:10:41 UTC; 둘 다 12:10:49 시작). 빌드의 `make clean` 이
  실행 파일을 지운 뒤 smoke 가 run 을 시작해 모두 exit 127(`No such file or directory`). 코드 문제가 아니다 — smoke 는 이제 실행 파일이 소스
  (`ttHHanalyzer_unified.{cc,h}`, `include/*.h`, `src/*.cc`)보다 오래되었으면 시작하지 않고(exit 2, "build first"), exit 127 에는 그 뜻을 붙인다.
- **smoke 2 차(cluster 2180635, 68/69)**: 진짜 파일·진짜 payload 로 mc_sig·mc_tt·pre_sig·mc_multi(10 파일)·pre_multi(20 파일)·dC_pnet·dC_nopnet·dI
  모두 PASS — JEC/JER·jet ID·veto map·PU·UParTAK4 이름, stamp, cutflow. 읽은 것: era C 의 HadTrigger 비율 PNet 있는 파일 31 %, 없는 파일 30 %(4J3T
  OR); TTHH genWeight = 1(Runs Σgenw = count = 24,500); QCD MLM 의 weight ≈ 1.7e7(prescan 이 정규화); **jet veto map 이 event 의 23–25 % 를 뺀다**
  (TTHH 25 %, TTbar 23 %, Data 23 %: Data·MC 같음 — 6 jet 이상 event 는 jet 이 많아 veto 영역에 걸릴 확률이 크다; AN 에서 따로 볼 수용도 손실).
  TIMING: main MC 2,000–3,600 event/s, prescan 3,000–5,000 event/s, Data 1,000–3,500(작은 파일, 고정 비용이 큼); 크기 1.4–2.0 kB/event.
- **FAIL 하나: `dC_mix`**(PNet 있는 era C 파일, 없는 파일 순서의 두 파일 job)가 E11 로 멈추지 않고 exit 0. `requireSameBranchSet_` 은 첫 파일의
  집합을 treestream 의 `present()` 로 봤는데, 그 목록은 `TChain::GetListOfBranches()` — `GetEntries()` 뒤 chain 이 올려 둔 tree 의 것이다.
  ROOT 6.30(KNU)에서는 그 목록에 첫 파일의 PNet branch 가 없었고(그러면 첫 파일에서 그 bit 는 0 으로 읽힌다), ROOT 6.40(컨테이너)에서는 첫
  파일이라 시험이 통과했다. 고침: job 의 모든 파일을 이 함수가 직접 열어 첫 파일과 비교한다(treestream 목록에 기대지 않음). 컨테이너 시험
  (offline smoke 의 mix, smoke_2024 합성의 dC_mix)은 그대로 E11.
  **지금 생산에는 영향이 없다**: 2024 Data 는 job 당 1 파일, MC 는 dataset 마다 branch 집합 하나(MC 스캔 기록). 그리고 고친 코드의 KNU 빌드는
  **prescan·main 이 끝난 뒤**에 한다(빌드의 `make clean` 이 돌고 있는 job 의 실행 파일을 지운다).
- **prescan 제출**(10-05 22:47 KST): 60 샘플 983 job. job 로그(ttHToNonbb job 1): 20 파일 같은 branch 집합, Runs Σgenw 1.276e6 / Events 1.028e6
  (skim), 179 만 event. 로그의 `trigSF=ON` 은 prescan 과 무관(SF 를 쓰지 않음, trigger SF 경로 null → SF=1).
- **`--report` 가 10 분 넘게 아무것도 안 찍음**(사용자): 끝난 output 마다 /pnfs 의 파일을 PyROOT 로 열어 완료를 판정하고(983 개, 파일당 0.5–2 s),
  표는 끝에 한 번 찍힌다(`| tail` 도 끝까지 모은다). 진행은 `condor_q -totals` 와 출력 파일 수(`ls .../*/*.root | wc -l`)로 보고, `--report` 는
  큐가 빈 뒤 한 번. 제출기: report/status 모드에서 샘플마다 `[report] i/N <sample>` 을 stderr 로(파이프와 상관없이 보인다).
- **plot 도구**: `plotter/make_plots.py`(신규) — yml 의 샘플마다 merge 된 `<base>/<sample>.root`; 2024 는 TTbb_*·TT4b 를 기본으로 뺀다
  (D-2026-10-05-C); `Tree/cutflow_w` 로 단계마다 MC·Data·Data/MC 와 샘플별 수율 표(`YIELDS.txt`, `--check-only` 는 여기까지); PyROOT 로
  `structure_info.yml`(TH1 618 개, `--include-hist`/`--exclude-hist`); `samples_config.yml`; `stack_plotter.C` 를 compact·detailed 로. 출력
  `condor/plots/<base 이름>_<UTC>/`(gitignore), 모든 plot 을 한 파일에 `plots_<grouping>/all_<grouping>.pdf`(617 쪽). `plotter/stack_plotter.C`:
  lumi·√s·메모 줄을 env 로(`TTHH_PLOT_LUMI`, `TTHH_PLOT_SQRTS`, `TTHH_PLOT_NOTE`; 없으면 2017 표기 그대로), 여러 쪽 PDF(`TTHH_PLOT_MULTIPAGE`),
  입력 파일은 실행 내내 한 번만 연다(예전: 히스토그램 × 샘플마다 열고 닫음 — 2024 는 618 × 72 번; 같은 그림인지 25 개 렌더링 비교로 확인),
  2024 이름의 그룹(ttHTobb_had/semilep/dilep → ttH, TTZToQQ·TTLL_*·TTNuNu → ttV, TTTWminus/plus → 3t/4t, `ST_` 로 시작 → single t).
  수율 표에는 skim 전도 있다(사용자 10-05: "정확한 cutflow 를 그리려면 skim 전 event 수도 세야"): MC 샘플마다 'generated' = 제출기의 weight
  × Σgenw(Runs) = lumi × σ × BR(prescan summary 의 Runs 합은 skim 전 모든 생성 event), skim 효율 Σgenw(Events)/Σgenw(Runs), 그리고
  noCut/(weight × Σgenw(Events)) — merge 된 파일에 main job 이 다 한 번씩 있으면 평균 PU weight 쯤, 빠졌으면 뚜렷이 작다(0.7–1.3 밖이면 WARN).
  Data 의 skim 전 수는 넣지 않는다: 생산에 lumi mask 가 없어 NtupleForge `ForgeAudit` 의 `n_in` 은 golden 이 아닌 LS 도 센다. analyzer 의
  cutflow 히스토그램 자체에 skim 전 bin 을 넣는 것은 C++ 변경이라 첫 look 뒤로.
  제출기: skim 된 생산에서 Σgenw tree < runs 는 정상이므로 `[warn] ... differ` 대신 `[skim] ... tree/runs = <효율>`(report 표에는 찍지 않음).
  blinding: 첫 look 의 plot 은 모두 preselection 이라 data 를 숨기지 않는다(D-2026-10-02-E). 컨테이너 시험: 2024 이름 76 개(가짜 입력) 모두
  그룹에 들어감(경고 0), 616 PDF + 여러 쪽 PDF, 표기 `109.8 fb^-1 (13.6 TeV)`·`2024 C-I`·`#sigma: provisional (13.6 TeV)`·`no trigger/b-tag SF`.

### 13. prescan 끝, consolidate 의 `EXIT : 1`(도구의 판정), main 제출 전 (10-06 0 시 KST)

- **prescan**: 60 샘플 983/983 job 유효, 빠진 job 번호 없음(consolidate 기록 `runlogs/run_knu_consolidate_prescan_2024_20261005_150333.log`,
  커밋 `ddb65396`; 11 s). skim 통과율 Σgenw(Events)/Σgenw(Runs): TTHHto4b 95.0 %, TTTT 98.8 %, ttHTobb_had 88.0 %, TTbar_Hadronic 52.0 %,
  TTbar_SemiLep 31.7 %, TTbar_DiLep 16.2 %, QCD_HT200to400 2.3 %, QCD_HT2000toInf 33.0 %, WW 2.0 %. tt+bbb·tt+4b(61/62/71/72)는 모두 0 —
  D-2026-10-05-C(2024 는 tt+nb lookup 없음) 그대로; 첫 look 은 TTbb_*·TT4b 를 빼고 TTbar inclusive 를 쓰므로 영향 없다.
- **`EXIT : 1` 인데 "No anomalies"**: 표의 `chk` 가 MC 60 개 모두 `FAIL`. 실패한 검사는 (4) Σgenw(Events) = Σgenw(Runs) 하나다 — Events tree 는
  6j20 skim 뒤, Runs tree 는 skim 전이라 skim 된 입력에서는 정의상 다르다(`skim%` 열). 코드 주석은 이 검사를 "informational" 이라 했지만 실제로는
  `chk` 열과 exit code 를 정했고 경고 문구가 없어서, 경고만 보는 anomaly 보고는 "No anomalies" 였다. 07-29 의 2017 prescan(skim 없음)은 61 개
  모두 이 검사를 통과해 드러나지 않았다. **숫자는 맞다**: 다른 검사(파일 유효, 빠진 번호, genTtbarId·ttCat 분할)는 모두 통과했고, 정규화는
  Σgenw(Runs)(skim 전, 제출기)를 쓴다.
- **고침(커밋 C): `consolidate_prescan.py --skimmed`.** 이 옵션이면 (4) 대신 `events_le_runs`(Events ≤ Runs: skim 은 event 를 빼기만 한다)를
  보고, 표 아래에 INFO 한 줄(`ΣgenW(Events) < ΣgenW(Runs) for n of m MC samples = the skim`). 옵션이 없으면(skim 없는 생산) (4) 가 틀릴 때
  경고 문구를 남기고 `--skimmed` 를 권한다. 실패한 검사는 모두 경고를 남기고(ttCat weight 분할은 경고가 없었다), anomaly 보고·JSON meta·exit
  code 가 같은 판정(`has_anomaly`)을 쓴다 — "No anomalies" 이면 exit 0, 그 반대도. JSON meta 에 `skimmed`. skim 없는 입력은 검사도 결과도 그대로
  (2017 summary: 경고 0, 검사 모두 통과 → exit 0). 시험 `test/test_consolidate_prescan.py`(신규; PyROOT 로 가짜 한 행 prescan 파일, 18 check —
  skim 된 입력의 옵션 없음/있음, Events > Runs, ttCat weight 분할, 빠진 job 번호, skim 없는 입력): 컨테이너 PASS 18/18; 옛 도구는 KNU 의 증상
  그대로("No anomalies" 인데 exit 1) FAIL. KNU 의 python 3.9 로 문법과 ROOT 없는 부분의 실행 확인. **consolidate 를 다시 돌릴 필요는 없다**:
  고침은 summary 의 숫자를 바꾸지 않는다. 다음 skim 된 생산의 prescan 부터 `--skimmed`(워크스페이스 RUNBOOK §23 F).
- **main preflight**(사용자, 10-06): 33 PASS, 0 WARN, 0 FAIL, `READY TO SUBMIT`(~5,291 job; files per job MC 10·Data 1; prescan coverage 60).
  커밋 B 의 제출기로 내면 제출 기록에 MC 샘플마다 `[warn] <sample>: sumGenW runs=… vs tree=… differ >0.01% (using runs)` 한 줄(60 줄) — 같은
  skim 차이다(커밋 C 의 제출기는 `[skim]` 줄).
- **Data lumi**(10-05 기록 `runlogs/run_knu_data_lumi_2024_20261005_094727.log`, 커밋 `886e87b9`, `RESULT OK`): golden LS 가운데 생산에 없는 것
  JetMET0 5,859 / 281,662 (2.08 %), JetMET1 1,981 (0.70 %). 두 PD 는 같은 LS 의 event 를 나눠 받으므로 Data 는 109.816 fb⁻¹ 의 1.4 % 쯤(LS 수
  기준)이 빠진 셈 → 첫 look 의 Data/MC 가 그만큼 낮을 수 있다. 처리된 LS 의 brilcalc 가 다음(빠진 LS 는 KNU 의
  `condor/lumi_2024/JetMET{0,1}_missing_golden.json`). `GOLDEN_OUTSIDE` 16 run(378985–379355, 5,939 LS)은 era B — C–I lumi 밖이라 상관없다.

### 14. 실패 확인 장치의 점검과 보강 (10-06; 커밋 D)

- **물음**(사용자 10-06): "job 이 fail 난 것을 어디서 확인 가능하며 모든 작업 플로우에서 확인 후 resubmit 이 가능하게 했는가?
  에러나 fail 을 제대로 체크할 수 있도록 코드가 잘 짜여져 있나?"
- **점검(코드를 다시 읽음)**: analyzer job(prescan·main)은 갖춰져 있다 — 번호 붙은 exit code(`docs/reference/ERROR_CODES.md`)로
  fail-fast, condor 로그의 return value, 제출기 `--report`/`--status`(끝 표시로 완료 판정)와 `--resubmit`(빠진 job 만 같은
  번호로). consolidate 는 빠진 job 번호·열리지 않는 출력·교차 검사, merge 는 hadd 의 실패(못 여는 입력이면 실패)와
  `[hadd] OK`, make_plots 는 `MISSING`·`FLAG`, 빌드·smoke·plot 은 `tools/runlog/status.sh` 와 기록의 `EXIT`.
  **빈틈**: (1) exit 0·끝 표시가 있는데 입력 파일을 건너뛴 job — treestream 은 첫 파일만 직접 열고(못 열면 exit 1) 나머지는
  TChain 에 맡기며, TChain 은 못 여는 파일을 `.err` 에 오류만 남기고 0 event 로 친다; prescan 의 Runs 읽기도 못 연 파일은
  경고만(`[Prescan][WARN] cannot open`) — Runs 합이 줄면 MC weight 가 커진다. 커밋 C 의 analyzer(여러 파일 job 의 모든 파일을
  시작에 직접 엶, 못 열면 E11)가 대부분 막지만 이번 생산은 커밋 B 실행 파일. Data 는 job 당 1 파일이라 해당 없음.
  (2) merge 는 디렉터리의 `.root` 를 모두 합침 — job 수 대조 없음(빠지면 적게, 예전 제출의 파일이 남으면 두 번).
  (3) make_plots 의 noCut/기대값은 평균 PU weight 라 0.7–1.3 밖만 — job 하나(1–2 %)는 안 보임; RUNBOOK §23 J 의 grep 이
  `WARN` 을 빠뜨렸다. (4) `--report` 의 miss 는 실패·대기·실행 중을 구분 못 함; treestream `read()` 는 LoadTree < 0 이면 멈추지
  않고 돌아와 직전 event 값이 남는다(드묾).
- **이번 생산의 사후 확인**(10-06 4 시 KNU, 사용자): main 5,291 job 모두 return value 0, `--report` 100 %; merge 76/76
  return value 0·`[hadd] OK`, 입력 5,291 개 = job 수; job `.err` 의 ROOT 입출력 오류(prescan·main) 0; MC 60 샘플 모두 merge 된
  noCut(가중치 없음) = prescan event 수, prescan 이 Runs 를 읽은 파일 수 = filelist 줄 수; make_plots WARN 없음, 샘플별
  noCut/기대값 0.995–1.010 → **이번 생산은 job·파일 빠짐 없음**.
- **보강(커밋 D, Python — 빌드 없이 이번 생산에도 쓴다)**:
  `plotter/make_plots.py` — MC 정확 개수 검사: merge 된 `Tree/cutflow` 1 번 bin(가중치 없음) = prescan `nEvents_total`(같은
  filelist; MC 는 모든 event 가 noCut 에 닿는다). TH1F 를 hadd 가 float 로 더하는 반올림을 고려해 max(2, 1e-5 N) 넘게 다르면
  FLAG(RESULT FAIL); `EVENTS MC n of m ...` 줄; RESULT 에 WARN 수. `outputMerger/merge_outputs.py` — `--config <분석 yml>`
  (+`--filelist-dir`): 제출기와 같은 나눔으로 프로세스마다 job 수 N 을 구해 `<proc>_0..N-1` 과 대조, 빠진 번호·남는 파일이
  있으면 합치지 않음(`--allow-incomplete` 로만), yml 에 없는 디렉터리·디렉터리가 없는 샘플(ABSENT) 표시, worker 에 N 을 넘김;
  `--report`: `_merge_workdir/<base>_<stamp>/` 의 기록(arguments 줄 = ProcId, condor 로그, `.out` 의 `[hadd] OK`, 합친 파일)으로
  프로세스마다 가장 최근 시도의 상태(ok/failed/held/pending/not-merged), 정상 아니면 exit 1; `--resubmit`: failed·not-merged
  ·removed 만 다시(pending·held 는 그대로; `--skip-existing` 은 반쪽 파일도 건너뛰므로 재시도에 쓰지 않는다).
  `outputMerger/run_one_hadd.sh` — `<indir 이름>_*.root` 만 합치고, 4 번째 인자 N 과 입력 수가 다르면 exit 7(3 번째 인자 `-` =
  cmsenv 없음). `consolidate_prescan.py --filelist-dir DIR` — MC: Σ nFiles = `DIR/filelist_<sample>.txt` 줄 수(다르면 경고 →
  anomaly). `submit_job_FH_Tier3_unified.py` — `--report` 표에 `fail`(가장 최근 시도가 0 아닌 return value·signal·condor_rm,
  또는 rc 0 인데 끝 표시 없음)·`wait`(idle/running/held), `--status` 는 빠진 job 마다 cluster.proc·return value 와 이름·`.err`
  경로(샘플의 condor 로그, job `.out` 의 `[ output file name ] -->`, 마지막 제출의 arguments 파일로 job 번호를 찾는다).
  `docs/reference/ERROR_CODES.md` 끝에 도구의 코드(treestream 1, 127/137/143, run_one_hadd 2–7 등).
  시험: `test/test_failure_checks.py`(신규, 24: make_plots 개수 FLAG·float 여유, merge 의 대조·local merge·worker exit 7·
  `--report`·`--resubmit` 선택, 제출기 진단 여섯 경우·표), `test/test_consolidate_prescan.py`(24, `--filelist-dir` 셋 더함);
  컨테이너 PASS, python 3.9 문법.
- **보강(커밋 E, C++ — KNU 다시 빌드 `knu_build_c` 로 들어간다)**: `requireSameBranchSet_`(main 은 첫 event, prescan 은
  시작)이 job 의 모든 파일을 직접 열 때, 못 여는 파일(파일 없음·`Events` 없음)은 E30(`INPUT_OPEN_FAIL`, 예전 E11)으로 멈추고,
  파일별 `Events` entry 의 합이 chain 의 entry 수(`_ev->size()`)와 다르면 E30; 1 파일 job 도 센다. 새 줄
  `[inputs] <n> input file(s), Events entries <N> = the chain's.`(여러 파일 job 의 `[branches] ... in each.` 줄은 그대로 — smoke 가
  읽는다). prescan 의 `readRunsTreeSums`: MC 에서 파일을 못 열거나 `Runs` tree·`genEventSumw` 가 없으면 E30(Data 는 경고 그대로),
  읽은 파일 수 = job 의 파일 수. 확인(컨테이너, ROOT 6.40): TChain 이 못 여는 파일·0 바이트·없는 파일을 오류 줄만 남기고
  건너뛰어 GetEntries 가 좋은 파일의 합만 낸다(5 + 7 = 12); 빌드 OK; offline smoke 38/38(새 넷: 두 파일 job 의 `[inputs]`,
  1 파일 job 의 `[inputs]`, 둘째 파일을 못 여는 job → E30, Runs 없는 MC prescan → E30; 2017 출력은 옛 빌드와 같음); unit PASS.
  **정정**: 점검의 (4) 뒷부분 — "treestream `read()` 가 LoadTree < 0 이면 직전 event 값이 남는다" — 는 analyzer 경로에서는 틀렸다.
  analyzer 는 `eventBuffer::read` 를 거치고, 그것이 `input->read(entry) < 0` 이면 `** eventBuffer::read - cannot load entry` 를
  찍고 exit 1 로 멈춘다(fork `8be42e8` 의 생성기, 2026-10-03 패치). 그래서 treestream 은 고치지 않았다.
- **첫 look 의 수율**(위 확인 뒤 다시 낸 `--check-only`): QCD 가 HT>500 에서 MC 의 90 %, nTotal 에서 78 %. 다른 MC 를 그대로
  두고 Data 에 맞추려면 QCD 에 HT>500 0.55, nb≥2 0.49, nTotal 0.46, nb≥4 1.27 — LO QCD 가 전체로 2 배쯤 많고 b-jet 이 많은 쪽
  비율은 낮다(trigger·b-tag SF 없음). 장부 문제는 아니고, 분석에서 QCD 를 데이터로 정규화해야 한다는 신호.

### 15. KNU smoke 의 `dC_mix` exit 139 — 멈춘 job 이 제 exit code 로 끝나게 (10-06; 커밋 F)

- **본 것**(10-06, KNU, 사용자): 커밋 D(`ef4c87a0`, Python 만 — E 의 C++ 는 맥에 커밋되지 않은 채 남았다)를 받아 `knu_build_c`(exit 0;
  실행 파일은 커밋 C 의 analyzer) 다음 `knu_smoke_2024_c`: `RESULT: 68 PASS, 1 FAIL`, `EXIT : 1` —
  `CHECK dC_mix: two era-C files with different HLT branches in one job stop (E11) FAIL (exit 139)`. 같은 때 plot `knu_plots_2024` 는 exit 0.
- **원인(판단)**: 커밋 C 의 `requireSameBranchSet_`(E 도 끝내는 길은 같다)은 job 의 두 파일을 직접 열어 다름을 찾고 `[FATAL][E11] ... do not have the same
  branches` 를 찍은 뒤 `std::exit(11)` 을 불렀다. `std::exit` 는 exit 때 정리 — ROOT 의 end-of-process cleanup(열린 파일을 닫고 그 안의
  객체를 지운다)과 static 소멸자 — 를 돌리고, ROOT 6.30(CMSSW_14_2_1, KNU)에서 이것이 segfault 하면 job 은 11 대신 139(SIGSEGV)로
  끝난다. 2026-07-06 Data job 의 era 검사 두 exit 에서 같은 일이 있었다(CHANGELOG; 그때는 그 두 곳만 `std::_Exit`). 컨테이너(ROOT 6.40)는
  정리에서 죽지 않아 offline smoke 의 같은 경우가 11 이었다. (확인 필요: KNU `condor/smoke_2024_<UTC>/dC_mix.log` 의 끝에
  `[FATAL][E11]` 줄이 있는지 — 있으면 판정은 했고 끝내는 길만 틀렸다.)
- **고침(커밋 F, C++ — E 와 함께 KNU 다시 빌드)**: `include/ExitCodes.h` 의 `tthh::fatalExit(code)` — 출력(`std::cout`·`clog`·`cerr`, stdio)을
  flush 하고 `std::_Exit(code)`: exit 때 정리를 건너뛴다. analyzer 의 fatal 경로는 모두 이것으로 끝난다: `std::exit` 42 곳
  (`ttHHanalyzer_unified.cc` 18, `ttHHanalyzer_unified.h` 3, `src/CorrectionsManager.cc` 12, `src/ExpandedTtbarId.cc` 5,
  `src/StitchFactors.cc` 1, `include/EraConfig.h` 1, `include/ConfigPath.h` 2)과 헤더의 `std::_Exit` 2 곳(flush 가 더해짐).
  우리 코드 밖의 `exit()` — treestream, `include/eventBuffer.h`(생성된 것; 읽기 실패 `** eventBuffer::read - cannot load entry N;
  stopping` 의 exit 1), `src/tnm.cc`(인자 검사, 못 여는 filelist) — 는 `main()` 이 등록하는 `on_exit` handler 가 받는다: 맨 먼저
  (`(void)gROOT` 뒤 — 이 줄이 ROOT 의 `InitInterpreter` 를 불러 TROOT 를 지우는 handler 를 먼저 등록시킨다, 지우면 안 됨)와 event loop
  바로 전, 두 번. glibc 는 exit 때 handler 를 나중 등록부터 부르므로, 이 handler 는 그 전에 등록된 것(ROOT 의 end-of-process cleanup,
  라이브러리의 static 소멸자) 앞에서 돌고, status 가 0 이 아니면 `tthh::fatalExit(status)`. 그 뒤에 등록된 것(함수 안 static, 도중에
  읽는 dictionary 의 것)과 thread_local 소멸자는 그보다 먼저 돈다 — 그래서 이 길은 최선의 노력이고 확실한 길은 `fatalExit` 이다.
  status 0(정상 끝)은 정리를 그대로 한다. `on_exit` 는 glibc 의 것이라 그 밖에서는 등록하지 않는다(KNU·lxplus·컨테이너는 glibc).
  멈춘 job 의 출력 파일은 닫히지 않은 채 남는다(ROOT 가 복구해 열 수는 있다). 끝 표시 `cutflow_w_full` 이 없으므로 제출기
  `--report` 는 예전과 같이 미완료로 센다(offline smoke 의 멈춘 출력으로 확인). 정상 job 의 경로·출력은 같다. 제출기의 exit code
  이름표(`_EXTRA_RC`)의 139 에 "`[FATAL]` 줄 바로 뒤면 `fatalExit` 전 빌드" 를 더했다; `docs/reference/ERROR_CODES.md` 에 절 하나.
- **독립 검토(subagent, 10-06)로 더한 것**: (1) `src/tnm.cc` 의 `error()` 가 `exit(0)` 이었다 — `fileNames()` 가 filelist 를 못 열면
  `** error ** unable to open file` 을 찍고 **exit 0**(제출기는 끝 표시 없음으로만 잡았다) → exit 1. (2) `on_exit` 의 범위를 위처럼 바로 적고
  event loop 전에 한 번 더 등록. (3) b-tag reweight 의 FATAL 두 줄이 옛 번호 "exit 45"/"exit 46" 을 찍었다(실제 80/81, STEP 18 의 번호) →
  `[FATAL][E80]`·`[FATAL][E81]` 과 실제 번호, stitch 의 `[FATAL][stitch]` 를 `[FATAL][E63][stitch]` 로(`[FATAL]\[E[0-9]+\]` grep 에 걸리게);
  `README.md`·`docs/RUNBOOK_2017_SF_rederive.md` 의 45/46 도 80/51/81 로. (4) 시험에 대조(`return 3` 이 `crash_teardown.so` 아래 139)를 더해
  라이브러리가 안 실린 채 PASS 하지 않게. 그대로 둔 것: CorrectionsManager·ExpandedTtbarId·StitchFactors·EraConfig 의 FATAL 줄은
  `[FATAL][E<code>]` 꼴이 아니다(번호는 맞다; 꼴은 다음에).
- **시험(컨테이너, clean 빌드)**: 빌드 OK(경고 27, 전과 같음). offline smoke 43/43 — F 의 다섯: 대조(`return 3` 만 하는 프로그램이
  `crash_teardown.so` 아래 139), 그 라이브러리(`LD_PRELOAD`; exit status 가 0 이 아니면 exit 때 handler 에서 SIGSEGV — KNU 의 증상을 흉내)를
  건 `--btagsf on`(E11)·두 파일 E11·`--sample` 없는 실행(`tnm.cc` 의 exit 1, `on_exit` 경로)이 각각 11, 11, 1, 못 여는 filelist → exit 1.
  같은 스크립트로 F 없는 빌드(E)는 대조만 PASS, 넷은 FAIL(139, 139, 139, 0) — 증상 재현; 옛 빌드도 139(`NOTE` 줄). 2017 출력은 옛 빌드와
  같음; unit PASS; `test/test_failure_checks.py` 24/24, `test/test_consolidate_prescan.py` 24/24.
- **KNU 에서 볼 것**: 맥에서 E·F 커밋 → KNU pull → `knu_build_f` → `knu_smoke_2024_f` 에서 `RESULT: 69 PASS, 0 FAIL`(`dC_mix` exit 11),
  analyzer 로그(`condor/smoke_2024_<UTC>/*.log`)에 E 의 `[inputs] ... = the chain's.` 줄(RUNBOOK §23 M).

### 16. 첫 plot 보기 (10-06; `knu_plots_2024`, 맥 `~/claude/NtuplizerDev/plots_2024/all_{compact,detailed}.pdf`)

- **출력**: PDF 둘, 각 619 쪽(그림 618 + 끝 쪽; ROOT 6.30/09, 2026-10-05 19:18 UTC): `jet/` 의 JEC 검증 24, cut step 0–13 마다 변수 42
  (jet1–6 의 pT·η·φ·b-tag, HT, had W 질량, Higgs 후보 질량, `ht`/`hadW`/`nbjets`/`njets` 의 raw·btagSF·full) = 588, `TtCatValidation` 2,
  cutflow 4. detailed 는 그룹을 더 나눈다(V+jets 와 VV, ttH 와 tH, 3t/4t, tt+VV/VH).
- **수율**(마지막 단계 `nTotal` = HT>500·nb≥2·30<HadW<250; `nbjets>=3`·`>=4` 는 관찰 전용, `HiggsMassWindow` 는 비활성 — 코드의 표 그대로):
  Data 5.02e6, MC 8.70e6(QCD 6.76e6 = 78 %, tt 1.74e6), ttHH 9.2. cutflow 의 Data/MC: trigger 뒤 0.43–0.44, HT>500 0.60, nb≥3 0.98, nb≥4 1.21.
- **HT 에 따른 Data/MC**(trigger 뒤, step 1): HT 500 GeV 에서 0.1 쯤, 700 에서 0.45, 900 에서 0.9, 1000 에서 1 에 닿고 1150–1500 에서
  1.05, 그 위로 천천히 내려가 4000 에서 0.82. HT>500 뒤(step 7)도 같은 모양. 곧 모자람은 HT < 1000 GeV — 오프라인 HT 가
  `HLT_PFHT1050` 의 plateau 아래라 b-tag multijet 경로(4J3T, 6J PNet)로만 들어오는 곳 — 에 몰려 있고 그 위는 Data ≈ MC. 그러므로 §14 의
  "QCD ×0.46–0.55" 는 고른 정규화가 아니라 그 경로들의 Data/MC 차이로 보인다: trigger SF 가 없으니 MC 의 online b-tag 효율이 높거나,
  그 경로로만 들어온 event 가 JetMET PD 에 다 있지 않거나(확인 필요: Stage 6 의 경로별 효율, 2024 HLT 메뉴의 PD 배정).
- **b-jet 많은 쪽**: nb≥4(step 10)에서 HT > 900 GeV 의 Data/MC 가 2 쯤(ratio 칸 [0, 2] 밖이라 점이 안 보이는 bin 이 있다), 낮은 HT 는
  위와 같은 turn-on. b-tag SF 없음과 QCD 의 b 많은 쪽 모델링 몫. njets(step 6)는 6 에서 0.45, 13 이상에서 1.6(HT 와 상관).
- **그림 꼴**(물리 아님): 쪽마다 왼쪽 ~27 % 가 빈칸(쪽 = A4 를 자른 550×567 pt 에 90° 회전), `Total MC` 글이 그림과 겹침, ratio 칸 밖의
  점은 안 보임, cutflow 의 끝 label 이 축 제목과 겹침, `TtCatValidation` 쪽은 MC 만(Data 0), raw cutflow 쪽(`cut step`, MC 6.17e7)의 비율은
  뜻 없음. 고칠지는 사용자 결정.

## 확인하지 못한 것

- TTbar_Hadronic 의 `260930_162708/0000/forgedNtuple_443.root` 는 크기 0(10-02 18:48 KST; NtupleForge `docs/05_troubleshooting.md` A29):
  원인(CRAB 이 그 job 을 무엇으로 아는지)은 아직. 2024 의 완결성(dataset 마다 job 수 = DAS 파일 수, 열리지 않는 출력)은 NtupleForge P8 집계로 본다(RUNBOOK §21).
- MC PU 분포를 premix 시나리오의 확률표(`SimGeneral/MixingModule` 의 cfi)로 확인하는 것: Summer24 의 시나리오 이름을 모른다(파일럿의 실현
  분포로 충분하다고 보고 미룸).
- Run 3 MET filter 목록과 jet veto 의 jet 조건(pT > 15, tight, EM < 0.9, muon ΔR 0.2)은 기억에서 옮긴 것 → JME/JERC 의 2024 권고로 확인할 것
  (PLAN §9.2 의 칸). `jetid.json` 의 설명은 "Run3 Rereco2022CDE"(인벤토리) — 2024 전용 기준이 따로 없는지 JME 로.
- `data/samples_2024.json` 의 σ 는 모두 임시. 낮은 신뢰도 다섯(비율로 올린 것)과 TTZHTo4b(XSDB 의 exact-dataset 값이 B(Z→bb) 만 포함한
  것으로 보여 쓰지 않음)는 첫 plot 에 영향이 없거나 작다. XSDB 로 확인할 사람: CERN 로그인이 있는 사용자.
- D-2026-10-05-B(2024 MC 의 4J3T 는 PNet 만)는 PROPOSED — Stage 6 의 trigger SF 와 함께 정한다.
- 커밋 E·F 의 KNU 빌드와 smoke(`dC_mix` 가 11 로 끝나야 함, 69/69): 아직(§15). `dC_mix` 의 139 가 exit 때 정리의 segfault 라는 판단은
  KNU 의 `condor/smoke_2024_<UTC>/dC_mix.log`(그 끝에 `[FATAL][E11]` 줄이 있는지)로 확인할 것(RUNBOOK §23 M 의 첫 명령). 커밋 C 의 analyzer(+D) 빌드의 smoke 는 68/69(`dC_mix` 139, §15),
  커밋 B 의 것은 §12(68/69).
- Data 의 처리된 LS 의 lumi(brilcalc): golden LS 의 1.4 % 쯤이 생산에 없다(§13) — 그 전까지 Data/MC 는 109.816 fb⁻¹ 기준.
