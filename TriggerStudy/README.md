# Trigger SF Analysis (ttHH Hadronic Channel)

CMS ttHH 전(全)-hadronic 분석을 위한 trigger scale factor (SF) 도출/적용/검증 파이프라인.
Single muon orthogonal control region에서 hadronic trigger의 efficiency를 측정하고,
Data/MC 비율로 SF를 계산해 correctionlib JSON으로 저장한 뒤, 같은 코드로 SF를 다시 적용해
검증 플롯을 생성한다.

> 깊은 구조/방법론은 [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) 참고.

---

## 0. 연도 (STEP 26 L, 2026-10-09 — 상세: `../docs/changes/STEP_26_trigger_sf_2024.md`)

연도는 env `TTHH_YEAR`(없으면 2017 = 이전 동작)로 정하고, `run_analysis.sh --year` 가 그것을 export 한다.

| | 2017 | 2024 |
|---|---|---|
| reference(직교) trigger | `HLT_IsoMu27` | `HLT_IsoMu24` |
| hadronic OR | era B 는 `_B` slot(CSV 이름), C–F 는 `_CDEF` | `_CDEF` slot 만(analyzer 가 PNet 경로를 넣음; Data 는 4J3T DeepJet 도), `_B` 는 0 이어야 함 |
| Data | `SingleMuon_Run2017B..F` | xsec_db 의 `Muon0/1_Run2024*` 16 개 |
| xsec_db / prescan 기본값 | `data/samples_2017UL.json`, `prescan_summary/` | `data/samples_2024.json`, `prescan_summary_2024/` |
| 출력 위치 기본값 | `TriggerStudy/`(이전처럼) | `TriggerStudy/run_2024/` |

```bash
# 2024 (KNU, cmsenv 뒤; TTHH_SKIM_DIR = merge 된 btagtrig 출력)
TTHH_SKIM_DIR=/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/AnalyzerOutput_btagtrig_notrig_2024 bash run_analysis.sh --year 2024 --jobs 4
```

다른 해의 입력은 멈춘다(exit 1 또는 `RESULT FAIL`): `TTHH_YEAR` 없이(=2017) 2024 skim(`passTrigger_HLT_IsoMu24` 가 있음), 2024 에
2017 xsec_db(`_meta.era`)나 2017 prescan(`meta.year`, 없으면 `meta.input_base` 의 `_<YYYY>`), 다른 해의 Data 이름, Step 2 의 다른 해
JSON, reference 가 한 번도 안 켜진 sample(muon CR 100 개 이상), 다른 해의 Step 1/2 출력을 DeriveSF/PlotTriggerEfficiency 로
(`TrigStudyStamp`: EventLooper 가 출력마다 `year=...; reference=...` 를 남김; 표시가 없는 옛 출력은 2017 로만 받음). DeriveSF 는
측정된 bin 이 없는 category 가 있으면 `RESULT FAIL`(exit 1): 이번 JSON 은 `trigger_sf.json.gz.FAILED`, 있던 것은
`trigger_sf.json.gz.previous` 로(아무것도 지우지 않음); 입력이 없거나 다른 해면 디렉터리를 건드리지 않는다. JSON 의 description 에
`year=<YYYY>; reference=...; hadronic OR: ...` 가 들어가고, analyzer 는 다른 해의 JSON 을 거부한다(E50). 알려진 빈틈: 손으로 한
hadd 가 표시 없는 옛(2017) 출력과 2024 출력을 섞으면 잡지 못한다 — run_analysis.sh 를 쓴다.

합성 입력으로 전체를 시험(설치 확인용, 몇 분): `bash ../test/trigger_study/run_trigstudy_synth.sh <빌드된 tempTTHH>`.

---

## 1. Quickstart

```bash
# 1) 빌드
make            # exe_TrigStudy 생성
make info       # (선택) 빌드 변수 dump
make clean

# 2) 전체 파이프라인 실행 (2017; 2024 는 위 0 절)
bash run_analysis.sh            # = --year 2017, 출력은 TriggerStudy/
```

`run_analysis.sh`는 다음을 순차/병렬 수행한다:

1. **Step 1** — `exe_TrigStudy <sample> 0`을 모든 샘플에 병렬 실행 → `output_<sample>.root`
2. **hadd merge** → `output_SingleMuon.root`, `output_TTbarInc.root`
3. **DeriveSF** — `root -l -b -q DeriveSF.cpp` → `TriggerSF.root`, `trigger_sf.json.gz`
4. **Step 2** — `exe_TrigStudy <sample> 1` 병렬 실행 → `validated_<sample>.root`
5. **hadd merge** → `validated_SingleMuon.root`, `validated_TTbarInc.root`
6. **Plotting** — `root -l -b -q PlotTriggerEfficiency.cpp` → `Validation_TrigEff_*.pdf`

각 단계는 `set -euo pipefail` + `wait_and_check`로 한 프로세스라도 실패하면 즉시 중단된다.

---

## 2. Prerequisites

| 항목 | 비고 |
|---|---|
| ROOT | 6.x 이상 (`root-config` PATH 필요) |
| correctionlib | C++ 헤더 + 런타임. `correction config --cflags/--ldflags` 동작해야 함 |
| nlohmann/json | header-only, `include/nlohmann/json.hpp`로 vendoring |
| zlib | DeriveSF의 `.json.gz` 압축용 |
| 컴파일러 | C++17 이상. macOS는 clang++, Linux는 g++ 자동 감지 |
| Python3 | macOS에서 correctionlib rpath 추출용 (Makefile 내부) |

---

## 3. 디렉토리 구조

```
project_root/
├── Makefile                       # OS 자동 감지 + auto-dep tracking
├── run_analysis.sh                # 전체 파이프라인 오케스트레이터
├── TriggerSFCalculator.cpp        # main(), thin entry wrapper
├── DeriveSF.cpp                   # ROOT 매크로: SF 도출 + JSON 빌드
├── PlotTriggerEfficiency.cpp      # ROOT 매크로: validation 플롯
├── include/
│   ├── Config.hh                  # 중앙 설정 매니저 (가장 자주 만지는 파일)
│   └── nlohmann/json.hpp          # vendored
├── src/
│   ├── EventLooper.{hh,cpp}       # 핵심 분석 클래스
│   └── NtupleReader.{hh,cc}       # TTree branch wrapper
└── tmp/                           # 빌드 산출물 (gitignore 권장)
```

---

## 4. Config.hh — 가장 자주 손대는 파일

모든 설정이 `Config` 클래스에 inline static으로 들어 있다. 외부 파일 없음. 변경 후 재빌드 필요 (`include`되므로).

### 4-A. 반드시 본인 환경에 맞춰야 할 것

```cpp
// Section 1
static inline const std::string inputBaseDir =
    "/Users/jhlee/ttHH/ntuple/skimmed/gen_tier3/";   // ← 본인 경로
static inline const std::string treePath = "Tree/Tree";
```

### 4-B. Sample Registry (새 샘플 추가 지점)

`Config::SampleRegistry()` 안의 `std::map`에 한 줄 추가:

```cpp
{"NewSample_Name", {false, 0.000123}}   // {isData, weight}
// MC weight = (Xsec * Lumi) / Sum(genEventSumw)  ← 사전 계산
// Data는 항상 {true, 1.0}
```

샘플이 등록되어 있지 않으면 `EventLooper`가 `[FATAL] Unknown sample`로 즉시 중단한다.

### 4-C. Analysis Flags

| 플래그 | 현재 값 | 의미 |
|---|---|---|
| `useNBjets` | `true` | NB-jets 카테고리화 활성 (3-bin: `nB0to2`/`nB3`/`nB4p`) |
| `useEta` | `false` | 6번째 jet η 카테고리화 (현재 inclusive) |
| `useFitInterpolation` | `false` | DeriveSF의 빈 bin 채우기: `true`면 2D quad-log fit, `false`면 nearest extrapolation |
| `verbose` | `false` | 이벤트 루프 verbose 출력 |
| `progressInterval` | `10000000` | 진행 상황 보고 주기 (이벤트 수) |
| `kMinPassData` | `10.0` | Data low-stat 기준 (Pass 이벤트 수) |
| `kMinNeffMC` | `20.0` | MC low-stat 기준 (Σw²로부터 계산한 N_eff) |

