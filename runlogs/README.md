# runlogs — 실행 기록 (커밋함)

> **무엇:** `tools/runlog/runlog.sh` 와 `tools/runlog/condor_run.sh` 가 남긴 기록. 파일 하나 = 실행 하나.
> **규칙:** docs/DECISIONS.md D-2026-10-04-A — 우리 프로그램의 출력이므로 커밋한다. crab 이 들어간 명령의 기록은
> `nocommit/`(gitignore)에만 있고 LEDGER 에도 없다(CRAB transcript 에는 pre-signed S3 URL 이 있다).

- `run_<step>_<UTC 시작>.log`: 머리(시각·host·cwd·git HEAD 와 수정된 추적 파일 수·명령·CMSSW·ROOT·python·condor job),
  본문(stdout+stderr), 꼬리(시간·**EXIT**·그동안 바뀐 저장소 파일).
- `LEDGER.tsv`: 한 줄에 실행 하나 — `utc_start  step  exit  wall_s  host  git_head  log  outputs`.
- 사용법: `tools/runlog/README.md`.
