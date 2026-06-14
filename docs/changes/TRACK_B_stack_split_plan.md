# TRACK B — Stack plotter category-split 계획 (옵션 b; main 재실행 후)

상태: **설계 확정, 구현 보류**. main output Tree(stitched evtWeight +
expandedTtbarId branch 포함)가 선행 필요.

## 결정 (사용자 승인)
옵션 **(b)**: analyzer가 카테고리별 hist를 추가 생산하지 않는다. 대신 plotter
(또는 중간 hist 생성 단계)가 main output `Tree`에서 `expandedTtbarId` +
`evtWeight` 를 읽어 ttbar family 샘플을 fine category로 분리해 hist를 만든다.
→ analyzer hist 폭증 없음, 카테고리 정의 변경 시 analyzer 재실행 불필요.

## 현재 상태
- `stack_plotter.C`: category 분리 없음 (yml의 sample 단위로만 stack).
- main output Tree: `expandedTtbarId`, `evtWeight`, `stitchWeight` branch 존재
  (Step 1·7.5에서 확보). stitch 적용 후 inclusive ttbar 안에 LF/cc/ttb 혼재.

## 변경 항목 (구현 시)
1. ttbar family(inclusive 3 + ttbb 3 + tt4b) 샘플은 Tree loop으로 읽어
   `ClassifyExpanded(expandedTtbarId%100)` 기준 6 fine category
   (LF/cc/ttb/tt2b/ttbbb/tt4b)로 분리해 per-category hist 작성.
   - 카테고리 함수는 **공유 헤더 `Config_TtCatGroup.hh`** 사용 (plotter에 사본
     금지 — Step 7.5 single source 원칙).
2. 비-ttbar 샘플은 기존대로 sample 단위.
3. stack 순서: ttH AN Fig 51/52 관례 (signal→ttbb→ttcc→tt+lf→ttV→V+jets→
   single t→diboson→multijet) 에 맞춰 yml 정렬 또는 plotter에서 정렬.
4. ratio panel: rough validation 단계는 0.0-2.0 유지, signal extraction 진입
   시 0.5-1.5 (AN 관례) — 별도 결정.

## ⚠ stitching 이중카운팅
현 plotter는 stitching 미적용(inclusive + dedicated 동시 stack → 이중). 옵션 (b)
구현 시 evtWeight에 이미 stitchWeight(0/r/1)가 곱해져 있으므로, **Tree의
evtWeight를 그대로 쓰면** 이중카운팅이 자동 해소된다 (analyzer가 비-owned
이벤트를 weight 0으로 만들었기 때문). plotter는 SF를 다시 곱하지 않는다
(이중 적용 금지 — 프로젝트 규약).

## 검증 계획 (main output 확보 후)
- 카테고리 합 = 전체: Σ(per-category hist) == sample 전체 hist (weight 보존).
- 이중카운팅 부재: inclusive tt2b 영역 yield ≈ 0 (stitch=0 확인),
  dedicated ttbb가 그 영역을 채움.
- plotter category 함수 출력 == analyzer/도구의 MakeProcessKey 분류 (공유 헤더).
