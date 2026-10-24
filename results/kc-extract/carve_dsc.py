#!/usr/bin/env python3
"""Carve images out of a (split) dyld shared cache — bq21-механика для macOS/iOS.
Usage: carve_dsc.py <cache_dir> <name-substr> [<name-substr>...]
Вырезает полный файловый диапазон образа в CWD: <basename>.carve"""
import struct, sys, os

CACHE_DIR = sys.argv[1]
WANTS = sys.argv[2:] or ['QuartzCore']

files = sorted(os.path.join(CACHE_DIR, f) for f in os.listdir(CACHE_DIR)
               if f.startswith('dyld_shared_cache_') and '.symbols' not in f)

all_maps = []   # (addr, size, subpath, fileoff)
images = []     # (pathstr, addr)

for sf in files:
    try:
        f = open(sf, 'rb')
    except OSError:
        continue
    with f:
        f.seek(0)
        hdr = f.read(0x140)
        if len(hdr) < 0x40:
            continue
        magic = hdr[:16].rstrip(b'\0').decode('latin1', 'replace')
        if 'dyld' not in magic:
            continue
        moff, mcnt, ioff, icnt = struct.unpack_from('<4I', hdr, 16)
        size = os.path.getsize(sf)
        # маппинги
        if 0 < moff < size and 0 < mcnt <= 64:
            f.seek(moff)
            rec = f.read(0x28 * mcnt)
            if len(rec) == 0x28 * mcnt:
                for i in range(mcnt):
                    addr, sz, foff = struct.unpack_from('<QQQ', rec, i * 0x28)
                    if addr and sz:
                        all_maps.append((addr, sz, sf, foff))
        # таблица образов
        if 0 < ioff < size and 0 < icnt < 30000:
            f.seek(ioff)
            raw = f.read(icnt * 0x20)
            if len(raw) == icnt * 0x20:
                n_ok = 0
                for i in range(icnt):
                    addr, modtime, inode, pathoff, pad = struct.unpack_from('<QQQII', raw, i * 0x20)
                    if pathoff == 0 or pathoff >= size:
                        continue
                    f.seek(pathoff)
                    pb = f.read(600)
                    p = pb.split(b'\0')[0].decode('utf-8', 'replace')
                    if p and addr:
                        images.append((p, addr))
                        n_ok += 1
                if n_ok:
                    print(f"{os.path.basename(sf)}: images={n_ok}")

print(f"всего образов: {len(images)}, mappings: {len(all_maps)}")
if not images:
    sys.exit("нет таблиц образов")

def addr_to_file(addr):
    for (a, s, sf, fo) in all_maps:
        if a <= addr < a + s:
            return sf, fo + (addr - a)
    return None, None

order = sorted(images, key=lambda t: t[1])
for want in WANTS:
    hits = [(p, a) for (p, a) in order if want in p]
    print(f"'{want}': {len(hits)} хитов")
    if not hits:
        continue
    for p, a in hits[:3]:
        print(f"  {p} @ {a:#x}")
    p, a = hits[0]
    idx = order.index((p, a))
    end = order[idx + 1][1] if idx + 1 < len(order) else a + 0x4000000
    sf1, fo1 = addr_to_file(a)
    sf2, fo2 = addr_to_file(end - 1)
    if not sf1 or not sf2 or sf1 != sf2:
        # не паримся: вырезаем из sf1 максимально
        end_a = a + 0x2000000
        for (mx_a, mx_s, mx_sf, mx_fo) in all_maps:
            if mx_sf == sf1 and mx_a <= a < mx_a + mx_s:
                end_a = min(end_a, mx_a + mx_s)
        size = end_a - a
    else:
        size = fo2 - fo1
    size = min(size, 128 * 1024 * 1024)
    out = os.path.basename(p).replace('/', '_') + '.carve'
    with open(sf1, 'rb') as f:
        f.seek(fo1)
        data = f.read(size)
    open(out, 'wb').write(data)
    print(f"  carved {size} байт из {os.path.basename(sf1)}@{fo1:#x} -> {out}")
