# ML(DNN·GATJA)과 계통 불확도 — 받은 자료의 정리, 우리 출력의 빈칸, 순서

> **Purpose:** FH 분석은 ML 두 개(b jet 배정 GATJA, 다중 분류 DNN)와 계통 불확도까지 가야 한다. 이 문서는 받은 자료(AN-2022/122 v26, GATJA 발표·코드,
> SWAN 의 FH·DL DNN 코드와 표본)가 무엇을 하는지 정리하고, 지금의 `Tree/Tree` 와 비교해 빈칸과 채우는 순서를 정한다.
> **Audience:** 이 분석을 이어받는 사람과 AI 세션(대화 기억 없음).
> **Status:** **PROPOSED** · 2026-10-10 (2) (사용자 방향 D-2026-10-10-B 반영: §5.2 Tree v1 은 STEP 27 로 **구현·시험 끝**, §8 의 결정 갱신, §9 FH DNN·GATJA 의
> AN 기반 설계, §10 학습 데이터·환경 추가) · 2026-10-10 (AI 정리, 독립 대조 한 번 반영). §1–4 는 자료를 읽은 사실, §5–10 은 제안(표시가 있으면 결정).
> **Links:** [`DECISIONS.md`](DECISIONS.md) D-2026-10-08-A(2024 b-tag), D-2026-10-10-A(BTV cms-talk 검토) · [`STATUS.md`](STATUS.md) ·
> [`changes/STEP_25_btag_fixedWP_2024.md`](changes/STEP_25_btag_fixedWP_2024.md) · 맥 워크스페이스 `GATJA/`, `SWAN_projects/`, `Materials/TTHH/`.

## Bottom line

1. **지금 `Tree/Tree`(60 branch)로는 FH DNN 도 GATJA 도 돌릴 수 없다.** 2024-06 의 FHSample 을 만든 옛 analyzer 에는 DNN 변수 branch 가 있었지만
   (2026-06-11 백업: `_inputTree->Branch` 239 줄 중 203 줄이 이미 주석) 지금 코드에서는 지워졌다. 계산 helper(ΔR·Δη 통계, Fox-Wolfram, centrality,
   maxPT 질량, light·b jet HT)와 HH χ² 계산은 남아 있으나 Tree 에 쓰지 않는다. jet φ·질량, HH χ², W 후보 질량, jet 과 quark 의 gen 매칭도 Tree 에 없다.
2. **[10-10 (2) 구현 끝: STEP 27 O, [`changes/STEP_27_treev1_btv_rule.md`](changes/STEP_27_treev1_btv_rule.md)]** **다음 analyzer 빌드(btagtrig 뒤, SF 를 켠 main 셋 앞)에 "Tree v1"(§5.2)을 함께 넣자(PROPOSED).** jet φ·질량 vector, gen 매칭 vector(MC),
   HH χ²·W 후보, DL 식 event 변수 ~55 개, PU·L1·scale·PS weight 변형, Run 2 b-tag shape 변형 weight. 그러면 SF main 셋 하나가 control plot
   (DNN 입력의 Data/MC 포함)과 첫 ML 학습 표본을 겸한다. 사용자 FH DNN(V3)의 입력 112 개는 Tree v1 과 내보내기 도구로 모두 덮인다(부록 A).
   되살리는 helper 에는 고칠 것이 있다: ΔR 의 Δφ 를 [0, π] 로 접지 않는다(옛 FHSample 의 ΔR 계열도 같은 코드) — §5.2.
3. **이름 함정(§3).** 옛 FH ntuple·옛 DL template 은 모든 branch 이름 앞에 `b` 를 붙였다(`bjetPT1` = **첫 jet**, `bbjetPT1` = 첫 b jet,
   `baplanarity`·`bH0` = jet 전체). DL 새 이름에서는 `bjetPT1` = **첫 b jet**, `baplanarity`·`bH0` = **b jet** 의 양이다. 섞으면 조용히 틀린 입력이 된다.
   새 branch 는 DL 새 이름을 따르되 맨 `b` 접두 대신 `bjet` 접두(지금의 `bjetAplanarity` 처럼)를 쓰자(PROPOSED).
4. **순서(§7):** (A) control plot 완성 + Tree v1(지금) → (B) ML 내보내기 도구와 FH DNN 기준선 → (C) GATJA 의 FH 판 → (D) 계통(weight 변형 →
   JES/JER 재실행 → datacard). FH 고유의 QCD multijet data-driven 추정은 별도 계획. 2024 의 b-tag 입력은 D-2026-10-10-A 의 WP 칸 방식.

---

## 1. 자료와 위치

| 자료 | 위치(맥 `~/claude/NtuplizerDev/`) | 내용 |
|---|---|---|
| AN-2022/122 **v26**(2025-11-27), 214 쪽 | `Materials/TTHH/TTHH_AN/ttHH_AN_AN2022_122_v26.pdf` | Run 2 SL·DL. FH 는 "별도 AN, 진행 중"(§1 l.58–59). 변수 §6.2(Table 43), JABDT §6.2.6, GATJA §6.2.7(Fig 36–43, Table 48), SL DNN §7.1(Table 49), DL DNN §7.3(Fig 87–91, Table 51), 계통 §8.2(Table 53), SL 변수 정의 부록 B |
| GATJA 발표(ML group, 2025-12-10, Ö. Sahin, 19 쪽) | `GATJA/GATJA_MLG.pdf`, `GATJA_presentation_indico_link.txt`(indico 1618963) | 동기, 구조, 학습, 성능, ttHH 적용 |
| GATJA 발표 녹화 | `GATJA/MLG_10-12-25_1.mp4`(117 MB) | **보지 않음**(PDF·AN 으로 대신) |
| gatja-studio | `GATJA/gatja-studio/`(`gitlab.cern.ch/gatja/gatja-studio` 의 clone, `a4651a1`) | GATJA 노트북을 Python package 로 옮긴 것(§2.2) |
| SWAN FHCode | `SWAN_projects/FHCode/` | 사용자의 FH DNN 노트북 다섯 판(2024-06~09), 학습된 모델 `bestAF.hdf5`, 결과 그림(§2.4) |
| SWAN gamzeCode | `SWAN_projects/gamzeCode/` | Gamze 의 DL DNN template·노트북, `snowttHHtools.py`, 모델(§2.5) |
| SWAN gamzeSample | `SWAN_projects/gamzeSample/`(12 파일, 348 MB) | DL ntuple(2024-05~06; ROOT key 시각 05-24, 파일 시각 06-04) — DL 새 이름의 실물(§2.5; 일부 branch 는 값이 깨져 있음) |
| SWAN FHSample | EOS `/eos/user/j/junghyun/SWAN_projects/FHSample` 에만(3.5 GiB, 13 파일) | 옛 tempTTHH 의 FH 출력(2024-06-26). **받지 않음**(사용자: 새로 만들 것). 파일 목록 §2.4 |
| 참고 발표 | `Materials/TTHH/` | SL approval talk(2026-07-19), Wei(2026-07-20), pre-approval(Higgs group, 2025-04-15), 우리 Hbb meeting 덱(2026-07-30 발표; 표지 날짜는 16 July) |

