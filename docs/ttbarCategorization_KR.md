# ttHH(HH→4b) Fully-Hadronic Channel 의 ttbar Categorization

**Personal Analysis Note — JH**
**Run2 2017 NanoAODv9 UltraLegacy, CMSSW_14_2_1**
*최종 수정: 2026-04-12*

---

## Abstract

이 문서는 ttHH(HH→4b) fully-hadronic 분석에서 사용하는 per-event ttbar
categorization scheme 를 기술한다. 다루는 내용은 물리적 motivation,
AN 이 인용하는 official algorithm (CMS `GenTtbarCategorizer` plugin
을 거쳐 NanoAOD 의 `genTtbarId` integer 로 노출되는 경로),
cross-check 용으로 독립 구현된 algorithm, event-level 에서 bbb / 4b
sub-class 를 분리하지 않고 collapse 한 이유, NtupleForge 와 analyzer
양쪽의 기술적 구현, 그리고 두 codebase 가 1.28 M MC events 위에서
byte-level 로 동등함을 증명한 validation pipeline 이다.

---

## 1. 물리적 motivation

### 1.1 왜 ttbar 를 categorize 하는가

ttHH(HH→4b) fully-hadronic final state 는 parton level 에서 6 개의
b-jet 과 4 개의 light jet 으로 구성된다 — 두 top decay 에서 각각
b 와 W → qq 가 나오고, 두 Higgs decay 에서 각각 H → bb 가 나온다.
Hadronization, parton shower, detector reconstruction 을 거치면
typical signal event 는 7~10 개의 reconstructed jet 과 ≥4 개의
b-tagged jet 을 가지게 된다. 이 final state 의 dominant irreducible
background 는 추가적인 heavy-flavour radiation 을 동반하는 QCD
ttbar production, 특히 **ttbar + b-jets** 과정이다 (CMS notation
으로 흔히 tt+bb, tt+b, tt+2b 로 표기).

문제는 ttbar Monte Carlo 가 모든 가능한 heavy-flavour multiplicity
를 한 sample 안에 포함하도록 생산되지 않는다는 점이다. Production
은 다음과 같이 나뉜다:

- **Inclusive ttbar samples** (예: `TTToHadronic`, `TTToSemiLeptonic`,
  `TTTo2L2Nu`) — 5-flavour scheme (5FS) 으로 생성. b-quark 가 massless
  로 취급되고 추가 b-jet 은 parton shower (PS) 에서 나옴.
- **Dedicated ttbb samples** (예: `TTbb_4f_FxFx`) — 4-flavour scheme
  (4FS) 으로 생성. b-quark mass 를 유지하고 matrix element (ME) 가
  명시적으로 추가 b-quark 를 만듦.

두 sample 은 같은 phase space 영역 (g→bb splitting 으로 추가 b-jet
이 나오는 event) 을 동시에 채운다. 하나의 physics analysis 에 두
sample 을 합치려면 double-counting 을 피하기 위해 **stitching** 이
필요하다: 4FS dedicated sample 에서는 heavy-flavour category 만 취하
고, 5FS inclusive sample 에서는 light-flavour category 만 취해서,
event 당 heavy-flavour content 의 boundary 에서 join 한다.

이 문서가 기술하는 categorization machinery 는 그 boundary 를
정의하는 도구다.

### 1.2 "Additional" 의 정의

이 문서 전체에서 "additional" 은 **top quark decay 에서 나오지 않은**
b-quark / c-quark (그리고 거기서 형성되는 hadron / jet) 를 가리킨다.
Typical fully-hadronic ttbar event 에는 t → bW vertex 에서 나온 두
개의 b-quark 가 있는데, 이것들은 "additional" 이 *아니다*. Parton
shower 에서 gluon splitting 으로 만들어진 세 번째 b-quark 가 있는
event 는 "additional" b-quark 가 한 개 있는 것이다.

Categorization 은 generator-level final-state particle 로부터
clustering 된 gen-jet 위에서 수행되며, 다음의 fixed kinematic
acceptance 를 사용한다:

```
gen-jet pT  >  20 GeV
|gen-jet η| <  2.4
```

이 cut 들은 CMS POG `GenHFHadronMatcher` default [Ref. 2] 와 일치하
며 본 분석 전체에서 사용되는 값이다.

### 1.3 분석에서 어디에 쓰이는가

Per-event ttbar category 는 downstream 에서 세 가지 결정에 쓰인다:

1. **5FS / 4FS sample stitching** — tt+LF / tt+HF (light-flavour vs
   heavy-flavour) boundary 에서. 가장 핵심적인 사용처.
2. **Per-category systematics** — 예를 들어 tt+bb 와 tt+LF 는 b-tag
   working point 가 calibration 된 jet flavour fraction 이 다르므로
   별도의 b-tag SF normalisation 이 필요하다.
3. **DNN background nodes** — multi-class background discriminator 의
   tt+nb output node 는 모든 tt + (additional b-jet) category 의 event
   를 받는다. bbb / 4b 구분을 per-event level 에서 할 필요는 없는데,
   DNN 이 어차피 이들을 같은 node 로 merge 하기 때문이다 (§3.3 참조).

---

## 2. AN reference 와 official algorithm

### 2.1 Authority chain

본 분석이 따르는 categorization logic 의 출처는 다음과 같다:

1. **ttHH AN-2022/122** [Ref. 4], §3.1 (object & event categorization)
   과 §3.4 (sample stitching). 이 섹션들은 CMS POG GenHFHadronMatcher
   → GenTtbarCategorizer plugin chain 을 official categoriser 로
   인용하며, bbb / 4b sub-class 가 per-event 가 아니라 *sample* level
   에서 어떻게 구성되는지 (Option1 / Option2 prescription) 정의한다.
2. **ttH AN-19-094** [Ref. 5], §6.1.2 — 같은 plugin chain 을 사용하는
   더 이른 시기의 ttH 분석. ttHH AN 이 다루지 않는 hadronic channel
   관련 내용에 대한 cross-reference 로 사용한다.
3. **CMS GenTtbarCategorizer plugin source code** [Ref. 1] —
   `genTtbarId` integer encoding 의 ground truth 구현. CMSSW
   repository 의 `TopQuarkAnalysis/TopTools/plugins/GenTtbarCategorizer.cc`
   파일 lines 282–300 에서 확인 가능.
4. **CMS GenHFHadronMatcher TWiki** [Ref. 2] — GenTtbarCategorizer 에
   입력을 공급하는 ghost-clustering 절차에 대한 narrative 문서.

ttHH AN 과 ttH AN 이 충돌하면 ttHH 가 우선한다. AN 과 source code 가
충돌하면 source code 가 우선한다. 이 문서는 그 순서를 따른다.

### 2.2 `genTtbarId` integer

CMS `GenTtbarCategorizer` plugin 은 NanoAOD upstream 에서 동작하는
EDM producer 로서, event 마다 하나의 integer (`genTtbarId`) 를 쓴다.
이 integer 는 event 의 heavy-flavour content 를 packed 형태로 표현
한 값이다. NanoAOD 는 이를 `Events.genTtbarId` 로 노출한다 [Ref. 3].

Plugin source [Ref. 1, lines 282–300] 에서 가져온 decoding rule 은
다음과 같다:

```
genTtbarId % 100   의미
─────────────────  ────────────────────────────────────────────────────
        0          tt + LF      추가 heavy-flavour jet 없음
       41 – 45     tt + cc      ≥1 add c-jet, add b-jet 없음
       51          tt +  b      add b-jet 1개, 그 안에 b-hadron 1개
       52          tt + 2b      add b-jet 1개, 그 안에 b-hadron ≥2개
                                (collinear g→bb splitting 이 한 jet
                                 안에 merge 된 경우)
       53, 54, 55  tt + bb      add b-jet ≥2개
       그 외       not ttbar    ttbar sample 이 아님 (data, signal 등)
```

`genTtbarId` 의 백자리 / 천자리는 c-hadron multiplicity, c-jet count,
top decay product 와의 overlap 등 별도 정보를 담지만, 본 분석에서는
modulo-100 부분만 사용한다.

### 2.3 Plugin 이 알려주지 *못하는* 것

이 문서의 나머지 부분을 결정하는 핵심 관찰:

> **`genTtbarId` 는 add b-jet 의 개수가 2 이상일 때 그 정확한 개수를
> encoding 하지 않는다.** 코드 53, 54, 55 의 차이는 *leading 두
> 개의 b-jet 안의 b-hadron multiplicity* 일 뿐이고, b-jet 개수
> 자체와는 무관하다.

이 사실은 plugin source [Ref. 1, lines 282–300] 에서 직접 확인할 수
있다: 코드 53/54/55 는 *첫 번째* 와 *두 번째* additional b-jet 안에
clustering 된 b-hadron 의 개수를 세어서 만들어지며, "총 add b-jet 이
몇 개인가?" 라는 질문은 답이 ≥2 가 되는 순간 encoding 에서 사라진다.

따라서 **bbb / 4b 구분 (3개 vs ≥4개의 add b-jet) 은 `genTtbarId` 만
으로는 복원할 수 없다**. ttHH AN [Ref. 4] 은 이 문제를 bbb / 4b 를
*sample* level 에서 정의하는 방식으로 해결한다 (§3.4 의 Option1 /
Option2 prescription).

이것이 §3 의 5-category schema 를 정당화하는 핵심 architectural fact
다.

---

## 3. 5-category schema (Option B′)

### 3.1 카테고리

다섯 개의 mutually exclusive event-level category:

| #  | 이름             | AN 대응         | 정의                                                                |
|----|------------------|-----------------|---------------------------------------------------------------------|
| 0  | `LightFlavour`   | tt + LF         | 추가 b- 또는 c-jet 없음                                             |
| 1  | `AddCjet`        | tt + cc         | ≥1 add c-jet, add b-jet 없음                                        |
| 2  | `Add1Bjet_1Had`  | tt + b          | add b-jet 정확히 1개, 그 안에 b-hadron 정확히 1개                   |
| 3  | `Add1Bjet_2Had`  | tt + 2b         | add b-jet 정확히 1개, 그 안에 b-hadron ≥2개 (collinear g→bb)        |
| 4  | `Add2Bjet`       | tt + bb (∪ bbb ∪ 4b) | add b-jet ≥2개 (AN 의 bb / bbb / 4b 를 sample level 에서 모두 포함) |

ttbar 가 아닌 event 는 sentinel `NoTTJets` 로 mapping. GenPart /
GenJet input 이 없는 event (예: data NanoAOD) 는 sentinel `Unknown`
으로 mapping.

### 3.2 Naming convention 의 근거

