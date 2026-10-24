#!/usr/bin/env python3
"""xref_raw.py — v177-механика для raw-карва: adrp+add/addimm → целевой VM-адрес.
Usage: xref_raw.py <carve.bin> <base_vmaddr_hex> <target_vmaddr_hex>
Печатает все (adrp_addr, add_addr, next_instr) пары, где adrp+add == target,
и окно функций вокруг (по 12 инструкций)."""
import sys, re
import capstone

carve = sys.argv[1]
base = int(sys.argv[2], 16)
target = int(sys.argv[3], 16)

data = open(carve, 'rb').read()
# mach_header_64: magic(4) cputype(4) cpusubtype(4) filetype(4) ncmds(4) sizeofcmds(4) flags(4) reserved(4) = 0x20
import struct
assert data[:4] == b'\xcf\xfa\xed\xfe', "не MH_64"
ncmds, sizeofcmds = struct.unpack_from('<II', data, 16)
hdr_end = 0x20 + sizeofcmds
print(f"ncmds={ncmds} sizeofcmds={sizeofcmds} → текст с {hdr_end:#x}")
md = capstone.Cs(capstone.CS_ARCH_ARM64, capstone.CS_MODE_LITTLE_ENDIAN)
md.detail = False
md.skipdata = True

insns = list(md.disasm(data[hdr_end:], base + hdr_end))
print(f"инструкций: {len(insns)}")

addr_to_idx = {ins.address: i for i, ins in enumerate(insns)}
def idx_of(insns, a):
    return addr_to_idx.get(a, 0)
hits = []
prev_adrp = {}
for idx, ins in enumerate(insns):
    if ins.mnemonic == 'adrp':
        m = re.match(r'x(\d+),\s*(0x[0-9a-f]+)', ins.op_str)
        if m:
            prev_adrp[int(m.group(1))] = (int(m.group(2), 16), ins.address, idx)
    elif ins.mnemonic == 'add':
        m = re.match(r'x(\d+),\s*x(\d+),\s*#(0x[0-9a-f]+|\d+)', ins.op_str)
        if m:
            rd, rn, imm = int(m.group(1)), int(m.group(2)), int(m.group(3), 0)
            if rn in prev_adrp:
                page, adrp_addr, adrp_idx = prev_adrp[rn]
                if page + imm == target:
                    hits.append((adrp_addr, ins.address))
    elif ins.mnemonic in ('ldr',):
        # ldr xN, [xM, #imm] после adrp — тоже учитываем
        m = re.match(r'(w|x)(\d+),\s*\[x(\d+),\s*#(0x[0-9a-f]+|\d+)\]', ins.op_str)
        if m:
            rn = int(m.group(3)); imm = int(m.group(4), 0)
            if rn in prev_adrp:
                page, adrp_addr, adrp_idx = prev_adrp[rn]
                if page + imm == target:
                    hits.append((adrp_addr, ins.address))

print(f"xref-ов на {target:#x}: {len(hits)}")
for a, b in hits[:20]:
    print(f"  adrp@{a:#x} → add/ldr@{b:#x}")
    lo = max(0, idx_of(insns, a) - 40)
    ai = idx_of(insns, a)
    for ins2 in insns[lo:ai + 5]:
        print(f"    {ins2.address:#x}: {ins2.mnemonic} {ins2.op_str}")
    print()