> **주의**: `useNBjets`/`useEta`를 바꾸면 SF의 차원이 바뀌어 correctionlib JSON schema도 바뀐다. 반드시 Step1부터 다시 돌려야 함.

### 4-D. Binning (HT × pT × η × NB)

`Config::InitBinning()` 안에서 직접 수정:

```cpp
_htBins  = {500, 550, 600, 700, 800, 1000, 1500, 2500};  // GeV
_ptBins  = {40, 45, 50, 60, 70, 120, 200};                // 6th jet pT, GeV
_etaBins = {-2.4, -1.2, 0.0, 1.2, 2.4};                   // useEta=true일 때만
_nbBins  = {
    NBBin::Range(0, 2),   // "nB0to2"
    NBBin::Exact(3),      // "nB3"
    NBBin::AtLeast(4)     // "nB4p"
};
```

`NBBin`은 `Exact(n)`, `Range(lo,hi)`, `AtLeast(n)` 세 가지 factory만 쓴다. 카테고리 간 overlap이 있으면 `Config::Validate()`가 잡아준다.

---

## 5. 단계별 수동 실행

전체 파이프라인을 한꺼번에 돌리지 않고 단계별로 디버깅할 때. **[STEP 26 L] 먼저 `export TTHH_YEAR=2017`(또는 2024)**,
그리고 macro 용 `export ROOT_INCLUDE_PATH=$PWD:$PWD/include:$PWD/../include` — 연도가 없으면 2017 이고, 2024 skim 이면
EventLooper 가 멈춘다. 샘플 이름은 campaign 이름(`SingleMuon_Run2017B`, `TTbar_DiLep`, `Muon0_Run2024C-MINIv6NANOv15-v1`):

```bash
# 2017 (2024: TTHH_YEAR=2024 에 TTHH_SKIM_DIR, TTHH_XSEC_DB=../data/samples_2024.json,
#       TTHH_PRESCAN=../prescan_summary_2024/prescan_summary.json 도 — 다른 해의 xsec_db·prescan 이면 exe 가 멈춘다)
export TTHH_YEAR=2017
export ROOT_INCLUDE_PATH=$PWD:$PWD/include:$PWD/../include

# Step 1만 (특정 샘플 하나)
./exe_TrigStudy SingleMuon_Run2017B 0
./exe_TrigStudy TTbar_DiLep 0

# Merge
hadd -f output_SingleMuon.root output_SingleMuon_Run2017?.root
hadd -f output_TTbarInc.root output_TTbar_DiLep.root output_TTbar_Hadronic.root output_TTbar_SemiLep.root

# DeriveSF만 (exit 0 = RESULT OK)
root -l -b -q DeriveSF.cpp

# Step 2 (SF 적용)
./exe_TrigStudy TTbar_DiLep 1

# Plotting만
root -l -b -q PlotTriggerEfficiency.cpp
```

---

## 6. 출력 산출물

| 단계 | 파일 | 내용 |
|---|---|---|
| Step 1 | `output_<sample>.root` | NB×Eta 카테고리별 (HT × pT) 2D `Total`/`Pass` 맵 + 1D 히스토그램 |
| DeriveSF | `TriggerSF.root` | 카테고리별 efficiency, SF (measured/filled), error 2D 히스토그램 |
| DeriveSF | `trigger_sf.json.gz` | correctionlib schema v2 JSON (Step 2의 입력) |
| DeriveSF | `*.pdf` | Data/MC Total/Pass counts, efficiency, SF, SF error 시각화 |
| Step 2 | `validated_<sample>.root` | `_noSF`/`_SF` 1D 히스토그램 (HT, pT, Eta, nbJets) |
| Plotting | `Validation_TrigEff_<var>.pdf` | Data vs MC(no SF) vs MC(with SF) + ratio panel |

---

## 7. 흔한 작업

### 7-A. 새로운 MC 샘플 추가