## 2. 각 ML 의 목표·구성·내용

### 2.1 DL 다중 분류 DNN (AN §7.3)

- **목표:** DL(2ℓ) event 를 process 별 node 로 분류. event 는 점수가 가장 높은 node 로 가고, 그 점수 분포가 fit 의 판별자.
- **출력:** Fig 87 의 그림은 7 node(ttHH, ttZH→4b, ttZZ→4b, ttZ→bb, ttH→bb, tt4b, tt). 본문(l.1145–1146)은 tt(SL)·tt(DL)·tt4b 를 tt 하나로 합쳐
  6 node 이고, 결과 그림(Fig 89·91: tt4b 없는 tt; Fig 90: tt4b 를 넣은 tt)과 Fig 88 의 범례도 6 class 다.
- **입력:** Table 43 + Fox-Wolfram(§6.2.4, Table 43 밖) + jet 별 b-tag 판별값(Fig 88 의 `jetBTagDisc1/3/4/5`, Table 43 밖) + GATJA 출력 24 개.
  Table 43 의 묶음 — 다중도(N_jets, N_bjets, N_light); jet·b jet 1–6 의 pT, |η|; lepton 1·2 의 pT, |η|; HT, HT^b, HT^light, HT^lepton, ST;
  평균 질량 m_j^avg, m_b^avg, m_light^avg, (m²)_b^avg; m_jjj^maxpT, m_jbb^maxpT, m_μμ, m_ee; Δη_jj^avg, Δη_bb^avg, Δη_bb^max; ΔR_jj^avg, ΔR_bb^avg,
  ΔR_jj^min, ΔR_bb^min, ΔR^min 쌍의 질량·pT(jj, bb); χ²_HH, χ²_ZZ, χ²_ZH 와 후보 질량(m_H1, m_H2, m_Z1, m_Z2, m_ZH,Z, m_ZH,H), pT(H1), pT(H2);
  aplanarity, centrality, sphericity, transverse sphericity, C, D(jet·b jet). 본문은 Fox-Wolfram·event shape·centrality 를 "DL 만" 이라 하지만
  (l.787, l.808) SL 상위 50(Table 49)에도 jet aplanarity(4 위)와 H1(8 위)이 있다 — AN 이 서로 맞지 않는다.
- **χ²(§6.2.1):** χ²_XY = (m_j1j2 − m_X)²/σ² + (m_j3j4 − m_Y)²/σ²(σ 는 JER 에서). b jet 이 4 개면 그 4 개, 5 개 이상이면 χ² 가 가장 작은 4 개,
  **M b jet 3 개 + loose b jet 이 있으면 넷째는 loose b jet 에서, loose 도 없으면 light jet 에서**.
- **학습:** QuantileTransformer → [0,1] scaling; 4 hidden(512, 256, 128, 64), dropout 0.2, batch 1024, LAMB + cosine decay, ReLU, softmax,
  categorical cross-entropy, validation 20 %(Table 51).
- **중요 변수 상위 20(Fig 88, SHAP):** bjetNumber, minDeltaRMassbb, gatja_0, ST, gatja_3, averageDeltaEtabb, jetBTagDisc5, averageDeltaEtajj,
  jetBTagDisc1, leptonHT, gatja_5, jetBTagDisc4, lightjetHT, jetBTagDisc3, lightjetNumber, gatja_2, chi2Higgs, gatja_6, bjetPT3, bjetAverageMassSqr —
  b-tag 판별값과 GATJA 출력이 많다.
- **결과:** GATJA 출력을 넣으면 Z_A 가 27 % 좋아진다(2017, Fig 89).

### 2.2 GATJA — Graph Attention based Jet Assignment (AN §6.2.7, 발표, gatja-studio)

- **목표:** b-tag 된 jet **하나하나**를 Higgs / top / 기타(Z 등)로 분류하는 object 기반 배정. 조합을 다 도는 event 기반 방법(JABDT, SPA-NET)과 달리
  jet 단위라 다른 final state 로 옮기기 쉽다(발표: HLT 에도 넣을 수 있다).
- **graph:** node = 그 b jet. 이웃 N1, N2 = 그 jet 과 짝지었을 때 χ²(m_H) 가 가장 낮은 b jet 과 두 번째로 낮은 b jet(`bjetMinChiHiggsIndex`,
  `bjetSecMinChiHiggsIndex`). 근거(발표 7 쪽): 혼잡한 final state 에서는 맞는 짝이 최소 χ² 가 아니라 두 번째인 경우가 많다.
- **입력(AN Fig 38, `graphs.py::_main_columns`):** node 5 개(pT, η, φ, 자기의 `bjetMinChiHiggsIndex`, b-tag 판별값; `stage_one_index_node = 5`),
  이웃마다 4 개(pT, η, φ, b-tag 판별값), event 변수: jetAverageMass, bjetAverageMassSqr, jetHT, bjetHT, lightjetHT, jetNumber, bjetNumber,
  lepton 1·2 의 pT·η·φ, met·metPhi, ΔR·Δη 통계 15 개(averageDeltaEtabb … minDeltaRpTbj), maxPTmassjjj·jbb. b jet 최대 8 개.
- **출력:** b jet 마다 (H, top, 기타) 확률 3 개 → 8 × 3 = 24 개 `gatja_0 … gatja_23`(Table 48: 0,3,…,21 = H; 1,4,…,22 = top; 2,5,…,23 = 기타).
  선택에는 쓰지 않고 DL DNN 의 입력으로만 쓴다.
- **label:** gen 매칭 — `bjetHiggsMatched_i`, `bjetTopMatched_i`, 둘 다 아니면 기타. ttZZ 는 gen 문제로 Higgs 매칭을 0 으로 둔다
  (`data.py::zero_ttzz_higgs_matches`).
- **구조:** `make_model_gnn`(pipeline 이 쓰는 것) — node·이웃을 각각 dense encoder(256→128, skip) → node 와의 행렬곱 softmax 로 attention weight →
  node 와 이웃 max 를 합치고 event 변수 encoder 와 결합 → skip connection 이 있는 dense 8 층(2048 ×4, 1024, 512, 128, 32, dropout 0.15) → softmax 3.
  `make_model_gnn_adv`(token 3 개에 multi-head self-attention, 출력 4)도 있으나 pipeline 은 쓰지 않고 label 은 3 열이다. 발표 8 쪽의 두 head
  (H/top/기타, N1/N2/기타)와는 아직 대응되지 않는다.
