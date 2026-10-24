#!/usr/bin/env python3
"""Disassemble m_sum16 from carved kernel __TEXT_EXEC blobs (capstone).

Modes:
  dis <bin> <vmaddr_base> <vma> <count>       — linear disasm at VMA
  find <bin> <vmaddr_base> <string_vma>       — find adrp+add pairs resolving
                                                to string_vma, print VMA+offs
"""
import struct
import sys

from capstone import Cs, CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN

def load(binpath):
    return open(binpath, "rb").read()

def disasm(blob, base, vma, count, skip=0):
    md = Cs(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN)
    off = vma - base + skip
    for i in md.disasm(blob[off:off + count * 4], vma + skip):
        print(f"{i.address:#018x}:  {i.bytes.hex():<10s} {i.mnemonic} {i.op_str}")

def find_refs(blob, base, string_vma, verbose=True):
    """Scan for adrp X, page + add X, X, #imm pairs resolving to string_vma."""
    hits = []
    n = len(blob) // 4
    for i in range(n - 1):
        w0 = struct.unpack_from("<I", blob, i * 4)[0]
        if (w0 & 0x9F000000) != 0x90000000:   # adrp
            continue
        w1 = struct.unpack_from("<I", blob, (i + 1) * 4)[0]
        if (w1 & 0xFFC00000) != 0x91000000:   # add imm
            continue
        # decode adrp
        rd0 = w0 & 0x1F
        immlo = (w0 >> 29) & 3
        immhi = (w0 >> 5) & 0x7FFFF
        imm = (immhi << 2) | immlo
        if imm & (1 << 20):
            imm -= 1 << 21
        page = ((base + i * 4) & ~0xFFF) + (imm << 12)
        # decode add Xd, Xn, #imm12 (LSL 0)
        rd1 = w1 & 0x1F
        rn1 = (w1 >> 5) & 0x1F
        imm12 = (w1 >> 10) & 0xFFF
        shift = (w1 >> 22) & 3
        if rd0 == rn1 == rd1 and shift == 0:
            tgt = page + imm12
            if tgt == string_vma:
                hits.append(base + i * 4)
                if verbose:
                    print(f"  adrp+add -> string @ instr {base + i*4:#x}")
    return hits

def main():
    mode = sys.argv[1]
    blob = load(sys.argv[2])
    base = int(sys.argv[3], 0)
    if mode == "dis":
        vma = int(sys.argv[4], 0)
        count = int(sys.argv[5], 0)
        skip = int(sys.argv[6], 0) if len(sys.argv) > 6 else 0
        disasm(blob, base, vma, count, skip)
    elif mode == "find":
        svma = int(sys.argv[4], 0)
        find_refs(blob, base, svma)
    else:
        print(__doc__)

if __name__ == "__main__":
    main()
