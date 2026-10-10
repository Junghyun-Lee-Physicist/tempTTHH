#include "tnm.h"
#include <cmath> 
#include <algorithm>
#include <vector>
#include <map>
#include <memory>   // [STEP 24] requireSameBranchSet_
#include "TVector3.h"
#include "ttHHanalyzer_unified.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>   // [tt+nb] std::getenv for EXPANDED_TTBARID_DIR override
#include "ExitCodes.h"    // [STEP18] canonical exit codes
#include "ConfigPath.h"   // [STEP18] cfgpath::resolve (no code-default)

#include "correction.h"

#include "Logger.h"
using namespace Logger;

// ───────────────────────────────────────────────────────────────────────────
// [2018] objectJet 의 DeepJet WP static 정의.
//   값은 EraConfig::btagWP() 에서 objectJet::configureBtagWP() 로 주입한다.
//   여기서는 **의도적으로 NaN** 으로 시작한다. 0 으로 두면 주입을 빠뜨렸을 때
//   모든 jet 이 b-tag 로 분류되면서도 프로그램이 멀쩡히 도는(=무증상) 상태가
//   된다. NaN 이면 비교가 전부 false 가 되어 즉시 눈에 띄고,
//   assertBtagWPConfigured() 가 그보다 먼저 잡는다.
// ───────────────────────────────────────────────────────────────────────────
float objectJet::valbTagTight  = std::numeric_limits<float>::quiet_NaN();
float objectJet::valbTagMedium = std::numeric_limits<float>::quiet_NaN();
float objectJet::valbTagLoose  = std::numeric_limits<float>::quiet_NaN();
bool  objectJet::bTagWPConfigured = false;

// ───────────────────────────────────────────────────────────────────────────
// B-tag Event Weight 계산 (Shape Correction 방식)
// ───────────────────────────────────────────────────────────────────────────
void ttHHanalyzer_unified::computeBTagWeight(event* thisEvent) {
    
    // 초기화
    bTagWeight_central_ = 1.0;
    bTagWeight_up_hf_ = 1.0;
    bTagWeight_down_hf_ = 1.0;
    bTagWeight_up_lf_ = 1.0;
    bTagWeight_down_lf_ = 1.0;
    bTagWeight_up_cferr1_ = 1.0;
    bTagWeight_down_cferr1_ = 1.0;
    bTagWeight_up_cferr2_ = 1.0;
    bTagWeight_down_cferr2_ = 1.0;
    bTagWeight_oldRule_ = 1.0f;   // [STEP 27 N]
    bTagWeight_cAsB_    = 1.0f;
    
    // Data는 SF 적용하지 않음
    if (_DataOrMC == "Data") return;
    // [STEP 25 K] 2024: the fixed-WP method (method 1a with L and M; D-2026-10-08-A) when the payload SF and our
    //   efficiency JSON are loaded; without the JSON the weights stay 1 (--btagsf on then stops, E52)
    if (_btagFixedWP) {
        if (_btagFixedWPReady) computeBTagWeightFixedWP_(thisEvent);
        return;
    }
    // [STEP 24] no b-tag shape SF for this year: every weight stays 1
    if (!_hasBTagShapeSF) return;
    
    // 선택된 모든 jet에 대해 SF 계산 — the central weight for every event (the cut-flow chains use it).
    // [STEP 27 O] The per-source variations are computed for the events that reach the tree only
    //   (computeBTagShapeVariations_, BTV's recipe); before, this loop varied hf on b jets only and lf on light jets
    //   only, values that were never written anywhere.
    const auto* jets = thisEvent->getSelJets();
    for (const auto* jet : *jets)
        bTagWeight_central_ *= corrMgr->getBTagSF_Shape(jet->hadFlav, jet->getp4()->Eta(), jet->getp4()->Pt(),
                                                        jet->bTagCSV, "central");
    if (debugCorrections)
        std::cout << "[BTagWeight] central=" << bTagWeight_central_ << std::endl;
}

// ───────────────────────────────────────────────────────────────────────────
// [STEP 27 O] Run 2 deepJet shape SF per uncertainty source (AN-2022/122 Table 53: hf, lf, hfstats1/2, lfstats1/2,
//   cferr1/2; jes goes with the JES runs). BTV's recipe (the same as bTagSF_ReweightStudy's resolveJetSystematic):
//   a c jet takes the variation for cferr1/2 and its central SF otherwise; a b or light jet takes the variation for
//   every source but cferr1/2 (the payload knows what each source does to each flavour). Tree branches
//   bTagWeight_<source>_up/_down. The b-tag norm reweight of each variation is not applied here (it is derived per
//   variation in the reweight study, AN §5.3) -- downstream multiplies it in.
// ───────────────────────────────────────────────────────────────────────────
void ttHHanalyzer_unified::computeBTagShapeVariations_(event* thisEvent) {
    double w[8][2];
    for (auto& r : w) r[0] = r[1] = 1.0;
    for (const auto* jet : *thisEvent->getSelJets()) {
        const double pt = jet->getp4()->Pt(), eta = jet->getp4()->Eta(), disc = jet->bTagCSV;
        const int flav = jet->hadFlav;
        const bool isC = (flav == 4);
        const double c = corrMgr->getBTagSF_Shape(flav, eta, pt, disc, "central");
        for (int k = 0; k < 8; ++k) {
            const bool cferr = (k >= 6);
            if (cferr != isC) { w[k][0] *= c; w[k][1] *= c; continue; }
            w[k][0] *= corrMgr->getBTagSF_Shape(flav, eta, pt, disc, std::string("up_") + kBTagShapeSrc[k]);
            w[k][1] *= corrMgr->getBTagSF_Shape(flav, eta, pt, disc, std::string("down_") + kBTagShapeSrc[k]);
        }
    }
    for (int k = 0; k < 8; ++k)
        for (int v = 0; v < 2; ++v) _tv.bTagShapeVar[k][v] = static_cast<float>(w[k][v]);
    // the old members follow (hf, lf, cferr1, cferr2), so nothing holds a value of the old recipe
    bTagWeight_up_hf_ = _tv.bTagShapeVar[0][0];     bTagWeight_down_hf_ = _tv.bTagShapeVar[0][1];
    bTagWeight_up_lf_ = _tv.bTagShapeVar[1][0];     bTagWeight_down_lf_ = _tv.bTagShapeVar[1][1];
    bTagWeight_up_cferr1_ = _tv.bTagShapeVar[6][0]; bTagWeight_down_cferr1_ = _tv.bTagShapeVar[6][1];
    bTagWeight_up_cferr2_ = _tv.bTagShapeVar[7][0]; bTagWeight_down_cferr2_ = _tv.bTagShapeVar[7][1];
}

// ───────────────────────────────────────────────────────────────────────────
// [STEP 25 K] B-tag event weight, fixed-WP method (2024; docs/DECISIONS.md D-2026-10-08-A)
//   BTV "method 1a" with the two WPs the selection uses: a selected jet is a b jet when its score >= M
//   (selectbJet) and a light jet when its score < L (selectLightJet, the hadronic W), so each jet is in one of
//   three bins and its weight is P_Data(bin) / P_MC(bin):
//     score >= M      : SF_M e_M / e_M                              = SF_M
//     L <= score < M  : (SF_L e_L - SF_M e_M) / (e_L - e_M)   [STEP 27 N: 1 when the numerator is < 0, BTV's rule]
//     score < L       : (1 - SF_L e_L) / (1 - e_L)              [a numerator < 0 stays 0, a question to BTV]
//   e_L, e_M: our MC efficiency for the jet's flavour, pT, |eta| and process group (getBTagEff); SF: the payload
//   (getBTagSF_WP; 2024 b jets only -> 1 for c and light jets, whose weight is then exactly 1). The event weight
//   is the product over the selected jets; it keeps the pre-tag normalization on average (the closure line at
//   the end of the job). up/down: the payload's b-jet SF variation (in the hf slots; lf/cferr stay central).
// ───────────────────────────────────────────────────────────────────────────
void ttHHanalyzer_unified::computeBTagWeightFixedWP_(event* thisEvent) {
    const float wpL = objectJet::valbTagLoose, wpM = objectJet::valbTagMedium;
    auto clampEff = [](double e) { return std::min(std::max(e, 1.0e-4), 1.0 - 1.0e-4); };
    double w[3] = {1.0, 1.0, 1.0};                                    // central, up, down (BTV's rule)
    double wOld = 1.0, wCB = 1.0;                                     // [STEP 27 N] comparison weights (central)
    static const char* syst[3] = {"central", "up", "down"};
    // method 1a for one jet. interNegIsOne: BTV's rule (an intermediate numerator < 0 -> 1; D-2026-10-10-A (1)),
    //   false = the commit-K rule (-> 0). A negative fail-L numerator (SF_L e_L > 1) is 0 in both.
    auto jetWeight = [&](double s, double sL, double sM, double eL, double eM, bool interNegIsOne,
                         bool& interNeg) -> double {
        interNeg = false;
        if (s >= wpM) return sM;
        if (s >= wpL) {
            const double pmc = eL - eM;
            if (!(pmc > 1.0e-6)) return 1.0;
            const double num = sL * eL - sM * eM;
            if (num < 0.0) { interNeg = true; return interNegIsOne ? 1.0 : 0.0; }
            return num / pmc;
        }
        const double pmc = 1.0 - eL;
        if (!(pmc > 1.0e-6)) return 1.0;
        return std::max(0.0, 1.0 - sL * eL) / pmc;
    };
    for (const auto* jet : *thisEvent->getSelJets()) {
        const int f = jet->hadFlav;
        const double pt = jet->getp4()->Pt();
        const double ae = std::abs(jet->getp4()->Eta());
        const double s  = jet->bTagCSV;
        // [STEP 27 N] "c jets with the b SF" (comparison): a c jet takes the b-jet SF (flavour 5) and our c-jet
        //   efficiencies, while the payload has no c SF of its own
        if (f == 4 && !corrMgr->btagFixedWPCovers(4) && corrMgr->btagFixedWPCovers(5)) {
            const double sL = corrMgr->getBTagSF_WP(5, ae, pt, "L", "central");
            const double sM = corrMgr->getBTagSF_WP(5, ae, pt, "M", "central");
            const double eL = clampEff(corrMgr->getBTagEff(_btagEffGroup, 4, ae, pt, "L"));
            const double eM = std::min(clampEff(corrMgr->getBTagEff(_btagEffGroup, 4, ae, pt, "M")), eL);
            bool in = false;
            const double wc = jetWeight(s, sL, sM, eL, eM, true, in);
            wCB *= wc;
            ++_btagNJetCAsB;
            if (wc > 10.0) ++_btagNBigCAsB;   // [STEP 27 P] the c maps are not required at load (review R4)
        }
        if (!corrMgr->btagFixedWPCovers(f)) continue;   // no SF for this flavour (2024: c, light): weight exactly 1
        const double sL0 = corrMgr->getBTagSF_WP(f, ae, pt, "L", "central");
        const double sM0 = corrMgr->getBTagSF_WP(f, ae, pt, "M", "central");
        const double eL = clampEff(corrMgr->getBTagEff(_btagEffGroup, f, ae, pt, "L"));
        const double eM = std::min(clampEff(corrMgr->getBTagEff(_btagEffGroup, f, ae, pt, "M")), eL);
        for (int v = 0; v < 3; ++v) {
            const double sL = v == 0 ? sL0 : corrMgr->getBTagSF_WP(f, ae, pt, "L", syst[v]);
            const double sM = v == 0 ? sM0 : corrMgr->getBTagSF_WP(f, ae, pt, "M", syst[v]);
            bool interNeg = false;
            const double wj = jetWeight(s, sL, sM, eL, eM, true, interNeg);
            w[v] *= wj;
            if (v == 0) {
                wCB *= wj;                                             // b jets: as the main weight
                bool dummy = false;
                wOld *= jetWeight(s, sL, sM, eL, eM, false, dummy);    // the commit-K rule
            }
            // the end-of-job [btagSF] lines count the jet weights that point at maps that do not fit (> 10), the
            //   P_Data < 0 cases set to 0 (fail L: SF x e > 1, likely for the up variation of a high-efficiency
            //   bin) and the intermediate ones set to 1 (BTV's rule)
            if (v == 0 && wj > 10.0) ++_btagNBig;
            if (wj == 0.0) ++_btagNZero[v];
            if (interNeg) ++_btagNInterNeg[v];
        }
        ++_btagNJet;
    }
    bTagWeight_central_ = static_cast<float>(w[0]);
    bTagWeight_up_hf_   = static_cast<float>(w[1]);
    bTagWeight_down_hf_ = static_cast<float>(w[2]);
    bTagWeight_up_lf_ = bTagWeight_down_lf_ = bTagWeight_central_;
    bTagWeight_up_cferr1_ = bTagWeight_down_cferr1_ = bTagWeight_central_;
    bTagWeight_up_cferr2_ = bTagWeight_down_cferr2_ = bTagWeight_central_;
    bTagWeight_oldRule_ = static_cast<float>(wOld);
    bTagWeight_cAsB_    = static_cast<float>(wCB);
    if (debugCorrections)
        std::cout << "[BTagWeight][fixed WP] central=" << w[0] << " up=" << w[1] << " down=" << w[2]
                  << " oldRule=" << wOld << " cAsB=" << wCB << std::endl;
}

// ───────────────────────────────────────────────────────────────────────────
// [STEP 25 K] b-tag efficiency histograms (MC of a fixed-WP year, not prescan; tools/stage7/btag_eff_maps.py
//   makes the maps from them)
//   BTagEff/h2_<b|c|l>_<all|L|M|T>: selected jets in pT x |eta|, all of them and those with score >= the WP,
//   filled at the HT step (every cut but the b-tag ones; main: after the hadronic trigger and the lepton veto or
//   the CR lepton, btagtrig: neither of the two) with the event weight before the SFs (base x PU x genW x
//   stitch). TH2D, Sumw2. BTagEff/h_nevt: the number of events filled.
// ───────────────────────────────────────────────────────────────────────────
void ttHHanalyzer_unified::bookBTagEff_() {
    if (!_btagFixedWP || _DataOrMC == "Data" || _analysisMode == AnalysisMode::kPrescan) return;
    TDirectory* keep = gDirectory;
    _of->file->cd();
    _btagEffDir = _of->file->mkdir("BTagEff");
    _btagEffDir->cd();
    static const double ptEdges[] = {20, 30, 40, 50, 60, 80, 100, 130, 170, 220, 300, 400, 600, 1000};
    static const double etaEdges[] = {0.0, 0.6, 1.2, 1.8, 2.5};
    const int nPt = sizeof(ptEdges) / sizeof(ptEdges[0]) - 1, nEta = sizeof(etaEdges) / sizeof(etaEdges[0]) - 1;
    static const char* fl[3] = {"b", "c", "l"};
    static const char* wp[4] = {"all", "L", "M", "T"};
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 4; ++j) {
            const TString name = TString::Format("h2_%s_%s", fl[i], wp[j]);
            const TString title = TString::Format("%s jets, %s;p_{T} [GeV];|#eta|", fl[i],
                                                  j == 0 ? "all" : TString::Format("score >= %s", wp[j]).Data());
            _hBTagEff[i][j] = new TH2D(name, title, nPt, ptEdges, nEta, etaEdges);
            _hBTagEff[i][j]->Sumw2();
        }
    // the events filled (unweighted): tools/stage7/btag_eff_maps.py compares it with the HT>500 bin of Tree/cutflow
    //   of the same merged file -- equal when every job of the sample filled these histograms (an output of an older
    //   executable has none)
    _hBTagEffNevt = new TH1D("h_nevt", "events filled (the HT step);;events", 1, 0.0, 1.0);
    // the WPs of the year x the jobs (bin 4 = 1 per job): hadd adds them, so the merged file has WP = bin / bin 4;
    //   tools/stage7/btag_eff_maps.py puts them into the maps' description and the analyzer checks them at load
    _hBTagEffWP = new TH1D("h_wp", "b-tag WPs (L, M, T) x jobs; ; ", 4, 0.0, 4.0);
    _hBTagEffWP->SetBinContent(1, objectJet::valbTagLoose);
    _hBTagEffWP->SetBinContent(2, objectJet::valbTagMedium);
    _hBTagEffWP->SetBinContent(3, objectJet::valbTagTight);
    _hBTagEffWP->SetBinContent(4, 1.0);
    // [STEP 27 N] the closure per number of true b jets and per M-tag multiplicity, for the main weight and the two
    //   comparison weights (D-2026-10-10-A (2), (4)); hadd adds them, ratios are taken after the merge
    static const char* wn[4] = {"w", "wb", "wb_oldRule", "wb_cAsB"};
    static const char* wt[4] = {"sum w", "sum w x bTagWeight", "sum w x bTagWeight_oldRule", "sum w x bTagWeight_cAsB"};
    for (int k = 0; k < 4; ++k) {
        _hBTagTrueB[k] = new TH1D(TString::Format("h_ntrueb_%s", wn[k]),
                                  TString::Format("HT step: %s;true b jets among the selected (4 = #geq 4);", wt[k]), 5, 0.0, 5.0);
        _hBTagNbM[k]   = new TH1D(TString::Format("h_nbM_%s", wn[k]),
                                  TString::Format("HT step: %s;b-tagged (#geq M) jets (6 = #geq 6);", wt[k]), 7, 0.0, 7.0);
        _hBTagTrueB[k]->Sumw2();
        _hBTagNbM[k]->Sumw2();
    }
    if (keep) keep->cd();
}

// [STEP 27 N] at the HT step (applyEventScaleFactors, before any b-tag cut), MC of a fixed-WP year with the weight ready
void ttHHanalyzer_unified::fillBTagClosure_(event* thisEvent) {
    if (!_btagEffDir || !_btagFixedWPReady) return;
    int ntb = 0;
    for (const auto* j : *thisEvent->getSelJets()) ntb += (j->hadFlav == 5);
    const int itb = std::min(ntb, 4), inb = std::min(thisEvent->getnSelbJet(), 6);
    const double w = _evtWeight;
    const double ws[4] = {w, w * bTagWeight_central_, w * bTagWeight_oldRule_, w * bTagWeight_cAsB_};
    for (int k = 0; k < 4; ++k) {
        _hBTagTrueB[k]->Fill(itb + 0.5, ws[k]);
        _hBTagNbM[k]->Fill(inb + 0.5, ws[k]);
    }
    _btagTrueBW[itb]  += w;
    _btagTrueBWB[itb] += ws[1];
    _btagClosureWOld  += ws[2];
    _btagClosureWCB   += ws[3];
}

void ttHHanalyzer_unified::fillBTagEff_(event* thisEvent) {
    if (!_btagEffDir) return;
    _hBTagEffNevt->Fill(0.5);
    const float wp[3] = {objectJet::valbTagLoose, objectJet::valbTagMedium, objectJet::valbTagTight};
    for (const auto* jet : *thisEvent->getSelJets()) {
        const int i = (jet->hadFlav == 5) ? 0 : (jet->hadFlav == 4) ? 1 : 2;
        const double pt = jet->getp4()->Pt(), ae = std::abs(jet->getp4()->Eta()), s = jet->bTagCSV;
        _hBTagEff[i][0]->Fill(pt, ae, _evtWeight);
        for (int j = 0; j < 3; ++j)
            if (s >= wp[j]) _hBTagEff[i][j + 1]->Fill(pt, ae, _evtWeight);
    }
}

void ttHHanalyzer_unified::performAnalysis(){

    if (_analysisMode == AnalysisMode::kPrescan) {
        // ⚠ TEMPORARY mode — migrate to NtupleForge eventually.
        // Skips the full selection/object pipeline; pure gen-level counter.
        runPrescan();
    } else {
        loop(noSys, false);
    }

    _of->file->Close();

}

