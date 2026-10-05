#!/usr/bin/env python3
"""kext_xref.py — resolve a string reference inside the collection, without
carving anything.

Why this exists rather than reusing xref_maestro.py: that tool works on a
single extracted Mach-O, but a carved kext in this kernelcache is not a
contiguous byte range (§168), so there is no single file to point it at. What
is available is the pair of coordinates that actually matters: for a string at
collection offset `at`, the owning segment gives the virtual address, and for
every segment with an initprot that allows execution, the (fileoff, vmaddr)
pair gives enough to compute an ADRP's PC-relative target correctly.

So the walk is done directly against the collection:

  string at `at`  ->  segment containing it  ->  va = vmaddr + (at - foff)
  instruction at `off` in an executable segment
                  ->  pc = vmaddr + (off - foff)          <- a VA, not a file offset
                  ->  page = (pc & ~0xfff) + signext(immhi:immlo:'00')
                  ->  match when page + imm12 == va, within a few instructions

Both of those PC mistakes (§167's ROM work, and the earlier kext attempt) are the
kind that produce "this string is never referenced" rather than an error, so the
arithmetic is spelled out rather than factored into a helper that could be
misread.

Alignment: __TEXT_EXEC here starts at file offset 0x242f7 in one kext and at
other unaligned offsets in others, so iteration steps from each segment's own
start rather than from an absolute multiple of 4.
"""
import struct
import sys

LC_SEGMENT_64 = 0x19
LC_FILESET_ENTRY = 0x80000035


def sx(v, bits):
    m = 1 << (bits - 1)
    return (v ^ m) - m


def kext_entry(d, want):
    n = struct.unpack_from("<I", d, 16)[0]
    off = 32
    while True:
        cmd, cs = struct.unpack_from("<II", d, off)
        if cs == 0:
            return None
        if cmd == LC_FILESET_ENTRY:
            name = d[off + 32:off + cs].split(b"\x00")[0].decode(errors="replace")
            if name == want:
                return struct.unpack_from("<Q", d, off + 16)[0]
        off += cs


def segments(d, base):
    ncmds = struct.unpack_from("<I", d, base + 16)[0]
    o = base + 32
    out = []
    for _ in range(ncmds):
        if o + 8 > len(d):
            break
        cmd, cs = struct.unpack_from("<II", d, o)
        if cs == 0:
            break
        if cmd == LC_SEGMENT_64:
            nm = d[o + 8:o + 24].rstrip(b"\x00").decode(errors="replace")
            vmaddr, vmsize, foff, filesize = struct.unpack_from("<QQQQ", d, o + 24)
            out.append((nm, vmaddr, vmsize, foff, filesize))
        o += cs
    return out


def main():
    kc, kext = sys.argv[1], sys.argv[2]
    targets = [int(x, 0) for x in sys.argv[3].split(",")]
    win = int(sys.argv[4]) if len(sys.argv) > 4 else 12
    d = open(kc, "rb").read()
    base = kext_entry(d, kext)
    if base is None:
        print("kext not found:", kext)
        return
    segs = segments(d, base)
    print("kext %s at collection offset 0x%x" % (kext, base))
    for nm, va, vs, fo, fs in segs:
        print("   %-13s va=%#-18x fo=%#-10x size=%#-9x end=%#x"
              % (nm, va, fo, fs, fo + fs))

    want = {}
    for t in targets:
        for nm, va, vs, fo, fs in segs:
            if fo and fs and fo <= t < fo + fs:
                want[t] = va + (t - fo)
                break
        if t not in want:
            print("  target 0x%x not inside any segment of this kext" % t)
    pages = {}
    for t, va in want.items():
        pages.setdefault(va & ~0xFFF, set()).add(va)
    print("\ntargets:")
    for t, va in want.items():
        print("   at 0x%06x -> VA 0x%x  page 0x%x" % (t, va, va & ~0xFFF))

    hits = []
    for nm, va, vs, fo, fs in segs:
        if not fs or nm in ("__TEXT", "__LINKEDIT"):
            continue          # __TEXT holds the strings, not the code
        end = min(len(d), fo + fs)
        for off in range(fo, end - 3, 4):
            w = struct.unpack_from("<I", d, off)[0]
            if (w & 0x9F000000) != 0x90000000:
                continue
            pc = va + (off - fo)
            imm = sx((((w >> 5) & 0x7FFFF) << 2) | ((w >> 29) & 3), 21) << 12
            page = (pc & ~0xFFF) + imm
            if page not in pages:
                continue
            rd = w & 31
            for k in range(1, 7):
                p2 = off + 4 * k
                if p2 + 4 > len(d):
                    break
                w2 = struct.unpack_from("<I", d, p2)[0]
                if (w2 & 0x7F800000) == 0x11000000 and ((w2 >> 5) & 31) == rd:
                    cand = page + ((w2 >> 10) & 0xFFF)
                    for t, tva in want.items():
                        if cand == tva:
                            hits.append((nm, off, p2, t))
                if (w2 & 0x3F000000) == 0x39000000 and ((w2 >> 5) & 31) == rd:
                    cand = page + (((w2 >> 10) & 0xFFF) << (w2 >> 30))
                    for t, tva in want.items():
                        if cand == tva:
                            hits.append((nm, off, p2, t))
    uniq = []
    for h in hits:
        if h not in uniq:
            uniq.append(h)
    print("\n%d reference sites" % len(uniq))
    for segname, off, p2, t in uniq:
        seg = [s for s in segs if s[0] == segname][0]
        print("  string 0x%06x  <- ADRP 0x%06x ADD 0x%06x  in %s"
              % (t, off, p2, segname))


if __name__ == "__main__":
    main()