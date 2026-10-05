#!/usr/bin/env python3
"""eventbuffer_from_record.py -- include/eventBuffer.h from a KNU manifest record (STEP 24; PLAN 9.6 D16)

The KNU job tools/stage1/eventbuffer_manifest.py writes the branch union (variables.txt, HLT cut to
tools/stage1/hlt_keep.txt) into its runlog. This rebuilds the header from that record with the same
treestream fork, checks it against the record, stamps it, and writes it into the repository:

    python3 tools/stage1/eventbuffer_from_record.py --record runlogs/run_knu_eventbuffer_manifest_<UTC>.log
            --treestream <fork checkout, same commit as the record> [--repo .] [--check-only]

1. the 'BEGIN VARIABLES <md5>' ... 'END VARIABLES' block of the record, md5 checked
2. the record must end in 'RESULT OK' (else exit 2); its TREESTREAM commit must be the commit of
   --treestream, and the fork must have no local changes in bin/, src/treestream.cc, include/treestream.h
   (else exit 1: the header or the copied files could differ from what the stamp names)
3. the fork's mkeventbuffer.py on that text; the new header without its 'Created:'/'Author:' lines
   must have the record's HEADER md5_normalized (else exit 1)
4. the 'Created:' and 'Author:' lines are replaced by fixed provenance lines, and after
   '#define EVENTBUFFER_H' comes
       #define TTHH_EVENTBUFFER_STAMP "treestream <commit>; variables md5 <md5>; record <file>"
   which the analyzer prints at start (so a job log says which header it was built with)
5. writes <repo>/include/eventBuffer.h and <repo>/include/eventBuffer_variables.txt, and copies the
   fork's src/treestream.cc and include/treestream.h to <repo>/src/ and <repo>/include/ (D16: the
   read-error stop, A14, lives in treestream.cc)
--check-only does 1-3 and writes nothing. Exit: 0 ok; 1 a check failed; 2 bad arguments or record.
"""
from __future__ import print_function

import argparse
import hashlib
import os
import re
import shutil
import subprocess
import sys
import tempfile

NORMALIZE_RE = re.compile(r"^// (Created|Author):")


def md5_bytes(b):
    return hashlib.md5(b).hexdigest()


def read_record(path):
    """Returns (variables text, md5 in the BEGIN line, treestream commit, header md5) from a record."""
    with open(path, encoding="utf-8", errors="replace") as f:
        lines = f.read().splitlines(True)
    begin = [i for i, l in enumerate(lines) if l.startswith("BEGIN VARIABLES ")]
    end = [i for i, l in enumerate(lines) if l.rstrip("\n") == "END VARIABLES"]
    if len(begin) != 1 or len(end) != 1 or end[0] < begin[0]:
        raise ValueError("record needs exactly one BEGIN VARIABLES ... END VARIABLES block (found %d/%d)"
                         % (len(begin), len(end)))
    md5 = lines[begin[0]].split()[2]
    text = "".join(lines[begin[0] + 1:end[0]])
    commit = None
    hmd5 = None
    for l in lines:
        m = re.match(r"^TREESTREAM \S+ @ (\S+)", l)
        if m:
            commit = m.group(1)
        m = re.match(r"^HEADER rc=0 lines=\d+ md5_normalized=([0-9a-f]{32})", l)
        if m:
            hmd5 = m.group(1)
    if not commit or not hmd5:
        raise ValueError("record has no TREESTREAM or HEADER md5_normalized line")
    if not any(l.rstrip("\n") == "RESULT OK" for l in lines):
        raise ValueError("the record does not end in 'RESULT OK' (a failed manifest run: see its RESULT line)")
    return text, md5, commit, hmd5


def fork_commit(ts):
    try:
        return subprocess.run(["git", "-C", ts, "log", "-1", "--format=%h"], stdout=subprocess.PIPE,
                              stderr=subprocess.DEVNULL, universal_newlines=True).stdout.strip()
    except OSError:
        return ""


def fork_local_changes(ts):
    """Uncommitted changes in the files this tool uses or copies ('' when clean)."""
    try:
        return subprocess.run(["git", "-C", ts, "status", "--short", "--", "bin", "src/treestream.cc",
                               "include/treestream.h"], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
                              universal_newlines=True).stdout.strip()
    except OSError:
        return "?"


def generate(ts, text, workdir):
    var = os.path.join(workdir, "variables.txt")
    with open(var, "w") as f:
        f.write(text)
    env = dict(os.environ)
    env["TREESTREAM_PATH"] = ts
    hdr = os.path.join(workdir, "eventBuffer.h")
    p = subprocess.run([sys.executable, os.path.join(ts, "bin", "mkeventbuffer.py"), var, "-o", hdr],
                       cwd=workdir, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                       universal_newlines=True)
    if p.returncode != 0 or not os.path.isfile(hdr):
        raise RuntimeError("mkeventbuffer.py failed (exit %d):\n%s" % (p.returncode, p.stdout[-2000:]))
    with open(hdr, encoding="utf-8") as f:
        return f.read()


