# STEP 21 — plotter: 전체 샘플 + 2-모드 그룹핑 + cutflow legend 수율 + PDF

- 날짜: 2026-07-10
- 변경: `plotter/stack_plotter.C`, `plotter/scenario_runner.py`,
  `plotter/samples_config.yml`

## DECIDED

### 1. 샘플 전체 등록 (MC 24 → 61)
`samples_config.yml` 에 37개 신규 MC 추가 (ttHToNonbb, tHq/tHW,
TTWJetsToQQ/LNu, TTZToLLNuNu, single-top 6종, WW/WZ/ZZ, W/ZJetsToQQ_HT 6종,
WJetsToLNu_HT 8종, DYJetsToLL_M50_HT 8종). Data 는 FH 기준 JetHT+BTagCSV 유지
(SingleMuon 은 muon CR 설정용 — 별도 yml 로). 아직 hadd 안 된 샘플은 파일
열기 실패 시 그 샘플만 조용히 skip 되므로 부분 준비 상태에서도 안전.

### 2. 그룹핑 2-모드 (env `TTHH_PLOT_GROUPING`, runner `--grouping`)

| | compact (기본, MC 10줄) | detailed (MC 13줄) |
|---|---|---|
| ttH/tH | t̄tH+tH 통합 | t̄tH / tH 분리 |
| tt+VV/VH, 3t/4t | t̄t+X (rare) 통합 | t̄t+VV/VH / 3t/4t 분리 |
| V+jets, VV | V+jets/VV 통합 | V+jets / VV 분리 |
| 공통 | QCD, t̄t, t̄t+b̄b, t̄t+4b, t̄t+V, **t̄t+ZH/ZZ(4b)**, single t, signal t̄tHH | 동일 |

- **TTZHTo4b/TTZZTo4b 는 두 모드 모두 별도 legend 줄** (`t#bar{t}+ZH/ZZ(4b)`,
  signal 유사 4b 배경이라 rare 에 묻지 않음 — 사용자 결정).
- output 은 `plots_compact/` / `plots_detailed/` 로 분리 (상호 덮어쓰기 방지).
- runner: `--grouping compact|detailed|both` (both = 두 모드 순차 실행,
  로그도 `plotter_<grouping>.log` 로 분리).

### 3. 매칭: substring → exact-name 테이블 (정확성 수정)
구 `GetProcessGroup` 은 substring 라우팅이라 순서 충돌 주석을 수동 관리했고
실제 오배정이 있었다: **TTWW/TTWZ(tt+VV), TTWH(tt+VH), TTZHTo4b/TTZZTo4b,
TTTW(3top) 가 전부 "t#bar{t}+V" 로 흡수**. STEP21 은
`GetFineProcessKey()` — exact-name map + HT-slice prefix 규칙(QCD_HT/
WJetsToLNu_HT/WJetsToQQ_HT/ZJetsToQQ_HT/DYJetsToLL_M50_HT) — 으로 교체.
구 hadd 별칭(tt4b/ttHH 소문자 등) 도 테이블에 등록해 하위호환. 미배정
샘플은 Other + stderr 경고 1회 (조용한 오배정 불가능).

### 4. cutflow legend 수율 = 최종 cut 이후 (`LegendYield`)
기존 legend 수치는 모든 hist 에서 `Integral(0,N+1)` — cutflow 에서는
"noCut+각 step 의 합"이라 noCut 지배 수치(≈ cut 전 total 로 보임)였다.
`IsCutflowHist()` + `LegendYield()` 도입: cutflow 는 **마지막 bin(nTotal)**,
일반 kinematic hist 는 기존 Integral 유지(그건 올바른 의미). Data/그룹/
Total MC 세 곳 모두 적용, 그룹 정렬(yield 내림차순)도 최종 수율 기준이 됨.
cutflow 플롯 legend 에 "yields after final cut" 헤더 명시. loop 후반의
중복 tail/isCutflow 재계산은 loop 상단 1회 계산으로 통합.

### 5. 출력 PNG → PDF
`SaveAs(...pdf)` (벡터 — DP note/Beamer 삽입 유리), runner 의 plot 카운트
glob 도 `*.pdf` 로.

## 검증
- **그룹핑 전수 단위 테스트 (g++ 실컴파일)**: `GetFineProcessKey`/
  `GetProcessGroup` 을 추출해 61개 샘플 + 구 별칭을 두 모드에서 전수 라우팅
  — ALL PASSED. 옛 오배정 3종(TTTW/TTWW/TTZHTo4b→ttV) 회귀 방어 포함.
  그룹 수 확인: detailed 13+signal, compact 10+signal.
- runner `ast.parse` OK. `stack_plotter.C` brace/paren balance 원본 대비
  변화 없음(balance-neutral).
- **미검증(환경 제약)**: ROOT 렌더링 (PDF 출력·legend 배치) — 로컬에서
  `TTHH_PLOT_GROUPING=detailed root -l -b -q stack_plotter.C` 1회 확인 권장.
  detailed 모드 legend 는 기존 `SetNColumns(2)` 로 13+3줄 수용 예상이나
  겹침 시 `TLegend(0.42,0.55,...)` 의 y0 를 낮출 것.

## 롤백
grouping 블록을 구 substring `GetProcessGroup` 으로, `LegendYield` 3개소를
`Integral(0,N+1)` 로, SaveAs 를 `.png` 로, runner 의 `--grouping`/env 전달
제거, samples_config.yml 신규 37개 블록 삭제.
