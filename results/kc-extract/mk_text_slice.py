#!/usr/bin/env python3
"""mk_text_slice.py — вырезает VA-диапазон из плоского iOS kernelcache в
самостоятельный Mach-O, который llvm-objdump -d умеет дезассемблировать.

Зачем: load commands этого kernelcache несут сегменты БЕЗ section-записей
для __TEXT_EXEC (objdump -h показывает лишь __text/__info), поэтому
objdump -d на исходном файле печатает пусто и -start-address не помогает.
Сегменты при этом корректны — VA-модель плоская, что подтверждено
xref_maestro.py (§189: точные adrp+add пары для строк ip-парсера).

Использование:
    mk_text_slice.py <kernelcache> <va_start_hex> <va_end_hex> <out.macho>
"""
import struct
import sys

kc, va_start, va_end, out = (sys.argv[1], int(sys.argv[2], 16),
                             int(sys.argv[3], 16), sys.argv[4])
d = open(kc, "rb").read()
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

fo_start = None
for nm, va, vs, f, fs in segs:
    if va <= va_start < va + vs:
        fo_start = f + (va_start - va)
        break
if fo_start is None:
    sys.exit("va_start 0x%x не входит ни в один сегмент" % va_start)
n = va_end - va_start
blob = d[fo_start:fo_start + n]
if len(blob) != n:
    sys.exit("срез обрезан: %d < %d" % (len(blob), n))

FILEOFF = 0x1000
LC = 72 + 80                      # LC_SEGMENT_64 + 1 section_64
buf = bytearray(FILEOFF + n)
# mach_header_64
struct.pack_into("<8I", buf, 0, 0xFEEDFACF, 0x0100000C, 0, 2, 1, LC, 0x200000, 0)
# LC_SEGMENT_64 __TEXT
struct.pack_into("<II", buf, 32, 0x19, LC)
buf[40:56] = b"__TEXT".ljust(16, b"\x00")
vmsize = (n + 0xFFF) & ~0xFFF
struct.pack_into("<QQQQ", buf, 56, va_start, vmsize, FILEOFF, n)
struct.pack_into("<IIII", buf, 88, 5, 5, 1, 0)
# section_64 __TEXT,__text
buf[104:120] = b"__text".ljust(16, b"\x00")
buf[120:136] = b"__TEXT".ljust(16, b"\x00")
struct.pack_into("<QQ", buf, 136, va_start, n)
struct.pack_into("<IIII", buf, 152, FILEOFF, 0, 0, 0)
struct.pack_into("<IIII", buf, 168, 0x80000400, 0, 0, 0)
buf[FILEOFF:FILEOFF + n] = blob
open(out, "wb").write(bytes(buf))
print("wrote %s: va 0x%x..0x%x (%d bytes)" % (out, va_start, va_end, n))