- **학습(stage 1):** 표본 ttHH, ttZZ, ttH(SL·DL), tt(SL·DL), ttbb(tt4b 는 읽지만 stage 1 frame 에 넣지 않음); sample weight = `btagweight`;
  Robust → Quantile → MinMax scaler; LAMB, cosine decay + warmup, early stopping(patience 100), 상한 1000 epoch; jet 1,900 만 개 이상(AN l.983).
  5M parameter 가 A100 한 장에서 30 분, 50M 판은 12 시간에 성능 향상(발표 10 쪽).
- **stage 2(검증):** GATJA 출력 24 개만으로 ttHH 대 ttZH+ttZZ 이진 분류 — 크게 갈린다(발표 16–17 쪽, AN Fig 43). GATJA 가 위상을 배웠다는 확인.
  배포된 `config.py` 에는 ttZH 경로가 비어 있어, 경로를 채우기 전에는 stage 2 에 ttZH 가 없다.
- **코드 상태(gatja-studio):** `pip install -e .` 뒤 `gatja-studio stage1 --years 2016`. Python ≥ 3.10, TensorFlow + tensorflow-addons +
  tf-models-official + uproot. `compat.py` 가 저장소 **바깥**의 `src/snowttHHtools.py` 를 import 한다(SWAN 사본 있음). 표본 경로
  `data/ntuples_*_allSys/all_*.root`(DL 의 allSys ntuple — 우리에게 없음), 학습된 weight 파일도 없음. 사소한 것: `_main_columns` 에
  `jetAverageMass` 가 두 번 들어 있다(아마 하나는 `bjetAverageMass`).
- **FH 로 옮길 때:** node·이웃·label 의 정의는 채널과 무관하다. event 변수의 lepton·MET 은 FH 에서 의미가 없으니 빼거나 FH 변수(W 후보 질량,
  light jet HT·수)로 바꾸고 stage 1 을 FH 표본으로 다시 학습해야 한다(GPU). label 에는 우리 출력의 gen 매칭이 필요하다(§5).

### 2.3 SL DNN 과 JABDT (AN §7.1, §6.2.6) — 참고

- **SL DNN:** 9 node(ttHH, ttH, ttZ, ttZH, ttZZ, tt+lf, tt+cc, tt+mb, tt+nb). 변수 333 개에서 첫 층 weight 합으로 상위 50 개(Table 49; 1 위 JABDT ttHH
  BDT 점수, BLR·BLR transformed 도 상위). 홀수 event 로 학습, 짝수로 평가; 55/25/20 분할; 4 hidden(512, 256, 128, 64), Adam 2×10⁻⁵, dropout 0.2,
  Z_A 를 epoch 마다 감시.
- **JABDT:** 6 jet 을 b quark 6 개(t 둘, H 넷)에 gen 매칭(ΔR < 0.4)한 순열을 맞는 쪽으로, 나머지를 틀린 쪽으로 BDT(TMVA, 300 tree, depth 4) 학습;
  event 기반이라 순열이 많다(Table 46: 맞는 순열 대 틀린 순열 ≈ 1 : 2,500–2,800).
- **BLR(§6.2.5):** jet 별 b-tag 판별값의 flavour 별 pdf(pT, η 의존)로 "6 b" 대 "4 b" 가설의 likelihood 비.
- **우리 FH 계획(Hbb meeting 덱: "Analysis strategy" PDF 9 쪽, backup "DNN input variables" PDF 40 쪽):** SL 과 같은 9 node, 입력은 jet·b jet
  pT·η, n_jets, n_b, HT, b-tag 판별값, ΔR_jj·ΔR_bb(avg·min), χ²(HH/ZH/ZZ), event shape, BLR, jet 배정.

### 2.4 사용자의 FH DNN (SWAN FHCode)

- **노트북:** `240626`·`240702`(FHSample 의 FH, 입력 202 개 — 옛 이름의 jet b-tag 판별값 `bjetBTagDisc1…` 포함; 240702 는 pandas `append` 를
  `concat` 으로 바꾼 것), `240710`(FH 입력 174 개), `240710_V2`(136), **`240807_V3`(112; 마지막 수정 2024-09-11, 그날의 `bestAF.hdf5`·그림과 짝)**.
  셀 하나의 `Channel = "FH"|"DL"` 로 두 채널을 같이 다룬다.
- **표본(FHSample, 2024-06-26, 옛 tempTTHH 의 `Tree/Tree`):** QCD 10 MB, tt4b 204 MB, ttFH 68 MB, ttHH 976 MB, ttHtobb 144 MB, tttt 624 MB,
  tttW 9 MB, ttWH 7 MB, ttWW 1.6 MB, ttWZ 1.9 MB, ttZHto4b 919 MB, ttZtobb 109 MB, ttZZto4b 634 MB.
- **V3:** 13 class(ttHH, ttFH, ttHtobb, ttWW, ttZHto4b, ttZtobb, QCD, tttW, ttZZto4b, tt4b, tttt, ttWH, ttWZ). 입력 112 개(옛 FH 이름): jet 1–12 의
  pT·η, b jet 1–8 의 pT·η·φ, light jet 1–6 의 pT·η, 다중도 셋, HT 셋, 평균 질량·maxPT 질량, ΔR·Δη 통계 18 개, χ²(HH, ZZ, ZH)와 후보 질량·pT,
  event shape(jet·b jet)·centrality. **b-tag 판별값은 아예 없고**, Fox-Wolfram·lepton·GATJA 변수는 주석으로 빠져 있다.
- **학습:** 표본마다 fraction 으로 덜어 개수를 맞춤(ttHH 0.0119, ttFH 0.178, ttHtobb 0.079, ttZHto4b 0.0124, ttZtobb 0.101, ttZZto4b 0.017,
  tt4b 0.054, tttt 0.0185, 나머지 1.0); event weight 는 쓰지 않고 class weight 는 균형식(total / (N_class × count)); Robust → Standard → MinMax;
  모델 `disc1D(523, 3)` = Dense 523(LeakyReLU, dropout 0.1) → 523 → 261 → 130 → softmax 13(`bestAF.hdf5` 에서 입력 112, 출력 13 확인);
  Adam 3×10⁻⁶, batch 512, 150 epoch, ReduceLROnPlateau, 최선 checkpoint 저장.
