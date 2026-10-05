#!/usr/bin/env python3
"""variables_from_header.py -- the branch records of an existing eventBuffer.h, in treestream's variables.txt
format (STEP 24; D-2026-10-05-D)

    python3 tools/stage1/variables_from_header.py <eventBuffer.h> [-o FILE] [--label TEXT]

Why: the 2017 v20 ntuples (ttHH2017UL_fullNano_v20) were removed from KNU (user, 2026-10-05; 2017 comes back
as a v15 production, D-2026-10-02-A), so the branch union for the new header cannot read a 2017 v9 file. The
analyzer's 2017/2018 (NanoAOD v9) code still has to compile and read v9 files the way it did, and the header
of 2026-06-29 (mkanalyzer.py v2.0.3, made from those 2017 v20 files) holds exactly that branch set. This tool
writes it back as records, which tools/stage1/eventbuffer_manifest.py --extra-variables merges into the union
of the files it reads (the same widest-type and per-counter maxcount rules as treestream's mkvariables.py
--merge).

Per branch the header selects (input->select("Events/<b>", <var>)):
  type     the C++ type of <var> (std::vector<T> -> T): bool, float, double, int, unsigned int -> uint,
           long / long long -> long64, unsigned long / unsigned long long -> ulong64, unsigned char -> uchar,
           char -> char, short -> short, unsigned short -> ushort; another type is an error (exit 1)
  maxcount the N of <var>.resize(N) in the select block (1 for a scalar)
  counter  from output->add("Events/<b>[<counter>]", <var>) (none for a scalar)
Output: 'Tree Events', a provenance comment (source md5, generator line, record count), one record per line
'<type>/Events/<b>/<b>/<maxcount> <counter>' in the header's select order. Exit: 0 ok; 1 a branch could not
be described (no declaration, unknown type, a vector without resize or counter); 2 bad arguments.
"""
from __future__ import print_function

import argparse
import hashlib
import re
import sys

TYPES = {"bool": "bool", "float": "float", "double": "double", "int": "int", "unsigned int": "uint",
         "long": "long64", "long long": "long64", "unsigned long": "ulong64", "unsigned long long": "ulong64",
         "unsigned char": "uchar", "char": "char", "short": "short", "unsigned short": "ushort",
         "Long64_t": "long64", "ULong64_t": "ulong64", "Int_t": "int", "UInt_t": "uint", "Float_t": "float",
         "Double_t": "double", "Bool_t": "bool", "UChar_t": "uchar", "Char_t": "char", "Short_t": "short",
         "UShort_t": "ushort"}
DECL_RE = re.compile(r"^  (std::vector<\s*([^>]+?)\s*>|[A-Za-z_][A-Za-z0-9_ ]*?)\t([A-Za-z_][A-Za-z0-9_]*);\s*$")
SELECT_RE = re.compile(r'input->select\("Events/([A-Za-z0-9_]+)",\s*([A-Za-z0-9_]+)\)')
RESIZE_RE = re.compile(r'([A-Za-z0-9_]+)\.resize\((\d+)\);\s*input->select\("Events/([A-Za-z0-9_]+)"')
ADD_RE = re.compile(r'output->add\("Events/([A-Za-z0-9_]+)(?:\[([A-Za-z0-9_]+)\])?",\s*([A-Za-z0-9_]+)\)')


def records(text):
    """[(type, branch, maxcount, counter)] in select order, and a list of problems"""
    decl = {}
    for line in text.splitlines():
        m = DECL_RE.match(line)
        if m:
            vec = m.group(2) is not None
            decl[m.group(3)] = (m.group(2).strip() if vec else m.group(1).strip(), vec)
    resize = {m.group(3): int(m.group(2)) for m in RESIZE_RE.finditer(text)}
    counter = {}
    for m in ADD_RE.finditer(text):
        counter[m.group(1)] = m.group(2) or ""
    out, problems, seen = [], [], set()
    for m in SELECT_RE.finditer(text):
        b, var = m.group(1), m.group(2)
        if b in seen:
            continue
        seen.add(b)
        if var not in decl:
            problems.append("%s: no declaration of %s" % (b, var))
            continue
        ctype, vec = decl[var]
        t = TYPES.get(ctype)
        if t is None:
            problems.append("%s: unknown C++ type '%s'" % (b, ctype))
            continue
        if vec:
            if b not in resize or not counter.get(b):
                problems.append("%s: a vector without resize(N) or a counter in output->add" % b)
                continue
            out.append((t, b, resize[b], counter[b]))
        else:
            out.append((t, b, 1, ""))
    return out, problems


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("header")
    ap.add_argument("-o", "--output", help="default: stdout")
    ap.add_argument("--label", default="", help="a provenance note for the comment line")
    a = ap.parse_args(argv)
    try:
        raw = open(a.header, "rb").read()
    except OSError as e:
        print("ERROR %s" % e, file=sys.stderr)
        return 2
    text = raw.decode("utf-8", "replace")
    gen = next((l.strip() for l in text.splitlines() if l.startswith("// Created:")), "// Created: ?")
    recs, problems = records(text)
    for p in problems:
        print("PROBLEM %s" % p, file=sys.stderr)
    lines = ["Tree Events", "",
             "# tempTTHH tools/stage1/variables_from_header.py (STEP 24): the branch records of %s (md5 %s; %s)%s"
             % (a.header, hashlib.md5(raw).hexdigest(), gen.lstrip("/ ").strip(), ("; " + a.label) if a.label else ""),
             "# records=%d (vectors %d, scalars %d)" % (len(recs), sum(1 for r in recs if r[3]), sum(1 for r in recs if not r[3]))]
    lines += ["%s/Events/%s/%s/%d %s" % (t, b, b, n, c) for t, b, n, c in recs]
    out = "\n".join(lines) + "\n"
    if a.output:
        with open(a.output, "w") as f:
            f.write(out)
        print("WROTE %s records=%d problems=%d md5=%s" % (a.output, len(recs), len(problems),
                                                           hashlib.md5(out.encode()).hexdigest()), file=sys.stderr)
    else:
        sys.stdout.write(out)
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