void ttHHanalyzer_unified::loop(sysName sysType, bool up){

    // [STEP3][debug] kDebug 모드: cut 테이블 무결성 검사 + 덤프.
    // kCutSequence가 CutStep enum 순서와 _cutStepLabels에 정확히 1:1인지
    // 시작 시 검증한다 (표/라벨/enum의 3중 정의가 어긋나는 사고 방지).
    if (_dbg.on()) {
        std::printf("[dbg][seltable] ── kCutSequence (%zu entries) ──\n", kCutSequence.size());
        bool tableOK = true;
        for (size_t i = 0; i < kCutSequence.size(); ++i) {
            const auto& c = kCutSequence[i];
            const int idx = static_cast<int>(c.step);
            const bool stepOK  = (idx == static_cast<int>(i));
            const bool labelOK = (idx < static_cast<int>(_cutStepLabels.size()) &&
                                  _cutStepLabels.at(idx) == c.label);
            std::printf("[dbg][seltable]  %2zu %-16s enforce=0x%X %s%s%s\n",
                        i, c.label, c.enforceIn,
                        c.recordOnlyIfPass ? "(관찰:충족시기록) " : "",
                        stepOK ? "" : "[STEP-ORDER MISMATCH!] ",
                        labelOK ? "" : "[LABEL MISMATCH vs _cutStepLabels!]");
            tableOK = tableOK && stepOK && labelOK;
        }
        std::printf("[dbg][seltable] integrity: %s\n", tableOK ? "OK" : "** BROKEN — 위 항목 확인 **");

        // [STEP4][debug] 경로 통제 env 상태 덤프 — yml→condor sh→env 주입이
        // 실제로 전달됐는지 로컬/condor 로그에서 즉시 확인할 수 있다.
        const char* pathEnvs[] = { "TTHH_JSONPOG_PATH", "TTHH_GOLDENJSON_PATH",
                                   "TTHH_TRIGSF_DIR",   "TTHH_BTAGRW_JSON",
                                   "STITCH_FACTORS_JSON", "EXPANDED_TTBARID_DIR",
                                   "TTHH_PU_JSON" };   // [STEP 24]
        for (const char* pe : pathEnvs) {
            const char* v = std::getenv(pe);
            std::printf("[dbg][paths] %-22s = %s\n", pe, (v && *v) ? v : "(unset/__NULL__)");
        }
    }


    int nevents = _ev->size();
    // nevents = 100; // DEBUG: removed - run all events for full cross-validation

    //std::cout<<"weight = "<<_weight<<std::endl;  
    std::cout << "Base Weight = " << _baseWeight << std::endl; // 시작 시 base weight(xsec×lumi/Σgenw) 확인용
    _SampleWeight = _baseWeight; // Tree branch에 기록되는 base weight (SF 곱하기 전 — evtWeight와 구분)

    cout<<endl;
    print("This analyzer commented out [ \"WTF\" log ] in the header, Please check if you want!!!", "magenta", "warning");

    cout<<endl;
    print("--------------------------------------------------------------------------", "b");
    print("Before start, Let's check the analysis information", "b");
    print("Run Year    ----> [  " + _runYear + "  ]", "b");
    print("Data or MC  ----> [  " + _DataOrMC + "  ]", "b");
    print("Sample Name ----> [  " + _sampleName + "  ]", "b");
    
    string checklist = "[ tnm.cc ] & [ analyzer header ] & [ main ] & [ analyzer constructor ]";
    bool exitFlag = false;
    if(_runYear == "nothing"){
        print("RunYear is not defined, Please check the" + checklist, "r", "error");
        exitFlag = true;
    }
    if(_DataOrMC == "nothing"){
        print("Whether Data or MC is not defined, Please check the" + checklist, "r", "error");
        exitFlag = true;
    }
    if(_sampleName == "nothing"){
        print("SampleName is not defined, Please check the" + checklist, "r", "error");
        exitFlag = true;
    }
    print("--------------------------------------------------------------------------", "b");
    cout<<endl;
    if(exitFlag) tthh::fatalExit(tthh::CONFIG_BAD_RUNINFO);

    // ── [stitch] setup report + Condor-catchable prerequisite check ──────────
    // Only when the stitch JSON was loaded (main / btagtrig). trigsf, validation
    // run stitch-free, so this block is skipped there.
    if (_stitch.loaded()) {
        _stitch.printConfigSummary();
        if (_stitch.inPlan() && !_expTtbarId.active()) {
            std::cerr << "\n[FATAL][E" << tthh::STITCH_EXPTTID_INACTIVE << "][stitch] sample '" << _sampleName
                      << "' is in the stitch plan (role " << _stitch.role() << ") but the Expanded_genTtbarId "
                      << "lookup is INACTIVE (no ttnb_<sample>.root loaded).\n"
                      << "  tt+nb (61/62/71/72) would never be tagged -> wrong stitch.\n"
                      << "  Provide the lookup (DerivedCorr/expandedTtbarId or "
                      << "$EXPANDED_TTBARID_DIR). Aborting (exit 63).\n" << std::endl;
            tthh::fatalExit(tthh::STITCH_EXPTTID_INACTIVE);
        }
    }

    if(debugCorrections) std::cout<<"debug : Before begin the entry.."<<std::endl;
    std::string analysisInfo = _runYear + ", " + _DataOrMC + ", " + _sampleName;

    for(int entry=0; entry < nevents; entry++){
        event * currentEvent = new event;
        _ev->read(entry);       // read an event into event buffer
        process(currentEvent, sysType, up);

        if (entry % 10000 == 0){
        //if (entry % 1 == 0){
            print("Processed events of " + analysisInfo + ": " + to_string(entry) ,"c");
            currentEvent->summarize();
        }

        delete currentEvent;
    }

    
    _expTtbarId.printSummary();   // [tt+nb] hit/miss + genTtbarId self-check summary

    // [stitch] per-category Sum(weight) before/after the multiplier, so the log
    // shows exactly what the stitch did to this sample's composition.
    // (main / btagtrig only; skipped when the JSON was not loaded.)
    if (_stitch.loaded()) _stitch.printRunSummary();
    _dbg.summary();   // [STEP2][debug] kDebug 종료 요약 (cutflow + 값 집계 + NaN/Inf)

    // [stitch] diagnostic — 이 샘플의 expandedTtbarId%100 → b-tag reweight
    // processKey 매핑. [STEP7.5]에서 MakeProcessKey가 tt+nb(61/62/71/72)를
    // "tt+nb" 그룹으로 분리하도록 확장됨 (ttHH AN D.0.5 근거) — 아래 진단은
    // 이제 YES를 출력해야 정상이다. NO가 나오면 옛 헤더로 빌드된 것.
    if (!_btagKeyByExpSub.empty()) {
        std::cout << "[stitch] b-tag reweight processKey by expandedTtbarId%100 "
                  << "(sample=" << _sampleName << "):\n";
        std::string key2b;
        for (const auto& kv : _btagKeyByExpSub) {
            std::cout << "    sub=" << std::setw(3) << kv.first << " -> " << kv.second << "\n";
            if (kv.first >= 53 && kv.first <= 55 && key2b.empty()) key2b = kv.second;
        }
        bool nbSplit = true;
        for (const auto& kv : _btagKeyByExpSub)
            if (kv.first >= 61 && !key2b.empty() && kv.second == key2b) nbSplit = false;
        std::cout << "    -> tt+nb split from tt+2b in the b-tag key: "
                  << (nbSplit ? "YES" : "NO (extend MakeProcessKey if AN requires it)")
                  << "\n" << std::endl;
    }

    if(debugCorrections) std::cout<<"debug : Before [ writeHistos() ]"<<std::endl;
    writeHistos();
    if(debugCorrections) std::cout<<"debug : After writeHistos() & Before writeTree()"<<std::endl;
    writeTree();
    if(debugCorrections) std::cout<<"debug : After writeTree() & Before hcutFlow()"<<std::endl;

    // 종료 시 cutflow 요약을 stdout으로 출력 (히스토그램과 별개의 텍스트 확인용)
    // [STEP 24] event cleaning, item by item (the noiseFilter cut step holds both; PLAN 9.1 Stage 2)
    std::cout << "[cleaning] events " << _nCleanAll << ": MET filters fail " << _nCleanMETFail
              << ", jet veto map " << (_useJetVetoMap ? std::to_string(_nCleanVetoFail) : std::string("not used"))
              << " (before the trigger; the two can overlap)" << std::endl;
    // [D-2026-10-06-A] the hadronic trigger, per path group (the b-tag group = 4J3T, 6J1T, 6J2T as the year uses
    //   them) and what the Data/MC rule took (in main/debug = the HadTrigger step of the cutflow, where the trigger
    //   is enforced; btagtrig only records the step, so its HadTrigger count is every event)
    std::cout << "[trigger] events " << _nTrigAll << ": HLT_PFHT1050 " << _nTrigHT << ", b-tag paths " << _nTrigBtag
              << ", both " << _nTrigBoth << ", b-tag without HLT_PFHT1050 " << (_nTrigBtag - _nTrigBoth)
              << "; taken " << _nTrigTaken << " (" << _DataOrMC << " " << _sampleName << ")" << std::endl;
    // [STEP 25 K] the fixed-WP b-tag weight keeps the pre-tag normalization: over the events at the HT step (before
    //   any b-tag cut) sum(w x bTagWeight) / sum(w) is about 1 when the efficiency maps fit this sample
    if (_btagFixedWPReady) {
        std::cout << "[btagSF] closure at the HT step (no b-tag cut yet): events " << _btagClosureN
                  << ", sum(w x bTagWeight) / sum(w) = "
                  << (_btagClosureW != 0.0 ? _btagClosureWB / _btagClosureW : 0.0)
                  << " (efficiency group '" << _btagEffGroup << "'; about 1 expected)" << std::endl;
        std::cout << "[btagSF] jets weighted (b, all events): " << _btagNJet << "; jet weight > 10: " << _btagNBig
                  << "; set to 0 (P_Data < 0): central " << _btagNZero[0] << ", up " << _btagNZero[1] << ", down "
                  << _btagNZero[2] << std::endl;
        // [STEP 27 N] D-2026-10-10-A: BTV's intermediate rule, the closure per true b jets, the comparison weights
        std::cout << "[btagSF] intermediate (L <= score < M) numerator < 0 -> jet weight 1 (BTV): central "
                  << _btagNInterNeg[0] << ", up " << _btagNInterNeg[1] << ", down " << _btagNInterNeg[2] << std::endl;
        std::cout << "[btagSF] closure per true b jets at the HT step, sum(w x bTagWeight) / sum(w):";
        for (int k = 0; k < 5; ++k)
            std::cout << " " << (k == 4 ? ">=4" : std::to_string(k)) << "b "
                      << (_btagTrueBW[k] != 0.0 ? std::to_string(_btagTrueBWB[k] / _btagTrueBW[k]) : std::string("-"));
        std::cout << " (each about 1 when the maps fit; BTagEff/h_ntrueb_*)" << std::endl;
        std::cout << "[btagSF] comparison weights, closure at the HT step: oldRule (intermediate < 0 -> 0) "
                  << (_btagClosureW != 0.0 ? _btagClosureWOld / _btagClosureW : 0.0)
                  << ", cAsB (c jets with the b SF; " << _btagNJetCAsB << " c jets weighted in all events, "
                  << _btagNBigCAsB << " with a jet weight > 10) "
                  << (_btagClosureW != 0.0 ? _btagClosureWCB / _btagClosureW : 0.0) << std::endl;
    }
    std::cout << "=== CutFlow Summary ===" << std::endl;
    for (size_t i = 0; i < _cutStepLabels.size(); ++i) {
        std::cout << _cutStepLabels[i] 
                  << " : " << _cutFlowCount[i] 
                  << " (weighted: " << _cutFlowWeight[i] << ")" 
                  << std::endl;
        
        // (hCutFlow는 processStep에서 직접 Fill되므로 여기서 SetBinContent 하지 않음)
    }

    hCutFlow         ->Write();
    if(debugCorrections) std::cout<<"debug : After hCutFlow() & Before SF-aware cutflows"<<std::endl;
    hCutFlow_w       ->Write();
    hCutFlow_w_btagSF->Write();
    hCutFlow_w_full  ->Write();
    if(debugCorrections) std::cout<<"debug : After SF-aware cutflows"<<std::endl;

}

// ═══════════════════════════════════════════════════════════════════════════
// createObjects — NanoAOD branch → 분석 객체(jet/lepton) 생성.
//   jets: JES/JER(sysType,up) 적용 → pT/η/ID/PUid (Cuts::jet*) → selectJet
//         (내부에서 DeepJet M=valbTagMedium으로 b-jet 분류)
//   leptons: veto 정의(Cuts::subLead*)로 수집; btagtrig은 lead-muon gate 추가
// ═══════════════════════════════════════════════════════════════════════════
// ═══════════════════════════════════════════════════════════════════════════
// [2018] 2018 hadronic HLT branch 존재 확인 (첫 이벤트 1회)
//
//  eventBuffer 는 입력에 없는 branch 를 initBuffers() 에서 0 으로 두고 그냥
//  넘어간다. trigger 에서 0 은 "안 터졌다"와 형태가 같으므로, 경로 이름이
//  하나라도 틀리거나 NanoAOD 버전이 다르면 **크래시도 경고도 없이 통과
//  이벤트가 0 이 된다.** 히스토그램이 비어 있는 걸 사람이 눈으로 발견할
//  때까지 아무도 모른다.
//
//  그래서 "요청했고(choose) 실제로 붙었는가(present)"를 명시적으로
//  확인하고, 하나라도 없으면 즉시 FATAL 로 끊는다.
//
//  [2026-07-29 수정] 처음에는 `_ev->successBranches` 를 봤는데 컴파일이 안 된다.
//    그 벡터는 eventBuffer 의 **멤버가 아니라** `select()` 안의 지역 변수다
//    (include/eventBuffer.h:6537 선언 → :10110 에서 리포트 출력 후 소멸).
//    그래서 select() 가 끝난 뒤에는 존재하지 않는다.
//
//    대신 그 벡터에 원소가 들어가는 **조건 자체**를 재현한다. eventBuffer.h 의
//    생성 코드는 모든 branch 에 대해 정확히 이 모양이다:
//        if ( choose["<b>"] )
//          if (input->present("<b>")) { input->select(...); successBranches.push_back("<b>"); }
//          else                       { missingBranches.push_back("<b>"); }
//    즉 `choose[b] && input->present(b)` 가 successBranches 멤버십과 동치다.
//    두 조건을 모두 봐야 한다 — 하나만 보면 다음이 새어 나간다:
//      · choose=false, present=true  → select() 미호출 → 값이 계속 0
//      · choose=true,  present=false → branch 부재     → 값이 계속 0
//    둘 다 "안 터졌다"와 구별이 안 되는 무음 실패다.
//
//    ⚠ `choose` 는 std::map 이므로 operator[] 를 쓰면 없는 키를 **삽입**한다.
//      조회는 반드시 find() 로 한다.
// ═══════════════════════════════════════════════════════════════════════════
void ttHHanalyzer_unified::requireTriggerBranches2018_() {
    static bool checked = false;
    if (checked) return;
    checked = true;

    const std::vector<std::string> required = {
        "Events/HLT_PFHT1050",
        "Events/HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5",
        "Events/HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94",
        "Events/HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59",
    };

    std::vector<std::string> absentInFile;   // choose 했는데 파일에 없음
    std::vector<std::string> notChosen;      // 파일엔 있는데 eventBuffer 가 안 읽음
    for (const auto& r : required) {
        auto itc = _ev->choose.find(r);
        const bool chosen  = (itc != _ev->choose.end() && itc->second);
        const bool present = (_ev->input != nullptr) && _ev->input->present(r);
        if (!chosen)       notChosen.push_back(r);
        else if (!present) absentInFile.push_back(r);
    }

    std::vector<std::string> absent = absentInFile;
    absent.insert(absent.end(), notChosen.begin(), notChosen.end());

    if (!absent.empty()) {
        std::cerr << "\n[FATAL][E" << tthh::CONFIG_BAD_RUNINFO
                  << "] 2018 hadronic HLT branches unusable:\n";
        for (const auto& a : absentInFile)
            std::cerr << "    - " << a << "   (chosen, but NOT in the input file)\n";
        for (const auto& a : notChosen)
            std::cerr << "    - " << a << "   (in eventBuffer 'choose'? NO -- "
                                            "add it to include/eventBuffer.h)\n";
        std::cerr
            << "  These are REQUIRED for runYear=2018. A missing HLT branch reads\n"
            << "  back as 0 (= 'did not fire'), so continuing would silently select\n"
            << "  zero events instead of failing. Check the NanoAOD version and the\n"
            << "  branch list in include/eventBuffer.h.\n";
        tthh::fatalExit(tthh::CONFIG_BAD_RUNINFO);
    }

    std::cout << "[trigger] 2018 hadronic HLT branches: all "
              << required.size() << " present." << std::endl;
}

// ═══════════════════════════════════════════════════════════════════════════
// [STEP 24] Stage 1 (c), PLAN 9.3 / D-2026-10-02-B: no silent 0 for a branch the analysis reads.
//   Every branch this year's code reads (below) must be selected by the eventBuffer (choose) AND present
//   in the input file. Checked once, at the first event. treestream fixes ONE branch set for the whole
//   chain (its branch list, taken from the tree the TChain has loaded after GetEntries -- the first file
//   with ROOT 6.40, another one with ROOT 6.30 at KNU, STEP 24 section 12): a branch absent there reads 0
//   in every file, one present there but absent in a file stops the job (exit 1). So a job with several
//   files must have the same branch set in all of them (requireSameBranchSet_, which reads every file
//   itself: 2024 era C Data files differ in their HLT branches). Counters (nJet, ...) are not in
//   `choose`; their arrays are checked instead. The 2018 hadronic HLT check above is the same idea.
// ═══════════════════════════════════════════════════════════════════════════
bool ttHHanalyzer_unified::branchUsable_(const std::string& b, std::string& why) const {
    const std::string key = "Events/" + b;
    auto itc = _ev->choose.find(key);
    if (itc == _ev->choose.end() || !itc->second) { why = "not in include/eventBuffer.h (or not chosen)"; return false; }
    if (_ev->input == nullptr || !_ev->input->present(key)) { why = "not in the input file"; return false; }
    return true;
}

void ttHHanalyzer_unified::requireUsable_(const std::vector<std::string>& req,
                                          const std::vector<std::vector<std::string>>& anyOf,
                                          const std::string& label) {
    std::vector<std::string> bad;
    std::string why;
    for (const auto& b : req)
        if (!branchUsable_(b, why)) bad.push_back(b + "   (" + why + ")");
    for (const auto& g : anyOf) {
        bool one = false;
        for (const auto& b : g) if (branchUsable_(b, why)) { one = true; break; }
        if (!one) {
            std::string names;
            for (const auto& b : g) names += (names.empty() ? "" : " | ") + b;
            bad.push_back("one of: " + names + "   (none usable)");
        }
    }
    if (!bad.empty()) {
        std::cerr << "\n[FATAL][E" << tthh::CONFIG_BAD_RUNINFO << "] branches the runYear=" << _runYear
                  << " " << _DataOrMC << " " << label << " reads are not usable:\n";
        for (const auto& b : bad) std::cerr << "    - " << b << "\n";
        std::cerr << "  A branch the code reads that is not there reads back as 0, i.e. a silent wrong result\n"
                  << "  (D-2026-10-02-B). Check the NanoAOD version of the input (2018 v15 is not supported yet:\n"
                  << "  PLAN 9 Y3) and that include/eventBuffer.h was made from inputs that have these branches.\n";
        tthh::fatalExit(tthh::CONFIG_BAD_RUNINFO);
    }
    // which members of each any-of group the file has (2024 Data: the 4J3T path(s) of this file's runs)
    std::string groups;
    for (const auto& g : anyOf) {
        groups += " and one of {";
        for (size_t i = 0; i < g.size(); ++i)
            groups += (i ? ", " : "") + g[i] + (branchUsable_(g[i], why) ? ": yes" : ": no");
        groups += "}";
    }
    std::cout << "[branches] runYear=" << _runYear << " " << _DataOrMC << " (" << label << "): all " << req.size()
              << " required branches present" << groups << "." << std::endl;
}

void ttHHanalyzer_unified::requireBranches_() {
    static bool checked = false;
    if (checked) return;
    checked = true;
    const bool isMC = (_DataOrMC != "Data");
    std::vector<std::string> req = {
        "run", "luminosityBlock", "event", "PV_npvsGood",
        "Jet_pt", "Jet_eta", "Jet_phi", "Jet_mass", "Jet_rawFactor", "Jet_area",
        "Muon_pt", "Muon_eta", "Muon_phi", "Muon_tightId", "Muon_pfRelIso04_all", "Muon_charge", "Muon_miniPFRelIso_all",
        "Electron_pt", "Electron_eta", "Electron_phi", "Electron_deltaEtaSC", "Electron_charge",
        "Electron_miniPFRelIso_all", "Electron_pfRelIso03_all",
        "HLT_PFHT1050"};
    std::vector<std::vector<std::string>> anyOf;   // at least one of each group
    if (_isRun3) {
        for (const char* b : {"Rho_fixedGridRhoFastjetAll", "PuppiMET_pt", "PuppiMET_phi", "Jet_btagUParTAK4B", "Jet_chHEF", "Jet_neHEF",
                              "Jet_chEmEF", "Jet_neEmEF", "Jet_muEF", "Jet_chMultiplicity", "Jet_neMultiplicity",
                              "Electron_mvaIso_WP90", "Muon_isPFcand", "HLT_IsoMu24",
                              "HLT_PFHT450_SixPFJet36_PNetBTag0p35", "HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50"})
            req.push_back(b);
        // 4J3T (2024, D4): the DeepJet path in the first 38 runs, the PNet path after, never both
        // (docs/reference/LUMI_SOURCES.md 6.3) -> a Data file has at least one; MC has both, the PNet one is used
        if (isMC) req.push_back("HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3");
        else anyOf.push_back({"HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3",
                              "HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5"});
    } else {
        for (const char* b : {"fixedGridRhoFastjetAll", "MET_pt", "MET_phi", "Jet_jetId", "Jet_puId", "Jet_btagDeepFlavB",
                              "Electron_mvaFall17V2Iso_WP90"})
            req.push_back(b);
        if (_runYear == "2017") {
            // era B (Data) has the older CSV names only; C-F and MC the PF names only (NtupleForge inventories)
            const bool eraB = (_DataOrMC == "Data" && _era == "B");
            for (const char* b : (eraB ? std::vector<const char*>{"HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07",
                                                                   "HLT_PFHT430_SixJet40_BTagCSV_p080",
                                                                   "HLT_PFHT380_SixJet32_DoubleBTagCSV_p075"}
                                       : std::vector<const char*>{"HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0",
                                                                   "HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5",
                                                                   "HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2"}))
                req.push_back(b);
            req.push_back("HLT_IsoMu27");                 // the 2017 trigger-SF reference (fillTree)
        } else {
            req.push_back("HLT_IsoMu24");                 // the 2018 trigger-SF reference (fillTree)
        }
        if (isMC && EraConfig::usesL1Prefiring(_runYear))
            for (const char* b : {"L1PreFiringWeight_Nom", "L1PreFiringWeight_Up", "L1PreFiringWeight_Dn"})   // [STEP 27 O] up/down
                req.push_back(b);
    }
    if (isMC)
        for (const char* b : {"genWeight", "Pileup_nTrueInt", "genTtbarId", "Jet_hadronFlavour", "Jet_partonFlavour",
                              "Jet_genJetIdx", "GenJet_pt", "GenJet_eta", "GenJet_phi", "GenJet_hadronFlavour",
                              "GenPart_pdgId", "GenPart_statusFlags", "GenPart_genPartIdxMother", "GenPart_pt",
                              "GenPart_eta", "GenPart_phi", "GenPart_mass"})
            req.push_back(b);
    for (const auto& f : EraConfig::metFilters(_runYear)) req.push_back(f);
    requireUsable_(req, anyOf, "analysis code");
    requireSameBranchSet_();
}

void ttHHanalyzer_unified::requirePrescanBranches_() {
    std::vector<std::string> req = {"run", "luminosityBlock", "event"};
    if (_DataOrMC != "Data") { req.push_back("genWeight"); req.push_back("genTtbarId"); }
    requireUsable_(req, {}, "prescan");
    requireSameBranchSet_();
}

void ttHHanalyzer_unified::requireSameBranchSet_() {
    const std::vector<std::string> files = _ev->input->filenames();
    // [STEP 24, KNU smoke 2026-10-05] Every file of the job is compared with the FIRST file, each read here with TFile.
    //   treestream's own list (present()) is not used for this: it comes from TChain::GetListOfBranches() after
    //   GetEntries(), i.e. from whichever tree the chain has loaded at that moment. With ROOT 6.30 (KNU) a job of a
    //   PNet and a no-PNet era C file (in that order) ran through with exit 0, so that list did not hold the first
    //   file's PNet branch (the bit then reads 0 in the first file); with ROOT 6.40 (container) the same job stopped.
    // [2026-10-06] Every file is also counted, one-file jobs too. ROOT's TChain skips a file it cannot open (or one
    //   without the tree) with only an error line and counts it as 0 entries, so the job would end with exit 0 on
    //   fewer events (tempTTHH STEP 24 section 14). Here: a file that cannot be read stops the job (E30), and the
    //   Events entries of the files must add up to the chain's (_ev->size()).
    std::vector<std::string> names;
    for (const auto& kv : _ev->choose)
        if (kv.second && kv.first.compare(0, 7, "Events/") == 0) names.push_back(kv.first.substr(7));
    auto presence = [&names](const std::string& path, std::vector<char>& has, Long64_t& n) -> bool {
        std::unique_ptr<TFile> f(TFile::Open(path.c_str(), "READ"));
        TTree* t = (f && !f->IsZombie()) ? dynamic_cast<TTree*>(f->Get("Events")) : nullptr;
        if (!t) return false;
        n = t->GetEntries();
        has.assign(names.size(), 0);
        for (size_t j = 0; j < names.size(); ++j) has[j] = (t->GetBranch(names[j].c_str()) != nullptr);
        return true;
    };
    std::vector<std::string> unreadable, diffs;
    std::vector<char> first, here;
    Long64_t sum = 0;
    for (size_t i = 0; i < files.size(); ++i) {
        std::vector<char>& has = (i == 0) ? first : here;
        Long64_t n = 0;
        if (!presence(files[i], has, n)) {
            unreadable.push_back(files[i]);
            continue;
        }
        sum += n;
        if (i == 0 || first.empty()) continue;      // first.empty(): the first file is unreadable (reported below)
        int nd = 0;
        for (size_t j = 0; j < names.size(); ++j) {
            if (here[j] == first[j]) continue;
            if (nd++ < 10)
                diffs.push_back(files[i] + ": " + names[j] + (here[j] ? " present (absent in the first file)"
                                                                       : " absent (present in the first file)"));
        }
        if (nd > 10) diffs.push_back(files[i] + ": ... " + std::to_string(nd - 10) + " more");
    }
    if (!unreadable.empty()) {
        std::cerr << "\n[FATAL][E" << tthh::INPUT_OPEN_FAIL << "] " << unreadable.size() << " of the " << files.size()
                  << " input files of this job cannot be read (no file or no Events tree):\n";
        for (const auto& u : unreadable) std::cerr << "    - " << u << "\n";
        std::cerr << "  ROOT's TChain skips such a file with only an error line, and the job would end with exit 0 on\n"
                  << "  fewer events. Check the file (size, the storage), correct the filelist if it is gone for good,\n"
                  << "  and send the job again (submit_job_FH_Tier3_unified.py ... --resubmit).\n";
        tthh::fatalExit(tthh::INPUT_OPEN_FAIL);
    }
    if (!diffs.empty()) {
        std::cerr << "\n[FATAL][E" << tthh::CONFIG_BAD_RUNINFO << "] the " << files.size()
                  << " input files of this job do not have the same branches (first file " << files[0] << "):\n";
        for (const auto& d : diffs) std::cerr << "    - " << d << "\n";
        std::cerr << "  treestream uses ONE branch set for the whole job (the chain's branch list; which file it comes from\n"
                  << "  depends on the ROOT version), so a branch missing in some files reads as 0 there or stops the job.\n"
                  << "  Run such files one per job (files_per_job: 1, the 2024 yml), or group them by branch set\n"
                  << "  (tools/stage0/branch_signature.py).\n";
        tthh::fatalExit(tthh::CONFIG_BAD_RUNINFO);
    }
    const Long64_t chain = static_cast<Long64_t>(_ev->size());
    if (sum != chain) {
        std::cerr << "\n[FATAL][E" << tthh::INPUT_OPEN_FAIL << "] the Events entries of the " << files.size()
                  << " input files add up to " << sum << ", the chain has " << chain << ": ROOT skipped (part of) a file\n"
                  << "  when it counted the chain (look for 'Error in <TFile' above). Send the job again.\n";
        tthh::fatalExit(tthh::INPUT_OPEN_FAIL);
    }
    if (files.size() > 1)
        std::cout << "[branches] " << files.size() << " input files, the same " << names.size()
                  << " selected branches in each." << std::endl;
    std::cout << "[inputs] " << files.size() << (files.size() == 1 ? " input file" : " input files")
              << ", Events entries " << sum << " = the chain's." << std::endl;
}