- **산출물:** 혼동 행렬, loss·accuracy 곡선, 입력 weight 그림. `FHCode/ttHH.root` 는 DL class(ttHH … ttSL)의 출력 히스토그램이라 FH 결과가 아니다.

### 2.5 Gamze 의 DL DNN 코드(gamzeCode)와 DL ntuple(gamzeSample)

- **`ttHH_DNN_template.py/.ipynb`:** DL 7 class, 옛 `b` 이름 ntuple(`/eos/cms/store/user/gsokmen/UL/...`), 표본마다 절반으로 학습, Robust →
  Standard → MinMax, `disc1D(523, 3)`, class weight, 출력 히스토그램을 ROOT 로(`ttHH.root` 등).
- **새 노트북(`0612Version2`, `backupFor240619`, `code_v2`, `ttHH_DNN_jhlee`):** DL 새 이름. `bestAF.hdf5` 는 입력 148, 출력 7. **그 148 개에는
  `weight` 와 gen 매칭 label(`bjetHiggsMatched1–8`, `bjetHiggsMatcheddR6–8`), χ² 이웃 index 가 입력으로 들어 있다** — data 에는 없는 진실 정보라
  이 모델은 시험용일 뿐 쓸 수 없다(V3 노트북의 `DL_features` 148 개도 같다).
- **`snowttHHtools.py`(FHCode 와 같은 파일):** `disc1D`, `extractEvents`(uproot → pandas, −99 → −5), `evaluatettHHDNN`(보조 모델 ttXX 의 출력을 입력에
  붙이는 2 단 DNN). GATJA 의 `compat.py` 가 이것을 import 한다.
- **gamzeSample(DL ntuple):** `Tree/Tree` 233 branch(Float16) — jet·b jet 1–8 의 pT·η·φ·BTagDisc·HadFlav, light jet 1–3, met·metPhi, ΔR·Δη 통계,
  Fox-Wolfram(`H`, `bH`, `R`, `bR`), 평균 질량·HT·다중도, χ² 와 후보 질량·pT, centrality, event shape(jet·b jet), `weight`, lepton(pT·η·φ·charge,
  μ·e 따로, 쌍의 질량·pT·η·φ), leptonHT·ST, **GATJA 용** `bjetHiggsMatched`, `bjetTopMatched`, `bjetHiggsMatcheddR`, `bjetMinChiHiggsIndex`,
  `bjetSecMinChiHiggsIndex`(1–8). `btagweight` 는 없다(지금 GATJA 코드는 그것을 요구 → 더 새 allSys ntuple 이 따로 있다).
  **값이 깨진 branch:** 확인한 두 파일(`all_ttbbDL`, `all_ttbbSL`)에서 ΔR·Δη 통계 18 개, maxPTmass 둘, Fox-Wolfram 전부, centrality 둘, metPhi 가
  상수·0·10³⁰ 같은 쓰레기 값이다(채우지 않은 변수로 보임). kinematics·χ²·event shape·met 은 정상. 그래서 이 파일은 그 변수들의 정의 기준이 될 수 없다.

## 3. 이름 규칙 — 함정

| 뜻 | 옛 FH(FHSample, V3)·옛 DL template | DL 새 이름(gamzeSample, GATJA) | 지금 우리 `Tree/Tree` | 제안(새 branch) |
|---|---|---|---|---|
| jet i 의 pT | `bjetPT{i}` | `jetPT{i}` | `jetPt[i]`(vector) | vector 그대로 |
| b jet i 의 pT | `bbjetPT{i}` | `bjetPT{i}` | 없음(`bTagScore ≥ M` 인 `jetPt`) | 내보내기 도구가 DL 새 이름으로 |
| light jet i 의 pT | `blightjetPT{i}` | `lightjetPT{i}` | 없음(`bTagScore < L`) | 같음 |
| jet 수 / b jet 수 | `bjetNumber` / `bbjetNumber` | `jetNumber` / `bjetNumber` | `nJets` / `nbJets` | 그대로 |
| jet·b jet 평균 질량 | `bjetAverageMass` / `bbJetAverageMass` | `jetAverageMass` / `bjetAverageMass` | 없음 | DL 새 이름 |
| aplanarity(jet / b jet) | `baplanarity` / `bbaplanarity` | `aplanarity` / `baplanarity` | `aplanarity` / `bjetAplanarity` | 그대로 |
| Fox-Wolfram H0(jet / b jet) | `bH0` / `bbH0` | `H0` / `bH0` | 없음 | `H0` / `bjetH0` |
| χ²_HH | `bchi2Higgs` | `chi2Higgs` | 없음(멤버 `_minChi2Higgs` 만) | `chi2Higgs` |
| MET | `bmet` | `met` | `MET_pt` | 그대로(+ `metPhi`) |

옛 규칙의 맨 앞 `b` 는 b jet 과 상관없는 branch 접두였다(멤버 이름과 겹치지 않게 붙인 것으로 보임). 그래서 `bjetPT1`, `bjetAverageMass`,
`baplanarity`, `bH0` 가 두 규칙에서 서로 다른 것을 뜻한다. **제안:** 도구들이 이미 쓰는 우리 이름(`nJets`, `HT`, `jetPt` …)은 그대로 두고, 새 branch 는
DL 새 이름을 따르되 DL 새 이름이 맨 `b` 하나만 붙인 b jet 양(`baplanarity`, `bH0`, `bR1` …)은 우리 Tree 에서 `bjet` 접두(`bjetAplanarity` 처럼)로
쓴다. DL 새 이름의 평평한 열은 ML 내보내기 도구가 만들고, V3 노트북을 새 표본에 쓸 때는 부록 A 의 대응표로 바꾼다.

## 4. 지금 우리 출력 — `Tree/Tree`(2024 main, 커밋 `fc461713`)

selection 전체를 통과한 event 만 들어간다(`process()` 에서 `selectObjects` 뒤 `fillTree`). 60 branch:

- trigger·flag: `passTrigger_HLT_IsoMu27`, `_IsoMu24`(2017 외), `_PFHT1050`, `_6J1T_B/_CDEF`, `_6J2T_B/_CDEF`, `_4J3T_B/_CDEF`, `passHadTrig`,
  `passMETFilters`, `failGoldenJson`
- 다중도·kinematics: `nMuons`, `nElecs`, `nVetoLeptons`, `nJets`, `nbJets`, `HT`, `MET_pt`
- 선택 jet vector(pT 순): `jetPt`, `jetEta`, `bTagScore`, `jetPUids`, `hadFlavs`, `partonFlavs`
- event 번호: `eventNumber`, `runNumber`
- weight: `evtWeight`, `evtWeight_btagSF`, `evtWeight_full`, `SampleWeight`, `PUWeight`, `L1PrefiringWeight`, `genWeight`, `stitchWeight`,
  `bTagWeight`(+ `_up`/`_down` 은 2024 fixed WP 만), `triggerSF`/`_up`/`_down`, `btagNormReweight`
