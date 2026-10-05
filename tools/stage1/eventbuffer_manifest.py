#!/usr/bin/env python3
"""eventbuffer_manifest.py -- the branch union of our ntuples for include/eventBuffer.h (STEP 24; PLAN 9.3, 9.6 D16)

KNU job (reads /pnfs). The log it writes is the record: the header is made from that log on the Mac
(tools/stage1/eventbuffer_from_record.py) and committed there, so nothing but runlogs/ is committed
at KNU.

    python3 tools/stage1/eventbuffer_manifest.py --treestream ../treestream
            [--base TAG=DIR ...] [--glob '*/*/*/*/*.root'] [--group-level 2] [--tree Events]
            [--keep tools/stage1/hlt_keep.txt] [--source FILE ...] [--work DIR] [--max-try 5]

Without --base the KNU productions are used (under $KNU_STORE, default
/pnfs/knu.ac.kr/data/cms/store/user/junghyun): 2024MC ttHH2024_v15_had_MC_v1, 2024Data
ttHH2024_v15_had_Data_v1, 2018MC ttHH2018UL_v15_had_MC_v1, 2018Data ttHH2018UL_v15_had_Data_v1,
2017 ttHH2017UL_fullNano_v20. A base that does not exist is reported (MISSINGBASE) and skipped.

Steps
  1. per base and CRAB task (the directory --group-level above the file; default 2 = <PD>/<request>/
     <YYMMDD_hhmmss>, so a dataset with several task directories gives one file per task), the first
     file in sorted order that opens cleanly (not a zombie, not recovered, has the tree); failed/ and
     log/ are skipped; at most --max-try files are tried per task
  2. one symlink per file with a unique name: mkvariables.py --merge keys its inputs by basename,
     and CRAB file names (forgedNtuple_1.root, ...) repeat across datasets
  3. the fork's mkvariables.py --merge -> variables_raw.txt (widest type on type conflicts)
  4. records named HLT_* or L1_* are dropped unless listed in --keep -> variables.txt
  5. checks: every HLT name the analyzer source reads (_ev->HLT_x, "Events/HLT_x") is kept (else
     exit 1: the header would not have it); kept names that no input has are listed as WARN
  6. the fork's mkeventbuffer.py variables.txt -> eventBuffer.h, to show that it generates; its md5
     is printed without the 'Created:' and 'Author:' lines so another machine can reproduce it
  7. variables.txt is printed between 'BEGIN VARIABLES <md5>' and 'END VARIABLES'

Exit: 0 ok; 1 a check failed (CODE_MISSING, a task without a readable file, a base that is missing
or matches no file); 2 bad arguments or setup (no treestream build, no input at all); 3 a generator
(mkvariables.py, mkeventbuffer.py) failed. Only a record that ends with 'RESULT OK' is accepted by
eventbuffer_from_record.py.
"""
from __future__ import print_function

import argparse
import collections
import glob
import hashlib
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
MENU_PREFIXES = ("HLT_", "L1_")
DEFAULT_BASES = [
    ("2024MC", "ttHH2024_v15_had_MC_v1"),
    ("2024Data", "ttHH2024_v15_had_Data_v1"),
    ("2018MC", "ttHH2018UL_v15_had_MC_v1"),
    ("2018Data", "ttHH2018UL_v15_had_Data_v1"),
    ("2017", "ttHH2017UL_fullNano_v20"),
]
CODE_HLT_RE = re.compile(r'(?:_ev->|"Events/)(HLT_[A-Za-z0-9_]+)')
NORMALIZE_RE = re.compile(r"^// (Created|Author):")


def md5_bytes(b):
    return hashlib.md5(b).hexdigest()


def md5_file(path):
    with open(path, "rb") as f:
        return md5_bytes(f.read())


def normalized_header_md5(path):
    """md5 of a generated header without the lines that depend on the time and the user."""
    with open(path, "rb") as f:
        lines = f.read().decode("utf-8", "replace").splitlines(True)
    keep = [l for l in lines if not NORMALIZE_RE.match(l)]
    return md5_bytes("".join(keep).encode("utf-8")), len(lines)


def is_record(line):
    s = line.strip()
    return bool(s) and not s.startswith("#") and s.split()[0].lower() not in ("tree", "tree:") and "/" in s


def parse_record(line):
    """'type/Events/Branch/Name/count counter' -> (type, tree/branch, name, 'count counter')."""
    head, _, tail = line.strip().rpartition("/")
    head2, _, name = head.rpartition("/")
    btype, _, tb = head2.partition("/")
    return btype, tb, name, tail


