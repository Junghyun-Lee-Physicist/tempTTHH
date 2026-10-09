# Changelog — ttHH(HH→bbbb) Fully-Hadronic Analysis

> **Purpose:** one chronological line per change, newest first, linking to the full record. The detail lives in [`changes/STEP_*.md`](changes/); this file is the index, not a copy.
> **Audience:** anyone tracing when and why something changed.
> **Status:** append-only · last meaningful update **2026-10-09** (M).
> **Links:** decisions [`DECISIONS.md`](DECISIONS.md) · state [`STATUS.md`](STATUS.md).

> Append-only: add new entries at the top; do not rewrite history. "Detail" links point to the full per-step record.

## 2026-10-09: [merge][도구] STEP 26 M — merge job 의 stall guard, 큐에 그 프로세스의 merge·analyzer job 이 있으면 merge 하지 않음

KNU 10-09: FH merge(cluster 2181958)의 84 개 중 ParkingHH_Run2024F(입력 599)가 /pnfs 에서 멈춘 채 4 시간 'running'(CPU 14 s / 15,294 s, 출력
73,596 byte 그대로) — K2 의 stall guard 는 analyzer 의 submit 파일에만 있었다. `outputMerger/merge_outputs.py`: `merge.sub` 에 제출기의
`stall_guard_exprs()` 다섯 줄(`--stall-guard on|off`, 기본 on; 다시 시작한 hadd 는 `-f` 로 파일을 새로 만든다); 합치기 전에(local·condor·`--dry-run`)
`condor_q -af:j Args Arguments` 로 큐(X 포함)의 job 이 합칠 `<base>/<proc>.root`(같은 프로세스의 merge — 둘이 함께 돌면 반쪽 파일이 남아도
`--report` 가 ok) 나 입력 `<base>/<proc>/<proc>_<N>.root`(그 프로세스의 analyzer job — 출력은 시작 때 생겨 개수 대조를 통과)를 인자로 가지면 exit 2
(링크로 닿은 같은 파일도; 상대 경로는 보지 않음, `--base` 는 절대 경로로); 큐를 못 읽으면 condor 제출은 exit 2; condor_submit 이 실패하거나
중단된(Ctrl-C, SIGTERM) 시도는 `submit_failed.txt` → 로그에 사건이 없는 job 은 `--report` 의 failed(전에는 영원히 pending), `--resubmit` 이 다시 고름;
`--dry-run` 의 work 디렉터리는 `dry_run.txt` 로 표시, 제출하지 않은 job 은 `--report` 가 보지 않음(전에는 pending 으로 남음; 손으로 제출한 것은 그
로그가 정함); 시도마다 work 디렉터리 하나(같은 초의 두 실행이 한 디렉터리를 나눠 쓰던 것).
`tools/runlog/condor_run.sh`: `--stall-guard on` 일 때 `job.sub` 에 같은 다섯 줄(기본 off — /pnfs 훑기는 건강해도 CPU 5 % 미만일 수 있고
`y1_reference.sh` 는 다시 시작하면 실패; plot·TriggerStudy·smoke·빌드에 켠다). 시험: `test_failure_checks.py` 76/76(+13, I; ClassAd 모듈이 있으면 77;
condor_q·condor_submit 은 가짜, 실행할 수 없으면 I 의 condor 실행은 하지 않음), `tools/runlog/test_runlog.sh` 85/85(+2, T30). 독립 검토 두 번 반영.
상세 [`changes/STEP_26_trigger_sf_2024.md`](changes/STEP_26_trigger_sf_2024.md) §9.

## 2026-10-09: [TriggerStudy][analyzer][제출기] STEP 26 L — 2024 trigger SF: TriggerStudy 의 연도, JSON 의 연도 확인 (Stage 6)

TriggerStudy: env `TTHH_YEAR`(없으면 2017 = 전의 동작; 2017·2024 만) — 2024 는 기준 `HLT_IsoMu24`, hadronic OR 은 `_CDEF` slot(analyzer 가 넣는
PNet 경로; `_B` 가 켜지면 exit 1), Data 는 xsec_db 의 Muon0/1 16; `run_analysis.sh` 다시 씀(`--year`, `--workdir`(2024 기본 `run_2024/`), `--jobs`,
절대경로, 표본마다 로그와 `[TrigStudy]` 줄, macro 의 exit code 와 마지막 RESULT); EventLooper 가 다른 해의 입력에 exit 1(2017 인데
`passTrigger_HLT_IsoMu24` 가 있는 skim, 2024 인데 없는 skim, xsec_db `_meta.era`, prescan 의 해, Data 이름의 해, 기준이 안 켜진 표본, Step 2 의 JSON)
과 출력마다 `TrigStudyStamp`; DeriveSF 는 int(0/1), 다른 해의 Step 1 출력·map 없는 category·측정 bin 0 개의 category 에 RESULT FAIL, JSON 은
`.tmp` → OK 일 때만 `trigger_sf.json.gz`(FAIL 이면 `.FAILED`, 있던 것은 `.previous`; 지우지 않음), description 셋에 `year=; reference=; hadronic OR:`;
PlotTriggerEfficiency 는 lumi 표기·stamp·RESULT. `include/SampleRegistry.hh`: 2024 Data 이름(`<PD>_Run<YYYY><E>-...`), `xsecEra()`, `prescanYear()`.
analyzer `CorrectionsManager::loadTrigger_`: 다른 해의 trigger SF JSON(태그 없음 = 2017)은 E50(MC, 모든 mode). 제출기: preflight 의 `trigger SF JSON`
줄, 제출 때 첫 표본 전 E50(다른 해·입력 순서는 모든 mode, 파일 없음·못 읽음은 main/debug). `bTagSF_ReweightStudy`: 모르는 Data PD 는 시작 때 FATAL.
시험: `test/trigger_study/run_trigstudy_synth.sh` 32/32(합성 skim 의 Data/MC 비를 117 칸에서 되찾음, 2017 은 K2 빌드와 bin 단위로 같음), 오프라인
smoke 95/95(+12), `test_failure_checks.py` 64/64(+4, H), `test_SampleRegistry` 59(+12). 독립 검토 두 번 반영. `TriggerStudy/Makefile`·
`bTagSF_ReweightStudy/Makefile` 그대로. 상세 [`changes/STEP_26_trigger_sf_2024.md`](changes/STEP_26_trigger_sf_2024.md).

## 2026-10-08: [제출기] STEP 25 K2 — condor stall guard: CPU 를 쓰지 않는 'running' job 을 hold 하고 release

KNU 10-08: 2024 μCR·eCR 의 마지막 analyzer job 7 개가 /pnfs 입력 읽기에서 멈춘 채 12 시간 'running'(CPU 2–320 s / 42,000–47,000 s, `.out` 은
05:24–06:05 KST 이후 그대로, 6 개가 cluster333); condor 는 프로세스가 살아 있는지만 본다. `submit_job_FH_Tier3_unified.py`: 모든 submit 파일
(제출·`--resubmit`)에 `periodic_hold`(지금 run > 1 h 이고 CPU(user + system) < 그 시간의 5 %), `periodic_hold_reason`(`tthh stall guard: running
<min> min with <s> s CPU on <slot@machine>`), `periodic_hold_subcode` 4201, `periodic_release`(그 hold 만, 5 분 뒤, NumJobStarts < 3: 같은 job 이
처음부터 다시), `requirements`(LastRemoteHost 의 machine 은 피함); `--stall-guard on|off`(기본 on), preflight 의 `condor stall guard` 줄, 제출 때
`[stall-guard]` 줄, `--report` 의 wait 안내. 시험 `test_failure_checks.py` 59/59(+3), ClassAd 모듈(classad2 / classad)이 있으면 60/60(식 평가).
상세 [`changes/STEP_25_btag_fixedWP_2024.md`](changes/STEP_25_btag_fixedWP_2024.md) §10.

## 2026-10-08: [analyzer][도구][제출기][config] STEP 25 K — 2024 b-tag SF 는 fixed WP(method 1a), 효율 map 도구, 2024 btagtrig yml (D-2026-10-08-A)

사용자 결정 D-2026-10-08-A: 2024 는 지금 BTV 의 fixed-WP 방법, shape 방법이 확인되면 전환. analyzer: `EraConfig::btagMethod`(2024 FixedWP,
Run 2 Shape)와 `btagFixedWPPayload`(`btagging_preliminary.json.gz` → `UParTAK4_kinfit`, b jet 만); `CorrectionsManager` 가 그 payload(입력 이름
확인, 격자 평가, up/down 은 payload 의 total → correlated+uncorrelated → kinfit 출처들의 제곱합 순으로 찾음(central 과 달라야 셈; 못 찾으면
WARN); 못 읽으면 WARN)와 우리 효율 JSON(env `TTHH_BTAGEFF_JSON`; main/debug 에서 못 읽거나 이 job 의 묶음·flavour·WP 에 답하지 못하거나 다른 WP 로
만들었으면 E52)을 읽고, `computeBTagWeightFixedWP_` 가 jet 마다 세 칸(≥ M, L–M, < L)의 P_Data/P_MC 곱을
`bTagWeight`(+ `bTagWeight_up/_down` branch)에; `--btagsf on` 은 MC 에 payload(E40)와 효율 JSON(E52)을 요구(전의 E11 대신); MC 출력에 `BTagEff/`
(HT 단계의 선택 jet, flavour × {all, ≥L, ≥M, ≥T}, pT × |η|, `h_nevt`, `h_wp`), job 끝에 closure 줄(Σ w·bTagWeight / Σ w ≈ 1)과 jet weight 수
줄; 새 exit code E52 `BTAGEFF_LOAD_FAIL`; `include/BTagEffGroup.h`(tt·qcd·other). `tools/stage7/btag_eff_maps.py`(+ 시험 26): merge 된 MC 의
`BTagEff/` 를 묶음마다 더해 correctionlib JSON(`btag_eff`, `btag_eff_groups`; N_eff < 50·b 의 tag 칸 N_eff < 10·비물리 칸은 |η| 합 → 'all';
WP·flavours 를 description 에). 제출기: `path_btag_eff_json`(2024 의 모든 job 에
`TTHH_BTAGEFF_JSON`, `--btagsf on` 이면 MC 에 필수), preflight 가 효율 JSON 을 읽어 확인. yml: 2024 main·prescan 에 `path_btag_eff_json: null`,
새 `Tier3_2024_FH_unified_btagtrig.yml`(Muon0/1 16 + MC 60). plotter: `BTagEff/` 는 그리지 않고, SF 표기가 2024 b-tag 을 "fixed WP, b jets" 로.
시험: 오프라인 smoke 83/83(+33: payload·E52·E40·효율 도구·tree 에서 다시 계산한 weight·closure·sources payload·default payload·다른 표본·읽히지만
답하지 못하는 JSON), 실패 확인 56/56(+7), 단위 시험 PASS(EraConfig +4), 컨테이너의 smoke_2024 모의 106/106; 2017 과 2024 의 기존 출력은 J 와
같다(새 것은 `BTagEff/` 와 두 branch 뿐). 독립 검토(AI agent)의 결함 둘(0·1 효율, 답하지 못하는 JSON 의 abort)을 고침. 상세
[`changes/STEP_25_btag_fixedWP_2024.md`](changes/STEP_25_btag_fixedWP_2024.md), [`DECISIONS.md`](DECISIONS.md) D-2026-10-08-A,
[`reference/ERROR_CODES.md`](reference/ERROR_CODES.md) E52, [`reference/CONFIG_PATHS.md`](reference/CONFIG_PATHS.md) `path_btag_eff_json`.

