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


## 6. 완료판정(report/resubmit)의 한계 — 알려진 거짓 양성/음성
`_output_is_complete` 는 "output 파일 존재 → TFile 열림(not zombie) → (디렉토리
재귀로) non-empty TTree 1개 이상" 으로 판정한다. files-per-job/filelist 를
일치시켜도 다음 경우는 **여전히 틀릴 수 있다**:

**거짓 음성 (완료인데 missing — 재제출 낭비, 위험 낮음)**
- cmsenv 밖 실행 → PyROOT 없음 → 전부 incomplete. 단 WARN 출력되어 인지 가능.
- 제출 후 마스터 filelist 변경(파일 추가/순서 변경) → chunk↔job_idx 어긋남.
- output tree 깊이 5단 이상(현재 재귀 depth≤4). 현 구조는 Tree/ 1단이라 무관.

**거짓 양성 (미완료인데 complete — 조용한 누락, 위험 높음)**
- **half-written**: job 이 중간에 죽었어도 이미 write 된 TTree 에 entry 가 1개라도
  있으면 complete 로 본다. entry 수가 기대치와 맞는지는 **검사하지 않음**. → 가장
  주의할 케이스. yield 가 조용히 적게 나올 수 있다.
- 옛 output 잔존: 다른 설정/N 으로 만든 `_i.root` 가 남아 있으면 complete 로 오인.
  (대책: region/SF 별 디렉토리 분리 + 재제출 전 옛 output 정리.)
- ROOT recover: 깨진 파일을 TFile 이 자동 복구해 IsZombie=false 가 되면, 복구된
  일부 TTree 로 통과 가능(드묾; Warning 은 gErrorIgnoreLevel 로 억제됨).

**실무 방어**: (a) 제출 후 filelist 고정, (b) 재제출/영역전환 시 옛 output 정리,
(c) half-written 의심 시 entry 수 직접 확인:
`root -l -b -q f.root -e '((TTree*)gFile->Get("Tree/<name>"))->GetEntries()'`.
**근본 개선(미구현)**: 정상 종료 마커(예: output 에 nProcessed 스칼라 기록 후
report 가 그 존재/값을 검사)를 두면 half-written 을 거를 수 있다. → 향후 과제.

> **[2026-07-10 후속]** 위 근본 개선은 **STEP_19 에서 구현됨** — analyzer 가
> loop() 마지막에 Write 하는 `cutflow_w_full` 을 종료 마커로 사용하도록
> `_output_is_complete` 를 교체 (analyzer 무수정, 기존 output 소급 적용).
> 이 문서의 "거짓 음성" 목록에 빠져 있던 케이스(**정당하게 빈 output**:
> selection 통과 0건 → Tree entries=0 → 영구 재제출 루프)도 같이 해소.
> 상세: [`STEP_19_completion_marker_and_ttcat_note.md`](STEP_19_completion_marker_and_ttcat_note.md).