- ttbar: `genTtbarId`, `expandedTtbarId`
- event shape(STEP 5): `aplanarity`, `sphericity`, `transSphericity`, `eventC`, `eventD` 와 b jet 판 다섯
- χ²(STEP 6): `chi2ZH`, `mZcandZH`, `mHcandZH`, `chi2ZZ`, `mZ1ZZ`, `mZ2ZZ` — **HH 는 없다**

코드에 남아 있는 것:

- `event` 의 helper `getStats`·`getStatsComb`(ΔR·Δη·Δφ 통계, 최소 ΔR 쌍의 질량·pT), `getFoxWolfram`, `getCentrality(V2)`, `getMaxPTSame/Comb`,
  `getnLightJet`, `getSumSelLightJetScalarpT`, `getSumSelbJetScalarpT`.
- HH χ²(`HiggsReconstructor`, **b jet ≥ 4 일 때만**): m(H1)·m(H2)는 cut-step 히스토그램(`_cutStepHiggsMass01/02`)에, χ²_HH 와 pT(H) 는 멤버에만.
  W 후보 질량(`closestMassPair`)은 cut 과 cut-step 히스토그램(`_cutStepHadWMass*`)에만.
- gen: `createObjects` 가 hard-process b quark(`statusFlags` bit 8)를 H·t 조상 표시(`hasHiggsMother`, `hasTopMother`)와 함께 `_selectGenParts` 에
  모은다 — 그 뒤를 쓰는 곳은 없고, jet 과의 매칭과 W·Z 의 딸 quark 는 없다. `objectJet` 의 `matchedtoHiggs`·`matchedtoTop`·`minChiHiggsIndex`
  필드는 값을 넣는 코드가 없다.
- eventBuffer 에는 Jet φ·질량, GenPart(pdgId, mother, statusFlags), GenJet, `LHEScaleWeight`, `LHEPdfWeight`, `PSWeight`, `L1PreFiringWeight_Up/Dn`,
  `Pileup_nTrueInt` 가 있다.

## 5. 빈칸과 채우는 방법

### 5.1 표

| 필요한 것 | 쓰는 곳 | 지금 | 채우는 방법 | 비용 |
|---|---|---|---|---|
| jet φ·질량(선택 jet) | ΔR·질량 변수의 재계산, GATJA 입력 | 없음 | analyzer: vector 2 | 작음 |
| HH χ², m(H1), m(H2), pT(H1), pT(H2) | DNN 상위 변수, SR 의 Higgs 질량 창 | χ²·pT 는 멤버, 질량은 cut-step 히스토그램 | analyzer: branch 5 | 매우 작음 |
| χ² 의 b jet 3 개 처리 | nb = 3 event 의 DNN 입력 | b jet ≥ 4 일 때만 계산 | AN §6.2.1 대로(넷째 jet = loose b → light); 기존 cut 단계는 그대로 두고 **새 branch** 로 | 작음 |
| W 후보 질량 | FH 고유 입력, GATJA FH 판 | cut·cut-step 히스토그램에만 | analyzer: branch 1 | 매우 작음 |
| light jet 수·HT, b jet HT | DNN, GATJA | helper 만 | analyzer | 매우 작음 |
| ΔR·Δη 통계 20, 평균 질량 4, maxPT 질량 2 | DNN, GATJA(Table 43) | helper 만(Δφ 를 접지 않는 버그) | analyzer: 옛 계산을 고쳐 DL 새 이름으로 | 작음 |
| Fox-Wolfram H0–H4, R1–R4(jet·b jet) | DL DNN 입력 | helper 만(정의가 AN 과 다름) | analyzer(AN 정의로) | 작음 |
| centrality | DL DNN 입력 | helper 만 | analyzer | 작음 |
| MET φ | GATJA 입력(FH 에서는 뺄 수 있음) | 없음 | analyzer: branch 1 | 매우 작음 |
| gen 매칭(jet 별: H→b, t→b, W→q, Z→b 와 어느 mother) | GATJA label, jet 배정 연구, DNN 진단 | H·t 의 b quark 만 모음, 매칭 없음 | analyzer(MC): H·Z·t·W 의 딸 quark 와 ΔR < 0.4(AN JABDT 와 같은 값), 가까운 순으로 중복 없이 | 중간 |
| χ²(m_H) 이웃 index(b jet 별) | GATJA graph | 없음 | ML 내보내기 도구(b jet 4-vector 로) | 작음 |
| BLR | SL·FH 계획의 입력 | 없음 | 내보내기 도구(MC 의 flavour 별 b-tag pdf 필요) | 중간, 나중 |
| event parity | 학습·평가 분할 | `eventNumber` 있음 | — | — |
| PU weight up/down | 계통 | nominal 만 | analyzer(payload 에 up/down 있음; 2024 는 우리 JSON) | 매우 작음 |
| L1 prefiring up/down | 계통(2017) | 2017 nominal 만(2018 은 EraConfig 로 끔, 2024 해당 없음) | analyzer(NanoAOD) | 매우 작음 |
| scale(μR, μF) weight | 계통 | 없음 | analyzer: `LHEScaleWeight` vector(≤ 9) | 작음 |
| PS(ISR, FSR) weight | 계통 | 없음 | analyzer: `PSWeight` vector(4) | 작음 |
| PDF weight | 계통(PDF shape) | 없음 | analyzer: `LHEPdfWeight` vector(~100) | **큼** — 나중에, 필요한 표본만 |
| b-tag shape 변형(Run 2: hf, lf, hfstats1/2, lfstats1/2, cferr1/2) | 계통 | central 만 Tree(hf·lf·cferr 은 계산만, stats 는 없음) | analyzer: weight 16 | 작음 |
| b-tag fixed WP(2024) | 계통·비교 | up/down 있음 | + 커밋 N 의 변형(D-2026-10-10-A) | 작음 |
| JES·JER 변형 | 계통(shape) | 없음(`sysName kJES/kJER` 은 정의만, `smearJER(..., "nom")`) | analyzer: sysType 배선 + 변형마다 재실행(출력 디렉터리 따로) | **큼**(실행 ×4) |
| trigger SF up/down | 계통 | 있음(2017; 2024 는 유도 중) | — | — |
| lepton SF(CR) | 계통(CR) | 없음(N4) | 따로 | 중간 |
| QCD multijet 추정 | FH 고유 배경 | 계획 | 따로 설계 | 큼 |

