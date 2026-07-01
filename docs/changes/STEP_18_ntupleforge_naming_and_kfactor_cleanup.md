# STEP_18 — NtupleForge sample-naming 채택 + k-factor 필드 정리

**BLUF**: sample 이름의 **단일 진실원천(single source of truth)을 NtupleForge campaign config
(`campaign_ttHH2017UL_fullNano_v20`)의 dataset key 로 통일**했다. analyzer·config·plotter·b-tag SF
도구 전반의 short_name 을 그 이름으로 re-key 했고, data 샘플은 `JetHT_Run2017B` 형식으로 통일,
submitter 의 era 추출 정규식(regex)을 이에 맞게 확장했다. 또한 `kfactor_an_ref` 숫자 필드를
**제거하고 note(주석) 텍스트로 강등**했다(중복 적용 방지). 결과: prescan / main / btagtrig 세 모드가
신규 이름 체계에서 일관되게 동작.

상위 맥락은 STEP_17(full-NanoAOD dataset 전환 + genTtbarId 기반 categorization) 참조.

---

## DECIDED

### 1. NtupleForge 이름을 canonical 로 채택
이전까지 파이프라인은 짧은 별칭(short alias, 예 `ttHH`, `TTToHadronic`, `JetHT_B`)을 썼고,
사용자 제공 xsec_db(`data/samples_2017UL.json`)·문서는 NtupleForge 정식 이름
(예 `TTHHto4b`, `TTbar_Hadronic`, `JetHT_Run2017B`)을 썼다. 이 **이름 불일치가 main/btagtrig
의 xsec_db lookup FATAL 의 직접 원인**이었다(약 20 MC + 15 data 미등록 처리).

해결: 코드/설정 쪽을 NtupleForge 이름으로 맞춤(= JSON 을 진실원천으로 인정). 향후 ntuplizer 와
analyzer 가 동일 키를 공유 → drift 위험 제거.

### 2. Rename map (OLD short alias → NtupleForge)
| OLD | NEW | | OLD | NEW |
|---|---|---|---|---|
| `TTToHadronic` | `TTbar_Hadronic` | | `tttt` | `TTTT` |
| `TTToSemiLeptonic` | `TTbar_SemiLep` | | `tttW` | `TTTW` |
| `TTTo2L2Nu` | `TTbar_DiLep` | | `ttWH` | `TTWH` |
| `ttbb_Hadronic` | `TTbb_Hadronic` | | `ttWW` | `TTWW` |
| `ttbb_SemiLeptonic` | `TTbb_SemiLep` | | `ttWZ` | `TTWZ` |
| `ttbb_2L2Nu` | `TTbb_DiLep` | | `ttWJetsToQQ` | `TTWJetsToQQ` |
| `tt4b` | `TT4b` | | `ttWJetsToLNu` | `TTWJetsToLNu` |
| `ttZtobb` | `TTZToBB` | | `ttZToLLNuNu` | `TTZToLLNuNu` |
| `ttHH` | `TTHHto4b` | | `ttHtobb` | `ttHTobb` |
| `ttZHto4b` | `TTZHTo4b` | | `ttZZto4b` | `TTZZTo4b` |
| `JetHT_B`..`_F` | `JetHT_Run2017B`..`F` | | `BTagCSV_*`/`SingleMuon_*` | `*_Run2017<X>` |

`ttHToNonbb`, `tHq`, `tHW`, `ST_*`, `WW/WZ/ZZ`, `QCD_HT*`, `WJetsToQQ_HT*`, `ZJetsToQQ_HT*`,
`WJetsToLNu_HT*`, `DYJetsToLL_M50_HT*` 는 이미 동일 → 무변경.

### 3. 안전 구분 — sample 이름 ≠ process-group key ≠ stitch category (보존)
re-key 시 **동일 문자열이 다른 의미로 쓰이는 곳은 보존**했다(blind sed 금지):
- **process-group key** `tt+LF / tt+cc / tt+B / tt+nb / ttH / ttHH / ttZH4b / ttZZ4b` —
  `Config_TtCatGroup.hh::AllProcessGroupKeys` 및 `MakeProcessKey` 의 **반환값**. 그대로 유지.
  → 예: `MakeProcessKey` 는 `sampleName == "TTHHto4b"` (신규명) 일 때 group key `"ttHH"` (보존) 를 반환.
  b-tag reweight JSON 의 group key 도 `"ttHH"` 로 불변 → analyzer lookup 정합 유지.
- **stitch category label** `LF / cc / ttb / tt2b / ttbbb / tt4b` —
  `compute_stitch_factors.py`(`sub_to_category`, `owns`, `STITCH_CATEGORIES`) 및
  `StitchFactors.cc` 주석. 그대로 유지(sub-code 71/72 → category `"tt4b"`).
