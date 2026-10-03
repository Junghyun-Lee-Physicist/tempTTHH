# Reference — 적분 루미노시티(lumi) 가 사는 모든 곳

> **목적**: lumi 정본 값과, 값을 바꿀 때 **어디를 다 고쳐야 하는지**의 단일 목록. 2017/2018 모두.
> **대상 독자**: lumi 를 확정·변경하려는 사람(사람·AI).
> **상태**: ACTIVE · 작성 2026-07-26 · **값 확정 2026-07-27**(LUM POG TWiki 원문 대조) · **2017 반영 2026-10-02**(사용자 결정 N1, [`../DECISIONS.md`](../DECISIONS.md) D-2026-10-02-D): §3 의 #1–#3·#6·#7 을 42.07 로. 남은 것은 #8(다른 저장소) — §3 표의 "상태" 열 참조. · **2024 brilcalc 실측 2026-10-02**(§6).
> **관련**: 가중치 공식은 `data/samples_<era>UL.json._meta.weight_formula`.

## 결론 먼저 (BLUF)

**정본은 LUM POG 의 "Recorded Golden Legacy" 값이다: 2017 = 42.07 fb⁻¹, 2018 = 59.56 fb⁻¹.**

- **2018 은 59.83 이 아니다.** 59.83 은 잠정값으로 들고 있던 숫자이며 LUM POG 권고 페이지에
  **아예 등장하지 않는다**. 2026-07-27 에 원문 표를 대조해 **59.56 으로 정정**했다.
