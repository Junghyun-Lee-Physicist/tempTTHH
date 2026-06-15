#!/usr/bin/env python3
"""
compute_stitch_factors_fixed_config.py
──────────────────────────────────────

`ttHHanalyzer_unified --mode prescan` 출력 파일들을 모아서

ttbar stitching scaling factor (r_B, r_4b) 와 anchor normalization 을 계산한다.

이 버전은 CLI 인자를 쓰지 않는다. analyzer output 위치, partition option,
luminosity, cross section, 출력 JSON 위치 등은 전부 코드 최상단의 USER CONFIG
블록에서 직접 수정한다.

현재 analyzer 는 `--mode prescan` 으로 sample 당 1행짜리 `prescan` TTree 를
파일마다 쓴다 (writePrescanTree). 이 스크립트는 그 행들을 sample 별로 합쳐
stitching factor (r_B, r_4b) 와 per-event normalization 을 계산한다.

★ EXPANDED:
    analyzer 패치 §5 이후 prescan tree 에는 expanded id bin
    (sumGenW_id_61/62/71/72 = tt+bbb/tt+4b) 이 들어 있다. tt+nb 이벤트는
    53/54/55 에서 빠져 61-72 로 옮겨졌으므로, 이제 tt4b partition 을
    "진짜 >=3 b-jet" (61-72) 으로 잡을 수 있다 — 예전처럼 54/55 를 tt+4b 로
    근사하던 hack 이 더 이상 필요 없다 (option EXP).

AN reference
────────────
    ttHH AN-2022/122 §3.3      SL channel Option1 (gen-level partition)
    ttHH AN-2022/122 §3.4      DL gen-level 4b exclusion
    ttHH AN-2022/122 §3.2      tt+bbb / tt+4b sample-level construction
                               (line 324-329: Option1 uses tt4b sample for
                                both tt+bbb and tt+4b classes — combined
                                into tt+nb at DNN stage)
    ttH  AN-19-094  §6.2.1     NNLO+NNLL anchoring procedure
    ttH  AN-19-094  Appendix C tt+jets background uncertainty treatment

Anchoring 은 dedicated sample 의 decay-channel coverage 에 의해 결정된다
(decay channel 은 orthogonal phase space 이므로 매칭되는 inclusive 채널에
σ_total × BR_channel 로 anchor 하면 self-consistent 하다):

  * ttbb : per-decay-channel 4FS sample 이 SE 채널 모두 존재한다
           (ttbb_Hadronic / ttbb_SemiLeptonic / ttbb_2L2Nu). 각 ttbb_c 는
           자기 채널의 tt+2b 만 owns 하고, 매칭 inclusive TTTo_c 에
           σ_total × BR_c 로 anchor, TTTo_c 에서만 tt+2b 를 reject 한다.
           → tt+2b 는 세 inclusive 전부에서 reject 되고 각각 dedicated 로 채워진다.
  * tt4b : decay-inclusive (census Had/SL/DL ≈ 46/44/11). 세 채널 전부의
           tt+nb 를 owns 하고, Had+SL+DL inclusive merge 에 σ_total (no BR) 로
           anchor, 세 inclusive 전부에서 tt+nb 를 reject 한다.

  [HISTORY] 이전 버전은 ttbb_Hadronic 하나만 써서 hadronic 채널의 tt+2b 만
  교체하고 SL/DL tt+2b 는 5FS inclusive 로 남겼다 (보수적 잔재; option
  EXP_TTBB_HAD_ONLY 로 보존). per-channel ttbb sample 이 확인되어 EXP 가
  3-채널 ttbb stitch 로 갱신되었다 (2026-06).

사용법
──────
    # 아래 USER CONFIG만 확인한 뒤 실행
    python3 compute_stitch_factors_fixed_config.py
"""

from __future__ import annotations

import json
import logging
import sys
from dataclasses import asdict, dataclass, field
from pathlib import Path
from typing import Iterable

import numpy as np
import uproot

# ──────────────────────────────────────────────────────────────────────────────
# USER CONFIG — 여기만 바꾸면 됨
# ──────────────────────────────────────────────────────────────────────────────

ANALYZER_OUTPUT_DIR = Path(
    "/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/AnalyzerOutput_prescan"
)

# Prescan TTree name written by the analyzer (writePrescanTree(): "prescan").
# (The old standalone normcheck mode used "normCheck"; the unified analyzer
#  writes "prescan". Change here only if you rename the tree.)
PRESCAN_TREE = "prescan"

# 출력 JSON 위치. 상대 경로면 현재 실행 디렉토리에 생성된다.
# 권장: analyzer 가 읽는 DerivedCorr 아래에 둔다 (CorrectionsManager 와 같은 위치 관습).
OUTPUT_JSON_PATH = Path("DerivedCorr/stitchFactors/stitch_factors_2017.json")

# Partition option (current production uses the EXP family with expanded id §5):
#   EXP               = per-channel ttbb (Had/SL/DL each anchored to its own
#                       inclusive) + decay-inclusive tt4b. ← DEFAULT, physically
#                       correct when all three 4FS ttbb samples exist.
#   EXP_TTBB_HAD_ONLY = ttbb_Hadronic only (SL/DL tt+2b left as 5FS inclusive)
#                       + decay-inclusive tt4b. Conservative fallback.
#   EXP_HAD_TT4B      = ttbb_Hadronic + hadronic-only tt4b (use if tt4b census
#                       is ~100% full-hadronic).
# Legacy gen-partition options (pre-expanded-id, kept for reference):
#   A = ttHH AN SL Option1 차용: inclusive={LF+cc}, ttbb={51,52,53}, tt4b={54,55}
#   B = ttH FH-style:           inclusive={LF+cc}, ttbb={51,52,53,54,55}, tt4b skip
#   C = ttHH Option2:           현재 gen-level partition 기준으로 B와 동일
PARTITION_OPTION = "EXP"

# 2017 UL — 41.48 fb⁻¹ = 41480 pb⁻¹
LUMI_PB_INV = 41480.0

