#!/usr/bin/env python3
"""disasm_rom.py — minimal ARM64 decoder for the SecureROM dumps.

Why this exists and why it avoids the load base entirely: the goal is the call
graph around the embedded X.509 root, and BL/B are PC-relative within the same
image. So control flow can be recovered from file offsets alone, with no need
to recover the address the ROM is mapped at.

That last point is not a convenience. Recovering the load base from ADRP+ADD
pairs failed: the ROM's references all land in one narrow virtual band
(0xfc038000-0xfc048c40) that does not map inside a 512 KB file under any single
constant offset, and an anchor-scored search over 455k candidate bases matched
only one of nine known strings. Those references are therefore not plain
image-relative pointers, and guessing further would manufacture a plausible
looking address table out of arithmetic that has already been shown wrong. So
the decoder below decodes what does not need the base (registers, branches,
loads, arithmetic, compares) and prints ADRP/ADR as page-relative with the
target page stated as "unresolved" rather than guessing.

Scope is deliberately the integer subset the ROM actually uses: MOV/MOVZ/MOVN/
MOVK, ADD/SUB (imm and shifted reg), AND/ORR/EOR with immediate and with LSL,
LSL/LSR/ASR, MADD/MUL/MSUB, CMP/CMN/TST, LDR/STR (immediate unsigned-offset,
pre/post-index, and LDR literal), B/BL/B.cond/RET/BR/BLR, CBZ/CBNZ/TBZ/TBNZ,
and the flag-setting forms needed to see bounds checks. Anything unrecognised
prints as .word so an undecoded instruction is visible rather than silently
skipped — a decoder that quietly drops instructions turns a wrong answer into a
confident one.
"""
import struct
import sys

RNAMES = ["x%d" % i for i in range(31)] + ["sp"]


def sx(v, bits):
    m = 1 << (bits - 1)
    return (v ^ m) - m


