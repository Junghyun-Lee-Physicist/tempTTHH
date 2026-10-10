# 계획: QCD multijet 배경의 data-driven 추정 (FH)

> **Purpose:** FH 의 지배적 배경인 QCD multijet 을 데이터에서 추정하는 방법을 정한다 — 선례(ttH(bb) FH, AN-19-094 §8.1)를 정확히 옮겨 적고, ttHH FH 로 옮길 때
> 바뀌는 것과 열린 것을 표시한다.
> **Audience:** QCD 추정을 구현·검토할 사람과 AI 세션.
> **Status:** **PROPOSED** (AI 2026-10-10; 사용자 방향 D-2026-10-10-B (6): "QCD data driven도 준비해야 한다. 단 이건 lepton selection을 해서 적절한 data/MC
> ratio가 나오면 하자"). §2 는 AN-19-094 v20 의 사실(쪽 표기: 인쇄 p / PDF p, PDF = 인쇄 + 2), §3 이후는 제안.
> **Links:** [`ROADMAP_FH.md`](ROADMAP_FH.md) W7 · [`PLAN_ML_SYST.md`](PLAN_ML_SYST.md) §9(DNN 의 QCD class) · [`PLAN_v15_2018UL_2024.md`](PLAN_v15_2018UL_2024.md) §9.5(QCD 비교의 정의) ·
> 맥 `Materials/TTHH/TTH_AN/AN2019_094_v20_ttHAnalysis.pdf`

## 결론 먼저

1. **방법:** ttH(bb) FH 의 방법을 출발점으로 한다 — b-tag 축(TR: 2M + ≥2L, CR: 3M + ≥1L, SR: ≥4M)과 m_qq 축(W 질량창 중앙 / 사이드밴드)의 직교 영역, CR 의
   (data − MC) 를 loose jet 의 운동학 보정(TF_loose: pT, η, ΔR_min)으로 SR 의 QCD 모양으로 옮김, **정규화는 fit 에서 자유**(연도 × 범주).
2. **선행 조건(사용자):** lepton CR(μCR, eCR — QCD 가 억제된 영역)에서 MC(tt+X 등)의 Data/MC 가 합리적일 것. CR·SR 에서 data 에서 빼는 것이 그 MC 이기 때문이다.
   지금(2024): trigger SF 와 b-tag SF 를 켠 CR 이 아직 없고 W→ℓν·DY 표본이 없다(D13) — 그 셋이 먼저.
3. **ttHH 에서 달라지는 것:** SR 의 b 다중도가 높아질 수 있고(신호의 b quark 6 개) 그만큼 CR → SR 외삽이 길어진다; SR 통계가 작아 자유 정규화의 제약이 약해진다;
   DNN 입력은 TR·CR·SR 에서 모양이 같은 것이어야 한다(b-tag 점수 회피, ttH FH 의 원칙).
4. **이미 된 것(STEP 27):** Tree 에 m_qq(`invMassHadW`), loose 다중도(`nLooseJets`), jet 마다 WP 칸(`jetBTagWP`)·pT·η·φ — 영역 정의와 TF 유도에 필요한 것이
   다 있다. 다음은 영역별 수율 표 도구와 TF 유도 도구.

## 1. 왜 data-driven 인가

- **QCD MC 의 한계(실측):** 2017 QCD-HT MC 의 event 당 weight 가 낮은 HT bin 에서 매우 크다(HT 200–300 ≈ 1.1×10³, 300–500 ≈ 2.5×10²; 사용자 Hbb 발표
  2026-07-30 의 표본표로 계산 — 발표 digest). reco HT > 500 을 통과한 낮은 HT bin event 하나가 높은 b·jet 다중도 영역에 스파이크를 만든다.
- **모양과 정규화:** 2017 과 2024 모두 jet 다중도에서 Data/MC 기울기(2024 HT1050 영역: 6 jet 1.38 → 13 jet 약 2; STATUS 10-07, D-2026-10-07-B).
- **선례:** ttH(bb) FH 는 QCD MC 를 초기 연구에만 썼다 — "형태·정규화 모두 모델링 불량", 2017/18 은 PS tune 차이로 jet 다중도 불일치(AN-19-094 p.66–67 / PDF 68–69).
  SR 에서 QCD 가 67–83 %(Tables 83·89·95 — 2016·2017·2018 — 의 사전 수율로 계산: 2016 79.7/75.1/66.8 %, 2017 82.7/79.3/74.3 %, 2018 81.7/77.5/72.0 %, 7j/8j/≥9j).

## 2. 선례: ttH(bb) FH 의 방법 (AN-19-094 v20 §8.1, 사실)

### 2.1 영역 (Table 63, p.99 / PDF 101; Fig. 50)

| b-tag (DeepJet) | m_qq 중앙 | m_qq 사이드밴드 |
|---|---|---|
| M = 2, L ≥ 4 (2M + ≥2L) | **TR**: 학습 표본(ANN 의 배경 class) | ValTR: 입력 변수 검증 |
| M = 3, L ≥ 4 (3M + ≥1L) | **CR**: SR 의 QCD 모양을 여기서 | ValCR: VR 의 모양 검증용 |
| M ≥ 4 | **SR**: 최종 분석 영역 | VR: 추정과 data 비교 |

- m_qq = W 질량에 가장 가까운 dijet 질량, **b-tag 영역 정의에 쓰인 jet 은 제외**(두 축이 구성상 무상관). 중앙창 60 < m_qq < 100 GeV(7·8 jet); 9 jet 은
  본문 70–92 GeV(p.99 / PDF 101, l.1162) vs Table 63/65 의 72–90 GeV — **AN 안에서 불일치**. 사이드밴드 30–60(72) 또는 100(90)–250 GeV. baseline 에 30 < m_qq < 250.
- 영역은 범주(7, 8, ≥9 jet)마다 따로. 범주는 SR 의 (7j, ≥4b), (8j, ≥4b), (≥9j, ≥4b) × 3 년(p.97–98 / PDF 99–100).

### 2.2 절차 (p.99–100 / PDF 101–102; Fig. 52)

1. CR 의 data 에서 tt+jets 와 기타 MC 를 뺀다 → CR 의 QCD.
2. CR 의 **loose b-tag jet** 에 운동학 보정 TF_loose 를 곱한다(§2.3).
3. 보정된 CR 의 QCD 모양 = SR 의 QCD 모양. 정규화의 시작값 = SR 의 (data − MC(배경 + 신호)); **fit 에서 자유**.
4. 같은 1–3 을 ValCR → VR 에 적용해 검증.

### 2.3 TF_loose (§8.1.1, p.101, 108 / PDF 103, 110; 식 13–16)

- baseline 뒤, **DeepJet 값 상위 두 jet 을 뺀** jet 에서 (M 통과 jet 수)/(L 통과·M 실패 jet 수) 를 pT–η–ΔR_min(처음 두 b-tag jet 까지의 최소 ΔR) 공간에서.
- 1 차원 함수를 차례로 맞춤(pT → η → ΔR_min):
  f(pT) = p0 + erf(p1(pT − p2))(p3 + p4 pT); g(η) = Σ_{i=0}^{16} p_i η^i; h(ΔR_min) = Σ_{i=0}^{6} p_i ΔR_min^i.
- CR 의 loose jet 에 f·g·h 를 곱한다(여러 loose jet 일 때의 결합은 **AN 에 없음**). 연도별로 유도. **data 에서 유도한 것은 data 에, tt+jets MC 에서 유도한 것은
  CR 의 모든 MC 에**. TR(학습 표본)에도 적용.
- 계통: 2 차 η 재보정 g2(η) = Σ_{i=0}^{4} p_i η^i 를 더한 것이 up, down = nominal(**단측**, 연도별 비상관, `CMS_ttHbb_FH_TFLoose_{year}`; 크기 수치는 AN 에 없음).

### 2.4 검증 (p.100–105 / PDF 102–107)

- **m_qq 사이드밴드 VR**: ValCR → VR 의 추정 + MC 를 VR data 와(Fig. 53; 입력 변수 App. E.2). 정규화는 구성상 맞으므로 **모양만** 검증.
- **MediumLoose(ML) 폐쇄 검정**: 3M 영역 안을 ML WP(2016 0.11185, 2017 0.101, 2018 0.093; [L, M) 를 tt MC 기준 둘로 나누는 값, Table 64)로 다시 나눠 CRx(ML = 3) →
  SRx(ML ≥ 4) 를 SR 외삽의 대리로(Table 65, Figs. 55–56). 같은 방식의 보정(L–ML 대 ML–M jet)을 따로 유도.
- 별도의 non-closure 불확도는 **넣지 않았다**(Table 79 에 없음).

### 2.5 fit 과 결과 (p.233–241 / PDF 235–243)

- QCD 정규화 9 개(`CMS_ttHbb_bgnorm_ddQCD_{year}_fh_j{7,8,9}_t4`), 사전 제약 없음. 사후: FH 단독 data fit 0.912–0.974(±1.3–3.5 %), 전체 결합 0.964–0.991.
  FH 단독 Asimov 에서 impact 상위 10 개 중 9 개가 이 정규화.
- ANN 은 범주마다 binary(ttH vs **TR 의 data**), 입력 19/21/21 개, **b-tag 값은 직접 쓰지 않음**(TR·CR·SR 에서 모양이 같은 변수만; §10.1, p.154 / PDF 156).
- FH 단독: 기대 μ = 1.00 +0.77/−0.78(1.3σ), 관측 −1.31 +0.82/−0.89; tt+B·tt+C 정규화는 FH 단독에서 50 % lnN(민감도 부족).

## 3. ttHH FH 로 옮기기 (PROPOSED)

### 3.1 SR 과 범주 — 열림

- 사용자 Hbb 발표(2026-07-30 p.14)의 계획: nb ≥ 4, njets 7/8/≥9, Higgs 질량창 100–140 GeV. ttHH 의 신호는 b quark 6 개(H 둘 + t 둘)라 nb ≥ 5 나 (nb = 4, nb ≥ 5)
  범주가 더 민감할 수 있다 — **SR 최적화(ROADMAP W5) 전에 QCD 방법이 그 선택을 받아들일 수 있게** 영역을 일반화한다:
  SR(nM ≥ N), CR(nM = N − 1, L ≥ N), TR(nM = N − 2, L ≥ N). N = 4 가 ttH 와 같은 기준선.
- jet 범주: ttH 는 ≥9j 가 가장 민감(S/√B 2.14 vs 7j 1.47, Fig. 503); ttHH 는 parton 10 개라 ≥9j·≥10j 쪽으로 — 그 범주의 TR·CR 통계 확인이 먼저.

### 3.2 m_qq 축 — 이미 Tree 에

- 우리 `invMassHadW`(STEP 27) = cut `30<HadW<250` 의 값: m_W(80.377)에 가장 가까운 dijet 질량을 **light jet(score < L)** 둘 이상이면 그 중에서. M·L jet 이 빠지므로
  "b-tag 영역 정의에 쓰인 jet 은 제외"(ttH)를 만족한다. light jet 이 둘 미만이면 모든 jet 에서 — 그때는 b-tag 정의와 상관이 생길 수 있으니 그 event 의 비율을 센다(§4).
- 창: 60–100 GeV 로 시작(9 jet 의 AN 불일치는 우리 범주가 정해진 뒤 다시).

### 3.3 TF 와 template

- TF_loose 를 그대로(연도별; Run 2 DeepJet, 2024 UParTAK4 의 L·M). 여러 L–M jet 의 결합(AN 에 없음)은 **jet 마다의 곱**으로 시작하고 ML 폐쇄 검정으로 판정.
- template = CR 의 (data × TF) − (MC × TF_tt) 의 모양, 정규화 자유. MC 에서 신호를 뺄지(AN 에 없음)는 신호 오염을 재서 정한다(ttHH 는 작을 것).
- **2024 의 b-tag 보정:** CR·SR 의 MC 는 fixed-WP weight(D-2026-10-08-A, -10-10-A) — TR·CR 의 MC 효율 map 이 그 영역에 맞는지(true-b closure, STEP 27 N)가 차감의
  질을 정한다.

### 3.4 검증과 불확도

- VR(m_qq 사이드밴드)과 ML 폐쇄 검정을 그대로. Run 3 tagger 에도 [L, M) 를 tt MC 기준 둘로 나누는 ML WP 를 새로 정한다.
- **non-closure 를 shape 불확도로 넣는 것을 권고**(ttH 는 넣지 않음): b 다중도가 높을수록 외삽이 길어 리뷰에서 물을 것.
- TF_loose 단측 계통(2 차 η 보정), 차감한 MC 의 불확도 전파(AN 에 없음 → 우리는 MC 변형으로 template 을 다시 만들어 전파), template 통계(CR data 의 통계; autoMCStats
  가 데이터 template 을 덮는지 확인).

### 3.5 DNN 의 QCD class

- ttH FH 처럼 **TR 의 data(TF 적용)** 를 QCD class 로(QCD MC 는 SR 통계·모델링 불량). AN-2022/122 의 multi-class 구조(SL 9 node)에 QCD node 를 더하는 형태
  — PLAN_ML_SYST §9. 입력은 TR·CR·SR 모양 동등성 검사(ttH §10.1)를 통과한 것만.

## 4. 구현 순서 (PROPOSED)

1. **선행:** trigger SF·b-tag SF 를 켠 2024 μCR·eCR 의 Data/MC(SF main 셋) — 사용자 판정. W→ℓν·DY 없이 모양 위주(D-2026-10-07-B), 생산은 D13.
2. `tools/qcd/region_yields.py`(새): Tree 에서 (nM, nLooseJets, invMassHadW 중앙/사이드밴드, nJets 범주)별 Data·MC(공정별) 수율과 Σw² — TR/CR/SR/ValTR/ValCR/VR 표,
   QCD 비율(MC 기준 참고), light jet 두 개 미만 event 의 비율.
3. `tools/qcd/derive_tf_loose.py`(새): baseline 의 jet 에서(상위 두 b-tag jet 제외) M / (L−M) 비의 pT, η, ΔR_min 순차 맞춤, data·tt MC 따로, 연도별 → JSON(correctionlib).
4. template 도구: CR(data × TF − MC × TF_tt) → SR 모양; VR 비교 plot; ML 폐쇄 검정.
5. datacard 의 QCD 과정(자유 rateParam, TF 계통, non-closure shape).

## 5. ttHH 특유의 위험

- **외삽 길이:** SR = ≥5M 이면 CR(4M + ≥1L 또는 3M + ≥2L)에서 승격되는 jet 이 많아지고 TF 의 곱과 2 차 효과가 쌓인다 — 승격 jet 수별 폐쇄 검정.
- **SR 통계:** 자유 정규화가 data 로 덜 고정된다 — 정규화 파라미터의 공유(연도·범주)나 CR 동시 fit 을 검토.
- **tt+bb:** FH 단독은 tt+B 정규화에 약하다(ttH: 50 % lnN) — SL/DL 결합 또는 외부 제약.
- **data 만의 결함을 template 이 흡수**(ttH 의 MEM 버그 사례, p.65 / PDF 67) — 입력 변수의 MC–data 를 따로 점검.

## 확인하지 못한 것

- AN-19-094 의 9 jet 중앙창(70–92 vs 72–90), 여러 loose jet 의 TF 결합, CR 차감에 신호 포함 여부, TF 계통의 크기 — AN 에 없거나 모호(digest §10).
- ttHH FH 에서 영역별 통계 — 첫 SF main 뒤 §4 2 의 표로.
