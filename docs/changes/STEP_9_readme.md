# STEP 9 — README 전면 갱신

- 날짜: 2026-06-12
- 요구사항: 14 (README를 현재 코드 상태에 맞게 갱신)
- 변경: README.md
- 백업: docs/backup_20260611/README.md

## 무엇을
구식 내용(el7/CMSSW_10_6_28, ttHHanalyzer_trigger, 위치 인자 6개 CLI,
깨진 6장 "확장 가이드")을 현재 구조로 교체. ttCat categorization 섹션
("tt+jets Event Categorization", 약 105-246행)은 최신이라 그대로 보존.

새/갱신 섹션: (1) 분석 모드 표 main/btagtrig/prescan/debug + 제거 모드 명시,
debug 사용법. (2) 사전 준비/컴파일 CMSSW_14_2_1·correctionlib·EventShape
정적 라이브러리·도구 빌드. (3) 보정 경로 표 + FATAL exit 맵(40-49).
(4) Selection: kCutSequence·SelectionCuts.h·AN Tab.55. (5) 로컬 실행
--flag value. (6) Condor: report/files-per-job/resubmit/resubmit-to,
마스터 filelist 자동 분할, per-file split 불필요. (7) 확장 가이드 정정.

## 검증
- 코드펜스 14개(짝수 균형), 헤더 28개 정상.
- 옛 잔재(ttHHanalyzer_trigger/kBTagSFDerivation/깨진 6장) 0건
  (CMSSW_10_6 언급 1건은 "이전됨" 설명으로 의도적).

## 롤백
cp docs/backup_20260611/README.md .
