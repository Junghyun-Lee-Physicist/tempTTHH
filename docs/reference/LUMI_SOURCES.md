# Reference — 적분 루미노시티(lumi) 가 사는 모든 곳

> **목적**: lumi 정본 값과, 값을 바꿀 때 **어디를 다 고쳐야 하는지**의 단일 목록. 2017/2018 모두.
> **대상 독자**: lumi 를 확정·변경하려는 사람(사람·AI).
> **상태**: ACTIVE · 작성 2026-07-26 · **값 확정 2026-07-27**(LUM POG TWiki 원문 대조). 코드 반영은 부분 완료 — 아래 §3 표의 "상태" 열 참조.
> **관련**: 가중치 공식은 `data/samples_<era>UL.json._meta.weight_formula`.

## 결론 먼저 (BLUF)

**정본은 LUM POG 의 "Recorded Golden Legacy" 값이다: 2017 = 42.07 fb⁻¹, 2018 = 59.56 fb⁻¹.**

- **2018 은 59.83 이 아니다.** 59.83 은 잠정값으로 들고 있던 숫자이며 LUM POG 권고 페이지에
  **아예 등장하지 않는다**. 2026-07-27 에 원문 표를 대조해 **59.56 으로 정정**했다.
- **2017 = 42.07 은 확정.** 우리가 직접 돌린 brilcalc 결과 **42.0688** 과 소수 둘째자리까지 일치한다.
- 따라서 `AnalyzerConfig/Tier3_2017_FH_unified_{main,btagtrig}.yml` 의 **41.48 은 확정적으로 틀렸다**
  (42.07/41.48 = **1.0142** → 전 MC weight 가 1.42% 어긋남). 고치면 **main·btagtrig 재생산이 필요**하므로
  아직 바꾸지 않았다 — §4 체크리스트대로 한 번에 처리할 것. **이것이 유일하게 남은 물리 영향 항목이다.**

## 1. 출처 (references)

| | |
|---|---|
| LUM POG 문서 index | <https://twiki.cern.ch/twiki/bin/viewauth/CMS/TWikiLUM> |
| Run 2 권고 페이지 (정본 표) | <https://twiki.cern.ch/twiki/bin/view/CMS/LumiRecommendationsRun2> |
| 논문 인용 | **CMS-PAS-LUM-20-001** (2017–2018 및 조합). 2015–2016 은 CMS-LUM-17-003 |
| 최신 상관/조합 연구 | <https://indico.cern.ch/event/1617597/> |

권고 페이지가 지시하는 계산 명령(분석 고유 JSON 으로 **직접** 돌릴 것):

```bash
brilcalc lumi --normtag /cvmfs/cms-bril.cern.ch/cms-lumi-pog/Normtags/normtag_PHYSICS.json \
              -u /fb -i [YOUR_ANALYSIS_JSON]
```

> TWiki 원문 주의사항: *"Remember the lumi values for your analysis normalization need to be
> evaluated depending on your analysis jobs and triggers using the command above."*
> 즉 아래 표의 값은 **full dataset** 값이고, 실제 정규화는 우리 분석의 certified JSON +
> trigger 선택으로 다시 계산해야 한다. 2017 은 했고(42.0688), **2018 은 아직 안 했다.**

## 2. LUM POG 표 (Run 2 pp 13 TeV, normtag_PHYSICS) — 우리에게 필요한 부분

| 행 | 2017 | 2018 |
|---|---|---|
| Delivered [1/fb] | 50.51 | 67.56 |
| Recorded DCSOnly [1/fb] | 45.21 | 62.20 |
| Recorded Golden **PreLegacy** [1/fb] | 42.12 | 59.47 |
| **Recorded Golden Legacy [1/fb]** ← **우리가 쓰는 값** | **42.07** | **59.56** |
| Uncertainty [%] | **0.82** | **0.84** |

- **왜 Legacy Golden 인가**: 우리 샘플은 UltraLegacy(NanoAODv9 / MiniAODv2) 재처리이므로
  Legacy certification 이 맞는 짝이다. PreLegacy 값(42.12 / 59.47)을 쓰면 안 된다.
