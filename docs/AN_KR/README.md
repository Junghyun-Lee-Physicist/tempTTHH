# AN_KR — ttHH(→4b) FH 채널 한국어 분석 노트 (LaTeX)

> **Purpose:** 분석의 흐름·물리적 근거·바뀐 것·남은 것을 한 문서로 엮는 한국어 작업본(CMS 공식 AN 아님). 이 README 는 빌드와 작성 규칙이다.
> **Status:** v0.1 (2026-10-10; 코드 기준 STEP 27 P). 결정: [`../DECISIONS.md`](../DECISIONS.md) D-2026-10-10-B (4).
> **정본:** 숫자와 상태의 정본은 md 문서와 코드다. 이 문서는 그것을 출처와 함께 옮긴다(다르면 정본을 따르고 다름을 적는다).

## 빌드

```bash
cd docs/AN_KR
./build.sh                                   # 전체 → out/AN_KR.pdf (XeLaTeX 2–3 번)
./build.sh -o ~/somewhere                    # 다른 출력 디렉터리
./build.sh --chapter sections/05_btag.tex    # 한 장만 시험 → out/chapter_05_btag.pdf
./build.sh --keep                            # 임시 빌드 디렉터리를 남김(오류 조사)
```

- 필요: `xelatex`(TeX Live 2022 이상 또는 MacTeX), `kotex`, 글꼴 **Noto Serif CJK KR, Noto Sans CJK KR, Noto Sans Mono CJK KR**, `tikz-feynman`, `tcolorbox`.
  맥: MacTeX + Noto **CJK** 판 글꼴 셋(Google Fonts 의 "Noto Sans KR/Serif KR" 이 아니라 이름에 CJK 가 든 것; notofonts/noto-cjk 배포본).
  빌드 전에 `fc-list | grep "CJK KR"` 로 셋이 보이는지 확인한다(`build.sh` 도 확인한다).
- 끝 줄에 쪽 수, 정의 안 된 참조 수, 1 pt 이상 Overfull hbox 수가 나온다. 전체 빌드는 **0 / 0** 이어야 한다(한 장 빌드에서는 다른 장을 가리키는 참조가
  정의 안 된 것으로 나오는 것이 정상).
- **PDF 는 커밋하지 않는다**(`*.pdf` 와 `out/` 은 git 이 무시). 읽는 판은 맥 워크스페이스 `AN_KR_pdf/AN_KR_v<판>_<날짜>.pdf`.

## 파일

| 파일 | 내용 |
|---|---|
| `main.tex` | 표지, 차례, 장 순서, 판 표기(`\ANKRversion`, `\ANKRdate`) |
| `preamble.tex` | 글꼴·쪽 모양·매크로(아래). 새 매크로는 여기에만 |
| `sections/00_about.tex` … `13_plan.tex` | 본문 0–13 장 |
| `sections/A_treev1.tex`, `B_glossary.tex`, `C_sources.tex` | 부록: Tree v1 branch 전부, 용어, 출처 |
| `build.sh` | 빌드 |

## 작성 규칙

**문체.** "~다" 평서문(md 문서와 같음). 물리학자가 영어로 쓰는 말(jet, b-tag, scale factor, working point, trigger, fit, nuisance …)은 영어로 두고 처음 나올 때
짧게 풀이한다. "왜"를 쓴다: 직관, 유도의 줄기, 무엇이 틀릴 수 있는가. 장마다 첫머리에 요약 한 단락.

**장의 순서(대체로).** (a) 물리적 배경·이론 → (b) Run 2 ttHH AN 에서는 → (c) FH 고유는 ttH(bb) FH AN 에서는 → (d) 우리 FH 분석에서는(지금의 코드, 상태
표지) → (e) Run 3(2024) 조정과 근거 → (f) 변경 이력(STEP n, D-…) → (g) 미결 사항(`openq`).

**출처.** 숫자·사실 바로 뒤에 `\src{...}`:
- `\src{AN-22-122 v26, PDF p.23, Table 9}` — 언제나 PDF 쪽(인쇄 쪽 = PDF − 2)
- `\src{AN-19-094 v20, PDF p.101}` · `\src{사전승인 발표 p.N}` · `\src{승인 발표 p.N}` · `\src{Wei Wei, DPF 2026 p.N}` · `\src{Hbb 회의 FH 발표 p.N}` ·
  `\src{KCMS 발표 p.N}` · `\src{GATJA 발표 p.N}`