이름에는 historical 한 `tt+b` / `tt+2b` 표기 대신 명시적 `Bjet` 과
`Had` suffix 를 사용했다. 이유는 AN 의 "2b" 표기가 매우 ambiguous
하기 때문이다 — "2 개의 b-jet" 으로 읽을 수도 있고 (틀림: 1 개의
b-jet) "2 개의 b-hadron" 으로 읽을 수도 있다 (맞음). `Add1Bjet_2Had`
이라는 이름은 add b-jet 이 **하나** 있고 그 안에 b-hadron 이 **두**
개 있다는 것을 누구나 즉시 보게 만들어, code 를 읽을 때 가장 흔한
혼동의 원인을 제거한다.

### 3.3 왜 5 개이고 7 개가 아닌가

세 가지 독립적인 이유가 모두 같은 결론을 가리킨다:

1. **Plugin 이 구분하지 못한다.** §2.3 에서 보였듯이
   GenTtbarCategorizer integer 는 add b-jet 의 개수가 ≥2 일 때 그
   개수를 encoding 하지 않는다. AN 의 tt+bb 를 bb / bbb / 4b 로
   분리하려면 AN 이 인용하는 도구 *너머* 의 re-implementation 이
   필요한데, 그런 re-implementation 은 어떤 형태로든 AN 의 정의와
   일치하지 않게 된다.

2. **AN 은 bbb / 4b 를 sample level 에서 정의한다.** ttHH AN §3.4
   [Ref. 4] 는 bbb / 4b prescription (Option1 / Option2) 을 어느 4FS
   sample 을 쓰고 그 sample 의 generator-level filter 가 무엇인가에
   기반해 정의하지, per-event GenHFHadronMatcher labelling 으로 정
   의하지 않는다. 이 resolution 의 per-event categorization 은 AN 이
   요구하는 것이 아니다.

3. **Downstream DNN 이 어차피 merge 한다.** ttHH multi-class
   background DNN 은 tt+b, tt+2b, tt+bb, tt+bbb, tt+4b 를 모두 받는
   하나의 tt+nb output node 를 가진다. event level 에서 bbb 와 4b 를
   구분해도 분석 차원의 이득이 없다.

따라서 5-category schema 는 *필요하고* (integer 가 더 이상 알려주지
않으므로) *충분하다* (AN 과 분석 디자인이 더 이상 요구하지 않으므로).

---

## 4. 두 algorithm, 두 branch set

### 4.1 두 algorithm

**Algorithm A — POG path (`decode_genttbarid`)**

NanoAOD 의 `genTtbarId` integer 를 읽고 §2.2 의 표를 사용해
`genTtbarId % 100` 을 5 개 category 중 하나로 mapping. Stateless 한
~10 줄짜리 함수. AN [Ref. 4, §3.1] 이 official method 로 인용하는
것이 이 path 다. `genTtbarId` 가 없는 data NanoAOD 에서는 `NoTTJets`
를 반환 (source code 3 = `NO_GENTTBARID`).

**Algorithm B — GenPart path (`_categorize_genpart_xval`)**

같은 5-category label 을 `genTtbarId` 를 건드리지 않고 raw GenPart 와
GenJet 정보로부터 re-derive 한다. 절차:

1. 모든 `GenPart` entry 를 walking 하면서 b-hadron 인 것 (digit
   pattern 에 대한 PDG-ID test), last copy 인 것 (status flag bit 13
   set), mother chain 30 generation 안에 top quark 가 **없는** 것을
   수집.
2. 모든 `GenJet` entry 를 walking 하면서 `hadronFlavour == 5`,
   `pT > 20 GeV`, `|η| < 2.4` 인 것을 수집. 이것이 acceptance 안의
   "additional gen b-jet" 들이다.
3. 각 additional b-hadron 에 대해 ΔR < 0.4 안에서 가장 가까운 gen
   b-jet 을 찾는다. Match 가 성공하면 그 jet 의 b-hadron count 를
   1 증가. 끝에서 `len(jet_bh_map)` 이 add b-jet 의 개수 (`nBJets`)
   이고, value 들이 per-jet b-hadron count 이다.
4. Decision tree 적용:
   ```
   nBJets >= 2          → kAdd2Bjet
   nBJets == 1, 1 had   → kAdd1Bjet_1Had
   nBJets == 1, ≥2 had  → kAdd1Bjet_2Had
   nBJets == 0          → add c-jet 검사 (PDG-ID 4 hadron 과
                          hadronFlavour==4 에 대해 같은 절차)
                          matched 가 있으면 → kAddCjet
                          없으면              → kLightFlavour
   ```

이 algorithm 은 POG ghost-clustering 과는 다른 strategy 를 쓴다 —
POG 가 jet clustering 자체에 b-hadron 을 ghost 로 embed 하는 반면,
우리는 ordinary gen-jet 에 대해 사후 ΔR-matching 을 한다. 따라서 두
path 는 같은 양에 대한 **진짜로 독립적인 estimator** 다. 두 path 의
일치도는 heavy-flavour signal sample 에서는 높고 (>97%) light-flavour
가 dominant 한 inclusive ttbar sample 에서는 낮다 (~73%). 이
disagreement 는 GenPart walker 의 알려진 weakness 에서 온다 — §6
참조.

### 4.2 두 branch set

NtupleForge 는 두 algorithm 의 결과를 모두 slimmed ntuple 에 두 개의
parallel branch namespace 로 쓴다:

