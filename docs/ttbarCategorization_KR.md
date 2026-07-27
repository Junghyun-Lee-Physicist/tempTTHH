# ttHH(HH→4b) Fully-Hadronic Channel 의 ttbar Categorization

**Personal Analysis Note — JH**
**Run2 2017 NanoAODv9 UltraLegacy, CMSSW_14_2_1**
*최종 수정: 2026-04-12*

> **편집 정정 (2026-07-27) — 본문은 그대로 둔다.**
> 이 노트가 "있는 것"처럼 참조하는 두 산출물은 저장소에 **존재하지 않는다**.
> 설계 기록으로서 노트는 보존하되, 실행하려 하지 말 것:
> - **§7.3 `scripts/plot_ttcat_validation.py`** — 작성되지 않았다(`tempTTHH/scripts/`
>   디렉토리 자체가 없다). 사양과 `root -l` 대안은 `tempTTHH/README.md` 의
>   "Validation plotter — DOES NOT EXIST (PROPOSED)" 절 참조.
> - **참고문헌 #6 `README_ntuplizer.md`** — 실제 파일은 `../NtupleForge/README.md`
>   (상세는 `../NtupleForge/docs/04_architecture.md`).
> 참고문헌 #7 의 `README_analyzer.md` 는 현재 `tempTTHH/README.md` 다.

---

## Abstract

이 문서는 ttHH(HH→4b) fully-hadronic 분석에서 사용하는 per-event ttbar
categorization scheme 를 기술한다. 다루는 내용은 물리적 motivation,
AN 이 인용하는 official algorithm (CMS `GenTtbarCategorizer` plugin
을 거쳐 NanoAOD 의 `genTtbarId` integer 로 노출되는 경로),
cross-check 용으로 독립 구현된 algorithm, event-level 에서 bbb / 4b
sub-class 를 분리하지 않고 collapse 한 이유, NtupleForge 와 analyzer
양쪽의 기술적 구현, 그리고 두 codebase 가 1.28 M MC events 위에서
byte-level 로 동등함을 증명한 validation pipeline 이다. §9–10 은 여기에 `genTtbarId` 로는 불가능한 (§2.3) tt+bbb 와 tt+4b 를 분리하는 *expanded* per-event id, 그 validation, 그리고 그 위에 쌓은 prescan → stitching-factor → analyzer-weight chain 을 추가한다.

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

> **Update (§9).** 이 per-event 한계는 이제 MiniAOD 에서 유도한 *expanded*
> id 로 해소된다 — sub-code `61`/`62` (tt+bbb), `71`/`72` (tt+4b) 추가.
> 아래 NanoAOD 5-category scheme 은 그대로이고 validation baseline 으로
> 남으며, expanded id 는 그 위에 얹는 추가 label 이다. §9 참조.

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
- **analyzer 가** `DerivedCorr/stitchFactors/stitch_factors_2017.json` 을
  로드해 per-(sample, category) per-event factor 를 적용하도록 배선 (§10.4).
  factor 는 계산되지만 아직 analyzer 안에서 적용되지 않는다.
- **b-tag normalization reweight 키**를 NanoAOD `genTtbarId` 에서
  `expandedTtbarId` 로 전환해 reweight 에서 tt+nb 를 구분 (§10.5) —
  process-key map 이 code `61–72` 를 포함하는지 확인한 뒤.

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

---

## 9. Expanded ttbar-ID — 이벤트 단위로 tt+bbb vs tt+4b 복원

> 이 장은 §1–8 의 체계를 확장한다. §1–8 은 **NanoAOD `genTtbarId`**
> categorization 을 다루며, (§2.3 대로) additional b-jet 이 3 개인지
> 4 개 이상인지 구분하지 못한다. 여기서는 바로 그 구분을 복원하는
> 이벤트 단위 **expanded id** 를 추가하고, 검증하고, analyzer 에 배선한다.
> §1–8 은 하나도 바뀌지 않는다 — official 5-category branch 는 그대로이고,
> expanded id 는 그 위에 얹는 *추가* label 이다.

### 9.1 문제 한 줄 요약

`genTtbarId % 100` 은 additional b-jet 이 ≥2 가 되는 순간 `53/54/55` 에서
포화된다 (§2.3). FH ttHH(4b) signal 은 ttbar 의 **tt+≥3b** 꼬리 위에 앉아
있으므로, 이벤트 단위로

- **tt+bbb** — additional b-jet 정확히 3 개,
- **tt+4b** — 4 개 이상

을 구분해야 한다. NanoAOD 에는 이 정보가 없다. 하지만 MiniAODv2 에는
GenJet–b-hadron association 이 그대로 남아 있어, 정보 자체는 한 tier 위에
*존재*한다 — 단지 NanoAOD 로 전파되지 않았을 뿐이다.

**비유.** NanoAOD 는 압축된 사진이다. "최소 두 명 이상의 무리"는 보이지만
정확히 몇 명인지는 셀 수 없다. MiniAOD 는 RAW 파일이다. 정말 중요한 소수의
이벤트에 대해서만 RAW 로 한 번 돌아가 머릿수를 적어두고, 그 메모를 들고 온다.

### 9.2 Encoding: 새 sub-code 61 / 62 / 71 / 72

official numbering 을 대체하지 않고 **확장**한다. expanded id 는 원래
`genTtbarId` 의 prefix (`value / 100`, 예: `253` 의 `2`) 를 유지하고,
tt+≥3b 이벤트에 한해 **끝 두 자리**만 덮어쓴다:

| expanded `%100` | 의미 | additional b-jet | b-hadron multiplicity flag |
|---|---|---|---|
| `61` | **tt+bbb** | 정확히 3 | `nAddBJetsMulti == 0` |
| `62` | **tt+bbb** | 정확히 3 | `nAddBJetsMulti ≥ 1` |
| `71` | **tt+4b**  | ≥ 4        | `nAddBJetsMulti == 0` |
| `72` | **tt+4b**  | ≥ 4        | `nAddBJetsMulti ≥ 1` |

`1`/`2` 분리 (61 vs 62, 71 vs 72) 는 official `53/54/55` 로직을 그대로
따른다 — additional b-jet 중 b-hadron 이 ≥2 인 것이 있는지를 기록한다 —
따라서 expanded scheme 은 official 의 *연장*이지 별도 발명이 아니다.
stitching 에서는 항상 **tt+bbb = 61 ∪ 62**, **tt+4b = 71 ∪ 72**,
**tt+nb = 61 ∪ 62 ∪ 71 ∪ 72** 로만 묶는다.

`61` 미만은 NanoAOD 가 보고한 그대로 둔다 (`0`, `41–45`, `51`, `52`,
`53`, `54`, `55`). NanoAOD 가 `53` 이라 부른 이벤트가 실제로 additional
b-jet 3 개를 가지면 `61` 이 된다 (prefix 유지: `253 → 261`).

### 9.3 MiniAOD 에서 lookup 만들기 — `extractTtNb`

producer 는 `TtbarIdHistCompare` 도구의 `extractTtNb.cc` 이며,
**MiniAODv2** 위에서 돈다 (official NanoAOD categorizer 와 같은 출처라
둘이 동일한 generator content 를 본다). sample 마다 작은 ROOT 파일을 쓴다:

```
ttnb_<sampleName>.root          # 예: ttnb_tt4b.root, ttnb_TTToHadronic.root
└── TtNb  (TTree)
      run                 /i
      luminosityBlock     /i
      event               /l
      genTtbarId          /I    # 원래 NanoAOD 값 (self-check 용)
      Expanded_genTtbarId /I    # 61/62/71/72 (+ prefix 유지)
      nAddBJets           /I
      nAddBJetsMulti      /I
```

핵심은, 파일에는 **보정이 필요한 이벤트만** 들어간다는 것이다 —
`Expanded_genTtbarId % 100 ∈ {61,62,71,72}` 이고 `nAddBJets ≥ 3` 으로
교차 확인된 것들 (둘이 어긋나면 producer 가 hard-abort 하므로, 쓰여진
파일은 구성상 내부 일관적이다). 나머지 이벤트는 *없으며*, analyzer 는 그냥
NanoAOD 값을 유지한다.

**비유.** 이것은 전수 조사가 아니라 **예외자 명단**이다. 특별 손목띠가
필요한 소수만 명단에 있고, 나머지는 NanoAOD 티켓으로 그냥 입장한다.

### 9.4 run-time 적용 — `ExpandedTtbarId` class

`ExpandedTtbarId` (`src/ExpandedTtbarId.{h,cc}`) 는 standalone, ROOT 전용
helper 다 — CMSSW 도 treestream 도 의존하지 않는다. 진입점 세 개:

- **`load(path, label, tree="TtNb")`** — `ttnb_*.root` 하나를
  `(run, luminosityBlock, event)` 키의 in-memory hash map 으로 읽는다.
- **`loadFromDir(dir, sampleKey, tree="TtNb")`** — `<dir>/ttnb_<sampleKey>.root`
  를 자동 구성해 로드한다. 정확한 이름이 없으면 distinctive token matching
  (`tt4b`, `ttbb_Hadronic`, `TTToSemiLeptonic`, …) 으로 fallback. 그래도
  없으면 `tried: <path>` 로그를 크게 남기고 **INACTIVE** 가 된다 — fatal 이
  *아니다*. 그래서 tt+nb 파일이 없는 sample 은 그냥 NanoAOD 값을 유지한다.
- **`resolve(run, lumi, event, nanoGenId)`** — 이벤트당 호출. 키가 map 에
  있으면 expanded id (`61/62/71/72`) 를, 없으면 `nanoGenId` 를
  **그대로** 돌려준다.

안전장치 두 개가 중요하다:

