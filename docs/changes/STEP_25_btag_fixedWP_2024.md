# STEP 25 — 2024 b-tag SF: fixed-WP 방법(method 1a)과 우리 MC 효율 map, 2024 btagtrig (커밋 K)

- 날짜: 2026-10-08. 사용자 결정 [`../DECISIONS.md`](../DECISIONS.md) D-2026-10-08-A("일단 작업 자체는 fixed WP로 진행하다가 shape correction
  방법이 확인되면 그것으로 전환한다. 이 사항은 기록을 분명이 해 두어라" / "일단 analyzer를 그에 맞추어 작업하자").
- 신규: `include/BTagEffGroup.h`, `tools/stage7/btag_eff_maps.py`, `tools/stage7/test_btag_eff_maps.py`,
  `AnalyzerConfig/Tier3_2024_FH_unified_btagtrig.yml`, 이 문서
- 수정(C++): `include/EraConfig.h`(`BTagMethod`, `btagMethod`, `btagFixedWPPayload`), `include/ExitCodes.h`(E52),
  `include/CorrectionsManager.h`, `src/CorrectionsManager.cc`, `ttHHanalyzer_unified.h`, `ttHHanalyzer_unified.cc`
- 수정(Python·설정·시험): `submit_job_FH_Tier3_unified.py`, `plotter/make_plots.py`, `AnalyzerConfig/Tier3_2024_FH_unified_{main,prescan}.yml`,
  `test/offline_smoke/{fake_pog.py, synth_nano.py, run_offline_smoke.sh}`, `test/test_failure_checks.py`, `test/test_EraConfig.cc`,
  `tools/stage3/smoke_2024.sh`
- 수정(문서): `README.md`, `docs/{DECISIONS.md, PLAN_v15_2018UL_2024.md, STATUS.md, CHANGELOG.md}`,
  `docs/reference/{ERROR_CODES.md, CONFIG_PATHS.md}`; 워크스페이스 `RUNBOOK_lxplus_2026-09-16.md` §28, `00_START_HERE.md` (18)
- KNU: **다시 빌드는 지금의 큐(10-08 03:43 KST 9,890 job)가 빈 뒤**(job 이 실행 파일을 쓴다). header 가 바뀌었으므로 `build_check.sh` 의 clean
  빌드로(최상위 `Makefile` 에 header 의존성이 없다, PLAN §9.3).
- 커밋 K2(10-08, §10): condor stall guard — 수정 `submit_job_FH_Tier3_unified.py`, `test/test_failure_checks.py`(G), `README.md`,
  `docs/{STATUS.md, CHANGELOG.md}`, 이 문서 §10; 워크스페이스 RUNBOOK §28. 제출기만이라 KNU 에서 다시 빌드할 필요 없음.

## DECIDED / 실측

### 1. 사실과 결정

- **payload**(KNU 의 jsonpog 목록 10-08, PLAN §9.2): `POG/BTV/2024_Summer24` 는 2025-09-24 판 그대로. SF 는 `btagging_preliminary.json.gz`
  의 `UParTAK4_kinfit` 하나 — fixed WP, **b jet(flavour 5)만**, |η| < 2.5 한 bin, pT 20–600 GeV 8 bin, 입력 `systematic, working_point,
  flavor, abseta, pt`, 계통은 fsrdef·hdamp·isrdef·jer·jes·mass·statistic·tune 로 나뉨(정확한 key 이름은 아직 모름 → §4). shape SF 도 c·light
  SF 도 없다. Run 2 의 방법(deepJet shape SF + norm reweight)은 그대로 못 간다(PLAN §9.6 D10).
- **지금까지의 control plot 에는 trigger SF 도 b-tag SF 도 없다**(D-2026-10-05-A 의 첫 look, 출력 이름의 `_notrig`; 2024 는 `--btagsf on` 이
  E11 로 막혀 있었다). Stage 6(trigger SF)과 Stage 7(b-tag)이 그것이고, 이 커밋이 Stage 7 의 2024 부분.
- **결정**: 2024 는 지금 BTV 의 fixed-WP event weight("method 1a")를 우리 두 WP 로; c·light jet 은 SF 1; 효율은 우리 MC; norm reweight 없음;
  BTV 의 2024 shape 방법이 확인되면 그것으로(`EraConfig::btagMethod("2024")` → `Shape`, 그때 norm reweight 도 2024 용으로). 목표는 2024·2018
  의 FH·μCR·eCR control plot 을 trigger SF(와 그 검증 plot)·b-tag SF 와 함께 보고 2017 v9 와 비교하는 것.
- **사용자의 예상**("fixed WP 를 쓴다고 ratio 가 크게 변할 것은 없다")에 대한 AI 의 답(결정 본문): b-tag 수별 수율과 운동학 분포에는 맞다(두
  방법 다 우리 범주를 정하는 WP 의 tag 비율을 고친다). 맞지 않거나 아직 아닌 것: (a) b-tag 점수 분포(fixed WP 는 세 칸만 고치고 칸 안의 모양은 MC
  그대로), (b) mistag — nb ≥ 2 의 FH(multijet 많음)에서는 tag 된 jet 의 상당수가 c·light 이고(10-07 HT1050 control region 의 Data/MC 1.46 이 중간
  점수에 있었다), 2024 에는 c·light SF 가 없어 어느 방법도 그것을 못 고친다, (c) 2017 v9(shape SF + norm reweight)와의 비교는 점수 모양이 아니라
  b-tag 범주·운동학으로, (d) jet 수의 기울기는 b-tag 효과가 아니다.

### 2. 방법 (`ttHHanalyzer_unified.cc` `computeBTagWeightFixedWP_`)

선택된 jet 은 우리 selection 의 두 WP 로 세 칸 중 하나다: b jet = 점수 ≥ M(`selectbJet`), W 의 light jet = 점수 < L(`selectLightJet`). jet 마다
그 칸의 P_Data / P_MC:

| 칸 | P_MC | P_Data | weight |
|---|---|---|---|
| 점수 ≥ M | e_M | SF_M e_M | SF_M |
| L ≤ 점수 < M | e_L − e_M | SF_L e_L − SF_M e_M | (SF_L e_L − SF_M e_M) / (e_L − e_M) |
| 점수 < L | 1 − e_L | 1 − SF_L e_L | (1 − SF_L e_L) / (1 − e_L) |

event weight 는 모든 선택 jet 의 곱(`bTagWeight`). M 하나만 쓰는 두 칸 판은 쓰지 않았다: W 의 jet 이 L 로 정의되므로 L–M 은 따로 셀 칸이다.
- e_L, e_M: 우리 MC 효율(§3), jet 의 flavour·pT·|η|·process 묶음. [1e-4, 1 − 1e-4] 로 자르고 e_M ≤ e_L; 분모가 1e-6 보다 작거나 분자가 음수면
  그 jet 의 weight 는 1 / 0 쪽으로(코드의 `max(0, …)`).
- SF: `UParTAK4_kinfit`(b jet). 입력은 payload 가 선언한 이름의 순서대로 넣고(load 때 이름 확인; 모르는 입력이면 그 payload 를 쓰지 않음),
  |η| 는 [0, 2.4999], pT 는 [20, 599.9] 로 잘라 넣는다(payload 의 binning 밖은 flow `error` 일 수 있다). **c·light jet 은 SF 1** — weight 정확히 1
  (`btagFixedWPCovers`); BTV 가 c·light 를 주면 `EraConfig::btagFixedWPPayload` 의 `bJetsOnly` 를 끄고 같은 식이 그것을 쓴다.
- up/down(`bTagWeight_up/_down` branch, 새; 2024 에만): payload 의 `up`/`down` 이 있으면 그것, 없으면 `correlated`+`uncorrelated` 짝(둘 다 있을 때),
  없으면 kinfit 출처들(`up_<s>`/`down_<s>` 또는 `<s>_up`/`<s>_down`)을 central 과의 차의 제곱합으로; 아무것도 없으면 central. 어느 것을 찾았는지
  load 줄이 말한다(`... -> UParTAK4_kinfit (b jets; ...); up/down: ...`). 두 묶음을 함께 더하지 않는다(한 쪽이 다른 쪽의 분해일 수 있어 이중
  계산). lf·cferr 칸(2017 shape 의 branch)은 central 과 같게.
- **norm reweight 없음**: method 1a 는 효율이 그 MC 에 맞으면 b-tag 요구 전의 정규화를 지킨다(jet 마다 세 칸의 기댓값 = 1). 그 확인이 job 끝의
  `[btagSF] closure at the HT step (no b-tag cut yet): events N, sum(w x bTagWeight) / sum(w) = X` 줄(HT 단계 = b-tag 요구 전, MC 마다).
- `--btagsf on`(2024 MC): production `evtWeight` 에 `bTagWeight` 를 곱한다(Run 2 의 shape 와 같은 자리, `applyEventScaleFactors`). `--btagsf off`
  여도 효율 JSON 이 주어지면 `bTagWeight` branch 와 `evtWeight_btagSF` tier 는 채워진다(production weight 만 그대로). Data 는 weight 없음.

### 3. 효율 map

- **analyzer**: 2024 MC(prescan 제외)의 출력에 `BTagEff/` — `h2_<b|c|l>_<all|L|M|T>`(선택 jet, pT 13 bin 20…1000 × |η| 4 bin 0–2.5, Sumw2) 와
  `h_nevt`(채운 event 수), `h_wp`(그 해의 WP L·M·T × job 수, 4 번째 칸 = job 수: hadd 가 더하므로 merge 된 파일에서 WP = 칸 / 4 번째 칸).
  HT 단계(b-tag 을 뺀 모든 cut 뒤; main 은 trigger 와 lepton veto 또는 CR lepton 뒤, btagtrig 는 둘 다 없이)에서, SF 전의 event
  weight(base × PU × genW × stitch)로. 2017 출력에는 없다(출력은 J 와 같음, §6).
- **묶음**(`include/BTagEffGroup.h` = 도구의 `btag_eff_group`, 시험이 2024 표본 이름 100 개로 둘을 맞춘다): `qcd`(QCD*), `tt`(tt/TT 로 시작:
  ttbar·ttH·ttHH·ttV·tttt …), `other`(single top, tH, V+jets, VV …), 그리고 모두를 더한 `all`. map 에 없는 묶음은 `all`(로그에 한 줄).
- **도구** `tools/stage7/btag_eff_maps.py --config <yml> --base <merge 된 출력>`: yml 의 MC 표본(Data 건너뜀; 2024 의 4FS TTbb_*·TT4b 는 stack 처럼
  뺌, D-2026-10-05-C)의 `<base>/<sample>.root` 에서 `BTagEff/` 를 묶음마다 더한다. 표본마다 `h_nevt` = 그 파일의 `Tree/cutflow` `HT>500` 칸이어야
  한다(그 표본의 모든 job 이 히스토그램을 채웠다; 다르면 MISMATCH, 아무것도 쓰지 않음). 칸마다 e = Σw(점수 ≥ WP) / Σw, 분모의 유효 jet 수
  N_eff = (Σw)² / Σw² 가 `--min-neff`(기본 50) 이상이고, **b jet 은 weight 가 쓰는 세 tag 칸(점수 < L, L–M, ≥ M) 각각의 N_eff 가
  `--min-neff-bin`(기본 10) 이상일 때**; 아니면 그 묶음의 그 pT 칸을 |η| 로 더한 것 → `all` 의 칸 → `all` 의 |η| 합 → `all` 전체(전체는 세 칸이
  0 보다 크기만 하면). 합이 비물리적인 칸(0 ≤ num_T ≤ num_M ≤ num_L ≤ den 이 아님: 성긴 칸의 음의 weight)도 다음 단계로. 왜: 0 이나 1 에 붙은
  효율은 그 WP 반대편 jet 에 1/1e-4 까지의 weight 를 준다 — map 을 만든 표본에는 그 칸에 jet 이 없지만, 그 map 으로 weight 를 받는 다른 표본(stack
  에서 빠진 4FS TTbb_*·TT4b 가 tt map 으로, 빠뜨린 표본)에는 있다(10-08 검토가 컨테이너에서 보인 일: QCD 묶음의 b map 이 e = 1 → 다른 파일의
  QCD 에서 closure 4·10⁹; 그 전에도 음의 weight 로 `bTagWeight` 2·10⁵, closure −2113 → 지금 규칙 뒤 0.991, 다른 표본 1.04, jet weight > 10 없음).
  tag 칸 규칙은 `--flavours-required`(기본 `b`: 2024 payload 는 b SF 뿐이라 analyzer 는 c·light jet 에 weight 1 이고 그 map 을 읽지 않는다; c·light
  SF 가 오면 `b c l`)의 flavour 에만; 나머지는 N_eff·물리 규칙만(NOTE 줄). 그리고 표본마다 `h_wp` 의 WP 가 같아야 하고(다르면 FAIL), 그 값을
  description 의 `wp=L:..,M:..,T:..` 로, `--flavours-required` 를 `flavours_required=` 로 적는다. 출력: correctionlib JSON(schema v2)
  `btag_eff`(입력 group, flavor, working_point, abseta, pt; multibinning abseta × pt, clamp; 모르는 group 은 `all`)와 `btag_eff_groups`(group →
  1/0), description 에 `year=<YYYY>`·`wp=`·`flavours_required=`·base·표본 수·시각; correctionlib Python 으로 모든 칸 중심을 다시 읽어 비교
  (`VERIFY ok`). `--root-out`(map·단계·N_eff 의 TH2D), `--plots`(map 의 여러 쪽 PDF), `--check-only`.
- **제출기**: `path_btag_eff_json` → `TTHH_BTAGEFF_JSON`(2024 의 모든 job 이 키를 읽는다: 없으면 E12; null → `__NULL__`); `--btagsf on` 이면 MC
  job 에 필수(null → E13; Data 는 아님). preflight: 그 JSON 을 node 마다 읽는다 — `btag_eff` = group category(default 있음) → flavor(0·4·5) →
  working_point(L·M·T) → (abseta, pt) multibinning, 값 [0, 1]; `btag_eff_groups` 의 default; description 의 `year=`(yml 과 같아야)와 `wp=`;
  `[PASS] 2024 b-tag efficiency JSON ... (groups tt qcd other all)`, `[PASS] 2024 --btagsf on: the fixed-WP b-tag weight ...`.
- **어디서 만드나**: 2024 btagtrig 의 MC(§5) — trigger 와 lepton 요구 없는 FH preselection. BTV 권고는 "분석의 phase space 에서" 이고 SR 은 trigger
  뒤라 b-tag trigger 가 효율을 조금 바꿀 수 있다; 그러나 tag 된 jet 의 weight(SF_M)는 효율과 무관하고 나머지 칸의 효율 민감도는 (1 − SF) 배라
  작다. main 출력의 closure 줄이 1 에서 1–2 % 넘게 벗어나면 main MC 의 `BTagEff/`(trigger·lepton 요구 뒤)로 map 을 다시 만든다(결정의 supersede).
  **[10-10 정정, D-2026-10-10-A]** 위의 "(1 − SF) 배라 작다" 는 틀렸다: 효율이 바뀔 때 tag 되지 않은 jet 의 weight 는 (1 − SF)/(1 − ε)² 배로
  움직여, b jet 의 L 실패 칸(1 − e_L ≈ 0.05)에서는 작지 않다(SF_L = 0.97 에서 e_L 0.95 → 0.96 이면 1.57 → 1.72). 그리고 2024 FH trigger
  (ParkingHH)는 HLT 에서 PNet b-tag 을 요구해 trigger 뒤의 효율이 btagtrig 보다 높을 수 있다. 그래서 첫 SF main 의 `BTagEff/`(BTV 가 말하는
  영역: 분석 selection 뒤, b-tag 요구만 빼고)로 map 을 다시 만들어 btagtrig map 과 비교한다 — closure 줄과 상관없이.

### 4. 실패 처리

| 경우 | 결과 |
|---|---|
| 2024 job 에 `TTHH_BTAGEFF_JSON` 없음 | E12(제출기가 키 없는 yml 을 막는다) |
| `--btagsf on`, MC, 효율 JSON null | 제출 E13; 직접 실행이면 analyzer E52 `needs our MC efficiency JSON` |
| `--btagsf on`, MC, BTV payload 를 못 읽음 | E40 `the BTV payload SF is not loaded: <이유>`(load 는 central 을 L·M 에서 \|η\| 0·1.25·2.4999 × pT 20·55·599.9 격자로 평가해 본다) |
| `--btagsf off`, payload 를 못 읽음 | WARN 한 줄, 그대로 감(그 weight 를 쓰지 않는 실행 — 첫 look·btagtrig — 이 preliminary payload 에 묶이지 않게) |
| 효율 JSON 이 깨짐(main/debug) | E52; btagtrig/prescan 은 WARN |
| 효율 JSON 이 읽히지만 답하지 못함 — 이 job 의 묶음이나 `all` 의 flavour·WP 하나가 없음, `btag_eff_groups` 가 이 job 의 묶음에 답하지 못함(default 없음), 다른 WP 로 만듦(`wp=`), payload 가 SF 를 주는 flavour 가 `flavours_required=` 에 없음 | load 때 E52(main/debug; btagtrig/prescan WARN) — 10-08 검토 전에는 event loop 의 uncaught exception(exit 134) |
| event loop 의 효율 평가 오류 | E52(`getBTagEff`, `btagEffHasGroup`; abort 도 조용한 값도 없음) |
| payload 에 알아보는 up/down 이 없음(키가 default 에만 닿는 것 포함: central 과 다른 값이어야 센다) | load 때 WARN, `--btagsf on` 이면 WARN 한 번 더; `bTagWeight_up/_down` = central; KNU smoke_2024 는 FAIL 로 보인다 |
| b jet SF 평가 오류(job 중) | E40(조용한 1 없음) |
| Data 의 `--btagsf on` | 그대로(weight 없음) |

payload 의 계통 key 는 아직 모른다(위 §2 의 순서로 찾는다). KNU 의 smoke_2024 가 진짜 payload 로 처음 load 하며 `BTAGSF <run> ...` 줄에 무엇을
찾았는지 적는다; NtupleForge `script/jsonpog_inventory.py --name-regex UParTAK4_kinfit` 의 `KEYS` 줄이 그 목록(워크스페이스 RUNBOOK §28 4·5).

### 5. 2024 btagtrig (`AnalyzerConfig/Tier3_2024_FH_unified_btagtrig.yml`)

Muon0/1 C–I 16 dataset(era I 는 두 판) + main 과 같은 MC 60, files per job MC 10·Data 1, 경로는 main 과 같고 trigsf·btagrw·stitch·expttid·효율은
null. 한 run 이 두 곳에 쓰인다(2017 RUNBOOK_2017_SF_rederive §3 과 같음): trigger SF(Muon0/1 의 `HLT_IsoMu24` + muon 1 개, TriggerStudy 의 2024
설정은 Stage 6 의 2 부)와 효율 map(MC 의 `BTagEff/`). 제출: `--mode btagtrig --trigsf off --btagsf off --btagrw off` → 출력
`AnalyzerOutput_btagtrig_notrig_2024`. 참고: btagtrig 의 lead muon 문턱 `Cuts::leadMuonPt` = 29 GeV 는 2017 `IsoMu27` 의 plateau 다; 2024 의
`IsoMu24` 에는 보수적이고(plateau 위), 바꾸지 않는다(AN 근거 없이 바꾸지 않음; Stage 6 에서 turn-on 을 보고 정한다).

### 6. 시험 (컨테이너, ROOT 6.40 pip·correctionlib 2.9; KNU 는 ROOT 6.30)

- 오프라인 smoke(`test/offline_smoke/run_offline_smoke.sh <K 빌드> <J 빌드>`): **83/83**(J 의 같은 실행 50, +33): payload load 줄과 "NOT ready",
  `--btagsf on` 의 E52(효율 없음)·E40(payload 없음), 효율 JSON 깨짐 main E52·btagtrig WARN, `TTHH_BTAGEFF_JSON` 없음 E12, Data `--btagsf on` 통과,
  btagtrig MC 둘 → `btag_eff_maps.py`(h_nevt = HT 단계, VERIFY) → `--btagsf on` run 의 `bTagWeight`/`_up`/`_down` 을 Tree/Tree 의 jet 에서
  correctionlib 으로 다시 계산(588 event, b jet 1,453, 상대 차 최대 6e-8), closure 0.992(새 synth 표본 `--b-low 0.3`: b 효율이 (0, 1) 안),
  sources 만 있는 payload(`fake_pog.py --btv-sources`: `up_jes/down_jes`, `statistic_up/statistic_down`)의 제곱합, main 의 `evtWeight(on) =
  evtWeight(off) × bTagWeight`(같은 event, 같은 bTagWeight), main 출력의 `BTagEff` 와 Data 에 없음; [10-08 검토 뒤] job 끝의 jet weight 수 줄,
  default 에만 닿는 payload(`fake_pog.py --btv-default`: 변주 없음, WARN 둘, up = down = central), map 을 만들지 않은 표본(QCD 를 다른 파일에서:
  jet weight > 10 없음, closure 1.04), 읽히지만 답하지 못하는 효율 JSON 셋(groups default 없음·tt b 의 L 없음·다른 WP: load 때 E52, btagtrig 는
  WARN). 2017 출력 넷은 J 빌드와 같음(y1_compare PASS).
- 2024 출력 J 대 K(`y1_compare.py`, MC main·Data main·MC btagtrig): 기존 히스토그램·tree 전부 같고 새 것은 `BTagEff/` 13 개와 두 branch 뿐 →
  큐에 있는 J 출력과 K 로 다시 낸 job 의 출력이 섞여도 기존 plot 은 같다(효율 map 만 J 출력에서는 못 만든다: NO-BTAGEFF).
- `tools/stage7/test_btag_eff_maps.py` **26/26**: C++·Python 묶음이 2024 표본 100 개와 경계 20 개에서 같음, 칸마다 num/den(pT × |η| 순서),
  `all` 합, fallback 1·2 단계, 비물리 칸 건너뜀, b 의 e_L = 1 칸 건너뜀(thin), c 는 그대로(`--flavours-required b c l` 이면 건너뜀), `h_wp` 의
  WP 가 description 에, 모르는 group → all, clamp, 제출기의 JSON 읽기(연도 다르면 문제), `--root-out`/`--plots`, MISMATCH·MISSING·
  `--allow-missing`·NO-BTAGEFF·`--check-only`·다른 WP 의 표본.
- `test/test_failure_checks.py` **56/56**(+7, F): preflight 의 null+on FAIL, 쓸 수 있는 JSON PASS(묶음), 2017 JSON FAIL, 키 없음 FAIL, off PASS,
  JSON 을 node 마다(groups default 없음, WP 하나 없음); 제출의 env(`__NULL__`, on 이면 MC E13·Data 통과, 실제 경로, 2017 은 내보내지 않음,
  2024 키 없음 E12).
- 단위 시험 PASS(`test_EraConfig` 에 +4: 2024 FixedWP, Run 2 Shape, payload 파일·이름·b only). stage0 19/19, consolidate 24/24, runlog 83/83,
  build_check 18/18; stage1 15/16 은 J 와 같은 환경 항목(`../treestream` 빌드 없음).
- `tools/stage3/smoke_2024.sh`(KNU 의 진짜 파일 smoke)에 MC 마다 payload load 확인과 `BTAGSF` 줄, `BTagEff/h_nevt` = HT 단계(Data 는 없음),
  새 run `mc_bt`(btagtrig MC)·`mu_bt`(btagtrig Muon0 era I, filelist 가 있으면); btagtrig 의 HadTrigger 칸은 모든 event(= noCut)라 그 검사를
  나눔; payload 의 up/down 을 찾았는지(못 찾으면 FAIL: 진짜 kinfit 의 key 가 코드가 아는 것이 아님). 컨테이너의 모의(가짜 payload·synth 파일):
  **106/106**(G 81).
- plotter: `BTagEff/` 는 구조 목록에서 뺀다(MC 에만 있어 Data 쪽이 비고 TH1D 를 TH1F 로 읽음); SF 표기 `trigger SF + b-tag SF (fixed WP, b jets)
  applied` 등.

### 7. KNU 의 10-08 상태 (사용자가 붙인 출력, 03:43 KST)

- FH 의 ParkingHH 8 은 끝, μCR 은 ParkingHH F–I 가 남음, eCR 8,062 대기; 큐 9,890; `du` job 은 10 시간 넘게 도는 중; `proxy.cert` 10-16 03:42 까지.
- P8(10-05 생산, dataset 별): MC 7/60 FAIL 출력 없는 job 25 — TTbar_Hadronic 3, TTbar_DiLep 1, QCD_HT600to800 1, QCD_HT800to1000 2,
  ttHTobb_semilep 10, ST_s_top_had 5, WJetsToQQ_HT2500toInf 3; Data 5/32 FAIL 24 — 모두 JetMET: JetMET0 C 1·F 15·G 5, JetMET1 C 1·E 2.
  ParkingHH ALL PASS. MC 의 빠진 job 은 정규화에 영향 없음(prescan 이 같은 filelist 의 Σgenw); Data 는 lumi 가 모자란다.
- 처리된 golden LS 의 빠짐: JetMET0 2.080 %, JetMET1 0.703 %, ParkingHH 0.398 %(ParkingHH 는 dataset 쪽 빈칸). era B 는 쓰지 않음(16 run, 5,939 LS).
  결과 전에 CRAB 로 되살리거나, 세 PD 공통의 처리된 LS mask 를 만들어 brilcalc 로 lumi 를 다시(OPEN).

### 8. 다음 (워크스페이스 RUNBOOK §28, 큐가 빈 뒤)

1. 지금 run 셋(FH, μCR, eCR; SF 없음)의 완료 확인·merge·plot — J 실행 파일 그대로.
2. 맥 커밋 K → KNU pull, clean 빌드, 단위 시험, smoke_2024(진짜 payload 의 load 줄과 계통 key), jsonpog 목록의 kinfit `KEYS`.
3. 2024 btagtrig preflight·제출(Muon0/1 + MC) → merge → `btag_eff_maps.py`(map + PDF) → 커밋.
4. Stage 6 의 2 부(AI): TriggerStudy 의 2024 설정(`Config.hh` 의 연도, `HLT_IsoMu24` 기준, 2024 경로 bit, lumi 109.816, 2017 binning; `Makefile`
   은 그대로) → trigger SF·검증 plot.
5. main 셋을 `--trigsf on --btagsf on` 으로(main yml 에 `path_trigsf_dir`·`path_btag_eff_json`) → closure 줄 확인 → plot, 2017 v9 와 비교.
6. 2018 v15 경로(PLAN Y3; 2018 은 shape SF + norm reweight 그대로).

### 9. 독립 검토 (10-08, 반영)

다른 AI agent 가 diff 전체를 보았다(코드만 읽고, 시험은 자기 임시 디렉터리에서). weight 식과 closure, weight·히스토그램·selection 이 같은 jet·WP·
SF 전 weight 를 쓰는 것, `computeBTagWeight` 가 HT 단계 전에 모든 MC event 에서 도는 것, 히스토그램이 한 번만 쓰이는 것, correctionlib 입력의
순서·type 과 multibinning 순서, 2017 이 그대로인 것, Python 3.9 문법은 확인. 확인된 결함 둘을 고쳤다:
1. **map 이 0·1 의 효율을 받아들임** → 다른 표본에서 jet weight 수백, event 1e11. 고침: b jet 의 세 tag 칸 규칙(`--min-neff-bin`), `--flavours-
   required`, 시험(e_L = 1 칸, 다른 파일의 QCD).
2. **읽히지만 답하지 못하는 효율 JSON 이 event loop 에서 abort(134)**, preflight 는 통과 → 고침: load 때 이 job 의 묶음과 `all` 의 모든
   flavour·WP 를 map 안팎에서 평가, `btag_eff_groups` 가 이 job 의 묶음에 답하는지, WP(`h_wp` → `wp=`)와 `flavours_required=`; event loop 의
   평가는 try/catch → E52; preflight 는 node 마다.
그럴 법하다고 한 것 가운데 반영: 변주를 못 찾으면 WARN 둘과 KNU smoke FAIL, default 에만 닿는 key 는 세지 않음(central 과 달라야), payload 를
격자로 평가, P_Data < 0 으로 0 이 된 jet 수와 weight > 10 인 jet 수를 job 끝에. 그대로 둔 것: 효율 JSON 을 주면 `--btagsf off` 여도 weight 를
계산하므로(branch·tier) b jet SF 평가 오류는 E40 이다(load 의 격자 평가 뒤라 일어날 일이 거의 없다); pT > 600 의 SF 는 마지막 칸 값(BTV 관례,
불확도를 키우지 않음 — 물리 선택, AN 근거와 함께 Stage 9 에서).

### 10. 커밋 K2 — condor stall guard (10-08, 제출기만; analyzer·물리 그대로)

**일어난 일(KNU, 사용자가 붙인 출력).** 17:02 KST 에 세 출력(FH, μCR, eCR; SF 없음) 모두 파일 8,062 개였지만 큐에 analyzer job 7 개가 'running'.
17:41 의 확인:

| job | 표본 | 시작 | `.out` 마지막 변경 | 마지막 진행 | CPU / 경과 (s) | machine |
|---|---|---|---|---|---|---|
| 2181307.855 | μCR ParkingHH_Run2024G (Data) | 04:40 | 06:05 | 30,000 / 197,479 | 6 / 46,867 | cluster333 |
| 2181314.5 | eCR ttHTobb_dilep | 04:55 | 05:24 | 470,000 / 565,366 | 93 / 45,918 | cluster348 |
| 2181315.14 | eCR ttHToNonbb | 05:03 | 06:01 | 610,000 / 1,014,362 | 146 / 45,469 | cluster333 |
| 2181319.3 | eCR tHW | 05:34 | 05:44 | 710,000 / 819,975 | 149 / 43,610 | cluster333 |
| 2181319.24 | eCR tHW | 05:43 | 05:52 | 510,000 / 668,493 | 114 / 43,068 | cluster333 |
| 2181320.1 | eCR TTbar_Hadronic | 05:44 | 05:57 | 1,590,000 / 2,697,372 | 320 / 42,987 | cluster333 |
| 2181320.26 | eCR TTbar_Hadronic | 05:53 | 05:58 | (입력 파일의 branch 확인 단계) | 2 / 42,471 | cluster333 |

모두 05:24–06:05 사이에 멈췄고(입력을 /pnfs 에서 읽다가; 7 개 중 6 개가 cluster333), 그 뒤 12 시간 CPU 를 쓰지 않았다. 정상일 때는 초당 1,000–2,000
event(tHW_3: 10 분에 71 만, CPU 25 %; TTbar_Hadronic_1: 13 분에 159 만, CPU 41 %). condor 의 'running' 은 프로세스가 살아 있다는 뜻일 뿐이고 submit
파일에 시간·CPU 규칙이 없었으므로 아무 일도 일어나지 않았다(노드가 죽으면 lease 가 끝난 뒤 Idle 로 돌아가지만, 입력에서 멈춘 프로세스는 계속
'running'). 파일 수 8,062 는 완료가 아니다: analyzer 는 시작할 때 출력 파일을 RECREATE 로 만들고(`src/tnm.cc`) 내용은 event loop 가 끝난 뒤
쓴다 — 완료는 `--report`(종료 마커 `cutflow_w_full`; 도는 job 은 `wait`). 워크스페이스 RUNBOOK §28 1·7 의 확인 명령도 고침(전의 `condor_q -totals
| tail -2` 는 모든 사용자의 합계를 보였다). 조치(사용자에게): 7 개를 `condor_hold` → `condor_qedit` 로 Requirements 에 cluster333 제외 →
`condor_release`(같은 job 이 처음부터, 같은 출력; `--resubmit` 은 큐의 다른 job 도 다시 내므로 쓰지 않음). **AI 의 실수(18:28 에 드러남):** 그
명령이 식을 `condor_q -af Requirements` 로 읽었는데 `-af` 는 식을 평가하므로(TARGET 이 없어 `undefined`) Requirements 가 `(undefined) && (TARGET.Machine
isnt "cluster333.knu.ac.kr")` 가 되어 job 7 개가 Idle 로 멈췄다(`condor_q -better-analyze`: 74 machine 모두 거부). 되살리기: 같은 cluster 의 끝난
job 에서 `condor_history <cluster> -limit 1 -af:r Requirements`(평가하지 않은 원래 식)를 가져와 cluster333·cluster348 을 빼고 다시 넣음(워크스페이스
RUNBOOK §28 의 '10-08 18:28'). condor 식을 읽어 다시 쓸 때는 `-af:r`. 18:28 의 cluster333: slot1_2·slot1_24 가 14 시간 넘게 Busy·LoadAv 0.000.

**바꾼 것(`submit_job_FH_Tier3_unified.py`).** 모듈 상수와 `stall_guard_exprs()`·`stall_guard_summary()`; `write_condor_submission_file()` 이
`--stall-guard on`(기본)일 때 `queue` 앞에 주석 한 줄과 다섯 줄:

```
periodic_hold           = (JobStatus == 2) && ((time() - EnteredCurrentStatus) > 3600) && ((ifThenElse(isUndefined(RemoteUserCpu), 0, RemoteUserCpu) + ifThenElse(isUndefined(RemoteSysCpu), 0, RemoteSysCpu)) < 0.05 * (time() - EnteredCurrentStatus))
periodic_hold_reason    = strcat("tthh stall guard: running ", string(int((time() - EnteredCurrentStatus) / 60)), " min with ", string(int((...CPU...))), " s CPU on ", ifThenElse(isUndefined(RemoteHost), "?", RemoteHost))
periodic_hold_subcode   = 4201
periodic_release        = (HoldReasonCode == 3) && (HoldReasonSubCode == 4201) && (NumJobStarts < 3) && ((time() - EnteredCurrentStatus) > 300)
requirements            = isUndefined(LastRemoteHost) || ((LastRemoteHost != TARGET.Machine) && (substr(LastRemoteHost, size(LastRemoteHost) - size(TARGET.Machine) - 1) != strcat("@", TARGET.Machine)))
```

- hold: 지금 run(EnteredCurrentStatus 부터)이 1 시간을 넘고 CPU(user + system; 아직 값이 없으면 0)가 그 시간의 5 % 미만. 비율은 run 전체의 것이라
  건강하게 시작한 뒤 멈춘 job 은 (그 CPU 초) / 0.05 뒤에 잡힌다: TTbar_Hadronic_1 은 1.8 h, 2181320.26 은 1 h. CPU 를 다 쓰면서 느린 job
  (무한 loop)은 잡지 않는다. 앞의 run 의 CPU 가 함께 세어지는 condor 라도 잡는 시점이 늦어질 뿐이다.
- release: 이 hold(HoldReasonCode 3 = job policy, subcode 4201)만, hold 5 분 뒤(멈춘 프로세스가 죽을 시간), 시작이 3 번 미만일 때 — 같은 job 이 같은
  인자·출력으로 처음부터(analyzer 가 출력 파일을 지우고 새로 만든다, 중복 없음). 세 번째 run 도 멈추면 held 로 남는다: `--report` 의 wait,
  `--status` 의 hold 이유, 그 job 의 입력 파일을 볼 것. 사용자 hold(code 1)·메모리 hold(34) 등은 건드리지 않는다.
- requirements: 마지막에 돈 machine(LastRemoteHost 의 '@' 뒤, 또는 이름 그대로)은 피한다; 처음 run 은 제한 없음. condor_submit 이 기본 조건
  (Arch, OpSys, Disk, Memory, FileSystemDomain/HasFileTransfer)을 `&&` 로 붙인다(24.0 의 condor_submit 논리로 확인). `split()`·
  `stringListMember()` 는 HTCondor 확장이라 순수 ClassAd 라이브러리(그리고 시험)에 없고, `regexp()` 로 끝을 정확히 맞추려면 submit 파일의 macro
  문자 `$` 가 필요해서 `substr`/`size` 로 썼다.
- `--stall-guard on|off`(기본 on; off 면 위 줄들을 쓰지 않음), preflight 의 `[PASS] condor stall guard ...`(off 면 WARN), 제출 때 `[stall-guard]`
  줄, `--report` 의 wait 안내에 stall guard 의 held 설명. merge(`outputMerger`)·`tools/runlog/condor_run.sh` 의 job 에는 넣지 않았다.

**시험.** `test/test_failure_checks.py` G: submit 파일의 다섯 줄이 `stall_guard_exprs()` 그대로·주석 줄·`queue` 가 마지막; `--stall-guard off` 면 그
줄들만 빠짐(나머지 같음), 속성이 없는 객체(전의 호출)는 on; preflight 의 PASS/WARN 줄; ClassAd 모듈(`classad2` 또는 `classad`; `pip install
htcondor`)이 있으면 식 평가 — 10-08 의 job 들은 hold, 3 h·90 %·10 % CPU·0.8 h·idle 은 아님, TTbar_Hadronic_1(320 s)은 1.5 h 에는 아니고 2 h 에,
release 는 첫·둘째 뒤에만·5 분 뒤에만·다른 hold 는 아님, requirements 는 cluster333 거부·cluster348·cluster33·첫 run 허용. 컨테이너: 59/59(모듈
없음), 60/60(HTCondor 25.14.1 `classad2`, 24.0.24 `classad`). 따로: 24.0 의 `htcondor.Submit(...).jobs()`(condor_submit 과 같은 논리)로 submit
파일을 job ad 로 펼쳐 PeriodicHold/PeriodicHoldReason/PeriodicHoldSubCode/PeriodicRelease 와 기본 조건이 붙은 Requirements 를 확인하고, 그 ad 로
cluster333 거부·cluster348 허용, 10-08 의 값에서 hold 와 이유 문자열(`tthh stall guard: running 720 min with 149 s CPU on slot1_1@cluster333...`)을
확인. KNU 의 condor 버전에서의 실제 동작(첫 hold·release)은 다음 제출에서 본다.

**되돌리기.** 이번만: `--stall-guard off`. 전부: 이 커밋을 revert(제출기와 시험·문서뿐; analyzer 는 다시 빌드할 필요 없음).

## 확인하지 못한 것

- 진짜 `UParTAK4_kinfit` 의 계통 key 이름과 binning 의 flow(코드는 둘 다 견딘다; KNU smoke 의 `BTAGSF` 줄과 jsonpog 목록으로 확인).
- 진짜 MC 의 효율 값과 fallback 이 얼마나 쓰이는지, main 출력의 closure(효율 map 을 만든 뒤 처음 보인다).
- KNU 의 ROOT 6.30·correctionlib(CMSSW_14_2_1) 에서의 빌드와 Python 3.9 에서의 도구(컨테이너는 ROOT 6.40, Python 3.11; 문법은 3.9 에 맞춤).
- KNU 의 `filelistTier3_2024/` 에 Muon0/1 의 filelist 가 있는지(btagtrig preflight 가 본다; 없으면 `make_filelists_v15.py --year 2024`).
- K2: KNU 의 HTCondor 가 `periodic_hold_reason`·`periodic_hold_subcode` 를 받는지와 첫 실제 hold·release(다음 제출의 job ad:
  `condor_q <job> -af PeriodicHold PeriodicRelease Requirements`); 10-08 의 /pnfs 멈춤의 원인(KNU 관리자).
