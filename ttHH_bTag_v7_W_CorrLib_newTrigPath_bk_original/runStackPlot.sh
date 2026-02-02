#!/bin/bash
###############################################
# File: runStackPlot.sh
# Usage: ./runStackPlot.sh
# Purpose:
#   - makeBTagStackPlot.C 매크로를 실행하여
#     모든 샘플의 stack plot을 생성
###############################################

echo "============================================="
echo "Creating B-tag SF Stack Plots..."
echo "============================================="

# plots 디렉토리 생성
mkdir -p plots

# ROOT 매크로 실행
##root -l -b -q "makeBTagStackPlot.C"
root -l -b -q "makeBTagStackPlot_Final.C"

echo ""
echo "============================================="
echo "Stack plots created successfully!"
echo "Check the 'plots' directory for output files."
echo "============================================="
echo ""
echo "Created plots:"
echo "  - BTagStackPlot_nJets_noSF.pdf"
echo "  - BTagStackPlot_nJets_withSF.pdf"
echo "  - BTagStackPlot_nJets_reweight.pdf"
echo "  - BTagStackPlot_HT_noSF.pdf"
echo "  - BTagStackPlot_HT_withSF.pdf"
echo "  - BTagStackPlot_HT_reweight.pdf"
echo "  - BTagStackPlot_jet0_pT_noSF.pdf"
echo "  - BTagStackPlot_jet0_pT_withSF.pdf"
echo "  - BTagStackPlot_jet0_pT_reweight.pdf"
echo "  - BTagStackPlot_jet0_bTag_noSF.pdf"
echo "  - BTagStackPlot_jet0_bTag_withSF.pdf"
echo "  - BTagStackPlot_jet0_bTag_reweight.pdf"
echo "  - BTagStackPlot_jet1_pT_noSF.pdf"
echo "  - BTagStackPlot_jet1_pT_withSF.pdf"
echo "  - BTagStackPlot_jet1_pT_reweight.pdf"
echo "  - BTagStackPlot_jet1_bTag_noSF.pdf"
echo "  - BTagStackPlot_jet1_bTag_withSF.pdf"
echo "  - BTagStackPlot_jet1_bTag_reweight.pdf"
echo "============================================="