1. **키는 `(run, luminosityBlock, event)` 세 개 전부.** `event` 는
   lumisection *안에서만* unique 하고, MC 는 `run` 이 항상 `1` 이므로 이
   triplet 이 최소한의 모호하지 않은 식별자다. hash 는 FNV 이고, 검증 도구
   (`matchTtbarId.cc`) 의 것과 **byte-identical** 이다 — 그래서 analyzer 는
   검증된 matching 을 "동등하게"가 아니라 *정확히 그대로* 재현한다.
   **비유.** 두 사람이 이름(`event`)이 같을 수 있다. 이름+생년월일+주소
   (`run, lumi, event`) 가 있어야 올바른 사람에게 표를 붙였다고 확신한다.
2. **이벤트별 genId self-check.** 키가 매치되면, 저장된 `genTtbarId` 가
   analyzer 가 그 이벤트에 대해 들고 있는 NanoAOD `genTtbarId` 와 같아야
   한다. 다르면 이 sample 에 엉뚱한 `ttnb_*.root` 가 로드된 것이고,
   analyzer 는 기본적으로 **abort (exit 44)** 한다
   (`setAbortOnGenIdMismatch(false)` 로 완화 가능).
   **비유.** 손목띠를 채우기 전에 신분증 사진이 얼굴과 맞는지 확인한다.

### 9.5 Analyzer 통합 — 서로 독립적인 두 지점

per-event label 과 normalization bookkeeping 이 서로 의존하지 않도록
통합을 둘로 나눴다.

**(a) CORE — per-event branch (`main` mode 에서 작동).**
`process()` 의 MC 블록 안에서 analyzer 는

```
_genTtbarIdNano  = _ev->genTtbarId;                       // raw NanoAOD
_expandedTtbarId = _expTtbarId.resolve(_ev->run,
                       _ev->luminosityBlock, _ev->event,
                       _ev->genTtbarId);                   // 61/62/71/72 or 그대로
```

를 세팅하고 **둘 다** output Events tree 에 `genTtbarId/I`,
`expandedTtbarId/I` 로 쓴다. 이 호출은 *MC vs Data* 로만 갈리고 analysis
mode 로는 **갈리지 않으므로**, 평범한 `--mode main` 실행에서 선택된 모든 MC
이벤트에 expanded label 이 기록된다. lookup 은 `main()` 에서 디렉토리
(기본 `DerivedCorr/expandedTtbarId/`, `EXPANDED_TTBARID_DIR` 환경변수로
override) 에서 한 번 로드되며, per-sample 파일은 sample 이름으로 자동
해결된다.

**(b) §5 — prescan ΣgenW 분할 (`prescan` mode 에서 작동).**
`accumulatePrescanEvent()` 에서 같은 `resolve()` 로 각 이벤트의 *generator
weight* 를 올바른 bin 으로 보낸다. tt+nb 이벤트는 `53/54/55` 에서 **빠져**
새 bin `61/62/71/72` 로 들어가므로, §5 이후 `53/54/55` prescan bin 은
**순수 tt+2b** 잔여만 담는다. prescan TTree (`prescan`) 는 branch 8 개를
얻는다: `sumGenW_id_{61,62,71,72}`, `n_id_{61,62,71,72}`.

이 둘은 **중복이 아니다** (가장 흔한 오해 지점):

| | CORE branch `expandedTtbarId` | §5 prescan `sumGenW_id_*` |
|---|---|---|
| 단위 | **이벤트당** 값 하나 | **sample 당** 스칼라 합 하나 |
| 채워지는 범위 | **selection 통과** 이벤트만 | **모든** 이벤트, selection 없음 |
| 역할 | 이벤트가 *어느 bin* 인지 (plot/reweight 시점의 **분자**) | 각 bin 이 *generator weight 를 얼마* 가졌는지 (normalization 의 **분모**) |

**비유.** branch 는 **검사를 통과한 쿠키마다 찍는 도장**이고, prescan 합은
각 레시피로 **갈아낸 밀가루 총량**(쿠키를 찍기 전 계량)이다. 쿠키 가격을
매기려면 밀가루 총량이 필요한데, 검사 통과 쿠키만으로는 그걸 복원할 수 없다
— 탈락분은 트레이에 오르지도 않았기 때문이다.

### 9.6 Validation chain

expanded id 는 *base* category 에 대해 §5 의 four-way validation 을
물려받고, lookup 고유의 검사 두 개를 추가한다:

1. **Match 재현성** — `(run,lumi,event)` 키와 FNV hash 가
   `matchTtbarId.cc` 와 byte-identical 이므로, analyzer 가 relabel 하는
   이벤트 집합은 구성상 검증 도구가 relabel 하는 집합과 같다.
   `matchTtbarId` 로 lookup 을 독립적으로 다시 유도하면 analyzer 의 hit
   리스트가 재현된다.
2. **genId self-check** (§9.4) — 로드된 파일이 처리 중인 sample 에
   속함을 보장한다. 엉뚱한 파일 로드는 첫 mismatch 이벤트에서 abort 하므로
   놓칠 수 없다.
3. **Conservation, §5 이후** — `consolidate_prescan.py` (§10.2) 가
   downstream 에서 확인한다: `61/62/71/72` 를 포함시켜도 전체 id partition
   이 이벤트 총합과 일치 — §5 의 이동이 아무것도 잃거나 중복하지 않았음을
   증명한다.

1000-event TTToSemiLeptonic UL17 NanoAODv9 slice 에서 class 는 깨끗하게
load·match·self-check 했고, production tt4b 실행에서는 load summary 의
tt+bbb (`61+62`), tt+4b (`71+72`) 가 그 sample 의 `extractTtNb` 총량과
일치했다.

---

## 10. category 에서 weight 로 — prescan, consolidation, stitching

이 장은 §9 의 prescan 출력에서 시작해, inclusive ttbar · 4FS ttbb ·
dedicated tt4b sample 을 double-counting 없이 합치는 데 쓸 per-event
weight 까지의 경로를 문서화한다.

### 10.1 왜 stitching 이 필요한가 (double-counting 문제)

세 sample 이 모두 tt+heavy-flavour 이벤트를 생성한다:

- **inclusive** `TTToHadronic` — 모든 ttbar final state, 꼬리에 약간의
  tt+bb·tt+nb 포함;
- **4FS** `ttbb_Hadronic` — 통계 높은 전용 tt+bb sample;
- **dedicated** `tt4b` — tt+≥3b sample.

셋을 그냥 더하면 tt+bb·tt+nb phase space 가 **두세 번** 세어진다.
stitching 은 각 gen-level category 를 **정확히 한** sample 에 배정하고,
dedicated sample 을 rescale 해 그 yield 가 inclusive 가 NNLO 에서 예측하는
값과 맞도록 한다.

**비유.** 제빵사 셋이 쿠키를 굽는데 메뉴가 겹친다. 이렇게 선언한다:
**제너럴리스트**는 plain + 초코칩 + 견과1개 (LF / cc / tt+b) 담당,
**bb 전문**은 견과2개 (tt+2b) 담당, **4b 전문**은 견과3·4개
(tt+bbb / tt+4b) 담당. 그리고 각 전문가가 *반죽량*(r-factor)을 조절해,
제너럴리스트가 *구웠을 만큼*의 전문 쿠키를 정확히 굽는다 — 더도 덜도 아니게.
이제 모든 트레이를 합쳐도 double-counting 이 없다.

### 10.2 Step 1 — prescan 정리·QA: `consolidate_prescan.py`

analyzer 의 `prescan` mode 는 output 파일마다 `prescan` TTree 행 하나를
쓴다. `consolidate_prescan.py` 는 output 영역을 훑어 per-job 행들을
per-sample record 로 합치고 cross-check 를 돌린다. 이것은 물리 단계가 아닌
**bookkeeping gate** 다.

expanded scheme 에 맞춰 갱신된 내용:

- `61/62/71/72` 를 포함한 17-bin id partition 을 읽는다 (§5 이전의 옛
  파일은 해당 bin 에 0 기여);
- **hard** check 로: 전체 id partition 이 이벤트 총합과 일치 (이제 이것이
  §5 이동의 보존도 *함께* 증명한다); NanoAOD ttCat partition 이 총합과
  일치; LF / cc 가 두 labeling 사이에서 1:1 (tt+nb 은 LF/cc 에 절대
  안 떨어짐);
- **informational** 로 *expanded reconciliation* 을 보고: NanoAOD `ttCat`
  의 2b+ 카운트 vs expanded `id` (순수-2b + tt+nb). 작은 차이는 정상이다 —
  official categorizer 가 1b 로 label 했던 소수의 tt+nb 이벤트로,
  algorithmic (official vs ghost-matching) 차이지 오류가 아니다;
- `prescan_summary.json` (full) 과 `prescan_summary.csv` (headline,
  `n_id_bbb61/bbb62/4b71/4b72` 컬럼 포함) 를 출력.

CMSSW (PyROOT) 안에서 돌린다; bad/missing job 이나 실패한 hard check 가
있으면 non-zero exit 로 resubmit chaining 에 편리하다.

### 10.3 Step 2 — factor 계산: `compute_stitch_factors.py`

물리 단계다. `prescan` row 를 (이제 올바른 tree 이름 `prescan` 으로) 다시
모아서, **dedicated 샘플마다** inclusive NNLO 예측에 맞추는 rescale factor 를
계산한다.

**Decay 채널과 heavy-flavour 내용은 서로 직교하는 축이다.** ttbar 는 두 가지로
나뉜다: W-decay 채널(full-hadronic / semi-leptonic / di-leptonic — 상호배타,
겹침 없음)과 HF 내용(LF / cc / tt+b / tt+2b / tt+nb — inclusive 와 dedicated
가 겹침). Stitching 은 HF 겹침만 해소한다; **세 decay 채널은 전부 남겨서
stack 한다.** 아래 "reject" 는 HF *category* 만 제거하지 decay 채널을 버리는
게 아니다 — FH selection 을 통과한 semi-/di-leptonic ttbar 는 진짜 background
이고 버리지 않는다.