```
Primary set (genTtbarId path)         Cross-check set (GenPart path)
─────────────────────────────────     ──────────────────────────────────
ttCat_LightFlavour     : Bool         ttCatXval_LightFlavour    : Bool
ttCat_AddCjet          : Bool         ttCatXval_AddCjet         : Bool
ttCat_Add1Bjet_1Had    : Bool         ttCatXval_Add1Bjet_1Had   : Bool
ttCat_Add1Bjet_2Had    : Bool         ttCatXval_Add1Bjet_2Had   : Bool
ttCat_Add2Bjet         : Bool         ttCatXval_Add2Bjet        : Bool
ttCatSource            : Int          ttCatXvalSource           : Int
```

총 12 개 branch / event. 어떤 ttbar event 에서도 각 set 안에서
정확히 하나의 Bool branch 만 set 된다. 비-ttbar event 에서는 다섯
개가 모두 False 이고 source integer 가 그 이유를 알려준다.

`ttCatSource` / `ttCatXvalSource` integer 의 의미:

| Source | 의미 |
|--------|------|
| 0      | `GENTTBARID` — branch set 이 genTtbarId path 에서 채워짐 |
| 0      | `GENPART`    — branch set 이 GenPart path 에서 채워짐    |
| 2      | `NO_TTBAR`   — event 에 ttbar pair 없음, 다섯 Bool 모두 False |
| 3      | `NO_GENTTBARID` / `NO_GENINFO` — 필요한 input 부재 (data) |

두 namespace 의 source code "0" 이 서로 다른 algorithm 을 가리키는
것은 의도된 것이며, NtupleForge README [Ref. 6] 에 명시되어 있다.

### 4.3 다운스트림은 어느 것을 쓰는가

**항상 `ttCat_*` (genTtbarId path).** 이유:

- AN [Ref. 4, §3.1] 이 인용하는 것이 이 path 이다.
- 모든 ttbar 관련 CMS 분석이 사용하는 official CMS plugin chain
  GenHFHadronMatcher → GenTtbarCategorizer 의 출력이다.
- POG 가 내부적으로 사용하는 ghost-clustering 절차 [Ref. 2] 가
  NanoAOD 가 적용하는 parton-history pruning 에 대해 GenPart walker
  보다 robust 하다.

Cross-check set `ttCatXval_*` 는 어떤 downstream physics 결정에서도
**절대** 읽지 않는다. 그것의 유일한 목적은 `ttCat_*` (그리고 두
algorithm 을 모두 다시 계산하는 analyzer 의 결과 — §5 참조) 와 비교
되어, 두 implementation 사이의 drift 를 physics 에 영향을 주기 전에
detect 하는 것이다. Architectural rule:

> **`ttCatXval_*` 를 제거해도 단 하나의 physics 결과도 바뀌지 않는다.**

이 rule 이 깨진다면 validation infrastructure 가 잘못 사용되고 있는
것이다.

---

## 5. Four-way validation

### 5.1 네 개의 estimator

C++ analyzer 와 Python ntuplizer 사이의 logic drift 를 가능한 빨리
detect 하기 위해, analyzer 는 매 MC event 마다 같은 per-event label
에 대한 **네 개의 독립적인 estimator** 를 계산하거나 읽는다:

| 이름          | 출처                            | Algorithm                                 |
|---------------|---------------------------------|-------------------------------------------|
| `ANA_GENPART` | analyzer C++                    | 자체 GenPart algorithm                    |
| `ANA_GENID`   | analyzer C++                    | `genTtbarId%100` decode                   |
| `NTU_PRIMARY` | ntuple `ttCat_*` branches       | ntuplizer 의 `decode_genttbarid` (Python) |
| `NTU_XVAL`    | ntuple `ttCatXval_*` branches   | ntuplizer 의 GenPart algorithm (Python)   |

C++ analyzer 는 어떤 single branch 만 읽고 멈추는 일이 없다. 항상 두
ANA_* 값을 처음부터 계산하고 두 NTU_* 값을 ntuple 에서 읽은 다음 6
개 pair 를 모두 비교한다.

### 5.2 두 개의 must-agree pair

C(4,2) = 6 개의 pair 중에서 두 개는 *byte 단위로 동등할 수밖에 없는*
구조다:

```
ANA_GENID   ≡  NTU_PRIMARY    (둘 다 같은 integer 를 decode)
ANA_GENPART ≡  NTU_XVAL       (같은 algorithm 을 두 언어로 작성)
```

이 두 pair 중 어느 한쪽이라도 disagree 한다면 그것은 physics effect
가 아니라 regression 이다. "같은 integer 를 decode" 하는 두 구현이
event level 에서 disagree 할 수 있는 의미 있는 시나리오는 없다 —
disagree 하면 그건 bug 다. 두 GenPart 구현 (analyzer C++, ntuplizer
Python) 도 §4.1 의 recipe 를 step-by-step 똑같이 따르므로 byte-level
agreement 는 목표가 아니라 success criterion 이다.

나머지 4 개 pair 는 *informational* 이다 — POG ghost-clustering 과
우리 GenPart walker 사이의 진짜 algorithmic 차이를 반영한다. 이들의
일치도는 sample composition 에 따라 달라지며 §6 의 주제이다.

### 5.3 Validation output

매 MC event 마다 analyzer 는:

1. 그 event 의 GenPart / GenJet vector 와 `genTtbarId` integer 로부터
   `ANA_GENPART` 와 `ANA_GENID` 를 직접 계산.
2. `ttCat_*` 에서 `NTU_PRIMARY` 를, `ttCatXval_*` 에서 `NTU_XVAL` 을
   읽음.
