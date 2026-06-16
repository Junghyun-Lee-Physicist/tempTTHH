# docs/changes — 코드 변경 기록 규약

## 스텝 인덱스 (2026-06 리팩토링)

| Step | 제목 | 핵심 |
|---|---|---|
| 0 | docs_system | 변경 기록 체계 + 백업 디렉토리 |
| 1 | stitchWeight_branch | output tree에 stitchWeight branch |
| 2 | mode_reduction_debug_mode | trigsf/validation 제거 + **kDebug** 신설 + DebugLogger |
| 3 | selection_refactor | **kCutSequence** 선언적 테이블 + SelectionCuts.h |
| 4 | paths_yml_fatal | 경로 env/yml화 + FATAL 47/48/49 |
| 5 | eventshape_library | **libEventShape.a** + tree branch 10개 |
| 6 | higgs_reconstructor | **HiggsReconstructor** (HH/ZH/ZZ 1-sweep) |
| 7 | condor_improvements | argparse + files-per-job + 재제출/리포트 |
| 7.5 | category_dispatch | **8-group 카테고리 디스패치** (라이브 버그 2건 수정) |
| 8 | comment_cleanup | 죽은 코드 1050줄 제거 + mojibake 복구 + 한글화 |
| 9 | readme | 최상위 README 전면 갱신 |
| 10 | ttbb_weight_and_muon_option | ttbb SL/DL weight 보고(⚠AN 불일치) + single-muon offline 옵션 |
| 11 | xsec_db_single_source | cross section 단일 소스(xsec_db) + weight 런타임 합성 + 과소정규화 해소 |
| 12 | fb_units_and_xsec_fix | fb 단위 전환 + AN 기반 σ/BR 수정(k 미적용) + yml 통일 |
| 13 | makefile_debug_levels | Makefile 빌드 레벨(평소 -g / DEBUG=1 ASan) |
| 14 | 3tier_weight_metCR_normcheck | 3-tier weight(trigSF/+btagShape/+normRW) + lepton CR(1ℓ+MET) + prescan WARN 수정 |
| 15 | resubmit_treecheck_region_output | resubmit 완료판정 중첩TTree 버그 + region output 분리 + report 거짓메시지 |
| 16 | sf_toggles_and_dir_split | SF 적용 토글(--trigsf/--btagsf/--btagrw) + region/SF 조합 output 디렉토리 |

별도 트랙 (선행조건 대기):
- **TRACK_A** (도구 업데이트) — btagtrig 재실행 후
- **TRACK_B** (stack category-split) — main 재실행 후

---


이 디렉토리는 2026-06 리팩토링 워크플로우의 **모든 코드 변경 이력**을 담는다.
목적: 변경이 잘못됐을 때 **이전 상태로 정확히 되돌릴 수 있도록** 하는 것.

## 규약

1. **스텝 단위 기록**: 워크플로우의 각 스텝(Step N)마다
   `STEP_N_<제목>.md` 파일 하나를 만든다.
2. **기록 필수 항목** (아래 템플릿 참조):
   - 무엇을 바꿨는가 (파일별, 위치별)
   - 왜 바꿨는가 (요구사항 번호 / 물리·구조적 근거)
   - 원래는 어떤 형태였는가 (원본 코드 스니펫 또는 백업 경로)
   - 롤백 방법 (명령어 수준)
   - 검증(validation) 방법과 결과
3. **백업**: 스텝에서 처음 변경되는 파일은 변경 **전** 상태를
   `docs/backup_<YYYYMMDD>/` 아래에 보존한다.
   - 같은 파일이 여러 스텝에서 바뀌면 최초 백업본이 "원본"이고,
     스텝별 차이는 각 STEP 문서의 diff로 추적한다.
4. **물리 로직 변경 금지 원칙**: 이 워크플로우는 구조/인프라 정리이며
   물리 로직(selection 값, weight 공식, categorization)을 바꾸지 않는다.
   불가피하게 바뀌면 STEP 문서에 AN 근거를 명시한다 (프로젝트 규약).

## 템플릿

```markdown
# STEP N — <제목>

- 날짜: YYYY-MM-DD
- 요구사항: (사용자 요구 번호)
- 변경 파일: (목록)
- 백업: docs/backup_YYYYMMDD/<파일들> (또는 "이전 스텝에서 백업됨")

## 무엇을 바꿨나
(파일별 / 위치별 상세)

## 왜 바꿨나
(근거)

## 원래 형태
(원본 스니펫 또는 백업 경로 + diff)

## 롤백 방법
(명령어)

## 검증
(방법 + 결과)
```

## 백업 현황

| 백업 디렉토리 | 내용 | 생성 스텝 |
|---|---|---|
| `docs/backup_20260611/` | `ttHHanalyzer_unified.cc`, `ttHHanalyzer_unified.h` — 2026-06-09 시점 원본 (stitch 통합본, stitchWeight branch 추가 전) | Step 0 |
| `docs/backup_20260611/submit_job_FH_Tier3_unified.py` | Step 2 변경 전 원본 (validation 지원 포함) | Step 2 |
| `docs/backup_20260611/AnalyzerConfig/*.yml` | Step 2 변경 전 원본 (모드 주석) | Step 2 |
| `docs/backup_20260611/src/CorrectionsManager.cc`, `include/CorrectionsManager.h` | Step 4 변경 전 원본 (하드코딩 경로 + WARN-continue) | Step 4 |
| `docs/backup_20260611/Makefile` | Step 5 변경 전 원본 (EventShape 타깃 없음) | Step 5 |
| `docs/backup_20260611/bTagSF_ReweightStudy_include/Config_TtCatGroup.hh` | Step 7.5 변경 전 원본 (7그룹, 61-72 미인식) | Step 7.5 |
| `docs/backup_20260611/src/tnm.cc`, `include/tnm.h` | Step 8 변경 전 원본 (--val-* 파서/필드 포함) | Step 8 |
| `docs/backup_20260611/README.md` | Step 9 변경 전 원본 (구식 el7/위치인자) | Step 9 |
| (Step 10은 analyzer만 변경 — Step 0 백업이 원본; ttbb weight는 코드 미변경) | | Step 10 |
