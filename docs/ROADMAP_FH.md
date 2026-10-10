# ROADMAP — ttHH(→4b) FH: 지금부터 결과까지의 작업 흐름

> **Purpose:** FH 분석 전체를 워크스트림(W1–W8)으로 나누고, 각각의 지금 상태·다음 할 일·막는 것·문서를 한 장에 둔다. 순서와 의존성의 단일 출처.
> 세부 계획은 각 워크스트림의 문서에, 오늘의 상태는 [`STATUS.md`](STATUS.md)에, 결정은 [`DECISIONS.md`](DECISIONS.md)에 있다(여기는 링크만).
> **Audience:** 이 분석을 이어받는 사람과 AI 세션(대화 기억 없음).
> **Status:** **PROPOSED** (AI 2026-10-10, 사용자 방향 D-2026-10-10-B 를 받아 작성; 사용자 검토 전; 10-10 (3) §6 에 7–10 추가 — AN_KR 을 쓰며 찾은 것). 갱신은 워크스트림의 상태가 바뀔 때.
> **Links:** [`STATUS.md`](STATUS.md) · [`DECISIONS.md`](DECISIONS.md) D-2026-10-10-B · [`PLAN_ML_SYST.md`](PLAN_ML_SYST.md) · [`PLAN_QCD_DD.md`](PLAN_QCD_DD.md) ·
> [`PLAN_v15_2018UL_2024.md`](PLAN_v15_2018UL_2024.md) §9 · [`AN_KR/`](AN_KR/) · NtupleForge `docs/01_STATUS.md` · 워크스페이스 `RUNBOOK_lxplus_2026-09-16.md`

## 결론 먼저

1. **지금의 임계 경로는 W1(2024 control plot, SF 적용)이다**: btagtrig 완료 → merge → b-tag 효율 map → TriggerStudy(2024 trigger SF) → **SF main 셋(FH, μCR,
   eCR)** → plot. 그 SF main 을 **STEP 27 의 실행 파일(Tree v1 포함)** 로 돌리면 control plot·DNN 입력의 Data/MC·첫 ML 학습 표본이 한 번에 나온다.
2. **병렬로 준비된 것(10-10):** Tree v1·BTV 규칙(STEP 27, 코드·시험 끝), 계통 weight 의 Tree 기록, plotter 의 DNN 입력 plot(`--tree-v1`), 제출기
   `--memory` 2 GB, 계획 문서 셋(ML·계통, QCD, 이 문서), 한국어 AN v0.1.
3. **결과까지 막는 외부 의존 셋:** (a) Run 2 v15 의 신호·`TT4b`·`TTZH`·`TTZZ`·`THW` 가 중앙에 없다(요청·enriched 생산 대기 — "Run 2 전체로 학습"과 Run 2
   SR 이 이것을 기다린다), (b) 2016·2017 v15 hadronic ntuple 생산 전, (c) 2024 의 W→ℓν·DY 표본(lepton CR 의 MC 합)과 tt+nb patch(2024 stitching).
4. **QCD data-driven(W7)은 lepton CR 의 Data/MC 가 합리적으로 나온 뒤**(D-2026-10-10-B (6)). 방법의 출발점은 ttH(bb) FH(AN-19-094 §8.1).

## 1. 목표와 범위

- **결과:** ttHH(HH→bb̄bb̄) FH 채널의 신호 세기 상한(과 HEFT 해석은 SL 처럼 나중), Run 2 전체(2016preVFP, 2016postVFP, 2017, 2018) + 2024. SL/DL(HIG-24-016)과의
  결합은 "채널들이 양립 가능하면"(Hbb 발표 2026-07-30 p.38). 문서 경로(같은 AN 의 보완 채널 vs 후속 결과)는 팀 확인 필요(두 발표의 표현이 다름 — AN_KR §1).
- **일정(사용자 KCMS 슬라이드 p.4, 2026-08):** 2026 하반기 full Run 2 확장 + QCD data-driven 방법 확립·검증; 2027 상반기 multiclass DNN, SR 최적화, 계통 → fit.
- **원칙:** AN-2022/122(Run 2 SL/DL)의 정의를 따르고 FH 와 Run 3 에 필요한 만큼만 바꾼다; 억지로 바꿀 바엔 Run 2 기준(D-2026-10-10-B (1)). FH 고유의
  것(QCD multijet, hadronic trigger)은 ttH(bb) FH(AN-19-094)를 선례로 쓴다.

