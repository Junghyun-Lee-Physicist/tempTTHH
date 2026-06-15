# STEP 12 (Track C 연장) — fb 단위 전환 + AN 기반 σ/BR/k 수정 + yml 4종 통일

- 날짜: 2026-06-15
- 요구: (1) cross section을 fb 단위로, (2) decay BR 미반영·틀린 σ를 AN 참조로
  수정, (3) pb→fb 시 analyzer 영향 확인, (4) prescan의 b-tag reweight 의존성
  확인, (5) yml 4종 중 무엇이 도는지
- 변경: `data/samples_2017UL.json`, `submit_job_FH_Tier3_unified.py`,
  `compute_stitch_factors.py`, `AnalyzerConfig/*.yml`(4종)
- The Barn html: 키/라벨 치환 5곳 (사용자 원본에 적용 — 본 문서 §5)

## 1. fb 단위 전환
- `cross_section_pb` → `cross_section_fb` (값 ×1000). `_meta.lumi_pb_inv`
  (41480) → `lumi_fb_inv` (41.48).
- **σ_eff = cross_section_fb × br × kfactor**.
- ⚠ analyzer 코드 **무변경**: analyzer는 `--weight`로 합성된 숫자만 받고,
  단위는 submitter 합성식에서 lumi와 약분된다. `σ[fb]×lumi[fb⁻¹]` =
  `σ[pb]×lumi[pb⁻¹]` → weight 숫자 동일 (ttbb/tt/tt4b로 검증, 비트 일치).

## 2. AN 기반 σ/BR/kfactor 수정 (ttHH AN Tab.9-16)
| 샘플 | 이전(틀림) | 수정 | 근거 |
|---|---|---|---|
| ttHH | σ_eff=0.756 fb (BR 미적용) | σ=0.669, br=(0.5824)², k=1.158 → 0.263 fb | Tab.10 |
| ttHtobb | br=1 | σ=507.1, br=0.5824 → 295.34 fb | Tab.11 |
| ttZtobb | 126.2 (역산) | σ=861, br=0.15 → 129.15 fb | Tab.14 |
| ttZZto4b | 0.0446 (역산) | σ=1.98, br=(0.15)², k=1.42 → 0.064 fb | Tab.16 |
| ttZHto4b | br=1 | σ=1.535, br=0.15×0.5824, k=1.35 → 0.181 fb | Tab.16 |
| TTTo*/ttbb* | (정상) | σ_total + 채널 BR 분리 유지 | Tab.12/9 |
| tt4b | (정상) | 296 fb, decay-incl | Tab.9 |

**핵심 수정**: signal-like 샘플의 decay BR(Z→bb=0.15, H→bb=0.5824)이 br=1.0으로
뭉개져 있던 것을 분리. `kfactor` 필드 신설(LO 샘플 k; 기본 1.0).
검증: σ_eff = xsec_fb×br×k 가 AN 명시값과 전부 일치.

### k-factor 결정 (사용자): **곱하지 않음**
ttHH(1.158)/ttZZto4b(1.42)/ttZHto4b(1.35)의 k-factor를 당장 적용하지 않기로
함 (AN 본문이 최종 정규화에 곱하는지 미확정). → **모든 샘플 `kfactor: 1.0`**.
AN 참고값은 각 샘플의 `kfactor_an_ref` 필드에 보존 — 확정 시 `kfactor`를 그
값으로 바꾸면 weight 자동 반영(코드 무변경).

### BR을 σ에 곱하지 않음 (사용자): **분리 유지**
혼동 방지를 위해 `cross_section_fb`에는 **순수 σ_total**만(decay BR 미포함),
`br`은 별도 필드. weight 합성 시에만 `σ × br × k`. 각 샘플에 `weight_note`로
"sigma_eff = σ × br = ... fb" 합성 과정을 명시. The Barn은 σ_total / br / k /
σ_eff(계산) 를 별도 컬럼으로 표시.

## 3. yml 4종 통일 (Q5 답)
이전엔 **TRACKC만 신형식**(bare-string + xsec_db), 나머지 3종은 구형식(weight
명시). 구형식도 submitter가 override로 받아 돌긴 하나 xsec_db 단일소스 이점이
없음. → **4종 전부 신형식 통일**:
- `main`: 39 samples (main_TRACKC는 제거 — main과 동일했음. `--mode main`이면
  자동 선택되므로 `--config` 불필요)
- `prescan`: 24 (MC만; Data는 genWeight 없음)
- `btagtrig`: 39 (MC+Data)
- 모두 `analysis_mode`만 다르고 common(xsec_db/prescan/lumi_fb_inv)·샘플은 공유.
→ `--config` 생략 시 `..._<mode>.yml` 자동 선택되므로 4종 다 바로 돈다.

## 4. prescan의 b-tag reweight 의존성 (Q4 답)
**없다.** prescan은 `requireDerived=false`(main/debug만 true)라 CorrectionsManager가
파생 보정 누락을 FATAL로 안 만든다(WARN+1.0). 게다가 `runPrescan()`은
selectObjects/applyEventScaleFactors/getBTagReweight를 **호출조차 안 함**
(gen-level 스칼라만 누산). → **b-tag reweight JSON 없이 prescan 정상 동작.**

## 5. The Barn html 패치 (사용자 원본에 적용)
fb json과 맞추려면 5곳 치환:
1. `data-k="cross_section_pb"` → `cross_section_fb`, 라벨 `σ (pb)` → `σ (fb)`
2. `s.cross_section_pb` → `s.cross_section_fb` (JS 데이터 접근)
3. `cross_section_pb != null` → `cross_section_fb != null`
4. `cross_section_pb == null` → `cross_section_fb == null`
5. `(s.cross_section_pb != null ? 1 : 0)` → `(s.cross_section_fb != null ? 1 : 0)`
(선택) br/kfactor 컬럼 추가 시 thead/renderRows에 셀 추가.

## 6. 검증
- σ_eff = xsec_fb×br×k ↔ AN Tab.9-16: 10개 샘플 전부 일치.
- fb 전환 weight 불변: ttbb/tt/tt4b가 pb시절과 비트 일치.
- yml 4종 파싱 + weight 합성: 전부 정상.
- prescan: requireDerived=false + runPrescan이 SF 미호출 — 코드로 확인.

## 롤백
`cp docs/backup_20260611/{submit_job...,compute_stitch_factors.py} .` +
data/samples_2017UL.json은 업로드본으로 복원.