// [STEP 24] EraConfig::metFilters(year) -> the eventBuffer members. An unknown name is fatal (it would be
//   a filter that silently does nothing). The member table lists every flag either list can name.
void ttHHanalyzer_unified::setupMetFilters_() {
    const std::map<std::string, const bool*> member = {
        {"Flag_goodVertices",                       &_ev->Flag_goodVertices},
        {"Flag_globalSuperTightHalo2016Filter",     &_ev->Flag_globalSuperTightHalo2016Filter},
        {"Flag_HBHENoiseFilter",                    &_ev->Flag_HBHENoiseFilter},
        {"Flag_HBHENoiseIsoFilter",                 &_ev->Flag_HBHENoiseIsoFilter},
        {"Flag_EcalDeadCellTriggerPrimitiveFilter", &_ev->Flag_EcalDeadCellTriggerPrimitiveFilter},
        {"Flag_BadPFMuonFilter",                    &_ev->Flag_BadPFMuonFilter},
        {"Flag_BadPFMuonDzFilter",                  &_ev->Flag_BadPFMuonDzFilter},
        {"Flag_hfNoisyHitsFilter",                  &_ev->Flag_hfNoisyHitsFilter},
        {"Flag_eeBadScFilter",                      &_ev->Flag_eeBadScFilter},
        {"Flag_ecalBadCalibFilter",                 &_ev->Flag_ecalBadCalibFilter},
    };
    _metFilterPtrs.clear();
    std::string names;
    for (const auto& f : EraConfig::metFilters(_runYear)) {
        auto it = member.find(f);
        if (it == member.end()) {
            std::cerr << "\n[FATAL][E" << tthh::CONFIG_BAD_RUNINFO << "] MET filter '" << f
                      << "' (EraConfig::metFilters) has no member in setupMetFilters_.\n" << std::endl;
            tthh::fatalExit(tthh::CONFIG_BAD_RUNINFO);
        }
        _metFilterPtrs.push_back(it->second);
        names += (names.empty() ? "" : " ") + f;
    }
    std::cout << "[cleaning] MET filters (" << _runYear << "): " << names << std::endl;
}

// [STEP 24] jet veto map (2024): the event is vetoed when a jet in a veto region passes
//   NanoAOD pT > 15 GeV, tight ID (AK4PUPPI_Tight), chEmEF + neEmEF < 0.9 and has no PF muon within
//   dR 0.2 (the JERC recommendation as remembered — to be confirmed, PLAN 9.6 D11). The NanoAOD pT is used
//   (the cut is loose and the JEC difference small); |eta| above 5.19 is outside the map.
bool ttHHanalyzer_unified::jetVetoed_(const std::vector<eventBuffer::Jet_s>& jets,
                                      const std::vector<eventBuffer::Muon_s>& muons) const {
    for (const auto& j : jets) {
        if (!(j.pt > 15.f) || std::fabs(j.eta) > 5.19f) continue;
        if (!(j.chEmEF + j.neEmEF < 0.9f)) continue;
        if (!corrMgr->passJetIdTight(j.eta, j.chHEF, j.neHEF, j.chEmEF, j.neEmEF, j.muEF,
                                     j.chMultiplicity, j.neMultiplicity)) continue;
        bool nearMuon = false;
        for (const auto& m : muons)
            if (m.isPFcand && deltaR(j.eta, j.phi, m.eta, m.phi) < 0.2) { nearMuon = true; break; }
        if (nearMuon) continue;
        if (corrMgr->inJetVetoMap(j.eta, j.phi)) return true;
    }
    return false;
}

void ttHHanalyzer_unified::createObjects(event * thisEvent, sysName sysType, bool up){

    _ev->fillObjects();
 
    // =================================================================
    // 1. [Definition] 복잡한 HLT 경로를 의미 있는 변수로 변환
    // =================================================================

    // ─────────────────────────────────────────────────────────────────
    // [2018] Hadronic trigger — 연도/Era 분기
    //
    //  왜 분기가 필요한가
    //  ------------------
    //  2017 의 주력 경로(...TriplePFBTagCSV_3p0, ...PFBTagCSV_1p5,
    //  ...DoublePFBTagCSV_2p2)는 **2018 메뉴에 존재하지 않는다**. 2018 은
    //  DeepCSV 기반 경로로 교체되었다. eventBuffer 는 없는 branch 를 0 으로
    //  두므로 (initBuffers), 분기 없이 2018 을 돌리면 세 경로가 모두 false 가
    //  되어 HT1050 만 남고 **경고 하나 없이 통계가 급감**한다.
    //  그래서 아래 requireTriggerBranches() 로 "이 연도가 요구하는 경로가
    //  입력 파일에 실제로 있는지"를 첫 이벤트에서 한 번 검사하고, 없으면
    //  FATAL 로 끊는다. 조용히 0 event 를 내는 경로를 남기지 않는다.
    //
    //  PD 구성
    //  -------
    //   2017 : BTagCSV(4J3T) + JetHT(6J/HT)  -> orthogonality veto 필요
    //   2018 : **JetHT 단독** (BTagCSV PD 는 2018 에 없음)
    //          -> 모든 경로가 한 PD 안에 있으므로 veto 하면 안 된다.
    //             (veto 를 남겨두면 4J3T 가 터진 이벤트를 아무도 안 가져가
    //              통째로 사라진다.)
    //   2024 : JetMET0/1(HT1050) + **ParkingHH**(4J3T, 6J1T, 6J2T) -> veto 필요 (D-2026-10-06-A;
    //          HLT 메뉴 run 380115·382913·386604 의 process.datasets, tempTTHH STEP 24 §17). 2017 과 같은
    //          구조이고 겹치는 event 는 JetMET 이 가져간다(아래 2024 Data 분기).
    // ─────────────────────────────────────────────────────────────────
    bool fired_4J3T = false, fired_6J1T = false, fired_6J2T = false;
    bool fired_HT   = _ev->HLT_PFHT1050;   // 2017/2018/2024 공통

    requireBranches_();   // [STEP 24] every year, once (first event)

    if (_runYear == "2018") {
        requireTriggerBranches2018_();

        fired_4J3T = _ev->HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5;
        fired_6J2T = _ev->HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94;
        fired_6J1T = _ev->HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59;
    }
    else if (_runYear == "2017") {
        // 2017 Era B 는 CSV(=pre-"PF") 이름의 옛 경로를 쓴다. 기존 동작 그대로.
        const bool isEraB = (_era == "B");
        fired_4J3T = isEraB ? _ev->HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07
                            : _ev->HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0;
        fired_6J1T = isEraB ? _ev->HLT_PFHT430_SixJet40_BTagCSV_p080
                            : _ev->HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5;
        fired_6J2T = isEraB ? _ev->HLT_PFHT380_SixJet32_DoubleBTagCSV_p075
                            : _ev->HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2;
    }
    else if (_runYear == "2024") {
        // [STEP 24] 2024 (PLAN 9.6 D4): the PNet counterparts of the 2017 paths; HT1050 as before.
        //   4J3T: the DeepJet path ran in the first 38 runs (6.354 fb-1 BRIL), the PNet path after, never
        //   both (LUMI_SOURCES 6.3) -> Data takes the OR. MC has both branches; it uses the PNet path only,
        //   like 2017 MC uses the non-era-B names. PROVISIONAL until Stage 6 (trigger SF) defines the MC
        //   emulation with the SF (docs/DECISIONS.md D-2026-10-05-B).
        const bool pnet4J3T    = _ev->HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3;
        const bool deepjet4J3T = _ev->HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5;
        fired_4J3T = (_DataOrMC == "Data") ? (pnet4J3T || deepjet4J3T) : pnet4J3T;
        fired_6J1T = _ev->HLT_PFHT450_SixPFJet36_PNetBTag0p35;
        fired_6J2T = _ev->HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50;
    }
    else {
        // 2016 은 경로 세트가 아직 확정되지 않았다. 조용히 2017 경로로
        // 돌아가면 위와 같은 무증상 통계 손실이 나므로 명시적으로 막는다.
        std::cerr << "\n[FATAL][E" << tthh::CONFIG_BAD_RUNINFO
                  << "] Hadronic HLT path set is not defined for runYear='"
                  << _runYear << "'.\n"
                  << "  Only 2017, 2018 and 2024 are implemented. Add the path set here\n"
                  << "  (and the branches to eventBuffer.h) before running this year.\n";
        tthh::fatalExit(tthh::CONFIG_BAD_RUNINFO);
    }

    // 논리 그룹
    const bool group_4J3T  = fired_4J3T;                              // (2017) BTagCSV PD 담당
    const bool group_JetHT = (fired_6J1T || fired_6J2T || fired_HT);  // JetHT PD 담당

    // =================================================================
    // 2. [Application] 데이터셋별 역할 분담 (Orthogonality Enforcement)
    // =================================================================

    bool passHadTrig = false;

    if (_DataOrMC == "MC") {
        // [MC]: 해당 연도의 경로 중 뭐라도 터지면 가져감 (OR)
        passHadTrig = (group_4J3T || group_JetHT);
    }
    else if (_runYear == "2024") {
        // [D-2026-10-06-A] 2024 Data: each PD takes the paths it records (STEP 24 §17). The HLT menus of runs
        //   380115 (C), 382913 (F) and 386604 (I) put HLT_PFHT1050 in JetMET0 and JetMET1 (the two split the
        //   events, not the paths) and the four b-tag paths (4J3T PNet and DeepJet, 6J1T, 6J2T) in ParkingHH
        //   only. An event can be in both, so it is taken once, from JetMET -- the 2017 BTagCSV/JetHT rule with
        //   the roles swapped:
        //     JetMET0/1 : HLT_PFHT1050
        //     ParkingHH : (4J3T || 6J1T || 6J2T) && !HLT_PFHT1050
        //     Muon0/1   : the whole OR (the trigger-SF measurement sample, as SingleMuon in 2017/2018)
        //   Before 2026-10-06 JetMET took the whole OR on the belief that it recorded every hadronic path; it
        //   then also kept the b-tag events that had come in through some other JetMET path, a fraction only.
        //   Without ParkingHH the 2024 Data hold the HLT_PFHT1050 part of the OR, nothing below its turn-on.
        const bool isJetMET  = (_sampleName.find("JetMET") != std::string::npos);
        const bool isParking = (_sampleName.find("ParkingHH") != std::string::npos);
        const bool isMuonPD  = (_sampleName.find("Muon0") != std::string::npos ||
                                _sampleName.find("Muon1") != std::string::npos);
        const bool groupBtag = (fired_4J3T || fired_6J1T || fired_6J2T);
        if (isJetMET && !isParking) {
            passHadTrig = fired_HT;
        }
        else if (isParking && !isJetMET) {
            passHadTrig = groupBtag && !fired_HT;
        }
        else if (isMuonPD && !isJetMET && !isParking) {
            passHadTrig = (group_4J3T || group_JetHT);
        }
        else {
            std::cerr << "\n[FATAL][E" << tthh::CONFIG_BAD_RUNINFO
                      << "] 2024 Data PD not recognised (expected JetMET0/1, ParkingHH or Muon0/1): "
                      << _sampleName << std::endl;
            tthh::fatalExit(tthh::CONFIG_BAD_RUNINFO);
        }
        static bool ruleShown = false;
        if (!ruleShown) {
            ruleShown = true;
            std::cout << "[trigger] 2024 Data " << (isJetMET ? "JetMET" : isParking ? "ParkingHH" : "Muon0/1")
                      << " takes: "
                      << (isJetMET ? "HLT_PFHT1050 (the b-tag paths are recorded in ParkingHH)"
                          : isParking ? "(4J3T PNet|DeepJet || 6J1T || 6J2T) && !HLT_PFHT1050 (HLT_PFHT1050 events "
                                        "come from JetMET)"
                          : "the whole OR (trigger-SF sample)")
                      << " [D-2026-10-06-A]" << std::endl;
        }
    }
    else if (_runYear == "2018") {
        // [2018 Data] JetHT 단독 PD — 중복 계수 위험이 없으므로 단순 OR.
        if (_sampleName.find("JetHT") != std::string::npos) {
            passHadTrig = (group_4J3T || group_JetHT);
        }
        else if (_sampleName.find("SingleMuon") != std::string::npos) {
            passHadTrig = (group_4J3T || group_JetHT);   // trigger SF 측정용
        }
        else {
            std::cerr << "\n[FATAL][E" << tthh::CONFIG_BAD_RUNINFO
                      << "] 2018 Data PD not recognised (expected JetHT or SingleMuon): "
                      << _sampleName << std::endl;
            tthh::fatalExit(tthh::CONFIG_BAD_RUNINFO);
        }
    }
    else { // [2017 Data] — 기존 로직 그대로
        if (_sampleName.find("BTagCSV") != std::string::npos) {
            // [Rule 1] BTagCSV 데이터셋은 4J3T 그룹만 챙긴다.
            passHadTrig = group_4J3T;
        }
        else if (_sampleName.find("JetHT") != std::string::npos) {
            // [Rule 2] JetHT 데이터셋은 자기 그룹(6J or HT)을 챙기되,
            //          만약 4J3T가 같이 터졌으면 BTagCSV에 양보한다. (Veto)
            passHadTrig = (group_JetHT && !group_4J3T);
        }
        else if (_sampleName.find("SingleMuon") != std::string::npos) {
	    // [Rule 3] Single muon for Trigger Study
	    // Handle the logic same as MC trigger path
            passHadTrig = (group_4J3T || group_JetHT);
        }
        else {
            // 안전장치 — 미인식 Data PD 는 trigger 경로가 정의되지 않음
             std::cerr << "\n[FATAL][E" << tthh::CONFIG_BAD_RUNINFO
                       << "] Unknown Data Sample (no JetHT/BTagCSV/SingleMuon trigger-PD match): "
                       << _sampleName << std::endl;
             tthh::fatalExit(tthh::CONFIG_BAD_RUNINFO);
        }
    }

    // [D-2026-10-06-A] counts for the end-of-job [trigger] line (which paths fired, what the rule took)
    {
        const bool anyBtag = (fired_4J3T || fired_6J1T || fired_6J2T);
        ++_nTrigAll;
        if (fired_HT) ++_nTrigHT;
        if (anyBtag) ++_nTrigBtag;
        if (fired_HT && anyBtag) ++_nTrigBoth;
        if (passHadTrig) ++_nTrigTaken;
    }

    // 최종 결과 적용
    thisEvent->setHadTrigger(passHadTrig);
 
    // Set the noise filter
    // https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2#2018_2017_data_and_MC_UL
    // [STEP 24] the list is EraConfig::metFilters(year) (Run 2: the nine flags this line had; 2024: the
    //   Run 3 list). 2024 adds the jet veto map to the same cut step (noiseFilter): the cutflow keeps its
    //   bins, the two parts are counted apart (_nCleanMETFail, _nCleanVetoFail; printed at the end).
    const bool metOK = passMetFilters_();
    _jetVetoed = _useJetVetoMap && jetVetoed_(_ev->Jet, _ev->Muon);
    ++_nCleanAll;
    if (!metOK) ++_nCleanMETFail;
    if (_jetVetoed) ++_nCleanVetoFail;
    thisEvent->setFilter(metOK && !_jetVetoed);

  
    thisEvent->setPV(_ev->PV_npvsGood);
    std::vector<eventBuffer::GenPart_s> genPart = _ev->GenPart;      
    std::vector<eventBuffer::GenJet_s> genJet = _ev->GenJet;
    std::vector<eventBuffer::Jet_s> jet = _ev->Jet;
    std::vector<eventBuffer::Muon_s> muonT = _ev->Muon;
    std::vector<eventBuffer::Electron_s> ele = _ev->Electron;
    objectGenPart * currentGenPart; 
    objectBoostedJet * currentBoostedJet;
    objectJet * currentJet;
    objectLep * currentMuon;
    objectLep * currentEle;
    float e = 1., es  = 1., pe = 1., pes = 1.;
    float me = 1., mes = 1., pme = 1.,  pmes = 1.;   


    // Leading lepton def
    // But FH channel don't need this..
    // We just use subleading lepton def for veto
    // update in 5th Jan, 2026


// =============================================================
// Lepton Object Definition   
 
    // =============================================================
    // STEP 1: Count VetoLeptons (Executed in all modes)
    // Purpose: To determine lepton veto for the hadronic channel
    // =============================================================
    int nVetoMuons = 0, nVetoEle = 0;

    for(int i = 0; i < muonT.size(); i++){
        if(fabs(muonT[i].eta) < Cuts::muonEta && 
           muonT[i].tightId == true && 
           muonT[i].pfRelIso04_all < Cuts::muonIso &&
           muonT[i].pt > Cuts::subLeadMuonPt) {
            nVetoMuons++;
        }
    }
    for(int i = 0; i < ele.size(); i++){
        if((fabs(ele[i].deltaEtaSC + ele[i].eta) < 1.4442 || 
            fabs(ele[i].deltaEtaSC + ele[i].eta) > 1.5660) &&
           fabs(ele[i].eta) < Cuts::eleEta && 
           eleWP90_(ele[i]) && 
           //ele[i].pfRelIso03_all < eleIso && // We don't need Iso, It's already in ID
           ele[i].pt > Cuts::subLeadElePt) {
            nVetoEle++;
        }
    }
    // Select all leptons which pass loose selection cut
    thisEvent->setnVetoLepton(nVetoMuons + nVetoEle);
 
   
    // =============================================================
    // STEP 2: Lepton Collection (Executed if collectLeptons is true)
    // =============================================================
    if (_policy.collectLeptons) {
 
        // -------------------------------------------------------------------------
        // 2a. Check Lead Lepton Gate
        // Purpose: To save computing resources, we first check if there is at least 
        //          one "Trigger-capable" (High pT, Tight) lepton in the event.
 	// -------------------------------------------------------------------------
        bool hasLeadMuon = false;
        for(int i = 0; i < muonT.size(); i++){
            // Check for Lead Muon (High pT, Tight ID)
            if(fabs(muonT[i].eta) < Cuts::muonEta && 
               muonT[i].tightId == true && 
               muonT[i].pfRelIso04_all < Cuts::muonIso &&
               muonT[i].pt > Cuts::leadMuonPt) { 
                hasLeadMuon = true;
                break;
            }
        }
        
        // Check for Lead Electron
        // (Calculation is done only if needed or generic, but passGate logic controls usage)
        bool hasLeadElectron = false;
        if (!_policy.requireLeadMuonOnly) { // Optimization: Skip if we only care about Muons
            for(int i = 0; i < ele.size(); i++){
                // Check for Lead Electron (High pT, Tight ID)
                if((fabs(ele[i].deltaEtaSC + ele[i].eta) < 1.4442 || 
                    fabs(ele[i].deltaEtaSC + ele[i].eta) > 1.5660) &&
                   fabs(ele[i].eta) < Cuts::eleEta && 
                   eleWP90_(ele[i]) && 
                   //ele[i].pfRelIso03_all < eleIso && // We don't need Iso, It's already in ID
                   ele[i].pt > Cuts::leadElePt) { 
                    hasLeadElectron = true;
                    break;
                }
            }
        }
        
        // -------------------------------------------------------------------------
        // Define PassGate Logic Explicitly
        // -------------------------------------------------------------------------
        bool passGate = false;
    
        if (_policy.requireLeadMuonOnly) {
            // [TriggerSF / Single Muon Mode]
            // STRICT REQUIREMENT: Must have at least one Leading Muon.
            // We ignore Leading Electrons here because they cannot trigger the Muon HLT.
	    // And Collect All Sublead Electron even event don't satify lead electron cut
            passGate = hasLeadMuon;
        } 
        else {
            // [Standard Mode, You can use this for Leptonic Channel]
            // Requirement: Either a Leading Muon OR a Leading Electron exists.
            passGate = hasLeadMuon || hasLeadElectron;
        }

        // -------------------------------------------------------------------------
        // 2b. Actual Lepton Collection
        // -------------------------------------------------------------------------
        if (passGate) {
        
            // [Muon Collection]
            // Collect ALL muons passing the Veto (Sub-lead) threshold.
            // Even if TriggerSF requires a Lead Muon to pass the gate, 
            // we collect softer muons here to check for Dilepton veto later.
            for(int i = 0; i < muonT.size(); i++){
                if(fabs(muonT[i].eta) < Cuts::muonEta && 
                   muonT[i].tightId == true && 
                   muonT[i].pfRelIso04_all < Cuts::muonIso &&
                   muonT[i].pt > Cuts::subLeadMuonPt) { 
                   
                    currentMuon = new objectLep(muonT[i].pt, muonT[i].eta, muonT[i].phi, 0.);
                    currentMuon->charge = muonT[i].charge;
                    currentMuon->miniPFRelIso = muonT[i].miniPFRelIso_all;
                    currentMuon->pfRelIso04 = muonT[i].pfRelIso04_all;
                    thisEvent->selectMuon(currentMuon);
                }
            }
            
            // [Electron Collection]
            // Always collect Veto Electrons (Sub-lead) to ensure 'nElectrons' is correct.
            // This is crucial for vetoing dilepton events even in Single Muon analysis.
            for(int i = 0; i < ele.size(); i++){
                if((fabs(ele[i].deltaEtaSC + ele[i].eta) < 1.4442 || 
                    fabs(ele[i].deltaEtaSC + ele[i].eta) > 1.5660) &&
                   fabs(ele[i].eta) < Cuts::eleEta && 
                   eleWP90_(ele[i]) && 
                   //ele[i].pfRelIso03_all < eleIso && // We don't need Iso, It's already in ID
                   ele[i].pt > Cuts::subLeadElePt) { 
                   
                    currentEle = new objectLep(ele[i].pt, ele[i].eta, ele[i].phi, 0.);
                    currentEle->charge = ele[i].charge;
                    currentEle->miniPFRelIso = ele[i].miniPFRelIso_all;
                    currentEle->pfRelIso03 = ele[i].pfRelIso03_all;
                    thisEvent->selectEle(currentEle);
                }
            }
        
        // Sort leptons by pT (Descending order)
        thisEvent->orderLeptons();
        }
    
    } // End of [ _policy.collectLeptons ] if statement

// End of Lepton Object Definition   
// =============================================================


    float dR = 0., deltaEta = 0., deltaPhi = 0.;
    bool passPuId = false;
    // [STEP 24] v15: Rho_fixedGridRhoFastjetAll (same quantity, renamed)
    float rho = _isRun3 ? _ev->Rho_fixedGridRhoFastjetAll : _ev->fixedGridRhoFastjetAll;
    for (int i = 0; i < (int)jet.size(); ++i) {

        const auto& jetRaw = jet[i];

        // 1. Pre-cuts
        // [UPDATE] 보정 후 기준으로 pT 컷을 적용하기 위해 여기서는 eta/ID만 최소한으로 확인
        // [STEP 24] v15 has no Jet_jetId: 2024 recomputes it with JME's jetid.json, the same level
        //   (AK4PUPPI_TightLeptonVeto = tight && lepton veto = Jet_jetId >= 6 of v9, Cuts::jetID).
        //   eta first, so the payload (|eta| <= 5.2) only sees central jets.
        if( !(fabs(jetRaw.eta) < Cuts::jetEta) ) continue;
        if (_isRun3) {
            if (!corrMgr->passJetIdTightLepVeto(jetRaw.eta, jetRaw.chHEF, jetRaw.neHEF, jetRaw.chEmEF, jetRaw.neEmEF,
                                                jetRaw.muEF, jetRaw.chMultiplicity, jetRaw.neMultiplicity)) continue;
        } else if (!(jetRaw.jetId >= Cuts::jetID)) continue;
 
        // 2. Calculation (지역 변수 사용, Heap 할당 X)
        float ntuplePt  = jetRaw.pt;                // NanoAOD Default (Corrected)
        float rawFactor = jetRaw.rawFactor;
        float rawPt     = ntuplePt * (1.0f - rawFactor);
        float rawMass   = jetRaw.mass * (1.0f - rawFactor);

            // --- A) Re-apply JEC ---
            // [STEP 24] phi and run are inputs of the 2024 compound (L2Relative, DATA L2L3Residual);
            //   the Run 2 compounds do not take them (CorrectionsManager::getJEC)
            double jecSF = corrMgr->getJEC(jetRaw.eta, jetRaw.phi, rawPt, jetRaw.area, rho, _ev->run);
            float ptJEC  = rawPt * jecSF;               // 내가 재계산한 pT
            float massJEC = rawMass * jecSF;


            // --- [Validation] On-the-fly Check --- 
            // (JEC 재적용 관련 validation, 재적용 JEC와 ntuple default JEC pT의 차이가 크면 에러 출력)
            float relDiff = -1.f;        
            if (ntuplePt > 0) {
                relDiff = (ptJEC - ntuplePt) / ntuplePt;
            
                // 디버깅용: 차이가 너무 크면 출력
                if (debugCorrections && fabs(relDiff) > 0.01) { 
                     std::cout << "[JEC Diff] Idx:" << i << " Orig:" << ntuplePt << " New:" << ptJEC << std::endl;
                }
            }
        
            // --- B) Apply JER ---
	    double genPt=-1.f, genEta=0.0, genPhi=0.0;
            if (jetRaw.genJetIdx >= 0 && jetRaw.genJetIdx < (int)genJet.size()) {
                const auto& g = genJet[jetRaw.genJetIdx];
                genPt=g.pt; genEta=g.eta; genPhi=g.phi;
            }
            unsigned int run=_ev->run, lumi=_ev->luminosityBlock;
            unsigned long long event=_ev->event;
            int currentJetIdx = i;

            const double smearedPt = corrMgr->smearJER(ptJEC, jetRaw.eta, jetRaw.phi, rho,
                                          run, lumi, event, currentJetIdx,
                                          genPt, genEta, genPhi, "nom");

	    const double jerFactor = (ptJEC>0.0) ? (smearedPt/ptJEC) : 1.0;
	    const double massJECJER = massJEC * jerFactor;

        // 3. Final Cuts (on Smeared pT)
        if (smearedPt < Cuts::jetPt) continue;
        
        // PU ID Check (Low pT only)
        passPuId = true;
        // [STEP 24] no PU jet ID for PUPPI jets (2024; EraConfig::usesPUJetID)
        if (_usePUJetID && smearedPt < 50.0 && jetRaw.puId < Cuts::jetPUid) {
             continue; 
        }

        // 4. Create Final Object (단 한 번만 생성)
        objectJet* newJet = new objectJet(
            smearedPt,
            jetRaw.eta,
            jetRaw.phi,
            massJECJER
        );

        // 메타데이터 저장
        newJet->JEC_DiffRatio = relDiff;
        // [STEP 24] 2024: UParTAK4 score (its WPs: EraConfig::btagWP); jetID 6 = passed the tight +
        //   lepton-veto ID above; jetPUid 7 = no PU ID for PUPPI jets (every WP bit set, nothing cut)
        newJet->bTagCSV       = _isRun3 ? jetRaw.btagUParTAK4B : jetRaw.btagDeepFlavB;
        newJet->jetID         = _isRun3 ? 6 : jetRaw.jetId;
        newJet->jetPUid       = _usePUJetID ? jetRaw.puId : 7;
        newJet->passPuId      = passPuId;
        newJet->hadFlav       = jetRaw.hadronFlavour;
        newJet->partonFlav    = jetRaw.partonFlavour;
        newJet->genMatchedPt  = genPt; // 필요하다면
        if (jetRaw.mass > 0.0f) {
            newJet->mass_DiffRatio = (massJEC - jetRaw.mass) / jetRaw.mass;
        } else {
            newJet->mass_DiffRatio = 0.0f;
        }

        // 이벤트에 등록 + b-tag WP에 따른 분류 (selectJet 내부에서 b-jet 판정)
        // [2018] WP 는 연도별 주입값이다. 주입 없이 여기 도달하면 FATAL.
        objectJet::assertBtagWPConfigured("createObjects/jet classification");
        thisEvent->selectJet(newJet);
        if (newJet->bTagCSV >= objectJet::valbTagMedium) {
            thisEvent->selectbJet(newJet);
        } 
	else if (newJet->bTagCSV < objectJet::valbTagLoose) {
            thisEvent->selectLightJet(newJet);
        }

    }   // <--- Jet Loop End (여기가 닫혔는지 꼭 확인!)
    
	thisEvent->orderJets();

    // =================================================================
    // GenPart Loop: Memory Leak Fix
    // =================================================================
    for (int i = 0; i < (int)genPart.size(); i++) {
        
        // [중요] 조건 먼저 검사 (조건 불만족 시 객체 생성 안 함 -> 메모리 누수 방지)
        bool isBQuark = (abs(genPart[i].pdgId) == 5);
        bool isHard   = (genPart[i].statusFlags & 256);

        if (!isBQuark || !isHard) continue;

        // 합격한 경우에만 new 할당
        currentGenPart = new objectGenPart(
            genPart[i].pt, genPart[i].eta, genPart[i].phi, genPart[i].mass
        );
        currentGenPart->hasHiggsMother = false;
        currentGenPart->hasTopMother   = false;

        // Mother Tracing (While loop로 간소화)
        int motherInd = genPart[i].genPartIdxMother;
        while (motherInd >= 0 && motherInd < (int)genPart.size()) {
            int mPDG = abs(genPart[motherInd].pdgId);
            bool mStatus = (genPart[motherInd].statusFlags & 256);

            if (mStatus) {
                if (mPDG == 25) currentGenPart->hasHiggsMother = true;
                if (mPDG == 6)  currentGenPart->hasTopMother   = true;
            }
            if (currentGenPart->hasHiggsMother && currentGenPart->hasTopMother) break;
            
            motherInd = genPart[motherInd].genPartIdxMother;
        }

        thisEvent->selectGenPart(currentGenPart);
    } // GenPart Loop End

} // CreateObject Function End
           

