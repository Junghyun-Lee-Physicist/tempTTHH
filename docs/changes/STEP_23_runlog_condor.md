# STEP 23 — 실행 기록(runlog)과 KNU condor: 오래 걸리는 단계는 job 으로, 출력은 커밋으로

- 날짜: 2026-10-04 (코드: 도구만. analyzer 의 물리 로직 변경 없음)
- 신규: `tools/runlog/{runlog.sh, condor_run.sh, status.sh, test_runlog.sh, README.md}`,
  `tools/stage0/{runs_in_lumiblocks.py, branch_signature.py, build_check.sh, treestream_v15check.sh, test_stage0.py}`,
  `test/run_unit_tests.sh`, `runlogs/README.md`
- 수정: `.gitignore`(`runlogs/nocommit/`, `runlogs/*.lock`, `runlogs/.runlog_mark_*`), `README.md` §3(header 를 바꾼 뒤 clean 빌드)·§7.6(새 절)
- 결정: [`../DECISIONS.md`](../DECISIONS.md) D-2026-10-04-A (우리 프로그램의 실행 기록은 커밋, CRAB transcript 만 제외)
- 배경: PLAN §9 Stage 0 의 KNU 단계(빌드, `/pnfs` 파일 훑기 10~30 분, treestream 시험)를 대화형 셸에서 돌리고 출력을 복사해
  AI 세션에 붙였다. 10-03 KNU 의 clean 빌드가 오래 걸리자 사용자가 물었다(10-04): "이런 작업들 condor로 대체하고 로그를 남겨서
  너한테 알려주게 하는 방법 없냐?" — 그리고 "로컬 run 로그나 condor job 로그는 올려도 될텐데 (어차피 우리가 만든 cout이라면 말이야)".

## DECIDED

### 1. `runlog.sh` — NtupleForge 의 형식 그대로, 이 저장소에

NtupleForge `script/runlog.sh`(2026-09-16, lxplus 의 모든 단계에 씀)를 옮겼다. 명령 하나를 돌리고
`runlogs/run_<step>_<UTC>.log`(머리: 시각·host·cwd·git HEAD 와 수정된 추적 파일 수·명령·CMSSW·ROOT·python·condor job id,
본문: stdout+stderr, 꼬리: 시간·**EXIT**·그동안 바뀐 저장소 파일)와 `runlogs/LEDGER.tsv` 한 줄을 남긴다. exit code 는 명령의 것.
NtupleForge 판과 다른 점:
- cwd 가 다른 git checkout(예: treestream)이면 그 HEAD 도 머리에 적는다.
- 명령을 뒤에서 돌리고 SIGTERM·SIGINT 를 잡는다: `condor_rm`, hold, eviction, Ctrl-C 로 멈춰도 꼬리(`stopped : by SIGTERM`,
  EXIT 143/130)와 LEDGER 줄이 남는다(검토 4).
- 시작 표시 파일을 `/tmp` 가 아니라 `runlogs/` 에 둔다(condor job 이 물려받은 `TMPDIR` 가 worker 에 없을 수 있다). 로그 파일은
  noclobber 로 만든다(같은 초에 같은 step 이 둘이어도 섞이지 않음).
- `runlogs/` 에 쓸 수 없으면 명령은 그대로 돌고 기록은 stdout 에만.
- LEDGER 줄은 `flock` 으로 쓴다(NFS 에서 두 job 이 같이 끝나도 줄이 섞이지 않게).
- 커밋하지 않는 기록: 명령에 crab 이 있으면 처음부터, 출력에 pre-signed URL 서명(`X-Amz-Signature`, `X-Amz-Credential`,
  `AWSAccessKeyId`, `?…&Signature=`)이 있으면 끝난 뒤에 `runlogs/nocommit/` 으로 옮기고 LEDGER 에서 뺀다(검토 8: 안에서 crab 을
  부르는 스크립트도 막는다).
- condor 가 넘긴 `RUNLOG_CMD_DISPLAY`·`RUNLOG_CONDOR_JOBDIR` 는 읽은 뒤 지운다: 명령 안의 `runlog.sh` 가 바깥 job 의 기록을
  덮어쓰지 않게(검토 2).
