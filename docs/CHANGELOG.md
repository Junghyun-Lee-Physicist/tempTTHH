# Changelog — ttHH(HH→bbbb) Fully-Hadronic Analysis

> **Purpose:** one chronological line per change, newest first, linking to the full record. The detail lives in [`changes/STEP_*.md`](changes/); this file is the index, not a copy.
> **Audience:** anyone tracing when and why something changed.
> **Status:** append-only · last meaningful update **2026-07-12**.
> **Links:** decisions [`DECISIONS.md`](DECISIONS.md) · state [`STATUS.md`](STATUS.md).

> Append-only: add new entries at the top; do not rewrite history. "Detail" links point to the full per-step record.

## 2026-07-12 — STEP 22: merge_outputs.py — AnalyzerOutput 자동 발견 hadd (local 병렬/condor)

- 신규 `outputMerger/merge_outputs.py`: `<base>/<proc>/<proc>_*.root` → `<base>/<proc>.root` merge 를 **디렉토리 자동 발견**으로 수행 — 구 merge_submitter.py 의 하드코딩 목록(stale 이름으로 다수 샘플이 조용히 누락될 상태) 제거. `--mode local --jobs N`(병렬 + 완료 대기 + OK/FAIL 요약 + exit 전파) / `--mode condor`(proc 당 1 job, submit_hadd_validation 템플릿), `--only/--exclude/--skip-existing/--list/--dry-run`. 실행 단위는 검증된 `run_one_hadd.sh` 재사용.
- 검증: 74개 실디렉토리명 가짜 트리 + stub runner 로 발견/필터/병렬/실패복구/condor 생성 end-to-end 테스트 전부 통과. 실제 hadd 는 Tier3 에서 `--list` → `--dry-run` 순 확인 권장.
- Detail: [`changes/STEP_22_merge_outputs_autodiscovery.md`](changes/STEP_22_merge_outputs_autodiscovery.md).

## 2026-07-10 — STEP 21: plotter 전체 샘플 + compact/detailed 2-모드 그룹핑 + cutflow legend 수율 + PDF

- `plotter/samples_config.yml`: MC 24 → **61** (ttHToNonbb, tH, ttW/ttZ 잔여, single-top, VV, V+jets, DY 등 37개 추가; 미존재 파일은 자동 skip).
- `plotter/stack_plotter.C`: 그룹핑을 substring → **exact-name 테이블**로 교체 — 구 라우팅의 오배정(TTWW/TTWZ/TTWH/TTZH*/TTZZ*/TTTW 가 전부 "tt+V" 흡수) 수정. env `TTHH_PLOT_GROUPING` 으로 **compact(MC 10줄, 기본)/detailed(MC 13줄)** 2-모드; TTZHTo4b/TTZZTo4b 는 두 모드 모두 별도 줄(`tt+ZH/ZZ(4b)`). cutflow legend 수율을 Integral(≈noCut 지배) → **최종 cut bin(nTotal)** 으로 (`LegendYield`), legend 에 "yields after final cut" 헤더. 출력 PNG → **PDF**, 모드별 `plots_<grouping>/` 분리.
- `plotter/scenario_runner.py`: `--grouping compact|detailed|both` 추가(env 전달, 모드별 로그/PDF 카운트).
- 검증: 그룹핑 함수 g++ 실컴파일 전수 테스트(61 샘플 × 2 모드 + 회귀 방어) ALL PASSED. ROOT 렌더링은 로컬 1회 확인 권장.
- Detail: [`changes/STEP_21_plotter_full_samples_two_mode_grouping.md`](changes/STEP_21_plotter_full_samples_two_mode_grouping.md).

## 2026-07-10 — STEP 20: --report 요약 테이블 + --status 상세 + 조회 read-only + lazy tmp

- `submit_job_FH_Tier3_unified.py`: `--report` 는 이제 CRAB 스타일 요약 테이블(`sample|jobs|done|miss|done%` + TOTAL)만 출력하고, 구 상세 출력(weight + missing idx 목록)은 신규 `--status` 로 이동. 조회 모드를 완전 read-only 화 — report 가 `arguments_<sample>.txt` 를 빈 파일로 truncate 하던 부작용 수정, output dir mkdir/chmod skip. tmp 디렉토리는 lazy 생성으로 전환: 첫 per-job filelist 를 쓸 때만 생성 → 모든 호출이 샘플마다 빈 `tmp_<sample>_<ts>/` 를 만들던 리터 제거 (빈 tmp 존재 = 그 invocation 에서 재큐 0건이던 원인 규명 포함). 판정 기준은 STEP 19 종료 마커 그대로.
- Detail: [`changes/STEP_20_report_table_status_readonly.md`](changes/STEP_20_report_table_status_readonly.md).

## 2026-07-10 — STEP 19: report/resubmit 완료판정 = 종료 마커 + non-tt ttCat 요약 게이트

- `submit_job_FH_Tier3_unified.py` `_output_is_complete` (non-prescan): "non-empty TTree" 기준 → **`cutflow_w_full` 종료 마커** 기준으로 교체. selection 을 아무 이벤트도 통과 못한 정상 job(저-HT WJets/QCD 에서 구조적 발생)의 거짓 missing(→ 영구 재제출 루프)과, STEP_15 §6 의 고위험 half-written 거짓 complete 를 동시에 해소. 기존 output 에 소급 적용 — 재실행 없이 `--report` 재실행으로 충분. prescan 분기 불변.
- `ttHHanalyzer_unified.cc` `printTtCatSummary`: pair 비교(ANA_GENPART vs ANA_GENID)를 `IsTtbarFamily` 로 게이트. non-tt 샘플은 genTtbarId 디코드가 noTT 를 표현할 수 없어(NanoAOD 가 모든 MC 에 gtid≥0 기록) agreement 0% 가 정의상 결과 — "~97% 기대" 라벨 대신 SKIPPED 사유를 출력. 진단 stdout 만 변경, 물리/branch 무영향. **analyzer 재빌드 필요.**
- Detail: [`changes/STEP_19_completion_marker_and_ttcat_note.md`](changes/STEP_19_completion_marker_and_ttcat_note.md).