// ============================================================================
// selectObjects — 이벤트 선택 + Trigger SF + B-tag Reweight 적용
//
// Weight 적용 순서 (Weight multiplication order):
//   _evtWeight = baseWeight × PU × L1PreFiring × genWeight
//                             (← process() 함수에서 이미 계산됨)
//   → computeBTagWeight()  로 bTagWeight_central_ 계산됨
//   → selectObjects()에서:
//       Step kNoCut   : _evtWeight (b-tag SF 미포함) 로 cutflow 시작
//       Step kHadTrigger~kHT : 기본 selection cuts
//       Step kNumbJets : b-jet cut 적용 후
//       Step kHadWMass  → kHiggsMass
//       Step kTotal    : _evtWeight에 btagSF × trigSF × btagReweight 곱한 최종 weight
//
// ═══════════════════════════════════════════════════════════════════════════
// 설계 결정 (Design Decision):
//   Trigger SF와 B-tag Reweight는 모든 selection cut을 통과한 후,
//   kTotal step 직전에 _evtWeight에 곱해진다.
//   이유: 두 보정 모두 selection 후의 kinematics (HT, nJets, jet6PT)에 의존하므로
//   cut 이전에 적용하면 의미가 없다.
//   단, cutflow 중간 단계에서는 보정 전 weight를 사용하여 selection efficiency를
// ============================================================================
// [STEP3] applyEventScaleFactors — 이벤트 단위 보정(SF) 블록
// HT cut 통과 직후(= nbjet cut 이전)에 kCutSequence의 onAfter로 호출된다.
// 기존 selectObjects 인라인 블록을 메서드로 추출 — 내용은 비트 동일 이동.
// 위치 의미(왜 step 7과 8 사이인가)는 블록 첫 주석 [MOVED ...] 참조.
// ============================================================================
void ttHHanalyzer_unified::applyEventScaleFactors(event* thisEvent){
    // ══════════════════════════════════════════════════════════════════════
    // [MOVED from post-step-10 to pre-step-8]
    // Event-level corrections applied here — between HT cut (step 7) and
    // nbjet cut (step 8) — so cutflow histograms from step 8 onwards see
    // the SF-corrected MC. This is required to read the b-tag SF / trigger
    // SF / norm reweight effect from the cutflow ratios.
    //
    // [ttH AN A.2.1] inclusive ttbar dispatched by genTtbarId; non-ttbar
    // by sample name. See Config_TtCatGroup.hh::MakeProcessKey().
    // ══════════════════════════════════════════════════════════════════════
    triggerSF_        = 1.0f;
    triggerSF_up_     = 1.0f;
    triggerSF_down_   = 1.0f;
    btagNormReweight_ = 1.0f;

    // Always populate the 3 chain weights with the FULL meaning, regardless
    // of validation toggles — they are diagnostic, not the production path.
    _evtWeight_chain_raw    = _evtWeight;     // baseline only
    _evtWeight_chain_btagSF = _evtWeight;     // will multiply btagShape below
    _evtWeight_chain_full   = _evtWeight;     // will multiply all SFs below

    if (_DataOrMC != "Data") {

        // [STEP 25 K] fixed-WP years: the efficiency histograms (every cut but the b-tag ones passed here) and the
        //   closure of the b-tag weight (sum w x bTagWeight / sum w over these events, printed at the end: ~1)
        if (_btagFixedWP) {
            fillBTagEff_(thisEvent);
            if (_btagFixedWPReady) {
                _btagClosureW  += _evtWeight;
                _btagClosureWB += _evtWeight * bTagWeight_central_;
                ++_btagClosureN;
                fillBTagClosure_(thisEvent);   // [STEP 27 N] per true b jets, the comparison weights
            }
        }

        // ── b-tag shape SF ─────────────────────────────────────────────
        // Both chains get it (even if legacy _evtWeight skips it in
        // validation when toggle is off).
        // [3-tier] b-tag shape SF 는 production evtWeight 에 곱하지 않는다.
        //   evtWeight        = ... × trigSF                (b-tag SF/RW 없음, 기본)
        //   evtWeight_btagSF = evtWeight × btagShape
        //   evtWeight_full   = evtWeight_btagSF × btagNormRW
        // 사용자 결정: stack plot 1차는 trigSF만; b-tag 영향은 별도 tier로 비교.
        _evtWeight_chain_btagSF *= bTagWeight_central_;
        _evtWeight_chain_full   *= bTagWeight_central_;
        if (_applyBtagSF) _evtWeight *= bTagWeight_central_;   // [SF toggle]
        _dbg.kv("sf", "btagShape", bTagWeight_central_);

        // ── Trigger SF ────────────────────────────────────────────────
        const int    nbjet   = thisEvent->getnSelbJet();
        const double ht      = thisEvent->getSumSelJetScalarpT();
        const double jet6pt  = thisEvent->getSelJets()->at(5)->getp4()->Pt();
        const double jet6eta = thisEvent->getSelJets()->at(5)->getp4()->Eta();

        triggerSF_      = static_cast<float>(corrMgr->getTriggerSF(nbjet, jet6eta, ht, jet6pt,  0.0));
        triggerSF_up_   = static_cast<float>(corrMgr->getTriggerSF(nbjet, jet6eta, ht, jet6pt, +1.0));
        triggerSF_down_ = static_cast<float>(corrMgr->getTriggerSF(nbjet, jet6eta, ht, jet6pt, -1.0));

        // trigSF: tier 는 항상, production evtWeight 는 토글.
        _evtWeight_chain_btagSF *= triggerSF_;
        _evtWeight_chain_full   *= triggerSF_;
        if (_applyTrigSF) _evtWeight *= triggerSF_;   // [SF toggle]
        _dbg.kv("sf", "triggerSF", triggerSF_);

        // ── b-tag normalization reweight ──────────────────────────────
        // [ttHH AN-2022/122 / ttH AN App. A.2.1] the b-tag-shape SF distorts
        // the per-category normalization, so it is restored per (sample, HF
        // category). Key on the EXPANDED id (not NanoAOD genTtbarId) so tt+nb
        // (61/62/71/72) carries its own reweight bin, distinct from tt+2b
        // (53/54/55) — high b-jet multiplicity is exactly where tt+nb lives.
        // The stitch multiplier above already fixed the MC composition feeding
        // this derivation, so the reweight is computed on the stitched mix.
        const int nJets = thisEvent->getnSelJet();
        const std::string processKey = TtCatGroup::MakeProcessKey(
            _sampleName, _expandedTtbarId);
        if (processKey.empty()) {
            std::cerr << "\n[FATAL][E" << tthh::PROCESSKEY_EMPTY << "][btagRW] MakeProcessKey() returned an EMPTY key for"
                      << " sample='" << _sampleName << "' expandedTtbarId="
                      << _expandedTtbarId << " (sub="
                      << (((_expandedTtbarId % 100) + 100) % 100) << ").\n"
                      << "  Config_TtCatGroup.hh must map this code. Aborting (exit " << tthh::PROCESSKEY_EMPTY << ")"
                      << " so the Condor job is flagged.\n" << std::endl;
            tthh::fatalExit(tthh::PROCESSKEY_EMPTY);
        }
        // diagnostic: remember the (expandedSub -> processKey) mapping once, so
        // the end-of-job log shows whether 61/62/71/72 get keys distinct from 53.
        {
            const int esub = ((_expandedTtbarId % 100) + 100) % 100;
            if (_btagKeyByExpSub.find(esub) == _btagKeyByExpSub.end())
                _btagKeyByExpSub[esub] = processKey;
        }
        if (_skipBtagReweight) {
            // [debug-only] TTHH_SKIP_BTAGRW=1 — getBTagReweight() 호출 자체를
            // 건너뛴다 (tt+nb 등 키가 옛 JSON에 없을 때 FATAL 46 회피).
            // reweight 미적용 == 1.0. processKey 진단은 위에서 이미 기록됨.
            btagNormReweight_ = 1.0f;
        } else {
            btagNormReweight_ = static_cast<float>(
                corrMgr->getBTagReweight("central", processKey, nJets, ht));
            if (!std::isfinite(btagNormReweight_)) {
                std::cerr << "\n[FATAL][E" << tthh::REWEIGHT_NONFINITE << "][btagRW] non-finite reweight (" << btagNormReweight_
                          << ") for processKey='" << processKey << "' nJets=" << nJets
                          << " ht=" << ht << ". Aborting (exit " << tthh::REWEIGHT_NONFINITE << ").\n" << std::endl;
                tthh::fatalExit(tthh::REWEIGHT_NONFINITE);
            }
        }

        // [3-tier] norm reweight: full tier 는 항상, production evtWeight 는 토글.
        _evtWeight_chain_full *= btagNormReweight_;
        if (_applyBtagRW) _evtWeight *= btagNormReweight_;   // [SF toggle]
        _dbg.kv("sf", "btagNormRW", btagNormReweight_);
        _dbg.kv("sf", "evtWeight_afterSF", _evtWeight);
        _dbg.kv("sf", "evtWeight_btagSF", _evtWeight_chain_btagSF);
        _dbg.kv("sf", "evtWeight_full",   _evtWeight_chain_full);

        if (debugCorrections) {
            std::cout << "[selectObjects] Corrections (mode="
                      << analysisModeName(_analysisMode) << "):"
                      << " sample=" << _sampleName
                      << " genTtbarId=" << _ev->genTtbarId
                      << " expandedTtbarId=" << _expandedTtbarId
                      << " processKey=" << processKey
                      << " btagSF=" << bTagWeight_central_
                      << " trigSF=" << triggerSF_
                      << " btagRW=" << btagNormReweight_
                      << " w_raw="    << _evtWeight_chain_raw
                      << " w_btagSF=" << _evtWeight_chain_btagSF
                      << " w_full="   << _evtWeight_chain_full
                      << " _evtWeight=" << _evtWeight
                      << std::endl;
        }
    }
}

// ============================================================================
// [STEP3] computeLeptonJetStats — lepton-jet / b-jet-lepton 보조 통계
// lepton veto 단계 직후(통과/관찰 무관, 생존 시) kCutSequence의 onAfter로 호출.
// ============================================================================
void ttHHanalyzer_unified::computeLeptonJetStats(event* thisEvent){
    thisEvent->getStatsComb(thisEvent->getSelJets(),  thisEvent->getSelLeptons(), ljetStat);
    thisEvent->getStatsComb(thisEvent->getSelbJets(), thisEvent->getSelLeptons(), lbjetStat);
}

// ============================================================================
// [STEP3] kCutSequence — selection 시퀀스의 단일 정의 (선언적 cut 테이블)
// ----------------------------------------------------------------------------
// 열 의미:
//   step / label        : cutflow 단계와 라벨 (_cutStepLabels와 1:1)
//   enforceIn           : cut으로 "강제"되는 모드 비트
//                         (kSelBitMainLike = main+debug, kSelBitBtagTrig = btagtrig,
//                          kSelEnforceAll = 둘 다, kSelObserveOnly = 기록 전용)
//   recordOnlyIfPass    : true = 조건 충족시에만 기록 (≥3/≥4 b-tag 관찰 단계)
//   pass                : 통과 조건 (nullptr = 무조건 통과; 카운터 단계)
//   onAfter             : 생존 직후 부수 작업
//
// cut 값(Cuts::*)의 출처는 include/SelectionCuts.h 의 AN 주석 참조.
// ============================================================================
const std::vector<ttHHanalyzer_unified::CutDef> ttHHanalyzer_unified::kCutSequence = {
    // 0. 모든 이벤트 카운트
    { CutStep::kNoCut,         "noCut",           kSelObserveOnly, false, nullptr, nullptr },

    // 1. Hadronic trigger — main/debug에서만 강제 (btagtrig은 skim에 bit 저장만)
    { CutStep::kHadTrigger,    "HadTrigger",      kSelBitMainLike, false,
      [](ttHHanalyzer_unified&, event& e, float)->bool { return e.getHadTriggerAccept(); },
      nullptr },

    // 2. MET noise filter — 전 모드 강제 (skim invariant)  [AN §4.2]
    { CutStep::kNoiseFilter,   "noiseFilter",     kSelEnforceAll,  false,
      [](ttHHanalyzer_unified&, event& e, float)->bool { return e.getMETFilter(); },
      nullptr },

    // 3. Primary vertex — 전 모드 강제  [AN §4.2]
    { CutStep::kPrimaryVertex, "pv>=1",           kSelEnforceAll,  false,
      [](ttHHanalyzer_unified&, event& e, float)->bool { return e.getPVvalue(); },
      nullptr },

    // 4. nJets ≥ 6 — 전 모드 강제  [AN Tab.55]
    { CutStep::kNumJets,       "njets>=6",        kSelEnforceAll,  false,
      [](ttHHanalyzer_unified&, event& e, float)->bool { return e.getnSelJet() >= Cuts::nJets; },
      nullptr },

    // 5. 6th jet pT > 40 — 전 모드 강제 (HLT 성능)  [AN Tab.55]
    { CutStep::kSixthJetPt,    "6thJetsPT>40",    kSelEnforceAll,  false,
      [](ttHHanalyzer_unified&, event& e, float)->bool {
          return e.getSelJets()->at(5)->getp4()->Pt() > Cuts::sixthJetPt; },
      nullptr },

    // 6. Lepton veto — main/debug에서만 강제 (btagtrig은 muon control 유지)
    //    [AN Tab.55] 생존 시 lepton-jet 통계 계산 (이전 코드와 동일 위치)
    { CutStep::kLeptonVeto,    "nlepton==0",      kSelBitMainLike, false,
      [](ttHHanalyzer_unified&, event& e, float)->bool { return e.getnVetoLepton() == Cuts::nLeptons; },
      [](ttHHanalyzer_unified& a, event& e){ a.computeLeptonJetStats(&e); } },

    // 7. HT > 500 — 전 모드 강제  [AN Tab.55]
    //    통과 직후 이벤트 단위 SF 적용 (b-tag shape × trigger SF × norm RW) —
    //    step 8 이후 cutflow가 SF 보정된 MC를 보도록 하기 위함 ([MOVED] 주석 참조)
    { CutStep::kHT,            "HT>500",          kSelEnforceAll,  false,
      [](ttHHanalyzer_unified&, event& e, float)->bool { return e.getSumSelJetScalarpT() > Cuts::HT; },
      [](ttHHanalyzer_unified& a, event& e){ a.applyEventScaleFactors(&e); } },

    // 8. nbJets ≥ 2 (baseline) — main/debug 강제  [AN Tab.55] ([round2] was 4)
    { CutStep::kNumbJets2,     "nbjets>=2",       kSelBitMainLike, false,
      [](ttHHanalyzer_unified&, event& e, float)->bool { return e.getnSelbJet() >= Cuts::nbJets; },
      nullptr },

    // 9-10. nbJets ≥ 3 / ≥ 4 — 관찰 전용 (cut 아님; 충족 시에만 기록 →
    //       cutflow ratio로 다음 단계 영향만 읽는다. SR 분류는 이후 단계)
    { CutStep::kNumbJets3,     "nbjets>=3",       kSelObserveOnly, true,
      [](ttHHanalyzer_unified&, event& e, float)->bool { return e.getnSelbJet() >= 3; },
      nullptr },
    { CutStep::kNumbJets4,     "nbjets>=4",       kSelObserveOnly, true,
      [](ttHHanalyzer_unified&, event& e, float)->bool { return e.getnSelbJet() >= 4; },
      nullptr },

    // 11. Hadronic W mass window — main/debug 강제  [AN Tab.55: 30 < m_qq < 250]
    { CutStep::kHadWMass,      "30<HadW<250",     kSelBitMainLike, false,
      [](ttHHanalyzer_unified&, event&, float wMass)->bool {
          return !(wMass < Cuts::hadWMassLo || wMass > Cuts::hadWMassHi); },
      nullptr },

    // 12. Higgs mass window — 현재 비활성 (05 Jan 2026), 카운터만
    { CutStep::kHiggsMass,     "HiggsMassWindow", kSelObserveOnly, false, nullptr, nullptr },

    // 13. Total — 최종 이벤트
    { CutStep::kTotal,         "nTotal",          kSelObserveOnly, false, nullptr, nullptr },
};

