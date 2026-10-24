#!/usr/bin/env python3
"""Carve images from a modern macOS split dyld cache using the .map text manifest.
Usage: carve_dsc_map.py <cache_dir> <name-substr>...
Выход: <basename>.carve (сегменты подряд) + печать сегментов."""
import struct, sys, os, re

CACHE_DIR = sys.argv[1]
WANTS = sys.argv[2:] or ['QuartzCore.framework']

mapfile = os.path.join(CACHE_DIR, 'dyld_shared_cache_arm64e.map')
text = open(mapfile).read()

# 1. VM ranges: собрать маппинги всех сабфайлов (их собственные хедеры)
subs = {}
for fn in sorted(os.listdir(CACHE_DIR)):
    if not fn.startswith('dyld_shared_cache_arm64e.') or fn.endswith(('.map', '.atlas', '.symbols')):
        continue
    p = os.path.join(CACHE_DIR, fn)
    with open(p, 'rb') as f:
        hdr = f.read(0x100)
    moff, mcnt = struct.unpack_from('<II', hdr, 16)
    if not (0 < moff < os.path.getsize(p) and 0 < mcnt <= 64):
        continue
    with open(p, 'rb') as f:
        f.seek(moff)
        rec = f.read(0x28 * mcnt)
    maps = []
    for i in range(mcnt):
        addr, sz, foff = struct.unpack_from('<QQQ', rec, i * 0x28)
        if addr and sz:
            maps.append((addr, sz, foff))
    subs[fn] = maps

def vm_to_file(addr):
    for fn, maps in subs.items():
        for (a, s, fo) in maps:
            if a <= addr < a + s:
                return os.path.join(CACHE_DIR, fn), fo + (addr - a)
    return None, None

# 2. Парс .map: образ → сегменты
images = {}
cur = None
for line in text.splitlines():
    if line.startswith('/'):
        cur = line
        images.setdefault(cur, [])
    else:
        m = re.match(r'\s*(\S+)\s+(0x[0-9A-Fa-f]+)\s+->\s+(0x[0-9A-Fa-f]+)', line)
        if m and cur:
            seg, a1, a2 = m.group(1), int(m.group(2), 16), int(m.group(3), 16)
            if a2 > a1 and (a2 - a1) < 0x4000000:
                images[cur].append((seg, a1, a2))

print(f"образов в .map: {len(images)}")
for want in WANTS:
    hits = [p for p in images if want in p]
    print(f"'{want}': {len(hits)}")
    for p in hits[:3]:
        segs = images[p]
        total = sum(a2 - a1 for _, a1, a2 in segs)
        print(f"  {p}: {len(segs)} сегментов, {total>>10} KB: " +
              ", ".join(f"{s}({(a2-a1)>>10}K)" for s, a1, a2 in segs[:8]))
        out = os.path.basename(p).replace('/', '_') + '.carve'
        with open(out, 'wb') as o:
            for seg, a1, a2 in segs:
                sf, fo = vm_to_file(a1)
                if not sf:
                    print(f"    {seg} @ {a1:#x}: НЕ В MAPPINGS — пропуск")
                    continue
                with open(sf, 'rb') as f:
                    f.seek(fo)
                    data = f.read(a2 - a1)
                o.write(data)
                print(f"    {seg}: {a2-a1} байт из {os.path.basename(sf)}@{fo:#x}")
        print(f"  -> {out} ({os.path.getsize(out)>>10} KB)")