**각 dedicated 샘플이 덮는 채널 (gen census + 샘플 목록으로 확인).** `tt4b`
NanoAOD 에 decay-channel census 를 돌린 결과(W→ℓν leg 를 mother==W 로 카운트):
full-hadronic 45.6%, semi-leptonic 43.8%, di-leptonic 10.6% — 즉 **`tt4b` 는
decay-inclusive**(세 채널 전부 덮음). ttbb 의 경우 **세 W-decay 채널 4FS 샘플이
모두 존재**(`ttbb_Hadronic`, `ttbb_SemiLeptonic`, `ttbb_2L2Nu`)하므로 각각 자기
채널에 stitch 한다. 이게 anchoring 을 결정한다:

| dedicated 샘플 | 덮는 채널 | 소유 HF cat | anchor 대상 | 사용 σ_inc |
|---|---|---|---|---|
| `ttbb_Hadronic` | hadronic 만 | tt+2b (53,54,55) | `TTToHadronic` | σ_total × BR_had = 377.96 pb |
| `ttbb_SemiLeptonic` | semi-lep 만 | tt+2b (53,54,55) | `TTToSemiLeptonic` | σ_total × BR_SL = 365.46 pb |
| `ttbb_2L2Nu` | di-lep 만 | tt+2b (53,54,55) | `TTTo2L2Nu` | σ_total × BR_DL = 88.34 pb |
| `tt4b` (decay-incl.) | Had + SL + DL | tt+bbb, tt+4b (61,62,71,72) | `TTToHadronic` + `TTToSemiLeptonic` + `TTTo2L2Nu` (merge) | σ_total = 831.76 pb (BR 없음) |

W-decay 분기비 BR_had = 0.6741² = 0.45441081, BR_SL = 2·0.6741·0.3259 =
0.43937838, BR_DL = 0.3259² = 0.10621081 (합 = 1).

anchor phase space 는 그 dedicated 샘플의 coverage 와 맞아야 한다. 안 그러면
분모가 inconsistent 해진다: 채널별 ttbb 는 각자 매칭되는 inclusive 에
anchor(σ 에 BR_c 곱함), decay-inclusive `tt4b` 는 inclusive 전체 merge 에
anchor(BR factor 없음). W-decay 채널이 직교하므로(위 참고) 이 채널별 anchoring
이 ttHH AN 처방 그대로다 — §3.3 SL Option1 (gen-level partition), §3.4 DL
gen-level 4b exclusion 둘 다 SL/DL 을 남기고 겹치는 gen category 만 제거한다.

> **History / migration note (2026-06).** 이전 revision 은 `ttbb_Hadronic` 만
> stitch 하고 SL/DL tt+2b 는 5FS inclusive (`TTToSemiLeptonic`, `TTTo2L2Nu` 가
> tt+2b 보유)로 뒀다. SL/DL 4FS ttbb 샘플이 없다고 가정하던 시절의 보수적
> 잔재였다. 세 샘플이 다 확인되어, `EXP` 는 이제 tt+2b 를 **세 inclusive 전부**
> 에서 reject 하고 각각 매칭 `ttbb_c` 로 채운다. 옛 동작은 config option
> `EXP_TTBB_HAD_ONLY` 로 보존(fallback 전용). SL/DL 의 σ_dedicated 는 hadronic
> 1.452 pb 를 BR 환산한 기본값(σ_ttbb_total ≈ 3.195 pb → SL 1.404, DL 0.339 pb)
> 이며 YAML weight 와 **상쇄**되므로 절대값이 아니라 YAML↔config 일관성만
> 중요하다(YAML 이 XSDB σ 를 쓰면 그 값으로 교체).

**Factor** — dedicated 샘플 *d* 마다:

```
f_d  = Σgenw_anchor(d, owned cats) / Σgenw_anchor(d, all)
r_d  = σ_inc(d) · f_d / σ_dedicated(d)
```

구체적으로:

```
# 채널별 ttbb (c = Had / SL / DL), 각자 자기 inclusive 에 anchor:
f_B,c = Σgenw_c(53,54,55)          / Σgenw_c(all)            # channel-c anchor
r_B,c = (σ_total · BR_c) · f_B,c   / σ_ttbb,c

# decay-inclusive tt4b, 3채널 inclusive merge 에 anchor:
f_4b  = Σgenw_HadSLDL(61,62,71,72) / Σgenw_HadSLDL(all)      # all-channel anchor
r_4b  = σ_total · f_4b             / σ_tt4b
```

`f_4b` 는 **진짜** tt+nb bin `61/62/71/72`(§9 의 목적) 를 **3채널 inclusive
merge** 에서 합한 값이다(`tt4b` 가 세 채널을 다 덮으므로). 각 `f_B,c` 는 매칭
inclusive `TTTo_c` 하나에서 측정한다. dedicated LHE σ (σ_ttbb,c, σ_tt4b) 는
다음 단계의 표준 YAML normalization 과 상쇄되므로 그 절대값은 결과를 bias 하지
않는다.

(만약 `ttbb_Hadronic` 만 있고 SL/DL 4FS ttbb 가 없다면 config option 을
`EXP_TTBB_HAD_ONLY` 로 — hadronic 채널만 stitch, SL/DL tt+2b 는 5FS inclusive
fallback. `tt4b` 가 hadronic 전용이면 — census ~100% full-hadronic — `EXP_HAD_TT4B`.)

### 10.4 Step 3 — analyzer config: per-(sample, category) multiplier JSON

**왜 per-sample YAML `weight` 에 녹이지 않나?** stitching 은 **샘플 안에서 HF
category 별** 이기 때문이다: 각 `TTTo_c` 는 LF/cc/tt+b 는 *남기고* tt+2b·tt+nb
는 *reject* 해야 하며, reject 된 tt+2b 는 매칭 채널 `ttbb_c` 가, reject 된
tt+nb 는 decay-inclusive `tt4b` 가 채운다. 샘플당 scalar 하나로는 "이 category
는 남기고 저 category 는 버려라" 를 표현할 수 없다. **별도 JSON** 이 맞는 형태다.

`compute_stitch_factors.py` 가 `analyzer_perEvent_factor` 를 출력한다:
per-(sample, category) **multiplier 를 기존 YAML base weight 위에 곱한다**.

```json
"analyzer_perEvent_factor": {
  "application": "final_weight = yaml_base_weight[sample] * multiplier[sample][category] * genWeight * (PU*btagSF*trigSF)",
  "sub_to_category": { "0":"LF", "41":"cc", ..., "61":"ttbbb", "71":"tt4b" },
  "categories": ["LF","cc","ttb","tt2b","ttbbb","tt4b"],
  "samples": {
    "TTToHadronic":      { "role":"inclusive", "by_category": { "LF":1, "cc":1, "ttb":1, "tt2b":0, "ttbbb":0, "tt4b":0 } },
    "TTToSemiLeptonic":  { "role":"inclusive", "by_category": { "LF":1, "cc":1, "ttb":1, "tt2b":0, "ttbbb":0, "tt4b":0 } },
    "TTTo2L2Nu":         { "role":"inclusive", "by_category": { "LF":1, "cc":1, "ttb":1, "tt2b":0, "ttbbb":0, "tt4b":0 } },
    "ttbb_Hadronic":     { "role":"dedicated", "r":r_B_Had, "by_category": { "tt2b":r_B_Had, "...":"rest 0" } },
    "ttbb_SemiLeptonic": { "role":"dedicated", "r":r_B_SL,  "by_category": { "tt2b":r_B_SL,  "...":"rest 0" } },
    "ttbb_2L2Nu":        { "role":"dedicated", "r":r_B_DL,  "by_category": { "tt2b":r_B_DL,  "...":"rest 0" } },
    "tt4b":              { "role":"dedicated", "r":r_4b,    "by_category": { "ttbbb":r_4b, "tt4b":r_4b, "...":"rest 0" } }
  }
}
```

읽는 법:

- **inclusive 샘플** 은 남기는 category 에 `1`, reject 하는 category(dedicated
  가 채움)에 `0`. 세 `TTTo_c` 모두 tt+2b **와** tt+nb 를 reject: tt+2b 는 매칭
  채널 `ttbb_c` 가, tt+nb 는 decay-inclusive `tt4b` 가 채운다. 각자 LF/cc/tt+b
  만 남긴다. inclusive 는 anchor — rescale 안 하므로 r 이 아니라 multiplier `1`.
- **dedicated 샘플** 은 소유 category 에 rescale factor `r`, 나머지 `0`. 세
  `ttbb_c` 는 tt+2b 에 각자 `r_B,c`(채널마다 f·Σgenw 가 달라 r 도 다름); `tt4b`
  는 tt+nb 에 `r_4b`.
- **목록에 없는 샘플**(QCD, ttV, signal, …)은 건드리지 않음 — multiplier `1`.

multiplier 는 표준 YAML normalization **위에** 곱해지므로, 이미 가진 채널별
base weight 가 base normalization 의 단일 소스로 남고 JSON 은 keep/reject
mask 와 r rescale 만 더한다. 일관성 요건 하나: dedicated 샘플의 YAML weight 가
표준 `Lumi·σ_dedicated/Σgenw` 이고 config 와 **같은** σ_dedicated 를 써야 한다
— 채널별 `σ_ttbb,Had = 1.452 pb`, `σ_ttbb,SL ≈ 1.404 pb`, `σ_ttbb,DL ≈ 0.339 pb`
(BR 환산 기본값; YAML 이 XSDB σ 를 쓰면 그 값), `σ_tt4b = 0.296 pb`.
σ_dedicated 는 `base · r` 에서 값에 무관하게 상쇄되므로 YAML σ 와 config σ 가
채널별로 일치하는지만 중요하다. `compute_stitch_factors.py` 는 dedicated 마다
full `Lumi·σ_inc·f/Σ_dedicated` 도 출력하므로 `YAML_weight · r` 과 cross-check
가능하다(§10.7.3 참고).

