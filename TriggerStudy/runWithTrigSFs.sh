#!/bin/bash
set -euo pipefail

# ─────────────────────────────────────────────────────
# 1) EventLooper 실행 (Step 02: 적용/클로저용 RAW 효율 산출)
#    ※ exe_TrigStudy <sample> <mode>
#       여기서 '1'은 네 바이너리가 쓰는 Step02 모드(적용)로 가정.
#       (SingleMuon도 돌려서 Data RAW 효율을 함께 만든다)
# ─────────────────────────────────────────────────────
./exe_TrigStudy TTTo2L2Nu 1 &
./exe_TrigStudy ttTohadronic 1 &
./exe_TrigStudy TTToSemiLeptonic 1 &

./exe_TrigStudy SingleMuon_B 1 &
./exe_TrigStudy SingleMuon_C 1 &
./exe_TrigStudy SingleMuon_D 1 &
./exe_TrigStudy SingleMuon_E 1 &
./exe_TrigStudy SingleMuon_F 1 &

wait

# ─────────────────────────────────────────────────────
# 2) 산출물 정리
# ─────────────────────────────────────────────────────
timestamp="250915_TriggerStep02_ApplySFs"
outdir="output/${timestamp}"
plotter_sf="PlotTriggerEfficiency.cpp"   # 새 플로터(사후 곱셈)
mkdir -p "${outdir}"
mv corrected_*.root "${outdir}"

# 새 플로터를 작업 디렉토리에 복사
cp "${plotter_sf}" "${outdir}"

cd "${outdir}"

# ─────────────────────────────────────────────────────
# 3) 병합: SingleMuon → corrected_Data.root, ttJets → corrected_ttJets.root
# ─────────────────────────────────────────────────────
hadd -f corrected_Data.root corrected_SingleMuon_*.root
hadd -f corrected_ttJets.root corrected_TTToSemiLeptonic.root corrected_TTTo2L2Nu.root corrected_ttTohadronic.root

# 호환성용 이름 (optional)
cp corrected_ttJets.root ttJets.root

# 작업 폴더
mkdir -p merger
mv corrected_Data.root corrected_ttJets.root "${plotter_sf}" merger/
cd merger

# ─────────────────────────────────────────────────────
# 4) 플롯 + SF 파일 생성 (사후 곱셈)
#    인자:
#      1) Data 파일(SingleMuon 병합)
#      2) SF를 측정할 MC 파일(ttJets 병합)
#      3) SF를 적용할 MC 파일(비우면 2번과 동일; 여기선 클로저용으로 동일 파일 사용)
# ─────────────────────────────────────────────────────
root -l -b -q "${plotter_sf}(\"corrected_Data.root\",\"corrected_ttJets.root\",\"corrected_ttJets.root\")"