3. 4 개의 1D count histogram (estimator 당 하나) 과 6 개의 2D
   confusion-matrix histogram (pair 당 하나) 을 output ROOT file 의
   `TtCatValidation/` directory 에 채움.
4. `genTtbarId%100` vs `ANA_GENPART` 의 60-bin × 5-bin TH2 를 채워서
   anomalous POG code 를 spotting 할 수 있게 함.
5. 첫 20 개 event 에 대해, 그리고 그 이후 must-agree pair 가 disagree
   하거나 category 가 heavy-flavour 인 event 에 대해 (최대 100 event
   까지), 네 label 을 나란히 보여주는 per-event debug dump 를 stdout
   에 출력. Per-jet b-hadron map 과 `MUST-AGREE BROKEN` 명시 marker
   포함.

Job 종료 시점에 analyzer 는 summary 를 출력한다: estimator 별
event count, pair 별 agreement 백분율 (must-agree pair 의 모든
off-diagonal cell 을 명시적으로 listing), informational pair 의
breakdown.

### 5.4 TTToHadronic 검증 결과

2017 NanoAODv9 UL TTToHadronic sample 전체를 돌린 결과 (1,280,000
events processed):

```
[ttCatSummary] --- MUST-AGREE PAIRS (any disagreement = bug) ---
ANA_GENID   vs NTU_PRIMARY    agree=1,280,000   disagree=0   100.0000%
ANA_GENPART vs NTU_XVAL       agree=1,280,000   disagree=0   100.0000%

[ttCatSummary] --- INFORMATIONAL PAIRS (real algorithmic diff) ---
ANA_GENPART vs ANA_GENID      agree=  929,370   disagree=350,630   72.6070%
ANA_GENPART vs NTU_PRIMARY    agree=  929,370   disagree=350,630   72.6070%
ANA_GENID   vs NTU_XVAL       agree=  929,370   disagree=350,630   72.6070%
NTU_PRIMARY vs NTU_XVAL       agree=  929,370   disagree=350,630   72.6070%
```

두 must-agree pair 가 모두 perfect (1.28 M events 에서 off-diagonal
cell 0 개). 4 개의 informational pair 가 모두 *정확히 같은* count 를
보인다: 929,370 / 350,630. 이 자체가 강력한 self-consistency check
이다 — `ANA_GENID ≡ NTU_PRIMARY` 와 `ANA_GENPART ≡ NTU_XVAL` 이 동시
에 정확히 성립할 때만 4 개 pair 가 두 개의 동치류 (equivalence
class) 로 partition 되어 똑같은 confusion matrix 를 만들 수 있다.

**결론: ntuplizer ↔ analyzer parity 가 1.28 M event 위에서 byte
level 로 확립되었다.**

---

## 6. POG vs GenPart 차이

### 6.1 관찰된 패턴

§5.4 의 4 개 informational pair 가 ~73% 일치를 보이는데, 이는
TTHHTo4b signal sample 에서 이전에 관찰된 ~97% 보다 훨씬 낮다.
Disagreement 는 다음과 같은 event 에 집중되어 있다:

- `genTtbarId % 100 == 0` (POG 가 추가 heavy flavour 없다고 말함)
- GenPart algorithm 이 1~2 개의 add b-jet 을 발견

즉, POG 가 기록하지 않은 b-jet activity 를 GenPart algorithm 이 보고
있다. 관찰된 모든 mismatch 의 방향이 동일하다: GenPart 가 POG 보다
*더 많은* add b-jet 을 찾고, 그 반대는 없다.

### 6.2 왜 이렇게 되는가

두 가지 기여 factor:

1. **Pruned GenPart history.** NanoAOD 는 full Pythia history 가
   아니라 `prunedGenParticles` 를 저장한다. Mother-chain walker
   `_has_top_ancestor` 는 `GenPart_genPartIdxMother` 를 30 generation
   까지 따라가며 top quark 를 찾는데, chain 이 pruning 되어 있으면
   (parton-shower hadron 의 intermediate state 가 dropped 되는 typical
   case) walker 는 top 을 못 찾고 b-hadron 을 "additional" 로
   labelling 한다 — 사실은 top decay 에서 온 것인데도.

2. **POG 는 ΔR matching 이 아니라 ghost-clustering 을 쓴다.**
   GenHFHadronMatcher 절차 [Ref. 2] 는 b-hadron 을 zero-momentum
   "ghost" 입자로 jet clustering 자체에 embed 하기 때문에, b-hadron 과
   jet 의 association 이 simple ΔR < 0.4 cone 이 아니라 full jet
   algorithm (recombination 과 area effect 포함) 을 사용한다. Top
   decay 에서 나온 b-jet 이 g → bb splitting 에서 나온 add b-jet 과
   overlap 하는 topology 에서 특히 중요하다.

이 두 effect 가 결합하여, parton history 가 짧은 event (TTToHadronic
같은 inclusive ttbar sample 에서 typical) 에서 POG 는 top decay
b-jet 을 정확히 식별하는데 GenPart walker 는 그것을 additional 로
mis-labelling 한다.

### 6.3 왜 이것이 문제가 아닌가

세 가지 이유:

1. **Downstream physics 가 POG path 를 사용한다.** ~27% 의
   disagreement 는 어떤 histogram cut, weight, stitching boundary 에도
   도달하지 않는다.