- 우리 기록: `\src{D-2026-10-10-A}`, `\src{STEP 27 §P}`, `\src{PLAN\_ML\_SYST §9}`, `\src{STATUS 10-09 (3)}`; 코드: `\src{TreeVars.h, chi2AN}`
- `\src{}` 안의 밑줄은 `\_`. 그림에서 읽은 값은 "그림 판독", 우리 해석은 `\tagours` 또는 "우리 판단". 교과서 수준의 설명은 출처 없이.
- 숫자를 기억으로 쓰지 않는다. 출처에 없으면 "확인 못 함"과 `openq`.

**매크로(`preamble.tex`).**
- 기호: `\ttbar \ttHH \ttH \ttZ \ttZH \ttZZ \ttW \tttt \ttbb \ttcc \ttjj \bbbar \Hbb \HHfourb \pt \HT \MET \kl \kt \dR \chisq \GeV \TeV \fb \pb \fbinv \sqrts`
  (글과 수식 모두에서 됨: `30\GeV`, `$\pt>30\GeV$`).
- 상태 표지: `\tagimpl`(구현됨) `\tagtest`(시험됨) `\tagplan`(계획) `\tagopen`(미결) `\tagan`(Run 2 AN 그대로) `\tagrunthree`(Run 3 조정) `\tagours`(우리 선택).
- 상자: `note`(이해를 위한 설명), `andiff`(Run 2 AN 과 다른 점 + 근거), `openq`(미결 사항), `impl`(우리 코드에서). 제목 바꾸기: `\begin{note}[제목]`.
- 파일·코드 이름: `\file{include/TreeVars.h}`, `\code{computeBTagWeightFixedWP_}`(밑줄 그대로, 빈칸도 그대로, 아무 데서나 줄바꿈).
  **`\section`, `\caption`, `\footnote`, `\src` 같은 다른 명령의 인자 안에서는 쓰지 않는다** — 거기서는 `\texttt{..\_..}`.
- `\emph{}` 는 고딕(sans)으로 찍힌다(Noto Serif CJK 에 기울임꼴이 없음).

**label.** 장: `ch:about, intro, samples, trigger, objects, btag, selection, reco, qcd, mva, syst, stats, status, plan, treev1, glossary, sources`.
그 밖은 장 열쇠를 앞에: `sec:btag-fixedwp`, `eq:btag-method1a`, `tab:btag-wp`, `fig:btag-flow`.

**표·그림.** booktabs(`\toprule \midrule \bottomrule`), 넓은 글은 `tabularx` 와 열 `L`/`C`, 긴 표는 `longtable`; 본문 폭 16.4 cm 안. 그림은 TikZ 만(외부 그림은
`figures/` 를 만들 때 정한다). Feynman 도형은 `tikz-feynman` — XeLaTeX 에서는 자동 배치가 안 되므로 `\vertex ... at (x,y)` 로 놓고 `\diagram*`.

## 고치는 규칙

- 코드가 바뀌면(새 STEP) 그 장의 "변경 이력"과 12 장의 표.
- 결정이 생기면(새 D-…) 그 장의 본문과 `openq`, 13 장의 결정 대기.
- 판을 올리면 `main.tex` 의 `\ANKRversion`/`\ANKRdate` 와 0 장의 개정 이력을 함께. 맥의 `AN_KR_pdf/` 에 그 판의 PDF.
- 전체 빌드가 정의 안 된 참조 0, Overfull 0 으로 끝나야 커밋한다.

## v0.1 을 만든 방법

장 초안은 AI 에이전트 넷이 원문(AN-22-122 v26, AN-19-094 v20 의 본문 추출본), 발표 여섯 편, 저장소의 기록과 코드를 읽고 썼고(0·12·13 장과 부록 B·C 는
조정자), 다른 에이전트 둘이 숫자·쪽·식을 원문과 코드에 대조해 62 곳을 고쳤다. 남은 의문은 각 장의 `openq` 에 있다.
