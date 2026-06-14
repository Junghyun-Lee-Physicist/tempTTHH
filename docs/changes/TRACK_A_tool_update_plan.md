# TRACK A — b-tag SF 도구 업데이트 계획 (btagtrig 재실행 후)

상태: **설계 확정, 구현 보류**. 도구는 새 skim(stitchWeight+expandedTtbarId
branch 포함)을 입력으로 받아야 검증 가능하므로, btagtrig 모드 재실행이 선행.

## 현재 상태 (조사 결과)
- `bTagSF_ReweightStudy/src/NtupleReader.cc`: `genTtbarId`만 읽음
  (`stitchWeight`/`expandedTtbarId` branch 미인식).
- `Config_TtCatGroup.hh`: **이미 Step 7.5에서 8그룹 디스패치로 확장됨**
  (도구가 공유). 도구 소스는 string 인터페이스만 써서 enum 확장에 안전.
- 따라서 도구 쪽 코드 변경의 핵심은 NtupleReader + base weight 조립 + tt+nb
  stat 리포트 3가지.

## 변경 항목 (구현 시)
1. **NtupleReader**: `stitchWeight`(Float) + `expandedTtbarId`(Int) branch
   추가 read (가드: 없으면 1.0 / -1). data member + SetBranchAddress + Getter.
2. **base weight**: Pass 1/2 의 ω_base 에 stitchWeight 곱
   (`ω_base = genW·PU·L1·xsec/Σgenw · stitchWeight`) — analyzer와 동일 stitched
   조성으로 r 유도. ⚠ analyzer와 **반드시 동일** (single source: 같은 공식).
3. **process key**: `MakeProcessKey(sample, expandedTtbarId)` 호출 (현재는
   genTtbarId 전달) → 8그룹(tt+nb 포함) 유도.
4. **tt+nb stat 리포트**: makeReweightJSON에서 각 (group,syst,bin)의 유효 통계가
   ttH AN §A.2.1의 ≤30% 기준을 만족하는지 출력. tt+nb 그룹이 미달이면 tt+B로
   병합하는 fallback 플래그 제공 (AN 처방).

## 검증 계획 (새 skim 확보 후)
- Pass-1 closure: 각 (group,bin)에서 `Σω_noSF ≈ Σω_reweighted` (per-bin yield 보존).
- analyzer↔도구 stitched base 일치: 같은 sample 1개에서 ω_base 합 비교.
- 8그룹 JSON에 tt+nb 키 존재 → analyzer main이 exit 46 없이 로드되는지.
- tt+nb stat 리포트가 bin별 통계를 정확히 집계하는지 (수기 cross-check).

## AN 근거
ttH AN §A.2/§A.2.1 (process group, ≤30% bin stat), ttHH AN §D.0.4-5
(ttbb owned portion, tt4b 별도 유닛).