## 2026-10-07: [제출기][data] STEP 24 J — main 제출이 76 cluster 뒤 E20: Data 의 xsec_db 항목, 모든 표본의 weight 입력을 먼저, `--only`

KNU 의 2024 main 제출(커밋 H 의 yml)이 MC 60 + JetMET 16 = 76 cluster 를 낸 뒤 `[FATAL][E20]`(ParkingHH 가 `data/samples_2024.json` 에
없음)으로 멈췄다 — preflight 는 xsec_db 를 MC 표본에만 보았다. `data/samples_2024.json`: ParkingHH 8 항목(σ null). `submit_job_FH_Tier3_unified.py`:
`weight_input_problems()`(preflight 와 제출이 같이 씀; Data 도 항목이 있어야 하고, 이름과 항목이 어긋나면 문제), preflight 의 `xsec_db coverage
(Data)` 줄, 제출 전에 모든 표본을 한 번에 보고 하나라도 없으면 아무것도 내지 않고 E20/E21(all or nothing; `--resubmit`·`--report`·`--status` 도),
`--only PATTERN[,...]`(같은 출력 base·condor 디렉터리로 표본 일부만 — 큐의 76 은 그대로 두고 ParkingHH 8 만 낸다). 시험
`test_failure_checks.py` 49/49(+13). P8 요약: ParkingHH ALL PASS(6j20 24.7 %, 358 GB); 10-05 의 MC 7/60·Data 5/32 FAIL(출력 없는 job 25·24,
Data 는 모두 JetMET). 상세 [`changes/STEP_24_stage1_2_2024.md`](changes/STEP_24_stage1_2_2024.md) §20, [`reference/ERROR_CODES.md`](reference/ERROR_CODES.md)
E20/E21.

## 2026-10-07: [plotter] STEP 24 I — jet-multiplicity diagnostics from the event tree; 2024 lepton CR 을 지금 (D-2026-10-07-B)

`plotter/make_plots.py --tree-cut`: control 히스토그램 일곱 더(26 → 33) — pT > 40·> 50 GeV jet 수, |η| < 2.0·2.0–2.4 jet 수, nb = 2·nb ≥ 3 의
nJets, 모든 선택 jet 의 b-tag 점수 — 와 `TREEFLAV` 표(점수 0.1 칸마다 MC 의 hadron flavour 분율, Data, Data/MC). 사용자 관찰(2017: lepton CR
에서도 jet 수가 어긋남)의 원인을 b-tag 상관·부드러운/앞쪽 jet·생성기·trigger 로 나누어 보기 위한 것. 시험 `test_failure_checks.py` 36/36(+3).
결정 D-2026-10-07-B: 2024 lepton CR(μ, e)을 FH 재실행과 함께 지금, lep 표본 생산은 KNU 저장소 사용량을 본 뒤; 다음 작업은 Stage 6·7 과 2018
v15 경로. 상세 [`changes/STEP_24_stage1_2_2024.md`](changes/STEP_24_stage1_2_2024.md) §19.

## 2026-10-07: [config][도구] STEP 24 H — 2024 main 에 ParkingHH, filelist 도구가 ParkingHH config 를 읽음; G 의 KNU 결과와 HT1050 control plot

KNU(사용자, 기록 `f51852a2`): `knu_build_g` 0, 단위 시험 PASS, `knu_smoke_2024_g` 81/81(신호 TTHHto4b 는 OR 통과 event 의 83 % 가 b-tag
경로로만 들어온다). `HLT_PFHT1050` && HT > 1200 GeV 의 tree control plot: Data/MC 1.46 — HT·jet pT·event shape 은 평평하고 넘침은 nb ≥ 2 의
b-tag(중간 점수, mistag 영역; 2024 b-tag 보정 없음)에 몰려 있다; QCD 낮은 HT bin 의 큰 weight event 몇 개가 한 bin 짜리 튐. ParkingHH 생산
끝(NtupleForge V60). 결정 D-2026-10-07-A: main 을 G 실행 파일로 전체(MC + Data, 8,062 job) 다시, 첫 look 출력은 `_firstlook` 으로 보존.
`AnalyzerConfig/Tier3_2024_FH_unified_main.yml`: ParkingHH 8 sample. `tools/stage3/make_filelists_v15.py`: `--forge-dir` 가
`config_ttHH<year>_v15_had_ParkingHH.yaml` 도(있으면) 읽고 `FORGECONFIG` 줄. C++ 는 그대로(KNU 다시 빌드 없음). 상세
[`changes/STEP_24_stage1_2_2024.md`](changes/STEP_24_stage1_2_2024.md) §18, 결정 [`DECISIONS.md`](DECISIONS.md) D-2026-10-07-A.

## 2026-10-06: [analyzer][plotter][제출기] STEP 24 G — 2024 의 b-tag 경로는 ParkingHH: PD 규칙, event tree 의 control plot (D-2026-10-06-A)

lxplus 의 HLT 메뉴(run 380115·382913·386604, 사용자): 우리 OR 의 b-tag 경로 넷(4J3T PNet·DeepJet, 6J1T, 6J2T)은 `ParkingHH` 에만,
`HLT_PFHT1050` 은 JetMET0/1 에 → 2024 Data(JetMET 만)에 b-tag 경로로만 들어온 event 가 없어 첫 look 의 HT < 1000 GeV 가 모자랐다.
`ttHHanalyzer_unified.{cc,h}`: 2024 Data 의 PD 규칙 — JetMET0/1 → `HLT_PFHT1050`, ParkingHH → b-tag 경로 && !`HLT_PFHT1050`, Muon0/1·MC →
OR(그대로) — 와 `[trigger]` 줄 둘(규칙, job 끝의 경로별 수; main 에서 taken = cutflow 의 HadTrigger). `plotter/make_plots.py --tree-cut EXPR`:
merge 된 main 출력의 `Tree/Tree` 에서 고른 event 로 control 히스토그램 26 개를 만들어 그린다(MC `evtWeight`, Data 1; `TREEYIELD` 줄) —
`HLT_PFHT1050` && HT > 1200 GeV 의 control plot 을 analyzer 를 다시 돌리지 않고. `plotter/stack_plotter.C`: `Control/` 의 축 제목.
제출기: `ParkingHH_*` 는 Data, 2024 preflight 가 era 별로 JetMET·ParkingHH 짝을 본다(WARN). `include/ExitCodes.h` 주석.
시험: offline smoke 50/50(+7), `test_failure_checks.py` 33/33(+9), `smoke_2024.sh` +12(81). 상세
[`changes/STEP_24_stage1_2_2024.md`](changes/STEP_24_stage1_2_2024.md) §17, 결정 [`DECISIONS.md`](DECISIONS.md) D-2026-10-06-A.

## 2026-10-06: [문서] STEP 24 F 의 KNU 결과, 옛 문서 정리

KNU(사용자): E·F 커밋 `b29856eb` → `knu_build_f` exit 0, 단위 시험 PASS, `knu_smoke_2024_f` 69/69(`dC_mix` 가 E11 로 PASS, E 의
`[inputs]` 줄); 그 전 `knu_smoke_2024_c` 의 `dC_mix.log` 는 `[FATAL][E11]` 뒤 ` *** Break *** segmentation violation`(판단대로).
첫 plot 의 관찰(STEP 24 §16)에 단서 하나: 2017 의 4J3T 는 BTagCSV PD 였다 → 2024 의 b-tag 경로가 JetMET PD 에 있는지 확인할 것.
옛 문서 정리(코드 변화 없음): `README.md` §4 — "코드 안 기본 경로" 문구와 옛 FATAL 번호(40–49)를 지금 정책(E12·E13)과 번호로,
`path_pu_json` 행; §7.0 — 출력 디렉터리의 연도 성분(STEP 24 부터 있음); `docs/reference/CONFIG_PATHS.md` — `path_pu_json`/`TTHH_PU_JSON`
과 2024 yml 의 값; `outputMerger/README.md` — 옛 validation 파이프라인 기록이라는 표시와 지금 쓰는 법. 상세
[`changes/STEP_24_stage1_2_2024.md`](changes/STEP_24_stage1_2_2024.md) §15–§16.

## 2026-10-06: [analyzer] STEP 24 E·F — E(입력 완결성 E30, 커밋되지 않았던 것)와 F(멈춘 job 이 제 exit code 로: `tthh::fatalExit`, main 의 `on_exit`)

커밋 E(C++: 못 여는 입력 파일·파일별 entry 합·MC prescan Runs → E30, `[inputs]` 줄, offline smoke +4 — 아래 D·E 항목)는 D(`ef4c87a0`)와
함께 커밋되지 않고 맥에 남아 있었다 → 이 커밋이 E 와 F 를 함께 담는다. KNU smoke(`knu_build_c` = 커밋 C 의 analyzer + D, 뒤
`knu_smoke_2024_c`) 68/69: `dC_mix`(branch 집합이 다른 era C 두 파일의 job)가 E11 이 아닌 exit 139. `requireSameBranchSet_`(커밋 C)이 다름을 찾아 `[FATAL][E11]` 을 찍고 `std::exit(11)` 을 부르면 exit 때 정리(ROOT 의 end-of-process cleanup, static 소멸자)가
돌고, ROOT 6.30(KNU)에서 그것이 segfault 하면 job 은 139 로 끝난다 — 2026-07-06 Data job 의 era 검사 두 exit 와 같은 일(그때는 그 두 곳만
`std::_Exit`). → `include/ExitCodes.h` 에 `tthh::fatalExit(code)`(출력 flush 뒤 `std::_Exit`), analyzer 의 fatal 경로 모두가 이것으로
끝난다(`std::exit` 42 곳: `ttHHanalyzer_unified.cc`·`.h`, `src/CorrectionsManager.cc`·`ExpandedTtbarId.cc`·`StitchFactors.cc`,
`include/EraConfig.h`·`ConfigPath.h`; 헤더의 `std::_Exit` 2 곳도). 우리 코드 밖의 `exit()`(treestream, `eventBuffer.h` 의 읽기 실패
exit 1, `tnm.cc`)는 `main()` 이 맨 먼저(`(void)gROOT` 뒤)와 event loop 바로 전에 등록하는 `on_exit` handler 가 0 이 아닌 status 만 같은
방식으로 끝낸다 — 그보다 먼저 등록된 exit 때 handler 앞에서만 돌므로 최선의 노력(0 인 정상 끝은 정리 그대로; glibc). 독립 검토로 더한 것:
`src/tnm.cc` 의 `error()` 가 `exit(0)` — filelist 를 못 여는 job 이 성공으로 끝났다 → exit 1; b-tag reweight 의 FATAL 두 줄이 옛 번호
"exit 45/46" 을 찍던 것을 실제 80/81 과 `[FATAL][E80]`·`[E81]` 로, stitch 의 것을 `[FATAL][E63]` 으로(README·`docs/RUNBOOK_2017_SF_rederive.md`
의 45/46 도). 정상 job 의 경로·출력은 같다. 제출기 `--status` 의 139 설명에 "`[FATAL]` 줄 바로 뒤면 이 커밋 전 빌드". 시험: offline
smoke 43(E 의 +4, F 의 +5: `crash_teardown.so`(exit status 가 0 이 아니면 exit 때 SIGSEGV; `LD_PRELOAD`)가 듣는지 대조 하나, 그 아래 E11 둘과
tnm 의 exit 1 이 제 code, 못 여는 filelist → exit 1 — F 없는 빌드는 넷 모두 FAIL(139, 139, 139, 0)), unit·Python 시험 PASS. 상세
[`changes/STEP_24_stage1_2_2024.md`](changes/STEP_24_stage1_2_2024.md) §15(첫 plot 의 관찰은 §16), [`reference/ERROR_CODES.md`](reference/ERROR_CODES.md).