def read_keep(path):
    names, seen = [], set()
    with open(path) as f:
        for raw in f:
            s = raw.split("#", 1)[0].strip()
            if not s:
                continue
            if not s.startswith(MENU_PREFIXES):
                raise ValueError("keep list %s: %r does not start with HLT_ or L1_" % (path, s))
            if s not in seen:
                seen.add(s)
                names.append(s)
    return names


def code_hlt_names(sources):
    names = set()
    for p in sources:
        with open(p) as f:
            names.update(CODE_HLT_RE.findall(f.read()))
    return names


def prune(raw_lines, keep):
    """Drop HLT_/L1_ records not in keep. Returns (records kept, stats)."""
    keep = set(keep)
    out, dropped, kept_menu, n_rec = [], collections.Counter(), set(), 0
    for line in raw_lines:
        if not is_record(line):
            continue
        n_rec += 1
        name = parse_record(line)[2]
        pre = next((p for p in MENU_PREFIXES if name.startswith(p)), None)
        if pre is not None and name not in keep:
            dropped[pre] += 1
            continue
        if pre is not None:
            kept_menu.add(name)
        out.append(line.rstrip("\n"))
    return out, {"records_in": n_rec, "dropped": dropped, "kept_menu": kept_menu}


def variables_text(records, tree, comment_lines):
    """The pruned manifest in the format mkanalyzer.py reads (first line 'Tree <name>', a blank line)."""
    body = ["Tree %s" % tree, ""] + ["# " + c for c in comment_lines] + [""] + records
    return "\n".join(body) + "\n"


def check_file(ROOT, path, tree):
    why = None
    tf = None
    try:
        tf = ROOT.TFile.Open(path)
    except Exception as e:  # newer PyROOT raises OSError
        why = "cannot open (%s)" % str(e).splitlines()[0][:100] if str(e) else "cannot open"
    if why is None and (not tf or tf.IsZombie()):
        why = "cannot open (zombie)"
    if why is None and tf.TestBit(ROOT.TFile.kRecovered):
        why = "truncated (ROOT recovered its keys)"
    t = tf.Get(tree) if why is None else None
    if why is None and not t:
        why = "no tree %s" % tree
    n = t.GetListOfBranches().GetEntries() if why is None else 0
    if tf:
        tf.Close()
    return why, n


def pick_inputs(ROOT, bases, pattern, group_level, tree, max_try):
    chosen, problems = [], 0
    for tag, base in bases:
        if not os.path.isdir(base):
            print("MISSINGBASE %s %s" % (tag, base))
            problems += 1
            continue
        files = sorted(f for f in glob.glob(os.path.join(base, pattern))
                       if "/failed/" not in f and "/log/" not in f)
        groups = collections.OrderedDict()
        for f in files:
            g = f
            for _ in range(group_level):
                g = os.path.dirname(g)
            groups.setdefault(os.path.relpath(g, base), []).append(f)
        print("BASE %s %s datasets=%d files=%d" % (tag, base, len(groups), len(files)))
        if not files:
            print("EMPTYBASE %s %s (no file matches the pattern)" % (tag, base))
            problems += 1
        sys.stdout.flush()
        for key, flist in groups.items():
            for f in flist[:max_try]:
                why, n = check_file(ROOT, f, tree)
                if why is None:
                    chosen.append((tag, key, f, n))
                    break
                print("SKIP %s %s %s %s" % (tag, key, f, why))
            else:
                print("NOINPUT %s %s (no readable file among the first %d)" % (tag, key, min(max_try, len(flist))))
                problems += 1
        sys.stdout.flush()
    return chosen, problems


def summarize_conflict(line):
    """'Name: a.root=uchar, b.root=int -> int' -> 'Name: uchar x1, int x1 -> int' (one line per branch
    instead of one entry per input file)."""
    m = re.match(r"^\s*(\S+): (.*) -> (\S+)\s*$", line)
    if not m:
        return line.strip()
    counts = collections.OrderedDict()
    for item in m.group(2).split(", "):
        t = item.rpartition("=")[2]
        counts[t] = counts.get(t, 0) + 1
    return "%s: %s -> %s" % (m.group(1), ", ".join("%s x%d" % kv for kv in counts.items()), m.group(3))


def link_name(i, tag, key):
    s = re.sub(r"[^A-Za-z0-9_.-]+", "_", key.replace("/", "__"))
    return "%03d_%s__%s.root" % (i, tag, s)