# Cross sections, all converted to pb.
#
# σ_inc  : NNLO+NNLL inclusive ttbar (Top++ v2.0, m_top = 172.5 GeV) = 831.76 pb.
#          Each dedicated sample multiplies this by the BR of the decay phase
#          space it is anchored to:
#            ttbb_c (per channel) → σ_total × BR_c   (hadronic/SL/DL)
#            tt4b  (decay-incl.)  → σ_total          (no BR; all channels)
# σ_ttbb : per-channel 4FS ttbb LHE σ. Hadronic = 1452 fb = 1.452 pb.
# σ_tt4b : 296 fb (decay-inclusive, not BR-filtered per the provider) = 0.296 pb
#
# ── BR decomposition (W hadronic BR = 0.6741; BR_HAD = 0.6741² exactly) ──────
#   BR_HAD = W_had²         = 0.45441081   (fully hadronic)
#   BR_SL  = 2·W_had·W_lep  = 0.43937838   (semi-leptonic)
#   BR_DL  = W_lep²         = 0.10621081   (di-leptonic)         Σ = 1.00000000
#
# ── Why the dedicated σ values barely matter ────────────────────────────────
#   r_d = σ_inc(d) · f_d / σ_ded(d), and the analyzer multiplies r_d ON TOP of
#   the dedicated YAML base weight Lumi·σ_ded/Σgenw. So σ_ded CANCELS exactly:
#       YAML · r = (Lumi·σ_ded/Σgenw) · (σ_inc·f/σ_ded) = Lumi·σ_inc·f/Σgenw.
#   The ONLY requirement is that each σ_ded below equals the σ used in that
#   sample's YAML weight. The per-channel ttbb defaults are BR-scaled from the
#   hadronic value (σ_ttbb_total = 1.452/BR_HAD ≈ 3.195 pb, then × BR_c); replace
#   with the official XSDB σ if your YAML uses those — result is unchanged as
#   long as YAML and here agree.
# ──────────────────────────────────────────────────────────────────────────────
# [TrackC] σ·BR은 xsec_db(single source)에서 읽는다. compute_stitch_factors와
# submitter(base weight)가 같은 db를 참조해야 r·base가 약분되어 yield가 보존된다
# (과거: r은 σ_ded=(1.452/BR_HAD)×BR, main.yml은 1.452×BR — 불일치로 SL/DL이
#  ~2.2x 과소정규화. db 통일로 원천 해소).
#
# σ_inc(채널) = db[inclusive].cross_section_pb × BR_channel
# σ_ded       = db[dedicated].cross_section_pb × BR  (dedicated의 br 필드)
# ──────────────────────────────────────────────────────────────────────────────
import json as _json
import os as _os

XSEC_DB_PATH = _os.environ.get(
    "TTHH_XSEC_DB",
    _os.path.join(_os.path.dirname(_os.path.abspath(__file__)), "data", "samples_2017UL.json"))

def _load_xsec_db(path=XSEC_DB_PATH):
    with open(path) as f:
        return _json.load(f)

_XDB = _load_xsec_db()

def _sigma_eff(sample):
    """db의 cross_section_fb × br × kfactor (= σ_eff, fb). dedicated/inclusive 공통.
    stitch r은 σ_inc/σ_ded 비율이라 단위(fb/pb)에 무관하나, db 통일을 위해 fb 사용."""
    rec = _XDB[sample]
    xs = rec["cross_section_fb"]
    if xs is None:
        raise ValueError(f"{sample}: cross_section_fb is null (Data?)")
    return xs * (rec.get("br", 1.0) or 1.0) * (rec.get("kfactor", 1.0) or 1.0)

def _sigma_total(sample):
    """db의 cross_section_fb (BR 적용 전, total/inclusive, fb)."""
    return _XDB[sample]["cross_section_fb"]

# BR (db meta가 아닌 표준 W BR — STITCH_PLANS의 σ_inc 채널 분해에 사용)
W_HAD = 0.6741
W_LEP = 1.0 - W_HAD
BR_HAD = W_HAD * W_HAD                                         # 0.45441081
BR_SL  = 2.0 * W_HAD * W_LEP                                   # 0.43937838
BR_DL  = W_LEP * W_LEP                                         # 0.10621081

# σ_inc total (tt+jets) — db의 TTToHadronic cross_section_pb (BR 적용 전)
SIGMA_INC_NNLO_TOTAL_PB = _sigma_total("TTToHadronic")        # 831.76
SIGMA_INC_NNLO          = SIGMA_INC_NNLO_TOTAL_PB * BR_HAD

# σ_dedicated — db에서 σ_eff(=xsec×br)로 읽는다 (채널 BR 일관 적용)
SIGMA_TTBB_HAD = _sigma_eff("ttbb_Hadronic")                  # 1.452 × BR_HAD = 0.6598
SIGMA_TTBB_SL  = _sigma_eff("ttbb_SemiLeptonic")              # 1.452 × BR_SL  = 0.6380
SIGMA_TTBB_DL  = _sigma_eff("ttbb_2L2Nu")                     # 1.452 × BR_DL  = 0.1542
SIGMA_TTBB_LHE = SIGMA_TTBB_HAD                               # backward-compat alias
SIGMA_TT4B_LHE = _sigma_eff("tt4b")                           # 0.296 (br=1)

# True면 아래 EXPECTED_SAMPLE_DIRS에 적힌 디렉토리만 scan한다.
# False면 ANALYZER_OUTPUT_DIR 아래의 모든 하위 디렉토리를 scan한다.
ONLY_USE_LISTED_SAMPLE_DIRS = False

# sample 디렉토리 바로 아래의 ROOT 파일만 읽는다.
# 만약 sample/subdir/*.root 구조라면 "**/*.root" 로 바꾸면 된다.
ROOT_FILE_GLOB = "*.root"

VERBOSE = False

