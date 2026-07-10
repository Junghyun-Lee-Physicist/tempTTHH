# STEP 19 — 완료판정을 종료 마커(cutflow) 기준으로 교체 + non-tt ttCat 요약 게이트

- 날짜: 2026-07-10
- 증상 1: 저-HT 샘플(WJetsToLNu_HT70To100/HT100To200/HT200To400, QCD_HT200to300 등)의
  main job 들이 정상 완료(exit 0, `=== CutFlow Summary ===` 출력, output ROOT 존재)했는데도
  `--report` 가 대량 missing 으로 보고 (예: WJetsToLNu_HT70To100 399/406 missing).
  `--resubmit` 하면 재실행 후에도 같은 이유로 다시 missing → **영구 재제출 루프**.
- 증상 2: non-tt 샘플의 `[ttCatSummary]` 가 `agree=0 disagree=N agreement=0.0000%` 를
  "~97% 기대" 라벨과 함께 출력 → 버그로 오인 가능.
- 변경: `submit_job_FH_Tier3_unified.py`, `ttHHanalyzer_unified.cc` (진단 출력만)

## 1. (핵심) 완료판정 기준: non-empty TTree → 종료 마커 `cutflow_w_full`

### 원인
STEP_15 가 도입한 non-prescan 완료판정은 "재귀 탐색으로 **entry≥1 인 TTree** 발견"
이었다. 그런데 `fillTree()` 는 **full selection 통과 이벤트만** 기록하므로,
selection 을 아무 이벤트도 통과하지 못한 **정상** job(FH selection 은 HT>500 +
PFHT300~1050 급 트리거를 요구 → 저-HT WJets/QCD 에서 파일 단위로 구조적으로 발생)은
`Tree/Tree` entries=0 → 거짓 incomplete. 이는 STEP_15 §6 이 나열한 거짓 음성
목록에 빠져 있던 케이스이며, 같은 §6 이 "근본 개선(미구현): 정상 종료 마커"라고
적어 둔 바로 그 해법으로 고친다.

report 의 missing 분포가 이 진단과 일치함을 확인했다: missing 은 "파일당 ≥1 개
통과 이벤트가 나올 확률"에 반비례해 저-HT 샘플에 집중 (QCD_HT300to500 63/66
complete vs QCD_HT200to300 16/77). 해당 job 의 .out 에는 CutFlow Summary 까지
정상 출력되어 있었다 (noCut=641, HadTrigger 이후 전부 0 — MC 트리거 경로가 읽는
`HLT_*SixPFJet*`/`HLT_PFHT1050` branch 는 파일에 존재하므로 branch 누락 artifact
가 아니라 진짜 물리적 결과).

### 수정
analyzer `loop()` 의 **마지막** write 는 cutflow 4종
(`cutflow`/`cutflow_w`/`cutflow_w_btagSF`/`cutflow_w_full`) 이며 `writeHistos()`
→ `writeTree()` **이후** 에 기록된다. 따라서 `cutflow_w_full` 키의 존재 자체가
"event loop 완주" 마커다. `_output_is_complete` 의 non-prescan 분기를
"재귀 탐색으로 `cutflow_w_full` 키 발견" 으로 교체 (`_has_nonempty_tree` →
`_has_end_marker`, depth≤4 안전장치 유지, prescan 분기 불변).

이 기준은 기존 기준의 두 오류를 **동시에** 고친다:
- **거짓 missing 해소**: 정당하게 빈 output (Tree entries=0) 도 cutflow 는
  있으므로 complete. analyzer 는 예전부터 cutflow 를 기록해 왔으므로
  **기존 output 에 소급 적용** — 재실행 없이 `--report` 만 다시 돌리면 된다.
- **거짓 complete 해소** (STEP_15 §6 의 고위험 half-written 케이스): 중간에
  죽은 job 은 Tree 에 entry 가 남아도 cutflow 를 끝내 Write 하지 못하므로
  incomplete 로 정확히 재큐된다.