- analyzer **본체**(`ttHHanalyzer_unified.cc/.h`)는 sample 이름 하드코딩 없음:
  trigger PD 는 `find("JetHT"/"BTagCSV"/"SingleMuon")` substring(→ `JetHT_Run2017B` 도 매칭),
  categorization 은 `TtCatGroup::MakeProcessKey(_sampleName,...)` 위임, ttnb lookup 파일명은
  런타임에 `_sampleName` 으로 구성. → analyzer 본체 무변경.

### 4. data era 추출 정규식 확장 (`submit_job_FH_Tier3_unified.py`)
```python
m = (re.search(r"Run2017([A-Z])$", self.sample_name)   # JetHT_Run2017B -> B
     or re.search(r"_([A-Z])$", self.sample_name))      # (legacy JetHT_B fallback)
```
신규 `Run2017<X>` 우선, 구 `_<X>` fallback 유지.

### 5. k-factor 필드 정리 (`data/samples_2017UL.json`)
- 모든 샘플 `kfactor = 1.0`. (formula `w = L·σ·BR·k / Σgenw` 에서 k 는 항상 1.0)
- 별도 숫자 필드 `kfactor_an_ref` (ttHH 1.158 / ttZHto4b 1.35 / ttZZto4b 1.42) **제거** →
  각 엔트리 `note` 로 강등: "Handbook/AN ref k=X — 이미 cross_section_fb(NLO target)에 반영, 중복적용 금지".
  - 근거: submitter 는 `kfactor_an_ref` 를 읽지 않음(완전 inert). 숫자 필드로 두면 오적용
    (double-count) 위험만 남음 → provenance 는 텍스트로 기록이 안전(Guide §3.4/§10.2, CHANGELOG 권고).
- xsec 값은 사용자 제공 업로드본 그대로 사용(STEP_18 에서 미변경). 갱신 내역(사용자 작업):
  `tHq 74.25 fb`, `tHW 15.17 fb`, `TTWJetsToQQ 443.2`, `TTWJetsToLNu 216.3`,
  `TTZToLLNuNu 243.9`, `ST_s_lep 3549(confirmed)`, `ST_s_had 7104(채움, verify_xsec=true)`.
  → **null-xsec MC 0건** → main/btagtrig 에 61 MC 전부 포함 가능.

---

## 변경 파일
- `data/samples_2017UL.json` — 업로드본 채택 + `kfactor_an_ref` 5개 제거→note, `_meta` 문구 갱신
- `make_filelists.py` — MC short_name re-key + data 출력명 `filelist_<PD>_Run2017<period>.txt`
- `AnalyzerConfig/Tier3_2017_FH_unified_{prescan,main,btagtrig}.yml` — 신규명으로 재생성
  (prescan 61 MC / main·btagtrig 61 MC + 15 data)
- `submit_job_FH_Tier3_unified.py` — era 정규식 확장
- `bTagSF_ReweightStudy/include/Config_TtCatGroup.hh` — sample-name 비교만 re-key(group key 보존)
- `src/ExpandedTtbarId.cc` — ttnb fallback tag 리스트 re-key
- `bTagSF_ReweightStudy/include/Config.hh`, `TriggerStudy/include/Config.hh` — SampleRegistry re-key
- `compute_stitch_factors.py` — **sample 참조만** re-key(category 보존)
- 보조 도구(plot scripts, scenario_runner, stack_plotter) — sample 리스트/색상 substring re-key

## 검증 (이번 STEP)
- yml↔xsec_db 정합: prescan 61, main/btagtrig 76, 미등록 0, null-xsec MC 0 → **PASS**
- group key 8개 보존 확인, MakeProcessKey LHS=신규명/RHS=group key, IsInclusiveTtbar/IsDedicatedTtbarHF 신규명 → **PASS**
- stitch category `tt4b` 보존 확인 → **PASS**
- Python `ast.parse` 편집 파일 전부 OK; C++ `{}` 균형 0
- era 추출: `JetHT_Run2017B→B`, 구 `JetHT_B→B` 둘 다 정상

---

## OPEN / 후속(STEP_18 범위 밖)
1. **보조 도구 compile/run 검증**: b-tag SF 도구·TriggerStudy·plotter·outputMerger 는 prescan/main/btagtrig
   로 exercise 되지 않으므로 별도 빌드/실행 확인 필요(이번엔 syntax/brace 수준만 검증).