## 2026-10-06: [analyzer·도구] STEP 24 D·E — 실패 확인: 못 여는 입력 파일과 entry 합(E30), merge 의 job 수 대조와 --report/--resubmit, make_plots 의 MC 정확 개수, 제출기 --report 의 fail/wait

사용자(10-06): "job 이 fail 난 것을 어디서 확인 가능하며 모든 작업 플로우에서 확인 후 resubmit 이 가능한가, 코드가 잘
짜여져 있나" → 단계마다 점검. 빈틈: exit 0 인데 입력 파일을 건너뛴 job(TChain 은 못 여는 파일을 오류만 찍고 건너뜀,
prescan Runs 도 경고만), merge 는 job 수를 대조하지 않음, make_plots 의 완결성 검사가 거침, `--report` 의 miss 가 실패와
대기를 구분 못 함. 이번 2024 생산은 사후 확인으로 빠짐 없음(KNU 10-06). 더한 것(Python, 빌드 없음): `plotter/make_plots.py`
의 `EVENTS`(MC: merge 된 noCut = prescan event 수, 아니면 FLAG), `outputMerger/merge_outputs.py --config`(job 번호 대조,
`--report`, `--resubmit`)와 `run_one_hadd.sh` 의 입력 수 대조(exit 7), `consolidate_prescan.py --filelist-dir`, 제출기
`--report` 의 fail/wait 열과 `--status` 의 원인(return value·이름·`.err`), `docs/reference/ERROR_CODES.md` 의 도구 코드.
시험 `test/test_failure_checks.py` 24, `test/test_consolidate_prescan.py` 24. E(C++, 다시 빌드로 들어감): job 의 모든 입력
파일을 시작에 직접 열어 못 여는 파일은 E30(예전 E11), 파일별 `Events` entry 합 ≠ chain 이면 E30, 새 `[inputs]` 줄; MC prescan
의 Runs 를 못 읽으면 E30; offline smoke 38(+4). 정정: treestream `read()` 의 LoadTree < 0 은 `eventBuffer::read` 가 이미 exit 1 로
멈춘다(고치지 않음). 상세 [`changes/STEP_24_stage1_2_2024.md`](changes/STEP_24_stage1_2_2024.md) §14.

## 2026-10-05: [analyzer·plotter·도구] STEP 24 C — 여러 파일 job 의 branch 검사를 파일 직접 비교로, 2024 plot 도구, smoke 의 빌드 순서 검사, consolidate 의 skim 판정

KNU smoke(진짜 파일·payload) 68/69: 남은 `dC_mix` FAIL — `requireSameBranchSet_` 이 첫 파일의 branch 집합을 treestream 의 `present()`(TChain
목록; ROOT 6.30 에서는 첫 파일의 것이 아님)로 봐서, branch 집합이 다른 두 파일 job 이 E11 없이 돌았다 → job 의 모든 파일을 직접 열어 첫
파일과 비교(지금 생산은 Data 1 파일/job, MC dataset 마다 집합 하나라 영향 없음; KNU 빌드는 main 뒤). `tools/stage3/smoke_2024.sh`: 실행 파일이
소스보다 오래되었으면 시작하지 않음(1 차 smoke 가 빌드와 동시에 돌아 exit 127), exit 127 설명. `plotter/make_plots.py`(신규: merge 된 출력의
수율 표, `structure_info.yml`·`samples_config.yml`, compact/detailed plot, 여러 쪽 PDF; 2024 는 TTbb_*·TT4b 를 쌓지 않음 D-2026-10-05-C),
`plotter/stack_plotter.C`(lumi·√s·메모 줄 env, 여러 쪽 PDF, 2024 샘플 이름의 그룹, 입력 파일은 한 번만 열기; env 가 없으면 2017 과 같다),
제출기 `--report`/`--status` 의 샘플별 진행 줄(stderr), skim 된 생산의 Σgenw tree < runs 는 `[skim]` 효율 줄(경고 아님); 수율 표의
skim 전 열(MC generated = lumi×σ×BR, skim 효율, main job 완결 비율). `consolidate_prescan.py --skimmed`: 2024 prescan 의 consolidate 가
"No anomalies" 인데 `EXIT : 1`(MC 60 개 모두 `chk FAIL`) — skim 된 입력의 Σgenw(Events) < Σgenw(Runs) 를 경고 없이 실패로 셌다(숫자는 정상) →
옵션이면 그것은 INFO, Events ≤ Runs 만 검사; 옵션이 없으면 틀릴 때 경고; 실패한 검사는 모두 경고(ttCat weight 분할 포함), anomaly 보고·JSON
meta·exit code 가 같은 판정; skim 없는 2017 은 그대로. 시험 `test/test_consolidate_prescan.py`(신규, 18 check). 상세
[`changes/STEP_24_stage1_2_2024.md`](changes/STEP_24_stage1_2_2024.md) §12–13.

## 2026-10-05: [analyzer·제출기·도구] STEP 24 B — 진짜 eventBuffer.h(KNU 기록)와 2024 analyzer, files_per_job_data, 진짜 파일 smoke

`include/eventBuffer.h`(기록 `run_knu_eventbuffer_manifest_20261005_105905.log` 에서 `eventbuffer_from_record.py`; 764 record, md5 기록과 같음,
stamp), `include/eventBuffer_variables.txt`, fork `8be42e8` 의 `src/treestream.cc`·`include/treestream.h`; 묶음 2 의 `include/EraConfig.h`,
`include/CorrectionsManager.h`, `src/CorrectionsManager.cc`, `ttHHanalyzer_unified.{h,cc}`, `submit_job_FH_Tier3_unified.py`, `test/test_EraConfig.cc`
(§6) 에 로그 줄 셋(`[objectJet]` 의 tagger 이름, `[CorrectionsManager] JEC/JER` payload 이름, `[branches]` 의 any-of yes/no — 물리 없음).
제출기: `common.files_per_job_data`(Data 에만), 2024 Data 에 > 1 이면 제출에서도 그 샘플을 건너뜀, job 수 추정은 샘플마다(2017 출력 같음).
2024 yml: prescan 20, main MC 10·Data 1 files per job. `tools/stage3/smoke_2024.sh`(KNU 의 진짜 2024 파일로, condor job 의 환경, Data 는 golden LS
가 있는 파일, 여러 파일 MC job 둘; 합성 입력 69/69),
`test/offline_smoke/run_offline_smoke.sh` 34 check. 컨테이너: 빌드 OK, unit PASS, `test_stage1` 48/48, smoke 34/34(2017 옛 빌드와 같음).
상세 [`changes/STEP_24_stage1_2_2024.md`](changes/STEP_24_stage1_2_2024.md) §11.

## 2026-10-05: [도구] STEP 24 A2 — 2017 v20 이 KNU 에서 지워져 v9 branch 집합을 지금 header 에서 (D-2026-10-05-D); 2024 PU weight

`tools/stage1/variables_from_header.py`(mkanalyzer 판 header → record), `tools/stage1/variables_v9_2017UL_fullNano_v20.txt`(1782 record,
2026-06-29 header `b08a58b4` 에서), `eventbuffer_manifest.py`(`--extra-variables`, 기본 base 에서 2017 뺌), `make_filelists_v15.py`
(`--forge-dir`: 명령에서 `crab` 을 빼 기록이 커밋되게), 시험 48. `DerivedCorr/PU/2024_Summer24/puWeights_2024.json`(KNU 의 MC 분포
기록 `run_knu_pu_mcprofile_20261005_094457.log` 에서; data 평균 50.0, MC 45.4). 상세 [`changes/STEP_24_stage1_2_2024.md`](changes/STEP_24_stage1_2_2024.md) §10.

## 2026-10-05: [analyzer·제출기·도구] STEP 24 묶음 2 — 2024 지원(코드는 진짜 header 와 함께 커밋), Stage 3 도구·yml·임시 σ, 오프라인 smoke

이번 커밋: `tools/stage3/make_filelists_v15.py`(v15 CRAB 배치 → `filelistTier3_2024/`, 크기 0 제외, task 중복·config 누락 FAIL),
`tools/stage3/data_lumi_check.py`(Data 의 LS 가 golden 에 다 있는지, era I 두 dataset 의 중복), `test/offline_smoke/`(가짜 payload·합성 파일로
analyzer 를 돌리는 시험, 31 check), `data/samples_2024.json`(임시 13.6 TeV σ, 출처·신뢰도 표기), `AnalyzerConfig/Tier3_2024_FH_unified_{prescan,main}.yml`,
`tools/stage1/y1_reference.sh`(2017 tt+nb lookup 디렉터리, `EXPECTED`)·`y1_compare.py`(`EXPECTED` 의 run 이 양쪽에 없으면 FAIL)·`test_stage1.py`(42),
결정 D-2026-10-05-B(2024 MC 4J3T = PNet, PROPOSED)·-C(2024 는 tt+nb lookup 없이). **다음 커밋(진짜 header 와 함께)**: `include/EraConfig.h`,
`include/CorrectionsManager.h`, `src/CorrectionsManager.cc`, `ttHHanalyzer_unified.{h,cc}`, `submit_job_FH_Tier3_unified.py`, `test/test_EraConfig.cc`
— 2017/2018 의 selection·weight 는 그대로(옛/새 빌드 출력 동일), 없는 branch 는 0 대신 E11. 상세
[`changes/STEP_24_stage1_2_2024.md`](changes/STEP_24_stage1_2_2024.md) §3, §6–9.

## 2026-10-05: [도구·입력] STEP 24 묶음 1 — eventBuffer 합집합의 KNU 기록, 2024 PU weight, 2017 회귀 기준(Y1) (analyzer 로직 변경 없음)

