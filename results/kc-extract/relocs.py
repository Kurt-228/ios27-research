import struct
data = open('com_apple_iokit_IOSurface.macho','rb').read()
ncmds = struct.unpack_from('<I', data, 16)[0]
off = 32; segs=[]; symoff=stroff=nsyms=0; locreloff=locreloc=extreloff=nextrel=0; dysym=None
for i in range(ncmds):
    cmd, cs = struct.unpack_from('<II', data, off)
    if cmd == 0x19:
        vmaddr, vmsize, fileoff, filesize = struct.unpack_from('<QQQQ', data, off+24)
        segs.append((vmaddr, fileoff, filesize))
    elif cmd == 0x2:
        symoff, nsyms, stroff, strsize = struct.unpack_from('<IIII', data, off+8)
    elif cmd == 0xb:
        dysym = struct.unpack_from('<18I', data, off+8)
    off += cs
print('symtab', hex(symoff), nsyms, 'dysym loc', dysym and (hex(dysym[6]), dysym[7], hex(dysym[8]), dysym[9], hex(dysym[10]), dysym[11]))
