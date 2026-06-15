# STEP 14 — 3-tier weight + lepton CR(MET cut) + prescan WARN 수정 + 정규화 검증

- 날짜: 2026-06-15
- 요구: (1) prescan WARN 거짓경고 수정, (2) main에 trigSF만/+btagShape/+normRW
  3가지 weight를 동시 출력, (3) QCD 억제용 1ℓ+MET CR 추가(electron/muon),
  (4) 정규화 요소(σ·BR·N·Σgenw·k=1)가 제대로 합성되는지 검증
- 변경: `ttHHanalyzer_unified.cc`, `ttHHanalyzer_unified.h`, `src/tnm.cc`, `include/tnm.h`, `submit_job_FH_Tier3_unified.py`

## 1. prescan WARN 거짓경고 수정
tt+nb(61/62/71/72)를 53/54/55 에서 빼내 별도 빈으로 보내므로, ntuple
`ttCat_Add2Bjet`(≥2 add-b 전체)과 비교하려면 tt+nb 빈을 다시 더해야 한다.
이전: `sum_2b_or_more = id_53+54+55` → 항상 tt+nb 만큼 어긋나 거짓 WARN.
수정: `+ id_61+62+71+72`. (consolidate_prescan 은 이미 올바르게 비교했음;
analyzer 내부 WARN 만 낡았던 것.)

## 2. 3-tier weight (b-tag SF/reweight 영향 비교)
**사용자 결정**: b-tag SF/reweight 는 yield 를 20-30% 바꿀 뿐 2배는 아니므로,
1차 stack plot 은 trigSF 만으로. 단 셋을 동시 출력해 비교 가능하게:

| branch | 구성 |
|---|---|
| `evtWeight` | base × PU × L1 × genW × stitch × **trigSF** (b-tag SF/RW 없음) ← 기본 |
| `evtWeight_btagSF` | evtWeight × btagShape SF |
| `evtWeight_full` | evtWeight_btagSF × btagNormReweight |

SF 체인 재구성: 기존엔 `_evtWeight`에 셋 다 곱했으나, 이제 `_evtWeight`는
trigSF 만; btagShape/normRW 는 `_evtWeight_chain_btagSF/full` 에만. 세 tier 를
output tree 의 3 branch 로 기록. stack plotter 가 골라 쓴다.
- norm reweight 는 `TTHH_SKIP_BTAGRW=1` 시 1.0 (8그룹 JSON 전까지). full tier
  에만 영향, 기본 evtWeight 엔 무관.

## 3. lepton CR (QCD 억제) — CLI --region (env 아님)
**사용자 결정**: env 대신 CLI 인자로. condor 전파 문제 없고 명시적.
3가지 selection 영역 (lepton veto step 런타임 치환, SF 추가 없음):
- `--region ""`(기본) : FH (lepton veto = 0 lepton)
- `--region muon`     : 정확히 muon 1 + electron 0 + **MET_pt > 20 GeV**
- `--region electron` : 정확히 electron 1 + muon 0 + **MET_pt > 20 GeV**

QCD multijet 은 진짜 lepton·MET 가 거의 없어 1ℓ+MET 요구로 크게 준다.
`metCutCR = 20 GeV` 상수. main/debug 에서만 활성. MET 는 `_ev->MET_pt`.

배선: tnm.cc `--region` 파싱 → `commandLine::region` → main() 에서
`analysis.setRegion(cl.region)` → `_lepCRmode` → 루프의 lepton veto step 치환.
submitter: `--region {muon,electron}` 인자 또는 yml `common.region`.
```bash
# 로컬
./ttHHanalyzer_unified --mode main ... --region muon       # muon CR
./ttHHanalyzer_unified --mode main ...                      # 기본 FH
# condor (submitter 인자 또는 yml common.region)
python3 submit_job_FH_Tier3_unified.py --mode main --region muon --files-per-job 5
python3 submit_job_FH_Tier3_unified.py --mode main --region electron --files-per-job 5
python3 submit_job_FH_Tier3_unified.py --mode main --files-per-job 5   # 기본 FH
```

## 4. 정규화 요소 검증 (사용자 요구)
```
MC:   _evtWeight = baseWeight × PU × L1 × genW × stitch × trigSF
Data: _evtWeight = baseWeight(=1) × stitch(=1) × trigSF   (PU/L1/genW 미적용)
baseWeight (submitter 합성, --weight) = lumi_fb × σ_fb × br × kfactor / Σgenw
```
- **공통 정규화(σ·BR·N·Σgenw·k=1)는 baseWeight 하나로 합성** → CLI 로 주입.
- **genWeight 는 per-event** 로 analyzer 가 곱함(MC만). k=1.0 이라 곱해도 무영향.
- Data 는 σ=null → weight=1, genW/PU/L1 미적용.
- 검증: 새 prescan Σgenw + fb xsec_db 로 base weight 재계산 — ttHH 1.07e-6,
  TTToHadronic 2.14e-4 등 합리적. dedicated 는 base×stitch_r 로 yield 보존
  (ttbb_Had 5.77e-4, tt4b 3.57e-4) — compute_stitch_factors 와 일치.

## 검증
- brace balance cc/h = 1/1 (기준). 3-tier·lepCR·WARN 배선 확인.
- SF 체인: `_evtWeight *= triggerSF_` 만, btagShape/normRW 는 chain 전용.
- 정규화: base = lumi×σ×br×k/Σgenw, genW per-event, Data=1 — 손계산 일치.

## 로컬 검증
```bash
make -j4
# 기본 FH (trigSF만)
./ttHHanalyzer_unified --mode main --filelist <l> --output o.root \
    --weight <w> --year 2017 --dataOrMC MC --sample TTToHadronic
# muon CR (QCD 억제)
TTHH_LEPCR=muon ./ttHHanalyzer_unified --mode main ... 
# tree 에 evtWeight / evtWeight_btagSF / evtWeight_full 3 branch 확인
```

## 롤백
이 STEP 의 편집(3-tier, lepCR, WARN)을 .cc/.h 에서 제거. b-tag SF 를
evtWeight 에 다시 곱하려면 SF 체인의 `_evtWeight *= bTagWeight_central_`/
`*= btagNormReweight_` 복원.
