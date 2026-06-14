# STEP 2 — 모드 축소 (trigsf/validation 제거) + kDebug 모드 신설

- 날짜: 2026-06-11
- 요구사항: 7 (main/btagtrig/prescan만 유지) + 신규 지시 (debug 모드 추가)
- 변경 파일: `ttHHanalyzer_unified.h`, `ttHHanalyzer_unified.cc`,
  `submit_job_FH_Tier3_unified.py`, `AnalyzerConfig/*.yml` (주석 1줄씩)
- 신규 파일: `include/DebugLogger.h`
- 백업: `docs/backup_20260611/` (analyzer는 Step 0, submitter/yml은 본 스텝에서 백업)

## 1. 무엇을 바꿨나

### 1.1 모드 제거 — `kTriggerSFStudy`, `kValidationStudy`

**`ttHHanalyzer_unified.h`** (−124줄 / +38줄):
- `AnalysisMode` enum에서 두 멤버 제거, `kDebug` 추가
- `parseAnalysisMode`: "trigsf"/"validation" 입력 시 **명시적 [FATAL] throw**
  (제거 사유와 대체 경로 — trigsf → standalone TriggerStudy — 를 메시지에 포함;
  조용한 오동작 방지). 유효 모드 목록 "main, btagtrig, prescan, debug"로 갱신.
- `analysisModeName`: 두 case 제거, `kDebug → "Debug"` 추가
- `SelectionPolicy`: `requireSingleMuon` 필드 제거 (trigsf 전용이었음) —
  8필드 → **7필드**. `fromMode`의 trigsf/validation case 제거,
  `kDebug`는 `kMainAnalysis`와 동일 policy로 fallthrough.
  `print()`의 "Require 1 Muon" 줄 제거.
- **`ValidationConfig` struct 통째 제거** (83줄): scenario별 cut/SF 토글 번들.
  원형은 백업 참조. 제거 위치에 안내 주석 남김.
- `setValidationConfig()` 메서드, `_valCfg` 멤버 제거.

**`ttHHanalyzer_unified.cc`** (−105줄 / +46줄):
- `selectObjects`:
  - Higgs reco 최소 b-jet 수: `_valCfg.forceHiggsRecoMinBjets` 분기 제거 → 상수 4
  - Lepton veto: `requireSingleMuon`(1μ+0e, trigsf 전용) 분기 제거 →
    `applyLeptonVeto`만 (ttH AN Table 55 FH와 동일)
  - SF 적용: `isVal ? _valCfg.applyXxxSF : true` 토글 3종 제거 →
    **무조건 적용** (`_evtWeight *= bTagWeight_central_` 등).
    물리 동작은 main 모드 기준 **불변** (토글은 validation 전용이었고
    main에선 항상 true였음).
  - nbJets: validation 단일-컷 분기 제거 → production 3단(≥2 cut, ≥3/≥4 기록)만
  - HadW window: `_valCfg.applyHadWWindow` 삼항 제거 → `_policy.applyHadWMassCut`
- `main()`:
  - `--val-*` → `ValidationConfig` push 블록(36줄) 제거
  - stitch JSON 게이팅: `kDebug` 추가 (debug = main 미러이므로 stitch 로드)
  - `debugVerbose = (mode == kDebug)` 연결 (기존 거시 단계 로그 활성화)
- **tnm.cc/tnm.h의 `--val-*` 파서 필드는 이번에 건드리지 않음** (미사용 잔재,
  컴파일 무해) — Step 8 정리 단계에서 제거 예정. main() 주석에 명시.

### 1.2 submitter — validation 지원 제거 (−105줄 / +30줄)
`submit_job_FH_Tier3_unified.py`:
- `process_config_file`: scenario 외부 루프 제거, 단일 패스만.
  **유효 모드 조기 검증 추가**: `analysis_mode ∉ {main, btagtrig, prescan, debug}`
  이면 condor 제출 전에 `[FATAL]` ValueError (잘못된 yml로 job 뿌리는 사고 방지).
- `parse_config_entry(entry, common, scenario)` → `(entry, common)`;
  scenario 합성 output_dir 분기 제거.