def normalized(header_text):
    return "".join(l for l in header_text.splitlines(True) if not NORMALIZE_RE.match(l))


def stamp(header_text, commit, vmd5, record_name):
    out = []
    done = False
    for l in header_text.splitlines(True):
        if l.startswith("// Created:"):
            l = "// Created:     by mkanalyzer.py (treestream %s) from include/eventBuffer_variables.txt\n" % commit
        elif l.startswith("// Author:"):
            l = "// Author:      generated: tools/stage1/eventbuffer_from_record.py, record %s\n" % record_name
        out.append(l)
        if not done and l.startswith("#define EVENTBUFFER_H"):
            out.append("// [STEP 24] provenance: printed by the analyzer at start (ttHHanalyzer_unified.cc main)\n")
            out.append('#define TTHH_EVENTBUFFER_STAMP "treestream %s; variables md5 %s; record %s"\n'
                       % (commit, vmd5, record_name))
            done = True
    if not done:
        raise ValueError("no '#define EVENTBUFFER_H' line in the generated header")
    return "".join(out)


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--record", required=True)
    ap.add_argument("--treestream", required=True)
    ap.add_argument("--repo", default=os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
    ap.add_argument("--check-only", action="store_true")
    a = ap.parse_args(argv)
    ts = os.path.abspath(a.treestream)
    if not os.path.isfile(os.path.join(ts, "bin", "mkeventbuffer.py")):
        print("ERROR not a treestream checkout: %s" % ts)
        return 2
    try:
        text, vmd5_rec, commit_rec, hmd5_rec = read_record(a.record)
    except (OSError, ValueError) as e:
        print("ERROR record: %s" % e)
        return 2
    vmd5 = md5_bytes(text.encode("utf-8"))
    print("RECORD %s" % a.record)
    print("VARIABLES md5=%s (record %s) records=%d" % (vmd5, vmd5_rec,
          sum(1 for l in text.splitlines() if l and not l.startswith("#") and "/" in l)))
    bad = []
    if vmd5 != vmd5_rec:
        bad.append("variables md5 differs from the record (cut or edited block?)")
    commit = fork_commit(ts)
    print("TREESTREAM %s @ %s (record @ %s)" % (ts, commit or "?", commit_rec))
    if not commit or not commit_rec.startswith(commit) and not commit.startswith(commit_rec):
        bad.append("treestream commit %s is not the record's %s" % (commit or "?", commit_rec))
    dirty = fork_local_changes(ts)
    if dirty:
        bad.append("the fork has local changes in bin/ or treestream.cc/.h (%s): the stamp would name a clean commit"
                   % " ".join(dirty.split()))
    if bad:
        print("RESULT FAIL (" + "; ".join(bad) + ")")
        return 1
    work = tempfile.mkdtemp(prefix="ebrec_")
    try:
        hdr = generate(ts, text, work)
    except RuntimeError as e:
        print("ERROR %s" % e)
        return 1
    hmd5 = md5_bytes(normalized(hdr).encode("utf-8"))
    print("HEADER lines=%d md5_normalized=%s (record %s)" % (hdr.count("\n"), hmd5, hmd5_rec))
    if hmd5 != hmd5_rec:
        print("RESULT FAIL (the header from the record differs from the one the KNU job generated)")
        return 1
    if a.check_only:
        print("RESULT OK (check only, nothing written)")
        return 0
    record_name = os.path.basename(a.record)
    out = stamp(hdr, commit_rec, vmd5, record_name)
    inc = os.path.join(a.repo, "include")
    src = os.path.join(a.repo, "src")
    with open(os.path.join(inc, "eventBuffer.h"), "w") as f:
        f.write(out)
    with open(os.path.join(inc, "eventBuffer_variables.txt"), "w") as f:
        f.write(text)
    shutil.copyfile(os.path.join(ts, "src", "treestream.cc"), os.path.join(src, "treestream.cc"))
    shutil.copyfile(os.path.join(ts, "include", "treestream.h"), os.path.join(inc, "treestream.h"))
    for p in ("include/eventBuffer.h", "include/eventBuffer_variables.txt", "src/treestream.cc", "include/treestream.h"):
        with open(os.path.join(a.repo, p), "rb") as f:
            print("WROTE %s md5=%s" % (p, md5_bytes(f.read())))
    print("RESULT OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())