2. **AN 이 POG 를 인용한다.** ttHH AN [Ref. 4, §3.1] 은 official
   tool 로 명시적으로 GenHFHadronMatcher / GenTtbarCategorizer chain
   을 인용한다. 우리는 AN 이 말하는 것을 따른다.
3. **이 disagreement 는 우리 코드의 bug 가 아니라 NanoAOD GenPart 의
   알려진 property 다.** GenPart re-implementation 과 POG plugin 은
   같은 질문에 다른 input (pruned GenPart vs full ghost-clustering) 으
   로 답하는 것이고, 모든 case 에서 같은 답이 나올 것을 기대하지
   않는다.

미래의 어느 버전에서 분석의 primary path 를 POG 에서 *벗어나게*
바꾼다면 (현재는 계획 없음), GenPart walker 를 개선해야 할 것이다.
가능한 개선:

- Mother-chain walk depth 를 30 → 50 으로 증가.
- "Additional" 선언 전에 `isHardProcess` 와 `fromHardProcess` status
  flag 를 cross-check.
- Last-copy 대신 first-copy ancestor 를 따라감.
- Ghost-augmented `GenJetAK4` collection 을 사용하는 ghost-clustering
  으로 전환 (이 경우 자체 jet clustering 을 off-line 으로 돌려야 함).

본 분석에서는 이들 중 어떤 것도 필요하지 않다.

---

## 7. 기술적 구현

### 7.1 NtupleForge (Python)

**File:** `modules/ttbarCategorizer.py` (~970 lines). 운영 문서는
[Ref. 6] 에 있음.

핵심 entry point 와 helper:

| Symbol | 역할 |
|--------|------|
| `TtbarCategorizer` (class) | event 마다 한 번 실행되는 NanoAOD-tools `Module` subclass |
| `decode_genttbarid(int)` | Algorithm A — `genTtbarId%100` 의 5-category mapping |
| `_categorize_genpart_xval(event, has_genpart, has_genjet)` | Algorithm B — GenPart 기반 re-derivation, 5 category 이름 중 하나 또는 `None` 반환 |
| `_is_b_hadron(pdgId)` / `_is_c_hadron(pdgId)` | Meson / baryon digit pattern 에 대한 PDG-ID test |
| `_has_top_ancestor(event, idx, nGP)` | 30-generation mother-chain walk, `abs(pdgId)==6` 검색 |
| `_delta_r2(η1, φ1, η2, φ2)` | φ wrap-around 처리된 ΔR² |
| `make_default_module()` + `MODULES` | `run_postproc.py` driver 가 사용하는 factory |

Module 의 `analyze(event)` method:

1. `beginFile` 에서 branch availability 를 eager check (이전의 lazy
   `hasattr` 방식이 data NanoAOD 에서 `RuntimeError` 를 raise 했던
   문제를 대체).
2. `decode_genttbarid()` 를 실행하고 5 개의 `ttCat_*` Bool branch +
   `ttCatSource` integer 를 씀.
3. `_categorize_genpart_xval()` 를 실행하고 5 개의 `ttCatXval_*` Bool
   branch + `ttCatXvalSource` integer 를 씀.
4. `--ttcat-debug-csv` 가 set 되어 있으면 (default off) `ttcat_debug.csv`
   에 한 줄 append.

두 algorithm 은 매 ttbar event 에서 모든 mode 에 대해 독립적이고
무조건적으로 실행된다. 한 쪽을 skip 하는 fast path 는 없다.

**상수** (single source of truth, `modules/ttbarCategorizer.py`):

```python
GEN_JET_PT_MIN  = 20.0    # GeV
GEN_JET_ETA_MAX = 2.4
DR_MATCH_MAX    = 0.4
```

**Compatibility helper** (`modules/_nanoaod_compat.py`):

- `to_int(x)` — `event.GenJet_hadronFlavour[j]` 가 integer 가 아닌
  `bytes` object 를 반환하는 NanoAOD `UChar_t` quirk 를 처리.
- `safe_len(arr, branch_name)` — `__len__` 을 구현하지 않는 raw
  `TTreeReaderArray` object 를 처리하며, `GetSize()` fallback 을 가짐.

### 7.2 Analyzer (C++)

**Files:** `ttHHanalyzer_unified.h`, `ttHHanalyzer_unified.cc`. 운영
문서는 [Ref. 7] 에 있음.

**Enum** (`ttHHanalyzer_unified.h`):

```cpp
enum class TtCat {
    kLightFlavour, kAddCjet, kAdd1Bjet1Had, kAdd1Bjet2Had,
    kAdd2Bjet, kNoTTJets, kUnknown, kNCategories
};
```

**Categorization API** (`ttHHanalyzer_unified.h`):

| Method | Algorithm | 읽는 것 |
|--------|-----------|---------|
| `computeTtCategoryFromGenPart()` | 자체 GenPart walker | `GenPart_*`, `GenJet_*` |
| `computeTtCategoryFromGenTtbarId()` | `genTtbarId%100` decode | `genTtbarId` |
| `readNtuplePrimaryCategory()` | 없음 — pure read | `ttCat_*`, `ttCatSource` |
| `readNtupleXvalCategory()` | 없음 — pure read | `ttCatXval_*`, `ttCatXvalSource` |
| `officialTtCategory()` | `readNtuplePrimaryCategory()` 의 wrapper | 위와 동일 |

