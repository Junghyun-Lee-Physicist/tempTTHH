# STEP 11 (Track C) — cross section 단일 소스(xsec_db) 도입 + weight 런타임 합성

- 날짜: 2026-06-14
- 요구: σ/N/BR/비고를 JSON 단일 소스로; genWeight는 prescan으로 모으고 Run의
  sumW와 비교하되 사용은 runs.genEventSumw; yml은 샘플 이름만; compute_stitch가
  같은 table을 읽어 파일마다 다른 σ 안 쓰게; The Barn html로 확인
- 신규: `data/samples_2017UL.json`, `data/samples_2018UL.json`(stub),
  `AnalyzerConfig/Tier3_2017_FH_unified_main_TRACKC.yml`, `barn/`(The Barn 안내)
- 변경: `submit_job_FH_Tier3_unified.py`, `compute_stitch_factors.py`
- 백업: `docs/backup_20260611/{submit_job_FH_Tier3_unified.py, compute_stitch_factors.py}`

## 1. 무엇을 / 왜

### 문제 (이전 구조)
σ가 3중으로 흩어지고(`compute_stitch_factors.py` 하드코딩 + main.yml weight에
합성 박제 + prescan) 수동 동기화. 그 결과:
- **ttbb SL/DL이 2.2x(=1/BR_HAD) 과소정규화** — stitch JSON의 r은
  σ_ded=(1.452/BR_HAD)×BR로, main.yml weight는 σ_ded=1.452×BR로 계산되어
  **r과 base의 σ_ded 관례가 불일치**. changelog가 "σ_ded는 약분되니 절대값
  무관, 단 YAML↔config 일치 필요"라 경고했으나 그 일치가 SL/DL에서 깨져 있었음.
  (단일 파일 BR 이중곱이 아니라 **두 파일의 관례 불일치**가 정체.)

### 해결 — xsec_db 단일 소스
```
data/samples_2017UL.json:  샘플별 cross_section_pb(total/inclusive, BR 전) +
                           br(채널 BR) + nevents/nfiles/accuracy/ref/comment
                           (The Barn html이 읽는 그 스키마 + 분석 필요분)
σ_eff = cross_section_pb × br   (BR 한 번 — 이중적용 구조적 불가)
```
- **submitter**: yml은 샘플 이름만(`- TTToHadronic`). weight를 런타임 합성:
  `base = lumi × cross_section_pb × br / sumGenW(runs.genEventSumw)`.
  filelist는 규칙 `filelist_<name>.txt`, data_or_mc는 db의 σ null 여부로 자동.
  yml에 weight/filelist 명시 시 override(하위호환).
- **compute_stitch_factors.py**: σ 상수를 `_sigma_eff()`/`_sigma_total()` 로
  **같은 db에서 읽음**. → r의 σ_ded와 base의 σ_ded가 동일 db에서 나오므로
  **base×r에서 σ_ded 약분 보장** → 과소정규화 원천 해소.
- **prescan**: `runs.genEventSumw` 사용, `events.sumGenW_total`은 비교만
  (합성 시 >0.01% 차이면 경고).

### dedicated base weight 개념 (중요)
dedicated(ttbb) base는 **σ_dedicated**를 쓴다. r=σ_inc·f/σ_ded가 곱해져
최종 base×r = lumi·σ_inc·f/Σgenw 가 되어 σ_ded가 약분된다. 따라서 db의 ttbb
`cross_section_pb=1.452`(inclusive)는 base 계산상 OLD와 같은 값을 주지만(정상),
**r을 같은 db로 계산**하므로 곱이 올바른 정규화로 떨어진다.

## 2. 검증 (VALIDATION — 사용자 핵심 요구)
모든 의문 계산을 트랙 C 최종 코드로 재검증 (전부 통과):

| # | 항목 | 결과 |
|---|---|---|
| V1 | xsec_db σ ↔ ttHH AN Table 9 | TTToHad 831.76 / ttbb 1.452 / tt4b 0.296 / ttHH 0.756 fb **일치** |
| V2 | σ_eff = xsec×br (BR 한 번) | 6샘플 기대값 일치 (TTToHad 377.96 … ttbb_DL 0.1542) |
| V3 | prescan runs vs tree Σgenw | 최대 상대차 4.4e-8 — 일치, 사용은 runs |
| V4 | **base×r 약분** | 3채널 모두 base×r = lumi·σ_inc·f/Σgenw **비트 일치** |
| V5 | 과소정규화 OLD vs NEW | SL/DL OLD 대비 NEW ×2.2007(=1/BR_HAD) — **해소 확인** |
| V6 | owned fraction f | ttbb_Had f=0.004187, tt4b f=9.8e-5 (log와 대조) |

submitter 통합 테스트: 39샘플 bare-string 파싱 + 24 MC weight 합성 + 15 Data
(weight=1) + filelist 규칙 + 자동 data판정 + override 전부 동작.

## 3. ⚠ 운영 순서 (사용자 계획)
1. **prescan 재실행** → 새 `prescan_summary.json` (runs.genEventSumw).
2. **compute_stitch_factors 실행** (자동으로 xsec_db 읽음) → 새
   `stitch_factors_2017.json`. ★ r 값이 바뀜 (Had 1.090→2.40 등) — base도
   함께 바뀌어 곱 보존이므로 정상. db 통일로 SL/DL 과소정규화 해소.
3. **submit** (간소 yml) → weight 자동 합성.
4. The Barn html로 σ 확인 (data/samples_2017UL.json).

## 4. 미확정 (db에 표시, 사용자 확인 필요)
- **ttHH/ttZHto4b/ttZZto4b**: db의 σ가 Table 9 직접값(ttHH 0.756 fb)인데
  현 main.yml weight 역산 σ_eff와 2.88x 차이(ttHH). HH→bbbb BR 추가 적용
  여부 확인 필요 (db comment에 표시, ttZH/ttZZ는 역산값으로 채움).
- 이 샘플들은 stitching 무관(자체 group)이라 base weight 절대값만 영향.

## 5. 롤백
```bash
cp docs/backup_20260611/submit_job_FH_Tier3_unified.py .
cp docs/backup_20260611/compute_stitch_factors.py .
rm -rf data/ barn/ AnalyzerConfig/Tier3_2017_FH_unified_main_TRACKC.yml
```
