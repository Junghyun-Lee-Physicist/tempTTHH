#!/usr/bin/env python3
"""
compute_stitch_factors_fixed_config.py
──────────────────────────────────────

`ttHHanalyzer_unified --mode normcheck` 출력 파일들을 모아서

ttbar stitching scaling factor (r_B, r_4b) 와 anchor normalization 을 계산한다.

이 버전은 CLI 인자를 쓰지 않는다. analyzer output 위치, partition option,
luminosity, cross section, 출력 JSON 위치 등은 전부 코드 최상단의 USER CONFIG
블록에서 직접 수정한다.

⚠ TEMPORARY:
    `kNormalizationCheck` mode 자체와 마찬가지로, 본격적인 production
    체계에서는 이 정보가 NtupleForge 단계에서 Runs friend tree 로 박혀야
    한다. 이 스크립트는 그 전 단계의 standalone analyzer 결과를 모아
    factor 를 계산하는 임시 후처리이다.

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

분석은 FH channel + ttbb_hadronic-filtered sample 을 사용한다. 따라서
inclusive ttbar anchor base 는 TTToHadronic 만 사용하고, σ_inc 에도
hadronic BR (0.45441081) 을 곱한다 — 그렇게 해야 phase space 가
일치하고 anchoring 식이 자기 일관적이다. (SemiLep/2L2Nu inclusive 는
별도 ttbb_SL/DL sample 이 부재한 상황에서는 어차피 FH analysis 의
lepton veto 에 잘리므로 r_B 계산에서 제외하는 것이 맞다.)

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
    "/pnfs/knu.ac.kr/data/cms/store/user/junghyun/ttHH/AnalyzerOutput_normcheck"
)

# 출력 JSON 위치. 상대 경로면 현재 실행 디렉토리에 생성된다.
OUTPUT_JSON_PATH = Path("stitch_factors_2017.json")

# Partition option:
#   A = ttHH AN SL Option1 차용: inclusive={LF+cc}, ttbb={51,52,53}, tt4b={54,55}
#       → ttbb 가 tt+b / tt+2b 담당, tt4b 가 tt+bbb / tt+4b (id 54,55) 담당
#   B = ttH FH-style:           inclusive={LF+cc}, ttbb={51,52,53,54,55}, tt4b skip
#   C = ttHH Option2:           현재 gen-level partition 기준으로 B와 동일
#                                (Option2 의 차이는 reco-level DNN class 정의)
PARTITION_OPTION = "A"

# 2017 UL — 41.48 fb⁻¹ = 41480 pb⁻¹
LUMI_PB_INV = 41480.0

# Cross sections, all converted to pb.
#
# σ_inc  : NNLO+NNLL inclusive ttbar (Top++ v2.0, m_top = 172.5 GeV) = 831.76 pb
#          We anchor to TTToHadronic only (see header note) so multiply by
#          hadronic branching fraction BR_HAD = 0.45441081
#          → σ_inc_effective = 831.76 × 0.45441081 = 377.964 pb
# σ_ttbb : 1452 fb (hadronic-filtered) = 1.452 pb
# σ_tt4b : 296 fb (inclusive, not BR-filtered per the sample provider) = 0.296 pb
#
# Note:  r_B = σ_inc · f_B / σ_ttbb  → σ_ttbb 는 cancel.  σ_tt4b 도 same.
#        BR 이 σ_inc 와 σ_ttbb 양쪽에 동일하게 들어가면 BR 도 cancel —
#        anchoring 의 본질. 따라서 BR 적용 방식이 self-consistent 한지가
#        중요하지 절대값은 결과에 영향 없음.
BR_HAD                  = 0.45441081
SIGMA_INC_NNLO_TOTAL_PB = 831.76
SIGMA_INC_NNLO          = SIGMA_INC_NNLO_TOTAL_PB * BR_HAD    # ≈ 377.964 pb
SIGMA_TTBB_LHE          = 1.452                                # pb (1452 fb)
SIGMA_TT4B_LHE          = 0.296                                # pb (296 fb)

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
    "ttHH", "ttbb", "tt4b", "ttHtobb", "ttWH", "ttWW", "ttWZ",
    "ttZHto4b", "ttZZto4b", "ttZtobb", "tttW", "tttt",
})

# Inclusive ttbar anchor base — TTToHadronic ONLY.
# Reason: ttbb sample 은 hadronic-filtered 만 사용 가능. 따라서 anchor 도
#         FH phase space 에 맞춰야 자기 일관적. SemiLep / 2L2Nu inclusive 는
#         FH analysis 의 lepton veto 에 잘리는 영역이며, 그것을 anchor base 에
#         포함시키면 r_B 분모가 inconsistent 해진다.
# Future: ttbb_SL / ttbb_DL sample 도 production 되면 다시 합치는 식으로 확장.
INCLUSIVE_TTBAR_NAMES = frozenset({
    "TTToHadronic",
})

# 위 anchor 에서 제외되지만 normCheck report 에는 표시되는 sample 들 — 단순 시각화 목적
ADDITIONAL_TTBAR_FOR_REPORT = frozenset({
    "TTToSemiLeptonic",
    "TTTo2L2Nu",
})

TTBB_SAMPLE_NAME = "ttbb"
TT4B_SAMPLE_NAME = "tt4b"

# ──────────────────────────────────────────────────────────────────────────────
# Constants / logger
# ──────────────────────────────────────────────────────────────────────────────

LOG = logging.getLogger("stitch")

PARTITION_OPTIONS: dict[str, dict[str, frozenset[int]]] = {
    "A": {
        # ttHH AN SL Option1 을 FH 에 차용
        "inclusive": frozenset({0, 41, 42, 43, 44, 45}),
        "ttbb":      frozenset({51, 52, 53}),
        "tt4b":      frozenset({54, 55}),
    },
    "B": {
        # ttH AN FH 처방: ttbb 가 tt+B 전체 담당, tt4b 미사용
        "inclusive": frozenset({0, 41, 42, 43, 44, 45}),
        "ttbb":      frozenset({51, 52, 53, 54, 55}),
        "tt4b":      frozenset(),
    },
    "C": {
        # ttHH AN Option2: 현재 gen-level partition 은 옵션 B 와 동일
        "inclusive": frozenset({0, 41, 42, 43, 44, 45}),
        "ttbb":      frozenset({51, 52, 53, 54, 55}),
        "tt4b":      frozenset(),
    },
}

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
        "other": 0.0,
    })

    def sum_in(self, ids: Iterable[int]) -> float:
        return sum(self.sumGW_id.get(str(i), 0.0) for i in ids)


@dataclass
class StitchResult:
    option: str
    lumi_pb_inv: float
    sigma_inc_NNLO_total: float        # 831.76 pb (참고용)
    br_had: float                       # 0.45441081
    sigma_inc_NNLO_hadronic: float      # 377.964 pb (실제 anchor 에 사용)
    sigma_ttbb_LHE: float
    sigma_tt4b_LHE: float

    f_B: float
    f_4b: float
    r_B: float
    r_4b: float

    sumGW_inc_all: float
    sumGW_inc_ttbb_partition: float
    sumGW_inc_tt4b_partition: float
    sumGW_ttbb_all: float
    sumGW_tt4b_all: float

    # analyzer 의 SampleRegistry weight field에 넣을 수 있는 값.
    # event weight = perEvt_factor_* × genWeight × mask
    perEvt_factor_inclusive: float = 0.0
    perEvt_factor_ttbb: float = 0.0
    perEvt_factor_tt4b: float = 0.0

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
            if "normCheck" not in f:
                LOG.warning("no `normCheck` tree in %s — skipped", path)
                return None
            tree = f["normCheck"]
            if tree.num_entries == 0:
                LOG.warning("empty normCheck tree in %s — skipped", path)
                return None
            arrs = tree.arrays(library="np")
            if not arrs:
                LOG.warning("normCheck tree has no readable branches in %s — skipped", path)
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
        for i in (41, 42, 43, 44, 45, 51, 52, 53, 54, 55):
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

def merge_inclusive(samples: dict[str, SampleSums]) -> SampleSums | None:
    """Sum the inclusive-ttbar samples per INCLUSIVE_TTBAR_NAMES.

    With current config (TTToHadronic only) this is effectively a passthrough
    but kept as a generic merge for future extension (e.g., adding ttbb_SL/DL).
    """
    merged: SampleSums | None = None
    for name in sorted(INCLUSIVE_TTBAR_NAMES):
        if name not in samples:
            continue
        s = samples[name]
        if merged is None:
            merged = SampleSums(sample_name="<inclusive_ttbar_merged>")
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


def compute_factors(
    samples: dict[str, SampleSums],
    option: str,
    lumi_pb_inv: float,
    sigma_inc: float,
    sigma_ttbb: float,
    sigma_tt4b: float,
) -> StitchResult:
    if option not in PARTITION_OPTIONS:
        raise ValueError(f"unknown option {option!r} (valid: A/B/C)")
    part = PARTITION_OPTIONS[option]

    inc_merged = merge_inclusive(samples)
    if inc_merged is None or inc_merged.sumGW_total <= 0:
        raise RuntimeError(
            "No inclusive ttbar samples found — expected at least one of "
            f"{sorted(INCLUSIVE_TTBAR_NAMES)}"
        )

    sumGW_inc_all = inc_merged.sumGW_total

    ids_ttbb = part["ttbb"]
    ids_tt4b = part["tt4b"]

    sumGW_inc_in_ttbb_part = inc_merged.sum_in(ids_ttbb)
    sumGW_inc_in_tt4b_part = inc_merged.sum_in(ids_tt4b)

    f_B  = sumGW_inc_in_ttbb_part / sumGW_inc_all if sumGW_inc_all > 0 else 0.0
    f_4b = sumGW_inc_in_tt4b_part / sumGW_inc_all if sumGW_inc_all > 0 else 0.0

    r_B  = (sigma_inc * f_B  / sigma_ttbb) if sigma_ttbb > 0 else 0.0
    r_4b = (sigma_inc * f_4b / sigma_tt4b) if (sigma_tt4b > 0 and ids_tt4b) else 0.0

    ttbb = samples.get(TTBB_SAMPLE_NAME)
    tt4b = samples.get(TT4B_SAMPLE_NAME)

    sumGW_ttbb_all = ttbb.sumGW_total if ttbb else 0.0
    sumGW_tt4b_all = tt4b.sumGW_total if tt4b else 0.0

    peF_inc = (lumi_pb_inv * sigma_inc) / sumGW_inc_all if sumGW_inc_all > 0 else 0.0
    peF_ttbb = (
        (lumi_pb_inv * sigma_inc * f_B) / sumGW_ttbb_all
        if sumGW_ttbb_all > 0 else 0.0
    )
    peF_tt4b = (
        (lumi_pb_inv * sigma_inc * f_4b) / sumGW_tt4b_all
        if (sumGW_tt4b_all > 0 and ids_tt4b) else 0.0
    )

    return StitchResult(
        option=option,
        lumi_pb_inv=lumi_pb_inv,
        sigma_inc_NNLO_total=SIGMA_INC_NNLO_TOTAL_PB,
        br_had=BR_HAD,
        sigma_inc_NNLO_hadronic=sigma_inc,
        sigma_ttbb_LHE=sigma_ttbb,
        sigma_tt4b_LHE=sigma_tt4b,
        f_B=f_B,
        f_4b=f_4b,
        r_B=r_B,
        r_4b=r_4b,
        sumGW_inc_all=sumGW_inc_all,
        sumGW_inc_ttbb_partition=sumGW_inc_in_ttbb_part,
        sumGW_inc_tt4b_partition=sumGW_inc_in_tt4b_part,
        sumGW_ttbb_all=sumGW_ttbb_all,
        sumGW_tt4b_all=sumGW_tt4b_all,
        perEvt_factor_inclusive=peF_inc,
        perEvt_factor_ttbb=peF_ttbb,
        perEvt_factor_tt4b=peF_tt4b,
    )

# ──────────────────────────────────────────────────────────────────────────────
# Reporting
# ──────────────────────────────────────────────────────────────────────────────

def print_config() -> None:
    print()
    print("══════════════════════════════════════════════════════════════════")
    print(" Config")
    print("──────────────────────────────────────────────────────────────────")
    print(f"  ANALYZER_OUTPUT_DIR         = {ANALYZER_OUTPUT_DIR}")
    print(f"  OUTPUT_JSON_PATH            = {OUTPUT_JSON_PATH}")
    print(f"  PARTITION_OPTION            = {PARTITION_OPTION}")
    print(f"  ONLY_USE_LISTED_SAMPLE_DIRS = {ONLY_USE_LISTED_SAMPLE_DIRS}")
    print(f"  ROOT_FILE_GLOB              = {ROOT_FILE_GLOB}")
    print(f"  LUMI_PB_INV                 = {LUMI_PB_INV}")
    print(f"  BR_HAD                      = {BR_HAD}")
    print(f"  σ_inc total (NNLO+NNLL)     = {SIGMA_INC_NNLO_TOTAL_PB} pb")
    print(f"  σ_inc × BR_HAD              = {SIGMA_INC_NNLO:.6f} pb  ← used for anchor")
    print(f"  σ_ttbb (hadronic LHE)       = {SIGMA_TTBB_LHE} pb")
    print(f"  σ_tt4b                      = {SIGMA_TT4B_LHE} pb")
    print(f"  Inclusive ttbar anchor base = {sorted(INCLUSIVE_TTBAR_NAMES)}")
    print(f"  (excluded from anchor)      = {sorted(ADDITIONAL_TTBAR_FOR_REPORT)}")
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
    cols = ("0", "41-45", "51", "52", "53", "54", "55", "other", "lt0")
    hdr = f"{'sample':<28}" + "".join(f"{c:>12}" for c in cols)
    print(hdr)
    print("─" * len(hdr))
    for name in sorted(samples):
        s = samples[name]
        if s.is_data:
            continue
        cc = sum(s.sumGW_id.get(str(i), 0.0) for i in (41, 42, 43, 44, 45))
        vals = [
            s.sumGW_id.get("0", 0.0), cc,
            s.sumGW_id.get("51", 0.0), s.sumGW_id.get("52", 0.0),
            s.sumGW_id.get("53", 0.0), s.sumGW_id.get("54", 0.0),
            s.sumGW_id.get("55", 0.0),
            s.sumGW_id.get("other", 0.0), s.sumGW_id.get("lt0", 0.0),
        ]
        if all(v == 0 for v in vals):
            continue
        cells = "".join(f"{v:>12.4g}" for v in vals)
        print(f"{name:<28}{cells}")
    print("══════════════════════════════════════════════════════════════════")


def print_result(res: StitchResult) -> None:
    print()
    print("══════════════════════════════════════════════════════════════════")
    print(f" Stitching result — option {res.option}")
    print("──────────────────────────────────────────────────────────────────")
    print(f"  lumi                           = {res.lumi_pb_inv:>14.2f} pb⁻¹")
    print(f"  σ_inc total (NNLO+NNLL)        = {res.sigma_inc_NNLO_total:>14.4f} pb")
    print(f"  BR_HAD                         = {res.br_had:>14.6f}")
    print(f"  σ_inc × BR_HAD (anchor)        = {res.sigma_inc_NNLO_hadronic:>14.4f} pb")
    print(f"  σ_ttbb (LHE hadronic)          = {res.sigma_ttbb_LHE:>14.4f} pb  (cancels)")
    print(f"  σ_tt4b (LHE)                   = {res.sigma_tt4b_LHE:>14.4f} pb  (cancels)")
    print()
    print(f"  Σgenw   inclusive (total)              = {res.sumGW_inc_all:>14.6g}")
    print(f"  Σgenw   inclusive (in ttbb partition)  = {res.sumGW_inc_ttbb_partition:>14.6g}")
    print(f"  Σgenw   inclusive (in tt4b partition)  = {res.sumGW_inc_tt4b_partition:>14.6g}")
    print(f"  Σgenw   ttbb total                     = {res.sumGW_ttbb_all:>14.6g}")
    print(f"  Σgenw   tt4b total                     = {res.sumGW_tt4b_all:>14.6g}")
    print()
    print(f"  f_B   = Σ_inc(ttbb_part) / Σ_inc(all)  = {res.f_B:>14.6f}")
    print(f"  f_4b  = Σ_inc(tt4b_part) / Σ_inc(all)  = {res.f_4b:>14.6f}")
    print(f"  r_B   = σ_inc · f_B  / σ_ttbb          = {res.r_B:>14.6f}")
    print(f"  r_4b  = σ_inc · f_4b / σ_tt4b          = {res.r_4b:>14.6f}")
    print()
    print("──────────────────────────────────────────────────────────────────")
    print(" Final per-event factor, multiply by event genWeight")
    print(" → drop into Config::SampleRegistry as `weight` field")
    print("──────────────────────────────────────────────────────────────────")
    print(f"  inclusive ttbar  : {res.perEvt_factor_inclusive:>14.10g}")
    print(f"  ttbb             : {res.perEvt_factor_ttbb:>14.10g}")
    print(f"  tt4b             : {res.perEvt_factor_tt4b:>14.10g}"
          + ("   (sample SKIPPED in this option)" if res.f_4b == 0.0 else ""))
    print("══════════════════════════════════════════════════════════════════")


def write_json(out_path: Path, res: StitchResult, partition_used: dict[str, frozenset[int]]) -> None:
    payload = {
        "_comment": (
            "Stitching factors for the ttHH FH Run2 2017 UL analysis. "
            "Computed by compute_stitch_factors_fixed_config.py from kNormalizationCheck "
            "analyzer output. Analyzer should read perEvt_factor_* directly into "
            "the SampleRegistry weight field. "
            "ttHH AN-2022/122 §3.3 Option1 / §3.2 (tt+bbb/tt+4b are constructed "
            "at sample level — Option1 uses tt4b sample for both, no gen-level split)."
        ),
        "config": {
            "analyzer_output_dir": str(ANALYZER_OUTPUT_DIR),
            "only_use_listed_sample_dirs": ONLY_USE_LISTED_SAMPLE_DIRS,
            "root_file_glob": ROOT_FILE_GLOB,
            "expected_sample_dirs": sorted(EXPECTED_SAMPLE_DIRS),
            "inclusive_ttbar_names_used_for_anchor": sorted(INCLUSIVE_TTBAR_NAMES),
            "inclusive_ttbar_names_excluded_from_anchor": sorted(ADDITIONAL_TTBAR_FOR_REPORT),
            "ttbb_sample_name": TTBB_SAMPLE_NAME,
            "tt4b_sample_name": TT4B_SAMPLE_NAME,
            "br_had": BR_HAD,
            "sigma_inc_total_pb": SIGMA_INC_NNLO_TOTAL_PB,
        },
        "result": asdict(res),
        "partition": {k: sorted(v) for k, v in partition_used.items()},
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

    if PARTITION_OPTION not in PARTITION_OPTIONS:
        LOG.error("unknown PARTITION_OPTION=%r. Valid options are A/B/C", PARTITION_OPTION)
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
        res = compute_factors(
            samples,
            option=PARTITION_OPTION,
            lumi_pb_inv=LUMI_PB_INV,
            sigma_inc=SIGMA_INC_NNLO,
            sigma_ttbb=SIGMA_TTBB_LHE,
            sigma_tt4b=SIGMA_TT4B_LHE,
        )
    except RuntimeError as e:
        LOG.error("%s", e)
        return 2

    print_result(res)
    write_json(OUTPUT_JSON_PATH, res, PARTITION_OPTIONS[PARTITION_OPTION])

    return 0


if __name__ == "__main__":
    sys.exit(main())
