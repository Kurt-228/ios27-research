#!/usr/bin/env python3
"""Carve LC_FILESET_ENTRY entries out of a kernel collection, rebasing segment
file offsets so each kext becomes a standalone valid Mach-O."""
import struct, sys

KC = sys.argv[1]
WANT = set(sys.argv[2:])

data = open(KC, 'rb').read()
magic, cputype, cpusubtype, filetype, ncmds, sizeofcmds, flags, reserved = struct.unpack_from('<8I', data, 0)
assert magic == 0xFEEDFACF
off = 32
entries = []
for i in range(ncmds):
    cmd, cmdsize = struct.unpack_from('<II', data, off)
    if cmd == 0x80000035:
        vmaddr, fileoff = struct.unpack_from('<QQ', data, off + 8)
        stroff = struct.unpack_from('<I', data, off + 24)[0]
        name = data[off + stroff:off + cmdsize].split(b'\x00')[0].decode()
        entries.append((fileoff, vmaddr, name))
    off += cmdsize

def rebase_off(old, segs):
    for oldstart, size, newstart in segs:
        if oldstart <= old < oldstart + size:
            return newstart + (old - oldstart)
    return old

for fileoff, vmaddr, name in entries:
    if WANT and name not in WANT:
        continue
    hdr = data[fileoff:fileoff + 32]
    ncmds2, sizeofcmds2 = struct.unpack_from('<II', hdr, 16)
    cmds = []
    so = fileoff + 32
    for j in range(ncmds2):
        c2, cs2 = struct.unpack_from('<II', data, so)
        cmds.append((so, c2, cs2))
        so += cs2
    newhdr_size = 32 + sizeofcmds2
    out = bytearray(hdr + data[fileoff + 32:fileoff + 32 + sizeofcmds2])
    cursor = newhdr_size
    segs = []
    for so, c2, cs2 in cmds:
        rel = so - fileoff
        if c2 == 0x19:  # LC_SEGMENT_64
            seg_fileoff = struct.unpack_from('<Q', data, so + 40)[0]
            filesize = struct.unpack_from('<Q', data, so + 48)[0]
            nsects = struct.unpack_from('<I', data, so + 64)[0]
            newbase = cursor
            if filesize:
                struct.pack_into('<Q', out, rel + 40, cursor)
                segs.append((seg_fileoff, filesize, cursor))
                cursor += filesize
            for s in range(nsects):
                srel = rel + 72 + s * 80
                sec_off = struct.unpack_from('<I', data, so + 72 + s * 80 + 48)[0]
                struct.pack_into('<I', out, srel + 48, rebase_off(sec_off, segs))
            if filesize:
                out += data[seg_fileoff:seg_fileoff + filesize]
        elif c2 == 0x2:  # LC_SYMTAB
            symoff, nsyms, stroff, strsize = struct.unpack_from('<IIII', data, so + 8)
            struct.pack_into('<IIII', out, rel + 8,
                             rebase_off(symoff, segs), nsyms,
                             rebase_off(stroff, segs), strsize)
        elif c2 == 0xb:  # LC_DYSYMTAB (18 x uint32)
            vals = list(struct.unpack_from('<18I', data, so + 8))
            for k in (6, 8, 10, 12, 14, 16):  # offset fields
                vals[k] = rebase_off(vals[k], segs)
            struct.pack_into('<18I', out, rel + 8, *vals)
        elif c2 == 0x26:  # LC_FUNCTION_STARTS / LC_DATA_IN_CODE style: dataoff, datasize
            dataoff, datasize = struct.unpack_from('<II', data, so + 8)
            struct.pack_into('<II', out, rel + 8, rebase_off(dataoff, segs), datasize)
    fname = name.replace('.', '_') + '.macho'
    open(fname, 'wb').write(bytes(out))
    print(f'{name}: wrote {fname} ({len(out)} bytes)')