//   편향 없이 계산할 수 있도록 한다.
// ═══════════════════════════════════════════════════════════════════════════
bool ttHHanalyzer_unified::selectObjects(event *thisEvent){
     
    // Initialize three diagnostic weight chains to the raw event weight so
    // pre-SF cutflow steps (noCut through HT) are filled consistently.
    _evtWeight_chain_raw    = _evtWeight;
    _evtWeight_chain_btagSF = _evtWeight;
    _evtWeight_chain_full   = _evtWeight;

    // ──────────────────────────────────────────────────────────────────────
    // processStep 람다 (lambda): cutflow 카운트 및 히스토그램 채우기
    // 각 selection cut 단계에서 호출하여 이벤트 수/가중치를 기록한다.
    // ──────────────────────────────────────────────────────────────────────
    auto processStep = [&](CutStep step, float wMassVal) {
        int idx = static_cast<int>(step);
        // [STEP2][debug] 히스토그램과 독립적으로 stdout에서 cutflow 재구성
        // (hCutFlow 채움 로직의 교차 검증용; kDebug에서만 동작)
        _dbg.cut(idx, _cutStepLabels.at(idx).c_str(), _evtWeight);
        
        if (idx < _cutFlowCount.size()) {
            _cutFlowCount[idx]  += 1.0;
            _cutFlowWeight[idx] += _evtWeight;   // legacy
        }

        hCutFlow         ->Fill(idx);
        hCutFlow_w       ->Fill(idx, _evtWeight_chain_raw);
        hCutFlow_w_btagSF->Fill(idx, _evtWeight_chain_btagSF);
        hCutFlow_w_full  ->Fill(idx, _evtWeight_chain_full);

        fillCutStepHist(step, thisEvent, wMassVal);
    };


    // ──────────────────────────────────────────────────────────────────────
    // Hadronic W Mass 계산 (W boson mass reconstruction)
    // light jet이 2개 이상이면 light jet 쌍에서, 아니면 전체 jet에서 계산
    // ──────────────────────────────────────────────────────────────────────
    const float wMass = 80.377f;
    float hadWMass = closestMassPair(
        thisEvent->getSelLightJets()->size() >= 2 ? thisEvent->getSelLightJets() : thisEvent->getSelJets(),
        wMass
    );
    _hadWMassTree = hadWMass;   // [STEP 27 O] the m_qq of the cut (and of the QCD regions, PLAN_QCD_DD) into the tree


    // ──────────────────────────────────────────────────────────────────────
    // Higgs Reconstruction (Higgs 재구성)
    //
    // ttHH(4b) 분석에서 H→bb 붕괴를 재구성한다.
    // b-jet 4개를 2쌍으로 나누어 각 쌍의 불변질량(invariant mass)이
    // Higgs 질량 (125 GeV)에 가장 가까운 조합을 χ² 최소화로 선택한다.
    //
    // [최적화] Pair Cache 기반 알고리즘:
    //   1) 모든 b-jet 쌍의 불변질량 + pT합을 O(N²) 으로 미리 계산 (precompute)
    //   2) C(N,4) × 3 pairing 순회 시 TLorentzVector 덧셈을 재사용
    //   → 기존 대비 TLorentzVector 연산량 ~60% 절감
    //
    // 복잡도 (Complexity): O(N²) 캐시 + O(N⁴) 탐색 (기존과 동일)
    //   nbJets=4: 3 pairings (최소)
    //   nbJets=6: 45 pairings
    //   nbJets=8: 210 pairings
    // ──────────────────────────────────────────────────────────────────────
    _minChi2Higgs = cLargeValue;
    _bbMassMin1Higgs = -1.0f;
    _bbMassMin2Higgs = -1.0f;

    auto* bjets = thisEvent->getSelbJets();

    // Higgs reco는 b-jet >= 4 일 때만 수행. 부족하면 _minChi2Higgs를 sentinel
    // (cLargeValue)로 남겨 downstream(fillCutStepHist)이 "reco 없음"으로 처리.
    // [STEP2] kValidationStudy 제거에 따라 _valCfg 가변 하한 분기 삭제.
    const int hRecoMin = 4;
    if (_policy.doHiggsReconstruction) {
        const size_t nb = bjets->size();
        // Also guard against hRecoMin <= 0 (effectively disabled by user).

        if (hRecoMin > 0 && nb >= static_cast<size_t>(hRecoMin)) {
            // ═════════════════════════════════════════════════════════════
            // [STEP6] di-mother 재구성 — HiggsReconstructor 클래스로 위임.
            // pair cache + C(N,4)×3 pairing 한 번의 sweep으로 HH/ZH/ZZ 가설을
            // 동시에 평가한다 (기존 인라인 코드는 HH만; 알고리즘·χ² 식·순회
            // 순서는 비트 동일 — docs/changes/STEP_6 oracle 테스트).
            // ═════════════════════════════════════════════════════════════
            std::vector<const TLorentzVector*> bjp4;
            bjp4.reserve(nb);
            for (size_t i = 0; i < nb; ++i) bjp4.push_back(bjets->at(i)->getp4());

            _higgsReco.reconstruct(bjp4, cHiggsMass, cZMass);

            // HH — 기존 멤버로 복사 (갱신 조건도 원본과 동일: sentinel보다 좋을 때)
            const HiggsReconstructor::PairResult& hh = _higgsReco.HH();
            if (hh.valid() && _minChi2Higgs > hh.chi2) {
                _minChi2Higgs    = hh.chi2;
                _bbMassMin1Higgs = hh.mass1;
                _bbMassMin2Higgs = hh.mass2;
                _bpTHiggs1       = hh.pt1;
                _bpTHiggs2       = hh.pt2;
            }
            // ZH / ZZ — 신규 가설은 tree branch로만 기록 (히스토는 추후)
            const HiggsReconstructor::PairResult& zh = _higgsReco.ZH();
            const HiggsReconstructor::PairResult& zz = _higgsReco.ZZ();
            _hr_chi2ZH = zh.chi2; _hr_mZcandZH = zh.mass1; _hr_mHcandZH = zh.mass2;
            _hr_chi2ZZ = zz.chi2; _hr_mZ1ZZ    = zz.mass1; _hr_mZ2ZZ    = zz.mass2;

            // [STEP6][debug] 가설별 χ² 추적 — 종료 요약에서 chi2HH ≤ (ZH 대비
            // 신호스러움) 등 분포 sanity + NaN 검출
            _dbg.kv("higgsreco", "chi2HH", _minChi2Higgs);
            _dbg.kv("higgsreco", "chi2ZH", _hr_chi2ZH);
            _dbg.kv("higgsreco", "chi2ZZ", _hr_chi2ZZ);
            _dbg.kv("higgsreco", "mHH1",   _bbMassMin1Higgs);
        } // end if (hRecoMin > 0 && nb >= hRecoMin)
    } // end if doHiggsReconstruction


    // ══════════════════════════════════════════════════════════════════════
    // Selection Cuts (이벤트 선택 조건)
    //
    // 각 단계에서 조건을 충족하지 못하면 false를 반환하여 이벤트를 버린다.
    // processStep()으로 cutflow를 기록한다.
    // ══════════════════════════════════════════════════════════════════════

    // ══════════════════════════════════════════════════════════════════════
    // [STEP3] 선언적 cut 테이블 실행
    //
    // selection의 "무엇을/어떤 순서로/어느 모드에서" 는 전부 kCutSequence
    // (이 파일, selectObjects 바로 위)에 표로 정의되어 있다. 이 루프는 그
    // 표를 순서대로 실행할 뿐이다:
    //   - pass 실패 + 강제 모드(enforceIn)      → 이벤트 reject
    //   - pass 성공 또는 비강제(관찰)            → cutflow 기록 (recordOnlyIfPass
    //                                            가 true인 단계는 성공시에만 기록)
    //   - 생존 시 onAfter 부수 작업 실행 (통계 계산, SF 적용)
    // STEP3 이전 if-나열과의 동작 동일성: docs/changes/STEP_3_*.md §5 검증표.
    // ══════════════════════════════════════════════════════════════════════
    const uint8_t mBit = selectionModeBit(_analysisMode);
    for (const CutDef& c : kCutSequence) {
        bool ok = (c.pass == nullptr) ? true
                                      : c.pass(*this, *thisEvent, hadWMass);
        // [muon-val] 옵션이 켜지면 lepton veto cut을 "정확히 1 muon + 0 e"로 치환
        // (테이블 람다는 capture-less라 여기서 런타임 분기; 다른 step은 불변).
        if (_require1Muon && c.step == CutStep::kLeptonVeto) {
            ok = (thisEvent->getnSelMuon() == 1 && thisEvent->getnSelElectron() == 0);
        }
        // [lepton-CR] lepton veto step 을 '1ℓ + 반대flavor 0 + MET>cut' 으로 치환
        if (!_lepCRmode.empty() && c.step == CutStep::kLeptonVeto) {
            const int nMu = thisEvent->getnSelMuon();
            const int nEl = thisEvent->getnSelElectron();
            const float met = metPt_();   // [STEP 24] 2024: PuppiMET (N3)
            if (_lepCRmode == "muon")
                ok = (nMu == 1 && nEl == 0 && met > metCutCR);
            else // "electron"
                ok = (nEl == 1 && nMu == 0 && met > metCutCR);
        }
        if (!ok && (c.enforceIn & mBit)) {
            return false;                             // 강제 cut 실패 → reject
        }
        if (ok || !c.recordOnlyIfPass) {
            processStep(c.step, hadWMass);            // cutflow 기록
        }
        if (c.onAfter) {
            c.onAfter(*this, *thisEvent);             // 생존 시 부수 작업
        }
    }

    return true;
}


// [STEP8] motherReco() 제거 — 호출처 0인 죽은 메서드 (HiggsReconstructor로 대체됨).
//         원형은 docs/backup_20260611 참조.

// ═══════════════════════════════════════════════════════════════════════════
// analyze — selection 이후·기록 이전의 파생량 계산 단계.
// 현재: [STEP5] event shape (jets/b-jets 각 5변수). Higgs reco는 selectObjects
// 내 HiggsReconstructor 호출로 수행됨에 유의 (b-jet 확정 직후가 필요해서).
// ═══════════════════════════════════════════════════════════════════════════
void ttHHanalyzer_unified::analyze(event *thisEvent){

    // ══════════════════════════════════════════════════════════════════════
    // [STEP5] Event shape 계산 — selected jets / b-jets의 momentum tensor
    // (EventShape 라이브러리: sphericity/aplanarity/C/D; di-lepton analyzer의
    //  소스-include 방식 대신 libEventShape.a 링크 사용)
    // 객체 2개 미만이면 tensor가 퇴화하므로 계산하지 않음 (-1 유지).
    // ══════════════════════════════════════════════════════════════════════
    {
        std::vector<TVector3> vJet, vBjet;
        for (auto* j : *thisEvent->getSelJets())  vJet.push_back(j->getp4()->Vect());
        for (auto* b : *thisEvent->getSelbJets()) vBjet.push_back(b->getp4()->Vect());

        if (vJet.size() >= 2) {
            EventShape es(vJet);
            _es_aplanarity      = static_cast<Float_t>(es.getAplanarity());
            _es_sphericity      = static_cast<Float_t>(es.getSphericity());
            _es_transSphericity = static_cast<Float_t>(es.getTransSphericity());
            _es_C               = static_cast<Float_t>(es.getC());
            _es_D               = static_cast<Float_t>(es.getD());
        }
        if (vBjet.size() >= 2) {
            EventShape esB(vBjet);
            _es_bjetAplanarity      = static_cast<Float_t>(esB.getAplanarity());
            _es_bjetSphericity      = static_cast<Float_t>(esB.getSphericity());
            _es_bjetTransSphericity = static_cast<Float_t>(esB.getTransSphericity());
            _es_bjetC               = static_cast<Float_t>(esB.getC());
            _es_bjetD               = static_cast<Float_t>(esB.getD());
        }
        // [STEP5][debug] 정의역 검증: sphericity∈[0,1], aplanarity∈[0,0.5],
        // C,D∈[0,1] — 범위 밖/NaN이면 종료 요약의 BAD/minmax로 드러난다.
        _dbg.kv("evtshape", "sphericity", _es_sphericity);
        _dbg.kv("evtshape", "aplanarity", _es_aplanarity);
        _dbg.kv("evtshape", "C",          _es_C);
        _dbg.kv("evtshape", "D",          _es_D);
        _dbg.kv("evtshape", "bjetSphericity", _es_bjetSphericity);
    }


    //    std::map<std::string, float> testVars = HypoComb->GetBestPermutation(getLepP4(thisEvent),getJetP4(thisEvent),getJetCSV(thisEvent),*(thisEvent->getMET()->getp4()));
    //    HypoComb.GetBestPermutation(getLepP4(thisEvent),getJetP4(thisEvent),getJetCSV(thisEvent),*(thisEvent->getMET()->getp4()));
    //    std::cout<< "BLR: " << testVars["Evt_blr"] << std::endl;

}


