# STEP 3 — Selection 구조 현대화 (제안 ①②③④ 적용) + 미사용 cut 정리

- 날짜: 2026-06-11
- 요구사항: 6 (미사용 selection 제거 + 가독성) / 승인된 제안 ①②③④
- 변경 파일: `ttHHanalyzer_unified.h`, `ttHHanalyzer_unified.cc`
- 신규 파일: `include/SelectionCuts.h`
- 백업: `docs/backup_20260611/` (Step 0에서 백업됨 — 동일 파일)

## 1. 무엇을 바꿨나

### ① `std::map<std::string,float> cut` → `include/SelectionCuts.h` (namespace `Cuts`)
- map의 `operator[]`는 오타 키를 **조용히 0.0으로 삽입**한다 — 무음 버그 원천.
  (실제로 `cut["eleIso"]`는 map에 키가 주석 처리된 채 4곳에서 참조되고 있었다 —
  다행히 4곳 전부 **주석 코드**라 라이브 버그는 아니었지만, 함정이 실존함을 확인.)
- `constexpr` 타입 보존 상수로 교체: 오타=컴파일 에러, int/float 구분 유지,
  lookup 비용 0, **상수마다 AN 출처 주석** (ttH AN-19-094 Tab.55/§4.x).
- 라이브 코드의 `cut["X"]` 사용 24곳 전부 `Cuts::X`로 치환
  (nJets/nLeptons/nbJets/jetPt/jetEta/jetID/jetPUid/sixthJetPt/HT/
   leadElePt/leadMuonPt/subLeadElePt/subLeadMuonPt/eleEta/muonEta/muonIso).
- hadW window 30/250은 코드에 하드코딩돼 있던 것을 `Cuts::hadWMassLo/Hi`로 승격.
- **미사용 키 제거** (요구 6): `boostedJetPt`, `boostedJetEta`, `bTagDisc`(0.80 —
  b-jet 정의는 `objectJet::valbTagMedium`=0.3040을 쓰므로 혼동 잔재였음),
  `hadHiggsPt`, `nlJets`(사용 0건), `eleIso`(주석 키), `trigger/filter/pv`
  (=1.0 enable 플래그 — 항상 켜져 있었고, 모드별 on/off는 테이블의 enforceIn이
  담당하므로 의미 소멸). `////` 주석 코드 안의 잔존 참조 3건(boosted 블록)은
  Step 8 주석 정리 때 블록째 제거 예정.

### ② 선언적 cut 테이블 — `kCutSequence`
- selectObjects의 if-나열(약 220줄)을 **테이블(14 entries) + 실행 루프(약 20줄)**로 교체.
- `CutDef { step, label, enforceIn, recordOnlyIfPass, pass, onAfter }`:
  - `enforceIn`: cut이 reject로 작동하는 모드 비트
    (`kSelBitMainLike`=main+debug, `kSelBitBtagTrig`, `kSelEnforceAll`, `kSelObserveOnly`)
  - `pass`: capture-less 람다 (함수 포인터 — 이벤트당 할당 0)
  - `onAfter`: 생존 직후 부수 작업 — LepVeto 뒤 통계, **HT 뒤 SF 적용**
    (SF가 step 7과 8 사이에 와야 하는 기존 [MOVED] 의미 보존)
- selection 전체가 .cc의 **한 화면 표**로 보이고, 라벨·순서·강제 여부가
  한 곳에서 정의된다.
- 기존 인라인 SF 블록(약 100줄)은 `applyEventScaleFactors(event*)` 메서드로
  **비트 동일 이동** (내용 무변경 — [MOVED] 주석 포함 그대로), 통계 2줄은
  `computeLeptonJetStats(event*)`로.

### ③ 관찰(observe) 단계의 명시화
- nbJets≥3/≥4: `enforceIn=kSelObserveOnly` + `recordOnlyIfPass=true`
  (충족 시에만 기록 — cut 아님). btagtrig에서 꺼지는 cut들
  (HadTrigger/LepVeto/nbJets2/HadW)은 `recordOnlyIfPass=false`
  (도달 시 항상 기록 — 기존 "policy off여도 processStep은 호출" 동작 보존).
  두 가지 다른 "관찰" semantics가 필드로 명시된다.