- 꼬리에 기록 크기(`log_kb`)를 적고 2 MB 가 넘으면 안내를 붙인다: 커밋하는 것은 단계의 기록이지 대량 출력이 아니다
  (NtupleForge `docs/03_DECISIONS.md` D-2026-08-17-no-logs-in-git 의 tier 2 와 같은 뜻; 대량 출력은 `condor/` 아래 파일로).

### 2. `condor_run.sh` — 이 저장소의 KNU condor 관례

제출 파일은 이미 KNU 에서 도는 두 제출기를 따른다(`submit_job_FH_Tier3_unified.py` 의 `write_condor_submission_file`·
`create_executable_script`, `outputMerger/merge_outputs.py` 의 `write_condor`·`run_one_hadd.sh`): `getenv = True`,
`MY.WantOS = "el9"`, `request_memory`, worker 에서 `source /cvmfs/cms.cern.ch/cmsset_default.sh` → `cd $CMSSW_BASE/src` →
`scram runtime -sh`, 공유 파일 시스템의 절대 경로, `/pnfs` 는 worker 가 파일로 직접 읽음. 추가한 것:
- 명령을 `printf %q` 로 `payload.sh` 에 적어 인자를 그대로 넘긴다(공백·`$`·`*`·따옴표·빈 인자; 검토에서 줄바꿈·탭·백틱·
  `$(…)`·한글·공백이 든 경로까지 확인).
- `job.sh` 가 `runlog.sh` 로 `payload.sh` 를 감싸서, 환경이 worker 에서 실패해도 기록에 남는다: exit 90(cmsset), 91(CMSSW
  디렉터리), 92(`scram runtime` — 출력을 받아 상태를 본 뒤 `eval`; `eval $(...)` 는 실패를 삼킨다), 93(작업 디렉터리),
  94(`--source`). `job.sh` 자신의 `cd` 가 실패하면(worker 에 공유 파일 시스템이 없음) 기록 전이라 condor 의 `job.out` 에만 남는다.
- `TMPDIR` 를 job 의 scratch 로(`getenv` 가 cms01 의 `TMPDIR` 를 가져오면 worker 의 g++·ROOT 가 임시 파일을 못 만들 수 있다).
- `--source F`: cmsenv 뒤 작업 디렉터리에서 F 를 source(analyzer 자체를 돌릴 때 `setup.sh`).
- proxy 는 `proxy.cert` 가 있고 1 시간 넘게 남았을 때만 붙인다(이 job 들은 grid 접근이 없다; 만료된 proxy 를 붙이면 제출이 거절된다).
- 메모리 기본 4GB(검사·시험). `-j4` 빌드는 `--cpus 4 --memory 12GB`(이 저장소의 analyzer job 이 12 GB 를 요청한다; 검토 10).
- 4000 자가 넘는 명령은 기록 머리에 줄여 적는다(환경 변수 크기 제한; 전체는 `payload.sh` 에; 검토 5).
- `condor_submit` 이 0 으로 끝났는데 cluster 번호를 못 읽으면 실패로 보지 않고 `cluster.txt = ?` 와 경고(중복 제출 방지; 검토 6).
- job 디렉터리는 `condor/runlog/<step>_<UTC>/`(이미 gitignore 인 `condor/`). condor 의 `job.log` 에는 schedd·worker 의 IP 주소와
  포트가 있어 커밋하지 않는다.
- `status.sh`: job 마다 cluster·상태·exit code·기록 파일. 상태는 큐에 있으면 condor_q 의 것(held 면 HoldReason), 큐에 없으면
  done(exit code 있음)·gone(no exit)(기록은 시작됐으나 exit code 없음)·gone(기록도 없음); condor_q 가 실패하면 unknown.
  제출 시각 순(검토 1: 처음엔 step 이름 순이라 최근 job 이 빠졌다).

### 3. Stage 0 의 KNU 단계를 스크립트로 (워크스페이스 RUNBOOK §20 의 heredoc 대신)