def treestream_info(ts):
    try:
        c = subprocess.run(["git", "-C", ts, "log", "-1", "--format=%h %s"], stdout=subprocess.PIPE,
                           stderr=subprocess.DEVNULL, universal_newlines=True).stdout.strip()
        st = subprocess.run(["git", "-C", ts, "status", "--short", "--", "bin", "src/treestream.cc",
                             "include/treestream.h"], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
                            universal_newlines=True).stdout.strip()
    except OSError:
        c, st = "", ""
    return (c or "?"), ("clean" if not st else "MODIFIED: " + " ".join(st.split()))


def tool_env(ts):
    env = dict(os.environ)
    env["TREESTREAM_PATH"] = ts
    for var, add in (("PATH", os.path.join(ts, "bin")), ("LD_LIBRARY_PATH", os.path.join(ts, "lib")),
                     ("PYTHONPATH", ts + os.pathsep + os.path.join(ts, "lib"))):
        env[var] = add + (os.pathsep + env[var] if env.get(var) else "")
    return env


def run_tool(cmd, cwd, env, logpath):
    with open(logpath, "w") as log:
        rc = subprocess.call(cmd, cwd=cwd, env=env, stdout=log, stderr=subprocess.STDOUT)
    with open(logpath) as f:
        return rc, f.read().splitlines()


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--treestream", required=True, help="the fork checkout (built: lib/libtreestream.so)")
    ap.add_argument("--base", action="append", default=[], help="TAG=DIR (repeatable)")
    ap.add_argument("--glob", default="*/*/*/*/*.root")
    ap.add_argument("--group-level", type=int, default=2,
                    help="one input per directory this many levels above the file (default 2 = the CRAB task\n"
                         "<PD>/<request>/<YYMMDD_hhmmss>: every task of a dataset, old ones included, adds its branches)")
    ap.add_argument("--tree", default="Events")
    ap.add_argument("--keep", default=os.path.join(REPO, "tools", "stage1", "hlt_keep.txt"))
    ap.add_argument("--source", action="append", default=[],
                    help="analyzer source to scan for HLT names (default: ttHHanalyzer_unified.cc/.h)")
    ap.add_argument("--work", default="")
    ap.add_argument("--max-try", type=int, default=5)
    a = ap.parse_args(argv)

    ts = os.path.abspath(a.treestream)
    need = ["bin/mkvariables.py", "bin/mkeventbuffer.py", "bin/mkanalyzer.py", "include/treestream.h"]
    miss = [p for p in need if not os.path.isfile(os.path.join(ts, p))]
    if miss:
        print("ERROR not a treestream checkout (missing %s): %s" % (", ".join(miss), ts))
        return 2
    if not glob.glob(os.path.join(ts, "lib", "libtreestream.*")):
        print("ERROR %s/lib/libtreestream.* missing: build it first (cd %s && make lib)" % (ts, ts))
        return 2
    commit, state = treestream_info(ts)
    print("TREESTREAM %s @ %s (%s)" % (ts, commit, state))

    bases = []
    for b in a.base:
        if "=" not in b:
            print("ERROR --base needs TAG=DIR: %s" % b)
            return 2
        t, d = b.split("=", 1)
        bases.append((t, d))
    if not bases:
        store = os.environ.get("KNU_STORE", "/pnfs/knu.ac.kr/data/cms/store/user/junghyun")
        bases = [(t, os.path.join(store, d)) for t, d in DEFAULT_BASES]
    sources = a.source or [os.path.join(REPO, "ttHHanalyzer_unified.cc"), os.path.join(REPO, "ttHHanalyzer_unified.h")]
    try:
        keep = read_keep(a.keep)
    except (OSError, ValueError) as e:
        print("ERROR keep list: %s" % e)
        return 2
    print("KEEP %s names=%d md5=%s" % (a.keep, len(keep), md5_file(a.keep)))
    print("TOOL %s md5=%s" % (os.path.abspath(__file__), md5_file(os.path.abspath(__file__))))
    sys.stdout.flush()

    work = os.path.abspath(a.work or os.path.join(os.environ.get("TMPDIR", "/tmp"),
                                                  "eventbuffer_manifest_%d" % os.getpid()))
    os.makedirs(os.path.join(work, "inputs"), exist_ok=True)
    print("WORK %s" % work)

    import ROOT
    ROOT.gROOT.SetBatch(True)
    chosen, problems = pick_inputs(ROOT, bases, a.glob, a.group_level, a.tree, a.max_try)
    if not chosen:
        print("ERROR no input file")
        return 2
    links = []
    for i, (tag, key, f, n) in enumerate(chosen):
        ln = os.path.join(work, "inputs", link_name(i, tag, key))
        if os.path.lexists(ln):
            os.remove(ln)
        os.symlink(os.path.abspath(f), ln)
        links.append(ln)
        print("INPUT %03d %s %s branches=%d %s" % (i, tag, key, n, f))
    sys.stdout.flush()

    env = tool_env(ts)
    raw = os.path.join(work, "variables_raw.txt")
    if len(links) > 1:
        cmd = [sys.executable, os.path.join(ts, "bin", "mkvariables.py"), "--merge"] + links + ["--tree", a.tree, "-o", raw]
    else:   # --merge needs two files; single mode writes ./variables.txt
        if os.path.exists(os.path.join(work, "variables.txt")):
            os.remove(os.path.join(work, "variables.txt"))
        cmd = [sys.executable, os.path.join(ts, "bin", "mkvariables.py"), links[0], a.tree]
    rc, lines = run_tool(cmd, work, env, os.path.join(work, "mkvariables.log"))
    if len(links) == 1 and rc == 0 and os.path.isfile(os.path.join(work, "variables.txt")):
        os.replace(os.path.join(work, "variables.txt"), raw)
    if len(links) == 1:
        lines.append("  Total unique branches (union): %s" % next(
            (l.split(":")[-1].strip() for l in lines if "branches found" in l), "?"))
    conflicts = [summarize_conflict(l) for l in lines if re.match(r"^\s{4}\S+: .* -> \S+$", l)]
    union = next((l.split(":")[-1].strip() for l in lines if "Total unique branches" in l), "?")
    print("MKVAR rc=%d union=%s conflicts=%d" % (rc, union, len(conflicts)))
    for c in conflicts:
        print("CONFLICT %s" % c)
    if rc != 0 or not os.path.isfile(raw):
        print("--- tail of mkvariables.log ---")
        print("\n".join(lines[-30:]))
        return 3

    with open(raw) as f:
        raw_lines = f.readlines()
    records, st = prune(raw_lines, keep)
    print("PRUNE records_in=%d kept=%d dropped_HLT=%d dropped_L1=%d kept_menu=%d"
          % (st["records_in"], len(records), st["dropped"]["HLT_"], st["dropped"]["L1_"], len(st["kept_menu"])))
    absent = [k for k in keep if k not in st["kept_menu"]]
    for k in absent:
        print("KEEP_ABSENT %s (WARN: in no input)" % k)
    code = code_hlt_names(sources)
    code_missing = sorted(code - st["kept_menu"])
    print("CODE_HLT names=%d missing=%d (sources: %s)" % (len(code), len(code_missing),
                                                           ", ".join(os.path.relpath(s, REPO) for s in sources)))
    for c in code_missing:
        print("CODE_MISSING %s (read by the analyzer, not kept: add it to the keep list or an input that has it)" % c)

    comment = ["tempTTHH tools/stage1/eventbuffer_manifest.py (STEP 24); treestream %s" % commit,
               "inputs: %d files, one per dataset, from %s" % (len(chosen), ", ".join(sorted(set(c[0] for c in chosen)))),
               "union %s records; HLT_/L1_ records kept only from %s (md5 %s): %d of %d"
               % (st["records_in"], os.path.basename(a.keep), md5_file(a.keep), len(st["kept_menu"]),
                  len(st["kept_menu"]) + sum(st["dropped"].values()))]
    text = variables_text(records, a.tree, comment)
    var = os.path.join(work, "variables.txt")
    with open(var, "w") as f:
        f.write(text)
    vmd5 = md5_file(var)

    hdr = os.path.join(work, "eventBuffer.h")
    rc2, lines2 = run_tool([sys.executable, os.path.join(ts, "bin", "mkeventbuffer.py"), var, "-o", hdr],
                           work, env, os.path.join(work, "mkeventbuffer.log"))
    if rc2 != 0 or not os.path.isfile(hdr):
        print("HEADER rc=%d" % rc2)
        print("\n".join(lines2[-30:]))
        return 3
    hmd5, hn = normalized_header_md5(hdr)
    print("HEADER rc=0 lines=%d md5_normalized=%s (without the Created/Author lines)" % (hn, hmd5))
    print("VARIABLES md5=%s lines=%d records=%d" % (vmd5, text.count("\n"), len(records)))
    print("BEGIN VARIABLES %s" % vmd5)
    sys.stdout.write(text)
    print("END VARIABLES")
    bad = []
    if code_missing:
        bad.append("%d HLT names read by the analyzer are not kept" % len(code_missing))
    if problems:
        bad.append("%d bases or tasks without a readable file (MISSINGBASE, EMPTYBASE, NOINPUT)" % problems)
    print("RESULT %s" % ("OK" if not bad else "FAIL (" + "; ".join(bad) + ")"))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