`tools/stage1/eventbuffer_manifest.py`(KNU: 우리 생산 다섯의 CRAB task 마다 한 파일, 겹치는 CRAB 파일 이름은 symlink 로 구분, fork 의
`mkvariables.py --merge`, `HLT_*`·`L1_*` 는 `hlt_keep.txt` 27 개만, analyzer 가 읽는 HLT 이름 검사, 잘린 `variables.txt` 를 기록 안에),
`eventbuffer_from_record.py`(맥: 그 기록에서 같은 fork 로 header, md5 확인, stamp, treestream.cc/.h 사본), `tools/stage2/pu_weights.py`
(KNU 의 MC 분포 기록 → correctionlib JSON), `y1_reference.sh`·`y1_compare.py`, 시험 `test_stage1.py` 40(컨테이너 40/40; 독립 검토 9 건 반영). 입력:
`GoldenJson/2024_Summer24/`(2026-08-04 판), `DerivedCorr/PU/2024_Summer24/input/`(DQM 2024CDEFGHI 69200/66000/72400 ub; 4월 판의 LS 집합은
우리 JSON 과 lumi 0.2 % 차이). 결정 D-2026-10-05-A(첫 plot 범위, D14 109.816, D15, blinding DECIDED). 상세
[`changes/STEP_24_stage1_2_2024.md`](changes/STEP_24_stage1_2_2024.md).

## 2026-10-04 (2): [도구] KNU 첫 실행 뒤 — `build_check.sh` 의 판정, `runlog.sh` 의 signal, 시험 보강 (analyzer 로직 변경 없음)

KNU(cms01, `90feebc0`): `test_runlog.sh` 69/2(어느 둘인지 미확인), `test_stage0.py` 19/19, 그리고 10-03 대화형 빌드의 기록
`runlogs/run_knu_build_1003_20261004_164415.log` 이 실행 파일 없이 `EXIT : 0` — 그 빌드는 `lib/` 뒤 링크 전에 끊겼다(condor 로 다시
빌드). `build_check.sh`: 끝난 빌드만 exit 0(실행 파일이 있어야; `--from-log` 는 VERBOSE log 의 `[Done] Built` 줄, 조용한 log 는 실행
파일의 시각으로; make 의 `***` 줄도 오류), `--from-log ""` 가 clean 빌드를 시작하던 것을 막음, 빌드는 `VERBOSE=1`(새 시험
`tools/stage0/test_build_check.sh` 18). `runlog.sh`: TERM/INT/HUP 을 처음부터 잡고, 명령과 그 자식 전부에 TERM, 기록 복사기는 출력을
끝까지 쓰고, 남은 것은 5 초 뒤 SIGKILL, 명령이 끝난 뒤의 signal 은 기록이 끝날 때까지 무시. `test_runlog.sh` 71 → 83(symlink 인
`TMPDIR`, T13 의 경쟁, 진짜 condor 명령이 잡히면 ABORT, 사용자 git 설정 없이 만드는 시험 저장소, signal 시험 일곱). 독립 검토 두 번.
`.gitignore`: KNU 커밋 `39936117` 에 잘못 들어간 빌드 산출물·작업 파일(실행 파일, `lib/`, ROOT dictionary, `bTagSF_ReweightStudy/exe_*`,
`outputMerger/_merge_workdir/`, `prescan_summary_bk/`) — 다음 커밋에서 추적을 끊는다.
상세 [`changes/STEP_23_runlog_condor.md`](changes/STEP_23_runlog_condor.md) "2026-10-04 (2)".

## 2026-10-04: [도구] 실행 기록(runlog)과 KNU condor — 오래 걸리는 단계는 job 으로, 출력은 커밋 (analyzer 로직 변경 없음)

`tools/runlog/`(`runlog.sh`: NtupleForge 와 같은 형식의 실행 기록 `runlogs/run_<step>_<UTC>.log` + `runlogs/LEDGER.tsv`;
`condor_run.sh`: 같은 기록을 KNU condor job 으로 — 이 저장소의 제출 관례 `getenv`·`MY.WantOS`·worker cmsenv; `status.sh`; 시험 71),
`tools/stage0/`(D14 `runs_in_lumiblocks.py`, D16 `branch_signature.py`, `build_check.sh`, `treestream_v15check.sh`; 시험 19),
`test/run_unit_tests.sh`, `runlogs/README.md`, `.gitignore` 세 줄, `README.md` §3·§7.6. 사용자 결정 [`DECISIONS.md`](DECISIONS.md)
D-2026-10-04-A: 우리 프로그램의 실행 기록은 커밋, CRAB transcript 만 제외. 워크스페이스 RUNBOOK §20 9·10 의 heredoc 은 새 PyROOT 에서
깨진 파일 하나에 죽는 것을 고쳐 스크립트로 옮겼다. 독립 검토 한 번(14 건, 모두 반영). 상세 [`changes/STEP_23_runlog_condor.md`](changes/STEP_23_runlog_condor.md).

## 2026-10-03 (2): [기록] treestream 패치 커밋, KNU 의 pull 거절과 clean 빌드 (코드 변경 없음)

사용자가 10-03 점검의 패치를 fork treestream `forTTHH_v1` 에 커밋했다(`8be42e8`; 패치를 적용한 트리와 22 파일 모두 같음) — [`PLAN_v15_2018UL_2024.md`](PLAN_v15_2018UL_2024.md) §9.3·Stage 1 (a)·D16 에 커밋 번호.
KNU 의 이 저장소는 `c1178fe5`(07-29)에 있고, KNU 에서만 고친 `AnalyzerConfig/Tier3_2017_FH_unified_main.yml` 을 `1aa450ef`(2017 lumi 42.07)도
바꾸므로 `git pull --ff-only` 가 거절됐다 → 백업 뒤 그 파일만 stash·pull·pop(워크스페이스 RUNBOOK §20 6). 최상위 `Makefile` 의 목적 파일
규칙은 `.cc` 에만 의존해 header 변경을 모른다: header 를 바꾼 뒤에는 `make clean && make -j4`(PLAN §9.3). [`STATUS.md`](STATUS.md) 10-03 (2).

## 2026-10-03: [기록] Stage 0 정리 — 2024 payload 실측, PHYSICS 유효 lumi, eventBuffer 생성기 점검 (코드 변경 없음)

lxplus 의 jsonpog payload 목록(10-02, 로그는 사용자가 scp)을 [`PLAN_v15_2018UL_2024.md`](PLAN_v15_2018UL_2024.md) §9.2 의 2024 열에: UParTAK4 WP 값,
b-tag SF 는 `preliminary` 의 fixed-WP kinfit(b jet 만)뿐이고 shape SF 가 없음, jet veto map `Summer24Prompt24_RunBCDEFGHI_V1` 의 type `jetvetomap`
(10-02 에 잘린 출력만 보고 "없다"고 했던 것을 전체 로그로 바로잡음), jet ID `AK4PUPPI_Tight`(2022 기준 파일), JEC `Summer24Prompt24_V1`·JER
`Summer23BPixPrompt23_RunD_JRV1`, LUM 의 2024 없음, EGM·MUO 이름. normtag_PHYSICS 유효 lumi(`HLT_IsoMu24` 109.157, `HLT_PFHT1050` 109.144)와 HLT 표가
없는 run 380126–380128 을 [`reference/LUMI_SOURCES.md`](reference/LUMI_SOURCES.md) §6.4 에. eventBuffer 의 생성기는 사용자 fork treestream `forTTHH_v1`
이며, 2024 v15 스키마로 점검한 결과와 패치 제안을 PLAN §9.3 에(자료: 워크스페이스 `treestream_review_2026-10-03/`). §9.6: D10·D11·D14 갱신, D15(2024
PU weight)·D16(2024 eventBuffer 의 입력과 HLT 범위) 새로.

## 2026-10-02 (2): [기록] Stage 0b — 2024 lumi·golden JSON·HLT prescale 실측 (코드 변경 없음)

lxplus955 에서 NtupleForge `script/lumi_hlt_check.sh`(brilcalc 3.9.4, `EXIT : 0`)의 결과를 [`reference/LUMI_SOURCES.md`](reference/LUMI_SOURCES.md) §6 에:
C–I 109.816 fb⁻¹(normtag_PHYSICS; era 값이 PdmV 표와 같음; normtag_BRIL 은 104.504), golden JSON 2026-08-04 판(md5 `3f8543e8…`, run 475, LS 287,601;
생산은 lumimask 없이 했으므로 재생산 불필요), 2024 hadronic·`HLT_IsoMu24` 후보는 모두 prescale 없음(유효 lumi 비 0.9936 으로 같음; 1 이 아닌 것은
HLT 기록이 없는 LS 1,907 개 때문), 4J3T 짝은 초기 `..._TriplePFBTagDeepJet_4p5` 와 그 뒤 `..._PNet3BTag_4p3` 의 OR. [`PLAN_v15_2018UL_2024.md`](PLAN_v15_2018UL_2024.md)
§9.2 의 2024 golden JSON·lumi·trigger 칸, §9.6 D4 갱신, D14(정규화 lumi) 새로. 로그는 사용자 결정으로 public 인 NtupleForge 에 커밋하지 않는다.

## 2026-10-02: [결정·문서] 2024 먼저, 단계별 계획(Stage 0~9); [설정] 2017 lumi 42.07

사용자 결정(10-02)을 [`DECISIONS.md`](DECISIONS.md) D-2026-10-02-A~E 에: v15 의 첫 대상은 2024(A), eventBuffer 는 branch 가 바뀌면 생성기로
다시 만들고 분석이 읽는 branch 의 필수 목록을 입력 파일마다 검사(B, PLAN §7 D7 의 "손으로 넣기"를 대체), 연도별 event cleaning 표(C),
N0~N8 의 답(D), blinding — preselection·CR 에서는 data 를 숨기지 않음(E, PROPOSED). [`PLAN_v15_2018UL_2024.md`](PLAN_v15_2018UL_2024.md)
§9(새): 단계와 통과 기준, 연도별 event cleaning 표(2017·2018·2024), eventBuffer 와 무음 0, 2024 ttbar ID 검증(`ForgeAudit` 코드 분율,
`ForgeTTbbKeys` 완결성, analyzer 경로, 별도 트랙 V6), QCD 비교의 정의, 새 결정 D10~D13(D4 갱신). §5 의 순서는 §9 가 대체한다.
설정(N1): `AnalyzerConfig/Tier3_2017_FH_unified_{main,prescan}.yml` 의 `lumi_fb_inv` 41.48 → 42.07(btagtrig 는 07-29), 표기
`plotter/stack_plotter.C` 42.07, `bTagSF_ReweightStudy/plot_btag{,_pyroot}.py` "42.1 fb^{-1}", `compute_stitch_factors.py` 의 `LUMI_PB_INV`
42070(진단용 `peF_full` 에만; stitch 배수는 그대로). 41.48 로 만든 2017 산출물은 없다(N0). [`reference/LUMI_SOURCES.md`](reference/LUMI_SOURCES.md)
§3 갱신. 다음은 Stage 0(워크스페이스 RUNBOOK §20; 확인용 도구 NtupleForge `script/jsonpog_inventory.py`, `script/lumi_hlt_check.sh`).

## 2026-10-01: [문서] v15 ntuple(2018UL·2024) 계획 PROPOSED (코드 변경 없음)