- **2017 = 42.07 은 확정.** 우리가 직접 돌린 brilcalc 결과 **42.0688** 과 소수 둘째자리까지 일치한다.
- 따라서 `AnalyzerConfig/Tier3_2017_FH_unified_{main,btagtrig}.yml` 의 **41.48 은 확정적으로 틀렸다**
  (42.07/41.48 = **1.0142** → 전 MC weight 가 1.42% 어긋남). btagtrig 는 07-29 에, main·prescan 과 plot 표기
  (#6·#7)는 **2026-10-02 에 42.07 로 고쳤다**(사용자 결정 N1). 41.48 로 만든 2017 산출물은 없다(이 코드로 2017
  사슬을 돌린 적 없음, 사용자 답 N0) → 재생산할 것도 없다. 남은 것은 beamer(#8, 다른 저장소)다.
- **2024 (Run 3)**: golden JSON 은 2024B 도 덮지만 우리 Data 는 C–I 뿐이다. 그래서 정규화 lumi 는 C–I 의 brilcalc 값이고
  PdmV 표의 109.95(B 포함)가 아니다. **2026-10-02 실측(§6): C–I = 109.816 fb⁻¹ (normtag_PHYSICS)**. PdmV 표의 era 값은
  normtag_PHYSICS 와 소수 둘째 자리까지 같고, normtag_BRIL 은 104.504(−4.8 %)다. 분석 trigger 의 유효 lumi 는 그보다 0.64 % 작게
  나온다(§6.3) — 어느 쪽으로 정규화할지는 [`../PLAN_v15_2018UL_2024.md`](../PLAN_v15_2018UL_2024.md) §9.6 D14.

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
| 1 | `AnalyzerConfig/Tier3_2017_FH_unified_main.yml:16` | 42.07 | 42.07 | ✅ **2026-10-02** (was 41.48; 그 값으로 만든 산출물 없음) |
| 2 | `AnalyzerConfig/Tier3_2017_FH_unified_btagtrig.yml:20` | 42.07 | 42.07 | ✅ 2026-07-29 |
| 3 | `AnalyzerConfig/Tier3_2017_FH_unified_prescan.yml:18` | 42.07 | 42.07 | ✅ 2026-10-02 (prescan 은 weight 미사용 → 영향 없음, 일관성용) |
| 4 | `data/samples_2017UL.json._meta.lumi_fb_inv` | 42.07 | 42.07 | ✅ |
| 4b | 같은 파일 `lumi_full_golden_legacy_fb_inv` / `lumi_brilcalc_result_fb_inv` / `lumi_uncertainty_percent` | 42.07 / 42.0688 / 0.82 | 동일 | ✅ (+`lumi_golden_prelegacy_fb_inv` 42.12, index URL 추가) |
| 5 | `data/samples_2018UL.json._meta.lumi_fb_inv` | **59.56** | 59.56 | ✅ **2026-07-27 정정 (was 59.83)** |
| 5b | 같은 파일 lumi_* 블록 | 0.84% / PreLegacy 59.47 / citation / golden JSON / nuisance | — | ✅ 신설 (2017 과 같은 스키마) |
| 6 | `plotter/stack_plotter.C:639` `double LUMI = 42.07;` | 42.07 | 42.07 (+연도 분기) | ✅ 2026-10-02 값; 여전히 하드코딩이라 연도 일반화는 PLAN §9 Stage 8 |
| 7 | `bTagSF_ReweightStudy/plot_btag_pyroot.py:106`, `plot_btag.py:96` `"42.1 fb^{-1}"` | 42.1 | 42.07 | ✅ 2026-10-02 (was 41.5; 표기만) |
| 8 | `ttHH_beamer_v7.2_overleaf/config.tex:31` `\LumiText{41.48~fb^{-1}}` | 41.48 | 42.07 | ❌ 하드코딩 |
| 9 | `ttHH_beamer_v7.2_overleaf/slides/12_backup_samples.tex` 각주 | 42.07 / **59.56** | 동일 | ✅ #4·#5 에서 **자동 생성** — 직접 수정 금지 |

**#5 를 고칠 때 같이 고쳐야 하는 생성기**: `NtupleForge/script/build_ul18_from_log.py:78-…` 의
`_meta` 리터럴. 이걸 안 고치면 다음 재생성에서 값이 되돌아간다 — 2026-07-27 에 함께 반영하고
**재생성 후 byte-identical 확인**했다.

### 남아 있는 모순 (2026-10-02 갱신)

1. ~~weight(41.48) ≠ 표기(42.07)~~ — 2026-10-02 에 #1–#3 을 고쳐 해소(그 전 값으로 만든 2017 산출물 없음).
2. 같은 PDF 안에서 본문 41.48(#8) vs backup 42.07(#9). **남음** (beamer 저장소).
3. ~~세 번째 값 41.5(#7)~~ — 2026-10-02 해소.

### 2018 에 아직 없는 것

| 대상 | 상태 |
|---|---|
| `AnalyzerConfig/Tier3_2018_FH_unified_*.yml` (#1–3 의 2018판) | **미생성**. 생성 스니펫은 `RUNBOOK_UL18_to_controlplots.md` §5·§6·§7 — 그 스니펫이 **59.56** 을 쓴다 |
| `compute_stitch_factors.py:106` `LUMI_PB_INV` | 2017 값 42070.0 하드코딩(2026-10-02, was 41480.0; 진단용 `peF_full` 에만 쓰이고 stitch 배수 r 에서는 상쇄). 2018 은 **59560.0** (RUNBOOK §5 스니펫) |
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

## 6. 2024 (Run 3): brilcalc 실측 (2026-10-02)

> **어떻게:** NtupleForge `script/lumi_hlt_check.sh`(기본값 그대로; workspace RUNBOOK §20 4), lxplus955, brilcalc 3.9.4
> (LUM POG `brilws-docker` env), 1,015 s, `EXIT : 0`. 로그는 사용자 결정(10-02)으로 public 인 NtupleForge 에 커밋하지 않고
> lxplus 의 `script/runlogs/nocommit/` 에 둔다. 아래 숫자는 사용자가 붙인 출력 그대로다.

### 6.1 golden JSON

| | 값 |
|---|---|
| 파일 | `/eos/user/c/cmsdqm/www/CAF/certification/Collisions24/Cert_Collisions2024_378981_386951_Golden.json` |
| 판 | **2026-08-04** 판(46,348 byte, md5 `3f8543e8062915c9de97472e7dcf744f`). 그 전 판(2024-12-19, 44,548 byte)은 같은 디렉터리에 `..._Golden_before_TrkML2026_review.json` 으로 남아 있다 → 이름만 보고 옛 사본을 쓰지 않도록 analyzer 의 `GoldenJson/` 사본은 md5 로 확인한다 |
| 내용 | run 475 개(378985–386951), LS 287,601 |
| 생산과의 관계 | NtupleForge 의 2024 Data CRAB 생산은 lumimask 를 쓰지 않았다(config·`submit_crab.py` 에 없음) → 판이 바뀌어도 재생산은 필요 없고 analyzer 에서 이 판을 적용하면 된다 |

### 6.2 recorded lumi [fb⁻¹]

| 범위 | run | normtag_PHYSICS | normtag_BRIL | BRIL/PHYSICS |
|---|---|---|---|---|
| JSON 전체 (B–I) | 378985–386951 | 109.947 | 104.635 | 0.952 |
| **C–I (우리 Data)** | 379412–387121 | **109.816** | 104.504 | 0.952 |
| C | 379412–380252 | 7.261 | 7.248 | 0.998 |
| D | 380253–380947 | 7.976 | 7.914 | 0.992 |
| E | 380948–381943 | 11.423 | 11.257 | 0.985 |
| F | 381944–383779 | 28.038 | 25.385 | 0.905 |
| G | 383780–385813 | 38.069 | 36.336 | 0.954 |
| H | 385814–386408 | 5.490 | 5.282 | 0.962 |
| I | 386409–387121 | 11.558 | 11.083 | 0.959 |

- era 일곱의 합 = C–I 값(PHYSICS 109.8156 = 109.8156). PdmV 표(C 7.26, D 7.98, E 11.42, F 28.04, G 38.07, H 5.49, I 11.56, 전체 109.95)는
  **normtag_PHYSICS 와 같다** → PdmV 가 PHYSICS 를 썼다. normtag_BRIL(2026-06-14 판)은 F 에서 9.5 %, G–I 에서 3.8–4.6 %, C–E 에서 0.2–1.5 % 작다. 어느 normtag 가 권고인지는
  LUM POG 의 Run 3 권고 페이지로 확인할 것(이 세션에서는 열 수 없다; 그 전까지 PHYSICS 를 쓴다, §9.6 D14).
- brilcalc 경고: 12 개 run(379774, 379866, 381113, 381309, 381417, 381443, 382568, 384579, 384614, 384981, 385012, 385618)에서 JSON 의 LS
  범위가 normtag 범위보다 넓다(run 의 처음·끝 몇 LS). 그 LS 는 lumi 가 없다: JSON 전체에서 287,601 − 287,524 = 77 LS (0.03 %).
  분석에서 이 LS 의 event 를 버릴지는 영향이 작아 두지 않는다(기록만).

### 6.3 HLT 경로의 유효 lumi (normtag_BRIL, C–I; brilcalc 는 경로를 판마다 한 줄로 내고 스크립트가 판을 더함)

| 경로 | 판 | 합 [fb⁻¹] | / C–I 합 104.504 |
|---|---|---|---|
| `HLT_PFHT1050` | 5 | 103.835 | 0.9936 |
| `HLT_PFHT450_SixPFJet36_PNetBTag0p35` | 5 | 103.835 | 0.9936 |
| `HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50` | 5 | 103.835 | 0.9936 |
| `HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3` (`_2p0` 도 같음) | 4 | 97.481 | 0.9328 |
| `HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5` | 1 (v8) | 6.354 | 0.0608 |
| `HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p55` (`0p60`, `QuadPFJet35_..._0p60` 도 같음) | 5 | 103.835 | 0.9936 |
| `HLT_PFHT340_QuadPFJet70_50_40_40_PNet2BTagMean0p70` | 5 | 103.835 | 0.9936 |
| `HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_4p3` (`_5p6` 도 같음) | 5 | 103.835 | 0.9936 |
| `HLT_IsoMu24`, `HLT_IsoMu27` | 5 | 103.848 | 0.9937 |
| `HLT_Ele30_WPTight_Gsf` | 3 | 103.835 | 0.9936 |
| b-tag 없는 판(`HLT_PFHT400_SixPFJet32`, `HLT_PFHT450_SixPFJet36`, `HLT_PFHT330PT30_QuadPFJet_75_60_45_40`) | 5 | 1.598, 4.153, 0.811 | 0.015, 0.040, 0.008 (prescale) |

- **prescale 이 없는 경로는 모두 0.9936(IsoMu24 는 0.9937)에서 같다.** 1 이 아닌 이유는 경로가 아니라 LS 쪽이다: C–I 의 281,562 LS 가운데
  어느 경로에도 1,907 LS(0.68 %)의 HLT 기록이 없다(경로마다 ncms 합 279,655). 그 LS 가 data 에 event 가 없는 LS 인지 lumi DB 의 기록만
  빠진 것인지를 run 목록으로 본다(workspace RUNBOOK §20 4b).
- **4J3T 의 짝은 기간에 따라 다른 경로다**: 처음 기간(HLT 판 하나, 38 run, BRIL 6.354 fb⁻¹, 대부분 era C)은 `..._TriplePFBTagDeepJet_4p5_v8`,
  그 뒤 전부(97.481)는 `..._PNet3BTag_4p3`. 둘을 더하면 103.835 = prescale 없는 경로와 같다 → 둘을 OR 하면 C–I 전체를 덮는다(§9.6 D4).

### 6.4 normtag_PHYSICS 의 유효 lumi 와 HLT 기록이 없는 run (2026-10-02, workspace RUNBOOK §20 4b)

> lxplus955, `lumi_hlt_check.sh --normtag normtag_PHYSICS.json --eras "CI:379412:387121" --paths "HLT_PFHT1050_v* HLT_IsoMu24_v*"`, 303 s,
> `EXIT : 0`; 이어서 그 work 디렉터리의 run 별 출력을 awk 로 비교. 숫자는 사용자가 붙인 출력 그대로다(로그는 lxplus `nocommit/`).

| 경로 | 판 | recorded [fb⁻¹] | / C–I 109.8156 |
|---|---|---|---|
| `HLT_PFHT1050` | v25–v29 (6.365, 8.219, 11.423, 28.036, 55.101) | **109.1435** | 0.9939 |
| `HLT_IsoMu24` | v20–v24 | **109.1572** | 0.9940 |

HLT 기록이 없는 LS (normtag_PHYSICS, C–I): **4 run, 1,876 LS** — run 별 ncms 와 HLT ncms:

| run | era | ncms | HLT ncms | run 의 recorded [fb⁻¹] |
|---|---|---|---|---|
| 380126 | C | 952 | 0 | 0.38244 |
| 380127 | C | 535 | 0 | 0.17066 |
| 380128 | C | 387 | 0 | 0.10062 |
| 384029 | G | 445 | 443 | 0.18904 (빠진 것은 2 LS) |

- 세 run(380126–380128)은 HLT 표가 통째로 없다: 0.654 fb⁻¹ = C–I 와 IsoMu24 유효 lumi 의 차(0.658)의 거의 전부. 남는 0.004 fb⁻¹ 가운데 384029
  의 2 LS 는 0.001 쯤이고 나머지는 이 출력으로 나눌 수 없다(C–I 의 0.004 %, 무시할 크기). §6.3 의 1,907 LS 는 같은 것을 normtag_BRIL 로 센 수다(normtag 마다 덮는 LS 가 조금 다르다: C–I 의 ncms 가 BRIL 281,562, PHYSICS 281,564).
- `HLT_PFHT1050` 이 `HLT_IsoMu24` 보다 0.014 fb⁻¹ 작은 것은 마지막 판(v29 132,530 대 v24 132,561 LS)의 31 LS 차이다.
- 정규화(PLAN §9.6 D14): 세 run 의 LS 가 우리 `JetMET0/1` 2024C 출력의 `LuminosityBlocks` tree 에 있으면 data 는 있고 lumi DB 의 HLT 표만 없는
  것이므로 C–I 전체 109.816, 없으면 109.157. KNU 확인 명령은 RUNBOOK §20 9.