## 2026-06-30 — fix: make_filelists TTbar/ttbb disk-directory keys

- `make_filelists.py` `SAMPLE_MAP`: corrected 6 keys (on-disk dataset dir names) that STEP 18 had wrongly renamed to the short_name form (`TTbar_Hadronic_…`, `TTbb_4f_TTbar_…`). Real dirs are `TTToHadronic_…` / `TTToSemiLeptonic_…` / `TTTo2L2Nu_…` and `TTbb_4f_TTTo*_…`; this fixes the 6 `[MISSING] Directory not found` errors. short_names (values, = project keys) unchanged, so analyzer/xsec_db/yml/group-map are unaffected. Restores the module's own documented invariant ("on-disk primary dataset 이름은 불변").

## 2026-06-30 — STEP 18 (part C): exit-code system + correction-path policy + docs

- Added `include/ExitCodes.h` (canonical, collision-free exit codes) and `include/ConfigPath.h` (path resolver with no code default). Remapped all scattered exit codes in `ttHHanalyzer_unified.cc`, `src/CorrectionsManager.cc`, `src/ExpandedTtbarId.cc`, `src/StitchFactors.cc` to the new scheme; fixed the `41`/`43` collisions. → see [`DECISIONS.md`](DECISIONS.md) D-2026-06-30-C.
- Replaced the submitter's blank-means-default path export with the explicit-policy loop (E12/E13, `__NULL__` sentinel, always-export); numbered the xsec/prescan FATALs (E20/E21); taught the yml loader `null`→None. → D-2026-06-30-D.
- Updated `AnalyzerConfig/Tier3_2017_FH_unified_{prescan,main,btagtrig}.yml` `common.path_*` (jsonpog/golden explicit; the four derived corrections `null`=disabled). → D-2026-06-30-E.
- Created the guideline-conformant doc set: [`README.md`](README.md), [`STATUS.md`](STATUS.md), [`DECISIONS.md`](DECISIONS.md), this file, [`reference/ERROR_CODES.md`](reference/ERROR_CODES.md), [`reference/CONFIG_PATHS.md`](reference/CONFIG_PATHS.md); copied the documentation guideline into `docs/`.
- Detail: [`changes/STEP_18_ntupleforge_naming_and_kfactor_cleanup.md`](changes/STEP_18_ntupleforge_naming_and_kfactor_cleanup.md) (part C section).

## 2026-06-30 — STEP 18 (parts A–B): NtupleForge naming + k-factor cleanup

- Adopted NtupleForge campaign names as the project-wide sample key (analyzer/config/plotter/b-tag-SF). k-factors folded into notes (`kfactor=1.0`). → D-2026-06-30-A, D-2026-06-30-B.
- Detail: [`changes/STEP_18_ntupleforge_naming_and_kfactor_cleanup.md`](changes/STEP_18_ntupleforge_naming_and_kfactor_cleanup.md).

## 2026-06-29 — STEP 17 — full-NanoAOD dataset + categorization

- Migrated the analyzer to the `ttHH2017UL_fullNano_v20` campaign and to standard `genTtbarId` decode. Detail: [`changes/STEP_17_fullNano_dataset_and_categorization.md`](changes/STEP_17_fullNano_dataset_and_categorization.md).

## Earlier — STEP 0–16

See the per-step records in [`changes/`](changes/) (indexed by [`changes/README.md`](changes/README.md)). Notable: STEP 11 (xsec_db single source), STEP 12 (fb units), STEP 14 (3-tier weight + MET-CR + norm check), STEP 16 (SF toggles + output-dir split), STEP 4 (paths/yml FATAL — superseded in part by STEP 18 part C path policy).

## 2026-07-06 — Data era-check fix (STEP_18 follow-up #2)
- **Bug**: all Data jobs in `main` mode died with `[ERROR] eraName mismatch ... eraName: D, sampleName: BTagCSV_Run2017D` followed by a segfault. Root cause: `ttHHanalyzer_unified.h::extractEraFromSampleName()` still assumed the *legacy* Data naming (`JetHT_B`, single-letter last token). STEP_18 renamed Data samples to `<PD>_Run2017<E>`, so extraction returned `""` → mismatch → FATAL. MC does not take this branch, hence "MC runs fine, Data dies".
- **Fix 1**: `extractEraFromSampleName()` now recognizes `Run20YY<E>` (era-agnostic: 2016/2017/2018; tolerant of `_ext1` suffixes) with the legacy single-letter form kept as fallback. Unit-tested against 13 old/new/edge sample names.
- **Fix 2**: the two runinfo exits (`MC must not define era`, `era mismatch`) now emit `[FATAL][E11]` (tthh::CONFIG_BAD_RUNINFO) and use `std::_Exit` — `std::exit` triggered a ROOT-teardown segfault that polluted the exit status (139 instead of a canonical code). `ExitCodes.h` is now included by the header directly.
- Files: `ttHHanalyzer_unified.h` only. Rebuild required (`make`).
