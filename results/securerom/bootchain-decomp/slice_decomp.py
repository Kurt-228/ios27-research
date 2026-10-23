#!/usr/bin/env python3
"""slice_decomp.py — pull functions out of a DumpDecomp dump by entry range.

Usage: slice_decomp.py <dump.c> <start_hex> <end_hex> [strings.tsv]

Prints selected functions with string constants annotated (when strings.tsv
is given: any 0xNNNN literal equal to a known string offset is suffixed with
`/* STR: ... */`; annotation is a hint, not a claim about dereference).
"""
import re
import sys

dump = sys.argv[1]
lo = int(sys.argv[2], 16)
hi = int(sys.argv[3], 16)
strmap = {}
if len(sys.argv) > 4:
    for line in open(sys.argv[4]):
        parts = line.rstrip("\n").split("\t")
        if len(parts) >= 3:
            strmap[int(parts[0], 16)] = parts[2]

pat = re.compile(r"0x([0-9a-fA-F]+)\b")
out = []
take = False
for line in open(dump, errors="replace"):
    if line.startswith("=== FUNC "):
        m = re.match(r"=== FUNC ([0-9a-f]+) sz=(\d+)", line)
        addr = int(m.group(1), 16)
        take = lo <= addr < hi
        if take:
            out.append("\n" + line.rstrip())
        continue
    if take and strmap:
        def sub(m):
            v = int(m.group(1), 16)
            s = strmap.get(v)
            if s is not None and 0x200 <= v <= 0x80000:
                return "0x%x /* %s */" % (v, s.replace("*/", "* /"))
            return m.group(0)
        line = pat.sub(sub, line)
    if take:
        out.append(line.rstrip("\n"))
print("\n".join(out))