### 5.2 Tree v1 — 다음 빌드에 넣자는 것(PROPOSED; **10-10 (2) STEP 27 로 구현** — 이름·정의의 최종판은 STEP 27 문서 §O.1; 아래와 다른 점: Δη/ΔR 통계 21 개(bj 에 max ΔR 도), `centralityjl/jb` 대신 `centrality`·`bjetCentrality`(AN 식 12, FH 는 jet 만), `invMassHadW`·`nLooseJets`·`jetBTagWP` 추가, χ² 는 AN 의 jet 선택을 nM ≥ 2 로 넓힌 판)

- **jet vector(선택 jet, `jetPt` 와 같은 순서):** `jetPhi`, `jetMass`; MC 만 `jetGenMatch`(0 없음, 1 H→b, 2 t→b, 3 W→q, 4 Z→b),
  `jetGenMother`(그 event 의 hard-process 입자 번호: H1·H2·t·t̄·W⁺·W⁻·Z1·Z2, 없으면 −1 — 짝 정보).
- **event 변수:** `chi2Higgs`, `invMassH1`, `invMassH2`, `PTH1`, `PTH2`(HH; b jet ≥ 4), 같은 다섯의 AN 식 nb = 3 판(이름은 구현 때),
  `invMassHadW`, `lightjetNumber`, `bjetHT`, `lightjetHT`, `jetAverageMass`, `bjetAverageMass`, `lightjetAverageMass`, `bjetAverageMassSqr`,
  ΔR·Δη 통계 20(`averageDeltaRjj` … `minDeltaRpTbj`), `maxPTmassjjj`, `maxPTmassjbb`, `centralityjl`(FH: jet 만), `centralityjb`,
  `H0`–`H4`, `R1`–`R4`, `bjetH0`–`bjetH4`, `bjetR1`–`bjetR4`, `metPhi`. ZH·ZZ 는 지금 이름(`chi2ZH` …) 그대로 두고 내보내기 도구가 DL 이름
  (`chi2HiggsZ`, `chi2Z` …)으로 옮긴다.
- **weight(MC):** `PUWeight_up/_down`, `L1PrefiringWeight_up/_down`(2017), `LHEScaleWeight`, `PSWeight`, Run 2 의 `bTagWeight_<src>_up/_down` 16 개,
  2024 의 커밋 N 변형.
- **구현 때 고칠 것:** (1) `getStats`·`getStatsComb` 의 Δφ 는 `fabs(φ1 − φ2)` 라 ±π 를 가로지르는 쌍의 ΔR 이 틀린다 — [0, π] 로 접는다(옛 FHSample 의
  ΔR 계열도 같은 코드로 만들어졌다). (2) `getFoxWolfram` 은 i < j 쌍만 더하고 (ΣE)² 로 나눈다 — AN 식(13–16)은 i = j 를 포함하고 s 로 나눈다; AN 식으로.
  (3) χ² 항은 (m − m_X)²/√(0.1 ΣpT)(옛 코드와 같음, STEP 6 에서 비트 동일)이고 AN 은 σ² 를 JER 에서 잡는다 — 기존 cut 단계는 그대로 두고, DNN 입력의
  정의를 어느 쪽으로 할지 정한다(§8). (4) gamzeSample 은 ΔR·FW·centrality 의 기준이 될 수 없으니(§2.5) 정의 대조는 AN 식과 옛 analyzer 코드로.
- **크기 어림:** event 변수 ~55 + weight ~20 개 float(≈ 300 B) + jet vector 4 개(jet 8 개면 ≈ 130 B) + vector weight 13 개 — 압축 전 event 당
  ~0.5 kB. PDF 는 빼고. 지금 Tree 의 event 당 크기를 KNU 에서 재고 정한다.
- **확인 방법:** 오프라인 smoke(합성 표본)에서 새 branch 의 정의역; 몇 변수(ΔR_bb^min, χ²_HH, W 질량)를 Tree 의 vector(pT, η, φ, m)에서 Python 으로
  다시 계산해 bin 단위로 같음(±π 를 가로지르는 쌍을 일부러 넣어서); gen 매칭은 ttHH MC 에서 "H 에 매칭된 b jet 쌍의 질량이 125 GeV 근처" 분포로.

### 5.3 ML 내보내기 도구(PROPOSED, `tools/ml/`)

merge 된 출력의 `Tree/Tree` → 학습용 평평한 표(ROOT 또는 parquet): DL 새 이름의 jet·b jet·light jet 열(b jet = `bTagScore ≥ M`, light = `< L`),
GATJA 이웃 index(b jet 쌍의 χ²(m_H)), label(`jetGenMatch` 에서), 학습 weight(`evtWeight`·SF·xsec), parity(`eventNumber % 2`), class 번호.
진실 정보(label, `jetGenMatch`)와 weight 는 **입력 열과 따로** 둔다(§2.5 의 SWAN DL 판처럼 입력에 섞이지 않게). 2024 의 b-tag 입력은 연속 점수 대신
WP 칸(<L, L–M, M–T, T–XT, XT–XXT, ≥XXT)을 함께 만든다(D-2026-10-10-A).

## 6. 계통 불확도 — AN Table 53 대 우리

| 계통 | AN 형식 | 우리 2017(2018 v15 는 아직 실행 전) | 우리 2024(fixed WP) | 할 일 |
|---|---|---|---|---|
| luminosity | rate | — | — | datacard |
| lepton ID/iso | shape | CR 만, 미적용(N4) | 같음 | lepton SF 와 변형(CR) |
| trigger | shape | SF + up/down 있음 | SF 유도 중(STEP 26) | 연도별 비상관(datacard) |
| L1 prefiring | shape | nominal 만 | 해당 없음 | up/down |
| pileup | shape | nominal 만 | nominal 만 | up/down |
| JES | shape | 없음 | 없음 | 변형 실행(Total 또는 regrouped; 2024 payload 확인) |
| JER | shape | 없음 | 없음 | 변형 실행 |
| b-tag HF/LF/stats/cferr | shape | central 만 | 해당 없음 | weight 16 + norm |
| b-tag fixed WP | — | 해당 없음 | b jet up/down(c·light SF 없음) | D-2026-10-10-A |
| μR·μF, PDF+αs(rate) | rate | — | — | datacard(Table 9) |
| PDF shape | shape | 없음 | 없음 | `LHEPdfWeight` |
| μR·μF·ISR·FSR(shape; AN 은 SL 만) | shape | 없음 | 없음 | `LHEScaleWeight`, `PSWeight` |
| bin-by-bin | — | 없음(datacard 를 만들 때 `autoMCStats`) | 같음 | — |
| tt+HF 모델링(tt+B 비율 등) | AN §3, §8.2.1 | stitching 있음 | 2024 lookup 없음(D-2026-10-05-C) | 비율 계통 정의 |
| QCD multijet(FH) | — | 계획 | 계획 | data-driven 설계 |

