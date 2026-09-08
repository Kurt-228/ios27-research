#!/usr/bin/env python3
"""Generic vmaddr->fileoff resolver + reader for a carved kext Mach-O."""
import struct, sys

PATH = sys.argv[1]
data = open(PATH, 'rb').read()
magic = struct.unpack_from('<I', data, 0)[0]
assert magic == 0xFEEDFACF
ncmds, sizeofcmds = struct.unpack_from('<II', data, 16)
segs = []
off = 32
for i in range(ncmds):
    cmd, cmdsize = struct.unpack_from('<II', data, off)
    if cmd == 0x19:
        vmaddr, vmsize, fileoff, filesize = struct.unpack_from('<QQQQ', data, off + 24)
        segs.append((vmaddr, vmsize, fileoff, filesize))
    off += cmdsize

def foff(vm):
    for v, vs, f, fs in segs:
        if v <= vm < v + vs:
            return f + (vm - v)
    return None

if len(sys.argv) >= 4 and sys.argv[2] == 'hex':
    vm = int(sys.argv[3], 16)
    n = int(sys.argv[4], 0)
    o = foff(vm)
    print(' '.join(f'{b:02x}' for b in data[o:o + n]))
else:
    for a in sys.argv[2:]:
        vm = int(a, 16)
        o = foff(vm)
        if o is None:
            print(a, 'OUT OF RANGE')
            continue
        end = data.index(b'\x00', o)
        print(hex(vm), repr(data[o:end].decode('utf-8', 'replace')))