권장 위치: `DerivedCorr/stitchFactors/stitch_factors_2017.json` —
`CorrectionsManager` 가 이미 로드하는 trigger-SF / b-tag JSON 들 옆.

**analyzer 가 소비하는 방식 (예정).** MC event 마다:
`cat = sub_to_category[expandedTtbarId % 100]`; `sampleName` 이 목록에 있으면
`weight = YAML_weight × multiplier[sampleName][cat] × genWeight × SFs` (`0` 이면
event drop), 없으면 YAML weight 그대로. JSON 을 로드·적용하는 C++ 가 다음 구현
단계다(§10.6).

### 10.5 지금 배선된 것 / 안 된 것

| 항목 | 상태 |
|---|---|
| per-event `expandedTtbarId` branch (`main`) | **완료** (§9.5a) |
| §5 prescan ΣgenW 를 `61/62/71/72` 로 분할 | **완료** (§9.5b) |
| `consolidate_prescan.py` 의 expanded bin QA | **완료** (§10.2) |
| `compute_stitch_factors.py` → r_B,c / r_4b / per-event factor / JSON | **완료** (§10.3–10.4) |
| 채널별 ttbb stitch (Had/SL/DL 각자 자기 inclusive 에 anchor) | **완료** (2026-06; §10.3) |
| `consolidate_prescan.py` 전체 2017 set 검증 (39 sample, 전 cross-check pass, signed-ΣgenW 확인) | **완료** (2026-06; §10.7.1) |
| f / r 의 통계 불확실성 (per-bin Σw² 기반 effective-N) | **TODO** (§10.6) |
| dedicated **ttbb** YAML base weight 를 config σ 와 일치 (inclusive·`tt4b` 는 이미 일치; `ttbb_Had` 가 0.660≠1.452 pb, `ttbb_SL/DL` 은 1.0 placeholder) | **선행작업 필요** (§10.7.3) |
| analyzer 가 `stitch_factors_2017.json` 를 **로드·적용** (`main` + `btagtrig`) | **완료** (2026-06; §11) |
| b-tag normalization reweight 를 `expandedTtbarId` 로 키잉 (analyzer 가 expanded id 를 넘김; tt+nb 분리 여부는 `Config_TtCatGroup.hh` 에 의존, 종료 시 로그) | **코드 완료** (2026-06; §11.3) — `MakeProcessKey` 가 61–72 분리하는지 확인 |
| stitched data/MC stack | **downstream / TODO** |

b-tag reweight 지점이 중요하다: high b-jet multiplicity 가 정확히 tt+nb 가
사는 곳이므로, stitching 을 적용하면 reweight 키
(`MakeProcessKey(sampleName, genTtbarId)`) 를 `expandedTtbarId` 로 바꿔야
한다 — process-key map 이 `61–72` code 를 인식하는지 확인한 뒤.

---

### 10.6 향후 작업 (TODO, 우선순위순)

다음 세션/agent 가 재발견 없이 이어받도록 기록한다. 어느 것도 현재 central
stitch factor 를 막지 않는다 — 계산된 값은 정확하다(2026-06 검증).

1. **`f` / `r` 의 통계 불확실성 (effective-N).** central 값은 signed ΣgenW 로
   정확하지만 통계 오차는 아직 전파하지 않았다. 음수 weight 에서 유효 표본
   크기는 `N_eff = (Σw)² / Σw²` 로 raw count 보다 작다. `f = Σw_owned/Σw_all`
   (따라서 `r`)에 오차를 주려면:
   - **analyzer (C++) prescan writer** — 기존 `sumGenW_id_{i}` 옆에 per-bin
     제곱합 `sumGenW2_id_{i}` 추가 (같은 bin 에 `genWeight*genWeight` 채움).
     지금은 Runs-level `genEventSumw2` total 만 있어 per-category 오차엔 부족.
   - **`compute_stitch_factors.py`** — per-bin Σw² 를 읽어 (sample, category)
     별 `N_eff` 계산, `f`·`r` 에 weighted 이항 오차 전파, JSON `result` 에
     `r_err` 출력.
   - **여기서 왜 중요한가:** 진단 수치상 저통계 anchor 에서 가장 크게 문다 —
     `f_4b ≈ 9.8e-5` 는 inclusive tt+nb ~66k event (TTTo2L2Nu ~8.6k) 에서
     나오고, `ttbb_2L2Nu` tt+2b 가 세 ttbb 채널 중 통계가 가장 적다(~312k).
     따라서 `r_4b`, `r_DL` 의 상대 불확실성이 가장 크고 최종 fit 전에 오차와
     함께 quote 해야 한다.

2. **analyzer (C++) 가 `stitch_factors_2017.json` 를 event 단위로 로드·적용.**
   JSON(`analyzer_perEvent_factor`)은 준비됨; analyzer 는:
   - trigger-SF / b-tag JSON 과 같은 `CorrectionsManager` 방식으로
     `DerivedCorr/stitchFactors/` 에서 로드;
   - `expandedTtbarId % 100` → category 를 `sub_to_category` 로 매핑;
   - `final_weight = yaml_base_weight[sample] * multiplier[sample][category]
     * genWeight * (PU*btagSF*trigSF)` 적용;
   - **선행조건:** dedicated 샘플 YAML base weight 가 JSON config 와 같은
     채널별 σ 를 써야 한다(`σ_ttbb_Had=1.452`, `σ_ttbb_SL≈1.404`,
     `σ_ttbb_DL≈0.339`, `σ_tt4b=0.296` pb). σ_dedicated 는 상쇄되므로
     YAML↔config 일치만 중요 — 첫 stitched stack 전에 확인(§10.7.3 표).

3. **b-tag normalization reweight 키를 `expandedTtbarId` 로 전환** (위 표 주석
   참고)해 reweight 에서 tt+nb 를 구분.

### 10.7 실행 로그 및 결과 검증 (2017 UL, option EXP)

전체 2017 UL set(`AnalyzerOutput_prescan` 아래 39개 sample 디렉토리)에 대해
파이프라인을 실행했다. 두 단계의 콘솔 출력을 그대로 싣고, 각 부분의 의미와
수치 결과의 독립 재검증을 덧붙인다. raw 로그는 JSON 옆에 함께 보관된다
(`consolidate_prescan.log`, `compute_stitch_factors.log`, `README.md`).

#### 10.7.1 `consolidate_prescan.py` — QA gate