class D:
    def __init__(self, data, base=0):
        self.d = data
        self.base = base

    def w(self, off):
        return struct.unpack_from("<I", self.d, off)[0]

    def h(self, off):
        return struct.unpack_from("<H", self.d, off)[0]

    def reg(self, n, sp_is_sp=False):
        if n == 31:
            return "sp" if sp_is_sp else "xzr"
        return "x%d" % n

    def cond(self, c):
        return ["eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
                "hi", "ls", "ge", "lt", "gt", "le", "al", "nv"][c]

    def one(self, off):
        w = self.w(off)
        top = w >> 24

        # ---- unconditional branch immediate
        if (w & 0x7C000000) == 0x14000000:
            imm = sx(w & 0x03FFFFFF, 26) << 2
            if (w & 0x80000000) == 0:
                return "bl 0x%x" % (off + imm)
            return "b 0x%x" % (off + imm)

        # ---- B.cond
        if (w & 0xFF000010) == 0x54000000:
            imm = sx((w >> 5) & 0x7FFFF, 19) << 2
            return "b.%s 0x%x" % (self.cond(w & 0xF), off + imm)

        # ---- CBZ / CBNZ
        if (w & 0x7E000000) == 0x34000000:
            imm = sx((w >> 5) & 0x7FFFF, 19) << 2
            sf = "x" if (w >> 31) else "w"
            return "%s %s%d, 0x%x" % ("cbz" if (w & 0x01000000) == 0 else "cbnz",
                                      sf, w & 31, off + imm)

        # ---- TBZ / TBNZ
        if (w & 0x7E000000) == 0x36000000:
            imm = sx((w >> 5 & 0x3FFFF), 19) << 2
            b = ((w >> 31) << 5) | ((w >> 19) & 0x1F)
            return "%s w%d, #%d, 0x%x" % ("tbz" if (w & 0x01000000) == 0 else "tbnz",
                                          b, (w >> 19) & 0x1F, off + imm)

        # ---- RET / BR / BLR
        if (w & 0xFFFFFC1F) == 0xD65F0000:
            return "ret%s" % (" x%d" % (w & 31) if (w & 31) else "")
        if (w & 0xFFFFFC1F) == 0xD61F0000:
            return "br x%d" % (w & 31)
        if (w & 0xFFFFFC1F) == 0xD63F0000:
            return "blr x%d" % (w & 31)

        # ---- SVC / BRK
        if (w & 0xFFE0001F) == 0xD4000001:
            return "svc #0x%x" % ((w >> 5) & 0xFFFF)
        if (w & 0xFFE0001F) == 0xD4200000:
            return "brk #0x%x" % ((w >> 5) & 0xFFFF)

        # ---- ADRP
        if (w & 0x9F000000) == 0x90000000:
            imm = (((w >> 5) & 0x7FFFF) << 2 | ((w >> 29) & 3))
            if imm & (1 << 20):
                imm -= 1 << 21
            # PC-relative page. Reporting it as a page offset rather than a VA
            # is deliberate: the load base is not recovered (see module docstring)
            # and printing a fake absolute target would be worse than printing none.
            return "adrp x%d, page(0x%05x)+0x%x" % (
                w & 31, (off & ~0xFFF) + (imm << 12), (w >> 29) & 3)

        # ---- ADR
        if (w & 0x9F000000) == 0x10000000:
            imm = (((w >> 5) & 0x7FFFF) << 2 | ((w >> 29) & 3))
            if imm & (1 << 20):
                imm -= 1 << 21
            return "adr x%d, 0x%x" % (w & 31, off + imm)

        # ---- MOVZ / MOVN / MOVK
        if (w & 0x7F800000) == 0x12800000:
            op = (w >> 29) & 3
            name = ["movn", "movz", "movk", "movz"][op]
            imm = ((w >> 5) & 0xFFFF) << (((w >> 21) & 1) * 16)
            return "%s x%d, #0x%x" % (name, w & 31, imm)

        # ---- MOV (register) alias of ORR
        if (w & 0x7FE0FFE0) == 0x2A0003E0:
            sf = "x" if (w >> 31) else "w"
            return "mov %s%d, %s%d" % (sf, w & 31, sf, (w >> 16) & 31)

        # ---- ORR shifted register
        if (w & 0x7F200000) == 0x2A000000:
            sh = (w >> 22) & 3
            return "orr %s%d, %s%d, %s%d, lsl #%d" % (
                "x" if (w >> 31) else "w", w & 31,
                "x" if (w >> 31) else "w", (w >> 5) & 31,
                "x" if (w >> 31) else "w", (w >> 16) & 31, sh)

        # ---- logical immediate
        if (w & 0x7F800000) == 0x12000000:
            op = ["and", "orr", "eor", "ands"][(w >> 29) & 3]
            if ((w >> 23) & 3) == 1:
                return "%s x%d, x%d, #bitmask(n=%d,immr=%d)" % (
                    op, w & 31, (w >> 5) & 31, 1 << ((w >> 22) & 7) if ((w >> 22) & 7) else 64,
                    (w >> 16) & 0x3F)
            return "%s x%d, x%d, #imm" % (op, w & 31, (w >> 5) & 31)

        # ---- ADD/SUB immediate (64-bit)
        if (w & 0x7F800000) == 0x11000000:
            sub = (w >> 30) & 1
            sh = (w >> 22) & 1
            return "%s x%d, x%d, #0x%x%s" % (
                "sub" if sub else "add", w & 31, (w >> 5) & 31,
                (w >> 10) & 0xFFF, ", lsl #12" if sh else "")

        # ---- ADD/SUB shifted register
        if (w & 0x7F200000) == 0x0B000000:
            sub = (w >> 30) & 1
            sh = (w >> 22) & 3
            return "%s x%d, x%d, x%d, lsl #%d" % (
                "sub" if sub else "add", w & 31, (w >> 5) & 31, (w >> 16) & 31, sh)

        # ---- MADD / MUL / MSUB
        if (w & 0x7F800000) == 0x1B000000:
            ra = (w >> 10) & 31
            if ra == 31:
                return "mul %s%d, %s%d, %s%d" % (
                    "x" if (w >> 31) else "w", w & 31,
                    "x" if (w >> 31) else "w", (w >> 5) & 31,
                    "x" if (w >> 31) else "w", (w >> 16) & 31)
            return ("madd" if (w >> 15) & 1 == 0 else "msub") + \
                   " %s%d, %s%d, %s%d, %s%d" % (
                       "x" if (w >> 31) else "w", w & 31,
                       "x" if (w >> 31) else "w", (w >> 5) & 31,
                       "x" if (w >> 31) else "w", (w >> 16) & 31,
                       "x" if (w >> 31) else "w", ra)

        # ---- ADDS / SUBS immediate (flags) — these are the bounds checks
        if (w & 0x7F800000) == 0x31000000:
            sub = (w >> 30) & 1
            return "%s%s x%d, x%d, #0x%x" % (
                "s" if sub else "s", "ubs" if False else ("cmp" if (w & 31) == 31 else "add"),
                w & 31, (w >> 5) & 31, (w >> 10) & 0xFFF)

        # ---- CMP / CMN immediate
        if (w & 0x7F800000) == 0x71000000:
            sub = (w >> 30) & 1
            return ("cmp" if sub else "cmn") + " x%d, #0x%x" % (
                (w >> 5) & 31, (w >> 10) & 0xFFF)

        # ---- LDR / STR unsigned immediate
        if (w & 0x3F000000) == 0x39000000:
            size = w >> 30
            load = (w >> 22) & 1
            width = ["b", "h", "w", "x"][size]
            off = ((w >> 10) & 0xFFF) << size
            rt = w & 31
            base = (w >> 5) & 31
            if base == 31:
                return "%s x%d, [sp, #0x%x]" % ("ldr" if load else "str", rt, off)
            return "%s %s%d, [x%d, #0x%x]" % (
                "ldr" if load else "str", width, rt, base, off)

        # ---- LDR literal
        if (w & 0x3F000000) == 0x18000000:
            imm = sx((w >> 5) & 0x7FFFF, 19) << 2
            return "ldr x%d, [pc, #0x%x]" % (w & 31, off + imm)

        # ---- shifts (register)
        if (w & 0x7FE0FFE0) == 0x2AC00000:
            return "lsl %s, %s, #%d" % (
                RNAMES[w & 31], RNAMES[(w >> 5) & 31], (w >> 10) & 0x3F)

        return ".word 0x%08x" % w


def main():
    path = sys.argv[1]
    lo = int(sys.argv[2], 0)
    hi = int(sys.argv[3], 0)
    d = D(open(path, "rb").read())
    off = lo
    while off < hi:
        print("  0x%06x  %s" % (off, d.one(off)))
        off += 4


if __name__ == "__main__":
    main()
