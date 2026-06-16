# STEP 15 — resubmit 완료판정 버그(중첩 TTree) + region output 분리 + report 거짓메시지

- 날짜: 2026-06-15
- 증상: main job 들이 실제로는 정상 완료(output ROOT 존재, Tree 디렉토리 보유)인데
  `--report` 가 `complete=0 missing=N` 으로 전부 미완료라 보고. 또 그 직후
  `All outputs already complete` 거짓 메시지. region(muon/electron) 별 output 이
  같은 `AnalyzerOutput_main` 에 덮어씀.
- 변경: `submit_job_FH_Tier3_unified.py`

## 1. (핵심) 완료판정이 중첩 TTree 를 못 봄
`_output_is_complete` 의 non-prescan 분기가 **최상위 키만** 순회:
```python
for key in f.GetListOfKeys():        # 최상위만
    if obj.InheritsFrom("TTree") and obj.GetEntries()>0: return True
```
그런데 분석 output 의 tree 는 `Tree/` **디렉토리 안**에 있다(최상위는 jet/Lepton/
CutflowKinematics/TtCatValidation/Tree 전부 TDirectoryFile). → TTree 를 못 찾아
**모든 파일이 거짓 incomplete** → `complete=0`.
(prescan 은 tree 가 최상위라 `f.Get("prescan")` 으로 찾아져 영향 없었음 — 그래서
prescan resubmit 은 정상이었다.)

**수정**: 디렉토리를 재귀로 내려가며 non-empty TTree 탐색(`_has_nonempty_tree`,
depth≤4 안전장치). 이제 `Tree/` 안 tree 를 찾아 complete 로 정확히 판정.

## 2. report 모드 거짓 'All outputs already complete'
`generate_argument_list` 는 report 시 집계만 하고 `return 0`. 호출부
`setup_and_submit_job` 가 그 0 을 'job 0개=완료'로 오해해 거짓 메시지 출력.
→ `report_only` 면 그 메시지 분기 전에 `return`. report 는 현황만 출력.

## 3. region 별 output 분리 (muon/electron CR)
`path_output_base = AnalyzerOutput_<mode>` 에 region 미반영 → FH/muon/electron 이
같은 경로에 덮어씀. condor filelist/log 경로도 공유 → job 충돌.
**수정**: `_region_suffix = "_<region>" if region else ""` 를 output base 와
condor 경로에 추가. 결과:
- FH(기본)      : `AnalyzerOutput_main/`
- muon CR       : `AnalyzerOutput_main_muon/`
- electron CR   : `AnalyzerOutput_main_electron/`
region 없으면 기존과 100% 동일.

## 검증
- syntax OK. 재귀 탐색/ region suffix/ report 분기 배선 확인.
- 사용자 환경: ttHH_0.root 에 `Tree;1` 디렉토리 확인됨 → 재귀 탐색이 잡음.
- prescan 영향 없음(최상위 prescan tree 경로 그대로).

## 로컬/사용자 검증
```bash
# report 가 이제 정확히 complete 집계
python3 submit_job_FH_Tier3_unified.py --mode main --report
# hold/실패분만 재제출
python3 submit_job_FH_Tier3_unified.py --mode main --resubmit
# muon CR 은 별도 디렉토리로
python3 submit_job_FH_Tier3_unified.py --mode main --region muon --files-per-job 5
```

## 롤백
`_output_is_complete` 의 재귀 블록을 원래 최상위 루프로, output base/report 분기
원복.