## 2. 워크스트림

| W | 무엇 | 지금(2026-10-10) | 다음 | 막는 것 | 문서 |
|---|---|---|---|---|---|
| **W1** control plot | 2024 FH·μCR·eCR, trigger SF + b-tag SF(fixed WP) + stitching(2024 은 tt+nb 없이) → 2018 v15 → 2017 v9 와 비교 → full Run 2 | btagtrig 실행 중(KNU), SF 없는 plot 셋 있음(10-09) | btagtrig 완료 → map → TriggerStudy → STEP 27 빌드 → SF main 셋 → plot(`--tree-v1` 포함) | btagtrig 속도(request_memory, RUNBOOK §32) | STATUS, PLAN_v15 §9, RUNBOOK §31–33 |
| **W2** analyzer 연도 | Run 2 를 **v15 이름**으로 읽기(2018 v15 먼저, 2016·2017 은 생산 뒤): §3 의 목록 | analyzer 의 Run 2 는 v9 이름; 2018 v15 는 필수 branch 검사에서 FATAL(의도) | §3 의 A–K, Y3(v9↔v15 jet ID 대조) | 2016·2017 v15 ntuple(W3) | PLAN_v15 §4·§9, 이 문서 §3 |
| **W3** 표본(NtupleForge) | 2016·2017 v15 hadronic 생산, Run 2 부재 5 종(중앙 요청 + enriched 사설 병행), 2024 lep 표본(D13), 2024 tt+nb patch(V6), 2025 Data | 2018UL v15·2024 v15·ParkingHH 끝 | 사용자 결정: 2016·2017 생산 시작 시점, Phase 0(enriched) 착수 | KNU 저장공간(KCMS p.4), 중앙 요청 답장 | NtupleForge `01_STATUS.md` 표 1·15·19·21·22, `ttHH/04_mc_request_2026-09.md` |
| **W4** Tree·ML 입력 | Tree v1(STEP 27), 내보내기 도구(`tools/ml/`), GATJA 의 FH 입력 | Tree v1 코드·시험 끝 | SF main 으로 표본 생성, 내보내기 도구 | W1 의 SF main | PLAN_ML_SYST §5, STEP_27 |
| **W5** ML | FH DNN(AN SL DNN 구조 + QCD class), GATJA 의 FH 판, SWAN(기본)·KNU GPU | 설계(PLAN_ML_SYST §9) | 2024 표본으로 기준선 | W4, W7(QCD class 의 정의는 TR data) | PLAN_ML_SYST §9–10 |
| **W6** 계통·통계 | weight 변형(Tree v1), JES/JER 변형 실행, b-tag norm 변형별, MC 통계 band, datacard(combine) | weight 의 Tree 기록 끝, 목록(PLAN_ML_SYST §6) | plotter 의 MC 통계 band 확인, JES/JER sysType 경로 설계 | W1 | PLAN_ML_SYST §6, AN_KR §10–11 |
| **W7** QCD data-driven | ttH(bb) FH 방법(TR/CR/SR × m_qq, loose-jet 운동학 보정, 자유 정규화)의 ttHH 판 | 계획(PLAN_QCD_DD) | lepton CR 의 Data/MC 확인 → 영역 수율 표 → TF 유도 도구 | 선행 조건: lepton CR Data/MC(D-2026-10-10-B (6)); W1 | PLAN_QCD_DD |
| **W8** 한국어 AN | 작업과 함께 자라는 분석 노트(이론·변화·현재 상태) | v0.1(10-10) | 매 STEP 마다 해당 장 갱신 | — | `AN_KR/`, 맥 `AN_KR_pdf/` |

## 3. W2 — Run 2 를 v15 로 읽기 위한 목록 (2018 v15 먼저)

v9 → v15 의 물리 객체 변화는 연도와 무관하게 하나다(NtupleForge `08_branch_schema_migration.md` §3.1–3.5: MC 126 삭제/348 추가/86 형 변경이 2016·2017·2018 에서
같은 집합). 그래서 2018 v15 에서 한 일이 2016·2017 v15 에 그대로 간다. **값을 코드에 넣기 전에 출처(POG payload·twiki)로 확인**(NtupleForge 01_STATUS 23).

