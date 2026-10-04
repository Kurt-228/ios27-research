#!/usr/bin/env python3
"""xref_maestro.py — find the code that references a byte offset in a kext.

Why this and not the project's existing relocs.py: §136 and §143 both recorded
that GOT/chained-fixup references to strings do not resolve offline in these
kexts. That is true for *indirect* references through a relocation table. It is
not true for a string address materialised by a PC-relative pair, which is the
usual ARM64 way code reaches a rodata literal and involves no relocator at all.
So this resolves ADRP+ADD/LDR pairs directly and reports them, which is the one
reference mechanism that does not need the fixup machinery to be understood.

The load base does have to be known, and for a carved kext it is recoverable:
every segment's vmaddr is in the LC load commands, and __TEXT gives the base
directly, so fileoffset == vaddr - (vmaddr of __TEXT segment). That is why this
works where the bare ROM did not (§167) — there the mapping base was unknown and
unrecoverable, here it is stated in the header.

For each hit it prints a window of surrounding instructions with BL targets
resolved, because the interesting question is never "is this string referenced"
but "what does the caller do with the number next to it".
"""
import struct
import sys


def segments(path):
    d = open(path, "rb").read()
    ncmds = struct.unpack_from("<I", d, 16)[0]
    off = 32
    segs = []
    for _ in range(ncmds):
        cmd, cs = struct.unpack_from("<II", d, off)
        if cmd == 0x19:
            name = d[off + 8:off + 24].rstrip(b"\x00").decode(errors="replace")
            vmaddr, vmsize, fileoff, filesize = struct.unpack_from("<QQQQ", d, off + 24)
            segs.append((name, vmaddr, vmsize, fileoff, filesize))
        if cs == 0:
            break
        off += cs
    return d, segs


def sx(v, bits):
    m = 1 << (bits - 1)
    return (v ^ m) - m



def _mnem(d, off):
    w = struct.unpack_from("<I", d, off)[0]
    if (w & 0x7C000000) == 0x14000000:
        return ("bl" if (w & 0x80000000) == 0 else "b"), (sx(w & 0x03FFFFFF, 26) << 2)
    if (w & 0xFF000010) == 0x54000000:
        return "b.cond", (sx((w >> 5) & 0x7FFFF, 19) << 2)
    if (w & 0x7E000000) == 0x34000000:
        return ("cbz" if (w & 0x01000000) == 0 else "cbnz"), (sx((w >> 5) & 0x7FFFF, 19) << 2)
    if (w & 0x9F000000) == 0x90000000:
        return "adrp", None
    if (w & 0x7F800000) == 0x12800000:
        return ["movn", "movz", "movk", "movz"][(w >> 29) & 3], \
               ((w >> 5) & 0xFFFF) << (((w >> 21) & 1) * 16)
    if (w & 0x7F800000) == 0x11000000:
        return ("sub" if (w >> 30) & 1 else "add"), (w >> 10) & 0xFFF
    if (w & 0x7F800000) == 0x71000000:
        return ("cmp" if (w >> 30) & 1 else "cmn"), (w >> 10) & 0xFFF
    if (w & 0x7FE0FFE0) == 0x2A0003E0:
        return "mov x%d, x%d" % (w & 31, (w >> 16) & 31), None
    if (w & 0x7F200000) == 0x0B000000:
        return ("sub" if (w >> 30) & 1 else "add x%d, x%d, x%d"), None
    if (w & 0x3F000000) == 0x39000000:
        return ("ldr" if (w >> 22) & 1 else "str") + " %s%d, [x%d, #0x%x]" % (
            ["b", "h", "w", "x"][w >> 30], w & 31, (w >> 5) & 31,
            ((w >> 10) & 0xFFF) << (w >> 30)), None
    if (w & 0xFFFFFC1F) == 0xD65F0000:
        return "ret", None
    if (w & 0x7F800000) == 0x1B000000:
        return ("mul" if ((w >> 10) & 31) == 31 else "madd"), None
    return "0x%08x" % w, None
    w = struct.unpack_from("<I", d, off)[0]
    if (w & 0x7C000000) == 0x14000000:
        imm = sx(w & 0x03FFFFFF, 26) << 2
        t = off + base + imm
        nm = "bl" if (w & 0x80000000) == 0 else "b"
        return "%s 0x%x" % (nm, t - base), t
    if (w & 0xFF000010) == 0x54000000:
        imm = sx((w >> 5) & 0x7FFFF, 19) << 2
        return "b.cond 0x%x" % (off + imm), None
    if (w & 0x9F000000) == 0x90000000:
        imm = sx((((w >> 5) & 0x7FFFF) << 2) | ((w >> 29) & 3), 21) << 12
        return "adrp x%d, 0x%x" % (w & 31, (off & ~0xFFF) + imm), None
    if (w & 0x7F800000) == 0x11000000:
        return "%s x%d, x%d, #0x%x" % ("sub" if (w >> 30) & 1 else "add",
                                       w & 31, (w >> 5) & 31, (w >> 10) & 0xFFF), None
    if (w & 0x3F000000) == 0x39000000:
        size = w >> 30
        load = (w >> 22) & 1
        return "%s %s%d, [x%d, #0x%x]" % (
            "ldr" if load else "str", ["b", "h", "w", "x"][size],
            w & 31, (w >> 5) & 31, ((w >> 10) & 0xFFF) << size), None
    if (w & 0x7F800000) == 0x71000000:
        return "%s x%d, #0x%x" % ("cmp" if (w >> 30) & 1 else "cmn",
                                  (w >> 5) & 31, (w >> 10) & 0xFFF), None
    if (w & 0x7F800000) == 0xD2800000:
        return "movz x%d, #0x%x" % (w & 31, ((w >> 5) & 0xFFFF) << (((w >> 21) & 1) * 16)), None
    if (w & 0xFFFFFC1F) == 0xD65F0000:
        return "ret", None
    return "0x%08x" % w, None


