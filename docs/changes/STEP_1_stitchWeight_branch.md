# STEP 1 — `stitchWeight` branch를 btagtrig/main output tree에 추가

- 날짜: 2026-06-11
- 요구사항: 14 (stitch weight 관련 업데이트)
- 변경 파일: `ttHHanalyzer_unified.h`, `ttHHanalyzer_unified.cc`
- 백업: `docs/backup_20260611/` (Step 0에서 백업됨)

## 무엇을 바꿨나 — 정확히 4곳, 12줄 추가 (삭제 0줄)

### `ttHHanalyzer_unified.h` (2곳, +10줄)

1) **멤버 선언** — line 1304, `_expandedTtbarId` 선언 바로 아래:
```cpp
Float_t _stitchWeight  = 1.0f;      // [stitch] per-event stitch multiplier written to
                                    // the output tree. 1.0 for Data / non-plan samples /
                                    // modes where stitching is gated off (trigsf, ...).
                                    // Downstream tools rebuild the clean stitched base:
                                    //   base = genWeight*PU*L1Prefire*xsec * stitchWeight
                                    // (evtWeight already has btagSF/trigSF/normRW in it).
```

2) **output tree branch 등록** — line 2975-2978, `expandedTtbarId` branch 바로 아래:
```cpp
// [stitch] per-event stitch multiplier (1.0 if not applicable). Lets
// derivative tools (b-tag reweight / trigger SF) rebuild the stitched
// pre-SF base weight without duplicating StitchFactors logic.
_inputTree->Branch("stitchWeight",    &_stitchWeight,    "stitchWeight/F");
```

### `ttHHanalyzer_unified.cc` (2곳, +2줄)

3) **per-event 초기화** — line 1356, process()의 `_expandedTtbarId = -1;` 직후:
```cpp
_stitchWeight    = 1.0f;   // [stitch] tree branch default (Data / non-plan / gated modes)
```

4) **multiplier 기록** — line 1501, `_evtWeight *= stitchMult;` 직후:
```cpp
_stitchWeight = static_cast<float>(stitchMult);  // [stitch] -> output tree branch
```

실행 흐름: process() 시작에서 1.0 reset → (main/btagtrig + plan 샘플일 때만)
resolve 직후 stitch 블록에서 0/r 기록 → fillTree()의 `_inputTree->Fill()`이
멤버 값을 그대로 직렬화. Data/비-plan 샘플/게이트된 모드(trigsf 등)는 1.0 유지.

## 왜 바꿨나
btagtrig output skim(`Tree/Tree`)을 읽는 파생 도구(`bTagSF_ReweightStudy`,
`TriggerStudy`)는 `evtWeight`를 쓰지 않고 weight를 구성요소로부터 재계산한다.
그런데 `evtWeight`에는 btagShapeSF × trigSF × btagNormReweight가 이미 곱해져
있어 b-tag SF "유도"의 깨끗한 base로 쓸 수 없다 (순환). 도구가
`base = genWeight × PU × L1Prefire × xsec × stitchWeight`
로 **SF 없는 stitched base**를 복원할 수 있도록 stitch multiplier를 독립 branch로
내보낸다. 이는 ttHH AN App. D.0.4의 유도 방식(ttbb 샘플을 owned category 부분만
사용 = stitched 조성으로 유도)과 정합한다. StitchFactors 로직을 도구 쪽에
복제하지 않아도 된다 (single source of truth 원칙).

## 원래 형태
원본에는 `_stitchWeight` 멤버·branch·기록이 전혀 없었다 (stitch multiplier는
`_evtWeight`에 곱혀질 뿐 출력으로 보존되지 않음). 원본 전체는
`docs/backup_20260611/` 참조. 변경 diff:
```
ttHHanalyzer_unified.cc : 2 hunk (+2줄)   @ 1356, 1501
ttHHanalyzer_unified.h  : 2 hunk (+10줄)  @ 1304-1309, 2975-2978
```

## 롤백 방법
```bash
cp docs/backup_20260611/ttHHanalyzer_unified.cc .
cp docs/backup_20260611/ttHHanalyzer_unified.h  .
```
(또는 위 4곳의 추가 줄만 제거)

## 검증 (전부 통과)
1. 백업 대비 diff = 정확히 4 hunk / +12줄 / 삭제 0 — 부수 변경 없음 ✔
2. `_stitchWeight` 참조 4곳 (선언/branch/reset/기록) grep 일치 ✔
3. brace balance 원본과 동일 (cc=1, h=1; 문자열 내 brace로 인한 기존값) ✔
4. branch 위치 `expandedTtbarId` 직후, leaf 타입 `/F` ✔
5. 흐름 순서: reset(1356) → stitch 기록(1501) → fillTree(1690) — Fill 이전에 값 확정 ✔
6. 동일 편집본이 이전에 stub harness 컴파일 검증을 통과한 outputs본과 byte-identical ✔

## 후속 (이 branch의 소비자)
- `bTagSF_ReweightStudy` / `TriggerStudy`의 `NtupleReader`에 `expandedTtbarId`,
  `stitchWeight` branch 읽기 추가 → baseWeight에 stitchWeight 곱 (추후 스텝, 요구 4).
- **주의**: 이 branch가 포함된 skim을 얻으려면 btagtrig 모드 재실행 필요.
