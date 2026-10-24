#!/usr/bin/env python3
"""mk_text_macho.py — карв → валидный тонкий Mach-O (только __TEXT).
Переписывает load commands карва: единственный LC_SEGMENT_64 __TEXT с
fileoff=0 (контент карва = VA-contiguous от mach_header), секции __TEXT
с пересчитанными offsets. Остальные сегменты выкидываются.
Usage: mk_text_macho.py <carve.bin> <out.macho>"""
import struct, sys

src, out = sys.argv[1], sys.argv[2]
d = bytearray(open(src, 'rb').read())
magic, cputype, cpusub, ftype, ncmds, sizeofcmds, flags = struct.unpack_from('<7I', d, 0)
assert magic == 0xFEEDFACF, hex(magic)
BASE = None
# найдём __TEXT сегмент
off = 32
text_seg = None
for i in range(ncmds):
    cmd, cmdsize = struct.unpack_from('<II', d, off)
    if cmd == 0x19:
        name = bytes(d[off+8:off+24]).rstrip(b'\0')
        if name == b'__TEXT':
            vmaddr, vmsize, fileoff, filesize = struct.unpack_from('<4Q', d, off+24)
            maxprot, initprot, nsects, sflags = struct.unpack_from('<4I', d, off+56)
            BASE = vmaddr
            text_seg = (off, cmdsize, vmaddr, vmsize, nsects)
    off += cmdsize
assert text_seg, "__TEXT не найден"
seg_off, seg_size, vmaddr, vmsize, nsects = text_seg
print(f"__TEXT vm={vmaddr:#x}+{vmsize:#x}, {nsects} секций, BASE={BASE:#x}")

# новый LC_SEGMENT_64: fileoff=0, filesize=len(d)
struct.pack_into('<Q', d, seg_off+24+16, 0)            # fileoff = 0
struct.pack_into('<Q', d, seg_off+24+24, min(vmsize, len(d)))  # filesize

# секции: offset = addr - BASE (клампим)
soff = seg_off + 72
for s in range(nsects):
    sect_addr, sect_size, sect_off = struct.unpack_from('<QQQ', d, soff+32)
    new_off = sect_addr - BASE
    if new_off < 0 or new_off + sect_size > len(d):
        new_off = 0
        sect_size = 0
        struct.pack_into('<QQ', d, soff+32, sect_addr, sect_size)
    struct.pack_into('<Q', d, soff+48, new_off)
    name = bytes(d[soff:soff+16]).rstrip(b'\0').decode()
    print(f"  секция {name}: addr={sect_addr:#x} off→{new_off:#x} size={sect_size:#x}")
    soff += 80

# ncmds = 1, sizeofcmds = seg_size
struct.pack_into('<II', d, 16, 1, seg_size)
# отрезаем лишнее после load commands? — оставляем (контент!)
open(out, 'wb').write(bytes(d))
print(f"готово: {out} ({len(d)} байт), ncmds=1 (__TEXT only)")
