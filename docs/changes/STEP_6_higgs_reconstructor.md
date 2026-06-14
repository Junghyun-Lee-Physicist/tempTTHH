# STEP 6 — HiggsReconstructor 클래스 (HH/ZH/ZZ 동시 재구성)

- 날짜: 2026-06-12
- 요구사항: 5b (다양한 Higgs reco를 클래스로 분리; di-lepton 방식의 비효율 제거)
- 신규: `include/HiggsReconstructor.h`, `src/HiggsReconstructor.cc` (Makefile은
  src/*.cc 와일드카드라 무변경 — 공유 라이브러리에 자동 포함)
- 변경: `ttHHanalyzer_unified.h`, `ttHHanalyzer_unified.cc`
- 백업: analyzer는 Step 0 백업; 신규 파일은 백업 불필요

## 무엇을/왜
- selectObjects의 인라인 HH 재구성(pair cache + C(N,4)×3 pairing, 3372자)을
  `HiggsReconstructor` 클래스로 추출.
- **di-lepton 대비 핵심 개선**: di-lepton `diMotherReco`는 가설(HH/ZZ/HZ)마다
  전체 조합을 재탐색(3중 비용). 본 클래스는 pair cache 구축 후 **조합 sweep
  한 번**에 세 가설의 χ²를 동시 평가 — 비용 1/3.
- 비대칭 가설(ZH)은 pairing당 (pair1↔pair2) 양방향 할당 평가 (대칭 HH/ZZ는 생략).
- χ² = (m−target)²/√(sumPt/2×0.2) — 원본 식 그대로.
- HH 결과는 기존 멤버(`_minChi2Higgs/_bbMassMin1,2Higgs/_bpTHiggs1,2`)로 복사
  (갱신 조건 동일) — **기존 히스토/branch 경로 하위 호환**.
- ZH/ZZ는 신규 멤버 6개 + tree branch 6개:
  `chi2ZH, mZcandZH, mHcandZH, chi2ZZ, mZ1ZZ, mZ2ZZ` (-1 = 미수행/b-jet<4).
- nb<4면 전 결과 invalid (sentinel -1).
- debug hook `[dbg][higgsreco]`: chi2HH/ZH/ZZ 추적 (NaN·분포 sanity).

## 검증 (전부 통과)
1. **Oracle 비트 동일성**: 실수학 TLorentzVector 스텁으로 STEP6 이전 인라인
   알고리즘(원문 복사)과 클래스 HH 결과를 무작위 이벤트 10,000개(nb=4–6)
   전수 비교 — 최초 시도에서 **2280 mismatch 발견**: 원본은 두 χ² 항(double)을
   더한 뒤 한 번만 float로 내리는데, 클래스가 항마다 캐스팅했던 차이.
   chi2Term을 double 반환 + 합산 후 단일 캐스팅으로 수정 → **mismatch 0
   (chi2/mass1/mass2/pt1/pt2 전부 비트 동일)**.
2. ZH: 무차별(brute-force, 양방향 할당) oracle과 값 차이 max **0** —
   탐색 공간 완전 동일 입증.
3. 경계: nb=3 → 전 가설 invalid ✔. brace 불변 ✔.

## 롤백
신규 2파일 삭제 + analyzer의 [STEP6] 블록을 Step 0 백업의 인라인 블록으로 복원.

## 로컬 검증
make 후 main 1파일: 기존 minChi2Higgs 히스토가 STEP6 전과 **바이너리 동일**해야
(oracle이 보증). debug 모드에서 `[dbg][higgsreco]` chi2 분포 확인.
