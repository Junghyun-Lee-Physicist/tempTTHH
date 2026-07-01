# STEP 17 — fullNano_v20 dataset 갱신 + prescan 분리 + categorization full-Nano 전환

- 날짜: 2026-06-29
- 요구: (1) 새 ntuple production `ttHH2017UL_fullNano_v20` (config_ttHH2017UL.yaml,
  full-NanoAOD pass-through, 신규 dataset 다수)에 맞춰 config/filelist/script 갱신.
  (2) prescan 은 cross section/BR normalization 불필요 — 코드에서 그걸 막던 부분 해소.
  (3) analyzer 의 샘플별 분기/branch 의존을 새 dataset 에 맞게 갱신. 모든 dataset 사용.
- 변경 파일:
  - `make_filelists.py`
  - `AnalyzerConfig/Tier3_2017_FH_unified_prescan.yml`
  - `submit_job_FH_Tier3_unified.py`
  - `ttHHanalyzer_unified.h`, `ttHHanalyzer_unified.cc`
- 백업: `docs/backup_20260629/` (위 5개 파일의 변경 전 원본)

---

## 0. 배경 — 새 ntuple 은 full NanoAOD 다 (핵심 발견)

`config_ttHH2017UL.yaml` 은 `analysis_module: ["modules/noop.py","MODULES"]` +
`branch_file: "branches/branch_keep_all.txt"`, jobID/output_base 에 "fullNano".
즉 출력이 slim ttHH ntuple 이 아니라 **full NanoAOD pass-through** (모든 NanoAOD
branch 유지, custom branch 추가 없음). 사용자 확인:
- 출력 파일명: `slimmedNtuple_<N>.root` (기존과 동일 — make_filelists glob 불변).
- on-disk 경로: `/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH2017UL_fullNano_v20`.
- **ntuplizer categorization branch (`ttCat_*`, `ttCatXval_*`) 없음** → 작업 필요.

근거 정황: 현재 `include/eventBuffer.h` 가 이미 full-Nano 로 재생성되어 있었음
(`ttCat_LightFlavour` 0개; `eventBuffer.h.v18_backup` 엔 6개). 그런데 analyzer 는
여전히 그 branch 를 읽고 있어 **현재 eventBuffer.h 로는 컴파일 불가** 상태였다.

---

## 1. make_filelists.py — dataset 갱신

### 무엇을
- `SAMPLE_DIR` → `.../ttHH2017UL_fullNano_v20`.
- `sample_mapping` 에 신규 dataset 추가 (총 MC 61 + Data PD 3).
- glob 은 `slimmedNtuple*.root` 유지 (확인됨).

### naming 정책 (중요)
- **기존 샘플 = OLD short_name 유지** (`TTToHadronic`/`ttHH`/`tt4b`/`ttbb_*`/`ttZtobb`
  /`ttZHto4b`/`ttZZto4b`/`ttWW`/`ttWH`/`ttWZ`/`tttW`/`tttt`/`ttHtobb`).
  config YAML 의 key(`TTbar_Hadronic` 등)는 rename 됐지만 그건 ntuplizer 내부 라벨일
  뿐, on-disk **primary dataset 이름은 불변**이고 analyzer 의 exact-match
  categorization/stitching(`Config_TtCatGroup.hh`), registry, prescan_summary,
  plotter yml 이 모두 OLD 이름에 묶여 있으므로 바꾸지 않는다 (물리 로직 리스크 0).
- **신규 샘플 = config YAML key 를 short_name 으로** (legacy 없음):
  `ttHToNonbb`, `tHq`, `tHW`, `ttWJetsToQQ`, `ttWJetsToLNu`, `ttZToLLNuNu`,
  `ST_{t_top,t_antitop,tW_top,tW_antitop,s_lep,s_had}`, `WW`/`WZ`/`ZZ`,
  `WJetsToQQ_HT*`(3), `ZJetsToQQ_HT*`(3), `WJetsToLNu_HT*`(8), `DYJetsToLL_M50_HT*`(8).