// ═══════════════════════════════════════════════════════════════════════════
// process — 이벤트 단위 메인 파이프라인
//   ① per-event reset (stitchWeight, event shape, ZH/ZZ 결과, expandedTtbarId)
//   ② weight 조립 (base × PU × L1Prefire × genWeight)
//   ③ computeBTagWeight (shape SF; selection 전, JEC/JER 적용 후 kinematics)
//   ④ expandedTtbarId resolve + stitch multiplier 적용/기록
//   ⑤ createObjects → selectObjects(cut 테이블+SF) → analyze → fillHistos → fillTree
// ═══════════════════════════════════════════════════════════════════════════
void ttHHanalyzer_unified::process(event* thisEvent, sysName sysType, bool up){

    // 1) Golden JSON filter for data
    _failGoldenJson = false;
    if (_DataOrMC == "Data") {
        int run  = _ev->run;
        int lumi = _ev->luminosityBlock;
        if (!corrMgr->passGoldenJSON(run, lumi)) {
            if (debugCorrections) {
                std::cout << "[GoldenJSON] Skipping event: run=" 
                          << run << " lumi=" << lumi << std::endl;
            }
            _failGoldenJson = true;
            return;
        }
        if (debugCorrections) {
            std::cout << "[GoldenJSON] Accepted event: run=" 
                      << run << " lumi=" << lumi << std::endl;
        }
    }

    // [tt+nb] per-event default. Overwritten in the MC block below for tt+nb
    // hits. Data has no genTtbarId branch, so leave -1.
    _genTtbarIdNano  = -1;
    _expandedTtbarId = -1;
    _stitchWeight    = 1.0f;   // [stitch] tree branch default (Data / non-plan / gated modes)
    // [STEP6] 추가 가설 결과 per-event reset (-1 = 미수행)
    _hr_chi2ZH = _hr_mZcandZH = _hr_mHcandZH = -1.f;
    _hr_chi2ZZ = _hr_mZ1ZZ    = _hr_mZ2ZZ    = -1.f;
    // [STEP5] event shape per-event reset (-1 = 미계산; analyze()에서 채움)
    _es_aplanarity = _es_sphericity = _es_transSphericity = _es_C = _es_D = -1.f;
    _es_bjetAplanarity = _es_bjetSphericity = _es_bjetTransSphericity = _es_bjetC = _es_bjetD = -1.f;
    _dbg.nextEvent();           // [STEP2][debug] 이벤트 경계 (kDebug에서만 동작)

    if (debugCorrections) {
	std::cout << "[process] : Current evtWeight="<< _evtWeight << std::endl;
    }

    _PUWeight = 1.0;
    // 2) Pileup reweighting for MC
    if (_DataOrMC != "Data") {
        float nTrue = _ev->Pileup_nTrueInt;
        _PUWeight = corrMgr->getPUWeight(nTrue, "nominal");
        if (debugCorrections) {
            std::cout << "[PU] nTrue=" << nTrue 
                      << " weight=" << _PUWeight;
        }
    }

    // 3) L1 pre-firing weight
    // [2018] 2018 에는 L1 ECAL prefiring 이 없다 (AN p.118: "this effect does not
    //   affect 2018 data-taking era") — 그래서 2018 NanoAODv9 에는
    //   `L1PreFiringWeight_Nom` branch 자체가 없다. eventBuffer 는 없는 branch 를
    //   0 으로 남기므로, 연도 게이트 없이 곱하면 **모든 MC 이벤트 weight 가 0**
    //   이 된다. 크래시도 경고도 없이 히스토그램만 전부 비는 무증상 실패라
    //   반드시 EraConfig 로 분기한다.
    _L1PrefiringWeight = 1.0;
    if (_DataOrMC != "Data" && EraConfig::usesL1Prefiring(_runYear)) {
	_L1PrefiringWeight = _ev->L1PreFiringWeight_Nom;

	// 게이트를 통과한 연도(2016/2017)에서 0 이 나오면 그건 branch 누락이지
	// 물리가 아니다. 조용히 통과시키지 않는다.
	if (!(_L1PrefiringWeight > 0.f)) {
	    std::cerr << "\n[FATAL][E" << tthh::CONFIG_BAD_RUNINFO
		      << "] L1PreFiringWeight_Nom = " << _L1PrefiringWeight
		      << " for runYear=" << _runYear << ".\n"
		      << "  This year is expected to HAVE the prefiring branch; a\n"
		      << "  non-positive value means the branch is missing or unread,\n"
		      << "  which would zero out every MC event weight.\n";
	    tthh::fatalExit(tthh::CONFIG_BAD_RUNINFO);
	}

        if (debugCorrections) {
	    std::cout << "[L1PreFiring] weight=" << _L1PrefiringWeight <<std::endl;
	}
    }


    // 4) gen weight
    _genWeight = 1.0;
    if (_DataOrMC != "Data") {
        _genWeight = _ev->genWeight;
	if (debugCorrections) {
            std::cout << "[genWeight] weight=" << _genWeight <<std::endl;
        }
    }

    // 5) Build physics objects in thisEvent from the raw buffer
    // Event cleaning, Trigger set (Here, event are not rejected, just define filter, trig, etc...
    // and object definitions
    // ═══════════════════════════════════════════════════════════════════════
    // Step: 물리 객체 생성 (JEC/JER 적용됨)
    // ═══════════════════════════════════════════════════════════════════════
    createObjects(thisEvent, sysType, up);

    // 6) Calculate Event Weight
    _evtWeight = _baseWeight; // base weights are got from json config
    if (_DataOrMC != "Data") {
	_evtWeight *= _PUWeight;
	_evtWeight *= _L1PrefiringWeight;
	_evtWeight *= _genWeight;
	// [STEP2][debug] weight 구성요소 추적 (kDebug에서만 동작; NaN/Inf 집계 포함)
	_dbg.kv("weight", "genWeight",        _genWeight);
	_dbg.kv("weight", "PUWeight",         _PUWeight);
	_dbg.kv("weight", "L1Prefire",        _L1PrefiringWeight);
	_dbg.kv("weight", "base_xsecLumi",    _baseWeight);
	_dbg.kv("weight", "evtWeight_preSF",  _evtWeight);
        if(debugCorrections) std::cout << "Final Event Weight: " << _evtWeight << std::endl;
    }

    // ═══════════════════════════════════════════════════════════════════════
    // Step: B-tag Weight 계산 (object 단계, selection 전)
    // JEC/JER이 적용된 jet kinematics를 사용
    // ═══════════════════════════════════════════════════════════════════════
    computeBTagWeight(thisEvent);

    // ═══════════════════════════════════════════════════════════════════════
    // tt+jets categorization 2-way validation (runs on ALL MC events,
    // pre-selection).  [STEP17] full-NanoAOD 전환으로 ntuple ttCat_*/ttCatXval_*
    // branch 가 없어 NTU_PRIMARY/NTU_XVAL 추정자를 제거했다. 남은 두 source:
    //
    //   ANA_GENPART  analyzer's own GenPart algorithm
    //                (computeTtCategoryFromGenPart)
    //   ANA_GENID    analyzer's decode of genTtbarId%100
    //                (computeTtCategoryFromGenTtbarId)
    //
    // Expected:
    //   ANA_GENID vs ANA_GENPART     ~97% 일치 (POG vs GenPart, real diff)
    //
    // Downstream physics 는 officialTtCategory() 를 쓰며, 이는 이제 ANA_GENID
    // (표준 genTtbarId 디코드)를 반환한다 — CMS GenTtbarCategorizer 가 NanoAOD
    // 단계에서 계산한 값으로 옛 ntuplizer ttCat_* primary 와 값 동일.
    //   근거: CMS GenTtbarCategorizer.cc; ttHH AN-2022/122 §3.3·§3.4; ttH AN §A.2.1.
    // ═══════════════════════════════════════════════════════════════════════
    if (_DataOrMC != "Data") {

        // ── Branch availability check (first event only) ──
        static bool branchCheckDone = false;
        if (!branchCheckDone) {
            branchCheckDone = true;
            std::cout << "\n[TTCAT_BRANCH_CHECK] ═══════════════════════════════\n";
            std::cout << "  GenPart_pdgId.size()             = " << _ev->GenPart_pdgId.size() << "\n";
            std::cout << "  GenPart_statusFlags.size()       = " << _ev->GenPart_statusFlags.size() << "\n";
            std::cout << "  GenPart_genPartIdxMother.size()  = " << _ev->GenPart_genPartIdxMother.size() << "\n";
            std::cout << "  GenPart_eta.size()               = " << _ev->GenPart_eta.size() << "\n";
            std::cout << "  GenPart_phi.size()               = " << _ev->GenPart_phi.size() << "\n";
            std::cout << "  GenJet_pt.size()                 = " << _ev->GenJet_pt.size() << "\n";
            std::cout << "  GenJet_eta.size()                = " << _ev->GenJet_eta.size() << "\n";
            std::cout << "  GenJet_phi.size()                = " << _ev->GenJet_phi.size() << "\n";
            std::cout << "  GenJet_hadronFlavour.size()      = " << _ev->GenJet_hadronFlavour.size() << "\n";
            std::cout << "  genTtbarId                       = " << _ev->genTtbarId << "\n";
            // [STEP17] full-Nano: ntuple ttCat_*/ttCatXval_* branch 부재 — 출력 제거.
            if (_ev->GenPart_statusFlags.size() == 0) {
                std::cout << "  *** WARNING: GenPart_statusFlags is EMPTY! ***\n"
                          << "  *** isLastCopy cannot be checked -> all events will fall to LightFlavour ***\n";
            }
            if (_ev->GenPart_genPartIdxMother.size() == 0) {
                std::cout << "  *** WARNING: GenPart_genPartIdxMother is EMPTY! ***\n"
                          << "  *** Top ancestor check cannot work. ***\n";
            }
            if (_ev->GenJet_hadronFlavour.size() == 0) {
                std::cout << "  *** WARNING: GenJet_hadronFlavour is EMPTY! ***\n"
                          << "  *** No b-jets can be identified. ***\n";
            }
            std::cout << "[TTCAT_BRANCH_CHECK] ═══════════════════════════════\n\n";
        }

        // ── [tt+nb] attach extended ttbar-Id from the per-sample lookup ──
        //   in lookup -> Expanded_genTtbarId (sub-code 61/62/71/72)
        //   not in    -> NanoAOD genTtbarId unchanged
        // resolve() also self-checks the lookup's genTtbarId against NanoAOD's
        // for this (run,lumi,event); a mismatch means the wrong ttnb file.
        _genTtbarIdNano  = _ev->genTtbarId;
        _expandedTtbarId = _expTtbarId.resolve(_ev->run, _ev->luminosityBlock,
                                               _ev->event, _ev->genTtbarId);

        // ── [stitch] per-(sample,category) MULTIPLIER on top of the YAML base ──
        // Applied HERE — after expandedTtbarId is resolved, before selectObjects
        // seeds the SF chains — so BOTH the main analysis and the btagtrig
        // reweight-derivation see the stitched MC composition.
        //   category = sub_to_category[expandedTtbarId % 100]
        //   inclusive : 1 on kept HF cats / 0 on rejected (filled by a dedicated)
        //   dedicated : r on owned cats / 0 elsewhere
        //   not in plan: 1 (untouched).  A 0 drops the event from THIS sample.
        // See ttbarCategorization.md s10 (stitch) / s11 (this application).
        if (_stitch.inPlan()) {
            const double stitchMult = _stitch.factor(_expandedTtbarId, _evtWeight);
            _evtWeight *= stitchMult;
            _stitchWeight = static_cast<float>(stitchMult);  // [stitch] -> output tree branch
            _dbg.kv("stitch", "mult",            stitchMult);          // [STEP2][debug]
            _dbg.kv("stitch", "expandedTtbarId", (double)_expandedTtbarId);
            if (debugCorrections) {
                std::cout << "[stitch] sample=" << _sampleName
                          << " expandedTtbarId=" << _expandedTtbarId
                          << " cat=" << _stitch.category(_expandedTtbarId)
                          << " mult=" << stitchMult
                          << " -> _evtWeight=" << _evtWeight << std::endl;
            }
        }

        // ── [STEP17] full-Nano: 2 estimator (ntuple ttCat_*/ttCatXval_* 부재) ──
        //   ANA_GENPART : computeTtCategoryFromGenPart()   (GenPart 알고리즘)
        //   ANA_GENID   : computeTtCategoryFromGenTtbarId() (표준 genTtbarId 디코드)
        // officialTtCategory() 가 쓰는 것은 ANA_GENID. 둘은 POG vs GenPart 차이로
        // ~97% 일치(완전 일치 아님; real algorithmic diff).
        const TtCat anaGenPart = computeTtCategoryFromGenPart();
        const TtCat anaGenId   = computeTtCategoryFromGenTtbarId();

        const int anaGenPartIdx = static_cast<int>(anaGenPart);
        const int anaGenIdIdx   = static_cast<int>(anaGenId);

        // Did the two estimators disagree?
        const bool anyDisagreement = (anaGenPartIdx != anaGenIdIdx);

        // ── Per-event debug print ──
        // First 20 events always; events 20–100 only if a "must agree"
        // pair disagrees, or if the event lands in a heavy-flavour
        // category (which is the interesting working point for ttHH).
        static int dbgCount = 0;
        const bool isHeavyFlav = (anaGenPart == TtCat::kAdd1Bjet1Had ||
                                  anaGenPart == TtCat::kAdd1Bjet2Had ||
                                  anaGenPart == TtCat::kAdd2Bjet);
        const bool shouldPrint =
            (dbgCount < 5) ||
            (dbgCount < 10 && (anyDisagreement || isHeavyFlav));

        if (shouldPrint) {
            std::cout << "\n===== [ttCatDebug] Event #" << dbgCount
                      << " (run:lumi:evt = " << _ev->run << ":"
                      << _ev->luminosityBlock << ":" << _ev->event
                      << ") =====\n";

            // 1) Sizes
            std::cout << "  [Sizes] GenPart=" << _ev->GenPart_pdgId.size()
                      << " GenJet=" << _ev->GenJet_pt.size()
                      << " GenJet_hadFlav=" << _ev->GenJet_hadronFlavour.size() << "\n";

            // 2) genTtbarId
            std::cout << "  [genTtbarId] " << _ev->genTtbarId
                      << " (mod100=" << (_ev->genTtbarId % 100) << ")\n";

            // 3) [STEP17] ntuple ttCat_*/ttCatXval_* 부재 (full-Nano) — 출력 제거.

            // 4) tt pair
            const bool hasTT = eventHasTTPair();
            std::cout << "  [analyzer GenPart] hasTTPair=" << hasTT << "\n";

            // 5) Per-jet B hadron map (from analyzer's algorithm)
            if (hasTT && genPartCount() > 0 && genJetCount() > 0) {
                const auto result = countAdditionalBJetsDetailed();

                // First 5 events: per-particle dump of additional B hadrons
                if (dbgCount < 5) {
                    int bhIdx = 0;
                    for (int i = 0; i < genPartCount(); ++i) {
                        if (!isBHadron(_ev->GenPart_pdgId[i])) continue;
                        if (!((_ev->GenPart_statusFlags[i] >> 13) & 1)) continue;
                        if (hasTopAncestor(i)) continue;
                        std::cout << "    BH#" << bhIdx++
                                  << " GP_idx=" << i
                                  << " pdgId=" << _ev->GenPart_pdgId[i]
                                  << " eta=" << _ev->GenPart_eta[i]
                                  << " phi=" << _ev->GenPart_phi[i] << "\n";
                    }
                }

                std::cout << "  [analyzer GenPart] addBJets=" << result.nJets
                          << " addBH(total)=" << result.nHadrons
                          << " addBH(matched)=" << result.nMatched
                          << " addCJets=" << analyzerAdditionalCJetCount() << "\n";

                if (!result.jetBHMap.empty()) {
                    // Sort for deterministic output
                    std::map<int,int> sortedMap(result.jetBHMap.begin(),
                                                result.jetBHMap.end());
                    std::cout << "  [per-jet BH map]";
                    for (const auto& [jetIdx, bhCount] : sortedMap) {
                        std::cout << " GenJet#" << jetIdx
                                  << "(pT=" << _ev->GenJet_pt[jetIdx]
                                  << ",eta=" << _ev->GenJet_eta[jetIdx]
                                  << "):" << bhCount << "BH";
                    }
                    std::cout << "\n";

                    // 1Bjet → 1Had vs 2Had decision
                    if (result.nJets == 1) {
                        const int singleJetBH = sortedMap.begin()->second;
                        std::cout << "  [1Bjet split] single jet has "
                                  << singleJetBH << " matched BH -> "
                                  << (singleJetBH >= 2 ? "Add1Bjet_2Had" : "Add1Bjet_1Had")
                                  << "\n";
                    }
                }
            }

            // 6) [STEP17] 두 estimator 비교
            auto fmt = [](TtCat c) {
                return std::string(ttCatShortName(static_cast<int>(c)));
            };
            std::cout << "  [2-way label] "
                      << "ANA_GENPART=" << fmt(anaGenPart)
                      << "  ANA_GENID=" << fmt(anaGenId)
                      << "\n";

            // 7) Mismatch flag (POG vs GenPart real diff)
            if (anyDisagreement) {
                std::cout << "  [info] ANA_GENPART != ANA_GENID"
                          << " (POG vs GenPart, real algorithmic diff)\n";
            } else {
                std::cout << "  [2-way] BOTH AGREE\n";
            }

            ++dbgCount;
        }

        // ── Fill 1D count histograms ── [STEP17] 2 source 만
        _hTtCat_Counts_AnaGenPart->Fill(anaGenPartIdx + 0.5);
        _hTtCat_Counts_AnaGenId  ->Fill(anaGenIdIdx   + 0.5);

        // ── Fill 2D pair-wise comparison (GenPart vs genTtbarId 디코드) ──
        _hTtCat_AnaGenPart_vs_AnaGenId->Fill(anaGenPartIdx + 0.5, anaGenIdIdx + 0.5);

        // ── genTtbarId mod 100 vs analyzer GenPart category ──
        const int gtidMod = _ev->genTtbarId % 100;
        _hTtCat_GenTtbarIdMod100->Fill(gtidMod, anaGenPartIdx + 0.5);
    }

    // 7) Baseline selection + event cleaning, apply trigger path, reconstruct higher objects
    if(!selectObjects(thisEvent)) return;
    _passMETFilters = false;
    _passMETFilters = thisEvent->getMETFilter();
    //if(_passMETFilters != true){
    //    std::cout<<"ERROR : MET filter class does not work properly.. It should reject all of events are not passed filters in [ selectObjects ].."<<std::endl;
    //    exit(555);
    //}

    // 5) Final analysis steps: kinematics, histograms, tree
    analyze(thisEvent);
    fillHistos(thisEvent);
    fillTree(thisEvent);

}


void ttHHanalyzer_unified::fillHistos(event * thisEvent){

    //    for(int i=0; i < cutflow.size(); i++){
    //	std::cout << "cutflow size: " << cutflow[i] << std::endl; 
	//	hCutFlow->SetBinContent(1, cutflow[0]);
    //    }
    
    
   int nSel = thisEvent->getnSelJet();
   for(int ih = 0; ih < nSel && ih < nHistsJets; ++ih){
     float JEC_DiffRatio = thisEvent->getSelJets()->at(ih)->JEC_DiffRatio;   // nanoAOD JEC 적용 후 pT
     float Mass_DiffRatio = thisEvent->getSelJets()->at(ih)->mass_DiffRatio;

     // [ 직접 JEC 해체 후 최신 버전 재적용한 JEC - NanoAOD orinigal Pt (NanoAOD의 기본 JEC) ] / [ NanoAOD original Pt ]
     h_JEC_DiffRatio.at(ih)->Fill(JEC_DiffRatio, _evtWeight);
     h_JEC_Mass_DiffRatio.at(ih)->Fill(Mass_DiffRatio, _evtWeight);
   }
  

    /*    hleptonNumber->Fill(thisEvent->getnSelLepton(),_evtWeight*thisEvent->getbTagSys());


    if(thisEvent->getnSelMuon() == 2){
	hDiMuonMass->Fill(thisEvent->getSelMuonsMass(),_evtWeight*thisEvent->getbTagSys());
	hDiMuonPT->Fill(thisEvent->getSelMuonsPT(),_evtWeight*thisEvent->getbTagSys());
	hDiMuonEta->Fill(thisEvent->getSelMuonsEta(),_evtWeight*thisEvent->getbTagSys());
    }

    if(thisEvent->getnSelElectron() == 2){
	hDiElectronMass->Fill(thisEvent->getSelElectronsMass(),_evtWeight*thisEvent->getbTagSys());
	hDiElectronPT->Fill(thisEvent->getSelElectronsPT(),_evtWeight*thisEvent->getbTagSys());
	hDiElectronEta->Fill(thisEvent->getSelElectronsEta(),_evtWeight*thisEvent->getbTagSys());
    }

    hleptonHT->Fill(thisEvent->getSelLeptonHT(),_evtWeight*thisEvent->getbTagSys());
    hST->Fill(thisEvent->getSelLeptonST(),_evtWeight*thisEvent->getbTagSys());
    hLeptonPT1->Fill(thisEvent->getSelLeptons()->at(0)->getp4()->Pt(), _evtWeight*thisEvent->getbTagSys());
    hLeptonEta1->Fill(thisEvent->getSelLeptons()->at(0)->getp4()->Eta(), _evtWeight*thisEvent->getbTagSys());
    hLeptonPT2->Fill(thisEvent->getSelLeptons()->at(1)->getp4()->Pt(), _evtWeight*thisEvent->getbTagSys());
    hLeptonEta2->Fill(thisEvent->getSelLeptons()->at(1)->getp4()->Eta(), _evtWeight*thisEvent->getbTagSys());


    if(thisEvent->getnSelMuon() > 0){
	hMuonPT1->Fill(thisEvent->getSelMuons()->at(0)->getp4()->Pt(), _evtWeight*thisEvent->getbTagSys());
	hMuonEta1->Fill(thisEvent->getSelMuons()->at(0)->getp4()->Eta(), _evtWeight*thisEvent->getbTagSys());
    }
    
    if(thisEvent->getnSelElectron() > 0){
	hElePT1->Fill(thisEvent->getSelElectrons()->at(0)->getp4()->Pt(), _evtWeight*thisEvent->getbTagSys());
	hEleEta1->Fill(thisEvent->getSelElectrons()->at(0)->getp4()->Eta(), _evtWeight*thisEvent->getbTagSys());
    }
    
    if(thisEvent->getnSelMuon() > 1){
	hMuonPT2->Fill(thisEvent->getSelMuons()->at(1)->getp4()->Pt(), _evtWeight*thisEvent->getbTagSys());
	hMuonEta2->Fill(thisEvent->getSelMuons()->at(1)->getp4()->Eta(), _evtWeight*thisEvent->getbTagSys());
    }
    
    if(thisEvent->getnSelElectron() > 1){
	hElePT2->Fill(thisEvent->getSelElectrons()->at(1)->getp4()->Pt(), _evtWeight*thisEvent->getbTagSys());
	hEleEta2->Fill(thisEvent->getSelElectrons()->at(1)->getp4()->Eta(), _evtWeight*thisEvent->getbTagSys());
    } 

    hLepCharge1->Fill(thisEvent->getSelLeptons()->at(0)->charge, _evtWeight*thisEvent->getbTagSys());
    hLepCharge2->Fill(thisEvent->getSelLeptons()->at(1)->charge, _evtWeight*thisEvent->getbTagSys());
    */
}


void ttHHanalyzer_unified::writeHistos(){
    _of->file->cd();
    _histoDirs.at(0)->cd();
    for(int ih=0; ih<nHistsJets; ih++){
//        hjetsPTs.at(ih)->Write();
//        hjetsEtas.at(ih)->Write();
//        hjetsBTagDisc.at(ih)->Write();
  
        // JEC-only 원본 pT
        h_JEC_DiffRatio.at(ih)->Write();
        h_JEC_Mass_DiffRatio.at(ih)->Write();

    }


    _histoDirs.at(1)->cd();
    
    /*    hLepCharge1->Write();
    hLepCharge2->Write();

    hleptonNumber->Write();
    hDiMuonMass->Write();
    hDiElectronMass->Write();
    hDiMuonPT->Write();
    hDiElectronPT->Write();
    hDiMuonEta->Write();
    hDiElectronEta->Write();
    hleptonHT->Write();
    hST->Write();
    hLeptonEta1->Write();
    hLeptonPT1->Write();
    hLeptonEta2->Write();
    hLeptonPT2->Write();
    
    hMuonEta1->Write();
    hMuonPT1->Write();
    hEleEta1->Write();
    hElePT1->Write();
    hMuonEta2->Write();
    hMuonPT2->Write();
    hEleEta2->Write();
    hElePT2->Write(); */

    _histoDirs.at(2)->cd();
    for (size_t i = 0; i < _cutStepJetPt.size(); ++i) {
        for (size_t j = 0; j < kNJetsForCutStep; ++j) {
            _cutStepJetPt.at(i).at(j)->Write();
            _cutStepJetEta.at(i).at(j)->Write();
            _cutStepJetPhi.at(i).at(j)->Write();
            _cutStepBTag.at(i).at(j)->Write();
        }
        _cutStepHT.at(i)->Write();
        _cutStepHadWMass.at(i)->Write();
        _cutStepHiggsMass01.at(i)->Write();
        _cutStepHiggsMass02.at(i)->Write();

        // [NEW] Three-chain per-step hists
        _cutStepHT_btagSF       .at(i)->Write();
        _cutStepHT_full         .at(i)->Write();
        _cutStepHadWMass_btagSF .at(i)->Write();
        _cutStepHadWMass_full   .at(i)->Write();
        _cutStepHiggsMass01_btagSF.at(i)->Write();
        _cutStepHiggsMass01_full  .at(i)->Write();
        _cutStepHiggsMass02_btagSF.at(i)->Write();
        _cutStepHiggsMass02_full  .at(i)->Write();
        _cutStepNbJets_raw      .at(i)->Write();
        _cutStepNbJets_btagSF   .at(i)->Write();
        _cutStepNbJets_full     .at(i)->Write();
        _cutStepNjets_raw       .at(i)->Write();
        _cutStepNjets_btagSF    .at(i)->Write();
        _cutStepNjets_full      .at(i)->Write();
    }

    // ── tt+jets categorization validation histograms (write + summary) ──
    // [STEP17] full-Nano: NTU_* 제거, 2 source + 1 pair + GenTtbarIdMod100 만.
    _histoDirs.at(3)->cd();
    _hTtCat_Counts_AnaGenPart->Write();
    _hTtCat_Counts_AnaGenId  ->Write();

    _hTtCat_AnaGenPart_vs_AnaGenId  ->Write();

    _hTtCat_GenTtbarIdMod100->Write();

    // ── End-of-job summary ──
    //
    // For each estimator: per-category event count (1D).
    // For each pair: diagonal (agreement) and off-diagonal (disagreement) totals,
    // with per-cell off-diagonal listing for the "must-agree" pairs.
    std::cout << "\n[ttCatSummary] ═══════════════════════════════════════════════" << std::endl;
    const double totalEvents = _hTtCat_Counts_AnaGenPart->GetEntries();
    std::cout << "[ttCatSummary] Total MC events processed: "
              << static_cast<long long>(totalEvents) << std::endl;

    // Per-estimator breakdown
    auto dumpCounts = [&](const char* name, TH1F* h) {
        std::cout << "[ttCatSummary] " << name << " breakdown:" << std::endl;
        for (int i = 1; i <= kNTtCat; ++i) {
            const double c = h->GetBinContent(i);
            if (c > 0) {
                std::cout << "  " << ttCatName(i-1) << ": "
                          << static_cast<long long>(c) << std::endl;
            }
        }
    };
    dumpCounts("ANA_GENPART (analyzer GenPart)",   _hTtCat_Counts_AnaGenPart);
    dumpCounts("ANA_GENID   (analyzer genTtbarId)", _hTtCat_Counts_AnaGenId);

    // Pair-wise agreement summary
    auto pairAgreement = [&](const char* label, TH2F* h, bool listOff) {
        double diag = 0, off = 0;
        for (int ix = 1; ix <= kNTtCat; ++ix) {
            for (int iy = 1; iy <= kNTtCat; ++iy) {
                const double c = h->GetBinContent(ix, iy);
                if (ix == iy) {
                    diag += c;
                } else if (c > 0) {
                    off += c;
                    if (listOff) {
                        std::cout << "    OFF-DIAG: " << ttCatName(ix-1)
                                  << " vs " << ttCatName(iy-1)
                                  << " = " << static_cast<long long>(c) << std::endl;
                    }
                }
            }
        }
        const double total = diag + off;
        std::cout << "[ttCatSummary] " << label
                  << "  agree=" << static_cast<long long>(diag)
                  << "  disagree=" << static_cast<long long>(off);
        if (total > 0) {
            std::cout << std::fixed << std::setprecision(4)
                      << "  agreement=" << (diag / total * 100.0) << "%";
        }
        std::cout << std::endl;
    };

    // [STEP17] full-Nano: ntuple ttCat_*/ttCatXval_* 부재 → 단일 pair
    // (POG genTtbarId 디코드 vs GenPart). off-diagonal cell 을 나열한다.
    //
    // [STEP19] 비교의 유효 범위를 샘플로 게이트한다 (진단 출력만 변경; 물리 무관).
    //   - ttbar family (TTbar_*/TTbb_*/TT4b): 두 estimator 모두 유의미.
    //     기대 agreement: HF-enriched >97%, LF-dominated inclusive tt ~73%
    //     (docs/ttbarCategorization.md §4.1).
    //   - 그 외(non-tt: V+jets/QCD/single-t/diboson/ttV/...): NanoAOD
    //     ttbarCategorization sequence 는 모든 MC 에서 돌아 genTtbarId>=0 을
    //     채우고(top 제외 대상이 없어 모든 HF jet 이 "additional"), 디코드는
    //     gtid<0 에서만 noTT 를 내므로 noTT 를 표현할 수 없다. 반면 GenPart
    //     경로는 eventHasTTPair() 게이트로 전부 noTT(정답). → agreement 는
    //     정의상 0% 이며 비교가 무의미하다. 이때 ANA_GENID breakdown 은
    //     "이 샘플의 additional-HF 조성" 참고용으로만 읽을 것.
    if (TtCatGroup::IsTtbarFamily(_sampleName)) {
        std::cout << "[ttCatSummary] --- ANA_GENPART vs ANA_GENID (POG vs GenPart, ~97% 기대) ---" << std::endl;
        pairAgreement("ANA_GENPART vs ANA_GENID    ",
                      _hTtCat_AnaGenPart_vs_AnaGenId,   /*listOff=*/true);
    } else {
        std::cout << "[ttCatSummary] --- ANA_GENPART vs ANA_GENID: SKIPPED (non-tt sample '"
                  << _sampleName << "') ---\n"
                  << "[ttCatSummary]     genTtbarId 디코드는 non-tt 에서 noTT 를 표현 못함(gtid>=0 항상)\n"
                  << "[ttCatSummary]     → agreement 0% 는 정의상 결과. ANA_GENID breakdown 은 additional-HF 조성 참고용."
                  << std::endl;
        pairAgreement("ANA_GENPART vs ANA_GENID    ",
                      _hTtCat_AnaGenPart_vs_AnaGenId,   /*listOff=*/false);
    }

    std::cout << "[ttCatSummary] ═══════════════════════════════════════════════\n"
              << std::endl;

    // [STEP 25 K] b-tag efficiency histograms (MC of a fixed-WP year)
    if (_btagEffDir) {
        _btagEffDir->cd();
        for (auto& row : _hBTagEff)
            for (TH2D* h : row)
                if (h) h->Write();
        if (_hBTagEffNevt) _hBTagEffNevt->Write();
        if (_hBTagEffWP) _hBTagEffWP->Write();
        for (int k = 0; k < 4; ++k) {   // [STEP 27 N] the closure per true b jets / per M-tag multiplicity
            if (_hBTagTrueB[k]) _hBTagTrueB[k]->Write();
            if (_hBTagNbM[k]) _hBTagNbM[k]->Write();
        }
        _of->file->cd();
    }
}
// ═══════════════════════════════════════════════════════════════════════════
// fillTree — selection 통과 이벤트를 flat tree(Tree/Tree)에 기록.
// 파생 도구(bTagSF_ReweightStudy/TriggerStudy)의 입력 skim이 되는 branch들:
//   evtWeight(모든 SF 곱힌 최종), stitchWeight(0/r/1), expandedTtbarId,
//   bTagWeight·triggerSF·btagNormReweight(개별 SF), kinematics, event shape,
//   chi2 가설(HH는 기존 멤버, ZH/ZZ는 [STEP6] branch).
// ═══════════════════════════════════════════════════════════════════════════
void ttHHanalyzer_unified::fillTree(event * thisEvent){

    jetPt.clear();
    jetEta.clear();
    bTagScore.clear();
    hadFlavs.clear();
    partonFlavs.clear();
    jetPUids.clear();

    // For Trigger Path
    passTrigger_HLT_IsoMu27 = _ev->HLT_IsoMu27; // Reference Muon Trigger
    passTrigger_HLT_IsoMu24 = _ev->HLT_IsoMu24; // [STEP 24]
    passTrigger_HLT_PFHT1050 = _ev->HLT_PFHT1050;
    //passTrigger_HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59 = _ev->HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59;
    //passTrigger_HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94 = _ev->HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94;
    //passTrigger_HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5 = _ev->HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5;
    if (_runYear == "2024") {
        // [STEP 24] 2024: the PNet paths in the CDEF slots (the slot = "the main version"); the _B
        //   slots are 0. 4J3T as in the trigger decision: Data the PNet | DeepJet OR, MC the PNet path.
        passTrigger_6J1T_B    = false;
        passTrigger_6J2T_B    = false;
        passTrigger_4J3T_B    = false;
        passTrigger_6J1T_CDEF = _ev->HLT_PFHT450_SixPFJet36_PNetBTag0p35;
        passTrigger_6J2T_CDEF = _ev->HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50;
        passTrigger_4J3T_CDEF = _ev->HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3 ||
            (_DataOrMC == "Data" && _ev->HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5);
    } else {
    passTrigger_6J1T_B    = _ev->HLT_PFHT430_SixJet40_BTagCSV_p080;
    passTrigger_6J1T_CDEF = _ev->HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5;
    passTrigger_6J2T_B    = _ev->HLT_PFHT380_SixJet32_DoubleBTagCSV_p075;
    passTrigger_6J2T_CDEF = _ev->HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2;
    passTrigger_4J3T_B    = _ev->HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07;
    passTrigger_4J3T_CDEF = _ev->HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0;
    }

    nMuons = thisEvent->getnSelMuon();
    nElecs = thisEvent->getnSelElectron();
    // [2026-07-29] main 의 lepton veto 와 동일한 값 (kLeptonVeto 단계가 쓰는 것).
    //   nMuons/nElecs 와 달리 lead-muon gate 와 무관하게 항상 채워진다
    //   (setnVetoLepton() 은 collectLeptons 분기 **밖**에서 호출된다).
    nVetoLeptons = thisEvent->getnVetoLepton();
    nJets = thisEvent->getnSelJet();
    nbJets = thisEvent->getnSelbJet();
    HT = thisEvent->getSumSelJetScalarpT();
    // [2026-07-29] lepton-CR 재현용. _lepCRmode 가 쓰는 것과 **같은 값**이어야
    //   하므로 동일하게 _ev->MET_pt 를 그대로 싣는다 (가공하지 않는다).
    MET_pt = metPt_();   // [STEP 24] the value the lepton CR cut uses (2024: PuppiMET)


    // Fill the jet information [ It will fill the nJets && maximum 30th jets ]
    for (int i = 0; i < nJets; ++i) {

        jetPt.push_back(thisEvent->getSelJets()->at(i)->getp4()->Pt());
        jetEta.push_back(thisEvent->getSelJets()->at(i)->getp4()->Eta());
        bTagScore.push_back(thisEvent->getSelJets()->at(i)->bTagCSV);
        hadFlavs.push_back(thisEvent->getSelJets()->at(i)->hadFlav);
        partonFlavs.push_back(thisEvent->getSelJets()->at(i)->partonFlav);
        jetPUids.push_back(thisEvent->getSelJets()->at(i)->jetPUid);
        
        // In the Event Buffer, you can find the struct jet_s which accesses to defined branch variables in NanoAODv9
        // And then in the .hh files, you also can check variables like  bTagCSV or hadFlav as objectJet class's  member variable
        // Then you can get the info at NanoAODv9 from jet_s structure and put them into the .hh's objectJet class member variable in the iteration of jets
        // Finally here, you can store variables in the objectJet class's member variables into tree
    } 

    eventNumber = _ev->event;
    runNumber = _ev->run;
 
    evtWeight        = _evtWeight;                              // trigSF만
    evtWeight_btagSF = static_cast<float>(_evtWeight_chain_btagSF);  // +btagShape
    evtWeight_full   = static_cast<float>(_evtWeight_chain_full);    // +btagNormRW
    SampleWeight = _SampleWeight;
    PUWeight = _PUWeight;
    L1PrefiringWeight = _L1PrefiringWeight;
    genWeight = _genWeight;
    bTagWeight = bTagWeight_central_;
    failGoldenJson = _failGoldenJson;
    passMETFilters = _passMETFilters;
    passHadTrig = thisEvent->getHadTriggerAccept();

   
    // [STEP 27 P] the Run 2 per-source shape weights also give the STEP 26 branches bTagWeight_up/_down (hf), so they
    //   are computed for every tree event of an MC shape year, with or without Tree v1
    if (_DataOrMC != "Data" && !_btagFixedWP && _hasBTagShapeSF) computeBTagShapeVariations_(thisEvent);
    if (_treeV1) fillTreeV1_(thisEvent);   // [STEP 27 O] ML inputs, jet truth labels, systematic weights

    // [STEP2][debug] tree 기록값 추적 — branch에 실리는 최종값 검증 (kDebug)
    _dbg.kv("tree", "evtWeight",       _evtWeight);
    _dbg.kv("tree", "stitchWeight",    _stitchWeight);
    _dbg.kv("tree", "expandedTtbarId", (double)_expandedTtbarId);
    _inputTree->Fill();
}