| 스크립트 | 무엇 | heredoc 대비 |
|---|---|---|
| `tools/stage0/runs_in_lumiblocks.py` | D14: 요청한 run 의 `LuminosityBlocks` LS 수(skim 전)와 `Events` 수(skim 뒤), 파일 수 | 새 PyROOT 는 못 여는 파일에서 `TFile.Open` 이 OSError 를 던진다(ROOT 6.40 에서 재현; ROOT 6.30.09 의 lxplus 에서도 같았다 — NtupleForge 원장 V54). 그래서 heredoc 은 **첫 깨진 파일에서 죽는다**(KNU 에서 돌기 전에 발견). 이제 못 여는 파일, 잘린 파일(ROOT 가 key 를 복구한 파일 — stage-out 실패 등; 검토 3), 읽기 오류가 나는 파일은 `UNREADABLE` 로 세고 그 파일의 숫자는 쓰지 않으며 exit 3. `Events` 는 run 마다가 아니라 히스토그램 하나로 한 번 읽는다. 한계: `Events` 중간 basket 의 손상을 ROOT 가 stderr 에만 알리면 여기서는 모른다 |
| `tools/stage0/branch_signature.py` | D16: dataset 마다 branch 집합 수와 일부 파일에만 있는 branch | 같은 OSError·잘린 파일 처리, 묶음 이름은 `--base` 아래의 경로 전체(`<PD>/<request>`; 다른 PD·다른 campaign 의 같은 request 이름이 섞이지 않게; 검토 9), 끝에 `SUMMARY` |
| `tools/stage0/build_check.sh` | `make clean && make -j N` 과 짧은 보고(rc, warning/error 줄 수, 첫 error, 산출물 시각, md5). `--from-log` 는 이미 돈 빌드의 보고만 | 전체 make 출력은 `condor/build/` 에(생성된 `eventBuffer.h` 의 warning 수천 줄) |
| `tools/stage0/treestream_v15check.sh` | 패치한 treestream fork: 사전 새로 만들어 `make lib` → `run_tests.sh` → `run_realfiles.sh`(TTto4Q, JetMET0 C, JetMET0 I) | ROOT `Warning in <...>` 줄은 개수만. git pull/clone 은 하지 않는다(ssh 키) |
| `test/run_unit_tests.sh` | 단위 시험 셋 + `test_SampleRegistry` 의 입력 두 파일(md5, git 상태) | — |

### 4. 빌드는 clean 으로

최상위 `Makefile` 의 목적 파일 규칙은 `.cc` 에만 의존한다(`$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc`, header 의존성 없음). header 를 바꾼 뒤
그냥 `make` 는 아무 말 없이 옛 header 로 만든 목적 파일을 쓴다. Stage 1 에서 `eventBuffer.h` 를 다시 만들 때 특히 위험하다
(PLAN §9.3). 그래서 `build_check.sh` 는 언제나 `make clean` 부터 한다; `README.md` §3 에도 적었다. Makefile 자체(의존성 `-MMD`)는
바꾸지 않았다 — 별도 결정.

## 검증 (AI 세션, 2026-10-04)

- `tools/runlog/test_runlog.sh`: **71/71 PASS** (root 로, 그리고 일반 사용자 `nobody` 로 — 후자에서만 도는 T12 포함). 가짜
  `condor_submit`·`condor_q`·`voms-proxy-info`·`scram` 과 가짜 cmsset 으로, 만든 `job.sh` 를 로컬 bash 를 worker 삼아 돌림:
  기록의 머리·본문·꼬리·LEDGER, exit code 전달(7), crab → nocommit, 바뀐 파일 목록에서 `tmp/`·`condor/` 제외, 잘못된 사용 여섯
  (exit 2), dry-run 제출 파일(관례 다섯 줄), 인자 그대로, cwd·`TMPDIR`·cmsenv·condor id, 환경 실패 exit 90·92·94 가 기록에 남음,
  proxy 규칙 넷, `status.sh`(done, held+HoldReason, not-submitted, gone, 시각 순, condor_q 실패 → unknown), 병렬 12 개의 LEDGER 줄
  12 개(필드 8), SIGTERM → 꼬리와 LEDGER 에 143, 안쪽 `runlog.sh` 가 바깥 job 기록을 덮지 않음, 출력의 pre-signed URL → nocommit,
  cluster 번호 없는 제출 → 성공으로, 600 개 인자(긴 명령)가 그대로 도착하고 머리는 줄임, `--source`, 2 MB 넘는 기록의 안내.
