import struct, subprocess

data = open('com_apple_iokit_IOSurface.macho','rb').read()
# parse segments to map vmaddr->fileoff
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
    return None

syms = {}
out = subprocess.run(['nm','com_apple_iokit_IOSurface.macho'],capture_output=True,text=True).stdout
for line in out.splitlines():
    p = line.split()
    if len(p)==3 and p[1] in 'TtSs':
        try: syms[int(p[0],16)] = p[2]
        except: pass

def name(vm):
    if vm in syms: return syms[vm]
    best = max((a for a in syms if a <= vm), default=None)
    if best is not None and vm-best < 0x2000:
        return syms[best] + f'+0x{vm-best:x}'
    return f'0x{vm:x}'

def dump(vm, count, label):
    print(f'=== {label} @ 0x{vm:x}, {count} entries ===')
    fo = foff(vm)
    for i in range(count):
        func, = struct.unpack_from('<Q', data, fo + i*0x28)
        sci, ssi, sco, sso = struct.unpack_from('<IIII', data, fo + i*0x28+8)
        allowAsync = data[fo + i*0x28 + 0x18]
        ent, = struct.unpack_from('<Q', data, fo + i*0x28 + 0x20)
        es = ''
        if ent:
            efo = foff(ent)
            if efo: es = data[efo:efo+64].split(b'\0')[0].decode(errors='replace')
        def fmt(v): return 'VAR' if v == 0xffffffff else str(v)
        print(f'{i:3d}: {name(func):60s} sin={fmt(sci):3s} sinStruct={fmt(ssi):6s} sout={fmt(sco):3s} soutStruct={fmt(sso):6s} async={allowAsync} ent={es}')

dump(0xfffffe00088c1e38, 63, 'sMethodDescs')
dump(0xfffffe00088c2810, 63, 'sMethodDescsRestricted')
