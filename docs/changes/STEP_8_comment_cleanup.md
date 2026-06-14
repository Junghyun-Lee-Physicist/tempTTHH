# STEP 8 — 죽은 코드/주석 정리 + mojibake 복구 + 한글 배너

- 날짜: 2026-06-12
- 요구사항: 9 (안 쓰는 것 정리), 16 (주석 한글화)
- 변경 파일: `ttHHanalyzer_unified.cc`, `ttHHanalyzer_unified.h`,
  `src/tnm.cc`, `include/tnm.h`
- 백업: tnm 2파일 본 스텝에서 추가 백업
  (`docs/backup_20260611/src/tnm.cc`, `docs/backup_20260611/include/tnm.h`);
  analyzer는 Step 0 백업

## 1. 무엇을 바꿨나

### (A) `////` 죽은 코드 제거 — **cc 828줄 + h 222줄**
boosted-jet 블록(386–432), analyze()의 di-lepton 잔재(1163–1339),
fillHistos의 죽은 Fill 호출 수백 줄(2308–2699) 등 `////`(및 `//////`)로
주석 처리된 코드 전부. 추가로 3연속 이상 빈 줄을 1개로 압축(cc 82 + h 11줄).

**증명**: 제거 전후 `g++ -fpreprocessed -dD -E -P` 출력 diff = **0줄/0줄**
(cc, h 모두) — 컴파일러 수준에서 **코드 토큰이 한 글자도 변하지 않았음**을
기계적으로 입증. (A)·(B)·(F)는 이 증명 범위 안의 주석-only 변경이다.

### (B) mojibake(깨진 인코딩 한글 주석) 복구 — 4블록
cc 160–161 (base weight 출력/_SampleWeight), 260 (cutflow 요약),
268 (SetBinContent 안 하는 이유), 662 (selectJet의 b-jet 분류) —
문맥에서 의미를 복원해 정상 한글로 재작성.

### (C) tnm의 `--val-*` 잔재 제거 (Step 2에서 미룬 것) — **코드 변경**
- `src/tnm.cc`: `--val-*` 파서 11줄 + validation 플래그 출력 블록(15줄)
- `include/tnm.h`: `valScenario` 등 필드 13줄
검증: `val[A-Z]` 잔존 0, brace 0/0.

### (D) `motherReco()` 죽은 메서드 제거 — **코드 변경**
호출처 **0건**(grep 전수)으로 확인된 단일-mother χ² 메서드.
.cc 정의(1292자) + .h 선언 제거. (`diMotherReco`는 이미 부재 —
Step 6의 HiggsReconstructor가 di-mother 재구성을 전담.)

### (E) di-lepton eventShape 잔재 주석 2줄 제거 (process 직전)

### (F) 핵심 함수 한글 배너 4개 추가
`process`(파이프라인 5단계), `fillTree`(skim branch 목록),
`analyze`(event shape; Higgs reco 위치 주의), `createObjects`(객체 정의).

### (G) Fill 없는 histogram 조사 — **목록만, 제거 안 함**
TH1/TH2 포인터 선언 14개 중 어디서도 `->Fill()` 되지 않는 것:
- `_hJES` — JES 진단용 placeholder로 보임
- `hmet` — MET hist 선언만 존재
- (`h`는 함수 파라미터 매칭 오탐)
대부분의 hist는 컨테이너(map/vector) 기반이라 이 단순 스캔의 범위 밖.
빈 hist 2개는 output 스키마(plotter/structure_info)와 얽혀 있어 제거하지
않고 후보로만 기록 — 제거 원하면 별도 확인 후 1줄 diff로 처리.

## 2. 검증 (전부 통과)
1. **preprocessed-diff 0/0** — (A)(B)(F)의 주석-only 증명 (위 §1-A)
2. (C)(D) 코드 변경분: 참조 잔존 0 (`--val`/`val[A-Z]`/`motherReco`),
   brace balance 전 파일 불변 (cc/h = 1/1 기존값, tnm = 0/0)
3. mojibake(`\ufffd`) 잔존 0, `////` 잔존 0
4. 이전 스텝 기능 보존: `_stitchWeight/_dbg./kCutSequence/_higgsReco/_es_`
   55곳 전부 보존

## 3. 사용자 로컬 검증
```bash
make   # (C)(D)가 코드 변경이므로 컴파일 필수 확인
# 주석-only 증명 재현 (백업 대비):
strip(){ g++ -fpreprocessed -dD -E -P "$1"; }
diff <(strip docs/backup_20260611/ttHHanalyzer_unified.cc) <(strip ttHHanalyzer_unified.cc) | less
#  → 차이는 (D) motherReco 제거 + Step1~7.5의 실제 코드 변경분만 보여야 함
# main 1파일 회귀: Step8 전/후 output 바이너리 동일 (주석/죽은코드 제거는 무영향)
```

## 4. 롤백
```bash
cp docs/backup_20260611/src/tnm.cc src/
cp docs/backup_20260611/include/tnm.h include/
# analyzer 주석 복원은 비실용적(1,050줄) — 필요 시 Step 0 백업에서 전체 복원 후
# STEP_1~7.5 재적용
```
