# STEP 10 — ttbb SL/DL weight 계산(보고) + single-muon offline 옵션

- 날짜: 2026-06-12
- 요구: ttbb SL/DL weight 계산(사용자 직접 비교 예정), muon validation을
  별도 모드 없이 main의 offline 1-muon 옵션으로
- 변경: `ttHHanalyzer_unified.h`, `ttHHanalyzer_unified.cc`
- 신규 분석/보고: 본 문서

## A. ttbb weight — ⚠ AN 불일치 발견 (코드 미변경, 사용자 결정 대기)

### 사용자 요청 계산 결과 (lumi=41480 pb⁻¹, Σgenw=prescan runs.genEventSumw)
| Σgenw | ttbb_Hadronic | ttbb_SemiLeptonic | ttbb_2L2Nu |
|---|---|---|---|
| | 113 736 166.15 | 153 797 754.09 | 15 930 889.90 |

`weight = σ_eff × lumi / Σgenw` — σ_eff 가정에 따라:

| 가정 | σ_total | weight_Had | weight_SL | weight_DL |
|---|---|---|---|---|
| **A** σ=1.452/BR_HAD (현 compute_stitch_factors.py) | 3.1953 pb | 5.2955e-4 | **3.7866e-4** | **8.8366e-4** |
| **B** σ=43.74 pb (★ttH AN §6.2.4 L960-962, LHE) | 43.74 pb | 7.2488e-3 | 5.1833e-3 | 1.2096e-2 |
| **C** σ=1.452×BR (현 main.yml 값과 일치) | 0.6598 pb | 2.4063e-4 | 1.7207e-4 | 4.0154e-4 |

### 발견된 두 가지 불일치
1. **현 main.yml ttbb 3종은 가정 C** — 즉 σ_eff=1.452×BR. 1.452 pb는
   compute_stitch_factors.py 주석상 "hadronic 4FS" 단면적인데, 거기에 BR을
   **한 번 더** 곱한 값이 weight에 들어가 있다 (BR 이중 적용 가능성).
   *주: 메모리상 "placeholder 1.0"으로 알려진 SL/DL weight는 실제로는 이미
   가정 C 값(1.7207e-4, 4.0154e-4)이 채워져 있었다. prescan의 xsec_used=1.0은
   prescan 단계 입력일 뿐, main.yml에는 값이 있다.*
2. **AN 값은 43.74 pb** (가정 B). 1.452 pb 와 30배 차이. ttH AN-19-094
   §6.2.4: "a total cross section of 43.74 pb for the ttbb sample, taken from
   the LHE event information of the MC sample". 같은 표(Table 61)는 ttbb(4FS)의
   tt+bb 정규화를 4.56 pb, tt+B를 21.34 pb로 인용.

### 권고 (사용자 결정 필요 — an_usage_rules #2: 임의 수정 금지)
- 1.452 pb 의 출처/의미를 확정해야 한다. AN의 43.74 pb (LHE total)와의 관계:
  43.74 가 4FS ttbb의 inclusive total이고, 1.452 가 어떤 phase-space/필터 적용
  후 값인지 (또는 단순 오기인지) 확인 필요.
- stitching r_B 는 σ_inc/σ_dedicated **비율**로 들어가므로, σ_dedicated 절대값이
  바뀌면 r_B 와 per-event weight가 함께 바뀐다. → weight 확정 전 stitch JSON
  재유도 보류.
- 사용자가 세 가정의 weight를 직접 비교 후 결정하면, main.yml ttbb 3종 +
  compute_stitch_factors.py 의 σ 상수를 일괄 갱신한다 (1파일+1파일 diff).

## B. single-muon offline 옵션 (구현 완료)
별도 모드 대신 main/debug에서 env `TTHH_REQUIRE_1MUON=1` 로 켜는 옵션:
- 켜지면 `_policy.collectLeptons=true`, `requireLeadMuonOnly=true` 강제(muon 수집),
  그리고 cut 테이블의 **lepton veto step만** 런타임에 "정확히 muon 1 + electron 0"
  으로 치환. **그 외 selection·weight·SF는 main과 완전히 동일** (추가 SF 없음).
- 목적: AN trigger 측정 영역(1μ + FH baseline)을 offline에서 흉내 (사용자 지시).
- 구현: capture-less 테이블 람다는 상태를 못 가지므로 루프에서
  `if (_require1Muon && c.step==kLeptonVeto)` 분기. 플래그 off면 기존과 100% 동일.

### 검증
- 모드 블록 발췌 재컴파일 OK, brace 불변, `_require1Muon` 배선 4곳.
- off(기본)일 때 테이블 루프 동작은 Step 3 semantics 시뮬레이션과 동일(분기 미발동).

### 로컬 검증
```bash
make
# 1-muon 영역:
TTHH_REQUIRE_1MUON=1 ./ttHHanalyzer_unified --mode main --filelist <list> \
    --output mu1.root --weight <w> --year 2017 --dataOrMC MC --sample TTToHadronic
#  → 시작 로그 "[muon-val] ... '정확히 muon 1 + electron 0'" 확인
#  → cutflow의 lepton-veto 단계가 1μ 요구로 바뀐 yield인지 확인
# 옵션 off(기본): 기존 main과 hCutFlow 바이너리 동일해야
```

## 롤백
```bash
# B만 되돌리려면 .h의 [muon-val] 멤버/생성자 블록 + .cc 루프 분기 제거
```