| # | 항목 | 내용 | 출처·확인 |
|---|---|---|---|
| A | 스키마 선택 | 연도와 별개인 "NanoAOD 판"(v9/v15) — yml 또는 첫 파일의 branch(예: `Rho_fixedGridRhoFastjetAll`)로. `EraConfig::isRun3` 가 이름 선택까지 맡고 있는 것을 나눈다 | 설계(AI) |
| B | 이름 | rho `Rho_fixedGridRhoFastjetAll`, MET `PFMET_pt`·`PFMET_phi`(N3), electron `Electron_mvaIso_WP90` | 08 §3.1(실측) |
| C | jet ID | `Jet_jetId` 없음 → UL CHS Tight + TightLepVeto 재계산(재료 `Jet_{ne,ch}{H,Em}EF`, `Jet_muEF`, multiplicity 셋 — UChar_t) | 08 §3.4(재료 실측); 기준값은 JME(기억 → 확인), **Y3**: 같은 MiniAOD 의 v9·v15 2018 파일을 짝지어 v9 `Jet_jetId` 와 비교 |
| D | PU jet ID | `Jet_puId` 없음 → `Jet_puIdDisc` 에 UL WP 임계값(η·pT bin, 연도별) | 08 §3.4; 임계값은 JME twiki(확인) |
| E | trigger | 2018: DeepCSV 경로 넷, 2018A 초기 run 은 다른 이름(D1); 2016: `HLT_PFHT450_SixJet40_BTagCSV_p056`, `HLT_PFHT400_SixJet30_DoubleBTagCSV_p056`, `HLT_PFJet450`(AN-19-094 Table 24) | NtupleForge 08 §7.4(경로 존재 실측), AN-19-094 p.31·37 |
| F | Data PD | 2018 JetHT 만(BTagCSV 없음); 2016 은 ttH FH 가 JetHT 만 씀 — BTagCSV 2016 을 쓸지·veto | NtupleForge 01_STATUS(2018 BTagCSV 부재), AN-19-094 p.31 |
| G | 보정 payload | JEC/JER(2016APV/2016/2018 UL), deepJet shape SF(UL 연도별), PU(LUM UL), L1 prefiring(2016·2017; v15 에 `L1PreFiringWeight_*` 있음) | jsonpog(KNU 에서 목록) |
| H | lumi·golden JSON | 2018 59.56(LUM TWiki; brilcalc 미실행 X14), 2016 두 half 는 LUM 권고값(확인), 2016 golden JSON 파일 | `reference/LUMI_SOURCES.md` |
| I | xsec_db·prescan·stitch | `samples_2016{preVFP,postVFP}*.json`, 2018 v15 이름, 연도별 prescan_summary 와 stitch factor | 22j(NtupleForge) |
| J | tt+nb lookup | 2018: 6 표본 patch 있음(V45), `TT4b`·신호는 enriched 뒤; 2016: 없음 | TTHHGenCategoryTools |
| K | trigger SF·b-tag norm | TriggerStudy 의 2018·2016 설정(기준 trigger: ttH FH 는 2016 IsoMu24, 2017·2018 IsoMu27 — 우리 2018 은 IsoMu24 로 적혀 있어 결정 필요), bTagSF_ReweightStudy 의 연도 | AN-19-094 p.30; `requireBranches_` |

## 4. 순서와 의존성

```
지금 ─ W1: btagtrig ─ merge ─ eff map ─ TriggerStudy ─ [STEP 27 빌드] ─ SF main(FH, μCR, eCR) ─ plot(--tree-v1)
            │                                                        │
            │                                                        ├─ W4: 내보내기 도구 → W5: 2024 DNN 기준선(QCD class = TR data, W7 의 정의)
            │                                                        ├─ W6: weight 변형 template, MC 통계 band
            │                                                        └─ lepton CR Data/MC 판정 ─ W7: QCD data-driven
            ├─ W2: 2018 v15 (A–K) ─ 2018 control plot ─ 2017 v9 와 연도 비교
            └─ W3: 2016·2017 v15 생산, Run 2 부재 5 종 ─ W2(2016·2017) ─ full Run 2 control plot ─ Run 2 학습
W8(AN_KR): 위의 각 단계가 끝날 때마다 해당 장
```