def main():
    path = sys.argv[1]
    targets = [int(x, 0) for x in sys.argv[2].split(",")]
    win = int(sys.argv[3]) if len(sys.argv) > 3 else 40
    d, segs = segments(path)
    text = [s for s in segs if s[0] == "__TEXT"][0]
    base = text[1]
    tro = text[3]
    tsz = min(text[2], text[4]) if text[4] else text[2]
    # Scan every segment that carries file bytes, not just __TEXT. In this
    # collection the executable code sits in __TEXT_EXEC and __TEXT is mostly
    # constants — scanning only __TEXT found nothing, which read as "this string
    # is never referenced" rather than "I looked in the wrong segment".
    scan_ranges = [(fo, fs) for nm, va, vs, fo, fs in segs
                   if fs and nm != "__LINKEDIT"]
    print("__TEXT vmaddr=%#x fileoff=%#x  => fileoff == vaddr - %#x" % (base, tro, base))
    for name, va, vs, fo, fs in segs:
        print("   %-14s va=%#-14x fo=%#-10x size=%#x" % (name, va, fo, fs))

    # A file offset becomes a VA via the *image* base, not the __TEXT segment's
    # own fileoff. Getting this wrong silently yields zero hits, which looks
    # exactly like "the string is never referenced" — so it is worth being
    # explicit: va = vmaddr(__TEXT) + (fileoff - fileoff(__TEXT)).
    want_pages = {}
    for t in targets:
        va = base + (t - tro)
        want_pages.setdefault(va & ~0xFFF, []).append((t, va))

    hits = []
    # Step 4 bytes *from each segment's own start*. __TEXT_EXEC begins at file
    # offset 0x242f7, which is not a multiple of 4, so every instruction in it
    # sits at an offset congruent to 3 mod 4. An absolute `off & 3` alignment
    # guard therefore skipped the entire executable segment and reported
    # "this string is never referenced" — five consecutive runs agreed on that
    # wrong answer, which is exactly the failure mode this project keeps
    # hitting: a tool that is confidently wrong is more dangerous than one that
    # is broken, because zero hits reads like a real measurement.
    for _nm, _va, _vs, _fo, _fs in segs:
        if _nm == "__LINKEDIT" or not _fs:
            continue
        _end = min(len(d), _fo + _fs)
        for off in range(_fo, _end - 3, 4):
            w = struct.unpack_from("<I", d, off)[0]
            if (w & 0x9F000000) != 0x90000000:
                continue
            pc = _va + (off - _fo)          # ADRP's PC is the instruction's VA
            imm = sx((((w >> 5) & 0x7FFFF) << 2) | ((w >> 29) & 3), 21) << 12
            page = (pc & ~0xFFF) + imm
            if page not in want_pages:
                continue
            rd = w & 31
            for k in range(1, 7):
                p2 = off + 4 * k
                if p2 + 4 > len(d):
                    break
                w2 = struct.unpack_from("<I", d, p2)[0]
                if (w2 & 0x7F800000) == 0x11000000 and ((w2 >> 5) & 31) == rd:
                    cand = page + ((w2 >> 10) & 0xFFF)
                    for t, tva in want_pages[page]:
                        if cand == tva:
                            hits.append((off, p2, t))
                if (w2 & 0x3F000000) == 0x39000000 and ((w2 >> 5) & 31) == rd:
                    cand = page + (((w2 >> 10) & 0xFFF) << (w2 >> 30))
                    for t, tva in want_pages[page]:
                        if cand == tva:
                            hits.append((off, p2, t))
                if (w2 & 0x7F200000) == 0x0B000000 and ((w2 >> 5) & 31) == rd:
                    cand = page + (((w2 >> 16) & 31) << ((w2 >> 22) & 3))
                    for t, tva in want_pages[page]:
                        if cand == tva:
                            hits.append((off, p2, t))
    uniq = []
    for h in hits:
        if h not in uniq:
            uniq.append(h)
    print("\n%d reference sites" % len(uniq))
    for off, p2, t in uniq:
        seg = [s for s in segs if s[3] <= off < s[3] + s[4]][0]
        s = max(seg[3], off - win * 4)
        e = min(seg[3] + seg[4], off + win * 4)
        print("\n=== string file 0x%06x referenced at 0x%06x ===" % (t, off))
        o = s
        while o < e:
            nm, arg = _mnem(d, o)
            txt = nm if arg is None else "%s 0x%x" % (nm, arg)
            if nm in ("bl", "b", "b.cond", "cbz", "cbnz", "tbz", "tbnz") and arg is not None:
                tgt_va = seg[1] + (o - seg[3]) + arg
                txt += "   ; -> file 0x%x" % (seg[3] + (tgt_va - seg[1]))
            print("  %s 0x%06x  %s" % (">>" if o == off else ("**" if o == p2 else "  "), o, txt))
            o += 4
    return


if __name__ == "__main__":
    main()