# 현재 AnalyzerOutput_normcheck 아래에 존재한다고 알려준 결과 디렉토리 목록.
EXPECTED_SAMPLE_DIRS = frozenset({
    "BTagCSV_B", "BTagCSV_C", "BTagCSV_D", "BTagCSV_E", "BTagCSV_F",
    "JetHT_B", "JetHT_C", "JetHT_D", "JetHT_E", "JetHT_F",
    "SingleMuon_B", "SingleMuon_C", "SingleMuon_D", "SingleMuon_E", "SingleMuon_F",
    "QCD_HT200to300", "QCD_HT300to500", "QCD_HT500to700",
    "QCD_HT700to1000", "QCD_HT1000to1500", "QCD_HT1500to2000", "QCD_HT2000toInf",
    "TTTo2L2Nu", "TTToHadronic", "TTToSemiLeptonic",
    "ttHH", "ttbb_Hadronic", "ttbb_SemiLeptonic", "ttbb_2L2Nu",
    "tt4b", "ttHtobb", "ttWH", "ttWW", "ttWZ",
    "ttZHto4b", "ttZZto4b", "ttZtobb", "tttW", "tttt",
})

# NOTE: anchoring is PER-DEDICATED-SAMPLE and lives in STITCH_PLANS below (each
# dedicated sample names its own `anchor` inclusive set + σ_inc). The anchor
# scope = the dedicated sample's decay-channel coverage:
#   - ttbb_c (per-channel 4FS, one per W-decay channel) → anchors to TTTo_c with
#     σ_total × BR_c, rejects tt+2b from TTTo_c only. All three channels covered.
#   - tt4b (decay-inclusive census ~46/44/11 Had/SL/DL) → anchors to the full
#     Had+SL+DL inclusive merge with σ_total (NO BR), rejects tt+nb from all three.
# A single hadronic-only ttbb anchor (option EXP_TTBB_HAD_ONLY) was correct only
# when SL/DL ttbb samples were unavailable; they exist, so EXP now stitches all
# three ttbb channels.
TTBB_HAD_SAMPLE_NAME = "ttbb_Hadronic"      # 4FS ttbb, fully-hadronic
TTBB_SL_SAMPLE_NAME  = "ttbb_SemiLeptonic"  # 4FS ttbb, semi-leptonic
TTBB_DL_SAMPLE_NAME  = "ttbb_2L2Nu"         # 4FS ttbb, di-leptonic
TT4B_SAMPLE_NAME     = "tt4b"               # dedicated tt+nb, decay-inclusive

# ──────────────────────────────────────────────────────────────────────────────
# Constants / logger
# ──────────────────────────────────────────────────────────────────────────────

LOG = logging.getLogger("stitch")

# ── Stitch plans ──────────────────────────────────────────────────────────────
# A plan describes, per DEDICATED sample: which gen categories it OWNS, which
# inclusive samples it is ANCHORED to (+ which σ_inc to use), its own LHE σ, and
# which inclusive samples must have the owned categories REJECTED. This makes the
# decay-channel structure explicit (channels are orthogonal: each
# decay × HF cell is filled by exactly one sample).
#
# Confirmed for this analysis (decay-channel census on the tt4b NanoAOD:
# full-had 45.6%, semi 43.8%, dilep 10.6%; per-channel 4FS ttbb samples exist
# for all three W-decay channels):
#   ttbb_{Had,SL,DL} : PER-CHANNEL 4FS -> each owns tt+2b in its OWN channel,
#       anchored to its matching inclusive TTTo_c (σ_inc = σ_total × BR_c),
#       rejected from TTTo_c only. tt+2b is thus rejected from ALL THREE
#       inclusive samples and filled by the matching dedicated.
#   tt4b             : DECAY-INCLUSIVE -> owns tt+bbb+tt+4b in ALL channels,
#       anchored to the full Had+SL+DL inclusive merge (σ_inc = σ_total, NO BR),
#       rejected from all three inclusive samples.
STITCH_PLANS: dict[str, dict] = {
    "EXP": {
        "inclusive": ["TTToHadronic", "TTToSemiLeptonic", "TTTo2L2Nu"],
        "dedicated": {
            # ── per-channel 4FS ttbb: one dedicated per W-decay channel ──────────
            # Each owns tt+2b in its OWN channel, anchors to the matching inclusive
            # with σ_total × BR_channel, and rejects tt+2b from that inclusive only.
            "ttbb_Hadronic": {
                "owns":            ["tt2b"],
                "anchor":          ["TTToHadronic"],
                "sigma_inc":       SIGMA_INC_NNLO_TOTAL_PB * BR_HAD,   # hadronic phase space
                "sigma_dedicated": SIGMA_TTBB_HAD,
                "reject_from":     ["TTToHadronic"],
            },
            "ttbb_SemiLeptonic": {
                "owns":            ["tt2b"],
                "anchor":          ["TTToSemiLeptonic"],
                "sigma_inc":       SIGMA_INC_NNLO_TOTAL_PB * BR_SL,    # SL phase space
                "sigma_dedicated": SIGMA_TTBB_SL,
                "reject_from":     ["TTToSemiLeptonic"],
            },
            "ttbb_2L2Nu": {
                "owns":            ["tt2b"],
                "anchor":          ["TTTo2L2Nu"],
                "sigma_inc":       SIGMA_INC_NNLO_TOTAL_PB * BR_DL,    # DL phase space
                "sigma_dedicated": SIGMA_TTBB_DL,
                "reject_from":     ["TTTo2L2Nu"],
            },
            # ── decay-inclusive tt4b: covers all three channels ─────────────────
            "tt4b": {
                "owns":            ["ttbbb", "tt4b"],
                "anchor":          ["TTToHadronic", "TTToSemiLeptonic", "TTTo2L2Nu"],
                "sigma_inc":       SIGMA_INC_NNLO_TOTAL_PB,            # all channels, NO BR
                "sigma_dedicated": SIGMA_TT4B_LHE,
                "reject_from":     ["TTToHadronic", "TTToSemiLeptonic", "TTTo2L2Nu"],
            },
        },
    },
    # Conservative fallback: use only when SL/DL 4FS ttbb samples are unavailable.
    # ttbb_Hadronic covers the hadronic channel's tt+2b; SL/DL tt+2b stay as the
    # 5FS inclusive fallback (kept in TTToSemiLeptonic/TTTo2L2Nu). tt4b unchanged.
    "EXP_TTBB_HAD_ONLY": {
        "inclusive": ["TTToHadronic", "TTToSemiLeptonic", "TTTo2L2Nu"],
        "dedicated": {
            "ttbb_Hadronic": {
                "owns":            ["tt2b"],
                "anchor":          ["TTToHadronic"],
                "sigma_inc":       SIGMA_INC_NNLO_TOTAL_PB * BR_HAD,   # hadronic phase space
                "sigma_dedicated": SIGMA_TTBB_HAD,
                "reject_from":     ["TTToHadronic"],
            },
            "tt4b": {
                "owns":            ["ttbbb", "tt4b"],
                "anchor":          ["TTToHadronic", "TTToSemiLeptonic", "TTTo2L2Nu"],
                "sigma_inc":       SIGMA_INC_NNLO_TOTAL_PB,            # all channels, NO BR
                "sigma_dedicated": SIGMA_TT4B_LHE,
                "reject_from":     ["TTToHadronic", "TTToSemiLeptonic", "TTTo2L2Nu"],
            },
        },
    },
    # Use this instead if your tt4b is HADRONIC-ONLY (census ~100% full-had):
    # tt4b then anchors/rejects only in the hadronic channel, like ttbb.
    "EXP_HAD_TT4B": {
        "inclusive": ["TTToHadronic", "TTToSemiLeptonic", "TTTo2L2Nu"],
        "dedicated": {
            "ttbb_Hadronic": {
                "owns": ["tt2b"], "anchor": ["TTToHadronic"],
                "sigma_inc": SIGMA_INC_NNLO_TOTAL_PB * BR_HAD,
                "sigma_dedicated": SIGMA_TTBB_LHE, "reject_from": ["TTToHadronic"],
            },
            "tt4b": {
                "owns": ["ttbbb", "tt4b"], "anchor": ["TTToHadronic"],
                "sigma_inc": SIGMA_INC_NNLO_TOTAL_PB * BR_HAD,
                "sigma_dedicated": SIGMA_TT4B_LHE, "reject_from": ["TTToHadronic"],
            },
        },
    },
}

