#!/usr/bin/env python3
"""
================================================================================
SCRIPT: extract_structure.py
================================================================================

[ Purpose ]
Scan an analyzer-output ROOT file and produce `structure_info.yml` listing
EXACTLY the histograms that the stack plotter should draw. This script is the
single source of truth for the "what is plottable?" policy — the plotter
itself reads the yml and trusts it without applying any further filter.

[ What counts as "plottable" ]
By default:
  - TH1F / TH1D / TH1   (1-D histograms)
By default-skipped (always; cannot be undone via flags):
  - TTree / TBranch / TLeaf / TNtuple / TDirectoryFile / TList / TKey
  - TGraph* (graphs are not stackable in this plotter)

Optional inclusions (CLI flags):
  --include-2d        also keep TH2F / TH2D / TH2
  --include-profile   also keep TProfile / TProfile2D
  --include PATTERN   keep ONLY histograms whose key_path matches PATTERN
                      (regex; can be repeated to OR multiple patterns)
  --exclude PATTERN   drop histograms whose key_path matches PATTERN
                      (regex; can be repeated; applied after --include)

[ Output format ]
A FLAT yaml file. The stack plotter consumes this directly, in the order it
appears in the file. Each entry carries enough metadata that the plotter does
not need to re-open the ROOT file just to learn binning.

    description: 'Plottable histograms extracted from <input>'
    input_file: /abs/path/to/sample.root
    total_count: 142
    histograms:
      - key_path: CutflowKinematics/cutStep_8_ht
        classname: TH1F
        title: 'H_{T}'
        nbins: 50
        xlow: 0.0
        xhigh: 4000.0
      - key_path: Tree/cutflow_w
        classname: TH1F
        title: 'N_{weighted}'
        nbins: 11
        xlow: 0.0
        xhigh: 11.0
      ...

[ Usage ]
Default (1-D hists only, default in/out paths):

    python3 extract_structure.py

Custom input file, custom output yml:

    python3 extract_structure.py \
        --input /pnfs/.../baseline/TTToHadronic/TTToHadronic_0.root \
        --output structure_info.yml

Filter examples:

    # Skip all PerJet/* debugging hists (if your analyzer emits them):
    python3 extract_structure.py --exclude '^PerJet/'

    # Only Cutflow group + Tree group:
    python3 extract_structure.py \
        --include '^CutflowKinematics/' --include '^Tree/'

    # Include TH2 maps too:
    python3 extract_structure.py --include-2d

[ Why both `--include` and `--exclude` ]
`--include` semantics: if any --include pattern is given, only matching
entries survive (allowlist). Then --exclude removes from that survivor set.
With no --include flag, everything 1-D (or 1-D + 2-D if --include-2d) goes
through, then --exclude prunes.

[ Notes ]
- Output is yaml-style but written by hand (no PyYAML dependency).
- The hist title can contain ROOT LaTeX syntax (e.g. 'H_{T}'); we wrap the
  value in single quotes when it contains special characters so a downstream
  yaml parser does not misinterpret it.
================================================================================
"""

from __future__ import annotations

import argparse
import os
import re
import sys
from pathlib import Path

try:
    import uproot
except ImportError:
    print("[Error] uproot is required. Install with: pip install uproot")
    sys.exit(1)


# -----------------------------------------------------------------------------
# Plottable classification policy
# -----------------------------------------------------------------------------
# Always-skip set: structural / non-histogram objects. These can never be
# stacked, so there is no flag to override them.
ALWAYS_SKIP_PREFIXES = (
    "TTree", "TBranch", "TLeaf", "TNtuple",
    "TDirectoryFile", "TDirectory", "TList", "TKey",
    "TGraph",
)

# Default-keep set: standard 1-D histograms.
DEFAULT_KEEP_PREFIXES_1D = ("TH1F", "TH1D", "TH1S", "TH1I", "TH1C", "TH1")

# Optional-keep sets, gated by CLI flags.
OPTIONAL_KEEP_2D       = ("TH2F", "TH2D", "TH2S", "TH2I", "TH2C", "TH2")
OPTIONAL_KEEP_PROFILE  = ("TProfile", "TProfile2D")


def is_plottable(classname: str,
                 include_2d: bool,
                 include_profile: bool) -> bool:
    """Return True iff a ROOT class with this name should be kept by default."""
    if any(classname.startswith(p) for p in ALWAYS_SKIP_PREFIXES):
        return False
    if any(classname.startswith(p) for p in DEFAULT_KEEP_PREFIXES_1D):
        return True
    if include_2d and any(classname.startswith(p) for p in OPTIONAL_KEEP_2D):
        return True
    if include_profile and any(classname.startswith(p)
                               for p in OPTIONAL_KEEP_PROFILE):
        return True
    return False


