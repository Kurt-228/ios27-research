#!/usr/bin/env python3
"""fetch_ipsw_members.py — range-fetch named members out of the restore image.

Same mechanics as get_kc27.py steps 1-4 (HEAD -> EOCD -> central dir ->
local header -> inflate), but takes explicit member names, because the
extracted copies in results/securerom/ipsw/ went dataless (iCloud-evicted
APFS clones) and brctl cannot materialize them.

Usage: python3 relay/fetch_ipsw_members.py <outdir> <member-name> [more...]
"""
import os
import struct
import sys
import urllib.request
import zlib

URL = ("https://updates.cdn-apple.com/2026SpringSeed/fullrestores/140-58754/"
       "BE6FFC19-A20C-4D29-B0E0-EF9DF7CC0EBE/iPhone16,2_27.0_24A5390f_Restore.ipsw")
OUT = sys.argv[1]
wanted = set(sys.argv[2:])
TAIL = 256 * 1024
os.makedirs(OUT, exist_ok=True)
downloaded = 0


def fetch(start, end, desc=""):
    global downloaded
    req = urllib.request.Request(URL, headers={"Range": f"bytes={start}-{end}"})
    with urllib.request.urlopen(req, timeout=180) as r:
        data = r.read()
    downloaded += len(data)
    print(f"  [{desc}] {start}-{end}: {len(data)} B (total {downloaded/1e6:.1f} MB)",
          flush=True)
    return data


req = urllib.request.Request(URL, method="HEAD")
with urllib.request.urlopen(req, timeout=60) as r:
    total = int(r.headers["Content-Length"])
print(f"IPSW size {total}")

tail = fetch(max(0, total - TAIL), total - 1, "tail")
eocd = tail.rfind(b"PK\x05\x06")
assert eocd >= 0, "EOCD not found"
n_entries = struct.unpack_from("<H", tail, eocd + 10)[0]
cd_size = struct.unpack_from("<I", tail, eocd + 12)[0]
cd_off = struct.unpack_from("<I", tail, eocd + 16)[0]
if cd_off == 0xFFFFFFFF or n_entries == 0xFFFF:
    loc = tail.rfind(b"PK\x06\x07", 0, eocd)
    z64_off = struct.unpack_from("<Q", tail, loc + 8)[0]
    z64 = fetch(z64_off, z64_off + 96, "zip64")
    n_entries = struct.unpack_from("<Q", z64, 32)[0]
    cd_size = struct.unpack_from("<Q", z64, 40)[0]
    cd_off = struct.unpack_from("<Q", z64, 48)[0]
print(f"central dir {cd_off}+{cd_size}, {n_entries} entries")

cd = fetch(cd_off, cd_off + cd_size - 1, "central dir")
found = {}
pos = 0
while pos + 46 <= len(cd) and cd[pos:pos + 4] == b"PK\x01\x02":
    method = struct.unpack_from("<H", cd, pos + 10)[0]
    csize = struct.unpack_from("<I", cd, pos + 20)[0]
    usize = struct.unpack_from("<I", cd, pos + 24)[0]
    nlen, xlen, clen = struct.unpack_from("<HHH", cd, pos + 28)
    lho = struct.unpack_from("<I", cd, pos + 42)[0]
    name = cd[pos + 46:pos + 46 + nlen].decode("utf-8", "replace")
    if csize == 0xFFFFFFFF or usize == 0xFFFFFFFF or lho == 0xFFFFFFFF:
        # zip64 extra field
        extra = cd[pos + 46 + nlen:pos + 46 + nlen + xlen]
        epos = 0
        while epos + 4 <= len(extra):
            eid, esz = struct.unpack_from("<HH", extra, epos)
            if eid == 0x0001:
                p = epos + 4
                if usize == 0xFFFFFFFF:
                    usize = struct.unpack_from("<Q", extra, p)[0]; p += 8
                if csize == 0xFFFFFFFF:
                    csize = struct.unpack_from("<Q", extra, p)[0]; p += 8
                if lho == 0xFFFFFFFF:
                    lho = struct.unpack_from("<Q", extra, p)[0]; p += 8
                break
            epos += 4 + esz
    if name in wanted:
        found[name] = (method, csize, usize, lho)
    pos += 46 + nlen + xlen + clen

missing = wanted - set(found)
if missing:
    print("NOT FOUND:", sorted(missing))

for name, (method, csize, usize, lho) in found.items():
    hdr = fetch(lho, lho + 29, f"lho {os.path.basename(name)}")
    nlen, xlen = struct.unpack_from("<HH", hdr, 26)
    start = lho + 30 + nlen + xlen
    comp = fetch(start, start + csize - 1, f"data {os.path.basename(name)}")
    if method == 8:
        raw = zlib.decompress(comp, -15)
    else:
        raw = comp
    assert len(raw) == usize, (name, len(raw), usize)
    dst = os.path.join(OUT, os.path.basename(name))
    with open(dst, "wb") as f:
        f.write(raw)
    print(f"  OK {name} -> {dst} ({len(raw)} B)", flush=True)
print("done")