잔여 한계: "cutflow Write 직후 ~ TFile Close 직전" 사이에 죽는 극히 좁은 창은
complete 로 판정될 수 있으나, 이 시점이면 모든 데이터가 이미 기록된 상태라
실무 영향 무시 가능. STEP_15 §6 의 나머지 항목(cmsenv 밖 실행, filelist 변경,
옛 output 잔존)은 그대로 유효하다.

## 2. non-tt 샘플의 ttCatSummary pair 비교 게이트 (진단 출력만; 물리 무관)

### 원인
두 estimator 의 noTT 처리가 비대칭이다:
- `computeTtCategoryFromGenPart` 는 `eventHasTTPair()` 게이트로 non-tt 이벤트를
  전부 sentinel `kNoTTJets` 로 보낸다 (정답).
- `computeTtCategoryFromGenTtbarId` 는 `gtid<0` 에서만 `kNoTTJets` 를 반환하는데,
  NanoAOD 의 ttbarCategorization sequence(GenTtbarCategorizer) 는 **모든 MC** 에서
  돌아 항상 `genTtbarId>=0` 을 채운다. non-tt 이벤트에서는 top decay product 로
  제외할 것이 없어 이벤트의 **모든** HF jet 이 "additional" 로 분류된다
  (0=LF, 41-45=c, 51/52=b). → 디코드는 non-tt 에서 noTT 를 표현할 수 없다.

따라서 non-tt 샘플에서 agreement 0% 는 정의상 결과다. "~97%(HF signal),
~73%(LF-dominated inclusive tt)" 기대치(ttbarCategorization.md §4.1)는
**ttbar family 에만** 해당한다. 실제 WJetsToLNu_HT70To100 의 ANA_GENID
breakdown (LF 530 / c 99 / b 12 = 82.7/15.4/1.9%) 은 W+c 가 큰 W+jets 의
HF 조성으로 합리적 — 로그 자체는 건강하다.

Downstream 안전성: `officialTtCategory()`(=genTtbarId 디코드)를 소비하는
`MakeProcessKey` 는 `IsTtbarFamily(sampleName)` 게이트가 먼저라 non-tt 샘플은
genTtbarId 값과 무관하게 고정 `"tt+LF"` 키를 받고(ttH AN A.2.1), stitch 도
샘플 게이트라 영향 없음. → **물리 결과 무영향, 표기 문제**.

### 수정
`printTtCatSummary()` 의 pair 요약을 `TtCatGroup::IsTtbarFamily(_sampleName)`
으로 분기:
- tt family: 기존 그대로 ("~97% 기대" 라벨 + off-diagonal 나열).
- non-tt: `SKIPPED (non-tt sample)` 헤더 + 사유 2줄 출력, agreement 수치는
  참고용으로 유지하되 off-diagonal 나열은 억제 (`listOff=false`).
헤더는 `ttHHanalyzer_unified.h` 가 이미 include 하는 `Config_TtCatGroup.hh` 사용.
분류/히스토그램/branch 는 일절 불변 — stdout 요약만 변경.

## 검증
- submitter: `python3 -c "ast.parse(...)"` 구문 OK. prescan 분기 불변 확인.
- analyzer: 이 환경에서는 CMSSW 빌드 불가(DECISIONS 의 검증 방식) — brace/paren
  balance 검사 통과. **lxplus/Tier3 에서 `make` 재빌드 필요.**
- 기대 동작: 패치 후 `--report` 재실행 시 저-HT 샘플의 missing 이 "진짜 실패분"
  만 남아야 한다. 교차검증: missing idx 의 condor `.out` 에
  `grep -l "=== CutFlow Summary ==="` (있으면 완주 = 마커 기준으로 complete).

## 롤백
- submitter: `_has_end_marker` 블록을 STEP_15 의 `_has_nonempty_tree` 로 원복.
- analyzer: `printTtCatSummary` 의 if/else 를 원래 무조건 출력으로 원복.