## 7. 순서 (PROPOSED)

**A. 지금 — control plot 완성(T3 대기 중).** btagtrig 완료 → merge → 효율 map → trigger SF(STEP 26). 그동안 AI 가 준비·시험: 커밋 N(BTV 검토
반영, D-2026-10-10-A), 커밋 O(Tree v1, §5.2), 제출기의 `request_memory` 기본값을 측정에 맞게(2024 analyzer job 25,313 개의 최대 286 MB; 지금 12 GB
고정이라 pool 1,728 slot 중 10 개에만 맞음 — STATUS 10-10). → btagtrig analyzer job 이 모두 끝난 뒤 빌드 한 번 → SF 를 켠 main 셋(FH, μCR, eCR) →
control plot: 지금 것 + DNN 입력의 Data/MC(`make_plots.py` TREE_HISTS 에 추가; AN §6.3·6.4 에 해당).

**B. ML 내보내기 도구와 FH DNN 기준선.** §5.3 도구 → V3 노트북을 script 로(새 이름, node 는 §8 결정) — GATJA 없이 먼저.

**C. GATJA 의 FH 판.** stage 1 을 FH 표본으로(lepton·MET 대신 FH event 변수), 출력 24 → DNN 입력. GPU(lxplus HTCondor 또는 SWAN).

**D. 계통.** weight 변형으로 template → JES·JER 재실행(sysType) → datacard(combine) 와 rate 계통. QCD data-driven 은 별도 계획.

## 8. 정해 줄 것 (OPEN)

> 10-10 (2): 사용자 방향(D-2026-10-10-B)으로 1 은 "준비하라"(코드·시험 끝, 넣는 시점은 빌드 때 확인), 7 은 **DECIDED**(SWAN 기본·필요하면 KNU GPU, 학습은 Run 2 전체 +
> 2024). 6 은 §9 의 제안으로 좁혔다. 새 항목 8–9.

1. Tree v1 을 다음 빌드(SF main 앞)에 넣을지 — 권고: 예(SF main 셋을 다시 돌리지 않으려면). **10-10 (2): 코드 준비 끝(STEP 27), 시험 110/110.**
2. 이름: §3 의 제안(DL 새 이름 + b jet 양은 `bjet` 접두) — 권고: 예.
3. PDF weight 를 지금 넣을지 — 권고: 아니오(크다; 계통 단계에서 필요한 표본만).
4. gen 매칭 정의: ΔR < 0.4, 가까운 순 중복 없이; H·t·W·Z 의 마지막 사본의 딸 quark.
5. χ² 의 nb = 3 처리(AN 식을 새 branch 로)와 χ² 의 σ(우리 식 대 AN 의 JER) — 기존 cut 단계·SR 정의는 그대로.
6. FH DNN node: Hbb 덱의 9 개(SL 과 같음) + QCD 를 어떻게(node 또는 data-driven 으로 빼기) — V3 의 13 class 와 다르다.
7. ~~학습 GPU 와 학습 연도(2017 v9 / 2018 v15 / 2024).~~ **DECIDED (D-2026-10-10-B (7))**: SWAN 기본(필요하면 KNU GPU), 학습은 Run 2 전체 + 2024 — Run 2 의 v15
   신호 등 다섯 표본이 아직 없다는 의존(§10).
8. DNN 입력에 b-tag 점수·BLR 을 쓸지 — QCD template 의 CR→SR 이전과 충돌(§9.2) — 권고: TR·CR·SR 모양 동등성 검사를 통과한 변수만.
9. FH DNN 의 연도 처리: AN 처럼 연도별 학습 vs Run 2 를 묶고 연도 표지 — 권고: AN(연도별)으로 시작, 통계가 모자라면 묶음.

## 9. FH DNN 과 GATJA — AN 기반 설계 (PROPOSED, 10-10 (2); 사용자 방향 D-2026-10-10-B (1))

### 9.1 출발점 — AN-2022/122 의 DNN (사실; 쪽은 PDF)

| | SL DNN (§7.1, p.106–111) | DL DNN (§7.3, p.115–118) |
|---|---|---|
| 입력 | 50 개(시작 333, 첫 층 weight 합으로 선택; Table 49) | Table 43 전부 + GATJA 24 |
| 구조 | [512, 256, 128, 64], ReLU, softmax, dropout 0.2, batch 1024 | 같은 층, LAMB, cosine decay |
| 학습 | Adam lr 2×10⁻⁵ × 0.99312^epoch, categorical CE, Z_A(σ_b = 0.1)가 최대인 epoch, 최소 100 epoch | validation 0.2 |
| 표본 분할 | MC 홀수 event 학습(55/25/20), 짝수 event 로 판별변수, **연도별 학습** | 연도별 |
| 출력 | 9 node: ttHH, ttH, ttZH, ttZZ, ttZ, tt+lf, tt+cc, tt+nb, tt+mb → fit 은 ttHH, ttH, ttZ, ttbar | 6 node: ttHH, ttZH, ttZZ, ttH, ttZ, tt |
| 전처리 | QuantileTransformer(균등) + MinMaxScaler | 같음 |

### 9.2 FH 로 옮길 때 (PROPOSED)

- **class:** SL 의 9 node + **QCD**(ttH(bb) FH 처럼 TR 의 data, TF 적용; PLAN_QCD_DD §3.5). QCD node 로 분류된 event 는 QCD 가 많은 범주로 fit 에 들어가 QCD 정규화를
  잡는 데 쓴다 — 범주 설계는 SR 최적화(ROADMAP W5) 때.
- **입력:** Table 43(DL)과 부록 B(SL) 중 FH 에 있는 것(lepton·MET 제외) — Tree v1 이 덮는다(STEP 27 §O.1). **제약:** QCD template 을 낮은 b-tag 영역에서 가져오므로
  ttH FH 는 b-tag 값과 직접 상관된 입력을 피했다(AN-19-094 §8.1.3, §10.1: TR·CR·SR 의 모양이 같은 변수만; BLR 미사용). AN-2022/122 SL 은 b-tag 점수(d3…d6),
  BLR 을 많이 쓴다 → FH 에서는 **모양 동등성 검사를 의무**로 하고 통과한 것만(§8 8). χ²(AN jet 선택)도 nM = 3 에서 4 번째 jet 이 L–M 이므로 검사 대상.