// ═══════════════════════════════════════════════════════════════════════════
// [STEP 27 O] Tree v1 — docs/PLAN_ML_SYST.md §5.2. Everything from the selected jets (the order of jetPt) and their
//   b-tag scores; the definitions are in include/TreeVars.h (AN-2022/122 §6.2) and include/GenMatch.h.
// ═══════════════════════════════════════════════════════════════════════════
void ttHHanalyzer_unified::fillTreeV1_(event* thisEvent) {
    const auto* jets = thisEvent->getSelJets();
    const float wpL = objectJet::valbTagLoose, wpM = objectJet::valbTagMedium, wpT = objectJet::valbTagTight;
    const bool isMC = (_DataOrMC != "Data");
    _tv.jetPhi.clear(); _tv.jetMass.clear(); _tv.jetBTagWP.clear();
    TreeVars::P4s pAll, pB, pNonB, pLight;
    std::vector<float> score;
    std::vector<bool> isB;
    std::vector<std::pair<float, float>> etaPhi;
    int nLoose = 0;
    for (const auto* j : *jets) {
        const TLorentzVector* p = j->getp4();
        const float sc = j->bTagCSV;
        pAll.push_back(p);
        score.push_back(sc);
        isB.push_back(sc >= wpM);
        (sc >= wpM ? pB : pNonB).push_back(p);
        if (sc < wpL) pLight.push_back(p);
        if (sc >= wpL) ++nLoose;
        _tv.jetPhi.push_back(static_cast<float>(p->Phi()));
        _tv.jetMass.push_back(static_cast<float>(p->M()));
        _tv.jetBTagWP.push_back(sc >= wpT ? 3 : sc >= wpM ? 2 : sc >= wpL ? 1 : 0);   // [STEP 27 P] a NaN score -> 0
        etaPhi.emplace_back(static_cast<float>(p->Eta()), static_cast<float>(p->Phi()));
    }
    _tv.lightjetNumber      = static_cast<int>(pLight.size());
    _tv.nLooseJets          = nLoose;
    _tv.bjetHT              = TreeVars::scalarHT(pB);
    _tv.lightjetHT          = TreeVars::scalarHT(pLight);
    _tv.jetAverageMass      = TreeVars::averageMass(pAll);
    _tv.bjetAverageMass     = TreeVars::averageMass(pB);
    _tv.lightjetAverageMass = TreeVars::averageMass(pLight);
    _tv.bjetAverageMassSqr  = TreeVars::averageMassSqr(pB);
    _tv.maxPTmassjjj        = TreeVars::maxPTmassjjj(pAll);
    _tv.maxPTmassjbb        = TreeVars::maxPTmassjbb(pAll, isB);
    _tv.jj                  = TreeVars::pairStatsSame(pAll);
    _tv.bb                  = TreeVars::pairStatsSame(pB);
    _tv.bj                  = TreeVars::pairStatsCross(pB, pNonB);
    _tv.fwJet               = TreeVars::foxWolfram(pAll);
    _tv.fwB                 = TreeVars::foxWolfram(pB);
    _tv.centrality          = TreeVars::centrality(pAll);
    _tv.bjetCentrality      = TreeVars::centrality(pB);
    _tv.chi2                = TreeVars::chi2AN(pAll, score, wpL, wpM, kTreeMH, kTreeMZ);
    _tv.metPhi              = metPhi_();

    // jet truth labels (MC): GenMatch.h
    if (isMC) {
        std::vector<GenMatch::GenP> gp;
        gp.reserve(_ev->GenPart.size());
        for (const auto& g : _ev->GenPart)
            gp.push_back({g.pdgId, g.genPartIdxMother, g.statusFlags, g.pt, g.eta, g.phi});
        GenMatch::matchJets(etaPhi, GenMatch::hardPartons(gp), 0.4f, _tv.jetGenMatch, _tv.jetGenMotherIdx,
                            &_tv.jetGenTopIdx);
    } else {
        _tv.jetGenMatch.assign(jets->size(), 0);
        _tv.jetGenMotherIdx.assign(jets->size(), -1);
        _tv.jetGenTopIdx.assign(jets->size(), -1);
    }

    // systematic weights (MC)
    _tv.PUWeight_up = _tv.PUWeight_down = 1.f;
    _tv.L1PrefiringWeight_up = _tv.L1PrefiringWeight_down = 1.f;
    _tv.LHEScaleWeight.clear(); _tv.PSWeight.clear(); _tv.LHEPdfWeight.clear();
    // _tv.bTagShapeVar: set by computeBTagShapeVariations_ in fillTree (MC of a shape year); 1 otherwise, never changed
    if (!isMC) return;
    const double nTrue = _ev->Pileup_nTrueInt;
    _tv.PUWeight_up   = static_cast<float>(corrMgr->getPUWeight(nTrue, "up"));
    _tv.PUWeight_down = static_cast<float>(corrMgr->getPUWeight(nTrue, "down"));
    if (EraConfig::usesL1Prefiring(_runYear)) {   // 2016/2017 (AN p.118); the branches are required then
        _tv.L1PrefiringWeight_up   = _ev->L1PreFiringWeight_Up;
        _tv.L1PrefiringWeight_down = _ev->L1PreFiringWeight_Dn;
    }
    _tv.LHEScaleWeight = _ev->LHEScaleWeight;
    _tv.PSWeight       = _ev->PSWeight;
    if (_treePdf) _tv.LHEPdfWeight = _ev->LHEPdfWeight;
    if (!_treeWeightInfoDone) {   // once per job: what the sample's theory-weight vectors hold
        _treeWeightInfoDone = true;
        std::cout << "[treeV1] first tree event of " << _sampleName << ": LHEScaleWeight " << _ev->LHEScaleWeight.size()
                  << ", PSWeight " << _ev->PSWeight.size() << ", LHEPdfWeight " << _ev->LHEPdfWeight.size()
                  << (_treePdf ? " (stored)" : " (not stored; --tree-pdf on)") << " entries; PU up/down "
                  << _tv.PUWeight_up << "/" << _tv.PUWeight_down << "; b-tag "
                  << (_btagFixedWP ? "fixed-WP (bTagWeight_up/_down, _oldRule, _cAsB)"
                                   : (_hasBTagShapeSF ? "shape per source (16 branches)" : "none"))
                  << std::endl;
    }
}

void ttHHanalyzer_unified::writeTree(){
    _of->file->cd();
    _treeDirs.at(0)->cd();
    _inputTree->Write();
    //    _inputTree->Delete();
    
}

// ============================================================================
// [kPrescan] mode — gen-level accumulators
//
// ⚠ TEMPORARY IMPLEMENTATION.
// This logic should eventually live inside NtupleForge so that the
// per-sample Σ genWeight, the per-(genTtbarId %% 100) breakdown, and
// the NanoAOD Runs tree numbers are written into the produced ntuple
// as a tiny friend tree alongside the Events tree. That removes the
// need to re-open files in the analyzer just to read the Runs tree,
// and guarantees consistency between skim-stage counting and the
// downstream normalisation.
//
// AN refs:
//   ttHH AN-2022/122 §3.3   (SL Option1 — gen-level partition for stitching)
//   ttHH AN-2022/122 §3.4   (DL — gen-level 4b exclusion)
//   ttH  AN-19-094  §6.2.1  (FH/SL/DL — NNLO+NNLL anchoring procedure)
//   ttH  AN-19-094  App. C  (rate/shape uncertainty treatment)
// ============================================================================

void ttHHanalyzer_unified::runPrescan() {
    print("--------------------------------------------------------------------------", "b");
    print("[Prescan] Running kPrescan mode — no selection applied.", "b");
    print("[Prescan] Sample: " + _sampleName, "b");
    print("--------------------------------------------------------------------------", "b");

    requirePrescanBranches_();   // [STEP 24] genWeight / genTtbarId / run-LS-event: no silent 0 here either

    // 1) Read the Runs tree(s) BEFORE the Events loop.
    //    Uses a fresh TFile per input file; never touches eventBuffer's TFile.
    readRunsTreeSums();

    // 2) Events loop — gen-level scalars only (no createObjects / selectObjects).
    const long long nEv = _ev->size();
    std::cout << "[Prescan] Events tree: " << nEv << " entries" << std::endl;
    for (long long i = 0; i < nEv; ++i) {
        _ev->read(i);
        accumulatePrescanEvent();

        if (i % 200000 == 0) {
            std::cout << "[Prescan] processed " << i << " / " << nEv << std::endl;
        }
    }

    // 3) Dump one-row TTree + console summary.
    writePrescanTree();
}

// ----------------------------------------------------------------------------

void ttHHanalyzer_unified::readRunsTreeSums() {
    // Acquire the input file list via itreestream's public accessor.
    // (See treestream.h: std::vector<std::string> itreestream::filenames();)
    const std::vector<std::string> fnames = _ev->input->filenames();
    std::cout << "[Prescan] Runs tree: scanning " << fnames.size()
              << " input file(s)" << std::endl;

    _prescan_runs_sumW          = 0.0;
    _prescan_runs_sumW2         = 0.0;
    _prescan_runs_count         = 0;
    _prescan_nFilesProcessed    = 0;

    // [2026-10-06] MC: every input file must give its Runs sum -- the sample's normalization is Σgenw(Runs) over
    //   ALL its files. A file that cannot be opened, or has no Runs tree / genEventSumw, used to be skipped with a
    //   warning only (the job ended with exit 0 and the weight of the sample came out too large); now it stops the
    //   job (E30). Data keeps the warning (no genEventSumw there).
    const bool mc = (_DataOrMC != "Data");
    auto runsFatal = [&](const std::string& fn, const std::string& why) {
        std::cerr << "\n[FATAL][E" << tthh::INPUT_OPEN_FAIL << "] [Prescan] " << why << ": " << fn << "\n"
                  << "  The sample's Σgenw(Runs) (its MC normalization) needs every input file. Check the file, then\n"
                  << "  send the job again (submit_job_FH_Tier3_unified.py --mode prescan ... --resubmit).\n";
        tthh::fatalExit(tthh::INPUT_OPEN_FAIL);
    };
    for (const auto& fn : fnames) {
        // Fresh TFile, disjoint from eventBuffer's file handle.
        std::unique_ptr<TFile> f(TFile::Open(fn.c_str(), "READ"));
        if (!f || f->IsZombie()) {
            if (mc) runsFatal(fn, "cannot open");
            std::cerr << "[Prescan][WARN] cannot open: " << fn << std::endl;
            continue;
        }

        TTree* runs = nullptr;
        f->GetObject("Runs", runs);
        if (!runs) {
            if (mc) runsFatal(fn, "no Runs tree");
            continue;                                // Data: no Runs/genEventSumw needed
        }
        if (mc && !runs->GetBranch("genEventSumw")) runsFatal(fn, "no genEventSumw in the Runs tree");

        Double_t  sumW_one   = 0.0;
        Double_t  sumW2_one  = 0.0;
        Long64_t count_one  = 0;

        if (runs->GetBranch("genEventSumw"))
            runs->SetBranchAddress("genEventSumw",  &sumW_one);
        if (runs->GetBranch("genEventSumw2"))
            runs->SetBranchAddress("genEventSumw2", &sumW2_one);
        if (runs->GetBranch("genEventCount"))
            runs->SetBranchAddress("genEventCount", &count_one);

        const Long64_t nR = runs->GetEntries();
        for (Long64_t r = 0; r < nR; ++r) {
            // Reset before each entry in case some branches are absent
            sumW_one = 0.0; sumW2_one = 0.0; count_one = 0;
            runs->GetEntry(r);
            _prescan_runs_sumW  += sumW_one;
            _prescan_runs_sumW2 += sumW2_one;
            _prescan_runs_count += count_one;
        }
        ++_prescan_nFilesProcessed;
        // unique_ptr<TFile>::Close() happens here at scope exit.
    }

    if (mc && _prescan_nFilesProcessed != static_cast<int>(fnames.size())) {   // cannot happen after the checks above
        std::cerr << "\n[FATAL][E" << tthh::INPUT_OPEN_FAIL << "] [Prescan] Runs read in " << _prescan_nFilesProcessed
                  << " of " << fnames.size() << " input files.\n";
        tthh::fatalExit(tthh::INPUT_OPEN_FAIL);
    }
    std::cout << "[Prescan] Runs tree totals: "
              << "Σgenw=" << _prescan_runs_sumW
              << "  Σgenw²=" << _prescan_runs_sumW2
              << "  count=" << _prescan_runs_count
              << "  (over " << _prescan_nFilesProcessed << " files)" << std::endl;
}

// ----------------------------------------------------------------------------

void ttHHanalyzer_unified::accumulatePrescanEvent() {
    // Treat Data as gw=1 so nEvents_total is still meaningful.
    const double gw = (_DataOrMC != "Data") ? _ev->genWeight : 1.0;

    ++_prescan_nEvents_total;
    _prescan_sumGW_total += gw;
    if      (gw > 0) _prescan_sumGW_pos += gw;
    else if (gw < 0) _prescan_sumGW_neg += gw;

    if (_DataOrMC == "Data") return;

    // --- 11-bin raw genTtbarId %% 100 (stitching-partition input) ---
    //     Σgenw bins and unweighted-count bins are accumulated together.
    const int gtid = _ev->genTtbarId;
    // [tt+nb] resolve the expanded id; tt+nb events (sub 61/62/71/72) are routed
    // to their own bins and REMOVED from the 53/54/55 bins, so 53/54/55 become
    // the pure tt+bb remainder. Lookup is injected in main() regardless of mode.
    const int eid  = _expTtbarId.resolve(_ev->run, _ev->luminosityBlock,
                                         _ev->event, gtid);
    const int esub = ((eid % 100) + 100) % 100;
    if (gtid < 0) {
        _prescan_sumGW_id_lt0 += gw; ++_prescan_n_id_lt0;
    } else if (esub == 61) { _prescan_sumGW_id_61 += gw; ++_prescan_n_id_61;
    } else if (esub == 62) { _prescan_sumGW_id_62 += gw; ++_prescan_n_id_62;
    } else if (esub == 71) { _prescan_sumGW_id_71 += gw; ++_prescan_n_id_71;
    } else if (esub == 72) { _prescan_sumGW_id_72 += gw; ++_prescan_n_id_72;
    } else {
        switch (gtid % 100) {
            case  0: _prescan_sumGW_id_0     += gw; ++_prescan_n_id_0;     break;
            case 41: _prescan_sumGW_id_41    += gw; ++_prescan_n_id_41;    break;
            case 42: _prescan_sumGW_id_42    += gw; ++_prescan_n_id_42;    break;
            case 43: _prescan_sumGW_id_43    += gw; ++_prescan_n_id_43;    break;
            case 44: _prescan_sumGW_id_44    += gw; ++_prescan_n_id_44;    break;
            case 45: _prescan_sumGW_id_45    += gw; ++_prescan_n_id_45;    break;
            case 51: _prescan_sumGW_id_51    += gw; ++_prescan_n_id_51;    break;
            case 52: _prescan_sumGW_id_52    += gw; ++_prescan_n_id_52;    break;
            case 53: _prescan_sumGW_id_53    += gw; ++_prescan_n_id_53;    break;
            case 54: _prescan_sumGW_id_54    += gw; ++_prescan_n_id_54;    break;
            case 55: _prescan_sumGW_id_55    += gw; ++_prescan_n_id_55;    break;
            default: _prescan_sumGW_id_other += gw; ++_prescan_n_id_other; break;
        }
    }

    // --- 5-bucket ttCat cross-check --- [STEP17]
    // full-Nano 전환: officialTtCategory() 가 genTtbarId 디코드를 쓰므로 아래
    // ttCat-bucket 합은 위 genTtbarId%100 bin 합과 구성상 일치한다(자기일관성
    // 점검). prescan tree schema(sumGenW_ttCat_*)는 downstream(consolidate_prescan/
    // compute_stitch_factors) 호환을 위해 유지.
    const TtCat cat = officialTtCategory();
    switch (cat) {
        case TtCat::kLightFlavour:
            _prescan_sumGW_ttCat_LF   += gw; ++_prescan_n_ttCat_LF;   break;
        case TtCat::kAddCjet:
            _prescan_sumGW_ttCat_Cj   += gw; ++_prescan_n_ttCat_Cj;   break;
        case TtCat::kAdd1Bjet1Had:
            _prescan_sumGW_ttCat_1B1H += gw; ++_prescan_n_ttCat_1B1H; break;
        case TtCat::kAdd1Bjet2Had:
            _prescan_sumGW_ttCat_1B2H += gw; ++_prescan_n_ttCat_1B2H; break;
        case TtCat::kAdd2Bjet:
            _prescan_sumGW_ttCat_2B   += gw; ++_prescan_n_ttCat_2B;   break;
        case TtCat::kNoTTJets:
            _prescan_sumGW_ttCat_NoTT += gw; ++_prescan_n_ttCat_NoTT; break;
        default: break;   // kUnknown → leave uncounted (will show up as diff)
    }
}

// ----------------------------------------------------------------------------

