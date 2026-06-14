# STEP 5 — EventShape 외부 라이브러리화 + hadronic analyzer 통합

- 날짜: 2026-06-11
- 요구사항: 5a (EventShape를 소스-include 대신 별도 컴파일 라이브러리로;
  hadronic에서 event shape variable 사용 가능하게)
- 변경 파일: `Makefile`, `ttHHanalyzer_unified.h`, `ttHHanalyzer_unified.cc`
- 신규 파일: 없음 (기존 `EventShape/Class/` 패키지를 그대로 사용)
- 백업: `docs/backup_20260611/Makefile` (본 스텝)

## 1. 무엇을 바꿨나

### 1.1 라이브러리화 (`lib/libEventShape.a`)
di-lepton analyzer는 `#include "EventShape/Class/src/EventShape.cc"`로 **소스를
직접 include**했다 (매 빌드 재컴파일, ODR 위험, 의존 불투명). 대체:
- Makefile에 정적 라이브러리 타깃 추가:
  `EventShape/Class/src/EventShape.cc` → `tmp/EventShape.o` → `ar rcs lib/libEventShape.a`
- `all` 타깃과 application 링크에 `-lEventShape` 추가
  (단독 빌드도 가능: `make lib/libEventShape.a`)
- 소비자는 헤더만 include: `#include "EventShape/Class/interface/EventShape.h"`
  (`-I.` 가 이미 INCLUDE_FLAGS에 있어 추가 경로 불필요)
- `EventShape/Class/` 소스 자체는 **무변경** (외부 저작 코드 — D.G. Sheffield).

### 1.2 hadronic analyzer 통합
- `analyze()` 시작부에서 selected jets / b-jets의 `TVector3`로 두 `EventShape`
  인스턴스를 만들어 5변수 × 2 (jets/b-jets) 계산:
  sphericity / transSphericity / aplanarity / C / D.
  (di-lepton과 동일 변수 집합; di-lepton의 `eventShapeJet/eventShapeBjet`
  `new` 할당과 달리 **스택 객체** — 누수 없음.)
- 객체 < 2개면 momentum tensor가 퇴화하므로 계산하지 않고 **-1 유지**
  (btagtrig에서 b-jet 0~1개 이벤트 보호).
- 멤버 10개(`_es_*`, Float_t) + **output tree branch 10개** 추가:
  `aplanarity, sphericity, transSphericity, eventC, eventD,
   bjet{Aplanarity,Sphericity,TransSphericity,EventC,EventD}` —
  stitchWeight branch 바로 아래. per-event reset(-1)은 process() 시작부.
- 호출 순서 보장: process() = createObjects → selectObjects → **analyze**(계산)
  → fillHistos → fillTree(branch 기록) — 기록 전에 값 확정.
- 히스토그램은 이번에 추가하지 않음 (tree 기반 검증 후 필요 시; di-lepton의
  hAplanarity 등은 참조용).

### 1.3 debug hook (Step 5분)
`[dbg][evtshape]`로 sphericity/aplanarity/C/D/bjetSphericity 추적 —
종료 요약의 min/max로 **정의역 검증** (sphericity∈[0,1], aplanarity∈[0,0.5],
C,D∈[0,1]; -1은 미계산 표식), BAD 카운트로 NaN 검출.

## 2. 왜 바꿨나
사용자 지시: di-lepton의 소스-include 방식이 비효율적 — 별도 컴파일 라이브러리로.
event shape는 FH ttHH(4b)에서 QCD(평면적/2-jet적) vs ttHH(구형) 분리에 유효한
DNN 입력 후보 — hadronic tree에 선제 탑재.

## 3. 원래 형태 / 롤백
- Makefile 원형: `docs/backup_20260611/Makefile` (EventShape 타깃 없음)
- analyzer: Step 0 백업 (EventShape 관련 코드 전무; analyze()의 //// 죽은
  주석 블록에 과거 di-lepton식 잔재만 있었음 — 그 블록은 Step 8 제거 예정)
```bash
cp docs/backup_20260611/Makefile .
# analyzer는 _es_ 멤버/branch/계산 블록 제거 (grep '_es_' 22곳 + STEP5 주석)
```

## 4. 검증 (전부 통과)
1. **EventShape.cc 실제 컴파일** (ROOT 헤더를 동작 스텁으로 대체, repo-root
   include 경로): 구문 OK.
2. **API 사용 패턴 컴파일+링크+실행**: `vector<TVector3>` → 생성자 → 5 getter —
   analyzer 통합 코드와 동일 패턴이 실제 EventShape.cc와 링크되어 실행됨.
3. **Makefile dry-run** (`make -n`): `lib/libEventShape.a` 타깃 그래프 정상
   (컴파일→ar→앱 링크에 -lEventShape), `all`에 EventShape 단계 5회 등장.
4. 배선 grep: `_es_` .h 14곳(멤버 10+branch 등) / .cc 17곳(reset+계산+hook),
   brace balance 불변.

**사용자 로컬 검증**:
```bash
make                      # libEventShape.a 생성 + 링크 확인
make lib/libEventShape.a  # 단독 빌드
# debug 모드 1파일:
grep '\[dbg\]\[evtshape\]' dbg.log     # per-event 값
grep '\[dbg\]\[acc\].*evtshape' dbg.log # min/max 정의역 + BAD=0 확인
# ROOT에서: Tree의 sphericity 분포 [0,1], aplanarity [0,0.5], -1 peak는
# (b-jet<2) 이벤트 비율과 일치해야
```
