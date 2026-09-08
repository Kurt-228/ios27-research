import struct
data = open('com_apple_iokit_IOSurface.macho','rb').read()
ncmds = struct.unpack_from('<I', data, 16)[0]
off = 32
segs = []
for i in range(ncmds):
    cmd, cs = struct.unpack_from('<II', data, off)
    if cmd == 0x19:
        vmaddr, vmsize, fileoff, filesize = struct.unpack_from('<QQQQ', data, off+24)
        segs.append((vmaddr, fileoff, filesize))
    off += cs
def foff(vm):
    for vmaddr, fileoff, size in segs:
        if vmaddr <= vm < vmaddr+size:
            return fileoff + (vm-vmaddr)
fo = foff(0xfffffe00088c1e38)
for i in range(4):
    print(data[fo+i*0x28:fo+i*0x28+0x28].hex())