2. **`verify_xsec=true` 잔존 검증**: `ST_s_had`, `ST_tW_top/antitop`, `WW/WZ/ZZ` 등 — XSDB/GenXSecAnalyzer 로 사용자 확정 예정.
3. **신규 dataset derived corrections 재생성**: `DerivedCorr/expandedTtbarId/ttnb_<NtupleForge명>.root`,
   stitch_factors — 신규 sample 이름으로. (코드는 런타임에 sampleName 으로 파일명 구성 → 파일만 재생성하면 됨)
4. **base+ext 병합 실검증**: make_filelists os.walk 가 base/ext primary dataset 을 실제로 합치는지
   (on-disk 디렉터리 구조 의존).
5. prescan → xsec verify → main/btagtrig 순서 준수(FATAL coupling).

---

## PART C — 번호 exit-code 시스템 + correction-path 정책 (2026-06-30)

**BLUF**: analyzer/submitter의 모든 필수-로직 실패를 **충돌 없는 번호 exit code**로 종료시켜
Condor 로그에서 `grep '[FATAL][E'` 만으로 원인 식별이 되게 했고, `common.path_*` 의미론을
**"빈 값=금지(E12) / null=선택 비활성 / 필수면 null 금지(E13) / 코드 default 폐기"** 로 바꿨다.

상세 정본(중복 금지, one-fact-one-place):
- exit code 전체 표 → [`../reference/ERROR_CODES.md`](../reference/ERROR_CODES.md) (= `include/ExitCodes.h` 미러)
- path 정책 계약 → [`../reference/CONFIG_PATHS.md`](../reference/CONFIG_PATHS.md)
- 결정 근거 → [`../DECISIONS.md`](../DECISIONS.md) D-2026-06-30-C, -D, -E

### 신규 파일
- `include/ExitCodes.h` — `namespace tthh; enum ExitCode`. subsystem 대역(10s config, 20s norm,
  30s input, 40s central-corr, 50s derived, 60s stitch, 70s expttid, 80s per-event). **번호 불변.**
- `include/ConfigPath.h` — `cfgpath::resolve(env, what, required)`. env 미설정/빈값 → FATAL E12;
  `"__NULL__"` → required면 E13, 아니면 `""`(비활성) 반환; 그 외 → 경로 반환(로드는 호출측 책임).

### 코드 변경 (요지)
- `src/CorrectionsManager.cc`: `envOr(name,default)` **제거** → `cfgpath::resolve`. jsonpog=required,
  goldenjson=required(Data 한정), trigsf/btagrw=optional(빈 반환 시 로드 skip → SF=1). exit remap
  49→40(central)/41(golden), 47→50, 48·46→51.
- `ttHHanalyzer_unified.cc`: runinfo→E11, 입력 실패→E30, mode→E10, MakeProcessKey→E80, 비유한→E81,
  stitch-inactive 43→E63. stitch/expttid 경로도 `cfgpath::resolve`(default 폐기; null→미로드/INACTIVE).
- `src/ExpandedTtbarId.cc`: 41/42/43/44 → 70/71/72/73. `src/StitchFactors.cc`: 40/41/42/43 → 60/61/62.
- `submit_job_FH_Tier3_unified.py`: module-level `_fatal(code,msg)` + `EXIT`(12/13/20/21) + `NULL_SENTINEL`.
  path-export 루프를 정책 검증(빈값 E12 / null: optional→sentinel, required→E13 / 실경로→export, **항상 export**)
  으로 교체. xsec/prescan/sumw FATAL 번호화(E20/E21). 자체 YAML 로더에 `null/~/none → None` 추가
  (따옴표 `""` 는 빈 문자열로 유지 → null 과 구분).
- `AnalyzerConfig/Tier3_2017_FH_unified_{prescan,main,btagtrig}.yml`: `common.path_*` 갱신
  (jsonpog/golden = 명시 경로, 나머지 4개 = `null` 비활성). 정책 주석 추가.

### 검증 (이 환경)
- 헤더 standalone 컴파일 OK; 사용된 `tthh::` 상수 17개 전부 정의 확인.
- 편집 5개 C++ 파일의 brace/paren 델타가 **원본과 동일**(균형 보존).
- submitter `ast.parse` OK; 커스텀 로더로 실제 yml 파싱하여 path 검증 시뮬레이션:
  prescan(MC/Data)=OK(+4개 `__NULL__`), main+`--trigsf on`+null=E13(설계대로), main+`--trigsf off`=OK.
- **미완(OPEN)**: CMSSW_14_2_1 실제 build/run, 보조도구(BTagSF/TriggerStudy/plotter) compile.

### 주의 (DECIDED interim, D-2026-06-30-E)
`--trigsf` 기본값이 `on` 이므로 trigsf 경로가 `null` 인 채 `main` 을 돌리면 **E13**. trigger SF 재유도
전까지는 `main` 을 `--trigsf off --btagrw off` 로 돌리거나 실경로를 줄 것.
