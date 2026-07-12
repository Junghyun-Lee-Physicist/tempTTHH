# STEP 22 — merge_outputs.py: AnalyzerOutput 자동 발견 hadd (local 병렬 / condor)

- 날짜: 2026-07-12
- 신규: `outputMerger/merge_outputs.py`
- 문서: `outputMerger/README.md` 에 사용 섹션 추가
- 배경: 구 `merge_submitter.py` (repo 외부 스크립트) 는 process 목록을
  하드코딩했다. 목록이 실제 디렉토리와 어긋나면 **조용히 빠지는데**, 실제로
  구 목록에는 stale 이름(ttHtobb / TTToHadronic / ttbb_2L2Nu 등 — 현재
  디렉토리는 ttHTobb / TTbar_Hadronic / TTbb_DiLep)이 다수 섞여 있어
  그대로 돌리면 상당수 샘플이 merge 에서 누락될 상태였다.

## DECIDED

### 1. 자동 발견 (하드코딩 목록 제거)
`<base>` 하위 디렉토리 `d` 중 `d.name + "_*.root"` 파일이 1개 이상인 것만
merge 대상. 그 외(재제출 분리 폴더, `_plot_workdir` 등)는 "패턴 미매칭"
으로 리포트만 하고 미대상 — 새 샘플 추가/이름 변경 시 스크립트 수정 불필요.
출력은 구 컨벤션 그대로 `<base>/<proc>.root`.

옵션: `--only GLOB...` / `--exclude GLOB...` (부분 merge),
`--skip-existing` (성공분 제외 → 실패분만 재시도), `--list` (표만),
`--dry-run` (condor 는 `.sub` 생성까지).

### 2. 실행 모드 2종 — 실행 단위는 기존 `run_one_hadd.sh` 재사용
run_one_hadd.sh 는 cmsenv bootstrap, `@filelist` (argv 한계 회피),
출력 sanity 를 이미 내장한 검증된 러너다 (validation hadd 에서 사용 중).

- `--mode local --jobs N`: ThreadPoolExecutor 로 N-way 병렬, **완료 대기 +
  프로세스별 OK/FAIL·시간·크기 요약표**, 실패 시 exit 1 과 실패 목록 출력.
  (구 스크립트의 local 모드는 fire-and-forget Popen — 대기도 결과 확인도
  없었다.) 로그: `_merge_workdir/<base>_<ts>/logs/<proc>.log`.
- `--mode condor`: 프로세스당 1 job. submit 템플릿은
  `submit_hadd_validation.py` 와 동일 컨벤션 (x509userproxy / getenv /
  MY.WantOS / request_memory / `queue args from arguments.txt`).

### 3. 실패 복구 워크플로
```bash
python3 merge_outputs.py --base ... --mode local --jobs 8          # 1차
python3 merge_outputs.py --base ... --mode local --jobs 8 --skip-existing   # 실패분만
```

## 검증 (이 환경에서 end-to-end 실행)
사용자 제공 74개 디렉토리 이름 그대로 가짜 트리 + stub runner 로:
- 발견: 76 merge 대상 / 미매칭 2 (`_plot_workdir`, 빈 디렉토리) 정확 분리,
  샘플 디렉토리 안의 하위 디렉토리(재제출 폴더 흉내)는 무해.
- 필터: `--only 'QCD_*' 'TTbar_*' --exclude 'QCD_HT2000*'` → 9 대상 (기대치).
- local 6-way + 고의 실패 1건(TTWZ): 75 OK / 1 FAIL, exit=1 전파,
  `--skip-existing` 재실행 시 TTWZ 1건만 재대상 — 복구 루프 확인.
- condor `--dry-run`: `merge.sub`/`arguments.txt` 내용 검증 (템플릿 일치).
- **미검증(환경 제약)**: 실제 hadd/condor_submit — Tier3 에서 첫 실행은
  `--list` → `--mode condor --dry-run` 순으로 확인 권장.

## 롤백
`merge_outputs.py` 삭제 (기존 스크립트 무변경 — 신규 파일 추가만).