# expanded genTtbarId %% 100 sub-code -> stitching category label.
CAT_OF_SUB: dict[int, str] = {
    0: "LF",
    41: "cc", 42: "cc", 43: "cc", 44: "cc", 45: "cc",
    51: "ttb", 52: "ttb",
    53: "tt2b", 54: "tt2b", 55: "tt2b",
    61: "ttbbb", 62: "ttbbb",
    71: "tt4b", 72: "tt4b",
}

# inverse: category label -> the expandedTtbarId % 100 sub-codes mapping to it.
CAT_TO_SUBS: dict[str, list[int]] = {}
for _sub, _cat in CAT_OF_SUB.items():
    CAT_TO_SUBS.setdefault(_cat, []).append(_sub)

# ──────────────────────────────────────────────────────────────────────────────
# Data containers
# ──────────────────────────────────────────────────────────────────────────────

@dataclass
class SampleSums:
    """One normCheck row, aggregated across all split files of a sample."""

    sample_name: str
    is_data: bool = False
    n_files: int = 0
    n_events_total: int = 0
    sumGW_total: float = 0.0
    sumGW_pos: float = 0.0
    sumGW_neg: float = 0.0
    sumGW_runs: float = 0.0
    sumGW2_runs: float = 0.0
    # genTtbarId % 100 bin
    sumGW_id: dict[str, float] = field(default_factory=lambda: {
        "lt0": 0.0, "0": 0.0,
        "41": 0.0, "42": 0.0, "43": 0.0, "44": 0.0, "45": 0.0,
        "51": 0.0, "52": 0.0, "53": 0.0, "54": 0.0, "55": 0.0,
        "61": 0.0, "62": 0.0, "71": 0.0, "72": 0.0,   # tt+nb (expanded, §5)
        "other": 0.0,
    })

    def sum_in(self, ids: Iterable[int]) -> float:
        return sum(self.sumGW_id.get(str(i), 0.0) for i in ids)


@dataclass
class DedicatedFactor:
    """Stitching factor for one dedicated sample, anchored to its own phase space."""
    sample: str
    owns: list                 # category labels filled, e.g. ["tt2b"] or ["ttbbb","tt4b"]
    owns_subs: list            # expandedTtbarId%100 codes for `owns`
    anchor: list               # inclusive sample names it is anchored to
    sigma_inc: float           # σ_inc used (× BR_had hadronic-only, NO BR decay-inclusive)
    sigma_dedicated: float     # dedicated LHE σ (cancels against the YAML base weight)
    sumGW_anchor_all: float
    sumGW_anchor_owned: float
    sumGW_dedicated_all: float
    f: float                   # owned / all  in the anchor
    r: float                   # σ_inc · f / σ_dedicated  (multiply dedicated YAML weight by this)
    peF_full: float            # Lumi · σ_inc · f / Σ_dedicated  (full per-event factor, cross-check)


@dataclass
class StitchResult:
    option: str
    lumi_pb_inv: float
    sigma_inc_total: float     # 831.76 pb (NNLO+NNLL)
    br_had: float
    inclusive: list            # inclusive sample names
    dedicated: list            # list[DedicatedFactor]
    reject_map: dict           # inclusive sample -> [category labels removed]

# ──────────────────────────────────────────────────────────────────────────────
# IO
# ──────────────────────────────────────────────────────────────────────────────