### ④ `SelectionPolicy` 슬림화 (7 bool → 3 bool)
- cut on/off 4종(`applyTriggerCut/applyLeptonVeto/applyBJetCut/applyHadWMassCut`)은
  테이블 `enforceIn`으로 흡수 — 위치 인자 bool 나열 제거.
- 남은 비-cut 동작 3종(`collectLeptons/requireLeadMuonOnly/doHiggsReconstruction`)은
  **명시적 멤버 대입**으로 초기화 (위치 인자 아님).
- `selectionModeBit(AnalysisMode)` 헬퍼 추가 (debug→main 비트 매핑).

### [STEP3 debug hook] cut 테이블 무결성 검사
- kDebug 모드 시작 시(loop 진입) `[dbg][seltable]`로 테이블 전체를 덤프하고
  **kCutSequence ↔ CutStep enum 순서 ↔ _cutStepLabels** 3중 정의의 1:1을
  런타임 검증한다 (불일치 시 항목별 MISMATCH 표시).

## 2. 왜 바꿨나
요구 6(가독성) + 사용자 승인 제안. 핵심 동기: 무음 0-삽입 함정 제거,
cut/라벨/순서의 3중 수동 동기화 제거, selection의 단일 정의화.

## 3. 원래 형태
`docs/backup_20260611/ttHHanalyzer_unified.{cc,h}`:
- .h: `std::map<std::string,float> cut {...}` 27개 키 (8개 미사용/주석)
- .h: `SelectionPolicy` 7 bool + 위치 인자 초기화
- .cc: selectObjects 내 Step 0~11 if-나열 + 인라인 SF 블록 + 인라인 통계
누적 diff (Step1+2+3): cc −324/+306, h −170/+105.

## 4. 롤백 방법
```bash
cp docs/backup_20260611/ttHHanalyzer_unified.{cc,h} .   # Step1-3 모두 롤백
rm include/SelectionCuts.h
# Step1(stitchWeight)만 유지하려면 STEP_1 문서의 4편집 재적용
```

## 5. 검증 (전부 통과)

**자동 (sandbox 실행)**:
1. **Semantics 동일성 시뮬레이션 — 가장 강한 검증**: 테이블 루프와
   STEP3 이전 if-나열(oracle)을 별도 구현으로 컴파일,
   **무작위 이벤트 200,000개 × {main, btagtrig}** 에 대해
   (kept 여부, 기록된 step 목록, 통계 hook, SF hook) 4-튜플 전수 비교 →
   **mismatch 0** . 강제/관찰/조건부 기록의 모든 조합이 비트 동일.
2. 라벨 1:1 대조: kCutSequence 14개 label ↔ `_cutStepLabels` 14개 — ALL MATCH
3. `SelectionCuts.h` 단독 컴파일 + static_assert (값 6/2/40/500/30/250)
4. 새 policy + selectionModeBit 발췌 컴파일+실행 테스트 통과
5. 잔존 grep: `cut[` 라이브 0건(3건은 //// 죽은 주석), 제거 policy 필드 0건
   (2건은 설명 주석), brace 불변, stitchWeight 배선(4+debug hook 1) 보존
6. SF 블록 이동: 추출 시 중괄호 균형 assert + 원문 그대로 삽입 확인

**사용자 로컬 검증**:
```bash
make
# 1) [필수] 회귀: 같은 입력 1파일, Step3 전/후 main 모드 →
#    hCutFlow / hCutFlow_w_full / 출력 tree 가 바이너리 동일해야
#    (semantics 시뮬레이션이 보증하지만 실데이터로 재확인)
# 2) debug 모드: 시작 로그에서
#    grep '\[dbg\]\[seltable\]' — integrity: OK 확인
#    grep '\[dbg\]\[sel\]'      — 종료 요약 cutflow == hCutFlow 대조
```

## 6. 동작 불변 보증
- cut 값 자체는 전부 동일 (map 값 → constexpr 동일 값; int/float 비교 결과 불변).
- 시퀀스/기록/강제/부수작업의 semantics는 §5-1의 200k 이벤트 전수 비교로
  main·btagtrig 모두 **비트 동일** 입증.
- prescan은 selectObjects를 호출하지 않으므로 무관.
- SF 블록은 내용 무변경 이동 (Step 2에서 토글 제거된 상태 그대로).