- Golden JSON (Legacy):
  - 2017 `Collisions17/13TeV/Legacy_2017/Cert_294927-306462_13TeV_UL2017_Collisions17_GoldenJSON.txt`
  - 2018 `Collisions18/13TeV/Legacy_2018/Cert_314472-325175_13TeV_Legacy2018_Collisions18_JSON.txt`
  - analyzer 는 `CorrectionsManager.cc:248-250` 에서 이 파일명을 조립하는데 **run range 가 2017 로
    하드코딩**되어 있다 → 2018 Data 는 전부 **E41**. (연도 의존 감사 P0 #5.)

### combine 조직값 (단일 연도 fit)

TWiki 의 새 권고(Cholesky minimal decomposition). 단일 연도 fit 은 **이름에 그 연도가 들어간
nuisance 를 모두** 쓴다:

| 연도 | nuisance |
|---|---|
| 2017 | `lumi_13TeV_151617_l` = 1.0055, `lumi_13TeV_15161718_l` = 1.0061 |
| 2018 | `lumi_13TeV_15161718_l` = 1.0084 |

(구 `lumi_1…lumi_7` 7-parameter 방식은 TWiki 가 **discouraged** 로 표시.)

## 3. lumi 가 실제로 박혀 있는 모든 지점

`submit_job_FH_Tier3_unified.py:614` → `lumi = float(common.get("lumi_fb_inv", meta.get("lumi_fb_inv")))`
— **yml 이 우선**, `xsec_db._meta` 는 fallback 이다. 그래서 #1–#3 이 물리를 바꾸는 유일한 지점이고
나머지는 표기다. `--preflight` 가 yml↔`_meta` 불일치를 **WARN** 으로 보고한다(`:332-336`).

| # | 파일:위치 | 현재 값 | 정본이면 | 상태 |
|---|---|---|---|---|
| 1 | `AnalyzerConfig/Tier3_2017_FH_unified_main.yml:13` | **41.48** | 42.07 | ❌ **미반영 (물리 영향, 재생산 필요)** |
| 2 | `AnalyzerConfig/Tier3_2017_FH_unified_btagtrig.yml:13` | **41.48** | 42.07 | ❌ **미반영 (물리 영향, 재생산 필요)** |
| 3 | `AnalyzerConfig/Tier3_2017_FH_unified_prescan.yml:15` | 41.48 | 42.07 | ❌ 미반영 (prescan 은 weight 미사용 → 영향 없음, 일관성용) |
| 4 | `data/samples_2017UL.json._meta.lumi_fb_inv` | 42.07 | 42.07 | ✅ |
| 4b | 같은 파일 `lumi_full_golden_legacy_fb_inv` / `lumi_brilcalc_result_fb_inv` / `lumi_uncertainty_percent` | 42.07 / 42.0688 / 0.82 | 동일 | ✅ (+`lumi_golden_prelegacy_fb_inv` 42.12, index URL 추가) |
| 5 | `data/samples_2018UL.json._meta.lumi_fb_inv` | **59.56** | 59.56 | ✅ **2026-07-27 정정 (was 59.83)** |
| 5b | 같은 파일 lumi_* 블록 | 0.84% / PreLegacy 59.47 / citation / golden JSON / nuisance | — | ✅ 신설 (2017 과 같은 스키마) |
| 6 | `plotter/stack_plotter.C:639` `double LUMI = 41.48;` | 41.48 | 42.07 (+2018 분기) | ❌ 하드코딩, 연도 일반화 필요 |
| 7 | `bTagSF_ReweightStudy/plot_btag_pyroot.py:106` `"41.5 fb^{-1}"` | **41.5** | 42.07 | ❌ 하드코딩(세 번째 값) |
| 8 | `ttHH_beamer_v7.2_overleaf/config.tex:31` `\LumiText{41.48~fb^{-1}}` | 41.48 | 42.07 | ❌ 하드코딩 |
| 9 | `ttHH_beamer_v7.2_overleaf/slides/12_backup_samples.tex` 각주 | 42.07 / **59.56** | 동일 | ✅ #4·#5 에서 **자동 생성** — 직접 수정 금지 |

**#5 를 고칠 때 같이 고쳐야 하는 생성기**: `NtupleForge/script/build_ul18_from_log.py:78-…` 의
`_meta` 리터럴. 이걸 안 고치면 다음 재생성에서 값이 되돌아간다 — 2026-07-27 에 함께 반영하고
**재생성 후 byte-identical 확인**했다.

### 남아 있는 모순 (2026-07-27 현재)

1. **weight(41.48) ≠ 표기(42.07)** — 2017 수율을 발표할 때 설명이 안 된다. **#1–#3 이 유일한 원인.**
2. 같은 PDF 안에서 본문 41.48(#8) vs backup 42.07(#9).
3. 세 번째 값 41.5(#7)가 b-tag study plot 에만 존재.

### 2018 에 아직 없는 것

| 대상 | 상태 |
|---|---|
| `AnalyzerConfig/Tier3_2018_FH_unified_*.yml` (#1–3 의 2018판) | **미생성**. 생성 스니펫은 `RUNBOOK_UL18_to_controlplots.md` §5·§6·§7 — 그 스니펫이 **59.56** 을 쓴다 |
| `compute_stitch_factors.py:104` `LUMI_PB_INV` | 2017 값 41480.0 하드코딩. 2018 은 **59560.0** (RUNBOOK §5 스니펫) |
| 2018 brilcalc 실측 | **미실행** → `_meta.lumi_brilcalc_result_fb_inv: null` |
| `plotter`/`bTagSF` /beamer 의 2018 분기 | 미생성 (#6·#7·#8 이 2017 문자열 1개씩만 가짐) |

## 4. 값을 바꿀 때의 변경 절차 (체크리스트)

정본과 근거는 이제 §1·§2 에 있으므로, 남은 일은 **#1–#3 + #6–#8 반영**이다.

1. **#1–#3** yml 3개 (연도별) — 이게 물리 결과를 바꾸는 유일한 지점.
   ```bash
   cd tempTTHH
   sed -i 's/^\(\s*lumi_fb_inv:\s*\)41\.48/\142.07/' \
       AnalyzerConfig/Tier3_2017_FH_unified_{main,btagtrig,prescan}.yml
   grep -n lumi_fb_inv AnalyzerConfig/Tier3_2017_FH_unified_*.yml
   ```
2. **#6 / #7 / #8** plot label · beamer 하드코딩.
3. **#9 재생성** (직접 편집 금지):
   ```bash
   cd ttHH_beamer_v7.2_overleaf
   python3 tools/make_sample_tables.py \
       --json ../tempTTHH/data/samples_2017UL.json --era 2017UL \
       --json ../tempTTHH/data/samples_2018UL.json --era 2018UL \
       --out slides/12_backup_samples.tex && make
   ```
   `_meta.lumi_ref` 에 "OPEN"/"preliminary" 문자열이 있으면 각주에 **(PRELIMINARY)** 가 자동으로
   붙는다. 2018 의 `lumi_ref` 는 2026-07-27 에 그 문구가 사라졌으므로 **재생성하면 2018 각주의
   (PRELIMINARY) 마커도 없어진다** — 아직 재생성하지 않았다면 표에는 여전히 59.83+PRELIMINARY 가
   인쇄되어 있다.
4. **재생산 필요 여부**: `main`/`btagtrig` 산출물은 weight 가 바뀌므로 **재생산 대상**.
   `prescan` 은 lumi 를 쓰지 않으므로 무관(`Σgenw` 만 누산).
   b-tag norm reweight JSON 과 stitch factor 는 weight 기반이므로 **함께 재유도**.
5. **검증**: `python3 submit_job_FH_Tier3_unified.py --mode main --preflight` 가
   `lumi consistency` 를 PASS 로 보고하는지 확인(WARN 이면 아직 불일치).

## 5. 권장 (미실행, PROPOSED)

- **하드코딩(#6·#7·#8) 제거**: plot label 과 beamer 매크로가
  `samples_<era>UL.json._meta.lumi_fb_inv` 를 읽게 하면 lumi 정본이 1곳(JSON)+사용처 1곳(yml)로
  줄어든다. 지금은 9곳이라 한 곳만 고치면 조용히 모순이 남는다.
- 궁극적으로는 **yml 이 `_meta` 를 참조**하도록 해서 정본을 JSON 하나로 만드는 것이 맞다
  (현재는 yml 이 우선이라 JSON 이 정본 역할을 못 한다).
- **2018 brilcalc 실측**: 우리 certified JSON + trigger 선택으로 돌려
  `_meta.lumi_brilcalc_result_fb_inv` 를 채운다. 2017 에서 42.07 vs 42.0688 처럼 소수 둘째자리
  일치를 확인하는 것이 목적.
