#!/usr/bin/env python3
"""kext_strings.py — collect exactly one kext's strings, from its own segments.

This replaces the carve-then-scan approach entirely, and the reason it works
where the carve failed is the thing the carve got wrong.

In this kernelcache the fileset entry for a kext points at its __TEXT and
nothing else. Verified directly: IOAccessoryManager is at file 0x86dfb0 and the
next fileset entry is at 0x891bd0 — a delta of exactly 0x23c20, which is that
kext's __TEXT filesize (0x23c1f) rounded to alignment. So the entries enumerate
the __TEXT fsegment only. The remaining segments are scattered elsewhere in the
collection:

    __TEXT        foff=0x86dfb0     <- the fileset entry
    __TEXT_EXEC   foff=0x2bcdab0
    __DATA        foff=0x4493458
    __DATA_CONST  foff=0x10f8f90
    __LINKEDIT    foff=0x4530000

Which means "the kext" is not a contiguous byte range, and every attempt to
compute one fails silently in a different direction:

  - next fileset entry  ->  146 KB, truncates before __TEXT_EXEC even starts
  - max(foff+filesize)  ->  64 MB, because __LINKEDIT belongs to another
                           fsegment region and its end is nowhere near this kext

The fix is to stop wanting an image. For a string audit all that matters is the
set of ranges this kext owns, and those are exactly its segments. Strings are
therefore harvested per segment and attributed to the kext that owns them, which
is both correct and faster than writing 265 blobs to disk.

Each kext is reported with its own segment list so a reviewer can check the
attribution, and the class name of every string is preserved so
length_audit.py can classify it.
"""
import os
import re
import struct
import sys

LC_SEGMENT_64 = 0x19
LC_FILESET_ENTRY = 0x80000035


def entries(d):
    n = struct.unpack_from("<I", d, 16)[0]
    off = 32
    out = []
    for _ in range(n):
        cmd, cs = struct.unpack_from("<II", d, off)
        if cs == 0:
            break
        if cmd == LC_FILESET_ENTRY:
            foff = struct.unpack_from("<Q", d, off + 16)[0]
            name = d[off + 32:off + cs].split(b"\x00")[0].decode(errors="replace")
            if name:
                out.append((foff, name))
        off += cs
    return out


def segments(d, base):
    """(name, foff, filesize) for every segment of the kext at `base`."""
    try:
        if d[base:base + 4] != b"\xcf\xfa\xed\xfe":
            return []
        ncmds = struct.unpack_from("<I", d, base + 16)[0]
    except Exception:
        return []
    o = base + 32
    segs = []
    for _ in range(ncmds):
        if o + 8 > len(d):
            break
        cmd, cs = struct.unpack_from("<II", d, o)
        if cs == 0:
            break
        if cmd == LC_SEGMENT_64:
            nm = d[o + 8:o + 24].rstrip(b"\x00").decode(errors="replace")
            foff = struct.unpack_from("<Q", d, o + 40)[0]
            filesize = struct.unpack_from("<Q", d, o + 48)[0]
            segs.append((nm, foff, filesize))
        o += cs
    return segs


def strings_in(d, foff, filesize, minlen=10):
    if not foff or not filesize or foff + filesize > len(d):
        return []
    blob = d[foff:foff + filesize]
    return [(foff + m.start(), m.group().decode(errors="replace"))
            for m in re.finditer(rb"[ -~]{%d,}" % minlen, blob)]


def main():
    kc = sys.argv[1]
    out = sys.argv[2]
    d = open(kc, "rb").read()
    os.makedirs(out, exist_ok=True)
    total = 0
    with open(os.path.join(out, "kext_strings.tsv"), "w") as f:
        for foff, name in entries(d):
            segs = segments(d, foff)
            if not segs:
                continue
            n = 0
            for sname, so, ss in segs:
                for at, s in strings_in(d, so, ss):
                    f.write("%s\t%s\t0x%x\t%s\n" % (name, sname, at, s))
                    n += 1
            total += n
            if n:
                print("%-52s %5d strings  segs=%s"
                      % (name[:52], n, ",".join(s[0] for s in segs)))
    print("\n%d strings total -> %s" % (total, os.path.join(out, "kext_strings.tsv")))


if __name__ == "__main__":
    main()