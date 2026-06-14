# STEP 7.5 — Trigger SF / b-tag reweight / stack의 카테고리 체계 확정 + 키 매핑 확장

- 날짜: 2026-06-12
- 배경: "SF 적용·reweight 유도·stack을 tt+LF 등 카테고리로 나눠야 하는가" 질문에
  대한 AN 근거 확정 및 구현 (stack은 옵션 (b)로 사용자 결정)
- 변경: `bTagSF_ReweightStudy/include/Config_TtCatGroup.hh` (공유 헤더),
  `src/CorrectionsManager.cc`, `ttHHanalyzer_unified.cc` (주석 1곳)
- 백업: `docs/backup_20260611/bTagSF_ReweightStudy_include/Config_TtCatGroup.hh`

## 0. 결론 (AN 근거)
| 대상 | 카테고리 분리 | 근거 |
|---|---|---|
| b-tag norm reweight | **YES** — HF category가 키, decay channel 병합 | ttH AN §A.2 ("all tt decay channels included"; minor→tt+LF), ttHH AN §D.0.4 (ttbb owned portion만 = stitched 조성 유도), §D.0.5 (**tt4b 별도 norm 유닛** → tt+nb 그룹) |
| trigger SF | **NO** — kinematic binning만 | ttH AN §3.1.1 ((HT, N b-tag, jet6 pT) 함수, process 분류 없음; nbjet 축이 카테고리 차이 흡수; muon 영역 data-limited) |
| stack | **YES** — stitch 후에도 inclusive 안에 LF/cc/ttb 혼재 | ttHH AN §3.2 (7 ttbar category) |

## 1. 발견된 라이브 버그 2건 (이번에 수정)
1. **per-channel ttbb 미스라우트**: 헤더가 `sampleName=="ttbb"`만 알아서
   main.yml의 `ttbb_Hadronic/SemiLeptonic/2L2Nu`가 default **"tt+LF"** 키를
   받고 있었다 — 즉 ttbb dedicated 3종이 tt+LF의 norm reweight를 곱는 중이었음.
2. **inclusive 61–72 미스라우트**: `Classify`가 61/62/71/72를 몰라 kNotTT→
   "tt+LF". (stitch가 weight=0으로 만들어 yield 영향은 없었으나 키 자체가 틀림.)

## 2. 헤더 확장 (`Config_TtCatGroup.hh`, single source — analyzer·도구 공유)
- `Group::kNB` 추가 (61,62=tt+bbb / 71,72=tt+4b). 기존 enum 값 보존.
- `Classify`: 61–72 → kNB (genTtbarId≤55만 들어오는 기존 경로 동작 불변).
- `IsDedicatedTtbarHF` / `IsTtbarFamily` 신설 (per-channel 명 + 구명 호환).
- `MakeProcessKey`: **ttbar family 전체를 category 디스패치**로 — sample 고정
  키("ttbb"/"tt4b"→tt+B) 제거. kNB → **"tt+nb"**. stitch-zero 이벤트도 자기
  category 키를 받음 (weight 0이라 yield 무영향; 유도/적용 키 일관성 목적).
- `AllProcessGroupKeys`: "tt+nb" 추가 → **8 그룹**.
- `ProcessKeysForSample`: ttbar family → 4 category 전부.

analyzer는 이미 `MakeProcessKey(_sampleName, _expandedTtbarId)`로 호출하므로
**헤더 수정만으로 적용 경로가 자동 정합**된다. 종료 진단(`[stitch] ... processKey
by expandedTtbarId%100`)은 이제 "tt+nb split: YES"를 출력해야 정상.

## 3. corrMgr — 평가 실패 silent 1.0 제거 (요구 12 연장)
`getTriggerSF`/`getBTagReweight`의 evaluate 예외가 **경고 5회 후 silent 1.0**
이었다. main/debug(requireDerived_)에서는 FATAL로:
- getTriggerSF 평가 실패 → exit **47**
- getBTagReweight 평가 실패 → exit **46** (전형 원인: **매핑 변경 후 JSON
  미재생성** — 예: 새 "tt+nb" 키가 옛 JSON에 없음. 메시지에 재유도 안내 포함.)
