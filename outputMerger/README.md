# ttHH(4b) FH — Validation Study Pipeline

이 디렉토리는 ttHH(4b) Fully Hadronic 분석의 **Step 8 (≥4 b-tag) 영역에서
Data/MC ratio가 무너지는 원인 진단**을 위한 도구 모음입니다.
analyzer 의 cut 과 SF 를 시나리오별로 toggle 하여 condor 일괄 제출하고,
결과를 stack plot 으로 비교합니다.

## Table of Contents

1. [전체 흐름](#전체-흐름)
2. [구성 파일](#구성-파일)
3. [환경 준비](#환경-준비)
4. [Phase 1 — analyzer 빌드](#phase-1--analyzer-빌드)
5. [Phase 2 — condor submit](#phase-2--condor-submit)
6. [Phase 3 — plotting](#phase-3--plotting)
7. [Phase 4 — 결과 해석](#phase-4--결과-해석)
8. [scenario 추가/수정](#scenario-추가수정)
9. [정책 변경 — 어느 파일을 고쳐야 하나](#정책-변경--어느-파일을-고쳐야-하나)
10. [troubleshooting](#troubleshooting)

---

## 전체 흐름

```
[ analyzer (validation mode) ]
        │
        │ 12 scenario × 37 sample × per-file split
        ▼
[ condor jobs (analysis) ]        ← submit_job_FH_Tier3_unified.py
        │
        │ output: <base>/<scenario>/<sample>/<sample>_<n>.root
        ▼
[ condor jobs (hadd) ]            ← submit_hadd_validation.py + run_one_hadd.sh
        │
        │ output: <base>/<scenario>/<sample>.root (one per (scenario, sample))
        ▼
[ scenario_runner.py ]            ← per-scenario yml 생성 + plotter 호출
        │
        │ uses: extract_structure.py + stack_plotter.C
        ▼
[ <base>/_plot_workdir/<scen>/plots/*.png ]
```

각 단계 산출물:

| 단계 | 입력 | 출력 |
|---|---|---|
| Phase 1 | analyzer 소스 | `ttHHanalyzer_unified` binary |
| Phase 2 | YAML scenario 정의 + sample list | condor jobs → split ROOT 파일 다수 |
| Phase 2½ | split ROOT 파일들 | per-(scenario, sample) merged ROOT 파일 |
| Phase 3 | merged ROOT 파일 | per-scenario PNG 다수 |
| Phase 4 | PNG | 진단 결론 |

---

## 구성 파일

이 wrapping pipeline 은 **6 개 파일** 로 이뤄집니다:

| 파일 | 역할 | 수정 빈도 |
|---|---|---|
| `Config_TtCatGroup.hh` | process group 분류 + stitching weight (analyzer + b-tag SF tool 공유) | 낮음 |
| `WeightAblation.h` | (옵션) 7 chain weight 평행 fill 인프라. 현재 미사용 | 거의 안 함 |
| `PATCH_INSTRUCTIONS.md` | analyzer 에 적용할 patch 들의 line 번호 + 교체 블록 | 1회 |
| `main_function.cpp` | analyzer driver `int main()` 의 새 본문 (validation 인자 wiring) | 1회 |
| `Tier3_2017_FH_unified_validation.yml` | 12 scenario 정의 + sample list | scenario 추가 시 |
| `submit_job_FH_Tier3_unified.py` | condor 제출 (scenario × sample 이중 loop) | 거의 안 함 |
| `extract_structure.py` | ROOT 파일 → flat plottable hist list (single source of truth) | 정책 변경 시 |
| `stack_plotter.C` | yml → PNG. 정책 없음, 단순 plot 도구 | yields/ratio 표현 변경 시 |
| `scenario_runner.py` | 12 scenario 일괄 plot 실행 wrapper | 거의 안 함 |

### 단일 진실 원칙 (single source of truth)

| 정책 | 정해지는 곳 |
|---|---|
| 어떤 hist 를 그릴 것인가 | `extract_structure.py` |
| 어떤 cut 을 toggle 할 것인가 | `Tier3_2017_FH_unified_validation.yml` (scenario block) |
| process group 매핑 | `Config_TtCatGroup.hh::MakeProcessKey` |
| sample stitching | `Config_TtCatGroup.hh::StitchingWeight` |
| SF 적용 시점 / 가산 순서 | `ttHHanalyzer_unified.cc::selectObjects` (PATCH C3) |

다른 도구 (`stack_plotter.C`, `scenario_runner.py`) 는 정책을 갖지 않습니다.
이 원칙 위배 (예: plotter 안에서 hist 종류 filter) 가 발견되면 즉시 정책
파일 쪽으로 옮겨 주세요.

---

## 환경 준비

### CMSSW + ROOT
사용자 환경에서 사용 중인 CMSSW + ROOT (예: CMSSW_14_2_1) 활성화:

```bash
cd ${CMSSW_BASE}/src
cmsenv
```

### Python 의존성

`uproot` 가 필요합니다 (`extract_structure.py`, `scenario_runner.py` fallback):

```bash
python3 -c "import uproot" || pip install --user uproot
```

다른 의존 라이브러리는 모두 stdlib.

### Proxy

condor 제출 시:

```bash
voms-proxy-init -voms cms -valid 192:0
cp /tmp/x509up_u$(id -u) ${ANALYZER_DIR}/proxy.cert
```

---

## Phase 1 — analyzer 빌드

### 1.1 사전 작업 (이미 했으면 skip)

다음 6 patch 가 적용된 상태여야 합니다:

| patch | 대상 | 무엇 |
|---|---|---|
| H1 | `ttHHanalyzer_unified.h` line 51 | `kValidationStudy` enum 추가 |
| H2 | `ttHHanalyzer_unified.h` line 58, 81 | parser 와 모드명 갱신 |
| H3 | `ttHHanalyzer_unified.h` line 103 | SelectionPolicy::fromMode 에 `kValidationStudy` case 추가 |
| H4 | `ttHHanalyzer_unified.h` line ~1023 직전 | `ValidationConfig` 구조체 + `setValidationConfig()` |
| C1 | `ttHHanalyzer_unified.cc` line 710 | Higgs reco 를 `forceHiggsRecoMinBjets` 로 gate |
| C2 | `ttHHanalyzer_unified.cc` line 856 | Step-8 nbJets cut + Step-9 hadW window 가 validation toggle 따르도록 |
| **C3** | `ttHHanalyzer_unified.cc` line 893–947 | **SF 곱셈 블록을 step 8 *전* 으로 이동** + 각 SF 가 toggle 따르도록 |
| Driver | `int main()` | `--val-*` 인자 → `ValidationConfig` → `analyzer.setValidationConfig()` |

**상세는 `PATCH_INSTRUCTIONS.md`** 참조. C3 가 가장 중요한 변경입니다.

### 1.2 tnm.h / tnm.cc 확장 (이미 했으면 skip)

`commandLine` struct 에 11 개 `val*` 멤버 + `decode()` 가 `--val-*` flag
parsing 하도록 확장 — 사용자가 이미 적용한 상태로 가정.

### 1.3 Config_TtCatGroup.hh

`bTagSF_ReweightStudy/include/Config_TtCatGroup.hh` 에 패치된 버전 사용
(D2 strict + StitchingMode/Weight API).

### 1.4 빌드

```bash
cd ${ANALYZER_DIR}
make clean && make
```

빌드 완료 후 `./ttHHanalyzer_unified` binary 생성.

### 1.5 dry-run 검증 (필수)

condor 던지기 전 1 sample 로 sanity check:

```bash
./ttHHanalyzer_unified \
    --filelist filelistTier3/filelist_TTToHadronic.txt \
    --output /tmp/test_val.root \
    --weight 0.000214351205 \
    --year 2017 \
    --dataOrMC MC \
    --sample TTToHadronic \
    --mode validation \
    --val-scenario sanity_test \
    --val-nbJetsCut 2
```

**기대 출력**:

```
[Validation] applying scenario: [ValidationConfig] scenario=sanity_test
                                nbJetsCut=2 hadWWin=1 ... | SF: btagShape=1 ...
```

`nbJetsCut=2` 가 출력되면 argv → ValidationConfig 배선 정상.

ROOT 파일도 점검:

```bash
root -l -b -q -e '
  TFile f("/tmp/test_val.root");
  f.ls();
  TH1F* h = (TH1F*)f.Get("Tree/cutflow_w");
  if (h) {
    cout << "cutflow OK: " << h->GetEntries() << " entries" << endl;
  } else cout << "cutflow MISSING" << endl;
'
```

---

## Phase 2 — condor submit

### 2.1 YAML 배치

`Tier3_2017_FH_unified_validation.yml` 을 분석 디렉토리의 표준 위치로:

```bash
cp Tier3_2017_FH_unified_validation.yml \
   ${ANALYZER_DIR}/AnalyzerConfig/
```

YAML 안에는 12 scenario 가 정의돼 있음:

| 그룹 | scenario | 목적 |
|---|---|---|
| A | `baseline` | sanity (main mode 와 동일 결과 expected) |
| B | `noBjetCut`, `noHadWWindow` | 단일 cut N-1 |
| C | `bcut0`, `bcut1`, `bcut2`, `bcut3` | b-jet cut variation (사용자 핵심 hypothesis) |
| D | `noSF_all`, `noBtagShape`, `noBtagNorm`, `noTrigSF` | SF ablation |
| E | `ttHVR` | ttH AN VR-style preset |

### 2.2 submit script 배치

```bash
cp submit_job_FH_Tier3_unified.py ${ANALYZER_DIR}/
```

### 2.3 작은 batch 로 검증 (권장)

처음엔 1 scenario × 1 sample 만 돌려보기:

YAML 임시 백업 후 `validation_scenarios:` 와 `samples:` 를 한 줄씩 줄여서:

```yaml
validation_scenarios:
  - name: baseline
    nbJetsCut: 4
    ...
samples:
  - filelist: "filelist_TTToHadronic.txt"
    output_dir: "TTToHadronic"
    ...
```

```bash
python3 submit_job_FH_Tier3_unified.py
```

condor queue dispatch + output dir 생성 확인 후 원본 YAML 복원.

### 2.4 전체 submit

```bash
cd ${ANALYZER_DIR}
python3 submit_job_FH_Tier3_unified.py
```

콘솔 출력:

```
======================================================================
[VALIDATION] Submitting scenario: baseline
======================================================================
Setting up job for sample: [baseline] SingleMuon_B
Submitting job for sample: [baseline] SingleMuon_B
...
======================================================================
[VALIDATION] Submitting scenario: bcut0
======================================================================
...
```

job 수: **12 scenarios × 37 samples × N file splits** ≈ 수백–수천 jobs.

### 2.5 출력 구조

```
${path_output_base}/
├── baseline/
│   ├── BTagCSV_B/
│   │   ├── BTagCSV_B_0.root
│   │   └── BTagCSV_B_1.root
│   ├── TTToHadronic/
│   └── ...37 sample dirs
├── bcut0/
│   └── ...same 37 dirs
├── ...12 scenarios
```

### 2.6 disk / queue 주의

- output 이 12배 증가 (scenario 수). `df -h` 또는 사용자 quota 미리 확인.
- KISTI Tier3 condor 에 한 번에 너무 많은 job dispatch 시 queue 정책 위반.
  필요 시 5 scenario 씩 나눠 던지거나 YAML 임시 trim.

---

## Phase 2½ — per-(scenario, sample) hadd via condor

분석 jobs 가 끝나면 `<base>/<scen>/<sample>/` 에 split ROOT 파일들이 있고
plotter 가 이를 in-memory 합산 가능하지만, **로컬에서 plotter 가 매번 split
파일을 모두 열어야 해서 매우 느림**. 그래서 hadd 단계를 condor 로 분산:

### 2½.1 두 파일 배치

`submit_hadd_validation.py` 와 `run_one_hadd.sh` 를 같은 디렉토리에 두기:

```bash
# 권장 layout — analyzer 디렉토리 아래 outputMerger/ 서브디렉토리에 모음:
mkdir -p ${ANALYZER_DIR}/outputMerger
cp submit_hadd_validation.py run_one_hadd.sh \
   stack_plotter.C extract_structure.py scenario_runner.py \
   ${ANALYZER_DIR}/outputMerger/
chmod +x ${ANALYZER_DIR}/outputMerger/run_one_hadd.sh
cd ${ANALYZER_DIR}/outputMerger
```

### 2½.1a Proxy 배치

`submit_hadd_validation.py` 는 **자기 위치** 의 `proxy.cert` 를 자동으로
default 사용합니다. 따라서 위 layout 기준 proxy 배치:

```bash
voms-proxy-init -voms cms -valid 192:0
cp /tmp/x509up_u$(id -u) ${ANALYZER_DIR}/outputMerger/proxy.cert
```

이미 analyzer-root 에 `proxy.cert` 를 두고 그것을 재사용하고 싶다면
`--proxy ../proxy.cert` 옵션으로 override 가능:

```bash
python3 submit_hadd_validation.py --proxy ../proxy.cert
```

### 2½.2 dry-run 으로 작업 list 확인

```bash
python3 submit_hadd_validation.py --dry-run
```

콘솔에 발견된 (scenario, sample) 쌍 수가 표시됨:
```
[discover] 432 jobs across 12 scenarios:
             baseline               37 samples
             bcut0                  37 samples
             ...
             ttHVR                  35 samples
```

생성된 `condor_hadd_<timestamp>/` 디렉토리에:
- `arguments.txt` — 한 줄에 `<indir> <outfile>` (한 hadd job 의 인자)
- `hadd.sub` — condor submit description

### 2½.3 제출

```bash
python3 submit_hadd_validation.py
```

기본 동작:
- `<base>/_plot_workdir` 같은 wrapper 디렉토리는 자동 skip (밑줄 prefix)
- 빈 sample 디렉토리도 자동 skip
- 모든 살아있는 (scenario, sample) 쌍에 대해 condor job 생성

### 2½.4 자주 쓰는 옵션

```bash
# 일부 scenario / sample 만:
python3 submit_hadd_validation.py --scenarios baseline bcut0
python3 submit_hadd_validation.py --samples TTToHadronic ttHH

# 이미 merge 된 것 skip (재실행 시 유용):
python3 submit_hadd_validation.py --skip-existing

# 메모리 / OS 변경:
python3 submit_hadd_validation.py --memory '8 GB' --os-version el9
```

### 2½.5 출력 구조

각 hadd job 이 끝나면:
```
<base>/<scen>/<sample>/             # split files (그대로 유지)
<base>/<scen>/<sample>.root         # NEW: merged target
```

동시에 `<base>/<scen>/<sample>/` 안의 split 파일은 **그대로 보존**됩니다 —
재실행 / 검증 / 일부 split 만 다시 더하기 등 가능.

### 2½.6 진행 상황 모니터

```bash
condor_q $USER                                     # queue 상태
ls condor_hadd_<timestamp>/logs/                   # job 별 로그
tail -f condor_hadd_<timestamp>/logs/job.*.out     # live 출력
```

완료 후 검증:
```bash
# 기대 파일 수와 실제 비교
EXPECTED=$(wc -l < condor_hadd_<timestamp>/arguments.txt)
ACTUAL=$(ls /pnfs/.../AnalyzerOutput_validation/*/*.root 2>/dev/null | wc -l)
echo "expected=$EXPECTED  actual=$ACTUAL"
```

실패한 job 재실행:
```bash
python3 submit_hadd_validation.py --skip-existing
```
(이미 만들어진 .root 는 건너뛰고 빠진 것만 재제출.)

### 2½.7 hadd 가 끝난 뒤 plotter 의 동작

기본 구조에서는 충돌 없습니다:

```
<base>/<scen>/<sample>/             # 디렉토리 — split files 들어있음
<base>/<scen>/<sample>.root         # 파일    — merged 결과
```

`scenario_runner.py::scan_scenario` 는 다음 우선순위로 sample 의 ROOT 파일을
선택합니다:

1. **merged `<base>/<scen>/<sample>.root` 가 존재하고 크기 > 0** → 그것 1개만 사용 (빠름)
2. 그 외 → split `<base>/<scen>/<sample>/*.root` 모두 (느림, fallback)

따라서 hadd 가 끝나면 **plotter 가 자동으로 merged 파일을 사용** — 사용자는
이전과 동일하게 `python3 scenario_runner.py ...` 만 실행하시면 됩니다. split
파일은 그대로 보존되므로 부분 재실행이나 검증도 그대로 가능.

---

condor jobs 모두 끝난 후 진행.

### 3.1 plotter 디렉토리 준비

권장 layout 따랐다면 (Phase 2½.1) 이미 `outputMerger/` 에 모든 plot 도구가
있습니다:

```bash
cd ${ANALYZER_DIR}/outputMerger
ls
# extract_structure.py  scenario_runner.py  stack_plotter.C
# submit_hadd_validation.py  run_one_hadd.sh  proxy.cert  README.md
```

별도 위치를 쓰려면 다음과 같이 복사:

```bash
mkdir -p ${WORK}/plotter
cp stack_plotter.C extract_structure.py scenario_runner.py ${WORK}/plotter/
cd ${WORK}/plotter
```

### 3.2 (선택) 정책 미리 확인

`extract_structure.py` 가 어떤 hist 를 plottable 로 분류할지 단독 실행해서
먼저 check 가능:

```bash
python3 extract_structure.py \
    --input /pnfs/.../baseline/TTToHadronic/TTToHadronic_0.root \
    --output /tmp/sample_struct.yml
head -30 /tmp/sample_struct.yml
wc -l /tmp/sample_struct.yml
```

원치 않는 hist 가 보이면 filter 추가:

```bash
python3 extract_structure.py \
    --input /pnfs/.../baseline/TTToHadronic/TTToHadronic_0.root \
    --output /tmp/sample_struct.yml \
    --exclude '^Tree/Tree$'        # TTree 자체 객체는 안 그림
```

### 3.3 일괄 plot 실행

```bash
python3 scenario_runner.py \
    --base    /pnfs/.../AnalyzerOutput_validation \
    --plotter ./stack_plotter.C \
    --jobs    4
```

기본 동작:

1. master `structure_info.yml` 1개 생성 (첫 scenario 의 TTToHadronic 기준)
2. 12 scenario 각각:
   - `samples_config.yml` 생성 (per-sample file list)
   - `structure_info.yml` master 복사
   - `stack_plotter.C` 복사
   - `cd <workdir>/<scen> && root -l -b -q stack_plotter.C` 실행
3. 결과: `<base>/_plot_workdir/<scen>/plots/<varname>.png`

### 3.4 결과 위치

```
${path_output_base}/_plot_workdir/
├── baseline/
│   ├── samples_config.yml
│   ├── structure_info.yml
│   ├── stack_plotter.C
│   ├── plotter.log
│   └── plots/
│       ├── Tree_cutflow.png
│       ├── Tree_cutflow_w.png             ← cutflow stack plot
│       ├── CutflowKinematics_cutStep_8_ht.png
│       ├── CutflowKinematics_cutStep_8_higgs_can01.png
│       ├── CutflowKinematics_cutStep_8_higgs_can02.png
│       └── ... per-step kinematics
├── bcut0/
│   └── plots/...
└── ...12 scenarios
```

### 3.5 자주 쓰는 옵션

```bash
# 일부 scenario 만:
python3 scenario_runner.py \
    --base /pnfs/.../AnalyzerOutput_validation \
    --plotter ./stack_plotter.C \
    --scenarios baseline bcut0 bcut1 bcut2 bcut3

# 디버그 hist 제외하고 plot:
# (extract_structure.py 를 직접 실행해서 master yml 미리 만들고,
#  scenario_runner 가 그 master 를 reuse 하도록 -- 단 현 구현은
#  master 추출 시 CLI flag forwarding 안 됨. 수동 우회:)
python3 extract_structure.py \
    --input /pnfs/.../baseline/TTToHadronic/TTToHadronic_0.root \
    --output /pnfs/.../AnalyzerOutput_validation/_plot_workdir/structure_info.master.yml \
    --exclude '^PerJet/'
python3 scenario_runner.py \
    --base /pnfs/.../AnalyzerOutput_validation \
    --plotter ./stack_plotter.C
# (master 가 이미 있으면 추출 단계 skip)

# Dry-run (yml 만 만들고 plotter 안 돌림):
python3 scenario_runner.py \
    --base /pnfs/.../AnalyzerOutput_validation \
    --plotter ./stack_plotter.C \
    --dry-run
```

### 3.6 plot 자체 특징

| 항목 | 동작 |
|---|---|
| Legend | `label   yield` 형식. QCD HT slice 7개 → 한 줄 (yield 합산) + `Total MC` 라인 |
| stack 색상 | smart color (sample name 기반): QCD red gradient, ttbar 청-teal-green, signal/rare 별도 |
| ratio panel | 0.0–2.0 (cutflow 는 0.0–2.5) |
| log Y | 강제 |
| lumi 라벨 | `41.5 fb⁻¹ (13 TeV)` (2017 hardcoded) |
| 파일명 | path 의 `/` 와 ` ` 모두 `_` 로 sanitize → `Tree_cutflow_w.png` |

---

## Phase 4 — 결과 해석

### 4.1 sanity check (가장 먼저)

`baseline` 의 `Tree_cutflow_w.png` 와 `cutStep_8_*.png` 들이 나오는지 확인.

### 4.2 핵심 비교 — Step 8 ratio 가 어디서 회복되는가

| 비교 | 의미 |
|---|---|
| `bcut0` vs `bcut3` vs `baseline` (=4) | b-jet cut 하나만 변동시켰을 때 ratio 변화 추세 |
| `noBtagShape` vs `baseline` | b-tag shape SF 가 ≥4 b-tag 영역에서 over/under-correct 인지 |
| `noBtagNorm` vs `baseline` | b-tag norm reweight 의 ≥4 b-tag extrapolation closure |
| `noTrigSF` vs `baseline` | trigger SF 의 ≥4 b-tag mis-calibration |
| `noSF_all` vs `baseline` | SF 전부 끈 자연스러운 mismatch (lower bound) |
| `ttHVR` vs `baseline` | ttH VR-style 광역 selection 에서 ratio 회복 정도 |

이상적 결과:
- b-jet cut 을 풀면 (`bcut0`/`bcut1`) ratio 가 1.0 에 가까워지면 → b-tag 영역 SF closure 문제 확정.
- 단일 SF 끄기로는 부분 회복만 보이면 → 여러 SF 가 부분적으로 책임.
- `ttHVR` 에서 회복 → ttH AN-style VR 정의를 분석 baseline 으로 채택 검토.

### 4.3 Higgs / W reco 검증

`cutStep_8_higgs_can01.png` 에서 125 GeV peak 이 보이는지:
- 보임 → reco 알고리즘 정상, ratio 무너짐은 별도 이슈
- 안 보임 → reco 알고리즘 자체 문제 (chi² minimization, b-jet pairing)

`bcut2` / `bcut3` 같은 looser b-jet cut 에서 Higgs reco 가 어떻게 변하는지도
같이 보면 reco 가 b-jet 후보 부족 때문인지 algorithmic 인지 분리 가능.

### 4.4 다음 라운드

진단 결과를 기반으로:
- SF 재유도 (b-tag norm reweight binning 재조정)
- Higgs reco algorithm 점검
- ttbar stitching 정책 확정
- 결과를 `docs/DECISIONS.md` ADR-004 (Step 8 closure) 에 기록

---

## scenario 추가/수정

YAML 의 `validation_scenarios:` 블록에 항목을 추가하면 자동 dispatch 됩니다.

### 새 scenario 추가 예시

```yaml
- name: bcut3_noTrig
  nbJetsCut: 3
  applyHadWWindow: true
  applyHiggsWindow: false
  tightenJet8: false
  forceHiggsRecoMinBjets: 4
  applyBtagShapeSF: true
  applyBtagNormSF: true
  applyTriggerSF: false        # 변경점
  applyTopPtSF: false
  ttHVRStyle: false
```

**sample 추가/제거**: `samples:` 블록에 추가/제거. 단 `Config::SampleRegistry`
(b-tag SF tool / trigger SF tool) 와 sync 필요 — `docs/ARCHITECTURE.md` §10
참조.

---

## 정책 변경 — 어느 파일을 고쳐야 하나

| 변경 | 고칠 파일 | 다시 해야 할 것 |
|---|---|---|
| 새 hist 를 plot 추가 | analyzer (hist allocation), `extract_structure.py` 는 자동 catch | analyzer 재빌드 + condor 재실행 + plotting |
| plot 에서 특정 hist 제외 | `extract_structure.py` 에 `--exclude` flag 또는 코드 안 ALWAYS_SKIP | plotting 만 재실행 |
| ratio panel y range | `stack_plotter.C::pad2` 블록 | plotting 만 재실행 |
| legend yield 포맷 | `stack_plotter.C::FormatYield` | plotting 만 재실행 |
| 새 scenario 추가 | YAML | condor + plotting 재실행 |
| cut 상수 변경 (Higgs window 값 등) | analyzer (`cMHWindowLo` 등) | analyzer 재빌드 + 전체 재실행 |
| process group 매핑 변경 | `Config_TtCatGroup.hh::MakeProcessKey` | b-tag SF JSON 재유도 + analyzer 재빌드 |
| stitching 정책 변경 | `Config_TtCatGroup.hh::StitchingWeight` + 호출부 | analyzer 재빌드 + 전체 재실행 |
| SF 적용 순서 변경 | `ttHHanalyzer_unified.cc::selectObjects` | analyzer 재빌드 + 전체 재실행 |

---

## troubleshooting

### `[error] uproot not installed`
```bash
pip install --user uproot
```

### `[error] file not found: --input`
default 경로가 사용자 mac 기준 (`/Users/jhlee/...`). 명시적 `--input` 전달:
```bash
python3 extract_structure.py --input /pnfs/.../baseline/TTToHadronic/TTToHadronic_0.root
```

### `--val-*` 가 무시되는 듯
analyzer dry-run 시 console 의 `[Validation] applying scenario:` 라인에서
실제 적용된 값 확인. 다른 값이 나오면:
- `tnm.h::commandLine` 에 멤버 추가 됐는지
- `tnm.cc::decode` 에 flag parsing 추가 됐는지
- `int main()` 에서 `commandLine cl` 의 `val*` 멤버를 `ValidationConfig` 로
  복사하는 코드가 있는지 확인.

### ROOT 의 `cutflow_w` 가 안 보임
```bash
root -l -b -q -e 'TFile f("output.root"); f.ls();'
```
출력에서 `KEY: TDirectoryFile	Tree;1` 안에 들어있는지 확인:
```bash
root -l -b -q -e 'TFile f("output.root"); ((TDirectory*)f.Get("Tree"))->ls();'
```
`Tree/cutflow` 와 `Tree/cutflow_w` 보여야 함.

### Plot 이 비어 있음 (Empty 메시지)
- 해당 hist 가 정말 그 sample 에 비어있을 수 있음 (e.g. Data 의 MC-only hist)
- Sample 별 ROOT 파일 1개를 직접 열어 hist contents 확인:
```bash
root -l -b -q -e '
  TFile f("/pnfs/.../baseline/TTToHadronic/TTToHadronic_0.root");
  TH1F* h = (TH1F*)f.Get("CutflowKinematics/cutStep_8_ht");
  if (h) cout << h->GetEntries() << " entries" << endl;
'
```

### Legend 가 너무 길어 plot 영역 침범
`stack_plotter.C::TLegend` 의 좌표 (`0.42, 0.55, 0.93, 0.89`) 를 조정.
Top 늘리려면 4번째 인자를 0.92 등으로.

### scenario_runner 가 ROOT 호출 시 fail
`<workdir>/<scen>/plotter.log` 마지막 25 줄에 ROOT 에러 출력. 가장 흔한
원인:
- structure_info.yml 의 hist path 가 실제 ROOT 파일 안에 없음 (sample 별 구조
  차이) → log 의 `[Skip] No MC` 메시지 다수
- 부분 corruption 된 ROOT 파일 (condor job 중도 실패) → 그 파일만 단독 열어
  `f.IsZombie()` 확인

### condor job 일부 fail
- `${path_output_base}/<scen>/<sample>/` 에 split file 수가 input filelist 와
  맞는지 확인. 부족하면 부족한 split 만 재제출.
- log: `condor/filelistTier3_unified_validation/log_<scen>_<sample>.*.log`

---

## 다음 단계

이 pipeline 으로 12 scenario 결과를 모두 받은 뒤:

1. Step 8 ratio 무너짐의 책임 SF (또는 reco 알고리즘) 확정
2. `docs/DECISIONS.md` ADR-004 (Step 8 closure investigation) 기록
3. SF 재유도 또는 reco 보강 또는 cut 재정의
4. (별도 phase 에서) 본 분석 main mode 재실행

---

## 변경 이력

- v1: validation phase 첫 도입. analyzer 4 mode 화, `Config_TtCatGroup.hh` D2
  strict + Stitching API. 6 patch 적용.
- v2: stack_plotter 에 yield 표시 + cutflow 자동 그림.
- v3: plotter 의 hist filter 정책을 `extract_structure.py` 로 이관, yml
  flat 형식으로 통일.

---

문제 발생 시 condor log + dry-run output 같이 첨부해서 알려 주세요.