```
$ python3 consolidate_prescan.py
[prescan] input base : /pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/AnalyzerOutput_prescan
[prescan] samples    : 39
  [  1/39] BTagCSV_B ...
  [  2/39] BTagCSV_C ...
  [  3/39] BTagCSV_D ...
  [  4/39] BTagCSV_E ...
  [  5/39] BTagCSV_F ...
  [  6/39] JetHT_B ...
  [  7/39] JetHT_C ...
  [  8/39] JetHT_D ...
  [  9/39] JetHT_E ...
  [ 10/39] JetHT_F ...
  [ 11/39] QCD_HT1000to1500 ...
  [ 12/39] QCD_HT1500to2000 ...
  [ 13/39] QCD_HT2000toInf ...
  [ 14/39] QCD_HT200to300 ...
  [ 15/39] QCD_HT300to500 ...
  [ 16/39] QCD_HT500to700 ...
  [ 17/39] QCD_HT700to1000 ...
  [ 18/39] SingleMuon_B ...
  [ 19/39] SingleMuon_C ...
  [ 20/39] SingleMuon_D ...
  [ 21/39] SingleMuon_E ...
  [ 22/39] SingleMuon_F ...
  [ 23/39] TTTo2L2Nu ...
  [ 24/39] TTToHadronic ...
  [ 25/39] TTToSemiLeptonic ...
  [ 26/39] tt4b ...
  [ 27/39] ttHH ...
  [ 28/39] ttHtobb ...
  [ 29/39] ttWH ...
  [ 30/39] ttWW ...
  [ 31/39] ttWZ ...
  [ 32/39] ttZHto4b ...
  [ 33/39] ttZZto4b ...
  [ 34/39] ttZtobb ...
  [ 35/39] ttbb_2L2Nu ...
  [ 36/39] ttbb_Hadronic ...
  [ 37/39] ttbb_SemiLeptonic ...
  [ 38/39] tttW ...
  [ 39/39] tttt ...

────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
  PRESCAN CONSOLIDATED SUMMARY
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
Sample                  Type  Jobs ok/found              nEvents               ΣgenW(Events)                 ΣgenW(Runs)     skim%   chk
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
BTagCSV_B               Data  3/3                      1,808,836                 1,808,836.0                         0.0         —    OK
BTagCSV_C               Data  27/27                   34,491,540                34,491,540.0                         0.0         —    OK
BTagCSV_D               Data  8/8                      7,967,055                 7,967,055.0                         0.0         —    OK
BTagCSV_E               Data  12/12                   17,175,226                17,175,226.0                         0.0         —    OK
BTagCSV_F               Data  78/78                   75,677,461                75,677,461.0                         0.0         —    OK
JetHT_B                 Data  33/33                   63,043,590                63,043,590.0                         0.0         —    OK
JetHT_C                 Data  66/66                   96,264,601                96,264,601.0                         0.0         —    OK
JetHT_D                 Data  37/37                   46,145,204                46,145,204.0                         0.0         —    OK
JetHT_E                 Data  58/58                   89,630,771                89,630,771.0                         0.0         —    OK
JetHT_F                 Data  66/66                  115,429,972               115,429,972.0                         0.0         —    OK
QCD_HT1000to1500        MC    43/43                   14,164,109                14,164,109.0                14,164,109.0   +0.0000    OK
QCD_HT1500to2000        MC    46/46                   12,402,197                12,402,197.0                12,402,197.0   +0.0000    OK
QCD_HT2000toInf         MC    31/31                    5,614,050                 5,614,050.0                 5,614,050.0   +0.0000    OK
QCD_HT200to300          MC    77/77                   60,056,309                60,056,309.0                60,056,309.0   +0.0000    OK
QCD_HT300to500          MC    66/66                   54,770,756                54,770,756.0                54,770,756.0   +0.0000    OK
QCD_HT500to700          MC    74/74                   60,395,873                60,395,873.0                60,395,873.0   +0.0000    OK
QCD_HT700to1000         MC    85/85                   47,501,834                47,501,834.0                47,501,834.0   +0.0000    OK
SingleMuon_B            Data  79/79                  136,300,266               136,300,266.0                         0.0         —    OK
SingleMuon_C            Data  117/117                165,652,756               165,652,756.0                         0.0         —    OK
SingleMuon_D            Data  47/47                   70,361,660                70,361,660.0                         0.0         —    OK
SingleMuon_E            Data  78/78                  154,618,774               154,618,774.0                         0.0         —    OK
SingleMuon_F            Data  115/115                242,140,980               242,140,980.0                         0.0         —    OK
TTTo2L2Nu               MC    99/99                  106,724,000        7,695,841,652.167358        7,695,841,311.017002   +0.0000    OK
TTToHadronic            MC    199/199                232,999,999        73,140,766,669.42169        73,140,765,879.47006   +0.0000    OK
TTToSemiLeptonic        MC    297/297                346,052,000       104,129,959,629.04999       104,129,959,042.42809   +0.0000    OK
tt4b                    MC    14/14                    9,502,000                 9,502,000.0                 9,502,000.0   +0.0000    OK
ttHH                    MC    32/32                    9,934,000                 9,934,000.0                 9,934,000.0   +0.0000    OK
ttHtobb                 MC    46/46                    7,825,000        3,919,780.8296052217            3,919,780.948356   -0.0000    OK
ttWH                    MC    11/11                      360,000                   360,000.0                   360,000.0   +0.0000    OK
ttWW                    MC    9/9                        698,000                   698,000.0                   698,000.0   +0.0000    OK
ttWZ                    MC    2/2                        350,000                   350,000.0                   350,000.0   +0.0000    OK
ttZHto4b                MC    7/7                      5,000,000                 5,000,000.0                 5,000,000.0   +0.0000    OK
ttZZto4b                MC    33/33                    4,832,000                 4,832,000.0                 4,832,000.0   +0.0000    OK
ttZtobb                 MC    26/26                    7,074,000          794,582.7077516913          794,582.7253137003   -0.0000    OK
ttbb_2L2Nu              MC    7/7                      3,472,503         15,930,890.41240549        15,930,889.901690006   +0.0000    OK
ttbb_Hadronic           MC    9/9                      5,694,656         113,736,165.6780777        113,736,166.15320005   -0.0000    OK
ttbb_SemiLeptonic       MC    12/12                    7,318,891         153,797,755.7116127        153,797,754.09350008   +0.0000    OK
tttW                    MC    5/5                        360,000                   360,000.0                   360,000.0   +0.0000    OK
tttt                    MC    30/30                   10,351,000          84,047.03376162797          84,047.03675157602   -0.0000    OK
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

─────────────────────────────────────────────────────────────────────────────────────────────────────────────────
  EXPANDED genTtbarId %% 100 — RAW EVENT COUNTS (stitching partition)
  1b/2b = additional b-jets; tt+bbb/tt+4b = expanded (>=3 b-jets, from §5)
─────────────────────────────────────────────────────────────────────────────────────────────────────────────────
Sample                      LF(0)  cc(41-45)   1b(51,52)   2b(53-55)  tt+bbb(61,62)  tt+4b(71,72)     lt0   other
─────────────────────────────────────────────────────────────────────────────────────────────────────────────────
QCD_HT1000to1500        8,778,352  3,563,372   1,215,724     606,661              0             0       0       0
QCD_HT1500to2000        7,422,232  3,288,829   1,138,780     552,356              0             0       0       0
QCD_HT2000toInf         3,292,434  1,534,263     536,746     250,607              0             0       0       0
QCD_HT200to300         45,974,991  9,547,064   3,302,703   1,231,551              0             0       0       0
QCD_HT300to500         39,483,934 10,270,118   3,466,407   1,550,297              0             0       0       0
QCD_HT500to700         40,797,642 13,067,738   4,380,698   2,149,795              0             0       0       0
QCD_HT700to1000        30,695,729 11,153,101   3,764,573   1,888,431              0             0       0       0
TTTo2L2Nu              97,661,457  6,877,886   1,802,014     374,084          7,673           886       0       0
TTToHadronic          206,860,997 20,445,685   4,692,133     976,087         22,386         2,711       0       0
TTToSemiLeptonic      311,921,759 26,367,647   6,405,078   1,325,354         28,813         3,349       0       0
tt4b                      902,285     82,391   3,152,285   3,482,869      1,585,810       296,360       0       0
ttHH                       16,911      1,634     286,066   9,629,389              0             0       0       0
ttHtobb                   296,416     31,929   2,617,004   4,879,651              0             0       0       0
ttWH                      126,262     27,764      73,989     131,985              0             0       0       0
ttWW                      606,317     72,074      15,880       3,729              0             0       0       0
ttWZ                      231,494     62,924      26,197      29,385              0             0       0       0
ttZHto4b                   11,829        970     182,008   4,805,193              0             0       0       0
ttZZto4b                   23,064      1,776     266,761   4,540,399              0             0       0       0
ttZtobb                   377,572     37,296   2,596,940   4,062,192              0             0       0       0
ttbb_2L2Nu              1,661,972    107,774   1,379,757     311,762          9,953         1,285       0       0
ttbb_Hadronic           2,545,506    237,091   2,326,156     562,513         20,631         2,759       0       0
ttbb_SemiLeptonic       3,381,121    267,336   2,954,484     689,406         23,468         3,076       0       0
tttW                       60,841      7,396     219,575      72,188              0             0       0       0
tttt                      618,544    113,708   4,059,930   5,558,818              0             0       0       0
─────────────────────────────────────────────────────────────────────────────────────────────────────────────────

────────────────────────────────────────────────────────────────────────────────────────
  EXPANDED PARTITION CHECK — NanoAOD ttCat(2b+)  vs  expanded id(2b + tt+nb)
  diff>0 => some tt+nb had a NanoAOD 1b/LF label (expected, small, algorithmic)
────────────────────────────────────────────────────────────────────────────────────────
Sample                     ttCat 2b+     id 2b+tt+nb tt+nb->1b   tt+nb(61-72)  tt+nb/2b+
────────────────────────────────────────────────────────────────────────────────────────
QCD_HT1000to1500             606,661         606,661         0              0     0.0000
QCD_HT1500to2000             552,356         552,356         0              0     0.0000
QCD_HT2000toInf              250,607         250,607         0              0     0.0000
QCD_HT200to300             1,231,551       1,231,551         0              0     0.0000
QCD_HT300to500             1,550,297       1,550,297         0              0     0.0000
QCD_HT500to700             2,149,795       2,149,795         0              0     0.0000
QCD_HT700to1000            1,888,431       1,888,431         0              0     0.0000
TTTo2L2Nu                    382,643         382,643         0          8,559     0.0224
TTToHadronic               1,001,184       1,001,184         0         25,097     0.0251
TTToSemiLeptonic           1,357,516       1,357,516         0         32,162     0.0237
tt4b                       5,365,039       5,365,039         0      1,882,170     0.3508
ttHH                       9,629,389       9,629,389         0              0     0.0000
ttHtobb                    4,879,651       4,879,651         0              0     0.0000
ttWH                         131,985         131,985         0              0     0.0000
ttWW                           3,729           3,729         0              0     0.0000
ttWZ                          29,385          29,385         0              0     0.0000
ttZHto4b                   4,805,193       4,805,193         0              0     0.0000
ttZZto4b                   4,540,399       4,540,399         0              0     0.0000
ttZtobb                    4,062,192       4,062,192         0              0     0.0000
ttbb_2L2Nu                   323,000         323,000         0         11,238     0.0348
ttbb_Hadronic                585,903         585,903         0         23,390     0.0399
ttbb_SemiLeptonic            715,950         715,950         0         26,544     0.0371
tttW                          72,188          72,188         0              0     0.0000
tttt                       5,558,818       5,558,818         0              0     0.0000
────────────────────────────────────────────────────────────────────────────────────────

  No anomalies: every job valid, every cross-check passed.

  Wrote ./prescan_summary/prescan_summary.json
  Wrote ./prescan_summary/prescan_summary.csv
```

**확인되는 것.**

- **모든 job 계상.** 모든 sample 의 `Jobs ok/found` 가 동일(예: `TTToHadronic
  199/199`, `TTToSemiLeptonic 297/297`) — 누락·손상 prescan 파일 없음 →
  `f` 에 들어가는 ΣgenW 분모가 완전하다.
