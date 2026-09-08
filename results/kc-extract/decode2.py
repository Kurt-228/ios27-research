import struct, subprocess
data = open('com_apple_iokit_IOSurface.macho','rb').read()
ncmds = struct.unpack_from('<I', data, 16)[0]
off = 32; segs = []
for i in range(ncmds):
    cmd, cs = struct.unpack_from('<II', data, off)
    if cmd == 0x19:
        vmaddr, vmsize, fileoff, filesize = struct.unpack_from('<QQQQ', data, off+24)
        segs.append((vmaddr, fileoff, filesize))
    off += cs
def foff(vm):
    for vmaddr, fileoff, size in segs:
        if vmaddr <= vm < vmaddr+size: return fileoff + (vm-vmaddr)

syms = {}
out = subprocess.run(['nm','com_apple_iokit_IOSurface.macho'],capture_output=True,text=True).stdout
for line in out.splitlines():
    p = line.split()
    if len(p)==3 and p[1] in 'TtSs':
        try: syms[int(p[0],16)] = p[2]
        except: pass

K1 = 0x7faf42ad0901c000
K2 = 0x7faf42ad09004000
for tbl, vm in (('sMethodDescs',0xfffffe00088c1e38),('sMethodDescsRestricted',0xfffffe00088c2810)):
    print(f'=== {tbl} ===')
    fo = foff(vm)
    for i in range(63):
        raw, = struct.unpack_from('<Q', data, fo+i*0x28)
        func = raw ^ (K1 if i < 10 else K2)
        hit = 'OK ' if func in syms else '???'
        sci, ssi, sco, sso = struct.unpack_from('<IIII', data, fo+i*0x28+8)
        aa = data[fo+i*0x28+0x18]
        f=lambda v:'VAR' if v==0xffffffff else str(v)
        print(f'{i:3d} {hit} {syms.get(func, hex(func)):62s} sin={f(sci):3s} sinS={f(ssi):5s} sout={f(sco):3s} soutS={f(sso):5s} as={aa}')
