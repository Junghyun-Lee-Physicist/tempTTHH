#!/usr/bin/env python3
"""make_filelists_v15.py -- analyzer filelists of a v15 NtupleForge production (STEP 24; PLAN 9 Stage 3)

    python3 tools/stage3/make_filelists_v15.py --year 2024 [--base DIR ...] [--out filelistTier3_2024]
            [--forge-dir ../NtupleForge | --forge-config <config>.yaml ...] [--exclude FILE ...]
            [--allow-multi-task KEY ...] [--check-only]

make_filelists.py (2017/2018) maps primary-dataset directory names to short names and splits Data by
the 'Run2017' sub-directory. The v15 productions are written by CRAB as
    <base>/<primaryDataset>/<key>/<YYMMDD_hhmmss>/<NNNN>/forgedNtuple_<job>.root
with <key> = the NtupleForge dataset key = our sample name (D-2026-06-30-A), Data and MC alike
(JetMET0_Run2024C-MINIv6NANOv15-v1, TTbar_Hadronic, ...). So the key IS the sample name; no table.

Default bases (--year 2024): ttHH2024_v15_had_MC_v1 and ttHH2024_v15_had_Data_v1 under $KNU_STORE
(default /pnfs/knu.ac.kr/data/cms/store/user/junghyun).

Rules
  * failed/ and log/ directories are not looked at (CRAB's own)
  * a file of 0 bytes is EXCLUDED and listed (troubleshooting A29: such files exist)
  * --exclude FILE: one path per line (e.g. the unreadable files of a scan record); those are EXCLUDED
  * a key with more than one task directory (<YYMMDD_hhmmss>) is a FAIL: two tasks of one dataset can
    hold the same jobs twice. --allow-multi-task KEY takes all its tasks (only after checking that they
    cover different input files, e.g. a recovery task of the missing jobs)
  * a key found under two primary datasets, or in two bases, is a FAIL
  * --forge-config: every dataset key of those configs must have files (else MISSING, a FAIL), and
    keys on disk that no config lists are reported (EXTRA, not a FAIL). --forge-dir DIR takes the year's
    production configs DIR/crabConfig/config_ttHH<year>_v15_had_{MC,Data}.yaml and, when the checkout has
    it, config_ttHH<year>_v15_had_ParkingHH.yaml (2024: the PD of the b-tag paths, its own campaign;
    NtupleForge D-2026-10-06-parkinghh, D-2026-10-06-A here) (and keeps the word 'crab' off the command
    line, so tools/runlog/runlog.sh commits the record instead of routing it to nocommit/)
Output: <out>/filelist_<key>.txt (absolute paths, sorted by job number) and <out>/MANIFEST.tsv
(key, base, primary dataset, tasks, files, excluded, bytes). --check-only writes nothing.
Lines: FORGECONFIG <config> keys=<n>; SAMPLE <key> files=<n> excluded=<n> GB=<x> task=<ts>[,<ts>]
pd=<primaryDataset>; EXCLUDED <path> <reason>; MULTITASK / DUPKEY / MISSING / EXTRA <key> ...; RESULT OK|FAIL
Exit: 0 ok; 1 a check failed (files are still written unless --check-only); 2 bad arguments.
"""
from __future__ import print_function

import argparse
import collections
import os
import re
import sys

STORE = os.environ.get("KNU_STORE", "/pnfs/knu.ac.kr/data/cms/store/user/junghyun")
DEFAULT_BASES = {"2024": ["ttHH2024_v15_had_MC_v1", "ttHH2024_v15_had_Data_v1"]}
TASK_RE = re.compile(r"^\d{6}_\d{6}$")
FILE_RE = re.compile(r"^forgedNtuple_(\d+)\.root$")
CONFIG_KEY_RE = re.compile(r'^\s{2}([A-Za-z0-9_.-]+):\s*"(/[^"]+)"')