def find_normcheck_files(base_dir: Path) -> dict[str, list[Path]]:
    """
    ANALYZER_OUTPUT_DIR의 자식 디렉토리들을 sample 이름으로 보고,
    각 sample 디렉토리 안의 ROOT 파일을 모은다.
    """
    if not base_dir.is_dir():
        raise FileNotFoundError(f"not a directory: {base_dir}")

    child_dirs = {child.name: child for child in sorted(base_dir.iterdir()) if child.is_dir()}

    if ONLY_USE_LISTED_SAMPLE_DIRS:
        scan_names = sorted(EXPECTED_SAMPLE_DIRS)
        missing_dirs = sorted(EXPECTED_SAMPLE_DIRS - set(child_dirs))
        extra_dirs = sorted(set(child_dirs) - EXPECTED_SAMPLE_DIRS)

        if missing_dirs:
            LOG.warning("listed sample directories not found: %s", ", ".join(missing_dirs))
        if extra_dirs:
            LOG.warning(
                "extra directories under output path are ignored because "
                "ONLY_USE_LISTED_SAMPLE_DIRS=True: %s",
                ", ".join(extra_dirs),
            )
    else:
        scan_names = sorted(child_dirs)

    out: dict[str, list[Path]] = {}
    for name in scan_names:
        child = child_dirs.get(name)
        if child is None:
            continue
        roots = sorted(child.glob(ROOT_FILE_GLOB))
        if roots:
            out[name] = roots
        else:
            LOG.warning("no ROOT files matching %r under %s — skipped", ROOT_FILE_GLOB, child)

    return out


def _first_value_as_python(value):
    """Convert numpy scalar / bytes into a plain Python value when possible."""
    if hasattr(value, "item"):
        value = value.item()
    if isinstance(value, bytes):
        return value.decode(errors="replace")
    return value


def read_normcheck_tree(path: Path) -> dict[str, float | int | str | bool] | None:
    """Open one ROOT file and read its `normCheck` tree, expected to contain one row."""
    try:
        with uproot.open(path) as f:
            if PRESCAN_TREE not in f:
                LOG.warning("no `%s` tree in %s — skipped", PRESCAN_TREE, path)
                return None
            tree = f[PRESCAN_TREE]
            if tree.num_entries == 0:
                LOG.warning("empty %s tree in %s — skipped", PRESCAN_TREE, path)
                return None
            arrs = tree.arrays(library="np")
            if not arrs:
                LOG.warning("%s tree has no readable branches in %s — skipped", PRESCAN_TREE, path)
                return None
            # analyzer fills exactly one row per file. If more exist, use first row.
            return {k: _first_value_as_python(v[0]) for k, v in arrs.items()}
    except Exception as e:
        LOG.error("failed to read %s: %s", path, e)
        return None


def aggregate_sample(name: str, paths: list[Path]) -> SampleSums:
    """Merge all per-file normCheck rows for a single sample."""
    s = SampleSums(sample_name=name)
    for p in paths:
        row = read_normcheck_tree(p)
        if row is None:
            continue

        s.n_files += int(row.get("nFiles", 1) or 1)
        s.n_events_total += int(row.get("nEvents_total", 0) or 0)
        s.is_data = bool(row.get("isData", False))

        s.sumGW_total += float(row.get("sumGenW_total", 0.0) or 0.0)
        s.sumGW_pos   += float(row.get("sumGenW_pos",   0.0) or 0.0)
        s.sumGW_neg   += float(row.get("sumGenW_neg",   0.0) or 0.0)
        s.sumGW_runs  += float(row.get("genEventSumw_runs",  0.0) or 0.0)
        s.sumGW2_runs += float(row.get("genEventSumw2_runs", 0.0) or 0.0)

        s.sumGW_id["lt0"]   += float(row.get("sumGenW_id_lt0",   0.0) or 0.0)
        s.sumGW_id["0"]     += float(row.get("sumGenW_id_0",     0.0) or 0.0)
        for i in (41, 42, 43, 44, 45, 51, 52, 53, 54, 55, 61, 62, 71, 72):
            s.sumGW_id[str(i)] += float(row.get(f"sumGenW_id_{i}", 0.0) or 0.0)
        s.sumGW_id["other"] += float(row.get("sumGenW_id_other", 0.0) or 0.0)

    return s

# ──────────────────────────────────────────────────────────────────────────────
# Validation
# ──────────────────────────────────────────────────────────────────────────────

def _close(a: float, b: float, rtol: float = 1e-5, atol: float = 1.0) -> bool:
    return abs(a - b) <= atol + rtol * max(abs(a), abs(b))


def check_sample_consistency(s: SampleSums) -> list[str]:
    """Per-sample sanity checks. Returns warning strings."""
    warns: list[str] = []
    if s.is_data:
        return warns

    if s.sumGW_runs != 0.0:
        rel = (s.sumGW_total - s.sumGW_runs) / s.sumGW_runs
        if abs(rel) > 1e-4:
            warns.append(
                f"  [{s.sample_name}] Events-Runs mismatch: "
                f"events={s.sumGW_total:.6g}, runs={s.sumGW_runs:.6g}, "
                f"rel-diff={rel:.3e}  ← skim 적용된 ntuple 이면 음수가 정상."
            )

    sum_ids = sum(s.sumGW_id.values())
    if not _close(sum_ids, s.sumGW_total, rtol=1e-5, atol=10.0):
        warns.append(
            f"  [{s.sample_name}] Σ id-bins ({sum_ids:.6g}) ≠ "
            f"Σ genW total ({s.sumGW_total:.6g})"
        )

    if s.sumGW_pos > 0:
        neg_frac = abs(s.sumGW_neg) / s.sumGW_pos
        if neg_frac > 0.50:
            warns.append(
                f"  [{s.sample_name}] high negative-weight fraction "
                f"|neg|/pos = {neg_frac:.3f}  ← Powheg/aMC@NLO 정상 범위 확인 필요"
            )
    return warns

# ──────────────────────────────────────────────────────────────────────────────
# Core calculation
# ──────────────────────────────────────────────────────────────────────────────

def merge_named(samples: dict[str, SampleSums], names: Iterable[str],
                label: str) -> SampleSums | None:
    """Sum the named samples into one SampleSums (generic anchor merge)."""
    merged: SampleSums | None = None
    for name in names:
        s = samples.get(name)
        if s is None:
            continue
        if merged is None:
            merged = SampleSums(sample_name=label)
        merged.n_files += s.n_files
        merged.n_events_total += s.n_events_total
        merged.sumGW_total += s.sumGW_total
        merged.sumGW_pos   += s.sumGW_pos
        merged.sumGW_neg   += s.sumGW_neg
        merged.sumGW_runs  += s.sumGW_runs
        merged.sumGW2_runs += s.sumGW2_runs
        for k, v in s.sumGW_id.items():
            merged.sumGW_id[k] = merged.sumGW_id.get(k, 0.0) + v
    return merged