[`PLAN_v15_2018UL_2024.md`](PLAN_v15_2018UL_2024.md): NtupleForge v15 ntuple 로 전체 사슬을 돌리기 위한 작업 목록(A0~A6, W0, S1p~S5),
순서(2018UL 먼저: 생산 끝, Data KNU 집계 PASS), 연도별 드라이버 `tools/run_year.py` 설계(dataset 단위 완료 판정, 샘플마다 고정된 파일 목록,
단계마다 입력 hash), 사용자 결정 D1~D9. §2 는 이 저장소를 읽어 확인한 막는 것 20 가지(무증상 오답 8: `Jet_jetId`·`Jet_puId` 부재, rho·MET·
electron ID rename, 2017 이름의 trigger bit, `failed/` 를 넣는 파일 목록, 제출 때 상수인 정규화, `TT4b` 를 가정한 stitch, skim 된 2024 의 prescan).
[`STATUS.md`](STATUS.md) 와 [`README.md`](README.md) 에 가리키는 줄.

## 2026-07-29 (2) — [2017 SF 재유도] 다운스트림 두 패키지 정합화 + 무음 오류 6건 차단

목표: **2017 trigger SF / b-tag norm reweight 를 지금 재유도**할 수 있는 상태로 만들기.
운영 절차는 [`RUNBOOK_2017_SF_rederive.md`](RUNBOOK_2017_SF_rederive.md).

고친 것은 전부 "크래시 없이 틀린 숫자가 나오던" 종류다:

| # | 위치 | 증상 (고치기 전) | 조치 |
|---|---|---|---|
| 1 | `src/StitchFactors.cc` | 2017 stitch json 이 구 이름이라 조회 실패 → `multiplier=1.0` → inclusive tt 와 dedicated ttbb/tt4b **이중계수** | `SampleAlias` 로 신·구 모두 시도, stitch 집합인데 못 찾으면 **FATAL** |
| 2 | `src/ExpandedTtbarId.cc` | 같은 이유로 lookup INACTIVE → tt+nb(61/62/71/72) **0건** | 동일 (alias + FATAL) |
| 3 | 두 SF 패키지 `Config.hh` | 구 dataset Σgenw 기준 magic number 표가 **두 벌 복사본**으로 존재 | `include/SampleRegistry.hh` 신설 — xsec_db + prescan_summary 런타임 조회, 제출기와 동일 공식 |
| 4 | `EventLooper.cpp` / `BTagSFProcessor.cpp` | era 를 첫 `_` 로 잘라 `Run2017B` → `era=="B"` 가 항상 거짓 → Run B 를 CDEF bit 로 평가 | 제출기와 같은 `Run\d{4}([A-Z])$` 로 통일 |
| 5 | `BTagSFProcessor.cpp` | group dispatch 를 원본 `genTtbarId` 로 → tt+nb 그룹 전 bin 1.0 (8-key JSON 처럼 보임). `stitchWeight` 미적용 | `expandedTtbarId` 로 교체 + `stitchWeight` 곱함, branch 부재 시 FATAL |
| 6 | `BTagSFProcessor.cpp` | lepton 조건이 **없어서** FH 이벤트와 muon-control 이벤트를 섞어 유도 | `nVetoLeptons==0` (main 과 동일 정의). 그 값을 skim 에 싣도록 analyzer 에 branch 추가 |

부수 변경:
- 하드코딩 절대경로(`/Users/jhlee/...`) → `TTHH_SKIM_DIR` / `TTHH_BTAG_JSON` / `TTHH_TRIGSF_JSON` env override. 재빌드 불필요.
- `TriggerStudy/include/nlohmann/json.hpp` 가 **split-header 3.12.0 의 json.hpp 한 장**뿐이라 include 가 깨져 있었다 (DeriveSF.cpp 는 interpreter 로 이 경로를 직접 include 한다). 나머지 두 곳과 같은 amalgamated 3.11.3 으로 교체.
- `run_analysis.sh` 샘플명을 campaign 이름으로, `run_all.py` 목록을 **자동 유도**로 (전에는 마지막 한 줄 빼고 전부 주석 처리돼 1개 샘플만 돌고 있었다). 제외되는 샘플은 이유와 함께 전부 출력.
- `Tier3_2017_FH_unified_btagtrig.yml`: `lumi 41.48 → 42.07`(xsec_db `_meta` 와 일치, 제출기 lumi 검사 통과), `path_stitch_json` / `path_expanded_ttbarid_dir` 활성화. 2017 lookup 7개를 `DerivedCorr/expandedTtbarId/2017/` 로 복사(byte-identical, 구 이름 유지 — 파일 안에 tree 이름 `TtNb` 가 박혀 있어 개명 불가).
- `test/test_SampleRegistry.cc` (ROOT 불필요, 47 assertion). 특히 **`mustBeInStitchPlan()` 이 ttbar 7종을 넘지 않는지**를 고정한다 — alias 표를 넓히면서 이 분리가 깨지면 비-ttbar 샘플이 전부 FATAL 로 죽는다.

**검증 상태:** `SampleRegistry` / `SampleAlias` / 두 `Config.hh` 는 이 환경에서 컴파일·실행·테스트 통과. ROOT 의존 파일(`EventLooper.cpp`, `BTagSFProcessor.cpp`, analyzer)은 **미빌드** — CMSSW 환경에서 `make` 필요.
**남은 blocker:** repo 의 `prescan_summary/prescan_summary.json` 이 구 이름 24개짜리다. 신규 campaign 61개 MC 중 47개가 없다 (`--preflight` 가 "prescan coverage 54 problem(s)" 로 보고). KNU 에서 `consolidate_prescan.py` 재실행 후 갱신 필요.

## 2026-07-27 (3) — [값 확정] 2018 lumi **59.83 → 59.56** (LUM POG 원문 대조)

사용자가 LUM POG TWiki 원문(PDF 2편)을 제공 → 표를 직접 대조해 값을 확정했다.

