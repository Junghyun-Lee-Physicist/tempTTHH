#pragma once
// =============================================================================
// SelectionCuts — 분석 selection 상수의 단일 소스 (single source of truth)
// -----------------------------------------------------------------------------
// [STEP3] 기존 `std::map<std::string,float> cut` 을 대체한다.
//
// 교체 이유:
//   1. map의 operator[]는 존재하지 않는 키를 조용히 0.0으로 삽입한다
//      (오타 → 컴파일 OK, 런타임 에러 없음, cut 값만 0이 되는 무음 버그).
//      constexpr 상수는 오타가 즉시 컴파일 에러.
//   2. int cut(nJets 등)이 float로 강제되는 타입 손실 제거.
//   3. 문자열 lookup 비용 0 (hot loop).
//   4. 상수마다 AN 출처를 주석으로 강제 — selection 정의가 이 파일 하나로
//      한눈에 보인다.
//
// ⚠ 값 변경 시 반드시 AN 근거를 주석에 갱신할 것 (프로젝트 규약).
// ⚠ era 의존 값이 생기면 (Run3 확장 등) 이 namespace를 era-keyed 구조로 승격한다.
//   현재 여기 있는 값들은 2017/2018 공통이라고 **가정**하고 쓰는 중이다.
//   연도별로 갈라야 하는 값은 `include/EraConfig.h` 로 보낸다 (b-tag WP 가 그 예).
//   미해결 (P1): `sixthJetPt=40` / `HT=500` 은 2017 HLT(`SixPFJet40`/`PFHT380`) 의
//   plateau 기준이다. 2018 은 `SixPFJet36`/`PFHT400`/`PFHT330PT30` 이므로 plateau
//   재확인이 필요하다. `leadMuonPt=29` 도 `IsoMu27` 기준 → 2018 `IsoMu24` 에 맞춰
//   26 으로 내려야 한다. 둘 다 아직 안 했다.
// =============================================================================

namespace Cuts {

// ─────────────────────────────────────────────────────────────────────────────
// 이벤트 selection — ttH AN-19-094 §5 Table 55, FH channel (2017)
// (현재 baseline은 AN Table 55와 동일; SR 분류(7/8/≥9 jets × ≥4 b)는
//  cutflow 단계 기록(kNumbJets3/4)과 이후 카테고리화로 수행)
// ─────────────────────────────────────────────────────────────────────────────
constexpr int   nJets      = 6;      // 최소 jet 수               [AN Tab.55] ([round2] was 8)
constexpr float sixthJetPt = 40.f;   // 6th jet pT 하한 — HLT 성능 [AN Tab.55, §5 l.703-704]
constexpr float HT         = 500.f;  // H_T 하한                  [AN Tab.55]
constexpr int   nbJets     = 2;      // 최소 b-tag 수 (baseline)   [AN Tab.55] ([round2] was 4)
constexpr int   nLeptons   = 0;      // lepton veto (FH)          [AN Tab.55]
constexpr float hadWMassLo = 30.f;   // m_qq 하한                  [AN Tab.55, §5 l.704-706]
constexpr float hadWMassHi = 250.f;  // m_qq 상한                  [AN Tab.55]

// ─────────────────────────────────────────────────────────────────────────────
// Jet 정의 — AN §4.5 (2017 UL)
// b-tag WP는 objectJet::valbTagMedium (DeepJet M)을 사용 — 여기 중복 정의하지 않는다.
// ⚠ WP 는 **연도 의존**이므로 값을 여기 적지 않는다 (2017 = 0.3040, 2018 = 0.2783,
//   AN Table 32). 유일한 출처는 `include/EraConfig.h` 의 `EraConfig::btagWP()` 이고,
//   objectJet::configureBtagWP() 가 분석 시작 시 주입한다.
// ─────────────────────────────────────────────────────────────────────────────
constexpr float jetPt   = 30.f;      // jet pT 하한 (JES/JER 적용 후) [AN Tab.55]
constexpr float jetEta  = 2.4f;      // |η| 상한                      [AN Tab.55]
constexpr int   jetID   = 6;         // tight && tightLepVeto         [AN §4.5 / JME]
constexpr int   jetPUid = 4;         // PU id loose 이상 (pT<50에 적용) [AN §4.5 / JME]

// ─────────────────────────────────────────────────────────────────────────────
// Veto lepton 정의 — DL channel의 subleading lepton 기준을 차용 (AN §5 l.701-702:
// "no leptons as identified by the DL lepton-selection criteria for the
//  subleading lepton"), AN §4.3 (electrons) / §4.4 (muons)
// lead*Pt 는 lepton "수집" 시의 leading 후보 임계 (btagtrig 모드의 muon
// control에서 사용), subLead*Pt 가 veto 임계 (pT>15 lepton 존재 시 reject).
// ─────────────────────────────────────────────────────────────────────────────
constexpr float leadElePt     = 30.f;   // [AN Tab.55 SL: e 29/30/30 — 2017=30]
constexpr float leadMuonPt    = 29.f;   // [AN Tab.55 SL: μ 26/29/26 — 2017=29, IsoMu27 plateau]
constexpr float subLeadElePt  = 15.f;   // veto 임계 [AN Tab.55 "additional leptons max 15"]
constexpr float subLeadMuonPt = 15.f;   // veto 임계 [AN Tab.55]
constexpr float eleEta        = 2.5f;   // [AN §4.3; Tab.55의 lepton 2.4보다 보수적 — ECAL 수용]
constexpr float muonEta       = 2.4f;   // [AN §4.4 / Tab.55]
constexpr float muonIso       = 0.15f;  // PF rel. iso (tight WP) [AN §4.4 / MUO POG]

// ─────────────────────────────────────────────────────────────────────────────
// 이벤트 cleaning — AN §4.2
// (기존 cut["trigger"/"filter"/"pv"] = 1.0 enable 플래그는 제거 — 항상 적용이며,
//  모드별 on/off는 cut 테이블의 enforceIn 비트마스크가 담당)
// ─────────────────────────────────────────────────────────────────────────────

// [STEP3에서 제거된 키] — 전부 미사용(주석 코드 전용)이었음:
//   boostedJetPt(10), boostedJetEta(2.4), bTagDisc(0.80), hadHiggsPt(20),
//   nlJets(0), eleIso(주석), nVetoLeptons/vetoLepPt(이미 주석)
//   원형은 docs/backup_20260611/ttHHanalyzer_unified.h 의 cut map 참조.

} // namespace Cuts
