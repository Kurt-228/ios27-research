#!/usr/bin/env python3
"""wrap_flat.py — оборачивает ПЛОСКИЙ VA-дамп образа dyld-кэша (v184 carve:
дамп начинается с base-адреса образа, offset = VA - base) в самостоятельный
Mach-O для xcrun objdump -d.

Отличие от mk_text_slice.py: сегментные load commands в дампе кэша несут
файловые офсеты КЭША (за границей дампа), поэтому трансляция VA->offset
делается тривиально: off = VA - base.

Использование:
    wrap_flat.py <dump> <base_va_hex> <out.macho>
"""
import struct
import sys

dump, base, out = sys.argv[1], int(sys.argv[2], 16), sys.argv[3]
d = open(dump, "rb").read()
n = len(d)

FILEOFF = 0x1000
LC = 72 + 80                      # LC_SEGMENT_64 + 1 section_64
buf = bytearray(FILEOFF + n)
# mach_header_64 (arm64e)
struct.pack_into("<8I", buf, 0, 0xFEEDFACF, 0x0100000C, 0, 2, 1, LC, 0x200000, 0)
# LC_SEGMENT_64 __TEXT (r-x покрывает весь дамп)
struct.pack_into("<II", buf, 32, 0x19, LC)
buf[40:56] = b"__TEXT".ljust(16, b"\x00")
vmsize = (n + 0xFFF) & ~0xFFF
struct.pack_into("<QQQQ", buf, 56, base, vmsize, FILEOFF, n)
struct.pack_into("<IIII", buf, 88, 7, 5, 1, 0)   # maxprot r-x, initprot r-x, 1 section
# section_64 __TEXT,__text
buf[104:120] = b"__text".ljust(16, b"\x00")
buf[120:136] = b"__TEXT".ljust(16, b"\x00")
struct.pack_into("<QQ", buf, 136, base, n)
struct.pack_into("<IIII", buf, 152, FILEOFF, 0, 0, 0)
struct.pack_into("<IIII", buf, 168, 0x80000400, 0, 0, 0)
buf[FILEOFF:FILEOFF + n] = d
open(out, "wb").write(bytes(buf))
print("wrote %s: base 0x%x, %d bytes text" % (out, base, n))