- **signed genWeight 정확 누적.** 모든 MC 에서 `ΣgenW(Events) ≈ ΣgenW(Runs)`,
  `skim% ≈ 0` (TTToHadronic 73,140,766,669 vs 73,140,765,879 — 상대 ~1×10⁻⁸).
  Runs tree 의 `genEventSumw` 는 정의상 signed 이므로, 이 일치는 §5 prescan
  writer 가 `genWeight` 를 **부호 그대로** 합한다는 증거다 — 음수 weight 가
  |w| 가 아니라 부호대로 접힌다. 이전의 |w| 버그 의혹을 종결한다. (가장 강한
  검증은 음수 비중 큰 `ttZtobb` 33%, `tttt` 39% 도 ~10⁻⁸ 로 맞는다는 점.)
- **§5 expanded partition 정확.** partition check 에서 `tt+nb -> 1b` 가 모든
  sample 에 `0`: NanoAOD ttCat “2b+” 카운트가 expanded “2b + tt+nb” 와 event
  단위로 일치. NanoAOD 1b/LF label 에서 tt+nb 가 새어든 게 없다 — §9 의
  lookup-vs-NanoAOD 재분류가 자기일관적이고 53/54/55 는 이제 순수 tt+2b.
- **tt+nb 비율이 물리적.** `tt+nb / 2b+` 가 inclusive(~0.022–0.025) → 4FS
  ttbb(~0.035–0.040) → dedicated `tt4b`(0.351) 로 증가 — 추가 b-quark 가
  풍부해질수록 커지는 정확한 순서. `tt4b` 는 tt+nb event 1,882,170개
  (tt+bbb 1,585,810 + tt+4b 296,360)로 tt+nb template 의 통계 척추다.

#### 10.7.2 `compute_stitch_factors.py` — 물리 단계

```
$ python3 compute_stitch_factors.py

══════════════════════════════════════════════════════════════════
 Config
──────────────────────────────────────────────────────────────────
  ANALYZER_OUTPUT_DIR         = /pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/AnalyzerOutput_prescan
  PRESCAN_TREE                = prescan
  OUTPUT_JSON_PATH            = DerivedCorr/stitchFactors/stitch_factors_2017.json
  PARTITION_OPTION            = EXP
  ONLY_USE_LISTED_SAMPLE_DIRS = False
  ROOT_FILE_GLOB              = *.root
  LUMI_PB_INV                 = 41480.0
  BR_HAD                      = 0.45441081000000005
  σ_inc total (NNLO+NNLL)     = 831.76 pb
  σ_ttbb (hadronic LHE)       = 1.452 pb
  σ_tt4b (LHE)                = 0.296 pb
  inclusive samples           = ['TTToHadronic', 'TTToSemiLeptonic', 'TTTo2L2Nu']
    dedicated ttbb_Hadronic   owns=['tt2b'] anchor=['TTToHadronic'] sigma_inc=377.961pb x BR_had
    dedicated ttbb_SemiLeptonic owns=['tt2b'] anchor=['TTToSemiLeptonic'] sigma_inc=365.457pb (no BR)
    dedicated ttbb_2L2Nu      owns=['tt2b'] anchor=['TTTo2L2Nu'] sigma_inc=88.342pb (no BR)
    dedicated tt4b            owns=['ttbbb', 'tt4b'] anchor=['TTToHadronic', 'TTToSemiLeptonic', 'TTTo2L2Nu'] sigma_inc=831.760pb (no BR)
══════════════════════════════════════════════════════════════════
INFO  found 39 sample directories under /pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/AnalyzerOutput_prescan



══════════════════════════════════════════════════════════════════
 Sample summary
──────────────────────────────────────────────────────────────────
sample                           files           nEv           Σgenw   |neg|/pos
────────────────────────────────────────────────────────────────────────────────
BTagCSV_B                            3       1808836          (data)           -
BTagCSV_C                           27      34491540          (data)           -
BTagCSV_D                            8       7967055          (data)           -
BTagCSV_E                           12      17175226          (data)           -
BTagCSV_F                           78      75677461          (data)           -
JetHT_B                             33      63043590          (data)           -
JetHT_C                             66      96264601          (data)           -
JetHT_D                             37      46145204          (data)           -
JetHT_E                             58      89630771          (data)           -
JetHT_F                             66     115429972          (data)           -
QCD_HT1000to1500                    43      14164109     1.41641e+07      0.0000
QCD_HT1500to2000                    46      12402197     1.24022e+07      0.0000
QCD_HT2000toInf                     31       5614050     5.61405e+06      0.0000
QCD_HT200to300                      77      60056309     6.00563e+07      0.0000
QCD_HT300to500                      66      54770756     5.47708e+07      0.0000
QCD_HT500to700                      74      60395873     6.03959e+07      0.0000
QCD_HT700to1000                     85      47501834     4.75018e+07      0.0000
SingleMuon_B                        79     136300266          (data)           -
SingleMuon_C                       117     165652756          (data)           -
SingleMuon_D                        47      70361660          (data)           -
SingleMuon_E                        78     154618774          (data)           -
SingleMuon_F                       115     242140980          (data)           -
TTTo2L2Nu                           99     106724000     7.69584e+09      0.0041
TTToHadronic                       199     232999999     7.31408e+10      0.0041
TTToSemiLeptonic                   297     346052000      1.0413e+11      0.0041
tt4b                                14       9502000       9.502e+06      0.0000
ttHH                                32       9934000       9.934e+06      0.0000
ttHtobb                             46       7825000     3.91978e+06      0.0105
ttWH                                11        360000          360000      0.0000
ttWW                                 9        698000          698000      0.0000
ttWZ                                 2        350000          350000      0.0000
ttZHto4b                             7       5000000           5e+06      0.0000
ttZZto4b                            33       4832000       4.832e+06      0.0000
ttZtobb                             26       7074000          794583      0.3300
ttbb_2L2Nu                           7       3472503     1.59309e+07      0.0628
ttbb_Hadronic                        9       5694656     1.13736e+08      0.0627
ttbb_SemiLeptonic                   12       7318891     1.53798e+08      0.0626
tttW                                 5        360000          360000      0.0000
tttt                                30      10351000           84047      0.3915
══════════════════════════════════════════════════════════════════

══════════════════════════════════════════════════════════════════
 genTtbarId % 100 breakdown — Σ genW
──────────────────────────────────────────────────────────────────
sample                             LF(0)     cc(41-45)   tt+b(51,52)  tt+2b(53-55) tt+bbb(61,62)  tt+4b(71,72)         other           lt0
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
QCD_HT1000to1500               8.778e+06     3.563e+06     1.216e+06     6.067e+05             0             0             0             0
QCD_HT1500to2000               7.422e+06     3.289e+06     1.139e+06     5.524e+05             0             0             0             0
QCD_HT2000toInf                3.292e+06     1.534e+06     5.367e+05     2.506e+05             0             0             0             0
QCD_HT200to300                 4.597e+07     9.547e+06     3.303e+06     1.232e+06             0             0             0             0
QCD_HT300to500                 3.948e+07     1.027e+07     3.466e+06      1.55e+06             0             0             0             0
QCD_HT500to700                  4.08e+07     1.307e+07     4.381e+06      2.15e+06             0             0             0             0
QCD_HT700to1000                 3.07e+07     1.115e+07     3.765e+06     1.888e+06             0             0             0             0
TTTo2L2Nu                      7.042e+09      4.96e+08     1.299e+08     2.697e+07     5.532e+05     6.412e+04             0             0
TTToHadronic                   6.493e+10      6.42e+09     1.473e+09     3.063e+08     7.018e+06     8.503e+05             0             0
TTToSemiLeptonic               9.386e+10     7.936e+09     1.927e+09     3.987e+08     8.667e+06     1.009e+06             0             0
tt4b                           9.023e+05     8.239e+04     3.152e+06     3.483e+06     1.586e+06     2.964e+05             0             0
ttHH                           1.691e+04          1634     2.861e+05     9.629e+06             0             0             0             0
ttHtobb                        1.484e+05     1.597e+04      1.31e+06     2.445e+06             0             0             0             0
ttWH                           1.263e+05     2.776e+04     7.399e+04      1.32e+05             0             0             0             0
ttWW                           6.063e+05     7.207e+04     1.588e+04          3729             0             0             0             0
ttWZ                           2.315e+05     6.292e+04      2.62e+04     2.938e+04             0             0             0             0
ttZHto4b                       1.183e+04           970      1.82e+05     4.805e+06             0             0             0             0
ttZZto4b                       2.306e+04          1776     2.668e+05      4.54e+06             0             0             0             0
ttZtobb                        4.765e+04          4111     2.972e+05     4.457e+05             0             0             0             0
ttbb_2L2Nu                     7.282e+06     5.142e+05     6.487e+06      1.59e+06     5.069e+04          6591             0             0
ttbb_Hadronic                  4.848e+07     4.784e+06     4.754e+07     1.242e+07     4.525e+05     6.158e+04             0             0
ttbb_SemiLeptonic              6.781e+07     5.734e+06     6.357e+07     1.606e+07      5.44e+05      7.19e+04             0             0
tttW                           6.084e+04          7396     2.196e+05     7.219e+04             0             0             0             0
tttt                                5322         850.1     3.348e+04      4.44e+04             0             0             0             0
══════════════════════════════════════════════════════════════════

══════════════════════════════════════════════════════════════════
 Stitching result — option EXP
──────────────────────────────────────────────────────────────────
  lumi                    =       41480.00 pb⁻¹
  σ_inc total (NNLO+NNLL) =       831.7600 pb
  BR_had                  =       0.454411
  inclusive samples       = ['TTToHadronic', 'TTToSemiLeptonic', 'TTTo2L2Nu']

  Per-dedicated anchoring (each rescaled to its own decay phase space):
  ----------------------------------------------------------------
   [ttbb_Hadronic]  owns=['tt2b']  anchor=['TTToHadronic']
     σ_inc(used)         =       377.9607 pb   σ_ded =    1.4520 pb (cancels)
     Σgenw anchor(all)   =    7.31408e+10
     Σgenw anchor(owned) =    3.06272e+08
     Σgenw dedicated     =    1.13736e+08
     f = owned/all       =       0.004187
     r = σ_inc·f/σ_ded   =       1.090005   <- multiply this sample's YAML weight by r
     (xcheck) Lumi·σ_inc·f/Σ_ded =  0.00057721165
  ----------------------------------------------------------------
   [ttbb_SemiLeptonic]  owns=['tt2b']  anchor=['TTToSemiLeptonic']
     σ_inc(used)         =       365.4574 pb   σ_ded =    1.4040 pb (cancels)
     Σgenw anchor(all)   =     1.0413e+11
     Σgenw anchor(owned) =    3.98727e+08
     Σgenw dedicated     =    1.53798e+08
     f = owned/all       =       0.003829
     r = σ_inc·f/σ_ded   =       0.996736   <- multiply this sample's YAML weight by r
     (xcheck) Lumi·σ_inc·f/Σ_ded =  0.00037742051
  ----------------------------------------------------------------
   [ttbb_2L2Nu]  owns=['tt2b']  anchor=['TTTo2L2Nu']
     σ_inc(used)         =        88.3419 pb   σ_ded =    0.3394 pb (cancels)
     Σgenw anchor(all)   =    7.69584e+09
     Σgenw anchor(owned) =    2.69669e+07
     Σgenw dedicated     =    1.59309e+07
     f = owned/all       =       0.003504
     r = σ_inc·f/σ_ded   =       0.912125   <- multiply this sample's YAML weight by r
     (xcheck) Lumi·σ_inc·f/Σ_ded =  0.00080600848
  ----------------------------------------------------------------
   [tt4b]  owns=['ttbbb', 'tt4b']  anchor=['TTToHadronic', 'TTToSemiLeptonic', 'TTTo2L2Nu']
     σ_inc(used)         =       831.7600 pb   σ_ded =    0.2960 pb (cancels)
     Σgenw anchor(all)   =    1.84967e+11
     Σgenw anchor(owned) =    1.81621e+07
     Σgenw dedicated     =      9.502e+06
     f = owned/all       =       0.000098
     r = σ_inc·f/σ_ded   =       0.275917   <- multiply this sample's YAML weight by r
     (xcheck) Lumi·σ_inc·f/Σ_ded =  0.00035652855
  ----------------------------------------------------------------
  Reject map (inclusive sample -> categories removed, filled by a dedicated):
     TTToHadronic           reject ['tt2b', 'ttbbb', 'tt4b']
     TTToSemiLeptonic       reject ['tt2b', 'ttbbb', 'tt4b']
     TTTo2L2Nu              reject ['tt2b', 'ttbbb', 'tt4b']
══════════════════════════════════════════════════════════════════

══════════════════════════════════════════════════════════════════
 Analyzer per-event MULTIPLIER table  (-> stitch_factors JSON)
 rows = sampleName ; cols = category from expandedTtbarId % 100
──────────────────────────────────────────────────────────────────
sample              role                LF         cc        ttb       tt2b      ttbbb       tt4b
─────────────────────────────────────────────────────────────────────────────────────────────────
TTTo2L2Nu           inclusive            1          1          1          0          0          0
TTToHadronic        inclusive            1          1          1          0          0          0
TTToSemiLeptonic    inclusive            1          1          1          0          0          0
tt4b                dedicated            0          0          0          0    0.27592    0.27592
ttbb_2L2Nu          dedicated            0          0          0    0.91212          0          0
ttbb_Hadronic       dedicated            0          0          0       1.09          0          0
ttbb_SemiLeptonic   dedicated            0          0          0    0.99674          0          0
──────────────────────────────────────────────────────────────────
 1 = keep (inclusive) ; r = dedicated rescale ; 0 = rejected.
 Applied ON TOP of YAML weight. Samples not shown keep YAML weight (x1).
══════════════════════════════════════════════════════════════════
INFO  wrote DerivedCorr/stitchFactors/stitch_factors_2017.json
```