**출처** (문서 전반에 링크로 명시):
[TWikiLUM](https://twiki.cern.ch/twiki/bin/viewauth/CMS/TWikiLUM) (index) ·
[LumiRecommendationsRun2](https://twiki.cern.ch/twiki/bin/view/CMS/LumiRecommendationsRun2) (정본 표) ·
인용 **CMS-PAS-LUM-20-001**.

**정본 = "Recorded Golden Legacy"** 행: **2017 = 42.07** (0.82 %), **2018 = 59.56** (0.84 %).
PreLegacy(42.12 / 59.47)는 쓰지 않는다 — 우리 샘플은 UltraLegacy 재처리이므로 Legacy
certification 이 맞는 짝이다.

- **59.83 은 그 페이지에 존재하지 않는 값이었다.** 잠정 placeholder 였고 이제 정정됐다:
  `data/samples_2018UL.json._meta` + 생성기 `NtupleForge/script/build_ul18_from_log.py`
  (재생성 후 **byte-identical** 확인 — 값이 되돌아가지 않는다), RUNBOOK 의 yml 생성 스니펫 3곳
  (`lumi_fb_inv: 59.56`) 과 stitch 스니펫(`LUMI_PB_INV = 59560.0`), beamer backup 표
  (재생성 → 각주 `L=59.56`, **PRELIMINARY 마커 0개**).
- `_meta` 에 2017 과 같은 스키마로 lumi 블록 신설: 불확도 0.84 %, PreLegacy 59.47,
  Golden JSON 파일명(`Legacy_2018/Cert_314472-325175_...`), brilcalc 명령,
  citation, 그리고 **2018-only combine nuisance `lumi_13TeV_15161718_l = 1.0084`**.
  `lumi_brilcalc_result_fb_inv` 는 **null** — TWiki 가 요구하는 "분석 고유 certified JSON 으로
  재실행"을 아직 안 했다(2017 은 42.0688 로 확인됨).
- **2017 은 값이 아니라 코드가 문제였음이 확정됐다**: `_meta` 의 42.07 이 옳고,
  `AnalyzerConfig/Tier3_2017_FH_unified_{main,btagtrig}.yml` 의 **41.48 이 틀렸다**
  (전 MC weight 1.42 % 오차). 고치면 **main·btagtrig 재생산 + b-tag reweight JSON·stitch
  factor 재유도**가 필요하므로 **사용자 승인 대기**로 남겼다 — 이번 변경에 포함하지 않았다.
- `reference/LUMI_SOURCES.md` 전면 개편: BLUF 에 정본 값, §1 references, §2 LUM POG 표 발췌 +
  combine nuisance, §3 을 **9개 지점 × 반영 여부(✅/❌)** 표로. OPEN #6 도 "값 확정 / 코드 일부
  미반영"으로 재작성.

## 2026-07-27 (4) — [문서] 진행 중 CRAB 부분 산출물용 로컬 검증 워크북 신설

`WORKBOOK_local_test_partial_CRAB.md` (워크스페이스 루트). 두 캠페인이 **끝나기 전에** 이미
stage-out 된 파일만으로 할 수 있는 검증을 순서대로 정리했다.

- **핵심 근거**: `matchTtbarId` 는 `unmatched > 0` 이어도 **exit 0** 을 낸다
  (`tools/matchTtbarId.cc:476-481`). 즉 부분 산출물로 **정확성**(byte-identity + 확장 불변식)은
  지금 완전히 검증 가능하고, 검증 못 하는 것은 **완결성**(nevents 대조)뿐이다.
- 테스트 A(파일별 불변식 매크로) · B(filelist + **중복 제출 가드의 첫 실제 EOS 실행**) ·
  C(extend↔중앙 NanoAOD 매칭, 가장 작은 `ttbb_2L2Nu` = nano 6파일/4.79 M 부터) ·
  D(`forgedNtuple_*.root` sanity) · E(patch 추출 구조만) + 판정 요약표 + 로그 6종.
- **경고 명시**: 부분 patch 를 `DerivedCorr/expandedTtbarId/2018` 에 복사하면 빠진 event 가
  조용히 "확장 안 됨"으로 처리되어 **틀린 물리 결과**가 나온다. `/tmp` + `PARTIAL_DO_NOT_USE_`
  접두사로 강제.
- 하위 에이전트 검증에서 잡아 고친 것: `$?` 가 **tee 의 상태**라 모든 실패가 0 으로 보이던 문제
  (→ `${PIPESTATUS[0]}`, 6곳), `ls bin/` 기대 목록에 없는 `compareExtendToCentral`,
  `genTtbarId` 부재를 전 MC 에 FAIL 로 처리해 QCD/WJets/ST/TTHH 가 전부 가짜 FAIL 나던 게이트
  (→ ttbar-parented 샘플만 FAIL, 나머지는 info), ntuple 쪽 `timestamps` 열에 extend 전용
  "1이 아니면 중단" 규칙을 적용하면 안 된다는 주의(JetHT 4 task / `_ext1` 2 task 는 정상).

## 2026-07-27 (2) — [문서·정리] 재현성 감사: 깨진 명령 수정, 유령 문서 제거, .gitignore 보강

코드 로직 변경 **없음**. 하위 에이전트로 전 저장소 문서를 실제 CLI 와 대조한 결과의 반영이다.

**깨져 있던 명령 (복붙하면 실패했다) — 이 저장소 몫:**

- `README.md` §7.0 은 `AnalyzerConfig/Tier3_2018_FH_unified_prescan.yml` 을 이미 있는 것처럼
  가리켰다. `AnalyzerConfig/` 에는 `Tier3_2017_*` 3개뿐 → **파생 스니펫이 있는 RUNBOOK §5 를
  가리키도록 수정**.
- `README.md` "Validation plotter — `scripts/plot_ttcat_validation.py`" 절 전체가 **존재하지 않는
  스크립트**를 4가지 호출 예시까지 곁들여 기존 도구처럼 설명하고 있었다(`tempTTHH/scripts/`
  디렉토리 자체가 없다). **PROPOSED 사양으로 강등** + 당장 쓸 수 있는 `root -l` 대안 병기.
- `README.md` 의 `README_ntuplizer.md` 참조 → 실제 경로 `../NtupleForge/README.md`.
- RUNBOOK 쪽(워크스페이스): `compute_stitch_factors.py` 는 **argparse 가 아예 없는데**
  `--xsec-db/--prescan/--out` 을 주는 명령이 실려 있었다 — 플래그가 조용히 무시되고 2017 상수로
  돌아 **`stitch_factors_2017.json` 을 덮어쓴다**. USER CONFIG 를 고치는 스니펫으로 교체.
  `consolidate_prescan.py` 도 출력이 항상 `<outdir>/prescan_summary.json` 이고 `--input-base`
  기본값이 2017 이라 인자 없이 부르면 **2017 요약을 덮어쓴다** → `--input-base`·`--outdir` 필수
  명시로 교체. `merge_outputs.py`·`scenario_runner.py` 의 `required=True` 인 `--base`
  (그리고 `--plotter`) 누락도 실제 경로로 채웠다 — `--trigsf off` 면 output 디렉토리가
  `AnalyzerOutput_main_notrig` 임을 명시.

**숫자 정정**: 연도 의존 감사 항목 수를 **13 → 17**(P0 7 + P0′ 5 + P1 5)로 통일
(`CHANGELOG.md`·`STATUS.md`·RUNBOOK 헤더가 13 이라 하고 §4 표는 17개를 나열하고 있었다).

**연도 혼입 경고를 문서에 명시**: `submit_job_FH_Tier3_unified.py:158-160` 의
`path_output_base` 에 연도 성분이 없어 2018 산출물이 2017 과 같은 `AnalyzerOutput_*` 에 섞인다
(P0′ #9). README §7.0 과 RUNBOOK merge 단계 양쪽에 경고 박스 추가.

**`.gitignore` 보강**: `__pycache__/`·`*.py[cod]`(이 저장소에만 없어서 `tempTTHH/__pycache__`,
`python/ttHHmodules/__pycache__` 가 매번 untracked 로 떴다), `*.out`/`*.err`/`manager.log`/
`logs/`, `prescan_summary_20*/`. 블랭킷 `*.root`/`*.pdf` 규칙이 산출물까지 막는다는 주의도 명기.
정리: `__pycache__` 2곳, `.DS_Store` 삭제.
**미처리(사용자 판단 필요)**: `bTagSF_ReweightStudy/logs/` 에 condor `.out`/`.err` **64개(412 KB)
가 이미 커밋되어 있다** — 새 규칙은 추적 중인 파일을 untrack 하지 않으므로
`git rm -r --cached bTagSF_ReweightStudy/logs` 가 별도로 필요하다.

## 2026-07-27 — [문서] 2018UL 연도 의존 지점 전수 감사 (17항목, 코드 무변경)

- 목표 변경(사용자): **UL18 control plot 최단 경로**. 실행 문서 = 워크스페이스
  `RUNBOOK_UL18_to_controlplots.md` (구 `RUNBOOK_UL18_step1.md` 는 SUPERSEDED).
- analyzer/보정/스터디/플로터 전체를 grep 하여 연도 의존 지점 **17개**를 file:line 으로 확정하고
  P0(7)/P0'(5)/P1(5) 로 분류 → STATUS OPEN #5 에 표로 등재. **코드는 아직 바꾸지 않았다.**
- 특히 **조용히 틀리는 3건**을 확인: ① `L1PreFiringWeight_Nom` 은 2018 NanoAODv9 에 없고
  eventBuffer 가 스칼라를 0 으로 초기화하므로 `_evtWeight *= 0` → **전 MC weight 0, 경고 없음**
  ② 2017 전용 `JetHT && !BTagCSV` orthogonality veto 가 2018(BTagCSV PD 부재)에서 4J3T 발화
  event 를 폐기 ③ DeepJet WP 가 2017UL 상수라 2018 신호영역 정의가 틀어짐.
- 또한 **2018 Data 는 현재 filelist 조차 만들 수 없음**을 확인(`make_filelists.py` Data 분기가
  `Run2017` 하드코딩) → control plot 에 Data 를 넣으려면 이 일반화가 선행 필수.
- 그리고 **output 디렉토리에 연도 성분이 없어**(`submit_job_FH_Tier3_unified.py:158-162`)
  2018 산출물이 2017 과 같은 pnfs 디렉토리에 섞인다 — 2018 제출 전 반드시 수정.

## 2026-07-26 — [문서] reference/LUMI_SOURCES.md 신설 (lumi 산재 지점 전수 목록, 값 무변경)

- lumi 불일치를 추적하다 **lumi 가 3개 저장소 7개 지점**에 산재해 있음을 확인 → 신규
  [`reference/LUMI_SOURCES.md`](reference/LUMI_SOURCES.md) 에 전수 목록·성격(정본/fallback/
  하드코딩/자동생성)·영향·2018 에 아직 없는 것·변경 절차 체크리스트·재생산 판단을 정리했다.
- 드러난 모순 3건(값은 **바꾸지 않았다**): ① weight 사용값 41.48(yml) vs 표기값 42.07(xsec_db
  `_meta`, brilcalc 42.0688) → 1.42% ② 같은 발표자료 PDF 안에 41.48(본문)과 42.07(backup 표)
  동시 인쇄 ③ b-tag study plot 에만 세 번째 값 41.5.
- 사용자 결정: **brilcalc 재실행 후 2017·2018 을 한 번에 통일** (STATUS OPEN #6).

## 2026-07-26 — submitter: --preflight / --filelist-dir 추가, Data era 정규식 일반화 (**동작 변경**)

- **`--preflight` (신규, 읽기 전용).** 제출·디렉토리 생성·proxy 확인을 일절 하지 않고
  yml 스키마 / 실행파일 / 보정 경로 null 정책 / xsec_db·prescan_summary 커버리지 /
  filelist 존재·개수 / Data era 추출 / output·condor 디렉토리 쓰기권한 / `proxy.cert` 를
  점검하고 `preflight_<mode><suffix>_<timestamp>.log` 를 남긴다. FAIL 이 하나라도 있으면
  exit 1. 점검은 **제출 때와 동일한 파서**(`load_yaml_config`)와 동일한 prescan 스키마
  (`samples` 하위)를 쓴다.
- **`--filelist-dir` (신규).** master filelist 디렉토리를 인자로 받는다(기본 `filelistTier3`).
  연도별 분리(`make_filelists.py 2018` → `filelistTier3_2018/`)를 쓰기 위한 것.
- **[동작 변경] Data era 추출 정규식 `Run2017([A-Z])$` → `Run\d{4}([A-Z])$`.**
  기존 정규식은 2018 키(`JetHT_Run2018A`)를 잡지 못했고, fallback `_([A-Z])$` 도 'A' 앞이
  '8' 이라 실패해 **FATAL** 로 끝났다. 이제 2016/2017/2018/Run3 키 모두 era 를 추출한다.
  2017 키(`JetHT_Run2017B`)의 결과는 이전과 동일하다(회귀 없음).
- `make_filelists.py` era 인자화: `python3 make_filelists.py [2017|2018] [SAMPLE_DIR]`.
  기존에는 SAMPLE_DIR/OUTPUT_DIR 이 2017 고정이라 다른 연도로 쓰면 커밋된
  `filelistTier3/` 를 덮어썼다. 인자 없으면 2017 = 기존 동작 그대로.

## 2026-07-26 — make_filelists.py: ntuple 파일명 forgedNtuple 대응 (이중 매칭)

- NtupleForge 가 산출 ntuple 이름을 `slimmedNtuple.root` → **`forgedNtuple.root`** 로 rename(D-F, 2026-07-26). 이에 맞춰 `make_filelists.py` 에 `NTUPLE_PREFIXES = ("forgedNtuple", "slimmedNtuple")` 를 도입하고 `find_root_files()` 가 **두 prefix 를 모두** 수집하게 했다.
- **왜 이중 매칭인가**: 현재 Tier-3 에 있는 `ttHH2017UL_fullNano_v20` 생산물은 실제로 `slimmedNtuple_*.root` 다. forged 전용으로 바꾸면 기존 2017 filelist 재생성이 즉시 깨진다. 전 캠페인 재생산 완료 후에만 legacy prefix 를 제거할 것.
- analyzer 의 `TFile::Open` 은 filelist 의 경로를 그대로 열기 때문에 파일명 하드코딩이 없다 — 이 스크립트가 유일한 결합 지점이다.

## 2026-07-26 — [문서] xsec_db 참고문헌 오류 2건 발견 (값 무변경, OPEN)

ttH/ttHH AN 과 샘플 교차검증 중 `data/samples_2017UL.json`(→ 2018UL 로 그대로 복사됨)에서 발견. **값은 바꾸지 않았다** — 출처 확정은 사용자 판단 사항:

- `TTTT`/`TTWW`/`TTWH`/`TTWZ`/`TTTW` 의 `xsec_ref` 가 `"ttHH AN Tab.9"` 인데, AN-2022/122 Table 9 는 ttHH·ttH·tt+jets·tt+bb·tt+4b·ttZ·ttZZ·ttZH **8행뿐**이며 AN 본문에 ttWW/tttt/multi-top 언급이 없다 → 참고문헌이 틀렸거나 값의 실제 출처가 다름(XSDB 추정).
- `TTZToBB` = 861 fb (ref: AN Tab.16) 이지만 **AN Tab.9 의 ttZ = 841 fb** — 어느 정의/표가 맞는지 확인 필요.
- STATUS OPEN #2("verify provisional cross sections")의 구체 항목으로 편입.

## 2026-07-26 — [문서] 2018 trigger PD 로직 변경 필요성 확인 (코드 무변경, FUTURE)

- DAS 광역 스캔 결과 **BTagCSV PD 는 2018 에 존재하지 않음이 확정**(전 tier·전 status 0건;
  2018 PD 통합에서 BTagCSV/HTMHT/FSQJet/HighPt* 삭제, EGamma 신설. `/BTag*/Run2018*/` 는
  BTagMu = BTV muon-tag 캘리브레이션 PD 만 반환).
- **영향**: 현재 `ttHHanalyzer_unified.cc` ~L295–360 의 orthogonality 로직
  (4J3T→BTagCSV / 6J·HT→JetHT + veto)은 2017 전용이다. 2018 은 해당 path 들이 모두
  JetHT 에 있으므로 **JetHT 단독 OR** 로 축약해야 하며, 2018 path 이름도 다르다
  (`…TriplePFBTagDeepCSV_4p5` 등). 또한 미인식 Data PD 는 E`CONFIG_BAD_RUNINFO` 로
  즉시 종료하므로, 2018 config 투입 전에 이 로직을 연도/era map 으로 확장해야 한다
  (코드에 이미 TODO 존재: L297).
- 지금은 **문서화만** 했다 — 코드 변경 없음. 실행 계획은 workspace
  `00_CONTEXT_ExpandedTtbarId_NtupleForge_Migration.md` Track D5.

## 2026-07-26 — data/samples_2018UL.json 추가 (2018UL 확장 준비, 코드 무변경)

- `data/samples_2018UL.json` 신규 (85 샘플 = 77 MC + 8 Data): das_path/nevents/nfiles 는 DAS 스캔
  (`NtupleForge/script/das_ul18_scan_20260726_1657.log`), xsec/BR/kfactor/refs 는 `samples_2017UL.json`
  에서 복사 (13 TeV 동일값). 생성기: `NtupleForge/script/build_ul18_from_log.py`.
- OPEN 항목이 파일 `_meta`/`note` 에 명시됨: lumi 59.83 /fb 는 잠정(사용자 확정 예정),
  `frac_neg_weight=null`(UL18 prescan 후 재산출), Data 는 non-GT36 선택(XPOG 권장 확인 필요),
  **BTagCSV PD 는 2018 에 부재**(2018 PD 개편) — FH data 는 JetHT 로 커버.
- analyzer 코드는 변경 없음 (2018 캠페인 실행은 별도 STEP 으로).

## 2026-07-12 — STEP 22.1: merge env 정책 — bootstrap 제거, 명시적 cmsenv

- 실전 첫 실행에서 76/76 전멸 — cmsenv 없는 셸의 local 병렬 실행이 `run_one_hadd.sh` 의 `/tmp/CMSSW_14_2_1` 자동 bootstrap 을 8-way 로 race 시킨 것이 원인. **정책 결정: 스크립트는 환경을 만들지 않는다** — bootstrap 전체 삭제. local 은 사용자가 cmsenv/ROOT 준비(없으면 preflight 에서 1회 명확히 실패), condor 는 `--cmssw-src`(기본: 제출 셸 `$CMSSW_BASE/src` 자동)로 worker 가 cmsenv. runner 는 optional 3번째 인자 `[cmssw_src]` 수용, hadd 미존재 시 exit 3 + 안내.
- Detail: [`changes/STEP_22_merge_outputs_autodiscovery.md`](changes/STEP_22_merge_outputs_autodiscovery.md) 후속 섹션.

## 2026-07-12 — STEP 22: merge_outputs.py — AnalyzerOutput 자동 발견 hadd (local 병렬/condor)

- 신규 `outputMerger/merge_outputs.py`: `<base>/<proc>/<proc>_*.root` → `<base>/<proc>.root` merge 를 **디렉토리 자동 발견**으로 수행 — 구 merge_submitter.py 의 하드코딩 목록(stale 이름으로 다수 샘플이 조용히 누락될 상태) 제거. `--mode local --jobs N`(병렬 + 완료 대기 + OK/FAIL 요약 + exit 전파) / `--mode condor`(proc 당 1 job, submit_hadd_validation 템플릿), `--only/--exclude/--skip-existing/--list/--dry-run`. 실행 단위는 검증된 `run_one_hadd.sh` 재사용.
- 검증: 74개 실디렉토리명 가짜 트리 + stub runner 로 발견/필터/병렬/실패복구/condor 생성 end-to-end 테스트 전부 통과. 실제 hadd 는 Tier3 에서 `--list` → `--dry-run` 순 확인 권장.
- Detail: [`changes/STEP_22_merge_outputs_autodiscovery.md`](changes/STEP_22_merge_outputs_autodiscovery.md).

## 2026-07-10 — STEP 21: plotter 전체 샘플 + compact/detailed 2-모드 그룹핑 + cutflow legend 수율 + PDF

- `plotter/samples_config.yml`: MC 24 → **61** (ttHToNonbb, tH, ttW/ttZ 잔여, single-top, VV, V+jets, DY 등 37개 추가; 미존재 파일은 자동 skip).
- `plotter/stack_plotter.C`: 그룹핑을 substring → **exact-name 테이블**로 교체 — 구 라우팅의 오배정(TTWW/TTWZ/TTWH/TTZH*/TTZZ*/TTTW 가 전부 "tt+V" 흡수) 수정. env `TTHH_PLOT_GROUPING` 으로 **compact(MC 10줄, 기본)/detailed(MC 13줄)** 2-모드; TTZHTo4b/TTZZTo4b 는 두 모드 모두 별도 줄(`tt+ZH/ZZ(4b)`). cutflow legend 수율을 Integral(≈noCut 지배) → **최종 cut bin(nTotal)** 으로 (`LegendYield`), legend 에 "yields after final cut" 헤더. 출력 PNG → **PDF**, 모드별 `plots_<grouping>/` 분리.
- `plotter/scenario_runner.py`: `--grouping compact|detailed|both` 추가(env 전달, 모드별 로그/PDF 카운트).
- 검증: 그룹핑 함수 g++ 실컴파일 전수 테스트(61 샘플 × 2 모드 + 회귀 방어) ALL PASSED. ROOT 렌더링은 로컬 1회 확인 권장.
- Detail: [`changes/STEP_21_plotter_full_samples_two_mode_grouping.md`](changes/STEP_21_plotter_full_samples_two_mode_grouping.md).

## 2026-07-10 — STEP 20: --report 요약 테이블 + --status 상세 + 조회 read-only + lazy tmp

- `submit_job_FH_Tier3_unified.py`: `--report` 는 이제 CRAB 스타일 요약 테이블(`sample|jobs|done|miss|done%` + TOTAL)만 출력하고, 구 상세 출력(weight + missing idx 목록)은 신규 `--status` 로 이동. 조회 모드를 완전 read-only 화 — report 가 `arguments_<sample>.txt` 를 빈 파일로 truncate 하던 부작용 수정, output dir mkdir/chmod skip. tmp 디렉토리는 lazy 생성으로 전환: 첫 per-job filelist 를 쓸 때만 생성 → 모든 호출이 샘플마다 빈 `tmp_<sample>_<ts>/` 를 만들던 리터 제거 (빈 tmp 존재 = 그 invocation 에서 재큐 0건이던 원인 규명 포함). 판정 기준은 STEP 19 종료 마커 그대로.
- Detail: [`changes/STEP_20_report_table_status_readonly.md`](changes/STEP_20_report_table_status_readonly.md).

## 2026-07-10 — STEP 19: report/resubmit 완료판정 = 종료 마커 + non-tt ttCat 요약 게이트

- `submit_job_FH_Tier3_unified.py` `_output_is_complete` (non-prescan): "non-empty TTree" 기준 → **`cutflow_w_full` 종료 마커** 기준으로 교체. selection 을 아무 이벤트도 통과 못한 정상 job(저-HT WJets/QCD 에서 구조적 발생)의 거짓 missing(→ 영구 재제출 루프)과, STEP_15 §6 의 고위험 half-written 거짓 complete 를 동시에 해소. 기존 output 에 소급 적용 — 재실행 없이 `--report` 재실행으로 충분. prescan 분기 불변.
- `ttHHanalyzer_unified.cc` `printTtCatSummary`: pair 비교(ANA_GENPART vs ANA_GENID)를 `IsTtbarFamily` 로 게이트. non-tt 샘플은 genTtbarId 디코드가 noTT 를 표현할 수 없어(NanoAOD 가 모든 MC 에 gtid≥0 기록) agreement 0% 가 정의상 결과 — "~97% 기대" 라벨 대신 SKIPPED 사유를 출력. 진단 stdout 만 변경, 물리/branch 무영향. **analyzer 재빌드 필요.**
- Detail: [`changes/STEP_19_completion_marker_and_ttcat_note.md`](changes/STEP_19_completion_marker_and_ttcat_note.md).

## 2026-06-30 — fix: make_filelists TTbar/ttbb disk-directory keys

- `make_filelists.py` `SAMPLE_MAP`: corrected 6 keys (on-disk dataset dir names) that STEP 18 had wrongly renamed to the short_name form (`TTbar_Hadronic_…`, `TTbb_4f_TTbar_…`). Real dirs are `TTToHadronic_…` / `TTToSemiLeptonic_…` / `TTTo2L2Nu_…` and `TTbb_4f_TTTo*_…`; this fixes the 6 `[MISSING] Directory not found` errors. short_names (values, = project keys) unchanged, so analyzer/xsec_db/yml/group-map are unaffected. Restores the module's own documented invariant ("on-disk primary dataset 이름은 불변").

## 2026-06-30 — STEP 18 (part C): exit-code system + correction-path policy + docs

- Added `include/ExitCodes.h` (canonical, collision-free exit codes) and `include/ConfigPath.h` (path resolver with no code default). Remapped all scattered exit codes in `ttHHanalyzer_unified.cc`, `src/CorrectionsManager.cc`, `src/ExpandedTtbarId.cc`, `src/StitchFactors.cc` to the new scheme; fixed the `41`/`43` collisions. → see [`DECISIONS.md`](DECISIONS.md) D-2026-06-30-C.
- Replaced the submitter's blank-means-default path export with the explicit-policy loop (E12/E13, `__NULL__` sentinel, always-export); numbered the xsec/prescan FATALs (E20/E21); taught the yml loader `null`→None. → D-2026-06-30-D.
- Updated `AnalyzerConfig/Tier3_2017_FH_unified_{prescan,main,btagtrig}.yml` `common.path_*` (jsonpog/golden explicit; the four derived corrections `null`=disabled). → D-2026-06-30-E.
- Created the guideline-conformant doc set: [`README.md`](README.md), [`STATUS.md`](STATUS.md), [`DECISIONS.md`](DECISIONS.md), this file, [`reference/ERROR_CODES.md`](reference/ERROR_CODES.md), [`reference/CONFIG_PATHS.md`](reference/CONFIG_PATHS.md); copied the documentation guideline into `docs/`.
- Detail: [`changes/STEP_18_ntupleforge_naming_and_kfactor_cleanup.md`](changes/STEP_18_ntupleforge_naming_and_kfactor_cleanup.md) (part C section).

## 2026-06-30 — STEP 18 (parts A–B): NtupleForge naming + k-factor cleanup

- Adopted NtupleForge campaign names as the project-wide sample key (analyzer/config/plotter/b-tag-SF). k-factors folded into notes (`kfactor=1.0`). → D-2026-06-30-A, D-2026-06-30-B.
- Detail: [`changes/STEP_18_ntupleforge_naming_and_kfactor_cleanup.md`](changes/STEP_18_ntupleforge_naming_and_kfactor_cleanup.md).

## 2026-06-29 — STEP 17 — full-NanoAOD dataset + categorization

- Migrated the analyzer to the `ttHH2017UL_fullNano_v20` campaign and to standard `genTtbarId` decode. Detail: [`changes/STEP_17_fullNano_dataset_and_categorization.md`](changes/STEP_17_fullNano_dataset_and_categorization.md).

## Earlier — STEP 0–16

See the per-step records in [`changes/`](changes/) (indexed by [`changes/README.md`](changes/README.md)). Notable: STEP 11 (xsec_db single source), STEP 12 (fb units), STEP 14 (3-tier weight + MET-CR + norm check), STEP 16 (SF toggles + output-dir split), STEP 4 (paths/yml FATAL — superseded in part by STEP 18 part C path policy).

## 2026-07-06 — Data era-check fix (STEP_18 follow-up #2)
- **Bug**: all Data jobs in `main` mode died with `[ERROR] eraName mismatch ... eraName: D, sampleName: BTagCSV_Run2017D` followed by a segfault. Root cause: `ttHHanalyzer_unified.h::extractEraFromSampleName()` still assumed the *legacy* Data naming (`JetHT_B`, single-letter last token). STEP_18 renamed Data samples to `<PD>_Run2017<E>`, so extraction returned `""` → mismatch → FATAL. MC does not take this branch, hence "MC runs fine, Data dies".
- **Fix 1**: `extractEraFromSampleName()` now recognizes `Run20YY<E>` (era-agnostic: 2016/2017/2018; tolerant of `_ext1` suffixes) with the legacy single-letter form kept as fallback. Unit-tested against 13 old/new/edge sample names.
- **Fix 2**: the two runinfo exits (`MC must not define era`, `era mismatch`) now emit `[FATAL][E11]` (tthh::CONFIG_BAD_RUNINFO) and use `std::_Exit` — `std::exit` triggered a ROOT-teardown segfault that polluted the exit status (139 instead of a canonical code). `ExitCodes.h` is now included by the header directly.
- Files: `ttHHanalyzer_unified.h` only. Rebuild required (`make`).

---

## 2026-07-28 — OPEN item registered: `ExpandedTtbarId` should accept both patch tree names

Registered from `TTHHGenCategoryTools` (its `docs/07_analyzer_integration.md` §4 and
`docs/01_status.md` O2). **No code change here yet — nothing is broken.**

Context: the producer side renamed its patch convention to `ttbarIdPatch_<KEY>.root` / tree
`TtbarIdPatch` (TTHHGenCategoryTools D12), but this analyzer's loader hard-defaults to the legacy
`ttnb_<sampleKey>.root` / `TtNb`. On 2026-07-28 it was **decided to keep the legacy convention for
now**: the 2018 patches are being extracted with `--out ttnb_<KEY>.root --out-tree TtNb` on
purpose, so 2017 and 2018 share one convention and this analyzer needs no change to run the UL18
control plots.

The cleanup, when it happens: make `loadFromDir()` try `TtbarIdPatch` first and fall back to
`TtNb`. **Do not just flip the default** — the 2017 patches carry the tree name `TtNb` *inside the
file*, so a flip makes them unreadable and forces re-extraction from KNU Tier3.

Why this is worth writing down rather than remembering: getting it wrong fails **silently**. The
loader goes INACTIVE (by design, not fatal, because patch-less samples and Data must fall through
to the raw NanoAOD value), so the analyzer would quietly produce plots with the tt+nb extension
not applied. Any change here must be verified by asserting `active()` is true and `size()` equals
the patch row count.

See `STATUS.md` "Pending / next steps (OPEN)" item 2.


---

## 2026-07-29 — 2018UL 대응 P0 7건 완료: 연도 의존 상수를 `EraConfig.h` 로 일원화

`STATUS.md` OPEN #6 (2018UL 대응) 의 **P0 7건**을 구현했다. 목표는 "코드가 2017 과 2018
양쪽에서 동작한다" 이지만, 실제 위험은 그게 아니었다. **7건 전부가 "2018 을 주면 크래시
없이 조용히 틀린 결과가 나온다"** 였다. 그래서 각 수정마다 분기뿐 아니라 **틀린 상태를
FATAL 로 만드는 장치**를 같이 넣었다.

### 새 파일 — `include/EraConfig.h` (연도 상수 단일 소스)

연도 의존 값이 5개 파일에 흩어져 있던 걸 한 헤더로 모았다. 이 파일 밖에서
`if (year == "2017")` 를 쓰지 않는다는 것이 규약이다. 제공 항목:

| 함수 | 대체한 하드코딩 | 2018 값 근거 |
|---|---|---|
| `yearForCorr()` | `ttHHanalyzer_unified.h:1133` | jsonpog 디렉토리명 |
| `btagWP()` | `ttHHanalyzer_unified.h:243-245` | **AN Table 32** (p.41) |
| `usesL1Prefiring()` | `ttHHanalyzer_unified.cc:1198` | **AN p.118** |
| `goldenJsonFile()` | `src/CorrectionsManager.cc:248-250` | `GoldenJson/` 실제 파일 |
| `jecDataEraTag()` | `src/CorrectionsManager.cc:167-168` | Summer19UL18 Run A–D |
| `hemJes()` / `inHemRegion()` | (신규) | **AN p.118** HEM JES 변주 |
| `normalizeYear()` | (신규) | 표기 정규화 |

**조회 함수는 모르는 연도에 대해 예외 없이 `exit(11)` 이다.** 기본값으로 넘어가지 않는다 —
그게 이 버그들의 공통 원인이었다.

### P0 항목별

1. **#1 연도 게이트** — `if(_runYear=="2017") yearForCorr="2017_UL";` 에 `else` 가 없어서
   2018 은 `yearForCorr` 가 **빈 문자열**로 남았다. → `EraConfig::yearForCorr()`.
2. **#7 DeepJet WP** — 2017 값이 `static constexpr` 로 박혀 있었다. 2018 을 돌려도
   경고 없이 2017 WP(M=0.3040)로 b-jet 을 세어 **신호영역 정의가 달라진다**.
   → 연도별 주입(`objectJet::configureBtagWP()`), 2018 = 0.0490/0.2783/0.7100.
   미주입 상태로 jet 을 분류하면 `assertBtagWPConfigured()` 가 FATAL. static 초기값은
   **의도적으로 NaN** 이다 (0 이면 모든 jet 이 b-tag 인데도 멀쩡히 돈다).
3. **#4 L1 prefiring** — 2018 NanoAODv9 에는 `L1PreFiringWeight_Nom` branch 가 없고
   eventBuffer 는 없는 branch 를 0 으로 둔다. 게이트 없이 곱하면 **모든 MC weight 가 0**
   → 히스토그램 전부 빔. 크래시도 경고도 없음. 이번 P0 중 가장 위험했다.
   → `EraConfig::usesL1Prefiring()` 로 2018 비활성 + 적용 연도에서 값이 0 이면 FATAL.
4. **#2 eventBuffer 2018 HLT branch 3개 추가** — 5개 지점(선언/`choose`/`select`/
   `output->add`/`initBuffers`) 모두. 기존 branch 를 템플릿으로 스크립트 생성해 누락 방지.
5. **#3 trigger path + PD 라우팅** — 2017 주력 경로(CSV 계열)는 **2018 메뉴에 없다**.
   2018 은 DeepCSV 계열로 교체. 또한 2018 은 **JetHT 단독 PD** (BTagCSV PD 없음) 이므로
   orthogonality veto (`&& !group_4J3T`) 를 **제거**했다 — 남겨두면 4J3T 가 터진 이벤트를
   아무 PD 도 안 가져가 통째로 사라진다. 2017 로직은 그대로 보존.
   → `requireTriggerBranches2018_()` 가 첫 이벤트에서 4개 경로의 **실제 존재**를
     (`successBranches`) 확인하고 없으면 FATAL. 없는 HLT branch 는 0 = "안 터졌다" 와
     구분이 안 되므로, 검사하지 않으면 조용히 0 event 가 된다.
6. **#5 golden JSON 파일명** — run range `Cert_294927-306462_` 가 문자열에 하드코딩되어
   2017 외 연도는 존재하지 않는 파일명을 만들었다. 게다가 unknown year 일 때 `return` 만
   해서 **Data 가 golden 필터 없이 돌 수 있었다**. → `EraConfig::goldenJsonFile()`.
7. **#6 Data JEC era 태그** — 2018 이 `(era=="A") ? "A" : "D"` 로 축약되어 **B, C 에
   RunD JEC** 를 적용했다. 게다가 키가 없으면 `catch` 가 **MC JEC 로 조용히 fallback** 하고
   WARNING 한 줄만 찍었다(Data 에 MC JEC = jet energy scale 전면 오류, 다른 증상 없음).
   → era 문자 그대로 사용 + Data 는 fallback 폐지, FATAL.

### 곁가지로 발견해 고친 것

- **`2016PreVFP_UL` vs `2016preVFP_UL` 표기 불일치.** `CorrectionsManager` 의 비교 키는
  대문자 `P`, 디스크의 `GoldenJson/` 및 jsonpog 디렉토리는 소문자 `p` 였다. `runYear_` 는
  경로 성분으로 **그대로** 쓰이므로 2016 은 어느 쪽이든 깨져 있었다(그리고 golden JSON 은
  조용히 `return`). 생성자에서 `EraConfig::normalizeYear()` 로 정규화하고 키를 소문자로
  통일했다. **2016 은 여전히 미검증** — trigger path set 도 미정이라 명시적 FATAL 이다.

### 테스트 (ROOT/CMSSW 불필요, 즉시 실행 가능)

    g++ -std=c++17 -I include -o /tmp/t test/test_EraConfig.cc && /tmp/t
    ./test/test_EraConfig_fatal.sh

- `test/test_EraConfig.cc` — AN Table 32 값 정확 일치, L<M<T 단조성, `normalizeYear` 왕복,
  **2018 B/C 가 D 로 축약되지 않음**, HEM 영역 경계 6종, 2017 은 HEM 비활성. 전부 PASS.
- `test/test_EraConfig_fatal.sh` — 모르는 연도가 **exit 11** 로 죽는지 확인. PASS.
  (계약을 주석이 아니라 테스트로 고정하려는 것이다.)

### 남은 작업

- **빌드 미수행.** 이 환경에 ROOT 가 없어 `EraConfig.h` 단위 테스트까지만 검증했다.
  `make` 는 lxplus/KNU(CMSSW_14_2_1)에서 필요하다. **2017 재현성 확인이 필수** —
  2017 결과가 이전과 동일해야 한다(WP·prefiring·trigger 로직 모두 보존했으므로 동일해야 함).
- P0′ (#8–12), jet η-φ 2D map (HEM 검토용), P1 (trigger SF `IsoMu27`→`IsoMu24`, plateau 재확인).
- **HEM 결정 = AN 추종**: veto map 미사용, 2018 MC 에 JES 변주. `EraConfig::hemJes()` 에
  값과 근거를 넣어 뒀으나 **적용 코드는 아직 없다**(P0′ 이후). JME veto map
  (`Summer19UL18_V1` / `h2hot_ul18_plus_hem1516_plus_hbp2m1`) 은 최종 결과 전 재검토 항목.