# -----------------------------------------------------------------------------
# Tree-walker: descend the ROOT directory hierarchy and collect leaf hists
# -----------------------------------------------------------------------------
def collect_histograms(node,
                       prefix: str,
                       include_2d: bool,
                       include_profile: bool) -> list[dict]:
    """
    Recursively walk a uproot directory; return a list of dicts describing
    every plottable histogram inside it.
    """
    out: list[dict] = []
    for key in node.keys(cycle=False):
        # `key` may include ';N' cycle suffix in some uproot versions; cycle=False
        # already strips it, but defend against unusual builds.
        clean_key = key.split(";")[0]
        try:
            obj = node[clean_key]
        except Exception as e:
            print(f"  [warn] cannot access {prefix}{clean_key}: {e}")
            continue

        classname = getattr(obj, "classname", obj.__class__.__name__)

        # Recurse into directories regardless of whether they themselves are
        # "plottable" — they can contain histograms.
        if classname.startswith(("TDirectoryFile", "TDirectory")):
            out.extend(collect_histograms(
                obj, prefix + clean_key + "/",
                include_2d, include_profile))
            continue

        if not is_plottable(classname, include_2d, include_profile):
            continue

        # Extract minimal metadata for the plotter
        try:
            title = obj.title if hasattr(obj, "title") else ""
        except Exception:
            title = ""
        nbins = None
        xlow = None
        xhigh = None
        try:
            axis = obj.axis(0) if hasattr(obj, "axis") else None
            if axis is not None:
                nbins = int(getattr(axis, "high", 0) and 0)  # placeholder
        except Exception:
            pass
        # uproot uses axis edges; pull min/max + bin count without forcing
        # the histogram to be loaded fully.
        try:
            edges = obj.axis(0).edges()
            nbins = len(edges) - 1
            xlow = float(edges[0])
            xhigh = float(edges[-1])
        except Exception:
            pass

        out.append({
            "key_path": (prefix + clean_key).lstrip("/"),
            "classname": classname,
            "title": title,
            "nbins": nbins,
            "xlow": xlow,
            "xhigh": xhigh,
        })
    return out


# -----------------------------------------------------------------------------
# Filtering: include / exclude regex
# -----------------------------------------------------------------------------
def apply_path_filters(items: list[dict],
                       include_patterns: list[str],
                       exclude_patterns: list[str]) -> list[dict]:
    if include_patterns:
        inc = [re.compile(p) for p in include_patterns]
        items = [h for h in items if any(r.search(h["key_path"]) for r in inc)]
    if exclude_patterns:
        exc = [re.compile(p) for p in exclude_patterns]
        items = [h for h in items if not any(r.search(h["key_path"]) for r in exc)]
    return items


# -----------------------------------------------------------------------------
# YAML emit (hand-written, no PyYAML dependency)
# -----------------------------------------------------------------------------
def yaml_quote(value) -> str:
    """Quote a scalar safely for a hand-written yaml line."""
    if value is None:
        return "null"
    if isinstance(value, bool):
        return "true" if value else "false"
    if isinstance(value, (int, float)):
        # avoid Python "1e-05" or "inf"; format plainly
        if isinstance(value, float):
            if value != value:    # NaN
                return "null"
            return f"{value}"
        return str(value)
    s = str(value)
    if s == "":
        return "''"
    needs_quote = any(c in s for c in ":#[]{},&*!|>'\"%@`") or \
                  s != s.strip() or \
                  s.lower() in ("yes", "no", "true", "false", "null", "~")
    if needs_quote:
        # single-quote: escape internal single-quotes by doubling
        return "'" + s.replace("'", "''") + "'"
    return s


def emit_yaml(items: list[dict],
              input_file: Path,
              out_path: Path) -> None:
    lines = [
        f"description: {yaml_quote(f'Plottable histograms extracted from {input_file}')}",
        f"input_file: {yaml_quote(str(input_file))}",
        f"total_count: {len(items)}",
        "histograms:",
    ]
    for h in items:
        lines.append(f"  - key_path: {yaml_quote(h['key_path'])}")
        lines.append(f"    classname: {yaml_quote(h['classname'])}")
        lines.append(f"    title: {yaml_quote(h['title'])}")
        lines.append(f"    nbins: {yaml_quote(h['nbins'])}")
        lines.append(f"    xlow: {yaml_quote(h['xlow'])}")
        lines.append(f"    xhigh: {yaml_quote(h['xhigh'])}")
    out_path.write_text("\n".join(lines) + "\n")


# -----------------------------------------------------------------------------
# Driver
# -----------------------------------------------------------------------------
def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(
        description="Extract a flat list of plottable histograms from a ROOT file"
    )
    parser.add_argument(
        "--input",
        default="/Users/jhlee/ttHH/ntuple/skimmed/gen_tier3/TTToHadronic.root",
        help="Input ROOT file (default mirrors the previous hardcode)"
    )
    parser.add_argument(
        "--output",
        default="structure_info.yml",
        help="Output yml path (default: structure_info.yml in cwd)"
    )
    parser.add_argument(
        "--include-2d", action="store_true",
        help="Also keep TH2F/TH2D-class histograms"
    )
    parser.add_argument(
        "--include-profile", action="store_true",
        help="Also keep TProfile-class histograms"
    )
    parser.add_argument(
        "--include", action="append", default=[],
        help="Only keep entries whose key_path matches this regex "
             "(repeatable; OR-combined)"
    )
    parser.add_argument(
        "--exclude", action="append", default=[],
        help="Drop entries whose key_path matches this regex "
             "(repeatable; applied after --include)"
    )
    args = parser.parse_args(argv)

    in_path = Path(args.input)
    out_path = Path(args.output)

    if not in_path.is_file():
        print(f"[Error] file not found: {in_path}")
        return 2

    print(f"[-] Inspecting structure of: {in_path}")
    try:
        with uproot.open(str(in_path)) as f:
            items = collect_histograms(
                f, prefix="",
                include_2d=args.include_2d,
                include_profile=args.include_profile,
            )
    except Exception as e:
        print(f"[Error] failed to open: {e}")
        return 2

    n_before = len(items)
    items = apply_path_filters(items, args.include, args.exclude)
    n_after = len(items)

    if not items:
        print("[Warn] no plottable histograms after filtering!")
    else:
        print(f"[-] kept {n_after}/{n_before} histogram(s) after filters")

    # Stable order: first by directory path (depth-first traversal preserves
    # the natural ROOT ordering), so we just keep insertion order.
    emit_yaml(items, in_path, out_path)
    print(f"[Success] wrote {out_path}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