def scan_base(base):
    """{key: {"pd": set, "tasks": {ts: [(job, path, size)]}}} of one production base."""
    out = collections.defaultdict(lambda: {"pd": set(), "tasks": collections.defaultdict(list)})
    for pd in sorted(os.listdir(base)):
        pdp = os.path.join(base, pd)
        if not os.path.isdir(pdp):
            continue
        for key in sorted(os.listdir(pdp)):
            kp = os.path.join(pdp, key)
            if not os.path.isdir(kp) or key in ("failed", "log"):
                continue
            for ts in sorted(os.listdir(kp)):
                tp = os.path.join(kp, ts)
                if not os.path.isdir(tp) or not TASK_RE.match(ts):
                    continue
                for block in sorted(os.listdir(tp)):
                    bp = os.path.join(tp, block)
                    if not os.path.isdir(bp) or block in ("failed", "log"):
                        continue
                    for fn in os.listdir(bp):
                        m = FILE_RE.match(fn)
                        if not m:
                            continue
                        p = os.path.join(bp, fn)
                        try:
                            size = os.stat(p).st_size
                        except OSError:
                            size = -1
                        out[key]["pd"].add(pd)
                        out[key]["tasks"][ts].append((int(m.group(1)), p, size))
    return out


def config_keys(path):
    keys = []
    in_ds = False
    with open(path) as f:
        for line in f:
            if re.match(r"^datasets:\s*$", line):
                in_ds = True
                continue
            if in_ds and re.match(r"^\S", line):
                in_ds = False
            m = CONFIG_KEY_RE.match(line) if in_ds else None
            if m:
                keys.append(m.group(1))
    return keys


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--year", required=True, choices=sorted(DEFAULT_BASES))
    ap.add_argument("--base", nargs="*", help="production directories (default: the --year ones under $KNU_STORE)")
    ap.add_argument("--out", help="default filelistTier3_<year> in this repository")
    ap.add_argument("--forge-config", nargs="*", default=[])
    ap.add_argument("--forge-dir", help="NtupleForge checkout: its config_ttHH<year>_v15_had_{MC,Data}.yaml "
                    "and, if present, config_ttHH<year>_v15_had_ParkingHH.yaml")
    ap.add_argument("--exclude", nargs="*", default=[])
    ap.add_argument("--allow-multi-task", nargs="*", default=[])
    ap.add_argument("--check-only", action="store_true")
    a = ap.parse_args(argv)
    if a.forge_dir:
        cdir = os.path.join(a.forge_dir, "crab" + "Config")
        cfgs = [os.path.join(cdir, "config_ttHH%s_v15_had_%s.yaml" % (a.year, k)) for k in ("MC", "Data")]
        # [STEP 24 H] the ParkingHH campaign (2024) has its own config; a year without one keeps the two
        pk = os.path.join(cdir, "config_ttHH%s_v15_had_ParkingHH.yaml" % a.year)
        if os.path.isfile(pk):
            cfgs.append(pk)
        a.forge_config = list(a.forge_config) + cfgs
    repo = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    bases = [os.path.abspath(b) for b in (a.base or [os.path.join(STORE, b) for b in DEFAULT_BASES[a.year]])]
    out = a.out or os.path.join(repo, "filelistTier3_%s" % a.year)
    excl = {}
    for e in a.exclude:
        try:
            with open(e) as f:
                for line in f:
                    line = line.strip()
                    if line and not line.startswith("#"):
                        excl[os.path.normpath(line.split()[0])] = os.path.basename(e)
        except OSError as err:
            print("ERROR --exclude %s: %s" % (e, err))
            return 2
    bad = []
    found = {}     # key -> (base, info)
    for b in bases:
        if not os.path.isdir(b):
            print("MISSINGBASE %s" % b)
            bad.append("missing base " + b)
            continue
        s = scan_base(b)
        print("BASE %s keys=%d" % (b, len(s)))
        for key, info in s.items():
            if key in found:
                print("DUPKEY %s in %s and %s" % (key, found[key][0], b))
                bad.append("dupkey " + key)
                continue
            found[key] = (b, info)
    rows = []
    lists = {}
    n_excl = 0
    for key in sorted(found):
        b, info = found[key]
        if len(info["pd"]) > 1:
            print("DUPKEY %s under primary datasets %s" % (key, sorted(info["pd"])))
            bad.append("dupkey " + key)
        tasks = sorted(info["tasks"])
        if len(tasks) > 1 and key not in a.allow_multi_task:
            print("MULTITASK %s tasks=%s (two tasks can hold the same jobs twice; check, then --allow-multi-task %s)"
                  % (key, ",".join(tasks), key))
            bad.append("multitask " + key)
        files = []
        excluded = 0
        nbytes = 0
        for ts in tasks:
            for job, p, size in sorted(info["tasks"][ts]):
                reason = None
                if size == 0:
                    reason = "zero_bytes"
                elif size < 0:
                    reason = "stat_failed"
                elif os.path.normpath(p) in excl:
                    reason = "listed_in_" + excl[os.path.normpath(p)]
                if reason:
                    print("EXCLUDED %s %s" % (p, reason))
                    excluded += 1
                    continue
                files.append(p)
                nbytes += size
        jobs = collections.Counter()
        for ts in tasks:
            for job, _, _ in info["tasks"][ts]:
                jobs[job] += 1
        dup_jobs = sorted(j for j, c in jobs.items() if c > 1)
        if dup_jobs and key in a.allow_multi_task:
            print("MULTITASK %s job numbers in more than one task: %d (e.g. %s) -- the tasks overlap?"
                  % (key, len(dup_jobs), dup_jobs[:5]))
        n_excl += excluded
        print("SAMPLE %s files=%d excluded=%d GB=%.2f task=%s pd=%s"
              % (key, len(files), excluded, nbytes / 1e9, ",".join(tasks), ",".join(sorted(info["pd"]))))
        if not files:
            bad.append("no file " + key)
        lists[key] = files
        rows.append((key, b, ",".join(sorted(info["pd"])), ",".join(tasks), len(files), excluded, nbytes))
    want = []
    for c in a.forge_config:
        try:
            ck = config_keys(c)
        except OSError as err:
            print("ERROR --forge-config %s: %s" % (c, err))
            return 2
        print("FORGECONFIG %s keys=%d" % (os.path.basename(c), len(ck)))
        want += ck
    if a.forge_config:
        for k in sorted(set(want) - set(found)):
            print("MISSING %s (in the forge config, no files on disk)" % k)
            bad.append("missing " + k)
        for k in sorted(set(found) - set(want)):
            print("EXTRA %s (on disk, in no given forge config)" % k)
        print("CONFIG keys=%d on_disk=%d missing=%d" % (len(set(want)), len(found), len(set(want) - set(found))))
    if not a.check_only:
        os.makedirs(out, exist_ok=True)
        for key, files in lists.items():
            with open(os.path.join(out, "filelist_%s.txt" % key), "w") as f:
                for p in files:
                    f.write(p + "\n")
        with open(os.path.join(out, "MANIFEST.tsv"), "w") as f:
            f.write("key\tbase\tprimary_dataset\ttasks\tfiles\texcluded\tbytes\n")
            for r in rows:
                f.write("\t".join(str(x) for x in r) + "\n")
        print("WROTE %s (%d filelists, MANIFEST.tsv)" % (out, len(lists)))
    print("TOTAL samples=%d files=%d excluded=%d" % (len(lists), sum(len(v) for v in lists.values()), n_excl))
    print("RESULT %s" % ("OK" if not bad else "FAIL (" + "; ".join(bad[:12]) + (" ..." if len(bad) > 12 else "") + ")"))
    return 0 if not bad else 1


if __name__ == "__main__":
    sys.exit(main())
