# tools/runlog — 실행 기록과 KNU condor

> **목적:** KNU 에서 오래 걸리는 단계(빌드, `/pnfs` 파일 훑기, 시험)를 condor job 으로 돌리고, 무엇이 어느 커밋에서 어떻게
> 끝났는지를 이 저장소의 `runlogs/` 에 남긴다. 사람이 출력을 복사해 붙이는 대신 로그를 커밋한다.
> **상태:** 2026-10-04 작성 (docs/changes/STEP_23_runlog_condor.md). 커밋 규칙은 docs/DECISIONS.md D-2026-10-04-A.

## 한 번에 보기

```bash
# KNU, cmsenv 한 셸, tempTTHH 에서
/bin/bash tools/runlog/condor_run.sh <step> -- <명령> [인자...]      # condor job 하나
/bin/bash tools/runlog/runlog.sh <step> -- <명령> [인자...]          # 같은 기록을 지금 이 셸에서
/bin/bash tools/runlog/status.sh                                    # job 들의 상태
git add runlogs && git commit -m "runlogs: ..." && git push          # 기록 올리기 (switch 뒤)
```

| 파일 | 하는 일 |
|---|---|
| `runlog.sh` | 명령 하나를 돌리고 `runlogs/run_<step>_<UTC>.log`(머리: 시각·host·cwd·git HEAD 와 수정 파일 수·명령·CMSSW·ROOT·python·condor job, 본문: stdout+stderr, 꼬리: 시간·**EXIT**·바뀐 파일)와 `runlogs/LEDGER.tsv` 한 줄을 남긴다. exit code 는 명령의 것. NtupleForge `script/runlog.sh` 와 같은 형식 |
| `condor_run.sh` | 같은 기록을 condor job 으로. job 은 worker 에서 `$CMSSW_BASE/src` 로 cmsenv 하고, 제출한 디렉터리에서 명령을 그대로(인자 그대로) 돌린다 |
| `status.sh` | `condor_run.sh` 로 낸 job 마다 한 줄(제출 시각 순): cluster, 상태, exit code, 기록 파일. held 면 HoldReason |
| `test_runlog.sh` | 위 셋의 오프라인 시험(가짜 condor_submit·condor_q·scram; 71 check). `RESULT: 71 PASS, 0 FAIL` |

## condor job 의 모양

- 제출 파일은 이 저장소의 KNU 관례를 따른다(`submit_job_FH_Tier3_unified.py`, `outputMerger/merge_outputs.py`):
  `getenv = True`, `MY.WantOS = "el9"`, `request_memory`(기본 4GB), `request_cpus`(`--cpus`), `x509userproxy`.
  `-j4` 빌드는 `--cpus 4 --memory 12GB`. analyzer 자체를 돌릴 때는 `--source setup.sh`(cmsenv 뒤 작업 디렉터리에서).
- proxy 는 `proxy.cert` 가 있고 1 시간 넘게 남았을 때만 붙인다. 이 job 들은 `/pnfs` 를 파일로 읽어서 grid 접근이 필요 없다.
- job 디렉터리 `condor/runlog/<step>_<UTC>/`(gitignore): `payload.sh`(cmsenv → `TMPDIR` = job scratch → `cd` → 명령),
  `job.sh`(`runlog.sh` 로 `payload.sh` 를 감쌈), `job.sub`, condor 의 `job.out`·`job.err`·`job.log`.
  `job.log` 에는 schedd·worker 의 IP 주소와 포트가 있다 — `condor/` 를 커밋하지 않는 이유 하나.
- 환경이 worker 에서 안 되면 그것도 기록에 남는다: exit 90(cmsset), 91(CMSSW 디렉터리), 92(`scram runtime`), 93(작업 디렉터리),
  94(`--source`). `job.sh` 자신이 작업 디렉터리에 못 들어가면(공유 파일 시스템 없음) 기록 전이라 `job.out` 에만 남는다.
- `condor_rm`·hold·eviction 으로 멈추면 기록 꼬리에 `stopped : by SIGTERM`, EXIT 143.
- job 안에서는 ssh 키도 grid 암호도 없다. `git pull`·`git clone`·`voms-proxy-init` 은 제출 전에 직접 한다.

## 상태 읽기

- 큐에 있으면 condor_q 의 상태(idle, running, held …). `held` 면 HoldReason 이 같이 나온다(메모리 초과면 `--memory` 를 올려 다시).
- `done`: 큐에 없고 exit code 가 있다. `gone(no exit)`: 기록은 시작됐는데 exit code 가 없다(강제 종료). `gone`: 기록조차 없다 —
  job 이 `runlog.sh` 전에 죽었다(worker 에 공유 파일 시스템이 없었다 등): `condor/runlog/<dir>/job.out`·`job.err` 를 본다.
- `unknown(condor_q failed)`: schedd 가 바빠 condor_q 가 실패했다 — 잠시 뒤 다시.

## 기록 올리기

```bash
git pull --ff-only && git add runlogs && git commit -m "runlogs: <무엇>" && git push
```

`runlogs/nocommit/`(crab 이 들어간 명령의 기록)과 `runlogs/*.lock` 은 gitignore 다.