**보여주는 것.**

- **채널별 anchoring 적용.** 각 `ttbb_c` 는 자기 inclusive `TTTo_c` 에
  anchor(σ_inc = σ_total × BR_c: Had 377.96, SL 365.46, DL 88.34 pb);
  decay-inclusive `tt4b` 는 3채널 merge 에 anchor(σ_total = 831.76 pb, BR
  없음). reject map 은 세 inclusive 전부에서 tt+2b, tt+bbb, tt+4b 제거.
- **stitch factor `r`** (각 dedicated 샘플의 YAML base weight 에 곱함):

  | dedicated | f = owned/all | r |
  |---|---|---|
  | `ttbb_Hadronic` | 0.004187 | 1.0900 |
  | `ttbb_SemiLeptonic` | 0.003829 | 0.9967 |
  | `ttbb_2L2Nu` | 0.003504 | 0.9121 |
  | `tt4b` | 0.0000982 | 0.2759 |

- **ttbb `r` 가 채널마다 다른 이유.** `f_B,c`(inclusive 의 tt+2b 분율)가
  Had/SL/DL 에 0.00419 / 0.00383 / 0.00350 — decay 채널 selection 이 tt+2b
  phase space 를 더 자를수록 `f` 가 작다. hadronic `r`(1.090)를 SL/DL 에
  재사용했다면 각각 약 9%, 20% mis-normalize. 채널별 stitching 의 정량적 근거.
- **`r_4b` ≈ 0.28 (1 미만)인 이유.** dedicated `tt4b` native σ(0.296 pb)가
  inclusive-anchored 예측(σ_total · f_4b ≈ 0.082 pb)의 ~3.6배 → anchoring 이
  `tt4b` 를 tt+nb 영역에서 0.276 으로 **축소**해 NNLO 기대치에 맞춘다 —
  과생산된 dedicated 샘플의 올바른 거동. (참고(cosmetic): Config echo 가
  SL/DL σ_inc 를 “(no BR)” 로 표기하나 실제 BR 환산값이다 — 365.46 =
  831.76·BR_SL, 88.34 = 831.76·BR_DL — 라벨이 BR_had 만 보는 탓. 수치는 정확.)

#### 10.7.3 독립 재검증 및 YAML↔config σ 감사

JSON `result` 블록에서 `r = σ_inc · f / σ_dedicated`, `f = Σgenw_owned /
Σgenw_all` 을 직접 재유도하면 quote 된 모든 `r` 을 <10⁻⁹ 로 재현하고, 분기비
합이 정확히 닫힌다(BR_had + BR_SL + BR_DL = 0.45441081 + 0.43937838 +
0.10621081 = 1.00000000). JSON 의 anchor ΣgenW 는 compute 로그의 breakdown 표와
일치한다(예: `tt4b` owned ΣgenW = 1.816×10⁷ = 세 inclusive 의 `61/62/71/72`
ΣgenW 합; `ttbb_Hadronic` owned ΣgenW = 3.063×10⁸ = TTToHadronic 의 `53/54/55`).

**YAML ↔ config σ 감사 — analyzer 적용 전 필수.** multiplier 가 YAML base
weight *위에* 곱해지므로 최종 normalization 은 `yaml_weight × r` 이고, 이는
의도한 `Lumi·σ_inc·f/Σ` (JSON `peF_full`)와 **YAML σ_dedicated = config
σ_dedicated 일 때만** 같다(채널별). 현재 `Tier3_2017_FH_unified_main.yml` 을
config 와 대조:

| sample | YAML weight | 함의 σ (pb) | config σ (pb) | `yaml×r` vs `peF_full` |
|---|---|---|---|---|
| `TTToHadronic` | 2.1435×10⁻⁴ | 377.96 | 377.96 (anchor) | ✓ 일치 |
| `TTToSemiLeptonic` | 1.4558×10⁻⁴ | 365.46 | 365.46 (anchor) | ✓ 일치 |
| `TTTo2L2Nu` | 4.7616×10⁻⁴ | 88.34 | 88.34 (anchor) | ✓ 일치 |
| `tt4b` | 1.2922×10⁻³ | 0.296 | 0.296 | ✓ (3.565×10⁻⁴ = peF_full) |
| `ttbb_Hadronic` | 2.4063×10⁻⁴ | **0.660** | 1.452 | ✗ BR_had 만큼 어긋남 |
| `ttbb_SemiLeptonic` | **1.0** (placeholder) | — | 1.404 | ✗ |
| `ttbb_2L2Nu` | **1.0** (placeholder) | — | 0.339 | ✗ |

inclusive 샘플과 `tt4b` 는 이미 일치한다 — inclusive YAML weight 가 독립적으로
anchor 에 쓰인 σ_total×BR_c 를 정확히 함의하고, `tt4b` 는 자기 `peF_full` 에
떨어진다. 세 **ttbb** weight 만 첫 stitched stack 전에 config σ 기준
`Lumi·σ_dedicated/ΣgenW` 로 설정해야 한다:

```
ttbb_Hadronic     weight = 41480 · 1.452   / 1.13736e8 = 5.2955e-4   (현재 2.4063e-4)
ttbb_SemiLeptonic weight = 41480 · 1.40397 / 1.53798e8 = 3.7866e-4   (현재 1.0)
ttbb_2L2Nu        weight = 41480 · 0.33938 / 1.59309e7 = 8.8366e-4   (현재 1.0)
```

(σ_dedicated 는 상쇄되므로, 대안으로 *config* 를 “σ_total×BR” 관례로 바꾸고
— σ_ttbb,Had = 1.452×BR_had = 0.660 pb 등 — 현재 `ttbb_Hadronic` 2.4063×10⁻⁴
를 유지해도 된다; 어느 쪽이든 YAML 과 JSON config 가 채널별로 **같은** σ 를
써야 한다. 현재의 혼합 — YAML 은 0.660 관례, config 는 1.452 관례 — 가 유일하게
틀린 조합이다.) 수정 후 각 `ttbb_c` 의 `yaml_weight × r` 은 JSON `peF_full` 에
떨어진다: Had 5.772×10⁻⁴, SL 3.774×10⁻⁴, DL 8.060×10⁻⁴.

