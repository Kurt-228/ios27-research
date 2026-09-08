#!/usr/bin/env python3
"""Read a C string at a vmaddr from the kernel collection (IOGPUFamily __TEXT)."""
import struct, sys

data = open('BootKernelCollection.kc', 'rb').read()
# segments: (vmaddr, fileoff) from IOGPUFamily entry dump
TEXT = (0xfffffe0007a86af0, 11021040)
TEXTEXEC = (0xfffffe000ab80410, 62374928)
DATA = (0xfffffe000cbe3b88, 96336776)
DATACONST = (0xfffffe00085473c0, 22295488)
LINKEDIT = (0xfffffe000cd94000, 98107392)

def off(vm):
    for v, f in (TEXT, TEXTEXEC, DATA, DATACONST, LINKEDIT):
        if v <= vm < v + 0x3000000:
            return f + (vm - v)
    return None

for vm in [int(a, 16) for a in sys.argv[1:]]:
    o = off(vm)
    if o is None:
        print(hex(vm), 'OUT OF RANGE')
        continue
    end = data.index(b'\x00', o)
    print(hex(vm), repr(data[o:end].decode('utf-8', 'replace')))
