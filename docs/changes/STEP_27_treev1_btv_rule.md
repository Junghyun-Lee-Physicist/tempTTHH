# STEP 27 — BTV 다중 WP 규칙(N)과 Tree v1: ML 입력·jet 정답 표지·계통 weight(O)

- 날짜: 2026-10-10. 결정 [`../DECISIONS.md`](../DECISIONS.md) **D-2026-10-10-A**(N: 사용자 "5번에 정해 줄 것은 모두 네 권고대로 수용한다")와
  **D-2026-10-10-B**(O: "ntuple branch는 DNN과 GATJA 학습이 가능하도록 … control plot 작업을 하면서 동시에 이걸 준비하자", "systematic uncertainty …
  작업 준비도 함께"). 계획 [`../PLAN_ML_SYST.md`](../PLAN_ML_SYST.md) §5.2(Tree v1), §6(계통).
- 신규: `include/TreeVars.h`(Tree v1 의 event 변수, 4-vector 만으로), `include/GenMatch.h`(jet ↔ hard-process quark), `test/test_TreeVars.cc`(단위 시험 27),
  `test/offline_smoke/treev1_check.py`(analyzer 출력의 Tree v1 을 다시 계산해 대조), 이 문서
- 수정(analyzer): `ttHHanalyzer_unified.{cc,h}` — N: `computeBTagWeightFixedWP_`(BTV 규칙과 비교 weight 둘), `bookBTagEff_`·`fillBTagClosure_`(HT 단계의
  true-b 별·M-tag 다중도별 closure 히스토그램), 끝의 `[btagSF]` 줄; O: `fillTreeV1_`, `computeBTagShapeVariations_`, `initTree`(branch 75 + 연도별
  16 또는 2), `setTreePdf`, `metPhi_`, `requireBranches_`(MET φ, L1 prefiring up/down). `src/tnm.cc`·`include/tnm.h`(`--tree-pdf`).
- 수정(도구·시험): `submit_job_FH_Tier3_unified.py`(`--memory` 기본 **2 GB**, `--tree-pdf`), `plotter/make_plots.py`(`--tree-v1`),
  `test/run_unit_tests.sh`(+test_TreeVars), `test/offline_smoke/{run_offline_smoke.sh, synth_nano.py, fake_pog.py}`, `tools/runlog/condor_run.sh`(주석).
- **KNU 빌드:** header 가 바뀌었으므로 **`make clean && make -j4`**(PLAN §9.3: 최상위 Makefile 은 header 의존성이 없다). **btagtrig 의 analyzer job 이 큐에
  하나라도 있으면 빌드하지 않는다**(실행 파일을 쓰는 중). 명령은 워크스페이스 `RUNBOOK_lxplus_2026-09-16.md` §33.
- **바뀌지 않는 것:** selection, cutflow, 기존 히스토그램, 기존 Tree branch 의 값(2024 의 `bTagWeight` 는 아래 N 의 한 경우만), 2017 의 출력(컨테이너의
  Y1 비교: 2017 네 run 이 그 전 빌드와 같다, §5).
- **P (같은 날, 독립 검토 반영 — 아래 §P):** `jetGenTopIdx`(Tree v1 core 75 → **76**), analyzer·제출기 `--tree-v1 on|off`(기본 on), Run 2 shape 키의 load 검사(E40),
  병합기 `outputMerger/run_one_hadd.sh` 의 `Tree/Tree` branch 집합 검사(exit 8), cAsB 의 큰 jet weight 수, NaN 점수의 WP 칸, 제출기 `--memory` 0 거부.
  시험: 단위 **30/30**, offline smoke **116/116**, failure checks **81/81**.

## 결론 먼저

1. **N (D-2026-10-10-A):** 2024 fixed-WP weight 의 L–M 구간 인자에서 분자 SF_L ε_L − SF_M ε_M 이 음수이면 jet weight 를 **1**(BTV wiki 규칙; 전에는 0 →
   event weight 0). fail-L 분자 1 − SF_L ε_L 의 음수는 그대로 0(BTV 에 질문할 것). 같은 jet loop 에서 비교 weight 둘: `bTagWeight_oldRule`(커밋 K 의
   규칙), `bTagWeight_cAsB`(c jet 에 b-jet SF 와 우리 c 효율). HT 단계에서 **true b jet 수(0–4+)별·M-tag 다중도별 closure** 를 `BTagEff/` 에 히스토그램으로.
2. **O (Tree v1):** `Tree/Tree` 에 AN-2022/122 Table 43 의 FH 용 입력(χ² pairing 은 AN 의 jet 선택 규칙으로 nM ≥ 2 에서, ΔR·Δη 통계 21, Fox-Wolfram, centrality,
   평균 질량, max-pT 3-jet 질량, b/light HT), jet φ·질량·WP 칸, **MC 의 jet ↔ quark 정답 표지**(GATJA·재구성 효율용), **계통 weight**(PU up/down, L1 prefiring
   up/down, LHE scale·PS 벡터, Run 2 b-tag shape 의 출처 8 개 × up/down; PDF 는 `--tree-pdf on` 일 때만). 모든 정의는 header 두 개에 한 번씩.
3. **고친 옛 helper 의 결함(새 branch 는 새 함수로; 옛 helper 는 그대로 두고 쓰지 않음):** Δφ 를 [0, π] 로 접지 않음, FW 가 i = j 를 빼고 i < j 만, bj 통계가 b jet
   을 자기 자신과 짝지음(ΔR = 0), jjj 3-jet 조합이 같은 jet 을 두 번, (m²)_b^avg 를 (Σm)²/n 로. 옛 FHSample 의 해당 열도 같은 helper 로 만들어졌다.
4. **Run 2 b-tag shape 변형의 recipe 를 BTV 식으로**: c jet 은 cferr1/2 만, b·light jet 은 cferr 외 전부(payload 가 출처별로 무엇을 하는지 안다;
   `bTagSF_ReweightStudy` 의 `resolveJetSystematic` 과 같은 규칙). 전의 analyzer 는 hf 를 b jet 에만, lf 를 light jet 에만 걸었는데 그 값은 어디에도
   쓰이지 않았다(Tree 에 없음) — 결과에는 영향이 없었다.
5. **시험(컨테이너, ROOT 6.40 pip, correctionlib 2.9):** 단위 시험 27/27, offline smoke **110/110**(새 15: Tree v1 다섯 표본의 독립 재계산, BTV 규칙이
   실제로 작동하는 payload, closure 히스토그램, `--tree-pdf`), failure checks 76/76, runlog 85/85, btag_eff_maps 26/26, consolidate 24/24, stage0 19/19.
   stage1 은 15/1(치명 아님: treestream fork 가 컨테이너에 없음 — 바꾸기 전에도 같은 1 개).

## N — D-2026-10-10-A 의 구현

### N.1 jet weight (method 1a, 두 WP L·M; `computeBTagWeightFixedWP_`)

| jet 의 칸 | weight | 바뀐 점 |
|---|---|---|
| score ≥ M | SF_M | 없음 |
| L ≤ score < M | (SF_L ε_L − SF_M ε_M)/(ε_L − ε_M); **분자 < 0 → 1** | 전: max(0, 분자)/(ε_L − ε_M) = 0 |
| score < L | (1 − SF_L ε_L)/(1 − ε_L); 분자 < 0 → 0 | 없음(SF 에 연속; BTV 질문) |

up/down 도 같은 규칙. 분모가 1e−6 이하이면 1(전과 같음). 2024 의 c·light jet 은 SF 가 없어 weight 1(전과 같음).

**비교 weight(중앙값만, D-2026-10-10-A (4)):** `bTagWeight_oldRule` = 같은 jet 들에 커밋 K 의 규칙(L–M 분자 < 0 → 0). `bTagWeight_cAsB` = b jet 은 위의
weight, c jet(hadronFlavour 4)은 b-jet SF(flavour 5 로 payload 를 부름)와 **우리 c-jet 효율**(같은 map 의 c)로 같은 식, light 는 1. payload 가 c SF 를 갖게
되면(`btagFixedWPCovers(4)`) `cAsB` 는 주 weight 와 같아진다.

### N.2 HT 단계의 closure (D-2026-10-10-A (2), (4))

`applyEventScaleFactors`(HT > 500 통과 직후, b-tag cut 전)에서 `_btagFixedWPReady` 인 MC 만:

- `BTagEff/h_ntrueb_{w, wb, wb_oldRule, wb_cAsB}`: 선택 jet 중 hadronFlavour 5 의 수(0, 1, 2, 3, ≥4; 5 bin)에 Σw, Σw·bTagWeight, Σw·_oldRule, Σw·_cAsB.
- `BTagEff/h_nbM_{…}`: 같은 네 합을 M-tag 다중도(0 … ≥6; 7 bin)로.
- w = SF 전 weight(base × PU × L1 × genW × stitch; `fillBTagEff_` 와 같은 값). hadd 로 더해지므로 merge 뒤 bin 마다 비 = closure.
- job 끝의 줄 셋(기존 둘 뒤):

```
[btagSF] intermediate (L <= score < M) numerator < 0 -> jet weight 1 (BTV): central N, up N, down N
[btagSF] closure per true b jets at the HT step, sum(w x bTagWeight) / sum(w): 0b x 1b x 2b x 3b x >=4b x (each about 1 when the maps fit; BTagEff/h_ntrueb_*)
[btagSF] comparison weights, closure at the HT step: oldRule (intermediate < 0 -> 0) x, cAsB (c jets with the b SF; N c jets) x
```

true-b 별 비가 1 에서 벗어나는 크기가 BTV 가 경고한 "N_b 로의 효율 외삽" 편향이다(ttH(bb) Run 3 의 tt+light 사례). 크면 효율 map 에 true-b 축을 더하는
것을 검토한다(D-2026-10-10-A 의 supersede 조건).

### N.3 실제 payload 에서 무엇이 바뀌나

`UParTAK4_kinfit` 의 SF_M 이 SF_L 보다 ε_L/ε_M 배 이상 커야 L–M 분자가 음수가 된다(b jet 의 ε_L ≈ 0.9, ε_M ≈ 0.8 이면 SF_M/SF_L > 1.12). 중앙값에서는 드물
것으로 보이고(추론), up/down 에서 생길 수 있다. **얼마나 생기는지는 SF main job 의 위 첫 줄이 센다.** 0 이면 `bTagWeight` 는 K 와 비트 단위로 같다
(offline smoke 의 기본 payload 가 그 경우: oldRule = bTagWeight).

## O — Tree v1

### O.1 branch (Tree/Tree; 이름은 AN Table 43 / DL ntuple, b-jet 양은 `bjet` 접두 — PLAN §3)

b jet = score ≥ M, light jet = score < L, non-b = score < M. −1 = 그 event 에서 정의되지 않음(대상 수 부족).

| 묶음 | branch | 정의(`include/TreeVars.h`, `GenMatch.h`) |
|---|---|---|
| jet vector | `jetPhi`, `jetMass`, `jetBTagWP`(0 < L, 1 [L,M), 2 [M,T), 3 ≥ T) | `jetPt` 와 같은 순서(보정 뒤 pT 내림차순) |
| jet 정답(MC) | `jetGenMatch`(0 없음, 1 H→b, 2 t→b, 3 W→q, 4 Z→q), `jetGenMotherIdx`(어미의 첫 사본의 GenPart 번호, −1) | §O.2; Data 는 0 / −1 |
| 다중도 | `lightjetNumber`, `nLooseJets`(score ≥ L, b 포함) | |
| HT | `bjetHT`, `lightjetHT` | Σ pT |
| 질량 | `jetAverageMass`, `bjetAverageMass`, `lightjetAverageMass`, `bjetAverageMassSqr`(Σm²/n), `maxPTmassjjj`, `maxPTmassjbb` | AN Table 43 |
| 각도(21) | `{averageDeltaR, minDeltaR, maxDeltaR, averageDeltaEta, maxDeltaEta, minDeltaRMass, minDeltaRpT}` × `{jj, bb, bj}` | jj = 모든 jet 쌍, bb = b jet 쌍, bj = (b, non-b); Δφ 는 [0, π] |
| Fox-Wolfram | `H0`–`H4`, `R1`–`R4`, `bjetH0`–`bjetH4`, `bjetR1`–`bjetR4` | AN 식 (16), i = j 포함, s = (ΣE)² (AN 에 s 정의 없음 → e⁺e⁻ 관례); 대상 < 2 → −1 |
| centrality | `centrality`(jet), `bjetCentrality` | AN 식 (12), FH 는 lepton 없음: Σ pT / Σ \|p\| |
| χ² (AN 규칙) | `chi2Higgs`, `invMassH1`(m_H 에 가장 가까운 쌍), `invMassH2`, `PTH1`, `PTH2`, `chi2HiggsZ`, `invMassHiggsZ1`(H 후보), `invMassHiggsZ2`(Z 후보), `chi2Z`, `invMassZ1`, `invMassZ2` | §O.3 |
| MET | `metPhi` | `MET_pt` 와 같은 MET(2024 PUPPI, Run 2 v9 `MET_phi`) |
| m_qq | `invMassHadW` | `30<HadW<250` cut 이 쓰는 값: m_W(80.377)에 가장 가까운 dijet 질량, light jet(score < L)이 둘 이상이면 그 중에서, 아니면 모든 선택 jet 에서. ttH(bb) FH QCD 영역의 m_qq 축(PLAN_QCD_DD §3) |
| 계통 weight(MC) | `PUWeight_up/_down`, `L1PrefiringWeight_up/_down`(2016·2017; 그 외 1), `LHEScaleWeight`, `PSWeight`(NanoAOD 벡터 그대로), `LHEPdfWeight`(`--tree-pdf on` 일 때만) | §O.4 |
| Run 2 b-tag | `bTagWeight_{hf,lf,hfstats1,hfstats2,lfstats1,lfstats2,cferr1,cferr2}_{up,down}` (shape 연도만) | §O.5 |
| 2024 b-tag | `bTagWeight_oldRule`, `bTagWeight_cAsB` (fixed-WP 연도만) | §N.1 |

STEP 6 의 `chi2ZH`·`mZcandZH`·`mHcandZH`·`chi2ZZ`·`mZ1ZZ`·`mZ2ZZ`(cut-flow 재구성, b jet ≥ 4, 125.38/91 GeV)는 그대로 있다. 같은 event 의 nM ≥ 4 에서
새 χ² 와 옛 χ² 는 목표 질량만 다르다(알고리즘은 같다: 단위 시험 "nM = 5 bit-identical").

### O.2 jet ↔ quark 정답 (`GenMatch.h`; PROPOSED — PLAN §8 4)

- parton = `|pdgId|` 1–5, statusFlags 의 fromHardProcess(bit 8)와 isLastCopy(bit 13).
- 기원 = quark 자신의 사본을 건너뛴 첫 다른 `|pdgId|` 조상: H(25)·b → 1, t(6)·b → 2, W(24) → 3, Z(23) → 4. 그 밖(4FS tt+bb 의 추가 b, gluon 분열 등)은 parton 아님.
- 어미 번호 = 그 조상의 **첫 사본**(같은 H 의 두 b 가 같은 번호를 갖는다 → GATJA 의 "같은 Higgs" 정보).
- 짝짓기 = ΔR < 0.4 인 모든 (jet, parton) 쌍을 ΔR 오름차순으로, jet·parton 각각 한 번만(greedy unique). AN 의 JABDT 는 ΔR(jet, gen jet) < 0.4.
- GATJA 의 표지: `bjetHiggsMatched` = (b jet 의 `jetGenMatch` == 1), `bjetTopMatched` = (== 2).

### O.3 χ² pairing — AN 의 jet 선택 (AN-2022/122 §6.2.1 l.744–757)

| nM (score ≥ M) | 네 jet | 출처 |
|---|---|---|
| ≥ 4 | M jet 중 모든 4 개 조합 | AN (= cut-flow 재구성) |
| 3 | M 셋 + L–M jet 중 하나(없으면 L 미만 jet 중 하나) | AN |
| 2 | M 둘 + L–M jet 둘(하나뿐이면 그것과 L 미만 하나, 없으면 L 미만 둘) | **우리 확장**: ttH(bb) FH 방법의 TR(2M + ≥2L)·CR 에서 DNN 입력이 정의되도록(PLAN_QCD_DD §3) |
| < 2 | 없음(−1) | |

모든 후보 조합과 모든 pairing 에서 가설(HH, ZZ, ZH 양방향)마다 최소 χ². 목표 질량 m_H 125, m_Z 91.2(AN l.741–742). χ² 한 항은 HiggsReconstructor 의
(m − m_X)²/√(0.1 Σ pT)(2026-06 코드와 비트 동일) — **AN 의 σ("JER 의 함수")는 수치가 없어 OPEN**(PLAN §8 5).

### O.4 계통 weight

- PU: `CorrectionsManager::getPUWeight(nTrue, "up"/"down")` — Run 2 는 LUM POG payload, 2024 는 우리 DQM 판 JSON(최소 편향 σ ±4.6 %, D15).
- L1 prefiring: `L1PreFiringWeight_Up/_Dn`(2016·2017 MC; 그 연도에는 필수 branch 로 검사 — 없으면 시작 때 FATAL). 2018·2024 는 1.
- `LHEScaleWeight`, `PSWeight`: NanoAOD 벡터를 그대로(크기·순서는 표본의 것). **순서는 NanoAOD branch 제목이 말한다** — 보통 LHEScaleWeight 9 개
  [μR, μF] = [0.5,0.5], [0.5,1], [0.5,2], [1,0.5], [1,1], [1,2], [2,0.5], [2,1], [2,2](8 개인 표본은 [1,1] 이 빠짐), PSWeight 4 개 [ISR 2, FSR 1], [ISR 1, FSR 2],
  [ISR 0.5, FSR 1], [ISR 1, FSR 0.5] — 기억이므로 KNU 에서 실제 파일의 제목으로 확인(RUNBOOK §33). job 의 첫 Tree event 에서 `[treeV1]` 줄이 크기를 찍는다.
- `LHEPdfWeight`: event 당 ~100 float 이라 기본은 저장하지 않는다(`--tree-pdf on`; 제출기 `--tree-pdf on`).
- b-tag norm reweight 의 변형별 값(Run 2, AN §5.3 "변동마다 따로")은 이 STEP 에 없다: reweight study 가 변형마다 유도한 뒤 downstream 이 곱한다.

### O.5 Run 2 shape 의 출처별 weight

`computeBTagShapeVariations_`: Tree 에 들어가는 event 에서만 계산(event 당 jet × 17 번 payload 평가를 cut 전 모든 event 에 하지 않으려고). jet 마다 c 이면
cferr1/2 에서만 변형, b·light 이면 cferr 외 모든 출처에서 변형, 나머지는 중앙값. 중앙 `bTagWeight` 는 전처럼 모든 event 에서(cut-flow 사슬이 쓴다) — 값은 같다
(같은 함수, 같은 인자, 같은 곱 순서). AN Table 53 의 jes 출처는 JES 변형 실행(W5)과 함께.

### O.6 크기와 시간

event 당 float ~75 개 + 벡터 5 개(jet 수만큼) + LHEScale 9 + PS 4 → 압축 전 대략 0.5 kB 더(PLAN §5.2 의 어림). **KNU 에서 첫 SF main 의 merge 크기를 전과 비교**(RUNBOOK §33).
χ² 조합(nM = 2 에서 L 미만 jet 이 많으면 C(n,2) 조합)과 3-jet 조합 O(n³)은 Tree 에 들어가는 event 에서만 계산된다.

## 도구

- **제출기:** `--memory`(기본 **2 GB**; 전 12 GB 고정). 근거: 2024 analyzer job 25,313 개의 최대 286 MB·중앙값 125 MB(KNU 10-09, STATUS 10-10); 12 GB 는
  pool 1,728 slot 중 10 개에만 맞았다. 형식 `'<수> MB|GB'`, 다른 형식은 exit 2. `--tree-pdf on` 은 analyzer 에 넘긴다. preflight 에 한 줄.
- **plotter:** `--tree-v1`(`--tree-cut` 과 함께) — Tree v1 의 ML 입력 30 개와 m_qq 1 개(모두 31 개)를 Data/MC 로(AN §6.3–6.4 의 입력 검증에 해당). 옛 tree 는 그 열이 없어 TREE FAIL.

## 시험

### 단위 (`test/test_TreeVars.cc`, ROOT 필요 — `test/run_unit_tests.sh` 의 넷째)

Δφ 접기, ΔR·Δη 통계 = 무차별 계산, FW 의 닫힌 꼴(등 에너지 back-to-back: H0 = 1, H1 = 0, H2 = 1 …)과 무차별 이중합, centrality·HT·평균 질량, max-pT 3-jet,
χ²: nM = 5 에서 HiggsReconstructor 와 비트 동일·무차별과 같음, 질량 순서, nM = 3(L 있음·없음), nM = 2(L 하나), nM = 1 → −1; 정답 표지: 합성 gen record
(H 의 두 b 사본, t→b, W→q 둘, hard process 아닌 b, ±π 를 건너는 Z→b)에서 표지·어미·greedy 유일성. **27/27 PASS.**

### offline smoke (`test/offline_smoke/run_offline_smoke.sh`, 110/110)

- `treev1_check.py` 로 다섯 출력(2024 MC, 2024 MC `--tree-pdf on`, 2024 Data, 2017 MC, 2017 Data)의 **모든 event**: Tree 의 jet vector 로 모든 Tree v1 변수를 numpy 로
  다시 계산(헤더와 독립 구현), 입력 파일의 같은 event 로 metPhi·정답 표지(hard parton 과 greedy 매칭을 다시)·PU up/down(correctionlib)·L1 up/down·LHE/PS 벡터·
  2017 의 shape 출처 16 개(가짜 payload 에 출처마다 다른 값; BTV recipe)를 대조. 합성 표본의 gen record 는 b quark 를 b jet 위에, W quark 를 다른 jet 위에
  두었다(둘째 난수열 — 전의 표본 값은 그대로).
- N: 기본 payload(SF_M < SF_L)에서 oldRule·cAsB 재계산 일치, 음수 0 개, 끝의 줄 셋, `h_ntrueb_*`·`h_nbM_*` 의 비 = 찍힌 closure; **SF_M ≫ SF_L payload**
  (`fake_pog.py --btv-interneg`)에서 음수 분자가 생기고(트리 215, job 493 jet) `bTagWeight` = BTV 규칙, `_oldRule` = K 규칙이 재계산과 2e-6 안에서 같고 둘이 다르다.
- `--tree-pdf on` 은 103 개를 저장, 다른 값은 E11. 2017 네 run 은 그 전 빌드와 같다(y1_compare).

## P — 독립 검토 반영 (2026-10-10)

독립 검토(AI 에이전트, 읽기 전용, 컨테이너에서 옛·새 빌드를 같은 합성 입력으로 비교)는 **BUG 0**, RISK 4, NIT 10 을 냈다. 확인된 것: method-1a jet weight 와
BTV·K 규칙, `bTagWeight_oldRule` = 옛 빌드의 `bTagWeight`(588 사건 모두, 기본·interneg payload), shape recipe = `resolveJetSystematic`, 중앙값과 옛 출력
(2024 MC main·Data·btagtrig·`--btagsf on` 의 620/634 히스토그램과 공통 branch)이 같음, 모든 Tree v1 branch 가 사건마다 채워짐, `chi2AN` = 무차별 계산(4000 배치),
GenMatch 의 순환·범위 밖 어미 index 에 안전, 새 필수 branch 가 2017 v9·2024 v15 job 을 멈추지 않음. 반영:

| # | 검토 | 고친 것 | 시험 |
|---|---|---|---|
| R1 | 두 빌드의 출력(재빌드 뒤 재제출한 job)을 hadd 하면 실패하지 않고 옛 파일이 앞이면 새 branch 가, 새 파일이 앞이면 옛 파일의 사건 전부가 조용히 빠진다(히스토그램은 다 더해짐) | `run_one_hadd.sh`: hadd 전에 모든 입력의 `Tree/Tree` branch 집합을 ROOT macro 로 비교, 다르면 **exit 8**(처음 셋의 +/− branch 를 찍음), 검사가 못 돌면 exit 9 | failure checks B: 한 집합 → `[schema] OK`·병합, 한 파일에 branch 하나 더 → exit 8, hadd 안 함 |
| R2 | Run 2 의 출처별 shape weight: payload 가 키를 모르면 `getBTagSF_Shape` 가 1.0 을 돌려 변형이 조용히 틀림 | `CorrectionsManager::loadBTag`(Run 2): 출처 8 × up/down 을 레시피의 flavour(b·light: cferr 밖, c: cferr1/2)마다 한 번 계산 — 하나라도 실패하면 load 에서 **E40**, 성공하면 `[CorrectionsManager] b-tag shape SF (<year>): … (28 keys x flavours)` | smoke: 2017 의 줄; `fake_pog.py --btv2017-drop up_hfstats1` payload 는 E40 로 멈추며 키와 flavour 둘을 찍음 |
| R3 | Tree v1 은 끌 수 없고 tree 를 약 2.3 배로(압축 전 byte 의 56 %(2024 MC)·59 %(2017 MC), 사건당 약 0.66–0.72 kB) — btagtrig·lepton CR 에도 | analyzer `--tree-v1 on\|off`(기본 on; 생성자가 tree 를 booking 하므로 main 이 그 전에 정함), 제출기 `--tree-v1`(off 일 때만 넘김; preflight 줄에 표시). off = STEP 26 의 tree + 2024 비교 weight 둘. `--tree-pdf on` 은 Tree v1 이 필요(analyzer E11, 제출기 exit 2). Run 2 의 `computeBTagShapeVariations_` 는 `fillTree` 로 옮겨 off 에서도 옛 `bTagWeight_up/_down` 를 채움 | smoke: off 의 tree = on 의 tree − Tree v1 76(사건·값 같음), 히스토그램 모두 같음; off+pdf, 잘못된 값 → E11; failure checks E: 제출기의 둘 |
| R4 | `bTagWeight_cAsB` 는 load 때 c 효율 map 을 요구하지 않아 얇은 c bin 에서 jet weight 수백이 나와도 표시가 없음 | c jet weight > 10 을 세어 끝의 비교 weight 줄에: `… N c jets weighted in all events, M with a jet weight > 10` | smoke 의 줄 형식 |
| N1 | NaN 점수 → `jetBTagWP` = 3 | `sc >= T ? 3 : sc >= M ? 2 : sc >= L ? 1 : 0`(NaN → 0) | — |
| N3 | `--memory '0 GB'` 통과 | 0 이하 거부(exit 2) | failure checks E |
| N4 | Data 검사가 L1·PDF·shape 출처를 건너뜀 | `treev1_check.py` 의 Data 검사에 L1 up/down = 1, `LHEPdfWeight` 빈 것, 출처 weight = 1, top 번호 −1 | smoke(tv d24C, d17F) |
| N7 | `--tree-pdf on` 의 "(MC)" 줄이 Data 에도 | Data 는 "the LHEPdfWeight branch is booked; Data leaves it empty" | — |
| N8 | 끝의 "N c jets" 가 모든 사건의 수인지 불분명 | 줄에 "weighted in all events" | — |
| N9 | GenMatch 의 Δη 를 float 로(Python 은 double) | double 로 | — |
| N10 | W→q jet 의 어미는 W 라 FH 의 두 hadronic top 중 어느 top 인지 알 수 없음(GenPart 는 tree 에 없음) | **`jetGenTopIdx`**(vector<int>): t→b 는 top 의 첫 사본, t→W→q 는 W 의 첫 사본의 부모가 top 이면 그 top 의 첫 사본, 아니면 −1; Data −1 | 단위 3(공유 top 6, H·Z −1, 부모가 H 인 W → −1), smoke 의 재계산과 "top 에 연결된 jet" 수 |

미반영: N2(NaN 입력의 ΔR sentinel — 입력에 NaN 이 없음), N5(독립 구현이 같은 규칙 해석을 공유 — 정의는 AN 과 대조할 일), N6(synth 주석).
검토가 AN 이 없어 확인하지 못한 셋은 AN 본문(an.txt)으로 대조했다: centrality 의 분모는 AN 식 12 그대로 Σ|p|(PDF p.52; ttH 관례 Σ E 가 아님); `bj` 쌍
통계(b × non-b)는 AN Table 43 에 없다(Table 43 은 jj·bb; SL DNN 입력 목록에는 non-b·non-b(PDF p.108), 부록 B 에는 lepton·non-b(PDF p.151)) — DL ntuple 이름을 따른 우리 확장;
χ² 의 jet 선택은 nM ≥ 4 와 nM = 3 이 AN l.744–757 과 같고 nM = 2 는 우리 확장(AN_KR 7 장 표).
**branch 수(합성 출력에서 셈, `--tree-pdf` 없이):** 2017 = 57 + 76 + 16 = **149**(off 57), 2018 = 150(off 58), 2024 = 60 + 2 + 76 = **138**(off 62).

## 열린 것

- χ² 의 σ(AN 의 JER 식) — PLAN §8 5. 이름 규칙(§3)·정답 규칙(§O.2)은 PROPOSED(사용자 확인).
- LHEScaleWeight·PSWeight 의 순서를 실제 v15 파일의 제목으로 확인(RUNBOOK §33).
- Tree 크기와 CPU 를 첫 SF main 에서 측정(합성 출력: Tree v1 이 압축 전 byte 의 56–59 %). lepton CR·btagtrig 에서 `--tree-v1 off` 를 쓸지(사용자).
- 2024 c-jet SF(BTV 답), fail-L 음수(BTV 질문) — D-2026-10-10-A.

## 확인하지 못한 것

- 실제 BTV payload(2017 `deepJet_shape` 의 출처 키 이름 `up_hfstats1` 등, 2024 `UParTAK4_kinfit`)로 새 경로를 돌린 결과 — 컨테이너에는 가짜 payload 뿐.
  출처 키 이름은 `src/CorrectionsManager.cc` 주석과 `bTagSF_ReweightStudy` 가 쓰는 것과 같다(그 도구는 KNU 에서 2017 에 돌았다). **P 부터는 키가 틀리면
  job 이 load 에서 E40 으로 멈춘다**(조용히 틀리지 않는다).
- KNU 의 ROOT 6.30 hadd 에서 R1 의 동작(검토는 ROOT 6.40 으로 재현) — 병합기의 검사는 hadd 판과 상관없이 hadd 전에 멈춘다.
- 실제 v15 파일의 gen record 에서의 표지 분포 — 첫 SF main 뒤 ttHH 표본에서 "H 에 매칭된 b jet 쌍의 질량"을 보는 것으로(PLAN §5.2 확인 방법).