### ext 자동 병합
`TTZHTo4b_ext1`, `TTZZTo4b_ext1`, `WJetsToLNu_HT*_ext1/ext2` 는 base 와 primary
dataset 이름이 같아 SAMPLE_DIR 아래 같은 디렉토리에 들어간다. `find_root_files()`
의 `os.walk` 가 재귀 수집하므로 base+ext 가 한 filelist 로 자동 병합된다
(config 주석 "combine BOTH for statistics" 와 일치). → ext 별 항목 불필요.

### key 매핑 메모 (downstream)
- `TTbb_4f_TTToHadronic_...` → `ttbb_Hadronic` (config key `TTbb_Hadronic` 와 다름; 의도).
- `TTHHTo4b_...` → `ttHH`. `TT4b_...` → `tt4b`. `TTZToBB_...` → `ttZtobb`.

---

## 2. prescan yml — 신규 MC 61개 전부 등록

- `Tier3_2017_FH_unified_prescan.yml` 의 `samples:` 를 새 production 전체 MC(61)로 교체.
- Data 는 제외: `runPrescan()` 의 `readRunsTreeSums()` 가 Data NanoAOD 의 부재한
  Runs/genEventSumw 를 skip 하므로 prescan 대상이 아님.

---

## 3. submitter — prescan 을 normalization 과 분리

### 문제 (이전)
`parse_config_entry` 가 prescan 모드와 무관하게 weight 를 합성하려 했다:
- `data_or_mc` 를 `xsec_db`(cross_section_fb null) 로 판정 → xsec_db 에 없는 신규 MC 는
  `None` 으로 읽혀 **"Data" 오분류** → era 추출 실패 → FATAL.
- `_compute_base_weight()` 가 `xsec_db` + `prescan_summary.json` 둘 다 요구 → 신규
  샘플은 둘 다 없음 → FATAL. (게다가 prescan 이 prescan_summary 를 만드는 단계라
  닭-달걀 문제.)

### 해결
`analysis_mode == "prescan"` 이면:
- `data_or_mc`: PD 이름으로 판정 (`JetHT|BTagCSV|SingleMuon` → Data, 그 외 MC).
- `weight`: **1.0 고정** (runPrescan 이 Σgenw 만 누산, weight 미사용).
→ prescan 이 `xsec_db`·`prescan_summary` 에 의존하지 않으므로, 신규 샘플을 그 둘에
  먼저 등록하지 않아도 바로 돈다. (yml `weight`/`data_or_mc` 명시 시 override 유지.)

### prescan 이 normalization 불필요한 근거 (코드)
`runPrescan()`(ttHHanalyzer_unified.cc) 은 `readRunsTreeSums()`(Runs tree genEventSumw
합) + `accumulatePrescanEvent()`(genWeight·genTtbarId%100 누산) + `writePrescanTree()`
만 호출. `selectObjects`/SF/btag reweight/stitching 미호출. `requireDerived=false` 라
CorrectionsManager 도 파생 보정 누락을 FATAL 로 만들지 않음. cross section/BR/k 는
prescan 산출물(Σgenw)을 **입력**으로 나중 weight 합성에만 쓰인다.

---

## 4. analyzer categorization — ntuple branch → 표준 genTtbarId 디코드 (full-Nano 대응)

### 무엇을 / 왜
full-Nano ntuple 에 `ttCat_*`/`ttCatXval_*` custom branch 가 없어 analyzer 가 그걸
읽던 코드는 컴파일 불가 + 의미 없음. categorization 의 single source of truth 를
**표준 NanoAOD `genTtbarId` 의 런타임 디코드**로 이동:

