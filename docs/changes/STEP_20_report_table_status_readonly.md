# STEP 20 — --report 요약 테이블 + --status 상세 모드 + 조회 read-only 화 + lazy tmp 디렉토리

- 날짜: 2026-07-10
- 배경: NtupleForge `crab/submit_crab.py` 의 `--report`(compact per-sample 테이블)
  스타일을 condor submitter 에도 도입. 기존 `--report` 는 샘플별 나열이라
  전체 상황(어디가 얼마나 비었나)을 한눈에 못 봤다.
- 변경: `submit_job_FH_Tier3_unified.py`, `QUICK_COMMANDS.txt` (문서)

## DECIDED

### 1. 조회 모드 2종으로 분리

| 옵션 | 출력 | 용도 |
|---|---|---|
| `--report` | 마지막에 요약 테이블 한 장 (`sample \| jobs \| done \| miss \| done%` + TOTAL, missing 샘플에 `<-- missing` 플래그) | "전체가 잘 돌았나" 한눈 확인 |
| `--status` (신규) | 샘플별 상세 — `[weight]` 라인 + `[report] ... missing=N -> idx [...]` (구 `--report` 동작) + 마지막에 같은 테이블 | "어느 idx 가 비었나" 추적 |

내부적으로는 기존 `report_only` 플래그 하나로 묶고(제출 안 함 분기 전부 재사용)
`report_verbose` 로 출력만 가른다. `--report`+`--status` 동시 사용,
조회+`--resubmit` 동시 사용은 FATAL. 판정 기준은 STEP19 종료 마커 그대로 —
**done = event loop 완주 output**. 큐/실행 중인 job 도 output 이 없으면 miss 로
집계된다(miss = 실패 + 미실행 + 실행중; 실시간 큐 상태는 `condor_q`).

### 2. 조회 모드 read-only 화 (버그 수정 포함)

기존 `--report` 는 조회인데도 디스크를 건드렸다:
- **`arguments_<sample>.txt` truncate 버그**: `generate_argument_list` 가 모드
  불문 `open(..., "w")` 로 열어, report 모드에서는 아무것도 안 쓰고 닫히며
  직전 제출의 인자 기록을 **빈 파일로 만들었다**. 큐잉된 job 은 제출 시점에
  인자를 이미 읽어 실행엔 무해하지만 provenance 가 소실됐다. → 인자 라인을
  메모리에 모았다가 제출/재제출일 때만 파일에 쓴다.
- output 디렉토리 `mkdir -p`/`chmod 755` (`prepare_output_directory`) 와
  "Output directory exists / Set permissions" 출력 → 조회 모드에서 skip.
- 샘플마다 빈 `tmp_*` 디렉토리 생성 → 아래 3번으로 해결.

### 3. tmp 디렉토리 lazy 생성 — "빈 tmp_* 리터" 원인 제거

**원인 규명**: `parse_config_entry` 가 `tmp_<sample>_<invocation timestamp>` 를
모든 호출(submit/resubmit/report)에서 샘플마다 **무조건 mkdir** 했다. per-job
filelist 는 실제 (재)큐잉되는 job 에 대해서만 그 안에 쓰이므로:
- 같은 timestamp 인데 어떤 tmp 는 차 있고(재큐 job 존재) 어떤 tmp 는 빈 것
  (그 샘플은 전부 complete → 재큐 0건) — 사용자가 관찰한 현상 그대로.
- report 호출도 매번 전 샘플의 빈 tmp 세트를 새로 만들었다.

**수정**: eager mkdir 제거, `generate_argument_list` 가 첫 filelist 를 쓰기
직전에만 `os.makedirs(exist_ok=True)`. 이제 tmp 디렉토리의 존재 자체가
"이 invocation 이 이 샘플에 job 을 실제로 큐잉했다"는 의미가 된다 (job 의
.out/.err 도 여기에 쌓이므로 semantics 일관). 기존에 쌓인 빈 디렉토리 청소:

```bash
find condor/filelistTier3_unified_main -maxdepth 1 -type d -name 'tmp_*' -empty -delete
```

(주의: 실행/큐잉 중인 invocation 의 tmp 는 .out/.err 가 아직 없으면 빈
디렉토리일 수 있다 — 큐가 완전히 빈 상태에서 청소할 것.)

### 4. 부수 정리
- compact report 에서 `[weight]`/"Setting up job" 배너 억제 (--status 는 유지).
- `submit_command.txt` 조회 라벨에 status 구분 추가.
- `QUICK_COMMANDS.txt` 에 --report/--status 의미 갱신.

## 비변경 (invariant)
- 완료 판정 로직(STEP19 `_has_end_marker`), 제출/재제출 경로의 filelist·인자
  생성 내용, output 명명(`<sample>_<jobIdx>.root`), prescan 분기 — 모두 불변.
- `--report` 를 쓰던 기존 워크플로: 판정은 동일, 출력 형태만 테이블로 변경.
  구 출력이 필요하면 `--status`.

## 검증
- `ast.parse` 구문 OK. `_print_report_table` 을 실제 report 수치
  (TTHHto4b 32/32, QCD_HT200to300 16/77, WJetsToLNu_HT70To100 7/406,
  HT100To200 11/773)로 단위 테스트 — 정렬/TOTAL/플래그 확인.
- 정적 흐름 검사: 제출·재제출 경로에서 인자 파일 기록 유지, lazy mkdir 존재,
  `[resubmit]` 출력 블록 불변 확인.
- **미검증(환경 제약)**: 실제 Tier3 에서의 end-to-end (ProxyChecker·PyROOT·
  pnfs 필요) — 첫 실행은 `--report` 로 (read-only 라 안전).

## 롤백
- `--status` argparse 항목과 `report_verbose` 분기 제거, `_print_report_table`
  삭제, `generate_argument_list` 를 STEP19 형태(with open(arg_list_file..))로
  원복, `parse_config_entry` 의 eager `make_directory(self.tmp_folder)` 복원.