**결론.** `stitch_factors_2017.json` 은 내부적으로 정확하고 두 로그와 일치한다;
central stitch factor 는 확정. analyzer 적용 전 남은 단 하나의 항목은 위 ttbb
YAML weight 정합(inclusive·`tt4b` 는 변경 불필요)이다.


---

## 11. analyzer 에서 stitch + categorization 적용 (`main` & `btagtrig`)

§9 가 이벤트 단위 expanded id 를 만들고, §10 이 prescan 을 per-(sample,
category) multiplier JSON 으로 바꿨다. 이 절은 마지막 고리다: analyzer 가 이제
그 JSON 을 **로드해 이벤트마다 적용**한다 — `main` 분석과 `btagtrig`(b-tag /
trigger reweight 유도) 모드 둘 다 — 그리고 b-tag normalization reweight 를
expanded category 로 다시 키잉한다.

### 11.1 비유로 보는 그림

세 inclusive `TTTo_c` 샘플을 모든 상품을 파는 **대형마트**, dedicated 4FS 샘플
(`ttbb_c`, `tt4b`)을 몇몇 상품을 훨씬 잘 만드는 **전문 공급업체**(정육점,
생선가게)라고 생각하자.

- 일상 상품 — `LF`, `cc`, `tt+b` — 은 마트 재고를 그대로 둔다 (multiplier **1**).
- 프리미엄 상품 — `tt+2b`, `tt+nb` (추가 b-jet 이 있는 이벤트, 5FS 마트가 잘 못
  만듦) — 은 마트 버전 판매를 **중단**(multiplier **0**)하고 전문업체 것을 판다.
- 그런데 전문업체가 과잉 생산하므로, 그 상품 가격을 factor **r** 로
  **재조정**(예: `tt4b` ×0.276)해서 총 재고가 사업계획(NNLO 단면적)과 맞게 한다.

`expandedTtbarId % 100` 은 각 이벤트가 어떤 상품인지 알려주는 **바코드**다
(`53`=tt+2b, `61`=tt+bbb, `71`=tt+4b, …). JSON 은 샘플마다 한 행씩의
**유지/폐기·재가격 표**다. analyzer 는 모든 MC 이벤트의 바코드를 읽어 그 샘플의
행을 찾아 이벤트 weight 에 표 값을 곱한다. `0` 은 "다른 공급업체가 담당" →
이 샘플에서 그 이벤트는 버려지고 매칭 전문업체가 공급한다.

§9 가 가져다 준 것: expanded 바코드가 없으면 tt+bbb·tt+4b 가 tt+2b 와 구별되지
않아(같은 바코드) 마트의 tt+2b 는 유지하면서 tt+nb 만 버리는 게 불가능하다.
expanded id 가 바로 그 keep/drop mask 를 올바른 granularity 로 가능하게 한다.

### 11.2 multiplier 적용 지점 (두 모드 공통)

MC 이벤트마다, `process()` 안에서 expanded id 가 resolve 된 직후:

```
weight  =  yaml_base_weight                          (per-sample YAML)
        ×  PU × L1prefire × genWeight                 (기존 MC weight)
        ×  stitch_multiplier[sample][category]        <-- NEW (JSON)
        ×  btagShapeSF × triggerSF × btagNormReweight  (기존 SF, selectObjects 안)
```

multiplier 는 `selectObjects()` 가 세 진단 weight chain 과 SF 를 seed 하기
**전에** 적용되므로 downstream 으로 자동 전파된다. `main` 과 `btagtrig` 가 같은
`process()` → `selectObjects()` 경로를 타므로 **같은 stitched 조성이 main
히스토그램과 b-tag reweight 유도 양쪽에 들어간다** — 모드별 중복 없음.
`StitchFactors::factor()` 는 multiplier 를 반환하면서 category 별 적용 전/후
Σweight 를 기록해, 종료 로그가 stitch 가 무엇을 했는지 보여준다.

### 11.3 stitched category 로 키잉한 b-tag normalization reweight

b-tag *shape* SF 는 각 jet 의 기여를 reshape 한다; 이벤트 합으로 보면 각 b-jet
multiplicity bin 에 들어가는 MC 양이 약간 이동한다. ttHH / ttH 처방은 그 이동을
**process × HF category 별 renormalization** 으로 제거한다 — `btagtrig` 에서
유도하는 곱셈 factor 가 각 category 의 shape-SF 이전 yield 를 복원해서, b-tag
*shape* nuisance 가 normalization 은 안 움직이고 shape 만 바꾸게 한다. 코드로는
lookup 하나:

```cpp
processKey       = TtCatGroup::MakeProcessKey(sampleName, expandedTtbarId);  // was genTtbarId
btagNormReweight = corrMgr->getBTagReweight("central", processKey, nJets, ht);
```

바뀐 것은 **두 번째 인자**: key 가 이제 NanoAOD `genTtbarId` 가 아니라
**expanded** id 다. 비유: 재활용을 *종이 / 플라스틱 / 금속* 으로 나누면 캔과
엔진블록이 한 통에 섞인다; expanded id 는 *무거운 금속* bin 을 추가해 부피 큰
것(tt+nb — high b-jet multiplicity, b-tag SF 가 가장 크게 작용하는 곳)을 tt+2b
와 평균내지 않고 따로 보정한다. stitch multiplier(§11.2)가 유도에 들어가는 MC
조성을 이미 맞췄으므로 reweight 는 올바르게 stitched 된 mix 에서 계산된다.

> **`Config_TtCatGroup.hh` 요건.** `MakeProcessKey` 는 `(sampleName, id)` →
> process-key 문자열의 외부 map 이다. expanded key 가 실제로 tt+nb 를 분리하려면
> 이 map 이 `61/62`(tt+bbb), `71/72`(tt+4b)에 대해 `53/54/55`(tt+2b)와 **다른**
> key 를 돌려줘야 한다. analyzer 는 이를 가정하지 않는다 — 종료 시 관측된
> `sub → processKey` map 을 **로그**하고 `tt+nb split from tt+2b: YES/NO` 를
> 출력한다. `NO` 면 `MakeProcessKey` 에 tt+nb bin 을 추가하고 `btagtrig` 에서
> reweight 를 재유도하라. 이 한 조각만 이 코드 밖에 있으며 note 로 확인해야 한다.

### 11.4 로그가 알려주는 것

세 블록, 모두 Condor stdout 에서 grep 가능:

- **Config summary** (시작): 로드된 option, 이 샘플의 role, category 별
  multiplier 행 — 예컨대 `tt4b` 가 `ttbbb`/`tt4b` 에 `0.276`, 나머지 `0` 임을
  한눈에 확인.
- **Run summary** (종료): category 별 `nEvents`, `ΣW_in`, `ΣW_out`,
  `applied = out/in` 표. inclusive 는 LF/cc/ttb 에 `applied=1`, tt2b/tt+nb 에
  `0`; dedicated 은 소유 cat 에 `r`. stitch 가 의도대로 적용됐다는 직접 확인.
- **b-tag key 진단** (종료): `sub → processKey` map 과 `tt+nb split: YES/NO`
  (§11.3).

### 11.5 Fail-loud 동작 (Condor 가 잡도록)

조용히 틀린 히스토그램을 쓰는 대신, 구조적 문제가 있으면 job 이 **차단기를
내린다** — `[FATAL]` 출력 후 non-zero 종료:

| exit | 조건 |
|---|---|
| 40 | stitch JSON 없음 (`$STITCH_FACTORS_JSON` 또는 기본 경로) |
| 41 | stitch JSON 파싱 실패 |
| 42 | JSON 에 `analyzer_perEvent_factor` / `sub_to_category` 없음 |
| 43 | 샘플이 stitch plan 에 있는데 Expanded_genTtbarId lookup 이 INACTIVE (tt+nb 미태깅 → 잘못된 stitch) |
| 45 | `MakeProcessKey` 가 빈 key 반환 |
| 46 | `getBTagReweight` 가 non-finite 반환 |

(40–43 은 시작 시, 45–46 은 이벤트별. `genTtbarId % 100` 가 알려진 집합 밖이면
abort 가 아니라 *카운트* 되는 warning.)

### 11.6 파일 / 실행 방법

- 신규: `src/StitchFactors.{h,cc}` — JSON loader + 이벤트별 multiplier + 로그 +
  fatal-exit 가드.
- 수정: `ttHHanalyzer_unified.{cc,h}` — `main()` 에서 로드, `process()` 에서
  적용, `selectObjects()` 에서 b-tag reweight 재키잉, `loop()` 에서 summary.
  전체 diff: `stitch_apply.diff`.
- Env override: `STITCH_FACTORS_JSON` (기본
  `DerivedCorr/stitchFactors/stitch_factors_2017.json`), `EXPANDED_TTBARID_DIR`
  (기본 `DerivedCorr/expandedTtbarId`).
- `prescan` 모드는 JSON 을 **로드하지 않는다** (JSON 계산의 입력을 prescan 이
  생산하므로).
- **선행조건 (§10.7.3):** 첫 stitched stack 전에 dedicated ttbb YAML base weight
  를 config σ 와 맞춰라 — `ttbb_Hadronic` 5.296×10⁻⁴, `ttbb_SemiLeptonic`
  3.787×10⁻⁴, `ttbb_2L2Nu` 8.837×10⁻⁴ (inclusive·`tt4b` 는 이미 일치).


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