- `officialTtCategory()` : `readNtuplePrimaryCategory()` → `computeTtCategoryFromGenTtbarId()`.
- `readNtuplePrimaryCategory()` / `readNtupleXvalCategory()` : **삭제** (branch 부재).
- 4-way validation(ANA_GENPART/ANA_GENID/NTU_PRIMARY/NTU_XVAL) → **2-way**
  (ANA_GENPART vs ANA_GENID): 히스토그램 포인터/booking/fill/dump/debug-print 에서
  NTU_* 전부 제거. 남은 hist: `ttCat_Counts_AnaGenPart`, `ttCat_Counts_AnaGenId`,
  `ttCat_AnaGenPart_vs_AnaGenId`, `ttCat_GenTtbarIdMod100`.
- branch-availability print 와 per-event debug print 에서 `_ev->ttCat*` 라인 제거.
- prescan ttCat-bucket cross-check: `readNtuplePrimaryCategory()` → `officialTtCategory()`
  (genTtbarId 디코드). prescan tree schema(`sumGenW_ttCat_*`)는 downstream
  (`consolidate_prescan.py`/`compute_stitch_factors.py`) 호환 위해 유지. id-bin 합과
  bucket 합이 구성상 일치 → 자기일관성 점검으로 동작(거짓 경보 없음).

### 값 보존 + AN 근거 (물리 로직)
`computeTtCategoryFromGenTtbarId()` 는 코드 주석상 "Equivalent to the ntuplizer's
decode_genttbarid()" 이며 CMS GenTtbarCategorizer.cc 의 디코드와 동일:
`%100`: 0→LF, 41–45→cc, 51→1B1H, 52→1B2H, 53–55→2B. 옛 ntuplizer 의 `ttCat_*`
primary 가 genTtbarId 에서 유도되던 경로(ttCatSource=GENTTBARID)와 **동일한 값**을
준다 → 카테고리 값 불변, source 위치만 ntuplizer→analyzer 로 이동.
- 근거: CMS GenTtbarCategorizer.cc; **ttHH AN-2022/122 §3.3·§3.4** (ttbar
  categorization); **ttH AN-19-094 §A.2.1** (per-process group).
- `Config_TtCatGroup.hh::Classify()` 의 genTtbarId 디코드와도 일관 (61/62/71/72 의
  tt+nb 분리는 여전히 ExpandedTtbarId 주입 lookup 에서 처리; 본 STEP 무관).

### 영향 없는 것
- `computeTtCategoryFromGenPart()` (GenPart 알고리즘) 유지 — 교차검증용.
- stitching(`StitchFactors`)·ExpandedTtbarId 주입은 `genTtbarId`/lookup 기반이라 불변.
- trigger PD-exclusivity 는 `_sampleName.find("BTagCSV"/"JetHT"/"SingleMuon")`
  (substring) 이라 data naming 변화에 robust — 변경 불필요.

---

## 5. 롤백

```bash
cd <repo>
cp docs/backup_20260629/make_filelists.py .
cp docs/backup_20260629/Tier3_2017_FH_unified_prescan.yml AnalyzerConfig/
cp docs/backup_20260629/submit_job_FH_Tier3_unified.py .
cp docs/backup_20260629/ttHHanalyzer_unified.h .
cp docs/backup_20260629/ttHHanalyzer_unified.cc .
```
(주의: 롤백 시 analyzer 는 다시 `ttCat_*` branch 를 요구하므로 full-Nano eventBuffer.h
와는 컴파일되지 않는다. 롤백은 slim ntuple + 구 eventBuffer.h 조합에서만 유효.)

## 6. 검증 (정적)

- make_filelists.py: 구문 OK, sample_mapping = 61 MC + 3 Data PD, SAMPLE_DIR=v20.
- prescan yml: 61 샘플 파싱(submitter loader), 전부 MC 판정, weight=1.0 (xsec_db/
  prescan_summary 미접근).