1. `Config::SampleRegistry()`에 `{"SampleName", {false, weight}}` 한 줄 추가
2. `run_analysis.sh`의 Step1/Step2 블록에 `./exe_TrigStudy SampleName <mode> &` 두 줄 추가
3. (선택) 머지 단계의 `hadd` 패턴이 새 샘플을 포함하는지 확인
4. `make` (Config.hh가 inline이므로 필수)

### 7-B. Binning 변경

1. `Config::InitBinning()` 수정
2. `make`
3. **Step 1부터 전체 재실행** — SF schema 자체가 바뀌므로 hadd된 결과 재사용 불가

### 7-C. SF 도출 디버그 모드 (correctionlib 우회)

`EventLooper.cpp` 내 `useDebugSF` 토글:

```cpp
bool useDebugSF = false;  // ← true로 바꾸면
```

`true`로 바꾸면 Step 2에서 JSON 대신 `TriggerSF.root`의 `Histograms/SF_measured_*` 히스토그램을 직접 `FindBin` lookup으로 사용한다. JSON 빌드 단계에 버그가 의심될 때 비교용으로 사용. 변경 후 `make` 필수.

### 7-D. Validation에서 추가 변수 확인

`Config::useEta = true` 또는 `useNBjets = true`로 바꾸면 `PlotTriggerEfficiency.cpp`가 자동으로 해당 변수 플롯을 추가 생성한다 (조건부 분기 내장됨).

---

## 8. 안전장치 (Fail-fast Invariants)

이 코드는 의도적으로 **fail-fast** 설계다. 다음 조건 위반 시 즉시 `exit`:

- ntuple skim invariant: `nJets ≥ 6`, `6th jet pT > 40`, `HT ≥ 500`, `|η₆| ≤ 2.4`, `passGoldenJson` (Data) → `exit(1)`
- Data인데 `weight ≠ 1` 또는 `sf ≠ 1` → `exit(55)`
- MC, `!passHadTrig`인데 `sf ≠ 1` → `exit(56)`
- correctionlib `evaluate()` 예외 → `exit(57)`
- ntuple의 `passHadTrig`와 코드에서 재계산한 OR 결과 불일치 → `exit(1)`
- 등록되지 않은 샘플 이름 → `exit(1)`
- [STEP 26 L] 다른 해의 입력(0 절의 목록) → `exit(1)`; 2024 skim 의 `_B` slot 이 켜짐 → `exit(1)`; DeriveSF 의 측정 bin 없는
  category·다른 해의 Step 1 출력 → `RESULT FAIL`, exit 1, `trigger_sf.json.gz` 없음

이 중 어느 하나가 터지면 보통 ntuple 단계의 문제이거나 Config 누락이다. **에러 코드 번호로 어디서 죽었는지 즉시 식별 가능**.

---

## 9. Troubleshooting

| 증상 | 가능한 원인 | 조치 |
|---|---|---|
| `Unknown sample` | Config registry 누락 | 4-B 참고 |
| `Cannot open` (input) | `inputBaseDir` 잘못됨 | 4-A 확인 |
| `Cannot open` (JSON) | DeriveSF 단계 미수행 | Step1 → DeriveSF 순서 확인 |
| `evaluate failed` (`exit 57`) | JSON과 EventLooper의 입력 차원 불일치 | binning/flag 변경 후 Step1부터 재실행 |
| macOS dyld 오류 | correctionlib rpath 못찾음 | `make info`로 `CORR_LIB_PATH` 확인 |
| validation ratio가 1에서 크게 벗어남 | low-stat bin 채우기 실패 | `useFitInterpolation` 토글, `kMinPassData/kMinNeffMC` 조정 |

---

## 10. 자세한 방법론

- Trigger SF 측정 전략 (orthogonal IsoMu27 reference)
- Clopper-Pearson vs N_eff binomial 오차 처리의 근거
- 빈 bin 채우기 (2D quadratic fit in log-space) 알고리즘
- SF 적용 시 numerator-only 보정의 이유
- correctionlib JSON schema 구조

→ 모두 [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md)에 정리.