void ttHHanalyzer_unified::writePrescanTree() {
    _of->file->cd();

    TTree* nct = new TTree("prescan",
        "Per-sample gen-level normalization check (1 row per file; "
        "hadd-friendly — post-process to compute r_B, r_4b).");

    // String branches need local std::string variables addressable from TBranch
    std::string sName  = _sampleName;
    std::string rYear  = _runYear;
    std::string dEra   = _era;
    bool        isData = (_DataOrMC == "Data");
    double      xsec_used = _baseWeight;   // analyzer's effective per-event base

    nct->Branch("sampleName",      &sName);
    nct->Branch("runYear",         &rYear);
    nct->Branch("dataEra",         &dEra);
    nct->Branch("isData",          &isData,            "isData/O");
    nct->Branch("xsec_used",       &xsec_used,         "xsec_used/D");
    nct->Branch("nFiles",          &_prescan_nFilesProcessed, "nFiles/I");

    // Events tree direct sums (skim-affected)
    nct->Branch("nEvents_total",   &_prescan_nEvents_total, "nEvents_total/L");
    nct->Branch("sumGenW_total",   &_prescan_sumGW_total,   "sumGenW_total/D");
    nct->Branch("sumGenW_pos",     &_prescan_sumGW_pos,     "sumGenW_pos/D");
    nct->Branch("sumGenW_neg",     &_prescan_sumGW_neg,     "sumGenW_neg/D");

    // NanoAOD Runs tree (skim-independent)
    nct->Branch("genEventSumw_runs",  &_prescan_runs_sumW,  "genEventSumw_runs/D");
    nct->Branch("genEventSumw2_runs", &_prescan_runs_sumW2, "genEventSumw2_runs/D");
    nct->Branch("genEventCount_runs", &_prescan_runs_count, "genEventCount_runs/L");

    // Cross-check: skim attrition (Events - Runs). Usually ≤ 0 (skim removed events).
    double diff = _prescan_sumGW_total - _prescan_runs_sumW;
    nct->Branch("diff_eventsVsRuns",  &diff,             "diff_eventsVsRuns/D");

    // 11-bin raw genTtbarId %% 100
    nct->Branch("sumGenW_id_lt0",   &_prescan_sumGW_id_lt0,   "sumGenW_id_lt0/D");
    nct->Branch("sumGenW_id_0",     &_prescan_sumGW_id_0,     "sumGenW_id_0/D");
    nct->Branch("sumGenW_id_41",    &_prescan_sumGW_id_41,    "sumGenW_id_41/D");
    nct->Branch("sumGenW_id_42",    &_prescan_sumGW_id_42,    "sumGenW_id_42/D");
    nct->Branch("sumGenW_id_43",    &_prescan_sumGW_id_43,    "sumGenW_id_43/D");
    nct->Branch("sumGenW_id_44",    &_prescan_sumGW_id_44,    "sumGenW_id_44/D");
    nct->Branch("sumGenW_id_45",    &_prescan_sumGW_id_45,    "sumGenW_id_45/D");
    nct->Branch("sumGenW_id_51",    &_prescan_sumGW_id_51,    "sumGenW_id_51/D");
    nct->Branch("sumGenW_id_52",    &_prescan_sumGW_id_52,    "sumGenW_id_52/D");
    nct->Branch("sumGenW_id_53",    &_prescan_sumGW_id_53,    "sumGenW_id_53/D");
    nct->Branch("sumGenW_id_54",    &_prescan_sumGW_id_54,    "sumGenW_id_54/D");
    nct->Branch("sumGenW_id_55",    &_prescan_sumGW_id_55,    "sumGenW_id_55/D");
    // [tt+nb] expanded partition (tt+nb removed from 53/54/55 above)
    nct->Branch("sumGenW_id_61",    &_prescan_sumGW_id_61,    "sumGenW_id_61/D");
    nct->Branch("sumGenW_id_62",    &_prescan_sumGW_id_62,    "sumGenW_id_62/D");
    nct->Branch("sumGenW_id_71",    &_prescan_sumGW_id_71,    "sumGenW_id_71/D");
    nct->Branch("sumGenW_id_72",    &_prescan_sumGW_id_72,    "sumGenW_id_72/D");
    nct->Branch("sumGenW_id_other", &_prescan_sumGW_id_other, "sumGenW_id_other/D");

    // 11-bin raw genTtbarId %% 100 — unweighted event counts
    // (companion to sumGenW_id_*; per-category entry counts for stitching bookkeeping)
    nct->Branch("n_id_lt0",   &_prescan_n_id_lt0,   "n_id_lt0/L");
    nct->Branch("n_id_0",     &_prescan_n_id_0,     "n_id_0/L");
    nct->Branch("n_id_41",    &_prescan_n_id_41,    "n_id_41/L");
    nct->Branch("n_id_42",    &_prescan_n_id_42,    "n_id_42/L");
    nct->Branch("n_id_43",    &_prescan_n_id_43,    "n_id_43/L");
    nct->Branch("n_id_44",    &_prescan_n_id_44,    "n_id_44/L");
    nct->Branch("n_id_45",    &_prescan_n_id_45,    "n_id_45/L");
    nct->Branch("n_id_51",    &_prescan_n_id_51,    "n_id_51/L");
    nct->Branch("n_id_52",    &_prescan_n_id_52,    "n_id_52/L");
    nct->Branch("n_id_53",    &_prescan_n_id_53,    "n_id_53/L");
    nct->Branch("n_id_54",    &_prescan_n_id_54,    "n_id_54/L");
    nct->Branch("n_id_55",    &_prescan_n_id_55,    "n_id_55/L");
    // [tt+nb] expanded partition counts
    nct->Branch("n_id_61",    &_prescan_n_id_61,    "n_id_61/L");
    nct->Branch("n_id_62",    &_prescan_n_id_62,    "n_id_62/L");
    nct->Branch("n_id_71",    &_prescan_n_id_71,    "n_id_71/L");
    nct->Branch("n_id_72",    &_prescan_n_id_72,    "n_id_72/L");
    nct->Branch("n_id_other", &_prescan_n_id_other, "n_id_other/L");

    // 5-bucket ttCat (ntuple primary) — Σ genWeight
    nct->Branch("sumGenW_ttCat_LightFlavour",  &_prescan_sumGW_ttCat_LF,   "sumGenW_ttCat_LightFlavour/D");
    nct->Branch("sumGenW_ttCat_AddCjet",       &_prescan_sumGW_ttCat_Cj,   "sumGenW_ttCat_AddCjet/D");
    nct->Branch("sumGenW_ttCat_Add1Bjet_1Had", &_prescan_sumGW_ttCat_1B1H, "sumGenW_ttCat_Add1Bjet_1Had/D");
    nct->Branch("sumGenW_ttCat_Add1Bjet_2Had", &_prescan_sumGW_ttCat_1B2H, "sumGenW_ttCat_Add1Bjet_2Had/D");
    nct->Branch("sumGenW_ttCat_Add2Bjet",      &_prescan_sumGW_ttCat_2B,   "sumGenW_ttCat_Add2Bjet/D");
    nct->Branch("sumGenW_ttCat_NoTT",          &_prescan_sumGW_ttCat_NoTT, "sumGenW_ttCat_NoTT/D");

    // 5-bucket ttCat — unweighted counts
    nct->Branch("n_ttCat_LightFlavour",  &_prescan_n_ttCat_LF,   "n_ttCat_LightFlavour/L");
    nct->Branch("n_ttCat_AddCjet",       &_prescan_n_ttCat_Cj,   "n_ttCat_AddCjet/L");
    nct->Branch("n_ttCat_Add1Bjet_1Had", &_prescan_n_ttCat_1B1H, "n_ttCat_Add1Bjet_1Had/L");
    nct->Branch("n_ttCat_Add1Bjet_2Had", &_prescan_n_ttCat_1B2H, "n_ttCat_Add1Bjet_2Had/L");
    nct->Branch("n_ttCat_Add2Bjet",      &_prescan_n_ttCat_2B,   "n_ttCat_Add2Bjet/L");
    nct->Branch("n_ttCat_NoTT",          &_prescan_n_ttCat_NoTT, "n_ttCat_NoTT/L");

    nct->Fill();
    nct->Write();

    // --- Console summary ---
    std::cout << "\n══════════════════════════════════════════════════════════════════\n";
    std::cout << "[Prescan] Summary — " << _sampleName << "\n";
    std::cout << "------------------------------------------------------------------\n";
    std::cout << "  nEvents (Events tree)  : " << _prescan_nEvents_total << "\n";
    std::cout << "  Σgenw  (Events tree)   : " << _prescan_sumGW_total
              << "   (pos=" << _prescan_sumGW_pos
              << ", neg=" << _prescan_sumGW_neg << ")\n";
    std::cout << "  Σgenw  (Runs tree)     : " << _prescan_runs_sumW << "\n";
    std::cout << "  Σgenw² (Runs tree)     : " << _prescan_runs_sumW2 << "\n";
    std::cout << "  count  (Runs tree)     : " << _prescan_runs_count << "\n";
    std::cout << "  diff (Events - Runs)   : " << diff
              << "   ← non-zero = skim attrition (expected if ntuplizer skimmed)\n";

    if (_DataOrMC != "Data") {
        const double sum_cc = _prescan_sumGW_id_41 + _prescan_sumGW_id_42 + _prescan_sumGW_id_43
                            + _prescan_sumGW_id_44 + _prescan_sumGW_id_45;
        std::cout << "  --- by genTtbarId %% 100 (raw, weighted) ---\n";
        std::cout << "    id < 0    (no/none)  : " << _prescan_sumGW_id_lt0   << "\n";
        std::cout << "    id =  0   (LF)       : " << _prescan_sumGW_id_0     << "\n";
        std::cout << "    id 41-45  (cc total) : " << sum_cc << "\n";
        std::cout << "    id = 51   (1b/1H)    : " << _prescan_sumGW_id_51    << "\n";
        std::cout << "    id = 52   (1b/2H)    : " << _prescan_sumGW_id_52    << "\n";
        std::cout << "    id = 53   (2b)       : " << _prescan_sumGW_id_53    << "\n";
        std::cout << "    id = 54   (bb)       : " << _prescan_sumGW_id_54    << "\n";
        std::cout << "    id = 55   (4b)       : " << _prescan_sumGW_id_55    << "\n";
        std::cout << "    id  other            : " << _prescan_sumGW_id_other << "\n";
        const Long64_t sum_n_cc = _prescan_n_id_41 + _prescan_n_id_42 + _prescan_n_id_43
                                + _prescan_n_id_44 + _prescan_n_id_45;
        std::cout << "  --- by genTtbarId %% 100 (raw, unweighted event counts) ---\n";
        std::cout << "    n id < 0  (no/none)  : " << _prescan_n_id_lt0   << "\n";
        std::cout << "    n id =  0 (LF)       : " << _prescan_n_id_0     << "\n";
        std::cout << "    n id 41-45 (cc total): " << sum_n_cc << "\n";
        std::cout << "    n id = 51 (1b/1H)    : " << _prescan_n_id_51    << "\n";
        std::cout << "    n id = 52 (1b/2H)    : " << _prescan_n_id_52    << "\n";
        std::cout << "    n id = 53 (2b)       : " << _prescan_n_id_53    << "\n";
        std::cout << "    n id = 54 (bb)       : " << _prescan_n_id_54    << "\n";
        std::cout << "    n id = 55 (4b)       : " << _prescan_n_id_55    << "\n";
        std::cout << "    n id  other          : " << _prescan_n_id_other << "\n";
        std::cout << "  --- ntuple ttCat cross-check (Σ genw should match id sums) ---\n";
        std::cout << "    LF       : " << _prescan_sumGW_ttCat_LF
                  << "   (n=" << _prescan_n_ttCat_LF << ")\n";
        std::cout << "    AddCjet  : " << _prescan_sumGW_ttCat_Cj
                  << "   (n=" << _prescan_n_ttCat_Cj << ")\n";
        std::cout << "    1B/1H    : " << _prescan_sumGW_ttCat_1B1H
                  << "   (n=" << _prescan_n_ttCat_1B1H << ")\n";
        std::cout << "    1B/2H    : " << _prescan_sumGW_ttCat_1B2H
                  << "   (n=" << _prescan_n_ttCat_1B2H << ")\n";
        std::cout << "    Add2Bjet : " << _prescan_sumGW_ttCat_2B
                  << "   (n=" << _prescan_n_ttCat_2B << ")\n";
        std::cout << "    NoTT     : " << _prescan_sumGW_ttCat_NoTT
                  << "   (n=" << _prescan_n_ttCat_NoTT << ")\n";

        // Inline cross-check warnings (loose tolerance — float rounding)
        const double tol = 1e-3 * std::max(1.0, std::abs(_prescan_sumGW_total));
        if (std::abs(_prescan_sumGW_id_0 - _prescan_sumGW_ttCat_LF) > tol) {
            std::cerr << "[Prescan][WARN] id=0 (" << _prescan_sumGW_id_0
                      << ") != ttCat_LF (" << _prescan_sumGW_ttCat_LF << ")\n";
        }
        if (std::abs(sum_cc - _prescan_sumGW_ttCat_Cj) > tol) {
            std::cerr << "[Prescan][WARN] id∈41-45 (" << sum_cc
                      << ") != ttCat_AddCjet (" << _prescan_sumGW_ttCat_Cj << ")\n";
        }
        if (std::abs(_prescan_sumGW_id_51 - _prescan_sumGW_ttCat_1B1H) > tol) {
            std::cerr << "[Prescan][WARN] id=51 vs ttCat_1B1H drift\n";
        }
        if (std::abs(_prescan_sumGW_id_52 - _prescan_sumGW_ttCat_1B2H) > tol) {
            std::cerr << "[Prescan][WARN] id=52 vs ttCat_1B2H drift\n";
        }
        // [tt+nb] 분해 카운터는 61/62/71/72(tt+nb)를 53/54/55에서 빼내 별도 빈으로
        // 보낸다. ntuple의 ttCat_Add2Bjet 은 ≥2 add-b 전체(분리 안 함)이므로,
        // cross-check 비교에는 tt+nb 빈을 다시 더해야 한다 (안 그러면 거짓 경고).
        const double sum_2b_or_more = _prescan_sumGW_id_53 + _prescan_sumGW_id_54
                                    + _prescan_sumGW_id_55
                                    + _prescan_sumGW_id_61 + _prescan_sumGW_id_62
                                    + _prescan_sumGW_id_71 + _prescan_sumGW_id_72;
        if (std::abs(sum_2b_or_more - _prescan_sumGW_ttCat_2B) > tol) {
            std::cerr << "[Prescan][WARN] id∈53-55+tt+nb (" << sum_2b_or_more
                      << ") != ttCat_Add2Bjet (" << _prescan_sumGW_ttCat_2B << ")\n";
        }
    }
    std::cout << "══════════════════════════════════════════════════════════════════\n";
}


// ─────────────────────────────────────────────────────────────────────────────
// [2026-10-06] exit() outside the analyzer's fatal paths -- treestream and include/eventBuffer.h
//   ("** eventBuffer::read - cannot load entry N; stopping", exit 1), tnm.cc (bad arguments, a filelist
//   that cannot be opened) -- ends like tthh::fatalExit (include/ExitCodes.h: why), as far as it can:
//   glibc runs the exit-time handlers last-registered-first, so this handler runs before the ones
//   registered before it -- ROOT's end-of-process cleanup and the static destructors of the libraries
//   loaded at start, and (after `(void)gROOT` in main) the TROOT-delete handler of InitInterpreter --
//   but after the ones registered later (function-local statics, dictionaries loaded on the way) and
//   after the thread_local destructors. main() registers it first thing and again just before the
//   event loop. A non-zero status ends the process there (flush, std::_Exit); status 0, the normal end,
//   keeps the full teardown. on_exit is glibc's (Linux); elsewhere nothing is registered.
// ─────────────────────────────────────────────────────────────────────────────
#if defined(__GLIBC__)
static void endNonZeroExitWithoutTeardown(int status, void*) {
    if (status != 0) tthh::fatalExit(status);
}
#endif

// =============================================================================
// main() — entry point for the ttHH(4b) FH analyzer
// -----------------------------------------------------------------------------
// All argument parsing is done by tnm.cc::commandLine::decode.
// [STEP2] kValidationStudy 제거 — tnm.cc의 --val-* 파서 필드는 미사용 잔재로
// 남아 있으며(무해), Step 8 정리 단계에서 제거 예정.
// =============================================================================
int main(int argc, char** argv){
#if defined(__GLIBC__)
    (void)gROOT;   // keep BEFORE on_exit: sets up ROOT (InitInterpreter registers its TROOT-delete handler)
    on_exit(endNonZeroExitWithoutTeardown, nullptr);     // so this runs before ROOT's exit-time handlers
#endif

    commandLine cl(argc, argv);
    vector<string> filenames = fileNames(cl.filelist);
    double weight = cl.externalweight;   // global per-sample weight

    // ─────────────────────────────────────────────────────────────────────
    // 1. Parse and validate analysis mode
    // ─────────────────────────────────────────────────────────────────────
    AnalysisMode mode;
    try {
        mode = parseAnalysisMode(cl.analysisMode);
    }
    catch (const std::invalid_argument& e) {
        std::cerr << e.what() << std::endl;
        return tthh::CONFIG_BAD_MODE;
    }

    // ─────────────────────────────────────────────────────────────────────
    // 2. Echo arguments for log
    // ─────────────────────────────────────────────────────────────────────
    std::cout << "\n--------- Check arguments ---------------------------------------\n" << std::endl;
    std::cout << "  - [ output file name ] --> " << cl.outputfilename << std::endl;
    std::cout << "  - [ runYear -string- ] --> " << cl.runYear << std::endl;
    std::cout << "  - [ DataOrMC -string- ] --> " << cl.DataOrMC << std::endl;
    std::cout << "  - [ sampleName ] --> " << cl.sampleName << std::endl;
    std::cout << "  - [ eraName ] --> " << cl.eraName << std::endl;
    std::cout << "  - [ analysisMode ] --> " << analysisModeName(mode) << std::endl;
    std::cout << "\n--------- Check arguments ---------------------------------------\n" << std::endl;

    // ─────────────────────────────────────────────────────────────────────
    // 3. Open input ntuple stream
    // ─────────────────────────────────────────────────────────────────────
    itreestream stream(filenames, "Events");
    if ( !stream.good() ) {
        std::cerr << "\n[FATAL][E" << tthh::INPUT_OPEN_FAIL
                  << "] can't read root input files (Events tree).\n" << std::endl;
        tthh::fatalExit(tthh::INPUT_OPEN_FAIL);
    }
    eventBuffer ev(stream);
#ifdef TTHH_EVENTBUFFER_STAMP
    std::cout << "[eventBuffer] " << TTHH_EVENTBUFFER_STAMP << std::endl;   // [STEP 24] which header
#else
    std::cout << "[eventBuffer] header without a STEP 24 stamp" << std::endl;
#endif
    std::cout << " Output filename: " << cl.outputfilename << std::endl;

    // ─────────────────────────────────────────────────────────────────────
    // 4. Construct the analyzer instance
    //    constructor signature:
    //      (outFile, eventBuffer, weight, sysToggle, year, dataOrMC,
    //       sampleName, era, debug, mode)
    // ─────────────────────────────────────────────────────────────────────
    // [STEP 27 P] --tree-v1 on|off (default on): the constructor books the tree, so the switch is set before it
    if (!cl.treeV1.empty() && cl.treeV1 != "on" && cl.treeV1 != "off") {
        std::cerr << "\n[FATAL][E" << tthh::CONFIG_BAD_RUNINFO << "] --tree-v1 '" << cl.treeV1 << "': on or off.\n"
                  << std::endl;
        tthh::fatalExit(tthh::CONFIG_BAD_RUNINFO);
    }
    ttHHanalyzer_unified::treeV1Flag() = (cl.treeV1 != "off");
    if (cl.treeV1 == "off")
        std::cout << "[treeV1] --tree-v1 off: the Tree v1 branches are not booked (Tree/Tree as in STEP 26, plus the "
                     "fixed-WP comparison weights)" << std::endl;

    const bool debugVerbose = (mode == AnalysisMode::kDebug);  // [STEP2] 거시 단계 로그
    ttHHanalyzer_unified analysis(
        cl.outputfilename,
        &ev,
        weight,
        true,               // legacy systematics toggle
        cl.runYear,
        cl.DataOrMC,
        cl.sampleName,
        cl.eraName,
        debugVerbose,
        mode
    );

    // ─────────────────────────────────────────────────────────────────────
    // [tt+nb] Load the per-sample Expanded_genTtbarId lookup from the project's
    // DerivedCorr/expandedTtbarId/ directory: it picks ttnb_<sampleName>.root
    // automatically. Override the directory with EXPANDED_TTBARID_DIR (e.g. an
    // absolute path on a worker). Samples with no matching file -> INACTIVE
    // (NanoAOD genTtbarId used; correct for Data and non-ttbar samples).
    // Active in BOTH kPrescan and the main loop (it is set before
    // performAnalysis), so the prescan partition sees the same lookup.
    // ─────────────────────────────────────────────────────────────────────
    {
        // [STEP18] config 명시 경로만 (default 폐기). null -> "" -> lookup INACTIVE.
        analysis.setExpandedTtbarIdDir(
            cfgpath::resolve("EXPANDED_TTBARID_DIR",
                             "Expanded_genTtbarId lookup dir", /*required=*/false));
    }

    // [lepton-CR] CLI --region 주입 (QCD 억제 1ℓ+MET CR; main/debug 에서만 효과)
    analysis.setRegion(cl.region);

    // [SF toggle] CLI --trigsf/--btagsf/--btagrw 주입 (production evtWeight 구성)
    analysis.setSFflags(cl.sfTrig, cl.sfBtag, cl.sfBtagRw);

    // [STEP 27 O] CLI --tree-pdf on|off (default off): LHEPdfWeight in Tree/Tree
    analysis.setTreePdf(cl.treePdf);

    // ─────────────────────────────────────────────────────────────────────
    // [stitch] stitching multiplier JSON은 stitched 조성을 봐야 하는 모드에서만
    // 로드한다: main(최종 yield), btagtrig(b-tag norm reweight 문맥), debug(main
    // 미러). prescan은 의도적으로 stitch-free:
    //   - prescan은 이 JSON의 입력(ΣgenW 분해)을 "생산"하는 모드이므로.
    // 경로는 config(STITCH_FACTORS_JSON)에서 명시. null -> 미로드(stitch-free), 손상 -> fatal(60-62).
    if (mode == AnalysisMode::kMainAnalysis ||
        mode == AnalysisMode::kBTagAndTriggerStudy ||
        mode == AnalysisMode::kDebug) {
        const std::string sj = cfgpath::resolve("STITCH_FACTORS_JSON",
                                   "ttbar stitch-factors JSON", /*required=*/false);
        if (!sj.empty()) analysis.setStitchFactorsFile(sj);
    }

    // ─────────────────────────────────────────────────────────────────────
    // 5. Run the analysis
    // ─────────────────────────────────────────────────────────────────────
    if(debugVerbose) std::cout<<"debug : Before [ performAnalysis ] in main() function"<<std::endl;
#if defined(__GLIBC__)
    on_exit(endNonZeroExitWithoutTeardown, nullptr);     // again: also before the handlers registered while
#endif                                                   // setting up (an exit() in the event loop, e.g. eventBuffer::read)
    analysis.performAnalysis();
    if(debugVerbose) std::cout<<"debug : After [ performAnalysis() ] and Before [ ev.close() ].. in main() function"<<std::endl;
    ev.close();
    if(debugVerbose) std::cout<<"debug : After [ ev.close() ] in main() function"<<std::endl;

    return 0;
}