- submitter: 구문 OK.
- analyzer: `_ev->` 멤버 전수 대조 → eventBuffer.h(full-Nano)에 없는 멤버 **0개**
  (이전 15 → ttCat 12 제거 + DeepCSV 3개는 주석이라 무관). NTU_*/readNtuple* active
  참조 0. `_hTtCat_*` 남은 hist 4개만. brace 균형 = 원본과 동일(편집이 유발한 불균형 없음).

## 7. 다음 단계 (main-analysis phase — 본 STEP 범위 밖)

prescan 은 위로 충분. 본격 분석(main/btagtrig) 진입 시 추가 필요:
1. `data/samples_2017UL.json` (xsec_db): 신규 샘플 cross_section_fb/br 등록
   (main weight 합성용). 출처(XSDB/GenXSecAnalyzer/AN Table) 명시.
2. `Config_TtCatGroup.hh`: 신규 샘플 categorization 확인. 대부분 default `tt+LF`
   (ttH AN §A.2.1 minor-BG 처방)로 떨어져 코드 변경 불필요하나, **`ttHToNonbb`** 의
   group(tt+LF vs 별도) 은 AN 확인 필요. 변경 시 b-tag SF JSON 재생성 동반.
3. plotter `samples_config.yml` / bTagSF `Config.hh::SampleRegistry` 3중 동기화.
4. stitch_factors / b-tag reweight JSON 재생산 (신규 dataset 통계 반영).

---

## ADDENDUM: dataset registry & config lists (full set)

**BLUF**: 신규 dataset(`ttHH2017UL_fullNano_v20`)을 xsec_db와 main/btagtrig config에 반영. xsec 값은 **잠정(표준 참고값)** 이며 확정 필요.

### 변경
- **`data/samples_2017UL.json`** [DECIDED]: 신규 MC 37개 엔트리 추가(총 77). 각 엔트리에 `das_path`/`br`/`accuracy` 기입.
  - `cross_section_fb`는 **표준 참고값**이며 모두 `verify_xsec:true` 플래그(파일 기존 "확인 필요" 관례와 동일).
  - `ttHToNonbb`만 확정값(ttH total 507.1 fb × BR(H→non-bb)=0.4176).
  - `ST_s_had`는 `cross_section_fb:null`(미확정) → main/btagtrig에서 제외.
- **`AnalyzerConfig/Tier3_2017_FH_unified_main.yml`**, **`..._btagtrig.yml`** [DECIDED]: sample 리스트를 60 MC + 15 Data(75)로 확장. `ST_s_had` 제외(null xsec → FATAL 회피). 헤더에 prescan/xsec 선행 주의 명시.
- **`..._prescan.yml`**: 61 MC(ST_s_had 포함 — prescan은 Σgenw만 누적하므로 OK).

### 검증
- main/btagtrig의 모든 MC가 xsec_db에 non-null로 등록됨(`_compute_base_weight` FATAL 없음).
- prescan ↔ xsec_db MC 집합 완전 일치(orphan 0).

### OPEN (확정 필요)
- **[OPEN] xsec 확정**: `verify_xsec:true` 항목(특히 `tHq`,`tHW`,`ST_tW_top/antitop`,`ST_s_lep`,`WW`,`WZ`,`ZZ`)을 XSDB/GenXSecAnalyzer로 확정. `ST_s_had`는 null → 채운 뒤 main/btagtrig에 추가.
  - 주의: 이 환경에서 ttHH AN PDF가 손상되어 AN 표 인용 불가, `build_xsec_db.py` 부재. 잠정값은 수기 입력이며 권위 소스로 교체해야 함.
- **[OPEN] tt+nb lookup 재생성**: Expanded_genTtbarId는 main ntuple branch가 아니라 side lookup(`_expTtbarId.resolve` → `DerivedCorr/expandedTtbarId/ttnb_<sample>.root`). 신규 dataset 기준으로 `ttnb_<sample>.root`를 재생성해야 tt+nb 분리(sub-code 61/62/71/72) 동작. eventBuffer/prescan과 무관한 별도 산출물.