- `tools/stage0/test_stage0.py`: **19/19 PASS** — CRAB 모양의 가짜 NanoAOD 파일(알려진 run·LS·event·branch, `failed/` 사본, 깨진
  파일, 잘린 파일, 읽기 오류 파일, 두 campaign)로 D14 의 LS 수(두 파일·두 PD 에 걸친 같은 LS 는 한 번), event 수, 파일 수, run
  간격이 넓을 때의 run 별 경로, exit 2·3, 잘린 파일·읽기 오류 파일의 숫자를 쓰지 않음; D16 의 묶음(PD 별, campaign 별), branch
  집합 수, `HLT_C in 1 of 2`, `failed/` 제외, 깨진·잘린 파일 exit 3.
- `test/run_unit_tests.sh`: 실제 시험 셋 PASS(`PASS 47 / FAIL 0`). `prescan_summary.json` 의 sumw 를 1 % 바꾸면
  `test_SampleRegistry` 가 `PASS 45 / FAIL 2`(TTbar_Hadronic weight) — **KNU 의 로컬 수정 prescan summary 는 이 시험을 바꿀 수 있다**.
- `tools/stage0/treestream_v15check.sh`: 패치한 fork(`8be42e8` 와 같은 트리)와 지금의 NtupleForge slim 목록으로 `SUMMARY ... -> PASS`
  (T1 `counter checks: ok 270`, T3 `silent_zero 150`, T6 `481`, 합성 파일 셋 `RESULT PASS`), 2 분 18 초. 옛 slim 목록(09-27 이전)이면
  T1·T6 의 숫자가 다르다(360, 634) — 기대값은 NtupleForge 판에 묶여 있다.
- `build_check.sh`: 가짜 Makefile 로 성공(rc 0, warning 2), 실패(rc 2, 첫 error 와 문맥), `--from-log`.

## 독립 검토 (2026-10-04, 서브에이전트 한 번)

시험을 다시 돌리고(당시 52/52, 15/15, 47/0) 실험으로 14 개를 보고했다. 고친 것: 1 `status.sh` 가 step 이름 순으로 잘라 최근 job 이
빠짐 → 시각 순; 2 안쪽 `runlog.sh` 가 바깥 job 의 기록 경로·exit code 를 덮음 → 환경 변수를 읽고 지움; 3 D14 가 읽기 오류에서 죽고,
잘린 파일을 0 오류로 셈(재현에서 `Error in <TBranch::GetBasket>` 10 만 줄) → 잘린 파일 제외, 파일 단위로 읽기 오류 처리;
4 SIGTERM 에 꼬리가 없음 → trap; 5 긴 명령이 환경 변수 한도를 넘음 → 머리는 줄임; 6 cluster 번호를 못 읽으면 "FAILED" 로 잘못 알림;
7 condor_q 실패가 gone 으로 보임; 8 crab 을 안에서 부르는 스크립트 → 출력 검사; 9 묶음 이름이 campaign 을 섞음; 10 메모리 기본값
안내; 11 `--help` 범위; 12 시험이 저장소의 `.gitignore` 를 쓰지 않음; 13 문서와 코드의 어긋남(running 의 뜻, exit 93, `job.log` 를
커밋하지 않는 이유); 14 작은 것(hostname 없을 때, 빈 예외 메시지, noclobber, `git status` 의 lock). 검토가 "괜찮다" 고 확인한 것:
인자 전달의 모든 따옴표·특수 문자 경우, exit code 전달(7·90·92), 실제 `.gitignore`, `TMPDIR` 가 없는 worker, `flock` 이 없을 때,
EL9 의 GNU 옵션·bash 5.1, Python 3.9 문법, PyROOT 의 두 open 실패 경우, 파일 간 LS 중복 제거.

## 확인하지 못한 것 (KNU 에서 처음 확인)

- worker 가 `/u/user` 에 쓸 수 있는지: 기존 analyzer job 의 `.out`·`.err` 가 `tempTTHH/condor/` 아래(`/u/user`)에 써지므로 될 것으로
  본다(추론). 안 되면 `status.sh` 가 `gone` 이고 `condor/runlog/<dir>/job.out` 에 이유가 있다.
- KNU 가 메모리를 어떻게 강제하는지(cgroup 이면 넘을 때 hold, 기록에는 143).
- `request_cpus` 는 기존 KNU 제출 파일에 없던 줄이다(`--cpus` 를 줄 때만 쓴다).
- 첫 실제 job 의 결과로 이 절을 갱신한다.