def subs_of_cats(cats: Iterable[str]) -> list[int]:
    """Expand category labels to their expandedTtbarId%100 sub-codes."""
    out: list[int] = []
    for c in cats:
        out.extend(CAT_TO_SUBS.get(c, []))
    return out


def compute_plan(samples: dict[str, SampleSums], option: str,
                 lumi_pb_inv: float) -> StitchResult:
    """Compute per-dedicated stitching factors for a stitch plan.

    Each dedicated sample is anchored to ITS OWN inclusive phase space:
    ttbb_Hadronic -> hadronic inclusive (σ × BR_had); a decay-inclusive tt4b
    -> the full Had+SL+DL inclusive merge (σ, no BR). The reject map removes each
    dedicated's owned categories from exactly the inclusive samples it replaces,
    so every (decay-channel × HF-category) cell is filled once.
    """
    if option not in STITCH_PLANS:
        raise ValueError(f"unknown option {option!r} (valid: {'/'.join(STITCH_PLANS)})")
    plan = STITCH_PLANS[option]
    inclusive = list(plan["inclusive"])

    ded_results: list[DedicatedFactor] = []
    for dname, spec in plan["dedicated"].items():
        anchor = merge_named(samples, spec["anchor"], f"<anchor:{dname}>")
        if anchor is None or anchor.sumGW_total <= 0:
            raise RuntimeError(
                f"empty anchor for dedicated {dname!r}: {spec['anchor']} "
                f"(none found among aggregated samples)")
        owns_subs = subs_of_cats(spec["owns"])
        sumGW_anchor_all   = anchor.sumGW_total
        sumGW_anchor_owned = anchor.sum_in(owns_subs)
        f = sumGW_anchor_owned / sumGW_anchor_all if sumGW_anchor_all > 0 else 0.0
        sig_inc = float(spec["sigma_inc"])
        sig_ded = float(spec["sigma_dedicated"])
        r = (sig_inc * f / sig_ded) if sig_ded > 0 else 0.0
        ded = samples.get(dname)
        sumGW_ded_all = ded.sumGW_total if ded else 0.0
        peF_full = (lumi_pb_inv * sig_inc * f / sumGW_ded_all) if sumGW_ded_all > 0 else 0.0
        ded_results.append(DedicatedFactor(
            sample=dname, owns=list(spec["owns"]), owns_subs=sorted(owns_subs),
            anchor=list(spec["anchor"]), sigma_inc=sig_inc, sigma_dedicated=sig_ded,
            sumGW_anchor_all=sumGW_anchor_all, sumGW_anchor_owned=sumGW_anchor_owned,
            sumGW_dedicated_all=sumGW_ded_all, f=f, r=r, peF_full=peF_full))

    reject_map: dict[str, list[str]] = {inc: [] for inc in inclusive}
    for dname, spec in plan["dedicated"].items():
        for inc in spec["reject_from"]:
            reject_map.setdefault(inc, [])
            for c in spec["owns"]:
                if c not in reject_map[inc]:
                    reject_map[inc].append(c)

    return StitchResult(
        option=option, lumi_pb_inv=lumi_pb_inv,
        sigma_inc_total=SIGMA_INC_NNLO_TOTAL_PB, br_had=BR_HAD,
        inclusive=inclusive, dedicated=ded_results, reject_map=reject_map)


# Analyzer-ready per-event factor table
# ──────────────────────────────────────────────────────────────────────────────

# Canonical category order (from expandedTtbarId % 100 via CAT_OF_SUB).
STITCH_CATEGORIES = ["LF", "cc", "ttb", "tt2b", "ttbbb", "tt4b"]


def build_analyzer_table(res: StitchResult) -> dict:
    """Per-(sample, category) MULTIPLIER applied ON TOP of the YAML base weight:
        final w = yaml_weight[sample] * multiplier[sample][category]
                  * genWeight * (PU*btagSF*trigSF)
    - inclusive samples: 1.0 on kept categories, 0.0 on rejected ones (a dedicated
      sample fills those). The inclusive samples are the anchor; not rescaled.
    - dedicated samples: the rescale factor r on OWNED categories, 0 elsewhere.
      (r folds the anchor σ_inc and owned fraction f; the dedicated LHE σ cancels
      against the YAML base weight, which must be the standard Lumi·σ_ded/Σgenw.)
    - samples NOT listed keep their YAML weight unchanged (multiplier 1).
    """
    samples: dict[str, dict] = {}
    for inc in res.inclusive:
        rej = set(res.reject_map.get(inc, []))
        samples[inc] = {
            "role": "inclusive",
            "by_category": {c: (0.0 if c in rej else 1.0) for c in STITCH_CATEGORIES},
        }
    for d in res.dedicated:
        owned = set(d.owns)
        samples[d.sample] = {
            "role": "dedicated",
            "r": d.r,
            "by_category": {c: (d.r if c in owned else 0.0) for c in STITCH_CATEGORIES},
        }
    return {
        "note": (
            "Per-(sample,category) MULTIPLIER applied ON TOP of the YAML base weight. "
            "final weight = yaml_weight[sample] * multiplier[sample][category] * genWeight "
            "* (PU*btagSF*trigSF). category = sub_to_category[expandedTtbarId%100]. "
            "inclusive: 1 on kept, 0 on rejected (filled by a dedicated). dedicated: r on "
            "owned categories, 0 elsewhere (r rescales the dedicated sample to its inclusive "
            "anchor; the dedicated LHE σ cancels against the YAML base weight). Samples NOT "
            "listed keep their YAML weight (multiplier 1). REQUIREMENT: dedicated YAML weight "
            "must be Lumi*sigma_dedicated/Sumgenw using the SAME per-channel sigma as this "
            "config (sigma_ttbb_Had=1.452pb, sigma_ttbb_SL~1.404pb, sigma_ttbb_DL~0.339pb, "
            "sigma_tt4b=0.296pb); sigma_dedicated cancels so only YAML-vs-config agreement matters."
        ),
        "application": "final_weight = yaml_base_weight[sample] * multiplier[sample][category] * genWeight * (PU*btagSF*trigSF)",
        "sub_to_category": {str(k): v for k, v in sorted(CAT_OF_SUB.items())},
        "categories": STITCH_CATEGORIES,
        "samples": samples,
    }


