# tools/runlog — 실행 기록과 KNU condor

> **목적:** KNU 에서 오래 걸리는 단계(빌드, `/pnfs` 파일 훑기, 시험)를 condor job 으로 돌리고, 무엇이 어느 커밋에서 어떻게
> 끝났는지를 이 저장소의 `runlogs/` 에 남긴다. 사람이 출력을 복사해 붙이는 대신 로그를 커밋한다.
> **상태:** 2026-10-04 작성, 같은 날 (2) KNU 첫 실행 뒤 고침 (docs/changes/STEP_23_runlog_condor.md). 커밋 규칙은 docs/DECISIONS.md D-2026-10-04-A.

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
| `test_runlog.sh` | 위 셋의 오프라인 시험(가짜 condor_submit·condor_q·scram; 85 check). `RESULT: 85 PASS, 0 FAIL`. 가짜 대신 진짜 condor 명령이 잡히는 환경(noexec `TMPDIR` 등)이면 condor 시험 전에 `ABORT` 하고 멈춘다 |

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
- `condor_rm`·hold·eviction 으로 멈추면 기록 꼬리에 `stopped : by SIGTERM`, EXIT 143(Ctrl-C 는 SIGINT 130, ssh 끊김은 SIGHUP 129).
  명령이 시작되기 전이었으면 `… before the command started` 이고 명령은 돌지 않는다. SIGTERM 은 명령과 그 자식 전부에 가고, 기록은
  남은 출력까지 쓴 뒤 닫힌다; `RUNLOG_KILL_AFTER` 초(기본 5) 뒤에도 남은 것은 SIGKILL. `nohup` 아래에서는 SIGHUP 을 잡을 수 없다.
- job 안에서는 ssh 키도 grid 암호도 없다. `git pull`·`git clone`·`voms-proxy-init` 은 제출 전에 직접 한다.
- **stall guard(2026-10-09, STEP 26 M; `--stall-guard on|off`, 기본 off).** `on` 이면 analyzer job(`submit_job_FH_Tier3_unified.py`, STEP 25 K2)과
  같은 다섯 줄: 지금 run 이 1 시간을 넘고 CPU(user + system)가 그 시간의 5 % 미만이면 hold(이유 `tthh stall guard: running <분> min with <초> s CPU
  on <slot@machine>`), 5 분 뒤 다른 machine 에서 **처음부터 다시**, 모두 3 번 시작까지(그 뒤에는 held 로 남음). 그래서 CPU 를 꾸준히 쓰고 처음부터
  다시 해도 되는 명령에만 준다: plot(`make_plots.py` 는 새 디렉터리), TriggerStudy `run_analysis.sh`, smoke·합성 시험, 빌드(`build_check.sh` 는
  make clean 부터). /pnfs 훑기(파일 목록·branch signature·lumi 확인)는 건강해도 몇 시간 CPU 5 % 미만일 수 있어 주지 않고,
  `tools/stage1/y1_reference.sh` 는 있던 출력을 거절하므로 다시 시작하면 실패한다. 시작마다 기록이 하나씩(멈춘 것은 EXIT 143). 식은
  `condor_run.sh` 안에 글자로 있고, `test/test_failure_checks.py` I 가 제출기의 `stall_guard_exprs()` 와 같은지 본다.

## 상태 읽기

- 큐에 있으면 condor_q 의 상태(idle, running, held …). `held` 면 HoldReason 이 같이 나온다(메모리 초과면 `--memory` 를 올려 다시;
  `tthh stall guard: ...` 면 5 분 뒤 저절로 다시 시작 — held·idle 인 동안은 멈춘 시작의 exit code 143, 다시 돌기 시작하면 `-`, 끝나면 새 시작의 것).
- `done`: 큐에 없고 exit code 가 있다. `gone(no exit)`: 기록은 시작됐는데 exit code 가 없다(강제 종료). `gone`: 기록조차 없다 —
  job 이 `runlog.sh` 전에 죽었다(worker 에 공유 파일 시스템이 없었다 등): `condor/runlog/<dir>/job.out`·`job.err` 를 본다.
- `unknown(condor_q failed)`: schedd 가 바빠 condor_q 가 실패했다 — 잠시 뒤 다시.

## 기록 올리기

```bash
git pull --ff-only && git add runlogs && git commit -m "runlogs: <무엇>" && git push
```

`runlogs/nocommit/`(crab 이 들어간 명령의 기록)과 `runlogs/*.lock` 은 gitignore 다.