- **연도:** AN 대로 연도별 학습에서 시작(§8 9). 2024 는 UParTAK4, Run 2 는 DeepJet 이라 b-tag 관련 입력의 정의가 연도마다 다르다(점수를 쓰지 않으면 문제 작음).
- **분할·평가:** AN 의 홀/짝 event 와 Z_A epoch 선택 그대로.
- **2024 의 b-tag 정보:** fixed-WP SF 만 있으므로 점수 대신 WP 칸(`jetBTagWP`)을 쓰는 것이 SF 와 맞다(D-2026-10-10-A 의 BTV 문장 "WP-binned information may be a BDT/DNN input").

### 9.3 GATJA 의 FH 판 (PROPOSED)

- stage 1(jet 배정): b jet 마다 node = (pT, η, φ, b-tag, 자기 최소 χ² 짝 번호), 이웃 = χ²(m_H) 최소·둘째(GATJA 발표, `gatja-studio`). event 입력에서 lepton·MET 대신
  `HT`, `bjetHT`, `lightjetHT`, `nJets`, `nbJets`, 평균 질량들. **정답 표지 = `jetGenMatch`**(1 H→b → "Higgs", 2 t→b → "top", 그 밖 "other"; STEP 27 §O.2).
- FH 에서는 W→q jet(표지 3)도 정답이 있어 4 class(H, t, W, other)로 넓힐 수 있다 — AN 의 3 class 로 시작.
- stage 2(ttHH vs ttZH + ttZZ)는 GATJA 그대로.
- 같은 제약: GATJA 의 node 입력에 b-tag 점수가 있다 — QCD template 영역에서 쓸 수 있는지 §9.2 의 검사.

### 9.4 도구 (`tools/ml/`, 다음)

`export_tree.py`: Tree v1 → 평평한 열(DL 새 이름: `jetPT1…`, `bjetPT1…`, `lightjetPT1…` — §3 의 함정 주의), class 표지, weight(evtWeight·SF·계통), 홀/짝; `gatja_inputs.py`:
b jet node·이웃·표지. 둘 다 Tree v1 의 정의(STEP 27)를 다시 계산하지 않고 읽는다.

## 10. 학습 데이터와 환경 (사용자 결정 D-2026-10-10-B (7); 나머지 PROPOSED)

| 연도 | 신호 TTHH | TT4b | TTZH·TTZZ | 상태 |
|---|---|---|---|---|
| 2024 (Summer24 v15) | 중앙 `TTHH-HHto4B` | `TT4B` | 중앙 | 생산 끝 — **첫 학습 표본** |
| 2018 (UL v15) | **없음** | **없음** | **없음** | MC 42 + Data 8 생산 끝, 다섯 표본은 요청·enriched 대기 |
| 2016·2017 (UL v15) | 없음 | 없음 | 없음 | hadronic 생산 전 |

출처: NtupleForge `docs/ttHH/04_mc_request_2026-09.md` §1(Run 2 다섯 표본: NanoAODv9 에는 있고 v15 에 없음; 요청 메일 본문 2026-09-14, 발송·답장 기록은 §5 에 없음),
`01_STATUS.md` 표 19–22. **"Run 2 전체로 학습"은 이 다섯 표본을 기다린다.**

- **환경:** SWAN(GPU) 기본. 표본은 KNU `/pnfs` 의 merge 출력 → 내보내기 도구로 평평한 파일 → EOS(CERNBox)로 xrdcp. GATJA 는 Python ≥ 3.10, TensorFlow, tf-addons
  (LAMB), tf-models-official 이 필요(`gatja-studio`) — tf-addons 는 개발이 끝난 패키지라 SWAN 의 TF 판과 맞는지 먼저 확인.
- **크기:** 첫 SF main 의 Tree 크기로 어림(STEP 27 §O.6).

## 확인하지 못한 것

- FHSample 의 tree 내용(받지 않음; EOS 목록만), GATJA 녹화(보지 않음), AN 부록 B 의 SL 변수 333 개 정의(옮기지 않음).
- DL allSys ntuple 의 계통 branch 구성(`btagweight` 등)과 학습된 GATJA weight — 자료에 없다.
- gamzeSample 12 파일 중 두 파일만 값을 확인했다(§2.5 의 깨진 branch).

## 부록 A. V3 입력 112 개의 출처(Tree v1 기준)

| V3 이름(옛 FH) | 개수 | 새 출처 |
|---|---|---|
| `bjetNumber`, `bbjetNumber`, `blightjetNumber` | 3 | `nJets`, `nbJets`, `lightjetNumber`(새) |
| `bjetPT1–12`, `bjetEta1–12` | 24 | `jetPt`, `jetEta` vector |
| `bbjetPT1–8`, `bbjetEta1–8`, `bbjetPhi1–8` | 24 | vector 에서 `bTagScore ≥ M`(φ 는 `jetPhi` 새) |
| `blightjetPT1–6`, `blightjetEta1–6` | 12 | vector 에서 `bTagScore < L` |
| `bjetHT`, `bbjetHT`, `blightjetHT` | 3 | `HT`, `bjetHT`(새), `lightjetHT`(새) |
| `bjetAverageMass`, `bbJetAverageMass`, `bbJetAverageMassSqr`, `bmaxPTmassjjj`, `bmaxPTmassjbb` | 5 | `jetAverageMass`, `bjetAverageMass`, `bjetAverageMassSqr`, `maxPTmassjjj`, `maxPTmassjbb`(새) |
| ΔR·Δη 통계(`baverageDeltaEtajj` … `bminDeltaRpTbj`) | 18 | 새(DL 이름, Δφ 를 고친 것) |
| `bchi2Higgs`, `bchi2Z`, `bchi2HiggsZ` | 3 | `chi2Higgs`(새), `chi2ZZ`, `chi2ZH` |
| `binvMassH1/H2`, `binvMassZ1/Z2`, `binvMassHiggsZ1/HiggsZ2` | 6 | `invMassH1/H2`(새), `mZ1ZZ/mZ2ZZ`, **`mHcandZH`/`mZcandZH`**(옛 코드의 HiggsZ1 = H 후보, HiggsZ2 = Z 후보) |
| `bPTH1`, `bPTH2` | 2 | `PTH1`, `PTH2`(새) |
| `baplanarity`, `bsphericity`, `btransSphericity`, `bcValue`, `bdValue`, `bcentralityjl`, `bcentralityjb` | 7 | `aplanarity`, `sphericity`, `transSphericity`, `eventC`, `eventD`, `centralityjl`, `centralityjb`(새; 옛 코드: jet+lepton, jet+b jet) |
| `bbaplanarity`, `bbsphericity`, `bbtransSphericity`, `bbcValue`, `bbdValue` | 5 | `bjetAplanarity`, `bjetSphericity`, `bjetTransSphericity`, `bjetEventC`, `bjetEventD` |
| 합 | 112 | |