# ──────────────────────────────────────────────────────────────────────────────
# Reporting
# ──────────────────────────────────────────────────────────────────────────────

def print_config() -> None:
    print()
    print("══════════════════════════════════════════════════════════════════")
    print(" Config")
    print("──────────────────────────────────────────────────────────────────")
    print(f"  ANALYZER_OUTPUT_DIR         = {ANALYZER_OUTPUT_DIR}")
    print(f"  PRESCAN_TREE                = {PRESCAN_TREE}")
    print(f"  OUTPUT_JSON_PATH            = {OUTPUT_JSON_PATH}")
    print(f"  PARTITION_OPTION            = {PARTITION_OPTION}")
    print(f"  ONLY_USE_LISTED_SAMPLE_DIRS = {ONLY_USE_LISTED_SAMPLE_DIRS}")
    print(f"  ROOT_FILE_GLOB              = {ROOT_FILE_GLOB}")
    print(f"  LUMI_PB_INV                 = {LUMI_PB_INV}")
    print(f"  BR_HAD                      = {BR_HAD}")
    print(f"  σ_inc total (NNLO+NNLL)     = {SIGMA_INC_NNLO_TOTAL_PB} pb")
    print(f"  σ_ttbb (hadronic LHE)       = {SIGMA_TTBB_LHE} pb")
    print(f"  σ_tt4b (LHE)                = {SIGMA_TT4B_LHE} pb")
    _plan = STITCH_PLANS.get(PARTITION_OPTION, {})
    print(f"  inclusive samples           = {_plan.get('inclusive')}")
    for _dn, _sp in _plan.get("dedicated", {}).items():
        _br = "x BR_had" if abs(_sp['sigma_inc'] - SIGMA_INC_NNLO_TOTAL_PB * BR_HAD) < 1e-6 else "(no BR)"
        print(f"    dedicated {_dn:<15} owns={_sp['owns']} anchor={_sp['anchor']} "
              f"sigma_inc={_sp['sigma_inc']:.3f}pb {_br}")
    print("══════════════════════════════════════════════════════════════════")


def print_sample_table(samples: dict[str, SampleSums]) -> None:
    print()
    print("══════════════════════════════════════════════════════════════════")
    print(" Sample summary")
    print("──────────────────────────────────────────────────────────────────")
    hdr = (
        f"{'sample':<30}{'files':>8}{'nEv':>14}"
        f"{'Σgenw':>16}{'|neg|/pos':>12}"
    )
    print(hdr)
    print("─" * len(hdr))
    for name in sorted(samples):
        s = samples[name]
        if s.is_data:
            print(f"{name:<30}{s.n_files:>8}{s.n_events_total:>14}"
                  f"{'(data)':>16}{'-':>12}")
            continue
        neg_frac = abs(s.sumGW_neg) / s.sumGW_pos if s.sumGW_pos > 0 else 0.0
        print(f"{name:<30}{s.n_files:>8}{s.n_events_total:>14}"
              f"{s.sumGW_total:>16.6g}{neg_frac:>12.4f}")
    print("══════════════════════════════════════════════════════════════════")


def print_id_breakdown(samples: dict[str, SampleSums]) -> None:
    """Print the genTtbarId % 100 breakdown for ttbar-like samples."""
    print()
    print("══════════════════════════════════════════════════════════════════")
    print(" genTtbarId % 100 breakdown — Σ genW")
    print("──────────────────────────────────────────────────────────────────")
    cols = ("LF(0)", "cc(41-45)", "tt+b(51,52)", "tt+2b(53-55)",
            "tt+bbb(61,62)", "tt+4b(71,72)", "other", "lt0")
    hdr = f"{'sample':<26}" + "".join(f"{c:>14}" for c in cols)
    print(hdr)
    print("─" * len(hdr))
    g = lambda s, *ids: sum(s.sumGW_id.get(str(i), 0.0) for i in ids)
    for name in sorted(samples):
        s = samples[name]
        if s.is_data:
            continue
        vals = [
            g(s, 0), g(s, 41, 42, 43, 44, 45),
            g(s, 51, 52), g(s, 53, 54, 55),
            g(s, 61, 62), g(s, 71, 72),
            g(s, "other"), g(s, "lt0"),
        ]
        if all(v == 0 for v in vals):
            continue
        cells = "".join(f"{v:>14.4g}" for v in vals)
        print(f"{name:<26}{cells}")
    print("══════════════════════════════════════════════════════════════════")


def print_result(res: StitchResult) -> None:
    print()
    print("══════════════════════════════════════════════════════════════════")
    print(f" Stitching result — option {res.option}")
    print("──────────────────────────────────────────────────────────────────")
    print(f"  lumi                    = {res.lumi_pb_inv:>14.2f} pb⁻¹")
    print(f"  σ_inc total (NNLO+NNLL) = {res.sigma_inc_total:>14.4f} pb")
    print(f"  BR_had                  = {res.br_had:>14.6f}")
    print(f"  inclusive samples       = {res.inclusive}")
    print()
    print("  Per-dedicated anchoring (each rescaled to its own decay phase space):")
    for d in res.dedicated:
        print("  " + "-" * 64)
        print(f"   [{d.sample}]  owns={d.owns}  anchor={d.anchor}")
        print(f"     σ_inc(used)         = {d.sigma_inc:>14.4f} pb"
              f"   σ_ded = {d.sigma_dedicated:>9.4f} pb (cancels)")
        print(f"     Σgenw anchor(all)   = {d.sumGW_anchor_all:>14.6g}")
        print(f"     Σgenw anchor(owned) = {d.sumGW_anchor_owned:>14.6g}")
        print(f"     Σgenw dedicated     = {d.sumGW_dedicated_all:>14.6g}")
        print(f"     f = owned/all       = {d.f:>14.6f}")
        print(f"     r = σ_inc·f/σ_ded   = {d.r:>14.6f}"
              f"   <- multiply this sample's YAML weight by r")
        print(f"     (xcheck) Lumi·σ_inc·f/Σ_ded = {d.peF_full:>14.8g}")
    print("  " + "-" * 64)
    print("  Reject map (inclusive sample -> categories removed, filled by a dedicated):")
    for inc, cats in res.reject_map.items():
        print(f"     {inc:<22} reject {cats if cats else '(none)'}")
    print("══════════════════════════════════════════════════════════════════")