btagtrig(bootstrap)은 기존 WARN+1.0 유지.

## 4. ⚠ 운영 순서 (중요)
이 변경 후 **main/debug 모드는 옛 btagNormReweight.json(7그룹)으로 돌리면
"tt+nb" 키 미존재로 exit 46** 한다 — 의도된 동작(silent 1.0 금지). 순서:
1) btagtrig 재실행 (stitchWeight+expandedTtbarId 포함 새 skim)
2) 도구 업데이트(별도 트랙: NtupleReader에 두 branch 추가, stitched base 사용,
   본 헤더의 category 디스패치 — expandedTtbarId 전달) 후 exe_BTagSF → exe_MakeJSON
   으로 **8그룹 JSON 재유도**. 이때 tt+nb 그룹의 bin stat ≤30%(ttH AN A.2.1)
   자동 리포트 추가, 미달 시 tt+B 병합 fallback 결정.
3) main 재실행.

## 5. Stack 옵션 (b) — 설계 (구현은 main 재실행 후, plotter 패키지에서)
main output `Tree`가 `expandedTtbarId` + stitched/SF된 `evtWeight`를 가지므로,
plotter(또는 중간 hist 단계)가 `ClassifyExpanded` 기준으로 ttbar family 샘플을
6 fine category(LF/cc/ttb/tt2b/ttbbb/tt4b)로 분리해 hist를 만든다. analyzer
hist 폭증 없음, category 정의 변경 시 재실행 불필요. 카테고리 함수는 본 공유
헤더를 사용 (plotter에 사본 금지).

## 6. 검증 (전부 통과 — 사용자 요구: reweight 코드 검증 필수)
1. **Truth table 전수 (컴파일+실행)**: 32케이스 (inclusive×11 id, per-channel
   ttbb, tt4b, 구명, 신호 4종, minor 4종) — mismatch **0**.
2. **구버전 oracle 비교**: 백업 헤더를 OldTtCatGroup으로 컴파일, 15샘플×19 id
   전수 — 차이 **92건 전부 의도된 변경 화이트리스트**(① inclusive 61-72 fix,
   ② per-channel ttbb fix, ③ 구명 sample-고정→category) 내, **위반 0**.
3. **치역 검사**: MakeProcessKey의 모든 출력 ⊆ AllProcessGroupKeys(8) —
   미지 키 FATAL의 구조적 예방.
4. SanitizeKey 8개 유일 ("tt+nb"→"ttnb" 충돌 없음), ProcessKeysForSample
   (family=4키, minor=1키).
5. 도구 호환: 도구 소스는 Group enum switch/StitchingWeight 직접 사용 없음
   (string 인터페이스만) → enum 확장에 컴파일 안전. 도구는 genTtbarId(≤55)만
   전달하므로 새 skim 전까지 도구 동작 불변.
6. brace balance 전 파일 불변.

## 7. 사용자 로컬 검증
```bash
make && make -C bTagSF_ReweightStudy   # 양쪽 컴파일 (공유 헤더)
# debug 1파일 (ttbb_Hadronic 권장 — 버그 fix 직접 확인):
#   종료 로그 "[stitch] b-tag reweight processKey..." 에서
#   sub=53..55 -> tt+B (이전: tt+LF!), tt4b 파일이면 sub=71 -> tt+nb, split: YES
# 옛 JSON으로 main 실행 → exit 46 + "tt+nb ... JSON 재생성" 메시지 확인
```

## 롤백
```bash
cp docs/backup_20260611/bTagSF_ReweightStudy_include/Config_TtCatGroup.hh bTagSF_ReweightStudy/include/
# corrMgr의 [STEP7.5] 2개 catch 블록에서 requireDerived_ 분기 제거
```