- `_scenario_argv()` 함수(--val-* argv 조립) 통째 제거 + 호출부 2곳 제거.
- 자체 yml 파서의 `validation_scenarios:` 섹션 지원 제거.
- `setup_and_submit_job`의 scenario 라벨 출력 제거.

### 1.3 yml — 모드 주석 갱신 (각 1줄)
`AnalyzerConfig/Tier3_2017_FH_unified_{main,prescan,btagtrig}.yml`:
`# main, btagtrig, trigsf, prescan` → `# main, btagtrig, prescan, debug ...`

### 1.4 kDebug 모드 + `DebugLogger` (신규)

**설계**: debug = **main과 완전히 동일한 동작** (selection policy, SF 체인,
stitch 게이팅 모두 main 미러) + 단계별 검증 로그. 로컬에서 테스트 파일 1-2개로
각 로직이 정상 동작하는지 확인하는 용도. 다른 모드에서는 모든 hook이
no-op (bool 체크 1회 — hot loop 성능 영향 없음).

**`include/DebugLogger.h`** (신규, header-only, ROOT 비의존):
- 처음 N개 이벤트(기본 10, `TTHH_DEBUG_NEVENTS`로 조정) 상세 출력
- 전체 이벤트에 대해 stage/key별 집계: N, mean, min, max, **NaN/Inf 수**
- cutflow를 히스토그램과 **독립적으로** stdout에서 재구성 (hCutFlow 교차 검증)
- 종료 시 summary 표 출력, 비정상값 발견 시 WARNING

**analyzer hook 지점 (7곳)**:
| hook | 위치 | 검증 대상 |
|---|---|---|
| `nextEvent()` | process() 시작 | 이벤트 경계 |
| `kv("weight",...)` ×5 | weight 조립 직후 | genW/PU/L1/base/evtWeight 정상범위·NaN |
| `kv("stitch",...)` ×2 | stitch 적용 직후 | expandedTtbarId·multiplier |
| `kv("sf",...)` ×4 | SF 곱 직후 | btagShape/trigSF/btagNorm/최종 |
| `cut(...)` | processStep 람다 | cutflow (hCutFlow와 대조) |
| `kv("tree",...)` ×3 | `_inputTree->Fill()` 직전 | branch 기록값 |
| `summary()` | loop() 끝 | 전체 요약 |

이후 워크플로우 스텝마다 해당 스텝의 검증 hook을 이 로거에 추가한다
(stage 이름: "yml-path", "evtshape", "higgsreco" 등 — DebugLogger.h 상단 규약 참조).

## 2. 왜 바꿨나
- trigsf 모드의 역할(trigger SF 유도)은 standalone `TriggerStudy` 패키지가
  btagtrig skim에서 수행하는 구조로 대체됐다 (분리된 책임).
- validation 모드(시나리오별 cut/SF 토글)는 사용이 끝났고, 토글 분기들이
  main 경로의 SF 적용부 가독성을 해치고 있었다.
- debug 모드: 리팩토링이 진행되는 동안 각 스텝의 동작을 로컬에서 즉시
  검증할 수단 필요 (condor 없이 파일 1-2개로).

## 3. 원래 형태
백업: `docs/backup_20260611/{ttHHanalyzer_unified.cc,.h,submit_job_FH_Tier3_unified.py,AnalyzerConfig/*.yml}`.
주요 원형 요지:
- enum: `kMainAnalysis, kBTagAndTriggerStudy, kTriggerSFStudy, kValidationStudy, kPrescan`
- policy 8필드: `{Trig, LepV, bJet, HadW, CollLep, leadMu, HiggsReco, Req1Mu}`,
  trigsf = `{false,false,true,false,true,true,false,true}`,
  validation = main과 동일
- lepton veto에 `requireSingleMuon`(1μ+0e) 우선 분기
- SF 적용부: `if (useBtagShape) _evtWeight *= ...` 등 3개 토글
- nbJets: validation 단일-컷 + production 3단의 if/else
- main(): `--val-*` 11개 필드를 `ValidationConfig`로 push
- submitter: validation 모드 시 `validation_scenarios:` 외부 루프 +
  `<scenario>/<sample>` output 합성 + `--val-*` argv 조립

