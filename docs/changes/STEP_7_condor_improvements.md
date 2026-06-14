# STEP 7 — Condor submitter 개선 (master list 분할 / files-per-job / 실패 감지·재제출)

- 날짜: 2026-06-12
- 요구사항: 11 (filelist 자동 분할 + job당 파일 수 지정), 13 (실패 감지·리스트업·재제출·별도 출력)
- 변경: `submit_job_FH_Tier3_unified.py`, `AnalyzerConfig/*.yml` (files_per_job 키)
- 백업: `docs/backup_20260611/submit_job_FH_Tier3_unified.py` (Step 2 백업 = 원본)

## 무엇을
1. **CLI(argparse)**: `--mode {main,btagtrig,prescan,debug}`, `--files-per-job N`,
   `--resubmit`, `--resubmit-to SUB`, `--report`. 코드 내 상수 수정 불필요.
2. **files-per-job**: 샘플당 마스터 filelist(`filelist_<sample>.txt`) 하나만
   있으면 N개씩 결정적 chunk로 잘라 job 생성 (per-job filelist 자동 작성).
   `<sample>_<jobIdx>.root` ↔ chunk 영구 1:1 (같은 N으로 부르는 한).
   우선순위: CLI > yml `common.files_per_job` > 1. **N=1 = 기존 동작과 동일.**
   → `make_filelists.py`의 per-file split 디렉토리(filelistTier3/<sample>/)는
   더 이상 필요 없음 (Step 9 README에 명시 예정).
3. **실패 감지/재제출**: `--report` = 제출 없이 샘플별
   `files/jobs/complete/missing(+인덱스 목록)` 출력. `--resubmit` = 완료 output
   스킵, 미완료만 재queue. `--resubmit-to SUB` = 재제출 output을
   `<출력디렉토리>/<SUB>/`로 분리 저장 (원본과 비교 검증 용이).
   ⚠ 재제출/리포트는 **반드시 원 제출과 같은 files_per_job**으로 (chunk 매핑).

## 검증 (sandbox에서 전부 실행, 통과)
- T1: 7파일·N=3 → 3 jobs(3/3/1), per-job filelist 내용·output 인덱스 정확
- T2: N=1 → 7 jobs (하위호환)
- T3: 완료={0,2} 가짜 판정 → job1만 재queue
- T4: `--resubmit-to resub_v2` → output 경로 분리 + 디렉토리 생성
- T5: `--report` → 제출 0, `complete=2 missing=1 -> idx [1]` 리포트
- `--help` 실행 정상, `ast.parse` 구문 OK

## 로컬 검증
```bash
python3 submit_job_FH_Tier3_unified.py --mode main --report            # 현황만
python3 submit_job_FH_Tier3_unified.py --mode main --files-per-job 5   # 제출
python3 submit_job_FH_Tier3_unified.py --mode main --files-per-job 5 --resubmit --resubmit-to retry1
```

## 롤백
```bash
cp docs/backup_20260611/submit_job_FH_Tier3_unified.py .
```
argparse·chunking·resubmit/report 전체가 이 파일에 국한되므로 단일 복원으로 롤백.