GenPart walker 는 `ttbarCategorizer.py` 의 Python 구현과 byte
equivalent 다: 같은 30-generation mother walk, 같은 ΔR² < 0.16
matching, 같은 `isLastCopy` bit test (`statusFlags` 의 bit 13), 같은
GEN_JET_PT_MIN / GEN_JET_ETA_MAX 상수.

`ttHHanalyzer_unified.cc` 의 `process()` 안의 **validation block** —
`if (_DataOrMC != "Data")` 로 guarded:

1. 첫 event 의 branch availability dump (`[TTCAT_BRANCH_CHECK]`).
2. 4 개 estimator 계산.
3. Per-event debug print (첫 20 event, 그리고 그 이후 heavy-flavour
   또는 must-agree-broken event 에 대해 100 event 까지).
4. `TtCatValidation/` 에 4 + 6 + 1 = 11 개 histogram fill.

Header 의 private 섹션 (라인 ~1650-1690) 에 있는 **histogram declaration**:

```cpp
TH1F* _hTtCat_Counts_AnaGenPart  = nullptr;
TH1F* _hTtCat_Counts_AnaGenId    = nullptr;
TH1F* _hTtCat_Counts_NtuPrimary  = nullptr;
TH1F* _hTtCat_Counts_NtuXval     = nullptr;

TH2F* _hTtCat_AnaGenPart_vs_AnaGenId    = nullptr;
TH2F* _hTtCat_AnaGenPart_vs_NtuPrimary  = nullptr;
TH2F* _hTtCat_AnaGenPart_vs_NtuXval     = nullptr;  // MUST-AGREE
TH2F* _hTtCat_AnaGenId_vs_NtuPrimary    = nullptr;  // MUST-AGREE
TH2F* _hTtCat_AnaGenId_vs_NtuXval       = nullptr;
TH2F* _hTtCat_NtuPrimary_vs_NtuXval     = nullptr;

TH2F* _hTtCat_GenTtbarIdMod100          = nullptr;
```

`endjob()` 의 **end-of-job summary** 는 11 개 histogram 을 write 하고,
must-agree pair 의 off-diagonal 을 명시적으로 listing 하면서 pair 별
agreement 통계를 print.

### 7.3 Plotting

**File:** `scripts/plot_ttcat_validation.py` (PyROOT, 외부 의존성 없음).

Analyzer output ROOT file 의 `TtCatValidation/` 를 읽고 11 개
histogram 을 모두 PNG 로 render. Must-agree pair 는 plot title 에
auto-mark (`MUST-AGREE OK` / `MUST-AGREE BROKEN`). 개별 plot 과
4 × 3 one-page summary grid 모두 생성.

CLI:
```bash
python scripts/plot_ttcat_validation.py output.root [-o plots/]
       [--normalize {none,row,col}] [--log-counts]
```

### 7.4 Treestream / eventBuffer

Analyzer 가 사용하는 `eventBuffer.h` header 는 `variables.txt` 파일
에서 treestream 이 auto-generate 한다. 12 개의 ttCat 관련 branch 가
그 파일에 listed 되어야 함:

```
Bool_t  ttCat_LightFlavour
Bool_t  ttCat_AddCjet
Bool_t  ttCat_Add1Bjet_1Had
Bool_t  ttCat_Add1Bjet_2Had
Bool_t  ttCat_Add2Bjet
Int_t   ttCatSource
Bool_t  ttCatXval_LightFlavour
Bool_t  ttCatXval_AddCjet
Bool_t  ttCatXval_Add1Bjet_1Had
Bool_t  ttCatXval_Add1Bjet_2Had
Bool_t  ttCatXval_Add2Bjet
Int_t   ttCatXvalSource
```

`eventBuffer.h` 자체는 직접 편집되지 않는다. Analyzer 는 treestream
이 emit 한 그대로 `_ev->ttCat_LightFlavour` 등을 통해 branch 에
접근.

---

## 8. 운영 노트

### 8.1 Validation 실행 절차

새 ntuple production 마다 representative ttbar sample (typical 하게는
inclusive 에 대해 TTToHadronic, signal 에 대해 TTHHTo4b) 위에서
analyzer 를 돌리고 다음 세 가지를 확인:

1. **stdout** — end-of-job summary 에서 두 must-agree pair 가 모두
   `100.0000%` 와 `disagree=0` 을 report.
2. **`TtCatValidation/` ROOT histogram** — 두 must-agree pair plot 이
   모두 diagonal-only.
3. **`scripts/plot_ttcat_validation.py`** —
   `pair_AnaGenId_vs_NtuPrimary.png` 와 `pair_AnaGenPart_vs_NtuXval.png`
   가 title 에 `MUST-AGREE OK` 표시.

이 세 check 중 하나라도 실패하면 parity 가 복원될 때까지 그 production
sample 을 사용하지 말 것.

### 8.2 Known TODOs

- Branch 이름 `ttCatXval_*` (`Xval` = "cross-validation") 는 이 문서를
  읽지 않은 사람에게는 opaque. Treestream / analyzer 와 coordinate
  하여 다음 major refactor 에서 더 descriptive 한 이름 (예:
  `ttCatGenPart_*`) 으로 rename 예정.
- Analyzer 에 `validateTtCat` runtime flag 를 추가 (default `false`).
  Flag off 일 때 production run 은 4-way comparison 을 skip 하고
  `officialTtCategory()` 만 호출하여 per-event GenPart loop 비용을
  recover. Flag on 일 때 (새 ntuple production 검증, 또는
  categorizer code 를 건드린 후) 전체 비교 실행.
