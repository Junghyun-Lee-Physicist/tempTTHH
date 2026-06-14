# STEP 0 — docs 변경기록 체계 + 원본 백업

- 날짜: 2026-06-11
- 요구사항: 10 (변경 시 docs 아래 상세 기록 + 원본 백업)
- 변경 파일: 없음 (신규 파일만 추가)
- 신규: `docs/changes/README.md` (규약+템플릿), `docs/changes/STEP_0_docs_system.md` (본 문서),
  `docs/backup_20260611/ttHHanalyzer_unified.cc`, `docs/backup_20260611/ttHHanalyzer_unified.h`

## 무엇을 했나
1. `docs/changes/` 디렉토리 생성 — 이후 모든 스텝의 변경 기록(`STEP_N_*.md`)을 담는다.
   기록 규약과 템플릿은 `docs/changes/README.md` 참조.
2. `docs/backup_20260611/` 생성 — 2026-06-09 시점의 analyzer 원본
   (`ttHHanalyzer_unified.cc` 160,939 bytes / `ttHHanalyzer_unified.h` 135,619 bytes,
   md5 `530c454a...` / 원본과 백업 md5 일치 확인)을 보존.
   이 본은 **stitch 통합(StitchFactors 적용·모드 게이팅)까지 포함, stitchWeight branch는 미포함** 상태다.

## 왜 했나
이후 스텝들(모드 축소, selection 정리, 경로 yml화, 주석 정리 등)이 큰 폭의 코드
삭제·재배치를 동반하므로, 어떤 변경이든 **원본 대조와 롤백이 가능**해야 한다.

## 롤백 방법
백업 자체는 롤백 대상이 아니다. 임의 스텝 이후 원본 복귀:
```bash
cp docs/backup_20260611/ttHHanalyzer_unified.cc .
cp docs/backup_20260611/ttHHanalyzer_unified.h  .
```

## 검증
- `diff <원본> <백업>` = 0줄 (cc, h 모두) — 통과
- md5 동일 — 통과