def print_analyzer_table(table: dict) -> None:
    print()
    print("══════════════════════════════════════════════════════════════════")
    print(" Analyzer per-event MULTIPLIER table  (-> stitch_factors JSON)")
    print(" rows = sampleName ; cols = category from expandedTtbarId % 100")
    print("──────────────────────────────────────────────────────────────────")
    cats = table["categories"]
    hdr = f"{'sample':<20}{'role':<11}" + "".join(f"{c:>11}" for c in cats)
    print(hdr)
    print("─" * len(hdr))
    for name in sorted(table["samples"]):
        rec = table["samples"][name]
        cells = "".join(f"{rec['by_category'][c]:>11.5g}" for c in cats)
        print(f"{name:<20}{rec['role']:<11}{cells}")
    print("──────────────────────────────────────────────────────────────────")
    print(" 1 = keep (inclusive) ; r = dedicated rescale ; 0 = rejected.\n Applied ON TOP of YAML weight. Samples not shown keep YAML weight (x1).")
    print("══════════════════════════════════════════════════════════════════")


def write_json(out_path: Path, res: StitchResult, analyzer_table: dict) -> None:
    out_path.parent.mkdir(parents=True, exist_ok=True)
    plan = STITCH_PLANS[PARTITION_OPTION]
    payload = {
        "_comment": (
            "Stitching factors + per-event MULTIPLIER for the ttHH FH Run2 2017 UL "
            "analysis. Computed by compute_stitch_factors.py from the analyzer "
            "`prescan`-mode output (expanded genTtbarId, §5). The analyzer loads "
            "`analyzer_perEvent_factor` and applies the multiplier per event via "
            "expandedTtbarId % 100 (see that block's note). Each dedicated sample is "
            "anchored to its OWN decay phase space (per-channel ttbb_{Had,SL,DL}: "
            "matching inclusive, σ×BR_c; decay-inclusive tt4b: all channels, σ no-BR), "
            "per ttHH AN-2022/122 §3.2-3.4 "
            "(§3.3 SL Option1, §3.4 DL gen-level 4b exclusion). Per-dedicated f/r and Σgenw "
            "are under `result` for traceability."
        ),
        "config": {
            "analyzer_output_dir": str(ANALYZER_OUTPUT_DIR),
            "prescan_tree": PRESCAN_TREE,
            "stitch_option": PARTITION_OPTION,
            "only_use_listed_sample_dirs": ONLY_USE_LISTED_SAMPLE_DIRS,
            "root_file_glob": ROOT_FILE_GLOB,
            "lumi_pb_inv": LUMI_PB_INV,
            "br_had": BR_HAD,
            "sigma_inc_total_pb": SIGMA_INC_NNLO_TOTAL_PB,
            "sigma_ttbb_pb": SIGMA_TTBB_LHE,
            "sigma_tt4b_pb": SIGMA_TT4B_LHE,
        },
        "plan": {
            "inclusive": list(plan["inclusive"]),
            "dedicated": {
                k: {kk: (sorted(vv) if isinstance(vv, (set, frozenset)) else vv)
                    for kk, vv in v.items()}
                for k, v in plan["dedicated"].items()
            },
        },
        "result": asdict(res),
        "analyzer_perEvent_factor": analyzer_table,
    }
    out_path.write_text(json.dumps(payload, indent=2))
    LOG.info("wrote %s", out_path)

# ──────────────────────────────────────────────────────────────────────────────
# Main
# ──────────────────────────────────────────────────────────────────────────────

def main() -> int:
    logging.basicConfig(
        level=logging.DEBUG if VERBOSE else logging.INFO,
        format="%(levelname)s  %(message)s",
    )

    if PARTITION_OPTION not in STITCH_PLANS:
        LOG.error("unknown PARTITION_OPTION=%r. Valid: %s",
                  PARTITION_OPTION, "/".join(STITCH_PLANS))
        return 2

    print_config()

    try:
        files_by_sample = find_normcheck_files(ANALYZER_OUTPUT_DIR)
    except FileNotFoundError as e:
        LOG.error("%s", e)
        return 1

    if not files_by_sample:
        LOG.error("no sample directories with ROOT files under %s", ANALYZER_OUTPUT_DIR)
        return 1

    LOG.info("found %d sample directories under %s", len(files_by_sample), ANALYZER_OUTPUT_DIR)

    samples: dict[str, SampleSums] = {}
    for name, paths in files_by_sample.items():
        LOG.debug("aggregating %s (%d files)", name, len(paths))
        samples[name] = aggregate_sample(name, paths)

    print_sample_table(samples)
    print_id_breakdown(samples)

    all_warns: list[str] = []
    for s in samples.values():
        all_warns.extend(check_sample_consistency(s))
    if all_warns:
        print()
        print(" Validation warnings")
        print("──────────────────────────────────────────────────────────────────")
        for w in all_warns:
            print(w)

    try:
        res = compute_plan(samples, option=PARTITION_OPTION, lumi_pb_inv=LUMI_PB_INV)
    except RuntimeError as e:
        LOG.error("%s", e)
        return 2

    print_result(res)

    analyzer_table = build_analyzer_table(res)
    print_analyzer_table(analyzer_table)
    write_json(OUTPUT_JSON_PATH, res, analyzer_table)

    return 0


if __name__ == "__main__":
    sys.exit(main())