## 4. 롤백 방법
```bash
cp docs/backup_20260611/ttHHanalyzer_unified.{cc,h} .
cp docs/backup_20260611/submit_job_FH_Tier3_unified.py .
cp docs/backup_20260611/AnalyzerConfig/*.yml AnalyzerConfig/
rm include/DebugLogger.h   # (신규 파일)
```
단, 롤백 시 Step 1(stitchWeight branch)도 함께 사라짐에 유의 —
Step 1만 유지하려면 STEP_1 문서의 4편집을 백업본에 재적용.

## 5. 검증 (전부 통과)

**자동 (sandbox에서 실행됨)**:
1. 제거 심볼 잔존 grep: `kTriggerSFStudy`/`kValidationStudy`/`ValidationConfig`/
   `_valCfg`/`requireSingleMuon`/`isVal`/`useBtagShape`/`useTrigSF`/`useBtagNorm`/
   `cl.val` — **라이브 코드 0건** (남은 5건은 전부 [STEP2] 설명 주석)
2. brace balance 원본과 동일 (cc=1, h=1)
3. **모드 머신 실제 컴파일+실행 테스트** (enum+파서+policy 블록은 self-contained
   라 발췌 g++ 컴파일 가능): 유효 4모드 파싱 ✔, 제거 2모드 throw ✔,
   빈/오타 throw ✔, debug==main policy ✔, btagtrig/prescan policy 기대값 ✔
4. **DebugLogger 실제 컴파일+실행 테스트** (ROOT 비의존): NaN 주입 검출 ✔,
   cutflow 집계 정확 ✔, 비활성 시 no-op ✔, `-Wall -Wextra` 경고 0 ✔
5. **submitter 기능 테스트** (실행): yml 파싱 ✔, validation_scenarios 블록 무시 ✔,
   `analysis_mode: validation` 조기 [FATAL] 거부 ✔, `ast.parse` 구문 ✔
6. kDebug 배선 전수 grep: include/enum/파서/modeName/policy/생성자 enable/
   stitch 게이팅/debugVerbose — 15곳 확인

**사용자 로컬 검증 (권장)**:
```bash
make   # 컴파일 — sandbox엔 ROOT가 없어 여기서 못 함 (최우선 확인 사항)
# 1) 제거 모드 거부 확인 (즉시 [FATAL] 종료해야)
./ttHHanalyzer_unified.exe --filelist <1파일 list> --mode trigsf ... ; echo "exit=$?"
# 2) debug 모드 — 테스트 파일 1개 (TTToHadronic 권장: stitch+expandedTtbarId 경로 모두 작동)
TTHH_DEBUG_NEVENTS=20 ./ttHHanalyzer_unified.exe --filelist <1파일 list> \
    --mode debug --sampleName TTToHadronic --DataOrMC MC ... | tee dbg.log
grep '\[dbg\]' dbg.log | less
# 확인 포인트:
#  - [dbg][weight] evtWeight_preSF 에 BAD(NaN/Inf) 0건
#  - [dbg][stitch] mult ∈ {0, 1} (inclusive ttbar; ttbb/tt4b면 r 값)
#  - [dbg][sel] 종료 요약의 step별 N == hCutFlow bin 내용 (ROOT에서 대조)
#  - [dbg][tree] stitchWeight 분포가 기대와 일치
# 3) main 모드가 Step2 전과 동일한지 (가장 중요한 회귀 검증):
#    같은 입력 1파일로 Step2 전/후 바이너리 실행 → output ROOT의
#    hCutFlow, hCutFlow_w_full 바이너리 동일해야 (validation 토글 제거는
#    main 경로에서 no-op이므로)
```

## 6. 동작 불변 보증 (main/btagtrig/prescan)
제거된 코드는 전부 `_analysisMode == kValidationStudy` 또는
`_policy.requireSingleMuon`(trigsf에서만 true) 조건 하에 있었다.
main/btagtrig/prescan에서는 해당 분기가 원래 실행되지 않았으므로
**세 모드의 물리 동작은 비트 수준에서 불변**이다. (위 로컬 검증 3으로 확인 가능.)
DebugLogger hook은 kDebug가 아니면 bool 체크 후 즉시 return.