- Legacy `TTCatDebug.h` header (이전 validation era 의 stand-alone
  CSV-diff helper) 는 현재 analyzer 에서 사용되지 않으며 다음 cleanup
  pass 에서 `legacy/` sub-directory 로 이동하거나 제거할 것.

### 8.3 하지 말아야 할 것

- **Production 용으로 analyzer 에서 categorization 을 다시 구현하지
  말 것.** Analyzer 의 `computeTtCategoryFromGenPart()` 와
  `computeTtCategoryFromGenTtbarId()` 는 validation 전용이다.
  Production 결정은 `officialTtCategory()` 를 통해 가야 하고, 이것은
  타협 불가능.
- **`eventBuffer.h` 를 직접 수정하지 말 것.** Branch 추가/제거는
  `variables.txt` 에서 한 다음 treestream 을 다시 돌릴 것.
- **GEN_JET_PT_MIN, GEN_JET_ETA_MAX, DR_MATCH_MAX 상수를 silent
  하게 바꾸지 말 것.** 이들은 두 곳 (Python 과 C++) 에 등장하므로
  바꾸려면 양쪽 모두 update 해야 한다. 만약 상수가 바뀌면 `ttCat*_`
  branch 가 그것에서 derive 되었으므로 ntuple 도 다시 production
  해야 한다.

---

## References

1. **CMS `GenTtbarCategorizer` plugin source code.**
   `TopQuarkAnalysis/TopTools/plugins/GenTtbarCategorizer.cc`, CMSSW
   official repository.
   <https://github.com/cms-sw/cmssw/blob/master/TopQuarkAnalysis/TopTools/plugins/GenTtbarCategorizer.cc>
   `decode_genttbarid()` 가 사용하는 `genTtbarId` integer encoding rule
   은 이 파일의 lines ~282–300 에 구현되어 있다.
   *Cited in: §2.1, §2.2, §2.3.*

2. **CMS Public TWiki — GenHFHadronMatcher.**
   "Generator-level identification of heavy-flavour jets in top-quark
   events", CMS Public Wiki.
   <https://twiki.cern.ch/twiki/bin/view/CMSPublic/GenHFHadronMatcher>
   GenTtbarCategorizer 에 input 을 공급하는 ghost-clustering 절차의
   narrative 문서. 본 분석 전체에서 사용하는 gen-jet acceptance
   default (pT > 20 GeV, |η| < 2.4) 의 출처.
   *Cited in: §1.2, §2.1, §4.3, §6.2.*

3. **CMS Internal TWiki — NanoAOD ttbar generator-level info.**
   "Top quark Monte Carlo generator information in NanoAOD",
   `CMS.TopModGen`, CMS Internal Wiki.
   <https://twiki.cern.ch/twiki/bin/view/CMS/TopModGen>
   NanoAODv9 UltraLegacy 에서 노출되는 `genTtbarId` branch 와 그것을
   채우는 upstream EDM producer 에 대한 문서.
   *Cited in: §2.2 (NanoAOD branch source).*

4. **ttHH analysis note (primary).**
   CMS AN-2022/122, *"Search for ttHH production in the four-bottom
   final state"*, version v26.
   본 분석이 인용하는 primary AN. 이 문서와 관련된 섹션은 §3.1
   (object & event categorization), §3.2 (categorization 에 입력을
   제공하는 selection definition), §3.4 (5FS / 4FS sample stitching,
   bbb / 4b sample-level 구성을 위한 Option1 / Option2 prescription).
   Local files: `ttHH_AN_AN2022_122_v26-1-151.pdf`,
   `ttHH_AN_AN2022_122_v26-152-253.pdf`.
   *Cited in: §2.1, §2.3, §3.3, §4.3, §6.3.*

5. **ttH analysis note (reference).**
   CMS AN-2019/094, *"Search for ttH production with H → bb"*,
   version v20.
   같은 plugin chain 을 사용하는 더 이른 시기의 ttH 분석. ttHH AN 이
   다루지 않는 hadronic channel 관련 내용에 대한 cross-reference 로
   사용. 관련 섹션은 §6.1.2 (ttH(H→bb) 분석의 ttbar+jets categorization).
   Local files: `AN2019_094_v20_ttHAnalysis5494.pdf`,
   `AN2019_094_v20_ttHAnalysis95145.pdf`,
   `AN2019_094_v20_ttHAnalysis-146-301.pdf`,
   `AN2019_094_v20_ttHAnalysis302333.pdf`,
   `AN2019_094_v20_ttHAnalysis344398.pdf`.
   *Cited in: §2.1.*

6. **NtupleForge README.** `README_ntuplizer.md`, NtupleForge
   repository, 섹션 "tt+jets Event Categorizer", "References", "Future
   TODO". Ntuplizer 측 categorization module 과 그것이 쓰는 12 개의
   ttCat 관련 branch 에 대한 canonical operational 문서.
   *Cited in: §7.1 (operational details), §8.2 (TODO list cross-link).*

7. **ttHH Analyzer README.** `README_analyzer.md`, ttHH Analyzer
   repository, 섹션 "tt+jets Event Categorization & 4-way Validation".
   Analyzer 측 validation block, `TtCatValidation/` ROOT histogram,
   `scripts/plot_ttcat_validation.py` plotter 에 대한 canonical
   operational 문서.
   *Cited in: §5.3 (validation output), §7.2–§7.3 (implementation),
   §8.1 (validation procedure).*

---

*문서 끝.*