## 5. 지금 할 일 (2026-10-10 기준)

| 누구 | 무엇 | 명령 |
|---|---|---|
| 사용자(맥) | 10-10 문서 커밋(RUNBOOK §32 6) 뒤 STEP 27 + 계획 문서 커밋 | RUNBOOK §33 1 |
| 사용자(KNU) | btagtrig 상태 확인, 다 끝나면 pull·빌드·시험, SF main 셋은 STEP 27 실행 파일로 | RUNBOOK §32 1–5, §33 2–6 |
| AI | W2 의 A·B 설계와 코드(2018 v15), W7 의 영역 수율 도구, AN_KR 갱신 | — |
| 사용자 결정 | §6 의 OPEN | — |

## 6. 결정 대기 (OPEN; 권고는 AI)

1. **Tree v1 을 SF main 셋에 넣을지** — 권고: 예(시험 110/110; 아니면 ML 표본을 위해 main 을 한 번 더).
2. **DNN 입력에 b-tag 점수를 쓸지** — ttH FH 는 QCD template 을 낮은 b-tag 영역에서 가져오므로 b-tag 점수와의 직접 상관을 피했다(AN-19-094 §8.1.3,
   §10.1: TR·CR·SR 에서 모양이 같은 변수만). AN-2022/122 SL DNN 은 b-tag 점수를 많이 쓴다(d3…d6, BLR). FH 에서는 W7 의 방법과 맞아야 하므로 **"TR/CR/SR 모양
   동등성" 검사를 통과한 변수만**(권고) — PLAN_ML_SYST §9.
3. Run 2 학습 표본: 부재 5 종을 기다릴지, v9(2017 v20 은 KNU 에서 지워짐, D-2026-10-05-D)로 임시 학습할지 — 권고: 2024 로 먼저, Run 2 는 v15 신호 확보 뒤.
4. W2 K 의 2018 trigger SF 기준 경로(IsoMu24 vs IsoMu27).
5. 2016 Data 에 BTagCSV PD 를 쓸지(W2 F).
6. 문서 경로(같은 AN vs 별도 AN)·결합 계획 — 팀(Hbb 컨비너) 확인.
7. **ttbar stitching 의 정규화와 51/52 소유**(D-2026-10-10-C PROPOSED, AN_KR 2·12장): 지금의 r 에 p_own 이 빠져 stitched tt+bb 가 목표의 약 1/9
   — 고치는 식 r' = σ_inc·f/(σ_ded·p_own)과 closure 시험, 51/52 를 AN 처럼 4FS 로 할지. **stitching 을 넣은 control plot(W1) 전에** 정한다.
   권고: 둘 다 AN·ttH(bb) 대로.
8. veto muon 의 isolation 0.15(지금) vs 0.25(AN DL 부 lepton, ttH(bb) FH) — SL/DL 과의 직교성이 걸리므로 결합 전에(AN_KR 4장).
9. jet–lepton ΔR cleaning 을 넣을지 — lepton CR(W1·W7 의 선행 조건)에서만 영향(AN_KR 4·6장).
10. χ² 의 σ: 역사적 √(0.1 ΣpT) 를 둘지 JER 기반으로 바꿀지 — DNN 입력으로 쓰기 전(AN_KR 7장).

## 7. 위험

- QCD 가 SR 의 대부분(ttH FH 의 SR 에서 67–83 %, AN-19-094 Tables 83·89·95 로 계산)이라 결과는 QCD 추정의 질에 달려 있다. ttHH 는 b 다중도가 더 높아 CR→SR
  외삽 거리가 길다(PLAN_QCD_DD §5).
- FH 단독은 tt+B 정규화에 민감도가 약하다(ttH FH: 50 % lnN 으로 묶음) → SL/DL 과의 결합 또는 외부 제약 필요(AN-19-094 p.241).
- 높은 jet·b-tag 다중도에서 Data/MC 기울기(2017 과 2024 모두; STATUS 10-07, D-2026-10-07-B)가 QCD 만으로 설명되지 않을 수 있다.
- 저장공간(KNU Tier-3): full Run 2 + Run 3 생산과 Tree v1 의 커진 출력.
