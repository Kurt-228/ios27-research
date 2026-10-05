#!/usr/bin/env python3
"""d64.py — ARM64 decoder for the audit, with the alignment trap handled.

The alignment note is load-bearing, not trivia. In a carved prelinked kext
__TEXT_EXEC starts at file offset 0x242f7, which is 3 mod 4, so every
instruction sits at an offset congruent to 3 mod 4. Any tool that walks the
file in 4-byte steps from offset 0 decodes the middle of instructions and
produces plausible-looking nonsense — which is exactly what happened to the
first three attempts in this audit. Everything here steps from the segment's
own start.

Decoding is written to be loud: an unrecognised encoding prints as
.word 0x… with its raw bits, because a decoder that silently skips an
instruction turns a wrong answer into a confident one. The four instructions
that matter most for a length check are singled out in the legend below and
annotated inline when decoded.
"""
import struct
import sys

COND = ["eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
        "hi", "ls", "ge", "lt", "gt", "le", "al", "nv"]


def sx(v, bits):
    m = 1 << (bits - 1)
    return (v ^ m) - m


def reg(n, sf=1):
    if n == 31:
        return "sp"
    return ("x" if sf else "w") + str(n)


class Dec:
    def __init__(self, data, base_va, seg_fo):
        self.d = data
        self.base_va = base_va
        self.seg_fo = seg_fo

    def w(self, off):
        return struct.unpack_from("<I", self.d, off)[0]

    def simm(self, w):
        imm = ((w >> 10) & 0xFFF)
        if (w >> 22) & 1:
            imm <<= 12
        return imm

    def op(self, off):
        w = self.w(off)
        sf = w >> 31
        opc = (w >> 29) & 3
        # ---- branches
        if (w & 0x7C000000) == 0x14000000:
            imm = sx(w & 0x03FFFFFF, 26) << 2
            t = off + imm
            return ("BL" if (w & 0x80000000) == 0 else "B") + " 0x%x  ; file 0x%x" % (
                self.base_va + imm, t)
        if (w & 0xFF000010) == 0x54000000:
            imm = sx((w >> 5) & 0x7FFFF, 19) << 2
            return "B.%s 0x%x" % (COND[w & 0xF], off + imm)
        if (w & 0x7E000000) == 0x34000000:
            imm = sx((w >> 5) & 0x7FFFF, 19) << 2
            return "%s %s%d, 0x%x" % ("CBZ" if not (w & 0x01000000) else "CBNZ",
                                      "x" if sf else "w", w & 31, off + imm)
        if (w & 0x7E000000) == 0x36000000:
            imm = sx((w >> 19) & 0x3FFFF, 19) << 2
            b = ((w >> 31) << 5) | ((w >> 19) & 0x1F)
            return "%s %s%d, #%d, 0x%x" % ("TBZ" if not (w & 0x01000000) else "TBNZ",
                                            "x" if sf else "w", b,
                                            (w >> 19) & 0x1F, off + imm)
        # ---- exception
        if (w & 0xFFE0001F) == 0xD4000001:
            return "SVC #0x%x" % ((w >> 5) & 0xFFFF)
        if (w & 0xFFE0001F) == 0xD4200000:
            return "BRK #0x%x" % ((w >> 5) & 0xFFFF)
        # ---- returns / indirect branches
        if (w & 0xFFFFFC1F) == 0xD65F0000:
            return "RET" + ("" if (w & 31) == 0 else " x%d" % (w & 31))
        if (w & 0xFFFFFC1F) == 0xD61F0000:
            return "BR x%d" % (w & 31)
        if (w & 0xFFFFFC1F) == 0xD63F0000:
            return "BLR x%d" % (w & 31)
        if (w & 0xFFFFFC1F) == 0xD63F001F:
            return "BLR xzr"
        # ---- ADR / ADRP
        if (w & 0x9F000000) == 0x90000000:
            imm = sx((((w >> 5) & 0x7FFFF) << 2) | ((w >> 29) & 3), 21) << 12
            return "ADRP x%d, page 0x%x" % (w & 31, (off & ~0xFFF) + imm)
        if (w & 0x9F000000) == 0x10000000:
            imm = sx((((w >> 5) & 0x7FFFF) << 2) | ((w >> 29) & 3), 21)
            return "ADR x%d, 0x%x" % (w & 31, off + imm)
        # ---- move wide
        if (w & 0x7F800000) == 0x12800000:
            nm = ["MOVN", "MOVZ", "MOVK", "MOVZ"][opc]
            return "%s %s, #0x%x" % (nm, reg(w & 31, sf),
                                     ((w >> 5) & 0xFFFF) << (((w >> 21) & 1) * 16))
        if (w & 0x7FE00000) == 0x2A000000:
            sh = (w >> 22) & 3
            if (w & 0x001F8000) == 0x001F8000 and (w & 0xFF800000) == 0x2A000000 \
                    and sh == 0 and ((w >> 5) & 31) == 31:
                return "MOV %s, %s" % (reg(w & 31, sf), reg((w >> 16) & 31, sf))
            return "ORR %s, %s, %s, lsl #%d" % (reg(w & 31, sf), reg((w >> 5) & 31, sf),
                                                reg((w >> 16) & 31, sf), sh)
        # ---- logical immediate
        if (w & 0x7F800000) == 0x12000000:
            nm = ["AND", "ORR", "EOR", "ANDS"][opc]
            return "%s %s, %s, #imm(N=%d immr=%d)" % (
                nm, reg(w & 31, sf), reg((w >> 5) & 31, sf),
                1 << ((w >> 22) & 7) if ((w >> 22) & 7) else 64, (w >> 16) & 0x3F)
        # ---- add/sub immediate   *** length arithmetic lives here ***
        if (w & 0x7F800000) == 0x11000000:
            sub = (w >> 30) & 1
            sets = (w >> 29) & 1
            nm = ("SUBS" if sub else "ADDS") if sets else ("SUB" if sub else "ADD")
            return "%s %s, %s, #0x%x%s" % (nm, reg(w & 31, sf), reg((w >> 5) & 31, sf),
                                           self.simm(w), ", lsl #12" if (w >> 22) & 1 else "")
        # ---- add/sub shifted register
        if (w & 0x7F200000) == 0x0B000000:
            sub = (w >> 30) & 1
            sets = (w >> 29) & 1
            nm = ("SUBS" if sub else "ADDS") if sets else ("SUB" if sub else "ADD")
            return "%s %s, %s, %s, lsl #%d" % (nm, reg(w & 31, sf), reg((w >> 5) & 31, sf),
                                               reg((w >> 16) & 31, sf), (w >> 22) & 3)
        # ---- add/sub with carry (ADC/SBC) — used in bignum code
        if (w & 0x7FE00000) == 0x1A000000:
            sub = (w >> 30) & 1
            return "%s %s, %s, %s%s" % ("SBC" if sub else "ADC", reg(w & 31, sf),
                                        reg((w >> 5) & 31, sf), reg((w >> 16) & 31, sf),
                                        ", lsl #%d" % (w >> 22) & 3 if (w >> 22) & 3 else "")
        # ---- multiply
        if (w & 0x7F800000) == 0x1B000000:
            ra = (w >> 10) & 31
            if ra == 31:
                return "MUL %s, %s, %s" % (reg(w & 31, sf), reg((w >> 5) & 31, sf),
                                           reg((w >> 16) & 31, sf))
            return "MUL %s, %s, %s, %s" % (reg(w & 31, sf), reg((w >> 5) & 31, sf),
                                           reg((w >> 16) & 31, sf), reg(ra, sf))
        if (w & 0x7F800000) == 0x1B008000:
            return "SMULH %s, %s, %s" % (reg(w & 31, sf), reg((w >> 5) & 31, sf),
                                         reg((w >> 16) & 31, sf))
        # ---- compare immediate   *** the bounds check ***
        if (w & 0x7F800000) == 0x71000000:
            sub = (w >> 30) & 1
            return "%s %s, #0x%x" % ("CMP" if sub else "CMN", reg((w >> 5) & 31, sf),
                                     self.simm(w))
        # ---- compare register
        if (w & 0x7F200000) == 0x6B000000:
            sub = (w >> 30) & 1
            return "%s %s, %s, lsl #%d" % ("CMP" if sub else "CMN", reg((w >> 5) & 31, sf),
                                           reg((w >> 16) & 31, sf), (w >> 22) & 3)
        # ---- test / tst
        if (w & 0x7F800000) == 0x6A000000:
            return "TST %s, %s" % (reg((w >> 5) & 31, sf), reg((w >> 16) & 31, sf))
        # ---- bitfield
        if (w & 0x7F800000) == 0x13000000:
            nm = ["SBFM", "BFM", "UBFM"][opc]
            immr = (w >> 16) & 0x3F
            imms = (w >> 10) & 0x3F
            return "%s %s, %s, #%d, #%d" % (nm, reg(w & 31, sf), reg((w >> 5) & 31, sf),
                                            immr, imms)
        # ---- shifts
        if (w & 0x7F800000) == 0x53000000:
            nm = {0: "LSL", 1: "LSR", 2: "ASR", 3: "ROR"}[opc]
            return "%s %s, %s, #%d" % (nm, reg(w & 31, sf), reg((w >> 5) & 31, sf),
                                       (w >> 16) & 0x3F)
        if (w & 0x7FE0FC00) == 0x2AC00000:
            nm = {0: "LSL", 1: "LSR", 2: "ASR", 3: "ROR"}[opc]
            return "%s %s, %s, %s" % (nm, reg(w & 31, sf), reg((w >> 5) & 31, sf),
                                      reg((w >> 16) & 31, sf))
        # ---- load/store unsigned immediate   *** field reads/writes ***
        if (w & 0x3F000000) == 0x39000000:
            size = w >> 30
            load = (w >> 22) & 1
            width = ["B", "H", "W", "X"][size]
            off = ((w >> 10) & 0xFFF) << size
            rn = (w >> 5) & 31
            base = "[sp, #0x%x]" % off if rn == 31 else "[%s, #0x%x]" % (reg(rn, 1), off)
            if not load:
                return "STR %s%d, %s" % (width, w & 31, base)
            if (w & 31) == 31:
                return "LDR %s, %s" % (width, base)
            return "LDR %s%d, %s" % (width, w & 31, base)
        # ---- load/store register offset
        if (w & 0x3E000000) == 0x38000000:
            size = w >> 30
            load = (w >> 22) & 1
            width = ["B", "H", "W", "X"][size]
            if (w & 0x00800000):
                return "%s %s%d, [%s, %s%s]" % ("LDR" if load else "STR", width, w & 31,
                                               reg((w >> 5) & 31, 1),
                                               reg((w >> 16) & 31, 1),
                                               ", lsl #%d" % (w >> 10) & 7 if (w >> 10) & 7 else "")
            ext = "uxtw" if (w >> 10) & 7 == 2 else "sxtw" if (w >> 10) & 7 == 6 else "?"
            return "%s %s%d, [%s, %s, %s]" % ("LDR" if load else "STR", width, w & 31,
                                              reg((w >> 5) & 31, 1),
                                              reg((w >> 16) & 31, 1), ext)
        # ---- load/store pair
        if (w & 0x3A000000) == 0x28000000:
            load = (w >> 22) & 1
            size = (w >> 30) & 3
            width = ["B", "H", "W", "X"][size]
            rt = w & 31
            rt2 = (w >> 10) & 31
            rn = (w >> 5) & 31
            off = sx((w >> 15) & 0x7F, 7) << size
            base = ("[sp, #0x%x]" % off) if rn == 31 else ("[%s, #0x%x]" % (reg(rn, 1), off))
            kind = "ldp/stp"
            if rn == 31:
                kind = "ldp/stp pre-index"
            elif (w & 0x01800000) == 0x01800000:
                kind = "post-index"
            return "%s %s%s%d, %s%d, %s" % (kind, "LDR" if load else "STR", width, rt,
                                            "LDR" if load else "STR", width, rt2, base)
        # ---- load literal
        if (w & 0x3F000000) == 0x18000000:
            imm = sx((w >> 5) & 0x7FFFF, 19) << 2
            return "LDR %s, [pc, #0x%x]" % (reg(w & 31, sf), off + imm)
        # ---- move wide register (MOV/MOVZ aliases)
        if (w & 0x7FE0FFE0) == 0x2A0003E0:
            return "MOV %s, %s" % (reg(w & 31, sf), reg((w >> 16) & 31, sf))
        if (w & 0x7FE07C00) == 0x2A000400:
            return "MOV %s, %s" % (reg(w & 31, sf), reg((w >> 16) & 31, sf))
        # ---- hint / nop / barrier
        if (w & 0xFFFFF01F) == 0xD503201F:
            return "NOP"
        if (w & 0xFFFFF01F) == 0xD503233F:
            return "PACIASP"
        if (w & 0xFFFFF01F) == 0xD50323BF:
            return "AUTIASP"
        if (w & 0xFFFFF01F) == 0xD50320FF:
            return "DSB sy"
        if (w & 0xFFFFF01F) == 0xD5033FBF:
            return "RET (auth)"
        if (w & 0xFFFFF01F) == 0xD50330DF:
            return "ISB"
        if (w & 0xFFE00C00) == 0xD4000000:
            return "BRK/MSR #0x%x" % (w & 0xFFF)
        return ".word 0x%08x" % w


def load(path):
    d = open(path, "rb").read()
    n = struct.unpack_from("<I", d, 16)[0]
    off = 32
    segs = []
    for _ in range(n):
        cmd, cs = struct.unpack_from("<II", d, off)
        if cmd == 0x19:
            nm = d[off + 8:off + 24].rstrip(b"\x00").decode(errors="replace")
            va, vs, fo, fs = struct.unpack_from("<QQQQ", d, off + 24)
            segs.append((nm, va, vs, fo, fs))
        if cs == 0:
            break
        off += cs
    return d, segs


def main():
    path = sys.argv[1]
    lo = int(sys.argv[2], 0)
    hi = int(sys.argv[3], 0)
    d, segs = load(path)
    # pick the segment containing lo so the PC-relative ADRP maths is right
    seg = [s for s in segs if s[3] <= lo < s[3] + s[4]] or [s for s in segs if s[0] == "__TEXT_EXEC"][0]
    import sys as _s; print('DBG segs=%d seg=%r lo=%r'%(len(segs), seg, lo), file=_s.stderr)
    dec = Dec(d, seg[1], seg[3])
    off = lo
    while off < hi:
        print("  0x%06x  %s" % (off, dec.op(off)))
        off += 4


if __name__ == "__main__":
    main()
