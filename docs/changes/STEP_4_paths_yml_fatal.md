# STEP 4 — 보정 입력 경로의 yml 통제 + FATAL exit 체계 확장 (47/48/49)

- 날짜: 2026-06-11
- 요구사항: 8 (절대경로 → yml 통제), 12 (실패 시 에러코드 출력 후 종료)
- 변경 파일: `src/CorrectionsManager.cc`, `include/CorrectionsManager.h`,
  `ttHHanalyzer_unified.h`, `ttHHanalyzer_unified.cc`,
  `submit_job_FH_Tier3_unified.py`, `AnalyzerConfig/*.yml`
- 백업: `docs/backup_20260611/src/CorrectionsManager.cc`,
  `docs/backup_20260611/include/CorrectionsManager.h` (본 스텝),
  나머지는 기존 백업 참조

## 1. 무엇을 바꿨나

### 1.1 경로 주입 체계 (yml → condor sh의 export → env → 코드)
analyzer가 yml 파서를 갖지 않으므로(파싱은 submitter 책임), 기존
`STITCH_FACTORS_JSON`/`EXPANDED_TTBARID_DIR`과 **같은 env 패턴으로 통일**했다:

| yml `common.` 키 | env | 대상 |
|---|---|---|
| `path_jsonpog` | `TTHH_JSONPOG_PATH` | jsonpog-integration (POG 중앙) |
| `path_goldenjson` | `TTHH_GOLDENJSON_PATH` | GoldenJson 디렉토리 |
| `path_trigsf_dir` | `TTHH_TRIGSF_DIR` | trigger_sf.json.gz 디렉토리 |
| `path_btag_reweight_json` | `TTHH_BTAGRW_JSON` | btagNormReweight.json 파일 |
| `path_stitch_json` | `STITCH_FACTORS_JSON` | stitch JSON (기존 env 재사용) |
| `path_expanded_ttbarid_dir` | `EXPANDED_TTBARID_DIR` | ttnb lookup 디렉토리 (기존) |

- `CorrectionsManager` 생성자: 4개 경로를 `envOr(env, default, what)`로 결정 —
  **env 미설정/빈 값이면 기존 Tier3 하드코딩 값이 default** (하위호환: env 없이
  돌리면 이전과 완전 동일). 어떤 경로를 어디서 가져왔는지(`(env X)` /
  `(default; env X unset)`) 시작 로그에 전부 출력.
- submitter: `common.path_*` 중 비어 있지 않은 것만 condor 실행 sh에
  `export ENV="..."` + `echo '[paths] ...'` 로 주입 (condor 로그에서 확인 가능).
  빈 값/키 없음 → 미주입 → default 사용.
- 로컬 테스트: 같은 env를 쉘에서 직접 export 하면 됨.

### 1.2 FATAL exit 확장 — silent-wrong-output 경로 3개 제거
조사 결과 기존 코드에 **경고만 내고 계속 도는** 경로가 셋 있었다:
1. trigger SF JSON 누락/로드 실패 → WARN, SF=1.0으로 진행
2. b-tag norm reweight JSON 누락/로드 실패 → WARN, 1.0으로 진행
3. GoldenJSON 누락 → ERROR 출력 후 **Data가 인증 필터 없이** 진행

main 모드에서 1·2는 물리적으로 틀린 결과를 조용히 만든다. 새 정책:

| 코드 | 조건 | 모드 |
|---|---|---|
| **47** | trigger SF JSON 누락 또는 로드 실패 | main/debug (MC) |
| **48** | b-tag norm reweight JSON 누락/로드 실패 | main/debug (MC) |
| **49** | 중앙 보정(jsonpog JME/PU/BTV) 로드 throw, 또는 Data의 GoldenJSON 누락 | 전 모드 |

- 생성자에 `requireDerivedCorr` 인자(5번째) 추가. analyzer가
  `(mode==main || mode==debug)`로 전달 — **btagtrig/prescan은 부트스트랩 허용**
  (btagtrig은 그 보정들의 입력 skim을 "만드는" 모드이므로 WARN-continue 유지,
  메시지에 "(bootstrap mode)" 명시).
- 중앙 보정 로드(loadJME_/PU_/BTag_/Golden_)는 try/catch로 감싸 correctionlib
  throw를 `[FATAL]` 메시지 + exit(49)로 변환 (Condor .err에서 식별 가능).
- 기존 체계와 합치면: 40-43 stitch/expandedTtbarId, 45-46 b-tag key,
  **47-49 corrMgr** — 전 fatal이 고유 exit code.

### 1.3 debug hook (Step 4분)
kDebug 시작 시 `[dbg][paths]`로 6개 env의 (값/unset→default) 상태 덤프 —
yml→sh→env 전달이 실제로 됐는지 즉시 확인.

### 1.4 부수 정리
- Step 2 잔재 제거: `submit_job()`에 남아 있던 `_current_scenario` 라벨 참조.

## 2. 원래 형태 / 롤백
백업: `docs/backup_20260611/{src/CorrectionsManager.cc,include/CorrectionsManager.h}`.
원형 요지: 생성자에 Tier3 절대경로 4개 하드코딩(lxplus/Local 변형은 주석),
trigger/btagRW/golden 누락 시 WARN/ERROR 후 계속.
```bash
cp docs/backup_20260611/src/CorrectionsManager.cc src/
cp docs/backup_20260611/include/CorrectionsManager.h include/
# analyzer .h의 corrMgr 5-인자 호출 1곳을 4-인자로 되돌리기 (STEP 문서 §1.2)
```

## 3. 검증 (전부 통과)
1. **envOr 발췌 실제 컴파일+실행**: env 설정/미설정/빈 값 3케이스 — default
   fallback 정확.
2. **submitter 기능 테스트 (실행)**: yml `path_*` → `env_exports` 매핑(빈 값
   제외 확인), 생성된 sh에 export/echo 라인 정확히 포함, 빈 키는 미주입.
3. brace balance: corrMgr .cc/.h = 0/0, analyzer 불변. `std::exit` 6곳 =
   [FATAL] 6곳 일치. `ast.parse` submitter 구문 OK.
4. requireDerived 배선 13곳 grep 확인 (선언/전달/사용).

**사용자 로컬 검증**:
```bash
make
# (1) fatal 47 확인: 존재하지 않는 trigger SF 경로 주입 → 즉시 종료 + exit code
TTHH_TRIGSF_DIR=/nonexistent ./<exe> --mode debug ... ; echo "exit=$?"   # 47이어야
# (2) bootstrap 확인: btagtrig 모드는 같은 조건에서 WARN 후 정상 진행
TTHH_TRIGSF_DIR=/nonexistent ./<exe> --mode btagtrig ... # WARN (bootstrap mode)
# (3) 경로 미설정 시 default 동작 (이전과 동일) — 시작 로그의 [path] 라인 확인
```
